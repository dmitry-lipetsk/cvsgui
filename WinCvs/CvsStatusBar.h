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

#if !defined(AFX_CVSSTATUSBAR_H__F549ACF2_ECFE_4933_A05D_5FC8248C3CD4__INCLUDED_)
#define AFX_CVSSTATUSBAR_H__F549ACF2_ECFE_4933_A05D_5FC8248C3CD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CvsStatusBar.h : header file
//

/// Status bar pane id enum
typedef enum{
	kStatusMain,		/*!< Main pane */
	kStatusCVSROOT,		/*!< CVSROOT as per CVS/Root */
	kStatusRepository,	/*!< Repository location as per CVS/Repository */
	kStatusInfo			/*!< Information (running command animation and filtering indicator) */
} kStatusPaneId;

/////////////////////////////////////////////////////////////////////////////
// CCvsStatusBar window

class CCvsStatusBar : public CStatusBar
{
public:
// Construction
	CCvsStatusBar();
	virtual ~CCvsStatusBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCvsStatusBar)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CCvsStatusBar)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

#if _MSC_VER >= 1400
	afx_msg LRESULT OnNcHitTest(CPoint point);
#else
	afx_msg UINT OnNcHitTest(CPoint point);
#endif

	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CBitmap m_indic;
	int m_step;
	bool m_settimer;

	bool m_hasFilters;
	bool m_isShowIgnored;

	CToolTipCtrl m_tooltip;

	// Methods
	BOOL SetIndicators();
	void RefreshPane(kStatusPaneId pane);

	void DrawCvsRunningAnimation(const LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawTextRect(const LPDRAWITEMSTRUCT lpDrawItemStruct, const COLORREF color, const char* text);

public:
	// Interface
	void UpdatePathInfo(const char* path);
	void OnIgnoreChanged();
	void OnFilteringChanged();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CVSSTATUSBAR_H__F549ACF2_ECFE_4933_A05D_5FC8248C3CD4__INCLUDED_)
