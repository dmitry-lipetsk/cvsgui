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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- January 2003
 */

// SelectionDetailsDlg.cpp : implementation file
//

#include "stdafx.h"

#if qUnix
#	include "UCvsDialogs.h"
#endif

#ifdef WIN32
#	include "wincvs.h"
#endif /* !WIN32 */

#include "SelectionDetailsDlg.h"
#include "CvsEntries.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef WIN32

//////////////////////////////////////////////////////////////////////////
// SelectionDisplayInfo

/*!
	Create display info
	\param fileName Filename
	\param revision Revision
	\param path Path
*/
void SelectionDisplayInfo::Create(const std::string& fileName, const std::string& revision, const std::string& path)
{
	m_fileName = fileName;
	m_revision = revision;
	m_path = path;
}

/// Get filename
const std::string& SelectionDisplayInfo::GetFileName() const
{
	return m_fileName;
}

/// Get revision
const std::string& SelectionDisplayInfo::GetRevision() const
{
	return m_revision;
}

/// Get path
const std::string& SelectionDisplayInfo::GetPath() const
{
	return m_path;
}


/*!
	Callback method used for sorting of items in the list control
	\param itemData1 SelectionDisplayInfo* of the first element
	\param itemData2 SelectionDisplayInfo* of the second element
	\param lParam CompareInfo* Sort settings
	\return 1, -1 or 0 depending on the sort settings
*/
int CALLBACK CSelectionDetailsDlg::CompareItems(LPARAM itemData1, LPARAM itemData2, LPARAM lParam)
{
	int res = 0;

	const CompareInfo* info = (CompareInfo*)lParam;
	if( info )
	{
		SelectionDisplayInfo* displayInfo1 = (SelectionDisplayInfo*)itemData1;
		SelectionDisplayInfo* displayInfo2 = (SelectionDisplayInfo*)itemData2;
		
		if( displayInfo1 && displayInfo2 )
		{
			switch( info->m_column )
			{
			case 0:
				res = stricmp(displayInfo1->GetFileName().c_str(), displayInfo2->GetFileName().c_str());
				break;
			case 1:
				res = stricmp(displayInfo1->GetRevision().c_str(), displayInfo2->GetRevision().c_str());
				break;
			case 2:
				res = stricmp(displayInfo1->GetPath().c_str(), displayInfo2->GetPath().c_str());
				break;
			default:
				ASSERT(FALSE); // should not come here
				break;
			}
		}
	}

	if( res )
	{
		if( info->m_asc )
		{
			res *= -1;
		}
	}

	return res;
}

/////////////////////////////////////////////////////////////////////////////
// CSelectionDetailsDlg dialog

CSelectionDetailsDlg::CSelectionDetailsDlg(const MultiFiles* mf, const kSelDetType selDetType, const char* selection,
										   CWnd* pParent /*=NULL*/)
	: m_mf(mf), CHHelpDialog(CSelectionDetailsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectionDetailsDlg)
	m_selection = selection;
	m_selDetType = selDetType;
	//}}AFX_DATA_INIT

	m_sortAsc = false;
	m_sortColumn = 0;
}


void CSelectionDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectionDetailsDlg)
	DDX_Control(pDX, IDC_REVISION, m_revisionCheck);
	DDX_Control(pDX, IDC_FILENAME, m_fileNameCheck);
	DDX_Control(pDX, IDC_FILEPATH, m_pathCheck);
	DDX_Control(pDX, IDC_SELECTION, m_selectionEdit);
	DDX_Control(pDX, IDC_REVISION_SPIN, m_revisionSpinCtrl);
	DDX_Control(pDX, IDC_SELECTION_LIST, m_selectionListCtrl);
	DDX_Text(pDX, IDC_SELECTION, m_selection);
	DDX_Radio(pDX, IDC_FILENAME, m_selDetType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectionDetailsDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CSelectionDetailsDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_SELECTION_LIST, OnDblclkSelectionlist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_SELECTION_LIST, OnColumnclickSelectionlist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SELECTION_LIST, OnItemchangedSelectionlist)
	ON_NOTIFY(UDN_DELTAPOS, IDC_REVISION_SPIN, OnDeltaposRevisionSpin)
	ON_BN_CLICKED(IDC_FILENAME, OnFilename)
	ON_BN_CLICKED(IDC_FILEPATH, OnPath)
	ON_BN_CLICKED(IDC_REVISION, OnRevision)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectionDetailsDlg message handlers

/// OnInitDialog virtual override, initializes controls and displays data
BOOL CSelectionDetailsDlg::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	// Initialize controls
	SetListCtrlExtendedStyle(&m_selectionListCtrl, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);

	m_revisionSpinCtrl.SetRange(0, 100);

	// Setup controls
	CHeaderCtrl* headerCtrl = m_selectionListCtrl.GetHeaderCtrl();
	if( headerCtrl )
	{
		m_headerCtrl.SubclassWindow(headerCtrl->m_hWnd);
	}

	EnableControls();

	// Display items
	DisplaySelection();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectionDetailsDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_SELECTION,		IDH_IDC_SELECTION);
	SetCtrlHelpID(IDC_REVISION_SPIN,	IDH_IDC_REVISION_SPIN);
	SetCtrlHelpID(IDC_FILENAME,			IDH_IDC_FILENAME);
	SetCtrlHelpID(IDC_REVISION,			IDH_IDC_REVISION);
	SetCtrlHelpID(IDC_FILEPATH,			IDH_IDC_FILEPATH);
	SetCtrlHelpID(IDC_SELECTION_LIST,	IDH_IDC_SELECTION_LIST);
}

/// BN_CLICKED message handler, display dialog help
void CSelectionDetailsDlg::OnHelp() 
{
	ShowHelp();
}

/*!
	Insert columns and display the selection items
*/
void CSelectionDetailsDlg::DisplaySelection()
{
	CWndRedrawMngr redrawMngr(&m_selectionListCtrl);

	// Get the client area width to properly size columns
	CRect rect;
	m_selectionListCtrl.GetClientRect(rect);

	const int listCtrlWidth = rect.Width();
	const int listCtrlWidthNoScroll = listCtrlWidth - GetSystemMetrics(SM_CXVSCROLL);

	// Insert columns
	m_selectionListCtrl.InsertColumn(0, ColumnIndex::GetColumnName(ColumnIndex::kName, false), LVCFMT_LEFT, 200);
	m_selectionListCtrl.InsertColumn(1, ColumnIndex::GetColumnName(ColumnIndex::kRevision, false), LVCFMT_LEFT, 60);
	m_selectionListCtrl.InsertColumn(2, ColumnIndex::GetColumnName(ColumnIndex::kPath, false), LVCFMT_LEFT, listCtrlWidthNoScroll - 60 - 200);

	// The browser selection
	if( m_mf )
	{
		if( m_mf->TotalNumFiles() )
		{
			MultiFiles::const_iterator mfi;
			for(mfi = m_mf->begin(); mfi != m_mf->end(); ++mfi)
			{
				for(int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++)
				{
					string fileName;
					string currRev;
					string path;
					
					if( mfi->get(nIndex, path, fileName, currRev) )
					{
						const int itemIndex = m_selectionListCtrl.InsertItem(m_selectionListCtrl.GetItemCount(), fileName.c_str());
						if( itemIndex > -1 )
						{
							m_selectionListCtrl.SetItemText(itemIndex, 1, currRev.c_str());
							m_selectionListCtrl.SetItemText(itemIndex, 2, path.c_str());

							SelectionDisplayInfo* displayInfo = new SelectionDisplayInfo;
							if( displayInfo )
							{
								displayInfo->Create(string(fileName.c_str()), string(currRev.c_str()), string(path.c_str()));
								if( !m_selectionListCtrl.SetItemData(itemIndex, (DWORD)displayInfo) )
								{
									delete displayInfo;
								}
							}
						}
					}
				}
			}
		}
		else if( m_mf->NumDirs() )
		{
			for(int nIndex = 0; nIndex < m_mf->NumDirs(); nIndex++)
			{
				string path;
				if( m_mf->getdir(nIndex, path) )
				{
					const int itemIndex = m_selectionListCtrl.InsertItem(m_selectionListCtrl.GetItemCount(), "");
					if( itemIndex > -1 )
					{
						m_selectionListCtrl.SetItemText(itemIndex, 2, path.c_str());
						
						SelectionDisplayInfo* displayInfo = new SelectionDisplayInfo;
						if( displayInfo )
						{
							displayInfo->Create(string(""), string(""), string(path.c_str()));
							if( !m_selectionListCtrl.SetItemData(itemIndex, (DWORD)displayInfo) )
							{
								delete displayInfo;
							}
						}
					}
				}
			}
		}
	}
}

/*!
	Remove all items and release associated memory held in Item Data of the list control
*/
void CSelectionDetailsDlg::DeleteAllItems()
{
	for(int nIndex = 0; nIndex < m_selectionListCtrl.GetItemCount(); nIndex++)
	{
		SelectionDisplayInfo* displayInfo = (SelectionDisplayInfo*)m_selectionListCtrl.GetItemData(nIndex);
		delete displayInfo;
	}

	m_selectionListCtrl.DeleteAllItems();
}

/// WM_DESTROY message handler, release any memory allocated
void CSelectionDetailsDlg::OnDestroy() 
{
	DeleteAllItems();

	CHHelpDialog::OnDestroy();
}

/// NM_DBLCLK message handler, select the item and close dialog
void CSelectionDetailsDlg::OnDblclkSelectionlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem > -1 )
	{
		OnOK();
	}
	
	*pResult = 0;
}

/// LVN_COLUMNCLICK message handler, sort items in response to column click
void CSelectionDetailsDlg::OnColumnclickSelectionlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if( m_sortColumn != pNMListView->iSubItem )
	{
		m_sortAsc = false;
	}
	else
	{
		m_sortAsc = !m_sortAsc;
	}

	m_sortColumn = pNMListView->iSubItem;

	SortItems(m_sortColumn, m_sortAsc);

	*pResult = 0;
}

/*!
	Sort items listed in the list control
	\param column Column to sort items by
	\param asc Sort ascending or descending
	\note Using list control callback CompareItems
*/
void CSelectionDetailsDlg::SortItems(int column, bool asc)
{
	m_headerCtrl.SetSortImage(column, asc);

	CompareInfo info = { 0 };
	
	info.m_column = column;
	info.m_asc = asc;

	m_selectionListCtrl.SortItems(CompareItems, (DWORD)&info);
}

/// LVN_ITEMCHANGED message handler, update the item edit box value as selection is changing
void CSelectionDetailsDlg::OnItemchangedSelectionlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem > -1 && 
		pNMListView->uChanged & LVIF_STATE && pNMListView->uNewState & (LVIS_SELECTED | LVIS_FOCUSED) )
	{
		UpdateSelect(pNMListView->iItem);
	}
	
	*pResult = 0;
}

/// UDN_DELTAPOS notification handler, for the revision type move the revision up or down
void CSelectionDetailsDlg::OnDeltaposRevisionSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if( m_revisionCheck.GetCheck() )
	{
		CString selection;
		m_selectionEdit.GetWindowText(selection);

		const int dotPos = selection.ReverseFind('.');
		if( dotPos > 0 && dotPos < selection.GetLength() - 1 )
		{
			CString revLast = selection.Mid(dotPos + 1);
			int revLastNum = atoi(revLast);
			if( revLastNum > 0 )
			{
				revLastNum += pNMUpDown->iDelta;
				if( revLastNum > 0 )
				{
					revLast.Format("%d", revLastNum);
					selection = selection.Left(dotPos + 1) + revLast;
					
					m_selectionEdit.SetWindowText(selection);
				}
			}
		}
	}
	
	// We do not allow to change position
	*pResult = 1;
}

/// BN_CLICKED message handler, update the controls
void CSelectionDetailsDlg::OnFilename() 
{
	EnableControls();
	UpdateSelect(-1);
}

/// BN_CLICKED message handler, update the controls
void CSelectionDetailsDlg::OnPath() 
{
	EnableControls();
	UpdateSelect(-1);
}

/// BN_CLICKED message handler, update the controls
void CSelectionDetailsDlg::OnRevision() 
{
	EnableControls();
	UpdateSelect(-1);
}

/*!
	Enable/disable controls
*/
void CSelectionDetailsDlg::EnableControls()
{
	m_revisionSpinCtrl.EnableWindow(m_revisionCheck.GetCheck());
}

/*!
	Update the select edit box to the data from given list item
	\param item Item to update to, -1 to update to the currently selected item
*/
void CSelectionDetailsDlg::UpdateSelect(int item)
{
	if( item < 0 )
	{
		// Get the currently selected item
		POSITION pos = m_selectionListCtrl.GetFirstSelectedItemPosition();
		if( pos )
		{
			item = m_selectionListCtrl.GetNextSelectedItem(pos);
		}
	}

	if( item > -1 && item < m_selectionListCtrl.GetItemCount() )
	{
		// Update the select edit box
		int subItem = 0;
		
		if( m_revisionCheck.GetCheck() )
		{
			subItem = 1;
		}
		else if( m_pathCheck.GetCheck() )
		{
			subItem = 2;
		}
		
		m_selectionEdit.SetWindowText(m_selectionListCtrl.GetItemText(item, subItem));
	}
}

#endif /* !WIN32 */

#if qUnix
class UCvsSelectionDetailsDlg : public UWidget
{
	UDECLARE_DYNAMIC(UCvsSelectionDetailsDlg)
public:
	UCvsSelectionDetailsDlg(const MultiFiles* mf, const kSelDetType selDetType, const char* selection);
	virtual ~UCvsSelectionDetailsDlg() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
	};

	virtual void DoDataExchange(bool fill);
public:
	std::string 	m_selection;
	kSelDetType	 m_selDetType;

protected:
	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);

	UDECLARE_MESSAGE_MAP()
private:
	const MultiFiles* m_mf;
};

UIMPLEMENT_DYNAMIC(UCvsSelectionDetailsDlg, UWidget)

UBEGIN_MESSAGE_MAP(UCvsSelectionDetailsDlg, UWidget)
	ON_UCOMMAND(UCvsSelectionDetailsDlg::kOK, UCvsSelectionDetailsDlg::OnOK)
	ON_UCOMMAND(UCvsSelectionDetailsDlg::kCancel, UCvsSelectionDetailsDlg::OnCancel)
UEND_MESSAGE_MAP()

UCvsSelectionDetailsDlg::UCvsSelectionDetailsDlg(const MultiFiles* mf, const kSelDetType selDetType, const char* selection)
	: UWidget(::UEventGetWidID()), m_selection( selection), m_selDetType( selDetType), m_mf(mf)
{
}

int UCvsSelectionDetailsDlg::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsSelectionDetailsDlg::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsSelectionDetailsDlg::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

#endif /* qUnix */

/// Get the selection details
bool CompatGetSelectionDetails(const MultiFiles* mf, kSelDetType& selDetType, std::string& selection)
{
	bool userHitOK = false;

#ifdef WIN32
	CSelectionDetailsDlg selectionDetailsDlg(mf, selDetType, selection.c_str());

	if( selectionDetailsDlg.DoModal() == IDOK )
	{
		selection = selectionDetailsDlg.m_selection;
		selDetType = (kSelDetType)selectionDetailsDlg.m_selDetType;

		userHitOK = true;
	}
#endif /* WIN32 */

#if qUnix
	void *wid;
#	if qGTK
	wid = UCreate_SelDetailsDlg();
#	endif

	UCvsSelectionDetailsDlg *dlg = new UCvsSelectionDetailsDlg( mf, selDetType, selection.c_str());
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	if(dlg->DoModal())
	{
		selection = dlg->m_selection;
		selDetType = dlg->m_selDetType;

		userHitOK = true;
	}
	delete dlg;

#endif // qUnix

#ifdef qMacCvsPP
#endif /* qMacCvsPP */

	return userHitOK;
}
