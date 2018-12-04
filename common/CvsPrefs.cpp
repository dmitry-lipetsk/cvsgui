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

/*
 * CvsPrefs.cpp --- class to handle CVS settings
 */

#include "stdafx.h"

#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include "CvsPrefs.h"
#include "Authen.h"
#include "AppConsole.h"
#include "CvsArgs.h"
#include "MultiString.h"
#include "FileTraversal.h"

#ifdef TARGET_OS_MAC
#	define PCVS "\pCVSROOT"
#	include <Resources.h>
#	include <Folders.h>
#	include <MacErrors.h>
#	include <TextUtils.h>
#	include "VolsPaths.h"
#	include "MacMisc.h"
#	include "AppGlue.h"
#endif /* TARGET_OS_MAC */

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#ifdef HAVE_PWD_H
#	include <pwd.h>
#endif

#ifdef WIN32
#	include "WinCvsBrowser.h"
#	include "wincvs.h"

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#ifdef qMacCvsPP
#	include "GUSIInternal.h"
#	include "GUSIFileSpec.h"
#	include "MacCvsApp.h"
#	include <UProcess.h>
#endif

#if qUnix
#	include "UCvsApp.h"
#	include "UCvsFolders.h"
#endif

using namespace std;

#ifndef WIN32
#	define DEFAULT_MATCH_ENTRIES_CASE true
#else
#	define DEFAULT_MATCH_ENTRIES_CASE false
#endif
	
CvsPrefs gCvsPrefs;

static CPersistentInt gPersistentShema("P_PersistentShema", SCHEMA_0);

static CPersistentBool gZ9Option("P_Z9Option", false, kAddSettings);
static CPersistentBool gPruneOption("P_PruneOption", true, kAddSettings);
static CPersistentInt gIsoConvert("P_NIsoConvert", ISO8559_none, kAddSettings);
static CPersistentBool gICTextOption("P_ICTextOption", false, kAddSettings);
static CPersistentBool gAddBinControl("P_AddBinControl", true);
static CPersistentInt gMacBinEncoding("P_NMacBinEncoding", MAC_HQX, kAddSettings);
static CPersistentBool gMacBinEncodeOnlyBinaryFiles("P_MacBinOnlyBinaries", true, kAddSettings);
static CPersistentInt gUseShell("P_UseShell", SHELL_TCL);
static CPersistentBool gMatchEntriesCase("P_MatchEntriesCase", DEFAULT_MATCH_ENTRIES_CASE);
static CPersistentBool gSmartSorting("P_SmartSorting", false);

#ifdef WIN32 
static CPersistentInt gCheckoutFileAttribute("P_CheckoutFileAttribute", CHECKOUT_DEFAULT, kAddSettings);
static CPersistentInt gCvsMessages("P_CvsMessages", CVSMESSAGES_DEFAULT);
#else
static CPersistentBool gCheckoutRO("P_CheckoutRO", false, kAddSettings);
static CPersistentBool gQuietOption("P_QuietOption", false);
#endif

#ifndef WIN32
// Obsolete, replaced by gFileActionType
static CPersistentBool gUseViewerAlways("P_UseViewerAlways", false, kAddSettings);
#endif

CMString gOldCvsroots(NUM_CVSROOT, "P_OldCvsroots");
static CKeyString gLastCvsroot(100, "P_LastCvsroot");
static CKeyString gLastModuleName(100, "P_LastModuleName");

#if INTERNAL_AUTHEN
static CPersistentString gCvsroot("P_Cvsroot");
static CPersistentInt gKserverPort("P_KserverPort", -1, kAddSettings);
static CPersistentInt gPserverPort("P_PserverPort", -1, kAddSettings);
static CPersistentInt gRhostPort("P_RhostPort", -1, kAddSettings);
static CPersistentString gServerName("P_ServerName", 0L, kAddSettings);
static CPersistentString gRshName("P_RshName", 0L, kAddSettings);
#endif /*# INTERNAL_AUTHEN */

static CPersistentBool gEncryptCommunication("P_EncryptCommunication", false, kAddSettings);

#if !qUnix && !TARGET_RT_MAC_MACHO
static CPersistentString gHome("P_Home", 0L, kAddSettings);
#endif /* !qUnix */

#ifdef qUnix
static CPersistentString gConsoleFont("P_ConsoleFont", "-adobe-courier-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
#endif

#ifdef qUnix
static CPersistentString gViewer("P_Viewer", "emacs"); // not vi ;-)
#elif defined(qMacCvsPP)
static CPersistentString gViewer("P_Viewer", "");
#else
static CPersistentString gViewer("P_Viewer", "Notepad");
#endif

#ifdef qUnix
static CPersistentString gConflictEditor("P_ConflictEditor", "emacs");
#elif defined(qMacCvsPP)
static CPersistentString gConflictEditor("P_ConflictEditor", "");
#else
static CPersistentString gConflictEditor("P_ConflictEditor", "Notepad");
#endif

static CPersistentString gAnnotateViewer("P_AnnotateViewer", "");

static CPersistentString gHelper("P_Helper", "netscape");
static CPersistentString gBrowser("P_Browser", "gmc");

#ifdef qUnix
static CPersistentString gExtdiff("P_Extdiff", "emacs");
#elif TARGET_RT_MAC_MACHO
static CPersistentString gExtdiff("P_Extdiff", "/usr/bin/opendiff");
#else
static CPersistentString gExtdiff("P_Extdiff", "");
#endif

#if DIRTY_MCP_SUPPORT
static CPersistentBool gDirtySupport("P_DirtySupport", true, kAddSettings);
#endif

#if INTERNAL_AUTHEN
static CPersistentBool gAlwaysUseCvsroot("P_AlwaysUseCvsroot", false, kAddSettings);
#endif

static CPersistentInt gLogoutTimeOut("P_LogoutTimeOut", 0);
static CPersistentInt gRefreshInactiveTimeOut("P_RefreshInactiveTimeOut", 5000);
static CPersistentString gProxyHost("P_ProxyHost", "www.mydomain", kAddSettings);
static CPersistentInt gProxyPort("P_ProxyPort", 8888, kAddSettings);
static CPersistentBool gUseProxy("P_UseProxy", false, kAddSettings);
static CPersistentBool gUnixLF("P_UnixLF", false, kAddSettings);
static CPersistentBool gMacLF("P_MacLF", false, kAddSettings);
static CPersistentInt gZLevel("P_ZLevel", 9, kAddSettings);
static CPersistentInt gCvsVersion("P_CvsVersion", 0, kAddSettings);
static CPersistentBool gCvsConsole("P_CvsConsole", false, kAddSettings);
static CPersistentInt gWordWrapLogMsg("P_WordWrapLogMsg", 0, kAddSettings);
static CPersistentString gLastWorkingDir("P_LastWorkingDir", 0L, kAddSettings);

static CPersistentBool gDisableSplashScreen("P_DisableSplashScreen", false);

static CPersistentInt gStatusOutputType("P_StatusOutputType", STATUSOUTPUT_VERBOSE, kAddSettings);
static CPersistentInt gFileActionType("P_FileActionType", 0);
static CPersistentBool gUpdateBuildDirectories("P_UpdateBuildDirectories", false, kAddSettings);
static CPersistentBool gUpdateCaseSensitiveNames("P_UpdateCaseSensitiveNames", false, kAddSettings);
static CPersistentBool gUpdateLastCheckinTime("P_UpdateLastCheckinTime", false, kAddSettings);
static CPersistentBool gUpdateThreeWayConflicts("P_UpdateThreeWayConflicts", false, kAddSettings);

CPersistentBool gHideCommandDlgUpdate("P_HideCommandDlgUpdate", false);
CPersistentBool gHideCommandDlgQueryUpdate("P_HideCommandDlgQueryUpdate", false);
CPersistentBool gHideCommandDlgDiff("P_HideCommandDlgDiff", false);
CPersistentBool gHideCommandDlgLog("P_HideCommandDlgLog", false);
CPersistentBool gHideCommandDlgGraph("P_HideCommandDlgGraph", false);
CPersistentBool gHideCommandDlgAnnotate("P_HideCommandDlgAnnotate", false);
CPersistentBool gHideCommandDlgStatus("P_HideCommandDlgStatus", false);
CPersistentBool gHideCommandDlgFileAction("P_HideCommandDlgFileAction", false);
CPersistentBool gHideCommandDlgImportScan("P_HideCommandDlgImportScan", false);
CPersistentBool gHideCommandDlgRemove("P_HideCommandDlgRemove", false);

static CPersistentBool gHideCommandDlgUseShift("P_HideCommandDlgUseShift", true);
static CPersistentBool gCtrlPressedOnOK("P_CtrlPressedOnOK", true);

CKeyString gWhichCvs(20, "P_WhichCvs");
static CPersistentBool gUseAltCvs("P_UseAltCvs", false, kAddSettings);
#if TARGET_RT_MAC_MACHO
static CPersistentBool gUseCvsnt("P_UseCvsnt", true); 
static bool gIsInternalCvsnt = false;
#endif
static CPersistentString gSettingsFolder("P_SettingsFolder", 0L);
static CPersistentInt gShowCvs("P_ShowCvs", SHOWCVS_HIDE, kAddSettings);
static CPersistentBool gEnableCommandLineLimit("P_EnableCommandLineLimit", true);
static CPersistentInt gCommandLineLimit("P_CommandLineLimit", 32000); /*!< CreateProcess limit is 32767(Unicode string limit), cmd.exe has limit 8192, ShellExecute and ShellExecuteEx INTERNET_MAX_URL_LENGTH (~2048) on NT and MAX_PATH on Win95 */

static CPersistentString gTclLibrary("P_TclLibrary", 0L);
static CPersistentString gPythonLibrary("P_PythonLibrary", 0L);
static CPersistentString gMacrosFolder("P_MacrosFolder", 0L);
static CPersistentString gPythonLibFolder("P_PythonLibFolder", 0L);

#ifdef WIN32
CHECKOUT_FILEATTRIBUTE_TYPE CvsPrefs::CheckoutFileAttribute(void) const {return (CHECKOUT_FILEATTRIBUTE_TYPE)(int)gCheckoutFileAttribute;}
CVSMESSAGES_TYPE CvsPrefs::CvsMessages(void) const {return (CVSMESSAGES_TYPE)(int)gCvsMessages;}
#else
bool CvsPrefs::CheckoutRO(void) const {return gCheckoutRO;}
bool CvsPrefs::QuietOption(void) const {return gQuietOption;}
#endif

bool CvsPrefs::Z9Option(void) const {return gZ9Option;}
bool CvsPrefs::PruneOption(void) const {return gPruneOption;}
ISO8559 CvsPrefs::IsoConvert(void) const {return (ISO8559)(int)gIsoConvert;}
bool CvsPrefs::ICTextOption(void) const {return gICTextOption;}
bool CvsPrefs::AddControl(void) const {return gAddBinControl;}
bool CvsPrefs::MacBinEncodeBinariesOnly(void) const {return gMacBinEncodeOnlyBinaryFiles;}
MACENCODING CvsPrefs::MacBinEncoding(void) const {return (MACENCODING)(int)gMacBinEncoding;}

#if INTERNAL_AUTHEN
int CvsPrefs::KserverPort(void) const {return gKserverPort;}
int CvsPrefs::PserverPort(void) const {return gPserverPort;}
int CvsPrefs::RhostPort(void) const {return gRhostPort;}
const char* CvsPrefs::ServerName(void) const {return gServerName.empty() ? 0L : (const char*)gServerName.c_str();}
const char* CvsPrefs::RshName(void) const {return gRshName.empty() ? 0L : (const char*)gRshName.c_str();}
#endif /*# INTERNAL_AUTHEN */

bool CvsPrefs::MatchEntriesCase(void) const {return gMatchEntriesCase;}
bool CvsPrefs::SmartSorting(void) const {return gSmartSorting;}

#ifndef WIN32
bool CvsPrefs::UseViewerAlways(void) const {return gUseViewerAlways;}
#endif

bool CvsPrefs::EncryptCommunication(void) const {return gEncryptCommunication;}

const char* CvsPrefs::Home(void) const
{
#if !qUnix && !TARGET_RT_MAC_MACHO
#	ifdef WIN32
	// Windows 98 doesn't like C:\/.cvspass
	TrimRight(gHome, kPathDelimiter);
#	endif
	return gHome.c_str();
#else /* qUnix || TARGET_RT_MAC_MACHO */
	static string home;
	if (!home.empty())
		return home.c_str();

	char *env = getenv ("HOME");
	struct passwd *pw;
	if (env)
		home = env;
	else if ((pw = (struct passwd *) getpwuid (getuid ()))
			 && pw->pw_dir)
		home = pw->pw_dir;
	else
		return 0L;

	return home.c_str();
#endif /* qUnix ||ÊTARGET_RT_MAC_MACHO */
}

#ifdef qUnix
const char* CvsPrefs::ConsoleFont(void) const {return gConsoleFont.empty() ? 0L : (const char*)gConsoleFont.c_str();}
#endif

#if DIRTY_MCP_SUPPORT
bool CvsPrefs::DirtySupport(void) const {return gDirtySupport;}
#endif

const char* CvsPrefs::Viewer(void) const {return gViewer.c_str();}
const char* CvsPrefs::ConflictEditor(void) const {return gConflictEditor.c_str();}
const char* CvsPrefs::AnnotateViewer(void) const {return gAnnotateViewer.c_str();}
const char* CvsPrefs::Helper(void) const {return gHelper.c_str();}
const char* CvsPrefs::Browser(void) const {return gBrowser.c_str();}
const char* CvsPrefs::ExtDiff(void) const {return gExtdiff.c_str();}

int CvsPrefs::LogoutTimeOut(void) const {return gLogoutTimeOut;}
int CvsPrefs::RefreshInactiveTimeOut(void) const {return gRefreshInactiveTimeOut;}

#if INTERNAL_AUTHEN
bool CvsPrefs::AlwaysUseCvsroot(void) const {return gAlwaysUseCvsroot;}
#endif

const char* CvsPrefs::ProxyHost(void) const {return gProxyHost.c_str();}
int CvsPrefs::ProxyPort(void) const {return gProxyPort;}
bool CvsPrefs::UseProxy(void) const {return gUseProxy;}
bool CvsPrefs::UnixLF(void) const {return gUnixLF;}
bool CvsPrefs::MacLF(void) const {return gMacLF;}
int CvsPrefs::ZLevel(void) const {return gZLevel;}
int CvsPrefs::CvsVersion(void) const {return gCvsVersion;}
bool CvsPrefs::CvsConsole(void) const {return gCvsConsole;}
int CvsPrefs::WordWrapLogMsg(void) const {return gWordWrapLogMsg;}
SHELL_KIND CvsPrefs::GetUseShell(void) const {return (SHELL_KIND)(int)gUseShell;}
bool CvsPrefs::DisableSplashScreen(void) const {return gDisableSplashScreen;}

STATUSOUTPUT_TYPE CvsPrefs::StatusOutputType(void) const {return (STATUSOUTPUT_TYPE)(int)gStatusOutputType;}
FILEACTION_TYPE CvsPrefs::FileActionType(void) const {return (FILEACTION_TYPE)(int)gFileActionType;}
bool CvsPrefs::UpdateBuildDirectories(void) const {return gUpdateBuildDirectories;}
bool CvsPrefs::UpdateCaseSensitiveNames(void) const {return gUpdateCaseSensitiveNames;}
bool CvsPrefs::UpdateLastCheckinTime(void) const {return gUpdateLastCheckinTime;}
bool CvsPrefs::UpdateThreeWayConflicts(void) const {return gUpdateThreeWayConflicts;}

const char* CvsPrefs::SettingsFolder(void) const {return gSettingsFolder.c_str();}
const char* CvsPrefs::TclLibrary(void) const {return gTclLibrary.c_str();}
const char* CvsPrefs::PythonLibrary(void) const {return gPythonLibrary.c_str();}
const char* CvsPrefs::MacrosFolder(void) const {return gMacrosFolder.c_str();};
const char* CvsPrefs::PythonLibFolder(void) const {return gPythonLibFolder.c_str();};

/* Return the current working dir (last working dir).   */
/* This directory is automatically selected when WinCVS */
/* is opened the next time. */
const char* CvsPrefs::LastWorkingDir(void) const
{
	return gLastWorkingDir.c_str();
}

bool CvsPrefs::HideCommandDlgUpdate(void) const {return gHideCommandDlgUpdate;}
bool CvsPrefs::HideCommandDlgQueryUpdate(void) const {return gHideCommandDlgQueryUpdate;}
bool CvsPrefs::HideCommandDlgDiff(void) const {return gHideCommandDlgDiff;}
bool CvsPrefs::HideCommandDlgLog(void) const {return gHideCommandDlgLog;}
bool CvsPrefs::HideCommandDlgGraph(void) const {return gHideCommandDlgGraph;}
bool CvsPrefs::HideCommandDlgAnnotate(void) const {return gHideCommandDlgAnnotate;}
bool CvsPrefs::HideCommandDlgStatus(void) const {return gHideCommandDlgStatus;}
bool CvsPrefs::HideCommandDlgFileAction(void) const {return gHideCommandDlgFileAction;}
bool CvsPrefs::HideCommandDlgImportScan(void) const {return gHideCommandDlgImportScan;}
bool CvsPrefs::HideCommandDlgRemove(void) const {return gHideCommandDlgRemove;}

bool CvsPrefs::HideCommandDlgUseShift(void) const {return gHideCommandDlgUseShift;}
bool CvsPrefs::CtrlPressedOnOK(void) const {return gCtrlPressedOnOK;}

#ifdef WIN32
void CvsPrefs::SetCheckoutFileAttribute(CHECKOUT_FILEATTRIBUTE_TYPE newState) {gCheckoutFileAttribute = newState;}
void CvsPrefs::SetCvsMessages(CVSMESSAGES_TYPE newState) {gCvsMessages = newState;}
#else
void CvsPrefs::SetCheckoutRO(bool newState) {gCheckoutRO = newState;}
void CvsPrefs::SetQuietOption(bool newState) {gQuietOption = newState;}
#endif

void CvsPrefs::SetZ9Option(bool newState) {gZ9Option = newState;}
void CvsPrefs::SetPruneOption(bool newState) {gPruneOption = newState;}
void CvsPrefs::SetIsoConvert(ISO8559 newState) {gIsoConvert = (int)newState;}
void CvsPrefs::SetICTextOption(bool newState) {gICTextOption = newState;}
void CvsPrefs::SetAddControl(bool newState) {gAddBinControl = newState;}
void CvsPrefs::SetMacBinEncodeBinariesOnly(bool newState) {gMacBinEncodeOnlyBinaryFiles = newState;}
void CvsPrefs::SetMacBinEncoding(MACENCODING newState) {gMacBinEncoding = (int)newState;}

#if INTERNAL_AUTHEN
void CvsPrefs::SetKserverPort(int newState) {gKserverPort = newState;}
void CvsPrefs::SetPserverPort(int newState) {gPserverPort = newState;}
void CvsPrefs::SetRhostPort(int newState) {gRhostPort = newState;}
void CvsPrefs::SetServerName(const char* newservername) {gServerName = newservername;}
void CvsPrefs::SetRshName(const char* newrshname) {gRshName = newrshname;}
#endif /*# INTERNAL_AUTHEN */

void CvsPrefs::SetEncryptCommunication(bool newState) {gEncryptCommunication = newState;}
void CvsPrefs::SetMatchEntriesCase(bool newState) {gMatchEntriesCase = newState;}
void CvsPrefs::SetSmartSorting(bool newState) {gSmartSorting = newState;}

#ifndef WIN32
void CvsPrefs::SetUseViewerAlways(bool newState) {gUseViewerAlways = newState;}
#endif

void CvsPrefs::SetHome(const char* newhome)
{
#if !qUnix && !TARGET_RT_MAC_MACHO
	gHome = newhome;
#	ifdef WIN32
	// Windows 98 doesn't like C:\/.cvspass
	TrimRight(gHome, kPathDelimiter);
#	endif
#endif /* !qUnix && !TARGET_RT_MAC_MACHO */
}

#ifdef qUnix
void CvsPrefs::SetConsoleFont(const char* newFont) {gConsoleFont = newFont;}
#endif

#if DIRTY_MCP_SUPPORT
void CvsPrefs::SetDirtySupport(bool newState) {gDirtySupport = newState;}
#endif

void CvsPrefs::SetViewer(const char* newViewer) {gViewer = newViewer;}
void CvsPrefs::SetConflictEditor(const char* newConflictEditor) {gConflictEditor = newConflictEditor;}
void CvsPrefs::SetAnnotateViewer(const char* newAnnotateViewer) {gAnnotateViewer = newAnnotateViewer;}
void CvsPrefs::SetBrowser(const char* newBrowser) {gBrowser = newBrowser;}
void CvsPrefs::SetHelper(const char* newHelper) {gHelper = newHelper;}
void CvsPrefs::SetExtDiff(const char* newDiff) {gExtdiff = newDiff;}

void CvsPrefs::SetLogoutTimeOut(int newTimeout) {gLogoutTimeOut = newTimeout;}
void CvsPrefs::SetRefreshInactiveTimeOut(int newTimeout) {gRefreshInactiveTimeOut = newTimeout;}

#if INTERNAL_AUTHEN
void CvsPrefs::SetAlwaysUseCvsroot(bool newState) {gAlwaysUseCvsroot = newState;}
#endif

void CvsPrefs::SetUseProxy(bool useit) {gUseProxy = useit;}
void CvsPrefs::SetUnixLF(bool useit) {gUnixLF = useit;}
void CvsPrefs::SetMacLF(bool useit) {gMacLF = useit;}
void CvsPrefs::SetZLevel(int level) {gZLevel = level;}
void CvsPrefs::SetProxyHost(const char* newhost) {gProxyHost = newhost;}
void CvsPrefs::SetProxyPort(int newport) {gProxyPort = newport;}
void CvsPrefs::SetCvsVersion(int vers) {gCvsVersion = vers;}
void CvsPrefs::SetCvsConsole(bool console) {gCvsConsole = console;}
void CvsPrefs::SetWordWrapLogMsg(int wordwrap) {gWordWrapLogMsg = wordwrap;}
void CvsPrefs::SetLastWorkingDir(const char* workdir) {gLastWorkingDir = workdir;}
void CvsPrefs::SetUseShell(SHELL_KIND shell) {gUseShell = shell;}
void CvsPrefs::SetDisableSplashScreen(bool newState) {gDisableSplashScreen = newState;}

void CvsPrefs::SetStatusOutputType(STATUSOUTPUT_TYPE outputType) {gStatusOutputType = outputType;}
void CvsPrefs::SetFileActionType(FILEACTION_TYPE fileActionType) {gFileActionType = fileActionType;}
void CvsPrefs::SetUpdateBuildDirectories(bool newState) {gUpdateBuildDirectories = newState;}
void CvsPrefs::SetUpdateCaseSensitiveNames(bool newState) {gUpdateCaseSensitiveNames = newState;}
void CvsPrefs::SetUpdateLastCheckinTime(bool newState) {gUpdateLastCheckinTime = newState;}
void CvsPrefs::SetUpdateThreeWayConflicts(bool newState) {gUpdateThreeWayConflicts = newState;}

void CvsPrefs::SetSettingsFolder(const char* settingsFolder) {gSettingsFolder = settingsFolder;}
void CvsPrefs::SetTclLibrary(const char* tclLibrary) {gTclLibrary = tclLibrary;}
void CvsPrefs::SetPythonLibrary(const char* pythonLibrary) {gPythonLibrary = pythonLibrary;}
void CvsPrefs::SetMacrosFolder(const char* macrosFolder) {gMacrosFolder = macrosFolder;}
void CvsPrefs::SetPythonLibFolder(const char* pythonLibFolder) {gPythonLibFolder = pythonLibFolder;}

void CvsPrefs::SetHideCommandDlgUpdate(bool newState) {gHideCommandDlgUpdate = newState;}
void CvsPrefs::SetHideCommandDlgQueryUpdate(bool newState) {gHideCommandDlgQueryUpdate = newState;}
void CvsPrefs::SetHideCommandDlgDiff(bool newState) {gHideCommandDlgDiff = newState;}
void CvsPrefs::SetHideCommandDlgLog(bool newState) {gHideCommandDlgLog = newState;}
void CvsPrefs::SetHideCommandDlgGraph(bool newState) {gHideCommandDlgGraph = newState;}
void CvsPrefs::SetHideCommandDlgAnnotate(bool newState) {gHideCommandDlgAnnotate = newState;}
void CvsPrefs::SetHideCommandDlgStatus(bool newState) {gHideCommandDlgStatus = newState;}
void CvsPrefs::SetHideCommandDlgFileAction(bool newState) {gHideCommandDlgFileAction = newState;}
void CvsPrefs::SetHideCommandDlgImportScan(bool newState) {gHideCommandDlgImportScan = newState;}
void CvsPrefs::SetHideCommandDlgRemove(bool newState) {gHideCommandDlgRemove = newState;}

void CvsPrefs::SetHideCommandDlgUseShift(bool newState) {gHideCommandDlgUseShift = newState;}
void CvsPrefs::SetCtrlPressedOnOK(bool newState) {gCtrlPressedOnOK = newState;}

#if INTERNAL_AUTHEN
bool CvsPrefs::empty(void) const  {	return gCvsroot.empty();  }
int  CvsPrefs::length(void) const {	return gCvsroot.length(); }

CvsPrefs::operator const char*() const {	return (const char*)gCvsroot.c_str(); }
CvsPrefs::operator const unsigned char *() const {return (const unsigned char *)gCvsroot.c_str();}
const char* CvsPrefs::operator=(const char*newstr) {	gCvsroot = newstr; return gCvsroot.c_str(); }
const unsigned char *CvsPrefs::operator=(const unsigned char *newstr) {	gCvsroot = (const char*) newstr; return (const unsigned char *)gCvsroot.c_str(); }
#endif /* INTERNAL_AUTHEN */

void CvsPrefs::save(bool quiet /*= false*/)
{
#if INTERNAL_AUTHEN
	if( empty() )
		return;
#endif /* INTERNAL_AUTHEN */

	if( SCHEMA_CURRENT != gPersistentShema )
	{
		gPersistentShema = SCHEMA_CURRENT;
	}

	bool needSave = CPersistent::NeedSave();
	bool needSaveSettings = CPersistent::NeedSave(true);
	if( !needSave )
		return;

	CPersistent::SaveAll();

	if( !needSaveSettings )
		return;

#ifdef WIN32
	{
		extern CPersistentString gOldLoc;
		SavePersistentSettings(gOldLoc.c_str(), quiet);
	}
#endif
#ifdef qMacCvsPP
	CMacCvsApp::SavePersistentSettings(quiet);
#endif /* qMacCvsPP */
#if qUnix
	UCvsFolders *browser = UCvsApp::gApp->GetBrowserView();
	if(browser != 0L && browser->GetRoot() != 0L)
		SavePersistentSettings(browser->GetRoot(), quiet);
#endif
}

void CvsPrefs::load(void)
{
	bool loaded = CPersistent::LoadAll();

	if( loaded )
	{
		if( NeedCvsrootMigration() )
		{
			MigrateCvsrootList();
		}

		return;
	}

	// backward compatibility
#ifdef WIN32
	UINT value;
	
	CString cstr;
#if INTERNAL_AUTHEN
	cstr = AfxGetApp()->GetProfileString(PROFILE_NAME, "CVSROOT");
	if(!cstr.IsEmpty())
		*this = (const char*)cstr;
#endif /* INTERNAL_AUTHEN */

	if((value = AfxGetApp()->GetProfileInt(PROFILE_NAME, "CheckoutRO", -1)) != -1)
		SetCheckoutFileAttribute(value ? CHECKOUT_READONLY : CHECKOUT_DEFAULT);
	if((value = AfxGetApp()->GetProfileInt(PROFILE_NAME, "Z9Option", -1)) != -1)
		SetZ9Option(value ? 1 : 0);
	if((value = AfxGetApp()->GetProfileInt(PROFILE_NAME, "QuietOption", -1)) != -1)
		SetCvsMessages(value ? CVSMESSAGES_QUIET : CVSMESSAGES_DEFAULT);
	if((value = AfxGetApp()->GetProfileInt(PROFILE_NAME, "PruneOption", -1)) != -1)
		SetPruneOption(value ? 1 : 0);

#if INTERNAL_AUTHEN
	if((value = AfxGetApp()->GetProfileInt(PROFILE_NAME, "Authentication", -1)) != -1)
		gAuthen.setkind((AuthenKind)value);
#endif /* INTERNAL_AUTHEN */

	if((value = AfxGetApp()->GetProfileInt(PROFILE_NAME, "AddControl", -1)) != -1)
		SetAddControl(value ? 1 : 0);

	char tmp[20];
	for(int i = 0; i < NUM_CVSROOT; i++)
	{
		sprintf(tmp, "CVSROOTLIST%d", i);
		cstr = AfxGetApp()->GetProfileString(PROFILE_NAME, tmp);
		if(!cstr.IsEmpty())
			gOldCvsroots.Insert((const char*)cstr);
	}

#endif /* WIN32 */

}

/*!
	Check whether there is a need for the cvsroot history migration
	\return true if migration needed, false otherwise
	\note Implementation is platform-specific
*/
bool CvsPrefs::NeedCvsrootMigration()
{
	bool res = false;

#ifdef WIN32
	if( SCHEMA_0 == gPersistentShema )
	{
		res = true;
	}
#endif /* WIN32 */

#if qUnix
	if( SCHEMA_0 == gPersistentShema || SCHEMA_1 == gPersistentShema )
	{
		res = true;
	}
#endif // qUnix
	
#if qMacCvsPP
#endif // qMacCvsPP

	return res;
}

/*!
	Find the best CVS root match for a given path
	\param path Path to find the match for
	\param lastCvsroot Return last CVS root
	\return true if match was found, false otherwise
*/
bool FindBestLastCvsroot(const char* path, std::string& lastCvsroot)
{
	bool res = false;

	string bestCvsrootPath;
	string bestCvsroot;

	const CMString::list_t& list = gLastCvsroot.GetList();
	for(CMString::list_t::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		string key;
		string value;
		
		gLastCvsroot.Split(i->c_str(), key, value);

		if( StrPathCmp(key.c_str(), path) == 0 )
		{
			lastCvsroot = value;
			res = true;
			break;
		}
		
		string strKey = key;
		string strPath = path;

		strKey.erase(strKey.find_last_not_of(kPathDelimiter) + 1, string::npos);
		strPath.erase(strPath.find_last_not_of(kPathDelimiter) + 1, string::npos);
		
		transform(strKey.begin(), strKey.end(), strKey.begin(), tolower);
		transform(strPath.begin(), strPath.end(), strPath.begin(), tolower);

		if( strPath.find(strKey) != string::npos && strKey.length() > bestCvsrootPath.length() )
		{
			bestCvsrootPath = key;
			bestCvsroot = value;
		}
	}

	if( !res && !bestCvsroot.empty() )
	{
		lastCvsroot = bestCvsroot;
		res = true;
	}

	return res;
}

/*!
	Insert the mapping of last CVS root for a given path
	\param path Path
	\param cvsroot Last CVS root
*/
void InsertLastCvsroot(const char* path, const char* cvsroot)
{
	string mapValue;
	gLastCvsroot.Concatenate(mapValue, path, cvsroot);
	gLastCvsroot.Insert(mapValue.c_str());
}

/*!
	Find the best module name match for a given path
	\param path Path to find the match for
	\param lastModuleName Return last module name
	\return true if match was found, false otherwise
*/
bool FindBestLastModuleName(const char* path, std::string& lastModuleName)
{
	bool res = false;

	string bestModuleNamePath;
	string bestModuleName;

	const CMString::list_t& list = gLastModuleName.GetList();
	for(CMString::list_t::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		string key;
		string value;
		
		gLastModuleName.Split(i->c_str(), key, value);
		if( StrPathCmp(key.c_str(), path) == 0 )
		{
			lastModuleName = value;
			res = true;
			break;
		}
		
		string strKey = key;
		string strPath = path;

		strKey.erase(strKey.find_last_not_of(kPathDelimiter) + 1, string::npos);
		strPath.erase(strPath.find_last_not_of(kPathDelimiter) + 1, string::npos);
		
		transform(strKey.begin(), strKey.end(), strKey.begin(), tolower);
		transform(strPath.begin(), strPath.end(), strPath.begin(), tolower);

		if( strPath.find(strKey) != string::npos && strKey.length() > bestModuleName.length() )
		{
			bestModuleNamePath = key;
			bestModuleName = value;
		}
	}

	if( !res && !bestModuleName.empty() )
	{
		lastModuleName = bestModuleName;
		res = true;
	}

	return res;
}

/*!
	Insert the mapping of last module name for a given path
	\param path Path
	\param moduleName Last module name
*/
void InsertLastModuleName(const char* path, const char* moduleName)
{
	string mapValue;
	gLastModuleName.Concatenate(mapValue, path, moduleName);
	gLastModuleName.Insert(mapValue.c_str());
}

/*!
	Helper method to migrate into a single CMString for cvsroot history
*/
void MigrateCvsrootList()
{
	for(int nIndex = 0; nIndex <= NUM_CVSROOT; nIndex++)
	{
		string cvsroot;
		char tmp[20];
		sprintf(tmp, "P_Cvsroot%d", nIndex);
		
#ifdef WIN32
		BYTE* ppData;
		UINT pBytes = 0;

		if( !AfxGetApp()->GetProfileBinary(PROFILE_NAME, tmp, &ppData, &pBytes) )
		{
			continue;
		}

		cvsroot = (char*)ppData;
		delete ppData;
#endif /* WIN32 */
		
#if qUnix
		/* remove unused entries */
		gOldCvsroots.Remove(tmp);
#endif // qUnix
	
#if qMacCvsPP
#endif // qMacCvsPP
		
		if( !cvsroot.empty() )
		{
			gOldCvsroots.Insert(cvsroot.c_str());
		}
	}
}

#ifdef WIN32
kWinVersion CvsPrefs::WhichVersion()
{
	DWORD dwVersion = GetVersion(); // Get major and minor version numbers of Windows
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	DWORD dwBuild;

	// Get build numbers for Windows NT or Win32s
	if (dwVersion < 0x80000000)                // Windows NT
	{
		dwBuild = (DWORD)(HIWORD(dwVersion));
		return dwWindowsMajorVersion == 3 ? kWinNT35 : kWinNT4;
	}
	else if (dwWindowsMajorVersion < 4)        // Win16s
	{
		dwBuild = (DWORD)(HIWORD(dwVersion));
		return kWin16;
	}
	
	// Windows 95 -- No build numbers provided    dwBuild =  0;
	return kWin32;
}
#endif /* WIN32 */


//
// TODO: If we sometimes implement multi-threaded CVS/TCL macro 
// execution then we have to implement thread synchronization in
// IsXXXRunning and SetXXXRunning methods.
//
// NOTE! IsCvsRunning and SetCvsRunning methods are not yet
// used in Mac/Unix/WinCVS. Each of these version have had
// their own app.gCvsRunning variable. I haven't cleaned up
// the code and changed them to use this "one and only" 
// CvsRunning methods. 
//
bool CvsPrefs::IsCvsRunning(void) const
{ 
	return(fCvsRunningCounter > 0); 
}

int CvsPrefs::SetCvsRunning(bool launched)  
{ 
	if(launched) 
		fCvsRunningCounter++;
	else if(fCvsRunningCounter > 0) 
		fCvsRunningCounter--;

	return fCvsRunningCounter;		
}

bool CvsPrefs::IsTclFileRunning(void) const
{ 
	return(fTclFileRunningCounter > 0); 
}

int CvsPrefs::SetTclFileRunning(bool launched)  
{ 
	if(launched) 
		fTclFileRunningCounter++;
	else if(fTclFileRunningCounter > 0) 
		fTclFileRunningCounter--;

	return fTclFileRunningCounter;		
}

bool CvsPrefs::UseAltCvs(void) const
{
	return gUseAltCvs;
}

void CvsPrefs::SetUseAltCvs(bool useit)
{
	gUseAltCvs = useit;
}

#if TARGET_RT_MAC_MACHO
bool CvsPrefs::UseCvsnt(void) const
{
  return gUseCvsnt;
}

void CvsPrefs::SetUseCvsnt(bool useIt)
{
  gUseCvsnt = useIt;
}

bool CvsPrefs::IsInternalCvsnt() const
{
  return gIsInternalCvsnt;
}
#endif

/*!
	Set CVS executable path
	\param cvsPath Path to CVS executable
*/
void CvsPrefs::SetWhichCvs(const char* cvsPath)
{
	string appPath;

#ifdef WIN32
	::GetAppModule(appPath);

	CString strAppPath(appPath.c_str());
	strAppPath.MakeLower();
	appPath = strAppPath;
#elif qUnix
	appPath += EXECLIB;
	appPath += "/cvs";
#elif TARGET_RT_MAC_MACHO
	appPath = CMacCvsApp::gApp->GetAppPath();
#endif

	if( !appPath.empty() ) 
	{
		string value;
		gWhichCvs.Concatenate(value, appPath.c_str(), cvsPath);
		gWhichCvs.Insert(value.c_str());
	} 
	else
	{
		gWhichCvs.Insert(cvsPath);
	}

#ifdef WIN32
	// Print the warning if alternate CVS is not directly accessible
	if( cvsPath && strlen(cvsPath) )
	{
		struct stat sb;
		if( !(stat(cvsPath, &sb) != -1 && S_ISREG(sb.st_mode)) )
		{
			CColorConsole out;
			out << kRed << kBold << "Warning: " 
				<< kMagenta << "Alternate CVS executable not accessible, make sure that the file exists or it's in the PATH" 
				<< kNormal << kNL;
		}
	}
#endif
}

#ifdef WIN32
/*!
	Format CVS path for a given location
	\param folder CVS binary location, NULL to get CVS filename only
	\param addFolder true to add sub-directory to the folder, false to only add executable name
	\return CVS path
	\note CVS filename differs on different platforms
*/
std::string FormatCvsPath(const char* folder = NULL, const bool addFolder = true)
{
	string path;
		
	if( folder )
	{
		path = folder;
	}

	if( !path.empty() )
	{
		NormalizeFolderPath(path);
		
		if( addFolder )
		{
			path += "cvsnt";
			NormalizeFolderPath(path);
		}
	}

	path += gCvsPrefs.WhichVersion() == kWin32 ? "cvs95.exe" : "cvs.exe";

	return path;
}
#endif

/*!
	Get CVS executable path
	\param path Return the CVS path
	\param userOnly true to prevent searching for CVS if it's not explicitly specified
	\return CVS executable path
*/
const char* CvsPrefs::WhichCvs(std::string& path, bool userOnly) const
{
	path.erase();

	string appPath;

#ifdef WIN32
	::GetAppModule(appPath);
	
	CString strAppPath(appPath.c_str());
	strAppPath.MakeLower();
	appPath = strAppPath;
#elif qUnix
	appPath += EXECLIB;
	appPath += "/cvs";
#elif TARGET_RT_MAC_MACHO
	appPath = CMacCvsApp::gApp->GetAppPath();
	gIsInternalCvsnt = false;
#endif

	// search if beside the default cvs, the user defined one to use with this application only
	string candidate;

	if( !appPath.empty() )
	{
		string value;
		gWhichCvs.Concatenate(value, appPath.c_str(), "");
		CMString::list_t::const_iterator i = gWhichCvs.Find(value.c_str());
		if( i != gWhichCvs.end() )
		{
			string key;
			gWhichCvs.Split(i->c_str(), key, candidate);
		}
	}

#ifndef WIN32
	// verify this cvs exe is still there
	struct stat sb;
	if( !candidate.empty() && stat(candidate.c_str(), &sb) != -1 && S_ISREG(sb.st_mode) )
	{
		path = candidate;
		return path.c_str();
	}
#else
	if( !candidate.empty() )
	{
		path = candidate;
		return path.c_str();
	}
#endif

	// fall back to the default one.
	if( userOnly )
		return 0L;

#ifdef WIN32
	// Try the registry first
	HKEY hKey;
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"Software\\CVS\\PServer",
		0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS )
	{
		char cvsntPath[_MAX_PATH] = "";
		DWORD pathSize = _MAX_PATH;

		if( RegQueryValueEx(hKey, "InstallPath", NULL, NULL, (LPBYTE)cvsntPath, &pathSize) == ERROR_SUCCESS )
		{
			path = FormatCvsPath(cvsntPath, false);
			
			struct stat sb;
			if( stat(path.c_str(), &sb) != -1 && S_ISREG(sb.st_mode) )
			{
				return path.c_str();
			}
		}

		RegCloseKey(hKey);
	}

	// Check in the <Program Files>\cvsnt folder
	if( GetFolder(path, CSIDL_PROGRAM_FILES) )
	{
		path = FormatCvsPath(path.c_str());

		struct stat sb;
		if( stat(path.c_str(), &sb) != -1 && S_ISREG(sb.st_mode) )
		{
			return path.c_str();
		}
	}

	// Check in the <WinCvs>\cvsnt folder
	if( ::GetAppPath(path) )
	{
		path = FormatCvsPath(path.c_str());
		
		struct stat sb;
		if( stat(path.c_str(), &sb) != -1 && S_ISREG(sb.st_mode) )
		{
			return path.c_str();
		}
	}

	// If all the above failed then try the plain executable - it may be present in the PATH
	path = FormatCvsPath();

#elif qUnix
	if( path.empty() )
		path = EXECLIB;

	NormalizeFolderPath(path);

	path += "cvs";
#elif TARGET_RT_MAC_MACHO
	path = CMacCvsApp::gApp->GetAppResourcesPath();
	
	NormalizeFolderPath(path);

	if ( gUseCvsnt ) {
  	gIsInternalCvsnt = true;
  	path += "cvsnt";
  	path += kPathDelimiter;
  	path += "cvsnt";
	}
	else {
	}
	else {
	}
	else {
	  path += "cvsgui";
	}
#endif

	return path.c_str();
}

SHOWCVS_TYPE CvsPrefs::ShowCvs(void) const
{
	return (SHOWCVS_TYPE)(int)gShowCvs;
}

void CvsPrefs::SetShowCvs(SHOWCVS_TYPE showCvs)
{
	gShowCvs = showCvs;
}

bool CvsPrefs::EnableCommandLineLimit(void) const
{
	return gEnableCommandLineLimit;
}

void CvsPrefs::SetEnableCommandLineLimit(bool enableCommandLineLimit)
{
	gEnableCommandLineLimit = enableCommandLineLimit;
}

int CvsPrefs::CommandLineLimit(void) const
{
	return gCommandLineLimit;
}

void CvsPrefs::SetCommandLineLimit(int commandLineLimit)
{
	gCommandLineLimit = commandLineLimit;
}
