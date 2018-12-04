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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- June 1998
 */

/*
 * CvsIgnore.cpp --- parse the .cvsignore
 */

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "FileTraversal.h"
#include "AppConsole.h"
#include "CvsIgnore.h"
#include "getline.h"
#include "fnmatch.h"
#include "CvsPrefs.h"

#ifndef WIN32
#	include <ctype.h>
#endif /* !WIN32 */

#ifdef qMacCvsPP
#       include "MacCvsApp.h"
#       include "MacMisc.h"
#endif /* qMacCvsPP */

#ifdef WIN32
#	include "wincvs.h"	

#	ifdef _DEBUG
#		define new DEBUG_NEW
#		undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

using namespace std;

/*!
	Parse a line of space-separated wild cards and add them to the list
	\param ign Wild cards
	\param hold Hold
	\param ignlist Ignored list
	\note mostly duplicated from cvs/src/ignore.c
*/
static void ign_add(const char* ign, bool hold, ignored_list_type& ignlist)
{
	if (!ign || !*ign)
		return;

	int ign_hold = ignlist.size() == 0 ? -1 : (int)ignlist.size();

	for (; *ign; ign++)
	{
		const char *mark;

		/* ignore whitespace before the token */
		if (isspace (*ign))
			continue;

		/*
		* if we find a single character !, we must re-set the ignore list
		* (saving it if necessary).  We also catch * as a special case in a
		* global ignore file as an optimization
		*/
		if ((!*(ign+1) || isspace (*(ign+1))) && (*ign == '!' || *ign == '*'))
		{
			if (!hold)
			{
				/* permanently reset the ignore list */
				ignlist.erase(ignlist.begin(), ignlist.end());

				/* if we are doing a '!', continue; otherwise add the '*' */
				if (*ign == '!')
				{
					continue;
				}
			}
			else if (*ign == '!')
			{
				/* temporarily reset the ignore list */
				if (ign_hold >= 0)
				{
					ignlist.erase(ignlist.begin(), ignlist.begin() + ign_hold);
					ign_hold = -1;
				}
				continue;
			}
		}

		/* find the end of this token */
		for (mark = ign; *mark && !isspace (*mark); mark++)
			/* do nothing */ ;


		ignlist.push_back(string(ign).substr(0, mark - ign));

		if (*mark)
			ign = mark;
		else
			ign = mark - 1;
	}
}

/*!
	Read .cvsignore file and append entries to ignlist
	\param fileName Filename
	\param ignlist Ignored list
	\return true on success, false otherwise
*/
static bool ReadIgnoredFile(const char* fileName, ignored_list_type& ignlist)
{
	FILE *fp;
	char *line = NULL;
	size_t line_allocated = 0;

	fp = fopen(fileName, "r");
	if(fp != 0L)
	{
		while (getline (&line, &line_allocated, fp) >= 0)
			ign_add (line, false, ignlist);

		if (ferror (fp))
			cvs_err("Problem while parsing '%s'", fileName);

		fclose (fp);
	}
	
	if(line != 0L)
		free (line);

	return(fp != 0L);
}

/*!
	Open and parse the .cvsignore in the given folder
	\param ignlist Return ignore list
	\param path Folder path
*/
void BuildIgnoredList(ignored_list_type& ignlist, const char* path)
{
	ignlist.erase(ignlist.begin(), ignlist.end());

	static ignored_list_type deflist;
	static bool deflistparsed = false;
	
	if( !deflistparsed )
	{
#ifdef TARGET_OS_MAC
		UFSSpec theFolder;
		string home;
		
		if(MacGetPrefsFolder(theFolder, home) == noErr)
		{
#else /* !TARGET_OS_MAC */
		if( !IsNullOrEmpty(gCvsPrefs.Home()) )
		{
			string home(gCvsPrefs.Home());
#endif /* !TARGET_OS_MAC */
			NormalizeFolderPath(home);
			
			struct stat sb;
			if( stat(home.c_str(), &sb) != 0 || !S_ISDIR(sb.st_mode) )
			{
				cvs_err("Warning : unable to access your HOME path '%s'\n", gCvsPrefs.Home());
				cvs_err("          ~/.cvsignore will not be read\n");
			}

			home += ".cvsignore";

			// Read .cvsignore file from HOME directory.
			// If HOME dir doesn't have such a file then
			// try to read global file from executable path.
			if( ReadIgnoredFile(home.c_str(), deflist) == false )
			{
				string appPath;
				
#ifdef WIN32
				::GetAppPath(appPath);
#elif defined(qUnix)
				// Unix version uses only .cvsignore file in HOME dir
#else
#	ifdef qMacCvsPP
				appPath = CMacCvsApp::GetAppPath();
#	endif /* qMacCvsPP */
#endif
				if( !appPath.empty() )
				{
					NormalizeFolderPath(appPath);
					appPath += ".cvsignore";
					
					ReadIgnoredFile(appPath.c_str(), deflist);
				}
			}
		}

		deflistparsed = true;
	}
	else
	{
		ignlist = deflist;
	}
	
#if qUnix || TARGET_RT_MAC_MACHO
	// from cvs/src/ignore.c
	string ign_default(". .. core RCSLOG tags TAGS RCS SCCS .make.state\
 .nse_depinfo #* .#* cvslog.* ,* CVS CVS.adm .del-* *.a *.olb *.o *.obj\
 *.so *.Z *~ *.old *.elc *.ln *.bak *.BAK *.orig *.rej *.exe .DS_Store _$* *$");
	ign_add (ign_default.c_str(), false, ignlist);
#endif

	string ignPath(path);
	NormalizeFolderPath(ignPath);
	ignPath += ".cvsignore";

	// load the file from the current source directory
	ReadIgnoredFile(ignPath.c_str(), ignlist);
}

/*!
	Tells if the name match the .cvsignore
	\param filename Filename to match
	\param ignlist Ignore list to match
	\return true on success, false otherwise
*/
bool MatchIgnoredList(const char* filename, const ignored_list_type& ignlist)
{
	if( !IsNullOrEmpty(filename) )
	{
		for(ignored_list_type::const_iterator i = ignlist.begin(); i != ignlist.end(); ++i)
		{
			if( fnmatch(i->c_str(), filename, 0) == 0 )
			{
				return true;
			}
		}
	}

	return false;
}
