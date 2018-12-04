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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 2000
 */

// CustomizeMenus.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "CustomizeMenus.h"
#include "MMenuString.h"
#include "CvsPrefs.h"
#include "PythonGlue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/// Helper macro to stringize menu ID's
#define ID2STRING(res_id) res_id == res_id ? #res_id : #res_id

/// Helper macro to construct custom menu table
#define INVTAB(msg, e) {msg, #e, e}

/// Browser default menu
char* sDefBrowserMenu[] = 
{
	ID2STRING(ID_VIEW_UPDATE),
	ID2STRING(ID_VIEW_COMMIT),
	ID2STRING(ID_SEPARATOR),
	ID2STRING(ID_VIEW_EXPLORE),
	ID2STRING(ID_VIEW_NEWINSTANCE),
	ID2STRING(ID_VIEW_QUERYUPDATE),
	ID2STRING(ID_VIEW_RELOAD),
	ID2STRING(ID_SEPARATOR),
	ID2STRING(ID_VIEW_IMPORT),
	ID2STRING(ID_VIEW_CHECKOUT),
	0L
};

/// Files default menu
char* sDefFilesMenu[] = 
{
	ID2STRING(ID_VIEW_UPDATE),
	ID2STRING(ID_VIEW_COMMIT),
	ID2STRING(ID_SEPARATOR),
	ID2STRING(ID_VIEW_EXPLORE),
	ID2STRING(ID_VIEW_NEWINSTANCE),
	ID2STRING(ID_VIEW_QUERYUPDATE),
	ID2STRING(ID_VIEW_RELOAD),
	ID2STRING(ID_SEPARATOR),
	ID2STRING(ID_VIEW_EDITSEL),
	ID2STRING(ID_VIEW_EDITSELDEF),
	ID2STRING(ID_VIEW_OPENSEL),
	ID2STRING(ID_VIEW_OPENSELAS),
	ID2STRING(ID_SEPARATOR),
	ID2STRING(ID_VIEW_DIFF),
	ID2STRING(ID_VIEW_LOG),
	ID2STRING(ID_VIEW_STATUS),
	ID2STRING(ID_VIEW_GRAPH),
	ID2STRING(ID_QUERY_ANNOTATE),
	0L
};

static CMMenuString gFilesMenu(99, "P_FilesMenu", sDefFilesMenu);
static CMMenuString gBrowserMenu(99, "P_BrowserMenu", sDefBrowserMenu);

/// Custom menu items table
static CustInvertTable sInitInvTable[] =
{
	INVTAB("Separator",						ID_SEPARATOR),
	INVTAB("Pr&eferences...\tCtrl+F1",		ID_APP_PREFERENCES),
	INVTAB("&Command Line...",				ID_APP_CMDLINE),
	INVTAB("&Login...",						ID_APP_LOGIN),
	INVTAB("Lo&gout",						ID_APP_LOGOUT),
	INVTAB("&Stop cvs",						ID_STOPCVS),
	INVTAB("&Create a new repository...",	ID_VIEW_INIT),
	INVTAB("&Import module...",				ID_VIEW_IMPORT),
	INVTAB("Chec&kout module...",			ID_VIEW_CHECKOUT),
	INVTAB("Create a &tag by module...",	ID_APP_RTAGCREATE),
	INVTAB("&Delete a tag by module...",	ID_APP_RTAGDELETE),
	INVTAB("Create a &branch by module...",	ID_APP_RTAGBRANCH),
	INVTAB("&Update...\tCtrl+U",			ID_VIEW_UPDATE),
	INVTAB("Co&mmit...\tCtrl+M",			ID_VIEW_COMMIT),
	INVTAB("&Add",							ID_VIEW_ADD),
	INVTAB("Add &binary",					ID_VIEW_ADDB),
	INVTAB("Add &unicode",					ID_VIEW_ADDU),
	INVTAB("&Remove",						ID_VIEW_RMV),
	INVTAB("&Erase\tCtrl+Backspace",		ID_VIEW_TRASH),
	INVTAB("Create a &tag...",				ID_VIEW_TAGNEW),
	INVTAB("&Delete a tag...",				ID_VIEW_TAGDELETE),
	INVTAB("Create a &branch...",			ID_VIEW_TAGBRANCH),
	INVTAB("E&xplore\tF2",					ID_VIEW_EXPLORE),
	INVTAB("Open in new instance\tCtrl+F2", ID_VIEW_NEWINSTANCE),
	INVTAB("P&roperties\tAlt+Enter",		ID_QUERY_PROPERTIES),
	INVTAB("&Query Update\tCtrl+Q",			ID_VIEW_QUERYUPDATE),
	INVTAB("R&eload view\tF5",				ID_VIEW_RELOAD),
	INVTAB("&Shell-edit\tEnter",			ID_VIEW_EDITSEL),
	INVTAB("E&dit with editor\tShift+F4",	ID_VIEW_EDITSELDEF),
	INVTAB("&Open\tF7",						ID_VIEW_OPENSEL),
	INVTAB("Open &with...\tCtrl+F7",		ID_VIEW_OPENSELAS),
	INVTAB("&Shell default\tShift+F3",		ID_QUERY_SHELLACTION_DEFAULT),
	INVTAB("Edit with &conflict editor",	ID_VIEW_EDITSELCONFLICT),
	INVTAB("Open with &annotate viewer",	ID_VIEW_OPENSELANNOTATE),
	INVTAB("&Diff\tCtrl+D",					ID_VIEW_DIFF),
	INVTAB("&Compare",						ID_QUERY_COMPARE),
	INVTAB("&Log...",						ID_VIEW_LOG),
	INVTAB("&Status",						ID_VIEW_STATUS),
	INVTAB("&Graph...\tCtrl+G",				ID_VIEW_GRAPH),
	INVTAB("&Annotate...",					ID_QUERY_ANNOTATE),
	INVTAB("&Unlock",						ID_VIEW_UNLOCKF),
	INVTAB("&Edit",							ID_VIEW_EDIT),
	INVTAB("Reserve&d edit",				ID_VIEW_RESERVEDEDIT),
	INVTAB("&Force edit",					ID_VIEW_FORCEEDIT),
	INVTAB("&Unedit",						ID_VIEW_UNEDIT),
	INVTAB("&Add a Watch",					ID_VIEW_WATCHON),
	INVTAB("&Remove a watch",				ID_VIEW_WATCHOFF),
	INVTAB("&Release",						ID_VIEW_RELEASE),
	INVTAB("&Watchers",						ID_VIEW_WATCHERS),
	INVTAB("E&ditors",						ID_VIEW_EDITORS),
	{0L, 0L, 0}
};

/// Custom menu object
class CCustInvertTable : public std::vector<CustInvertTable>
{
public:
	// Construction
	CCustInvertTable();
	virtual ~CCustInvertTable();
	
	// Interface
	void Initialize();
	void FillCustInvertTableFromMenu(const CMenu &menu, const char* lpstrParentText);
	void Clear();
	
protected:
	// Data members
	std::vector<char*> m_garbage;
} sInvTable;

/*!
	Get custom menu item for a given key
	\param entry Entry key
	\return Custom menu item, NULL if not found
*/
const CustInvertTable* CustGetByKey(const char* entry)
{
	std::vector<CustInvertTable>::const_iterator tmp;
	sInvTable.Initialize();

	for(tmp = sInvTable.begin(); tmp != sInvTable.end(); ++tmp)
	{
		if( strcmp(entry, (*tmp).key) == 0 )
			return &*tmp;
	}

	return NULL;
}

/// Return the custom menu
CMenu* GetCustomMenu(int whichmenu, CView* pThis)
{
	CMMenuString* multiMenu;
	
	switch(whichmenu)
	{
	case kCustomBrowserMenu:
		multiMenu = &gBrowserMenu;
		break;
	case kCustomFilesMenu:
		multiMenu = &gFilesMenu;
		break;
	default:
		ASSERT(0);
		return 0L;
	}
	
	CMenu* res = new CMenu;
	VERIFY(res->CreatePopupMenu());
	
	CMMenuString::list_type& list = multiMenu->GetList();
	CMMenuString::list_type::iterator i = list.begin();
	
	while( i != list.end() )
	{
		const CustInvertTable* tab = CustGetByKey(i->c_str());
		if( tab == 0L )
		{
			TRACE1("Removing old command(%s)\n", i->c_str());
			
			list.erase(i);
			i = list.begin();
			continue;
		}
		
		i++;
	}
	
	for(i = list.begin(); i != list.end(); ++i)
	{
		const CustInvertTable* tab = CustGetByKey(i->c_str());
		if( tab )
		{
			if( tab->cmd == ID_SEPARATOR )
			{
				res->AppendMenu(MF_SEPARATOR);
			}
			else
			{
				res->AppendMenu(MF_STRING, tab->cmd, tab->title);
			}
		}
	}
	
	res->AppendMenu(MF_SEPARATOR);
	res->AppendMenu(MF_STRING, whichmenu == kCustomFilesMenu ? ID_CUST_FILES_MENU : ID_CUST_FOLDER_MENU, CString("Customize this menu..."));
	
	UpdateUIMenu(res, pThis);
	
	return res;
}

/// Ask to customize the menus
void CustomizeMenus(int whichmenu)
{
	CMMenuString *multiMenu;
	
	switch( whichmenu )
	{
	case kCustomBrowserMenu:
		multiMenu = &gBrowserMenu;
		break;
	case kCustomFilesMenu:
		multiMenu = &gFilesMenu;
		break;
	default:
		ASSERT(FALSE);
		return;
	}
	
	CCustomizeMenus custom(multiMenu);
	if( custom.DoModal() == IDOK )
	{
		gCvsPrefs.save();
	}
}

/*!
	Get custom menu item string
	\param entry Custom menu item
	\return Custom menu item string
*/
static char* GetInvString(const CustInvertTable* entry)
{
	static CString res;
	res = entry->title;
	
	int ind;
	if( (ind = res.Find('&')) != -1 )
		res.Delete(ind);
	
	if( (ind = res.Find('\t')) != -1 )
		res = res.Left(ind);
	
	return (char*)(const char*)res;
}

/*!
	Check whether the menu item entry is in the list
	\param entry Entry
	\param list List
	\return true if menu item found, false otherwise
*/
static bool IsInInvertList(const CustInvertTable* entry, const CMMenuString::list_type& list)
{
	for(CMMenuString::list_type::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		const CustInvertTable* tab = CustGetByKey(i->c_str());
		if( tab == 0L )
			continue;
		
		if( strcmp(entry->key, tab->key) == 0 )
			return true;
	}
	
	return false;
}

/// Compare callback function
static int CALLBACK CustMenuCompare(LPARAM data1, LPARAM data2, LPARAM data)
{
	CustInvertTable* d1 = (CustInvertTable*)data1;
	CustInvertTable* d2 = (CustInvertTable*)data2;
	
	CString s1(GetInvString(d1));
	CString s2(GetInvString(d2));
	
	int cmp = strcmp(s1, s2);
	if( cmp == 0 )
		return 0;
	
	if( d1->cmd == ID_SEPARATOR )
		return -1;

	if( d2->cmd == ID_SEPARATOR )
		return 1;

	return cmp;
}

CCustInvertTable::CCustInvertTable()
{
	// Init the cmd table with the static MFC commands
	const CustInvertTable* tmp = sInitInvTable;
	CustInvertTable newEntry;
	
	while( tmp->key != 0L )
	{
		newEntry = *tmp;
		push_back(newEntry);
		tmp++;
	}
}

CCustInvertTable::~CCustInvertTable()
{
	Clear();
}

/*!
	Clear the collection and release the memory
*/
void CCustInvertTable::Clear()
{
	std::vector<char*>::iterator i;
	for(i = m_garbage.begin(); i != m_garbage.end(); ++i)
	{
		free(*i);
	}

	m_garbage.clear();
}

/*!
	Get the macro inside the persistent cmd table
*/
void CCustInvertTable::Initialize()
{
	if( m_garbage.size() != 0 )
		return;

	for(int nIndex = 0; nIndex < kMacroTypeEnd; nIndex++)
	{
		if( HMENU hMenu = (HMENU)GetTopLevelMacroMenu((kMacroType)nIndex, false) )
		{
			CMenu parent;
			if( parent.Attach(hMenu) )
			{
				FillCustInvertTableFromMenu(parent, "");
				parent.Detach();
			}
		}
	}
}

/*!
	Fill table from menu
	\param menu Menu to fill the table from
	\param lpstrParentText Parent menu text
	\note "Tcl" parent text is used to detect TCL macros
*/
void CCustInvertTable::FillCustInvertTableFromMenu(const CMenu& menu, const char* lpstrParentText)
{
	for(UINT nIndex = 0; nIndex < menu.GetMenuItemCount(); nIndex++)
	{
		const int id = menu.GetMenuItemID(nIndex);
		if( id > 0 )
		{
			CString strText;
			menu.GetMenuString(nIndex, strText, MF_BYPOSITION);

			char* name = strdup(strText);
			m_garbage.push_back(name);

			CustInvertTable newEntry;
			newEntry.cmd = id;
			newEntry.key = name;
			newEntry.title = name;
			newEntry.isTcl = strcmp("Tcl", lpstrParentText) == 0;

			push_back(newEntry);
		}
		else if( id == -1 )
		{
			CString strParentText;
			menu.GetMenuString(nIndex, strParentText, MF_BYPOSITION);
			
			if( CMenu* subMenu = menu.GetSubMenu(nIndex) )
			{
				FillCustInvertTableFromMenu(*subMenu, strParentText);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCustomizeMenus dialog


CCustomizeMenus::CCustomizeMenus(CMMenuString* fromMenu, CWnd* pParent /*=NULL*/)
	: CHHelpDialog(CCustomizeMenus::IDD, pParent), m_Menu(fromMenu)
{
	//{{AFX_DATA_INIT(CCustomizeMenus)
	//}}AFX_DATA_INIT
}


void CCustomizeMenus::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomizeMenus)
	DDX_Control(pDX, IDC_MENULIST, m_menuList);
	DDX_Control(pDX, IDC_ALLCMDS, m_cmdsList);
	DDX_Control(pDX, IDC_UP, m_upbtn);
	DDX_Control(pDX, IDC_REMOVE, m_removebtn);
	DDX_Control(pDX, IDC_DOWN, m_downbtn);
	DDX_Control(pDX, IDC_ADD, m_addbtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomizeMenus, CHHelpDialog)
	//{{AFX_MSG_MAP(CCustomizeMenus)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ALLCMDS, OnItemchangedAllcmds)
	ON_NOTIFY(NM_DBLCLK, IDC_MENULIST, OnDblclkMenulist)
	ON_NOTIFY(NM_DBLCLK, IDC_ALLCMDS, OnDblclkAllcmds)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MENULIST, OnItemchangedMenulist)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomizeMenus message handlers

/// BN_CLICKED message handler, remove item
void CCustomizeMenus::OnRemove() 
{
	const int nItem = m_menuList.GetNextItem(-1, LVNI_SELECTED);
	if( nItem == -1 )
		return;

	const CustInvertTable* data = (const CustInvertTable*)m_menuList.GetItemData(nItem);

	if( data->cmd != ID_SEPARATOR )
	{
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvi.iItem = m_cmdsList.GetItemCount();
		lvi.iSubItem = 0;
		lvi.lParam = (DWORD)data;
		lvi.pszText = GetInvString(data);
		lvi.iImage = GetIcon(data);

		const int newItem = m_cmdsList.InsertItem(&lvi);
		if( newItem > -1 )
		{
			m_cmdsList.EnsureVisible(newItem, TRUE);
			m_cmdsList.SetItemState(newItem, LVIS_SELECTED, LVIS_SELECTED);
		}

		m_cmdsList.SortItems(CustMenuCompare, 0L);
	}

	m_menuList.DeleteItem(nItem);
}

/// BN_CLICKED message handler, add item
void CCustomizeMenus::OnAdd() 
{
	const int nItem = m_cmdsList.GetNextItem(-1, LVNI_SELECTED);
	if( nItem == -1 )
		return;

	const CustInvertTable* data = (const CustInvertTable*)m_cmdsList.GetItemData(nItem);

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvi.iItem = m_menuList.GetItemCount();
	lvi.iSubItem = 0;
	lvi.lParam = (DWORD)data;
	lvi.pszText = GetInvString(data);
	lvi.iImage = GetIcon(data);

	const int newItem = m_menuList.InsertItem(&lvi);
	if( newItem > -1 )
	{
		m_menuList.EnsureVisible(newItem, TRUE);
		m_menuList.SetItemState(newItem, LVIS_SELECTED, LVIS_SELECTED);
	}

	if( data->cmd != ID_SEPARATOR )
	{
		m_cmdsList.DeleteItem(nItem);
	}
}

/// OnOK virtual override, store the selected commands list
void CCustomizeMenus::OnOK()
{
	CMMenuString::list_type& list = m_Menu->GetList();
	list.erase(list.begin(), list.end());

	int nItem = -1;
	while( (nItem = m_menuList.GetNextItem(nItem, LVNI_ALL)) != -1 )
	{
		const CustInvertTable* data = (const CustInvertTable*)m_menuList.GetItemData(nItem);
		list.push_back(data->key);
	}
	
	CHHelpDialog::OnOK();
}

/// OnInitDialog virtual override, initialize controls
BOOL CCustomizeMenus::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	sInvTable.Initialize();
	
	// Initialize the buttons
	m_upbtn.EnableWindow(FALSE);
	m_downbtn.EnableWindow(FALSE);
	m_addbtn.EnableWindow(FALSE);
	m_removebtn.EnableWindow(FALSE);
	
	SetListCtrlExtendedStyle(&m_cmdsList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
	SetListCtrlExtendedStyle(&m_menuList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);

	m_SmallImageList.Create(IDR_CUSTMENUS, 16, 1, RGB(255, 0, 255));
	m_cmdsList.SetImageList(&m_SmallImageList, LVSIL_SMALL);
	m_menuList.SetImageList(&m_SmallImageList, LVSIL_SMALL);

	int cnt;
	const CMMenuString::list_type& list = m_Menu->GetList();
	
	// Get the client rect of the list view control
	CRect rect;
	m_cmdsList.GetClientRect(&rect);

	// Make the width of the column to the width of the client area minus the width of a scroll bar
	int nWidth = rect.right - ::GetSystemMetrics(SM_CXVSCROLL);

	// First the left list
	m_cmdsList.InsertColumn(0, "Available commands", LVCFMT_LEFT, nWidth);

	std::vector<CustInvertTable>::const_iterator table;
	for(cnt = 0, table = sInvTable.begin(); table != sInvTable.end(); ++table, cnt++)
	{
		const CustInvertTable& tablEntry = *table;
		if( table->cmd != ID_SEPARATOR && IsInInvertList(&tablEntry, list) )
			continue;

		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvi.iItem = cnt;
		lvi.iSubItem = 0;
		lvi.lParam = (DWORD)&tablEntry;
		lvi.pszText = GetInvString(&tablEntry);
		lvi.iImage = GetIcon(&tablEntry);

		m_cmdsList.InsertItem(&lvi);
	}

	m_cmdsList.SortItems(CustMenuCompare, 0L);

	// Then the right one
	m_menuList.InsertColumn(0, "Selected commands", LVCFMT_LEFT, nWidth);

	CMMenuString::list_type::const_iterator i;

	for(cnt = 0, i = list.begin(); i != list.end(); ++i, cnt++)
	{
		const CustInvertTable* tab = CustGetByKey(i->c_str());
		if( tab == NULL )
			continue;

		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvi.iItem = cnt;
		lvi.iSubItem = 0;
		lvi.lParam = (DWORD)tab;
		lvi.pszText = GetInvString(tab);
		lvi.iImage = GetIcon(tab);

		m_menuList.InsertItem(&lvi);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomizeMenus::InitHelpID()
{
	SetCtrlHelpID(IDC_ALLCMDS,	IDH_IDC_ALLCMDS);
	SetCtrlHelpID(IDC_MENULIST,	IDH_IDC_MENULIST);
	SetCtrlHelpID(IDC_ADD,		IDH_IDC_ADD);
	SetCtrlHelpID(IDC_REMOVE,	IDH_IDC_REMOVE);
	SetCtrlHelpID(IDC_UP,		IDH_IDC_UP);
	SetCtrlHelpID(IDC_DOWN,		IDH_IDC_DOWN);
}

/// LVN_ITEMCHANGED notification handler, enable buttons
void CCustomizeMenus::OnItemchangedAllcmds(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;

	m_addbtn.EnableWindow(m_cmdsList.GetNextItem(-1, LVNI_SELECTED) != -1);
}

/// NM_DBLCLK notification handler, remove item
void CCustomizeMenus::OnDblclkMenulist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnRemove();
	
	*pResult = 0;
}

/// NM_DBLCLK notification handler, add item
void CCustomizeMenus::OnDblclkAllcmds(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAdd();
	
	*pResult = 0;
}

/// LVN_ITEMCHANGED notification handler, enable buttons
void CCustomizeMenus::OnItemchangedMenulist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;

	const int nItem = m_menuList.GetNextItem(-1, LVNI_SELECTED);
	
	m_removebtn.EnableWindow(nItem != -1);

	if( nItem == -1 )
	{
		m_upbtn.EnableWindow(FALSE);
		m_downbtn.EnableWindow(FALSE);
	}
	else if( nItem == 0 )
	{
		m_upbtn.EnableWindow(FALSE);
		m_downbtn.EnableWindow(m_menuList.GetItemCount() > 1 ? TRUE : FALSE);
	}
	else if( nItem == m_menuList.GetItemCount() - 1 )
	{
		m_upbtn.EnableWindow(TRUE);
		m_downbtn.EnableWindow(FALSE);
	}
	else
	{
		m_upbtn.EnableWindow(TRUE);
		m_downbtn.EnableWindow(TRUE);
	}
}

/*!
	Move item
	\param offset Offset to move item
*/
void CCustomizeMenus::OnMove(int offset)
{
	const int nItem = m_menuList.GetNextItem(-1, LVNI_SELECTED);
	if( nItem == -1 )
		return;

	const CustInvertTable* data = (const CustInvertTable*)m_menuList.GetItemData(nItem);

	const int newPos = nItem + offset;
	if( newPos < 0 || newPos > m_menuList.GetItemCount() )
		return;

	VERIFY(m_menuList.DeleteItem(nItem));

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvi.iItem = newPos;
	lvi.iSubItem = 0;
	lvi.lParam = (DWORD)data;
	lvi.pszText = GetInvString(data);
	lvi.iImage = GetIcon(data);

	const int newItem = m_menuList.InsertItem(&lvi);
	if( newItem > -1 )
	{
		m_menuList.EnsureVisible(newItem, TRUE);
		m_menuList.SetItemState(newItem, LVIS_SELECTED, LVIS_SELECTED);
	}
}

/// BN_CLICKED message handler, move item down
void CCustomizeMenus::OnDown() 
{
	OnMove(1);
}

/// BN_CLICKED message handler, move item up
void CCustomizeMenus::OnUp() 
{
	OnMove(-1);
}

/*!
	Get the icon for the menu item entry
	\param entry Menu item entry
	\return Icon for the menu item entry
*/
int CCustomizeMenus::GetIcon(const CustInvertTable* entry)
{
	if( entry->cmd == ID_SEPARATOR )
		return 0;

	if( entry->cmd >= ID_MACRO_START && entry->cmd <= ID_MACRO_END )
		return entry->isTcl ? 2 : 3;

	return 1;
}

/// BN_CLICKED message handler, display dialog help
void CCustomizeMenus::OnHelp() 
{
	ShowHelp();
}
