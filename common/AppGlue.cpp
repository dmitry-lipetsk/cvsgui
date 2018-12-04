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
 * AppGlue.cpp --- glue code to access CVS services
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#ifdef TARGET_OS_MAC
#       include <Carbon/Carbon.h>
#	include <unistd.h>
#	include <errno.h>
//#	include <console.h>
#	include <ctype.h>
#	include "mktemp.h"
#	include "dll_process.h"
#	include "AppGlue.mac.h"
#	include "MacMisc.h"
#endif /* TARGET_OS_MAC */

#ifdef qMacCvsPP
#	include "MacCvsApp.h"
#	include "LogWindow.h"
#	include "MacBinEncoding.h"
#	include "MacCvsAE.h"
#endif /* qMacCvsPP */

#ifdef WIN32
#	include "wincvs.h"
#	include "PromptFiles.h"
#	include "BrowseFileView.h"

#	include <process.h>
#	include <direct.h>

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#include "cvsgui_process.h"

#if qUnix
#	include "UCvsApp.h"
#	include "UCvsFiles.h"
#	include <ctype.h>
#endif /* qUnix */

#ifdef HAVE_ERRNO_H
#	include <errno.h>
#endif

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#include "Authen.h"
#include "CvsPrefs.h"
#include "AppGlue.h"
#include "AppConsole.h"
#include "GetPassword.h"
#include "AskYesNo.h"
#include "CvsArgs.h"
#include "FileTraversal.h"
#include "CvsCommands.h"
#include "cvsgui_i18n.h"
#include "CvsAlert.h"

using namespace std;

/// Last CVS command exit code
static int sLastExitCode;

/// Temporary files list
CTmpFiles gTmpList;

#ifdef _MSC_VER
#	define CVS_EXTERN_C extern "C"
#else
#	define CVS_EXTERN_C
#endif

static CvsProcessCallbacks sCallTable = {
	glue_consoleout,
	glue_consoleerr,
	glue_getenv,
	glue_exit,
	glue_ondestroy
};

/*!
	Release any application data allocated
	\param process CVS Process 
*/
CVS_EXTERN_C void glue_ondestroy(const CvsProcess* process)
{
	if( process->appData )
	{
		CCvsProcessData* pCvsProcessData = (CCvsProcessData*)process->appData;
		delete pCvsProcessData;
	}
}

/*!
	Print the CVS command exit code
	\param code Exit code to print
	\param process CVS Process 
*/
CVS_EXTERN_C void glue_exit(const int code, const CvsProcess* process)
{
	if( !gCvsPrefs.IsTclFileRunning() )
	{
		if( code >= 0 )
			cvs_out(_i18n("\n***** CVS exited normally with code %d *****\n\n"), code);
		else
			cvs_out(_i18n("\n***** CVS has been aborted on user request *****\n\n"), code);
	}

	sLastExitCode = code;
}

/*!
	Send stdout of the CVS command to the console
	\param txt Text to be send to the console
	\param len Text length
	\param process CVS Process 
	\return The printed text length
*/
CVS_EXTERN_C long glue_consoleout(const char* txt, const long len, const CvsProcess* process)
{
	if( process->appData )
	{
		CCvsProcessData* pCvsProcessData = (CCvsProcessData*)process->appData;
		if( pCvsProcessData->GetCvsConsole() )
		{
			return pCvsProcessData->GetCvsConsole()->cvs_out(txt, len);
		}
	}

	cvs_outstr(txt, len);

	return len;
}

/*!
	Send stderr of the CVS command to the console
	\param txt Text to be send to the console
	\param len Text length
	\param process CVS Process 
	\return The printed text length
*/
CVS_EXTERN_C long glue_consoleerr(const char* txt, const long len, const CvsProcess* process)
{
	if( process->appData )
	{
		CCvsProcessData* pCvsProcessData = (CCvsProcessData*)process->appData;
		if( pCvsProcessData->GetCvsConsole() )
		{
			return pCvsProcessData->GetCvsConsole()->cvs_err(txt, len);
		}
	}

	cvs_errstr(txt, len);

	return len;
}

/*!
	Send stdin of the console to the CVS command
	\param txt Text to be send to the CVS command
	\param len Text length
	\param process CVS Process 
	\return The sent text length
*/
CVS_EXTERN_C long glue_consolein(const char* txt, const long len, const CvsProcess* process)
{
	return 0;
}

#ifdef TARGET_OS_MAC
#	define isGlobalEnvVariable(name) ( \
		strcmp(name, "CVS_PSERVER_PORT") == 0 || \
		strcmp(name, "CVS_RCMD_PORT") == 0 || \
		strcmp(name, "CVS_SERVER") == 0 || \
		strcmp(name, "CVS_RSH") == 0 || \
		strcmp(name, "CVS_RSA_IDENTITY") == 0 || \
		strcmp(name, "HOME") == 0 || \
		strcmp(name, "ISO8859") == 0 || \
		strcmp(name, "IC_ON_TXT") == 0 || \
		strcmp(name, "MAC_DEFAULT_RESOURCE_ENCODING") == 0 || \
		strcmp(name, "MAC_BINARY_TYPES_SINGLE") == 0 || \
		strcmp(name, "MAC_BINARY_TYPES_HQX") == 0 || \
		strcmp(name, "MAC_BINARY_TYPES_PLAIN") == 0 || \
		strcmp(name, "CVSUSEUNIXLF") == 0 || \
		strcmp(name, "DIRTY_MCP") == 0 \
		)
#endif /* TARGET_OS_MAC */

/*!
	Set the environmental variables for the CVS command
	\param name Environmental variable name
	\param process CVS Process 
	\return The environmental variable value as const char* 
	\note Make sure that the returned pointer doesn't get deallocated prematurely
*/
CVS_EXTERN_C const char* glue_getenv(const char* name, const CvsProcess* process)
{
	if( name == NULL )
		return NULL;
	
#ifdef WIN32
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( !app )
	{
		ASSERT(FALSE);	// need an application object
		return NULL;
	}

	if( ::GetCurrentThreadId() != app->m_nThreadID &&
		(strcmp(name, "CVS_GETPASS") == 0 || strcmp(name, "CVSLIB_YESNO") == 0 ||
		strcmp(name, "HOME") == 0) )
	{
		return app->GetEnvMainThread(name);
	}
#endif /* WIN32 */

#ifdef TARGET_OS_MAC
	char* result;
	if( (result = ae_getenv(name)) != (char*)-1 )
	{
		if( result == 0L && isGlobalEnvVariable(name) )
		{
			// in this case only we let continue
			// to get the MacCVS settings. So the CWCVS
			// users (or any AppleEvent user) can set
			// these variables in MacCVS but still can overide
			// them using Apple Events.
		}
		else
			return result;
	}
#endif /* TARGET_OS_MAC */

#if INTERNAL_AUTHEN
	// Give a chance for the new AuthenModel to react to the getenv callback
	AuthenModel* curModel = AuthenModel::GetInstance(gAuthen.kind());
	vector<AuthenModel*>& allInstances = AuthenModel::GetAllInstances();
	vector<AuthenModel*>::const_iterator i;

	if( curModel != 0L )
	{
		const char* res = curModel->OnGetEnv(name);
		if( res != 0L )
			return res;
	}

	for(i = allInstances.begin(); i != allInstances.end(); ++i)
	{
		AuthenModel* model = *i;

		if( model == curModel )
			continue;

		const char* res = model->OnGetEnv(name);
		if( res != 0L )
			return res;
	}

	if( strcmp(name, "USER") == 0
#ifdef WIN32
		// I do that (and see the "STRATA_HACK" in
		// subr.c) because people get tired on Windows
		// to have to check their Windows login :
		// this is only for .rhosts authentication.
		|| strcmp(name, "LOGNAME") == 0
#endif /* WIN32 */
		)
	{
		static char user[64];
		
		// extract from the cvsroot
		const char* ccvsroot = gCvsPrefs;
		ccvsroot = Authen::skiptoken(ccvsroot);
		char* login = strchr(ccvsroot, '@');
		if( login == NULL )
		{
			// for WIN32/gcvs this means the CVSROOT is local
#ifdef TARGET_OS_MAC
			cvs_err(_i18n("MacCvs: Cannot extract login from cvsroot %s !\n"), ccvsroot);
#endif /* TARGET_OS_MAC */
			return NULL;
		}

		int length = login - ccvsroot;
		memcpy(user, ccvsroot, length * sizeof(char));
		user[length] = '\0';

		return user;
	}
	else if( strcmp(name, "CVSROOT") == 0 )
	{
		static string root;
		
		// extract from the cvsroot
		const char* ccvsroot = gCvsPrefs;
		ccvsroot = Authen::skiptoken(ccvsroot);
		root = gAuthen.token();
		root += ccvsroot;

		return root.c_str();
	}
	else
#endif /* INTERNAL_AUTHEN */
#ifndef WIN32
	if( strcmp(name, "CVSREAD") == 0 )
	{
		return gCvsPrefs.CheckoutRO() ? (char*)"1" : 0L;
	}
	else
#endif
	if( strncmp(name, "CVS_GETPASS", strlen("CVS_GETPASS")) == 0 )
	{
		string str(_i18n("Enter the password:"));
		if( const char* tmp = strchr(name, '=') )
		{
			str = ++tmp;
		}

		return CompatGetPassword(str.c_str());
	}
	else if( strcmp(name, "CVSLIB_YESNO") == 0 )
	{
#ifdef qMacCvsPP
		// let user know we need some input
		CMacCvsApp::gApp->InteractWithUser();
		// make sure log window is front-most window, visible and not minimized, since we're referring to it in our [modal] alert
		LWindow* logwindow = CMacCvsApp::gApp->GetLogWindow();
		if ( logwindow )
		{
			if ( !logwindow->IsVisible() ) logwindow->Show();
			if ( IsWindowCollapsed(logwindow->GetMacWindow()) ) CollapseWindow(logwindow->GetMacWindow(), false);
			logwindow->Select();
		}
#endif /* qMacCvsPP */

		CvsAlert cvsAlert(kCvsAlertQuestionIcon, 
			_i18n("CVS is requiring you to answer yes or no."), _i18n("(See the console for details)"),
			BUTTONTITLE_YES, BUTTONTITLE_NO);

		return cvsAlert.ShowAlert() == kCvsAlertOKButton ? "yes" : "no";
	}
#if DIRTY_MCP_SUPPORT
	else if( strcmp(name, "DIRTY_MCP") == 0 )
	{
		return gCvsPrefs.DirtySupport() ? "yes" : "no";
	}
#endif
#ifdef WIN32
	else if( strcmp(name, "HOME") == 0 )
	{
		// prompt for home the first time
		if( IsNullOrEmpty(gCvsPrefs.Home()) )
		{
			char oldpath[_MAX_PATH];
			getcwd(oldpath, _MAX_PATH-1);
			
			string home;
			if( !BrowserGetDirectory(_i18n("Select your home directory"), home) )
			{
				chdir(oldpath);
				return "";
			}

			gCvsPrefs.SetHome(home.c_str());
			gCvsPrefs.save();
			chdir(oldpath);
		}

		// Windows 98 doesn't like C:\/.cvspass
		static string home;
		home = gCvsPrefs.Home();
		TrimRight(home, kPathDelimiter);

		return home.c_str();
	}
	else if( strcmp(name, "CVSUSEUNIXLF") == 0 )
	{
		return gCvsPrefs.UnixLF() ? "1" : 0L;
	}
#endif /*WIN32 */
#if TARGET_RT_MAC_MACHO
	else if( strcmp(name, "CVS_MACLF") == 0 )
	{
		return gCvsPrefs.MacLF() ? "1" : 0L;
	}
#endif
#ifdef TARGET_OS_MAC
	else if( strcmp(name, "ISO8859") == 0 )
	{
		static char iso[6];
		
		if( gCvsPrefs.IsoConvert() == ISO8559_none )
			return 0L;
		
		sprintf(iso, "%d", (int)gCvsPrefs.IsoConvert());
		return iso;
	}
	else if( strcmp(name, "IC_ON_TXT") == 0 )
	{
		return gCvsPrefs.ICTextOption() ? "1" : 0L;
	}
	else if( strcmp(name, "MAC_ENCODE_ONLY_BINARY_FILES") == 0 )
	{
		return gCvsPrefs.MacBinEncodeBinariesOnly() ? "1" : 0L;
	}
	else if( strcmp(name, "MAC_DEFAULT_RESOURCE_ENCODING") == 0 )
	{
		switch(gCvsPrefs.MacBinEncoding())
		{
			case MAC_HQX:
				return (char*)"HQX";
				break;
			case MAC_APPSINGLE:
				return (char*)"AppleSingle";
				break;
			case MAC_PLAIN_BINARY:
				return (char*)"PlainBinary";
				break;
		}
		return 0L;
	}
	else if( strcmp(name, "MAC_BINARY_TYPES_SINGLE") == 0 )
	{
		return (char*)MacBinMaps::GetEnvTypes(MAC_APPSINGLE);
	}
	else if( strcmp(name, "MAC_BINARY_TYPES_HQX") == 0 )
	{
		return (char*)MacBinMaps::GetEnvTypes(MAC_HQX);
	}
	else if( strcmp(name, "MAC_BINARY_TYPES_PLAIN") == 0 )
	{
		return (char*)MacBinMaps::GetEnvPlainTypes();
	}
#endif /* TARGET_OS_MAC */
	else 
	{
		char* res;
		res = getenv(name);
		return res;
	}
	
	return "";
}


#if TARGET_RT_MAC_MACHO
static void AdjustEnvironmentForInternalCvsnt()
{
  static  bool    pathsInitialized(false);
  static  CStr    cvsntPath;
  static  CStr    cvsntLibPath;
  if ( !pathsInitialized )
  {
    cvsntPath = CMacCvsApp::gApp->GetAppResourcesPath();
  	if(!cvsntPath.endsWith(kPathDelimiter))
  		cvsntPath << kPathDelimiter;
  	cvsntPath << "cvsnt";
  	cvsntLibPath = cvsntPath;
  	cvsntLibPath << "/lib/";
  	pathsInitialized = true;
  }
	
  if ( gCvsPrefs.IsInternalCvsnt() )
  {
	  char* dyldPath = getenv("DYLD_LIBRARY_PATH");
	  if( dyldPath == NULL  ||  strstr(dyldPath, cvsntLibPath.c_str()) == NULL )
	  {
	    CStr newDyldPath;
	    if ( dyldPath ) newDyldPath << dyldPath << ":";
	    newDyldPath << cvsntLibPath;
		setenv("DYLD_LIBRARY_PATH", newDyldPath.c_str(), 1);
		CStr cvslibPath(cvsntLibPath);
	    cvslibPath << "cvsnt";
		setenv("CVSLIB", cvslibPath.c_str(), 1);
	    setenv("CVS_DIR", cvsntPath.c_str(), 1);
	  }
  }
  else
  {
	  char* dyldPath = getenv("DYLD_LIBRARY_PATH");
	  if ( dyldPath != NULL )
	  {
	    if ( strstr(dyldPath, cvsntLibPath.c_str()) != NULL )
	    {
	      if ( strcmp(dyldPath, cvsntLibPath.c_str()) == 0 )
	        unsetenv("DYLD_LIBRARY_PATH");
	      else
	      {
	        CStr  newDyldPath(dyldPath);
	        CStr  envPart(":");
	        envPart += cvsntLibPath;
	        newDyldPath.Replace(envPart.c_str(), "");
	        if ( newDyldPath.empty() )
  	        unsetenv("DYLD_LIBRARY_PATH");
  	      else
  	        setenv("DYLD_LIBRARY_PATH", newDyldPath.c_str(), 1);
	      }
	      unsetenv("CVSLIB");
	      unsetenv("CVS_DIR");
	    }
	  }
  }
}

class CvsProcessIdler : public LPeriodical
{
protected:
  void SpendTime(const EventRecord	&)
  {
    cvs_process_give_time();
  }
public:
  CvsProcessIdler()
  {
    StartIdling(1 * kEventDurationMillisecond);
  }
};
#endif

/*!
	Launch CVS command
	\param test CVS testing
	\param path CVS binary file
	\param argc Arguments count
	\param argv Arguments
	\param console Console
	\param cvsfile CVS binary file
	\return The process return error code
*/
int launchCVS(int test, const char* path, int argc, char* const* argv, CCvsConsole* console /*= 0L*/, const char* cvsfile /*= 0L*/ )
{
#if INTERNAL_AUTHEN
	CAuthenSetUp setupEnv;
#endif /* INTERNAL_AUTHEN */
	CvsProcessStartupInfo startupInfo;

	startupInfo.hasTty = ( !test ? gCvsPrefs.CvsConsole() : true ); // test cvs has always tty (prevent printing cvs version info on console
	startupInfo.currentDirectory = path;

#ifdef WIN32
	WORD wShowWindow = startupInfo.showWindowState;
	switch( gCvsPrefs.ShowCvs() )
	{
	case SHOWCVS_HIDE:
		wShowWindow = SW_HIDE;
		break;
	case SHOWCVS_SHOWNORMAL:
		wShowWindow = SW_SHOWNORMAL;
		break;
	case SHOWCVS_SHOWNOACTIVATE:
		wShowWindow = SW_SHOWNOACTIVATE;
		break;
	case SHOWCVS_SHOWMINNOACTIVE:
		wShowWindow = SW_SHOWMINNOACTIVE;
		break;
	default:
		break;
	}

	startupInfo.showWindowState = wShowWindow;

	if( gCvsPrefs.EnableCommandLineLimit() )
	{
		startupInfo.commandLineLimit = gCvsPrefs.CommandLineLimit();
	}
#endif

	CCvsProcessData* pCvsProcessData = NULL;
	if( console )
	{
		pCvsProcessData = new CCvsProcessData;
		if( pCvsProcessData )
		{
			pCvsProcessData->SetCvsConsole(console);
		}
	}

	string cvscmd;

#ifdef WIN32
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
#elif qUnix
	UCvsApp* app = UCvsApp::gApp;
#elif TARGET_RT_MAC_MACHO
	CMacCvsApp* app = CMacCvsApp::gApp;
#endif
	
	if( app->IsCvsRunning() && cvs_process_is_active(app->GetCvsProcess()) )
	{
		cvs_err(_i18n("Error: CVS is already running !\n"));
		return -1;
	}
	
#if qUnix || TARGET_RT_MAC_MACHO
	if(path != 0L && chdir(path) != 0) {
		cvs_err(("Error: cannot chdir to %s (error %d)!\n"), path, errno);
	}
#endif // qUnix
	
	app->SetCvsStopping(false);
	
	if( !test )
	{
		gCvsPrefs.WhichCvs(cvscmd, gCvsPrefs.UseAltCvs());
		sLastExitCode = 0;
	}
	else
	{
		cvscmd = cvsfile;
		sLastExitCode = errNoCvsguiProtocol;
	}
	
#if qUnix || TARGET_RT_MAC_MACHO
	struct stat sb;

	if( stat(cvscmd.c_str(), &sb) == -1 ||
#if qUnix || TARGET_RT_MAC_MACHO
		 S_ISLNK(sb.st_mode) ||
#endif
		 !(S_ISREG(sb.st_mode)) )
	{
		cvs_err(_i18n("Unable to find the cvs process in : (error %d)\n"), errno);
		cvs_err("1) %s\n", (const char*)cvscmd.c_str());

		return errInternal;		
	}
#endif

#if TARGET_RT_MAC_MACHO
	// adjust dylib search path for embedded cvsnt if needed
	// (theoretically, we should really do this in the child process when launching cvsnt, 
	//  but since this is handled by stuff inside cvsgui_process, which doesn't know about 
	//  our persistent values, and thus doesn't know whether it's the internal cvsnt tool,
	//  we have to do this in the parent process; shouldn't hurt, though...)
  AdjustEnvironmentForInternalCvsnt();
#endif
	CvsProcess* proc = cvs_process_run(cvscmd.c_str(), argc - 1, (char**)argv + 1, &sCallTable, &startupInfo, (void*)pCvsProcessData);
	if( proc == 0L )
	{
		string errorDetails;

		if( startupInfo.errorInfo == badArguments )
		{
			errorDetails = "Invalid call arguments";
		}
		else if( startupInfo.errorInfo == noMemory )
		{
			errorDetails = "No memory to create CVS process";
		}
		else if( startupInfo.errorInfo == commandTmpFileError )
		{
			errorDetails = "Can't create CVS command file";
		}
		else if( startupInfo.errorInfo == commandFileError )
		{
			errorDetails = "Can't open CVS command file";
		}
		else if( startupInfo.errorInfo == systemError )
		{
#ifndef WIN32
			errorDetails = strerror(startupInfo.lastErrnoValue);
			errorDetails += " (error ";
			errorDetails += startupInfo.lastErrnoValue;
			errorDetails += ")";
#else
			string strMsg;
			LPVOID lpvBuffer = NULL;
			
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, 
				startupInfo.lastErrorValue,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpvBuffer, 
				0, 
				NULL);
			
			strMsg += (char*)lpvBuffer;
			TrimRight(strMsg);

			if( lpvBuffer )
			{
				LocalFree(lpvBuffer);
			}
			
			errorDetails = strMsg;
#endif
		}
		else
		{
			errorDetails = "Unknown error";
		}

		cvs_err(_i18n("Unable to initialize the CVS process: %s\n"), errorDetails.c_str());
		cvs_err(_i18n("The CVS used is : %s\n"), cvscmd.c_str());
	}
	else 
	{
		app->SetCvsRunning(true);
		app->SetCvsProcess(proc);
#if defined(WIN32) || defined(qUnix)
		app->PeekPumpAndIdle(true); 
#endif
#if !defined(qUnix)  &&  !defined(TARGET_RT_MAC_MACHO)
		if( console != 0L )
#endif
		{			
			time_t start = time(NULL);
		
#if TARGET_RT_MAC_MACHO
      CvsProcessIdler   idler;
  #if PP_Debug
      EventTime         startTime = GetCurrentEventTime();
  #endif
#endif
			while( TRUE )
			{
#if defined(WIN32) || defined(qUnix)
				app->PeekPumpAndIdle(false); 
#endif

#if TARGET_RT_MAC_MACHO
				glue_eventdispatcher(0L);
#endif
				cvs_process_give_time();
			
				if( !cvs_process_is_active(proc) )
					break;
				
				if ( test && ( (time(NULL) - start) > 10 ) )
				{	
					cvs_err(_i18n("testing cvs timed out\n"));
					cvs_process_stop(proc);
				}
			} // end while
#if TARGET_RT_MAC_MACHO
			cvs_process_destroy(proc);
			app->SetCvsRunning(false);
			app->SetCvsProcess(NULL);
  #if PP_Debug
			cvs_out("cvs operation took %.3lf seconds\n", GetCurrentEventTime() - startTime);
	#endif
#endif
		}
	}

	return sLastExitCode;
}

/*!
	Stop running CVS command
*/
void stopCVS()
{
#ifdef WIN32

	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	app->SetCvsStopping(true);

	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), 
		WM_SETMESSAGESTRING, 
		0, 
		(LPARAM)_i18n("***** Stopping CVS on user request !!! *****"));

	// Commented out for now to avoid the deadlock
	//cvs_out(_i18n("***** Stopping CVS on user request !!! *****\n\n"));

	cvs_process_stop(app->GetCvsProcess());
#endif
}

/*!
	Test whether specified cvs binary supports cvsgui protocol
	\param cvsfile Cvs binary file
	\param argc Arguments count
	\param argv	Arguments
	\return	The process return error code
*/
int testCVS(const char* cvsfile, int argc, char* const* argv)
{
	return launchCVS(TRUE, NULL, argc, argv, NULL, cvsfile);
}
/*!
	Launch cvs command
	\param path Cvs binary file
	\param argc Arguments count
	\param argv	Arguments
	\param console Console
	\return The process return error code
*/
int launchCVS(const char* path, int argc, char* const* argv, CCvsConsole* console /*= 0L*/)
{
	return launchCVS(FALSE, path, argc, argv, console, NULL);
}

/*!
	Create unique temp file for cvs output
	\param tmpFile File name
	\param prefix File prefix
	\param extension File extension
	\return The output file handle on success, NULL on error
*/
FILE* createCVSTempFile(std::string& tmpFile, const char* prefix, const char* extension)
{
	if( !MakeTmpFile(tmpFile, prefix, extension) )
		return NULL;

	FILE* res = fopen(tmpFile.c_str(), gCvsPrefs.UnixLF() ? "wb+" : "w+");
	if( res == NULL )
	{
		cvs_err(_i18n("Impossible to open the temp file '%s' for write (error %d)"), 
			tmpFile.c_str(), errno);

		return NULL;
	}

	return res;
}

/*!
	Register temporary file
	\param filename Filename
	\return Filename
*/
const char* registerTempFile(const char* filename)
{
	return gTmpList.push_back(filename);
}

/*!
	Launch cvs command and send a stream of the cvs output into a file
	\param dir Directory
	\param args	Cvs arguments
	\param tmpFile File name
	\param prefix File prefix
	\param extension File extension
	\return The output file handle on success, NULL on error
*/
FILE* launchCVS(const char* dir, const CvsArgs& args, std::string& tmpFile, 
				const char* prefix /*= 0L*/, const char* extension /*= 0L*/)
{
	FILE* res = createCVSTempFile(tmpFile, prefix, extension);
	if( !res )
		return 0L;

	CStreamConsole console(res);
	
	int ret = launchCVS(dir, args.Argc(), args.Argv(), &console);
	fseek(res, 0L, SEEK_SET);
	
	// if CVS breaks, we should recognize it (e.g. ext. diff should only start if everything goes fine)
	if( ret != 0 )
	{
		fclose(res);
		res = NULL;
	}

	return res;
}

/*!
	Launch cvs command and put the cvs output in a file
	\param dir	Directory
	\param args	Cvs arguments
	\param prefix File prefix
	\param extension File extension
	\return The file name on success, empty string on error
	\note The temp file is automatically removed when quitting
*/
const char* launchCVS(const char* dir, const CvsArgs& args, 
					  const char* prefix /*= 0L*/, const char* extension /*= 0L*/)
{
	string filename;
	FILE* file = launchCVS(dir, args, filename, prefix, extension);
	if( file == 0L )
		return "";

	fclose(file);

	return registerTempFile(filename.c_str());
}

#ifdef WIN32
void WaitForCvs(void)
{
	// pump messages manually
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();

	while( TRUE )
	{
		if( app->GetCvsProcess() == 0L )
			return;

		if( !cvs_process_is_active(app->GetCvsProcess()) )
			return;

		Sleep(100);
	}
}
#endif

#if TARGET_RT_MAC_MACHO
void WaitForCvs(void)
{
	if ( cvs_process_is_active(CMacCvsApp::gApp->GetCvsProcess()) )
	{
    CvsProcessIdler   idler;
  	while( cvs_process_is_active(CMacCvsApp::gApp->GetCvsProcess()) )
  	{
/*
  		UInt32 ticker = ::TickCount();
  		
  		while( cvs_process_give_time() )
  		{
  			if( (::TickCount() - ticker) >= 60 )
  				break;
  		}
*/
  		glue_eventdispatcher(0L);
  	}

  	CMacCvsApp::gApp->SetCvsRunning(false);
  }
}
#endif

//////////////////////////////////////////////////////////////////////////
// CCvsProcessData

CCvsProcessData::CCvsProcessData()
	: m_pCvsConsole(NULL)
{
}

/*!
	Set the console
	\param pCvsConsole CVS console
*/
void CCvsProcessData::SetCvsConsole(CCvsConsole* pCvsConsole)
{
	m_pCvsConsole = pCvsConsole;
}

/*!
	Get the console
	\return The console
	\note Const overload
*/
CCvsConsole* CCvsProcessData::GetCvsConsole() const
{
	return m_pCvsConsole;
}

/*!
	Get the console
	\return The console
*/
CCvsConsole* CCvsProcessData::GetCvsConsole()
{
	return m_pCvsConsole;
}

//////////////////////////////////////////////////////////////////////////
// 	CCvsFilterConsole

CCvsFilterConsole::CCvsFilterConsole(const char* filter, int options)
{
	m_state = AFTER_NL;
	m_filter = filter;
	m_options = options;
}

long CCvsFilterConsole::cvs_out(const char* txt, long len)
{
	if( m_options & FILTER_OUT )
		return cvs_filter(txt, len);

	cvs_outstr(txt, len);
	return len;
}

long CCvsFilterConsole::cvs_err(const char* txt, long len)
{
	if( m_options & FILTER_ERR )
		return cvs_filter(txt, len);

	cvs_errstr(txt, len);
	return len;
}

long CCvsFilterConsole::cvs_filter(const char* txt, long len)
{
	int lenOut = 0;

	while( len ) 
	{
		if( m_state == AFTER_NL && len >= (long)m_filter.length() )
		{
			if( !strncmp(txt, m_filter.c_str(), m_filter.length()) )
			{
				m_state = AFTER_FILTER;
				txt += m_filter.length();
				len -= m_filter.length();
				continue;
			}
		}

		if( *txt == '\r' || *txt == '\n' )
		{
			if( m_state == AFTER_FILTER )
			{
				if( m_options & PRINT_MATCH )
				{
					::cvs_outstr(m_afterFilter.c_str(), m_afterFilter.length());
					::cvs_outstr("\n", 1);
				}

				lenOut += m_afterFilter.length() + 1;

				OnFilter(m_afterFilter);
				m_afterFilter.erase();
			}

			m_state = AFTER_NL;
			txt++;
			len--;
		}
		else 
		{
			if( m_state == AFTER_FILTER )
			{
				char sAppend[2] = { txt[0], 0 };
				
				m_afterFilter += sAppend;
			}
			else
			{
				m_state = NONE;
			}

			txt++;
			len--;
		}
	}
	
	return lenOut;
}

//////////////////////////////////////////////////////////////////////////
// 	CCvsStreamConsole

CCvsStreamConsole::CCvsStreamConsole(FILE* fileOut /*= 0L*/, bool autoClose /*= true*/)
	: m_autoClose(autoClose), m_modeSet(false), m_fileOut(fileOut)
{
}

CCvsStreamConsole::~CCvsStreamConsole()
{
	if( m_autoClose && m_fileOut )
	{
		fclose(m_fileOut);
	}
}

/*!
	Set the output file mode
	\param binary true to set the binary mode, false to set the text mode
	\note Typically the default mode is text
*/
void CCvsStreamConsole::SetMode(bool binary)
{
#ifdef WIN32
	if( binary )
	{
		setmode(fileno(m_fileOut), _O_BINARY);
	}
	else
	{
		setmode(fileno(m_fileOut), gCvsPrefs.UnixLF() ? _O_BINARY : _O_TEXT);
	}
#endif

	m_modeSet = true;
}

/*!
	Check whether the file translation mode was set already
	\return true if the file translation mode was set already, false otherwise
	\note Should be used before call to setmode to prevent unnecessary mode change
*/
bool CCvsStreamConsole::IsModeSet() const
{
	return m_modeSet;
}

/*!
	Open the output file
	\param fileName File name to be opened
	\return true on success, false otherwise
	\note File is opened as text or binary depending on the line feed preference settings
*/
bool CCvsStreamConsole::Open(const char* fileName)
{
	m_fileOut = fopen(fileName, gCvsPrefs.UnixLF() ? "wb+" : "w+");

	return 0L != m_fileOut;
}

long CCvsStreamConsole::cvs_out(const char* txt, long len)
{
	if( !IsModeSet() )
	{
		SetMode(len == 0);
	}

	fwrite(txt, sizeof(char), len, m_fileOut);
	return len;
}

/// Call from override at the entry
long CCvsStreamConsole::cvs_err(const char* txt, long len)
{
	cvs_errstr(txt, len);
	return len;
}


//////////////////////////////////////////////////////////////////////////
// CStreamConsole

CStreamConsole::CStreamConsole(FILE* fileOut) 
	: CCvsStreamConsole(fileOut, false)
{
}

#if INTERNAL_AUTHEN
//////////////////////////////////////////////////////////////////////////
// CAuthenSetUp

CAuthenSetUp::CAuthenSetUp()
{
	AuthenModel* curModel = AuthenModel::GetInstance(gAuthen.kind());
	if( curModel != 0L )
		curModel->OnSetupEnv();
}

CAuthenSetUp::~CAuthenSetUp()
{
	AuthenModel* curModel = AuthenModel::GetInstance(gAuthen.kind());
	if( curModel != 0L )
		curModel->OnRestoreEnv();
}
#endif /* INTERNAL_AUTHEN */

//////////////////////////////////////////////////////////////////////////
// CTmpFiles

CTmpFiles::CTmpFiles()
{
#ifdef WIN32
	m_removeOnReboot = gCvsPrefs.WhichVersion() == kWinNT4;
#endif
}

CTmpFiles::~CTmpFiles()
{
#ifdef WIN32
	if( m_removeOnReboot )
	{
		return;
	}
#endif

	for(vector<string>::const_iterator i = m_allfiles.begin(); i != m_allfiles.end(); ++i)
	{
		const char* file = i->c_str();

#ifdef WIN32
		wnt_unlink(file);
#else
		unlink(file);
#endif
	}
}

/*!
	Add file to the temp files collection
	\param file File to add
	\return Added file name
*/
inline const char* CTmpFiles::push_back(const char* file)
{
	m_allfiles.push_back(file);

#ifdef WIN32
	if( m_removeOnReboot )
	{
		::MoveFileEx(file, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
	}
#endif

	return m_allfiles[m_allfiles.size() - 1].c_str();
}
