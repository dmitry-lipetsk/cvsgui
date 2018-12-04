// =================================================================================
//	WasteDemoApp.cp					©1999 Timothy Paustian All rights reserved.
// =================================================================================
//

#include <LGrowZone.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LString.h>
#include <LWindow.h>
#include <PP_Messages.h>
#include <UDesktop.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <UTextTraits.h>
#include <UDesktop.h>
#include <UWindows.h>
#include <UAppleEventsMgr.h>
#include <UStandardDialogs.h>

#ifndef __GESTALT__
#include <Gestalt.h>
#endif

#ifndef __TEXTSERVICES__
#include <TextServices.h>
#endif

#include "CTextDocument.h"
#include "WTextModel.h"
#include "WTextView.h"
#include "WTSMEventAttachment.h"
#include "NThisAppRegistry.h"

//Debugging stuff
#if WT_DEBUG == 1
	#include <LDebugMenuAttachment.h>
	#include <UDebugUtils.h>
#endif

#include <PP_DebugMacros.h>	//The ones I use are in PPlant.
#include "WasteDemoApp.h"


// =================================================================================
//		¥ Main Program
// =================================================================================

int
main()
{
	// Setup the throw and signal actions.
	SetDebugThrow_(debugAction_Alert );
	SetDebugSignal_(debugAction_Alert );

	// Initialize the heap. Parameter is number
	// of master handle blocks to allocate.
	InitializeHeap( 4 );
	
	// Initialize the MacOS toolbox.
	UQDGlobals::InitializeToolbox();

	// Install a GrowZone function to catch  low memory situations.
	// Parameter is the size of the memory reserve in bytes.
	new LGrowZone( 20000 );

	// Create the application object and run it.
	WasteDemoApp	theApp;
	theApp.Run();
	
	return 0;
}


const	SInt32	kAppVersion			=	0x01002001;
FSSpec	WasteDemoApp::sFileSpec;
bool	WasteDemoApp::sUsingTSM = false;
const	ResID	resFrontierMenuID	= 131;

// ---------------------------------------------------------------------------------
//		¥ WasteDempApp
// ---------------------------------------------------------------------------------

WasteDemoApp::WasteDemoApp()
{

	NThisAppRegistry::RegisterClassesUsed();
	//Used in apple events
	sFileSpec.vRefNum = -30;
}

// ---------------------------------------------------------------------------------
//		¥ ~WasteDemoApp
// ---------------------------------------------------------------------------------

WasteDemoApp::~WasteDemoApp()
{
	WERemoveTSMHandlers();
	
	#if WT_DEBUG	
	#endif
}


void
WasteDemoApp::Initialize()
{
	LApplication::Initialize();
	::FlushEvents( everyEvent, nil );
	
#if WT_DEBUG == 1

	// check the version
	{
		StResource versH('vers', 1);
		SInt32 theVers = *((SInt32*)(*versH));
		if ( kAppVersion != theVers ) {
			SignalPStr_("\pVersions don't match");
		}
	}		

	// build the debug menu
	
	LDebugMenuAttachment::InstallDebugMenu(this);

#endif	
	LMenuBar	*theMenuBar = LMenuBar::GetCurrentMenuBar();
	ValidateObject_(theMenuBar);

	LMenu *theFontMenu = theMenuBar->FetchMenu(132);
	ValidateSimpleObject_(theFontMenu);
	::AppendResMenu(theFontMenu->GetMacMenuH(), 'FONT' );
}

// ---------------------------------------------------------------------------------
//		¥ StartUp
// ---------------------------------------------------------------------------------

void
WasteDemoApp::StartUp()
{
	//Install the TSM handlers
	SInt32		response;
	
	//check for Text Services manager
	sUsingTSM = (::Gestalt(gestaltTSMgrVersion, &response) == noErr);
	
	//Tell the OS that we are TSM aware
	#if !TARGET_API_MAC_CARBON
	//we do not need this code for carbon applications
	if (sUsingTSM) 
		sUsingTSM = (::InitTSMAwareApplication() == noErr);
	//Finally add an attachment to filter events
	//and install the TSM handlers.
	if (sUsingTSM)
	{
		WTSMEventAttachment *eventAttachment;
		
		eventAttachment = new WTSMEventAttachment();
		ValidateObj_(eventAttachment);
		AddAttachment(eventAttachment, nil, true);
		ThrowIfOSErr_(WEInstallTSMHandlers());
	}
	#endif
	
	
	ObeyCommand( cmd_New, nil );
}

// ---------------------------------------------------------------------------------
//		¥ OpenDocument
// ---------------------------------------------------------------------------------

void
WasteDemoApp::OpenDocument(
	FSSpec	*inMacFSSpec )
{
	LDocument*	theDoc = LDocument::FindByFileSpec(*inMacFSSpec);
	
	if (theDoc != nil)
	{
		theDoc->MakeCurrent();
	}
	else
	{
		// Create a new document using the file spec.
		theDoc = new CTextDocument( this, inMacFSSpec );
		ValidateObj_(theDoc);
	}
}


// ---------------------------------------------------------------------------------
//		¥ MakeNewDocument
// ---------------------------------------------------------------------------------

LModelObject *
WasteDemoApp::MakeNewDocument()
{
	// Make a new empty document.
	CTextDocument *theDoc = new CTextDocument( this, nil );
	ValidateObj_(theDoc);
	return theDoc;
}


// ---------------------------------------------------------------------------------
//		¥ ChooseDocument
// ---------------------------------------------------------------------------------

void
WasteDemoApp::ChooseDocument()
{
	// Deactivate the desktop.
	UDesktop::Deactivate();

	FSSpec	returnSpec;
	bool openOK = PP_StandardDialogs::AskOpenOneFile('TEXT', returnSpec, kNavSelectAllReadableItem);

	// Activate the desktop.
	UDesktop::Activate();
	
	// Send an apple event to open the file.	
	if ( openOK ) SendAEOpenDoc( returnSpec );
}


// ---------------------------------------------------------------------------------
//		¥ PrintDocument
// ---------------------------------------------------------------------------------

void
WasteDemoApp::PrintDocument(
	FSSpec	*inMacFSSpec )
{
	// Create a new document using the file spec.
	CTextDocument	*theDocument = new CTextDocument( this, inMacFSSpec );
	ValidateObject_(theDocument);

	// Tell the document to print.
	theDocument->DoPrint();
	
	DisposeOf_(theDocument);
}

/********************************************************************************
	AppleEvent support
This is to support the open file event and references to documents by position
*********************************************************************************/
void
WasteDemoApp::HandleAppleEvent(
					const AppleEvent	&inAppleEvent,
					AppleEvent			&outAEReply,
					AEDesc				&outResult,
					long				inAENumber)
{
	switch (inAENumber) {
			
		case ae_OpenDoc:
		case ae_PrintDoc:
			//if sFileSpec is filled out then open using it
			//otherwise drop through to
			if(sFileSpec.vRefNum != -30){
				if (inAENumber == ae_OpenDoc) {
					OpenDocument(&sFileSpec);
				} else {
					PrintDocument(&sFileSpec);
				}
				sFileSpec.vRefNum = -30;
				break;
			} 
		default:
			LDocApplication::HandleAppleEvent(inAppleEvent, outAEReply,
								outResult, inAENumber);
			break;
	}
}

void
WasteDemoApp::GetSubModelByName(DescType		inModelID,
									Str255			inName,
									AEDesc			&outToken) const
{
	switch (inModelID) {
	
		case cFile: {
			AliasHandle		alias;
			Boolean			wasChanged;
			//extract an alias from the full path name that is in the inName parameter
			OSErr  anErr = ::NewAliasMinimalFromFullPath (inName[0], inName + 1, nil, nil, &alias);
			ThrowIfOSErr_(anErr);
			//generate an FSSpec from the alias as put in the class FSSpec variable
			anErr = ::ResolveAlias (nil, alias, &sFileSpec, &wasChanged);
			ThrowIfOSErr_(anErr);
			//This model object will handle the event since we have the FSSpec
			LModelObject *	theModel = DebugCast_(this, WasteDemoApp, LModelObject);
			PutInToken(theModel, outToken);
			break;
		}
			
		default:
			LDocApplication::GetSubModelByName(inModelID, inName, outToken);
			break;
	}
}

void
WasteDemoApp::GetSubModelByPosition(
							DescType		inModelID,
							SInt32			inPosition,
							AEDesc			&outToken) const
{
	switch (inModelID) {
	
		case cDocument:
			//first find the window ptr
			WindowPtr	macWindPtr = UWindows::FindNthWindow(inPosition);
			//if it's nil return an err
			if(macWindPtr == nil)
				ThrowOSErr_(errAENoSuchObject);
			//now grab the window object	
			LWindow	*theWindowObject = LWindow::FetchWindowObject(macWindPtr);
			ValidateObject_(theWindowObject);
			//get a reference to the text pane
			WTextView* theTextObject = FindPaneByID_(theWindowObject, 2, WTextView);
			ValidateObject_(theTextObject);
			WTextModel	*theModel = theTextObject->GetTextModel();
			ValidateObject_(theModel);
			//finally put the text object (which is what we want) into the token
			PutInToken(theModel, outToken);
			SInt32 tokenSize;
			#if TARGET_API_MAC_CARBON
			tokenSize = ::AEGetDescDataSize(&outToken);
			#else
			tokenSize = ::GetHandleSize(outToken.dataHandle);
			#endif
			break;
			
		default:
			LApplication::GetSubModelByPosition(inModelID, inPosition,
													outToken);
			break;
	}
}
