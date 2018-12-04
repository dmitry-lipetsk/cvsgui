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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2002
 */

/*
 * LoginDlg.cpp : the cvs login dialog implementation file
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */

#include "CvsPrefs.h"
#include "GetPrefs.h"
#include "LoginDlg.h"
#include "CvsRootDlg.h"

#ifdef WIN32

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CLogin_MAIN property page

IMPLEMENT_DYNAMIC(CLogin_MAIN, CHHelpPropertyPage)

CLogin_MAIN::CLogin_MAIN(bool forceRoot, const char* cvsroot) 
	: CHHelpPropertyPage(CLogin_MAIN::IDD)
{
	m_forceRoot = forceRoot;
	m_cvsroot = cvsroot;
	
	m_helpLogin = _T("Select \"Force using the CVSROOT\" option to login according to your general preferences settings "
		"(otherwise current sandbox settings will be used by CVS client executable).\r\n"
		"\r\n"
		"Login command is interactive and it will ask you for password.\r\n"
		"Entered password will be cached by CVS so you need to login only once for each CVSROOT.\r\n"
		"\r\n"
		"To remove the password from CVS cache use the logout command.");
}

CLogin_MAIN::~CLogin_MAIN()
{
}

void CLogin_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogin_MAIN)
	DDX_Control(pDX, IDC_CVSROOT_ENTRY_STATIC, m_cvsrootEntryPlaceholder);
	DDX_Text(pDX, IDC_HELPLOGIN, m_helpLogin);
	//}}AFX_DATA_MAP

	m_cvsrootEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_forceRoot, m_cvsroot);
}


BEGIN_MESSAGE_MAP(CLogin_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CLogin_MAIN)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogin_MAIN message handlers

/// OnInitDialog virtual override, enable controls
BOOL CLogin_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_cvsrootEntryDlg.Create(&m_cvsrootEntryPlaceholder, this, true);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLogin_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_HELPLOGIN,		IDH_IDC_HELPLOGIN);
}

#endif

/// Get the login options
bool CompatGetLogin(bool& forceCvsroot, std::string& cvsroot)
{
	bool userHitOK = false;

	static bool lastForceCvsroot = true;
	static std::string sLastCvsroot;
	
#ifdef WIN32
	CHHelpPropertySheet pages("Login settings");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CLogin_MAIN page1(lastForceCvsroot, sLastCvsroot.c_str());
	CGetPrefs_GLOBALS page2;

	pages.AddPage(&page1);
	pages.AddPage(&page2);

	if( pages.DoModal() == IDOK )
	{
		lastForceCvsroot = page1.m_forceRoot == TRUE;
		sLastCvsroot = page1.m_cvsroot;

		page2.StoreValues();

		userHitOK = true;
	}
#else
  userHitOK = true;
#endif /* WIN32 */

	if( userHitOK )
	{
		forceCvsroot = lastForceCvsroot;
		cvsroot = sLastCvsroot;
		
		gCvsPrefs.save();
	}

	return userHitOK;
}
