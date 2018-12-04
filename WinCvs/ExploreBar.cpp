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

// ExploreBar.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "MainFrm.h"
#include "ExploreBar.h"
#include "BrowseFileView.h"
#include "FileTraversal.h"
#include "CvsCommands.h"
#include "CvsPrefs.h"
#include "BrowseViewHandlers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMyShellTree, CCJShellTree)

BEGIN_MESSAGE_MAP(CMyShellTree, CCJShellTree)
	//{{AFX_MSG_MAP(CMyShellTree)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteitem)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CMyShellTree::DisableCommon()
{
	CWincvsApp *app = (CWincvsApp*)AfxGetApp();
	return app->IsCvsRunning();
}

void CMyShellTree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(DisableCommon())
	{
		*pResult = 1;
		return;
	}

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CCJShellTree::OnItemexpanding(pNMHDR,pResult);
	CCJShellTree::Invalidate();
	*pResult = 0;
}

void CMyShellTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(DisableCommon())
	{
		*pResult = 1;
		return;
	}

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CString szPath;
	CCJShellTree::OnSelchanged(pNMHDR,pResult);

	if(CCJShellTree::GetSelectedFolderPath(szPath))
	{
		CWincvsApp* app = (CWincvsApp *)AfxGetApp();
		CBrowseFileView *view = app->GetFileView();
		if(view != 0L)
			view->ResetView(szPath);
	}

	*pResult = 0;
}

void CMyShellTree::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(DisableCommon())
	{
		*pResult = 1;
		return;
	}

	// TODO: Add your control notification handler code here
	CCJShellTree::OnRclick(pNMHDR,pResult);
	*pResult = 0;
}

void CMyShellTree::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(DisableCommon())
	{
		*pResult = 1;
		return;
	}

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CCJShellTree::OnDeleteitem(pNMHDR,pResult);
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CExploreBar

IMPLEMENT_DYNCREATE(CExploreBar, CView)

CExploreBar::CExploreBar()
{
}

CExploreBar::~CExploreBar()
{
}


BEGIN_MESSAGE_MAP(CExploreBar, CView)
	//{{AFX_MSG_MAP(CExploreBar)
	ON_WM_WINDOWPOSCHANGED()
	ON_COMMAND(ID_VIEW_CHECKOUT, OnViewCheckout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHECKOUT, OnUpdateViewCheckout)
	ON_COMMAND(ID_VIEW_IMPORT, OnViewImport)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IMPORT, OnUpdateViewImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExploreBar drawing

void CExploreBar::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CExploreBar diagnostics

#ifdef _DEBUG
void CExploreBar::AssertValid() const
{
	CView::AssertValid();
}

void CExploreBar::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExploreBar message handlers

BOOL CExploreBar::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL res = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	if(res)
	{
		if (!m_TreeCtrl.Create(
			WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
			CRect(0,0,0,0), this, IDC_BAR_TREE))
		{
			TRACE0("Unable to create caption button.\n");
			return FALSE;
		}
	}

	//m_TreeCtrl.EnableImages();
	//m_TreeCtrl.PopulateTree();

	//SetChild(&m_TreeCtrl);

	return res;
}

void CExploreBar::OnWindowPosChanged(WINDOWPOS FAR* wp) 
{
	CView::OnWindowPosChanged(wp);

	CRect r;
	GetClientRect(&r);
	m_TreeCtrl.SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER);
}

void CExploreBar::OnUpdateCmd(CCmdUI* pCmdUI)
{
	CWincvsApp* app = (CWincvsApp *)AfxGetApp();
	if(DisableCommon())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	CString folderPath;
	if(!m_TreeCtrl.GetSelectedFolderPath(folderPath) || folderPath.IsEmpty())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	pCmdUI->Enable(TRUE);
}

bool CExploreBar::DisableCommon()
{
	CWincvsApp *app = (CWincvsApp *)AfxGetApp();
	return app->IsCvsRunning();
}

void CExploreBar::OnViewCheckout() 
{
	CString folderPath;
	if( m_TreeCtrl.GetSelectedFolderPath(folderPath) && !folderPath.IsEmpty() )
	{
		KoCheckoutHandler handler;
		handler.OnFolder(folderPath);
	}
}

void CExploreBar::OnUpdateViewCheckout(CCmdUI* pCmdUI) 
{
	OnUpdateCmd(pCmdUI);
	pCmdUI->SetText("Chec&kout module to selection...");
}

void CExploreBar::OnViewImport() 
{
	CString folderPath;
	if( m_TreeCtrl.GetSelectedFolderPath(folderPath) && !folderPath.IsEmpty() )
	{
		KoImportHandler handler;
		handler.OnFolder(folderPath);
	}
}

void CExploreBar::OnUpdateViewImport(CCmdUI* pCmdUI) 
{
	OnUpdateCmd(pCmdUI);
	pCmdUI->SetText("&Import module from selection...");
}
