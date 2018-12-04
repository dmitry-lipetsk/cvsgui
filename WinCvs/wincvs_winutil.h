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

#ifndef WINCVS_WINUTIL_H
#define WINCVS_WINUTIL_H

#include <string>

/// Default flag for the autocomplete enabled edit control
#define AUTOCOMPLETE_DEFFLAGS	(SHACF_FILESYSTEM | SHACF_AUTOAPPEND_FORCE_ON | SHACF_AUTOAPPEND_FORCE_ON | SHACF_USETAB)

/// Custom menu enum
enum
{
	kCustomBrowserMenu,	/*!< Folders browser menu */
	kCustomFilesMenu	/*!< Files browser menu */
};

CWnd* FindNextWindow(CWnd* pWnd, BOOL CheckTabStop = TRUE);
CWnd* FindPrevWindow(CWnd* pWnd, BOOL CheckTabStop = TRUE);

CMenu* GetSubCMenu(int nStartId, CView* pThis);
CMenu* GetMenuEntry(int nStartId, int& pos);
CMenu* GetTopPopupMenu(const char* menuString);

void UpdateUIMenu(CMenu* pMyMenu, CView* pThis);
int GetMenuItemPos(const CMenu* pMenu, const UINT nID);

bool IsLargeFonts();

bool IsHideCommandKeyPressed();
bool IsEscKeyPressed();

DWORD SetListCtrlExtendedStyle(CListCtrl* pListCtrl, const DWORD dwExtendedStyle);

BOOL EnableEditAutoComplete(CEdit* pWnd, const DWORD dwFlags = AUTOCOMPLETE_DEFFLAGS);
void PostGotoDlgCtrl(CDialog* pDlg, CWnd* pWndCtrl);

bool CheckLocationDriveType(const char* locationPath, const bool quiet = false);
bool GetAppPath(std::string& path);
bool GetAppModule(std::string& module);
bool GetFolder(std::string& strFolder, int folder);

bool GetAppVersion(std::string& vers);

CString FormatMillisecondTime();
CString FormatMillisecondTime(LPSYSTEMTIME lpSystemTime);
CString FormatHideOptionText();

int wnt_unlink(const char* filename);

/// Class to support efficient drawing of CWnd based windows by managing window's redraw
class CWndRedrawMngr
{
private:
	BOOL m_redraw;	/*!< Redraw state to redraw to */
	CWnd* m_pWnd;	/*!< Window to manage */

public:
	CWndRedrawMngr(CWnd* pWnd);
	virtual ~CWndRedrawMngr();
};

/*!
	Helper class to automatically issue the command to the given window using WM_COMMAND message
	\note To be used for refresh purposes after the synchronous command completes
*/
class CWndAutoCommand
{
public:
	// Construction
	CWndAutoCommand(const CWnd* pWnd, const UINT uiCmd);
	virtual ~CWndAutoCommand();

private:
	// Data members
	HWND m_hAutoCommandWnd;	/*!< Window handle to which the command will be send */
	UINT m_uiCmd;			/*!< Command to be send to the window */
};

#endif	/*WINCVS_WINUTIL_H*/
