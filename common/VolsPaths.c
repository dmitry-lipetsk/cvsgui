/*
 * Stollen from POV and added to MacCVS by
 * Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*==============================================================================
Project:	POV-Ray

Version:	3

File:	VolsPaths.c

Description:
	Various volume and pathname handling routines.
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
	941221	[esp]	Created
==============================================================================*/

#define VOLSPATHS_C

/*==== my header =====*/
#include "VolsPaths.h"


/*==== Macintosh-specific headers ====*/
#include <Types.h>
#include <Files.h>
#include <Resources.h>
#include <errors.h>			/* dupFNErr, etc */
#include <Memory.h>			/* NewPtr, etc */
#include <string.h>			/* strcpy/cat */
#include <strings.h>		/* p2cstr */
#include <TextUtils.h>		/* Munger */


/*==== Standard C headers ====*/


/*==== General definitions ====*/


/*==== Global variables (external scope) ====*/


/*==== Global variables (local scope) ====*/


// ---------------------------------------------------------------------
// VRef2VolName
// ---------------------------------------------------------------------
OSErr VRef2VolName(short theVRef, StringPtr theVolName)
{
	OSErr			anError;
	HVolumeParam	pbVInfo;

	pbVInfo.ioCompletion = NULL;
	theVolName[0] = '\0';
	pbVInfo.ioNamePtr = theVolName;
	pbVInfo.ioVRefNum = theVRef;
	pbVInfo.ioVolIndex = 0; // use VRefNum only
	anError = PBHGetVInfoSync((HParmBlkPtr)&pbVInfo);

	return anError;
} // VRef2VolName


// ---------------------------------------------------------------------
// VolName2VRef
// ---------------------------------------------------------------------
OSErr VolName2VRef(StringPtr myPVolName, short * theVRefPtr)
{
	OSErr			anError = noErr;
	Boolean			foundIt = false;
	short			theDriveNum;
	short			theVRefNum;
	HParamBlockRec	aParamBlk;
	char			myCVolName[32], theCVolName[32];
	Str31			theVolName;

	// convert passed vol name from P 2 C string for compare
	p2cstrcpy(myCVolName, myPVolName);

	// loop through all mounted volumes, see if myPVolName matches one
	for (theDriveNum = 1; !foundIt && (anError==noErr); theDriveNum++)
	{
		theVolName[0] = '\0';
		aParamBlk.volumeParam.ioNamePtr = (StringPtr)&theVolName;
		aParamBlk.volumeParam.ioVolIndex = theDriveNum;
		
		anError = PBHGetVInfoSync((HParmBlkPtr)&aParamBlk);
		if (anError == noErr)
		{
			p2cstrcpy(theCVolName, theVolName);
			if (strcmp(theCVolName, myCVolName)==0)
			{
				foundIt = true;
				theVRefNum = aParamBlk.volumeParam.ioVRefNum;
			}
		}
	}

	if (foundIt)
		*theVRefPtr = theVRefNum;
	else
		*theVRefPtr = 0;

	if (anError == nsvErr) // no such vol, just bumped end of volume list
		anError = noErr;

	return anError;
} // VolName2VRef


// ---------------------------------------------------------------------
// PathNameFromDirID
// ---------------------------------------------------------------------
/*
Function:	PathNameFromDirID

Parameters:
	aDirID		the directory ID to start at
	avRefNum	the volume ref # to start at
	aPathStyle	The style of pathname to format (based on OS)
	aPath		the final path string is placed in this buffer

Return Value:
	zero is returned if OK, else an error is returned

Purpose/Description:
	PathNameFromDirID takes a real vRefNum and a DirID and returns
	the full pathname that corresponds to it.  It does this by calling
	_PBGetCatInfo for the given directory and finding out its name and
	the DirID of its parent. It then performs the same operation on the
	parent, sticking its name onto the beginning of the first directory.
	This whole process is continued until we have processed everything up
	to the root directory (identified with a DirID of 2). 
*/
OSErr PathNameFromDirID(const long			aDirID,
						const short			avRefNum,
						char				* aPath)
{
	Handle		strHandle = NULL;
	OSErr		anError = noErr;
	long		currDirID = 0,
				nextDirID = 0;
	Size		theLen = 0;		// GetHandleSize
	char		tempName[64];	// directory or volume name
	CInfoPBRec	aPBblock;

	/* create an empty string handle for Munger */
	strHandle = NewHandle(0);

	if (strHandle == NULL)
	{
		anError = MemError();
	}
	else
	{
		*aPath = '\0';
		aPBblock.dirInfo.ioCompletion	= NULL;
		aPBblock.dirInfo.ioNamePtr		= (StringPtr)tempName;	// temp destination buffer
		nextDirID						= aDirID;	// start at DirID passed in..

		/* walk up the directories, collecting dir names as we go, until we hit the top (volname) */
		do {
			
			aPBblock.dirInfo.ioVRefNum		= avRefNum;
			aPBblock.dirInfo.ioFDirIndex	= -1; // ignore ioNamePtr input, use ioDirID
			aPBblock.dirInfo.ioDrDirID		= nextDirID;

			anError = PBGetCatInfoSync(&aPBblock);
			currDirID	= aPBblock.dirInfo.ioDrDirID;	// remember current
			nextDirID	= aPBblock.dirInfo.ioDrParID;	// find parent for next loop

			if (anError != noErr)
				break;

			/* Insert a separator in front of the Directory Entry */
			Munger(strHandle, 0L, NULL, 0L, ":", 1L);

			/* Insert the next directory/volume name into the front of the string */
			Munger(strHandle, 0L, NULL, 0L, &(tempName[1]), (long)tempName[0]);

			/* NOTE: fsRtDirID is defined in Files.h */
		} while (currDirID != fsRtDirID);

		/* now move the string into the parameter */
		theLen = GetHandleSize(strHandle);
		if (!anError && theLen)
		{
			BlockMove(&(**strHandle), aPath, theLen);
			anError = MemError();
		}
		aPath[theLen] = '\0';	/* set C string length */
	
		/* dispose of temp storage */
		DisposeHandle(strHandle);

	} /*else NULL*/

	return(anError);

} /* PathNameFromDirID */


// ---------------------------------------------------------------------
// PathNameFromWD
// ---------------------------------------------------------------------
/*
Function:	PathNameFromWD

Parameters:
	avRefNum	the volume ref # to start at (Assumes WD is set)
	aPathStyle	The style of pathname to format (based on OS)
	aPath		the final path string is placed in this buffer

Return Value:
	Describe the value returned, or void

Purpose/Description:
	PathNameFromWD takes an HFS Working Directory ID and returns
	the full pathname that corresponds to it.  It does this by calling
	_PBGetWDInfo to get the vRefNum and DirID of the real directory.
	It then calls PathNameFromDirID and returns its result.  
*/
#if !TARGET_API_MAC_CARBON
OSErr PathNameFromWD(	const short			avRefNum,
						char				* aPath)
{
    WDPBRec	aPBblock;
	OSErr	anError	= noErr;

/*
 PBGetWDInfo has a bug under A/UX 1.1.  If vRefNum is a real vRefNum
 and not a wdRefNum, then it returns garbage.  Since A/UX has only 1
 volume (in the Macintosh sense) and only 1 root directory, this can
 occur only when a file has been selected in the root directory (/).
 So we look for this and hard code the DirID and vRefNum.
 */
#if defined(AUX)
    if ((aPathStyle == ePathTypeAUX) && (avRefNum == -1))
	{
		anError = PathNameFromDirID(fsRtDirID, -1, aPath);
	}
	else
#endif
	{
		aPBblock.ioCompletion = NULL;
		aPBblock.ioNamePtr = NULL;
		aPBblock.ioVRefNum = avRefNum;
		aPBblock.ioWDIndex = 0;
		aPBblock.ioWDProcID = 0;
	
		/* Change the Working Directory number in vRefnum into a real vRefnum */
		/* and DirID. The real vRefnum is returned in ioVRefnum, and the real */
		/* DirID is returned in ioWDDirID. */
	
		anError = PBGetWDInfoSync(&aPBblock);
		if (anError == noErr)
			anError = PathNameFromDirID(aPBblock.ioWDDirID,
										aPBblock.ioWDVRefNum,
										aPath);
	}

	return(anError);

} /* PathNameFromWD */
#endif
