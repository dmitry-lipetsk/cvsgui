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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * LogDlg.cpp : the cvs log dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "OptionalPropertySheet.h"
#endif /* WIN32 */

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LMultiPanelView.h>
#	include <LCheckBox.h>
#	include <LEditText.h>
#	include <LPopupGroupBox.h>
#	include <LRadioButton.h>
#	include "LPopupFiller.h"

#	include "MacCvsConstant.h"
# include "UCommandDialogs.h"
# include "LogParse.h"
#endif /* qMacCvsPP */

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#include "LogDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"
#include "MultiString.h"
#include "ItemListDlg.h"

using namespace std;

/// Default log tag names
char* sDefLogTagNames[] = 
{
	"1.32:1.35",
	"1.32:",
	":1.35",
	"branch-release1-0",
	"HEAD",
	0L
};

/// Default log date names
char* sDefLogDateNames[] = 
{
	">1998-3-24",
	"<1998-1-24",
	"1998-1-24<1998-3-24",
	"<=3-24",
	0L
};

/// Default log state names
char* sDefLogStateNames[] = 
{
	"mystate",
	"dead",
	"exp",
	"mystate,mystate2",
	0L
};

/// Default log user names
char* sDefLogUserNames[] = 
{
	"johna",
	"garyb",
	"johna,garyb",
	0L
};

static CMString gLogRevNames(10, "P_LogTagNames", sDefLogTagNames);
static CMString gLogDateNames(10, "P_LogDateNames", sDefLogDateNames);
static CMString gLogStateNames(10, "P_LogStateNames", sDefLogStateNames);
static CMString gLogUserNames(10, "P_LogUserNames", sDefLogUserNames);

static CPersistentBool gLogSupress("P_LogSupress", false, kAddSettings);
static CPersistentBool gLogCvsRcsOutput("P_LogCvsRcsOutput", false, kAddSettings);
static CPersistentBool gLogNoRecurs("P_LogNoRecurs", false, kAddSettings);
static CPersistentBool gLogDefBranch("P_LogDefBranch", false, kAddSettings);
static CPersistentBool gLogHeaderOnly("P_LogHeaderOnly", false, kAddSettings);
static CPersistentBool gLogHeaderDesc("P_LogHeaderDesc", false, kAddSettings);
static CPersistentBool gLogNoTags("P_LogNoTags", false, kAddSettings);
static CPersistentBool gLogOnlyRCSfile("P_LogOnlyRCSfile", false, kAddSettings);

static CPersistentBool gGraphSupress("P_GraphSupress", false, kAddSettings);
static CPersistentBool gGraphCvsRcsOutput("P_GraphCvsRcsOutput", false, kAddSettings);
static CPersistentBool gGraphNoRecurs("P_GraphNoRecurs", false, kAddSettings);
static CPersistentBool gGraphDefBranch("P_GraphDefBranch", false, kAddSettings);
static CPersistentBool gGraphHeaderOnly("P_GraphHeaderOnly", false, kAddSettings);
static CPersistentBool gGraphHeaderDesc("P_GraphHeaderDesc", false, kAddSettings);
static CPersistentBool gGraphNoTags("P_GraphNoTags", false, kAddSettings);
static CPersistentBool gGraphOnlyRCSfile("P_GraphOnlyRCSfile", false, kAddSettings);


#ifdef WIN32
#include "GetPrefs.h"

//////////////////////////////////////////////////////////////////////////
// CLogMain

IMPLEMENT_DYNAMIC(CLogMain, CHHelpPropertyPage)

CLogMain::CLogMain(bool supress, bool cvsRcsOutput, bool noRecurs, bool defBranch, bool headerOnly, bool headerDesc, bool noTags, bool onlyRCSfile) 
	: CHHelpPropertyPage(CLogMain::IDD)
{
	//{{AFX_DATA_INIT(CLogMain)
	m_supress = supress;
	m_defBranch = defBranch;
	m_noRecurs = noRecurs;
	m_noTags = noTags;
	m_onlyRCS = onlyRCSfile;
	m_headerOnly = headerOnly;
	m_headerDesc = headerDesc;
	m_cvsRcsOutput = cvsRcsOutput;
	//}}AFX_DATA_INIT
}

CLogMain::~CLogMain()
{
}

void CLogMain::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogMain)
	DDX_Control(pDX, IDC_HEADERS_DESC, m_headerDescCheck);
	DDX_Control(pDX, IDC_HEADERS, m_headerOnlyCheck);
	DDX_Check(pDX, IDC_SUPRESS_HEADER, m_supress);
	DDX_Check(pDX, IDC_DEFAULT_BRANCH, m_defBranch);
	DDX_Check(pDX, IDC_NORECURS, m_noRecurs);
	DDX_Check(pDX, IDC_NOTAGS, m_noTags);
	DDX_Check(pDX, IDC_RCS_FILENAMES_ONLY, m_onlyRCS);
	DDX_Check(pDX, IDC_HEADERS, m_headerOnly);
	DDX_Check(pDX, IDC_HEADERS_DESC, m_headerDesc);
	DDX_Check(pDX, IDC_CVSRCSOUTPUT, m_cvsRcsOutput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogMain, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CLogMain)
	ON_BN_CLICKED(IDC_HEADERS, OnHeaders)
	ON_BN_CLICKED(IDC_HEADERS_DESC, OnHeadersdesc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogFilters message handlers

void CLogMain::InitHelpID()
{
	SetCtrlHelpID(IDC_SUPRESS_HEADER,		IDH_IDC_SUPRESS_HEADER);
	SetCtrlHelpID(IDC_CVSRCSOUTPUT,			IDH_IDC_CVSRCSOUTPUT);
	SetCtrlHelpID(IDC_DEFAULT_BRANCH,		IDH_IDC_DEFAULT_BRANCH);
	SetCtrlHelpID(IDC_NORECURS,				IDH_IDC_NORECURS);
	SetCtrlHelpID(IDC_RCS_FILENAMES_ONLY,	IDH_IDC_RCS_FILENAMES_ONLY);
	SetCtrlHelpID(IDC_NOTAGS,				IDH_IDC_NOTAGS);
	SetCtrlHelpID(IDC_HEADERS,				IDH_IDC_HEADERS);
	SetCtrlHelpID(IDC_HEADERS_DESC,			IDH_IDC_HEADERS_DESC);
}

/// BN_CLICKED message handler, reset headers desc if set
void CLogMain::OnHeaders() 
{
	if( m_headerOnlyCheck.GetCheck() && m_headerDescCheck.GetCheck() )
	{
		m_headerDescCheck.SetCheck(0);
	}
}

/// BN_CLICKED message handler, reset headers only if set
void CLogMain::OnHeadersdesc() 
{
	if( m_headerDescCheck.GetCheck() && m_headerOnlyCheck.GetCheck() )
	{
		m_headerOnlyCheck.SetCheck(0);
	}
}

//////////////////////////////////////////////////////////////////////////
// CLogFilters

IMPLEMENT_DYNAMIC(CLogFilters, CHHelpPropertyPage)

CLogFilters::CLogFilters(const MultiFiles* mf,
						 const char* sLastDate, const char* sLastRev,
						 const char* sLastUsers, const char* sLastStates) 
						 : m_mf(mf), CHHelpPropertyPage(CLogFilters::IDD)
{
	//{{AFX_DATA_INIT(CLogFilters)
	m_date = sLastDate;
	m_rev = sLastRev;
	m_state = sLastStates;
	m_user = sLastUsers;
	m_hasDate = FALSE;
	m_hasRev = FALSE;
	m_hasState = FALSE;
	m_hasUser = FALSE;
	//}}AFX_DATA_INIT

	m_revCombo.SetItems(&gLogRevNames);
	m_dateCombo.SetItems(&gLogDateNames);
	m_userCombo.SetItems(&gLogUserNames);
	m_stateCombo.SetItems(&gLogStateNames);
}

CLogFilters::~CLogFilters()
{
}

void CLogFilters::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogFilters)
	DDX_Control(pDX, IDC_HAS_USERS, m_hasUserCheck);
	DDX_Control(pDX, IDC_HAS_STATES, m_hasStateCheck);
	DDX_Control(pDX, IDC_HAS_REV, m_hasRevCheck);
	DDX_Control(pDX, IDC_HAS_DATE, m_hasDateCheck);
	DDX_Control(pDX, IDC_BROWSE_TAG, m_browseTagButton);
	DDX_Control(pDX, IDC_USERS, m_userCombo);
	DDX_Control(pDX, IDC_STATES, m_stateCombo);
	DDX_Control(pDX, IDC_RANGE_REV_TAG, m_revCombo);
	DDX_Control(pDX, IDC_RANGE_DATE, m_dateCombo);
	DDX_CBString(pDX, IDC_RANGE_DATE, m_date);
	DDX_CBString(pDX, IDC_RANGE_REV_TAG, m_rev);
	DDX_CBString(pDX, IDC_STATES, m_state);
	DDX_CBString(pDX, IDC_USERS, m_user);
	DDX_Check(pDX, IDC_HAS_DATE, m_hasDate);
	DDX_Check(pDX, IDC_HAS_REV, m_hasRev);
	DDX_Check(pDX, IDC_HAS_STATES, m_hasState);
	DDX_Check(pDX, IDC_HAS_USERS, m_hasUser);
	//}}AFX_DATA_MAP
	
	if( m_hasDate )
	{
		DDV_MinChars(pDX, m_date, 1);
	}

	if( m_hasState )
	{
		DDV_MinChars(pDX, m_state, 1);
	}

	if( !pDX->m_bSaveAndValidate )
	{
		OnRev();
		OnDate();
		OnState();
		OnUser();
	}

	DDX_ComboMString(pDX, IDC_RANGE_REV_TAG, m_revCombo);
	DDX_ComboMString(pDX, IDC_RANGE_DATE, m_dateCombo);
	DDX_ComboMString(pDX, IDC_USERS, m_userCombo);
	DDX_ComboMString(pDX, IDC_STATES, m_stateCombo);
}


BEGIN_MESSAGE_MAP(CLogFilters, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CLogFilters)
	ON_BN_CLICKED(IDC_HAS_DATE, OnDate)
	ON_BN_CLICKED(IDC_HAS_REV, OnRev)
	ON_BN_CLICKED(IDC_HAS_STATES, OnState)
	ON_BN_CLICKED(IDC_HAS_USERS, OnUser)
	ON_BN_CLICKED(IDC_BROWSE_TAG, OnBrowseTag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogFilters message handlers

void CLogFilters::InitHelpID()
{
	SetCtrlHelpID(IDC_HAS_REV,			IDH_IDC_HAS_REV);
	SetCtrlHelpID(IDC_RANGE_REV_TAG,	IDH_IDC_RANGE_REV_TAG);
	SetCtrlHelpID(IDC_BROWSE_TAG,		IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_HAS_DATE,			IDH_IDC_HAS_DATE);
	SetCtrlHelpID(IDC_RANGE_DATE,		IDH_IDC_RANGE_DATE);
	SetCtrlHelpID(IDC_HAS_STATES,		IDH_IDC_HAS_STATES);
	SetCtrlHelpID(IDC_STATES,			IDH_IDC_STATES);
	SetCtrlHelpID(IDC_HAS_USERS,		IDH_IDC_HAS_USERS);
	SetCtrlHelpID(IDC_USERS,			IDH_IDC_USERS);
}

/// BN_CLICKED message handler, enable date entry window
void CLogFilters::OnDate() 
{
	m_dateCombo.EnableWindow(m_hasDateCheck.GetCheck());
}

/// BN_CLICKED message handler, enable revision entry window
void CLogFilters::OnRev() 
{
	m_revCombo.EnableWindow(m_hasRevCheck.GetCheck());
	m_browseTagButton.EnableWindow(m_hasRevCheck.GetCheck());
}

/// BN_CLICKED message handler, browse for branch/tag
void CLogFilters::OnBrowseTag() 
{
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName) )
	{
		m_rev = tagName.c_str();
		m_revCombo.SetWindowText(m_rev);
	}
}

/// BN_CLICKED message handler, enable state entry window
void CLogFilters::OnState() 
{
	m_stateCombo.EnableWindow(m_hasStateCheck.GetCheck());
}

/// BN_CLICKED message handler, enable user entry window
void CLogFilters::OnUser() 
{
	m_userCombo.EnableWindow(m_hasUserCheck.GetCheck());
}
#endif /* WIN32 */

#ifdef qMacCvsPP
static void DoDataExchange_LogFilters(LWindow *theDialog, bool & hasDate, bool & hasStates,
		bool & hasRev, bool & hasUsers,
		std::string  & sLastDate, std::string  & sLastRev,
		std::string  & sLastUsers, std::string  & sLastStates, bool putValue)
{
	LCheckBox *chkd = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_LogCheckByDate));
	LEditText *editd = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_LogEditByDate));
	LCheckBox *chks = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_LogCheckByState));
	LEditText *edits = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_LogEditByState));
	LCheckBox *chkr = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_LogCheckByRev));
	LEditText *editr = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_LogEditByRev));
	LCheckBox *chku = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_LogCheckByUser));
	LEditText *editu = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_LogEditByUser));
	LPopupFiller *fillDate = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_LogFillByDate));
	LPopupFiller *fillState = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_LogFillByState));
	LPopupFiller *fillRev = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_LogFillByRev));
	LPopupFiller *fillUser = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_LogFillByUser));

	fillDate->DoDataExchange(gLogDateNames, putValue);
	fillState->DoDataExchange(gLogStateNames, putValue);
	fillRev->DoDataExchange(gLogRevNames, putValue);
	fillUser->DoDataExchange(gLogUserNames, putValue);

	if(putValue)
	{
		chkd->SetValue(hasDate ? Button_On : Button_Off);
		editd->SetText((Ptr)(const char *)sLastDate, sLastDate.length());

		chks->SetValue(hasStates ? Button_On : Button_Off);
		edits->SetText((Ptr)(const char *)sLastStates, sLastStates.length());

		chkr->SetValue(hasRev ? Button_On : Button_Off);
		editr->SetText((Ptr)(const char *)sLastRev, sLastRev.length());

		chku->SetValue(hasUsers ? Button_On : Button_Off);
		editu->SetText((Ptr)(const char *)sLastUsers, sLastUsers.length());
	}
	else
	{
		char str[255];
		Size len;

		hasDate = chkd->GetValue() == Button_On;
		editd->GetText(str, 254, &len);
		sLastDate.set(str, len);

		hasStates = chks->GetValue() == Button_On;
		edits->GetText(str, 254, &len);
		sLastStates.set(str, len);

		hasRev = chkr->GetValue() == Button_On;
		editr->GetText(str, 254, &len);
		sLastRev.set(str, len);

		hasUsers = chku->GetValue() == Button_On;
		editu->GetText(str, 254, &len);
		sLastUsers.set(str, len);
	}
}

static void DoDataExchange_LogSettings(LWindow *theDialog, bool & noRecurs, bool & defBranch,
	bool & headerOnly, bool & noTags, bool & onlyRCSfile, UInt16 & msgEncoding, bool putValue)
{
	LCheckBox *chkNoRecurs = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_NoRecurs));
	LCheckBox *chkDefBranch = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_LogDefBranch));
	LCheckBox *chkHeaderOnly = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_LogNoUserLogs));
	LCheckBox *chkNoTags = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_LogNoTags));
	LCheckBox *chkOnlyRCSfile = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_LogOnlyRcs));
	LPopupButton *messageEncoding = dynamic_cast<LPopupButton*>
	  (theDialog->FindPaneByID(item_LogMsgEncoding));

	if(putValue)
	{
		chkNoRecurs->SetValue(noRecurs ? Button_On : Button_Off);
		chkDefBranch->SetValue(defBranch ? Button_On : Button_Off);
		chkHeaderOnly->SetValue(headerOnly ? Button_On : Button_Off);
		chkNoTags->SetValue(noTags ? Button_On : Button_Off);
		chkOnlyRCSfile->SetValue(onlyRCSfile ? Button_On : Button_Off);
		messageEncoding->SetValue(msgEncoding);
	}
	else
	{
		noRecurs = chkNoRecurs->GetValue() == Button_On;
		defBranch = chkDefBranch->GetValue() == Button_On;
		headerOnly = chkHeaderOnly->GetValue() == Button_On;
		noTags = chkNoTags->GetValue() == Button_On;
		onlyRCSfile = chkOnlyRCSfile->GetValue() == Button_On;
		msgEncoding = messageEncoding->GetValue();
	}
}
#endif /* qMacCvsPP */

#if qUnix
class UCvsLog : public UWidget
{
	UDECLARE_DYNAMIC(UCvsLog)
public:
	UCvsLog() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsLog() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kTabGeneral				// 2
	};

	virtual void DoDataExchange(bool fill);

protected:
	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsLog, UWidget)

UBEGIN_MESSAGE_MAP(UCvsLog, UWidget)
	ON_UCOMMAND(UCvsLog::kOK, UCvsLog::OnOK)
	ON_UCOMMAND(UCvsLog::kCancel, UCvsLog::OnCancel)
UEND_MESSAGE_MAP()

int UCvsLog::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsLog::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsLog::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsLog_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsLog_MAIN)
public:
	UCvsLog_MAIN();
	virtual ~UCvsLog_MAIN() {}

	enum
	{
		kCheckBranch = EV_COMMAND_START,	// 0
		kCheckRecurs,			// 1
		kCheckRCS,				// 2
		kCheckTags,				// 3
		kCheckUsers				// 4
	};

	virtual void DoDataExchange(bool fill);

	bool m_defbranch;
	bool m_norecurs;
	bool m_notags;
	bool m_nousers;
	bool m_onlyrcs;

protected:
	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsLog_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsLog_MAIN, UWidget)
UEND_MESSAGE_MAP()

UCvsLog_MAIN::UCvsLog_MAIN() : UWidget(::UEventGetWidID())
{
	m_defbranch = false;
	m_norecurs = false;
	m_notags = false;
	m_nousers = false;
	m_onlyrcs = false;
}

void UCvsLog_MAIN::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckBranch, m_defbranch), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurs, m_norecurs), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckTags, m_notags), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckUsers, m_nousers), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRCS, m_onlyrcs), 0L);
	}
	else
	{
		m_defbranch = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckBranch, 0L);
		m_norecurs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurs, 0L);
		m_notags = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckTags, 0L);
		m_nousers = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckUsers, 0L);
		m_onlyrcs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRCS, 0L);
	}
}

class UCvsLog_FILTERS : public UWidget
{
	UDECLARE_DYNAMIC(UCvsLog_FILTERS)
public:
	UCvsLog_FILTERS(const char * sLastDate, const char * sLastRev,
		const char * sLastUsers, const char * sLastStates);
	virtual ~UCvsLog_FILTERS() {}

	enum
	{
		kCheckTag = EV_COMMAND_START,	// 0
		kCheckDate,			// 1
		kCheckState,		// 2
		kCheckUser,			// 3
		kComboTag,			// 4
		kComboDate,			// 5
		kComboState,		// 6
		kComboUser			// 7
	};

	virtual void DoDataExchange(bool fill);

	std::string  m_date;
	std::string  m_rev;
	std::string  m_state;
	std::string  m_user;
	bool m_hasrev;
	bool m_hasuser;
	bool m_hasdate;
	bool m_hasstate;
protected:

	ev_msg int OnSelect(int cmd);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsLog_FILTERS, UWidget)

UBEGIN_MESSAGE_MAP(UCvsLog_FILTERS, UWidget)
	ON_UCOMMAND_RANGE(UCvsLog_FILTERS::kCheckTag, UCvsLog_FILTERS::kCheckUser, UCvsLog_FILTERS::OnSelect)
UEND_MESSAGE_MAP()

UCvsLog_FILTERS::UCvsLog_FILTERS(const char * sLastDate, const char * sLastRev,
		const char * sLastUsers, const char * sLastStates) : UWidget(::UEventGetWidID())
{
	m_date = sLastDate;
	m_rev = sLastRev;
	m_state = sLastStates;
	m_user = sLastUsers;
	m_hasrev = false;
	m_hasuser = false;
	m_hasdate = false;
	m_hasstate = false;
}

int UCvsLog_FILTERS::OnSelect(int cmd)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, cmd, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(cmd + 4, state), 0L);
	return 0;
}

void UCvsLog_FILTERS::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kComboTag, gLogRevNames);
	::DoDataExchange(fill, GetWidID(), kComboDate, gLogDateNames);
	::DoDataExchange(fill, GetWidID(), kComboUser, gLogUserNames);
	::DoDataExchange(fill, GetWidID(), kComboState, gLogStateNames);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckDate, m_hasdate), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckTag, m_hasrev), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckState, m_hasstate), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckUser, m_hasuser), 0L);

		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboDate, (void *)(const char *)m_date.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboTag, (void *)(const char *)m_rev.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboState, (void *)(const char *)m_state.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboUser, (void *)(const char *)m_user.c_str());

		OnSelect(kCheckDate);
		OnSelect(kCheckTag);
		OnSelect(kCheckUser);
		OnSelect(kCheckState);
	}
	else
	{
		m_hasdate = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDate, 0L);
		m_hasrev = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckTag, 0L);
		m_hasstate = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckState, 0L);
		m_hasuser = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckUser, 0L);

		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboDate, &m_date);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboTag, &m_rev);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboState, &m_state);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboUser, &m_user);
	}
}
#endif // qUnix

/// Get the log options
bool CompatGetLog(const MultiFiles* mf,
				  bool& noRecurs, bool& defBranch, std::string& date,
				  bool& headerOnly, bool& headerDesc, bool& supress, bool& cvsRcsOutput, 
				  bool& noTags, bool& onlyRCSfile, bool& hasRev,
				  std::string& rev, std::string& states, bool& hasUsers, std::string& users,
				  bool outGraph /*= false*/)
{
	bool userHitOK = false;

	static string sLastDate;
	static string sLastRev;
	static string sLastUsers;
	static string sLastStates;

	bool hasDate = false;
	bool hasStates = false;

	hasRev = false;
	hasUsers = false;
	date = "";
	rev = "";
	states = "";
	users = "";
	supress =		outGraph ? (bool)gGraphSupress		: (bool)gLogSupress;
	cvsRcsOutput =	outGraph ? (bool)gGraphCvsRcsOutput	: (bool)gLogCvsRcsOutput;
	noRecurs =		outGraph ? (bool)gGraphNoRecurs		: (bool)gLogNoRecurs;
	defBranch =		outGraph ? (bool)gGraphDefBranch	: (bool)gLogDefBranch;
	headerOnly =	outGraph ? (bool)gGraphHeaderOnly	: (bool)gLogHeaderOnly;
	headerDesc =	outGraph ? (bool)gGraphHeaderDesc	: (bool)gLogHeaderDesc;
	noTags =		outGraph ? (bool)gGraphNoTags		: (bool)gLogNoTags;
	onlyRCSfile =	outGraph ? (bool)gGraphOnlyRCSfile	: (bool)gLogOnlyRCSfile;

#ifdef WIN32
	COptionalPropertySheet pages(outGraph ? gCvsPrefs.HideCommandDlgGraph() : gCvsPrefs.HideCommandDlgLog(), 
		outGraph ? "Graph settings" : "Log settings");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CLogMain page1(supress, cvsRcsOutput, noRecurs, defBranch, headerOnly, headerDesc, noTags, onlyRCSfile);
	CLogFilters page2(mf, sLastDate.c_str(), sLastRev.c_str(), sLastUsers.c_str(), sLastStates.c_str());
	CGetPrefs_GLOBALS page3;

	pages.AddPage(&page1);
	pages.AddPage(&page2);
	pages.AddPage(&page3);

	if( pages.DoModal() == IDOK )
	{
		supress = page1.m_supress ? true : false;
		cvsRcsOutput = page1.m_cvsRcsOutput ? true : false;
		noRecurs = page1.m_noRecurs == TRUE;
		defBranch = page1.m_defBranch == TRUE;
		headerOnly = page1.m_headerOnly == TRUE;
		headerDesc = page1.m_headerDesc == TRUE;
		noTags = page1.m_noTags == TRUE;
		onlyRCSfile = page1.m_onlyRCS == TRUE;

		sLastDate = page2.m_date;
		sLastRev = page2.m_rev;
		sLastUsers = page2.m_user;
		sLastStates = page2.m_state;

		hasRev = page2.m_hasRev == TRUE;
		hasUsers = page2.m_hasUser == TRUE;
		hasDate = page2.m_hasDate == TRUE;
		hasStates = page2.m_hasState == TRUE;

		page3.StoreValues();

		outGraph ? gCvsPrefs.SetHideCommandDlgGraph(pages.GetHideCommandDlg()) : gCvsPrefs.SetHideCommandDlgLog(pages.GetHideCommandDlg());

		userHitOK = true;
	}
#endif /* WIN32 */
#ifdef qMacCvsPP
	StOptionalDialogHandler	theHandler(dlg_Log, gCvsPrefs.HideCommandDlgLog(), LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	static UInt16 sRuntimePanel = 1;
	UInt16      msgEncoding(sCommitMsgEncoding);
	
	LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
		(theDialog->FindPaneByID(item_MultiViewPrefs));
	LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
	
	multiView->SwitchToPanel(3);
	DoDataExchange_Globals(theDialog, true);
	multiView->SwitchToPanel(2);
	DoDataExchange_LogFilters(theDialog, hasDate, hasStates, hasRev, hasUsers,
		sLastDate, sLastRev, sLastUsers, sLastStates, true);
	multiView->SwitchToPanel(1);
	DoDataExchange_LogSettings(theDialog, noRecurs, defBranch, headerOnly, noTags, onlyRCSfile, msgEncoding, true);

	groupView->SetValue(sRuntimePanel);
	theHandler.ShowDialog();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
	}
	theDialog->Hide();
	sRuntimePanel = groupView->GetValue();
	
	if(hitMessage == msg_OK)
	{
		multiView->SwitchToPanel(1);
		DoDataExchange_LogSettings(theDialog, noRecurs, defBranch, headerOnly, noTags, onlyRCSfile, msgEncoding, false);
		sCommitMsgEncoding = msgEncoding;
		multiView->SwitchToPanel(2);
		DoDataExchange_LogFilters(theDialog, hasDate, hasStates, hasRev, hasUsers,
		sLastDate, sLastRev, sLastUsers, sLastStates, false);
		multiView->SwitchToPanel(3);
		DoDataExchange_Globals(theDialog, false);
		gCvsPrefs.SetHideCommandDlgLog(theHandler.GetHideCommandDlg());
		userHitOK = true;
	}
#endif /* qMacCvsPP */
#if qUnix
	void *wid = UCreate_LogDlg();

	UCvsLog *dlg = new UCvsLog();
	UCvsLog_MAIN *tab1 = new UCvsLog_MAIN();
	UCvsLog_FILTERS *tab2 = new UCvsLog_FILTERS(sLastDate.c_str(), sLastRev.c_str(), sLastUsers.c_str(), sLastStates.c_str());
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	dlg->AddPage(tab1, UCvsLog::kTabGeneral, 0);
	dlg->AddPage(tab2, UCvsLog::kTabGeneral, 1);

	if(dlg->DoModal())
	{
		noRecurs = tab1->m_norecurs;
		defBranch = tab1->m_defbranch;
		headerOnly = tab1->m_nousers;
		noTags = tab1->m_notags;
		onlyRCSfile = tab1->m_onlyrcs;

		sLastDate = tab2->m_date;
		sLastRev = tab2->m_rev;
		sLastUsers = tab2->m_user;
		sLastStates = tab2->m_state;

		hasRev = tab2->m_hasrev;
		hasUsers = tab2->m_hasuser;
		hasDate = tab2->m_hasdate;
		hasStates = tab2->m_hasstate;

		userHitOK = true;
	}

	delete dlg;
#endif // qUnix

	if( userHitOK )
	{
		outGraph ? gGraphSupress		= supress      : gLogSupress		= supress;
		outGraph ? gGraphCvsRcsOutput	= cvsRcsOutput : gLogCvsRcsOutput	= cvsRcsOutput;
		outGraph ? gGraphNoRecurs		= noRecurs     : gLogNoRecurs		= noRecurs;
		outGraph ? gGraphDefBranch		= defBranch    : gLogDefBranch		= defBranch;
		outGraph ? gGraphHeaderOnly		= headerOnly   : gLogHeaderOnly		= headerOnly;
		outGraph ? gGraphHeaderDesc		= headerDesc   : gLogHeaderDesc		= headerDesc;
		outGraph ? gGraphNoTags			= noTags       : gLogNoTags			= noTags;
		outGraph ? gGraphOnlyRCSfile	= onlyRCSfile  : gLogOnlyRCSfile	= onlyRCSfile;

		if( hasDate && !sLastDate.empty() )
			date = sLastDate;
		
		if( hasRev )
			rev = sLastRev;
		
		if( hasUsers )
			users = sLastUsers;

		if( hasStates && !sLastStates.empty() )
			states = sLastStates;

		gCvsPrefs.save();
	}

	return userHitOK;
}
