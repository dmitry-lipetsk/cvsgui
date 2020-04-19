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

// WinCvsBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "MainFrm.h"
#include "WinCvsBrowser.h"
#include "AppConsole.h"
#include "CvsPrefs.h"
#include "CvsCommands.h"
#include "TclGlue.h"
#include "PythonGlue.h"
#include "BrowseViewHandlers.h"
#include "CustomizeMenus.h"
#include "umain.h"

using namespace std;

#ifdef WIN32
#	include "resource.h"
#endif /* WIN32 */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Folder icons enum
enum
{
	kIconFolderInvalid = -1,	/*!< Invalid icon */
	kIconFolderClosed = 0,		/*!< Closed folder */
	kIconFolderOpened,			/*!< Opened folder */
	kIconFolderCVSClosed,		/*!< Closed CVS folder */
	kIconFolderCVSOpened,		/*!< Opened CVS folder */
	kIconFolderIgnoreClosed,	/*!< Closed ignored folder */
	kIconFolderIgnoreOpened,	/*!< Opened ignored folder */
	kIconFolderLostClosed,		/*!< Closed missing CVS folder */
	kIconFolderLostOpened		/*!< Opened missing CVS folder */
};

/// Application message to reset the view
static const int WM_RESETVIEW = WM_APP + 12;

/// Empty font
static LOGFONT emptyLogFont = { 0 };

/// Dummy item text
static const char* gDummyFile = "@@@dummy file@@@";

CPersistentT<LOGFONT> gModulesFont("P_ModulesFont", emptyLogFont, kNoClass);

/*!
	Helper method to set the appropriate icon for a given node
	\param treeCtrl	Tree control to set the icon
	\param item Item handle to set the icon
	\param hasCvsInfos true for CVS folder, false otherwise
	\param data	Node data
	\return true on success, false otherwise
*/
static void SetIcon(CTreeCtrl& treeCtrl, HTREEITEM item, bool hasCvsInfos, EntnodeData* data = 0L)
{
	TV_ITEM pItem;
	pItem.hItem = item;
	pItem.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	VERIFY(treeCtrl.GetItem(&pItem));

	int newImage = kIconFolderInvalid;
	
	if( data == 0L )
	{
		newImage = hasCvsInfos ? kIconFolderCVSClosed : kIconFolderClosed;
	}
	else
	{
		if( data->IsIgnored() )
			newImage = kIconFolderIgnoreClosed;
		else if( data->IsUnknown() )
			newImage = kIconFolderClosed;
		else
			newImage = kIconFolderCVSClosed;
	}

	if( kIconFolderInvalid != newImage && pItem.iImage != newImage )
	{
		pItem.iImage = newImage;
		pItem.iSelectedImage = newImage + 1;
		pItem.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		treeCtrl.SetItem(&pItem);
	}
}

/*!
	Ignore changed notification function
	\param obj Browser view to be notified
*/
static void OnIgnoreChanged(CObject* obj)
{
	((CWinCvsBrowser*)obj)->OnIgnoreChanged();
}

/*!
	Filtering changed notification function
	\param obj Browse file view to be notified
*/
static void OnFilteringChanged(CObject* obj)
{
	((CWinCvsBrowser*)obj)->OnFilteringChanged();
}


//////////////////////////////////////////////////////////////////////////
// CSelectionChangeNotifier

CSelectionChangeNotifier::CSelectionChangeNotifier(CWinCvsBrowser* view, bool notify)
	: m_notify(notify), m_view(view), m_wasLocked(view->GetSelectionNotificationLocked())
{ 
	if( !m_wasLocked )
	{
		m_view->SetSelectionChanged(false);
		m_view->SetSelectionNotificationLocked(true);
	}
}

CSelectionChangeNotifier::~CSelectionChangeNotifier()
{
	m_view->SetSelectionNotificationLocked(m_wasLocked);
	if( !m_wasLocked )
	{
		if( m_notify )
		{
			m_view->NotifySelectionChange();
		}
		
		m_view->SetSelectionChanged(false);
	}
}

//////////////////////////////////////////////////////////////////////////
// TBrowserFillDummy

TBrowserFillDummy::TBrowserFillDummy(CTreeCtrl& treeCtrl, HTREEITEM root, HTREEITEM item, EntnodeData* data) 
	: m_root(root), m_item(item), m_treeCtrl(treeCtrl), m_empty(true), m_data(data), m_hascvs(false) 
{
}

kTraversal TBrowserFillDummy::EnterDirectory(const char* fullpath, const char* dirname, const FSSpec* macspec)
{
	if( m_item == 0L )
	{
		TV_INSERTSTRUCT tvstruct;
		tvstruct.hParent = m_root;
		tvstruct.hInsertAfter = TVI_SORT;
		tvstruct.item.iImage = kIconFolderClosed;
		tvstruct.item.iSelectedImage = kIconFolderOpened;
		tvstruct.item.pszText = IsNullOrEmpty(dirname) ? (char*)fullpath : (char*)dirname;
		tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
		m_item = m_treeCtrl.InsertItem(&tvstruct);
	}
	
	// Assign the entries to this item
	if( m_data != 0L )
	{
		EntnodeData* data = (EntnodeData*)m_treeCtrl.GetItemData(m_item);
		
		if( m_data != 0L )
			m_data->Ref();
		
		m_treeCtrl.SetItemData(m_item, (long)m_data);
		
		if( data != 0L )
			data->UnRef();
	}
	else
		m_data = (EntnodeData*)m_treeCtrl.GetItemData(m_item);
	
	return kContinueTraversal;
}

kTraversal TBrowserFillDummy::ExitDirectory(const char* fullpath)
{
	SetIcon(m_treeCtrl, m_item, m_hascvs, m_data);
	
	if( m_empty )
		return kContinueTraversal;
	
	// Add a dum': vc*1fg ;lm   my item to simulate the folder
	// is not empty. This dummy item will be
	// replaced by a complete listing when the
	// node is expanding...
	TV_INSERTSTRUCT tvstruct;
	tvstruct.hParent = m_item;
	tvstruct.hInsertAfter = TVI_SORT;
	tvstruct.item.pszText = (char*)gDummyFile;
	tvstruct.item.mask = TVIF_TEXT;
	m_treeCtrl.InsertItem(&tvstruct);
	
	return kContinueTraversal;
}

kTraversal TBrowserFillDummy::OnError(const char* err, int errcode)
{
	return kTraversalError;
}

kTraversal TBrowserFillDummy::OnIdle(const char* fullpath)
{
	return kContinueTraversal;
}

kTraversal TBrowserFillDummy::OnDirectory(const char* fullpath, const char* fullname, const char* name,
							   const struct stat& dir, const FSSpec* macspec)
{
	if( stricmp(name, "cvs") == 0 )
		m_hascvs = true;
	else
		m_empty = false;

	CSortList<ENTNODE> entries(200, ENTNODE::Compare);
	ignored_list_type ignlist;
	
	string uppath, folder;
	if( !SplitPath(fullpath, uppath, folder) || uppath.empty() )
	{
		uppath = fullpath;
		folder = ".";
	}
	
	BuildIgnoredList(ignlist, uppath.c_str());
	EntnodeData* data = Entries_SetVisited(uppath.c_str(), entries, folder.c_str(), dir, true, &ignlist);
	data->Ref();

	if( EntnodeData* oldData = (EntnodeData*)m_treeCtrl.GetItemData(m_item) )
		oldData->UnRef();

	m_treeCtrl.SetItemData(m_item, (long)data);

	return kSkipFile;
}

//////////////////////////////////////////////////////////////////////////
// TBrowserFill

TBrowserFill::TBrowserFill(CTreeCtrl& treeCtrl, HTREEITEM root, HTREEITEM item, CSortList<ENTNODE>& entries) 
	: m_root(root), m_item(item), m_treeCtrl(treeCtrl), m_hascvs(false), m_entries(entries), 
	m_isShowIgnored(false), m_isHideUnknown(false), m_isHideMissing(false)
{
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
    { 
		m_isShowIgnored = app->GetIgnoreModel()->IsShowIgnored();
		m_isHideUnknown = (app->GetFilterModel()->GetFilters() & kFilterHideUnknown) != 0;
		m_isHideMissing = (app->GetFilterModel()->GetFilters() & kFilterHideMissing) != 0;
    }
}

kTraversal TBrowserFill::EnterDirectory(const char* fullpath, const char* dirname, const FSSpec* macspec)
{
	ASSERT(m_item != 0L);
	
	Entries_Open (m_entries, fullpath);
	BuildIgnoredList(m_ignlist, fullpath);
	
	return kContinueTraversal;
}

kTraversal TBrowserFill::ExitDirectory(const char* fullpath)
{
	m_ignlist.erase(m_ignlist.begin(), m_ignlist.end());
	SetIcon(m_treeCtrl, m_item, m_hascvs, (EntnodeData*)m_treeCtrl.GetItemData(m_item));
	
	return kContinueTraversal;
}

kTraversal TBrowserFill::OnError(const char* err, int errcode)
{
	return kTraversalError;
}

kTraversal TBrowserFill::OnIdle(const char* fullpath)
{
	return kContinueTraversal;
}

kTraversal TBrowserFill::OnDirectory(const char* fullpath, const char* fullname, const char* name,
							   const struct stat& dir, const FSSpec* macspec)
{
	if( stricmp(name, "cvs") == 0 )
	{
		m_hascvs = true;
		return kSkipFile;
	}
	
	EntnodeData* data = Entries_SetVisited(fullpath, m_entries, name, dir, true, &m_ignlist);

	// Apply filters
	if( data->IsIgnored() )
	{
		// Ignore ignored
		if( !m_isShowIgnored )
		{
			return kSkipFile;
		}
	}
	else if( data->IsUnknown() )
	{
		// Hide unknown
		if( m_isHideUnknown )
		{
			return kSkipFile;
		}
	}

	// Create the item for the sub-directory
	TBrowserFillDummy traverse(m_treeCtrl, m_item, 0L, data);
	kTraversal res = FileTraverse(fullname, traverse);
	
	// Assign the icon regarding our Entries info
	if( traverse.GetItem() != 0L )
	{
		SetIcon(m_treeCtrl, traverse.GetItem(), m_hascvs, data);
	}
	
	return kSkipFile;
}

/// Get the hide missing filter
bool TBrowserFill::IsHideMissing() const
{
	return m_isHideMissing;
}

/////////////////////////////////////////////////////////////////////////////
// CWinCvsBrowser

CImageList CWinCvsBrowser::m_imageList;

IMPLEMENT_DYNCREATE(CWinCvsBrowser, CTreeView)

BEGIN_MESSAGE_MAP(CWinCvsBrowser, CTreeView)
	//{{AFX_MSG_MAP(CWinCvsBrowser)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ADD, OnUpdateViewAdd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMMIT, OnUpdateViewUpdate)
	ON_COMMAND(ID_VIEW_ADD, OnViewAdd)
	ON_COMMAND(ID_VIEW_COMMIT, OnViewCommit)
	ON_COMMAND(ID_VIEW_UPDATE, OnViewUpdate)
	ON_COMMAND(ID_VIEW_QUERYUPDATE, OnViewQueryUpdate)
	ON_COMMAND(ID_VIEW_DIFF, OnViewDiff)
	ON_COMMAND(ID_VIEW_LOG, OnViewLog)
	ON_COMMAND(ID_VIEW_STATUS, OnViewStatus)
	ON_COMMAND(ID_VIEW_UNLOCKF, OnViewUnlock)
	ON_COMMAND(ID_VIEW_WATCHON, OnViewWatchOn)
	ON_COMMAND(ID_VIEW_WATCHOFF, OnViewWatchOff)
	ON_COMMAND(ID_VIEW_EDIT, OnViewEdit)
	ON_COMMAND(ID_VIEW_UNEDIT, OnViewUnedit)
	ON_COMMAND(ID_VIEW_WATCHERS, OnViewWatchers)
	ON_COMMAND(ID_VIEW_EDITORS, OnViewEditors)
	ON_COMMAND(ID_VIEW_RELEASE, OnViewRelease)
	ON_COMMAND(ID_VIEW_TAGNEW, OnViewTagNew)
	ON_COMMAND(ID_VIEW_TAGDELETE, OnViewTagDelete)
	ON_COMMAND(ID_VIEW_TAGBRANCH, OnViewTagBranch)
	ON_COMMAND(ID_VIEW_EXPLORE, OnViewExplore)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPLORE, OnUpdateViewExplore)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RELOAD, OnUpdateViewReload)
	ON_COMMAND(ID_VIEW_RELOAD, OnViewReload)
	ON_COMMAND(ID_VIEW_CHECKOUT, OnViewCheckout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHECKOUT, OnUpdateViewCheckout)
	ON_COMMAND(ID_VIEW_IMPORT, OnViewImport)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IMPORT, OnUpdateViewImport)
	ON_WM_SETCURSOR()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_RESERVEDEDIT, OnViewReservededit)
	ON_COMMAND(ID_VIEW_FORCEEDIT, OnViewForceedit)
	ON_COMMAND(ID_APP_CMDLINE, OnAppCmdline)
	ON_COMMAND(ID_APP_RTAGBRANCH, OnAppRtagbranch)
	ON_COMMAND(ID_APP_RTAGCREATE, OnAppRtagcreate)
	ON_COMMAND(ID_APP_RTAGDELETE, OnAppRtagdelete)
	ON_UPDATE_COMMAND_UI(ID_APP_CMDLINE, OnUpdateAppCmdline)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_QUERY_PROPERTIES, OnQueryProperties)
	ON_UPDATE_COMMAND_UI(ID_QUERY_PROPERTIES, OnUpdateQueryProperties)
	ON_COMMAND(ID_VIEW_NEWINSTANCE, OnViewNewInstance)
	ON_COMMAND(ID_VIEW_RMV, OnViewRmv)
	ON_COMMAND(ID_VIEW_EXPORT, OnViewExport)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPORT, OnUpdateViewExport)
	ON_COMMAND(ID_FORMAT_FONT, OnFormatFont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEWINSTANCE, OnUpdateViewExplore)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DIFF, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDIT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESERVEDEDIT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FORCEEDIT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITORS, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOG, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RELEASE, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNEDIT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_QUERYUPDATE, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNLOCKF, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCHERS, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCHON, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCHOFF, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGNEW, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGDELETE, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGBRANCH, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_APP_RTAGBRANCH, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_APP_RTAGCREATE, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_APP_RTAGDELETE, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RMV, OnUpdateViewUpdate)
	ON_COMMAND(ID_QUERY_COMMANDPROMPT, OnQueryCommandprompt)
	ON_UPDATE_COMMAND_UI(ID_QUERY_COMMANDPROMPT, OnUpdateViewExplore)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(ID_MACRO_START, ID_MACRO_END, OnUpdateMacro)
	ON_COMMAND_EX_RANGE(ID_MACRO_START, ID_MACRO_END, OnMacro)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CUST_FILES_MENU, ID_CUST_FOLDER_MENU, OnUpdateCustomize)
	ON_COMMAND_EX_RANGE(ID_CUST_FILES_MENU, ID_CUST_FOLDER_MENU, OnCustomize)
	ON_MESSAGE(WM_RESETVIEW, OnResetView)
END_MESSAGE_MAP()

CWinCvsBrowser::CWinCvsBrowser()
	: m_selectionNotificationLocked(false), m_selectionChanged(false), m_resetViewPending(false)
{
	LOGFONT lf = gModulesFont;
	if( *lf.lfFaceName )
	{
		m_font = new CFont();
		m_font->CreateFontIndirect(&lf);
	}
	else
	{
		m_font = NULL;
	}
}

CWinCvsBrowser::~CWinCvsBrowser()
{
	if( m_font )
	{
		m_font->DeleteObject();
		delete m_font;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWinCvsBrowser drawing

/// OnDraw virtual override, no drawing code needed
void CWinCvsBrowser::OnDraw(CDC* pDC)
{
	// No draw code
}

/////////////////////////////////////////////////////////////////////////////
// CWinCvsBrowser diagnostics

#ifdef _DEBUG
void CWinCvsBrowser::AssertValid() const
{
	CTreeView::AssertValid();
}

void CWinCvsBrowser::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinCvsBrowser message handlers

/// Create virtual override, initialize browser tree control
BOOL CWinCvsBrowser::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if( !CTreeView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext) )
	{
		return FALSE;
	}
	
	if( m_imageList.m_hImageList == 0L )
	{
		const auto* const p=GetImageResources();

		assert(p);

		m_imageList.Create(p->Modules.ResID, p->Modules.Size, 1, RGB(255, 255, 255));
	}

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	treeCtrl.SetImageList(&m_imageList, TVSIL_NORMAL);

	SetNewStyle(TVS_HASLINES | TVS_HASBUTTONS /*| TVS_LINESATROOT*/, TRUE);

	return TRUE;
}

/*!
	Delete all items and handle the associated item data
	\param root If root != 0L then delete all childs of root (except root), if root == 0L then delete all
*/
void CWinCvsBrowser::DeleteAllItems(HTREEITEM root /*= 0L*/)
{
	// We don't want the file view to be notified when things get deleted
	// process selection after we are done with all that jazz below
	CSelectionChangeNotifier notifier(this, true); 

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	bool eraseRoot = false;
	if( root == 0L )
	{
		root = treeCtrl.GetRootItem( );
		eraseRoot = true;
	}

	if( root == 0L )
		return;

	CWndRedrawMngr redrawMngr(this);

	HTREEITEM childItem = treeCtrl.GetNextItem(root, TVGN_CHILD);

	// Erase all the childs
	while( childItem != 0L )
	{
		EntnodeData* data = (EntnodeData*)treeCtrl.GetItemData(childItem);
		if( data != 0L )
		{
			treeCtrl.SetItemData(childItem,0L);	//So that subsequent references after UnRef() won't SEGV
			data->UnRef();	//Or move this one after DeleteAllItems? so we don't loose one system call. For now, safe way
		}

		DeleteAllItems(childItem);

		HTREEITEM nextItem = treeCtrl.GetNextItem(childItem, TVGN_NEXT);
		VERIFY(treeCtrl.DeleteItem(childItem));
		childItem = nextItem;
	}

	// Erase the root only if initially root was 0L
	if( eraseRoot )
	{
		EntnodeData* data = (EntnodeData*)treeCtrl.GetItemData(root);
		if( data )
		{
			treeCtrl.SetItemData(root, 0L);	//So that subsequent references after UnRef() won't SEGV
			data->UnRef();
		}

		VERIFY(treeCtrl.DeleteItem(root));
	}
}

/*!
	Set the browser root path
	\param path Browser root path
	\param notifyView true to notify the file view, false otherwise
	\return true on success, false otherwise
*/
void CWinCvsBrowser::ResetBrowser(const char* path, bool notifyView)
{
	CWaitCursor doWait;
	CWndRedrawMngr redrawMngr(this);

	// Must make a copy of 'path' before calling DeleteAllItems(), since
	// this function may have been called with 'm_root' passed as the first 
	// parameter (in fact it is in several places in the code) whose value
	// gets deleted by the call to DeleteAllItems() below, making 'path'
	// point to an invalid memory location
	const string pathCopy(path ? path : "");
	
	DeleteAllItems();
	
	const bool isReload = !pathCopy.empty() && !m_root.empty() && stricmp(m_root.c_str(), pathCopy.c_str()) == 0;

	const string prevRoot(m_root);
	m_root = pathCopy;

	if( pathCopy.empty() )
		return;

	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	CMainFrame* mainFrm = app->GetMainFrame();

	if( !isReload )
	{
		// Save the previous root settings first
		if( !prevRoot.empty() && HasPersistentSettings(prevRoot.c_str()) )
		{
			mainFrm->StoreLocationSettings();
			SavePersistentSettings(prevRoot.c_str());
		}

		if( HasPersistentSettings(m_root.c_str()) )
		{
			LoadPersistentSettings(m_root.c_str());
			mainFrm->LoadLocationSettings();
		}
		else
		{
			// Check if the path has a CVS folder. If not,
			// we don't want the user to be prompted (like when 
			// WinCvs is starting for the first time).
			string cvsFolder(m_root);
			NormalizeFolderPath(cvsFolder);
			cvsFolder += "CVS";

			struct stat sb;
			if( stat(cvsFolder.c_str(), &sb) != -1 && S_ISDIR(sb.st_mode) )
				AskCreatePersistentSettings(m_root.c_str());
		}
	}

	if( notifyView )
	{
		CHistComboBoxEx& hist = mainFrm->GetBrowseHistory();
		hist.NewRoot(m_root.c_str());
	}

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	TBrowserFillDummy traverse(treeCtrl, 0L, 0L, 0L);
	kTraversal res = FileTraverse(m_root.c_str(), traverse);

	CBrowseFileView* fileView = app->GetFileView();
	if( fileView != 0L )
	{
		HTREEITEM root = treeCtrl.GetRootItem();
		if( root != 0L )
		{
			treeCtrl.Expand(root, TVE_EXPAND);
			if( notifyView )
			{
				VERIFY(treeCtrl.Select(root, TVGN_CARET));
			}
		}
		else
		{
			fileView->ResetView(pathCopy.c_str());
		}
	}
}

/*!
	Set the style for the tree control
	\param lStyleMask Style mask
	\param bSetBits true to set the new style, false to remove style
*/
void CWinCvsBrowser::SetNewStyle(long lStyleMask, BOOL bSetBits)
{
	long lStyleOld;

	lStyleOld = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyleOld &= ~lStyleMask;
	if( bSetBits )
		lStyleOld |= lStyleMask;

	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

/*!
	Given an item of the tree, guess the full path.
	\param item Item to retrieve the path from
	\param path Return the path for the item
*/
void CWinCvsBrowser::RetrievePath(HTREEITEM item, std::string& path)
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();
	
	string tmp, newPath;
	path = "";
	TV_ITEM pItem;
	char name[512];

	do
	{
		pItem.hItem = item;
		pItem.pszText = name;
		pItem.cchTextMax = 512;
		pItem.mask = TVIF_HANDLE | TVIF_TEXT;
		
		VERIFY(treeCtrl.GetItem(&pItem));
		item = treeCtrl.GetParentItem(item);

		tmp = path;
		newPath = item == 0L ? m_root.c_str() : pItem.pszText;
		NormalizeFolderPath(newPath);
		newPath += path;

		path = newPath;
	} while( item != 0L );
}

/// TVN_ITEMEXPANDING notification handler, expand the selected node
void CWinCvsBrowser::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CWndRedrawMngr redrawMngr(this);

	// Prevent from changing directory if cvs is running
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() )
	{
		*pResult = TRUE;
		return;
	}

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM item = pNMTreeView->itemNew.hItem;

	EntnodeData* data = (EntnodeData*)treeCtrl.GetItemData(item);
	if( data != 0L && data->IsMissing() )
	{
		*pResult = TRUE;
		return;
	}

	if( pNMTreeView->action & TVE_EXPAND )
	{
		// Erase all the childs
		DeleteAllItems(item);
		
		string path;
		RetrievePath(item, path);
		
		CSortList<ENTNODE> entries(200, ENTNODE::Compare);
		TBrowserFill traverse(treeCtrl, 0L, item, entries);
		kTraversal res = FileTraverse(path.c_str(), traverse);

		if( !traverse.IsHideMissing() )
		{
			// Add the missing folders
			Entries_SetMissing(entries);
			
			int numEntries = entries.NumOfElements();
			for(int i = 0; i < numEntries; i++)
			{
				const ENTNODE & theNode = entries.Get(i);
				EntnodeData* data = theNode.Data();
				if( !data->IsMissing() || data->GetType() != ENT_SUBDIR )
					continue;
				
				TV_INSERTSTRUCT tvstruct;
				tvstruct.hParent = item;
				tvstruct.hInsertAfter = TVI_SORT;
				tvstruct.item.iImage = kIconFolderLostClosed;
				tvstruct.item.iSelectedImage = kIconFolderLostOpened;
				tvstruct.item.pszText = (char*)((*data)[EntnodeData::kName]).c_str();
				tvstruct.item.lParam = (long)data->Ref();
				tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				treeCtrl.InsertItem(&tvstruct);
			}
		}
	}

	*pResult = 0;
}

/*!
	Update UI for CVS command
	\param pCmdUI CCmdUI UI handler
	\param needCvsInfos true if command needs the CVS controlled selection, false otherwise
*/
void CWinCvsBrowser::OnUpdateCmd(CCmdUI* pCmdUI, BOOL needCvsInfos)
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM selItem = treeCtrl.GetSelectedItem();
	if( selItem == 0L )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	string path;
	RetrievePath(selItem, path);
	NormalizeFolderPath(path);
	path += "CVS";

	struct stat sb;
	if( stat(path.c_str(), &sb) == -1 || !S_ISDIR(sb.st_mode) )
		pCmdUI->Enable(!needCvsInfos);
	else
		pCmdUI->Enable(needCvsInfos);
}

/// WM_RBUTTONDOWN message handler, select the clicked item
/// \note Prevent message from propagating to allow WM_CONTEXTMENU to work
void CWinCvsBrowser::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// Prevent from changing directory if cvs is running
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() )
		return;

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	
	UINT pFlags;
	HTREEITEM testItem = treeCtrl.HitTest(point, &pFlags);

	if( testItem != 0L && (pFlags & (TVHT_ONITEM | TVHT_ONITEMBUTTON | TVHT_ONITEMRIGHT)) != 0 )
	{
		treeCtrl.SelectItem(testItem);
	}

	//CTreeView::OnRButtonDown(nFlags, point);
}

/// WM_CONTEXTMENU message handler, display the custom menu
void CWinCvsBrowser::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// Prevent from changing directory if cvs is running
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() )
	{
		return;
	}

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	const HTREEITEM item = treeCtrl.GetSelectedItem();
	if( item )
	{
		// Set the track point in case it's not the mouse click
		if( -1 == point.x && -1 == point.y )
		{
			// Get the icon's rectangle
			CRect rectItem;
			if( treeCtrl.GetItemRect(item, &rectItem, TRUE) )
			{
				point = rectItem.CenterPoint();
				ClientToScreen(&point);
			}
		}

		treeCtrl.SelectItem(item);
		
		// Locate the submenu after item selection
		std::auto_ptr<CMenu> pCustomMenu(::GetCustomMenu(kCustomBrowserMenu, this));
		if( pCustomMenu.get() )
		{
			pCustomMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
		}
	}
}

/// TVN_SELCHANGING notification handler, prevent the selection of the missing item
void CWinCvsBrowser::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Prevent from changing directory if cvs is running
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() )
	{
		*pResult = TRUE;
		return;
	}

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM item = pNMTreeView->itemNew.hItem;
	
	EntnodeData* data = (EntnodeData*)treeCtrl.GetItemData(item);
	if( data != 0L && data->IsMissing() )
	{
		*pResult = TRUE;
		return;
	}

	*pResult = 0;
}

/// TVN_SELCHANGED notification handler, notify the views
void CWinCvsBrowser::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	m_selectionChanged = true;
	NotifySelectionChange();

	*pResult = 0;
}

/*!
	Issues notification on selected item change
*/
void CWinCvsBrowser::NotifySelectionChange()
{
	if( !m_selectionNotificationLocked )
	{
		if( m_selectionChanged )
		{
			string path;
			HTREEITEM item = GetTreeCtrl().GetSelectedItem();
			if( item )
			{
				RetrievePath(item, path);
				CWincvsApp* app = (CWincvsApp*)AfxGetApp();
				CBrowseFileView* fileView = app->GetFileView();
				if( fileView )
				{
					fileView->ResetView(path.c_str());
				}

				gCvsPrefs.SetLastWorkingDir(path.c_str());
			}
		}
	}
}

/*!
	Process the selection handler command
	\param handler Command selection handler
	\return true if the command was processed, false otherwise
*/
bool CWinCvsBrowser::ProcessSelectionCommand(KiSelectionHandler* handler)
{
	bool res = false;

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM selItem = treeCtrl.GetSelectedItem();
	
	if( selItem )
	{
		string path;

		RetrievePath(selItem, path);
		handler->OnFolder(path.c_str());

		res = true;
	}

	return res;
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateViewAdd(CCmdUI* pCmdUI) 
{
	OnUpdateCmd(pCmdUI, FALSE);
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateAppCmdline(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!DisableCommon());
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
	OnUpdateCmd(pCmdUI, TRUE);
}

/// WM_COMMAND message handler, <b>cvs add</b> selection
void CWinCvsBrowser::OnViewAdd() 
{
	KoAddHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs commit</b> selection
void CWinCvsBrowser::OnViewCommit() 
{
	KoCommitHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs update</b> selection
void CWinCvsBrowser::OnViewUpdate() 
{
	KoUpdateHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs -n update</b> selection
void CWinCvsBrowser::OnViewQueryUpdate() 
{
	KoQueryUpdateHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>[cvs] diff</b> selection
void CWinCvsBrowser::OnViewDiff() 
{
	KoDiffHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs log</b> selection
void CWinCvsBrowser::OnViewLog() 
{
	KoLogHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs status</b> selection
void CWinCvsBrowser::OnViewStatus() 
{
	KoStatusHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs admin -u</b> selection
void CWinCvsBrowser::OnViewUnlock() 
{
	KoUnlockHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs watch add</b> selection
void CWinCvsBrowser::OnViewWatchOn() 
{
	KoWatchOnHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs watch remove</b> selection
void CWinCvsBrowser::OnViewWatchOff() 
{
	KoWatchOffHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs edit</b> selection
void CWinCvsBrowser::OnViewEdit() 
{
	KoEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs edit -c</b> selection
void CWinCvsBrowser::OnViewReservededit() 
{
	KoReservedEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs edit -f</b> selection
void CWinCvsBrowser::OnViewForceedit() 
{
	KoForceEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs unedit</b> selection
void CWinCvsBrowser::OnViewUnedit() 
{
	KoUneditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs watchers</b> selection
void CWinCvsBrowser::OnViewWatchers() 
{
	KoWatchersHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs editors</b> selection
void CWinCvsBrowser::OnViewEditors() 
{
	KoEditorsHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs release</b> selection
void CWinCvsBrowser::OnViewRelease() 
{
	KoReleaseHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs tag</b> selection
void CWinCvsBrowser::OnViewTagNew() 
{
	KoCreateTagHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs tag -d</b> selection
void CWinCvsBrowser::OnViewTagDelete() 
{
	KoDeleteTagHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs tag -b</b> selection
void CWinCvsBrowser::OnViewTagBranch() 
{
	KoBranchTagHandler handler;
	ProcessSelectionCommand(&handler);
}

/// OnInitialUpdate virtual override, expand the root item and register ignore notification function
void CWinCvsBrowser::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	
	CWndRedrawMngr redrawMngr(this);

	if( m_font )
	{
		SetFont(m_font);
	}
	
	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM root = treeCtrl.GetRootItem();
	if( root != 0L )
	{
		treeCtrl.Expand(root, TVE_EXPAND);
		VERIFY(treeCtrl.Select(root, TVGN_CARET));
	}

	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		app->GetFilterModel()->GetNotificationManager()->CheckIn(this, ::OnFilteringChanged);
		app->GetIgnoreModel()->GetNotificationManager()->CheckIn(this, ::OnIgnoreChanged);
	}
}

/// WM_COMMAND message handler, open the explorer
void CWinCvsBrowser::OnViewExplore() 
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM selItem = treeCtrl.GetSelectedItem();
	
	string path;
	RetrievePath(selItem, path);
	
	LaunchExplore(path.c_str());
}

void CWinCvsBrowser::OnViewNewInstance() 
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM selItem = treeCtrl.GetSelectedItem();
	
	string path;
	RetrievePath(selItem, path);
	
	LaunchNewInst(path.c_str());
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateViewExplore(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM selItem = treeCtrl.GetSelectedItem();
	if( selItem == 0L )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
}

/// WM_COMMAND message handler, show the selection properties
void CWinCvsBrowser::OnQueryProperties() 
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM selItem = treeCtrl.GetSelectedItem();
	
	string path;
	RetrievePath(selItem, path);
	
	LaunchProperties(path.c_str());
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateQueryProperties(CCmdUI* pCmdUI) 
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM selItem = treeCtrl.GetSelectedItem();

	pCmdUI->Enable(selItem != NULL);
}

/*!
	Expand and select the path and optionally synchronize the file view
	\param path Path to select
	\param notifyView true to notify the file view, false otherwise
	\return true on success, false otherwise
*/
bool CWinCvsBrowser::StepToLocation(const char* path, bool notifyView /*= false*/)
{
	bool bRes = false;

	string root(m_root);
	NormalizeFolderPath(root);

	string subpath(path);
	NormalizeFolderPath(subpath);

	// Check if it is a sub-path
	if( strnicmp(root.c_str(), subpath.c_str(), root.length()) != 0 )
		return false;

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM item = treeCtrl.GetRootItem();
	HTREEITEM lastitem = 0L;
	if( item == 0L )
		return false;

	CWndRedrawMngr redrawMngr(this);

	if( stricmp(root.c_str(), subpath.c_str()) == 0 )
	{
		// Must select root node
		lastitem = item;
		treeCtrl.Expand(item, TVE_EXPAND);
		treeCtrl.EnsureVisible(item);
	}
	else
	{
		// Step inside synchronized
		const char* tmp = subpath.c_str() + root.length() - 1;
		while( (tmp = strchr(tmp, kPathDelimiter)) != 0L )
		{
			const char* name = ++tmp;
			if( name[0] == '\0' )
				break;
			
			string subname;
			const char* tmp2 = strchr(name, kPathDelimiter);
			if( tmp2 == 0L )
				subname = name;
			else
				subname.assign(name, tmp2 - name);
			
			HTREEITEM childItem = treeCtrl.GetNextItem(item, TVGN_CHILD);
			
			// Find the subitem which matches this name
			while( childItem != 0L )
			{
				if( EntnodeData* data = (EntnodeData*)treeCtrl.GetItemData(childItem) )
				{
					if( _stricmp(((*data)[EntnodeData::kName]).c_str(), subname.c_str()) == 0 )
					{
						// Found it !
						lastitem = item = childItem;
						treeCtrl.Expand(item, TVE_EXPAND);
						treeCtrl.EnsureVisible(item);
						break;
					}
				}
				
				childItem = treeCtrl.GetNextItem(childItem, TVGN_NEXT);
			}
		}
	}

	// In case this is called by the view, turn off notifying when the item gets selected
	if( lastitem != 0L )
	{
		CSelectionChangeNotifier notifier(this, notifyView); 
		VERIFY( bRes = (treeCtrl.Select(lastitem, TVGN_CARET) != 0) );
	}

	return bRes;
}

/*!
	Find the tree item corresponding to the given path
	\param path Path to find the item for
	\return Handle to the item for a given location, NULL if not found
*/
HTREEITEM CWinCvsBrowser::GetItemByLocation(const char* path)
{
	string root(m_root);
	NormalizeFolderPath(root);

	string subpath(path);
	NormalizeFolderPath(subpath);

	// Check if it is a sub-path
	if( strncmp(root.c_str(), subpath.c_str(), root.length()) != 0 )
		return 0L;

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM item = treeCtrl.GetRootItem();
	if( item == 0L )
		return 0L;

	// Step inside synchronized
	const char* pos = subpath.c_str() + root.length() - 1;
	while( (pos = strchr(pos, kPathDelimiter)) != 0L )
	{
		const char* name = ++pos;
		if( name[0] == '\0' )
			break;

		const char* delimPos = strchr(name, kPathDelimiter);
		string subname;
		if( delimPos == NULL )
			subname = name;
		else
			subname.assign(name, delimPos - name);

		HTREEITEM childItem = treeCtrl.GetNextItem(item, TVGN_CHILD);

		// Find the subitem which matches this name
		while( childItem != 0L )
		{
			EntnodeData* data = (EntnodeData*)treeCtrl.GetItemData(childItem);
			if( data != 0L )
			{
				if( _stricmp(((*data)[EntnodeData::kName]).c_str(), subname.c_str()) == 0 )
				{
					item = childItem;
					break;
				}
			}

			childItem = treeCtrl.GetNextItem(childItem, TVGN_NEXT);
		}

		if( childItem == 0L )
			return 0L;
	}

	return item;
}

/*!
	Store expanded items
	\param allExpanded Return the collection of expanded items
	\param root Start item handle, NULL to start from root item
	\note Recursive, only stores items where parents are also expanded
*/
void CWinCvsBrowser::StoreExpanded(std::vector<std::string>& allExpanded, HTREEITEM root /*= 0L*/)
{
	HTREEITEM item = root;
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if( item == NULL )
		item = treeCtrl.GetRootItem();

	if( item == NULL )
		return;

	TV_ITEM pItem = { 0 };
	pItem.hItem = item;
	pItem.mask = TVIF_STATE;
	pItem.stateMask = TVIS_EXPANDED;
	VERIFY(treeCtrl.GetItem(&pItem));

	if( pItem.state & TVIS_EXPANDED )
	{
		string path;
		RetrievePath(item, path);
		allExpanded.push_back(path);
	
		HTREEITEM childItem = treeCtrl.GetNextItem(item, TVGN_CHILD);
		while( childItem != NULL )
		{
			StoreExpanded(allExpanded, childItem);
			childItem = treeCtrl.GetNextItem(childItem, TVGN_NEXT);
		}
	}
}

/// Set the initial root
void CWinCvsBrowser::SetInitialRoot()
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	CMainFrame* mainFrm = app->GetMainFrame();
	CHistComboBoxEx& hist = mainFrm->GetBrowseHistory();
	string newpath(hist.GetOldLoc());
	
	ResetBrowser(newpath.c_str(), true);
}

/*!
	Force reload or check modifications before redrawing and optionally synchronize the file view
	\param forceReload true to force the refresh, false otherwise
	\param notifyView true to notify the file view, false otherwise
*/
void CWinCvsBrowser::ResetView(bool forceReload /*= false*/, bool notifyView /*= false*/)
{
	// No outstanding request to reset the view any more
	m_resetViewPending = false;

	CWaitCursor doWait;
	CWndRedrawMngr redrawMngr(this);

	bool contentChanged = false;
	
	vector<string> expandedItems;
	StoreExpanded(expandedItems);
	
	CTreeCtrl& treeCtrl = GetTreeCtrl();
	
	HTREEITEM selItem = treeCtrl.GetSelectedItem();
	int nSelItemOffset = -1;
	
	string selPath;
	if( selItem != NULL ) 
	{
		RetrievePath(selItem, selPath);
		
		CRect r;
		if( treeCtrl.GetItemRect(selItem, &r, FALSE/*bTextOnly*/) )
			nSelItemOffset = r.top / r.Height();
		
		if( nSelItemOffset < 0 || nSelItemOffset >= (int)treeCtrl.GetVisibleCount() )
			nSelItemOffset = -1;
	}

	const int nCx = treeCtrl.GetScrollPos(SB_HORZ);

	if( forceReload )
	{
		ResetBrowser(m_root.c_str());
		contentChanged = true;
	}

	if( contentChanged )
	{
		// Restore the expanded paths and the selected item
		vector<string>::const_iterator i;
		for(i = expandedItems.begin(); i != expandedItems.end(); ++i)
		{
			StepToLocation(i->c_str());
		}

		if( !selPath.empty() )
		{
			selItem = GetItemByLocation(selPath.c_str());
			if( selItem != NULL )
			{
				treeCtrl.EnsureVisible(selItem);
				VERIFY(treeCtrl.Select(selItem, TVGN_CARET));
			}
		}
		
		// If the previous offset in relation to the first visible item
		// is known, select the first visible item in a way that the
		// selected item remains at the same position
		if( nSelItemOffset >= 0 ) 
		{
			HTREEITEM firstVisibleItem = selItem;
			
			for( ; nSelItemOffset > 0; nSelItemOffset--)
			{
				firstVisibleItem = treeCtrl.GetPrevVisibleItem(firstVisibleItem);
			}
			
			treeCtrl.SelectSetFirstVisible(firstVisibleItem);
		}
	}

	treeCtrl.SetScrollPos(SB_HORZ, nCx, FALSE);

	// We don't need to notify the file view if we force the reload because that's already done
	if( notifyView )
	{
		CWincvsApp* app = (CWincvsApp*)AfxGetApp();
		app->GetFileView()->ResetView();
	}
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateViewReload(CCmdUI* pCmdUI) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	pCmdUI->Enable(!app->IsCvsRunning());
}

/// WM_COMMAND message handler, refresh view
void CWinCvsBrowser::OnViewReload() 
{
	ResetView(true, true);
}

/// WM_COMMAND message handler, invoke macro
BOOL CWinCvsBrowser::OnMacro(UINT nID)
{
	PyDoPython(WINCMD_to_UCMD(nID));

	return 1;
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateMacro(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	// Fill the cache with current selection
	if( !PyIsUICacheValid() )
	{
		CTreeCtrl& treeCtrl = GetTreeCtrl();
		HTREEITEM selItem = treeCtrl.GetSelectedItem();
		if( selItem != 0L )
		{
			bool deleteData = false;
			EntnodeData* data = (EntnodeData*)treeCtrl.GetItemData(selItem);
			if( data == 0L )
			{
				string selPath;
				RetrievePath(selItem, selPath);
				
				string uppath, folder;
				SplitPath(selPath.c_str(), uppath, folder);
				
				deleteData = true;
				EntnodePath *thePath = new EntnodePath(uppath.c_str());
				data = new EntnodeFolder(folder.c_str(), thePath);
				thePath->UnRef();
			}

			PyAppendCache(data);

			if( deleteData )
				data->UnRef();
		}
	}

	UCmdUI ucmdui(WINCMD_to_UCMD(pCmdUI->m_nID));
	ucmdui.pCmdUI = pCmdUI;
	PyDoCmdUI(&ucmdui);
}

/// PreCreateWindow virtual override, modify the window style
BOOL CWinCvsBrowser::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_TABSTOP | TVS_SHOWSELALWAYS;

	return CTreeView::PreCreateWindow(cs);
}

/*!
	Check whether the commands should be disabled
	\return true if commands should be disabled, false otherwise
*/
bool CWinCvsBrowser::DisableCommon()
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	return app->IsCvsRunning();
}

/// WM_COMMAND message handler, <b>cvs checkout</b>
void CWinCvsBrowser::OnViewCheckout() 
{
	KoCheckoutHandler handler;
	ProcessSelectionCommand(&handler);
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateViewCheckout(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
	pCmdUI->SetText("Chec&kout module...");
}

/// WM_COMMAND message handler, <b>cvs import</b>
void CWinCvsBrowser::OnViewImport() 
{
	KoImportHandler handler;
	ProcessSelectionCommand(&handler);
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateViewImport(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
	pCmdUI->SetText("&Import module...");
}

/// WM_COMMAND message handler, display the customize dialog
BOOL CWinCvsBrowser::OnCustomize(UINT nID)
{
	CustomizeMenus(nID == ID_CUST_FILES_MENU ? kCustomFilesMenu : kCustomBrowserMenu);
	return TRUE;
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateCustomize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

/// WM_SETCURSOR message handler, set the hourglass cursor during CVS commands
BOOL CWinCvsBrowser::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() )
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		return TRUE;
	}
	
	return CTreeView::OnSetCursor(pWnd, nHitTest, message);
}

/// WM_DESTROY message handler, unregister ignore notification function
void CWinCvsBrowser::OnDestroy() 
{
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		app->GetIgnoreModel()->GetNotificationManager()->CheckOut(this);
		app->GetFilterModel()->GetNotificationManager()->CheckOut(this);
	}

	CTreeView::OnDestroy();
}

/// Ignore changed notification handler, post reset view message
void CWinCvsBrowser::OnIgnoreChanged()
{
	if( !m_resetViewPending )
	{
		m_resetViewPending = true;
		PostMessage(WM_RESETVIEW);
	}	
}

/*!
	Notify the view that app's filtering has changed
*/
void CWinCvsBrowser::OnFilteringChanged()
{
	if( !m_resetViewPending )
	{
		m_resetViewPending = true;
		PostMessage(WM_RESETVIEW);
	}    
}

/// WM_RESETVIEW message handler, reset view
LRESULT CWinCvsBrowser::OnResetView(WPARAM, LPARAM)
{
	ResetView(true, false);
	return 0;
}

/// WM_COMMAND message handler, display command line dialog
void CWinCvsBrowser::OnAppCmdline() 
{
	KoCommandLineHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs rtag -b</b>
void CWinCvsBrowser::OnAppRtagbranch() 
{
	KoRtagBranchHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs rtag</b>
void CWinCvsBrowser::OnAppRtagcreate() 
{
	KoRtagCreateHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs rtag -d</b>
void CWinCvsBrowser::OnAppRtagdelete() 
{
	KoRtagDeleteHandler handler;
	ProcessSelectionCommand(&handler);
}

/// OnActivateView virtual override, make accelerators work
void CWinCvsBrowser::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// Trick the frame into believing there is no active view
	// Need this for MDI messages (Ctrl+Tab, Ctrl+F4, ...) to be translated correctly
	if( bActivate )
	{
		if( CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd() )
		{
			pMainFrame->SetActiveView(NULL);
		}
	}

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

/// WM_COMMAND message handler, <b>[cvs] remove</b> selection
void CWinCvsBrowser::OnViewRmv() 
{
	KoFileRemoveHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs export</b>
void CWinCvsBrowser::OnViewExport() 
{
	KoExportHandler handler;
	ProcessSelectionCommand(&handler);
}

/// UPDATE_COMMAND_UI handler
void CWinCvsBrowser::OnUpdateViewExport(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	pCmdUI->Enable(TRUE);
}

/// WM_COMMAND message handler, select font
void CWinCvsBrowser::OnFormatFont() 
{
	LOGFONT lf = gModulesFont;
	if( IsNullOrEmpty(lf.lfFaceName) )
	{
		CFont* font = GetFont();
		if( font )
		{
			font->GetLogFont(&lf);
		}
	}	
	
	CFontDialog dlg(&lf);
	
	if( dlg.DoModal() != IDOK )
		return;
	
	gModulesFont = *dlg.m_cf.lpLogFont;
	
	if( m_font )
		m_font->DeleteObject();
	else
		m_font = new CFont();
	
	m_font->CreateFontIndirect(dlg.m_cf.lpLogFont);
	
	SetFont(m_font);
}

/// WM_COMMAND message handler, open the Command Prompt
void CWinCvsBrowser::OnQueryCommandprompt() 
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM selItem = treeCtrl.GetSelectedItem();
	
	string path;
	RetrievePath(selItem, path);
	
	LaunchCommandPrompt(path.c_str());
}
