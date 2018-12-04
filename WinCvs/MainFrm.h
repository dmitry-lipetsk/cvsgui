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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com>
 */

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__D2D77DCC_8299_11D1_8949_444553540000__INCLUDED_)
#define AFX_MAINFRM_H__D2D77DCC_8299_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ConsoleBar.h"
#include "CvsStatusBar.h"
#include "HistComboBoxEx.h"
#include "SortHeaderCtrl.h"
#include "BrowseViewModel.h"
#include "BrowseViewColumn.h"

#define WINXP_MANIFEST_FIX

#if _MSC_VER <= 1200 && defined(WINXP_MANIFEST_FIX)
/*!
	Toolbar class to fix problems with manifest file on WinXP
	\note See kb843490 for details: http://support.microsoft.com/kb/843490/en
*/
class CWinToolBar : public CToolBar
{
public:
	void DrawGripper(CDC* pDC, const CRect& rect);
	void EraseNonClient();
	virtual void DoPaint(CDC* pDC);
	
protected:
	
	//{{AFX_MSG(CMyToolBar)
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};
#else
typedef CToolBar CWinToolBar;
#endif

/// Persistent window placement class
class CWinPlacement : public CPersistent
{
public:
	// Construction
	CWinPlacement(const char* uniqueName);
	virtual ~CWinPlacement();

private:
	// Data members
	WINDOWPLACEMENT m_placement;	/*!< Window placement data */
	bool m_hasOne;					/*!< Indicate the memory allocated */

	virtual unsigned int SizeOf(void) const;
	virtual const void* GetData(void) const;
	virtual void SetData(const void* ptr, unsigned int size);

public:
	// Interface
	CWinPlacement& operator=(const WINDOWPLACEMENT& newplace);

	WINDOWPLACEMENT GetPlacement() const;
	bool IsHasOne() const;
};

/// Main frame class
class CMainFrame : public CCJMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	// Construction
	CMainFrame();
	virtual ~CMainFrame();

// Attributes
protected:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  

// Generated message map functions
protected:

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnHistorySelect();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnViewRecursive();
	afx_msg void OnViewModified();
	afx_msg void OnUpdateViewModified(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewRecursive(CCmdUI* pCmdUI);
	afx_msg void OnViewConflict();
	afx_msg void OnUpdateViewConflict(CCmdUI* pCmdUI);
	afx_msg void OnViewUnknown();
	afx_msg void OnUpdateViewUnknown(CCmdUI* pCmdUI);
	afx_msg void OnViewMissing();
	afx_msg void OnUpdateViewMissing(CCmdUI* pCmdUI);
	afx_msg void OnViewIgnore();
	afx_msg void OnUpdateViewIgnore(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterMask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHistory(CCmdUI* pCmdUI);
	afx_msg void OnViewAdded();
	afx_msg void OnUpdateViewAdded(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewRemoved(CCmdUI* pCmdUI);
	afx_msg void OnViewRemoved();
	afx_msg void OnViewChanged();
	afx_msg void OnUpdateViewChanged(CCmdUI* pCmdUI);
	afx_msg void OnViewHideunknown();
	afx_msg void OnUpdateViewHideunknown(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnViewClearall();
	afx_msg void OnUpdateViewClearall(CCmdUI* pCmdUI);
	afx_msg void OnViewHidemissing();
	afx_msg void OnUpdateViewHidemissing(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFilterbarEnable(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFilterbarClearall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFilterbarOptions(CCmdUI* pCmdUI);
	afx_msg void OnNextWindow();
	afx_msg void OnPrevWindow();
	//}}AFX_MSG
	afx_msg void OnInitMenu(CMenu* pMenu);
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	KoFilterModel m_filter;			/*!< Filter model */
	KoRecursionModel m_recursion;	/*!< Recursion model */
	KoIgnoreModel m_ignore;			/*!< Ignore model */

	HWND m_hRestoreFocus;			/*!< Restore focus window */

	// Control bar embedded members
	CCvsStatusBar m_wndStatusBar;	/*!< Status bar */

	CWinToolBar m_wndToolBar;		/*!< File toolbar */
	CWinToolBar m_wndBrowser;		/*!< Browser toolbar */
	CWinToolBar m_wndMultiUser;		/*!< Multi user toolbar */
	CWinToolBar m_wndTags;			/*!< Tags toolbar */
	CWinToolBar m_wndList;			/*!< List toolbar */
	CWinToolBar m_wndFilter;		/*!< Filter toolbar */

	CHistComboBoxEx m_browseHist;	/*!< Browse history combo */
	CFont m_fontNormal;				/*!< Font */
	CBarConsole m_consolebar;		/*!< Console bar */
	CCJTabCtrlBar m_wndWorkspace;	/*!< Workspace bar */
	CImageList m_TabImages;			/*!< Workspace tab images */
	CCJWindowPlacement m_state;		/*!< Bar state */

	// Methods
	BOOL InitializeDockingWindows();
	BOOL CreateStatusBar();
	BOOL CreateToolbar(const char* title, const UINT toolbarID, CToolBar* toolBar, CToolBar* dockToolBar = NULL, UINT childID = AFX_IDW_TOOLBAR);
	void DockControlBarLeftOf(CToolBar* toolBar, CToolBar* dockToolBar);
	BOOL CreateLocationCombo();

	void LoadDockState();

	inline CMDIMenuList& GetMenuList(void);

public:
	// Interface
	void InitialShowWindow(int nCmdShow);

	BOOL OnBroadcastCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	
	void SetFilterBarMask(CSortHeaderCtrl& ctrlHeader, const KiColumnModel* columnModel);
	void ClearFilters();
	void SetContext(const KoColumnContext* context);
	void ToggleFilterMasksEnable();
	bool IsFilterMasksEnable() const;

	inline CWincvsView* GetConsoleView();
	inline CCJTabCtrlBar* GetWorkspace();
	inline CStatusBar* GetStatusBar();
	inline CHistComboBoxEx& GetBrowseHistory(void);
	CCJShellTree* GetExplorerView();
	CWinCvsBrowser* GetBrowserView();

	const char* GetEnvMainThread(const char* name);

	inline KiFilterModel* GetFilterModel();
	inline KiRecursionModel* GetRecursionModel();
	inline KiIgnoreModel* GetIgnoreModel();

	void DropFiles(HDROP hDropInfo);
	
	void ChangeLocation();
	void LoadLocationSettings();
	void StoreLocationSettings();
};

//////////////////////////////////////////////////////////////////////////
// Inline implementation

/// Get the console view
inline CWincvsView* CMainFrame::GetConsoleView()
{
	return m_consolebar.m_view;
}

/// Get workspace
inline CCJTabCtrlBar* CMainFrame::GetWorkspace()
{
	return &m_wndWorkspace;
}

/// Get the status bar
inline CStatusBar* CMainFrame::GetStatusBar()
{
	return &m_wndStatusBar;
}

/// Get the browse location combo box
inline CHistComboBoxEx& CMainFrame::GetBrowseHistory(void)
{
	return m_browseHist;
}

/// Get the filter model implementation
inline KiFilterModel* CMainFrame::GetFilterModel()
{
	return &m_filter;
}

/// Get the recursion model implementation
inline KiRecursionModel* CMainFrame::GetRecursionModel()
{
	return &m_recursion;
}

/// Get the ignore model implementation
inline KiIgnoreModel* CMainFrame::GetIgnoreModel()
{
	return &m_ignore;
}

/// Get the menu list
inline CMDIMenuList& CMainFrame::GetMenuList(void) 
{
	return m_menuList;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__D2D77DCC_8299_11D1_8949_444553540000__INCLUDED_)
