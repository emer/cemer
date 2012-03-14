/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

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
     CSS_BITNEG = 325,
     CSS_NOT = 326,
     CSS_UNARYMINUS = 327,
     CSS_SCOPER = 328,
     CSS_POINTSAT = 329
   };
#endif
/* Tokens.  */
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
#define CSS_BITNEG 325
#define CSS_NOT 326
#define CSS_UNARYMINUS 327
#define CSS_SCOPER 328
#define CSS_POINTSAT 329




/* Copy the first part of user declarations.  */
#line 18 "css_parse.y"


#include "css_machine.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "css_builtin.h"
#include "css_ta.h"

#include <sstream>

#if defined(SUN4) && !defined(__GNUG__) && !defined(SOLARIS)
#include <alloca.h>
#endif

#if defined(SGI) || defined(SGIdebug)
#include <alloca.h>
#endif

void yyerror(const char* s);
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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef struct YYSTYPE
#line 51 "css_parse.y"
{
  cssElPlusIVal el_ival;
  cssElPtr     	el;		/* for coding */
  int		ival;		/* for program indexes (progdx) and other ints */
  char*        	nm;
}
/* Line 193 of yacc.c.  */
#line 281 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 294 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
struct yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (struct yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
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
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2204

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  97
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  325
/* YYNRULES -- Number of states.  */
#define YYNSTATES  532

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   329

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      91,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    79,    66,     2,
      87,    93,    77,    75,    48,    76,    86,    78,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    61,    92,
       2,    49,     2,    60,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    88,     2,    94,    65,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    47,    64,    95,    96,     2,     2,     2,
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
      73,    74,    80,    81,    82,    83,    84,    85,    89,    90
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    11,    15,    18,    21,    24,
      28,    30,    32,    34,    36,    38,    40,    42,    44,    47,
      50,    53,    56,    59,    62,    65,    68,    70,    72,    75,
      79,    83,    85,    89,    93,    97,   101,   105,   106,   109,
     114,   116,   119,   121,   123,   125,   127,   132,   139,   145,
     152,   154,   157,   161,   168,   171,   174,   176,   178,   181,
     183,   186,   188,   190,   194,   203,   211,   216,   218,   222,
     224,   226,   228,   230,   232,   236,   238,   240,   243,   245,
     248,   250,   252,   254,   256,   259,   261,   263,   266,   269,
     272,   275,   277,   282,   289,   290,   292,   295,   299,   304,
     309,   315,   322,   325,   326,   328,   331,   336,   341,   346,
     352,   357,   360,   364,   367,   371,   374,   378,   382,   387,
     389,   393,   395,   399,   404,   407,   410,   412,   416,   418,
     420,   423,   426,   429,   432,   435,   439,   443,   445,   448,
     451,   455,   457,   459,   462,   465,   468,   470,   472,   474,
     477,   479,   481,   483,   485,   487,   490,   492,   494,   496,
     500,   505,   508,   511,   513,   515,   519,   522,   524,   526,
     528,   530,   537,   539,   546,   554,   556,   558,   561,   563,
     565,   568,   570,   572,   575,   577,   581,   585,   587,   591,
     595,   599,   601,   603,   605,   608,   613,   617,   618,   620,
     622,   626,   628,   630,   632,   634,   636,   638,   640,   642,
     646,   650,   654,   658,   662,   666,   670,   674,   678,   682,
     686,   690,   694,   700,   705,   709,   715,   719,   723,   726,
     729,   733,   737,   741,   745,   749,   753,   759,   763,   767,
     771,   775,   779,   783,   787,   791,   795,   799,   802,   805,
     808,   811,   814,   817,   820,   822,   825,   830,   835,   837,
     841,   843,   845,   847,   849,   851,   855,   857,   859,   861,
     863,   865,   868,   871,   875,   880,   886,   888,   890,   894,
     898,   900,   903,   907,   910,   914,   918,   921,   924,   928,
     933,   938,   943,   947,   949,   951,   953,   955,   957,   959,
     961,   963,   965,   967,   969,   971,   973,   975,   977,   979,
     981,   983,   987,   989,   991,   993,   997,   999,  1001,  1003,
    1005,  1006,  1008,  1010,  1014,  1016
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      98,     0,    -1,    -1,    98,    91,    -1,    98,   101,   100,
      -1,    98,   107,   100,    -1,    98,   183,    -1,    98,   109,
      -1,    98,   153,    -1,    98,     1,   100,    -1,    92,    -1,
      92,    -1,    91,    -1,   102,    -1,   103,    -1,   104,    -1,
     105,    -1,   106,    -1,     3,    11,    -1,     4,    16,    -1,
       4,    15,    -1,     5,    15,    -1,     6,    16,    -1,     6,
      15,    -1,     7,    15,    -1,     7,    16,    -1,     8,    -1,
       9,    -1,    36,   108,    -1,    37,   187,   199,    -1,    37,
     187,   187,    -1,    38,    -1,    38,   207,   187,    -1,    38,
     207,    13,    -1,    38,   207,    37,    -1,    38,   207,    38,
      -1,    38,   207,    19,    -1,    -1,   207,   205,    -1,    87,
     207,   205,    93,    -1,   110,    -1,   111,    99,    -1,   112,
      -1,   115,    -1,   120,    -1,   138,    -1,   147,   145,   210,
      99,    -1,   147,   199,    49,   207,   184,   210,    -1,   198,
      49,   207,   184,   210,    -1,   147,   199,   207,   113,   210,
      99,    -1,   114,    -1,   113,   114,    -1,    88,   184,    94,
      -1,   116,    47,   117,    95,   210,    99,    -1,    21,   199,
      -1,    21,    18,    -1,    21,    -1,   118,    -1,   117,   118,
      -1,   119,    -1,   119,    48,    -1,     1,    -1,   199,    -1,
     199,    49,    10,    -1,    20,   121,    47,   137,   129,    95,
     210,    99,    -1,    20,   121,    47,   137,    95,   210,    99,
      -1,    20,   123,   210,    99,    -1,   122,    -1,   122,    61,
     124,    -1,   199,    -1,    18,    -1,   199,    -1,    18,    -1,
     125,    -1,   124,    48,   125,    -1,   126,    -1,   148,    -1,
     127,   148,    -1,   128,    -1,   127,   128,    -1,    43,    -1,
      45,    -1,    42,    -1,   130,    -1,   129,   130,    -1,   131,
      -1,   135,    -1,   115,   137,    -1,    43,    61,    -1,    42,
      61,    -1,    44,    61,    -1,     1,    -1,   147,   199,    99,
     137,    -1,   147,   199,   132,   133,    99,   137,    -1,    -1,
     134,    -1,   133,   134,    -1,    88,    10,    94,    -1,   136,
     141,    99,   137,    -1,    18,   141,    99,   137,    -1,    96,
      18,   141,    99,   137,    -1,    45,    96,    18,   141,    99,
     137,    -1,   147,   199,    -1,    -1,    17,    -1,   137,    17,
      -1,   139,   141,   210,    99,    -1,   139,   141,   210,   181,
      -1,   147,    13,   141,    99,    -1,   147,    13,   141,   210,
     181,    -1,   140,   141,   210,   182,    -1,   147,   199,    -1,
     147,   150,   199,    -1,   150,    18,    -1,   150,    96,    18,
      -1,    87,    93,    -1,    87,   142,    93,    -1,    87,    93,
      41,    -1,    87,   142,    93,    41,    -1,   143,    -1,   142,
      48,   143,    -1,   144,    -1,   144,    49,   186,    -1,   144,
      49,   150,    16,    -1,   147,    16,    -1,   147,   202,    -1,
     146,    -1,   145,    48,   146,    -1,   199,    -1,   148,    -1,
      39,   148,    -1,    40,   148,    -1,    41,   148,    -1,    45,
     148,    -1,    46,   148,    -1,    46,    45,   148,    -1,    45,
      46,   148,    -1,   149,    -1,   149,   152,    -1,   149,    66,
      -1,   149,   152,    66,    -1,    18,    -1,    19,    -1,   150,
      22,    -1,    18,    89,    -1,    19,    89,    -1,    18,    -1,
      19,    -1,    77,    -1,   152,    77,    -1,   171,    -1,   174,
      -1,   154,    -1,   155,    -1,   181,    -1,   184,    99,    -1,
     160,    -1,   169,    -1,   167,    -1,    30,   207,    99,    -1,
      30,   207,   184,    99,    -1,    31,    99,    -1,    32,    99,
      -1,   158,    -1,   156,    -1,    28,   157,    61,    -1,    29,
      61,    -1,    10,    -1,    11,    -1,    12,    -1,    14,    -1,
     159,    87,   207,   184,    93,    47,    -1,    27,    -1,   161,
      87,   162,   164,   166,   153,    -1,   161,    87,   208,   162,
     164,   166,   153,    -1,    33,    -1,   163,    -1,   163,   184,
      -1,    92,    -1,   165,    -1,   165,   208,    -1,    92,    -1,
      93,    -1,   168,   153,    -1,    24,    -1,   170,   178,    99,
      -1,   170,   178,   153,    -1,    23,    -1,   176,   178,   154,
      -1,   176,   178,   155,    -1,   176,   178,   181,    -1,   171,
      -1,   154,    -1,   183,    -1,   177,   154,    -1,   177,   172,
     175,   173,    -1,   177,   172,   174,    -1,    -1,    25,    -1,
      26,    -1,   179,   184,   180,    -1,    87,    -1,    93,    -1,
      47,    -1,    47,    -1,    95,    -1,   186,    -1,   185,    -1,
     196,    -1,   184,    49,   184,    -1,   184,    49,    47,    -1,
     184,    59,   184,    -1,   184,    58,   184,    -1,   184,    57,
     184,    -1,   184,    56,   184,    -1,   184,    55,   184,    -1,
     184,    54,   184,    -1,   184,    53,   184,    -1,   184,    52,
     184,    -1,   184,    51,   184,    -1,   184,    50,   184,    -1,
      34,   207,   151,    -1,    34,   207,   151,    87,    93,    -1,
      34,   207,   151,   114,    -1,    35,   207,   184,    -1,    35,
      88,    94,   207,   184,    -1,   184,    62,   184,    -1,   184,
      63,   184,    -1,    66,   184,    -1,    77,   184,    -1,   184,
      72,   184,    -1,   184,    71,   184,    -1,   184,    70,   184,
      -1,   184,    69,   184,    -1,   184,    68,   184,    -1,   184,
      67,   184,    -1,   184,    60,   184,    61,   184,    -1,   184,
      75,   184,    -1,   184,    76,   184,    -1,   184,    77,   184,
      -1,   184,    78,   184,    -1,   184,    79,   184,    -1,   184,
      74,   184,    -1,   184,    73,   184,    -1,   184,    66,   184,
      -1,   184,    65,   184,    -1,   184,    64,   184,    -1,   184,
      81,    -1,    81,   184,    -1,   184,    80,    -1,    80,   184,
      -1,    76,   184,    -1,    84,   184,    -1,    96,   184,    -1,
     188,    -1,   184,   188,    -1,    87,   147,    93,   184,    -1,
     147,    87,   184,    93,    -1,   194,    -1,    87,   184,    93,
      -1,    10,    -1,    11,    -1,    12,    -1,    14,    -1,    36,
      -1,   189,   193,    94,    -1,    88,    -1,    61,    -1,   184,
      -1,    19,    -1,   190,    -1,   184,   190,    -1,   190,   184,
      -1,   184,   190,   184,    -1,   184,   190,   184,   190,    -1,
     184,   190,   184,   190,   184,    -1,    92,    -1,   191,    -1,
     193,    48,   191,    -1,   193,   192,   191,    -1,    13,    -1,
     195,    93,    -1,   195,   203,    93,    -1,    13,    87,    -1,
     185,   200,   201,    -1,   186,   200,   201,    -1,   200,   201,
      -1,   150,   201,    -1,   197,   210,    93,    -1,   197,   210,
     203,    93,    -1,   185,   200,   201,    87,    -1,   186,   200,
     201,    87,    -1,   150,   201,    87,    -1,    16,    -1,    16,
      -1,    10,    -1,    11,    -1,    12,    -1,    14,    -1,    13,
      -1,    86,    -1,    90,    -1,    16,    -1,    11,    -1,    12,
      -1,    14,    -1,    13,    -1,    19,    -1,    10,    -1,   187,
      -1,   204,    -1,   203,    48,   204,    -1,   184,    -1,    19,
      -1,   206,    -1,   205,    48,   206,    -1,   184,    -1,    19,
      -1,    16,    -1,    18,    -1,    -1,   209,    -1,   111,    -1,
     208,    48,   208,    -1,   184,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   156,   156,   157,   158,   159,   160,   163,   166,   167,
     170,   173,   174,   177,   178,   179,   180,   181,   186,   192,
     194,   195,   198,   201,   202,   205,   208,   212,   223,   225,
     229,   231,   233,   237,   241,   245,   249,   255,   256,   257,
     261,   262,   263,   264,   265,   266,   269,   279,   297,   321,
     337,   338,   342,   345,   353,   361,   369,   379,   380,   383,
     384,   385,   388,   406,   427,   431,   435,   441,   442,   445,
     452,   474,   480,   488,   489,   492,   502,   503,   506,   507,
     510,   511,   512,   515,   516,   519,   520,   521,   522,   523,
     524,   525,   528,   540,   556,   564,   565,   569,   573,   579,
     595,   612,   632,   653,   654,   655,   658,   664,   676,   684,
     702,   715,   728,   754,   775,   797,   798,   799,   800,   803,
     804,   807,   808,   811,   818,   826,   836,   837,   844,   853,
     854,   855,   856,   858,   859,   860,   861,   864,   865,   867,
     869,   874,   875,   876,   887,   892,   896,   897,   900,   901,
     904,   905,   906,   909,   910,   913,   914,   915,   916,   917,
     918,   919,   920,   921,   922,   925,   934,   946,   947,   948,
     949,   953,   970,   978,   993,  1016,  1023,  1024,  1027,  1034,
    1035,  1038,  1048,  1058,  1061,  1070,  1093,  1107,  1110,  1115,
    1119,  1125,  1126,  1127,  1130,  1135,  1137,  1141,  1146,  1150,
    1168,  1175,  1182,  1186,  1192,  1195,  1218,  1219,  1223,  1224,
    1225,  1235,  1236,  1237,  1238,  1239,  1240,  1241,  1242,  1243,
    1244,  1245,  1246,  1247,  1249,  1250,  1251,  1252,  1253,  1254,
    1255,  1256,  1257,  1258,  1259,  1260,  1261,  1262,  1263,  1264,
    1265,  1266,  1267,  1268,  1269,  1270,  1271,  1272,  1273,  1274,
    1275,  1276,  1277,  1278,  1279,  1280,  1282,  1288,  1294,  1295,
    1299,  1300,  1301,  1302,  1305,  1308,  1316,  1319,  1322,  1323,
    1324,  1325,  1326,  1327,  1328,  1329,  1332,  1335,  1336,  1337,
    1341,  1342,  1343,  1358,  1374,  1375,  1383,  1384,  1390,  1395,
    1412,  1414,  1432,  1456,  1460,  1462,  1464,  1466,  1470,  1474,
    1480,  1482,  1486,  1490,  1494,  1495,  1496,  1497,  1498,  1499,
    1502,  1503,  1506,  1507,  1510,  1511,  1514,  1515,  1516,  1519,
    1522,  1525,  1526,  1527,  1530,  1533
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
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
  "CSS_PLUSPLUS", "CSS_UNARY", "CSS_BITNEG", "CSS_NOT", "CSS_UNARYMINUS",
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
  "typeorscp", "scopetype", "typeonly", "ptrs", "stmt", "miscbrastmt",
  "miscstmt", "caseitem", "caseexpr", "switchblock", "switch", "forloop",
  "for", "for_cond", "for_cond_sc", "for_incr", "for_incr_sc",
  "for_end_paren", "doloop", "do", "whiloop", "while", "ifstmt",
  "elseifstmt", "noelsestmt", "elsestmt", "popelse", "if", "else", "cond",
  "cond_paren", "cond_end_paren", "bra", "mbr_bra", "ket", "expr",
  "comb_expr", "primitive", "anycmd", "matrixarray", "startmatrix",
  "matcolon", "matexprlsel", "matsemicolon", "matexprlist", "normfuncall",
  "normfun", "memb_expr", "membfun", "undefname", "name", "getmemb",
  "membname", "membnms", "exprlist", "exprlsel", "cmd_exprlist",
  "cmd_exprlsel", "argstop", "stmtlist", "stmtel", "end", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   123,    44,    61,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
      63,    58,   312,   313,   124,    94,    38,   314,   315,   316,
     317,   318,   319,   320,   321,    43,    45,    42,    47,    37,
     322,   323,   324,   325,   326,   327,    46,    40,    91,   328,
     329,    10,    59,    41,    93,   125,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    97,    98,    98,    98,    98,    98,    98,    98,    98,
      99,   100,   100,   101,   101,   101,   101,   101,   102,   103,
     103,   103,   104,   104,   104,   104,   105,   106,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   108,   108,   108,
     109,   109,   109,   109,   109,   109,   110,   111,   111,   112,
     113,   113,   114,   115,   116,   116,   116,   117,   117,   118,
     118,   118,   119,   119,   120,   120,   120,   121,   121,   122,
     122,   123,   123,   124,   124,   125,   126,   126,   127,   127,
     128,   128,   128,   129,   129,   130,   130,   130,   130,   130,
     130,   130,   131,   131,   132,   133,   133,   134,   135,   135,
     135,   135,   136,   137,   137,   137,   138,   138,   138,   138,
     138,   139,   140,   140,   140,   141,   141,   141,   141,   142,
     142,   143,   143,   143,   144,   144,   145,   145,   146,   147,
     147,   147,   147,   147,   147,   147,   147,   148,   148,   148,
     148,   149,   149,   149,   150,   150,   151,   151,   152,   152,
     153,   153,   153,   154,   154,   155,   155,   155,   155,   155,
     155,   155,   155,   155,   155,   156,   156,   157,   157,   157,
     157,   158,   159,   160,   160,   161,   162,   162,   163,   164,
     164,   165,   166,   167,   168,   169,   169,   170,   171,   172,
     172,   173,   173,   173,   174,   174,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   184,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     186,   186,   186,   186,   187,   188,   189,   190,   191,   191,
     191,   191,   191,   191,   191,   191,   192,   193,   193,   193,
     194,   194,   194,   195,   196,   196,   196,   196,   196,   196,
     197,   197,   197,   198,   199,   199,   199,   199,   199,   199,
     200,   200,   201,   201,   202,   202,   202,   202,   202,   202,
     203,   203,   204,   204,   205,   205,   206,   206,   206,   206,
     207,   208,   208,   208,   209,   210
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     3,     3,     2,     2,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     2,     3,
       3,     1,     3,     3,     3,     3,     3,     0,     2,     4,
       1,     2,     1,     1,     1,     1,     4,     6,     5,     6,
       1,     2,     3,     6,     2,     2,     1,     1,     2,     1,
       2,     1,     1,     3,     8,     7,     4,     1,     3,     1,
       1,     1,     1,     1,     3,     1,     1,     2,     1,     2,
       1,     1,     1,     1,     2,     1,     1,     2,     2,     2,
       2,     1,     4,     6,     0,     1,     2,     3,     4,     4,
       5,     6,     2,     0,     1,     2,     4,     4,     4,     5,
       4,     2,     3,     2,     3,     2,     3,     3,     4,     1,
       3,     1,     3,     4,     2,     2,     1,     3,     1,     1,
       2,     2,     2,     2,     2,     3,     3,     1,     2,     2,
       3,     1,     1,     2,     2,     2,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     3,
       4,     2,     2,     1,     1,     3,     2,     1,     1,     1,
       1,     6,     1,     6,     7,     1,     1,     2,     1,     1,
       2,     1,     1,     2,     1,     3,     3,     1,     3,     3,
       3,     1,     1,     1,     2,     4,     3,     0,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     5,     4,     3,     5,     3,     3,     2,     2,
       3,     3,     3,     3,     3,     3,     5,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
       2,     2,     2,     2,     1,     2,     4,     4,     1,     3,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     2,     2,     3,     4,     5,     1,     1,     3,     3,
       1,     2,     3,     2,     3,     3,     2,     2,     3,     4,
       4,     4,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     1,     1,     3,     1,     1,     1,     1,
       0,     1,     1,     3,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,    26,
      27,   260,   261,   262,   280,   263,   293,   141,   142,     0,
      56,   187,   184,   198,   199,   172,     0,     0,   320,     0,
       0,   175,   320,   320,   320,     0,   320,     0,     0,     0,
       0,     0,   203,     0,     0,     0,     0,     0,     0,   300,
       0,   266,   301,     3,   205,     0,     0,    13,    14,    15,
      16,    17,     0,     7,    40,     0,    42,    43,     0,    44,
      45,     0,     0,     0,   129,   137,     0,     8,   152,   153,
     164,   163,     0,   156,     0,   158,     0,   157,     0,   150,
     151,     0,     0,   154,     6,     0,   207,   206,   254,     0,
     258,     0,   208,   325,     0,     0,    12,    11,     9,    18,
      20,    19,    21,    23,    22,    24,    25,   283,   144,   145,
     295,   296,   297,   299,   298,   294,    70,     0,    67,   325,
      69,    55,    54,   167,   168,   169,   170,     0,   166,     0,
      10,   161,   162,     0,     0,     0,   320,    28,     0,   264,
       0,     0,   130,     0,   131,   132,     0,   133,     0,   134,
       0,     0,   228,   251,   229,   250,   248,   252,     0,     0,
     253,     4,     5,    41,     0,     0,   325,   325,   299,     0,
       0,     0,   325,   126,     0,   128,   139,   148,   138,   303,
     302,   113,   143,     0,   287,   320,     0,   183,   201,     0,
       0,     0,   194,   197,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   249,   247,   155,   255,     0,
       0,   142,   267,   268,   270,   277,     0,   142,   281,   312,
       0,   310,     0,   320,   286,   103,     0,     0,   165,   159,
       0,   146,   147,   221,   320,   224,     0,   318,   319,   317,
     316,    38,   314,    30,    29,    33,    36,    34,    35,    32,
     136,   135,     0,   259,    61,     0,    57,    59,    62,   115,
       0,   119,   121,     0,     0,     0,   325,     0,     0,     0,
     112,   320,     0,   140,   149,   114,   292,     0,   178,   322,
       0,     0,   176,   324,     0,   321,   185,   186,     0,   188,
     196,     0,     0,   210,   209,   220,   219,   218,   217,   216,
     215,   214,   213,   212,   211,     0,   226,   227,   246,   245,
     244,   235,   234,   233,   232,   231,   230,   243,   242,   237,
     238,   239,   240,   241,   284,   285,   271,   272,     0,   276,
     265,     0,     0,   282,   288,     0,     0,   104,     0,    82,
      80,    81,    68,    73,    75,     0,    78,    76,    66,   160,
       0,     0,   223,     0,     0,     0,   256,   325,    58,    60,
       0,   117,     0,   116,     0,   308,   304,   306,   305,   124,
     307,   309,   125,   106,   107,   204,   110,   108,     0,   257,
     127,   128,    46,     0,   325,    50,     0,     0,   181,     0,
     179,   177,     0,     0,   202,   200,   192,   191,   195,   193,
     189,   190,     0,   290,   291,   273,   278,   279,   311,   289,
     325,    91,   105,   141,     0,     0,     0,     0,   325,     0,
     103,     0,    83,    85,    86,     0,     0,     0,    79,    77,
     222,     0,   225,    39,   315,     0,    63,   120,   118,     0,
     122,   109,   325,    51,     0,     0,   182,     0,   180,   323,
       0,   236,   274,    48,     0,    89,    88,    90,     0,     0,
       0,    87,   325,    84,     0,    94,    74,    52,    53,   123,
      47,    49,   171,   173,     0,   275,   103,     0,    65,     0,
       0,   103,   103,     0,   174,    99,     0,   103,    64,    98,
      92,     0,     0,    95,   103,   100,     0,   103,    96,   101,
      97,    93
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,   141,   108,    56,    57,    58,    59,    60,    61,
      62,   147,    63,    64,   309,    66,   414,   382,   450,    68,
     285,   286,   287,    69,   127,   128,   129,   372,   373,   374,
     375,   376,   451,   452,   453,   513,   522,   523,   454,   455,
     368,    70,    71,    72,   176,   290,   291,   292,   182,   183,
     160,    74,    75,   161,   263,   188,    77,    78,    79,    80,
     137,    81,    82,    83,    84,   311,   312,   419,   420,   477,
      85,    86,    87,    88,    89,   203,   428,    90,   321,    91,
      92,   199,   200,   425,    93,   406,    94,    95,    96,    97,
     150,    98,    99,   244,   245,   361,   246,   100,   101,   102,
     103,   104,   288,   105,   194,   402,   250,   251,   271,   272,
     139,   314,   315,   252
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -340
static const yytype_int16 yypact[] =
{
    -340,   788,  -340,   149,     9,   238,    54,   268,   277,  -340,
    -340,  -340,  -340,  -340,    -4,  -340,  -340,     4,    29,   420,
     723,  -340,  -340,  -340,  -340,  -340,   221,    -2,  -340,    59,
      59,  -340,  -340,    19,   159,   118,   211,   330,   330,   330,
     143,   147,  -340,  1658,  1658,  1658,  1658,  1658,  1658,  -340,
    1658,  -340,  -340,  -340,  -340,  1658,   149,  -340,  -340,  -340,
    -340,  -340,   149,  -340,  -340,    59,  -340,  -340,   139,  -340,
    -340,   108,   108,    76,  -340,   -13,    44,  -340,  -340,  -340,
    -340,  -340,   123,  -340,   136,  -340,   875,  -340,   160,  -340,
    -340,   160,  1043,  -340,  -340,  1992,    97,    97,  -340,  1184,
    -340,  1244,  -340,  -340,   227,   213,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,   170,   208,   209,  -340,
     187,  -340,  -340,  -340,  -340,  -340,  -340,   224,  -340,  1304,
    -340,  -340,  -340,   332,   173,  1658,  -340,  -340,  1391,  -340,
     261,   206,  -340,   273,  -340,  -340,   330,  -340,   330,  -340,
     212,    99,    61,   216,    61,    61,    61,   216,    84,  1812,
     216,  -340,  -340,  -340,   404,   172,  -340,  -340,   108,     4,
      29,  1658,   253,  -340,   374,    35,  -340,  -340,   -10,  -340,
    -340,  -340,  -340,   319,   254,  -340,   517,  -340,  -340,   684,
    1658,  1124,  -340,   329,   160,  1451,  1658,  1658,  1658,  1658,
    1658,  1658,  1658,  1658,  1658,  1658,  1658,  1658,  1658,  1658,
    1658,  1658,  1658,  1658,  1658,  1658,  1658,  1658,  1658,  1658,
    1658,  1658,  1658,  1658,  1658,  -340,  -340,  -340,  -340,   213,
     213,   -12,  -340,  2036,  1658,  -340,   -24,    24,  -340,  2116,
      13,  -340,  1511,  -340,  -340,   340,   218,    59,  -340,  -340,
    1992,  -340,  -340,   266,  -340,    61,  1391,  -340,   191,   269,
    2116,   313,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  1658,  -340,  -340,    32,  -340,   325,   327,   333,
      15,  -340,   331,   326,    34,   334,    59,  1857,   374,    59,
    -340,  -340,   290,  -340,  -340,  -340,  -340,  1658,  -340,  -340,
      18,   287,  1658,  2116,    -1,  -340,  -340,  -340,  1902,  -340,
    -340,   956,  1124,  -340,  2116,  2116,  2116,  2116,  2116,  2116,
    2116,  2116,  2116,  2116,  2116,  2076,   600,   772,   858,   938,
    1024,   465,   465,   465,   465,   465,   465,   511,   511,   210,
     210,    61,    61,    61,   296,   304,  1658,  2116,  1184,  -340,
    -340,  1184,  1718,  -340,  -340,    23,  1658,  -340,    57,  -340,
    -340,  -340,   347,  -340,  -340,   218,  -340,  -340,  -340,  -340,
     306,  1658,  -340,  1658,    31,  1391,    61,  -340,  -340,  -340,
     387,  -340,   175,   360,   353,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,   355,  -340,
    -340,  -340,  -340,  1658,   290,  -340,  1947,   354,  -340,   315,
    1598,  2116,  1598,   287,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  1658,  -340,  -340,  2036,  -340,  -340,  -340,  -340,
    2116,  -340,  -340,    50,   345,   348,   349,     8,  -340,   393,
     340,   163,  -340,  -340,  -340,   108,   374,   218,  -340,  -340,
    -340,  1766,    61,  -340,  -340,    59,  -340,  -340,  -340,   397,
    -340,  -340,  2116,  -340,    59,   375,  -340,   875,   378,  -340,
     315,   431,  1658,  -340,    59,  -340,  -340,  -340,   409,    59,
     108,   422,  -340,  -340,    59,    87,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,   875,  2116,   340,   108,  -340,    59,
      59,   340,   340,   357,  -340,   422,    59,   340,  -340,   422,
     422,   427,   134,  -340,   340,   422,   352,   340,  -340,   422,
    -340,   422
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -340,  -340,   -30,    63,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,   441,  -340,  -340,  -279,   442,  -340,
    -340,   164,  -340,  -340,  -340,  -340,  -340,  -340,    -7,  -340,
    -340,    78,  -340,    12,  -340,  -340,  -340,   -67,  -340,  -340,
    -339,  -340,  -340,  -340,   -66,  -340,    68,  -340,  -340,   178,
       1,   -28,  -340,     0,  -340,  -340,   -79,   -87,   151,  -340,
    -340,  -340,  -340,  -340,  -340,   168,  -340,    55,  -340,    -3,
    -340,  -340,  -340,  -340,   166,  -340,  -340,   281,  -340,   396,
    -340,   -75,  -340,  -340,  -272,  -340,   169,   100,  -340,    95,
    -136,   305,  -340,  -235,  -309,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,   -16,   263,   -80,  -340,   262,   153,   250,   132,
     -15,  -224,  -340,   -44
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -321
static const yytype_int16 yytable[] =
{
     142,    76,    73,   130,   132,   202,   177,   197,   356,   152,
     154,   155,   157,   159,   273,   279,   201,   143,   145,   148,
     109,   151,   404,   415,   358,   254,    17,    18,   120,   121,
     122,   123,   124,   284,   125,   173,  -269,   153,   153,   153,
     153,   153,   120,   121,   122,   123,   124,   422,   125,   436,
     431,   168,   437,   186,   156,   189,   303,   185,   441,   138,
     190,   362,   191,   392,   187,   237,   192,   304,   359,   112,
     360,   362,  -313,   184,   442,   443,    18,   119,    20,   385,
    -269,    42,  -269,   117,   301,   257,   120,   121,   122,   178,
     124,   308,   125,   118,   179,   180,    37,    38,    39,   444,
     445,   446,   447,    41,   488,   181,   363,   144,   393,   259,
     189,   491,   296,   119,   319,   190,   439,  -313,   119,   171,
     317,   192,  -111,  -320,   463,   172,   140,   387,   280,   322,
     281,   266,   294,   295,   274,   473,   471,   175,   299,   118,
     193,   235,   236,   162,   163,   164,   165,   166,   167,    51,
     169,   140,   448,   449,   149,   170,   153,   401,   153,   354,
     355,    17,    18,   181,   441,    17,    18,   515,   300,   316,
     302,   181,   519,   520,  -102,   153,   293,   282,   525,   140,
     307,   443,    18,    49,    20,   529,   174,    52,   531,   156,
      17,    18,   158,    17,    18,   175,   478,   310,   479,   243,
     482,   249,    37,    38,    39,   444,   445,   446,   447,    41,
     195,    37,    38,    39,    37,    38,    39,    40,    41,   275,
      40,    41,   521,   196,   189,   276,   140,   378,   377,   190,
     379,   133,   134,   135,   426,   136,    17,    18,   366,   260,
     106,   107,   149,   277,   278,   265,   146,   198,   270,   383,
     -37,   -37,   408,   110,   111,   255,   153,  -141,   492,   449,
     369,   370,   -72,   371,   403,   289,   407,   264,  -141,   412,
     256,   120,   121,   122,   123,   124,   253,   125,  -141,   -71,
     118,   297,   411,   113,   114,   258,   413,   232,   233,   234,
     235,   236,   115,   116,   417,   192,   313,   149,    51,   181,
     318,   298,   -31,   -31,    51,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   352,   353,  -142,   395,   305,   396,   397,
     398,   306,   399,   465,   357,   400,  -142,   459,    17,    18,
     261,   262,   249,   380,   381,    24,  -142,   367,   119,   239,
     240,   385,   149,    11,    12,    13,   270,    15,   153,   456,
     474,   179,   180,   389,   391,   153,   390,   484,   381,   418,
     394,   405,   386,   433,   120,   121,   122,   123,   124,   494,
     125,   434,   153,   293,   469,   457,   483,   466,   503,   460,
     238,   468,    42,   301,   489,   284,   485,   416,   476,   486,
     487,   490,   421,   499,   120,   121,   122,   123,   124,   157,
     125,   310,   502,   310,   509,   514,   422,   507,   500,   377,
     120,   121,   122,   123,   124,   498,   125,   526,   126,   442,
     495,   516,    65,    67,   501,   521,   530,   153,   510,   388,
     496,   153,   456,   458,   506,   528,   435,   153,   243,   508,
     467,   243,   249,   493,   511,   512,   440,   238,   238,   238,
     238,   238,   238,   430,   238,   238,   410,   504,   480,   517,
     518,   461,   423,   462,   320,   270,   524,   427,   204,   470,
     429,   216,   527,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   472,   365,   438,   384,   464,     0,    51,
     313,     0,   313,     0,     0,     0,     0,    11,    12,    13,
      14,    15,   481,    16,     0,    17,    18,     0,   228,   229,
     230,   231,   232,   233,   234,   235,   236,     0,   238,     0,
       0,    32,    33,    51,   238,     0,    37,    38,    39,     0,
       0,     0,    40,    41,     0,   238,     0,     0,     0,     0,
     238,     0,     0,     0,     0,   238,     0,     0,     0,     0,
       0,     0,   505,    43,     0,     0,   230,   231,   232,   233,
     234,   235,   236,    44,    45,     0,     0,    46,    47,    51,
       0,    48,   238,    49,    50,    51,     0,    52,     0,   308,
       0,     0,     0,    55,     0,     0,     0,     0,   238,     0,
       0,     0,     0,   238,     0,     0,     0,     0,     0,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,     0,
       0,     0,   238,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,     0,     0,     0,     0,     0,     0,    51,     0,
       0,   238,     0,     0,    11,    12,    13,    14,    15,     0,
       0,     0,    17,    18,     0,     0,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
       0,   238,     0,    37,    38,    39,   238,     0,     0,    40,
      41,    42,     0,   120,   121,   122,   123,   124,     0,   125,
     238,   131,     0,     0,     0,   238,     0,     0,     0,     0,
      43,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      44,    45,     0,     0,    46,    47,   238,   238,    48,     0,
      49,    50,    51,     0,    52,     0,   140,   238,     0,     0,
      55,     0,     0,     0,     0,     0,   238,     0,     2,     3,
       0,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,     0,    16,     0,    17,    18,    19,    20,
     238,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
       0,     0,     0,    40,    41,    42,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,    43,     0,     0,     0,     0,     0,
      51,     0,     0,     0,    44,    45,     0,     0,    46,    47,
       0,     0,    48,     0,    49,    50,    51,     0,    52,    53,
       0,     0,     0,    54,    55,    11,    12,    13,    14,    15,
       0,     0,     0,    17,    18,     0,     0,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     0,     0,     0,    37,    38,    39,     0,     0,     0,
      40,    41,    42,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
       0,    43,     0,     0,     0,     0,    51,     0,     0,     0,
       0,    44,    45,     0,     0,    46,    47,     0,     0,    48,
       0,    49,    50,    51,     0,    52,    11,    12,    13,    14,
      15,    55,     0,     0,    17,    18,     0,     0,     0,    21,
      22,    23,     0,    25,    26,    27,    28,    29,    30,    31,
      32,    33,     0,     0,     0,    37,    38,    39,     0,     0,
       0,    40,    41,    42,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
       0,     0,    43,     0,     0,     0,    51,     0,     0,     0,
       0,     0,    44,    45,     0,     0,    46,    47,     0,     0,
      48,     0,    49,    50,    51,     0,    52,     0,     0,     0,
       0,    54,    55,    11,    12,    13,    14,    15,     0,     0,
       0,    17,    18,     0,     0,     0,    21,    22,    23,     0,
      25,    26,    27,    28,    29,    30,    31,    32,    33,     0,
       0,     0,    37,    38,    39,     0,     0,     0,    40,    41,
      42,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,     0,     0,     0,    43,
       0,     0,    51,     0,     0,     0,     0,     0,     0,    44,
      45,     0,     0,    46,    47,     0,     0,    48,     0,    49,
      50,    51,     0,    52,    11,    12,    13,    14,    15,    55,
       0,     0,    17,    18,     0,     0,     0,    21,    22,     0,
       0,    25,    26,    27,    28,    29,    30,    31,    32,    33,
       0,     0,     0,    37,    38,    39,     0,     0,     0,    40,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      43,     0,     0,     0,    11,    12,    13,    14,    15,     0,
      44,    45,    17,   241,    46,    47,     0,     0,    48,     0,
      49,    50,    51,     0,    52,     0,     0,     0,    32,    33,
      55,     0,     0,    37,    38,    39,     0,     0,     0,    40,
      41,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   242,     0,     0,     0,     0,
      43,     0,     0,     0,    11,    12,    13,    14,    15,     0,
      44,    45,    17,   247,    46,    47,     0,     0,    48,     0,
      49,    50,    51,     0,    52,     0,     0,     0,    32,    33,
      55,     0,     0,    37,    38,    39,     0,     0,     0,    40,
      41,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      43,     0,     0,     0,    11,    12,    13,    14,    15,     0,
      44,    45,    17,    18,    46,    47,     0,     0,    48,     0,
      49,    50,    51,     0,    52,     0,     0,   248,    32,    33,
      55,     0,     0,    37,    38,    39,     0,     0,     0,    40,
      41,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      43,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      44,    45,     0,     0,    46,    47,     0,     0,    48,     0,
      49,    50,    51,     0,    52,     0,   140,     0,     0,     0,
      55,    11,    12,    13,    14,    15,     0,   267,     0,   268,
     269,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    32,    33,     0,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,     0,     0,
       0,    11,    12,    13,    14,    15,     0,    44,    45,    17,
      18,    46,    47,     0,     0,    48,     0,    49,    50,    51,
       0,    52,     0,     0,     0,    32,    33,    55,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,   323,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,     0,     0,
       0,    11,    12,    13,    14,    15,     0,    44,    45,    17,
     247,    46,    47,     0,     0,    48,     0,    49,    50,    51,
       0,    52,     0,     0,     0,    32,    33,    55,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    44,    45,     0,
       0,    46,    47,     0,     0,    48,     0,    49,    50,    51,
       0,    52,     0,     0,   364,     0,     0,    55,    11,    12,
      13,    14,    15,     0,    16,     0,    17,    18,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,    33,     0,     0,     0,    37,    38,    39,
       0,     0,     0,    40,    41,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,     0,     0,     0,    11,    12,
      13,    14,    15,     0,    44,    45,    17,    18,    46,    47,
       0,     0,    48,     0,    49,    50,    51,     0,    52,     0,
       0,     0,    32,    33,    55,     0,     0,    37,    38,    39,
       0,     0,     0,    40,    41,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,     0,     0,     0,    11,    12,
      13,    14,    15,     0,    44,    45,    17,   247,    46,    47,
       0,     0,    48,     0,    49,    50,    51,     0,    52,     0,
       0,     0,    32,    33,    55,     0,     0,    37,    38,    39,
       0,     0,     0,    40,    41,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    44,    45,     0,     0,    46,    47,
       0,     0,    48,     0,    49,    50,    51,     0,    52,     0,
       0,     0,     0,     0,    55,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,     0,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,     0,     0,
       0,     0,     0,     0,    51,     0,     0,     0,     0,     0,
     497,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,     0,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,     0,     0,     0,     0,     0,     0,
      51,     0,     0,     0,     0,   283,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,     0,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,     0,
       0,     0,     0,     0,     0,    51,     0,     0,     0,     0,
     409,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,     0,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,     0,     0,     0,     0,     0,     0,
      51,     0,     0,     0,     0,   424,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,     0,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,     0,
       0,     0,     0,     0,     0,    51,     0,     0,     0,     0,
     475,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,     0,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,     0,     0,     0,     0,     0,     0,
      51,     0,     0,     0,   140,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   242,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,     0,     0,
       0,     0,     0,     0,    51,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   432,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,     0,     0,
       0,     0,     0,     0,    51,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,     0,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,     0,     0,
       0,     0,     0,     0,    51
};

static const yytype_int16 yycheck[] =
{
      30,     1,     1,    19,    20,    92,    72,    86,   243,    37,
      38,    39,    40,    41,   150,   151,    91,    32,    33,    34,
      11,    36,   294,   302,    48,   105,    18,    19,    10,    11,
      12,    13,    14,     1,    16,    65,    48,    37,    38,    39,
      40,    41,    10,    11,    12,    13,    14,    48,    16,   358,
     322,    50,   361,    66,    46,    11,    66,    73,     1,    61,
      16,    48,    18,    48,    77,    95,    22,    77,    92,    15,
      94,    48,    48,    73,    17,    18,    19,    89,    21,    48,
      92,    47,    94,    87,    49,   129,    10,    11,    12,    13,
      14,    92,    16,    89,    18,    19,    39,    40,    41,    42,
      43,    44,    45,    46,    96,    87,    93,    88,    93,   139,
      11,   450,   178,    89,   201,    16,    93,    93,    89,    56,
     199,    22,    87,    88,    93,    62,    92,    95,   156,   204,
     158,   146,   176,   177,   150,   414,   408,    87,   182,    89,
      96,    80,    81,    43,    44,    45,    46,    47,    48,    88,
      50,    92,    95,    96,    36,    55,   156,   293,   158,   239,
     240,    18,    19,    87,     1,    18,    19,   506,   184,   199,
     185,    87,   511,   512,    87,   175,   175,    93,   517,    92,
     195,    18,    19,    86,    21,   524,    47,    90,   527,    46,
      18,    19,    45,    18,    19,    87,   420,   196,   422,    99,
     435,   101,    39,    40,    41,    42,    43,    44,    45,    46,
      87,    39,    40,    41,    39,    40,    41,    45,    46,    13,
      45,    46,    88,    87,    11,    19,    92,   257,   256,    16,
     260,    10,    11,    12,   321,    14,    18,    19,   253,   139,
      91,    92,    36,    37,    38,   145,    87,    87,   148,   264,
      91,    92,   296,    15,    16,    47,   256,    66,    95,    96,
      42,    43,    92,    45,   294,    93,   296,    94,    77,   299,
      61,    10,    11,    12,    13,    14,    49,    16,    87,    92,
      89,   181,   298,    15,    16,    61,   301,    77,    78,    79,
      80,    81,    15,    16,   310,    22,   196,    36,    88,    87,
     200,    48,    91,    92,    88,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,    66,    10,    18,    12,    13,
      14,    87,    16,   387,   244,    19,    77,   375,    18,    19,
      18,    19,   252,    87,    88,    26,    87,    17,    89,    96,
      97,    48,    36,    10,    11,    12,   266,    14,   368,   368,
     414,    18,    19,    48,    41,   375,    49,   443,    88,    92,
      49,    47,   282,    87,    10,    11,    12,    13,    14,   455,
      16,    87,   392,   392,   394,    48,   440,    10,   477,    93,
      95,    41,    47,    49,   448,     1,    61,   307,    93,    61,
      61,    18,   312,    16,    10,    11,    12,    13,    14,   447,
      16,   420,    47,   422,   490,   504,    48,    18,   472,   457,
      10,    11,    12,    13,    14,   465,    16,    10,    18,    17,
     456,   507,     1,     1,   474,    88,    94,   447,   492,   285,
     457,   451,   451,   375,   484,   522,   356,   457,   358,   489,
     392,   361,   362,   451,   494,   495,   366,   162,   163,   164,
     165,   166,   167,   322,   169,   170,   298,   480,   423,   509,
     510,   381,   314,   383,   203,   385,   516,   321,    92,   394,
     321,    60,   522,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,   413,   252,   362,   266,   385,    -1,    88,
     420,    -1,   422,    -1,    -1,    -1,    -1,    10,    11,    12,
      13,    14,   432,    16,    -1,    18,    19,    -1,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    -1,   243,    -1,
      -1,    34,    35,    88,   249,    -1,    39,    40,    41,    -1,
      -1,    -1,    45,    46,    -1,   260,    -1,    -1,    -1,    -1,
     265,    -1,    -1,    -1,    -1,   270,    -1,    -1,    -1,    -1,
      -1,    -1,   482,    66,    -1,    -1,    75,    76,    77,    78,
      79,    80,    81,    76,    77,    -1,    -1,    80,    81,    88,
      -1,    84,   297,    86,    87,    88,    -1,    90,    -1,    92,
      -1,    -1,    -1,    96,    -1,    -1,    -1,    -1,   313,    -1,
      -1,    -1,    -1,   318,    -1,    -1,    -1,    -1,    -1,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,    -1,
      -1,    -1,   357,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,
      -1,   386,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    19,    -1,    -1,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,   416,    -1,    39,    40,    41,   421,    -1,    -1,    45,
      46,    47,    -1,    10,    11,    12,    13,    14,    -1,    16,
     435,    18,    -1,    -1,    -1,   440,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,   461,   462,    84,    -1,
      86,    87,    88,    -1,    90,    -1,    92,   472,    -1,    -1,
      96,    -1,    -1,    -1,    -1,    -1,   481,    -1,     0,     1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    16,    -1,    18,    19,    20,    21,
     505,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    45,    46,    47,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    66,    -1,    -1,    -1,    -1,    -1,
      88,    -1,    -1,    -1,    76,    77,    -1,    -1,    80,    81,
      -1,    -1,    84,    -1,    86,    87,    88,    -1,    90,    91,
      -1,    -1,    -1,    95,    96,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,
      45,    46,    47,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      -1,    66,    -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,
      -1,    76,    77,    -1,    -1,    80,    81,    -1,    -1,    84,
      -1,    86,    87,    88,    -1,    90,    10,    11,    12,    13,
      14,    96,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,
      24,    25,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    39,    40,    41,    -1,    -1,
      -1,    45,    46,    47,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      -1,    -1,    66,    -1,    -1,    -1,    88,    -1,    -1,    -1,
      -1,    -1,    76,    77,    -1,    -1,    80,    81,    -1,    -1,
      84,    -1,    86,    87,    88,    -1,    90,    -1,    -1,    -1,
      -1,    95,    96,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    -1,    -1,    -1,    23,    24,    25,    -1,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,    46,
      47,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    -1,    -1,    -1,    66,
      -1,    -1,    88,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      77,    -1,    -1,    80,    81,    -1,    -1,    84,    -1,    86,
      87,    88,    -1,    90,    10,    11,    12,    13,    14,    96,
      -1,    -1,    18,    19,    -1,    -1,    -1,    23,    24,    -1,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      76,    77,    18,    19,    80,    81,    -1,    -1,    84,    -1,
      86,    87,    88,    -1,    90,    -1,    -1,    -1,    34,    35,
      96,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      76,    77,    18,    19,    80,    81,    -1,    -1,    84,    -1,
      86,    87,    88,    -1,    90,    -1,    -1,    -1,    34,    35,
      96,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      76,    77,    18,    19,    80,    81,    -1,    -1,    84,    -1,
      86,    87,    88,    -1,    90,    -1,    -1,    93,    34,    35,
      96,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,    -1,    -1,    84,    -1,
      86,    87,    88,    -1,    90,    -1,    92,    -1,    -1,    -1,
      96,    10,    11,    12,    13,    14,    -1,    16,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    -1,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    10,    11,    12,    13,    14,    -1,    76,    77,    18,
      19,    80,    81,    -1,    -1,    84,    -1,    86,    87,    88,
      -1,    90,    -1,    -1,    -1,    34,    35,    96,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    47,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    10,    11,    12,    13,    14,    -1,    76,    77,    18,
      19,    80,    81,    -1,    -1,    84,    -1,    86,    87,    88,
      -1,    90,    -1,    -1,    -1,    34,    35,    96,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,
      -1,    80,    81,    -1,    -1,    84,    -1,    86,    87,    88,
      -1,    90,    -1,    -1,    93,    -1,    -1,    96,    10,    11,
      12,    13,    14,    -1,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    34,    35,    -1,    -1,    -1,    39,    40,    41,
      -1,    -1,    -1,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    10,    11,
      12,    13,    14,    -1,    76,    77,    18,    19,    80,    81,
      -1,    -1,    84,    -1,    86,    87,    88,    -1,    90,    -1,
      -1,    -1,    34,    35,    96,    -1,    -1,    39,    40,    41,
      -1,    -1,    -1,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    10,    11,
      12,    13,    14,    -1,    76,    77,    18,    19,    80,    81,
      -1,    -1,    84,    -1,    86,    87,    88,    -1,    90,    -1,
      -1,    -1,    34,    35,    96,    -1,    -1,    39,    40,    41,
      -1,    -1,    -1,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    76,    77,    -1,    -1,    80,    81,
      -1,    -1,    84,    -1,    86,    87,    88,    -1,    90,    -1,
      -1,    -1,    -1,    -1,    96,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    -1,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,    -1,    -1,
      94,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    -1,    -1,    -1,    -1,    93,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,    -1,
      93,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    -1,    -1,    -1,    -1,    93,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,    -1,
      93,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    -1,    -1,    -1,    92,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    -1,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    88
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    98,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    16,    18,    19,    20,
      21,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      45,    46,    47,    66,    76,    77,    80,    81,    84,    86,
      87,    88,    90,    91,    95,    96,   101,   102,   103,   104,
     105,   106,   107,   109,   110,   111,   112,   115,   116,   120,
     138,   139,   140,   147,   148,   149,   150,   153,   154,   155,
     156,   158,   159,   160,   161,   167,   168,   169,   170,   171,
     174,   176,   177,   181,   183,   184,   185,   186,   188,   189,
     194,   195,   196,   197,   198,   200,    91,    92,   100,    11,
      15,    16,    15,    15,    16,    15,    16,    87,    89,    89,
      10,    11,    12,    13,    14,    16,    18,   121,   122,   123,
     199,    18,   199,    10,    11,    12,    14,   157,    61,   207,
      92,    99,    99,   207,    88,   207,    87,   108,   207,    36,
     187,   207,   148,   150,   148,   148,    46,   148,    45,   148,
     147,   150,   184,   184,   184,   184,   184,   184,   147,   184,
     184,   100,   100,    99,    47,    87,   141,   141,    13,    18,
      19,    87,   145,   146,   150,   199,    66,    77,   152,    11,
      16,    18,    22,    96,   201,    87,    87,   153,    87,   178,
     179,   178,   154,   172,   176,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    99,   188,   200,
     200,    19,    61,   184,   190,   191,   193,    19,    93,   184,
     203,   204,   210,    49,   201,    47,    61,   210,    61,    99,
     184,    18,    19,   151,    94,   184,   207,    16,    18,    19,
     184,   205,   206,   187,   199,    13,    19,    37,    38,   187,
     148,   148,    93,    93,     1,   117,   118,   119,   199,    93,
     142,   143,   144,   147,   210,   210,   141,   184,    48,   210,
     199,    49,   207,    66,    77,    18,    87,   207,    92,   111,
     147,   162,   163,   184,   208,   209,    99,   153,   184,   154,
     174,   175,   178,    47,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   201,   201,   190,   184,    48,    92,
      94,   192,    48,    93,    93,   203,   207,    17,   137,    42,
      43,    45,   124,   125,   126,   127,   128,   148,    99,    99,
      87,    88,   114,   207,   205,    48,   184,    95,   118,    48,
      49,    41,    48,    93,    49,    10,    12,    13,    14,    16,
      19,   187,   202,    99,   181,    47,   182,    99,   210,    93,
     146,   199,    99,   207,   113,   114,   184,   199,    92,   164,
     165,   184,    48,   162,    93,   180,   154,   171,   173,   183,
     155,   181,    61,    87,    87,   184,   191,   191,   204,    93,
     184,     1,    17,    18,    42,    43,    44,    45,    95,    96,
     115,   129,   130,   131,   135,   136,   147,    48,   128,   148,
      93,   184,   184,    93,   206,   210,    10,   143,    41,   150,
     186,   181,   184,   114,   210,    93,    93,   166,   208,   208,
     164,   184,   190,   210,   141,    61,    61,    61,    96,   210,
      18,   137,    95,   130,   141,   199,   125,    94,    99,    16,
     210,    99,    47,   153,   166,   184,    99,    18,    99,   141,
     210,    99,    99,   132,   153,   137,   141,    99,    99,   137,
     137,    88,   133,   134,    99,   137,    10,    99,   134,   137,
      94,   137
};

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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
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
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
  YYUSE (yyvaluep);

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
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	struct yyalloc *yyptr =
	  (struct yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
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

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
#line 157 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Blank; }
    break;

  case 4:
#line 158 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 5:
#line 159 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return (yyvsp[(2) - (3)].ival); }
    break;

  case 6:
#line 160 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 7:
#line 163 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 8:
#line 166 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 9:
#line 167 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Err; }
    break;

  case 10:
#line 170 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 11:
#line 173 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 12:
#line 174 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 18:
#line 186 "css_parse.y"
    {
            cssEl* tmp = (yyvsp[(2) - (2)].el).El();
	    cssMisc::cur_top->SetInclude((const char*)*tmp);
	    cssEl::Done((yyvsp[(2) - (2)].el).El()); }
    break;

  case 19:
#line 192 "css_parse.y"
    {
            cssMisc::Defines.Push(new cssDef(0, (yyvsp[(2) - (2)].nm))); }
    break;

  case 21:
#line 195 "css_parse.y"
    { cssMisc::Defines.Remove((yyvsp[(2) - (2)].el).El()); }
    break;

  case 22:
#line 198 "css_parse.y"
    { /* if its a name, its not defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 24:
#line 202 "css_parse.y"
    { /* if its a def, its defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 26:
#line 208 "css_parse.y"
    { /* if you see it, it applies to u*/
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog()); }
    break;

  case 28:
#line 223 "css_parse.y"
    {
            Code1((yyvsp[(1) - (2)].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 29:
#line 225 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssRef((yyvsp[(2) - (3)].el)));
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 30:
#line 229 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc; }
    break;

  case 31:
#line 231 "css_parse.y"
    {
	    Code1((yyvsp[(1) - (1)].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 32:
#line 233 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 33:
#line 237 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 34:
#line 241 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 35:
#line 245 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 36:
#line 249 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 37:
#line 255 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 39:
#line 257 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 46:
#line 269 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(3) - (4)].ival));
	    if((yyvsp[(2) - (4)].ival) < 0) (yyval.ival) = (yyvsp[(3) - (4)].ival)-1; /* if no coding, its the end, else not */
	    else (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 47:
#line 279 "css_parse.y"
    {
 	    ((yyvsp[(1) - (6)].el).El())->MakeToken(cssMisc::cur_top->Prog());
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
#line 297 "css_parse.y"
    {	/* no type information, get from last expr */
	    cssProg* prg = cssMisc::cur_top->Prog();
	    prg->UnCode();	/* undo the end jump coding */
	    css_progdx actln = (yyvsp[(5) - (5)].ival)-2;
	    if(prg->insts[actln]->IsJump()) { /* if jmp, get before */
	      actln = (yyvsp[(3) - (5)].ival)+1;	/* go for the first thing if the last is no good */
	    }
	    cssEl* extyp = prg->insts[actln]->inst.El();
	    if(extyp->GetType() == cssEl::T_ElCFun && extyp->name == "make_matrix") {
	      extyp = cssBI::matrix_inst;
	    }
	    extyp->MakeToken(prg);
	    cssRef* tmp = (cssRef*)prg->Stack()->Pop();
	    (yyval.ival) = Code3(tmp->ptr, cssBI::init_asgn, cssBI::pop);
	    if(!cssMisc::cur_top->AmCmdProg()) {
	      cssMisc::Warning(prg,
			       "Warning: created implicit variable:",
			       tmp->ptr.El()->name,
			       "of type: ", extyp->GetTypeName()); }
	    cssEl::Done(tmp); }
    break;

  case 49:
#line 321 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (6)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->UnCode();	/* undo the end jump coding */
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(1) - (6)].el).El()); /* type is an arg */
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((yyvsp[(4) - (6)].ival))); /* num of dims */
	    cssMisc::VoidArray.tmp_str = ((yyvsp[(1) - (6)].el).El())->tmp_str;
	    cssMisc::VoidArray.MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    (yyval.ival) = Code3(tmp->ptr, cssBI::array_alloc, cssBI::pop);
	    cssEl::Done(tmp); }
    break;

  case 50:
#line 337 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 51:
#line 338 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 52:
#line 342 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 53:
#line 345 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(5) - (6)].ival)-1]->SetJump((yyvsp[(5) - (6)].ival)); (yyval.ival) = (yyvsp[(5) - (6)].ival)-1;
	    if(cssMisc::cur_enum->name.before(5) == "enum_") /* don't keep anon enums around */
	      delete cssMisc::cur_enum;
	    cssMisc::parsing_args = false; /* needed for - numbers */
	    cssMisc::cur_enum = NULL; }
    break;

  case 54:
#line 353 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_enum = new cssEnumType((const char*)*nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum);
	    cssMisc::parsing_args = true; 
	    cssEl::Done(nm); }
    break;

  case 55:
#line 361 "css_parse.y"
    { /* redefining */
	    if((yyvsp[(2) - (2)].el).El()->GetType() != cssEl::T_EnumType) {
	      yyerror("Attempt to redefine non-enum type as an enum");
	      return cssProg::YY_Err; }
   	    cssMisc::cur_enum = (cssEnumType*)(yyvsp[(2) - (2)].el).El();
 	    cssMisc::cur_enum->enums->Reset(); /* prepare for new enums.. */
	    cssMisc::parsing_args = true; 
	    cssMisc::cur_enum->enum_cnt = 0; }
    break;

  case 56:
#line 369 "css_parse.y"
    { /* anonymous  */
	    String nm = "anonenum_"; nm += String(cssMisc::anon_type_cnt++); nm += "_";
            cssMisc::cur_enum = new cssEnumType((const char*)nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum);
	    cssMisc::parsing_args = true; 
	  }
    break;

  case 61:
#line 385 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 62:
#line 388 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssSpace* en_own;
	    if(cssMisc::cur_class != NULL) en_own = cssMisc::cur_class->types;
	    /* todo: global keyword??   else en_own = &(cssMisc::Enums); */
	    else en_own = &(cssMisc::cur_top->enums);
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
#line 406 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssSpace* en_own;
	    if(cssMisc::cur_class != NULL) en_own = cssMisc::cur_class->types;
	    /* todo: global keyword??   else en_own = &(cssMisc::Enums); */
	    else en_own = &(cssMisc::cur_top->enums);
	    cssElPtr itm_ptr = en_own->FindName((const char*)*nm);	cssEnum* itm;
	    if(itm_ptr != 0) { /* redef */
	      itm = (cssEnum*)itm_ptr.El();
	      itm->val = (int)*((yyvsp[(3) - (3)].el).El());
	      itm->SetEnumType(cssMisc::cur_enum);
	    }
	    else {
	      itm = new cssEnum(cssMisc::cur_enum, (int)*((yyvsp[(3) - (3)].el).El()), (const char*)*nm);
	      en_own->Push(itm);
	    }
	    cssMisc::cur_enum->enum_cnt = itm->val + 1;
	    cssMisc::cur_enum->enums->Push(itm); cssEl::Done(nm); }
    break;

  case 64:
#line 427 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(7) - (8)].ival)-1]->SetJump((yyvsp[(7) - (8)].ival)); (yyval.ival) = (yyvsp[(7) - (8)].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[(4) - (8)].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 65:
#line 431 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(6) - (7)].ival)-1]->SetJump((yyvsp[(6) - (7)].ival)); (yyval.ival) = (yyvsp[(6) - (7)].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[(4) - (7)].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 66:
#line 435 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(3) - (4)].ival)); (yyval.ival) = (yyvsp[(3) - (4)].ival)-1;
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 69:
#line 445 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssMisc::cur_class = new cssClassType((const char*)*nm);
	    cssMisc::cur_class->last_top = cssMisc::cur_top;
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
	    cssEl::Done(nm); }
    break;

  case 70:
#line 452 "css_parse.y"
    { /* redefining */
	    if((yyvsp[(1) - (1)].el).El()->GetType() != cssEl::T_ClassType) {
	      yyerror("Attempt to redefine non-class type as a class");
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_class = (cssClassType*)(yyvsp[(1) - (1)].el).El();
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
#line 474 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_class = new cssClassType((const char*)*nm);
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
            cssEl::Done(nm); }
    break;

  case 72:
#line 480 "css_parse.y"
    {
            if((yyvsp[(1) - (1)].el).El()->GetType() != cssEl::T_ClassType) {
              yyerror("Attempt to redefine non-class type as a class");
              return cssProg::YY_Err;
            }
            cssMisc::cur_class = (cssClassType*)(yyvsp[(1) - (1)].el).El(); }
    break;

  case 75:
#line 492 "css_parse.y"
    {
            if((yyvsp[(1) - (1)].el).El()->GetType() != cssEl::T_ClassType) {
	      yyerror("Cannot inherit from non-class type");
	      /* cssMisc::TypesSpace.Remove(cssMisc::cur_class); */
	      cssMisc::cur_top->types.Remove(cssMisc::cur_class);
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_class->AddParent((cssClassType*)((yyvsp[(1) - (1)].el).El())); }
    break;

  case 77:
#line 503 "css_parse.y"
    { (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 86:
#line 520 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 87:
#line 521 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 91:
#line 525 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 92:
#line 528 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
            cssMisc::parsing_membdefn = true;
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssClassMember* mbr = new cssClassMember((yyvsp[(1) - (4)].el).El(), (const char*)*nm);
            cssMisc::cur_class->members->Push(mbr);
            cssMisc::cur_class->GetComments(mbr, (yyvsp[(4) - (4)].el));
            cssMisc::parsing_membdefn = false;
            cssEl::Done(nm); }
    break;

  case 93:
#line 540 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (6)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
            cssMisc::parsing_membdefn = true;
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((yyvsp[(4) - (6)].ival))); /* num of dims */
            cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(1) - (6)].el).El()); /* type is an arg */
            cssMisc::VoidArrayType.MakeTempToken(cssMisc::cur_top->Prog());
            cssArrayType* tmp = (cssArrayType*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssClassMember* mbr = new cssClassMember(tmp, tmp->name);
            cssMisc::cur_class->members->Push(mbr);
            cssMisc::cur_class->GetComments(mbr, (yyvsp[(6) - (6)].el));
            cssMisc::parsing_membdefn = false; }
    break;

  case 94:
#line 556 "css_parse.y"
    {
            /* swap places with void marker */
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_top->Prog()->Stack()->Push(&cssMisc::Void);
            cssMisc::cur_top->Prog()->Stack()->Push(nm); }
    break;

  case 95:
#line 564 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 96:
#line 565 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 97:
#line 569 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((int)*(yyvsp[(2) - (3)].el).El())); }
    break;

  case 98:
#line 573 "css_parse.y"
    {       /* pre-declare function */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[(1) - (4)].el).El();
            cssMisc::cur_class->GetComments(fun, (yyvsp[(4) - (4)].el));
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr, true); }
    break;

  case 99:
#line 579 "css_parse.y"
    {       /* constructor */
	    if((yyvsp[(1) - (4)].el).El() != cssMisc::cur_class) {
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
	    fun->argc = (yyvsp[(2) - (4)].ival) + 1;	      	fun->GetArgDefs();
            cssMisc::cur_class->GetComments(fun, (yyvsp[(4) - (4)].el)); }
    break;

  case 100:
#line 595 "css_parse.y"
    {       /* destructor */
	    if((yyvsp[(2) - (5)].el).El() != cssMisc::cur_class) {
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
	    fun->argc = (yyvsp[(3) - (5)].ival) + 1;		fun->GetArgDefs();
            cssMisc::cur_class->GetComments(fun, (yyvsp[(5) - (5)].el));}
    break;

  case 101:
#line 612 "css_parse.y"
    {       /* destructor */
	    if((yyvsp[(3) - (6)].el).El() != cssMisc::cur_class) {
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
	    fun->argc = (yyvsp[(4) - (6)].ival) + 1;		fun->GetArgDefs();
            cssMisc::cur_class->GetComments(fun, (yyvsp[(6) - (6)].el));}
    break;

  case 102:
#line 632 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (2)].el).El()->tmp_str == "const") {
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
	    if((yyvsp[(1) - (2)].el).El()->tmp_str == "virtual") fun->is_virtual = true;
	    fun->SetRetvType((yyvsp[(1) - (2)].el).El()); /* preserve type info for later if nec */
            cssEl::Done(nm); }
    break;

  case 103:
#line 653 "css_parse.y"
    { (yyval.el).Reset(); }
    break;

  case 104:
#line 654 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (1)].el); }
    break;

  case 105:
#line 655 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (2)].el); }
    break;

  case 106:
#line 658 "css_parse.y"
    {	/* pre-declare function */
            cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(3) - (4)].ival));	(yyval.ival) = (yyvsp[(3) - (4)].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[(1) - (4)].el).El();
	    fun->argc = (yyvsp[(2) - (4)].ival); fun->GetArgDefs(); }
    break;

  case 107:
#line 664 "css_parse.y"
    {
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[(1) - (4)].el).El();
	    cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(4) - (4)].ival)+1); (yyval.ival) = (yyvsp[(3) - (4)].ival)-1;
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    cssCodeBlock* bra_blk = (cssCodeBlock*)(cssMisc::cur_top->Prog()->insts[(yyvsp[(4) - (4)].ival)]->inst.El());
	    cssMisc::cur_top->Prog()->insts[(yyvsp[(4) - (4)].ival)]->SetInst((yyvsp[(1) - (4)].el)); /* replace bra_blk with fun */
	    cssMisc::cur_top->RemoveVar(bra_blk);	/* get rid of the bra_blk */
	    cssMisc::cur_top->SetPush(fun->fun); /* this is the one we want to push */
	    fun->Define(cssMisc::ConstExpr); }
    break;

  case 108:
#line 676 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 109:
#line 684 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (5)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->insts[(yyvsp[(4) - (5)].ival)-1]->SetJump((yyvsp[(5) - (5)].ival)+1);	(yyval.ival) = (yyvsp[(4) - (5)].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[(2) - (5)].el).El();
	    fun->SetRetvType((yyvsp[(1) - (5)].el).El());
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    cssCodeBlock* bra_blk = (cssCodeBlock*)(cssMisc::cur_top->Prog()->insts[(yyvsp[(5) - (5)].ival)]->inst.El());
	    cssMisc::cur_top->Prog()->insts[(yyvsp[(5) - (5)].ival)]->SetInst((yyvsp[(2) - (5)].el)); /* replace bra with existing one */
	    cssMisc::cur_top->RemoveVar(bra_blk);	/* get rid of the bra_blk */
	    fun->fun->Reset();	/* reset it for new defn */
	    cssMisc::cur_top->SetPush(fun->fun); /* push this one */
	    fun->Define(cssMisc::ConstExpr); }
    break;

  case 110:
#line 702 "css_parse.y"
    {
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[(1) - (4)].el).El();
	    cssClassType* cls = cssMisc::cur_class;
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr);
	    cssMisc::cur_method = fun; /* this is now the current method */
	    cssElPtr fun_ptr;  fun_ptr.SetNVirtMethod(cls, cls->methods->GetIndex(fun));
	    css_progdx nxt_ln = Code1(fun_ptr); /* code it so it shows up in a listing.. */
	    cssMisc::cur_top->SetPush(fun->fun); /* put it on the stack.. */
	    cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump(nxt_ln+1); (yyval.ival) = (yyvsp[(3) - (4)].ival)-1; }
    break;

  case 111:
#line 715 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (2)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssEl* nm = cssMisc::cur_top->Prog()->Stack()->Pop();  /* get rid of name */
	    cssScriptFun* fun = new cssScriptFun((const char*)*nm);
	    fun->SetRetvType((yyvsp[(1) - (2)].el).El()); /* preserve type info for later if nec */
	    if((yyvsp[(1) - (2)].el).El()->tmp_str == "extern") (yyval.el) = cssMisc::Externs.PushUniqNameOld(fun);
	    else (yyval.el) = cssMisc::cur_top->AddStatic(fun);
            cssEl::Done(nm); }
    break;

  case 112:
#line 728 "css_parse.y"
    {
            cssMisc::cur_scope = NULL;
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (3)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if(((yyvsp[(2) - (3)].el).El())->GetType() != cssEl::T_ClassType) {
	      yyerror("attempt to define member function in non-class type");
	      return cssProg::YY_Err; }
	    cssClassType* cls = (cssClassType*)(yyvsp[(2) - (3)].el).El();
	    cssMisc::cur_class = cls; /* this is now the current class */
	    cssEl* nm = cssMisc::cur_top->Prog()->Stack()->Pop();  /* get rid of name */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMethodFmName((const char*)*nm);
	    if(fun == &cssMisc::Void) {
	      yyerror("member function not declared in class type");
	      return cssProg::YY_Err; }
            if(fun->GetType() == cssEl::T_MbrCFun) {
              yyerror("member function is hardcoded");
              return cssProg::YY_Err; }
	    if((fun->retv_type != (yyvsp[(1) - (3)].el).El())
	       && !(fun->retv_type->IsRef() || fun->retv_type->GetType() == cssEl::T_Ptr))
	      cssMisc::Warning(NULL, "return type does not match that of declaration");
	    fun->SetRetvType((yyvsp[(1) - (3)].el).El());
	    (yyval.el).SetDirect(fun);  cssEl::Done(nm); }
    break;

  case 113:
#line 754 "css_parse.y"
    {
            cssMisc::cur_scope = NULL;
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (2)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if(((yyvsp[(1) - (2)].el).El())->GetType() != cssEl::T_ClassType) {
	      yyerror("attempt to define constructor member function in non-class type");
	      return cssProg::YY_Err; }
	    cssClassType* cls = (cssClassType*)(yyvsp[(1) - (2)].el).El();
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
#line 775 "css_parse.y"
    {
            cssMisc::cur_scope = NULL;
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (3)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if(((yyvsp[(1) - (3)].el).El())->GetType() != cssEl::T_ClassType) {
	      yyerror("attempt to define destructor member function in non-class type");
	      return cssProg::YY_Err; }
	    cssClassType* cls = (cssClassType*)(yyvsp[(1) - (3)].el).El();
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
#line 797 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 116:
#line 798 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 117:
#line 799 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 118:
#line 800 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 119:
#line 803 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 120:
#line 804 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 121:
#line 807 "css_parse.y"
    { cssMisc::parsing_args = false; }
    break;

  case 122:
#line 808 "css_parse.y"
    {
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push((yyvsp[(3) - (3)].el).El()); }
    break;

  case 123:
#line 811 "css_parse.y"
    {
            cssEl* itm = (yyvsp[(3) - (4)].el).El()->GetScoped((yyvsp[(4) - (4)].nm));
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push(itm); cssMisc::cur_scope = NULL; }
    break;

  case 124:
#line 818 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (2)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[(2) - (2)].nm)));
	    ((yyvsp[(1) - (2)].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 125:
#line 826 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (2)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[(2) - (2)].el).El()->GetName()));
	    ((yyvsp[(1) - (2)].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 127:
#line 837 "css_parse.y"
    {
	    if(((yyvsp[(1) - (3)].ival) < 0) && ((yyvsp[(3) - (3)].ival) < 0)) (yyval.ival) = -1;
	    else if(((yyvsp[(1) - (3)].ival) >= 0) && ((yyvsp[(3) - (3)].ival) < 0)) (yyval.ival) = (yyvsp[(1) - (3)].ival);
	    else if(((yyvsp[(1) - (3)].ival) < 0) && ((yyvsp[(3) - (3)].ival) >= 0)) (yyval.ival) = (yyvsp[(3) - (3)].ival);
	    else (yyval.ival) = -1; }
    break;

  case 128:
#line 844 "css_parse.y"
    {
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      (yyval.ival) = Code2(tmp->ptr, cssBI::constr); }
	    else (yyval.ival) = -1;
	    cssEl::Done(tmp); }
    break;

  case 129:
#line 853 "css_parse.y"
    { (yyvsp[(1) - (1)].el).El()->tmp_str = ""; }
    break;

  case 130:
#line 854 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "extern"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 131:
#line 855 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "static"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 132:
#line 856 "css_parse.y"
    { /* use const expr prog to code stuff */
	  (yyvsp[(2) - (2)].el).El()->tmp_str = "const"; (yyval.el) = (yyvsp[(2) - (2)].el); cssMisc::CodeConstExpr(); }
    break;

  case 133:
#line 858 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 134:
#line 859 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = ""; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 135:
#line 860 "css_parse.y"
    { (yyvsp[(3) - (3)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(3) - (3)].el); }
    break;

  case 136:
#line 861 "css_parse.y"
    { (yyvsp[(3) - (3)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(3) - (3)].el); }
    break;

  case 137:
#line 864 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (1)].el); cssMisc::cur_type = (yyval.el); }
    break;

  case 138:
#line 865 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->GetPtrType((yyvsp[(1) - (2)].el).El(), (yyvsp[(2) - (2)].ival)); cssMisc::cur_type = (yyval.el); }
    break;

  case 139:
#line 867 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->GetRefType((yyvsp[(1) - (2)].el).El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 140:
#line 869 "css_parse.y"
    {
	    cssElPtr pt = cssMisc::cur_top->GetPtrType((yyvsp[(1) - (3)].el).El(), (yyvsp[(2) - (3)].ival));
            (yyval.el) = cssMisc::cur_top->GetRefType(pt.El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 143:
#line 876 "css_parse.y"
    {
          cssEl* itm;
          if((yyvsp[(1) - (2)].el).El()->GetParse() != CSS_PTR)
	    itm = (yyvsp[(1) - (2)].el).El()->GetScoped((const char*)(yyvsp[(2) - (2)].el).El()->name);
	  else
	    itm = (yyvsp[(2) - (2)].el).El();
	  if(itm != &cssMisc::Void)  (yyval.el).SetDirect(itm);
	  else			(yyval.el).Reset();
	  cssMisc::cur_type = (yyval.el); cssMisc::cur_scope = NULL; }
    break;

  case 144:
#line 887 "css_parse.y"
    {
          if(((yyvsp[(1) - (2)].el).El()->GetType() != cssEl::T_ClassType) && ((yyvsp[(1) - (2)].el).El()->GetType() != cssEl::T_EnumType)) {
	     yyerror("scoping of non-class or enum type");
	     return cssProg::YY_Err; }
           cssMisc::cur_scope = (yyvsp[(1) - (2)].el).El(); }
    break;

  case 145:
#line 892 "css_parse.y"
    {
           cssMisc::cur_scope = (yyvsp[(1) - (2)].el).El(); }
    break;

  case 148:
#line 900 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 149:
#line 901 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 152:
#line 906 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastif = -1; }
    break;

  case 155:
#line 913 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 159:
#line 917 "css_parse.y"
    { Code1((yyvsp[(1) - (3)].el)); (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 160:
#line 918 "css_parse.y"
    { Code1((yyvsp[(1) - (4)].el)); (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 161:
#line 919 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (2)].el)); }
    break;

  case 162:
#line 920 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (2)].el)); }
    break;

  case 165:
#line 925 "css_parse.y"
    {
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; // next instr is 'it'
  	    if(cssMisc::cur_top->Prog()->name != cssSwitchBlock_Name) {
	      yyerror("case statement not in a switch block");
	      return cssProg::YY_Err;
	    }
	    cssElPtr aryptr = cssMisc::cur_top->Prog()->literals.FindName(cssSwitchJump_Name);
	    cssArray* val_ary = (cssArray*)aryptr.El();
	    val_ary->items->Push(new cssInt((yyval.ival), (const char*)*((yyvsp[(2) - (3)].el).El()))); }
    break;

  case 166:
#line 934 "css_parse.y"
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

  case 171:
#line 953 "css_parse.y"
    {
            (yyval.ival) = (yyvsp[(1) - (6)].ival);
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

  case 172:
#line 970 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssSwitchBlock_Name, cssMisc::cur_top->Prog());
	    blk->loop_type = cssCodeBlock::SWITCH;
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 173:
#line 978 "css_parse.y"
    {
	   cssProg* cp = cssMisc::cur_top->Prog();
	   if(cp->owner_blk == NULL) {
	     yyerror("for loop current prog should have owner_blk, doesnt!");
	   }
	   cssMisc::cur_top->Pop(); /* pop the for_incr block */
	   Code1(cssBI::pop);	/* and code for getting rid of cond val */
	   cp = cssMisc::cur_top->Prog(); /* current guy */
	   cssInst* for_incr = cp->insts[(yyvsp[(4) - (6)].ival)];
	   cssInst* for_loop_stmt = cp->insts[(yyvsp[(5) - (6)].ival)];
	   /* swap */
	   cp->insts[(yyvsp[(4) - (6)].ival)] = for_loop_stmt;
	   cp->insts[(yyvsp[(5) - (6)].ival)] = for_incr;
	   cssMisc::cur_top->Pop(); /* pop the whole for loop! */
	   (yyval.ival) = (yyvsp[(1) - (6)].ival); }
    break;

  case 174:
#line 993 "css_parse.y"
    {
	   cssProg* cp = cssMisc::cur_top->Prog();
	   if(cp->owner_blk == NULL) {
	     yyerror("for loop current prog should have owner_blk, doesnt!");
	   }
	   cssMisc::cur_top->Pop(); /* pop the for_incr block */
	   Code1(cssBI::pop);	/* and code for getting rid of cond val */
	   /* swap the order of these: $6 = for_end_paren = stmt block,  $5 = for_incr = incr block */
	   cp = cssMisc::cur_top->Prog(); /* current guy */
	   cssInst* for_incr = cp->insts[(yyvsp[(5) - (7)].ival)];
	   cssInst* for_loop_stmt = cp->insts[(yyvsp[(6) - (7)].ival)];
	   /* swap */
	   cp->insts[(yyvsp[(5) - (7)].ival)] = for_loop_stmt;
	   cp->insts[(yyvsp[(6) - (7)].ival)] = for_incr;
	   /* check if stmt is a new block: if so, then don't pop this guy */
	   
	   cssMisc::cur_top->Pop(); /* pop the whole for loop! */
	   (yyval.ival) = (yyvsp[(1) - (7)].ival); }
    break;

  case 175:
#line 1016 "css_parse.y"
    { /* for loop contained within own block */
            cssCodeBlock* blk = new cssCodeBlock(cssForLoop_Name, cssMisc::cur_top->Prog());
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 178:
#line 1027 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 181:
#line 1038 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of cond, push incr */
	      cssMisc::cur_top->ResetParseFlags();
	      cssCodeBlock* blk = new cssCodeBlock(cssForIncr_Name, cssMisc::cur_top->Prog());
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_back = 3; /* go back 3 to the cond */
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 182:
#line 1048 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of incr */
	      cssCodeBlock* blk = new cssCodeBlock(cssForLoopStmt_Name, cssMisc::cur_top->Prog());
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_type = cssCodeBlock::FOR;
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 184:
#line 1061 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssDoLoop_Name, cssMisc::cur_top->Prog());
	    blk->loop_type = cssCodeBlock::DO;
	    cssMisc::cur_top->AddStatic(blk); /* while is all inside this do! */
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 185:
#line 1070 "css_parse.y"
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
	      (yyval.ival) = (yyvsp[(2) - (3)].ival);
	    } }
    break;

  case 186:
#line 1093 "css_parse.y"
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
	  (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 187:
#line 1107 "css_parse.y"
    { }
    break;

  case 188:
#line 1110 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 189:
#line 1115 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          cssMisc::cur_top->Prog()->lastelseif = false; /* use this flag! */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 190:
#line 1119 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          cssMisc::cur_top->Prog()->lastelseif = true; /* be on guard for another else! */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); cssMisc::cur_top->DoCompileCtrl(); /* need to do the bra! */ }
    break;

  case 194:
#line 1130 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); /* pop the if_false block */
	    /* now check for other else blocks that need popping! */
	    cssMisc::cur_top->PopElseBlocks(); /* pop residual elses! */
	    (yyval.ival) = (yyvsp[(1) - (2)].ival);  }
    break;

  case 195:
#line 1135 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[(1) - (4)].ival);  }
    break;

  case 196:
#line 1137 "css_parse.y"
    { /* do not pop if going into another else */
	  (yyval.ival) = (yyvsp[(1) - (3)].ival);  }
    break;

  case 197:
#line 1141 "css_parse.y"
    {
	  if(!cssMisc::cur_top->Prog()->lastelseif) {
	    cssMisc::cur_top->PopElseBlocks(); } }
    break;

  case 198:
#line 1146 "css_parse.y"
    {
          cssMisc::cur_top->Prog()->lastif = cssMisc::cur_top->Prog()->size; }
    break;

  case 199:
#line 1150 "css_parse.y"
    {
  	    css_progdx tmp = cssMisc::cur_top->Prog()->lastif;
  	    if(tmp < 0) {
/*  	      cssProg* tmprg = cssMisc::cur_top->PrvProg(); /\* also look up one level *\/ */
/*  	      if(tmprg) tmp = tmprg->lastif; */
	      if(tmp < 0) {
		yyerror("else without matching if");
		return cssProg::YY_Err;
	      }
	    }
	    cssMisc::cur_top->Prog()->lastif = -1; /* reset it */
            cssCodeBlock* blk = new cssCodeBlock(cssElseBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::ELSE; /* start block of else */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code2(blk, cssBI::pop); /* pop after else to get rid of prev if cond */
	    cssMisc::cur_top->Push(blk->code); }
    break;

  case 200:
#line 1168 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssIfTrueBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	    cssMisc::cur_top->AddStatic(blk);
	    Code1(blk); cssMisc::cur_top->Push(blk->code); (yyval.ival) = (yyvsp[(1) - (3)].ival); }
    break;

  case 201:
#line 1175 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 202:
#line 1182 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); }
    break;

  case 203:
#line 1186 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssBlock_Name, cssMisc::cur_top->Prog());
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->SetPush(blk->code); }
    break;

  case 204:
#line 1192 "css_parse.y"
    {  }
    break;

  case 205:
#line 1195 "css_parse.y"
    {
	    cssScriptFun* sfun = cssMisc::cur_top->Prog()->owner_fun;
	    if((sfun != NULL) && (sfun->GetType() == cssEl::T_MbrScriptFun)) {
	      cssMisc::cur_class = NULL; /* get rid of current class pointer.. */
	      cssMisc::cur_method = NULL; } /* and current method pointer */
	    cssProg* prv = cssMisc::cur_top->PrvProg();
	    if(prv && prv->lastelseif) {
	      /* now need to check if the next token is an else -- if so, then no pops! */
	      bool got_else = cssMisc::cur_top->ParseElseCheck();
	      if(!got_else) {
		cssMisc::cur_top->Pop(); /* get rid of this guy now */
		cssMisc::cur_top->PopElseBlocks(); /* get rid of this guy now */
	      }
	      else {
		cssMisc::cur_top->SetPop();
	      }
	    }
	    else {
	      cssMisc::cur_top->SetPop();
	    }
	  }
    break;

  case 206:
#line 1218 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 209:
#line 1224 "css_parse.y"
    { Code1(cssBI::asgn); }
    break;

  case 210:
#line 1225 "css_parse.y"
    {
	    int c; String inp;
	    while(((c = cssMisc::cur_top->Getc()) != '}') && (c != EOF)) inp += (char)c;
	    cssSStream* ss = new cssSStream();
	    cssMisc::cur_top->AddLiteral(ss);
 	    stringstream* sss = (stringstream*)ss->GetVoidPtr();
	    *sss << inp;
	    sss->seekg(0, ios::beg);
	    Code3(ss, cssBI::arg_swap, cssBI::rshift);
 	    Code1(cssBI::sstream_rewind); /* rewind stream for next use.. */ }
    break;

  case 211:
#line 1235 "css_parse.y"
    { Code1(cssBI::asgn_add); }
    break;

  case 212:
#line 1236 "css_parse.y"
    { Code1(cssBI::asgn_sub); }
    break;

  case 213:
#line 1237 "css_parse.y"
    { Code1(cssBI::asgn_mult); }
    break;

  case 214:
#line 1238 "css_parse.y"
    { Code1(cssBI::asgn_div); }
    break;

  case 215:
#line 1239 "css_parse.y"
    { Code1(cssBI::asgn_mod); }
    break;

  case 216:
#line 1240 "css_parse.y"
    { Code1(cssBI::asgn_lshift); }
    break;

  case 217:
#line 1241 "css_parse.y"
    { Code1(cssBI::asgn_rshift); }
    break;

  case 218:
#line 1242 "css_parse.y"
    { Code1(cssBI::asgn_and); }
    break;

  case 219:
#line 1243 "css_parse.y"
    { Code1(cssBI::asgn_xor); }
    break;

  case 220:
#line 1244 "css_parse.y"
    { Code1(cssBI::asgn_or); }
    break;

  case 221:
#line 1245 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code2((yyvsp[(3) - (3)].el),(yyvsp[(1) - (3)].el)); }
    break;

  case 222:
#line 1246 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (5)].ival); Code2((yyvsp[(3) - (5)].el),(yyvsp[(1) - (5)].el)); }
    break;

  case 223:
#line 1247 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[(2) - (4)].ival); Code2((yyvsp[(3) - (4)].el),(yyvsp[(1) - (4)].el)); }
    break;

  case 224:
#line 1249 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code1((yyvsp[(1) - (3)].el)); }
    break;

  case 225:
#line 1250 "css_parse.y"
    { (yyval.ival) = (yyvsp[(4) - (5)].ival); Code1((yyvsp[(1) - (5)].el)); }
    break;

  case 226:
#line 1251 "css_parse.y"
    { Code1(cssBI::lor); }
    break;

  case 227:
#line 1252 "css_parse.y"
    { Code1(cssBI::land); }
    break;

  case 228:
#line 1253 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::addr_of); }
    break;

  case 229:
#line 1254 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::de_ptr); }
    break;

  case 230:
#line 1255 "css_parse.y"
    { Code1(cssBI::gt); }
    break;

  case 231:
#line 1256 "css_parse.y"
    { Code1(cssBI::ge); }
    break;

  case 232:
#line 1257 "css_parse.y"
    { Code1(cssBI::lt); }
    break;

  case 233:
#line 1258 "css_parse.y"
    { Code1(cssBI::le); }
    break;

  case 234:
#line 1259 "css_parse.y"
    { Code1(cssBI::eq); }
    break;

  case 235:
#line 1260 "css_parse.y"
    { Code1(cssBI::ne); }
    break;

  case 236:
#line 1261 "css_parse.y"
    { Code1(cssBI::cond); }
    break;

  case 237:
#line 1262 "css_parse.y"
    { Code1(cssBI::add); }
    break;

  case 238:
#line 1263 "css_parse.y"
    { Code1(cssBI::sub); }
    break;

  case 239:
#line 1264 "css_parse.y"
    { Code1(cssBI::mul); }
    break;

  case 240:
#line 1265 "css_parse.y"
    { Code1(cssBI::div); }
    break;

  case 241:
#line 1266 "css_parse.y"
    { Code1(cssBI::modulo); }
    break;

  case 242:
#line 1267 "css_parse.y"
    { Code1(cssBI::lshift); }
    break;

  case 243:
#line 1268 "css_parse.y"
    { Code1(cssBI::rshift); }
    break;

  case 244:
#line 1269 "css_parse.y"
    { Code1(cssBI::bit_and); }
    break;

  case 245:
#line 1270 "css_parse.y"
    { Code1(cssBI::bit_xor); }
    break;

  case 246:
#line 1271 "css_parse.y"
    { Code1(cssBI::bit_or); }
    break;

  case 247:
#line 1272 "css_parse.y"
    { Code1(cssBI::asgn_post_pp); }
    break;

  case 248:
#line 1273 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::asgn_pre_pp); }
    break;

  case 249:
#line 1274 "css_parse.y"
    { Code1(cssBI::asgn_post_mm); }
    break;

  case 250:
#line 1275 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::asgn_pre_mm); }
    break;

  case 251:
#line 1276 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::neg); }
    break;

  case 252:
#line 1277 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::lnot); }
    break;

  case 253:
#line 1278 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::bitneg); }
    break;

  case 254:
#line 1279 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (1)].ival); }
    break;

  case 255:
#line 1280 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival); Code1(cssBI::de_array); }
    break;

  case 256:
#line 1282 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(2) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = (yyvsp[(4) - (4)].ival); Code2((yyvsp[(2) - (4)].el), cssBI::cast); }
    break;

  case 257:
#line 1288 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
  	    (yyval.ival) = (yyvsp[(3) - (4)].ival); Code2((yyvsp[(1) - (4)].el), cssBI::cast); }
    break;

  case 259:
#line 1295 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 265:
#line 1308 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code1(cssBI::make_matrix);
	    int act_args = (yyvsp[(2) - (3)].ival);
	    if(act_args > cssElFun::ArgMax) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(), "Too many initializer values for matrix, should have at most:", String(cssElFun::ArgMax), "got:",String(act_args)); }
	  }
    break;

  case 266:
#line 1316 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); /* an arg stop */ }
    break;

  case 267:
#line 1319 "css_parse.y"
    { (yyval.ival) = Code1(cssBI::colon_mark); }
    break;

  case 269:
#line 1323 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 270:
#line 1324 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (1)].ival); Code1(cssBI::colon_end_mark); }
    break;

  case 271:
#line 1325 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival); Code1(cssBI::colon_end_mark); }
    break;

  case 272:
#line 1326 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival); Code1(cssBI::colon_end_mark); }
    break;

  case 273:
#line 1327 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival); Code1(cssBI::colon_end_mark); }
    break;

  case 274:
#line 1328 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (4)].ival); Code1(cssBI::colon_end_mark); }
    break;

  case 275:
#line 1329 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (5)].ival); Code1(cssBI::colon_end_mark); }
    break;

  case 276:
#line 1332 "css_parse.y"
    { (yyval.ival) = Code1(cssBI::semicolon_mark); }
    break;

  case 277:
#line 1335 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 278:
#line 1336 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 279:
#line 1337 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 2; }
    break;

  case 280:
#line 1341 "css_parse.y"
    { (yyval.ival) = Code2(cssBI::push_next, (yyvsp[(1) - (1)].el)); }
    break;

  case 281:
#line 1342 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].el_ival).ival; Code1((yyvsp[(1) - (2)].el_ival).el); }
    break;

  case 282:
#line 1343 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].el_ival).ival; Code1((yyvsp[(1) - (3)].el_ival).el);
	  if(((yyvsp[(1) - (3)].el_ival).el.El()->GetType() == cssEl::T_ElCFun) ||
	     /*	     ($1.el.El()->GetType() == cssEl::T_MbrCFun) || */
	     ((yyvsp[(1) - (3)].el_ival).el.El()->GetType() == cssEl::T_ScriptFun) ||
	     ((yyvsp[(1) - (3)].el_ival).el.El()->GetType() == cssEl::T_MbrScriptFun)) {
	    cssElFun* fun = (cssElFun*)(yyvsp[(1) - (3)].el_ival).el.El();
	    int max_args = fun->argc;  int min_args = (fun->argc - fun->arg_defs.size);
	    int act_args = (yyvsp[(2) - (3)].ival);
	    if(fun->GetType() == cssEl::T_MbrScriptFun) { max_args--; min_args--; }
	    if((fun->argc >= 0) && (act_args > max_args)) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(), "Too many arguments for function:",fun->name,", should have at most:", String(max_args), "got:",String(act_args)); }
	    if((fun->argc >= 0) && (act_args < min_args)) {
              cssMisc::Warning(cssMisc::cur_top->Prog(), "Too few arguments for function:",fun->name,", should have at least:",String(min_args),"got:",String(act_args)); } } }
    break;

  case 283:
#line 1358 "css_parse.y"
    {
            (yyval.el_ival).ival = Code1(cssMisc::VoidElPtr); /* an arg stop */
	    (yyval.el_ival).el = (yyvsp[(1) - (2)].el);	/* save this pointer for later.. */
            if((yyvsp[(1) - (2)].el).El()->GetType() == cssEl::T_MbrScriptFun)
	      Code1(cssBI::push_cur_this); /* push this as first arg.. */
	    else if((yyvsp[(1) - (2)].el).El()->GetType() == cssEl::T_MbrCFun) {
/* 	      cssMbrCFun* mbrfun = (cssMbrCFun*)$1.El(); */
	      /* always push this for all mbrc funs (change from prev behavior) */
/* 	      if((mbrfun->funp == &cssClassType::InheritsFrom_stub) || */
/* 		 (mbrfun->funp == &cssClassType::Load_stub) || */
/* 		 (mbrfun->funp == &cssClassType::Save_stub)) */
		Code1(cssBI::push_cur_this); /* push this as first arg.. */
	    } }
    break;

  case 284:
#line 1374 "css_parse.y"
    { Code2((yyvsp[(3) - (3)].el), cssBI::points_at);}
    break;

  case 285:
#line 1375 "css_parse.y"
    {
	    int mbno = (yyvsp[(1) - (3)].el).El()->GetMemberNo((const char*)*((yyvsp[(3) - (3)].el).El()));
	    if(mbno < 0) { (yyval.ival) = Code3((yyvsp[(1) - (3)].el), (yyvsp[(3) - (3)].el), cssBI::points_at); }
	    else { cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
		   (yyval.ival) = Code3((yyvsp[(1) - (3)].el), tmpint, cssBI::points_at); } }
    break;

  case 286:
#line 1383 "css_parse.y"
    { (yyval.ival) = Code3(cssBI::push_root, (yyvsp[(2) - (2)].el), cssBI::points_at); }
    break;

  case 287:
#line 1384 "css_parse.y"
    {
	  cssMisc::cur_scope = NULL;
	    cssEl* scp = (yyvsp[(1) - (2)].el).El()->GetScoped((const char*)*((yyvsp[(2) - (2)].el).El()));
	    if(scp != &cssMisc::Void) {  (yyval.ival) = Code1(scp); }
	    else { (yyval.ival) = Code3((yyvsp[(1) - (2)].el), (yyvsp[(2) - (2)].el), cssBI::scoper); } }
    break;

  case 288:
#line 1390 "css_parse.y"
    {
	  /* argstop is put in by member_fun; member_fun skips over end jump, 
	     uses it to find member_call*/
	  (yyval.ival) = (yyvsp[(1) - (3)].el_ival).ival;
	  cssMisc::cur_top->Prog()->insts[(yyvsp[(2) - (3)].ival)-1]->SetJump(Code1(cssBI::member_call)); }
    break;

  case 289:
#line 1395 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[(1) - (4)].el_ival).ival;
	  cssMisc::cur_top->Prog()->insts[(yyvsp[(2) - (4)].ival)-1]->SetJump(Code1(cssBI::member_call));
	  if(((yyvsp[(1) - (4)].el_ival).el.El()->GetType() == cssEl::T_ElCFun) ||
	     ((yyvsp[(1) - (4)].el_ival).el.El()->GetType() == cssEl::T_MbrCFun) ||
	     ((yyvsp[(1) - (4)].el_ival).el.El()->GetType() == cssEl::T_ScriptFun) ||
	     ((yyvsp[(1) - (4)].el_ival).el.El()->GetType() == cssEl::T_MbrScriptFun)) {
	    cssElFun* fun = (cssElFun*)(yyvsp[(1) - (4)].el_ival).el.El();
	    int max_args = fun->argc;  int min_args = (fun->argc - fun->arg_defs.size);
	    int act_args = (yyvsp[(3) - (4)].ival);
	    if(fun->GetType() == cssEl::T_MbrScriptFun) { max_args--; min_args--; }
	    if((fun->argc >= 0) && (act_args > max_args)) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(), "Too many arguments for function:",fun->name,", should have at most:", String(max_args), "got:",String(act_args)); }
	    if((fun->argc >= 0) && (act_args < min_args)) {
              cssMisc::Warning(cssMisc::cur_top->Prog(), "Too few arguments for function:",fun->name,", should have at least:",String(min_args),"got:",String(act_args)); } } }
    break;

  case 290:
#line 1412 "css_parse.y"
    { Code2((yyvsp[(3) - (4)].el), cssBI::member_fun);
                                                  (yyval.el_ival).ival = (yyvsp[(1) - (4)].ival); (yyval.el_ival).el.Reset(); }
    break;

  case 291:
#line 1414 "css_parse.y"
    { (yyval.el_ival).el.Reset();
	    int mbno = (yyvsp[(1) - (4)].el).El()->GetMethodNo((const char*)*((yyvsp[(3) - (4)].el).El()));
	    if(mbno < 0) { /* don't complain for pointers and references */
	      if(!(yyvsp[(1) - (4)].el).El()->MembersDynamic())
		cssMisc::Warning(NULL, "Member Function:",(const char*)*((yyvsp[(3) - (4)].el).El()),
				 "not found in parent object, will be resolved dynamically");
	      (yyval.el_ival).ival = Code3((yyvsp[(1) - (4)].el), (yyvsp[(3) - (4)].el), cssBI::member_fun); }
	    else {
	      cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
	      (yyval.el_ival).ival = Code3((yyvsp[(1) - (4)].el), tmpint, cssBI::member_fun);
	      cssEl* ths = (yyvsp[(1) - (4)].el).El();
	      if(ths->GetType() == cssEl::T_Ptr)
		ths = ((cssPtr*)ths)->el_type;
	      if((ths != NULL) && ((ths->GetType() == cssEl::T_Class) || (ths->GetType() == cssEl::T_ClassType))) {
		cssEl* fun = ths->GetMethodFmName((const char*)*((yyvsp[(3) - (4)].el).El()));
		if(fun->GetType() != cssEl::T_MbrCFun)
		  (yyval.el_ival).el.SetDirect(fun);
	      } } }
    break;

  case 292:
#line 1432 "css_parse.y"
    {
	    cssMisc::cur_scope = NULL; (yyval.el_ival).el.Reset();
	    int mbno = (yyvsp[(1) - (3)].el).El()->GetMethodNo((const char*)*((yyvsp[(2) - (3)].el).El()));
	    if(mbno < 0) { /* don't complain for pointers and references */
	      if(!(yyvsp[(1) - (3)].el).El()->MembersDynamic())
		cssMisc::Warning(NULL, "Member Function:",(const char*)*((yyvsp[(2) - (3)].el).El()),
				 "not found in parent object, will be resolved dynamically");
	      (yyval.el_ival).ival = Code3((yyvsp[(1) - (3)].el), (yyvsp[(2) - (3)].el), cssBI::member_fun); }
	    else {
	      cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
	      (yyval.el_ival).ival = Code3((yyvsp[(1) - (3)].el), tmpint, cssBI::member_fun);
	      cssEl* ths = (yyvsp[(1) - (3)].el).El();
	      if(ths->GetType() == cssEl::T_Ptr)
		ths = ((cssPtr*)ths)->el_type;
	      if((ths != NULL) && ((ths->GetType() == cssEl::T_Class) || (ths->GetType() == cssEl::T_ClassType))) {
		cssEl* fun = ths->GetMethodFmName((const char*)*((yyvsp[(2) - (3)].el).El()));
		if(fun->GetType() != cssEl::T_MbrCFun)
		  (yyval.el_ival).el.SetDirect(fun);
	      } } }
    break;

  case 293:
#line 1456 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].nm))); }
    break;

  case 294:
#line 1460 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].nm))); }
    break;

  case 295:
#line 1462 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName())); }
    break;

  case 296:
#line 1464 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName())); }
    break;

  case 297:
#line 1466 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 298:
#line 1470 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 299:
#line 1474 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding function:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 300:
#line 1480 "css_parse.y"
    { /* these are treated identically in parsing */
           cssMisc::cur_top->parse_path_expr = true; }
    break;

  case 301:
#line 1482 "css_parse.y"
    {
           cssMisc::cur_top->parse_path_expr = true; }
    break;

  case 302:
#line 1486 "css_parse.y"
    {
   	   String tmpstr = String((yyvsp[(1) - (1)].nm));
           (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr);
           cssMisc::cur_top->parse_path_expr = false; }
    break;

  case 303:
#line 1490 "css_parse.y"
    {
           cssMisc::cur_top->parse_path_expr = false; }
    break;

  case 310:
#line 1502 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 311:
#line 1503 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 313:
#line 1507 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 314:
#line 1510 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 315:
#line 1511 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 317:
#line 1515 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 318:
#line 1516 "css_parse.y"
    { 
	  String tmpstr = String((yyvsp[(1) - (1)].nm));
	  (yyval.ival) = Code1(cssMisc::cur_top->AddLiteral(tmpstr)); }
    break;

  case 319:
#line 1519 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 320:
#line 1522 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); }
    break;

  case 324:
#line 1530 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 325:
#line 1533 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;


/* Line 1267 of yacc.c.  */
#line 4353 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
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
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1537 "css_parse.y"


	/* end of grammar */


void yyerror(const char* s) { 	/* called for yacc syntax error */
  cssMisc::SyntaxError(s);
}

