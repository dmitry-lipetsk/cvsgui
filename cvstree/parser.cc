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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "parser.yy"

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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 60 "parser.yy"
typedef union YYSTYPE {
	char *string;
	int aint;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 208 "parser.cc"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 220 "parser.cc"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   97

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  70
/* YYNRULES -- Number of rules. */
#define YYNRULES  111
/* YYNRULES -- Number of states. */
#define YYNSTATES  143

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   289

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    36,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    35,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    12,    16,    18,
      19,    23,    24,    28,    29,    32,    34,    36,    38,    40,
      42,    44,    46,    48,    50,    52,    53,    56,    57,    59,
      61,    62,    66,    67,    70,    71,    73,    75,    79,    80,
      82,    86,    87,    88,    92,    93,    97,    98,    99,   100,
     101,   102,   103,   104,   122,   123,   127,   130,   131,   135,
     136,   137,   143,   144,   148,   149,   153,   154,   158,   159,
     163,   164,   167,   168,   170,   172,   174,   176,   177,   178,
     184,   185,   186,   191,   192,   196,   199,   200,   203,   204,
     209,   212,   213,   216,   217,   221,   223,   224,   228,   229,
     234,   235,   238,   239,   244,   245,   249,   250,   254,   255,
     259,   260
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      39,     0,    -1,    40,    -1,    -1,    40,    41,    -1,    45,
      -1,    -1,    24,    42,    53,    -1,    25,    -1,    -1,     1,
      43,    47,    -1,    -1,    77,    44,    47,    -1,    -1,    45,
      46,    -1,   106,    -1,   104,    -1,   102,    -1,   100,    -1,
      94,    -1,    90,    -1,    86,    -1,    84,    -1,    79,    -1,
      49,    -1,    -1,    47,    48,    -1,    -1,     8,    -1,     4,
      -1,    -1,     9,    50,    51,    -1,    -1,    51,    52,    -1,
      -1,     8,    -1,     4,    -1,    57,    54,    55,    -1,    -1,
      59,    -1,    54,    35,    59,    -1,    -1,    -1,    23,    56,
      74,    -1,    -1,    21,    58,     6,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    22,    60,    78,    36,    61,    78,
      36,    62,    78,    63,    78,    37,    64,    78,    37,    65,
      78,    -1,    -1,    29,    66,    76,    -1,    28,    77,    -1,
      -1,    30,    67,    76,    -1,    -1,    -1,    26,    68,    78,
      69,    78,    -1,    -1,    32,    70,    76,    -1,    -1,    31,
      71,    76,    -1,    -1,    33,    72,     6,    -1,    -1,    34,
      73,    76,    -1,    -1,    74,    75,    -1,    -1,     7,    -1,
       4,    -1,     5,    -1,     3,    -1,    -1,    -1,    10,    80,
      78,    81,    82,    -1,    -1,    -1,    35,    11,    83,    78,
      -1,    -1,    12,    85,    76,    -1,    13,    87,    -1,    -1,
      87,    88,    -1,    -1,    77,    37,    89,     6,    -1,    14,
      91,    -1,    -1,    91,    92,    -1,    -1,    77,    93,    77,
      -1,    77,    -1,    -1,    15,    95,    97,    -1,    -1,    15,
      16,    96,    97,    -1,    -1,    97,    98,    -1,    -1,    77,
      37,    99,     6,    -1,    -1,    17,   101,     6,    -1,    -1,
      18,   103,     6,    -1,    -1,    19,   105,    76,    -1,    -1,
      20,   107,    76,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   103,   103,   106,   108,   112,   113,   113,   124,   125,
     125,   135,   135,   147,   148,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   164,   166,   169,   171,   178,
     188,   188,   192,   194,   197,   199,   204,   211,   214,   215,
     216,   219,   220,   220,   225,   225,   237,   238,   239,   245,
     251,   257,   263,   238,   283,   283,   292,   300,   300,   309,
     310,   309,   326,   326,   335,   335,   343,   343,   351,   351,
     361,   363,   366,   368,   380,   384,   388,   392,   393,   392,
     402,   404,   404,   414,   414,   424,   427,   429,   433,   433,
     453,   456,   458,   462,   462,   471,   482,   482,   489,   489,
     498,   500,   504,   504,   524,   524,   536,   536,   548,   548,
     559,   559
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "STRING", "TAG", "REVNUM", 
  "REVNUMLISTITEM", "ID_CHAR", "ID_DESCRIPTION", "ID_TOTREVISIONS", 
  "ID_SELREVISIONS", "ID_KEYWORD", "ID_SYMBOLIC", "ID_ACCESS", "ID_LOCKS", 
  "ID_STRICT", "ID_BRANCH", "ID_HEAD", "ID_WORKING", "ID_RCS", 
  "ID_REVISION", "ID_DATE", "ID_BRANCHES", "ID_REVFOLLOW", "ID_ENDFOLLOW", 
  "ID_LINES", "ID_ANYBUTSEP", "ID_LOCKEDBY", "ID_AUTHOR", "ID_STATE", 
  "ID_COMMITID", "ID_KOPT", "ID_MERGEPOINT", "ID_FILENAME", "';'", "'/'", 
  "':'", "$accept", "cvslog", "allDeclarations", "allDeclaration", "@1", 
  "@2", "@3", "rcsDeclarations", "rcsDeclaration", "anyDeclarations", 
  "anyDeclaration", "descDeclaration", "@4", "descListDeclarations", 
  "descListDeclaration", "allrevDeclarations", "revOtherDeclarations", 
  "revBranches", "@5", "revRevision", "@6", "revOtherDeclaration", "@7", 
  "@8", "@9", "@10", "@11", "@12", "@13", "@14", "@15", "@16", "@17", 
  "@18", "@19", "@20", "revnumListDeclarations", "revnumListDeclaration", 
  "stringDeclaration", "tagDeclaration", "intDeclaration", 
  "totRevDeclaration", "@21", "@22", "selRevDeclaration", "@23", 
  "keyDeclaration", "@24", "symbolicNamesDeclaration", 
  "symbolicListDeclarations", "symbolicListDeclaration", "@25", 
  "accessDeclaration", "accessListDeclarations", "accessListDeclaration", 
  "@26", "locksDeclaration", "@27", "@28", "locksListDeclarations", 
  "locksListDeclaration", "@29", "branchRevDeclaration", "@30", 
  "headRevDeclaration", "@31", "workingFileDeclaration", "@32", 
  "rcsFileDeclaration", "@33", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,    59,    47,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    38,    39,    40,    40,    41,    42,    41,    41,    43,
      41,    44,    41,    45,    45,    46,    46,    46,    46,    46,
      46,    46,    46,    46,    46,    47,    47,    48,    48,    48,
      50,    49,    51,    51,    52,    52,    52,    53,    54,    54,
      54,    55,    56,    55,    58,    57,    59,    60,    61,    62,
      63,    64,    65,    59,    66,    59,    59,    67,    59,    68,
      69,    59,    70,    59,    71,    59,    72,    59,    73,    59,
      74,    74,    75,    75,    76,    77,    78,    80,    81,    79,
      82,    83,    82,    85,    84,    86,    87,    87,    89,    88,
      90,    91,    91,    93,    92,    92,    95,    94,    96,    94,
      97,    97,    99,    98,   101,   100,   103,   102,   105,   104,
     107,   106
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     2,     1,     0,     3,     1,     0,
       3,     0,     3,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     2,     0,     1,     1,
       0,     3,     0,     2,     0,     1,     1,     3,     0,     1,
       3,     0,     0,     3,     0,     3,     0,     0,     0,     0,
       0,     0,     0,    17,     0,     3,     2,     0,     3,     0,
       0,     5,     0,     3,     0,     3,     0,     3,     0,     3,
       0,     2,     0,     1,     1,     1,     1,     0,     0,     5,
       0,     0,     4,     0,     3,     2,     0,     2,     0,     4,
       2,     0,     2,     0,     3,     1,     0,     3,     0,     4,
       0,     2,     0,     4,     0,     3,     0,     3,     0,     3,
       0,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     0,     0,     1,     9,    75,     6,     8,     4,     5,
      11,    25,     0,    30,    77,    83,    86,    91,    96,   104,
     106,   108,   110,    14,    24,    23,    22,    21,    20,    19,
      18,    17,    16,    15,    25,    10,    44,     7,    38,    32,
       0,     0,    85,    90,    98,   100,     0,     0,     0,     0,
      12,    29,    28,    26,     0,    47,    59,     0,    54,    57,
      64,    62,    66,    68,    41,    39,    31,    76,    78,    74,
      84,     0,    87,    95,    92,   100,    97,   105,   107,   109,
     111,    45,     0,     0,    56,     0,     0,     0,     0,     0,
       0,    42,    46,    37,    36,    35,    33,    80,    88,     0,
      99,     0,   101,     0,    60,    55,    58,    65,    63,    67,
      69,    70,    40,     0,    79,     0,    94,   102,    48,     0,
      43,    81,    89,     0,     0,    61,    73,    71,     0,   103,
       0,    82,    49,     0,    50,     0,     0,    51,     0,     0,
      52,     0,    53
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     2,     8,    12,    11,    34,     9,    23,    35,
      53,    24,    39,    66,    96,    37,    64,    93,   111,    38,
      54,    65,    82,   124,   133,   135,   138,   141,    85,    86,
      83,   119,    88,    87,    89,    90,   120,   127,    70,   101,
      68,    25,    40,    97,   114,   128,    26,    41,    27,    42,
      72,   115,    28,    43,    74,    99,    29,    45,    75,    76,
     102,   123,    30,    46,    31,    47,    32,    48,    33,    49
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -82
static const yysigned_char yypact[] =
{
     -82,     7,     4,   -82,   -82,   -82,   -82,   -82,   -82,    62,
     -82,   -82,   -13,   -82,   -82,   -82,   -82,   -82,     9,   -82,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,
     -82,   -82,   -82,   -82,   -82,     2,   -82,   -82,    36,   -82,
      17,    23,    26,    26,   -82,   -82,    27,    28,    23,    23,
       2,   -82,   -82,   -82,    29,   -82,   -82,    26,   -82,   -82,
     -82,   -82,   -82,   -82,   -20,   -82,    22,   -82,   -82,   -82,
     -82,    -5,   -82,    31,   -82,   -82,    26,   -82,   -82,   -82,
     -82,   -82,    17,    17,   -82,    23,    23,    23,    23,    33,
      23,   -82,    36,   -82,   -82,   -82,   -82,    10,   -82,    26,
      26,     0,   -82,     6,   -82,   -82,   -82,   -82,   -82,   -82,
     -82,   -82,   -82,    35,   -82,    38,   -82,   -82,   -82,    17,
      49,   -82,   -82,    53,    17,   -82,   -82,   -82,    17,   -82,
      25,   -82,   -82,    17,   -82,    17,    41,   -82,    17,    46,
     -82,    17,   -82
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,    39,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,
     -82,   -29,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -37,    -2,
     -81,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,    11,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -94
static const short yytable[] =
{
      10,   103,   104,    91,    -2,     4,    51,     3,    36,     5,
      52,    79,    80,   -13,   -13,    92,   -13,   -13,   -13,   -13,
      67,   -13,   -13,   -13,   -13,    44,    94,    69,     6,     7,
      95,     5,    98,    77,    78,    81,   -93,   117,   125,   109,
      71,    73,   118,   130,   122,   113,   121,   131,   105,   106,
     107,   108,   134,   110,   136,    84,   126,   139,    55,   129,
     142,   132,    56,   112,    57,    58,    59,    60,    61,    62,
      63,    13,    14,    50,    15,    16,    17,    18,   137,    19,
      20,    21,    22,   140,     0,     0,   100,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   116
};

static const short yycheck[] =
{
       2,    82,    83,    23,     0,     1,     4,     0,    21,     5,
       8,    48,    49,     9,    10,    35,    12,    13,    14,    15,
       3,    17,    18,    19,    20,    16,     4,     4,    24,    25,
       8,     5,    37,     6,     6,     6,     5,    37,   119,     6,
      42,    43,    36,   124,     6,    35,    11,   128,    85,    86,
      87,    88,   133,    90,   135,    57,     7,   138,    22,     6,
     141,    36,    26,    92,    28,    29,    30,    31,    32,    33,
      34,     9,    10,    34,    12,    13,    14,    15,    37,    17,
      18,    19,    20,    37,    -1,    -1,    75,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    99
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    39,    40,     0,     1,     5,    24,    25,    41,    45,
      77,    43,    42,     9,    10,    12,    13,    14,    15,    17,
      18,    19,    20,    46,    49,    79,    84,    86,    90,    94,
     100,   102,   104,   106,    44,    47,    21,    53,    57,    50,
      80,    85,    87,    91,    16,    95,   101,   103,   105,   107,
      47,     4,     8,    48,    58,    22,    26,    28,    29,    30,
      31,    32,    33,    34,    54,    59,    51,     3,    78,     4,
      76,    77,    88,    77,    92,    96,    97,     6,     6,    76,
      76,     6,    60,    68,    77,    66,    67,    71,    70,    72,
      73,    23,    35,    55,     4,     8,    52,    81,    37,    93,
      97,    77,    98,    78,    78,    76,    76,    76,    76,     6,
      76,    56,    59,    35,    82,    89,    77,    37,    36,    69,
      74,    11,     6,    99,    61,    78,     7,    75,    83,     6,
      78,    78,    36,    62,    78,    63,    78,    37,    64,    78,
      37,    65,    78
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 6:
#line 113 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				gRcsFile->AllRevs().push_back(CRevFile());
				gRevFile = &gRcsFile->LastRev();
			}

			expect (kRev);
		}
    break;

  case 8:
#line 124 "parser.yy"
    { gRcsFile = 0L; expect(kNone); }
    break;

  case 9:
#line 125 "parser.yy"
    {
										int c = yyvsp[0].aint;
										yyclearin;
										expect(kAnyButSep);
										if(gRevFile != 0L)
											gRevFile->DescLog() += (char)c;
										else if(gRcsFile != 0L)
											gRcsFile->DescLog() += (char)c;
									}
    break;

  case 11:
#line 135 "parser.yy"
    {
										expect(kAnyButSep);
										if(gRevFile != 0L)
											gRevFile->DescLog() += sStr;
										else if(gRcsFile != 0L)
											gRcsFile->DescLog() += sStr;

										printDebug("Desp ", sStr.c_str(), true);
    }
    break;

  case 28:
#line 171 "parser.yy"
    {
							char c = yyvsp[0].aint;
							if(gRevFile != 0L)
								gRevFile->DescLog() += c;
							else if(gRcsFile != 0L)
								gRcsFile->DescLog() += c;
						}
    break;

  case 29:
#line 178 "parser.yy"
    {
							if(gRevFile != 0L)
								gRevFile->DescLog() += yyvsp[0].string;
							else if(gRcsFile != 0L)
								gRcsFile->DescLog() += yyvsp[0].string;
							free(yyvsp[0].string);
						}
    break;

  case 30:
#line 188 "parser.yy"
    { expect(kAnyButSep); }
    break;

  case 35:
#line 199 "parser.yy"
    {
			char c = yyvsp[0].aint;
			if(gRcsFile != 0L)
				gRcsFile->DescLog() += c;
		}
    break;

  case 36:
#line 204 "parser.yy"
    {
			if(gRcsFile != 0L)
				gRcsFile->DescLog() += yyvsp[0].string;
			free(yyvsp[0].string);
		}
    break;

  case 42:
#line 220 "parser.yy"
    { expect(kRevList); }
    break;

  case 44:
#line 225 "parser.yy"
    { expect(kRevnum); }
    break;

  case 45:
#line 226 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				printDebug("\nRevNum ", sRev.c_str(), true);

				gRevFile->RevNum() = sRev;
			}
			sRev.reset();
		}
    break;

  case 47:
#line 238 "parser.yy"
    { expect(kDate); }
    break;

  case 48:
#line 239 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_year = sInt - 1900;
			}
		}
    break;

  case 49:
#line 245 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_mon = sInt ? sInt - 1 : 0;
			}
		}
    break;

  case 50:
#line 251 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_mday = sInt;
			}
		}
    break;

  case 51:
#line 257 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_hour = sInt;
			}
		}
    break;

  case 52:
#line 263 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->RevTime().tm_min = sInt;
			}
		}
    break;

  case 53:
#line 269 "parser.yy"
    {
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
    break;

  case 54:
#line 283 "parser.yy"
    { expect(kAuthorValue); }
    break;

  case 55:
#line 284 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->Author() = sStr;
				printDebug("Author ", sStr.c_str(), true);
			}
		}
    break;

  case 56:
#line 293 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->Locker() = sStr;
				printDebug("Locker ", sStr.c_str(), true);
			}
		}
    break;

  case 57:
#line 300 "parser.yy"
    { expect(kState); }
    break;

  case 58:
#line 301 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->State() = sStr;
                printDebug("State ", sStr.c_str(), true);
			}
		}
    break;

  case 59:
#line 309 "parser.yy"
    { expect(kInt); }
    break;

  case 60:
#line 310 "parser.yy"
    {
			expect(kInt);
			if(gRevFile != 0L)
			{
				gRevFile->ChgPos() = sInt;
				printDebug("Lines +", sInt, false);
			}
		}
    break;

  case 61:
#line 318 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->ChgNeg() = sInt;
				printDebug(" ", sInt, true);
			}
		}
    break;

  case 62:
#line 326 "parser.yy"
    { expect(kKeyword); }
    break;

  case 63:
#line 327 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->KeywordSubst() = sStr;
				printDebug("KeywordSubst ", sStr.c_str(), true);
			}			
		}
    break;

  case 64:
#line 335 "parser.yy"
    { expect(kID); }
    break;

  case 65:
#line 336 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->CommitID() = sStr;
				printDebug("CommitID ", sStr.c_str(), true);
			}
		}
    break;

  case 66:
#line 343 "parser.yy"
    { expect(kRevnum); }
    break;

  case 67:
#line 344 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->MergePoint() = sRev;
				printDebug("MergePoint ",sRev.c_str(),true);
			}
		}
    break;

  case 68:
#line 351 "parser.yy"
    { expect(kFilename); }
    break;

  case 69:
#line 352 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->Filename() = sStr;
				printDebug("Filename ", sStr.c_str(), true);
			}
		}
    break;

  case 73:
#line 368 "parser.yy"
    {
			if(gRevFile != 0L)
			{
				gRevFile->BranchesList().push_back(sRev);
			}

			sRev.reset();
			expect(kRevList);
		}
    break;

  case 74:
#line 380 "parser.yy"
    { sStr = yyvsp[0].string; free(yyvsp[0].string); }
    break;

  case 75:
#line 384 "parser.yy"
    { sStr = yyvsp[0].string; free(yyvsp[0].string); }
    break;

  case 76:
#line 388 "parser.yy"
    { sInt = yyvsp[0].aint; }
    break;

  case 77:
#line 392 "parser.yy"
    { expect(kInt); }
    break;

  case 78:
#line 393 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				gRcsFile->TotRevisions() = sInt;
			}
		}
    break;

  case 81:
#line 404 "parser.yy"
    { expect(kInt); }
    break;

  case 82:
#line 405 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				gRcsFile->SelRevisions() = sInt;
			}
		}
    break;

  case 83:
#line 414 "parser.yy"
    { expect(kKeyword); }
    break;

  case 84:
#line 415 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				gRcsFile->KeywordSubst() = sStr;
			}
		}
    break;

  case 88:
#line 433 "parser.yy"
    {
			expect(kRevnum);
			if(gRcsFile != 0L)
			{
				CRevNumber newRev;
				newRev.Tag() = sStr;
				gRcsFile->SymbolicList().push_back(newRev);
			}
		}
    break;

  case 89:
#line 442 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				sRev.Tag() = gRcsFile->LastSymbName().Tag();
				gRcsFile->LastSymbName() = sRev;
			}
			sRev.reset();
		}
    break;

  case 93:
#line 462 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				std::string newAccess;
				newAccess = sStr;
				gRcsFile->AccessList().push_back(newAccess);
			}
		}
    break;

  case 95:
#line 471 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				std::string newAccess;
				newAccess = sStr;
				gRcsFile->AccessList().push_back(newAccess);
			}
		}
    break;

  case 96:
#line 482 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				gRcsFile->LockStrict() = false;
			}
		}
    break;

  case 98:
#line 489 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				gRcsFile->LockStrict() = true;
			}
		}
    break;

  case 102:
#line 504 "parser.yy"
    {
			expect(kRevnum);
			if(gRcsFile != 0L)
			{
				CRevNumber newRev;
				newRev.Tag() = sStr;
				gRcsFile->LocksList().push_back(newRev);
			}
		}
    break;

  case 103:
#line 513 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				sRev.Tag() = gRcsFile->LastLock().Tag();
				gRcsFile->LastLock() = sRev;
			}
			sRev.reset();
		}
    break;

  case 104:
#line 524 "parser.yy"
    { expect(kRevnumOrNil); }
    break;

  case 105:
#line 525 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				gRcsFile->BranchRev() = sRev;
				printDebug("Branch :", sRev.c_str(), true);
			}
			sRev.reset();
		}
    break;

  case 106:
#line 536 "parser.yy"
    { expect(kRevnum); }
    break;

  case 107:
#line 537 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				gRcsFile->HeadRev() = sRev;
				printDebug("Head :", sRev.c_str(), true);
			}
			sRev.reset();
		}
    break;

  case 108:
#line 548 "parser.yy"
    { expect(kString); }
    break;

  case 109:
#line 549 "parser.yy"
    {
			if(gRcsFile != 0L)
			{
				gRcsFile->WorkingFile() = sStr;
				printDebug("WorkingFile :", sStr.c_str(), true);
			}
		}
    break;

  case 110:
#line 559 "parser.yy"
    { expect(kString); }
    break;

  case 111:
#line 560 "parser.yy"
    {
			// push a new rcs file
			gRcsFiles.push_back(CRcsFile());
			gRcsFile = &gRcsFiles[gRcsFiles.size() - 1];
			gRcsFile->RcsFile() = sStr;
		}
    break;


    }

/* Line 999 of yacc.c.  */
#line 1801 "parser.cc"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 567 "parser.yy"


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

