/* Copyright (C) 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.  */

/*
 * Modified for WinCvs/MacCVS : Alexandre Parenteau <aubonbeurre@hotmail.com> --- June 1998
 */

#ifndef	_FNMATCH_H

#define	_FNMATCH_H	1

#ifdef __cplusplus
extern "C" {
#endif

/* Bits set in the FLAGS argument to `fnmatch'.  */
#undef FNM_PATHNAME
#define	FNM_PATHNAME	(1 << 0)/* No wildcard can ever match `/'.  */
#undef FNM_NOESCAPE
#define	FNM_NOESCAPE	(1 << 1)/* Backslashes don't quote special chars.  */
#undef FNM_PERIOD
#define	FNM_PERIOD	(1 << 2)/* Leading `.' is matched only explicitly.  */
#undef __FNM_FLAGS
#define	__FNM_FLAGS	(FNM_PATHNAME|FNM_NOESCAPE|FNM_PERIOD)

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#undef FNM_NOMATCH
#define	FNM_NOMATCH	1

#if !defined(__STDC__) && (defined(WIN32)|| defined(TARGET_OS_MAC))
#	define __STDC__ 1
#endif

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
#if __STDC__
extern int fnmatch (const char *pattern, const char *string, int flags);
#else
extern int fnmatch ();
#endif

#if defined (__CYGWIN32__) || defined (WIN32)
	/* Under Windows NT, filenames are case-insensitive, and both / and \
	   are path component separators.  */

#	define FOLD_FN_CHAR(c) (WNT_filename_classes[(unsigned char) (c)])
	extern unsigned char WNT_filename_classes[];
#endif /* defined (__CYGWIN32__) || defined (WIN32) */

#ifdef __cplusplus
}
#endif

#endif	/* fnmatch.h */
