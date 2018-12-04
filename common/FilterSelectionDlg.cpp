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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- September 2003
 */

/*
 * FilterSelectionDlg.cpp : the filter selection dialog
 */


#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif	/* WIN32 */

#include "FilterSelectionDlg.h"

/// Number of previous filters entries
#define PREV_FILTER_SIZE 100

static CMString gPrevFilterName(PREV_FILTER_SIZE,		"P_PrevFilterName");
static CMString gPrevFilterExtention(PREV_FILTER_SIZE,	"P_PrevFilterExtention");
static CMString gPrevFilterRevision(PREV_FILTER_SIZE,	"P_PrevFilterRevision");
static CMString gPrevFilterOption(PREV_FILTER_SIZE,		"P_PrevFilterOption");
static CMString gPrevFilterState(PREV_FILTER_SIZE,		"P_PrevFilterState");
static CMString gPrevFilterEncoding(PREV_FILTER_SIZE,	"P_PrevFilterEncoding");
static CMString gPrevFilterStickyTag(PREV_FILTER_SIZE,	"P_PrevFilterStickyTag");
//static CMString gPrevFilterTimeStamp(PREV_FILTER_SIZE,	"P_PrevFilterTimeStamp");
//static CMString gPrevFilterConflict(PREV_FILTER_SIZE,	"P_PrevFilterConflict");
static CMString gPrevFilterInfo(PREV_FILTER_SIZE,		"P_PrevFilterInfo");
static CMString gPrevFilterPath(PREV_FILTER_SIZE,		"P_PrevFilterPath");
static CMString gPrevFilterModifed(PREV_FILTER_SIZE,	"P_PrevFilterModifed");

/*!
	Get the previous filter for a given column index
	\param columnIndex Column index
	\return The previous filter
*/
CMString* GetPrevFilter(const int columnIndex)
{
	CMString* pRes = NULL;

	if( columnIndex >= 0 && columnIndex < ColumnIndex::kEnd )
	{
		static CMString* prevFilters[ColumnIndex::kEnd] = { 0 };
		
		if( !prevFilters[0] )
		{
			prevFilters[ColumnIndex::kName		] = &gPrevFilterName;
			prevFilters[ColumnIndex::kExtention	] = &gPrevFilterExtention;
			prevFilters[ColumnIndex::kRevision	] = &gPrevFilterRevision ;
			prevFilters[ColumnIndex::kOption	] = &gPrevFilterOption;
			prevFilters[ColumnIndex::kState		] = &gPrevFilterState;
			prevFilters[ColumnIndex::kEncoding	] = &gPrevFilterEncoding;
			prevFilters[ColumnIndex::kStickyTag	] = &gPrevFilterStickyTag;
			prevFilters[ColumnIndex::kTimeStamp	] = NULL;	//&gPrevFilterTimeStamp;
			prevFilters[ColumnIndex::kConflict	] = NULL;	//&gPrevFilterConflict;
			prevFilters[ColumnIndex::kInfo		] = &gPrevFilterInfo;
			prevFilters[ColumnIndex::kPath		] = &gPrevFilterPath;
			prevFilters[ColumnIndex::kModTime	] = &gPrevFilterModifed;
		}

		pRes = prevFilters[columnIndex];
	}

	return pRes;
}

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif

/////////////////////////////////////////////////////////////////////////////
// CFilterSelectionDlg dialog


CFilterSelectionDlg::CFilterSelectionDlg(const int columnIndex, const char* filterText, 
										 CWnd* pParent /*=NULL*/)
	: m_contentsCombo(USmartCombo::AutoDropWidth), m_filterCombo(USmartCombo::RemoveItems), 
	CHHelpDialog(CFilterSelectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterSelectionDlg)
	m_filterText = _T("");
	//}}AFX_DATA_INIT

	m_columnIndex = columnIndex;
	m_filterText = filterText;
}


void CFilterSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterSelectionDlg)
	DDX_Control(pDX, IDC_FILTER, m_filterCombo);
	DDX_Control(pDX, IDC_FILTER_CONTENTS, m_contentsCombo);
	DDX_CBString(pDX, IDC_FILTER, m_filterText);
	//}}AFX_DATA_MAP

	if( pDX->m_bSaveAndValidate )
	{
		if( !m_filterText.IsEmpty() )
		{
			CMString* prevFilter = GetPrevFilter(m_columnIndex);
			if( prevFilter )
			{
				prevFilter->Insert(m_filterText);
			}
		}
	}
	else
	{
		// Display only
		DDX_ComboMString(pDX, IDC_FILTER, m_filterCombo);
	}
}


BEGIN_MESSAGE_MAP(CFilterSelectionDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CFilterSelectionDlg)
	ON_CBN_SELCHANGE(IDC_FILTER_CONTENTS, OnSelchangeContentsCombo)
	ON_CBN_DBLCLK(IDC_FILTER, OnDblclkCombofilter)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterSelectionDlg message handlers

/// OnInitDialog virtual override, position the window so it looks like a drop-down list rather than a dialog
BOOL CFilterSelectionDlg::OnInitDialog() 
{
	// Extra initialization
	CMString* prevFilter = GetPrevFilter(m_columnIndex);
	if( prevFilter )
	{
		m_filterCombo.SetItems(prevFilter);
	}

	CHHelpDialog::OnInitDialog();

	// Add the columns to the contents combo
	for(int index = 0; index != ColumnIndex::kEnd; index++)
	{
		if( GetPrevFilter(index) )
		{
			const int item = m_contentsCombo.AddString(ColumnIndex::GetColumnName(index, false));
			if( item > -1  )
			{
				m_contentsCombo.SetItemData(item, (DWORD)index);
			}
		}
	}

	// Find and set the filter column
	for(int nIndex = 0; nIndex < m_contentsCombo.GetCount(); nIndex++)
	{
		if( m_contentsCombo.GetItemData(nIndex) == (DWORD)m_columnIndex )
		{
			m_contentsCombo.SetCurSel(nIndex);
			break;
		}
	}

	// Select the item in the list
	const int itemIndex = m_filterCombo.FindStringExact(-1, m_filterText);
	if( itemIndex > -1 )
	{
		m_filterCombo.SetCurSel(itemIndex);
	}

	// Set the dialog window title
	CString strTitle;
	strTitle.Format("Select %s Filter", ColumnIndex::GetColumnName(m_columnIndex, false));
	SetWindowText(strTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFilterSelectionDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_FILTER,			IDH_IDC_FILTER);
	SetCtrlHelpID(IDC_FILTER_CONTENTS,	IDH_IDC_FILTER_CONTENTS);
}

/// BN_CLICKED message handler, display dialog help
void CFilterSelectionDlg::OnHelp() 
{
	ShowHelp();
}

/// CBN_SELCHANGE notification handler, change the contents as appropriate
void CFilterSelectionDlg::OnSelchangeContentsCombo() 
{
	const int selection = m_contentsCombo.GetCurSel();
	if( selection > -1 )
	{
		// Store the filter text
		CString filterText;
		m_filterCombo.GetWindowText(filterText);

		// Set the items
		const int columnIndex = (int)m_contentsCombo.GetItemData(selection);
		CMString* prevFilter = GetPrevFilter(columnIndex);
		if( prevFilter )
		{
			m_filterCombo.SetItems(prevFilter);
		}

		UpdateData(FALSE);
		
		// Restore the filter text and the selection
		m_filterCombo.SetWindowText(filterText);

		const int itemIndex = m_filterCombo.FindStringExact(-1, filterText);
		if( itemIndex > -1 )
		{
			m_filterCombo.SetCurSel(itemIndex);
		}
	}
}

/// CBN_DBLCLK notification handler, select the item
void CFilterSelectionDlg::OnDblclkCombofilter() 
{
	OnOK();
}

#endif	/* WIN32 */

/*!
	Get the filter selection
	\param columnIndex The column index
	\param filterText Filter text
	\return true if user selection was made, false otherwise
*/
bool CompatGetFilterSelection(const int columnIndex, std::string& filterText)
{
	bool userHitOK = false;

#ifdef WIN32
	CFilterSelectionDlg filterSelection(columnIndex, filterText.c_str());

	if( filterSelection.DoModal() == IDOK )
	{
		filterText = filterSelection.m_filterText;

		userHitOK = true;
	}
#endif /* WIN32 */

#ifdef qMacCvsPP
#endif /* qMacCvsPP */

#if qUnix
#endif // qUnix

	return userHitOK;
}
