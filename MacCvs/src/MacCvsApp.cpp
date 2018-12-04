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
 * MacCvsApp.cpp --- application main
 */
#include "GUSIInternal.h"

#include "GUSIFileSpec.h"

#include <unistd.h>
#include <sys/wait.h>

#include <PP_DebugMacros.h>

#include "MacCvsApp.h"
#include "MacCvsConstant.h"
#include "AppGlue.mac.h"

#include "LCVSWasteEdit.h"
#include "LogWindow.h"
//#include "CWastePrintout.h"
#include <UModalDialogs.h>
#include <UControlRegistry.h>

#include <LEditText.h>
#include <LCheckBox.h>
#include <LPopupButton.h>
#include <LTextEditView.h>
#include <LPushButton.h>
#include <LPopupGroupBox.h>
#include <LOutlineTable.h>

#include <LActiveScroller.h>
#include <LTable.h>
#include <LIconPane.h>
#include <LGroupBox.h>
#include <LTextButton.h>

#ifdef __MWERKS__
#include <UDebugNew.h>
#endif
#include <UDebugUtils.h>
#include <UHeapUtils.h>
#include <UMemoryEater.h>
#include <UOnyx.h>

#include <UStandardDialogs.h>
#include <LCMAttachment.h>
#include <UCMMUtils.h>

#	include <LCFBundle.h>
#	include <LCFURL.h>
#	include <LCFString.h>

#if PP_Debug	
#	include <LDebugMenuAttachment.h>
#	include <LCommanderTree.h>
#	include <LPaneTree.h>
#	include <LTreeWindow.h>
# define AmIBeingMWDebugged()	false
#endif

#if __MWERKS__ >= 0x2200
#	include <UMenus.h>
#endif

#include "GetPrefs.h"
#include "Authen.h"
#include "CvsPrefs.h"
#include "AppGlue.h"
#include "AppConsole.h"
#include "MacCvsAE.h"
#include "Persistent.h"
#include "MacBinEncoding.h"
#include "ImportFilterDlg.h"
#include "PromptFiles.h"
#include "CvsLog.h"
#include "MultiString.h"
#include "CvsCommands.h"
#include "MacrosSetup.h"
#include "TclGlue.h"
#include "PythonGlue.h"
#include "BrowseViewModel.h"
#include "BrowseViewHandlers.h"
#include "ItemListDlg.h"

#include "LRadioEnabler.h"
#include "LCheckEnabler.h"
#include "LPopupFiller.h"
#include "LRadioTabEnabler.h"
#include "CBrowserTable.h"
#include "CTreeView.h"
#include "LCmdHelpBevelBtn.h"
#include "ABalloon.h"
#include "CBrowserWind.h"
#include "umain.h"
#include "UPseudoSupport.h"
#include "CScrollWheelAttachment.h"
#include "CCVSToolbar.h"
#include "CvsArgs.h"
#include "LIconControl.h"
#include "MacMisc.h"


#if !defined(MAC_OS_X_VERSION_MIN_REQUIRED) /* ||  MAC_OS_X_VERSION_MIN_REQUIRED == MAC_OS_X_VERSION_10_0 */
  enum { 
    kQDUseCGTextRendering = (1<<1),
    kQDUseCGTextMetrics = (1<<2)
  };
#endif

#define kSleepTimeWhenIdleAndFront  10    // for cursor blink time
#define kSleepTimeWhenIdleAndBackground   60*60 // one minute sleep time so we catch the auto-logout
#define kSleepTimeWhenActiveAndFront  0
#define kSleepTimeWhenActiveAndBackground   0

static const Point defaultToolbarPosition = { 10, 50 };

static CPersistentBool gWarnPython("P_WarnPython", true);
static CPersistentT<short> gLogFont("P_LogFont", kFontIDMonaco, kNoClass);
static CPersistentT<short> gLogSize("P_LogSize", 9, kNoClass);
static CPersistentBool gUseCGTextRendering("P_CGTextRendering", true);
static CPersistentBool  gToolbarDocked("P_ToolbarDocked", true);
static CPersistentBool  gToolbarVisible("P_ToolbarVisible", true);
static CPersistentT<Point> gToolbarPosition("P_ToolbarPosition", defaultToolbarPosition, kNoClass);
extern CPersistentBool gUseExtDiff;

template class TMPersistent<CWindowSpec>;
class CMWindowSpec : public TMPersistent<CWindowSpec>
{
public:
  using TMPersistent<CWindowSpec>::list_t;
  using TMPersistent<CWindowSpec>::GetList;
	CMWindowSpec(const char* uniqueName, kClassPersistent pclass = kNoClass) : TMPersistent<CWindowSpec>(uniqueName, pclass)
	{
	}
	void RemoveAll()
	{
		list_t&	allSpecs = this->GetList ();
		allSpecs.erase (allSpecs.begin (), allSpecs.end ());
	}
	void insert_begin(CWindowSpec& inValue) 
	{
		list_t&	allSpecs = this->GetList ();
	  allSpecs.insert(allSpecs.begin(), inValue);
	}
};

CMString sBrowserLoc(20, "P_BrowserLocMachO");
static CMWindowSpec gBrowserPos("P_BrowserPosMachO", kNoClass);

#define kMacCvsPrefsFolderName  "MacCvs Preferences"

static char gAppPath[MAX_PATH];
static char gAppResourcesPath[MAX_PATH];

CMacCvsApp *CMacCvsApp::gApp = nil;
short gRealAppResID = -1;

#undef GetMBarHeight

static void AppMain()
{
	UEnvironment::InitEnvironment();

#if PP_Debug

		// If under the debugger, use the debugger for everything
		// (throw, signal, DebugNew), else use alerts.
	
	if (AmIBeingMWDebugged()) {

		UDebugging::SetDebugThrow(debugAction_Debugger);
		UDebugging::SetDebugSignal(debugAction_Debugger);

	}
	else
	{

			// Not under the MW Debug, so use alerts. If you use
			// another debugger, you could set this up the same way. Point
			// is to use the debugger's facilities when under the debugger,
			// else alerts.
		
		UDebugging::SetDebugThrow(debugAction_Alert);
		UDebugging::SetDebugSignal(debugAction_Alert);

			// Use our own error handler for DebugNew which uses alerts
			// instead of DebugStr calls.
	
		PP_DebugNewInstallPPErrorHandler_();
	}
#else
		// In final build mode so nothing should be seen. These are
		// commented out because gDebugThrow and gDebugSignal are
		// initialized to debugAction_Nothing -- assignment here is
		// unnecessary (but left in as a comment for illustration).
	
	UDebugging::SetDebugThrow(debugAction_Nothing);
	UDebugging::SetDebugSignal(debugAction_Nothing);
#endif	

		// Clean up any "leaks" that might have occured at static
		// initialization time.
	{
		SLResetLeaks_();
#ifdef __MWERKS__
		DebugNewForgetLeaks_();
#endif
	}

		// Normal initializations
	InitializeHeap(5);
	UQDGlobals::InitializeToolbox();
//	InitOpenTransport();
	::InitCursor();
	::FlushEvents(everyEvent, nil);

	LCFBundle bundle(::CFBundleGetMainBundle());
	LCFURL url(::CFBundleCopyBundleURL(bundle));
	url.Adopt(::CFURLCreateCopyDeletingLastPathComponent(nil, url.GetTypeRef()));
	::CFURLGetFileSystemRepresentation(url.GetTypeRef(), TRUE, (UInt8 *)gAppPath, MAX_PATH);
	url.Adopt(::CFBundleCopyResourcesDirectoryURL(bundle));
	::CFURLGetFileSystemRepresentation(url.GetTypeRef(), TRUE, (UInt8 *)gAppResourcesPath, MAX_PATH);

		// Check Debugging environment
#if PP_Debug
	UDebugUtils::CheckEnvironment();
#endif
	
/*
		// Install a GrowZone to catch low-memory situations	
	LGrowZone* theGZ = NEW LGrowZone(20000);
	ValidateObject_(theGZ);
	SignalIf_(theGZ->MemoryIsLow());
*/

		// Create the application object and run. The scope of
		// the object is limited as we need to control when
		// the object's destructor is called (in relation to
		// the code that follows it).
	{
		CMacCvsApp	theApp;
		theApp.Run();
	}

#if PP_Debug

		// This cleanup isn't necessary (they are items that are to
		// remain for the duration of the application's run time. When
		// the app quits and the Process Manager reclaims the heap,
		// the memory occupied by these items will be released). This
		// is just done to keep things like DebugNew and Spotlight
		// quiet.
	
	LMenuBar*	theBar = LMenuBar::GetCurrentMenuBar();
	delete theBar;

	URegistrar::DisposeClassTable();

	LPeriodical::DeleteIdlerAndRepeaterQueues();

	UMemoryEater::DeleteMemoryLists();

	LModelDirector*	theDirector = LModelDirector::GetModelDirector();
	delete theDirector;

	LModelObject::DestroyLazyList();

	UScreenPort::Dispose();
	
	LComparator*	theCompare = LComparator::GetComparator();
	delete theCompare;
	
	LLongComparator*	theLongCompare = LLongComparator::GetComparator();
	delete theLongCompare;

//	DisposeOf_(theGZ);
	
#endif

#ifdef __MWERKS__
	DebugNewReportLeaks_();
#endif
}

#ifdef __MWERKS__
#if __MWERKS__ < 0x2301
void	PP_NewHandler() throw (PP_STD::bad_alloc);	// defined in PPDebug_New.cp
#endif
#endif

int
main()
{
	gRealAppResID = ::CurResFile();

#ifdef __MWERKS__
#if __MWERKS__ < 0x2301
		// First things first. Install a new_handler.
	(void)PP_STD::set_new_handler(PP_NewHandler);
#endif
#endif

		// Don't let exceptions propagate outside of main
	try {
		AppMain();
	} catch (...) {
		SignalStringLiteral_("Exception caught in main");
	}
	
	// If this is called after __terminate, it will cause OpenTransport
	// to crash (on 8/9, not Carbon)
	// All the sockets may not be clposed at this point, mainly because
	// the user may have cancelled a connection and the socket is scheduled
	// to be removed later by GUSI.
	//GUSIContext::LiquidateAll();
	
	return 0;
}

static FSRef* GetAppFSRef(FSRef& outAppRef)
{
	LCFBundle bundle(::CFBundleGetMainBundle());
	LCFURL url(::CFBundleCopyBundleURL(bundle));
	return CFURLGetFSRef(url.GetTypeRef(), &outAppRef) ? &outAppRef : NULL;
}

class	LInverseCheckEnabler : public LCheckEnabler
{
public:
	enum {class_ID = 'CHKe'};

	LInverseCheckEnabler(LStream *inStream) : LCheckEnabler(inStream) {}

	virtual void		SetValue(SInt32 inValue);
};

void LInverseCheckEnabler::SetValue(SInt32 inValue)
{
	LCheckBox::SetValue(inValue);
	
	LView *superView = GetSuperView();
	if(superView == 0L)
	{
		Assert_(0);
		return;
	}

	LPane *view = dynamic_cast<LPane*>(superView->FindPaneByID(mEnableView));
	
	if(view == 0L)
	{
		Assert_(0);
		return;
	}
	
	if(GetValue() == Button_On)
	{
		view->Disable();
	}
	else
	{
		view->Enable();
	}
}



class CAboutWindow : public LWindow
{
public:
	enum { class_ID = 'CABT' };
	
	CAboutWindow(LStream* inStream) : LWindow(inStream) {}
  void FinishCreateSelf();
};

void CAboutWindow::FinishCreateSelf()
{
  LPane*    version = this->FindPaneByID('vers');
  if ( version )
  {
  	LCFBundle bundle(::CFBundleGetMainBundle());
  	LCFString vers((CFStringRef)::CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleGetInfoString")));
  	char versStr[100];
  	vers.GetCString(versStr, 100);
  	version->SetDescriptor(CPStr(versStr));
  }
  
  LIconControl*  iconPane = dynamic_cast<LIconControl*>(this->FindPaneByID('icon'));
  if ( iconPane )
  {
    CIconRef        icon;
    FSRef           appFSRef;
    if ( GetFileIcon(GetAppFSRef(appFSRef), kSelectorAllAvailableData, icon) == noErr )
    {
      ControlButtonContentInfo    content;
      content.contentType = kControlContentIconRef;
      content.u.iconRef = icon.GetIconRef();
      iconPane->SetDataTag(0, kControlIconContentTag, sizeof(content), &content);
    }
  }
  
}

class CMacCvsLogout : public LPeriodical

	{
		
		UInt32			mIdleStart;
		
	public:
	
						CMacCvsLogout	(void);
		virtual			~CMacCvsLogout	(void);
		
		virtual	void	SpendTime		(const EventRecord&	inMacEvent);
	};
	
CMacCvsLogout::CMacCvsLogout (void)
	
	: mIdleStart (::TickCount ())
	
	{ // begin CMacCvsLogout
		
		//	To get _every_ event (not just idles)
		StartRepeating (30 * kEventDurationSecond);
		
	} // end CMacCvsLogout

CMacCvsLogout::~CMacCvsLogout (void)
	
	{ // begin ~CMacCvsLogout
		
	} // end ~CMacCvsLogout

void
CMacCvsLogout::SpendTime (

	const EventRecord&	inMacEvent)
	
	{ // begin SpendTime
		
		//	Do we need to do anything?
		if	(gAuthen.kind() != pserver || 
			 !gCvsPrefs.HasLoogedIn()  ||
			 (gCvsPrefs.LogoutTimeOut() <= 0))
			return;
		
		//	Calculate the idle time
		UInt32	curTicks = ::TickCount ();
		switch (inMacEvent.what) {
			//	Reset events
			case mouseDown:
			case mouseUp:
			case keyDown:
			case keyUp:
			case autoKey:
			case osEvt:
			case kHighLevelEvent:
				mIdleStart = curTicks;
				break;
			
			//	Idle events
			case updateEvt:
			case activateEvt:
			case diskEvt:
				break;
			} // switch
		
		if ((curTicks - mIdleStart) < (gCvsPrefs.LogoutTimeOut() * 60L * 60L)) return;
		
		mIdleStart = curTicks;
		cvs_err ("Making automatic logout after %d minute(s):\n", gCvsPrefs.LogoutTimeOut());
		CvsCmdLogout ();
	
	} // end SpendTime

const char *CMacCvsApp::GetAppPath(void)
{
	return gAppPath;
}

const char *CMacCvsApp::GetAppResourcesPath(void)
{
	return gAppResourcesPath;
}

const char *CMacCvsApp::GetMacCvsPrefsPath(void)
{
	static CStr		prefsPath;
	
	if ( prefsPath.empty() )
	{
		OSStatus	err;
		FSRef			ref;
		CStr			path;
		
		err = ::FSFindFolder(kOnAppropriateDisk, kPreferencesFolderType, kCreateFolder, &ref);
	
		if (err == noErr)
		{
			char buffer[PATH_MAX];
			struct stat sb;
			
			err = FSRefMakePath(&ref, (UInt8*)buffer, sizeof(buffer));
			if ( err == noErr )
			{
			  path = buffer;

				if(!path.endsWith(kPathDelimiter))
					path << kPathDelimiter;
				path << kMacCvsPrefsFolderName;

				if(stat(path, &sb) != -1)
				{
					if(!S_ISDIR(sb.st_mode))
					{
						cvs_err("The file '%s' already exists and is not a directory", path.c_str(), errno);
						err = errFSNotAFolder;
					}
				}
				else if( mkdir(path, 0750) != 0)
				{
					cvs_err("Unable to create the directory '%s' (error %d)\n", path.c_str(), errno);
					err = afpAccessDenied;
				}
			}
			else cvs_err("Unable to get preferences folder path (error %d)\n", err);
		}
		else cvs_err("Unable to find preferences folder (error %d)\n", err);
	
		if ( err == noErr ) prefsPath = path;
	}
	
	return prefsPath;
}

CMacCvsApp::~CMacCvsApp()
{
	::ExitMovies();

	//	Clean up the logout task
	delete mLogoutTask;
	mLogoutTask = 0;
	
	mWindowList.RemoveItemsAt(mWindowList.GetCount(), 1);
	delete mSpinCursor;
	mSpinCursor = 0;

		// Clean up after Standard Dialogs
	PP_StandardDialogs::Unload();
	
#if PP_Debug
	DisposeOf_(mDebugAttachment);
#endif
}

CMacCvsApp::CMacCvsApp (void) 
	
	: LDocApplication ()
	, mLogoutTask (0)
	, m_IsFrontProcess(true)
	, m_Notif(0L)
	, m_NotifIcon(0L)
	, m_CvsRunning (0)
	, gCvsStopping (false)
	, mWindow(0L)
	, m_CvsProcess(0L)
{
	gApp = this;

#if qCvsDebug
	SetDebugThrow_( debugAction_Debugger );
	SetDebugSignal_( debugAction_Debugger );
#else
	SetDebugThrow_( debugAction_Alert );
	SetDebugSignal_( debugAction_Alert );
#endif
	
		// Register ourselves with the Appearance Manager
	if (UEnvironment::HasFeature(env_HasAppearance)) {
		::RegisterAppearanceClient();
	}

		// Register debugging classes
#if PP_Debug
	RegisterClass_(LCommanderTree);
	RegisterClass_(LPaneTree);
	RegisterClass_(LTreeWindow);
#endif

	// Register PowerPlant class creator functions.
	RegisterClass_(LButton);
	RegisterClass_(LCaption);
	RegisterClass_(LDialogBox);
	RegisterClass_(LEditField);
	RegisterClass_(LListBox);
	RegisterClass_(LPane);
	RegisterClass_(LPicture);
	RegisterClass_(LPlaceHolder);
	RegisterClass_(LPrintout);
	RegisterClass_(LScroller);
	RegisterClass_(LStdControl);
	RegisterClass_(LStdButton);
	RegisterClass_(LStdCheckBox);
	RegisterClass_(LStdRadioButton);
	RegisterClass_(LStdPopupMenu);
	RegisterClass_(LTextEditView);
	RegisterClass_(LView);
	RegisterClass_(LWindow);

	RegisterClass_(LRadioGroup);
	RegisterClass_(LTabGroup);

	RegisterClass_(LCicnButton);
	RegisterClass_(LOffscreenView);
	RegisterClass_(LActiveScroller);
	RegisterClass_(LTable);
	RegisterClass_(LIconPane);
	RegisterClass_(LGroupBox);
	RegisterClass_(LTextButton);
	RegisterClass_(LCMAttachment);
	RegisterClass_(LBorderAttachment);

	// Register the Appearance Manager/GA classes
#if PP_Target_Carbon
									// AM always present on Carbon
		UControlRegistry::RegisterAMClasses();
#else
		UControlRegistry::RegisterClasses();
#endif
	RegisterClass_(LTextEditView);

	// Register custom classes.
	RegisterClass_(CAboutWindow);
	RegisterClass_(CCvsWasteEdit);
	RegisterClass_(CLogWindow);
//	RegisterClass_(CWastePrintout);
	RegisterClass_(LRadioEnabler);
	RegisterClass_(LCheckEnabler);
	RegisterClass_(LInverseCheckEnabler);
	RegisterClass_(LPopupFiller);
	RegisterClass_(LRadioTabEnabler);
	RegisterClass_(LColorEraseAttachment);
	RegisterClass_(LEraseAttachment);
	
	RegisterClass_(LOutlineTable);
	RegisterClass_(LTableView);
	RegisterClass_(LSelectTable);
	RegisterClass_(COutlineTable);
	RegisterClass_(CMacBinTable);
	RegisterClass_(CBrowserTable);
	RegisterClass_(CBrowserCMAttachment);
	RegisterClass_(CBrowserWind);
	RegisterClass_(CTreeView);
	RegisterClass_(LCmdHelpBevelButton);
	
	RegisterClass_(ABalloon);
	RegisterClass_(AAutoBalloon);
	RegisterClass_(AUMBalloon);
	RegisterClass_(TCVSToolbar);
	ABalloon::EnableControlKeyPop();
	
	CBrowserWind::RegisterClasses();
	
//	SetSleepTime(kSleepTimeWhenIdleAndFront);
SetSleepTime(0xffffffff);

	// set-up the spin cursor
	mSpinCursor = NEW LBWCursorList((ResIDT)0);
	mSpinCounter = 0;
	mLastSpinTicks = 0;
	mLastUserCancelCheckTicks = 0;
	
		// Preload facilities for the Standard Dialogs
	PP_StandardDialogs::Load();

		// Require at least Navigation Services 1.1. See comments
		// above SetTryNavServices in UConditionalDialogs.cp for why
		// you might wish to do this.
#if PP_StdDialogs_Option == PP_StdDialogs_Conditional
	UConditionalDialogs::SetTryNavServices(0x01108000);
#endif

		// Initialize contextual menus
	UCMMUtils::Initialize();
	AddAttachment(NEW LCMAttachment);

	mLogoutTask = NEW CMacCvsLogout;
}

Boolean		CMacCvsApp::AttemptQuit( SInt32 inSaveOption )
{
	Boolean res = LDocApplication::AttemptQuit(inSaveOption);
	if(res)
	{
		//	Save the window locations
		gBrowserPos.RemoveAll();
/*
		CMWindowSpec::list_t&	allSpecs = gBrowserPos.GetList ();
		allSpecs.erase (allSpecs.begin (), allSpecs.end ());
*/		
		
		for (WindowPtr	macWindowP = ::FrontWindow(); macWindowP != 0; macWindowP = ::GetNextWindow (macWindowP)) {
			LWindow*	theWindow = LWindow::FetchWindowObject(macWindowP);
			if (!theWindow->HasAttribute(windAttr_Regular)) continue;

			CWindowSpec	logSpec;
			logSpec.pathIndex = -1;	//	Default to log window
			
			if (theWindow != mWindow) {
				CBrowserTable*	browser = dynamic_cast<CBrowserTable*> (theWindow->FindPaneByID (item_BrowserTable));
				if (!browser) continue;
				
				//	Find the path
				CMString::list_t::const_iterator i = sBrowserLoc.Find (browser->GetPath ());
				if (i == sBrowserLoc.end ()) continue;
					
				logSpec.pathIndex = i - sBrowserLoc.begin ();
			} // else
				
			logSpec.visible = theWindow->IsVisible ();
			logSpec.active = theWindow->IsActive ();
			theWindow->GetGlobalBounds (logSpec.globalBounds);
			gBrowserPos.insert_begin(logSpec);	//	Insert in reverse order
			} // for
			
		gBrowserPos.TouchTimeStamp ();
		
		CloseBrowsers();
		
		Rect toolbarBounds;
		m_toolbar->GetGlobalBounds(toolbarBounds);
		gToolbarPosition = *(Point*)&toolbarBounds;
		m_toolbar->CanCloseNow(true);
		gToolbarVisible = (m_toolbar->GetVisibleState() != triState_Off);
		
		gCvsPrefs.save();
	}
	return res;
}

void CMacCvsApp::DoSpinCursor(void)
{
  UInt32    ticks(TickCount());
  if ( ticks - mLastSpinTicks > 10 ) // spinning about 6 times a second should be enough...
	{
	  mSpinCursor->Animate(mSpinCounter++);
	  mLastSpinTicks = ticks;
	}
}

bool CMacCvsApp::CheckForUserCancel(void)
{
  bool      result(false);
  UInt32    ticks(TickCount());
  if ( ticks - mLastUserCancelCheckTicks > 6 ) // checking about 10 times a second should be enough...
	{
	  mLastUserCancelCheckTicks = ticks;
	  if ( CheckEventQueueForUserCancel() ) result = true;
	}
	return result;
}

void CMacCvsApp::MakeMenuBar (void)

{ // begin MakeMenuBar
	
	LDocApplication::MakeMenuBar();
	
#if PP_Uses_Aqua_MenuBar
	if (UEnvironment::HasFeature(env_HasAquaTheme)) {
		MenuItemDataRec	itemData;
		itemData.whichData = kMenuItemDataCmdKey;
		itemData.cmdKey = ';';
		SetMenuItemData (nil, kHICommandPreferences, true, &itemData);
	} // if
#endif

#if PP_Debug


		// Check the version
		//
		// When you have added your 'vers' resource, uncomment
		// this. You may need to change the kAppVersion constant
		// to use your own.
//	{
//		StResource versH(FOUR_CHAR_CODE('vers'), 1);
//		SInt32 theVers = *((SInt32*)(*versH));
//		if (kAppVersion != theVers) {
//			SignalStringLiteral_("Versions don't match");
//		}
//	}		
	
		// Build the Debug menu. The SDebugInfo structure is
		// manually filled out here to allow ease of customization
		// (this is project stationery ;-)

	SDebugInfo	theDebugInfo;
	
	theDebugInfo.debugMenuID			= MENU_DebugMenu;
	theDebugInfo.debugMenuIconID		= icsX_DebugMenuTitle;

	theDebugInfo.commanderTreePPobID	= PPob_AMLCommanderTreeWindow;
	theDebugInfo.commanderTreePaneID	= TreeWindow_Tree;
	theDebugInfo.commanderTreeThreshold	= 1;	// Every 1 second
	
	theDebugInfo.paneTreePPobID			= PPob_AMLPaneTreeWindow;
	theDebugInfo.paneTreePaneID			= TreeWindow_Tree;
	theDebugInfo.paneTreeThreshold		= 0;	// Don't autoupdate
	
	theDebugInfo.validPPobDlogID		= PPob_AMDialogValidatePPob;
	theDebugInfo.validPPobEditID		= ValidatePPob_EditResIDT;
	
	theDebugInfo.eatMemPPobDlogID		= PPob_AMEatMemoryDialog;
	theDebugInfo.eatMemRadioHandleID	= EatMemoryDialog_RadioHandle;
	theDebugInfo.eatMemRadioPtrID		= EatMemoryDialog_RadioPtr;
	theDebugInfo.eatMemEditID			= EatMemoryDialog_EditAmount;
	theDebugInfo.eatMemRadioGroupID		= EatMemoryDialog_RadioGroup;
	
	mDebugAttachment = NEW LDebugMenuAttachment(theDebugInfo);
	ValidateObject_(mDebugAttachment);
	mDebugAttachment->InitDebugMenu();
			
	AddAttachment(mDebugAttachment);

#endif

	// load preferences
	gCvsPrefs.load();
	
  if ( gUseCGTextRendering )
  {
    CFBundleRef   qdBundle(CFBundleGetBundleWithIdentifier(CFSTR("com.apple.HIToolbox")));
    if ( qdBundle  &&  CFBundleLoadExecutable(qdBundle) )
    {
      UInt32 (*pQDSwapTextFlags)(UInt32 newFlags);
      
      pQDSwapTextFlags = (UInt32(*)(UInt32))CFBundleGetFunctionPointerForName(qdBundle, CFSTR("QDSwapTextFlags"));
      if ( pQDSwapTextFlags != NULL ) pQDSwapTextFlags(kQDUseCGTextRendering|kQDUseCGTextMetrics);
    }
  }

} // end MakeMenuBar

void CMacCvsApp::GetAppName( UStr & name )
{
	LCFBundle bundle(::CFBundleGetMainBundle());
	LCFString vers((CFStringRef)::CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleShortVersionString")));
	char versStr[100];
	vers.GetCString(versStr, 100);
	name = versStr;
}

void CMacCvsApp::Initialize()
{
	LDocApplication::Initialize();

	::EnterMovies();
	
	::UseResFile(gRealAppResID);
	CPython_Interp::InitPythonSupport();
	
	// Create log window
	mWindow = (CLogWindow *)LWindow::CreateWindow(rPPob_CVSLogWindow, this);
	Assert_(mWindow);

	// add it to the windows list
	if(mWindow != 0L)
	{
		mWindowList.InsertItemsAt(1, LArray::index_Last, mWindow);
		UpdateWindowsMenu();
	}

	// create the toolbar
	m_toolbar = dynamic_cast<TCVSToolbar *>(
		LWindow::CreateWindow(rPPob_CVSToolbar, LCommander::GetTopCommander()));
	ThrowIfNil_(m_toolbar);

	// load the toolbar flying fish movie...
	try
	{
		LCFBundle bundle(::CFBundleGetMainBundle());
		LCFURL urlresources(::CFBundleCopyResourcesDirectoryURL(bundle));
		LCFString movieName("toolbar.mov");
		LCFURL url(::CFURLCreateCopyAppendingPathComponent(NULL, urlresources.GetTypeRef(), movieName.GetTypeRef(), FALSE));
		char rsrcPath[MAX_PATH];
		::CFURLGetFileSystemRepresentation(url.GetTypeRef(), TRUE, (UInt8 *)rsrcPath, MAX_PATH);
		Movie newMovie;
		GUSIFileSpec movieref(rsrcPath);
		FSSpec moviespec;
		ThrowIfOSErr_(::FSGetCatalogInfo(movieref, kFSCatInfoNone, NULL, NULL, &moviespec, NULL));
		
		short dataRefnum, actualResID;
		OSErr err = ::OpenMovieFile(&moviespec, &dataRefnum, 0);
		ThrowIfOSErr_(err);
		err = ::NewMovieFromFile(&newMovie, dataRefnum, &actualResID, NULL, newMovieActive, NULL);
		ThrowIfOSErr_(err);
		::CloseMovieFile(dataRefnum);

		m_toolbar->HaveMovie(newMovie);
		m_toolbar->CanCloseNow(false);
	}
	catch(...)
	{
	}

	//	Load the prefs (log window needed)
	bool	wasEmptyPrefs = gCvsPrefs.empty();
	if(wasEmptyPrefs)
		CompatGetPrefs();
	else
	{
		UStr cvers;
		GetAppName(cvers);
		cvs_out("Welcome to %s\n", cvers.c_str());
		cvs_out("CVSROOT: %s (%s)\n", (const char *)gCvsPrefs, gAuthen.kindstr());
	}

	// waste doesn't seem to want to change the font
	// when the selection is empty. hum...
	RefreshLogFont();
	
	// check whether settings need to be migrated
	//
	CStr		appSettingsPath(GetAppPath());
	if ( !appSettingsPath.empty() )
	{
		struct stat sb;

		if(!appSettingsPath.endsWith(kPathDelimiter))
			appSettingsPath << kPathDelimiter;
		appSettingsPath << "Settings";
		
		if ( stat(appSettingsPath, &sb) != -1 && S_ISDIR(sb.st_mode) )
		{
			CStr	prefsPath(GetMacCvsPrefsPath());
			if ( !prefsPath.empty() )
			{
				CStr	settingsPath(prefsPath);
				if ( !settingsPath.endsWith(kPathDelimiter) )
					settingsPath << kPathDelimiter;
				settingsPath << "Settings";
				
				if ( stat(settingsPath, &sb) == -1 )
				{
					// tell user we've discovered an old settings folder
					//
					LCFString		message("MacCvs has found a \"Settings\" folder in the old location (next to the MacCvs application). Do you want to move this folder to the new default location?");
					LCFString		explanation;
					explanation.Append("The new default \"Settings\" folder location is inside the \"MacCvs Preferences\" folder in your user preferences folder:\n\n");
					explanation.Append(prefsPath);
					explanation.Append("\n");

					AlertStdCFStringAlertParamRec		alertParams;
					if ( GetStandardAlertDefaultParams(&alertParams, kStdCFStringAlertVersionOne) == noErr )
					{
						DialogRef 	alert;
						
						alertParams.movable = true;
						alertParams.defaultText = CFSTR("Move Folder");
						alertParams.cancelText = CFSTR("Use old Location");
						//alertParams.otherText = CFSTR("Quit");
						alertParams.defaultButton = kAlertStdAlertOKButton;
						
						if ( CreateStandardAlert(kAlertCautionAlert, message, explanation, &alertParams, &alert) == noErr )
						{
							DialogItemIndex			itemHit;
							RunStandardAlert(alert, NULL, &itemHit);
							if ( itemHit == kAlertStdAlertOKButton )
							{
								pid_t		pid = fork();
								int			status;
								
								if ( pid == 0 )
								{
									int 	exec_stat = execlp("mv", "mv", (const char*)appSettingsPath, (const char*)settingsPath, NULL);
									exit(exec_stat);
								}
								
								if ( pid == -1 ) status = -1;
								else waitpid(pid, &status, 0);
								
								if ( status == 0 )
								{
									cvs_out("Successfully moved Settings folder from '%s' to '%s'\n", (const char*)appSettingsPath, (const char*)settingsPath);
								}
								else
								{
									alertParams.defaultText = (CFStringRef)kAlertDefaultOKText;
									alertParams.cancelText = NULL;
									alertParams.defaultButton = kAlertStdAlertOKButton;
									if ( CreateStandardAlert(kAlertStopAlert, 
																					 CFSTR("Sorry, moving the Settings folder failed."), NULL, &alertParams, &alert) == noErr )
										RunStandardAlert(alert, NULL, &itemHit);
								}
							}
							else if ( itemHit == kAlertStdAlertOtherButton )
								ThrowOSErr_(userCanceledErr);
							else gCvsPrefs.SetSettingsFolder(appSettingsPath);
						}
					}
				}
			}
		}
	}

	m_toolbar->GetFrameSize(m_undockedToolbarSize);
	this->DockToolbar(gToolbarDocked);
	if ( gToolbarVisible ) m_toolbar->Show();
	m_toolbar->UpdatePort();
	
	bool hasTCL = CTcl_Interp::IsAvail();
	bool hasPY = CPython_Interp::IsAvail();

	if(gCvsPrefs.GetUseShell() == SHELL_PYTHON && !hasPY && hasTCL)
	{
		gCvsPrefs.SetUseShell(SHELL_TCL);
	}
	else if(gCvsPrefs.GetUseShell() == SHELL_TCL && hasPY && !hasTCL)
	{
		gCvsPrefs.SetUseShell(SHELL_PYTHON);
	}

	if(gCvsPrefs.GetUseShell() == SHELL_TCL)
	{
		cvs_out("TCL is available, shell is enabled : help (select and press enter)\n");
	}

	if(!hasTCL && !hasPY)
	{
		cvs_err("TCL or Python are *not* available, shell is disabled\n");
	}

	// update the browser menu
	const	CMString::list_t&	pathList (sBrowserLoc.GetList());
	for (CMString::list_t::const_reverse_iterator i = pathList.rbegin (); i != pathList.rend (); ++i)
	{
		if (GUSIFileSpec (*i).Exists ()) continue;
		
		sBrowserLoc.Remove (*i);
	}

	// load the macros
	//MacrosReloadAll();
	//UpdateMacroMenus();

	//	Create the saved windows
	LWindow*	initialActive = 0;
	const CMWindowSpec::list_t& allSpecs = gBrowserPos.GetList();
	for (CMWindowSpec::list_t::const_iterator i = allSpecs.begin (); i != allSpecs.end (); ++i) {
		LWindow*	w = 0;
		
		if (i->pathIndex < 0) {
			w = mWindow;
			
			Rect b = i->globalBounds;
			if(b.right > b.left && b.bottom > b.top)
			{
				if(b.top < 80)
					::OffsetRect(&b, 0, 80 - b.top);
				w->DoSetBounds (b);
			}
			else
				cvs_err("Huh, wrong persistent bounds for the window...\n");
				
			if (i->visible) {
				w->Show ();
				w->UpdatePort ();
				} // if
				
			else w->Hide ();
			} // if
			
		else {
			//	Find the path
			if (i->pathIndex >= (sBrowserLoc.end () - sBrowserLoc.begin ())) continue;
			
			const UStr & tmpPath = sBrowserLoc.GetList()[i->pathIndex];
			w = NewBrowser (tmpPath, &*i, true);
			} // else
		
		if (i->active) initialActive = w;
		} // for
	
	mWindow->SetAutoShow(true);
	
	//	Build the browser menu 
	//	(and start all the window popups in the same place)
	UpdateBrowserMenu();
	
	if (initialActive) 
		initialActive->Select ();
	else mWindow->Show ();
	
	PyMacrosReloadAll();
	
	// warn if Python is not installed
	if(!CPython_Interp::IsAvail() && (bool)gWarnPython)
	{
		StDialogHandler	theHandler(dlg_NoPythonMachO, LCommander::GetTopCommander());
		LWindow *theDialog = theHandler.GetDialog();
		ThrowIfNil_(theDialog);
		
		theDialog->Show();
		MessageT hitMessage;
		while (true)
		{
			hitMessage = theHandler.DoDialog();
			
			if (hitMessage == msg_OK)
				break;
		}
		
		if(theDialog->GetValueForPaneID ('ps01') == Button_Off)
		{
			gWarnPython = false;
		}
		
		theDialog->Hide();
	}
	
	mWindow->SetSelectOnError(true); // don't automatically activate on python & tcl errors at startup
	
	// mouse wheel support
	AddAttachment( new CScrollWheelAttachment(true) );
}

void CMacCvsApp::DockToolbar(bool dockIt)
{
	if ( dockIt )
	{
  	GDHandle mainScreen = ::GetMainDevice();
  	Rect winRect = ((**mainScreen).gdRect);
  	
  	winRect.top = ::GetMBarHeight() - 1;
  	Rect outBounds;
  	m_toolbar->GetGlobalBounds(outBounds);
  	winRect.bottom = winRect.top + outBounds.bottom - outBounds.top;

  	Rect    oldBounds;
  	m_toolbar->GetGlobalBounds(oldBounds);
  	if ( oldBounds.top != winRect.top  ||  oldBounds.left != winRect.left )
  	{
  	  gToolbarPosition = *(Point*)&oldBounds;
  	  m_toolbar->MoveWindowTo(winRect.left, winRect.top);
  	}
  	m_toolbar->ResizeWindowTo(winRect.right - winRect.left, winRect.bottom - winRect.top);
  }
  else
  {
    Rect    boundsToCheck;
    
    *(Point*)&boundsToCheck = gToolbarPosition;
    boundsToCheck.right = boundsToCheck.left + m_undockedToolbarSize.width;
    boundsToCheck.bottom = boundsToCheck.top + 8; // only consider title bar
    
    // verify this is a valid bounds rect
    GDHandle  aScreen = ::GetDeviceList();
    bool      validPosition(false);
    while ( aScreen )
    {
      Rect  r;
      if ( GetAvailableWindowPositioningBounds(aScreen, &r) == noErr  &&  SectRect(&boundsToCheck, &r, &r) )
      {
        validPosition = true;
        break;
      }
      aScreen = GetNextDevice(aScreen);
    }
    // if it's valid, set the toolbar bounds
    if ( validPosition )
      m_toolbar->MoveWindowTo(boundsToCheck.left, boundsToCheck.top);
    m_toolbar->ResizeWindowTo(m_undockedToolbarSize.width, m_undockedToolbarSize.height);
  }
  gToolbarDocked = dockIt;
}

void CMacCvsApp::UpdateWindowsMenu()
{
	SInt32 i, numItems;
	LMenuBar *menuBar;
	LMenu *menu;
	LWindow *w;
	Str255 outName;
	
	menuBar	= LMenuBar::GetCurrentMenuBar();
	menu	= menuBar->FetchMenu(menu_Windows);
	Assert_(menu);
	if(menu == 0L)
		return;
	
	numItems = mWindowList.GetCount();
	
	// We have been called after a window creation or a window deletion.
	// So try to remove one more item than windows here.
	for(i = 0; i < numItems + 1; i++)
		menu->RemoveCommand(cmd_FirstWindow + i);
	

	TArrayIterator<LWindow*> iterator(mWindowList);
	
	for(i = 0; iterator.Next(w); i++)
	{
		w->GetDescriptor(outName);
		menu->InsertCommand(outName, cmd_FirstWindow + i, max_Int16 - 1);
		if ( i <= 9 ) SetMenuItemCommandKey(menu->GetMacMenuH(), menu->IndexFromCommand(cmd_FirstWindow+i), false, '0'+i);
	}

	LCommander::SetUpdateCommandStatus(true);
}

void CMacCvsApp::RemoveWindowFromListByView(LView * aView)
{
	while(aView != 0L && aView->GetSuperView() != 0L)
		aView = aView->GetSuperView();
	
	Assert_(aView != 0L);
	if(aView != 0L)
	{
		LWindow *wind = dynamic_cast<LWindow *>(aView);
		Assert_(wind != 0L);
		if(wind != 0L)
		{
			mWindowList.Remove(wind);
			UpdateWindowsMenu();
			
			if(wind == mWindow)
				mWindow = 0L;
		}
	}
}

void CMacCvsApp::ShowAboutBox()
{
  LWindow*   aboutWindow = LWindow::FindWindowByID(dlg_About);
  if ( aboutWindow ) aboutWindow->Select();
  else
  {
    aboutWindow = LWindow::CreateWindow(dlg_About, this);
    aboutWindow->SetPaneID(dlg_About);
    aboutWindow->Show();
  }
}

void CMacCvsApp::DoCredits(void)
{
	UStr cvers;
	GetAppName(cvers);

	cvs_out("\n%s - Client\n", cvers.c_str());
	cvs_out("MacCVS is maintained by Alexandre Parenteau & Jens Miltner\n");
	cvs_out("- MacCvs page : http://cvsgui.sourceforge.net\n");
	cvs_out("- cvs page : http://www.cvshome.org\n");
	cvs_out("- Mac port :\n");
	cvs_out("\t* Mike Ladwig <mike@twinpeaks.prc.com>\n");
	cvs_out("\t* Matthias Neeracher <neeri@iis.ee.ethz.ch>\n");
	cvs_out("\t* Alexandre Parenteau <aubonbeurre@hotmail.com>\n");
	cvs_out("- With contributions of :\n");
	cvs_out("\t* Richard Wesley <hawkfish@electricfish.com> (MacOSX, AppleEvents and more...)\n");
	cvs_out("\t* Miro Jurisic <meeroh@mit.edu> (Kerberos)\n");
	cvs_out("\t* Aleks Totic <atotic@netscape.com> (AppleSingle)\n");
	cvs_out("\t* Jocelyn Houle <houlejo@IRO.UMontreal.CA> (Help balloons)\n");
	cvs_out("- Special thanks to :\n");
	cvs_out("\t* The Strata developpers who are so patients with MacCVS\n");
	cvs_out("\t  and help to identify a lot of problems.\n");
	cvs_out("\t* Jack Jansen for MacPython.\n");
	cvs_out("- Bug reports, suggestions :\n");
	cvs_out("\t* Richard Wesley, Alain Aslag Roy, ƒric Aubourg and many others\n");
	cvs_out("\t  (see the ChangeLog)...\n");
	cvs_out("- Cvs contributors :\n");
	cvs_out("\t* Too many to be listed here, see the 'ChangeLog' instead !\n");
}

void CMacCvsApp::DoCopyright(void)
{
	cvs_out("\nBoth MacCVS and CVS are distributed under the terms of\n");
	cvs_out("the GNU General Public Licence (GPL).\n\n");
	cvs_out("* MacCVS : Mike Ladwig, Matthias Neeracher, Alexandre Parenteau\n");
	cvs_out("* MacCVS/WinCvs : maintained by Alexandre Parenteau (Strata Inc.)\n");
	cvs_out("* CVS : Copyright © 1989-2001 Brian Berliner, david d `zoo' zuhn,\n");
	cvs_out("        Jeff Polk, and other authors\n");
	cvs_out("* GUSI : Copyright © 1992-1997 Matthias Neeracher\n");
	cvs_out("* Kerberos : Copyright © 1997 the Massachusetts Institute of Technology\n");
	cvs_out("* WASTE Text Engine : Copyright © 1993-2000 Marco Piovanelli \n");
	cvs_out("* WTextView : Copyright © 1995-1998 Timothy Paustian (paustian@bact.wisc.edu)\n");
	cvs_out("* ABalloon : Freeware of James W. Walker (jwwalker@kagi.com)\n");
	cvs_out("* Metrowerks Standard Library, PowerPlant & CodeWarrior :\n");
	cvs_out("    Copyright © 1996-2003 Metrowerks Inc.\n");
	cvs_out("* MPW, MoreFiles : Copyright © Apple Computer Inc.\n");
	cvs_out("* TCL : Copyright © Sun Microsystems Inc.\n");
	cvs_out("* Python : Copyright © Guido van Rossum\n");
	cvs_out("\nSome others copyrights may apply, check the source code for details.\n");
}

Boolean CMacCvsApp::ObeyCommand(CommandT inCommand, void *ioParam)
{
	if(inCommand >= cmd_FirstWindow && inCommand < cmd_LastWindow)
	{
		LWindow	*w = 0L;
		if(mWindowList.FetchItemAt(inCommand - cmd_FirstWindow + 1, w))
		{
			if(!w->IsVisible())
			{
				w->Show();
				w->Select();
			}
			else if(!UDesktop::WindowIsSelected(w))
				w->Select();
		}
		else
			Assert_(0);
			
		return true;
	}
	
	if(inCommand >= cmd_FirstMacro && inCommand < cmd_LastMacro)
	{
		PyDoPython(inCommand);
		return true;
	}

	if(inCommand >= cmd_FirstBrowser && inCommand < cmd_LastBrowser)
	{
		const std::vector<CStr> & allPaths = sBrowserLoc.GetList();
		CStr newpath = allPaths[inCommand - cmd_FirstBrowser];
		if ( NewBrowser(newpath) != NULL )
  		UpdateBrowserMenu(newpath);
		return true;
	}

	switch(inCommand)
	{
	case cmd_Stop:
		SetCvsStopping(true);
		if(m_CvsProcess != 0L && cvs_process_is_active(m_CvsProcess))
		{
			cvs_process_stop(m_CvsProcess);
			cvs_out("*****CVS stopped on user request !!! *****\n\n");
		}
		break;
	case cmd_Credits :
                DoCredits();
		break;
	case cmd_Copyright :
		DoCopyright();
		break;
	case cmd_MacCvsPrefs :
		::CompatGetPrefs();
		break;
	case cmd_Checkout :
	{
		KoCheckoutHandler handler;
		handler.OnFolder(NULL);
		break;
	}
	case cmd_Import :
	{
		KoImportHandler handler;
		handler.OnFolder(NULL);
		break;
	}
	case cmd_Login :
		::CvsCmdLogin();
		break;
	case cmd_Logout :
		::CvsCmdLogout();
		break;
	case cmd_RtagCreate :
	{
		KoRtagCreateHandler handler;
		CvsCmdRtag(handler, kTagCreate);
		break;
	}
	case cmd_RtagDelete :
	{
		KoRtagDeleteHandler handler;
		CvsCmdRtag(handler, kTagDelete);
		break;
	}
	case cmd_RtagBranch :
	{
		KoRtagBranchHandler handler;
		CvsCmdRtag(handler, kTagBranch);
		break;
	}
	case cmd_CmdLine :
	{
		KoCommandLineHandler handler;
		CvsCmdCommandLine(handler);
		break;
	}
#if 0
	case cmd_UpdateFolder :
		::CvsCmdUpdateFolder();
		break;
	case cmd_CommitFolder :
		::CvsCmdCommitFolder();
		break;
	case cmd_UpdateFiles :
		::CvsCmdUpdateFiles();
		break;
	case cmd_CommitFiles :
		::CvsCmdCommitFiles();
		break;
	case cmd_DiffFiles :
		::CvsCmdDiffFiles();
		break;
	case cmd_LogFiles :
		::CvsCmdLogFiles();
		break;
	case cmd_AddFiles :
		::CvsCmdAddFiles();
		break;
	case cmd_AddBinaryFiles :
		::CvsCmdAddBinaryFiles();
		break;
	case cmd_RemoveFiles :
		::CvsCmdRemoveFiles();
		break;
	case cmd_StatusFiles :
		::CvsCmdStatusFiles();
		break;
	case cmd_CancelChanges :
		::CvsCmdCancelChangesFiles();
		break;
	case cmd_AddFolder :
		::CvsCmdAddFolder();
		break;
	case cmd_DiffFolder :
		::CvsCmdDiffFolder();
		break;
	case cmd_LogFolder :
		::CvsCmdLogFolder();
		break;
	case cmd_LockFiles :
		::CvsCmdLockFiles();
		break;
	case cmd_UnlockFiles :
		::CvsCmdUnlockFiles();
		break;
	case cmd_StatusFolder :
		::CvsCmdStatusFolder();
		break;
	case cmd_EditFiles :
		::CvsCmdEditFiles();
		break;
	case cmd_UneditFiles :
		::CvsCmdUneditFiles();
		break;
	case cmd_WatchOnFiles :
		::CvsCmdWatchOnFiles();
		break;
	case cmd_WatchOffFiles :
		::CvsCmdWatchOffFiles();
		break;
	case cmd_WatchersFiles :
		::CvsCmdWatchersFiles();
		break;
	case cmd_EditorsFiles :
		::CvsCmdEditorsFiles();
		break;
	case cmd_LockFolder :
		::CvsCmdLockFolder();
		break;
	case cmd_UnlockFolder :
		::CvsCmdUnlockFolder();
		break;
	case cmd_EditFolder :
		::CvsCmdEditFolder();
		break;
	case cmd_UneditFolder :
		::CvsCmdUneditFolder();
		break;
	case cmd_WatchOnFolder :
		::CvsCmdWatchOnFolder();
		break;
	case cmd_WatchOffFolder :
		::CvsCmdWatchOffFolder();
		break;
	case cmd_WatchersFolder :
		::CvsCmdWatchersFolder();
		break;
	case cmd_EditorsFolder :
		::CvsCmdEditorsFolder();
		break;
	case cmd_ReleaseFolder :
		::CvsCmdReleaseFolder();
		break;
	case cmd_TagCreateFolder :
		::CvsCmdTagCreateFolder();
		break;
	case cmd_TagDeleteFolder :
		::CvsCmdTagDeleteFolder();
		break;
	case cmd_TagBranchFolder :
		::CvsCmdTagBranchFolder();
		break;
	case cmd_TagCreateFiles :
		::CvsCmdTagCreateFiles();
		break;
	case cmd_TagDeleteFiles :
		::CvsCmdTagDeleteFiles();
		break;
	case cmd_TagBranchFiles :
		::CvsCmdTagBranchFiles();
		break;
#endif
	case cmd_NewBrowser :
	{
		CStr			dir;
		if ( BrowserGetDirectory("Select a directory to browse :", dir) )
		{
			if ( NewBrowser(dir, NULL, true) != NULL )
  			UpdateBrowserMenu(dir);
		}
		break;
	}
	case cmd_DockToolbar:
	{
	  this->DockToolbar(!gToolbarDocked);
	  break;
	}
	case cmd_ShowToolbar:
	  if ( m_toolbar->IsVisible() ) m_toolbar->Hide(); else m_toolbar->Show();
	  break;
	default:
		// Call inherited.
		return LDocApplication::ObeyCommand(inCommand, ioParam);
	}
	return true;
}

static EKeyStatus	MyFilterFunc(TEHandle			inMacTEH,
									   UInt16			inKeyCode,
									   UInt16			&ioCharCode,
									   EventModifiers	inModifiers)
{
	TEKeyFilterFunc keyFilter = UKeyFilters::SelectTEKeyFilter(keyFilter_PrintingChar);
	EKeyStatus status = keyFilter(inMacTEH, inKeyCode, ioCharCode, inModifiers);
	UInt16		theChar		 = (UInt16) (inKeyCode & charCodeMask);
	if(theChar == char_Enter || theChar == char_Return)
	{
		LCommander* target = LCommander::GetTarget();

		if (target != nil)
			target->ProcessCommand(cmd_FilterEdit, 0L);
	}
	
	return status;
}

LWindow* CMacCvsApp::NewBrowser(const char *dir, const CWindowSpec* inLoc, bool inForceNewBrowser)
{
  if ( !inForceNewBrowser ) 
  {
    // try to locate existing browser for same directory first
		for (WindowPtr	macWindowP = ::FrontWindow(); macWindowP != 0; macWindowP = ::GetNextWindow (macWindowP)) {
			LWindow*	theWindow = LWindow::FetchWindowObject(macWindowP);
			if (!theWindow->HasAttribute(windAttr_Regular)) continue;

			if (theWindow != mWindow) {
				CBrowserTable*	browser = dynamic_cast<CBrowserTable*> (theWindow->FindPaneByID (item_BrowserTable));
				if (!browser) continue;
				
				//	compare the path
				if ( strcmp(dir, browser->GetPath()) == 0 )
				{
				  // that's a browser for this path, so just activate it
				  theWindow->Select();
				  return theWindow;
				}
			}
		}
  }
  
	CBrowserWind *theWindow = dynamic_cast<CBrowserWind*> (LWindow::CreateWindow(wind_Browser, this));
	ThrowIfNil_(theWindow);
	
	// update the browser menu
	AddListener (theWindow);
	if ( !theWindow->LocateBrowser (dir) )
	{
	  // locating the browser failed (e.g. user refused to update old-style sandbox)
	  // -> clean up again and get rid of window
	  //
	  RemoveListener(theWindow);
	  delete theWindow;
	  return NULL;
	}
		
	LEditText *edit = dynamic_cast<LEditText*>
		(theWindow->FindPaneByID(cmd_FilterEdit));
	if(edit != 0L)
		edit->SetKeyFilter(MyFilterFunc);
	
	// adjust size 
	if (inLoc) {
		theWindow->DoSetBounds (inLoc->globalBounds);
			
		if (inLoc->visible) 
			theWindow->Show ();
		else theWindow->Hide ();
		} // if
		
	else {
		static int offset = 1;
		Rect	windowRect;
		theWindow->CalcStandardBounds(windowRect);
		if((windowRect.right - windowRect.left) >= 640)
			windowRect.right = windowRect.left + 640;
		if((windowRect.bottom - windowRect.top) >= 400)
			windowRect.bottom = windowRect.top + 400;
		
		OffsetRect(&windowRect, 25 * offset, 25 * offset);
		offset++;
		theWindow->DoSetBounds(windowRect);
		
		theWindow->Show();
		} // else
	
	mWindowList.InsertItemsAt(1, LArray::index_Last, theWindow);
	UpdateWindowsMenu();
	
	return theWindow;
}

LWindow* CMacCvsApp::NewBrowser(const UFSSpec & spec, const CWindowSpec* inLoc, bool inForceNewBrowser)
{
	GUSIFileSpec in(spec);
	const char *dir = in.FullPath();
	if(!in.Exists())
	{
		cvs_err("Error while opening a new browser (file doesn't exist)\n");
		return 0;
	}
	
	if(in.CatInfo()->IsFile())
	{
		CStr uppath, file;
		SplitPath(dir, uppath, file);
		return NewBrowser(uppath, inLoc, inForceNewBrowser);
	}
	
	return NewBrowser(dir, inLoc, inForceNewBrowser);
}

void CMacCvsApp::UpdateBrowserMenu(const char* dir)
{
	SInt32 i, numItems;
	LMenuBar *menuBar;
	LMenu *menu;
	
	if (dir) sBrowserLoc.Insert(dir);

	menuBar	= LMenuBar::GetCurrentMenuBar();
	menu	= menuBar->FetchMenu(menu_RecentBrowser);
	Assert_(menu);
	if(menu == 0L)
		return;
	
	std::vector<CStr>::const_iterator j;
	const std::vector<CStr> & allPaths = sBrowserLoc.GetList();
	numItems = allPaths.size();
	
	// We have been called after a window creation or a window deletion.
	// So try to remove one more item than windows here.
	for(i = 0; i < numItems + 1; i++)
		menu->RemoveCommand(cmd_FirstBrowser + i);
	
	//	Create list of names
	std::vector<CStr>	allNames;
	for(j = allPaths.begin(); j != allPaths.end(); ++j)
	{
		CStr	path;
		CStr	name;
		SplitPath(*j,  path, name);
		allNames.insert (allNames.end (), name);
	}
	
	//	Create list of menu titles
	std::vector<CStr>	menuNames;
	std::vector<CStr>::const_iterator k;
	for(k = allNames.begin(); k != allNames.end(); ++k)
	{
		if (1 == std::count (allNames.begin(), allNames.end(), *k))
			menuNames.insert(menuNames.end (), k->c_str ());
		else menuNames.insert(menuNames.end (), allPaths[k-allNames.begin()]);
	}
	
	for(i = 0, j = menuNames.begin(); j != menuNames.end(); ++j, ++i)
	{
		menu->InsertCommand(Str_Dummy, cmd_FirstBrowser + i, max_Int16 - 1);
		::SetMenuItemText(menu->GetMacMenuH(), ::CountMenuItems(menu->GetMacMenuH()), UTF8_to_LString(*j));
	}
	
	BroadcastMessage (msg_UpdateBrowserMenu, (void*) dir);
	
	LCommander::SetUpdateCommandStatus(true);
}

void CMacCvsApp::UpdateMenus (void)
{ // begin UpdateMenus	
	LDocApplication::UpdateMenus ();
	
#if PP_Uses_Aqua_MenuBar
	if (UEnvironment::HasFeature(env_HasAquaTheme))
	{
		Boolean	outEnabled;
		Boolean	outUsesMark;
		UInt16	outMark;
		Str255	outName;
		
		LCommander::GetTarget ()->ProcessCommandStatus (cmd_MacCvsPrefs, outEnabled, outUsesMark, outMark, outName);
		if (outEnabled)
			EnableMenuCommand (nil, kHICommandPreferences);
		else DisableMenuCommand (nil, kHICommandPreferences);

		LCommander::GetTarget ()->ProcessCommandStatus (cmd_Quit, outEnabled, outUsesMark, outMark, outName);
		if (outEnabled)
			EnableMenuCommand (nil, kHICommandQuit);
		else DisableMenuCommand (nil, kHICommandQuit);
	} // if
#endif

} // end UpdateMenus

void CMacCvsApp::NewLogTree(CLogNode *node, const char *dir)
{
	Assert_(node != 0L && dir != 0L);
	Assert_(node->GetType() == kNodeHeader);
	
	if(node->GetType() != kNodeHeader)
		return;
	
	CLogNodeHeader *header = (CLogNodeHeader *)node;
	LStr255 title(UTF8_to_LString((**header).WorkingFile()));

	LWindow *theWindow = LWindow::CreateWindow(wind_LogTree, this);
	ThrowIfNil_(theWindow);
	
	// set the window's title to the volume name			
	theWindow->Show();
	theWindow->SetDescriptor(title);
	
	CTreeView *tree = dynamic_cast<CTreeView*>
		(theWindow->FindPaneByID(item_TreeView));
	ThrowIfNil_(tree);
	
	tree->SetNode(node, dir);

	mWindowList.InsertItemsAt(1, LArray::index_Last, theWindow);
	UpdateWindowsMenu();
}
	
void CMacCvsApp::RefreshBrowsers(bool forceReload)
{
	LWindow*	theWindow = UDesktop::FetchTopRegular();
	if(theWindow == 0L)
		return;
	
	WindowPtr	macWindowP = theWindow->GetMacWindow();

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != 0L)
	{
		CBrowserTable *browse = dynamic_cast<CBrowserTable*>
			(theWindow->FindPaneByID(item_BrowserTable));
		if(browse != 0L)
			browse->ResetBrowser(forceReload ||
				(theWindow == UDesktop::FetchTopRegular() && browse->GetRecursionModel()->IsShowRecursive())
			);
		macWindowP = GetNextWindow(macWindowP);
	}
	
	FlushValidPasswords();
}

void CMacCvsApp::CloseBrowsers(void)
{
	LWindow*	theWindow = UDesktop::FetchTopRegular();
	if(theWindow == 0L)
		return;
	
	WindowPtr	macWindowP = theWindow->GetMacWindow();

	while ((theWindow = LWindow::FetchWindowObject(macWindowP)) != 0L)
	{
		theWindow->DoClose();

		macWindowP = GetNextWindow(macWindowP);
	}
}

void CMacCvsApp::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	if(inCommand < 0) // sub popup menus
	{
		outEnabled = true;
		return;
	}
	
	if(inCommand >= cmd_FirstMacro && inCommand < cmd_LastMacro)
	{
		if(CMacCvsApp::gApp->IsCvsRunning())
		{
			outEnabled = false;
			return;
		}
		
		UPseudoCmdStatusWrapper wrapper;
		wrapper.outEnabled = &outEnabled;
		wrapper.outUsesMark = &outUsesMark;
		wrapper.outMark = &outMark;
		wrapper.outName = outName;
		UCmdUI cmdui(inCommand);
		cmdui.m_wrapper = &wrapper;
		PyDoCmdUI(&cmdui);
		return;
	}

	if(inCommand >= cmd_FirstWindow && inCommand < cmd_LastWindow)
	{
		LWindow	*w = 0L;
		outEnabled = mWindowList.FetchItemAt(inCommand - cmd_FirstWindow + 1, w);
		Assert_(outEnabled);
		
		if(w != 0L)
		{
			outUsesMark = true;
			outMark = UDesktop::WindowIsSelected(w) ? kMenuCheckmarkGlyph : (IsWindowCollapsed(w->GetMacWindow()) ? kMenuDiamondGlyph : ' ');
		}
		return;
	}

	if(inCommand >= cmd_FirstBrowser && inCommand < cmd_LastBrowser)
	{
		outEnabled = true;
		return;
	}

	switch(inCommand)
	{
	case cmd_Credits :
	case cmd_Copyright :
	case cmd_MacCvsPrefs :
	case cmd_NewBrowser :
		outEnabled = true;
		break;
	case cmd_DockToolbar:
	  outEnabled = true;
		outUsesMark = true;
	  outMark = gToolbarDocked ? kMenuCheckmarkGlyph : 0;
	  break;
	case cmd_ShowToolbar:
	  outEnabled = true;
		outUsesMark = true;
	  outMark = m_toolbar->IsVisible() ? kMenuCheckmarkGlyph : 0;
	  break;
	case cmd_Stop:
		outEnabled = IsCvsRunning() && !IsCvsStopping();
		break;
	case cmd_Checkout :
	case cmd_Import :
	case cmd_Login :
	case cmd_Logout :
	case cmd_RtagCreate :
	case cmd_RtagDelete :
	case cmd_RtagBranch :
#if 0
	case cmd_CmdLine :
	case cmd_UpdateFolder :
	case cmd_CommitFolder :
	case cmd_UpdateFiles :
	case cmd_CommitFiles :
	case cmd_DiffFiles :
	case cmd_LogFiles :
	case cmd_AddFiles :
	case cmd_AddBinaryFiles :
	case cmd_RemoveFiles :
	case cmd_StatusFiles :
	case cmd_CancelChanges :
	case cmd_AddFolder :
	case cmd_DiffFolder :
	case cmd_LogFolder :
	case cmd_LockFiles :
	case cmd_UnlockFiles :
	case cmd_StatusFolder :
	case cmd_EditFiles :
	case cmd_UneditFiles :
	case cmd_WatchOnFiles :
	case cmd_WatchOffFiles :
	case cmd_WatchersFiles :
	case cmd_EditorsFiles :
	case cmd_LockFolder :
	case cmd_UnlockFolder :
	case cmd_EditFolder :
	case cmd_UneditFolder :
	case cmd_WatchOnFolder :
	case cmd_WatchOffFolder :
	case cmd_WatchersFolder :
	case cmd_EditorsFolder :
	case cmd_ReleaseFolder :
	case cmd_TagCreateFolder :
	case cmd_TagDeleteFolder :
	case cmd_TagBranchFolder :
	case cmd_TagCreateFiles :
	case cmd_TagDeleteFiles :
	case cmd_TagBranchFiles :
#endif
		outEnabled = !gCvsPrefs.empty() && !CMacCvsApp::gApp->IsCvsRunning();
		break;

	default:
		// Call inherited.
		LDocApplication::FindCommandStatus( inCommand,
			outEnabled, outUsesMark, outMark, outName );
		break;
	}
}

static void updateQuoteFontPreview(LWindow *theWindow)
{
	// disable/emable the OK button if the field is empty
	
	LPopupButton *names = dynamic_cast<LPopupButton*>
								(theWindow->FindPaneByID(item_FontNames));
	LPopupButton *sizes = dynamic_cast<LPopupButton*>
								(theWindow->FindPaneByID(item_FontSizes));
	LEditText *quote = dynamic_cast<LEditText*>
								(theWindow->FindPaneByID(item_FontQuote));
	
	if(names == 0L || sizes == 0L || quote == 0L)
	{
		Assert_(0);
		return;
	}
	
	SInt16 curifont = names->GetCurrentMenuItem();
	SInt16 curisize = sizes->GetCurrentMenuItem();
	
	if(curifont <= 0 || curisize <= 0)
	{
		Assert_(0);
		return;
	}
	
	Str255 fontn;
	short fontid;
	
	names->GetMenuItemText(curifont, fontn);
	::GetFNum(fontn, &fontid);
	
	TEHandle te = quote->GetMacTEH();
	switch(curisize)
	{
		case 1: (**te).txSize = 9; break;
		case 2: (**te).txSize = 10; break;
		case 3: (**te).txSize = 12; break;
		case 4: (**te).txSize = 14; break;
		case 5: (**te).txSize = 16; break;
		case 6: (**te).txSize = 18; break;
	}
	(**te).txFont = fontid;
	
	// Set line spacing based on Font
	FMInput	fontSpecs;
	fontSpecs.family	= (**te).txFont;
	fontSpecs.size		= (**te).txSize;
	fontSpecs.face		= (**te).txFace;
	fontSpecs.needBits	= false;
	fontSpecs.device	= 0;
	fontSpecs.numer.h	= 1;
	fontSpecs.numer.v	= 1;
	fontSpecs.denom.h	= 1;
	fontSpecs.denom.v	= 1;
	
	FMOutPtr	info = ::FMSwapFont(&fontSpecs);
	
	(**te).lineHeight = info->ascent + info->descent + info->leading;
	(**te).fontAscent = info->ascent;
	::TECalText(te);
	quote->Refresh();
}

void CMacCvsApp::ListenToMessage(MessageT inMessage, void *ioParam)
{
	LWindow *theWindow = UDesktop::FetchTopModal();
	if ( theWindow == nil )
		return;
	
	switch (inMessage)
	{
		case item_ModuleName:
		{
			// disable/emable the OK button if the field is empty
			
			LPushButton *theOK = dynamic_cast<LPushButton*>
										(theWindow->FindPaneByID(item_OK));
			LEditText *mname = dynamic_cast<LEditText*>
				(theWindow->FindPaneByID(item_ModuleName));
			if(theOK == 0L || mname == 0L)
			{
				Assert_(0);
				return;
			}
			
			char buf[3];
			Size len;
			mname->GetText(buf, 2, &len);
			
			theOK->SetEnabled(len > 0);
			break;
		}
		case item_FontNames:
		case item_FontSizes:
			updateQuoteFontPreview(theWindow);
			break;
		case item_MacBinEdit:
		case item_MacBinAdd:
		case item_MacBinDelete:
			DoHitButton_BinFiles(theWindow, inMessage);
			break;
	}
}

void CMacCvsApp::MakeModelDirector()
{
	NEW LMacCVSModelDirector(this);
}

void CMacCvsApp::AdjustCursor(const EventRecord& inMacEvent)
{
    if ( this->IsCvsRunning() ) this->DoSpinCursor();
    else LApplication::AdjustCursor(inMacEvent);
}

void CMacCvsApp::ProcessNextEvent()
{
//printf("%.3lf - CMacCvsApp::ProcessNextEvent\n", GetCurrentEventTime());
	while (!gPendingAEQueue.IsEmpty () && !IsCvsRunning())
	{
		// There is an AppleEvent pending on the queue, handle it
		AppleEvent aev, reply;
		gPendingAEQueue.DequeueEvent(aev, reply);
		AEResumeTheCurrentEvent (&aev, &reply , (AEEventHandlerUPP) kAEUseStandardDispatch, 0);
	}

	LApplication::ProcessNextEvent();
}

void CMacCvsApp::DispatchEvent	(const EventRecord& inMacEvent)
{
	if(inMacEvent.what == mouseDown)
	{
		WindowPtr	macWindowP;
		SInt16		thePart = ::MacFindWindow(inMacEvent.where, &macWindowP);
		if(thePart != inMenuBar)
			PyInvalUICache();
	}
	else if(inMacEvent.what == keyDown)
	{
		PyInvalUICache();
	}

	LApplication::DispatchEvent(inMacEvent);
}

void CMacCvsApp::RefreshLogFont(void)
{
	if(mWindow == 0L)
		return;
	WTextView *we = mWindow->GetWE();
	if(we == 0L)
		return;
	we->SelectAll();
	TextStyle newStyle;
	newStyle.tsFont = gLogFont;
	newStyle.tsSize = gLogSize;
	we->SetStyle(weDoFont | weDoSize, newStyle);
	we->SetTextSelection(max_Int32, max_Int32);
}

void DoDataExchange (LView *theDialog, PaneIDT paneID, bool & ioValue, bool putValue)
{
	if (putValue)
		theDialog->SetValueForPaneID (paneID, ioValue ? Button_On : Button_Off);
	else ioValue = theDialog->GetValueForPaneID(paneID) == Button_On;
}

void DoDataExchange (LView *theDialog, PaneIDT paneID, SInt32 & ioValue, bool putValue)
{
	if (putValue)
		theDialog->SetValueForPaneID (paneID, ioValue);
	else ioValue = theDialog->GetValueForPaneID(paneID);
}

void DoDataExchange (LView *theDialog, PaneIDT paneID, UPStr & ioValue, bool putValue)
{
	Str255	str;
	
	if (putValue) {
		::BlockMoveData ((Ptr)(const char *)ioValue, str + 1, str[0] = ioValue.length());
		theDialog->SetDescriptorForPaneID (paneID, str);
		} // if
		
	else {
		theDialog->GetDescriptorForPaneID(paneID, str);
		ioValue.set ((const char*)str + 1, str[0]);
		} // else
}

void DoDataExchange (LView *theDialog, PaneIDT paneID, UStr & ioValue, bool putValue)
{
	Str255	str;
	
	if (putValue) {
		::BlockMoveData ((Ptr)(const char *)ioValue, str + 1, str[0] = ioValue.length());
		theDialog->SetDescriptorForPaneID (paneID, str);
		} // if
		
	else {
		theDialog->GetDescriptorForPaneID(paneID, str);
		ioValue.set ((const char*)str + 1, str[0]);
		} // else
}

void DoDataExchange_Globals(LWindow *theDialog, bool putValue)
{
	bool	checkoutRO = gCvsPrefs.CheckoutRO();
	bool	pruneDir = gCvsPrefs.PruneOption();
	bool	tcpCompress = gCvsPrefs.Z9Option();
	bool	quietMsg = gCvsPrefs.QuietOption();
	bool	addControl = gCvsPrefs.AddControl();
	bool	dirtySupport = gCvsPrefs.DirtySupport();
	SInt32	zLevel = gCvsPrefs.ZLevel();
	SInt32	autoLogoutTime = gCvsPrefs.LogoutTimeOut();
	bool	autoLogout = autoLogoutTime > 0;
	bool	unixLF = gCvsPrefs.MacLF();

	if (putValue)
	{
		DoDataExchange (theDialog, item_CheckoutRO, checkoutRO, putValue);
		DoDataExchange (theDialog, item_PruneDir, pruneDir, putValue);
		DoDataExchange (theDialog, item_TCPCompress, tcpCompress, putValue);
		DoDataExchange (theDialog, item_QuietMsg, quietMsg, putValue);
		DoDataExchange (theDialog, item_AddControl, addControl, putValue);
		DoDataExchange (theDialog, item_DirtySupport, dirtySupport, putValue);
		DoDataExchange (theDialog, item_ZLevel, zLevel, putValue);
		DoDataExchange (theDialog, item_AutoLogout, autoLogout, putValue);
		DoDataExchange (theDialog, item_AutoLogoutTime, autoLogoutTime, putValue);
		DoDataExchange (theDialog, item_UnixLF, unixLF, putValue);
	}
	
	else
	{
		bool newValue;
		
		DoDataExchange (theDialog, item_CheckoutRO, newValue, putValue);
		if(newValue != checkoutRO)
			gCvsPrefs.SetCheckoutRO(newValue);
		
		DoDataExchange (theDialog, item_PruneDir, newValue, putValue);
		if(newValue != pruneDir)
			gCvsPrefs.SetPruneOption(newValue);
		
		DoDataExchange (theDialog, item_TCPCompress, newValue, putValue);
		if(newValue != tcpCompress)
			gCvsPrefs.SetZ9Option(newValue);
		
		DoDataExchange (theDialog, item_QuietMsg, newValue, putValue);
		if(newValue != quietMsg)
			gCvsPrefs.SetQuietOption(newValue);
		
		DoDataExchange (theDialog, item_AddControl, newValue, putValue);
		if(newValue != addControl)
			gCvsPrefs.SetAddControl(newValue);
		
		DoDataExchange (theDialog, item_DirtySupport, newValue, putValue);
		if(newValue != dirtySupport)
			gCvsPrefs.SetDirtySupport(newValue);
		
		DoDataExchange (theDialog, item_UnixLF, newValue, putValue);
		if(newValue != unixLF)
			gCvsPrefs.SetMacLF(newValue);
		
		SInt32 newInt;
		
		DoDataExchange (theDialog, item_ZLevel, newInt, putValue);
		if(newInt != zLevel)
			gCvsPrefs.SetZLevel(newInt);
		
		DoDataExchange (theDialog, item_AutoLogout, newValue, putValue);
		if (newValue)
			DoDataExchange (theDialog, item_AutoLogoutTime, newInt, putValue);
		else newInt = 0;
		
		if(newInt != autoLogoutTime)
			gCvsPrefs.SetLogoutTimeOut(newInt);
	}
}

void SetupHandler_CVSRoot(LWindow* theDialog, StDialogHandler& inHandler)
{
	LControl*   control;
	control = dynamic_cast<LControl*> (theDialog->FindPaneByID (item_ForceCVSRoot));
	if ( control ) control->AddListener (&inHandler);
}

void DoDataExchange_CVSRoot(LWindow *theDialog, bool& forceCvsroot, UStr& cvsroot, bool putValue)
{
	DoDataExchange (theDialog, item_ForceCVSRoot, forceCvsroot, putValue);
	DoDataExchange (theDialog, item_CVSRoot, cvsroot, putValue);
	if ( putValue ) HandleHitMessage_CVSRoot(theDialog, item_ForceCVSRoot);
}

bool HandleHitMessage_CVSRoot(LWindow *theDialog, MessageT hitMessage)
{
  switch ( hitMessage ) {
    case item_ForceCVSRoot:
    {
      // enable / disable CVSROOT text box
      LPane*  cvsroot = theDialog->FindPaneByID(item_CVSRoot);
      LPane*  forceCvsroot = theDialog->FindPaneByID(item_ForceCVSRoot);
      if ( cvsroot  &&  forceCvsroot ) {
        if ( forceCvsroot->GetValue() != Button_Off )
          cvsroot->Enable();
        else
          cvsroot->Disable();
      }
    }
    return true;
    
    case item_CVSRootWizard:
      ///\todo cvsroot wizard
      return true;
  }
  
  return false;
}


static 	SInt32    sLastDiffToolValue(0);
static  bool      sAllowExtDiff(true);
void DoDataExchange_DiffTool(LWindow *theDialog, UStr& diffTool, bool& extDiff, bool allowExtDiff, bool putValue)
{
	LMenuController *usediff = dynamic_cast<LMenuController*>
		(theDialog->FindPaneByID(item_UseExtDiff));
	LEditText *editdiff = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditExtDiff));
	LPane   *useExtDiff = theDialog->FindPaneByID(item_DiffExternal);
	LPane   *editDiffButton = theDialog->FindPaneByID(item_BrowseExtDiff);
	
	if ( putValue ) 
	{
	  SInt32    diffToolPopupValue(1);
    if ( diffTool == kDiffTool_CodeWarrior )
      diffToolPopupValue = diff_CodeWarrior;
    else if ( diffTool == kDiffTool_BBEdit )
      diffToolPopupValue = diff_BBEdit;
    else if ( diffTool == kDiffTool_FileMerge )
      diffToolPopupValue = diff_FileMerge;
    else {
      diffToolPopupValue = diff_Custom;
      if ( editdiff ) editdiff->SetDescriptor(UTF8_to_LString(diffTool));
	  }
	  if ( usediff ) usediff->SetCurrentMenuItem(diffToolPopupValue);
	  if ( diffToolPopupValue != 4 ) {
      LPane *diffToolContainer(theDialog->FindPaneByID(item_EditExtDiffView));
      if ( diffToolContainer ) diffToolContainer->Disable();
	  }
	  if ( allowExtDiff ) {
  	  if ( useExtDiff ) useExtDiff->SetValue(extDiff ? Button_On : Button_Off);
    } else {
      // disable all external diff related panes
      if ( useExtDiff ) {
        useExtDiff->SetValue(Button_Off);
        useExtDiff->Disable();
      }
      LPane *diffToolContainer(theDialog->FindPaneByID(item_EditExtDiffView));
      if ( diffToolContainer ) diffToolContainer->Disable();
    }
    sLastDiffToolValue = diffToolPopupValue;
    sAllowExtDiff = allowExtDiff;
	} 
	else
	{
		if ( usediff ) {
		  SInt32    diffToolPopupValue(usediff->GetCurrentMenuItem());
		  switch ( diffToolPopupValue ) {
		    case diff_CodeWarrior:
		      diffTool = kDiffTool_CodeWarrior;
		      break;
		    case diff_BBEdit:
		      diffTool = kDiffTool_BBEdit;
		      break;
		    case diff_FileMerge:
		      diffTool = kDiffTool_FileMerge;
		      break;
		    default:
      		if ( editdiff ) {
        		LStr255   s;
      		  editdiff->GetText(s);
      		  diffTool = LString_to_UTF8(s);
      		}
      		break;
      }
      if ( diffTool.empty() ) extDiff = false;
    }
	  if ( allowExtDiff && useExtDiff ) extDiff = useExtDiff->GetValue() == Button_On;
	}
}

bool HandleHitMessage_DiffTool(LWindow *theDialog, MessageT hitMessage)
{
	if (hitMessage == item_BrowseExtDiff)
	{
		MultiFiles mf;
		if( BrowserGetMultiFiles("Select the external diff application", mf, true, kSelectExecutable) )
		{
			CvsArgs args(false);
			const char *dir = (*mf.begin()).add(args);
			char * const *argv = args.Argv();
			UStr fullpath;
			fullpath = dir;
			if(!fullpath.endsWith(kPathDelimiter))
				fullpath << kPathDelimiter;
			fullpath << argv[0];

			LEditText *editdiff = dynamic_cast<LEditText*>
				(theDialog->FindPaneByID(item_EditExtDiff));
			editdiff->SetText(UTF8_to_LString(fullpath.c_str()));
		}
		return true;
	}
	else if ( hitMessage == item_DiffExternal )
	{
	  LPane*  useExtDiff = theDialog->FindPaneByID(item_DiffExternal);
	  
	  if ( useExtDiff ) { 
	    LPane*  customDiffView = theDialog->FindPaneByID(item_EditExtDiffView);
	    LPane*  diffTool = theDialog->FindPaneByID(item_UseExtDiff);
	    
	    if ( useExtDiff->GetValue() == Button_On  &&  sLastDiffToolValue == diff_Custom  &&  sAllowExtDiff ) {
	      if ( customDiffView ) customDiffView->Enable();
	    } else {
	      if ( customDiffView ) customDiffView->Disable();
	    }
	    if ( useExtDiff->GetValue() == Button_On ) {
	      if ( diffTool ) diffTool->Enable();
	    } else {
	      if ( diffTool ) diffTool->Disable();
	    }
	  }
	}
	else if ( hitMessage == /*item_UseExtDiff*/0 /*popup group box doesn't send value message?*/ )
	{
	  LPane*  useExtDiff = theDialog->FindPaneByID(item_UseExtDiff);
    if ( useExtDiff  &&  useExtDiff->GetValue() != sLastDiffToolValue )
    {
      sLastDiffToolValue = useExtDiff->GetValue();
	    LPane*  customDiffView = theDialog->FindPaneByID(item_EditExtDiffView);
		  if ( sLastDiffToolValue != diff_Custom  ||  !sAllowExtDiff )
		  {
		    if ( customDiffView ) customDiffView->Disable();
		  }
		  else
		  {
		    if ( customDiffView ) customDiffView->Enable();
		  }
		  return true;
    }
	}
	return false;
}


void DoDataExchange_TextFiles(LWindow *theDialog, bool putValue)
{ 
	LCheckBox *chkICOnText = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_ICOnText));
	LPopupButton *popISO = dynamic_cast<LPopupButton*>
		(theDialog->FindPaneByID(item_ISOConvert));

  CStr    diffTool(gCvsPrefs.ExtDiff());
  bool    extDiff(gUseExtDiff);
  
  if(putValue)
	{
	  DoDataExchange_DiffTool(theDialog, diffTool, extDiff, true, true);
		chkICOnText->SetValue(gCvsPrefs.ICTextOption() ? Button_On : Button_Off);
		popISO->SetCurrentMenuItem((SInt16)gCvsPrefs.IsoConvert() + 1);
	}
	else
	{
		gCvsPrefs.SetICTextOption(chkICOnText->GetValue() == Button_On);
		gCvsPrefs.SetIsoConvert((ISO8559)(popISO->GetCurrentMenuItem() - 1));
	  DoDataExchange_DiffTool(theDialog, diffTool, extDiff, true, false);
		gCvsPrefs.SetExtDiff(diffTool);
	}
}

void DoDataExchange_BinFiles(LWindow *theDialog, MacBinMaps & maps, bool putValue)
{
	LPopupButton *popEncod = dynamic_cast<LPopupButton*>
		(theDialog->FindPaneByID(item_MacBinPopUp));
	LPushButton *but1 = dynamic_cast<LPushButton*>
		(theDialog->FindPaneByID(item_MacBinEdit));
	LPushButton *but2 = dynamic_cast<LPushButton*>
		(theDialog->FindPaneByID(item_MacBinAdd));
	LPushButton *but3 = dynamic_cast<LPushButton*>
		(theDialog->FindPaneByID(item_MacBinDelete));
	LCheckBox *chk1 = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_MacBinOnlyBinaries));

	if(putValue)
	{
		popEncod->SetCurrentMenuItem((SInt16)gCvsPrefs.MacBinEncoding() + 1);
		but1->AddListener(CMacCvsApp::gApp);
		but2->AddListener(CMacCvsApp::gApp);
		but3->AddListener(CMacCvsApp::gApp);
		chk1->SetValue(gCvsPrefs.MacBinEncodeBinariesOnly() ? Button_On : Button_Off);
	}
	else
	{
		gCvsPrefs.SetMacBinEncoding((MACENCODING)(popEncod->GetCurrentMenuItem() - 1));
		gCvsPrefs.SetMacBinEncodeBinariesOnly(chk1->GetValue() == Button_On);
	}
}

void DoHitButton_BinFiles(LWindow *theDialog, MessageT hitMessage)
{
	CMacBinTable *tab = dynamic_cast<CMacBinTable*>
		(theDialog->FindPaneByID(item_MacBinList));
	CMacBinItem *item;
	STableCell	cell = tab->GetFirstSelectedCell();
	
	if(hitMessage == item_MacBinEdit)
	{
		// simulate dbl click
		if(!cell.IsNullCell())
		{
			item = dynamic_cast<CMacBinItem*>(tab->FindItemForRow(cell.row));
			Assert_(item != 0L);
			if(item != 0L)
				item->EditCell();
		}
	}
	else if(hitMessage == item_MacBinAdd)
	{
		const MACBINMAP *entry = tab->GetCurrentMap()->AddEntry(
			'\?\?\?\?', NULL, "New Entry", MAC_HQX, true);
		if(entry != 0L)
		{
			item = NEW CMacBinItem(*entry);
			ThrowIfNil_(item);
		
			// and insert it at the end of the table
			tab->InsertItem(item, nil, nil);
			if(!item->EditCell())
			{
				item->RemoveCell();
				tab->RemoveItem(item);
			}
		}
	}
	else if(hitMessage == item_MacBinDelete)
	{
		if(!cell.IsNullCell())
		{
			item = dynamic_cast<CMacBinItem*>(tab->FindItemForRow(cell.row));
			Assert_(item != 0L);
			if(item != 0L)
			{
				item->RemoveCell();
				tab->RemoveItem(item);
			}
		}
	}
}

void DoDataExchange_Fonts(LWindow *theDialog, bool putValue)
{
	LPopupButton *names = dynamic_cast<LPopupButton*>
								(theDialog->FindPaneByID(item_FontNames));
	LPopupButton *sizes = dynamic_cast<LPopupButton*>
								(theDialog->FindPaneByID(item_FontSizes));
	
	if(putValue)
	{
		MenuHandle fontMenu = names->GetMacMenuH();
		short numfonts = ::CountMenuItems(fontMenu), fontid, i;
		bool foundIt = false;
		for(i = 1; i <= numfonts; i++)
		{
			Str255 pname;
			::GetMenuItemText (fontMenu, i, pname);
			::GetFNum(pname, &fontid);
			if(fontid == (short)gLogFont)
			{
				names->SetCurrentMenuItem(i);
				foundIt = true;
				break;
			}
		}
		
		if(!foundIt)
		{
			for(i = 1; i <= numfonts; i++)
			{
				Str255 pname;
				::GetMenuItemText (fontMenu, i, pname);
				::GetFNum(pname, &fontid);
				if(fontid == kFontIDMonaco)
				{
					names->SetCurrentMenuItem(i);
					foundIt = true;
					break;
				}
			}
		}
		Assert_(foundIt);

		MenuHandle sizeMenu = sizes->GetMacMenuH();
		switch((short)gLogSize)
		{
			default:
			case 9: i = 1; break;
			case 10: i = 2; break;
			case 12: i = 3; break;
			case 14: i = 4; break;
			case 16: i = 5; break;
			case 18: i = 6; break;
		}
		sizes->SetCurrentMenuItem(i);

		// add the listeners and broadcast 'item_FontNames'
		// in order to update the quote
		names->AddListener(CMacCvsApp::gApp);
		sizes->AddListener(CMacCvsApp::gApp);
		updateQuoteFontPreview(theDialog);
	}
	else
	{
		SInt16 curifont = names->GetCurrentMenuItem();
		SInt16 curisize = sizes->GetCurrentMenuItem();
		
		if(curifont <= 0 || curisize <= 0)
		{
			Assert_(0);
			return;
		}
		
		Str255 fontn;
		short fontid;
		
		names->GetMenuItemText(curifont, fontn);
		::GetFNum(fontn, &fontid);
		
		gLogFont = fontid;
		switch(curisize)
		{
			default:
			case 1: gLogSize = 9; break;
			case 2: gLogSize = 10; break;
			case 3: gLogSize = 12; break;
			case 4: gLogSize = 14; break;
			case 5: gLogSize = 16; break;
			case 6: gLogSize = 18; break;
		}
		CMacCvsApp::gApp->RefreshLogFont();
	}
}

void CMacCvsApp::EventResume(const EventRecord &inMacEvent)
{
	m_IsFrontProcess = true;
//	SetSleepTime(this->IsCvsRunning() ? kSleepTimeWhenActiveAndFront : kSleepTimeWhenIdleAndFront);
	
	if(m_Notif)
	{
		::NMRemove(m_Notif);
		delete m_Notif;
		m_Notif = 0L;
		if(m_NotifIcon)
		{
			::DisposeIconSuite(m_NotifIcon, true);
			m_NotifIcon = 0L;
		}
	}
	
	LDocApplication::EventResume(inMacEvent);

	if(!CMacCvsApp::gApp->IsCvsRunning() && !TraversalReport::GetRefRunning())
		RefreshBrowsers(true);
}

void CMacCvsApp::EventSuspend(const EventRecord &inMacEvent)
{
	m_IsFrontProcess = false;
//	SetSleepTime(this->IsCvsRunning() ? kSleepTimeWhenActiveAndBackground : kSleepTimeWhenIdleAndBackground);

	LDocApplication::EventSuspend(inMacEvent);
}

void CMacCvsApp::SpendSomeTime(void)
{
	glue_eventdispatcher(0L);
}

bool CMacCvsApp::NotifyUser(const char *msg)
{
	if(m_IsFrontProcess)
		return false;
	
	if(m_Notif == 0L)
	{
		m_Notif =  new NMRec;
		ThrowIfNULL_(m_Notif);

		m_Notif->qType = nmType;
		m_Notif->nmMark = 1;

		OSErr	err = ::GetIconSuite(&m_NotifIcon, 128, svAllSmallData);
		m_Notif->nmIcon = err != noErr ? m_NotifIcon : 0L;
		m_Notif->nmSound = nil;
		m_Notif->nmStr = nil;
		LStr255 str;
		if(msg != 0L && msg[0] != '\0')
		{
			str = msg;
			m_Notif->nmStr = str;
		}
		m_Notif->nmResp = nil;
		::NMInstall(m_Notif);
	}
	
	return true;
}

bool CMacCvsApp::InteractWithUser(SInt32 inTimeoutInTicks)
{
  return UAppleEventsMgr::InteractWithUser(Throw_No, inTimeoutInTicks) == noErr;
}

void CMacCvsApp::SavePersistentSettings(bool quiet /*= false*/)
{
	LWindow *theWindow = UDesktop::FetchTopRegular();
	if (theWindow == 0L)
		return;
	
	CBrowserTable *brows = dynamic_cast<CBrowserTable*>
		(theWindow->FindPaneByID(item_BrowserTable));
	if(brows == 0L)
		return;

	::SavePersistentSettings(brows->GetPath(), quiet);
}

bool CMacCvsApp::CanUseKeychain()
{
	return gCvsPrefs.UseKeyChain() && (UEnvironment::GetOSVersion() < 0x1000 || UEnvironment::GetOSVersion() >= 0x1012) && KeychainManagerAvailable();
}

static std::vector<UStr> sAllPassKeys;
	// this are not the password, but instead a key like (alexp@cvs.strata.net:/cvsroot)

bool CMacCvsApp::CheckLoopingPassword(const char *key)
{
	std::vector<UStr>::iterator i = std::find(sAllPassKeys.begin(), sAllPassKeys.end(), UStr(key));
	return i == sAllPassKeys.end();
}

void CMacCvsApp::PushValidPassword(const char *key)
{
	sAllPassKeys.push_back(UStr(key));
}

void CMacCvsApp::FlushValidPasswords(void)
{
	sAllPassKeys.erase(sAllPassKeys.begin(), sAllPassKeys.end());
}

LListener *GetDlgHandler(void)
{
	return CMacCvsApp::gApp;
}

void CMacCvsApp::SetCvsRunning (bool state)
{
	SetUpdateCommandStatus(true);
	if ( state )
	{
	  if ( m_CvsRunning++ == 0 )
	    if ( m_toolbar ) m_toolbar->Start();
	}
	else
	{
	  if ( --m_CvsRunning == 0 )
	    if ( m_toolbar ) m_toolbar->Stop();
	}
//	SetSleepTime(m_CvsRunning ? (m_IsFrontProcess ? kSleepTimeWhenActiveAndFront : kSleepTimeWhenActiveAndBackground) : (m_IsFrontProcess ? kSleepTimeWhenIdleAndFront : kSleepTimeWhenIdleAndBackground));
//  SetSleepTime(m_CvsRunning ? 0 : 0xffffffff);
}

void CMacCvsApp::SetCvsStopping (bool state)
{
	SetUpdateCommandStatus(true);
	if (state ^ gCvsStopping)
	{
		gCvsStopping = state;
	}
}

//! launch a helper app or open documents with a helper app
// \param inArgs an argument list that contains the helper app/tool path in the first parameter and optional documents to open / parameters in the following parameters.
//               For a plain tool, the remaining arguments are just passed to the tool.
//               For an application, the remaining arguments are interpreted as paths to documents the app should open.
// \result Success or failure
//
bool CMacCvsApp::Execute(int argc, char* const* argv)
{
  // must at least have one argument - the application/tool path
  if ( argc == 0 ) return false;
  
  OSStatus  err(noErr);
	CStr      path(argv[0]);
	//
	// first do some variable replacements in the path
	//
	if ( path.length() > 8  &&  path.substr(0, 8) == "{maccvs}" )
	{
	  // replace "{maccvs}" with path to application package
	  CStr  _path(CMacCvsApp::gApp->GetAppPath());
	  if ( !_path.endsWith(kPathDelimiter) )
	    _path << kPathDelimiter;
	  _path << path.substr(8, path.length()-8);
	  path = _path;
	}
	else if ( path.length() > 12  &&  path.substr(0, 12) == "{maccvs_res}" )
	{
	  // replace "{maccvs_res}" with path to application package's resources folder
	  CStr  _path(CMacCvsApp::gApp->GetAppResourcesPath());
	  if ( !_path.endsWith(kPathDelimiter) )
	    _path << kPathDelimiter;
	  _path << path.substr(12, path.length()-12);
	  path = _path;
	}
	//
	// now check for existence and get application status
	//
	Boolean   isFolder;
	FSRef     toolFSRef;
	LSItemInfoRecord  toolInfo;
	if ( FSPathMakeRef((const UInt8*)static_cast<const char*>(path), &toolFSRef, &isFolder) == noErr  &&
	     LSCopyItemInfoForRef(&toolFSRef, kLSRequestBasicFlagsOnly, &toolInfo) == noErr )
	{
	  // check whether it's is a plain tool or an application
	  //
	  if ( toolInfo.flags & kLSItemInfoIsApplication )
	  {
	    FSRef   *docs = NULL;
	    if ( argc > 1 )
	    {
  	    docs = new FSRef[argc-1];
  	    for ( int i(1); i < argc; ++i )
  	    {
  	      if ( (err=FSPathMakeRef((const UInt8*)argv[i], &docs[i-1], NULL)) != noErr )
  	      {
  	        cvs_err("can't locate document '%s' while trying to launch with '%s'\n", argv[i], static_cast<const char*>(path));
  	        break;
  	      }
        }
  	    if ( err == noErr )
  	    {
    	    LSLaunchFSRefSpec   launchSpec = { &toolFSRef, argc-1, docs, NULL, kLSLaunchDefaults|kLSLaunchDontAddToRecents, NULL};
      	  err = LSOpenFromRefSpec(&launchSpec, NULL);
      	  if ( err != noErr ) cvs_err("launching application %s failed with error %d\n", static_cast<const char*>(path), err);
        }
        if ( docs ) delete[] docs;
      }
	  }
	  else if ( !isFolder )
  	{
	    CStr      command;
  	  command << "'" << path << "'";
  	  for ( int i(1); i < argc; ++i )
  	    command << " '" << argv[i] << "'";
    	err = system(command);
    	if ( err ) cvs_err("an error occured while executing %s: %s\n", static_cast<const char*>(path), strerror(errno));
    }
    else cvs_err("expected file for tool '%s', but this is a folder!\n", static_cast<const char*>(path));
  }
  else cvs_err("tool or application '%s' not found\n", static_cast<const char*>(path));

	return err == noErr;
}

short CMacCvsApp::GetToolbarHeight() const
{
  if ( m_toolbar == NULL ) return 0;
  
  Rect  r;
  m_toolbar->CalcPortFrameRect(r);
  return r.bottom - r.top;
}

Rect CMacCvsApp::GetToolbarGlobalBounds() const
{
  Rect    r;
  
  if ( m_toolbar == NULL ) 
    r.top = r.left = r.bottom = r.right = 0;
  else
    m_toolbar->GetGlobalBounds(r);
  
  return r;
}

#if PP_Debug
void CMacCvsApp::SetSleepTime( UInt32 inSleepTime )
{
  printf("MacCvs: settings sleep time to %lu ticks (%.3lf seconds)\n", inSleepTime, (inSleepTime*kEventDurationSecond)/60);
  LApplication::SetSleepTime(inSleepTime);
}
#endif

//
// Runtime intialization
//

// adjusting for the toolbar
short CvsGetMBarHeight(void)
{
	return ::GetMBarHeight() + 40; //(CMacCvsApp::gApp ? CMacCvsApp::gApp->GetToolbarHeight() : 0);
}

