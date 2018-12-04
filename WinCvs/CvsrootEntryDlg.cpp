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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- August 18 2003
 */

/*
 * CvsrootEntryDlg.cpp : CVSROOT entry complex
 */

#include "stdafx.h"
#include "wincvs.h"
#include "CvsrootEntryDlg.h"
#include "CvsRootDlg.h"
#include "CvsPrefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CCvsrootEntryDlg property page

IMPLEMENT_DYNCREATE(CCvsrootEntryDlg, CHHelpPropertyPage)

CCvsrootEntryDlg::CCvsrootEntryDlg() : CHHelpPropertyPage(CCvsrootEntryDlg::IDD)
{
	//{{AFX_DATA_INIT(CCvsrootEntryDlg)
	m_forceRoot = FALSE;
	m_cvsroot = _T("");
	//}}AFX_DATA_INIT
}

CCvsrootEntryDlg::~CCvsrootEntryDlg()
{
}

void CCvsrootEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCvsrootEntryDlg)
	DDX_Control(pDX, IDC_FORCE_CVSROOT, m_forceRootCheck);
	DDX_Control(pDX, IDC_BROWSE_CVSROOT, m_selCvsrootButton);
	DDX_Control(pDX, IDC_CVSROOT, m_cvsrootCombo);
	DDX_Check(pDX, IDC_FORCE_CVSROOT, m_forceRoot);
	DDX_CBString(pDX, IDC_CVSROOT, m_cvsroot);
	//}}AFX_DATA_MAP

	DDX_ComboMString(pDX, IDC_CVSROOT, m_cvsrootCombo);
	
	if( m_forceRoot )
	{
		DDV_MinChars(pDX, m_cvsroot, 1);
	}
	
	if( !pDX->m_bSaveAndValidate )
	{
		m_selCvsrootButton.EnableWindow(m_forceRoot);
	}
}


BEGIN_MESSAGE_MAP(CCvsrootEntryDlg, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CCvsrootEntryDlg)
	ON_BN_CLICKED(IDC_BROWSE_CVSROOT, OnBrowseCvsroot)
	ON_BN_CLICKED(IDC_FORCE_CVSROOT, OnForceCvsroot)
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCvsrootEntryDlg message handlers

void CCvsrootEntryDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_FORCE_CVSROOT,	IDH_IDC_FORCE_CVSROOT);
	SetCtrlHelpID(IDC_CVSROOT,			IDH_IDC_CVSROOT);
	SetCtrlHelpID(IDC_BROWSE_CVSROOT,	IDH_IDC_BROWSE_CVSROOT);
}

/// BN_CLICKED message handler, select CVSROOT
void CCvsrootEntryDlg::OnBrowseCvsroot() 
{
	CString strCvsroot;
	m_cvsrootCombo.GetWindowText(strCvsroot);
	
	string cvsroot(strCvsroot);
	if( CompatGetCvsRoot(cvsroot) )
	{
		m_cvsrootCombo.SetWindowText(cvsroot.c_str());
	}
}

/// BN_CLICKED message handler, enable controls
void CCvsrootEntryDlg::OnForceCvsroot() 
{
	const BOOL enable = m_forceRootCheck.GetCheck();
	
	m_selCvsrootButton.EnableWindow(enable);
	m_cvsrootCombo.EnableWindow(enable);
}

/*!
	Create the entry dialog
	\param pWndInsertAfter Placeholder window
	\param pParentWnd Parent window
	\param setFocus true to set the focus to the control when the dialog shows for the first time, false otherwise
	\param updateData true to invoke UpdateData(FALSE) for the parent dialog after the creation to display the initial text entry, false otherwise
	\return TRUE on success, FALSE otherwise
*/
BOOL CCvsrootEntryDlg::Create(const CWnd* pWndInsertAfter, CDialog* pParentWnd, const bool setFocus /*= false*/, const bool updateData /*= true*/)
{
	BOOL res = FALSE;

	ASSERT(pWndInsertAfter != NULL);	// Invalid pointer
	ASSERT(pParentWnd != NULL);			// Invalid pointer

	m_pParentWnd = pParentWnd;

	m_cvsrootCombo.SetItems(&gOldCvsroots);

	if( CHHelpPropertyPage::Create(CCvsrootEntryDlg::IDD, m_pParentWnd) )
	{
		// Get the width difference
		CRect entryRect;
		GetWindowRect(entryRect);

		CRect windowRect;
		pWndInsertAfter->GetWindowRect(windowRect);

		const int widthDiff = windowRect.Width() - entryRect.Width();

		// Adjust the size of CVSROOT combo and move the browse button to stick in
		if( widthDiff )
		{
			CRect cvsrootComboRect;
			m_cvsrootCombo.GetWindowRect(cvsrootComboRect);
			
			m_cvsrootCombo.SetWindowPos(NULL, 0, 0, cvsrootComboRect.Width() + widthDiff, cvsrootComboRect.Height(), SWP_NOZORDER | SWP_NOMOVE);
		}

		if( widthDiff )
		{
			CRect selCvsrootButtonRect;
			m_selCvsrootButton.GetWindowRect(selCvsrootButtonRect);
			
			pParentWnd->ScreenToClient(selCvsrootButtonRect);

			m_selCvsrootButton.SetWindowPos(NULL, selCvsrootButtonRect.left + widthDiff, selCvsrootButtonRect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}

		// Move the entry window into the placeholder space
		pParentWnd->ScreenToClient(windowRect);
		SetWindowPos(pWndInsertAfter, windowRect.left, windowRect.top, windowRect.Width(), windowRect.Height(), SWP_SHOWWINDOW);

		// Update state of controls
		if( updateData )
		{
			m_pParentWnd->UpdateData(FALSE);
		}

		OnForceCvsroot();

		// Set the focus if required
		if( setFocus )
		{
			PostGotoDlgCtrl(pParentWnd, m_forceRoot ? (CWnd*)&m_cvsrootCombo : (CWnd*)&m_forceRootCheck);
		}

		res = TRUE;
	}

	return res;
}

/*!
	Enable or disable controls
	\param enable Set to TRUE to enable controls, FALSE to disable controls
*/
void CCvsrootEntryDlg::EnableControls(const BOOL enable)
{
	m_forceRootCheck.EnableWindow(enable);
	m_selCvsrootButton.EnableWindow(enable);
	m_cvsrootCombo.EnableWindow(enable);

	if( enable )
	{
		OnForceCvsroot();
	}
}

/*!
	Invoke DDX for multiline entry
	\param bSaveAndValidate Validation flag
	\param entryState Return entered entry state
	\param entryText Return the entered text
	\return TRUE on success, FALSE otherwise
*/
BOOL CCvsrootEntryDlg::UpdateData(BOOL bSaveAndValidate, int& entryState, CString& entryText)
{
	BOOL res = FALSE;

	if( bSaveAndValidate )
	{
		res = ::IsWindow(GetSafeHwnd()) ? CHHelpPropertyPage::UpdateData(TRUE) : TRUE;

		entryState = m_forceRoot;
		entryText = m_cvsroot;
	}
	else
	{
		m_forceRoot = entryState;
		m_cvsroot = entryText;

		res = ::IsWindow(GetSafeHwnd()) ? CHHelpPropertyPage::UpdateData(FALSE) : TRUE;
	}

	return res;
}

/*!
	Get the state of force CVSROOT checkbox
	\return The state of force CVSROOT checkbox
*/
BOOL CCvsrootEntryDlg::GetForceCvsrootCheck() const
{
	return m_forceRootCheck.GetCheck();
}

/*!
	Get the CVSROOT text
	\param cvsroot Return CVSROOT text
*/
void CCvsrootEntryDlg::GetCvsrootWindowText(CString& cvsroot) const
{
	m_cvsrootCombo.GetWindowText(cvsroot);
}

/// WM_ENABLE message handler, update controls state
void CCvsrootEntryDlg::OnEnable(BOOL bEnable) 
{
	CHHelpPropertyPage::OnEnable(bEnable);
	
	OnForceCvsroot();
}
