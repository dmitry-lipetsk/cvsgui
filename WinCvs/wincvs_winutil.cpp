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

#include "stdafx.h"
#include "wincvs_winutil.h"
#include "MultiString.h"
#include "wincvs.h"
#include "MainFrm.h"
#include "CustomizeMenus.h"
#include "CvsPrefs.h"
#include "CvsCommands.h"
#include "AppConsole.h"

using namespace std;


/// Maximum retry number for fixed unlink function
#define MAX_UNLINK_RETRY	50

/////////////////////////////////////////////////////////////////////////////
// helper functions declaration

CWnd* FindNextSiblingWindow(CWnd* pWnd, BOOL CheckTabStop);
CWnd* FindNextChildWindow(CWnd* pWnd, BOOL CheckTabStop);
CWnd* GetNextParentWindow(CWnd* pWnd);

CWnd* FindPrevSiblingWindow(CWnd* pWnd, BOOL CheckTabStop);
CWnd* GetPrevParentWindow(CWnd* pWnd);
CWnd* GetEndOfDecendantWindow(CWnd* pWnd);


/////////////////////////////////////////////////////////////////////////////
// inline function(s)

inline BOOL IsTargetWindow(CWnd* pWnd, BOOL CheckTabStop)
{
	return ((!CheckTabStop) || (WS_TABSTOP & pWnd->GetStyle()));
}

/////////////////////////////////////////////////////////////////////////////
// implementation

CWnd* FindNextWindow(CWnd* pWnd, BOOL CheckTabStop)
{
	ASSERT_VALID(pWnd);
	if (pWnd)
	{
		CWnd* pNextWnd;
		pNextWnd = FindNextChildWindow(pWnd, CheckTabStop);
		if (pNextWnd)
			return pNextWnd;

		pNextWnd = FindNextSiblingWindow(pWnd, CheckTabStop);
		if (pNextWnd)
			return pNextWnd;

		CWnd* pParentWnd = GetNextParentWindow(pWnd);
		while ((pParentWnd) && (pNextWnd != pWnd))
		{
			pNextWnd = FindNextChildWindow(pParentWnd, CheckTabStop);
			if (pNextWnd)
				return pNextWnd;
			pParentWnd = GetNextParentWindow(pParentWnd);
		}
	}
	return pWnd;
}

CWnd* GetNextParentWindow(CWnd* pWnd)
{
	if (pWnd)
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
			return pWnd->GetTopWindow();

		CFrameWnd* pFrameWnd = pWnd->GetParentFrame();
		ASSERT_VALID(pFrameWnd);
		CWnd* pParentWnd = pWnd->GetParent();
		ASSERT_VALID(pParentWnd);

		CWnd* pCrntWnd;
		BOOL bFound = FALSE;
		while ((!bFound) && (pParentWnd != pFrameWnd))
		{
			pCrntWnd = pParentWnd->GetNextWindow();
			if (pCrntWnd)
				bFound = TRUE;
			else
				pParentWnd = pParentWnd->GetParent();
		}
		return (bFound) ? pCrntWnd : pParentWnd->GetTopWindow();
	}
	return NULL;
}

CWnd* FindNextSiblingWindow(CWnd* pWnd, BOOL CheckTabStop)
{
	if (pWnd)
	{
		CWnd* pSiblingWnd = pWnd->GetNextWindow();
		while (pSiblingWnd)
		{
			if (IsTargetWindow(pSiblingWnd, CheckTabStop))
				return pSiblingWnd;
			CWnd* pChildWnd = FindNextChildWindow(pSiblingWnd, CheckTabStop);
			if (pChildWnd)
				return pChildWnd;
			pSiblingWnd = pSiblingWnd->GetNextWindow();
		}
	}
	return NULL;
}

CWnd* FindNextChildWindow(CWnd* pWnd, BOOL CheckTabStop)
{
	if (pWnd)
	{
		CWnd* pChildWnd = pWnd->GetTopWindow();
		while (pChildWnd)
		{
			if (IsTargetWindow(pChildWnd, CheckTabStop))
				return pChildWnd;
			CWnd* pSiblingWnd = FindNextSiblingWindow(pChildWnd, CheckTabStop);
			if (pSiblingWnd)
				return pSiblingWnd;
			pChildWnd = pChildWnd->GetTopWindow();
		}
	}
	return NULL;
}

CWnd* FindPrevWindow(CWnd* pWnd, BOOL CheckTabStop)
{
	ASSERT_VALID(pWnd);
	if (pWnd)
	{
		CWnd* pWndCrntLevel = pWnd;
		while (TRUE)
		{
			CWnd* pFoundWnd;
			pFoundWnd = FindPrevSiblingWindow(pWndCrntLevel, CheckTabStop);
			if (pFoundWnd)
				return pFoundWnd;
			pWndCrntLevel = GetPrevParentWindow(pWndCrntLevel);
			if (!pWndCrntLevel)
			{
				pFoundWnd = GetEndOfDecendantWindow(pWnd->GetParentFrame());
				if (IsTargetWindow(pFoundWnd, CheckTabStop))
					return pFoundWnd;
				pWndCrntLevel = pFoundWnd;
			}
		} 
	}
	return NULL;
}

CWnd* GetPrevParentWindow(CWnd* pWnd)
{
	if (pWnd)
	{
		if (!pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
		{
			CWnd* pParentWnd = pWnd->GetParent();
			ASSERT_VALID(pParentWnd);
			return pParentWnd;
		}
	}
	return NULL;
}

CWnd* FindPrevSiblingWindow(CWnd* pWnd, BOOL CheckTabStop)
{
	if (pWnd)
	{
		CWnd* pSiblingWnd = pWnd->GetWindow(GW_HWNDPREV);
		while (pSiblingWnd)
		{
			if (IsTargetWindow(pSiblingWnd, CheckTabStop))
				return pSiblingWnd;
			pSiblingWnd = pSiblingWnd->GetWindow(GW_HWNDPREV);
		}
	}
	return NULL;
}

CWnd* GetEndOfDecendantWindow(CWnd* pWnd)
{
	CWnd* pTmpWnd = pWnd;
	CWnd* pPreTmpWnd;
	while (pTmpWnd)
	{
		pPreTmpWnd = pTmpWnd;
		pTmpWnd = pTmpWnd->GetWindow(GW_CHILD);
		if (pTmpWnd)
		{
			pPreTmpWnd = pTmpWnd;
			pTmpWnd = pTmpWnd->GetWindow(GW_HWNDLAST);
		}
	}
	return pPreTmpWnd;
}

void UpdateUIMenu(CMenu* pMyMenu, CView* pThis)
{
	CCmdUI state;
	
	state.m_pMenu = pMyMenu;
	state.m_nIndexMax = pMyMenu->GetMenuItemCount();

	for(state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
	{
		state.m_nID = pMyMenu->GetMenuItemID(state.m_nIndex);
		if( state.m_nID == 0 )
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if( state.m_nID == (UINT)-1 )
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMyMenu->GetSubMenu(state.m_nIndex);
			if( state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1 )
			{
				continue;       // first item of popup can't be routed to
			}

			state.DoUpdate(pThis, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(pThis, TRUE);
		}

#	if 0 /* ??? */
		// adjust for menu deletions and additions
		UINT nCount = pMyMenu->GetMenuItemCount();
		if( nCount < state.m_nIndexMax )
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			
			while( state.m_nIndex < nCount && pMyMenu->GetMenuItemID(state.m_nIndex) == state.m_nID )
			{
				state.m_nIndex++;
			}
		}

		state.m_nIndexMax = nCount;
#	endif
	}
}

/*!
	Get the menu item position for given menu ID
	\param pMenu Menu
	\param nID Menu item ID
	\return Position (index) if the item is found, -1 if not found
*/
int GetMenuItemPos(const CMenu* pMenu, const UINT nID)
{
	int res = -1;

	const int itemCount = pMenu->GetMenuItemCount();
	for(int nIndex = 0; nIndex < itemCount; nIndex++)
	{
		if( pMenu->GetMenuItemID(nIndex) == nID )
		{
			res = nIndex;
			break;
		}
	}

	return res;
}

/// Return the menu which has the cmd nStartId
CMenu* GetSubCMenu(int nStartId, CView* pThis)
{
	CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
	ASSERT(pTopMenu != 0L);
	if( pTopMenu == 0L )
		return NULL;

	CMenu* pMyMenu=NULL;
	for(int iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
	{
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if( pMenu && pMenu->GetMenuItemID(0) == (UINT)nStartId )
		{
			pMyMenu = pMenu;
			break;
		}
	}

	ASSERT(pMyMenu != 0L);
	if( pMyMenu == 0L )
		return NULL;

	UpdateUIMenu(pMyMenu, pThis);

	return pMyMenu;
}

CMenu* _GetMenuEntryRecurs(CMenu* menu, int nStartId, int& pos)
{
	if( menu == 0L )
		return 0l;

	for(int iPos = 0, _c=menu->GetMenuItemCount(); iPos < _c; ++iPos)
	{
		const UINT id = menu->GetMenuItemID(iPos);
		if( id == (UINT)nStartId )
		{
			pos = iPos;
			return menu;
		}

		if( id == (UINT)-1 )
		{
			CMenu* const res = _GetMenuEntryRecurs(menu->GetSubMenu(iPos), nStartId, pos);
			if( res != 0L )
				return res;
		}
	}

	return 0L;
}

/// Return the menu and position of a cmd
CMenu* GetMenuEntry(int nStartId, int& pos)
{
	CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
	ASSERT(pTopMenu != NULL);

	return _GetMenuEntryRecurs(pTopMenu, nStartId, pos);
}

/// Get the popup menu given the popup menu string
CMenu* GetTopPopupMenu(const char* menuString)
{
	CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
	if( pTopMenu == 0L )
	{
		ASSERT(FALSE);	// No top level menu
		return NULL;
	}

	const int topMenuItemCount = (int)pTopMenu->GetMenuItemCount();
	CString popupString;
	
	for(int nIndex = 0; nIndex < topMenuItemCount; nIndex++)
	{
		pTopMenu->GetMenuString(nIndex, popupString, MF_BYPOSITION);
		if( menuString == popupString )
		{
			// Found
			return pTopMenu->GetSubMenu(nIndex);
		}
	}

	// Not found
	return NULL;
}

/*!
	Check whether Large Fonts are set in the display settings
	\return true if Large Fonts are set, false otherwise
	\note The actual testing is only done once, 
	but since OS requires a reboot after font's size change it should not be a problem
*/
bool IsLargeFonts()
{
	static bool bLargeFonts = false;
	static bool defined = false;

	if( !defined )
	{
		defined = true;
		
		HWND hDesktop = ::GetDesktopWindow();
		HDC hDesktopDC = ::GetDC(hDesktop);
		short nLogPixels = ::GetDeviceCaps(hDesktopDC, LOGPIXELSX);
		
		::ReleaseDC(hDesktop, hDesktopDC);
		
		bLargeFonts = (nLogPixels > 96);
	}

	return bLargeFonts;
}

/*!
	Check whether the hide command dialog key is pressed
	\return true if hiding key pressed, false otherwise
	\note The hiding key is determined in the preferences and it can be either Shift or Ctrl
*/
bool IsHideCommandKeyPressed()
{
	return ::GetKeyState(gCvsPrefs.HideCommandDlgUseShift() ? VK_SHIFT : VK_CONTROL) < 0;
}

/*!
	Check whether the Esc key is pressed or was pressed since the last check
	\return true if Esc key pressed, false otherwise
*/
bool IsEscKeyPressed()
{
	bool res = false;

	const SHORT escState = GetAsyncKeyState(VK_ESCAPE);
	if( (escState & 0x8000) || (escState & 0x0001) )
	{
		res = true;
	}

	return res;
}

/*!
	Set the extended style for the list control
	\param pListCtrl List control to set the style for
	\param dwExtendedStyle
	\return The previous extended style of the list control
*/
DWORD SetListCtrlExtendedStyle(CListCtrl* pListCtrl, const DWORD dwExtendedStyle)
{
	if( !pListCtrl )
	{
		ASSERT(FALSE);	// bad pointer
		return 0;
	}

	return pListCtrl->SetExtendedStyle(pListCtrl->GetExtendedStyle() | dwExtendedStyle);
}

/*!
	Enable the autocomplete 
	\param pWnd Edit control
	\param dwFlags Flag for autocomplete
	\return TRUE on success, FALSE otherwise
*/
BOOL EnableEditAutoComplete(CEdit* pWnd, const DWORD dwFlags /*= AUTOCOMPLETE_DEFFLAGS*/)
{
	return SUCCEEDED(SHAutoComplete(pWnd->GetSafeHwnd(), dwFlags));
}

/*!
	Post the message to set the focus in the dialog to given control window
	\param pDlg Dialog to post the message to
	\param pWndCtrl Control to set the focus
	\return true on success, false otherwise
*/
void PostGotoDlgCtrl(CDialog* pDlg, CWnd* pWndCtrl)
{ 
	ASSERT(::IsWindow(pDlg->GetSafeHwnd()));

	::PostMessage(pDlg->GetSafeHwnd(), WM_NEXTDLGCTL, (WPARAM)pWndCtrl->GetSafeHwnd(), 1L);
}

/*!
	Check the sandbox drive type and optionally print the warning if it's not a fixed local drive
	\param locationPath Location path to check
	\param quiet true to prevent error logging, false to report warnings
	\return true if location drive is acceptable, false otherwise
*/
bool CheckLocationDriveType(const char* locationPath, const bool quiet /*= false*/)
{
	bool res = false;

	char drive[_MAX_DRIVE] = { 0 };
	const char* warningMessage = "";

	_splitpath(locationPath, drive, NULL, NULL, NULL);

	if( strlen(drive) )
	{
		string rootPathName = drive;
		NormalizeFolderPath(rootPathName);

		const UINT driveType = GetDriveType(rootPathName.c_str());
		switch( driveType )
		{
		case DRIVE_FIXED:
			res = true;
			break;
		default:
			ASSERT(FALSE); // Incorrect return
		case DRIVE_UNKNOWN:
			warningMessage = "Warning: Unable to detect drive type";
			break;
		case DRIVE_NO_ROOT_DIR:
			warningMessage = "Warning: Invalid drive root path, there might be no volume mounted at the path";
			break;
		case DRIVE_REMOVABLE:
			warningMessage = "Warning: Using removable drive may cause performance decrease";
			break;
		case DRIVE_REMOTE:
			warningMessage = "Warning: Using remote (network) drive may cause file system errors and performance decrease";
			break;
		case DRIVE_CDROM:
			warningMessage = "Warning: Using CD-ROM drive may cause performance decrease";
			break;
		case DRIVE_RAMDISK:
			warningMessage = "Warning: Using RAM disk drive may cause file(s) loss";
			break;
		}
	}
	else
	{
		// UNC path
		if( strlen(locationPath) >= 2 && 
			locationPath[0] == '\\' && locationPath[1] == '\\' )
		{
			warningMessage = "Warning: Using remote (network) drive may cause file system errors and performance decrease";
		}
	}

	if( !res && !quiet && strlen(warningMessage) )
	{
		CColorConsole out;
		out << kRed << kBold << warningMessage << kNormal << kNL;
		out << kBlue << kBold << "Use local fixed drives for best safety and high performance" << kNormal << kNL;
	}

	return res;
}

/*!
	Get the application path
	\param path Return the application path
	\return true on success, false otherwise
*/
bool GetAppPath(std::string& path)
{
	bool res = false;

	path.erase();
	
	if( GetAppModule(path) )
	{
		string uppath, exefile;
		if( ::SplitPath(path.c_str(), uppath, exefile) )
		{
			path = uppath.c_str();
			res = true;
		}
	}

	return res;
}

/*!
	Get the application module path
	\param module Return the application module path
	\return true on success, false otherwise
*/
bool GetAppModule(std::string& module)
{
	bool res = false;

	module.erase();
	
	const HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( hInst != NULL )
	{
		char apath[_MAX_PATH];
		
		const DWORD len = ::GetModuleFileName(hInst, apath, _MAX_PATH);
		if( len > 0 )
		{
			module = apath;
			res = true;
		}
	}

	return res;
}

/*!
	Get the specified folder
	\param strFolder Return folder
	\param folder Folder to retrieve, as per SHGetFolderPath function
	\return true on success, false otherwise
	\note Wrapper for SHGetFolderPath
*/
bool GetFolder(std::string& strFolder, int folder)
{
	bool res = false;

	char aFolder[_MAX_PATH];

	if( SUCCEEDED(SHGetFolderPath(NULL, folder, NULL, 0, aFolder)) ) 
	{
		strFolder = aFolder;
		res = true;
	}
	
	return res;
}

/*!
	Get the application version
	\param vers Return the application version
	\return true on success, false otherwise
*/
bool GetAppVersion(std::string& vers)
{
	bool bRet = false;
	char szFullPath[_MAX_PATH]; 
	DWORD len = ::GetModuleFileName(AfxGetInstanceHandle(), szFullPath, sizeof(szFullPath)); 
	DWORD dwVerHnd; 

	DWORD dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd); 
	if( dwVerInfoSize && len )
	{
		// If we were able to get the information, process it
		HANDLE hMem; 
		LPVOID lpvMem; 
		char szGetName[256]; 
		int cchRoot; 
		
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize); 
		lpvMem = GlobalLock(hMem); 
		
		GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem); 
		
		lstrcpy(szGetName, "\\StringFileInfo\\040904b0\\"); 
		cchRoot = lstrlen(szGetName); 
		
		BOOL fRet; 
		UINT cchVer = 0; 
		LPVOID lszVer = NULL; 
		
		lstrcpy(&szGetName[cchRoot], "ProductVersion");
		fRet = VerQueryValue(lpvMem, szGetName, &lszVer, &cchVer); 
		
		if( fRet && cchVer && lszVer ) 
		{ 
			vers = (char*)lszVer;
			bRet = true;
		}

		GlobalUnlock(hMem); 
		GlobalFree(hMem); 
	}

	return bRet;
}

/*!
	Format time string with millisecond precision
	\return Formatted time string
	\note Local time obtained using GetLocalTime API
*/
CString FormatMillisecondTime()
{
	CString res;

	SYSTEMTIME systemTime = { 0 };
	GetLocalTime(&systemTime);

	res.Format("%02d:%02d:%02d.%03d", 
		systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

	return res;
}

/*!
	Format time string with millisecond precision
	\param lpSystemTime Optional parameter to format the time for, if NULL the local time will be used
	\return Formatted time string
*/
CString FormatMillisecondTime(LPSYSTEMTIME lpSystemTime)
{
	CString res;

	ASSERT(lpSystemTime != NULL); // Invalid pointer

	res.Format("%02d:%02d:%02d.%03d", 
		lpSystemTime->wHour, lpSystemTime->wMinute, lpSystemTime->wSecond, lpSystemTime->wMilliseconds);

	return res;
}

/*!
	Format hide option checkbox text
	\return Formatted checkbox text
*/
CString FormatHideOptionText()
{
	CString strRes;
	strRes.Format(_T("Display only if %s key is down"), gCvsPrefs.HideCommandDlgUseShift() ? "Shift" : "Ctrl");

	return strRes;
}

/*!
	Fixed version of unlink for WIN32
	\param filename File name
	\return 0 on success, -1 otherwise
*/
int wnt_unlink(const char* filename)
{
	int res = -1;

	for(int nIndex = 0; nIndex < MAX_UNLINK_RETRY; nIndex++)
	{
		res = unlink(filename);
		if( res == 0 )
		{
			break;
		}
		else if( EACCES != errno )
		{
			return -1;
		}

		Sleep(60);
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////
// CWndRedrawManager

/// Constructor sets the redraw to FALSE unless overidden by redraw flag
CWndRedrawMngr::CWndRedrawMngr(CWnd* pWnd)
{
	ASSERT(pWnd != NULL);
	m_pWnd = pWnd;
	
	if( m_pWnd )
	{
		m_redraw = (BOOL)::GetWindowLongPtr(m_pWnd->m_hWnd, GWLP_USERDATA);
		if( !m_redraw )
		{
			//m_pWnd->LockWindowUpdate();
			m_pWnd->SetRedraw(FALSE);
			::SetWindowLongPtr(m_pWnd->m_hWnd, GWLP_USERDATA, TRUE);
		}
	}
}

/// Destructor will make sure that Redraw is set to TRUE when object goes out of scope
CWndRedrawMngr::~CWndRedrawMngr()
{
	if( m_pWnd )
	{
		const BOOL redraw = (BOOL)::GetWindowLongPtr(m_pWnd->m_hWnd, GWLP_USERDATA);
		if( redraw != m_redraw )
		{
			//m_pWnd->UnlockWindowUpdate();
			m_pWnd->SetRedraw(TRUE);
			::SetWindowLongPtr(m_pWnd->m_hWnd, GWLP_USERDATA, m_redraw);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CWndAutoCommand

CWndAutoCommand::CWndAutoCommand(const CWnd* pWnd, const UINT uiCmd)
{
	m_hAutoCommandWnd = pWnd->GetSafeHwnd();
	m_uiCmd = uiCmd;
}

CWndAutoCommand::~CWndAutoCommand()
{
	::PostMessage(m_hAutoCommandWnd, WM_COMMAND, (WPARAM)m_uiCmd, (LPARAM)0);
}
