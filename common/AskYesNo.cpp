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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

// AskYesNo.cpp : ask yes or no !

#include "stdafx.h"

#ifdef WIN32
#	include "resource.h"
#endif /* WIN32 */

#ifdef qMacCvsPP
#	include "CvsAlert.h"
#	include "MacCvsApp.h"
#	include "MacCvsConstant.h"
#	include "LogWindow.h"
# include <memory>
#endif /* qMacCvsPP */

#include <string>

#include "AskYesNo.h"
#include "uwidget.h"

#ifdef qUnix
#	include "UCvsDialogs.h"
#endif /* qUnix */

#include "cvsgui_i18n.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class UPromptMessage : public UWidget
{
	UDECLARE_DYNAMIC(UPromptMessage)
public:
	UPromptMessage(const char *msg, const char *title, const char *def,
		const char *cncl) : UWidget(UEventGetWidID()), m_msg(msg), m_title(title),
		m_ok(def), m_cncl(cncl)
	{
	}

	virtual ~UPromptMessage() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kMsg					// 2
	};

	virtual void DoDataExchange(bool fill)
	{
		if(fill)
		{
			UEventSendMessage(GetWidID(), EV_SETTEXT, kOK, (void *)m_ok.c_str());
			UEventSendMessage(GetWidID(), EV_SETTEXT, kUMainWidget, (void *)m_title.c_str());
			UEventSendMessage(GetWidID(), EV_SETTEXT, kCancel, (void *)m_cncl.c_str());
			UEventSendMessage(GetWidID(), EV_SETTEXT, kMsg, (void *)m_msg.c_str());
		}
		else
		{
		}
	}

protected:
	string m_msg, m_title, m_ok, m_cncl;

	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UPromptMessage, UWidget)

UBEGIN_MESSAGE_MAP(UPromptMessage, UWidget)
	ON_UCOMMAND(kOK, UPromptMessage::OnOK)
	ON_UCOMMAND(kCancel, UPromptMessage::OnCancel)
UEND_MESSAGE_MAP()

int UPromptMessage::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UPromptMessage::OnCancel(void)
{
	EndModal(false);
	return 0;
}

class UEditMessage : public UPromptMessage
{
	UDECLARE_DYNAMIC(UEditMessage)
public:
	UEditMessage(const char* msg, const char* edit, const char* title, const char* def,	const char* cncl) 
		: UPromptMessage(msg, title, def, cncl), m_edit(edit)
	{
	}

	virtual ~UEditMessage() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kMsg,					// 2
		kEdit					// 3
	};

	virtual void DoDataExchange(bool fill)
	{
		UPromptMessage::DoDataExchange(fill);

		if(fill)
		{
			UEventSendMessage(GetWidID(), EV_SETTEXT, kEdit, (void *)m_edit.c_str());
		}
		else
		{
			UEventSendMessage(GetWidID(), EV_GETTEXT, kEdit, &m_edit);
		}
	}

	string m_edit;

protected:

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UEditMessage, UPromptMessage)

UBEGIN_MESSAGE_MAP(UEditMessage, UPromptMessage)
UEND_MESSAGE_MAP()

/*!
	Prompt for yes or no, returns "yes" or "no" and the value of the edit field
	\param msg Message
	\param edit Return the value of the edit field
	\param title Title
	\param def Default button title
	\param cncl Cancel button title
	\return true on success, false otherwise
	\note Obsolete, CvsPrompt should be used instead
*/
bool PromptEditMessage(const char* msg, std::string& edit, const char* title /*= "Message"*/,
					   const char* def /*= "OK"*/, const char* cncl /*= "Cancel"*/)
{
	bool res = false;

#if qUnix
	void *wid = UCreate_AskYesNoDlg();
	UEditMessage* dlg = new UEditMessage(msg, edit.c_str(), title, def, cncl);
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	res = dlg->DoModal();

#else

	std::auto_ptr<UEditMessage> dlg(new UEditMessage(msg, edit.c_str(), title, def, cncl));

#ifdef WIN32
	res = dlg->DoModal(IDD_EDITMESSAGE) ? true : false;
#endif
#ifdef qMacCvsPP
	res = dlg->DoModal(dlg_PromptEditCommand);
#endif

#endif
	
	if( res )
		edit = dlg->m_edit;

	return res;
}
