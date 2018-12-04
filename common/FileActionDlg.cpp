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
* FileActionDlg.cpp : the file action dialog
*/

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "OptionalPropertySheet.h"
#endif /* WIN32 */

#include "FileActionDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"

#ifdef WIN32
#include "GetPrefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFileAction_MAIN property page

IMPLEMENT_DYNAMIC(CFileAction_MAIN, CHHelpPropertyPage)

CFileAction_MAIN::CFileAction_MAIN(const MultiFiles* mf, kLaunchType launchType) 
	: m_mf(mf), m_launchType(launchType), m_fileActionCombo(USmartCombo::AutoDropWidth), 
	CHHelpPropertyPage(CFileAction_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CFileAction_MAIN)
	m_fileAction = -1;
	m_sameActionNextTime = FALSE;
	//}}AFX_DATA_INIT

	m_sameActionNextTime = TRUE;
}

CFileAction_MAIN::~CFileAction_MAIN()
{
}

void CFileAction_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileAction_MAIN)
	DDX_Control(pDX, IDC_ACTION_DESC, m_fileActionDescStatic);
	DDX_Control(pDX, IDC_ACTION, m_fileActionCombo);
	DDX_CBIndex(pDX, IDC_ACTION, m_fileAction);
	DDX_Check(pDX, IDC_SAME_ACTION_NEXT_TIME, m_sameActionNextTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileAction_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CFileAction_MAIN)
	ON_CBN_SELCHANGE(IDC_ACTION, OnSelchangeAction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileAction_MAIN message handlers

/// OnInitDialog virtual override, initialize the controls
BOOL CFileAction_MAIN::OnInitDialog() 
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

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileAction_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_ACTION,					IDH_IDC_ACTION);
	SetCtrlHelpID(IDC_ACTION_DESC,				IDH_IDC_ACTION_DESC);
	SetCtrlHelpID(IDC_SAME_ACTION_NEXT_TIME,	IDH_IDC_SAME_ACTION_NEXT_TIME);
}

/// CBN_SELCHANGE notification handler, update action description
void CFileAction_MAIN::OnSelchangeAction() 
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
kLaunchType CFileAction_MAIN::GetLaunchType() const
{
	return m_launchType;
}

/// OnOK virtual override, retrieve launch type for selection
void CFileAction_MAIN::OnOK() 
{
	UpdateData();

	m_launchType = (kLaunchType)m_fileActionCombo.GetItemData(m_fileAction);
	
	CHHelpPropertyPage::OnOK();
}

#endif /* WIN32 */

/// Get the file action options
bool CompatGetFileAction(const MultiFiles* mf, kLaunchType& launchType)
{
	bool userHitOK = false;
	
	launchType = (kLaunchType)gCvsPrefs.FileActionType();

#ifdef WIN32
	COptionalPropertySheet pages(gCvsPrefs.HideCommandDlgFileAction(), "File action settings");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CFileAction_MAIN page1(mf, launchType);
	CGetPrefs_GLOBALS page2;
	
	pages.AddPage(&page1);
	pages.AddPage(&page2);
	
	if( pages.DoModal() == IDOK )
	{
		launchType = (kLaunchType)page1.GetLaunchType();

		if( page1.m_sameActionNextTime )
		{
			gCvsPrefs.SetFileActionType((FILEACTION_TYPE)launchType);
		}

		page2.StoreValues();
		gCvsPrefs.SetHideCommandDlgFileAction(pages.GetHideCommandDlg());
		userHitOK = true;
	}
#endif /* WIN32 */
	
#ifdef qMacCvsPP
	userHitOK = true;
#endif /* qMacCvsPP */
	
#if qUnix
	userHitOK = true;
#endif // qUnix
	
	if( userHitOK )
	{
		gCvsPrefs.save();
	}
	
	return userHitOK;
}
