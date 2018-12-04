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

#include "GUSIInternal.h"
#include "GUSIFileSpec.h"

#include "stdfolder.h"
#include "stdmfiles.h"

#include <string.h>		// strcmp
#include <memory.h>		// BlockMove
#include <TextUtils.h>	// TruncString
#include <LowMem.h>		// LM...
#include <Lists.h>
#include <stdlib.h>

#	include <UConditionalDialogs.h>
#	include <UNavServicesDialogs.h>

static UFSSpec *	gFiles = NULL;
static int			gNumFiles = 0;

#ifdef __cplusplus
extern "C" {
#endif

static int sortFiles(UFSSpec *s1, UFSSpec *s2)
{
	FSCatalogInfo i1, i2;
	if(FSGetCatalogInfo(s1, kFSCatInfoParentDirID | kFSCatInfoVolume, &i1, NULL, NULL, NULL) == noErr &&
		FSGetCatalogInfo(s2, kFSCatInfoParentDirID | kFSCatInfoVolume, &i2, NULL, NULL, NULL) == noErr)
	{
		return i1.volume < i2.volume ? -1 :
			(i1.volume > i2.volume ? 1 :
				(i1.parentDirID < i2.parentDirID ? -1 :
					(i1.parentDirID > i2.parentDirID ? 1 : 0)));
	}
	return 0;
}

#ifdef __cplusplus
}
#endif

Boolean SF_PromptForMFiles(Point sfPoint, const StringPtr aPrompt)
{
	if(gFiles != NULL)
	{
		free(gFiles);
		gNumFiles = 0;
		gFiles = NULL;
	}

	UNavServicesDialogs::LFileChooser chooser;
	
	LFileTypeList inFileTypes(fileTypes_All);
	NavDialogOptions *options = chooser.GetDialogOptions();
	if(options != nil)
	{
		LStr255::CopyPStr(aPrompt, options->windowTitle);
		options->location = sfPoint;
	}
	
	if(!chooser.AskOpenFile(inFileTypes))
		return false;

	gNumFiles = chooser.GetNumberOfFiles();
	gFiles = (UFSSpec *)malloc(gNumFiles * sizeof(UFSSpec));
	for( int i = 1; i <= gNumFiles; i++)
	{
		FSSpec spec;
		chooser.GetFileSpec(i, spec);
		FSpMakeFSRef(&spec, &gFiles[i - 1]);
	}
	/* sort the names by folder */
	qsort(gFiles, gNumFiles, sizeof(UFSSpec), (int (*)(const void*, const void*))sortFiles);                      /* mm 961031 */

	return true;
} // .


Boolean SF_PFMF_GetCurrentDir(SF_PFMF_INFO *info, UFSSpec *dirSpec)
{
	int offset = *info;
	
	if(gFiles == NULL || offset >= gNumFiles || dirSpec == NULL)
		return false;
	
	*dirSpec = gFiles[offset];
	
	return FSGetCatalogInfo(dirSpec, kFSCatInfoNone, NULL, NULL, NULL, dirSpec) == noErr;
}

Boolean SF_PFMF_DoTraversal(SF_PFMF_INFO *info, Boolean byDir, SF_PFMF_HOOK hook, void *hook_data)
{
	int offset = *info, f;
	
	if(gFiles == NULL || offset >= gNumFiles)
		return false;
	
	for(f = offset; f < gNumFiles; f++)
	{
		if(sortFiles(&gFiles[offset], &gFiles[f]) != 0 || (!byDir && f > offset))
			break;
		hook(&gFiles[f], hook_data);
	}
	
	*info = f;
	return true;
}
