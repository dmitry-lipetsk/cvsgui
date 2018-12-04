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
 * TextBinary.h --- utilities to ckeck if files are binary or text
 */

#ifndef TEXTBINARY_H
#define TEXTBINARY_H

#ifdef TARGET_OS_MAC
#	include <Files.h>
#	if TARGET_API_MAC_OSX
#		define UFSSpec FSRef
#	else
#		define UFSSpec FSSpec
#	endif
#else /* !TARGET_OS_MAC */
	typedef int FSSpec;
#	define UFSSpec FSSpec
#endif /* !TARGET_OS_MAC */

//! File type enum
typedef enum
{
	// text and binary flags
	kFileIsOK,			/*!< good file */
	kFileMissing,		/*!< file is missing or unreadable */
	kFileIsAlias,		/*!< file is an alias */
	kFileInvalidName,	/*!< invalid character ('/') */
	
	// text flags
	kTextWrongLF,		/*!< for example it has Mac \\r on Windows */
	kTextIsBinary,		/*!< file should be text but seems to be binary */
	kTextEscapeChar,	/*!< file has some extras characters (0x00-0x1F, 0x80-0xFF) */
	kTextWrongSig,		/*!< file has not the 'TEXT' signature (Mac only) */
	kTextIsUnicode,		/*!< file should be text but seems to be unicode file */

	// binary flags
	kBinIsText,			/*!< binary file seems to be a text file */
	kBinWrongSig,		/*!< file should not have the 'TEXT' signature in it (Mac only) */
	kBinIsUnicode,		/*!< binary file seems to be unicode file */

	// unicode flags
	kUnicodeIsText,		/*!< unicode file seems to be text file */
	kUnicodeIsBinary	/*!< unicode file seems to be binary */
} kTextBinTYPE;

//! File type enum
typedef enum
{
	kFileTypeText,		/*!< Text type */
	kFileTypeBin,		/*!< Binary type */
	kFileTypeUnicode	/*!< Unicode type */
} kFileType;

kTextBinTYPE FileIsText(const char *arg, const char *dir, const UFSSpec * spec = 0L);
kTextBinTYPE FileIsBinary(const char *arg, const char *dir, const UFSSpec * spec = 0L);
kTextBinTYPE FileIsUnicode(const char *arg, const char *dir, const UFSSpec * spec = 0L);

#endif /* TEXTBINARY_H */
