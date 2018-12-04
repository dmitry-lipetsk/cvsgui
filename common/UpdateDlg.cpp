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
 * UpdateDlg.cpp : the cvs update dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "OptionalPropertySheet.h"
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
# include "UCommandDialogs.h"
#endif /* qMacCvsPP */

#include "UpdateDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"
#include "ItemListDlg.h"

using namespace std;

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

extern CMString gRevNames;
extern CMString gReleasesNames;
extern CMString gDateNames;
extern CMString gDateOrRevNames;

/// Default tag names
char* sDefTagNames[] = 
{
	"1.1",
	"1.6.2.4",
	"HEAD",
	"branch-release-1-0",
	0L
};

/// Default dates
char* sDefDateNames[] = 
{
	"1998-3",
	"1998-3-24",
	"1998-3-24 20:55",
	"24 Mar",
	"24 Mar 1998",
	"<=1998-3-24",
	0L
};

/// Default dates/revisions
char *sDefRevDateNames[] = 
{
	"1.1",
	"1.6.2.4",
	"HEAD",
	"branch-release-1-0",
	"1998-3",
	"1998-3-24",
	"1998-3-24 20:55",
	"24 Mar",
	"24 Mar 1998",
	"<=1998-3-24",
	0L
};

CMString gRevNames(20, "P_TagNames", sDefTagNames);
CMString gDateNames(20, "P_DateNames", sDefDateNames);
CMString gDateOrRevNames(20, "P_RebDateNames", sDefRevDateNames);

#ifdef WIN32

//////////////////////////////////////////////////////////////////////////
// CUpdateMain

IMPLEMENT_DYNAMIC(CUpdateMain, CHHelpPropertyPage)

CUpdateMain::CUpdateMain(bool createMissDir) 
	: CHHelpPropertyPage(CUpdateMain::IDD)
{
	//{{AFX_DATA_INIT(CUpdateMain)
	m_norecurs = FALSE;
	m_createmissdir = createMissDir;
	m_getcleancopy = FALSE;
	//}}AFX_DATA_INIT
}

CUpdateMain::~CUpdateMain()
{
}

void CUpdateMain::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateMain)
	DDX_Check(pDX, IDC_NORECURS, m_norecurs);
	DDX_Check(pDX, IDC_CREATE_MISSING_DIR, m_createmissdir);
	DDX_Check(pDX, IDC_GET_CLEAN_COPY, m_getcleancopy);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateMain, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CUpdateMain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CUpdateMain::InitHelpID()
{
	SetCtrlHelpID(IDC_NORECURS,				IDH_IDC_NORECURS);
	SetCtrlHelpID(IDC_CREATE_MISSING_DIR,	IDH_IDC_CREATE_MISSING_DIR);
	SetCtrlHelpID(IDC_GET_CLEAN_COPY,		IDH_IDC_GET_CLEAN_COPY);
}

//////////////////////////////////////////////////////////////////////////
// CUpdateOptions

IMPLEMENT_DYNAMIC(CUpdateOptions, CHHelpPropertyPage)

CUpdateOptions::CUpdateOptions(const CCvsrootEntryDlg* cvsrootEntryDlg, 
							   const MultiFiles* mf, 
							   const bool isExport, 
							   const char* rev, const char* date, const char* keyword, 
							   const bool resetSticky, const bool caseSensitiveNames, const bool lastCheckinTime) 
							   : m_cvsrootEntryDlg(cvsrootEntryDlg),
							   m_mf(mf), 
							   m_export(isExport),
							   CHHelpPropertyPage(CUpdateOptions::IDD)
{
	//{{AFX_DATA_INIT(CUpdateOptions)
	m_date = date;
	m_rev = rev;
	m_hasDate = FALSE;
	m_match = FALSE;
	m_hasRev = FALSE;
	m_resetSticky = resetSticky;
	m_stdout = FALSE;
	m_caseSensitiveNames = caseSensitiveNames;
	m_lastCheckinTime = lastCheckinTime;
	//}}AFX_DATA_INIT

	m_keyword = keyword;

	m_revCombo.SetItems(&gRevNames);
	m_dateCombo.SetItems(&gDateNames);
}

CUpdateOptions::~CUpdateOptions()
{
}

void CUpdateOptions::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateOptions)
	DDX_Control(pDX, IDC_STDOUT, m_stdoutCheck);
	DDX_Control(pDX, IDC_SUBSTMODE_ENTRY_STATIC, m_substModeEntryPlaceholder);
	DDX_Control(pDX, IDC_FORCE_HEAD_REV, m_matchCheck);
	DDX_Control(pDX, IDC_RESET_STICKY, m_resetStickyCheck);
	DDX_Control(pDX, IDC_BROWSE_TAG, m_browseTagButton);
	DDX_Control(pDX, IDC_HAS_REV, m_hasRevCheck);
	DDX_Control(pDX, IDC_HAS_DATE, m_hasDateCheck);
	DDX_Control(pDX, IDC_REV_TAG, m_revCombo);
	DDX_Control(pDX, IDC_DATE, m_dateCombo);
	DDX_CBString(pDX, IDC_DATE, m_date);
	DDX_CBString(pDX, IDC_REV_TAG, m_rev);
	DDX_Check(pDX, IDC_HAS_DATE, m_hasDate);
	DDX_Check(pDX, IDC_FORCE_HEAD_REV, m_match);
	DDX_Check(pDX, IDC_HAS_REV, m_hasRev);
	DDX_Check(pDX, IDC_RESET_STICKY, m_resetSticky);
	DDX_Check(pDX, IDC_STDOUT, m_stdout);
	DDX_Check(pDX, IDC_CASE_SENSITIVE_NAMES, m_caseSensitiveNames);
	DDX_Check(pDX, IDC_LAST_CHECKIN_TIME, m_lastCheckinTime);
	//}}AFX_DATA_MAP
	
	if( !m_resetSticky )
	{
		if( m_hasRev )
		{
			DDV_MinChars(pDX, m_rev, 1);
		}
		
		if( m_hasDate )
		{
			DDV_MinChars(pDX, m_date, 1);
		}
	}
	
	if( !pDX->m_bSaveAndValidate )
	{
		OnDelsticky();
		OnDate();
		OnRev();
	}

	DDX_ComboMString(pDX, IDC_REV_TAG, m_revCombo);
	DDX_ComboMString(pDX, IDC_DATE, m_dateCombo);

	m_substModeEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_keyword);
}


BEGIN_MESSAGE_MAP(CUpdateOptions, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CUpdateOptions)
	ON_BN_CLICKED(IDC_HAS_DATE, OnDate)
	ON_BN_CLICKED(IDC_HAS_REV, OnRev)
	ON_BN_CLICKED(IDC_BROWSE_TAG, OnBrowseTag)
	ON_BN_CLICKED(IDC_RESET_STICKY, OnDelsticky)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateOptions message handlers

/// OnInitDialog virtual override, initialize the controls
BOOL CUpdateOptions::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_resetStickyCheck.EnableWindow(!m_export);
	m_stdoutCheck.EnableWindow(!m_export);

	m_substModeEntryDlg.Create(&m_substModeEntryPlaceholder, this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUpdateOptions::InitHelpID()
{
	SetCtrlHelpID(IDC_RESET_STICKY,			IDH_IDC_RESET_STICKY);
	SetCtrlHelpID(IDC_HAS_DATE,				IDH_IDC_HAS_DATE);
	SetCtrlHelpID(IDC_DATE,					IDH_IDC_DATE);
	SetCtrlHelpID(IDC_HAS_REV,				IDH_IDC_HAS_REV);
	SetCtrlHelpID(IDC_REV_TAG,				IDH_IDC_REV_TAG);
	SetCtrlHelpID(IDC_BROWSE_TAG,			IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_FORCE_HEAD_REV,		IDH_IDC_FORCE_HEAD_REV);
	SetCtrlHelpID(IDC_STDOUT,				IDH_IDC_STDOUT);
	SetCtrlHelpID(IDC_CASE_SENSITIVE_NAMES,	IDH_IDC_CASE_SENSITIVE_NAMES);
	SetCtrlHelpID(IDC_LAST_CHECKIN_TIME,	IDH_IDC_LAST_CHECKIN_TIME);
}

/// BN_CLICKED message handler, enable controls
void CUpdateOptions::OnDate() 
{
	const BOOL enable = m_hasDateCheck.GetCheck() != 0 && !m_resetStickyCheck.GetCheck();
	
	m_dateCombo.EnableWindow(enable);
}

/// BN_CLICKED message handler, enable controls
void CUpdateOptions::OnRev() 
{
	const BOOL enable = m_hasRevCheck.GetCheck() && !m_resetStickyCheck.GetCheck();
	
	m_revCombo.EnableWindow(enable);
	m_browseTagButton.EnableWindow(enable);
}

/// BN_CLICKED message handler, display browse tag/branch dialog
void CUpdateOptions::OnBrowseTag() 
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

/// BN_CLICKED message handler, enable controls
void CUpdateOptions::OnDelsticky() 
{
	const BOOL enable = !m_resetStickyCheck.GetCheck();

	m_hasDateCheck.EnableWindow(enable);
	m_dateCombo.EnableWindow(enable);
	m_hasRevCheck.EnableWindow(enable);
	m_revCombo.EnableWindow(enable);
	m_browseTagButton.EnableWindow(enable);
	m_matchCheck.EnableWindow(enable);

	if( ::IsWindow(m_substModeEntryDlg.GetSafeHwnd()) )
	{
		m_substModeEntryDlg.EnableControls(enable);
	}

	OnDate();
	OnRev();
}
		 
//////////////////////////////////////////////////////////////////////////
// CUpdateMerge

IMPLEMENT_DYNAMIC(CUpdateMerge, CHHelpPropertyPage)

CUpdateMerge::CUpdateMerge(const CCvsrootEntryDlg* cvsrootEntryDlg, 
						   const MultiFiles* mf,
						   bool isCheckout, 
						   const char* rev1, const char* rev2, const char* date1, const char* date2,
						   const bool threeWayConflicts) 
						   : m_cvsrootEntryDlg(cvsrootEntryDlg),
						   m_mf(mf), 
						   m_mergeTypeCombo(USmartCombo::AutoDropWidth), 
						   CHHelpPropertyPage(CUpdateMerge::IDD)
{
	//{{AFX_DATA_INIT(CUpdateMerge)
	m_rev1 = rev1;
	m_rev2 = rev2;
	m_hasDate1 = FALSE;
	m_hasDate2 = FALSE;
	m_date1 = _T("");
	m_date2 = _T("");
	m_threeWayConflicts = threeWayConflicts;
	m_branchPointMerge = FALSE;
	m_mergeType = 0;
	//}}AFX_DATA_INIT

	m_isCheckout = isCheckout;

	m_rev1Combo.SetItems(m_isCheckout ? &gReleasesNames : &gRevNames);
	m_rev2Combo.SetItems(m_isCheckout ? &gReleasesNames : &gRevNames);
	m_date1Combo.SetItems(&gDateNames);
	m_date2Combo.SetItems(&gDateNames);
}

CUpdateMerge::~CUpdateMerge()
{
}

void CUpdateMerge::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateMerge)
	DDX_Control(pDX, IDC_MERGE_TYPE, m_mergeTypeCombo);
	DDX_Control(pDX, IDC_BRANCHPOINT_MERGE, m_branchPointMergeCheck);
	DDX_Control(pDX, IDC_THREE_WAY_CONFLICTS, m_threeWayConflictsCheck);
	DDX_Control(pDX, IDC_HAS_DATE2, m_hasDate2Check);
	DDX_Control(pDX, IDC_HAS_DATE1, m_hasDate1Check);
	DDX_Control(pDX, IDC_BROWSE_TAG2, m_browse2TagButton);
	DDX_Control(pDX, IDC_BROWSE_TAG1, m_browse1TagButton);
	DDX_Control(pDX, IDC_DATE2, m_date2Combo);
	DDX_Control(pDX, IDC_DATE1, m_date1Combo);
	DDX_Control(pDX, IDC_REV_TAG2, m_rev2Combo);
	DDX_Control(pDX, IDC_REV_TAG, m_rev1Combo);
	DDX_CBString(pDX, IDC_REV_TAG, m_rev1);
	DDX_CBString(pDX, IDC_REV_TAG2, m_rev2);
	DDX_Check(pDX, IDC_HAS_DATE1, m_hasDate1);
	DDX_Check(pDX, IDC_HAS_DATE2, m_hasDate2);
	DDX_CBString(pDX, IDC_DATE1, m_date1);
	DDX_CBString(pDX, IDC_DATE2, m_date2);
	DDX_Check(pDX, IDC_THREE_WAY_CONFLICTS, m_threeWayConflicts);
	DDX_Check(pDX, IDC_BRANCHPOINT_MERGE, m_branchPointMerge);
	DDX_CBIndex(pDX, IDC_MERGE_TYPE, m_mergeType);
	//}}AFX_DATA_MAP

	if( m_mergeType > 0 == 1 || m_mergeType == 2 )
	{
		DDV_MinChars(pDX, m_rev1, 1);
		if( m_hasDate1 )
		{
			DDV_MinChars(pDX, m_date1, 1);
		}
	}
	
	if( m_mergeType == 2 )
	{
		DDV_MinChars(pDX, m_rev2, 1);
		if( m_hasDate2 )
		{
			DDV_MinChars(pDX, m_date2, 1);
		}
	}
	
	if( !pDX->m_bSaveAndValidate )
	{
		OnSelchangeMergeType();
	}

	DDX_ComboMString(pDX, IDC_REV_TAG, m_rev1Combo);
	DDX_ComboMString(pDX, IDC_REV_TAG2, m_rev2Combo);
	DDX_ComboMString(pDX, IDC_DATE1, m_date1Combo);
	DDX_ComboMString(pDX, IDC_DATE2, m_date2Combo);
}

void CUpdateMerge::InitHelpID()
{
	SetCtrlHelpID(IDC_MERGE_TYPE,			IDH_IDC_MERGE_TYPE);
	SetCtrlHelpID(IDC_REV_TAG,				IDH_IDC_REV_TAG);
	SetCtrlHelpID(IDC_BROWSE_TAG1,			IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_HAS_DATE1,			IDH_IDC_HAS_DATE);
	SetCtrlHelpID(IDC_DATE1,				IDH_IDC_DATE);
	SetCtrlHelpID(IDC_REV_TAG2,				IDH_IDC_REV_TAG);
	SetCtrlHelpID(IDC_BROWSE_TAG2,			IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_HAS_DATE2,			IDH_IDC_HAS_DATE);
	SetCtrlHelpID(IDC_DATE2,				IDH_IDC_DATE);
	SetCtrlHelpID(IDC_THREE_WAY_CONFLICTS,	IDH_IDC_THREE_WAY_CONFLICTS);
	SetCtrlHelpID(IDC_BRANCHPOINT_MERGE,	IDH_IDC_BRANCHPOINT_MERGE);
}

/// CBN_SELCHANGE message handler, enable controls
void CUpdateMerge::OnSelchangeMergeType() 
{
	switch( m_mergeTypeCombo.GetCurSel() )
	{
	case 1:
		m_rev1Combo.EnableWindow(TRUE);
		m_rev2Combo.EnableWindow(FALSE);
		m_browse1TagButton.EnableWindow(TRUE);
		m_browse2TagButton.EnableWindow(FALSE);
		break;
	case 2:
		m_rev1Combo.EnableWindow(TRUE);
		m_rev2Combo.EnableWindow(TRUE);
		m_browse1TagButton.EnableWindow(TRUE);
		m_browse2TagButton.EnableWindow(TRUE);
		break;
	default:
		m_rev1Combo.EnableWindow(FALSE);
		m_rev2Combo.EnableWindow(FALSE);
		m_browse1TagButton.EnableWindow(FALSE);
		m_browse2TagButton.EnableWindow(FALSE);
		break;
	}

	m_hasDate1Check.EnableWindow(m_rev1Combo.IsWindowEnabled());
	m_hasDate2Check.EnableWindow(m_rev2Combo.IsWindowEnabled());
	m_date1Combo.EnableWindow(m_rev1Combo.IsWindowEnabled() && m_hasDate1Check.GetCheck());
	m_date2Combo.EnableWindow(m_rev2Combo.IsWindowEnabled() && m_hasDate2Check.GetCheck());
	m_threeWayConflictsCheck.EnableWindow(m_rev1Combo.IsWindowEnabled());
	m_branchPointMergeCheck.EnableWindow(m_rev1Combo.IsWindowEnabled());
}

/// BN_CLICKED message handler, enable controls
void CUpdateMerge::OnCheckdate1() 
{
	OnSelchangeMergeType();
}

/// BN_CLICKED message handler, enable controls
void CUpdateMerge::OnCheckdate2() 
{
	OnSelchangeMergeType();
}

/// BN_CLICKED message handler, display browse tag/branch dialog
void CUpdateMerge::OnBrowseTag1() 
{
	auto_ptr<ItemSelectionData> itemSelectionData;
	
	if( m_cvsrootEntryDlg )
	{
		itemSelectionData = auto_ptr<ItemSelectionData>(new ItemSelectionData(*m_cvsrootEntryDlg));
	}
	
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName, itemSelectionData.get()) )
	{
		m_rev1 = tagName.c_str();
		m_rev1Combo.SetWindowText(m_rev1);
	}
}

/// BN_CLICKED message handler, display browse tag/branch dialog
void CUpdateMerge::OnBrowseTag2() 
{	
	auto_ptr<ItemSelectionData> itemSelectionData;
	
	if( m_cvsrootEntryDlg )
	{
		itemSelectionData = auto_ptr<ItemSelectionData>(new ItemSelectionData(*m_cvsrootEntryDlg));
	}
	
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName, itemSelectionData.get()) )
	{
		m_rev2 = tagName.c_str();
		m_rev2Combo.SetWindowText(m_rev2);
	}
}

BEGIN_MESSAGE_MAP(CUpdateMerge, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CUpdateMerge)
	ON_BN_CLICKED(IDC_HAS_DATE1, OnCheckdate1)
	ON_BN_CLICKED(IDC_HAS_DATE2, OnCheckdate2)
	ON_BN_CLICKED(IDC_BROWSE_TAG1, OnBrowseTag1)
	ON_BN_CLICKED(IDC_BROWSE_TAG2, OnBrowseTag2)
	ON_CBN_SELCHANGE(IDC_MERGE_TYPE, OnSelchangeMergeType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#endif /* WIN32 */

#ifdef qMacCvsPP
void DoDataExchange_UpdateMerge(LWindow *theDialog, 
                                const MultiFiles* mf,
	                              bool & merge1, 
	                              bool & merge2,
                                CStr & rev1, 
                                CStr & rev2,
                                bool & hasDate1,
                                CStr & date1,
                                bool & hasDate2,
                                CStr & date2,
                                bool & produce3WayConflicts,
                                bool & mergeFromBranchPoint,
                                bool putValue)
{
/*
	LEditText *editRev = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditRevTag1));
	LEditText *editRev2 = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditRevTag2));
*/
	LPopupFiller *fill1 = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerRev1));
	LPopupFiller *fill2 = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerRev2));
	
	LTagWrapper *wrapper1 = putValue ? NEW LTagWrapper(fill1, mf) : 0L;
	fill1->DoDataExchange(gRevNames, putValue, wrapper1);
	LTagWrapper *wrapper2 = putValue ? NEW LTagWrapper(fill2, mf) : 0L;
	fill2->DoDataExchange(gRevNames, putValue, wrapper2);
	
	DoDataExchange(theDialog, item_Produce3WayConflicts, produce3WayConflicts, putValue);
	DoDataExchange(theDialog, item_MergeFromBranchPoint, mergeFromBranchPoint, putValue);
	DoDataExchange(theDialog, item_CheckDate1, hasDate1, putValue);
	DoDataExchange(theDialog, item_CheckDate2, hasDate2, putValue);
	DoDataExchange(theDialog, item_EditDate1, date1, putValue);
	DoDataExchange(theDialog, item_EditDate2, date2, putValue);
	DoDataExchange(theDialog, item_EditRevTag1, rev1, putValue);
	DoDataExchange(theDialog, item_EditRevTag2, rev2, putValue);
	
	if(putValue)
	{
		if(merge1 && merge2)
			theDialog->SetValueForPaneID (item_MergeType, 3);
		else if(merge1)
			theDialog->SetValueForPaneID (item_MergeType, 2);
		else
			theDialog->SetValueForPaneID (item_MergeType, 1);

/*
		editRev->SetText((Ptr)(const char *)rev1, rev1.length());
		editRev2->SetText((Ptr)(const char *)rev2, rev2.length());
		editDate1->SetText((Ptr)(const char *)date1, date1.length());
		editDate2->SetText((Ptr)(const char *)date2, date2.length());
*/
	}
	else
	{
	  switch ( theDialog->GetValueForPaneID(item_MergeType) )
	  {
	    case 3:
	      merge1 = merge2 = true;
	      break;
	    case 2:
	      merge1 = true;
	      merge2 = false;
	      break;
	    default:
  	    merge1 = merge2 = false;
	  }

/*
		char str[255];
		Size len;
		
		editRev->GetText(str, 254, &len);
		rev1.set(str, len);
		editRev2->GetText(str, 254, &len);
		rev2.set(str, len);
*/
	}
}

void SetupHandler_UpdateMerge(LWindow* theDialog, StDialogHandler& inHandler)
{
	LControl*   control;
	control = dynamic_cast<LControl*> (theDialog->FindPaneByID (item_MergeType));
	if ( control ) control->AddListener (&inHandler);
}

bool HandleHitMessage_UpdateMerge(LWindow *theDialog, MessageT hitMessage)
{
  if ( hitMessage == item_MergeType ) 
  {
    switch ( theDialog->GetValueForPaneID(item_MergeType) )
    {
      case 1:
        theDialog->FindPaneByID(item_MergeRev1Options)->Disable();
        theDialog->FindPaneByID(item_MergeRev2Options)->Disable();
        theDialog->FindPaneByID(item_Produce3WayConflicts)->Disable();
        theDialog->FindPaneByID(item_MergeFromBranchPoint)->Disable();
        break;
      case 2:
        theDialog->FindPaneByID(item_MergeRev1Options)->Enable();
        theDialog->FindPaneByID(item_MergeRev2Options)->Disable();
        theDialog->FindPaneByID(item_Produce3WayConflicts)->Enable();
        theDialog->FindPaneByID(item_MergeFromBranchPoint)->Enable();
        break;
      case 3:
        theDialog->FindPaneByID(item_MergeRev1Options)->Enable();
        theDialog->FindPaneByID(item_MergeRev2Options)->Enable();
        theDialog->FindPaneByID(item_Produce3WayConflicts)->Enable();
        theDialog->FindPaneByID(item_MergeFromBranchPoint)->Enable();
        break;
    }
    return true;
  }
  return false;
}

void DoDataExchange_UpdateOptions(LWindow *theDialog, 
                                  const MultiFiles* mf,
                                  bool & hasDate, 
                                  bool & hasRev,
                                  CStr & date, 
                                  CStr & rev, 
                                  bool & useMostRecent, 
                                  bool & resetSticky, 
                                  bool & toStdout,
		                              CStr & keyword,
		                              bool & caseSensitiveNames,
		                              bool & lastCheckinTime,
                                  bool putValue)
{
	LCheckBox *chkHasDate = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckDate));
	LCheckBox *chkHasRev = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckRevTag));
	LCheckBox *chkMostRecent = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_MostRecentRev));
	LCheckBox *chkHasKeyword = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckKeyword));
	LCheckBox *chkCaseSensitive = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CaseSensitive));
	LCheckBox *chkUseLastCheckinTime = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_UseLastCheckinTime));
	LEditText *editDate = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditDate));
	LEditText *editRev = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditRevTag));
	LEditText *editKeyword = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_Keyword));
	LPopupFiller *fillTag = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerTag));
	LPopupFiller *fillDate = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerDate));
	
	LTagWrapper *wrapper = putValue ? NEW LTagWrapper(fillTag, mf) : 0L;
	fillTag->DoDataExchange(gRevNames, putValue, wrapper);
	fillDate->DoDataExchange(gDateNames, putValue);
	DoDataExchange (theDialog, item_ResetSticky, resetSticky, putValue);
	DoDataExchange (theDialog, item_ToStout, toStdout, putValue);

	if(putValue)
	{
		chkHasDate->SetValue(hasDate ? Button_On : Button_Off);
		chkHasRev->SetValue(hasRev ? Button_On : Button_Off);
		chkHasKeyword->SetValue(keyword.empty() ? Button_Off : Button_On);
		chkMostRecent->SetValue(useMostRecent ? Button_On : Button_Off);
		chkCaseSensitive->SetValue(caseSensitiveNames ? Button_On : Button_Off);
		chkUseLastCheckinTime->SetValue(lastCheckinTime ? Button_On : Button_Off);
		editDate->SetText((Ptr)(const char *)date, date.length());
		editRev->SetText((Ptr)(const char *)rev, rev.length());
		editKeyword->SetText((Ptr)(const char*)keyword, keyword.length());
	}
	else
	{
		caseSensitiveNames = chkCaseSensitive->GetValue() == Button_On;
		lastCheckinTime = chkUseLastCheckinTime->GetValue() == Button_On;
	  if ( !resetSticky ) 
	  {
  		hasDate = chkHasDate->GetValue() == Button_On;
  		hasRev = chkHasRev->GetValue() == Button_On;
  		useMostRecent = chkMostRecent->GetValue() == Button_On;

  		char str[255];
  		Size len;
  		
  		editDate->GetText(str, 254, &len);
  		date.set(str, len);
  		editRev->GetText(str, 254, &len);
  		rev.set(str, len);
  		if ( chkHasKeyword->GetValue() == Button_On ) 
  		{
  		  editKeyword->GetText(str, 254, &len);
  		  keyword.set(str, len);
  		}
  	}
	}
}

static void DoDataExchange_UpdateSettings(LWindow *theDialog, 
                                          bool & noRecurs,
	                                        bool & createMissDir, 
	                                        bool & getCleanCopy, 
	                                        bool putValue)
{
	DoDataExchange (theDialog, item_NoRecurs, noRecurs, putValue);
	DoDataExchange (theDialog, item_CreateMissDir, createMissDir, putValue);
	DoDataExchange (theDialog, item_GetCleanCopy, getCleanCopy, putValue);
	
}
#endif /* qMacCvsPP */

#if qUnix
class UCvsUpdate : public UWidget
{
	UDECLARE_DYNAMIC(UCvsUpdate)
public:
	UCvsUpdate() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsUpdate() {}

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

UIMPLEMENT_DYNAMIC(UCvsUpdate, UWidget)

UBEGIN_MESSAGE_MAP(UCvsUpdate, UWidget)
	ON_UCOMMAND(UCvsUpdate::kOK, UCvsUpdate::OnOK)
	ON_UCOMMAND(UCvsUpdate::kCancel, UCvsUpdate::OnCancel)
UEND_MESSAGE_MAP()

int UCvsUpdate::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsUpdate::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsUpdate::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsUpdate_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsUpdate_MAIN)
public:
	UCvsUpdate_MAIN() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsUpdate_MAIN() {}

	enum
	{
		kCheckStdout = EV_COMMAND_START,	// 0
		kCheckRecurse,			// 1
		kCheckReset,			// 2
		kCheckMissing,			// 3
		kGetCleanCopy			// 4
	};

	virtual void DoDataExchange(bool fill);

	bool m_toStdout;
	bool m_noRecurs;
	bool m_resetSticky;
	bool m_createMissDir;
	bool m_getCleanCopy;
protected:

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsUpdate_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsUpdate_MAIN, UWidget)
UEND_MESSAGE_MAP()

void UCvsUpdate_MAIN::DoDataExchange(bool fill)
{
	if(fill)
	{
		m_toStdout = false;
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckStdout, m_toStdout), 0L);
		m_noRecurs = false;
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurse, m_noRecurs), 0L);
		m_resetSticky = false;
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckReset, m_resetSticky), 0L);
		m_createMissDir = false;
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckMissing, m_createMissDir), 0L);
		m_getCleanCopy = false;
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kGetCleanCopy, m_getCleanCopy), 0L);
	}
	else
	{
		m_toStdout = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckStdout, 0L) != 0;
		m_noRecurs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurse, 0L) != 0;
		m_resetSticky = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckReset, 0L) != 0;
		m_createMissDir = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckMissing, 0L) != 0;
		m_getCleanCopy = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kGetCleanCopy, 0L) != 0;
	}
}

class UCvsUpdate_STICKY : public UWidget
{
	UDECLARE_DYNAMIC(UCvsUpdate_STICKY)
public:
	UCvsUpdate_STICKY(const char *rev, const char *date);
	virtual ~UCvsUpdate_STICKY() {}

	enum
	{
		kCheckDate = EV_COMMAND_START,	// 0
		kComboDate,			// 1
		kCheckRev,			// 2
		kComboRev,			// 3
		kCheckMatch			// 4
	};

	virtual void DoDataExchange(bool fill);

	std::string m_date;
	std::string m_rev;
	bool m_hasdate;
	bool m_hasrev;
	bool m_match;
protected:

	ev_msg int OnDate(void);
	ev_msg int OnRev(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsUpdate_STICKY, UWidget)

UBEGIN_MESSAGE_MAP(UCvsUpdate_STICKY, UWidget)
	ON_UCOMMAND(UCvsUpdate_STICKY::kCheckDate, UCvsUpdate_STICKY::OnDate)
	ON_UCOMMAND(UCvsUpdate_STICKY::kCheckRev, UCvsUpdate_STICKY::OnRev)
UEND_MESSAGE_MAP()

UCvsUpdate_STICKY::UCvsUpdate_STICKY(const char *rev, const char *date) : UWidget(::UEventGetWidID())
{
	m_date = date;
	m_rev = rev;
	m_hasdate = false;
	m_hasrev = false;
	m_match = false;
}

int UCvsUpdate_STICKY::OnDate()
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDate, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboDate, state), 0L);
	return 0;
}

int UCvsUpdate_STICKY::OnRev()
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRev, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev, state), 0L);
	return 0;
}

void UCvsUpdate_STICKY::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kComboRev, gRevNames);
	::DoDataExchange(fill, GetWidID(), kComboDate, gDateNames);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckDate, m_hasdate), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRev, m_hasrev), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckMatch, m_match), 0L);

		OnDate();
		OnRev();
	}
	else
	{
		m_hasdate = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDate, 0L);
		m_hasrev = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRev, 0L);
		m_match = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckMatch, 0L);

		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboDate, &m_date);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboRev, &m_rev);
	}
}

class UCvsUpdate_MERGE : public UWidget
{
	UDECLARE_DYNAMIC(UCvsUpdate_MERGE)
public:
	UCvsUpdate_MERGE(const char *rev1, const char *rev2);
	virtual ~UCvsUpdate_MERGE() {}

	enum
	{
		kRadioNone = EV_COMMAND_START,	// 0
		kRadioMerge1,		// 1
		kRadioMerge2,		// 2
		kComboRev1,			// 3
		kComboRev2			// 4
	};

	virtual void DoDataExchange(bool fill);

	std::string m_rev1;
	std::string m_rev2;
	int m_merge;
protected:

	ev_msg int OnMerge(int cmd);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsUpdate_MERGE, UWidget)

UBEGIN_MESSAGE_MAP(UCvsUpdate_MERGE, UWidget)
	ON_UCOMMAND_RANGE(UCvsUpdate_MERGE::kRadioNone, UCvsUpdate_MERGE::kRadioMerge2, UCvsUpdate_MERGE::OnMerge)
UEND_MESSAGE_MAP()

UCvsUpdate_MERGE::UCvsUpdate_MERGE(const char *rev1, const char *rev2) : UWidget(::UEventGetWidID())
{
	m_rev1 = rev1;
	m_rev2 = rev2;
	m_merge = 0;
}

int UCvsUpdate_MERGE::OnMerge(int cmd)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRadioNone, 0L);
	if(state)
	{
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev1, 0), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev2, 0), 0L);
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev1, 1), 0L);
		state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRadioMerge1, 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev2, state == 0), 0L);
	}
	return 0;
}

void UCvsUpdate_MERGE::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kComboRev1, gRevNames);
	::DoDataExchange(fill, GetWidID(), kComboRev2, gRevNames);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kRadioNone + m_merge, 1), 0L);

		OnMerge(kRadioNone + m_merge);
	}
	else
	{
		if(UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRadioNone, 0L))
			m_merge = 0;
		else if(UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRadioMerge1, 0L))
			m_merge = 1;
		else
			m_merge = 2;

		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboRev1, &m_rev1);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboRev2, &m_rev2);
	}
}
#endif // qUnix

/// Get the update options
bool CompatGetUpdate(const MultiFiles* mf,
					 const bool queryOnly,
					 bool& toStdout, bool& noRecurs, bool& resetSticky,
					 std::string& date, std::string& rev,
					 bool& useMostRecent,
					 std::string& rev1, std::string& rev2,
					 bool& branchPointMerge, 
					 bool& threeWayConflicts, 
					 bool& createMissDir,
					 bool& getCleanCopy,
					 std::string& keyword,
					 bool& caseSensitiveNames,
					 bool& lastCheckinTime)
{
	bool userHitOK = false;

	static string sLastDate;
	static string sLastRev;
	static string sLastRev1;
	static string sLastRev2;
	static string sLastDate1;
	static string sLastDate2;

	bool hasDate = false;
	bool hasRev = false;
	bool merge1 = false;
	bool merge2 = false;
	bool hasDate1 = false;
	bool hasDate2 = false;

	date = "";
	rev = "";
	rev1 = "";
	rev2 = "";
	branchPointMerge = false;
	threeWayConflicts = gCvsPrefs.UpdateThreeWayConflicts();
	toStdout = false;
	noRecurs = false;
	resetSticky = false;
	useMostRecent = false;
	createMissDir = gCvsPrefs.UpdateBuildDirectories();
	getCleanCopy = false;
	keyword = "";
	caseSensitiveNames = gCvsPrefs.UpdateCaseSensitiveNames();
	lastCheckinTime = gCvsPrefs.UpdateLastCheckinTime();

	string caption = queryOnly ? _i18n("Query Update settings") : _i18n("Update settings");
	bool blnHideCommandDlg = queryOnly ? gCvsPrefs.HideCommandDlgQueryUpdate() : gCvsPrefs.HideCommandDlgUpdate();
	
#ifdef WIN32
	COptionalPropertySheet pages(blnHideCommandDlg, caption.c_str());
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CUpdateMain page1(createMissDir);
	CUpdateOptions page2(NULL, mf, false, sLastRev.c_str(), sLastDate.c_str(), keyword.c_str(), resetSticky, caseSensitiveNames, lastCheckinTime);
	CUpdateMerge page3(NULL, mf, false, sLastRev1.c_str(), sLastRev2.c_str(), sLastDate1.c_str(), sLastDate2.c_str(), threeWayConflicts);
	CGetPrefs_GLOBALS page4;

	pages.AddPage(&page1);
	pages.AddPage(&page2);
	pages.AddPage(&page3);
	pages.AddPage(&page4);
	
	if( pages.DoModal() == IDOK )
	{
		noRecurs = page1.m_norecurs == TRUE;
		gCvsPrefs.SetUpdateBuildDirectories(page1.m_createmissdir == TRUE);
		getCleanCopy = page1.m_getcleancopy == TRUE;

		sLastDate = page2.m_date;
		sLastRev = page2.m_rev;
		
		resetSticky = page2.m_resetSticky == TRUE;
		if( !resetSticky )
		{
			hasDate = page2.m_hasDate == TRUE;
			hasRev = page2.m_hasRev == TRUE;
			useMostRecent = page2.m_match == TRUE;
			keyword = page2.m_keyword;
		}
		
		toStdout = page2.m_stdout == TRUE;
		gCvsPrefs.SetUpdateCaseSensitiveNames(page2.m_caseSensitiveNames == TRUE);
		gCvsPrefs.SetUpdateLastCheckinTime(page2.m_lastCheckinTime == TRUE);
		
		merge1 = page3.m_mergeType >= 1;
		merge2 = page3.m_mergeType == 2;
		sLastRev1 = page3.m_rev1;
		sLastRev2 = page3.m_rev2;
		hasDate1 = page3.m_hasDate1 == TRUE;
		hasDate2 = page3.m_hasDate2 == TRUE;
		sLastDate1 = page3.m_date1;
		sLastDate2 = page3.m_date2;
		branchPointMerge = page3.m_branchPointMerge == TRUE;
		gCvsPrefs.SetUpdateThreeWayConflicts(page3.m_threeWayConflicts == TRUE);

		page4.StoreValues();
		
		if( queryOnly )
		{
			gCvsPrefs.SetHideCommandDlgQueryUpdate(pages.GetHideCommandDlg());
		}
		else
		{
			gCvsPrefs.SetHideCommandDlgUpdate(pages.GetHideCommandDlg());
		}

		userHitOK = true;
	}
#endif /* WIN32 */
#ifdef qMacCvsPP
	StOptionalDialogHandler	theHandler(dlg_Update, blnHideCommandDlg, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	static UInt16 sRuntimePanel = 1;
	
	LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
		(theDialog->FindPaneByID(item_MultiViewPrefs));
	LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
	
	multiView->SwitchToPanel(4);
	DoDataExchange_Globals(theDialog, true);
	multiView->SwitchToPanel(3);
	DoDataExchange_UpdateMerge(theDialog, mf, merge1, merge2, sLastRev1, sLastRev2, hasDate1, sLastDate1, hasDate2, sLastDate2, threeWayConflicts, branchPointMerge, true);
	SetupHandler_UpdateMerge(theDialog, theHandler);
	multiView->SwitchToPanel(2);
	DoDataExchange_UpdateOptions(theDialog, mf, hasDate, hasRev, sLastDate, sLastRev, useMostRecent, resetSticky, toStdout, keyword, caseSensitiveNames, lastCheckinTime, true);
	multiView->SwitchToPanel(1);
	DoDataExchange_UpdateSettings(theDialog, noRecurs, createMissDir, getCleanCopy, true);
	
	groupView->SetValue(sRuntimePanel);
  
	
	theHandler.ShowDialog();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
		
		HandleHitMessage_UpdateMerge(theDialog, hitMessage);
	}
	theDialog->Hide();
	sRuntimePanel = groupView->GetValue();
	
	if(hitMessage == msg_OK)
	{
		multiView->SwitchToPanel(1);
		DoDataExchange_UpdateSettings(theDialog, noRecurs, createMissDir, getCleanCopy, false);
		multiView->SwitchToPanel(2);
		DoDataExchange_UpdateOptions(theDialog, mf, hasDate, hasRev, sLastDate, sLastRev, useMostRecent, resetSticky, toStdout, keyword, caseSensitiveNames, lastCheckinTime, false);
		multiView->SwitchToPanel(3);
		DoDataExchange_UpdateMerge(theDialog, mf, merge1, merge2, sLastRev1, sLastRev2, hasDate1, sLastDate1, hasDate2, sLastDate2, threeWayConflicts, branchPointMerge, false);
		multiView->SwitchToPanel(4);
		DoDataExchange_Globals(theDialog, false);
		gCvsPrefs.SetUpdateBuildDirectories(createMissDir);
		gCvsPrefs.SetUpdateCaseSensitiveNames(caseSensitiveNames);
		gCvsPrefs.SetUpdateLastCheckinTime(lastCheckinTime);
		gCvsPrefs.SetUpdateThreeWayConflicts(threeWayConflicts);

		if( queryOnly )
		{
			gCvsPrefs.SetHideCommandDlgQueryUpdate(theHandler.GetHideCommandDlg());
		}
		else
		{
			gCvsPrefs.SetHideCommandDlgUpdate(theHandler.GetHideCommandDlg());
		}

		userHitOK = true;
	}
#endif /* qMacCvsPP */
#if qUnix
	void *wid;
#	if qGTK
	wid = UCreate_UpdateDlg();
#	endif

	UCvsUpdate *dlg = new UCvsUpdate();
	UCvsUpdate_MAIN *tab1 = new UCvsUpdate_MAIN();
	UCvsUpdate_STICKY *tab2 = new UCvsUpdate_STICKY(sLastRev.c_str(), sLastDate.c_str());
	UCvsUpdate_MERGE *tab3 = new UCvsUpdate_MERGE(sLastRev1.c_str(), sLastRev2.c_str());
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	dlg->AddPage(tab1, UCvsUpdate::kTabGeneral, 0);
	dlg->AddPage(tab2, UCvsUpdate::kTabGeneral, 1);
	dlg->AddPage(tab3, UCvsUpdate::kTabGeneral, 2);

	if(dlg->DoModal())
	{
		toStdout = tab1->m_toStdout;
		noRecurs = tab1->m_noRecurs;
		resetSticky = tab1->m_resetSticky;
		gCvsPrefs.SetUpdateBuildDirectories(tab1->m_createMissDir);
		getCleanCopy = tab1->m_getCleanCopy;

		sLastDate = tab2->m_date;
		sLastRev = tab2->m_rev;
		hasDate = tab2->m_hasdate;
		hasRev = tab2->m_hasrev;
		useMostRecent = tab2->m_match;

		merge1 = tab3->m_merge >= 1;
		merge2 = tab3->m_merge == 2;
		sLastRev1 = tab3->m_rev1;
		sLastRev2 = tab3->m_rev2;

		userHitOK = true;
	}

	delete dlg;
#endif // qUnix

	if( userHitOK )
	{
		createMissDir = gCvsPrefs.UpdateBuildDirectories();
		threeWayConflicts = gCvsPrefs.UpdateThreeWayConflicts();
		caseSensitiveNames = gCvsPrefs.UpdateCaseSensitiveNames();
		lastCheckinTime = gCvsPrefs.UpdateLastCheckinTime();

		if( hasDate && !sLastDate.empty() )
			date = sLastDate;

		if( hasRev && !sLastRev.empty() )
			rev = sLastRev;

		if( merge1 && merge2 && !sLastRev1.empty() && !sLastRev2.empty() )
		{
			rev1 = sLastRev1;
			rev2 = sLastRev2;
			
			if( hasDate1 )
			{
				rev1 += ":";
				rev1 += sLastDate1;
			}
			
			if( hasDate2 )
			{
				rev2 += ":";
				rev2 += sLastDate2;
			}
		}
		
		if( merge1 && !merge2 && !sLastRev1.empty() )
		{
			rev1 = sLastRev1;
			
			if( hasDate1 )
			{
				rev1 += ":";
				rev1 += sLastDate1;
			}
		}

		gCvsPrefs.save();
	}

	return userHitOK;
}
