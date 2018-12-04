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

// PseudoList.cpp : implementation file
//

#include "stdafx.h"
#include "PseudoList.h"
#include "umain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPseudoList

CPseudoList::CPseudoList(int wid, int cmd) : m_wid(wid), m_cmd(cmd)
{
}

CPseudoList::~CPseudoList()
{
}


BEGIN_MESSAGE_MAP(CPseudoList, CListCtrl)
	//{{AFX_MSG_MAP(CPseudoList)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPseudoList message handlers

void CPseudoList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	UINT pFlags;
	int nItem = HitTest(point, &pFlags);
	if(nItem != -1)
	{
		CString s = GetItemText(nItem, 0);

		UEventSendMessage(m_wid, EV_LIST_DBLCLICK, UMAKEINT(m_cmd, nItem),
			(void *)(const char *)s);
	}

	CListCtrl::OnLButtonDblClk(nFlags, point);
}
