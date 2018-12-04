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

#if !defined(AFX_EXPLOREBAR_H__A132A164_1A90_11D3_B1A1_004095463819__INCLUDED_)
#define AFX_EXPLOREBAR_H__A132A164_1A90_11D3_B1A1_004095463819__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExploreBar.h : header file
//

#include "CJShellTree.h"

/////////////////////////////////////////////////////////////////////////////
// CExploreBar view

class CMyShellTree : public CCJShellTree
{
	DECLARE_DYNAMIC(CMyShellTree)

protected:
	bool DisableCommon();

	//{{AFX_MSG(CMyShellTree)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CExploreBar : public CView
{
protected:
	CExploreBar();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CExploreBar)

// Attributes
public:
	CMyShellTree  m_TreeCtrl;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExploreBar)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CExploreBar();
	void OnUpdateCmd(CCmdUI* pCmdUI);
	bool DisableCommon();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CExploreBar)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnViewCheckout();
	afx_msg void OnUpdateViewCheckout(CCmdUI* pCmdUI);
	afx_msg void OnViewImport();
	afx_msg void OnUpdateViewImport(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPLOREBAR_H__A132A164_1A90_11D3_B1A1_004095463819__INCLUDED_)
