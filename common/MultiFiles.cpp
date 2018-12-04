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
 * MultiFiles.h --- class to store multiple files by directory
 */

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <map>
#include <utility>
#include <algorithm>
#include <string>

#include "MultiFiles.h"
#include "CvsArgs.h"
#include "AppConsole.h"
#include "AppGlue.h"

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

using namespace std;

/// Functor to compare MultiFilesEntry by directory path
class MatchDir_Test : public unary_function<const MultiFilesEntry&, bool>
{
	const std::string& m_dir; /*!< Folder to compare with */
	
public:
	MatchDir_Test(const std::string& dir) 
		: m_dir(dir) 
	{
	}
	
	result_type operator()(argument_type entry) const
	{
		return m_dir == entry.GetDir();
	}
};

//////////////////////////////////////////////////////////////////////////
// MultiFilesEntryIndex

MultiFilesEntryIndex::MultiFilesEntryIndex(const int index, MultiFiles::const_iterator mfi)
	: m_index(index), m_mfi(mfi)
{
}

//////////////////////////////////////////////////////////////////////////
// FileEntry

FileEntry::FileEntry()
{
}

FileEntry::FileEntry(const char* filename, const UFSSpec* macspec /*= NULL*/, const char* currRevision /*= NULL*/)
{
	m_file = filename;

	if( currRevision != 0L )
		m_currRev = currRevision;

  	if( macspec != 0L )
		m_spec = *macspec;
}

FileEntry::FileEntry(const FileEntry& entry)
{
	*this = entry;
}

//////////////////////////////////////////////////////////////////////////
// MultiFilesEntry

MultiFilesEntry::MultiFilesEntry() 
{
}

MultiFilesEntry::MultiFilesEntry(const char* path)
{
	m_dir = path;
}

MultiFilesEntry::MultiFilesEntry(const MultiFilesEntry& entries)
{
	*this = entries;
}

MultiFilesEntry& MultiFilesEntry::operator=(const MultiFilesEntry& entries)
{
	m_dir = entries.m_dir;
	m_files = entries.m_files;

	return *this;
}

/*!
	Set the directory
	\param newdir Directory to set
*/
void MultiFilesEntry::setdir(const char* newdir)
{
	m_dir = newdir;
}

/*!
	Add file
	\param file Filename
	\param spec Mac spec
	\param currRevision Current revision
*/
void MultiFilesEntry::add(const char* file, const UFSSpec* spec /*= NULL*/, const char* currRevision /*= NULL*/)
{
	FileEntry entry(file, spec, currRevision);
	m_files.push_back(entry);
}

/*!
	Remove last file added
*/
void MultiFilesEntry::del_last()
{
	m_files.pop_back();
}

/*!
	Add files to the command arguments
	\param args Command arguments
	\return The directory
*/
const char* MultiFilesEntry::add(CvsArgs& args) const
{
	vector<FileEntry>::const_iterator i;
	
	// Mark the end of command line switches and the beginning of files
	if( m_files.size() > 0 )
	{
		args.startfiles();
	}

	// Add the files
	for(i = m_files.begin(); i != m_files.end(); ++i)
	{
		args.addfile((*i).m_file.c_str(), m_dir.c_str(), &(*i).m_spec, (*i).m_currRev.c_str());
	}

	return m_dir.c_str();
}

/*!
	Add folder to the command arguments
	\param args Command arguments
	\param uppath Return up-directory
	\param folder Return folder or file
	\return The directory held by uppath
	\note To be used when ever the directory must be given as parameter rather than execute location (e.g. add folder or release command)
*/
const char* MultiFilesEntry::addfolder(CvsArgs& args, std::string& uppath, std::string& folder) const
{
	if( !SplitPath(m_dir.c_str(), uppath, folder) )
	{
		// Even thought something didn't work we will try to continue
		uppath = m_dir;
		folder = ".";
	}

	args.add(folder.c_str());

	return uppath.c_str();
}

//////////////////////////////////////////////////////////////////////////
// MultiFiles

MultiFiles::MultiFiles()
{
}

/*!
	Copy constructor
	\param mf Object to copy from
*/
MultiFiles::MultiFiles(const MultiFiles& mf)
{
	this->operator=(mf);
}

/*!
	Assignment operator
	\param mf Object to copy from
*/
MultiFiles& MultiFiles::operator=(const MultiFiles& mf)
{
	if( &mf != this )
		this->m_dirs = mf.m_dirs;

	return *this;
}

/*!
	Add a new directory
	\param dir Directory to add
*/
void MultiFiles::newdir(const char* dir)
{
	MultiFilesEntry entry(dir);
	m_dirs.push_back(dir);
}

/*!
	Add a new file to the current directory
	\param file Filename
	\param spec Mac spec
	\param currRevision Current revision
*/
void MultiFiles::newfile(const char* file, const UFSSpec* spec /*= NULL*/, const char* currRevision /*= NULL*/)
{
	if( m_dirs.size() == 0 )
		return;

	MultiFilesEntry& entry = m_dirs[m_dirs.size() - 1];
	entry.add(file, spec, currRevision);
}

/// Get the directory at the specified index, return false if failed
bool MultiFiles::getdir(const int index, std::string& path) const
{
	if( index < 0 || index >= (int)m_dirs.size() )
		return false;

	const MultiFilesEntry& entry = m_dirs[index];

	path = entry.m_dir;

	return true;
}

/// Reset for reuse
void MultiFiles::reset()
{
	m_dirs.erase(m_dirs.begin(), m_dirs.end());
}

/// Get the total number of files
int MultiFiles::TotalNumFiles(void) const
{
	int total = 0;

	vector<MultiFilesEntry>::const_iterator i;
	for(i = m_dirs.begin(); i != m_dirs.end(); ++i)
	{
		total += i->NumFiles();
	}

	return total;
}

/*!
	Adjust the file list so that we end up with a list that contains no file entries with partial path names
	\return true on success, false otherwise
*/
bool MultiFiles::Normalize()
{
	vector<MultiFilesEntry> workingCopy(m_dirs);
	m_dirs.clear();

	for(vector<MultiFilesEntry>::const_iterator i(workingCopy.begin()); i != workingCopy.end(); ++i)
	{
		if( i->m_files.empty() )
		{
			// special case for empty directories in MultiFiles:
			// just add the directory to our directory list
			vector<MultiFilesEntry>::iterator found = find_if(m_dirs.begin(), m_dirs.end(), MatchDir_Test(i->m_dir));
			if( found == m_dirs.end() )
			{
				// directory not found in list -> create new directory
				this->newdir(i->m_dir.c_str());
			}
		}
		else 
		{
			for(vector<FileEntry>::const_iterator j(i->m_files.begin()); j != i->m_files.end(); j++)
			{
				string fullDirPath, dir, name;
				SplitPath(j->m_file.c_str(), dir, name);

				if( j->m_file.rfind(kPathDelimiter) != string::npos ) 
				{
					// insert current directory path in front of partial path
					fullDirPath = i->m_dir;
					NormalizeFolderPath(fullDirPath);
					fullDirPath += dir;
				}
				else 
				{
					fullDirPath = i->m_dir; // file does not have a partial path
				}
				
				vector<MultiFilesEntry>::iterator found = find_if(m_dirs.begin(), m_dirs.end(), MatchDir_Test(fullDirPath));
				if( found != m_dirs.end() )
				{
					// directory already in list -> just append the new file entry
					found->add(name.c_str(), &j->m_spec, j->m_currRev.c_str());
				}
				else
				{
					// directory not found in list -> create new directory and append the file to the new directory
					this->newdir(fullDirPath.c_str());
					this->newfile(name.c_str(), &j->m_spec, j->m_currRev.c_str());
				}
			}
		}
	}

	return true;
}
