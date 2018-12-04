// CJExplorerBar.h : header file
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
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 2005/08/09 00:22:44 $
 * $Revision: 1.2 $
 * $Archive: /CodeJock/Include/CJExplorerBar.h $
 *
 * $History: CJExplorerBar.h $
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/Include
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 7:23a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:17p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 4:23p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/31/99    Time: 4:23p
 * Updated in $/CodeJockey/Include
 * Made some cosmetic enhancements to more closely match the windows
 * explorer bar.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 4:22p
 * Created in $/CodeJockey/Include
 * Initial release
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJEXPLORERBAR_H__
#define __CJEXPLORERBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CJControlBar.h"
#include "CJCaption.h"

// CCJExplorerBar is a CCJControlBar derived class which is simular
// to the explorer bar that is seen in Windows or Internet Explorer.
class _CJX_EXT_CLASS CCJExplorerBar : public CCJControlBar
{
	DECLARE_DYNAMIC(CCJExplorerBar)

public:

	// Default constructor
	//
	CCJExplorerBar();

	// Virtual destructor
	//
	virtual ~CCJExplorerBar();

protected:

	CCJCaption		m_Caption;			// frame caption contains 
	CCJFlatButton	m_CaptionButton;	// close button located on frame
	CString			m_strCaption;		// text that is displayed in the caption
	HICON			m_hIcon;			// handle to button icon.

public:

	// this member function is used to create an explorer bar
	//
	virtual BOOL Create(
		// the parent window.
		CWnd* pParentWnd,
		// The ID of the control bar.
		UINT nID,
		// Points to a null-terminated character string that contains the 
		// control bar title.
		LPCTSTR lpszWindowName = NULL,
		// The default size the control bar is to be.
		CSize sizeDefault = CSize(200,100),
		// Specifies the window style attributes.
		DWORD dwStyle = CBRS_LEFT);

	// this member function is called by the control bar, and
	// can be overloaded in derived classes to return the rect
	// for the child window associated with the control bar.
	//
	virtual void GetChildRect(
		// reference to a CRect object which contains the size of
		// the child window associated with the control bar.
		CRect &rect);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJExplorerBar)
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJExplorerBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonClose();
	afx_msg void OnUpdateButtonClose(CCmdUI* pCmdUI);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJEXPLORERBAR_H__

