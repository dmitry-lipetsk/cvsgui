%{
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "common.h"

#if qCvsDebug
#	define YYDEBUG 1
#else
#	define YYDEBUG 0
#endif

#ifdef __MWERKS__
	using namespace std;
#endif

static void expect(kTokens type);

static int sInt;
static std::string sStr;
static CRevNumber sRev;

static void printDebug(const char *tag, const char* value, bool setEndl)
{
#if YYDEBUG != 0
	if (yydebug) {
		printf("%s%s%s", tag, value, setEndl ? "\n" : "");
	}
#endif
}

static void printDebug(const char *tag, int value, bool setEndl)
{
#if YYDEBUG != 0
	if (yydebug) {
		printf("%s%d%s", tag, value, setEndl ? "\n" : "");
	}
#endif
}

%}

%union {
	char *string;
	int aint;
};

%token <aint> INTEGER
%token <string> STRING
%token <string> TAG
%token <revnum> REVNUM
%token <revnum> REVNUMLISTITEM
%token <aint> ID_CHAR
%token <aint> error

%token ID_DESCRIPTION
%token ID_TOTREVISIONS
%token ID_SELREVISIONS
%token ID_KEYWORD
%token ID_SYMBOLIC
%token ID_ACCESS
%token ID_LOCKS
%token ID_STRICT
%token ID_BRANCH
%token ID_HEAD
%token ID_WORKING
%token ID_RCS
%token ID_REVISION
%token ID_DATE
%token ID_BRANCHES
%token ID_REVFOLLOW
%token ID_ENDFOLLOW
%token ID_LINES
%token ID_ANYBUTSEP
%token ID_CHAR
%token ID_LOCKEDBY
%token ID_AUTHOR
%token ID_STATE
%token ID_COMMITID
%token ID_KOPT
%token ID_MERGEPOINT
%token ID_FILENAME

%%

cvslog:		allDeclarations
	;

allDeclarations:
		/* Empty is OK */
	|	allDeclarations allDeclaration
	;

allDeclaration:
		rcsDeclarations
	|	ID_REVFOLLOW		{
			if(gRcsFile != 0L)
			{
				gRcsFile->AllRevs().push_back(CRevFile());
				gRevFile = &gRcsFile->LastRev();
			}

			expect (kRev);
		}
		allrevDeclarations

	|	ID_ENDFOLLOW	{ gRcsFile = 0L; expect(kNone); }
	|	error 						{
										int c = $1;
										yyclearin;
										expect(kAnyButSep);
										if(gRevFile != 0L)
											gRevFile->DescLog() += (char)c;
										else if(gRcsFile != 0L)
											gRcsFile->DescLog() += (char)c;
									}
			anyDeclarations
	|	tagDeclaration				{
										expect(kAnyButSep);
										if(gRevFile != 0L)
											gRevFile->DescLog() += sStr;
										else if(gRcsFile != 0L)
											gRcsFile->DescLog() += sStr;

										printDebug("Desp ", sStr.c_str(), true);
    }
			anyDeclarations
	;

rcsDeclarations:
	|	rcsDeclarations rcsDeclaration
	;

rcsDeclaration:
		rcsFileDeclaration
	|	workingFileDeclaration
	|	headRevDeclaration
	|	branchRevDeclaration
	|	locksDeclaration
	|	accessDeclaration
	|	symbolicNamesDeclaration
	|	keyDeclaration
	|	totRevDeclaration
	|	descDeclaration
	;

anyDeclarations:
		/* Empty is OK */
	|	anyDeclarations anyDeclaration
	;

anyDeclaration:
		/* Empty is OK */
	|	ID_CHAR			{
							char c = $1;
							if(gRevFile != 0L)
								gRevFile->DescLog() += c;
							else if(gRcsFile != 0L)
								gRcsFile->DescLog() += c;
						}
	|	STRING			{
							if(gRevFile != 0L)
								gRevFile->DescLog() += $1;
							else if(gRcsFile != 0L)
								gRcsFile->DescLog() += $1;
							free($1);
						}
	;

descDeclaration:
		ID_DESCRIPTION 					{ expect(kAnyButSep); }
			descListDeclarations
	;

descListDeclarations:
		/* Empty is OK */
	|	descListDeclarations descListDeclaration
	;

descListDeclaration:
		/* Empty is OK */
	|	ID_CHAR		{
			char c = $1;
			if(gRcsFile != 0L)
				gRcsFile->DescLog() += c;
		}
	|	STRING		{
			if(gRcsFile != 0L)
				gRcsFile->DescLog() += $1;
			free($1);
		}
	;

allrevDeclarations: revRevision revOtherDeclarations revBranches
 	;
	
revOtherDeclarations:
	|	revOtherDeclaration
	|	revOtherDeclarations ';' revOtherDeclaration
	;

revBranches:
	|	ID_BRANCHES     { expect(kRevList); }
		revnumListDeclarations
		;

revRevision:
		ID_REVISION 				{ expect(kRevnum); }
		REVNUM					{
			if(gRevFile != 0L)
			{
				printDebug("\nRevNum ", sRev.c_str(), true);

				gRevFile->RevNum() = sRev;
			}
			sRev.reset();
		}
		;
		
revOtherDeclaration:	
		|	ID_DATE		{ expect(kDate); }
		intDeclaration '/'	{
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_year = sInt - 1900;
			}
		}
		intDeclaration '/'	{
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_mon = sInt ? sInt - 1 : 0;
			}
		}
		intDeclaration		{
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_mday = sInt;
			}
		}
		intDeclaration ':'	{
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_hour = sInt;
			}
		}
		intDeclaration ':'	{
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_min = sInt;
			}
		}
		intDeclaration		{
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_sec = sInt;
			}

			expect(kNone);
			
			if(gRevFile != 0L)
			{
				printDebug("Date ", asctime(&(gRevFile->RevTime())), false);
			}
		}

	|	ID_AUTHOR	{ expect(kAuthorValue); }
		stringDeclaration	{
			if(gRevFile != 0L)
			{
				gRevFile->Author() = sStr;
				printDebug("Author ", sStr.c_str(), true);
			}
		}

	|	ID_LOCKEDBY 
		tagDeclaration {
			if(gRevFile != 0L)
			{
				gRevFile->Locker() = sStr;
				printDebug("Locker ", sStr.c_str(), true);
			}
		}
	|	ID_STATE	{ expect(kState); }
		stringDeclaration {
			if(gRevFile != 0L)
			{
				gRevFile->State() = sStr;
                printDebug("State ", sStr.c_str(), true);
			}
		}
		
	|	ID_LINES 	{ expect(kInt); }
		intDeclaration		{
			expect(kInt);
			if(gRevFile != 0L)
			{
				gRevFile->ChgPos() = sInt;
				printDebug("Lines +", sInt, false);
			}
		}
		intDeclaration		{
			if(gRevFile != 0L)
			{
				gRevFile->ChgNeg() = sInt;
				printDebug(" ", sInt, true);
			}
		}
		
	|	ID_KOPT	{ expect(kKeyword); }
		stringDeclaration	{
			if(gRevFile != 0L)
			{
				gRevFile->KeywordSubst() = sStr;
				printDebug("KeywordSubst ", sStr.c_str(), true);
			}			
		}
		
	|	ID_COMMITID	{ expect(kID); }
		stringDeclaration {
			if(gRevFile != 0L)
			{
				gRevFile->CommitID() = sStr;
				printDebug("CommitID ", sStr.c_str(), true);
			}
		}
	|	ID_MERGEPOINT	{ expect(kRevnum); }
		REVNUM		{
			if(gRevFile != 0L)
			{
				gRevFile->MergePoint() = sRev;
				printDebug("MergePoint ",sRev.c_str(),true);
			}
		}
	|	ID_FILENAME	{ expect(kFilename); }
		stringDeclaration {
			if(gRevFile != 0L)
			{
				gRevFile->Filename() = sStr;
				printDebug("Filename ", sStr.c_str(), true);
			}
		}
	;

revnumListDeclarations:
		/* Empty is OK */
	|	revnumListDeclarations revnumListDeclaration
	;

revnumListDeclaration:
		/* Empty is OK */
	|	REVNUMLISTITEM 	{
			if(gRevFile != 0L)
			{
				gRevFile->BranchesList().push_back(sRev);
			}

			sRev.reset();
			expect(kRevList);
		}
	;

stringDeclaration:
		STRING		{ sStr = $1; free($1); }
	;

tagDeclaration:
		TAG		{ sStr = $1; free($1); }
	;

intDeclaration:
		INTEGER		{ sInt = $1; }
	;

totRevDeclaration:
		ID_TOTREVISIONS		{ expect(kInt); }
		intDeclaration		{
			if(gRcsFile != 0L)
			{
				gRcsFile->TotRevisions() = sInt;
			}
		}
		selRevDeclaration
	;

selRevDeclaration:
		/* Empty is OK */
	|	';' ID_SELREVISIONS 	{ expect(kInt); }
		intDeclaration		{
			if(gRcsFile != 0L)
			{
				gRcsFile->SelRevisions() = sInt;
			}
		}
	;

keyDeclaration:
		ID_KEYWORD 	{ expect(kKeyword); }
		stringDeclaration	{
			if(gRcsFile != 0L)
			{
				gRcsFile->KeywordSubst() = sStr;
			}
		}
	;

symbolicNamesDeclaration:
		ID_SYMBOLIC  symbolicListDeclarations
	;

symbolicListDeclarations:
		/* Empty is OK */
	|	symbolicListDeclarations symbolicListDeclaration
	;

symbolicListDeclaration:
		tagDeclaration ':'	{
			expect(kRevnum);
			if(gRcsFile != 0L)
			{
				CRevNumber newRev;
				newRev.Tag() = sStr;
				gRcsFile->SymbolicList().push_back(newRev);
			}
		}
		REVNUM			{
			if(gRcsFile != 0L)
			{
				sRev.Tag() = gRcsFile->LastSymbName().Tag();
				gRcsFile->LastSymbName() = sRev;
			}
			sRev.reset();
		}
	;

accessDeclaration:
		ID_ACCESS  accessListDeclarations
	;

accessListDeclarations:
		/* Empty is OK */
	|	accessListDeclarations accessListDeclaration
	;

accessListDeclaration:
		tagDeclaration 		{
			if(gRcsFile != 0L)
			{
				std::string newAccess;
				newAccess = sStr;
				gRcsFile->AccessList().push_back(newAccess);
			}
		}
		tagDeclaration
	|	tagDeclaration		{
			if(gRcsFile != 0L)
			{
				std::string newAccess;
				newAccess = sStr;
				gRcsFile->AccessList().push_back(newAccess);
			}
		}
	;

locksDeclaration:
		ID_LOCKS 		{
			if(gRcsFile != 0L)
			{
				gRcsFile->LockStrict() = false;
			}
		}
		locksListDeclarations
	|	ID_LOCKS ID_STRICT	{
			if(gRcsFile != 0L)
			{
				gRcsFile->LockStrict() = true;
			}
		}
		locksListDeclarations
	;

locksListDeclarations:
		/* Empty is OK */
	|	locksListDeclarations locksListDeclaration
	;

locksListDeclaration:
		tagDeclaration ':'	{
			expect(kRevnum);
			if(gRcsFile != 0L)
			{
				CRevNumber newRev;
				newRev.Tag() = sStr;
				gRcsFile->LocksList().push_back(newRev);
			}
		}
		REVNUM			{
			if(gRcsFile != 0L)
			{
				sRev.Tag() = gRcsFile->LastLock().Tag();
				gRcsFile->LastLock() = sRev;
			}
			sRev.reset();
		}
	;

branchRevDeclaration:
		ID_BRANCH 		{ expect(kRevnumOrNil); }
		REVNUM			{
			if(gRcsFile != 0L)
			{
				gRcsFile->BranchRev() = sRev;
				printDebug("Branch :", sRev.c_str(), true);
			}
			sRev.reset();
		}
	;

headRevDeclaration:
		ID_HEAD 		{ expect(kRevnum); }
		REVNUM 			{
			if(gRcsFile != 0L)
			{
				gRcsFile->HeadRev() = sRev;
				printDebug("Head :", sRev.c_str(), true);
			}
			sRev.reset();
		}
	;

workingFileDeclaration:
		ID_WORKING 		{ expect(kString); }
		stringDeclaration	{
			if(gRcsFile != 0L)
			{
				gRcsFile->WorkingFile() = sStr;
				printDebug("WorkingFile :", sStr.c_str(), true);
			}
		}
	;

rcsFileDeclaration:
		ID_RCS 			{ expect(kString); }
		stringDeclaration	{
			// push a new rcs file
			gRcsFiles.push_back(CRcsFile());
			gRcsFile = &gRcsFiles[gRcsFiles.size() - 1];
			gRcsFile->RcsFile() = sStr;
		}
	;
%%

void yyerror(const char * /*txt*/, ...)
{
	expect(kAnyButSep);
}

static void expect(kTokens type)
{
	expectToken = type;
}

void yylval_set(const char *txt)
{
	yylval.string = strdup(txt);
}

void yylval_set(int anint)
{
	yylval.aint = anint;
}

void yylval_set(const CRevNumber & rev)
{
	sRev = rev;
}

void yyreset(void)
{
	currentLineNumber = 1;
	sStr = "";
	sInt = 0;
	sRev.reset();
	gRcsFiles.erase(gRcsFiles.begin(), gRcsFiles.end());
	gRcsFile = 0L;
	gRevFile = 0L;
	expect(kNone);
}
