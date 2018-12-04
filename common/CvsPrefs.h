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
 * CvsPrefs.h --- class to handle CVS settings
 */

#ifndef CVSPREFS_H
#define CVSPREFS_H

#include <string.h>
#include "ustr.h"

#ifdef TARGET_OS_MAC
#	include <Files.h>
#endif /* TARGET_OS_MAC */

#ifdef WIN32
#	define DIRTY_MCP_SUPPORT	0
#else
#	define DIRTY_MCP_SUPPORT	1
#endif

/// Schema enum
typedef enum
{
	SCHEMA_0,					/*!< Default */
	SCHEMA_1,					/*!< WinCvs 1.3b7 and greater */
	SCHEMA_2,					/*!< gCvs 1.1 and greater */
	SCHEMA_CURRENT = SCHEMA_2	/*!< Current schema */
} SCHEMA_NUMBER;

/// ISO8559
typedef enum
{
	ISO8559_none,			/*!< No conversion */
	ISO8559_1,				/*!< Latin1 */
	ISO8559_1_MacCvsPro,	/*!< Latin1 */
	ISO8559_2,				/*!< Latin2 */
	ISO8559_3,				/*!< Latin3 */
	ISO8559_4,				/*!< Latin4 */
	ISO8559_5,				/*!< Cyrillic */
	ISO8559_6,				/*!< Arabic */
	ISO8559_7,				/*!< Greek */
	ISO8559_8,				/*!< Hebrew */
	ISO8559_9,				/*!< Latin5 (Turkish) */
	ISO8559_10				/*!< Latin6 (Nordic) */
} ISO8559;

/// Mac encoding
typedef enum
{
	MAC_HQX,			/*!< HQX (7bits) */
	MAC_APPSINGLE,		/*!< Apple Single */
	MAC_PLAIN_BINARY	/*!< Plain binary without resource fork */
} MACENCODING;

/// Shell type
typedef enum
{
	SHELL_TCL,		/*!< TCL */
	SHELL_PYTHON	/*!< Python */
} SHELL_KIND;

/// Status output type
typedef enum
{
	STATUSOUTPUT_STANDARD,	/*!< Standard */
	STATUSOUTPUT_QUICK,		/*!< Quick (-q) */
	STATUSOUTPUT_VERBOSE	/*!< Verbose (-v) */
} STATUSOUTPUT_TYPE;

/// kLaunchType
typedef int FILEACTION_TYPE;

/// Show CVS window type
typedef enum
{
	SHOWCVS_HIDE,			/*!< Hide */
	SHOWCVS_SHOWNORMAL,		/*!< Show normal and activate */
	SHOWCVS_SHOWNOACTIVATE,	/*!< Show and don't activate */
	SHOWCVS_SHOWMINNOACTIVE	/*!< Show as icon and don't activate */
} SHOWCVS_TYPE;

/// Checkout file attribute
typedef enum
{
	CHECKOUT_DEFAULT,	/*!< Default - writable unless watches are set */
	CHECKOUT_READONLY,	/*!< Always read-only */
	CHECKOUT_WRITABLE	/*!< Read-write */
} CHECKOUT_FILEATTRIBUTE_TYPE;

/// CVS messages
typedef enum
{
	CVSMESSAGES_DEFAULT,		/*!< Default - full messages */
	CVSMESSAGES_QUIET,			/*!< Somewhat quiet */
	CVSMESSAGES_REALYYQUIET	/*!< Really quiet */
} CVSMESSAGES_TYPE;

#define NUM_CVSROOT 20

#ifdef WIN32
	/// Windows version enum
	typedef enum
	{
		kWin16,		/*!< Win3.1 */
		kWinNT35,	/*!< NT3.5 */
		kWinNT4,	/*!< NT4+ */
		kWin32		/*!< Win9x */
	} kWinVersion;
#endif /* WIN32 */


class CMString;
class CKeyString;

extern CMString gOldCvsroots;

bool FindBestLastCvsroot(const char* path, std::string& lastCvsroot);
void InsertLastCvsroot(const char* path, const char* cvsroot);

bool FindBestLastModuleName(const char* path, std::string& lastModuleName);
void InsertLastModuleName(const char* path, const char* moduleName);

void MigrateCvsrootList();

/// Preferences
class CvsPrefs
{
	
public :
	inline CvsPrefs() 
		: fHasLoggedIn(false), fCvsRunningCounter(0), fTclFileRunningCounter(0) 
	{
	}

	inline ~CvsPrefs()
	{
	}

#if INTERNAL_AUTHEN
	// CVSROOT
	bool empty(void) const;
	int length(void) const;

	const char* operator=(const char* newstr);
		// set to a new C String (0L is OK)
	const unsigned char *operator=(const unsigned char* newstr);
		// set to a new P String (0L is OK)

	operator const char*() const;
		// as a C string
	operator const unsigned char*() const;
		// as a P string
#endif /* INTERNAL_AUTHEN */

	// misc. options
#if DIRTY_MCP_SUPPORT
	bool DirtySupport(void) const;
#endif

#ifdef WIN32
	CHECKOUT_FILEATTRIBUTE_TYPE CheckoutFileAttribute(void) const;
	CVSMESSAGES_TYPE CvsMessages(void) const;
#else
	bool CheckoutRO(void) const;
	bool QuietOption(void) const;
#endif

	bool Z9Option(void) const;
	bool PruneOption(void) const;
	ISO8559 IsoConvert(void) const;
	bool ICTextOption(void) const;
	bool AddControl(void) const;
	MACENCODING MacBinEncoding(void) const;
	bool MacBinEncodeBinariesOnly(void) const;

#if INTERNAL_AUTHEN
	int KserverPort(void) const;
	int PserverPort(void) const;
	int RhostPort(void) const;
	const char* ServerName(void) const;
	const char* RshName(void) const;
#endif /*# INTERNAL_AUTHEN */
	
	bool EncryptCommunication(void) const;
	const char* ConsoleFont(void) const;
	const char* Home(void) const;
	const char* Viewer(void) const;
	const char* ConflictEditor(void) const;
	const char* AnnotateViewer(void) const;
	const char* ExtDiff(void) const;
	const char* Helper(void) const;
	const char* Browser(void) const;
	int LogoutTimeOut(void) const;
	int RefreshInactiveTimeOut(void) const;
#if INTERNAL_AUTHEN
	bool AlwaysUseCvsroot(void) const;
#endif
	bool UseProxy(void) const;
	const char* ProxyHost(void) const;
	int ProxyPort(void) const;
	bool UnixLF(void) const;
	bool MacLF(void) const;
	int ZLevel(void) const;
	int CvsVersion(void) const;
	bool CvsConsole(void) const;
	int WordWrapLogMsg(void) const;
	const char* LastWorkingDir(void) const;
	bool UseKeyChain(void) const; // on mac
	bool MatchEntriesCase(void) const;
	bool SmartSorting(void) const;
	
	bool HideCommandDlgUpdate(void) const;
	bool HideCommandDlgQueryUpdate(void) const;
	bool HideCommandDlgDiff(void) const;
	bool HideCommandDlgLog(void) const;
	bool HideCommandDlgGraph(void) const;
	bool HideCommandDlgAnnotate(void) const;
	bool HideCommandDlgStatus(void) const;
	bool HideCommandDlgFileAction(void) const;
	bool HideCommandDlgImportScan(void) const;
	bool HideCommandDlgRemove(void) const;

	bool HideCommandDlgUseShift(void) const;
	bool CtrlPressedOnOK(void) const;

	SHELL_KIND GetUseShell(void) const;
	const char* getConsoleFont(void) const;
	bool DisableSplashScreen(void) const;

	STATUSOUTPUT_TYPE StatusOutputType(void) const;
	FILEACTION_TYPE FileActionType(void) const;
	bool UpdateBuildDirectories() const;
	bool UpdateCaseSensitiveNames() const;
	bool UpdateLastCheckinTime() const;
	bool UpdateThreeWayConflicts() const;

	const char* SettingsFolder(void) const;
	const char* TclLibrary(void) const;
	const char* PythonLibrary(void) const;
	const char* MacrosFolder(void) const;
	const char* PythonLibFolder(void) const;

#ifndef WIN32
	bool UseViewerAlways(void) const;
	void SetUseViewerAlways(bool newState);
#endif

#if DIRTY_MCP_SUPPORT
	void SetDirtySupport(bool newState);
#endif

#ifdef WIN32
	void SetCheckoutFileAttribute(CHECKOUT_FILEATTRIBUTE_TYPE newState);
	void SetCvsMessages(CVSMESSAGES_TYPE newState);
#else
	void SetCheckoutRO(bool newState);
	void SetQuietOption(bool newState);
#endif

	void SetZ9Option(bool newState);
	void SetPruneOption(bool newState);
	void SetIsoConvert(ISO8559 newState);
	void SetICTextOption(bool newState);
	void SetAddControl(bool newState);
	void SetMacBinEncodeBinariesOnly(bool newState);
	void SetMacBinEncoding(MACENCODING newState);
	
#if INTERNAL_AUTHEN
	void SetKserverPort(int newState);
	void SetPserverPort(int newState);
	void SetRhostPort(int newState);
	void SetServerName(const char* newservername);
	void SetRshName(const char* newrshname);
#endif /*# INTERNAL_AUTHEN */
	
	void SetEncryptCommunication(bool newState);
	void SetHome(const char* newhome);
	void SetConsoleFont(const char* newFont);
	void SetViewer(const char* newViewer);
	void SetConflictEditor(const char* newConflictEditor);
	void SetAnnotateViewer(const char* newAnnotateViewer);
	void SetExtDiff(const char* newDiff);
	void SetHelper(const char* newHelper);
	void SetBrowser(const char* newBrowser);
	void SetLogoutTimeOut(int newTimeout);
	void SetRefreshInactiveTimeOut(int newTimeout);
#if INTERNAL_AUTHEN
	void SetAlwaysUseCvsroot(bool newState);
#endif
	void SetUseProxy(bool useit);
	void SetProxyHost(const char* newhost);
	void SetProxyPort(int newport);
	void SetUnixLF(bool useit);
	void SetMacLF(bool useit);
	void SetZLevel(int level);
	void SetCvsVersion(int version);
	void SetCvsConsole(bool console);
	void SetWordWrapLogMsg(int wordwrap);
	void SetLastWorkingDir(const char* workdir);
	void SetMatchEntriesCase(bool newState);
	void SetSmartSorting(bool newState);
	
	void SetHideCommandDlgUpdate(bool newState);
	void SetHideCommandDlgQueryUpdate(bool newState);
	void SetHideCommandDlgDiff(bool newState);
	void SetHideCommandDlgLog(bool newState);
	void SetHideCommandDlgGraph(bool newState);
	void SetHideCommandDlgAnnotate(bool newState);
	void SetHideCommandDlgStatus(bool newState);
	void SetHideCommandDlgFileAction(bool newState);
	void SetHideCommandDlgImportScan(bool newState);
	void SetHideCommandDlgRemove(bool newState);

	void SetHideCommandDlgUseShift(bool newState);
	void SetCtrlPressedOnOK(bool newState);

	void SetUseShell(SHELL_KIND shell);
	void SetDisableSplashScreen(bool newState);

	void SetStatusOutputType(STATUSOUTPUT_TYPE outputType);
	void SetFileActionType(FILEACTION_TYPE fileActionType);
	void SetUpdateBuildDirectories(bool newState);
	void SetUpdateCaseSensitiveNames(bool newState);
	void SetUpdateLastCheckinTime(bool newState);
	void SetUpdateThreeWayConflicts(bool newState);

	void SetSettingsFolder(const char* settingsFolder);
	void SetTclLibrary(const char* tclLibrary);
	void SetPythonLibrary(const char* pythonLibrary);
	void SetMacrosFolder(const char* macrosFolder);
	void SetPythonLibFolder(const char* pythonLibFolder);
	
#ifdef WIN32
	kWinVersion WhichVersion();
#endif /* WIN32 */

	bool UseAltCvs(void) const;
	void SetUseAltCvs(bool useIt);
#if TARGET_RT_MAC_MACHO
	bool UseCvsnt(void) const;
	void SetUseCvsnt(bool useIt);
	bool IsInternalCvsnt() const;
#endif
	const char* WhichCvs(std::string& path, bool userOnly = false) const;
	void SetWhichCvs(const char* cvsPath);	// Full path of the CVS used
	SHOWCVS_TYPE ShowCvs(void) const;
	void SetShowCvs(SHOWCVS_TYPE showCvs);
	bool EnableCommandLineLimit(void) const;
	void SetEnableCommandLineLimit(bool enableCommandLineLimit);
	int CommandLineLimit(void) const;
	void SetCommandLineLimit(int commandLineLimit);

	// login/logout flag in order to manage auto-logout
	inline bool HasLoogedIn(void) const { return fHasLoggedIn; }
	inline void SetHasLoogedIn(bool newstate) { fHasLoggedIn = newstate; }

	// Usage counter flags for CVS commands and TCL macro files.
	// Each SetCvsRunning(TRUE) must have equivalent 
	// SetCvsRunning(FALSE) call.
	bool IsCvsRunning(void) const;
	int  SetCvsRunning(bool launched);
	bool IsTclFileRunning(void) const;
	int  SetTclFileRunning(bool launched);

	// save/load preferences
	void save(bool quiet = false);
	void load(void);

	bool NeedCvsrootMigration();

protected :
	bool fHasLoggedIn;

	int  fCvsRunningCounter;	  // Usage counters to CVS commands
	int  fTclFileRunningCounter;  // and Tcl macro file executions
};

extern CvsPrefs gCvsPrefs;

#endif /* CVSPREFS_H */
