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

#if !defined(AFX_WINCVSBROWSER_H__144A4971_CAB4_11D1_BC80_000000000000__INCLUDED_)
#define AFX_WINCVSBROWSER_H__144A4971_CAB4_11D1_BC80_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WinCvsBrowser.h : header file
//

#include <afxcview.h>
#include "FileTraversal.h"
#include "BrowseFileView.h"

class KiSelectionHandler;
class CWinCvsBrowser;
class EntnodeData;

/*!
	Class to ensure that selection notification is properly issued
	\note Constructor can be configured either to issue notification or discard it
*/
class CSelectionChangeNotifier
{
public:
    // Construction
	CSelectionChangeNotifier(CWinCvsBrowser* view, bool notify);
	virtual ~CSelectionChangeNotifier();

private:
	// Data members
	bool m_notify;			/*!< If true, the object will issue proper selection notification when going out of scope */
	bool m_wasLocked;		/*!< Flag to indicate whether the view was already locked at the time of initialization */
	CWinCvsBrowser* m_view;	/*!< View to change the state */
};

/// Class to create an item with a dummy item inside if and only if the folder is not empty
class TBrowserFillDummy : public TraversalReport
{
public:
	// Construction
	TBrowserFillDummy(CTreeCtrl& treeCtrl, HTREEITEM root, HTREEITEM item, EntnodeData* data);

private:
	// Data members
	HTREEITEM m_root;		/*!< Root item handle */
	HTREEITEM m_item;		/*!< Current item handle */
	CTreeCtrl& m_treeCtrl;	/*!< Tree control */
	bool m_empty;			/*!< Empty flag */
	bool m_hascvs;			/*!< CVS meta-data flag */
	EntnodeData* m_data;	/*!< Node data */

	// Methods
	virtual kTraversal EnterDirectory(const char* fullpath, const char* dirname, const FSSpec* macspec);
	virtual kTraversal ExitDirectory(const char* fullpath);
	virtual kTraversal OnError(const char* err, int errcode);
	virtual kTraversal OnIdle(const char* fullpath);
	virtual kTraversal OnDirectory(const char* fullpath, const char* fullname, const char* name,
		const struct stat& dir, const FSSpec* macspec);

public:
	// Interface

	/// Get the current item
	inline HTREEITEM GetItem() const { return m_item; };
};

/// Class to create an item with all the subdirectories inside for a given path and a root item
class TBrowserFill : public TraversalReport
{
public:
	// Construction
	TBrowserFill(CTreeCtrl& treeCtrl, HTREEITEM root, HTREEITEM item, CSortList<ENTNODE>& entries);

private:
	// Data members
	HTREEITEM m_root;				/*!< Root item handle */
	HTREEITEM m_item;				/*!< Current item handle */
	CTreeCtrl& m_treeCtrl;			/*!< Tree control */
	
	bool m_hascvs;					/*!< CVS meta-data flag */
	CSortList<ENTNODE>& m_entries;	/*!< Entries list */
	ignored_list_type m_ignlist;	/*!< Ignore list */

	bool m_isShowIgnored;			/*!< Show ignored flag */
	bool m_isHideUnknown;			/*!< Hide unknown flag */
	bool m_isHideMissing;			/*!< Hide missing flag */

	// Methods
	virtual kTraversal EnterDirectory(const char* fullpath, const char* dirname, const FSSpec* macspec);
	virtual kTraversal ExitDirectory(const char* fullpath);
	virtual kTraversal OnError(const char* err, int errcode);
	virtual kTraversal OnIdle(const char* fullpath);
	virtual kTraversal OnDirectory(const char* fullpath, const char* fullname, const char* name, 
		const struct stat& dir, const FSSpec* macspec);

public:
	// Interface
	bool IsHideMissing() const;
};

/////////////////////////////////////////////////////////////////////////////
// CWinCvsBrowser view

class CWinCvsBrowser : public CTreeView
{
protected:
	DECLARE_DYNCREATE(CWinCvsBrowser)

	CWinCvsBrowser();           // protected constructor used by dynamic creation
	virtual ~CWinCvsBrowser();

// Attributes
protected:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinCvsBrowser)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CWinCvsBrowser)
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateViewAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnViewAdd();
	afx_msg void OnViewCommit();
	afx_msg void OnViewUpdate();
	afx_msg void OnViewQueryUpdate();
	afx_msg void OnViewDiff();
	afx_msg void OnViewLog();
	afx_msg void OnViewStatus();
	afx_msg void OnViewUnlock();
	afx_msg void OnViewWatchOn();
	afx_msg void OnViewWatchOff();
	afx_msg void OnViewEdit();
	afx_msg void OnViewUnedit();
	afx_msg void OnViewWatchers();
	afx_msg void OnViewEditors();
	afx_msg void OnViewRelease();
	afx_msg void OnViewTagNew();
	afx_msg void OnViewTagDelete();
	afx_msg void OnViewTagBranch();
	afx_msg void OnViewExplore();
	afx_msg void OnUpdateViewExplore(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewReload(CCmdUI* pCmdUI);
	afx_msg void OnViewReload();
	afx_msg void OnViewCheckout();
	afx_msg void OnUpdateViewCheckout(CCmdUI* pCmdUI);
	afx_msg void OnViewImport();
	afx_msg void OnUpdateViewImport(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnViewReservededit();
	afx_msg void OnViewForceedit();
	afx_msg void OnAppCmdline();
	afx_msg void OnAppRtagbranch();
	afx_msg void OnAppRtagcreate();
	afx_msg void OnAppRtagdelete();
	afx_msg void OnUpdateAppCmdline(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnQueryProperties();
	afx_msg void OnUpdateQueryProperties(CCmdUI* pCmdUI);
	afx_msg void OnViewNewInstance();
	afx_msg void OnViewRmv();
	afx_msg void OnViewExport();
	afx_msg void OnUpdateViewExport(CCmdUI* pCmdUI);
	afx_msg void OnFormatFont();
	afx_msg void OnQueryCommandprompt();
	//}}AFX_MSG
	afx_msg void OnUpdateMacro(CCmdUI* pCmdUI);
	afx_msg BOOL OnMacro(UINT nID);
	afx_msg void OnUpdateCustomize(CCmdUI* pCmdUI);
	afx_msg BOOL OnCustomize(UINT nID);
	afx_msg LRESULT OnResetView(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
private:
	// Data members
	std::string m_root;					/*!< Current path of the browser */
	static CImageList m_imageList;		/*!< Several folders aspects image list */
	CFont* m_font;						/*!< Font */

    bool m_selectionNotificationLocked;	/*!< Flag to indicate whether the propagating selection changes is currently locked out */
    bool m_selectionChanged;			/*!< Flag to indicate whether selection indeed changed since it was set to true, valid if m_isSelectionNotificationLocked is true */
	bool m_resetViewPending;			/*!< Flag to indicate whether request to rebuild the view is pending */

	// Methods
	void StoreExpanded(std::vector<std::string>& allExpanded, HTREEITEM root = 0L);

	bool DisableCommon();
	void OnUpdateCmd(CCmdUI* pCmdUI, BOOL needCvsInfos);

	void SetNewStyle(long lStyleMask, BOOL bSetBits);
	HTREEITEM GetItemByLocation(const char* path);
	void DeleteAllItems(HTREEITEM root = 0L);
	void RetrievePath(HTREEITEM item, std::string& path);

	bool ProcessSelectionCommand(KiSelectionHandler* handler);

public:
	// Interface
	void SetInitialRoot();
	void ResetView(bool forceReload = false, bool notifyView = false);
	bool StepToLocation(const char* path, bool notifyView = false);
	void ResetBrowser(const char* path, bool notifyView = false);
	
	void OnIgnoreChanged();
	void OnFilteringChanged();
	void NotifySelectionChange();

	inline const std::string& GetRoot(void) const;
	inline bool GetSelectionNotificationLocked() const;
	inline void SetSelectionNotificationLocked(bool selectionNotificationLocked);
	inline bool GetSelectionChanged() const;
	inline void SetSelectionChanged(bool selectionChanged);
};

/// Get the current root
inline const std::string& CWinCvsBrowser::GetRoot(void) const
{
	return m_root;
}

/// Get the selection notification locked state
inline bool CWinCvsBrowser::GetSelectionNotificationLocked() const
{
	return m_selectionNotificationLocked;
}

/// Set the selection notification locked state
inline void CWinCvsBrowser::SetSelectionNotificationLocked(bool selectionNotificationLocked)
{
	m_selectionNotificationLocked = selectionNotificationLocked;
}

/// Get the selection changed state
inline bool CWinCvsBrowser::GetSelectionChanged() const
{
	return m_selectionChanged;
}

/// Set the selection changed state
inline void CWinCvsBrowser::SetSelectionChanged(bool selectionChanged)
{
	m_selectionChanged = selectionChanged;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCVSBROWSER_H__144A4971_CAB4_11D1_BC80_000000000000__INCLUDED_)
