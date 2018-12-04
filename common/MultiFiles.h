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

#ifndef MULTIFILES_H
#define MULTIFILES_H

#include <vector>
#include "TextBinary.h"
#include "ustr.h"

class CvsArgs;

/// Used internally
class FileEntry
{
public:
	FileEntry();
	FileEntry(const char* filename, const UFSSpec* macspec = NULL, const char* currRevision = NULL);
	FileEntry(const FileEntry& entry);

	inline FileEntry& operator=(const FileEntry& entry);

protected:
	std::string m_file;		/*!< Filename */
	std::string m_currRev;	/*!< Current revision */
	UFSSpec m_spec;			/*!< Mac spec */

friend class MultiFilesEntry;
friend class MultiFiles;
};

/// Used internally
class MultiFilesEntry
{
public:
	MultiFilesEntry();
	MultiFilesEntry(const char* path);
	MultiFilesEntry(const MultiFilesEntry& entries);

	MultiFilesEntry& operator=(const MultiFilesEntry& entries);

	void setdir(const char* newdir);
	void add(const char* file, const UFSSpec* spec = NULL, const char* currRevision = NULL);
	void del_last();
	
	inline bool get(const int index, std::string& path, std::string& fileName, std::string& currRev) const;

	inline int NumFiles(void) const;
	inline const char* GetDir() const;
	inline bool GetFullPath(const int index, std::string& fullPath) const;

	const char* add(CvsArgs& args) const;
	const char* addfolder(CvsArgs& args, std::string& uppath, std::string& folder) const;

protected:
	std::string m_dir;				/*!< Directory */
	std::vector<FileEntry> m_files;	/*!< Files */

friend class MultiFiles;
};

/// Store multiple files by directory
class MultiFiles
{
public:
	MultiFiles();
	MultiFiles(const MultiFiles& mf);

	MultiFiles& operator=(const MultiFiles& mf);

	void newdir(const char* dir);
	void newfile(const char* file, const UFSSpec* spec = NULL, const char* currRevision = NULL);

	typedef std::vector<MultiFilesEntry>::const_iterator const_iterator;
	typedef std::vector<MultiFilesEntry>::iterator iterator;
	
	inline const_iterator begin() const;
	inline const_iterator end() const;
	inline iterator begin();
	inline iterator end();

	bool getdir(const int index, std::string& path) const;

	inline int NumDirs(void) const;
	int TotalNumFiles(void) const;

	void reset();

	bool Normalize();
	
protected:
	std::vector<MultiFilesEntry> m_dirs;
};

/// A pair of MultiFiles::const_iterator and index allowing to identify the FileEntry
class MultiFilesEntryIndex
{
public:
	MultiFilesEntryIndex(const int index, MultiFiles::const_iterator mfi);

	inline int GetIndex() const;
	inline MultiFiles::const_iterator GetIterator() const;

private:
	int m_index;
	MultiFiles::const_iterator m_mfi;
};

//////////////////////////////////////////////////////////////////////////
// FileEntry inline implementation

inline FileEntry& FileEntry::operator=(const FileEntry& entry)
{
	m_file = entry.m_file;
	m_currRev = entry.m_currRev;
	m_spec = entry.m_spec;
	
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// MultiFilesEntry inline implementation

/*!
	Get the file in the current directory
	\param index File index
	\param path Return path
	\param fileName Return filename
	\param currRev Return current revision
	\return true on success, false otherwise
*/
inline bool MultiFilesEntry::get(const int index, std::string& path, std::string& fileName, std::string& currRev) const
{
	if( index < 0 || (size_t)index >= m_files.size() )
		return false;
	
	path = m_dir;
	fileName = m_files[index].m_file;
	currRev = m_files[index].m_currRev;
	
	return true;
}

/// Get the number of files in the current dir
inline int MultiFilesEntry::NumFiles(void) const
{
	return m_files.size();
}

/// Get the current directory
inline const char* MultiFilesEntry::GetDir() const
{
	return m_dir.c_str();
}

/*!
	Get the file in the current directory
	\param index File index
	\param fullPath Return full, normalized path of the file
	\return true on success, false otherwise
*/
inline bool MultiFilesEntry::GetFullPath(const int index, std::string& fullPath) const
{
	if( index < 0 || (size_t)index >= m_files.size() )
		return false;
	
	fullPath = m_dir;
	NormalizeFolderPath(fullPath);
	fullPath += m_files[index].m_file;

	return true;
}
//////////////////////////////////////////////////////////////////////////
// MultiFilesEntryIndex	inline implementation

/// Get the index
inline int MultiFilesEntryIndex::GetIndex() const
{
	return m_index;
}

/// Get the iterator
inline MultiFiles::const_iterator MultiFilesEntryIndex::GetIterator() const
{ 
	return m_mfi;
}

//////////////////////////////////////////////////////////////////////////
// 	MultiFiles inline implementation

/// Iterator begin
inline MultiFiles::const_iterator MultiFiles::begin() const
{
	return m_dirs.begin();
}

/// Iterator end
inline MultiFiles::const_iterator MultiFiles::end() const
{
	return m_dirs.end();
}

/// Iterator begin
inline MultiFiles::iterator MultiFiles::begin()
{
	return m_dirs.begin();
}

/// Iterator end
inline MultiFiles::iterator MultiFiles::end()
{
	return m_dirs.end();
}

/// Get the number of directories
inline int MultiFiles::NumDirs(void) const
{
	return m_dirs.size();
}

#endif /* MULTIFILES_H */
