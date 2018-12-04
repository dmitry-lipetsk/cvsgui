// CJToolBarBase.h : header file
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
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Luis Barreira's article 'Classes for new IE4 controls'
// http://www.codeguru.com/controls/ie4_controls_classes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 2005/08/09 00:22:45 $
 * $Revision: 1.2 $
 * $Archive: /CodeJockey/Include/CJToolBarBase.h $
 *
 * $History: CJToolBarBase.h $
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:27p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:12a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:10p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:29p
 * Created in $/CodeJockey/Include
 * Copied from MFC v6 and techno preview for v5. Added to extend
 * functionality to Visual C++ 5.0 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CCJTOOLBARBASE_H__
#define __CCJTOOLBARBASE_H__

#ifdef _VC_VERSION_5
typedef struct tagAFX_OLDTOOLINFO
{
	UINT cbSize;
	UINT uFlags;
	HWND hwnd;
	UINT uId;
	RECT rect;
	HINSTANCE hinst;
	LPTSTR lpszText;
} AFX_OLDTOOLINFO;

#define CBRS_GRIPPER					0x00400000L
#endif // _VC_VERSION_5

// CCJToolBarBase is a CControlBar derived class whis is used 
// by the CCJToolBar class.
class _CJX_EXT_CLASS CCJToolBarBase : public CControlBar
{
	DECLARE_DYNAMIC(CCJToolBarBase)
		// Construction
protected:
	CCJToolBarBase();
	
// Attributes
	// getting and setting border space
	void SetBorders(LPCRECT lpRect);
	void SetBorders(int cxLeft = 0, int cyTop = 0, int cxRight = 0, int cyBottom = 0);
	CRect GetBorders() const;

// Implementation
public:
	bool	m_bInReBar;	// true if rebar is parent.
	bool	m_bExStyle;	// true if created with the CreateEx style.
	
	virtual ~CCJToolBarBase();
	virtual void DoPaint(CDC* pDC);
	void DrawGripper(CDC* pDC, const CRect& rect);
	
	// implementation helpers
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	void CalcInsideRect(CRect& rect, BOOL bHorz) const; // adjusts borders etc
	void EraseNonClient();
	void DrawBorders(CDC* pDC, CRect& rect);
	
	//{{AFX_MSG(CCJToolBarBase)
	afx_msg void OnPaint();
	afx_msg void OnWindowPosChanging(LPWINDOWPOS lpWndPos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// inline functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJToolBarBase::SetBorders(LPCRECT lpRect)
	{ SetBorders(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom); }

#endif //__CCJTOOLBARBASE_H__

/////////////////////////////////////////////////////////////////////////////

