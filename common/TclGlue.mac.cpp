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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2001
 */

/*
 * TclGlue.mac.cpp --- glue to the TCL language specific to mac/carbon
 */

#include "stdafx.h"

#if qCarbon

#include "TclGlue.mac.h"
#include <CFBundle.h>
#include <stdlib.h>
#include <string.h>

#include "AppConsole.h"
#include <mach-o/dyld.h>

// struct to keep necessary logistic data for maintaining a connection
// to a shared library on various implementations
//
struct _CompatConnectID
{
	CFURLRef		  bundleURL;
	CFBundleRef		myBundle;
	const struct mach_header *      dyld_header;

  _CompatConnectID() : 
      bundleURL(NULL), 
      myBundle(NULL)
     ,dyld_header(NULL)
  {
  }
};

int CarbonCompatLoadLibrary(CompatConnectID * connID, const char *name_dll)
{
	if(connID == 0L)
		return 0;

	*connID = 0L;

	_CompatConnectID *c = new _CompatConnectID;
	
	*connID = (CompatConnectID)c;
	
	if(c == 0L)
		return 0;

	CFStringRef s = CFStringCreateWithCString(0L, name_dll, kCFStringEncodingUTF8);
	if(s == 0L)
		goto fail;
	
	c->bundleURL	= CFURLCreateWithFileSystemPath(0L, s, kCFURLPOSIXPathStyle, false );
	if(c->bundleURL == 0L)
		goto fail;

	c->myBundle	= CFBundleCreate(0L, c->bundleURL );
	if(c->myBundle == 0L)
	{
    // check whether file exists
    //
    FSRef   libFile;
    FSCatalogInfo   libFileInfo;
    if ( CFURLGetFSRef(c->bundleURL, &libFile)  &&  
         FSGetCatalogInfo(&libFile, kFSCatInfoNodeFlags, &libFileInfo, NULL, NULL, NULL) == noErr  &&
         (libFileInfo.nodeFlags & kFSNodeIsDirectoryMask) == 0 )
    {
      CFRelease(c->bundleURL);
      c->bundleURL = 0L;

      c->dyld_header = NSAddImage(name_dll, NSADDIMAGE_OPTION_NONE);
      if ( c->dyld_header == NULL ) 
      {
    		cvs_err("Error loading %s\n", name_dll);
    		goto fail;
      }
    }
    else goto fail; // not a file or file not found
  }
  else
  {
		if(!CFBundleLoadExecutable( c->myBundle ))
			goto fail;
  }
  
	CFRelease( s );
	return 1;
fail:
	if(s != 0L)
		CFRelease( s );
	if(c != 0L)
	{
		if(c->bundleURL != 0L)
		  CFRelease( c->bundleURL );
		if(c->myBundle != 0L)
		  CFRelease( c->myBundle );
		*connID = 0L;
		delete c;
	}
	return 0;
}

int CarbonCompatCloseLibrary(CompatConnectID * connID)
{
	if(connID == 0L)
		return 0;

	_CompatConnectID *c = (_CompatConnectID *)*connID;
	if(c != 0L)
	{
		if ( c->dyld_header )
		{
// %%% ??? how to unload an image ???
		}
		else
		{
			CFBundleUnloadExecutable( c->myBundle );
			CFRelease( c->bundleURL );
			CFRelease( c->myBundle );
		}
		delete c;
	}
	*connID = 0L;

	return 1;
}

void *_CarbonCompatCallLibrary(CompatConnectID connID, const char *name_func)
{
	_CompatConnectID *c = (_CompatConnectID *)connID;

  if ( c->dyld_header )
  {
  	char funcname[258];

  	sprintf(funcname, "_%.200s", name_func);
  	if (NSIsSymbolNameDefinedInImage(c->dyld_header, funcname)) {
  	  NSSymbol theSym;
  	  void* p;
  		theSym = NSLookupSymbolInImage(c->dyld_header, funcname, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND);
  		p = NSAddressOfSymbol(theSym);
#if qCvsDebug
      if ( p == NULL ) cvs_err("_CarbonCompatCallLibrary(dyld): symbol '%s' not found!\n", name_func);
#endif
  		return p;
  	}
  	
#if qCvsDebug
    cvs_err("_CarbonCompatCallLibrary(dyld): symbol '%s' not found!\n", name_func);
#endif
  	return 0L;
  }
  else
	{
		CFStringRef s = CFStringCreateWithCString(0L, name_func, kCFStringEncodingMacRoman);
		if(s == 0L)
			return 0L;
		
		void *ptr = CFBundleGetFunctionPointerForName(c->myBundle, s);
		CFRelease( s );
  
#if qCvsDebug
    if ( ptr == NULL ) cvs_err("_CarbonCompatCallLibrary(CFBundle): symbol '%s' not found!\n", name_func);
#endif
	  return ptr;
  }
	
#if qCvsDebug
  cvs_err("_CarbonCompatCallLibrary: symbol '%s' not found!\n", name_func);
#endif

	return 0L;
}

#define CompatLoadLibrary CarbonCompatLoadLibrary
#define CompatCloseLibrary CarbonCompatCloseLibrary
#define _CompatCallLibrary _CarbonCompatCallLibrary

#if 0 // TARGET_RT_MAC_MACHO
typedef void *dl_funcptr;

static dl_funcptr _PyImport_GetDynLoadFunc(const char *shortname, const char *pathname)
{
	dl_funcptr p = NULL;
	char funcname[258];

	sprintf(funcname, "_init%.200s", shortname);

	/* This is also NeXT-specific. However, frameworks (the new style
	of shared library) and rld() can't be used in the same program;
	instead, you have to use dyld, which is mostly unimplemented. */
	{
		NSObjectFileImageReturnCode rc;
		NSObjectFileImage image;
		NSModule newModule;
		NSSymbol theSym;
		const char *errString;
	
		if (NSIsSymbolNameDefined(funcname)) {
			theSym = NSLookupAndBindSymbol(funcname);
			p = (dl_funcptr)NSAddressOfSymbol(theSym);
			return p;
		}
		rc = NSCreateObjectFileImageFromFile(pathname, &image);
		switch(rc) {
		    default:
		    case NSObjectFileImageFailure:
		    case NSObjectFileImageFormat:
		    /* for these a message is printed on stderr by dyld */
			errString = "Can't create object file image";
			break;
		    case NSObjectFileImageSuccess:
			errString = NULL;
			break;
		    case NSObjectFileImageInappropriateFile:
			errString = "Inappropriate file type for dynamic loading";
			break;
		    case NSObjectFileImageArch:
			errString = "Wrong CPU type in object file";
			break;
		    case NSObjectFileImageAccess:
			errString = "Can't read object file (no access)";
			break;
		}
		if (errString == NULL) {
			newModule = NSLinkModule(image, pathname,
				NSLINKMODULE_OPTION_BINDNOW|NSLINKMODULE_OPTION_RETURN_ON_ERROR);
			if (!newModule)
				errString = "Failure linking new module";
		}
		if (errString != NULL) {
			cvs_err("%s\n", errString);
			return NULL;
		}
		if (!NSIsSymbolNameDefined(funcname)) {
			/* UnlinkModule() isn't implemented in current versions, but calling it does no harm */
			NSUnLinkModule(newModule, FALSE);
			cvs_err("Loaded module does not contain symbol %.200s", funcname);
			return NULL;
		}
		theSym = NSLookupAndBindSymbol(funcname);
		p = (dl_funcptr)NSAddressOfSymbol(theSym);
 	}

	return p;
}

void *_tcl_cfm_macho(void *func)
{
  return func;
}

int CarbonCompatLoadLibrary(CompatConnectID * connID, const char *name_dll)
{
	NSObjectFileImageReturnCode rc;
	NSObjectFileImage image;
	NSModule newModule;
	NSSymbol theSym;
	const char *errString;

	*connID = 0L;
	
	rc = NSCreateObjectFileImageFromFile(name_dll, &image);
	switch(rc) {
	    default:
	    case NSObjectFileImageFailure:
	    case NSObjectFileImageFormat:
	    /* for these a message is printed on stderr by dyld */
		errString = "Can't create object file image";
		break;
	    case NSObjectFileImageSuccess:
		errString = NULL;
		break;
	    case NSObjectFileImageInappropriateFile:
		errString = "Inappropriate file type for dynamic loading";
		break;
	    case NSObjectFileImageArch:
		errString = "Wrong CPU type in object file";
		break;
	    case NSObjectFileImageAccess:
		errString = "Can't read object file (no access)";
		break;
	}
	if (errString == NULL) {
		newModule = NSLinkModule(image, name_dll,
			NSLINKMODULE_OPTION_BINDNOW|NSLINKMODULE_OPTION_RETURN_ON_ERROR);
		if (!newModule)
			errString = "Failure linking new module";
	}
	if (errString != NULL) {
		cvs_err(errString);
		return 0;
	}
	
	*connID = (CompatConnectID)newModule;
	
	return 1;
}

int CarbonCompatCloseLibrary(CompatConnectID * connID)
{
	if(connID == 0L || *connID == 0L)
		return 0;

	NSUnLinkModule((NSModule)*connID, FALSE);
	*connID = 0L;

	return 1;
}

void *_CarbonCompatCallLibrary(CompatConnectID connID, const char *name_func)
{
	dl_funcptr p = NULL;
	char funcname[258];

	sprintf(funcname, "_init%.200s", name_func);

	/* This is also NeXT-specific. However, frameworks (the new style
	of shared library) and rld() can't be used in the same program;
	instead, you have to use dyld, which is mostly unimplemented. */
	NSSymbol theSym;

	if (NSIsSymbolNameDefined(funcname)) {
		theSym = NSLookupAndBindSymbol(funcname);
		p = (dl_funcptr)NSAddressOfSymbol(theSym);
		return p;
	}
	
	return 0L;
}

#endif // TARGET_RT_MAC_MACHO

#endif // qCarbon
