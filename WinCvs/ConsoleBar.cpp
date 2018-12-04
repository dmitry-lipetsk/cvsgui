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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1999
 */

/*
 * ConsoleBar.cpp -- Bar which owns the console
 */

#include "stdafx.h"
#include "ConsoleBar.h"
#include "wincvsView.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBarConsole

CBarConsole::CBarConsole() : m_view(0L)
{
	// TODO: add construction code here.
}

CBarConsole::~CBarConsole()
{
	// TODO: add destruction code here.
}

BEGIN_MESSAGE_MAP(CBarConsole, CCJControlBar)
	//{{AFX_MSG_MAP(CBarConsole)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarConsole message handlers

int CBarConsole::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_view = (CWincvsView *)RUNTIME_CLASS(CWincvsView)->CreateObject();
	if(m_view == 0L)
	{
		TRACE0("Failed to create view for CBarConsole\n");
		return -1;
	}

	// TODO: Add your specialized creation code here
	if (!m_view->Create(NULL, "",WS_CHILD | WS_VISIBLE | WS_BORDER,
		CRect(0,0,0,0), this, 0/*IDC_LIST_CTRL_2*/))
	{
		TRACE0("Failed to create view for CMyBarLeft\n");
		return -1;
	}

	SetChild(m_view);

	return 0;
}
