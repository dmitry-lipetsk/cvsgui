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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

// ChildFrm.cpp : implementation of the CLogFrame class
//

#include "stdafx.h"
#include "wincvs.h"

#include "LogFrm.h"
#include "WinCvsBrowser.h"
#include "BrowseFileView.h"
#include "WincvsView.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// inline function(s)

/////////////////////////////////////////////////////////////////////////////
// CLogFrame

IMPLEMENT_DYNCREATE(CLogFrame, CCJMDIChildWnd)

BEGIN_MESSAGE_MAP(CLogFrame, CCJMDIChildWnd)
	//{{AFX_MSG_MAP(CLogFrame)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogFrame construction/destruction

CLogFrame::CLogFrame()
{
	// TODO: add member initialization code here
	
}

CLogFrame::~CLogFrame()
{
}

BOOL CLogFrame::OnCreateClient(LPCREATESTRUCT lpcs,
	 CCreateContext* pContext)
{
	BOOL res = CCJMDIChildWnd::OnCreateClient(lpcs, pContext);
	MDIMaximize();

	// We do not even want to tempt the user to destroy the file view
	// Gray out the menu item and the close button
	// (Has the side effect of showing the MDI system menu and the MDI
	//  close/minimize/restore buttons; they get shown anyway the first 
	//  time a graph window becomes visible)
	// This is nothing but cosmetics -- Ctrl+F4 still triggers
	// OnClose -> OnDestroy, so we still need to override OnClose
	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_GRAYED | MF_BYCOMMAND);

	return res;
}

/////////////////////////////////////////////////////////////////////////////
// CLogFrame diagnostics

#ifdef _DEBUG
void CLogFrame::AssertValid() const
{
	CCJMDIChildWnd::AssertValid();
}

void CLogFrame::Dump(CDumpContext& dc) const
{
	CCJMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLogFrame message handlers

void CLogFrame::OnClose() 
{
	// prevent from closing the main frame
	//CCJMDIChildWnd::OnClose();
}

BOOL CLogFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->GetMainFrame()->OnBroadcastCmdMsg(nID, nCode, pExtra, pHandlerInfo) )
		return TRUE;

	return CCJMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
