// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
/* A Bison parser, made by GNU Bison 1.875c.  */

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
     CLASS = 258,
     STRUCT = 259,
     UNION = 260,
     ENUM = 261,
     FUNTYPE = 262,
     STATIC = 263,
     TEMPLATE = 264,
     CONST = 265,
     TYPEDEF = 266,
     TA_TYPEDEF = 267,
     TYPE = 268,
     NUMBER = 269,
     FUNCTION = 270,
     ARRAY = 271,
     NAME = 272,
     COMMENT = 273,
     FUNCALL = 274,
     SCOPER = 275,
     EQUALS = 276,
     PUBLIC = 277,
     PRIVATE = 278,
     PROTECTED = 279,
     OPERATOR = 280,
     FRIEND = 281,
     THISNAME = 282,
     REGFUN = 283,
     VIRTUAL = 284
   };
#endif
#define CLASS 258
#define STRUCT 259
#define UNION 260
#define ENUM 261
#define FUNTYPE 262
#define STATIC 263
#define TEMPLATE 264
#define CONST 265
#define TYPEDEF 266
#define TA_TYPEDEF 267
#define TYPE 268
#define NUMBER 269
#define FUNCTION 270
#define ARRAY 271
#define NAME 272
#define COMMENT 273
#define FUNCALL 274
#define SCOPER 275
#define EQUALS 276
#define PUBLIC 277
#define PRIVATE 278
#define PROTECTED 279
#define OPERATOR 280
#define FRIEND 281
#define THISNAME 282
#define REGFUN 283
#define VIRTUAL 284




/* Copy the first part of user declarations.  */
#line 41 "mta_parse.y"


#include "maketa.h"

#if defined(SUN4) && !defined(__GNUG__) && !defined(SOLARIS)
#include <alloca.h>
#endif

#if defined(SGI) || defined(SGIdebug)
#include <alloca.h>
#endif

static String_PArray bogus_inh_opts;

#define SETDESC(ty,cm)	mta->SetDesc(cm, ty->desc, ty->inh_opts, ty->opts, ty->lists)

#define SETENUMDESC(ty,cm) mta->SetDesc(cm, ty->desc, bogus_inh_opts, ty->opts, ty->lists)

void yyerror(char *s);
int yylex();



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 67 "mta_parse.y"
typedef struct YYSTYPE {
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  char*    	chr;
  int	   	rval;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 166 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 178 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
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
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
struct yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (struct yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
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
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   492

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  45
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  83
/* YYNRULES -- Number of rules. */
#define YYNRULES  203
/* YYNRULES -- Number of states. */
#define YYNSTATES  336

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    43,     2,
      31,    32,    30,    39,    36,    40,    42,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    35,    44,
      38,     2,    37,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    34,     2,    33,    41,     2,     2,     2,
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
      25,    26,    27,    28,    29
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    19,    22,
      25,    28,    30,    32,    34,    37,    40,    45,    49,    54,
      57,    65,    70,    72,    77,    80,    84,    88,    91,    93,
      95,    98,   102,   104,   107,   111,   115,   118,   122,   126,
     128,   132,   135,   138,   140,   142,   144,   148,   150,   153,
     155,   158,   160,   163,   165,   167,   169,   172,   176,   180,
     183,   187,   191,   197,   199,   202,   207,   209,   214,   218,
     221,   223,   226,   228,   231,   233,   236,   238,   240,   244,
     246,   250,   254,   256,   258,   260,   262,   265,   267,   269,
     271,   273,   276,   278,   280,   283,   286,   289,   291,   293,
     296,   300,   303,   308,   313,   315,   319,   321,   326,   328,
     332,   337,   342,   347,   353,   359,   364,   368,   373,   378,
     384,   389,   391,   394,   398,   401,   406,   413,   417,   420,
     422,   424,   426,   429,   432,   435,   439,   441,   444,   448,
     450,   452,   455,   458,   462,   464,   467,   469,   473,   478,
     480,   483,   485,   488,   492,   499,   501,   503,   506,   510,
     513,   515,   517,   520,   522,   524,   526,   529,   531,   533,
     536,   538,   541,   543,   546,   548,   551,   554,   558,   562,
     566,   570,   573,   575,   580,   585,   587,   590,   592,   596,
     598,   600,   602,   605,   607,   609,   611,   614,   616,   618,
     620,   622,   624,   626
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      46,     0,    -1,    -1,    46,    48,    -1,    46,    52,    -1,
      46,    56,    -1,    46,    58,    -1,    46,    68,    -1,    46,
      75,    -1,    46,    47,    -1,    46,     1,    -1,    12,    -1,
      49,    -1,    50,    -1,    50,    18,    -1,    11,    51,    -1,
      11,    60,   120,   124,    -1,   114,   113,   124,    -1,   114,
      18,   113,   124,    -1,   114,   124,    -1,   114,    31,    30,
     113,    32,   101,   124,    -1,   114,    20,    30,   113,    -1,
      53,    -1,    54,    78,    33,   124,    -1,    55,    34,    -1,
      55,    18,    34,    -1,    55,    34,    18,    -1,     6,   113,
      -1,     6,    -1,    57,    -1,    63,   124,    -1,    63,   124,
      18,    -1,    59,    -1,    60,   124,    -1,    60,   123,   124,
      -1,    61,    85,    33,    -1,    62,    34,    -1,    62,    18,
      34,    -1,    62,    34,    18,    -1,    63,    -1,    63,    35,
      64,    -1,   127,   113,    -1,   127,    13,    -1,   127,    -1,
     126,    -1,    65,    -1,    64,    36,    65,    -1,   114,    -1,
      66,   114,    -1,    17,    -1,    66,    17,    -1,    67,    -1,
      66,    67,    -1,   125,    -1,    29,    -1,    69,    -1,    70,
     124,    -1,    70,   123,   124,    -1,    71,    85,    33,    -1,
      72,    34,    -1,    72,    18,    34,    -1,    72,    34,    18,
      -1,     9,    73,    74,    37,    62,    -1,    38,    -1,     3,
     113,    -1,    74,    36,     3,   113,    -1,    76,    -1,    28,
     112,    77,    98,    -1,    28,    77,    98,    -1,    97,   101,
      -1,    79,    -1,    78,    79,    -1,    80,    -1,    80,    18,
      -1,    81,    -1,    81,    36,    -1,     1,    -1,    84,    -1,
      84,    21,    82,    -1,    83,    -1,    82,    39,    83,    -1,
      82,    40,    83,    -1,    17,    -1,    14,    -1,    17,    -1,
      86,    -1,    85,    86,    -1,    87,    -1,    93,    -1,    53,
      -1,    50,    -1,    60,   124,    -1,     1,    -1,    88,    -1,
      22,    35,    -1,    23,    35,    -1,    24,    35,    -1,    18,
      -1,    89,    -1,     8,    89,    -1,   111,    90,   124,    -1,
      90,   124,    -1,   111,    91,    16,   124,    -1,   111,    92,
     101,   124,    -1,    91,    -1,    90,    36,    91,    -1,    17,
      -1,    31,    30,    17,    32,    -1,    94,    -1,   108,   102,
      98,    -1,   121,   108,   102,    98,    -1,   108,   102,   106,
      98,    -1,    41,   108,   102,    98,    -1,   121,    41,   108,
     102,    98,    -1,    26,   112,    25,   101,    98,    -1,    26,
      25,   101,    98,    -1,    26,   112,   124,    -1,    26,     3,
     112,   124,    -1,    26,     3,    17,   124,    -1,    26,   112,
      97,   101,    98,    -1,    26,    97,   101,    98,    -1,    95,
      -1,     8,    95,    -1,   111,    96,    98,    -1,    96,    98,
      -1,   111,    25,   101,    98,    -1,   111,    25,    31,    32,
     101,    98,    -1,    25,   101,    98,    -1,    97,   101,    -1,
      17,    -1,   124,    -1,    99,    -1,   100,   124,    -1,    18,
     124,    -1,    18,    99,    -1,    18,   100,   124,    -1,    15,
      -1,    10,    15,    -1,    10,    18,    15,    -1,    21,    -1,
      10,    -1,    10,    21,    -1,    31,    32,    -1,    31,   103,
      32,    -1,    32,    -1,   103,    32,    -1,   104,    -1,   103,
      36,   104,    -1,   103,    42,    42,    42,    -1,   105,    -1,
     105,    21,    -1,   114,    -1,   114,    17,    -1,   114,    17,
      16,    -1,   114,    31,    30,    17,    32,   101,    -1,    17,
      -1,   107,    -1,   106,   107,    -1,   109,    13,    19,    -1,
      27,    31,    -1,    35,    -1,    30,    -1,   110,    30,    -1,
     112,    -1,   114,    -1,   121,    -1,   121,   114,    -1,    17,
      -1,   115,    -1,   115,    43,    -1,   116,    -1,   116,   110,
      -1,   117,    -1,    10,   117,    -1,   118,    -1,   126,   118,
      -1,   126,   113,    -1,    13,    20,    17,    -1,    27,    20,
      17,    -1,    13,    20,    13,    -1,    27,    20,    13,    -1,
      20,    13,    -1,    27,    -1,    13,    73,   119,    37,    -1,
      27,    73,   119,    37,    -1,    13,    -1,   118,    13,    -1,
      13,    -1,   119,    36,    13,    -1,    17,    -1,    13,    -1,
     122,    -1,   121,   122,    -1,     7,    -1,    29,    -1,    17,
      -1,    17,    16,    -1,    44,    -1,    22,    -1,    23,    -1,
      24,    -1,     4,    -1,     5,    -1,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   136,   136,   137,   140,   142,   145,   148,   150,   152,
     154,   159,   162,   170,   171,   175,   176,   180,   185,   192,
     222,   225,   230,   236,   239,   240,   241,   244,   248,   255,
     263,   264,   268,   276,   277,   281,   287,   290,   292,   297,
     298,   301,   306,   310,   316,   325,   328,   336,   337,   338,
     339,   342,   343,   346,   347,   351,   359,   360,   364,   370,
     373,   375,   380,   387,   391,   392,   395,   401,   403,   407,
     412,   413,   416,   417,   420,   421,   422,   427,   430,   436,
     437,   442,   450,   451,   455,   458,   459,   462,   471,   484,
     487,   490,   494,   498,   499,   500,   501,   502,   522,   523,
     529,   530,   531,   539,   543,   547,   553,   559,   564,   565,
     566,   567,   568,   569,   570,   571,   572,   573,   574,   575,
     583,   594,   595,   599,   600,   601,   602,   603,   606,   613,
     617,   618,   619,   620,   621,   622,   626,   627,   628,   632,
     633,   634,   637,   638,   641,   642,   645,   646,   647,   650,
     652,   660,   664,   668,   672,   676,   683,   684,   687,   691,
     695,   698,   699,   702,   705,   706,   707,   710,   713,   714,
     724,   725,   733,   734,   743,   744,   745,   746,   750,   754,
     755,   756,   757,   758,   775,   781,   782,   792,   793,   796,
     797,   800,   801,   804,   805,   808,   809,   812,   815,   816,
     817,   821,   822,   826
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CLASS", "STRUCT", "UNION", "ENUM",
  "FUNTYPE", "STATIC", "TEMPLATE", "CONST", "TYPEDEF", "TA_TYPEDEF",
  "TYPE", "NUMBER", "FUNCTION", "ARRAY", "NAME", "COMMENT", "FUNCALL",
  "SCOPER", "EQUALS", "PUBLIC", "PRIVATE", "PROTECTED", "OPERATOR",
  "FRIEND", "THISNAME", "REGFUN", "VIRTUAL", "'*'", "'('", "')'", "'}'",
  "'{'", "':'", "','", "'>'", "'<'", "'+'", "'-'", "'~'", "'.'", "'&'",
  "';'", "$accept", "list", "preparsed", "typedefn", "typedefns",
  "typedsub", "defn", "enumdefn", "enumdsub", "enumname", "enumnm",
  "classdecl", "classdecls", "classdefn", "classdefns", "classdsub",
  "classname", "classhead", "classnm", "classinh", "classpar", "classptyp",
  "classpmod", "templdefn", "templdefns", "templdsub", "templname",
  "templhead", "templopen", "templpars", "fundecl", "funnm", "regfundefn",
  "enums", "enumline", "enumitm", "enumitms", "enummath", "enummathitm",
  "enmitmname", "membs", "membline", "membdefn", "basicmemb", "nostatmemb",
  "membnames", "membname", "membfunp", "methdefn", "basicmeth",
  "nostatmeth", "mbfundefn", "methname", "fundefn", "funsubdefn",
  "funsubdecl", "funargs", "constfun", "args", "argdefn", "subargdefn",
  "constrlist", "constref", "consthsnm", "constcoln", "ptrs", "membtype",
  "ftype", "tyname", "type", "noreftype", "constype", "subtype",
  "combtype", "templtypes", "tdname", "funtspec", "funtsmod", "varname",
  "term", "access", "structstruct", "classkeyword", 0
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
      42,    40,    41,   125,   123,    58,    44,    62,    60,    43,
      45,   126,    46,    38,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    45,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    47,    48,    49,    49,    50,    50,    51,    51,    51,
      51,    51,    52,    53,    54,    54,    54,    55,    55,    56,
      57,    57,    58,    59,    59,    60,    61,    61,    61,    62,
      62,    63,    63,    63,    63,    64,    64,    65,    65,    65,
      65,    66,    66,    67,    67,    68,    69,    69,    70,    71,
      71,    71,    72,    73,    74,    74,    75,    76,    76,    77,
      78,    78,    79,    79,    80,    80,    80,    81,    81,    82,
      82,    82,    83,    83,    84,    85,    85,    86,    86,    86,
      86,    86,    86,    87,    87,    87,    87,    87,    88,    88,
      89,    89,    89,    89,    90,    90,    91,    92,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    94,    94,    95,    95,    95,    95,    95,    96,    97,
      98,    98,    98,    98,    98,    98,    99,    99,    99,   100,
     100,   100,   101,   101,   102,   102,   103,   103,   103,   104,
     104,   105,   105,   105,   105,   105,   106,   106,   107,   108,
     109,   110,   110,   111,   112,   112,   112,   113,   114,   114,
     115,   115,   116,   116,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   118,   118,   119,   119,   120,
     120,   121,   121,   122,   122,   123,   123,   124,   125,   125,
     125,   126,   126,   127
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     1,     1,     2,     2,     4,     3,     4,     2,
       7,     4,     1,     4,     2,     3,     3,     2,     1,     1,
       2,     3,     1,     2,     3,     3,     2,     3,     3,     1,
       3,     2,     2,     1,     1,     1,     3,     1,     2,     1,
       2,     1,     2,     1,     1,     1,     2,     3,     3,     2,
       3,     3,     5,     1,     2,     4,     1,     4,     3,     2,
       1,     2,     1,     2,     1,     2,     1,     1,     3,     1,
       3,     3,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     2,     1,     1,     2,     2,     2,     1,     1,     2,
       3,     2,     4,     4,     1,     3,     1,     4,     1,     3,
       4,     4,     4,     5,     5,     4,     3,     4,     4,     5,
       4,     1,     2,     3,     2,     4,     6,     3,     2,     1,
       1,     1,     2,     2,     2,     3,     1,     2,     3,     1,
       1,     2,     2,     3,     1,     2,     1,     3,     4,     1,
       2,     1,     2,     3,     6,     1,     1,     2,     3,     2,
       1,     1,     2,     1,     1,     1,     2,     1,     1,     2,
       1,     2,     1,     2,     1,     2,     2,     3,     3,     3,
       3,     2,     1,     4,     4,     1,     2,     1,     3,     1,
       1,     1,     2,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,    10,   203,   201,   202,    28,     0,     0,
      11,     0,     9,     3,    12,    13,     4,    22,     0,     0,
       5,    29,     6,    32,     0,     0,     0,    39,     7,    55,
       0,     0,     0,     8,    66,    44,    43,   167,    27,    63,
       0,     0,   185,     0,   182,    15,     0,    39,     0,   168,
     170,   172,   174,    44,   193,   129,   194,     0,     0,     0,
     164,   165,   191,     0,    14,    76,    84,     0,    70,    72,
      74,    77,     0,    24,   195,   197,     0,    33,    92,     0,
     106,    97,     0,     0,     0,     0,     0,   182,     0,    90,
      89,     0,     0,    85,    87,    93,    98,     0,   104,    88,
     108,   121,     0,     0,     0,     0,   163,   165,     0,    36,
       0,    30,     0,    56,     0,     0,    59,    42,    41,     0,
       0,   173,     0,     0,   181,     0,     0,   190,   189,     0,
       0,     0,     0,     0,    19,   169,   161,   171,   186,   185,
     176,   175,   140,   136,     0,   139,    68,   131,     0,   130,
       0,    69,     0,   166,   192,     0,    71,    73,    75,     0,
      25,    26,   196,    34,    99,   122,    94,    95,    96,     0,
       0,     0,     0,     0,   159,     0,     0,    91,    35,    86,
       0,   101,   124,   128,   155,   144,     0,     0,   146,   149,
     151,     0,     0,     0,   104,     0,     0,     0,     0,    37,
      38,    49,   198,   199,   200,    54,    40,    45,     0,    51,
      47,    53,    31,    57,    58,    60,    61,    64,     0,     0,
     179,   177,   187,     0,   180,   178,     0,    16,     0,     0,
       0,    17,   162,   137,     0,   141,   134,     0,   133,   132,
     142,     0,    67,    23,    83,    82,    78,    79,   127,     0,
       0,     0,     0,     0,     0,   116,     0,   106,   105,   160,
     109,     0,   156,     0,   145,     0,     0,   150,   152,     0,
       0,     0,     0,   100,     0,     0,   123,     0,     0,     0,
      50,    52,    48,     0,    62,     0,   183,   184,    18,    21,
       0,   138,   135,   143,     0,     0,   118,   117,   115,   120,
       0,     0,   112,   111,   157,     0,   147,     0,   153,     0,
     142,   125,     0,   102,   103,     0,   110,    46,    65,   188,
       0,    80,    81,   114,   119,   158,   148,     0,     0,   107,
     113,     0,     0,   126,    20,   154
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,    12,    13,    14,    89,    45,    16,    90,    18,
      19,    20,    21,    22,    23,    91,    25,    26,    47,   206,
     207,   208,   209,    28,    29,    30,    31,    32,   126,   120,
      33,    34,    57,    67,    68,    69,    70,   246,   247,    71,
      92,    93,    94,    95,    96,    97,    98,   195,    99,   100,
     101,   102,   103,   146,   147,   148,   151,   186,   187,   188,
     189,   261,   262,   104,   263,   137,   105,   106,   140,    60,
      49,    50,    51,    52,   223,   129,    61,    62,    76,   149,
     211,    63,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -166
static const short yypact[] =
{
    -166,   235,  -166,  -166,  -166,  -166,  -166,     6,   -13,   463,
    -166,   416,  -166,  -166,  -166,    93,  -166,  -166,    26,    46,
    -166,  -166,  -166,  -166,    14,   133,    63,    86,  -166,  -166,
      14,   133,    74,  -166,  -166,  -166,   135,  -166,  -166,  -166,
      81,   102,    -1,   100,    66,  -166,   154,   110,   201,    92,
     117,  -166,   141,   185,  -166,  -166,  -166,    72,   130,   171,
    -166,   445,  -166,   185,  -166,  -166,  -166,    68,  -166,   177,
     195,   189,   200,   219,   238,  -166,   212,  -166,  -166,   405,
     229,  -166,   230,   231,   233,   130,   352,    -3,   251,  -166,
    -166,   212,   269,  -166,  -166,  -166,  -166,   163,  -166,  -166,
    -166,  -166,    72,   130,   174,   180,  -166,    34,   254,   272,
     363,   279,   212,  -166,   308,   265,   282,  -166,  -166,     6,
      35,  -166,   199,   288,  -166,   207,   288,  -166,  -166,   212,
       6,   273,   274,   212,  -166,  -166,  -166,   275,  -166,  -166,
    -166,   141,   246,  -166,    47,  -166,  -166,  -166,   212,  -166,
     176,  -166,    72,  -166,  -166,   212,  -166,  -166,  -166,   109,
    -166,  -166,  -166,  -166,  -166,  -166,  -166,  -166,  -166,    72,
     434,   130,   130,     5,  -166,   277,   174,  -166,  -166,  -166,
     289,  -166,  -166,  -166,  -166,  -166,    30,   216,  -166,   296,
       1,   291,   290,   163,   307,   130,    72,   251,   174,  -166,
    -166,  -166,  -166,  -166,  -166,  -166,   293,  -166,   384,  -166,
    -166,  -166,  -166,  -166,  -166,  -166,  -166,  -166,   321,   280,
    -166,  -166,  -166,    96,  -166,  -166,   136,  -166,   212,     6,
       6,  -166,  -166,  -166,   312,  -166,  -166,   212,  -166,  -166,
    -166,   217,  -166,  -166,  -166,  -166,   143,  -166,  -166,   212,
     212,    72,    72,   130,   130,  -166,    72,  -166,  -166,  -166,
    -166,    30,  -166,   323,  -166,   465,   298,  -166,   326,   313,
     334,    72,   328,  -166,   212,   212,  -166,   174,    72,   363,
    -166,  -166,  -166,     6,  -166,   333,  -166,  -166,  -166,  -166,
     316,  -166,  -166,  -166,   109,   109,  -166,  -166,  -166,  -166,
      72,    72,  -166,  -166,  -166,   331,  -166,   310,  -166,   336,
     130,  -166,   332,  -166,  -166,    72,  -166,  -166,  -166,  -166,
     130,  -166,  -166,  -166,  -166,  -166,  -166,   338,    72,  -166,
    -166,   212,   130,  -166,  -166,  -166
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -166,  -166,  -166,  -166,  -166,   357,  -166,  -166,   359,  -166,
    -166,  -166,  -166,  -166,  -166,   208,  -166,   144,   370,  -166,
      95,  -166,   167,  -166,  -166,  -166,  -166,  -166,    13,  -166,
    -166,  -166,   319,  -166,   315,  -166,  -166,  -166,   -72,  -166,
     353,   -36,  -166,  -166,   314,   286,   -91,  -166,  -166,  -166,
     317,   294,    -9,   -86,   256,   258,   -77,  -165,  -143,   138,
    -166,  -166,   134,   -78,  -166,  -166,  -166,    -7,    -2,    -8,
    -166,  -166,   364,     7,   301,  -166,   -16,   -48,   368,   -24,
    -166,    11,  -166
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -130
static const short yytable[] =
{
      77,    48,    58,   111,    59,    38,   113,   241,   169,   107,
     176,   256,    35,   154,   194,   107,   182,   125,   268,   122,
      53,    40,    55,    37,   134,    39,   183,    65,   174,   198,
     253,    74,   269,   278,   118,    39,    53,    39,     5,     6,
     142,    54,    53,    66,    41,   143,   133,    42,   144,    75,
      58,   145,   163,   153,    43,   123,   179,   142,    75,   154,
     141,    87,   143,    56,    72,   259,   242,   177,   145,    65,
     141,   218,   219,   181,    75,   197,   107,   172,   179,   173,
      73,   108,   142,   248,   119,    66,   125,   143,   213,   258,
     144,    75,   115,   145,   251,   252,   190,   109,   107,   153,
     260,   155,   210,    53,    39,   227,     5,     6,   116,   231,
     276,    64,   315,   124,   271,    42,    75,   217,   275,   277,
     238,   110,    43,   244,   239,    53,   245,   241,   228,    44,
      75,   243,   285,   286,    78,   135,     4,     5,     6,     7,
      54,    79,   190,    41,     9,   110,    42,   136,   117,   255,
      80,    81,    37,    43,   138,    82,    83,    84,    85,    86,
      87,   150,    56,   250,   254,   298,   299,   127,   190,   273,
     302,   128,   285,   287,    88,   303,   300,   301,     5,     6,
       5,     6,   294,   295,    41,   311,    41,    42,    55,    42,
     190,   184,   316,   184,    43,   157,    43,    80,   139,   180,
     282,    44,    37,    44,   288,   191,   185,    75,   240,    24,
     159,   192,   220,   292,   323,   324,   221,    46,    37,   130,
     224,   131,   321,   322,   225,   296,   297,   289,   290,   330,
      35,   158,   132,   328,   160,     2,     3,   161,     4,     5,
       6,     7,   333,   331,     8,    75,     9,    10,   264,   293,
     313,   314,   265,   265,   162,   335,    75,   190,   266,   266,
    -129,   233,   190,    11,   234,   166,   167,   235,   168,   190,
      78,   210,     4,     5,     6,     7,    54,    79,   175,    41,
       9,   318,    42,     4,     5,     6,    80,    81,   199,    43,
     200,    82,    83,    84,    85,    86,    87,   212,    56,   215,
     216,   222,   178,   229,   230,   232,   257,   334,   174,    78,
      88,     4,     5,     6,     7,    54,    79,   267,    41,     9,
     272,    42,   270,   274,   283,    80,    81,   291,    43,   279,
      82,    83,    84,    85,    86,    87,   305,    56,     5,     6,
     307,   214,   308,   309,    41,   312,   319,    42,   320,    88,
     325,   184,   326,   327,    43,   170,     5,     6,    15,    54,
      17,    44,    41,   284,   329,    42,   310,     5,     6,    55,
     332,    27,    43,    41,   317,   281,    42,   171,   152,    44,
     201,    56,   156,    43,   114,   202,   203,   204,     5,     6,
      44,   193,   205,   164,    41,   304,   165,    42,   112,   196,
     236,   280,   237,   306,    43,   121,   202,   203,   204,     5,
       6,    44,    54,   205,     0,    41,     0,     0,    42,     0,
       5,     6,    80,    54,     0,    43,    41,   226,     0,    42,
      85,     0,    44,    55,    56,     0,    43,     0,     5,     6,
       0,    54,     0,    44,    41,    56,     0,    42,     0,     5,
       6,   249,    54,     0,    43,    41,     0,     0,    42,     0,
       0,    44,     0,    56,     0,    43,     4,     5,     6,     5,
       6,     0,    44,    41,    56,    41,    42,     0,    42,     0,
       0,     0,   184,    43,     0,    43,     0,     0,     0,     0,
      44,     0,    44
};

static const short yycheck[] =
{
      24,     9,    11,    27,    11,     7,    30,   150,    85,    25,
      88,   176,     1,    61,   105,    31,   102,    20,    17,    20,
       9,     8,    17,    17,    48,    38,   103,     1,    31,   107,
      25,    17,    31,   198,    36,    38,    25,    38,     4,     5,
      10,     7,    31,    17,    10,    15,    48,    13,    18,    44,
      59,    21,    76,    61,    20,    42,    92,    10,    44,   107,
      53,    27,    15,    29,    18,    35,   152,    91,    21,     1,
      63,    36,    37,    97,    44,    41,    92,    86,   114,    86,
      34,    18,    10,   169,     3,    17,    20,    15,   112,   180,
      18,    44,    18,    21,   171,   172,   104,    34,   114,   107,
     186,    33,   110,    92,    38,   129,     4,     5,    34,   133,
     196,    18,   277,    13,   191,    13,    44,   119,   195,   197,
     144,    35,    20,    14,   148,   114,    17,   270,   130,    27,
      44,   155,    36,    37,     1,    43,     3,     4,     5,     6,
       7,     8,   150,    10,    11,    35,    13,    30,    13,   173,
      17,    18,    17,    20,    13,    22,    23,    24,    25,    26,
      27,    31,    29,   170,   173,   251,   252,    13,   176,   193,
     256,    17,    36,    37,    41,   261,   253,   254,     4,     5,
       4,     5,    39,    40,    10,   271,    10,    13,    17,    13,
     198,    17,   278,    17,    20,    18,    20,    17,    13,    36,
     208,    27,    17,    27,   228,    25,    32,    44,    32,     1,
      21,    31,    13,   237,   300,   301,    17,     9,    17,    18,
      13,    20,   294,   295,    17,   249,   250,   229,   230,   315,
     219,    36,    31,   310,    34,     0,     1,    18,     3,     4,
       5,     6,   328,   320,     9,    44,    11,    12,    32,    32,
     274,   275,    36,    36,    16,   332,    44,   265,    42,    42,
      31,    15,   270,    28,    18,    35,    35,    21,    35,   277,
       1,   279,     3,     4,     5,     6,     7,     8,    27,    10,
      11,   283,    13,     3,     4,     5,    17,    18,    34,    20,
      18,    22,    23,    24,    25,    26,    27,    18,    29,    34,
      18,    13,    33,    30,    30,    30,    17,   331,    31,     1,
      41,     3,     4,     5,     6,     7,     8,    21,    10,    11,
      30,    13,    31,    16,     3,    17,    18,    15,    20,    36,
      22,    23,    24,    25,    26,    27,    13,    29,     4,     5,
      42,    33,    16,    30,    10,    17,    13,    13,    32,    41,
      19,    17,    42,    17,    20,     3,     4,     5,     1,     7,
       1,    27,    10,   219,    32,    13,    32,     4,     5,    17,
      32,     1,    20,    10,   279,   208,    13,    25,    59,    27,
      17,    29,    67,    20,    31,    22,    23,    24,     4,     5,
      27,   105,    29,    79,    10,   261,    79,    13,    30,   105,
     144,    17,   144,   265,    20,    41,    22,    23,    24,     4,
       5,    27,     7,    29,    -1,    10,    -1,    -1,    13,    -1,
       4,     5,    17,     7,    -1,    20,    10,   126,    -1,    13,
      25,    -1,    27,    17,    29,    -1,    20,    -1,     4,     5,
      -1,     7,    -1,    27,    10,    29,    -1,    13,    -1,     4,
       5,    17,     7,    -1,    20,    10,    -1,    -1,    13,    -1,
      -1,    27,    -1,    29,    -1,    20,     3,     4,     5,     4,
       5,    -1,    27,    10,    29,    10,    13,    -1,    13,    -1,
      -1,    -1,    17,    20,    -1,    20,    -1,    -1,    -1,    -1,
      27,    -1,    27
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    46,     0,     1,     3,     4,     5,     6,     9,    11,
      12,    28,    47,    48,    49,    50,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    68,    69,
      70,    71,    72,    75,    76,   126,   127,    17,   113,    38,
      73,    10,    13,    20,    27,    51,    60,    63,   114,   115,
     116,   117,   118,   126,     7,    17,    29,    77,    97,   112,
     114,   121,   122,   126,    18,     1,    17,    78,    79,    80,
      81,    84,    18,    34,    17,    44,   123,   124,     1,     8,
      17,    18,    22,    23,    24,    25,    26,    27,    41,    50,
      53,    60,    85,    86,    87,    88,    89,    90,    91,    93,
      94,    95,    96,    97,   108,   111,   112,   121,    18,    34,
      35,   124,   123,   124,    85,    18,    34,    13,   113,     3,
      74,   117,    20,    73,    13,    20,    73,    13,    17,   120,
      18,    20,    31,   113,   124,    43,    30,   110,    13,    13,
     113,   118,    10,    15,    18,    21,    98,    99,   100,   124,
      31,   101,    77,   114,   122,    33,    79,    18,    36,    21,
      34,    18,    16,   124,    89,    95,    35,    35,    35,   101,
       3,    25,    97,   112,    31,    27,   108,   124,    33,    86,
      36,   124,    98,   101,    17,    32,   102,   103,   104,   105,
     114,    25,    31,    90,    91,    92,    96,    41,   108,    34,
      18,    17,    22,    23,    24,    29,    64,    65,    66,    67,
     114,   125,    18,   124,    33,    34,    18,   113,    36,    37,
      13,    17,    13,   119,    13,    17,   119,   124,   113,    30,
      30,   124,    30,    15,    18,    21,    99,   100,   124,   124,
      32,   103,    98,   124,    14,    17,    82,    83,    98,    17,
     112,   101,   101,    25,    97,   124,   102,    17,    91,    35,
      98,   106,   107,   109,    32,    36,    42,    21,    17,    31,
      31,   101,    30,   124,    16,   101,    98,   108,   102,    36,
      17,    67,   114,     3,    62,    36,    37,    37,   124,   113,
     113,    15,   124,    32,    39,    40,   124,   124,    98,    98,
     101,   101,    98,    98,   107,    13,   104,    42,    16,    30,
      32,    98,    17,   124,   124,   102,    98,    65,   113,    13,
      32,    83,    83,    98,    98,    19,    42,    17,   101,    32,
      98,   101,    32,    98,   124,   101
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
#define YYERROR		goto yyerrorlab


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
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
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
| TOP (included).                                                   |
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

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
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
	struct yyalloc *yyptr =
	  (struct yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
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
        case 2:
#line 136 "mta_parse.y"
    { mta->yy_state = MTA::YYRet_Exit; }
    break;

  case 3:
#line 137 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok;
	    return mta->yy_state; }
    break;

  case 4:
#line 140 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 5:
#line 142 "mta_parse.y"
    {
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 6:
#line 145 "mta_parse.y"
    {
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 7:
#line 148 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 8:
#line 150 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 9:
#line 152 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 10:
#line 154 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
    break;

  case 11:
#line 159 "mta_parse.y"
    { }
    break;

  case 12:
#line 162 "mta_parse.y"
    {
  	  if(yyvsp[0].typ != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("typedef", sp, yyval.typ); } }
    break;

  case 13:
#line 170 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 14:
#line 171 "mta_parse.y"
    {
	    if(yyvsp[-1].typ != NULL)  SETDESC(yyvsp[-1].typ,yyvsp[0].chr); }
    break;

  case 15:
#line 175 "mta_parse.y"
    { yyval.typ = yyvsp[0].typ; }
    break;

  case 16:
#line 176 "mta_parse.y"
    {
	    yyval.typ = yyvsp[-2].typ; yyvsp[-2].typ->name = yyvsp[-1].chr; mta->type_stack.Pop(); }
    break;

  case 17:
#line 180 "mta_parse.y"
    {
            yyval.typ = yyvsp[-1].typ; yyvsp[-1].typ->AddParent(yyvsp[-2].typ); yyvsp[-1].typ->ptr = yyvsp[-2].typ->ptr;
	    yyvsp[-1].typ->par_formal.BorrowUnique(yyvsp[-2].typ->par_formal);
	    yyvsp[-1].typ->par_cache.BorrowUnique(yyvsp[-2].typ->par_cache);
	    mta->type_stack.Pop(); }
    break;

  case 18:
#line 185 "mta_parse.y"
    { /* annoying place for a comment, but.. */
            yyval.typ = yyvsp[-1].typ; yyvsp[-1].typ->AddParent(yyvsp[-3].typ); yyvsp[-1].typ->ptr = yyvsp[-3].typ->ptr;
	    yyvsp[-1].typ->par_formal.BorrowUnique(yyvsp[-3].typ->par_formal);
	    yyvsp[-1].typ->par_cache.BorrowUnique(yyvsp[-3].typ->par_cache);
	    mta->type_stack.Pop(); }
    break;

  case 19:
#line 192 "mta_parse.y"
    {
	    TypeDef* td = yyvsp[-1].typ->parents[1]; mta->type_stack.Pop();
	    TypeSpace* sp = yyvsp[-1].typ->owner;
	    sp->Remove(yyvsp[-1].typ); /* get rid of new one, cuz it is bogus */
	    /* not on list that it would be placed on now.. */
	    if((td->owner != mta->spc) && (mta->spc->FindName(td->name) == NULL)) {
	      if(mta->verbose >= 3)
		cerr << "transfered: " << td->name << " from: " << td->owner->name
		     << " to: " << mta->spc->name << "\n";
	      mta->spc->Transfer(td); /* now check for parent which is a combo of basic types */
	      if((td->parents.size == 1) && (td->parents[0]->owner != mta->spc) &&
		 (td->parents[0]->parents.size == 2)) {
		/* has one parent that is a combo-type which might be basic */
		TypeDef* par = td->parents[0];
		if((mta->spc_builtin.FindName(par->parents[0]->name) != NULL) &&
		   (mta->spc_builtin.FindName(par->parents[1]->name) != NULL)) {
		  if(mta->verbose >= 3)
		    cerr << "transfered: " << par->name << " from: " << par->owner->name
			 << " to: " << mta->spc->name << "\n";
		  TypeDef* already_there = mta->spc->FindName(par->name);
		  if(already_there == NULL)
		    mta->spc->Transfer(par); /* move parent to this list too */
		  else
		    td->parents.ReplaceLink(0, already_there);
		}
	      }
	    }
	    yyval.typ = td; }
    break;

  case 20:
#line 222 "mta_parse.y"
    {
	    yyval.typ = yyvsp[-3].typ; yyval.typ->AddParent(&TA_void_ptr); yyval.typ->ptr = 1;
	    mta->type_stack.Pop(); }
    break;

  case 21:
#line 225 "mta_parse.y"
    {
	    yyval.typ = yyvsp[0].typ; yyval.typ->AddParent(&TA_void_ptr); yyval.typ->ptr = 1;
	    mta->type_stack.Pop(); }
    break;

  case 22:
#line 230 "mta_parse.y"
    {
            TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
            yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("enum", sp, yyval.typ); }
    break;

  case 25:
#line 240 "mta_parse.y"
    { SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); }
    break;

  case 26:
#line 241 "mta_parse.y"
    { SETDESC(yyvsp[-2].typ,yyvsp[0].chr); }
    break;

  case 27:
#line 244 "mta_parse.y"
    {
  	    yyval.typ = yyvsp[0].typ;
	    yyvsp[0].typ->AddParFormal(&TA_enum); mta->cur_enum = yyvsp[0].typ;
	    mta->type_stack.Pop(); }
    break;

  case 28:
#line 248 "mta_parse.y"
    {
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->cur_enum = yyval.typ;
	    yyval.typ->AddParFormal(&TA_enum); yyval.typ->internal = true; }
    break;

  case 29:
#line 255 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("class", sp, yyval.typ);
	    mta->type_stack.Pop(); }
    break;

  case 30:
#line 263 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 31:
#line 264 "mta_parse.y"
    { SETDESC(yyvsp[-2].typ,yyvsp[0].chr); }
    break;

  case 32:
#line 268 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("class", sp, yyval.typ);
	    mta->type_stack.Pop(); }
    break;

  case 35:
#line 281 "mta_parse.y"
    {
	    if(yyvsp[-2].typ->HasOption("NO_TOKENS")) yyvsp[-2].typ->tokens.keep = false;
	    else yyvsp[-2].typ->tokens.keep = true; }
    break;

  case 36:
#line 287 "mta_parse.y"
    {
	    yyvsp[-1].typ->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 37:
#line 290 "mta_parse.y"
    {
	    SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 38:
#line 292 "mta_parse.y"
    {
	    SETDESC(yyvsp[-2].typ,yyvsp[0].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 39:
#line 297 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 40:
#line 298 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 41:
#line 301 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
            yyval.typ = yyvsp[0].typ; mta->last_class = mta->cur_class; mta->cur_class = yyvsp[0].typ;
	    yyvsp[0].typ->AddParFormal(&TA_class);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 42:
#line 306 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    yyval.typ = yyvsp[0].typ; mta->last_class = mta->cur_class; mta->cur_class = yyvsp[0].typ;
            mta->cur_mstate = MTA::prvt; }
    break;

  case 43:
#line 310 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = yyvsp[0].typ->name + "_" + (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->type_stack.Push(yyval.typ);
	    mta->last_class = mta->cur_class; mta->cur_class = yyval.typ;
            mta->cur_mstate = MTA::prvt; }
    break;

  case 44:
#line 316 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = yyvsp[0].typ->name + "_" + (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->type_stack.Push(yyval.typ);
	    mta->last_class = mta->cur_class; mta->cur_class = yyval.typ;
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 45:
#line 325 "mta_parse.y"
    {
            if(yyvsp[0].typ->InheritsFrom(TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent(yyvsp[0].typ); }
    break;

  case 46:
#line 328 "mta_parse.y"
    {
            if(yyvsp[0].typ->InheritsFrom(&TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent(yyvsp[0].typ);
	    if(!mta->cur_class->HasOption("MULT_INHERIT"))
	      mta->cur_class->opts.Add("MULT_INHERIT"); }
    break;

  case 48:
#line 337 "mta_parse.y"
    { yyval.typ = yyvsp[0].typ; }
    break;

  case 49:
#line 338 "mta_parse.y"
    { yyval.typ = &TA_class; }
    break;

  case 50:
#line 339 "mta_parse.y"
    { yyval.typ = &TA_class; }
    break;

  case 55:
#line 351 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("template", sp, yyval.typ);
	    mta->type_stack.Pop(); }
    break;

  case 58:
#line 364 "mta_parse.y"
    {
          if(yyvsp[-2].typ->HasOption("NO_TOKENS")) yyvsp[-2].typ->tokens.keep = false;
	  else yyvsp[-2].typ->tokens.keep = true; }
    break;

  case 59:
#line 370 "mta_parse.y"
    {
            mta->state = MTA::Parse_inclass; yyvsp[-1].typ->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 60:
#line 373 "mta_parse.y"
    {
	    SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 61:
#line 375 "mta_parse.y"
    {
	    SETDESC(yyvsp[-2].typ,yyvsp[0].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 62:
#line 380 "mta_parse.y"
    {
	    yyvsp[0].typ->templ_pars.Reset();
	    yyvsp[0].typ->templ_pars.Duplicate(mta->cur_templ_pars);
	    yyvsp[0].typ->internal = true;
	    yyvsp[0].typ->AddParFormal(&TA_template); yyval.typ = yyvsp[0].typ; }
    break;

  case 63:
#line 387 "mta_parse.y"
    { mta->cur_templ_pars.Reset(); }
    break;

  case 64:
#line 391 "mta_parse.y"
    { mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[0].typ; }
    break;

  case 65:
#line 392 "mta_parse.y"
    { mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[-3].typ; }
    break;

  case 66:
#line 395 "mta_parse.y"
    {
            if(mta->spc == &(mta->spc_target)) /* only add reg_funs in target space */
	       TA_taRegFun.methods.AddUniqNameNew(yyvsp[0].meth);
	    mta->meth_stack.Pop(); }
    break;

  case 67:
#line 401 "mta_parse.y"
    {
            yyvsp[-1].meth->type = yyvsp[-2].typ; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); yyval.meth = yyvsp[-1].meth; }
    break;

  case 68:
#line 403 "mta_parse.y"
    {
            yyvsp[-1].meth->type = &TA_int; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); yyval.meth = yyvsp[-1].meth; }
    break;

  case 69:
#line 407 "mta_parse.y"
    {
            yyvsp[-1].meth->is_static = true; /* consider these to be static functions */
            yyvsp[-1].meth->fun_argc = yyvsp[0].rval; yyvsp[-1].meth->arg_types.size = yyvsp[0].rval; mta->burp_fundefn = true; }
    break;

  case 73:
#line 417 "mta_parse.y"
    { if(yyvsp[-1].enm != NULL) SETENUMDESC(yyvsp[-1].enm,yyvsp[0].chr); }
    break;

  case 76:
#line 422 "mta_parse.y"
    { /* trying to do some math */
           yyval.enm = NULL;
	   mta->skiptocommarb(); }
    break;

  case 77:
#line 427 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add(yyvsp[0].enm);
	    mta->enum_stack.Pop(); }
    break;

  case 78:
#line 430 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add(yyvsp[-2].enm); if(yyvsp[0].rval != -424242) yyvsp[-2].enm->enum_no = yyvsp[0].rval;
	    mta->enum_stack.Pop(); }
    break;

  case 80:
#line 437 "mta_parse.y"
    {
	  if((yyvsp[-2].rval != -424242) && (yyvsp[0].rval != -424242))  yyval.rval = yyvsp[-2].rval + yyvsp[0].rval;
	  else if(yyvsp[-2].rval != -424242)	yyval.rval = yyvsp[-2].rval;
	  else if(yyvsp[0].rval != -424242)	yyval.rval = yyvsp[0].rval;
	  else				yyval.rval = -424242; }
    break;

  case 81:
#line 442 "mta_parse.y"
    {
	  if((yyvsp[-2].rval != -424242) && (yyvsp[0].rval != -424242)) yyval.rval = yyvsp[-2].rval - yyvsp[0].rval;
	  else if(yyvsp[-2].rval != -424242)	yyval.rval = yyvsp[-2].rval;
	  else if(yyvsp[0].rval != -424242)	yyval.rval = yyvsp[0].rval;
	  else				yyval.rval = -424242; }
    break;

  case 82:
#line 450 "mta_parse.y"
    { yyval.rval = -424242; }
    break;

  case 84:
#line 455 "mta_parse.y"
    { yyval.enm = new EnumDef(yyvsp[0].chr); mta->enum_stack.Push(yyval.enm); }
    break;

  case 85:
#line 458 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 86:
#line 459 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 87:
#line 462 "mta_parse.y"
    {
            if(yyvsp[0].memb != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->HasOption("IGNORE"))
		 && !(yyvsp[0].memb->type->DerivesFrom(TA_const))) {
		mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
		if(mta->verbose >= 3)
		  cerr << "member: " << yyvsp[0].memb->name << " added to: "
		       << mta->cur_class->name << "\n"; } }
	    mta->memb_stack.Pop(); yyval.typ = NULL; }
    break;

  case 88:
#line 471 "mta_parse.y"
    {
            if(yyvsp[0].meth != NULL) {
	      if(mta->cur_mstate == MTA::pblc) {
		if(yyvsp[0].meth->HasOption("IGNORE"))
		  mta->cur_class->ignore_meths.AddUnique(yyvsp[0].meth->name);
		else {
		  mta->cur_class->methods.AddUniqNameNew(yyvsp[0].meth);
		  if(mta->verbose >= 3)
		    cerr << "method: " << yyvsp[0].meth->name << " added to: "
			 << mta->cur_class->name << "\n"; } } }
	    else {
	      mta->cur_meth = NULL; }
	    mta->meth_stack.Pop(); yyval.typ = NULL; }
    break;

  case 89:
#line 484 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew(yyvsp[0].typ);
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; }
    break;

  case 90:
#line 487 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew(yyvsp[0].typ);
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; }
    break;

  case 91:
#line 490 "mta_parse.y"
    { /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew(yyvsp[-1].typ);
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; }
    break;

  case 92:
#line 494 "mta_parse.y"
    { yyval.typ = NULL; }
    break;

  case 94:
#line 499 "mta_parse.y"
    { yyval.memb = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 95:
#line 500 "mta_parse.y"
    { yyval.memb = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 96:
#line 501 "mta_parse.y"
    { yyval.memb = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 97:
#line 502 "mta_parse.y"
    {
	    yyval.memb = NULL;
	    if(mta->last_memb != NULL) {
	      SETDESC(mta->last_memb, yyvsp[0].chr);
	      if(mta->last_memb->HasOption("IGNORE"))
		mta->cur_class->members.Remove(mta->last_memb); }
	    else if(mta->last_meth != NULL) {
	      SETDESC(mta->last_meth, yyvsp[0].chr);
	      if(mta->last_meth->HasOption("IGNORE")) {
		mta->cur_class->ignore_meths.AddUnique(mta->last_meth->name);
		mta->cur_class->methods.Remove(mta->last_meth);
		mta->last_meth = NULL; }
	      else if((mta->last_meth->opts.size > 0) || (mta->last_meth->lists.size > 0)) {
		mta->cur_class->methods.AddUniqNameNew(mta->last_meth);
		if(mta->verbose >= 3)
		  cerr << "method: " << mta->last_meth->name << " added to: "
		       << mta->cur_class->name << "\n"; } } }
    break;

  case 99:
#line 523 "mta_parse.y"
    {
	  yyval.memb = yyvsp[0].memb; if(yyvsp[0].memb != NULL) yyvsp[0].memb->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 100:
#line 529 "mta_parse.y"
    { yyval.memb = yyvsp[-1].memb; }
    break;

  case 101:
#line 530 "mta_parse.y"
    { }
    break;

  case 102:
#line 531 "mta_parse.y"
    {
	    yyval.memb = yyvsp[-2].memb;
	    String nm = yyvsp[-3].typ->name + "_ary";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[-3].typ->ptr + 1);
	    nty->AddParFormal(&TA_ta_array); nty->AddParent(yyvsp[-3].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-3].typ);
	    TypeDef* uty = sp->AddUniqNameOld(nty); yyvsp[-2].memb->type = uty;
	    if(uty == nty) mta->TypeAdded("array", sp, uty); }
    break;

  case 103:
#line 539 "mta_parse.y"
    { yyvsp[-2].memb->type = yyvsp[-3].typ; yyval.memb = yyvsp[-2].memb; }
    break;

  case 104:
#line 543 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
             mta->memb_stack.Pop(); yyval.memb = NULL; }
    break;

  case 105:
#line 547 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
             mta->memb_stack.Pop(); yyval.memb = NULL; }
    break;

  case 106:
#line 553 "mta_parse.y"
    {
            yyval.memb = new MemberDef(yyvsp[0].chr); mta->cur_memb = yyval.memb; mta->memb_stack.Push(yyval.memb);
            if(mta->cur_memb_type != NULL) yyval.memb->type = mta->cur_memb_type;
            else yyval.memb->type = &TA_int; }
    break;

  case 107:
#line 559 "mta_parse.y"
    {
	    yyval.memb = new MemberDef(yyvsp[-1].chr); mta->cur_memb = yyval.memb; mta->memb_stack.Push(yyval.memb);
	    yyval.memb->fun_ptr = 1; }
    break;

  case 109:
#line 565 "mta_parse.y"
    { yyval.meth = NULL; mta->thisname = false; }
    break;

  case 110:
#line 566 "mta_parse.y"
    { yyval.meth = NULL; mta->thisname = false; }
    break;

  case 111:
#line 567 "mta_parse.y"
    { yyval.meth = NULL; mta->thisname = false; }
    break;

  case 112:
#line 568 "mta_parse.y"
    { yyval.meth = NULL; mta->thisname = false; }
    break;

  case 113:
#line 569 "mta_parse.y"
    { yyval.meth = NULL; mta->thisname = false; }
    break;

  case 114:
#line 570 "mta_parse.y"
    { yyval.meth = NULL; }
    break;

  case 115:
#line 571 "mta_parse.y"
    { yyval.meth = NULL; }
    break;

  case 116:
#line 572 "mta_parse.y"
    { yyval.meth = NULL; }
    break;

  case 117:
#line 573 "mta_parse.y"
    { yyval.meth = NULL; }
    break;

  case 118:
#line 574 "mta_parse.y"
    { yyval.meth = NULL; }
    break;

  case 119:
#line 575 "mta_parse.y"
    {
	    yyval.meth = NULL; String tmp = yyvsp[0].chr;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew(yyvsp[-2].meth); yyvsp[-2].meth->type = yyvsp[-3].typ;
	      mta->meth_stack.Pop();  yyvsp[-2].meth->fun_argc = yyvsp[-1].rval; yyvsp[-2].meth->arg_types.size = yyvsp[-1].rval;
	      yyvsp[-2].meth->is_static = true; /* consider these to be static functions */
	      SETDESC(yyvsp[-2].meth,yyvsp[0].chr); }
	    else { yyval.meth = NULL; mta->meth_stack.Pop(); } }
    break;

  case 120:
#line 583 "mta_parse.y"
    {
	    yyval.meth = NULL; String tmp = yyvsp[0].chr;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew(yyvsp[-2].meth); yyvsp[-2].meth->type = &TA_int;
	      mta->meth_stack.Pop();  yyvsp[-2].meth->fun_argc = yyvsp[-1].rval; yyvsp[-2].meth->arg_types.size = yyvsp[-1].rval;
	      yyvsp[-2].meth->is_static = true; /* consider these to be static functions */
	      SETDESC(yyvsp[-2].meth,yyvsp[0].chr); }
	    else { yyval.meth = 0; mta->meth_stack.Pop(); } }
    break;

  case 122:
#line 595 "mta_parse.y"
    { yyval.meth = yyvsp[0].meth; yyvsp[0].meth->is_static = true; }
    break;

  case 123:
#line 599 "mta_parse.y"
    { yyval.meth = yyvsp[-1].meth; yyvsp[-1].meth->type = yyvsp[-2].typ; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); }
    break;

  case 124:
#line 600 "mta_parse.y"
    { yyvsp[-1].meth->type = &TA_int; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); }
    break;

  case 125:
#line 601 "mta_parse.y"
    { yyval.meth = NULL; }
    break;

  case 126:
#line 602 "mta_parse.y"
    { yyval.meth = NULL; }
    break;

  case 127:
#line 603 "mta_parse.y"
    { yyval.meth = NULL; }
    break;

  case 128:
#line 606 "mta_parse.y"
    {
             yyvsp[-1].meth->fun_argc = yyvsp[0].rval; yyvsp[-1].meth->arg_types.size = yyvsp[0].rval; mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if(yyvsp[-1].meth->fun_argd > yyvsp[-1].meth->fun_argc) yyvsp[-1].meth->fun_argd = -1; }
    break;

  case 129:
#line 613 "mta_parse.y"
    {
            yyval.meth = new MethodDef(yyvsp[0].chr); mta->cur_meth = yyval.meth; mta->meth_stack.Push(yyval.meth); }
    break;

  case 130:
#line 617 "mta_parse.y"
    { yyval.chr = ""; }
    break;

  case 131:
#line 618 "mta_parse.y"
    { yyval.chr = yyvsp[0].chr; }
    break;

  case 132:
#line 619 "mta_parse.y"
    { yyval.chr = yyvsp[-1].chr; }
    break;

  case 133:
#line 620 "mta_parse.y"
    { yyval.chr = yyvsp[-1].chr; }
    break;

  case 134:
#line 621 "mta_parse.y"
    { yyval.chr = yyvsp[-1].chr; }
    break;

  case 135:
#line 622 "mta_parse.y"
    { yyval.chr = yyvsp[-2].chr; }
    break;

  case 136:
#line 626 "mta_parse.y"
    { yyval.chr = ""; }
    break;

  case 137:
#line 627 "mta_parse.y"
    { yyval.chr = ""; }
    break;

  case 138:
#line 628 "mta_parse.y"
    { yyval.chr = yyvsp[-1].chr; }
    break;

  case 139:
#line 632 "mta_parse.y"
    { yyval.chr = ""; }
    break;

  case 140:
#line 633 "mta_parse.y"
    { yyval.chr = ""; }
    break;

  case 141:
#line 634 "mta_parse.y"
    { yyval.chr = ""; }
    break;

  case 142:
#line 637 "mta_parse.y"
    { yyval.rval = 0; }
    break;

  case 143:
#line 638 "mta_parse.y"
    { yyval.rval = yyvsp[-1].rval; }
    break;

  case 144:
#line 641 "mta_parse.y"
    { yyval.rval = 0; }
    break;

  case 145:
#line 642 "mta_parse.y"
    { yyval.rval = yyvsp[-1].rval; }
    break;

  case 146:
#line 645 "mta_parse.y"
    { yyval.rval = 1; }
    break;

  case 147:
#line 646 "mta_parse.y"
    { yyval.rval = yyvsp[-2].rval + 1; }
    break;

  case 148:
#line 647 "mta_parse.y"
    { yyval.rval = yyvsp[-3].rval; }
    break;

  case 149:
#line 650 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 150:
#line 652 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_defs.Add(yyvsp[0].chr);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 151:
#line 660 "mta_parse.y"
    {
	    yyval.rval = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[0].typ); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 152:
#line 664 "mta_parse.y"
    {
	    yyval.rval = 1; String nm = yyvsp[0].chr;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-1].typ); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 153:
#line 668 "mta_parse.y"
    {
	    yyval.rval = 1; String nm = String(yyvsp[-1].chr) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-2].typ); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 154:
#line 672 "mta_parse.y"
    {
	    yyval.rval = 1; String nm = String("(*") + String(yyvsp[-2].chr) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-5].typ); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 155:
#line 676 "mta_parse.y"
    {
	    yyval.rval = 1; String nm = yyvsp[0].chr;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 159:
#line 691 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 160:
#line 695 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 161:
#line 698 "mta_parse.y"
    { yyval.rval = 1; }
    break;

  case 162:
#line 699 "mta_parse.y"
    { yyval.rval = yyvsp[-1].rval + 1; }
    break;

  case 163:
#line 702 "mta_parse.y"
    { mta->cur_memb_type = yyvsp[0].typ; }
    break;

  case 165:
#line 706 "mta_parse.y"
    { yyval.typ = &TA_int; }
    break;

  case 166:
#line 707 "mta_parse.y"
    { yyval.typ = yyvsp[0].typ; }
    break;

  case 167:
#line 710 "mta_parse.y"
    { yyval.typ = new TypeDef(yyvsp[0].chr); mta->type_stack.Push(yyval.typ); }
    break;

  case 169:
#line 714 "mta_parse.y"
    {
	    String nm = yyvsp[-1].typ->name + "_ref";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[-1].typ->ptr, true);
	    nty->AddParent(yyvsp[-1].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-1].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("ref", sp, yyval.typ); }
    break;

  case 171:
#line 725 "mta_parse.y"
    {
 	    int i; String nm = yyvsp[-1].typ->name; for(i=0; i<yyvsp[0].rval; i++) nm += "_ptr";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[0].rval); nty->AddParent(yyvsp[-1].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-1].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("ptr", sp, yyval.typ); }
    break;

  case 173:
#line 734 "mta_parse.y"
    {
	    String nm = yyvsp[-1].typ->name + "_" + yyvsp[0].typ->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = yyvsp[0].typ->size; nty->AddParent(yyvsp[-1].typ); nty->AddParent(yyvsp[0].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("const", sp, yyval.typ); }
    break;

  case 175:
#line 744 "mta_parse.y"
    { yyval.typ = yyvsp[0].typ; }
    break;

  case 176:
#line 745 "mta_parse.y"
    { yyval.typ = yyvsp[0].typ; }
    break;

  case 177:
#line 746 "mta_parse.y"
    {
	    TypeDef* td; if((td = yyvsp[-2].typ->sub_types.FindName(yyvsp[0].chr)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    yyval.typ = td; }
    break;

  case 178:
#line 750 "mta_parse.y"
    {
	    TypeDef* td; if((td = yyvsp[-2].typ->sub_types.FindName(yyvsp[0].chr)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    yyval.typ = td; }
    break;

  case 179:
#line 754 "mta_parse.y"
    { yyval.typ = yyvsp[0].typ; }
    break;

  case 180:
#line 755 "mta_parse.y"
    { yyval.typ = yyvsp[0].typ; }
    break;

  case 181:
#line 756 "mta_parse.y"
    { yyval.typ = yyvsp[0].typ; }
    break;

  case 183:
#line 758 "mta_parse.y"
    { /* a template */
 	    if(!(yyvsp[-3].typ->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    if((yyvsp[-1].typ->owner != NULL) && (yyvsp[-1].typ->owner->owner != NULL))
	      yyval.typ = yyvsp[-3].typ;	/* don't allow internal types with external templates */
	    else {
	      String nm = yyvsp[-3].typ->GetTemplName(mta->cur_templ_pars);
	      TypeDef* td;
	      int lx_tok;
	      if((td = mta->FindName(nm, lx_tok)) == NULL) {
		td = yyvsp[-3].typ->Clone(); td->name = nm;
		td->SetTemplType(yyvsp[-3].typ, mta->cur_templ_pars);
		TypeSpace* sp = mta->GetTypeSpace(yyvsp[-3].typ);
 		yyval.typ = sp->AddUniqNameOld(td);
		if(yyval.typ == td) mta->TypeAdded("template instance", sp, yyval.typ); }
	      else
		yyval.typ = td; } }
    break;

  case 184:
#line 775 "mta_parse.y"
    { /* this template */
	    if(!(yyvsp[-3].typ->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    yyval.typ = yyvsp[-3].typ; }
    break;

  case 186:
#line 782 "mta_parse.y"
    {
	    String nm = yyvsp[-1].typ->name + "_" + yyvsp[0].typ->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = yyvsp[0].typ->size; nty->AddParent(yyvsp[-1].typ); nty->AddParent(yyvsp[0].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("combo", sp, yyval.typ); }
    break;

  case 187:
#line 792 "mta_parse.y"
    { mta->cur_templ_pars.Link(yyvsp[0].typ); }
    break;

  case 188:
#line 793 "mta_parse.y"
    { mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[-2].typ; }
    break;

  case 190:
#line 797 "mta_parse.y"
    { yyval.chr = yyvsp[0].typ->name; }
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2495 "y.tab.c"

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

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
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
      YYPOPSTACK;
      yystate = *yyssp;
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


#line 829 "mta_parse.y"


	/* end of grammar */

void yyerror(char *s) { 	/* called for yacc syntax error */
  int i;
  if((mta->verbose < 1) && (mta->spc != &(mta->spc_target)))
    return;

  if(strcmp(s, "parse error") == 0) {
    cerr << "Syntax Error, line " << mta->st_line << ":\t" << MTA::LastLn << "\n";
    cerr << "\t\t\t";
    for(i=0; i < mta->st_col + 1; i++)
      cerr << " ";
    cerr << "^\n";
  }
  else {
    cerr << s << "line " << mta->st_line << ":\t" << MTA::LastLn << "\n";
  }
}

