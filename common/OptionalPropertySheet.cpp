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

// OptionalPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "OptionalPropertySheet.h"
#include "CvsPrefs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionalPropertySheet

IMPLEMENT_DYNAMIC(COptionalPropertySheet, CHHelpPropertySheet)

COptionalPropertySheet::COptionalPropertySheet(bool bOptionChecked, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CHHelpPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_bOptionChecked = bOptionChecked;
}

COptionalPropertySheet::COptionalPropertySheet(bool bOptionChecked, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CHHelpPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_bOptionChecked = bOptionChecked;
}

COptionalPropertySheet::~COptionalPropertySheet()
{
}


BEGIN_MESSAGE_MAP(COptionalPropertySheet, CHHelpPropertySheet)
//{{AFX_MSG_MAP(COptionalPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionalPropertySheet message handlers

/// OnInitDialog virtual override, create and add the option checkbox
BOOL COptionalPropertySheet::OnInitDialog() 
{
	CHHelpPropertySheet::OnInitDialog();
	
	// Add the option check box
	CRect rectWnd;
	GetOptionCheckRect(rectWnd);

	m_optionCheck.Create(FormatHideOptionText(), 
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_TABSTOP, 
		rectWnd, this, IDC_OPTIONCHECK_BUTTON);
	
	m_optionCheck.SetFont(GetFont());
	
	// Set the checkbox
	m_optionCheck.SetCheck(m_bOptionChecked);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionalPropertySheet::InitHelpID()
{
	SetCtrlHelpID(IDC_OPTIONCHECK_BUTTON, IDH_IDC_OPTIONCHECK_BUTTON);
}

/*!
	Calculate the rectangle for the option check box window
	\param lpRect Return rectangle for the option check box window
*/
void COptionalPropertySheet::GetOptionCheckRect(LPRECT lpRect)
{
	if( lpRect )
	{
		CWnd* pOkWnd = GetDlgItem(IDOK);
		if( pOkWnd )
		{
			CRect rectOkWnd;
			pOkWnd->GetWindowRect(rectOkWnd);
			
			CRect rectWnd;
			GetWindowRect(rectWnd);

			const int margin = rectWnd.bottom - rectOkWnd.bottom;
			const int height = IsLargeFonts() ? 19 : 16;
			const int width = rectOkWnd.left  - rectWnd.left - (margin * 2);

			GetClientRect(rectWnd);

			lpRect->left = rectWnd.left + margin;
			lpRect->right = lpRect->left + width;

			lpRect->bottom = rectWnd.bottom - margin - (rectOkWnd.Height() - height) / 4;
			lpRect->top = lpRect->bottom - height;
		}
		else
		{
			ASSERT(FALSE);	// Can't get the OK button window?

			CRect rectWnd;
			GetClientRect(rectWnd);
			
			rectWnd.bottom -= 13;
			rectWnd.left   += 7;
			rectWnd.top     = rectWnd.bottom - (IsLargeFonts() ? 19 : 16);
			rectWnd.right   = rectWnd.left   + (IsLargeFonts() ? 235 : 180);

			lpRect->bottom = rectWnd.bottom;
			lpRect->left = rectWnd.left;
			lpRect->top = rectWnd.top;
			lpRect->right = rectWnd.right;
		}
	}
	else
	{
		ASSERT(FALSE);	// Bad pointer
	}
}

/// DoModal virtual override, check the Shift key and the hide command option settings
INT_PTR COptionalPropertySheet::DoModal() 
{
	return !m_bOptionChecked || (m_bOptionChecked && IsHideCommandKeyPressed()) ? CHHelpPropertySheet::DoModal() : IDOK;
}

/// OnCommand virtual override, save the state of the hide command option checkbox if the OK button was pressed
BOOL COptionalPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if( wParam == IDOK )
	{
		// Save the check state of the display option
		m_bOptionChecked = (m_optionCheck.GetCheck() == 1);
	}
	
	return CHHelpPropertySheet::OnCommand(wParam, lParam);
}

/// Get the hide command option value
bool COptionalPropertySheet::GetHideCommandDlg()
{
	return m_bOptionChecked;
}
