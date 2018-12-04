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
 * CvsArgs.cpp --- class to handle cvs arguments
 */

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <string>
#include <sstream>

#include "CvsPrefs.h"
#include "CvsArgs.h"
#include "AppConsole.h"
#include "Authen.h"
#include "FileTraversal.h"
#include "MoveToTrash.h"

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

using namespace std;

#define MAX_PRINT_ARG 80

static CStaticAllocT<char> buf;

static char *mystrdup(const char *string)
{
	int size = strlen(string);
	char *result = (char *)malloc((size + 1) * sizeof(char));
	if(result == NULL)
	{
		cvs_err("Impossible to allocate %d bytes !\n", size + 1);
		exit(1);
	}
	strcpy(result, string);
	return result;
}

/// Reduce a long string for printing, add "..." if the length is up the limit
static char *reduceString(const char *str, CStaticAllocT<char> & buf)
{
	buf.AdjustSize(MAX_PRINT_ARG + 4); // "..." + '\0'
	size_t len = strlen(str);
	if(len < MAX_PRINT_ARG)
	{
		strcpy(buf, str);
	}
	else
	{
		memcpy((char *)buf, str, MAX_PRINT_ARG * sizeof(char));
		char *tmp = (char *)buf + MAX_PRINT_ARG;
		*tmp++ = '.';
		*tmp++ = '.';
		*tmp++ = '.';
		*tmp = '\0';
	}

	return buf;
}

/// Replace \n by "\n" so it's readable
static char *expandLF(const char *str, CStaticAllocT<char> & buf)
{
	int numLFs = 0;
	const char *runner = str;
	while((runner = strchr(runner, '\n')) != 0L)
	{
		numLFs++;
		runner++;
	}

	size_t len = strlen(str);
	buf.AdjustSize(len + numLFs + 1); // numLFs * "\n" + '\0'

	char *tmp = buf;
	char c;
	runner = str;
	while((c = *runner++) != '\0')
	{
		if(c == '\n')
		{
			*tmp++ = '\\';
			*tmp++ = 'n';
		}
		else
			*tmp++ = c;
	}
	*tmp++ = '\0';

	return buf;
}

//////////////////////////////////////////////////////////////////////////
// CvsArgs

CvsArgs::CvsArgs(bool defargs /*= true*/, bool verbose /*= false*/)
{
	m_argv = NULL;
	m_argc = 0;
	m_hasEndOpt = false;

	reset(defargs, verbose);
}

CvsArgs::CvsArgs(char* const* argv, int argc)
{
	m_argv = NULL;
	m_argc = 0;
	m_hasEndOpt = false;
	
	reset(false);

	for(int i = 0; i < argc; i++)
	{
		add(argv[i]);
	}
}

CvsArgs::~CvsArgs()
{
	reset(false);
}

/*!
	Reset the arguments
	\param defargs Add default arguments from preferences
	\param verbose Get full CVS output (e.g. bulk update for diff)
*/
void CvsArgs::reset(bool defargs /*= true*/, bool verbose /*= false*/)
{
	if( m_argv != 0L )
	{
		for(int i = 0; i < m_argc; i++)
		{
			if( m_argv[i] != 0L )
				free(m_argv[i]);
		}

		free(m_argv);
	}
		
	m_argv = NULL;
	m_argc = 0;
	m_hasEndOpt = false;
	
	if( defargs )
	{
		int numargs = 1;
		
		if( gCvsPrefs.Z9Option() )
			numargs++;

#ifndef WIN32
		if( !verbose && gCvsPrefs.QuietOption() )
			numargs++;
#else
		if( gCvsPrefs.CheckoutFileAttribute() != CHECKOUT_DEFAULT )
			numargs++;

		if( !verbose && gCvsPrefs.CvsMessages() != CVSMESSAGES_DEFAULT )
			numargs++;
#endif

#if INTERNAL_AUTHEN
		AuthenModel* curModel = AuthenModel::GetInstance(gAuthen.kind());
		bool encrypt = gCvsPrefs.EncryptCommunication() && curModel->HasEncryption();
		if( encrypt )
			numargs++;
		
		if( gCvsPrefs.AlwaysUseCvsroot() )
			numargs += 2;
#else
		bool encrypt = gCvsPrefs.EncryptCommunication();
		if( encrypt )
			numargs++;
#endif /* INTERNAL_AUTHEN */

		m_argv = (char**)malloc(numargs * sizeof(char*));
		m_argv[m_argc++] = mystrdup(CVS_CMD);

#if INTERNAL_AUTHEN
		if( gCvsPrefs.AlwaysUseCvsroot() )
		{
			m_argv[m_argc++] = mystrdup("-d");
			
			string root;
			const char* ccvsroot = gCvsPrefs;
			ccvsroot = Authen::skiptoken(ccvsroot);
			root = gAuthen.token();
			root += ccvsroot;
			
			m_argv[m_argc++] = mystrdup(root.c_str());
		}
#endif /* INTERNAL_AUTHEN */

		if( gCvsPrefs.Z9Option() )
		{
			ostringstream strLevel;
			strLevel << "-z";
			strLevel << gCvsPrefs.ZLevel();
			strLevel << ends;
			
			m_argv[m_argc++] = mystrdup(strLevel.str().c_str());
		}

#ifndef WIN32
		if( !verbose && gCvsPrefs.QuietOption() )
			m_argv[m_argc++] = mystrdup("-q");
#else
		switch( gCvsPrefs.CheckoutFileAttribute() )
		{
		case CHECKOUT_READONLY:
			m_argv[m_argc++] = mystrdup("-r");
			break;
		case CHECKOUT_WRITABLE:
			m_argv[m_argc++] = mystrdup("-w");
			break;
		default:
			break;
		}

		if( !verbose )
		{
			switch( gCvsPrefs.CvsMessages() )
			{
			case CVSMESSAGES_QUIET:
				m_argv[m_argc++] = mystrdup("-q");
				break;
			case CVSMESSAGES_REALYYQUIET:
				m_argv[m_argc++] = mystrdup("-Q");
				break;
			default:
				break;
			}
		}
#endif

		if( encrypt )
			m_argv[m_argc++] = mystrdup("-x");
	}
}

/*!
	Print the command line to the console
	\param indirectory The directory for the command
	\note Start of files marker is detected and not printed, end of options is printed
*/
void CvsArgs::print(const char* indirectory /*= 0L*/)
{
	static CStaticAllocT<char> buf;

	for(int i = 0; i < m_argc; i++)
	{
		string newarg = m_argv[i];
		if( newarg == "\n" )
		{
			continue;
		}

		bool hasLF = ::strchr(newarg.c_str(), '\n') != 0L;
		size_t len = newarg.length();

		if( len > MAX_PRINT_ARG )
			newarg = reduceString(newarg.c_str(), buf);

		if( hasLF )
			newarg = expandLF(newarg.c_str(), buf);

		bool hasSpace = ::strchr(newarg.c_str(), ' ') != 0L;
		if( hasSpace )
			cvs_out("\"");

		cvs_outstr(newarg.c_str(), newarg.length());
		
		if( hasSpace )
			cvs_out("\"");

		cvs_out(" ");
	}

	if( indirectory != NULL && strlen(indirectory) )
	{
		cvs_out("(in directory %s)", indirectory);
	}

	cvs_out("\n");
}

/*!
	Add argument
	\param arg Argument to be added
*/
void CvsArgs::add(const char* arg)
{
	if( m_argv == 0L )
		m_argv = (char**)malloc(2 * sizeof(char*));
	else
		m_argv = (char**)realloc(m_argv, (m_argc + 2) * sizeof(char*));

	m_argv[m_argc++] = arg != 0L ? mystrdup(arg) : 0L;
	m_argv[m_argc] = 0L;
}

/*!
	Add file
	\param arg File to be added
	\param dir Directory of the file
	\param spec UFS Spec
	\param currRevision Current revision
	\return true on success, false otherwise
*/
void CvsArgs::addfile(const char* arg, const char* /*dir*/, const UFSSpec* /*spec*/, const char* /*currRevision*/)
{
	if( !m_hasEndOpt )
	{
		endopt();
	}

	add(arg);
}

/*!
	Parse given argument string and add the sub-arguments
	\param arg Argument to be parsed
	\return The number of added sub-arguments
*/
int CvsArgs::parse(const char* arg)
{
	int res = 0;

	stringstream strArg;
	strArg << arg;
	string token;

	do{
		strArg >> token;
		if( !token.empty() )
		{
			res++;
			add(token.c_str());
		}
	}while( !strArg.eof() );

	return res;
}

/*!
	Add options end marker - double dash ("--")
*/
void CvsArgs::endopt()
{
	m_hasEndOpt = true;
	add("--");
}

/*!
	Add the files start marker - newline character ("\n")
	\note An options end marker is added automatically if it was not added before
	\todo Remove - it's not useful as we are not escaping backslash any more
*/
void CvsArgs::startfiles()
{
	if( !m_hasEndOpt )
	{
		endopt();
	}

#ifdef WIN32
	//add("\n");
#endif
}

#if INTERNAL_AUTHEN
/// Add current CVSROOT
void CvsArgs::addcvsroot()
{
	// Extract from the cvsroot
	const char* ccvsroot = gCvsPrefs;
	ccvsroot = Authen::skiptoken(ccvsroot);
	
	string root;
	
	root = gAuthen.token();
	root += ccvsroot;
	
	add("-d");
	add(root.c_str());
}
#endif /* INTERNAL_AUTHEN */

/// Get the arguments list
char* const* CvsArgs::Argv(void) const
{
	return m_argv;
}

/// Get the arguments count
int CvsArgs::Argc(void) const
{
	return m_argc;
}

//////////////////////////////////////////////////////////////////////////
// ControlCvsArgs

void ControlCvsArgs::addfile(const char* arg, const char* dir, const UFSSpec* spec,
	const char* currRevision)
{
	CvsArgs::addfile(arg, dir, spec);
	
	if( !gCvsPrefs.AddControl() )
		return;

	kTextBinTYPE state = FileIsType(arg, dir, spec);
	switch(state)
	{
	case kFileIsOK:
		break;

	case kFileMissing:
		// we can resurect a file which is missing by re-adding it
		break;

	case kFileIsAlias:
		cvs_err("Warning : \'%s\' is an alias\n", arg);
		SetProblem(true);
		break;

	case kFileInvalidName:
		cvs_err("Warning : \'%s\' has an invalid name\n", arg);
		SetProblem(true);
		break;

	case kTextWrongLF:
		cvs_err("Warning : \'%s\' seems to have the wrong line feed for this machine, you should correct it first\n", arg);
		SetProblem(true);
		break;

	case kUnicodeIsBinary: // fall thru intentional
	case kTextIsBinary:
		cvs_err("Warning : \'%s\' seems to be \'binary\', you should use the \'Add binary\' command instead...\n", arg);
		SetProblem(true);
		break;

	case kTextEscapeChar:
#ifdef TARGET_OS_MAC
		if(gCvsPrefs.IsoConvert() == ISO8559_none)
		{
			cvs_err("Warning : \'%s\' has some escape characters in it (0x00-0x20, 0x80-0xFF),\n", arg);
			cvs_err("Warning   so you may need to have the ISO8559 translation option set in the preferences\n");
			cvs_err("Warning   before commiting\n");
		}
#else /* !TARGET_OS_MAC */
		cvs_err("Warning : \'%s\' has some escape characters in it (0x00-0x20, 0x80-0xFF), you should correct it first\n", arg);
#endif /* !TARGET_OS_MAC */
		break;

	case kTextWrongSig:
#if TARGET_RT_MAC_MACHO
		cvs_err("Warning : \'%s\' has a file type different from \'TEXT\' or 0, you should correct it first\n", arg);
#else
		cvs_err("Warning : \'%s\' has a file type different from \'TEXT\', you should correct it first\n", arg);
#endif
		SetProblem(true);
		break;

	case kUnicodeIsText: // fall thru intentional
	case kBinIsText:
		cvs_err("Warning : \'%s\' seems to be a text file, you should consider to use the \'Add selection\' command instead\n", arg);
		SetProblem(true);
		break;

	case kBinWrongSig:
		cvs_err("Warning : \'%s\' has a file type of \'TEXT\', you should correct it first\n", arg);
		SetProblem(true);
		break;

	case kTextIsUnicode: // fall thru intentional
	case kBinIsUnicode:
		cvs_err("Warning : \'%s\' seems to be an Unicode file, you should consider to use the \'Add unicode\' command instead\n", arg);
		SetProblem(true);
		break;

	default:
		cvs_err("Warning : \'%s\' has some unknown problems\n", arg);
		SetProblem(true);
		break;
	}
}

/*!
	Get the problem indicator
	\return true if there is a problem, false if all correct
*/
bool ControlCvsArgs::HasProblem(void) const
{
	return m_gotProblem;
}

/*!
	Set the problem indicator
	\param newState New state of the problem indicator
*/
void ControlCvsArgs::SetProblem(bool newState) 
{
	m_gotProblem = newState;
}

//////////////////////////////////////////////////////////////////////////
// RemoveCvsArgs

RemoveCvsArgs::RemoveCvsArgs(bool defargs /*= true*/, bool verbose /*= false*/)
	: m_moveToRecycleBin(false),
	CvsArgs(defargs, verbose)
{
}

RemoveCvsArgs::RemoveCvsArgs(char* const* argv, int argc)
	: m_moveToRecycleBin(false),
	CvsArgs(argv, argc)
{
}

void RemoveCvsArgs::addfile(const char* arg, const char* dir, const UFSSpec* spec,
	const char* currRevision)
{
	CvsArgs::addfile(arg, dir, spec);

	if( !m_moveToRecycleBin )
	{
		return;
	}

	string fullpath(dir);
	NormalizeFolderPath(fullpath);
	fullpath += arg;

	// check the file is not already deleted
	struct stat sb;
	if( stat(dir, &sb) != -1 && S_ISDIR(sb.st_mode) &&
		stat(fullpath.c_str(), &sb) == -1 && 
		errno == ENOENT )
	{
		return;
	}

	if( !CompatMoveToTrash(arg, dir, spec) )
#ifdef WIN32
		cvs_err("Unable to send '%s' to the recycle bin\n", arg);
	else
		cvs_out("'%s' has been moved successfully to the recycle bin...\n", arg);
#else
		cvs_err("Unable to send '%s' to the trash\n", arg);
	else
		cvs_out("'%s' has been moved successfully to the trash...\n", arg);
#endif
}

/*!
	Set the move to recycle bin
	\param moveToRecycleBin New value
*/
void RemoveCvsArgs::SetMoveToRecycleBin(bool moveToRecycleBin)
{
	m_moveToRecycleBin = moveToRecycleBin;
}