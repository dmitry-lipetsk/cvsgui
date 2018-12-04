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

// LineCmd.cpp : implementation file
//

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LCheckBoxGroupBox.h>
#	include <LTextEditView.h>
#	include <LPushButton.h>
#	include <LMultiPanelView.h>
#	include <LPopupGroupBox.h>
#	include <LCheckBox.h>
#	include <LEditText.h>

#	include "MacCvsConstant.h"
# include "MacMisc.h"
#endif /* qMacCvsPP */

#include "LineCmd.h"
#include "MultiString.h"
#include "CvsArgs.h"
#include "PromptFiles.h"
#include "CvsCommands.h"
#include "MultiFiles.h"
#include "ItemListDlg.h"
#include "CvsRootDlg.h"
#include "GetPrefs.h"
#include "CvsPrefs.h"

#if qUnix
#	include "UCvsDialogs.h"
#endif

using namespace std;

/// Default previous commands
char* sDefPrevCvsCmd[] = 
{
	"cvs --help-options",
	"cvs --help-commands",
	"cvs --help-synonyms",
	"cvs -H <cvs_command>",
	0L
};

static CMString gPrevCvsCmd(20, "P_PrevCvsCmd", sDefPrevCvsCmd);
static CMString gPrevCvsCmdDir(20, "P_PrevCvsCmdDir");

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#ifdef qMacCvsPP
static void DoDataExchange_CmdlineMain(LWindow *theDialog, std::string  & cmdline, std::string  & path, bool & hasPath, bool & addDefault, bool & addSelection, bool putValue)
{
	LCheckBox *cbox = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckGroupPath));
	LCheckBox *addDefault_cbox = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_AddDefault));
	LCheckBox *addSelection_cbox = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_AddSelection));
	LEditText *epath = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_ShowPath));
	LTextEditView *cline = dynamic_cast<LTextEditView*>
		(theDialog->FindPaneByID(item_NonEmptyTextEdit));
	
	if(putValue)
	{
	  std::string     sysEncodedCmdLine(UTF8_to_SysEncoding(cmdline));
		cline->SetTextPtr((Ptr)(const char *)sysEncodedCmdLine, sysEncodedCmdLine.length());
		cbox->SetValue(hasPath ? Button_On : Button_Off);
		epath->SetDescriptor(UTF8_to_LString(path));
		addDefault_cbox->SetValue(addDefault ? Button_On : Button_Off);
		addSelection_cbox->SetValue(addSelection ? Button_On : Button_Off);
		
		// this will take care about an empty module name
		theDialog->SetLatentSub(cline);
		//cline->AddListener(LCommander::GetTopCommander());
		if(cmdline.length() == 0)
		{
			LPushButton *theOK = dynamic_cast<LPushButton*>
										(theDialog->FindPaneByID(item_OK));
			Assert_(theOK != 0L);
			theOK->Disable();
		}
	}
	else
	{
		Handle hdl = cline->GetTextHandle();
		Assert_(hdl != 0L);
		std::string  str;
		str.set(*hdl, GetHandleSize(hdl));
		cmdline = SysEncoding_to_UTF8(str);
		
		LStr255 outDescriptor;
		epath->GetDescriptor(outDescriptor);
		path = LString_to_UTF8(outDescriptor);
		hasPath = cbox->GetValue() == Button_On;
		addDefault = addDefault_cbox->GetValue() == Button_On;
		addSelection = addSelection_cbox->GetValue() == Button_On;
	}
}
#endif /* qMacCvsPP */

#ifdef WIN32
IMPLEMENT_DYNAMIC(CCmdLine_MAIN, CHHelpPropertyPage)

CCmdLine_MAIN::CCmdLine_MAIN(const MultiFiles* mf, 
							 const char* lastcmd, bool hasPath, const char* lastpath,
							 bool addDefault, bool addSelection, 
							 bool forceRoot, const char* cvsroot) 
							 : m_mf(mf), 
							 m_multilineEntryDlg(&m_cvsrootEntryDlg, mf), 
							 CHHelpPropertyPage(CCmdLine_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CCmdLine_MAIN)
	m_checkPath = hasPath;
	m_folderLoc = lastpath;
	m_addDefault = addDefault;
	m_addSelection = addSelection;
	//}}AFX_DATA_INIT

	m_forceRoot = forceRoot;
	m_cvsroot = cvsroot;

	m_cmdLine = lastcmd;

	m_hasSelection = m_mf->NumDirs() > 0;

	m_folderLocCombo.SetItems(&gPrevCvsCmdDir);
}

CCmdLine_MAIN::~CCmdLine_MAIN()
{
}

void CCmdLine_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdLine_MAIN)
	DDX_Control(pDX, IDC_CVSROOT_ENTRY_STATIC, m_cvsrootEntryPlaceholder);
	DDX_Control(pDX, IDC_MULTILINE_ENTRY_STATIC, m_multilineEntryPlaceholder);
	DDX_Control(pDX, IDC_PATH, m_folderLocCombo);
	DDX_Control(pDX, IDC_ADD_SELECTION, m_addSelectionCheck);
	DDX_Control(pDX, IDC_HAS_PATH, m_checkPathCheck);
	DDX_Control(pDX, IDC_BROWSE_PATH, m_selFolderButton);
	DDX_Check(pDX, IDC_HAS_PATH, m_checkPath);
	DDX_Text(pDX, IDC_PATH, m_folderLoc);
	DDX_Check(pDX, IDC_ADD_DEFAULT, m_addDefault);
	DDX_Check(pDX, IDC_ADD_SELECTION, m_addSelection);
	//}}AFX_DATA_MAP
	
	DDX_ComboMString(pDX, IDC_PATH, m_folderLocCombo);

	if( m_checkPath )
	{
		DDV_MinChars(pDX, m_folderLoc, 1);
		DDV_CheckPathExists(pDX, IDC_PATH, m_folderLoc);
	}

	if( !pDX->m_bSaveAndValidate )
	{
		m_selFolderButton.EnableWindow(m_checkPath);
	}
	
	m_multilineEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_cmdLine);
	m_cvsrootEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_forceRoot, m_cvsroot);
}


BEGIN_MESSAGE_MAP(CCmdLine_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CCmdLine_MAIN)
	ON_BN_CLICKED(IDC_HAS_PATH, OnCheckpath)
	ON_BN_CLICKED(IDC_BROWSE_PATH, OnSelfolder)
	ON_BN_CLICKED(IDC_ADD_SELECTION, OnAddSelection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdLine_MAIN message handlers

/// BN_CLICKED message handler, enable controls
void CCmdLine_MAIN::OnCheckpath() 
{
	const BOOL enable = m_checkPathCheck.GetCheck();

	m_selFolderButton.EnableWindow(enable);
	m_folderLocCombo.EnableWindow(enable);
	m_addSelectionCheck.EnableWindow(!enable && m_hasSelection);

	if( enable && m_hasSelection && m_mf )
	{
		CString strFolderLoc;
		m_folderLocCombo.GetWindowText(strFolderLoc);
		
		if( strFolderLoc.IsEmpty() )
		{
			string path;
			if( m_mf->getdir(0, path) )
			{
				m_folderLocCombo.SetWindowText(path.c_str());
			}
		}
	}
}

/// BN_CLICKED message handler, select folder to execute command
void CCmdLine_MAIN::OnSelfolder() 
{
	CString strFolderLoc;
    m_folderLocCombo.GetWindowText(strFolderLoc);
	
	string folderLoc(strFolderLoc);
	if( BrowserGetDirectory("Select where to execute", folderLoc, IsWindow(m_hWnd) ? this : NULL) )
	{
		m_folderLocCombo.SetWindowText(folderLoc.c_str());
	}
}

/// OnInitDialog virtual override, enable controls and setup autocomplete
BOOL CCmdLine_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_multilineEntryDlg.Create(kMultilineCmdLine, &gPrevCvsCmd, &m_multilineEntryPlaceholder, this);
	m_cvsrootEntryDlg.Create(&m_cvsrootEntryPlaceholder, this);

	EnableEditAutoComplete(m_folderLocCombo.GetEditCtrl());
	
	OnCheckpath();
	OnAddSelection();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCmdLine_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_ADD_DEFAULT,		IDH_IDC_ADD_DEFAULT);
	SetCtrlHelpID(IDC_HAS_PATH,			IDH_IDC_HAS_PATH);
	SetCtrlHelpID(IDC_ADD_SELECTION,	IDH_IDC_ADD_SELECTION);
	SetCtrlHelpID(IDC_PATH,				IDH_IDC_PATH);
	SetCtrlHelpID(IDC_BROWSE_PATH,		IDH_IDC_BROWSE_PATH);
}

/// BN_CLICKED message handler, enable controls
void CCmdLine_MAIN::OnAddSelection() 
{
	if( m_addSelectionCheck.IsWindowEnabled() )
	{
		const BOOL enable = m_addSelectionCheck.GetCheck() ? false : true;
		
		OnCheckpath();
		
		m_checkPathCheck.EnableWindow(enable);
		m_selFolderButton.EnableWindow(enable);
		m_folderLocCombo.EnableWindow(enable);
		
		if( enable )
		{
			OnCheckpath();
		}
	}
}

/*!
	Get the entered command line
	\return The entered command line
*/
CString CCmdLine_MAIN::GetCmdLine() const
{
	return m_cmdLine;
}

#endif /* WIN32 */

#if qUnix
class UCvsCmdline : public UWidget
{
	UDECLARE_DYNAMIC(UCvsCmdline)
public:
	UCvsCmdline() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsCmdline() {}

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

UIMPLEMENT_DYNAMIC(UCvsCmdline, UWidget)

UBEGIN_MESSAGE_MAP(UCvsCmdline, UWidget)
	ON_UCOMMAND(UCvsCmdline::kOK, UCvsCmdline::OnOK)
	ON_UCOMMAND(UCvsCmdline::kCancel, UCvsCmdline::OnCancel)
UEND_MESSAGE_MAP()

int UCvsCmdline::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsCmdline::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsCmdline::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsCmdline_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsCmdline_MAIN)
public:
	UCvsCmdline_MAIN(const char *lastcmd, bool hasPath,
					 const char *lastpath);
	virtual ~UCvsCmdline_MAIN() {}

	enum
	{
		kEditCmd = EV_COMMAND_START,	// 0
		kCheckPath,			// 1
		kStatPath,			// 2
		kBtnPath			// 3
	};

	virtual void DoDataExchange(bool fill);

	bool m_checkpath;
	std::string  m_cmdline;
	std::string  m_folderloc;
protected:
	ev_msg int OnCheckPath(void);
	ev_msg int OnBtnPath(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsCmdline_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsCmdline_MAIN, UWidget)
	ON_UCOMMAND(UCvsCmdline_MAIN::kCheckPath, UCvsCmdline_MAIN::OnCheckPath)
	ON_UCOMMAND(UCvsCmdline_MAIN::kBtnPath, UCvsCmdline_MAIN::OnBtnPath)
UEND_MESSAGE_MAP()

UCvsCmdline_MAIN::UCvsCmdline_MAIN(const char *lastcmd, bool hasPath,
							 const char *lastpath) : UWidget(::UEventGetWidID())
{
	m_checkpath = hasPath;
	m_cmdline = lastcmd;
	m_folderloc = lastpath;
}

int UCvsCmdline_MAIN::OnCheckPath()
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckPath, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kStatPath, state), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kBtnPath, state), 0L);
	return 0;
}

int UCvsCmdline_MAIN::OnBtnPath()
{
	std::string  dir;
	if( BrowserGetDirectory("Select where to execute", dir) )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kStatPath, (void*)dir.c_str());
	}

	return 0;
}

void UCvsCmdline_MAIN::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckPath, m_checkpath), 0L);
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditCmd, (void *)(const char *)m_cmdline.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kStatPath, (void *)(const char *)m_folderloc.c_str());

		OnCheckPath();
	}
	else
	{
		m_checkpath = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckPath, 0L) != 0;
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditCmd, &m_cmdline);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kStatPath, &m_folderloc);
	}
}
#endif // qUnix

/// Get the command line options
bool CompatGetCommand(const MultiFiles* mf,
					  std::string& cmd, std::string& dir, bool& addDefault, bool& addSelection,
					  bool& forceCvsroot, std::string& cvsroot)
{
	bool userHitOK = false;

	static string sLastCmdline(DEF_CVS_CMD);
	static string sLastPath("");
	static bool sLastHasPath = false;
	static bool lastAddDefault = false;
	static bool lastAddSelection = false;
	static bool lastForceCvsroot = false;
	static string sLastCvsroot("");

	if( mf && mf->NumDirs() > 0 )
	{
		string path;
		mf->getdir(0, path);
		
		FindBestLastCvsroot(path.c_str(), sLastCvsroot);
	}
	
#ifdef WIN32
	CHHelpPropertySheet pages(_i18n("Command line settings"));
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CCmdLine_MAIN page1(mf, sLastCmdline.c_str(), sLastHasPath, sLastPath.c_str(), lastAddDefault, lastAddSelection, lastForceCvsroot, sLastCvsroot.c_str());
	CGetPrefs_GLOBALS page2;

	pages.AddPage(&page1);
	pages.AddPage(&page2);
	
	if( pages.DoModal() == IDOK )
	{
		sLastCmdline = page1.GetCmdLine();
		sLastHasPath = page1.m_checkPath == TRUE;
		sLastPath = page1.m_folderLoc;
		lastAddDefault = page1.m_addDefault ? true : false;
		lastAddSelection = page1.m_addSelection ? true : false;
		lastForceCvsroot = page1.m_forceRoot ? true : false;
		sLastCvsroot = page1.m_cvsroot;

		page2.StoreValues();

		userHitOK = true;
	}
#endif /* WIN32 */

#if qUnix
	void *wid = UCreate_CmdlineDlg();

	UCvsCmdline *dlg = new UCvsCmdline();
	UCvsCmdline_MAIN *tab1 = new UCvsCmdline_MAIN(sLastCmdline.c_str(), sLastHasPath, sLastPath.c_str());
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	dlg->AddPage(tab1, UCvsCmdline::kTabGeneral, 0);

	if(dlg->DoModal())
	{
		sLastCmdline = tab1->m_cmdline;
		sLastHasPath = tab1->m_checkpath;
		sLastPath = tab1->m_folderloc;

		userHitOK = true;
	}

	delete dlg;
#endif // qUnix

#ifdef qMacCvsPP
	StDialogHandler	theHandler(dlg_Cmdline, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	static UInt16 sRuntimePanel = 1;
	
	LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
		(theDialog->FindPaneByID(item_MultiViewPrefs));
	LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
	
	multiView->SwitchToPanel(2);
	DoDataExchange_Globals(theDialog, true);
	multiView->SwitchToPanel(1);
	SetupHandler_CVSRoot(theDialog, theHandler);
	DoDataExchange_CVSRoot (theDialog, lastForceCvsroot, sLastCvsroot, true);
	DoDataExchange_CmdlineMain(theDialog, sLastCmdline, sLastPath, sLastHasPath, lastAddDefault, lastAddSelection, true);

	LCheckBox *cbox = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckGroupPath));
	LPushButton *epath = dynamic_cast<LPushButton*>
		(theDialog->FindPaneByID(item_EditPath));
	LEditText *pathDisplay = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_ShowPath));
	epath->AddListener(&theHandler);
	cbox->AddListener(&theHandler);
	
	groupView->SetValue(sRuntimePanel);
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
		
		if(hitMessage == item_CheckGroupPath || hitMessage == item_EditPath)
		{
			cbox = dynamic_cast<LCheckBox*>
				(theDialog->FindPaneByID(item_CheckGroupPath));
			pathDisplay = dynamic_cast<LEditText*>
				(theDialog->FindPaneByID(item_ShowPath));
			if(cbox->GetValue() == Button_Off)
			{
				continue;
			}
			
			if( BrowserGetDirectory("Select where to execute", dir) )
			{
				CPStr tmp(dir.c_str());
				pathDisplay->SetDescriptor(tmp);
			}
			else if(hitMessage == item_CheckGroupPath)
			{
				cbox->SetValue(Button_Off);
			}
		}
    else HandleHitMessage_CVSRoot(theDialog, hitMessage);
	}
	theDialog->Hide();
	sRuntimePanel = groupView->GetValue();
	
	if(hitMessage == msg_OK)
	{
		multiView->SwitchToPanel(1);
		DoDataExchange_CmdlineMain(theDialog, sLastCmdline, sLastPath, sLastHasPath, lastAddDefault, lastAddSelection, false);
	  DoDataExchange_CVSRoot (theDialog, lastForceCvsroot, sLastCvsroot, false);
		multiView->SwitchToPanel(2);
		DoDataExchange_Globals(theDialog, false);
		Assert_(sLastCmdline.length() != 0);

		userHitOK = true;
	}
#endif /* qMacCvsPP */

	if( userHitOK )
	{
		cmd = sLastCmdline;
		dir = sLastHasPath ? sLastPath.c_str() : "";

		addDefault = lastAddDefault;
		addSelection = lastAddSelection;
		
		forceCvsroot = lastForceCvsroot;
		cvsroot = lastForceCvsroot ? sLastCvsroot.c_str() : "";

		if( forceCvsroot )
		{
			string path;

			if( sLastHasPath )
			{
				path = dir;
			}
			else if( addSelection && mf && mf->NumDirs() > 0)
			{
				mf->getdir(0, path);
			}
			
			if( !path.empty() )
			{
				InsertLastCvsroot(path.c_str(), cvsroot.c_str());
			}
		}
		
		gCvsPrefs.save();
	}

	return userHitOK;
}
