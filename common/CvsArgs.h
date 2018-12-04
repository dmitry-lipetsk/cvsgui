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
 * CvsArgs.h --- class to handle cvs arguments
 */

#ifndef CVSARGS_H
#define CVSARGS_H

#include "TextBinary.h"
#include "Persistent.h"

//////////////////////////////////////////////////////////////////////////
// CvsArgs

/// CVS command line client abbreviation
#define CVS_CMD	"cvs"

/// Class to handle cvs arguments, append "cvs" and misc options defined by preferences ("-z9" for example)
class CvsArgs
{
public :
	CvsArgs(bool defargs = true, bool verbose = false);
	CvsArgs(char* const* argv, int argc);
	virtual ~CvsArgs();

	virtual void add(const char* arg);
	virtual void addfile(const char* arg, const char* dir = 0L, const UFSSpec* spec = 0L, const char* currRevision = 0L);
	
	virtual int parse(const char* arg);

	void endopt();
	void startfiles();

#if INTERNAL_AUTHEN
	void addcvsroot();
#endif /* INTERNAL_AUTHEN */

	void reset(bool defargs = true, bool verbose = false);

	void print(const char* indirectory = 0L);

	char* const* Argv(void) const;
	int Argc(void) const;

protected:
	char** m_argv;	/*!< Arguments list */
	int m_argc;		/*!< Arguments count */

private:
	bool m_hasEndOpt;	/*!< Flag to store whether the options end marker is already set */
};

//////////////////////////////////////////////////////////////////////////
// ControlCvsArgs

/// Base class for control arguments used by add commands (added files are tested appropriate)
class ControlCvsArgs : public CvsArgs
{
public:
	ControlCvsArgs() : m_gotProblem(false) { }
	
	virtual void addfile(const char* arg, const char* dir = 0L, const UFSSpec* spec = 0L,
		const char* currRevision = 0L);
	
	bool HasProblem(void) const;
	void SetProblem(bool newState);

protected:
	/// Virtual override to test the file type
	virtual kTextBinTYPE FileIsType(const char* arg, const char* dir, const UFSSpec* spec = 0L) = 0;

	/// Virtual override to get the type description
	virtual const char* TypeDesc() = 0;

private:
	bool m_gotProblem; /*!< Problem indicator */
};

//////////////////////////////////////////////////////////////////////////
// AddCvsArgs

/// Class to add files
class AddCvsArgs : public ControlCvsArgs
{
protected:
	virtual kTextBinTYPE FileIsType(const char* arg, const char* dir, const UFSSpec* spec = 0L)
	{
		return FileIsText(arg, dir, spec);
	}
	
	virtual const char* TypeDesc()
	{
		return "TEXT";
	}
};

//////////////////////////////////////////////////////////////////////////
// AddBinCvsArgs

/// Class to add binary files
class AddBinCvsArgs : public ControlCvsArgs
{
protected:
	virtual kTextBinTYPE FileIsType(const char* arg, const char* dir, const UFSSpec* spec = 0L)
	{
		return FileIsBinary(arg, dir, spec);
	}
	
	virtual const char* TypeDesc()
	{
		return "binary";
	}
};

//////////////////////////////////////////////////////////////////////////
// AddUnicodeCvsArgs

/// Class to add Unicode files
class AddUnicodeCvsArgs : public ControlCvsArgs
{
protected:
	virtual kTextBinTYPE FileIsType(const char* arg, const char* dir, const UFSSpec* spec = 0L)
	{
		return FileIsUnicode(arg, dir, spec);
	}
	
	virtual const char* TypeDesc()
	{
		return "Unicode";
	}
};

//////////////////////////////////////////////////////////////////////////
// RemoveCvsArgs

/// Class to remove files
class RemoveCvsArgs : public CvsArgs
{
public:
	RemoveCvsArgs(bool defargs = true, bool verbose = false);
	RemoveCvsArgs(char* const* argv, int argc);

	virtual void addfile(const char* arg, const char* dir = 0L,
		const UFSSpec* spec = 0L, const char* currRevision = 0L);

	void SetMoveToRecycleBin(bool moveToRecycleBin);

private:
	bool m_moveToRecycleBin; /*!< true to move files to the recycle bin */
};

#endif /* CVSPREFS_H */
