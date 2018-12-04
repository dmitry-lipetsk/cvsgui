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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- March 2002
 */

/*
 * StatusDlg.cpp : the cvs status dialog
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

#include "StatusDlg.h"
#include "MultiFiles.h"

#ifdef WIN32
#include "GetPrefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatus_MAIN property page

IMPLEMENT_DYNAMIC(CStatus_MAIN, CHHelpPropertyPage)

CStatus_MAIN::CStatus_MAIN(bool noRecurs, STATUSOUTPUT_TYPE outputType) 
	: CHHelpPropertyPage(CStatus_MAIN::IDD)
{
	m_noRecurs = noRecurs;
	m_outputType = (int)outputType;
}

CStatus_MAIN::~CStatus_MAIN()
{
}

void CStatus_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatus_MAIN)
	DDX_Check(pDX, IDC_NORECURS, m_noRecurs);
	DDX_Radio(pDX, IDC_OUTPUT_STANDARD, m_outputType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatus_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CStatus_MAIN)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatus_MAIN message handlers

void CStatus_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_OUTPUT_STANDARD,	IDH_IDC_OUTPUT_STANDARD);
	SetCtrlHelpID(IDC_OUTPUT_QUICK,		IDH_IDC_OUTPUT_QUICK);
	SetCtrlHelpID(IDC_OUTPUT_VERBOSE,	IDH_IDC_OUTPUT_VERBOSE);
	SetCtrlHelpID(IDC_NORECURS,			IDH_IDC_NORECURS);
}

#endif /* WIN32 */

/// Get the status options
bool CompatGetStatus(const MultiFiles* mf, bool& noRecurs, STATUSOUTPUT_TYPE& outputType)
{
	bool userHitOK = false;

	noRecurs = false;
	outputType = gCvsPrefs.StatusOutputType();
	
#ifdef WIN32
	COptionalPropertySheet pages(gCvsPrefs.HideCommandDlgStatus(), "Status settings");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	CStatus_MAIN page1(noRecurs, outputType);
	CGetPrefs_GLOBALS page2;
	pages.AddPage(&page1);
	pages.AddPage(&page2);

	if( pages.DoModal() == IDOK )
	{
		noRecurs = page1.m_noRecurs ? true : false;
		gCvsPrefs.SetStatusOutputType((STATUSOUTPUT_TYPE)page1.m_outputType);

		page2.StoreValues();
		gCvsPrefs.SetHideCommandDlgStatus(pages.GetHideCommandDlg());
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
		outputType = gCvsPrefs.StatusOutputType();
		
		gCvsPrefs.save();
	}
	
	return userHitOK;
}
