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

// BrowserBar.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "BrowserBar.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrowserBar

IMPLEMENT_DYNCREATE(CBrowserBar, CView)

CBrowserBar::CBrowserBar() : m_browser(0L)
{
}

CBrowserBar::~CBrowserBar()
{
}


BEGIN_MESSAGE_MAP(CBrowserBar, CView)
	//{{AFX_MSG_MAP(CBrowserBar)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowserBar drawing

/////////////////////////////////////////////////////////////////////////////
// CBrowserBar diagnostics

#ifdef _DEBUG
void CBrowserBar::AssertValid() const
{
	CView::AssertValid();
}

void CBrowserBar::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrowserBar message handlers

void CBrowserBar::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

BOOL CBrowserBar::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL res = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	if(!res)
		return FALSE;	

	m_browser = (CWinCvsBrowser *)RUNTIME_CLASS(CWinCvsBrowser)->CreateObject();
	VERIFY(m_browser->Create(0L, "", WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,0,0), this, 0));

	return TRUE;
}

void CBrowserBar::OnWindowPosChanged(WINDOWPOS FAR* wp) 
{
	CView::OnWindowPosChanged(wp);

	//m_browser->SetWindowPos(NULL, wp->x, wp->y, wp->cx, wp->cy, SWP_NOZORDER);
	CRect r;
	GetClientRect(&r);
	if(m_browser != 0L)
		m_browser->SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER);
}

void CBrowserBar::OnDestroy() 
{
	if(m_browser)
		m_browser->DestroyWindow();

	CView::OnDestroy();
}

void CBrowserBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect r;
	GetClientRect(&r);
	CBrush brush(GetSysColor(COLOR_WINDOW));
	dc.FillRect(r, &brush);
}

/// OnActivateView virtual override, make accelerators work
void CBrowserBar::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// Trick the frame into believing there is no active view
	// Need this for MDI messages (Ctrl+Tab, Ctrl+F4, ...) to be translated correctly
	if( bActivate )
	{
		if( CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd() )
		{
			pMainFrame->SetActiveView(NULL);
		}
	}

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
