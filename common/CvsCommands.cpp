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
 * CvsCommands.cpp --- set of CVS commands
 */

#include "stdafx.h"

#include <errno.h>

#ifdef WIN32
#	include <process.h>
#	include "wincvs.h"

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#include "CvsArgs.h"
#include "CvsCommands.h"
#include "MultiFiles.h"
#include "PromptFiles.h"
#include "AppConsole.h"
#include "AppGlue.h"
#include "MoveToTrash.h"
#include "LineCmd.h"
#include "CvsAlert.h"
#include "CommitDlg.h"
#include "ImportDlg.h"
#include "CheckoutDlg.h"
#include "TclGlue.h"
#include "ImportFilterDlg.h"
#include "UpdateDlg.h"
#include "LogDlg.h"
#include "DiffDlg.h"
#include "LogParse.h"
#include "DiffParse.h"
#include "UpdateToTempParse.h"
#include "RtagDlg.h"
#include "TagDlg.h"
#include "InitDlg.h"
#include "ReleaseDlg.h"
#include "AnnotateDlg.h"
#include "StatusDlg.h"
#include "LoginDlg.h"
#include "BrowseViewHandlers.h"
#include "LaunchHandlers.h"
#include "FileActionDlg.h"
#include "Authen.h"
#include "RemoveDlg.h"
#include <sstream>
#include <list>

#ifdef qMacCvsPP
#	include "AppGlue.mac.h"
#	include "MacMisc.h"
#	include "MacCvsApp.h"
#	include "GUSIFileSpec.h"
#	include <LaunchServices.h>
#endif

#ifdef qUnix
#	include "UCvsApp.h"
#endif

using namespace std;

/*!
	Get the edit type name
	\param editType Edit type
	\return The edit type name
*/
const char* GetEditTypeName(const kEditCmdType editType)
{
	const char* res = _i18n("Unknown");

	switch( editType )
	{
	case kEditNormal:
		res = _i18n("Edit");
		break;
	case kEditReserved:
		res = _i18n("Reserved Edit");
		break;
	case kEditForce:
		res = _i18n("Force Edit");
		break;
	default:
		break;
	}

	return res;
}

/*!
	Get the tag type name
	\param tagType Edit type
	\return The tag type name
*/
const char* GetTagTypeName(const kTagCmdType tagType)
{
	const char* res = _i18n("Unknown");

	switch( tagType )
	{
	case kTagCreate:
		res = _i18n("Create Tag");
		break;
	case kTagDelete:
		res = _i18n("Delete Tag");
		break;
	case kTagBranch:
		res = _i18n("Branch");
		break;
	default:
		break;
	}

	return res;
}

/*!
	Get the tag type name
	\param tagType Edit type
	\return The tag type name
*/
const char* GetRtagTypeName(const kTagCmdType tagType)
{
	const char* res = _i18n("Unknown");

	switch( tagType )
	{
	case kTagCreate:
		res = _i18n("Rtag Create");
		break;
	case kTagDelete:
		res = _i18n("Rtag Delete");
		break;
	case kTagBranch:
		res = _i18n("RTag Branch");
		break;
	default:
		break;
	}

	return res;
}

/*!
	Verify the selection handler is properly setup to carry on launching command
	\param handler Command handler to be tested
	\param mf Returned selection as MultiFiles
	\param forceNormalize specifies whether to force normalization of the MultiFiles or rely on the selection handler's requirements
	\return true if handler is properly setup, false otherwise
*/
static bool CheckSelectionHandler(KiSelectionHandler& handler, MultiFiles*& mf, bool forceNormalize=false)
{
#if INTERNAL_AUTHEN
	if( gCvsPrefs.empty() )
	{
		return false;
	}
#endif /* INTERNAL_AUTHEN */

	mf = handler.GetSelection();
	if( !mf && handler.GetNeedSelection() )
	{
		return false;
	}

	return ( forceNormalize || handler.NeedsNormalize() ) ? mf->Normalize() : true;
}

/*!
	Execute <b>cvs init</b> command
*/
void CvsCmdInit()
{
#if INTERNAL_AUTHEN
	if( gCvsPrefs.empty() )
		return;
#endif /* INTERNAL_AUTHEN */
	
	bool forceCvsroot;
	string cvsroot;

	if( !CompatGetInit(forceCvsroot, cvsroot) )
		return;

	CvsArgs args;

	if( forceCvsroot )
	{
#if INTERNAL_AUTHEN
		args.addcvsroot();
#else
		args.add("-d");
		args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
	}

	args.add("init");
	
	args.print();
	launchCVS(0L, args.Argc(), args.Argv());
}

/*!
	Execute <b>cvs login</b> command
*/
void CvsCmdLogin(void)
{
#if INTERNAL_AUTHEN
	if( gCvsPrefs.empty() )
		return;
#endif /* INTERNAL_AUTHEN */
	
	bool forceCvsroot;
	string cvsroot;

	if( !CompatGetLogin(forceCvsroot, cvsroot) )
		return;

#if INTERNAL_AUTHEN
	AuthenModel* curModel = AuthenModel::GetInstance(gAuthen.kind());
	if( !curModel->HasLogin() )
	{
		cvs_err(_i18n("Logging in is required for 'pserver' or 'sspi' authentication only (see Preferences dialog).\n"
			"Please consult the CVS manual for more details.\n"));
	}
#endif /* INTERNAL_AUTHEN */
	
	if( gCvsPrefs.HasLoogedIn() && gCvsPrefs.LogoutTimeOut() > 0 )
	{
		cvs_err(_i18n("Making automatic logout:\n"));
		CvsCmdLogout();
		WaitForCvs();
	}
	
	gCvsPrefs.SetHasLoogedIn(true);

	CvsArgs args;

	if( forceCvsroot )
	{
#if INTERNAL_AUTHEN
		args.addcvsroot();
#else
		args.add("-d");
		args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
	}

	args.add("login");

	args.print();
	launchCVS(0L, args.Argc(), args.Argv());
}

/*!
	Execute <b>cvs logout</b> command
*/
void CvsCmdLogout(void)
{
#if INTERNAL_AUTHEN
	if( gCvsPrefs.empty() )
		return;

	AuthenModel* curModel = AuthenModel::GetInstance(gAuthen.kind());
	if( !curModel->HasLogin() )
	{
		cvs_err(_i18n("Logging out is required for 'pserver' or 'sspi' authentication only (see Preferences dialog).\n"
			"Please consult the CVS manual for more details.\n"));
	}
#endif /* INTERNAL_AUTHEN */
	
	gCvsPrefs.SetHasLoogedIn(false);

	CvsArgs args;

	args.add("logout");

	args.print();
	launchCVS(0L, args.Argc(), args.Argv());
}

/*!
	Execute <b>cvs rtag</b> command
	\param handler Selection for the command
	\param tagType Tag type
*/
void CvsCmdRtag(KiSelectionHandler& handler, const kTagCmdType tagType)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	bool norecurs;
	bool movDelBranch;
	bool overwriteExisting;
	bool useMostRecent;
	bool lookAttic;
	string modname;
	string tagname;
	string date;
	string rev;
	bool forceCvsroot;
	string cvsroot;

	if( !CompatGetRtag(mf, tagType, norecurs, movDelBranch, overwriteExisting, tagname, modname, date, rev, useMostRecent, lookAttic, forceCvsroot, cvsroot) )
		return;

	CvsArgs args;

	if( forceCvsroot )
	{
#if INTERNAL_AUTHEN
		args.addcvsroot();
#else
		args.add("-d");
		args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
	}

	args.add("rtag");
	
	switch( tagType )
	{
	case kTagCreate:
		// Nothing to do
		break;
	case kTagDelete:
		args.add("-d");
		break;
	case kTagBranch:
		args.add("-b");
		break;
	default:
		break;
	}
	
	if( norecurs )
		args.add("-l");
	
	if( movDelBranch )
		args.add("-B");
	
	if( lookAttic )
		args.add("-a");
	
	if( tagType == kTagCreate || tagType == kTagBranch )
	{
		if( useMostRecent )
			args.add("-f");
		
		if( overwriteExisting )
			args.add("-F");
		
		if( !date.empty() )
		{
			args.add("-D");
			args.add(date.c_str());
		}
		
		if( !rev.empty() )
		{
			args.add("-r");
			args.add(rev.c_str());
		}
	}

	args.endopt();
	args.add(tagname.c_str());
	args.add(modname.c_str());
	
	args.print();
	launchCVS(0L, args.Argc(), args.Argv());
}

/*!
	Execute <b>cvs checkout</b> command
	\param handler Selection for the command
*/
void CvsCmdCheckoutModule(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi = mf->begin();
	const char* dir = mfi->GetDir();

	bool norecurs;
	string modname;
	string path(dir);
	bool toStdout;
	string date;
	string rev;
	bool useMostRecent;
	string rev1;
	string rev2;
	bool branchPointMerge;
	bool threeWayConflicts;
	bool forceCvsroot;
	string cvsroot;
	bool overrideCheckoutDir;
	string checkoutDir;
	string keyword;
	bool resetSticky;
	bool dontShortenPaths;
	bool caseSensitiveNames;
	bool lastCheckinTime;

	if( !CompatGetCheckout(mf, modname, path, norecurs,
		toStdout, date, rev, useMostRecent, rev1, rev2, branchPointMerge, threeWayConflicts, 
		forceCvsroot, cvsroot, overrideCheckoutDir, checkoutDir, keyword, resetSticky, dontShortenPaths, 
		caseSensitiveNames, lastCheckinTime) )
	{
		return;
	}

	CvsArgs args;

	if( forceCvsroot )
	{
#if INTERNAL_AUTHEN
		if ( cvsroot.empty() )
		  args.addcvsroot();
		else 
		{
			args.add("-d");
			args.add(cvsroot.c_str());
		}
#else
		args.add("-d");
		args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
	}

	args.add("checkout");

	if( gCvsPrefs.PruneOption() )
		args.add("-P");
	
	if( norecurs )
		args.add("-l");
	
	if( toStdout )
		args.add("-p");
	
	if( caseSensitiveNames )
		args.add("-S");
	
	if( lastCheckinTime )
		args.add("-t");
	
	if( resetSticky )
	{
		args.add("-A");
	}
	else
	{
		if( !keyword.empty() )
		{
			args.add("-k");
			args.add(keyword.c_str());
		}
		
		if( useMostRecent )
		{
			args.add("-f");
		}
		
		if( !date.empty() )
		{
			args.add("-D");
			args.add(date.c_str());
		}
		
		if( !rev.empty() )
		{
			args.add("-r");
			args.add(rev.c_str());
		}
	}

	if( !rev1.empty() )
	{
		if( branchPointMerge )
			args.add("-b");
		
		if( threeWayConflicts )
			args.add("-3");
		
		string tmp;
		tmp = "-j";
		tmp += rev1;
		args.add(tmp.c_str());
	}
	
	if( !rev2.empty() )
	{
		string tmp;
		tmp = "-j";
		tmp += rev2;
		args.add(tmp.c_str());
	}
	
	if( overrideCheckoutDir )
	{
		if( dontShortenPaths )
			args.add("-N");

		args.add("-d");
		args.add(checkoutDir.c_str());
	}

	args.endopt();

	vector<string> modules;
	ParseCommandLine(modname.c_str(), modules);

	for(vector<string>::const_iterator it = modules.begin(); it != modules.end(); it++)
	{
		args.add(it->c_str());
	}

	args.print(path.c_str());
	launchCVS(path.c_str(), args.Argc(), args.Argv());
}

/*!
	Execute <b>cvs export</b> command
	\param handler Selection for the command
*/
void CvsCmdExportModule(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi = mf->begin();
	const char* dir = mfi->GetDir();

	bool norecurs;
	string modname;
	string path(dir);
	string date;
	string rev;
	bool useMostRecent;

	bool forceCvsroot;
	string cvsroot;
	bool overrideCheckoutDir;
	string checkoutDir;
	string keyword;
	bool dontShortenPaths;
	bool caseSensitiveNames;
	bool lastCheckinTime;

	if( !CompatGetExport(mf, modname, path, norecurs,
		date, rev, useMostRecent, 
		forceCvsroot, cvsroot, overrideCheckoutDir, checkoutDir, keyword, dontShortenPaths, 
		caseSensitiveNames, lastCheckinTime) )
	{
		return;
	}

	CvsArgs args;

	if( forceCvsroot )
	{
#if INTERNAL_AUTHEN
		if ( cvsroot.empty() )
		  args.addcvsroot();
		else 
		{
			args.add("-d");
			args.add(cvsroot.c_str());
		}
#else
		args.add("-d");
		args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
	}

	args.add("export");

	if( norecurs )
		args.add("-l");
	
	if( caseSensitiveNames )
		args.add("-S");
	
	if( lastCheckinTime )
		args.add("-t");
	
	if( !keyword.empty() )
	{
		args.add("-k");
		args.add(keyword.c_str());
	}
	
	if( useMostRecent )
	{
		args.add("-f");
	}
	
	if( !date.empty() )
	{
		args.add("-D");
		args.add(date.c_str());
	}
	
	if( !rev.empty() )
	{
		args.add("-r");
		args.add(rev.c_str());
	}

	if( rev.empty() && date.empty() )
	{
		args.add("-r");
		args.add("HEAD");
	}

	if( overrideCheckoutDir )
	{
		if( dontShortenPaths )
			args.add("-N");

		args.add("-d");
		args.add(checkoutDir.c_str());
	}

	args.endopt();

	vector<string> modules;
	ParseCommandLine(modname.c_str(), modules);

	for(vector<string>::const_iterator it = modules.begin(); it != modules.end(); it++)
	{
		args.add(it->c_str());
	}

	args.print(path.c_str());
	launchCVS(path.c_str(), args.Argc(), args.Argv());
}

/*!
	Execute <b>cvs import</b> command
	\param handler Selection for the command
*/
void CvsCmdImportModule(KiSelectionHandler& handler)
{	
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		bool serverCvswrappers;
		ReportConflict* entries = NULL;
		ReportWarning* warnings = NULL;
		string modname, vendortag, reltag, path(mfi->GetDir()), vendorBranchId;
		string logmsg;
		bool useDefIgnore;
		bool useFilesTime;
		bool createSandbox;
		bool dontCreateVendorTag;
		bool overwriteReleaseTags;
		bool forceCvsroot;
		string cvsroot;

		if( !CompatGetImport(mf, serverCvswrappers, entries, warnings, modname, logmsg, vendortag, reltag, vendorBranchId, path, useDefIgnore, useFilesTime, createSandbox, dontCreateVendorTag, overwriteReleaseTags, forceCvsroot, cvsroot) )
			return;
		
		CvsArgs args;
		
		if( forceCvsroot )
		{
#if INTERNAL_AUTHEN
			if ( cvsroot.empty() )
			  args.addcvsroot();
			else 
			{
  			args.add("-d");
	  		args.add(cvsroot.c_str());
			}
#else
			args.add("-d");
			args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
		}
		
		args.add("import");
		
		if( createSandbox )
		{
			args.add("-C");
		}

		if( useFilesTime )
		{
			args.add("-d");
		}
		
		if( !dontCreateVendorTag && overwriteReleaseTags )
		{
			args.add("-f");
		}

		// Fill the wrappers
		ReportConflict* tmpentry = entries;
		
		if( !useDefIgnore )
		{
			args.add("-I"); // no default (core, *.o...)
			args.add("!");
			args.add("-I"); // but ignore the CVS folders
			args.add("CVS");
		}
		
		while( tmpentry != 0L )
		{
			if( tmpentry->HasIgnore() )
			{
				string ign;
				args.add("-I");
				
				if( tmpentry->IsExtension() )
					ign += '*';
				
				ign += tmpentry->GetPattern();
				args.add(ign.c_str());
			}
			else if( !serverCvswrappers && 
				!tmpentry->HasForceText() &&
				(tmpentry->IsBinary() || tmpentry->HasForceBinary() || tmpentry->HasForceNoKeywords() || 
				tmpentry->IsUnicode() || tmpentry->HasForceUnicode()) )
			{
				string wrap;
				const char* pattern = tmpentry->GetPattern();
				bool hasSpace = strchr(pattern, ' ') != 0L;
				args.add("-W");
				
				if( hasSpace )
					wrap += '"';
				
				if( tmpentry->IsExtension() )
					wrap += '*';
				
				wrap += pattern;
				
				if( hasSpace )
					wrap += '"';
				
				if( tmpentry->HasForceNoKeywords() )
				{
					wrap += " -k 'o'";
				}
				else if( tmpentry->HasForceBinary() )
				{
					wrap += " -k 'b'";
				}
				else if( tmpentry->HasForceUnicode() )
				{
					wrap += " -k 'u'";
				}
				else if( tmpentry->IsBinary() )
				{
					wrap += " -k 'b'";
				}
				else if( tmpentry->IsUnicode() )
				{
					wrap += " -k 'u'";
				}
				
				args.add(wrap.c_str());
			}
			
			tmpentry = tmpentry->next;
		}
		
		free_list_types(warnings, entries);
		
		if( !vendorBranchId.empty() )
		{
			args.add("-b");
			args.add(vendorBranchId.c_str());
		}

		args.add("-m");
		args.add(NormalizeLogMsg(logmsg).c_str());
		
		if( dontCreateVendorTag )
		{
			args.add("-n");
			args.endopt();
			args.add(modname.c_str());
		}
		else
		{
			args.endopt();
			args.add(modname.c_str());
			args.add(vendortag.c_str());
			args.add(reltag.c_str());
		}

		args.print(path.c_str());
		launchCVS(path.c_str(), args.Argc(), args.Argv());
	}
}

/*!
	Execute command line
	\param handler Selection for the command
*/
void CvsCmdCommandLine(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	string dir;
	string cmd;
	bool addDefault;
	bool addSelection;
	bool forceCvsroot;
	string cvsroot;

	if( !CompatGetCommand(mf, cmd, dir, addDefault, addSelection, forceCvsroot, cvsroot) )
		return;

	vector<string> arguments;
	ParseCommandLine(cmd.c_str(), arguments);

	if( addSelection )
	{
		MultiFiles::const_iterator mfi;
		for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
		{
			CvsArgs args(addDefault);
			
			if( addDefault && forceCvsroot )
			{
#if INTERNAL_AUTHEN
				args.addcvsroot();
#else
				args.add("-d");
				args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
			}

			for(vector<string>::const_iterator it = arguments.begin(); it != arguments.end(); it++)
			{
				if( addDefault && it == arguments.begin() && it->compare(CVS_CMD) == 0 )
				{
					// Skip the "cvs" arguments as it's already added by args constructor
					continue;
				}

				args.add(it->c_str());
				
				if( !addDefault && forceCvsroot && it == arguments.begin() && it->compare(CVS_CMD) == 0 )
				{
#if INTERNAL_AUTHEN
					args.addcvsroot();
#else
					args.add("-d");
					args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
				}
			}
			
			const char* dir = mfi->add(args);
			args.print(dir);
			launchCVS(dir, args.Argc(), args.Argv());
		}
	}
	else
	{
		CvsArgs args(addDefault);
		
		if( addDefault && forceCvsroot )
		{
#if INTERNAL_AUTHEN
			args.addcvsroot();
#else
			args.add("-d");
			args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
		}

		for(vector<string>::const_iterator it = arguments.begin(); it != arguments.end(); it++)
		{
			if( addDefault && it == arguments.begin() && it->compare(CVS_CMD) == 0 )
			{
				// Skip the "cvs" arguments as it's already added by args constructor
				continue;
			}
			
			args.add(it->c_str());
			
			if( !addDefault && forceCvsroot && it == arguments.begin() && it->compare(CVS_CMD) == 0 )
			{
#if INTERNAL_AUTHEN
				args.addcvsroot();
#else
				args.add("-d");
				args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
			}
		}
		
		args.print(dir.c_str());
		launchCVS(dir.c_str(), args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs update</b> command
	\param handler Selection for the command
*/
void CvsCmdCancelChanges(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		RemoveCvsArgs args;
		args.add("update");

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs update</b> command
	\param handler Selection for the command
	\param queryOnly true for query update, false for the update command
*/
void CvsCmdUpdate(KiSelectionHandler& handler, bool queryOnly)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	bool toStdout;
	bool noRecurs;
	bool resetSticky;
	string date;
	string rev;
	bool useMostRecent;
	string rev1;
	string rev2;
	bool branchPointMerge;
	bool threeWayConflicts;
	bool createMissDir;
	bool getCleanCopy;
	string keyword;
	bool caseSensitiveNames;
	bool lastCheckinTime;

	if( !CompatGetUpdate(mf, queryOnly, toStdout, noRecurs, resetSticky,
		date, rev, useMostRecent, rev1, rev2, branchPointMerge, threeWayConflicts, 
		createMissDir, getCleanCopy, keyword, caseSensitiveNames, lastCheckinTime) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;

		if( queryOnly )
			args.add("-n");
		
		args.add("update");

		if( !resetSticky && !keyword.empty() )
		{
			args.add("-k");
			args.add(keyword.c_str());
		}

		if( gCvsPrefs.PruneOption() )
			args.add("-P");

		if( !queryOnly )
		{
			if( caseSensitiveNames )
				args.add("-S");
			
			if( lastCheckinTime )
				args.add("-t");
			
			if( resetSticky )
				args.add("-A");
		
			if( createMissDir )
				args.add("-d");
			
			if( getCleanCopy )
				args.add("-C");
			
			if( !resetSticky && !date.empty() )
			{
				args.add("-D");
				args.add(date.c_str());
			}
			
			if( !resetSticky && useMostRecent )
				args.add("-f");
			
			if( !rev1.empty() )
			{
				if( branchPointMerge )
					args.add("-b");

				if( threeWayConflicts )
					args.add("-3");

				string tmp;
				tmp = "-j";
				tmp += rev1;
				args.add(tmp.c_str());
			}
			
			if( !rev2.empty() )
			{
				string tmp;
				tmp = "-j";
				tmp += rev2;
				args.add(tmp.c_str());
			}
			
			if( noRecurs )
				args.add("-l");
			
			if( toStdout )
				args.add("-p");
			
			if( !resetSticky && !rev.empty() )
			{
				args.add("-r");
				args.add(rev.c_str());
			}
		}

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs commit</b> command
	\param handler Selection for the command
*/
void CvsCmdCommit(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	bool norecurs;
	string logmsg;
	bool forceCommit;
	bool forceRecurse;
	string rev;
	bool noModuleProgram;
	bool checkValidEdits;	

	if( !CompatGetCommit(mf, logmsg, norecurs, forceCommit, forceRecurse, rev, noModuleProgram, checkValidEdits) )
		return;
	
	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("commit");

		if( checkValidEdits )
			args.add("-c");
		
		if( norecurs )
			args.add("-l");
		
		if( noModuleProgram )
			args.add("-n");
		
		if( forceCommit )
			args.add("-f");
		
		if( forceRecurse )
			args.add("-R");
		
		args.add("-m");
		args.add(NormalizeLogMsg(logmsg).c_str());
		
		if( !rev.empty() )
		{
			args.add("-r");
			args.add(rev.c_str());
		}

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs diff</b> or <b>external diff</b> command
	\param handler Selection for the command
	\param diffType Diff command type
*/
void CvsCmdDiff(KiSelectionHandler& handler, kDiffCmdType diffType)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	bool noRecurs;
	bool isDate1;
	bool isDate2;
	bool useExtDiff;
	bool bulkUpdate;
	string rev1;
	string rev2;
	bool unifiedDiff;
	bool noWhiteSpace;
	bool noBlankLines;
	bool ignoreCase;
	string diffOptions;
	string keyword;
	string diffTool;

	if( diffType == kDiffGraph )
	{
		// determine initial revision settings
		if( mf->TotalNumFiles() > 0 )
		{
			MultiFilesEntry& mfe = *mf->begin();
			string ignorePath, ignoreName, rev;

			switch( mfe.NumFiles() )
			{
			case 2:
				if( mfe.get(1, ignorePath, ignoreName, rev) ) 
					rev2 = rev;

				// Even if two files are selected, leave only one in mfe:
				mfe.del_last();
				
				// fall thru
			case 1:
				if( mfe.get(0, ignorePath, ignoreName, rev) ) 
					rev1 = rev;

				break;
			}
		}
	}
	
	if( !CompatGetDiff(mf, diffType, noRecurs, isDate1, isDate2, rev1, rev2, useExtDiff, diffTool,
		bulkUpdate, unifiedDiff, noWhiteSpace, noBlankLines, ignoreCase, keyword, diffOptions, 
		true) )
	{
		return;
	}

	// determine changed files with diff --brief -N
	MultiFiles mfDiffFolder;
	if( !mf->TotalNumFiles() && useExtDiff )
	{
		cvs_out(_i18n("Looking for changed files...\n"));

		bool cvsDiffResult = true;
		for(MultiFiles::const_iterator mfi = mf->begin(); mfi != mf->end(); ++mfi)
		{
			CvsArgs args;
			args.add("diff");

			if( noRecurs )
				args.add("-l");

			if( !rev1.empty() )
			{
				args.add(isDate1 ? "-D" : "-r");
				args.add(rev1.c_str());
			}

			if( !rev2.empty() )
			{
				args.add(isDate2 ? "-D" : "-r");
				args.add(rev2.c_str());
			}

			args.add("--brief"); // only report whether files differ
			args.add("-N");      // treat absent files as empty
			
			const char* dir = mfi->add(args);
			args.print(dir);

	 		cvsDiffResult = CvsDiffParse(dir, args, mfDiffFolder);
	 		if( !cvsDiffResult )
			{
				// Try external diff if anything fails
				cvs_out(_i18n("Search abandoned\n"));
				useExtDiff = true;
				break;
			}
		}

		if( cvsDiffResult )
		{
			mf = &mfDiffFolder;
			cvs_out(_i18n("Total files found: %d\n"), mf->TotalNumFiles());
		}
	}

	bool extDiffResult = false;

	for(MultiFiles::const_iterator mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		if( mfi->NumFiles() && useExtDiff )
		{
			if( !bulkUpdate ) 
			{
				string base, ext;
				string path, filename, currRev;
				string file1, file2;

				for(int i = 0; ; i++)
				{
					if( !mfi->get(i, path, filename, currRev) )
						break;

					// Graph diff now always contains only one file,
					// no need to check that here

					// note that filename can contain subfolders
					SplitFilename(filename.c_str(), base, ext);

					CvsArgs args;
					string tmpf(base);
					tmpf += '_';
					
					if( !rev1.empty() )
						tmpf += rev1;
					else
						tmpf += currRev;
					
					tmpf += '_';

					// convert subfolder names to regular name
					FlatenSubFolders(tmpf);

					args.add("update");
					args.add("-p");
					
					if( !rev1.empty() )
					{
						args.add(isDate1 ? "-D" : "-r");
						args.add(rev1.c_str());
					}
 					else if( !currRev.empty() ) 
					{
 						args.add("-r");
						args.add(currRev.c_str());
  					}

					args.startfiles();
					args.add(filename.c_str());

					args.print(path.c_str());
					file1 = launchCVS(path.c_str(), args, tmpf.c_str(), ext.c_str());
					if( file1.empty() )
						return; // Stop immediately when the user cancels the command

					if( rev2.empty() )
					{
						file2 = path;
						NormalizeFolderPath(file2);
						file2 += filename;
					}
					else
					{
						CvsArgs args2;

						tmpf = base;
						tmpf += '_';
						tmpf += rev2;
						tmpf += '_';

						// convert subfolder names to regular name
						FlatenSubFolders(tmpf);

						args2.add("update");
						args2.add("-p");
						args2.add(isDate2 ? "-D" : "-r");
						args2.add(rev2.c_str());

						args2.startfiles();
						args2.add(filename.c_str());

						args2.print(path.c_str());
						file2 = launchCVS(path.c_str(), args2, tmpf.c_str(), ext.c_str());
					}

					if( file2.empty() )
						return; // Stop immediately when the user cancels the command

					extDiffResult = LaunchDiff(file1.c_str(), file2.c_str(), diffTool.c_str(), unifiedDiff, noWhiteSpace, noBlankLines, ignoreCase);
				}
			}
			else
			{
				CvsArgs args(true, true); // Must be verbose

				// Issue one update command for all files:
				args.add("update");
				args.add("-p");

				if( !rev1.empty() )
				{
					args.add(isDate1 ? "-D" : "-r");
					args.add(rev1.c_str());
				}
				else
				{
					args.add("-r");
					// Tag "BASE" works properly for multi-file diffs
					args.add("BASE");
				}

				const char* path = mfi->add(args);
				args.print(path);

				// Keep option to upgrade to three-way diff later
				const int DIFF_COUNT = 2;
				vector<string> diffFiles[DIFF_COUNT];

				// Get first set of files from CVS (rev1)
				// Need this block to call ~CCvsUpdateToTempConsole() on time
				{
					CCvsUpdateToTempConsole console(CCvsUpdateToTempConsole::kUpdateFilter, &(*mfi), rev1.c_str(), diffFiles[0]);
					int iRes = launchCVS(path, args.Argc(), args.Argv(), &console);
					// Stop immediately if the user cancels the command or if there is an error
					if( iRes )
					{
						console.Abort();
						return;
					}
				}

				if( rev2.empty() )
				{
					// Get second set of files from file system
					for(int i = 0; ; i++)
					{
						string path, filename, currRev;

						if( !mfi->get(i, path, filename, currRev) )
							break;

						string file = path;
						NormalizeFolderPath(file);
						file += filename;

						diffFiles[1].push_back(file);
					}
				}
				else
				{
					// Get second set of files from CVS (rev2)
					CvsArgs args2(true, true);

					args2.add("update");
					args2.add("-p");
					args2.add(isDate2 ? "-D" : "-r");
					args2.add(rev2.c_str());

					const char* path = mfi->add(args2);
		
					args2.print(path);

					{
						CCvsUpdateToTempConsole console(CCvsUpdateToTempConsole::kUpdateFilter, &(*mfi), rev1.c_str(), diffFiles[0]);
						int iRes = launchCVS(path, args2.Argc(), args2.Argv(), &CCvsUpdateToTempConsole(CCvsUpdateToTempConsole::kUpdateFilter, &(*mfi), rev2.c_str(), diffFiles[1]));
						// Stop immediately if the user cancels the command or if there is an error
						if( iRes )
						{
							console.Abort();
							return;
						}
					}
				}

				// An iterator for each set of diff files
				vector<string>::const_iterator iDiffFile[DIFF_COUNT];
				int i;

				for(i = 0; i < DIFF_COUNT; i++)
					iDiffFile[i] = diffFiles[i].begin();

				// A list of diff sets
				list<vector<string> > lDiffSets;

				// Iterate over all pairs
				for(;;)
				{
					// Is one of the iterators at end?
					for(i = 0; i < DIFF_COUNT; i++)
					{
						if( iDiffFile[i] == diffFiles[i].end() )
							break;
					}

					if( i < DIFF_COUNT )
						break;

					// Check if all files are available
					for(i = 0; i < DIFF_COUNT; i++)
					{
						if( iDiffFile[i]->length() == 0 )
							break;
					}

					// Are all files available?
					if( i >= DIFF_COUNT )
					{
						// Yes: new diff set
						lDiffSets.push_back(vector<string>());
						list<vector<string> >::reference rCurrent = lDiffSets.back();

						rCurrent.resize(DIFF_COUNT);
						for(i = 0; i < DIFF_COUNT; i++)
						{
							rCurrent[i] = *iDiffFile[i];
						}
					}

					// Increment each iterator
					for(i = 0; i < DIFF_COUNT; i++)
						iDiffFile[i]++;
				}

				// Launch diff for each diff set
				for(list<vector<string> >::const_iterator iDiffSet = lDiffSets.begin();
					iDiffSet != lDiffSets.end(); iDiffSet++)
				{
					extDiffResult = LaunchDiff((*iDiffSet)[0].c_str(), (*iDiffSet)[1].c_str(),
						diffTool.c_str(), unifiedDiff, noWhiteSpace, noBlankLines,
						ignoreCase);

					if( !extDiffResult )
						break;
				}

			}
		}

		if( !mfi->NumFiles() || !useExtDiff || !extDiffResult )
		{
			CvsArgs args;
			args.add("diff");

			if( noRecurs )
				args.add("-l");

			if( !rev1.empty() )
			{
				args.add(isDate1 ? "-D" : "-r");
				args.add(rev1.c_str());
			}
			
			if( !rev2.empty() )
			{
				args.add(isDate2 ? "-D" : "-r");
				args.add(rev2.c_str());
			}
			
			if( unifiedDiff )
				args.add("-u");	    // --unified, -u

			if( noWhiteSpace )
				args.add("-wb");	//--ignore-all-space --ignore-space-change

			if( noBlankLines )
				args.add("-B");		//--ignore-blank-lines

			if( ignoreCase )
				args.add("-i");		//--ignore-case

			if( !keyword.empty() )
			{
				args.add("-k");
				args.add(keyword.c_str());
			}

			if( !diffOptions.empty() )
			{
				args.parse(diffOptions.c_str());
			}

			const char* dir = mfi->add(args);
			args.print(dir);
			launchCVS(dir, args.Argc(), args.Argv());
		}
	}
}

/*!
	Execute <b>cvs add</b> command
	\param handler Selection for the command
	\param addType Add type
*/
void CvsCmdAdd(KiSelectionHandler& handler, kAddCmdType addType /*= kAddNormal*/)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		if( mfi->NumFiles() > 0 )
		{
			// Files
			ControlCvsArgs* args = NULL;
			switch( addType )
			{
			default:
			case kAddNormal:
				args = new AddCvsArgs;
				break;
			case kAddBinary:
				args = new AddBinCvsArgs;
				break;
			case kAddUnicode:
				args = new AddUnicodeCvsArgs;
				break;
			}

			if( !args )
			{
				cvs_err("Unable to allocate memory for arguments list\n");
				return;
			}

			args->add("add");
			
			if( addType == kAddBinary )
			{
				args->add("-kb");
			}
			else if( addType == kAddUnicode )
			{
				args->add("-ku");
			}

			const char* dir = mfi->add(*args);
			
			CvsAlert cvsAlert(kCvsAlertWarningIcon, 
				_i18n("Do you want to continue?"), _i18n("Some problems have been found while adding files and should be corrected. See the console window for details."),
				BUTTONTITLE_CONTINUE, BUTTONTITLE_CANCEL);

			if( !args->HasProblem() || cvsAlert.ShowAlert() == kCvsAlertOKButton )
			{
				args->print(dir);
				launchCVS(dir, args->Argc(), args->Argv());
			}

			delete args;
		}
		else
		{
			// Directory
			CvsArgs args;
			args.add("add");

			args.endopt();

			string uppath, folder;
			const char* dir = mfi->addfolder(args, uppath, folder);

			args.print(dir);
			launchCVS(dir, args.Argc(), args.Argv());
		}
	}
}

/*!
	Execute <b>cvs remove</b> command
	\param handler Selection for the command
*/
void CvsCmdRemove(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	bool noRecurs;
	kFilesRemoveType filesRemoveType;

	if( !CompatGetRemove(mf, noRecurs, filesRemoveType) )
	{
		return;
	}

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		RemoveCvsArgs args;
		
		if( filesRemoveType == kMoveToRecycleBin )
		{
			args.SetMoveToRecycleBin(true);
			CompatBeginMoveToTrash();
		}
		
		args.add("remove");

		if( noRecurs )
			args.add("-l");

		if( filesRemoveType == kRemoveByCvs )
		{
			args.add("-f");
		}

		const char* dir = mfi->add(args);
		args.print(dir);
		
		if( filesRemoveType == kMoveToRecycleBin )
		{
			CompatEndMoveToTrash();
		}
		
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs log</b> or <b>graph</b> command
	\param handler Selection for the command
	\param outGraph true for the graph
	\param defWnd Default window
*/
void CvsCmdLog(KiSelectionHandler& handler, bool outGraph, void* defWnd)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	bool noRecurs;
	bool defBranch;
	string date;
	bool headerOnly;
	bool headerDesc;
	bool supress;
	bool cvsRcsOutput;
	bool noTags;
	bool onlyRCSfile;
	bool hasRev;
	string rev;
	string states;
	bool hasUser;
	string users;

	if( !CompatGetLog(mf, noRecurs, defBranch, date,
		headerOnly, headerDesc, supress, cvsRcsOutput, 
		noTags, onlyRCSfile, hasRev,
		rev, states, hasUser, users,
		outGraph) )
	{
		return;
	}

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("log");

		if( noRecurs )
			args.add("-l");

		if( defBranch )
			args.add("-b");

		if( headerDesc )
		{
			args.add("-t");
		}
		else
		{
			if( headerOnly )
				args.add("-h");
		}

		if( supress )
			args.add("-S");

		if( cvsRcsOutput )
			args.add("-X");
		
		if( noTags )
			args.add("-N");

		if( onlyRCSfile )
			args.add("-R");

		if( !date.empty() )
		{
			args.add("-d");
			args.add(date.c_str());
		}

		if( !states.empty() )
		{
			args.add("-s");
			args.add(states.c_str());
		}

		if( hasRev )
		{
			string tmp;
			tmp = "-r";
			tmp += rev;
			args.add(tmp.c_str());
		}

		if( hasUser )
		{
			string tmp;
			tmp = "-w";
			tmp += users;
			args.add(tmp.c_str());
		}

		const char* dir = mfi->add(args);
		args.print(dir);
		CvsLogParse(dir, args, outGraph, defWnd);
	}
}

/*!
	Execute <b>cvs status</b> command
	\param handler Selection for the command
*/
void CvsCmdStatus(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	bool noRecurs = false;
	STATUSOUTPUT_TYPE outputType = STATUSOUTPUT_VERBOSE;

	if( !CompatGetStatus(mf, noRecurs, outputType) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("status");

		if( noRecurs )
			args.add("-l");

		if( STATUSOUTPUT_VERBOSE == outputType )
		{
			args.add("-v");
		}
		else if( STATUSOUTPUT_QUICK == outputType )
		{
			args.add("-q");
		}

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs admin -l</b> command
	\param handler Selection for the command
*/
void CvsCmdLock(KiSelectionHandler& handler)
{	
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("admin");
		args.add("-l");

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs admin -u</b> command
	\param handler Selection for the command
*/
void CvsCmdUnlock(KiSelectionHandler& handler)
{	
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("admin");
		args.add("-u");

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs tag</b> command
	\param handler Selection for the command
	\param tagType Tag type
*/
void CvsCmdTag(KiSelectionHandler& handler, const kTagCmdType tagType)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;
	
	bool noRecurs;
	bool movDelBranch;
	bool overwriteExisting;
	bool checkUnmod;
	string tagName;
	bool useMostRecent;
	string date;
	string rev;
	
	if( kTagCreateGraph == tagType || 
		kTagDeleteGraph == tagType || 
		kTagBranchGraph == tagType )
	{
		if( mf->TotalNumFiles() == 1 )
		{
			const MultiFilesEntry& mfe = *mf->begin();
			
			if( mfe.NumFiles() == 1 )
			{
				string ignorePath, ignoreName, revision;
				
				if( mfe.get(0, ignorePath, ignoreName, revision) ) 
					rev = revision;
			}
		}
	}
	
	if( !CompatGetTag(mf, tagType, noRecurs, movDelBranch, overwriteExisting, tagName, checkUnmod, date, rev, useMostRecent) )
		return;
	
	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("tag");
		
		switch( tagType )
		{
		case kTagCreate:
		case kTagCreateGraph:
			// Nothing to do
			break;
		case kTagDelete:
		case kTagDeleteGraph:
			args.add("-d");
			break;
		case kTagBranch:
		case kTagBranchGraph:
			args.add("-b");
			break;
		default:
			ASSERT(FALSE); // Unknown type
			break;
		}

		if( noRecurs )
			args.add("-l");
		
		if( movDelBranch )
			args.add("-B");
		
		if( tagType == kTagCreate || 
			tagType == kTagBranch ||
			tagType == kTagCreateGraph || 
			tagType == kTagBranchGraph )
		{
			if( useMostRecent )
				args.add("-f");
			
			if( overwriteExisting )
				args.add("-F");
			
			if( checkUnmod )
				args.add("-c");

			if( !date.empty() )
			{
				args.add("-D");
				args.add(date.c_str());
			}
			
			if( !rev.empty() )
			{
				args.add("-r");
				args.add(rev.c_str());
			}
		}
		
		args.endopt();
		args.add(tagName.c_str());
		
		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv(), handler.GetConsole());
	}
}

/*!
	Execute <b>cvs edit</b> command
	\param handler Selection for the command
	\param editType Edit type
*/
void CvsCmdEdit(KiSelectionHandler& handler, kEditCmdType editType /*= kNormal*/)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("edit");

		switch( editType )
		{
		default:
		case kEditNormal:
			// intentionally blank
			break;
		case kEditReserved:
			args.add("-c");
			break;
		case kEditForce:
			args.add("-f");
			break;
		}
		
		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs unedit</b> command
	\param handler Selection for the command
*/
void CvsCmdUnedit(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("unedit");

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs watch add</b> command
	\param handler Selection for the command
*/
void CvsCmdWatchOn(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("watch");
		args.add("add");

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs watch remove</b> command
	\param handler Selection for the command
*/
void CvsCmdWatchOff(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("watch");
		args.add("remove");

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs release</b> command
	\param handler Selection for the command
*/
void CvsCmdRelease(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	kReleaseDelType removeDirectoryOption = kReleaseNoDel;
	bool forceCvsroot;
	string cvsroot;
	
	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		if( !CompatGetRelease(mf, mfi->GetDir(), removeDirectoryOption, forceCvsroot, cvsroot) )
		{
			continue;
		}
		
		CvsArgs args;

		if( forceCvsroot )
		{
#if INTERNAL_AUTHEN
			args.addcvsroot();
#else
			args.add("-d");
			args.add(cvsroot.c_str());
#endif /* INTERNAL_AUTHEN */
		}

		args.add("release");
		
		switch( removeDirectoryOption )
		{
		default:
		case kReleaseNoDel:
			break;
		case kReleaseDelDir:
			args.add("-d");
			break;
		case kReleaseDelAll:
			args.add("-d");
			args.add("-f");
			break;
		case kReleaseDelCvsInfo:
			args.add("-e");
			break;
		}
		
		args.endopt();

		string uppath, folder;
		const char* dir = mfi->addfolder(args, uppath, folder);

		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs watchers</b> command
	\param handler Selection for the command
*/
void CvsCmdWatchers(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("watchers");

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs editors</b> command
	\param handler Selection for the command
*/
void CvsCmdEditors(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		CvsArgs args;
		args.add("editors");

		const char* dir = mfi->add(args);
		args.print(dir);
		launchCVS(dir, args.Argc(), args.Argv());
	}
}

/*!
	Execute <b>cvs annotate</b> command
	\param handler Selection for the command
	\param annotateType Annotate command type
*/
void CvsCmdAnnotate(KiSelectionHandler& handler, kAnnotateCmdType annotateType)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;

	bool noRecurs;
	bool isDate;
	string rev;
	bool force;
	bool bulkAnnotate;
	kLaunchType launchType = kLaunchAnnotateViewer;

	if( annotateType == kAnnotateGraph )
	{
		if( mf->TotalNumFiles() == 1 )
		{
			const MultiFilesEntry& mfe = *mf->begin();

			if( mfe.NumFiles() == 1 )
			{
				string ignorePath, ignoreName, revision;
				
				if( mfe.get(0, ignorePath, ignoreName, revision) ) 
					rev = revision;
			}
		}
	}
	
	if( !CompatGetAnnotate(mf, annotateType, noRecurs, isDate, rev, force, bulkAnnotate, launchType) )
		return;

	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		if( !bulkAnnotate )
		{
			string base, ext;
			string path, filename, currRev;
			string file;
			
			for(int i = 0; ; i++)
			{
				if( !mfi->get(i, path, filename, currRev) )
					break;
				
				// note that filename can contain subfolders
				SplitFilename(filename.c_str(), base, ext);
				
				CvsArgs args;
				string tmpf(base);
				
				tmpf += "_ann_";
				
				if( !rev.empty() )
				{
					tmpf += rev;
					tmpf += '_';
				}
				
				// convert subfolder names to regular name
				FlatenSubFolders(tmpf);
				
				args.add("annotate");
				
				if( noRecurs )
				{
					args.add("-l");
				}
				
				if( force )
				{
					args.add("-f");
				}
				
				if( !rev.empty() )
				{
					args.add(isDate ? "-D" : "-r");
					args.add(rev.c_str());
				}
				
				args.addfile(filename.c_str());
				args.print(path.c_str());
				file = launchCVS(path.c_str(), args, tmpf.c_str(), ext.c_str());
				
				if( file.empty() )
					continue;
				
				LaunchHandler(launchType, file.c_str());
			}
		}
		else
		{
			CvsArgs args(true, true); // Must be verbose
			
			args.add("annotate");
			
			if( noRecurs )
			{
				args.add("-l");
			}
			
			if( force )
			{
				args.add("-f");
			}
			
			if( !rev.empty() )
			{
				args.add(isDate ? "-D" : "-r");
				args.add(rev.c_str());
			}

			const char* path = mfi->add(args);
			args.print(path);

			vector<string> annotateFiles;
			
			// Need this block to call ~CCvsUpdateToTempConsole() on time
			{
				CCvsUpdateToTempConsole console(CCvsUpdateToTempConsole::kAnnotateFilter, &(*mfi), rev.c_str(), annotateFiles);
				int iRes = launchCVS(path, args.Argc(), args.Argv(), &console);
				// Stop immediately if the user cancels the command or if there is an error
				if( iRes )
				{
					console.Abort();
					return;
				}
			}

			// Launch editor
			for(vector<string>::iterator it = annotateFiles.begin(); it != annotateFiles.end(); it++)
			{
				LaunchHandler(launchType, it->c_str());
			}
		}
	}
}

/*!
	Execute <b>erase</b> command
	\param handler Selection for the command
*/
void CvsCmdMoveToTrash(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;
	
	const char* primaryMsg = NULL;
	const char* secondaryMsg = NULL;
	kCvsAlertButtonType defaultButton = kCvsAlertCancelButton;
	
#if defined (TARGET_OS_MAC)
	primaryMsg = _i18n("Are you sure you want to move the selected files to the trash?");
	secondaryMsg = _i18n("The files will be moved into the trash just as if you deleted them from the Finder.");
	defaultButton = kCvsAlertOKButton;
#elif defined (WIN32)
	primaryMsg = _i18n("Are you sure you want to move the selected files to the recycle bin?");
#else
	primaryMsg = _i18n("Are you sure you want to remove the selected files?");
#endif

	CvsAlert cvsAlert(kCvsAlertQuestionIcon, 
		primaryMsg, secondaryMsg);

	if( cvsAlert.ShowAlert(defaultButton) != kCvsAlertOKButton )
	{
		return;
	}
	
	CompatBeginMoveToTrash();
	
	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		const int count = mfi->NumFiles();
		for(int i = 0; i < count; i++)
		{
			string fullPath;
			if( mfi->GetFullPath(i, fullPath) )
			{
				if( !CompatMoveToTrash(fullPath.c_str()) )
				{
					stringstream errText;
					
					// HINT for translate: "Unable to remove '<FILE>' (error <ERROR>)\n"
					errText << _i18n("Unable to remove '") << fullPath.c_str() << _i18n("' (error ") << errno << ")\n";
					errText << ends;
					
					cvs_err(errText.str().c_str());
				}
				else
				{
					stringstream infoText;
#if defined (TARGET_OS_MAC)
					infoText << "'" << fullPath.c_str() << _i18n("' has been moved successfully to the trash\n");
#elif defined (WIN32)
					infoText << "'" << fullPath.c_str() << _i18n("' has been moved successfully to the recycle bin\n");
#else
					infoText << "'" << fullPath.c_str() << _i18n("' has been removed successfully\n");
#endif
					infoText << ends;

					cvs_out(infoText.str().c_str());
				}
			}
		}
	}
	
	CompatEndMoveToTrash();
}

/*!
	Execute <b>file action</b> command
	\param handler Selection for the command
*/
void CvsCmdFileAction(KiSelectionHandler& handler)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;
	
	bool noRecurs = false;
	kLaunchType launchType = kLaunchEdit;
	
	if( !CompatGetFileAction(mf, launchType) )
		return;
	
	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++ )
		{
			string fullPath;
			mfi->GetFullPath(nIndex, fullPath);
			
			LaunchHandler(launchType, fullPath.c_str());
		}
	}
}

/*!
	Execute <b>launch</b> command
	\param handler Selection for the command
	\param launchType Launch type
*/
void CvsCmdLaunch(KiSelectionHandler& handler, const kLaunchType launchType)
{
	MultiFiles* mf = NULL;
	if( !CheckSelectionHandler(handler, mf) )
		return;
	
	MultiFiles::const_iterator mfi;
	for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
	{
		for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++ )
		{
			string fullPath;
			mfi->GetFullPath(nIndex, fullPath);

			LaunchHandler(launchType, fullPath.c_str());
		}
	}
}
