/*
 * Stollen from POV and added to MacCVS by
 * Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*==============================================================================
Project:	POV

Version:	3

File:		StdFolder.c

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

#define STDFOLDER_C

#include "stdfolder.h"

#include <string.h>		// strcmp
#include <memory.h>		// BlockMove
#include <textutils.h>	// TruncString
#include <LowMem.h>		// LM...

#if __MWERKS__ >= 0x2200
#	define qUsePowerPlant true
#endif

#if qUsePowerPlant
#	include <UConditionalDialogs.h>
#	include <UNavServicesDialogs.h>
#endif

#define	kFolderBit		4	// bit set in ioFlAttrib for a directory

#define	kGetDirBTN		10	// dlog item, Select directory button
#define	kDirNameST		11	// dlog item, Select directory name static text
#define	kPromptST		12	// dlog item, Prompt static text


static Boolean		gDirSelectionFlag;
static Str63		gPrevSelectedName;
static Str255		gThePrompt;


// ---------------------------------------------------------------------
// Set default SF Getfile dialog initial directory
void SF_SetSFCurrent(FSSpecPtr pTheFSFilePtr)
{
	OSErr		anError = noErr;

	// Standard File wants the dirID and the negated vRefnum..
	// so give it to 'em that way
	LMSetCurDirStore(pTheFSFilePtr->parID);
	LMSetSFSaveDisk(-(pTheFSFilePtr->vRefNum));

} // SF_SetSFCurrent


// ---------------------------------------------------------------------
// Get default SF Getfile dialog initial volume
short SF_GetSFCurVol(void)
{
	return -LMGetSFSaveDisk();
} // SF_GetSFCurVol


// ---------------------------------------------------------------------
// Get default SF Getfile dialog initial directory
long SF_GetSFCurDir(void)
{
	return LMGetCurDirStore();
} // SF_GetSFCurDir


/*
---------------------------------------------------------------------------------
	The file filter function used to select directories is quite simple; it
	ensures that only directories, not files, are listed in the dialog box
	displayed by CustomGetFile.  The function MyCustomFileFilter simply
	inspects the appropriate bit in the file attributes (ioFlAttrib) field of
	the catalog information parameter block passed to it. If the directory bit
	is set, the file filter function returns false, indicating that the item
	should appear in the list; otherwise, the file filter function returns
	true to exclude the item from the list. Because a volume is identified via
	its root directory, volumes also appear in the list of items in the dialog
	box. 
*/
static pascal Boolean SFCustomFileFilter(CInfoPBPtr pb, void *myDataPtr)
{	/*list directories only*/
#pragma unused(myDataPtr)
	short	fileAttributes;

	fileAttributes = (pb->hFileInfo.ioFlAttrib >> kFolderBit) & 0x01;

	return (fileAttributes == 0);
} // SFCustomFileFilter


/*
---------------------------------------------------------------------------------
	The title of the Select button should identify which directory is
	available for selection.  The ShowFolderItem procedure is passed a handle
	to the button whose title is to be changed, the name of the directory
	available for selection, && the buttonÕs enclosing rectangle. The global
	variable gPrevSelectedName holds the full directory name, before
	truncation. 
*/
static void ShowFolderItem(DialogPtr theDialog, Str255 folderName)
{
	short	result;			/*result of TruncString*/
	short	width;			/*width available for name of directory*/
	short	stItemType;		/*needed for GetDialogItem*/
	Handle	stItemHdl;		/*needed for GetDialogItem*/
	Rect	stItemRect;		/*needed for GetDialogItem*/

	// find the custom static text in dialog
	GetDialogItem(theDialog, kDirNameST, &stItemType, &stItemHdl, &stItemRect);

	// Pascal string copy
	BlockMove(folderName, gPrevSelectedName, folderName[0]+1);

	// truncate string to fit into rect
	width = stItemRect.right - stItemRect.left - 2;
	result = TruncString(width, folderName, truncMiddle); /* Script Mgr call */

	// stuff folder name into display field
	SetDialogItemText((Handle)stItemHdl, folderName);

} // ShowFolderItem


static void ShowPrompt(DialogPtr theDialog)
{
	short	stItemType;		/*needed for GetDialogItem*/
	Handle	stItemHdl;		/*needed for GetDialogItem*/
	Rect	stItemRect;		/*needed for GetDialogItem*/

	// find the custom static text in dialog
	GetDialogItem(theDialog, kPromptST, &stItemType, &stItemHdl, &stItemRect);

	// stuff folder name into display field
	SetDialogItemText((Handle)stItemHdl, gThePrompt);

} // ShowPrompt



/*
---------------------------------------------------------------------------------
	The dialog hook function calls the ShowFolderItem procedure to copy the
	truncated title of the selected item into the Select button. This title
	eliminates possible user confusion about which directory is available for
	selection. If no item in the list is selected, the dialog hook function
	uses the name of the directory shown in the pop-up menu as the title of
	the Select button. A dialog hook function manages most of the process of
	letting the user select a director.  The MyDlgHook dialog hook function
	defined in Listing 3-18 calls the File Manager function PBGetCatInfo to
	retrieve the name of the directory to be selected. When the dialog hook
	function is first called (that is, when item is set to sfHookFirstCall),
	MyDlgHook determines the current volume && directory by calling the
	functions GetSFCurVol && GetSFCurDir. When MyDlgHook is called each
	subsequent time, MyDlgHook calls PBGetCatInfo with the volume reference
	number && directory ID of the previously opened directory. 
*/
static pascal short SF_DlgHook(short itemHit, DialogPtr theDialog, void * myDataPtr)

{
	short		returnItem;		/*what to return*/
	Str63		myName;
	CInfoPBRec	myPB;
	StandardFileReply	*mySFRPtr;
	OSErr		myErr;

	returnItem = itemHit;	/*default, except in special cases below*/

	if (GetWRefCon((WindowPtr)theDialog) == sfMainDialogRefCon)
	{	// this function is only for main dialog box

		if (itemHit == sfHookFirstCall)
		{
				/* Determine current folder name */
				myPB.hFileInfo.ioCompletion	= NULL;
				myPB.hFileInfo.ioNamePtr	= myName;
				myPB.hFileInfo.ioVRefNum	= SF_GetSFCurVol();
				myPB.hFileInfo.ioFDirIndex	= -1;
				myPB.hFileInfo.ioDirID		= SF_GetSFCurDir();
				myErr = PBGetCatInfoSync(&myPB);
				/* set title of Select button first time in */
				ShowFolderItem(theDialog, myName);
				ShowPrompt(theDialog);
		}
		else if (itemHit != sfHookLastCall)
		{
			/* Get mySFRPtr from 3rd parameter to hook function. */
			mySFRPtr = (StandardFileReply*)myDataPtr;
			// Track name of folder that can be selected.
			if ((mySFRPtr->sfIsFolder) || (mySFRPtr->sfIsVolume))
			{	// user selected an item in the SFList
				BlockMove(mySFRPtr->sfFile.name, myName, mySFRPtr->sfFile.name[0]+1);
			}
			else
			{
				// get name of parent folder if no folder/volume selected
				myPB.hFileInfo.ioCompletion	= NULL;
				myPB.hFileInfo.ioNamePtr	= myName;
				myPB.hFileInfo.ioVRefNum	= mySFRPtr->sfFile.vRefNum;
				myPB.hFileInfo.ioFDirIndex	= -1;
				myPB.dirInfo.ioDrDirID		= mySFRPtr->sfFile.parID;
				myErr = PBGetCatInfoSync(&myPB);
				mySFRPtr->sfFile.parID = myPB.dirInfo.ioDrParID;

				// fix up sfIsVolume/sfIsFolder depending on choice
				if (mySFRPtr->sfFile.parID == fsRtDirID)
					mySFRPtr->sfIsVolume = -1;	// in root directory
				else
					mySFRPtr->sfIsFolder = -1;	// down in directory or dir selected
			}

			// Change directory name in button title if changed.
			// Note the tricky Pascal string compare.
			if (strncmp((char*)myName, (char*)gPrevSelectedName, myName[0]+1) != 0)
			{
				ShowFolderItem(theDialog, myName);
			}

			switch (itemHit)
			{
				case kGetDirBTN:					/* force return by faking a cancel */
					returnItem = sfItemCancelButton;
					break;
				case sfItemCancelButton:
					gDirSelectionFlag = false;		/* flag no directory was selected */						
					break;
			}
		} // else not first call
	} // if main dialog box

	return returnItem;

} // SF_DlgHook



/*
---------------------------------------------------------------------------------
	The function PromptForFolder uses the file filter
	function and the dialog hook functions defined above to manage the
	directory selection dialog box. On exit, DoGetDirectory returns a standard
	file reply record describing the selected directory. The DoGetDirectory
	function initializes the two global variables gPrevSelectedName &&
	gDirSelectionFlag. As you have seen, these two variables are used by the
	custom dialog hook function. PromptForFolder  calls CustomGetFile to
	display the directory selection dialog box & handle user selections.
	When the user selects a directory or clicks the Cancel button, the dialog
	hook function returns sfItemCancelButton & CustomGetFile exits. At that
	point, the reply record contains information about the last item selected
	in the list of available items. 
*/

Boolean SF_PromptForFolder(Point sfPoint, const StringPtr aPrompt, FSSpec * spec)
{
	short				myNumTypes;
	SFTypeList			myTypes;		/*types of files to display*/	
	FileFilterYDUPP		myFileFilterUPP;
	DlgHookYDUPP		myDlgHookUPP;
	StandardFileReply	sfr;
	
#if qUsePowerPlant
	UNavServicesDialogs::LFileChooser chooser;
	
	NavDialogOptions *options = chooser.GetDialogOptions();
	if(options != nil)
	{
		LStr255::CopyPStr(aPrompt, options->windowTitle);
		options->location = sfPoint;
	}
	
	SInt32 outFolderDirID;
	return chooser.AskChooseFolder(*spec, outFolderDirID);
#endif

	gPrevSelectedName[0]	= '\0';		/*initialize name of previous selection*/
	gDirSelectionFlag		= true;		/*initialize directory selection flag*/
	myNumTypes				= -1;		/*allows invisible folders to show up*/

	myFileFilterUPP = NewFileFilterYDProc(SFCustomFileFilter);
	myDlgHookUPP = NewDlgHookYDProc(SF_DlgHook);

	if (aPrompt)
		BlockMove(aPrompt, gThePrompt, aPrompt[0]+1); // save the prompt
	else
		gThePrompt[0] = 0;

	CustomGetFile(	myFileFilterUPP,
					myNumTypes,
					myTypes,
					&sfr,
					rSF_FolderPromptDLOG,
					sfPoint,
					myDlgHookUPP,
					(ModalFilterYDUPP)NULL, 
					/*(ActivationOrderListPtr)*/NULL,
					(ActivateYDUPP)NULL,
					&sfr);

	DisposeRoutineDescriptor(myFileFilterUPP);
	DisposeRoutineDescriptor(myDlgHookUPP);

	/*Get the name of the directory.*/
	if (gDirSelectionFlag)
	{
		// Use saved name...
		BlockMove(gPrevSelectedName, sfr.sfFile.name, gPrevSelectedName[0]+1);
		if (sfr.sfIsVolume)
		{
			sfr.sfFile.parID = fsRtDirID; // sometimes it's not! :-)
		}
	}

	/* did they choose something? */
	sfr.sfGood = gDirSelectionFlag;

	gDirSelectionFlag = false;

	*spec = sfr.sfFile;
	return sfr.sfGood;

} // SF_PromptForFolder



// ---------------------------------------------------------------------------------
// If what you really wanted from SF_PromptForFolder was the folder's DirID,
// and not its parent, pass the FSSpec in here and you'll get the next dirID down.
long SF_GetFSSubDirID(FSSpecPtr theFSDir)
{
	OSErr		anError;
	CInfoPBRec	myPB;

	// get name of parent folder if no folder/volume selected
	myPB.hFileInfo.ioCompletion	= NULL;
	myPB.hFileInfo.ioNamePtr	= theFSDir->name;
	myPB.hFileInfo.ioVRefNum	= theFSDir->vRefNum;
	myPB.hFileInfo.ioFDirIndex	= 0; // use ioNamePtr (-1 = ignore)
	myPB.dirInfo.ioDrDirID		= theFSDir->parID;
	anError = PBGetCatInfoSync(&myPB);
	return myPB.dirInfo.ioDrDirID;
} // SF_GetFSSubDirID

