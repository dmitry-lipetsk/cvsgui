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

#ifndef MACMISC_H
#define MACMISC_H

#include "TextBinary.h"
#include "ustr.h"
#include <Dialogs.h>

Handle GetDItemHdl(DialogPtr dlg, SInt16 Id);
ControlHandle GetDItemCtrlHdl(DialogPtr dlg, SInt16 Id);
SInt16 GetDItemMenuSel(DialogPtr dlg, SInt16 Id);
void SetDItemMenuSel(DialogPtr dlg, SInt16 Id, SInt16 sel);

OSErr MacGetSpecialFolder(UFSSpec & theFolder, UStr & thePath, OSType inType,
	const char* inDesc, bool createFolder = false);
OSErr MacGetTempFolder(UFSSpec & theFolder, UStr & thePath);
OSErr MacGetPrefsFolder(UFSSpec & theFolder, UStr & thePath);

UStr ConvertString(const char* inStringToConvert, CFStringEncoding inInputEncoding, CFStringEncoding inOutputEncoding, bool allowLossyConversion=false);
UStr SysEncoding_to_UTF8(const char* sysEncodedString, bool allowLossyConversion=false);
UStr UTF8_to_SysEncoding(const char* utf8EncodedString, bool allowLossyConversion=false);
UStr LString_to_UTF8(const LString& inString, bool allowLossyConversion=false);
LStr255 UTF8_to_LString(const char* inString, bool allowLossyConversion=false);
UStr CFString_to_UStr(CFStringRef inStringToConvert, CFStringEncoding inRequestedEncoding, bool *outConversionSuccessful=NULL);
void UTF8_to_PString_Copy(StringPtr outPString, const char* inUTF8String);

#endif /* MACMISC_H */
