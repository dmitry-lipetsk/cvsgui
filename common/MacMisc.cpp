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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*
 * MacMisc.cpp --- misc. utilities for mac
 */

/* here should be a define, to recorgnize if this file is needed in dev-tools */
#ifdef TARGET_OS_MAC

#include "GUSIInternal.h"
#include "GUSIFileSpec.h"
#include "Persistent.h"
#include "AppConsole.h"

#include "MacMisc.h"
#include "VolsPaths.h"

Handle
GetDItemHdl(DialogPtr dlg, SInt16 Id)
{
	SInt16	type;
	Handle	item;
	Rect	box;
	
	GetDialogItem(dlg, Id, &type, &item, &box);
	return item;
}

ControlHandle
GetDItemCtrlHdl(DialogPtr dlg, SInt16 Id)
{
	SInt16	type;
	Handle	item;
	Rect	box;
	
	GetDialogItem(dlg, Id, &type, &item, &box);
	return (ControlHandle)item;
}

SInt16 GetDItemMenuSel(DialogPtr dlg, SInt16 Id)
{
	ControlHandle ctrl = GetDItemCtrlHdl(dlg, Id);
	return ctrl != 0L ? GetControlValue(ctrl) : 0;
}

void SetDItemMenuSel(DialogPtr dlg, SInt16 Id, SInt16 sel)
{
	ControlHandle ctrl = GetDItemCtrlHdl(dlg, Id);
	if(ctrl != 0L)
		SetControlValue(ctrl, sel);
}


OSErr
MacGetSpecialFolder(UFSSpec & theFolder, UStr & thePath, OSType inType,
	const char* inDesc, bool createFolder)
{
	OSErr err;
	err = ::FSFindFolder(kOnAppropriateDisk, inType,
					createFolder, &theFolder);
	if (err != noErr)
	{
		cvs_err("MacCvs: Unable to find the %s folder (error %d)\n", inDesc, err);
		return err;
	}

	GUSIFileSpec spec(theFolder);
	thePath = spec.FullPath();

	return err;
}

OSErr
MacGetTempFolder(UFSSpec & theFolder, UStr & thePath)
{
	static UFSSpec cacheFile = {0, 0, '\0'};
	static UStr cachePath;
	if(!cachePath.empty())
	{
		theFolder = cacheFile;
		thePath = cachePath;
		return noErr;
	}
	
	OSErr err = MacGetSpecialFolder (theFolder, thePath, kTemporaryFolderType, "temporary", true);
	if(err == noErr)
	{
		cacheFile = theFolder;
		cachePath = thePath;
	}
	
	return err;
}

OSErr
MacGetPrefsFolder(UFSSpec & theFolder, UStr & thePath)
{
	static UFSSpec cacheFile = {0, 0, '\0'};
	static UStr cachePath;
	if(!cachePath.empty())
	{
		theFolder = cacheFile;
		thePath = cachePath;
		return noErr;
	}
	
	OSErr err = MacGetSpecialFolder (theFolder, thePath, kPreferencesFolderType, "preferences");
	if(err == noErr)
	{
		cacheFile = theFolder;
		cachePath = thePath;
	}
	
	return err;
}

UStr CFString_to_UStr(CFStringRef inStringToConvert, CFStringEncoding inRequestedEncoding, bool *outConversionSuccessful)
{
  if ( outConversionSuccessful ) *outConversionSuccessful = true;
  if ( inStringToConvert == NULL ) return UStr();
  
  static char*    buf(NULL);
  static CFIndex  bufLen(0);
  CFIndex         stringLen(CFStringGetLength(inStringToConvert));
  CFIndex         maxLen(CFStringGetMaximumSizeForEncoding(stringLen, inRequestedEncoding)+1);
  
  if ( maxLen > bufLen ) {
    buf = static_cast<char*>(realloc(buf, maxLen));
    if ( buf == NULL ) {
      cvs_err("out of memory while trying to convert a string\n");
      return UStr();
    }
    bufLen = maxLen;
  }
  if ( !CFStringGetCString(inStringToConvert, buf, maxLen, inRequestedEncoding) ) {
    // try lossy conversion
    CFIndex   byteCount(0);
    CFStringGetBytes(inStringToConvert, CFRangeMake(0, stringLen), inRequestedEncoding, '?', false, (UInt8*)buf, maxLen, &byteCount);
    buf[byteCount] = 0;
    if ( outConversionSuccessful ) *outConversionSuccessful = false;
  }
  return UStr(buf);
}

UStr ConvertString(const char* inStringToConvert, CFStringEncoding inInputEncoding, CFStringEncoding inOutputEncoding, bool allowLossyConversion)
{
  if ( inStringToConvert == NULL  ||  *inStringToConvert == '\0' ) return UStr();
  
  CFStringRef     convStr(CFStringCreateWithCStringNoCopy(NULL, inStringToConvert, inInputEncoding, kCFAllocatorNull));
  if ( convStr == NULL ) return UStr(inStringToConvert);
  
  static char*    buf(NULL);
  static CFIndex  bufLen(0);
  CFIndex         convStrLen(CFStringGetLength(convStr));
  CFIndex         maxLen(CFStringGetMaximumSizeForEncoding(convStrLen, inOutputEncoding)+1);
  
  if ( maxLen > bufLen ) {
    buf = static_cast<char*>(realloc(buf, maxLen));
    if ( buf == NULL ) {
      cvs_err("out of memory while trying to convert a string\n");
      return UStr();
    }
    bufLen = maxLen;
  }
  if ( !CFStringGetCString(convStr, buf, maxLen, inOutputEncoding) ) {
    if ( allowLossyConversion ) {
      CFIndex   byteCount(0);
      CFStringGetBytes(convStr, CFRangeMake(0, convStrLen), inOutputEncoding, '?', false, (UInt8*)buf, maxLen, &byteCount);
      buf[byteCount] = 0;
    }
    else {
      CFRelease(convStr);
      cvs_err("couldn't convert '%s' to encoding %d\n", inStringToConvert, inOutputEncoding);
      return UStr(inStringToConvert);
    }
  }
  CFRelease(convStr);
  return UStr(buf);
}

UStr UTF8_to_SysEncoding(const char* utf8EncodedString, bool allowLossyConversion)
{
  return ConvertString(utf8EncodedString, kCFStringEncodingUTF8, CFStringGetSystemEncoding(), allowLossyConversion);
}

UStr SysEncoding_to_UTF8(const char* sysEncodedString, bool allowLossyConversion)
{
  return ConvertString(sysEncodedString, CFStringGetSystemEncoding(), kCFStringEncodingUTF8, allowLossyConversion);
}

UStr LString_to_UTF8(const LString& inString, bool allowLossyConversion)
{
  LStr255    str(inString);
  return SysEncoding_to_UTF8(LString::PToCStr(str), allowLossyConversion);
}

LStr255 UTF8_to_LString(const char* inString, bool allowLossyConversion)
{
  return LStr255(UTF8_to_SysEncoding(inString, allowLossyConversion));
}

void UTF8_to_PString_Copy(StringPtr outPString, const char* inUTF8String)
{
  CFStringRef     convStr(CFStringCreateWithCStringNoCopy(NULL, inUTF8String, kCFStringEncodingUTF8, kCFAllocatorNull));
  outPString[0] = 0;
  if ( convStr )
  {
    CFIndex   len;
    CFStringGetBytes(convStr, CFRangeMake(0, CFStringGetLength(convStr)), CFStringGetSystemEncoding(), '?', false, &outPString[1], 255, &len);
    outPString[0] = len;
  }
}


#endif /* TARGET_OS_MAC */
