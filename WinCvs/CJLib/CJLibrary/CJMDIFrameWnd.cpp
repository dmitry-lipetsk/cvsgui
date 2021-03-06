// CJMDIFrameWnd.cpp : implementation file
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
 * $Date: 2006/02/20 19:41:16 $
 * $Revision: 1.3 $
 * $Archive: /CodeJock/CJLibrary/CJMDIFrameWnd.cpp $
 *
 * $History: CJMDIFrameWnd.cpp $
 * 
 * *****************  Version 17  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:04p
 * Updated in $/CodeJock/CJLibrary
 * Minor update to PreCreateWindow(...) for flicker free drawing.
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 10/29/99   Time: 12:43a
 * Updated in $/CodeJock/CJLibrary
 * Overloaded PreCreateWindow(...) for flicker free drawing.
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 11:55p
 * Updated in $/CodeJock/CJLibrary
 * Fixed possible access violations in NewMenu methods. Thanks to Gero
 * Kuehn [gero.kuehn@isc-soft.de] for help with this.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 6:15p
 * Updated in $/CodeJock/CJLibrary
 * Fixed bug with a document becomes the active document when the user
 * clicks inside a tab in the CCJTabControlBar to perform some operation.
 * When closing the application, MFC decides in CFrameWnd::OnClose() to
 * check if the 'active' document is modified before checking all other
 * documuents. If the active document is not modified and can close its
 * frame, it does not check other documents individually and shuts down
 * without asking the important 'modified save' questions. To fix this,
 * OnClose() was overriden to add a call to "SetActiveView(NULL)" just
 * before the base class OnClose is called. This will check all documents
 * asking the save question before closing the application. Thanks to
 * Simon Clark [spc@otleda.com] for help with this.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 10:05a
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) to help to eliminate screen flicker.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 10/06/99   Time: 6:21a
 * Updated in $/CodeJockey/CJLibrary
 * Added cool menu support for MDI applications that do not have a
 * document/view architecture. Added GetInitializedMenu() to
 * CCJMDIChildWnd which will initialize the menu for a given view,  and
 * overloaded InitializeMenu() in CCJMDIFrameWnd. Thanks to Golovanov
 * Michael [Golovanov@SoftLab.RU] for help with this.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 9/16/99    Time: 2:35p
 * Updated in $/CodeJockey/CJLibrary
 * Fixed bug with save / restore bar state, and with saved bar size
 * changing whenever the application starts.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 8:25p
 * Updated in $/CodeJockey/CJ60Lib
 * Slight modification made to popup menu code.
 * 
 * *****************  Version 7  *****************
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
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 5:23p
 * Updated in $/CodeJockey/CJ60Lib
 * Further modifications for CCJMenu support.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 2:13a
 * Updated in $/CodeJockey/CJ60Lib
 * Re-wrote CCJMenu releated methods so that multiple documents can be
 * added with associated menu support. Added typedef CList <CCJMenu*,
 * CCJMenu*> CMDIMenuList; so that menus pointers are stored in a list and
 * not member variables.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/25/99    Time: 10:37p
 * Updated in $/CodeJockey/CJ60Lib
 * Added support for Brent Corkum's BCMenu (CCJMenu), which is a replacement for the
 * CCoolMenu class.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 8:41p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/12/99    Time: 8:41p
 * Updated in $/CodeJockey/CJ60Lib
 * Total re-write to add side-by-side docking support for CCJControlBar
 * class.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 8:41p
 * Created in $/CodeJockey/CJ60Lib
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJMDIFrameWnd.h"
#include "CJDockBar.h"
#include "CJSizeDockBar.h"
#include "CJControlBar.h"
#include "CJMiniDockFrameWnd.h"
#include "CJToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJMDIFrameWnd

CCJMDIFrameWnd::CCJMDIFrameWnd()
{

}

CCJMDIFrameWnd::~CCJMDIFrameWnd()
{
	// Free up any allocated memory.
	while (!m_menuList.IsEmpty()) {
		CCJMenu* pMenu = m_menuList.RemoveTail();
		_delete(pMenu);
	}
}

IMPLEMENT_DYNCREATE(CCJMDIFrameWnd, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CCJMDIFrameWnd, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CCJMDIFrameWnd)
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_INITMENUPOPUP()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJMDIFrameWnd message handlers

// dwDockBarMap
DWORD CCJMDIFrameWnd::dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};

void CCJMDIFrameWnd::DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout(TRUE);
	
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP)				 ? AFX_IDW_DOCKBAR_TOP		: n;
	n = (dw&CBRS_ALIGN_BOTTOM	&& n==0) ? AFX_IDW_DOCKBAR_BOTTOM	: n;
	n = (dw&CBRS_ALIGN_LEFT		&& n==0) ? AFX_IDW_DOCKBAR_LEFT		: n;
	n = (dw&CBRS_ALIGN_RIGHT	&& n==0) ? AFX_IDW_DOCKBAR_RIGHT	: n;
	
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}

void CCJMDIFrameWnd::EnableDocking(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);
	CMDIFrameWnd::EnableDocking(dwDockStyle);
	
	for (int i = 0; i < 4; i++) {
		if (dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY) {
			CDockBar* pDock = (CDockBar*)GetControlBar(dwDockBarMap[i][0]);
			
			// make sure the dock bar is of correct type
			if( pDock || !pDock->IsKindOf(RUNTIME_CLASS(CCJDockBar)) ) {
				BOOL bNeedDelete = ! pDock->m_bAutoDelete;
				pDock->m_pDockSite->RemoveControlBar(pDock);
				pDock->m_pDockSite = 0;	// avoid problems in destroying the dockbar
				pDock->DestroyWindow();
				if( bNeedDelete )
					delete pDock;
				pDock = 0;
			}
			
			if( pDock == 0 ) {
				pDock = new CCJDockBar;
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
					dwDockBarMap[i][1], dwDockBarMap[i][0])) {
					AfxThrowResourceException();
				}
			}
		}
	}
}

void CCJMDIFrameWnd::RecalcLayout(BOOL bNotify) 
{
	CMDIFrameWnd::RecalcLayout(bNotify);
	RecalcAllExcept(NULL);
}

// Yiannhs changed this
void CCJMDIFrameWnd::EnableDockingSizeBar(DWORD dwDockStyle, DockFlatBorders FlatFlags )
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	m_pFloatingFrameClass = RUNTIME_CLASS(CCJMiniDockFrameWnd);
	for (int i = 0; i < 4; i++)
	{
		if (dwSizeBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CCJSizeDockBar* pDock = (CCJSizeDockBar*)GetControlBar(dwSizeBarMap[i][0]);
			if (pDock == NULL)
			{
				pDock = new CCJSizeDockBar;

				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
						dwSizeBarMap[i][1], dwSizeBarMap[i][0]))
				{
					AfxThrowResourceException();
				}
                //Yiannhs enable Flat Borders For dock Bars
                pDock->FlatBorder( (FlatFlags >> i) & 0x0001 ); 
			}
		}
	}
}

void CCJMDIFrameWnd::RecalcAllExcept(CCJSizeDockBar *pBar)
{
	for (int i = 0; i < 4; i++)
	{
		CCJSizeDockBar* pDock = (CCJSizeDockBar*)GetControlBar(dwSizeBarMap[i][0]);
		if (pDock != NULL && pDock != pBar)
		{
			pDock->CalcSizeBarLayout();	
		}
	}
}

void CCJMDIFrameWnd::DockSizeBar(CControlBar * pBar,CCJSizeDockBar *pDockBar, LPRECT lpRect)
{
	ASSERT(pBar != NULL);
	ASSERT(pBar->m_pDockContext != NULL);
	
	if( pDockBar == NULL )
	{
		for (int i = 0; i < 4; i++)
		{
			if ((dwSizeBarMap[i][1] & CBRS_ALIGN_ANY) == (pBar->m_dwStyle & CBRS_ALIGN_ANY))
			{
				pDockBar = (CCJSizeDockBar*)GetControlBar(dwSizeBarMap[i][0]);
				ASSERT(pDockBar != NULL);

				int nSize = 0;

				switch(i)
				{
				case 0: // horizontal dock
				case 1:
					nSize = ((CCJControlBar*)pBar)->GetHorzSize();
					break;
				case 2: // vertical dock
				case 3:
					nSize = ((CCJControlBar*)pBar)->GetVertSize();
					break;
				}

				pDockBar->SetActualSize( nSize );
				break;
			}
		}
	}
	
	ASSERT(pDockBar != NULL);
	ASSERT(m_listControlBars.Find(pBar) != NULL);
	ASSERT(pBar->m_pDockSite == this);
	((CCJControlBar*)pBar)->Normalize();
	pDockBar->DockControlBar(pBar, lpRect);
}

HMENU CCJMDIFrameWnd::NewMenu(UINT nMenuID, UINT nToolbarID)
{
	// Allocate a new CCJMenu object.
	CCJMenu* pMenu = new CCJMenu;
	if( pMenu == NULL ) {
		return NULL;
	}

	// Load the menu and toolbar resource.
	pMenu->LoadMenu(nMenuID);
#if 0
	if (nToolbarID) {
		pMenu->LoadToolbar(nToolbarID);
	}
#else
	UNREFERENCED_PARAMETER(nToolbarID);
#endif

	// Add the menu to the CList object.
	m_menuList.AddTail(pMenu);

	// Detach the menu.
	return(pMenu->Detach());
}

HMENU CCJMDIFrameWnd::NewMenu(UINT nMenuID, UINT* nToolbarIDs, int nSize)
{
	// Allocate a new CCJMenu object.
	CCJMenu* pMenu = new CCJMenu;
	if( pMenu == NULL ) {
		return NULL;
	}

	// Load the menu and toolbar resources.
	pMenu->LoadMenu(nMenuID);
#if 0
	if (nToolbarIDs) {
		pMenu->LoadToolbars(nToolbarIDs, nSize);
	}
#else
	UNREFERENCED_PARAMETER(nToolbarIDs);
	UNREFERENCED_PARAMETER(nSize);
#endif

	// Add the menu to the CList object.
	m_menuList.AddTail(pMenu);

	// Detach the menu.
	return(pMenu->Detach());
}

BOOL CCJMDIFrameWnd::InitializeMenu(CMultiDocTemplate* pDocTemplate, UINT nMenuID, UINT nToolBarID/*=NULL*/, UINT nDefaultMenuID/*=NULL*/)
{
	ASSERT(nMenuID);			// must be valid.
	
	// If no menus have been created yet, set the frame default,
	// this is the menu that is displayed when no views are active.
	if (m_menuList.IsEmpty()) {
		ASSERT(nDefaultMenuID != 0);
		m_hMenuDefault = NewMenu(nDefaultMenuID, nToolBarID);
		OnUpdateFrameMenu(m_hMenuDefault);
	}
	// Create the Doc/View menu. ONLY if there is a template passed.
	if( pDocTemplate )
	{
		ASSERT_VALID(pDocTemplate);	// must be valid.
		pDocTemplate->m_hMenuShared = NewMenu(nMenuID, nToolBarID);
	}
	return TRUE;
}

BOOL CCJMDIFrameWnd::InitializeMenu(CMultiDocTemplate* pDocTemplate, UINT nMenuID, UINT* nToolbarIDs/*=NULL*/, int nSize/*=NULL*/, UINT nDefaultMenuID/*=NULL*/)
{
	ASSERT(nMenuID);			// must be valid.

	// If no menus have been created yet, set the frame default,
	// this is the menu that is displayed when no views are active.
	if (m_menuList.IsEmpty()) {
		ASSERT(nDefaultMenuID != 0);
		m_hMenuDefault = NewMenu(nDefaultMenuID, nToolbarIDs, nSize);
		OnUpdateFrameMenu(m_hMenuDefault);
	}
	// Create the Doc/View menu. ONLY if there is a template passed.  
	if( pDocTemplate )
	{
		ASSERT_VALID(pDocTemplate);	// must be valid.
		pDocTemplate->m_hMenuShared = NewMenu(nMenuID, nToolbarIDs, nSize);
	}
	
	return TRUE;
}

BOOL CCJMDIFrameWnd::InitializeMenu( HMENU* phMenu,	UINT nMenuID, UINT nToolbarID/*=NULL*/)
{
	if( !phMenu ) {
		return FALSE;
	}

	ASSERT(nMenuID);			// must be valid.
	*phMenu = NewMenu( nMenuID, nToolbarID );
	
	return (BOOL)(*phMenu != NULL);
}

BOOL CCJMDIFrameWnd::InitializeMenu( HMENU* phMenu, UINT nMenuID, UINT* nToolbarIDs/*=NULL*/, int nSize/*=NULL*/)
{
	if( !phMenu ) {
		return FALSE;
	}

	ASSERT(nMenuID);			// must be valid.
	*phMenu = NewMenu(nMenuID, nToolbarIDs, nSize);
	
	return (BOOL)(*phMenu != NULL);
}

void CCJMDIFrameWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	if (lpMeasureItemStruct->CtlType == ODT_MENU)
	{
		if (IsMenu((HMENU)lpMeasureItemStruct->itemID)) {
			CMenu* pMenu = CMenu::FromHandle((HMENU)lpMeasureItemStruct->itemID);
			
			// Loop through all menus in list.
			for (POSITION pos = m_menuList.GetHeadPosition(); pos; m_menuList.GetNext(pos))
			{
				CCJMenu* pListMenu = m_menuList.GetAt(pos);

				if (::IsMenu(pListMenu->GetSafeHmenu())) {
					pListMenu->MeasureItem(lpMeasureItemStruct);
					return;
				}
			}
		}
	}

	CMDIFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CCJMDIFrameWnd::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	// Loop through all menus in list.
	for (POSITION pos = m_menuList.GetHeadPosition(); pos; m_menuList.GetNext(pos))
	{
		CCJMenu* pListMenu = m_menuList.GetAt(pos);

#if 0
		if(pListMenu->IsMenu(pMenu)) {
			return CCJMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
		}
#endif
	}

	return CMDIFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
}

void CCJMDIFrameWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	if(!bSysMenu)
	{
		// Loop through all menus in list.
		for (POSITION pos = m_menuList.GetHeadPosition(); pos; m_menuList.GetNext(pos))
		{
			CCJMenu* pListMenu = m_menuList.GetAt(pos);

#if 0
			if(pListMenu->IsMenu(pPopupMenu)) {
				CCJMenu::UpdateMenu(pPopupMenu);
				break;
			}
#endif
		}
	}
}

void CCJMDIFrameWnd::LoadBarState(LPCTSTR lpszProfileName)
{
	CCJSizeDockBar::m_bIsDockBar = true;
	CFrameWnd::LoadBarState(lpszProfileName);
	CCJSizeDockBar::m_bIsDockBar = false;
}

void CCJMDIFrameWnd::SaveBarState(LPCTSTR lpszProfileName) const
{
	CCJSizeDockBar::m_bIsDockBar = true;
	CFrameWnd::SaveBarState(lpszProfileName);
	CCJSizeDockBar::m_bIsDockBar = false;
}

void CCJMDIFrameWnd::OnClose() 
{
	// Clear active view before close
	SetActiveView (NULL);
	CMDIFrameWnd::OnClose();
}

// Default implementation.
BOOL CCJMDIFrameWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CMDIFrameWnd::PreCreateWindow(cs);
}

////////////////
// Overloaded for flicker-free drawing with no CS_VREDRAW and CS_HREDRAW.
// Override in CMainFrame and call this method, rather than the default. Pass
// in your apps icon resource usually IDR_MAINFRAME.
//
BOOL CCJMDIFrameWnd::PreCreateWindow(CREATESTRUCT& cs, UINT nIconID) 
{
	// Register wnd class with no redraw, cursor (use default), background
	// brush and load your apps icon resource.
	cs.lpszClass = AfxRegisterWndClass( 0, NULL, NULL,
		AfxGetApp()->LoadIcon(nIconID));
	ASSERT(cs.lpszClass);
	
	return CMDIFrameWnd::PreCreateWindow(cs);
}
