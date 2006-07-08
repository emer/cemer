/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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
     CSS_PP_INCLUDE = 258,
     CSS_PP_DEFINE = 259,
     CSS_PP_UNDEF = 260,
     CSS_PP_IFDEF = 261,
     CSS_PP_IFNDEF = 262,
     CSS_PP_ELSE = 263,
     CSS_PP_ENDIF = 264,
     CSS_NUMBER = 265,
     CSS_STRING = 266,
     CSS_VAR = 267,
     CSS_FUN = 268,
     CSS_PTR = 269,
     CSS_PP_DEF = 270,
     CSS_NAME = 271,
     CSS_COMMENT = 272,
     CSS_TYPE = 273,
     CSS_PTRTYPE = 274,
     CSS_CLASS = 275,
     CSS_ENUM = 276,
     CSS_SCPTYPE = 277,
     CSS_WHILE = 278,
     CSS_DO = 279,
     CSS_IF = 280,
     CSS_ELSE = 281,
     CSS_SWITCH = 282,
     CSS_CASE = 283,
     CSS_DEFAULT = 284,
     CSS_RETURN = 285,
     CSS_BREAK = 286,
     CSS_CONTINUE = 287,
     CSS_FOR = 288,
     CSS_NEW = 289,
     CSS_DELETE = 290,
     CSS_COMMAND = 291,
     CSS_ALIAS = 292,
     CSS_REMOVE = 293,
     CSS_TYPECMD = 294,
     CSS_HELP = 295,
     CSS_EXTERN = 296,
     CSS_STATIC = 297,
     CSS_CONST = 298,
     CSS_PRIVATE = 299,
     CSS_PUBLIC = 300,
     CSS_PROTECTED = 301,
     CSS_VIRTUAL = 302,
     CSS_INLINE = 303,
     CSS_ASGN_OR = 304,
     CSS_ASGN_XOR = 305,
     CSS_ASGN_AND = 306,
     CSS_ASGN_RSHIFT = 307,
     CSS_ASGN_LSHIFT = 308,
     CSS_ASGN_MOD = 309,
     CSS_ASGN_DIV = 310,
     CSS_ASGN_MULT = 311,
     CSS_ASGN_SUB = 312,
     CSS_ASGN_ADD = 313,
     CSS_OR = 314,
     CSS_AND = 315,
     CSS_NE = 316,
     CSS_EQ = 317,
     CSS_LE = 318,
     CSS_LT = 319,
     CSS_GE = 320,
     CSS_GT = 321,
     CSS_RSHIFT = 322,
     CSS_LSHIFT = 323,
     CSS_MINMIN = 324,
     CSS_PLUSPLUS = 325,
     CSS_UNARY = 326,
     CSS_NOT = 327,
     CSS_UNARYMINUS = 328,
     CSS_SCOPER = 329,
     CSS_POINTSAT = 330
   };
#endif
#define CSS_PP_INCLUDE 258
#define CSS_PP_DEFINE 259
#define CSS_PP_UNDEF 260
#define CSS_PP_IFDEF 261
#define CSS_PP_IFNDEF 262
#define CSS_PP_ELSE 263
#define CSS_PP_ENDIF 264
#define CSS_NUMBER 265
#define CSS_STRING 266
#define CSS_VAR 267
#define CSS_FUN 268
#define CSS_PTR 269
#define CSS_PP_DEF 270
#define CSS_NAME 271
#define CSS_COMMENT 272
#define CSS_TYPE 273
#define CSS_PTRTYPE 274
#define CSS_CLASS 275
#define CSS_ENUM 276
#define CSS_SCPTYPE 277
#define CSS_WHILE 278
#define CSS_DO 279
#define CSS_IF 280
#define CSS_ELSE 281
#define CSS_SWITCH 282
#define CSS_CASE 283
#define CSS_DEFAULT 284
#define CSS_RETURN 285
#define CSS_BREAK 286
#define CSS_CONTINUE 287
#define CSS_FOR 288
#define CSS_NEW 289
#define CSS_DELETE 290
#define CSS_COMMAND 291
#define CSS_ALIAS 292
#define CSS_REMOVE 293
#define CSS_TYPECMD 294
#define CSS_HELP 295
#define CSS_EXTERN 296
#define CSS_STATIC 297
#define CSS_CONST 298
#define CSS_PRIVATE 299
#define CSS_PUBLIC 300
#define CSS_PROTECTED 301
#define CSS_VIRTUAL 302
#define CSS_INLINE 303
#define CSS_ASGN_OR 304
#define CSS_ASGN_XOR 305
#define CSS_ASGN_AND 306
#define CSS_ASGN_RSHIFT 307
#define CSS_ASGN_LSHIFT 308
#define CSS_ASGN_MOD 309
#define CSS_ASGN_DIV 310
#define CSS_ASGN_MULT 311
#define CSS_ASGN_SUB 312
#define CSS_ASGN_ADD 313
#define CSS_OR 314
#define CSS_AND 315
#define CSS_NE 316
#define CSS_EQ 317
#define CSS_LE 318
#define CSS_LT 319
#define CSS_GE 320
#define CSS_GT 321
#define CSS_RSHIFT 322
#define CSS_LSHIFT 323
#define CSS_MINMIN 324
#define CSS_PLUSPLUS 325
#define CSS_UNARY 326
#define CSS_NOT 327
#define CSS_UNARYMINUS 328
#define CSS_SCOPER 329
#define CSS_POINTSAT 330




/* Copy the first part of user declarations.  */
#line 18 "css_parse.y"


#include "css_machine.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "css_builtin.h"
#include "ta_css.h"
#ifdef GUI_IV
#include "css_iv.h"
#endif // GUI_IV
#ifdef GUI_QT
#include "css_qt.h"
#include "css_qtdata.h"
#include "css_qtdialog.h"
#endif

#include <sstream>

#if defined(SUN4) && !defined(__GNUG__) && !defined(SOLARIS)
#include <alloca.h>
#endif

#if defined(SGI) || defined(SGIdebug)
#include <alloca.h>
#endif

void yyerror(char* s);
/* int yyparse(); */
int yylex();

#define Code1(x) 	cssMisc::cur_top->Prog()->Code(x)
#define Code2(x,y) 	cssMisc::cur_top->Prog()->Code(x); cssMisc::cur_top->Prog()->Code(y)
#define Code3(x,y,z) 	cssMisc::cur_top->Prog()->Code(x); cssMisc::cur_top->Prog()->Code(y); cssMisc::cur_top->Prog()->Code(z)
#define Code4(w,x,y,z) 	cssMisc::cur_top->Prog()->Code(w); cssMisc::cur_top->Prog()->Code(x); cssMisc::cur_top->Prog()->Code(y); cssMisc::cur_top->Prog()->Code(z);



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
#line 58 "css_parse.y"
typedef struct YYSTYPE {
  cssElPlusIVal el_ival;
  cssElPtr     	el;		/* for coding */
  int		ival;		/* for program indexes (progdx) and other ints */
  char*        	nm;
} YYSTYPE;
/* Line 190 of yacc.c.  */
#line 270 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 213 of yacc.c.  */
#line 282 "y.tab.c"

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
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
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
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (struct yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
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
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1709

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  98
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  96
/* YYNRULES -- Number of rules. */
#define YYNRULES  293
/* YYNRULES -- Number of states. */
#define YYNSTATES  516

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   330

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      92,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    81,    68,     2,
      88,    94,    79,    77,    50,    78,    87,    80,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    63,    93,
       2,    51,     2,    62,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    89,     2,    95,    67,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    49,    66,    96,    97,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    64,    65,    69,    70,    71,    72,
      73,    74,    75,    76,    82,    83,    84,    85,    86,    90,
      91
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     7,    11,    15,    18,    21,    24,
      28,    30,    32,    34,    36,    38,    40,    42,    44,    47,
      50,    53,    56,    59,    62,    65,    68,    70,    72,    75,
      78,    82,    85,    89,    93,    97,    99,   103,   107,   111,
     115,   119,   123,   124,   127,   132,   134,   136,   138,   140,
     142,   144,   149,   157,   164,   171,   173,   176,   180,   187,
     190,   193,   195,   197,   200,   202,   205,   207,   209,   213,
     222,   230,   235,   237,   241,   243,   245,   247,   249,   251,
     255,   257,   259,   262,   264,   267,   269,   271,   273,   275,
     278,   280,   282,   285,   288,   291,   294,   296,   301,   308,
     309,   311,   314,   318,   323,   328,   334,   341,   344,   345,
     347,   350,   355,   360,   365,   371,   376,   379,   383,   386,
     390,   393,   397,   401,   406,   408,   412,   414,   418,   421,
     424,   426,   430,   432,   434,   437,   440,   443,   446,   449,
     453,   457,   459,   462,   465,   469,   471,   473,   476,   479,
     482,   484,   486,   488,   491,   493,   495,   497,   499,   502,
     504,   506,   510,   515,   518,   521,   523,   525,   527,   531,
     534,   536,   538,   540,   547,   549,   559,   570,   581,   593,
     595,   598,   600,   605,   610,   612,   617,   622,   626,   630,
     632,   634,   638,   640,   642,   644,   646,   648,   650,   654,
     658,   662,   666,   670,   674,   678,   682,   686,   690,   694,
     698,   702,   708,   713,   717,   723,   727,   731,   734,   737,
     741,   745,   749,   753,   757,   761,   767,   771,   775,   779,
     783,   787,   791,   795,   799,   803,   807,   810,   813,   816,
     819,   822,   825,   830,   835,   840,   842,   846,   848,   850,
     852,   854,   856,   858,   860,   862,   865,   869,   872,   876,
     880,   883,   886,   890,   895,   900,   905,   909,   911,   913,
     915,   917,   919,   921,   923,   925,   927,   929,   931,   933,
     935,   937,   939,   941,   943,   945,   949,   951,   953,   954,
     956,   960,   962,   964
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
      99,     0,    -1,    -1,    99,    92,    -1,    99,   102,   101,
      -1,    99,   108,   101,    -1,    99,   173,    -1,    99,   110,
      -1,    99,   154,    -1,    99,     1,   101,    -1,    93,    -1,
      93,    -1,    92,    -1,   103,    -1,   104,    -1,   105,    -1,
     106,    -1,   107,    -1,     3,    11,    -1,     4,    16,    -1,
       4,    15,    -1,     5,    15,    -1,     6,    16,    -1,     6,
      15,    -1,     7,    15,    -1,     7,    16,    -1,     8,    -1,
       9,    -1,    36,   109,    -1,    39,   109,    -1,    39,   189,
      18,    -1,    38,   109,    -1,    38,   189,    18,    -1,    37,
     177,   183,    -1,    37,   177,   177,    -1,    40,    -1,    40,
     189,   177,    -1,    40,   189,    13,    -1,    40,   189,    37,
      -1,    40,   189,    40,    -1,    40,   189,    19,    -1,    40,
     189,    18,    -1,    -1,   189,   187,    -1,    88,   189,   187,
      94,    -1,   111,    -1,   112,    -1,   113,    -1,   116,    -1,
     121,    -1,   139,    -1,   148,   146,   193,   100,    -1,   148,
     183,    51,   189,   174,   193,   100,    -1,   182,    51,   189,
     174,   193,   100,    -1,   148,   183,   189,   114,   193,   100,
      -1,   115,    -1,   114,   115,    -1,    89,   174,    95,    -1,
     117,    49,   118,    96,   193,   100,    -1,    21,   183,    -1,
      21,    18,    -1,    21,    -1,   119,    -1,   118,   119,    -1,
     120,    -1,   120,    50,    -1,     1,    -1,   183,    -1,   183,
      51,    10,    -1,    20,   122,    49,   138,   130,    96,   193,
     100,    -1,    20,   122,    49,   138,    96,   193,   100,    -1,
      20,   124,   193,   100,    -1,   123,    -1,   123,    63,   125,
      -1,   183,    -1,    18,    -1,   183,    -1,    18,    -1,   126,
      -1,   125,    50,   126,    -1,   127,    -1,   149,    -1,   128,
     149,    -1,   129,    -1,   128,   129,    -1,    45,    -1,    47,
      -1,    44,    -1,   131,    -1,   130,   131,    -1,   132,    -1,
     136,    -1,   116,   138,    -1,    45,    63,    -1,    44,    63,
      -1,    46,    63,    -1,     1,    -1,   148,   183,   100,   138,
      -1,   148,   183,   133,   134,   100,   138,    -1,    -1,   135,
      -1,   134,   135,    -1,    89,    10,    95,    -1,   137,   142,
     100,   138,    -1,    18,   142,   100,   138,    -1,    97,    18,
     142,   100,   138,    -1,    47,    97,    18,   142,   100,   138,
      -1,   148,   183,    -1,    -1,    17,    -1,   138,    17,    -1,
     140,   142,   193,   100,    -1,   140,   142,   193,   171,    -1,
     148,    13,   142,   100,    -1,   148,    13,   142,   193,   171,
      -1,   141,   142,   193,   172,    -1,   148,   183,    -1,   148,
     151,   183,    -1,   151,    18,    -1,   151,    97,    18,    -1,
      88,    94,    -1,    88,   143,    94,    -1,    88,    94,    43,
      -1,    88,   143,    94,    43,    -1,   144,    -1,   143,    50,
     144,    -1,   145,    -1,   145,    51,   176,    -1,   148,    16,
      -1,   148,    12,    -1,   147,    -1,   146,    50,   147,    -1,
     183,    -1,   149,    -1,    41,   149,    -1,    42,   149,    -1,
      43,   149,    -1,    47,   149,    -1,    48,   149,    -1,    48,
      47,   149,    -1,    47,    48,   149,    -1,   150,    -1,   150,
     153,    -1,   150,    68,    -1,   150,   153,    68,    -1,    18,
      -1,    19,    -1,   151,    22,    -1,    18,    90,    -1,    19,
      90,    -1,    18,    -1,    19,    -1,    79,    -1,   153,    79,
      -1,   155,    -1,   167,    -1,   156,    -1,   163,    -1,   174,
     100,    -1,   161,    -1,   165,    -1,    30,   189,   100,    -1,
      30,   189,   174,   100,    -1,    31,   100,    -1,    32,   100,
      -1,   159,    -1,   157,    -1,   171,    -1,    28,   158,    63,
      -1,    29,    63,    -1,    10,    -1,    11,    -1,    12,    -1,
     160,    88,   189,   174,    94,   171,    -1,    27,    -1,   162,
      88,   192,   174,   192,   193,    94,   154,   193,    -1,   162,
      88,   192,   174,   192,   190,   193,    94,   154,   193,    -1,
     162,    88,   190,   192,   174,   192,   193,    94,   154,   193,
      -1,   162,    88,   190,   192,   174,   192,   190,   193,    94,
     154,   193,    -1,    33,    -1,   164,   154,    -1,    24,    -1,
     166,   170,   100,   193,    -1,   166,   170,   154,   193,    -1,
      23,    -1,   168,   170,   155,   193,    -1,   168,   170,   167,
     193,    -1,   169,   155,   193,    -1,   169,   167,   193,    -1,
      25,    -1,    26,    -1,    88,   174,    94,    -1,    49,    -1,
      49,    -1,    96,    -1,   176,    -1,   175,    -1,   180,    -1,
     174,    51,   174,    -1,   174,    51,    49,    -1,   174,    61,
     174,    -1,   174,    60,   174,    -1,   174,    59,   174,    -1,
     174,    58,   174,    -1,   174,    57,   174,    -1,   174,    56,
     174,    -1,   174,    55,   174,    -1,   174,    54,   174,    -1,
     174,    53,   174,    -1,   174,    52,   174,    -1,    34,   189,
     152,    -1,    34,   189,   152,    88,    94,    -1,    34,   189,
     152,   115,    -1,    35,   189,   174,    -1,    35,    89,    95,
     189,   174,    -1,   174,    64,   174,    -1,   174,    65,   174,
      -1,    68,   174,    -1,    79,   174,    -1,   174,    74,   174,
      -1,   174,    73,   174,    -1,   174,    72,   174,    -1,   174,
      71,   174,    -1,   174,    70,   174,    -1,   174,    69,   174,
      -1,   174,    62,   174,    63,   174,    -1,   174,    77,   174,
      -1,   174,    78,   174,    -1,   174,    79,   174,    -1,   174,
      80,   174,    -1,   174,    81,   174,    -1,   174,    76,   174,
      -1,   174,    75,   174,    -1,   174,    68,   174,    -1,   174,
      67,   174,    -1,   174,    66,   174,    -1,   174,    83,    -1,
      83,   174,    -1,   174,    82,    -1,    82,   174,    -1,    78,
     174,    -1,    85,   174,    -1,   174,    89,   174,    95,    -1,
      88,   148,    94,   174,    -1,   148,    88,   174,    94,    -1,
     178,    -1,    88,   174,    94,    -1,    10,    -1,    11,    -1,
      12,    -1,    14,    -1,    36,    -1,    38,    -1,    39,    -1,
      13,    -1,   179,    94,    -1,   179,   187,    94,    -1,    13,
      88,    -1,   175,   184,   185,    -1,   176,   184,   185,    -1,
      87,   185,    -1,   151,   185,    -1,   181,    94,   193,    -1,
     181,   187,    94,   193,    -1,   175,   184,   185,    88,    -1,
     176,   184,   185,    88,    -1,   151,   185,    88,    -1,    16,
      -1,    16,    -1,    10,    -1,    11,    -1,    12,    -1,    14,
      -1,    13,    -1,    87,    -1,    91,    -1,    16,    -1,    11,
      -1,   186,    -1,    12,    -1,    14,    -1,    13,    -1,    19,
      -1,   177,    -1,   188,    -1,   187,    50,   188,    -1,   174,
      -1,    19,    -1,    -1,   191,    -1,   190,    50,   190,    -1,
     174,    -1,    93,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   163,   163,   164,   165,   166,   167,   170,   173,   174,
     177,   180,   181,   184,   185,   186,   187,   188,   193,   199,
     201,   202,   205,   208,   209,   212,   215,   219,   232,   234,
     236,   238,   240,   242,   246,   248,   250,   252,   254,   256,
     258,   260,   264,   265,   266,   270,   271,   272,   273,   274,
     275,   278,   288,   306,   326,   342,   343,   347,   350,   357,
     364,   371,   379,   380,   383,   384,   385,   388,   405,   425,
     429,   433,   439,   440,   443,   450,   472,   478,   486,   487,
     490,   500,   501,   504,   505,   508,   509,   510,   513,   514,
     517,   518,   519,   520,   521,   522,   523,   526,   538,   554,
     562,   563,   567,   571,   577,   593,   610,   630,   651,   652,
     653,   656,   662,   674,   682,   700,   714,   727,   753,   774,
     796,   797,   798,   799,   802,   803,   806,   807,   813,   821,
     831,   832,   839,   848,   849,   850,   851,   853,   854,   855,
     856,   859,   860,   866,   868,   876,   877,   878,   889,   894,
     898,   899,   902,   903,   906,   907,   910,   911,   914,   915,
     916,   917,   918,   919,   920,   921,   922,   923,   926,   935,
     947,   948,   949,   953,   978,   982,   989,   996,  1003,  1010,
    1016,  1019,  1024,  1035,  1040,  1048,  1053,  1057,  1066,  1078,
    1084,  1089,  1092,  1099,  1102,  1111,  1112,  1116,  1117,  1118,
    1128,  1129,  1130,  1131,  1132,  1133,  1134,  1135,  1136,  1137,
    1138,  1139,  1140,  1142,  1143,  1144,  1145,  1146,  1147,  1148,
    1149,  1150,  1151,  1152,  1153,  1154,  1155,  1156,  1157,  1158,
    1159,  1160,  1161,  1162,  1163,  1164,  1165,  1166,  1167,  1168,
    1169,  1170,  1171,  1172,  1178,  1184,  1185,  1189,  1190,  1191,
    1192,  1195,  1196,  1197,  1201,  1202,  1203,  1218,  1234,  1235,
    1243,  1244,  1249,  1250,  1265,  1267,  1285,  1309,  1313,  1315,
    1317,  1319,  1323,  1327,  1333,  1334,  1337,  1340,  1341,  1346,
    1347,  1348,  1349,  1350,  1353,  1354,  1357,  1358,  1361,  1364,
    1365,  1368,  1371,  1374
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CSS_PP_INCLUDE", "CSS_PP_DEFINE",
  "CSS_PP_UNDEF", "CSS_PP_IFDEF", "CSS_PP_IFNDEF", "CSS_PP_ELSE",
  "CSS_PP_ENDIF", "CSS_NUMBER", "CSS_STRING", "CSS_VAR", "CSS_FUN",
  "CSS_PTR", "CSS_PP_DEF", "CSS_NAME", "CSS_COMMENT", "CSS_TYPE",
  "CSS_PTRTYPE", "CSS_CLASS", "CSS_ENUM", "CSS_SCPTYPE", "CSS_WHILE",
  "CSS_DO", "CSS_IF", "CSS_ELSE", "CSS_SWITCH", "CSS_CASE", "CSS_DEFAULT",
  "CSS_RETURN", "CSS_BREAK", "CSS_CONTINUE", "CSS_FOR", "CSS_NEW",
  "CSS_DELETE", "CSS_COMMAND", "CSS_ALIAS", "CSS_REMOVE", "CSS_TYPECMD",
  "CSS_HELP", "CSS_EXTERN", "CSS_STATIC", "CSS_CONST", "CSS_PRIVATE",
  "CSS_PUBLIC", "CSS_PROTECTED", "CSS_VIRTUAL", "CSS_INLINE", "'{'", "','",
  "'='", "CSS_ASGN_OR", "CSS_ASGN_XOR", "CSS_ASGN_AND", "CSS_ASGN_RSHIFT",
  "CSS_ASGN_LSHIFT", "CSS_ASGN_MOD", "CSS_ASGN_DIV", "CSS_ASGN_MULT",
  "CSS_ASGN_SUB", "CSS_ASGN_ADD", "'?'", "':'", "CSS_OR", "CSS_AND", "'|'",
  "'^'", "'&'", "CSS_NE", "CSS_EQ", "CSS_LE", "CSS_LT", "CSS_GE", "CSS_GT",
  "CSS_RSHIFT", "CSS_LSHIFT", "'+'", "'-'", "'*'", "'/'", "'%'",
  "CSS_MINMIN", "CSS_PLUSPLUS", "CSS_UNARY", "CSS_NOT", "CSS_UNARYMINUS",
  "'.'", "'('", "'['", "CSS_SCOPER", "CSS_POINTSAT", "'\\n'", "';'", "')'",
  "']'", "'}'", "'~'", "$accept", "toplev", "term", "crterm", "pproc",
  "ppinclude", "ppdefine", "ppifdef", "ppelse", "ppendif", "command",
  "cmd_args", "defn", "vardefn", "vardefin", "arraydefn", "arraydims",
  "arraydim", "enumdefn", "enumname", "enums", "enumline", "enumitms",
  "classdefn", "classhead", "classnm", "classfwd", "classinh", "classpar",
  "classtyp", "inh_mods", "inh_mod", "membs", "membline", "membdefn",
  "membardimstop", "membardims", "membardim", "methdefn", "methname",
  "classcmt", "fundefn", "fundname", "methdname", "funargs", "arglist",
  "argdefn", "subargdefn", "tynames", "tyname", "type", "type_el",
  "typeorscp", "scopetype", "typeonly", "ptrs", "stmt", "noifstmt",
  "nodostmt", "caseitem", "caseexpr", "switchblock", "switch", "forloop",
  "for", "doloop", "do", "whiloop", "while", "ifstmt", "if", "else",
  "cond", "bra", "mbr_bra", "ket", "expr", "comb_expr", "primitive",
  "anycmd", "normfuncall", "normfun", "memb_expr", "membfun", "undefname",
  "name", "getmemb", "membname", "membnms", "exprlist", "exprlsel",
  "argstop", "stmtlist", "stmtel", "scend", "end", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   123,
      44,    61,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,    63,    58,   314,   315,   124,    94,    38,   316,
     317,   318,   319,   320,   321,   322,   323,    43,    45,    42,
      47,    37,   324,   325,   326,   327,   328,    46,    40,    91,
     329,   330,    10,    59,    41,    93,   125,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    98,    99,    99,    99,    99,    99,    99,    99,    99,
     100,   101,   101,   102,   102,   102,   102,   102,   103,   104,
     104,   104,   105,   105,   105,   105,   106,   107,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   109,   109,   109,   110,   110,   110,   110,   110,
     110,   111,   112,   112,   113,   114,   114,   115,   116,   117,
     117,   117,   118,   118,   119,   119,   119,   120,   120,   121,
     121,   121,   122,   122,   123,   123,   124,   124,   125,   125,
     126,   127,   127,   128,   128,   129,   129,   129,   130,   130,
     131,   131,   131,   131,   131,   131,   131,   132,   132,   133,
     134,   134,   135,   136,   136,   136,   136,   137,   138,   138,
     138,   139,   139,   139,   139,   139,   140,   141,   141,   141,
     142,   142,   142,   142,   143,   143,   144,   144,   145,   145,
     146,   146,   147,   148,   148,   148,   148,   148,   148,   148,
     148,   149,   149,   149,   149,   150,   150,   150,   151,   151,
     152,   152,   153,   153,   154,   154,   155,   155,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   157,   157,
     158,   158,   158,   159,   160,   161,   161,   161,   161,   162,
     163,   164,   165,   165,   166,   167,   167,   167,   167,   168,
     169,   170,   171,   172,   173,   174,   174,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   176,   176,   176,
     176,   177,   177,   177,   178,   178,   178,   179,   180,   180,
     180,   180,   180,   180,   181,   181,   181,   182,   183,   183,
     183,   183,   183,   183,   184,   184,   185,   185,   185,   186,
     186,   186,   186,   186,   187,   187,   188,   188,   189,   190,
     190,   191,   192,   193
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     3,     3,     2,     2,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     2,     2,
       3,     2,     3,     3,     3,     1,     3,     3,     3,     3,
       3,     3,     0,     2,     4,     1,     1,     1,     1,     1,
       1,     4,     7,     6,     6,     1,     2,     3,     6,     2,
       2,     1,     1,     2,     1,     2,     1,     1,     3,     8,
       7,     4,     1,     3,     1,     1,     1,     1,     1,     3,
       1,     1,     2,     1,     2,     1,     1,     1,     1,     2,
       1,     1,     2,     2,     2,     2,     1,     4,     6,     0,
       1,     2,     3,     4,     4,     5,     6,     2,     0,     1,
       2,     4,     4,     4,     5,     4,     2,     3,     2,     3,
       2,     3,     3,     4,     1,     3,     1,     3,     2,     2,
       1,     3,     1,     1,     2,     2,     2,     2,     2,     3,
       3,     1,     2,     2,     3,     1,     1,     2,     2,     2,
       1,     1,     1,     2,     1,     1,     1,     1,     2,     1,
       1,     3,     4,     2,     2,     1,     1,     1,     3,     2,
       1,     1,     1,     6,     1,     9,    10,    10,    11,     1,
       2,     1,     4,     4,     1,     4,     4,     3,     3,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     5,     4,     3,     5,     3,     3,     2,     2,     3,
       3,     3,     3,     3,     3,     5,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     2,     2,
       2,     2,     4,     4,     4,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     2,     3,     3,
       2,     2,     3,     4,     4,     4,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     0,     1,
       3,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,    26,
      27,   247,   248,   249,   254,   250,   267,   145,   146,     0,
      61,   184,   181,   189,   190,   174,     0,     0,   288,     0,
       0,   179,   288,   288,   288,     0,   288,   288,   288,     0,
       0,     0,     0,     0,   192,     0,     0,     0,     0,     0,
       0,     0,     0,     3,   194,     0,    13,    14,    15,    16,
      17,     0,     7,    45,    46,    47,    48,     0,    49,    50,
       0,     0,     0,   133,   141,     0,     8,   154,   156,   166,
     165,     0,   159,     0,   157,     0,   160,     0,   155,     0,
       0,   167,     6,     0,   196,   195,   245,     0,   197,     0,
       0,    12,    11,     9,    18,    20,    19,    21,    23,    22,
      24,    25,   257,   148,   149,   269,   270,   271,   273,   272,
     268,    75,     0,    72,   293,    74,    60,    59,   170,   171,
     172,     0,   169,     0,    10,   163,   164,     0,     0,     0,
     288,    28,     0,   251,   252,   253,     0,    31,     0,    29,
       0,     0,   134,     0,   135,   136,     0,   137,     0,   138,
       0,     0,   217,   240,   218,   239,   237,   241,   277,   279,
     281,   280,   276,   282,   283,   260,   278,     0,     0,     4,
       5,     0,     0,   293,   293,   273,     0,     0,     0,   293,
     130,     0,   132,   143,   152,   142,   118,   147,     0,   261,
     288,     0,   180,     0,     0,     0,   293,   293,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   238,   236,
       0,   158,   274,   275,     0,     0,   287,   255,   286,     0,
     284,   293,     0,   288,   108,     0,     0,   168,   161,     0,
     150,   151,   210,   288,   213,     0,    43,    34,    33,   145,
     145,    37,    41,    40,    38,    39,    36,   140,   139,     0,
     246,    66,     0,    62,    64,    67,   120,     0,   124,   126,
       0,     0,     0,   293,     0,     0,     0,   117,   288,     0,
     144,   153,   119,   266,     0,   292,   291,     0,   289,     0,
       0,   293,   293,   293,   293,   187,   188,   199,   198,   209,
     208,   207,   206,   205,   204,   203,   202,   201,   200,     0,
     215,   216,   235,   234,   233,   224,   223,   222,   221,   220,
     219,   232,   231,   226,   227,   228,   229,   230,     0,   258,
     259,     0,   256,   262,   293,     0,   109,     0,    87,    85,
      86,    73,    78,    80,     0,    83,    81,    71,   162,     0,
       0,   212,     0,     0,   243,   293,    63,    65,     0,   122,
       0,   121,     0,   129,   128,   111,   112,   193,   115,   113,
       0,   244,   131,   132,    51,     0,   293,    55,     0,     0,
       0,     0,   191,   182,   183,   185,   186,     0,   242,   264,
     265,   285,   263,   293,    96,   110,   145,     0,     0,     0,
       0,   293,     0,   108,     0,    88,    90,    91,     0,     0,
       0,    84,    82,   211,     0,   214,    44,     0,    68,   125,
     123,   127,   114,   293,    56,     0,     0,   290,     0,   293,
     225,     0,     0,    94,    93,    95,     0,     0,     0,    92,
     293,    89,     0,    99,    79,    57,    58,     0,    54,   173,
     293,   293,     0,    53,   108,     0,    70,     0,     0,   108,
     108,     0,    52,   293,     0,     0,     0,   104,     0,   108,
      69,   103,    97,     0,     0,   100,     0,     0,     0,   293,
     108,   105,     0,   108,   101,     0,   293,   293,   175,   106,
     102,    98,   293,   177,   176,   178
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,   135,   103,    55,    56,    57,    58,    59,    60,
      61,   141,    62,    63,    64,    65,   396,   371,   423,    67,
     282,   283,   284,    68,   122,   123,   124,   361,   362,   363,
     364,   365,   424,   425,   426,   481,   494,   495,   427,   428,
     357,    69,    70,    71,   183,   287,   288,   289,   189,   190,
     160,    73,    74,   161,   262,   195,    76,    77,    78,    79,
     131,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,   204,    91,   388,    92,    93,    94,    95,   174,
      96,    97,    98,    99,   100,   285,   244,   199,   176,   266,
     250,   133,   307,   308,   309,   256
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -376
static const short int yypact[] =
{
    -376,   648,  -376,   -73,    40,   187,     6,   209,   215,  -376,
    -376,  -376,  -376,  -376,   -15,  -376,  -376,   -14,    54,   400,
     578,  -376,  -376,  -376,  -376,  -376,    25,    83,  -376,    66,
      66,  -376,  -376,    84,    92,   181,    92,    92,    61,   249,
     249,   249,    89,    91,  -376,  1023,  1023,  1023,  1023,  1023,
    1023,   340,  1023,  -376,  -376,   -73,  -376,  -376,  -376,  -376,
    -376,   -73,  -376,  -376,  -376,  -376,  -376,   132,  -376,  -376,
     102,   102,   230,  -376,    32,   150,  -376,  -376,  -376,  -376,
    -376,   104,  -376,   110,  -376,   943,  -376,   118,  -376,   118,
     943,  -376,  -376,  1469,    45,    45,  -376,   443,  -376,   535,
     158,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,   122,   162,   165,  -376,   128,  -376,  -376,  -376,  -376,
    -376,   172,  -376,   797,  -376,  -376,  -376,   296,   169,  1023,
    -376,  -376,  1085,  -376,  -376,  -376,   355,  -376,  1103,  -376,
    1165,   214,  -376,   223,  -376,  -376,   249,  -376,   249,  -376,
     186,   460,    58,   168,    58,    58,    58,   168,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,   -17,  1293,  -376,
    -376,   389,   294,  -376,  -376,   102,   -14,    54,  1023,   257,
    -376,   614,    17,  -376,  -376,    60,  -376,  -376,   293,   228,
    -376,   859,  -376,  1023,   735,   943,  -376,  -376,  1005,  1023,
    1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,
    1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,
    1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,  -376,  -376,
    1023,  -376,  -376,  -376,   340,   340,   238,  -376,  1594,   -32,
    -376,  -376,   -19,  -376,   308,   131,    66,  -376,  -376,  1469,
    -376,  -376,   231,  -376,    58,  1085,   279,  -376,  -376,   146,
     180,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  1023,
    -376,  -376,    16,  -376,   280,   281,   291,     2,  -376,   288,
     139,     8,   289,    66,  1337,   614,    66,  -376,  -376,   251,
    -376,  -376,  -376,  -376,  1023,  -376,  1594,   -27,  -376,  1023,
    1381,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  1594,  1594,
    1594,  1594,  1594,  1594,  1594,  1594,  1594,  1594,  1594,  1555,
     843,   632,   779,   717,   924,   425,   425,   425,   425,   425,
     425,   483,   483,   461,   461,    58,    58,    58,  1203,   255,
     258,  1085,  -376,  -376,  -376,  1023,  -376,    37,  -376,  -376,
    -376,   298,  -376,  -376,   131,  -376,  -376,  -376,  -376,   261,
    1023,  -376,  1023,    10,    58,  -376,  -376,  -376,   339,  -376,
     152,   317,   248,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
     312,  -376,  -376,  -376,  -376,  1023,   251,  -376,  1425,  1023,
    1023,  1512,  -376,  -376,  -376,  -376,  -376,  1023,  -376,  -376,
    -376,  -376,  -376,  1594,  -376,  -376,   -29,   300,   307,   310,
      51,  -376,   356,   308,    46,  -376,  -376,  -376,   102,   614,
     131,  -376,  -376,  -376,  1248,    58,  -376,    66,  -376,  -376,
    -376,  -376,  -376,  1594,  -376,    66,   312,  -376,  1512,  1023,
    1620,    66,    66,  -376,  -376,  -376,   357,    66,   102,   365,
    -376,  -376,    66,     9,  -376,  -376,  -376,    66,  -376,  -376,
    1023,   333,   290,  -376,   308,   102,  -376,    66,    66,   308,
     308,   297,  -376,   333,   301,   302,   943,   365,    66,   308,
    -376,   365,   365,   375,    98,  -376,   304,   943,   943,  -376,
     308,   365,   309,   308,  -376,   943,  -376,  -376,  -376,   365,
    -376,   365,  -376,  -376,  -376,  -376
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -376,  -376,   -30,    69,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,   287,  -376,  -376,  -376,  -376,  -376,  -274,   405,  -376,
    -376,   126,  -376,  -376,  -376,  -376,  -376,  -376,    -4,  -376,
    -376,    70,  -376,    11,  -376,  -376,  -376,   -55,  -376,  -376,
    -266,  -376,  -376,  -376,   -66,  -376,    62,  -376,  -376,   149,
       1,   -33,  -376,     0,  -376,  -376,   -69,   -78,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,   -76,
    -376,  -376,   352,  -269,  -376,  -376,    68,  -376,    63,   -20,
    -376,  -376,  -376,  -376,  -376,    14,   351,   -48,  -376,   -86,
     108,    12,  -375,  -376,  -303,   138
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -289
static const short int yytable[] =
{
     136,    75,    72,   175,   400,   184,   152,   154,   155,   157,
     159,   249,   206,   252,   207,   146,   202,   281,   351,   101,
     102,   107,   386,   399,   447,   397,   115,   116,   117,   118,
     119,   351,   120,   125,   127,   128,   129,   130,   414,   153,
     153,   153,   153,   153,   137,   139,   142,   414,   148,   150,
     151,   104,   380,   177,   415,   416,    18,    44,    20,   182,
     351,   113,   352,   241,   416,    18,   305,    20,   298,    17,
      18,   188,   191,   112,   471,   354,   113,   279,    39,    40,
      41,   417,   418,   419,   420,    43,   192,    39,    40,    41,
     417,   418,   419,   420,    43,   483,   381,  -107,   449,   156,
     193,   134,   134,   258,   436,  -116,  -288,    17,    18,    17,
      18,   194,   375,   162,   163,   164,   165,   166,   167,   293,
     178,   442,   444,   277,   179,   278,   267,   313,   300,   314,
     180,   276,   242,   421,   422,   312,   243,   156,   158,   301,
     238,   239,   460,   422,   114,   470,   132,   240,   456,    17,
      18,   383,   265,   -35,   -35,   384,   153,   459,   153,   134,
     268,   168,   169,   170,   171,   248,   172,   248,   196,   173,
      17,    18,   197,   138,   311,   358,   359,   469,   360,   373,
     140,   181,   153,   290,   -42,   -42,   143,   493,   144,   145,
     182,   134,   200,    39,    40,    41,   349,   350,   201,    42,
      43,   259,   105,   106,   299,   297,   203,   264,   487,   253,
     248,   254,   304,   491,   492,   -77,   248,   143,   248,   144,
     145,   -76,   366,   501,   108,   109,   367,   271,   255,   368,
     110,   111,   272,   273,   509,   257,   113,   511,   -32,   -32,
     115,   116,   117,   185,   119,   197,   120,   198,   186,   187,
     143,   274,   144,   145,   275,   153,   294,   240,    11,    12,
      13,   385,    15,   389,   263,   355,   394,    17,    18,   306,
     113,   310,   -30,   -30,   188,   372,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   344,   345,   346,   347,  -146,   295,   348,   393,
     395,   302,    17,    18,   260,   261,   303,  -146,   188,   369,
     370,   291,   292,   147,   149,   356,  -146,   296,   114,   351,
     377,   432,   378,   248,   379,    39,    40,    41,   387,   382,
     370,    42,    43,   409,   315,   316,   410,   374,   430,   438,
     452,   168,   169,   170,   171,   433,   172,   153,   429,   173,
     440,    44,   462,   453,   153,   115,   116,   117,   118,   119,
     454,   120,   398,   455,   458,   475,   143,   401,   144,   145,
     153,   290,   415,   399,   486,   502,   493,   157,   286,   353,
     281,   143,   477,   144,   145,   497,   498,   366,   505,   115,
     116,   117,   118,   119,   510,   120,    66,   466,   376,   488,
     115,   116,   117,   118,   119,   468,   120,   499,   121,   248,
     153,   473,   474,   413,   153,   429,   464,   476,   506,   507,
     153,   390,   479,   480,   431,   461,   512,   482,   434,   504,
     435,   205,   439,   463,   392,   441,   245,   489,   490,   403,
     404,   405,   406,    11,    12,    13,    14,    15,   500,   411,
       0,    17,   246,   443,   503,     0,     0,   306,   448,     0,
       0,   168,   169,   170,   171,   450,   172,    32,    33,   173,
       0,     0,   197,     0,    39,    40,    41,     0,     0,     0,
      42,    43,   412,     0,     0,     0,   143,     0,   144,   145,
     231,   232,   233,   234,   235,   236,   237,   238,   239,     0,
       0,    45,     0,   437,   240,     0,     0,   306,     0,     0,
       0,    46,    47,     0,     0,    48,    49,     0,    50,     0,
      51,    52,     0,     0,   445,     0,     0,   247,   306,     0,
     235,   236,   237,   238,   239,    11,    12,    13,    14,    15,
     240,   451,     0,    17,   246,     0,     0,     0,     0,   457,
     233,   234,   235,   236,   237,   238,   239,     0,     0,    32,
      33,     0,   240,     0,     0,     0,    39,    40,    41,     0,
       0,   467,    42,    43,     0,     0,     0,   472,   115,   116,
     117,   118,   119,     0,   120,     0,   126,     0,   478,     0,
       0,     0,     0,    45,     0,     0,     0,     0,   484,   485,
       0,     0,     0,    46,    47,     0,     0,    48,    49,     0,
      50,   496,    51,    52,   115,   116,   117,   118,   119,   251,
     120,     0,     0,     0,     0,     0,     0,   508,     0,     0,
       0,     0,     0,     0,   513,   514,     0,     0,     2,     3,
     515,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,     0,    16,     0,    17,    18,    19,    20,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,     0,     0,     0,    42,    43,    44,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,    45,     0,     0,     0,
       0,   240,     0,     0,     0,     0,    46,    47,     0,     0,
      48,    49,     0,    50,     0,    51,    52,     0,     0,     0,
      53,     0,     0,     0,    54,    11,    12,    13,    14,    15,
       0,     0,     0,    17,    18,     0,     0,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     0,     0,     0,     0,     0,    39,    40,    41,     0,
       0,     0,    42,    43,    44,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,     0,     0,    45,     0,     0,   240,    11,    12,    13,
      14,    15,     0,    46,    47,    17,    18,    48,    49,     0,
      50,     0,    51,    52,     0,     0,     0,     0,   134,     0,
       0,    32,    33,     0,     0,     0,     0,     0,    39,    40,
      41,     0,     0,     0,    42,    43,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,     0,     0,    45,     0,     0,   240,    11,
      12,    13,    14,    15,     0,    46,    47,    17,    18,    48,
      49,     0,    50,     0,    51,    52,     0,     0,     0,     0,
     134,     0,     0,    32,    33,     0,     0,     0,     0,     0,
      39,    40,    41,     0,     0,     0,    42,    43,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,    45,     0,     0,
       0,     0,   240,     0,     0,     0,     0,    46,    47,     0,
       0,    48,    49,     0,    50,     0,    51,    52,     0,     0,
       0,     0,   305,    11,    12,    13,    14,    15,     0,     0,
       0,    17,    18,     0,     0,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,     0,
       0,     0,     0,     0,    39,    40,    41,     0,     0,     0,
      42,    43,    44,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,     0,     0,
       0,    45,     0,   240,     0,    11,    12,    13,    14,    15,
       0,    46,    47,    17,    18,    48,    49,     0,    50,     0,
      51,    52,     0,    11,    12,    13,    14,    15,     0,    32,
      33,    17,    18,     0,     0,     0,    39,    40,    41,     0,
       0,     0,    42,    43,   317,     0,     0,    32,    33,     0,
       0,     0,     0,     0,    39,    40,    41,     0,     0,     0,
      42,    43,     0,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    46,    47,     0,     0,    48,    49,     0,
      50,    45,    51,    52,     0,    11,    12,    13,    14,    15,
       0,    46,    47,    17,   246,    48,    49,     0,    50,     0,
      51,    52,     0,    11,    12,    13,    14,    15,     0,    32,
      33,   269,   246,     0,     0,     0,    39,    40,    41,     0,
       0,     0,    42,    43,     0,     0,     0,    32,    33,     0,
       0,     0,     0,     0,    39,    40,    41,     0,     0,     0,
      42,    43,     0,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    46,    47,     0,     0,    48,    49,     0,
      50,    45,    51,    52,     0,    11,    12,    13,    14,    15,
       0,    46,    47,   270,   246,    48,    49,     0,    50,     0,
      51,    52,     0,     0,     0,     0,     0,     0,     0,    32,
      33,     0,     0,     0,     0,     0,    39,    40,    41,     0,
       0,     0,    42,    43,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    46,    47,     0,     0,    48,    49,     0,
      50,     0,    51,    52,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,     0,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,     0,     0,     0,
       0,     0,   240,     0,     0,     0,     0,     0,   408,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,     0,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,     0,     0,     0,     0,     0,   240,     0,     0,
       0,     0,     0,   465,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,     0,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,     0,     0,     0,
       0,     0,   240,     0,     0,     0,     0,   280,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
       0,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,     0,     0,     0,     0,     0,   240,     0,     0,     0,
       0,   391,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,     0,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,     0,     0,     0,     0,     0,
     240,     0,     0,     0,     0,   402,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,     0,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,     0,
       0,     0,     0,     0,   240,     0,     0,     0,     0,   446,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,     0,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,     0,     0,     0,     0,     0,   240,     0,
       0,     0,   134,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,     0,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,     0,     0,     0,     0,
       0,   240,     0,     0,     0,   305,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   407,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,     0,
       0,     0,     0,     0,   240,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,     0,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,     0,     0,
       0,     0,   219,   240,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,     0,     0,     0,     0,     0,   240
};

static const short int yycheck[] =
{
      30,     1,     1,    51,   307,    71,    39,    40,    41,    42,
      43,    97,    90,    99,    90,    35,    85,     1,    50,    92,
      93,    15,   291,    50,   399,   299,    10,    11,    12,    13,
      14,    50,    16,    19,    20,    10,    11,    12,     1,    39,
      40,    41,    42,    43,    32,    33,    34,     1,    36,    37,
      38,    11,    50,    52,    17,    18,    19,    49,    21,    88,
      50,    90,    94,    93,    18,    19,    93,    21,    51,    18,
      19,    88,    72,    88,   449,    94,    90,    94,    41,    42,
      43,    44,    45,    46,    47,    48,    72,    41,    42,    43,
      44,    45,    46,    47,    48,   470,    94,    88,   401,    48,
      68,    93,    93,   133,    94,    88,    89,    18,    19,    18,
      19,    79,    96,    45,    46,    47,    48,    49,    50,   185,
      52,   390,   396,   156,    55,   158,   146,   205,    68,   205,
      61,   151,    87,    96,    97,   204,    91,    48,    47,    79,
      82,    83,    96,    97,    90,   448,    63,    89,    97,    18,
      19,    12,   140,    92,    93,    16,   156,   423,   158,    93,
     146,    11,    12,    13,    14,    97,    16,    99,    18,    19,
      18,    19,    22,    89,   204,    44,    45,   446,    47,   265,
      88,    49,   182,   182,    92,    93,    36,    89,    38,    39,
      88,    93,    88,    41,    42,    43,   244,   245,    88,    47,
      48,   133,    15,    16,   192,   191,    88,   139,   474,    51,
     142,    49,   200,   479,   480,    93,   148,    36,   150,    38,
      39,    93,   255,   489,    15,    16,   256,    13,    63,   259,
      15,    16,    18,    19,   500,    63,    90,   503,    92,    93,
      10,    11,    12,    13,    14,    22,    16,    97,    18,    19,
      36,    37,    38,    39,    40,   255,   188,    89,    10,    11,
      12,   291,    14,   293,    95,   253,   296,    18,    19,   201,
      90,   203,    92,    93,    88,   263,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,    68,    50,   240,   295,
     298,    18,    18,    19,    18,    19,    88,    79,    88,    88,
      89,   183,   184,    36,    37,    17,    88,   189,    90,    50,
      50,   364,    51,   265,    43,    41,    42,    43,    49,    51,
      89,    47,    48,    88,   206,   207,    88,   279,    50,    10,
     416,    11,    12,    13,    14,    94,    16,   357,   357,    19,
      43,    49,   428,    63,   364,    10,    11,    12,    13,    14,
      63,    16,   304,    63,    18,    18,    36,   309,    38,    39,
     380,   380,    17,    50,    94,    10,    89,   420,    94,   251,
       1,    36,   458,    38,    39,    94,    94,   430,    94,    10,
      11,    12,    13,    14,    95,    16,     1,   437,   282,   475,
      10,    11,    12,    13,    14,   445,    16,   486,    18,   351,
     420,   451,   452,   355,   424,   424,   430,   457,   497,   498,
     430,   293,   462,   463,   364,   424,   505,   467,   370,   494,
     372,    89,   380,   429,   295,   382,    95,   477,   478,   311,
     312,   313,   314,    10,    11,    12,    13,    14,   488,   351,
      -1,    18,    19,   395,   494,    -1,    -1,   399,   400,    -1,
      -1,    11,    12,    13,    14,   407,    16,    34,    35,    19,
      -1,    -1,    22,    -1,    41,    42,    43,    -1,    -1,    -1,
      47,    48,   354,    -1,    -1,    -1,    36,    -1,    38,    39,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    -1,
      -1,    68,    -1,   375,    89,    -1,    -1,   449,    -1,    -1,
      -1,    78,    79,    -1,    -1,    82,    83,    -1,    85,    -1,
      87,    88,    -1,    -1,   396,    -1,    -1,    94,   470,    -1,
      79,    80,    81,    82,    83,    10,    11,    12,    13,    14,
      89,   413,    -1,    18,    19,    -1,    -1,    -1,    -1,   421,
      77,    78,    79,    80,    81,    82,    83,    -1,    -1,    34,
      35,    -1,    89,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,   443,    47,    48,    -1,    -1,    -1,   449,    10,    11,
      12,    13,    14,    -1,    16,    -1,    18,    -1,   460,    -1,
      -1,    -1,    -1,    68,    -1,    -1,    -1,    -1,   470,   471,
      -1,    -1,    -1,    78,    79,    -1,    -1,    82,    83,    -1,
      85,   483,    87,    88,    10,    11,    12,    13,    14,    94,
      16,    -1,    -1,    -1,    -1,    -1,    -1,   499,    -1,    -1,
      -1,    -1,    -1,    -1,   506,   507,    -1,    -1,     0,     1,
     512,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    16,    -1,    18,    19,    20,    21,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    -1,    -1,    47,    48,    49,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    68,    -1,    -1,    -1,
      -1,    89,    -1,    -1,    -1,    -1,    78,    79,    -1,    -1,
      82,    83,    -1,    85,    -1,    87,    88,    -1,    -1,    -1,
      92,    -1,    -1,    -1,    96,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,    -1,    47,    48,    49,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    -1,    -1,    68,    -1,    -1,    89,    10,    11,    12,
      13,    14,    -1,    78,    79,    18,    19,    82,    83,    -1,
      85,    -1,    87,    88,    -1,    -1,    -1,    -1,    93,    -1,
      -1,    34,    35,    -1,    -1,    -1,    -1,    -1,    41,    42,
      43,    -1,    -1,    -1,    47,    48,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    -1,    -1,    68,    -1,    -1,    89,    10,
      11,    12,    13,    14,    -1,    78,    79,    18,    19,    82,
      83,    -1,    85,    -1,    87,    88,    -1,    -1,    -1,    -1,
      93,    -1,    -1,    34,    35,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    -1,    -1,    -1,    47,    48,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    68,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    -1,    -1,    78,    79,    -1,
      -1,    82,    83,    -1,    85,    -1,    87,    88,    -1,    -1,
      -1,    -1,    93,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    -1,    -1,    -1,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,    -1,
      47,    48,    49,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    -1,    -1,
      -1,    68,    -1,    89,    -1,    10,    11,    12,    13,    14,
      -1,    78,    79,    18,    19,    82,    83,    -1,    85,    -1,
      87,    88,    -1,    10,    11,    12,    13,    14,    -1,    34,
      35,    18,    19,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,    -1,    47,    48,    49,    -1,    -1,    34,    35,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,    -1,
      47,    48,    -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    -1,    -1,    82,    83,    -1,
      85,    68,    87,    88,    -1,    10,    11,    12,    13,    14,
      -1,    78,    79,    18,    19,    82,    83,    -1,    85,    -1,
      87,    88,    -1,    10,    11,    12,    13,    14,    -1,    34,
      35,    18,    19,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    34,    35,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,    -1,
      47,    48,    -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    -1,    -1,    82,    83,    -1,
      85,    68,    87,    88,    -1,    10,    11,    12,    13,    14,
      -1,    78,    79,    18,    19,    82,    83,    -1,    85,    -1,
      87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      35,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    -1,    -1,    82,    83,    -1,
      85,    -1,    87,    88,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    -1,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,    95,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    -1,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,
      -1,    -1,    -1,    95,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    -1,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    -1,    -1,    94,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      -1,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,
      -1,    94,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    -1,    94,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,    94,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    -1,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    -1,    93,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    -1,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,
      -1,    89,    -1,    -1,    -1,    93,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    89,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    -1,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    -1,    -1,
      -1,    -1,    62,    89,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,    89
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    99,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    16,    18,    19,    20,
      21,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    47,    48,    49,    68,    78,    79,    82,    83,
      85,    87,    88,    92,    96,   102,   103,   104,   105,   106,
     107,   108,   110,   111,   112,   113,   116,   117,   121,   139,
     140,   141,   148,   149,   150,   151,   154,   155,   156,   157,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   171,   173,   174,   175,   176,   178,   179,   180,   181,
     182,    92,    93,   101,    11,    15,    16,    15,    15,    16,
      15,    16,    88,    90,    90,    10,    11,    12,    13,    14,
      16,    18,   122,   123,   124,   183,    18,   183,    10,    11,
      12,   158,    63,   189,    93,   100,   100,   189,    89,   189,
      88,   109,   189,    36,    38,    39,   177,   109,   189,   109,
     189,   189,   149,   151,   149,   149,    48,   149,    47,   149,
     148,   151,   174,   174,   174,   174,   174,   174,    11,    12,
      13,    14,    16,    19,   177,   185,   186,   148,   174,   101,
     101,    49,    88,   142,   142,    13,    18,    19,    88,   146,
     147,   151,   183,    68,    79,   153,    18,    22,    97,   185,
      88,    88,   154,    88,   170,   170,   155,   167,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      89,   100,    87,    91,   184,   184,    19,    94,   174,   187,
     188,    94,   187,    51,    49,    63,   193,    63,   100,   174,
      18,    19,   152,    95,   174,   189,   187,   177,   183,    18,
      18,    13,    18,    19,    37,    40,   177,   149,   149,    94,
      94,     1,   118,   119,   120,   183,    94,   143,   144,   145,
     148,   193,   193,   142,   174,    50,   193,   183,    51,   189,
      68,    79,    18,    88,   189,    93,   174,   190,   191,   192,
     174,   100,   154,   155,   167,   193,   193,    49,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   185,
     185,    50,    94,   193,    94,   189,    17,   138,    44,    45,
      47,   125,   126,   127,   128,   129,   149,   100,   100,    88,
      89,   115,   189,   187,   174,    96,   119,    50,    51,    43,
      50,    94,    51,    12,    16,   100,   171,    49,   172,   100,
     193,    94,   147,   183,   100,   189,   114,   115,   174,    50,
     192,   174,    94,   193,   193,   193,   193,    63,    95,    88,
      88,   188,   193,   174,     1,    17,    18,    44,    45,    46,
      47,    96,    97,   116,   130,   131,   132,   136,   137,   148,
      50,   129,   149,    94,   174,   174,    94,   193,    10,   144,
      43,   176,   171,   174,   115,   193,    94,   190,   174,   192,
     174,   193,   142,    63,    63,    63,    97,   193,    18,   138,
      96,   131,   142,   183,   126,    95,   100,   193,   100,   171,
     192,   190,   193,   100,   100,    18,   100,   142,   193,   100,
     100,   133,   100,   190,   193,   193,    94,   138,   142,   100,
     100,   138,   138,    89,   134,   135,   193,    94,    94,   154,
     100,   138,    10,   100,   135,    94,   154,   154,   193,   138,
      95,   138,   154,   193,   193,   193
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


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
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

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
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
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
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
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

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


  yyvsp[0] = yylval;

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
	short int *yyss1 = yyss;


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
	short int *yyss1 = yyss;
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
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

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
        case 3:
#line 164 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Blank; }
    break;

  case 4:
#line 165 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 5:
#line 166 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return (yyvsp[-1].ival); }
    break;

  case 6:
#line 167 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 7:
#line 170 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 8:
#line 173 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 9:
#line 174 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Err; }
    break;

  case 10:
#line 177 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 11:
#line 180 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 12:
#line 181 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 18:
#line 193 "css_parse.y"
    {
            cssEl* tmp = (yyvsp[0].el).El();
	    cssMisc::cur_top->SetInclude((const char*)*tmp);
	    cssEl::Done((yyvsp[0].el).El()); }
    break;

  case 19:
#line 199 "css_parse.y"
    {
            cssMisc::Defines.Push(new cssDef(0, (yyvsp[0].nm))); }
    break;

  case 21:
#line 202 "css_parse.y"
    { cssMisc::Defines.Remove((yyvsp[0].el).El()); }
    break;

  case 22:
#line 205 "css_parse.y"
    { /* if its a name, its not defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 24:
#line 209 "css_parse.y"
    { /* if its a def, its defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 26:
#line 215 "css_parse.y"
    { /* if you see it, it applies to u*/
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog()); }
    break;

  case 28:
#line 232 "css_parse.y"
    {
            Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 29:
#line 234 "css_parse.y"
    {
	    Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 30:
#line 236 "css_parse.y"
    {
	    Code2((yyvsp[0].el),(yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 31:
#line 238 "css_parse.y"
    {
	    Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 32:
#line 240 "css_parse.y"
    {
	    Code2((yyvsp[0].el),(yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 33:
#line 242 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssRef((yyvsp[-1].el)));
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 34:
#line 246 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc; }
    break;

  case 35:
#line 248 "css_parse.y"
    {
	    Code1((yyvsp[0].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 36:
#line 250 "css_parse.y"
    {
	    Code2((yyvsp[0].el), (yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 37:
#line 252 "css_parse.y"
    {
	    Code2((yyvsp[0].el), (yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 38:
#line 254 "css_parse.y"
    {
	    Code2((yyvsp[0].el), (yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 39:
#line 256 "css_parse.y"
    {
	    Code2((yyvsp[0].el), (yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 40:
#line 258 "css_parse.y"
    {
	    Code2((yyvsp[0].el), (yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 41:
#line 260 "css_parse.y"
    {
	    Code2((yyvsp[0].el), (yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 42:
#line 264 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 44:
#line 266 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 51:
#line 278 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival));
	    if((yyvsp[-2].ival) < 0) (yyval.ival) = (yyvsp[-1].ival)-1; /* if no coding, its the end, else not */
	    else (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 52:
#line 288 "css_parse.y"
    {
 	    ((yyvsp[-6].el).El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    /* constants get dealt with differently */
	    if(tmp->ptr.ptr == (void*)&(cssMisc::Constants)) {
	      /* code in ConstExpr space the initializer */
	      cssMisc::ConstExprTop->Prog()->UnCode();	/* undo the end jump coding */
	      Code3(tmp->ptr, cssBI::init_asgn, cssBI::pop);
	      cssMisc::ConstExprTop->Run(); /* run the const expr assigning val */
	      cssMisc::CodeTop(); /* revert to coding using top */
	      (yyval.ival) = Code1(cssMisc::cur_top->Prog()->size+1); /* need to have an rval */
	    }
	    else {
	      cssMisc::cur_top->Prog()->UnCode();	/* undo the end jump coding */
	      (yyval.ival) = Code3(tmp->ptr, cssBI::init_asgn, cssBI::pop);
	    }
	    cssEl::Done(tmp); }
    break;

  case 53:
#line 306 "css_parse.y"
    {	/* no type information, get from last expr */
	    cssMisc::cur_top->Prog()->UnCode();	/* undo the end jump coding */
	    css_progdx actln = (yyvsp[-1].ival)-2;
	    if(cssMisc::cur_top->Prog()->insts[actln]->IsJump()) { /* if jmp, get before */
	      actln = (yyvsp[-3].ival)+1;	/* go for the first thing if the last is no good */
	    }
	    cssEl* extyp = cssMisc::cur_top->Prog()->insts[actln]->inst.El();
 	    extyp->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    (yyval.ival) = Code3(tmp->ptr, cssBI::init_asgn, cssBI::pop);
	    if(!cssMisc::cur_top->AmCmdProg()) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(),
			       "Warning: created implicit variable:",
			       tmp->ptr.El()->name,
			       "of type: ", extyp->GetTypeName()); }
	    cssEl::Done(tmp); }
    break;

  case 54:
#line 326 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-5].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->UnCode();	/* undo the end jump coding */
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[-5].el).El()); /* type is an arg */
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((yyvsp[-2].ival))); /* num of dims */
	    cssMisc::VoidArray.tmp_str = ((yyvsp[-5].el).El())->tmp_str;
	    cssMisc::VoidArray.MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    (yyval.ival) = Code3(tmp->ptr, cssBI::array_alloc, cssBI::pop);
	    cssEl::Done(tmp); }
    break;

  case 55:
#line 342 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 56:
#line 343 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 57:
#line 347 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 58:
#line 350 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
	    if(cssMisc::cur_enum->name.before(5) == "enum_") /* don't keep anon enums around */
	      delete cssMisc::cur_enum;
	    cssMisc::cur_enum = NULL; }
    break;

  case 59:
#line 357 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_enum = new cssEnumType((const char*)*nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum);
	    cssEl::Done(nm); }
    break;

  case 60:
#line 364 "css_parse.y"
    { /* redefining */
	    if((yyvsp[0].el).El()->GetType() != cssEl::T_EnumType) {
	      yyerror("Attempt to redefine non-enum type as an enum");
	      return cssProg::YY_Err; }
   	    cssMisc::cur_enum = (cssEnumType*)(yyvsp[0].el).El();
 	    cssMisc::cur_enum->enums->Reset(); /* prepare for new enums.. */
	    cssMisc::cur_enum->enum_cnt = 0; }
    break;

  case 61:
#line 371 "css_parse.y"
    { /* anonymous  */
	    String nm = "anonenum_"; nm += String(cssMisc::anon_type_cnt++); nm += "_";
            cssMisc::cur_enum = new cssEnumType((const char*)nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum); }
    break;

  case 66:
#line 385 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 67:
#line 388 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssSpace* en_own;
	    if(cssMisc::cur_class != NULL) en_own = cssMisc::cur_class->types;
	    else en_own = &(cssMisc::Enums);
	    cssElPtr itm_ptr = en_own->FindName((const char*)*nm); 	cssEnum* itm;
	    if(itm_ptr != 0) { /* redef */
	      itm = (cssEnum*)itm_ptr.El();
	      itm->val = cssMisc::cur_enum->enum_cnt;
	      itm->SetEnumType(cssMisc::cur_enum);
	    } else {
	      itm = new cssEnum(cssMisc::cur_enum, cssMisc::cur_enum->enum_cnt,
				(const char*)*nm);
	      en_own->Push(itm);
	    }
	    cssMisc::cur_enum->enum_cnt = itm->val + 1;
	    cssMisc::cur_enum->enums->Push(itm); cssEl::Done(nm); }
    break;

  case 68:
#line 405 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssSpace* en_own;
	    if(cssMisc::cur_class != NULL) en_own = cssMisc::cur_class->types;
	    else en_own = &(cssMisc::Enums);
	    cssElPtr itm_ptr = en_own->FindName((const char*)*nm);	cssEnum* itm;
	    if(itm_ptr != 0) { /* redef */
	      itm = (cssEnum*)itm_ptr.El();
	      itm->val = (int)*((yyvsp[0].el).El());
	      itm->SetEnumType(cssMisc::cur_enum);
	    }
	    else {
	      itm = new cssEnum(cssMisc::cur_enum, (int)*((yyvsp[0].el).El()), (const char*)*nm);
	      en_own->Push(itm);
	    }
	    cssMisc::cur_enum->enum_cnt = itm->val + 1;
	    cssMisc::cur_enum->enums->Push(itm); cssEl::Done(nm); }
    break;

  case 69:
#line 425 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[-4].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 70:
#line 429 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[-3].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 71:
#line 433 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 74:
#line 443 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssMisc::cur_class = new cssClassType((const char*)*nm);
	    cssMisc::cur_class->last_top = cssMisc::cur_top;
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
	    cssEl::Done(nm); }
    break;

  case 75:
#line 450 "css_parse.y"
    { /* redefining */
	    if((yyvsp[0].el).El()->GetType() != cssEl::T_ClassType) {
	      yyerror("Attempt to redefine non-class type as a class");
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_class = (cssClassType*)(yyvsp[0].el).El();
#ifdef GUI_IV
	    cssivSession::CancelClassEdits(cssMisc::cur_class);
#endif // GUI_IV
	    if(cssMisc::cur_class->last_top != cssMisc::cur_top) {
	      cssMisc::cur_class->last_top = cssMisc::cur_top;
	      cssMisc::cur_class->multi_space = true;
	    }
            cssMisc::cur_class->methods->Reset();
	    cssMisc::cur_class->members->Reset();
            cssMisc::cur_class->member_desc.Reset();
            cssMisc::cur_class->member_opts.Reset();
            cssMisc::cur_class->types->Reset();
	    cssMisc::cur_class->parents->Reset();
	    cssMisc::cur_class->AddBuiltins();  }
    break;

  case 76:
#line 472 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_class = new cssClassType((const char*)*nm);
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
            cssEl::Done(nm); }
    break;

  case 77:
#line 478 "css_parse.y"
    {
            if((yyvsp[0].el).El()->GetType() != cssEl::T_ClassType) {
              yyerror("Attempt to redefine non-class type as a class");
              return cssProg::YY_Err;
            }
            cssMisc::cur_class = (cssClassType*)(yyvsp[0].el).El(); }
    break;

  case 80:
#line 490 "css_parse.y"
    {
            if((yyvsp[0].el).El()->GetType() != cssEl::T_ClassType) {
	      yyerror("Cannot inherit from non-class type");
	      /* cssMisc::TypesSpace.Remove(cssMisc::cur_class); */
	      cssMisc::cur_top->types.Remove(cssMisc::cur_class);
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_class->AddParent((cssClassType*)((yyvsp[0].el).El())); }
    break;

  case 82:
#line 501 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); }
    break;

  case 91:
#line 518 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 92:
#line 519 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 96:
#line 523 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 97:
#line 526 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
            cssMisc::parsing_membdefn = true;
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssClassMember* mbr = new cssClassMember((yyvsp[-3].el).El(), (const char*)*nm);
            cssMisc::cur_class->members->Push(mbr);
            cssMisc::cur_class->GetComments(mbr, (yyvsp[0].el));
            cssMisc::parsing_membdefn = false;
            cssEl::Done(nm); }
    break;

  case 98:
#line 538 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-5].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
            cssMisc::parsing_membdefn = true;
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((yyvsp[-2].ival))); /* num of dims */
            cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[-5].el).El()); /* type is an arg */
            cssMisc::VoidArrayType.MakeTempToken(cssMisc::cur_top->Prog());
            cssArrayType* tmp = (cssArrayType*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssClassMember* mbr = new cssClassMember(tmp, tmp->name);
            cssMisc::cur_class->members->Push(mbr);
            cssMisc::cur_class->GetComments(mbr, (yyvsp[0].el));
            cssMisc::parsing_membdefn = false; }
    break;

  case 99:
#line 554 "css_parse.y"
    {
            /* swap places with void marker */
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_top->Prog()->Stack()->Push(&cssMisc::Void);
            cssMisc::cur_top->Prog()->Stack()->Push(nm); }
    break;

  case 100:
#line 562 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 101:
#line 563 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 102:
#line 567 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((int)*(yyvsp[-1].el).El())); }
    break;

  case 103:
#line 571 "css_parse.y"
    {       /* pre-declare function */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[-3].el).El();
            cssMisc::cur_class->GetComments(fun, (yyvsp[0].el));
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr, true); }
    break;

  case 104:
#line 577 "css_parse.y"
    {       /* constructor */
	    if((yyvsp[-3].el).El() != cssMisc::cur_class) {
	      yyerror("constructor-like method does not match current class type");
	      return cssProg::YY_Err; }
	    cssElPtr fun_ptr = cssMisc::cur_class->methods->FindName((const char*)cssMisc::cur_class->name);
	    cssMbrScriptFun* fun;
	    if(fun_ptr != 0)
	      fun = (cssMbrScriptFun*)fun_ptr.El();
	    else {
	      fun = new cssMbrScriptFun
		((const char*)cssMisc::cur_class->name, cssMisc::cur_class);
	      cssMisc::cur_class->methods->Push(fun);
	    }
	    fun->is_tor = true;
	    fun->argc = (yyvsp[-2].ival) + 1;	      	fun->GetArgDefs();
            cssMisc::cur_class->GetComments(fun, (yyvsp[0].el)); }
    break;

  case 105:
#line 593 "css_parse.y"
    {       /* destructor */
	    if((yyvsp[-3].el).El() != cssMisc::cur_class) {
	      yyerror("destructor-like method does not match current class type");
	      return cssProg::YY_Err; }
	    String nm = "~";
	    nm += cssMisc::cur_class->name;
	    cssElPtr fun_ptr = cssMisc::cur_class->methods->FindName((const char*)nm);
	    cssMbrScriptFun* fun;
	    if(fun_ptr != 0)
	      fun = (cssMbrScriptFun*)fun_ptr.El();
	    else {
	      fun = new cssMbrScriptFun(nm, cssMisc::cur_class);
	      cssMisc::cur_class->methods->Push(fun);
	    }
	    fun->is_tor = true;
	    fun->argc = (yyvsp[-2].ival) + 1;		fun->GetArgDefs();
            cssMisc::cur_class->GetComments(fun, (yyvsp[0].el));}
    break;

  case 106:
#line 610 "css_parse.y"
    {       /* destructor */
	    if((yyvsp[-3].el).El() != cssMisc::cur_class) {
	      yyerror("destructor-like method does not match current class type");
	      return cssProg::YY_Err; }
	    String nm = "~";
	    nm += cssMisc::cur_class->name;
	    cssElPtr fun_ptr = cssMisc::cur_class->methods->FindName((const char*)nm);
	    cssMbrScriptFun* fun;
	    if(fun_ptr != 0)
	      fun = (cssMbrScriptFun*)fun_ptr.El();
	    else {
	      fun = new cssMbrScriptFun(nm, cssMisc::cur_class);
	      cssMisc::cur_class->methods->Push(fun);
	    }
	    fun->is_tor = true;
	    fun->is_virtual = true;
	    fun->argc = (yyvsp[-2].ival) + 1;		fun->GetArgDefs();
            cssMisc::cur_class->GetComments(fun, (yyvsp[0].el));}
    break;

  case 107:
#line 630 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-1].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssEl* nm = cssMisc::cur_top->Prog()->Stack()->Pop();  /* get rid of name */
	    cssElPtr fun_ptr = cssMisc::cur_class->methods->FindName((const char*)*nm);
	    cssMbrScriptFun* fun;
	    if(fun_ptr != 0) {
	      fun = (cssMbrScriptFun*)fun_ptr.El();
	      (yyval.el) = fun_ptr;
	    }
	    else {
	      fun = new cssMbrScriptFun((const char*)*nm, cssMisc::cur_class);
	      (yyval.el) = cssMisc::cur_class->methods->Push(fun);
	    }
	    if((yyvsp[-1].el).El()->tmp_str == "virtual") fun->is_virtual = true;
	    fun->SetRetvType((yyvsp[-1].el).El()); /* preserve type info for later if nec */
            cssEl::Done(nm); }
    break;

  case 108:
#line 651 "css_parse.y"
    { (yyval.el).Reset(); }
    break;

  case 109:
#line 652 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); }
    break;

  case 110:
#line 653 "css_parse.y"
    { (yyval.el) = (yyvsp[-1].el); }
    break;

  case 111:
#line 656 "css_parse.y"
    {	/* pre-declare function */
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival));	(yyval.ival) = (yyvsp[-1].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[-3].el).El();
	    fun->argc = (yyvsp[-2].ival); fun->GetArgDefs(); }
    break;

  case 112:
#line 662 "css_parse.y"
    {
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[-3].el).El();
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[0].ival)+1); (yyval.ival) = (yyvsp[-1].ival)-1;
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    cssScriptFun* bra_fun = (cssScriptFun*)(cssMisc::cur_top->Prog()->insts[(yyvsp[0].ival)]->inst.El());
	    cssMisc::cur_top->Prog()->insts[(yyvsp[0].ival)]->SetInst((yyvsp[-3].el)); /* replace bra with fun */
	    cssMisc::cur_top->RemoveVar(bra_fun);	/* get rid of the bra */
	    cssMisc::cur_top->SetPush(fun->fun); /* this is the one we want to push */
	    fun->Define(cssMisc::ConstExpr); }
    break;

  case 113:
#line 674 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 114:
#line 682 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-4].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[0].ival)+1);	(yyval.ival) = (yyvsp[-1].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[-3].el).El();
	    fun->SetRetvType((yyvsp[-4].el).El());
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    cssScriptFun* bra_fun = (cssScriptFun*)(cssMisc::cur_top->Prog()->insts[(yyvsp[0].ival)]->inst.El());
	    cssMisc::cur_top->Prog()->insts[(yyvsp[0].ival)]->SetInst((yyvsp[-3].el)); /* replace bra with existing one */
	    cssMisc::cur_top->RemoveVar(bra_fun);	/* get rid of the bra */
	    fun->fun->Reset();	/* reset it for new defn */
	    cssMisc::cur_top->SetPush(fun->fun); /* push this one */
	    fun->Define(cssMisc::ConstExpr); }
    break;

  case 115:
#line 700 "css_parse.y"
    {
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[-3].el).El();
	    cssClassType* cls = cssMisc::cur_class;
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr);
	    cssMisc::cur_method = fun; /* this is now the current method */
	    cssElPtr fun_ptr;  fun_ptr.SetNVirtMethod(cls, cls->methods->GetIndex(fun));
	    css_progdx nxt_ln = Code1(fun_ptr); /* code it so it shows up in a listing.. */
	    cssMisc::cur_top->Prog()->insts[ nxt_ln ]->SetDefn();
	    cssMisc::cur_top->SetPush(fun->fun); /* put it on the stack.. */
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine(nxt_ln+1); (yyval.ival) = (yyvsp[-1].ival)-1; }
    break;

  case 116:
#line 714 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-1].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssEl* nm = cssMisc::cur_top->Prog()->Stack()->Pop();  /* get rid of name */
	    cssScriptFun* fun = new cssScriptFun((const char*)*nm);
	    fun->SetRetvType((yyvsp[-1].el).El()); /* preserve type info for later if nec */
	    if((yyvsp[-1].el).El()->tmp_str == "extern") (yyval.el) = cssMisc::Externs.PushUniqNameOld(fun);
	    else (yyval.el) = cssMisc::cur_top->AddStatic(fun);
            cssEl::Done(nm); }
    break;

  case 117:
#line 727 "css_parse.y"
    {
            cssMisc::cur_scope = NULL;
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-2].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if(((yyvsp[-1].el).El())->GetType() != cssEl::T_ClassType) {
	      yyerror("attempt to define member function in non-class type");
	      return cssProg::YY_Err; }
	    cssClassType* cls = (cssClassType*)(yyvsp[-1].el).El();
	    cssMisc::cur_class = cls; /* this is now the current class */
	    cssEl* nm = cssMisc::cur_top->Prog()->Stack()->Pop();  /* get rid of name */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMemberFun((const char*)*nm);
	    if(fun == &cssMisc::Void) {
	      yyerror("member function not declared in class type");
	      return cssProg::YY_Err; }
            if(fun->GetType() == cssEl::T_MbrCFun) {
              yyerror("member function is hardcoded");
              return cssProg::YY_Err; }
	    if((fun->retv_type != (yyvsp[-2].el).El())
	       && !(fun->retv_type->IsRef() || fun->retv_type->GetType() == cssEl::T_Ptr))
	      cssMisc::Warning(NULL, "return type does not match that of declaration");
	    fun->SetRetvType((yyvsp[-2].el).El());
	    (yyval.el).SetDirect(fun);  cssEl::Done(nm); }
    break;

  case 118:
#line 753 "css_parse.y"
    {
            cssMisc::cur_scope = NULL;
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-1].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if(((yyvsp[-1].el).El())->GetType() != cssEl::T_ClassType) {
	      yyerror("attempt to define constructor member function in non-class type");
	      return cssProg::YY_Err; }
	    cssClassType* cls = (cssClassType*)(yyvsp[-1].el).El();
	    cssMisc::cur_class = cls; /* this is now the current class */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMemberFun((const char*)cls->name);
	    if(fun == &cssMisc::Void) {
	      yyerror("constructor member function not declared in class type");
	      return cssProg::YY_Err; }
            if(fun->GetType() == cssEl::T_MbrCFun) {
              yyerror("member function is hardcoded");
              return cssProg::YY_Err; }
	    (yyval.el).SetDirect(fun); }
    break;

  case 119:
#line 774 "css_parse.y"
    {
            cssMisc::cur_scope = NULL;
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-2].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if(((yyvsp[-2].el).El())->GetType() != cssEl::T_ClassType) {
	      yyerror("attempt to define destructor member function in non-class type");
	      return cssProg::YY_Err; }
	    cssClassType* cls = (cssClassType*)(yyvsp[-2].el).El();
	    cssMisc::cur_class = cls; /* this is now the current class */
	    String nm = "~"; nm += cls->name;
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMemberFun((const char*)nm);
	    if(fun == &cssMisc::Void) {
	      yyerror("destructor member function not declared in class type");
              return cssProg::YY_Err; }
            if(fun->GetType() == cssEl::T_MbrCFun) {
              yyerror("member function is hardcoded");
	      return cssProg::YY_Err; }
	    (yyval.el).SetDirect(fun); }
    break;

  case 120:
#line 796 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 121:
#line 797 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 122:
#line 798 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 123:
#line 799 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 124:
#line 802 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 125:
#line 803 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival) + 1; }
    break;

  case 126:
#line 806 "css_parse.y"
    { cssMisc::parsing_args = false; }
    break;

  case 127:
#line 807 "css_parse.y"
    {
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push((yyvsp[0].el).El()); }
    break;

  case 128:
#line 813 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-1].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[0].nm)));
	    ((yyvsp[-1].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 129:
#line 821 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-1].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    ((yyvsp[-1].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 131:
#line 832 "css_parse.y"
    {
	    if(((yyvsp[-2].ival) < 0) && ((yyvsp[0].ival) < 0)) (yyval.ival) = -1;
	    else if(((yyvsp[-2].ival) >= 0) && ((yyvsp[0].ival) < 0)) (yyval.ival) = (yyvsp[-2].ival);
	    else if(((yyvsp[-2].ival) < 0) && ((yyvsp[0].ival) >= 0)) (yyval.ival) = (yyvsp[0].ival);
	    else (yyval.ival) = -1; }
    break;

  case 132:
#line 839 "css_parse.y"
    {
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      (yyval.ival) = Code2(tmp->ptr, cssBI::constr); }
	    else (yyval.ival) = -1;
	    cssEl::Done(tmp); }
    break;

  case 133:
#line 848 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = ""; }
    break;

  case 134:
#line 849 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "extern"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 135:
#line 850 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "static"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 136:
#line 851 "css_parse.y"
    { /* use const expr prog to code stuff */
	  (yyvsp[0].el).El()->tmp_str = "const"; (yyval.el) = (yyvsp[0].el); cssMisc::CodeConstExpr(); }
    break;

  case 137:
#line 853 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 138:
#line 854 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = ""; (yyval.el) = (yyvsp[0].el); }
    break;

  case 139:
#line 855 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 140:
#line 856 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 141:
#line 859 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); cssMisc::cur_type = (yyval.el); }
    break;

  case 142:
#line 860 "css_parse.y"
    {
	  if(((yyvsp[-1].el).El()->GetParse() != CSS_PTR) || ((yyvsp[0].ival) >= 2)) {
	    (yyval.el) = cssMisc::cur_top->AddPtrType((yyvsp[-1].el).El()); cssMisc::cur_type = (yyval.el);
	  }
	  else {
	    (yyval.el) = (yyvsp[-1].el); cssMisc::cur_type = (yyval.el); } }
    break;

  case 143:
#line 866 "css_parse.y"
    {
	  (yyval.el) = cssMisc::cur_top->AddRefType((yyvsp[-1].el).El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 144:
#line 868 "css_parse.y"
    {
	  if(((yyvsp[-2].el).El()->GetParse() != CSS_PTR) || ((yyvsp[-1].ival) >= 2)) {
            cssElPtr npt = cssMisc::cur_top->AddPtrType((yyvsp[-2].el).El());
            (yyval.el) = cssMisc::cur_top->AddRefType(npt.El()); cssMisc::cur_type = (yyval.el); }
	  else {
	    (yyval.el) = cssMisc::cur_top->AddRefType((yyvsp[-2].el).El()); cssMisc::cur_type = (yyval.el); } }
    break;

  case 147:
#line 878 "css_parse.y"
    {
          cssEl* itm;
          if((yyvsp[-1].el).El()->GetParse() != CSS_PTR)
	    itm = (yyvsp[-1].el).El()->GetScoped((const char*)(yyvsp[0].el).El()->name);
	  else
	    itm = (yyvsp[0].el).El();
	  if(itm != &cssMisc::Void)  (yyval.el).SetDirect(itm);
	  else			(yyval.el).Reset();
	  cssMisc::cur_type = (yyval.el); cssMisc::cur_scope = NULL; }
    break;

  case 148:
#line 889 "css_parse.y"
    {
           if((yyvsp[-1].el).El()->GetType() != cssEl::T_ClassType) {
	     yyerror("scoping of non-class type");
	     return cssProg::YY_Err; }
           cssMisc::cur_scope = (yyvsp[-1].el).El(); }
    break;

  case 149:
#line 894 "css_parse.y"
    {
           cssMisc::cur_scope = (yyvsp[-1].el).El(); }
    break;

  case 152:
#line 902 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 153:
#line 903 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 154:
#line 906 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastif = -1; }
    break;

  case 156:
#line 910 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastdo = -1; }
    break;

  case 158:
#line 914 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 161:
#line 917 "css_parse.y"
    { Code1((yyvsp[-2].el)); (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 162:
#line 918 "css_parse.y"
    { Code1((yyvsp[-3].el)); (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 163:
#line 919 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[-1].el)); }
    break;

  case 164:
#line 920 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[-1].el)); }
    break;

  case 168:
#line 926 "css_parse.y"
    {
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; // next instr is 'it'
  	    if(cssMisc::cur_top->Prog()->name != cssSwitchBlock_Name) {
	      yyerror("case statement not in a switch block");
	      return cssProg::YY_Err;
	    }
	    cssElPtr aryptr = cssMisc::cur_top->Prog()->literals.FindName(cssSwitchJump_Name);
	    cssArray* val_ary = (cssArray*)aryptr.El();
	    val_ary->items->Push(new cssInt((yyval.ival), (const char*)*((yyvsp[-1].el).El()))); }
    break;

  case 169:
#line 935 "css_parse.y"
    {
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; // next instr is 'it'
  	    if(cssMisc::cur_top->Prog()->name != cssSwitchBlock_Name) {
	      yyerror("case statement not in a switch block");
	      return cssProg::YY_Err;
	    }
	    cssElPtr aryptr = cssMisc::cur_top->Prog()->literals.FindName(cssSwitchJump_Name);
	    cssArray* val_ary = (cssArray*)aryptr.El();
	    val_ary->items->Push(new cssInt((yyval.ival), cssSwitchDefault_Name)); }
    break;

  case 173:
#line 953 "css_parse.y"
    {
            (yyval.ival) = (yyvsp[-5].ival);
	    /* value to switch on */
	    cssMisc::ConstExpr->Stack()->Push(new cssRef(cssMisc::ConstExpr->Autos()->Push
							  (new cssString(0,cssSwitchVar_Name))));
	    /* bogus return value */
	    cssMisc::ConstExpr->Stack()->Push(new cssRef(cssMisc::ConstExpr->Autos()->Push
							  (new cssInt(0,cssRetv_Name))));
            cssScriptFun* tmp = (cssScriptFun*)(cssMisc::cur_top->Prog()->insts[(yyvsp[0].ival)]->inst.El());
	    tmp->Define(cssMisc::ConstExpr, false, cssSwitchBlock_Name);
	    tmp->is_block = true; /* this is actually a block.. */
	    /* make the jump-table address array: ints whose name is val, val is adr */
	    tmp->fun->Stack()->Push(new cssString(cssSwitchJump_Name));
	    tmp->fun->Stack()->Push(new cssInt(1)); /* type of ary element */
	    tmp->fun->Stack()->Push(new cssInt(1)); /* number of dimensions */
  	    cssMisc::VoidArray.tmp_str = "literal";
	    cssMisc::VoidArray.MakeToken(tmp->fun); /* make the array */
	    cssMisc::VoidArray.tmp_str = "";
	    cssRef* aryptr = (cssRef*) tmp->fun->Stack()->Pop();
	    ((cssArray*)aryptr->ptr.El())->items->DelPop(); /* get rid of first el */
	    tmp->fun->Code(cssBI::switch_jump);
	    cssEl::Done(aryptr); }
    break;

  case 174:
#line 978 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 175:
#line 982 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-8].ival) + 1]->SetLine((yyvsp[-5].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-8].ival) + 2]->SetLine((yyvsp[-3].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-8].ival) + 3]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-8].ival) + 4]->SetLine((yyvsp[0].ival)); }
    break;

  case 176:
#line 989 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 1]->SetLine((yyvsp[-6].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 2]->SetLine((yyvsp[-4].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 3]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 4]->SetLine((yyvsp[0].ival)); }
    break;

  case 177:
#line 996 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 1]->SetLine((yyvsp[-5].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 2]->SetLine((yyvsp[-4].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 3]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 4]->SetLine((yyvsp[0].ival)); }
    break;

  case 178:
#line 1003 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-10].ival) + 1]->SetLine((yyvsp[-6].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-10].ival) + 2]->SetLine((yyvsp[-4].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-10].ival) + 3]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-10].ival) + 4]->SetLine((yyvsp[0].ival)); }
    break;

  case 179:
#line 1010 "css_parse.y"
    {
            (yyval.ival) = Code2((yyvsp[0].el),cssInst::Stop);
	    Code3(cssInst::Stop, cssInst::Stop, cssInst::Stop); }
    break;

  case 180:
#line 1016 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastdo = (yyvsp[-1].ival); }
    break;

  case 181:
#line 1019 "css_parse.y"
    { (yyval.ival) = Code3((yyvsp[0].el), cssInst::Stop, cssInst::Stop);
                                  cssMisc::cur_top->Prog()->lastdo = (yyval.ival); }
    break;

  case 182:
#line 1024 "css_parse.y"
    {
	    if(cssMisc::cur_top->Prog()->lastdo >= 0) {
 	      cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 1]->SetLine((yyvsp[-2].ival));
	      cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 2]->SetLine((yyvsp[0].ival));
	    }
	    else {
	      cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 1]->SetLine((yyvsp[0].ival)-1); /* do nothing */
	      cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 2]->SetLine((yyvsp[0].ival));
	    }
	    cssMisc::cur_top->Prog()->lastdo = -1;	}
    break;

  case 183:
#line 1035 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 1]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 2]->SetLine((yyvsp[0].ival)); }
    break;

  case 184:
#line 1040 "css_parse.y"
    {
            if(cssMisc::cur_top->Prog()->lastdo < 0) {
	      (yyval.ival) = Code3((yyvsp[0].el), cssInst::Stop, cssInst::Stop); }
            else { (yyval.ival) = cssMisc::cur_top->Prog()->lastdo;} }
    break;

  case 185:
#line 1048 "css_parse.y"
    {
            if(cssMisc::cur_top->debug > 3)
	      cerr << "\nvalue of then is: " << (yyvsp[-1].ival) << "\n";
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 1]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival)]->EndIf((yyvsp[0].ival)); }
    break;

  case 186:
#line 1053 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 1]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)]->previf = (yyvsp[-3].ival);
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)]->EndIf(); }
    break;

  case 187:
#line 1057 "css_parse.y"
    {
	    css_progdx tmp = cssMisc::cur_top->Prog()->elseif;
	    if(tmp < 0) {
	      yyerror("else without matching if");
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_top->Prog()->elseif = -1;	/* reset the else */
	    cssMisc::cur_top->Prog()->insts[tmp + 2]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[tmp]->EndIf((yyvsp[0].ival)); (yyval.ival) = tmp; }
    break;

  case 188:
#line 1066 "css_parse.y"
    {
	    css_progdx tmp = cssMisc::cur_top->Prog()->elseif;
	    if(tmp < 0) {
	      yyerror("else without matching if");
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_top->Prog()->elseif = -1;	/* reset the else */
	    cssMisc::cur_top->Prog()->insts[tmp + 2]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)]->previf = tmp;
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)]->EndIf(); (yyval.ival) = tmp; }
    break;

  case 189:
#line 1078 "css_parse.y"
    {
            (yyval.ival) = Code1((yyvsp[0].el)); Code3(cssInst::Stop, cssInst::Stop, cssInst::Stop);
	    cssMisc::cur_top->Prog()->ResetLasts();
	    cssMisc::cur_top->Prog()->lastif = (yyval.ival); }
    break;

  case 190:
#line 1084 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->elseif = cssMisc::cur_top->Prog()->lastif;
	    cssMisc::cur_top->Prog()->ResetLasts(); }
    break;

  case 191:
#line 1089 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 192:
#line 1092 "css_parse.y"
    {
            cssScriptFun* b = new cssScriptFun(cssBlock_Name);
	    cssMisc::cur_top->AddStatic(b);
	    (yyval.ival) = Code1(b);  cssMisc::cur_top->Prog()->insts[ (yyval.ival) ]->SetDefn();
	    cssMisc::cur_top->SetPush(b->fun); }
    break;

  case 193:
#line 1099 "css_parse.y"
    {  }
    break;

  case 194:
#line 1102 "css_parse.y"
    {
            (yyval.ival) = Code1(cssInst::Stop); cssMisc::cur_top->SetPop();
	    cssScriptFun* sfun = cssMisc::cur_top->Prog()->owner;
	    if((sfun != NULL) && (sfun->GetType() == cssEl::T_MbrScriptFun)) {
	      cssMisc::cur_class = NULL; /* get rid of current class pointer.. */
	      cssMisc::cur_method = NULL; } /* and current method pointer */
	  }
    break;

  case 195:
#line 1111 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 198:
#line 1117 "css_parse.y"
    { Code1(cssBI::asgn); }
    break;

  case 199:
#line 1118 "css_parse.y"
    {
	    int c; String inp;
	    while(((c = cssMisc::cur_top->Prog()->Getc()) != '}') && (c != EOF)) inp += (char)c;
	    cssSStream* ss = new cssSStream();
	    cssMisc::cur_top->AddLiteral(ss);
 	    stringstream* sss = (stringstream*)ss->GetVoidPtr();
	    *sss << inp;
	    sss->seekg(0, ios::beg);
	    Code3(ss, cssBI::arg_swap, cssBI::rshift);
 	    Code1(cssBI::sstream_rewind); /* rewind stream for next use.. */ }
    break;

  case 200:
#line 1128 "css_parse.y"
    { Code1(cssBI::asgn_add); }
    break;

  case 201:
#line 1129 "css_parse.y"
    { Code1(cssBI::asgn_sub); }
    break;

  case 202:
#line 1130 "css_parse.y"
    { Code1(cssBI::asgn_mult); }
    break;

  case 203:
#line 1131 "css_parse.y"
    { Code1(cssBI::asgn_div); }
    break;

  case 204:
#line 1132 "css_parse.y"
    { Code1(cssBI::asgn_mod); }
    break;

  case 205:
#line 1133 "css_parse.y"
    { Code1(cssBI::asgn_lshift); }
    break;

  case 206:
#line 1134 "css_parse.y"
    { Code1(cssBI::asgn_rshift); }
    break;

  case 207:
#line 1135 "css_parse.y"
    { Code1(cssBI::asgn_and); }
    break;

  case 208:
#line 1136 "css_parse.y"
    { Code1(cssBI::asgn_xor); }
    break;

  case 209:
#line 1137 "css_parse.y"
    { Code1(cssBI::asgn_or); }
    break;

  case 210:
#line 1138 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code2((yyvsp[0].el),(yyvsp[-2].el)); }
    break;

  case 211:
#line 1139 "css_parse.y"
    { (yyval.ival) = (yyvsp[-3].ival); Code2((yyvsp[-2].el),(yyvsp[-4].el)); }
    break;

  case 212:
#line 1140 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[-2].ival); Code2((yyvsp[-1].el),(yyvsp[-3].el)); }
    break;

  case 213:
#line 1142 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code1((yyvsp[-2].el)); }
    break;

  case 214:
#line 1143 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code1((yyvsp[-4].el)); }
    break;

  case 215:
#line 1144 "css_parse.y"
    { Code1(cssBI::lor); }
    break;

  case 216:
#line 1145 "css_parse.y"
    { Code1(cssBI::land); }
    break;

  case 217:
#line 1146 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::addr_of); }
    break;

  case 218:
#line 1147 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::de_ptr); }
    break;

  case 219:
#line 1148 "css_parse.y"
    { Code1(cssBI::gt); }
    break;

  case 220:
#line 1149 "css_parse.y"
    { Code1(cssBI::ge); }
    break;

  case 221:
#line 1150 "css_parse.y"
    { Code1(cssBI::lt); }
    break;

  case 222:
#line 1151 "css_parse.y"
    { Code1(cssBI::le); }
    break;

  case 223:
#line 1152 "css_parse.y"
    { Code1(cssBI::eq); }
    break;

  case 224:
#line 1153 "css_parse.y"
    { Code1(cssBI::ne); }
    break;

  case 225:
#line 1154 "css_parse.y"
    { Code1(cssBI::cond); }
    break;

  case 226:
#line 1155 "css_parse.y"
    { Code1(cssBI::add); }
    break;

  case 227:
#line 1156 "css_parse.y"
    { Code1(cssBI::sub); }
    break;

  case 228:
#line 1157 "css_parse.y"
    { Code1(cssBI::mul); }
    break;

  case 229:
#line 1158 "css_parse.y"
    { Code1(cssBI::div); }
    break;

  case 230:
#line 1159 "css_parse.y"
    { Code1(cssBI::modulo); }
    break;

  case 231:
#line 1160 "css_parse.y"
    { Code1(cssBI::lshift); }
    break;

  case 232:
#line 1161 "css_parse.y"
    { Code1(cssBI::rshift); }
    break;

  case 233:
#line 1162 "css_parse.y"
    { Code1(cssBI::bit_and); }
    break;

  case 234:
#line 1163 "css_parse.y"
    { Code1(cssBI::bit_xor); }
    break;

  case 235:
#line 1164 "css_parse.y"
    { Code1(cssBI::bit_or); }
    break;

  case 236:
#line 1165 "css_parse.y"
    { Code1(cssBI::asgn_post_pp); }
    break;

  case 237:
#line 1166 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::asgn_pre_pp); }
    break;

  case 238:
#line 1167 "css_parse.y"
    { Code1(cssBI::asgn_post_mm); }
    break;

  case 239:
#line 1168 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::asgn_pre_mm); }
    break;

  case 240:
#line 1169 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::neg); }
    break;

  case 241:
#line 1170 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::lnot); }
    break;

  case 242:
#line 1171 "css_parse.y"
    { Code1(cssBI::de_array); }
    break;

  case 243:
#line 1172 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-2].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = (yyvsp[0].ival); Code2((yyvsp[-2].el), cssBI::cast); }
    break;

  case 244:
#line 1178 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
  	    (yyval.ival) = (yyvsp[-1].ival); Code2((yyvsp[-3].el), cssBI::cast); }
    break;

  case 246:
#line 1185 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 254:
#line 1201 "css_parse.y"
    { (yyval.ival) = Code2(cssBI::push_next, (yyvsp[0].el)); }
    break;

  case 255:
#line 1202 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].el_ival).ival; Code1((yyvsp[-1].el_ival).el); }
    break;

  case 256:
#line 1203 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].el_ival).ival; Code1((yyvsp[-2].el_ival).el);
	  if(((yyvsp[-2].el_ival).el.El()->GetType() == cssEl::T_ElCFun) ||
	     /*	     ($1.el.El()->GetType() == cssEl::T_MbrCFun) || */
	     ((yyvsp[-2].el_ival).el.El()->GetType() == cssEl::T_ScriptFun) ||
	     ((yyvsp[-2].el_ival).el.El()->GetType() == cssEl::T_MbrScriptFun)) {
	    cssElFun* fun = (cssElFun*)(yyvsp[-2].el_ival).el.El();
	    int max_args = fun->argc;  int min_args = (fun->argc - fun->arg_defs.size);
	    int act_args = (yyvsp[-1].ival);
	    if(fun->GetType() == cssEl::T_MbrScriptFun) { max_args--; min_args--; }
	    if((fun->argc >= 0) && (act_args > max_args)) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(), "Too many arguments for function:",fun->name,", should have at most:", String(max_args), "got:",String(act_args)); }
	    if((fun->argc >= 0) && (act_args < min_args)) {
              cssMisc::Warning(cssMisc::cur_top->Prog(), "Too few arguments for function:",fun->name,", should have at least:",String(min_args),"got:",String(act_args)); } } }
    break;

  case 257:
#line 1218 "css_parse.y"
    {
            (yyval.el_ival).ival = Code1(cssMisc::VoidElPtr); /* an arg stop */
	    (yyval.el_ival).el = (yyvsp[-1].el);	/* save this pointer for later.. */
            if((yyvsp[-1].el).El()->GetType() == cssEl::T_MbrScriptFun)
	      Code1(cssBI::push_cur_this); /* push this as first arg.. */
	    else if((yyvsp[-1].el).El()->GetType() == cssEl::T_MbrCFun) {
	      /* only push this for MbrCFun that are builtins (Load, Save, or InheritsFrom) on a script class! */
	      cssMbrCFun* mbrfun = (cssMbrCFun*)(yyvsp[-1].el).El();
	      if((mbrfun->funp == &cssClassType::InheritsFrom_stub) ||
		 (mbrfun->funp == &cssClassType::Load_stub) ||
		 (mbrfun->funp == &cssClassType::Save_stub))
		Code1(cssBI::push_cur_this); /* push this as first arg.. */
	    } }
    break;

  case 258:
#line 1234 "css_parse.y"
    { Code2((yyvsp[0].el), cssBI::points_at);}
    break;

  case 259:
#line 1235 "css_parse.y"
    {
	    int mbno = (yyvsp[-2].el).El()->GetMemberNo((const char*)*((yyvsp[0].el).El()));
	    if(mbno < 0) { (yyval.ival) = Code3((yyvsp[-2].el), (yyvsp[0].el), cssBI::points_at); }
	    else { cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
		   (yyval.ival) = Code3((yyvsp[-2].el), tmpint, cssBI::points_at); } }
    break;

  case 260:
#line 1243 "css_parse.y"
    { (yyval.ival) = Code3(cssBI::push_root, (yyvsp[0].el), cssBI::points_at); }
    break;

  case 261:
#line 1244 "css_parse.y"
    {
	  cssMisc::cur_scope = NULL;
	    cssEl* scp = (yyvsp[-1].el).El()->GetScoped((const char*)*((yyvsp[0].el).El()));
	    if(scp != &cssMisc::Void) {  (yyval.ival) = Code1(scp); }
	    else { (yyval.ival) = Code3((yyvsp[-1].el), (yyvsp[0].el), cssBI::scoper); } }
    break;

  case 262:
#line 1249 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].el_ival).ival; }
    break;

  case 263:
#line 1250 "css_parse.y"
    { (yyval.ival) = (yyvsp[-3].el_ival).ival;
	  if(((yyvsp[-3].el_ival).el.El()->GetType() == cssEl::T_ElCFun) ||
	     ((yyvsp[-3].el_ival).el.El()->GetType() == cssEl::T_MbrCFun) ||
	     ((yyvsp[-3].el_ival).el.El()->GetType() == cssEl::T_ScriptFun) ||
	     ((yyvsp[-3].el_ival).el.El()->GetType() == cssEl::T_MbrScriptFun)) {
	    cssElFun* fun = (cssElFun*)(yyvsp[-3].el_ival).el.El();
	    int max_args = fun->argc;  int min_args = (fun->argc - fun->arg_defs.size);
	    int act_args = (yyvsp[-2].ival);
	    if(fun->GetType() == cssEl::T_MbrScriptFun) { max_args--; min_args--; }
	    if((fun->argc >= 0) && (act_args > max_args)) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(), "Too many arguments for function:",fun->name,", should have at most:", String(max_args), "got:",String(act_args)); }
	    if((fun->argc >= 0) && (act_args < min_args)) {
              cssMisc::Warning(cssMisc::cur_top->Prog(), "Too few arguments for function:",fun->name,", should have at least:",String(min_args),"got:",String(act_args)); } } }
    break;

  case 264:
#line 1265 "css_parse.y"
    { Code2((yyvsp[-1].el), cssBI::member_fun);
                                                  (yyval.el_ival).ival = (yyvsp[-3].ival); (yyval.el_ival).el.Reset(); }
    break;

  case 265:
#line 1267 "css_parse.y"
    { (yyval.el_ival).el.Reset();
	    int mbno = (yyvsp[-3].el).El()->GetMemberFunNo((const char*)*((yyvsp[-1].el).El()));
	    if(mbno < 0) { /* don't complain for pointers and references */
	      if(!(yyvsp[-3].el).El()->IsRef() && ((yyvsp[-3].el).El()->GetType() != cssEl::T_Ptr))
		cssMisc::Warning(NULL, "Member Function:",(const char*)*((yyvsp[-1].el).El()),
				 "not found in parent object, will be resolved dynamically");
	      (yyval.el_ival).ival = Code3((yyvsp[-3].el), (yyvsp[-1].el), cssBI::member_fun); }
	    else {
	      cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
	      (yyval.el_ival).ival = Code3((yyvsp[-3].el), tmpint, cssBI::member_fun);
	      cssEl* ths = (yyvsp[-3].el).El();
	      if(ths->GetType() == cssEl::T_Ptr)
		ths = ((cssPtr*)ths)->el_type;
	      if((ths != NULL) && ((ths->GetType() == cssEl::T_Class) || (ths->GetType() == cssEl::T_ClassType))) {
		cssEl* fun = ths->GetMemberFun((const char*)*((yyvsp[-1].el).El()));
		if(fun->GetType() != cssEl::T_MbrCFun)
		  (yyval.el_ival).el.SetDirect(fun);
	      } } }
    break;

  case 266:
#line 1285 "css_parse.y"
    {
	    cssMisc::cur_scope = NULL; (yyval.el_ival).el.Reset();
	    int mbno = (yyvsp[-2].el).El()->GetMemberFunNo((const char*)*((yyvsp[-1].el).El()));
	    if(mbno < 0) { /* don't complain for pointers and references */
	      if(!(yyvsp[-2].el).El()->IsRef() && ((yyvsp[-2].el).El()->GetType() != cssEl::T_Ptr))
		cssMisc::Warning(NULL, "Member Function:",(const char*)*((yyvsp[-1].el).El()),
				 "not found in parent object, will be resolved dynamically");
	      (yyval.el_ival).ival = Code3((yyvsp[-2].el), (yyvsp[-1].el), cssBI::member_fun); }
	    else {
	      cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
	      (yyval.el_ival).ival = Code3((yyvsp[-2].el), tmpint, cssBI::member_fun);
	      cssEl* ths = (yyvsp[-2].el).El();
	      if(ths->GetType() == cssEl::T_Ptr)
		ths = ((cssPtr*)ths)->el_type;
	      if((ths != NULL) && ((ths->GetType() == cssEl::T_Class) || (ths->GetType() == cssEl::T_ClassType))) {
		cssEl* fun = ths->GetMemberFun((const char*)*((yyvsp[-1].el).El()));
		if(fun->GetType() != cssEl::T_MbrCFun)
		  (yyval.el_ival).el.SetDirect(fun);
	      } } }
    break;

  case 267:
#line 1309 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].nm))); }
    break;

  case 268:
#line 1313 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].nm))); }
    break;

  case 269:
#line 1315 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName())); }
    break;

  case 270:
#line 1317 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName())); }
    break;

  case 271:
#line 1319 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 272:
#line 1323 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 273:
#line 1327 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding function:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 276:
#line 1337 "css_parse.y"
    {
   	    String tmpstr = String((yyvsp[0].nm));
            (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr); }
    break;

  case 278:
#line 1341 "css_parse.y"
    { /* this is source of shift-reduce problems */
	    String tmpstr = String((yyvsp[0].el).El()->GetName());
	    (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr); }
    break;

  case 284:
#line 1353 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 285:
#line 1354 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival) + 1; }
    break;

  case 287:
#line 1358 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 288:
#line 1361 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); }
    break;

  case 291:
#line 1368 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 292:
#line 1371 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 293:
#line 1374 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;


    }

/* Line 1037 of yacc.c.  */
#line 3714 "y.tab.c"

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
      /* If just tried and failed to reuse look-ahead token after an
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
		 yydestruct ("Error: popping",
                             yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
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


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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
  yydestruct ("Error: discarding lookahead",
              yytoken, &yylval);
  yychar = YYEMPTY;
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


#line 1378 "css_parse.y"


	/* end of grammar */


void yyerror(char* s) { 	/* called for yacc syntax error */
  int i;

  ostream* fh = &cerr;
  if(cssMisc::cur_top->cmd_shell != NULL)
    fh = cssMisc::cur_top->cmd_shell->ferr;

  if(strcmp(s, "parse error") == 0) {
    String src = cssMisc::cur_top->Prog()->GetSrcLC(cssMisc::cur_top->Prog()->tok_line);
    src.gsub('\t',' ');		// replace tabs
    *(fh) << "Syntax Error, line " << cssMisc::cur_top->src_ln << ":\t"
      << src;
    *(fh) << "\t\t\t";
    for(i=0; i < cssMisc::cur_top->Prog()->tok_col; i++)
      *(fh) << " ";
    *(fh) << "^\n";
  }
  else {
    *(fh) << s << " line " << cssMisc::cur_top->src_ln << ":\t"
      << cssMisc::cur_top->Prog()->GetSrcLC(cssMisc::cur_top->Prog()->tok_line);
  }
  fh->flush();
}

