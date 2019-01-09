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
 * BrowseFileView.cpp -- Implements the view with all the files
 * 
 */

#include "stdafx.h"
#include "wincvs.h"

#include <process.h>
#include <vector>
#include <IO.h>

#include "BrowseFileView.h"
#include "CvsCommands.h"
#include "MultiFiles.h"
#include "CvsArgs.h"
#include "AppConsole.h"
#include "CvsPrefs.h"
#include "CvsIgnore.h"
#include "TclGlue.h"
#include "MoveToTrash.h"
#include "WinCvsBrowser.h"
#include "MainFrm.h"
#include "BrowseViewModel.h"
#include "BrowseViewHandlers.h"
#include "FileViewWatcher.h"
#include "BrowseViewColumn.h"
#include "PythonGlue.h"
#include "FilterMaskOptDlg.h"
#include "LaunchHandlers.h"
#include "CustomizeMenus.h"
#include "umain.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/// Default value for file change detection timer
static const int FILECHANGE_TIMER = 999;

/// Extra space for printed text
static const int kSpc = 3;

/// Application message to reset the view
static const int WM_RESETVIEW = WM_APP + 12;

/// Empty font
static LOGFONT emptyLogFont = { 0 };

// Persistent values
CPersistentInt gFileViewFilterMasksReactionSpeed("P_FileViewFilterMasksReactionSpeed", 60 * 1000);
CPersistentT<LOGFONT> gFileViewFont("P_FileViewFont", emptyLogFont, kNoClass);

/*!
	Filtering changed notification function
	\param obj Browse file view to be notified
*/
static void OnFilteringChanged(CObject* obj)
{
	((CBrowseFileView*)obj)->OnFilteringChanged();
}

/*!
	Filter Bar changed notification function
	\param obj Browse file view to be notified
*/
static void OnFilterBarChanged(CObject* obj)
{
	((CBrowseFileView*)obj)->OnFilterBarChanged();
}

//////////////////////////////////////////////////////////////////////////
// CRecursionLock

CRecursionLock::CRecursionLock(bool& semaphore) 
	: m_semaphore(semaphore)
{
	m_semaphore = true;
}

CRecursionLock::~CRecursionLock()
{
	m_semaphore = false;
}

//////////////////////////////////////////////////////////////////////////
// KoFileViewTraversalContext

KoFileViewTraversalContext::KoFileViewTraversalContext(CBrowseFileView* view)
	: m_view(view), m_filter(NULL), m_isRecursive(false), m_isShowIgnored(false)
{
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{ 
		m_filter = app->GetFilterModel();
		m_isRecursive = app->GetRecursionModel()->IsShowRecursive();
		m_isShowIgnored = app->GetIgnoreModel()->IsShowIgnored();
	}
}

/// Get the recursive flag
bool KoFileViewTraversalContext::IsRecursive() const
{
	return m_isRecursive;
}

/// Get the show ignored flag
bool KoFileViewTraversalContext::IsShowIgnored() const
{
	return m_isShowIgnored;
}

/// Get the unknown flag
bool KoFileViewTraversalContext::IsShowUnknown() const
{
	return (m_filter->GetFilters() & kFilterUnknown) != 0;
}

/// Get the hide unknown flag
bool KoFileViewTraversalContext::IsHideUnknown() const
{
	return (m_filter->GetFilters() & kFilterHideUnknown) != 0;
}

/// Get the show missing flag
bool KoFileViewTraversalContext::IsShowMissing() const
{
	return (m_filter->GetFilters() & kFilterMissing) != 0;
}

/// Get the show missing flag
bool KoFileViewTraversalContext::IsShowModified() const
{
	return (m_filter->GetFilters() & kFilterModified) != 0;
}

/// Get the file view
CBrowseFileView* KoFileViewTraversalContext::GetView() const
{
	return m_view;
}

/*!
	Check the node's match
	\param data Node data to be checked
	\return true if node matches, false otherwise
*/
bool KoFileViewTraversalContext::IsMatch(EntnodeData* data) const
{
	return m_filter->IsMatch(data);
}

//////////////////////////////////////////////////////////////////////////
// TViewFill

TViewFill::TViewFill(const KoFileViewTraversalContext* context) 
	: m_context(context), m_entries(1000, ENTNODE::Compare)
{
}

kTraversal TViewFill::EnterDirectory(const char* fullpath, const char* dirname, const FSSpec* macspec)
{
	Entries_Open(m_entries, fullpath);
	BuildIgnoredList(m_ignlist, fullpath);
	return kContinueTraversal;
}

kTraversal TViewFill::ExitDirectory(const char* fullpath)
{
	// Add the missing files
	Entries_SetMissing(m_entries);
	
	const int numEntries = m_entries.NumOfElements();

	for(int i = 0; i < numEntries; i++)
	{
		const ENTNODE& theNode = m_entries.Get(i);
		EntnodeData* data = theNode.Data();

		if( data->IsMissing() && m_context->IsMatch(data) )
		{
			if( !m_context->IsRecursive() || data->GetType() == ENT_FILE )
			{
				const int iImage = data->GetType() == ENT_FILE ?
					(data->IsRemoved() ? kFileIconRemoved : kFileIconMiss) : kFolderIconMiss;
				
				InitItem(data, iImage);
			}
		}
	}
	
	m_ignlist.clear();

	return kContinueTraversal;
}

/*!
	Initialize and add new item to list view
	\param data Node to be added
	\param iImage Image for the node
	\param tag Tag
	\return Index of the new inserted item
*/
int TViewFill::InitItem(EntnodeData* data, int iImage, const char* tag /*= 0L*/)
{
	CListCtrl& listCtrl = m_context->GetView()->GetListCtrl();
	KoColumnContext context(m_context->GetView()->GetPath().c_str());
	
	TCHAR buf[256];
	m_context->GetView()->GetColumnModel()->GetAt(0)->GetText(&context, data, buf, 256);
	
	LV_ITEM lvi;			
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvi.iItem = listCtrl.GetItemCount();
	lvi.iSubItem = 0;
	lvi.pszText = buf;
	lvi.iImage = iImage;
	lvi.lParam = (LPARAM)(LPVOID)data;
	
	const int pos = listCtrl.InsertItem(&lvi);
	if( pos >= 0 )
	{
		// Note ownership addition
		data->Ref();
		
		// Set item text for additional columns			
		KiColumnModel* model = m_context->GetView()->GetColumnModel();
		int columnCount = model->GetCount();

		for(int j = 1; j < columnCount; j++)
		{
			model->GetAt(j)->GetText(&context, data, buf, 256);
			listCtrl.SetItemText(pos, j, buf);
		}
		
		if( tag != 0L && tag[0] != '\0' && !m_context->IsRecursive() )
		{
			listCtrl.SetItemText(pos, EntnodeFile::kTag, tag);
		}
	}

	return pos;
}

kTraversal TViewFill::OnError(const char* err, int errcode)
{
	return kTraversalError;
}

kTraversal TViewFill::OnIdle(const char* fullpath)
{
	// Allow to stop traversing
	if( IsEscKeyPressed() )
	{
		return kStopTraversal;
	}

	return kContinueTraversal;
}

kTraversal TViewFill::OnDirectory(const char* fullpath, const char* fullname, const char* name,
								  const struct stat& dir, const FSSpec* macspec)
{
	if( stricmp(name, "CVS") == 0 )
		return kSkipFile;
	
	EntnodeData* data = Entries_SetVisited(fullpath, m_entries, name, dir, true, &m_ignlist);
	
	if( m_context->IsRecursive() )
	{
		if( (!data->IsIgnored() || m_context->IsShowIgnored()) && !data->IsMissing() )
		{
			TViewFill traverse(m_context);
			FileTraverse(fullname, traverse);
		}

		return kSkipFile;			
	}
	
	if( data->IsIgnored() )
	{
		// Ignore ignored
		if( !m_context->IsShowIgnored() )
		{
			return kSkipFile;			
		}
	}
	else if( data->IsUnknown() )
	{
		// Hide unknown
		if( m_context->IsHideUnknown() )
		{
			return kSkipFile;			
		}
	}

	if( m_context->IsShowModified() )
	{
		// Hide folders unless some of the matching show filters are set
		if( !(data->IsIgnored() && m_context->IsShowIgnored() || data->IsUnknown() && m_context->IsShowUnknown()) )
		{
			return kSkipFile;			
		}
	}

	if( m_context->IsShowMissing() && !data->IsMissing() )
	{
		// Show only missing unless any matching show filters are set
		if( !(data->IsIgnored() && m_context->IsShowIgnored() || data->IsUnknown() && m_context->IsShowUnknown()) )
		{
			return kSkipFile;			
		}
	}

	if( !data->IsUnknown() && m_context->IsShowUnknown() )
	{
		// Don't show CVS files if show unknown is set
		return kSkipFile;			
	}

	// Get the tag
	string subCVS = fullname;
	NormalizeFolderPath(subCVS);
	subCVS += "CVS";

	string tagName;
	Tag_Open(tagName, subCVS.c_str());
	
	InitItem(data, CBrowseFileView::GetImageForEntry(data), tagName.c_str());

	return kSkipFile;
}

kTraversal TViewFill::OnAlias(const char* fullpath, const char* fullname, const char* name,
							  const struct stat& dir, const FSSpec* macspec)
{
	return OnFile(fullpath, fullname, name, dir, macspec);
}

kTraversal TViewFill::OnFile(const char* fullpath, const char* fullname, const char* name,
						  const struct stat& dir, const FSSpec* macspec)
{
	EntnodeData* data = Entries_SetVisited(fullpath, m_entries, name, dir, false, &m_ignlist);
	if( !m_context->IsShowIgnored() && data->IsIgnored() )
		return kContinueTraversal;
	
	if( m_context->IsMatch(data) )
	{
		int pos = InitItem(data, CBrowseFileView::GetImageForEntry(data));
		if( pos >=	0 )
		{
			UINT state = data->IsLocked() ? INDEXTOSTATEIMAGEMASK(2) : INDEXTOSTATEIMAGEMASK(1);
			CListCtrl& listCtrl = m_context->GetView()->GetListCtrl();
			listCtrl.SetItemState(pos, state, LVIS_STATEIMAGEMASK);
		}
	}

	return kContinueTraversal;
}


/////////////////////////////////////////////////////////////////////////////
// CBrowseFileView

CImageList CBrowseFileView::m_smallImageList;
CImageList CBrowseFileView::m_stateImageList;

IMPLEMENT_DYNCREATE(CBrowseFileView, CListView)

BEGIN_MESSAGE_MAP(CBrowseFileView, CListView)
	//{{AFX_MSG_MAP(CBrowseFileView)
	ON_COMMAND(ID_VIEW_SMALLICONS, OnViewSmallIcons)
	ON_COMMAND(ID_VIEW_FULLLIST, OnViewList)
	ON_COMMAND(ID_VIEW_ROWDETAILS, OnViewFullRowDetails)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SMALLICONS, OnUpdateViewSmallIcons)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ADD, OnUpdateViewAdd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ADDB, OnUpdateViewAddB)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMMIT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RELEASE, OnUpdateViewRelease)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RMV, OnUpdateViewRmv)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRAPH, OnUpdateViewGraph)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLLIST, OnUpdateViewList)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITSELDEF, OnUpdateViewEditseldef)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ROWDETAILS, OnUpdateViewFullRowDetails)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_COMMAND(ID_VIEW_ADD, OnViewAdd)
	ON_COMMAND(ID_VIEW_ADDB, OnViewAddb)
	ON_COMMAND(ID_VIEW_COMMIT, OnViewCommit)
	ON_COMMAND(ID_VIEW_RMV, OnViewRmv)
	ON_COMMAND(ID_VIEW_UPDATE, OnViewUpdate)
	ON_COMMAND(ID_VIEW_QUERYUPDATE, OnViewQueryUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RELOAD, OnUpdateViewReload)
	ON_COMMAND(ID_VIEW_RELOAD, OnViewReload)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_COMMAND(ID_VIEW_UPONE, OnViewUpone)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPONE, OnUpdateViewUpone)
	ON_COMMAND(ID_VIEW_TRASH, OnViewTrash)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRASH, OnUpdateViewTrash)
	ON_COMMAND(ID_VIEW_DIFF, OnViewDiff)
	ON_COMMAND(ID_VIEW_LOG, OnViewLog)
	ON_COMMAND(ID_VIEW_GRAPH, OnViewGraph)
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
	ON_COMMAND(ID_VIEW_NEWINSTANCE, OnViewNewInstance)
	ON_WM_TIMER()
	ON_COMMAND(ID_VIEW_EDITSEL, OnViewEditsel)
	ON_COMMAND(ID_VIEW_EDITSELDEF, OnViewEditseldef)
	ON_COMMAND(ID_VIEW_OPENSEL, OnViewOpensel)
	ON_COMMAND(ID_VIEW_OPENSELAS, OnViewOpenselas)
	ON_COMMAND(ID_VIEW_CHECKOUT, OnViewCheckout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHECKOUT, OnUpdateViewCheckout)
	ON_COMMAND(ID_VIEW_IMPORT, OnViewImport)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IMPORT, OnUpdateViewImport)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, OnBeginrdrag)
	ON_WM_SETCURSOR()	
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_QUERY_ANNOTATE, OnUpdateViewAnnotate)
	ON_COMMAND(ID_QUERY_ANNOTATE, OnAnnotate)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_VIEW_RESERVEDEDIT, OnViewReservededit)
	ON_COMMAND(ID_VIEW_FORCEEDIT, OnViewForceedit)
	ON_COMMAND(ID_VIEW_ADDU, OnViewAddu)
	ON_COMMAND(ID_APP_CMDLINE, OnAppCmdline)
	ON_UPDATE_COMMAND_UI(ID_APP_CMDLINE, OnUpdateCvsCmd)
	ON_COMMAND(ID_APP_RTAGBRANCH, OnAppRtagbranch)
	ON_COMMAND(ID_APP_RTAGCREATE, OnAppRtagcreate)
	ON_COMMAND(ID_APP_RTAGDELETE, OnAppRtagdelete)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_FILTERBAR_ENABLE, OnViewFilterbarEnable)
	ON_COMMAND(ID_VIEW_FILTERBAR_CLEARALL, OnViewFilterbarClearall)
	ON_COMMAND(ID_VIEW_FILTERBAR_OPTIONS, OnViewFilterbarOptions)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, OnDeleteallitems)
	ON_COMMAND(ID_QUERY_SHELLACTION_DEFAULT, OnQueryShellactionDefault)
	ON_COMMAND(ID_QUERY_PROPERTIES, OnQueryProperties)
	ON_UPDATE_COMMAND_UI(ID_QUERY_PROPERTIES, OnUpdateQueryProperties)
	ON_WM_SETTINGCHANGE()
	ON_UPDATE_COMMAND_UI(ID_QUERY_COMPARE, OnUpdateQueryCompare)
	ON_COMMAND(ID_QUERY_COMPARE, OnQueryCompare)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITSELCONFLICT, OnUpdateViewEditselconflict)
	ON_COMMAND(ID_VIEW_EDITSELCONFLICT, OnViewEditselconflict)
	ON_COMMAND(ID_VIEW_OPENSELANNOTATE, OnViewOpenselannotate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPENSELANNOTATE, OnUpdateViewOpenselannotate)
	ON_COMMAND(ID_VIEW_EXPORT, OnViewExport)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPORT, OnUpdateViewExport)
	ON_COMMAND(ID_FORMAT_FONT, OnFormatFont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEWINSTANCE, OnUpdateViewExplore)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ADDU, OnUpdateViewAddB)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DIFF, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDIT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESERVEDEDIT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FORCEEDIT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITORS, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOG, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGNEW, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGDELETE, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGBRANCH, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNEDIT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_QUERYUPDATE, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNLOCKF, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCHERS, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCHON, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCHOFF, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITSEL, OnUpdateViewTrash)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPENSEL, OnUpdateViewTrash)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPENSELAS, OnUpdateViewTrash)
	ON_UPDATE_COMMAND_UI(ID_APP_RTAGBRANCH, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_APP_RTAGCREATE, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_APP_RTAGDELETE, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_QUERY_SHELLACTION_DEFAULT, OnUpdateViewTrash)
	ON_COMMAND(ID_QUERY_COMMANDPROMPT, OnQueryCommandprompt)
	ON_UPDATE_COMMAND_UI(ID_QUERY_COMMANDPROMPT, OnUpdateViewExplore)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MACRO_START, ID_MACRO_END, OnUpdateMacro)
	ON_COMMAND_EX_RANGE(ID_MACRO_START, ID_MACRO_END, OnMacro)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CUST_FILES_MENU, ID_CUST_FOLDER_MENU, OnUpdateCustomize)
	ON_COMMAND_EX_RANGE(ID_CUST_FILES_MENU, ID_CUST_FOLDER_MENU, OnCustomize)
	ON_MESSAGE(KoWatcher::WM_FILECHANGE, OnFileChange)
	ON_MESSAGE(WM_RESETVIEW, OnResetView)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseFileView construction/destruction

CBrowseFileView::CBrowseFileView() 
	: m_checkChanges(false), m_watch(NULL), m_isResetViewLocked(false), m_isFilesChange(false), 
	m_isResetViewPending(false), m_uiFilechangeTimer(0)
{
	LOGFONT lf = gFileViewFont;
	if( *lf.lfFaceName )
	{
		m_font = new CFont();
		m_font->CreateFontIndirect(&lf);
	}
	else
	{
		m_font = NULL;
	}
	
	m_sortRegular = EntnodeData::kName;
	m_sortFlat = EntnodeData::kName;
	m_ascendantRegular = false;
	m_ascendantFlat = false;
	
	SetRecursive(false);
	
	m_entriesMod = 0;
	m_entriesLogMod = 0;	
}

CBrowseFileView::~CBrowseFileView()
{
	if( m_font )
	{
		m_font->DeleteObject();
		delete m_font;
	}
}

/// PreCreateWindow virtual override, modify the window style
BOOL CBrowseFileView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_TABSTOP | LVS_SHOWSELALWAYS | LVS_REPORT;
	cs.dwExStyle |= LVS_EX_FULLROWSELECT;

	return CListView::PreCreateWindow(cs);
}

/*!
	Get the ID of the image which reflects the state of the file/directory
	\param data Node data to get the image for
	\return The ID of the image
*/
int CBrowseFileView::GetImageForEntry(const EntnodeData* const data)
{
	int result = -1;

	if( data->GetType() == ENT_FILE )
	{
		if( data->IsIgnored() )
		{
			result = kFileIconIgnored;
		}
		else if( data->IsUnknown() )
		{
			result = kFileIconUnknown;
		}
		else if( !IsNullOrEmpty(data->GetConflict()) )
		{
			result = data->IsResolved() ? kFileIconResolved : kFileIconConflict;
		}
		else if( data->IsRemoved() )
		{
			result = kFileIconRemoved;
		}
		else 
		{	
			const string& encoding = (*data)[EntnodeFile::kEncoding];
			if( !encoding.empty() )
			{
				switch( encoding[0] )
				{
				default:
				case 'T':
					result = data->IsUnmodified() ? kFileIconText : 
						data->IsAdded() ? kFileIconAdded : kFileIconTextMod;
					break;
				case 'B':
					result = data->IsUnmodified() ? kFileIconBinary : 
						data->IsAdded() ? kFileIconBinaryAdded : kFileIconBinaryMod;
					break;
				case 'U':
					result = data->IsUnmodified() ? kFileIconUnicode : 
						data->IsAdded() ? kFileIconUnicodeAdded : kFileIconUnicodeMod;
					break;
				case 'E':
					result = data->IsUnmodified() ? kFileIconExtended : 
						data->IsAdded() ? kFileIconExtendedAdded : kFileIconExtendedMod;
					break;
				}
			}
			else
			{
				result = data->IsUnmodified() ? kFileIconText : 
					data->IsAdded() ? kFileIconAdded : kFileIconTextMod;
			}
		}
	}
	else
	{
		if( data->IsIgnored() )
		{
			result = kFolderIconIgnored;
		}
		else if( data->IsUnknown() )
		{
			result = kFolderIconUnknown;
		}
		else
		{
			result = kFolderIcon;
		}
	}

	return result;
}

/*!
	Set the recursive flag and model
	\param isRecursive New recursive state
*/
void CBrowseFileView::SetRecursive(bool isRecursive)
{
	m_isRecursive = isRecursive;
	m_modelColumns = m_isRecursive ? KiColumnModel::GetRecursiveModel() : KiColumnModel::GetRegularModel();
}

/// OnInitialUpdate virtual override, initialize list control and start the refresh timer
void CBrowseFileView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	CListCtrl& ListCtrl = GetListCtrl();

	// Replace standard header with sort header
	if( HWND hWnd = ListView_GetHeader(ListCtrl.m_hWnd) )
	{
		m_ctrlHeader.SubclassWindow(hWnd);
		
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if( pMainFrame->GetSafeHwnd() )
		{
			m_ctrlHeader.EnableHeaderFilters(pMainFrame->IsFilterMasksEnable());
			m_ctrlHeader.SetFilterChangeTimeout(gFileViewFilterMasksReactionSpeed);
		}

		m_ctrlHeader.GetNotificationManager()->CheckIn(this, ::OnFilterBarChanged);
	}

	SetListCtrlExtendedStyle(&ListCtrl, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP);

	// Set image lists
	if( !m_smallImageList.m_hImageList )
		m_smallImageList.Create(IDB_SMALLICONS, 16, 1, RGB(255, 255, 255));

	if( !m_stateImageList.m_hImageList )
		m_stateImageList.Create(IDR_STATEICONS, 16, 1, RGB(255, 0, 0));
	
	ListCtrl.SetImageList(&m_smallImageList, LVSIL_SMALL);
	ListCtrl.SetImageList(&m_stateImageList, LVSIL_STATE);

	if( m_font )
	{
		SetFont(m_font);
	}

	KoFileViewTraversalContext context(this);
	SetRecursive(context.IsRecursive());	
	
	AddColumns();

	LoadColumns(context.IsRecursive());

	// Register for notifications on filter change
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
    { 
		app->GetFilterModel()->GetNotificationManager()->CheckIn(this, ::OnFilteringChanged);
		app->GetRecursionModel()->GetNotificationManager()->CheckIn(this, ::OnFilteringChanged);
		app->GetIgnoreModel()->GetNotificationManager()->CheckIn(this, ::OnFilteringChanged);
    }

	m_uiFilechangeTimer = SetTimer(FILECHANGE_TIMER, 200, 0L);
	if( 0L == m_uiFilechangeTimer )
	{
		cvs_err("Can't start the file change notification timer.\n");
	}
}

/*!
	Add columns to view
*/
void CBrowseFileView::AddColumns()
{	
	KiColumnModel* model = GetColumnModel();
	const int columnCount = model->GetCount();
	CListCtrl& listCtrl = GetListCtrl();
	
	// Add columns
	for(int i = 0; i < columnCount; i++)
	{
		LV_COLUMN lvc;
		
		model->GetAt(i)->GetSetupData(&lvc);
		lvc.iSubItem = i;
		lvc.mask |= LVCF_SUBITEM;
		listCtrl.InsertColumn(i,&lvc);
	}
}

/*!
	Delete all columns
*/	
void CBrowseFileView::DeleteColumns()
{
	// Clear the filters
	if( m_ctrlHeader.GetSafeHwnd() )
	{
		Header_ClearAllFilters(m_ctrlHeader.GetSafeHwnd());

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if( pMainFrame->GetSafeHwnd() )
		{
			pMainFrame->SetFilterBarMask(m_ctrlHeader, GetColumnModel());
		}
	}

	CListCtrl& listCtrl = GetListCtrl();
	const int nCount = listCtrl.GetHeaderCtrl()->GetItemCount();
	
	// Delete columns
	for(int nIndex = 0; nIndex < nCount; nIndex++)
	{
		listCtrl.DeleteColumn(0);
	}
}

/*!
	Get the modification time of the CVS/Entries and CVS/Entries.log files
	\param newEntriesMod Return the modification time of the CVS/Entries file
	\param newEntriesLogMod Return the modification time of the CVS/Entries.log file
*/
void CBrowseFileView::GetEntriesModTime(time_t& newEntriesMod,  time_t& newEntriesLogMod)
{
	string entriesPath(m_path.c_str());
	NormalizeFolderPath(entriesPath);
	entriesPath += "CVS";
	entriesPath += kPathDelimiter;
	entriesPath += "Entries";

	struct stat sb;
	if( stat(entriesPath.c_str(), &sb) != -1 )
		newEntriesMod = sb.st_mtime;
	
	entriesPath += ".log";

	if( stat(entriesPath.c_str(), &sb) != -1 )
		newEntriesLogMod = sb.st_mtime;
}

/*!
	Reload and check for changes 
	\param notifyBrowser true to notify the directories tree
	\param selectFile If specified place the selection on this file
	\param forceReload true to force the refresh, false otherwise
	\param selectFolder If specified place the selection on this folder
	\note Should not be called from another thread
*/
void CBrowseFileView::ResetView(bool notifyBrowser /* = false */, const char* selectFile /* = NULL */, bool forceReload /* = true */, const char* selectFolder /*= NULL*/)
{
	ASSERT(!(selectFile && selectFolder)); // Can't select file and folder at the same time

	// No outstanding request to reset the view any more
	m_isResetViewPending = false;

	if( m_isResetViewLocked )
	{
		// No recursion
		return;
	}

	CRecursionLock recursionLock(m_isResetViewLocked);

	CWndRedrawMngr redrawMngr(this);

	// Reset dirty indicators
	m_isFilesChange = false;
	m_entriesMod = 0;
	m_entriesLogMod = 0;

	GetEntriesModTime(m_entriesMod, m_entriesLogMod);

	CWaitCursor doWait;

	CListCtrl& ListCtrl = GetListCtrl();

	// Get the selection to later try to restore it
	CvsArgs selection(false);
	CvsArgs visibleItems(false);
	CvsArgs focusedItems(false);
	CvsArgs selectionMarkItems(false);
	vector<bool> visibleItemsSelected;

	if( !selectFile && !selectFolder )
	{
		int nItem = -1;
		string buf;
		
		int nTopIndex = ListCtrl.GetTopIndex();
		int nVisible = min(ListCtrl.GetCountPerPage(), ListCtrl.GetItemCount() - nTopIndex);
		
		while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}
			
			selection.add(data->GetFullPathName(buf));
		}

		// Fetch all visible items, selected or not
		for(nItem = nTopIndex; nItem < nTopIndex + nVisible; nItem++)
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			visibleItems.add(data->GetFullPathName(buf));
			visibleItemsSelected.push_back((ListCtrl.GetItemState(nItem, LVIS_SELECTED) & LVIS_SELECTED) != 0);
		}

		nItem = -1;
		while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_FOCUSED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			focusedItems.add(data->GetFullPathName(buf));
		}

		nItem = ListCtrl.GetSelectionMark();
		if( nItem >= 0 ) 
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( !EntnodeData::IsUpOneNode(data) )
			{
				selectionMarkItems.add(data->GetFullPathName(buf));
			}
		}
	}

	if( forceReload ) 
	{
		// Mark current x scroll pos to restore it later
		int nCx = ListCtrl.GetScrollPos(SB_HORZ);

		// Delete all items
		VERIFY(ListCtrl.DeleteAllItems());

		// Set the status bar refresh message
		if( CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd() )
		{
			pMainFrame->GetStatusBar()->SetWindowText("Refreshing, press Esc to stop...");
		}

		// Make sure columns are preserved after the flat/recursive mode switch
		KoFileViewTraversalContext context(this);
		if( (context.IsRecursive() && !m_isRecursive) || (!context.IsRecursive() && m_isRecursive) )
		{
			KiColumnModel* oldModel = GetColumnModel();
			const int oldCount = oldModel->GetCount();
			
			// Preserve any filter bar text
			for(int nIndex = 0; nIndex < oldCount; nIndex++)
			{
				const std::string filterText = m_ctrlHeader.GetFilterText(nIndex);
				const int filterType = oldModel->GetType(nIndex);
				
				if( m_filterBarFilters.find(filterType) != m_filterBarFilters.end() )
				{
					m_filterBarFilters.erase(filterType);
				}
				
				if( !filterText.empty() )
				{
					m_filterBarFilters.insert(std::make_pair(filterType, filterText));
				}
			}

			// Save and delete columns
			SaveColumns(m_isRecursive);
			DeleteColumns();
			
			// Add and load new columns
			SetRecursive(context.IsRecursive());
			
			AddColumns();
			LoadColumns(m_isRecursive);

			// Set any preserved filter bar text
			if( !m_filterBarFilters.empty() )
			{
				KiColumnModel* newModel = GetColumnModel();
				const int newCount = newModel->GetCount();

				for(std::map<int, std::string>::const_iterator i = m_filterBarFilters.begin(); i != m_filterBarFilters.end(); ++i)
				{
					const int filterType = (*i).first;

					for(int pos = 0; pos < newCount; pos++)
					{
						if( filterType == newModel->GetType(pos) )
						{
							m_ctrlHeader.SetFilterText(pos, (*i).second.c_str());
							break;
						}
					}
				}

				CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
				if( pMainFrame->GetSafeHwnd() )
				{
					pMainFrame->SetFilterBarMask(m_ctrlHeader, GetColumnModel());
				}
			}
		}

		// Re-fetch all items
		TViewFill traverse(&context);
		kTraversal res = FileTraverse(m_path.c_str(), traverse);
		
		if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
		{
			// Add parent item
			if( !IsSamePath(m_path.c_str(), app->GetRoot()) )
			{
				ListCtrl.InsertItem(0, "..", kFolderIconOneUp);
			}
		}

		// Find the file for selection
		if( selectFile || selectFolder )
		{
			const char* selectPath = selectFolder ? selectFolder : selectFile;

			int count = ListCtrl.GetItemCount();
			for(int i = 0; i < count; i++)
			{
				EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(i);
				if( EntnodeData::IsUpOneNode(data) )
				{
					continue;
				}

				switch( data->GetType() )
				{
				case ENT_SUBDIR:
					// Files only
					if( selectFile )
					{
						continue;
					}

					break;
				case ENT_FILE:
					// Folders only
					if( selectFolder )
					{
						continue;
					}
					break;
				default:
					ASSERT(FALSE); // Unknown type
					break;
				}

				string buf;
				const char* currentPath = data->GetFullPathName(buf);
				if( stricmp(selectPath, currentPath) == 0 )
				{
					selection.reset(false); //we don't want any other selection
					selection.add(currentPath);
					break;
				}
			}
		}

		// Need to reset the ScrollPos and to scroll for proper functionality
		ListCtrl.SetScrollPos(SB_HORZ, 0, FALSE);
		VERIFY(ListCtrl.Scroll(CSize(nCx, 0)));
	}

	Resort();

	int argcSelection = selection.Argc();
	char* const* argvSelection = selection.Argv();

	int argcVisibleItems = visibleItems.Argc();
	char* const* argvVisibleItems = visibleItems.Argv();
	
	int argcFocusedItem = focusedItems.Argc();
	char* const* argvFocusedItem = focusedItems.Argv();

	int argcSelectionMarkItem = selectionMarkItems.Argc();
	char* const* argvSelectionMarkItem = selectionMarkItems.Argv();

	if( forceReload ) 
	{
		// Restore idle message
		if( CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd() )
		{
			CString strIdleString;
			strIdleString.LoadString(AFX_IDS_IDLEMESSAGE);
			
			pMainFrame->GetStatusBar()->SetWindowText(strIdleString);
		}

		// Restore the selection if any
		if( argcSelection )
		{
			bool firstSelected = false;

			const int count = ListCtrl.GetItemCount();
			for(int nItem = 0; nItem < count; nItem++)
			{
				EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
				if( EntnodeData::IsUpOneNode(data) )
				{
					continue;
				}

				string buf;
				const char* currentFile = data->GetFullPathName(buf);

				for(int i = 0; i < argcSelection; i++)
				{
					const char* fn = argvSelection[i];
					if( stricmp(fn, currentFile) == 0 )
					{
						if( !firstSelected )
						{
							firstSelected = true;
							VERIFY(ListCtrl.SetItemState(nItem, LVIS_FOCUSED, LVIS_FOCUSED));
						}

						VERIFY(ListCtrl.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED));
						break;
					}
				}
			}
		}
		
		// If there is no selection then set the first item focused to allow user input
		if( ListCtrl.GetItemCount() > 0 && ListCtrl.GetSelectedCount() == 0 )
		{
			VERIFY(ListCtrl.SetItemState(0, LVIS_FOCUSED, LVIS_FOCUSED));
			
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(0);
			if( EntnodeData::IsUpOneNode(data) )
			{
				VERIFY(ListCtrl.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED));
			}
		}
	}
	
	int nFirstItem = -1;
	int nFirstItemSelected = -1;
	int nFirstItemFocused = -1;
	int nFirstItemSelectionMark = -1;
	int nItem = -1;

	for(nItem = -1; (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1; )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}
		
		string buf;
		const char* currentFile = data->GetFullPathName(buf);

		int i;
		for(i = 0; i < argcSelection; i++)
		{
			const char* fn = argvSelection[i];
			if( stricmp(fn, currentFile) == 0 )
			{
				nFirstItemSelected = nItem;

				// Don't step into that loop again
				argcSelection = 0;
			}
		}

		for(i = 0; i < argcVisibleItems; i++)
		{
			// Looking for selected items only
			// Don't really need that, only for performance
			if( !visibleItemsSelected[i] )
				continue;

			const char* fn = argvVisibleItems[i];
			if( stricmp(fn, currentFile) == 0 )
			{
				// Set the position of that item as close as possible to the
				// previous position
				nFirstItem = max(nItem - i, 0);

				// Don't step into that loop again
				argcVisibleItems = 0;
			}
		}

		for(i = 0; i < argcFocusedItem; i++)
		{
			const char* fn = argvFocusedItem[i];
			if( stricmp(fn, currentFile) == 0 )
			{
				nFirstItemFocused = nItem;

				// Don't step into that loop again
				argcFocusedItem = 0;
			}
		}

		for(i = 0; i < argcSelectionMarkItem; i++)
		{
			const char* fn = argvSelectionMarkItem[i];
			if( stricmp(fn, currentFile) == 0 )
			{
				nFirstItemSelectionMark = nItem;

				// Don't step into that loop again
				argcSelectionMarkItem = 0;
			}
		}

		// Optimization, leave loop if nothing to do:
		// (Binary | is probably faster than logical &&)
		if( (argcSelection | argcVisibleItems | argcFocusedItem | argcSelectionMarkItem) == 0 )
			break;
	}

	// All the previously selected items have been deselected?
	// Look for the first previously deselected item
	if( nFirstItem < 0 )
	{
		int nItems = ListCtrl.GetItemCount();
		for(nItem = 0; nItem < nItems; nItem++)
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}
			
			string buf;
			const char* currentFile = data->GetFullPathName(buf);

			for(int i = 0; i < argcVisibleItems; i++)
			{
				// Looking for deselected items only
				// Don't really need that, only for performance
				if( visibleItemsSelected[i] )
					continue;

				const char* fn = argvVisibleItems[i];
				if( stricmp(fn, currentFile) == 0 )
				{
					// Set the position of that item as close as possible to the
					// previous position
					nFirstItem = max(nItem - i, 0);

					// Exit all loops
					argcVisibleItems = 0;
					nItems = 0;
				}
			}
		}
	}
	
	// All the previously visible files have been removed? Ensure the first selected item is visible
	if( nFirstItem < 0 )
		nFirstItem = nFirstItemSelected;
	
	// Make sure the first selected item is visible
	if( nFirstItem >= 0 ) 
	{
		const int bottomMargin = 3;
		const int pageSize = ListCtrl.GetCountPerPage();
		if( nFirstItem > pageSize - bottomMargin )
		{
			const int maxItem = ListCtrl.GetItemCount() - 1;
			ListCtrl.EnsureVisible(min(maxItem, nFirstItem + bottomMargin), FALSE);
		}
		else
		{
			// Fits in the page
			VERIFY(ListCtrl.EnsureVisible(0, FALSE));
		}

		VERIFY(ListCtrl.EnsureVisible(nFirstItem, FALSE));
	}

	// Need to traverse all deselected items to set focus, since focus may be set
	// on inactive item
	// (For performance reasons, the focus item has been checked in the previous loop
	//  over all selected items, since this is the most frequent case)
	// The selection mark item is selected by definition and does not need
	// to be checked here
	for(nItem = -1; argcFocusedItem && (nItem = ListCtrl.GetNextItem(nItem, LVNI_ALL)) != -1; )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}
		
		string buf;
		const char* currentFile = data->GetFullPathName(buf);

		int i;
		for(i = 0; i < argcFocusedItem; i++)
		{
			const char* fn = argvFocusedItem[i];
			if( stricmp(fn, currentFile) == 0 )
			{
				nFirstItemFocused = nItem;

				// Don't step into that loop again, leave outer loop
				argcFocusedItem = 0;
			}
		}
	}

	// Only change focus item if the focus was set before to an item that has
	// disappeared now
	if( argcFocusedItem != 0 )
	{
		// Set focus on first selected item if no focus has been set yet
		if( nFirstItemFocused < 0 )
			nFirstItemFocused = nFirstItemSelected;

		// Set focus on first visible item if no focus has been set yet
		if( nFirstItemFocused < 0 )
			nFirstItemFocused = nFirstItem;
	}
	
	// Only set focus if we have an item to set the focus to
	if( (nFirstItemFocused >= 0) && (nFirstItemFocused < ListCtrl.GetItemCount()) )
	{
		VERIFY(ListCtrl.SetItemState(nFirstItemFocused, LVIS_FOCUSED, LVIS_FOCUSED) !=0);
	}

	// Only change selection mark item if the selection mark was set before to
	// an item that has disappeared now
	if( argcSelectionMarkItem != 0 )
	{
		// Set focus on first selected item if no focus has been set yet
		if( nFirstItemSelectionMark < 0 )
			nFirstItemSelectionMark = nFirstItemFocused;
	}

	// Only set selection mark if we have an item to set the selection mark to
	if( (nFirstItemSelectionMark >= 0) && (nFirstItemSelectionMark < ListCtrl.GetItemCount()) )
	{
		ListCtrl.SetSelectionMark(nFirstItemSelectionMark);
	}

	if( notifyBrowser )
	{
		// Notify the tree
		if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
		{
			if( CWinCvsBrowser* pBrowser = app->GetBrowserView() )
			{
				pBrowser->ResetView(true);
			}
		}
	}

	// Start monitoring file changes
	if( !m_watch )
	{
		m_watch = new KoWatcher(m_hWnd);
	}

	m_watch->SignalStartWatch(m_path.c_str(), m_isRecursive);
	
	// FIXME -> temporary fix for PWD problems
	chdir(m_path.c_str());
}

/*!
	Reset the view to the given path
	\param path Path to reset the view
	\param notifyBrowser true to notify the directories tree
	\param selectFile If specified place the selection on this file
	\param forceReload true to force the refresh, false otherwise
	\param selectFolder If specified place the selection on this folder
*/
void CBrowseFileView::ResetView(const char* path, bool notifyBrowser /* = false */, const char* selectFile /* = NULL */, bool forceReload /* = true */, const char* selectFolder /*= NULL*/)
{
	m_path = path;
	NormalizeFolderPath(m_path, false);

	if( CWnd* pWnd = AfxGetMainWnd() )
	{
		CMainFrame* pMainFrame = (CMainFrame*)pWnd;
		
		if( pMainFrame->GetSafeHwnd() )
		{
			KoColumnContext context(m_path.c_str());
			
			// Update the column context
			pMainFrame->SetContext(&context);

			// Update the filter bar display
			m_ctrlHeader.EnableHeaderFilters(pMainFrame->IsFilterMasksEnable());
			m_ctrlHeader.SetFilterChangeTimeout(gFileViewFilterMasksReactionSpeed);

			// Update the Status bar
			if( CCvsStatusBar* pStatusBar = (CCvsStatusBar*)pMainFrame->GetStatusBar() )
			{
				pStatusBar->UpdatePathInfo(path);
			}
		}
	}

	ResetView(false, selectFile, forceReload, selectFolder);
	
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		app->GetDocument()->SetTitle(m_path.c_str());
		
		if( notifyBrowser )
		{
			// Notify the dir tree
			if( CWinCvsBrowser* pBrowser = app->GetBrowserView() )
			{
				pBrowser->StepToLocation(path);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBrowseFileView diagnostics

#ifdef _DEBUG
void CBrowseFileView::AssertValid() const
{
	CListView::AssertValid();
}

void CBrowseFileView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/*!
	Set the view type
	\param dwViewType New view type
	\return TRUE on success, FALSE otherwise
*/
BOOL CBrowseFileView::SetViewType(DWORD dwViewType)
{
	return ModifyStyle(LVS_TYPEMASK, dwViewType & LVS_TYPEMASK);
}

/*!
	Get the view type
	\return The view type
*/
DWORD CBrowseFileView::GetViewType() const
{
	return GetStyle() & LVS_TYPEMASK;
}

/*!
	Set the small icons view
*/
void CBrowseFileView::OnViewSmallIcons() 
{
	if( GetViewType() != LVS_SMALLICON )
		SetViewType(LVS_SMALLICON);
}

/*!
	Set the list view
*/
void CBrowseFileView::OnViewList() 
{
	if( GetViewType() != LVS_LIST )
		SetViewType(LVS_LIST);
}

/*!
	Set the full row details view
*/
void CBrowseFileView::OnViewFullRowDetails() 
{
	if( GetViewType() != LVS_REPORT )
		SetViewType(LVS_REPORT);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewSmallIcons(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetViewType() == LVS_SMALLICON);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewList(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetViewType() == LVS_LIST);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewFullRowDetails(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetViewType() == LVS_REPORT);
}

/*!
	Enter the folder if it exists
	\param fullpath Full path of the folder to enter
*/
void CBrowseFileView::EnterFolder(const char* fullpath)
{
	struct stat st;
	if( stat(fullpath, &st) != 0 && errno == ENOENT )
	{
		// Assume we're looking at directory previously in CVS, 
		// but not currently in the file system.
		// If we ever run across another case (permissions perhaps)
		// then you might want to add code here.
		cvs_err("'%s' is not accessible\n", (char*)fullpath);
	}
	else
	{
		ResetView(fullpath, true);
		gCvsPrefs.SetLastWorkingDir(fullpath);
	}
}

/// WM_LBUTTONDBLCLK message handler, edit the selection
void CBrowseFileView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() )
		return;

	CListCtrl& ListCtrl = GetListCtrl();

	UINT pFlags;
	int nItem = ListCtrl.HitTest(point, &pFlags);
	if( nItem != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			OnViewUpone();
		}
		else if( data->GetType() == ENT_SUBDIR )
		{
			string buf;
			EnterFolder(data->GetFullPathName(buf));
		}
		else
		{
			KoFileActionHandler handler;
			ProcessSelectionCommand(&handler);
		}

		return;
	}

	CListView::OnLButtonDblClk(nFlags, point);
}

/*!
	Check whether the commands should be disabled
	\param bCheckSelection true if selection should be taken into consideration
	\return true if commands should be disabled, false otherwise
*/
bool CBrowseFileView::DisableCommon(bool bCheckSelection /*= true*/)
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	CListCtrl& ListCtrl = GetListCtrl();

	const bool bUpOneSelected = ListCtrl.GetItemCount() > 0 && 
		((ListCtrl.GetItemState(0, LVIS_SELECTED) & LVIS_SELECTED) == LVIS_SELECTED) &&
		EntnodeData::IsUpOneNode((EntnodeData*)ListCtrl.GetItemData(0));

	const bool bCommon = app->IsCvsRunning() || ListCtrl.GetSelectedCount() == 1 && bUpOneSelected;
	const bool bSelection = ListCtrl.GetSelectedCount() == 0 || this != CWnd::GetFocus();

	return bCheckSelection ? bCommon || bSelection : bCommon;
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewRelease(CCmdUI* pCmdUI)
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CListCtrl& ListCtrl = GetListCtrl();
	
	int nItem = -1;
	BOOL res = TRUE;
	int numFiles = 0;
	int numFolders = 0;

	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->IsUnknown() || data->GetType() != ENT_SUBDIR )
		{
			res = FALSE;
			break;
		}

		numFolders++;

		if( numFolders > 1 )
		{
			res = FALSE;
			break;
		}
	}

	pCmdUI->Enable(res);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewAdd(CCmdUI* pCmdUI)
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CListCtrl& ListCtrl = GetListCtrl();
	
	int nItem = -1;
	BOOL res = TRUE;
	int numFiles = 0;
	int numFolders = 0;

	string strPath;

	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( !(data->IsUnknown() || (data->IsRemoved() && data->IsMissing())) )
		{
			res = FALSE;
			break;
		}

		if( data->GetType() == ENT_FILE )
		{
			numFiles++;

			if( strPath.empty() )
			{
				strPath = data->GetFolderPath();
			}
			else
			{
				if( strPath != data->GetFolderPath() )
				{
					res = FALSE;
					break;
				}
			}
		}
		else
		{
			numFolders++;
		}

		if( (numFiles != 0 && numFolders != 0) || numFolders > 1 )
		{
			res = FALSE;
			break;
		}
	}

	pCmdUI->Enable(res);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewAddB(CCmdUI* pCmdUI)
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CListCtrl& ListCtrl = GetListCtrl();
	
	int nItem = -1;
	BOOL res = TRUE;

	string strPath;

	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->GetType() == ENT_FILE )
		{
			if( strPath.empty() )
			{
				strPath = data->GetFolderPath();
			}
			else
			{
				if( strPath != data->GetFolderPath() )
				{
					res = FALSE;
					break;
				}
			}
		}
		else
		{
			res = FALSE;
			break;
		}

		if( !(data->IsUnknown() || (data->IsRemoved() && data->IsMissing())) )
		{
			res = FALSE;
			break;
		}
	}

	pCmdUI->Enable(res);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewUpdate(CCmdUI* pCmdUI)
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CListCtrl& ListCtrl = GetListCtrl();
	
	int nItem = -1;
	BOOL res = TRUE;
	int numFiles = 0;
	int numFolders = 0;

	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->IsUnknown() )
		{
			res = FALSE;
			break;
		}

		if( data->GetType() == ENT_FILE )
			numFiles++;
		else
			numFolders++;

		if( (numFiles != 0 && numFolders != 0) || numFolders > 1 )
		{
			res = FALSE;
			break;
		}
	}

	pCmdUI->Enable(res);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewGraph(CCmdUI* pCmdUI)
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CListCtrl& ListCtrl = GetListCtrl();
	
	int nItem = -1;
	BOOL res = TRUE;

	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->IsUnknown() || data->GetType() == ENT_SUBDIR )
		{
			res = FALSE;
			break;
		}
	}

	pCmdUI->Enable(res);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewRmv(CCmdUI* pCmdUI)
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CListCtrl& ListCtrl = GetListCtrl();
	
	int nItem = -1;
	BOOL res = TRUE;
	int numFiles = 0;
	int numFolders = 0;

	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->IsUnknown() || data->IsRemoved() )
		{
			res = FALSE;
			break;
		}
	
		if( data->GetType() == ENT_FILE )
			numFiles++;
		else
			numFolders++;
		
		if( (numFiles != 0 && numFolders != 0) || numFolders > 1 )
		{
			res = FALSE;
			break;
		}
	}

	pCmdUI->Enable(res);
}

/*!
	Resort according to the current key
*/
void CBrowseFileView::Resort(void)
{
	CListCtrl& ListCtrl = GetListCtrl();
	KoColumnContext context(GetPath().c_str());
	KiColumnModel* model = GetColumnModel();
	
	const int sort = m_isRecursive ? m_sortFlat : m_sortRegular;
	const bool ascendant = m_isRecursive ? m_ascendantFlat : m_ascendantRegular;

	CSortParam sortParam(ascendant, true, model->GetAt(sort), model->GetAt(0), &context);
	m_ctrlHeader.SetSortImage(sort, ascendant);

	ListCtrl.SortItems(_Compare, (LPARAM)&sortParam);
}

/// LVN_COLUMNCLICK notification handler, sort items
void CBrowseFileView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CListCtrl& ListCtrl = GetListCtrl();

	int& sort = m_isRecursive ? m_sortFlat : m_sortRegular;
	bool& ascendant = m_isRecursive ? m_ascendantFlat : m_ascendantRegular;

	if( pNMListView->iSubItem == sort )
	{
		ascendant = !ascendant;
	}
	else
	{
		sort = pNMListView->iSubItem;
		
		KiColumnModel* model = GetColumnModel();
		ascendant = model->GetAt(sort)->IsDefaultAscending();
	}

	// Internally performs a Resort() with restoring the current view position
	ResetView(false, NULL, false);

	*pResult = 0;
}

/*!
	Process the selection handler command
	\param handler Command selection handler
	\return true if the command was processed, false otherwise
*/
bool CBrowseFileView::ProcessSelectionCommand(KiSelectionHandler* handler)
{
	bool res = false;

	CListCtrl& ListCtrl = GetListCtrl();
	int nItem = -1;
	int pathLen = GetPath().length();

	MultiFiles mf;
	
	// First add the folders
	mf.newdir(GetPath().c_str());
	
	// Process selection
	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		string buf;
		const char* fullpath = data->GetFullPathName(buf);
		
		if( data->GetType() == ENT_SUBDIR && !data->IsMissing() )
		{						
			handler->OnFolder(fullpath);
			res = true;

			break;
		}
		else
		{
			const char* fn = data->GetFullPathName(buf) + pathLen;
			if( *fn == '\\' )
			{
				fn++;
			}

			mf.newfile(fn, 0, ((*data)[EntnodeFile::kVN]).c_str());
		}
	}

	if( mf.TotalNumFiles() != 0 )
	{
		handler->OnFiles(&mf);
		res = true;
	}

	return res;
}

/// WM_COMMAND message handler, <b>cvs add</b> selection
void CBrowseFileView::OnViewAdd() 
{
	KoAddHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs add -kb</b> selection
void CBrowseFileView::OnViewAddb() 
{
	KoAddBinaryHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs add -ku</b> selection
void CBrowseFileView::OnViewAddu() 
{
	KoAddUnicodeHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs commit</b> selection
void CBrowseFileView::OnViewCommit() 
{
	KoCommitHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs remove</b> selection
void CBrowseFileView::OnViewRmv() 
{
	KoFileRemoveHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs update</b> selection
void CBrowseFileView::OnViewUpdate() 
{
	KoUpdateHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs -n update</b> selection
void CBrowseFileView::OnViewQueryUpdate() 
{
	KoQueryUpdateHandler handler;
	ProcessSelectionCommand(&handler);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewReload(CCmdUI* pCmdUI) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	pCmdUI->Enable(!app->IsCvsRunning());
}

/// WM_COMMAND message handler, refresh view
void CBrowseFileView::OnViewReload() 
{
	ResetView(true);
}

/// LVN_KEYDOWN notification handler, handle selected keys
void CBrowseFileView::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() )
		return;

	CListCtrl& ListCtrl = GetListCtrl();
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	if( pLVKeyDow->wVKey == VK_BACK )
	{
		OnViewUpone();
	}
	else if( pLVKeyDow->wVKey == VK_RETURN && ListCtrl.GetSelectedCount() == 1 )
	{
		int nItem = -1;
		if( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				OnViewUpone();
			}
			else if( data->GetType() == ENT_SUBDIR )
			{
				string buf;
				EnterFolder(data->GetFullPathName(buf));
			}
			else
			{
				KoFileActionHandler handler;
				ProcessSelectionCommand(&handler);
			}
		}
	}
	
	*pResult = 0;
}

/*!
	Get the path length
	\param path The path to get the length for
	\return true on success, false otherwise
	\note Takes the ending path delimiter into consideration
*/
int CBrowseFileView::GetPathLength(LPCTSTR path)
{
	int len = _tcslen(path);
	return (len > 0 && path[len - 1] == kPathDelimiter) ? (len - 1) : len;
}

/*!
	Compare two paths
	\param path1 First path
	\param path2 Seconds path
	\return true if paths are same, false otherwise
*/
bool CBrowseFileView::IsSamePath(LPCTSTR path1, LPCTSTR path2)
{	 
	int len1 = GetPathLength(path1);
	int len2 = GetPathLength(path2);
	return len1 == len2 && _tcsnicmp(path1, path2, len1) == 0;
}

/// WM_COMMAND message handler, go up one directory level unless we are at the root already
void CBrowseFileView::OnViewUpone() 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( !IsSamePath(m_path.c_str(), app->GetRoot()) && chdir(m_path.c_str()) == 0 && chdir("..") == 0 )
	{
		char newpath[1024];
		getcwd(newpath, 1023);

		string strSelectFolder(m_path);
		ResetView(newpath, true, NULL, true, strSelectFolder.c_str());
	}
	else
	{
		MessageBeep(MB_ICONHAND);
	}
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewUpone(CCmdUI* pCmdUI) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	pCmdUI->Enable(!app->IsCvsRunning() && !IsSamePath(m_path.c_str(), app->GetRoot()));
}

/// WM_COMMAND message handler, move the selection to trash
void CBrowseFileView::OnViewTrash() 
{
	KoMoveToTrashHandler handler;
	ProcessSelectionCommand(&handler);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewTrash(CCmdUI* pCmdUI) 
{
	bool enabled = !DisableCommon();
	if( enabled )
	{
		KoUpdateMoveToTrashHandler handler;
		ProcessSelectionCommand(&handler);
		enabled = handler.IsEnabled();
	}

	pCmdUI->Enable(enabled);
}

/// WM_COMMAND message handler, <b>[cvs] diff</b> selection
void CBrowseFileView::OnViewDiff() 
{
	KoDiffHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs log</b> selection
void CBrowseFileView::OnViewLog() 
{
	KoLogHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, graph selection
void CBrowseFileView::OnViewGraph() 
{
	KoGraphHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs status</b> selection
void CBrowseFileView::OnViewStatus() 
{
	KoStatusHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs admin -u</b> selection
void CBrowseFileView::OnViewUnlock() 
{
	KoUnlockHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs watch add</b> selection
void CBrowseFileView::OnViewWatchOn() 
{
	KoWatchOnHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs watch remove</b> selection
void CBrowseFileView::OnViewWatchOff()
{
	KoWatchOffHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs edit</b> selection
void CBrowseFileView::OnViewEdit()
{
	KoEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs edit -c</b> selection
void CBrowseFileView::OnViewReservededit() 
{
	KoReservedEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs edit -f</b> selection
void CBrowseFileView::OnViewForceedit() 
{
	KoForceEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs unedit</b> selection
void CBrowseFileView::OnViewUnedit()
{
	KoUneditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs release</b> selection
void CBrowseFileView::OnViewRelease()
{
	KoReleaseHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs watchers</b> selection
void CBrowseFileView::OnViewWatchers()
{
	KoWatchersHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs editors</b> selection
void CBrowseFileView::OnViewEditors()
{
	KoEditorsHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_CONTEXTMENU message handler, display the custom menu
void CBrowseFileView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if( DisableCommon() )
	{
		return;
	}

	CListCtrl& listCtrl = GetListCtrl();
	if( listCtrl.GetSelectedCount() > 0 )
	{
		// Try to find a selected and focused item
		int item = listCtrl.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED);
		if( item == -1 )
		{
			// Get the first selected item then
			item = listCtrl.GetNextItem(-1, LVNI_SELECTED);
		}

		if( item > -1 )
		{
			// Set the track point in case it's not the mouse click
			if( -1 == point.x && -1 == point.y )
			{
				// Get the icon's rectangle
				CRect rectItem;
				if( listCtrl.GetItemRect(item, &rectItem, LVIR_ICON) )
				{
					point = rectItem.CenterPoint();
					ClientToScreen(&point);
				}
			}
			
			// Display the pop-up menu
			std::auto_ptr<CMenu> pCustomMenu(::GetCustomMenu(kCustomFilesMenu, this));
			if( pCustomMenu.get() )
			{
				pCustomMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
			}
		}
	}
}

/// WM_COMMAND message handler, <b>cvs tag</b> selection
void CBrowseFileView::OnViewTagNew() 
{
	KoCreateTagHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs tag -d</b> selection
void CBrowseFileView::OnViewTagDelete() 
{
	KoDeleteTagHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs tag -b</b> selection
void CBrowseFileView::OnViewTagBranch() 
{
	KoBranchTagHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, customize menus
BOOL CBrowseFileView::OnCustomize(UINT nID)
{
	CustomizeMenus(nID == ID_CUST_FILES_MENU ? kCustomFilesMenu : kCustomBrowserMenu);
	return TRUE;
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateCustomize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

/// WM_COMMAND message handler, call the macro on selection
BOOL CBrowseFileView::OnMacro(UINT nID)
{
	PyDoPython(WINCMD_to_UCMD(nID));

	return 1;
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateMacro(CCmdUI* pCmdUI) 
{
	if( DisableCommon(false) )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	// Fill the cache with current selection
	if( !PyIsUICacheValid() )
	{
		CListCtrl& ListCtrl = GetListCtrl();
		int nItem = -1;
		while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			PyAppendCache(data);
		}
	}

	UCmdUI ucmdui(WINCMD_to_UCMD(pCmdUI->m_nID));
	ucmdui.pCmdUI = pCmdUI;
	PyDoCmdUI(&ucmdui);
}

/// WM_COMMAND message handler, explore the selection
void CBrowseFileView::OnViewExplore() 
{
	CListCtrl& ListCtrl = GetListCtrl();

	if( ListCtrl.GetSelectedCount() )
	{
		int nItem = -1;

		while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			if( data->IsMissing() )
			{
				continue;
			}

			string path;
			data->GetFullPathName(path);
			TrimRight(path, kPathDelimiter);
			
			LaunchExplore(path.c_str());
		}
	}
	else
	{
		LaunchExplore(m_path.c_str());
	}
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewExplore(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	bool enable = false;

	CListCtrl& listCtrl = GetListCtrl();
	int nItem = -1;
	
	if( listCtrl.GetSelectedCount() )
	{
		while( (nItem = listCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)listCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			if( !data->IsMissing() )
			{
				enable = true;
				break;
			}
		}
	}
	else
	{
		if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
		{
			if( CWinCvsBrowser* pBrowser = app->GetBrowserView() )
			{
				enable = pBrowser->GetTreeCtrl().GetRootItem() != NULL;
			}
		}
	}

	pCmdUI->Enable(enable);
}


/// WM_COMMAND message handler, open the selection in new instance
void CBrowseFileView::OnViewNewInstance() 
{
	CListCtrl& ListCtrl = GetListCtrl();

	if( ListCtrl.GetSelectedCount() )
	{
		int nItem = -1;

		while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			if( data->IsMissing() )
			{
				continue;
			}

			string path;
			data->GetFullPathName(path);
			
			LaunchNewInst(path.c_str());
		}
	}
	else
	{
		LaunchNewInst(m_path.c_str());
	}
}

/// WM_COMMAND message handler, show the selection properties
void CBrowseFileView::OnQueryProperties() 
{
	CListCtrl& listCtrl = GetListCtrl();
	
	if( listCtrl.GetSelectedCount() )
	{
		int nItem = -1;

		while( (nItem = listCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)listCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			if( data->IsMissing() )
			{
				continue;
			}

			string path;
			data->GetFullPathName(path);
			
			LaunchProperties(path.c_str());
		}
	}
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateQueryProperties(CCmdUI* pCmdUI) 
{
	bool enable = false;

	CListCtrl& listCtrl = GetListCtrl();
	
	if( listCtrl.GetSelectedCount() )
	{
		int nItem = -1;

		while( (nItem = listCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)listCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			if( !data->IsMissing() )
			{
				enable = true;
				break;
			}
		}
	}

	pCmdUI->Enable(enable);
}

/// WM_TIMER message handler, check for the file changes
void CBrowseFileView::OnTimer(UINT_PTR nIDEvent) 
{
	if( nIDEvent == m_uiFilechangeTimer )
	{
		if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
		{
			if( !app->IsCvsRunning() && !app->IsModalDialog() && ( ( gCvsPrefs.RefreshInactiveTimeOut() > 0 ) || !app->IsBackgroundProcess() ) )
			{
				// cvs command finished
				if( m_checkChanges )
				{
					// Refresh the directory tree and file view
					m_checkChanges = false;
					ResetView(true);
					
					return;
				}
				
				// file change
				if( m_isFilesChange )
				{				
					ResetView();
				}
				
				// check if we need to reload
				time_t newEntriesMod = m_entriesMod;
				time_t newEntriesLogMod = m_entriesLogMod;
				
				GetEntriesModTime(newEntriesMod, newEntriesLogMod);
				
				if( m_entriesMod != newEntriesMod || m_entriesLogMod != newEntriesLogMod )
				{
					ResetView();
				}
			}
		}

		return;
	}

	CListView::OnTimer(nIDEvent);
}

/// KoWatcher::WM_FILECHANGE message handler, mark the files change
LRESULT CBrowseFileView::OnFileChange(WPARAM, LPARAM)
{
	m_isFilesChange = true;

	return 0;
}

/// OnBeginPrinting virtual override
void CBrowseFileView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CListView::OnBeginPrinting(pDC, pInfo);

	int nPageHeight = pDC->GetDeviceCaps(VERTRES);
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	// compute the bounds
	CListCtrl& ListCtrl = GetListCtrl();
	m_numberOfLines = ListCtrl.GetItemCount();
	m_heightOfLines = 2 * tm.tmHeight;
	m_numberByPages = nPageHeight / m_heightOfLines;
	m_numberOfPages = m_numberOfLines / m_numberByPages + 1;

	pInfo->SetMaxPage(m_numberOfPages);
}

/// OnPreparePrinting virtual override
BOOL CBrowseFileView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	BOOL res = DoPreparePrinting(pInfo);
	if( !res )
		return res;

	return TRUE;
}

/// OnPrint virtual override
void CBrowseFileView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if( !pDC->IsPrinting() ) 
		return;

	// Now compute the first item to draw
	CListCtrl& ListCtrl = GetListCtrl();
	int nFirstItem = -1;
	int numToIgnore = m_numberByPages * (pInfo->m_nCurPage - 1);
	
	while( numToIgnore-- && (nFirstItem = ListCtrl.GetNextItem(nFirstItem, LVNI_ALL)) != -1 )
	{
	}
	
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	int nItem = nFirstItem, n = 0;
	int horz = pInfo->m_rectDraw.Width();
	int vert = pInfo->m_rectDraw.Height();
	
	// Measure for each column
	KiColumnModel* model = GetColumnModel();
	int columnCount = model->GetCount();
	std::vector<int> widths(columnCount, 0);
	int totalwidth = 0;
	int j;
	
	KoColumnContext context(GetPath().c_str());
	while( n++ < m_numberByPages && (nItem = ListCtrl.GetNextItem(nItem, LVNI_ALL)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		for(j = 0; j < columnCount; j++)
		{
			TCHAR buffer[256];
			model->GetAt(j)->GetText(&context, data, buffer, 256);
			int width = pDC->GetTextExtent(buffer, _tcslen(buffer)).cx + kSpc;
			widths[j] = max(widths[j], width);
		}
	}

	for(j = 0; j < columnCount; j++)
	{
		totalwidth += widths[j];
	}

	for(j = 0; j < columnCount; j++)
	{
		widths[j] += (horz - totalwidth) / columnCount;
	}

	// Compute the margins
	for(j = columnCount - 1; j >= 0; j--)
	{
		int margin = 0;
		for(int i = j - 1; i >= 0; i--)
		{
			margin += widths[i];
		}
		
		widths[j] = margin;
	}

	// Now print
	nItem = nFirstItem;
	n = 0;
	CPoint off = pInfo->m_rectDraw.TopLeft();

	while( n++ < m_numberByPages && (nItem = ListCtrl.GetNextItem(nItem, LVNI_ALL)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		for(j = 0; j < columnCount; j++)
		{
			const string& info = (*data)[j];
			if( info.empty() )
				continue;

			CPoint point = off;
			point.x += widths[j];
			CRect rect(0, 0, 0, 0);

			pDC->DrawText(info.c_str(), -1, &rect, DT_CALCRECT);
			rect.OffsetRect(point);
			
			pDC->DrawText(info.c_str(), -1, &rect, DT_CENTER);
		}
		
		off += CPoint(0, m_heightOfLines);
	}
	
	//CListView::OnPrint(pDC, pInfo);
}

/// WM_COMMAND message handler, edit selection
void CBrowseFileView::OnViewEditsel() 
{
	KoLaunchEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, edit selection
void CBrowseFileView::OnViewEditseldef() 
{
	KoLaunchDefaultEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, open selection
void CBrowseFileView::OnViewOpensel() 
{
	KoLaunchOpenHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, open selection using Open With... dialog
void CBrowseFileView::OnViewOpenselas() 
{
	KoLaunchOpenAsHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, launch default associated shell action
void CBrowseFileView::OnQueryShellactionDefault() 
{
	KoLaunchDefaultHandler handler;
	ProcessSelectionCommand(&handler);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewEditseldef(CCmdUI* pCmdUI)
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	CListCtrl& ListCtrl = GetListCtrl();
	int nItem = -1;
	BOOL res = TRUE;
	
	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->GetType() == ENT_SUBDIR || data->IsMissing() )
		{
			res = FALSE;
			break;
		}
	}
	
	pCmdUI->Enable(res);
}

/// WM_COMMAND message handler, <b>cvs co</b>
void CBrowseFileView::OnViewCheckout() 
{
	KoCheckoutHandler handler;

	if( !ProcessSelectionCommand(&handler) )
	{
		handler.OnFolder(m_path.c_str());
	}
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewCheckout(CCmdUI* pCmdUI) 
{
	if( DisableCommon(false) )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
	pCmdUI->SetText("Chec&kout module...");
}

/// WM_COMMAND message handler, <b>cvs import</b> selection
void CBrowseFileView::OnViewImport() 
{
	KoImportHandler handler;

	if( !ProcessSelectionCommand(&handler) )
	{
		handler.OnFolder(m_path.c_str());
	}
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewImport(CCmdUI* pCmdUI) 
{
	if( DisableCommon(false) )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText("&Import module...");
}

/*!
	Drag and drop
	\note Implemented by Nikita Jorniak <nikita@tanner.com>
	Directories are intentionally left out
	This works to drag files from WinCVS and drop them to any external viewer/editor which supports Windows Shell standard CF_HDROP Clipboard style
*/
void CBrowseFileView::DoDrag()
{
	CListCtrl& ListCtrl = GetListCtrl();
	int nItem=-1;
	int nSize = 0;
	
	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->GetType() != ENT_SUBDIR )
		{
			string buf;
			const char* fullpath = data->GetFullPathName(buf);
			nSize += strlen(fullpath) + 1;
		}
	}

	if( nSize > 0 )
	{
		HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES)+nSize+1);
		if( hMem != NULL )
		{
			DROPFILES& df = *reinterpret_cast<DROPFILES*>(hMem);
			df.pFiles = sizeof(DROPFILES);
			df.fNC = FALSE;
			df.pt.x = df.pt.y = 0;
			df.fWide = FALSE;

			nItem=-1;
			int Offset = sizeof(DROPFILES);
			while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
			{
				EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
				if( EntnodeData::IsUpOneNode(data) )
				{
					continue;
				}

				if( data->GetType() != ENT_SUBDIR)
				{
					string buf;
					const char* fullpath = data->GetFullPathName(buf);
					int len = strlen(fullpath) + 1;
					memcpy(reinterpret_cast<BYTE*>(hMem)+Offset, fullpath, len);
					Offset+=len;
				}
			}

			reinterpret_cast<BYTE*>(hMem)[Offset]=0;

			//! \todo
			//!  Rewrite with RAII or with smart pointer
			COleDataSource* pDS=new COleDataSource(); //throw
			pDS->CacheGlobalData( CF_HDROP, hMem );
			pDS->DoDragDrop( DROPEFFECT_COPY );
			pDS->InternalRelease();
		}
	}
}

/// LVN_BEGINDRAG notification handler
void CBrowseFileView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DoDrag();
	*pResult = 0;
}

/// LVN_BEGINRDRAG notification handler
void CBrowseFileView::OnBeginrdrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DoDrag();
	*pResult = 0;
}

/// WM_DESTROY message handler, save settings, unregister change notifications and kill timers
void CBrowseFileView::OnDestroy() 
{
	// Kill the timer
	if( m_uiFilechangeTimer )
	{
		KillTimer(m_uiFilechangeTimer);
	}

	// Stop file watch
	if( m_watch )
	{
		m_watch->SignalTerminate();
		m_watch = NULL;
	}
	
	// Save settings
	SaveColumns(m_isRecursive);

	// Unregister notifications 
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
    { 
		app->GetFilterModel()->GetNotificationManager()->CheckOut(this);
		app->GetRecursionModel()->GetNotificationManager()->CheckOut(this);
		app->GetIgnoreModel()->GetNotificationManager()->CheckOut(this);
    }

	m_ctrlHeader.GetNotificationManager()->CheckOut(this);

	CListView::OnDestroy();
}

/*!
	Format the entry prefix
	\param isRecursive Recursive state
	\return Formatted entry prefix
*/
CString CBrowseFileView::FormatEntrryPrefix(bool isRecursive) const
{
	return isRecursive ? STR_FILEDETAILVIEW_COLUMN_PREFIX_FLAT : STR_FILEDETAILVIEW_COLUMN_PREFIX_REG;
}

/*!
	Save column state
	\param isRecursive Recursive state
*/
void CBrowseFileView::SaveColumns(bool isRecursive)
{
	const int columnCount = m_ctrlHeader.GetItemCount();

	CString columnsWidth;
	CString columnsOrder;

	CString strNum;

	for(int nIndex = 0; nIndex < columnCount; nIndex++)
	{
		HDITEM headerItem = { 0 };
		headerItem.mask = HDI_WIDTH;

		m_ctrlHeader.GetItem(nIndex, &headerItem);

		strNum.Format("%d", headerItem.cxy);
		columnsWidth += strNum + CHR_FILEDETAILVIEW_COLUMN_DATASEP;
	}

	int* orderArray = new int[columnCount];

	if( orderArray && m_ctrlHeader.GetOrderArray(orderArray, columnCount) )
	{
		for(int nIndex = 0; nIndex < columnCount; nIndex++)
		{
			strNum.Format("%d", orderArray[nIndex]);
			columnsOrder += strNum + CHR_FILEDETAILVIEW_COLUMN_DATASEP;
		}
		
		delete [] orderArray;
	}

	int sort = isRecursive ? m_sortFlat : m_sortRegular;
	bool ascendant = isRecursive ? m_ascendantFlat : m_ascendantRegular;

	AfxGetApp()->WriteProfileInt(STR_FILEDETAILVIEW_COLUMN_SECTION,
		FormatEntrryPrefix(isRecursive) + STR_FILEDETAILVIEW_COLUMN_ENTRY_SORT, 
		sort);

	AfxGetApp()->WriteProfileInt(STR_FILEDETAILVIEW_COLUMN_SECTION,
		FormatEntrryPrefix(isRecursive) + STR_FILEDETAILVIEW_COLUMN_ENTRY_SORTASC,
		ascendant);

	AfxGetApp()->WriteProfileString(STR_FILEDETAILVIEW_COLUMN_SECTION, 
		FormatEntrryPrefix(isRecursive) + STR_FILEDETAILVIEW_COLUMN_ENTRY_WIDTH, 
		columnsWidth);

	AfxGetApp()->WriteProfileString(STR_FILEDETAILVIEW_COLUMN_SECTION, 
		FormatEntrryPrefix(isRecursive) + STR_FILEDETAILVIEW_COLUMN_ENTRY_ORDER, 
		columnsOrder);
}

/*!
	Load column state
	\param isRecursive Recursive state
	\return true on success, false otherwise
*/
bool CBrowseFileView::LoadColumns(bool isRecursive)
{
	int& sort = isRecursive ? m_sortFlat : m_sortRegular;
	bool& ascendant = isRecursive ? m_ascendantFlat : m_ascendantRegular;

	sort = AfxGetApp()->GetProfileInt(STR_FILEDETAILVIEW_COLUMN_SECTION,
		FormatEntrryPrefix(isRecursive) + STR_FILEDETAILVIEW_COLUMN_ENTRY_SORT, EntnodeData::kName);

	ascendant = AfxGetApp()->GetProfileInt(STR_FILEDETAILVIEW_COLUMN_SECTION,
		FormatEntrryPrefix(isRecursive) + STR_FILEDETAILVIEW_COLUMN_ENTRY_SORTASC, 0) ? true : false;
	
	CString columnsWidth = AfxGetApp()->GetProfileString(STR_FILEDETAILVIEW_COLUMN_SECTION, 
		FormatEntrryPrefix(isRecursive) + STR_FILEDETAILVIEW_COLUMN_ENTRY_WIDTH);

	CString columnsOrder = AfxGetApp()->GetProfileString(STR_FILEDETAILVIEW_COLUMN_SECTION, 
		FormatEntrryPrefix(isRecursive) + STR_FILEDETAILVIEW_COLUMN_ENTRY_ORDER);

	if( columnsWidth.IsEmpty() || columnsOrder.IsEmpty() )
	{
		return false;
	}

	const int columnCount = m_ctrlHeader.GetItemCount();

	int* orderArray = new int[columnCount];
	if( orderArray )
	{
		CString strSubString;

		for(int nIndex = 0; nIndex < columnCount; nIndex++)
		{
			AfxExtractSubString(strSubString, columnsWidth, nIndex, CHR_FILEDETAILVIEW_COLUMN_DATASEP);
			GetListCtrl().SetColumnWidth(nIndex, atoi(strSubString));
			
			AfxExtractSubString(strSubString, columnsOrder, nIndex, CHR_FILEDETAILVIEW_COLUMN_DATASEP);
			orderArray[nIndex] = atoi(strSubString);
		}
		
		m_ctrlHeader.SetOrderArray(columnCount, orderArray);
		
		delete [] orderArray;
	}
	
	return true;
}

/// WM_SETCURSOR message handler, set the hourglass cursor when cvs command is running
BOOL CBrowseFileView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() )
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		return TRUE;
	}
	
	return CListView::OnSetCursor(pWnd, nHitTest, message);
}

/*!
	Notify the view that app's filtering has changed
*/
void CBrowseFileView::OnFilteringChanged()
{
	if( !m_isResetViewPending )
	{
		m_isResetViewPending = true;
		PostMessage(WM_RESETVIEW);
	}    
}

/*!
	Notify the view that app's filtering has changed
*/
void CBrowseFileView::OnFilterBarChanged()
{
	if( !m_isResetViewPending )
	{
		m_isResetViewPending = true;
		
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if( pMainFrame->GetSafeHwnd() )
		{
			pMainFrame->SetFilterBarMask(m_ctrlHeader, GetColumnModel());
		}

		PostMessage(WM_RESETVIEW);
	}    
}

/// LVN_DELETEITEM notification handler
void CBrowseFileView::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* p = (NM_LISTVIEW*)pNMHDR;
	if( p->lParam ) 
	{
		EntnodeData* data = (EntnodeData*)(LPVOID)p->lParam;
		data->UnRef();
	}

	*pResult = 0;
}

/// WM_RESETVIEW message handler, reset view
LRESULT CBrowseFileView::OnResetView(WPARAM, LPARAM)
{
	ResetView(false, NULL);
	return 0;
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewAnnotate(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	CListCtrl& ListCtrl = GetListCtrl();
	
	int nItem = -1;
	BOOL res = TRUE;
	
	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->IsUnknown() || data->GetType() == ENT_SUBDIR )
		{
			res = FALSE;
			break;
		}
	}
	
	pCmdUI->Enable(res);
}

/// WM_COMMAND message handler, <b>cvs annotate</b> selection
void CBrowseFileView::OnAnnotate() 
{
	KoAnnotateHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, select all file items
void CBrowseFileView::OnEditSelectAll() 
{
	CListCtrl& ListCtrl = GetListCtrl();
	
	for( int nIndex = 0; nIndex < ListCtrl.GetItemCount(); nIndex++ )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nIndex);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->GetType() == ENT_FILE )
		{
			ListCtrl.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
	
	if( ListCtrl.GetSelectedCount() )
	{
		ListCtrl.SetFocus();
	}
}

/// WM_COMMAND message handler, display command line dialog
void CBrowseFileView::OnAppCmdline() 
{
	KoCommandLineHandler handler;

	if( !ProcessSelectionCommand(&handler) )
	{
		handler.OnFolder(m_path.c_str());
	}
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateCvsCmd(CCmdUI* pCmdUI) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	pCmdUI->Enable(app->IsCvsRunning() ? FALSE : TRUE);
}

/// WM_COMMAND message handler, <b>cvs rtag -b</b> selection
void CBrowseFileView::OnAppRtagbranch() 
{
	KoRtagBranchHandler handler;

	if( !ProcessSelectionCommand(&handler) )
	{
		handler.OnFolder(m_path.c_str());
	}
}

/// WM_COMMAND message handler, <b>cvs rtag</b> selection
void CBrowseFileView::OnAppRtagcreate() 
{
	KoRtagCreateHandler handler;

	if( !ProcessSelectionCommand(&handler) )
	{
		handler.OnFolder(m_path.c_str());
	}
}

/// WM_COMMAND message handler, <b>cvs rtag -d</b> selection
void CBrowseFileView::OnAppRtagdelete() 
{
	KoRtagDeleteHandler handler;

	if( !ProcessSelectionCommand(&handler) )
	{
		handler.OnFolder(m_path.c_str());
	}
}

/// WM_COMMAND message handler, enable/disable filter bar
void CBrowseFileView::OnViewFilterbarEnable() 
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if( pMainFrame->GetSafeHwnd() )
	{
		pMainFrame->ToggleFilterMasksEnable();
		m_ctrlHeader.EnableHeaderFilters(pMainFrame->IsFilterMasksEnable());
		m_ctrlHeader.SetFilterChangeTimeout(gFileViewFilterMasksReactionSpeed);
	}
	
	PostMessage(WM_RESETVIEW);
}

/// WM_COMMAND message handler, clear all filter bar filters
void CBrowseFileView::OnViewFilterbarClearall() 
{
	if( m_ctrlHeader.GetSafeHwnd() )
	{
		Header_ClearAllFilters(m_ctrlHeader.GetSafeHwnd());
	}
}

/// WM_COMMAND message handler, clear all filter bar filters
void CBrowseFileView::OnViewFilterbarOptions() 
{
	int reactionSpeed = gFileViewFilterMasksReactionSpeed;

	if( CompatGetFilterMaskOptions(reactionSpeed) )
	{
		gFileViewFilterMasksReactionSpeed = reactionSpeed;
		m_ctrlHeader.SetFilterChangeTimeout(gFileViewFilterMasksReactionSpeed);
	}
}

/// LVN_ITEMCHANGED notification handler, report selected items
void CBrowseFileView::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if( pNMListView->uChanged & LVIF_STATE && 
		(pNMListView->uNewState & LVIS_SELECTED || pNMListView->uOldState & LVIS_SELECTED) )
	{
		stringstream str;
		
		const UINT cnt = GetListCtrl().GetSelectedCount();
		if( cnt > 0 )
		{
			str << (int)cnt << (cnt == 1 ? " object " : " objects") << " selected" << ends;
		}

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if( pMainFrame )
		{
			pMainFrame->GetStatusBar()->SetWindowText(str.str().c_str());
		}
	}
	
	*pResult = 0;
}

/// LVN_DELETEALLITEMS notification handler, reset status bar text
void CBrowseFileView::OnDeleteallitems(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if( pMainFrame )
	{
		CString strIdleString;
		strIdleString.LoadString(AFX_IDS_IDLEMESSAGE);

		pMainFrame->GetStatusBar()->SetWindowText(strIdleString);
	}

	*pResult = 0;
}

/// WM_SETTINGCHANGE message handler, trigger refresh to update the view according to the new settings
void CBrowseFileView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CListView::OnSettingChange(uFlags, lpszSection);
	
	PostMessage(WM_RESETVIEW);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateQueryCompare(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	CListCtrl& ListCtrl = GetListCtrl();
	
	int nItem = -1;
	BOOL res = TRUE;
	int numFiles = 0;
	int numFolders = 0;
	
	if( ListCtrl.GetSelectedCount() == 2 )
	{
		while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			if( data->IsMissing() )
			{
				res = FALSE;
				break;
			}
			
			if( data->GetType() == ENT_FILE )
				numFiles++;
			else
				numFolders++;
			
		}

		if( numFiles < 2 && numFolders < 2 )
		{
			res = FALSE;
		}
	}
	else
	{
		res = FALSE;
	}
	
	pCmdUI->Enable(res);
}

/// WM_COMMAND message handler, <b>compare</b> selection
void CBrowseFileView::OnQueryCompare() 
{
	CListCtrl& listCtrl = GetListCtrl();
	
	if( listCtrl.GetSelectedCount() == 2 )
	{
		string path1;
		string path2;

		int nItem = -1;
		while( (nItem = listCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)listCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}

			if( data->IsMissing() )
			{
				continue;
			}

			if( path1.empty() )
			{
				data->GetFullPathName(path1);
			}
			else if( path2.empty() )
			{
				data->GetFullPathName(path2);
			}
			else
			{
				break;
			}
		}

		LaunchDiff(path1.c_str(), path2.c_str());
	}
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewEditselconflict(CCmdUI* pCmdUI) 
{
	if( DisableCommon() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	CListCtrl& ListCtrl = GetListCtrl();
	int nItem = -1;
	BOOL res = TRUE;
	
	while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
	{
		EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
		if( EntnodeData::IsUpOneNode(data) )
		{
			continue;
		}

		if( data->GetType() == ENT_SUBDIR || 
			data->IsMissing() || 
			IsNullOrEmpty(data->GetConflict()) )
		{
			res = FALSE;
			break;
		}
	}
	
	pCmdUI->Enable(res);
}

/// WM_COMMAND message handler, launch conflict editor
void CBrowseFileView::OnViewEditselconflict() 
{
	KoLaunchConflictEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewOpenselannotate(CCmdUI* pCmdUI) 
{
	bool enabled = !DisableCommon();
	if( enabled )
	{
		KoUpdateMoveToTrashHandler handler;
		ProcessSelectionCommand(&handler);
		enabled = handler.IsEnabled();
	}
	
	pCmdUI->Enable(enabled);
}

/// WM_COMMAND message handler, launch annotate viewer
void CBrowseFileView::OnViewOpenselannotate() 
{
	KoLaunchConflictEditHandler handler;
	ProcessSelectionCommand(&handler);
}

/// WM_COMMAND message handler, <b>cvs export</b>
void CBrowseFileView::OnViewExport() 
{
	KoExportHandler handler;
	
	if( !ProcessSelectionCommand(&handler) )
	{
		handler.OnFolder(m_path.c_str());
	}
}

/// UPDATE_COMMAND_UI handler
void CBrowseFileView::OnUpdateViewExport(CCmdUI* pCmdUI) 
{
	if( DisableCommon(false) )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	pCmdUI->Enable(TRUE);
}

/// WM_COMMAND message handler, select font
void CBrowseFileView::OnFormatFont() 
{
	LOGFONT lf = gFileViewFont;
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
	
	gFileViewFont = *dlg.m_cf.lpLogFont;
	
	if( m_font )
		m_font->DeleteObject();
	else
		m_font = new CFont();
	
	m_font->CreateFontIndirect(dlg.m_cf.lpLogFont);
	
	SetFont(m_font);
}

/// WM_COMMAND message handler, open the Command Prompt
void CBrowseFileView::OnQueryCommandprompt() 
{
	CListCtrl& ListCtrl = GetListCtrl();
	
	if( ListCtrl.GetSelectedCount() )
	{
		int nItem = -1;
		
		while( (nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED)) != -1 )
		{
			EntnodeData* data = (EntnodeData*)ListCtrl.GetItemData(nItem);
			if( EntnodeData::IsUpOneNode(data) )
			{
				continue;
			}
			
			if( data->IsMissing() )
			{
				continue;
			}
			
			string path;
			data->GetFullPathName(path);
			TrimRight(path, kPathDelimiter);
			
			LaunchCommandPrompt(path.c_str());
		}
	}
	else
	{
		LaunchCommandPrompt(m_path.c_str());
	}
}
