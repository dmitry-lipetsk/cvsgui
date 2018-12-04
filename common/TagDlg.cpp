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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- July 1998
 */

/*
 * TagDlg.cpp : the cvs tag dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "GetPrefs.h"
#endif /* WIN32 */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LMultiPanelView.h>
#	include <LCheckBox.h>
#	include <LEditText.h>
#	include <LPopupGroupBox.h>
#	include <LRadioButton.h>
#	include "LPopupFiller.h"

#	include "MacCvsConstant.h"
#endif /* qMacCvsPP */

#include "TagDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"
#include "UpdateDlg.h"
#include "ItemListDlg.h"
#include "RtagDlg.h"

using namespace std;

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CTag_MAIN property page

IMPLEMENT_DYNAMIC(CTag_MAIN, CHHelpPropertyPage)

CTag_MAIN::CTag_MAIN(const MultiFiles* mf, const kTagCmdType type, 
					 const char* sLastTag, 
					 bool noRecurs, bool movDelBranch, 
					 bool checkUnmod /*= false*/, bool overwriteExisting /*= false*/) 
					 : m_mf(mf), CHHelpPropertyPage(CTag_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CTag_MAIN)
	//}}AFX_DATA_INIT

	m_type = type;

	m_noRecurs = noRecurs;
	m_movDelBranch = movDelBranch;
	m_checkUnmod = checkUnmod;
	m_overwriteExisting = overwriteExisting;
	m_tagName = sLastTag;

	m_tagCombo.SetItems(&gRevNames);
}

CTag_MAIN::~CTag_MAIN()
{
}

void CTag_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTag_MAIN)
	DDX_Control(pDX, IDC_BROWSE_TAG, m_browseTagButton);
	DDX_Control(pDX, IDC_NORECURS, m_noRecursCheck);
	DDX_Control(pDX, IDC_TAGDESC_TEXT, m_tagDescStatic);
	DDX_Control(pDX, IDC_GRPBOX_TEXT, m_grpBox);
	DDX_Control(pDX, IDC_OVERWRITE, m_overwriteExistingCheck);
	DDX_Control(pDX, IDC_CHECK_UNMODIFIED, m_checkUnmodCheck);
	DDX_Control(pDX, IDC_TAGNAME, m_tagCombo);
	DDX_Check(pDX, IDC_NORECURS, m_noRecurs);
	DDX_Check(pDX, IDC_CHECK_UNMODIFIED, m_checkUnmod);
	DDX_Check(pDX, IDC_OVERWRITE, m_overwriteExisting);
	DDX_CBString(pDX, IDC_TAGNAME, m_tagName);
	DDX_Check(pDX, IDC_MOVDEL_BRANCH, m_movDelBranch);
	//}}AFX_DATA_MAP

	DDV_MinChars(pDX, m_tagName, 1);
	DDX_ComboMString(pDX, IDC_TAGNAME, m_tagCombo);
}


BEGIN_MESSAGE_MAP(CTag_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CTag_MAIN)
	ON_BN_CLICKED(IDC_BROWSE_TAG, OnBrowseTag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTag_MAIN message handlers

/// OnInitDialog virtual override, initialize controls
BOOL CTag_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Set the type-specific data
	CString strGrpBox;
	CString strTagDesc;
	
	switch( m_type )
	{
	case kTagCreate:
	case kTagCreateGraph:
		strGrpBox = _i18n("Enter the tag name to create:");
		strTagDesc = _i18n("New tag &name:");
		break;
	case kTagDelete:
	case kTagDeleteGraph:
		strGrpBox = _i18n("Enter the tag name to delete:");
		strTagDesc = _i18n("Delete tag &name:");

		m_checkUnmodCheck.ShowWindow(SW_HIDE);
		m_overwriteExistingCheck.ShowWindow(SW_HIDE);

		if( kTagDeleteGraph == m_type )
		{
			m_tagCombo.SetReadOnly(TRUE);
			m_tagCombo.ResetContent();
			
			if( !m_tagName.IsEmpty() )
			{
				m_tagCombo.AddString(m_tagName);
				m_tagCombo.SetWindowText(m_tagName);
			}

			m_browseTagButton.EnableWindow(FALSE);

			m_noRecursCheck.EnableWindow(FALSE);
		}
		break;
	case kTagBranch:
	case kTagBranchGraph:
		strGrpBox = _i18n("Enter the branch name to create:");
		strTagDesc = _i18n("New branch &name:");
		break;
	default:
		ASSERT(FALSE); //unknown type
	}
	
	m_grpBox.SetWindowText(strGrpBox);
	m_tagDescStatic.SetWindowText(strTagDesc);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTag_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_TAGNAME,			IDH_IDC_TAGNAME);
	SetCtrlHelpID(IDC_BROWSE_TAG,		IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_NORECURS,			IDH_IDC_NORECURS);
	SetCtrlHelpID(IDC_MOVDEL_BRANCH,	IDH_IDC_MOVDEL_BRANCH);
	SetCtrlHelpID(IDC_CHECK_UNMODIFIED,	IDH_IDC_CHECK_UNMODIFIED);
	SetCtrlHelpID(IDC_OVERWRITE,		IDH_IDC_OVERWRITE);
}

/// BN_CLICKED message handler, select tag
void CTag_MAIN::OnBrowseTag() 
{
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName) )
	{
		m_tagName = tagName.c_str();
		m_tagCombo.SetWindowText(m_tagName);
	}
}

#endif /* WIN32 */

#ifdef qMacCvsPP
static void DoDataExchange_TagCreateMain(LWindow *theDialog, const MultiFiles* mf,
	std::string & sLastTag, bool & noRecurs, bool & overwriteExist,
	bool & checkUnmod, bool putValue)
{
	LEditText *tname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_BranchRev));
	LCheckBox *chkNoRecurs = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_NoRecurs));
	LCheckBox *chkOverwrite = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_OverwriteRev));
	LCheckBox *chkUnmod = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckUnmod));
	LPopupFiller *fillTag = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerTag));
	
	LTagWrapper *wrapper = putValue ? NEW LTagWrapper(fillTag, mf) : 0L;
	fillTag->DoDataExchange(gRevNames, putValue, wrapper);

	if(putValue)
	{
		chkNoRecurs->SetValue(noRecurs ? Button_On : Button_Off);
		chkOverwrite->SetValue(overwriteExist ? Button_On : Button_Off);
		chkUnmod->SetValue(checkUnmod ? Button_On : Button_Off);
		tname->SetText((Ptr)(const char *)sLastTag, sLastTag.length());
	}
	else
	{
		char str[255];
		Size len;
		
		tname->GetText(str, 254, &len);
		sLastTag.set(str, len);
		noRecurs = chkNoRecurs->GetValue() == Button_On;
		overwriteExist = chkOverwrite->GetValue() == Button_On;
		checkUnmod = chkUnmod->GetValue() == Button_On;
	}
}

static void DoDataExchange_TagDeleteMain(LWindow *theDialog, const MultiFiles* mf,
	std::string & sLastTag, bool & noRecurs, bool putValue)
{
	LEditText *tname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_BranchRev));
	LCheckBox *chkNoRecurs = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_NoRecurs));
	LPopupFiller *fillTag = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerTag));
	
	LTagWrapper *wrapper = putValue ? NEW LTagWrapper(fillTag, mf) : 0L;
	fillTag->DoDataExchange(gRevNames, putValue, wrapper);

	if(putValue)
	{
		tname->SetText((Ptr)(const char *)sLastTag, sLastTag.length());
		chkNoRecurs->SetValue(noRecurs ? Button_On : Button_Off);
	}
	else
	{
		noRecurs = chkNoRecurs->GetValue() == Button_On;

		char str[255];
		Size len;
		
		tname->GetText(str, 254, &len);
		sLastTag.set(str, len);
	}
}

static void DoDataExchange_TagBranchMain(LWindow *theDialog, const MultiFiles* mf,
	std::string & sLastBranch, bool & noRecurs, bool & checkUnmod, bool putValue)
{
	LEditText *tname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_BranchRev));
	LCheckBox *chkNoRecurs = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_NoRecurs));
	LCheckBox *chkUnmod = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckUnmod));
	LPopupFiller *fillTag = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerTag));
	
	LTagWrapper *wrapper = putValue ? NEW LTagWrapper(fillTag, mf) : 0L;
	fillTag->DoDataExchange(gRevNames, putValue, wrapper);

	if(putValue)
	{
		chkNoRecurs->SetValue(noRecurs ? Button_On : Button_Off);
		chkUnmod->SetValue(checkUnmod ? Button_On : Button_Off);
		tname->SetText((Ptr)(const char *)sLastBranch, sLastBranch.length());
	}
	else
	{
		char str[255];
		Size len;
		
		tname->GetText(str, 254, &len);
		sLastBranch.set(str, len);
		noRecurs = chkNoRecurs->GetValue() == Button_On;
		checkUnmod = chkUnmod->GetValue() == Button_On;
	}
}

static void DoDataExchange_TagBranchOptions(LWindow *theDialog,
	bool & noRecurs, bool putValue)
{
	LCheckBox *chkNoRecurs = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_NoRecurs));
	
	if(putValue)
	{
		chkNoRecurs->SetValue(noRecurs ? Button_On : Button_Off);
	}
	else
	{
		noRecurs = chkNoRecurs->GetValue() == Button_On;
	}
}
#endif /* qMacCvsPP */

#if qUnix
class UCvsTagCreate : public UWidget
{
	UDECLARE_DYNAMIC(UCvsTagCreate)
public:
	UCvsTagCreate() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsTagCreate() {}

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

UIMPLEMENT_DYNAMIC(UCvsTagCreate, UWidget)

UBEGIN_MESSAGE_MAP(UCvsTagCreate, UWidget)
	ON_UCOMMAND(UCvsTagCreate::kOK, UCvsTagCreate::OnOK)
	ON_UCOMMAND(UCvsTagCreate::kCancel, UCvsTagCreate::OnCancel)
UEND_MESSAGE_MAP()

int UCvsTagCreate::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsTagCreate::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsTagCreate::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsTagCreate_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsTagCreate_MAIN)
public:
	UCvsTagCreate_MAIN(const char * TagsLastTag, bool noRecurs,
					   bool overwriteExist, bool checkUnmod);
	virtual ~UCvsTagCreate_MAIN() {}

	enum
	{
		kEditTag = EV_COMMAND_START,	// 0
		kCheckRecurs,		// 1
		kCheckUnmod,		// 2
		kCheckOverwrite,	// 3
	};

	virtual void DoDataExchange(bool fill);

	std::string m_tagname;
	bool m_norecurs;
	bool m_checkunmod;
	bool m_overwrite;
protected:

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsTagCreate_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsTagCreate_MAIN, UWidget)
UEND_MESSAGE_MAP()

UCvsTagCreate_MAIN::UCvsTagCreate_MAIN(const char * sLastTag, bool noRecurs,
					   bool overwriteExist, bool checkUnmod) : UWidget(::UEventGetWidID())
{
	m_tagname = sLastTag;
	m_norecurs = noRecurs;
	m_checkunmod = checkUnmod;
	m_overwrite = overwriteExist;
}

void UCvsTagCreate_MAIN::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kEditTag, gRevNames);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditTag, (void *)(const char *)m_tagname.c_str());

		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckUnmod, m_checkunmod), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurs, m_norecurs), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckOverwrite, m_overwrite), 0L);
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditTag, &m_tagname);

		m_checkunmod = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckUnmod, 0L);
		m_norecurs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurs, 0L);
		m_overwrite = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckOverwrite, 0L);
	}
}

class UCvsTagDel : public UWidget
{
	UDECLARE_DYNAMIC(UCvsTagDel)
public:
	UCvsTagDel() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsTagDel() {}

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

UIMPLEMENT_DYNAMIC(UCvsTagDel, UWidget)

UBEGIN_MESSAGE_MAP(UCvsTagDel, UWidget)
	ON_UCOMMAND(UCvsTagDel::kOK, UCvsTagDel::OnOK)
	ON_UCOMMAND(UCvsTagDel::kCancel, UCvsTagDel::OnCancel)
UEND_MESSAGE_MAP()

int UCvsTagDel::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsTagDel::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsTagDel::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsTagDel_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsTagDel_MAIN)
public:
	UCvsTagDel_MAIN(const char * sLastTag, bool noRecurs);
	virtual ~UCvsTagDel_MAIN() {}

	enum
	{
		kEditTag = EV_COMMAND_START,	// 0
		kCheckRecurs		// 1
	};

	virtual void DoDataExchange(bool fill);

	bool m_norecurs;
	std::string m_tagname;
protected:

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsTagDel_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsTagDel_MAIN, UWidget)
UEND_MESSAGE_MAP()

UCvsTagDel_MAIN::UCvsTagDel_MAIN(const char * sLastTag, bool noRecurs) : UWidget(::UEventGetWidID())
{
	m_norecurs = noRecurs;
	m_tagname = sLastTag;
}

void UCvsTagDel_MAIN::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kEditTag, gRevNames);
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditTag, (void *)(const char *)m_tagname.c_str());

		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurs, m_norecurs), 0L);
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditTag, &m_tagname);

		m_norecurs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurs, 0L);
	}
}

class UCvsTagBranch : public UWidget
{
	UDECLARE_DYNAMIC(UCvsTagBranch)
public:
	UCvsTagBranch() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsTagBranch() {}

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

UIMPLEMENT_DYNAMIC(UCvsTagBranch, UWidget)

UBEGIN_MESSAGE_MAP(UCvsTagBranch, UWidget)
	ON_UCOMMAND(UCvsTagBranch::kOK, UCvsTagBranch::OnOK)
	ON_UCOMMAND(UCvsTagBranch::kCancel, UCvsTagBranch::OnCancel)
UEND_MESSAGE_MAP()

int UCvsTagBranch::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsTagBranch::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsTagBranch::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsTagBranch_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsTagBranch_MAIN)
public:
	UCvsTagBranch_MAIN(const char * sLastTag, bool noRecurs,
					   bool checkUnmod);
	virtual ~UCvsTagBranch_MAIN() {}

	enum
	{
		kEditTag = EV_COMMAND_START,	// 0
		kCheckRecurs,		// 1
		kCheckUnmod			// 2
	};

	virtual void DoDataExchange(bool fill);

	std::string m_tagname;
	bool m_norecurs;
	bool m_checkunmod;
protected:

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsTagBranch_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsTagBranch_MAIN, UWidget)
UEND_MESSAGE_MAP()

UCvsTagBranch_MAIN::UCvsTagBranch_MAIN(const char * sLastTag, bool noRecurs,
									   bool checkUnmod) : UWidget(::UEventGetWidID())
{
	m_tagname = sLastTag;
	m_norecurs = noRecurs;
	m_checkunmod = checkUnmod;
}

void UCvsTagBranch_MAIN::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kEditTag, gRevNames);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditTag, (void *)(const char *)m_tagname.c_str());

		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckUnmod, m_checkunmod), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurs, m_norecurs), 0L);
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditTag, &m_tagname);

		m_checkunmod = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckUnmod, 0L);
		m_norecurs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurs, 0L);
	}
}
#endif // qUnix

/// Get the tag options
bool CompatGetTag(const MultiFiles* mf, const kTagCmdType tagType, 
				  bool& noRecurs, bool& movDelBranch,
				  bool& overwriteExisting,
				  std::string& tagName, bool& checkUnmod,
				  std::string& date, std::string& rev, bool& useMostRecent)
{
	bool userHitOK = false;

	static string sLastDate;
	static string sLastRev;
	static string sLastTag;
	
	bool hasDate = false;
	bool hasRev = false;
	string initialRev;
	string initialDate;
	
	// Graph view annotate supplies initial revisions, so don't override them
	if( kTagCreateGraph == tagType || 
		kTagDeleteGraph == tagType || 
		kTagBranchGraph == tagType )
	{
		initialRev = rev;
		hasRev = !rev.empty();
		initialDate.erase();
	}
	else
	{
		rev.erase();
		initialRev = sLastRev;
		initialDate = sLastDate;
	}

	tagName.erase();
	noRecurs = false;
	movDelBranch = false;
	useMostRecent = false;
	overwriteExisting = false;
	checkUnmod = false;

#ifdef WIN32
	const char* title = "Tag settings";
	
	switch( tagType )
	{
	case kTagCreate:
		title = "Create tag settings";
		break;
	case kTagDelete:
		title = "Delete tag settings";
		break;
	case kTagBranch:
		title = "Create branch settings";
		break;
	default:
		break;
	}
	
	CHHelpPropertySheet pages(title);
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	CTag_MAIN page1(mf, tagType, sLastTag.c_str(), noRecurs, checkUnmod, overwriteExisting);
	CRtag_OPTIONS page2(NULL, mf, tagType, hasDate, hasRev, initialDate.c_str(), initialRev.c_str(), useMostRecent);
	CGetPrefs_GLOBALS page3;

	pages.AddPage(&page1);

	if( tagType != kTagDelete && tagType != kTagDeleteGraph )
	{
		pages.AddPage(&page2);
	}
	else
	{
		page1.m_tagName = initialRev.c_str();
	}
	
	pages.AddPage(&page3);
	
	if( pages.DoModal() == IDOK )
	{
		checkUnmod = page1.m_checkUnmod == TRUE;
		noRecurs = page1.m_noRecurs == TRUE;
		movDelBranch = page1.m_movDelBranch == TRUE;
		sLastTag = page1.m_tagName;
		overwriteExisting = page1.m_overwriteExisting == TRUE;

		if( tagType != kTagDelete && tagType != kTagDeleteGraph )
		{
			sLastDate = page2.m_date;
			sLastRev = page2.m_rev;
			hasDate = page2.m_hasDate == TRUE;
			hasRev = page2.m_hasRev == TRUE;
			useMostRecent = page2.m_useMostRecent == TRUE;
		}
		
		page3.StoreValues();
		userHitOK = true;
	}
#endif /* WIN32 */
#if qUnix
	switch( tagType )
	{
	case kTagCreate:
		{
			void *wid = UCreate_TagDlg();
			
			UCvsTagCreate *dlg = new UCvsTagCreate();
			UCvsTagCreate_MAIN *tab1 =
				new UCvsTagCreate_MAIN(sLastTag.c_str(), noRecurs, overwriteExisting, checkUnmod);
			UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
			dlg->AddPage(tab1, UCvsTagCreate::kTabGeneral, 0);
			
			if(dlg->DoModal())
			{
				checkUnmod = tab1->m_checkunmod;
				noRecurs = tab1->m_norecurs;
				sLastTag = tab1->m_tagname;
				overwriteExisting = tab1->m_overwrite;
				
				userHitOK = true;
			}
			
			delete dlg;
		}
		break;
	case kTagDelete:
		{
			void *wid = UCreate_UntagDlg();
			
			UCvsTagDel *dlg = new UCvsTagDel();
			UCvsTagDel_MAIN *tab1 = new UCvsTagDel_MAIN(sLastTag.c_str(), noRecurs);
			UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
			dlg->AddPage(tab1, UCvsTagDel::kTabGeneral, 0);
			
			if(dlg->DoModal())
			{
				noRecurs = tab1->m_norecurs;
				sLastTag = tab1->m_tagname;
				
				userHitOK = true;
			}
			
			delete dlg;
		}
		break;
	case kTagBranch:
		{
			void *wid = UCreate_BranchDlg();
			
			UCvsTagBranch *dlg = new UCvsTagBranch();
			UCvsTagBranch_MAIN *tab1 = new UCvsTagBranch_MAIN(sLastTag.c_str(), noRecurs, checkUnmod);
			UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
			dlg->AddPage(tab1, UCvsTagBranch::kTabGeneral, 0);
			
			if(dlg->DoModal())
			{
				checkUnmod = tab1->m_checkunmod;
				noRecurs = tab1->m_norecurs;
				sLastTag = tab1->m_tagname;
				
				userHitOK = true;
			}
			
			delete dlg;
		}
		break;
	default:
		break;
	}
#endif // qUnix
#ifdef qMacCvsPP
	switch( tagType )
	{
	case kTagCreate:
		{
			StDialogHandler	theHandler(dlg_TagCreate, LCommander::GetTopCommander());
			LWindow *theDialog = theHandler.GetDialog();
			ThrowIfNil_(theDialog);
			static UInt16 sRuntimePanel = 1;
			
			LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
				(theDialog->FindPaneByID(item_MultiViewPrefs));
			LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
			
			multiView->SwitchToPanel(2);
			DoDataExchange_Globals(theDialog, true);
			multiView->SwitchToPanel(1);
			DoDataExchange_TagCreateMain(theDialog, mf, sLastTag,
				noRecurs, overwriteExisting, checkUnmod, true);
			
			groupView->SetValue(sRuntimePanel);
			theDialog->Show();
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
				DoDataExchange_TagCreateMain(theDialog, mf, sLastTag,
					noRecurs, overwriteExisting, checkUnmod, false);
				multiView->SwitchToPanel(2);
				DoDataExchange_Globals(theDialog, false);
				userHitOK = true;
			}
		}
		break;
	case kTagDelete:
		{
			StDialogHandler	theHandler(dlg_TagDelete, LCommander::GetTopCommander());
			LWindow *theDialog = theHandler.GetDialog();
			ThrowIfNil_(theDialog);
			static UInt16 sRuntimePanel = 1;
			
			LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
				(theDialog->FindPaneByID(item_MultiViewPrefs));
			LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
			
			multiView->SwitchToPanel(2);
			DoDataExchange_Globals(theDialog, true);
			multiView->SwitchToPanel(1);
			DoDataExchange_TagDeleteMain(theDialog, mf, sLastTag, noRecurs, true);
			
			groupView->SetValue(sRuntimePanel);
			theDialog->Show();
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
				DoDataExchange_TagDeleteMain(theDialog, mf, sLastTag, noRecurs, false);
				multiView->SwitchToPanel(2);
				DoDataExchange_Globals(theDialog, false);
				userHitOK = true;
			}
		}
		break;
	case kTagBranch:
		{
			StDialogHandler	theHandler(dlg_TagBranch, LCommander::GetTopCommander());
			LWindow *theDialog = theHandler.GetDialog();
			ThrowIfNil_(theDialog);
			static UInt16 sRuntimePanel = 1;
			
			LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
				(theDialog->FindPaneByID(item_MultiViewPrefs));
			LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
			
			multiView->SwitchToPanel(2);
			DoDataExchange_Globals(theDialog, true);
			multiView->SwitchToPanel(1);
			DoDataExchange_TagBranchMain(theDialog, mf, sLastTag,
				noRecurs, checkUnmod, true);
			
			groupView->SetValue(sRuntimePanel);
			theDialog->Show();
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
				DoDataExchange_TagBranchMain(theDialog, mf, sLastTag,
					noRecurs, checkUnmod, false);
				multiView->SwitchToPanel(2);
				DoDataExchange_Globals(theDialog, false);
				userHitOK = true;
			}
		}
		break;
	default:
		break;
	}
#endif /* qMacCvsPP */

	if( userHitOK )
	{
		if( tagType != kTagDelete && tagType != kTagDeleteGraph )
		{
			if( hasDate && !sLastDate.empty() )
				date = sLastDate;
			
			if( hasRev && !sLastRev.empty() )
				rev = sLastRev;
		}
		
		tagName = sLastTag;

		gCvsPrefs.save();
	}

	return userHitOK;
}
