// CJPagerCtrl.cpp : implementation file
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
 * $Revision: 1.3 $
 * $Archive: /CodeJockey/CJLibrary/CJPagerCtrl.cpp $
 *
 * $History: CJPagerCtrl.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 6/14/99    Time: 10:44p
 * Updated in $/CodeJockey/CJ60Lib
 * Cleaned up code to eliminate level 4 compilier warnings.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 10:45p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/02/99    Time: 10:44p
 * Updated in $/CodeJockey/CJ60Lib
 * Cleaned up class source, wrapped all pager messages.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/02/98   Time: 10:44p
 * Created in $/CodeJockey/CJ60Lib
 * First release version.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJPagerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJPagerCtrl

CCJPagerCtrl::CCJPagerCtrl()
{
   // initialize common controls
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_PAGESCROLLER_CLASS;
	VERIFY(InitCommonControlsEx(&icex));
}

CCJPagerCtrl::~CCJPagerCtrl()
{
}

IMPLEMENT_DYNAMIC(CCJPagerCtrl, CWnd)

BEGIN_MESSAGE_MAP(CCJPagerCtrl, CWnd)
	//{{AFX_MSG_MAP(CCJPagerCtrl)
	ON_NOTIFY_REFLECT_EX(PGN_SCROLL, OnPagerScroll)
	ON_NOTIFY_REFLECT_EX(PGN_CALCSIZE, OnPagerCalcSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCJPagerCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	CWnd* pWnd = this;
    return pWnd->Create(WC_PAGESCROLLER, NULL, dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CCJPagerCtrl message handlers

BOOL CCJPagerCtrl::OnPagerCalcSize(NMHDR* pNMPG, LRESULT* pResult)
{
	NMPGCALCSIZE* pNMPGCalcSize = reinterpret_cast<NMPGCALCSIZE*>(pNMPG);
	switch(pNMPGCalcSize->dwFlag)
    {
	case PGF_CALCWIDTH:
		pNMPGCalcSize->iWidth = m_nWidth;
		break;
		
	case PGF_CALCHEIGHT:
		pNMPGCalcSize->iHeight = m_nHeight;
        break;
	}
	
    *pResult = 0;
	return FALSE;	// Let parent handle message
}

BOOL CCJPagerCtrl::OnPagerScroll(NMHDR* /*pNMPGScroll*/, LRESULT* pResult)
{
    *pResult = 0;
	return FALSE;	// Let parent handle message
}
