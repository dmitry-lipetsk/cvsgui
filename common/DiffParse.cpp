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
 * Author : Kirill Müller <mail@kirill-mueller.de> --- February 2006
 */

/*
 * DiffParse.cpp : parse the CVS diff output
 */

#include "stdafx.h"

#include <stdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef TARGET_OS_MAC
#	include <unistd.h>
#endif /* TARGET_OS_MAC */

#include "DiffParse.h"
#include "AppConsole.h"
#include "getline.h"
#include <algorithm>

using namespace std;

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

//////////////////////////////////////////////////////////////////////////
// 	CCvsDiffConsole

CCvsDiffConsole::CCvsDiffConsole(MultiFiles* mf, const char* dir) 
	: CCvsFilterConsole("Index: ", CCvsFilterConsole::FILTER_OUT | CCvsFilterConsole::PRINT_MATCH)
{
	m_mf = mf;
	m_dir = dir;
}

void CCvsDiffConsole::OnFilter(const std::string& txt)
{
	if( !m_mf->NumDirs() )
		m_mf->newdir(m_dir.c_str());

	string txtBS = txt;

	if( kPathDelimiter != '/' )
	{
		replace(txtBS.begin(), txtBS.end(), '/', kPathDelimiter);
	}

	m_mf->newfile(txtBS.c_str());
}

/*!
	Launch a CVS diff command, parse the output, add the changed files to a MultiFiles collection
	\param dir Directory
	\param args Arguments
	\param files Differing files
	\return true on success, false otherwise
*/
bool CvsDiffParse(const char* dir, const CvsArgs& args, MultiFiles& files)
{
	CCvsDiffConsole myConsole(&files, dir);
	
	const int ret = launchCVS(dir, args.Argc(), args.Argv(), &myConsole);

	return ret != 2;
}
