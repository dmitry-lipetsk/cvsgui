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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

/*
 * 
 */

#include "stdafx.h"

#if qGTK
#	include <gtk/gtk.h>
#endif

#include "UCvsFrame.h"
#include "UCvsApp.h"
#include "UCvsCommands.h"
#include "UCvsConsole.h"
#include "UCvsFolders.h"
#include "UCvsFiles.h"
#include "UCvsToolbar.h"
#include "CvsAlert.h"
#include "PromptFiles.h"
#include "CvsPrefs.h"
#include "AppConsole.h"
#include "CvsCommands.h"
#include "TclGlue.h"
#include "MacrosSetup.h"
#include "GetPrefs.h"
#include "UCvsDialogs.h"
#include "BrowseViewHandlers.h"
#include "AboutDlg.h"
#include "MultiString.h"
#ifdef USE_PYTHON
#	include "PythonGlue.h"
#endif

UIMPLEMENT_DYNAMIC(UCvsFrame, UWidget)

UBEGIN_MESSAGE_MAP(UCvsFrame, UWidget)
	ON_UUPDATECMD_RANGE(cmdABOUT, cmdABOUT, UCvsFrame::OnCmdUIAnytime)
	ON_UUPDATECMD_RANGE(cmdCREDITS, cmdCOPYRIGHTS, UCvsFrame::OnCmdUIAnytime)
	ON_UUPDATECMD_RANGE(UCvsFrame::kDirCombo, UCvsFrame::kBrowserPopup, UCvsFrame::OnCmdUIAnytime)
	ON_UUPDATECMD(cmdEXIT, UCvsFrame::OnCmdUIQuit)
	ON_UUPDATECMD_RANGE(cmdPREFERENCES, cmdPREFERENCES, UCvsFrame::OnCmdUIAnytime)
	ON_UUPDATECMD(cmdSTOPCVS, UCvsFrame::OnCmdUIStopcvs)
	ON_UUPDATECMD(cmdRTAGCREATE, UCvsFrame::OnCmdUICvsCmd)
	ON_UUPDATECMD(cmdOPEN, UCvsFrame::OnCmdUICvsCmd)
	ON_UUPDATECMD(cmdSAVESETTINGS, UCvsFrame::OnCmdUICvsCmd)
	ON_UUPDATECMD(cmdIGNORE, UCvsFrame::OnCmdUIViewIgnored)
	ON_UUPDATECMD(cmdVIEW_FILEMASK, UCvsFrame::OnCmdUIFilterMask)
	ON_UUPDATECMD(cmdVIEW_ADDED, UCvsFrame::OnCmdUIViewAdded)
	ON_UUPDATECMD(cmdVIEW_MODIFIED, UCvsFrame::OnCmdUIViewModified)
	ON_UUPDATECMD(cmdVIEW_CONFLICT, UCvsFrame::OnCmdUIViewConflict)
	ON_UUPDATECMD(cmdVIEW_MISSING, UCvsFrame::OnCmdUIViewMissing)
	ON_UUPDATECMD(cmdVIEW_UNKNOWN, UCvsFrame::OnCmdUIViewUnknown)
	ON_UUPDATECMD(cmdVIEW_REMOVED, UCvsFrame::OnCmdUIViewRemoved)
	ON_UUPDATECMD(cmdVIEW_CHANGED, UCvsFrame::OnCmdUIViewChanged)
	ON_UUPDATECMD(cmdHIDE_UNKNOWN, UCvsFrame::OnCmdUIHideUnknown)
	ON_UUPDATECMD(cmdRELOAD, UCvsFrame::OnCmdUIReload)
	ON_UUPDATECMD(cmdINIT, UCvsFrame::OnCmdUICvsCmd)
	ON_UUPDATECMD(cmdRTAGDELETE, UCvsFrame::OnCmdUICvsCmd)
	ON_UUPDATECMD(cmdRTAGBRANCH, UCvsFrame::OnCmdUICvsCmd)
	ON_UUPDATECMD(cmdCMDLINE, UCvsFrame::OnCmdUICvsCmd)
	ON_UUPDATECMD(cmdLOGIN, UCvsFrame::OnCmdUICvsCmd)
	ON_UUPDATECMD(cmdLOGOUT, UCvsFrame::OnCmdUICvsCmd)
#ifdef USE_PYTHON
	ON_UUPDATECMD(cmdUSE_TCLSHELL, UCvsFrame::OnCmdUIUseTclShell)
	ON_UUPDATECMD(cmdUSE_PYSHELL, UCvsFrame::OnCmdUIUsePyShell)
	ON_UUPDATECMD_RANGE(cmdPyMACROS, cmdPyMACROSEND, UCvsFrame::OnCmdUIMacroAdmin)
#else
	ON_UUPDATECMD_RANGE(cmdMACROADMIN, cmdMACROADMINEND, UCvsFrame::OnCmdUIMacroAdmin)
#endif
	ON_COMBO_SEL(UCvsFrame::kDirCombo, UCvsFrame::OnComboSelDir)
	ON_COMBO_SEL(cmdVIEW_FILEMASK, UCvsFrame::OnCmdReload)
	ON_UCOMMAND(cmdVIEW_FILEMASK, UCvsFrame::OnCmdReload)
	ON_UCOMMAND(UCvsFrame::kBrowserPopup, UCvsFrame::OnBrowserPopup)
	ON_UCOMMAND(cmdOPEN, UCvsFrame::OnBrowserPopup)
	ON_UCOMMAND(cmdCOPYRIGHTS, UCvsFrame::OnCopyrights)
	ON_UCOMMAND(cmdCREDITS, UCvsFrame::OnCredits)
	ON_UCOMMAND(cmdABOUT, UCvsFrame::OnAbout)
	ON_UCOMMAND(cmdEXIT, UCvsFrame::OnQuit)
	ON_UCOMMAND(cmdPREFERENCES, UCvsFrame::OnCmdPreferences)
	ON_UCOMMAND(cmdSAVESETTINGS, UCvsFrame::OnCmdSaveSettings)
	ON_UCOMMAND(cmdRTAGCREATE, UCvsFrame::OnCmdRtagNew)
	ON_UCOMMAND(cmdRTAGDELETE, UCvsFrame::OnCmdRtagDelete)
	ON_UCOMMAND(cmdRTAGBRANCH, UCvsFrame::OnCmdRtagBranch)
	ON_UCOMMAND(cmdCMDLINE, UCvsFrame::OnCmdCmdline)
	ON_UCOMMAND(cmdLOGIN, UCvsFrame::OnCmdLogin)
	ON_UCOMMAND(cmdLOGOUT, UCvsFrame::OnCmdLogout)
	ON_UCOMMAND(cmdSTOPCVS, UCvsFrame::OnCmdStopcvs)
	ON_UCOMMAND(cmdINIT, UCvsFrame::OnCmdInit)
	ON_UCOMMAND(cmdIGNORE, UCvsFrame::OnCmdIgnore)
	ON_UCOMMAND(cmdVIEW_ADDED, UCvsFrame::OnViewAdded)
	ON_UCOMMAND(cmdVIEW_MODIFIED, UCvsFrame::OnViewModified)
	ON_UCOMMAND(cmdVIEW_CONFLICT, UCvsFrame::OnViewConflict)
	ON_UCOMMAND(cmdVIEW_MISSING, UCvsFrame::OnViewMissing)
	ON_UCOMMAND(cmdVIEW_UNKNOWN, UCvsFrame::OnViewUnknown)
	ON_UCOMMAND(cmdVIEW_REMOVED, UCvsFrame::OnViewRemoved)
	ON_UCOMMAND(cmdVIEW_CHANGED, UCvsFrame::OnViewChanged)
	ON_UCOMMAND(cmdHIDE_UNKNOWN, UCvsFrame::OnHideUnknown)
	ON_UCOMMAND(cmdRELOAD, UCvsFrame::OnCmdReload)
#ifdef USE_PYTHON
	ON_UCOMMAND(cmdUSE_TCLSHELL, UCvsFrame::OnCmdUseTclShell)
	ON_UCOMMAND(cmdUSE_PYSHELL, UCvsFrame::OnCmdUsePyShell)
	ON_UCOMMAND_RANGE(cmdPyMACROS, cmdPyMACROSEND, UCvsFrame::OnCmdMacroAdmin)
#else
	ON_UCOMMAND_RANGE(cmdMACROADMIN, cmdMACROADMINEND, UCvsFrame::OnCmdMacroAdmin)
#endif
	ON_UCREATE(UCvsFrame)
	ON_UDESTROY(UCvsFrame)
UEND_MESSAGE_MAP()

#if 0
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	//ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, CDocument::OnUpdatePasteMenu)
	ON_COMMAND_EX_RANGE(ID_MACRO_ADMIN, ID_MACRO_ADMIN + 99, OnMacroAdmin)
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
#endif

#if qGTK
# define BLOCK_UPDATE_UI {if(InUpdateUI) return 0;}
# define START_UPDATE_UI {InUpdateUI = true;}
# define END_UPDATE_UI {InUpdateUI = false;}
#else
# define BLOCK_UPDATE_UI
# define START_UPDATE_UI
# define END_UPDATE_UI
#endif

CKeyString filterFileMasks(NUM_FILEMASKS, "P_OldFilterFileMasks");

UCvsFrame::UCvsFrame() : UWidget(kUCvsFrameID), InUpdateUI(false)
{
}

UCvsFrame::~UCvsFrame()
{
}

void onIgnoreChanged(CObject *obj)
{
	((UCvsFrame *)obj)->OnCmdIgnore();
}

void UCvsFrame::OnCreate()
{
	if( UCvsApp* app = UCvsApp::gApp )
		app->GetIgnoreModel()->GetNotificationManager()->CheckIn((CObject*)this, onIgnoreChanged);
}

void UCvsFrame::OnDestroy(void)
{
#if qGTK
	gtk_main_quit();
#endif /* qGTK */
	
	if( UCvsApp* app = UCvsApp::gApp )
		app->GetIgnoreModel()->GetNotificationManager()->CheckOut((CObject*)this);
	delete this;
}

void UCvsFrame::OnComboSelDir(int pos, const char *txt)
{
	UCvsApp::gApp->GetBrowserView()->ResetBrowser(txt, true);
}

int UCvsFrame::OnBrowserPopup(void)
{
	std::string newPath;
	if( BrowserGetDirectory(_i18n("Select a folder to open with the browser"), newPath) )
	{
		UCvsApp::gApp->GetBrowserView()->ResetBrowser(newPath.c_str(), true);
	}

	return 0;
}

int UCvsFrame::OnQuit(void)
{
	if(UCvsApp::gApp->IsCvsRunning()) {
		CvsAlert al(kCvsAlertNoteIcon, _i18n("Cvs is currently running."), 
				 _i18n("Stop CVS first before exiting."), BUTTONTITLE_OK, NULL);
	  
		al.ShowAlert(kCvsAlertOKButton);
		
		return 0;
	}
	
	gCvsPrefs.save();
	
#if qGTK
	gtk_main_quit();
#endif /* qGTK */

	return 0;
}

void UCvsFrame::OnCmdUIQuit(UCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}

void UCvsFrame::OnCmdUIAnytime(int cmd, UCmdUI *pCmdUI)
{
	pCmdUI->Enable(!UCvsApp::gApp->IsCvsRunning());
}

int UCvsFrame::OnAbout()
{
	CompatAboutDialog();
	return 0;
}

int UCvsFrame::OnCopyrights()
{
	const char *appName = UCvsApp::gApp->GetAppName();

	cvs_out(_i18n("\nBoth %s and CVS are distributed under the terms of\n"
		      "the GNU General Public License (GPL).\n\n"), appName);
	cvs_out(_i18n("* %s : maintained by Alexandre Parenteau (aubonbeurre@hotmail.com)\n"), appName);
	cvs_out(_i18n("* CVS : Copyright © 1989-1998 Brian Berliner, david d `zoo' zuhn,\n"));
	cvs_out(_i18n("        Jeff Polk, and other authors\n"));
#ifdef macintosh
	printf("* MacCVS : Mike Ladwig, Matthias Neeracher, Alexandre Parenteau\n");
	printf("* GUSI : Copyright © 1992-1997 Matthias Neeracher\n");
	printf("* Waste : Copyright © 1993-1998 Marco Piovanelli\n");
	printf("* CWASTEEdit : Copyright © 1995-1997 Timothy Paustian\n");
	printf(_i18n("* ABalloon : Freeware of James W. Walker (jwwalker@kagi.com)\n"));
	printf("* Metrowerks Standard Library, PowerPlant & CodeWarrior :\n");
	printf("    Copyright © 1996-1997 Metrowerks Inc.\n");
	printf("* MPW, MoreFiles : Copyright © Apple Computer Inc.\n");
#endif
#ifdef WIN32
	cvs_out(_i18n("* WinCvs documentation : Copyright © 1999 Don Harper <don@lamrc.com>.\n"));
	cvs_out(_i18n("* CJLIB : Copyright © 1998-99 Kirk Stowell <kstowell@codejockeys.com>\n"));
	cvs_out(_i18n("* MFC : Copyright © 1992-1997 Microsoft Corporation\n"));
#endif
	cvs_out(_i18n("* Kerberos : Copyright © 1997 Massachusetts Institute of Technology\n"));
	cvs_out(_i18n("* TCL : Copyright © Sun Microsystems Inc.\n"));
	cvs_out(_i18n("\nSome others copyrights may apply, check the source code for details.\n"));
	return 0;
}

int UCvsFrame::OnCredits()
{
	const char *appName = UCvsApp::gApp->GetAppName();
	const char *appVers = UCvsApp::gApp->GetAppVersion();

#ifndef macintosh
	cvs_out(_i18n("\n%s %s - Client/Local\n"), appName, appVers);
#else
	cvs_out(_i18n("\n%s %s - Client\n"), appName, appVers);
#endif
	cvs_out(_i18n("%s is maintained by Alexandre Parenteau\n"), appName);
	cvs_out(_i18n("- WinCvs/MacCvs/gcvs page : http://www.wincvs.org or http://cvsgui.sourceforge.net/\n"));
	cvs_out(_i18n("- cvs page : http://www.cvshome.org\n"));
#ifdef WIN32
	cvs_out(_i18n("- WinCvs documentation : Don Harper <don@lamrc.com>.\n"));
	cvs_out(_i18n("- WinHelp cvs documentation : Norbert Klamann.\n"));
#endif
#ifdef macintosh
	printf(_i18n("- Mac port :\n"));
	printf("\t* Mike Ladwig <mike@twinpeaks.prc.com>\n");
	printf("\t* Matthias Neeracher <neeri@iis.ee.ethz.ch>\n");
	printf("\t* Alexandre Parenteau <aubonbeurre@hotmail.com>\n");
	printf(_i18n("- With contributions of :\n"));
	printf("\t* Miro Jurisic <meeroh@mit.edu> (Kerberos)\n");
	printf("\t* Aleks Totic <atotic@netscape.com> (AppleSingle)\n");
	printf("\t* Jocelyn Houle <houlejo@IRO.UMontreal.CA> (Help balloons)\n");
#endif
	cvs_out(_i18n("- Special thanks to :\n"));
	cvs_out(_i18n("\t* Gary Bringhurst from C3D Digital who gives me time, resources,\n"
	          "\t  help and ideas for doing this project (http://www.3d.com)\n"));
	cvs_out(_i18n("\t* The Freeware community for providing cvs, Linux, Perl, GTK, gcc\n"
	          "\t  and so many others valuables tools\n"));
	cvs_out(_i18n("\t* The users of the CvsGui project (WinCvs, MacCvs, gcvs) who have\n"
	          "\t  defined all the functionnalities and help every day to enhance it\n"));
	cvs_out(_i18n("- Cvs contributors :\n"));
	cvs_out(_i18n("\t* Too many to be listed here, see the 'ChangeLog' instead !\n"));
	return 0;
}

int UCvsFrame::OnCmdPreferences(void)
{
	UCvsConsole* console = UCvsApp::gApp->GetConsole();
	CompatGetPrefs();
	
	if(console != 0l)
		console->setFont(gCvsPrefs.ConsoleFont());
	return 0;
}

int UCvsFrame::OnCmdSaveSettings(void)
{
	UCvsFolders *browser = UCvsApp::gApp->GetBrowserView();
	if(browser != 0L && browser->GetRoot() != 0L)
		AskCreatePersistentSettings(browser->GetRoot());
	return 0;
}

int UCvsFrame::OnCmdRtagNew(void)
{
  KoRtagCreateHandler handler;
	CvsCmdRtag(handler, kTagCreate);
  
	return 0;
}

int UCvsFrame::OnCmdRtagDelete(void)
{
  KoRtagDeleteHandler handler;
	CvsCmdRtag(handler, kTagDelete);
	return 0;
}

int UCvsFrame::OnCmdRtagBranch(void)
{
  KoRtagBranchHandler handler;
	CvsCmdRtag(handler, kTagBranch);
	return 0;
}

int UCvsFrame::OnCmdCmdline(void)
{
  KoCommandLineHandler handler;
	CvsCmdCommandLine(handler);
	return 0;
}

int UCvsFrame::OnCmdLogin(void)
{
	CvsCmdLogin();
	return 0;
}

int UCvsFrame::OnCmdLogout(void)
{
	CvsCmdLogout();
	return 0;
}

int UCvsFrame::OnCmdStopcvs(void)
{
	UCvsApp::gApp->SetCvsStopping(true);
	if(UCvsApp::gApp->GetCvsProcess() != 0L && cvs_process_is_active(UCvsApp::gApp->GetCvsProcess()))
	{
		cvs_process_stop(UCvsApp::gApp->GetCvsProcess());
		cvs_out(_i18n("***** CVS stopped on user request !!! *****\n\n"));
	}
	
	return 0;
}

int UCvsFrame::OnCmdInit(void)
{
	CvsCmdInit();
	return 0;
}

int UCvsFrame::OnCmdMacroAdmin(int cmd)
{
#ifndef USE_PYTHON
	CMacroEntry & entry = gMacrosAdmin.entries[cmd - cmdMACROADMIN];
	CvsCmdMacrosAdmin(entry.path);
#else
	PyDoPython(cmd);
#endif
	return 0;
}

void UCvsFrame::OnCmdUIStopcvs(UCmdUI *pCmdUI)
{
	pCmdUI->Enable(UCvsApp::gApp->IsCvsRunning() && !UCvsApp::gApp->IsCvsStopping());
}

void UCvsFrame::OnCmdUICvsCmd(UCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsAppEnabled());
}

void UCvsFrame::OnCmdUIMacroAdmin(int cmd, UCmdUI *pCmdUI)
{
#ifndef USE_PYTHON
	pCmdUI->Enable(IsAppEnabled() && CTcl_Interp::IsAvail());
#else
	if(!IsAppEnabled())
	{
		pCmdUI->Enable(false);
		return;
	}
	if( !PyIsUICacheValid() )
    {
		int nItem = -1;
		while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
		{
			EntnodeData *data;
			bool deleteData = false;
			UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);
			
			PyAppendCache(data);
		}
	} {

	UCmdUI ucmdui(pCmdUI->m_nID);
	ucmdui.pCmdUI = pCmdUI;
	PyDoCmdUI(&ucmdui);
	}
#endif
}

int UCvsFrame::OnViewIgnored()
{
	BLOCK_UPDATE_UI
	m_ignore.ToggleIgnore();
	m_ignore.GetNotificationManager()->NotifyAll();
	OnFilterMaskEntered();
	
	return 0;
}

void UCvsFrame::OnCmdUIViewIgnored(UCmdUI* pCmdUI)
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->Check(m_ignore.IsShowIgnored());
	END_UPDATE_UI
}

void UCvsFrame::OnCmdUIFilterMask(UCmdUI* pCmdUI)
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	END_UPDATE_UI
}

bool UCvsFrame::FreshenFileMask(void)
{
	std::string strFileMask;
	UCvsToolbar* toolbar = (UCvsToolbar *)::UEventQueryWidget(kUCvsFilterToolbarID);	

	UEventSendMessage(toolbar->GetWidID(), EV_GETTEXT, cmdVIEW_FILEMASK, &strFileMask);
	
	filterFileMasks.Insert(strFileMask.c_str());
		
	::DoDataExchange(true, toolbar->GetWidID(), cmdVIEW_FILEMASK, filterFileMasks);
	
	return m_filter.SetMask(EntnodeData::kName, strFileMask.c_str());
}

int UCvsFrame::OnViewModified()
{
	BLOCK_UPDATE_UI
	m_filter.ToggleModified();
	FreshenFileMask();
	m_filter.GetNotificationManager()->NotifyAll();
	
	return 0;
}

void UCvsFrame::OnCmdUIViewModified(UCmdUI* pCmdUI)
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->Check(m_filter.IsModified());
	END_UPDATE_UI
}

void UCvsFrame::OnFilterMaskEntered()
{
//	BLOCK_UPDATE_UI
	if (FreshenFileMask())
	{
		m_filter.GetNotificationManager()->NotifyAll();
	}
}

int UCvsFrame::OnViewConflict()
{
	BLOCK_UPDATE_UI
	m_filter.ToggleConflict();
	FreshenFileMask();
	m_filter.GetNotificationManager()->NotifyAll();
	
	return 0;
}

void UCvsFrame::OnCmdUIViewConflict(UCmdUI* pCmdUI)
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->Check(m_filter.IsConflict());
	END_UPDATE_UI
}

int UCvsFrame::OnViewUnknown()
{
	BLOCK_UPDATE_UI
	m_filter.ToggleUnknown();
	FreshenFileMask();
	m_filter.GetNotificationManager()->NotifyAll();
	
	return 0;
}

void UCvsFrame::OnCmdUIViewUnknown(UCmdUI* pCmdUI)
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->Check(m_filter.IsUnknown());
	END_UPDATE_UI
}

int UCvsFrame::OnViewMissing()
{
	BLOCK_UPDATE_UI
	m_filter.ToggleMissing();
	FreshenFileMask();
	m_filter.GetNotificationManager()->NotifyAll();
	
	return 0;
}

void UCvsFrame::OnCmdUIViewMissing(UCmdUI* pCmdUI)
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->Check(m_filter.IsMissing());
	END_UPDATE_UI
}

int UCvsFrame::OnViewAdded() 
{
	BLOCK_UPDATE_UI
	m_filter.ToggleAdded();
	FreshenFileMask();
	m_filter.GetNotificationManager()->NotifyAll();
	
	return 0;
}

void UCvsFrame::OnCmdUIViewAdded(UCmdUI* pCmdUI) 
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->Check(m_filter.IsAdded());
	END_UPDATE_UI
}

void UCvsFrame::OnCmdUIViewRemoved(UCmdUI* pCmdUI) 
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->Check(m_filter.IsRemoved());
	END_UPDATE_UI
}

int UCvsFrame::OnViewRemoved() 
{
	BLOCK_UPDATE_UI
	m_filter.ToggleRemoved();
	FreshenFileMask();
	m_filter.GetNotificationManager()->NotifyAll();
	
	return 0;
}

int UCvsFrame::OnViewChanged() 
{
	BLOCK_UPDATE_UI
	m_filter.ToggleChanged();
	FreshenFileMask();
	m_filter.GetNotificationManager()->NotifyAll();
	
	return 0;
}

void UCvsFrame::OnCmdUIViewChanged(UCmdUI* pCmdUI) 
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->Check(m_filter.IsChanged());
	END_UPDATE_UI
}

int UCvsFrame::OnHideUnknown() 
{
	BLOCK_UPDATE_UI
	m_filter.ToggleHideUnknown();
	FreshenFileMask();
	m_filter.GetNotificationManager()->NotifyAll();
	
	return 0;
}

void UCvsFrame::OnCmdUIHideUnknown(UCmdUI* pCmdUI) 
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled() && m_filter.IsDefaultMode());	
	pCmdUI->Check(m_filter.IsHideUnknown());
	END_UPDATE_UI
}

int UCvsFrame::OnCmdReload() 
{
	BLOCK_UPDATE_UI
	FreshenFileMask();
	m_filter.GetNotificationManager()->NotifyAll();
	
	return 0;
}

void UCvsFrame::OnCmdUIReload(UCmdUI *pCmdUI)
{
	START_UPDATE_UI
	pCmdUI->Enable(IsAppEnabled());
	END_UPDATE_UI
}

#ifdef USE_PYTHON
int UCvsFrame::OnCmdUsePyShell() 
{
	BLOCK_UPDATE_UI
	if( gCvsPrefs.GetUseShell() == SHELL_PYTHON)
		return 0;
	gCvsPrefs.SetUseShell(SHELL_PYTHON);
	cvs_out("Switching the shell to Python...\n");
	
	UCvsApp* app = UCvsApp::gApp;
	if( app )
	{
		app->RestartScriptEngine();
	}
	
	return 0;
}

void UCvsFrame::OnCmdUIUsePyShell(UCmdUI * pCmdUI) 
{
	START_UPDATE_UI
	pCmdUI->Enable(CPython_Interp::IsAvail());
	pCmdUI->Check(gCvsPrefs.GetUseShell() == SHELL_PYTHON);
	END_UPDATE_UI
}

int UCvsFrame::OnCmdUseTclShell() 
{
	BLOCK_UPDATE_UI
	if( gCvsPrefs.GetUseShell() == SHELL_TCL)
		return 0;
	gCvsPrefs.SetUseShell(SHELL_TCL);
	cvs_out("Switching the shell to TCL...\n");
	
	UCvsApp* app = UCvsApp::gApp;
	if( app )
	{
		app->RestartScriptEngine();

	}
	
	return 0;
}

void UCvsFrame::OnCmdUIUseTclShell(UCmdUI * pCmdUI) 
{
	START_UPDATE_UI
	pCmdUI->Enable(CTcl_Interp::IsAvail());
	pCmdUI->Check(gCvsPrefs.GetUseShell() == SHELL_TCL);
	END_UPDATE_UI
}
#endif

int UCvsFrame::OnCmdIgnore(void)
{
	BLOCK_UPDATE_UI
	KoIgnoreModel *m_ignore = (KoIgnoreModel *)UCvsApp::gApp->GetIgnoreModel();
	m_ignore->ToggleIgnore();

	UCvsApp::gApp->GetBrowserView()->ResetView(true, true);
	return 0;
}

void UCvsFrame::OnCmdUIIgnore(UCmdUI *pCmdUI)
{	
	START_UPDATE_UI
	KoIgnoreModel *m_ignore = (KoIgnoreModel *)UCvsApp::gApp->GetIgnoreModel();
	pCmdUI->Enable(IsAppEnabled());
	pCmdUI->Check(m_ignore->IsShowIgnored());
	END_UPDATE_UI
}
