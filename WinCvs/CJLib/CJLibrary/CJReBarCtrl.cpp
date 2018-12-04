// CJReBarCtrl.cpp : implementation file
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
 * $Date: 2005/08/09 00:22:44 $
 * $Revision: 1.2 $
 * $Archive: /CodeJockey/CJLibrary/CJReBarCtrl.cpp $
 *
 * $History: CJReBarCtrl.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:26p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:29p
 * Created in $/CodeJockey/CJLibrary
 * Copied from MFC v6 and techno preview for v5. Added to extend
 * functionality to Visual C++ 5.0 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJReBarCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJReBarCtrl

CCJReBarCtrl::CCJReBarCtrl()
{
}

BOOL CCJReBarCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
                        UINT nID)
{
	// initialize common controls
#if _MSC_VER >= 1200
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_COOL_REG));
#else
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_COOL_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));
#endif

	CWnd* pWnd = this;
	return pWnd->Create(REBARCLASSNAME, NULL, dwStyle, rect, pParentWnd, nID);
}

CImageList* CCJReBarCtrl::GetImageList() const
{
  REBARINFO rbi;
  rbi.cbSize = sizeof(rbi);
  rbi.fMask = RBIM_IMAGELIST;
  return GetBarInfo(&rbi) ? CImageList::FromHandle(rbi.himl) : NULL;
}

BOOL CCJReBarCtrl::SetImageList(CImageList* pImageList)
{
  REBARINFO rbi;
  rbi.cbSize = sizeof(rbi);
  rbi.fMask = RBIM_IMAGELIST;
  rbi.himl = (HIMAGELIST)pImageList->GetSafeHandle();
  return SetBarInfo(&rbi);
}

BOOL CCJReBarCtrl::GetColorScheme(COLORSCHEME* lpcs)
{
  lpcs->dwSize = sizeof(COLORSCHEME);
  return SendMessage(RB_GETCOLORSCHEME, 0, (LPARAM)lpcs);
}

void CCJReBarCtrl::SetColorScheme(const COLORSCHEME* lpcs)
{
  ((COLORSCHEME*)lpcs)->dwSize = sizeof(COLORSCHEME);
  SendMessage(RB_SETCOLORSCHEME, 0, (LPARAM)lpcs);
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CCJReBarCtrl, CWnd)

/////////////////////////////////////////////////////////////////////////////
