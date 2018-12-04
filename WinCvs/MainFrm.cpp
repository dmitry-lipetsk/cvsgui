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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "wincvs.h"

#include "MainFrm.h"
#include "WinCvsBrowser.h"
#include "BrowseFileView.h"
#include "WincvsView.h"
#include "Persistent.h"
#include "AppConsole.h"
#include "Splash.h"
#include "BrowserBar.h"
#include "ExploreBar.h"
#include "CvsPrefs.h"
#include "AppGlue.h"
#include "CvsAlert.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Define to disable the "Explore" view
#define NOEXPLOREVIEW

CMString gHistoryLocs(100, "P_BrowserLocs");
static CWinPlacement gMainPlacement("P_MainPlacement");

static bool sAskingMainThread;
static string sGetEnvResult;

static CSemaphore sGetEnv(0);

/*!
	Check whether to enable commands
	\return true if commands can be enabled, false otherwise
*/
static bool IsAppEnabled()
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	return !app->IsCvsRunning();
}

#if _MSC_VER <= 1200 && defined(WINXP_MANIFEST_FIX)
//////////////////////////////////////////////////////
// CWinToolBar

BEGIN_MESSAGE_MAP(CWinToolBar, CToolBar)
//{{AFX_MSG_MAP(CWinToolBar)
ON_WM_NCPAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWinToolBar::OnNcPaint() 
{
	EraseNonClient();
}

void CWinToolBar::EraseNonClient()
{
	// Get window DC that is clipped to the non-client area.
	CWindowDC dc(this);
	CRect rectClient;
	GetClientRect(rectClient);
	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect(rectClient);
	
	// Draw the borders in the non-client area.
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	DrawBorders(&dc, rectWindow);
	
	// Erase the parts that are not drawn.
	dc.IntersectClipRect(rectWindow);
	SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);
	
	// Draw the gripper in the non-client area.
	DrawGripper(&dc, rectWindow);
}

void CWinToolBar::DoPaint(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	
	// Paint inside the client area.
	CRect rect;
	GetClientRect(rect);
	DrawBorders(pDC, rect);
	DrawGripper(pDC, rect);
}

void CWinToolBar::DrawGripper(CDC* pDC, const CRect& rect)
{
	pDC->FillSolidRect( &rect, ::GetSysColor(COLOR_BTNFACE)); // Fill in the background.
	CToolBar::DrawGripper(pDC,rect);
}
#endif

//////////////////////////////////////////////////////////////////////////
// CWinPlacement

CWinPlacement::CWinPlacement(const char* uniqueName) 
	: CPersistent(uniqueName, kNoClass)
{
	m_hasOne = false;
}

CWinPlacement::~CWinPlacement()
{
}

unsigned int CWinPlacement::SizeOf(void) const
{
	return m_hasOne ? sizeof(WINDOWPLACEMENT) : 0;
}

const void* CWinPlacement::GetData(void) const
{
	return m_hasOne ? &m_placement : 0L;
}

void CWinPlacement::SetData(const void* ptr, unsigned int size)
{
	if( size == sizeof(WINDOWPLACEMENT) )
	{
		memcpy(&m_placement, ptr, sizeof(WINDOWPLACEMENT));
		m_hasOne = true;
	}
}

CWinPlacement& CWinPlacement::operator=(const WINDOWPLACEMENT& newplace)
{
	TouchTimeStamp();
	
	memcpy(&m_placement, &newplace, sizeof(WINDOWPLACEMENT));
	m_hasOne = true;

	return *this;
}

/// Get the window placement
WINDOWPLACEMENT CWinPlacement::GetPlacement() const
{
	return m_placement;
}

/// Get the allocated memory flag
bool CWinPlacement::IsHasOne() const
{
	return m_hasOne;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CCJMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CCJMDIFrameWnd)
	ON_WM_INITMENU()
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_CBN_SELCHANGE(ID_HISTORYBROWSE, OnHistorySelect)
	ON_WM_ACTIVATE()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_VIEW_RECURSIVE, OnViewRecursive)
	ON_COMMAND(ID_VIEW_MODIFIED, OnViewModified)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MODIFIED, OnUpdateViewModified)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RECURSIVE, OnUpdateViewRecursive)
	ON_COMMAND(ID_VIEW_CONFLICT, OnViewConflict)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONFLICT, OnUpdateViewConflict)
	ON_COMMAND(ID_VIEW_UNKNOWN, OnViewUnknown)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNKNOWN, OnUpdateViewUnknown)
	ON_COMMAND(ID_VIEW_MISSING, OnViewMissing)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MISSING, OnUpdateViewMissing)
	ON_COMMAND(ID_VIEW_IGNORE, OnViewIgnore)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IGNORE, OnUpdateViewIgnore)
	ON_UPDATE_COMMAND_UI(ID_FILTERMASK, OnUpdateFilterMask)
	ON_UPDATE_COMMAND_UI(ID_HISTORYBROWSE, OnUpdateHistory)
	ON_COMMAND(ID_VIEW_ADDED, OnViewAdded)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ADDED, OnUpdateViewAdded)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REMOVED, OnUpdateViewRemoved)
	ON_COMMAND(ID_VIEW_REMOVED, OnViewRemoved)
	ON_COMMAND(ID_VIEW_CHANGED, OnViewChanged)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHANGED, OnUpdateViewChanged)
	ON_COMMAND(ID_VIEW_HIDEUNKNOWN, OnViewHideunknown)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDEUNKNOWN, OnUpdateViewHideunknown)
	ON_WM_TIMER()
	ON_COMMAND(ID_VIEW_CLEARALL, OnViewClearall)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLEARALL, OnUpdateViewClearall)
	ON_COMMAND(ID_VIEW_HIDEMISSING, OnViewHidemissing)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDEMISSING, OnUpdateViewHidemissing)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTERBAR_ENABLE, OnUpdateViewFilterbarEnable)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTERBAR_CLEARALL, OnUpdateViewFilterbarClearall)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTERBAR_OPTIONS, OnUpdateViewFilterbarOptions)
	ON_COMMAND(ID_VIEW_NEXTWINDOW, OnNextWindow)
	ON_COMMAND(ID_VIEW_PREVWINDOW, OnPrevWindow)
	//}}AFX_MSG_MAP
	ON_COMMAND_EX(ID_VIEW_BROWSER_BAR, CCJMDIFrameWnd::OnBarCheck)
	ON_COMMAND_EX(ID_VIEW_FILES_BAR, CCJMDIFrameWnd::OnBarCheck)
	ON_COMMAND_EX(ID_VIEW_FILTER_BAR, CCJMDIFrameWnd::OnBarCheck)
	ON_COMMAND_EX(ID_VIEW_MULTI_BAR, CCJMDIFrameWnd::OnBarCheck)
	ON_COMMAND_EX(ID_VIEW_TAGS_BAR, CCJMDIFrameWnd::OnBarCheck)
	ON_COMMAND_EX(ID_VIEW_CONSOLE_BAR, CCJMDIFrameWnd::OnBarCheck)
	ON_COMMAND_EX(ID_VIEW_WORKSPACE, CCJMDIFrameWnd::OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BROWSER_BAR, CCJMDIFrameWnd::OnUpdateControlBarMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILES_BAR, CCJMDIFrameWnd::OnUpdateControlBarMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTER_BAR, CCJMDIFrameWnd::OnUpdateControlBarMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MULTI_BAR, CCJMDIFrameWnd::OnUpdateControlBarMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGS_BAR, CCJMDIFrameWnd::OnUpdateControlBarMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONSOLE_BAR, CCJMDIFrameWnd::OnUpdateControlBarMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, CCJMDIFrameWnd::OnUpdateControlBarMenu)
	// Global help commands
	//ON_COMMAND(ID_HELP_FINDER, CCJMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CCJMDIFrameWnd::OnHelp)
	//ON_COMMAND(ID_CONTEXT_HELP, CCJMDIFrameWnd::OnContextHelp)
	//ON_COMMAND(ID_DEFAULT_HELP, CCJMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{	
	m_hRestoreFocus = NULL;

	LoadLocationSettings();
}

CMainFrame::~CMainFrame()
{
}

/*!
	Create and initialize docking windows
	\return TRUE on success, FALSE otherwise
*/
BOOL CMainFrame::InitializeDockingWindows()
{
	// Initial size when docked: top, bottom, left, right.
	// ??? SetInitialSize(125,225,225,225);
	
	if( !m_wndWorkspace.Create(this, ID_VIEW_WORKSPACE, _T("Workspace"), CSize(200, 100), CBRS_LEFT) )
	{
		TRACE0("Failed to create dialog bar (Workspace)\n");
		return FALSE;		// fail to create
	}

	m_wndWorkspace.AddView(_T("Modules"), RUNTIME_CLASS(CBrowserBar));

#ifndef NOEXPLOREVIEW 
	m_wndWorkspace.AddView(_T("Explore"), RUNTIME_CLASS(CExploreBar));
#endif

	m_wndWorkspace.SetActiveView(0);
	
	// Define the image list to use with the tab control
	m_TabImages.Create(IDR_WORKSPACE, 16, 1, RGB(255,255,255));
	m_wndWorkspace.SetTabImageList(&m_TabImages);
	
	//m_wndWorkspace.SetMenuID(IDR_POPUP);
	m_wndWorkspace.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);

	if( !m_consolebar.Create(this, ID_VIEW_CONSOLE_BAR, _T("Output"), CSize(300, 100), CBRS_BOTTOM) )
	{
		TRACE0("Failed to create dialog bar (Output)\n");
		return FALSE;		// fail to create
	}
	
	//m_consolebar.SetMenuID(IDR_POPUP);
	m_consolebar.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);

	EnableDockingSizeBar(CBRS_ALIGN_ANY);
	DockSizeBar(&m_wndWorkspace);
	DockSizeBar(&m_consolebar);

	return TRUE;
}

/// WM_CREATE message handler, create frame and toolbars
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CCJMDIFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;
	
	if( !CreateStatusBar() )
	{
		return -1;
	}

	EnableDocking(CBRS_ALIGN_ANY);

	if( !CreateToolbar("Standard", IDR_MAINFRAME, &m_wndToolBar) || 
		!CreateToolbar("Browse", IDR_BROWSER, &m_wndBrowser, &m_wndToolBar, ID_VIEW_BROWSER_BAR) || 
		!CreateToolbar("Files", IDR_FILELIST, &m_wndList, &m_wndBrowser, ID_VIEW_FILES_BAR) || 
		!CreateToolbar("Multi-Users", IDR_MULTIUSER, &m_wndMultiUser, &m_wndList, ID_VIEW_MULTI_BAR) || 
		!CreateToolbar("Tags", IDR_TAGS, &m_wndTags, &m_wndMultiUser, ID_VIEW_TAGS_BAR) || 
		!CreateToolbar("Filter", IDR_FILTER, &m_wndFilter, &m_wndTags, ID_VIEW_FILTER_BAR) )
	{
		return -1;
	}

	if( !CreateLocationCombo() )
	{
		return -1;
	}

	if( !InitializeDockingWindows() )
	{
		TRACE0("Failed to initialize docking windows\n");
		return -1;	// fail to create
	}

	LoadDockState();

	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);

	SetTimer(99, 200, 0L);

	return 0;
}

/*!
	Create status bar
	\return TRUE on success, FALSE otherwise
*/
BOOL CMainFrame::CreateStatusBar()
{
	if( !m_wndStatusBar.Create(this) )
	{
		TRACE0("Failed to create status bar\n");
		return FALSE;	// fail to create
	}

	return TRUE;
}

/*!
	Create the toolbar and dock it
	\param title Toolbar title
	\param toolbarID Toolbar resource ID
	\param toolBar Toolbar
	\param dockToolBar Dock toolbar, if NULL it will dock the toolbar to the top side of the frame window
	\param childID Toolbar's child window ID
	\return true on success, false otherwise
	\note We adjust toolbar height to 28 pixels because the default of 26 pixels seems a little bit too small
*/
BOOL CMainFrame::CreateToolbar(const char* title, const UINT toolbarID, CToolBar* toolBar, CToolBar* dockToolBar /*= NULL*/, UINT childID /*= AFX_IDW_TOOLBAR*/)
{
	if( !toolBar->CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(0, 0, 0, 0), childID) ||
		!toolBar->LoadToolBar(toolbarID) )
	{
		TRACE1("Failed to create toolbar (%s)\n", title);
		return FALSE;	// fail to create
	}

	toolBar->SetWindowText(title);
	toolBar->SetHeight(28);
	toolBar->SetBarStyle(toolBar->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	toolBar->EnableDocking(CBRS_ALIGN_ANY);
	
	if( dockToolBar )
	{
		DockControlBarLeftOf(toolBar, dockToolBar);
	}
	else
	{
		DockControlBar(toolBar, AFX_IDW_DOCKBAR_TOP);
	}

	return TRUE;
}

/*!
	Dock the control bar left of the given control bar
	\param toolBar Control bar to dock
	\param dockToolBar Control bar to dock to
*/
void CMainFrame::DockControlBarLeftOf(CToolBar* toolBar, CToolBar* dockToolBar)
{
	// Get MFC to adjust the dimensions of all docked ToolBars so that GetWindowRect will be accurate
	RecalcLayout();
	
	CRect rect;
	dockToolBar->GetWindowRect(&rect);
	rect.OffsetRect(1, 0);
	DWORD dwLeftOfStyle = dockToolBar->GetBarStyle();

	UINT dockBarID = 0;
	dockBarID = (dwLeftOfStyle & CBRS_ALIGN_TOP)						? AFX_IDW_DOCKBAR_TOP : dockBarID;
	dockBarID = (dwLeftOfStyle & CBRS_ALIGN_BOTTOM && dockBarID == 0)	? AFX_IDW_DOCKBAR_BOTTOM : dockBarID;
	dockBarID = (dwLeftOfStyle & CBRS_ALIGN_LEFT && dockBarID == 0)		? AFX_IDW_DOCKBAR_LEFT : dockBarID;
	dockBarID = (dwLeftOfStyle & CBRS_ALIGN_RIGHT && dockBarID == 0)	? AFX_IDW_DOCKBAR_RIGHT : dockBarID;

	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a separate line.  By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(toolBar, dockBarID, &rect);
}

/*!
	Create location combo box
	\return true on success, false otherwise
*/
BOOL CMainFrame::CreateLocationCombo()
{
	m_wndBrowser.SetButtonInfo(3, ID_SEPARATOR, TBBS_SEPARATOR, 200);

	CRect rect;
	m_wndBrowser.GetItemRect(3, &rect);
	rect.bottom += HISTLOC_VISIBLEITEMS * HISTLOC_IMAGE_CX + rect.bottom % HISTLOC_IMAGE_CX;
	
	m_browseHist.SetItems(&gHistoryLocs);

	if( !m_browseHist.Create(CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL, rect, &m_wndBrowser, ID_HISTORYBROWSE) )
	{
		TRACE0("Failed to create combobox (Browse History)\n");
		return FALSE;	// fail to create
	}

	// Older versions of Windows* (NT 3.51 for instance) fail with DEFAULT_GUI_FONT
	if( !m_fontNormal.CreateStockObject(DEFAULT_GUI_FONT) )
	{
		if( !m_fontNormal.CreatePointFont(80, "MS Sans Serif") )
		{
			TRACE0("Failed to create default GUI font\n");
			return FALSE;
		}
	}

	// Make browse history use default GUI font
	m_browseHist.SetFont(&m_fontNormal);

	return TRUE;
}

/*!
	Load the dock state, but check the toolbar exists (MFC bug)
*/
void CMainFrame::LoadDockState()
{
#if 1
	// 
	CDockState state;
	state.LoadState(_T("Bar State"));

	for(int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
	{
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
		if( !pInfo->m_bFloating )
		{
			pInfo->m_pBar = GetControlBar(pInfo->m_nBarID);
			if( pInfo->m_pBar == NULL )
			{
				delete (CControlBarInfo*)state.m_arrBarInfo[i];
				state.m_arrBarInfo.RemoveAt(i);
				i--;
			}
		}
	}

	SetDockState(state);
#else
	LoadBarState(_T("Bar State"));
#endif
	m_state.LoadWindowPos(this);
}

/// PreCreateWindow virtual override
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CCJMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CCJMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CCJMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

/// WM_CLOSE message handler, save windows positions
void CMainFrame::OnClose() 
{
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		if( app->IsCvsRunning() )
		{
			CvsAlert(kCvsAlertStopIcon, 
				_i18n("CVS is currently running."), _i18n("Stop CVS first before exiting."), 
				BUTTONTITLE_OK, NULL).ShowAlert();
			
			return;
		}
		
		// Write all persistent settings
		StoreLocationSettings();
		
		// Charybids Ltd's suggested fix:
		// Saving the window positions whilst iconic trashes the registry.
		if( !IsIconic() )
		{
			m_state.SaveWindowPos(this);
			SaveBarState(_T("Bar State"));
			
			WINDOWPLACEMENT wp;
			wp.length = sizeof wp;
			if( GetWindowPlacement(&wp) )
			{
				wp.flags &= WPF_RESTORETOMAXIMIZED;
				gMainPlacement = wp;
			}
		}
		
		// Only way I found to notify the browser and to prevent the memory leak support
		if( CWinCvsBrowser* pBrowser = app->GetBrowserView() )
		{
			pBrowser->ResetBrowser(NULL);	
		}
	}

	CCJMDIFrameWnd::OnClose();

	gCvsPrefs.save(true);
}

/// WM_DROPFILES message handler
void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	SetActiveWindow();	// activate us first !
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	CWinApp* pApp = AfxGetApp();
	for(UINT iFile = 0; iFile < nFiles; iFile++)
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);

		if( iFile == 0 )
		{
			CColorConsole out;
			out << kBold << "Locating: " << kNormal << kUnderline << szFileName << kNormal <<  kNormal << kNL;

			CWincvsApp* pApp = (CWincvsApp *)AfxGetApp();
			if( pApp )
			{
				const TCHAR *p1, *p2 = szFileName;
				
				while( p2 )
				{
					p1 = ++p2;
					p2 = strchr(p2, _T('\\'));
				}			

				if( CWinCvsBrowser* pBrowser = pApp->GetBrowserView() )
				{
					if( pBrowser->StepToLocation(szFileName, true) )
					{
						pApp->GetFileView()->ResetView(false, p1);
					}
				}
			}
		}

		// replace \ by /
		TCHAR *tmp = szFileName;
		while( (tmp = strchr(tmp, '\\')) != 0L )
		{
			*tmp++ = '/';
		}
		
		cvs_out("%s\n", szFileName);

		//pApp->OpenDocumentFile(szFileName);
	}//end for

	::DragFinish(hDropInfo);
}

/// Drop files
void CMainFrame::DropFiles(HDROP hDropInfo)
{
	OnDropFiles(hDropInfo);
}

/// WM_INITMENU message handler
void CMainFrame::OnInitMenu(CMenu* pMenu)
{
	CCJMDIFrameWnd::OnInitMenu(pMenu);
}

/*!
	Show and place window
	\param nCmdShow Show command
*/
void CMainFrame::InitialShowWindow(int nCmdShow)
{
	if( gMainPlacement.IsHasOne() ) 
	{
		WINDOWPLACEMENT wp = gMainPlacement.GetPlacement();
		
		if( nCmdShow != SW_SHOWNORMAL )
			wp.showCmd = nCmdShow;
		else if( wp.showCmd & SW_SHOWMINIMIZED )
			wp.showCmd = (wp.flags & WPF_RESTORETOMAXIMIZED) ? SW_MAXIMIZE : SW_SHOWNORMAL;
		
		SetWindowPlacement(&wp);
	}
	else
	{
		ShowWindow(nCmdShow);
	}
}

/// CBN_SELCHANGE message handler, change browse location
void CMainFrame::OnHistorySelect()
{
	ChangeLocation();
}

/// Change browse location
void CMainFrame::ChangeLocation()
{
	const int curSel = m_browseHist.GetCurSel();
	if( curSel > CB_ERR )
	{
		CString setText;
		m_browseHist.GetLBText(curSel, setText);
		
		if( !setText.IsEmpty() )
		{
			if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
			{
				CheckLocationDriveType(setText);

				if( CWinCvsBrowser* pBrowser = app->GetBrowserView() )
				{
					pBrowser->ResetBrowser(setText, true);
				}

				app->ActivateBrowserView();
			}
		}
	}
}

/*!
	Load location-specific settings
*/
void CMainFrame::LoadLocationSettings()
{
	m_filter.LoadSettings();
	m_recursion.LoadSettings();
	m_ignore.LoadSettings();

	m_filter.GetNotificationManager()->NotifyAll();
	m_recursion.GetNotificationManager()->NotifyAll();
	m_ignore.GetNotificationManager()->NotifyAll();
}

/*!
	Store location-specific settings
*/
void CMainFrame::StoreLocationSettings()
{
	m_filter.SaveSettings();
	m_recursion.SaveSettings();
	m_ignore.SaveSettings();
}

/// Get the browser view
CWinCvsBrowser* CMainFrame::GetBrowserView()
{
	CBrowserBar* bar = (CBrowserBar*)m_wndWorkspace.GetView(0);

	return bar->GetBrowserView();
}

/// Get the explorer view
CCJShellTree* CMainFrame::GetExplorerView()
{
	CExploreBar* bar = (CExploreBar*)m_wndWorkspace.GetView(1);
	return &bar->m_TreeCtrl;
}

/// Cmd msg routing on the dock bars
BOOL CMainFrame::OnBroadcastCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	CPushRoutingFrame push(this);

	CWincvsApp* app = (CWincvsApp*)AfxGetApp();

	// pump through dock bars FIRST
	CWinCvsBrowser* bView = app->GetBrowserView();
	if( bView != NULL && bView == CWnd::GetFocus() && bView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo) )
		return TRUE;

#ifndef NOEXPLOREVIEW
	// pump through dock bars FIRST
	CCJShellTree* eView = app->GetExplorerView();
	if( eView != NULL && eView == CWnd::GetFocus() )
	{	
		if( eView->GetParent()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo) )
			return TRUE;
	}
#endif

	CWincvsView* cView = app->GetConsoleView();
	if( cView != NULL && cView == CWnd::GetFocus() && cView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo) )
		return TRUE;

	return FALSE;
}

/// Handle window activation, prevent the focus to be gone from FileView to the Workspace - for the Dialogs etc
void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWincvsApp* pApp = (CWincvsApp*)AfxGetApp();
	ASSERT(pApp);

	if( WA_INACTIVE == nState && !m_hRestoreFocus )
	{
		m_hRestoreFocus = ::GetFocus();

		// Special case: The dropdown window of the history combo box gets destroyed
		// after deactivation, this renders m_hRestoreFocus invalid
		// Set focus to the combo box itself rather than to the dropdown window
		if( ::GetParent(m_hRestoreFocus) == m_browseHist.GetSafeHwnd() )
			m_hRestoreFocus = m_browseHist.GetSafeHwnd();

		pApp->SetBackgroundProcess(true);
	}
	else
	{
		pApp->SetBackgroundProcess(false);

		if( !bMinimized && m_hRestoreFocus )
		{
			if( ::IsWindow(m_hRestoreFocus) )
			{
				::SetFocus(m_hRestoreFocus);
			}

			m_hRestoreFocus = NULL;
		}
	}
}

/// WM_SYSCOMMAND message handler, prevent the focus to be gone from FileView to the Workspace - for the Minimize
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if( SC_MINIMIZE == nID )
	{
		CWincvsApp* pApp = (CWincvsApp*)AfxGetApp();
		ASSERT(pApp);
		
		m_hRestoreFocus = ::GetFocus();
	}

	CCJMDIFrameWnd::OnSysCommand(nID, lParam);
}

/*!
	Toggle the filter mask enable
*/
void CMainFrame::ToggleFilterMasksEnable()
{
	m_filter.ToggleFilterMasksEnable();
}

/*!
	Get the filter mask enable flag
	\return The filter mask enable flag
*/
bool CMainFrame::IsFilterMasksEnable() const
{
	return m_filter.IsFilterMasksEnable();
}

/*!
	Set the filter mask according to filter bar entries
	\param ctrlHeader Header control to get the filter bar entries from
	\param columnModel Column model
*/
void CMainFrame::SetFilterBarMask(CSortHeaderCtrl& ctrlHeader, const KiColumnModel* columnModel)
{
	const int columnCount = columnModel->GetCount();
	for(int nIndex = 0; nIndex < columnCount; nIndex++)
	{
		std::string filterText = ctrlHeader.GetFilterText(nIndex);
		const int filterType = columnModel->GetType(nIndex);

		m_filter.SetMask(filterType, filterText.c_str());
	}
}

/*!
	Clear the filter mask
*/
void CMainFrame::ClearFilters()
{
	m_filter.ClearAll();
}

/*!
	Set the column context
	\param context Context to be set
*/
void CMainFrame::SetContext(const KoColumnContext* context)
{
	m_filter.SetContext(context);
}

/// Toggle recursive view
void CMainFrame::OnViewRecursive() 
{
	m_recursion.ToggleRecursion();
	m_recursion.GetNotificationManager()->NotifyAll();
}

/// Update recursive view command
void CMainFrame::OnUpdateViewRecursive(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->SetCheck(m_recursion.IsShowRecursive());
}

/// Clear all filters
void CMainFrame::OnViewClearall() 
{
	m_filter.ClearAll();

	if( m_ignore.IsShowIgnored() )
	{
		m_ignore.ToggleIgnore();
	}

	m_filter.GetNotificationManager()->NotifyAll();
}

/// Update clear all filters command
void CMainFrame::OnUpdateViewClearall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled() && (m_filter.HasFilters() || m_ignore.IsShowIgnored()));
}

/// Toggle modified filter
void CMainFrame::OnViewModified() 
{
	m_filter.ToggleModified();
	m_filter.GetNotificationManager()->NotifyAll();
}

/// Update modified filter command
void CMainFrame::OnUpdateViewModified(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->SetCheck(m_filter.IsModified());
}

/// Toggle conflict filter
void CMainFrame::OnViewConflict() 
{
	m_filter.ToggleConflict();
	m_filter.GetNotificationManager()->NotifyAll();
}

/// Update conflict filter
void CMainFrame::OnUpdateViewConflict(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->SetCheck(m_filter.IsConflict());
}

/// Toggle unknown filter
void CMainFrame::OnViewUnknown() 
{
	m_filter.ToggleUnknown();
	m_filter.GetNotificationManager()->NotifyAll();
}

/// Update unknown filter
void CMainFrame::OnUpdateViewUnknown(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->SetCheck(m_filter.IsUnknown());
}

/// Toggle missing filter
void CMainFrame::OnViewMissing() 
{
	m_filter.ToggleMissing();
	m_filter.GetNotificationManager()->NotifyAll();
}

/// Update missing filter
void CMainFrame::OnUpdateViewMissing(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->SetCheck(m_filter.IsMissing());	
}

/// Toggle hide missing filter
void CMainFrame::OnViewHidemissing() 
{
	m_filter.ToggleHideMissing();
	m_filter.GetNotificationManager()->NotifyAll();
}

/// Update hide missing filter
void CMainFrame::OnUpdateViewHidemissing(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled() && !m_filter.HasAnyModified());
	pCmdUI->SetCheck(m_filter.IsHideMissing());	
}

/// Toggle ignore filter
void CMainFrame::OnViewIgnore() 
{
	m_ignore.ToggleIgnore();
	m_ignore.GetNotificationManager()->NotifyAll();
}

/// Update ignore filter
void CMainFrame::OnUpdateViewIgnore(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->SetCheck(m_ignore.IsShowIgnored());
}

/// Toggle added filter
void CMainFrame::OnViewAdded() 
{
	m_filter.ToggleAdded();
	m_filter.GetNotificationManager()->NotifyAll();
}

/// Update added filter
void CMainFrame::OnUpdateViewAdded(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->SetCheck(m_filter.IsAdded());
}

/// Toggle removed filter
void CMainFrame::OnUpdateViewRemoved(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->SetCheck(m_filter.IsRemoved());
}

/// Update removed filter
void CMainFrame::OnViewRemoved() 
{
	m_filter.ToggleRemoved();
	m_filter.GetNotificationManager()->NotifyAll();
}

/// Toggle changed filter
void CMainFrame::OnViewChanged() 
{
	m_filter.ToggleChanged();
	m_filter.GetNotificationManager()->NotifyAll();
}

/// Update changed filter
void CMainFrame::OnUpdateViewChanged(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->SetCheck(m_filter.IsChanged());
}

/// Toggle hide unknown filter
void CMainFrame::OnViewHideunknown() 
{
	m_filter.ToggleHideUnknown();
	m_filter.GetNotificationManager()->NotifyAll();
}

/// Update hide unknown filter
void CMainFrame::OnUpdateViewHideunknown(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled() && m_filter.IsDefaultMode());	
	pCmdUI->SetCheck(m_filter.IsHideUnknown());
}

/// Update filter mask
void CMainFrame::OnUpdateFilterMask(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());	
}

/// Update history
void CMainFrame::OnUpdateHistory(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());		
}

/*!
	Get the environment variable
	\param name Name of the variable to get
	\return The variable name
	\note It locks the sGetEnv semaphore
*/
const char* CMainFrame::GetEnvMainThread(const char* name)
{
	ASSERT(::GetCurrentThreadId() != AfxGetApp()->m_nThreadID);

	sGetEnvResult = name;
	sAskingMainThread = true;

	sGetEnv.Lock();

	return sGetEnvResult.c_str();
}

/// WM_TIMER message handler, get the environment variable and unlocks the sGetEnv semaphore 
void CMainFrame::OnTimer(UINT_PTR /*nIDEvent*/) 
{
	if( sAskingMainThread )
	{
		sAskingMainThread = false;

		sGetEnvResult = glue_getenv(sGetEnvResult.c_str(), NULL); // FIXME -> temporary
		sGetEnv.Unlock();
	}
}

/// Update filter bar enable
void CMainFrame::OnUpdateViewFilterbarEnable(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());	
	pCmdUI->SetCheck(m_filter.IsFilterMasksEnable());
}

/// Update clear all filter bar
void CMainFrame::OnUpdateViewFilterbarClearall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled() && m_filter.IsFilterMasksEnable() && m_filter.HasFilterMasks());	
}

/// Update filter bar options
void CMainFrame::OnUpdateViewFilterbarOptions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsAppEnabled());	
}

/// WinHelp virtual override, call HTML Help instead of WinHelp
void CMainFrame::WinHelp(DWORD dwData, UINT nCmd) 
{
	::HtmlHelp(*GetDesktopWindow(), AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, nCmd == HELP_CONTEXT ? dwData : 0);

	//CCJMDIFrameWnd::WinHelp(dwData, nCmd);
}

/// PreTranslateMessage virtual override, handle tab key to switch between windows
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN && 
		pMsg->wParam == VK_TAB && 
		!(0x8000 & GetKeyState(VK_CONTROL)) ) 
	{
		UINT uMessageId = (0x8000 & GetKeyState(VK_SHIFT)) ? ID_VIEW_PREVWINDOW : ID_VIEW_NEXTWINDOW;
		PostMessage(WM_COMMAND, MAKEWPARAM(uMessageId, 1), 0);
		return TRUE;
	}
	
	return CCJMDIFrameWnd::PreTranslateMessage(pMsg);
}

/// ID_VIEW_NEXTWINDOW message handler, go to the next window
void CMainFrame::OnNextWindow()
{
	CMDIChildWnd* pMDIChildWnd = MDIGetActive();
	CWnd* pMDIActive = pMDIChildWnd ? pMDIChildWnd->GetActiveView() : NULL;
	CWnd* pWndFocus = GetFocus();
	CWnd* pWnd = pWndFocus;
	BOOL bFound = FALSE;

	// Move focus to next window
	// Cycle through windows in the following order:
	// Console view -> browser view -> MDI view
	// Skip window if hidden
	// If in doubt, select MDI window

	// Go to next window
	if( !bFound && pWnd == pMDIActive )
	{
		if( pWnd = GetConsoleView() )
		{
			bFound = pWnd->IsWindowVisible();
		}
	}

	if( !bFound && pWnd == GetConsoleView() )
	{
		if( pWnd = GetBrowserView() )
		{
			bFound = (pWnd->IsWindowVisible());
		}
	}

	if( !bFound )
	{
		pWnd = pMDIActive;
	}

	if( pWnd && pWnd != pWndFocus )
	{
		pWnd->SetFocus();
	}
}

/// ID_VIEW_PREVWINDOW message handler, go to the next window
void CMainFrame::OnPrevWindow()
{
	CMDIChildWnd* pMDIChildWnd = MDIGetActive();
	CWnd* pMDIActive = pMDIChildWnd ? pMDIChildWnd->GetActiveView() : NULL;
	CWnd* pWndFocus = GetFocus();
	CWnd* pWnd = pWndFocus;
	BOOL bFound = FALSE;

	// Move focus to previous window
	// Cycle through windows in the following order:
	// Browser view -> console view -> MDI view
	// Skip window if hidden
	// If in doubt, select MDI window

	if( !bFound && pWnd == pMDIActive )
	{
		if( pWnd = GetBrowserView() )
		{
			bFound = pWnd->IsWindowVisible();
		}
	}

	if( !bFound && pWnd == GetBrowserView() ) 
	{
		if( pWnd = GetConsoleView() )
		{
			bFound = pWnd->IsWindowVisible();
		}
	}

	if ( !bFound ) 
	{
		pWnd = pMDIActive;
	}

	if( pWnd && pWnd != pWndFocus )
	{
		pWnd->SetFocus();
	}
}
