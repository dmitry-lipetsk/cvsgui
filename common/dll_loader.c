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
 * dll_loader.cpp --- code to load dynamic code
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif /* HAVE_CONFIG_H */

#if !defined(WIN32) && !defined(TARGET_OS_MAC)
#	include "dynl.h"
#endif /* !WIN32 && !TARGET_OS_MAC */

#ifdef WIN32
#	include <windows.h>
#	include <winuser.h>
#	include <stdio.h>
#else
#	include <string.h>
#endif /* WIN32 */

#include "dll_loader.h"

#ifdef WIN32

#if _MSC_VER >= 1400
#	pragma warning(disable : 4996)
#endif

#ifndef FALSE
#	define FALSE 0
#endif

#ifndef TRUE
#	define TRUE 1
#endif

#define STR_REGKEY_APPKEY		"Software\\WinCvs\\wincvs"
#define STR_REGKEY_LASTLIBVAL	"LastLoadLibrary"

static void SetLastLoadedLibrary(const char* name);
static int GetFailedLibrary(char* lastLibPath, int pathSize);

#endif

void* sCompatProc;

#ifdef TARGET_OS_MAC
#	include <TextUtils.h>
	static ProcPtr getSymFunction(CompatConnectID connID, const Str255 name)
	{
		long symCount;
		Ptr symAddr;
		int i;
		OSErr err = CountSymbols(connID, &symCount);
		if(err != noErr)
			return 0L;
		
		for(i = 0; i < symCount; i++)
		{
			CFragSymbolClass symClass;
			Str255 symName;
			err = GetIndSymbol(connID, i, symName, &symAddr, &symClass);
			if(err != noErr)
				return 0L;
			if(memcmp(symName, name, name[0]) == 0)
				break;
		}
		if(i == symCount)
			return 0L;
		return (ProcPtr)symAddr;
	}
#endif /* TARGET_OS_MAC */

/*!
	Load the shared library
	\param connID Return the connection ID
	\param name_dll	Filename of the library (on Mac the fragment name of the shared library)
	\return true on success, false otherwise
*/
int CompatLoadLibrary(CompatConnectID* connID, const char* name_dll)
{
#ifdef WIN32
	int loadFailed = FALSE;
	int confirmLoad = FALSE;
#endif

	if( connID == 0L )
		return 0;

#ifdef TARGET_OS_MAC
	{
		OSErr err;
		Ptr mainAddr;
		Str255 errMessage;
		Str255 pname;
		
		c2pstrcpy(pname, name_dll);
		err = GetSharedLibrary(pname, kPowerPCCFragArch, kPrivateCFragCopy, connID, &mainAddr, errMessage);
		return err == noErr;
	}
#elif defined(WIN32)

	// In case the dll caused problems we will give user a warning and a chance to cancel load
	{
		char lastLibPath[_MAX_PATH] = "";

		if( GetFailedLibrary(lastLibPath, _MAX_PATH) )
		{
			if( stricmp(name_dll, lastLibPath) == 0 )
			{
				confirmLoad = TRUE;
			}
			else
			{
				loadFailed = TRUE;
			}
		}
	}

	if( confirmLoad )
	{
		char message[1024] = "";

		sprintf(message, 
			"DLL library loading deadlock detected!\n"
			"File: %s\n"
			"DLL failed to load properly (stalled) during previous attempt. Please review your settings in the preferences and your PATH environmental variable in the system settings to assure this is the correct file and there are no incompatible duplicates that might be accessed instead.\n"
			"You can reset this warning message by deleting the following registry key: %s\n"
			"\nAre you sure you want to attempt loading the file?",
			name_dll,
			"HKEY_CURRENT_USER" "\\" STR_REGKEY_APPKEY "\\" STR_REGKEY_LASTLIBVAL);

		if( MessageBox(NULL, 
			message, 
			"WinCvs DLL load deadlock", 
			MB_ICONERROR | MB_YESNO | MB_DEFBUTTON2 | MB_SYSTEMMODAL) != IDYES )
		{
			return 0L;
		}
	}

	if( !loadFailed || confirmLoad )
	{
		SetLastLoadedLibrary(name_dll);
	}
	
	*connID = LoadLibrary(name_dll);
	
	if( !loadFailed || confirmLoad )
	{
		SetLastLoadedLibrary("");
	}

	return (*connID) != 0L;
#else
	*connID = dynl_link(name_dll);
	return (*connID) != 0L;
#endif
}

/*!
	Load the shared library
	\param connID The connection ID, should not be used anymore after this call 
	\return true on success, false otherwise
*/
int CompatCloseLibrary(CompatConnectID* connID)
{
	if( connID == 0L )
		return 0;

#ifdef TARGET_OS_MAC
	CloseConnection(connID);
#elif defined(WIN32)
	FreeLibrary(*connID);
	connID = 0L;
#else
	dynl_unlink(*connID);
	connID = 0L;
#endif

	return 1;
}

/*!
	Call a function in the shared library
	\param connID The connection ID
	\param name_func Function name be called
	\return The function address
*/
void* _CompatCallLibrary(CompatConnectID connID, const char* name_func)
{
#ifdef TARGET_OS_MAC
	ProcPtr symAddr;
	Str255 pname;

	c2pstrcpy(pname, name_func);

	symAddr = getSymFunction(connID, pname);
	return symAddr;
#elif defined(WIN32)
	return GetProcAddress(connID, name_func);
#else
	return dynl_func(name_func, connID);
#endif
}

//////////////////////////////////////////////////////////////////////////
// Internal functions

#ifdef WIN32

/*!
	Set last loaded library setting
	\param name Name of the last loaded library, empty string to reset
*/
static void SetLastLoadedLibrary(const char* name)
{
	HKEY hKey;
	if( RegOpenKeyEx(HKEY_CURRENT_USER, STR_REGKEY_APPKEY, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS )
	{
		RegSetValueEx(hKey, STR_REGKEY_LASTLIBVAL, 0, REG_SZ, name, strlen(name) + 1);
		RegCloseKey(hKey);
	}
}

/*!
	Get the failed library path, if any
	\param lastLibPath Return library path, you must allocate memory for that
	\param pathSize Size of the library path buffer
	\return TRUE if there is a library that failed to load, FALSE otherwise
*/
static int GetFailedLibrary(char* lastLibPath, int pathSize)
{
	int res = FALSE;

	HKEY hKey;
	if( RegOpenKeyEx(HKEY_CURRENT_USER, STR_REGKEY_APPKEY, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS )
	{
		if( RegQueryValueEx(hKey, STR_REGKEY_LASTLIBVAL, NULL, NULL, (LPBYTE)lastLibPath, &pathSize) == ERROR_SUCCESS )
		{
			if( pathSize > 0 && lastLibPath[0] != 0L )
			{
				res = TRUE;
			}
		}
		
		RegCloseKey(hKey);
	}

	return res;
}

/*!
	Reset failed library setting
	\note To be called at the exit of application since a controlled exit means correct loading
*/
void ResetFailedLibrary()
{
	SetLastLoadedLibrary("");
}

#endif
