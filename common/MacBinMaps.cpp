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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

/*
 * MacBinMaps.cpp --- class to store the mac binary encoding mappings
 */

/* here should be a define, to recorgnize if this file is needed in dev-tools */
#ifdef TARGET_OS_MAC

#include <Carbon.h>
#include <stdlib.h>

#include "MacBinMaps.h"
#include "Persistent.h"

typedef struct
{
#	define MAX_MACBIN_DESC_OLD 63
	OSType sig;
	char desc[MAX_MACBIN_DESC_OLD + 1];
	MACENCODING encod;
	bool plainBin;
	bool useDefault;
} MACBINMAP_OLD;

typedef struct
{
	int             numMaps;
	MACBINMAP_OLD maps[1];
} MACBINMAPENTRIES_OLD;

class PMacBinMaps : public MacBinMaps, public CPersistent
{
public:
	PMacBinMaps(const char *uniqueName) : CPersistent(uniqueName, kNoClass)
	{
		if(entries != 0L)
		{
			free(entries);
			entries = 0L;
		}
	}
	
	virtual ~PMacBinMaps()
	{
	}
	
	virtual unsigned int SizeOf(void) const
	{
		if(entries == 0L)
			return 0;
		return sizeof(MACBINMAPENTRIES) + (entries->numMaps - 1) * sizeof(MACBINMAP);
	}
	
	virtual const void *GetData(void) const
	{
		return entries;
	}
	virtual void SetData(const void *ptr, unsigned int size)
	{
		if(entries != 0L)
		{
			free(entries);
			entries = 0L;
		}
		
		if(size < sizeof(int))
			return;
		
		if ( size < (sizeof(int) + sizeof(unsigned long))  ||
		     *static_cast<const unsigned long*>(ptr) != MACBINMAP_COOKIEVALUE )
		{
		  const MACBINMAPENTRIES_OLD *oldEntries = static_cast<const MACBINMAPENTRIES_OLD *>(ptr);
		  if((sizeof(MACBINMAPENTRIES_OLD) + (oldEntries->numMaps - 1) * sizeof(MACBINMAP_OLD)) != size)
			  return;
  		
  		entries = (MACBINMAPENTRIES *)malloc((oldEntries->numMaps ? oldEntries->numMaps-1 : 0) * sizeof(MACBINMAP) + sizeof(MACBINMAPENTRIES));
  		if(entries == 0L)
  			return;
  		entries->cookie = MACBINMAP_COOKIEVALUE;
  		entries->numMaps = oldEntries->numMaps;
  	  for ( int i(0); i < oldEntries->numMaps; ++i )
  	  {
  	    entries->maps[i].sig = oldEntries->maps[i].sig;
  	    entries->maps[i].ext[0] = '\0';
  	    strncpy(entries->maps[i].desc, oldEntries->maps[i].desc, MAX_MACBIN_DESC);
  	    entries->maps[i].encod = oldEntries->maps[i].encod;
  	    if ( oldEntries->maps[i].plainBin ) entries->maps[i].encod = MAC_PLAIN_BINARY;
  	    entries->maps[i].useDefault = oldEntries->maps[i].useDefault;
  	  }
		}
		else
		{		
  		MACBINMAPENTRIES *newEntries = (MACBINMAPENTRIES *)ptr;
  		if((sizeof(MACBINMAPENTRIES) + (newEntries->numMaps - 1) * sizeof(MACBINMAP)) != size)
  			return;
  		
  		entries = (MACBINMAPENTRIES *)malloc(size);
  		if(entries == 0L)
  			return;
  		
  		memcpy(entries, ptr, size);
    }
	}
	PMacBinMaps& operator = (const MacBinMaps& inSrc)
	{
	  MacBinMaps::operator = (inSrc);
	  this->TouchTimeStamp();
	  return *this;
  }
} gAllBinMaps("P_MacBinMaps");

MacBinMaps::MacBinMaps()
{
	Reset();
}

MacBinMaps::~MacBinMaps()
{
	if(entries != 0L)
		free(entries);
}

void MacBinMaps::Reset(void)
{
	entries = (MACBINMAPENTRIES *)malloc(sizeof(int)+sizeof(unsigned long));
	if(entries == 0L)
		return;
  entries->cookie = MACBINMAP_COOKIEVALUE;
	entries->numMaps = 0;
}

const char *MacBinMaps::GetEnvPlainTypes(void)
{
  return MacBinMaps::GetEnvTypes(MAC_PLAIN_BINARY);
}

const char *MacBinMaps::GetEnvTypes(MACENCODING type)
{
	static CStr result;
	result = "";
	const MACBINMAPENTRIES * allEntries = gAllBinMaps.Entries();
	if(allEntries == 0L)
		return 0L;
	
	int i;
	for(i = 0; i < allEntries->numMaps; i++)
	{
		const MACBINMAP & curmap = allEntries->maps[i];
		if(!curmap.useDefault && curmap.encod == type)
		{
			result << ((char *)&curmap.sig)[0];
			result << ((char *)&curmap.sig)[1];
			result << ((char *)&curmap.sig)[2];
			result << ((char *)&curmap.sig)[3];
			result << '/';
			result << curmap.ext;
			result << ';';
		}
	}
	return result;
}

void MacBinMaps::LoadDefaults(void)
{
	if(gAllBinMaps.SizeOf() == 0L)
	{
		CFDictionaryRef   infoDict(CFBundleGetInfoDictionary(CFBundleGetMainBundle()));
		if ( infoDict )
		{
		  CFArrayRef    defaultMappings(static_cast<CFArrayRef>(CFDictionaryGetValue(infoDict, CFSTR("DefaultBinaryMappings"))));
		  if ( defaultMappings )
		  {
		    UInt32    count(CFArrayGetCount(defaultMappings));
		    for ( UInt32 i(0); i < count; ++i )
		    {
		      CFDictionaryRef   mapping(static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(defaultMappings, i)));
		      if ( mapping )
		      {
		        CFStringRef   fileType(static_cast<CFStringRef>(CFDictionaryGetValue(mapping, CFSTR("FileType"))));
		        CFStringRef   extension(static_cast<CFStringRef>(CFDictionaryGetValue(mapping, CFSTR("Extension"))));
		        CFStringRef   description(static_cast<CFStringRef>(CFDictionaryGetValue(mapping, CFSTR("Description"))));
		        CFStringRef   encoding(static_cast<CFStringRef>(CFDictionaryGetValue(mapping, CFSTR("Encoding"))));
		        
		        MACENCODING   encod(MAC_HQX);
		        bool          defaultEncoding(false);
		        if ( CFStringCompare(encoding, CFSTR("PlainBinary"), kCFCompareCaseInsensitive) == kCFCompareEqualTo )
		          encod = MAC_PLAIN_BINARY;
		        else if ( CFStringCompare(encoding, CFSTR("AppleSingle"), kCFCompareCaseInsensitive) == kCFCompareEqualTo )
		          encod = MAC_APPSINGLE;
		        else if ( CFStringCompare(encoding, CFSTR("Default"), kCFCompareCaseInsensitive) == kCFCompareEqualTo )
		          defaultEncoding = true;
		        
		        OSType        type('\?\?\?\?');
		        char          ext[MAX_MACBIN_EXTENSION];
		        char          desc[MAX_MACBIN_DESC];
	          CFIndex       usedBufLen;
	          if ( ((CFStringGetLength(fileType) == 4  &&  CFStringGetBytes(fileType, CFRangeMake(0,4), kCFStringEncodingMacRoman, 0, false, (UInt8*)&type, 4, &usedBufLen) == 4) ||
	                 CFStringGetLength(fileType) == 0)  &&
	               usedBufLen == 4  &&
	               CFStringGetCString(extension, ext, sizeof(ext), kCFStringEncodingUTF8)  &&
	               CFStringGetCString(description, desc, sizeof(desc), kCFStringEncodingUTF8) )
	            this->AddEntry(type, ext, desc, encod, defaultEncoding);
		      }
		    }
		  }
		}
	}
	else
	{
		*this = gAllBinMaps;
	}
}

void MacBinMaps::SaveDefaults(void)
{
	gAllBinMaps = *this;
}

const MACBINMAP *MacBinMaps::AddEntry(OSType sig, const char* extension, const char *desc, MACENCODING encod, bool useDefault)
{
	if(entries == 0L)
		return 0L;
	
	if ( extension == NULL ) extension = "";
	
	int i;
	for(i = 0; i < entries->numMaps; i++)
	{
		if(entries->maps[i].sig == sig  &&  strcmp(entries->maps[i].ext, extension) == 0)
			break;
	}
	
	if(i != entries->numMaps)
	{
		MACBINMAP & curmap = entries->maps[i];
		curmap.sig = sig;
		strncpy(curmap.ext, extension, MAX_MACBIN_EXTENSION);
		curmap.desc[0] = '\0';
		if(desc != 0L)
			strncpy(curmap.desc, desc, MAX_MACBIN_DESC);
		curmap.encod = encod;
		curmap.useDefault = useDefault;
		return &curmap;
	}
	
	entries = (MACBINMAPENTRIES *)realloc(entries, sizeof(MACBINMAPENTRIES) + entries->numMaps * sizeof(MACBINMAP));
	if(entries == 0L)
		return 0L;
	
	MACBINMAP & newmap = entries->maps[entries->numMaps++];
	newmap.sig = sig;
	strncpy(newmap.ext, extension, MAX_MACBIN_EXTENSION);
	newmap.desc[0] = '\0';
	if(desc != 0L)
		strncpy(newmap.desc, desc, MAX_MACBIN_DESC);
	newmap.encod = encod;
	newmap.useDefault = useDefault;
	return &newmap;
}

void MacBinMaps::RemoveEntry(OSType sig, const char* extension)
{
	if(entries == 0L)
		return;
	if ( extension == NULL ) extension = "";
	
	int i;
	for(i = 0; i < entries->numMaps; i++)
	{
		if(entries->maps[i].sig == sig && strcmp(entries->maps[i].ext, extension) == 0)
			break;
	}
	
	if(i == entries->numMaps)
		return;

	memmove(&entries->maps[i], &entries->maps[i + 1],
		(entries->numMaps - i - 1) * sizeof(MACBINMAP));
	entries->numMaps--;
	entries = (MACBINMAPENTRIES *)realloc(entries, sizeof(MACBINMAPENTRIES) + (entries->numMaps - 1) * sizeof(MACBINMAP));
}

MacBinMaps & MacBinMaps::operator=(const MacBinMaps & otherMap)
{
	if(entries != 0L)
		free(entries);
	Reset();
	if(entries == 0L)
		return *this;

	const MACBINMAPENTRIES *theseEntries = otherMap.Entries();
	if(theseEntries != 0L) for(int i = 0; i < theseEntries->numMaps; i++)
	{
		const MACBINMAP & newmap = theseEntries->maps[i];
		AddEntry(newmap.sig, newmap.ext, newmap.desc, newmap.encod, newmap.useDefault);
	}
	return *this;
}

#endif /* TARGET_OS_MAC */
