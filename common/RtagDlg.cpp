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
 * RtagDlg.cpp : the cvs rtag dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "GetPrefs.h"
#endif /* WIN32 */

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LMultiPanelView.h>
#	include <LCheckBox.h>
#	include <LEditText.h>
#	include <LPopupGroupBox.h>
#	include <LRadioButton.h>
#	include "LPopupFiller.h"

#	include "MacCvsApp.h"
#	include "MacCvsConstant.h"
#endif /* qMacCvsPP */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#include "RtagDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"
#include "UpdateDlg.h"
#include "ItemListDlg.h"
#include "CvsRootDlg.h"

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
// CRtag_MAIN property page

IMPLEMENT_DYNAMIC(CRtag_MAIN, CHHelpPropertyPage)

CRtag_MAIN::CRtag_MAIN(const MultiFiles* mf, const kTagCmdType type, 
					   const char* sLastModule, const char* sLastTag,
					   bool noRecurs, bool movDelBranch, bool lookAttic, 
					   bool forceRoot, const char* cvsroot,
					   bool overwriteExist /*= false*/) 
					   : m_mf(mf), CHHelpPropertyPage(CRtag_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CRtag_MAIN)
	//}}AFX_DATA_INIT

	m_forceRoot = forceRoot;
	m_cvsroot = cvsroot;

	m_type = type;

	m_tagName = sLastTag;
	m_moduleName = sLastModule;

	m_noRecurs = noRecurs;
	m_movDelBranch = movDelBranch;
	
	m_lookAttic = lookAttic;
	m_overwriteExisting = overwriteExist;

	m_moduleCombo.SetItems(&gOldModules);
	m_tagCombo.SetItems(&gRevNames);
}

CRtag_MAIN::~CRtag_MAIN()
{
}

void CRtag_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRtag_MAIN)
	DDX_Control(pDX, IDC_CVSROOT_ENTRY_STATIC, m_cvsrootEntryPlaceholder);
	DDX_Control(pDX, IDC_OVERWRITE, m_overwriteExistingCheck);
	DDX_Control(pDX, IDC_MODULEDESC_TEXT, m_moduleDescStatic);
	DDX_Control(pDX, IDC_TAGDESC_TEXT, m_tagDescStatic);
	DDX_Control(pDX, IDC_GRPBOX_TEXT, m_grpBox);
	DDX_Control(pDX, IDC_MODNAME, m_moduleCombo);
	DDX_Control(pDX, IDC_TAGNAME, m_tagCombo);
	DDX_CBString(pDX, IDC_TAGNAME, m_tagName);
	DDX_CBString(pDX, IDC_MODNAME, m_moduleName);
	DDX_Check(pDX, IDC_NORECURS, m_noRecurs);
	DDX_Check(pDX, IDC_ATTIC, m_lookAttic);
	DDX_Check(pDX, IDC_OVERWRITE, m_overwriteExisting);
	DDX_Check(pDX, IDC_MOVDEL_BRANCH, m_movDelBranch);
	//}}AFX_DATA_MAP

	DDV_MinChars(pDX, m_moduleName, 1);
	DDV_MinChars(pDX, m_tagName, 1);

	DDX_ComboMString(pDX, IDC_MODNAME, m_moduleCombo);
	DDX_ComboMString(pDX, IDC_TAGNAME, m_tagCombo);

	m_cvsrootEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_forceRoot, m_cvsroot);
}


BEGIN_MESSAGE_MAP(CRtag_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CRtag_MAIN)
	ON_BN_CLICKED(IDC_BROWSE_TAG, OnBrowseTag)
	ON_BN_CLICKED(IDC_BROWSE_MODNAME, OnBrowseModules)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRtag_MAIN message handlers

/// OnInitDialog virtual override, enable controls and setup entry descriptions
BOOL CRtag_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Set the type-specific data
	CString strGrpBox;
	CString strTagDesc;
	CString strmoduleDesc;
	
	switch( m_type )
	{
	case kTagCreate:
		strGrpBox = "Enter the tag name and which module to tag:";
		strTagDesc = "New &tag name:";
		strmoduleDesc = "&Module to tag:";
		break;
	case kTagDelete:
		strGrpBox = "Enter the tag name to delete and which module to untag:";
		strTagDesc = "Delete &tag name:";
		strmoduleDesc = "&Module to untag:";

		m_overwriteExistingCheck.ShowWindow(SW_HIDE);
		break;
	case kTagBranch:
		strGrpBox = "Enter the branch name and which module to fork:";
		strTagDesc = "New &branch name:";
		strmoduleDesc = "&Module to fork:";
		break;
	default:
		ASSERT(FALSE); //unknown type
	}
	
	m_grpBox.SetWindowText(strGrpBox);
	m_tagDescStatic.SetWindowText(strTagDesc);
	m_moduleDescStatic.SetWindowText(strmoduleDesc);;

	m_cvsrootEntryDlg.Create(&m_cvsrootEntryPlaceholder, this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRtag_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_TAGNAME,			IDH_IDC_TAGNAME);
	SetCtrlHelpID(IDC_BROWSE_TAG,		IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_MODNAME,			IDH_IDC_MODNAME);
	SetCtrlHelpID(IDC_BROWSE_MODNAME,	IDH_IDC_BROWSE_MODNAME);
	SetCtrlHelpID(IDC_NORECURS,			IDH_IDC_NORECURS);
	SetCtrlHelpID(IDC_MOVDEL_BRANCH,	IDH_IDC_MOVDEL_BRANCH);
	SetCtrlHelpID(IDC_ATTIC,			IDH_IDC_ATTIC);
	SetCtrlHelpID(IDC_OVERWRITE,		IDH_IDC_OVERWRITE);
}


/// BN_CLICKED message handler, select tag
void CRtag_MAIN::OnBrowseTag() 
{
	ItemSelectionData itemSelectionData(m_cvsrootEntryDlg);
	
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName, &itemSelectionData) )
	{
		m_tagName = tagName.c_str();
		m_tagCombo.SetWindowText(m_tagName);
	}
}

/// BN_CLICKED message handler, select module
void CRtag_MAIN::OnBrowseModules() 
{
	ItemSelectionData itemSelectionData(m_cvsrootEntryDlg);
	
	string moduleName;
	if( CompatGetModuleListItem(m_mf, moduleName, &itemSelectionData) )
	{
		m_moduleName = moduleName.c_str();
		m_moduleCombo.SetWindowText(m_moduleName);
	}
}

/// Get the CVSROOT entry dialog
const CCvsrootEntryDlg& CRtag_MAIN::GetCvsrootEntryDlg() const
{
	return m_cvsrootEntryDlg;
}

/////////////////////////////////////////////////////////////////////////////
// CRtag_OPTIONS property page

IMPLEMENT_DYNAMIC(CRtag_OPTIONS, CHHelpPropertyPage)

CRtag_OPTIONS::CRtag_OPTIONS(const CCvsrootEntryDlg* cvsrootEntryDlg, 
							 const MultiFiles* mf, const kTagCmdType type, 
							 bool hasDate, bool hasRev,
							 const char *sLastDate, const char *sLastRev,
							 bool useMostRecent) 
							 : m_cvsrootEntryDlg(cvsrootEntryDlg), 
							 m_mf(mf), 
							 CHHelpPropertyPage(CRtag_OPTIONS::IDD)
{
	m_type = type;

	m_date = sLastDate;
	m_rev = sLastRev;
	m_hasDate = hasDate;
	m_hasRev = hasRev;
	m_useMostRecent = useMostRecent;

	m_revCombo.SetItems(&gRevNames);
	m_dateCombo.SetItems(&gDateNames);
}

CRtag_OPTIONS::~CRtag_OPTIONS()
{
}

void CRtag_OPTIONS::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRtag_OPTIONS)
	DDX_Control(pDX, IDC_FORCE_HEAD_REV, m_useMostRecentCheck);
	DDX_Control(pDX, IDC_BROWSE_TAG, m_browseTagButton);
	DDX_Control(pDX, IDC_HAS_REV, m_hasRevCheck);
	DDX_Control(pDX, IDC_HAS_DATE, m_hasDateCheck);
	DDX_Control(pDX, IDC_REV_TAG, m_revCombo);
	DDX_Control(pDX, IDC_DATE, m_dateCombo);
	DDX_Control(pDX, IDC_GRPBOX_TEXT, m_grpBox);
	DDX_CBString(pDX, IDC_DATE, m_date);
	DDX_CBString(pDX, IDC_REV_TAG, m_rev);
	DDX_Check(pDX, IDC_HAS_DATE, m_hasDate);
	DDX_Check(pDX, IDC_HAS_REV, m_hasRev);
	DDX_Check(pDX, IDC_FORCE_HEAD_REV, m_useMostRecent);
	//}}AFX_DATA_MAP

	if( m_hasRev )
	{
		DDV_MinChars(pDX, m_rev, 1);
	}
	
	if( m_hasDate )
	{
		DDV_MinChars(pDX, m_date, 1);
	}
	
	if( !pDX->m_bSaveAndValidate )
	{
		OnDate();
		OnRev();
	}

	DDX_ComboMString(pDX, IDC_REV_TAG, m_revCombo);
	DDX_ComboMString(pDX, IDC_DATE, m_dateCombo);
}


BEGIN_MESSAGE_MAP(CRtag_OPTIONS, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CRtag_OPTIONS)
	ON_BN_CLICKED(IDC_HAS_DATE, OnDate)
	ON_BN_CLICKED(IDC_HAS_REV, OnRev)
	ON_BN_CLICKED(IDC_BROWSE_TAG, OnBrowseTag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRtag_OPTIONS message handlers

/// BN_CLICKED message handler, enable controls
void CRtag_OPTIONS::OnDate() 
{
	m_dateCombo.EnableWindow(m_hasDateCheck.GetCheck());
}

/// BN_CLICKED message handler, enable controls
void CRtag_OPTIONS::OnRev() 
{
	const BOOL enable = m_hasRevCheck.GetCheck();

	m_revCombo.EnableWindow(enable);
	m_browseTagButton.EnableWindow(enable);
}

/// OnInitDialog virtual override, enable controls and setup entry descriptions
BOOL CRtag_OPTIONS::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Special setup for the tag invoked using the graph view
	if( kTagCreateGraph == m_type || 
		kTagDeleteGraph == m_type || 
		kTagBranchGraph == m_type )
	{
		m_hasDateCheck.EnableWindow(FALSE);
		m_dateCombo.EnableWindow(FALSE);
		
		m_hasRevCheck.EnableWindow(FALSE);
		m_hasRevCheck.SetCheck(1);
		
		m_browseTagButton.EnableWindow(FALSE);
		
		m_revCombo.SetReadOnly(TRUE);
		m_revCombo.ResetContent();
		
		if( !m_rev.IsEmpty() )
		{
			m_revCombo.AddString(m_rev);
			m_revCombo.SetWindowText(m_rev);
		}
		
		m_useMostRecentCheck.EnableWindow(FALSE);
	}
	
	// Set the type-specific data
	CString strGrpBox;
	
	switch( m_type )
	{
	case kTagCreate:
	case kTagCreateGraph:
		strGrpBox = "Optionally use this revision/tag/date to create the tag:";
		break;
	case kTagDelete:
	case kTagDeleteGraph:
		ASSERT(FALSE); //delete tag has no options
		break;
	case kTagBranch:
	case kTagBranchGraph:
		strGrpBox = "Optionally use this revision/tag/date to create the branch:";
		break;
	default:
		ASSERT(FALSE); //unknown type
	}
	
	m_grpBox.SetWindowText(strGrpBox);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRtag_OPTIONS::InitHelpID()
{
	SetCtrlHelpID(IDC_HAS_DATE,			IDH_IDC_HAS_DATE);
	SetCtrlHelpID(IDC_DATE,				IDH_IDC_DATE);
	SetCtrlHelpID(IDC_HAS_REV,			IDH_IDC_HAS_REV);
	SetCtrlHelpID(IDC_REV_TAG,			IDH_IDC_REV_TAG);
	SetCtrlHelpID(IDC_BROWSE_TAG,		IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_FORCE_HEAD_REV,	IDH_IDC_FORCE_HEAD_REV);
}

/// BN_CLICKED message handler, select tag
void CRtag_OPTIONS::OnBrowseTag() 
{
	auto_ptr<ItemSelectionData> itemSelectionData;
		
	if( m_cvsrootEntryDlg )
	{
		itemSelectionData = auto_ptr<ItemSelectionData>(new ItemSelectionData(*m_cvsrootEntryDlg));
	}
	
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName, itemSelectionData.get()) )
	{
		m_rev = tagName.c_str();
		m_revCombo.SetWindowText(m_rev);
	}
}

#endif /* WIN32 */

#ifdef qMacCvsPP
static void DoDataExchange_RtagCreateMain(LWindow *theDialog, const MultiFiles* mf,
	std::string & sLastModule, std::string & sLastTag,
	bool & hasDate, bool & hasRev,
	std::string & sLastDate, std::string & sLastRev,
	bool & useMostRecent, bool & lookAttic, bool putValue)
{
	LEditText *mname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_ModuleName));
	LEditText *tname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_BranchRev));
	LCheckBox *chkHasDate = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckDate));
	LCheckBox *chkHasRev = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckRevTag));
	LCheckBox *chkMostRecent = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_MostRecentRev));
	LCheckBox *chkLookAttic = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_InAttic));
	LEditText *editDate = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditDate));
	LEditText *editRev = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditRevTag));
	LPopupFiller *fillRev = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerRev1));
	LPopupFiller *fillTag = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerTag));
	LPopupFiller *fillDate = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerDate));
	
	LTagWrapper *wrapper1 = putValue ? NEW LTagWrapper(fillRev, mf) : 0L;
	fillRev->DoDataExchange(gRevNames, putValue, wrapper1);
	LTagWrapper *wrapper2 = putValue ? NEW LTagWrapper(fillTag, mf) : 0L;
	fillTag->DoDataExchange(gRevNames, putValue, wrapper2);
	fillDate->DoDataExchange(gDateNames, putValue);

	if(putValue)
	{
		mname->SetText((Ptr)(const char *)sLastModule, sLastModule.length());
		tname->SetText((Ptr)(const char *)sLastTag, sLastTag.length());
		chkHasDate->SetValue(hasDate ? Button_On : Button_Off);
		chkHasRev->SetValue(hasRev ? Button_On : Button_Off);
		chkMostRecent->SetValue(useMostRecent ? Button_On : Button_Off);
		chkLookAttic->SetValue(lookAttic ? Button_On : Button_Off);
		editDate->SetText((Ptr)(const char *)sLastDate, sLastDate.length());
		editRev->SetText((Ptr)(const char *)sLastRev, sLastRev.length());
	}
	else
	{
		hasDate = chkHasDate->GetValue() == Button_On;
		hasRev = chkHasRev->GetValue() == Button_On;
		useMostRecent = chkMostRecent->GetValue() == Button_On;
		lookAttic = chkLookAttic->GetValue() == Button_On;

		char str[255];
		Size len;
		
		mname->GetText(str, 254, &len);
		sLastModule.set(str, len);
		tname->GetText(str, 254, &len);
		sLastTag.set(str, len);
		editDate->GetText(str, 254, &len);
		sLastDate.set(str, len);
		editRev->GetText(str, 254, &len);
		sLastRev.set(str, len);
	}
}

static void DoDataExchange_RtagCreateOptions(LWindow *theDialog,
	bool & noRecurs, bool & overwriteExist, bool putValue)
{
	LCheckBox *chkNoRecurs = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_NoRecurs));
	LCheckBox *chkOverwrite = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_OverwriteRev));
	
	if(putValue)
	{
		chkNoRecurs->SetValue(noRecurs ? Button_On : Button_Off);
		chkOverwrite->SetValue(overwriteExist ? Button_On : Button_Off);
	}
	else
	{
		noRecurs = chkNoRecurs->GetValue() == Button_On;
		overwriteExist = chkOverwrite->GetValue() == Button_On;
	}
}

static void DoDataExchange_RtagDeleteMain(LWindow *theDialog, const MultiFiles* mf,
	std::string & sLastModule, std::string & sLastTag,
	bool & noRecurs, bool & lookAttic, bool putValue)
{
	LEditText *mname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_ModuleName));
	LEditText *tname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_BranchRev));
	LCheckBox *chkLookAttic = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_InAttic));
	LCheckBox *chkNoRecurs = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_NoRecurs));
	LPopupFiller *fillTag = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerTag));
	
	LTagWrapper *wrapper = putValue ? NEW LTagWrapper(fillTag, mf) : 0L;
	fillTag->DoDataExchange(gRevNames, putValue, wrapper);

	if(putValue)
	{
		mname->SetText((Ptr)(const char *)sLastModule, sLastModule.length());
		tname->SetText((Ptr)(const char *)sLastTag, sLastTag.length());
		chkLookAttic->SetValue(lookAttic ? Button_On : Button_Off);
		chkNoRecurs->SetValue(noRecurs ? Button_On : Button_Off);
	}
	else
	{
		lookAttic = chkLookAttic->GetValue() == Button_On;
		noRecurs = chkNoRecurs->GetValue() == Button_On;

		char str[255];
		Size len;
		
		mname->GetText(str, 254, &len);
		sLastModule.set(str, len);
		tname->GetText(str, 254, &len);
		sLastTag.set(str, len);
	}
}

static void DoDataExchange_RtagBranchMain(LWindow *theDialog, const MultiFiles* mf,
	std::string & sLastModule, std::string & sLastBranch,
	bool & hasDate, bool & hasRev,
	std::string & sLastDate, std::string & sLastRev,
	bool & useMostRecent, bool & lookAttic, bool putValue)
{
	LEditText *mname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_ModuleName));
	LEditText *tname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_BranchRev));
	LCheckBox *chkHasDate = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckDate));
	LCheckBox *chkHasRev = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckRevTag));
	LCheckBox *chkMostRecent = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_MostRecentRev));
	LCheckBox *chkLookAttic = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_InAttic));
	LEditText *editDate = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditDate));
	LEditText *editRev = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditRevTag));
	LPopupFiller *fillRev = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerRev1));
	LPopupFiller *fillTag = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerTag));
	LPopupFiller *fillDate = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerDate));
	
	LTagWrapper *wrapper1 = putValue ? NEW LTagWrapper(fillRev, mf) : 0L;
	fillRev->DoDataExchange(gRevNames, putValue, wrapper1);
	LTagWrapper *wrapper2 = putValue ? NEW LTagWrapper(fillTag, mf) : 0L;
	fillTag->DoDataExchange(gRevNames, putValue, wrapper2);
	fillDate->DoDataExchange(gDateNames, putValue);

	if(putValue)
	{
		mname->SetText((Ptr)(const char *)sLastModule, sLastModule.length());
		tname->SetText((Ptr)(const char *)sLastBranch, sLastBranch.length());
		chkHasDate->SetValue(hasDate ? Button_On : Button_Off);
		chkHasRev->SetValue(hasRev ? Button_On : Button_Off);
		chkMostRecent->SetValue(useMostRecent ? Button_On : Button_Off);
		chkLookAttic->SetValue(lookAttic ? Button_On : Button_Off);
		editDate->SetText((Ptr)(const char *)sLastDate, sLastDate.length());
		editRev->SetText((Ptr)(const char *)sLastRev, sLastRev.length());
	}
	else
	{
		hasDate = chkHasDate->GetValue() == Button_On;
		hasRev = chkHasRev->GetValue() == Button_On;
		useMostRecent = chkMostRecent->GetValue() == Button_On;
		lookAttic = chkLookAttic->GetValue() == Button_On;

		char str[255];
		Size len;
		
		mname->GetText(str, 254, &len);
		sLastModule.set(str, len);
		tname->GetText(str, 254, &len);
		sLastBranch.set(str, len);
		editDate->GetText(str, 254, &len);
		sLastDate.set(str, len);
		editRev->GetText(str, 254, &len);
		sLastRev.set(str, len);
	}
}

static void DoDataExchange_RtagBranchOptions(LWindow *theDialog,
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
class UCvsRtagCreate : public UWidget
{
	UDECLARE_DYNAMIC(UCvsRtagCreate)
public:
	UCvsRtagCreate() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsRtagCreate() {}

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

UIMPLEMENT_DYNAMIC(UCvsRtagCreate, UWidget)

UBEGIN_MESSAGE_MAP(UCvsRtagCreate, UWidget)
	ON_UCOMMAND(UCvsRtagCreate::kOK, UCvsRtagCreate::OnOK)
	ON_UCOMMAND(UCvsRtagCreate::kCancel, UCvsRtagCreate::OnCancel)
UEND_MESSAGE_MAP()

int UCvsRtagCreate::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsRtagCreate::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsRtagCreate::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsRtagCreate_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsRtagCreate_MAIN)
public:
	UCvsRtagCreate_MAIN(const char * sLastModule, const char * sLastTag,
						bool hasDate, bool hasRev,
						const char * sLastDate, const char * sLastRev,
						bool noRecurs, bool overwriteExist,
						bool useMostRecent, bool lookAttic);
	virtual ~UCvsRtagCreate_MAIN() {}

	enum
	{
		kEditTag = EV_COMMAND_START,	// 0
		kEditMod,			// 1
		kCheckRev,			// 2
		kCheckDate,			// 3
		kComboRev,			// 4
		kComboDate,			// 5
		kCheckRecurs,		// 6
		kCheckAttic,		// 7
		kCheckOverwrite,	// 8
		kCheckMatch			// 9
	};

	virtual void DoDataExchange(bool fill);

	bool m_attic;
	std::string m_date;
	std::string m_rev;
	bool m_hasdate;
	bool m_hasrev;
	bool m_norecurs;
	bool m_match;
	std::string m_modname;
	std::string m_tagname;
	bool m_overwrite;
protected:
	ev_msg int OnChecks(int cmd);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsRtagCreate_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsRtagCreate_MAIN, UWidget)
	ON_UCOMMAND_RANGE(UCvsRtagCreate_MAIN::kCheckRev, UCvsRtagCreate_MAIN::kCheckDate, UCvsRtagCreate_MAIN::OnChecks)
UEND_MESSAGE_MAP()

UCvsRtagCreate_MAIN::UCvsRtagCreate_MAIN(const char * sLastModule, const char * sLastTag,
										 bool hasDate, bool hasRev,
										 const char * sLastDate, const char * sLastRev,
										 bool noRecurs, bool overwriteExist,
										 bool useMostRecent, bool lookAttic) : UWidget(::UEventGetWidID())
{
	m_attic = lookAttic;
	m_date = sLastDate;
	m_rev = sLastRev;
	m_hasdate = hasDate;
	m_hasrev = hasRev;
	m_norecurs = noRecurs;
	m_match = useMostRecent;
	m_modname = sLastModule;
	m_tagname = sLastTag;
	m_overwrite = overwriteExist;
}

int UCvsRtagCreate_MAIN::OnChecks(int cmd)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, cmd, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(cmd + 2, state), 0L);
	return 0;
}

void UCvsRtagCreate_MAIN::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kComboRev, gRevNames);
	::DoDataExchange(fill, GetWidID(), kComboDate, gDateNames);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditTag, (void *)(const char *)m_tagname.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditMod, (void *)(const char *)m_modname.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboRev, (void *)(const char *)m_rev.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboDate, (void *)(const char *)m_date.c_str());

		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckDate, m_hasdate), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRev, m_hasrev), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckAttic, m_attic), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurs, m_norecurs), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckOverwrite, m_overwrite), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckMatch, m_match), 0L);

		OnChecks(kCheckRev);
		OnChecks(kCheckDate);
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditTag, &m_tagname);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditMod, &m_modname);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboRev, &m_rev);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboDate, &m_date);

		m_hasdate = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDate, 0L);
		m_hasrev = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRev, 0L);
		m_attic = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckAttic, 0L);
		m_norecurs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurs, 0L);
		m_overwrite = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckOverwrite, 0L);
		m_match = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckMatch, 0L);
	}
}

class UCvsRtagDel : public UWidget
{
	UDECLARE_DYNAMIC(UCvsRtagDel)
public:
	UCvsRtagDel() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsRtagDel() {}

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

UIMPLEMENT_DYNAMIC(UCvsRtagDel, UWidget)

UBEGIN_MESSAGE_MAP(UCvsRtagDel, UWidget)
	ON_UCOMMAND(UCvsRtagDel::kOK, UCvsRtagDel::OnOK)
	ON_UCOMMAND(UCvsRtagDel::kCancel, UCvsRtagDel::OnCancel)
UEND_MESSAGE_MAP()

int UCvsRtagDel::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsRtagDel::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsRtagDel::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsRtagDel_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsRtagDel_MAIN)
public:
	UCvsRtagDel_MAIN(const char * sLastModule,
					 const char * sLastTag, bool noRecurs,
					 bool lookAttic);
	virtual ~UCvsRtagDel_MAIN() {}

	enum
	{
		kEditTag = EV_COMMAND_START,	// 0
		kEditMod,			// 1
		kCheckRecurs,		// 2
		kCheckAttic			// 3
	};

	virtual void DoDataExchange(bool fill);

	bool m_attic;
	bool m_norecurs;
	std::string m_modname;
	std::string m_tagname;
protected:

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsRtagDel_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsRtagDel_MAIN, UWidget)
UEND_MESSAGE_MAP()

UCvsRtagDel_MAIN::UCvsRtagDel_MAIN(const char * sLastModule,
								   const char * sLastTag, bool noRecurs,
								   bool lookAttic) : UWidget(::UEventGetWidID())
{
	m_attic = lookAttic;
	m_norecurs = noRecurs;
	m_modname = sLastModule;
	m_tagname = sLastTag;
}

void UCvsRtagDel_MAIN::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditTag, (void *)(const char *)m_tagname.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditMod, (void *)(const char *)m_modname.c_str());

		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckAttic, m_attic), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurs, m_norecurs), 0L);
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditTag, &m_tagname);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditMod, &m_modname);

		m_attic = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckAttic, 0L);
		m_norecurs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurs, 0L);
	}
}

class UCvsRtagBranch : public UWidget
{
	UDECLARE_DYNAMIC(UCvsRtagBranch)
public:
	UCvsRtagBranch() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsRtagBranch() {}

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

UIMPLEMENT_DYNAMIC(UCvsRtagBranch, UWidget)

UBEGIN_MESSAGE_MAP(UCvsRtagBranch, UWidget)
	ON_UCOMMAND(UCvsRtagBranch::kOK, UCvsRtagBranch::OnOK)
	ON_UCOMMAND(UCvsRtagBranch::kCancel, UCvsRtagBranch::OnCancel)
UEND_MESSAGE_MAP()

int UCvsRtagBranch::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsRtagBranch::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsRtagBranch::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsRtagBranch_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsRtagBranch_MAIN)
public:
	UCvsRtagBranch_MAIN(const char * sLastModule, const char * sLastTag,
						bool hasDate, bool hasRev,
						const char * sLastDate, const char * sLastRev,
						bool noRecurs, bool useMostRecent, bool lookAttic);
	virtual ~UCvsRtagBranch_MAIN() {}

	enum
	{
		kEditTag = EV_COMMAND_START,	// 0
		kEditMod,			// 1
		kCheckRev,			// 2
		kCheckDate,			// 3
		kComboRev,			// 4
		kComboDate,			// 5
		kCheckRecurs,		// 6
		kCheckAttic,		// 7
		kCheckMatch			// 9
	};

	virtual void DoDataExchange(bool fill);

	bool m_attic;
	std::string m_date;
	std::string m_rev;
	bool m_hasdate;
	bool m_hasrev;
	bool m_norecurs;
	bool m_match;
	std::string m_modname;
	std::string m_tagname;
protected:
	ev_msg int OnChecks(int cmd);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsRtagBranch_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsRtagBranch_MAIN, UWidget)
	ON_UCOMMAND_RANGE(UCvsRtagBranch_MAIN::kCheckRev, UCvsRtagBranch_MAIN::kCheckDate, UCvsRtagBranch_MAIN::OnChecks)
UEND_MESSAGE_MAP()

UCvsRtagBranch_MAIN::UCvsRtagBranch_MAIN(const char * sLastModule, const char * sLastTag,
											 bool hasDate, bool hasRev, const char * sLastDate,
											 const char * sLastRev, bool noRecurs, bool useMostRecent,
											 bool lookAttic) : UWidget(::UEventGetWidID())
{
	m_attic = lookAttic;
	m_date = sLastDate;
	m_rev = sLastRev;
	m_hasdate = hasDate;
	m_hasrev = hasRev;
	m_norecurs = noRecurs;
	m_match = useMostRecent;
	m_modname = sLastModule;
	m_tagname = sLastTag;
}

int UCvsRtagBranch_MAIN::OnChecks(int cmd)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, cmd, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(cmd + 2, state), 0L);
	return 0;
}

void UCvsRtagBranch_MAIN::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kComboRev, gRevNames);
	::DoDataExchange(fill, GetWidID(), kComboDate, gDateNames);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditTag, (void *)(const char *)m_tagname.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditMod, (void *)(const char *)m_modname.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboRev, (void *)(const char *)m_rev.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboDate, (void *)(const char *)m_date.c_str());

		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckDate, m_hasdate), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRev, m_hasrev), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckAttic, m_attic), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurs, m_norecurs), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckMatch, m_match), 0L);

		OnChecks(kCheckRev);
		OnChecks(kCheckDate);
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditTag, &m_tagname);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditMod, &m_modname);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboRev, &m_rev);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboDate, &m_date);

		m_hasdate = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDate, 0L);
		m_hasrev = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRev, 0L);
		m_attic = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckAttic, 0L);
		m_norecurs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurs, 0L);
		m_match = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckMatch, 0L);
	}
}

#endif // qUnix

/// Get the rtag options
bool CompatGetRtag(const MultiFiles* mf, const kTagCmdType tagType, 
				   bool& noRecurs, bool& movDelBranch,
				   bool& overwriteExisting, std::string& tagName,
				   std::string& modName, std::string& date, std::string& rev,
				   bool& useMostRecent, bool& lookAttic,
				   bool& forceCvsroot, std::string& cvsroot)
{
	bool userHitOK = false;

	static string sLastModule;
	static string sLastTag;
	static string sLastDate;
	static string sLastRev;
	static bool lastForceCvsroot = true;
	static string sLastCvsroot;

	bool hasDate = false;
	bool hasRev = false;

	date = "";
	rev = "";
	tagName = "";
	modName = "";
	noRecurs = false;
	movDelBranch = false;
	overwriteExisting = false;
	useMostRecent = false;
	lookAttic = false;

	if( mf && mf->NumDirs() > 0 )
	{
		string path;
		mf->getdir(0, path);

		FindBestLastCvsroot(path.c_str(), sLastCvsroot);
		FindBestLastModuleName(path.c_str(), sLastModule);
	}

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

	CRtag_MAIN page1(mf, tagType, sLastModule.c_str(), sLastTag.c_str(), noRecurs, movDelBranch, lookAttic, lastForceCvsroot, sLastCvsroot.c_str(), overwriteExisting);
	CRtag_OPTIONS page2(&page1.GetCvsrootEntryDlg(), mf, tagType, hasDate, hasRev, sLastDate.c_str(), sLastRev.c_str(), useMostRecent);
	CGetPrefs_GLOBALS page3;
	
	pages.AddPage(&page1);

	if( tagType != kTagDelete )
	{
		pages.AddPage(&page2);
	}

	pages.AddPage(&page3);
	
	if( pages.DoModal() == IDOK )
	{
		sLastModule = page1.m_moduleName;
		sLastTag = page1.m_tagName;
		noRecurs = page1.m_noRecurs == TRUE;
		movDelBranch = page1.m_movDelBranch == TRUE;
		overwriteExisting = page1.m_overwriteExisting == TRUE;
		lookAttic = page1.m_lookAttic == TRUE;
		lastForceCvsroot = page1.m_forceRoot == TRUE;
		sLastCvsroot = page1.m_cvsroot;

		if( tagType != kTagDelete )
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
//	}
	switch( tagType )
	{
	case kTagCreate:
		{
			void *wid = UCreate_TagModDlg();
			
			UCvsRtagCreate *dlg = new UCvsRtagCreate();
			UCvsRtagCreate_MAIN *tab1 =
				new UCvsRtagCreate_MAIN(sLastModule.c_str(), sLastTag.c_str(), hasDate, hasRev,
				sLastDate.c_str(), sLastRev.c_str(), noRecurs, overwriteExisting,
				useMostRecent, lookAttic);
			UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
			dlg->AddPage(tab1, UCvsRtagCreate::kTabGeneral, 0);
			
			if(dlg->DoModal())
			{
				lookAttic = tab1->m_attic;
				sLastDate = tab1->m_date;
				sLastRev = tab1->m_rev;
				hasDate = tab1->m_hasdate;
				hasRev = tab1->m_hasrev;
				noRecurs = tab1->m_norecurs;
				useMostRecent = tab1->m_match;
				sLastModule = tab1->m_modname;
				sLastTag = tab1->m_tagname;
				overwriteExisting = tab1->m_overwrite;
				
				userHitOK = true;
			}
			
			delete dlg;
		}
		break;
	case kTagDelete:
		{
			void *wid = UCreate_UntagModDlg();
			
			UCvsRtagDel *dlg = new UCvsRtagDel();
			UCvsRtagDel_MAIN *tab1 = new UCvsRtagDel_MAIN(sLastModule.c_str(), sLastTag.c_str(),
				noRecurs, lookAttic);
			UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
			dlg->AddPage(tab1, UCvsRtagDel::kTabGeneral, 0);
			
			if(dlg->DoModal())
			{
				lookAttic = tab1->m_attic;
				noRecurs = tab1->m_norecurs;
				sLastModule = tab1->m_modname;
				sLastTag = tab1->m_tagname;
				
				userHitOK = true;
			}
			
			delete dlg;
		}
		break;
	case kTagBranch:
		{
			void *wid = UCreate_BranchModDlg();
			
			UCvsRtagBranch *dlg = new UCvsRtagBranch();
			UCvsRtagBranch_MAIN *tab1 = 
				new UCvsRtagBranch_MAIN(sLastModule.c_str(), sLastTag.c_str(), hasDate, hasRev,
				sLastDate.c_str(), sLastRev.c_str(), noRecurs, useMostRecent,
				lookAttic);
			UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
			dlg->AddPage(tab1, UCvsRtagBranch::kTabGeneral, 0);
			
			if(dlg->DoModal())
			{
				lookAttic = tab1->m_attic;
				sLastDate = tab1->m_date;
				sLastRev = tab1->m_rev;
				hasDate = tab1->m_hasdate;
				hasRev = tab1->m_hasrev;
				noRecurs = tab1->m_norecurs;
				useMostRecent = tab1->m_match;
				sLastModule = tab1->m_modname;
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
			StDialogHandler	theHandler(dlg_RtagCreate, LCommander::GetTopCommander());
			LWindow *theDialog = theHandler.GetDialog();
			ThrowIfNil_(theDialog);
			static UInt16 sRuntimePanel = 1;
			
			LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
				(theDialog->FindPaneByID(item_MultiViewPrefs));
			LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
			
			multiView->SwitchToPanel(3);
			DoDataExchange_Globals(theDialog, true);
			multiView->SwitchToPanel(2);
			DoDataExchange_RtagCreateOptions(theDialog, noRecurs, overwriteExisting, true);
			multiView->SwitchToPanel(1);
			DoDataExchange_RtagCreateMain(theDialog, mf, sLastModule, sLastTag,
				hasDate, hasRev, sLastDate, sLastRev,
				useMostRecent, lookAttic, true);
			
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
				DoDataExchange_RtagCreateMain(theDialog, mf, sLastModule, sLastTag,
					hasDate, hasRev, sLastDate, sLastRev,
					useMostRecent, lookAttic, false);
				multiView->SwitchToPanel(2);
				DoDataExchange_RtagCreateOptions(theDialog, noRecurs, overwriteExisting, false);
				multiView->SwitchToPanel(3);
				DoDataExchange_Globals(theDialog, false);
				userHitOK = true;
			}
		}
		break;
	case kTagDelete:
		{
			StDialogHandler	theHandler(dlg_RtagDelete, LCommander::GetTopCommander());
			LWindow *theDialog = theHandler.GetDialog();
			ThrowIfNil_(theDialog);
			static UInt16 sRuntimePanel = 1;
			
			LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
				(theDialog->FindPaneByID(item_MultiViewPrefs));
			LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
			
			multiView->SwitchToPanel(2);
			DoDataExchange_Globals(theDialog, true);
			multiView->SwitchToPanel(1);
			DoDataExchange_RtagDeleteMain(theDialog, mf, sLastModule, sLastTag,
				noRecurs, lookAttic, true);
			
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
				DoDataExchange_RtagDeleteMain(theDialog, mf, sLastModule, sLastTag,
					noRecurs, lookAttic, false);
				multiView->SwitchToPanel(2);
				DoDataExchange_Globals(theDialog, false);
				userHitOK = true;
			}
		}
		break;
	case kTagBranch:
		{
			StDialogHandler	theHandler(dlg_RtagBranch, LCommander::GetTopCommander());
			LWindow *theDialog = theHandler.GetDialog();
			ThrowIfNil_(theDialog);
			static UInt16 sRuntimePanel = 1;
			
			LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
				(theDialog->FindPaneByID(item_MultiViewPrefs));
			LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
			
			multiView->SwitchToPanel(3);
			DoDataExchange_Globals(theDialog, true);
			multiView->SwitchToPanel(2);
			DoDataExchange_RtagBranchOptions(theDialog, noRecurs, true);
			multiView->SwitchToPanel(1);
			DoDataExchange_RtagBranchMain(theDialog, mf, sLastModule, sLastTag,
				hasDate, hasRev, sLastDate, sLastRev,
				useMostRecent, lookAttic, true);
			
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
				DoDataExchange_RtagBranchMain(theDialog, mf, sLastModule, sLastTag,
					hasDate, hasRev, sLastDate, sLastRev,
					useMostRecent, lookAttic, false);
				multiView->SwitchToPanel(2);
				DoDataExchange_RtagBranchOptions(theDialog, noRecurs, false);
				multiView->SwitchToPanel(3);
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
		if( tagType != kTagDelete )
		{
			if( hasDate && !sLastDate.empty() )
				date = sLastDate;
			
			if( hasRev && !sLastRev.empty() )
				rev = sLastRev;
		}
	
		tagName = sLastTag;
		modName = sLastModule;

		forceCvsroot = lastForceCvsroot;
		cvsroot = sLastCvsroot;

		if( mf && mf->NumDirs() > 0 && forceCvsroot )
		{
			string path;
			mf->getdir(0, path);
			
			InsertLastCvsroot(path.c_str(), cvsroot.c_str());
			InsertLastModuleName(path.c_str(), modName.c_str());
		}

		gCvsPrefs.save();
	}

	return userHitOK;
}
