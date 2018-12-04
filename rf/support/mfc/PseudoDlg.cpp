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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

// PseudoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "uwidget.h"
#include "PseudoDlg.h"
#include "PseudoDraw.h"
#include "PseudoList.h"
#include "SmartComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IS_PSEUDO_CTL(id) ((id) >= WINCMD_DLG_START && (id) < WINCMD_DLG_END)

/////////////////////////////////////////////////////////////////////////////
// CPseudoDlg dialog

CPseudoDlg::CPseudoDlg(int dlgid, UWidget *pseudo, CWnd* pParent /*=NULL*/)
	: CDialog(dlgid, pParent), m_pseudo(pseudo)
{
	//{{AFX_DATA_INIT(CPseudoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPseudoDlg::~CPseudoDlg()
{
	if(m_pseudo != 0L)
	{
		UEventSendMessage(m_pseudo->GetWidID(), EV_DESTROY, 0, 0L);

		std::map<int, void *>::iterator k;
		for(k = m_pseudo->GetWidgets().begin(); k != m_pseudo->GetWidgets().end(); ++k)
		{
			CWnd *wnd = (CWnd *)(*k).second;
			if((*k).first != kUMainWidget)
				delete wnd;
		}
	}
}

void CPseudoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if(m_pseudo != 0L)
	{
		std::map<int, void *>::iterator i;
		for(i = m_pseudo->GetWidgets().begin(); i != m_pseudo->GetWidgets().end(); ++i)
		{
			if((*i).first != kUMainWidget)
				DDX_Control(pDX, UCMD_to_WINCTL((*i).first), *(CWnd *)(*i).second);
		}

		m_pseudo->DoDataExchange(!pDX->m_bSaveAndValidate);
	}

	//{{AFX_DATA_MAP(CPseudoDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPseudoDlg, CDialog)
	//{{AFX_MSG_MAP(CPseudoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void WinInitWidgets(CWnd *wnd, UWidget *pseudo)
{
	CWnd* pWndStart = 0L;
	pWndStart = wnd->GetTopWindow();
	while(pWndStart != 0L)
	{
		HWND hwnd = *pWndStart;
		int id = ::GetDlgCtrlID(*pWndStart);
		CWnd *res = 0L;
		if(IS_PSEUDO_CTL(id))
		{
			TCHAR szCompare[100];
			::GetClassName(hwnd, szCompare, _countof(szCompare));
			DWORD style = (DWORD)GetWindowLong(hwnd, GWL_STYLE);

			// check overided widgets
			std::map<int, CRuntimeClass *> & list = pseudo->GetOverideList();
			std::map<int, CRuntimeClass *>::const_iterator i = list.find(WINCTL_to_UCMD(id));
			if(i != list.end())
			{
				CRuntimeClass *clss = (*i).second;
				if(clss->IsDerivedFrom(RUNTIME_CLASS(CWnd)))
				{
					res = (CWnd *)clss->CreateObject();
				}
			}

			if(res == 0L)
			{
				if(stricmp(szCompare, "BUTTON") == 0)
				{
 					CButton *btn = new CButton();
					res = btn;
				}
				else if(stricmp(szCompare, "Static") == 0)
				{
					if(style & SS_ETCHEDFRAME)
					{
 						CPseudoDraw *draw = new CPseudoDraw(pseudo, WINCTL_to_UCMD(id));
						res = draw;
					}
					else
					{
						CStatic *stat = new CStatic();
						res = stat;
					}
				}
				else if(stricmp(szCompare, "ComboBox") == 0)
				{
					CComboBox *combo = new CSmartComboBox(USmartCombo::AutoDropWidth);
					res = combo;
				}
				else if(stricmp(szCompare, WC_LISTVIEW) == 0)
				{
					CPseudoList *list = new CPseudoList(pseudo->GetWidID(), WINCTL_to_UCMD(id));
					res = list;
				}
				else
				{
					res = new CWnd();
				}
			}

			UEventSendMessage(pseudo->GetWidID(), EV_INIT_WIDGET, WINCTL_to_UCMD(id), res);
		}
		pWndStart = pWndStart->GetNextWindow();
	}

	std::map<int, CRuntimeClass *> & list = pseudo->GetOverideList();
	list.erase(list.begin(), list.end());
}

/////////////////////////////////////////////////////////////////////////////
// CPseudoDlg message handlers

BOOL CPseudoDlg::OnInitDialog() 
{
	if(m_pseudo != 0L)
	{
		WinInitWidgets(this, m_pseudo);
		UEventSendMessage(m_pseudo->GetWidID(), EV_CREATE, 0, 0L);
	}
	
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

static int findIdForWnd(CWnd *wnd, UWidget *wid)
{
	const std::map<int, void *> & allWids = wid->GetWidgets();
	std::map<int, void *>::const_iterator i;
	for(i = allWids.begin(); i != allWids.end(); ++i)
	{
		if(wnd == (CWnd *)(*i).second)
			return (*i).first;
	}
	return -1;
}

static int UWinToUMod(UINT nFlags)
{
	int res = 0;
	if(nFlags & MK_LBUTTON)
		res |= VK_LBUTTON;
	if(nFlags & MK_RBUTTON)
		res |= UK_RBUTTON;
	if(nFlags & MK_SHIFT)
		res |= UK_SHIFT;
	if(nFlags & MK_CONTROL)
		res |= UK_CONTROL;
	if(nFlags & MK_MBUTTON)
		res |= UK_MBUTTON;
	if ((GetKeyState(VK_MENU) & 0x8000) != 0)
		res |= UK_ALT;

	return res;
}

LRESULT UWinPseudoWindowProc(CWnd *wnd, UWidget *wid, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool sentOne = false;

	if(wid == 0L)
		return 1;

	if (message == WM_COMMAND)
	{
		UINT nID = LOWORD(wParam);
		if(!IS_PSEUDO_CTL(nID))
			return 1;

		int nCode = HIWORD(wParam);
		HWND hWndCtrl = (HWND)lParam;
		switch(nCode)
		{
		case BN_CLICKED:
			UEventSendMessage(wid->GetWidID(), EV_CMD, WINCTL_to_UCMD(nID), 0L);
			sentOne = true;
			break;
		case CBN_SELCHANGE:
		{
			ASSERT(::IsWindow(hWndCtrl));
			int sel = (int)::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0);
			const char *txt = 0L;
			char buf[1024] = {'\0'};
			if(sel != -1 && (int)::SendMessage(hWndCtrl, CB_GETLBTEXT, sel, (LPARAM)buf) != CB_ERR)
				txt = buf;
			if(strcmp(buf, "-") != 0) // separator
				UEventSendMessage(wid->GetWidID(), EV_COMBO_SEL,
					UMAKEINT(WINCTL_to_UCMD(nID), sel), (void *)txt);
			sentOne = true;
			break;
		}
		case EN_CHANGE:
			if(wid->m_noDispatch)
				break;

			ASSERT(::IsWindow(hWndCtrl));
			UEventSendMessage(wid->GetWidID(), EV_EDITCHANGE,
				WINCTL_to_UCMD(nID), 0L);
			break;
		}
	}
	else if (message == WM_NOTIFY)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (pNMHDR->hwndFrom != NULL)
		{
			UINT nID = _AfxGetDlgCtrlID(pNMHDR->hwndFrom);
			int nCode = pNMHDR->code;
			if(!IS_PSEUDO_CTL(nID))
				return 1;

			switch(nCode)
			{
			case LVN_ITEMCHANGED:
				NMLISTVIEW *state = (NMLISTVIEW *)pNMHDR;

				HWND wnd = state->hdr.hwndFrom;
				ASSERT(::IsWindow(wnd));

				int id = state->hdr.idFrom;
				std::map<int, void *>::const_iterator i = wid->GetWidgets().find(WINCTL_to_UCMD(id));
				if(i != wid->GetWidgets().end())
				{
					CWnd *w = (CWnd *)(*i).second;
					if(w->IsKindOf(RUNTIME_CLASS(CListCtrl)))
					{
						CListCtrl *list = (CListCtrl*)w;
						CString s = list->GetItemText(state->iItem, 0);

						UEventSendMessage(wid->GetWidID(), EV_LIST_SELECTING,
							UMAKEINT(WINCTL_to_UCMD(state->hdr.idFrom), state->iItem), (void *)(const char *)s);
						sentOne = true;
					}
				}
				break;
			}
		}
	}
	else if (message == WM_SIZE)
	{
		int id = findIdForWnd(wnd, wid);
		if(id != -1)
		{
			URECT r = {0, 0, LOWORD(lParam), HIWORD(lParam)};
			UEventSendMessage(wid->GetWidID(), EV_RESIZE, id, &r);
			sentOne = true;
		}
	}
	else if (message == WM_MOUSEMOVE)
	{
		int id = findIdForWnd(wnd, wid);
		if(id != -1)
		{
			UEventSendMessage(wid->GetWidID(), EV_MMOVE, UMAKEINT(LOWORD(lParam), HIWORD(lParam)),
				(void *)UWinToUMod(wParam));
			sentOne = true;
		}
	}
	else if (message == WM_LBUTTONDOWN)
	{
		int id = findIdForWnd(wnd, wid);
		if(id != -1)
		{
			UEventSendMessage(wid->GetWidID(), EV_BTNDOWN, UMAKEINT(LOWORD(lParam), HIWORD(lParam)),
				(void *)UWinToUMod(wParam));
			sentOne = true;
		}
	}
	else if (message == WM_LBUTTONUP)
	{
		int id = findIdForWnd(wnd, wid);
		if(id != -1)
		{
			UEventSendMessage(wid->GetWidID(), EV_BTNUP, UMAKEINT(LOWORD(lParam), HIWORD(lParam)),
				(void *)UWinToUMod(wParam));
			sentOne = true;
		}
	}
	else if (message == WM_LBUTTONDBLCLK)
	{
		int id = findIdForWnd(wnd, wid);
		if(id != -1)
		{
			UEventSendMessage(wid->GetWidID(), EV_BTNDBLDOWN, UMAKEINT(LOWORD(lParam), HIWORD(lParam)),
				(void *)UWinToUMod(wParam));
			sentOne = true;
		}
	}
	else if (message == WM_KEYDOWN)
	{
		int id = findIdForWnd(wnd, wid);
		if(id != -1)
		{
			UEventSendMessage(wid->GetWidID(), EV_KEYDOWN, wParam, 0L);
			sentOne = true;
		}
	}
	else if (message == WM_HSCROLL)
	{
		HWND hwndScrollBar = (HWND) lParam;       // handle to scroll bar 
		int nID = ::GetDlgCtrlID(hwndScrollBar);
		if(!IS_PSEUDO_CTL(nID))
			return 1;

		int cmd = WINCTL_to_UCMD(nID);
		CWnd *wnds = (CWnd *)wid->GetWidget(cmd);
		if(wnds == 0L || !wnds->IsKindOf(RUNTIME_CLASS(CScrollBar)))
			return 1;

		CScrollBar *scroll = (CScrollBar *)wnds;

		int nScrollCode = (int) LOWORD(wParam);  // scroll bar value 
		int nPos;
		if(nScrollCode == SB_THUMBPOSITION || nScrollCode == SB_THUMBTRACK)
		{
			nPos = (short int) HIWORD(wParam);   // scroll box position

			if(nScrollCode == SB_THUMBPOSITION)
				scroll->SetScrollPos(nPos);
		}
		else
		{
			SCROLLINFO info;
			VERIFY(scroll->GetScrollInfo(&info));
			nPos = info.nPos;

			switch(nScrollCode)
			{
			case SB_ENDSCROLL:
				return 1;
				break;
			case SB_LINELEFT:
				nPos -= 1;
				break;
			case SB_LINERIGHT:
				nPos += 1;
				break;
			case SB_PAGELEFT:
				nPos -= info.nPage;
				break;
			case SB_PAGERIGHT:
				nPos += info.nPage;
				break;
			case SB_LEFT:
				nPos = info.nMin;
				break;
			case SB_RIGHT:
				nPos = info.nMax;
				break;
			}

			scroll->SetScrollPos(nPos);
		}

		UEventSendMessage(wid->GetWidID(), EV_SCROLLCHANGE, WINCTL_to_UCMD(nID),
			(void *)nPos);
		sentOne = true;
	}
	else if (message == WM_VSCROLL)
	{
		HWND hwndScrollBar = (HWND) lParam;       // handle to scroll bar 
		int nID = ::GetDlgCtrlID(hwndScrollBar);
		if(!IS_PSEUDO_CTL(nID))
			return 1;

		int cmd = WINCTL_to_UCMD(nID);
		CWnd *wnds = (CWnd *)wid->GetWidget(cmd);
		if(wnds == 0L || !wnds->IsKindOf(RUNTIME_CLASS(CScrollBar)))
			return 1;

		CScrollBar *scroll = (CScrollBar *)wnds;

		int nScrollCode = (int) LOWORD(wParam);  // scroll bar value 
		int nPos;
		if(nScrollCode == SB_THUMBPOSITION || nScrollCode == SB_THUMBTRACK)
		{
			nPos = (short int) HIWORD(wParam);   // scroll box position

			if(nScrollCode == SB_THUMBPOSITION)
				scroll->SetScrollPos(nPos);
		}
		else
		{
			SCROLLINFO info;
			VERIFY(scroll->GetScrollInfo(&info));
			nPos = info.nPos;

			switch(nScrollCode)
			{
			case SB_ENDSCROLL:
				return 1;
				break;
			case SB_LINEUP:
				nPos -= 1;
				break;
			case SB_LINEDOWN:
				nPos += 1;
				break;
			case SB_PAGEUP:
				nPos -= info.nPage;
				break;
			case SB_PAGEDOWN:
				nPos += info.nPage;
				break;
			case SB_TOP:
				nPos = info.nMin;
				break;
			case SB_BOTTOM:
				nPos = info.nMax;
				break;
			}

			if(nPos > info.nMax)
				nPos = info.nMax;
			else if(nPos < info.nMin)
				nPos = info.nMin;
			scroll->SetScrollPos(nPos);
		}

		UEventSendMessage(wid->GetWidID(), EV_SCROLLCHANGE, WINCTL_to_UCMD(nID),
			(void *)nPos);
		sentOne = true;
	}

	if(sentOne)
		UWidget::DoValidUI(false);

	return 1;
}

LRESULT CPseudoDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pseudo != 0L && !UWinPseudoWindowProc(this, m_pseudo, message, wParam, lParam))
		return 0;

	return CDialog::WindowProc(message, wParam, lParam);
}
