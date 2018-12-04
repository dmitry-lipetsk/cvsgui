#pragma once

#include "GUSIFileSpec.h"
#include "ustr.h"
#include <LCFString.h>

UInt8 *posix_to_utf8(const char *path)
{
#if 0
	static char utfpath[PATH_MAX];

	utfpath[0] = '\0';
	
	CFStringRef str = ::CFStringCreateWithCString(kCFAllocatorDefault, path, kCFStringEncodingMacRoman);
	if(str != NULL)
	{
		::CFStringGetCString(str, utfpath, sizeof(utfpath), kCFStringEncodingUTF8);
		::CFRelease(str);
	}

	return (UInt8 *)utfpath;
#else
	return (UInt8 *)path;
#endif
}

char *utf8_to_posix(const UInt8 *utfpath)
{
#if 0
	static char path[PATH_MAX];

	path[0] = '\0';
	
	CFStringRef str = ::CFStringCreateWithCString(kCFAllocatorDefault, (char *)utfpath, kCFStringEncodingUTF8);
	if(str != NULL)
	{
		::CFStringGetCString(str, path, sizeof(path), kCFStringEncodingMacRoman);
		::CFRelease(str);
	}

	return path;
#else
	return (char *)utfpath;
#endif
}

const GUSICatInfo * GUSIFileSpec::CatInfo() const
{
	if (fValidInfo)	// Valid already
		return &fInfo;
		
	fValidInfo = !(fError = FSGetCatalogInfo(&fSpec, kFSCatInfoGettableInfo, &fInfo.info,
		NULL, NULL, NULL));
		
	return fError ? nil : &fInfo;
}

GUSIFileSpec::GUSIFileSpec(const char * path)
	: fValidInfo(false)
{
	fError = FSPathMakeRef(posix_to_utf8(path), &fSpec, NULL);
	if(fError == fnfErr)
		fError = noErr;
}

GUSIFileSpec::GUSIFileSpec(const GUSIFileSpec & spec)
	: fValidInfo(false), fSpec(spec.fSpec), fError(spec.fError)
{
}

GUSIFileSpec::GUSIFileSpec(const FSRef & spec)							
	: fValidInfo(false), fSpec(spec), fError(noErr)
{
}

GUSIFileSpec::GUSIFileSpec(const FSSpec & spec)							
	: fValidInfo(false), fError(noErr)
{
	fError = FSpMakeFSRef(&spec, &fSpec);
}

static UStr scratch1;
static UStr scratch2;
static UStr scratch3;
static UStr scratch4;
static UStr scratch5;
static int cnt = 1;

char *	GUSIFileSpec::FullPath() const
{
	cnt++;
	if(cnt == 6)
		cnt = 1;
	
	UStr *res;
	if(cnt == 1)
		res = &scratch1;
	else if(cnt == 2)
		res = &scratch2;
	else if(cnt == 3)
		res = &scratch3;
	else if(cnt == 4)
		res = &scratch4;
	else
		res = &scratch5;

	UInt8 path[PATH_MAX];
	if((fError = FSRefMakePath(&fSpec, path, sizeof(path))) == noErr)
	{
		*res = utf8_to_posix(path);
		return (char *)(*res).c_str();
	}
	
	return NULL;
}

GUSIFileSpec & GUSIFileSpec::AddPathComponent(const char * name, int length)
{
	LCFString str(name, kCFStringEncodingUTF8);
	UniChar buffer[PATH_MAX];
	CFRange range = {0, str.GetLength()};
	::CFStringGetCharacters(str, range, buffer);
	FSRef newref;
	
	fError = ::FSMakeFSRefUnicode(&fSpec, str.GetLength(), buffer, kTextEncodingDefaultFormat, &newref);
	
	if (fError == noErr)
	{
		fValidInfo = false;

		fSpec = newref;
	}

	return *this;
}
