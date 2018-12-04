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

#if !defined(AFX_OPTIONALPROPERTYSHEET_H__F82DC8BB_73CD_4AEA_8635_1A0932F24C7D__INCLUDED_)
#define AFX_OPTIONALPROPERTYSHEET_H__F82DC8BB_73CD_4AEA_8635_1A0932F24C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionalPropertySheet.h : header file
//

#include "HHelpSupport.h"

/////////////////////////////////////////////////////////////////////////////
// COptionalPropertySheet

class COptionalPropertySheet : public CHHelpPropertySheet
{
	DECLARE_DYNAMIC(COptionalPropertySheet)

// Construction
public:
	COptionalPropertySheet(bool bOptionChecked, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionalPropertySheet(bool bOptionChecked, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionalPropertySheet)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COptionalPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(COptionalPropertySheet)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	// Data members
	bool m_bOptionChecked; /*!< The initial state of the option check box */ 
	CButton m_optionCheck; /*!< The object wrapping the option check box */

	// Methods
	virtual void InitHelpID();
	
	void GetOptionCheckRect(LPRECT lpRect);

public:
	// Interface
	bool GetHideCommandDlg();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONALPROPERTYSHEET_H__F82DC8BB_73CD_4AEA_8635_1A0932F24C7D__INCLUDED_)
