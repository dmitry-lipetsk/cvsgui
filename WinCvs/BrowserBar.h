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

#if !defined(AFX_BROWSERBAR_H__64721953_F9A8_11D2_BDEC_000000000000__INCLUDED_)
#define AFX_BROWSERBAR_H__64721953_F9A8_11D2_BDEC_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BrowserBar.h : header file
//

#include "WinCvsBrowser.h"

/////////////////////////////////////////////////////////////////////////////
// CBrowserBar view

class CBrowserBar : public CView
{
public:
	virtual ~CBrowserBar();

protected:
	CBrowserBar();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBrowserBar)

// Attributes
public:

// Operations
public:
	inline CWinCvsBrowser *GetBrowserView() { return m_browser; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserBar)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CWinCvsBrowser *m_browser;

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrowserBar)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSERBAR_H__64721953_F9A8_11D2_BDEC_000000000000__INCLUDED_)
