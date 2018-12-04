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
 * dll_garbage.h --- memory garbage collection for the dll
 */

#ifndef DLL_GARBAGE_H
#define DLL_GARBAGE_H

#include <stdlib.h>
#ifdef WIN32
#	include <malloc.h>
#endif /* WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

#if qCvsDebug
#	define malloc(s) shl_malloc(s, __LINE__, __FILE__)
#	define calloc(m, s) shl_calloc(m, s, __LINE__, __FILE__)
#	define realloc(p, s) shl_realloc(p, s, __LINE__, __FILE__)
	void *	shl_calloc(size_t nmemb, size_t size, int line, const char *file);
	void *	shl_malloc(size_t size, int line, const char *file);
	void *	shl_realloc(void * ptr, size_t size, int line, const char *file);
#else /* !qCvsDebug */
#	define malloc shl_malloc
#	define calloc shl_calloc
#	define realloc shl_realloc
	void *	shl_calloc(size_t nmemb, size_t size);
	void *	shl_malloc(size_t size);
	void *	shl_realloc(void * ptr, size_t size);
#endif /* !qCvsDebug */

#define free shl_free
void shl_free(void *ptr);

void garbyNew(void);
void garbyRemove(int verboseSummary, int verboseMemoryLeak);

#ifdef __cplusplus
}
#endif

#endif /* DLL_GARBAGE_H */
