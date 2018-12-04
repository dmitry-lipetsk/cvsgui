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

#if !defined(AFX_STATICDRAW_H__8DCC3165_D42E_4FAA_BCCF_F4328685535E__INCLUDED_)
#define AFX_STATICDRAW_H__8DCC3165_D42E_4FAA_BCCF_F4328685535E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PseudoDraw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPseudoDraw window

class UWidget;

class CPseudoDraw : public CStatic
{
// Construction
public:
	CPseudoDraw(UWidget *pseudo, int cmdid);

// Attributes
public:
	UWidget *m_pseudo;
	int m_cmdid;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPseudoDraw)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPseudoDraw();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPseudoDraw)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICDRAW_H__8DCC3165_D42E_4FAA_BCCF_F4328685535E__INCLUDED_)
