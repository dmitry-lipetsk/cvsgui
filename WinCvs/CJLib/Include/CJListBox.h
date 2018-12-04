// CJListBox.h : header file
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
 * $Archive: /CodeJock/Include/CJListBox.h $
 *
 * $History: CJListBox.h $
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 9:36p
 * Created in $/CodeJock/Include
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJLISTBOX_H__
#define __CJLISTBOX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CCJListBox is a CListBox derived class which creates a
// flicker free listbox control.
class _CJX_EXT_CLASS CCJListBox : public CListBox
{
	DECLARE_DYNAMIC(CCJListBox)

// Construction
public:
	CCJListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJListBox();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJListBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJLISTBOX_H__

