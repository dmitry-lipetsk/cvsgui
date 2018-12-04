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

// GraphFrm.cpp : implementation of the CGraphFrame class
//

#include "stdafx.h"
#include "wincvs.h"

#include "GraphFrm.h"
#include "GraphView.h"
#include "WinCvsView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphFrame

IMPLEMENT_DYNCREATE(CGraphFrame, CCJMDIChildWnd)

BEGIN_MESSAGE_MAP(CGraphFrame, CCJMDIChildWnd)
	//{{AFX_MSG_MAP(CGraphFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphFrame construction/destruction

CGraphFrame::CGraphFrame()
{
	// TODO: add member initialization code here
	
}

CGraphFrame::~CGraphFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGraphFrame diagnostics

#ifdef _DEBUG
void CGraphFrame::AssertValid() const
{
	CCJMDIChildWnd::AssertValid();
}

void CGraphFrame::Dump(CDumpContext& dc) const
{
	CCJMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphFrame message handlers

BOOL CGraphFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->GetMainFrame()->OnBroadcastCmdMsg(nID, nCode, pExtra, pHandlerInfo) )
		return TRUE;

	return CCJMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
