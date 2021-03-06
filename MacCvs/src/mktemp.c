/* Copyright (C) 1991, 1992, 1993, 1996 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errors.h>

#include "mktemp.h"

#ifdef macintosh
#	define getpid TickCount
#endif /* macintosh */

/*
 * Modified for WinCvs/MacCVS : Alexandre Parenteau <aubonbeurre@hotmail.com> --- June 1998
 */

/* Generate a unique temporary file name from TEMPLATE.
   The last six characters of TEMPLATE must be "XXXXXX";
   they are replaced with a string that makes the filename unique.  */
char *
mktemp (char *tmpl)
{
  static const char letters[]
    = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  size_t len;
  size_t i;

  len = strlen (tmpl);
  if (len < 6 || strcmp (&tmpl[len - 6], "XXXXXX"))
    {
      errno = EINVAL;
      return NULL;
    }

  if (sprintf (&tmpl[len - 5], "%.5u",
	       (unsigned int) getpid () % 100000) != 5)
    /* Inconceivable lossage.  */
    return NULL;

  for (i = 0; i < sizeof (letters); ++i)
    {
      struct stat ignored;

      tmpl[len - 6] = letters[i];

      if (stat (tmpl, &ignored) < 0 && errno == ENOENT)
	/* The file does not exist.  So return this name.  */
	return tmpl;
    }

  /* We return the null string if we can't find a unique file name.  */
  tmpl[0] = '\0';
  return tmpl;
}
