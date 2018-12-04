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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- August 1998
 */

/*
 * MacrosSetup.cpp --- hook the macros files
 */

#include "stdafx.h"

#include "string"

#ifndef WIN32
#	include <ctype.h>
#endif /* !WIN32 */

#ifdef HAVE_STRING_H
#	include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#	include <strings.h>
#endif

#ifdef HAVE_ERRNO_H
#	include <errno.h>
#endif

#include "MacrosSetup.h"
#include "FileTraversal.h"
#include "CvsPrefs.h"

#ifdef TARGET_OS_MAC
#	include "MacMisc.h"
#	include <errno.h>
#endif

#ifdef qMacCvsPP
#	include "MacCvsApp.h"
#endif /* qMacCvsPP */

using namespace std;

#ifdef WIN32
#	include "wincvs.h"

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#define MACROS_FOLDER "Macros"
#define PYTHONLIB_FOLDER "PythonLib"
	
static void GetLoc(std::string& path, const char* subFolder)
{
	path.erase();

#ifdef WIN32
	::GetAppPath(path);

	string appPath = path;
	// first the macro folder may be ./macro
	string test = appPath;
	NormalizeFolderPath(test);
	test += subFolder;

	struct stat sb;
	if(stat(test.c_str(), &sb) != -1 && S_ISDIR(sb.st_mode))
	{
		path = test;
		return;
	}

	string uppath, file;
	while( SplitPath(appPath.c_str(), uppath, file) && !uppath.empty() )
	{
		test = uppath.c_str();
		NormalizeFolderPath(test);
		test += subFolder;

		if( stat(test.c_str(), &sb) != -1 && S_ISDIR(sb.st_mode) )
		{
			path = test;
			return;
		}
		
		appPath = uppath.c_str();
	}
	path = test;
#elif defined(qUnix)
	struct stat sb;
	path = PACKAGE_DATA_DIR;
	if(stat(path.c_str(), &sb) == -1 || !S_ISDIR(sb.st_mode))
	{
		path = PACKAGE_SOURCE_DIR;
		std::string uppath, folder;
		SplitPath(path.c_str(), uppath, folder);
		path = uppath;
		NormalizeFolderPath(path);
		path += subFolder;
	}
#elif qMacCvsPP
	UStr appPath = CMacCvsApp::GetAppPath();
	if(!appPath.endsWith(kPathDelimiter))
		appPath << kPathDelimiter;
	UStr test = appPath;
	test << subFolder;

	struct stat sb;
	if(stat(test, &sb) != -1 && S_ISDIR(sb.st_mode))
	{
		path = test;
		return;
	}

	UStr uppath, file;
	while(SplitPath(appPath, uppath, file) && !uppath.empty())
	{
		test = uppath;
		if(!test.endsWith(kPathDelimiter))
			test << kPathDelimiter;
		test << subFolder;

		if(stat(test, &sb) != -1 && S_ISDIR(sb.st_mode))
		{
			path = test;
			return;
		}
		appPath = uppath;
	}
	path = test;
#endif
}

/*!
	Get the macros location
	\param path Return macros location
*/
void MacrosGetLoc(std::string& path)
{
	if( IsNullOrEmpty(gCvsPrefs.MacrosFolder()) )
	{
		FormatDefaultMacrosGetLoc(path);
	}
	else
	{
		path = gCvsPrefs.MacrosFolder();
	}
}

/*!
	Get python library root path
	\param path Return python library path
*/
void PythonLibGetLoc(std::string& path)
{
	if( IsNullOrEmpty(gCvsPrefs.PythonLibFolder()) )
	{
		FormatDefaultPythonLibGetLoc(path);
	}
	else
	{
		path = gCvsPrefs.PythonLibFolder();
	}
}

/*!
	Format the default macros folder string
	\param path Return the formatted macros folder string
*/
void FormatDefaultMacrosGetLoc(std::string& path)
{
	GetLoc(path, MACROS_FOLDER);
}

/*!
	Format the default PythonLib folder string
	\param path Return the formatted PythonLib folder string
*/
void FormatDefaultPythonLibGetLoc(std::string& path)
{
	GetLoc(path, PYTHONLIB_FOLDER);
}
