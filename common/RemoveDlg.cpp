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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- January 2007
 */

/*
 * RemoveDlg.cpp : the cvs remove dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "OptionalPropertySheet.h"
#	include "GetPrefs.h"
#endif /* WIN32 */

#include "RemoveDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif /* WIN32 */

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CRemoveMain

IMPLEMENT_DYNAMIC(CRemoveMain, CHHelpPropertyPage)

CRemoveMain::CRemoveMain(const MultiFiles* mf, const bool noRecurs, const kFilesRemoveType filesRemoveType) 
	: m_mf(mf),
	m_filesRemoveTypeCombo(USmartCombo::AutoDropWidth),
	CHHelpPropertyPage(CRemoveMain::IDD)
{
	//{{AFX_DATA_INIT(CRemoveMain)
	m_norecurs = noRecurs;
	m_filesRemoveType = filesRemoveType;
	//}}AFX_DATA_INIT
}

CRemoveMain::~CRemoveMain()
{
}

void CRemoveMain::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemoveMain)
	DDX_Control(pDX, IDC_FILES_REMOVAL_TYPE, m_filesRemoveTypeCombo);
	DDX_Check(pDX, IDC_NORECURS, m_norecurs);
	DDX_CBIndex(pDX, IDC_FILES_REMOVAL_TYPE, m_filesRemoveType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemoveMain, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CRemoveMain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemoveMain message handlers

void CRemoveMain::InitHelpID()
{
	SetCtrlHelpID(IDC_NORECURS,				IDH_IDC_NORECURS);
	SetCtrlHelpID(IDC_FILES_REMOVAL_TYPE,	IDH_IDC_FILES_REMOVAL_TYPE);
}

/// OnInitDialog virtual override, initialize the controls
BOOL CRemoveMain::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization here
	if( m_mf->TotalNumFiles() > 0 )
	{
		m_filesRemoveTypeCombo.InsertString(0, "Move files to recycle bin");
	}

	m_filesRemoveTypeCombo.InsertString(0, "Allow CVS to remove files");
	m_filesRemoveTypeCombo.InsertString(0, "Don't remove files");

	m_filesRemoveTypeCombo.SetCurSel(m_filesRemoveType);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#endif /* WIN32 */

/// Get the update options
bool CompatGetRemove(const MultiFiles* mf, 
					 bool& noRecurs, 
					 kFilesRemoveType& filesRemoveType)
{
	bool userHitOK = false;
	
	static bool sNoRecurs = false;
	static kFilesRemoveType sFilesRemoveTypeFiles = kMoveToRecycleBin;
	static kFilesRemoveType sFilesRemoveTypeFolder = kRemoveByCvs;

	noRecurs = sNoRecurs;
	filesRemoveType = mf->TotalNumFiles() > 0 ? sFilesRemoveTypeFiles : sFilesRemoveTypeFolder;

#ifdef WIN32
	COptionalPropertySheet pages(gCvsPrefs.HideCommandDlgRemove(), _i18n("Remove settings"));
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CRemoveMain page1(mf, noRecurs, filesRemoveType);
	CGetPrefs_GLOBALS page2;
	
	pages.AddPage(&page1);
	pages.AddPage(&page2);
	
	if( pages.DoModal() == IDOK )
	{
		sNoRecurs = page1.m_norecurs == TRUE;

		if( mf->TotalNumFiles() > 0 )
		{
			sFilesRemoveTypeFiles = (kFilesRemoveType)page1.m_filesRemoveType;
		}
		else
		{
			sFilesRemoveTypeFolder = (kFilesRemoveType)page1.m_filesRemoveType;
		}

		page2.StoreValues();
		gCvsPrefs.SetHideCommandDlgRemove(pages.GetHideCommandDlg());
		
		userHitOK = true;
	}
#endif /* WIN32 */

	if( userHitOK )
	{
		filesRemoveType = mf->TotalNumFiles() > 0 ? sFilesRemoveTypeFiles : sFilesRemoveTypeFolder;
		noRecurs = sNoRecurs;

		gCvsPrefs.save();
	}

	return userHitOK;
}
