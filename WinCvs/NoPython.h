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

#if !defined(AFX_NOPYTHON_H__F675DE42_0FA7_4785_8946_E8FC861BBA57__INCLUDED_)
#define AFX_NOPYTHON_H__F675DE42_0FA7_4785_8946_E8FC861BBA57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NoPython.h : header file
//

#include "HHelpSupport.h"
#include "HtmlStatic.h"

void WarnNoPython(void);

/////////////////////////////////////////////////////////////////////////////
// CNoPython dialog

class CNoPython : public CHHelpDialog
{
// Construction
public:
	CNoPython(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNoPython)
	enum { IDD = IDD_NOPYTHON };
	CHtmlStatic m_url;
	BOOL	m_remind;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoPython)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNoPython)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOPYTHON_H__F675DE42_0FA7_4785_8946_E8FC861BBA57__INCLUDED_)
