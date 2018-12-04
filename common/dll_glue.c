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
 * dll_glue.cpp --- glue code for the dll to communicate with the caller
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <unistd.h>
#include <stdio.h>
#include <setjmp.h>
#include <errno.h>

#include "dll_glue.h"
#include "dll_garbage.h"
#include "GUSIMSLSetup.h"

#if	!TARGET_API_MAC_CARBON
	static UniversalProcPtr __glue_getenv = 0L;
	static UniversalProcPtr __glue_consoleout = 0L;
	static UniversalProcPtr __glue_consoleerr = 0L;
	static UniversalProcPtr __glue_consolein = 0L;
	static UniversalProcPtr __glue_eventdispatch = 0L;
#else /* TARGET_API_MAC_CARBON */
	static dllglue_getenv_func __glue_getenv = 0L;
	static dllglue_consoleout_func __glue_consoleout = 0L;
	static dllglue_consoleout_func __glue_consoleerr = 0L;
	static dllglue_consolein_func __glue_consolein = 0L;
	static dllglue_eventdispath_func __glue_eventdispatch = 0L;
#endif /* TARGET_API_MAC_CARBON */

static int __glue_exit_code = 0;
static jmp_buf _glue_buf;

extern int __no_msl_lf_convert; // GUSIMSLSetup.cpp

extern int main(int argc, char **argv);

void
dllglue_flushconsole(void)
{
	fflush(stdout);
	fflush(stderr);
}

static void initialize_all(void)
{
	GusiMSLSetWriteConsole(dllglue_consoleout);
	GusiMSLSetErrConsole(dllglue_consoleerr);
	GusiMSLSetInConsole(dllglue_consolein);
	GusiMSLSetHandleEvent(dllglue_dispatchevent);

	// create text files with the Unix LF (0x0A)
	// the default is CR on Classic/Carbon-CFM
	if(getenv("CVSUSEUNIXLF") != NULL)
	{
		__no_msl_lf_convert = 1;
	}
}

static void flush_all(void)
{
	garbyRemove(true, false);
	dllglue_flushconsole();
	dllglue_setgetenv(0L);
	dllglue_setconsoleout(0L);
	dllglue_setconsoleerr(0L);
	dllglue_setconsolein(0L);
	dllglue_seteventdispatch(0L);
}

void
dllglue_exit(int code)
{
	__glue_exit_code = code;
	longjmp(_glue_buf, 1);
}

char *
dllglue_getenv(const char * name)
{
	if(__glue_getenv != 0L)
	{
#if	!TARGET_API_MAC_CARBON
		return (char *)CallUniversalProc(__glue_getenv, uppGetEnvProcInfo, name);
#else /* TARGET_API_MAC_CARBON */
		return __glue_getenv(name);
#endif /* TARGET_API_MAC_CARBON */
	}
	return 0L;
}

long dllglue_consoleout(char *buffer, long n)
{
#if	!TARGET_API_MAC_CARBON
	if(__glue_consoleout != 0L)
	{
		return (int)CallUniversalProc(__glue_consoleout, uppConsoleOutProcInfo, buffer, n);
	}
#else
	if(__glue_consoleout != 0L)
	{
		return __glue_consoleout(buffer, n);
	}
#endif
	return 0;
}

long dllglue_consoleerr(char *buffer, long n)
{
#if	!TARGET_API_MAC_CARBON
	if(__glue_consoleerr != 0L)
	{
		return (int)CallUniversalProc(__glue_consoleerr, uppConsoleOutProcInfo, buffer, n);
	}
#else
	if(__glue_consoleerr != 0L)
	{
		return __glue_consoleerr(buffer, n);
	}
#endif
	return 0;
}

long dllglue_consolein(char *buffer, long n)
{
#if	!TARGET_API_MAC_CARBON
	if(__glue_consolein != 0L)
	{
		return (int)CallUniversalProc(__glue_consolein, uppConsoleInProcInfo, buffer, n);
	}
#else
	if(__glue_consolein != 0L)
	{
		return __glue_consolein(buffer, n);
	}
#endif
	return 0;
}

int dllglue_dispatchevent(EventRecord *userevent)
{
#if	!TARGET_API_MAC_CARBON
	if(__glue_eventdispatch != 0L)
	{
		return (int)CallUniversalProc(__glue_eventdispatch, uppEventDispatchProcInfo, userevent);
	}
#else
	if(__glue_eventdispatch != 0L)
	{
		return __glue_eventdispatch(userevent);
	}
#endif
	return 0;
}

int
dllglue_main(const char *path, int argc, char **argv)
{
	if(setjmp(_glue_buf) == 0)
	{
		initialize_all();

		if(path != 0L && chdir(path) != 0)
		{
			fprintf(stderr, "Error: Unable to chdir %s (error %d)\n", path, errno);
			exit(1);
		}
	
		main(argc, argv);
	}
	else
	{
	}
	
	flush_all();
	
	return __glue_exit_code;
}

int
dllglue_decode_file(char *infile)
{
	if(setjmp(_glue_buf) == 0)
	{
		initialize_all();

		__glue_exit_code = decode_file (infile);
	}
	else
	{
	}
	
	flush_all();
	
	return __glue_exit_code;
}

int
#if	!TARGET_API_MAC_CARBON
dllglue_setgetenv(UniversalProcPtr agetenv)
#else /* TARGET_API_MAC_CARBON */
dllglue_setgetenv(dllglue_getenv_func agetenv)
#endif /* TARGET_API_MAC_CARBON */
{
	if(__glue_getenv != 0L)
	{
#if	!TARGET_API_MAC_CARBON
		DisposeRoutineDescriptor(__glue_getenv);
#endif /* !TARGET_API_MAC_CARBON */
		__glue_getenv = 0L;
	}
	__glue_getenv = agetenv;
	return 0;
}

#if !TARGET_API_MAC_CARBON
int
dllglue_seteventdispatch(UniversalProcPtr aeventdispatch)
#else
int
dllglue_seteventdispatch(dllglue_eventdispath_func aeventdispatch)
#endif
{
#if !TARGET_API_MAC_CARBON
	if(__glue_eventdispatch != 0L)
	{
		DisposeRoutineDescriptor(__glue_eventdispatch);
		__glue_eventdispatch = 0L;
	}
#endif
	__glue_eventdispatch = aeventdispatch;
	return 0;
}

int
#if	!TARGET_API_MAC_CARBON
dllglue_setconsoleout(UniversalProcPtr aconsoleout)
#else /* TARGET_API_MAC_CARBON */
dllglue_setconsoleout(dllglue_consoleout_func aconsoleout)
#endif /* TARGET_API_MAC_CARBON */
{
	if(__glue_consoleout != 0L)
	{
#if	!TARGET_API_MAC_CARBON
		DisposeRoutineDescriptor(__glue_consoleout);
#endif /* !TARGET_API_MAC_CARBON */
		__glue_consoleout = 0L;
	}
	__glue_consoleout = aconsoleout;
	return 0;
}

int
#if	!TARGET_API_MAC_CARBON
dllglue_setconsoleerr(UniversalProcPtr aconsoleerr)
#else /* TARGET_API_MAC_CARBON */
dllglue_setconsoleerr(dllglue_consoleout_func aconsoleerr)
#endif /* TARGET_API_MAC_CARBON */
{
	if(__glue_consoleerr != 0L)
	{
#if	!TARGET_API_MAC_CARBON
		DisposeRoutineDescriptor(__glue_consoleerr);
#endif /* !TARGET_API_MAC_CARBON */
		__glue_consoleerr = 0L;
	}
	__glue_consoleerr = aconsoleerr;
	return 0;
}

int
#if	!TARGET_API_MAC_CARBON
dllglue_setconsolein(UniversalProcPtr aconsolein)
#else /* TARGET_API_MAC_CARBON */
dllglue_setconsolein(dllglue_consolein_func aconsolein)
#endif /* TARGET_API_MAC_CARBON */
{
	if(__glue_consolein != 0L)
	{
#if	!TARGET_API_MAC_CARBON
		DisposeRoutineDescriptor(__glue_consolein);
#endif /* !TARGET_API_MAC_CARBON */
		__glue_consolein = 0L;
	}
	__glue_consolein = aconsolein;
	return 0;
}

//
// Runtime intialization
//

// from Metrowerks runtime
extern pascal OSErr __initialize(CFragConnectionID connID);
extern pascal void __terminate(void);

// for cvs
pascal OSErr __cvs_initialize(const CFragInitBlock * theInitBlock);
pascal void __cvs_terminate(void);

FSSpec sCvsContainerFileSpec;

pascal OSErr __cvs_initialize(const CFragInitBlock * theInitBlock)
{
	OSErr result = noErr;

	if(CFragHasFileLocation(theInitBlock->fragLocator.where) &&
		theInitBlock->fragLocator.u.onDisk.fileSpec != 0L)
	{
		sCvsContainerFileSpec = *theInitBlock->fragLocator.u.onDisk.fileSpec;
	}
	garbyNew();

	result = __initialize(theInitBlock->connectionID);

	return result;
}

pascal void __cvs_terminate(void)
{
	__terminate();

	garbyRemove(false, false);
}
