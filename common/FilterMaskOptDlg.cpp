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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- August 2003
 */

/*
 * FilterMaskOptDlg.cpp --- filter mask options dialog
 */

#include "stdafx.h"

#ifdef WIN32
#include "wincvs.h"
#endif /* WIN32 */

#include "FilterMaskOptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CFilterMaskOptDlg dialog


CFilterMaskOptDlg::CFilterMaskOptDlg(const int reactionSpeed, CWnd* pParent /*=NULL*/)
	: CHHelpDialog(CFilterMaskOptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterMaskOptDlg)
	m_reactionSpeed = reactionSpeed;
	//}}AFX_DATA_INIT
}


void CFilterMaskOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterMaskOptDlg)
	DDX_Control(pDX, IDC_REACTION_SPEED_SECONDS, m_reactionSpeedStatic);
	DDX_Control(pDX, IDC_REACTION_SPEED_SPIN, m_reactionSpeedSpin);
	DDX_Text(pDX, IDC_REACTION_SPEED, m_reactionSpeed);
	DDV_MinMaxInt(pDX, m_reactionSpeed, 0, 1000000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterMaskOptDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CFilterMaskOptDlg)
	ON_EN_CHANGE(IDC_REACTION_SPEED, OnChangeReactionSpeed)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterMaskOptDlg message handlers

/// OnInitDialog virtual override
BOOL CFilterMaskOptDlg::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	m_reactionSpeedSpin.SetRange32(0, 1000000);
	
	UpdateReactionSpeedInfo();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFilterMaskOptDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_REACTION_SPEED,			IDH_IDC_REACTION_SPEED);
	SetCtrlHelpID(IDC_REACTION_SPEED_SPIN,		IDH_IDC_REACTION_SPEED_SPIN);
	SetCtrlHelpID(IDC_REACTION_SPEED_SECONDS,	IDH_IDC_REACTION_SPEED_SECONDS);
}

/// BN_CLICKED message handler, display dialog help
void CFilterMaskOptDlg::OnHelp() 
{
	ShowHelp();
}

/// EN_CHANGE notification handler, update the setting info
void CFilterMaskOptDlg::OnChangeReactionSpeed() 
{
	UpdateReactionSpeedInfo();
}

/*!
	Update reaction speed information text
*/
void CFilterMaskOptDlg::UpdateReactionSpeedInfo()
{
	if( ::IsWindow(m_reactionSpeedStatic.GetSafeHwnd()) )
	{
		BOOL trans = FALSE;
		const int reactionSpeed = GetDlgItemInt(IDC_REACTION_SPEED, &trans);
		
		CString reactionSpeedInfo;
		if( trans )
		{
			const double secReactionSpeed = (double)reactionSpeed / 1000;

			CString strSecReactionSpeed;
			strSecReactionSpeed.Format("%f", secReactionSpeed);
			strSecReactionSpeed.TrimRight("0");
			strSecReactionSpeed.TrimRight(".");

			if( !strSecReactionSpeed.IsEmpty() )
			{
				reactionSpeedInfo.Format("(%s second%s)", strSecReactionSpeed, secReactionSpeed >= 2 ? "s" : "");
			}
		}
		
		m_reactionSpeedStatic.SetWindowText(reactionSpeedInfo);
	}
}
#endif /* WIN32 */

/// Get the graph options
bool CompatGetFilterMaskOptions(int& reactionSpeed)
{
	bool userHitOK = false;
	
#ifdef WIN32
	CFilterMaskOptDlg filterMaskOptDlg(reactionSpeed);
	
	if( filterMaskOptDlg.DoModal() == IDOK )
	{
		reactionSpeed = filterMaskOptDlg.m_reactionSpeed;

		userHitOK = true;
	}
#endif /* WIN32 */

#ifdef qMacCvsPP
#endif /* qMacCvsPP */

#if qUnix
#endif // qUnix
	
	return userHitOK;
}
