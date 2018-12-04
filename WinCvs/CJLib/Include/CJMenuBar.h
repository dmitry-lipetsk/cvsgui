// CJMenuBar.h : header file
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
// Based on the code written by Paul DiLascia for MSJ.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 2005/08/09 00:22:45 $
 * $Revision: 1.2 $
 * $Archive: /CodeJockey/Include/CJMenuBar.h $
 *
 * $History: CJMenuBar.h $
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:13p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/15/99    Time: 12:34a
 * Created in $/CodeJockey/Include
 * Initial creation. Extended the original class written by Paul DiLascia
 * for MSJ. Class uses CCJMenu and fixes some bugs with the original
 * class.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJMENUBAR_H__
#define __CJMENUBAR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CJToolBar.h"
#if 0
#include "CJMenu.h"        // CCJMenu class declaration
#endif
#include "Subclass.h"

#define CCJMenu CMenu

//////////////////
// CCJMenuBar uses this private class to intercept messages on behalf
// of its owning frame, as well as the MDI client window. Conceptually,
// these should be two different hooks, but I want to save code.
//
class _CJX_EXT_CLASS CCJMenuBarFrameHook : public CSubclassWnd {
protected:
   friend class CCJMenuBar;
   CCJMenuBar* m_pMenuBar;
   CCJMenuBarFrameHook();
   ~CCJMenuBarFrameHook();
   BOOL Install(CCJMenuBar* pMenuBar, HWND hWndToHook);
   virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);
};

enum TRACKINGSTATE { // menubar has three states:
	TRACK_NONE = 0,   // * normal, not tracking anything
	TRACK_BUTTON,     // * tracking buttons (F10/Alt mode)
	TRACK_POPUP       // * tracking popups
};

//////////////////
// CCJMenuBar implements an Office 97-style menu bar. Use it the way you would
// a CToolBar, only you need not call LoadToolbar. All you have to do is
//
// * Create the CCJMenuBar from your OnCreate or OnCreateBands handler.
//
// * Call LoadMenu to load a menu. This will set your frame's menu to NULL.
//
// * Implemenent your frame's PreTranslateMessage function, to call
//   CCJMenuBar::TranslateFrameMessage. 
//
class _CJX_EXT_CLASS CCJMenuBar : public CCJToolBar
{
	DECLARE_DYNAMIC(CCJMenuBar)
// Construction
public:
	CCJMenuBar();
	virtual ~CCJMenuBar();

	virtual BOOL Create(CWnd* pParentWnd,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_TOOLBAR);

	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
		CRect rcBorders = CRect(0, 0, 0, 0),
		UINT nID = AFX_IDW_TOOLBAR);
	
// Attributes
public:

	BOOL				m_bAutoRemoveFrameMenu;	// set frame's menu to NULL

protected:

	friend class CCJMenuBarFrameHook;

	CCJMenuBarFrameHook m_frameHook;			// hooks frame window messages
	CStringArray		m_arStrings;			// array of menu item names
	int					m_nMenuID;				// menu resource id.
	CCJMenu				m_popupMenu;			// the menu.
	
	// menu tracking stuff:
	int					m_iPopupTracking;		// which popup I'm tracking if any
	int					m_iNewPopup;			// next menu to track
	BOOL				m_bProcessRightArrow;	// process l/r arrow keys?
	BOOL				m_bProcessLeftArrow;	// ...
	BOOL				m_bEscapeWasPressed;	// user pressed escape to exit menu
	CPoint				m_ptMouse;				// mouse location when tracking popup
	HMENU				m_hMenuTracking;		// current popup I'm tracking
	CFont				m_Font;					// menu font.

	TRACKINGSTATE		m_iTrackingState;		// current tracking state

// Operations
public:

	BOOL LoadMenu(UINT nID);
	CCJMenu* GetMenu();	// get current menu
	TRACKINGSTATE GetTrackingState(int& iPopup);
	static BOOL bTRACE;						 // set TRUE to see TRACE msgs

protected:

	// helpers
	void RecomputeToolbarSize();
	void RecomputeMenuLayout();
	void UpdateFont();
	int	GetNextOrPrevButton(int iButton, BOOL bPrev);
	void SetTrackingState(TRACKINGSTATE iState, int iButton=-1);
	void TrackPopup(int iButton);
	void ToggleTrackButtonMode();
	void CancelMenuAndTrackNewOne(int iButton);
	void OnMenuSelect(HMENU hmenu, UINT nItemID);
	CPoint ComputeMenuTrackPoint(const CRect& rcButn, TPMPARAMS& tpm);
	BOOL IsValidButton(int iButton) const;
	int HitTest(CPoint p) const;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJMenuBar)
	public:
	virtual BOOL TranslateFrameMessage(MSG* pMsg);
	protected:
	virtual BOOL OnMenuInput(MSG& m);	 // handle popup menu input
	virtual void OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJMenuBar)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateMenuButton(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	static LRESULT CALLBACK MenuInputFilter(int code, WPARAM wp, LPARAM lp);
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE BOOL CCJMenuBar::IsValidButton(int iButton) const
	{ return 0 <= iButton && iButton < GetButtonCount(); }

_CJXLIB_INLINE CCJMenu* CCJMenuBar::GetMenu()
	{ return &m_popupMenu; }

_CJXLIB_INLINE TRACKINGSTATE CCJMenuBar::GetTrackingState(int& iPopup)
	{ iPopup = m_iPopupTracking; return m_iTrackingState; }

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CJMENUBAR_H__
