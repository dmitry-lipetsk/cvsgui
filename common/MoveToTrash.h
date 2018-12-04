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
 * MoveToTrash.h --- send the files to the trash
 * Author : Miro Jurisic <meeroh@MIT.EDU> --- December 1997
 * Modified : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

#ifndef MOVETOTRASH_H
#define MOVETOTRASH_H

#include "TextBinary.h"

bool CompatMoveToTrash(const char *file, const char *dir = 0L, const UFSSpec *spec = 0L);
	// return false if failed

void CompatBeginMoveToTrash(void);
void CompatEndMoveToTrash(void);

#endif /* MOVETOTRASH_H */
