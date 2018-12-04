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

#include <string.h>
#include "common.h"
#include "parser.h"

// Current line number
int currentLineNumber = 1;

// token expected by the parser
kTokens expectToken = kNone;

CRevNumber revnumber;

extern "C" int yywrap();
extern "C" int yywrap() { BEGIN INITIAL; return 1; }
int  readNumber( int offset );
void reset( void );
void incLineNumber( void );
static void parse_revision_number__(const char* text, size_t len);
%}

	/* Normal state:  parsing nodes.  The initial start state is used */
	/* only to recognize the VRML header. */
%x NODE

	/* Start tokens for all of the field types, */
	/* except for MFNode and SFNode, which are almost completely handled */
	/* by the parser: */
%x ST_STRING ST_TAG ST_REV ST_REVORNIL ST_INREV ST_REVLISTS ST_INT
%x ST_ANYBUTSEP ST_DATE ST_AUTHOR_VALUE ST_KEYWORD ST_ID ST_STATE ST_FILENAME

	/* integer */
int ([\+-]?[0-9]+)

	/* positive integer */
date_int ([\+]?[0-9]+)

	/* Whitespace. */
ws ([ \t\r\n]+)
	/* And the same pattern without the newline */
wsnnl ([ \t\r])

	/* a keyword string */
keyword (b|B|t|u|\{[^\}]*\})?([cklLovz]*)

	/* an id */
id ([[:alnum:]]*)

	/* a string */
str ([^ \t\r\n][^\t\r\n]*)

	/* a tag string */
tag_str ([^ \t\n\r:;])+

	/* a state string */
state_str ([^ \t\r\n:;][^\t\r:;]*)

	/* a author string */
author ([^ \t\r\n:;][^\t\r\n:;]*)

	/* a filename string */
filename ([^ \t\r\n][^\t\r\n]*)

	/*revfollow (----------------------------)*/
revfollow ^(\-{28}\r?\nrevision)

	/*endfollow (=============================================================================)*/
endfollow ^(\={77})$
revision_number_branch_part (\.[0-9]+)
revision_number ([0-9]+{revision_number_branch_part}+)

	/*Should we contain newline?*/
revision_number_list_item {wsnnl}*{revision_number}[;]
%%

%{
	/* Switch into a new start state if the parser */
	/* just told us that we've read a field name */
	/* and should expect a field value (or IS) */
#if qCvsDebug
	if (yy_flex_debug)
		fprintf(stderr,"LEX--> Start State %d\n", expectToken);
#endif
	switch(expectToken)
	{
	case kNone: BEGIN NODE; break;
	case kString: BEGIN ST_STRING; break;
	case kRevnum: BEGIN ST_REV; break;
	case kRevnumOrNil: BEGIN ST_REVORNIL; break;
	case kID: BEGIN ST_ID; break;
	case kFilename: BEGIN ST_FILENAME; break;
	case kInt: BEGIN ST_INT; break;
	case kDate: BEGIN ST_DATE; break;
	case kRev: BEGIN ST_REV; break;
	case kKeyword: BEGIN ST_KEYWORD; break;
	case kAuthorValue: BEGIN ST_AUTHOR_VALUE; break;
	case kAnyButSep: BEGIN ST_ANYBUTSEP; break;
	case kState: BEGIN ST_STATE; break;
	case kTag: BEGIN ST_TAG; break;
	case kRevList: BEGIN ST_REVLISTS; break;
	default: yyerror("ACK: Bad expectToken"); break;
	}
%}

<INITIAL>{ws}*	{ BEGIN NODE; }

<NODE>RCS\ file:				{ return ID_RCS; }
<NODE>Working\ file:			{ return ID_WORKING; }
<NODE>head:						{ return ID_HEAD; }
<NODE>branch:					{ return ID_BRANCH; }
<NODE>locks:					{ return ID_LOCKS; }
<NODE>access\ list:				{ return ID_ACCESS; }
<NODE>symbolic\ names:			{ return ID_SYMBOLIC; }
<NODE>keyword\ substitution:	{ return ID_KEYWORD; }
<NODE>total\ revisions:			{ return ID_TOTREVISIONS; }
<NODE>selected\ revisions:		{ return ID_SELREVISIONS; }
<NODE>description:				{ return ID_DESCRIPTION; }
<ST_REV>revision				{ return ID_REVISION; }

T_ID
<NODE>date:						{ return ID_DATE; }
<NODE>author:					{ return ID_AUTHOR; }
<NODE>state:					{ return ID_STATE; }
<NODE>branches:					{ return ID_BRANCHES; }
<NODE>strict					{ return ID_STRICT; }
<NODE>lines:					{ return ID_LINES; }
<NODE>kopt:						{ return ID_KOPT; }
<NODE>commitid:					{ return ID_COMMITID; }
<NODE>mergepoint:				{ return ID_MERGEPOINT; }
<NODE>filename:					{ return ID_FILENAME; }
<NODE>locked\ by:				{ return ID_LOCKEDBY; }

<NODE>{endfollow}				{ return ID_ENDFOLLOW; }

<ST_STATE>{state_str}		{ yylval_set(yytext); reset(); return STRING; }
<ST_TAG>{tag_str}		{ yylval_set(yytext); reset(); return STRING; }
<NODE>{tag_str}				{ yylval_set(yytext); return TAG; }
<ST_FILENAME>{filename}						{ yytext[yyleng-1] = '\0'; yylval_set(yytext); reset(); return STRING; }
<ST_ID>{id}						{ yylval_set(yytext); reset(); return STRING; }

<ST_KEYWORD>{keyword}			{ yylval_set(yytext); reset(); return STRING; }

<ST_AUTHOR_VALUE>{author}		{ yylval_set(yytext); reset(); return STRING; }
<NODE,ST_ANYBUTSEP>{revfollow}		{         yyless(28); reset(); return ID_REVFOLLOW; }
<ST_ANYBUTSEP>{endfollow}		{                     reset(); return ID_ENDFOLLOW; }
<ST_ANYBUTSEP>{str}				{ yylval_set(yytext); return STRING; }
<ST_ANYBUTSEP>\n				{ incLineNumber(); yylval_set((int)(unsigned char)yytext[0]); return ID_CHAR; }
<ST_ANYBUTSEP>.					{ yylval_set((int)(unsigned char)yytext[0]); return ID_CHAR; }

<ST_STRING>{str}				{ yylval_set(yytext); reset(); return STRING; }

<ST_REVLISTS>{revision_number_list_item}	{ parse_revision_number__(yytext, yyleng - 1); return REVNUMLISTITEM; }
<ST_REVLISTS>.				{ yyless(0); reset(); }
<ST_REV,ST_REVORNIL>{int}		{ revnumber.reset(); readNumber(0); BEGIN ST_INREV; expectToken = kNone;}
<ST_INREV>\.{int}				{                    readNumber(1); BEGIN ST_INREV; expectToken = kNone;}
<ST_REVORNIL>[^ \t\r\n]			{ yyless(0); /* put back the spaces */	reset();                        return REVNUM; }
<ST_INREV>.						{ yyless(0); /* put back the spaces */	reset(); yylval_set(revnumber);	return REVNUM; }

<ST_INT>{int}					{ int v=readNumber(0); reset(); yylval_set(v); return INTEGER; }

	/*
	 *settings for date strings
	 */
<NODE>[\+]{date_int};					{ return ';'; }
<ST_DATE>{date_int}					{ int v=readNumber(0);          yylval_set(v); return INTEGER; }
<ST_DATE>[-]					{ return '/'; } // change '-' in dates to '/'
<ST_DATE>[/:]					{ return yytext[0]; }

								/* Whitespace and catch-all rules apply to all start states: */
<*>{wsnnl}+						;

								/* This is also whitespace, but we'll keep track of line number */
								/* to report in errors: */
<*>{wsnnl}*\n{wsnnl}*			{
								  incLineNumber();
	
								  if(expectToken == kAnyButSep)
								  {
									// strange bug, flex is sometimes getting here in
									// a kAnyButSep mode, while it should not
									yylval_set(yytext);
									return STRING;
								  }
								}

				/* This catch-all rule catches anything not covered by any of */
				/* the above: */
<*>. 			{ yylval_set((int)(unsigned char)yytext[0]); return (int)(unsigned char)yytext[0]; }

%%

/* offset needed by '<ST_INREV>\.{int}' to ignore leading '.' */
int readNumber(int offset)
{
	int v;
	if(sscanf(yytext + offset, "%d", &v) == 1)
		revnumber += v;
	else
		yyerror("%s not an integer value", yytext);
		
	return v;
}

#include <algorithm>
#include <cctype>
static std::pair<int, const char*> atoi__(const char* first__, const char* last__)
{
	while (isspace(*first__)) {
		first__++;
	}
	int c = *first__++;
	int total = 0;
	for (; isdigit(c); c = *first__++) {
		total = 10 * total + (c - '0');
		if (first__ == last__) {
			break;
		}
	}
	return std::make_pair(total, first__);
}

static void parse_revision_number__(const char* text, size_t len)
{
	const char* first = text;
	const char* const end = first + len;
	const char* last = end;
	CRevNumber revnumber_tmp;
	while (first < end)
	{
		std::pair<int, const char*> v = atoi__(first, last = std::find(first, end, '.'));
		revnumber_tmp += v.first;
		first = ++last;
	}
	yylval_set(revnumber_tmp);
}

void reset()
{
	BEGIN NODE; 
	expectToken = kNone;	
}

void incLineNumber()
{
	++currentLineNumber;
#if qCvsDebug
	if (yy_flex_debug)
		fprintf(stderr,"LINE--> %d\n", currentLineNumber);
#endif
}
