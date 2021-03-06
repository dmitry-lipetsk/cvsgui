// CJMiniDockFrameWnd.h : header file
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
 * $Date: 2005/08/09 00:22:45 $
 * $Revision: 1.2 $
 * $Archive: /CodeJockey/Include/CJMiniDockFrameWnd.h $
 *
 * $History: CJMiniDockFrameWnd.h $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:13p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 6/14/99    Time: 10:46p
 * Updated in $/CodeJockey/Include
 * Cleaned up code to eliminate level 4 compilier warnings.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/14/99    Time: 10:40p
 * Updated in $/CodeJockey/Include
 * Daniel Harth [danielh@mail.matco.ro] - Added support for diagonal
 * sizing.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 10:39p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 10:39p
 * Created in $/CodeJockey/Include
 * Initial release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJMINIDOCKFRAMEWND_H__
#define __CJMINIDOCKFRAMEWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CCJMiniDockFrameWnd is a CMiniDockFrameWnd class which is used to
// handle docking for the CCJControlBar class.
class _CJX_EXT_CLASS CCJMiniDockFrameWnd : public CMiniDockFrameWnd
{
	DECLARE_DYNCREATE(CCJMiniDockFrameWnd)

public:

	// Default constructor
	//
	CCJMiniDockFrameWnd();

	// Virtual destructor
	//
	virtual ~CCJMiniDockFrameWnd();

protected:

	// make CDockBar a friend to this class for access to member data
	//
	friend class CDockBar;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJMiniDockFrameWnd)
	public:
	virtual BOOL Create(CWnd* pParent, DWORD dwBarStyle);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJMiniDockFrameWnd)
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJMINIDOCKFRAMEWND_H__

