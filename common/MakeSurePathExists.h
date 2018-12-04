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
 * MakeSurePathExists.h --- check if the directory exists and prompt to create one if it doesn't
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- January 27, 2001
 * Modified : 
 */

#ifndef __MAKESUREPATHEXISTS_H__
#define __MAKESUREPATHEXISTS_H__

bool CreateFolder(const char* fn);
bool CompatMakeSurePathExists(const char* path, bool promptForCreation = true);

#endif //__MAKESUREPATHEXISTS_H__
