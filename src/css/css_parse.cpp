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
#define Code5(w,x,y,z,zz) 	cssMisc::cur_top->Prog()->Code(w); cssMisc::cur_top->Prog()->Code(x); cssMisc::cur_top->Prog()->Code(y); cssMisc::cur_top->Prog()->Code(z); cssMisc::cur_top->Prog()->Code(zz);



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
#line 59 "css_parse.y"
typedef struct YYSTYPE {
  cssElPlusIVal el_ival;
  cssElPtr     	el;		/* for coding */
  int		ival;		/* for program indexes (progdx) and other ints */
  char*        	nm;
} YYSTYPE;
/* Line 190 of yacc.c.  */
#line 271 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 213 of yacc.c.  */
#line 283 "y.tab.c"

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
#define YYLAST   1705

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  98
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  102
/* YYNRULES -- Number of rules. */
#define YYNRULES  300
/* YYNRULES -- Number of states. */
#define YYNSTATES  504

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
     115,   119,   123,   124,   127,   132,   134,   137,   139,   141,
     143,   145,   150,   157,   164,   171,   173,   176,   180,   187,
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
     534,   536,   538,   540,   547,   549,   556,   564,   566,   568,
     571,   573,   575,   578,   580,   582,   585,   587,   591,   595,
     597,   601,   605,   608,   611,   613,   615,   619,   621,   623,
     625,   627,   629,   631,   633,   635,   639,   643,   647,   651,
     655,   659,   663,   667,   671,   675,   679,   683,   687,   693,
     698,   702,   708,   712,   716,   719,   722,   726,   730,   734,
     738,   742,   746,   752,   756,   760,   764,   768,   772,   776,
     780,   784,   788,   792,   795,   798,   801,   804,   807,   810,
     815,   820,   825,   827,   831,   833,   835,   837,   839,   841,
     843,   845,   847,   850,   854,   857,   861,   865,   868,   871,
     875,   880,   885,   890,   894,   896,   898,   900,   902,   904,
     906,   908,   910,   912,   914,   916,   918,   920,   922,   924,
     926,   928,   930,   934,   936,   938,   939,   941,   943,   947,
     949
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
      99,     0,    -1,    -1,    99,    92,    -1,    99,   102,   101,
      -1,    99,   108,   101,    -1,    99,   180,    -1,    99,   110,
      -1,    99,   154,    -1,    99,     1,   101,    -1,    93,    -1,
      93,    -1,    92,    -1,   103,    -1,   104,    -1,   105,    -1,
     106,    -1,   107,    -1,     3,    11,    -1,     4,    16,    -1,
       4,    15,    -1,     5,    15,    -1,     6,    16,    -1,     6,
      15,    -1,     7,    15,    -1,     7,    16,    -1,     8,    -1,
       9,    -1,    36,   109,    -1,    39,   109,    -1,    39,   196,
      18,    -1,    38,   109,    -1,    38,   196,    18,    -1,    37,
     184,   190,    -1,    37,   184,   184,    -1,    40,    -1,    40,
     196,   184,    -1,    40,   196,    13,    -1,    40,   196,    37,
      -1,    40,   196,    40,    -1,    40,   196,    19,    -1,    40,
     196,    18,    -1,    -1,   196,   194,    -1,    88,   196,   194,
      94,    -1,   111,    -1,   112,   100,    -1,   113,    -1,   116,
      -1,   121,    -1,   139,    -1,   148,   146,   199,   100,    -1,
     148,   190,    51,   196,   181,   199,    -1,   189,    51,   196,
     181,   199,   100,    -1,   148,   190,   196,   114,   199,   100,
      -1,   115,    -1,   114,   115,    -1,    89,   181,    95,    -1,
     117,    49,   118,    96,   199,   100,    -1,    21,   190,    -1,
      21,    18,    -1,    21,    -1,   119,    -1,   118,   119,    -1,
     120,    -1,   120,    50,    -1,     1,    -1,   190,    -1,   190,
      51,    10,    -1,    20,   122,    49,   138,   130,    96,   199,
     100,    -1,    20,   122,    49,   138,    96,   199,   100,    -1,
      20,   124,   199,   100,    -1,   123,    -1,   123,    63,   125,
      -1,   190,    -1,    18,    -1,   190,    -1,    18,    -1,   126,
      -1,   125,    50,   126,    -1,   127,    -1,   149,    -1,   128,
     149,    -1,   129,    -1,   128,   129,    -1,    45,    -1,    47,
      -1,    44,    -1,   131,    -1,   130,   131,    -1,   132,    -1,
     136,    -1,   116,   138,    -1,    45,    63,    -1,    44,    63,
      -1,    46,    63,    -1,     1,    -1,   148,   190,   100,   138,
      -1,   148,   190,   133,   134,   100,   138,    -1,    -1,   135,
      -1,   134,   135,    -1,    89,    10,    95,    -1,   137,   142,
     100,   138,    -1,    18,   142,   100,   138,    -1,    97,    18,
     142,   100,   138,    -1,    47,    97,    18,   142,   100,   138,
      -1,   148,   190,    -1,    -1,    17,    -1,   138,    17,    -1,
     140,   142,   199,   100,    -1,   140,   142,   199,   178,    -1,
     148,    13,   142,   100,    -1,   148,    13,   142,   199,   178,
      -1,   141,   142,   199,   179,    -1,   148,   190,    -1,   148,
     151,   190,    -1,   151,    18,    -1,   151,    97,    18,    -1,
      88,    94,    -1,    88,   143,    94,    -1,    88,    94,    43,
      -1,    88,   143,    94,    43,    -1,   144,    -1,   143,    50,
     144,    -1,   145,    -1,   145,    51,   183,    -1,   148,    16,
      -1,   148,    12,    -1,   147,    -1,   146,    50,   147,    -1,
     190,    -1,   149,    -1,    41,   149,    -1,    42,   149,    -1,
      43,   149,    -1,    47,   149,    -1,    48,   149,    -1,    48,
      47,   149,    -1,    47,    48,   149,    -1,   150,    -1,   150,
     153,    -1,   150,    68,    -1,   150,   153,    68,    -1,    18,
      -1,    19,    -1,   151,    22,    -1,    18,    90,    -1,    19,
      90,    -1,    18,    -1,    19,    -1,    79,    -1,   153,    79,
      -1,   155,    -1,   172,    -1,   156,    -1,   168,    -1,   181,
     100,    -1,   161,    -1,   170,    -1,    30,   196,   100,    -1,
      30,   196,   181,   100,    -1,    31,   100,    -1,    32,   100,
      -1,   159,    -1,   157,    -1,   178,    -1,    28,   158,    63,
      -1,    29,    63,    -1,    10,    -1,    11,    -1,    12,    -1,
     160,    88,   196,   181,    94,    49,    -1,    27,    -1,   162,
      88,   163,   165,   167,   154,    -1,   162,    88,   197,   163,
     165,   167,   154,    -1,    33,    -1,   164,    -1,   164,   181,
      -1,    93,    -1,   166,    -1,   166,   197,    -1,    93,    -1,
      94,    -1,   169,   154,    -1,    24,    -1,   171,   175,   100,
      -1,   171,   175,   154,    -1,    23,    -1,   173,   175,   155,
      -1,   173,   175,   172,    -1,   174,   155,    -1,   174,   172,
      -1,    25,    -1,    26,    -1,   176,   181,   177,    -1,    88,
      -1,    94,    -1,    49,    -1,    49,    -1,    96,    -1,   183,
      -1,   182,    -1,   187,    -1,   181,    51,   181,    -1,   181,
      51,    49,    -1,   181,    61,   181,    -1,   181,    60,   181,
      -1,   181,    59,   181,    -1,   181,    58,   181,    -1,   181,
      57,   181,    -1,   181,    56,   181,    -1,   181,    55,   181,
      -1,   181,    54,   181,    -1,   181,    53,   181,    -1,   181,
      52,   181,    -1,    34,   196,   152,    -1,    34,   196,   152,
      88,    94,    -1,    34,   196,   152,   115,    -1,    35,   196,
     181,    -1,    35,    89,    95,   196,   181,    -1,   181,    64,
     181,    -1,   181,    65,   181,    -1,    68,   181,    -1,    79,
     181,    -1,   181,    74,   181,    -1,   181,    73,   181,    -1,
     181,    72,   181,    -1,   181,    71,   181,    -1,   181,    70,
     181,    -1,   181,    69,   181,    -1,   181,    62,   181,    63,
     181,    -1,   181,    77,   181,    -1,   181,    78,   181,    -1,
     181,    79,   181,    -1,   181,    80,   181,    -1,   181,    81,
     181,    -1,   181,    76,   181,    -1,   181,    75,   181,    -1,
     181,    68,   181,    -1,   181,    67,   181,    -1,   181,    66,
     181,    -1,   181,    83,    -1,    83,   181,    -1,   181,    82,
      -1,    82,   181,    -1,    78,   181,    -1,    85,   181,    -1,
     181,    89,   181,    95,    -1,    88,   148,    94,   181,    -1,
     148,    88,   181,    94,    -1,   185,    -1,    88,   181,    94,
      -1,    10,    -1,    11,    -1,    12,    -1,    14,    -1,    36,
      -1,    38,    -1,    39,    -1,    13,    -1,   186,    94,    -1,
     186,   194,    94,    -1,    13,    88,    -1,   182,   191,   192,
      -1,   183,   191,   192,    -1,    87,   192,    -1,   151,   192,
      -1,   188,   199,    94,    -1,   188,   199,   194,    94,    -1,
     182,   191,   192,    88,    -1,   183,   191,   192,    88,    -1,
     151,   192,    88,    -1,    16,    -1,    16,    -1,    10,    -1,
      11,    -1,    12,    -1,    14,    -1,    13,    -1,    87,    -1,
      91,    -1,    16,    -1,    11,    -1,   193,    -1,    12,    -1,
      14,    -1,    13,    -1,    19,    -1,   184,    -1,   195,    -1,
     194,    50,   195,    -1,   181,    -1,    19,    -1,    -1,   198,
      -1,   112,    -1,   197,    50,   197,    -1,   181,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   163,   163,   164,   165,   166,   167,   170,   173,   174,
     177,   180,   181,   184,   185,   186,   187,   188,   193,   199,
     201,   202,   205,   208,   209,   212,   215,   219,   230,   232,
     234,   236,   238,   240,   244,   246,   248,   252,   256,   260,
     264,   268,   274,   275,   276,   280,   281,   282,   283,   284,
     285,   288,   298,   316,   336,   352,   353,   357,   360,   367,
     374,   381,   389,   390,   393,   394,   395,   398,   415,   435,
     439,   443,   449,   450,   453,   460,   482,   488,   496,   497,
     500,   510,   511,   514,   515,   518,   519,   520,   523,   524,
     527,   528,   529,   530,   531,   532,   533,   536,   548,   564,
     572,   573,   577,   581,   587,   603,   620,   640,   661,   662,
     663,   666,   672,   684,   692,   710,   723,   736,   762,   783,
     805,   806,   807,   808,   811,   812,   815,   816,   822,   830,
     840,   841,   848,   857,   858,   859,   860,   862,   863,   864,
     865,   868,   869,   875,   877,   885,   886,   887,   898,   903,
     907,   908,   911,   912,   915,   916,   919,   920,   923,   924,
     925,   926,   927,   928,   929,   930,   931,   932,   935,   944,
     956,   957,   958,   962,   984,   993,  1008,  1031,  1039,  1040,
    1043,  1051,  1052,  1055,  1066,  1077,  1080,  1090,  1113,  1127,
    1130,  1133,  1136,  1143,  1147,  1151,  1166,  1174,  1182,  1186,
    1193,  1196,  1205,  1206,  1210,  1211,  1212,  1222,  1223,  1224,
    1225,  1226,  1227,  1228,  1229,  1230,  1231,  1232,  1233,  1234,
    1236,  1237,  1238,  1239,  1240,  1241,  1242,  1243,  1244,  1245,
    1246,  1247,  1248,  1249,  1250,  1251,  1252,  1253,  1254,  1255,
    1256,  1257,  1258,  1259,  1260,  1261,  1262,  1263,  1264,  1265,
    1266,  1272,  1278,  1279,  1283,  1284,  1285,  1286,  1289,  1290,
    1291,  1295,  1296,  1297,  1312,  1328,  1329,  1337,  1338,  1344,
    1349,  1366,  1368,  1386,  1410,  1414,  1416,  1418,  1420,  1424,
    1428,  1434,  1435,  1438,  1441,  1442,  1447,  1448,  1449,  1450,
    1451,  1454,  1455,  1458,  1459,  1462,  1465,  1466,  1467,  1470,
    1473
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
  "for", "for_cond", "for_cond_sc", "for_incr", "for_incr_sc",
  "for_end_paren", "doloop", "do", "whiloop", "while", "ifstmt", "if",
  "else", "cond", "cond_paren", "cond_end_paren", "bra", "mbr_bra", "ket",
  "expr", "comb_expr", "primitive", "anycmd", "normfuncall", "normfun",
  "memb_expr", "membfun", "undefname", "name", "getmemb", "membname",
  "membnms", "exprlist", "exprlsel", "argstop", "stmtlist", "stmtel",
  "end", 0
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
     158,   158,   158,   159,   160,   161,   161,   162,   163,   163,
     164,   165,   165,   166,   167,   168,   169,   170,   170,   171,
     172,   172,   172,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   181,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   183,   183,   183,   183,   184,   184,
     184,   185,   185,   185,   186,   187,   187,   187,   187,   187,
     187,   188,   188,   188,   189,   190,   190,   190,   190,   190,
     190,   191,   191,   192,   192,   192,   193,   193,   193,   193,
     193,   194,   194,   195,   195,   196,   197,   197,   197,   198,
     199
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     3,     3,     2,     2,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     2,     2,
       3,     2,     3,     3,     3,     1,     3,     3,     3,     3,
       3,     3,     0,     2,     4,     1,     2,     1,     1,     1,
       1,     4,     6,     6,     6,     1,     2,     3,     6,     2,
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
       1,     1,     1,     6,     1,     6,     7,     1,     1,     2,
       1,     1,     2,     1,     1,     2,     1,     3,     3,     1,
       3,     3,     2,     2,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     5,     4,
       3,     5,     3,     3,     2,     2,     3,     3,     3,     3,
       3,     3,     5,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     2,     2,     2,     2,     4,
       4,     4,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     3,     2,     3,     3,     2,     2,     3,
       4,     4,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     0,     1,     1,     3,     1,
       0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,    26,
      27,   254,   255,   256,   261,   257,   274,   145,   146,     0,
      61,   189,   186,   194,   195,   174,     0,     0,   295,     0,
       0,   177,   295,   295,   295,     0,   295,   295,   295,     0,
       0,     0,     0,     0,   199,     0,     0,     0,     0,     0,
       0,     0,     0,     3,   201,     0,    13,    14,    15,    16,
      17,     0,     7,    45,     0,    47,    48,     0,    49,    50,
       0,     0,     0,   133,   141,     0,     8,   154,   156,   166,
     165,     0,   159,     0,   157,     0,   160,     0,   155,     0,
       0,   167,     6,     0,   203,   202,   252,     0,   204,   300,
       0,    12,    11,     9,    18,    20,    19,    21,    23,    22,
      24,    25,   264,   148,   149,   276,   277,   278,   280,   279,
     275,    75,     0,    72,   300,    74,    60,    59,   170,   171,
     172,     0,   169,     0,    10,   163,   164,     0,     0,     0,
     295,    28,     0,   258,   259,   260,     0,    31,     0,    29,
       0,     0,   134,     0,   135,   136,     0,   137,     0,   138,
       0,     0,   224,   247,   225,   246,   244,   248,   284,   286,
     288,   287,   283,   289,   290,   267,   285,     0,     0,     4,
       5,    46,     0,     0,   300,   300,   280,     0,     0,     0,
     300,   130,     0,   132,   143,   152,   142,   118,   147,     0,
     268,   295,     0,   185,   197,     0,     0,     0,   192,   193,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     245,   243,     0,   158,   281,   282,     0,     0,   294,   262,
     293,     0,   291,     0,   295,   108,     0,     0,   168,   161,
       0,   150,   151,   217,   295,   220,     0,    43,    34,    33,
     145,   145,    37,    41,    40,    38,    39,    36,   140,   139,
       0,   253,    66,     0,    62,    64,    67,   120,     0,   124,
     126,     0,     0,     0,   300,     0,     0,     0,   117,   295,
       0,   144,   153,   119,   273,     0,   180,   297,     0,     0,
     178,   299,     0,   296,   187,   188,     0,   190,   191,   206,
     205,   216,   215,   214,   213,   212,   211,   210,   209,   208,
     207,     0,   222,   223,   242,   241,   240,   231,   230,   229,
     228,   227,   226,   239,   238,   233,   234,   235,   236,   237,
       0,   265,   266,     0,   263,   269,     0,     0,   109,     0,
      87,    85,    86,    73,    78,    80,     0,    83,    81,    71,
     162,     0,     0,   219,     0,     0,   250,   300,    63,    65,
       0,   122,     0,   121,     0,   129,   128,   111,   112,   200,
     115,   113,     0,   251,   131,   132,    51,     0,   300,    55,
       0,     0,   183,     0,   181,   179,     0,     0,   198,   196,
       0,   249,   271,   272,   292,   270,   300,    96,   110,   145,
       0,     0,     0,     0,   300,     0,   108,     0,    88,    90,
      91,     0,     0,     0,    84,    82,   218,     0,   221,    44,
       0,    68,   125,   123,   127,   114,   300,    56,     0,     0,
     184,     0,   182,   298,     0,   232,     0,     0,    94,    93,
      95,     0,     0,     0,    92,   300,    89,     0,    99,    79,
      57,    58,    52,    54,   173,   175,     0,    53,   108,     0,
      70,     0,     0,   108,   108,     0,   176,   104,     0,   108,
      69,   103,    97,     0,     0,   100,   108,   105,     0,   108,
     101,   106,   102,    98
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,   135,   103,    55,    56,    57,    58,    59,    60,
      61,   141,    62,    63,   307,    65,   398,   373,   426,    67,
     283,   284,   285,    68,   122,   123,   124,   363,   364,   365,
     366,   367,   427,   428,   429,   485,   494,   495,   430,   431,
     359,    69,    70,    71,   184,   288,   289,   290,   190,   191,
     160,    73,    74,   161,   263,   196,    76,    77,    78,    79,
     131,    80,    81,    82,    83,   309,   310,   403,   404,   451,
      84,    85,    86,    87,    88,    89,    90,   205,   206,   409,
      91,   390,    92,    93,    94,    95,   174,    96,    97,    98,
      99,   100,   286,   246,   200,   176,   267,   252,   133,   312,
     313,   253
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -287
static const short int yypact[] =
{
    -287,   497,  -287,   113,    11,   234,    37,   240,   250,  -287,
    -287,  -287,  -287,  -287,    41,  -287,  -287,    15,    57,   478,
     643,  -287,  -287,  -287,  -287,  -287,   263,    77,  -287,    63,
      63,  -287,  -287,    72,   106,    50,   106,   106,   190,   266,
     266,   266,    65,    81,  -287,  1026,  1026,  1026,  1026,  1026,
    1026,   734,  1026,  -287,  -287,   113,  -287,  -287,  -287,  -287,
    -287,   113,  -287,  -287,    63,  -287,  -287,   118,  -287,  -287,
      86,    86,   152,  -287,    40,    17,  -287,  -287,  -287,  -287,
    -287,    93,  -287,   101,  -287,   867,  -287,   104,  -287,   104,
     867,  -287,  -287,  1472,    62,    62,  -287,   584,  -287,  -287,
     144,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,   107,   161,   158,  -287,   136,  -287,  -287,  -287,  -287,
    -287,   182,  -287,   783,  -287,  -287,  -287,   269,   164,  1026,
    -287,  -287,  1088,  -287,  -287,  -287,   222,  -287,  1106,  -287,
    1168,   430,  -287,   231,  -287,  -287,   266,  -287,   266,  -287,
     175,   443,    69,   203,    69,    69,    69,   203,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,  -287,  -287,   -56,  1296,  -287,
    -287,  -287,   376,   160,  -287,  -287,    86,    15,    57,  1026,
     244,  -287,   663,    66,  -287,  -287,    43,  -287,  -287,   261,
     210,  -287,   765,  -287,  -287,   681,  1026,   867,  -287,  -287,
     929,  1026,  1026,  1026,  1026,  1026,  1026,  1026,  1026,  1026,
    1026,  1026,  1026,  1026,  1026,  1026,  1026,  1026,  1026,  1026,
    1026,  1026,  1026,  1026,  1026,  1026,  1026,  1026,  1026,  1026,
    -287,  -287,  1026,  -287,  -287,  -287,   734,   734,    19,  -287,
    1554,   -33,  -287,   602,  -287,   312,   326,    63,  -287,  -287,
    1472,  -287,  -287,   201,  -287,    69,  1088,   280,  -287,  -287,
     119,   133,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    1026,  -287,  -287,    64,  -287,   283,   285,   296,     8,  -287,
     289,   171,   -12,   292,    63,  1340,   663,    63,  -287,  -287,
     257,  -287,  -287,  -287,  -287,  1026,  -287,  -287,    35,   254,
    1026,  1554,   -24,  -287,  -287,  -287,  1384,  -287,  -287,  -287,
    1554,  1554,  1554,  1554,  1554,  1554,  1554,  1554,  1554,  1554,
    1554,  1515,  1152,   481,   912,   849,  1601,  1616,  1616,  1616,
    1616,  1616,  1616,   394,   394,   528,   528,    69,    69,    69,
    1206,   264,   270,  1088,  -287,  -287,    10,  1026,  -287,    49,
    -287,  -287,  -287,   298,  -287,  -287,   326,  -287,  -287,  -287,
    -287,   255,  1026,  -287,  1026,    12,    69,  -287,  -287,  -287,
     341,  -287,   229,   314,   232,  -287,  -287,  -287,  -287,  -287,
    -287,  -287,   310,  -287,  -287,  -287,  -287,  1026,   257,  -287,
    1428,   311,  -287,   271,  1008,  1554,  1008,   254,  -287,  -287,
    1026,  -287,  -287,  -287,  -287,  -287,  1554,  -287,  -287,    22,
     300,   301,   305,    53,  -287,   354,   312,   172,  -287,  -287,
    -287,    86,   663,   326,  -287,  -287,  -287,  1251,    69,  -287,
      63,  -287,  -287,  -287,  -287,  -287,  1554,  -287,    63,   327,
    -287,   867,   325,  -287,   271,  1580,    63,    63,  -287,  -287,
    -287,   361,    63,    86,   363,  -287,  -287,    63,   -29,  -287,
    -287,  -287,  -287,  -287,  -287,  -287,   867,  -287,   312,    86,
    -287,    63,    63,   312,   312,   304,  -287,   363,    63,   312,
    -287,   363,   363,   371,    83,  -287,   312,   363,   290,   312,
    -287,   363,  -287,   363
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -287,  -287,   -30,    24,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,   260,  -287,  -287,   395,  -287,  -287,  -282,   396,  -287,
    -287,   112,  -287,  -287,  -287,  -287,  -287,  -287,   -31,  -287,
    -287,    34,  -287,   -22,  -287,  -287,  -287,   -87,  -287,  -287,
    -146,  -287,  -287,  -287,   -65,  -287,    27,  -287,  -287,   115,
       2,   -32,  -287,     1,  -287,  -287,   -73,   -74,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,   100,  -287,     6,  -287,   -39,
    -287,  -287,  -287,  -287,   -63,  -287,  -287,   330,  -287,  -287,
    -277,  -287,  -287,    89,  -287,    32,   -21,  -287,  -287,  -287,
    -287,  -287,   -15,   335,   -50,  -287,   -84,    67,   -13,  -286,
    -287,   -42
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -296
static const short int yytable[] =
{
     136,   175,    75,    72,   125,   127,   185,   152,   154,   155,
     157,   159,   203,   251,   146,   388,   208,   353,   399,   137,
     139,   142,   104,   148,   150,   151,   406,   209,   168,   169,
     170,   171,   189,   172,   181,   197,   173,    44,   280,   198,
     153,   153,   153,   153,   153,   115,   116,   117,   118,   119,
     417,   120,   107,   143,   177,   144,   145,   193,   382,  -107,
     353,   354,   353,   243,   134,   282,   418,   419,    18,   306,
      20,    17,    18,   192,   115,   116,   117,   118,   119,   179,
     120,   134,   257,    17,    18,   180,   143,  -146,   144,   145,
      39,    40,    41,   420,   421,   422,   423,    43,  -146,    17,
      18,   156,   383,   259,   415,   113,   439,  -146,   194,   114,
     183,   301,   113,   156,   199,   445,   447,   299,   452,   195,
     453,   294,   302,   189,   278,   268,   279,   266,   158,   112,
     277,   269,   315,   317,   162,   163,   164,   165,   166,   167,
     132,   178,   292,   293,   318,   424,   425,   114,   297,   244,
     461,   240,   241,   245,  -116,  -295,   134,   153,   242,   153,
     377,   138,   115,   116,   117,   186,   119,   182,   120,   356,
     187,   188,   493,   417,   183,   314,   134,   298,    17,    18,
     300,   201,   375,   385,   153,   291,   250,   386,   305,   202,
     419,    18,   204,    20,   140,   254,   351,   352,   -42,   -42,
     -77,    39,    40,    41,   308,   101,   102,    42,    43,   113,
     255,   -32,   -32,    39,    40,    41,   420,   421,   422,   423,
      43,   256,   260,   113,   368,   -30,   -30,   369,   265,   -76,
     370,   250,   115,   116,   117,   118,   119,   250,   120,   250,
     189,   357,    11,    12,    13,   258,    15,    17,    18,   105,
     106,   374,   392,   198,   287,   108,   109,   153,   143,   264,
     144,   145,   387,   189,   391,   110,   111,   396,   465,   425,
      39,    40,    41,   128,   129,   130,    42,    43,   295,   303,
     464,   395,   -35,   -35,    17,    18,   397,   261,   262,   371,
     372,   311,   242,   401,   296,   316,   147,   149,   304,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,   342,   343,   344,   345,   346,   347,   348,   349,   358,
     353,   350,   487,   379,   435,   440,   380,   491,   492,   381,
     384,   389,   250,   497,    17,    18,   372,   402,   433,   436,
     501,   441,   412,   503,   457,   250,   448,   443,   413,    44,
     153,   432,   299,   458,   459,   450,   467,   153,   460,   376,
     360,   361,   463,   362,   456,   406,   474,   282,   475,   479,
     418,   498,   462,   153,   291,   502,   115,   116,   117,   118,
     119,   157,   120,   493,   400,   378,    64,    66,   481,   405,
     434,   368,   469,   486,   472,   466,   308,   500,   308,   442,
     471,   394,   407,   454,   488,   476,   444,   468,   473,   207,
     414,     0,     0,   482,   153,     0,   477,   478,   153,   432,
     247,     0,   480,     0,   153,     0,     0,   483,   484,     0,
       0,     0,   250,   272,     0,     0,   416,     0,   273,   274,
       0,   489,   490,     0,   168,   169,   170,   171,   496,   172,
       0,   437,   173,   438,   499,   198,   143,   275,   144,   145,
     276,   235,   236,   237,   238,   239,   240,   241,     0,   143,
       0,   144,   145,   242,     0,     0,   446,     0,   115,   116,
     117,   118,   119,   311,   120,   311,   121,     2,     3,   455,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,     0,    16,     0,    17,    18,    19,    20,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,     0,     0,     0,    42,    43,    44,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,    45,     0,     0,     0,     0,
     242,     0,     0,     0,     0,    46,    47,     0,     0,    48,
      49,     0,    50,     0,    51,    52,     0,     0,     0,    53,
       0,     0,     0,    54,    11,    12,    13,    14,    15,     0,
       0,     0,    17,   248,     0,     0,     0,   237,   238,   239,
     240,   241,    11,    12,    13,    14,    15,   242,    32,    33,
      17,   248,     0,     0,     0,    39,    40,    41,     0,     0,
       0,    42,    43,     0,     0,     0,    32,    33,     0,     0,
       0,     0,     0,    39,    40,    41,     0,     0,     0,    42,
      43,     0,    45,   115,   116,   117,   118,   119,     0,   120,
       0,   126,    46,    47,     0,     0,    48,    49,     0,    50,
      45,    51,    52,   115,   116,   117,   118,   119,   249,   120,
      46,    47,     0,     0,    48,    49,     0,    50,     0,    51,
      52,    11,    12,    13,    14,    15,   355,     0,     0,    17,
      18,     0,     0,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,     0,     0,     0,
       0,     0,    39,    40,    41,     0,     0,     0,    42,    43,
      44,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   168,   169,   170,   171,    45,
     172,     0,     0,   173,     0,     0,     0,     0,     0,    46,
      47,     0,     0,    48,    49,     0,    50,     0,    51,    52,
     143,     0,   144,   145,   134,    11,    12,    13,    14,    15,
       0,    16,     0,    17,    18,     0,     0,     0,     0,     0,
       0,     0,     0,    11,    12,    13,    14,    15,     0,    32,
      33,    17,    18,     0,     0,     0,    39,    40,    41,     0,
       0,     0,    42,    43,     0,     0,     0,    32,    33,     0,
       0,     0,     0,     0,    39,    40,    41,     0,     0,     0,
      42,    43,     0,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    46,    47,     0,     0,    48,    49,     0,
      50,    45,    51,    52,     0,     0,     0,     0,   306,     0,
       0,    46,    47,     0,     0,    48,    49,     0,    50,     0,
      51,    52,     0,     0,     0,     0,   134,    11,    12,    13,
      14,    15,     0,     0,     0,    17,    18,     0,     0,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,     0,     0,     0,     0,     0,    39,    40,
      41,     0,     0,     0,    42,    43,    44,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,     0,     0,    45,     0,     0,   242,    11,
      12,    13,    14,    15,     0,    46,    47,    17,    18,    48,
      49,     0,    50,     0,    51,    52,     0,     0,     0,     0,
       0,     0,     0,    32,    33,     0,     0,     0,     0,     0,
      39,    40,    41,     0,     0,     0,    42,    43,   319,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,     0,    45,     0,     0,
       0,   242,     0,     0,     0,     0,     0,    46,    47,     0,
       0,    48,    49,     0,    50,     0,    51,    52,    11,    12,
      13,    14,    15,     0,    16,     0,    17,    18,     0,     0,
       0,     0,     0,     0,     0,     0,    11,    12,    13,    14,
      15,     0,    32,    33,    17,    18,     0,     0,     0,    39,
      40,    41,     0,     0,     0,    42,    43,     0,     0,     0,
      32,    33,     0,     0,     0,     0,     0,    39,    40,    41,
       0,     0,     0,    42,    43,     0,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,    47,     0,     0,
      48,    49,     0,    50,    45,    51,    52,     0,    11,    12,
      13,    14,    15,     0,    46,    47,    17,   248,    48,    49,
       0,    50,     0,    51,    52,     0,    11,    12,    13,    14,
      15,     0,    32,    33,   270,   248,     0,     0,     0,    39,
      40,    41,     0,     0,     0,    42,    43,     0,     0,     0,
      32,    33,     0,     0,     0,     0,     0,    39,    40,    41,
       0,     0,     0,    42,    43,     0,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,    47,     0,     0,
      48,    49,     0,    50,    45,    51,    52,     0,    11,    12,
      13,    14,    15,     0,    46,    47,   271,   248,    48,    49,
       0,    50,     0,    51,    52,     0,     0,     0,     0,     0,
       0,     0,    32,    33,     0,     0,     0,     0,     0,    39,
      40,    41,     0,     0,     0,    42,    43,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,    45,     0,     0,     0,
       0,   242,     0,     0,     0,     0,    46,    47,     0,     0,
      48,    49,     0,    50,     0,    51,    52,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,     0,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
       0,     0,     0,     0,     0,   242,     0,     0,     0,     0,
       0,   411,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,     0,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,     0,     0,     0,     0,     0,
     242,     0,     0,     0,     0,     0,   470,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,     0,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
       0,     0,     0,     0,     0,   242,     0,     0,     0,     0,
     281,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,     0,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,     0,     0,     0,     0,     0,   242,
       0,     0,     0,     0,   393,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,     0,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,     0,     0,
       0,     0,     0,   242,     0,     0,     0,     0,   408,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,     0,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,     0,     0,     0,     0,     0,   242,     0,     0,
       0,     0,   449,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,     0,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,     0,     0,     0,     0,
       0,   242,     0,     0,     0,   134,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   410,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,     0,
       0,     0,     0,     0,   242,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,     0,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,     0,     0,
       0,     0,   221,   242,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,     0,     0,     0,     0,     0,   242,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,     0,     0,     0,     0,     0,
     242,   233,   234,   235,   236,   237,   238,   239,   240,   241,
       0,     0,     0,     0,     0,   242
};

static const short int yycheck[] =
{
      30,    51,     1,     1,    19,    20,    71,    39,    40,    41,
      42,    43,    85,    97,    35,   292,    90,    50,   300,    32,
      33,    34,    11,    36,    37,    38,    50,    90,    11,    12,
      13,    14,    88,    16,    64,    18,    19,    49,    94,    22,
      39,    40,    41,    42,    43,    10,    11,    12,    13,    14,
       1,    16,    15,    36,    52,    38,    39,    72,    50,    88,
      50,    94,    50,    93,    93,     1,    17,    18,    19,    93,
      21,    18,    19,    72,    10,    11,    12,    13,    14,    55,
      16,    93,   124,    18,    19,    61,    36,    68,    38,    39,
      41,    42,    43,    44,    45,    46,    47,    48,    79,    18,
      19,    48,    94,   133,    94,    90,    94,    88,    68,    90,
      88,    68,    90,    48,    97,   392,   398,    51,   404,    79,
     406,   186,    79,    88,   156,   146,   158,   140,    47,    88,
     151,   146,   205,   207,    45,    46,    47,    48,    49,    50,
      63,    52,   184,   185,   207,    96,    97,    90,   190,    87,
      97,    82,    83,    91,    88,    89,    93,   156,    89,   158,
      96,    89,    10,    11,    12,    13,    14,    49,    16,   253,
      18,    19,    89,     1,    88,   205,    93,   192,    18,    19,
     193,    88,   266,    12,   183,   183,    97,    16,   201,    88,
      18,    19,    88,    21,    88,    51,   246,   247,    92,    93,
      93,    41,    42,    43,   202,    92,    93,    47,    48,    90,
      49,    92,    93,    41,    42,    43,    44,    45,    46,    47,
      48,    63,   133,    90,   256,    92,    93,   257,   139,    93,
     260,   142,    10,    11,    12,    13,    14,   148,    16,   150,
      88,   254,    10,    11,    12,    63,    14,    18,    19,    15,
      16,   264,   294,    22,    94,    15,    16,   256,    36,    95,
      38,    39,   292,    88,   294,    15,    16,   297,    96,    97,
      41,    42,    43,    10,    11,    12,    47,    48,   189,    18,
     426,   296,    92,    93,    18,    19,   299,    18,    19,    88,
      89,   202,    89,   308,    50,   206,    36,    37,    88,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,    17,
      50,   242,   478,    50,   366,   377,    51,   483,   484,    43,
      51,    49,   253,   489,    18,    19,    89,    93,    50,    94,
     496,    10,    88,   499,   419,   266,   398,    43,    88,    49,
     359,   359,    51,    63,    63,    94,   431,   366,    63,   280,
      44,    45,    18,    47,   416,    50,    49,     1,   451,    18,
      17,    10,   424,   382,   382,    95,    10,    11,    12,    13,
      14,   423,    16,    89,   305,   283,     1,     1,   463,   310,
     366,   433,   433,   476,   446,   427,   404,   494,   406,   382,
     440,   296,   312,   407,   479,   454,   384,   432,   448,    89,
     353,    -1,    -1,   465,   423,    -1,   456,   457,   427,   427,
      95,    -1,   462,    -1,   433,    -1,    -1,   467,   468,    -1,
      -1,    -1,   353,    13,    -1,    -1,   357,    -1,    18,    19,
      -1,   481,   482,    -1,    11,    12,    13,    14,   488,    16,
      -1,   372,    19,   374,   494,    22,    36,    37,    38,    39,
      40,    77,    78,    79,    80,    81,    82,    83,    -1,    36,
      -1,    38,    39,    89,    -1,    -1,   397,    -1,    10,    11,
      12,    13,    14,   404,    16,   406,    18,     0,     1,   410,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    16,    -1,    18,    19,    20,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    47,    48,    49,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    68,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    -1,    78,    79,    -1,    -1,    82,
      83,    -1,    85,    -1,    87,    88,    -1,    -1,    -1,    92,
      -1,    -1,    -1,    96,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    19,    -1,    -1,    -1,    79,    80,    81,
      82,    83,    10,    11,    12,    13,    14,    89,    34,    35,
      18,    19,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    34,    35,    -1,    -1,
      -1,    -1,    -1,    41,    42,    43,    -1,    -1,    -1,    47,
      48,    -1,    68,    10,    11,    12,    13,    14,    -1,    16,
      -1,    18,    78,    79,    -1,    -1,    82,    83,    -1,    85,
      68,    87,    88,    10,    11,    12,    13,    14,    94,    16,
      78,    79,    -1,    -1,    82,    83,    -1,    85,    -1,    87,
      88,    10,    11,    12,    13,    14,    94,    -1,    -1,    18,
      19,    -1,    -1,    -1,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    41,    42,    43,    -1,    -1,    -1,    47,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    11,    12,    13,    14,    68,
      16,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    78,
      79,    -1,    -1,    82,    83,    -1,    85,    -1,    87,    88,
      36,    -1,    38,    39,    93,    10,    11,    12,    13,    14,
      -1,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,    34,
      35,    18,    19,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    34,    35,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,    -1,
      47,    48,    -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    -1,    -1,    82,    83,    -1,
      85,    68,    87,    88,    -1,    -1,    -1,    -1,    93,    -1,
      -1,    78,    79,    -1,    -1,    82,    83,    -1,    85,    -1,
      87,    88,    -1,    -1,    -1,    -1,    93,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    41,    42,
      43,    -1,    -1,    -1,    47,    48,    49,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    -1,    -1,    68,    -1,    -1,    89,    10,
      11,    12,    13,    14,    -1,    78,    79,    18,    19,    82,
      83,    -1,    85,    -1,    87,    88,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    34,    35,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    -1,    -1,    -1,    47,    48,    49,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    -1,    68,    -1,    -1,
      -1,    89,    -1,    -1,    -1,    -1,    -1,    78,    79,    -1,
      -1,    82,    83,    -1,    85,    -1,    87,    88,    10,    11,
      12,    13,    14,    -1,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,
      14,    -1,    34,    35,    18,    19,    -1,    -1,    -1,    41,
      42,    43,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      34,    35,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,
      -1,    -1,    -1,    47,    48,    -1,    68,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    -1,    -1,
      82,    83,    -1,    85,    68,    87,    88,    -1,    10,    11,
      12,    13,    14,    -1,    78,    79,    18,    19,    82,    83,
      -1,    85,    -1,    87,    88,    -1,    10,    11,    12,    13,
      14,    -1,    34,    35,    18,    19,    -1,    -1,    -1,    41,
      42,    43,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      34,    35,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,
      -1,    -1,    -1,    47,    48,    -1,    68,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    -1,    -1,
      82,    83,    -1,    85,    68,    87,    88,    -1,    10,    11,
      12,    13,    14,    -1,    78,    79,    18,    19,    82,    83,
      -1,    85,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    34,    35,    -1,    -1,    -1,    -1,    -1,    41,
      42,    43,    -1,    -1,    -1,    47,    48,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    68,    -1,    -1,    -1,
      -1,    89,    -1,    -1,    -1,    -1,    78,    79,    -1,    -1,
      82,    83,    -1,    85,    -1,    87,    88,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    -1,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,
      -1,    95,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    -1,    -1,    95,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    -1,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,
      94,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    -1,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,    89,
      -1,    -1,    -1,    -1,    94,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    -1,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    -1,    -1,
      -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,    94,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    -1,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,
      -1,    -1,    94,    51,    52,    53,    54,    55,    56,    57,
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
      80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,    89,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      89,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    89
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
     159,   160,   161,   162,   168,   169,   170,   171,   172,   173,
     174,   178,   180,   181,   182,   183,   185,   186,   187,   188,
     189,    92,    93,   101,    11,    15,    16,    15,    15,    16,
      15,    16,    88,    90,    90,    10,    11,    12,    13,    14,
      16,    18,   122,   123,   124,   190,    18,   190,    10,    11,
      12,   158,    63,   196,    93,   100,   100,   196,    89,   196,
      88,   109,   196,    36,    38,    39,   184,   109,   196,   109,
     196,   196,   149,   151,   149,   149,    48,   149,    47,   149,
     148,   151,   181,   181,   181,   181,   181,   181,    11,    12,
      13,    14,    16,    19,   184,   192,   193,   148,   181,   101,
     101,   100,    49,    88,   142,   142,    13,    18,    19,    88,
     146,   147,   151,   190,    68,    79,   153,    18,    22,    97,
     192,    88,    88,   154,    88,   175,   176,   175,   155,   172,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    89,   100,    87,    91,   191,   191,    19,    94,
     181,   194,   195,   199,    51,    49,    63,   199,    63,   100,
     181,    18,    19,   152,    95,   181,   196,   194,   184,   190,
      18,    18,    13,    18,    19,    37,    40,   184,   149,   149,
      94,    94,     1,   118,   119,   120,   190,    94,   143,   144,
     145,   148,   199,   199,   142,   181,    50,   199,   190,    51,
     196,    68,    79,    18,    88,   196,    93,   112,   148,   163,
     164,   181,   197,   198,   100,   154,   181,   155,   172,    49,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   192,   192,    50,    94,    94,   194,   196,    17,   138,
      44,    45,    47,   125,   126,   127,   128,   129,   149,   100,
     100,    88,    89,   115,   196,   194,   181,    96,   119,    50,
      51,    43,    50,    94,    51,    12,    16,   100,   178,    49,
     179,   100,   199,    94,   147,   190,   100,   196,   114,   115,
     181,   190,    93,   165,   166,   181,    50,   163,    94,   177,
      63,    95,    88,    88,   195,    94,   181,     1,    17,    18,
      44,    45,    46,    47,    96,    97,   116,   130,   131,   132,
     136,   137,   148,    50,   129,   149,    94,   181,   181,    94,
     199,    10,   144,    43,   183,   178,   181,   115,   199,    94,
      94,   167,   197,   197,   165,   181,   199,   142,    63,    63,
      63,    97,   199,    18,   138,    96,   131,   142,   190,   126,
      95,   100,   199,   100,    49,   154,   167,   100,   100,    18,
     100,   142,   199,   100,   100,   133,   154,   138,   142,   100,
     100,   138,   138,    89,   134,   135,   100,   138,    10,   100,
     135,   138,    95,   138
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
#line 230 "css_parse.y"
    {
            Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 29:
#line 232 "css_parse.y"
    {
	    Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 30:
#line 234 "css_parse.y"
    {
	    Code2((yyvsp[0].el),(yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 31:
#line 236 "css_parse.y"
    {
	    Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 32:
#line 238 "css_parse.y"
    {
	    Code2((yyvsp[0].el),(yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 33:
#line 240 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssRef((yyvsp[-1].el)));
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 34:
#line 244 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc; }
    break;

  case 35:
#line 246 "css_parse.y"
    {
	    Code1((yyvsp[0].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 36:
#line 248 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 37:
#line 252 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 38:
#line 256 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 39:
#line 260 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 40:
#line 264 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 41:
#line 268 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 42:
#line 274 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 44:
#line 276 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 51:
#line 288 "css_parse.y"
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
#line 298 "css_parse.y"
    {
 	    ((yyvsp[-5].el).El())->MakeToken(cssMisc::cur_top->Prog());
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
#line 316 "css_parse.y"
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
#line 336 "css_parse.y"
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
#line 352 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 56:
#line 353 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 57:
#line 357 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 58:
#line 360 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
	    if(cssMisc::cur_enum->name.before(5) == "enum_") /* don't keep anon enums around */
	      delete cssMisc::cur_enum;
	    cssMisc::cur_enum = NULL; }
    break;

  case 59:
#line 367 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_enum = new cssEnumType((const char*)*nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum);
	    cssEl::Done(nm); }
    break;

  case 60:
#line 374 "css_parse.y"
    { /* redefining */
	    if((yyvsp[0].el).El()->GetType() != cssEl::T_EnumType) {
	      yyerror("Attempt to redefine non-enum type as an enum");
	      return cssProg::YY_Err; }
   	    cssMisc::cur_enum = (cssEnumType*)(yyvsp[0].el).El();
 	    cssMisc::cur_enum->enums->Reset(); /* prepare for new enums.. */
	    cssMisc::cur_enum->enum_cnt = 0; }
    break;

  case 61:
#line 381 "css_parse.y"
    { /* anonymous  */
	    String nm = "anonenum_"; nm += String(cssMisc::anon_type_cnt++); nm += "_";
            cssMisc::cur_enum = new cssEnumType((const char*)nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum); }
    break;

  case 66:
#line 395 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 67:
#line 398 "css_parse.y"
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
#line 415 "css_parse.y"
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
#line 435 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[-4].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 70:
#line 439 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[-3].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 71:
#line 443 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 74:
#line 453 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssMisc::cur_class = new cssClassType((const char*)*nm);
	    cssMisc::cur_class->last_top = cssMisc::cur_top;
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
	    cssEl::Done(nm); }
    break;

  case 75:
#line 460 "css_parse.y"
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
#line 482 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_class = new cssClassType((const char*)*nm);
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
            cssEl::Done(nm); }
    break;

  case 77:
#line 488 "css_parse.y"
    {
            if((yyvsp[0].el).El()->GetType() != cssEl::T_ClassType) {
              yyerror("Attempt to redefine non-class type as a class");
              return cssProg::YY_Err;
            }
            cssMisc::cur_class = (cssClassType*)(yyvsp[0].el).El(); }
    break;

  case 80:
#line 500 "css_parse.y"
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
#line 511 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); }
    break;

  case 91:
#line 528 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 92:
#line 529 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 96:
#line 533 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 97:
#line 536 "css_parse.y"
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
#line 548 "css_parse.y"
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
#line 564 "css_parse.y"
    {
            /* swap places with void marker */
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_top->Prog()->Stack()->Push(&cssMisc::Void);
            cssMisc::cur_top->Prog()->Stack()->Push(nm); }
    break;

  case 100:
#line 572 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 101:
#line 573 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 102:
#line 577 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((int)*(yyvsp[-1].el).El())); }
    break;

  case 103:
#line 581 "css_parse.y"
    {       /* pre-declare function */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[-3].el).El();
            cssMisc::cur_class->GetComments(fun, (yyvsp[0].el));
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr, true); }
    break;

  case 104:
#line 587 "css_parse.y"
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
#line 603 "css_parse.y"
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
#line 620 "css_parse.y"
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
#line 640 "css_parse.y"
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
#line 661 "css_parse.y"
    { (yyval.el).Reset(); }
    break;

  case 109:
#line 662 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); }
    break;

  case 110:
#line 663 "css_parse.y"
    { (yyval.el) = (yyvsp[-1].el); }
    break;

  case 111:
#line 666 "css_parse.y"
    {	/* pre-declare function */
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival));	(yyval.ival) = (yyvsp[-1].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[-3].el).El();
	    fun->argc = (yyvsp[-2].ival); fun->GetArgDefs(); }
    break;

  case 112:
#line 672 "css_parse.y"
    {
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[-3].el).El();
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[0].ival)+1); (yyval.ival) = (yyvsp[-1].ival)-1;
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    cssCodeBlock* bra_blk = (cssCodeBlock*)(cssMisc::cur_top->Prog()->insts[(yyvsp[0].ival)]->inst.El());
	    cssMisc::cur_top->Prog()->insts[(yyvsp[0].ival)]->SetInst((yyvsp[-3].el)); /* replace bra_blk with fun */
	    cssMisc::cur_top->RemoveVar(bra_blk);	/* get rid of the bra_blk */
	    cssMisc::cur_top->SetPush(fun->fun); /* this is the one we want to push */
	    fun->Define(cssMisc::ConstExpr); }
    break;

  case 113:
#line 684 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 114:
#line 692 "css_parse.y"
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
	    cssCodeBlock* bra_blk = (cssCodeBlock*)(cssMisc::cur_top->Prog()->insts[(yyvsp[0].ival)]->inst.El());
	    cssMisc::cur_top->Prog()->insts[(yyvsp[0].ival)]->SetInst((yyvsp[-3].el)); /* replace bra with existing one */
	    cssMisc::cur_top->RemoveVar(bra_blk);	/* get rid of the bra_blk */
	    fun->fun->Reset();	/* reset it for new defn */
	    cssMisc::cur_top->SetPush(fun->fun); /* push this one */
	    fun->Define(cssMisc::ConstExpr); }
    break;

  case 115:
#line 710 "css_parse.y"
    {
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[-3].el).El();
	    cssClassType* cls = cssMisc::cur_class;
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr);
	    cssMisc::cur_method = fun; /* this is now the current method */
	    cssElPtr fun_ptr;  fun_ptr.SetNVirtMethod(cls, cls->methods->GetIndex(fun));
	    css_progdx nxt_ln = Code1(fun_ptr); /* code it so it shows up in a listing.. */
	    cssMisc::cur_top->SetPush(fun->fun); /* put it on the stack.. */
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine(nxt_ln+1); (yyval.ival) = (yyvsp[-1].ival)-1; }
    break;

  case 116:
#line 723 "css_parse.y"
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
#line 736 "css_parse.y"
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
#line 762 "css_parse.y"
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
#line 783 "css_parse.y"
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
#line 805 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 121:
#line 806 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 122:
#line 807 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 123:
#line 808 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 124:
#line 811 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 125:
#line 812 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival) + 1; }
    break;

  case 126:
#line 815 "css_parse.y"
    { cssMisc::parsing_args = false; }
    break;

  case 127:
#line 816 "css_parse.y"
    {
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push((yyvsp[0].el).El()); }
    break;

  case 128:
#line 822 "css_parse.y"
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
#line 830 "css_parse.y"
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
#line 841 "css_parse.y"
    {
	    if(((yyvsp[-2].ival) < 0) && ((yyvsp[0].ival) < 0)) (yyval.ival) = -1;
	    else if(((yyvsp[-2].ival) >= 0) && ((yyvsp[0].ival) < 0)) (yyval.ival) = (yyvsp[-2].ival);
	    else if(((yyvsp[-2].ival) < 0) && ((yyvsp[0].ival) >= 0)) (yyval.ival) = (yyvsp[0].ival);
	    else (yyval.ival) = -1; }
    break;

  case 132:
#line 848 "css_parse.y"
    {
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      (yyval.ival) = Code2(tmp->ptr, cssBI::constr); }
	    else (yyval.ival) = -1;
	    cssEl::Done(tmp); }
    break;

  case 133:
#line 857 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = ""; }
    break;

  case 134:
#line 858 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "extern"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 135:
#line 859 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "static"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 136:
#line 860 "css_parse.y"
    { /* use const expr prog to code stuff */
	  (yyvsp[0].el).El()->tmp_str = "const"; (yyval.el) = (yyvsp[0].el); cssMisc::CodeConstExpr(); }
    break;

  case 137:
#line 862 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 138:
#line 863 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = ""; (yyval.el) = (yyvsp[0].el); }
    break;

  case 139:
#line 864 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 140:
#line 865 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 141:
#line 868 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); cssMisc::cur_type = (yyval.el); }
    break;

  case 142:
#line 869 "css_parse.y"
    {
	  if(((yyvsp[-1].el).El()->GetParse() != CSS_PTR) || ((yyvsp[0].ival) >= 2)) {
	    (yyval.el) = cssMisc::cur_top->AddPtrType((yyvsp[-1].el).El()); cssMisc::cur_type = (yyval.el);
	  }
	  else {
	    (yyval.el) = (yyvsp[-1].el); cssMisc::cur_type = (yyval.el); } }
    break;

  case 143:
#line 875 "css_parse.y"
    {
	  (yyval.el) = cssMisc::cur_top->AddRefType((yyvsp[-1].el).El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 144:
#line 877 "css_parse.y"
    {
	  if(((yyvsp[-2].el).El()->GetParse() != CSS_PTR) || ((yyvsp[-1].ival) >= 2)) {
            cssElPtr npt = cssMisc::cur_top->AddPtrType((yyvsp[-2].el).El());
            (yyval.el) = cssMisc::cur_top->AddRefType(npt.El()); cssMisc::cur_type = (yyval.el); }
	  else {
	    (yyval.el) = cssMisc::cur_top->AddRefType((yyvsp[-2].el).El()); cssMisc::cur_type = (yyval.el); } }
    break;

  case 147:
#line 887 "css_parse.y"
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
#line 898 "css_parse.y"
    {
           if((yyvsp[-1].el).El()->GetType() != cssEl::T_ClassType) {
	     yyerror("scoping of non-class type");
	     return cssProg::YY_Err; }
           cssMisc::cur_scope = (yyvsp[-1].el).El(); }
    break;

  case 149:
#line 903 "css_parse.y"
    {
           cssMisc::cur_scope = (yyvsp[-1].el).El(); }
    break;

  case 152:
#line 911 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 153:
#line 912 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 154:
#line 915 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastif = -1; }
    break;

  case 158:
#line 923 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 161:
#line 926 "css_parse.y"
    { Code1((yyvsp[-2].el)); (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 162:
#line 927 "css_parse.y"
    { Code1((yyvsp[-3].el)); (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 163:
#line 928 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[-1].el)); }
    break;

  case 164:
#line 929 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[-1].el)); }
    break;

  case 168:
#line 935 "css_parse.y"
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
#line 944 "css_parse.y"
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
#line 962 "css_parse.y"
    {
            (yyval.ival) = (yyvsp[-5].ival);
	    /* value to switch on */
/* 	    cssMisc::ConstExpr->Stack()->Push(new cssRef(cssMisc::ConstExpr->Autos()->Push */
/* 							  (new cssString(0,cssSwitchVar_Name)))); */
/* 	    /\* bogus return value *\/ */
/* 	    cssMisc::ConstExpr->Stack()->Push(new cssRef(cssMisc::ConstExpr->Autos()->Push */
/* 							  (new cssInt(0,cssRetv_Name)))); */
            cssCodeBlock* jmp_blk = (cssCodeBlock*)(cssMisc::cur_top->Prog()->owner_blk);
	    /* make the jump-table address array: ints whose name is val, val is adr */
	    jmp_blk->code->Stack()->Push(new cssString(cssSwitchJump_Name));
	    jmp_blk->code->Stack()->Push(new cssInt(1)); /* type of ary element */
	    jmp_blk->code->Stack()->Push(new cssInt(1)); /* number of dimensions */
  	    cssMisc::VoidArray.tmp_str = "literal";
	    cssMisc::VoidArray.MakeToken(jmp_blk->code); /* make the array */
	    cssMisc::VoidArray.tmp_str = "";
	    cssRef* aryptr = (cssRef*) jmp_blk->code->Stack()->Pop();
	    ((cssArray*)aryptr->ptr.El())->items->DelPop(); /* get rid of first el */
	    Code1(cssBI::switch_jump); /* this gets expr as arg! */
	    cssEl::Done(aryptr); }
    break;

  case 174:
#line 984 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssSwitchBlock_Name);
	    blk->loop_type = cssCodeBlock::SWITCH;
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 175:
#line 993 "css_parse.y"
    {
	   cssProg* cp = cssMisc::cur_top->Prog();
	   if(cp->owner_blk == NULL) {
	     yyerror("for loop current prog should have owner_blk, doesnt!");
	   }
	   cssMisc::cur_top->Pop(); /* pop the for_incr block */
	   Code1(cssBI::pop);	/* and code for getting rid of cond val */
	   cp = cssMisc::cur_top->Prog(); /* current guy */
	   cssInst* for_incr = cp->insts[(yyvsp[-2].ival)];
	   cssInst* for_loop_stmt = cp->insts[(yyvsp[-1].ival)];
	   /* swap */
	   cp->insts[(yyvsp[-2].ival)] = for_loop_stmt;
	   cp->insts[(yyvsp[-1].ival)] = for_incr;
	   cssMisc::cur_top->Pop(); /* pop the whole for loop! */
	   (yyval.ival) = (yyvsp[-5].ival); }
    break;

  case 176:
#line 1008 "css_parse.y"
    {
	   cssProg* cp = cssMisc::cur_top->Prog();
	   if(cp->owner_blk == NULL) {
	     yyerror("for loop current prog should have owner_blk, doesnt!");
	   }
	   cssMisc::cur_top->Pop(); /* pop the for_incr block */
	   Code1(cssBI::pop);	/* and code for getting rid of cond val */
	   /* swap the order of these: $6 = for_end_paren = stmt block,  $5 = for_incr = incr block */
	   cp = cssMisc::cur_top->Prog(); /* current guy */
	   cssInst* for_incr = cp->insts[(yyvsp[-2].ival)];
	   cssInst* for_loop_stmt = cp->insts[(yyvsp[-1].ival)];
	   /* swap */
	   cp->insts[(yyvsp[-2].ival)] = for_loop_stmt;
	   cp->insts[(yyvsp[-1].ival)] = for_incr;
	   /* check if stmt is a new block: if so, then don't pop this guy */
	   
	   cssMisc::cur_top->Pop(); /* pop the whole for loop! */
	   (yyval.ival) = (yyvsp[-6].ival); }
    break;

  case 177:
#line 1031 "css_parse.y"
    { /* for loop contained within own block */
            cssCodeBlock* blk = new cssCodeBlock(cssForLoop_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 180:
#line 1043 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 183:
#line 1055 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of cond, push incr */
	      cssMisc::cur_top->ResetParseFlags();
	      cssCodeBlock* blk = new cssCodeBlock(cssForIncr_Name);
	      blk->owner_prog = cssMisc::cur_top->Prog();
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_back = 3; /* go back 3 to the cond */
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 184:
#line 1066 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of incr */
	      cssCodeBlock* blk = new cssCodeBlock(cssForLoopStmt_Name);
	      blk->owner_prog = cssMisc::cur_top->Prog();
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_type = cssCodeBlock::FOR;
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 186:
#line 1080 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssDoLoop_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->loop_type = cssCodeBlock::DO;
	    cssMisc::cur_top->AddStatic(blk); /* while is all inside this do! */
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 187:
#line 1090 "css_parse.y"
    {
            cssProg* prv_prg = cssMisc::cur_top->PrvProg();
	    if((prv_prg != NULL) && (prv_prg->owner_blk != NULL)
	       && (prv_prg->owner_blk->loop_type == cssCodeBlock::DO)) {
	      cssMisc::cur_top->Pop(); /* pop the if_true block from cond
					  null and should be removed (todo:!) */	      
	      (yyval.ival) = Code1(cssBI::doloop); /* this is inside of the do block */
	      cssMisc::cur_top->Pop(); /* pop the do_loop from do */
	      
	    }
	    else {
	      cssProg* cp = cssMisc::cur_top->Prog();
	      if(cp->owner_blk == NULL) {
		yyerror("while loop current prog should have owner_blk, doesnt!");
	      }
	      else {
		cp->owner_blk->loop_back = 2; /* jump back 2 steps to cond if loop was run! */
		cp->owner_blk->loop_type = cssCodeBlock::WHILE;
	      }	    
	      cssMisc::cur_top->Pop(); /* pop the if_true block */
	      Code1(cssBI::pop);	/* and code for getting rid of cond val */
	      (yyval.ival) = (yyvsp[-1].ival);
	    } }
    break;

  case 188:
#line 1113 "css_parse.y"
    {
	  cssProg* cp = cssMisc::cur_top->Prog();
	  if(cp->owner_blk == NULL) {
	    yyerror("while loop current prog should have owner_blk, doesnt!");
	  }
	  else {
	    cp->owner_blk->loop_back = 2; /* jump back 2 steps to cond if loop was run! */
	    cp->owner_blk->loop_type = cssCodeBlock::WHILE;
	  }	    
	  cssMisc::cur_top->Pop(); /* pop the if_true block */
	  Code1(cssBI::pop);	/* and code for getting rid of cond val */
	  (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 189:
#line 1127 "css_parse.y"
    { }
    break;

  case 190:
#line 1130 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 191:
#line 1133 "css_parse.y"
    {
	  cssMisc::cur_top->Pop(); /* pop the if_true block */
	  (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 192:
#line 1136 "css_parse.y"
    {
	  cssMisc::cur_top->Pop(); /* pop the if_false block */
	  /* now check for other else blocks that need popping! */
	  while((cssMisc::cur_top->Prog()->owner_blk != NULL) &&
		(cssMisc::cur_top->Prog()->owner_blk->action == cssCodeBlock::ELSE)) {
	    cssMisc::cur_top->Pop(); } /* pop residual elses! */
	  (yyval.ival) = (yyvsp[-1].ival);  }
    break;

  case 193:
#line 1143 "css_parse.y"
    { /* do not pop the ifstmt here!! */
	  (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 194:
#line 1147 "css_parse.y"
    {
          cssMisc::cur_top->Prog()->lastif = cssMisc::cur_top->Prog()->size; }
    break;

  case 195:
#line 1151 "css_parse.y"
    {
  	    css_progdx tmp = cssMisc::cur_top->Prog()->lastif;
  	    if(tmp < 0) {
	      yyerror("else without matching if");
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_top->Prog()->lastif = -1; /* reset it */
            cssCodeBlock* blk = new cssCodeBlock(cssElseBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::ELSE; /* start block of else */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code2(blk, cssBI::pop); /* pop after else to get rid of prev if cond */
	    cssMisc::cur_top->Push(blk->code); }
    break;

  case 196:
#line 1166 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssIfTrueBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	    cssMisc::cur_top->AddStatic(blk);
	    Code1(blk); cssMisc::cur_top->Push(blk->code); (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 197:
#line 1174 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 198:
#line 1182 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); }
    break;

  case 199:
#line 1186 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->SetPush(blk->code); }
    break;

  case 200:
#line 1193 "css_parse.y"
    {  }
    break;

  case 201:
#line 1196 "css_parse.y"
    {
            cssMisc::cur_top->SetPop();
	    cssScriptFun* sfun = cssMisc::cur_top->Prog()->owner_fun;
	    if((sfun != NULL) && (sfun->GetType() == cssEl::T_MbrScriptFun)) {
	      cssMisc::cur_class = NULL; /* get rid of current class pointer.. */
	      cssMisc::cur_method = NULL; } /* and current method pointer */
	  }
    break;

  case 202:
#line 1205 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 205:
#line 1211 "css_parse.y"
    { Code1(cssBI::asgn); }
    break;

  case 206:
#line 1212 "css_parse.y"
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

  case 207:
#line 1222 "css_parse.y"
    { Code1(cssBI::asgn_add); }
    break;

  case 208:
#line 1223 "css_parse.y"
    { Code1(cssBI::asgn_sub); }
    break;

  case 209:
#line 1224 "css_parse.y"
    { Code1(cssBI::asgn_mult); }
    break;

  case 210:
#line 1225 "css_parse.y"
    { Code1(cssBI::asgn_div); }
    break;

  case 211:
#line 1226 "css_parse.y"
    { Code1(cssBI::asgn_mod); }
    break;

  case 212:
#line 1227 "css_parse.y"
    { Code1(cssBI::asgn_lshift); }
    break;

  case 213:
#line 1228 "css_parse.y"
    { Code1(cssBI::asgn_rshift); }
    break;

  case 214:
#line 1229 "css_parse.y"
    { Code1(cssBI::asgn_and); }
    break;

  case 215:
#line 1230 "css_parse.y"
    { Code1(cssBI::asgn_xor); }
    break;

  case 216:
#line 1231 "css_parse.y"
    { Code1(cssBI::asgn_or); }
    break;

  case 217:
#line 1232 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code2((yyvsp[0].el),(yyvsp[-2].el)); }
    break;

  case 218:
#line 1233 "css_parse.y"
    { (yyval.ival) = (yyvsp[-3].ival); Code2((yyvsp[-2].el),(yyvsp[-4].el)); }
    break;

  case 219:
#line 1234 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[-2].ival); Code2((yyvsp[-1].el),(yyvsp[-3].el)); }
    break;

  case 220:
#line 1236 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code1((yyvsp[-2].el)); }
    break;

  case 221:
#line 1237 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code1((yyvsp[-4].el)); }
    break;

  case 222:
#line 1238 "css_parse.y"
    { Code1(cssBI::lor); }
    break;

  case 223:
#line 1239 "css_parse.y"
    { Code1(cssBI::land); }
    break;

  case 224:
#line 1240 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::addr_of); }
    break;

  case 225:
#line 1241 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::de_ptr); }
    break;

  case 226:
#line 1242 "css_parse.y"
    { Code1(cssBI::gt); }
    break;

  case 227:
#line 1243 "css_parse.y"
    { Code1(cssBI::ge); }
    break;

  case 228:
#line 1244 "css_parse.y"
    { Code1(cssBI::lt); }
    break;

  case 229:
#line 1245 "css_parse.y"
    { Code1(cssBI::le); }
    break;

  case 230:
#line 1246 "css_parse.y"
    { Code1(cssBI::eq); }
    break;

  case 231:
#line 1247 "css_parse.y"
    { Code1(cssBI::ne); }
    break;

  case 232:
#line 1248 "css_parse.y"
    { Code1(cssBI::cond); }
    break;

  case 233:
#line 1249 "css_parse.y"
    { Code1(cssBI::add); }
    break;

  case 234:
#line 1250 "css_parse.y"
    { Code1(cssBI::sub); }
    break;

  case 235:
#line 1251 "css_parse.y"
    { Code1(cssBI::mul); }
    break;

  case 236:
#line 1252 "css_parse.y"
    { Code1(cssBI::div); }
    break;

  case 237:
#line 1253 "css_parse.y"
    { Code1(cssBI::modulo); }
    break;

  case 238:
#line 1254 "css_parse.y"
    { Code1(cssBI::lshift); }
    break;

  case 239:
#line 1255 "css_parse.y"
    { Code1(cssBI::rshift); }
    break;

  case 240:
#line 1256 "css_parse.y"
    { Code1(cssBI::bit_and); }
    break;

  case 241:
#line 1257 "css_parse.y"
    { Code1(cssBI::bit_xor); }
    break;

  case 242:
#line 1258 "css_parse.y"
    { Code1(cssBI::bit_or); }
    break;

  case 243:
#line 1259 "css_parse.y"
    { Code1(cssBI::asgn_post_pp); }
    break;

  case 244:
#line 1260 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::asgn_pre_pp); }
    break;

  case 245:
#line 1261 "css_parse.y"
    { Code1(cssBI::asgn_post_mm); }
    break;

  case 246:
#line 1262 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::asgn_pre_mm); }
    break;

  case 247:
#line 1263 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::neg); }
    break;

  case 248:
#line 1264 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::lnot); }
    break;

  case 249:
#line 1265 "css_parse.y"
    { Code1(cssBI::de_array); }
    break;

  case 250:
#line 1266 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-2].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = (yyvsp[0].ival); Code2((yyvsp[-2].el), cssBI::cast); }
    break;

  case 251:
#line 1272 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
  	    (yyval.ival) = (yyvsp[-1].ival); Code2((yyvsp[-3].el), cssBI::cast); }
    break;

  case 253:
#line 1279 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 261:
#line 1295 "css_parse.y"
    { (yyval.ival) = Code2(cssBI::push_next, (yyvsp[0].el)); }
    break;

  case 262:
#line 1296 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].el_ival).ival; Code1((yyvsp[-1].el_ival).el); }
    break;

  case 263:
#line 1297 "css_parse.y"
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

  case 264:
#line 1312 "css_parse.y"
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

  case 265:
#line 1328 "css_parse.y"
    { Code2((yyvsp[0].el), cssBI::points_at);}
    break;

  case 266:
#line 1329 "css_parse.y"
    {
	    int mbno = (yyvsp[-2].el).El()->GetMemberNo((const char*)*((yyvsp[0].el).El()));
	    if(mbno < 0) { (yyval.ival) = Code3((yyvsp[-2].el), (yyvsp[0].el), cssBI::points_at); }
	    else { cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
		   (yyval.ival) = Code3((yyvsp[-2].el), tmpint, cssBI::points_at); } }
    break;

  case 267:
#line 1337 "css_parse.y"
    { (yyval.ival) = Code3(cssBI::push_root, (yyvsp[0].el), cssBI::points_at); }
    break;

  case 268:
#line 1338 "css_parse.y"
    {
	  cssMisc::cur_scope = NULL;
	    cssEl* scp = (yyvsp[-1].el).El()->GetScoped((const char*)*((yyvsp[0].el).El()));
	    if(scp != &cssMisc::Void) {  (yyval.ival) = Code1(scp); }
	    else { (yyval.ival) = Code3((yyvsp[-1].el), (yyvsp[0].el), cssBI::scoper); } }
    break;

  case 269:
#line 1344 "css_parse.y"
    {
	  /* argstop is put in by member_fun; member_fun skips over end jump, 
	     uses it to find member_call*/
	  (yyval.ival) = (yyvsp[-2].el_ival).ival;
	  cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine(Code1(cssBI::member_call)); }
    break;

  case 270:
#line 1349 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[-3].el_ival).ival;
	  cssMisc::cur_top->Prog()->insts[(yyvsp[-2].ival)-1]->SetLine(Code1(cssBI::member_call));
	  if(((yyvsp[-3].el_ival).el.El()->GetType() == cssEl::T_ElCFun) ||
	     ((yyvsp[-3].el_ival).el.El()->GetType() == cssEl::T_MbrCFun) ||
	     ((yyvsp[-3].el_ival).el.El()->GetType() == cssEl::T_ScriptFun) ||
	     ((yyvsp[-3].el_ival).el.El()->GetType() == cssEl::T_MbrScriptFun)) {
	    cssElFun* fun = (cssElFun*)(yyvsp[-3].el_ival).el.El();
	    int max_args = fun->argc;  int min_args = (fun->argc - fun->arg_defs.size);
	    int act_args = (yyvsp[-1].ival);
	    if(fun->GetType() == cssEl::T_MbrScriptFun) { max_args--; min_args--; }
	    if((fun->argc >= 0) && (act_args > max_args)) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(), "Too many arguments for function:",fun->name,", should have at most:", String(max_args), "got:",String(act_args)); }
	    if((fun->argc >= 0) && (act_args < min_args)) {
              cssMisc::Warning(cssMisc::cur_top->Prog(), "Too few arguments for function:",fun->name,", should have at least:",String(min_args),"got:",String(act_args)); } } }
    break;

  case 271:
#line 1366 "css_parse.y"
    { Code2((yyvsp[-1].el), cssBI::member_fun);
                                                  (yyval.el_ival).ival = (yyvsp[-3].ival); (yyval.el_ival).el.Reset(); }
    break;

  case 272:
#line 1368 "css_parse.y"
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

  case 273:
#line 1386 "css_parse.y"
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

  case 274:
#line 1410 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].nm))); }
    break;

  case 275:
#line 1414 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].nm))); }
    break;

  case 276:
#line 1416 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName())); }
    break;

  case 277:
#line 1418 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName())); }
    break;

  case 278:
#line 1420 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 279:
#line 1424 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 280:
#line 1428 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding function:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 283:
#line 1438 "css_parse.y"
    {
   	    String tmpstr = String((yyvsp[0].nm));
            (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr); }
    break;

  case 285:
#line 1442 "css_parse.y"
    { /* this is source of shift-reduce problems */
	    String tmpstr = String((yyvsp[0].el).El()->GetName());
	    (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr); }
    break;

  case 291:
#line 1454 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 292:
#line 1455 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival) + 1; }
    break;

  case 294:
#line 1459 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 295:
#line 1462 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); }
    break;

  case 299:
#line 1470 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 300:
#line 1473 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;


    }

/* Line 1037 of yacc.c.  */
#line 3804 "y.tab.c"

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


#line 1477 "css_parse.y"


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

