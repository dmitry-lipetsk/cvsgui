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

#if !defined(AFX_CVSROOTENTRYDLG_H__397EDB3A_4E1C_493F_857F_4153440C80D9__INCLUDED_)
#define AFX_CVSROOTENTRYDLG_H__397EDB3A_4E1C_493F_857F_4153440C80D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CvsrootEntryDlg.h : header file
//

#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CCvsrootEntryDlg dialog

class CCvsrootEntryDlg : public CHHelpPropertyPage
{
	DECLARE_DYNCREATE(CCvsrootEntryDlg)

// Construction
public:
	CCvsrootEntryDlg();
	~CCvsrootEntryDlg();

// Dialog Data
	//{{AFX_DATA(CCvsrootEntryDlg)
	enum { IDD = IDD_CVSROOT_ENTRY };
	CButton	m_forceRootCheck;
	CButton	m_selCvsrootButton;
	CSmartComboBox	m_cvsrootCombo;
	BOOL	m_forceRoot;
	CString	m_cvsroot;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCvsrootEntryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCvsrootEntryDlg)
	afx_msg void OnBrowseCvsroot();
	afx_msg void OnForceCvsroot();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();

public:
	// Interface
	BOOL Create(const CWnd* pWndInsertAfter, CDialog* pParentWnd, const bool setFocus = false, const bool updateData = true);
	void EnableControls(const BOOL enable);
	
	BOOL UpdateData(BOOL bSaveAndValidate, int& entryState, CString& entryText);

	BOOL GetForceCvsrootCheck() const;
	void GetCvsrootWindowText(CString& cvsroot) const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CVSROOTENTRYDLG_H__397EDB3A_4E1C_493F_857F_4153440C80D9__INCLUDED_)
