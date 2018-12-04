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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com>
 */

// NoPython.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "NoPython.h"
#include "Persistent.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CPersistentBool gWarnPython("P_WarnPython", true);

/////////////////////////////////////////////////////////////////////////////
// CNoPython dialog


CNoPython::CNoPython(CWnd* pParent /*=NULL*/)
	: CHHelpDialog(CNoPython::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNoPython)
	m_remind = TRUE;
	//}}AFX_DATA_INIT
}


void CNoPython::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNoPython)
	DDX_Control(pDX, IDC_PYTHON_URL, m_url);
	DDX_Check(pDX, IDC_REMIND, m_remind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNoPython, CHHelpDialog)
	//{{AFX_MSG_MAP(CNoPython)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoPython message handlers

/// OnInitDialog virtual override, initialize controls
BOOL CNoPython::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	m_url.SetLink(_T("http://www.python.org"));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNoPython::InitHelpID()
{
	SetCtrlHelpID(IDC_PYTHON_URL,	IDH_IDC_PYTHON_URL);
	SetCtrlHelpID(IDC_REMIND,		IDH_IDC_REMIND);
}

/// BN_CLICKED message handler, display dialog help
void CNoPython::OnHelp() 
{
	ShowHelp();
}

/*!
	Display the warning about missing Python installation
*/
void WarnNoPython(void)
{
	if( (bool)gWarnPython )
	{
		CNoPython dlg;
		dlg.DoModal();
		
		if( !dlg.m_remind )
			gWarnPython = false;
	}
}
