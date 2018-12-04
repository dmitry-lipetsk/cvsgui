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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 1998
 */

/*
 * Persistent.cpp --- utilities to store persitent values
 */

#ifdef TARGET_OS_MAC
#	include "GUSIInternal.h"
#	include "GUSIFileSpec.h"
#endif

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>

#if defined(HAVE_ERRNO_H) || defined(WIN32)
#	include <errno.h>
#endif

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#ifndef WIN32
#	include <ctype.h>
#endif /* !WIN32 */

#ifdef WIN32
#	include "wincvs.h"
#endif

#include "Persistent.h"
#include "AppConsole.h"
#include "uconsole.h"
#include "uwidget.h"
#include "FileTraversal.h"
#include "Authen.h"
#include "CvsPrefs.h"
#include "getline.h"
#include "CvsAlert.h"
#include "CvsCommands.h"
#include "SaveSettingsDlg.h"

#ifdef TARGET_OS_MAC
#	include <Carbon.h>
#	include "VolsPaths.h"
#	include "MacMisc.h"
#	ifndef Assert_ // for mpw
#		include <assert.h>
#		define Assert_(cond) assert(cond)
#	endif
# include "MultiString.h"
#endif /* TARGET_OS_MAC */

#if qMacCvsPP
#	include "MacCvsApp.h"
#	include "MacCvsConstant.h"
#endif

#if TARGET_RT_MAC_MACHO
#	include <LCFString.h>
#endif

#if qUnix
#	include "UCvsDialogs.h"
#endif

using namespace std;

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

namespace {
	inline unsigned char hexnibble(char ch)
	{
		if ( ch >= '0'  &&  ch <= '9' ) return ch - '0';
		if ( ch >= 'A'  &&  ch <= 'F' ) return ch - 'A' + 10;
		if ( ch >= 'a'  &&  ch <= 'f' ) return ch - 'a' + 10;
		return 0;
	}
}

static bool LoadSettingsInternal(const char* fullname, 
								 bool (*func)(const char* token, const char* value, void* data), void* data);

vector<CPersistent::CPersistentEntry>* CPersistent::m_fAllEntries = 0L;

UIMPLEMENT_DYNAMIC(CPersistent, UObject)
UIMPLEMENT_DYNAMIC(CPersistentInt, CPersistent)
UIMPLEMENT_DYNAMIC(CPersistentBool, CPersistent)
UIMPLEMENT_DYNAMIC(CPersistentString, CPersistent)

CPersistent::CPersistent(const char* uniqueName, kClassPersistent pclass) : m_fTimeStamp(0)
{
	Register(this, uniqueName, pclass);
	TouchTimeStamp();
}

CPersistent::~CPersistent()
{
	UnRegister(this);
}

/*!
	Register a value to store/load
	\param value Value
	\param uniqueName Name of the value
	\param pclass Class of the value
*/
void CPersistent::Register(CPersistent* value, const char* uniqueName, kClassPersistent pclass)
{
	vector<CPersistentEntry>::const_iterator i;
	if( m_fAllEntries == 0L )
		m_fAllEntries = new vector<CPersistentEntry>;

	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		if(strcmp(uniqueName, (*i).m_fUniqueName) == 0)
		{
			UAppConsole("Error : duplicate persistent %s !\n", uniqueName);
		}
	}

	CPersistentEntry newentry;
	newentry.m_fClass = pclass;
	newentry.m_fUniqueName = uniqueName;
	newentry.m_fValue = value;
	
	m_fAllEntries->push_back(newentry);
}

/*!
	Unregister a value to store/load
	\param value Value to unregister
*/
void CPersistent::UnRegister(CPersistent* value)
{
	vector<CPersistentEntry>::iterator i;
	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		if( value == (*i).m_fValue )
			break;
	}

	if( i == m_fAllEntries->end() )
	{
		UAppConsole("Error : Unknown persistent value !\n");
		return;
	}

	m_fAllEntries->erase(i);
	if( m_fAllEntries->size() == 0 )
	{
		delete m_fAllEntries;
		m_fAllEntries = 0L;
	}
}

/*!
	Simple diff comparing two files
	\param file1 First file
	\param file2 Second file
	\return 0 if files are the same, -1 otherwise
*/
static int simple_diff(const char* file1, const char* file2)
{
	FILE* f1 = fopen(file1, "rb");
	if(f1 == 0L)
		return -1;

	FILE* f2 = fopen(file2, "rb");
	if( f2 == 0L )
	{
		fclose(f1);
		return 1;
	}

	int res = -1;
	char buf1[8000];
	char buf2[8000];
	size_t num1, num2;

	while( !feof(f1) && !feof(f2) && !ferror(f1) && !ferror(f2) )
	{
		num1 = fread(buf1, sizeof(char), 8000, f1);
		num2 = fread(buf2, sizeof(char), 8000, f2);
		if( num1 != num2 || (res = memcmp(buf1, buf2, num1 * sizeof(char))) != 0 )
			goto fail;
	}

	if( ferror(f1) || ferror(f2) )
		goto fail;

	if( feof(f1) ^ feof(f2) )
		goto fail;

	res = 0;

fail:
	fclose(f1);
	fclose(f2);

	return res;
}


bool CPersistent::LoadAllSettings(const char* settingFileName)
{
	return LoadSettingsInternal(settingFileName, LoadPersistentSettings, 0L);
}


/*!
	Save all persistent values with the class kAddSettings
	\param settingFileName Setting file name
	\param filePath Settings file path
	\return true if the values were actually saved, false otherwise
*/
bool CPersistent::SaveAllSettings(const char* settingFileName, const char* filePath)
{
	string tmpSettingsFile(settingFileName);
	tmpSettingsFile += "__";

	FILE* fd = fopen(tmpSettingsFile.c_str(), "w");
	if( fd == 0L )
	{
		cvs_err("Failed to open the setting file '%s' for writing (error %d)", tmpSettingsFile.c_str(), errno);
		return false;
	}

	fprintf(fd, "VERSION=%d\n", 1);

	string path(filePath);
	TrimRight(path, kPathDelimiter);

	fprintf(fd, "PATH=%s\n", path.c_str());
#if INTERNAL_AUTHEN
	fprintf(fd, "AUTHENTICATION=%s\n", (const char*)Authen::kindToToken(gAuthen.kind()));
	fprintf(fd, "CVSROOT=%s\n", (const char*)gCvsPrefs);
#endif /* INTERNAL_AUTHEN */

	for(vector<CPersistentEntry>::const_iterator i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		const CPersistentEntry& entry = *i;
		if( (entry.m_fClass & kAddSettings) == 0 )
			continue;

		if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentBool)) )
		{
			CPersistentBool* pbool = (CPersistentBool*)entry.m_fValue;
			bool state = *pbool;
			fprintf(fd, "%s=%s\n", entry.m_fUniqueName, state ? "true" : "false");
		}
		else if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentInt)) )
		{
			CPersistentInt* pint = (CPersistentInt*)entry.m_fValue;
			int val = *pint;
			fprintf(fd, "%s=%d\n", entry.m_fUniqueName, val);
		}
		else if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentString)) )
		{
			CPersistentString* pstr = (CPersistentString*)entry.m_fValue;
			const char* str = pstr->c_str();
			fprintf(fd, "%s=%s\n", entry.m_fUniqueName, (const char*)(str == 0L ? "" : str));
		}
		else
		{
			// write out data as hex string
			int dataSize(entry.m_fValue->SizeOf());
			char* hexstring = (char*)malloc(2*dataSize + 1);
			if( hexstring == NULL ) 
			{
				cvs_err("couldn't write persistent value '%s'\n", entry.m_fUniqueName);
			}
			else 
			{
				const unsigned char* p = (const unsigned char*)entry.m_fValue->GetData();
				char* s = hexstring;
				static const char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
				
				// length first
				for( int i = 0; i < dataSize; ++i )
				{
					*s++ = hexDigits[(*p >> 4) & 0x0f];
					*s++ = hexDigits[(*p) & 0x0f];
					++p;
				}
				
				*s = '\0';
				fprintf(fd, "%s=%s\n", entry.m_fUniqueName, hexstring);
				free(hexstring);
			}
		}
	}

	fclose(fd);
	
	if( simple_diff(tmpSettingsFile.c_str(), settingFileName) == 0 )
	{
		if( unlink(tmpSettingsFile.c_str()) != 0 && errno != ENOENT )
			cvs_err("Could not remove file '%s' (error %d)\n", tmpSettingsFile.c_str(), errno);
	
		return false;
	}

	if( unlink(settingFileName) != 0 && errno != ENOENT )
	{
		cvs_err("Could not remove file '%s' (error %d)\n", (const char*)settingFileName, errno);
		return false;
	}
	
	if( rename(tmpSettingsFile.c_str(), settingFileName) != 0 )
	{
		cvs_err("Could not rename file '%s' (error %d)\n", tmpSettingsFile.c_str(), errno);
		return false;
	}
	
	return true;
}

#ifdef TARGET_OS_MAC
static OSErr GetPreferencesFile(FSRef & mMacFileRef)
{
	OSErr	theErr;
	FSRef ref;
	theErr = ::FSFindFolder(kOnAppropriateDisk, kPreferencesFolderType,
					kDontCreateFolder, &ref);
	
	if( theErr == noErr )
	{
		LCFString str(kCVS_prefName);
		UniChar buffer[PATH_MAX];
		CFRange range = {0, str.GetLength()};
		::CFStringGetCharacters(str, range, buffer);
		
		theErr = ::FSMakeFSRefUnicode(&ref, str.GetLength(), buffer, kTextEncodingDefaultFormat, &mMacFileRef);
	}

	return theErr;
}
#endif // TARGET_OS_MAC

/*!
	Save the registered values
*/
void CPersistent::SaveAll(void)
{
#ifdef TARGET_OS_MAC
	vector<CPersistentEntry>::iterator i;
	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		CPersistentEntry & entry = *i;
		int size = entry.m_fValue->SizeOf();
		if( size <= 0 )
			continue;
		
		CFPropertyListRef   value(NULL);
		CFStringRef         key(CFStringCreateWithCString(NULL, entry.m_fUniqueName, kCFStringEncodingASCII));
		
		if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentBool)) )
		{
		  value = *static_cast<CPersistentBool*>(entry.m_fValue) ? kCFBooleanTrue : kCFBooleanFalse;
		  CFRetain(value);
		}
		else if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentInt)) )
		{
			CPersistentInt* pint = (CPersistentInt*)entry.m_fValue;
			SInt32 val = *pint;
			value = CFNumberCreate(NULL, kCFNumberSInt32Type, &val);
		}
		else if( dynamic_cast<TMString<CStr>*>(entry.m_fValue) != NULL )
		{
		  CFMutableArrayRef    valueArray(CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks));
		  if ( valueArray != NULL ) {
		    TMString<CStr>* ptmstr = static_cast<TMString<CStr>*>(entry.m_fValue);
		    for ( TMString<CStr>::list_t::const_iterator i(ptmstr->GetList().begin()); i != ptmstr->GetList().end(); ++i ) {
		      CFStringRef   str(CFStringCreateWithCString(NULL, i->empty() ? "" : static_cast<const char*>(*i), kCFStringEncodingUTF8));
		      if ( str ) {
		        CFArrayAppendValue(valueArray, str);
		        CFRelease(str);
		      } else cvs_err("couldn't create string entry for multi-string array\n");
		    }
		  }
		  value = valueArray;
		}
		else if( dynamic_cast<TMString<CPStr>*>(entry.m_fValue) != NULL )
		{
		  CFMutableArrayRef    valueArray(CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks));
		  if ( valueArray != NULL ) {
		    TMString<CPStr>* ptmstr = static_cast<TMString<CPStr>*>(entry.m_fValue);
		    for ( TMString<CPStr>::list_t::const_iterator i(ptmstr->GetList().begin()); i != ptmstr->GetList().end(); ++i ) {
		      CFStringRef   str(CFStringCreateWithCString(NULL, i->empty() ? "" : static_cast<const char*>(*i), kCFStringEncodingUTF8));
		      if ( str ) {
		        CFArrayAppendValue(valueArray, str);
		        CFRelease(str);
		      } else cvs_err("couldn't create string entry for multi-string array\n");
		    }
		  }
		  value = valueArray;
		}
		else if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentString)) )
		{
			CPersistentString* pstr = (CPersistentString*)entry.m_fValue;
			value = CFStringCreateWithCString(NULL, *pstr, kCFStringEncodingUTF8);
		}
		else if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(PCPStr)) )
		{
			PCPStr* pstr = (PCPStr*)entry.m_fValue;
			value = CFStringCreateWithCString(NULL, *pstr, kCFStringEncodingUTF8);
		}
		else
		{
			value = CFDataCreate(NULL, (const UInt8*)entry.m_fValue->GetData(), entry.m_fValue->SizeOf());
		}
		
    if ( value ) {
      CFPreferencesSetAppValue(key, value, kCFPreferencesCurrentApplication);
    } else {
      cvs_err("Couldn't create preference value for %s.\n", entry.m_fUniqueName);
    }
    if ( value ) CFRelease(value);
    if ( key ) CFRelease(key);
	}
	
	UInt32        bundleVersion(CFBundleGetVersionNumber(CFBundleGetMainBundle()));
	CFNumberRef   bundleVersionPref(CFNumberCreate(NULL, kCFNumberSInt32Type, &bundleVersion));
	if ( bundleVersionPref ) {
  	CFPreferencesSetAppValue(CFSTR("MacCvs:PrefsVersion"), bundleVersionPref, kCFPreferencesCurrentApplication);
  	CFRelease(bundleVersionPref);
  } else {
    cvs_err("Couldn't write prefs version.\n");
  }
  if ( !CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication) ) {
    cvs_err("Couldn't write preferences.\n");
  }	
#endif /* TARGET_OS_MAC */
#ifdef WIN32
	vector<CPersistentEntry>::iterator i;
	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		CPersistentEntry& entry = *i;
		unsigned int size = entry.m_fValue->SizeOf();
		if( size == 0 )
			continue;
		
		if( !AfxGetApp()->WriteProfileBinary(PROFILE_NAME, entry.m_fUniqueName, (BYTE*)entry.m_fValue->GetData(), size) )
		{
			cvs_err("Error while saving preferences !\n");
			break;
		}
	}
#endif /* WIN32 */
#ifdef qUnix
	std::string home;
	home = gCvsPrefs.Home();
	NormalizeFileName(home);

	home += ".gcvsrc";
	FILE* out = fopen(home.c_str(), "wb");
	if( out == 0L )
	{
		cvs_err("Unable to open '%s' for writing (error %d)", (const char*)home.c_str(), errno);
		return;
	}

	int version = 1;
	fwrite(&version, sizeof(int), 1, out);
	int numEntries = m_fAllEntries->size();
	fwrite(&numEntries, sizeof(int), 1, out);
	vector<CPersistentEntry>::iterator i;
	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		CPersistentEntry& entry = *i;
		unsigned int size = entry.m_fValue->SizeOf();

		fwrite(entry.m_fUniqueName, sizeof(char), strlen(entry.m_fUniqueName) + 1, out);
		fwrite(&size, sizeof(unsigned int), 1, out);
		fwrite(entry.m_fValue->GetData(), 1, size, out);
	}

	if( ferror(out) )
		cvs_err("Error while writing '%s' (error %d)", (const char*)home.c_str(), errno);

	fclose(out);
#endif /* qUnix */

	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		CPersistentEntry& entry = *i;
		entry.m_fValue->ResetTimeStamp();
	}
}

/*!
	Load the registered values
	\param only	Force load only this persistent value
	\return true on success, false otherwise
*/
bool CPersistent::LoadAll(const char* only)
{
	bool found_one = false;
#ifndef qUnix
	vector<CPersistentEntry>::iterator i;
#endif
#ifdef TARGET_OS_MAC
  Boolean   keyExists(false);
  UInt32    prefsVersion;
  if ( CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication) )
    prefsVersion = CFPreferencesGetAppIntegerValue(CFSTR("MacCvs:PrefsVersion"), kCFPreferencesCurrentApplication, &keyExists);
  if ( keyExists ) {
    
  	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
  	{
  		CPersistentEntry& entry = *i;
  		CFStringRef       key(CFStringCreateWithCString(NULL, entry.m_fUniqueName, kCFStringEncodingASCII));
  		if ( key ) {
    		keyExists = false;
    		
    		if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentBool)) )
    		{
    		  Boolean   value(CFPreferencesGetAppBooleanValue(key, kCFPreferencesCurrentApplication, &keyExists));
    		  if ( keyExists ) *static_cast<CPersistentBool*>(entry.m_fValue) = (value == true);
    		}
    		else if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentInt)) )
    		{
    		  SInt32 value(CFPreferencesGetAppIntegerValue(key, kCFPreferencesCurrentApplication, &keyExists));
    		  if ( keyExists ) *static_cast<CPersistentInt*>(entry.m_fValue) = value;
    		}
    		else if( dynamic_cast<TMString<CStr>*>(entry.m_fValue) != NULL )
    		{
    		  CFPropertyListRef    value(CFPreferencesCopyAppValue(key, kCFPreferencesCurrentApplication));
    		  if ( value ) {
    		    if ( CFGetTypeID(value) == CFArrayGetTypeID() ) {
    		      static_cast<TMString<CStr>*>(entry.m_fValue)->RemoveAll();
      		    CFArrayRef    valueArray = static_cast<CFArrayRef>(value);
      		    CFIndex       itemCount(CFArrayGetCount(valueArray));
      		    // MultiStrings only know how to insert at the beginning, so let's iterate backwards
      		    for ( CFIndex i(itemCount); i > 0; --i ) {
      		      CFTypeRef   stringValue(CFArrayGetValueAtIndex(valueArray, i-1));
      		      if ( stringValue  &&  CFGetTypeID(stringValue) == CFStringGetTypeID() ) {
      		        static_cast<TMString<CStr>*>(entry.m_fValue)->Insert(CFString_to_UStr(static_cast<CFStringRef>(stringValue), kCFStringEncodingUTF8));
      		      }
      		    }
      		    keyExists = true;
      		  }
    		    CFRelease(value);
    		  }
    		}
    		else if( dynamic_cast<TMString<CPStr>*>(entry.m_fValue) != NULL )
    		{
    		  CFPropertyListRef    value(CFPreferencesCopyAppValue(key, kCFPreferencesCurrentApplication));
    		  if ( value ) {
    		    if ( CFGetTypeID(value) == CFArrayGetTypeID() ) {
    		      static_cast<TMString<CPStr>*>(entry.m_fValue)->RemoveAll();
      		    CFArrayRef    valueArray = static_cast<CFArrayRef>(value);
      		    CFIndex       itemCount(CFArrayGetCount(valueArray));
      		    // MultiStrings only know how to insert at the beginning, so let's iterate backwards
      		    for ( CFIndex i(itemCount); i > 0 ; --i ) {
      		      CFTypeRef   stringValue(CFArrayGetValueAtIndex(valueArray, i-1));
      		      if ( stringValue  &&  CFGetTypeID(stringValue) == CFStringGetTypeID() ) {
      		        static_cast<TMString<CPStr>*>(entry.m_fValue)->Insert(CFString_to_UStr(static_cast<CFStringRef>(stringValue), kCFStringEncodingUTF8));
      		      }
      		    }
      		    keyExists = true;
      		  }
    		    CFRelease(value);
    		  }
    		}
    		else if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentString)) )
    		{
    		  CFPropertyListRef    value(CFPreferencesCopyAppValue(key, kCFPreferencesCurrentApplication));
    		  if ( value  &&  CFGetTypeID(value) == CFStringGetTypeID() ) {
    		    *(CPersistentString*)entry.m_fValue = CFString_to_UStr(static_cast<CFStringRef>(value), kCFStringEncodingUTF8);
    		    keyExists = true;
    		    CFRelease(value);
    		  }
    		}
    		else if( entry.m_fValue->IsKindOf(URUNTIME_CLASS(PCPStr)) )
    		{
    		  CFPropertyListRef    value(CFPreferencesCopyAppValue(key, kCFPreferencesCurrentApplication));
    		  if ( value  &&  CFGetTypeID(value) == CFStringGetTypeID() ) {
    		    *(PCPStr*)entry.m_fValue = CFString_to_UStr(static_cast<CFStringRef>(value), kCFStringEncodingUTF8);
    		    keyExists = true;
    		    CFRelease(value);
    		  }
    		}
    		else
    		{
    		  CFPropertyListRef    value(CFPreferencesCopyAppValue(key, kCFPreferencesCurrentApplication));
    		  if ( value  &&  CFGetTypeID(value) == CFDataGetTypeID() ) {
    		    entry.m_fValue->SetData(CFDataGetBytePtr(static_cast<CFDataRef>(value)), CFDataGetLength(static_cast<CFDataRef>(value)));
    		    keyExists = true;
    		    CFRelease(value);
    		  }
    		}

    		if ( keyExists ) entry.m_fValue->ResetTimeStamp();       
      }
      else cvs_err("Couldn't create prefs key for '%s'\n", static_cast<const char*>(entry.m_fUniqueName));      
    }
  } else {
    // no CFPreferences based prefs yet -> attempt to read resource based settings
  	FSRef thePrefFile;
  	short FRef, oldRef;
  	OSErr err;
  	
  	err = GetPreferencesFile(thePrefFile);
  	
  	if( err != noErr )
  		return false;

  	oldRef = CurResFile();
  	FRef = FSOpenResFile(&thePrefFile, fsRdPerm);
  	if( FRef < 0 )
  		return false;

  	CPStr name;
  	UseResFile(FRef);
  	
  	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
  	{
  		CPersistentEntry& entry = *i;
  		name = entry.m_fUniqueName;

  		Handle hdl = Get1NamedResource(kCVS_prefID, name);
  		if( hdl == 0L || (only && strcmp(only, name) != 0) )
  			continue;
  		
  		HLock(hdl);
  		found_one = true;
  		entry.m_fValue->SetData(*hdl, GetHandleSize(hdl));
  		entry.m_fValue->ResetTimeStamp();
  		HUnlock(hdl);
  	}

  	// migrate to the new gOldCvsroots container
  	if( gOldCvsroots.GetList().size() == 0 && only == 0L )
  	{
  		for(int nIndex = 0; nIndex <= NUM_CVSROOT; nIndex++)
  		{
  			char tmp[20];
  			sprintf(tmp, "P_Cvsroot%d", nIndex);

  			CPStr pstr(tmp);
  			Handle hdl = Get1NamedResource(kCVS_prefID, pstr);
  			if( hdl == 0L )
  				continue;

  			string  root((char*)*hdl);
  			if( !root.empty() )
  				gOldCvsroots.Insert(root);
  		}
  	}

  	CloseResFile(FRef);
  	UseResFile(oldRef);
  	
  	// and rename old prefs file
		LCFString str(kCVS_prefName " (old)");
		UniChar buffer[256];
		CFRange range = {0, str.GetLength()};
		::CFStringGetCharacters(str, range, buffer);
  	FSRenameUnicode(&thePrefFile, str.GetLength(), buffer, kTextEncodingUnknown, NULL);
  }
#endif /* TARGET_OS_MAC */
#ifdef WIN32
	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		CPersistentEntry & entry = *i;
		BYTE* ppData;
		UINT pBytes;

		if( only && strcmp(only, entry.m_fUniqueName) != 0 )
			continue;

		if( !AfxGetApp()->GetProfileBinary(PROFILE_NAME, entry.m_fUniqueName, &ppData, &pBytes))
		{
			continue;
		}

		found_one = true;
		entry.m_fValue->SetData(ppData, pBytes);
		entry.m_fValue->ResetTimeStamp();

		delete ppData;
	}
#endif /* WIN32 */
#ifdef qUnix
	bool error     = false;

	string  home;
	string  cvsroot;
	
	
	home = gCvsPrefs.Home();
	NormalizeFolderPath(home);
	
	home += ".gcvsrc";

	FILE* out = fopen(home.c_str(), "rb");
	if( out == 0L )
		return false;

	int version;
	int numEntries;
	CStaticAllocT<char> alloc;
	fread(&version, sizeof(int), 1, out);
	if( version != 1 )
	{
		cvs_err("Unexpected version %d in the file '%s'\n", version, (const char*)home.c_str());
		error = true;
	} else {
		fread(&numEntries, sizeof(int), 1, out);
		if( numEntries < 0 )
		{
			cvs_err("Unexpected # %d in the file '%s'\n", numEntries, (const char*)home.c_str());
			error = true;
		}
	}
	
	/* something has gone wrong !!!! */
	if( error ) {
		fclose(out);
		return false;
	}
	
	while( !feof(out) && !ferror(out) )
	{
		string  name;
		while( !feof(out) && !ferror(out) )
		{
			int c = getc(out);
			if( EOF == c)
				break;
			name += (char)c;
			if( c == '\0' )
				break;
		}

		if( feof(out) && name.length())
			cvs_err("Unexpected end of file in '%s'\n", (const char*)home.c_str());
		
		if( ferror(out) )
			cvs_err("Unexpected error %d in '%s'\n", errno, (const char*)home.c_str());
				
		if( feof(out) || ferror(out) ) {
			break;
		}

		unsigned int size = 0;
		fread(&size, sizeof(unsigned int), 1, out);
		
		if ( size > 1024 || size < 0) {
			cvs_err("%s is bogus (size: %d - %s)\n", (const char*)home.c_str(), size, name.c_str());
			break;
		} 
		
		alloc.AdjustSize(size);
		if ( size != fread((char*)alloc, 1, size, out)) {
			cvs_err("%s is bogus (fread - %s)\n", (const char*)home.c_str(), name.c_str());
			break;
		} 

		if( only && strcmp(only, name.c_str()) != 0 )
			continue;
		
		CPersistentEntry* entry = Find( name.c_str());
		if ( NULL == entry) {
			if( name.compare("P_Cvsroot") != 0) {
				cvsroot = (char*)alloc;
				if( !cvsroot.empty() ) {
					gOldCvsroots.Insert(cvsroot.c_str());
				} else {
					gOldCvsroots.Insert(name.c_str());
				}
				
				continue;
			}

			cvs_err("dropping value for persitent '%s' from %s (not needed any longer)\n", name.c_str(), (const char*)home.c_str());
			continue;
		}

		found_one = true;
		entry->m_fValue->SetData((char*)alloc, size);
		entry->m_fValue->ResetTimeStamp();
	}

	if( ferror(out) )
		cvs_err("Error while reading '%s' (error %d)\n", (const char*)home.c_str(), errno);

	fclose(out);
#endif /* qUnix */
	
	return found_one;
}

/*!
	Find given setting
	\param uniqueName Name of the setting to fing
	\return The setting pointer if found, NULL otherwise
*/
CPersistent::CPersistentEntry* CPersistent::Find(const char* uniqueName)
{
	vector<CPersistentEntry>::iterator i;
	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		CPersistentEntry& entry = *i;
		if( !strcmp(uniqueName, entry.m_fUniqueName) )
			return &entry;
	}

	return 0L;
}

/*!
	Check whether save is needed
	\param onlySettings true to only check setting that have kAddSettings
	\return true if save is needed, false otherwise
*/
bool CPersistent::NeedSave(bool onlySettings /*= false*/)
{
	vector<CPersistentEntry>::const_iterator i;
	for(i = m_fAllEntries->begin(); i != m_fAllEntries->end(); ++i)
	{
		const CPersistentEntry& entry = *i;
		if( onlySettings && (entry.m_fClass & kAddSettings) == 0 )
			continue;

		if( entry.m_fValue->m_fTimeStamp != 0 )
			return true;
	}

	return false;
}

bool CPersistent::LoadPersistentSettings(const char* token, const char* value, void* data)
{
#if INTERNAL_AUTHEN
	if( strcmp(token, "CVSROOT") == 0 )
	{
		gCvsPrefs = value;
	}
	else if( strcmp(token, "AUTHENTICATION") == 0 )
	{
		gAuthen.setkind(Authen::tokenToKind(value));
	}
	else if( strcmp(token, "PATH") == 0 )
	{
	}
	else
#endif /* INTERNAL_AUTHEN */
	{
		CPersistent::CPersistentEntry* entry = CPersistent::Find(token);
		if( entry == 0L )
		{
			//cvs_err("Persistent value '%s' is not registered\n", token);
		}
		else
		{
			if( entry->m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentBool)) )
			{
				CPersistentBool* pbool = (CPersistentBool*)entry->m_fValue;
				if( strcmp(value, "true") == 0 )
					*pbool = true;
				else if( strcmp(value, "false") == 0 )
					*pbool = false;
				else
					cvs_err("Invalid value '%s' for persistent '%s'\n", (const char*)value, entry->m_fUniqueName);
			}
			else if( entry->m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentInt)) )
			{
				CPersistentInt* pint = (CPersistentInt*)entry->m_fValue;
				int aint;
				int check = sscanf(value, "%d", &aint);
				if( check == 1 )
					*pint = aint;
				else
					cvs_err("Invalid value '%s' for persistent '%s'\n", (const char*)value, entry->m_fUniqueName);
			}
			else if( entry->m_fValue->IsKindOf(URUNTIME_CLASS(CPersistentString)) )
			{
				CPersistentString* pstr = (CPersistentString*)entry->m_fValue;
				*pstr = value;
			}
			else
			{
				int dataSize = strlen(value)/2;
				unsigned char* pdata = (unsigned char*)malloc(dataSize);
				if( pdata == NULL )
				{
					cvs_err("Couldn't read persistent value '%s' - out of memory!\n", entry->m_fUniqueName);
				}
				else
				{
					const char* s = value;
					for( int i(0); i < dataSize; ++i ) 
					{
						pdata[i] = (hexnibble(*s++)<<4) | hexnibble(*s++);
					}
					entry->m_fValue->SetData(pdata, dataSize);
					free(pdata);
				}
			}
		}
	}

	return false;
}


#if !qMPWCvs

//////////////////////////////////////////////////////////////////////////
// Settings management

/*!
	Format the default settings folder string
	\param path Return the formatted settings folder string
	\return true on success, false otherwise
*/
bool FormatDefaultSettingsLoc(string & path)
{
#ifdef WIN32
	if( !::GetAppPath(path) )
	{
		return false;
	}

#elif defined(qUnix)
	struct stat sb;

	path = gCvsPrefs.Home();
	NormalizeFolderPath(path);
	
	path += ".gcvs";

	if( stat(path.c_str(), &sb) != -1 )
	{
		if( !S_ISDIR(sb.st_mode) )
		{
			cvs_err("The file '%s' already exists and is not a directory", (const char*)path.c_str(), errno);
			return false;
		}
	}
	else if( mkdir(path.c_str(), 0750) != 0 )
	{
		cvs_err("Unable to create the directory '%s' (error %d)", (const char*)path.c_str(), errno);
		return false;
	}

#elif defined(qMacCvsPP)
	path = CMacCvsApp::GetMacCvsPrefsPath();
#endif

	NormalizeFolderPath(path);
	path += "Settings";

	return true;
}

/*!
	Get the settings folder (location)
	\param path Return the settings folder
	\return true on success, false otherwise
*/
static bool SettingsGetLoc(string& path)
{
	struct stat sb;

	path.erase();

	if( !IsNullOrEmpty(gCvsPrefs.SettingsFolder()) )
	{
		path = gCvsPrefs.SettingsFolder();
	}
	else
	{
		if( !FormatDefaultSettingsLoc(path) )
		{
			return false;
		}
	}

	if( stat(path.c_str(), &sb) != -1 && S_ISDIR(sb.st_mode) )
		return true;

#if qUnix || TARGET_RT_MAC_MACHO
	if( mkdir(path.c_str(), 0750) != 0 )
#else
	if( mkdir(path.c_str()) != 0 )
#endif
	{
		cvs_err("Failed to create the setting folder '%s' (error %d)", path.c_str(), errno);
		return false;
	}

	return true;
}

#ifndef WIN32

class USaveSettings : public UWidget
{
public:
	UDECLARE_DYNAMIC(USaveSettings)
public:
	USaveSettings(const char* path, const char* oldname);
	virtual ~USaveSettings();

	virtual void DoDataExchange(bool fill);

	enum
	{
#if qUnix
		kOK = EV_COMMAND_START,
		kCancel,
		kSettingName,
#else
		kSettingName = EV_COMMAND_START,
#endif
		kCopyPath,
		kCopyFrom,
		kCopyList
	};

	string  m_settingName;
	string  m_copyFromPath;

protected:
	string  m_path;
	string  m_oldname;
	map<string , string > m_list;

	ev_msg int OnCopyFrom(void);
#if qUnix
	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);
#endif

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(USaveSettings, UWidget)

UBEGIN_MESSAGE_MAP(USaveSettings, UWidget)
	ON_UCOMMAND(USaveSettings::kCopyFrom, USaveSettings::OnCopyFrom)
#if qUnix
	ON_UCOMMAND(USaveSettings::kOK, USaveSettings::OnOK)
	ON_UCOMMAND(USaveSettings::kCancel, USaveSettings::OnCancel)
#endif
UEND_MESSAGE_MAP()

#if qUnix
int USaveSettings::OnOK(void)
{
	EndModal(true);
	return 0;
}

int USaveSettings::OnCancel(void)
{
	EndModal(false);
	return 0;
}
#endif

USaveSettings::USaveSettings(const char* path, const char* oldname) :
	UWidget(::UEventGetWidID()), m_path(path), m_oldname(oldname)
{
}

USaveSettings::~USaveSettings()
{
}

void USaveSettings::DoDataExchange(bool fill)
{
	if( fill )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kCopyPath, (void*)(const char*)m_path.c_str());

		string  defname;
		if( m_oldname.empty() )
		{
			string  uppath;
			SplitPath(m_path.c_str(), uppath, defname);
		}
		else
			defname = m_oldname;

		UEventSendMessage(GetWidID(), EV_SETTEXT, kSettingName, (void*)(const char*)defname.c_str());

		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kCopyList, 0), 0L);

		UEventSendMessage(GetWidID(), EV_COMBO_RESETALL, kCopyList, 0L);

		if(GetAllPersistentSettings(m_list))
		{
			map<string, string>::const_iterator i;
			for(i = m_list.begin(); i != m_list.end(); ++i)
			{
				UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kCopyList,
					(void*)(const char*)((*i).first).c_str());
			}
		}
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kSettingName, &m_settingName);

		int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCopyFrom, 0L);
		if( state )
		{
			string what;
			UEventSendMessage(GetWidID(), EV_COMBO_GETSEL, kCopyList, &what);

			map<string, string>::const_iterator i;
			i = m_list.find(what);
			if(i != m_list.end())
			{
				m_copyFromPath = (*i).second;
			}
		}
	}
}

int USaveSettings::OnCopyFrom(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCopyFrom, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kCopyList, state), 0L);
	return 1;
}

#endif /* !WIN32 */

static bool LoadSettingsInternal(const char* fullname, 
								 bool (*func)(const char* token, const char* value, void* data), void* data)
{
	bool res = false;
	int line_length;
	char* line = 0L;
	size_t line_chars_allocated = 0;
	FILE* fp = fopen(fullname, "r");

	if( fp == 0L )
	{
		cvs_err("Failed to open the setting file '%s' for reading (error %d)", fullname, errno);
		goto fail;
	}

	while( (line_length = getline(&line, &line_chars_allocated, fp)) > 0 )
	{
		// read each token=value
		char* tmp = 0L;

		if( (tmp = strchr(line, '\n')) != 0L )
			*tmp = '\0';

		tmp = line;
		while( isspace(*tmp) && *tmp != '\0' )
			tmp++;

		if( *tmp == '\0' || *tmp == '#' )
			continue;

		string token;
		string value;
		const char* tmp2 = strchr(tmp, '=');
		if( tmp2 == 0L || tmp2 == tmp )
			continue;

		token.assign(tmp, tmp2 - tmp);
		value = tmp2 + 1;

		// check version
		if( strcmp(token.c_str(), "VERSION") == 0 )
		{
			if( atoi(value.c_str()) != 1 )
			{
				cvs_err("Newer format than expected in the setting file '%s'\n", fullname);
				goto fail;
			}

			continue;
		}

		if( func(token.c_str(), value.c_str(), data) )
			break;
	}

	if( line_length < 0 && !feof (fp) )
	{
		cvs_err("Cannot read file (error %d)\n", errno);
		goto fail;
	}

	res = true;

fail:
	if( line != 0L )
		free(line);
	
	if( fp!= 0L )
		fclose(fp);

	return res;
}

static void getSettingName(const char* fromfile, string & res)
{
	string base, ext;
	string uppath, filename;
	
	SplitPath(fromfile, uppath, filename);
	SplitFilename(filename.c_str(), base, ext);

	if( !base.empty() && stricmp(ext.c_str(), "txt") == 0 )
		res = base;
	else
		res = filename;
}

class CFindSettings : public TraversalReport
{
public:
	CFindSettings(const char* findPath) 
		: m_pathToFind(findPath), m_list(NULL)
	{
		TrimRight(m_pathToFind, kPathDelimiter);
	}

	CFindSettings(map<string, string>& list) 
		: m_list(&list)
	{
	}

	virtual ~CFindSettings() 
	{
	}

	virtual kTraversal OnError(const char* err, int errcode)
	{
		cvs_err(err);
		cvs_err("\n");
		
		return kTraversalError;
	}

	virtual kTraversal OnFile(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec);

	static bool searchPath(const char* token, const char* value, void* data);
	static bool searchAllPath(const char* token, const char* value, void* data);

	string m_pathToFind;
	string m_settingsFile;
	bool m_foundIt;
	string m_topfolder;
	map<string, string>* m_list;
};

bool CFindSettings::searchPath(const char* token, const char* value, void* data)
{
	CFindSettings* loader = (CFindSettings*)data;
	// if we give a sub-folder to look for, it will return the
	// top folder's settings
	if( strcmp(token, "PATH") == 0 &&
#if qUnix
		strcmp(value, loader->m_pathToFind.c_str()) == 0
#else
		stricmp(value, loader->m_pathToFind.c_str()) == 0
#endif
		)
	{
		loader->m_foundIt = true;
		loader->m_topfolder = value;

		return true;
	}

	return false;
}

bool CFindSettings::searchAllPath(const char* token, const char* value, void* data)
{
	CFindSettings *loader = (CFindSettings*)data;
	if( strcmp(token, "PATH") == 0 )
	{
		getSettingName(loader->m_settingsFile.c_str(), loader->m_settingsFile);
		map<string, string>::value_type v(loader->m_settingsFile, string(value));
		loader->m_list->insert(v);

		return true;
	}

	return false;
}

kTraversal CFindSettings::OnFile(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec)
{
	m_foundIt = false;

	if( m_list == 0L )
	{
		LoadSettingsInternal(fullname, searchPath, this);
	}
	else
	{
		m_settingsFile = fullname;
		LoadSettingsInternal(fullname, searchAllPath, this);
	}

	if( m_foundIt )
		m_settingsFile = fullname;
	
	return m_foundIt ? kStopTraversal : kContinueTraversal;
}

bool GetAllPersistentSettings(std::map<string , string >& res)
{
	string settingsDir;
	if( !SettingsGetLoc(settingsDir) )
		return false;

	CFindSettings finder(res);
	FileTraverse(settingsDir.c_str(), finder);

	return !res.empty();
}

bool HasPersistentSettings(const char* path)
{
	string settingsDir;
	if( !SettingsGetLoc(settingsDir) )
		return false;

	CFindSettings finder(path);
	FileTraverse(settingsDir.c_str(), finder);

	return !finder.m_settingsFile.empty();
}

/*!
	Load settings
	\param path Path to load the settings from
	\param quiet true to prevent displaying the message, false to display the message
*/
void LoadPersistentSettings(const char* path, bool quiet /*= false*/)
{
	string settingsDir;
	if( !SettingsGetLoc(settingsDir) )
		return;
	
	CFindSettings finder(path);
	FileTraverse(settingsDir.c_str(), finder);
	
	if( finder.m_settingsFile.empty() )
		return;
	
	if ( CPersistent::LoadAllSettings(finder.m_settingsFile.c_str()) && !quiet )
	{
		CColorConsole out;
		out << kBold << "Loaded settings for " << kNormal << kUnderline << path << kNormal << kBold << " successfully..." << kNormal << kNL;
	}
}


/*!
	Save all settings and print the message if the settings we actually saved
	\param settingFileName File name to save the settings in
	\param path Path to the file name
	\param quiet true to prevent displaying the message, false to display the message
	\note Just a wrapper for CPersistent::SaveAllSettings
*/
static void SaveAllSettings(const char* settingFileName, const char* path, bool quiet = false)
{
	if( CPersistent::SaveAllSettings(settingFileName, path) && !quiet )
	{
		CColorConsole out;
		out << kBold << "Saved settings for " << kNormal << kUnderline << path << kNormal << kBold << " successfully..." << kNormal << kNL;
	}
}

/*!
	"Save as" settings
	\param name Settings name
	\param path Path to settings
*/
static void SaveAsPersistentSettings(const char* name, const char* path)
{
	string settingFileName;
	if( !SettingsGetLoc(settingFileName) )
		return;

	NormalizeFolderPath(settingFileName);

	settingFileName += name;
	settingFileName += ".txt";

	SaveAllSettings(settingFileName.c_str(), path);
}

/*!
	Save settings
	\param path Path to settings
	\param quiet true to prevent displaying the message, false to display the message
*/
void SavePersistentSettings(const char* path, bool quiet /*= false*/)
{
	string settingsDir;
	if( !SettingsGetLoc(settingsDir) )
		return;

	CFindSettings finder(path);
	FileTraverse(settingsDir.c_str(), finder);

	if( finder.m_settingsFile.empty() )
		return;

	SaveAllSettings(finder.m_settingsFile.c_str(), finder.m_topfolder.c_str(), quiet);
}

/*!
	Prompt to create persistent settings
	\param path Path to settings
*/
void AskCreatePersistentSettings(const char* path)
{
	string settingsDir;
	if( !SettingsGetLoc(settingsDir) )
		return;

#ifndef WIN32
	int id = 0;
#endif

#ifdef qMacCvsPP
	id = dlg_SaveSettings;
#endif

	CFindSettings finder(path);
	FileTraverse(settingsDir.c_str(), finder);

	string oldName;

	if( !finder.m_settingsFile.empty() )
	{
		getSettingName(finder.m_settingsFile.c_str(), oldName);
	}

#ifdef WIN32
	CSaveSettingsDlg saveSettingsDlg(path, oldName.c_str());
#else
	USaveSettings* dlg = new USaveSettings(path, oldName.c_str());
#endif

#if qUnix
	void* wid;
#	if qGTK
	wid = UCreate_SettingsDlg();
#	endif

	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
#endif

#ifdef WIN32
	if( saveSettingsDlg.DoModal() == IDOK )
#else
	if( dlg->DoModal(id) )
#endif
	{

#ifdef WIN32
		string name = saveSettingsDlg.m_settingName;
#else
		string name = dlg->m_settingName;
#endif
		string fileToDelete;

		if( !name.empty() )
		{
			if( !oldName.empty() )
			{
				if( stricmp(oldName.c_str(), name.c_str()) != 0 )
				{
					string prompt("The setting '");
					prompt += oldName;
					prompt += "' already points on the same folder. ";
					prompt += "Press 'Delete' if you want to override this setting ";

					CvsAlert cvsAlert(kCvsAlertWarningIcon, 
						prompt.c_str(), "Two settings cannot point on the same location.", 
						BUTTONTITLE_DELETE, BUTTONTITLE_CANCEL);

					if( cvsAlert.ShowAlert() != kCvsAlertOKButton )
					{
#ifndef WIN32
						delete dlg;
#endif
						return;
					}

					fileToDelete = finder.m_settingsFile;
				}
			}

#ifdef WIN32
			if( saveSettingsDlg.m_copy && !saveSettingsDlg.m_copyFrom.IsEmpty() )
			{
				map<string, string> settingsList;
				if( GetAllPersistentSettings(settingsList) )
				{
					string what = saveSettingsDlg.m_copyFrom;
					
					map<string, string>::const_iterator i = settingsList.find(what);
					if( i != settingsList.end() )
					{
						string copyFromPath = (*i).second;
						LoadPersistentSettings(copyFromPath.c_str());
					}
				}
			}
#else
			if( !dlg->m_copyFromPath.empty() )
				LoadPersistentSettings(dlg->m_copyFromPath.c_str());
#endif

			NormalizeFileName(name);
			SaveAsPersistentSettings(name.c_str(), path);

			if( !fileToDelete.empty() )
			{
				if( remove(fileToDelete.c_str()) != 0 )
					cvs_err("Error while deleting '%s' (error %d)\n", fileToDelete.c_str(), errno);
			}
		}
	}

#ifndef WIN32
	delete dlg;
#endif
}

#endif // !qMPWCvs
