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
 * MoveToTrash.cpp --- send the files to the trash
 * Author : Miro Jurisic <meeroh@MIT.EDU> --- December 1997
 * Modified : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

#include "stdafx.h"

#ifdef TARGET_OS_MAC
#	include "GUSIInternal.h"
#	include "GUSIFileSpec.h"

#	include <Files.h>
#	include <Folders.h>
#	include <AERegistry.h>
#	include <AEPackObject.h>
#	include <AEObjects.h>

#endif /* TARGET_OS_MAC */

#ifdef qUnix
#	include <stdio.h>
#endif /* qUnix */

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#include "MoveToTrash.h"
#include "ustr.h"
#include "FileTraversal.h"
#include "AppConsole.h"

#ifdef WIN32
#	include <direct.h>
#endif /* WIN32 */

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

using namespace std;

#ifdef TARGET_OS_MAC
# define kTryFinderMoveToTrash 0 // bug in Finder prevents correctly moving files to trash, but Finder doesn't tell us it fails...

# if kTryFinderMoveToTrash
	static OSErr	GetAddressOfFinder (
		AEAddressDesc*	outFinderAddress);
	static OSErr	SendFilesToTrash (
		AEAddressDesc const*	inFinderAddress,
		short					inFileCount,
		const UFSSpec*			inFileList);
	static OSErr	PutFilesInAE (
		short			inFileCount,
		const UFSSpec*	inFileList,
		AppleEvent	*	ioAppleEvent);

	const	OSType	kFinderSignature	=	'MACS';
# endif // kTryFinderMoveToTrash

	static OSErr ToolboxMoveToTrash(const UFSSpec* spec);

	static std::vector<UFSSpec> sToTrash;
	static bool sTrashRetainer = false;
	
	
	static OSErr	MoveToTrash (const std::vector<UFSSpec> & inFiles)
	{
		OSErr	err = noErr;
		
		/*	First unlock the file so that the Finder can rename it
			if there are already files of that name in the Trash */
		std::vector<UFSSpec>::const_iterator i;
		for(i = inFiles.begin(); i != inFiles.end(); ++i)
		{
			FSCatalogInfo info;
			if((err = FSGetCatalogInfo(&*i, kFSCatInfoNodeFlags, &info, NULL, NULL, NULL)) == noErr &&
				(info.nodeFlags & kFSNodeLockedMask) != 0)
			{
				info.nodeFlags = info.nodeFlags & ~kFSNodeLockedMask;
				err = FSSetCatalogInfo(&*i, kFSCatInfoNodeFlags, &info);
			}
			if (err != noErr)
				goto cleanup;
		}

# if kTryFinderMoveToTrash
		AEDesc	finderAddress = {typeNull, nil};
		err = GetAddressOfFinder (&finderAddress);
		if (err != noErr)
			goto cleanup;
		
		err = SendFilesToTrash (&finderAddress, inFiles.size(), &inFiles[0]);
		if (err != noErr)
			goto cleanup;
    			
	cleanup:
		if (finderAddress.dataHandle != nil) {
			AEDisposeDesc (&finderAddress);
		}
		if(err != noErr)
# else // !kTryFinderMoveToTrash
  cleanup:
# endif // !kTryFinderMoveToTrash
		{
			/* try the toolbox way */
			for(i = inFiles.begin(); i != inFiles.end(); ++i)
			{
				err = ToolboxMoveToTrash(&*i);
        if ( err ) 
        {
          GUSIFileSpec  f(*i);
          const char*   path = f.FullPath();
          cvs_err("couldn't move file to trash (error %d): %s\n", err, path ? path : "<unable to construct path>");
        }
			}
		}
		
		return err;
	}

	static OSErr	MoveToTrash (const UFSSpec* inFile)
	{
		OSErr	err = noErr;
		
		/*	First unlock the file so that the Finder can rename it
			if there are already files of that name in the Trash */
		
		FSCatalogInfo info;
		if((err = FSGetCatalogInfo(inFile, kFSCatInfoNodeFlags, &info, NULL, NULL, NULL)) == noErr &&
			(info.nodeFlags & kFSNodeLockedMask) != 0)
		{
			info.nodeFlags = info.nodeFlags & ~kFSNodeLockedMask;
			err = FSSetCatalogInfo(inFile, kFSCatInfoNodeFlags, &info);
		}
		if (err != noErr)
			goto cleanup;

# if kTryFinderMoveToTrash
		AEDesc	finderAddress = {typeNull, nil};
		err = GetAddressOfFinder (&finderAddress);
		if (err != noErr)
			goto cleanup;
		
		err = SendFilesToTrash (&finderAddress, 1, inFile);
		if (err != noErr)
			goto cleanup;
			
	cleanup:
		if (finderAddress.dataHandle != nil) {
			AEDisposeDesc (&finderAddress);
		}
		if(err != noErr)
# else // !kTryFinderMoveToTrash
  cleanup:
# endif // !kTryFinderMoveToTrash
		{
			/* try the toolbox way */
			err = ToolboxMoveToTrash(inFile);
      if ( err ) 
      {
        GUSIFileSpec  f(*inFile);
        const char*   path = f.FullPath();
        cvs_err("couldn't move file to trash (error %d): %s\n", err, path ? path : "<unable to construct path>");
      }
		}
		
		return err;
	}

	static OSErr
	ToolboxMoveToTrash(const UFSSpec* spec)
	{
		OSErr err;

		FSRef              folder, parentFolder;
		FSVolumeRefNum     volume(kOnAppropriateDisk);
    FSCatalogInfo      info;
    // get the volume the file resides on and use this as the base for finding the trash folder
    // since each volume will contain it's own trash folder...
    // We'll also need to touch the parent folder once we're done, since this is also what Finder does (and it makes sense...),
    // so we need to remember the parent folder the file is in.
    //		
		if ( FSGetCatalogInfo(spec, kFSCatInfoVolume, &info, NULL, NULL, &parentFolder) == noErr )
		  volume = info.volume;
		// go ahead and find the trash folder on that volume.
		// the trash folder for the current user may not yet exist on that volume,
		// so ask to automatically create it
		//
		err = FSFindFolder(volume, kTrashFolderType, kCreateFolder, &folder);

		if(err != noErr)
			return err;
		
	  err = FSMoveObject(spec, &folder, NULL);
	  if ( err == dupFNErr )
	  {
		  // try to rename the duplicate file in the trash

  		HFSUniStr255    name;
		
	    err = FSGetCatalogInfo(spec, kFSCatInfoNone, NULL, &name, NULL, NULL);
      if ( err == noErr )
      {
  		  UInt16          origLen(name.length);
	      if ( origLen > 245 ) origLen = 245;

  		  FSRef   duplicateFile;
  		  err = FSMakeFSRefUnicode(&folder, name.length, name.unicode, kTextEncodingUnknown, &duplicateFile);
  		  for ( int i(1);; ++i )
  		  {
  		    // attempt to create new unique name
  		    HFSUniStr255    newName(name);
  		    char            num[16];
  		    int             numLen;
  		    
  		    numLen = sprintf(num, "%d", i);
  		    newName.unicode[origLen] = ' ';
  		    for ( int j(0); j < numLen; ++j ) newName.unicode[origLen + j + 1] = num[j];
  		    newName.length = origLen + numLen + 1;
  		    
		      err = FSRenameUnicode(&duplicateFile, newName.length, newName.unicode, kTextEncodingUnknown, NULL);
		      if ( err != dupFNErr ) break;
		    }
		    
		    if ( err == noErr )
		      err = FSMoveObject(spec, &folder, NULL);
		    
		  }
		}
    
		// touch original parent folder's modification date once we've successfully moved away the file
		if ( err == noErr )
		{
		  FSCatalogInfo   catInfo;
		  
		  err = GetUTCDateTime(&catInfo.contentModDate, kUTCDefaultOptions);
		  if ( err == noErr )
		    err = FSSetCatalogInfo(&parentFolder, kFSCatInfoContentMod, &catInfo);
		  if ( err )
		  {
		    cvs_err("couldn't touch modification date of files parent folder: error %d\n", err);
		    err = noErr; // not fatal...
		  }
		}
		
		return err;
	}

	/* get address of the Finder (to use as the destination of our appleevents) */

# if kTryFinderMoveToTrash
	static OSErr
	GetAddressOfFinder (
		AEAddressDesc*	outFinderAddress)
	{
		return (AECreateDesc (typeApplSignature, (Ptr) &kFinderSignature, sizeof (long), outFinderAddress));
	}

	/* Tell Finder to send a list of files to the trash */

	static OSErr
	SendFilesToTrash (
		AEAddressDesc const*	inFinderAddress,
		short					inFileCount,
		const UFSSpec*			inFileList)
	{
		OSErr		err = noErr;
		AppleEvent	theAppleEvent = {typeNull, nil};
		AppleEvent	theReply = {typeNull, nil};
		DescType	trashType = kTrashFolderType;
		AEDesc		keyData = {typeNull, nil};
		AEDesc		trashSpecifier = {typeNull, nil};
		AEDesc		nullDescriptor = {typeNull, nil};
		
		
		/* create the appleevent */
		err = AECreateAppleEvent (kAECoreSuite, kAEMove, inFinderAddress, kAutoGenerateReturnID, kAnyTransactionID, &theAppleEvent);
		if (err != noErr)
			goto cleanup;
		
		/* put the files into the direct object of the appleevent */
		err = PutFilesInAE (inFileCount, inFileList, &theAppleEvent);
		if (err != noErr)
			goto cleanup;
		
		/* create a descriptor for trash */
		err = AECreateDesc (typeType, (Ptr) &trashType, sizeof (DescType) , &keyData);
		if (err != noErr)
			goto cleanup;
		
		/* get specifier for trash object */
		err = CreateObjSpecifier (typeProperty, &nullDescriptor, formPropertyID, &keyData, true, &trashSpecifier);
		if (err != noErr)
			goto cleanup;
		
		/* put the trash specifier as the indirect object in the appleevent */
		err = AEPutParamDesc (&theAppleEvent, keyAEInsertHere, &trashSpecifier);
		if (err != noErr)
			goto cleanup;
		
		/* send the appleevent */
		err = AESend (&theAppleEvent, &theReply, kAEWaitReply, kAENormalPriority, kAEDefaultTimeout, nil, nil);
		if (err != noErr)
			goto cleanup;
			
		/* ignore the reply */	

	cleanup:

		/* dispose of the stuff that we allocated */
		if (theAppleEvent.dataHandle != nil) {
			AEDisposeDesc (&theAppleEvent);
		}
		if (theReply.dataHandle != nil) {
			AEDisposeDesc (&theReply);
		}
		if (keyData.dataHandle != nil) {
			AEDisposeDesc (&keyData);
		}
		if (trashSpecifier.dataHandle != nil) {
			AEDisposeDesc (&trashSpecifier);
		}
		/* don't have to dispose of nullDescriptor because it's null */
		
		return err;
	}

	/* put a list of files into the direct object of an apple event */

	static OSErr
	PutFilesInAE (
		short				inFileCount,
		const UFSSpec*		inFileList,
		AppleEvent*			ioAppleEvent)
	{
		OSErr		err;
		AEDescList	fileList = {typeNull, nil};
		short		i;
		AliasHandle alias;

		err = AECreateList(nil, 0, false, &fileList);
		if (err != noErr) return err;
		
		for (i = 1; i <= inFileCount; i++) {
			err = ::FSNewAlias(NULL, inFileList + i - 1, &alias);
			if (err != noErr) goto cleanup;
			
			::HLock((Handle)alias);
			err = AEPutPtr(&fileList, i, typeAlias, *alias, ::GetHandleSize((Handle)alias));
			::HUnlock((Handle)alias);
			
			::DisposeHandle((Handle)alias);
			if (err != noErr) goto cleanup;
		}
		
		err = AEPutParamDesc(ioAppleEvent, keyDirectObject, &fileList);
		
	cleanup:
		if (fileList.dataHandle != nil) {
			AEDisposeDesc(&fileList);
		}
		
		return err;
	}
# endif // kTryFinderMoveToTrash
#endif /* TARGET_OS_MAC */

void CompatBeginMoveToTrash(void)
{
#ifdef TARGET_OS_MAC
	sToTrash.erase(sToTrash.begin(), sToTrash.end());
	sTrashRetainer = true;
#endif
}

void CompatEndMoveToTrash(void)
{
#ifdef TARGET_OS_MAC
	if(sToTrash.size() > 0)
	{
		OSErr err;
		err = MoveToTrash(sToTrash);
		if(err != noErr)
			cvs_err("Unable to send files to the trash (Error %d)\n", err);
		else
			cvs_out("Files has been moved successfully to the trash...\n");
	}

	sToTrash.erase(sToTrash.begin(), sToTrash.end());
	sTrashRetainer = false;
#endif
}

bool CompatMoveToTrash(const char *file, const char *dir, const UFSSpec *spec)
{
#ifdef TARGET_OS_MAC
	if(sTrashRetainer)
	{
		if(spec)
			sToTrash.push_back(*spec);
		else
		{
			CStr fullpath(dir);
			if(!fullpath.empty() && !fullpath.endsWith(kPathDelimiter))
			{
				fullpath << kPathDelimiter;
			}
			fullpath << file;
			GUSIFileSpec mspec(fullpath);
			if(mspec.Error() != noErr)
			{
				errno = mspec.Error();
				return false;
			}
			sToTrash.push_back(mspec);
		}
		
		return true;
	}
	
	return spec != NULL ? MoveToTrash(spec) == noErr : false;
#elif defined(WIN32)
	SHFILEOPSTRUCT fileop;

	string fullpath(dir ? dir : "");
	NormalizeFolderPath(fullpath);
	fullpath += file;

	// Really need that? Not sure...
	TrimRight(fullpath);

	// Need that for ::SHFileOperation: It expects a string with _two_
	// null terminators in the .pFrom member.
	// This makes the length() member of fullpath invalid, but guarantees
	// proper functioning of the SHFileOperation call.
	fullpath += ' ';
	*fullpath.rbegin() = 0;

    fileop.hwnd     = *AfxGetMainWnd(); 
    fileop.wFunc    = FO_DELETE; 
    fileop.pFrom    = fullpath.c_str(); 
    fileop.pTo      = 0L; 
    fileop.fFlags   = FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;

	if(SHFileOperation(&fileop) != 0)
	{
		if(dir != 0L && chdir(dir) != 0)
			return false;

		cvs_err("Unable to send \'%s\' to the recycle bin (error %d)\n",
			file, GetLastError());
		cvs_err("Trying now to delete \'%s\' instead\n", file);
		return remove(file) == 0;
	}
	return true;
#else /* !TARGET_OS_MAC && !WIN32 */
	std::string fullpath(dir);
	NormalizeFolderPath(fullpath);
	fullpath += file;

	return remove(fullpath.c_str()) == 0;
#endif /* !TARGET_OS_MAC && !WIN32 */
}
