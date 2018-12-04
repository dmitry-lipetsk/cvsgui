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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- May 1999
 */

#include "stdafx.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#	ifdef _DEBUG
#		include <crtdbg.h>
#	endif
#endif

#if qMacAPP && qDebug
#	include "Debug_AC.h"
#endif

#include <stdio.h>
#include <stdarg.h>

#include "uconsole.h"

void UAppConsole(const char *format, ...)
{
	va_list args;
	char mess[1024] = {'\0'};

	UASSERT(format != NULL);

	va_start (args, format);
	vsprintf (mess, format, args);
	va_end (args);

#if qGTK
	g_warning(mess);
#elif defined(qUnix)
#elif qMacAPP
#	if qDebug
	DebugWarning_AC(1, mess, __FILE__, __LINE__);
#	endif
#elif qMacPP
	SignalString_(mess);
#elif defined(WIN32)
#	ifdef _DEBUG
	_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, mess);
#	endif
#else
	(void)printf(mess);
	(void)fflush(stdout);
#endif
}

