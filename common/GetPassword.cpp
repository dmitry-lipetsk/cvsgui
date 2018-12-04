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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

// GetPassword.cpp : implementation file
//

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LEditText.h>
#	include <LStaticText.h>

#	include "MacCvsApp.h"
#	include "MacCvsConstant.h"
#endif /* qMacCvsPP */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#include "GetPassword.h"
#include "CvsCommands.h"

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
// GetPassword dialog


GetPassword::GetPassword(CWnd* pParent /*=NULL*/)
	: CHHelpDialog(GetPassword::IDD, pParent)
{
	//{{AFX_DATA_INIT(GetPassword)
	m_passwd = _T("");
	//}}AFX_DATA_INIT
}


void GetPassword::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GetPassword)
	DDX_Text(pDX, IDC_PASSWORD, m_passwd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GetPassword, CHHelpDialog)
	//{{AFX_MSG_MAP(GetPassword)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GetPassword message handlers

/// OnInitDialog virtual override, set help IDs and bring window into the foreground
BOOL GetPassword::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization here
	if( !IsWindowVisible() )
	{
		// Force the window to the foreground in case it was covered by terminal or DOS box
		SetForegroundWindow();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void GetPassword::InitHelpID()
{
	SetCtrlHelpID(IDC_PASSWORD,	IDH_IDC_PASSWORD);
}

/// BN_CLICKED message handler, display dialog help
void GetPassword::OnHelp() 
{
	ShowHelp();
}

#endif /* WIN32 */

#if qUnix
class UCvsGetPasswd : public UWidget
{
	UDECLARE_DYNAMIC(UCvsGetPasswd)
public:
	UCvsGetPasswd() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsGetPasswd() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kPasswd					// 2
	};

	virtual void DoDataExchange(bool fill);

	std::string m_passwd;

protected:
	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsGetPasswd, UWidget)

UBEGIN_MESSAGE_MAP(UCvsGetPasswd, UWidget)
	ON_UCOMMAND(UCvsGetPasswd::kOK, UCvsGetPasswd::OnOK)
	ON_UCOMMAND(UCvsGetPasswd::kCancel, UCvsGetPasswd::OnCancel)
UEND_MESSAGE_MAP()

int UCvsGetPasswd::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsGetPasswd::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsGetPasswd::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kPasswd, &m_passwd);
	}
}
#endif // qUnix

/// Prompt for a password
char* CompatGetPassword(const char* prompt)
{
#ifdef WIN32
	static char passwd[255];
	GetPassword gp;

	if( gp.DoModal() == IDOK )
	{
		strcpy(passwd, gp.m_passwd);
		return passwd;
	}

	return "";
#endif /* WIN32 */
//{ Mac-Part
#ifdef qMacCvsPP
	if(!CMacCvsApp::gApp->IsFrontProcess() && CMacCvsApp::gApp->NotifyUser(0L))
	{
		while(!CMacCvsApp::gApp->IsFrontProcess())
		{
			CMacCvsApp::gApp->SpendSomeTime();
		}
	}

	static UStr thepasswd;
	KCItemRef theItem				= NULL;
	
	LStr255 pstr;
	UStr realPrompt;
#if !qCarbon || UNIVERSAL_INTERFACES_VERSION >= 0x0334
	LStr255 phost, puser;
	bool doUseKeyChain = false;
	OSStatus theStatus;
	UStr loopKey;
#endif

	if(GetEnvValue(prompt, "prompt", realPrompt))
	{
		pstr = realPrompt;

#if !qCarbon || UNIVERSAL_INTERFACES_VERSION >= 0x0334
		// copied from MacCvsPro::CMacCVSDoc.cpp
		UStr host, user, authen;
		if(CMacCvsApp::CanUseKeychain() &&
			GetEnvValue(prompt, "host", host) &&
			GetEnvValue(prompt, "user", user))
		{
			doUseKeyChain = true;
			
			OSStatus theStatus;
			UInt32 theLength;
			
			phost = host;
			puser = user;
			
			UStr authen;
			if(GetEnvValue(prompt, "authen", authen))
			{
				phost += ":";
				phost += authen;
			}
			
			UStr dir;
			if(GetEnvValue(prompt, "dir", dir))
			{
				phost += ":";
				phost += dir;
			}
			
			static char ppass[255];
			
			theStatus = KCFindGenericPassword(phost, puser, sizeof(ppass) - 1,
				ppass, &theLength, &theItem);
			
			user = puser;
			host = phost;
			loopKey = user;
			loopKey << '@';
			loopKey << host;
			// we could have an endless loop is the password is not valid
			if (theStatus == noErr && CMacCvsApp::gApp->CheckLoopingPassword(loopKey))
			{
				if (theItem != NULL)
					KCReleaseItem(&theItem);

				ppass[theLength] = '\0';
				
				return ppass;
			}
		}
#endif
	}
	else
		pstr = prompt;

	StDialogHandler	theHandler(dlg_Passwd, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);

	LEditText *passwd = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_GetPasswd));
	LStaticText *promptbox = dynamic_cast<LStaticText*>
		(theDialog->FindPaneByID('PROM'));
	theDialog->SetLatentSub(passwd);

	promptbox->SetText(pstr);
	
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
	}
	theDialog->Hide();
	
	if(hitMessage == msg_OK)
	{		
		passwd->GetText(pstr);

#if !qCarbon || UNIVERSAL_INTERFACES_VERSION >= 0x0334
		if (doUseKeyChain && CMacCvsApp::CanUseKeychain())
		{			
			if (theItem != NULL)
			{
				theStatus = KCSetData(theItem, pstr[0], &pstr[1]);
				
				if (theStatus == noErr)
					theStatus = KCUpdateItem(theItem);
			}
			else
			{
				theStatus = KCAddGenericPassword(phost, puser,
					pstr[0], &pstr[1], &theItem);
				
				KCAttribute theAttribute;
				
				if (theStatus == noErr)
				{
					UStr theDescriptionText("MacCVS password");
					theAttribute.tag = kDescriptionKCItemAttr;
					theAttribute.length = theDescriptionText.length();
					theAttribute.data = (void *)(const char *)theDescriptionText;
					theStatus = KCSetAttribute(theItem, &theAttribute);
				}
				
				OSType theTypeCreator;
				
				if (theStatus == noErr)
				{
					theTypeCreator = 'mCVS';
					theAttribute.tag = kCreatorKCItemAttr;
					theAttribute.length = sizeof(theTypeCreator);
					theAttribute.data = &theTypeCreator;
					theStatus = KCSetAttribute(theItem, &theAttribute);
				}
				
				if (theStatus == noErr)
				{
					theTypeCreator = 'APPL';
					theAttribute.tag = kTypeKCItemAttr;
					theStatus = KCSetAttribute(theItem, &theAttribute);
				}
				
				if (theStatus == noErr)
				{
					Boolean theCustomIcon = true;
					theAttribute.tag = kCustomIconKCItemAttr;
					theAttribute.length = sizeof(theCustomIcon);
					theAttribute.data = &theCustomIcon;
					theStatus = KCSetAttribute(theItem, &theAttribute);
				}
				
				if (theStatus == noErr)
					theStatus = KCUpdateItem(theItem);
			}

			if (theItem != NULL)
				KCReleaseItem(&theItem);
			
			CMacCvsApp::gApp->PushValidPassword(loopKey);
		}
#endif

		thepasswd = pstr;
		return thepasswd.length() == 0 ? (char *)"" : (char *)thepasswd.c_str();
	}

#if !qCarbon || UNIVERSAL_INTERFACES_VERSION >= 0x0334
	if (theItem != NULL)
		KCReleaseItem(&theItem);
#endif

	return "";
#endif /* qMacCvsPP */
//}
#if qUnix
	static char passwd[255];
	void *wid = UCreate_PasswdDlg();

	UCvsGetPasswd *dlg = new UCvsGetPasswd();
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	

	if(dlg->DoModal())
	{
		strcpy(passwd, dlg->m_passwd.c_str());
	}
	else
		strcpy(passwd, "");

	delete dlg;
	return passwd;
#endif // qUnix
}
