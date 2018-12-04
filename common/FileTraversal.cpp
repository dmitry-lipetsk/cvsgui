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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

/*
 * FileTraversal.cpp : class to traverse a file hierarchy
 */

#include "stdafx.h"

#ifdef WIN32
#	include "resource.h"
#endif /* WIN32 */

#if TARGET_OS_MAC
#	include "GUSIInternal.h"
#	include "GUSIFileSpec.h"
#endif

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_ERRNO_H
#	include <errno.h>
#endif /* HAVE_ERRNO_H */

#include "FileTraversal.h"
#include "ustr.h"
#include "umain.h"
#include <sstream>

#ifdef WIN32
#	include "AppConsole.h"
#	include "wincvsdebug.h"

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#if qUnix
#	include "UCvsApp.h"
#endif

using namespace std;

// Initialize traverse flag
bool TraversalReport::m_running = false;

/*!
	Traverse given path
	\param path Path to traverse
	\param report Return traversal report
	\param macDirSpec Mac specific
	\return kTraversal value to indicate whether to continue the traversing
*/
kTraversal FileTraverse(const char* path, TraversalReport& report, const UFSSpec* macDirSpec)
{
	kTraversal res = kContinueTraversal;

	const UFSSpec *macspec = 0L;

#if defined(WIN32)
#	if ENABLE_WINCVSDEBUG
	unsigned long dbg_mask = GetWinCvsDebugMask(false);
	CStr tracestr;
#	endif	// ENABLE_WINCVSDEBUG
#endif

#ifdef qUnix
	UCvsApp* app = UCvsApp::gApp;
#endif
	
	// Used to restore current directory
	static bool getPathFlag = false;
	char* originalPath = NULL;

	USemaphore policeman(getPathFlag);
	USemaphore runstate(TraversalReport::GetRefRunning());

	if( !policeman.IsEnteredTwice() )
	{
		originalPath = getcwd(NULL, MAX_PATH);
	}

#if TARGET_OS_MAC
	FSRef foldermacspec;

	if( macDirSpec != 0L )
	{
		foldermacspec = *macDirSpec;
	}
	else if( path != 0L )
	{
		FSPathMakeRef(posix_to_utf8(path), &foldermacspec, NULL);
	}

	macspec = &foldermacspec;
#endif

	// Have to put declaration here because of the goto
	string dirname; 
	string tmpName;
	const char* tmp;
	const char* next;

	DIR* dirp = 0L;
	if( path == 0L || (dirp = opendir(path)) == 0L )
	{
		stringstream errText;
		errText << "Error while accessing ";
		errText << path;
		errText << " (error " << errno << ')';
		errText << ends;
		res = report.OnError(errText.str().c_str(), errno);

		goto exit;
	}

	tmpName = path;
	NormalizeFolderPath(tmpName, false);

	tmp = tmpName.c_str();
	next = tmp;

	while( (tmp = strchr(next, kPathDelimiter)) != 0L )
	{
		next = ++tmp;
	}
	
	dirname = next;

	res = report.EnterDirectory(path, dirname.c_str(), macspec);
	if( res != kContinueTraversal )
		goto abort;

	while( STRUCT_DIRENT* dp = readdir(dirp) )
	{
		res = report.OnIdle(path);

		if( res != kContinueTraversal )
			goto abort;

		if( strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 )
			continue;

		string fullname = path;
		NormalizeFolderPath(fullname);
		fullname += dp->d_name;

		macspec = 0L;
		
#if TARGET_RT_MAC_MACHO
		GUSIFileSpec inspec(foldermacspec);
		inspec += dp->d_name;
		UFSSpec infspec = inspec;
		macspec = &infspec;
#else
#	if defined(WIN32)
#		if ENABLE_WINCVSDEBUG
		if( (dbg_mask & wcvsdbg_trace_modified_checks) && (dbg_mask & wcvsdbg_verbose) )
		{
			AppendWinCvsDebugMask(wcvsdbg_trace_dst_checks | wcvsdbg_trace_stat);
			tracestr = "Stat(";
			tracestr << static_cast<const char*>(fullname);
			tracestr << ") ...\n";
			cvs_err(tracestr);
			OutputDebugString(tracestr);
		}
#		endif	// ENABLE_WINCVSDEBUG
#	endif	// defined(WIN32)
#endif

		struct stat sb;
		const int statres = stat(fullname.c_str(), &sb);
		if( statres != -1 )
		{
			if( S_ISDIR(sb.st_mode) )
			{
				res = report.OnDirectory(path, fullname.c_str(), dp->d_name, sb, macspec);

				if( res == kSkipFile )
					continue;

				if( res != kContinueTraversal )
					goto abort;
   
				res = FileTraverse(fullname.c_str(), report, macspec);

				if( res != kContinueTraversal )
					goto abort;
			}
			else if( S_ISREG(sb.st_mode) )
			{				
				res = report.OnFile(path, fullname.c_str(), dp->d_name, sb, macspec);
				
				if( res == kSkipFile )
					continue;

				if( res != kContinueTraversal )
					goto abort;
			}
#ifdef S_ISLNK
			else if( S_ISLNK(sb.st_mode) )
			{
				res = report.OnAlias(path, fullname.c_str(), dp->d_name, sb, macspec);
				
				if( res == kSkipFile)
					continue;

				if( res != kContinueTraversal )
					goto abort;
			}
#endif /* S_ISLNK */
			else
			{
				/* TODO : Unix devices */
				stringstream errText;
				errText << "Error while accessing ";
				errText << fullname;
				errText << " (error " << errno << ')';
				errText << ends;
				res = report.OnError(errText.str().c_str(), errno);

				goto abort;
			}
		}
		else
		{
			stringstream errText;
			errText << "Error while accessing ";
			errText << fullname;
			errText << " (error " << errno << ')';
			errText << ends;
			res = report.OnError(errText.str().c_str(), errno);
      
			// no abort, because all 
			// * files not read yet will be signed as missing and all 
			// * folder will not be displayed
			//goto abort;
		}

#ifdef qUnix
		app->PeekPumpAndIdle(true);
		
		// print actual folder
		app->SetStatusBarText(path);
#endif
	}
	
	res = report.ExitDirectory(path);

abort:
	closedir(dirp);

exit:
	if( NULL != originalPath )
	{
		chdir(originalPath);
		free(originalPath);
		originalPath = NULL;
	}

#if defined(WIN32)
#	if ENABLE_WINCVSDEBUG
	SetWinCvsDebugMask(dbg_mask);
#	endif	// ENABLE_WINCVSDEBUG
#endif	// defined(WIN32)

#ifdef qUnix
	// print actual folder
	app->SetStatusBarText("");
#endif
	
	return res;
}

//////////////////////////////////////////////////////////////////////////
// TraversalReport

/*!
	Get the reference to the traverse flag
	\return The reference to the traverse flag
*/
bool& TraversalReport::GetRefRunning()
{
	return TraversalReport::m_running;
}

kTraversal TraversalReport::EnterDirectory(const char* fullpath, const char* dirname, const UFSSpec* macspec)
{
	return kContinueTraversal;
}

kTraversal TraversalReport::ExitDirectory(const char* fullpath)
{
	return kContinueTraversal;
}

kTraversal TraversalReport::OnError(const char* err, int errcode)
{
	return kTraversalError;
}

kTraversal TraversalReport::OnIdle(const char* fullpath)
{
	return kContinueTraversal;
}

kTraversal TraversalReport::OnAnyDevice(const char* fullpath,
										const char* fullname,
										const char* name,
										const struct stat& dir, const UFSSpec* macspec)
{
	return kContinueTraversal;
}

kTraversal TraversalReport::OnDirectory(const char* fullpath,
										const char* fullname,
										const char* name,
										const struct stat& dir, const UFSSpec* macspec)
{
	return OnAnyDevice(fullpath, fullname, name, dir, macspec);
}

kTraversal TraversalReport::OnFile(const char* fullpath,
								   const char* fullname,
								   const char* name,
								   const struct stat& dir, const UFSSpec* macspec)
{
	return OnAnyDevice(fullpath, fullname, name, dir, macspec);
}

kTraversal TraversalReport::OnAlias(const char* fullpath,
									const char* fullname,
									const char* name,
									const struct stat& dir, const UFSSpec* macspec)
{
	return OnAnyDevice(fullpath, fullname, name, dir, macspec);
}
