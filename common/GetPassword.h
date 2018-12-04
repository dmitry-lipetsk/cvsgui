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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

#if !defined(AFX_GETPASSWORD_H__5946A8C4_8517_11D1_8949_444553540000__INCLUDED_)
#define AFX_GETPASSWORD_H__5946A8C4_8517_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GetPassword.h : header file
//

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

char* CompatGetPassword(const char* prompt);

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
// GetPassword dialog

class GetPassword : public CHHelpDialog
{
// Construction
public:
	GetPassword(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(GetPassword)
	enum { IDD = IDD_PASSWRD };
	CString	m_passwd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GetPassword)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GetPassword)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_GETPASSWORD_H__5946A8C4_8517_11D1_8949_444553540000__INCLUDED_)
