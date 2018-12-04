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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- June 2004
 */

// HHelpSupport.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "HHelpSupport.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// CHHelpSupport Class

CHHelpSupport::CHHelpSupport()
{
}

CHHelpSupport::~CHHelpSupport()
{
}

/*!
	Get the application help file path
	\return The application help file path
*/
LPCTSTR CHHelpSupport::GetHelpFilePath()
{
	LPCTSTR res = "";

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL); // No application object

	if( pApp )
	{
		res = pApp->m_pszHelpFilePath;
	}

	return res;
}

/*!
	Initialize HTML help support
	\note To be called from within CWinApp::InitInstance override
*/
void CHHelpSupport::InitHTMLHelp()
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL); // No application object

	if( pApp )
	{
		// Set the HTML Help file path
		char path[_MAX_PATH] = "";
		char drive[_MAX_DRIVE] = "";
		char dir[_MAX_DIR] = "";
		char fname[_MAX_FNAME] = "";
		char ext[_MAX_EXT] = "";
		
		_splitpath(pApp->m_pszHelpFilePath, drive, dir, fname, ext);
		_makepath(path, drive, dir, fname, ".chm");
		
		free((void*)pApp->m_pszHelpFilePath);
		pApp->m_pszHelpFilePath = _tcsdup(path);
	}
}

/*!
	Get the window
	\return Window
	\note Using RTTI to downcast itself to CWnd
*/
CWnd* CHHelpSupport::GetWnd()
{
	return dynamic_cast<CWnd*>(this);
}

/*!
	Set the controls help IDs
*/
void CHHelpSupport::InitHelpID()
{
}

/*!
	Create help IDs data
	\param helpIDsData Help IDs data
*/
void CHHelpSupport::CreateHelpIDsData(tHelpIDsData& helpIDsData) const
{
	for(tHelpIDMap::const_iterator it = m_hhelpIDs.begin(); it != m_hhelpIDs.end(); ++it)
	{
		helpIDsData.push_back((*it).first);
		helpIDsData.push_back((*it).second);
	}

	helpIDsData.push_back(helpTopicEnd);
	helpIDsData.push_back(helpTopicEnd);
}

/*!
	Check whether the control can have a help topic
	\param ctrlID Control ID
	\return TRUE if the control can heave a help topic, FALSE otherwise
*/
BOOL CHHelpSupport::IsHelpCtrl(const UINT ctrlID) const
{
	return ctrlID != 0 && ctrlID != IDC_STATIC;
}

/*!
	Check whether the control is set to be ignored
	\param ctrlID Control ID
	\return TRUE if the control is set to be ignored, FALSE otherwise
*/
BOOL CHHelpSupport::IsIgnoredCtrl(const UINT ctrlID) const
{
	tHelpIDMap::const_iterator it = m_hhelpIDs.find(ctrlID);
	
	return it != m_hhelpIDs.end() && (*it).second == helpTopicIgnore;
}

/*!
	OnHelpInfo handler
	\param pHelpInfo Help info
	\return TRUE if handled, FALSE otherwise
*/
BOOL CHHelpSupport::DoOnHelpInfo(const HELPINFO* pHelpInfo)
{
	if( !IsHelpCtrl(pHelpInfo->iCtrlId) || IsIgnoredCtrl(pHelpInfo->iCtrlId) )
	{
		return TRUE;
	}

	if( HELPINFO_WINDOW == pHelpInfo->iContextType )
	{
		tHelpIDsData helpIDs;
		CreateHelpIDsData(helpIDs);

		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle,
			CHHelpSupport::GetHelpFilePath(),
			HH_TP_HELP_WM_HELP,
			(DWORD)(LPVOID)&(*helpIDs.begin())) != NULL;
	}

	return TRUE;
}

/*!
	OnContextMenu handler
*/
void CHHelpSupport::DoOnContextMenu(const CWnd* pWnd, CPoint point)
{
	if( !IsHelpCtrl(pWnd->GetDlgCtrlID()) )
	{
		return;
	}

	CMenu contextMenu;
	contextMenu.CreatePopupMenu();
	contextMenu.InsertMenu(0, MF_BYPOSITION, 1, _i18n("&What's This?"));

	if( -1 == point.x && -1 == point.y )
	{
		// Application key pressed
		CRect rect;
		pWnd->GetWindowRect(rect);

		point.x = rect.TopLeft().x + rect.Height() / 2;
		point.y = rect.CenterPoint().y;
	}

    if( contextMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, 
		point.x, point.y, GetWnd()) )
    {
		tHelpIDsData helpIDs;
		CreateHelpIDsData(helpIDs);

		::HtmlHelp(pWnd->GetSafeHwnd(),
			CHHelpSupport::GetHelpFilePath(),
			HH_TP_HELP_CONTEXTMENU,
			(DWORD)(LPVOID)&(*helpIDs.begin()));
    }
}

/*!
	Enable context help for a given window
*/
void CHHelpSupport::EnableWndContextHelp()
{
	if( GetWnd() )
	{
		GetWnd()->ModifyStyleEx(0, WS_EX_CONTEXTHELP);
	}
}

/*!
	Show context-sensitive help
*/
void CHHelpSupport::ShowHelp()
{
	GetWnd()->SendMessage(WM_COMMANDHELP);
}

/*!
	Set the help topic ID for a given control ID
	\param ctrlID Control ID
	\param helpTopic Help topic ID, use helpTopicIgnore constant to disable context help for the control
	\return Previous control's topic ID, helpTopicIgnore if no previous topic ID was present
*/
DWORD CHHelpSupport::SetCtrlHelpID(const DWORD ctrlID, const DWORD helpTopic)
{
	DWORD prevHelpID = helpTopicIgnore;

	tHelpIDMap::iterator it = m_hhelpIDs.find(ctrlID);
	if( it != m_hhelpIDs.end() )
	{
		prevHelpID = (*it).second;
		m_hhelpIDs.erase(it);
	}

	m_hhelpIDs.insert(std::make_pair(ctrlID, helpTopic));

	return prevHelpID;
}

/////////////////////////////////////////////////////////////////////////////
// CHHelpPropertySheet

IMPLEMENT_DYNAMIC(CHHelpPropertySheet, CPropertySheet)

CHHelpPropertySheet::CHHelpPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CHHelpPropertySheet::CHHelpPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CHHelpPropertySheet::~CHHelpPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CHHelpPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CHHelpPropertySheet)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHHelpPropertySheet message handlers

/// OnInitDialog virtual override, enable window context help
BOOL CHHelpPropertySheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();
	
	// Extra initialization
	EnableWndContextHelp();
	InitHelpID();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/// InitHelpID virtual override, enable direct use of CHHelpPropertySheet
void CHHelpPropertySheet::InitHelpID()
{
}

/// WM_HELPINFO message handler
BOOL CHHelpPropertySheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if( GetTabControl()->GetSafeHwnd() == pHelpInfo->hItemHandle )
	{
		// Show the page's help
		ShowHelp();
		return TRUE;
	}

	if( DoOnHelpInfo(pHelpInfo) )
	{
		return TRUE;
	}

	return CPropertySheet::OnHelpInfo(pHelpInfo);
}

/// WM_CONTEXTMENU message handler
void CHHelpPropertySheet::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	DoOnContextMenu(pWnd, point);
}

/// DoModal virtual override, do modal count
INT_PTR CHHelpPropertySheet::DoModal() 
{
	CWincvsApp::CModalGuard modalGuard;
	
	return CPropertySheet::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CHHelpPropertyPage property page

IMPLEMENT_DYNAMIC(CHHelpPropertyPage, CPropertyPage)

CHHelpPropertyPage::CHHelpPropertyPage(UINT nIDTemplate, UINT nIDCaption/* = 0*/) 
	: CPropertyPage(nIDTemplate, nIDCaption)
{
}

CHHelpPropertyPage::CHHelpPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption/* = 0*/) 
	: CPropertyPage(lpszTemplateName, nIDCaption)
{
}

CHHelpPropertyPage::~CHHelpPropertyPage()
{
}

void CHHelpPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHHelpPropertyPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHHelpPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CHHelpPropertyPage)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHHelpPropertyPage message handlers

/// OnInitDialog virtual override, enable window context help
BOOL CHHelpPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Extra initialization
	EnableWndContextHelp();
	InitHelpID();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/// WM_HELPINFO message handler
BOOL CHHelpPropertyPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if( DoOnHelpInfo(pHelpInfo) )
	{
		return TRUE;
	}
	
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}

/// WM_CONTEXTMENU message handler
void CHHelpPropertyPage::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	DoOnContextMenu(pWnd, point);
}

/////////////////////////////////////////////////////////////////////////////
// CHHelpDialog dialog

CHHelpDialog::CHHelpDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd /*= NULL*/)
	: CDialog(lpszTemplateName, pParentWnd)
{
}

CHHelpDialog::CHHelpDialog(UINT nIDTemplate, CWnd* pParentWnd /*= NULL*/)
	: CDialog(nIDTemplate, pParentWnd)
{
}

void CHHelpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHHelpDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHHelpDialog, CDialog)
	//{{AFX_MSG_MAP(CHHelpDialog)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHHelpDialog message handlers

/// OnInitDialog virtual override, enable window context help
BOOL CHHelpDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Extra initialization
	EnableWndContextHelp();
	InitHelpID();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/// WM_HELPINFO message handler
BOOL CHHelpDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if( DoOnHelpInfo(pHelpInfo) )
	{
		return TRUE;
	}
	
	return CDialog::OnHelpInfo(pHelpInfo);
}

/// WM_CONTEXTMENU message handler
void CHHelpDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	DoOnContextMenu(pWnd, point);
}

/// DoModal virtual override, do modal count
INT_PTR CHHelpDialog::DoModal() 
{
	CWincvsApp::CModalGuard modalGuard;
	
	return CDialog::DoModal();
}
