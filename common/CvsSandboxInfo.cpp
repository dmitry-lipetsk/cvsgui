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
 * Author : Oliver Giesen <ogware@gmx.net>
 */

// CvsSandboxInfo.cpp: implementation file
//

#include "stdafx.h"
#include "getline.h"
#include "wincvs.h"
#include "CvsSandboxInfo.h"
#include <fstream>

using std::ifstream;
using std::strstream; 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCvsSandboxInfo

CCvsSandboxInfo::CCvsSandboxInfo(const char* path)
	: m_path(path)
{
}

CCvsSandboxInfo::~CCvsSandboxInfo()
{
}

/*!
	Read administrative file data
	\param name File name to read
	\return Administrative file data
*/
string CCvsSandboxInfo::ReadAdminFile(const char* name) const
{
	string res;

	string adminPath(m_path.c_str());
	NormalizeFolderPath(adminPath);
	adminPath += "CVS";
	adminPath += kPathDelimiter;
	
	string filename = adminPath;
	filename += name;

	ifstream adminFile(filename.c_str());
	if( adminFile.is_open() )
	{
		std::getline(adminFile, res);
	}

	return res;
}

/*!
	Get the repository
	\return CVS/Repository data
*/
string CCvsSandboxInfo::GetRepository() const
{
	return ReadAdminFile("Repository");
}

/*!
	Get the root
	\return CVS/Root data
*/
string CCvsSandboxInfo::GetRoot() const
{
	return ReadAdminFile("Root");
}
