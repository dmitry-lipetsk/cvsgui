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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

#if !defined(AFX_PSEUDODLG_H__9EDC5FE5_21FD_4ACE_9AEB_7A521350A2EE__INCLUDED_)
#define AFX_PSEUDODLG_H__9EDC5FE5_21FD_4ACE_9AEB_7A521350A2EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PseudoDlg.h : header file
//

class UWidget;

/////////////////////////////////////////////////////////////////////////////
// CPseudoDlg dialog

class CPseudoDlg : public CDialog
{
// Construction
public:
	CPseudoDlg(int dlgid, UWidget *pseudo, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPseudoDlg();

// Dialog Data
	//{{AFX_DATA(CPseudoDlg)
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPseudoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	UWidget *m_pseudo;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CPseudoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSEUDODLG_H__9EDC5FE5_21FD_4ACE_9AEB_7A521350A2EE__INCLUDED_)
