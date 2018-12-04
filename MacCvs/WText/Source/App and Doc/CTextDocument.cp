// =================================================================================
//	CTextDocument.cp				©1996-2000 Metrowerks Inc. All rights reserved.
// =================================================================================
//	A subclass of LSingleDoc for basic management of the document.
//
//	Note that once the file is created, both data and resource fork are opened
//	with exclusive read/write access. Furthermore, the forks are not closed
//	until they need to be (e.g. document destruction). This not only adds
//	some protection from deleting the file in the Finder, others modifying
//	the file, etc., but it is also necessary for Navigation Services' use
//	of the Translation Manager (see the dtor). Please keep this need/assumption
//	in mind as you work with this class.

#include "CTextDocument.h"

#include <LFile.h>
#include <LPlaceHolder.h>
#include <LPrintout.h>
#include <LString.h>
#include <LWindow.h>
#include <PP_Messages.h>
#include <UMemoryMgr.h>
#include <UWindows.h>
#include <LCmdBevelButton.h>
#include <UModalDialogs.h>
#include <LEditText.h>
#include <PP_Resources.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LTabGroup.h>
#include <UResourceMgr.h>

#include "PrintingConstants.h"
#include "CTextDocument.h"
#include "WTextView.h"
#include "WTextModel.h"
#include "AppConstants.h"


#include <PP_DebugMacros.h>	//The ones I use are in PPlant

const	ResType	kIsOurFile = 'IOFL';
const	ResIDT	kIsOurFileID = 128;
const	OSType	kFileType		= 'TEXT';
const	OSType	kCreatorType	= 'WTxt';

PicHandle		CTextDocument::sFontPopupIcon = nil;
PicHandle		CTextDocument::sSizePopupIcon = nil;
PicHandle		CTextDocument::sStylePopupIcon = nil;
PicHandle		CTextDocument::sColorPopupIcon = nil;
PicHandle		CTextDocument::sLeftButtonIcon = nil;
PicHandle		CTextDocument::sCenterButtonIcon = nil;
PicHandle		CTextDocument::sRightButtonIcon = nil;
PicHandle		CTextDocument::sFullButtonIcon = nil;
// ---------------------------------------------------------------------------------
//		¥ CTextDocument(LCommander*, FSSpec*)
// ---------------------------------------------------------------------------------


CTextDocument::CTextDocument(
	LCommander	*inSuper,
	FSSpec		*inFileSpec )
		: LSingleDoc( inSuper )
{
	if(sFontPopupIcon == nil)
		InitializePopupIcons();
	// Create window for our document.
	mWindow = LWindow::CreateWindow( rPPob_TextWindow, this );
	ValidateObj_(mWindow);
	
	mFileDesignator = nil;
	//Point the buttons at their appropriate picture handles.
	LinkButtonToPicture('Font', sFontPopupIcon);
	LinkButtonToPicture('Size', sSizePopupIcon);
	LinkButtonToPicture('STyl', sStylePopupIcon);
	LinkButtonToPicture('Colr', sColorPopupIcon);
	LinkButtonToPicture('Left', sLeftButtonIcon);
	LinkButtonToPicture('Righ', sRightButtonIcon);
	LinkButtonToPicture('Cent', sCenterButtonIcon);
	LinkButtonToPicture('Just', sFullButtonIcon);
	
	// Specify that the text view should be the
	// target when the window is activated.
	mTextView =  FindPaneByID_(mWindow, kTextView, WTextView);
	ValidateObj_(mTextView);
	
	//create the text model
	LModelObject*	theModel = DebugCast_(this, CTextDocument, LModelObject);
	WTextModel	* theAppleScriptTextModel = new WTextModel(theModel, mTextView);
	ValidateObj_(theAppleScriptTextModel);
	mTextView->SetTextModel(theAppleScriptTextModel);
	
	mWindow->SetLatentSub( mTextView );
	//add an undoer to this view.
	
	Rect	stdBounds;
	//calculate in global coordinates where we are
	//we use this to preseve the window position and
	//not have them completely overlap
	Point 	presPoint = Point_00;
	mWindow->PortToGlobalPoint(presPoint);
	mWindow->CalcStandardBounds(stdBounds);
	SInt16	winWidth = stdBounds.right - stdBounds.left;
	winWidth = (winWidth > 500) ? 500 : winWidth;
	stdBounds.top = presPoint.v;
	stdBounds.left = presPoint.h;
	stdBounds.right = stdBounds.left + winWidth;
	mWindow->DoSetBounds(stdBounds);
	
	if ( inFileSpec == nil ) {

		// Set the name of new window.
		NameNewDoc();
		
	} else {
	
		// Display contents of file in window.
		StFeatureFlag noDraw(weFInhibitRedraw, weBitSet, mTextView->GetWEHandle());
		mIsNative = false;
		OpenFile( *inFileSpec );
	
	}
	
	theAppleScriptTextModel->SetSearchPointers(0, mTextView->GetTextLength());
		
	//Make sure we listen to the text engine to see if it gets modified
	mTextView->AddListener(this);
	
	//append the fonts to the font button in the window.
	LCmdBevelButton*	theFontButton = FindPaneByID_(mWindow, 'Font', LCmdBevelButton);
	
	::AppendResMenu(theFontButton->GetMacMenuH(), 'FONT' );
	mFontItemChecked = menu_NoItem;
	mSizeItemChecked = menu_NoItem;
	//intially the first item is checked
	//setting this to 1 will uncheck it when the first
	//FindCommandStatus is called, unless of course the
	//first menu item is selected
	mButtonFontChecked = 1;
	mButtonSizeChecked = 1;
	
	// Make the window visible.
	mWindow->Show();
	
	//clear the undo stack since we just opened the
	//document
	mTextView->ClearUndo();
}

Boolean
CTextDocument::IsModified()
{
	return mIsModified;
}

CTextDocument::~CTextDocument()
{
	try {
		TakeOffDuty();
		
		if ((mFile != nil) && (mFileDesignator != nil)) {
			mFile->CloseDataFork();
			mFile->CloseResourceFork();
			mFileDesignator->CompleteSave();
		}
	}
		
	catch (...) { }
	
	if (mFileDesignator != nil)
		delete mFileDesignator;
	
	TArray<LDocument*>	theList = LDocument::GetDocumentList();
	//this is the last document on the list.
	if(theList.GetCount() == 1)
	{
		::ReleaseResource((Handle)sFontPopupIcon);
		sFontPopupIcon = nil;
		ThrowIfResError_();
		::ReleaseResource((Handle)sSizePopupIcon);
		sSizePopupIcon = nil;
		ThrowIfResError_();
		::ReleaseResource((Handle)sStylePopupIcon);
		sStylePopupIcon = nil;
		ThrowIfResError_();
		::ReleaseResource((Handle)sColorPopupIcon);
		sColorPopupIcon = nil;
		ThrowIfResError_();
		::ReleaseResource((Handle)sLeftButtonIcon);
		sLeftButtonIcon = nil;
		ThrowIfResError_();
		::ReleaseResource((Handle)sCenterButtonIcon);
		sCenterButtonIcon = nil;
		ThrowIfResError_();
		::ReleaseResource((Handle)sRightButtonIcon);
		sRightButtonIcon = nil;
		ThrowIfResError_();
		::ReleaseResource((Handle)sFullButtonIcon);
		sFullButtonIcon = nil;
		ThrowIfResError_();
	}
}
// ---------------------------------------------------------------------------------
//		¥ InitializePopupIcons
// ---------------------------------------------------------------------------------
//
// Purpose: To initialize all the icons used in windows. They can be reused by all
// the windows in the class, but are not destroyed until the app shuts down.

void
CTextDocument::InitializePopupIcons()
{
	//make sure the app is the top resource
	StResourceContext	thisApp();
	sFontPopupIcon = ::GetPicture(133);
	sSizePopupIcon = ::GetPicture(134);
	sStylePopupIcon = ::GetPicture(135);
	sColorPopupIcon = ::GetPicture(137);
	sLeftButtonIcon = ::GetPicture(128);
	sCenterButtonIcon = ::GetPicture(129);
	sRightButtonIcon = ::GetPicture(130);
	sFullButtonIcon = ::GetPicture(131);
}

void
CTextDocument::LinkButtonToPicture(
						PaneIDT		inPaneID,
						PicHandle	inPicture)
{
	LCmdBevelButton* theButton = 
					FindPaneByID_(mWindow, inPaneID, LCmdBevelButton);
	ValidateObj_(theButton);
	ControlButtonContentInfo	butInfo;
	theButton->GetContentInfo(butInfo);
	butInfo.u.picture = inPicture;
	theButton->SetContentInfo(butInfo);
}
// ---------------------------------------------------------------------------------
//		¥ NameNewDoc
// ---------------------------------------------------------------------------------

void
CTextDocument::NameNewDoc()
{
								// Start with the default name ("untitled")
	LStr255		name(STRx_DefaultDocumentTitle, DefaultDocumentTitle_Untitled);
	
		// Check if an existing window has this name. If so,
		// append a count ("untitled 2"). Keep incrementing the
		// count until we find a unique name.
		
	long	num = 1;
	while (UWindows::FindNamedWindow(name) != nil) {
			
			// "untitled " (note trailing space)
		name.Assign(STRx_DefaultDocumentTitle, DefaultDocumentTitle_UntitledX);

		name += (++num);				// Increment and append count to name
	}		
	
	mWindow->SetDescriptor(name);		// Finally, set window title
}


// ---------------------------------------------------------------------------------
//	¥ OpenFile											[protected, virtual]
// ---------------------------------------------------------------------------------
//	Open the specified file

void
CTextDocument::OpenFile(
	FSSpec	&inFileSpec )
{
	// Create a new file object.
	mFile = new LFile( inFileSpec );
	ValidateObj_(mFile);
	SendDataToEngine();
	
	if(mIsNative)
		mFile->CloseResourceFork();
}

void
CTextDocument::SendDataToEngine()
{
	
	ValidateObj_(mFile);
	// Open the data fork.
	mFile->OpenDataFork( fsRdWrPerm );
	
	try
	{
		SInt16	resRefNum = mFile->OpenResourceFork(fsRdWrPerm);
		
		StResource	isOurFile(kIsOurFile, kIsOurFileID, false);
		if(isOurFile.mResourceH != nil)
			::BlockMoveData(*(isOurFile.Get()), &mIsNative, sizeof(bool));
	}
	catch(LException &	inErr)
	{
		mIsNative = false;
		//an end of file err means there is no
		//resrouce map. That is OK
		//other errors we want to throw
		if(inErr.GetErrorCode() != eofErr)
		{
			throw;
		}
	}
	
	// Read the entire file contents and close the file.
	Handle	theTextH = nil;
	theTextH = mFile->ReadDataFork();
	
	mFile->CloseDataFork();
	ValidateObj_(mTextView);
	mTextView->FocusDraw();
		
	try
	{
		if(mIsNative)
		{
			//note that the text handle is disposed of in
			//WTextView InsertFromNativeH. It is passed to an
			//LHandleStream. This assumes ownership and
			//deletes the handle.
			mTextView->InsertFromNativeH(theTextH);
		}
		else
		{
			SInt16	styleNum = ::Count1Resources('styl');
			StHandleBlock	theStyles;
			if(styleNum > 0)
			{	
				theStyles.Adopt(::Get1IndResource('styl', 1));
			}
			else
			{
				theStyles.Dispose();
			}
				
			StHandleLocker	lock(theTextH);
			SInt32		textSize = ::GetHandleSize(theTextH);
			mTextView->InsertText(*(theTextH), textSize, 
						(StScrpHandle)theStyles.Get());
		}
		mIsModified = false;
		mTextView->SetTextSelection(0,0);
		
		// Refresh the text view.
		mTextView->Refresh();
		// Set the window title to the name of the file and
		// flag that the document has an associated file.
		FSSpec theSpec;
		mFile->GetSpecifier(theSpec);
		mWindow->SetDescriptor( theSpec.name );
		mIsSpecified = true;
	} 
	catch (LException & inErr ) 
	{
		// Cleanup and rethrow the error.
		//
		if(theTextH != nil)
		{
			::DisposeHandle(theTextH);
			theTextH = nil;
		}
		if(inErr.GetErrorCode() == memFullErr)
		{
			if(::GetResource('ALRT', ALRT_LowMemory) != nil) 
			{
				::CautionAlert(ALRT_LowMemory, nil);
			}
			DisposeOf_(mFile);
		}
		else
			throw;

	}
	catch(OSErr	inErr)
	{
		// Cleanup and rethrow the error.
		//
		if(theTextH != nil)
		{
			::DisposeHandle(theTextH);
			theTextH = nil;
		}
		if(inErr == memFullErr)
		{
			if(::GetResource('ALRT', ALRT_LowMemory) != nil) 
			{
				::CautionAlert(ALRT_LowMemory, nil);
			}
			DisposeOf_(mFile);
		}
		else
			throw;
	}
}

// ---------------------------------------------------------------------------------
//		¥ DoAESave
// ---------------------------------------------------------------------------------

void
CTextDocument::DoAESave(
	FSSpec	&inFileSpec,
	OSType	inFileType )
{
	// Delete the existing file object.
	// Note: this does nothing to the actual file on disk.
	DisposeOf_(mFile);
	
	// Make a new file object.
	mFile = new LFile( inFileSpec );
	
	// Get the proper file type.
	OSType	theFileType = kFileType;
	if ( inFileType != fileType_Default ) theFileType = inFileType;

	// Make new file on disk (we'll use
	// SimpleText's creator for this example).
	mFile->CreateNewDataFile( kCreatorType, theFileType );
	
	// Write out the data.
	DoSave();

		// Change window title to reflect the new name.
	mWindow->SetDescriptor(inFileSpec.name);

		// Document now has a specified file.
	mIsSpecified = true;
}


// ---------------------------------------------------------------------------------
//	¥ DoSave											[public, virtual]
// ---------------------------------------------------------------------------------
//	Save the entire Document to its associated File, which must already exist

void
CTextDocument::DoSave()
{
	ValidateObj_(mFile);
	// Open the data fork.
	mFile->OpenDataFork( fsRdWrPerm );
	
	//try opening the resource fork
	//if this fails then create a resrouce fork
	try
	{
		mFile->OpenResourceFork(fsRdWrPerm);
	}
	catch(LException &	inErr)
	{
		if(inErr.GetErrorCode() == eofErr)
		{
			FSSpec	theSpec;
			mFile->GetSpecifier(theSpec);
			::FSpCreateResFile(&theSpec, kCreatorType, kFileType, smRoman);
			mFile->OpenResourceFork(fsRdWrPerm);
		}
		else
			throw;
	}

	StHandleBlock	isOurFile(1L);
	//once we call add resource, it is the responsibility of the
	//ResManager and we do not want to dispose of it
	isOurFile.Release();
	bool	youBetCha = true;
	StHandleLocker	lock(isOurFile);
	::BlockMoveData(&youBetCha, *(isOurFile.Get()), sizeof(youBetCha));
	::AddResource(isOurFile, kIsOurFile, kIsOurFileID, "\pIs our file");
	ThrowIfResError_();
	mFile->CloseResourceFork();
	
	//Check the pointer first
	ValidateObj_(mTextView);
	// Get the text from the text view.
	StHandleBlock	theTextH = mTextView->CopyIntoNativeH(0, mTextView->GetTextLength());
	// Lock the text handle.
	StHandleLocker	theLock( theTextH );
	
	// Write the text to the file.
	mFile->WriteDataFork(*theTextH.Get(), ::GetHandleSize(theTextH));

	// Close the data fork.
	mFile->CloseDataFork();

	// Saving makes doc un-dirty.
	mIsModified = false;
}


// ---------------------------------------------------------------------------------
//		¥ DoRevert
// ---------------------------------------------------------------------------------

void
CTextDocument::DoRevert()
{
	//create handles to store the data structures
	SendDataToEngine();
	mIsModified = false;
	// Refresh the text view.
	ValidateObj_(mTextView);
	mTextView->Refresh();
	//clear the undo stack since we just reverted
	mTextView->ClearUndo();
	
	//finally set the search pointers for applescripts to the length
	//of the text
	WTextModel *	theModel = mTextView->GetTextModel();
	if(theModel != nil)
		theModel->SetSearchPointers(0, mTextView->GetTextLength());
}


// ---------------------------------------------------------------------------------
//		¥ DoPrint
// ---------------------------------------------------------------------------------

void
CTextDocument::DoPrint()
{
		// Create the printout.
	StDeleter<LPrintout>	thePrintout(LPrintout::CreatePrintout(PPob_TextPrintout));
	ThrowIfNil_(thePrintout.Get());
	
		// Set the print record.
	thePrintout->SetPrintSpec(mPrintSpec);
	
		// Get the text placeholder.
	LPlaceHolder* thePlaceholder = dynamic_cast<LPlaceHolder*>
							(thePrintout->FindPaneByID(kTextPlaceholder));
	ThrowIfNil_(thePlaceholder);
	
		// Install the text view in the placeholder.
	thePlaceholder->InstallOccupant(mTextView, atNone);
	
	
		// Print.
	thePrintout->DoPrintJob();
	
	// Delete the printout (handled automatically by the
	// StDeleter object). The text view is returned
	// to the window when the placeholder is destroyed.
}


// ---------------------------------------------------------------------------
//	¥ AskSaveAs														  [public]
// ---------------------------------------------------------------------------
//	Ask the user to save a Document and give it a name
//
//	Returns false if the user cancels the operation

Boolean
CTextDocument::AskSaveAs(
	FSSpec&		outFSSpec,
	Boolean		inRecordIt)
{
	Boolean		saveOK = false;

	PP_StandardDialogs::LFileDesignator*	designator =
								new PP_StandardDialogs::LFileDesignator;
								
	designator->SetFileType( GetFileType() );
	
	Str255	defaultName;
	if (designator->AskDesignateFile(GetDescriptor(defaultName))) {
		
		designator->GetFileSpec(outFSSpec);
		
		if (designator->IsReplacing() && UsesFileSpec(outFSSpec)) {
									// User chose to replace the file with
									//   one of the same name. 
									
			if (inRecordIt) {		// This is the same as a save
				SendSelfAE(kAECoreSuite, kAESave, ExecuteAE_No);
			}
			
				// Same document is still open, so we don't need to complete
				// the save. However, user may have selected different
				// translation options, so we need to delete the original
				// designator and store the new one.
			
			delete mFileDesignator;
			mFileDesignator = designator;
				
			DoSave();				// Save data to disk
			saveOK = true;
			
		} else {
		
			if (inRecordIt) {		// Recording for AppleScript
				SendAESaveAs(outFSSpec, GetFileType(), ExecuteAE_No);
			}
			
			if (designator->IsReplacing()) {
				ThrowIfOSErr_(::FSpDelete(&outFSSpec));
			}
			
									// Save data to new file. This also
									//    closes the original file
			DoAESave(outFSSpec, fileType_Default);
			
			if (mFileDesignator != nil) {
									// Complete save of original designator
				mFileDesignator->CompleteSave();
				delete mFileDesignator;
			}
			
									// Store new designator so we can
									//   complete save when we close
									//   the document
			mFileDesignator = designator;
			
			saveOK = true;
		}
	}
	
	return saveOK;
}


// ---------------------------------------------------------------------------------
//	¥ SetPrintFrameSize									[protected, virtual]
// ---------------------------------------------------------------------------------

void
CTextDocument::SetPrintFrameSize()
{
	return;
}


// ---------------------------------------------------------------------------
//	¥ GetFileType										[public, virtual]
// ---------------------------------------------------------------------------
//	Return the type (four character code) of the file used for saving
//	the Document. Subclasses should override if they support saving files.

OSType
CTextDocument::GetFileType() const
{
	return ResType_Text;
}

// ---------------------------------------------------------------------------------
//		¥ ListenToMessage
// ---------------------------------------------------------------------------------
//
// Purpose: We are a listener to the text object. If it broadcasts a text changed 
// message, then we want to mark the doc as modified.

void
CTextDocument::ListenToMessage(
			MessageT	inMessage,
			void*		/*ioParam*/)
{
	if(inMessage == wTextChanged)
		mIsModified = true;
}

// ---------------------------------------------------------------------------
//	¥ ObeyCommand										[public, virtual]
// ---------------------------------------------------------------------------
//	Override provided here for convenience.

Boolean
CTextDocument::ObeyCommand(
				CommandT			inCommand,
				void *				ioParam)
{
	
	Boolean 		cmdHandled = true;
	ResIDT			menuID;
	SInt16			menuItem;
	
	TextStyle		theStyle;
	WEStyleMode	theMode = 0;
	Str255 	theFontName;
	SInt16	theFontNum;
	Str255 	theSizeText;
	SInt32 theNewSize;
	
	#if WASTE_VERSION > 0x02000000
	WERuler			theParaStyle;
	#endif
	
	ValidateObj_(mTextView);
								
	if (IsSyntheticCommand( inCommand, menuID, menuItem))
	{
					
		switch (menuID)
		{
			case main_MenuFont:
				LMenu	*theFontMenu =
					LMenuBar::GetCurrentMenuBar()->FetchMenu(main_MenuFont);
				ValidateSimpleObject_(theFontMenu);
			
				::GetMenuItemText( theFontMenu->GetMacMenuH(),
						menuItem, theFontName );
				::GetFNum(theFontName, &theFontNum);
				theStyle.tsFont = theFontNum;
				theMode = weDoFont;
				break;
	
			case main_MenuSize:
				LMenu	*theSizeMenu =
					LMenuBar::GetCurrentMenuBar()->FetchMenu(main_MenuSize);
				ValidateSimpleObject_(theSizeMenu);
				
				if(menuItem != 8)
				{
					::GetMenuItemText(theSizeMenu->GetMacMenuH(), menuItem, theSizeText);
					::StringToNum(theSizeText, &theNewSize);
					theStyle.tsSize = theNewSize;
					theMode = weDoSize;	
				}
				else
				{
					//we need to have an other size dialog box
				}
				break;
			default:
				cmdHandled = LSingleDoc::ObeyCommand(inCommand,ioParam);
				break;
		}
		if(theMode != 0)
			mTextView->SetStyle(theMode, theStyle);
		
	} else {
		bool doStyleChange = true;
		bool resendCommand = false;
		bool doPara = false;
		theMode = weDoFace + weDoToggleFace;
		
		switch (inCommand ) {
			case cmd_Plain:
				theStyle.tsFace = normal;
				theMode = weDoFace;
				break;
				
			case cmd_Bold:
				theStyle.tsFace = bold;
				break;
				
			case cmd_Italic:
				theStyle.tsFace = italic;
				break;
				
			case cmd_Underline:
				theStyle.tsFace = underline;
				break;
				
			case cmd_Outline:
				theStyle.tsFace = outline;
				break;
			
			case cmd_Shadow:
				theStyle.tsFace = shadow;
				break;
			
			case cmd_Condense:
				theStyle.tsFace = condense;
				break;
			
			case cmd_Extend:
				theStyle.tsFace = extend;
				break;
				
			case cmd_Black:
				theStyle.tsColor = RGBBlack;
				theMode = weDoColor;
				break;
				
			case cmd_Red:
				theStyle.tsColor = RGBRed;
				theMode = weDoColor;
				break;
			
			case cmd_Green:
				theStyle.tsColor = RGBGreen;			
				theMode = weDoColor;
				break;
				
			case cmd_Cyan:
				theStyle.tsColor = RGBCyan;			
				theMode = weDoColor;
				break;
			
			case cmd_Magenta:
				theStyle.tsColor = RGBMagenta;			
				theMode = weDoColor;
				break;
			
			case cmd_Blue:
				theStyle.tsColor = RGBBlue;		
				theMode = weDoColor;
				break;
				
			case cmd_Yellow:
			{	
				theStyle.tsColor = RGBYellow;
				theMode = weDoColor;
				break;
			}
			
			case cmd_Other:
			{
				//send up a dialog to grab some other number.
			}
			#if WASTE_VERSION > 0x02000000
			case cmd_JustifyDefault:
			{
				theParaStyle.alignment = weFlushDefault;
				theMode = weDoAlignment;
				doPara = true;
				break;
			}
			
			case cmd_JustifyLeft:
			{
				theParaStyle.alignment = weFlushLeft;
				theMode = weDoAlignment;
				doPara = true;
				break;
			}
				case cmd_JustifyRight:
			{
				theParaStyle.alignment = weFlushRight;
				theMode = weDoAlignment;
				doPara = true;
				break;
			}
				case cmd_JustifyFull:
			{
				theParaStyle.alignment = weJustify;
				theMode = weDoAlignment;
				doPara = true;
				break;
			}
			
			case cmd_JustifyCenter:
			{
				theParaStyle.alignment = weCenter;
				theMode = weDoAlignment;
				doPara = true;
				break;
			}
			#endif
			
			case cmd_ButtonFontMenu:
			{
				menuItem = *(SInt32*)ioParam;
				LCmdBevelButton* theFontButton = 
					FindPaneByID_(mWindow, 'Font', LCmdBevelButton);
				::GetMenuItemText( theFontButton->GetMacMenuH(),
						menuItem, theFontName );
				::GetFNum(theFontName, &theFontNum);
				theStyle.tsFont = theFontNum;
				theMode = weDoFont;
				break;
			}
			
			case cmd_ButtonSizeMenu:
			{
				menuItem = *(SInt32*)ioParam;
				if(menuItem != 8)
				{
					LCmdBevelButton* theSizeButton = 
						FindPaneByID_(mWindow, 'Size', LCmdBevelButton);
					::GetMenuItemText( theSizeButton->GetMacMenuH(),
							menuItem, theSizeText );
					::StringToNum(theSizeText, &theNewSize);
					theStyle.tsSize = theNewSize;
					theMode = weDoSize;
				}
				break;
			}
			case cmd_ButtonStyleMenu:
			{
				//warning, this only works if the menu on the button
				//is identical to the menu in the menubar. Change both
				//if you change one.
				menuID = main_MenuStyle;
				resendCommand = true;
				doStyleChange = false;
				break;	
			}
			case cmd_ButtonColorMenu:
			{
				menuID = main_MenuColor;
				resendCommand = true;
				doStyleChange = false;
				break;
			}
			default:
			{
				cmdHandled = LSingleDoc::ObeyCommand(inCommand,ioParam);
				doStyleChange = false;
				break;
			}
			
		}
		if(resendCommand)
		{
			LMenuBar*	theMenuBar = LMenuBar::GetCurrentMenuBar();
			ValidateObj_(theMenuBar);
			menuItem = *(SInt32*)ioParam;
			CommandT	theCommand = theMenuBar->FindCommand(menuID, menuItem);
			LCommander	*target = GetTarget();
			ValidateObject_(target);
			if(theCommand != cmd_Nothing && target != nil)
				target->ProcessCommand(theCommand);
		}
		if(doStyleChange)
		{
				mIsModified = true;
				#if WASTE_VERSION > 0x02000000

				if(doPara)
				{
					mTextView->AESetRuler(theMode, theParaStyle);
				}
				else
				#endif
					mTextView->AESetStyle(theMode, theStyle);
		}
	}
	return cmdHandled;
}

void
CTextDocument::FindCommandStatus (
								CommandT			inCommand,
								Boolean &			outEnabled,
								Boolean &			outUsesMark,
								UInt16 &			outMark,
								Str255				outName)
{
	outEnabled = false;
	
	outUsesMark = false;
	ResIDT		menuID;
	SInt16		menuItem;
	TextStyle	theStyle;
	//fill this with some flag values
	theStyle.tsFace = -1;
	theStyle.tsColor.red = -1;
	
	bool		checkTheStyle = false;
		
	if (IsSyntheticCommand(inCommand,menuID,menuItem))
	{
		switch (menuID)
		{
			case main_MenuFont:
			{	outEnabled=true;
				FindTheFontForMenu(main_MenuFont, mFontItemChecked);
				break;
			}
			case main_MenuSize:
			{
				outEnabled=true;
				FindTheSizeForMenu(main_MenuSize, mSizeItemChecked);
				break;
			}
			default:
				LCommander::FindCommandStatus(inCommand, outEnabled,
										outUsesMark, outMark, outName);
				break;
		}
	} else {
	switch (inCommand) {
		case cmd_Plain:
			theStyle.tsFace = normal;
			checkTheStyle = true;
			break;
		case cmd_Bold:
			theStyle.tsFace = bold;
			checkTheStyle = true;
			break;
		case cmd_Italic:
			theStyle.tsFace = italic;
			checkTheStyle = true;
			break;
		case cmd_Underline:
			theStyle.tsFace = underline;
			checkTheStyle = true;
			break;
		case cmd_Outline:
			theStyle.tsFace = outline;
			checkTheStyle = true;
			break;
		case cmd_Shadow:
			theStyle.tsFace = shadow;
			checkTheStyle = true;
			break;
		case cmd_Condense:
			theStyle.tsFace =condense;
			checkTheStyle = true;
			break;
		case cmd_Extend:
			theStyle.tsFace = extend;
			checkTheStyle = true;
			break;
		
		case cmd_Black:
			theStyle.tsColor = RGBBlack;
			checkTheStyle = true;
			break; 
		case cmd_Red:
			theStyle.tsColor = RGBRed;
			checkTheStyle = true;
			break; 
		
		case cmd_Green:
			theStyle.tsColor = RGBGreen;
			checkTheStyle = true;
			break; 
			
		case cmd_Cyan:
			theStyle.tsColor = RGBCyan;
			checkTheStyle = true;
			break; 
		
		case cmd_Magenta:
			theStyle.tsColor = RGBMagenta;
			checkTheStyle = true;
			break; 
		
		case cmd_Blue:
			theStyle.tsColor = RGBBlue;
			checkTheStyle = true;
			break; 
		
		case cmd_Yellow:
			theStyle.tsColor = RGBYellow;
			checkTheStyle = true;
			break; 
		
		case cmd_Other:
		{

		}
		case cmd_JustifyDefault:
			outEnabled = true;
			outUsesMark = true;
			if(CheckAlignment(weFlushDefault))
				outMark = checkMark;
			else
				outMark = noMark;
			break;
		case cmd_JustifyLeft:
			outEnabled = true;
			outUsesMark = true;
			if(CheckAlignment(weFlushLeft))
				outMark = checkMark;
			else
				outMark = noMark;
			break;
		case cmd_JustifyRight:
			outEnabled = true;
			outUsesMark = true;
			if(CheckAlignment(weFlushRight))
				outMark = checkMark;
			else
				outMark = noMark;
			break;
		case cmd_JustifyFull:
			outEnabled = true;
			outUsesMark = true;
			if(CheckAlignment(weJustify))
				outMark = checkMark;
			else
				outMark = noMark;
			break;
		case cmd_JustifyCenter:
			outEnabled = true;
			outUsesMark = true;
			if(CheckAlignment(weCenter))
				outMark = checkMark;
			else
				outMark = noMark;
			break;
		case cmd_HierFontMenu:
		case cmd_HierSizeMenu:
		case cmd_HierColorMenu:
		case cmd_HierStyleMenu:
			outEnabled = true;
			break;
		case cmd_HierAlignMenu:
			outEnabled = true;
			break;
		case cmd_ButtonColorMenu:
			DoColorMenu();
			outEnabled = true;
			break;
		case cmd_ButtonStyleMenu:
			DoStyleMenu();
			outEnabled = true;
			break;
		case cmd_ButtonFontMenu:
			
			FindTheFontForMenu(MENU_Font, mButtonFontChecked);
			outEnabled = true;
			break;
		
		case cmd_ButtonSizeMenu:
			FindTheSizeForMenu(MENU_Size, mButtonSizeChecked);
			outEnabled = true;
			break;
	default:
		LSingleDoc::FindCommandStatus(inCommand, outEnabled,
											outUsesMark, outMark, outName);
		break;
		}
	if(checkTheStyle != false)
		mTextView->StyleCheck(theStyle, outEnabled, outUsesMark, outMark);
	}
}

void
CTextDocument::FindTheFontForMenu(
						ResIDT		inMenuRes,
						SInt16	&	ioLastChecked)
{
	WEStyleMode	theMode = weDoFont;
	TextStyle	theStyle;
	ValidateObj_(mTextView);
	bool isTextStyle = mTextView->ContinuousStyle(theMode, theStyle);

	LStr255	textFont;
	::GetFontName(theStyle.tsFont, textFont);	
	
	DoMenuCheckMarks(inMenuRes, textFont, ioLastChecked, isTextStyle);
}

void
CTextDocument::FindTheSizeForMenu(
						ResIDT	inMenuRes,
						SInt16	&	ioLastChecked)
{
	WEStyleMode	theMode = weDoSize;
	TextStyle	theStyle;
	ValidateObj_(mTextView);
	bool isTextStyle = mTextView->ContinuousStyle(theMode, theStyle);
	LStr255	textSize;
	::NumToString(theStyle.tsSize, textSize);
	
	DoMenuCheckMarks(inMenuRes, textSize, ioLastChecked, isTextStyle);
}

//=================================================================
// ¥ DoMenuCheckMarks									[private]
//=================================================================
//
// Purpose: Reset the check marks on the menus. I found the SetItemMark
// was a really slow routine. Therefore I store the last item checked
// in a class variable to decrease the number of times it is called.
//
// Revision: 1/9/98 initial creation
//
//=================================================================	


void
CTextDocument::DoMenuCheckMarks(
					ResIDT		inMenuRes,
					LStr255 &	inCheckText,
					SInt16	&	ioLastMark,
					bool		inIsTextStyle)
{		
	MenuHandle theMenuH = nil;
	bool		doButton = false;
	LBevelButton	*theButton;
	switch(inMenuRes)
	{
		case MENU_Font:
		{
			LCmdBevelButton* theFontButton = 
					FindPaneByID_(mWindow, 'Font', LCmdBevelButton);
			theMenuH = theFontButton->GetMacMenuH();
			doButton = true;
			theButton = DebugCast_(theFontButton, LCmdBevelButton, LBevelButton);
			break;
		}
		case MENU_Size:
		{
			LCmdBevelButton* theSizeButton = 
					FindPaneByID_(mWindow, 'Size', LCmdBevelButton);
			theMenuH = theSizeButton->GetMacMenuH();
			doButton = true;
			theButton = DebugCast_(theSizeButton, LCmdBevelButton, LBevelButton);
			break;
		}
		case main_MenuFont:
		case main_MenuSize:
		{
			LMenu	*theMenu = LMenuBar::GetCurrentMenuBar()->FetchMenu(inMenuRes);
			ValidateSimpleObject_(theMenu);
			theMenuH = theMenu->GetMacMenuH();
			break;
		}
		default:
		{
			return;
			break;
		}
	}
	
	ThrowIfNil_(theMenuH);
	
	SInt16	menuSize = ::CountMenuItems(theMenuH);
	
	//add the other menus to this
	if(!inIsTextStyle)
	{
		if(ioLastMark != menu_NoItem)
		{
			::SetItemMark(theMenuH, ioLastMark, noMark);
		}
		ioLastMark = menu_NoItem;
		return;
	}
	
	bool	wasFound = false;
	LStr255	menuFont;
	//get rid of the last check mark.
	if(ioLastMark != menu_NoItem)
	{
		::SetItemMark(theMenuH, ioLastMark, noMark);
		ioLastMark = menu_NoItem;
	}
	//do a binary search for speed
	//we can do this because the menu items are arranged 
	//alphabetically.
	if(inMenuRes == main_MenuFont || inMenuRes == MENU_Font)
	{
		SInt16 menuMin = 1;
		SInt16 menuMax = menuSize;
		SInt16	currMenu;
		SInt16	comparison;
		while(menuMin <= menuMax)
		{
			currMenu = (menuMin + menuMax) >> 1;
			::GetMenuItemText(theMenuH, currMenu, menuFont);
			comparison = menuFont.CompareTo(inCheckText);
			if(comparison == 0)
			{
				if(doButton)
					theButton->SetCurrentMenuItem(currMenu);
				else
					::SetItemMark(theMenuH, currMenu, checkMark);
				
				ioLastMark = currMenu;
				break;
			}
			else if (comparison < 0)
			{
				menuMin = currMenu + 1;
			}
			else
			{
				menuMax = currMenu;
			}
			
		}
	}
	else
	{
		SInt16 i;
		for(i = 1; i++; i<=menuSize)
		{
			::GetMenuItemText(theMenuH, i, menuFont);
			if(menuFont == inCheckText)
			{
				wasFound = true;
				if(doButton)
					theButton->SetCurrentMenuItem(i);
				else
					::SetItemMark(theMenuH, i, checkMark);
					
				ioLastMark = i;
				break;
			}		
		}
		if(!wasFound && inMenuRes == main_MenuSize)
		{
			LStr255	otherText = "\pOther(";
			otherText += inCheckText;
			otherText += "\p)...";
			::SetMenuItemText(theMenuH, menuSize, otherText);
			::SetItemMark(theMenuH, menuSize, checkMark);
		}
	}
}

void
CTextDocument::DoColorMenu()
{
	WEStyleMode mode = weDoColor;
	TextStyle	ts;
	ValidateObj_(mTextView);
	bool isContinuous = mTextView->ContinuousStyle(mode, ts);
	LCmdBevelButton * theButton = FindPaneByID_(mWindow, 'Colr', LCmdBevelButton);
	
	
	if(isContinuous)
	{
		UInt16 red = ts.tsColor.red;
		UInt16 blue = ts.tsColor.blue;
		UInt16 green = ts.tsColor.green;
		//black
		if(red == 0 && blue == 0 && green == 0)
		{
			theButton->SetCurrentMenuItem(7);
			return;
		}
		//red
		if(red == 65535 && blue == 0 && green == 0)
		{
			theButton->SetCurrentMenuItem(1);
			return;
		}
		//yellow
		if(red == 65535 && blue == 65535 && green == 0)
		{
			theButton->SetCurrentMenuItem(4);
			return;
		}
		//blue
		if(red == 0 && blue == 65535 && green == 0)
		{
			theButton->SetCurrentMenuItem(3);
			return;
		}
		//green
		if(red == 0 && blue == 0 && green == 65535)
		{
			theButton->SetCurrentMenuItem(2);
			return;
		}
		//magenta
		if(red == 65535 && blue == 0 && green == 65535)
		{
			theButton->SetCurrentMenuItem(5);
			return;
		}
		//cyan
		if(red == 0 && blue == 65535 && green == 65535)
		{
			theButton->SetCurrentMenuItem(6);
			return;
		}
		
		theButton->SetCurrentMenuItem(8);
	}
	else
	{
		theButton->SetCurrentMenuItem(-1);
	}
		
}

//=================================================================
// ¥ CheckAlignment									[private]
//=================================================================
//
// Purpose: Figure out the check marks for the alignment menus
//
// Revision: 12/22/98 initial creation
//
//=================================================================	

bool
CTextDocument::CheckAlignment(
					WEAlignment	inAlignment)
{
	bool	retVal = false;
	
#if WASTE_VERSION > 0x02000000

	WERuler	theData;
	theData.alignment = inAlignment;
	SInt32	start, end;
	ValidateObj_(mTextView);
	
	mTextView->GetTextSelection(start, end);
	WERulerMode	theMode = weDoAlignment;
	bool isCont;
	
	isCont = mTextView->ContinuousRuler(theMode, theData);
	
	if(isCont)
	{
		//if it is continuous check the style to make sure it matches
		retVal = (theData.alignment == inAlignment);
	}
#else
#pragma unused(inAlignment)
#endif


	return retVal;
}

void
CTextDocument::DoStyleMenu()
{
	LCmdBevelButton* theStyleButton = 
					FindPaneByID_(mWindow, 'STyl', LCmdBevelButton);
	
	MenuHandle		macMenuH = theStyleButton->GetMacMenuH();
	ThrowIfNil_(macMenuH);
	
	TextStyle	theStyle;
	theStyle.tsFace = 0;
	WEStyleMode		theMode = weDoFace;
	SInt32			start, end;

	ValidateObj_(mTextView);
	mTextView->GetTextSelection(start, end);

	bool	isCont;
	
	isCont = mTextView->ContinuousStyle(theMode, theStyle);
	
	if(isCont)
	{
		
		if(theStyle.tsFace == normal)
		{
			::SetItemMark(macMenuH, 1, checkMark);
		}
		else
		{
			::SetItemMark(macMenuH, 1, noMark);
		}
		
		SInt16 styleMask = 0;
		SInt16 i;
		for (i = 0; i < 7; i++)
		{
			styleMask = (1 << i);
			if(theStyle.tsFace & styleMask)
			{
				::SetItemMark(macMenuH, i + 3, checkMark);
			}
			else
			{
				::SetItemMark(macMenuH, i + 3, noMark);
			}	
		}
	}
}