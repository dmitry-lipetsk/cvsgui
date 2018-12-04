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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- January 2004
 */

/*
 * LaunchHandlers.cpp : Launch helper functions
 */

#include "stdafx.h"

#include <string>

#ifdef WIN32
#	include "wincvs.h"
#endif /* !WIN32 */

#include "LaunchHandlers.h"
#include "CvsPrefs.h"
#include "AppConsole.h"
#include "CvsCommands.h"
#include "cvsgui_i18n.h"
#include "CvsArgs.h"

#if TARGET_RT_MAC_MACHO
#include "MacCvsApp.h"
#include "CBrowserPopup.h"
#include "GUSIFileSpec.h"
#include "MacCvsConstant.h"
#endif

#ifdef qUnix
#include "UCvsApp.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

#ifdef WIN32
/// Generic check of shell action success
#define SHELLACTION_SUCCEEDED(shellActionResult) (shellActionResult >= 32)

/// Shell Action type enum
typedef enum{
	kShellActionDefault,	/*!< Execute OS default action for object */
	kShellActionOpen,		/*!< Open object in OS-associated handler application (or execute object itself if it is executable) */
	kShellActionEdit,		/*!< Open object in OS-associated editor application */
	kShellActionOpenAs,		/*!< Open OS handler selection dialog and open object with selected application */
	kShellActionProperties	/*!< Open OS file properties dialog */
}kShellActionType;

/*!
	Get the launch type name
	\param launchType Launch type
	\param shortName true to retrieve the short name, false to get the long name
	\return The launch type name
*/
const char* GetLaunchTypeName(const kLaunchType launchType, const bool shortName /*= true*/)
{
	const char* res = "";

	switch( launchType )
	{
	case kLaunchEdit:
		res = shortName ? _i18n("Edit") : _i18n("Edit with the associated editor");
		break;
	case kLaunchDefaultEdit:
		res = shortName ? _i18n("Edit with editor") : _i18n("Edit with the default editor");
		break;
	case kLaunchOpen:
		res = shortName ? _i18n("Open") : _i18n("Open with the associated viewer");
		break;
	case kLaunchOpenAs:
		res = shortName ? _i18n("Open with...") : _i18n("Choose an application to open with");
		break;
	case kLaunchDefault:
		res = shortName ? _i18n("Default") : _i18n("Execute default associated shell action");
		break;
	case kLaunchConflictEdit:
		res = shortName ? _i18n("Edit with conflict editor") : _i18n("Edit with the conflict editor");
		break;
	case kLaunchAnnotateViewer:
		res = shortName ? _i18n("Open with annotate viewer") : _i18n("Open with the annotate viewer");
		break;
	default:
		ASSERT(FALSE); // Unknown type
		break;
	}

	return res;
}

/*!
	Launch a file, folder or program via the OS' high-level API
	\param actionType Manner in which the given object is to be launched
	\param filePath File or folder name to launch (may include full path and arguments)
	\param args Arguments to pass to the file in case it is an application
	\param workdir Working directory to launch the file from
	\return Returns an OS-specific exit code - use SHELLACTION_SUCCEEDED to test for success
*/
static UINT ShellAction(const kShellActionType actionType, const char* filePath, const char* args  = 0L, const char* workdir = 0L)
{
	char* verb;

	switch( actionType )
	{
	case kShellActionDefault:
		verb = NULL;
		break;
	case kShellActionOpen:
		verb = "open";
		break;
	case kShellActionEdit:
		verb = "edit";
		break;
	case kShellActionOpenAs:
		verb = "openas";
		break;
	case kShellActionProperties:
		verb = "properties";
		break;
	}
	
	HWND hParentWnd = AfxGetMainWnd()->GetSafeHwnd();
	
	SHELLEXECUTEINFO seInfo = { 0 };
	seInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	seInfo.hwnd = hParentWnd;
	seInfo.lpVerb = verb;
	seInfo.lpFile = filePath;
	seInfo.lpParameters = args;
	seInfo.lpDirectory = workdir;
	seInfo.nShow = SW_SHOWDEFAULT;
	
	if( actionType != kShellActionOpenAs )
	{
		seInfo.fMask |= SEE_MASK_FLAG_NO_UI;
	}
	
	if( actionType == kShellActionProperties )
	{
		seInfo.fMask |= SEE_MASK_INVOKEIDLIST;
	}
	
	ShellExecuteEx(&seInfo);

	return (UINT)seInfo.hInstApp;
}
#endif

/*!
	Launch a diff of two files
	\param f1 First file name
	\param f2 Second file name
	\param diffTool diff tool to launch (optional)
	\param unifiedDiff if \a true then use unified diff if possible
	\param noWhiteSpace if \a true then ignore extra white space if possible
	\param noBlankLines if \a true then ignore blank lines if possible
	\param ignoreCase if \a true then ignore case differences if possible
	\return true on success, false otherwise
*/
bool LaunchDiff(const char* f1, const char* f2, 
				const char* diffTool /*= NULL*/, bool unifiedDiff /*= false*/, bool noWhiteSpace /*= false*/, bool noBlankLines /*= false*/, bool ignoreCase /*= false*/)
{
	if( IsNullOrEmpty(diffTool) ) 
		diffTool = gCvsPrefs.ExtDiff();

	if( IsNullOrEmpty(diffTool) )
	{
		cvs_err(_i18n("The external diff program is not defined.\n"));
		cvs_err(_i18n("You need to set it in the preferences dialog.\n"));
		return false;
	}
	
#ifdef WIN32
	string args;
	string program, file1, file2;
	
	if( strchr(diffTool, ' ') != 0L )
	{
		program += '\"';
		program += diffTool;
		program += '\"';
	}
	else
	{
		program = diffTool;
	}
	
	if( strchr(f1, ' ') != 0L )
	{
		file1 += '\"';
		file1 += f1;
		file1 += '\"';
	}
	else
	{
		file1 = f1;
	}
	
	if( strchr(f2, ' ') != 0L )
	{
		file2 += '\"';
		file2 += f2;
		file2 += '\"';
	}
	else
	{
		file2 = f2;
	}
	
	args += file1 + " " + file2;
	
	UINT shellRes = ShellAction(kShellActionDefault, diffTool, args.c_str());
	if( !SHELLACTION_SUCCEEDED(shellRes) )
	{
		// ExtDiff app was not found. Let's try to execute it from
		// the application path. However, this is done only when ExtDiff
		// value doesn't contain absolute path ("c:\subdir\" or "\subdir\").
		if( strchr(diffTool, ':') == NULL && (diffTool[0]) != '\\' )
		{
			string appPath;
			
			::GetAppPath(appPath);
			NormalizeFolderPath(appPath);
			appPath += diffTool;
			
			shellRes = ShellAction(kShellActionDefault, appPath.c_str(), args.c_str());
		}
	}
	
	if( !SHELLACTION_SUCCEEDED(shellRes) ) 
	{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);
		
		cvs_err(_i18n("Unable to open '%s' (%s), using console instead\n"), diffTool, (LPCSTR)lpMsgBuf);
		// Free the buffer.
		LocalFree( lpMsgBuf );
		return false;
	}

#elif qUnix
	CvsArgs args(false);
	args.add(diffTool);
	args.add(f1);
	args.add(f2);
	UCvsApp::gApp->Execute(args.Argc(), args.Argv());
#elif TARGET_RT_MAC_MACHO
	CvsArgs args(false);
	args.add(diffTool);
	if ( noWhiteSpace ) {
		if ( strcmp(diffTool, kDiffTool_CodeWarrior) == 0  ||  strcmp(diffTool, kDiffTool_BBEdit) == 0 ) args.add("-wb");
	}
	if ( ignoreCase ) {
		if ( strcmp(diffTool, kDiffTool_CodeWarrior) == 0  ||  strcmp(diffTool, kDiffTool_BBEdit) == 0 ) args.add("-i");
	}
	args.add(f1);
	args.add(f2);
	CMacCvsApp::gApp->Execute(args.Argc(), args.Argv());
#endif
	
	return true;
}

/*!
	Launch the tool application for a file
	\param filePath File name
	\param tool Tool application
	\param par Return data parameter to return the info about running application process
	\return true on success, false otherwise
*/
bool LaunchTool(const char* filePath, const char* tool, void* par/*= NULL*/)
{
	bool res = false;

	if( IsNullOrEmpty(tool) )
	{
		cvs_err(_i18n("No tool defined\n"));
		return false;
	}

#ifndef qMacCvsPP
	string program, file;

	if( strchr(tool, ' ') != 0L )
	{
		program += '\"';
		program += tool;
		program += '\"';
	}
	else
	{
		program = tool;
	}

	if( strchr(filePath, ' ') != 0L )
	{
		file += '\"';
		file += filePath;
		file += '\"';
	}
	else
	{
		file = filePath;
	}

	string command;
	command += program;
	command += " ";
	command += file;
#endif

#ifdef WIN32
	STARTUPINFO sStartupInfo = { 0 };
	PROCESS_INFORMATION sProcessInformation = { 0 };

	// Execute the command with a call to the CreateProcess API call.
	sStartupInfo.cb = sizeof(sStartupInfo);
	sStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	sStartupInfo.wShowWindow = SW_SHOWNORMAL;
	
	if( !CreateProcess(NULL,
		(char*)command.c_str(),
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL, 
		&sStartupInfo,
		&sProcessInformation) )
	{
		sProcessInformation.hProcess = INVALID_HANDLE_VALUE;
		sProcessInformation.hThread = INVALID_HANDLE_VALUE;
	}
	
	if( sProcessInformation.hProcess != INVALID_HANDLE_VALUE )
	{
		res = true;
	}
	
	if( par )
	{
		*((PROCESS_INFORMATION*)par) = sProcessInformation;
	}
	else if( res )
	{
		CloseHandle(sProcessInformation.hProcess);
		CloseHandle(sProcessInformation.hThread);
	}

#endif /* WIN32 */

#ifdef qMacCvsPP
	res = true;
#endif /* qMacCvsPP */
	
#if qUnix
	res = true;
#endif // qUnix

	return res;
}

/*!
	Launch a handler for file
	\param launchType Type of the requested handler type
	\param filePath Full path to the file
	\return true on success, false otherwise
*/
bool LaunchHandler(const kLaunchType launchType, const char* filePath)
{
	bool res = false;

#ifdef WIN32
	string folder, file;
	SplitPath(filePath, folder, file);

	switch( launchType )
	{
	case kLaunchOpen:
		// Explicitly attempt "open"
		res = SHELLACTION_SUCCEEDED(ShellAction(kShellActionOpen, filePath, NULL, folder.c_str()));
		if( !res )
		{
			// Attempt OS default action for object
			res = SHELLACTION_SUCCEEDED(ShellAction(kShellActionDefault, filePath, NULL, folder.c_str()));
		}
		break;
	case kLaunchConflictEdit:
		// Try to open with conflict editor
		res = LaunchConflictEditor(filePath, NULL);
		break;
	case kLaunchAnnotateViewer:
		// Try to open with annotate viewer
		res = LaunchAnnotateViewer(filePath, NULL);
		break;
	default:
		// Nothing to do
		break;
	}

	if( !res && (launchType == kLaunchOpen || launchType == kLaunchOpenAs) )
	{
		// Show OS handler selection dialog
		res = SHELLACTION_SUCCEEDED(ShellAction(kShellActionOpenAs, filePath, NULL, folder.c_str()));
		if( !res )
		{
			cvs_err(_i18n("Unable to open '%s' (error %d), using editor instead\n"), (char*)filePath, GetLastError());
		}
	}
	
	if( !res && launchType != kLaunchDefaultEdit && launchType != kLaunchDefault )
	{
		// Attempt "edit" action
		res = SHELLACTION_SUCCEEDED(ShellAction(kShellActionEdit, filePath, NULL, folder.c_str()));
	}

	if( !res && launchType == kLaunchDefault )
	{
		res = SHELLACTION_SUCCEEDED(ShellAction(kShellActionDefault, filePath, NULL, folder.c_str()));
	}
#endif
	
	if( !res )
	{
		// If all else fails, open in default editor
		res = LaunchDefaultEditor(filePath);
	}
	
	return res;
}

/*!
	Launch a viewer for file
	\param filePath Full path to the file
	\param useOpenAs true to always show "Open with" dialog, false to only show it if no valid association exists
	\return true on success, false otherwise
*/
bool LaunchViewer(const char* filePath, bool useOpenAs /* = false */)
{
	bool res = false;
	
#ifdef WIN32
	bool needOpenAs = false;
	
	string folder, file;
	SplitPath(filePath, folder, file);
	
	if( !useOpenAs )
	{
		UINT shellRes = ShellAction(kShellActionOpen, filePath, NULL, folder.c_str());
		if( !SHELLACTION_SUCCEEDED(shellRes))
		{
			// If it failed because of no valid association and file has an extension then try the "Open with..." dialog instead
			needOpenAs = (file.rfind('.') > 0 && 
				(SE_ERR_NOASSOC == shellRes || SE_ERR_ASSOCINCOMPLETE == shellRes));
		}
		else
		{
			res = true;
		}
	}
	
	if( !res && (useOpenAs || needOpenAs) )
	{
		res = SHELLACTION_SUCCEEDED(ShellAction(kShellActionOpenAs, filePath, NULL, folder.c_str()));
	}
	
	if( !res )
	{
		cvs_err(_i18n("Unable to open '%s' (error %d), using default editor instead\n"), (char*)filePath, GetLastError());
		res = LaunchEditor(filePath);
	}

#endif /* WIN32 */
	
#ifdef qMacCvsPP 
  // fall thru to calling LaunchEditor below...
#endif /* qMacCvsPP */
	
#if qUnix
	  res = true;
#endif // qUnix

	if( !res )
	{
		res = LaunchEditor(filePath);
	}

	return res;
}

/*!
	Launch an editor for file
	\param filePath Full path to the file
	\param useDefault true to use OS setup editor, false to use default editor
	\return true on success, false otherwise
*/
bool LaunchEditor(const char* filePath, bool useDefault /*= false*/)
{
	bool res = false;
	
#ifdef WIN32
	if( !useDefault )
	{
		string folder, file;
		SplitPath(filePath, folder, file);
		
		res = SHELLACTION_SUCCEEDED(ShellAction(kShellActionEdit, filePath, NULL, folder.c_str()));
	}
	
#endif /* WIN32 */
	
#ifdef qMacCvsPP
	if ( !useDefault )
	{
		GUSIFileSpec  f(filePath);
		OSStatus      err;
		err = LSOpenFSRef(f, NULL);
		FSRef         application;
		
		err = LSGetApplicationForItem(f, kLSRolesAll, &application, NULL);
		if ( err != noErr )
		{
			// no luck finding a default binding -> try as text file
			err = LSGetApplicationForInfo('TEXT', kLSUnknownCreator, CFSTR("txt"), kLSRolesAll, &application, NULL);
		}
		if ( err == noErr )
		{
			LSLaunchFSRefSpec   launchSpec = { &application, 1, f, NULL, kLSLaunchDefaults|kLSLaunchDontAddToRecents, NULL };
			err = LSOpenFromRefSpec(&launchSpec, NULL);
		}
		if ( err != noErr )
		{
			cvs_err(_i18n("Unable to open '%s' (error %ld), using default editor instead\n"), (char*)filePath, err);
		}
		else res = true;
	}
#endif /* qMacCvsPP */
	
#if qUnix
#endif // qUnix
	
	if( !res )
	{
		res = LaunchDefaultEditor(filePath);
	}
	
	return res;
}

/*!
	Launch the default editor for a file
	\param filePath File name
	\param par Return data parameter to return the info about running application process
	\return true on success, false otherwise
*/
bool LaunchDefaultEditor(const char* filePath, void* par /* = NULL */)
{
	bool res = false;

#ifndef qMacCvsPP
	if( IsNullOrEmpty(gCvsPrefs.Viewer()) )
	{
		cvs_err(_i18n("You need to define a default editor in the preferences\n"));
		return false;
	}

	res = LaunchTool(filePath, gCvsPrefs.Viewer(), par);
#endif

#ifdef qMacCvsPP
	GUSIFileSpec  f(filePath);
	OSStatus      err(-1);
	FSRef         application;
	bool          gotViewer(false);
	
	if( gCvsPrefs.Viewer() != NULL )
	{
		// viewer contains creator and/or bundle identifier
		if ( strlen(gCvsPrefs.Viewer()) >= 4 )
		{
			OSType      appCreator;
			CFStringRef bundleIdentifier(NULL);
			
			memcpy(&appCreator, gCvsPrefs.Viewer(), 4);
			if ( strlen(gCvsPrefs.Viewer()) > 4  &&  gCvsPrefs.Viewer()[4] == ':' )
				bundleIdentifier = CFStringCreateWithCString(NULL, &gCvsPrefs.Viewer()[5], kCFStringEncodingUTF8);
			err = LSFindApplicationForInfo(appCreator == '\?\?\?\?' ? kLSUnknownCreator : appCreator, bundleIdentifier, NULL, &application, NULL);
			if ( err != noErr )
			{
				cvs_err(_i18n("Unable to find your default viewer '%s' (error %ld), will use default text editor.\n"), gCvsPrefs.Viewer(), err);
			}
		}
	}
	if ( err != noErr )
	{
		// use the default text file viewer
		err = LSGetApplicationForInfo('TEXT', kLSUnknownCreator, CFSTR("txt"), kLSRolesAll, &application, NULL);
	}
	
	if ( err == noErr )
	{
		LSLaunchFSRefSpec   launchSpec = { &application, 1, f, NULL, kLSLaunchDefaults|kLSLaunchDontAddToRecents, NULL };
		err = LSOpenFromRefSpec(&launchSpec, NULL);
	}
	if ( err != noErr )
	{
		cvs_err(_i18n("Unable to open '%s' (error %ld) with default editor '%s'.\n"), (char*)filePath, err, gCvsPrefs.Viewer() ? gCvsPrefs.Viewer() : "<default text editor>");
	}
	else 
		res = true;
#endif /* qMacCvsPP */
	
	return res;
}

/*!
	Launch the conflict editor for a file
	\param filePath File name
	\param par Return data parameter to return the info about running application process
	\return true on success, false otherwise
*/
bool LaunchConflictEditor(const char* filePath, void* par /*= NULL*/)
{
	if( IsNullOrEmpty(gCvsPrefs.ConflictEditor()) )
	{
		cvs_err(_i18n("You need to define a conflict editor in the preferences\n"));
		return false;
	}
	
	return LaunchTool(filePath, gCvsPrefs.ConflictEditor(), par);
}

/*!
	Launch the annotate viewer for a file
	\param filePath File name
	\param par Return data parameter to return the info about running application process
	\return true on success, false otherwise
*/
bool LaunchAnnotateViewer(const char* filePath, void* par /*= NULL*/)
{
	if( IsNullOrEmpty(gCvsPrefs.AnnotateViewer()) )
	{
		cvs_err(_i18n("You need to define an annotate viewer in the preferences\n"));
		return false;
	}
	
	return LaunchTool(filePath, gCvsPrefs.AnnotateViewer(), par);
}

/*!
	Helper function to explore given path
	\param path Path to explore
	\note On Windows it will select the path
*/
void LaunchExplore(const char* path)
{
#ifdef WIN32
	string sCmdLine = string("/e,/select,") + path;

	if( !SHELLACTION_SUCCEEDED(ShellAction(kShellActionDefault, "explorer.exe", sCmdLine.c_str())) )
	{
		cvs_err("Unable to explore '%s' (error %d)\n", (char*)path, GetLastError());
	}
#endif
#ifdef qMacCvsPP
  GUSIFileSpec    fileSpec(path);
  CBrowserPopup::SendFinderOpenAE(fileSpec, true);
#endif
#ifdef qUnix
	CvsArgs args(false);
	args.add(gCvsPrefs.Browser());
	args.add(path);
	UCvsApp::gApp->Execute(args.Argc(), args.Argv());
#endif
}

/*!
	Helper function to launch command prompt console with given path preset
	\param path Path to open command prompt with
	\note Using ComSpec environmental variable to determine command prompt binary typically
*/
void LaunchCommandPrompt(const char* path)
{
#ifdef WIN32
	char* comspec = getenv("ComSpec");
	if( IsNullOrEmpty(comspec) )
	{
		cvs_err("Unable to get ComSpec value\n");
		return;
	}

	string sCmdLine = string("/K cd /D ") + path;

	if( !SHELLACTION_SUCCEEDED(ShellAction(kShellActionDefault, comspec, sCmdLine.c_str())) )
	{
		cvs_err("Unable to launch Command Prompt for '%s' (error %d)\n", (char*)path, GetLastError());
	}

#endif
#ifdef qMacCvsPP
#endif
#ifdef qUnix
#endif
}

/*!
	Helper function to open given path in a new instance
	\param path Path to open
*/
void LaunchNewInst(const char* path)
{
	string appPath;

#ifdef WIN32
	::GetAppModule(appPath);

	if( !SHELLACTION_SUCCEEDED(ShellAction(kShellActionDefault, appPath.c_str(), path)) )
	{
		cvs_err("Unable to open new instance for '%s' (error %d)\n", (char*)path, GetLastError());
	}
#endif
#ifdef qMacCvsPP
#endif
#ifdef qUnix
#endif
}

/*!
	Helper function to show the properties of a given path
	\param path Path to show the properties
*/
void LaunchProperties(const char* path)
{
#ifdef WIN32
	if( !SHELLACTION_SUCCEEDED(ShellAction(kShellActionProperties, path)) )
	{
		cvs_err("Unable to show the properties '%s' (error %d)\n", (char*)path, GetLastError());
	}
#endif
#ifdef qMacCvsPP
#endif
#ifdef qUnix
#endif
}
