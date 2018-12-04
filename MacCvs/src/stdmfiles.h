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
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

#ifndef STDMFILE_H
#define STDMFILE_H

#include "TextBinary.h"

#include <MacTypes.h>				// Boolean
#include <Files.h>				// FSSpecPtr


#ifdef __cplusplus
extern "C" {
#endif

// ---- resource ID of custom dialog box

#define	rSF_MFilesPromptDLOG		6043


// ---- prototypes

Boolean SF_PromptForMFiles(Point sfPoint, const StringPtr aPrompt);

// after "SF_PromptForMFiles", the functions to get
// all the files (optionnaly by folder)

typedef int SF_PFMF_INFO;
typedef void (*SF_PFMF_HOOK) (UFSSpec *newFile, void *data);

#define SF_PFMF_BeginTraversal(info) {*(info) = 0;}
Boolean SF_PFMF_GetCurrentDir(SF_PFMF_INFO *info, UFSSpec *dirSpec);
Boolean SF_PFMF_DoTraversal(SF_PFMF_INFO *info, Boolean byDir, SF_PFMF_HOOK hook, void *hook_data);
#define SF_PFMF_EndTraversal(info)

#ifdef __cplusplus
}
#endif

#endif // STDMFILE_H
