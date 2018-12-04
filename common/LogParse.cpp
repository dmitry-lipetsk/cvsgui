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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * LogParse.cpp : parse the CVS log output
 */

#include "stdafx.h"

#include <errno.h>
#include <stdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#	include "wincvs.h"
#	include "GraphDoc.h"
#endif /* WIN32 */

#ifdef TARGET_OS_MAC
#	include <unistd.h>
#	include "MacCvsApp.h"
#	include "CTreeView.h"
# include "MacMisc.h"
#endif /* TARGET_OS_MAC */

#if qUnix
#	include "UCvsDialogs.h"
#	include "UCvsGraph.h"
#	include "UCvsApp.h"
#endif

#include "LogParse.h"
#include "CvsArgs.h"
#include "CvsLog.h"
#include "AppGlue.h"
#include "AppConsole.h"
#include "getline.h"
#include <sstream>

using namespace std;

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#ifdef qMacCvsPP
  CPersistentInt    sCommitMsgEncoding("P_MessageEncoding", kMsgEncodingMacOSRoman, kAddSettings);
#endif

/*!
	Test for cancel key being pressed
	\return true if cancel key has been pressed, false otherwise
*/
inline bool CheckForCancelKey()
{
#if defined(qMacCvsPP)
  return CheckEventQueueForUserCancel();
#else
  return IsEscKeyPressed();
#endif
}

/*!
	Test the cancel condition
	\return true if cancel condition met, false otherwise
*/
static bool TestCancel()
{
	if( CheckForCancelKey() )
	{
		cvs_err("Cancelled by pressing Esc key\n");
		return true;
	}

	return false;
}

/*!
	Print the file revision information
	\param out Console
	\param rev File revision
	\return true on success, false if printing interrupted
	\note If false is returned all printing should be stopped
*/
static bool print(CColorConsole& out, const CRevFile& rev)
{
	stringstream outStr;
	
	out << kBlue << "----------------------------" << kNormal << kNL;
	out << "Revision : " << kBold << kRed << rev.RevNum().c_str() << kNormal << kNL;
	
	if( !rev.Locker().empty() )
		out << "Locked by : '" << kBold << rev.Locker().c_str() << kNormal << '\'' << kNL;

	outStr << (rev.RevTime().tm_year + 1900) << '/' <<
		(rev.RevTime().tm_mon + 1) << '/' <<
		rev.RevTime().tm_mday << ' ' <<
		rev.RevTime().tm_hour << ':' <<
		rev.RevTime().tm_min << ':' <<
		rev.RevTime().tm_sec;
	out << "Date : " << kBold << outStr.str().c_str() << kNormal << kNL;

	out << "Author : '" << kBold << rev.Author().c_str() << kNormal << '\'' << kNL;
	out << "State : '" << kBold << rev.State().c_str() << kNormal << '\'' << kNL;
	out << "Lines : +" << kBold << rev.ChgPos() << ' ' << rev.ChgNeg() << kNormal << kNL;

	if( !rev.BranchesList().empty() )
	{
		out << "Branches :" << kNL;

		std::vector<CRevNumber>::const_iterator s;
		for(s = rev.BranchesList().begin(); s != rev.BranchesList().end(); ++s)
		{
			if( TestCancel() )
			{
				return false;
			}

			const CRevNumber& branch = *s;
			out << '\t' << kBold << branch.c_str() << kNormal << kNL;
		}
	}
	
	if( !rev.KeywordSubst().empty() )
		out << "Keyword : '" << kBold << rev.KeywordSubst().c_str() << kNormal << '\'' << kNL;
	
	if( !rev.CommitID().empty() )
		out << "CommitID : '" << kBold << rev.CommitID().c_str() << kNormal << '\'' << kNL;

	if( !rev.MergePoint().empty() )
		out << "MergePoint : '" << kBold << rev.MergePoint().c_str() << kNormal << '\'' << kNL;

	if( !rev.Filename().empty() )
		out << "Filename : '" << kBold << rev.Filename().c_str() << kNormal << '\'' << kNL;

	out << "Description :" << kNL << kBrown;

#ifdef qMacCvsPP
  if ( sCommitMsgEncoding == kMsgEncodingMacOSRoman ) out << rev.DescLog();
  else out << ConvertString(rev.DescLog(), sCommitMsgEncoding == kMsgEncodingUTF8 ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1, kCFStringEncodingMacRoman, true);
#else
	out << rev.DescLog().c_str();
#endif

	out << kNormal << kNL;

	return true;
}

/*!
	Print the file RCS information
	\param out Console
	\param rcs RCS file
	\param recurse true for recursive, false otherwise
	\return true on success, false if printing interrupted
	\note If false is returned all printing should be stopped
*/
static bool print(CColorConsole& out, const CRcsFile& rcs, bool recurse = true)
{
	out << "Rcs file : '" << kRed << rcs.RcsFile().c_str() << kNormal << '\'' << kNL;
	out << "Working file : '" << kBold << rcs.WorkingFile().c_str() << kNormal << '\'' << kNL;
	out << "Head revision : " << kBold << kRed << rcs.HeadRev().c_str() << kNormal << kNL;
	out << "Branch revision : " << kBold << rcs.BranchRev().c_str() << kNormal << kNL;

	out << "Locks :" << kBold << (rcs.LockStrict() ? " strict" : "") << kNormal << kNL;

	std::vector<CRevNumber>::const_iterator s;
	for(s = rcs.LocksList().begin(); s != rcs.LocksList().end(); ++s)
	{
		if( TestCancel() )
		{
			return false;
		}

		const CRevNumber & lock = *s;
		out << '\t' << kBold << lock.c_str() << " : '" << lock.Tag().c_str() << '\'' << kNormal << kNL;
	}

	out << "Access :" << kNL;

	std::vector<std::string>::const_iterator a;
	for(a = rcs.AccessList().begin(); a != rcs.AccessList().end(); ++a)
	{
		if( TestCancel() )
		{
			return false;
		}

		out << "\t'" << kBold << (*a).c_str() << kNormal << '\'' << kNL;
	}

	if( !rcs.SymbolicList().empty() )
	{
		out << "Symbolic names :" << kNL;

		std::vector<CRevNumber>::const_iterator n;
		for(n = rcs.SymbolicList().begin(); n != rcs.SymbolicList().end(); ++n)
		{
			if( TestCancel() )
			{
				return false;
			}
			
			const CRevNumber & symb = *n;
			out << '\t' << kBold << symb.c_str() << " : '" << symb.Tag().c_str() << '\'' << kNormal << kNL;
		}
	}

	out << "Keyword substitution : '" << kBold << rcs.KeywordSubst().c_str() << kNormal << '\'' << kNL;
	out << "Total revisions : " << kBold << rcs.TotRevisions() << kNormal << kNL;
	out << "Selected revisions : " << kBold << rcs.SelRevisions() << kNormal << kNL;
	out << "Description :" << kNL << kBrown << rcs.DescLog().c_str() << kNormal << kNL;

	if( recurse )  
	{
		std::vector<CRevFile>::const_iterator i;
		for(i = rcs.AllRevs().begin(); i != rcs.AllRevs().end(); ++i)
		{
			if( TestCancel() )
			{
				return false;
			}
			else
			{
				if( !print(out, *i) )
				{
					return false;
				}
			}
		}
	}

	out << kGreen << "===============================================" << kNormal << kNL;

	return true;
}

/*!
	Print the log node information
	\param out Console
	\param node Log node
	\param offset Offset string
	\return true on success, false if printing interrupted
	\note If false is returned all printing should be stopped
*/
static bool print(CColorConsole& out, const CLogNode* node, const std::string& offset)
{
	out << offset.c_str();
	switch(node->GetType())
	{
	case kNodeHeader :
		{
			CLogNodeHeader *header = (CLogNodeHeader*)node;
			out << offset.c_str() << "Rcs file : '" << kRed << (**header).RcsFile().c_str() << kNormal << '\'' << kNL;
		}
		break;
	case kNodeBranch :
		break;
	case kNodeRev :
		{
			CLogNodeRev *rev = (CLogNodeRev*)node;
			out << offset.c_str() << kBold << (**rev).RevNum().c_str() << kNormal << kNL;
		}
		break;
	case kNodeTag :
		break;
	}

	string newOffset;
	newOffset = offset;
	newOffset += "    ";

	std::vector<CLogNode *>::const_iterator i;
	for(i = node->Childs().begin(); i != node->Childs().end(); ++i)
	{
		if( TestCancel() )
		{
			return false;
		}

		if( !print(out, *i, newOffset) )
		{
			return false;
		}

		newOffset += ' ';
	}

	if( node->Next() != 0L )
	{
		if( !print(out, node->Next(), offset) )
		{
			return false;
		}
	}

	return true;
}

/*!
	Print the file node string
	\param out Console
	\param prefix Node information prefix
	\param nodeStr Node string
	\param separator true to print the separator line, false otherwise
	\return true on success, false if printing interrupted
	\note If false is returned all printing should be stopped
*/
static bool print(CColorConsole& out, const char* prefix, const char* nodeStr, bool separator = true)
{
	if( TestCancel() )
	{
		return false;
	}

	if( separator )
	{
		out << kBlue << "----------------------------" << kNormal << kNL;
	}

	out << prefix << " : " << kBold << nodeStr << kNormal << kNL;

	return true;
}

/*!
	Print a syntax colored of the CVS log node
	\param out Console
	\param node Log node
*/
void CvsLogOutput(CColorConsole& out, CLogNode* node)
{
	switch(node->GetType())
	{
	case kNodeHeader:
		{
			const CRcsFile& rcs = **(CLogNodeHeader*)node;
			print(out, rcs, false);
		}
		break;
	case kNodeBranch:
		{
			const std::string& str = **(CLogNodeBranch*)node;
			print(out, "Branch", str.c_str());
		}
		break;
	case kNodeRev:
		{
			const CRevFile& rev = **(CLogNodeRev*)node;
			print(out, rev);
		}
		break;
	case kNodeTag:
		{
			const std::string& str = **(CLogNodeTag*)node;
			print(out, "Tag", str.c_str());
		}
		break;
	}
}

/*!
	Launch a CVS log command, parse the output, output a graph or a syntax colored of the CVS log output
	\param dir Directory
	\param args Arguments
	\param outGraph true if the output is for graph, false if for the log command
	\param defWnd Default window
*/
void CvsLogParse(const char* dir, const CvsArgs& args, const bool outGraph /*= false*/, void* defWnd /*= 0L*/)
{
	// Make a stream of the cvs log output
	string tmpFile;
	FILE* output = launchCVS(dir, args, tmpFile);
	if( output == 0L )
		return;

	// Parse
	std::vector<CRcsFile>& allRcs = CvsLogParse(output);

	// Maybe something went wrong or may be the user
	// asked to print only the RCS names, so print the
	// output directly without parsing../
	if( allRcs.empty() )
	{
		char* line = 0L;
		size_t line_chars_allocated = 0;
		int line_length = 0;

		fseek(output, SEEK_SET, 0);
		
		while( (line_length = getline(&line, &line_chars_allocated, output)) > 0 )
		{
			cvs_outstr(line, line_length);
			cvs_outstr("\n", 1);
		}

		if( line_length < 0 && !feof(output) )
			cvs_err("Cannot read output file (error %d)\n", errno);

		if( line != 0L )
			free(line);
	}

	// Close the temp file
	fclose(output);

#ifdef WIN32
	if( wnt_unlink(tmpFile.c_str()) != 0 )
#else
	if( unlink(tmpFile.c_str()) != 0 )
#endif
	{
		cvs_err("Impossible to delete the file '%s' (error %d)\n", tmpFile.c_str(), errno);
	}

	if( allRcs.empty() )
		return;

	// Output the result
	CColorConsole out;

	std::vector<CRcsFile>::iterator i;
	for(i = allRcs.begin(); i != allRcs.end(); ++i)
	{
		CRcsFile& rcs = *i;
		
		if( !outGraph )
		{
			if( !print(out, rcs) )
			{
				break;
			}
		}
		else
		{
			CLogNode* node = CvsLogGraph(rcs);
			if( node == 0L )
			{
				cvs_err("Impossible to create a graph for %s\n", rcs.WorkingFile().c_str());
			}

#ifdef WIN32
			CWincvsApp* app = (CWincvsApp*)AfxGetApp();
			CGraphDoc* doc;

			if( defWnd != 0L )
				doc = (CGraphDoc*)defWnd;
			else
				doc = (CGraphDoc*)app->GetLogGraph()->OpenDocumentFile(NULL);

			doc->SetNode(node, dir);
#elif qMacCvsPP
			if( defWnd != 0L )
				((CTreeView*)defWnd)->SetNode(node, dir);
			else
				CMacCvsApp::gApp->NewLogTree(node, dir);
#elif qUnix
			UCvsGraph* graph = new UCvsGraph(node, dir);
			void* win = UCreate_GraphWindow();

			UEventSendMessage(graph->GetWidID(), EV_INIT_WIDGET, kUMainWidget, win);
			UEventSendMessage(graph->GetWidID(), EV_SHOW_WIDGET, UMAKEINT(kUMainWidget, 1), 0L);
#else
			CStr offset;
			print(out, node, offset);
			delete node;
#endif
		}
	}

	// Reset the parser
	CvsLogReset();
}
