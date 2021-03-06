/*
 * Stollen from POV and added to MacCVS by
 * Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*==============================================================================
Project:	POV

Version:	3

File:		StdFolder.h

Description:
	Routines to Prompt user to choose a folder (not a file) via almost-StdFile dialog.
------------------------------------------------------------------------------
Author:
	Eduard [esp] Schwan
------------------------------------------------------------------------------
	from Persistence of Vision(tm) Ray Tracer
	Copyright 1996 Persistence of Vision Team
------------------------------------------------------------------------------
	NOTICE: This source code file is provided so that users may experiment
	with enhancements to POV-Ray and to port the software to platforms other 
	than those supported by the POV-Ray Team.  There are strict rules under
	which you are permitted to use this file.  The rules are in the file
	named POVLEGAL.DOC which should be distributed with this file. If 
	POVLEGAL.DOC is not available or for more info please contact the POV-Ray
	Team Coordinator by leaving a message in CompuServe's Graphics Developer's
	Forum.  The latest version of POV-Ray may be found there as well.

	This program is based on the popular DKB raytracer version 2.12.
	DKBTrace was originally written by David K. Buck.
	DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
------------------------------------------------------------------------------
Change History:
	950401	[esp]	Stole idea and converted some code from DTS Pascal sample, with lots of changes.
==============================================================================*/

#ifndef STDFOLDER_H
#define STDFOLDER_H

//#include "config.h"
#include <MacTypes.h>				// Boolean
#include <Files.h>				// FSSpecPtr

// ---- resource ID of custom dialog box

#define	rSF_FolderPromptDLOG		6042


// ---- prototypes

void	SF_SetSFCurrent(FSSpecPtr pTheFSFilePtr);
short	SF_GetSFCurVol(void);
long	SF_GetSFCurDir(void);
Boolean SF_PromptForFolder(Point sfPoint, const StringPtr aPrompt, FSSpec * sfrPtr);
long SF_GetFSSubDirID(FSSpecPtr theFSDir);

#endif // STDFOLDER_H
