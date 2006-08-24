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
     CSS_HELP = 293,
     CSS_EXTERN = 294,
     CSS_STATIC = 295,
     CSS_CONST = 296,
     CSS_PRIVATE = 297,
     CSS_PUBLIC = 298,
     CSS_PROTECTED = 299,
     CSS_VIRTUAL = 300,
     CSS_INLINE = 301,
     CSS_ASGN_OR = 302,
     CSS_ASGN_XOR = 303,
     CSS_ASGN_AND = 304,
     CSS_ASGN_RSHIFT = 305,
     CSS_ASGN_LSHIFT = 306,
     CSS_ASGN_MOD = 307,
     CSS_ASGN_DIV = 308,
     CSS_ASGN_MULT = 309,
     CSS_ASGN_SUB = 310,
     CSS_ASGN_ADD = 311,
     CSS_OR = 312,
     CSS_AND = 313,
     CSS_NE = 314,
     CSS_EQ = 315,
     CSS_LE = 316,
     CSS_LT = 317,
     CSS_GE = 318,
     CSS_GT = 319,
     CSS_RSHIFT = 320,
     CSS_LSHIFT = 321,
     CSS_MINMIN = 322,
     CSS_PLUSPLUS = 323,
     CSS_UNARY = 324,
     CSS_NOT = 325,
     CSS_UNARYMINUS = 326,
     CSS_SCOPER = 327,
     CSS_POINTSAT = 328
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
#define CSS_HELP 293
#define CSS_EXTERN 294
#define CSS_STATIC 295
#define CSS_CONST 296
#define CSS_PRIVATE 297
#define CSS_PUBLIC 298
#define CSS_PROTECTED 299
#define CSS_VIRTUAL 300
#define CSS_INLINE 301
#define CSS_ASGN_OR 302
#define CSS_ASGN_XOR 303
#define CSS_ASGN_AND 304
#define CSS_ASGN_RSHIFT 305
#define CSS_ASGN_LSHIFT 306
#define CSS_ASGN_MOD 307
#define CSS_ASGN_DIV 308
#define CSS_ASGN_MULT 309
#define CSS_ASGN_SUB 310
#define CSS_ASGN_ADD 311
#define CSS_OR 312
#define CSS_AND 313
#define CSS_NE 314
#define CSS_EQ 315
#define CSS_LE 316
#define CSS_LT 317
#define CSS_GE 318
#define CSS_GT 319
#define CSS_RSHIFT 320
#define CSS_LSHIFT 321
#define CSS_MINMIN 322
#define CSS_PLUSPLUS 323
#define CSS_UNARY 324
#define CSS_NOT 325
#define CSS_UNARYMINUS 326
#define CSS_SCOPER 327
#define CSS_POINTSAT 328




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
#line 267 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 213 of yacc.c.  */
#line 279 "y.tab.c"

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
#define YYLAST   1618

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  104
/* YYNRULES -- Number of rules. */
#define YYNRULES  299
/* YYNRULES -- Number of states. */
#define YYNSTATES  500

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   328

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      90,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    79,    66,     2,
      86,    92,    77,    75,    48,    76,    85,    78,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    61,    91,
       2,    49,     2,    60,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    87,     2,    93,    65,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    47,    64,    94,    95,     2,     2,     2,
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
      45,    46,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    62,    63,    67,    68,    69,    70,    71,    72,
      73,    74,    80,    81,    82,    83,    84,    88,    89
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     7,    11,    15,    18,    21,    24,
      28,    30,    32,    34,    36,    38,    40,    42,    44,    47,
      50,    53,    56,    59,    62,    65,    68,    70,    72,    75,
      79,    83,    85,    89,    93,    97,   101,   105,   106,   109,
     114,   116,   119,   121,   123,   125,   127,   132,   139,   146,
     153,   155,   158,   162,   169,   172,   175,   177,   179,   182,
     184,   187,   189,   191,   195,   204,   212,   217,   219,   223,
     225,   227,   229,   231,   233,   237,   239,   241,   244,   246,
     249,   251,   253,   255,   257,   260,   262,   264,   267,   270,
     273,   276,   278,   283,   290,   291,   293,   296,   300,   305,
     310,   316,   323,   326,   327,   329,   332,   337,   342,   347,
     353,   358,   361,   365,   368,   372,   375,   379,   383,   388,
     390,   394,   396,   400,   403,   406,   408,   412,   414,   416,
     419,   422,   425,   428,   431,   435,   439,   441,   444,   447,
     451,   453,   455,   458,   461,   464,   466,   468,   470,   473,
     475,   477,   479,   481,   484,   486,   488,   492,   497,   500,
     503,   505,   507,   509,   513,   516,   518,   520,   522,   529,
     531,   538,   546,   548,   550,   553,   555,   557,   560,   562,
     564,   567,   569,   573,   577,   579,   583,   587,   590,   593,
     595,   597,   601,   603,   605,   607,   609,   611,   613,   615,
     617,   621,   625,   629,   633,   637,   641,   645,   649,   653,
     657,   661,   665,   669,   675,   680,   684,   690,   694,   698,
     701,   704,   708,   712,   716,   720,   724,   728,   734,   738,
     742,   746,   750,   754,   758,   762,   766,   770,   774,   777,
     780,   783,   786,   789,   792,   797,   802,   807,   809,   813,
     815,   817,   819,   821,   823,   825,   828,   832,   835,   839,
     843,   846,   849,   853,   858,   863,   868,   872,   874,   876,
     878,   880,   882,   884,   886,   888,   890,   892,   894,   896,
     898,   900,   902,   904,   906,   908,   912,   914,   916,   918,
     922,   924,   926,   928,   930,   931,   933,   935,   939,   941
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
      97,     0,    -1,    -1,    97,    90,    -1,    97,   100,    99,
      -1,    97,   106,    99,    -1,    97,   178,    -1,    97,   108,
      -1,    97,   152,    -1,    97,     1,    99,    -1,    91,    -1,
      91,    -1,    90,    -1,   101,    -1,   102,    -1,   103,    -1,
     104,    -1,   105,    -1,     3,    11,    -1,     4,    16,    -1,
       4,    15,    -1,     5,    15,    -1,     6,    16,    -1,     6,
      15,    -1,     7,    15,    -1,     7,    16,    -1,     8,    -1,
       9,    -1,    36,   107,    -1,    37,   182,   188,    -1,    37,
     182,   182,    -1,    38,    -1,    38,   196,   182,    -1,    38,
     196,    13,    -1,    38,   196,    37,    -1,    38,   196,    38,
      -1,    38,   196,    19,    -1,    -1,   196,   194,    -1,    86,
     196,   194,    92,    -1,   109,    -1,   110,    98,    -1,   111,
      -1,   114,    -1,   119,    -1,   137,    -1,   146,   144,   199,
      98,    -1,   146,   188,    49,   196,   179,   199,    -1,   187,
      49,   196,   179,   199,    98,    -1,   146,   188,   196,   112,
     199,    98,    -1,   113,    -1,   112,   113,    -1,    87,   179,
      93,    -1,   115,    47,   116,    94,   199,    98,    -1,    21,
     188,    -1,    21,    18,    -1,    21,    -1,   117,    -1,   116,
     117,    -1,   118,    -1,   118,    48,    -1,     1,    -1,   188,
      -1,   188,    49,    10,    -1,    20,   120,    47,   136,   128,
      94,   199,    98,    -1,    20,   120,    47,   136,    94,   199,
      98,    -1,    20,   122,   199,    98,    -1,   121,    -1,   121,
      61,   123,    -1,   188,    -1,    18,    -1,   188,    -1,    18,
      -1,   124,    -1,   123,    48,   124,    -1,   125,    -1,   147,
      -1,   126,   147,    -1,   127,    -1,   126,   127,    -1,    43,
      -1,    45,    -1,    42,    -1,   129,    -1,   128,   129,    -1,
     130,    -1,   134,    -1,   114,   136,    -1,    43,    61,    -1,
      42,    61,    -1,    44,    61,    -1,     1,    -1,   146,   188,
      98,   136,    -1,   146,   188,   131,   132,    98,   136,    -1,
      -1,   133,    -1,   132,   133,    -1,    87,    10,    93,    -1,
     135,   140,    98,   136,    -1,    18,   140,    98,   136,    -1,
      95,    18,   140,    98,   136,    -1,    45,    95,    18,   140,
      98,   136,    -1,   146,   188,    -1,    -1,    17,    -1,   136,
      17,    -1,   138,   140,   199,    98,    -1,   138,   140,   199,
     176,    -1,   146,    13,   140,    98,    -1,   146,    13,   140,
     199,   176,    -1,   139,   140,   199,   177,    -1,   146,   188,
      -1,   146,   149,   188,    -1,   149,    18,    -1,   149,    95,
      18,    -1,    86,    92,    -1,    86,   141,    92,    -1,    86,
      92,    41,    -1,    86,   141,    92,    41,    -1,   142,    -1,
     141,    48,   142,    -1,   143,    -1,   143,    49,   181,    -1,
     146,    16,    -1,   146,    12,    -1,   145,    -1,   144,    48,
     145,    -1,   188,    -1,   147,    -1,    39,   147,    -1,    40,
     147,    -1,    41,   147,    -1,    45,   147,    -1,    46,   147,
      -1,    46,    45,   147,    -1,    45,    46,   147,    -1,   148,
      -1,   148,   151,    -1,   148,    66,    -1,   148,   151,    66,
      -1,    18,    -1,    19,    -1,   149,    22,    -1,    18,    88,
      -1,    19,    88,    -1,    18,    -1,    19,    -1,    77,    -1,
     151,    77,    -1,   153,    -1,   170,    -1,   154,    -1,   166,
      -1,   179,    98,    -1,   159,    -1,   168,    -1,    30,   196,
      98,    -1,    30,   196,   179,    98,    -1,    31,    98,    -1,
      32,    98,    -1,   157,    -1,   155,    -1,   176,    -1,    28,
     156,    61,    -1,    29,    61,    -1,    10,    -1,    11,    -1,
      12,    -1,   158,    86,   196,   179,    92,    47,    -1,    27,
      -1,   160,    86,   161,   163,   165,   152,    -1,   160,    86,
     197,   161,   163,   165,   152,    -1,    33,    -1,   162,    -1,
     162,   179,    -1,    91,    -1,   164,    -1,   164,   197,    -1,
      91,    -1,    92,    -1,   167,   152,    -1,    24,    -1,   169,
     173,    98,    -1,   169,   173,   152,    -1,    23,    -1,   171,
     173,   153,    -1,   171,   173,   170,    -1,   172,   153,    -1,
     172,   170,    -1,    25,    -1,    26,    -1,   174,   179,   175,
      -1,    86,    -1,    92,    -1,    47,    -1,    47,    -1,    94,
      -1,   181,    -1,   180,    -1,   185,    -1,   179,    49,   179,
      -1,   179,    49,    47,    -1,   179,    59,   179,    -1,   179,
      58,   179,    -1,   179,    57,   179,    -1,   179,    56,   179,
      -1,   179,    55,   179,    -1,   179,    54,   179,    -1,   179,
      53,   179,    -1,   179,    52,   179,    -1,   179,    51,   179,
      -1,   179,    50,   179,    -1,    34,   196,   150,    -1,    34,
     196,   150,    86,    92,    -1,    34,   196,   150,   113,    -1,
      35,   196,   179,    -1,    35,    87,    93,   196,   179,    -1,
     179,    62,   179,    -1,   179,    63,   179,    -1,    66,   179,
      -1,    77,   179,    -1,   179,    72,   179,    -1,   179,    71,
     179,    -1,   179,    70,   179,    -1,   179,    69,   179,    -1,
     179,    68,   179,    -1,   179,    67,   179,    -1,   179,    60,
     179,    61,   179,    -1,   179,    75,   179,    -1,   179,    76,
     179,    -1,   179,    77,   179,    -1,   179,    78,   179,    -1,
     179,    79,   179,    -1,   179,    74,   179,    -1,   179,    73,
     179,    -1,   179,    66,   179,    -1,   179,    65,   179,    -1,
     179,    64,   179,    -1,   179,    81,    -1,    81,   179,    -1,
     179,    80,    -1,    80,   179,    -1,    76,   179,    -1,    83,
     179,    -1,   179,    87,   179,    93,    -1,    86,   146,    92,
     179,    -1,   146,    86,   179,    92,    -1,   183,    -1,    86,
     179,    92,    -1,    10,    -1,    11,    -1,    12,    -1,    14,
      -1,    36,    -1,    13,    -1,   184,    92,    -1,   184,   192,
      92,    -1,    13,    86,    -1,   180,   189,   190,    -1,   181,
     189,   190,    -1,    85,   190,    -1,   149,   190,    -1,   186,
     199,    92,    -1,   186,   199,   192,    92,    -1,   180,   189,
     190,    86,    -1,   181,   189,   190,    86,    -1,   149,   190,
      86,    -1,    16,    -1,    16,    -1,    10,    -1,    11,    -1,
      12,    -1,    14,    -1,    13,    -1,    85,    -1,    89,    -1,
      16,    -1,    11,    -1,   191,    -1,    12,    -1,    14,    -1,
      13,    -1,    19,    -1,   182,    -1,   193,    -1,   192,    48,
     193,    -1,   179,    -1,    19,    -1,   195,    -1,   194,    48,
     195,    -1,   179,    -1,    19,    -1,    16,    -1,    18,    -1,
      -1,   198,    -1,   110,    -1,   197,    48,   197,    -1,   179,
      -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   162,   162,   163,   164,   165,   166,   169,   172,   173,
     176,   179,   180,   183,   184,   185,   186,   187,   192,   198,
     200,   201,   204,   207,   208,   211,   214,   218,   229,   231,
     235,   237,   239,   243,   247,   251,   255,   261,   262,   263,
     267,   268,   269,   270,   271,   272,   275,   285,   303,   323,
     339,   340,   344,   347,   354,   361,   368,   376,   377,   380,
     381,   382,   385,   402,   422,   426,   430,   436,   437,   440,
     447,   469,   475,   483,   484,   487,   497,   498,   501,   502,
     505,   506,   507,   510,   511,   514,   515,   516,   517,   518,
     519,   520,   523,   535,   551,   559,   560,   564,   568,   574,
     590,   607,   627,   648,   649,   650,   653,   659,   671,   679,
     697,   710,   723,   749,   770,   792,   793,   794,   795,   798,
     799,   802,   803,   809,   817,   827,   828,   835,   844,   845,
     846,   847,   849,   850,   851,   852,   855,   856,   862,   864,
     872,   873,   874,   885,   890,   894,   895,   898,   899,   902,
     903,   906,   907,   910,   911,   912,   913,   914,   915,   916,
     917,   918,   919,   922,   931,   943,   944,   945,   949,   966,
     975,   990,  1013,  1021,  1022,  1025,  1033,  1034,  1037,  1048,
    1059,  1062,  1072,  1095,  1109,  1112,  1115,  1118,  1125,  1129,
    1133,  1148,  1156,  1164,  1168,  1175,  1178,  1187,  1188,  1192,
    1193,  1194,  1204,  1205,  1206,  1207,  1208,  1209,  1210,  1211,
    1212,  1213,  1214,  1215,  1216,  1218,  1219,  1220,  1221,  1222,
    1223,  1224,  1225,  1226,  1227,  1228,  1229,  1230,  1231,  1232,
    1233,  1234,  1235,  1236,  1237,  1238,  1239,  1240,  1241,  1242,
    1243,  1244,  1245,  1246,  1247,  1248,  1254,  1260,  1261,  1265,
    1266,  1267,  1268,  1271,  1275,  1276,  1277,  1292,  1308,  1309,
    1317,  1318,  1324,  1329,  1346,  1348,  1366,  1390,  1394,  1396,
    1398,  1400,  1404,  1408,  1414,  1415,  1418,  1421,  1422,  1427,
    1428,  1429,  1430,  1431,  1434,  1435,  1438,  1439,  1442,  1443,
    1446,  1447,  1448,  1451,  1454,  1457,  1458,  1459,  1462,  1465
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
  "CSS_DELETE", "CSS_COMMAND", "CSS_ALIAS", "CSS_HELP", "CSS_EXTERN",
  "CSS_STATIC", "CSS_CONST", "CSS_PRIVATE", "CSS_PUBLIC", "CSS_PROTECTED",
  "CSS_VIRTUAL", "CSS_INLINE", "'{'", "','", "'='", "CSS_ASGN_OR",
  "CSS_ASGN_XOR", "CSS_ASGN_AND", "CSS_ASGN_RSHIFT", "CSS_ASGN_LSHIFT",
  "CSS_ASGN_MOD", "CSS_ASGN_DIV", "CSS_ASGN_MULT", "CSS_ASGN_SUB",
  "CSS_ASGN_ADD", "'?'", "':'", "CSS_OR", "CSS_AND", "'|'", "'^'", "'&'",
  "CSS_NE", "CSS_EQ", "CSS_LE", "CSS_LT", "CSS_GE", "CSS_GT", "CSS_RSHIFT",
  "CSS_LSHIFT", "'+'", "'-'", "'*'", "'/'", "'%'", "CSS_MINMIN",
  "CSS_PLUSPLUS", "CSS_UNARY", "CSS_NOT", "CSS_UNARYMINUS", "'.'", "'('",
  "'['", "CSS_SCOPER", "CSS_POINTSAT", "'\\n'", "';'", "')'", "']'", "'}'",
  "'~'", "$accept", "toplev", "term", "crterm", "pproc", "ppinclude",
  "ppdefine", "ppifdef", "ppelse", "ppendif", "command", "cmd_args",
  "defn", "vardefn", "vardefin", "arraydefn", "arraydims", "arraydim",
  "enumdefn", "enumname", "enums", "enumline", "enumitms", "classdefn",
  "classhead", "classnm", "classfwd", "classinh", "classpar", "classtyp",
  "inh_mods", "inh_mod", "membs", "membline", "membdefn", "membardimstop",
  "membardims", "membardim", "methdefn", "methname", "classcmt", "fundefn",
  "fundname", "methdname", "funargs", "arglist", "argdefn", "subargdefn",
  "tynames", "tyname", "type", "type_el", "typeorscp", "scopetype",
  "typeonly", "ptrs", "stmt", "noifstmt", "nodostmt", "caseitem",
  "caseexpr", "switchblock", "switch", "forloop", "for", "for_cond",
  "for_cond_sc", "for_incr", "for_incr_sc", "for_end_paren", "doloop",
  "do", "whiloop", "while", "ifstmt", "if", "else", "cond", "cond_paren",
  "cond_end_paren", "bra", "mbr_bra", "ket", "expr", "comb_expr",
  "primitive", "anycmd", "normfuncall", "normfun", "memb_expr", "membfun",
  "undefname", "name", "getmemb", "membname", "membnms", "exprlist",
  "exprlsel", "cmd_exprlist", "cmd_exprlsel", "argstop", "stmtlist",
  "stmtel", "end", 0
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
     295,   296,   297,   298,   299,   300,   301,   123,    44,    61,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
      63,    58,   312,   313,   124,    94,    38,   314,   315,   316,
     317,   318,   319,   320,   321,    43,    45,    42,    47,    37,
     322,   323,   324,   325,   326,    46,    40,    91,   327,   328,
      10,    59,    41,    93,   125,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    96,    97,    97,    97,    97,    97,    97,    97,    97,
      98,    99,    99,   100,   100,   100,   100,   100,   101,   102,
     102,   102,   103,   103,   103,   103,   104,   105,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   107,   107,   107,
     108,   108,   108,   108,   108,   108,   109,   110,   110,   111,
     112,   112,   113,   114,   115,   115,   115,   116,   116,   117,
     117,   117,   118,   118,   119,   119,   119,   120,   120,   121,
     121,   122,   122,   123,   123,   124,   125,   125,   126,   126,
     127,   127,   127,   128,   128,   129,   129,   129,   129,   129,
     129,   129,   130,   130,   131,   132,   132,   133,   134,   134,
     134,   134,   135,   136,   136,   136,   137,   137,   137,   137,
     137,   138,   139,   139,   139,   140,   140,   140,   140,   141,
     141,   142,   142,   143,   143,   144,   144,   145,   146,   146,
     146,   146,   146,   146,   146,   146,   147,   147,   147,   147,
     148,   148,   148,   149,   149,   150,   150,   151,   151,   152,
     152,   153,   153,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   155,   155,   156,   156,   156,   157,   158,
     159,   159,   160,   161,   161,   162,   163,   163,   164,   165,
     166,   167,   168,   168,   169,   170,   170,   170,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   179,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   181,
     181,   181,   181,   182,   183,   183,   183,   184,   185,   185,
     185,   185,   185,   185,   186,   186,   186,   187,   188,   188,
     188,   188,   188,   188,   189,   189,   190,   190,   190,   191,
     191,   191,   191,   191,   192,   192,   193,   193,   194,   194,
     195,   195,   195,   195,   196,   197,   197,   197,   198,   199
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     3,     3,     2,     2,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     2,     3,
       3,     1,     3,     3,     3,     3,     3,     0,     2,     4,
       1,     2,     1,     1,     1,     1,     4,     6,     6,     6,
       1,     2,     3,     6,     2,     2,     1,     1,     2,     1,
       2,     1,     1,     3,     8,     7,     4,     1,     3,     1,
       1,     1,     1,     1,     3,     1,     1,     2,     1,     2,
       1,     1,     1,     1,     2,     1,     1,     2,     2,     2,
       2,     1,     4,     6,     0,     1,     2,     3,     4,     4,
       5,     6,     2,     0,     1,     2,     4,     4,     4,     5,
       4,     2,     3,     2,     3,     2,     3,     3,     4,     1,
       3,     1,     3,     2,     2,     1,     3,     1,     1,     2,
       2,     2,     2,     2,     3,     3,     1,     2,     2,     3,
       1,     1,     2,     2,     2,     1,     1,     1,     2,     1,
       1,     1,     1,     2,     1,     1,     3,     4,     2,     2,
       1,     1,     1,     3,     2,     1,     1,     1,     6,     1,
       6,     7,     1,     1,     2,     1,     1,     2,     1,     1,
       2,     1,     3,     3,     1,     3,     3,     2,     2,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     5,     4,     3,     5,     3,     3,     2,
       2,     3,     3,     3,     3,     3,     3,     5,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     2,     2,     2,     4,     4,     4,     1,     3,     1,
       1,     1,     1,     1,     1,     2,     3,     2,     3,     3,
       2,     2,     3,     4,     4,     4,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     3,
       1,     1,     1,     1,     0,     1,     1,     3,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,    26,
      27,   249,   250,   251,   254,   252,   267,   140,   141,     0,
      56,   184,   181,   189,   190,   169,     0,     0,   294,     0,
       0,   172,   294,   294,   294,     0,   294,     0,     0,     0,
       0,     0,   194,     0,     0,     0,     0,     0,     0,     0,
       0,     3,   196,     0,    13,    14,    15,    16,    17,     0,
       7,    40,     0,    42,    43,     0,    44,    45,     0,     0,
       0,   128,   136,     0,     8,   149,   151,   161,   160,     0,
     154,     0,   152,     0,   155,     0,   150,     0,     0,   162,
       6,     0,   198,   197,   247,     0,   199,   299,     0,    12,
      11,     9,    18,    20,    19,    21,    23,    22,    24,    25,
     257,   143,   144,   269,   270,   271,   273,   272,   268,    70,
       0,    67,   299,    69,    55,    54,   165,   166,   167,     0,
     164,     0,    10,   158,   159,     0,     0,     0,   294,    28,
       0,   253,     0,     0,   129,     0,   130,   131,     0,   132,
       0,   133,     0,     0,   219,   242,   220,   241,   239,   243,
     277,   279,   281,   280,   276,   282,   283,   260,   278,     0,
       0,     4,     5,    41,     0,     0,   299,   299,   273,     0,
       0,     0,   299,   125,     0,   127,   138,   147,   137,   113,
     142,     0,   261,   294,     0,   180,   192,     0,     0,     0,
     187,   188,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   240,   238,     0,   153,   274,   275,     0,     0,
     141,   255,   286,     0,   284,     0,   294,   103,     0,     0,
     163,   156,     0,   145,   146,   212,   294,   215,     0,   292,
     293,   291,   290,    38,   288,    30,    29,    33,    36,    34,
      35,    32,   135,   134,     0,   248,    61,     0,    57,    59,
      62,   115,     0,   119,   121,     0,     0,     0,   299,     0,
       0,     0,   112,   294,     0,   139,   148,   114,   266,     0,
     175,   296,     0,     0,   173,   298,     0,   295,   182,   183,
       0,   185,   186,   201,   200,   211,   210,   209,   208,   207,
     206,   205,   204,   203,   202,     0,   217,   218,   237,   236,
     235,   226,   225,   224,   223,   222,   221,   234,   233,   228,
     229,   230,   231,   232,     0,   258,   259,     0,   256,   262,
       0,     0,   104,     0,    82,    80,    81,    68,    73,    75,
       0,    78,    76,    66,   157,     0,     0,   214,     0,     0,
       0,   245,   299,    58,    60,     0,   117,     0,   116,     0,
     124,   123,   106,   107,   195,   110,   108,     0,   246,   126,
     127,    46,     0,   299,    50,     0,     0,   178,     0,   176,
     174,     0,     0,   193,   191,     0,   244,   264,   265,   285,
     263,   299,    91,   105,   140,     0,     0,     0,     0,   299,
       0,   103,     0,    83,    85,    86,     0,     0,     0,    79,
      77,   213,     0,   216,    39,   289,     0,    63,   120,   118,
     122,   109,   299,    51,     0,     0,   179,     0,   177,   297,
       0,   227,     0,     0,    89,    88,    90,     0,     0,     0,
      87,   299,    84,     0,    94,    74,    52,    53,    47,    49,
     168,   170,     0,    48,   103,     0,    65,     0,     0,   103,
     103,     0,   171,    99,     0,   103,    64,    98,    92,     0,
       0,    95,   103,   100,     0,   103,    96,   101,    97,    93
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,   133,   101,    53,    54,    55,    56,    57,    58,
      59,   139,    60,    61,   301,    63,   393,   367,   421,    65,
     277,   278,   279,    66,   120,   121,   122,   357,   358,   359,
     360,   361,   422,   423,   424,   481,   490,   491,   425,   426,
     353,    67,    68,    69,   176,   282,   283,   284,   182,   183,
     152,    71,    72,   153,   255,   188,    74,    75,    76,    77,
     129,    78,    79,    80,    81,   303,   304,   398,   399,   447,
      82,    83,    84,    85,    86,    87,    88,   197,   198,   404,
      89,   385,    90,    91,    92,    93,   166,    94,    95,    96,
      97,    98,   280,   238,   192,   168,   243,   244,   263,   264,
     131,   306,   307,   245
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -303
static const short int yypact[] =
{
    -303,   502,  -303,   -41,    24,   199,     4,   208,   231,  -303,
    -303,  -303,  -303,  -303,   -21,  -303,  -303,    -5,    50,   336,
     638,  -303,  -303,  -303,  -303,  -303,   190,    90,  -303,    57,
      57,  -303,  -303,    95,   204,   156,   162,   273,   273,   273,
      75,   118,  -303,  1034,  1034,  1034,  1034,  1034,  1034,   323,
    1034,  -303,  -303,   -41,  -303,  -303,  -303,  -303,  -303,   -41,
    -303,  -303,    57,  -303,  -303,   148,  -303,  -303,   121,   121,
      94,  -303,    51,   143,  -303,  -303,  -303,  -303,  -303,   123,
    -303,   130,  -303,   854,  -303,   135,  -303,   135,   854,  -303,
    -303,  1415,   -51,   -51,  -303,   587,  -303,  -303,   176,  -303,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,   138,
     184,   174,  -303,   146,  -303,  -303,  -303,  -303,  -303,   196,
    -303,   772,  -303,  -303,  -303,   322,   170,  1034,  -303,  -303,
     931,  -303,   263,   249,  -303,   244,  -303,  -303,   273,  -303,
     273,  -303,   183,   229,    54,   201,    54,    54,    54,   201,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,    82,
    1239,  -303,  -303,  -303,   428,   172,  -303,  -303,   121,    -5,
      50,  1034,   223,  -303,   405,   -13,  -303,  -303,    87,  -303,
    -303,   265,   210,  -303,   759,  -303,  -303,   677,  1034,   854,
    -303,  -303,   944,  1034,  1034,  1034,  1034,  1034,  1034,  1034,
    1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,
    1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,
    1034,  1034,  -303,  -303,  1034,  -303,  -303,  -303,   323,   323,
      10,  -303,  1497,   -25,  -303,   600,  -303,   314,    26,    57,
    -303,  -303,  1415,  -303,  -303,   274,  -303,    54,   931,  -303,
     122,   212,  1497,   290,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,  -303,  -303,  1034,  -303,  -303,    17,  -303,   295,
     296,   312,   -22,  -303,   309,    35,     1,   317,    57,  1283,
     405,    57,  -303,  -303,   278,  -303,  -303,  -303,  -303,  1034,
    -303,  -303,   173,   276,  1034,  1497,   -31,  -303,  -303,  -303,
    1327,  -303,  -303,  -303,  1497,  1497,  1497,  1497,  1497,  1497,
    1497,  1497,  1497,  1497,  1497,  1458,  1095,   486,   660,   836,
    1518,  1060,  1060,  1060,  1060,  1060,  1060,  1531,  1531,   297,
     297,    54,    54,    54,  1149,   282,   284,  1111,  -303,  -303,
       8,  1034,  -303,    45,  -303,  -303,  -303,   324,  -303,  -303,
      26,  -303,  -303,  -303,  -303,   279,  1034,  -303,  1034,    11,
     931,    54,  -303,  -303,  -303,   376,  -303,   187,   347,   371,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,   342,  -303,  -303,
    -303,  -303,  1034,   278,  -303,  1371,   343,  -303,   299,  1021,
    1497,  1021,   276,  -303,  -303,  1034,  -303,  -303,  -303,  -303,
    -303,  1497,  -303,  -303,    72,   332,   334,   338,    77,  -303,
     379,   314,    36,  -303,  -303,  -303,   121,   405,    26,  -303,
    -303,  -303,  1194,    54,  -303,  -303,    57,  -303,  -303,  -303,
    -303,  -303,  1497,  -303,    57,   354,  -303,   854,   359,  -303,
     299,   408,    57,    57,  -303,  -303,  -303,   390,    57,   121,
     393,  -303,  -303,    57,   107,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,   854,  -303,   314,   121,  -303,    57,    57,   314,
     314,   339,  -303,   393,    57,   314,  -303,   393,   393,   401,
      38,  -303,   314,   393,   344,   314,  -303,   393,  -303,   393
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -303,  -303,   -30,   116,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,  -303,  -303,   426,  -303,  -303,  -279,   430,  -303,
    -303,   155,  -303,  -303,  -303,  -303,  -303,  -303,     7,  -303,
    -303,    83,  -303,    23,  -303,  -303,  -303,   -39,  -303,  -303,
    -225,  -303,  -303,  -303,   -63,  -303,    76,  -303,  -303,   159,
       3,   -28,  -303,     2,  -303,  -303,   -78,   -81,  -303,  -303,
    -303,  -303,  -303,  -303,  -303,   150,  -303,    55,  -303,     9,
    -303,  -303,  -303,  -303,   -72,  -303,  -303,   365,  -303,  -303,
    -278,  -303,  -303,    99,  -303,    79,   -10,  -303,  -303,  -303,
    -303,  -303,   -18,   368,   -35,  -303,   217,   117,   205,    96,
     -12,  -302,  -303,    -6
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -295
static const short int yytable[] =
{
     134,   123,   125,    73,    70,   195,   177,   200,   383,   144,
     146,   147,   149,   151,   167,   394,   201,   401,   276,   105,
     135,   137,   140,   347,   143,   142,   377,   113,   114,   115,
     116,   117,   173,   118,   236,   102,   293,   412,   237,   145,
     145,   145,   145,   145,    17,    18,   412,   380,    42,    99,
     100,   381,   185,   169,   414,    18,   347,    20,  -287,   370,
     300,   235,   413,   414,    18,   110,    20,   348,   354,   355,
     378,   356,   184,  -111,  -294,    37,    38,    39,   415,   416,
     417,   418,    41,   111,    37,    38,    39,   415,   416,   417,
     418,    41,   132,    17,    18,    17,    18,   448,   112,   449,
     410,   251,  -287,   434,   113,   114,   115,   178,   117,   441,
     118,   372,   179,   180,   443,   288,   249,   186,   311,   309,
     272,   148,   273,   148,   266,   489,   258,   312,   187,   132,
     461,   420,   265,   271,   232,   233,    17,    18,   112,   419,
     420,   234,   154,   155,   156,   157,   158,   159,   132,   170,
     145,   130,   145,   295,   160,   161,   162,   163,   175,   164,
     111,   189,   165,   150,   296,   190,   292,   308,   181,   171,
     286,   287,   457,   294,   274,   172,   291,   145,   285,   141,
     181,   299,   136,   113,   114,   115,   116,   117,  -140,   118,
      17,    18,   141,  -102,   242,   174,   460,   302,   132,  -140,
     126,   127,   128,   345,   346,    17,    18,   175,  -140,   193,
     111,    37,    38,    39,   103,   104,   194,    40,    41,   363,
     362,   196,   364,   106,   107,   246,    37,    38,    39,   -72,
     252,   247,    40,    41,   351,   248,   257,   -71,   191,   262,
     160,   161,   162,   163,   368,   164,   108,   109,   165,   483,
     145,   190,   -31,   -31,   487,   488,   382,   250,   386,   181,
     493,   391,   267,   256,   281,   141,   190,   497,   268,   181,
     499,   290,   390,   113,   114,   115,   116,   117,  -141,   118,
     289,   392,   387,   297,   396,   141,   269,   270,   234,  -141,
     138,    17,    18,   305,   -37,   -37,   298,   310,  -141,   141,
     112,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   339,   340,   341,   342,
     343,   352,   430,   344,   160,   161,   162,   163,   370,   164,
     253,   254,   165,   374,   242,   375,   113,   114,   115,   116,
     117,   453,   118,   376,   119,   145,   427,   262,   379,   141,
     365,   366,   145,   463,   384,   366,   436,   397,   407,   471,
     408,   431,   428,   371,   229,   230,   231,   232,   233,   145,
     285,    11,    12,    13,   234,    15,   437,   444,   439,    42,
     149,   446,   293,   454,   482,   455,   477,   459,   395,   456,
     362,   470,   302,   400,   302,   452,   467,   401,   475,   464,
     413,   494,   484,   458,   469,   113,   114,   115,   116,   117,
     145,   118,   473,   474,   145,   427,   489,    62,   476,   276,
     145,    64,   373,   479,   480,   465,   468,   498,   113,   114,
     115,   116,   117,   429,   118,   462,   242,   485,   486,   389,
     411,   496,   199,   438,   492,   478,   402,   450,   440,   472,
     495,   239,   350,   369,   409,   432,   435,   433,   213,   262,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
       0,   442,     0,     0,     0,   234,     0,     0,   305,     0,
     305,     0,     2,     3,   451,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,     0,    16,     0,
      17,    18,    19,    20,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,     0,     0,     0,    40,    41,    42,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,    43,     0,
       0,     0,     0,   234,     0,     0,     0,     0,    44,    45,
       0,     0,    46,    47,     0,    48,     0,    49,    50,     0,
       0,     0,    51,     0,     0,     0,    52,    11,    12,    13,
      14,    15,     0,     0,     0,    17,   240,     0,     0,     0,
      11,    12,    13,    14,    15,     0,     0,     0,    17,   240,
       0,    32,    33,     0,     0,     0,    37,    38,    39,     0,
       0,     0,    40,    41,    32,    33,     0,     0,     0,    37,
      38,    39,     0,     0,     0,    40,    41,     0,   113,   114,
     115,   116,   117,    43,   118,     0,   124,     0,     0,     0,
       0,     0,     0,    44,    45,     0,    43,    46,    47,     0,
      48,     0,    49,    50,     0,     0,    44,    45,     0,   241,
      46,    47,     0,    48,     0,    49,    50,    11,    12,    13,
      14,    15,   349,     0,     0,    17,    18,     0,     0,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,     0,     0,     0,    37,    38,    39,     0,
       0,     0,    40,    41,    42,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,     0,    43,     0,     0,     0,   234,     0,     0,
       0,     0,     0,    44,    45,     0,     0,    46,    47,     0,
      48,     0,    49,    50,     0,     0,     0,     0,   132,    11,
      12,    13,    14,    15,     0,    16,     0,    17,    18,     0,
       0,     0,    11,    12,    13,    14,    15,     0,     0,     0,
      17,    18,     0,    32,    33,     0,     0,     0,    37,    38,
      39,     0,     0,     0,    40,    41,    32,    33,     0,     0,
       0,    37,    38,    39,     0,     0,     0,    40,    41,     0,
       0,     0,     0,     0,     0,    43,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    44,    45,     0,    43,    46,
      47,     0,    48,     0,    49,    50,     0,     0,    44,    45,
     300,     0,    46,    47,     0,    48,     0,    49,    50,     0,
       0,     0,     0,   132,    11,    12,    13,    14,    15,     0,
       0,     0,    17,    18,     0,     0,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
       0,     0,     0,    37,    38,    39,     0,     0,     0,    40,
      41,    42,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,     0,     0,
      43,     0,     0,   234,     0,     0,     0,     0,     0,     0,
      44,    45,     0,     0,    46,    47,     0,    48,     0,    49,
      50,    11,    12,    13,    14,    15,     0,   259,     0,   260,
     261,     0,     0,     0,    11,    12,    13,    14,    15,     0,
       0,     0,    17,    18,     0,    32,    33,     0,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,    32,    33,
       0,     0,     0,    37,    38,    39,     0,     0,     0,    40,
      41,   313,     0,     0,     0,     0,     0,    43,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    44,    45,     0,
      43,    46,    47,     0,    48,     0,    49,    50,     0,     0,
      44,    45,     0,     0,    46,    47,     0,    48,     0,    49,
      50,    11,    12,    13,    14,    15,     0,    16,     0,    17,
      18,     0,     0,     0,    11,    12,    13,    14,    15,     0,
       0,     0,    17,    18,     0,    32,    33,     0,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,    32,    33,
       0,     0,     0,    37,    38,    39,     0,     0,     0,    40,
      41,     0,     0,     0,     0,     0,     0,    43,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    44,    45,     0,
      43,    46,    47,     0,    48,     0,    49,    50,     0,     0,
      44,    45,     0,     0,    46,    47,     0,    48,     0,    49,
      50,    11,    12,    13,    14,    15,     0,     0,     0,    17,
     240,     0,     0,   225,   226,   227,   228,   229,   230,   231,
     232,   233,     0,     0,     0,    32,    33,   234,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,    43,     0,     0,
       0,     0,   234,     0,     0,     0,     0,    44,    45,     0,
       0,    46,    47,     0,    48,     0,    49,    50,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
       0,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,     0,     0,     0,     0,     0,   234,     0,     0,     0,
       0,     0,   406,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,     0,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,     0,     0,     0,     0,
       0,   234,     0,     0,     0,     0,     0,   466,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
       0,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,     0,     0,     0,     0,     0,   234,     0,     0,     0,
       0,   275,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,     0,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,     0,     0,     0,     0,     0,
     234,     0,     0,     0,     0,   388,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,     0,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,     0,
       0,     0,     0,     0,   234,     0,     0,     0,     0,   403,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,     0,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,     0,     0,     0,     0,     0,   234,     0,
       0,     0,     0,   445,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,     0,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,     0,     0,     0,
       0,     0,   234,     0,     0,     0,   132,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   405,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
       0,     0,     0,     0,     0,   234,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,     0,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,     0,
       0,     0,     0,     0,   234,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
       0,     0,     0,     0,     0,   234,   227,   228,   229,   230,
     231,   232,   233,     0,     0,     0,     0,     0,   234
};

static const short int yycheck[] =
{
      30,    19,    20,     1,     1,    83,    69,    88,   286,    37,
      38,    39,    40,    41,    49,   294,    88,    48,     1,    15,
      32,    33,    34,    48,    36,    35,    48,    10,    11,    12,
      13,    14,    62,    16,    85,    11,    49,     1,    89,    37,
      38,    39,    40,    41,    18,    19,     1,    12,    47,    90,
      91,    16,    70,    50,    18,    19,    48,    21,    48,    48,
      91,    91,    17,    18,    19,    86,    21,    92,    42,    43,
      92,    45,    70,    86,    87,    39,    40,    41,    42,    43,
      44,    45,    46,    88,    39,    40,    41,    42,    43,    44,
      45,    46,    91,    18,    19,    18,    19,   399,    88,   401,
      92,   131,    92,    92,    10,    11,    12,    13,    14,   387,
      16,    94,    18,    19,   393,   178,   122,    66,   199,   197,
     148,    46,   150,    46,   142,    87,   138,   199,    77,    91,
      94,    95,   142,   143,    80,    81,    18,    19,    88,    94,
      95,    87,    43,    44,    45,    46,    47,    48,    91,    50,
     148,    61,   150,    66,    11,    12,    13,    14,    86,    16,
      88,    18,    19,    45,    77,    22,   184,   197,    86,    53,
     176,   177,    95,   185,    92,    59,   182,   175,   175,    36,
      86,   193,    87,    10,    11,    12,    13,    14,    66,    16,
      18,    19,    36,    86,    95,    47,   421,   194,    91,    77,
      10,    11,    12,   238,   239,    18,    19,    86,    86,    86,
      88,    39,    40,    41,    15,    16,    86,    45,    46,   249,
     248,    86,   252,    15,    16,    49,    39,    40,    41,    91,
     131,    47,    45,    46,   246,    61,   137,    91,    95,   140,
      11,    12,    13,    14,   256,    16,    15,    16,    19,   474,
     248,    22,    90,    91,   479,   480,   286,    61,   288,    86,
     485,   291,    13,    93,    92,    36,    22,   492,    19,    86,
     495,    48,   290,    10,    11,    12,    13,    14,    66,    16,
     181,   293,   288,    18,   302,    36,    37,    38,    87,    77,
      86,    18,    19,   194,    90,    91,    86,   198,    86,    36,
      88,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,    17,   360,   234,    11,    12,    13,    14,    48,    16,
      18,    19,    19,    48,   245,    49,    10,    11,    12,    13,
      14,   414,    16,    41,    18,   353,   353,   258,    49,    36,
      86,    87,   360,   426,    47,    87,   372,    91,    86,   447,
      86,    92,    48,   274,    77,    78,    79,    80,    81,   377,
     377,    10,    11,    12,    87,    14,    10,   393,    41,    47,
     418,    92,    49,    61,   472,    61,   459,    18,   299,    61,
     428,    47,   399,   304,   401,   411,   436,    48,    18,   427,
      17,    10,   475,   419,   444,    10,    11,    12,    13,    14,
     418,    16,   452,   453,   422,   422,    87,     1,   458,     1,
     428,     1,   277,   463,   464,   428,   442,    93,    10,    11,
      12,    13,    14,   360,    16,   422,   347,   477,   478,   290,
     351,   490,    87,   377,   484,   461,   306,   402,   379,   450,
     490,    93,   245,   258,   347,   366,   370,   368,    60,   370,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      -1,   392,    -1,    -1,    -1,    87,    -1,    -1,   399,    -1,
     401,    -1,     0,     1,   405,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    16,    -1,
      18,    19,    20,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    -1,    -1,    -1,    45,    46,    47,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    66,    -1,
      -1,    -1,    -1,    87,    -1,    -1,    -1,    -1,    76,    77,
      -1,    -1,    80,    81,    -1,    83,    -1,    85,    86,    -1,
      -1,    -1,    90,    -1,    -1,    -1,    94,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    18,    19,
      -1,    34,    35,    -1,    -1,    -1,    39,    40,    41,    -1,
      -1,    -1,    45,    46,    34,    35,    -1,    -1,    -1,    39,
      40,    41,    -1,    -1,    -1,    45,    46,    -1,    10,    11,
      12,    13,    14,    66,    16,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    66,    80,    81,    -1,
      83,    -1,    85,    86,    -1,    -1,    76,    77,    -1,    92,
      80,    81,    -1,    83,    -1,    85,    86,    10,    11,    12,
      13,    14,    92,    -1,    -1,    18,    19,    -1,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    39,    40,    41,    -1,
      -1,    -1,    45,    46,    47,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    -1,    66,    -1,    -1,    -1,    87,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    80,    81,    -1,
      83,    -1,    85,    86,    -1,    -1,    -1,    -1,    91,    10,
      11,    12,    13,    14,    -1,    16,    -1,    18,    19,    -1,
      -1,    -1,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      18,    19,    -1,    34,    35,    -1,    -1,    -1,    39,    40,
      41,    -1,    -1,    -1,    45,    46,    34,    35,    -1,    -1,
      -1,    39,    40,    41,    -1,    -1,    -1,    45,    46,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    77,    -1,    66,    80,
      81,    -1,    83,    -1,    85,    86,    -1,    -1,    76,    77,
      91,    -1,    80,    81,    -1,    83,    -1,    85,    86,    -1,
      -1,    -1,    -1,    91,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    19,    -1,    -1,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    47,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
      66,    -1,    -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,    -1,    83,    -1,    85,
      86,    10,    11,    12,    13,    14,    -1,    16,    -1,    18,
      19,    -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    19,    -1,    34,    35,    -1,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    34,    35,
      -1,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    47,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,
      66,    80,    81,    -1,    83,    -1,    85,    86,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,    -1,    83,    -1,    85,
      86,    10,    11,    12,    13,    14,    -1,    16,    -1,    18,
      19,    -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    19,    -1,    34,    35,    -1,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    34,    35,
      -1,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,
      66,    80,    81,    -1,    83,    -1,    85,    86,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,    -1,    83,    -1,    85,
      86,    10,    11,    12,    13,    14,    -1,    -1,    -1,    18,
      19,    -1,    -1,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    -1,    -1,    -1,    34,    35,    87,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    66,    -1,    -1,
      -1,    -1,    87,    -1,    -1,    -1,    -1,    76,    77,    -1,
      -1,    80,    81,    -1,    83,    -1,    85,    86,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,    -1,
      -1,    -1,    93,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    -1,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,
      -1,    87,    -1,    -1,    -1,    -1,    -1,    93,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,    -1,
      -1,    92,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,
      87,    -1,    -1,    -1,    -1,    92,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      -1,    -1,    -1,    -1,    87,    -1,    -1,    -1,    -1,    92,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    -1,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    -1,    -1,    -1,    -1,    -1,    87,    -1,
      -1,    -1,    -1,    92,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    -1,    -1,    -1,
      -1,    -1,    87,    -1,    -1,    -1,    91,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      -1,    -1,    -1,    -1,    -1,    87,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      -1,    -1,    -1,    -1,    87,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      -1,    -1,    -1,    -1,    -1,    87,    75,    76,    77,    78,
      79,    80,    81,    -1,    -1,    -1,    -1,    -1,    87
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    97,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    16,    18,    19,    20,
      21,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      45,    46,    47,    66,    76,    77,    80,    81,    83,    85,
      86,    90,    94,   100,   101,   102,   103,   104,   105,   106,
     108,   109,   110,   111,   114,   115,   119,   137,   138,   139,
     146,   147,   148,   149,   152,   153,   154,   155,   157,   158,
     159,   160,   166,   167,   168,   169,   170,   171,   172,   176,
     178,   179,   180,   181,   183,   184,   185,   186,   187,    90,
      91,    99,    11,    15,    16,    15,    15,    16,    15,    16,
      86,    88,    88,    10,    11,    12,    13,    14,    16,    18,
     120,   121,   122,   188,    18,   188,    10,    11,    12,   156,
      61,   196,    91,    98,    98,   196,    87,   196,    86,   107,
     196,    36,   182,   196,   147,   149,   147,   147,    46,   147,
      45,   147,   146,   149,   179,   179,   179,   179,   179,   179,
      11,    12,    13,    14,    16,    19,   182,   190,   191,   146,
     179,    99,    99,    98,    47,    86,   140,   140,    13,    18,
      19,    86,   144,   145,   149,   188,    66,    77,   151,    18,
      22,    95,   190,    86,    86,   152,    86,   173,   174,   173,
     153,   170,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    87,    98,    85,    89,   189,   189,
      19,    92,   179,   192,   193,   199,    49,    47,    61,   199,
      61,    98,   179,    18,    19,   150,    93,   179,   196,    16,
      18,    19,   179,   194,   195,   182,   188,    13,    19,    37,
      38,   182,   147,   147,    92,    92,     1,   116,   117,   118,
     188,    92,   141,   142,   143,   146,   199,   199,   140,   179,
      48,   199,   188,    49,   196,    66,    77,    18,    86,   196,
      91,   110,   146,   161,   162,   179,   197,   198,    98,   152,
     179,   153,   170,    47,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   190,   190,    48,    92,    92,
     192,   196,    17,   136,    42,    43,    45,   123,   124,   125,
     126,   127,   147,    98,    98,    86,    87,   113,   196,   194,
      48,   179,    94,   117,    48,    49,    41,    48,    92,    49,
      12,    16,    98,   176,    47,   177,    98,   199,    92,   145,
     188,    98,   196,   112,   113,   179,   188,    91,   163,   164,
     179,    48,   161,    92,   175,    61,    93,    86,    86,   193,
      92,   179,     1,    17,    18,    42,    43,    44,    45,    94,
      95,   114,   128,   129,   130,   134,   135,   146,    48,   127,
     147,    92,   179,   179,    92,   195,   199,    10,   142,    41,
     181,   176,   179,   113,   199,    92,    92,   165,   197,   197,
     163,   179,   199,   140,    61,    61,    61,    95,   199,    18,
     136,    94,   129,   140,   188,   124,    93,    98,   199,    98,
      47,   152,   165,    98,    98,    18,    98,   140,   199,    98,
      98,   131,   152,   136,   140,    98,    98,   136,   136,    87,
     132,   133,    98,   136,    10,    98,   133,   136,    93,   136
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
#line 163 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Blank; }
    break;

  case 4:
#line 164 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 5:
#line 165 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return (yyvsp[-1].ival); }
    break;

  case 6:
#line 166 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 7:
#line 169 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 8:
#line 172 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 9:
#line 173 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Err; }
    break;

  case 10:
#line 176 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 11:
#line 179 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 12:
#line 180 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 18:
#line 192 "css_parse.y"
    {
            cssEl* tmp = (yyvsp[0].el).El();
	    cssMisc::cur_top->SetInclude((const char*)*tmp);
	    cssEl::Done((yyvsp[0].el).El()); }
    break;

  case 19:
#line 198 "css_parse.y"
    {
            cssMisc::Defines.Push(new cssDef(0, (yyvsp[0].nm))); }
    break;

  case 21:
#line 201 "css_parse.y"
    { cssMisc::Defines.Remove((yyvsp[0].el).El()); }
    break;

  case 22:
#line 204 "css_parse.y"
    { /* if its a name, its not defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 24:
#line 208 "css_parse.y"
    { /* if its a def, its defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 26:
#line 214 "css_parse.y"
    { /* if you see it, it applies to u*/
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog()); }
    break;

  case 28:
#line 229 "css_parse.y"
    {
            Code1((yyvsp[-1].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 29:
#line 231 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssRef((yyvsp[-1].el)));
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 30:
#line 235 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc; }
    break;

  case 31:
#line 237 "css_parse.y"
    {
	    Code1((yyvsp[0].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 32:
#line 239 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 33:
#line 243 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 34:
#line 247 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 35:
#line 251 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 36:
#line 255 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[0].el).El());
	    ((yyvsp[-2].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 37:
#line 261 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 39:
#line 263 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 46:
#line 275 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival));
	    if((yyvsp[-2].ival) < 0) (yyval.ival) = (yyvsp[-1].ival)-1; /* if no coding, its the end, else not */
	    else (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 47:
#line 285 "css_parse.y"
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

  case 48:
#line 303 "css_parse.y"
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

  case 49:
#line 323 "css_parse.y"
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

  case 50:
#line 339 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 51:
#line 340 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 52:
#line 344 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 53:
#line 347 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
	    if(cssMisc::cur_enum->name.before(5) == "enum_") /* don't keep anon enums around */
	      delete cssMisc::cur_enum;
	    cssMisc::cur_enum = NULL; }
    break;

  case 54:
#line 354 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_enum = new cssEnumType((const char*)*nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum);
	    cssEl::Done(nm); }
    break;

  case 55:
#line 361 "css_parse.y"
    { /* redefining */
	    if((yyvsp[0].el).El()->GetType() != cssEl::T_EnumType) {
	      yyerror("Attempt to redefine non-enum type as an enum");
	      return cssProg::YY_Err; }
   	    cssMisc::cur_enum = (cssEnumType*)(yyvsp[0].el).El();
 	    cssMisc::cur_enum->enums->Reset(); /* prepare for new enums.. */
	    cssMisc::cur_enum->enum_cnt = 0; }
    break;

  case 56:
#line 368 "css_parse.y"
    { /* anonymous  */
	    String nm = "anonenum_"; nm += String(cssMisc::anon_type_cnt++); nm += "_";
            cssMisc::cur_enum = new cssEnumType((const char*)nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum); }
    break;

  case 61:
#line 382 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 62:
#line 385 "css_parse.y"
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

  case 63:
#line 402 "css_parse.y"
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

  case 64:
#line 422 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[-4].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 65:
#line 426 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[-3].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 66:
#line 430 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival)); (yyval.ival) = (yyvsp[-1].ival)-1;
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 69:
#line 440 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssMisc::cur_class = new cssClassType((const char*)*nm);
	    cssMisc::cur_class->last_top = cssMisc::cur_top;
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
	    cssEl::Done(nm); }
    break;

  case 70:
#line 447 "css_parse.y"
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

  case 71:
#line 469 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_class = new cssClassType((const char*)*nm);
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
            cssEl::Done(nm); }
    break;

  case 72:
#line 475 "css_parse.y"
    {
            if((yyvsp[0].el).El()->GetType() != cssEl::T_ClassType) {
              yyerror("Attempt to redefine non-class type as a class");
              return cssProg::YY_Err;
            }
            cssMisc::cur_class = (cssClassType*)(yyvsp[0].el).El(); }
    break;

  case 75:
#line 487 "css_parse.y"
    {
            if((yyvsp[0].el).El()->GetType() != cssEl::T_ClassType) {
	      yyerror("Cannot inherit from non-class type");
	      /* cssMisc::TypesSpace.Remove(cssMisc::cur_class); */
	      cssMisc::cur_top->types.Remove(cssMisc::cur_class);
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_class->AddParent((cssClassType*)((yyvsp[0].el).El())); }
    break;

  case 77:
#line 498 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); }
    break;

  case 86:
#line 515 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 87:
#line 516 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 91:
#line 520 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 92:
#line 523 "css_parse.y"
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

  case 93:
#line 535 "css_parse.y"
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

  case 94:
#line 551 "css_parse.y"
    {
            /* swap places with void marker */
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_top->Prog()->Stack()->Push(&cssMisc::Void);
            cssMisc::cur_top->Prog()->Stack()->Push(nm); }
    break;

  case 95:
#line 559 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 96:
#line 560 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 97:
#line 564 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((int)*(yyvsp[-1].el).El())); }
    break;

  case 98:
#line 568 "css_parse.y"
    {       /* pre-declare function */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[-3].el).El();
            cssMisc::cur_class->GetComments(fun, (yyvsp[0].el));
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr, true); }
    break;

  case 99:
#line 574 "css_parse.y"
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

  case 100:
#line 590 "css_parse.y"
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

  case 101:
#line 607 "css_parse.y"
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

  case 102:
#line 627 "css_parse.y"
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

  case 103:
#line 648 "css_parse.y"
    { (yyval.el).Reset(); }
    break;

  case 104:
#line 649 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); }
    break;

  case 105:
#line 650 "css_parse.y"
    { (yyval.el) = (yyvsp[-1].el); }
    break;

  case 106:
#line 653 "css_parse.y"
    {	/* pre-declare function */
            cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine((yyvsp[-1].ival));	(yyval.ival) = (yyvsp[-1].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[-3].el).El();
	    fun->argc = (yyvsp[-2].ival); fun->GetArgDefs(); }
    break;

  case 107:
#line 659 "css_parse.y"
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

  case 108:
#line 671 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 109:
#line 679 "css_parse.y"
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

  case 110:
#line 697 "css_parse.y"
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

  case 111:
#line 710 "css_parse.y"
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

  case 112:
#line 723 "css_parse.y"
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
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMethodFmName((const char*)*nm);
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

  case 113:
#line 749 "css_parse.y"
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
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMethodFmName((const char*)cls->name);
	    if(fun == &cssMisc::Void) {
	      yyerror("constructor member function not declared in class type");
	      return cssProg::YY_Err; }
            if(fun->GetType() == cssEl::T_MbrCFun) {
              yyerror("member function is hardcoded");
              return cssProg::YY_Err; }
	    (yyval.el).SetDirect(fun); }
    break;

  case 114:
#line 770 "css_parse.y"
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
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMethodFmName((const char*)nm);
	    if(fun == &cssMisc::Void) {
	      yyerror("destructor member function not declared in class type");
              return cssProg::YY_Err; }
            if(fun->GetType() == cssEl::T_MbrCFun) {
              yyerror("member function is hardcoded");
	      return cssProg::YY_Err; }
	    (yyval.el).SetDirect(fun); }
    break;

  case 115:
#line 792 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 116:
#line 793 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 117:
#line 794 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 118:
#line 795 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 119:
#line 798 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 120:
#line 799 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival) + 1; }
    break;

  case 121:
#line 802 "css_parse.y"
    { cssMisc::parsing_args = false; }
    break;

  case 122:
#line 803 "css_parse.y"
    {
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push((yyvsp[0].el).El()); }
    break;

  case 123:
#line 809 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-1].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[0].nm)));
	    ((yyvsp[-1].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 124:
#line 817 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-1].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    ((yyvsp[-1].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 126:
#line 828 "css_parse.y"
    {
	    if(((yyvsp[-2].ival) < 0) && ((yyvsp[0].ival) < 0)) (yyval.ival) = -1;
	    else if(((yyvsp[-2].ival) >= 0) && ((yyvsp[0].ival) < 0)) (yyval.ival) = (yyvsp[-2].ival);
	    else if(((yyvsp[-2].ival) < 0) && ((yyvsp[0].ival) >= 0)) (yyval.ival) = (yyvsp[0].ival);
	    else (yyval.ival) = -1; }
    break;

  case 127:
#line 835 "css_parse.y"
    {
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      (yyval.ival) = Code2(tmp->ptr, cssBI::constr); }
	    else (yyval.ival) = -1;
	    cssEl::Done(tmp); }
    break;

  case 128:
#line 844 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = ""; }
    break;

  case 129:
#line 845 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "extern"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 130:
#line 846 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "static"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 131:
#line 847 "css_parse.y"
    { /* use const expr prog to code stuff */
	  (yyvsp[0].el).El()->tmp_str = "const"; (yyval.el) = (yyvsp[0].el); cssMisc::CodeConstExpr(); }
    break;

  case 132:
#line 849 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 133:
#line 850 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = ""; (yyval.el) = (yyvsp[0].el); }
    break;

  case 134:
#line 851 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 135:
#line 852 "css_parse.y"
    { (yyvsp[0].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[0].el); }
    break;

  case 136:
#line 855 "css_parse.y"
    { (yyval.el) = (yyvsp[0].el); cssMisc::cur_type = (yyval.el); }
    break;

  case 137:
#line 856 "css_parse.y"
    {
	  if(((yyvsp[-1].el).El()->GetParse() != CSS_PTR) || ((yyvsp[0].ival) >= 2)) {
	    (yyval.el) = cssMisc::cur_top->AddPtrType((yyvsp[-1].el).El()); cssMisc::cur_type = (yyval.el);
	  }
	  else {
	    (yyval.el) = (yyvsp[-1].el); cssMisc::cur_type = (yyval.el); } }
    break;

  case 138:
#line 862 "css_parse.y"
    {
	  (yyval.el) = cssMisc::cur_top->AddRefType((yyvsp[-1].el).El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 139:
#line 864 "css_parse.y"
    {
	  if(((yyvsp[-2].el).El()->GetParse() != CSS_PTR) || ((yyvsp[-1].ival) >= 2)) {
            cssElPtr npt = cssMisc::cur_top->AddPtrType((yyvsp[-2].el).El());
            (yyval.el) = cssMisc::cur_top->AddRefType(npt.El()); cssMisc::cur_type = (yyval.el); }
	  else {
	    (yyval.el) = cssMisc::cur_top->AddRefType((yyvsp[-2].el).El()); cssMisc::cur_type = (yyval.el); } }
    break;

  case 142:
#line 874 "css_parse.y"
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

  case 143:
#line 885 "css_parse.y"
    {
          if(((yyvsp[-1].el).El()->GetType() != cssEl::T_ClassType) && ((yyvsp[-1].el).El()->GetType() != cssEl::T_EnumType)) {
	     yyerror("scoping of non-class or enum type");
	     return cssProg::YY_Err; }
           cssMisc::cur_scope = (yyvsp[-1].el).El(); }
    break;

  case 144:
#line 890 "css_parse.y"
    {
           cssMisc::cur_scope = (yyvsp[-1].el).El(); }
    break;

  case 147:
#line 898 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 148:
#line 899 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival) + 1; }
    break;

  case 149:
#line 902 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastif = -1; }
    break;

  case 153:
#line 910 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 156:
#line 913 "css_parse.y"
    { Code1((yyvsp[-2].el)); (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 157:
#line 914 "css_parse.y"
    { Code1((yyvsp[-3].el)); (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 158:
#line 915 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[-1].el)); }
    break;

  case 159:
#line 916 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[-1].el)); }
    break;

  case 163:
#line 922 "css_parse.y"
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

  case 164:
#line 931 "css_parse.y"
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

  case 168:
#line 949 "css_parse.y"
    {
            (yyval.ival) = (yyvsp[-5].ival);
	    /* value to switch on is already on stack */
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

  case 169:
#line 966 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssSwitchBlock_Name);
	    blk->loop_type = cssCodeBlock::SWITCH;
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 170:
#line 975 "css_parse.y"
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

  case 171:
#line 990 "css_parse.y"
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

  case 172:
#line 1013 "css_parse.y"
    { /* for loop contained within own block */
            cssCodeBlock* blk = new cssCodeBlock(cssForLoop_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 175:
#line 1025 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 178:
#line 1037 "css_parse.y"
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

  case 179:
#line 1048 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of incr */
	      cssCodeBlock* blk = new cssCodeBlock(cssForLoopStmt_Name);
	      blk->owner_prog = cssMisc::cur_top->Prog();
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_type = cssCodeBlock::FOR;
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 181:
#line 1062 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssDoLoop_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->loop_type = cssCodeBlock::DO;
	    cssMisc::cur_top->AddStatic(blk); /* while is all inside this do! */
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 182:
#line 1072 "css_parse.y"
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

  case 183:
#line 1095 "css_parse.y"
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

  case 184:
#line 1109 "css_parse.y"
    { }
    break;

  case 185:
#line 1112 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 186:
#line 1115 "css_parse.y"
    {
	  cssMisc::cur_top->Pop(); /* pop the if_true block */
	  (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 187:
#line 1118 "css_parse.y"
    {
	  cssMisc::cur_top->Pop(); /* pop the if_false block */
	  /* now check for other else blocks that need popping! */
	  while((cssMisc::cur_top->Prog()->owner_blk != NULL) &&
		(cssMisc::cur_top->Prog()->owner_blk->action == cssCodeBlock::ELSE)) {
	    cssMisc::cur_top->Pop(); } /* pop residual elses! */
	  (yyval.ival) = (yyvsp[-1].ival);  }
    break;

  case 188:
#line 1125 "css_parse.y"
    { /* do not pop the ifstmt here!! */
	  (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 189:
#line 1129 "css_parse.y"
    {
          cssMisc::cur_top->Prog()->lastif = cssMisc::cur_top->Prog()->size; }
    break;

  case 190:
#line 1133 "css_parse.y"
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

  case 191:
#line 1148 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssIfTrueBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	    cssMisc::cur_top->AddStatic(blk);
	    Code1(blk); cssMisc::cur_top->Push(blk->code); (yyval.ival) = (yyvsp[-2].ival); }
    break;

  case 192:
#line 1156 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 193:
#line 1164 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); }
    break;

  case 194:
#line 1168 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->SetPush(blk->code); }
    break;

  case 195:
#line 1175 "css_parse.y"
    {  }
    break;

  case 196:
#line 1178 "css_parse.y"
    {
            cssMisc::cur_top->SetPop();
	    cssScriptFun* sfun = cssMisc::cur_top->Prog()->owner_fun;
	    if((sfun != NULL) && (sfun->GetType() == cssEl::T_MbrScriptFun)) {
	      cssMisc::cur_class = NULL; /* get rid of current class pointer.. */
	      cssMisc::cur_method = NULL; } /* and current method pointer */
	  }
    break;

  case 197:
#line 1187 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 200:
#line 1193 "css_parse.y"
    { Code1(cssBI::asgn); }
    break;

  case 201:
#line 1194 "css_parse.y"
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

  case 202:
#line 1204 "css_parse.y"
    { Code1(cssBI::asgn_add); }
    break;

  case 203:
#line 1205 "css_parse.y"
    { Code1(cssBI::asgn_sub); }
    break;

  case 204:
#line 1206 "css_parse.y"
    { Code1(cssBI::asgn_mult); }
    break;

  case 205:
#line 1207 "css_parse.y"
    { Code1(cssBI::asgn_div); }
    break;

  case 206:
#line 1208 "css_parse.y"
    { Code1(cssBI::asgn_mod); }
    break;

  case 207:
#line 1209 "css_parse.y"
    { Code1(cssBI::asgn_lshift); }
    break;

  case 208:
#line 1210 "css_parse.y"
    { Code1(cssBI::asgn_rshift); }
    break;

  case 209:
#line 1211 "css_parse.y"
    { Code1(cssBI::asgn_and); }
    break;

  case 210:
#line 1212 "css_parse.y"
    { Code1(cssBI::asgn_xor); }
    break;

  case 211:
#line 1213 "css_parse.y"
    { Code1(cssBI::asgn_or); }
    break;

  case 212:
#line 1214 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code2((yyvsp[0].el),(yyvsp[-2].el)); }
    break;

  case 213:
#line 1215 "css_parse.y"
    { (yyval.ival) = (yyvsp[-3].ival); Code2((yyvsp[-2].el),(yyvsp[-4].el)); }
    break;

  case 214:
#line 1216 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[-2].ival); Code2((yyvsp[-1].el),(yyvsp[-3].el)); }
    break;

  case 215:
#line 1218 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code1((yyvsp[-2].el)); }
    break;

  case 216:
#line 1219 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); Code1((yyvsp[-4].el)); }
    break;

  case 217:
#line 1220 "css_parse.y"
    { Code1(cssBI::lor); }
    break;

  case 218:
#line 1221 "css_parse.y"
    { Code1(cssBI::land); }
    break;

  case 219:
#line 1222 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::addr_of); }
    break;

  case 220:
#line 1223 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::de_ptr); }
    break;

  case 221:
#line 1224 "css_parse.y"
    { Code1(cssBI::gt); }
    break;

  case 222:
#line 1225 "css_parse.y"
    { Code1(cssBI::ge); }
    break;

  case 223:
#line 1226 "css_parse.y"
    { Code1(cssBI::lt); }
    break;

  case 224:
#line 1227 "css_parse.y"
    { Code1(cssBI::le); }
    break;

  case 225:
#line 1228 "css_parse.y"
    { Code1(cssBI::eq); }
    break;

  case 226:
#line 1229 "css_parse.y"
    { Code1(cssBI::ne); }
    break;

  case 227:
#line 1230 "css_parse.y"
    { Code1(cssBI::cond); }
    break;

  case 228:
#line 1231 "css_parse.y"
    { Code1(cssBI::add); }
    break;

  case 229:
#line 1232 "css_parse.y"
    { Code1(cssBI::sub); }
    break;

  case 230:
#line 1233 "css_parse.y"
    { Code1(cssBI::mul); }
    break;

  case 231:
#line 1234 "css_parse.y"
    { Code1(cssBI::div); }
    break;

  case 232:
#line 1235 "css_parse.y"
    { Code1(cssBI::modulo); }
    break;

  case 233:
#line 1236 "css_parse.y"
    { Code1(cssBI::lshift); }
    break;

  case 234:
#line 1237 "css_parse.y"
    { Code1(cssBI::rshift); }
    break;

  case 235:
#line 1238 "css_parse.y"
    { Code1(cssBI::bit_and); }
    break;

  case 236:
#line 1239 "css_parse.y"
    { Code1(cssBI::bit_xor); }
    break;

  case 237:
#line 1240 "css_parse.y"
    { Code1(cssBI::bit_or); }
    break;

  case 238:
#line 1241 "css_parse.y"
    { Code1(cssBI::asgn_post_pp); }
    break;

  case 239:
#line 1242 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::asgn_pre_pp); }
    break;

  case 240:
#line 1243 "css_parse.y"
    { Code1(cssBI::asgn_post_mm); }
    break;

  case 241:
#line 1244 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::asgn_pre_mm); }
    break;

  case 242:
#line 1245 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::neg); }
    break;

  case 243:
#line 1246 "css_parse.y"
    { (yyval.ival) = (yyvsp[0].ival); Code1(cssBI::lnot); }
    break;

  case 244:
#line 1247 "css_parse.y"
    { Code1(cssBI::de_array); }
    break;

  case 245:
#line 1248 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-2].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = (yyvsp[0].ival); Code2((yyvsp[-2].el), cssBI::cast); }
    break;

  case 246:
#line 1254 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[-3].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
  	    (yyval.ival) = (yyvsp[-1].ival); Code2((yyvsp[-3].el), cssBI::cast); }
    break;

  case 248:
#line 1261 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].ival); }
    break;

  case 254:
#line 1275 "css_parse.y"
    { (yyval.ival) = Code2(cssBI::push_next, (yyvsp[0].el)); }
    break;

  case 255:
#line 1276 "css_parse.y"
    { (yyval.ival) = (yyvsp[-1].el_ival).ival; Code1((yyvsp[-1].el_ival).el); }
    break;

  case 256:
#line 1277 "css_parse.y"
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
#line 1292 "css_parse.y"
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
#line 1308 "css_parse.y"
    { Code2((yyvsp[0].el), cssBI::points_at);}
    break;

  case 259:
#line 1309 "css_parse.y"
    {
	    int mbno = (yyvsp[-2].el).El()->GetMemberNo((const char*)*((yyvsp[0].el).El()));
	    if(mbno < 0) { (yyval.ival) = Code3((yyvsp[-2].el), (yyvsp[0].el), cssBI::points_at); }
	    else { cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
		   (yyval.ival) = Code3((yyvsp[-2].el), tmpint, cssBI::points_at); } }
    break;

  case 260:
#line 1317 "css_parse.y"
    { (yyval.ival) = Code3(cssBI::push_root, (yyvsp[0].el), cssBI::points_at); }
    break;

  case 261:
#line 1318 "css_parse.y"
    {
	  cssMisc::cur_scope = NULL;
	    cssEl* scp = (yyvsp[-1].el).El()->GetScoped((const char*)*((yyvsp[0].el).El()));
	    if(scp != &cssMisc::Void) {  (yyval.ival) = Code1(scp); }
	    else { (yyval.ival) = Code3((yyvsp[-1].el), (yyvsp[0].el), cssBI::scoper); } }
    break;

  case 262:
#line 1324 "css_parse.y"
    {
	  /* argstop is put in by member_fun; member_fun skips over end jump, 
	     uses it to find member_call*/
	  (yyval.ival) = (yyvsp[-2].el_ival).ival;
	  cssMisc::cur_top->Prog()->insts[(yyvsp[-1].ival)-1]->SetLine(Code1(cssBI::member_call)); }
    break;

  case 263:
#line 1329 "css_parse.y"
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

  case 264:
#line 1346 "css_parse.y"
    { Code2((yyvsp[-1].el), cssBI::member_fun);
                                                  (yyval.el_ival).ival = (yyvsp[-3].ival); (yyval.el_ival).el.Reset(); }
    break;

  case 265:
#line 1348 "css_parse.y"
    { (yyval.el_ival).el.Reset();
	    int mbno = (yyvsp[-3].el).El()->GetMethodNo((const char*)*((yyvsp[-1].el).El()));
	    if(mbno < 0) { /* don't complain for pointers and references */
	      if(!(yyvsp[-3].el).El()->MembersDynamic())
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
		cssEl* fun = ths->GetMethodFmName((const char*)*((yyvsp[-1].el).El()));
		if(fun->GetType() != cssEl::T_MbrCFun)
		  (yyval.el_ival).el.SetDirect(fun);
	      } } }
    break;

  case 266:
#line 1366 "css_parse.y"
    {
	    cssMisc::cur_scope = NULL; (yyval.el_ival).el.Reset();
	    int mbno = (yyvsp[-2].el).El()->GetMethodNo((const char*)*((yyvsp[-1].el).El()));
	    if(mbno < 0) { /* don't complain for pointers and references */
	      if(!(yyvsp[-2].el).El()->MembersDynamic())
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
		cssEl* fun = ths->GetMethodFmName((const char*)*((yyvsp[-1].el).El()));
		if(fun->GetType() != cssEl::T_MbrCFun)
		  (yyval.el_ival).el.SetDirect(fun);
	      } } }
    break;

  case 267:
#line 1390 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].nm))); }
    break;

  case 268:
#line 1394 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].nm))); }
    break;

  case 269:
#line 1396 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName())); }
    break;

  case 270:
#line 1398 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName())); }
    break;

  case 271:
#line 1400 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 272:
#line 1404 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 273:
#line 1408 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[0].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding function:", (yyvsp[0].el).El()->PrintStr()); }
    break;

  case 276:
#line 1418 "css_parse.y"
    {
   	    String tmpstr = String((yyvsp[0].nm));
            (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr); }
    break;

  case 278:
#line 1422 "css_parse.y"
    { /* this is source of shift-reduce problems */
	    String tmpstr = String((yyvsp[0].el).El()->GetName());
	    (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr); }
    break;

  case 284:
#line 1434 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 285:
#line 1435 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival) + 1; }
    break;

  case 287:
#line 1439 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 288:
#line 1442 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 289:
#line 1443 "css_parse.y"
    { (yyval.ival) = (yyvsp[-2].ival) + 1; }
    break;

  case 291:
#line 1447 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 292:
#line 1448 "css_parse.y"
    { 
	  String tmpstr = String((yyvsp[0].nm));
	  (yyval.ival) = Code1(cssMisc::cur_top->AddLiteral(tmpstr)); }
    break;

  case 293:
#line 1451 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[0].el)); }
    break;

  case 294:
#line 1454 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); }
    break;

  case 298:
#line 1462 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 299:
#line 1465 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;


    }

/* Line 1037 of yacc.c.  */
#line 3764 "y.tab.c"

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


#line 1469 "css_parse.y"


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
  taMisc::FlushConsole();
}

