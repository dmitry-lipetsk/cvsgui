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
 * ImportDlg.cpp : the cvs import dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */

#if qUnix
#	include "UCvsDialogs.h"
#	include "CheckoutDlg.h"
#endif

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LMultiPanelView.h>
#	include <LPopupGroupBox.h>
#	include <LCheckBox.h>
#	include <LTextEditView.h>
#	include <LEditText.h>
#	include <LPushButton.h>

#	include "MacCvsConstant.h"
#endif /* qMacCvsPP */

#include "ImportDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"
#include "MultiString.h"
#include "PromptFiles.h"
#include "ItemListDlg.h"
#include "CvsCommands.h"
#include "CvsAlert.h"
#include "CvsRootDlg.h"
#include "ImportFilterDlg.h"

using namespace std;

/// Default release tags
static char* sDefReleases[] = 
{
	"V10",
	"V101",
	0L
};

/// Default vendor tags
static char* sDefVendors[] = 
{
	"GNU",
	0L
};

/// Default module names
static char* sDefModuleNames[] = 
{
	"Module",
	"Project",
	0L
};

/// Default vendor branch ids
static char* sDefVendorBranchIds[] = 
{
	"1.1.7",
	"1.1.6",
	"1.1.5",
	"1.1.4",
	"1.1.3",
	"1.1.2",
	"1.1.1",
	0L
};

static CMString gVendorNames(20, "P_VendorNames", sDefVendors);
CMString gReleasesNames(20, "P_ReleaseNames", sDefReleases);
static CMString gImportModuleNames(20, "P_ImportModuleNames", sDefModuleNames);
static CMString gVendorBranchIds(20, "P_VendorBranchIds", sDefVendorBranchIds);
static CMString gImportLocalFolder(20, "P_ImportLocalFolder");

#ifdef WIN32
#	include "GetPrefs.h"

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif

extern CMString gCommitLogs;


//////////////////////////////////////////////////////////////////////////
// CImport_MAIN

IMPLEMENT_DYNAMIC(CImport_MAIN, CHHelpPropertyPage)

CImport_MAIN::CImport_MAIN(const MultiFiles* mf,
						   const char* logMsg, const char* modname,
						   const char* rtag, const char* vtag, const bool dontCreateVendorTag, const bool overwriteReleaseTags, 
						   bool forceRoot, const char* cvsroot) 
						   : m_mf(mf),
						   m_multilineEntryDlg(&m_cvsrootEntryDlg, mf), 
						   CHHelpPropertyPage(CImport_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CImport_MAIN)
	m_vtag = vtag;
	m_rtag = rtag;
	m_moduleName = modname;
	m_dontCreateVendorTag = dontCreateVendorTag;
	m_overwriteReleaseTags = overwriteReleaseTags;
	//}}AFX_DATA_INIT

	m_forceRoot = forceRoot;
	m_cvsroot = cvsroot;

	m_logMsg = logMsg;

	m_moduleCombo.SetItems(&gImportModuleNames);
	m_rtagCombo.SetItems(&gReleasesNames);
	m_vtagCombo.SetItems(&gVendorNames);
}

CImport_MAIN::~CImport_MAIN()
{
}

void CImport_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CImport_MAIN)
	DDX_Control(pDX, IDC_CVSROOT_ENTRY_STATIC, m_cvsrootEntryPlaceholder);
	DDX_Control(pDX, IDC_OVERWRITE_RELEASE_TAGS, m_overwriteReleaseTagsCheck);
	DDX_Control(pDX, IDC_MULTILINE_ENTRY_STATIC, m_multilineEntryPlaceholder);
	DDX_Control(pDX, IDC_BROWSE_TAG2, m_vtagButton);
	DDX_Control(pDX, IDC_BROWSE_TAG1, m_rtagButton);
	DDX_Control(pDX, IDC_NO_VENDOR_RELEASE_TAGS, m_dontCreateVendorTagCheck);
	DDX_Control(pDX, IDC_REPOSITORY_PATH, m_moduleCombo);
	DDX_Control(pDX, IDC_RELEASE_TAG, m_rtagCombo);
	DDX_Control(pDX, IDC_VENDOR_TAG, m_vtagCombo);
	DDX_CBString(pDX, IDC_VENDOR_TAG, m_vtag);
	DDX_CBString(pDX, IDC_RELEASE_TAG, m_rtag);
	DDX_CBString(pDX, IDC_REPOSITORY_PATH, m_moduleName);
	DDX_Check(pDX, IDC_NO_VENDOR_RELEASE_TAGS, m_dontCreateVendorTag);
	DDX_Check(pDX, IDC_OVERWRITE_RELEASE_TAGS, m_overwriteReleaseTags);
	//}}AFX_DATA_MAP

	DDV_MinChars(pDX, m_moduleName, 1);
	DDV_MinChars(pDX, m_rtag, 1);
	DDV_MinChars(pDX, m_vtag, 1);
	DDX_ComboMString(pDX, IDC_REPOSITORY_PATH, m_moduleCombo);
	DDX_ComboMString(pDX, IDC_VENDOR_TAG, m_vtagCombo);
	DDX_ComboMString(pDX, IDC_RELEASE_TAG, m_rtagCombo);

	// Error if the vendor and release tags are same
	if( pDX->m_bSaveAndValidate )
	{
		if( !m_dontCreateVendorTag && m_vtag == m_rtag )
		{
			CvsAlert(kCvsAlertWarningIcon, 
				"Invalid Vendor and Release tags.", "Vendor Tag and Release Tag can't have the same values.", 
				BUTTONTITLE_OK, NULL).ShowAlert();

			pDX->Fail();
		}
	}

	if( !pDX->m_bSaveAndValidate )
	{
		OnDontcreatevendortag();
	}

	m_multilineEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_logMsg);
	m_cvsrootEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_forceRoot, m_cvsroot);
}


BEGIN_MESSAGE_MAP(CImport_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CImport_MAIN)
	ON_BN_CLICKED(IDC_BROWSE_TAG1, OnBrowseTag1)
	ON_BN_CLICKED(IDC_BROWSE_TAG2, OnBrowseTag2)
	ON_BN_CLICKED(IDC_BROWSE_MODNAME, OnBrowseModules)
	ON_BN_CLICKED(IDC_NO_VENDOR_RELEASE_TAGS, OnDontcreatevendortag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImport_MAIN message handlers

/// OnInitDialog virtual override, initialize controls
BOOL CImport_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_multilineEntryDlg.Create(kMultilineLogMessage, &gCommitLogs, &m_multilineEntryPlaceholder, this);
	m_cvsrootEntryDlg.Create(&m_cvsrootEntryPlaceholder, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImport_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_REPOSITORY_PATH,			IDH_IDC_REPOSITORY_PATH);
	SetCtrlHelpID(IDC_BROWSE_MODNAME,			IDH_IDC_BROWSE_MODNAME);
	SetCtrlHelpID(IDC_VENDOR_TAG,				IDH_IDC_VENDOR_TAG);
	SetCtrlHelpID(IDC_BROWSE_TAG1,				IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_RELEASE_TAG,				IDH_IDC_RELEASE_TAG);
	SetCtrlHelpID(IDC_BROWSE_TAG2,				IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_NO_VENDOR_RELEASE_TAGS,	IDH_IDC_NO_VENDOR_RELEASE_TAGS);
	SetCtrlHelpID(IDC_OVERWRITE_RELEASE_TAGS,	IDH_IDC_OVERWRITE_RELEASE_TAGS);
}

/// BN_CLICKED message handler, display browse tag/branch dialog
void CImport_MAIN::OnBrowseTag1() 
{
	ItemSelectionData itemSelectionData(m_cvsrootEntryDlg);
	
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName, &itemSelectionData) )
	{
		m_vtag = tagName.c_str();
		m_vtagCombo.SetWindowText(m_vtag);
	}
}

/// BN_CLICKED message handler, display browse tag/branch dialog
void CImport_MAIN::OnBrowseTag2() 
{
	ItemSelectionData itemSelectionData(m_cvsrootEntryDlg);
	
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName, &itemSelectionData) )
	{
		m_rtag = tagName.c_str();
		m_rtagCombo.SetWindowText(m_rtag);
	}
}

/// BN_CLICKED message handler, browse for file/directory
void CImport_MAIN::OnBrowseModules() 
{
	ItemSelectionData itemSelectionData(m_cvsrootEntryDlg);
	
	string moduleName;
	if( CompatGetModuleListItem(m_mf, moduleName, &itemSelectionData) )
	{
		m_moduleName = moduleName.c_str();
		m_moduleCombo.SetWindowText(m_moduleName);
	}
}

/// BN_CLICKED message handler, disable the vendor and release tag
void CImport_MAIN::OnDontcreatevendortag() 
{
	const bool enable = m_dontCreateVendorTagCheck.GetCheck() == 0;

	m_rtagCombo.EnableWindow(enable);
	m_rtagButton.EnableWindow(enable);

	m_vtagCombo.EnableWindow(enable);
	m_vtagButton.EnableWindow(enable);

	m_overwriteReleaseTagsCheck.EnableWindow(enable);
}

/*!
	Get the log message
	\return The log message
*/
CString CImport_MAIN::GetLogMsg() const
{
	return m_logMsg;
}

/// Get the CVSROOT entry dialog
const CCvsrootEntryDlg& CImport_MAIN::GetCvsrootEntryDlg() const
{
	return m_cvsrootEntryDlg;
}

//////////////////////////////////////////////////////////////////////////
// CImport_OPTIONS

IMPLEMENT_DYNAMIC(CImport_OPTIONS, CHHelpPropertyPage)

CImport_OPTIONS::CImport_OPTIONS(const CCvsrootEntryDlg* cvsrootEntryDlg, 
								 const MultiFiles* mf, 
								 const bool serverCvswrappers, ReportConflict*& entries, ReportWarning*& warnings,
								 const char* path, const bool useDefIgnore, const bool useFilesTime, const bool createSandbox) 
								 : m_cvsrootEntryDlg(cvsrootEntryDlg), 
								 m_mf(mf), 
								 m_entries(entries), 
								 m_warnings(warnings),
								 CHHelpPropertyPage(CImport_OPTIONS::IDD)
{
	//{{AFX_DATA_INIT(CImport_OPTIONS)
	m_useDefIgnore = useDefIgnore;
	m_useFilesTime = useFilesTime;
	m_hasVendorBranch = FALSE;
	m_vendorBranch = _T("");
	m_createSandbox = createSandbox;
	m_path = path;
	m_serverCvswrappers = serverCvswrappers;
	//}}AFX_DATA_INIT

	m_vendorBranchCombo.SetItems(&gVendorBranchIds);
	m_pathCombo.SetItems(&gImportLocalFolder);
	m_pathCombo.SetReadOnly();
}

CImport_OPTIONS::~CImport_OPTIONS()
{
}

void CImport_OPTIONS::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CImport_OPTIONS)
	DDX_Control(pDX, IDC_SERVER_CVSWRAPPERS, m_serverCvswrappersCheck);
	DDX_Control(pDX, IDC_PATH, m_pathCombo);
	DDX_Control(pDX, IDC_BRANCHID, m_vendorBranchCombo);
	DDX_Control(pDX, IDC_BROWSE_BRANCHID, m_vendorBranchButton);
	DDX_Control(pDX, IDC_HAS_BRANCHID, m_vendorBranchCheck);
	DDX_Check(pDX, IDC_DEFAULT_CVSIGNORE, m_useDefIgnore);
	DDX_Check(pDX, IDC_FILES_MODIFICATION_TIME, m_useFilesTime);
	DDX_Check(pDX, IDC_HAS_BRANCHID, m_hasVendorBranch);
	DDX_CBString(pDX, IDC_BRANCHID, m_vendorBranch);
	DDX_Check(pDX, IDC_CREATE_SANDBOX, m_createSandbox);
	DDX_CBString(pDX, IDC_PATH, m_path);
	DDX_Check(pDX, IDC_SERVER_CVSWRAPPERS, m_serverCvswrappers);
	//}}AFX_DATA_MAP

	DDV_CheckPathExists(pDX, IDC_PATH, m_path, false);
	DDX_ComboMString(pDX, IDC_BRANCHID, m_vendorBranchCombo);
	DDX_ComboMString(pDX, IDC_PATH, m_pathCombo);
	
	if( !pDX->m_bSaveAndValidate )
	{
		OnBranchid();
	}
}


BEGIN_MESSAGE_MAP(CImport_OPTIONS, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CImport_OPTIONS)
	ON_BN_CLICKED(IDC_BROWSE_PATH, OnBtnpath)
	ON_BN_CLICKED(IDC_BROWSE_BRANCHID, OnBrowseBranchid)
	ON_BN_CLICKED(IDC_HAS_BRANCHID, OnBranchid)
	ON_BN_CLICKED(IDC_IMPORT_FILTER, OnImportfilter)
	ON_CBN_SELCHANGE(IDC_PATH, OnSelchangePath)
	ON_BN_CLICKED(IDC_SERVER_CVSWRAPPERS, OnServerCvswrappers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImport_OPTIONS message handlers

void CImport_OPTIONS::InitHelpID()
{
	SetCtrlHelpID(IDC_PATH,						IDH_IDC_PATH);
	SetCtrlHelpID(IDC_BROWSE_PATH,				IDH_IDC_BROWSE_PATH);
	SetCtrlHelpID(IDC_SERVER_CVSWRAPPERS,		IDH_IDC_SERVER_CVSWRAPPERS);
	SetCtrlHelpID(IDC_IMPORT_FILTER,			IDH_IDC_IMPORT_FILTER);
	SetCtrlHelpID(IDC_DEFAULT_CVSIGNORE,		IDH_IDC_DEFAULT_CVSIGNORE);
	SetCtrlHelpID(IDC_FILES_MODIFICATION_TIME,	IDH_IDC_FILES_MODIFICATION_TIME);
	SetCtrlHelpID(IDC_CREATE_SANDBOX,			IDH_IDC_CREATE_SANDBOX);
	SetCtrlHelpID(IDC_HAS_BRANCHID,				IDH_IDC_HAS_BRANCHID);
	SetCtrlHelpID(IDC_BRANCHID,					IDH_IDC_BRANCHID);
	SetCtrlHelpID(IDC_BROWSE_BRANCHID,			IDH_IDC_BROWSE_BRANCHID);
}

/// BN_CLICKED message handler, browse for import directory
void CImport_OPTIONS::OnBtnpath() 
{
	CString strPath;
	m_pathCombo.GetWindowText(strPath);

	string path(strPath);
	if( BrowserGetDirectory("Select a directory to import from", path, IsWindow(m_hWnd) ? this : NULL) )
	{
		if( strPath.CompareNoCase(path.c_str()) )
		{
			if( !ImportFilter(path.c_str()) )
				return;
		}

		m_pathCombo.SetWindowText(path.c_str());
	}
}

/// BN_CLICKED message handler, show import filter dialog
void CImport_OPTIONS::OnImportfilter() 
{
	CString strPath;
	m_pathCombo.GetWindowText(strPath);

	ReportConflict* entries = NULL;
	ReportWarning* warnings = NULL;

	copy_list_types(m_warnings, m_entries, warnings, entries);

	if( CompatImportFilter(strPath, entries, warnings) )
	{
		// Free the previous filters and use the new selection instead
		free_list_types(m_warnings, m_entries);
		
		m_entries = entries;
		m_warnings = warnings;
	}
	else
	{
		free_list_types(warnings, entries);
	}
}

/// BN_CLICKED message handler, show import filter dialog if unchecked and there are no filters specified
void CImport_OPTIONS::OnServerCvswrappers() 
{
	if( !m_serverCvswrappersCheck.GetCheck() )
	{
		if( !m_entries && !m_warnings )
		{
			OnImportfilter();
		}
	}
}

/// CBN_SELCHANGE message handler, run import filter
void CImport_OPTIONS::OnSelchangePath() 
{
	CString strPrevPath;
	m_pathCombo.GetWindowText(strPrevPath);

	m_pathCombo.EnsureSelchangeMatch();

	CString strPath;
	m_pathCombo.GetWindowText(strPath);
	
	if( strPrevPath.CompareNoCase(strPath) )
	{
		if( !ImportFilter(strPath) )
		{
			m_pathCombo.SelectString(-1, strPrevPath);
		}
	}
}

/*!
	Run the import filter
*/
bool CImport_OPTIONS::ImportFilter(const char* path)
{
	bool res = false;

	ReportConflict* entries = NULL;
	ReportWarning* warnings = NULL;

	if( CompatImportFilter(path, entries, warnings) )
	{
		// Free the previous filters and use the new selection instead
		free_list_types(m_warnings, m_entries);
		
		m_entries = entries;
		m_warnings = warnings;
		
		res = true;
	}

	return res;
}

/// BN_CLICKED message handler, browse for vendor branch id
void CImport_OPTIONS::OnBrowseBranchid() 
{
	auto_ptr<ItemSelectionData> itemSelectionData;
	
	if( m_cvsrootEntryDlg )
	{
		itemSelectionData = auto_ptr<ItemSelectionData>(new ItemSelectionData(*m_cvsrootEntryDlg));
	}
	
	string branchName;
	if( CompatGetTagListItem(m_mf, branchName, itemSelectionData.get()) )
	{
		m_vendorBranch = branchName.c_str();
		m_vendorBranchCombo.SetWindowText(m_vendorBranch);
	}
}

/// BN_CLICKED message handler, enable vendor branch controls
void CImport_OPTIONS::OnBranchid() 
{
	m_vendorBranchCombo.EnableWindow(m_vendorBranchCheck.GetCheck());
	m_vendorBranchButton.EnableWindow(m_vendorBranchCheck.GetCheck());
}

#endif /* WIN32 */

#ifdef qMacCvsPP
static void DoDataExchange_ImportMain(LWindow *theDialog, CStr & modname, CStr & logmsg, bool putValue)
{
	LEditText *mname = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_ModuleName));
	LTextEditView *editLog = dynamic_cast<LTextEditView*>
		(theDialog->FindPaneByID(item_LogMsg));
	
	if(putValue)
	{
		mname->SetText((Ptr)(const char *)modname, modname.length());	
		editLog->SetTextPtr((Ptr)(const char *)logmsg, logmsg.length());	
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
		Handle hdl = editLog->GetTextHandle();
		Assert_(hdl != 0L);
		logmsg.set(*hdl, GetHandleSize(hdl));
		
		char buf[512];
		Size len;
		mname->GetText(buf, 511, &len);
		modname.set(buf, len);
		
	}
}

static void DoDataExchange_ImportTags(LWindow *theDialog, CStr & vendortag, CStr & reltag, bool putValue)
{
	LEditText *vtag = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_Vendortag));
	LEditText *rtag = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_Reltag));
	
	if(putValue)
	{
		vtag->SetText((Ptr)(const char *)vendortag, vendortag.length());	
		rtag->SetText((Ptr)(const char *)reltag, reltag.length());	
		theDialog->SetLatentSub(vtag);
	}
	else
	{
		char buf[512];
		Size len;
		vtag->GetText(buf, 511, &len);
		vendortag.set(buf, len);
		rtag->GetText(buf, 511, &len);
		reltag.set(buf, len);
		
	}
}
#endif /* qMacCvsPP */

#if qUnix
class UCvsImport : public UWidget
{
	UDECLARE_DYNAMIC(UCvsImport)
public:
	UCvsImport() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsImport() {}

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

UIMPLEMENT_DYNAMIC(UCvsImport, UWidget)

UBEGIN_MESSAGE_MAP(UCvsImport, UWidget)
	ON_UCOMMAND(UCvsImport::kOK, UCvsImport::OnOK)
	ON_UCOMMAND(UCvsImport::kCancel, UCvsImport::OnCancel)
UEND_MESSAGE_MAP()

int UCvsImport::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsImport::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsImport::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsImport_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsImport_MAIN)
public:
	UCvsImport_MAIN(const char *logmsg, const char *modname,
					const char *rtag, const char *vtag);
	virtual ~UCvsImport_MAIN() {}

	enum
	{
		kEditModule = EV_COMMAND_START,	// 0
		kEditLog,						// 1
		kEditVendor,					// 2
		kEditRelease					// 3
	};

	virtual void DoDataExchange(bool fill);

	std::string m_logmsg;
	std::string m_modname;
	std::string m_rtag;
	std::string m_vtag;
protected:

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsImport_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsImport_MAIN, UWidget)
UEND_MESSAGE_MAP()

UCvsImport_MAIN::UCvsImport_MAIN(const char *logmsg, const char *modname,
								 const char *rtag, const char *vtag) : UWidget(::UEventGetWidID())
{
	m_logmsg = logmsg;
	m_modname = modname;
	m_rtag = rtag;
	m_vtag = vtag;
}

void UCvsImport_MAIN::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kEditModule, gOldModules);
	::DoDataExchange(fill, GetWidID(), kEditRelease, gReleasesNames);
	::DoDataExchange(fill, GetWidID(), kEditVendor, gVendorNames);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditLog, (void *)(const char *)m_logmsg.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditModule, (void *)(const char *)m_modname.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditRelease, (void *)(const char *)m_rtag.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditVendor, (void *)(const char *)m_vtag.c_str());
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditLog, &m_logmsg);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditModule, &m_modname);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditRelease, &m_rtag);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditVendor, &m_vtag);
	}
}

class UCvsImport_OPTIONS : public UWidget
{
	UDECLARE_DYNAMIC(UCvsImport_OPTIONS)
public:
	UCvsImport_OPTIONS(const char *path);
	virtual ~UCvsImport_OPTIONS() {}

	enum
	{
		kStatPath = EV_COMMAND_START,	// 0
		kBtnChange						// 1
	};

	virtual void DoDataExchange(bool fill);

	std::string m_path;
protected:

	ev_msg int OnChange(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsImport_OPTIONS, UWidget)

UBEGIN_MESSAGE_MAP(UCvsImport_OPTIONS, UWidget)
	ON_UCOMMAND(UCvsImport_OPTIONS::kBtnChange, UCvsImport_OPTIONS::OnChange)
UEND_MESSAGE_MAP()

UCvsImport_OPTIONS::UCvsImport_OPTIONS(const char *path) : UWidget(::UEventGetWidID())
{
	m_path = path;
}

int UCvsImport_OPTIONS::OnChange()
{
	std::string  dir;
	if( BrowserGetDirectory("Select a directory to import from", dir) )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kStatPath, (void*)dir.c_str());
	}

	return 0;
}

void UCvsImport_OPTIONS::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kStatPath, (void *)(const char *)m_path.c_str());
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kStatPath, &m_path);
	}
}
#endif // qUnix

/// Get the import options
bool CompatGetImport(const MultiFiles* mf,
					 bool& serverCvswrappers, ReportConflict*& entries, ReportWarning*& warnings, 
					 std::string& modname, std::string& logmsg,
					 std::string& vendortag, std::string& reltag, std::string& vendorBranchId, std::string& path, 
					 bool& useDefIgnore, bool& useFilesTime, bool& createSandbox,
					 bool& dontCreateVendorTag, bool& overwriteReleaseTags, 
					 bool& forceCvsroot, std::string& cvsroot)
{
	CvsAlert cvsAlert(kCvsAlertQuestionIcon, 
		_i18n("Do you want to scan import location?"), 
		_i18n("Selected import location will be scanned to detect file types before importing them.\n" 
			  "If you skip the scanning then server cvswrappers, if any, will be used to detect file types."),
		BUTTONTITLE_YES, BUTTONTITLE_NO);
	
	extern CPersistentBool gHideCommandDlgImportScan;
	cvsAlert.SetHideOption(&gHideCommandDlgImportScan);
	
	if( cvsAlert.ShowAlert() == kCvsAlertOKButton )
	{
		if( !CompatImportFilter(path.c_str(), entries, warnings) )
			return false;

		serverCvswrappers = false;
	}
	else
	{
		serverCvswrappers = true;
	}

	bool userHitOK = false;

	modname = "";
	logmsg = "";
	vendortag = "";
	reltag = "";
	vendorBranchId = "";
	useDefIgnore = false;
	useFilesTime = false;
	createSandbox = false;
	dontCreateVendorTag = false;
	overwriteReleaseTags = false;

	static string sLastModName;	/*Obsolete - remove it and always use sFolder! Get rid after all platforms have combo boxes for recent modules!*/
	
	static string sFolder;
	static string sLastMsg;
	static string sLastVTag("avendor");
	static string sLastRTag("arelease");
	static bool lastForceCvsroot = true;
	static string sLastCvsroot;
	
	bool hasVendorBranchId = false;

	// Find out the folder so we can suggest the module name
	string uppath, folder;
	if( SplitPath(path.c_str(), uppath, folder) )
		sFolder = folder;
	
	if( sLastModName.length() == 0L )	/*Obsolete - see above*/
	{
		sLastModName = folder;
	}

	FindBestLastCvsroot(path.c_str(), sLastCvsroot);

#ifdef WIN32
	CHHelpPropertySheet pages("Import settings");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CImport_MAIN page1(mf, sLastMsg.c_str(), sFolder.c_str(), sLastRTag.c_str(), sLastVTag.c_str(), dontCreateVendorTag, overwriteReleaseTags, lastForceCvsroot, sLastCvsroot.c_str());
	CImport_OPTIONS page2(&page1.GetCvsrootEntryDlg(), mf, serverCvswrappers, entries, warnings, path.c_str(), useDefIgnore, useFilesTime, createSandbox);
	CGetPrefs_GLOBALS page3;

	pages.AddPage(&page1);
	pages.AddPage(&page2);
	pages.AddPage(&page3);
	
	if( pages.DoModal() == IDOK )
	{
		sLastModName = (const char*)page1.m_moduleName;
		sLastMsg = (const char*)page1.GetLogMsg();
		sLastVTag = (const char*)page1.m_vtag;
		sLastRTag = (const char*)page1.m_rtag;
		dontCreateVendorTag = page1.m_dontCreateVendorTag != FALSE;
		overwriteReleaseTags = page1.m_overwriteReleaseTags != FALSE;
		lastForceCvsroot = page1.m_forceRoot == TRUE;
		sLastCvsroot = page1.m_cvsroot;

		hasVendorBranchId = page2.m_hasVendorBranch != FALSE;
		vendorBranchId = (const char*)page2.m_vendorBranch;
		path = (const char*)page2.m_path;
		serverCvswrappers = page2.m_serverCvswrappers != FALSE;
		useDefIgnore = page2.m_useDefIgnore != FALSE;
		useFilesTime = page2.m_useFilesTime != FALSE;
		createSandbox = page2.m_createSandbox != FALSE;

		page3.StoreValues();

		userHitOK = true;
	}
#endif /* WIN32 */

#ifdef qMacCvsPP
	StDialogHandler	theHandler(dlg_Import, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	static UInt16 sRuntimePanel = 1;
	
	LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
		(theDialog->FindPaneByID(item_MultiViewPrefs));
	LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
	
	multiView->SwitchToPanel(3);
	DoDataExchange_Globals(theDialog, true);
	multiView->SwitchToPanel(2);
	DoDataExchange (theDialog, item_ImportCvsIgnore, useDefIgnore, true);
	DoDataExchange (theDialog, item_ShowPath, path, true);
	dynamic_cast<LControl*> (theDialog->FindPaneByID (item_EditPath))->AddListener (&theHandler);
	dynamic_cast<LControl*> (theDialog->FindPaneByID (item_ImportRunFilter))->AddListener (&theHandler);
	dynamic_cast<LControl*> (theDialog->FindPaneByID (item_ImportUseVendorBranchID))->AddListener (&theHandler);
	dynamic_cast<LControl*> (theDialog->FindPaneByID (item_ImportNoVendorBranch))->AddListener (&theHandler);
	SetupHandler_CVSRoot(theDialog, theHandler);
	DoDataExchange_CVSRoot (theDialog, lastForceCvsroot, sLastCvsroot, true);
	theDialog->FindPaneByID(item_ImportVendorBranchID)->Disable();
	
	multiView->SwitchToPanel(1);
	DoDataExchange_ImportMain(theDialog, sLastModName, sLastMsg, true);
	DoDataExchange_ImportTags(theDialog, sLastVTag, sLastRTag, true);
	
	
	groupView->SetValue(sRuntimePanel);
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;

		if(hitMessage == item_EditPath)
		{
			std::string dir;
			if( BrowserGetDirectory("Select a directory to import from", dir) )
			{
				CPStr tmp(dir.c_str());
				theDialog->SetDescriptorForPaneID(item_ShowPath, tmp);
			}
		}
		else if(hitMessage == item_ImportRunFilter)
		{
	    ReportConflict* newEntries = NULL;
    	ReportWarning* newWarnings = NULL;

      LStr255   tmpPath;
      theDialog->GetDescriptorForPaneID(item_ShowPath, tmpPath);
    	if( CompatImportFilter(CPStr(tmpPath), newEntries, newWarnings) )
    	{
    		// Free the previous filters and use the new selection instead
    		free_list_types(warnings, entries);
    		
    		entries = newEntries;
    		warnings = newWarnings;
    	}
		}
		else if ( hitMessage == item_ForceCVSRoot  ||  hitMessage == item_CVSRootWizard )
		  HandleHitMessage_CVSRoot(theDialog, hitMessage);
		else if ( hitMessage == item_ImportNoVendorBranch ) {
		  if ( theDialog->FindPaneByID(item_ImportNoVendorBranch)->GetValue() == Button_Off) {
		    theDialog->FindPaneByID(item_Vendortag)->Enable();
		    theDialog->FindPaneByID(item_Reltag)->Enable();
		    theDialog->FindPaneByID(item_ImportOverwriteTags)->Enable();
		  } else {
		    theDialog->FindPaneByID(item_Vendortag)->Disable();
		    theDialog->FindPaneByID(item_Reltag)->Disable();
		    theDialog->FindPaneByID(item_ImportOverwriteTags)->SetValue(Button_Off);
		    theDialog->FindPaneByID(item_ImportOverwriteTags)->Disable();
		  }
		} else if ( hitMessage == item_ImportUseVendorBranchID ) {
		  if ( theDialog->FindPaneByID(item_ImportUseVendorBranchID)->GetValue() == Button_Off)
		    theDialog->FindPaneByID(item_ImportVendorBranchID)->Disable();
		  else
		    theDialog->FindPaneByID(item_ImportVendorBranchID)->Enable();
		}
	}
	theDialog->Hide();
	sRuntimePanel = groupView->GetValue();
	
	if(hitMessage == msg_OK)
	{
		multiView->SwitchToPanel(1);
		DoDataExchange_ImportMain(theDialog, sLastModName, sLastMsg, false);
		DoDataExchange_ImportTags(theDialog, sLastVTag, sLastRTag, false);
		multiView->SwitchToPanel(2);
		DoDataExchange (theDialog, item_ImportCvsIgnore, useDefIgnore, false);
		DoDataExchange (theDialog, item_ImportNoVendorBranch, dontCreateVendorTag, false);
		DoDataExchange (theDialog, item_ImportOverwriteTags, overwriteReleaseTags, false);
		DoDataExchange (theDialog, item_ImportUseFileModDate, useFilesTime, false);
		DoDataExchange (theDialog, item_ImportCreateCVSDirectories, createSandbox, false);
		DoDataExchange (theDialog, item_ImportUseVendorBranchID, hasVendorBranchId, false);
		DoDataExchange_CVSRoot (theDialog, lastForceCvsroot, sLastCvsroot, false);
		if ( hasVendorBranchId ) {
		  DoDataExchange (theDialog, item_ImportVendorBranchID, vendorBranchId, false);
		}
		DoDataExchange (theDialog, item_ShowPath, path, false);
		multiView->SwitchToPanel(3);
		DoDataExchange_Globals(theDialog, false);
		
		Assert_(sLastModName.length() != 0);
		
		userHitOK = true;
	}
#endif /* qMacCvsPP */

#if qUnix
	void *wid = UCreate_ImportDlg();

	UCvsImport *dlg = new UCvsImport();
	UCvsImport_MAIN *tab1 = new UCvsImport_MAIN(sLastMsg.c_str(), sLastModName.c_str(), sLastRTag.c_str(), sLastVTag.c_str());
	UCvsImport_OPTIONS *tab2 = new UCvsImport_OPTIONS(path.c_str());
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	dlg->AddPage(tab1, UCvsImport::kTabGeneral, 0);
	dlg->AddPage(tab2, UCvsImport::kTabGeneral, 1);

	if(dlg->DoModal())
	{
		sLastModName = tab1->m_modname;
		sLastMsg = tab1->m_logmsg;
		sLastVTag = tab1->m_vtag;
		sLastRTag = tab1->m_rtag;
		path = tab2->m_path;

		userHitOK = true;
	}

	delete dlg;
#endif // qUnix

	if( userHitOK )
	{
		gCvsPrefs.save();
		
		if( sLastMsg.length() == 0 )
			sLastMsg = "no message";

		if( sLastVTag.length() == 0 )
			sLastVTag = "initial";

		if( sLastRTag.length() == 0 )
			sLastRTag = "start";

		if( !hasVendorBranchId )
		{
			vendorBranchId = "";
		}

		modname = sLastModName.c_str();
		logmsg = sLastMsg;
		vendortag = sLastVTag;
		reltag = sLastRTag;

#ifdef TARGET_OS_MAC
		logmsg.replace('\015', '\012');
#endif /* TARGET_OS_MAC */

		forceCvsroot = lastForceCvsroot;
		cvsroot = sLastCvsroot;
		
		if( forceCvsroot )
		{
			InsertLastCvsroot(path.c_str(), cvsroot.c_str());
		}
		
		gCvsPrefs.save();
	}

	return userHitOK;
}
