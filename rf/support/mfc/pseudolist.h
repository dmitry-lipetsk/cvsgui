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

#if !defined(AFX_PSEUDOLIST_H__49AF86B3_E6AB_4885_843C_1CE5AC8C4980__INCLUDED_)
#define AFX_PSEUDOLIST_H__49AF86B3_E6AB_4885_843C_1CE5AC8C4980__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PseudoList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPseudoList window

class CPseudoList : public CListCtrl
{
// Construction
public:
	CPseudoList(int wid, int cmd);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPseudoList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPseudoList();

	// Generated message map functions
protected:
	int m_cmd, m_wid;

	//{{AFX_MSG(CPseudoList)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSEUDOLIST_H__49AF86B3_E6AB_4885_843C_1CE5AC8C4980__INCLUDED_)
