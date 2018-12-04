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
 * dll_glue.h --- glue code for the dll to communicate with the caller
 */

#ifndef DLL_GLUE_H
#define DLL_GLUE_H

#include <stdlib.h>
#include "dll_process.h"
#include <Events.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma export on

int	dllglue_decode_file (char *infile);
int dllglue_main(const char *path, int argc, char **argv);
#if	!TARGET_API_MAC_CARBON
	int dllglue_setgetenv(UniversalProcPtr agetenv);
	int dllglue_setconsoleout(UniversalProcPtr aconsoleout);
	int dllglue_setconsoleerr(UniversalProcPtr aconsoleerr);
	int dllglue_setconsolein(UniversalProcPtr aconsolein);
	int dllglue_seteventdispatch(UniversalProcPtr adispatch);
#else /* TARGET_API_MAC_CARBON */
	int dllglue_setgetenv(dllglue_getenv_func agetenv);
	int dllglue_setconsoleout(dllglue_consoleout_func aconsoleout);
	int dllglue_setconsoleerr(dllglue_consoleout_func aconsoleerr);
	int dllglue_setconsolein(dllglue_consolein_func aconsolein);
	int dllglue_seteventdispatch(dllglue_eventdispath_func adispatch);
#endif /* TARGET_API_MAC_CARBON */

#pragma export reset

/* glue code to intercept the getenv and exit */
#define exit dllglue_exit
#define getenv dllglue_getenv

void dllglue_exit(int code);
char * dllglue_getenv(const char *);

int dllglue_dispatchevent(EventRecord *userevent);
long dllglue_consoleout(char *buffer, long n);
long dllglue_consoleerr(char *buffer, long n);
long dllglue_consolein(char *buffer, long n);

/* console management, captures stdout and stderr */
void dllglue_flushconsole(void);

#ifdef __cplusplus
}
#endif

#endif /* DLL_GLUE_H */
