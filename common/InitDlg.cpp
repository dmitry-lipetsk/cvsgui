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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- June 1999
 */

/*
 * InitDlg.cpp : the cvs init dialog
 */

#include "stdafx.h"
#include <string>

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */

#include "InitDlg.h"
#include "CvsPrefs.h"
#include "CvsRootDlg.h"

#ifdef WIN32
#	include "GetPrefs.h"
#	include "PromptFiles.h"
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CInit_MAIN dialog

CInit_MAIN::CInit_MAIN(bool forceRoot, const char* cvsroot)
	: CHHelpPropertyPage(CInit_MAIN::IDD)
{
	m_forceRoot = forceRoot;
	m_cvsroot = cvsroot;

	m_helpinit = _T("If the CVSROOT is set to ""local"", "
		"the CVSROOT should be a Windows path (i.e. c:\\cvsroot). "
		"If the CVSROOT points on a remote machine, the path should be a Unix path "
		"(i.e. /fscvs/cvsroot : make sure you got the relevant security rights in "
		"order to create such a directory on the remote machine.");
}

void CInit_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInit_MAIN)
	DDX_Control(pDX, IDC_CVSROOT_ENTRY_STATIC, m_cvsrootEntryPlaceholder);
	DDX_Text(pDX, IDC_HELP_INIT, m_helpinit);
	//}}AFX_DATA_MAP

	m_cvsrootEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_forceRoot, m_cvsroot);
}

BEGIN_MESSAGE_MAP(CInit_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CInit_MAIN)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CInit_MAIN, CHHelpPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CInit_MAIN message handlers

/// OnInitDialog virtual override, enable controls
BOOL CInit_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_cvsrootEntryDlg.Create(&m_cvsrootEntryPlaceholder, this, true);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInit_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_HELP_INIT,		IDH_IDC_HELP_INIT);
}

#endif

/// Get the init options
bool CompatGetInit(bool& forceCvsroot, std::string& cvsroot)
{
	bool userHitOK = false;

	static bool lastForceCvsroot = true;
	static std::string sLastCvsroot;
	
#ifdef WIN32
	CHHelpPropertySheet pages("Init settings");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CInit_MAIN page1(lastForceCvsroot, sLastCvsroot.c_str());
	CGetPrefs_GLOBALS page2;
	
	pages.AddPage(&page1);
	pages.AddPage(&page2);

	if(pages.DoModal() == IDOK)
	{
		lastForceCvsroot = page1.m_forceRoot == TRUE;
		sLastCvsroot = page1.m_cvsroot;

		page2.StoreValues();

		userHitOK = true;
	}
#endif /* WIN32 */

	if( userHitOK )
	{
		forceCvsroot = lastForceCvsroot;
		cvsroot = sLastCvsroot;

		gCvsPrefs.save();
	}

	return userHitOK;
}
