/* A Bison parser, made by GNU Bison 1.875b.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     STRING = 259,
     TAG = 260,
     REVNUM = 261,
     REVNUMLISTITEM = 262,
     ID_CHAR = 263,
     ID_DESCRIPTION = 264,
     ID_TOTREVISIONS = 265,
     ID_SELREVISIONS = 266,
     ID_KEYWORD = 267,
     ID_SYMBOLIC = 268,
     ID_ACCESS = 269,
     ID_LOCKS = 270,
     ID_STRICT = 271,
     ID_BRANCH = 272,
     ID_HEAD = 273,
     ID_WORKING = 274,
     ID_RCS = 275,
     ID_REVISION = 276,
     ID_DATE = 277,
     ID_BRANCHES = 278,
     ID_REVFOLLOW = 279,
     ID_ENDFOLLOW = 280,
     ID_LINES = 281,
     ID_ANYBUTSEP = 282,
     ID_LOCKEDBY = 283,
     ID_AUTHOR = 284,
     ID_STATE = 285,
     ID_COMMITID = 286,
     ID_KOPT = 287,
     ID_MERGEPOINT = 288,
     ID_FILENAME = 289
   };
#endif
#define INTEGER 258
#define STRING 259
#define TAG 260
#define REVNUM 261
#define REVNUMLISTITEM 262
#define ID_CHAR 263
#define ID_DESCRIPTION 264
#define ID_TOTREVISIONS 265
#define ID_SELREVISIONS 266
#define ID_KEYWORD 267
#define ID_SYMBOLIC 268
#define ID_ACCESS 269
#define ID_LOCKS 270
#define ID_STRICT 271
#define ID_BRANCH 272
#define ID_HEAD 273
#define ID_WORKING 274
#define ID_RCS 275
#define ID_REVISION 276
#define ID_DATE 277
#define ID_BRANCHES 278
#define ID_REVFOLLOW 279
#define ID_ENDFOLLOW 280
#define ID_LINES 281
#define ID_ANYBUTSEP 282
#define ID_LOCKEDBY 283
#define ID_AUTHOR 284
#define ID_STATE 285
#define ID_COMMITID 286
#define ID_KOPT 287
#define ID_MERGEPOINT 288
#define ID_FILENAME 289




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 60 "parser.yy"
typedef union YYSTYPE {
	char *string;
	int aint;
} YYSTYPE;
/* Line 1252 of yacc.c.  */
#line 110 "parser.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



