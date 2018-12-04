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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2001
 */

/*
 * AdminOptionDlg.cpp --- admin options dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "AdminOptionDlg.h"
#	include "GetPrefs.h"
#	include "CvsAlert.h"
#endif /* WIN32 */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Default states
char* sDefStates[] = 
{
	"Rel",
	"Stab",
	"Exp",
	0L
};

extern CMString gCommitLogs;
static CMString gPrevStates(10, "P_PrevStates", sDefStates);

/////////////////////////////////////////////////////////////////////////////
// CAdminOption_MAIN dialog

#ifdef WIN32

CAdminOption_MAIN::CAdminOption_MAIN(const MultiFiles* mf, kAdminOptionDlgType type, const char* optionString, 
								 CWnd* pParent /*=NULL*/)
	: m_multilineEntryDlg(NULL, mf), 
	CHHelpPropertyPage(CAdminOption_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CAdminOption_MAIN)
	//}}AFX_DATA_INIT
	
	m_type = type;
	m_optionString = optionString;

	// Trim trailing white spaces and fix the line endings
	m_optionString.TrimRight();
	m_optionString.Replace("\n", "\r\n");
	
	// Set the page title
	m_psp.dwFlags |= PSP_USETITLE;

	switch( m_type )
	{
	case kChangeLog:
		m_psp.pszTitle = "Log message";
		break;
	case kSetState:
		m_psp.pszTitle = "State";
		break;
	case kSetDescription:
		m_psp.pszTitle = "Description";
		break;
	default:
		ASSERT(FALSE); //unknown type
		break;
	}
}


void CAdminOption_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdminOption_MAIN)
	DDX_Control(pDX, IDC_MULTILINE_ENTRY_STATIC, m_multilineEntryPlaceholder);
	//}}AFX_DATA_MAP

	m_multilineEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_optionString);
}


BEGIN_MESSAGE_MAP(CAdminOption_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CAdminOption_MAIN)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdminOption_MAIN message handlers

BOOL CAdminOption_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	SetMultilineEntry();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdminOption_MAIN::InitHelpID()
{
	// Nothing to do, it's all in multiline entry control
}

/*!
	Create multiline entry
*/
void CAdminOption_MAIN::SetMultilineEntry()
{
	kMultilineEntryType multilineEntryType = kMultilineLogMessage;
	CMString* pPreviousEntries = &gCommitLogs;

	switch( m_type )
	{
	case kChangeLog:
		multilineEntryType = kMultilineLogMessage;
		break;
	case kSetState:
		multilineEntryType = kMultilineState;
		pPreviousEntries = &gPrevStates;
		break;
	case kSetDescription:
		multilineEntryType = kMultilineDescription;
		break;
	default:
		ASSERT(FALSE); //unknown type
		break;
	}
	
	m_multilineEntryDlg.Create(multilineEntryType, pPreviousEntries, &m_multilineEntryPlaceholder, this);
}

/*!
	Get the option string
	\return The option string
*/
CString CAdminOption_MAIN::GetOptionString() const
{
	return m_optionString;
}

#endif /* WIN32 */

/*!
	Get the admin option
*/
bool CompatGetAdminOptions(const MultiFiles* mf, kAdminOptionDlgType type, std::string& optionString)
{
	bool userHitOK = false;

	const char* title = "Admin option settings";

	switch( type )
	{
	case kChangeLog:
		title = "Change log message";
		break;
	case kSetState:
		title = "Set state";
		break;
	case kSetDescription:
		title = "Set description";
		break;
	default:
#ifdef WIN32
		ASSERT(FALSE); //unknown type
#endif
		break;
	}

#ifdef WIN32
	CHHelpPropertySheet pages(title);
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CAdminOption_MAIN page1(mf, type, optionString.c_str());
	CGetPrefs_GLOBALS page2;
	
	pages.AddPage(&page1);
	pages.AddPage(&page2);
	
	if( pages.DoModal() == IDOK )
	{
		optionString = page1.GetOptionString();
		
		page2.StoreValues();
		
		userHitOK = true;
	}
#endif /* WIN32 */

#ifdef qMacCvsPP
#endif /* qMacCvsPP */
#if qUnix
#endif // qUnix
	
	return userHitOK;
}
