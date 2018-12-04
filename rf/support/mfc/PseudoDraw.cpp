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

// PseudoDraw.cpp : implementation file
//

#include "stdafx.h"

#include "PseudoDraw.h"
#include "uwidget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPseudoDraw

CPseudoDraw::CPseudoDraw(UWidget *pseudo, int cmdid) : m_pseudo(pseudo), m_cmdid(cmdid)
{
}

CPseudoDraw::~CPseudoDraw()
{
}


BEGIN_MESSAGE_MAP(CPseudoDraw, CStatic)
	//{{AFX_MSG_MAP(CPseudoDraw)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPseudoDraw message handlers

void CPseudoDraw::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(m_pseudo != 0L)
		UEventSendMessage(m_pseudo->GetWidID(), EV_CUSTOM_DRAW, m_cmdid, &dc);
}
