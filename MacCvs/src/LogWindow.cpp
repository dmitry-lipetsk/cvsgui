/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.

** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.

** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*
 * LogWindow.cpp --- log window of tha application
 */

const ResIDT	prto_PrintView		= 1000;
#define kStyleResourceID	128

#include "LogWindow.h"
#include "LCVSWasteEdit.h"
//#include "CWastePrintout.h"
#include "MacCvsApp.h"
#include "PythonGlue.h"

//#include <console.h>
#include <stdlib.h>
#include <UStandardDialogs.h>

static CLogWindow *sLogWindow = 0L;
char *CLogWindow::sbuf = 0L;
int CLogWindow::snumbuf = 0;


static pascal void _FlushCacheTimerFired(EventLoopTimerRef inTimer, void *inUserData)
{
  if ( inUserData ) {
    static_cast<CLogWindow*>(inUserData)->FlushCache();
  }
}
static EventLoopTimerUPP    sFlushCacheTimerUPP(NewEventLoopTimerUPP(&_FlushCacheTimerFired));


CLogWindow::CLogWindow(LStream *inStream) : LWindow(inStream), mAutoShow(false), mSelectOnError(false), mFlushCacheTimer(NULL), mFlushCacheTimerRunning(false)
{
#if 0
	mPrintRecordH = nil;
#endif
	mWE = nil;
	sLogWindow = this;
}

void CLogWindow::FinishCreateSelf()
{
	// adjust size (TODO : use persistent values ?)
	Rect	windowRect;
	CalcStandardBounds(windowRect);
	if((windowRect.right - windowRect.left) >= 560) 					//check to make sure we won't
		windowRect.right = windowRect.left + 560;
	if((windowRect.bottom - windowRect.top) >= 400) 					//check to make sure we won't
		windowRect.bottom = windowRect.top + 400;
		
	DoSetBounds(windowRect);

	WTextView *we = GetWE();
	SetLatentSub(we);
	
	Rect	frame;
	we->CalcPortFrameRect(frame);
	we->FocusDraw();
	EraseRect(&frame);
}

CLogWindow::~CLogWindow()
{
  if ( mFlushCacheTimer) RemoveEventLoopTimer(mFlushCacheTimer);
  
	CMacCvsApp::gApp->RemoveWindowFromListByView(this);
	snumbuf = 0;
	if(sbuf != 0L)
	{
		free(sbuf);
		sbuf = 0L;
	}
#if 0
	if (mPrintRecordH != nil)
		::DisposeHandle((Handle) mPrintRecordH);
#endif

	CPython_Interp::ClosePythonSupport();
}

WTextView *CLogWindow::FindWE(void)
{
	mWE = dynamic_cast<WTextView*>(FindPaneByID(CCvsWasteEdit::class_ID));
	ThrowIfNil_(mWE);
	return mWE;
}

void CLogWindow::DoSave(FSSpec & outFSSpec)
{
	WTextView *we = GetWE();

	LFile *mFile = NEW LFile(outFSSpec);				// ¥AST use LSingleDoc's LFile field
	ThrowIfNil_(mFile);
	
	mFile->CreateNewFile('ttxt', 'TEXT');	//create a new file with resource and data forks

	mFile->OpenDataFork(fsRdWrPerm);				//now open both of them for writing
#if 0
	SInt16 theResourceFileNum = mFile->OpenResourceFork(fsRdWrPerm);
	
	Handle theText = ::NewHandle(0);
	ThrowIfNil_(theText);

	Handle theStyles = ::NewHandle(0);
	ThrowIfNil_(theStyles);
	
	we->GetTextStyleSOUPData(theText, theStyles, 0L);
		
	SInt32 textSize = ::GetHandleSize(theText);
	SInt32 styleSize = ::GetHandleSize(theStyles);
	
	//write the text data
	{
		StHandleLocker lock(theText);
		SInt32 bytesWritten = mFile->WriteDataFork(*theText, textSize);
		ThrowIf_(bytesWritten != textSize);
	}
	
	//Prepare the resource fork for writing
	int	currentResourceFileNum = ::CurResFile();
	::UseResFile(theResourceFileNum);
	
	//add the style resource to the document
	if(styleSize != 0) {
		Handle  	styleHandle = ::Get1Resource('styl', kStyleResourceID);
		
		if(styleHandle != nil) { 
			//a resource exists, we want to replace it
			//get rid of the old resource
			::RemoveResource(styleHandle);
		}
		styleHandle = theStyles; 
		ThrowIfOSErr_(HandToHand(&styleHandle)); 
		::AddResource(styleHandle, 'styl', kStyleResourceID, nil);
		ThrowIfResError_();
		
		::UpdateResFile(theResourceFileNum);
		::ReleaseResource(styleHandle);
	}
		
	UseResFile(currentResourceFileNum);	
	
	if(theText != nil)
		DisposeHandle(theText);
	if(theStyles != nil)
		DisposeHandle(theStyles);

	mFile->CloseResourceFork();
#else
	StHandleBlock	theTextH = we->CopyIntoNativeH(0, we->GetTextLength());
	// Lock the text handle.
	StHandleLocker	theLock( theTextH );
	
	// Write the text to the file.
	mFile->WriteDataFork(*theTextH.Get(), ::GetHandleSize(theTextH));
#endif
	mFile->CloseDataFork();
	delete mFile;
}

Boolean CLogWindow::AskSaveAs(void)
{
	PP_StandardDialogs::LFileDesignator*	designator =
								NEW PP_StandardDialogs::LFileDesignator;
					
	designator->SetFileType(ResType_Text);
	
	Str255	defaultName;
	if (designator->AskDesignateFile(GetDescriptor(defaultName)))
	{	
		FSSpec outFSSpec;
		designator->GetFileSpec(outFSSpec);
		if(designator->IsReplacing())
			ThrowIfOSErr_(::FSpDelete(&outFSSpec));
		
		DoSave(outFSSpec);
		return true;
	}
	
	return false;
}

Boolean CLogWindow::ObeyCommand(CommandT inCommand, void *ioParam)
{
	switch(inCommand)
	{
	case cmd_SaveAs :
    this->FlushCache();
		AskSaveAs();
		break;
	case cmd_Print :
    this->FlushCache();
#if 0
		if (mPrintRecordH == nil) {
			mPrintRecordH = UPrintingMgr::CreatePrintRecord();
		}
		
		if (mPrintRecordH != nil) {
			UDesktop::Deactivate();
			Boolean	printIt = UPrintingMgr::AskPrintJob(mPrintRecordH);
			UDesktop::Activate();
			
			if (printIt)
				DoPrint();			
		} else {
			// ++ Couldn't allocate print record
			// ++ User probably has no printer selected in Chooser
		}
#endif
		break;
	case cmd_Close :
		Hide();
		break;
	default:
		// Call inherited.
		return LWindow::ObeyCommand(inCommand, ioParam);
	}
	return true;
}

void CLogWindow::DoPrint(void)
{
#if 0
	WTextView *we = GetWE();

	LongRect	viewRect;
	WEReference	hWE;
	Boolean		active;
	GrafPtr		oldPort;
	short		prevOutline, prevDrawScreen;
	//save old variables to be restored later
	hWE = we->GetWEHandle();
	
	WEGetViewRect(&viewRect, hWE);
	active = WEIsActive(hWE);
	WEGetInfo(wePort, &oldPort, hWE);	
	prevOutline = we->FeatureFlag(weFOutlineHilite, weBitClear);
	prevDrawScreen = we->FeatureFlag(weFDrawOffscreen, weBitClear);
	
	if(active){
		SetPort(oldPort);
		WEDeactivate(hWE);
	}
	
	//do the printout thing
	CWastePrintout		*thePrintout = CWastePrintout::CreatePrintout(prto_PrintView);
	Assert_(thePrintout != nil);
	
	thePrintout->SetPrintRecord(mPrintRecordH);
	thePrintout->SetWEHandle(hWE);
	
	long id = 'TBox';
	LPlaceHolder	*textPlace = (LPlaceHolder *) thePrintout->FindPaneByID(id);
	Assert_(textPlace != nil);
	textPlace->InstallOccupant(we, atNone);
	
	thePrintout->DoPrintJob();
	delete thePrintout;
	
	//restore the port
	WESetInfo(wePort, &oldPort, hWE);
	//resize the frame. This takes care of the destRect
	//and the viewRect
	we->ResizeFrameTo(viewRect.right-viewRect.left, viewRect.bottom - viewRect.top, false);
	
	//restore other variables
	if(active)
		WEActivate(hWE);
	we->FeatureFlag(weFOutlineHilite, prevOutline);
	we->FeatureFlag(weFDrawOffscreen, prevDrawScreen);
#endif
}


void CLogWindow::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{	
	switch(inCommand)
	{
	case cmd_Print :
	case cmd_SaveAs :
		outEnabled = true;
		break;
	case cmd_Close :
		outEnabled = IsVisible();
		break;
	
	default:
		// Call inherited.
		LWindow::FindCommandStatus( inCommand,
			outEnabled, outUsesMark, outMark, outName );
		break;
	}
}

static const RGBColor black = {0x0000, 0x0000, 0x0000};
static const RGBColor green = {0x0000, 0xA000, 0x0000};
static const RGBColor violet = {0xFFFF, 0x0000, 0xFFFF};
static const RGBColor red = {0xFFFF, 0x0000, 0x0000};
static const RGBColor blue = {0x0000, 0x0000, 0xFFFF};
static const RGBColor yellow = {0x8000, 0x8000, 0x0000};
static const RGBColor orange = {0xA000, 0x4000, 0x0000};

static bool operator != (const TextStyle& lhs, const TextStyle& rhs)
{
  return lhs.tsFace != rhs.tsFace ||
         lhs.tsColor.red != rhs.tsColor.red ||
         lhs.tsColor.green != rhs.tsColor.green ||
         lhs.tsColor.blue != rhs.tsColor.blue;
}

void CLogWindow::FeedThisLine(const char *buffer, long n, bool isStdErr)
{
	TextStyle ts, new_ts;
	
	ts.tsColor = black;
	ts.tsFace = normal;
	
	new_ts = ts;

	if(n >= 5 && buffer[1] == ' ' &&
		(buffer[0] == 'M' || buffer[0] == 'C' || buffer[0] == 'U' ||
		buffer[0] == 'P' || buffer[0] == 'N' || buffer[0] == 'A' || buffer[0] == 'R' ||
		buffer[0] == 'I' || buffer[0] == '?' || buffer[0] == '>' || buffer[0] == '<'))
	{
		switch(buffer[0])
		{
		case 'I' :
		case '?' :
			new_ts.tsColor = yellow;
			break;
		case 'U' :
		case 'N' :
			new_ts.tsColor = green;
			break;
		case 'A' :
		case 'R' :
		case 'M' :
			new_ts.tsColor = violet;
			break;
		case 'C' :
		case '<' :
			new_ts.tsColor = red;
			break;
		case 'P' :
		case '>' :
			new_ts.tsColor = blue;
			break;
		}
		new_ts.tsFace = bold;
	}
	else if(isStdErr)
	{
		new_ts.tsColor = orange;
		new_ts.tsFace = bold;
	}

  if ( mCachedOutput.empty() ) {
    mCachedStyle = new_ts;
  } else if ( new_ts != mCachedStyle ) {
    FlushCache();
    mCachedStyle = new_ts;
  }
  mCachedOutput.append(buffer, n);
  
  // make sure we'll update the log window in the near future
  if ( mFlushCacheTimer == NULL )
  {
    if ( InstallEventLoopTimer(GetCurrentEventLoop(), 100 * kEventDurationMillisecond, kEventDurationForever, sFlushCacheTimerUPP, this, &mFlushCacheTimer) == noErr )
      mFlushCacheTimerRunning = true;
    else
      this->FlushCache();
  }
  else if ( !mFlushCacheTimerRunning )
  {
    if ( SetEventLoopTimerNextFireTime(mFlushCacheTimer, 100 * kEventDurationMillisecond) == noErr )
      mFlushCacheTimerRunning = true;
  }
  if ( !mFlushCacheTimerRunning ) this->FlushCache(); // do it now, since it won't happen from the timer
}

void CLogWindow::OutColor(const char *txt, long len)
{
  this->FlushCache();
	if(len == 0)
		return;
	WTextView *we = GetWE();
	we->SetTextSelection(max_Int32, max_Int32);
	if(strchr(txt, '\n') != 0L)
	{
		UStr buf;
		buf.set(txt, len);
		buf.replace('\n', '\r');
		we->InsertText((char *)buf.c_str(), buf.length());
	}
	else
		we->InsertText((char *)txt, len);
}

void CLogWindow::OutColor(kConsoleColor color)
{
  this->FlushCache();
	WTextView *we = GetWE();
	we->SetTextSelection(max_Int32, max_Int32);

	TextStyle new_ts;
	switch(color)
	{
		default:
			return;
			break;
		case kNormal:
			new_ts.tsColor = black;
			new_ts.tsFace = normal;
			we->SetStyle(weDoColor | weDoFace, new_ts);
			break;
		case kBrown :
			new_ts.tsColor = orange;
			we->SetStyle(weDoColor, new_ts);
			break;
		case kGreen :
			new_ts.tsColor = green;
			we->SetStyle(weDoColor, new_ts);
			break;
		case kMagenta :
			new_ts.tsColor = violet;
			we->SetStyle(weDoColor, new_ts);
			break;
		case kRed :
			new_ts.tsColor = red;
			we->SetStyle(weDoColor, new_ts);
			break;
		case kBlue :
			new_ts.tsColor = blue;
			we->SetStyle(weDoColor, new_ts);
			break;
		case kBold :
			new_ts.tsFace = bold;
			we->SetStyle(weDoFace, new_ts);
			break;
		case kItalic :
			new_ts.tsFace = italic;
			we->SetStyle(weDoFace, new_ts);
			break;
		case kUnderline :
			new_ts.tsFace = underline;
			we->SetStyle(weDoFace, new_ts);
			break;
	}
}

void CLogWindow::OutColor(/*WEStyleMode*/int mode, const TextStyle  & ts)
{
  this->FlushCache();
	WTextView *we = GetWE();
	we->SetTextSelection(max_Int32, max_Int32);
	we->SetStyle(mode, ts);
}

void CLogWindow::FlushCache()
{
  mFlushCacheTimerRunning = false;
  if ( !mCachedOutput.empty() ) {
  	WTextView *we = GetWE();
  	we->SetTextSelection(max_Int32, max_Int32);

  	TextStyle ts;
  	
  	ts.tsColor = black;
  	ts.tsFace = normal;
  	
    we->SetStyle(/*weDoFace | */weDoColor, mCachedStyle);
  	we->InsertText((char *)mCachedOutput.c_str(), mCachedOutput.size());
  	we->SetStyle(/*weDoFace | */weDoColor, ts);

    mCachedOutput.clear();
  }
}

long CLogWindow::WriteCharsToConsole(const char *buffer, long n, bool isStdErr)
{
  bool    atLeastOneLine = false;
  
	Assert_( n != 0  &&  buffer != NULL );
	if( n == 0  ||  buffer == NULL )
	  return 0;
	  
	if(sbuf == 0L)
	{
		sbuf = (char *)malloc((MAX_CHAR_BY_LINE + 3) * sizeof(char));
		if(sbuf == 0L)
			return 0;
	}

	if(!IsVisible() && mAutoShow)
		Show();
	
  long  i=0;
	char  c;
once_again:
	for(; i < n && snumbuf < MAX_CHAR_BY_LINE; snumbuf++)
	{
		c = buffer[i++];
		if(c == 0x0d || c == 0x0a)
			break;
		sbuf[snumbuf] = c;
	}

	if(c == 0x0d || c == 0x0a || snumbuf == MAX_CHAR_BY_LINE)
	{
		sbuf[snumbuf++] = 0x0d;
		sbuf[snumbuf++] = 0x00;
	
		FeedThisLine(sbuf, snumbuf - 1, isStdErr);
		snumbuf = 0;
		atLeastOneLine = true;
	}

	if(i < n)
		goto once_again;
	
	if ( isStdErr  &&  atLeastOneLine  &&  mSelectOnError )
	  Select();

  return n;
}

void CLogWindow::WriteCharsToConsoleSelf(const std::string& str, bool isStdErr)
{
  bool    atLeastOneLine = false;
  
	Assert_( !str.empty() );
	if( str.empty() )
	  return;
	  
	if(sbuf == 0L)
	{
		sbuf = (char *)malloc((MAX_CHAR_BY_LINE + 3) * sizeof(char));
		if(sbuf == 0L)
			return;
	}

	if(!IsVisible() && mAutoShow)
		Show();
	
  std::string::const_iterator i(str.begin());
	char c;
once_again:
	for(; i != str.end() && snumbuf < MAX_CHAR_BY_LINE; snumbuf++)
	{
		c = *i++;
		if(c == 0x0d || c == 0x0a)
			break;
		sbuf[snumbuf] = c;
	}

	if(c == 0x0d || c == 0x0a || snumbuf == MAX_CHAR_BY_LINE)
	{
		sbuf[snumbuf++] = 0x0d;
		sbuf[snumbuf++] = 0x00;
	
		FeedThisLine(sbuf, snumbuf - 1, isStdErr);
		snumbuf = 0;
		atLeastOneLine = true;
	}

	if(i != str.end())
		goto once_again;
	
	if ( isStdErr  &&  atLeastOneLine  &&  mSelectOnError )
	  Select();
}
