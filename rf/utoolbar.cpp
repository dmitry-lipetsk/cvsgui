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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- May 1999
 */

#include "stdafx.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <float.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#include "utoolbar.h"
#include "uconsole.h"

#include <vector>
#include <map>

#if qGTK
	extern "C" GtkWidget* create_pixmap(GtkWidget *widget, const gchar *filename);
#endif

#if defined(_DEBUG) && defined(WIN32)
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

UIMPLEMENT_DYNAMIC(UToolbar, UWidget)

UBEGIN_MESSAGE_MAP(UToolbar, UWidget)
	ON_UPDATEUI(UToolbar)
	ON_UCOMMAND_RANGE(0, 0xFFFF, UToolbar::OnCommands)
	ON_UDESTROY(UToolbar)
	ON_TB_INIT(UToolbar)
	ON_TB_END(UToolbar)
	ON_TB_BUTTON(UToolbar)
	ON_TB_DROPDOWN(UToolbar)
	ON_TB_CHECKBTN(UToolbar)
	ON_TB_SEPARATOR(UToolbar)
	ON_USETTOOLTIP(0, 0xFFFF, UToolbar::OnSetTooltip)
UEND_MESSAGE_MAP()

typedef enum
{
	tbeDropDown,
	tbeButton,
	tbeSeparator
} tbeKind;

class TbEntry
{
public:
	TbEntry(tbeKind k) : kind(k) {}
	virtual ~TbEntry() {}

	tbeKind kind;
};

#ifndef WIN32
	typedef void *HBITMAP;
#endif

typedef struct
{
	int cnt;
	HBITMAP bitmap;
} AREF;

class aref
{
public:
	aref() : ref(0L)
	{
	}
	aref(HBITMAP b)
	{
		ref = new AREF;
		ref->cnt = 1;
		ref->bitmap = b;
	}
	aref(const aref & src)
	{
		ref = src.Ref();
	}
	virtual ~aref()
	{
		UnRef();
	}
	AREF *Ref(void) const
	{
		if(ref != 0L)
			ref->cnt++;
		return ref;
	}
	AREF *UnRef(void) const
	{
		if(ref != 0L && --ref->cnt == 0)
		{
#ifdef WIN32
			if(ref->bitmap != 0L)
				::DeleteObject(ref->bitmap);
#endif
			delete ref;
			((aref *)this)->ref = 0L;
		}
		return ref;
	}
	aref & operator=(const aref & src)
	{
		UnRef();
		ref = src.Ref();
		return *this;
	}
	operator HBITMAP()
	{
		return ref != 0L ? ref->bitmap : 0L;
	}
protected:
	AREF *ref;
};

class awidget
{
public:
	awidget(HBITMAP b, int c, const char *t, UTB_VAR v) :
		bitmap(b), cmd(c), tooltip(0L), var(v)
	{
		if(t != 0L)
			tooltip = strdup(t);
	}
	awidget() : bitmap(0L), cmd(0), tooltip(0L)
	{
	}
	awidget(const awidget & src)
	{
		*this = src;
	}

	virtual ~awidget()
	{
		if(tooltip != 0L)
			free(tooltip);
	}

	awidget & operator=(const awidget & src)
	{
		bitmap = src.bitmap;
		cmd = src.cmd;
		var = src.var;
		tooltip = src.tooltip != 0L ? strdup(src.tooltip) : 0L;

		return *this;
	}
	aref bitmap;
	int cmd;
	char *tooltip;
	UTB_VAR var;
};

class TbEntryButton : public TbEntry
{
public:
	TbEntryButton(HBITMAP b, int c, const char *t, UTB_VAR v) :
		TbEntry(tbeButton), wid(b, c, t, v), ischeck(false)
	{
	}
	virtual ~TbEntryButton()
	{
	}

	awidget wid;
	bool ischeck;
};

class TbEntryChkButton : public TbEntryButton
{
public:
	TbEntryChkButton(HBITMAP b, int c, const char *t, UTB_VAR v) :
		TbEntryButton(b, c, t, v)
	{
		ischeck = true;
	}
	virtual ~TbEntryChkButton()
	{
	}
};

class TbEntryDropDown : public TbEntry
{
public:
	TbEntryDropDown() : TbEntry(tbeDropDown), current(0) {}
	virtual ~TbEntryDropDown()
	{
	}

	std::vector<awidget> icons;
	int current;
};

class TbEntrySeparator : public TbEntry
{
public:
	TbEntrySeparator() : TbEntry(tbeSeparator) {}
	virtual ~TbEntrySeparator()
	{
	}
};

class WININFO
{
public:
	WININFO(int w, int h) : width(w), height(h) {}
	virtual ~WININFO()
	{
		std::vector<TbEntry *>::iterator i;
		for(i = entries.begin(); i != entries.end(); ++i)
		{
			delete (*i);
		}
	}

	std::vector<TbEntry *>::iterator Search(int cmd, int *dropindex = 0L)
	{
		std::vector<TbEntry *>::iterator i;
		for(i = entries.begin(); i != entries.end(); ++i)
		{
			TbEntry * entry = (*i);
			if(entry->kind == tbeDropDown)
			{
				TbEntryDropDown *drop = (TbEntryDropDown *)entry;
				std::vector<awidget>::iterator j;
				int index;

				for(j = drop->icons.begin(), index = 0; j != drop->icons.end(); ++j, index++)
				{
					if((*j).cmd == cmd)
					{
						if(dropindex != 0L)
							*dropindex = index;
						return i;
					}
				}
			}
			else if(entry->kind == tbeButton)
			{
				TbEntryButton *btn = (TbEntryButton *)entry;
				if(btn->wid.cmd == cmd)
					return i;
			}
		}
		return i;
	}

	std::vector<TbEntry *> entries;
	int width, height;
};

#ifdef WIN32
class CColorMenu : public CMenu
{
public:
// Operations
	void AppendColorMenuItem(UINT nID, awidget *what);

// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	CColorMenu(int width, int height);
	virtual ~CColorMenu();
	
	int m_width;
	int m_height;
};

CColorMenu::CColorMenu(int width, int height) : m_width(width), m_height(height)
{
	VERIFY(CreatePopupMenu());
}

CColorMenu::~CColorMenu()
{
}

void CColorMenu::AppendColorMenuItem(UINT nID, awidget *what)
{
	VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW, nID, (LPCTSTR)what));
}

void CColorMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// all items are of fixed size
	lpMIS->itemWidth = m_width + 4;
	lpMIS->itemHeight = m_height + 4;
}

void CColorMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	awidget *what = (awidget *)lpDIS->itemData;

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		COLORREF crHilite = ::GetSysColor(COLOR_MENU);
		CBrush br(crHilite);
		CRect r(lpDIS->rcItem);
		r.DeflateRect(1, 1);
		pDC->FrameRect(&r, &br);
	}

	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		// Paint the color item in the color requested
		CRect r(lpDIS->rcItem);
		r.OffsetRect((r.Width() - m_width) / 2, (r.Height() - m_height) / 2);
		CDC memdc;
		memdc.CreateCompatibleDC(pDC);
		CBitmap bmap;
		VERIFY(bmap.Attach(what->bitmap));
		CBitmap *old = memdc.SelectObject(&bmap);
		pDC->BitBlt(r.left, r.top, r.Width(), r.Height(), &memdc, 0, 0, SRCCOPY);
		memdc.SelectObject(old);
		bmap.Detach();
		VERIFY(memdc.DeleteDC());
	}

	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected - hilite frame
		COLORREF crHilite = ::GetSysColor(COLOR_HIGHLIGHT);
		CBrush br(crHilite);
		CRect r(lpDIS->rcItem);
		r.DeflateRect(1, 1);
		pDC->FrameRect(&r, &br);
	}
}

IMPLEMENT_DYNAMIC(CDynToolBar, CCJToolBar)

BEGIN_MESSAGE_MAP(CDynToolBar, CCJToolBar)
	//{{AFX_MSG_MAP(CDynToolBar)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDynToolBar::CDynToolBar() : m_info(0L), m_bitmap(0L),
	m_tracking(false), m_widid(0)
{
}

CDynToolBar::~CDynToolBar()
{
	//if(m_bitmap != 0L)
		//::DeleteObject(m_bitmap);
}

void CDynToolBar::OnDestroy(void)
{
	UEventSendMessage(GetPseudoID(), EV_DESTROY, 0, NULL);

	CCJToolBar::OnDestroy();
}

void CDynToolBar::OnDropDownButton(const NMTOOLBAR& nmtb, UINT nID, CRect rc)
{
	std::vector<TbEntry *>::iterator f = m_info->Search(WINCMD_to_UCMD(nID));
	if(f == m_info->entries.end())
	{
		TRACE("Unknown id");
		return;
	}

	TbEntryDropDown *entry = (TbEntryDropDown *)(*f);
	if(entry->kind != tbeDropDown)
	{
		TRACE("Unknown kind");
		return;
	}

	CColorMenu menu(m_info->width, m_info->height);
	std::vector<awidget>::iterator i;
	for(i = entry->icons.begin(); i != entry->icons.end(); ++i)
	{
		menu.AppendColorMenuItem(UCMD_to_WINCMD((*i).cmd), &i[0]);
	}

	m_tracking = true;
	menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
		rc.left, rc.bottom, GetOwner(), &rc);
	m_tracking = false;
}
#endif

UToolbar::UToolbar(int widid) : UWidget(widid), m_winInfo(0L)
#ifdef WIN32
	, m_wndToolBar(0L)
#endif
{
}

UToolbar::~UToolbar()
{
}

void UToolbar::OnTbInit(UTB_INIT *info)
{
	m_winInfo = new WININFO(info->widthbtn, info->heightbtn);

#ifdef WIN32
	WININFO & tb = *m_winInfo;
	m_wndToolBar = new CDynToolBar;
	CFrameWnd *f = m_wndToolBar->GetMainFrame();
	VERIFY(m_wndToolBar->Create(f, WS_CHILD | WS_VISIBLE | CBRS_TOP, info->winid));
	m_wndToolBar->SetBarStyle(m_wndToolBar->GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndToolBar->EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar->SetWinInfo(m_winInfo);
	m_wndToolBar->SetPseudoID(GetWidID());
	f->DockControlBar(m_wndToolBar);

	// configure the Win32 toolbar
	if(info->title != 0L)
		m_wndToolBar->SetWindowText(info->title);
	m_wndToolBar->SetSizes(CSize(info->widthbtn+7,info->heightbtn+6),
		CSize(info->widthbtn,info->heightbtn));
#endif
#ifdef qGTK
	void *wid = GetWidget(kUMainWidget);
	g_assert(wid != 0L && GTK_IS_TOOLBAR(wid));
#endif
}

#ifdef WIN32
void UToolbar::RebuildBar(bool onlyBitmap)
{
	WININFO & tb = *m_winInfo;

	ASSERT(m_wndToolBar != 0L);
	// create the bitmap associated with the toolbar
	CClientDC dc(m_wndToolBar);
	CBitmap* pbmOld = NULL;

	CDC dcDest;
	VERIFY(dcDest.CreateCompatibleDC(&dc));
	CDC dcSrc;
	VERIFY(dcSrc.CreateCompatibleDC(&dc));

	std::vector<UINT> styles;
	std::vector<TbEntry *>::iterator i;
	int numicons = 0;
	for(i = tb.entries.begin(); i != tb.entries.end(); ++i)
	{
		TbEntry * entry = *i;

		if(entry->kind == tbeDropDown)
		{
			TbEntryDropDown *drop = (TbEntryDropDown *)entry;
			styles.push_back(UCMD_to_WINCMD(drop->icons[drop->current].cmd));
			numicons++;
		}
		else if(entry->kind == tbeButton)
		{
			TbEntryButton *btn = (TbEntryButton *)entry;
			if(btn->wid.var != DDV_NONE)
			{
				styles.push_back(UTB_SEPARATOR);
			}
			else
			{
				numicons++;
				styles.push_back(btn->wid.cmd);
			}
		}
		else if(entry->kind == tbeSeparator)
		{
			styles.push_back(UTB_SEPARATOR);
		}
		else
		{
			ASSERT(0);
		}
	}

	if(!onlyBitmap && styles.size())
	{
		// create the Win32 toolbar
		m_wndToolBar->SetButtons(&styles.begin()[0], styles.size());
	}

	CBitmap bdest;
	bdest.CreateCompatibleBitmap(&dc, tb.width * numicons, tb.height);
	ASSERT(bdest.m_hObject != NULL);
	CBitmap *pbmOldDest = dcDest.SelectObject(&bdest);

	int cnt = 0, index = 0;
	for(i = tb.entries.begin(); i != tb.entries.end(); ++i, index++)
	{
		TbEntry * entry = *i;
		HBITMAP bmap = 0L;
		int cmd;
		bool dropdown = false;
		bool ischeck = false;

		if(entry->kind == tbeDropDown)
		{
			TbEntryDropDown *drop = (TbEntryDropDown *)entry;
			bmap = drop->icons[drop->current].bitmap;
			cmd = drop->icons[drop->current].cmd;
			dropdown = true;
		}
		else if(entry->kind == tbeButton)
		{
			TbEntryButton *btn = (TbEntryButton *)entry;
			if(btn->wid.var != DDV_NONE)
			{
				if(!onlyBitmap)
				{
					m_wndToolBar->SetButtonInfo(index, ID_SEPARATOR,
						TBBS_SEPARATOR, tb.width);
					CRect rect;
					m_wndToolBar->GetItemRect(index, &rect);
					/*VERIFY(Create("",
							WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_OWNERDRAW,
							rect, m_wndToolBar, UCMD_to_WINCMD(btn->wid.cmd)));*/
				}
				continue;
			}

			bmap = btn->wid.bitmap;
			cmd = btn->wid.cmd;
			ischeck = btn->ischeck;
		}
		else if(entry->kind == tbeSeparator)
		{
			m_wndToolBar->SetButtonInfo(index, ID_SEPARATOR,
				TBBS_SEPARATOR, 0);
			continue;
		}
		else
		{
			continue;
			ASSERT(0);
		}

		if(bmap != 0L)
		{
			CBitmap bsrc;
			VERIFY(bsrc.Attach(bmap));
			CBitmap *pbmOldSrc = dcSrc.SelectObject(&bsrc);

			dcDest.BitBlt(tb.width * cnt, 0, tb.width, tb.height,
					&dcSrc, 0, 0, SRCCOPY);
			bsrc.Detach();

			dcSrc.SelectObject(pbmOldSrc);
		}

		DWORD dwStyle = m_wndToolBar->GetButtonStyle(index);
		if(ischeck)
		{
			dwStyle &= ~TBBS_BUTTON;
			dwStyle |= TBBS_CHECKBOX;
		}
		m_wndToolBar->SetButtonInfo(index, UCMD_to_WINCMD(cmd),
			dwStyle, cnt);

		if(dropdown)
			m_wndToolBar->AddDropDownButton(UCMD_to_WINCMD(cmd), -1, TRUE);

		cnt++;
	}

	dcDest.SelectObject(pbmOldDest);
	VERIFY(dcDest.DeleteDC());
	VERIFY(dcSrc.DeleteDC());

	HBITMAP finalbmap = (HBITMAP)bdest.Detach();
	m_wndToolBar->SetBitmap(finalbmap);

	if(m_wndToolBar->m_bitmap != 0L)
		::DeleteObject(m_wndToolBar->m_bitmap);
	m_wndToolBar->m_bitmap = finalbmap;

}
#endif

void UToolbar::OnTbEnd(void)
{
#ifdef WIN32
	RebuildBar(false);
#endif
}

void UToolbar::OnTbButton(UTB_BUTTON *info)
{
#ifdef WIN32
	ASSERT(m_wndToolBar != 0L);
#endif

	WININFO & tb = *m_winInfo;
	HBITMAP bitmap = UMakeBitmap((char **)info->icon);
	TbEntryButton *btn = new TbEntryButton(bitmap, info->cmd,
		info->tooltip, info->varcode);
	tb.entries.push_back(btn);

#ifdef qGTK
	char status[256] = {'\0'};
	char prompt[256] = {'\0'};

	strcpy(status, info->tooltip);
	char *cr = strchr(status, '\n');
	if(cr != 0L)
	{
		strcpy(prompt, cr + 1);
		*cr = '\0';
	}
	GtkToolbar *toolbar = GTK_TOOLBAR(GetWidget(kUMainWidget));
	GtkWidget *tmp_toolbar_icon = info->icon != 0L ?
		create_pixmap(GTK_WIDGET(toolbar), (const char *)info->icon) : 0L;
	GtkWidget *wid = gtk_toolbar_append_element(toolbar, GTK_TOOLBAR_CHILD_BUTTON, 0L,
												prompt, status, 0L,
												tmp_toolbar_icon, 0L, 0L);
	UEventSendMessage(GetWidID(), EV_INIT_WIDGET, info->cmd, wid);
#endif
}

void UToolbar::OnTbSeparator(void)
{
#ifdef WIN32
	ASSERT(m_wndToolBar != 0L);
#endif

	WININFO & tb = *m_winInfo;
	TbEntrySeparator *btn = new TbEntrySeparator();
	tb.entries.push_back(btn);

#ifdef qGTK
	GtkToolbar *toolbar = GTK_TOOLBAR(GetWidget(kUMainWidget));

	/*GtkWidget *wid = */gtk_toolbar_append_space(toolbar);
#endif
}

void UToolbar::OnTbDropDown(UTB_DROPDOWN *info)
{
#ifdef WIN32
	ASSERT(m_wndToolBar != 0L);
#endif

	WININFO & tb = *m_winInfo;
	TbEntryDropDown *drop = new TbEntryDropDown();
	tb.entries.push_back(drop);
	int current = 0;

#ifndef qGTK
	UTB_BUTTON *btns = info->btns;
	while(btns->icon != 0L)
	{
		HBITMAP bitmap = UMakeBitmap((char **)btns->icon);
		awidget ad(bitmap, btns->cmd, btns->tooltip, btns->varcode);
		drop->icons.push_back(ad);
		if(btns->cmd == info->initialcmd)
			drop->current = current;
		btns++;
		current++;
	}
#else
	char status[256] = {'\0'};
	char prompt[256] = {'\0'};

	strcpy(status, info->tooltip);
	char *cr = strchr(status, '\n');
	if(cr != 0L)
	{
		strcpy(prompt, cr + 1);
		*cr = '\0';
	}
	
	UTB_BUTTON *btns = info->btns;
	GtkToolbar *toolbar = GTK_TOOLBAR(GetWidget(kUMainWidget));
	GtkWidget *newElement = gtk_combo_new ();
	
	while(btns && btns->icon != 0L) {
		if(btns->cmd == info->initialcmd)
			drop->current = current;
				
		btns++;
	}

	/* don't open the list on RETURN */
	gtk_combo_disable_activate(GTK_COMBO(newElement));
	
	gtk_toolbar_append_widget(toolbar, newElement, prompt, status);
	gtk_widget_show (newElement);
		
	UEventSendMessage(GetWidID(), EV_INIT_WIDGET, info->initialcmd, newElement);
#endif
}

void UToolbar::OnTbCheckBtn(UTB_BUTTON *info)
{
#ifdef WIN32
	ASSERT(m_wndToolBar != 0L);
#endif

	WININFO & tb = *m_winInfo;
	HBITMAP bitmap = UMakeBitmap((char **)info->icon);
	TbEntryButton *btn = new TbEntryChkButton(bitmap, info->cmd,
		info->tooltip, info->varcode);
	tb.entries.push_back(btn);

#ifdef qGTK
	char status[256] = {'\0'};
	char prompt[256] = {'\0'};

	strcpy(status, info->tooltip);
	char *cr = strchr(status, '\n');
	if(cr != 0L)
	{
		strcpy(prompt, cr + 1);
		*cr = '\0';
	}
	GtkToolbar *toolbar = GTK_TOOLBAR(GetWidget(kUMainWidget));
	GtkWidget *tmp_toolbar_icon = info->icon != 0L ?
		create_pixmap(GTK_WIDGET(toolbar), (const char *)info->icon) : 0L;
	GtkWidget *wid = gtk_toolbar_append_element(toolbar, GTK_TOOLBAR_CHILD_TOGGLEBUTTON, 0L,
												prompt, status, 0L,
												tmp_toolbar_icon, 0L, 0L);
	UEventSendMessage(GetWidID(), EV_INIT_WIDGET, info->cmd, wid);
#endif
}


void UToolbar::OnDestroy(void)
{
	if(m_winInfo != 0L)
		delete m_winInfo;
}

int UToolbar::UDispatchMessage(int/*UEventType*/ evt, int arg1, void *arg2, UDispatchInfo & dispatchInfo)
{
	if(evt == EV_CMD)
	{
		WININFO *info = m_winInfo;
		if(info != 0L)
		{
			int index;
			std::vector<TbEntry *>::iterator f = info->Search(arg1, &index);
			if(f != info->entries.end())
			{
				TbEntryDropDown *entry = (TbEntryDropDown *)*f;
				if(entry->kind == tbeDropDown)
				{
					// toggle the current bitmap
					if(entry->current != index)
					{
						entry->current = index;
#ifdef WIN32
						RebuildBar();
#endif
					}
				}
			}
		}
	}

	return UWidget::UDispatchMessage(evt, arg1, arg2, dispatchInfo);
}

void UToolbar::OnSetTooltip(int cmd, UCmdUI *pCmdUI)
{
	WININFO *info = m_winInfo;
	if(info == 0L)
		return;

	int index;
	std::vector<TbEntry *>::iterator f = info->Search(cmd, &index);
	if(f == info->entries.end())
		return;

	TbEntry * entry = *f;
	if(entry->kind == tbeDropDown)
	{
		TbEntryDropDown *drop = (TbEntryDropDown *)entry;
		awidget & wid = drop->icons[index];
		pCmdUI->SetTooltip(wid.tooltip);
	}
	else if(entry->kind == tbeButton)
	{
		TbEntryButton *btn = (TbEntryButton *)entry;
		pCmdUI->SetTooltip(btn->wid.tooltip);
	}
}

int UToolbar::OnCommands(int cmd)
{
#if 0
	static bool sSemaphore = false;
	USemaphore policeman(sSemaphore);
	if(policeman.IsEnteredTwice())
		return 1;
#endif
	// this is the interesting thing about the toolbar : it resends the
	// commands to other widgets according to the focus information and
	// allows the command to be handled several times if the handler return 1

	const STD map<int, UCmdTarget *> & targets = UEventGetTargets();
	int res = 0;
	int handledFlag;

	// first forward the command to the focused widget : if it doesn't want it
	// we give it to the next pseudo widget.
	if(sFocusWidID != -1 && sFocusWidID != GetWidID())
	{
		res = UEventSendMessageExt(sFocusWidID, EV_CMD, cmd, 0L, &handledFlag);
		if(/*res == 0 &&*/ handledFlag)
			return 0;
	}

	STD map<int, UCmdTarget *>::const_iterator i;
	for(i = targets.begin(); i != targets.end(); ++i)
	{
		int widid = (*i).first;
		if(widid == sFocusWidID || widid == GetWidID())
			continue;
		res = UEventSendMessageExt(widid, EV_CMD, cmd, 0L, &handledFlag);
		if(/*res == 0 &&*/ handledFlag)
			return 0;
	}

	UAppConsole("The cmd %d of the toolbar has no command handler\n", cmd);

	return 0;
}

void UToolbar::OnUpdateUI()
{
// on Windows the update UI is sent by MFC
#ifndef WIN32
	WININFO *info = m_winInfo;
	if(info == 0L)
		return;

	static bool sSemaphore = false;
	USemaphore policeman(sSemaphore);
	if(policeman.IsEnteredTwice())
		return;

	// this is the interesting thing about the toolbar : it resends the
	// update UI to other widgets according to the focus information and
	// allows the update to be handled several times if the handler asks for it
	// (by using UCmdUI::SetContinue)

	const STD map<int, UCmdTarget *> & targets = UEventGetTargets();
	std::vector<TbEntry *>::const_iterator f;
	int cmdid;

	for(f = info->entries.begin(); f != info->entries.end(); ++f)
	{
		TbEntry * entry = *f;
		if(entry->kind == tbeButton)
		{
			TbEntryButton *btn = (TbEntryButton *)entry;
			cmdid = btn->wid.cmd;
		}
		else
			continue;

		void *widget = GetWidget(cmdid);
		if(widget == 0L)
			continue;
		
		// first forward the command to the focused widget : if it doesn't want it
		// we give it to the next pseudo widget.
		if(sFocusWidID != -1 && sFocusWidID != GetWidID())
		{
			UCmdUI pUI(cmdid);
			pUI.m_widget = widget;
			UEventSendMessage(sFocusWidID, EV_UPDTCMD, pUI.m_nID, &pUI);
			if(!pUI.CanContinue())
				continue;
		}

		STD map<int, UCmdTarget *>::const_iterator i;
		for(i = targets.begin(); i != targets.end(); ++i)
		{
			int widid = (*i).first;
			if(widid == sFocusWidID || widid == GetWidID())
				continue;
			
			UCmdUI pUI(cmdid);
			pUI.m_widget = widget;
			UEventSendMessage(widid, EV_UPDTCMD, pUI.m_nID, &pUI);
			if(!pUI.CanContinue())
				break;
		}
		if(i == targets.end())
		{
			//UAppConsole("The cmd %d of the toolbar has no update handler, it's gonna be disabled\n", cmdid);
			UCmdUI pUI(cmdid);
			pUI.m_widget = widget;
			pUI.Enable(false);
		}
	}
#endif // !WIN32
}
