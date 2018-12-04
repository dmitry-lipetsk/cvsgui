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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- August 2005
 */

#if !defined(AFX_HTMLSTATIC_H__A68F9C47_185C_4020_A5BF_EFEC166210AA__INCLUDED_)
#define AFX_HTMLSTATIC_H__A68F9C47_185C_4020_A5BF_EFEC166210AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HtmlStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHtmlStatic window

/// HTML link control
class CHtmlStatic : public CStatic
{
// Construction
public:
	CHtmlStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlStatic)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHtmlStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlStatic)
	afx_msg void OnClicked();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CString m_link;			/*!< Explicit link, if empty the window text will be used instead */
	COLORREF m_textColor;	/*!< Text color */
	HBRUSH m_ctrlBrush;		/*!< Control brush */
	CFont m_underlineFont;	/*!< Underline font */

	CToolTipCtrl m_tooltip;

	// Methods
	CString GetLink() const;

public:
	// Interface
	void SetLink(const char* link);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLSTATIC_H__A68F9C47_185C_4020_A5BF_EFEC166210AA__INCLUDED_)
