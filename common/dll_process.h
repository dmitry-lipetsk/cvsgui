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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * dll_process.h --- allow to simulate a process using the dll mechanism
 */

#ifndef DLL_PROCESS_H
#define DLL_PROCESS_H

#ifdef TARGET_OS_MAC
#	include <CodeFragments.h>
#	include <Events.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

	typedef char * (*dllglue_getenv_func) (const char *);
	typedef long (*dllglue_consoleout_func) (char *, long);
	typedef long (*dllglue_consolein_func) (char *, long);
	typedef int (*dllglue_main_func) (const char *, int, char**);
	typedef int (*dllglue_setglue_func) (void *);
#	if TARGET_API_MAC_CARBON
	typedef int (*dllglue_eventdispath_func) (EventRecord *);
	typedef int (*dllglue_decode_file_func) (char *);
#	endif

#ifdef __cplusplus
}
#endif

#endif /* DLL_PROCESS_H */
