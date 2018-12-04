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

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "umain.h"
#include "uconsole.h"
#include "uwidget.h"

#include <map>

#if defined(_DEBUG) && defined(WIN32)
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

#if qMacAPP
#	include "UMenuMgr.h"
#	include "UCheckBox.h"
#	include "URadio.h"
#endif

#ifdef qMacPP
#	include "UPseudoSupport.h"
#	include <LCheckBox.h>
#	include <LRadioButton.h>
#	include <LEditText.h>
#	include <LStaticText.h>
#	include <LCheckBoxGroupBox.h>
#endif

#ifdef WIN32
#	include "PseudoButton.h"
#endif

/*!
  \def MIN_IDLE
  minimum idle used by the framework
  to handle the timers
*/
#define MIN_IDLE 100

static long sLastIdle = 0;

// handler to forward the tooltip (usually the status bar)
extern "C"
{
	static void (*sTooltipHandler) (const char *) = 0L;
}

// all the framework widgets are located there
class CAllTargets
{
public:
	CAllTargets() {}
	~CAllTargets()
	{
		// delete the remaining widgets at exit.
		// we don't make a loop because the widget gets
		// removed from the map when calling the destructor.
		STD map<int, UCmdTarget *>::iterator i;
		while(targets.size() != 0)
		{
			i = targets.begin();
			delete (*i).second;
		}
	}

	inline STD map<int, UCmdTarget *> & Get(void) { return targets; }
	inline const STD map<int, UCmdTarget *> & Get(void) const { return targets; }
protected:
	STD map<int, UCmdTarget *> targets;
} sAllTargets;

const UMSGMAP* UCmdTarget::GetBaseMessageMap()
	{ return 0L; }
const UMSGMAP* UCmdTarget::GetThisMessageMap()
	{ return &UCmdTarget::messageMap; }
const UMSGMAP* UCmdTarget::GetMessageMap() const
	{ return &UCmdTarget::messageMap; }
const UMSGMAP UCmdTarget::messageMap =
{ 0L, &UCmdTarget::_messageEntries[0] };
const UMSGMAP_ENTRY UCmdTarget::_messageEntries[] =
{
	{EV_NONE, epNone, UMSG_NOEXT, 0, 0, (UPMSG)0 }
};

static bool sTurnOffDispatching = false;

class CTurnOffDispatching
{
public:
	CTurnOffDispatching() : m_didSetIt(false)
	{
		if(!sTurnOffDispatching)
		{
			m_didSetIt = true;
			sTurnOffDispatching = true;
		}
	}

	~CTurnOffDispatching()
	{
		if(m_didSetIt)
			sTurnOffDispatching = false;
	}
protected:
	bool m_didSetIt;
};

UIMPLEMENT_DYNAMIC(UCmdTarget, UObject)

// the message class used to post retained messages
class CMsg
{
public:
	CMsg(int wid, int ev, int arg1, void *arg2) : m_wid(wid), m_ev(ev), m_arg1(arg1), m_arg2(arg2)
	{
	}

	CMsg() : m_wid(0), m_ev(0), m_arg1(0), m_arg2(0L)
	{
	}

	CMsg(const CMsg & msg) : m_wid(0), m_ev(0), m_arg1(0), m_arg2(0L)
	{
		*this = msg;
	}

	CMsg & operator=(const CMsg & msg)
	{
		m_wid = msg.m_wid;
		m_ev = msg.m_ev;
		m_arg1 = msg.m_arg1;
		m_arg2 = msg.m_arg2;

		return *this;
	}

	int m_wid, m_ev, m_arg1;
	void *m_arg2;

};

static std::vector<CMsg> sRetainedMsgs;

union UMessageMapFunctions
{
	UPMSG pfn;   // generic member function pointer

	// specific type safe variants
	void(UCmdTarget::*pfnV_V)(void);
	void(UCmdTarget::*pfnV_VP)(void *);
	void(UCmdTarget::*pfnV_iVP)(int,void *);
	void(UCmdTarget::*pfnV_i)(int);
	void(UCmdTarget::*pfnV_ii)(int, int);
	void(UCmdTarget::*pfnV_iii)(int, int, int);
	void(UCmdTarget::*pfnV_iiVP)(int, int, void *);
	int(UCmdTarget::*pfni_ii)(int, int);
	int(UCmdTarget::*pfni_i)(int);
	int(UCmdTarget::*pfni_V)();
	int(UCmdTarget::*pfni_iVP)(int,void *);
	int(UCmdTarget::*pfni_VP)(void *);
};

int UEventSendMessage(int wid, int/*UEventType*/ evt, int arg1, void *arg2)
{
	if(sTurnOffDispatching)
		return 0;

	const STD map<int, UCmdTarget *> & targets = sAllTargets.Get();
	STD map< int, UCmdTarget * >::const_iterator iter = targets.find(wid);
	int res = 0;

	if(iter != targets.end())
	{
		UCmdTarget *target = (*iter).second;
		UCmdTarget::UDispatchInfo dispatchInfo = UCmdTarget::kUNoDispatchInfo;
#if qDebug
		try {
#endif
		res = target->UDispatchMessage(evt, arg1, arg2, dispatchInfo);
#if qDebug
		} catch(...) {
			UASSERT(0);
			throw;
		}
#endif
	}
	else
	{
		UAppConsole("Unknown widget id : %d\n", wid);
	}

	return res;
}

int UEventSendMessageExt(int wid, int/*UEventType*/ evt, int arg1, void *arg2, int * handledFlag)
{
	const STD map<int, UCmdTarget *> & targets = sAllTargets.Get();
	STD map< int, UCmdTarget * >::const_iterator iter = targets.find(wid);
	int res = 0;
	UCmdTarget::UDispatchInfo dispatchInfo = UCmdTarget::kUNoDispatchInfo;

	if(iter != targets.end())
	{
		UCmdTarget *target = (*iter).second;
#if qDebug
		try {
#endif
		res = target->UDispatchMessage(evt, arg1, arg2, dispatchInfo);
#if qDebug
		} catch(...) {
			UASSERT(0);
			throw;
		}
#endif
	}
	else
	{
		UAppConsole("Unknown widget id : %d\n", wid);
	}

	*handledFlag = (dispatchInfo & UCmdTarget::kUDispatchHandled) != 0;
	return res;
}

// for the retained messages
int UEventPostMessage(int wid, int/*UEventType*/ evt, int arg1, void *arg2)
{
	sRetainedMsgs.push_back(CMsg(wid, evt, arg1, arg2));
	return 0;
}

// a notify message get broadcasted to every in the map.
int UEventNotifyMessage(int/*UEventType*/ evt, int arg1, void *arg2)
{
	STD map< int, UCmdTarget * >::const_iterator iter;
	const STD map<int, UCmdTarget *> & targets = sAllTargets.Get();
	int res = 0;

	for(iter = targets.begin(); iter != targets.end(); ++iter)
	{
		UCmdTarget *target = (*iter).second;
		UCmdTarget::UDispatchInfo dispatchInfo = UCmdTarget::kUNoDispatchInfo;
#if qDebug
		try {
#endif
		res = target->UDispatchMessage(evt, arg1, arg2, dispatchInfo);
#if qDebug
		} catch(...) {
			UASSERT(0);
			throw;
		}
#endif
		if(res)
			break;
	}

	return res;
}

int UEventGetWidID(void)
{
	static int newid = 100;
	return newid++;
}

UCmdTarget *UEventQueryWidget(int widid)
{
	const STD map<int, UCmdTarget *> & targets = sAllTargets.Get();
	STD map< int, UCmdTarget * >::const_iterator iter = targets.find(widid);
	if(iter != targets.end())
	{
		UCmdTarget *target = (*iter).second;
		return target;
	}
	return 0L;
}

void UEventRemoveWidID(int widid)
{
	STD map<int, UCmdTarget *> & targets = sAllTargets.Get();
	STD map<int, UCmdTarget *>::iterator i = targets.find(widid);
	if(i == targets.end())
		return;
	targets.erase(i);
}

int UCmdTarget::UDispatchMessage(int/*UEventType*/ evt, int arg1, void *arg2, UDispatchInfo & dispatchInfo)
{
	const UMSGMAP* themap;
	const UMSGMAP_ENTRY *entries;
	for(themap = GetMessageMap(); themap != 0L; themap = themap->pBaseMap)
	{
		entries = themap->lpEntries;
		while(entries->nMessage != EV_NONE)
		{
			if(entries->ext == UMSG_RANGE)
			{
				int cmd = (unsigned short)ULOWORD(arg1);
				if(evt == entries->nMessage && 
					cmd >= entries->ext1 &&
					cmd <= entries->ext2)
						goto dispatch;
			}
			else if(entries->nMessage == evt)
				goto dispatch;
			entries++;
		}
	}
	return 0;

dispatch:
	union UMessageMapFunctions mmf;
	mmf.pfn = entries->pfn;
	int res = 0;

	dispatchInfo = UDispatchInfo((int)dispatchInfo | UCmdTarget::kUDispatchHandled);

	switch(entries->nSig)
	{
	case epV_V:
		(this->*mmf.pfnV_V)();
		break;
	case epV_VP:
		(this->*mmf.pfnV_VP)(arg2);
		break;
	case epV_iVP:
		(this->*mmf.pfnV_iVP)(arg1, arg2);
		break;
	case epV_IVP:
		(this->*mmf.pfnV_iVP)(UHIWORD(arg1), arg2);
		break;
	case epV_i:
		(this->*mmf.pfnV_i)(arg1);
		break;
	case epV_I:
		(this->*mmf.pfnV_i)(UHIWORD(arg1));
		break;
	case epV_ii:
		(this->*mmf.pfnV_ii)(ULOWORD(arg1), UHIWORD(arg1));
		break;
	case epV_iii:
		(this->*mmf.pfnV_iii)(ULOWORD(arg1), UHIWORD(arg1), (int)arg2);
		break;
	case epV_iiVP:
		(this->*mmf.pfnV_iiVP)(ULOWORD(arg1), UHIWORD(arg1), arg2);
		break;
	case epi_ii:
		res = (this->*mmf.pfni_ii)(ULOWORD(arg1), UHIWORD(arg1));
		break;
	case epi_i:
		res = (this->*mmf.pfni_i)(arg1);
		break;
	case epi_V:
		res = (this->*mmf.pfni_V)();
		break;
	case epi_iVP:
		res = (this->*mmf.pfni_iVP)(arg1, arg2);
		break;
	case epi_VP:
		res = (this->*mmf.pfni_VP)(arg2);
		break;
	default:
		UAppConsole("Unknown event cast : %d\n", entries->nSig);
		break;
	}

	return res;
}

UCmdTarget::UCmdTarget(int widid) : m_widid(widid)
{
	std::map<int, UCmdTarget *>::value_type val(widid, this);
	sAllTargets.Get().insert(val);
}

UCmdTarget::~UCmdTarget()
{
	UEventRemoveWidID(m_widid);
}

void UCmdUI::Enable(bool bOn)
{
	m_continue = false;

#ifdef qMacAPP
	if((long)m_widget <= 2L)
	{
		// this is a menu
		::Enable(m_nID, bOn);
		m_widget = (void *)(bOn ? 1L : 2L);
			// see UCmdUI::Check
	}
	else
	{
		TView *view = (TView *)m_widget;
		if(MA_MEMBER(view, TView))
		{
			view->SetActiveState(bOn, kRedraw);
		}
	}
#endif
#ifdef qMacPP
	if(m_wrapper != 0L)
	{
		// this is a menu
		*m_wrapper->outEnabled = bOn;
	}
	else
	{
		LPane *view = (LPane *)m_widget;
		if(view != 0L)
		{
			if(bOn)
				view->Enable();
			else
				view->Disable();
		}
	}
#endif
#ifdef WIN32
	if(pCmdUI != 0L)
	{
		// this is a menu
		pCmdUI->Enable(bOn);
	}
	else if(m_widget)
	{
		// this something else
		CWnd *wid = (CWnd *)m_widget;
		BOOL state = bOn;
		if(state != wid->IsWindowEnabled())
			wid->EnableWindow(state);
	}
#endif
#ifdef qGTK
	if(pCmdUI != 0L)
	{
		// this is a menu
		pCmdUI->Enable(bOn);
	}
	else if(m_widget != 0L)
	{
		if(GTK_IS_WIDGET(m_widget))
		{
			gtk_widget_set_sensitive(GTK_WIDGET(m_widget), bOn);
		}
	}
#endif
}

void UCmdUI::Check(bool bOn)
{
	m_continue = false;

#ifdef qMacAPP
	if((long)m_widget <= 2L)
	{
		// this is a menu
		if(m_widget == 0L)
		{
			UASSERT("You need to call UCmdUI::Enable before UCmdUI::Check !\n");
		}
		else
		{
			::EnableCheck(m_nID, m_widget == (void *)1L, bOn);
		}
	}
	else
	{
		TView *view = (TView *)m_widget;
		if(MA_MEMBER(view, TCheckBox))
		{
			((TCheckBox *)view)->SetState(bOn, kRedraw);
		}
		else if(MA_MEMBER(view, TRadio))
		{
			((TRadio *)view)->SetState(bOn, kRedraw);
		}
	}
#endif
#ifdef qMacPP
	if(m_wrapper != 0L)
	{
		// this is a menu
		*m_wrapper->outUsesMark = true;
		*m_wrapper->outMark = bOn ? checkMark : noMark;
	}
	else
	{
		LPane *view = (LPane *)m_widget;
		if(dynamic_cast<LCheckBox*>(view) != 0L ||
			dynamic_cast<LRadioButton*>(view) != 0L ||
			dynamic_cast<LCheckBoxGroupBox*>(view) != 0L ||
			dynamic_cast<LBevelButton*>(view) != 0L)
		{
			USemaphore policeman(gTurnOffBroadcast);

			view->SetValue(bOn ? Button_On : Button_Off);
		}
	}
#endif
#ifdef WIN32
	if(pCmdUI != 0L)
	{
		// this is a menu
		pCmdUI->SetCheck(bOn ? 1 : 0);
	}
	else if(m_widget)
	{
		CWnd *wnd = (CWnd *)m_widget;
		if(wnd->IsKindOf(RUNTIME_CLASS(CPseudoToggle)))
		{
			CPseudoToggle *btn = (CPseudoToggle *)wnd;
			btn->SetState(bOn);
		}
		else if(wnd->IsKindOf(RUNTIME_CLASS(CButton)))
		{
			CButton *btn = (CButton *)wnd;
			btn->SetCheck(bOn ? BST_CHECKED : BST_UNCHECKED);
		}
	}
#endif
#ifdef qGTK
	if(m_widget != 0L)
	{
		if(GTK_IS_TOGGLE_BUTTON(m_widget))
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_widget), bOn);
		} 
		else if(GTK_IS_RADIO_MENU_ITEM(m_widget))
		{
			if(bOn) {
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_widget), bOn);
			}
		}
	}
#endif
}

void UCmdUI::SetText(const char *title)
{
	m_continue = false;

#ifdef qMacAPP
	if((long)m_widget <= 2L)
	{
		// this is a menu
		CStr255_AC pstr(title);
		SetCommandName(m_nID, pstr);
	}
	else
	{
		TCtlMgr *view = (TCtlMgr *)m_widget;
		if(MA_MEMBER(view, TCtlMgr))
		{
			view->SetText(CStr255_AC(title), kRedraw);
		}
	}
#endif
#ifdef qGTK
	if(m_widget != 0L)
	{
		if(GTK_IS_TOGGLE_BUTTON(m_widget))
		{
			GtkLabel *label = getLabelFromContainer(GTK_CONTAINER(m_widget));
			if(label != 0L)
				gtk_label_set_text(label, title);
		}
	}
#endif
#ifdef qMacPP
	if(m_wrapper != 0L)
	{
		// this is a menu
		LStr255 pstr(title);
		LString::CopyPStr(pstr, m_wrapper->outName);
	}
	else
	{
		LPane *view = (LPane *)m_widget;
		LEditText *edit;
		LStaticText *stat;
		if((edit = dynamic_cast<LEditText*>(view)) != 0L)
		{
			edit->SetText((Ptr)title, strlen(title));
		}
		else if((stat = dynamic_cast<LStaticText*>(view)) != 0L)
		{
			stat->SetText((Ptr)title, strlen(title));
		}
	}
#endif
#ifdef WIN32
	if(pCmdUI != 0L)
	{
		// this is a menu
		if(title != 0L && *title != '\0')
			pCmdUI->SetText(title);
	}
#endif
}

void UCmdUI::SetTooltip(const char *tooltip)
{
	if(tooltip == 0L)
		return;

	char status[256] = {'\0'};
	char prompt[256] = {'\0'};

	strcpy(status, tooltip);
	char *cr = strchr(status, '\n');
	if(cr != 0L)
	{
		strcpy(prompt, cr + 1);
		*cr = '\0';
	}
#ifdef WIN32
#ifndef _countof
	/// Get the size of an array
#	define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif
	NMHDR *hdr = (NMHDR *)pNMHDR;
	ASSERT(hdr->code == TTN_NEEDTEXTA || hdr->code == TTN_NEEDTEXTW);

	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)hdr;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)hdr;
#	ifndef _UNICODE
	if (hdr->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, prompt, _countof(pTTTA->szText));
	else
		_mbstowcsz(pTTTW->szText, prompt, _countof(pTTTW->szText));
#	else
	if (hdr->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, prompt, _countof(pTTTA->szText));
	else
		lstrcpyn(pTTTW->szText, prompt, _countof(pTTTW->szText));
#	endif
#endif

	if(sTooltipHandler != 0L)
		sTooltipHandler(status);
}

void UEventSetTooltipHandler(void (*handler) (const char *))
{
	sTooltipHandler = handler;
}

void UEventGiveTime(void)
{
	STD map< int, UCmdTarget * >::const_iterator iter;
	const STD map<int, UCmdTarget *> & targets = sAllTargets.Get();

	// post the retained events
	while(!sRetainedMsgs.empty())
	{
		CMsg msg = *sRetainedMsgs.begin();
		sRetainedMsgs.erase(sRetainedMsgs.begin());
		UEventSendMessage(msg.m_wid, msg.m_ev, msg.m_arg1, msg.m_arg2);
	}

	// take care about the EV_UPDATEUI notification
	if(!UWidget::IsUIValid())
	{
		UWidget::DoValidUI(true);

		// warn the widget it's going to happen so they can cache
		// some informations.
		for(iter = targets.begin(); iter != targets.end(); ++iter)
		{
			UCmdTarget *target = (*iter).second;
			if(!target->IsKindOf(URUNTIME_CLASS(UWidget)))
				continue;

			UWidget *wid = (UWidget *)target;
			wid->BeforeUpdateUI();
		}

		for(iter = targets.begin(); iter != targets.end(); ++iter)
		{
			UCmdTarget *target = (*iter).second;
			if(!target->IsKindOf(URUNTIME_CLASS(UWidget)))
				continue;

			UWidget *wid = (UWidget *)target;
			UEventSendMessage(wid->GetWidID(), EV_UPDATEUI, 0, 0L);
		}
	}

	long thisIdle = UTickCount();
	if((thisIdle - sLastIdle) < MIN_IDLE)
		return;

	// search the timers
	for(iter = targets.begin(); iter != targets.end(); ++iter)
	{
		UCmdTarget *target = (*iter).second;
		if(!target->IsKindOf(URUNTIME_CLASS(UWidget)))
			continue;

		UWidget *wid = (UWidget *)target;
		std::vector<UTimer> & timers = wid->GetTimers();
		std::vector<UTimer>::iterator t;
		for(t = timers.begin(); t != timers.end(); ++t)
		{
			UTimer & timer = *t;
			if((thisIdle - timer.m_lastlap) >= timer.m_lap)
			{
				// trigger the timer
				timer.m_lastlap = thisIdle;
				UEventSendMessage(wid->GetWidID(), EV_TIMER, timer.m_cmd, (void *)thisIdle);
			}
		}
	}

	sLastIdle = thisIdle;
}

long UTickCount(void)
{
#ifdef WIN32
	return ::GetTickCount();
#elif defined(TARGET_OS_MAC)
	long res = ::TickCount();
	return res = (float)res * (1000.0 / 60.0);
#else
	struct timeval tval;
	static long refcount;

	gettimeofday(&tval, 0L);

	if(refcount == 0)
		refcount = tval.tv_sec;

	return (tval.tv_sec - refcount) * 1000 + tval.tv_usec / 1000;
#endif
}

const STD map<int, UCmdTarget *> & UEventGetTargets(void)
{
	return sAllTargets.Get();
}
