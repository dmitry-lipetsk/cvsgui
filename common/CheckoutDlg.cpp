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
 * CheckoutDlg.h : the CVS checkout dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include <sys\stat.h>
#	include <direct.h>
#	include <errno.h>
#endif /* WIN32 */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LMultiPanelView.h>
#	include <LPopupGroupBox.h>
#	include <LCheckBox.h>
#	include <LTextEditView.h>
#	include <LEditText.h>
#	include <LPushButton.h>
#	include "LPopupFiller.h"

#	include "MacCvsConstant.h"
#endif /* qMacCvsPP */

#include "CheckoutDlg.h"
#include "CvsPrefs.h"
#include "MultiString.h"
#include "MultiFiles.h"
#include "UpdateDlg.h"
#include "PromptFiles.h"
#include "ItemListDlg.h"
#include "CvsRootDlg.h"
#include "FileTraversal.h"
#include "CvsEntries.h"

using namespace std;

CMString gOldModules(20, "P_OldModules");

/*!
	Check whether a given directory is a CVS-controlled one
	\param dir Directory to check
	\return true if the directory is a CVS-controlled, false otherwise
*/
static bool IsCvsDirectory(const char* dir)
{
	bool res = false;

	CSortList<ENTNODE> entries(200, ENTNODE::Compare);
	ignored_list_type ignlist;
	struct stat sb;

	string strPath = dir;
	strPath.erase(strPath.find_last_not_of(kPathDelimiter) + 1, string::npos);

	int statres = stat(strPath.c_str(), &sb);
	if( statres != -1 && S_ISDIR(sb.st_mode) )
	{
		string uppath, folder;
		if( !SplitPath(dir, uppath, folder) || uppath.empty() )
		{
			uppath = dir;
			folder = ".";
		}
		
		BuildIgnoredList(ignlist, uppath.c_str());
		
		EntnodeData* data = Entries_SetVisited(uppath.c_str(), entries, folder.c_str(), sb, true, &ignlist);
		if( !data->IsUnknown() )
		{
			res = true;
		}
	}

	return res;
}

#ifdef WIN32
#	include "GetPrefs.h"
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif

//////////////////////////////////////////////////////////////////////////
// CCheckout_MAIN

IMPLEMENT_DYNAMIC(CCheckout_MAIN, CHHelpPropertyPage)

CCheckout_MAIN::CCheckout_MAIN(const MultiFiles* mf, 
							   const char* modname, const char* path, const bool dontShortenPaths,
							   bool forceRoot, const char* cvsroot) 
	: m_mf(mf), 
	CHHelpPropertyPage(CCheckout_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CCheckout_MAIN)    
	m_noRecurse = FALSE;
	m_overrideCoDir = FALSE;
	m_checkoutDir = _T("");
	m_dontShortenPaths = FALSE;
	//}}AFX_DATA_INIT
	
	m_forceRoot = forceRoot;
	m_cvsroot = cvsroot;

	m_moduleName = modname;
	m_path = path;
	if( m_path.IsEmpty() )
	{
		CWincvsApp* app = (CWincvsApp*)AfxGetApp();
		if( 0L != app )
			m_path = app->GetRoot();
	}

	m_dontShortenPaths = dontShortenPaths;

	m_moduleCombo.SetItems(&gOldModules);
}

CCheckout_MAIN::~CCheckout_MAIN()
{
}

void CCheckout_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CCheckout_MAIN)
	DDX_Control(pDX, IDC_CVSROOT_ENTRY_STATIC, m_cvsrootEntryPlaceholder);
	DDX_Control(pDX, IDC_DONT_SHORTEN_PATHS, m_dontShortenPathsCheck);
	DDX_Control(pDX, IDC_CHECKOUT_DIR, m_checkoutDirEdit);
	DDX_Control(pDX, IDC_OVERRIDE_CHECKOUT_DIR, m_overrideCoDirCheck);
	DDX_Control(pDX, IDC_PATH, m_pathEdit);
	DDX_Control(pDX, IDC_MODNAME, m_moduleCombo);	    
	DDX_Text(pDX, IDC_MODNAME, m_moduleName);
	DDX_Text(pDX, IDC_PATH, m_path);
	DDX_Check(pDX, IDC_NORECURS, m_noRecurse);
	DDX_Check(pDX, IDC_OVERRIDE_CHECKOUT_DIR, m_overrideCoDir);
	DDX_Text(pDX, IDC_CHECKOUT_DIR, m_checkoutDir);
	DDX_Check(pDX, IDC_DONT_SHORTEN_PATHS, m_dontShortenPaths);
	//}}AFX_DATA_MAP

	DDV_MinChars(pDX, m_moduleName, 1);
	DDX_ComboMString(pDX, IDC_MODNAME, m_moduleCombo);
	DDV_CheckPathExists(pDX, IDC_PATH, m_path);

	if( !pDX->m_bSaveAndValidate )
	{
		//see if we are overriding the checkout directory
		OnCheckOverridecheckoutdir();
	}
	
	m_cvsrootEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_forceRoot, m_cvsroot);
}


BEGIN_MESSAGE_MAP(CCheckout_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CCheckout_MAIN)
	ON_BN_CLICKED(IDC_BROWSE_PATH, OnBtnPath)
	ON_BN_CLICKED(IDC_OVERRIDE_CHECKOUT_DIR, OnCheckOverridecheckoutdir)
	ON_BN_CLICKED(IDC_BROWSE_MODNAME, OnBrowseModules)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckout_MAIN message handlers

/// OnInitDialog virtual override, setup autocomplete
BOOL CCheckout_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	EnableEditAutoComplete(&m_pathEdit);
	
	m_cvsrootEntryDlg.Create(&m_cvsrootEntryPlaceholder, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCheckout_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_MODNAME,					IDH_IDC_MODNAME);
	SetCtrlHelpID(IDC_BROWSE_MODNAME,			IDH_IDC_BROWSE_MODNAME);
	SetCtrlHelpID(IDC_OVERRIDE_CHECKOUT_DIR,	IDH_IDC_OVERRIDE_CHECKOUT_DIR);
	SetCtrlHelpID(IDC_CHECKOUT_DIR,				IDH_IDC_CHECKOUT_DIR);
	SetCtrlHelpID(IDC_DONT_SHORTEN_PATHS,		IDH_IDC_DONT_SHORTEN_PATHS);
	SetCtrlHelpID(IDC_PATH,						IDH_IDC_PATH);
	SetCtrlHelpID(IDC_BROWSE_PATH,				IDH_IDC_BROWSE_PATH);
	SetCtrlHelpID(IDC_NORECURS,					IDH_IDC_NORECURS);
}

/// BN_CLICKED message handler, browse for checkout directory
void CCheckout_MAIN::OnBtnPath() 
{
	CString strPath;
	m_pathEdit.GetWindowText(strPath);
	
	string path(strPath);
	if( BrowserGetDirectory("Select a directory to checkout to", path, IsWindow(m_hWnd) ? this : NULL) )
	{
		m_pathEdit.SetWindowText(path.c_str());
	}
}

/// BN_CLICKED message handler, enable controls
void CCheckout_MAIN::OnCheckOverridecheckoutdir() 
{
	BOOL enableCheckoutDir = m_overrideCoDirCheck.GetCheck() != 0;

	m_checkoutDirEdit.EnableWindow(enableCheckoutDir);
	m_dontShortenPathsCheck.EnableWindow(enableCheckoutDir);

	if( enableCheckoutDir )
	{
		CString strCheckoutDir;
		m_checkoutDirEdit.GetWindowText(strCheckoutDir);
		if( strCheckoutDir.IsEmpty() )
		{
			CString strModuleName;
			m_moduleCombo.GetWindowText(strModuleName);

			if( !strModuleName.IsEmpty() )
			{
				m_checkoutDirEdit.SetWindowText(strModuleName);
				GotoDlgCtrl(&m_checkoutDirEdit);
			}
		}
	}
}

/// BN_CLICKED message handler, browse for file/directory
void CCheckout_MAIN::OnBrowseModules() 
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
const CCvsrootEntryDlg& CCheckout_MAIN::GetCvsrootEntryDlg() const
{
	return m_cvsrootEntryDlg;
}

#endif /* WIN32 */

#ifdef qMacCvsPP
static void DoDataExchange_CheckoutMain(LWindow *theDialog, const MultiFiles* mf, string & modname,
	bool & norecurs, bool & doexport, bool & overrideCheckoutDir, string & checkoutDir, bool & dontShortenModulePath, bool putValue)
{
	LEditText *mname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_ModuleName));
	LPopupFiller *filler = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_PopModName));
	
	LModuleWrapper *wrapper = putValue ? NEW LModuleWrapper(filler, mf) : 0L;
	filler->DoDataExchange(gOldModules, putValue, wrapper);
	
	DoDataExchange (theDialog, item_NoRecurs, norecurs, putValue);
	DoDataExchange (theDialog, item_CheckExport, doexport, putValue);
	DoDataExchange (theDialog, item_CheckGroupPath, overrideCheckoutDir, putValue);
	DoDataExchange (theDialog, item_ShowPath, checkoutDir, putValue);
	DoDataExchange (theDialog, item_CheckDontShortenModulePaths, dontShortenModulePath, putValue);
	
	if(putValue)
	{
		mname->SetText((Ptr)(const char *)modname, modname.length());	
		theDialog->SetLatentSub(mname);
		
		// this will take care about an empty module name
		mname->AddListener(GetDlgHandler());
		if(modname.length() == 0)
		{
			LPushButton *theOK = dynamic_cast<LPushButton*>
										(theDialog->FindPaneByID(item_OK));
			Assert_(theOK != 0L);
			theOK->Disable();
		}
	}
	else
	{
		char buf[512];
		Size len;
		mname->GetText(buf, 511, &len);
		modname.set(buf, len);
	}
}

static void 
DoDataExchange_CheckoutOptions(
	LView *theDialog, 
	const MultiFiles* mf,
	bool & hasDate, 
	bool & hasRev,
	string & sLastDate, 
	string & sLastRev, 
	bool & useMostRecent, 
	bool & doexport, 
	bool & forceCvsroot,
	bool putValue)

{
	LPopupFiller *fillTag = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerTag));
	LPopupFiller *fillDate = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerDate));
	
	LTagWrapper *wrapper = putValue ? NEW LTagWrapper(fillTag, mf) : 0L;
	fillTag->DoDataExchange(gRevNames, putValue, wrapper);
	fillDate->DoDataExchange(gDateNames, putValue);
	
	DoDataExchange (theDialog, item_CheckDate, hasDate, putValue);
	DoDataExchange (theDialog, item_CheckRevTag, hasRev, putValue);
	DoDataExchange (theDialog, item_EditDate, sLastDate, putValue);
	DoDataExchange (theDialog, item_EditRevTag, sLastRev, putValue);
	DoDataExchange (theDialog, item_MostRecentRev, useMostRecent, putValue);
	DoDataExchange (theDialog, item_CheckForceRoot, forceCvsroot, putValue);
	DoDataExchange (theDialog, item_CheckExport, doexport, putValue);

}
#endif /* qMacCvsPP */

#if qUnix
class UCvsCheckout : public UWidget
{
	UDECLARE_DYNAMIC(UCvsCheckout)
public:
	UCvsCheckout() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsCheckout() {}

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

UIMPLEMENT_DYNAMIC(UCvsCheckout, UWidget)

UBEGIN_MESSAGE_MAP(UCvsCheckout, UWidget)
	ON_UCOMMAND(UCvsCheckout::kOK, UCvsCheckout::OnOK)
	ON_UCOMMAND(UCvsCheckout::kCancel, UCvsCheckout::OnCancel)
UEND_MESSAGE_MAP()

int UCvsCheckout::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsCheckout::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsCheckout::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsCheckout_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsCheckout_MAIN)
public:
	UCvsCheckout_MAIN(const char * modname, const char * path);
	virtual ~UCvsCheckout_MAIN() {}

	enum
	{
		kComboPath = EV_COMMAND_START,	// 0
		kStaticPath,			// 1
		kBtnPath,				// 2
		kCheckStdout,			// 3
		kCheckRecurse			// 4
	};

	virtual void DoDataExchange(bool fill);

	string m_path;
	string m_modname;
	bool m_norecurs;
	bool m_stdout;

protected:
	ev_msg int OnPath(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsCheckout_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsCheckout_MAIN, UWidget)
	ON_UCOMMAND(UCvsCheckout_MAIN::kBtnPath, UCvsCheckout_MAIN::OnPath)
UEND_MESSAGE_MAP()

UCvsCheckout_MAIN::UCvsCheckout_MAIN(const char * modname, const char * path) : UWidget(::UEventGetWidID())
{
	m_path = path;
	m_modname = modname;
	m_norecurs = false;
	m_stdout = false;
}

int UCvsCheckout_MAIN::OnPath(void)
{
	string dir;
	if( BrowserGetDirectory("Select a directory to checkout to", dir) )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kStaticPath, (void*)dir.c_str());
	}
	
	return 0;
}

void UCvsCheckout_MAIN::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kComboPath, gOldModules);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckStdout, m_stdout), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurse, m_norecurs), 0L);

		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboPath, (void *)(const char *)m_modname.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kStaticPath, (void *)(const char *)m_path.c_str());
	}
	else
	{
		m_stdout = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckStdout, 0L) != 0;
		m_norecurs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurse, 0L) != 0;

		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboPath, &m_modname);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kStaticPath, &m_path);
	}
}

class UCvsCheckout_STICKY : public UWidget
{
	UDECLARE_DYNAMIC(UCvsCheckout_STICKY)
public:
	UCvsCheckout_STICKY(const char *rev, const char *date);
	virtual ~UCvsCheckout_STICKY() {}

	enum
	{
		kCheckDate = EV_COMMAND_START,	// 0
		kComboDate,			// 1
		kCheckRev,			// 2
		kComboRev,			// 3
		kCheckMatch			// 4
	};

	virtual void DoDataExchange(bool fill);

	string m_date;
	string m_rev;
	bool m_hasdate;
	bool m_hasrev;
	bool m_match;
protected:

	ev_msg int OnDate(void);
	ev_msg int OnRev(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsCheckout_STICKY, UWidget)

UBEGIN_MESSAGE_MAP(UCvsCheckout_STICKY, UWidget)
	ON_UCOMMAND(UCvsCheckout_STICKY::kCheckDate, UCvsCheckout_STICKY::OnDate)
	ON_UCOMMAND(UCvsCheckout_STICKY::kCheckRev, UCvsCheckout_STICKY::OnRev)
UEND_MESSAGE_MAP()

UCvsCheckout_STICKY::UCvsCheckout_STICKY(const char *rev, const char *date) : UWidget(::UEventGetWidID())
{
	m_date = date;
	m_rev = rev;
	m_hasdate = false;
	m_hasrev = false;
	m_match = false;
}

int UCvsCheckout_STICKY::OnDate()
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDate, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboDate, state), 0L);
	return 0;
}

int UCvsCheckout_STICKY::OnRev()
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRev, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev, state), 0L);
	return 0;
}

void UCvsCheckout_STICKY::DoDataExchange(bool fill)
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

class UCvsCheckout_OPTIONS : public UWidget
{
	UDECLARE_DYNAMIC(UCvsCheckout_OPTIONS)
public:
	UCvsCheckout_OPTIONS();
	virtual ~UCvsCheckout_OPTIONS() {}

	enum
	{
		kCheckForce = EV_COMMAND_START,	// 0
		kCheckExport		// 1
	};

	virtual void DoDataExchange(bool fill);

	bool m_export;
	bool m_forceroot;
protected:

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsCheckout_OPTIONS, UWidget)

UBEGIN_MESSAGE_MAP(UCvsCheckout_OPTIONS, UWidget)
UEND_MESSAGE_MAP()

UCvsCheckout_OPTIONS::UCvsCheckout_OPTIONS() : UWidget(::UEventGetWidID())
{
	m_export = false;
	m_forceroot = false;
}

void UCvsCheckout_OPTIONS::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckForce, m_forceroot), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckExport, m_export), 0L);
	}
	else
	{
		m_forceroot = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckForce, 0L);
		m_export = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckExport, 0L);
	}
}
#endif // qUnix

/// Get the checkout options
bool CompatGetCheckout(const MultiFiles* mf,
					   std::string& modname, std::string& path, bool& norecurs,
					   bool& toStdout, std::string& date, std::string& rev,
					   bool& useMostRecent, 
					   std::string& rev1, std::string& rev2,
					   bool& branchPointMerge, 
					   bool& threeWayConflicts, 
					   bool& forceCvsroot, std::string& cvsroot, 
					   bool& overrideCheckoutDir, std::string& checkoutDir,
					   std::string& keyword,
					   bool& resetSticky,
					   bool& dontShortenPaths,
					   bool& caseSensitiveNames,
					   bool& lastCheckinTime)
{
	bool userHitOK = false;

	date = "";
	rev = "";
	rev1 = "";
	rev2 = "";
	branchPointMerge = false;
	threeWayConflicts = gCvsPrefs.UpdateThreeWayConflicts();
	modname = "";
	norecurs = false;
	toStdout = false;
	useMostRecent = false;
	forceCvsroot = false;
	overrideCheckoutDir = false;
	caseSensitiveNames = gCvsPrefs.UpdateCaseSensitiveNames();
	lastCheckinTime = gCvsPrefs.UpdateLastCheckinTime();
	
	static string sLastModName;
	static string sLastRev;
	static string sLastDate;
	static string sLastRev1;
	static string sLastRev2;
	static string sLastDate1;
	static string sLastDate2;
	static bool lastDontShortenPaths = false;
	static bool lastForceCvsroot = true;
	static string sLastCvsroot;

	bool hasDate = false;
	bool hasRev = false;
	bool merge1 = false;
	bool merge2 = false;
	bool hasDate1 = false;
	bool hasDate2 = false;
	
	keyword = "";
	resetSticky = false;
	dontShortenPaths = lastDontShortenPaths;

	FindBestLastCvsroot(path.c_str(), sLastCvsroot);
	FindBestLastModuleName(path.c_str(), sLastModName);

	if( !path.empty() )
	{
		while( IsCvsDirectory(path.c_str()) )
		{
			string uppath, folder;
			if( !SplitPath(path.c_str(), uppath, folder) || uppath.empty() )
			{
				break;
			}

			path = uppath;
		}
	}

#ifdef WIN32
	CHHelpPropertySheet pages(_i18n("Checkout settings"));
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	CCheckout_MAIN page1(mf, sLastModName.c_str(), path.c_str(), dontShortenPaths, lastForceCvsroot, sLastCvsroot.c_str());
	CUpdateOptions page2(&page1.GetCvsrootEntryDlg(), mf, false, sLastRev.c_str(), sLastDate.c_str(), keyword.c_str(), resetSticky, caseSensitiveNames, lastCheckinTime);
	CUpdateMerge page3(&page1.GetCvsrootEntryDlg(), mf, true, sLastRev1.c_str(), sLastRev2.c_str(), sLastDate1.c_str(), sLastDate2.c_str(), threeWayConflicts);
	CGetPrefs_GLOBALS page4;

	pages.AddPage(&page1);
	pages.AddPage(&page2);
	pages.AddPage(&page3);
	pages.AddPage(&page4);
	
	if( pages.DoModal() == IDOK )
	{
		sLastModName = (const char*)page1.m_moduleName;
		path = (const char*)page1.m_path;
		norecurs = page1.m_noRecurse == TRUE;
		overrideCheckoutDir = page1.m_overrideCoDir == TRUE;
		checkoutDir = (const char*)page1.m_checkoutDir;
		lastDontShortenPaths = page1.m_dontShortenPaths == TRUE;
		lastForceCvsroot = page1.m_forceRoot == TRUE;
		sLastCvsroot = page1.m_cvsroot;

		sLastDate = page2.m_date;
		sLastRev = page2.m_rev;
		hasDate = page2.m_hasDate == TRUE;
		hasRev = page2.m_hasRev == TRUE;
		useMostRecent = page2.m_match == TRUE;
		keyword = page2.m_keyword;
		resetSticky = page2.m_resetSticky == TRUE;
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
		userHitOK = true;
	}
#endif /* WIN32 */
#ifdef qMacCvsPP
	StDialogHandler	theHandler(dlg_Checkout, LCommander::GetTopCommander());
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
	SetupHandler_CVSRoot(theDialog, theHandler);
	DoDataExchange_CVSRoot (theDialog, lastForceCvsroot, sLastCvsroot, true);
	DoDataExchange_CheckoutMain(theDialog, mf, sLastModName, norecurs, doexport, overrideCheckoutDir, checkoutDir, lastDontShortenPaths, true);
	dynamic_cast<LBroadcaster*> (theDialog->FindPaneByID (item_EditPath))->AddListener (&theHandler);
	dynamic_cast<LBroadcaster*> (theDialog->FindPaneByID (item_CheckGroupPath))->AddListener (&theHandler);
	
	
	groupView->SetValue(sRuntimePanel);
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
    
		if( !HandleHitMessage_CVSRoot(theDialog, hitMessage)  &&  
		    !HandleHitMessage_UpdateMerge(theDialog, hitMessage) )
		{
		  if (hitMessage == item_CheckGroupPath || hitMessage == item_EditPath)
  		{
  			if(theDialog->GetValueForPaneID(item_CheckGroupPath) == Button_Off)
  			{
  				continue;
  			}
  			
  			string dir;
  			if( BrowserGetDirectory("Select a directory to checkout to", dir) )
  			{
  				CPStr tmp(dir.c_str());
  				theDialog->SetDescriptorForPaneID(item_ShowPath, tmp);
  			}
  			else if(hitMessage == item_CheckGroupPath)
  			{
  				theDialog->SetValueForPaneID(item_CheckGroupPath, Button_Off);
  			}
  		}
  	}
	}
	theDialog->Hide();
	sRuntimePanel = groupView->GetValue();
	
	if(hitMessage == msg_OK)
	{
		multiView->SwitchToPanel(1);
	  DoDataExchange_CVSRoot (theDialog, lastForceCvsroot, sLastCvsroot, false);
		DoDataExchange_CheckoutMain(theDialog, mf, sLastModName, norecurs, doexport, overrideCheckoutDir, checkoutDir, lastDontShortenPaths, false);
		multiView->SwitchToPanel(2);
	  DoDataExchange_UpdateOptions(theDialog, mf, hasDate, hasRev, sLastDate, sLastRev, useMostRecent, resetSticky, toStdout, keyword, caseSensitiveNames, lastCheckinTime, false);
		multiView->SwitchToPanel(3);
	  DoDataExchange_UpdateMerge(theDialog, mf, merge1, merge2, sLastRev1, sLastRev2, hasDate1, sLastDate1, hasDate2, sLastDate2, threeWayConflicts, branchPointMerge, false);
		multiView->SwitchToPanel(4);
		DoDataExchange_Globals(theDialog, false);
		
		gCvsPrefs.SetUpdateCaseSensitiveNames(caseSensitiveNames);
		gCvsPrefs.SetUpdateLastCheckinTime(lastCheckinTime);
		gCvsPrefs.SetUpdateThreeWayConflicts(threeWayConflicts);

		Assert_(sLastModName.length() != 0);
		
		userHitOK = true;
	}
#endif /* qMacCvsPP */
#if qUnix
	void *wid = UCreate_CheckoutDlg();

	UCvsCheckout *dlg = new UCvsCheckout();
	UCvsCheckout_MAIN *tab1 = new UCvsCheckout_MAIN(sLastModName.c_str(), path.c_str());
	UCvsCheckout_STICKY *tab2 = new UCvsCheckout_STICKY(sLastRev.c_str(), sLastDate.c_str());
	UCvsCheckout_OPTIONS *tab3 = new UCvsCheckout_OPTIONS();
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	dlg->AddPage(tab1, UCvsCheckout::kTabGeneral, 0);
	dlg->AddPage(tab2, UCvsCheckout::kTabGeneral, 1);
	dlg->AddPage(tab3, UCvsCheckout::kTabGeneral, 2);

	if(dlg->DoModal())
	{
		sLastModName = tab1->m_modname;
		path = tab1->m_path;
		norecurs = tab1->m_norecurs;
		toStdout = tab1->m_stdout;

		sLastDate = tab2->m_date;
		sLastRev = tab2->m_rev;
		hasDate = tab2->m_hasdate;
		hasRev = tab2->m_hasrev;
		useMostRecent = tab2->m_match;

		doexport = tab3->m_export;
		forceCvsroot = tab3->m_forceroot;

		userHitOK = true;
	}

	delete dlg;
#endif // qUnix

	if( userHitOK )
	{
		threeWayConflicts = gCvsPrefs.UpdateThreeWayConflicts();
		caseSensitiveNames = gCvsPrefs.UpdateCaseSensitiveNames();
		lastCheckinTime = gCvsPrefs.UpdateLastCheckinTime();
		
		if( hasDate && !sLastDate.empty() )
			date = sLastDate;
		
		if( hasRev && !sLastRev.empty() )
			rev = sLastRev;

		modname = sLastModName;
		
		dontShortenPaths = lastDontShortenPaths;

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

		forceCvsroot = lastForceCvsroot;
		cvsroot = sLastCvsroot;
		
		if( forceCvsroot )
		{
			if( mf )
			{
				// Add selection rather than actual checkout folder
				string strSelection;
				mf->getdir(0, strSelection);

				if( !strSelection.empty() )
				{
					InsertLastCvsroot(strSelection.c_str(), cvsroot.c_str());
					InsertLastModuleName(strSelection.c_str(), modname.c_str());
				}
			}
			
			if( overrideCheckoutDir && !path.empty() )
			{
				// Add override folder if selected
				string strPath(path.c_str());
				strPath += kPathDelimiter;
				strPath += checkoutDir;
				
				InsertLastCvsroot(strPath.c_str(), cvsroot.c_str());
				InsertLastModuleName(strPath.c_str(), modname.c_str());
			}
		}

		gCvsPrefs.save();
	}

	return userHitOK;
}

/// Get the Export options
bool CompatGetExport(const MultiFiles* mf,
					   std::string& modname, std::string& path, bool& norecurs,
					   std::string& date, std::string& rev,
					   bool& useMostRecent, 
					   bool& forceCvsroot, std::string& cvsroot, 
					   bool& overrideCheckoutDir, std::string& checkoutDir,
					   std::string& keyword,
					   bool& dontShortenPaths,
					   bool& caseSensitiveNames,
					   bool& lastCheckinTime)
{
	bool userHitOK = false;

	date = "";
	rev = "";
	modname = "";
	norecurs = false;
	useMostRecent = false;
	forceCvsroot = false;
	overrideCheckoutDir = false;
	caseSensitiveNames = gCvsPrefs.UpdateCaseSensitiveNames();
	lastCheckinTime = gCvsPrefs.UpdateLastCheckinTime();
	
	static string sLastModName;
	static string sLastRev;
	static string sLastDate;
	static bool lastDontShortenPaths = false;
	static bool lastForceCvsroot = true;
	static string sLastCvsroot;

	bool hasDate = false;
	bool hasRev = false;

	keyword = "";
	dontShortenPaths = lastDontShortenPaths;

	FindBestLastCvsroot(path.c_str(), sLastCvsroot);
	FindBestLastModuleName(path.c_str(), sLastModName);

	if( !path.empty() )
	{
		while( IsCvsDirectory(path.c_str()) )
		{
			string uppath, folder;
			if( !SplitPath(path.c_str(), uppath, folder) || uppath.empty() )
			{
				break;
			}

			path = uppath;
		}
	}

#ifdef WIN32
	CHHelpPropertySheet pages(_i18n("Export settings"));
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	CCheckout_MAIN page1(mf, sLastModName.c_str(), path.c_str(), dontShortenPaths, lastForceCvsroot, sLastCvsroot.c_str());
	CUpdateOptions page2(&page1.GetCvsrootEntryDlg(), mf, true, sLastRev.c_str(), sLastDate.c_str(), keyword.c_str(), false, caseSensitiveNames, lastCheckinTime);
	CGetPrefs_GLOBALS page3;
	
	page1.m_psp.dwFlags |= PSP_USETITLE;
	page1.m_psp.pszTitle = "Export settings";
	
	page2.m_psp.dwFlags |= PSP_USETITLE;
	page2.m_psp.pszTitle = "Export options";

	pages.AddPage(&page1);
	pages.AddPage(&page2);
	pages.AddPage(&page3);

	if( pages.DoModal() == IDOK )
	{
		sLastModName = (const char*)page1.m_moduleName;
		path = (const char*)page1.m_path;
		norecurs = page1.m_noRecurse == TRUE;
		overrideCheckoutDir = page1.m_overrideCoDir == TRUE;
		checkoutDir = (const char*)page1.m_checkoutDir;
		lastDontShortenPaths = page1.m_dontShortenPaths == TRUE;
		lastForceCvsroot = page1.m_forceRoot == TRUE;
		sLastCvsroot = page1.m_cvsroot;

		sLastDate = page2.m_date;
		sLastRev = page2.m_rev;
		hasDate = page2.m_hasDate == TRUE;
		hasRev = page2.m_hasRev == TRUE;
		useMostRecent = page2.m_match == TRUE;
		keyword = page2.m_keyword;
		gCvsPrefs.SetUpdateCaseSensitiveNames(page2.m_caseSensitiveNames == TRUE);
		gCvsPrefs.SetUpdateLastCheckinTime(page2.m_lastCheckinTime == TRUE);

		page3.StoreValues();
		userHitOK = true;
	}
#endif /* WIN32 */
#ifdef qMacCvsPP
	StDialogHandler	theHandler(dlg_Checkout, LCommander::GetTopCommander());
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
	SetupHandler_CVSRoot(theDialog, theHandler);
	DoDataExchange_CVSRoot (theDialog, lastForceCvsroot, sLastCvsroot, true);
	DoDataExchange_CheckoutMain(theDialog, mf, sLastModName, norecurs, doexport, overrideCheckoutDir, checkoutDir, lastDontShortenPaths, true);
	dynamic_cast<LBroadcaster*> (theDialog->FindPaneByID (item_EditPath))->AddListener (&theHandler);
	dynamic_cast<LBroadcaster*> (theDialog->FindPaneByID (item_CheckGroupPath))->AddListener (&theHandler);
	
	
	groupView->SetValue(sRuntimePanel);
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
    
		if( !HandleHitMessage_CVSRoot(theDialog, hitMessage)  &&  
		    !HandleHitMessage_UpdateMerge(theDialog, hitMessage) )
		{
		  if (hitMessage == item_CheckGroupPath || hitMessage == item_EditPath)
  		{
  			if(theDialog->GetValueForPaneID(item_CheckGroupPath) == Button_Off)
  			{
  				continue;
  			}
  			
  			string dir;
  			if( BrowserGetDirectory("Select a directory to checkout to", dir) )
  			{
  				CPStr tmp(dir.c_str());
  				theDialog->SetDescriptorForPaneID(item_ShowPath, tmp);
  			}
  			else if(hitMessage == item_CheckGroupPath)
  			{
  				theDialog->SetValueForPaneID(item_CheckGroupPath, Button_Off);
  			}
  		}
  	}
	}
	theDialog->Hide();
	sRuntimePanel = groupView->GetValue();
	
	if(hitMessage == msg_OK)
	{
		multiView->SwitchToPanel(1);
	  DoDataExchange_CVSRoot (theDialog, lastForceCvsroot, sLastCvsroot, false);
		DoDataExchange_CheckoutMain(theDialog, mf, sLastModName, norecurs, doexport, overrideCheckoutDir, checkoutDir, lastDontShortenPaths, false);
		multiView->SwitchToPanel(2);
	  DoDataExchange_UpdateOptions(theDialog, mf, hasDate, hasRev, sLastDate, sLastRev, useMostRecent, resetSticky, toStdout, keyword, caseSensitiveNames, lastCheckinTime, false);
		multiView->SwitchToPanel(3);
	  DoDataExchange_UpdateMerge(theDialog, mf, merge1, merge2, sLastRev1, sLastRev2, hasDate1, sLastDate1, hasDate2, sLastDate2, threeWayConflicts, branchPointMerge, false);
		multiView->SwitchToPanel(4);
		DoDataExchange_Globals(theDialog, false);
		
		gCvsPrefs.SetUpdateCaseSensitiveNames(caseSensitiveNames);
		gCvsPrefs.SetUpdateLastCheckinTime(lastCheckinTime);
		gCvsPrefs.SetUpdateThreeWayConflicts(threeWayConflicts);

		Assert_(sLastModName.length() != 0);
		
		userHitOK = true;
	}
#endif /* qMacCvsPP */
#if qUnix
	void *wid = UCreate_CheckoutDlg();

	UCvsCheckout *dlg = new UCvsCheckout();
	UCvsCheckout_MAIN *tab1 = new UCvsCheckout_MAIN(sLastModName.c_str(), path.c_str());
	UCvsCheckout_STICKY *tab2 = new UCvsCheckout_STICKY(sLastRev.c_str(), sLastDate.c_str());
	UCvsCheckout_OPTIONS *tab3 = new UCvsCheckout_OPTIONS();
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	dlg->AddPage(tab1, UCvsCheckout::kTabGeneral, 0);
	dlg->AddPage(tab2, UCvsCheckout::kTabGeneral, 1);
	dlg->AddPage(tab3, UCvsCheckout::kTabGeneral, 2);

	if(dlg->DoModal())
	{
		sLastModName = tab1->m_modname;
		path = tab1->m_path;
		norecurs = tab1->m_norecurs;
		toStdout = tab1->m_stdout;

		sLastDate = tab2->m_date;
		sLastRev = tab2->m_rev;
		hasDate = tab2->m_hasdate;
		hasRev = tab2->m_hasrev;
		useMostRecent = tab2->m_match;

		doexport = tab3->m_export;
		forceCvsroot = tab3->m_forceroot;

		userHitOK = true;
	}

	delete dlg;
#endif // qUnix

	if( userHitOK )
	{
		caseSensitiveNames = gCvsPrefs.UpdateCaseSensitiveNames();
		lastCheckinTime = gCvsPrefs.UpdateLastCheckinTime();
		
		if( hasDate && !sLastDate.empty() )
			date = sLastDate;
		
		if( hasRev && !sLastRev.empty() )
			rev = sLastRev;

		modname = sLastModName;
		
		dontShortenPaths = lastDontShortenPaths;

		forceCvsroot = lastForceCvsroot;
		cvsroot = sLastCvsroot;
		
		if( forceCvsroot )
		{
			if( mf )
			{
				// Add selection rather than actual checkout folder
				string strSelection;
				mf->getdir(0, strSelection);

				if( !strSelection.empty() )
				{
					InsertLastCvsroot(strSelection.c_str(), cvsroot.c_str());
					InsertLastModuleName(strSelection.c_str(), modname.c_str());
				}
			}
			
			if( overrideCheckoutDir && !path.empty() )
			{
				// Add override folder if selected
				string strPath(path.c_str());
				strPath += kPathDelimiter;
				strPath += checkoutDir;
				
				InsertLastCvsroot(strPath.c_str(), cvsroot.c_str());
				InsertLastModuleName(strPath.c_str(), modname.c_str());
			}
		}

		gCvsPrefs.save();
	}

	return userHitOK;
}
