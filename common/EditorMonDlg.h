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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2001
 */

/*
 * EditorMonDlg.h --- editor monitor dialog
 */

#if !defined(AFX_EDITORMONDLG_H__5146BF16_567B_4017_A1FD_619A7BD07906__INCLUDED_)
#define AFX_EDITORMONDLG_H__5146BF16_567B_4017_A1FD_619A7BD07906__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditorMonDlg.h : header file
//

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

bool CompatGetEditorMsg(std::string& message);

/////////////////////////////////////////////////////////////////////////////
// CEditorMonDlg dialog

#ifdef WIN32

class CEditorMonDlg : public CHHelpDialog
{
// Construction
public:
	CEditorMonDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditorMonDlg)
	enum { IDD = IDD_EDITORMON };
	CString	m_tempFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorMonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditorMonDlg)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	UINT m_timer;

	// Methods
	virtual void InitHelpID();
	
public:
	// Interface
	HANDLE m_hProcess;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /* WIN32 */

#endif // !defined(AFX_EDITORMONDLG_H__5146BF16_567B_4017_A1FD_619A7BD07906__INCLUDED_)
