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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- June 2001
 */

/*
 * AnnotateDlg.cpp : the cvs annotate dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "OptionalPropertySheet.h"
#endif /* WIN32 */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LRadioButton.h>
#	include <LEditText.h>
#	include <LCheckBox.h>
#	include <LMultiPanelView.h>
#	include <LPopupGroupBox.h>
#	include "LPopupFiller.h"

#	include "MacCvsApp.h"
#endif /* qMacCvsPP */

#include "AnnotateDlg.h"
#include "CvsPrefs.h"
#include "MultiString.h"
#include "MultiFiles.h"
#include "ItemListDlg.h"

#ifdef WIN32
#include "GetPrefs.h"
#endif

using namespace std;

CPersistentBool gBulkAnnotate("P_AnnotateBulkAnnotate", false);

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnnotate_MAIN property page

IMPLEMENT_DYNAMIC(CAnnotate_MAIN, CHHelpPropertyPage)

CAnnotate_MAIN::CAnnotate_MAIN(const MultiFiles* mf, kAnnotateCmdType annotateType, 
							   const char* rev, bool isDate, bool bulkAnnotate, kLaunchType launchType) 
							   : m_mf(mf), 
							   m_annotateType(annotateType), 
							   m_launchType(launchType), 
							   m_fileActionCombo(USmartCombo::AutoDropWidth),
							   CHHelpPropertyPage(CAnnotate_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CAnnotate_MAIN)
	m_hasRev = FALSE;
	m_isDate = isDate;
	m_force = FALSE;
	m_noRecurse = FALSE;
	m_rev = rev;
	m_bulkAnnotate = bulkAnnotate;
	m_fileAction = -1;
	//}}AFX_DATA_INIT

	if( kAnnotateGraph != m_annotateType )
	{
		m_revCombo.SetItems(m_isDate ? &gDateNames : &gRevNames);
	}
	else
	{
		m_revCombo.SetFeature(USmartCombo::AutoDropWidth | USmartCombo::ReadOnly);
		
		if( m_mf && m_mf->TotalNumFiles() )
		{
			MultiFiles::const_iterator mfi = m_mf->begin();
			if( mfi != m_mf->end() && mfi->NumFiles() )
			{
				string path;
				string fileName;
				string currRev;
				
				mfi->get(0, path, fileName, currRev);
				m_rev = currRev.c_str();
			}
		}
	}
}

CAnnotate_MAIN::~CAnnotate_MAIN()
{
}

void CAnnotate_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnnotate_MAIN)
	DDX_Control(pDX, IDC_ACTION_DESC, m_fileActionDescStatic);
	DDX_Control(pDX, IDC_ACTION, m_fileActionCombo);
	DDX_Control(pDX, IDC_FORCE_HEAD_REV, m_forceCheck);
	DDX_Control(pDX, IDC_NORECURS, m_noRecurseCheck);
	DDX_Control(pDX, IDC_HAS_REV, m_hasRevCheck);
	DDX_Control(pDX, IDC_BROWSE_TAG, m_browseTagButton);
	DDX_Control(pDX, IDC_ISDATE, m_isDateCheck);
	DDX_Control(pDX, IDC_REV_TAG_DATE, m_revCombo);
	DDX_Check(pDX, IDC_HAS_REV, m_hasRev);
	DDX_Check(pDX, IDC_ISDATE, m_isDate);
	DDX_Check(pDX, IDC_FORCE_HEAD_REV, m_force);
	DDX_Check(pDX, IDC_NORECURS, m_noRecurse);
	DDX_CBString(pDX, IDC_REV_TAG_DATE, m_rev);
	DDX_Check(pDX, IDC_BULK_ANNOTATE, m_bulkAnnotate);
	DDX_CBIndex(pDX, IDC_ACTION, m_fileAction);
	//}}AFX_DATA_MAP

	if( kAnnotateGraph != m_annotateType )
	{
		if( m_hasRev )
		{
			DDV_MinChars(pDX, m_rev, 1);
		}
		
		DDX_ComboMString(pDX, IDC_REV_TAG_DATE, m_revCombo);
		
		if( !pDX->m_bSaveAndValidate )
		{
			OnSpecifyRev();
		}
	}
}


BEGIN_MESSAGE_MAP(CAnnotate_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CAnnotate_MAIN)
	ON_BN_CLICKED(IDC_HAS_REV, OnSpecifyRev)
	ON_BN_CLICKED(IDC_ISDATE, OnIsdate)
	ON_BN_CLICKED(IDC_BROWSE_TAG, OnBrowseTag)
	ON_CBN_SELCHANGE(IDC_ACTION, OnSelchangeAction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnnotate_MAIN message handlers

/// OnInitDialog virtual override, initialize the controls
BOOL CAnnotate_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	for(int index = 0; index != kLaunchEnd; index++)
	{
		const int item = m_fileActionCombo.AddString(GetLaunchTypeName((kLaunchType)index, true));
		if( item > -1  )
		{
			m_fileActionCombo.SetItemData(item, (DWORD)index);
			if( index == m_launchType )
			{
				m_fileActionCombo.SetCurSel(item);
			}
		}
	}
	
	if( m_fileActionCombo.GetCurSel() < 0 )
	{
		m_fileActionCombo.SetCurSel(0);
	}
	
	OnSelchangeAction();

	// Special setup for the annotate invoked using the graph view
	if( kAnnotateGraph == m_annotateType )
	{
		m_hasRevCheck.EnableWindow(FALSE);
		m_hasRevCheck.SetCheck(1);

		m_browseTagButton.EnableWindow(FALSE);
		
		m_isDateCheck.EnableWindow(FALSE);
		
		m_revCombo.SetReadOnly(TRUE);
		m_revCombo.ResetContent();
		
		if( !m_rev.IsEmpty() )
		{
			m_revCombo.AddString(m_rev);
			m_revCombo.SetWindowText(m_rev);
		}

		m_noRecurseCheck.EnableWindow(FALSE);
		m_forceCheck.EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAnnotate_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_HAS_REV,			IDH_IDC_HAS_REV);
	SetCtrlHelpID(IDC_ISDATE,			IDH_IDC_ISDATE);
	SetCtrlHelpID(IDC_REV_TAG_DATE,		IDH_IDC_REV_TAG_DATE);
	SetCtrlHelpID(IDC_BROWSE_TAG,		IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_FORCE_HEAD_REV,	IDH_IDC_FORCE_HEAD_REV);
	SetCtrlHelpID(IDC_NORECURS,			IDH_IDC_NORECURS);
	SetCtrlHelpID(IDC_BULK_ANNOTATE,	IDH_IDC_BULK_ANNOTATE);
	SetCtrlHelpID(IDC_ACTION,					IDH_IDC_ACTION);
	SetCtrlHelpID(IDC_ACTION_DESC,				IDH_IDC_ACTION_DESC);
	SetCtrlHelpID(IDC_SAME_ACTION_NEXT_TIME,	IDH_IDC_SAME_ACTION_NEXT_TIME);
}

/// BN_CLICKED message handler, enable controls
void CAnnotate_MAIN::OnSpecifyRev() 
{
	const BOOL bEnable = m_hasRevCheck.GetCheck();
	
	m_isDateCheck.EnableWindow(bEnable);
	m_revCombo.EnableWindow(bEnable);
	m_forceCheck.EnableWindow(bEnable);
	m_browseTagButton.EnableWindow(bEnable && !m_isDateCheck.GetCheck());
}

/// BN_CLICKED message handler, enable controls
void CAnnotate_MAIN::OnIsdate() 
{
	const BOOL isDate = m_isDateCheck.GetCheck();

	m_revCombo.SetItems(isDate ? &gDateNames : &gRevNames);
	DDX_ComboMString(NULL, IDC_REV_TAG_DATE, m_revCombo);

	m_browseTagButton.EnableWindow(!isDate);
}

/// BN_CLICKED message handler, display browse tag/branch dialog
void CAnnotate_MAIN::OnBrowseTag() 
{
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName) )
	{
		m_rev = tagName.c_str();
		m_revCombo.SetWindowText(m_rev);
	}
}

/// CBN_SELCHANGE notification handler, update action description
void CAnnotate_MAIN::OnSelchangeAction() 
{
	const int item = m_fileActionCombo.GetCurSel();
	if( item > -1 )
	{
		const kLaunchType launchType = (kLaunchType)m_fileActionCombo.GetItemData(item);
		m_fileActionDescStatic.SetWindowText(GetLaunchTypeName(launchType, false));
	}
}

/*!
	Get the selected launch type
	\return The selected launch type
*/
kLaunchType CAnnotate_MAIN::GetLaunchType() const
{
	return m_launchType;
}

/// OnOK virtual override, retrieve launch type for selection
void CAnnotate_MAIN::OnOK() 
{
	UpdateData();
	
	m_launchType = (kLaunchType)m_fileActionCombo.GetItemData(m_fileAction);
	
	CHHelpPropertyPage::OnOK();
}

#endif /* WIN32 */

/// Get the annotate options
bool CompatGetAnnotate(const MultiFiles* mf, kAnnotateCmdType annotateType, 
					   bool& noRecurs, bool& isDate, std::string& rev, bool& force,
					   bool& bulkAnnotate, kLaunchType& launchType)
{
	bool userHitOK = false;

	static string sLastRev;
	static bool bIsDate = false;
	static kLaunchType sLaunchType = kLaunchAnnotateViewer;
	
	bool hasRev = false;
	string initialRev;
	
	// Graph view annotate supplies initial revisions, so don't override them
	if( annotateType == kAnnotateGraph )
	{
		initialRev = rev;
		hasRev = !rev.empty();
	}
	else
	{
		rev = "";
		hasRev = false;
		initialRev = sLastRev;
	}

	noRecurs = false;
	force = false;
	isDate = bIsDate;
	
	bulkAnnotate = (bool)gBulkAnnotate;
	launchType = sLaunchType;

#ifdef WIN32
	COptionalPropertySheet pages(gCvsPrefs.HideCommandDlgAnnotate(), "Annotate settings");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CAnnotate_MAIN page1(mf, annotateType, initialRev.c_str(), isDate, bulkAnnotate, launchType);
	CGetPrefs_GLOBALS page2;
	
	pages.AddPage(&page1);
	pages.AddPage(&page2);
	
	if( pages.DoModal() == IDOK )
	{
		noRecurs = page1.m_noRecurse == TRUE;

		sLastRev = page1.m_rev;
		isDate = page1.m_isDate == TRUE;
		hasRev = page1.m_hasRev == TRUE;
		force = page1.m_force == TRUE;
		gBulkAnnotate = page1.m_bulkAnnotate ? true : false;
		sLaunchType = (kLaunchType)page1.GetLaunchType();

		page2.StoreValues();
		gCvsPrefs.SetHideCommandDlgAnnotate(pages.GetHideCommandDlg());
		userHitOK = true;
	}
#endif /* WIN32 */

#ifdef qMacCvsPP
  userHitOK = true;
#endif /* qMacCvsPP */

#if qUnix
#endif // qUnix
	
	if( userHitOK )
	{
		if( hasRev )
			rev = sLastRev;

		bIsDate = isDate;
		bulkAnnotate = (bool)gBulkAnnotate;
		launchType = sLaunchType;

		gCvsPrefs.save();
	}
	
	return userHitOK;
}
