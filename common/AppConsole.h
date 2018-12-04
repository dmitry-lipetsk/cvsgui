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
 * AppConsole.h --- output to the console
 */

#ifndef APPCONSOLE_H
#define APPCONSOLE_H

#include <stdio.h>

void cvs_out(const char *format, ...);
void cvs_err(const char *format, ...);
void cvs_outstr(const char *str, size_t len);
void cvs_errstr(const char *str, size_t len);

typedef enum
{
	kNormal,
	kBrown,
	kGreen,
	kMagenta,
	kRed,
	kBlue,
	kBold,
	kItalic,
	kUnderline,
	kNL // newline
} kConsoleColor;

#ifdef WIN32
class CWincvsView;
#if _RICHEDIT_VER >= 0x0210
#	define WCHARFORMAT CHARFORMAT2
#else
#	define WCHARFORMAT CHARFORMAT
#endif
#endif /* WIN32 */

class CColorConsole
{
public:
	CColorConsole();
	virtual ~CColorConsole();

	CColorConsole & operator<<(char c);
	CColorConsole & operator<<(kConsoleColor color);
	CColorConsole & operator<<(const char *str);
	CColorConsole & operator<<(int value);
protected:
#ifdef WIN32
	CWincvsView * fView;
	CWincvsView * GetView();
	WCHARFORMAT oldFormat;
public:
	CColorConsole(CWincvsView * aview);
#endif /* WIN32 */
};

#if defined(TARGET_OS_MAC) && !defined(qMacCvsPP)
	// for the mpw tools
#	define cvs_out printf
#	define cvs_err printf
#endif /* TARGET_OS_MAC && !qMacCvsPP */

#endif /* APPCONSOLE_H */
