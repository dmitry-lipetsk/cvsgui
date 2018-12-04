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
 * TextBinary.cpp --- utilities to ckeck if files are binary or text
 */

#include "stdafx.h"

#ifdef WIN32
#	include <sys/stat.h>
#	include <direct.h>
#	include <io.h>
#endif /* WIN32 */

#ifdef TARGET_OS_MAC
#	include <Resources.h>
#	include <unistd.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include "MacMisc.h"
#endif /* TARGET_OS_MAC */

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "TextBinary.h"
#include "FileTraversal.h"
#include "AppConsole.h"
#include "CvsPrefs.h"
#include "ustr.h"

#if !defined(S_ISLNK) && defined(S_IFLNK)
#	if defined(S_IFMT)
#		define	S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#	else
#		define S_ISLNK(m) ((m) & S_IFLNK)
#	endif
#endif

#define MAX_TEST_SIZE 100000

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

static CStaticAllocT<char> sBuf;
static int sBufSize;

static kTextBinTYPE FillBuffer(const char *arg, const char *dir, const UFSSpec * spec, bool bin)
{
	sBuf.AdjustSize(MAX_TEST_SIZE);
	
	if(chdir(dir) != 0)
	{
		cvs_err("Unable to chdir to ""%s"" (error %d)\n", dir, errno);
		return kFileMissing;
	}
	
	FILE *samp = fopen(arg, bin ? "rb" : "r");
	if(samp == 0L)
	{
		return kFileMissing;
	}
	
	sBufSize = fread(sBuf, sizeof(char), MAX_TEST_SIZE, samp);

	fclose(samp);

	if(sBufSize < 0)
	{
		cvs_err("Unable to read ""%s"" (error %d)\n", arg, errno);
		return kFileMissing;
	}
	
	return kFileIsOK;
}

static kTextBinTYPE TestBuffer(const char *arg, const char *dir, const UFSSpec * spec, kFileType expectType)
{
	if(sBufSize == 0)
		return kFileIsOK;
	
	bool isUnicode = false;
#ifdef WIN32
	isUnicode = gCvsPrefs.WhichVersion() == kWin32 ? false : IsTextUnicode(sBuf, sBufSize, NULL) == TRUE;
#endif /* WIN32 */

	// Windows : fread skips the \r, so we don't care.
	// Mac : MSL exchanges when reading the \r and \n,
	// so we look for \n
	// Others : default to \n
	
	const unsigned char nativeNL = '\n';
	const unsigned char nonnativeNL = '\r';
	
	float percent = 0;
	int numBinChars = 0;
	int numTextChars = 0;

	if( !isUnicode )
	{
		// figure if it is binary
		int i;
		unsigned char c;
		char *tmp;
		
		for(tmp = sBuf, i = 0; i < sBufSize; i++)
		{
			c = (unsigned char)*tmp++;
			if((c < 0x20 && c != nativeNL && c != nonnativeNL && c != '\t') || (c >= 0x80
#ifdef TARGET_OS_MAC
				&& c != (unsigned char)'Ñ' && c != (unsigned char)'¥' /* powerplant */
#endif /* TARGET_OS_MAC */		
				))
				numBinChars++;
			else
				numTextChars++;
		}
		
		percent = (float)numBinChars / (float)sBufSize;
	}
	
	// more than 5% binary makes binary
	bool isbin = percent >= 0.05;

	switch( expectType )
	{
	case kFileTypeText:
		if( isUnicode )
		{
			return kTextIsUnicode;
		}
		else if( isbin )
		{
			return kTextIsBinary;
		}
		break;
	case kFileTypeBin:
		if( isUnicode )
		{
			return kBinIsUnicode;
		}
		else if( !isbin )
		{
			return kBinIsText;
		}
		break;
	case kFileTypeUnicode:
		if( !isUnicode )
		{
			return isbin ? kUnicodeIsBinary : kUnicodeIsText;
		}
		break;
	default:
#ifdef WIN32
		ASSERT(FALSE); // uknown type
#endif
		break;
	}

	// for the text files, check if the new lines are good
	if(kFileTypeText == expectType)
	{
		int i;
		unsigned char c;
		char *tmp;
		int numNonNative = 0, numNative = 0;
		for(tmp = sBuf, i = 0; i < sBufSize; i++)
		{
			c = (unsigned char)*tmp++;
			if(c == nonnativeNL)
				numNonNative++;
			else if(c == nativeNL)
				numNative++;
		}
		if(numNative == 0)
		{
			if(numNonNative > 0)
				return kTextWrongLF;
		}
		else
		{
			percent = (float)numNonNative / (float)numNative;
			if(percent >= 0.1)
				return kTextWrongLF;
		}
		
		if(numBinChars > 0)
			return kTextEscapeChar;
	}
	
	return kFileIsOK;
}

#if TARGET_RT_MAC_MACHO
static kTextBinTYPE GetMacSig(const char *arg, const char *dir, const UFSSpec * spec, bool expectbin)
{
	FSCatalogInfo info;
	FInfo finfo;
	
	OSErr err = FSGetCatalogInfo(spec, kFSCatInfoNodeFlags | kFSCatInfoFinderInfo |
		kFSCatInfoDataSizes | kFSCatInfoRsrcSizes, &info, NULL, NULL, NULL);
	if(err != noErr || (info.nodeFlags & kFSNodeIsDirectoryMask))
	{
		cvs_err("Unable to get info on ""%s"" (error %d)\n", arg, err);
		return kFileMissing;
	}
	
	memcpy(&finfo, &info.finderInfo, 2 * sizeof(OSType));
	
	if(expectbin)
	{
		if(finfo.fdType == 'TEXT')
			return kBinWrongSig;
	}
	else
	{
		if(info.rsrcPhysicalSize > 0 && info.dataPhysicalSize == 0)
			return kTextIsBinary;
			
		if(finfo.fdType != 'TEXT' && finfo.fdType != 0)
			return kTextWrongSig;
	}
	
	return kFileIsOK;
}
#endif /* TARGET_RT_MAC_MACHO */


static kTextBinTYPE TestValidName(const char *filename)
{
	kTextBinTYPE result = kFileIsOK;

	if(strchr(filename, '/') != 0L)
		result = kFileInvalidName;
	if(strchr(filename, '\n') != 0L)
		result = kFileInvalidName;

#ifdef TARGET_OS_MAC
	if(strchr(filename, '\r') != 0L)
		result = kFileInvalidName;
	if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
		result = kFileInvalidName;
	if(strcmp(filename, "CVS") != 0 && stricmp(filename, "cvs") == 0)
		result = kFileInvalidName;
#endif /* !TARGET_OS_MAC */

	return result;
}

static kTextBinTYPE TestFileIsAlias(const char *arg, const char *dir, const UFSSpec * spec)
{
	kTextBinTYPE result = kFileIsOK;

#ifdef S_ISLNK
	std::string fullname;
	struct stat sb;

	fullname = dir;
	NormalizeFolderPath(fullname);
	
	fullname += arg;

	if (stat(fullname.c_str(), &sb) == -1)
		result = kFileMissing;
	else if(S_ISLNK(sb.st_mode))
		result = kFileIsAlias;
#endif /* S_ISLNK */

	return result;
}

kTextBinTYPE FileIsText(const char *arg, const char *dir, const UFSSpec * spec)
{
	kTextBinTYPE state;

#ifdef TARGET_OS_MAC
	state = GetMacSig(arg, dir, spec, false);
	if(state != kFileIsOK)
		return state;
#endif /* TARGET_OS_MAC */
	
	state = FillBuffer(arg, dir, spec, false);
	if(state != kFileIsOK)
		return state;
	
	if(sBufSize == 0)
		return kFileIsOK;

	state = TestBuffer(arg, dir, spec, kFileTypeText);
	if(state != kFileIsOK)
		return state;

	state = TestValidName(arg);
	if(state != kFileIsOK)
		return state;
	
	state = TestFileIsAlias(arg, dir, spec);
	if(state != kFileIsOK)
		return state;

	return kFileIsOK;
}

kTextBinTYPE FileIsBinary(const char *arg, const char *dir, const UFSSpec * spec)
{
	kTextBinTYPE state;

	state = FillBuffer(arg, dir, spec, true);
	if(state != kFileIsOK)
		return state;
	
	state = TestBuffer(arg, dir, spec, kFileTypeBin);

#ifdef TARGET_OS_MAC
	kTextBinTYPE sigstate = GetMacSig(arg, dir, spec, true);
	if(sigstate != kFileIsOK)
		return state;
	if(state != kFileIsOK && state != kBinIsText)
		return state;
#else /* !TARGET_OS_MAC */
	if(state != kFileIsOK)
		return state;
#endif /* !TARGET_OS_MAC */

	state = TestValidName(arg);
	if(state != kFileIsOK)
		return state;
	
	state = TestFileIsAlias(arg, dir, spec);
	if(state != kFileIsOK)
		return state;

	return kFileIsOK;
}

kTextBinTYPE FileIsUnicode(const char *arg, const char *dir, const UFSSpec * spec /*= 0L*/)
{
	kTextBinTYPE state;

	state = FillBuffer(arg, dir, spec, true);
	if(state != kFileIsOK)
		return state;
	
	state = TestBuffer(arg, dir, spec, kFileTypeUnicode);

#ifdef TARGET_OS_MAC
	kTextBinTYPE sigstate = GetMacSig(arg, dir, spec, true);
	if(sigstate != kFileIsOK)
		return state;
	if(state != kFileIsOK && state != kBinIsText)
		return state;
#else /* !TARGET_OS_MAC */
	if(state != kFileIsOK)
		return state;
#endif /* !TARGET_OS_MAC */

	state = TestValidName(arg);
	if(state != kFileIsOK)
		return state;
	
	state = TestFileIsAlias(arg, dir, spec);
	if(state != kFileIsOK)
		return state;

	return kFileIsOK;
}
