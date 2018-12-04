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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2000
 */

/*
 * ReleaseDlg.cpp : the cvs release dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */

#include "ReleaseDlg.h"
#include "MultiFiles.h"
#include "GetPrefs.h"
#include "CvsPrefs.h"
#include <string>

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

using namespace std;

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
// CRelease_MAIN property page

IMPLEMENT_DYNAMIC(CRelease_MAIN, CHHelpPropertyPage)

CRelease_MAIN::CRelease_MAIN(const char* dir,
		bool forceRoot, const char* cvsroot) 
	: CHHelpPropertyPage(CRelease_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CRelease_MAIN)
	m_dir = dir;
	//}}AFX_DATA_INIT

	m_forceRoot = forceRoot;
	m_cvsroot = cvsroot;

	m_removeDirectoryOption = kReleaseNoDel;
}

CRelease_MAIN::~CRelease_MAIN()
{
}

void CRelease_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRelease_MAIN)
	DDX_Control(pDX, IDC_CVSROOT_ENTRY_STATIC, m_cvsrootEntryPlaceholder);
	DDX_Control(pDX, IDC_DELETE_DIRECTORY, m_delDirCheck);
	DDX_Control(pDX, IDC_DELETE_CVS_INFO, m_delCvsInfoCheck);
	DDX_Control(pDX, IDC_DELETE_ALL_FILES, m_delAllCheck);
	DDX_Text(pDX, IDC_PATH, m_dir);
	//}}AFX_DATA_MAP

	if( pDX->m_bSaveAndValidate )
	{
		if( m_delCvsInfoCheck.GetCheck() )
		{
			m_removeDirectoryOption = kReleaseDelCvsInfo;
		}
		else if( m_delDirCheck.GetCheck() )
		{
			m_removeDirectoryOption = m_delAllCheck.GetCheck() ? kReleaseDelAll : kReleaseDelDir;
		}
	}

	m_cvsrootEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_forceRoot, m_cvsroot);
}


BEGIN_MESSAGE_MAP(CRelease_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CRelease_MAIN)
	ON_BN_CLICKED(IDC_DELETE_CVS_INFO, OnDelcvsinfo)
	ON_BN_CLICKED(IDC_DELETE_DIRECTORY, OnDeldir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRelease_MAIN message handlers

/// OnInitDialog virtual override, setup autocomplete
BOOL CRelease_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_cvsrootEntryDlg.Create(&m_cvsrootEntryPlaceholder, this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRelease_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_PATH,				IDH_IDC_PATH);
	SetCtrlHelpID(IDC_DELETE_CVS_INFO,	IDH_IDC_DELETE_CVS_INFO);
	SetCtrlHelpID(IDC_DELETE_DIRECTORY,	IDH_IDC_DELETE_DIRECTORY);
	SetCtrlHelpID(IDC_DELETE_ALL_FILES,	IDH_IDC_DELETE_ALL_FILES);
}

/*!
	Get the remove directory option
	\return The remove directory option
*/
kReleaseDelType CRelease_MAIN::GetRemoveDirectoryOption() const
{
	return m_removeDirectoryOption;
}
	 
/// BN_CLICKED message handler, uncheck other remove directory options if this one is selected
void CRelease_MAIN::OnDelcvsinfo() 
{
	if( m_delCvsInfoCheck.GetCheck() )
	{
		m_delAllCheck.EnableWindow(FALSE);

		m_delAllCheck.SetCheck(0);
		m_delDirCheck.SetCheck(0);
	}
}

/// BN_CLICKED message handler, uncheck other remove directory options if this one is selected and enable or disable deleting non-cvs files checkbox
void CRelease_MAIN::OnDeldir() 
{
	m_delAllCheck.EnableWindow(m_delDirCheck.GetCheck());
	m_delCvsInfoCheck.SetCheck(0);
	m_delAllCheck.SetCheck(0);
}
#endif /* WIN32 */

/// Get the release options
bool CompatGetRelease(const MultiFiles* mf, 
					  const char* dir, kReleaseDelType& removeDirectoryOption,
					  bool& forceCvsroot, std::string& cvsroot)
{
	bool userHitOK = false;

	forceCvsroot = false;

	static bool lastForceCvsroot = true;
	static string sLastCvsroot("");

	if( mf && mf->NumDirs() > 0 )
	{
		string path;
		mf->getdir(0, path);

		FindBestLastCvsroot(path.c_str(), sLastCvsroot);
	}
	
#ifdef WIN32
	CHHelpPropertySheet pages(_i18n("Release settings"));
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CRelease_MAIN page1(dir, lastForceCvsroot, sLastCvsroot.c_str());
	CGetPrefs_GLOBALS page2;

	pages.AddPage(&page1);
	pages.AddPage(&page2);

	if( pages.DoModal() == IDOK )
	{
		removeDirectoryOption = page1.GetRemoveDirectoryOption();
		lastForceCvsroot = page1.m_forceRoot == TRUE;
		sLastCvsroot = page1.m_cvsroot;
		
		page2.StoreValues();
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
		forceCvsroot = lastForceCvsroot;
		cvsroot = sLastCvsroot;

		if( mf && mf->NumDirs() > 0 && forceCvsroot )
		{
			string path;
			mf->getdir(0, path);
			
			InsertLastCvsroot(path.c_str(), cvsroot.c_str());
		}
	}

	return userHitOK;
}
