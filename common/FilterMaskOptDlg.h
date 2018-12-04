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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- August 2003
 */

/*
 * FilterMaskOptDlg.h --- filter mask options dialog
 */

#if !defined(AFX_FILTERMASKOPTDLG_H__8560F3E7_6DBF_4EB6_B33E_E438B3020000__INCLUDED_)
#define AFX_FILTERMASKOPTDLG_H__8560F3E7_6DBF_4EB6_B33E_E438B3020000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

bool CompatGetFilterMaskOptions(int& reactionSpeed);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CFilterMaskOptDlg dialog

class CFilterMaskOptDlg : public CHHelpDialog
{
// Construction
public:
	CFilterMaskOptDlg(const int reactionSpeed, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilterMaskOptDlg)
	enum { IDD = IDD_FILTERBAR_OPTIONS };
	CStatic	m_reactionSpeedStatic;
	CSpinButtonCtrl	m_reactionSpeedSpin;
	int		m_reactionSpeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterMaskOptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterMaskOptDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeReactionSpeed();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();

	void UpdateReactionSpeedInfo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /* WIN32 */

#endif // !defined(AFX_FILTERMASKOPTDLG_H__8560F3E7_6DBF_4EB6_B33E_E438B3020000__INCLUDED_)
