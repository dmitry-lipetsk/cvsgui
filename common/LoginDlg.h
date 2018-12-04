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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2002
 */

/*
 * LoginDlg.h : the cvs login dialog header file
 */

#if !defined(AFX_LOGIN_MAIN_H__CF191044_0332_477E_ABD2_13E56D643C14__INCLUDED_)
#define AFX_LOGIN_MAIN_H__CF191044_0332_477E_ABD2_13E56D643C14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#	include "HHelpSupport.h"
#	include "CvsrootEntryDlg.h"
#endif

bool CompatGetLogin(bool& forceCvsroot, std::string& cvsroot);

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
// CLogin_MAIN dialog

class CLogin_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CLogin_MAIN)

// Construction
public:
	CLogin_MAIN(bool forceRoot, const char* cvsroot);
	~CLogin_MAIN();

// Dialog Data
	//{{AFX_DATA(CLogin_MAIN)
	enum { IDD = IDD_LOGIN_MAIN };
	CStatic	m_cvsrootEntryPlaceholder;
	CString	m_helpLogin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogin_MAIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogin_MAIN)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CCvsrootEntryDlg m_cvsrootEntryDlg; /*!< CVSROOT entry child dialog */
	
	// Methods
	virtual void InitHelpID();

public:
	// Interface
	BOOL m_forceRoot;	/*!< Force CVSROOT flag */
	CString m_cvsroot;	/*!< CVSROOT string */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif

#endif // !defined(AFX_LOGIN_MAIN_H__CF191044_0332_477E_ABD2_13E56D643C14__INCLUDED_)
