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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- June 1999
 */

/*
 * InitDlg.cpp : the cvs init dialog
 */

#if !defined(AFX_INITDLG_H__155D02C3_2323_11D3_B1A6_004095463819__INCLUDED_)
#define AFX_INITDLG_H__155D02C3_2323_11D3_B1A6_004095463819__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#	include "HHelpSupport.h"
#	include "CvsrootEntryDlg.h"
#endif

bool CompatGetInit(bool& forceCvsroot, std::string& cvsroot);

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
// CInit_MAIN dialog

class CInit_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CInit_MAIN)

// Construction
public:
	CInit_MAIN(bool forceRoot, const char* cvsroot);

// Dialog Data
	//{{AFX_DATA(CInit_MAIN)
	enum { IDD = IDD_INIT_MAIN };
	CStatic	m_cvsrootEntryPlaceholder;
	CString	m_helpinit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInit_MAIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInit_MAIN)
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

#endif // !defined(AFX_INITDLG_H__155D02C3_2323_11D3_B1A6_004095463819__INCLUDED_)
