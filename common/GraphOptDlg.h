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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- September 2001
 */

/*
 * GraphOptDlg.h --- graph options dialog
 */

#if !defined(AFX_GRAPHOPTDLG_H__E05908EC_20A6_43FE_93C7_0964E6ABB6B1__INCLUDED_)
#define AFX_GRAPHOPTDLG_H__E05908EC_20A6_43FE_93C7_0964E6ABB6B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#	include "ColorStatic.h"
#	include "HHelpSupport.h"
#endif /* WIN32 */

bool CompatGetGraphOptions(CPersistentInt& selColor, CPersistentInt& shadowColor, 
						   CPersistentInt& headerColor, CPersistentInt& tagColor, CPersistentInt& branchColor, 
						   CPersistentInt& nodeColor, CPersistentInt& deadNodeColor, CPersistentInt& userStateNodeColor,
						   CPersistentInt& mergeColor);


#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CGraphOptDlg dialog

class CGraphOptDlg : public CHHelpDialog
{
// Construction
public:
	CGraphOptDlg(CPersistentInt& selColor, CPersistentInt& shadowColor, 
		CPersistentInt& headerColor, CPersistentInt& tagColor, CPersistentInt& branchColor, 
		CPersistentInt& nodeColor, CPersistentInt& deadNodeColor, CPersistentInt& userStateNodeColor,
		CPersistentInt& mergeColor, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGraphOptDlg)
	enum { IDD = IDD_GRAPH_OPTIONS };
	CButton	m_apply;
	CColorStatic	m_rgbShadow;
	CColorStatic	m_rgbSelection;
	CColorStatic	m_rgbState;
	CColorStatic	m_rgbDead;
	CColorStatic	m_rgbNode;
	CColorStatic	m_rgbBranch;
	CColorStatic	m_rgbTag;
	CColorStatic	m_rgbHeader;
	CColorStatic	m_rgbMerge;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphOptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGraphOptDlg)
	virtual void OnOK();
	afx_msg void OnHeader();
	afx_msg void OnTag();
	afx_msg void OnBranch();
	afx_msg void OnNode();
	afx_msg void OnDead();
	afx_msg void OnState();
	afx_msg void OnSelection();
	afx_msg void OnShadow();
	afx_msg void OnReset();
	virtual void OnCancel();
	afx_msg void OnMerge();
	afx_msg void OnHelp();
	afx_msg void OnApplyNow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CPersistentInt& m_selColor;
	CPersistentInt& m_shadowColor;
	CPersistentInt& m_headerColor;
	CPersistentInt& m_tagColor;
	CPersistentInt& m_branchColor;
	CPersistentInt& m_nodeColor;
	CPersistentInt& m_deadNodeColor;
	CPersistentInt& m_userStateNodeColor;
	CPersistentInt& m_mergeColor;
	
	int m_origSelColor;
	int m_origShadowColor;
	int m_origHeaderColor;
	int m_origTagColor;
	int m_origBranchColor;
	int m_origNodeColor;
	int m_origDeadNodeColor;
	int m_origUserStateNodeColor;
	int m_origMergeColor;

	// Methods
	virtual void InitHelpID();

	void PromptForColor(CColorStatic& colorStatic);
	void GetAllColors();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /* WIN32 */

#endif // !defined(AFX_GRAPHOPTDLG_H__E05908EC_20A6_43FE_93C7_0964E6ABB6B1__INCLUDED_)
