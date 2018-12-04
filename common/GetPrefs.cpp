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

// GetPrefs.cpp : implementation file
//

#include "stdafx.h"

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LMultiPanelView.h>
#	include <LEditText.h>
#	include <LPushButton.h>
#	include <LPopupGroupBox.h>

#	include "MacCvsConstant.h"
#	include "MacBinEncoding.h"
#	include "uwidget.h"
#endif /* qMacCvsPP */

#if qUnix
#	include "UCvsDialogs.h"
#	include <gtk/gtk.h>
#   include <gtk/gtkmain.h>
#   include <gtk/gtkfontsel.h>
#endif

#ifdef WIN32
#	include "wincvs.h"
#	include "umain.h"
//[2012-11-08] sf_animal: [#OLD_CODE] this is garbage? In any case - not supported in Win64.
//#	include <afxdao.h>
#	include "BrowseFileView.h"
#endif /* WIN32 */

#include "GetPrefs.h"
#include "CvsPrefs.h"
#include "Authen.h"
#include "AppConsole.h"
#include "AppGlue.h"
#include "PromptFiles.h"
#include "MultiFiles.h"
#include "CvsArgs.h"
#include "FileTraversal.h"
#include "CvsCommands.h"
#include "CvsAlert.h"
#include "MacrosSetup.h"

#ifdef WIN32   //{ Windows-Part

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

#ifdef WIN32
#define WINCVS_NAME_COLUMN	0
#define WINCVS_VALUE_COLUMN	1
#endif /* WIN32 */

CMString gPrevAltCvs(20, "P_PrevAltCvs");

extern CPersistentBool gHideCommandDlgUpdate;
extern CPersistentBool gHideCommandDlgQueryUpdate;
extern CPersistentBool gHideCommandDlgDiff;
extern CPersistentBool gHideCommandDlgLog;
extern CPersistentBool gHideCommandDlgGraph;
extern CPersistentBool gHideCommandDlgAnnotate;
extern CPersistentBool gHideCommandDlgStatus;
extern CPersistentBool gHideCommandDlgFileAction;
extern CPersistentBool gHideCommandDlgImportScan;
extern CPersistentBool gHideCommandDlgRemove;

/// Skip dialog info
struct SkipDialogInfo 
{
	const char* m_optionName;		/*!< Name of the option */
	CPersistentBool* m_pHideOption;	/*!< Hide option */
};

/// Skip dialog info container
static SkipDialogInfo gSkipDialogInfo [] = 
{
	"Update",		&gHideCommandDlgUpdate,
	"Query Update",	&gHideCommandDlgQueryUpdate,
	"Diff",			&gHideCommandDlgDiff,
	"Log",			&gHideCommandDlgLog,
	"Graph",		&gHideCommandDlgGraph,
	"Annotate",		&gHideCommandDlgAnnotate,
	"Status",		&gHideCommandDlgStatus,
	"File Action",	&gHideCommandDlgFileAction,
	"Import Scan",	&gHideCommandDlgImportScan,
	"Remove",		&gHideCommandDlgRemove
};

//////////////////////////////////////////////////////////////////////////
// CGetPrefs_WINCVS::CSettingInfo

CGetPrefs_WINCVS::CSettingInfo::CSettingInfo(kBrowseType browseType, const char* defaultPattern)
	: m_browseType(browseType),
	m_defaultPattern(defaultPattern)
{
}

/// Get the browse type
CGetPrefs_WINCVS::CSettingInfo::kBrowseType CGetPrefs_WINCVS::CSettingInfo::GetBrowseType() const
{
	return m_browseType;
}

/// Get the default pattern
const std::string& CGetPrefs_WINCVS::CSettingInfo::GetDefaultPattern() const
{
	return m_defaultPattern;
}

//////////////////////////////////////////////////////////////////////////
// CGetPrefs_GLOBALS

IMPLEMENT_DYNCREATE(CGetPrefs_GLOBALS, CHHelpPropertyPage)

CGetPrefs_GLOBALS::CGetPrefs_GLOBALS() 
	: CHHelpPropertyPage(CGetPrefs_GLOBALS::IDD), 
	m_isModified(false),
	m_checkoutFileAttributeCombo(USmartCombo::AutoDropWidth),
	m_cvsMessagesCombo(USmartCombo::AutoDropWidth)
{
	//{{AFX_DATA_INIT(CGetPrefs_GLOBALS)
	m_prune = gCvsPrefs.PruneOption();
	m_tcpip = gCvsPrefs.Z9Option();
	m_cntladd = gCvsPrefs.AddControl();
	m_logout = gCvsPrefs.LogoutTimeOut() > 0;
	m_logoutTime = gCvsPrefs.LogoutTimeOut();
	m_refreshInactive = gCvsPrefs.RefreshInactiveTimeOut() > 0;
	m_refreshInactiveTime = gCvsPrefs.RefreshInactiveTimeOut();
	m_unixlf = gCvsPrefs.UnixLF();
	m_zlevel = gCvsPrefs.ZLevel();
	m_disableSplashScreen = gCvsPrefs.DisableSplashScreen();
	m_matchEntriesCase = gCvsPrefs.MatchEntriesCase();
	m_smartSorting = gCvsPrefs.SmartSorting();
	m_encryptCommunication = gCvsPrefs.EncryptCommunication();
	m_checkoutFileAttribute = gCvsPrefs.CheckoutFileAttribute();
	m_cvsMessages = gCvsPrefs.CvsMessages();
	//}}AFX_DATA_INIT
}

CGetPrefs_GLOBALS::~CGetPrefs_GLOBALS()
{
}

void CGetPrefs_GLOBALS::DoDataExchange(CDataExchange* pDX)
{
	m_isModified = true;

	CHHelpPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CGetPrefs_GLOBALS)
	DDX_Control(pDX, IDC_CVSMESSAGES, m_cvsMessagesCombo);
	DDX_Control(pDX, IDC_CHECKOUTATTRIBUTE, m_checkoutFileAttributeCombo);
	DDX_Control(pDX, IDC_SPIN_AUTO_LOGOUT_TIME, m_logoutSpinButton);
	DDX_Control(pDX, IDC_AUTO_LOGOUT_TIME, m_logoutEdit);
	DDX_Control(pDX, IDC_AUTO_LOGOUT, m_logoutCheck);
	DDX_Control(pDX, IDC_SPIN_REFRESH_INACTIVE_TIME, m_refreshInactiveSpinButton);
	DDX_Control(pDX, IDC_REFRESH_INACTIVE_TIME, m_refreshInactiveEdit);
	DDX_Control(pDX, IDC_REFRESH_INACTIVE, m_refreshInactiveCheck);
	DDX_Control(pDX, IDC_SPIN_COMPRESSION, m_zspinSpinButton);
	DDX_Control(pDX, IDC_COMPRESSION, m_zlevelEdit);
	DDX_Control(pDX, IDC_HAS_COMPRESSION, m_tcpipCheck);
	DDX_Check(pDX, IDC_PRUNE_EMPTY_DIR, m_prune);
	DDX_Check(pDX, IDC_HAS_COMPRESSION, m_tcpip);
	DDX_Check(pDX, IDC_CONTROL_ADDED_FILES, m_cntladd);
	DDX_Check(pDX, IDC_AUTO_LOGOUT, m_logout);
	DDX_Text(pDX, IDC_AUTO_LOGOUT_TIME, m_logoutTime);
	DDV_MinMaxUInt(pDX, m_logoutTime, 0, 999999);
	DDX_Check(pDX, IDC_REFRESH_INACTIVE, m_refreshInactive);
	DDX_Text(pDX, IDC_REFRESH_INACTIVE_TIME, m_refreshInactiveTime);
	DDV_MinMaxUInt(pDX, m_refreshInactiveTime, 0, 999999);
	DDX_Check(pDX, IDC_UNIX_LF, m_unixlf);
	DDX_Text(pDX, IDC_COMPRESSION, m_zlevel);
	DDV_MinMaxUInt(pDX, m_zlevel, 1, 9);
	DDX_Check(pDX, IDC_NO_SPLASH_SCREEN, m_disableSplashScreen);
	DDX_Check(pDX, IDC_MATCH_ENTRIES_CASE, m_matchEntriesCase);
	DDX_Check(pDX, IDC_SMART_SORTING, m_smartSorting);
	DDX_Check(pDX, IDC_ENCRYPTION, m_encryptCommunication);
	DDX_CBIndex(pDX, IDC_CHECKOUTATTRIBUTE, m_checkoutFileAttribute);
	DDX_CBIndex(pDX, IDC_CVSMESSAGES, m_cvsMessages);
	//}}AFX_DATA_MAP

	if( !pDX->m_bSaveAndValidate )
	{
		OnTcpip();
		OnLogout();
		OnRefreshInactive();
	}
}

/// Save the values
void CGetPrefs_GLOBALS::StoreValues()
{
	if( !m_isModified )
		return;

	bool needsRefresh = (gCvsPrefs.MatchEntriesCase() != (m_matchEntriesCase != 0))
		|| (gCvsPrefs.SmartSorting() != (m_smartSorting != 0));

	gCvsPrefs.SetCheckoutFileAttribute((CHECKOUT_FILEATTRIBUTE_TYPE)m_checkoutFileAttribute);
	gCvsPrefs.SetCvsMessages((CVSMESSAGES_TYPE)m_cvsMessages);
	gCvsPrefs.SetPruneOption(m_prune ? true : false);
	gCvsPrefs.SetZ9Option(m_tcpip ? true : false);
	gCvsPrefs.SetAddControl(m_cntladd ? true : false);
	gCvsPrefs.SetLogoutTimeOut(m_logout ? (int)m_logoutTime : 0);
	gCvsPrefs.SetRefreshInactiveTimeOut(m_refreshInactive ? (int)m_refreshInactiveTime : 0);
	gCvsPrefs.SetUnixLF(m_unixlf != 0);
	gCvsPrefs.SetZLevel(m_zlevel);
	gCvsPrefs.SetDisableSplashScreen(m_disableSplashScreen ? true : false);
	gCvsPrefs.SetMatchEntriesCase(m_matchEntriesCase ? true : false);
	gCvsPrefs.SetSmartSorting(m_smartSorting ? true : false);
	gCvsPrefs.SetEncryptCommunication(m_encryptCommunication ? true : false);

	if( needsRefresh )
	{
		if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
		{
			app->GetFileView()->ResetView(true);
		}
	}
}

BEGIN_MESSAGE_MAP(CGetPrefs_GLOBALS, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CGetPrefs_GLOBALS)
	ON_BN_CLICKED(IDC_AUTO_LOGOUT, OnLogout)
	ON_BN_CLICKED(IDC_REFRESH_INACTIVE, OnRefreshInactive)
	ON_BN_CLICKED(IDC_UNIX_LF, OnCheckunixlf)
	ON_BN_CLICKED(IDC_HAS_COMPRESSION, OnTcpip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/// OnInitDialog virtual override, setup help IDs and spin controls
BOOL CGetPrefs_GLOBALS::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_logoutSpinButton.SetBase(10);
	m_logoutSpinButton.SetRange(0, 30000);
	
	m_refreshInactiveSpinButton.SetBase(100);
	m_refreshInactiveSpinButton.SetRange32(0, 60000);

	m_zspinSpinButton.SetBase(1);
	m_zspinSpinButton.SetRange(1, 9);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGetPrefs_GLOBALS::InitHelpID()
{
	SetCtrlHelpID(IDC_CHECKOUTATTRIBUTE,			IDH_IDC_CHECKOUTATTRIBUTE);
	SetCtrlHelpID(IDC_CVSMESSAGES,					IDH_IDC_CVSMESSAGES);
	SetCtrlHelpID(IDC_PRUNE_EMPTY_DIR,				IDH_IDC_PRUNE_EMPTY_DIR);
	SetCtrlHelpID(IDC_HAS_COMPRESSION,				IDH_IDC_HAS_COMPRESSION);
	SetCtrlHelpID(IDC_COMPRESSION,					IDH_IDC_COMPRESSION);
	SetCtrlHelpID(IDC_SPIN_COMPRESSION,				IDH_IDC_SPIN_COMPRESSION);
	SetCtrlHelpID(IDC_ENCRYPTION,					IDH_IDC_ENCRYPTION);
	SetCtrlHelpID(IDC_UNIX_LF,						IDH_IDC_UNIX_LF);
	SetCtrlHelpID(IDC_NO_SPLASH_SCREEN,				IDH_IDC_NO_SPLASH_SCREEN);
	SetCtrlHelpID(IDC_CONTROL_ADDED_FILES,			IDH_IDC_CONTROL_ADDED_FILES);
	SetCtrlHelpID(IDC_MATCH_ENTRIES_CASE,			IDH_IDC_MATCH_ENTRIES_CASE);
	SetCtrlHelpID(IDC_SMART_SORTING,				IDH_IDC_SMART_SORTING);
	SetCtrlHelpID(IDC_AUTO_LOGOUT,					IDH_IDC_AUTO_LOGOUT);
	SetCtrlHelpID(IDC_AUTO_LOGOUT_TIME,				IDH_IDC_AUTO_LOGOUT_TIME);
	SetCtrlHelpID(IDC_SPIN_AUTO_LOGOUT_TIME,		IDH_IDC_SPIN_AUTO_LOGOUT_TIME);
	SetCtrlHelpID(IDC_REFRESH_INACTIVE,				IDH_IDC_REFRESH_INACTIVE);
	SetCtrlHelpID(IDC_REFRESH_INACTIVE_TIME,		IDH_IDC_REFRESH_INACTIVE_TIME);
	SetCtrlHelpID(IDC_SPIN_REFRESH_INACTIVE_TIME,	IDH_IDC_SPIN_REFRESH_INACTIVE_TIME);
}

/// BN_CLICKED message handler, enable controls
void CGetPrefs_GLOBALS::OnTcpip() 
{
	m_zlevelEdit.EnableWindow(m_tcpipCheck.GetCheck() != 0);
	m_zspinSpinButton.EnableWindow(m_tcpipCheck.GetCheck() != 0);
}

/// BN_CLICKED message handler, enable controls
void CGetPrefs_GLOBALS::OnLogout() 
{
	m_logoutEdit.EnableWindow(m_logoutCheck.GetCheck() != 0);
	m_logoutSpinButton.EnableWindow(m_logoutCheck.GetCheck() != 0);
}

/// BN_CLICKED message handler, enable controls
void CGetPrefs_GLOBALS::OnRefreshInactive() 
{
	m_refreshInactiveEdit.EnableWindow(m_refreshInactiveCheck.GetCheck() != 0);
	m_refreshInactiveSpinButton.EnableWindow(m_refreshInactiveCheck.GetCheck() != 0);
}

/// BN_CLICKED message handler, display the warning about changing the line endings
void CGetPrefs_GLOBALS::OnCheckunixlf() 
{
	CvsAlert(kCvsAlertNoteIcon, "File-translation mode change.", 
		"Be careful when using this option!\n"
		"You can't use WinCvs in both modes - you have to use either Unix or DOS line feeds. "
		"If you experience some problems try to remove the global CVS files (such as .cvspass) because "
		"they probably don't match the new line feeds setting.").ShowAlert();
}

//////////////////////////////////////////////////////////////////////////
// CGetPrefs_WINCVS

IMPLEMENT_DYNCREATE(CGetPrefs_WINCVS, CHHelpPropertyPage)

CGetPrefs_WINCVS::CGetPrefs_WINCVS() 
	: m_settingsTypeCombo(USmartCombo::AutoDropWidth), 
	m_isModified(false),
	m_lastSelectedType(-1),
	CHHelpPropertyPage(CGetPrefs_WINCVS::IDD)
{
	//{{AFX_DATA_INIT(CGetPrefs_WINCVS)
	//}}AFX_DATA_INIT

	m_extDiff = gCvsPrefs.ExtDiff();
	m_viewer = gCvsPrefs.Viewer();
	m_conflictEditor = gCvsPrefs.ConflictEditor();
	m_annotateViewer = gCvsPrefs.AnnotateViewer();
	m_pythonLibrary = gCvsPrefs.PythonLibrary();
	m_tclLibrary = gCvsPrefs.TclLibrary();
	
	m_settingsFolder = gCvsPrefs.SettingsFolder();
	m_macrosFolder = gCvsPrefs.MacrosFolder();
	m_pythonLibFolder = gCvsPrefs.PythonLibFolder();
}

CGetPrefs_WINCVS::~CGetPrefs_WINCVS()
{
}

void CGetPrefs_WINCVS::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CGetPrefs_WINCVS)
	DDX_Control(pDX, IDC_WINCVS_SETTINGS_LIST, m_settingsList);
	DDX_Control(pDX, IDC_CLEAR_WINCVS_SETTINGS, m_clearButton);
	DDX_Control(pDX, IDC_BROWSE_WINCVS_SETTINGS, m_browseButton);
	DDX_Control(pDX, IDC_WINCVS_SETTINGS_TYPE, m_settingsTypeCombo);
	//}}AFX_DATA_MAP

	if( pDX->m_bSaveAndValidate )
	{
		GetSettings();
	}
	else
	{
		ShowSettings();
	}
}

/// Save the values
void CGetPrefs_WINCVS::StoreValues()
{
	if( !m_isModified )
		return;
	
	const bool restartScriptEngine = m_tclLibrary != gCvsPrefs.TclLibrary() || 
		m_pythonLibrary != gCvsPrefs.PythonLibrary() ||
		m_macrosFolder != gCvsPrefs.MacrosFolder() ||
		m_pythonLibFolder != gCvsPrefs.PythonLibFolder();

	gCvsPrefs.SetExtDiff(m_extDiff);
	gCvsPrefs.SetViewer(m_viewer);
	gCvsPrefs.SetConflictEditor(m_conflictEditor);
	gCvsPrefs.SetAnnotateViewer(m_annotateViewer);
	gCvsPrefs.SetPythonLibrary(m_pythonLibrary);
	gCvsPrefs.SetTclLibrary(m_tclLibrary);

	gCvsPrefs.SetSettingsFolder(m_settingsFolder);
	gCvsPrefs.SetMacrosFolder(m_macrosFolder);
	gCvsPrefs.SetPythonLibFolder(m_pythonLibFolder);

	if( restartScriptEngine )
	{
		if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
		{
			app->RestartScriptEngine();
		}
	}
}

BEGIN_MESSAGE_MAP(CGetPrefs_WINCVS, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CGetPrefs_WINCVS)
	ON_CBN_SELCHANGE(IDC_WINCVS_SETTINGS_TYPE, OnSelchangeWincvsSettingsType)
	ON_BN_CLICKED(IDC_BROWSE_WINCVS_SETTINGS, OnBrowseWincvsSettings)
	ON_BN_CLICKED(IDC_CLEAR_WINCVS_SETTINGS, OnClearWincvsSettings)
	ON_NOTIFY(NM_DBLCLK, IDC_WINCVS_SETTINGS_LIST, OnDblclkWincvsSettingsList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_WINCVS_SETTINGS_LIST, OnItemchangedWincvsSettingsList)
	ON_BN_CLICKED(IDC_BROWSE_WINCVS_SETTINGS, OnBrowseWincvsSettings)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/// OnInitDialog virtual override, setup autocomplete and help IDs
BOOL CGetPrefs_WINCVS::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	InitListCtrl();

	m_settingsTypeCombo.InsertString(0, "Folders");
	m_settingsTypeCombo.InsertString(0, "Programs");
	
	m_settingsTypeCombo.SetCurSel(0);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGetPrefs_WINCVS::InitHelpID()
{
	SetCtrlHelpID(IDC_WINCVS_SETTINGS_TYPE,		IDH_IDC_WINCVS_SETTINGS_TYPE);
	SetCtrlHelpID(IDC_WINCVS_SETTINGS_LIST,		IDH_IDC_WINCVS_SETTINGS_LIST);
	SetCtrlHelpID(IDC_BROWSE_WINCVS_SETTINGS,	IDH_IDC_BROWSE_WINCVS_SETTINGS);
	SetCtrlHelpID(IDC_CLEAR_WINCVS_SETTINGS,	IDH_IDC_CLEAR_WINCVS_SETTINGS);
}

/// WM_DESTROY message handler, clear items display
void CGetPrefs_WINCVS::OnDestroy() 
{
	ClearItemsDisplay();

	CHHelpPropertyPage::OnDestroy();
}

/// Initialize list control
void CGetPrefs_WINCVS::InitListCtrl()
{
	SetListCtrlExtendedStyle(&m_settingsList, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_GRIDLINES);
	
	// Get the client area width to properly size columns
	CRect rect;
	m_settingsList.GetClientRect(rect);
	
	const int listCtrlWidth = rect.Width();
	
	const int listCtrlWidthNoScroll = listCtrlWidth - GetSystemMetrics(SM_CXVSCROLL);
	const int fistColWidth = 100;
	
	m_settingsList.InsertColumn(WINCVS_NAME_COLUMN, _i18n("Name"), LVCFMT_LEFT, fistColWidth);
	m_settingsList.InsertColumn(WINCVS_VALUE_COLUMN, _i18n("Value"), LVCFMT_LEFT, listCtrlWidthNoScroll - fistColWidth);
}

/// ON_CBN_SELCHANGE message handler, show setting for selected type
void CGetPrefs_WINCVS::OnSelchangeWincvsSettingsType() 
{
	UpdateData(TRUE);
	UpdateData(FALSE);
	EnableEditControls();
}

/// Get the settings for the last selected type
void CGetPrefs_WINCVS::GetSettings()
{
	if( m_lastSelectedType < 0 )
	{
		return;
	}

	int item = 0;

	switch( m_lastSelectedType )
	{
	case kPrograms:
		m_extDiff = m_settingsList.GetItemText(item++, WINCVS_VALUE_COLUMN);
		m_viewer = m_settingsList.GetItemText(item++, WINCVS_VALUE_COLUMN);
		m_conflictEditor = m_settingsList.GetItemText(item++, WINCVS_VALUE_COLUMN);
		m_annotateViewer = m_settingsList.GetItemText(item++, WINCVS_VALUE_COLUMN);
		m_pythonLibrary = m_settingsList.GetItemText(item++, WINCVS_VALUE_COLUMN);
		m_tclLibrary = m_settingsList.GetItemText(item++, WINCVS_VALUE_COLUMN);
		break;
	case kFolders:
		m_settingsFolder = m_settingsList.GetItemText(item++, WINCVS_VALUE_COLUMN);
		m_macrosFolder = m_settingsList.GetItemText(item++, WINCVS_VALUE_COLUMN);
		m_pythonLibFolder = m_settingsList.GetItemText(item++, WINCVS_VALUE_COLUMN);
		break;
	default:
		ASSERT(FALSE); // Unknown type
		break;
	}
}

/// Clear items display
void CGetPrefs_WINCVS::ClearItemsDisplay()
{
	// Clear items display
	for(int nIndex = 0; nIndex < m_settingsList.GetItemCount(); nIndex++)
	{
		CSettingInfo* pSettingsInfo = (CSettingInfo*)m_settingsList.GetItemData(nIndex);
		delete pSettingsInfo;
	}

	m_settingsList.DeleteAllItems();
}

/// Show settings for a selected type
void CGetPrefs_WINCVS::ShowSettings()
{
	ClearItemsDisplay();
	
	m_lastSelectedType = m_settingsTypeCombo.GetCurSel();
	if( m_lastSelectedType < 0 )
	{
		return;
	}

	switch( m_lastSelectedType )
	{
	case kPrograms: // Programs
		AddSetting("External Diff", m_extDiff, CSettingInfo::kBrowseType::kBrowseExecutable, "windiff.exe");
		AddSetting("Default Editor", m_viewer, CSettingInfo::kBrowseType::kBrowseExecutable, "notepad.exe");
		AddSetting("Conflict Editor", m_conflictEditor, CSettingInfo::kBrowseType::kBrowseExecutable, "notepad.exe");
		AddSetting("Annotate Viewer", m_annotateViewer, CSettingInfo::kBrowseType::kBrowseExecutable, "notepad.exe");
		AddSetting("Python DLL", m_pythonLibrary, CSettingInfo::kBrowseType::kBrowseDll, "Python*.dll");
		AddSetting("TCL DLL", m_tclLibrary, CSettingInfo::kBrowseType::kBrowseDll, "TCL*.dll");
		break;
	case kFolders: // Folders
		{
			string defaultPattern;

			FormatDefaultSettingsLoc(defaultPattern);
			AddSetting("Settings", m_settingsFolder, CSettingInfo::kBrowseType::kBrowseFolder, defaultPattern.c_str());
			
			FormatDefaultMacrosGetLoc(defaultPattern);
			AddSetting("Macros", m_macrosFolder, CSettingInfo::kBrowseType::kBrowseFolder, defaultPattern.c_str());
			
			FormatDefaultPythonLibGetLoc(defaultPattern);
			AddSetting("PythonLib", m_pythonLibFolder, CSettingInfo::kBrowseType::kBrowseFolder, defaultPattern.c_str());
		}
		break;
	default:
		ASSERT(FALSE); // Unknown type
		break;
	}
}

/*!
	Add setting to the list
	\param name Name of the setting
	\param value Value of the setting
	\param browseType Setting value browse type
	\param defaultPattern Default pattern for the setting value
	\return Index of inserted setting on success, -1 otherwise
*/
int CGetPrefs_WINCVS::AddSetting(const char* name, const char* value, CSettingInfo::kBrowseType browseType, const char* defaultPattern /*= ""*/)
{
	const int item = m_settingsList.InsertItem(m_settingsList.GetItemCount(), name);
	if( item > -1 )
	{
		m_settingsList.SetItemText(item, WINCVS_VALUE_COLUMN, value);
		
		CSettingInfo* pSettingInfo = new CSettingInfo(browseType, defaultPattern);
		m_settingsList.SetItemData(item, (DWORD)pSettingInfo);
	}

	return item;
}

/// BN_CLICKED message handler, browse selected setting
void CGetPrefs_WINCVS::OnBrowseWincvsSettings() 
{
	const int item = m_settingsList.GetNextItem(-1, LVNI_SELECTED);
	if( item < 0 )
	{
		return;
	}

	CSettingInfo* pSettingsInfo = (CSettingInfo*)m_settingsList.GetItemData(item);
	if( !pSettingsInfo )
	{
		ASSERT(pSettingsInfo != NULL); // Invalid item data
		return;
	}

	const CString strName = m_settingsList.GetItemText(item, WINCVS_NAME_COLUMN);
	const CString strValue = m_settingsList.GetItemText(item, WINCVS_VALUE_COLUMN);

	string value = strValue;
	if( value.empty() )
	{
		value = pSettingsInfo->GetDefaultPattern().c_str();
	}

	bool valueSelected = false;

	switch( pSettingsInfo->GetBrowseType() )
	{
	case CSettingInfo::kBrowseExecutable:
		if( BrowserGetFile("Select the " + strName, kSelectExecutable, value, false) )
		{
			valueSelected = true;
		}
		break;
	case CSettingInfo::kBrowseDll:
		if( BrowserGetFile("Select the " + strName, kSelectDll, value, false) )
		{
			valueSelected = true;
		}
		break;
	case CSettingInfo::kBrowseFolder:
		if( BrowserGetDirectory("Select the " + strName + " directory", value, IsWindow(m_hWnd) ? this : NULL) )
		{
			valueSelected = true;	
		}
		break;
	default:
		ASSERT(FALSE); // Unknown type
		break;
	}

	if( valueSelected && strValue != value.c_str() )
	{
		m_isModified = true;

		m_settingsList.SetItemText(item, WINCVS_VALUE_COLUMN, value.c_str());
		EnableEditControls();
	}
}

/// BN_CLICKED message handler, clear selected setting
void CGetPrefs_WINCVS::OnClearWincvsSettings() 
{
	const int item = m_settingsList.GetNextItem(-1, LVNI_SELECTED);
	if( item > -1 )
	{
		m_isModified = true;
		
		m_settingsList.SetItemText(item, WINCVS_VALUE_COLUMN, "");
		EnableEditControls();
	}
}

/*!
	Enable or disable settings edit controls
*/
void CGetPrefs_WINCVS::EnableEditControls()
{
	const int item = m_settingsList.GetNextItem(-1, LVNI_SELECTED);
	if( item > -1 )
	{
		const CString strValue = m_settingsList.GetItemText(item, 1);
		
		m_browseButton.EnableWindow(TRUE);
		m_clearButton.EnableWindow(!strValue.IsEmpty());
	}
	else
	{
		m_browseButton.EnableWindow(FALSE);
		m_clearButton.EnableWindow(FALSE);
	}
}

/// NM_DBLCLK message handler, edit the selected setting
void CGetPrefs_WINCVS::OnDblclkWincvsSettingsList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem > -1 )
	{
		OnBrowseWincvsSettings();
	}
	
	*pResult = 0;
}

/// LVN_ITEMCHANGED message handler, enable edit controls
void CGetPrefs_WINCVS::OnItemchangedWincvsSettingsList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	EnableEditControls();
	
	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////
// CGetPrefs_CVS

IMPLEMENT_DYNCREATE(CGetPrefs_CVS, CHHelpPropertyPage)

CGetPrefs_CVS::CGetPrefs_CVS() 
	: CHHelpPropertyPage(CGetPrefs_CVS::IDD), m_isModified(false), m_showCvsCombo(CSmartComboBox::AutoDropWidth)
{
	string cvsPath;

	//{{AFX_DATA_INIT(CGetPrefs_CVS)
	m_home = gCvsPrefs.Home();
	m_useAltCvs = gCvsPrefs.WhichCvs(cvsPath, true) != 0L;
	m_cvsConsole = gCvsPrefs.CvsConsole();
	m_showCvs = gCvsPrefs.ShowCvs();
	m_altCvs = gCvsPrefs.WhichCvs(cvsPath);
	m_commandLineLimit = gCvsPrefs.CommandLineLimit();
	m_enableCommandLineLimit = gCvsPrefs.EnableCommandLineLimit() != 0;
	//}}AFX_DATA_INIT

	m_altCvsCombo.SetItems(&gPrevAltCvs);
}

CGetPrefs_CVS::~CGetPrefs_CVS()
{
}

void CGetPrefs_CVS::DoDataExchange(CDataExchange* pDX)
{
	m_isModified = true;

	CHHelpPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CGetPrefs_CVS)
	DDX_Control(pDX, IDC_ENABLE_CMDLINE_LIMIT, m_commandLineLimitCheck);
	DDX_Control(pDX, IDC_CMDLINE_LIMIT, m_commandLineLimitEdit);
	DDX_Control(pDX, IDC_ALTERNATE_CVS, m_altCvsCombo);
	DDX_Control(pDX, IDC_CONSOLE, m_cvsConsoleCheck);
	DDX_Control(pDX, IDC_SHOW_CVS_METHOD, m_showCvsCombo);
	DDX_Control(pDX, IDC_BROWSE_FILENAME, m_selAltCvsButton);
	DDX_Control(pDX, IDC_HAS_ALTERNATE_CVS, m_useAltCvsCheck);
	DDX_Control(pDX, IDC_HOME, m_homeEdit);
	DDX_Text(pDX, IDC_HOME, m_home);
	DDX_Check(pDX, IDC_HAS_ALTERNATE_CVS, m_useAltCvs);
	DDX_Check(pDX, IDC_CONSOLE, m_cvsConsole);
	DDX_CBIndex(pDX, IDC_SHOW_CVS_METHOD, m_showCvs);
	DDX_CBString(pDX, IDC_ALTERNATE_CVS, m_altCvs);
	DDX_Text(pDX, IDC_CMDLINE_LIMIT, m_commandLineLimit);
	DDV_MinMaxInt(pDX, m_commandLineLimit, 1024, 32767);
	DDX_Check(pDX, IDC_ENABLE_CMDLINE_LIMIT, m_enableCommandLineLimit);
	//}}AFX_DATA_MAP

	DDV_CheckPathExists(pDX, IDC_HOME, m_home);
	
	if( !pDX->m_bSaveAndValidate )
	{
		OnAltcvschk();
		OnEnableCmdlineLimit();
	}

	if( m_useAltCvs )
	{
		DDV_MinChars(pDX, m_altCvs, 1);
		DDX_ComboMString(pDX, IDC_ALTERNATE_CVS, m_altCvsCombo);
	}
}

BEGIN_MESSAGE_MAP(CGetPrefs_CVS, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CGetPrefs_CVS)
	ON_BN_CLICKED(IDC_BROWSE_PATH, OnBtnhome)
	ON_BN_CLICKED(IDC_HAS_ALTERNATE_CVS, OnAltcvschk)
	ON_BN_CLICKED(IDC_BROWSE_FILENAME, OnAltcvssel)
	ON_BN_CLICKED(IDC_CONSOLE, OnConsole)
	ON_BN_CLICKED(IDC_ENABLE_CMDLINE_LIMIT, OnEnableCmdlineLimit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/// OnInitDialog virtual override, initialize controls
BOOL CGetPrefs_CVS::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	EnableEditAutoComplete(m_altCvsCombo.GetEditCtrl());
	EnableEditAutoComplete(&m_homeEdit);

	m_showCvsCombo.AddString("Hidden");
	m_showCvsCombo.AddString("Normal Window");
	m_showCvsCombo.AddString("Non-active Window");
	m_showCvsCombo.AddString("Minimized non-active Window");

	m_showCvsCombo.SetCurSel(gCvsPrefs.ShowCvs());

	if( m_cvsConsole )
	{
		m_showCvsCombo.EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGetPrefs_CVS::InitHelpID()
{
	SetCtrlHelpID(IDC_HOME,					IDH_IDC_HOME);
	SetCtrlHelpID(IDC_BROWSE_PATH,			IDH_IDC_BROWSE_PATH);
	SetCtrlHelpID(IDC_HAS_ALTERNATE_CVS,	IDH_IDC_HAS_ALTERNATE_CVS);
	SetCtrlHelpID(IDC_ALTERNATE_CVS,		IDH_IDC_ALTERNATE_CVS);
	SetCtrlHelpID(IDC_BROWSE_FILENAME,		IDH_IDC_BROWSE_FILENAME);
	SetCtrlHelpID(IDC_CONSOLE,				IDH_IDC_CONSOLE);
	SetCtrlHelpID(IDC_SHOW_CVS_METHOD,		IDH_IDC_SHOW_CVS_METHOD);
	SetCtrlHelpID(IDC_ENABLE_CMDLINE_LIMIT,	IDH_IDC_ENABLE_CMDLINE_LIMIT);
	SetCtrlHelpID(IDC_CMDLINE_LIMIT,		IDH_IDC_CMDLINE_LIMIT);
}

/// BN_CLICKED message handler, enable show console combo box
void CGetPrefs_CVS::OnConsole() 
{
	m_showCvsCombo.EnableWindow(!m_cvsConsoleCheck.GetCheck());
}

/// BN_CLICKED message handler, enable command line limit entry
void CGetPrefs_CVS::OnEnableCmdlineLimit() 
{
	m_commandLineLimitEdit.EnableWindow(m_commandLineLimitCheck.GetCheck());
}

/// Save the values
void CGetPrefs_CVS::StoreValues()
{
	if( !m_isModified )
		return;

	gCvsPrefs.SetHome(m_home);
	gCvsPrefs.SetWhichCvs(m_useAltCvs && !m_altCvs.IsEmpty() ? (LPCTSTR)m_altCvs : "");
	gCvsPrefs.SetCvsConsole(m_cvsConsole != 0);
	gCvsPrefs.SetShowCvs((SHOWCVS_TYPE)m_showCvs);
	gCvsPrefs.SetEnableCommandLineLimit(m_enableCommandLineLimit != 0);
	gCvsPrefs.SetCommandLineLimit(m_commandLineLimit);
}

/// BN_CLICKED message handler, change HOME directory
void CGetPrefs_CVS::OnBtnhome()
{
	CString strHome;
    m_homeEdit.GetWindowText(strHome);
	
	string home(strHome);
	if( BrowserGetDirectory("Select the HOME directory", home, IsWindow(m_hWnd) ? this : NULL) )
	{
		m_homeEdit.SetWindowText(home.c_str());
	}
}

/// BN_CLICKED message handler, enable to change alternate CVS executable
void CGetPrefs_CVS::OnAltcvschk() 
{
	m_altCvsCombo.EnableWindow(m_useAltCvsCheck.GetCheck() != 0);
	m_selAltCvsButton.EnableWindow(m_useAltCvsCheck.GetCheck() != 0);
}

/// BN_CLICKED message handler, change alternate CVS executable
void CGetPrefs_CVS::OnAltcvssel() 
{
	CString altCvs;
	m_altCvsCombo.GetWindowText(altCvs);
	
	string fullpath = altCvs;
	if( BrowserGetFile("Select the CVS client application", kSelectExecutable, fullpath) )
	{
		m_altCvsCombo.SetWindowText(fullpath.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
// CGetPrefs_CVSCMD_DIALOGS

// Command Dialogs options page
IMPLEMENT_DYNCREATE(CGetPrefs_CVSCMD_DIALOGS, CHHelpPropertyPage)

CGetPrefs_CVSCMD_DIALOGS::CGetPrefs_CVSCMD_DIALOGS() 
	: CHHelpPropertyPage(CGetPrefs_CVSCMD_DIALOGS::IDD), m_isModified(false)
{
	//{{AFX_DATA_INIT(CGetPrefs_CVSCMD_DIALOGS)
	m_keyForce = gCvsPrefs.HideCommandDlgUseShift() != 0L ? 0 : 1;
	m_ctrlPressedOnOK = gCvsPrefs.CtrlPressedOnOK() != 0L;
	//}}AFX_DATA_INIT
}

CGetPrefs_CVSCMD_DIALOGS::~CGetPrefs_CVSCMD_DIALOGS()
{
}

void CGetPrefs_CVSCMD_DIALOGS::DoDataExchange(CDataExchange* pDX)
{
	m_isModified = true;

	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetPrefs_CVSCMD_DIALOGS)
	DDX_Control(pDX, IDC_SKIP_DIALOGS, m_skipDialogsListBox);
	DDX_Radio(pDX, IDC_FORCE_KEY_SHIFT, m_keyForce);
	DDX_Check(pDX, IDC_CTRL_PRESSED_ON_OK, m_ctrlPressedOnOK);
	//}}AFX_DATA_MAP
}

/// OnInitDialog virtual override, fill the hide options listbox
BOOL CGetPrefs_CVSCMD_DIALOGS::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	for(int nIndex = 0; nIndex < sizeof(gSkipDialogInfo) / sizeof(SkipDialogInfo); nIndex++)
	{
		const int item = m_skipDialogsListBox.AddString(gSkipDialogInfo[nIndex].m_optionName);
		if( item > -1 )
		{
			m_skipDialogsListBox.SetItemData(item, (DWORD)gSkipDialogInfo[nIndex].m_pHideOption);
			m_skipDialogsListBox.SetCheck(item, (bool)*gSkipDialogInfo[nIndex].m_pHideOption);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/*!
	OnOK virtual override, store skip dialogs settings
	\note  Other settings are stored in CGetPrefs_CVSCMD_DIALOGS::StoreValues
*/
void CGetPrefs_CVSCMD_DIALOGS::OnOK() 
{
	if( m_isModified )
	{
		const int itemCount = m_skipDialogsListBox.GetCount();
		for(int nIndex = 0; nIndex < itemCount; nIndex++)
		{
			CPersistentBool* pHideOption = (CPersistentBool*)m_skipDialogsListBox.GetItemData(nIndex);
			*pHideOption = m_skipDialogsListBox.GetCheck(nIndex) ? true : false;
		}
	}
	
	CHHelpPropertyPage::OnOK();
}

/*!
	Save the values
	\note Hide options are stored in CGetPrefs_CVSCMD_DIALOGS::OnOK because we need a listbox to be a window to retrieve the data
*/
void CGetPrefs_CVSCMD_DIALOGS::StoreValues()
{
	if( !m_isModified )
		return;

	gCvsPrefs.SetHideCommandDlgUseShift(m_keyForce == 0);
	gCvsPrefs.SetCtrlPressedOnOK(m_ctrlPressedOnOK ? true : false);
}

BEGIN_MESSAGE_MAP(CGetPrefs_CVSCMD_DIALOGS, CHHelpPropertyPage)
//{{AFX_MSG_MAP(CGetPrefs_CVSCMD_DIALOGS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CGetPrefs_CVSCMD_DIALOGS::InitHelpID()
{
	SetCtrlHelpID(IDC_SKIP_DIALOGS,			IDH_IDC_SKIP_DIALOGS);
	SetCtrlHelpID(IDC_FORCE_KEY_SHIFT,		IDH_IDC_FORCE_KEY_SHIFT);
	SetCtrlHelpID(IDC_FORCE_KEY_CTRL,		IDH_IDC_FORCE_KEY_CTRL);
	SetCtrlHelpID(IDC_CTRL_PRESSED_ON_OK,	IDH_IDC_CTRL_PRESSED_ON_OK);
}

/////////////////////////////////////////////////////////////////////////////
// CGetPrefs_General property page

IMPLEMENT_DYNCREATE(CGetPrefs_General, CHHelpPropertyPage)

CGetPrefs_General::CGetPrefs_General() : CHHelpPropertyPage(CGetPrefs_General::IDD)
{
	//{{AFX_DATA_INIT(CGetPrefs_General)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGetPrefs_General::~CGetPrefs_General()
{
}

void CGetPrefs_General::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetPrefs_General)
	DDX_Control(pDX, IDC_CVSROOT_MIGRATION_URL, m_cvsrootMigration);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetPrefs_General, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CGetPrefs_General)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetPrefs_General

BOOL CGetPrefs_General::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_cvsrootMigration.SetLink("http://cvsgui.sourceforge.net/newfaq.htm#CvsrootWizard");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGetPrefs_General::InitHelpID()
{
	SetCtrlHelpID(IDC_CVSROOT_MIGRATION_URL, helpTopicIgnore);
}

#endif //} WIN32

#if qUnix    //{ Unix-Part
class UCvsPrefs_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsPrefs_MAIN)
public:
	UCvsPrefs_MAIN() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsPrefs_MAIN() {}

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

UIMPLEMENT_DYNAMIC(UCvsPrefs_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsPrefs_MAIN, UWidget)
	ON_UCOMMAND(UCvsPrefs_MAIN::kOK, UCvsPrefs_MAIN::OnOK)
	ON_UCOMMAND(UCvsPrefs_MAIN::kCancel, UCvsPrefs_MAIN::OnCancel)
UEND_MESSAGE_MAP()

int UCvsPrefs_MAIN::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsPrefs_MAIN::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsPrefs_MAIN::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsPrefs_CVSROOT : public UWidget
{
	UDECLARE_DYNAMIC(UCvsPrefs_CVSROOT)
public:
	UCvsPrefs_CVSROOT();
	virtual ~UCvsPrefs_CVSROOT() {}

	enum
	{
		kCvsrootCombo = EV_COMMAND_START,	// 0
		kRshRadio,				// 1
		kPserverRadio,			// 2
		kLocalRadio,			// 3
		kSshRadio,				// 4
		kKerberosRadio,			// 5
		kCvsVersionCombo,		// 6
		kCvsConsoleRadio		// 7
	};

	virtual void DoDataExchange(bool fill);

	void StoreValues(void);
	
protected:
	std::string m_cvsroot;
	AuthenKind m_kind;
	bool m_console;
	int m_versCombo;

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsPrefs_CVSROOT, UWidget)

UBEGIN_MESSAGE_MAP(UCvsPrefs_CVSROOT, UWidget)
UEND_MESSAGE_MAP()

UCvsPrefs_CVSROOT::UCvsPrefs_CVSROOT() : UWidget(::UEventGetWidID())
{
	if(!gCvsPrefs.empty())
		m_cvsroot = (const char *)gCvsPrefs;
	m_kind    = gAuthen.kind();
	m_console = gCvsPrefs.CvsConsole();
	m_versCombo = gCvsPrefs.CvsVersion();
}

void UCvsPrefs_CVSROOT::StoreValues(void)
{
	gCvsPrefs = m_cvsroot.c_str();
	gAuthen.setkind(m_kind);
	gCvsPrefs.SetCvsConsole(m_console != 0);
	gCvsPrefs.SetCvsVersion(m_versCombo);
}

void UCvsPrefs_CVSROOT::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kCvsrootCombo, gOldCvsroots);
		
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kCvsrootCombo, (void *)(const char *)m_cvsroot.c_str());

		int item = kLocalRadio;
		
		switch(m_kind)
		{
		case local: item = kLocalRadio; break;
		case kserver: item = kKerberosRadio; break;
		case rhosts: item = kRshRadio; break;
		case pserver: item = kPserverRadio; break;
		case ssh: item = kSshRadio; break;
		case ntserver: break;	// not supported under *nix
		case gserver: break;	// not supported under *nix
		case server: break;		// not supported under *nix
		case sspi: break;		// not supported under *nix
		}
		
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(item, 1), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCvsConsoleRadio, m_console), 0L);
		UEventSendMessage(GetWidID(), EV_COMBO_SETSEL, UMAKEINT(kCvsVersionCombo,m_versCombo), 0L);

	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kCvsrootCombo, &m_cvsroot);

		m_kind = local;
		if(UEventSendMessage(GetWidID(), EV_QUERYSTATE, kLocalRadio, 0L))
			m_kind = local;
		else if(UEventSendMessage(GetWidID(), EV_QUERYSTATE, kKerberosRadio, 0L))
			m_kind = kserver;
		else if(UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRshRadio, 0L))
			m_kind = rhosts;
		else if(UEventSendMessage(GetWidID(), EV_QUERYSTATE, kPserverRadio, 0L))
			m_kind = pserver;
		else if(UEventSendMessage(GetWidID(), EV_QUERYSTATE, kSshRadio, 0L))
			m_kind = ssh;
		
		m_console = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCvsConsoleRadio, 0L) != 0;
		m_versCombo = UEventSendMessage(GetWidID(), EV_COMBO_GETSEL, kCvsVersionCombo, 0L);
	}
}

class UCvsPrefs_GLOBALS : public UWidget
{
	UDECLARE_DYNAMIC(UCvsPrefs_GLOBALS)
public:
	UCvsPrefs_GLOBALS();
	virtual ~UCvsPrefs_GLOBALS() {}

	enum
	{
		kcheckoutro = EV_COMMAND_START,	// 0
		kdirty,					// 1
		ktcpip,					// 2
		kprune,					// 3
		kquiet,					// 4
		kcntladd,				// 5
		kzlevel                 // 6
	};

	virtual void DoDataExchange(bool fill);
	
	void StoreValues(void);
	int  OnTcpip(void);

protected:
	bool m_checkoutro;
	bool m_dirty;
	bool m_tcpip;
	bool m_prune;
	bool m_quiet;
	bool m_cntladd;
	int  m_zlevel;

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsPrefs_GLOBALS, UWidget)

UBEGIN_MESSAGE_MAP(UCvsPrefs_GLOBALS, UWidget)
	ON_UCOMMAND(UCvsPrefs_GLOBALS::ktcpip, UCvsPrefs_GLOBALS::OnTcpip)
UEND_MESSAGE_MAP()

UCvsPrefs_GLOBALS::UCvsPrefs_GLOBALS() : UWidget(::UEventGetWidID())
{
	m_checkoutro = gCvsPrefs.CheckoutRO();
	m_prune      = gCvsPrefs.PruneOption();
	m_tcpip      = gCvsPrefs.Z9Option();
	m_quiet      = gCvsPrefs.QuietOption();
	m_cntladd    = gCvsPrefs.AddControl();
	m_dirty      = gCvsPrefs.DirtySupport();
	m_zlevel     = gCvsPrefs.ZLevel();
}

void UCvsPrefs_GLOBALS::StoreValues(void)
{
	gCvsPrefs.SetCheckoutRO(m_checkoutro);
	gCvsPrefs.SetPruneOption(m_prune);
	gCvsPrefs.SetZ9Option(m_tcpip);
	gCvsPrefs.SetQuietOption(m_quiet);
	gCvsPrefs.SetAddControl(m_cntladd);
	gCvsPrefs.SetDirtySupport(m_dirty);
	gCvsPrefs.SetZLevel(m_zlevel);
}

void UCvsPrefs_GLOBALS::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kcheckoutro, m_checkoutro), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kprune,      m_prune),      0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(ktcpip,      m_tcpip),      0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kquiet,      m_quiet),      0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kcntladd,    m_cntladd),    0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kdirty,      m_dirty),      0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kzlevel,     m_zlevel),     0L);
	}
	else
	{
		m_checkoutro = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kcheckoutro, 0L) != 0;
		m_prune      = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kprune,      0L) != 0;
		m_tcpip      = UEventSendMessage(GetWidID(), EV_QUERYSTATE, ktcpip,      0L) != 0;
		m_quiet      = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kquiet,      0L) != 0;
		m_cntladd    = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kcntladd,    0L) != 0;
		m_dirty      = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kdirty,      0L) != 0;
		m_zlevel     = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kzlevel,     0L);
	}
}


int UCvsPrefs_GLOBALS::OnTcpip(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, ktcpip, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kzlevel, state), 0L);
	return 0;
}

class UCvsPrefs_PORTS : public UWidget
{
	UDECLARE_DYNAMIC(UCvsPrefs_PORTS)
public:
	UCvsPrefs_PORTS();
	virtual ~UCvsPrefs_PORTS() {}

	enum
	{
		kcheckrhost = EV_COMMAND_START,	// 0
		kcheckpserver,			// 1
		kcheckkserver,			// 2
		kchecksname,			// 3
		kcheckrsh,				// 4

		kportrhost,				// 5
		kportpserver,			// 6
		kportkserver,			// 7

		ksname,					// 8
		krshname				// 9
	};

	virtual void DoDataExchange(bool fill);
	
	void StoreValues(void);

protected:
	bool m_checkrhost;
	bool m_checkpserver;
	bool m_checkkserver;
	bool m_checksname;
	bool m_checkrsh;
	int m_portrhost;
	int m_portpserver;
	int m_portkserver;
	std::string m_sname;
	std::string m_rshname;

	ev_msg int OnCheckPorts(int cmd);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsPrefs_PORTS, UWidget)

UBEGIN_MESSAGE_MAP(UCvsPrefs_PORTS, UWidget)
	ON_UCOMMAND_RANGE(UCvsPrefs_PORTS::kcheckrhost, UCvsPrefs_PORTS::kcheckrsh, UCvsPrefs_PORTS::OnCheckPorts)
UEND_MESSAGE_MAP()

UCvsPrefs_PORTS::UCvsPrefs_PORTS() : UWidget(::UEventGetWidID())
{
	m_checkkserver = gCvsPrefs.KserverPort() != -1;
	m_checkpserver = gCvsPrefs.PserverPort() != -1;
	m_checkrhost = gCvsPrefs.RhostPort() != -1;
	m_checksname = gCvsPrefs.ServerName() != 0L;
	m_checkrsh = gCvsPrefs.RshName() != 0L;
		
	m_portkserver = gCvsPrefs.KserverPort() != -1 ? gCvsPrefs.KserverPort() : 0;
	m_portpserver = gCvsPrefs.PserverPort() != -1 ? gCvsPrefs.PserverPort() : 0;
	m_portrhost = gCvsPrefs.RhostPort() != -1 ? gCvsPrefs.RhostPort() : 0;
	m_sname = gCvsPrefs.ServerName() == 0L ? "cvs" : gCvsPrefs.ServerName();
	m_rshname = gCvsPrefs.RshName() == 0L ? "ssh" : gCvsPrefs.RshName();
}

void UCvsPrefs_PORTS::StoreValues(void)
{
	gCvsPrefs.SetKserverPort(m_checkkserver ? m_portkserver : -1);
	gCvsPrefs.SetPserverPort(m_checkpserver ? m_portpserver : -1);
	gCvsPrefs.SetRhostPort(m_checkrhost ? m_portrhost : -1);
	
	gCvsPrefs.SetServerName(m_checksname ? (const char *)m_sname.c_str() : 0L);
	gCvsPrefs.SetRshName(m_checkrsh ? (const char *)m_rshname.c_str() : 0L);
}

int UCvsPrefs_PORTS::OnCheckPorts(int cmd)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, cmd, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(cmd + (kportrhost-kcheckrhost), state), 0L);
	
	if( cmd == kcheckrhost ) {
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kchecksname, state), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kcheckrsh, state), 0L);
	}
	
	return 0;
}

void UCvsPrefs_PORTS::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kcheckkserver, m_checkkserver), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kcheckpserver, m_checkpserver), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kcheckrhost, m_checkrhost), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kchecksname, m_checksname), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kcheckrsh, m_checkrsh), 0L);
		
		UEventSendMessage(GetWidID(), EV_SETINTEGER, UMAKEINT(kportkserver, m_portkserver), 0L);
		UEventSendMessage(GetWidID(), EV_SETINTEGER, UMAKEINT(kportpserver, m_portpserver), 0L);
		UEventSendMessage(GetWidID(), EV_SETINTEGER, UMAKEINT(kportrhost, m_portrhost), 0L);
		UEventSendMessage(GetWidID(), EV_SETTEXT, ksname, (void *)(const char *)m_sname.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, krshname, (void *)(const char *)m_rshname.c_str());

		OnCheckPorts(kcheckrhost);
		OnCheckPorts(kcheckpserver);
		OnCheckPorts(kcheckkserver);
		OnCheckPorts(kchecksname);
		OnCheckPorts(kcheckrsh);
	}
	else
	{
		m_checkkserver = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kcheckkserver, 0L) != 0;
		m_portkserver = UEventSendMessage(GetWidID(), EV_GETINTEGER, kportkserver, 0L);
		m_checkpserver = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kcheckpserver, 0L) != 0;
		m_portpserver = UEventSendMessage(GetWidID(), EV_GETINTEGER, kportpserver, 0L);
		m_checkrhost = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kcheckrhost, 0L) != 0;
		m_portrhost = UEventSendMessage(GetWidID(), EV_GETINTEGER, kportrhost, 0L);

		UEventSendMessage(GetWidID(), EV_GETTEXT, ksname, &m_sname);
		m_checksname = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kchecksname, 0L) != 0;
		UEventSendMessage(GetWidID(), EV_GETTEXT, krshname, &m_rshname);
		m_checkrsh = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kcheckrsh, 0L) != 0;
	}
}

class UCvsPrefs_PROXY : public UWidget
{
	UDECLARE_DYNAMIC(UCvsPrefs_PROXY)
public:
	UCvsPrefs_PROXY();
	virtual ~UCvsPrefs_PROXY() {}

	enum
	{
		kuseproxy = EV_COMMAND_START,	// 0
		kproxyhost,				// 1
		kproxyport				// 2
	};

	virtual void DoDataExchange(bool fill);
	
	void StoreValues(void);

protected:
	bool m_useproxy;
	std::string m_proxyhost;
	int m_proxyport;

	ev_msg int OnUseProxy(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsPrefs_PROXY, UWidget)

UBEGIN_MESSAGE_MAP(UCvsPrefs_PROXY, UWidget)
	ON_UCOMMAND(UCvsPrefs_PROXY::kuseproxy, UCvsPrefs_PROXY::OnUseProxy)
UEND_MESSAGE_MAP()

UCvsPrefs_PROXY::UCvsPrefs_PROXY() : UWidget(::UEventGetWidID())
{
	m_useproxy = gCvsPrefs.UseProxy();
	m_proxyport = gCvsPrefs.ProxyPort();
	m_proxyhost = gCvsPrefs.ProxyHost();
}

void UCvsPrefs_PROXY::StoreValues(void)
{
	gCvsPrefs.SetUseProxy(m_useproxy);
	gCvsPrefs.SetProxyHost(m_proxyhost.c_str());
	gCvsPrefs.SetProxyPort(m_proxyport);
}

int UCvsPrefs_PROXY::OnUseProxy(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kuseproxy, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kproxyhost, state), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kproxyport, state), 0L);
	return 0;
}

void UCvsPrefs_PROXY::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kuseproxy, m_useproxy), 0L);
		UEventSendMessage(GetWidID(), EV_SETINTEGER, UMAKEINT(kproxyport, m_proxyport), 0L);
		UEventSendMessage(GetWidID(), EV_SETTEXT, kproxyhost, (void *)(const char *)m_proxyhost.c_str());

		OnUseProxy();
	}
	else
	{
		std::string str;
		m_useproxy = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kuseproxy, 0L) != 0;
		UEventSendMessage(GetWidID(), EV_GETTEXT, kproxyhost, &m_proxyhost);
		m_proxyport = UEventSendMessage(GetWidID(), EV_GETINTEGER, kproxyport, 0L);
	}
}

UIMPLEMENT_DYNAMIC(UCvsPrefs_GCVS, UWidget)

UBEGIN_MESSAGE_MAP(UCvsPrefs_GCVS, UWidget)
	ON_UCOMMAND_RANGE(UCvsPrefs_GCVS::kdiffBtn, UCvsPrefs_GCVS::kcvsBtn, UCvsPrefs_GCVS::OnPickProgram)
	ON_UCOMMAND(UCvsPrefs_GCVS::kuseAltCvs, UCvsPrefs_GCVS::OnUseAlternateCvs)
	ON_UCOMMAND(UCvsPrefs_GCVS::kconsoleFontBtn, UCvsPrefs_GCVS::OnSelectFont)
UEND_MESSAGE_MAP()

UCvsPrefs_GCVS::UCvsPrefs_GCVS() : UWidget(::UEventGetWidID())
{
	std::string cvsPath;
	m_diffApp     = gCvsPrefs.ExtDiff();
	m_viewerApp   = gCvsPrefs.Viewer();
	m_browserApp  = gCvsPrefs.Browser();
	m_helpApp     = gCvsPrefs.Helper();
	m_useAltCvs   = gCvsPrefs.UseAltCvs();
	m_cvsApp      = gCvsPrefs.WhichCvs(cvsPath);
	m_consoleFont = gCvsPrefs.ConsoleFont();
}

bool UCvsPrefs_GCVS::checkCVS(const char *filename)
{
	CvsArgs args;
	int status;
	
	cvs_out("checking cvs\n");
	
	args.add("-v");
	
	status = testCVS(filename, args.Argc(), args.Argv());
	switch( status ) {
		case 0:
			break; // all OK
		case errNoCvsguiProtocol: 
			cvs_err( "actual cvs binary has no support for GUI-pipes.\n");
			cvs_err( "Please select another binary and check the 'alternate cvs' settings!\n\n");
			break;
		default:
			break;
	}
	
	return ( status == 0 ? true : false );
}
void UCvsPrefs_GCVS::StoreValues(void)
{
	gCvsPrefs.SetExtDiff(m_diffApp.c_str());
	gCvsPrefs.SetViewer(m_viewerApp.c_str());
	gCvsPrefs.SetBrowser(m_browserApp.c_str());
	gCvsPrefs.SetHelper(m_helpApp.c_str());
	gCvsPrefs.SetUseAltCvs(m_useAltCvs);
	
#ifdef qUnix
	gCvsPrefs.SetConsoleFont(m_consoleFont.c_str());
	
	// use selected cvs only if it supports '-cvsgui'
	if( m_useAltCvs ) {
		if( checkCVS( m_cvsApp.c_str() ) ) {
			gCvsPrefs.SetWhichCvs( m_cvsApp.c_str() );
		}
	} else {
		std::string cvscmd;
	
		// check if actual cvs binary supports cvsgui protocol
		checkCVS( gCvsPrefs.WhichCvs(cvscmd, m_useAltCvs) );
	}
#else
	gCvsPrefs.SetWhichCvs(m_cvsApp);
#endif
}

#ifdef qGTK  //{ GTK - Part	
static std::string gtksel;
static bool gtkModalLoop;

static gint gtkcancel(GtkWidget *widget, gpointer data)
{
	gtkModalLoop = false;
	return TRUE;
}

static void font_ok_sel (GtkWidget *w, gpointer data)
{
	gtkModalLoop = false;
	gtksel = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG (data));
}

static void font_cancel_sel (GtkWidget *w, gpointer data)
{
	gtkModalLoop = false;
}

int UCvsPrefs_GCVS::OnSelectFont(void)
{
	/* Create a new file selection widget */
	GtkWidget* fontw = gtk_font_selection_dialog_new ("choice console font");
	
	gtk_signal_connect(GTK_OBJECT(fontw), "delete_event",
					   GTK_SIGNAL_FUNC(gtkcancel), 0L);

	/* Connect the ok_button to file_ok_sel function */
	gtk_signal_connect(GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (fontw)->ok_button),
					  "clicked", (GtkSignalFunc) font_ok_sel, fontw);
	gtk_signal_connect(GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (fontw)->cancel_button),
					  "clicked", (GtkSignalFunc) font_cancel_sel, fontw);
	
	gtkModalLoop = true;
	gtksel = "";
	gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG (fontw), m_consoleFont.c_str());
	gtk_widget_show(fontw);
	gtk_grab_add(fontw);

	while( gtkModalLoop )
	{
		gtk_main_iteration_do(TRUE);
	}

	gtk_grab_remove (fontw);
	
	gtk_widget_destroy(fontw);

	if( !gtksel.empty() )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kconsoleFont, (void*)(const char*)gtksel.c_str());
	}
	
	return 0;
}
#endif /* qGTK */

int UCvsPrefs_GCVS::OnPickProgram(int cmd)
{
	std::string appPath;
	UEventSendMessage(GetWidID(), EV_GETTEXT, cmd - kdiffBtn + kdiffApp, &appPath);
	
	MultiFiles mf;
	if( !appPath.empty() )
	{
		std::string uppath;
		std::string file;

		if( SplitPath(appPath.c_str(), uppath, file) )
		{
			mf.newdir(uppath.c_str());
			mf.newfile(file.c_str());
		}
	}

	if( !BrowserGetMultiFiles("Select the application", mf, true, kSelectExecutable) )
		return 0;

	CvsArgs args(false);
	const char* dir = (*mf.begin()).add(args);
	char* const* argv = args.Argv();
	std::string fullpath;
	fullpath = dir;
	NormalizeFolderPath(fullpath);

	fullpath += argv[0];
	
#ifdef qUnix
	// use selected cvs only if it supports '-cvsgui'
	if( ( cmd == kcvsBtn ) && !checkCVS( fullpath.c_str() ) )
		return 0;
#endif
	
	UEventSendMessage(GetWidID(), EV_SETTEXT, cmd - kdiffBtn + kdiffApp, (void*)(const char*)fullpath.c_str());
	
	return 0;
}

int UCvsPrefs_GCVS::OnUseAlternateCvs(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kuseAltCvs, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kcvsApp, state), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kcvsBtn, state), 0L);
	return 0;
}

void UCvsPrefs_GCVS::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kdiffApp, (void *)(const char *)m_diffApp.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kviewerApp, (void *)(const char *)m_viewerApp.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kbrowserApp, (void *)(const char *)m_browserApp.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, khelpApp, (void *)(const char *)m_helpApp.c_str());
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kuseAltCvs, m_useAltCvs), 0L);
		UEventSendMessage(GetWidID(), EV_SETTEXT, kconsoleFont, (void *)(const char *)m_consoleFont.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kcvsApp, (void *)(const char *)m_cvsApp.c_str());
		OnUseAlternateCvs();
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kdiffApp, &m_diffApp);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kviewerApp, &m_viewerApp);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kbrowserApp, &m_browserApp);
		UEventSendMessage(GetWidID(), EV_GETTEXT, khelpApp, &m_helpApp);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kconsoleFont, &m_consoleFont);
		m_useAltCvs = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kuseAltCvs, 0L) != 0;
		UEventSendMessage(GetWidID(), EV_GETTEXT, kcvsApp, &m_cvsApp);
	}
}

#endif //} qUnix

#if qMacCvsPP   //{ Mac-Part

class UCvsPrefs_CVSROOT : public UWidget
{
	UDECLARE_DYNAMIC(UCvsPrefs_CVSROOT)
public:
	UCvsPrefs_CVSROOT();
	virtual ~UCvsPrefs_CVSROOT() {}

	enum
	{
		kAuthenPopup = EV_COMMAND_START + 40,	// 40
		kPath,				// 41
		kHost,				// 42
		kUser,				// 43
		kRootPopup,			// 44
		kRootStat,			// 45
		kCvsVers,			// 46
		kAuthenSettings,	// 47
		kAuthenDesc			// 48
	};

	virtual void DoDataExchange(bool fill);
	
protected:
	std::string m_cvsroot;
	AuthenKind m_kind;

	ev_msg void OnAuthenPopup(int pos, const char *txt);
	ev_msg void OnRootPopup(int pos, const char *txt);
	ev_msg void OnEditChange(int cmd);
	ev_msg int OnAuthenSettings(void);
	ev_msg void OnDestroy(void);
	
	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsPrefs_CVSROOT, UWidget)

UBEGIN_MESSAGE_MAP(UCvsPrefs_CVSROOT, UWidget)
	ON_COMBO_SEL(kAuthenPopup, UCvsPrefs_CVSROOT::OnAuthenPopup)
	ON_COMBO_SEL(kRootPopup, UCvsPrefs_CVSROOT::OnRootPopup)
	ON_EDITCHANGE_RANGE(kPath, kUser, UCvsPrefs_CVSROOT::OnEditChange)
	ON_UCOMMAND(kAuthenSettings, UCvsPrefs_CVSROOT::OnAuthenSettings)
	ON_UDESTROY(UCvsPrefs_CVSROOT)
UEND_MESSAGE_MAP()

UCvsPrefs_CVSROOT::UCvsPrefs_CVSROOT() : UWidget(::UEventGetWidID())
{
}

void UCvsPrefs_CVSROOT::OnAuthenPopup(int pos, const char *txt)
{
	m_kind = Authen::numToKind(pos);
	AuthenModel *model = AuthenModel::GetInstance(m_kind);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kHost, model->HasHost()), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kUser, model->HasUser()), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kAuthenSettings, model->HasSettings()), 0L);
	
	if(!model->HasHost())
		UEventSendMessage(GetWidID(), EV_SETTEXT, kHost, (void *)"");
	if(!model->HasUser())
		UEventSendMessage(GetWidID(), EV_SETTEXT, kUser, (void *)"");
	std::string desc;
	model->GetSettingsDesc(desc);
	UEventSendMessage(GetWidID(), EV_SETTEXT, kAuthenDesc, (void *)(const char *)desc);
}

void UCvsPrefs_CVSROOT::OnRootPopup(int pos, const char *txt)
{
	m_cvsroot = txt;
	UEventSendMessage(GetWidID(), EV_SETTEXT, kRootStat, (void *)txt);

	std::string theMethod, theUser, theHost, thePath;
	
	if(Authen::parse_cvsroot(txt, theMethod, theUser, theHost, thePath))
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kPath, (void *)(const char *)thePath);
		UEventSendMessage(GetWidID(), EV_SETTEXT, kHost, (void *)(const char *)theHost);
		UEventSendMessage(GetWidID(), EV_SETTEXT, kUser, (void *)(const char *)theUser);
		if (theMethod.length () > 0)
			UEventSendMessage(GetWidID(), EV_COMBO_SETSEL,
							  UMAKEINT(kAuthenPopup, Authen::kindToNum(Authen::tokenToKind (theMethod.c_str ()))), 0L);
	}
}

void UCvsPrefs_CVSROOT::OnEditChange(int cmd)
{
	std::string theUser, theHost, thePath;

	UEventSendMessage(GetWidID(), EV_GETTEXT, kPath, &thePath);
	UEventSendMessage(GetWidID(), EV_GETTEXT, kHost, &theHost);
	UEventSendMessage(GetWidID(), EV_GETTEXT, kUser, &theUser);
	
	m_cvsroot = "";
	AuthenModel *model = AuthenModel::GetInstance(m_kind);
	if(model->HasUser())
		m_cvsroot << theUser;
	if(model->HasHost())
	{
		m_cvsroot << '@';
		m_cvsroot << theHost;
	}
	if(model->HasUser() || model->HasHost())
		m_cvsroot << ':';
	m_cvsroot << thePath;
	UEventSendMessage(GetWidID(), EV_SETTEXT, kRootStat, (void *)(const char *)m_cvsroot);
}

int UCvsPrefs_CVSROOT::OnAuthenSettings(void)
{
	AuthenModel *model = AuthenModel::GetInstance(m_kind);
	model->DoSettings();
	std::string desc;
	model->GetSettingsDesc(desc);
	UEventSendMessage(GetWidID(), EV_SETTEXT, kAuthenDesc, (void *)(const char *)desc);
	return 0;
}

void UCvsPrefs_CVSROOT::OnDestroy(void)
{
	delete this;
}

void UCvsPrefs_CVSROOT::DoDataExchange(bool fill)
{
	if(fill)
	{		
		if(!gCvsPrefs.empty())
			m_cvsroot = (const char *)gCvsPrefs;
		m_kind = gAuthen.kind();

		UEventSendMessage(GetWidID(), EV_COMBO_RESETALL, kAuthenPopup, 0L);
		std::vector<AuthenModel *> & allAuthen = AuthenModel::GetAllInstances();
		std::vector<AuthenModel *>::iterator i;
		for(i = allAuthen.begin(); i != allAuthen.end(); ++i)
		{
			UEventSendMessage(GetWidID(), EV_COMBO_APPEND,
							  kAuthenPopup, (void *)(*i)->GetToken());
		}
		
		AuthenModel *model = AuthenModel::GetInstance(m_kind);
		UEventSendMessage(GetWidID(), EV_COMBO_SETSEL,
						  UMAKEINT(kAuthenPopup, Authen::kindToNum(model->GetKind())), 0L);
		
		UEventSendMessage(GetWidID(), EV_COMBO_RESETALL, kRootPopup, 0L);

		std::vector<CStr>::const_iterator haley;
		const std::vector<CStr> & alex = gOldCvsroots.GetList();
		for(haley = alex.begin(); haley != alex.end(); ++haley)
		{
			if(!(*haley).empty())
				UEventSendMessage(GetWidID(), EV_COMBO_APPEND,
								  kRootPopup, (void *)(*haley).c_str());
		}
		
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCvsVers, 1), 0L);

		std::string tmp(m_cvsroot);
		OnRootPopup(0, tmp);
		OnAuthenPopup(Authen::kindToNum(m_kind), 0L);

		UEventSendMessage(GetWidID(), EV_CHGFOCUS, kPath, 0L);
	}
	else
	{
		gAuthen.setkind(m_kind);

		std::string theUser, theHost, thePath;

		UEventSendMessage(GetWidID(), EV_GETTEXT, kPath, &thePath);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kHost, &theHost);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kUser, &theUser);

		std::string root;
		if(m_kind != local)
		{
			root << theUser;
			root << '@';
			root << theHost;
			root << ':';
			root << thePath;
		}
		else
			root = thePath;
		
		gCvsPrefs = root;
		
		if( !root.empty() )
			gOldCvsroots.Insert(root);
	}
}

static void DoDataExchange_Cvsroot(UCvsPrefs_CVSROOT *handler, bool putValue)
{
	handler->DoDataExchange(putValue);
}
#endif //} qMacCvsPP

/// Get the CVSROOT, authentication, and misc options
void CompatGetPrefs(void)
{
	bool userHitOK = false;

	// Store current CVSROOT
#if INTERNAL_AUTHEN
	const std::string prevCvsroot((const char*)gCvsPrefs);
	const AuthenKind prevKind = gAuthen.kind();
#endif /* INTERNAL_AUTHEN */

#ifdef WIN32   //{ WIN32
	CHHelpPropertySheet pages("WinCvs Preferences");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CGetPrefs_GLOBALS page1;
	CGetPrefs_CVS page2;
	CGetPrefs_WINCVS page3;
	CGetPrefs_CVSCMD_DIALOGS page4;
	CGetPrefs_General page5;

	pages.AddPage(&page1);
	pages.AddPage(&page2);
	pages.AddPage(&page3);
	pages.AddPage(&page4);
	pages.AddPage(&page5);

	if( pages.DoModal() == IDOK )
	{
		page1.StoreValues();
		page2.StoreValues();
		page3.StoreValues();
		page4.StoreValues();

		userHitOK = true;
	}
#endif //} WIN32 

#ifdef qMacCvsPP  //{ qMacCvsPP
	MacBinMaps maps;
	maps.LoadDefaults();
	CMacBinTable::SetCurrentMap(&maps);

	StDialogHandler	theHandler(dlg_Prefs, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	static UInt16 sRuntimePanel = 1;
	
	LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
		(theDialog->FindPaneByID(item_MultiViewPrefs));
	LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
	
	UCvsPrefs_CVSROOT *handler = new UCvsPrefs_CVSROOT;
	ThrowIfNil_(handler);
	
	UEventSendMessage(handler->GetWidID(), EV_INIT_WIDGET, 0, theDialog);
	
	multiView->SwitchToPanel(5);
	DoDataExchange_Fonts(theDialog, true);
	multiView->SwitchToPanel(4);
	DoDataExchange_BinFiles(theDialog, maps, true);
	multiView->SwitchToPanel(3);
	DoDataExchange_TextFiles(theDialog, true);
	LPushButton *diffbtn = dynamic_cast<LPushButton*>(theDialog->FindPaneByID(item_BrowseExtDiff));
	diffbtn->AddListener(&theHandler);
	multiView->SwitchToPanel(2);
	DoDataExchange_Globals(theDialog, true);
	multiView->SwitchToPanel(1);
	DoDataExchange_Cvsroot(handler, true);
	
	groupView->SetValue(sRuntimePanel);
	theDialog->Show();
	MessageT hitMessage;
	SInt32    lastDiffToolValue(0);
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;

    HandleHitMessage_DiffTool(theDialog, hitMessage);
	}
	theDialog->Hide();
	sRuntimePanel = groupView->GetValue();
	
	if(hitMessage == msg_OK)
	{
		userHitOK = true;

		multiView->SwitchToPanel(1);
		DoDataExchange_Cvsroot(handler, false);
		multiView->SwitchToPanel(2);
		DoDataExchange_Globals(theDialog, false);
		multiView->SwitchToPanel(3);
		DoDataExchange_TextFiles(theDialog, false);
		multiView->SwitchToPanel(4);
		DoDataExchange_BinFiles(theDialog, maps, false);
		multiView->SwitchToPanel(5);
		DoDataExchange_Fonts(theDialog, false);

		maps.SaveDefaults();
	}
#endif //} qMacCvsPP

#if qUnix  //{ Unix-Part
	void *wid = UCreate_gcvs_Settings();

	UCvsPrefs_MAIN *dlg = new UCvsPrefs_MAIN();
	UCvsPrefs_CVSROOT *tab1 = new UCvsPrefs_CVSROOT();
	UCvsPrefs_GLOBALS *tab2 = new UCvsPrefs_GLOBALS();
	UCvsPrefs_PORTS *tab3 = new UCvsPrefs_PORTS();
	UCvsPrefs_PROXY *tab4 = new UCvsPrefs_PROXY();
	UCvsPrefs_GCVS *tab5 = new UCvsPrefs_GCVS();
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);
	dlg->AddPage(tab1, UCvsPrefs_MAIN::kTabGeneral, 0);
	dlg->AddPage(tab2, UCvsPrefs_MAIN::kTabGeneral, 1);
	dlg->AddPage(tab3, UCvsPrefs_MAIN::kTabGeneral, 2);
	dlg->AddPage(tab4, UCvsPrefs_MAIN::kTabGeneral, 3);
	dlg->AddPage(tab5, UCvsPrefs_MAIN::kTabGeneral, 4);

	if(dlg->DoModal())
	{
		tab1->StoreValues();
		tab2->StoreValues();
		tab3->StoreValues();
		tab4->StoreValues();
		tab5->StoreValues();

		userHitOK = true;
	}
	delete dlg;
#endif //} qUnix

	if( userHitOK 
#if INTERNAL_AUTHEN
		&& !gCvsPrefs.empty() 
#endif /* INTERNAL_AUTHEN */
		)
	{
#if INTERNAL_AUTHEN
		if( prevCvsroot != (const char*)gCvsPrefs || prevKind != gAuthen.kind() )
		{
			cvs_out("NEW CVSROOT: %s (%s)\n", (const char*)gCvsPrefs, gAuthen.kindstr());
			cvs_err("CVSROOT change will only take effect when commands are run from a non-CVS folder.\n"
				"To change the CVSROOT of an existing sandbox please use the ChangeRoot macro.\n");
		}
		else
		{
			cvs_out("CVSROOT: %s (%s)\n", (const char*)gCvsPrefs, gAuthen.kindstr());
		}
#endif /* INTERNAL_AUTHEN */

		gCvsPrefs.save();
	}
}
