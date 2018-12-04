// CJMiniDockFrameWnd.cpp : implementation file
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
 * $Archive: /CodeJockey/CJLibrary/CJMiniDockFrameWnd.cpp $
 *
 * $History: CJMiniDockFrameWnd.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 6/09/99    Time: 8:33p
 * Updated in $/CodeJockey/CJ60Lib
 * Ioannhs Stamatopoulos (Yiannhs) [ystamat@mail.datamedia.gr] - Extended
 * docking windows, removed ::GetSysColor(...) calls and WM_SYSCOLORCHANGE
 * message handler, this is now automatically handled by the base class.
 * Added redraw flags to the create method and modified paint and layout
 * handlers to reduce flicker and un-necessary command handling. Dragging
 * rect now reflects actual control bar docked size. Added "flat" look to
 * the control bars, the control bars can now use the classic DevStudio
 * look, or use flat gradient gripper, borders and frame buttons. Overall,
 * docking windows now handle dragging, docking and floating much better,
 * and behave more like DevStudio.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/14/99    Time: 10:40p
 * Updated in $/CodeJockey/CJ60Lib
 * Daniel Harth [danielh@mail.matco.ro] - Added support for diagonal
 * sizing.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 10:39p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 10:39p
 * Created in $/CodeJockey/CJ60Lib
 * Initial release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJMiniDockFrameWnd.h"
#include "CJDockContext.h"
#include "CJSizeDockBar.h"
#include "CJControlBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJMiniDockFrameWnd

CCJMiniDockFrameWnd::CCJMiniDockFrameWnd()
{
	// TODO: add construction code here.
}

CCJMiniDockFrameWnd::~CCJMiniDockFrameWnd()
{
	// TODO: add destruction code here.
}

IMPLEMENT_DYNCREATE(CCJMiniDockFrameWnd, CMiniDockFrameWnd)

BEGIN_MESSAGE_MAP(CCJMiniDockFrameWnd, CMiniDockFrameWnd)
	//{{AFX_MSG_MAP(CCJMiniDockFrameWnd)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_MBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJMiniDockFrameWnd message handlers

void CCJMiniDockFrameWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	CCJSizeDockBar *pSizeBar = (CCJSizeDockBar*)&m_wndDockBar;
	
	if (nHitTest == HTCAPTION)
	{
		ActivateTopParent();
		if ((m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0)
		{
			int nPos = 1;
			CControlBar* pBar = NULL;
			while(pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize())
				pBar = pSizeBar->GetDockedSizeBar(nPos++);
			ASSERT(pBar != NULL);
			ASSERT_KINDOF(CControlBar, pBar);
			ASSERT(pBar->m_pDockContext != NULL);
			
			if (!pBar->IsKindOf(RUNTIME_CLASS(CCJControlBar))) {
				CMiniDockFrameWnd::OnNcLButtonDown(nHitTest,point);
				return;
			}
            
            //Yiannhs: This has to be fixed!! when the bar is floating
            //         and is over a visible dockbar, it arranges it inside
            //         the dock bar and does not drag, by just clicking on the caption
			((CCJDockContext *)pBar->m_pDockContext)->StartDragDockBar(point);
			pBar->SetFocus();
			return;
		}
	}

	else if (nHitTest >= HTSIZEFIRST && nHitTest <= HTSIZELAST)
	{
		ActivateTopParent();
		int nPos = 1;
		CControlBar* pBar = NULL;
		while(pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize())
			pBar = pSizeBar->GetDockedSizeBar(nPos++);
		
		ASSERT(pBar != NULL);
		ASSERT_KINDOF(CControlBar, pBar);
		ASSERT(pBar->m_pDockContext != NULL);
		ASSERT((m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0);
		
		if (!pBar->IsKindOf(RUNTIME_CLASS(CCJControlBar))) {
			CMiniDockFrameWnd::OnNcLButtonDown(nHitTest,point);
			return;
		}

		pBar->m_pDockContext->StartResize(nHitTest, point);
		return;
	}

    else {  //Yiannhs
	    CMiniFrameWnd::OnNcLButtonDown(nHitTest, point);
    }
}

void CCJMiniDockFrameWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint /*point*/) 
{
	if (nHitTest == HTCAPTION)
	{
		ActivateTopParent();
		CCJSizeDockBar * m_pSizeBar = (CCJSizeDockBar *)&m_wndDockBar;
		
		if ((m_pSizeBar->m_dwStyle & CBRS_FLOAT_MULTI) == 0)
		{
			int nPos = 1;
			CControlBar* pBar = NULL;
			while(pBar == NULL && nPos < m_pSizeBar->m_arrBars.GetSize())
				pBar = m_pSizeBar->GetDockedSizeBar(nPos++);
			
			ASSERT(pBar != NULL);
			ASSERT_KINDOF(CControlBar, pBar);
			ASSERT(pBar->m_pDockContext != NULL);

			if (pBar->IsKindOf(RUNTIME_CLASS(CCJControlBar)))
				pBar->m_pDockContext->ToggleDocking();
			
			return;
		}
	}
}

BOOL CCJMiniDockFrameWnd::Create(CWnd* pParent, DWORD dwBarStyle)
{
	// Let the base class do all the work.
	if (!CMiniDockFrameWnd::Create(pParent, dwBarStyle))
		return FALSE;

	// remove the MFS_4THICKFRAME style to allow diagonal
	// resizing.
	ModifyStyle(MFS_4THICKFRAME, 0);

	return TRUE;
}
