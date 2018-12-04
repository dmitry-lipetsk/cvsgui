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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

// wincvs.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "wincvs.h"

#include "MainFrm.h"
#include "wincvsDoc.h"
#include "GraphDoc.h"
#include "LogFrm.h"
#include "GraphFrm.h"
#include "GraphView.h"
#include "WinCvsBrowser.h"
#include "BrowseFileView.h"
#include "AppGlue.h"
#include "CvsPrefs.h"
#include "AppConsole.h"
#include "Splash.h"
#include "MultiString.h"
#include "CvsCommands.h"
#include "Authen.h"
#include "TclGlue.h"
#include "PythonGlue.h"
#include "BrowserBar.h"
#include "FileTraversal.h"
#include "MakeSurePathExists.h"
#include "cvsgui_process.h"
#include "CvsAlert.h"
#include "HHelpSupport.h"
#include "dll_loader.h"
#include "HtmlStatic.h"
#include "AboutDlg.h"

#include <algorithm>
#include <sstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/// Array of authors names
const char* gAuthors[] = 
{
	"Alexandre Parenteau <aubonbeurre@hotmail.com>", 
	"Jens Miltner <jum@mac.com>",
	"Jerzy Kaczorowski <kaczoroj@hotmail.com>", 
	"Karl-Heinz Bruenen <gcvs@bruenen-net.de>",
	"Oliver Giesen <ogware@gmx.net>"
};

/// Class to handle auto-logout
class CLogoutThread : public CWinThread
{
	DECLARE_DYNAMIC(CLogoutThread)
public:
	// Construction
	CLogoutThread();
	virtual ~CLogoutThread();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogoutThread)
	//}}AFX_VIRTUAL

protected:


	// Generated message map functions
	//{{AFX_MSG(CLogoutThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	// Data members
	bool m_term;

	// Methods
	virtual BOOL InitInstance();
	virtual void Delete();

public:
	// Interface
	void KillThread(void);
};

CLogoutThread::CLogoutThread() 
	: m_term(false)
{
}

CLogoutThread::~CLogoutThread()
{
}

/// Delete virtual override, delete the thread
void CLogoutThread::Delete()
{
	CWinThread::Delete();
}

/// InitInstance virtual override, run the thread loop
BOOL CLogoutThread::InitInstance()
{
	for( ; ; )
	{
		Sleep(1000);

		if( m_term )
			return FALSE;

		CWincvsApp* app = (CWincvsApp*)AfxGetApp();

		if( !gCvsPrefs.HasLoogedIn() || gCvsPrefs.LogoutTimeOut() <= 0 || app->IsCvsRunning() )
		{
			continue;
		}

		const DWORD time = ::GetTickCount();
		if( app->GetIdleTime() != 0 &&
			(int)(time - app->GetIdleTime()) >= (gCvsPrefs.LogoutTimeOut() * 60 * 1000) )
		{
			app->ResetIdleTime();
			
			cvs_err("Making automatic logout after %d minute(s):\n", gCvsPrefs.LogoutTimeOut());
			CvsCmdLogout();
		}
	}

	return FALSE;
}

/*!
	Set the thread to self-terminate
*/
void CLogoutThread::KillThread(void)
{
	m_term = true;
}


IMPLEMENT_DYNAMIC(CLogoutThread, CWinThread);

BEGIN_MESSAGE_MAP(CLogoutThread, CWinThread)
	//{{AFX_MSG_MAP(CLogoutThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/// Logout thread pointer
static CLogoutThread* gLogoutThread = NULL;

//////////////////////////////////////////////////////////////////////////
// CModalGuard

CWincvsApp::CModalGuard::CModalGuard()
{
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		app->IncrementModalCount();
	}
}

CWincvsApp::CModalGuard::~CModalGuard()
{
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		app->DecrementModalCount();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWincvsApp

BEGIN_MESSAGE_MAP(CWincvsApp, CWinApp)
	ON_COMMAND(CG_IDS_TIPOFTHEDAY, ShowTipOfTheDay)
	//{{AFX_MSG_MAP(CWincvsApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_COPYRIGHTS, OnAppCopyrights)
	ON_COMMAND(ID_APP_CREDITS, OnAppCredits)
	ON_UPDATE_COMMAND_UI(ID_APP_COPYRIGHTS, OnLogWindowIsPresent)
	ON_COMMAND(ID_STOPCVS, OnStopcvs)
	ON_UPDATE_COMMAND_UI(ID_STOPCVS, OnUpdateStopcvs)
	ON_UPDATE_COMMAND_UI(ID_APP_CREDITS, OnLogWindowIsPresent)
	ON_COMMAND(ID_HELP_CVS, OnHelpCvs)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWincvsApp construction

CWincvsApp::CWincvsApp() 
	: m_isBackgroundProcess(false),
	m_modalCount(0)
{
	m_CvsRunning = false;
	m_CvsStopping = false;
	m_idletime = 0;
	m_CvsProcess = NULL;

	m_hasTCL = false;
	m_hasPY = false;
}

CWincvsApp::~CWincvsApp()
{
	ResetFailedLibrary();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWincvsApp object

CWincvsApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {D2D77DC2-8299-11D1-8949-444553540000}
static const CLSID clsid =
{ 0xd2d77dc2, 0x8299, 0x11d1, { 0x89, 0x49, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };

/////////////////////////////////////////////////////////////////////////////
// CWincvsApp initialization

/*
 * The handle is of an unknown type.  Test the validity of this OS
 * handle by duplicating it, then closing the dupe.  The Win32 API
 * doesn't provide an IsValidHandle() function, so we have to emulate
 * it here.  This test will not work on a console handle reliably,
 * which is why we can't test every handle that comes into this
 * function in this way.
 */
bool IsValidHandle(HANDLE handle) 
{
    HANDLE dupedHandle = NULL;

    /*
     * GetFileType() returns FILE_TYPE_UNKNOWN for invalid handles.
     */

    if( GetFileType(handle) == FILE_TYPE_UNKNOWN )
	{
		if( !DuplicateHandle(GetCurrentProcess(), 
			handle,
			GetCurrentProcess(), 
			&dupedHandle, 
			0, 
			FALSE,
			DUPLICATE_SAME_ACCESS) ) 
		{
			/* 
			* Unable to make a duplicate. It's definately invalid at this point
			*/
			
			return false;
		}
		
		/*
		* Use structured exception handling (Win32 SEH) to protect the close
		* of this duped handle which might throw EXCEPTION_INVALID_HANDLE.
		*/
		__try 
		{
			CloseHandle(dupedHandle);
		}
		__except(EXCEPTION_EXECUTE_HANDLER) 
		{
			/*
			* Definately an invalid handle.  So, therefore, the original
			* is invalid also.
			*/
			
			return false;
		}
	}

	return true;
}

BOOL CWincvsApp::InitInstance()
{
#if qCvsDebug
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
#endif

	// David Gravereaux <davygrvy@interwoven.com> suggests this might
	// be a work around to the bug #42 (I love the bug number,
	// really fits the bug description ;-))
#if 0
	if( !IsValidHandle(GetStdHandle(STD_OUTPUT_HANDLE)) )
	{
		SetStdHandle(STD_OUTPUT_HANDLE, NULL);
	}
#endif

	// CG: The following block was added by the Splash Screen component.
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);

		CSplashWnd::EnableSplashScreen(
			(!(m_nCmdShow & SW_SHOWMINIMIZED)) && cmdInfo.m_bShowSplash);
	}

	if( !AfxSocketInit() )
	{
		CString errorMsg;
		errorMsg.LoadString(IDP_SOCKETS_INIT_FAILED);

		CvsAlert(kCvsAlertStopIcon, 
			errorMsg, NULL, 
			BUTTONTITLE_OK, NULL).ShowAlert();

		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// Initialize HTML Help support
	CHHelpSupport::InitHTMLHelp();

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("WinCvs"));

	LoadStdProfileSettings(5);  // Load standard INI file options (including MRU)
	gCvsPrefs.load();
	
	CSplashWnd::EnableSplashScreen(!gCvsPrefs.DisableSplashScreen());

    string strVersion;
    GetAppVersion(strVersion);
    WriteProfileString(_T(""), _T("Current Version"), strVersion.c_str());

	// We need to make a temp local copy of LastWorkingDir property here, 
	// because CWinCvsBrowser::OnSelchanging event would override 
	// LastWorkingDir preference when browser window is created.
	string lastWorkingDir(gCvsPrefs.LastWorkingDir());

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_logTempl = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWincvsDoc),
		RUNTIME_CLASS(CLogFrame),
		RUNTIME_CLASS(CBrowseFileView));
	AddDocTemplate(m_logTempl);

	m_logGraph = new CMultiDocTemplate(
		IDR_GRAPH,
		RUNTIME_CLASS(CGraphDoc),
		RUNTIME_CLASS(CGraphFrame),
		RUNTIME_CLASS(CGraphView));
	AddDocTemplate(m_logGraph);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	m_pMainWnd = pMainFrame;
	if( !pMainFrame->LoadFrame(IDR_CNTR_INPLACE) )
		return FALSE;

	// Initialize the cool menus...
#if 0
	UINT barIds[] = {IDR_BROWSER, IDR_MAINFRAME, IDR_MULTIUSER, IDR_TAGS, IDR_FILELIST, IDR_FILTER};
	m_mainFrame->InitializeMenu(m_logTempl,
		IDR_MAINFRAME, barIds, _countof(barIds), IDR_MAINFRAME);
	m_mainFrame->InitializeMenu(m_logGraph,
		IDR_GRAPH, barIds, _countof(barIds), IDR_GRAPH);
#endif

	// Print CVS binary location
	{
		std::string cvsPath;
		cvs_out("CVS executable: %s\n", gCvsPrefs.WhichCvs(cvsPath));
	}

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Try to resolve the long filename if the argument passed is the short filename (like when it's a call thru SendTo shell menu)
	if( -1 != cmdInfo.m_strFileName.Find('~') )
	{
		CString strLongFileName;
		CString strTempTileName = cmdInfo.m_strFileName;

		SHFILEINFO sFileInfo;
		int nSlashPos = -1;

		BOOL bError = FALSE;

		while( -1 != (nSlashPos = strTempTileName.ReverseFind(kPathDelimiter)) )
		{
			if( SHGetFileInfo((LPCTSTR)strTempTileName, 0, &sFileInfo, sizeof(sFileInfo), SHGFI_DISPLAYNAME) )
			{
				CString strFileNameBit = strTempTileName.Mid(nSlashPos + 1);

				strLongFileName = sFileInfo.szDisplayName + strLongFileName;
				strLongFileName = kPathDelimiter + strLongFileName;
				strTempTileName = strTempTileName.Left(nSlashPos);
			}
			else
			{
				bError = TRUE;
				break;	//we need it or we get the dead loop
			}
		}
		
		if( !bError )
		{
			cmdInfo.m_strFileName = strTempTileName + strLongFileName;
		}
	}
				
	// Open in the directory, so remove the file
	CString strOpenDirectory;
	CString strSelectFile;
	if( !cmdInfo.m_strFileName.IsEmpty() )
	{
		TCHAR fullpath[_MAX_PATH];
		_tfullpath(fullpath, cmdInfo.m_strFileName, _MAX_PATH);
		if( GetFileAttributes(cmdInfo.m_strFileName) != 0xFFFFFFFF )
		{
			cmdInfo.m_strFileName.Replace('/', kPathDelimiter);	//normalize path
			
			TCHAR drive[_MAX_DRIVE];
			TCHAR dir[_MAX_DIR];
			
			_tsplitpath(fullpath, drive, dir, NULL, NULL);
			strSelectFile = fullpath;
			_tmakepath(fullpath, drive, dir, NULL, NULL);
			strOpenDirectory = fullpath;
		}
	}

	// Dispatch commands specified on the command line
	// We need to trick the ProcessShellCommand a bit
	// because we might get a directory as a parameter we don't want it to check after the file...
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
	if( !ProcessShellCommand(cmdInfo) )
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		pMainFrame->InitialShowWindow(m_nCmdShow);
		pMainFrame->UpdateWindow();
		
		pMainFrame->DragAcceptFiles();
	}

	//CWincvsView *cons = GetConsoleView();
	//if(cons != NULL)
	//	cons->DragAcceptFiles();

	CWinCvsBrowser* browser = GetBrowserView();
	CWincvsDoc* doc = (CWincvsDoc*)GetDocument();
	if( doc != NULL && browser != NULL )
		doc->SetTitle(browser->GetRoot().c_str());

	if( CBrowseFileView* fileView = GetFileView() )
	{
		if( strOpenDirectory.IsEmpty() )
		{
			browser->SetInitialRoot();

			// Goto the last working directory, which was selected when WinCVS was closed
			if( !lastWorkingDir.empty() && browser->StepToLocation(lastWorkingDir.c_str()) )
			{
				CheckLocationDriveType(lastWorkingDir.c_str());
				fileView->ResetView(lastWorkingDir.c_str(), false, (LPCTSTR)NULL);
				gCvsPrefs.SetLastWorkingDir(lastWorkingDir.c_str());
			}
			else
			{
				CheckLocationDriveType(browser->GetRoot().c_str());
 				fileView->ResetView(browser->GetRoot().c_str());
			}
		}
		else
		{
			CColorConsole out;
			out << kBold << "Locating: " << kNormal << kUnderline << cmdInfo.m_strFileName << kNormal <<  kNormal << kNL;

			//search the browse location (will create a new location if not found)
			extern CMString gHistoryLocs;
			CString bestRoot = CHistComboBoxEx::FindBestRoot(gHistoryLocs, strOpenDirectory);
			if( !bestRoot.IsEmpty() )
			{
				browser->ResetBrowser(bestRoot, true);
			}
			else
			{
				browser->SetInitialRoot();
			}

			if( browser->StepToLocation(strOpenDirectory) )
			{
				CheckLocationDriveType(strOpenDirectory);
				fileView->ResetView(strOpenDirectory, false, strSelectFile);
			}
			else
			{
				CheckLocationDriveType(browser->GetRoot().c_str());
				fileView->ResetView(browser->GetRoot().c_str());
			}
		}
	}

	// If you launch WinCvs with the file or directory as a parameter you obviously don't need a "tip of the day"
	if( strOpenDirectory.IsEmpty() )
	{
		// CG: This line inserted by 'Tip of the Day' component.
		ShowTipAtStartup();
	}

	// Auto-logout thread
	gLogoutThread = new CLogoutThread();
	if( gLogoutThread == NULL )
	{
		cvs_err("Impossible to create the auto-logout thread !\n");
	}
	else
	{
		ASSERT_VALID(gLogoutThread);

		// Create Thread in a suspended state so we can set the Priority 
		// before it starts getting away from us
		if( !gLogoutThread->CreateThread(CREATE_SUSPENDED) )
		{
			cvs_err("Impossible to start the auto-logout thread (error %d)!\n", GetLastError());
			
			delete gLogoutThread;
			gLogoutThread = NULL;
		}

		if( gLogoutThread != NULL )
		{
			// If you want to make the sample more sprightly, set the thread priority here 
			// a little higher. It has been set at idle priority to keep from bogging down 
			// other apps that may also be running.
			VERIFY(gLogoutThread->SetThreadPriority(THREAD_PRIORITY_NORMAL));
			// Now the thread can run wild
			gLogoutThread->ResumeThread();
		}
	}

	// Start macros
	StartScriptEngine();

	// Make sure the current directory is properly setup
	if( CBrowseFileView* fileView = GetFileView() )
	{
		chdir(fileView->GetPath().c_str());
	}

	return TRUE;
}

/// App command to run the dialog
void CWincvsApp::OnAppAbout()
{
	CompatAboutDialog();
}

/*!
	Get the top console
	\return The top console, may be NULL
*/
CWincvsView* CWincvsApp::GetConsoleView(void) const
{
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		return pMainFrame->GetConsoleView();
	}
	
	return NULL;
}

/*!
	Get the notification manager
	\return The notification manager reference
*/
NotificationManager* CWincvsApp::GetNotificationManager()
{
	return &m_notificationManager;
}

/*!
	Get the mainframe status bar
	\return The mainframe status bar, may be NULL
*/
CStatusBar* CWincvsApp::GetStatusBar() const
{
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		return pMainFrame->GetStatusBar();
	}

	return NULL;
}

/*!
	Get the directory browser
	\return The directory browser, may be NULL
*/
CWinCvsBrowser* CWincvsApp::GetBrowserView(void) const
{
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		return pMainFrame->GetBrowserView();
	}

	return NULL;
}

/*!
	Get the current root
	\return Current root
*/
CString CWincvsApp::GetRoot() const
{
	CString root;

	if( CWincvsApp* app = (CWincvsApp *)AfxGetApp() )
	{
		if( CWinCvsBrowser* pBrowser = app->GetBrowserView() )
		{
			root = pBrowser->GetRoot().c_str();
		}
	}

    return root;
}

/*!
	Activate browser view
*/
void CWincvsApp::ActivateBrowserView(void)
{
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		pMainFrame->GetWorkspace()->SetActiveView(RUNTIME_CLASS(CBrowserBar));
	}
}

/*!
	Get the directory explorer
	\return The directory explorer, may be NULL
*/
CCJShellTree* CWincvsApp::GetExplorerView(void) const
{
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		return pMainFrame->GetExplorerView();
	}

	return NULL;
}

/*!
	Get the files browser
	\return The files browser
*/
CBrowseFileView* CWincvsApp::GetFileView(void) const
{
	
	POSITION post = GetFirstDocTemplatePosition();
	while( post != NULL )
	{
		CDocTemplate* tmpl = GetNextDocTemplate(post);
		if( !tmpl->IsKindOf(RUNTIME_CLASS(CMultiDocTemplate)) )
			continue;
		
		POSITION posd = tmpl->GetFirstDocPosition();
		while( posd != NULL )
		{
			CDocument* doc = tmpl->GetNextDoc(posd);
			if( !doc->IsKindOf(RUNTIME_CLASS(CWincvsDoc)) )
				continue;

			POSITION posv = doc->GetFirstViewPosition();
			while( posv != NULL )
			{
				CView* view = doc->GetNextView(posv);
				if( view->IsKindOf(RUNTIME_CLASS(CBrowseFileView)) )
					return (CBrowseFileView*)view;
			}
		}
	}

	return NULL;
}

/*!
	Get the document
	\return The document
*/
CDocument* CWincvsApp::GetDocument(void) const
{
	CWinApp* app = AfxGetApp();
	
	POSITION post = app->GetFirstDocTemplatePosition();
	while( post != NULL )
	{
		CDocTemplate* tmpl = app->GetNextDocTemplate(post);

		POSITION posd = tmpl->GetFirstDocPosition();
		while( posd != NULL )
		{
			CDocument* doc = tmpl->GetNextDoc(posd);
			if( !doc->IsKindOf(RUNTIME_CLASS(CWincvsDoc)) )
				continue;

			return doc;
		}
	}

	ASSERT(0);
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CWincvsApp commands

/// Check the path is valid and prompt to create if not (depending on the promptForCreation parameter)
void AFXAPI DDV_CheckPathExists(CDataExchange* pDX, int nIDC, CString& value, bool promptForCreation /*= true*/)
{
	if( pDX->m_bSaveAndValidate )
	{
		if( !CompatMakeSurePathExists(value, false) )
		{
			value = NormalizeFolderPath(string(value), false).c_str();

			if( !CompatMakeSurePathExists(value, promptForCreation) )
			{
				CvsAlert(kCvsAlertWarningIcon, 
					"Cannot find folder: " + value, NULL, 
					BUTTONTITLE_OK, NULL).ShowAlert();

				pDX->PrepareEditCtrl(nIDC);
				pDX->Fail();
			}
		}
	}
}

/// Check whether the CVSROOT entered is valid, correct if not...
void AFXAPI DDV_CheckCVSROOT(CDataExchange* pDX, CString& value)
{
	// Check after the trailing white spaces - often an unwanted result of copy-paste
	if( pDX->m_bSaveAndValidate )
	{
		CString trimmedValue(value);
		trimmedValue.TrimRight();

		if( trimmedValue.GetLength() != value.GetLength() )
		{
			CvsAlert cvsAlert(kCvsAlertQuestionIcon, 
				"Do you want to trim trailing spaces?", "The CVSROOT value contains the trailing spaces.", 
				BUTTONTITLE_YES, BUTTONTITLE_NO);

			if( cvsAlert.ShowAlert() == kCvsAlertOKButton )
			{
				value = trimmedValue;
			}
		}
	}
}

/*!
	Smart combo box DDX routine
	\param pDX Date exchange
	\param nIDC Control ID
	\param combo Smart combo box
	\return true on success, false otherwise
	\note If pDX passed as NULL then the combo is populated
*/
void AFXAPI DDX_ComboMString(CDataExchange* pDX, int nIDC, CSmartComboBox& combo)
{
	CMString* mstr = combo.GetItems();
	ASSERT(mstr != NULL);

	if( mstr )
	{
		if( !pDX || !pDX->m_bSaveAndValidate )
		{
			CString strOldText;
			combo.GetWindowText(strOldText);
			
			combo.ResetContent();

			const CMString::list_t& list = mstr->GetList();
			for(CMString::list_t::const_iterator i = list.begin(); i != list.end(); ++i)
			{
				if( combo.HasFeature(CSmartComboBox::Keyword) )
				{
					if( CMKeyString* pMKeyString = dynamic_cast<CMKeyString*>(combo.GetItems()) )
					{
						string key;
						string value;

						pMKeyString->Split(i->c_str(), key, value);
						if( key.compare(combo.GetKeyword()) == 0 )
						{
							combo.AddString(value.c_str());
						}
					}
					else if( CKeyString* pKeyString = dynamic_cast<CKeyString*>(combo.GetItems()) )
					{
						string key;
						string value;
						
						pKeyString->Split(i->c_str(), key, value);
						if( !value.empty() )
						{
							if( combo.FindStringExact(-1, value.c_str()) < 0 )
							{
								combo.AddString(value.c_str());
							}
						}
					}
				}
				else
				{
					combo.AddString(i->c_str());
				}
			}
			
			combo.SetWindowText(strOldText);
		}
		else
		{
			CString editValue;
			CString value;
			
			DDX_CBString(pDX, nIDC, editValue);

			if( !editValue.IsEmpty() )
			{
				if( combo.HasFeature(CSmartComboBox::Keyword) )
				{
					if( CMKeyString* pMKeyString = dynamic_cast<CMKeyString*>(combo.GetItems()) )
					{
						string mapValue;
						pMKeyString->Concatenate(mapValue, combo.GetKeyword().c_str(), editValue);
						
						value = mapValue.c_str();
					}
					else if( CKeyString* pKeyString = dynamic_cast<CKeyString*>(combo.GetItems()) )
					{
						string mapValue;
						pKeyString->Concatenate(mapValue, combo.GetKeyword().c_str(), editValue);
						
						value = mapValue.c_str();
					}
				}
				else
				{
					value = editValue;
				}
				
				const CMString::list_t& list = mstr->GetList();
				if( !(std::find(list.begin(), list.end(), string(value)) != list.end()) )
				{
					mstr->Insert(value);
					combo.AddString(editValue);
				}
				else
				{
					mstr->Remove(value);
					mstr->Insert(value);
				}
			}
		}
	}
}
	
/// Verify the entry has at least nChars characters
void AFXAPI DDV_MinChars(CDataExchange* pDX, CString const& value, int nChars)
{
	ASSERT(nChars > 0); // allow them something
	
	if( pDX->m_bSaveAndValidate && value.GetLength() < nChars )
	{
		CString errorMsg;
		errorMsg.Format("Please enter more than %d character%s.", nChars, nChars > 1 ? "s" : "");

		CvsAlert(kCvsAlertStopIcon, 
			errorMsg, NULL, 
			BUTTONTITLE_OK, NULL).ShowAlert();

		pDX->Fail();
	}
}

int CWincvsApp::ExitInstance() 
{
	return CWinApp::ExitInstance();
}

void CWincvsApp::OnLogWindowIsPresent(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetConsoleView() != NULL);
}

void CWincvsApp::OnFileNew()
{
	m_logTempl->OpenDocumentFile(NULL);
}

void CWincvsApp::OnAppCopyrights() 
{
	cvs_out("\nCvsGui, CVSNT and CVS are distributed under the terms of\n");
	cvs_out("the GNU General Public License (GPL).\n\n");
	cvs_out("* WinCvs: maintained by Jerzy Kaczorowski <kaczoroj@hotmail.com>\n");
	cvs_out("* MacCVS: maintained by Alexandre Parenteau <aubonbeurre@hotmail.com>\n");
	cvs_out("* gCvs: maintained by Karl-Heinz Bruenen <gcvs@bruenen-net.de>\n");
	cvs_out("* WinCvs documentation : Copyright © 1999 Don Harper <don@lamrc.com>.\n");
	cvs_out("* CVSNT : Copyright (c) 1999-2003 Tony Hoyle and others\n");
	cvs_out("* CVS : Copyright © 1989-1998 Brian Berliner, david d `zoo' zuhn,\n");
	cvs_out("        Jeff Polk, and other authors\n");
	cvs_out("* CJLIB : Copyright © 1998-99 Kirk Stowell <kstowell@codejockeys.com>\n");
	cvs_out("* Kerberos : Copyright © 1997 the Massachusetts Institute of Technology\n");
	cvs_out("* TCL : Copyright © Sun Microsystems Inc.\n");
	cvs_out("* Python : Copyright © Guido van Rossum\n");
	cvs_out("* MFC : Copyright © 1992-1997 Microsoft Corporation\n");
	cvs_out("\nSome other copyrights may apply, please check the source code for details.\n");
}

void CWincvsApp::OnAppCredits() 
{
	// WinCvs version
	string vers;

	if( GetAppVersion(vers) )
		cvs_out("\n%s\n", vers.c_str());
	else
		cvs_out("\nWinCvs\n");

	// Authors in random order
	cvs_out("- WinCvs is developed by:\n");
	
	const int authorsSize = sizeof(gAuthors) / sizeof(gAuthors[0]);
	vector<const char*> authors(authorsSize);

	for(int index = 0; index < authorsSize; index++)
	{
		authors[index] = gAuthors[index];
	}

	srand((unsigned)time(NULL));
	random_shuffle(authors.begin(), authors.end());

	for(vector<const char*>::const_iterator it = authors.begin(); it != authors.end(); it++)
	{
		std::stringstream strOut;
		strOut << "\t* "<< (*it) << "\n", 

		cvs_out(strOut.str().c_str());
	}

	cvs_out("\t* Other contributors (see the ChangeLog)\n");
	
	// More info
	cvs_out("- WinCvs page : http://cvsgui.sourceforge.net/ or http://www.wincvs.org\n");
	cvs_out("- CVSNT page : http://www.cvsnt.org\n");
	cvs_out("- CVS page : http://www.cvshome.org\n");
	cvs_out("- WinCvs documentation : Don Harper <don@lamrc.com>.\n");
	cvs_out("- WinHelp cvs documentation : Norbert Klamann.\n");
	
	cvs_out("- Special thanks to :\n");
	cvs_out("\t* The Strata Inc. developpers who are so patients with WinCvs\n");
	cvs_out("\t  and help to identify a lot of problems.\n");
	cvs_out("\t* All the people who are submitting bugs, patches and\n");
	cvs_out("\t  answer the cvsgui mailing list.\n");
	
	cvs_out("- Bug reports, suggestions :\n");
	cvs_out("\t* Richard Wesley, Alain Aslag Roy, Eric Aubourg and many others\n");
	cvs_out("- CVS contributors :\n");
	cvs_out("\t* Too many to be listed here, see the 'ChangeLog' instead !\n");
}

void CWincvsApp::ShowTipAtStartup(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	if( cmdInfo.m_bShowSplash )
	{
		CTipDlg dlg(IsWindow(m_pMainWnd->GetSafeHwnd()) ? m_pMainWnd : NULL);
		
		if( dlg.m_bStartup )
			dlg.DoModal();
	}
}

void CWincvsApp::ShowTipOfTheDay(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CTipDlg dlg;
	dlg.DoModal();
}

BOOL CWincvsApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if( CSplashWnd::PreTranslateAppMessage(pMsg) )
		return TRUE;

	DWORD time = ::GetTickCount();

	if( m_idletime == 0 ||
		pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN )
	{
		m_idletime = time;
		PyInvalUICache();
	}

	IdleCvsProgress();

	return CWinApp::PreTranslateMessage(pMsg);
}

BOOL CWincvsApp::SaveAllModified() 
{
	// Prevent from exiting when cvs is running
	if( m_CvsRunning )
	{
		m_CvsStopping = true;
		return TRUE;
	}

	if( gLogoutThread != NULL )
	{
		HANDLE hdl = gLogoutThread->m_hThread;
		gLogoutThread->KillThread();
		WaitForSingleObject(hdl, INFINITE);
		
		//delete gLogoutThread;
		gLogoutThread = NULL;
	}

	// Auto-logout
	if( gCvsPrefs.HasLoogedIn() && gCvsPrefs.LogoutTimeOut() > 0 )
	{
		cvs_err("Making automatic logout before quitting:\n");
		CvsCmdLogout();
		WaitForCvs();
	}

	return CWinApp::SaveAllModified();
}

/*!
	Ask for an environmental variable value from the main thread for those which require interaction (ex: CVS_GETPASS)
	\param name Environmental variable name
	\return The environmental variable value
*/
const char* CWincvsApp::GetEnvMainThread(const char* name)
{
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		return pMainFrame->GetEnvMainThread(name);
	}

	return "";
}

/*!
	Show the progress status during the idle time
*/
void CWincvsApp::IdleCvsProgress(void)
{
	static bool bWasCvsRunning = false;	// To restore the idle message once CVS has stoped

	if( !m_CvsRunning )
	{
		if( bWasCvsRunning )
		{
			bWasCvsRunning = false;

			CString strIdleString;
			strIdleString.LoadString(AFX_IDS_IDLEMESSAGE);

			if( CStatusBar* bar = GetStatusBar() )
			{
				bar->SetWindowText(strIdleString);
			}
		}

		return;
	}

	if( m_CvsProcess != NULL )
	{
		if( !cvs_process_is_active(m_CvsProcess) )
		{
			m_CvsRunning = false;
			m_CvsStopping = false;
			m_CvsProcess = NULL;

			//GetFileView()->CheckChanges();
		}
	}

	bWasCvsRunning = true;

	DWORD whatTimeIsIt = ::GetTickCount();
	static DWORD lastTime = 0;
	
	// Let's make some stuff every second while cvs is idling
	if( (whatTimeIsIt - lastTime) < 1000 )
		return;

	lastTime = whatTimeIsIt;
	
	char path[_MAX_PATH];
	getcwd(path, _MAX_PATH);
	
	CStatusBar* bar = GetStatusBar();

	if( bar != NULL )
		bar->SetWindowText(path);
}

/*!
	Get the filtering model
	\return The filtering model, may be NULL
*/
KiFilterModel* CWincvsApp::GetFilterModel() const
{
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		return pMainFrame->GetFilterModel();
	}

	return NULL;
}

/*!
	Get the recursion model
	\return The recursion model, may be NULL
*/
KiRecursionModel* CWincvsApp::GetRecursionModel() const
{
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		return pMainFrame->GetRecursionModel();
	}

	return NULL;
}

/*!
	Get the ignore model
	\return The ignore model, may be NULL
*/
KiIgnoreModel* CWincvsApp::GetIgnoreModel() const
{
	if( CMainFrame* pMainFrame = GetMainFrame() )
	{
		return pMainFrame->GetIgnoreModel();
	}

	return NULL;
}

/*!
	Update all documents views for all documents
	\param pSender Sender view
	\param hint Update hint
	\param pHint Update hint data
*/
void CWincvsApp::UpdateAllDocsView(CView* pSender /*= NULL*/, tUpdateHint hint /*= kNullHint*/, CObject* pHint /*= NULL*/)
{
	POSITION pos = GetFirstDocTemplatePosition();
	while( pos )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos); 
		
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while( pos2 )
		{
			CDocument* pDocument;
			if( pDocument = pTemplate->GetNextDoc(pos2) )
			{
				pDocument->UpdateAllViews(pSender, (LPARAM)hint, pHint);
			}
		}
	}
}

/*!
	Peek and pump application messages and perform the idle processing
	\param doIdle true to an idle processing, false otherwise
*/
void CWincvsApp::PeekPumpAndIdle(bool doIdle)
{
	MSG Msg;
	while( ::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE) )
	{
		PumpMessage();
	}
	
	if( doIdle )
	{
		LONG lIdle = 0;
		while( OnIdle(lIdle++) )
			;
	}
}

/*!
	Restart the script engine
*/
void CWincvsApp::RestartScriptEngine()
{
	StopScriptEngine();
	StartScriptEngine(true);
}

/*!
	Start the script engine
*/
void CWincvsApp::StartScriptEngine(const bool forceReload /*= false*/)
{
	CPython_Interp::InitPythonSupport(forceReload);
	
	PyMacrosReloadAll();

	m_hasTCL = CTcl_Interp::IsAvail(forceReload);
	m_hasPY = CPython_Interp::IsAvail();
	
	if( gCvsPrefs.GetUseShell() == SHELL_PYTHON && !m_hasPY && m_hasTCL )
	{
		cvs_out("Python is not available, switching the shell to TCL\n");
		gCvsPrefs.SetUseShell(SHELL_TCL);
	}
	else if( gCvsPrefs.GetUseShell() == SHELL_TCL && m_hasPY && !m_hasTCL )
	{
		cvs_out("TCL is not available, switching the shell to Python\n");
		gCvsPrefs.SetUseShell(SHELL_PYTHON);
	}
	
	if( gCvsPrefs.GetUseShell() == SHELL_TCL && m_hasTCL )
	{
		cvs_out("TCL is available, shell is enabled : help (select and press enter)\n");
	}
	
	if( !m_hasTCL && !m_hasPY )
	{
		cvs_err("TCL or Python are not available, shell is disabled\n");
	}
}

/*!
	Stop the script engine
*/
void CWincvsApp::StopScriptEngine()
{
	PyInvalUICache();
	CPython_Interp::ClosePythonSupport();
}

/// WM_COMMAND message handler, stop running CVS command
void CWincvsApp::OnStopcvs() 
{
	stopCVS();
}

/// UPDATE_COMMAND_UI handler
void CWincvsApp::OnUpdateStopcvs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsCvsRunning() && !IsCvsStopping() ? TRUE : FALSE);
}

/// Display CVS Documentation file
void CWincvsApp::OnHelpCvs() 
{
	string cvscmd;
	gCvsPrefs.WhichCvs(cvscmd, gCvsPrefs.UseAltCvs());

	string uppath, folder;
	if( SplitPath(cvscmd.c_str(), uppath, folder) )
	{
		string cvsHelpPath = uppath;
		cvsHelpPath += "cvs.chm";

		if( ::HtmlHelp(GetDesktopWindow(), cvsHelpPath.c_str(), HH_DISPLAY_TOPIC, 0) == NULL )
		{
			CvsAlert cvsAlert(kCvsAlertStopIcon, 
				_i18n("Unable to open CVS Documentation file."), 
				"Make sure CVS Documentation is installed in the same folder as your CVS executable.",
				BUTTONTITLE_OK, NULL);
			
			cvsAlert.ShowAlert();
		}
	}
	else
	{
		CvsAlert cvsAlert(kCvsAlertStopIcon, 
			_i18n("Unable to locate CVS Documentation file."), 
			"Make sure CVS Documentation is installed in the same folder as your CVS executable.",
			BUTTONTITLE_OK, NULL);
		
		cvsAlert.ShowAlert();
	}
}
