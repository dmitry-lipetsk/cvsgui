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
 * MakeSurePathExists.cpp --- check if the directory exists and prompt to create one if it doesn't
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- January 27, 2001
 * Modified : 
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include <sys\stat.h>
#	include <direct.h>
#	include <errno.h>
#endif /* WIN32 */

#if qUnix
#endif

#ifdef qMacCvsPP
#endif /* qMacCvsPP */

#include "MakeSurePathExists.h"
#include "FileTraversal.h"
#include "CvsAlert.h"

/*!
	Check whether the specified path exists and optionally prompt for creation if not existing
	\param path	Path to check
	\param promptForCreation true to prompt for creation, false to only check existance
	\return true if the path exists, false otherwise
*/
bool CompatMakeSurePathExists(const char* path, bool promptForCreation /*= true*/)
{
	bool valid = false;
	
#ifdef WIN32
	TCHAR fn[MAX_PATH];
	if( _tfullpath(fn, path, MAX_PATH) )
	{
		struct _tstat dirstat;
		if( _tstat(fn, &dirstat) == 0 )
		{
			if( dirstat.st_mode & _S_IFDIR )
			{
				valid = true;
			}
		}
		else
		{
			// Not found, ask for permission to create
			if( promptForCreation )
			{
				CvsAlert cvsAlert(kCvsAlertQuestionIcon, 
					"Do you want to create folder?", "Folder " + CString(path) + " not found.", 
					BUTTONTITLE_YES, BUTTONTITLE_NO);
				
				if( cvsAlert.ShowAlert() == kCvsAlertOKButton )
				{
					valid = CreateFolder(fn);
				}
			}
		}
	}
#endif
	
	return valid;
}

/*!
	Basic function to create path leading to the folder
	\param fn Folder to create
	\return true on success, false otherwise
*/
bool CreateFolder(const char* fn)
{
#ifdef WIN32
	// Remove possible trailing separator
	TCHAR folder[MAX_PATH];
	_tcscpy(folder, fn);
	int len = _tcslen(folder);
	if( len > 1 )
	{
		switch( folder[len - 1] )
		{
		case _T('\\'):
		case _T('/'):
			folder[len - 1] = 0;
			break;
		}
	}
	
	// Try to create
	if( _tmkdir(folder) == 0 )
	{
		return true;
	}
	
	// If path does not exist, create parent folder and then try again
	if( errno == ENOENT )
	{
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_PATH];
		TCHAR fname[_MAX_PATH];
		
		_tsplitpath(folder, drive, dir, fname, NULL);
		if( !*fname )
		{
			return false;
		}

		_tmakepath(fname, drive, dir, NULL, NULL);
		if( !CreateFolder(fname) )
		{
			return false;
		}
		
		return _tmkdir(folder) == 0;	
	}
#endif
	
	// Not possible to create
	return false;
}
