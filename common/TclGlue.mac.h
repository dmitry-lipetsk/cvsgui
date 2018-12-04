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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2001
 */

/*
 * TclGlue.mac.h --- glue to the TCL language specific to mac/carbon
 */

#ifndef TCLGLUEMAC_H
#define TCLGLUEMAC_H

#include "dll_loader.h"

int CarbonCompatLoadLibrary(CompatConnectID * connID, const char *name_dll);
int CarbonCompatCloseLibrary(CompatConnectID * connID);
void *_CarbonCompatCallLibrary(CompatConnectID connID, const char *name_func);
void *_tcl_cfm_macho(void *func);

#endif // TCLGLUEMAC_H
