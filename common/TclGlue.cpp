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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- May 1998
 * Modified : David Gravereaux <davygrvy@bigfoot.com> -- May 1999
 */

/*
 * TclGlue.cpp --- glue to the TCL language
 */
#include "stdafx.h"

#ifdef TARGET_OS_MAC
#	include "MacMisc.h"
#endif

#include <stdarg.h>
#include <string.h>
#include <algorithm>

#if !defined(USE_TCL)
#	define USE_TCL 1
#endif

#ifdef qMacCvsPP
#	include "MacCvsApp.h"
#endif /* qMacCvsPP */

#if qCarbon
#	include "TclGlue.mac.h"
#endif

using namespace std;

// this cannot work before the stub library is linked with wrong libraries
// and it causes WinCvs to crash
//#define USE_TCL_STUBS

#if !defined(__STDC__) && (defined(TARGET_OS_MAC) || defined(WIN32))
#	define __STDC__ 1
#	define UNDEF_STDC
#endif

#if USE_TCL
#include "tcl.h"

#if TCL_MAJOR_VERSION != 8 && TCL_MINOR_VERSION < 1
#	error "You need the Tcl 8.1.x header (or above) from Scriptics for this"
#endif
#endif

#ifdef UNDEF_STDC
#	undef __STDC__
#	define __STDC__ 0
#
#endif

#if 0//def WIN32
//  this comes with the Scriptic release, or
//  you can compile your own from generic/tclStubLib.c
//  in the source release of Tcl and build it with any
//  combination of the C runtime to match WinCVS, so you
//  don't get those:
// LINK : warning LNK4098: defaultlib "LIBCMTD" conflicts with use of other libs; use /NODEFAULTLIB:library
#pragma comment(lib,"tclstub81.lib")
#endif

#include "TclGlue.h"
#include "AppConsole.h"
#include "AppGlue.h"
#include "CvsArgs.h"
#include "CvsEntries.h"
#include "dll_loader.h"
#include "FileTraversal.h"
#include "MacrosSetup.h"
#include "MoveToTrash.h"
#include "CvsPrefs.h"
#include "CvsCommands.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "wincvsdebug.h"

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#if defined(_MSC_VER)
#	define DECLC extern "C"
#else
#	define DECLC
#endif

#if USE_TCL

#if defined(WIN32) || qCarbon
#	define DEC_GLUE(f) (*f##_glue)

	Tcl_Command	DEC_GLUE(Tcl_CreateCommand)
		_ANSI_ARGS_((Tcl_Interp *interp, const char *cmdName, Tcl_CmdProc *proc,
			ClientData clientData, Tcl_CmdDeleteProc *deleteProc));
	Tcl_Interp * DEC_GLUE(Tcl_CreateInterp) _ANSI_ARGS_((void));
	void DEC_GLUE(Tcl_AppendResult) _ANSI_ARGS_(TCL_VARARGS(Tcl_Interp *,interp));
	void DEC_GLUE(Tcl_SetResult) _ANSI_ARGS_((Tcl_Interp *interp, char *string, Tcl_FreeProc *freeProc));
	void DEC_GLUE(Tcl_DeleteInterp) _ANSI_ARGS_((Tcl_Interp *interp));
	int DEC_GLUE(Tcl_Eval) _ANSI_ARGS_((Tcl_Interp *interp, char *string));
	int DEC_GLUE(Tcl_EvalFile) _ANSI_ARGS_((Tcl_Interp *interp, char *fileName));
	char *		DEC_GLUE(Tcl_Alloc) _ANSI_ARGS_((unsigned int size));
	void		DEC_GLUE(Tcl_Free) _ANSI_ARGS_((char *ptr));
	char *		DEC_GLUE(Tcl_Realloc) _ANSI_ARGS_((char *ptr,
					unsigned int size));
	int		DEC_GLUE(Tcl_ExprLong) _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, long *ptr));
	char *		DEC_GLUE(Tcl_Concat) _ANSI_ARGS_((int argc, char *const *argv));
	char *		DEC_GLUE(Tcl_SetVar2) _ANSI_ARGS_((Tcl_Interp *interp,
			    const char *part1, const char *part2, const char *newValue,
			    int flags));
	void		DEC_GLUE(Tcl_DStringStartSublist) _ANSI_ARGS_((
			    Tcl_DString *dsPtr));
	void		DEC_GLUE(Tcl_DStringEndSublist) _ANSI_ARGS_((Tcl_DString *dsPtr));
	void		DEC_GLUE(Tcl_DStringFree) _ANSI_ARGS_((Tcl_DString *dsPtr));
	void		DEC_GLUE(Tcl_DStringInit) _ANSI_ARGS_((Tcl_DString *dsPtr));
	void		DEC_GLUE(Tcl_DStringResult) _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_DString *dsPtr));
	char *		DEC_GLUE(Tcl_DStringAppendElement) _ANSI_ARGS_((
			    Tcl_DString *dsPtr, CONST char *string));
	void		DEC_GLUE(TclFreeObj) _ANSI_ARGS_((Tcl_Obj *objPtr));
	void		DEC_GLUE(Tcl_SetObjResult) _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Obj *resultObjPtr));
	Tcl_Obj *	DEC_GLUE(Tcl_NewStringObj) _ANSI_ARGS_((CONST char *bytes,
			    int length));
	Tcl_Obj *	DEC_GLUE(Tcl_NewListObj) _ANSI_ARGS_((int objc,
			    Tcl_Obj *CONST objv[]));
	int		DEC_GLUE(Tcl_ListObjAppendElement) _ANSI_ARGS_((
			    Tcl_Interp *interp, Tcl_Obj *listPtr,
			    Tcl_Obj *objPtr));

	void		DEC_GLUE(Tcl_SourceRCFile) _ANSI_ARGS_((Tcl_Interp * interp));
	int		DEC_GLUE(Tcl_Init) _ANSI_ARGS_((Tcl_Interp * interp));
	char *		DEC_GLUE(Tcl_SetVar) _ANSI_ARGS_((Tcl_Interp * interp, 
				const char * varName, const char * newValue, int flags));
	void		DEC_GLUE(Tcl_FindExecutable) _ANSI_ARGS_((CONST char * argv0));

#	define GLUE_WRAP(f) f##_glue
#	define Tcl_CreateCommand GLUE_WRAP(Tcl_CreateCommand)
#	define Tcl_CreateInterp GLUE_WRAP(Tcl_CreateInterp)
#	define Tcl_AppendResult GLUE_WRAP(Tcl_AppendResult)
#	define Tcl_SetResult GLUE_WRAP(Tcl_SetResult)
#	define Tcl_DeleteInterp GLUE_WRAP(Tcl_DeleteInterp)
#	define Tcl_Eval GLUE_WRAP(Tcl_Eval)
#	define Tcl_EvalFile GLUE_WRAP(Tcl_EvalFile)
#	define Tcl_Alloc GLUE_WRAP(Tcl_Alloc)
#	define Tcl_Free GLUE_WRAP(Tcl_Free)
#	define Tcl_Realloc GLUE_WRAP(Tcl_Realloc)
#	define Tcl_ExprLong GLUE_WRAP(Tcl_ExprLong)
#	define Tcl_Concat GLUE_WRAP(Tcl_Concat)
#	define Tcl_SetVar2 GLUE_WRAP(Tcl_SetVar2)
#	define Tcl_DStringStartSublist GLUE_WRAP(Tcl_DStringStartSublist)
#	define Tcl_DStringEndSublist GLUE_WRAP(Tcl_DStringEndSublist)
#	define Tcl_DStringFree GLUE_WRAP(Tcl_DStringFree)
#	define Tcl_DStringInit GLUE_WRAP(Tcl_DStringInit)
#	define Tcl_DStringResult GLUE_WRAP(Tcl_DStringResult)
#	define Tcl_DStringAppendElement GLUE_WRAP(Tcl_DStringAppendElement)
#	define TclFreeObj GLUE_WRAP(TclFreeObj)
#	define Tcl_SetObjResult GLUE_WRAP(Tcl_SetObjResult)
#	define Tcl_NewStringObj GLUE_WRAP(Tcl_NewStringObj)
#	define Tcl_NewListObj GLUE_WRAP(Tcl_NewListObj)
#	define Tcl_ListObjAppendElement GLUE_WRAP(Tcl_ListObjAppendElement)
#	define Tcl_SourceRCFile GLUE_WRAP(Tcl_SourceRCFile)
#	define Tcl_Init GLUE_WRAP(Tcl_Init)
#	define Tcl_SetVar GLUE_WRAP(Tcl_SetVar)
#	define Tcl_FindExecutable GLUE_WRAP(Tcl_FindExecutable)
#endif /* WIN32 */

#if qCarbon
#	define _TCL_CALLBACK(f) ((Tcl_CmdProc *)(f))
#	define CompatLoadLibrary CarbonCompatLoadLibrary
#	define CompatCloseLibrary CarbonCompatCloseLibrary
#	define _CompatCallLibrary _CarbonCompatCallLibrary
#else
#	ifdef qUnix
#		define _TCL_CALLBACK(f) ((Tcl_CmdProc *)(f))
#		ifdef CONST84 // is defined at tcl8.4
#			define _TCL84_CONST CONST84
#		endif
#	else
#		define _TCL_CALLBACK(f) f
#	endif
#endif

#ifndef _TCL84_CONST
#	define _TCL84_CONST
#endif

extern "C"
{
	static int tclCvsProc (ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]);
	static int tclHelpProc (ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]);
	static int tclCvsOutProc (ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]);
	static int tclCvsErrProc (ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]);
	static int tclCvsBrowserProc (ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]);
	static int tclCvsEntriesProc (ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]);
	static void tclCvsEntriesDeleteProc(ClientData clientData);
#ifdef WIN32
	static int tclDirProc (ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]);
#endif /* WIN32 */

	static int tclCvsExitOnErrorProc(ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]);
	static int tclCvsLastErrorCodeProc(ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]);
	/* not used at the moment 
	   static Tcl_CmdProc tclSetDebugLevelProc(ClientData clientData, Tcl_Interp *interp, int argc, _TCL84_CONST char *argv[]); */
}

/// The tcl browser class entity
class CTCLBrowserEnt
{
public:
	CTCLBrowserEnt()
	{}

	CTCLBrowserEnt(EntnodeData* data)
		: node(data)
	{}

	CTCLBrowserEnt(const CTCLBrowserEnt& ent)
		: node(ent.node)
	{}

	~CTCLBrowserEnt()
	{}

	CTCLBrowserEnt& operator=(const CTCLBrowserEnt& ent)
	{
		node = ent.node;
		return *this;
	}

	bool IsSamePath(const char* pathstr) const
	{
		string path = pathstr;
		CTcl_Interp::Deunixfy(path);

		string file;
		node.Data()->GetFullPathName(file);
		CTcl_Interp::Deunixfy(file);

		return path.compare(file) == 0;
	}

	EntnodeData* GetData() const
	{
		return node.Data();
	}

	/// returns normalized path and file name (GetName() may return relative path)
	void GetNormalized(std::string& path, std::string& fname) const
	{
		string file;
		node.Data()->GetFullPathName(file);
		CTcl_Interp::Deunixfy(file);
		SplitPath(file.c_str(), path, fname);
	}
	
private:
	ENTNODE node;
};

static vector<CTCLBrowserEnt> sTclBrowser;

/// TCL console
class CTCLCvsConsole : public CCvsConsole
{
public:
	CTCLCvsConsole(Tcl_Interp* interp) : fInterp(interp)
	{
		fLen = 100;
		fAlloc = (char*)malloc((fLen + 1) * sizeof(char));
	}

	virtual ~CTCLCvsConsole()
	{
		if( fAlloc != 0L )
			free(fAlloc);
	}
	
	virtual long cvs_out(const char* txt, long len)
	{
		if( fAlloc == 0L )
			return 0;
		
		if( len > fLen )
		{
			fLen = len;
			fAlloc = (char*)realloc(fAlloc, (fLen + 1) * sizeof(char));
		}

		if( fAlloc == 0L )
			return 0;

		memcpy(fAlloc, txt, len * sizeof(char));
		fAlloc[len] = '\0';
		
		Tcl_AppendResult(fInterp, fAlloc, 0L);
		return len;
	}
	
	virtual long cvs_err(const char* txt, long len)
	{
		return cvs_out(txt, len);
	}
protected:
	Tcl_Interp* fInterp;
	int fLen;
	char* fAlloc;
};

DECLC static int tclCvsProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	CTcl_Interp* tcl_interp = (CTcl_Interp*) clientData;
	CvsArgs args;
	
	for(int i = 1; i < argc; i++)
	{
		args.add(argv[i]);
	}

	CTCLCvsConsole console(interp);
	int exitc = launchCVS(0L, args.Argc(), args.Argv(), &console);

	//
	// Set CvsLastErrorCode value. TCL script can query
	// this value using cvslasterrorcode command
	//
	tcl_interp->SetCvsLastErrorCode(exitc);

	//
	// If command succeeded then return always TCL_OK and 
	// proceed with script. If command failed (exitc != 0) 
	// then exit only if CvsExitOnError is TRUE.
	//
	if( exitc == 0 ) 
		return TCL_OK;
	else 
		return tcl_interp->GetCvsExitOnError() ? TCL_ERROR : TCL_OK;
}

DECLC static int tclHelpProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	Tcl_AppendResult(interp, "Help :\n", 0L);
	Tcl_AppendResult(interp, "   cvs  : the cvs tool.\n", 0L);
	Tcl_AppendResult(interp, "   help : this help.\n", 0L);
	Tcl_AppendResult(interp, "Others commands :\n", 0L);
	Tcl_AppendResult(interp, "   cd   : change directory\n", 0L);
	Tcl_AppendResult(interp, "   pwd  : print current directory\n", 0L);
	Tcl_AppendResult(interp, "   ls   : list the current directory\n", 0L);
	Tcl_AppendResult(interp, "   info ?  : misc. informations\n", 0L);
	Tcl_AppendResult(interp, "   info commands : all the TCL commands\n", 0L);

	return TCL_OK;
}

DECLC static int tclCvsOutProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	for(int i = 1; i < argc; i++)
	{
		cvs_outstr(argv[i], strlen(argv[i]));
	}

	return TCL_OK;
}

DECLC static int tclCvsErrProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	for(int i = 1; i < argc; i++)
	{
		cvs_errstr(argv[i], strlen(argv[i]));
	}

	return TCL_OK;
}

DECLC static int tclCvsBrowserProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	if( argc < 2 )
	{
		Tcl_AppendResult(interp, argv[0], " : Missing arguments\n", 0L);
		return TCL_ERROR;
	}

	vector<CTCLBrowserEnt> & browser = clientData != 0L ? *(vector<CTCLBrowserEnt>*)clientData : sTclBrowser;

	if( strcmp(argv[1], "get") == 0 )
	{
		if( argc != 2 )
		{
			char* str = Tcl_Concat(argc, argv);
			Tcl_AppendResult(interp, argv[0], " : Too many arguments : '", str, "'\n", 0L);
			Tcl_Free(str);

			return TCL_ERROR;
		}

		Tcl_Obj* list = Tcl_NewListObj(0, 0L);
		for(vector<CTCLBrowserEnt>::const_iterator i = browser.begin(); i != browser.end(); ++i)
		{
			const vector<CTCLBrowserEnt>::const_iterator entry = i;
			
			string path;
			entry->GetData()->GetFullPathName(path);
			CTcl_Interp::Unixfy(path);
			
			Tcl_Obj* obj = Tcl_NewStringObj(path.c_str(), path.length());
			Tcl_ListObjAppendElement(interp, list, obj);
			//TclFreeObj(obj);
		}

		Tcl_SetObjResult(interp, list);
		//TclFreeObj(list);
	}
	else if(strcmp(argv[1], "info") == 0)
	{
		if( argc != 4 )
		{
			char* str = Tcl_Concat(argc, argv);
			Tcl_AppendResult(interp, argv[0], " : Too many arguments : '", str, "'\n", 0L);
			Tcl_Free(str);
			return TCL_ERROR;
		}

		for(vector<CTCLBrowserEnt>::const_iterator i = browser.begin(); i != browser.end(); ++i)
		{
			const vector<CTCLBrowserEnt>::const_iterator entry = i;
			if( entry->IsSamePath(argv[2]) )
			{
				EntnodeData* data = entry->GetData();
				string path, fname;
				string value;

				entry->GetNormalized(path, fname);				
				
				const char* res = Tcl_SetVar2(interp, argv[3], "name", fname.c_str(), 0);
				if( res == 0L )
					goto err1;
				
				value = data->GetType() == ENT_FILE ? "file" : "folder";
				res = Tcl_SetVar2(interp, argv[3], "kind", value.c_str(), 0);
				if( res == 0L )
					goto err1;

				res = Tcl_SetVar2(interp, argv[3], "path", path.c_str(), 0);
				if( res == 0L )
					goto err1;

				value = (*data)[EntnodeData::kState];
				res = Tcl_SetVar2(interp, argv[3], "state", value.c_str(), 0);
				if( res == 0L )
					goto err1;

				value = data->IsMissing() ? "1" : "0";
				res = Tcl_SetVar2(interp, argv[3], "missing", value.c_str(), 0);
				if( res == 0L )
					goto err1;

				value = data->IsUnknown() ? "1" : "0";
				res = Tcl_SetVar2(interp, argv[3], "unknown", value.c_str(), 0);
				if( res == 0L )
					goto err1;

				value = data->IsIgnored() ? "1" : "0";
				res = Tcl_SetVar2(interp, argv[3], "ignored", value.c_str(), 0);
				if( res == 0L )
					goto err1;

				value = data->IsLocked() ? "1" : "0";
				res = Tcl_SetVar2(interp, argv[3], "locked", value.c_str(), 0);
				if( res == 0L )
					goto err1;

				value = data->IsUnmodified() ? "0" : "1";
				res = Tcl_SetVar2(interp, argv[3], "modified", value.c_str(), 0);
				if( res == 0L )
					goto err1;

				value = data->GetState();
				res = Tcl_SetVar2(interp, argv[3], "state", value.c_str(), 0);
				if( res == 0L )
					goto err1;

				if( data->GetType() == ENT_FILE )
				{
					value = (*data)[EntnodeFile::kVN];
					res = Tcl_SetVar2(interp, argv[3], "revision", value.c_str(), 0);
					if( res == 0L )
						goto err1;

					value = (*data)[EntnodeFile::kTS];
					res = Tcl_SetVar2(interp, argv[3], "timestamp", value.c_str(), 0);
					if( res == 0L )
						goto err1;

					value = (*data)[EntnodeFile::kOption];
					res = Tcl_SetVar2(interp, argv[3], "option", value.c_str(), 0);
					if( res == 0L )
						goto err1;

					value = (*data)[EntnodeFile::kEncoding];
					res = Tcl_SetVar2(interp, argv[3], "encoding", value.c_str(), 0);
					if( res == 0L )
						goto err1;
					
					value = (*data)[EntnodeFile::kTag];
					res = Tcl_SetVar2(interp, argv[3], "tag", value.c_str(), 0);
					if( res == 0L )
						goto err1;

					value = (*data)[EntnodeFile::kConflict];
					res = Tcl_SetVar2(interp, argv[3], "conflict", value.c_str(), 0);
					if( res == 0L )
						goto err1;
				}

				goto ok1;
err1:
				Tcl_AppendResult(interp, argv[0], " : Error while creating '", argv[3], "'\n", 0L);
				return TCL_ERROR;
ok1:			
				break;
			}
		}
	}
	else
	{
		char* str = Tcl_Concat(argc, argv);
		Tcl_AppendResult(interp, argv[0], " : Wrong arguments : '", str, "'\n", 0L);
		Tcl_Free(str);

		return TCL_ERROR;
	}

	return TCL_OK;
}

class CFillTclEntries : public TraversalReport
{
public:
	CSortList<ENTNODE>& m_entries;
	ignored_list_type m_ignlist;

	CFillTclEntries(CSortList<ENTNODE>& entries) :
		m_entries(entries) {}

	virtual ~CFillTclEntries() {}

	virtual kTraversal EnterDirectory(const char* fullpath, const char* dirname, const UFSSpec* macspec)
	{
		Entries_Open (m_entries, fullpath);
		BuildIgnoredList(m_ignlist, fullpath);
		
		return kContinueTraversal;
	}

	virtual kTraversal ExitDirectory(const char* fullpath)
	{
		m_ignlist.erase(m_ignlist.begin(), m_ignlist.end());
		
		return kContinueTraversal;
	}

	virtual kTraversal OnError(const char* err, int errcode)
	{
		return kTraversalError;
	}

	virtual kTraversal OnIdle(const char* fullpath)
	{
		return kContinueTraversal;
	}

	virtual kTraversal OnDirectory(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec)
	{
#if defined(WIN32) || defined(TARGET_OS_MAC)
		if( stricmp(name, "CVS") == 0 )
#else
		if( strcmp(name, "CVS") == 0 )
#endif
			return kSkipFile;

		/*EntnodeData *data = */Entries_SetVisited(fullpath, m_entries, name, dir, true, &m_ignlist);

		return kSkipFile;
	}

	virtual kTraversal OnAlias(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec)
	{
		return OnFile(fullpath, fullname, name, dir, macspec);
	}

	virtual kTraversal OnFile(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec)
	{
		/*EntnodeData *data = */Entries_SetVisited(fullpath, m_entries, name, dir, false, &m_ignlist);

		return kContinueTraversal;
	}
};


DECLC static void tclCvsEntriesDeleteProc(ClientData clientData)
{
	if( clientData != 0L )
		delete (vector<CTCLBrowserEnt>*)clientData;
}

DECLC static int tclCvsEntriesProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	if( argc != 3 )
	{
		Tcl_AppendResult(interp, argv[0], " : Wrong # of arguments\n", 0L);
		return TCL_ERROR;
	}

	CSortList<ENTNODE> entries(200, ENTNODE::Compare);

	// Refetch all items
	CFillTclEntries traverse(entries);
	string path(argv[1]);
	CTcl_Interp::Deunixfy(path);
	/*kTraversal res = */FileTraverse(path.c_str(), traverse);

	// add the missing files
	Entries_SetMissing(entries);

	// now make the TCL entries
	vector<CTCLBrowserEnt>* browser = new vector<CTCLBrowserEnt>;
	
	int numEntries = entries.NumOfElements();
	for(int i = 0; i < numEntries; i++)
	{
		const ENTNODE& theNode = entries.Get(i);
		EntnodeData* data = theNode.Data();
		browser->push_back(CTCLBrowserEnt(data));
	}

	Tcl_Command cmd = Tcl_CreateCommand (interp, argv[2], _TCL_CALLBACK(tclCvsBrowserProc),
		(ClientData)browser, tclCvsEntriesDeleteProc);
	
	return cmd != 0L ? TCL_OK : TCL_ERROR;
}

#ifdef WIN32
class LSReport : public TraversalReport
{
public:
	Tcl_Interp* fInterp;
	int fState;

	LSReport(Tcl_Interp* interp) : fInterp(interp), fState(TCL_OK) {}

	virtual kTraversal EnterDirectory(const char* fullpath, const char* dirname, const UFSSpec* macspec)
	{
		return kContinueTraversal;
	}

	virtual kTraversal ExitDirectory(const char* fullpath)
	{
		return kContinueTraversal;
	}

	virtual kTraversal OnError(const char* err, int errcode)
	{
		fState = TCL_ERROR;
		Tcl_AppendResult(fInterp, err, "\n", 0L);
		return kTraversalError;
	}

	virtual kTraversal OnIdle(const char* fullpath)
	{
		return kContinueTraversal;
	}

	virtual kTraversal OnAnyDevice(const char* fullpath,
		const char* fullname,
		const char* name,
		const struct stat& dir, const UFSSpec* macspec)
	{
		Tcl_AppendResult(fInterp, name, "\n", 0L);
		return kSkipFile;
	}
};

DECLC static int tclDirProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	LSReport report(interp);
	char path[256];
	getcwd(path, 256);
	kTraversal res = FileTraverse(path, report);
	return report.fState;
}
#endif /* WIN32 */

DECLC static int tclSafeRemove (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	if( argc < 2 )
	{
		Tcl_AppendResult(interp, argv[0], " : Missing list of files\n", 0L);
		return TCL_ERROR;
	}

	for(int i = 1; i < argc; i++)
	{
		string fn(argv[i]);
		CTcl_Interp::Deunixfy(fn);
		
		if( !CompatMoveToTrash(fn.c_str(), 0) )  // dir is not needed, TCL profides fully qualified file name
		{
			cvs_err("Unable to trash '%s'\n", fn.c_str());
		}
	}

	return TCL_OK;
}

DECLC static int tclCvsExitOnErrorProc(ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	CTcl_Interp* tcl_interp = (CTcl_Interp*)clientData;

	//
	// CvsExitOnError command can be used to define whether TCL macro
	// is automatically terminated if CVS command returned error (non zero).
	//
	// CvsExitOnError has following modes:
	//   false	= Do not exit automatically. CvsLastErrorCode contains the last error code.
	//	 true	= Exit TCL script in error (default).
	//
	if( argc < 2 )
	{
		Tcl_AppendResult(interp, argv[0], " : Missing argument (true|false)\n", 0L);
		return TCL_ERROR;
	}

	if( strcmp(argv[1], "true") == 0 )
		tcl_interp->SetCvsExitOnError(true);
	else if( strcmp(argv[1], "false") == 0 )
		tcl_interp->SetCvsExitOnError(false);
	else
	{
		char* str = Tcl_Concat(argc, argv);
		Tcl_AppendResult(interp, argv[0], " : Wrong argument : '", str, "'\n", 0L);
		Tcl_Free(str);

		return TCL_ERROR;
	}

	return TCL_OK;
}

DECLC static int tclCvsLastErrorCodeProc(ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	CTcl_Interp* tcl_interp = (CTcl_Interp*)clientData;

	if( argc < 2 )
	{
		char value[35];
		
		// CvsLastErrorCode called without arguments, so return current value
		sprintf(value, "%d", tcl_interp->GetCvsLastErrorCode());
		Tcl_AppendResult(interp, value, 0L);
	}
	else
	{
		// Set the value of LastErrorCode variable
		tcl_interp->SetCvsLastErrorCode(atoi(argv[1]));
	}
	
	return TCL_OK;
}

#if defined(WIN32)
#	if ENABLE_WINCVSDEBUG
DECLC static int tclGetFileModTimeProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
    struct wnt_stat sb;
    struct tm* p_utc = NULL;

	if( argc < 2 )
	{
		Tcl_AppendResult(interp, argv[0], " : Missing argument (int)\n", 0L);
		return TCL_ERROR;
	}

	if( argc > 2 )
	{
		char* str = Tcl_Concat(argc, argv);
		Tcl_AppendResult(interp, argv[0], " : Too many arguments : '", str, "'\n", 0L);
		Tcl_Free(str);
		return TCL_ERROR;
	}

    if( wnt_stat(argv[1], &sb) != 0 )
    {
		Tcl_AppendResult(interp, argv[0], " : Could not find file : '", argv[1], "'\n", 0L);
		return TCL_ERROR;
    }

    p_utc = gmtime(& sb.st_mtime);
    Tcl_AppendResult(interp, asctime(p_utc), 0L);
    return TCL_OK;
}

DECLC static int tclSetDebugMaskProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	if( argc < 2 )
	{
		Tcl_AppendResult(interp, argv[0], " : Missing argument (int)\n", 0L);
		return TCL_ERROR;
	}

	if( argc > 3 )
	{
		char* str = Tcl_Concat(argc, argv);
		Tcl_AppendResult(interp, argv[0], " : Too many arguments : '", str, "'\n", 0L);
		Tcl_Free(str);
		return TCL_ERROR;
	}

	char* endc = NULL;
	unsigned long n;
	bool b;
	
	n = strtoul(argv[1], &endc, 0);
	if( *endc )
	{
		Tcl_AppendResult(interp, argv[0], " : Could not understand argument '", argv[1], "'\n", 0L);
		return TCL_ERROR;
	}

	if( argc == 2 )
	{
		SetWinCvsDebugMask(n);
		return TCL_OK;
	}


	if( !stricmp(argv[2], "true") )
	{
		b = true;
	}
	else if( !stricmp(argv[2], "false") )
	{
		b = false;
	}
	else
	{
		b = strtoul(argv[2], &endc, 0) != 0;
		if( *endc )
		{
			Tcl_AppendResult(interp, argv[0], " : Could not understand argument '", argv[2], "'\n", 0L);
			return TCL_ERROR;
		}
	}

	SetWinCvsDebugMaskBit(n, b);

	return TCL_OK;
}

DECLC static int tclGetDebugMaskProc (ClientData clientData, Tcl_Interp* interp, int argc, _TCL84_CONST char* argv[])
{
	char retstr[32];
	if( argc < 2 )
	{
		_snprintf(retstr, 32, "%u", GetWinCvsDebugMask());
		Tcl_AppendResult(interp, retstr, 0L);
		return TCL_OK;
	}
	
	if( argc > 3 )
	{
		char* str = Tcl_Concat(argc, argv);
		Tcl_AppendResult(interp, argv[0], " : Too many arguments : '", str, "'\n", 0L);
		Tcl_Free(str);
		return TCL_ERROR;
	}

	char* endc = NULL;
	unsigned long n;
	
	n = strtoul(argv[1], & endc, 0);
	if( *endc ) 
	{
		Tcl_AppendResult(interp, argv[0], " : Could not understand argument '", argv[1], "'\n", 0L);
		return TCL_ERROR;
	}

	_snprintf(retstr, 32, "%u", GetWinCvsDebugMaskBit(n));
	Tcl_AppendResult(interp, retstr, 0L);

	return TCL_OK;
}
#	endif	// ENABLE_WINCVSDEBUG
#endif	// WIN32

#endif // USE_TCL

/*!
	Browser reset
*/
void TclBrowserReset(void)
{
#if USE_TCL
	sTclBrowser.erase(sTclBrowser.begin(), sTclBrowser.end());
#endif
}

/*!
	Browser filler
	\note although 'path' is not used here any longer, it makes sense to retain 
		that attribute since it gives you an option to figure out relative filename, 
		relative to 'path' that designates the folder in which the operation takes place
*/
void TclBrowserAppend(const char* /*path*/, EntnodeData* data)
{
#if USE_TCL
	sTclBrowser.push_back(CTCLBrowserEnt(data));
#endif
}

static Tcl_Interp* myTcl_CreateInterp(void)
{
#if USE_TCL
#	ifdef WIN32
	string path;
	::GetAppModule(path);
	Tcl_FindExecutable(path.c_str());
#	endif
#endif

#	ifdef qMacCvsPP
	CStr appPath = CMacCvsApp::GetAppPath();
	Tcl_FindExecutable(appPath);
#	endif

#	if USE_TCL
 	Tcl_Interp* res = Tcl_CreateInterp();
	return res;
#	else
	return NULL;
#	endif
}

CTcl_Interp::CTcl_Interp()
{
	fInterp = 0L;

	fExitOnError = true;
	fLastErrorCode = 0;
	
#if USE_TCL
	if( !CTcl_Interp::IsAvail() )
	{
		return;
	}
	
	fInterp = myTcl_CreateInterp();
	if( fInterp == 0L )
	{
		cvs_err("Unable to create a new TCL interpreter !\n");
		return;
	}

	Tcl_Command cmd;
	
	// this parameter is used to get access to CTcl_Interp::SetCvsLastErrorCode
	// in tclCvsProc method
	cmd = Tcl_CreateCommand (fInterp, "cvs", _TCL_CALLBACK(tclCvsProc),
		(ClientData)this, (Tcl_CmdDeleteProc *)0L);

	cmd = Tcl_CreateCommand (fInterp, "help", _TCL_CALLBACK(tclHelpProc),
		(ClientData)0L, (Tcl_CmdDeleteProc *)0L);
	cmd = Tcl_CreateCommand (fInterp, "cvsout", _TCL_CALLBACK(tclCvsOutProc),
		(ClientData)0L, (Tcl_CmdDeleteProc *)0L);
	cmd = Tcl_CreateCommand (fInterp, "cvserr", _TCL_CALLBACK(tclCvsErrProc),
		(ClientData)0L, (Tcl_CmdDeleteProc *)0L);
	cmd = Tcl_CreateCommand (fInterp, "cvsbrowser", _TCL_CALLBACK(tclCvsBrowserProc),
		(ClientData)0L, (Tcl_CmdDeleteProc *)0L);
	cmd = Tcl_CreateCommand (fInterp, "cvsentries", _TCL_CALLBACK(tclCvsEntriesProc),
		(ClientData)0L, (Tcl_CmdDeleteProc *)0L);
#ifdef WIN32
	cmd = Tcl_CreateCommand (fInterp, "ls", _TCL_CALLBACK(tclDirProc),
		(ClientData)0L, (Tcl_CmdDeleteProc *)0L);
	cmd = Tcl_CreateCommand (fInterp, "dir", _TCL_CALLBACK(tclDirProc),
		(ClientData)0L, (Tcl_CmdDeleteProc *)0L);
#endif /* WIN32 */
    cmd = Tcl_CreateCommand (fInterp, "trash", _TCL_CALLBACK(tclSafeRemove),
		(ClientData)0L, (Tcl_CmdDeleteProc *)0L);    

	cmd = Tcl_CreateCommand (fInterp, "cvsexitonerror", _TCL_CALLBACK(tclCvsExitOnErrorProc),
		(ClientData)this, (Tcl_CmdDeleteProc *)0L);

	cmd = Tcl_CreateCommand (fInterp, "cvslasterrorcode", _TCL_CALLBACK(tclCvsLastErrorCodeProc),
		(ClientData)this, (Tcl_CmdDeleteProc *)0L);
#if defined(WIN32)
#	if ENABLE_WINCVSDEBUG
	cmd = Tcl_CreateCommand (fInterp, "cvssetdebugmask", _TCL_CALLBACK(tclSetDebugMaskProc),
		(ClientData)this, (Tcl_CmdDeleteProc *)0L);
	cmd = Tcl_CreateCommand (fInterp, "cvsgetdebugmask", _TCL_CALLBACK(tclGetDebugMaskProc),
		(ClientData)this, (Tcl_CmdDeleteProc *)0L);
    cmd = Tcl_CreateCommand (fInterp, "cvsfilemodtime", _TCL_CALLBACK(tclGetFileModTimeProc),
		(ClientData)this, (Tcl_CmdDeleteProc *)0L);
#	endif	// ENABLE_WINCVSDEBUG
#endif	// defined(WIN32)

	string rcfile;
	MacrosGetLoc(rcfile);
	NormalizeFolderPath(rcfile);
	rcfile += "startup.tcl";

	Tcl_SetVar(fInterp, "tcl_rcFileName", rcfile.c_str(), TCL_GLOBAL_ONLY);
	Tcl_SetVar(fInterp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
	Tcl_Init(fInterp);
	Tcl_SourceRCFile(fInterp);
#endif
}

CTcl_Interp::~CTcl_Interp()
{
#if USE_TCL
	if( fInterp != 0L )
		Tcl_DeleteInterp(fInterp);
#endif
}

/*!
	Check whether TCL is available and load if not
	\param forceReload Force reload of Python library
	\return true if TCL is available, false otherwise
*/
bool CTcl_Interp::IsAvail(const bool forceReload /*= false*/)
{
#if USE_TCL
	static bool firstTime = true;
	static bool tclAvail = false;

	if( firstTime || forceReload )
	{
#	if defined(WIN32) || qCarbon
		static CompatConnectID connID = 0L;
		
		if( connID )
		{
			if( CompatCloseLibrary(&connID) )
			{
				connID = 0L;
				tclAvail = false;
			}
		}
#	endif

#	ifdef WIN32
		if( !tclAvail )
		{
			tclAvail = (gCvsPrefs.TclLibrary() && CompatLoadLibrary(&connID, gCvsPrefs.TclLibrary())) || 
				CompatLoadLibrary(&connID, "tcl87.dll") != 0 ||
				CompatLoadLibrary(&connID, "tcl86.dll") != 0 ||
				CompatLoadLibrary(&connID, "tcl85.dll") != 0 ||
				CompatLoadLibrary(&connID, "tcl84.dll") != 0 ||
				CompatLoadLibrary(&connID, "tcl83.dll") != 0 ||
				CompatLoadLibrary(&connID, "tcl82.dll") != 0 ||
				CompatLoadLibrary(&connID, "tcl81.dll") != 0;
		}
#	endif
	
#	if TARGET_RT_MAC_MACHO
		tclAvail = CompatLoadLibrary(&connID, "/usr/lib/libtcl.dylib");
#	endif

#	if defined(WIN32) || qCarbon
#	define LOAD_CODEFRAG(name) \
		if( tclAvail ) \
			tclAvail = (*(void**)&name##_glue = \
				_CompatCallLibrary(connID, #name)) != 0L
#	endif
#	if defined(WIN32) || TARGET_API_MAC_CARBON
		LOAD_CODEFRAG(Tcl_CreateCommand);
		LOAD_CODEFRAG(Tcl_CreateInterp);
		LOAD_CODEFRAG(Tcl_AppendResult);
		LOAD_CODEFRAG(Tcl_SetResult);
		LOAD_CODEFRAG(Tcl_DeleteInterp);
		LOAD_CODEFRAG(Tcl_Eval);
		LOAD_CODEFRAG(Tcl_EvalFile);
		LOAD_CODEFRAG(Tcl_Alloc);
		LOAD_CODEFRAG(Tcl_Free);
		LOAD_CODEFRAG(Tcl_Realloc);
		LOAD_CODEFRAG(Tcl_ExprLong);
		LOAD_CODEFRAG(Tcl_Concat);
		LOAD_CODEFRAG(Tcl_SetVar2);
		LOAD_CODEFRAG(Tcl_DStringStartSublist);
		LOAD_CODEFRAG(Tcl_DStringEndSublist);
		LOAD_CODEFRAG(Tcl_DStringFree);
		LOAD_CODEFRAG(Tcl_DStringInit);
		LOAD_CODEFRAG(Tcl_DStringResult);
		LOAD_CODEFRAG(Tcl_DStringAppendElement);
		LOAD_CODEFRAG(TclFreeObj);
		LOAD_CODEFRAG(Tcl_SetObjResult);
		LOAD_CODEFRAG(Tcl_NewStringObj);
		LOAD_CODEFRAG(Tcl_NewListObj);
		LOAD_CODEFRAG(Tcl_ListObjAppendElement);
		LOAD_CODEFRAG(Tcl_SourceRCFile);
		LOAD_CODEFRAG(Tcl_Init);
		LOAD_CODEFRAG(Tcl_SetVar);
		LOAD_CODEFRAG(Tcl_FindExecutable);

		if( !tclAvail )
			return false;
#	endif /* WIN32 */

		Tcl_Interp* interp = myTcl_CreateInterp();
		tclAvail = interp != 0L;
		if( !tclAvail )
			return false;

		Tcl_Init(interp);

		/* Destroy the interp we needed just for this */
		Tcl_DeleteInterp(interp);

		firstTime = false;
	}

	return tclAvail;
#else
	return false;
#endif
}

/*!
	Execute a TCL script
	\return true on success, false otherwise
*/
kTclRes CTcl_Interp::DoScript(const char* script)
{
#if USE_TCL
	bool bTclFileStarted;

	if( fInterp == 0L )
		return false;
	
	// If script cmd begins with "source " then this
	// must be TCL file execution. 
	bTclFileStarted = (strncmp(script, "source ", 7) == 0);
	if( bTclFileStarted )
		gCvsPrefs.SetTclFileRunning(true);

	int exitc = Tcl_Eval(fInterp, (char*)script);

	if( bTclFileStarted )
		gCvsPrefs.SetTclFileRunning(false);

	size_t len = strlen(fInterp->result);
	if( exitc == TCL_ERROR )
	{
		cvs_errstr(fInterp->result, len);
		if( len != 0 && fInterp->result[len - 1] != '\n' )
			cvs_errstr("\n", 1);
	}
	else
	{
		cvs_outstr(fInterp->result, len);
		if( len != 0 && fInterp->result[len - 1] != '\n' )
			cvs_outstr("\n", 1);
	}
	
	return exitc;
#else
	return false;
#endif
}

/*!
	Execute a TCL script
	\return true on success, false otherwise
*/
kTclRes CTcl_Interp::DoScriptVar(const char* format, ...)
{
#if USE_TCL
	if( fInterp == 0L )
		return false;
	
	va_list args;
	char script[1024] = {'\0'};

	va_start(args, format);
	vsprintf(script, format, args);
	va_end(args);
	
	return DoScript(script);
#else
	return false;
#endif
}

/*!
	Execute a TCL script
	\return true on success, false otherwise
*/
kTclRes CTcl_Interp::DoFile(const char* file)
{
#if USE_TCL
	if( fInterp == 0L )
		return false;
	
	int exitc = Tcl_EvalFile(fInterp, (char*)file);
	size_t len = strlen(fInterp->result);
	if( exitc == TCL_ERROR )
	{
		cvs_errstr(fInterp->result, len);
		if( len != 0 && fInterp->result[len - 1] != '\n' )
			cvs_errstr("\n", 1);
	}
	else
	{
		cvs_outstr(fInterp->result, len);
		if( len != 0 && fInterp->result[len - 1] != '\n' )
			cvs_outstr("\n", 1);
	}
	
	return exitc;
#else
	return false;
#endif
}

/*!
	Normalize the path separator to use kTCLPathDelimiter
	\param path Return the normalized path
*/
void CTcl_Interp::Unixfy(std::string& path)
{
	if( kTCLPathDelimiter != kPathDelimiter )
	{
		replace(path.begin(), path.end(), kPathDelimiter, kTCLPathDelimiter); 
	}

	TrimRight(path, kTCLPathDelimiter);
}

/*!
	Normalize the path separator to use kPathDelimiter
	\param path Return the normalized path
*/
void CTcl_Interp::Deunixfy(std::string& path)
{
	if( kTCLPathDelimiter != kPathDelimiter )
	{
		replace(path.begin(), path.end(), kTCLPathDelimiter, kPathDelimiter);
	}
}
