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
 * FileTraversal.h : class to traverse a file hierarchy
 */

#if !defined(AFX_FILETRAVERSAL_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_FILETRAVERSAL_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TextBinary.h"

#include <sys/stat.h>

#if !defined(S_ISDIR) && defined(S_IFDIR)
#	if defined(S_IFMT)
#		define	S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#	else
#		define S_ISDIR(m) ((m) & S_IFDIR)
#	endif
#endif

#if !defined(S_ISREG) && defined(S_IFREG)
#	if defined(S_IFMT)
#		define	S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#	else
#		define S_ISREG(m) ((m) & S_IFREG)
#	endif
#endif

#if !defined(S_ISLNK) && defined(S_IFLNK)
#	if defined(S_IFMT)
#		define	S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#	else
#		define S_ISLNK(m) ((m) & S_IFLNK)
#	endif
#endif

#ifndef MAX_PATH
#	ifdef PATH_MAX
#		define MAX_PATH PATH_MAX
#	else
#		define MAX_PATH 5000
#	endif
#endif

#ifdef WIN32
#	include "ndir.h"
#	include <direct.h>
#	define STRUCT_DIRENT struct direct
#else /* !WIN32 */
#	include <dirent.h>
#	define STRUCT_DIRENT struct dirent
#endif /* !WIN32 */

/// Traverse return codes enum
typedef enum
{
	kContinueTraversal,	/*!< Continue traversing */
	kStopTraversal,		/*!< Stop traversing */
	kSkipFile,			/*!< Skip file */
	kTraversalError		/*!< Error while traversing */
} kTraversal;

class TraversalReport;

kTraversal FileTraverse(const char* path, TraversalReport& report, const UFSSpec* macDirSpec = 0L);

/// Traverse report base class
class TraversalReport
{
private:
	// Data members
	static bool m_running;	/*!< Flag indicating that traversal is ongoing */
	
public:
	// Interface
	static bool& GetRefRunning();

	virtual kTraversal EnterDirectory(const char* fullpath, const char* dirname, const UFSSpec* macspec);
	virtual kTraversal ExitDirectory(const char* fullpath);
	
	virtual kTraversal OnError(const char* err, int errcode);
	virtual kTraversal OnIdle(const char* fullpath);

	virtual kTraversal OnAnyDevice(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec);

	virtual kTraversal OnDirectory(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec);

	virtual kTraversal OnFile(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec);

	virtual kTraversal OnAlias(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec);
};

#endif // !defined(AFX_FILETRAVERSAL_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
