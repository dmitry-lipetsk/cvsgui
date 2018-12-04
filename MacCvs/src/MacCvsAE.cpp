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
 * cvs_app_ae.c
 * UNIX environment handling stuff for macos
 *
 * These routines make MacCVS able to accept a standard UNIX command line,
 * environment, output redirection, and startup directory via AppleEvents
 * and redirect all output to AppleEvents, a file, or the SIOUX tty window.
 *
 * Michael Ladwig <mike@twinpeaks.prc.com> --- April 1996
 * Modified by Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */
 
#include <AppleEvents.h>
#include <AERegistry.h>

#if !TARGET_API_MAC_OSX
#include <EPPC.h>
#endif

#include <Sound.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MacCvsAE.h"
#include "MacCvsApp.h"
#include "MacCvsConstant.h"
#include "AppGlue.h"
#include "CvsArgs.h"
#include "FileTraversal.h"

#include <LCFURL.h>
#include <LCFString.h>

#define ArgMax					512	// Maximum number of Args passed via AE command
#define EnvMax					512	// Maximum number of Env Vars passed via AE command

enum { outToAE, outToFile, outToConsole };

static int		outputMode;
static FILE	*	outputFile;
static int		noLineBuffer; // unused

static Boolean gAEStarted = false;

PendingAEQueue	gPendingAEQueue;

static char * CopyInfo(AEDesc *desc)
{
#if !ACCESSOR_CALLS_ARE_FUNCTIONS
	Handle info = (Handle)desc->dataHandle;
	if(info)
	{
		::HLock (info);
		char * retarg = (char *)malloc(GetHandleSize(info) + 1);
	
		if (retarg)
		{
			memcpy(retarg, *info, GetHandleSize(info));
			retarg[GetHandleSize(info)] = 0;
		}
		::HUnlock (info);
			
		return retarg;
	}
	else
		return nil;
#else
	Size s = ::AEGetDescDataSize(desc);
	char * retarg = (char *)malloc(s + 1);
	if(retarg)
	{
		OSErr e = ::AEGetDescData(desc, retarg, s);
		if(e == noErr)
			retarg[s] = 0;
		else
		{
			free(retarg);
			retarg = 0L;
		}
		return retarg;
	}
	return 0L;
#endif
}

#pragma mark -
#pragma mark EnvTable

//	Request Parameters
const	DescType	keyEnvironment 		= 'ENVT';
const	DescType	keyWorkingDir 		= 'SPWD';

//	Reply Parameters
const	DescType	keyCVSStatus		= 'stat';
const	DescType	keyCVSDiagnostic	= 'diag';

class EnvTable {
	
		static	EnvTable*	sSingleton;
		
		int					mEnvC;
		char**				mEnvVars;
		char**				mEnvVals;
	
	public:
	
		static	EnvTable*	GetEnvTable (void) {return sSingleton;};
							
							EnvTable	(const AppleEvent&	inEvent);
							~EnvTable	(void);
							
		const	char*		GetEnv		(const	char*		var) const;
		
	};

EnvTable*
EnvTable::sSingleton = 0;

// ---------------------------------------------------------------------------
//	* EnvTable
// ---------------------------------------------------------------------------

EnvTable::EnvTable (

	const AppleEvent&	inEvent)
	
	: mEnvC (0)
	, mEnvVars ((char**) ::malloc (EnvMax * sizeof (*mEnvVars)))
	, mEnvVals ((char**) ::malloc (EnvMax * sizeof (*mEnvVals)))
	
	{ // begin EnvTable

		mEnvVars[mEnvC] = 0;
		mEnvVals[mEnvC] = 0;

		StAEDescriptor	args;
		long			argCount;
		
		if (noErr != ::AEGetParamDesc (&inEvent, keyEnvironment, typeAEList, args)) return;
		if (noErr != ::AECountItems (args, &argCount)) return;
		if (0 == argCount) return;
		
		OSErr			e = noErr;
		for (long i = 1; i < argCount; ++i) {
			AEKeyword	keyword;

			//	Move to next slot
			++mEnvC;
			mEnvVars[mEnvC] = nil;
			mEnvVals[mEnvC] = nil;

			// Variable
			StAEDescriptor	variable;
			e = ::AEGetNthDesc (args, i, typeChar, &keyword, variable);
			ThrowIfOSErr_(e);
			mEnvVars[mEnvC-1] = CopyInfo (variable);
			
			// Value
			StAEDescriptor	value;
			++i;
			e = ::AEGetNthDesc (args, i, typeChar, &keyword, value);
			ThrowIfOSErr_(e);
			mEnvVals[mEnvC-1] = CopyInfo (value);
			} // for
		
		sSingleton = this;
		
	} // end EnvTable

// ---------------------------------------------------------------------------
//	* ~EnvTable
// ---------------------------------------------------------------------------

EnvTable::~EnvTable (void)

	{ // begin ~EnvTable
	
		for (int i = 0; i < mEnvC; ++i) { 
			::free (mEnvVars[i]);
			mEnvVars[i] = 0;
			 
			::free (mEnvVals[i]);
			mEnvVals[i] = 0;
			} // for
			
		::free (mEnvVars);
		mEnvVars = 0;
		
		::free (mEnvVals);	
		mEnvVals = 0;
		
		mEnvC = 0;
		
		if (sSingleton == this) sSingleton = 0;
		
	} // end ~EnvTable
	
// ---------------------------------------------------------------------------
//	* GetEnv
// ---------------------------------------------------------------------------

const char*
EnvTable::GetEnv (

	const	char*	var) const

	{ // begin GetEnv

		// Look it up in the environment
		for (int i = 0; i < mEnvC; ++i) {
			if (0 == ::strcmp (mEnvVars[i], var))
				return mEnvVals[i];
			} // for
		
		return (char*) -1;
	
	} // end GetEnv

#pragma mark -
#pragma mark AECvsConsole

class AECvsConsole : public CCvsConsole {
		
	public:

		StHandleBlock	mStdout;
		StHandleBlock	mStderr;
		
						AECvsConsole	(void);
		virtual			~AECvsConsole	(void);
		
		virtual	long	cvs_out			(const char*	inConstChars,
										 long	inLen);
		virtual	long	cvs_err			(const char*	inConstChars,
										 long	inLen);
	};
	
// ---------------------------------------------------------------------------
//	* AECvsConsole
// ---------------------------------------------------------------------------

AECvsConsole::AECvsConsole (void)
	
	: mStdout ((Size) 0)
	, mStderr ((Size) 0)
	
	{ // begin AECvsConsole
	
	} // end AECvsConsole
	
// ---------------------------------------------------------------------------
//	* ~AECvsConsole
// ---------------------------------------------------------------------------

AECvsConsole::~AECvsConsole (void)

	{ // begin ~AECvsConsole
	
	} // end ~AECvsConsole
	
// ---------------------------------------------------------------------------
//	* cvs_out
// ---------------------------------------------------------------------------

long
AECvsConsole::cvs_out (

	const char*	inConstChars,
	long	inLen)
	
	{ // begin cvs_out
		
		::PtrAndHand (inConstChars, mStdout, inLen);
		
		return inLen;
		
	} // end cvs_out
	
// ---------------------------------------------------------------------------
//	* cvs_err
// ---------------------------------------------------------------------------

long
AECvsConsole::cvs_err (

	const char*	inConstChars,
	long	inLen)
	
	{ // begin cvs_err
		
		::PtrAndHand (inConstChars, mStderr, inLen);
		
		return inLen;
		
	} // end cvs_err

static void ConvertToNativeFS(char *path)
{
	if(strchr(path, ':'))
	{
		LCFString hfsname(path);
		LCFURL url(::CFURLCreateWithFileSystemPath(NULL, hfsname.GetTypeRef(), kCFURLHFSPathStyle, false));
		LCFString posix(::CFURLCopyFileSystemPath(url.GetTypeRef(), kCFURLPOSIXPathStyle));
		posix.GetCString(path, MAX_PATH, kCFStringEncodingUTF8);	
	}
}

#pragma mark -
static /*pascal*/ OSErr DoScript(const AppleEvent *event, AppleEvent *reply, long refCon)
{
	// Allocate some storage for the command line and the environment
	CvsArgs 	Args;
	EnvTable	env ();
	
	// Defaults
	outputMode = outToConsole;
	
	OSType		mode;
	DescType	typeCode;
	Size		size;
	AEDesc		args;
	long		i, argCount;
	char		*argString;
	char		startPWD[MAX_PATH] = ".";

#if !qCarbon
	long addrSize;
	TargetID	requestAddr;
	DescType	requestAddrType;
	AEAddressDesc	responseAddress;
#endif

	if (AEGetParamDesc(event, '----', typeAEList, &args) || AECountItems(&args, &argCount) || !argCount) 
		return errAEEventFailed;

	// Get the address of the requesting app to we can send information back
#if !qCarbon
	AEGetAttributePtr(event,
		keyAddressAttr,
		typeWildCard, 
		&requestAddrType,
		(Ptr) &requestAddr,
		sizeof(requestAddr),
		&addrSize);
#endif

	// Pull the command line from the event
	
	for (i = 0; i++<argCount;)
	{
		AEKeyword	key;
		AEDesc	arg;

		if (!AEGetNthDesc(&args, i, typeChar, &key, &arg))
		{
			Args.add (argString = CopyInfo(&arg));
			AEDisposeDesc(&arg);
			free (argString);
		}
	}
	AEDisposeDesc(&args);

	// Pull the environment variables from the event
	EnvTable 	envTable (*event);
	
	// Figure out what mode should be used to return results
	char tempOutputFileName[256], outputFileName[256];
	if (AEGetParamPtr(event, 'MODE', typeEnumerated, &typeCode, &mode, 4, &size))
		outputMode = outToConsole;
	else
	{
		switch (mode) {
			
			// Batch (return results via Apple Events)
			case 'TOAE':
				outputMode = outToAE;
				break;
			
			// File (return results via a file)		
			case 'FILE':
				outputMode = outToFile;
				if (AEGetParamPtr(event, 'FILE', typeChar, &typeCode, &outputFileName, sizeof(outputFileName)-1, &size))
				{
					outputMode = outToConsole;
					fprintf(stderr, "MacCVS Error: No filename parameter\n");
				}
				else
				{
					outputFileName[size] = 0;
					strcpy(tempOutputFileName, outputFileName);
					strcat(tempOutputFileName, ".TMP");
					ConvertToNativeFS(outputFileName);
					ConvertToNativeFS(tempOutputFileName);
					if((outputFile = fopen(tempOutputFileName, "w")) == NULL)
					{
						outputMode = outToConsole;
						fprintf(stderr, "MacCVS Error: Unable to open '%s'\n", tempOutputFileName);
					}
				}
				break;
		}
	}
	
    if (outputMode == outToAE)
    {
		//AECreateDesc(typeTargetID, &requestAddr, sizeof(requestAddr), &responseAddress);
	    //AECreateAppleEvent ('MCVS', 'DATA',
		//		&responseAddress,
		//		kAutoGenerateReturnID,
		//		kAnyTransactionID, 
		//		&gResponseEvent);
	}

	// Check to see if there is a starting pathname for this invokation
	if (! AEGetParamPtr(event, 'SPWD', typeChar, &typeCode, &startPWD, sizeof(startPWD)-1, &size))
	{
		startPWD[size] = 0;
		ConvertToNativeFS(startPWD);
		LoadPersistentSettings(startPWD, true);
	}
	
	// Check to see if we should not line buffer results in AE return mode
	Boolean flag;
	if (AEGetParamPtr(event, 'LBUF', typeBoolean, &typeCode, (Ptr) &flag, 1, &size))
		noLineBuffer = 0;
	else
		noLineBuffer = flag;

	gAEStarted = true;
	
	// call here !
	AECvsConsole	console;
	if(outputMode == outToConsole)
		launchCVS(startPWD, Args.Argc(), Args.Argv());
	else
		launchCVS(startPWD, Args.Argc(), Args.Argv(), &console);
	
	Size			outSize = ::GetHandleSize (console.mStdout);
	Size			diagSize = ::GetHandleSize (console.mStderr);
	
	if(outSize)
	{
		::PtrAndHand ("\0", console.mStdout, 1);
		::HLock (console.mStdout);
		if(outputMode == outToAE)
			::AEPutParamPtr (reply, keyDirectObject, typeChar, *console.mStdout, outSize);
		else
			fputs(*console.mStdout, outputFile);
		::HUnlock (console.mStdout);
	}
	
	if (diagSize)
	{
		::PtrAndHand ("\0", console.mStderr, 1);
		::HLock (console.mStderr);
		if(outputMode == outToAE)
			::AEPutParamPtr (reply, keyCVSDiagnostic, typeChar, *console.mStderr, diagSize);
		else
			fputs(*console.mStderr, outputFile);
		::HUnlock (console.mStderr);
	}
	
	::AEPutParamPtr(reply, 'DONE', typeChar, "DONE", 4);
	
	// clean up
	
	if(outputMode == outToFile)
	{
		fclose(outputFile);
		if(rename(tempOutputFileName, outputFileName) != 0)
			SysBeep(100);
	}
	else if(outputMode == outToAE)
	{
//		SendOutBuffer(true);
	
//		AEDisposeDesc(&gResponseEvent);
//#if !qCarbon
//		AEDisposeDesc(&responseAddress);
//#endif
	}

	// All Done
	gAEStarted = false;

	return noErr;
}

static OSErr 
DoAECommandLine (

	const AppleEvent*	event, 
	AppleEvent*			reply, 
	long 				refCon)

	{ // begin DoAECommandLine
		
		OSErr			e = noErr;
		DescType		typeCode;
		
		// Pull the command line from the event
		StAEDescriptor	argsDesc;
		long			argCount = 0;
		if ((noErr != ::AEGetParamDesc (event, keyDirectObject, typeAEList, argsDesc)) || 
			(noErr != ::AECountItems (argsDesc, &argCount)) || 
			(0 == argCount))
			return errAEEventFailed;
		
		//	Break out the arguments (all null-terminated)
		CvsArgs		args;
		//args.add ("cvs");
		for (long i = 1; i <= argCount; ++i) {
			AEKeyword			key;
			StAEDescriptor		argDesc;
		
			if (noErr != (e = ::AEGetNthDesc (argsDesc, i, typeChar, &key, argDesc)))
				return e;
			char *tmp = CopyInfo (argDesc);
			if(tmp)
			{
				args.add (tmp);
				free(tmp);
			}
			} // for

		// Pull the environment variables from the event
		EnvTable	envTable (*event);
		
		// Check to see if there is a starting pathname for this invokation
		Size	startPWDSize = 0;
		char	startPWD[MAX_PATH] = ".";
		if (noErr == ::AEGetParamPtr (event, keyWorkingDir, typeChar, &typeCode, &startPWD, sizeof (startPWD)-1, &startPWDSize))
		{
			startPWD[startPWDSize] = 0;
			ConvertToNativeFS(startPWD);
			LoadPersistentSettings(startPWD, true);
		}
	
		gAEStarted = true;
		
		// call here !
		AECvsConsole	console;
		long			status = launchCVS (startPWD, args.Argc (), args.Argv (), &console);
		
		// Stuff return codes
		char			null = 0;
		
		Size			outSize = ::GetHandleSize (console.mStdout);
		Size			diagSize = ::GetHandleSize (console.mStderr);
		
		::HLock (console.mStdout);
		::AEPutParamPtr (reply, typeWildCard, typeChar, *console.mStdout, outSize);
		
		if (diagSize) {
			::HLock (console.mStderr);
			::AEPutParamPtr (reply, keyCVSDiagnostic, typeChar, *console.mStderr, diagSize);
			} // if
			
		::AEPutParamPtr (reply, keyCVSStatus, typeLongInteger, &status, sizeof (status));

		// All Done
		gAEStarted = false;

		return noErr;
	
	} // end DoAECommandLine

char *ae_getenv(const char *var)
{
	if(!gAEStarted || EnvTable::GetEnvTable () == 0)
		return (char *) -1;
	
	// Look it up in the environment
	return (char*) EnvTable::GetEnvTable ()->GetEnv (var);
}

static OSStatus DoOpenSingleDoc(const AEDesc& inFileDesc)
{
  OSStatus          err(noErr);
  FSRef             fileRef;
  
  if ( inFileDesc.descriptorType != typeFSRef )
  {  
    StAEDescriptor    fileRefDesc;
    err = AECoerceDesc(&inFileDesc, typeFSRef, &fileRefDesc.mDesc);
    if ( err ) return err;
    err = AEGetDescData(fileRefDesc, &fileRef, sizeof(fileRef));
    if ( err ) return err;
  }
  else err = AEGetDescData(&inFileDesc, &fileRef, sizeof(fileRef));
  
  CMacCvsApp::gApp->NewBrowser(fileRef);
  
	ProcessSerialNumber currentProcess = { 0, kCurrentProcess };
	SetFrontProcessWithOptions(&currentProcess, kSetFrontProcessFrontWindowOnly);

  return err;
}

static OSStatus DoOpenDoc(const AppleEvent& inEvent, AppleEvent& outReply)
{ 
  StAEDescriptor    directParam;
  OSStatus          err;
  
  err = AEGetParamDesc(&inEvent, keyDirectObject, typeWildCard, &directParam.mDesc);
  if ( err ) return err;
  if ( directParam.DescriptorType() == typeAEList )
  {
    long    itemCount;
    err = AECountItems(directParam, &itemCount);
    if ( err ) return err;
    for ( long i(1); i <= itemCount; ++i )
    {
      StAEDescriptor    nthDesc;
      err = AEGetNthDesc(directParam, i, typeWildCard, NULL, &nthDesc.mDesc);
      if ( err ) return err;
      err = DoOpenSingleDoc(nthDesc);
      if ( err ) return err;
    }
  }
  else err = DoOpenSingleDoc(directParam);
  
  return err;
}

const long LMacCVSModelDirector::ae_MacCvsDoCommandLine = 12789;
const long LMacCVSModelDirector::ae_ShowPreferences = 1007;

LMacCVSModelDirector::LMacCVSModelDirector(LModelObject *inDefaultModel) : LModelDirector(inDefaultModel)
{
}

void LMacCVSModelDirector::HandleAppleEvent(
								const AppleEvent	&inAppleEvent,
								AppleEvent			&outReply,
#if __PowerPlant__ >= 0x02114002
								AEHandlerRefConT	inRefCon
#elif __PowerPlant__ >= 0x02100000
								UInt32				inRefCon
#else
								SInt32				inRefCon
#endif
								)
{
	if(CMacCvsApp::gApp->IsCvsRunning() || gAEStarted)
	{
		// Use this to ensure that this routine is not reentered, since cvs library is not reentrant	
		// If we get an event while already handling one, we suspend it and return. it will be resumed from
		// the main event loop.
		switch (inRefCon) {
			case ae_DoScript:
			case ae_MacCvsDoCommandLine:
				gPendingAEQueue.EnqueueEvent (inAppleEvent, outReply);
				AESuspendTheCurrentEvent (&inAppleEvent);
				return;
			} // switch
		
		// no way if we are in cvs !
		Throw_(errAEWaitCanceled);
	}
	
	switch (inRefCon) {
	  case ae_OpenDoc:
	    ThrowIfOSErr_(DoOpenDoc(inAppleEvent, outReply));
	    break;
	    
		case ae_DoScript:
			// obsolete
			ThrowIfOSErr_(DoScript (&inAppleEvent, &outReply, inRefCon));
			break;

		case ae_MacCvsDoCommandLine:
			ThrowIfOSErr_(DoAECommandLine (&inAppleEvent, &outReply, inRefCon));
			break;
			
		case ae_ShowPreferences:
			{
			Boolean	outEnabled;
			Boolean	outUsesMark;
			UInt16	outMark;
			Str255	outName;
			LCommander::GetTarget ()->ProcessCommandStatus (cmd_MacCvsPrefs, outEnabled, outUsesMark, outMark, outName);
			if (!outEnabled) ThrowOSErr_(errAEEventNotHandled);
			
		
#if PP_Uses_Aqua_MenuBar
		if (UEnvironment::HasFeature(env_HasAquaTheme))
			DisableMenuCommand (nil, kHICommandPreferences);
#endif
			LCommander::GetTarget ()->ProcessCommand (cmd_MacCvsPrefs, 0);
			}
			break;
			
		default:
			LModelDirector::HandleAppleEvent(inAppleEvent, outReply, inRefCon);
			break;
		} // switch

}
