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
     CSS_LIST = 292,
     CSS_EXIT = 293,
     CSS_ALIAS = 294,
     CSS_REMOVE = 295,
     CSS_STATUS = 296,
     CSS_TYPECMD = 297,
     CSS_CONT = 298,
     CSS_HELP = 299,
     CSS_EXTERN = 300,
     CSS_STATIC = 301,
     CSS_CONST = 302,
     CSS_PRIVATE = 303,
     CSS_PUBLIC = 304,
     CSS_PROTECTED = 305,
     CSS_VIRTUAL = 306,
     CSS_INLINE = 307,
     CSS_ASGN_OR = 308,
     CSS_ASGN_XOR = 309,
     CSS_ASGN_AND = 310,
     CSS_ASGN_RSHIFT = 311,
     CSS_ASGN_LSHIFT = 312,
     CSS_ASGN_MOD = 313,
     CSS_ASGN_DIV = 314,
     CSS_ASGN_MULT = 315,
     CSS_ASGN_SUB = 316,
     CSS_ASGN_ADD = 317,
     CSS_OR = 318,
     CSS_AND = 319,
     CSS_NE = 320,
     CSS_EQ = 321,
     CSS_LE = 322,
     CSS_LT = 323,
     CSS_GE = 324,
     CSS_GT = 325,
     CSS_RSHIFT = 326,
     CSS_LSHIFT = 327,
     CSS_MINMIN = 328,
     CSS_PLUSPLUS = 329,
     CSS_UNARY = 330,
     CSS_NOT = 331,
     CSS_UNARYMINUS = 332,
     CSS_SCOPER = 333,
     CSS_POINTSAT = 334
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
#define CSS_LIST 292
#define CSS_EXIT 293
#define CSS_ALIAS 294
#define CSS_REMOVE 295
#define CSS_STATUS 296
#define CSS_TYPECMD 297
#define CSS_CONT 298
#define CSS_HELP 299
#define CSS_EXTERN 300
#define CSS_STATIC 301
#define CSS_CONST 302
#define CSS_PRIVATE 303
#define CSS_PUBLIC 304
#define CSS_PROTECTED 305
#define CSS_VIRTUAL 306
#define CSS_INLINE 307
#define CSS_ASGN_OR 308
#define CSS_ASGN_XOR 309
#define CSS_ASGN_AND 310
#define CSS_ASGN_RSHIFT 311
#define CSS_ASGN_LSHIFT 312
#define CSS_ASGN_MOD 313
#define CSS_ASGN_DIV 314
#define CSS_ASGN_MULT 315
#define CSS_ASGN_SUB 316
#define CSS_ASGN_ADD 317
#define CSS_OR 318
#define CSS_AND 319
#define CSS_NE 320
#define CSS_EQ 321
#define CSS_LE 322
#define CSS_LT 323
#define CSS_GE 324
#define CSS_GT 325
#define CSS_RSHIFT 326
#define CSS_LSHIFT 327
#define CSS_MINMIN 328
#define CSS_PLUSPLUS 329
#define CSS_UNARY 330
#define CSS_NOT 331
#define CSS_UNARYMINUS 332
#define CSS_SCOPER 333
#define CSS_POINTSAT 334




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
#line 278 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 213 of yacc.c.  */
#line 290 "y.tab.c"

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
#define YYLAST   1804

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  102
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  96
/* YYNRULES -- Number of rules. */
#define YYNRULES  302
/* YYNRULES -- Number of states. */
#define YYNSTATES  529

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   334

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      96,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    85,    72,     2,
      92,    98,    83,    81,    54,    82,    91,    84,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    67,    97,
       2,    55,     2,    66,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    93,     2,    99,    71,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    53,    70,   100,   101,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    68,    69,
      73,    74,    75,    76,    77,    78,    79,    80,    86,    87,
      88,    89,    90,    94,    95
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     7,    11,    15,    18,    21,    24,
      28,    32,    34,    36,    38,    40,    42,    44,    46,    48,
      51,    54,    57,    60,    63,    66,    69,    72,    74,    76,
      79,    82,    86,    88,    90,    94,   100,   103,   107,   109,
     113,   117,   119,   122,   125,   128,   131,   134,   137,   138,
     141,   146,   148,   150,   152,   154,   156,   158,   163,   171,
     178,   185,   187,   190,   194,   201,   204,   207,   209,   211,
     214,   216,   219,   221,   223,   227,   236,   244,   249,   251,
     255,   257,   259,   261,   263,   265,   269,   271,   273,   276,
     278,   281,   283,   285,   287,   289,   292,   294,   296,   299,
     302,   305,   308,   310,   315,   322,   323,   325,   328,   332,
     337,   342,   348,   355,   358,   359,   361,   364,   369,   374,
     379,   385,   390,   393,   397,   400,   404,   407,   411,   415,
     420,   422,   426,   428,   432,   435,   438,   440,   444,   446,
     448,   451,   454,   457,   460,   463,   467,   471,   473,   476,
     479,   483,   485,   487,   490,   493,   496,   498,   500,   502,
     505,   507,   509,   511,   513,   516,   518,   520,   524,   529,
     532,   535,   537,   539,   541,   545,   548,   550,   552,   554,
     561,   563,   573,   584,   595,   607,   609,   612,   614,   619,
     624,   626,   631,   636,   640,   644,   646,   648,   652,   654,
     656,   658,   660,   662,   664,   668,   672,   676,   680,   684,
     688,   692,   696,   700,   704,   708,   712,   716,   722,   727,
     731,   737,   741,   745,   748,   751,   755,   759,   763,   767,
     771,   775,   781,   785,   789,   793,   797,   801,   805,   809,
     813,   817,   821,   824,   827,   830,   833,   836,   839,   844,
     849,   854,   856,   860,   862,   864,   866,   868,   870,   872,
     874,   876,   878,   880,   882,   885,   889,   892,   896,   900,
     903,   906,   910,   915,   920,   925,   929,   931,   933,   935,
     937,   939,   941,   943,   945,   947,   949,   951,   953,   955,
     957,   959,   961,   963,   965,   969,   971,   973,   974,   976,
     980,   982,   984
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     103,     0,    -1,    -1,   103,    96,    -1,   103,   106,   105,
      -1,   103,   112,   105,    -1,   103,   177,    -1,   103,   114,
      -1,   103,   158,    -1,   103,     1,   105,    -1,   103,    38,
     105,    -1,    97,    -1,    97,    -1,    96,    -1,   107,    -1,
     108,    -1,   109,    -1,   110,    -1,   111,    -1,     3,    11,
      -1,     4,    16,    -1,     4,    15,    -1,     5,    15,    -1,
       6,    16,    -1,     6,    15,    -1,     7,    15,    -1,     7,
      16,    -1,     8,    -1,     9,    -1,    36,   113,    -1,    42,
     113,    -1,    42,   193,    18,    -1,    43,    -1,    37,    -1,
      37,   193,   191,    -1,    37,    92,   193,   191,    98,    -1,
      40,   113,    -1,    40,   193,    18,    -1,    41,    -1,    39,
     181,   187,    -1,    39,   181,   181,    -1,    44,    -1,    44,
     181,    -1,    44,    13,    -1,    44,    39,    -1,    44,    44,
      -1,    44,    19,    -1,    44,    18,    -1,    -1,   193,   191,
      -1,    92,   193,   191,    98,    -1,   115,    -1,   116,    -1,
     117,    -1,   120,    -1,   125,    -1,   143,    -1,   152,   150,
     197,   104,    -1,   152,   187,    55,   193,   178,   197,   104,
      -1,   186,    55,   193,   178,   197,   104,    -1,   152,   187,
     193,   118,   197,   104,    -1,   119,    -1,   118,   119,    -1,
      93,   178,    99,    -1,   121,    53,   122,   100,   197,   104,
      -1,    21,   187,    -1,    21,    18,    -1,    21,    -1,   123,
      -1,   122,   123,    -1,   124,    -1,   124,    54,    -1,     1,
      -1,   187,    -1,   187,    55,    10,    -1,    20,   126,    53,
     142,   134,   100,   197,   104,    -1,    20,   126,    53,   142,
     100,   197,   104,    -1,    20,   128,   197,   104,    -1,   127,
      -1,   127,    67,   129,    -1,   187,    -1,    18,    -1,   187,
      -1,    18,    -1,   130,    -1,   129,    54,   130,    -1,   131,
      -1,   153,    -1,   132,   153,    -1,   133,    -1,   132,   133,
      -1,    49,    -1,    51,    -1,    48,    -1,   135,    -1,   134,
     135,    -1,   136,    -1,   140,    -1,   120,   142,    -1,    49,
      67,    -1,    48,    67,    -1,    50,    67,    -1,     1,    -1,
     152,   187,   104,   142,    -1,   152,   187,   137,   138,   104,
     142,    -1,    -1,   139,    -1,   138,   139,    -1,    93,    10,
      99,    -1,   141,   146,   104,   142,    -1,    18,   146,   104,
     142,    -1,   101,    18,   146,   104,   142,    -1,    51,   101,
      18,   146,   104,   142,    -1,   152,   187,    -1,    -1,    17,
      -1,   142,    17,    -1,   144,   146,   197,   104,    -1,   144,
     146,   197,   175,    -1,   152,    13,   146,   104,    -1,   152,
      13,   146,   197,   175,    -1,   145,   146,   197,   176,    -1,
     152,   187,    -1,   152,   155,   187,    -1,   155,    18,    -1,
     155,   101,    18,    -1,    92,    98,    -1,    92,   147,    98,
      -1,    92,    98,    47,    -1,    92,   147,    98,    47,    -1,
     148,    -1,   147,    54,   148,    -1,   149,    -1,   149,    55,
     180,    -1,   152,    16,    -1,   152,    12,    -1,   151,    -1,
     150,    54,   151,    -1,   187,    -1,   153,    -1,    45,   153,
      -1,    46,   153,    -1,    47,   153,    -1,    51,   153,    -1,
      52,   153,    -1,    52,    51,   153,    -1,    51,    52,   153,
      -1,   154,    -1,   154,   157,    -1,   154,    72,    -1,   154,
     157,    72,    -1,    18,    -1,    19,    -1,   155,    22,    -1,
      18,    94,    -1,    19,    94,    -1,    18,    -1,    19,    -1,
      83,    -1,   157,    83,    -1,   159,    -1,   171,    -1,   160,
      -1,   167,    -1,   178,   104,    -1,   165,    -1,   169,    -1,
      30,   193,   104,    -1,    30,   193,   178,   104,    -1,    31,
     104,    -1,    32,   104,    -1,   163,    -1,   161,    -1,   175,
      -1,    28,   162,    67,    -1,    29,    67,    -1,    10,    -1,
      11,    -1,    12,    -1,   164,    92,   193,   178,    98,   175,
      -1,    27,    -1,   166,    92,   196,   178,   196,   197,    98,
     158,   197,    -1,   166,    92,   196,   178,   196,   194,   197,
      98,   158,   197,    -1,   166,    92,   194,   196,   178,   196,
     197,    98,   158,   197,    -1,   166,    92,   194,   196,   178,
     196,   194,   197,    98,   158,   197,    -1,    33,    -1,   168,
     158,    -1,    24,    -1,   170,   174,   104,   197,    -1,   170,
     174,   158,   197,    -1,    23,    -1,   172,   174,   159,   197,
      -1,   172,   174,   171,   197,    -1,   173,   159,   197,    -1,
     173,   171,   197,    -1,    25,    -1,    26,    -1,    92,   178,
      98,    -1,    53,    -1,    53,    -1,   100,    -1,   180,    -1,
     179,    -1,   184,    -1,   178,    55,   178,    -1,   178,    55,
      53,    -1,   178,    65,   178,    -1,   178,    64,   178,    -1,
     178,    63,   178,    -1,   178,    62,   178,    -1,   178,    61,
     178,    -1,   178,    60,   178,    -1,   178,    59,   178,    -1,
     178,    58,   178,    -1,   178,    57,   178,    -1,   178,    56,
     178,    -1,    34,   193,   156,    -1,    34,   193,   156,    92,
      98,    -1,    34,   193,   156,   119,    -1,    35,   193,   178,
      -1,    35,    93,    99,   193,   178,    -1,   178,    68,   178,
      -1,   178,    69,   178,    -1,    72,   178,    -1,    83,   178,
      -1,   178,    78,   178,    -1,   178,    77,   178,    -1,   178,
      76,   178,    -1,   178,    75,   178,    -1,   178,    74,   178,
      -1,   178,    73,   178,    -1,   178,    66,   178,    67,   178,
      -1,   178,    81,   178,    -1,   178,    82,   178,    -1,   178,
      83,   178,    -1,   178,    84,   178,    -1,   178,    85,   178,
      -1,   178,    80,   178,    -1,   178,    79,   178,    -1,   178,
      72,   178,    -1,   178,    71,   178,    -1,   178,    70,   178,
      -1,   178,    87,    -1,    87,   178,    -1,   178,    86,    -1,
      86,   178,    -1,    82,   178,    -1,    89,   178,    -1,   178,
      93,   178,    99,    -1,    92,   152,    98,   178,    -1,   152,
      92,   178,    98,    -1,   182,    -1,    92,   178,    98,    -1,
      10,    -1,    11,    -1,    12,    -1,    14,    -1,    36,    -1,
      37,    -1,    40,    -1,    41,    -1,    38,    -1,    42,    -1,
      13,    -1,   183,    98,    -1,   183,   191,    98,    -1,    13,
      92,    -1,   179,   188,   189,    -1,   180,   188,   189,    -1,
      91,   189,    -1,   155,   189,    -1,   185,    98,   197,    -1,
     185,   191,    98,   197,    -1,   179,   188,   189,    92,    -1,
     180,   188,   189,    92,    -1,   155,   189,    92,    -1,    16,
      -1,    16,    -1,    10,    -1,    11,    -1,    12,    -1,    14,
      -1,    13,    -1,    91,    -1,    95,    -1,    16,    -1,    11,
      -1,   190,    -1,    12,    -1,    14,    -1,    13,    -1,    19,
      -1,   181,    -1,   192,    -1,   191,    54,   192,    -1,   178,
      -1,    19,    -1,    -1,   195,    -1,   194,    54,   194,    -1,
     178,    -1,    97,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   163,   163,   164,   165,   166,   167,   170,   173,   174,
     175,   178,   181,   182,   185,   186,   187,   188,   189,   194,
     200,   202,   203,   206,   209,   210,   213,   216,   220,   233,
     238,   243,   248,   254,   260,   267,   274,   279,   284,   290,
     294,   296,   302,   309,   316,   323,   330,   337,   346,   347,
     348,   352,   353,   354,   355,   356,   357,   360,   370,   388,
     408,   424,   425,   429,   432,   439,   446,   453,   461,   462,
     465,   466,   467,   470,   487,   507,   511,   515,   521,   522,
     525,   532,   554,   560,   568,   569,   572,   582,   583,   586,
     587,   590,   591,   592,   595,   596,   599,   600,   601,   602,
     603,   604,   605,   608,   620,   636,   644,   645,   649,   653,
     659,   675,   692,   712,   733,   734,   735,   738,   744,   756,
     764,   782,   796,   809,   835,   856,   878,   879,   880,   881,
     884,   885,   888,   889,   895,   903,   913,   914,   921,   930,
     931,   932,   933,   935,   936,   937,   938,   941,   942,   948,
     950,   958,   959,   960,   971,   976,   980,   981,   984,   985,
     988,   989,   992,   993,   996,   997,   998,   999,  1000,  1001,
    1002,  1003,  1004,  1005,  1008,  1017,  1029,  1030,  1031,  1035,
    1060,  1064,  1071,  1078,  1085,  1092,  1098,  1101,  1106,  1117,
    1122,  1130,  1135,  1139,  1148,  1160,  1166,  1171,  1174,  1181,
    1184,  1193,  1194,  1198,  1199,  1200,  1210,  1211,  1212,  1213,
    1214,  1215,  1216,  1217,  1218,  1219,  1220,  1221,  1222,  1224,
    1225,  1226,  1227,  1228,  1229,  1230,  1231,  1232,  1233,  1234,
    1235,  1236,  1237,  1238,  1239,  1240,  1241,  1242,  1243,  1244,
    1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,  1253,  1254,
    1260,  1266,  1267,  1271,  1272,  1273,  1274,  1277,  1278,  1279,
    1280,  1281,  1282,  1286,  1287,  1288,  1303,  1319,  1320,  1328,
    1329,  1334,  1335,  1350,  1352,  1370,  1394,  1398,  1400,  1402,
    1404,  1408,  1412,  1418,  1419,  1422,  1425,  1426,  1431,  1432,
    1433,  1434,  1435,  1438,  1439,  1442,  1443,  1446,  1449,  1450,
    1453,  1456,  1459
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
  "CSS_DELETE", "CSS_COMMAND", "CSS_LIST", "CSS_EXIT", "CSS_ALIAS",
  "CSS_REMOVE", "CSS_STATUS", "CSS_TYPECMD", "CSS_CONT", "CSS_HELP",
  "CSS_EXTERN", "CSS_STATIC", "CSS_CONST", "CSS_PRIVATE", "CSS_PUBLIC",
  "CSS_PROTECTED", "CSS_VIRTUAL", "CSS_INLINE", "'{'", "','", "'='",
  "CSS_ASGN_OR", "CSS_ASGN_XOR", "CSS_ASGN_AND", "CSS_ASGN_RSHIFT",
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   123,    44,    61,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,    63,    58,   318,   319,
     124,    94,    38,   320,   321,   322,   323,   324,   325,   326,
     327,    43,    45,    42,    47,    37,   328,   329,   330,   331,
     332,    46,    40,    91,   333,   334,    10,    59,    41,    93,
     125,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   102,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   104,   105,   105,   106,   106,   106,   106,   106,   107,
     108,   108,   108,   109,   109,   109,   109,   110,   111,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   113,   113,
     113,   114,   114,   114,   114,   114,   114,   115,   116,   116,
     117,   118,   118,   119,   120,   121,   121,   121,   122,   122,
     123,   123,   123,   124,   124,   125,   125,   125,   126,   126,
     127,   127,   128,   128,   129,   129,   130,   131,   131,   132,
     132,   133,   133,   133,   134,   134,   135,   135,   135,   135,
     135,   135,   135,   136,   136,   137,   138,   138,   139,   140,
     140,   140,   140,   141,   142,   142,   142,   143,   143,   143,
     143,   143,   144,   145,   145,   145,   146,   146,   146,   146,
     147,   147,   148,   148,   149,   149,   150,   150,   151,   152,
     152,   152,   152,   152,   152,   152,   152,   153,   153,   153,
     153,   154,   154,   154,   155,   155,   156,   156,   157,   157,
     158,   158,   159,   159,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   161,   161,   162,   162,   162,   163,
     164,   165,   165,   165,   165,   166,   167,   168,   169,   169,
     170,   171,   171,   171,   171,   172,   173,   174,   175,   176,
     177,   178,   178,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   180,   180,   180,   180,   181,   181,   181,
     181,   181,   181,   182,   182,   182,   183,   184,   184,   184,
     184,   184,   184,   185,   185,   185,   186,   187,   187,   187,
     187,   187,   187,   188,   188,   189,   189,   189,   190,   190,
     190,   190,   190,   191,   191,   192,   192,   193,   194,   194,
     195,   196,   197
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     3,     3,     2,     2,     2,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     1,     2,
       2,     3,     1,     1,     3,     5,     2,     3,     1,     3,
       3,     1,     2,     2,     2,     2,     2,     2,     0,     2,
       4,     1,     1,     1,     1,     1,     1,     4,     7,     6,
       6,     1,     2,     3,     6,     2,     2,     1,     1,     2,
       1,     2,     1,     1,     3,     8,     7,     4,     1,     3,
       1,     1,     1,     1,     1,     3,     1,     1,     2,     1,
       2,     1,     1,     1,     1,     2,     1,     1,     2,     2,
       2,     2,     1,     4,     6,     0,     1,     2,     3,     4,
       4,     5,     6,     2,     0,     1,     2,     4,     4,     4,
       5,     4,     2,     3,     2,     3,     2,     3,     3,     4,
       1,     3,     1,     3,     2,     2,     1,     3,     1,     1,
       2,     2,     2,     2,     2,     3,     3,     1,     2,     2,
       3,     1,     1,     2,     2,     2,     1,     1,     1,     2,
       1,     1,     1,     1,     2,     1,     1,     3,     4,     2,
       2,     1,     1,     1,     3,     2,     1,     1,     1,     6,
       1,     9,    10,    10,    11,     1,     2,     1,     4,     4,
       1,     4,     4,     3,     3,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     5,     4,     3,
       5,     3,     3,     2,     2,     3,     3,     3,     3,     3,
       3,     5,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     2,     2,     2,     4,     4,
       4,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     2,     3,     3,     2,
       2,     3,     4,     4,     4,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     0,     1,     3,
       1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,    27,
      28,   253,   254,   255,   263,   256,   276,   151,   152,     0,
      67,   190,   187,   195,   196,   180,     0,     0,   297,     0,
       0,   185,   297,   297,   297,   297,     0,     0,   297,    38,
     297,    32,    41,     0,     0,     0,     0,     0,   198,     0,
       0,     0,     0,     0,     0,     0,     0,     3,   200,     0,
      14,    15,    16,    17,    18,     0,     7,    51,    52,    53,
      54,     0,    55,    56,     0,     0,     0,   139,   147,     0,
       8,   160,   162,   172,   171,     0,   165,     0,   163,     0,
     166,     0,   161,     0,     0,   173,     6,     0,   202,   201,
     251,     0,   203,     0,     0,    13,    12,     9,    19,    21,
      20,    22,    24,    23,    25,    26,   266,   154,   155,   278,
     279,   280,   282,   281,   277,    81,     0,    78,   302,    80,
      66,    65,   176,   177,   178,     0,   175,     0,    11,   169,
     170,     0,     0,     0,   297,    29,     0,   297,     0,    10,
     257,   258,   261,   259,   260,   262,     0,    36,     0,    30,
       0,    43,    47,    46,    44,    45,    42,   140,     0,   141,
     142,     0,   143,     0,   144,     0,     0,   223,   246,   224,
     245,   243,   247,   286,   288,   290,   289,   285,   291,   292,
     269,   287,     0,     0,     4,     5,     0,     0,   302,   302,
     282,     0,     0,     0,   302,   136,     0,   138,   149,   158,
     148,   124,   153,     0,   270,   297,     0,   186,     0,     0,
       0,   302,   302,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   244,   242,     0,   164,   283,   284,     0,
       0,   296,   264,   295,     0,   293,   302,     0,   297,   114,
       0,     0,   174,   167,     0,   156,   157,   216,   297,   219,
       0,    49,     0,    34,    40,    39,   151,   151,   146,   145,
       0,   252,    72,     0,    68,    70,    73,   126,     0,   130,
     132,     0,     0,     0,   302,     0,     0,     0,   123,   297,
       0,   150,   159,   125,   275,     0,   301,   300,     0,   298,
       0,     0,   302,   302,   302,   302,   193,   194,   205,   204,
     215,   214,   213,   212,   211,   210,   209,   208,   207,   206,
       0,   221,   222,   241,   240,   239,   230,   229,   228,   227,
     226,   225,   238,   237,   232,   233,   234,   235,   236,     0,
     267,   268,     0,   265,   271,   302,     0,   115,     0,    93,
      91,    92,    79,    84,    86,     0,    89,    87,    77,   168,
       0,     0,   218,     0,     0,     0,   249,   302,    69,    71,
       0,   128,     0,   127,     0,   135,   134,   117,   118,   199,
     121,   119,     0,   250,   137,   138,    57,     0,   302,    61,
       0,     0,     0,     0,   197,   188,   189,   191,   192,     0,
     248,   273,   274,   294,   272,   302,   102,   116,   151,     0,
       0,     0,     0,   302,     0,   114,     0,    94,    96,    97,
       0,     0,     0,    90,    88,   217,     0,   220,    50,    35,
       0,    74,   131,   129,   133,   120,   302,    62,     0,     0,
     299,     0,   302,   231,     0,     0,   100,    99,   101,     0,
       0,     0,    98,   302,    95,     0,   105,    85,    63,    64,
       0,    60,   179,   302,   302,     0,    59,   114,     0,    76,
       0,     0,   114,   114,     0,    58,   302,     0,     0,     0,
     110,     0,   114,    75,   109,   103,     0,     0,   106,     0,
       0,     0,   302,   114,   111,     0,   114,   107,     0,   302,
     302,   181,   112,   108,   104,   302,   183,   182,   184
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,   139,   107,    59,    60,    61,    62,    63,    64,
      65,   145,    66,    67,    68,    69,   408,   382,   435,    71,
     293,   294,   295,    72,   126,   127,   128,   372,   373,   374,
     375,   376,   436,   437,   438,   494,   507,   508,   439,   440,
     368,    73,    74,    75,   198,   298,   299,   300,   204,   205,
     175,    77,    78,   176,   277,   210,    80,    81,    82,    83,
     135,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,   219,    95,   400,    96,    97,    98,    99,   189,
     100,   101,   102,   103,   104,   296,   259,   214,   191,   281,
     265,   137,   318,   319,   320,   271
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -376
static const short int yypact[] =
{
    -376,   562,  -376,   198,    52,   180,    95,   288,   291,  -376,
    -376,  -376,  -376,  -376,    49,  -376,  -376,    33,    56,   514,
     716,  -376,  -376,  -376,  -376,  -376,   299,    92,  -376,    71,
      71,  -376,  -376,    84,    29,    70,   198,   227,    29,  -376,
      29,  -376,   371,   296,   296,   296,    99,    20,  -376,  1106,
    1106,  1106,  1106,  1106,  1106,   670,  1106,  -376,  -376,   198,
    -376,  -376,  -376,  -376,  -376,   198,  -376,  -376,  -376,  -376,
    -376,   135,  -376,  -376,   137,   137,    72,  -376,   -41,   478,
    -376,  -376,  -376,  -376,  -376,   144,  -376,   153,  -376,  1006,
    -376,   162,  -376,   162,  1006,  -376,  -376,  1560,    74,    74,
    -376,   653,  -376,   703,   202,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,   165,   218,   208,  -376,   184,
    -376,  -376,  -376,  -376,  -376,   222,  -376,   852,  -376,  -376,
    -376,   303,   213,  1106,  -376,  -376,  1156,  -376,  1156,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,   468,  -376,  1206,  -376,
    1256,  -376,  -376,  -376,  -376,  -376,  -376,  -376,   295,  -376,
    -376,   296,  -376,   296,  -376,   231,   619,    18,   241,    18,
      18,    18,   241,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,   -58,  1384,  -376,  -376,   272,     4,  -376,  -376,
     137,    33,    56,  1106,   281,  -376,   613,    68,  -376,  -376,
     -31,  -376,  -376,   318,   245,  -376,   918,  -376,  1106,   786,
    1006,  -376,  -376,  1056,  1106,  1106,  1106,  1106,  1106,  1106,
    1106,  1106,  1106,  1106,  1106,  1106,  1106,  1106,  1106,  1106,
    1106,  1106,  1106,  1106,  1106,  1106,  1106,  1106,  1106,  1106,
    1106,  1106,  1106,  -376,  -376,  1106,  -376,  -376,  -376,   670,
     670,    37,  -376,  1685,   -37,  -376,  -376,   -17,  -376,   323,
     127,    71,  -376,  -376,  1560,  -376,  -376,   240,  -376,    18,
    1156,   287,  1156,   287,  -376,  -376,   136,   182,  -376,  -376,
    1106,  -376,  -376,   142,  -376,   290,   300,   298,     8,  -376,
     305,   116,    17,   297,    71,  1428,   613,    71,  -376,  -376,
     258,  -376,  -376,  -376,  -376,  1106,  -376,  1685,    15,  -376,
    1106,  1472,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  1685,
    1685,  1685,  1685,  1685,  1685,  1685,  1685,  1685,  1685,  1685,
    1646,   687,   901,   834,   768,   463,   743,   743,   743,   743,
     743,   743,   535,   535,   166,   166,    18,    18,    18,  1294,
     262,   264,  1156,  -376,  -376,  -376,  1106,  -376,    47,  -376,
    -376,  -376,   311,  -376,  -376,   127,  -376,  -376,  -376,  -376,
     268,  1106,  -376,  1106,    35,    46,    18,  -376,  -376,  -376,
     357,  -376,   301,   324,   171,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,   319,  -376,  -376,  -376,  -376,  1106,   258,  -376,
    1516,  1106,  1106,  1603,  -376,  -376,  -376,  -376,  -376,  1106,
    -376,  -376,  -376,  -376,  -376,  1685,  -376,  -376,   154,   306,
     307,   309,     2,  -376,   359,   323,   279,  -376,  -376,  -376,
     137,   613,   127,  -376,  -376,  -376,  1339,    18,  -376,  -376,
      71,  -376,  -376,  -376,  -376,  -376,  1685,  -376,    71,   319,
    -376,  1603,  1106,  1711,    71,    71,  -376,  -376,  -376,   360,
      71,   137,   364,  -376,  -376,    71,   -39,  -376,  -376,  -376,
      71,  -376,  -376,  1106,   328,   293,  -376,   323,   137,  -376,
      71,    71,   323,   323,   292,  -376,   328,   302,   304,  1006,
     364,    71,   323,  -376,   364,   364,   376,    77,  -376,   316,
    1006,  1006,  -376,   323,   364,   317,   323,  -376,  1006,  -376,
    -376,  -376,   364,  -376,   364,  -376,  -376,  -376,  -376
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -376,  -376,   -30,   121,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,   261,  -376,  -376,  -376,  -376,  -376,  -292,   393,  -376,
    -376,   104,  -376,  -376,  -376,  -376,  -376,  -376,   -44,  -376,
    -376,    24,  -376,   -33,  -376,  -376,  -376,  -102,  -376,  -376,
    -200,  -376,  -376,  -376,   -70,  -376,    25,  -376,  -376,   113,
       1,   -36,  -376,     0,  -376,  -376,   -56,   -81,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,   -78,
    -376,  -376,   329,  -287,  -376,  -376,   -24,  -376,    27,   -18,
    -376,  -376,  -376,  -376,  -376,   -16,   325,   -20,  -376,   -89,
      61,    40,  -375,  -376,  -312,    39
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -298
static const short int yytable[] =
{
     140,    79,    76,   129,   131,   199,   412,   167,   169,   170,
     172,   174,   264,   221,   267,   398,   222,   362,   409,   156,
      17,    18,    17,    18,   166,   177,   178,   179,   180,   181,
     182,   208,   193,   217,   203,   190,   460,   362,    17,    18,
     290,   311,   209,   168,   168,   168,   168,   168,   426,    43,
      44,    45,   312,  -113,   171,    46,    47,   192,   138,   283,
     207,   363,   392,   108,   427,   428,    18,   256,    20,   411,
      48,   173,   141,   143,   146,   148,   206,   263,   158,   263,
     160,   365,   119,   120,   121,   200,   123,   484,   124,   362,
     201,   202,    43,    44,    45,   429,   430,   431,   432,    47,
     362,   462,   297,   469,   253,   254,   393,   273,   496,  -152,
     111,   255,   316,   274,   138,   455,   457,    17,    18,   279,
    -152,   144,   263,   309,   263,   -48,   -48,   117,   395,  -152,
     304,   118,   396,   448,   263,   288,   263,   289,   284,   324,
     285,   116,   325,   292,   449,    17,    18,   433,   434,   483,
     118,   171,   119,   120,   121,   122,   123,   149,   124,   136,
    -122,  -297,   147,   323,   203,   257,   -33,   -33,   138,   258,
     506,   168,   482,   168,   138,   369,   370,   142,   371,   305,
     194,    11,    12,    13,   280,    15,   195,   282,   196,   322,
     308,   384,   317,   385,   321,   109,   110,   168,   301,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,   352,   353,   354,   355,   356,   357,   358,   197,
     117,   359,   -37,   -37,   377,   472,   215,   302,   303,   360,
     361,   378,   387,   307,   379,   216,   197,   310,   117,   250,
     251,   252,   253,   254,   218,   315,   263,   268,   263,   255,
     326,   327,   -83,   150,   151,   152,   386,   153,   154,   155,
     168,   269,   397,   292,   401,   270,   117,   406,   -31,   -31,
     426,   -82,   119,   120,   121,   122,   123,   500,   124,   272,
     405,   410,   504,   505,   105,   106,   413,   428,    18,   157,
      20,   159,   514,   112,   113,   364,   114,   115,   366,   132,
     133,   134,   278,   522,    17,    18,   524,   212,   383,    17,
      18,   275,   276,   203,    43,    44,    45,   429,   430,   431,
     432,    47,   380,   381,   255,   306,   313,   314,   263,   444,
     367,   362,   425,   402,   389,   391,    43,    44,    45,   407,
     399,   381,    46,    47,   421,   390,   422,   446,   465,   447,
     394,   415,   416,   417,   418,   442,   445,   451,   168,   441,
     475,   453,    48,   466,   467,   168,   468,   471,   488,   473,
     434,   427,   411,   456,   161,   506,   515,   317,   461,   162,
     163,   499,   168,   301,    70,   463,   172,   388,   477,   443,
     510,   490,   511,   474,   424,   517,   377,   150,   151,   152,
     164,   153,   154,   155,   518,   165,   523,   452,   501,   404,
     479,   454,   220,   423,   260,   476,   450,     0,   481,     0,
       0,     0,   168,     0,   486,   487,   168,   441,   317,     0,
     489,     0,   168,   512,     0,   492,   493,   458,     0,     0,
     495,     0,     0,     0,   519,   520,     0,     0,     0,   317,
     502,   503,   525,     0,   464,     0,     0,     0,     0,     0,
       0,   513,   470,     0,     0,     0,     0,   516,   119,   120,
     121,   122,   123,     0,   124,     0,     0,     0,     0,   183,
     184,   185,   186,     0,   187,   480,   211,   188,     0,     0,
     212,   485,     0,     0,   150,   151,   152,     0,   153,   154,
     155,     0,   491,     0,   150,   151,   152,     0,   153,   154,
     155,     0,   497,   498,   119,   120,   121,   122,   123,     0,
     124,     0,   125,     0,     0,   509,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,   521,     0,     0,     0,     0,   255,     0,   526,   527,
       0,     0,     2,     3,   528,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,     0,    16,   213,
      17,    18,    19,    20,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
       0,     0,     0,    46,    47,    48,   248,   249,   250,   251,
     252,   253,   254,   119,   120,   121,   122,   123,   255,   124,
     183,   184,   185,   186,    49,   187,     0,     0,   188,     0,
       0,   212,     0,     0,    50,    51,     0,     0,    52,    53,
       0,    54,     0,    55,    56,   150,   151,   152,    57,   153,
     154,   155,    58,    11,    12,    13,    14,    15,     0,     0,
       0,    17,   261,     0,     0,     0,     0,     0,     0,     0,
       0,   183,   184,   185,   186,     0,   187,    32,    33,   188,
       0,     0,     0,     0,     0,     0,     0,     0,    43,    44,
      45,     0,     0,     0,    46,    47,   150,   151,   152,     0,
     153,   154,   155,    11,    12,    13,    14,    15,     0,     0,
       0,    17,   261,     0,     0,    49,   119,   120,   121,   122,
     123,     0,   124,     0,   130,    50,    51,    32,    33,    52,
      53,     0,    54,     0,    55,    56,     0,     0,    43,    44,
      45,   262,     0,     0,    46,    47,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,    49,     0,     0,     0,     0,
     255,     0,     0,     0,     0,    50,    51,     0,     0,    52,
      53,     0,    54,     0,    55,    56,    11,    12,    13,    14,
      15,   266,     0,     0,    17,    18,     0,     0,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,   246,   247,   248,   249,   250,   251,   252,   253,
     254,    43,    44,    45,     0,     0,   255,    46,    47,    48,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,     0,     0,    49,     0,
       0,   255,    11,    12,    13,    14,    15,     0,    50,    51,
      17,    18,    52,    53,     0,    54,     0,    55,    56,     0,
       0,     0,     0,   138,     0,     0,    32,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,    44,    45,
       0,     0,     0,    46,    47,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,     0,     0,    49,     0,     0,   255,    11,    12,
      13,    14,    15,     0,    50,    51,    17,    18,    52,    53,
       0,    54,     0,    55,    56,     0,     0,     0,     0,   138,
       0,     0,    32,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    44,    45,     0,     0,     0,    46,
      47,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,     0,
      49,     0,     0,     0,   255,     0,     0,     0,     0,     0,
      50,    51,     0,     0,    52,    53,     0,    54,     0,    55,
      56,     0,     0,     0,     0,   316,    11,    12,    13,    14,
      15,     0,     0,     0,    17,    18,     0,     0,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,     0,     0,     0,    46,    47,    48,
       0,     0,     0,     0,     0,     0,    11,    12,    13,    14,
      15,     0,     0,     0,    17,    18,     0,     0,    49,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    50,    51,
      32,    33,    52,    53,     0,    54,     0,    55,    56,     0,
       0,    43,    44,    45,     0,     0,     0,    46,    47,   328,
       0,     0,     0,     0,     0,     0,    11,    12,    13,    14,
      15,     0,     0,     0,    17,    18,     0,     0,    49,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    50,    51,
      32,    33,    52,    53,     0,    54,     0,    55,    56,     0,
       0,    43,    44,    45,     0,     0,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,    11,    12,    13,    14,
      15,     0,     0,     0,    17,   261,     0,     0,    49,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    50,    51,
      32,    33,    52,    53,     0,    54,     0,    55,    56,     0,
       0,    43,    44,    45,     0,     0,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,    11,    12,    13,    14,
      15,     0,     0,     0,   286,   261,     0,     0,    49,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    50,    51,
      32,    33,    52,    53,     0,    54,     0,    55,    56,     0,
       0,    43,    44,    45,     0,     0,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,    11,    12,    13,    14,
      15,     0,     0,     0,   287,   261,     0,     0,    49,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    50,    51,
      32,    33,    52,    53,     0,    54,     0,    55,    56,     0,
       0,    43,    44,    45,     0,     0,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    49,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    50,    51,
       0,     0,    52,    53,     0,    54,     0,    55,    56,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,     0,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,     0,     0,     0,     0,     0,   255,     0,     0,
       0,     0,     0,   420,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,     0,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   254,     0,     0,     0,
       0,     0,   255,     0,     0,     0,     0,     0,   478,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,     0,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,     0,     0,     0,     0,     0,   255,     0,     0,
       0,     0,   291,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,     0,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,     0,     0,     0,     0,
       0,   255,     0,     0,     0,     0,   403,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,     0,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
       0,     0,     0,     0,     0,   255,     0,     0,     0,     0,
     414,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,     0,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,     0,     0,     0,     0,     0,   255,
       0,     0,     0,     0,   459,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,     0,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,     0,     0,
       0,     0,     0,   255,     0,     0,     0,   138,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
       0,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,     0,     0,     0,     0,     0,   255,     0,     0,     0,
     316,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   419,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,     0,     0,     0,     0,     0,   255,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,     0,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,     0,     0,     0,     0,   234,   255,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,     0,
       0,     0,     0,     0,   255
};

static const short int yycheck[] =
{
      30,     1,     1,    19,    20,    75,   318,    43,    44,    45,
      46,    47,   101,    94,   103,   302,    94,    54,   310,    37,
      18,    19,    18,    19,    42,    49,    50,    51,    52,    53,
      54,    72,    56,    89,    92,    55,   411,    54,    18,    19,
      98,    72,    83,    43,    44,    45,    46,    47,     1,    45,
      46,    47,    83,    92,    52,    51,    52,    56,    97,   148,
      76,    98,    54,    11,    17,    18,    19,    97,    21,    54,
      53,    51,    32,    33,    34,    35,    76,   101,    38,   103,
      40,    98,    10,    11,    12,    13,    14,   462,    16,    54,
      18,    19,    45,    46,    47,    48,    49,    50,    51,    52,
      54,   413,    98,   101,    86,    87,    98,   137,   483,    72,
      15,    93,    97,   137,    97,   402,   408,    18,    19,   143,
      83,    92,   146,    55,   148,    96,    97,    94,    12,    92,
     200,    94,    16,    98,   158,   171,   160,   173,   156,   220,
     156,    92,   220,     1,    98,    18,    19,   100,   101,   461,
      94,    52,    10,    11,    12,    13,    14,    36,    16,    67,
      92,    93,    92,   219,    92,    91,    96,    97,    97,    95,
      93,   171,   459,   173,    97,    48,    49,    93,    51,   203,
      59,    10,    11,    12,   144,    14,    65,   147,    53,   219,
     206,   280,   216,   282,   218,    15,    16,   197,   197,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,    92,
      94,   255,    96,    97,   270,   435,    92,   198,   199,   259,
     260,   271,   100,   204,   274,    92,    92,   207,    94,    83,
      84,    85,    86,    87,    92,   215,   280,    55,   282,    93,
     221,   222,    97,    36,    37,    38,   290,    40,    41,    42,
     270,    53,   302,     1,   304,    67,    94,   307,    96,    97,
       1,    97,    10,    11,    12,    13,    14,   487,    16,    67,
     306,   315,   492,   493,    96,    97,   320,    18,    19,    38,
      21,    40,   502,    15,    16,   266,    15,    16,   268,    10,
      11,    12,    99,   513,    18,    19,   516,    22,   278,    18,
      19,    18,    19,    92,    45,    46,    47,    48,    49,    50,
      51,    52,    92,    93,    93,    54,    18,    92,   362,   375,
      17,    54,   366,   304,    54,    47,    45,    46,    47,   309,
      53,    93,    51,    52,    92,    55,    92,   381,   428,   383,
      55,   322,   323,   324,   325,    54,    98,    10,   368,   368,
     440,    47,    53,    67,    67,   375,    67,    18,    18,   100,
     101,    17,    54,   407,    13,    93,    10,   411,   412,    18,
      19,    98,   392,   392,     1,   419,   432,   293,   442,   375,
      98,   471,    98,   436,   365,   507,   442,    36,    37,    38,
      39,    40,    41,    42,    98,    44,    99,   392,   488,   306,
     450,   394,    93,   362,    99,   441,   387,    -1,   458,    -1,
      -1,    -1,   432,    -1,   464,   465,   436,   436,   462,    -1,
     470,    -1,   442,   499,    -1,   475,   476,   408,    -1,    -1,
     480,    -1,    -1,    -1,   510,   511,    -1,    -1,    -1,   483,
     490,   491,   518,    -1,   425,    -1,    -1,    -1,    -1,    -1,
      -1,   501,   433,    -1,    -1,    -1,    -1,   507,    10,    11,
      12,    13,    14,    -1,    16,    -1,    -1,    -1,    -1,    11,
      12,    13,    14,    -1,    16,   456,    18,    19,    -1,    -1,
      22,   462,    -1,    -1,    36,    37,    38,    -1,    40,    41,
      42,    -1,   473,    -1,    36,    37,    38,    -1,    40,    41,
      42,    -1,   483,   484,    10,    11,    12,    13,    14,    -1,
      16,    -1,    18,    -1,    -1,   496,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,   512,    -1,    -1,    -1,    -1,    93,    -1,   519,   520,
      -1,    -1,     0,     1,   525,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    16,   101,
      18,    19,    20,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      -1,    -1,    -1,    51,    52,    53,    81,    82,    83,    84,
      85,    86,    87,    10,    11,    12,    13,    14,    93,    16,
      11,    12,    13,    14,    72,    16,    -1,    -1,    19,    -1,
      -1,    22,    -1,    -1,    82,    83,    -1,    -1,    86,    87,
      -1,    89,    -1,    91,    92,    36,    37,    38,    96,    40,
      41,    42,   100,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    11,    12,    13,    14,    -1,    16,    34,    35,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    46,
      47,    -1,    -1,    -1,    51,    52,    36,    37,    38,    -1,
      40,    41,    42,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    -1,    -1,    72,    10,    11,    12,    13,
      14,    -1,    16,    -1,    18,    82,    83,    34,    35,    86,
      87,    -1,    89,    -1,    91,    92,    -1,    -1,    45,    46,
      47,    98,    -1,    -1,    51,    52,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    72,    -1,    -1,    -1,    -1,
      93,    -1,    -1,    -1,    -1,    82,    83,    -1,    -1,    86,
      87,    -1,    89,    -1,    91,    92,    10,    11,    12,    13,
      14,    98,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    45,    46,    47,    -1,    -1,    93,    51,    52,    53,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    -1,    -1,    72,    -1,
      -1,    93,    10,    11,    12,    13,    14,    -1,    82,    83,
      18,    19,    86,    87,    -1,    89,    -1,    91,    92,    -1,
      -1,    -1,    -1,    97,    -1,    -1,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,
      -1,    -1,    -1,    51,    52,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    -1,    -1,    72,    -1,    -1,    93,    10,    11,
      12,    13,    14,    -1,    82,    83,    18,    19,    86,    87,
      -1,    89,    -1,    91,    92,    -1,    -1,    -1,    -1,    97,
      -1,    -1,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    46,    47,    -1,    -1,    -1,    51,
      52,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    -1,
      72,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,
      82,    83,    -1,    -1,    86,    87,    -1,    89,    -1,    91,
      92,    -1,    -1,    -1,    -1,    97,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    -1,    -1,    -1,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    18,    19,    -1,    -1,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,
      34,    35,    86,    87,    -1,    89,    -1,    91,    92,    -1,
      -1,    45,    46,    47,    -1,    -1,    -1,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    18,    19,    -1,    -1,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,
      34,    35,    86,    87,    -1,    89,    -1,    91,    92,    -1,
      -1,    45,    46,    47,    -1,    -1,    -1,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    18,    19,    -1,    -1,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,
      34,    35,    86,    87,    -1,    89,    -1,    91,    92,    -1,
      -1,    45,    46,    47,    -1,    -1,    -1,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    18,    19,    -1,    -1,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,
      34,    35,    86,    87,    -1,    89,    -1,    91,    92,    -1,
      -1,    45,    46,    47,    -1,    -1,    -1,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    18,    19,    -1,    -1,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,
      34,    35,    86,    87,    -1,    89,    -1,    91,    92,    -1,
      -1,    45,    46,    47,    -1,    -1,    -1,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,
      -1,    -1,    86,    87,    -1,    89,    -1,    91,    92,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    -1,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,
      -1,    -1,    -1,    99,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    -1,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    -1,    -1,    -1,
      -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,    99,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    -1,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,
      -1,    -1,    98,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    -1,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    -1,    -1,    -1,    -1,
      -1,    93,    -1,    -1,    -1,    -1,    98,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,
      98,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    -1,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,    93,
      -1,    -1,    -1,    -1,    98,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    -1,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    -1,    -1,
      -1,    -1,    -1,    93,    -1,    -1,    -1,    97,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,
      97,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,    93,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    -1,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    -1,    -1,    -1,    -1,    66,    93,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    -1,
      -1,    -1,    -1,    -1,    93
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   103,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    16,    18,    19,    20,
      21,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    51,    52,    53,    72,
      82,    83,    86,    87,    89,    91,    92,    96,   100,   106,
     107,   108,   109,   110,   111,   112,   114,   115,   116,   117,
     120,   121,   125,   143,   144,   145,   152,   153,   154,   155,
     158,   159,   160,   161,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   175,   177,   178,   179,   180,
     182,   183,   184,   185,   186,    96,    97,   105,    11,    15,
      16,    15,    15,    16,    15,    16,    92,    94,    94,    10,
      11,    12,    13,    14,    16,    18,   126,   127,   128,   187,
      18,   187,    10,    11,    12,   162,    67,   193,    97,   104,
     104,   193,    93,   193,    92,   113,   193,    92,   193,   105,
      36,    37,    38,    40,    41,    42,   181,   113,   193,   113,
     193,    13,    18,    19,    39,    44,   181,   153,   155,   153,
     153,    52,   153,    51,   153,   152,   155,   178,   178,   178,
     178,   178,   178,    11,    12,    13,    14,    16,    19,   181,
     189,   190,   152,   178,   105,   105,    53,    92,   146,   146,
      13,    18,    19,    92,   150,   151,   155,   187,    72,    83,
     157,    18,    22,   101,   189,    92,    92,   158,    92,   174,
     174,   159,   171,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    93,   104,    91,    95,   188,
     188,    19,    98,   178,   191,   192,    98,   191,    55,    53,
      67,   197,    67,   104,   178,    18,    19,   156,    99,   178,
     193,   191,   193,   191,   181,   187,    18,    18,   153,   153,
      98,    98,     1,   122,   123,   124,   187,    98,   147,   148,
     149,   152,   197,   197,   146,   178,    54,   197,   187,    55,
     193,    72,    83,    18,    92,   193,    97,   178,   194,   195,
     196,   178,   104,   158,   159,   171,   197,   197,    53,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     189,   189,    54,    98,   197,    98,   193,    17,   142,    48,
      49,    51,   129,   130,   131,   132,   133,   153,   104,   104,
      92,    93,   119,   193,   191,   191,   178,   100,   123,    54,
      55,    47,    54,    98,    55,    12,    16,   104,   175,    53,
     176,   104,   197,    98,   151,   187,   104,   193,   118,   119,
     178,    54,   196,   178,    98,   197,   197,   197,   197,    67,
      99,    92,    92,   192,   197,   178,     1,    17,    18,    48,
      49,    50,    51,   100,   101,   120,   134,   135,   136,   140,
     141,   152,    54,   133,   153,    98,   178,   178,    98,    98,
     197,    10,   148,    47,   180,   175,   178,   119,   197,    98,
     194,   178,   196,   178,   197,   146,    67,    67,    67,   101,
     197,    18,   142,   100,   135,   146,   187,   130,    99,   104,
     197,   104,   175,   196,   194,   197,   104,   104,    18,   104,
     146,   197,   104,   104,   137,   104,   194,   197,   197,    98,
     142,   146,   104,   104,   142,   142,    93,   138,   139,   197,
      98,    98,   158,   104,   142,    10,   104,   139,    98,   158,
     158,   197,   142,    99,   142,   158,   197,   197,   197
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
#line 175 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Exit; }
    break;

  case 11:
#line 178 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 12:
#line 181 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 13:
#line 182 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 19:
#line 194 "css_parse.y"
    {
            cssEl* tmp = (yyvsp[0].el).El();
	    cssMisc::cur_top->SetInclude((const char*)*tmp);
	    cssEl::Done((yyvsp[0].el).El()); }
    break;

  case 20:
#line 200 "css_parse.y"
    {
            cssMisc::Defines.Push(new cssDef(0, (yyvsp[0].nm))); }
    break;

  case 22:
#line 203 "css_parse.y"
    { cssMisc::Defines.Remove((yyvsp[0].el).El()); }
    break;

  case 23:
#line 206 "css_parse.y"
    { /* if its a name, its not defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 25:
#line 210 "css_parse.y"
    { /* if its a def, its defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 27:
#line 216 "css_parse.y"
    { /* if you see it, it applies to u*/
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog()); }
    break;

  case 29:
#line 233 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
            Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 30:
#line 238 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 31:
#line 243 "css_parse.y"
    {
	    if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    Code2((yyvsp[0].el),(yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 32:
#line 248 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_NoSrc;
	    Code1((yyvsp[0].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 33:
#line 254 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	   cssMisc::cur_top->src_ln = cssMisc::cur_top->st_src_ln;
	   cssMisc::cur_top->List(); (yyval.ival) = cssProg::YY_NoSrc; }
    break;

  case 34:
#line 260 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_Ok;
	    cssMisc::cur_top->src_ln = cssMisc::cur_top->st_src_ln;
	    Code1((yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 35:
#line 267 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_Ok;
	    cssMisc::cur_top->src_ln = cssMisc::cur_top->st_src_ln;
	    Code1((yyvsp[-4].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 36:
#line 274 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 37:
#line 279 "css_parse.y"
    {
	    if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    Code2((yyvsp[0].el),(yyvsp[-2].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 38:
#line 284 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_NoSrc;
	    Code1((yyvsp[0].el)); }
    break;

  case 39:
#line 290 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssRef((yyvsp[-1].el)));
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 40:
#line 294 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc; }
    break;

  case 41:
#line 296 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_NoSrc;
	    ((yyvsp[0].el).El())->Do(cssMisc::cur_top->Prog());}
    break;

  case 42:
#line 302 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-1].el).El())->Do(cssMisc::cur_top->Prog());}
    break;

  case 43:
#line 309 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-1].el).El())->Do(cssMisc::cur_top->Prog());}
    break;

  case 44:
#line 316 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-1].el).El())->Do(cssMisc::cur_top->Prog());}
    break;

  case 45:
#line 323 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-1].el).El())->Do(cssMisc::cur_top->Prog());}
    break;

  case 46:
#line 330 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-1].el).El())->Do(cssMisc::cur_top->Prog());}
    break;

  case 47:
#line 337 "css_parse.y"
    {
            if(!cssMisc::cur_top->AmCmdProg()) {
	      yyerror("commands are only available from the shell");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-1].el).El())->Do(cssMisc::cur_top->Prog());}
    break;

  case 48:
#line 346 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 50:
#line 348 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 57:
#line 360 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival));
	    if((yyvsp[-2].ival) < 0) (yyval.ival) = (yyvsp[-1].ival)-1; /* if no coding, its the end, else not */
	    else (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 58:
#line 370 "css_parse.y"
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

  case 59:
#line 388 "css_parse.y"
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

  case 60:
#line 408 "css_parse.y"
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

  case 61:
#line 424 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 62:
#line 425 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 63:
#line 429 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 64:
#line 432 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
	    if(cssMisc::cur_enum->name.before(5) == "enum_") /* don't keep anon enums around */
	      delete cssMisc::cur_enum;
	    cssMisc::cur_enum = NULL; }
    break;

  case 65:
#line 439 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_enum = new cssEnumType((const char*)*nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum);
	    cssEl::Done(nm); }
    break;

  case 66:
#line 446 "css_parse.y"
    { /* redefining */
	    if((yyvsp[0].el).El()->GetType() != cssEl::T_EnumType) {
	      yyerror("Attempt to redefine non-enum type as an enum");
	      return cssProg::YY_Err; }
   	    cssMisc::cur_enum = (cssEnumType*)(yyvsp[0].el).El();
 	    cssMisc::cur_enum->enums->Reset(); /* prepare for new enums.. */
	    cssMisc::cur_enum->enum_cnt = 0; }
    break;

  case 67:
#line 453 "css_parse.y"
    { /* anonymous  */
	    String nm = "anonenum_"; nm += String(cssMisc::anon_type_cnt++); nm += "_";
            cssMisc::cur_enum = new cssEnumType((const char*)nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum); }
    break;

  case 72:
#line 467 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 73:
#line 470 "css_parse.y"
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

  case 74:
#line 487 "css_parse.y"
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

  case 75:
#line 507 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[-4].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 76:
#line 511 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[-3].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 77:
#line 515 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 80:
#line 525 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssMisc::cur_class = new cssClassType((const char*)*nm);
	    cssMisc::cur_class->last_top = cssMisc::cur_top;
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
	    cssEl::Done(nm); }
    break;

  case 81:
#line 532 "css_parse.y"
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

  case 82:
#line 554 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_class = new cssClassType((const char*)*nm);
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
            cssEl::Done(nm); }
    break;

  case 83:
#line 560 "css_parse.y"
    {
            if((yyvsp[0].el).El()->GetType() != cssEl::T_ClassType) {
              yyerror("Attempt to redefine non-class type as a class");
              return cssProg::YY_Err;
            }
            cssMisc::cur_class = (cssClassType*)(yyvsp[0].el).El(); }
    break;

  case 86:
#line 572 "css_parse.y"
    {
            if((yyvsp[0].el).El()->GetType() != cssEl::T_ClassType) {
	      yyerror("Cannot inherit from non-class type");
	      /* cssMisc::TypesSpace.Remove(cssMisc::cur_class); */
	      cssMisc::cur_top->types.Remove(cssMisc::cur_class);
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_class->AddParent((cssClassType*)((yyvsp[0].el).El())); }
    break;

  case 88:
#line 583 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); }
    break;

  case 97:
#line 600 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 98:
#line 601 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 102:
#line 605 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 103:
#line 608 "css_parse.y"
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

  case 104:
#line 620 "css_parse.y"
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

  case 105:
#line 636 "css_parse.y"
    {
            /* swap places with void marker */
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_top->Prog()->Stack()->Push(&cssMisc::Void);
            cssMisc::cur_top->Prog()->Stack()->Push(nm); }
    break;

  case 106:
#line 644 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 107:
#line 645 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 108:
#line 649 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((int)*(yyvsp[-1].el).El())); }
    break;

  case 109:
#line 653 "css_parse.y"
    {       /* pre-declare function */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[-3].el).El();
            cssMisc::cur_class->GetComments(fun, (yyvsp[0].el));
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr, true); }
    break;

  case 110:
#line 659 "css_parse.y"
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

  case 111:
#line 675 "css_parse.y"
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

  case 112:
#line 692 "css_parse.y"
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

  case 113:
#line 712 "css_parse.y"
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

  case 114:
#line 733 "css_parse.y"
    { (yyval.el).Reset(); }
    break;

  case 115:
#line 734 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); }
    break;

  case 116:
#line 735 "css_parse.y"
    { (yyval.el) = (yyvsp[-1].el); }
    break;

  case 117:
#line 738 "css_parse.y"
    {	/* pre-declare function */
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival));	(yyval.ival) = (yyvsp[-1].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[-3].el).El();
	    fun->argc = (yyvsp[-2].ival); fun->GetArgDefs(); }
    break;

  case 118:
#line 744 "css_parse.y"
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

  case 119:
#line 756 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 120:
#line 764 "css_parse.y"
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

  case 121:
#line 782 "css_parse.y"
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

  case 122:
#line 796 "css_parse.y"
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

  case 123:
#line 809 "css_parse.y"
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

  case 124:
#line 835 "css_parse.y"
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

  case 125:
#line 856 "css_parse.y"
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

  case 126:
#line 878 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 127:
#line 879 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 128:
#line 880 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 129:
#line 881 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 130:
#line 884 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 131:
#line 885 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival) + 1; }
    break;

  case 132:
#line 888 "css_parse.y"
    { cssMisc::parsing_args = false; }
    break;

  case 133:
#line 889 "css_parse.y"
    {
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push((yyvsp[0].el).El()); }
    break;

  case 134:
#line 895 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-1].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[0].nm)));
	    ((yyvsp[-1].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 135:
#line 903 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-1].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    ((yyvsp[-1].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 137:
#line 914 "css_parse.y"
    {
	    if(((yyvsp[-2].ival) < 0) && ((yyvsp[0].ival) < 0)) (yyval.ival) = -1;
	    else if(((yyvsp[-2].ival) >= 0) && ((yyvsp[0].ival) < 0)) (yyval.ival) = (yyvsp[-2].ival);
	    else if(((yyvsp[-2].ival) < 0) && ((yyvsp[0].ival) >= 0)) (yyval.ival) = (yyvsp[0].ival);
	    else (yyval.ival) = -1; }
    break;

  case 138:
#line 921 "css_parse.y"
    {
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      (yyval.ival) = Code2(tmp->ptr, cssBI::constr); }
	    else (yyval.ival) = -1;
	    cssEl::Done(tmp); }
    break;

  case 139:
#line 930 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = ""; }
    break;

  case 140:
#line 931 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "extern"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 141:
#line 932 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "static"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 142:
#line 933 "css_parse.y"
    { /* use const expr prog to code stuff */
	  (yyvsp[0].el).El()->tmp_str = "const"; (yyval.el) = (yyvsp[0].el); cssMisc::CodeConstExpr(); }
    break;

  case 143:
#line 935 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 144:
#line 936 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = ""; (yyval.el) = (yyvsp[0].el); }
    break;

  case 145:
#line 937 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 146:
#line 938 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 147:
#line 941 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); cssMisc::cur_type = (yyval.el); }
    break;

  case 148:
#line 942 "css_parse.y"
    {
	  if(((yyvsp[-1].el).El()->GetParse() != CSS_PTR) || ((yyvsp[0].ival) >= 2)) {
	    (yyval.el) = cssMisc::cur_top->AddPtrType((yyvsp[-1].el).El()); cssMisc::cur_type = (yyval.el);
	  }
	  else {
	    (yyval.el) = (yyvsp[-1].el); cssMisc::cur_type = (yyval.el); } }
    break;

  case 149:
#line 948 "css_parse.y"
    {
	  (yyval.el) = cssMisc::cur_top->AddRefType((yyvsp[-1].el).El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 150:
#line 950 "css_parse.y"
    {
	  if(((yyvsp[-2].el).El()->GetParse() != CSS_PTR) || ((yyvsp[-1].ival) >= 2)) {
            cssElPtr npt = cssMisc::cur_top->AddPtrType((yyvsp[-2].el).El());
            (yyval.el) = cssMisc::cur_top->AddRefType(npt.El()); cssMisc::cur_type = (yyval.el); }
	  else {
	    (yyval.el) = cssMisc::cur_top->AddRefType((yyvsp[-2].el).El()); cssMisc::cur_type = (yyval.el); } }
    break;

  case 153:
#line 960 "css_parse.y"
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

  case 154:
#line 971 "css_parse.y"
    {
           if((yyvsp[-1].el).El()->GetType() != cssEl::T_ClassType) {
	     yyerror("scoping of non-class type");
	     return cssProg::YY_Err; }
           cssMisc::cur_scope = (yyvsp[-1].el).El(); }
    break;

  case 155:
#line 976 "css_parse.y"
    {
           cssMisc::cur_scope = (yyvsp[-1].el).El(); }
    break;

  case 158:
#line 984 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 159:
#line 985 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 160:
#line 988 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastif = -1; }
    break;

  case 162:
#line 992 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastdo = -1; }
    break;

  case 164:
#line 996 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 167:
#line 999 "css_parse.y"
    { Code1((yyvsp[-2].el)); (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 168:
#line 1000 "css_parse.y"
    { Code1((yyvsp[-3].el)); (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 169:
#line 1001 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[-1].el)); }
    break;

  case 170:
#line 1002 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[-1].el)); }
    break;

  case 174:
#line 1008 "css_parse.y"
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

  case 175:
#line 1017 "css_parse.y"
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

  case 179:
#line 1035 "css_parse.y"
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

  case 180:
#line 1060 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 181:
#line 1064 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-8].ival) + 1]->SetLine((yyvsp[-5].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-8].ival) + 2]->SetLine((yyvsp[-3].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-8].ival) + 3]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-8].ival) + 4]->SetLine((yyvsp[0].ival)); }
    break;

  case 182:
#line 1071 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 1]->SetLine((yyvsp[-6].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 2]->SetLine((yyvsp[-4].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 3]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 4]->SetLine((yyvsp[0].ival)); }
    break;

  case 183:
#line 1078 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 1]->SetLine((yyvsp[-5].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 2]->SetLine((yyvsp[-4].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 3]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-9].ival) + 4]->SetLine((yyvsp[0].ival)); }
    break;

  case 184:
#line 1085 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-10].ival) + 1]->SetLine((yyvsp[-6].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-10].ival) + 2]->SetLine((yyvsp[-4].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-10].ival) + 3]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-10].ival) + 4]->SetLine((yyvsp[0].ival)); }
    break;

  case 185:
#line 1092 "css_parse.y"
    {
            (yyval.ival) = Code2((yyvsp[0].el),cssInst::Stop);
	    Code3(cssInst::Stop, cssInst::Stop, cssInst::Stop); }
    break;

  case 186:
#line 1098 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastdo = (yyvsp[-1].ival); }
    break;

  case 187:
#line 1101 "css_parse.y"
    { (yyval.ival) = Code3((yyvsp[0].el), cssInst::Stop, cssInst::Stop);
                                  cssMisc::cur_top->Prog()->lastdo = (yyval.ival); }
    break;

  case 188:
#line 1106 "css_parse.y"
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

  case 189:
#line 1117 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 1]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 2]->SetLine((yyvsp[0].ival)); }
    break;

  case 190:
#line 1122 "css_parse.y"
    {
            if(cssMisc::cur_top->Prog()->lastdo < 0) {
	      (yyval.ival) = Code3((yyvsp[0].el), cssInst::Stop, cssInst::Stop); }
            else { (yyval.ival) = cssMisc::cur_top->Prog()->lastdo;} }
    break;

  case 191:
#line 1130 "css_parse.y"
    {
            if(cssMisc::cur_top->debug > 3)
	      cerr << "\nvalue of then is: " << (yyvsp[-1].ival) << "\n";
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 1]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival)]->EndIf((yyvsp[0].ival)); }
    break;

  case 192:
#line 1135 "css_parse.y"
    {
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-3].ival) + 1]->SetLine((yyvsp[-1].ival));
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)]->previf = (yyvsp[-3].ival);
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)]->EndIf(); }
    break;

  case 193:
#line 1139 "css_parse.y"
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

  case 194:
#line 1148 "css_parse.y"
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

  case 195:
#line 1160 "css_parse.y"
    {
            (yyval.ival) = Code1((yyvsp[0].el)); Code3(cssInst::Stop, cssInst::Stop, cssInst::Stop);
	    cssMisc::cur_top->Prog()->ResetLasts();
	    cssMisc::cur_top->Prog()->lastif = (yyval.ival); }
    break;

  case 196:
#line 1166 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->elseif = cssMisc::cur_top->Prog()->lastif;
	    cssMisc::cur_top->Prog()->ResetLasts(); }
    break;

  case 197:
#line 1171 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 198:
#line 1174 "css_parse.y"
    {
            cssScriptFun* b = new cssScriptFun(cssBlock_Name);
	    cssMisc::cur_top->AddStatic(b);
	    (yyval.ival) = Code1(b);  cssMisc::cur_top->Prog()->insts[ (yyval.ival) ]->SetDefn();
	    cssMisc::cur_top->SetPush(b->fun); }
    break;

  case 199:
#line 1181 "css_parse.y"
    {  }
    break;

  case 200:
#line 1184 "css_parse.y"
    {
            (yyval.ival) = Code1(cssInst::Stop); cssMisc::cur_top->SetPop();
	    cssScriptFun* sfun = cssMisc::cur_top->Prog()->owner;
	    if((sfun != NULL) && (sfun->GetType() == cssEl::T_MbrScriptFun)) {
	      cssMisc::cur_class = NULL; /* get rid of current class pointer.. */
	      cssMisc::cur_method = NULL; } /* and current method pointer */
	  }
    break;

  case 201:
#line 1193 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 204:
#line 1199 "css_parse.y"
    { Code1(cssBI::asgn); }
    break;

  case 205:
#line 1200 "css_parse.y"
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

  case 206:
#line 1210 "css_parse.y"
    { Code1(cssBI::asgn_add); }
    break;

  case 207:
#line 1211 "css_parse.y"
    { Code1(cssBI::asgn_sub); }
    break;

  case 208:
#line 1212 "css_parse.y"
    { Code1(cssBI::asgn_mult); }
    break;

  case 209:
#line 1213 "css_parse.y"
    { Code1(cssBI::asgn_div); }
    break;

  case 210:
#line 1214 "css_parse.y"
    { Code1(cssBI::asgn_mod); }
    break;

  case 211:
#line 1215 "css_parse.y"
    { Code1(cssBI::asgn_lshift); }
    break;

  case 212:
#line 1216 "css_parse.y"
    { Code1(cssBI::asgn_rshift); }
    break;

  case 213:
#line 1217 "css_parse.y"
    { Code1(cssBI::asgn_and); }
    break;

  case 214:
#line 1218 "css_parse.y"
    { Code1(cssBI::asgn_xor); }
    break;

  case 215:
#line 1219 "css_parse.y"
    { Code1(cssBI::asgn_or); }
    break;

  case 216:
#line 1220 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code2((yyvsp[0].el),(yyvsp[-2].el)); }
    break;

  case 217:
#line 1221 "css_parse.y"
    { (yyval.ival) = (yyvsp[-3].ival); Code2((yyvsp[-2].el),(yyvsp[-4].el)); }
    break;

  case 218:
#line 1222 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[-2].ival); Code2((yyvsp[-1].el),(yyvsp[-3].el)); }
    break;

  case 219:
#line 1224 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code1((yyvsp[-2].el)); }
    break;

  case 220:
#line 1225 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code1((yyvsp[-4].el)); }
    break;

  case 221:
#line 1226 "css_parse.y"
    { Code1(cssBI::lor); }
    break;

  case 222:
#line 1227 "css_parse.y"
    { Code1(cssBI::land); }
    break;

  case 223:
#line 1228 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::addr_of); }
    break;

  case 224:
#line 1229 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::de_ptr); }
    break;

  case 225:
#line 1230 "css_parse.y"
    { Code1(cssBI::gt); }
    break;

  case 226:
#line 1231 "css_parse.y"
    { Code1(cssBI::ge); }
    break;

  case 227:
#line 1232 "css_parse.y"
    { Code1(cssBI::lt); }
    break;

  case 228:
#line 1233 "css_parse.y"
    { Code1(cssBI::le); }
    break;

  case 229:
#line 1234 "css_parse.y"
    { Code1(cssBI::eq); }
    break;

  case 230:
#line 1235 "css_parse.y"
    { Code1(cssBI::ne); }
    break;

  case 231:
#line 1236 "css_parse.y"
    { Code1(cssBI::cond); }
    break;

  case 232:
#line 1237 "css_parse.y"
    { Code1(cssBI::add); }
    break;

  case 233:
#line 1238 "css_parse.y"
    { Code1(cssBI::sub); }
    break;

  case 234:
#line 1239 "css_parse.y"
    { Code1(cssBI::mul); }
    break;

  case 235:
#line 1240 "css_parse.y"
    { Code1(cssBI::div); }
    break;

  case 236:
#line 1241 "css_parse.y"
    { Code1(cssBI::modulo); }
    break;

  case 237:
#line 1242 "css_parse.y"
    { Code1(cssBI::lshift); }
    break;

  case 238:
#line 1243 "css_parse.y"
    { Code1(cssBI::rshift); }
    break;

  case 239:
#line 1244 "css_parse.y"
    { Code1(cssBI::bit_and); }
    break;

  case 240:
#line 1245 "css_parse.y"
    { Code1(cssBI::bit_xor); }
    break;

  case 241:
#line 1246 "css_parse.y"
    { Code1(cssBI::bit_or); }
    break;

  case 242:
#line 1247 "css_parse.y"
    { Code1(cssBI::asgn_post_pp); }
    break;

  case 243:
#line 1248 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::asgn_pre_pp); }
    break;

  case 244:
#line 1249 "css_parse.y"
    { Code1(cssBI::asgn_post_mm); }
    break;

  case 245:
#line 1250 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::asgn_pre_mm); }
    break;

  case 246:
#line 1251 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::neg); }
    break;

  case 247:
#line 1252 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::lnot); }
    break;

  case 248:
#line 1253 "css_parse.y"
    { Code1(cssBI::de_array); }
    break;

  case 249:
#line 1254 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-2].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = (yyvsp[0].ival); Code2((yyvsp[-2].el), cssBI::cast); }
    break;

  case 250:
#line 1260 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
  	    (yyval.ival) = (yyvsp[-1].ival); Code2((yyvsp[-3].el), cssBI::cast); }
    break;

  case 252:
#line 1267 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 263:
#line 1286 "css_parse.y"
    { (yyval.ival) = Code2(cssBI::push_next, (yyvsp[0].el)); }
    break;

  case 264:
#line 1287 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].el_ival).ival; Code1((yyvsp[-1].el_ival).el); }
    break;

  case 265:
#line 1288 "css_parse.y"
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

  case 266:
#line 1303 "css_parse.y"
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

  case 267:
#line 1319 "css_parse.y"
    { Code2((yyvsp[0].el), cssBI::points_at);}
    break;

  case 268:
#line 1320 "css_parse.y"
    {
	    int mbno = (yyvsp[-2].el).El()->GetMemberNo((const char*)*((yyvsp[0].el).El()));
	    if(mbno < 0) { (yyval.ival) = Code3((yyvsp[-2].el), (yyvsp[0].el), cssBI::points_at); }
	    else { cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
		   (yyval.ival) = Code3((yyvsp[-2].el), tmpint, cssBI::points_at); } }
    break;

  case 269:
#line 1328 "css_parse.y"
    { (yyval.ival) = Code3(cssBI::push_root, (yyvsp[0].el), cssBI::points_at); }
    break;

  case 270:
#line 1329 "css_parse.y"
    {
	  cssMisc::cur_scope = NULL;
	    cssEl* scp = (yyvsp[-1].el).El()->GetScoped((const char*)*((yyvsp[0].el).El()));
	    if(scp != &cssMisc::Void) {  (yyval.ival) = Code1(scp); }
	    else { (yyval.ival) = Code3((yyvsp[-1].el), (yyvsp[0].el), cssBI::scoper); } }
    break;

  case 271:
#line 1334 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].el_ival).ival; }
    break;

  case 272:
#line 1335 "css_parse.y"
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

  case 273:
#line 1350 "css_parse.y"
    { Code2((yyvsp[-1].el), cssBI::member_fun);
                                                  (yyval.el_ival).ival = (yyvsp[-3].ival); (yyval.el_ival).el.Reset(); }
    break;

  case 274:
#line 1352 "css_parse.y"
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

  case 275:
#line 1370 "css_parse.y"
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

  case 276:
#line 1394 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].nm))); }
    break;

  case 277:
#line 1398 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].nm))); }
    break;

  case 278:
#line 1400 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName())); }
    break;

  case 279:
#line 1402 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName())); }
    break;

  case 280:
#line 1404 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 281:
#line 1408 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 282:
#line 1412 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding function:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 285:
#line 1422 "css_parse.y"
    {
   	    String tmpstr = String((yyvsp[0].nm));
            (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr); }
    break;

  case 287:
#line 1426 "css_parse.y"
    { /* this is source of shift-reduce problems */
	    String tmpstr = String((yyvsp[0].el).El()->GetName());
	    (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr); }
    break;

  case 293:
#line 1438 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 294:
#line 1439 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival) + 1; }
    break;

  case 296:
#line 1443 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 297:
#line 1446 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); }
    break;

  case 300:
#line 1453 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 301:
#line 1456 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 302:
#line 1459 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;


    }

/* Line 1037 of yacc.c.  */
#line 3859 "y.tab.c"

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


#line 1463 "css_parse.y"


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

