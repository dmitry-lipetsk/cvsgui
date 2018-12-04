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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

/*
 * BrowseFileView.h -- Implements the view with all the files
 * 
 * FILTER-VIEW-DEV branch, last merged up to 1.22
 */

#if !defined(AFX_BROWSEFILEVIEW_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_BROWSEFILEVIEW_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CvsEntries.h"
#include "SortHeaderCtrl.h"
#include "FileTraversal.h"

/// Schema for column settings, change whenever the columns change to prevent erratic column order when upgrading to a new version
#define STR_FILEDETAILVIEW_COLUMN_SCHEMA		"1"

#define STR_FILEDETAILVIEW_COLUMN_SECTION		"FileView" STR_FILEDETAILVIEW_COLUMN_SCHEMA
#define STR_FILEDETAILVIEW_COLUMN_ENTRY_WIDTH	"Column Widths"
#define STR_FILEDETAILVIEW_COLUMN_ENTRY_ORDER	"Column Order"
#define STR_FILEDETAILVIEW_COLUMN_ENTRY_SORT	"Column Sort"
#define STR_FILEDETAILVIEW_COLUMN_ENTRY_SORTASC	"Column SortAsc"
#define CHR_FILEDETAILVIEW_COLUMN_DATASEP		','
#define STR_FILEDETAILVIEW_COLUMN_PREFIX_REG	"Regular "
#define STR_FILEDETAILVIEW_COLUMN_PREFIX_FLAT	"Flat "

class MultiFiles;
class KoWatcher;
class KiSelectionHandler;
class KiColumnModel;
class KiFilterModel;
class CBrowseFileView;

/// File/folder icons enum
enum
{
	kFileIconText = 0,		/*!< Text file */
	kFileIconBinary,		/*!< Binary file */
	kFileIconUnknown,		/*!< Unknown file */
	kFileIconAdded,			/*!< Added file */
	kFileIconConflict,		/*!< Conflict file */
	kFileIconResolved,		/*!< Resolved conflict file */
	kFileIconMiss,			/*!< Missing file */
	kFolderIcon,			/*!< Folder */
	kFolderIconUnknown,		/*!< Unknown folder */
	kFolderIconMiss,		/*!< Missing folder */
	kFolderIconOneUp,		/*!< Up one folder */
	kFileIconTextMod,		/*!< Modified text file */
	kFileIconBinaryMod,		/*!< Modified binary file */
	kFileIconIgnored,		/*!< Ignored file */
	kFolderIconIgnored,		/*!< Ignored folder */
	kFileIconRemoved,		/*!< Removed file */
	kFileIconBinaryAdded,	/*!< Added binary file */
	kFileIconUnicode,		/*!< Unicode file */
	kFileIconUnicodeMod,	/*!< Modified unicode file */
	kFileIconUnicodeAdded,	/*!< Added unicode file  */
	kFileIconExtended,		/*!< Extended encoding file */
	kFileIconExtendedMod,	/*!< Modified extended encoding file */
	kFileIconExtendedAdded	/*!< Added extended encoding file  */
};

//////////////////////////////////////////////////////////////////////////
// CRecursionLock

/// Class to prevent the recursive calls
class CRecursionLock
{
public:
	// Construction
	CRecursionLock(bool& semaphore) ;
	virtual ~CRecursionLock();

private:
	// Data members
	bool& m_semaphore;
};

//////////////////////////////////////////////////////////////////////////
// KoFileViewTraversalContext

/// Context for traversing
class KoFileViewTraversalContext
{	
public:
    // Construction
	KoFileViewTraversalContext(CBrowseFileView* view);

private:
	// Data members
    bool m_isRecursive;			/*!< Recursive flag */
	bool m_isShowIgnored;		/*!< Show ignored flag */
	KiFilterModel* m_filter;	/*!< Filter model */

	CBrowseFileView* m_view;	/*!< File view */

public:
	// Interface
	bool IsRecursive() const;
	bool IsShowIgnored() const;
	bool IsShowUnknown() const;
	bool IsHideUnknown() const;
	bool IsShowMissing() const;
	bool IsShowModified() const;

	bool IsMatch(EntnodeData* data) const;

	CBrowseFileView* GetView() const;
};

//////////////////////////////////////////////////////////////////////////
// TViewFill

/// Fill the view
class TViewFill : public TraversalReport
{
public:
    // Construction
	TViewFill(const KoFileViewTraversalContext* context);

private:
	// Data members
	CSortList<ENTNODE> m_entries;					/*!< Entries list */
	ignored_list_type m_ignlist;					/*!< Ignore list */
	const KoFileViewTraversalContext* m_context;	/*!< Traversal context */

public:
	// Interface
	int InitItem(EntnodeData* data, int iImage, const char* tag = 0L);

	virtual kTraversal EnterDirectory(const char* fullpath, const char* dirname, const FSSpec* macspec);
	virtual kTraversal ExitDirectory(const char* fullpath);
	virtual kTraversal OnError(const char* err, int errcode);
	virtual kTraversal OnIdle(const char* fullpath);
	
	virtual kTraversal OnDirectory(const char* fullpath, const char* fullname, const char* name,
		const struct stat& dir, const FSSpec* macspec);
	
	virtual kTraversal OnAlias(const char* fullpath, const char* fullname, const char* name,
		const struct stat& dir, const FSSpec* macspec);

	virtual kTraversal OnFile(const char* fullpath, const char* fullname, const char* name,
		const struct stat& dir, const FSSpec* macspec);
};


//////////////////////////////////////////////////////////////////////////
// CBrowseFileView

class CBrowseFileView : public CListView
{
protected: // create from serialization only
	CBrowseFileView();
	DECLARE_DYNCREATE(CBrowseFileView)

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseFileView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBrowseFileView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CBrowseFileView)
	afx_msg void OnViewSmallIcons();
	afx_msg void OnViewList();
	afx_msg void OnViewFullRowDetails();
	afx_msg void OnUpdateViewSmallIcons(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewAddB(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewRelease(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewRmv(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewGraph(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewList(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewEditseldef(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFullRowDetails(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnViewAdd();
	afx_msg void OnViewAddb();
	afx_msg void OnViewCommit();
	afx_msg void OnViewRmv();
	afx_msg void OnViewUpdate();
	afx_msg void OnViewQueryUpdate();
	afx_msg void OnUpdateViewReload(CCmdUI* pCmdUI);
	afx_msg void OnViewReload();
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnViewUpone();
	afx_msg void OnUpdateViewUpone(CCmdUI* pCmdUI);
	afx_msg void OnViewTrash();
	afx_msg void OnUpdateViewTrash(CCmdUI* pCmdUI);
	afx_msg void OnViewDiff();
	afx_msg void OnViewLog();
	afx_msg void OnViewGraph();
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
	afx_msg void OnViewNewInstance();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnViewEditsel();
	afx_msg void OnViewEditseldef();
	afx_msg void OnViewOpensel();
	afx_msg void OnViewOpenselas();
	afx_msg void OnViewCheckout();
	afx_msg void OnUpdateViewCheckout(CCmdUI* pCmdUI);
	afx_msg void OnViewImport();
	afx_msg void OnUpdateViewImport(CCmdUI* pCmdUI);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginrdrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateViewAnnotate(CCmdUI* pCmdUI);
	afx_msg void OnAnnotate();
	afx_msg void OnEditSelectAll();
	afx_msg void OnViewReservededit();
	afx_msg void OnViewForceedit();
	afx_msg void OnViewAddu();
	afx_msg void OnAppCmdline();
	afx_msg void OnUpdateCvsCmd(CCmdUI* pCmdUI);
	afx_msg void OnAppRtagbranch();
	afx_msg void OnAppRtagcreate();
	afx_msg void OnAppRtagdelete();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewFilterbarEnable();
	afx_msg void OnViewFilterbarClearall();
	afx_msg void OnViewFilterbarOptions();
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteallitems(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnQueryShellactionDefault();
	afx_msg void OnQueryProperties();
	afx_msg void OnUpdateQueryProperties(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnUpdateQueryCompare(CCmdUI* pCmdUI);
	afx_msg void OnQueryCompare();
	afx_msg void OnUpdateViewEditselconflict(CCmdUI* pCmdUI);
	afx_msg void OnViewEditselconflict();
	afx_msg void OnViewOpenselannotate();
	afx_msg void OnUpdateViewOpenselannotate(CCmdUI* pCmdUI);
	afx_msg void OnViewExport();
	afx_msg void OnUpdateViewExport(CCmdUI* pCmdUI);
	afx_msg void OnFormatFont();
	afx_msg void OnQueryCommandprompt();
	//}}AFX_MSG
	afx_msg void OnUpdateMacro(CCmdUI* pCmdUI);
	afx_msg BOOL OnMacro(UINT nID);
	afx_msg void OnUpdateCustomize(CCmdUI* pCmdUI);
	afx_msg BOOL OnCustomize(UINT nID);
	afx_msg LRESULT OnFileChange(WPARAM, LPARAM);
	afx_msg LRESULT OnResetView(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
private:
	// Data members
	std::string m_path;		/*!< Current path */
	
	int m_numberOfLines;	/*!< Number of lines (for printing) */
	int m_numberByPages;	/*!< Number by pages (for printing) */
	int m_numberOfPages;	/*!< Number of pages (for printing) */
	int m_heightOfLines;	/*!< Height of lines (for printing) */

	bool m_isRecursive;			/*!< true if displaying recursively */
	bool m_isResetViewLocked;	/*!< Flag to lock out recursion into ResetView() */
	bool m_isResetViewPending;	/*!< Flag to indicate whether there is an outstanding request to refresh the view contents */

	bool m_checkChanges;		/*!< Flag to force checking the changes */
	KoWatcher* m_watch;			/*!< Thread to monitor file change notifications */
	time_t m_entriesMod;		/*!< Time stamp of CVS/Entries */
	time_t m_entriesLogMod;		/*!< CVS/Entries.log */
	bool m_isFilesChange;		/*!< Flag to indicate whether a file change notification is pending */
	UINT m_uiFilechangeTimer;	/*!< Stores the timer event for file's change notification, default to FILECHANGE_TIMER */

	CSortHeaderCtrl m_ctrlHeader;	/*!< Sort header control */
	int m_sortRegular;				/*!< Regular sort key */
	int m_sortFlat;					/*!< Flat sort key */
	bool m_ascendantRegular;		/*!< Regular sort flag */
	bool m_ascendantFlat;			/*!< Flat sort flag */

	KiColumnModel* m_modelColumns;					/*!< Columnar model in use */
	std::map<int, std::string> m_filterBarFilters;	/*!< Cache of filter bar column filters text */

	static CImageList m_smallImageList;	/*!< File/folder image list */
	static CImageList m_stateImageList;	/*!< File state image list */
	CFont* m_font;						/*!< Font */

	// Methods
	void Resort(void);

	BOOL SetViewType(DWORD dwViewType);
	DWORD GetViewType() const;

	void GetEntriesModTime(time_t& newEntriesMod, time_t& newEntriesLogMod);
	
	void EnterFolder(const char* fullpath);

	void DoDrag();
	void SetRecursive(bool isRecursive);

	bool ProcessSelectionCommand(KiSelectionHandler* handler);

	bool DisableCommon(bool bCheckSelection = true);

	CString FormatEntrryPrefix(bool isRecursive) const;
	void SaveColumns(bool isRecursive);
	bool LoadColumns(bool isRecursive);
	void AddColumns();
	void DeleteColumns();

	int GetPathLength(LPCTSTR path);
	bool IsSamePath(LPCTSTR path1, LPCTSTR path2);

public:
	// Interface
	inline const std::string& GetPath() const;
	
	inline KiColumnModel* GetColumnModel() const;
	inline void CheckChanges(void);

    void OnFilteringChanged();
    void OnFilterBarChanged();

	void ResetView(bool notifyBrowser = false, const char* selectFile = NULL, bool forceReload = true, const char* selectFolder = NULL);
	void ResetView(const char* path, bool notifyBrowser = false, const char* selectFile = NULL, bool forceReload = true, const char* selectFolder = NULL);

	static int GetImageForEntry(const EntnodeData* data);
};

//////////////////////////////////////////////////////////////////////////
// Inline implementation

/// Get the current path
const std::string& CBrowseFileView::GetPath() const
{
	return m_path;
}

/// Get the columnar model of the data
inline KiColumnModel* CBrowseFileView::GetColumnModel() const
{
	return m_modelColumns;
}

/// Force the checking for changes after CVS command ends
inline void CBrowseFileView::CheckChanges(void)
{
	m_checkChanges = true;
}

#endif // !defined(AFX_BROWSEFILEVIEW_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
