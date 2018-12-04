#ifndef COMMON_H
#define COMMON_H

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

#ifdef WIN32
#	include <malloc.h> // for alloca
#endif /* WIN32 */

#include "CvsLog.h"

// reset all static/glocal variables
void yyreset(void);

// used to communicate values lexer->parser
void yylval_set(const char *str);
void yylval_set(int i);
void yylval_set(const CRevNumber & rev);

// token expected by the parser
typedef enum
{
	kNone,
	kString,
	kInt,
	kRev,
	kDate,
	kRevnum,
	kRevnumOrNil,
	kKeyword,
	kID,
	kFilename,
	kState,
	kTag,
	kAuthorValue,
	kAnyButSep,
	kRevList
} kTokens;
extern kTokens expectToken;

// error while parsing
void yyerror(const char *txt, ...);
extern int currentLineNumber;

// missing prototypes
int yylex(void);
extern FILE *yyin;
extern void yyrestart(FILE *);
extern int yyparse(void);

// tells the lexer to take from the stream widely
#define YY_NEVER_INTERACTIVE 1

#if qCvsDebug
	extern int yydebug;
#endif
extern int yy_flex_debug;

// the result is stored there
extern std::vector<CRcsFile> gRcsFiles;

// the current RCS file and the current revision parsed
extern CRcsFile *gRcsFile;
extern CRevFile *gRevFile;

#ifdef TARGET_OS_MAC
	extern void *cvstree_alloca(unsigned);
	extern char *cvstree_strdup(const char *);
#	define strdup cvstree_strdup
#	define alloca cvstree_alloca
#endif /* TARGET_OS_MAC */

#endif /* COMMON_H */
