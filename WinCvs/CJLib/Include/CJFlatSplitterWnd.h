// CJFlatSplitterWnd.h : header file
//
// This file is part of the CJLibrary for Visual C++ / MFC
// Copyright C 1998-1999 COdejock Software, All Rights Reserved.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 1, or (at your option)
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// Based on the CSplitterWndEx class written by Oleg G. Galkin.
// mailto:OlegGa@vest.msk.ru
// http://www.codeguru.com/splitter/show_hide_static_panes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 2005/08/09 00:22:44 $
 * $Revision: 1.2 $
 * $Archive: /CodeJock/Include/CJFlatSplitterWnd.h $
 *
 * $History: CJFlatSplitterWnd.h $
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:39p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:17p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:00p
 * Created in $/CodeJockey/Include
 * Class added to support flat splitters, removable splitter views and
 * view manipulation.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJFLATSPLITTERWND_H__
#define __CJFLATSPLITTERWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CCJFlatSplitterWnd is a CSplitterWnd derived class which adds the ability
// to hide and show splitter panes, based upon its index, and 
// to draw flat splitter windows simular to Oultook.
class _CJX_EXT_CLASS CCJFlatSplitterWnd : public CSplitterWnd
{
public:
	
	// Default constructor
	//
	CCJFlatSplitterWnd();

	// Virtual destructor
	//
	virtual ~CCJFlatSplitterWnd();

protected:

	int		m_nHiddenCol;		// Index of the hidden column.
	int		m_nHiddenRow;		// Index of the hidden row.
	BOOL	m_bFlatSplitter;	// true if the SPLS_FLATEDGE style was used during creation.

public:

	// call this member function to enable or disable flat splitters.
	//
	virtual void DisableFlatStyle(BOOL bDisable=TRUE);

	// this member function is called to switch, or swap a splitter
	// view with another.
	//
	virtual BOOL SwitchView(int nRow, int nCol, CView *pNewView);

	// this member function is called to replace an existing splitter
	// view with another.
	//
	virtual BOOL ReplaceView(int nRow, int nCol, CRuntimeClass *pViewClass);

	// this member function is called to show the column that was
	// previously hidden.
	//
	virtual void ShowColumn();

	// this member function will hide a column based upon its index.
	//
	virtual void HideColumn(int nColHide);

	// this member function is called to show the row that was
	// previously hidden.
	//
	virtual void ShowRow();

	// this member function will hide a row based upon its index.
	//
	virtual void HideRow(int nRowHide);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFlatSplitterWnd)
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJFlatSplitterWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJFlatSplitterWnd::DisableFlatStyle(BOOL bDisable)
	{ m_bFlatSplitter = !bDisable; }

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJFLATSPLITTERWND_H__
