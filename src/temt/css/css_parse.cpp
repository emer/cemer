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
#define YYLAST   2157

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  97
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  309
/* YYNRULES -- Number of states.  */
#define YYNSTATES  515

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
     114,   116,   119,   121,   123,   125,   127,   132,   139,   146,
     153,   155,   158,   162,   169,   172,   175,   177,   179,   182,
     184,   187,   189,   191,   195,   204,   212,   217,   219,   223,
     225,   227,   229,   231,   233,   237,   239,   241,   244,   246,
     249,   251,   253,   255,   257,   260,   262,   264,   267,   270,
     273,   276,   278,   283,   290,   291,   293,   296,   300,   305,
     310,   316,   323,   326,   327,   329,   332,   337,   342,   347,
     353,   358,   361,   365,   368,   372,   375,   379,   383,   388,
     390,   394,   396,   400,   405,   408,   411,   413,   417,   419,
     421,   424,   427,   430,   433,   436,   440,   444,   446,   449,
     452,   456,   458,   460,   463,   466,   469,   471,   473,   475,
     478,   480,   482,   484,   486,   488,   491,   493,   495,   497,
     501,   506,   509,   512,   514,   516,   520,   523,   525,   527,
     529,   531,   538,   540,   547,   555,   557,   559,   562,   564,
     566,   569,   571,   573,   576,   578,   582,   586,   588,   592,
     596,   600,   602,   604,   606,   609,   614,   618,   619,   621,
     623,   627,   629,   631,   633,   635,   637,   639,   641,   643,
     647,   651,   655,   659,   663,   667,   671,   675,   679,   683,
     687,   691,   695,   701,   706,   710,   716,   720,   724,   727,
     730,   734,   738,   742,   746,   750,   754,   760,   764,   768,
     772,   776,   780,   784,   788,   792,   796,   800,   803,   806,
     809,   812,   815,   818,   821,   826,   831,   836,   838,   842,
     844,   846,   848,   850,   852,   854,   857,   861,   864,   868,
     872,   875,   878,   882,   887,   892,   897,   901,   903,   905,
     907,   909,   911,   913,   915,   917,   919,   921,   923,   925,
     927,   929,   931,   933,   935,   937,   941,   943,   945,   947,
     951,   953,   955,   957,   959,   960,   962,   964,   968,   970
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
       9,    -1,    36,   108,    -1,    37,   187,   193,    -1,    37,
     187,   187,    -1,    38,    -1,    38,   201,   187,    -1,    38,
     201,    13,    -1,    38,   201,    37,    -1,    38,   201,    38,
      -1,    38,   201,    19,    -1,    -1,   201,   199,    -1,    87,
     201,   199,    93,    -1,   110,    -1,   111,    99,    -1,   112,
      -1,   115,    -1,   120,    -1,   138,    -1,   147,   145,   204,
      99,    -1,   147,   193,    49,   201,   184,   204,    -1,   192,
      49,   201,   184,   204,    99,    -1,   147,   193,   201,   113,
     204,    99,    -1,   114,    -1,   113,   114,    -1,    88,   184,
      94,    -1,   116,    47,   117,    95,   204,    99,    -1,    21,
     193,    -1,    21,    18,    -1,    21,    -1,   118,    -1,   117,
     118,    -1,   119,    -1,   119,    48,    -1,     1,    -1,   193,
      -1,   193,    49,    10,    -1,    20,   121,    47,   137,   129,
      95,   204,    99,    -1,    20,   121,    47,   137,    95,   204,
      99,    -1,    20,   123,   204,    99,    -1,   122,    -1,   122,
      61,   124,    -1,   193,    -1,    18,    -1,   193,    -1,    18,
      -1,   125,    -1,   124,    48,   125,    -1,   126,    -1,   148,
      -1,   127,   148,    -1,   128,    -1,   127,   128,    -1,    43,
      -1,    45,    -1,    42,    -1,   130,    -1,   129,   130,    -1,
     131,    -1,   135,    -1,   115,   137,    -1,    43,    61,    -1,
      42,    61,    -1,    44,    61,    -1,     1,    -1,   147,   193,
      99,   137,    -1,   147,   193,   132,   133,    99,   137,    -1,
      -1,   134,    -1,   133,   134,    -1,    88,    10,    94,    -1,
     136,   141,    99,   137,    -1,    18,   141,    99,   137,    -1,
      96,    18,   141,    99,   137,    -1,    45,    96,    18,   141,
      99,   137,    -1,   147,   193,    -1,    -1,    17,    -1,   137,
      17,    -1,   139,   141,   204,    99,    -1,   139,   141,   204,
     181,    -1,   147,    13,   141,    99,    -1,   147,    13,   141,
     204,   181,    -1,   140,   141,   204,   182,    -1,   147,   193,
      -1,   147,   150,   193,    -1,   150,    18,    -1,   150,    96,
      18,    -1,    87,    93,    -1,    87,   142,    93,    -1,    87,
      93,    41,    -1,    87,   142,    93,    41,    -1,   143,    -1,
     142,    48,   143,    -1,   144,    -1,   144,    49,   186,    -1,
     144,    49,   150,    16,    -1,   147,    16,    -1,   147,   196,
      -1,   146,    -1,   145,    48,   146,    -1,   193,    -1,   148,
      -1,    39,   148,    -1,    40,   148,    -1,    41,   148,    -1,
      45,   148,    -1,    46,   148,    -1,    46,    45,   148,    -1,
      45,    46,   148,    -1,   149,    -1,   149,   152,    -1,   149,
      66,    -1,   149,   152,    66,    -1,    18,    -1,    19,    -1,
     150,    22,    -1,    18,    89,    -1,    19,    89,    -1,    18,
      -1,    19,    -1,    77,    -1,   152,    77,    -1,   171,    -1,
     174,    -1,   154,    -1,   155,    -1,   181,    -1,   184,    99,
      -1,   160,    -1,   169,    -1,   167,    -1,    30,   201,    99,
      -1,    30,   201,   184,    99,    -1,    31,    99,    -1,    32,
      99,    -1,   158,    -1,   156,    -1,    28,   157,    61,    -1,
      29,    61,    -1,    10,    -1,    11,    -1,    12,    -1,    14,
      -1,   159,    87,   201,   184,    93,    47,    -1,    27,    -1,
     161,    87,   162,   164,   166,   153,    -1,   161,    87,   202,
     162,   164,   166,   153,    -1,    33,    -1,   163,    -1,   163,
     184,    -1,    92,    -1,   165,    -1,   165,   202,    -1,    92,
      -1,    93,    -1,   168,   153,    -1,    24,    -1,   170,   178,
      99,    -1,   170,   178,   153,    -1,    23,    -1,   176,   178,
     154,    -1,   176,   178,   155,    -1,   176,   178,   181,    -1,
     171,    -1,   154,    -1,   183,    -1,   177,   154,    -1,   177,
     172,   175,   173,    -1,   177,   172,   174,    -1,    -1,    25,
      -1,    26,    -1,   179,   184,   180,    -1,    87,    -1,    93,
      -1,    47,    -1,    47,    -1,    95,    -1,   186,    -1,   185,
      -1,   190,    -1,   184,    49,   184,    -1,   184,    49,    47,
      -1,   184,    59,   184,    -1,   184,    58,   184,    -1,   184,
      57,   184,    -1,   184,    56,   184,    -1,   184,    55,   184,
      -1,   184,    54,   184,    -1,   184,    53,   184,    -1,   184,
      52,   184,    -1,   184,    51,   184,    -1,   184,    50,   184,
      -1,    34,   201,   151,    -1,    34,   201,   151,    87,    93,
      -1,    34,   201,   151,   114,    -1,    35,   201,   184,    -1,
      35,    88,    94,   201,   184,    -1,   184,    62,   184,    -1,
     184,    63,   184,    -1,    66,   184,    -1,    77,   184,    -1,
     184,    72,   184,    -1,   184,    71,   184,    -1,   184,    70,
     184,    -1,   184,    69,   184,    -1,   184,    68,   184,    -1,
     184,    67,   184,    -1,   184,    60,   184,    61,   184,    -1,
     184,    75,   184,    -1,   184,    76,   184,    -1,   184,    77,
     184,    -1,   184,    78,   184,    -1,   184,    79,   184,    -1,
     184,    74,   184,    -1,   184,    73,   184,    -1,   184,    66,
     184,    -1,   184,    65,   184,    -1,   184,    64,   184,    -1,
     184,    81,    -1,    81,   184,    -1,   184,    80,    -1,    80,
     184,    -1,    76,   184,    -1,    84,   184,    -1,    96,   184,
      -1,   184,    88,   184,    94,    -1,    87,   147,    93,   184,
      -1,   147,    87,   184,    93,    -1,   188,    -1,    87,   184,
      93,    -1,    10,    -1,    11,    -1,    12,    -1,    14,    -1,
      36,    -1,    13,    -1,   189,    93,    -1,   189,   197,    93,
      -1,    13,    87,    -1,   185,   194,   195,    -1,   186,   194,
     195,    -1,   194,   195,    -1,   150,   195,    -1,   191,   204,
      93,    -1,   191,   204,   197,    93,    -1,   185,   194,   195,
      87,    -1,   186,   194,   195,    87,    -1,   150,   195,    87,
      -1,    16,    -1,    16,    -1,    10,    -1,    11,    -1,    12,
      -1,    14,    -1,    13,    -1,    86,    -1,    90,    -1,    16,
      -1,    11,    -1,    12,    -1,    14,    -1,    13,    -1,    19,
      -1,    10,    -1,   187,    -1,   198,    -1,   197,    48,   198,
      -1,   184,    -1,    19,    -1,   200,    -1,   199,    48,   200,
      -1,   184,    -1,    19,    -1,    16,    -1,    18,    -1,    -1,
     203,    -1,   111,    -1,   202,    48,   202,    -1,   184,    -1,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   154,   154,   155,   156,   157,   158,   161,   164,   165,
     168,   171,   172,   175,   176,   177,   178,   179,   184,   190,
     192,   193,   196,   199,   200,   203,   206,   210,   221,   223,
     227,   229,   231,   235,   239,   243,   247,   253,   254,   255,
     259,   260,   261,   262,   263,   264,   267,   277,   295,   315,
     331,   332,   336,   339,   347,   355,   363,   373,   374,   377,
     378,   379,   382,   400,   421,   425,   429,   435,   436,   439,
     446,   468,   474,   482,   483,   486,   496,   497,   500,   501,
     504,   505,   506,   509,   510,   513,   514,   515,   516,   517,
     518,   519,   522,   534,   550,   558,   559,   563,   567,   573,
     589,   606,   626,   647,   648,   649,   652,   658,   670,   678,
     696,   709,   722,   748,   769,   791,   792,   793,   794,   797,
     798,   801,   802,   805,   812,   820,   830,   831,   838,   847,
     848,   849,   850,   852,   853,   854,   855,   858,   859,   861,
     863,   868,   869,   870,   881,   886,   890,   891,   894,   895,
     898,   899,   900,   903,   904,   907,   908,   909,   910,   911,
     912,   913,   914,   915,   916,   919,   928,   940,   941,   942,
     943,   947,   964,   972,   987,  1010,  1017,  1018,  1021,  1028,
    1029,  1032,  1042,  1052,  1055,  1064,  1087,  1101,  1104,  1109,
    1113,  1119,  1120,  1121,  1124,  1129,  1131,  1135,  1140,  1144,
    1162,  1169,  1176,  1180,  1186,  1189,  1212,  1213,  1217,  1218,
    1219,  1229,  1230,  1231,  1232,  1233,  1234,  1235,  1236,  1237,
    1238,  1239,  1240,  1241,  1243,  1244,  1245,  1246,  1247,  1248,
    1249,  1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,  1258,
    1259,  1260,  1261,  1262,  1263,  1264,  1265,  1266,  1267,  1268,
    1269,  1270,  1271,  1272,  1273,  1274,  1280,  1286,  1287,  1291,
    1292,  1293,  1294,  1297,  1301,  1302,  1303,  1318,  1334,  1335,
    1343,  1344,  1350,  1355,  1372,  1374,  1392,  1416,  1420,  1422,
    1424,  1426,  1430,  1434,  1440,  1442,  1446,  1450,  1454,  1455,
    1456,  1457,  1458,  1459,  1462,  1463,  1466,  1467,  1470,  1471,
    1474,  1475,  1476,  1479,  1482,  1485,  1486,  1487,  1490,  1493
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
  "comb_expr", "primitive", "anycmd", "normfuncall", "normfun",
  "memb_expr", "membfun", "undefname", "name", "getmemb", "membname",
  "membnms", "exprlist", "exprlsel", "cmd_exprlist", "cmd_exprlsel",
  "argstop", "stmtlist", "stmtel", "end", 0
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
     185,   185,   185,   185,   185,   185,   185,   185,   185,   186,
     186,   186,   186,   187,   188,   188,   188,   189,   190,   190,
     190,   190,   190,   190,   191,   191,   191,   192,   193,   193,
     193,   193,   193,   193,   194,   194,   195,   195,   196,   196,
     196,   196,   196,   196,   197,   197,   198,   198,   199,   199,
     200,   200,   200,   200,   201,   202,   202,   202,   203,   204
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
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
static const yytype_uint16 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,    26,
      27,   259,   260,   261,   264,   262,   277,   141,   142,     0,
      56,   187,   184,   198,   199,   172,     0,     0,   304,     0,
       0,   175,   304,   304,   304,     0,   304,     0,     0,     0,
       0,     0,   203,     0,     0,     0,     0,     0,     0,   284,
       0,   285,     3,   205,     0,     0,    13,    14,    15,    16,
      17,     0,     7,    40,     0,    42,    43,     0,    44,    45,
       0,     0,     0,   129,   137,     0,     8,   152,   153,   164,
     163,     0,   156,     0,   158,     0,   157,     0,   150,   151,
       0,     0,   154,     6,     0,   207,   206,   257,     0,   208,
     309,     0,     0,    12,    11,     9,    18,    20,    19,    21,
      23,    22,    24,    25,   267,   144,   145,   279,   280,   281,
     283,   282,   278,    70,     0,    67,   309,    69,    55,    54,
     167,   168,   169,   170,     0,   166,     0,    10,   161,   162,
       0,     0,     0,   304,    28,     0,   263,     0,     0,   130,
       0,   131,   132,     0,   133,     0,   134,     0,     0,   228,
     251,   229,   250,   248,   252,     0,     0,   253,     4,     5,
      41,     0,     0,   309,   309,   283,     0,     0,     0,   309,
     126,     0,   128,   139,   148,   138,   287,   286,   113,   143,
       0,   271,   304,     0,   183,   201,     0,     0,     0,   194,
     197,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   249,   247,     0,   155,     0,     0,   142,   265,
     296,     0,   294,     0,   304,   270,   103,     0,     0,   165,
     159,     0,   146,   147,   221,   304,   224,     0,   302,   303,
     301,   300,    38,   298,    30,    29,    33,    36,    34,    35,
      32,   136,   135,     0,   258,    61,     0,    57,    59,    62,
     115,     0,   119,   121,     0,     0,     0,   309,     0,     0,
       0,   112,   304,     0,   140,   149,   114,   276,     0,   178,
     306,     0,     0,   176,   308,     0,   305,   185,   186,     0,
     188,   196,     0,     0,   210,   209,   220,   219,   218,   217,
     216,   215,   214,   213,   212,   211,     0,   226,   227,   246,
     245,   244,   235,   234,   233,   232,   231,   230,   243,   242,
     237,   238,   239,   240,   241,     0,   268,   269,     0,   266,
     272,     0,     0,   104,     0,    82,    80,    81,    68,    73,
      75,     0,    78,    76,    66,   160,     0,     0,   223,     0,
       0,     0,   255,   309,    58,    60,     0,   117,     0,   116,
       0,   292,   288,   290,   289,   124,   291,   293,   125,   106,
     107,   204,   110,   108,     0,   256,   127,   128,    46,     0,
     309,    50,     0,     0,   181,     0,   179,   177,     0,     0,
     202,   200,   192,   191,   195,   193,   189,   190,     0,   254,
     274,   275,   295,   273,   309,    91,   105,   141,     0,     0,
       0,     0,   309,     0,   103,     0,    83,    85,    86,     0,
       0,     0,    79,    77,   222,     0,   225,    39,   299,     0,
      63,   120,   118,     0,   122,   109,   309,    51,     0,     0,
     182,     0,   180,   307,     0,   236,     0,     0,    89,    88,
      90,     0,     0,     0,    87,   309,    84,     0,    94,    74,
      52,    53,   123,    47,    49,   171,   173,     0,    48,   103,
       0,    65,     0,     0,   103,   103,     0,   174,    99,     0,
     103,    64,    98,    92,     0,     0,    95,   103,   100,     0,
     103,    96,   101,    97,    93
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,   138,   105,    55,    56,    57,    58,    59,    60,
      61,   144,    62,    63,   300,    65,   400,   368,   434,    67,
     276,   277,   278,    68,   124,   125,   126,   358,   359,   360,
     361,   362,   435,   436,   437,   496,   505,   506,   438,   439,
     354,    69,    70,    71,   173,   281,   282,   283,   179,   180,
     157,    73,    74,   158,   254,   185,    76,    77,    78,    79,
     134,    80,    81,    82,    83,   302,   303,   405,   406,   461,
      84,    85,    86,    87,    88,   200,   414,    89,   312,    90,
      91,   196,   197,   411,    92,   392,    93,    94,    95,    96,
     147,    97,    98,    99,   100,   101,   279,   102,   191,   388,
     241,   242,   262,   263,   136,   305,   306,   243
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -309
static const yytype_int16 yypact[] =
{
    -309,   714,  -309,    30,    22,   168,    42,   221,   253,  -309,
    -309,  -309,  -309,  -309,   -22,  -309,  -309,    20,    35,   404,
     467,  -309,  -309,  -309,  -309,  -309,   313,    46,  -309,    39,
      39,  -309,  -309,    59,    87,   103,   199,   297,   297,   297,
      99,    95,  -309,  1605,  1605,  1605,  1605,  1605,  1605,  -309,
    1605,  -309,  -309,  -309,  1605,    30,  -309,  -309,  -309,  -309,
    -309,    30,  -309,  -309,    39,  -309,  -309,   118,  -309,  -309,
      63,    63,   179,  -309,   -23,    77,  -309,  -309,  -309,  -309,
    -309,   100,  -309,   107,  -309,   801,  -309,   117,  -309,  -309,
     117,   969,  -309,  -309,  1985,    90,    90,  -309,  1191,  -309,
    -309,   165,   127,  -309,  -309,  -309,  -309,  -309,  -309,  -309,
    -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,
    -309,  -309,  -309,   124,   135,   163,  -309,   141,  -309,  -309,
    -309,  -309,  -309,  -309,   166,  -309,  1251,  -309,  -309,  -309,
     317,   150,  1605,  -309,  -309,  1338,  -309,   189,   133,  -309,
     224,  -309,  -309,   297,  -309,   297,  -309,   162,   104,    68,
     182,    68,    68,    68,   182,    67,  1805,   182,  -309,  -309,
    -309,   318,    51,  -309,  -309,    63,    20,    35,  1605,   203,
    -309,   476,   -13,  -309,  -309,    12,  -309,  -309,  -309,  -309,
     238,   187,  -309,  1131,  -309,  -309,   533,  1605,  1050,  -309,
     257,   117,  1398,  1605,  1605,  1605,  1605,  1605,  1605,  1605,
    1605,  1605,  1605,  1605,  1605,  1605,  1605,  1605,  1605,  1605,
    1605,  1605,  1605,  1605,  1605,  1605,  1605,  1605,  1605,  1605,
    1605,  1605,  -309,  -309,  1605,  -309,   127,   127,    15,  -309,
    2069,     1,  -309,  1458,  -309,  -309,   259,   116,    39,  -309,
    -309,  1985,  -309,  -309,   250,  -309,    68,  1338,  -309,   184,
     198,  2069,   240,  -309,  -309,  -309,  -309,  -309,  -309,  -309,
    -309,  -309,  -309,  1605,  -309,  -309,    34,  -309,   269,   237,
     251,     8,  -309,   271,   229,   -16,   275,    39,  1850,   476,
      39,  -309,  -309,   261,  -309,  -309,  -309,  -309,  1605,  -309,
    -309,    16,   255,  1605,  2069,    13,  -309,  -309,  -309,  1895,
    -309,  -309,   882,  1050,  -309,  2069,  2069,  2069,  2069,  2069,
    2069,  2069,  2069,  2069,  2069,  2069,  2029,   607,   698,   784,
     564,   514,   312,   312,   312,   312,   312,   312,   376,   376,
     201,   201,    68,    68,    68,  1713,   264,   266,  1665,  -309,
    -309,    18,  1605,  -309,    41,  -309,  -309,  -309,   307,  -309,
    -309,   116,  -309,  -309,  -309,  -309,   263,  1605,  -309,  1605,
      23,  1338,    68,  -309,  -309,  -309,   348,  -309,   213,   319,
     353,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,
    -309,  -309,  -309,  -309,   321,  -309,  -309,  -309,  -309,  1605,
     261,  -309,  1940,   310,  -309,   273,  1545,  2069,  1545,   255,
    -309,  -309,  -309,  -309,  -309,  -309,  -309,  -309,  1605,  -309,
    -309,  -309,  -309,  -309,  2069,  -309,  -309,   -10,   309,   314,
     315,     6,  -309,   355,   259,   167,  -309,  -309,  -309,    63,
     476,   116,  -309,  -309,  -309,  1759,    68,  -309,  -309,    39,
    -309,  -309,  -309,   361,  -309,  -309,  2069,  -309,    39,   332,
    -309,   801,   334,  -309,   273,   233,    39,    39,  -309,  -309,
    -309,   365,    39,    63,   377,  -309,  -309,    39,   128,  -309,
    -309,  -309,  -309,  -309,  -309,  -309,  -309,   801,  -309,   259,
      63,  -309,    39,    39,   259,   259,   311,  -309,   377,    39,
     259,  -309,   377,   377,   385,   142,  -309,   259,   377,   303,
     259,  -309,   377,  -309,   377
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -309,  -309,   -30,   102,  -309,  -309,  -309,  -309,  -309,  -309,
    -309,  -309,  -309,  -309,   397,  -309,  -309,  -277,   400,  -309,
    -309,   126,  -309,  -309,  -309,  -309,  -309,  -309,   -37,  -309,
    -309,    44,  -309,   -29,  -309,  -309,  -309,   -98,  -309,  -309,
    -272,  -309,  -309,  -309,   -65,  -309,    43,  -309,  -309,   123,
       3,   -28,  -309,     0,  -309,  -309,   -77,   -86,   111,  -309,
    -309,  -309,  -309,  -309,  -309,   121,  -309,    21,  -309,   -35,
    -309,  -309,  -309,  -309,   115,  -309,  -309,   232,  -309,   342,
    -309,   -68,  -309,  -309,  -262,  -309,   132,   298,  -309,    65,
    -133,  -309,  -309,  -309,  -309,  -309,   -17,   244,   -95,  -309,
     196,    98,   192,    79,   -15,  -308,  -309,  -106
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -305
static const yytype_int16 yytable[] =
{
     139,    75,   127,   129,    72,   199,   174,   245,   194,   149,
     151,   152,   154,   156,   264,   270,   401,   140,   142,   145,
     248,   148,   198,   390,    17,    18,   117,   118,   119,   120,
     121,    42,   122,   106,   170,   275,   292,   150,   150,   150,
     150,   150,   425,   183,   117,   118,   119,   120,   121,   348,
     122,   417,   153,   165,   184,   182,   378,   109,   426,   427,
      18,   408,    20,  -297,   235,   114,   348,   285,   286,    17,
      18,   371,   181,   290,  -111,  -304,   137,   172,   294,   115,
      37,    38,    39,   428,   429,   430,   431,    41,   186,   295,
      37,    38,    39,   187,   349,   188,    40,    41,   462,   189,
     463,   379,   471,   178,   116,   299,   250,   135,  -297,   115,
     287,   423,   310,    17,    18,   186,   447,    17,    18,   308,
     187,   103,   104,   457,   116,   271,   189,   272,   257,   373,
     265,   137,   455,   313,    17,    18,   432,   433,   186,   146,
     155,   346,   347,   187,   280,   153,   266,   141,   232,   233,
     172,   387,   267,   150,   178,   150,   234,   168,   355,   356,
     273,   357,   474,   169,   291,   171,   307,   293,   425,   146,
     268,   269,   150,   190,   143,   284,    49,   298,   -37,   -37,
      51,   394,   246,   107,   108,   427,    18,   192,    20,   117,
     118,   119,   175,   121,   193,   122,   301,   176,   177,   117,
     118,   119,   120,   121,   195,   122,    37,    38,    39,   428,
     429,   430,   431,    41,   244,  -102,   -72,   498,   364,   363,
     137,   365,   502,   503,   247,   146,   412,   249,   508,   352,
     504,    17,    18,   -71,   137,   512,   110,   111,   514,   381,
     369,   382,   383,   384,   255,   385,   189,   150,   386,   178,
    -141,   289,    37,    38,    39,   389,   296,   393,    40,    41,
     398,  -141,   475,   433,  -142,   146,   178,   449,   112,   113,
     234,  -141,   397,   115,   297,  -142,   353,   399,   229,   230,
     231,   232,   233,    24,   403,  -142,   376,   116,   371,   234,
     -31,   -31,   377,   213,   458,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,    17,    18,   375,   466,   275,
     380,   234,   391,   130,   131,   132,   472,   133,   117,   118,
     119,   120,   121,   443,   122,   252,   253,   366,   367,   236,
     237,   159,   160,   161,   162,   163,   164,   404,   166,   367,
     483,   420,   167,   421,   150,   441,   444,   440,   450,   292,
     452,   150,   467,    11,    12,    13,   460,    15,    42,   493,
     468,   176,   177,   473,   477,   469,   470,   482,   150,   485,
     453,   284,   408,   490,   486,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   426,   509,   240,   513,    64,   504,
     234,    66,   374,   154,   479,   442,   476,   511,   492,   301,
     497,   301,   396,   363,   117,   118,   119,   120,   121,   481,
     122,   451,   123,   478,   416,   499,   409,   413,   484,   487,
     464,   150,   311,   201,   251,   150,   488,   489,   440,   351,
     256,   150,   491,   261,   415,   454,   422,   494,   495,   370,
     448,   227,   228,   229,   230,   231,   232,   233,     0,     0,
       0,     0,   500,   501,   234,     0,     0,     0,     0,   507,
       0,     0,     0,     0,     0,   510,   288,   117,   118,   119,
     120,   121,     0,   122,     0,   128,   117,   118,   119,   120,
     121,   304,   122,     0,     0,   309,     0,     0,     0,     0,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
       0,     0,   345,     0,     0,     0,     0,     0,     0,     0,
       0,   240,     0,    11,    12,    13,    14,    15,     0,     0,
       0,    17,    18,     0,     0,   261,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,     0,
       0,   372,    37,    38,    39,     0,     0,     0,    40,    41,
      42,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   402,     0,     0,    43,
       0,   407,   234,     0,     0,     0,     0,     0,     0,    44,
      45,     0,     0,    46,    47,     0,     0,    48,     0,    49,
      50,     0,     0,    51,     0,   137,     0,     0,     0,    54,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   240,     0,     0,     0,
     424,     0,   234,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   445,     0,   446,     0,   261,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,     0,
       0,     0,     0,     0,     0,   234,     0,   456,     0,     0,
       0,     0,     0,     0,   304,     0,   304,     0,     0,     0,
       0,     0,     0,     0,     2,     3,   465,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,     0,
      16,     0,    17,    18,    19,    20,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,     0,     0,     0,    40,
      41,    42,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
      43,     0,     0,     0,     0,     0,   234,     0,     0,     0,
      44,    45,     0,     0,    46,    47,     0,     0,    48,     0,
      49,    50,     0,     0,    51,    52,     0,     0,     0,    53,
      54,    11,    12,    13,    14,    15,     0,     0,     0,    17,
      18,     0,     0,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,     0,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,    42,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,     0,    43,     0,     0,
       0,     0,   234,     0,     0,     0,     0,    44,    45,     0,
       0,    46,    47,     0,     0,    48,     0,    49,    50,     0,
       0,    51,    11,    12,    13,    14,    15,    54,     0,     0,
      17,    18,     0,     0,     0,    21,    22,    23,     0,    25,
      26,    27,    28,    29,    30,    31,    32,    33,     0,     0,
       0,    37,    38,    39,     0,     0,     0,    40,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    43,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    44,    45,
       0,     0,    46,    47,     0,     0,    48,     0,    49,    50,
       0,     0,    51,     0,     0,     0,     0,    53,    54,    11,
      12,    13,    14,    15,     0,     0,     0,    17,    18,     0,
       0,     0,    21,    22,    23,     0,    25,    26,    27,    28,
      29,    30,    31,    32,    33,     0,     0,     0,    37,    38,
      39,     0,     0,     0,    40,    41,    42,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    43,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    44,    45,     0,     0,    46,
      47,     0,     0,    48,     0,    49,    50,     0,     0,    51,
      11,    12,    13,    14,    15,    54,     0,     0,    17,    18,
       0,     0,     0,    21,    22,     0,     0,    25,    26,    27,
      28,    29,    30,    31,    32,    33,     0,     0,     0,    37,
      38,    39,     0,     0,     0,    40,    41,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    43,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    44,    45,     0,     0,
      46,    47,     0,     0,    48,     0,    49,    50,     0,     0,
      51,    11,    12,    13,    14,    15,    54,    16,     0,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    32,    33,     0,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,     0,     0,
       0,    11,    12,    13,    14,    15,     0,    44,    45,    17,
     238,    46,    47,     0,     0,    48,     0,    49,    50,     0,
       0,    51,     0,   299,     0,    32,    33,    54,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,     0,     0,
       0,    11,    12,    13,    14,    15,     0,    44,    45,    17,
      18,    46,    47,     0,     0,    48,     0,    49,    50,     0,
       0,    51,     0,     0,   239,    32,    33,    54,     0,     0,
      37,    38,    39,     0,     0,     0,    40,    41,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    44,    45,     0,
       0,    46,    47,     0,     0,    48,     0,    49,    50,     0,
       0,    51,     0,   137,     0,     0,     0,    54,    11,    12,
      13,    14,    15,     0,   258,     0,   259,   260,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,    33,     0,     0,     0,    37,    38,    39,
       0,     0,     0,    40,    41,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,     0,     0,     0,    11,    12,
      13,    14,    15,     0,    44,    45,    17,    18,    46,    47,
       0,     0,    48,     0,    49,    50,     0,     0,    51,     0,
       0,     0,    32,    33,    54,     0,     0,    37,    38,    39,
       0,     0,     0,    40,    41,   314,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,     0,     0,     0,    11,    12,
      13,    14,    15,     0,    44,    45,    17,   238,    46,    47,
       0,     0,    48,     0,    49,    50,     0,     0,    51,     0,
       0,     0,    32,    33,    54,     0,     0,    37,    38,    39,
       0,     0,     0,    40,    41,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    44,    45,     0,     0,    46,    47,
       0,     0,    48,     0,    49,    50,     0,     0,    51,     0,
       0,   350,     0,     0,    54,    11,    12,    13,    14,    15,
       0,    16,     0,    17,    18,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      33,     0,     0,     0,    37,    38,    39,     0,     0,     0,
      40,    41,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,     0,     0,     0,    11,    12,    13,    14,    15,
       0,    44,    45,    17,    18,    46,    47,     0,     0,    48,
       0,    49,    50,     0,     0,    51,     0,     0,     0,    32,
      33,    54,     0,     0,    37,    38,    39,     0,     0,     0,
      40,    41,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,     0,     0,     0,    11,    12,    13,    14,    15,
       0,    44,    45,    17,   238,    46,    47,     0,     0,    48,
       0,    49,    50,     0,     0,    51,     0,     0,     0,    32,
      33,    54,     0,     0,    37,    38,    39,     0,     0,     0,
      40,    41,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    44,    45,     0,     0,    46,    47,     0,     0,    48,
       0,    49,    50,     0,     0,    51,     0,     0,     0,     0,
       0,    54,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,     0,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,     0,     0,     0,     0,     0,
       0,   234,     0,     0,     0,     0,     0,   419,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
       0,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,     0,     0,     0,     0,     0,     0,   234,     0,     0,
       0,     0,     0,   480,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,     0,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,     0,     0,     0,
       0,     0,     0,   234,     0,     0,     0,     0,   274,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,     0,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,     0,     0,     0,     0,     0,     0,   234,     0,
       0,     0,     0,   395,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,     0,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,     0,     0,     0,
       0,     0,     0,   234,     0,     0,     0,     0,   410,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,     0,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,     0,     0,     0,     0,     0,     0,   234,     0,
       0,     0,     0,   459,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,     0,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,     0,     0,     0,
       0,     0,     0,   234,     0,     0,     0,   137,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     418,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,     0,     0,     0,     0,     0,     0,   234,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
       0,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,     0,     0,     0,     0,     0,     0,   234
};

static const yytype_int16 yycheck[] =
{
      30,     1,    19,    20,     1,    91,    71,   102,    85,    37,
      38,    39,    40,    41,   147,   148,   293,    32,    33,    34,
     126,    36,    90,   285,    18,    19,    10,    11,    12,    13,
      14,    47,    16,    11,    64,     1,    49,    37,    38,    39,
      40,    41,     1,    66,    10,    11,    12,    13,    14,    48,
      16,   313,    46,    50,    77,    72,    48,    15,    17,    18,
      19,    48,    21,    48,    94,    87,    48,   173,   174,    18,
      19,    48,    72,   179,    87,    88,    92,    87,    66,    89,
      39,    40,    41,    42,    43,    44,    45,    46,    11,    77,
      39,    40,    41,    16,    93,    18,    45,    46,   406,    22,
     408,    93,    96,    87,    89,    92,   136,    61,    93,    89,
     175,    93,   198,    18,    19,    11,    93,    18,    19,   196,
      16,    91,    92,   400,    89,   153,    22,   155,   143,    95,
     147,    92,   394,   201,    18,    19,    95,    96,    11,    36,
      45,   236,   237,    16,    93,    46,    13,    88,    80,    81,
      87,   284,    19,   153,    87,   155,    88,    55,    42,    43,
      93,    45,   434,    61,   181,    47,   196,   182,     1,    36,
      37,    38,   172,    96,    87,   172,    86,   192,    91,    92,
      90,   287,    47,    15,    16,    18,    19,    87,    21,    10,
      11,    12,    13,    14,    87,    16,   193,    18,    19,    10,
      11,    12,    13,    14,    87,    16,    39,    40,    41,    42,
      43,    44,    45,    46,    49,    87,    92,   489,   248,   247,
      92,   251,   494,   495,    61,    36,   312,    61,   500,   244,
      88,    18,    19,    92,    92,   507,    15,    16,   510,    10,
     255,    12,    13,    14,    94,    16,    22,   247,    19,    87,
      66,    48,    39,    40,    41,   285,    18,   287,    45,    46,
     290,    77,    95,    96,    66,    36,    87,   373,    15,    16,
      88,    87,   289,    89,    87,    77,    17,   292,    77,    78,
      79,    80,    81,    26,   301,    87,    49,    89,    48,    88,
      91,    92,    41,    60,   400,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    18,    19,    48,   424,     1,
      49,    88,    47,    10,    11,    12,   432,    14,    10,    11,
      12,    13,    14,   361,    16,    18,    19,    87,    88,    95,
      96,    43,    44,    45,    46,    47,    48,    92,    50,    88,
     456,    87,    54,    87,   354,    48,    93,   354,    10,    49,
      41,   361,   427,    10,    11,    12,    93,    14,    47,   475,
      61,    18,    19,    18,   439,    61,    61,    16,   378,    47,
     380,   378,    48,    18,   461,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    17,    10,    98,    94,     1,    88,
      88,     1,   276,   431,   441,   361,   435,   505,   473,   406,
     487,   408,   289,   441,    10,    11,    12,    13,    14,   449,
      16,   378,    18,   440,   313,   490,   305,   312,   458,   464,
     409,   431,   200,    91,   136,   435,   466,   467,   435,   243,
     142,   441,   472,   145,   312,   380,   348,   477,   478,   257,
     371,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
      -1,    -1,   492,   493,    88,    -1,    -1,    -1,    -1,   499,
      -1,    -1,    -1,    -1,    -1,   505,   178,    10,    11,    12,
      13,    14,    -1,    16,    -1,    18,    10,    11,    12,    13,
      14,   193,    16,    -1,    -1,   197,    -1,    -1,    -1,    -1,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
      -1,    -1,   234,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   243,    -1,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    -1,    -1,   257,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,   273,    39,    40,    41,    -1,    -1,    -1,    45,    46,
      47,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,   298,    -1,    -1,    66,
      -1,   303,    88,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      77,    -1,    -1,    80,    81,    -1,    -1,    84,    -1,    86,
      87,    -1,    -1,    90,    -1,    92,    -1,    -1,    -1,    96,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,   348,    -1,    -1,    -1,
     352,    -1,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   367,    -1,   369,    -1,   371,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      -1,    -1,    -1,    -1,    -1,    88,    -1,   399,    -1,    -1,
      -1,    -1,    -1,    -1,   406,    -1,   408,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     0,     1,   418,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      16,    -1,    18,    19,    20,    21,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    47,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      66,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,    -1,    -1,    84,    -1,
      86,    87,    -1,    -1,    90,    91,    -1,    -1,    -1,    95,
      96,    10,    11,    12,    13,    14,    -1,    -1,    -1,    18,
      19,    -1,    -1,    -1,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    47,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    -1,    66,    -1,    -1,
      -1,    -1,    88,    -1,    -1,    -1,    -1,    76,    77,    -1,
      -1,    80,    81,    -1,    -1,    84,    -1,    86,    87,    -1,
      -1,    90,    10,    11,    12,    13,    14,    96,    -1,    -1,
      18,    19,    -1,    -1,    -1,    23,    24,    25,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    39,    40,    41,    -1,    -1,    -1,    45,    46,    47,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,
      -1,    -1,    80,    81,    -1,    -1,    84,    -1,    86,    87,
      -1,    -1,    90,    -1,    -1,    -1,    -1,    95,    96,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    18,    19,    -1,
      -1,    -1,    23,    24,    25,    -1,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    39,    40,
      41,    -1,    -1,    -1,    45,    46,    47,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    77,    -1,    -1,    80,
      81,    -1,    -1,    84,    -1,    86,    87,    -1,    -1,    90,
      10,    11,    12,    13,    14,    96,    -1,    -1,    18,    19,
      -1,    -1,    -1,    23,    24,    -1,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    39,
      40,    41,    -1,    -1,    -1,    45,    46,    47,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,    -1,
      80,    81,    -1,    -1,    84,    -1,    86,    87,    -1,    -1,
      90,    10,    11,    12,    13,    14,    96,    16,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    -1,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    10,    11,    12,    13,    14,    -1,    76,    77,    18,
      19,    80,    81,    -1,    -1,    84,    -1,    86,    87,    -1,
      -1,    90,    -1,    92,    -1,    34,    35,    96,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    10,    11,    12,    13,    14,    -1,    76,    77,    18,
      19,    80,    81,    -1,    -1,    84,    -1,    86,    87,    -1,
      -1,    90,    -1,    -1,    93,    34,    35,    96,    -1,    -1,
      39,    40,    41,    -1,    -1,    -1,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,
      -1,    80,    81,    -1,    -1,    84,    -1,    86,    87,    -1,
      -1,    90,    -1,    92,    -1,    -1,    -1,    96,    10,    11,
      12,    13,    14,    -1,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    34,    35,    -1,    -1,    -1,    39,    40,    41,
      -1,    -1,    -1,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    10,    11,
      12,    13,    14,    -1,    76,    77,    18,    19,    80,    81,
      -1,    -1,    84,    -1,    86,    87,    -1,    -1,    90,    -1,
      -1,    -1,    34,    35,    96,    -1,    -1,    39,    40,    41,
      -1,    -1,    -1,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    10,    11,
      12,    13,    14,    -1,    76,    77,    18,    19,    80,    81,
      -1,    -1,    84,    -1,    86,    87,    -1,    -1,    90,    -1,
      -1,    -1,    34,    35,    96,    -1,    -1,    39,    40,    41,
      -1,    -1,    -1,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    76,    77,    -1,    -1,    80,    81,
      -1,    -1,    84,    -1,    86,    87,    -1,    -1,    90,    -1,
      -1,    93,    -1,    -1,    96,    10,    11,    12,    13,    14,
      -1,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      35,    -1,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,
      45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    -1,    -1,    -1,    10,    11,    12,    13,    14,
      -1,    76,    77,    18,    19,    80,    81,    -1,    -1,    84,
      -1,    86,    87,    -1,    -1,    90,    -1,    -1,    -1,    34,
      35,    96,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,
      45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    -1,    -1,    -1,    10,    11,    12,    13,    14,
      -1,    76,    77,    18,    19,    80,    81,    -1,    -1,    84,
      -1,    86,    87,    -1,    -1,    90,    -1,    -1,    -1,    34,
      35,    96,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,
      45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    77,    -1,    -1,    80,    81,    -1,    -1,    84,
      -1,    86,    87,    -1,    -1,    90,    -1,    -1,    -1,    -1,
      -1,    96,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,
      -1,    88,    -1,    -1,    -1,    -1,    -1,    94,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,
      -1,    -1,    -1,    94,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    -1,    -1,    -1,
      -1,    -1,    -1,    88,    -1,    -1,    -1,    -1,    93,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,
      -1,    -1,    -1,    93,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    -1,    -1,    -1,
      -1,    -1,    -1,    88,    -1,    -1,    -1,    -1,    93,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,
      -1,    -1,    -1,    93,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    -1,    -1,    -1,
      -1,    -1,    -1,    88,    -1,    -1,    -1,    92,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    -1,    -1,    -1,    -1,    -1,    -1,    88,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    -1,    -1,    -1,    -1,    -1,    -1,    88
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
      87,    90,    91,    95,    96,   101,   102,   103,   104,   105,
     106,   107,   109,   110,   111,   112,   115,   116,   120,   138,
     139,   140,   147,   148,   149,   150,   153,   154,   155,   156,
     158,   159,   160,   161,   167,   168,   169,   170,   171,   174,
     176,   177,   181,   183,   184,   185,   186,   188,   189,   190,
     191,   192,   194,    91,    92,   100,    11,    15,    16,    15,
      15,    16,    15,    16,    87,    89,    89,    10,    11,    12,
      13,    14,    16,    18,   121,   122,   123,   193,    18,   193,
      10,    11,    12,    14,   157,    61,   201,    92,    99,    99,
     201,    88,   201,    87,   108,   201,    36,   187,   201,   148,
     150,   148,   148,    46,   148,    45,   148,   147,   150,   184,
     184,   184,   184,   184,   184,   147,   184,   184,   100,   100,
      99,    47,    87,   141,   141,    13,    18,    19,    87,   145,
     146,   150,   193,    66,    77,   152,    11,    16,    18,    22,
      96,   195,    87,    87,   153,    87,   178,   179,   178,   154,
     172,   176,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    88,    99,   194,   194,    19,    93,
     184,   197,   198,   204,    49,   195,    47,    61,   204,    61,
      99,   184,    18,    19,   151,    94,   184,   201,    16,    18,
      19,   184,   199,   200,   187,   193,    13,    19,    37,    38,
     187,   148,   148,    93,    93,     1,   117,   118,   119,   193,
      93,   142,   143,   144,   147,   204,   204,   141,   184,    48,
     204,   193,    49,   201,    66,    77,    18,    87,   201,    92,
     111,   147,   162,   163,   184,   202,   203,    99,   153,   184,
     154,   174,   175,   178,    47,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   195,   195,    48,    93,
      93,   197,   201,    17,   137,    42,    43,    45,   124,   125,
     126,   127,   128,   148,    99,    99,    87,    88,   114,   201,
     199,    48,   184,    95,   118,    48,    49,    41,    48,    93,
      49,    10,    12,    13,    14,    16,    19,   187,   196,    99,
     181,    47,   182,    99,   204,    93,   146,   193,    99,   201,
     113,   114,   184,   193,    92,   164,   165,   184,    48,   162,
      93,   180,   154,   171,   173,   183,   155,   181,    61,    94,
      87,    87,   198,    93,   184,     1,    17,    18,    42,    43,
      44,    45,    95,    96,   115,   129,   130,   131,   135,   136,
     147,    48,   128,   148,    93,   184,   184,    93,   200,   204,
      10,   143,    41,   150,   186,   181,   184,   114,   204,    93,
      93,   166,   202,   202,   164,   184,   204,   141,    61,    61,
      61,    96,   204,    18,   137,    95,   130,   141,   193,   125,
      94,    99,    16,   204,    99,    47,   153,   166,    99,    99,
      18,    99,   141,   204,    99,    99,   132,   153,   137,   141,
      99,    99,   137,   137,    88,   133,   134,    99,   137,    10,
      99,   134,   137,    94,   137
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
#line 155 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Blank; }
    break;

  case 4:
#line 156 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 5:
#line 157 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return (yyvsp[(2) - (3)].ival); }
    break;

  case 6:
#line 158 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 7:
#line 161 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 8:
#line 164 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 9:
#line 165 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Err; }
    break;

  case 10:
#line 168 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 11:
#line 171 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 12:
#line 172 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 18:
#line 184 "css_parse.y"
    {
            cssEl* tmp = (yyvsp[(2) - (2)].el).El();
	    cssMisc::cur_top->SetInclude((const char*)*tmp);
	    cssEl::Done((yyvsp[(2) - (2)].el).El()); }
    break;

  case 19:
#line 190 "css_parse.y"
    {
            cssMisc::Defines.Push(new cssDef(0, (yyvsp[(2) - (2)].nm))); }
    break;

  case 21:
#line 193 "css_parse.y"
    { cssMisc::Defines.Remove((yyvsp[(2) - (2)].el).El()); }
    break;

  case 22:
#line 196 "css_parse.y"
    { /* if its a name, its not defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 24:
#line 200 "css_parse.y"
    { /* if its a def, its defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 26:
#line 206 "css_parse.y"
    { /* if you see it, it applies to u*/
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog()); }
    break;

  case 28:
#line 221 "css_parse.y"
    {
            Code1((yyvsp[(1) - (2)].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 29:
#line 223 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssRef((yyvsp[(2) - (3)].el)));
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 30:
#line 227 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc; }
    break;

  case 31:
#line 229 "css_parse.y"
    {
	    Code1((yyvsp[(1) - (1)].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 32:
#line 231 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 33:
#line 235 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 34:
#line 239 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 35:
#line 243 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 36:
#line 247 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 37:
#line 253 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 39:
#line 255 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 46:
#line 267 "css_parse.y"
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
#line 277 "css_parse.y"
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
#line 295 "css_parse.y"
    {	/* no type information, get from last expr */
	    cssMisc::cur_top->Prog()->UnCode();	/* undo the end jump coding */
	    css_progdx actln = (yyvsp[(5) - (6)].ival)-2;
	    if(cssMisc::cur_top->Prog()->insts[actln]->IsJump()) { /* if jmp, get before */
	      actln = (yyvsp[(3) - (6)].ival)+1;	/* go for the first thing if the last is no good */
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
#line 315 "css_parse.y"
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
#line 331 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 51:
#line 332 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 52:
#line 336 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 53:
#line 339 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(5) - (6)].ival)-1]->SetJump((yyvsp[(5) - (6)].ival)); (yyval.ival) = (yyvsp[(5) - (6)].ival)-1;
	    if(cssMisc::cur_enum->name.before(5) == "enum_") /* don't keep anon enums around */
	      delete cssMisc::cur_enum;
	    cssMisc::parsing_args = false; /* needed for - numbers */
	    cssMisc::cur_enum = NULL; }
    break;

  case 54:
#line 347 "css_parse.y"
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
#line 355 "css_parse.y"
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
#line 363 "css_parse.y"
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
#line 379 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 62:
#line 382 "css_parse.y"
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
#line 400 "css_parse.y"
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
#line 421 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(7) - (8)].ival)-1]->SetJump((yyvsp[(7) - (8)].ival)); (yyval.ival) = (yyvsp[(7) - (8)].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[(4) - (8)].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 65:
#line 425 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(6) - (7)].ival)-1]->SetJump((yyvsp[(6) - (7)].ival)); (yyval.ival) = (yyvsp[(6) - (7)].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[(4) - (7)].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 66:
#line 429 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(3) - (4)].ival)); (yyval.ival) = (yyvsp[(3) - (4)].ival)-1;
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 69:
#line 439 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssMisc::cur_class = new cssClassType((const char*)*nm);
	    cssMisc::cur_class->last_top = cssMisc::cur_top;
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
	    cssEl::Done(nm); }
    break;

  case 70:
#line 446 "css_parse.y"
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
#line 468 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_class = new cssClassType((const char*)*nm);
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
            cssEl::Done(nm); }
    break;

  case 72:
#line 474 "css_parse.y"
    {
            if((yyvsp[(1) - (1)].el).El()->GetType() != cssEl::T_ClassType) {
              yyerror("Attempt to redefine non-class type as a class");
              return cssProg::YY_Err;
            }
            cssMisc::cur_class = (cssClassType*)(yyvsp[(1) - (1)].el).El(); }
    break;

  case 75:
#line 486 "css_parse.y"
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
#line 497 "css_parse.y"
    { (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 86:
#line 514 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 87:
#line 515 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 91:
#line 519 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 92:
#line 522 "css_parse.y"
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
#line 534 "css_parse.y"
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
#line 550 "css_parse.y"
    {
            /* swap places with void marker */
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_top->Prog()->Stack()->Push(&cssMisc::Void);
            cssMisc::cur_top->Prog()->Stack()->Push(nm); }
    break;

  case 95:
#line 558 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 96:
#line 559 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 97:
#line 563 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((int)*(yyvsp[(2) - (3)].el).El())); }
    break;

  case 98:
#line 567 "css_parse.y"
    {       /* pre-declare function */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[(1) - (4)].el).El();
            cssMisc::cur_class->GetComments(fun, (yyvsp[(4) - (4)].el));
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr, true); }
    break;

  case 99:
#line 573 "css_parse.y"
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
#line 589 "css_parse.y"
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
#line 606 "css_parse.y"
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
#line 626 "css_parse.y"
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
#line 647 "css_parse.y"
    { (yyval.el).Reset(); }
    break;

  case 104:
#line 648 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (1)].el); }
    break;

  case 105:
#line 649 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (2)].el); }
    break;

  case 106:
#line 652 "css_parse.y"
    {	/* pre-declare function */
            cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(3) - (4)].ival));	(yyval.ival) = (yyvsp[(3) - (4)].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[(1) - (4)].el).El();
	    fun->argc = (yyvsp[(2) - (4)].ival); fun->GetArgDefs(); }
    break;

  case 107:
#line 658 "css_parse.y"
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
#line 670 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 109:
#line 678 "css_parse.y"
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
#line 696 "css_parse.y"
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
#line 709 "css_parse.y"
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
#line 722 "css_parse.y"
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
#line 748 "css_parse.y"
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
#line 769 "css_parse.y"
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
#line 791 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 116:
#line 792 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 117:
#line 793 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 118:
#line 794 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 119:
#line 797 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 120:
#line 798 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 121:
#line 801 "css_parse.y"
    { cssMisc::parsing_args = false; }
    break;

  case 122:
#line 802 "css_parse.y"
    {
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push((yyvsp[(3) - (3)].el).El()); }
    break;

  case 123:
#line 805 "css_parse.y"
    {
            cssEl* itm = (yyvsp[(3) - (4)].el).El()->GetScoped((yyvsp[(4) - (4)].nm));
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push(itm); cssMisc::cur_scope = NULL; }
    break;

  case 124:
#line 812 "css_parse.y"
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
#line 820 "css_parse.y"
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
#line 831 "css_parse.y"
    {
	    if(((yyvsp[(1) - (3)].ival) < 0) && ((yyvsp[(3) - (3)].ival) < 0)) (yyval.ival) = -1;
	    else if(((yyvsp[(1) - (3)].ival) >= 0) && ((yyvsp[(3) - (3)].ival) < 0)) (yyval.ival) = (yyvsp[(1) - (3)].ival);
	    else if(((yyvsp[(1) - (3)].ival) < 0) && ((yyvsp[(3) - (3)].ival) >= 0)) (yyval.ival) = (yyvsp[(3) - (3)].ival);
	    else (yyval.ival) = -1; }
    break;

  case 128:
#line 838 "css_parse.y"
    {
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      (yyval.ival) = Code2(tmp->ptr, cssBI::constr); }
	    else (yyval.ival) = -1;
	    cssEl::Done(tmp); }
    break;

  case 129:
#line 847 "css_parse.y"
    { (yyvsp[(1) - (1)].el).El()->tmp_str = ""; }
    break;

  case 130:
#line 848 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "extern"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 131:
#line 849 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "static"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 132:
#line 850 "css_parse.y"
    { /* use const expr prog to code stuff */
	  (yyvsp[(2) - (2)].el).El()->tmp_str = "const"; (yyval.el) = (yyvsp[(2) - (2)].el); cssMisc::CodeConstExpr(); }
    break;

  case 133:
#line 852 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 134:
#line 853 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = ""; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 135:
#line 854 "css_parse.y"
    { (yyvsp[(3) - (3)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(3) - (3)].el); }
    break;

  case 136:
#line 855 "css_parse.y"
    { (yyvsp[(3) - (3)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(3) - (3)].el); }
    break;

  case 137:
#line 858 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (1)].el); cssMisc::cur_type = (yyval.el); }
    break;

  case 138:
#line 859 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->GetPtrType((yyvsp[(1) - (2)].el).El(), (yyvsp[(2) - (2)].ival)); cssMisc::cur_type = (yyval.el); }
    break;

  case 139:
#line 861 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->GetRefType((yyvsp[(1) - (2)].el).El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 140:
#line 863 "css_parse.y"
    {
	    cssElPtr pt = cssMisc::cur_top->GetPtrType((yyvsp[(1) - (3)].el).El(), (yyvsp[(2) - (3)].ival));
            (yyval.el) = cssMisc::cur_top->GetRefType(pt.El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 143:
#line 870 "css_parse.y"
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
#line 881 "css_parse.y"
    {
          if(((yyvsp[(1) - (2)].el).El()->GetType() != cssEl::T_ClassType) && ((yyvsp[(1) - (2)].el).El()->GetType() != cssEl::T_EnumType)) {
	     yyerror("scoping of non-class or enum type");
	     return cssProg::YY_Err; }
           cssMisc::cur_scope = (yyvsp[(1) - (2)].el).El(); }
    break;

  case 145:
#line 886 "css_parse.y"
    {
           cssMisc::cur_scope = (yyvsp[(1) - (2)].el).El(); }
    break;

  case 148:
#line 894 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 149:
#line 895 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 152:
#line 900 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastif = -1; }
    break;

  case 155:
#line 907 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 159:
#line 911 "css_parse.y"
    { Code1((yyvsp[(1) - (3)].el)); (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 160:
#line 912 "css_parse.y"
    { Code1((yyvsp[(1) - (4)].el)); (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 161:
#line 913 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (2)].el)); }
    break;

  case 162:
#line 914 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (2)].el)); }
    break;

  case 165:
#line 919 "css_parse.y"
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
#line 928 "css_parse.y"
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
#line 947 "css_parse.y"
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
#line 964 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssSwitchBlock_Name, cssMisc::cur_top->Prog());
	    blk->loop_type = cssCodeBlock::SWITCH;
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 173:
#line 972 "css_parse.y"
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
#line 987 "css_parse.y"
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
#line 1010 "css_parse.y"
    { /* for loop contained within own block */
            cssCodeBlock* blk = new cssCodeBlock(cssForLoop_Name, cssMisc::cur_top->Prog());
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 178:
#line 1021 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 181:
#line 1032 "css_parse.y"
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
#line 1042 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of incr */
	      cssCodeBlock* blk = new cssCodeBlock(cssForLoopStmt_Name, cssMisc::cur_top->Prog());
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_type = cssCodeBlock::FOR;
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 184:
#line 1055 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssDoLoop_Name, cssMisc::cur_top->Prog());
	    blk->loop_type = cssCodeBlock::DO;
	    cssMisc::cur_top->AddStatic(blk); /* while is all inside this do! */
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 185:
#line 1064 "css_parse.y"
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
#line 1087 "css_parse.y"
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
#line 1101 "css_parse.y"
    { }
    break;

  case 188:
#line 1104 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 189:
#line 1109 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          cssMisc::cur_top->Prog()->lastelseif = false; /* use this flag! */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 190:
#line 1113 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          cssMisc::cur_top->Prog()->lastelseif = true; /* be on guard for another else! */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); cssMisc::cur_top->DoCompileCtrl(); /* need to do the bra! */ }
    break;

  case 194:
#line 1124 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); /* pop the if_false block */
	    /* now check for other else blocks that need popping! */
	    cssMisc::cur_top->PopElseBlocks(); /* pop residual elses! */
	    (yyval.ival) = (yyvsp[(1) - (2)].ival);  }
    break;

  case 195:
#line 1129 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[(1) - (4)].ival);  }
    break;

  case 196:
#line 1131 "css_parse.y"
    { /* do not pop if going into another else */
	  (yyval.ival) = (yyvsp[(1) - (3)].ival);  }
    break;

  case 197:
#line 1135 "css_parse.y"
    {
	  if(!cssMisc::cur_top->Prog()->lastelseif) {
	    cssMisc::cur_top->PopElseBlocks(); } }
    break;

  case 198:
#line 1140 "css_parse.y"
    {
          cssMisc::cur_top->Prog()->lastif = cssMisc::cur_top->Prog()->size; }
    break;

  case 199:
#line 1144 "css_parse.y"
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
#line 1162 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssIfTrueBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	    cssMisc::cur_top->AddStatic(blk);
	    Code1(blk); cssMisc::cur_top->Push(blk->code); (yyval.ival) = (yyvsp[(1) - (3)].ival); }
    break;

  case 201:
#line 1169 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 202:
#line 1176 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); }
    break;

  case 203:
#line 1180 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssBlock_Name, cssMisc::cur_top->Prog());
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->SetPush(blk->code); }
    break;

  case 204:
#line 1186 "css_parse.y"
    {  }
    break;

  case 205:
#line 1189 "css_parse.y"
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
#line 1212 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 209:
#line 1218 "css_parse.y"
    { Code1(cssBI::asgn); }
    break;

  case 210:
#line 1219 "css_parse.y"
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
#line 1229 "css_parse.y"
    { Code1(cssBI::asgn_add); }
    break;

  case 212:
#line 1230 "css_parse.y"
    { Code1(cssBI::asgn_sub); }
    break;

  case 213:
#line 1231 "css_parse.y"
    { Code1(cssBI::asgn_mult); }
    break;

  case 214:
#line 1232 "css_parse.y"
    { Code1(cssBI::asgn_div); }
    break;

  case 215:
#line 1233 "css_parse.y"
    { Code1(cssBI::asgn_mod); }
    break;

  case 216:
#line 1234 "css_parse.y"
    { Code1(cssBI::asgn_lshift); }
    break;

  case 217:
#line 1235 "css_parse.y"
    { Code1(cssBI::asgn_rshift); }
    break;

  case 218:
#line 1236 "css_parse.y"
    { Code1(cssBI::asgn_and); }
    break;

  case 219:
#line 1237 "css_parse.y"
    { Code1(cssBI::asgn_xor); }
    break;

  case 220:
#line 1238 "css_parse.y"
    { Code1(cssBI::asgn_or); }
    break;

  case 221:
#line 1239 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code2((yyvsp[(3) - (3)].el),(yyvsp[(1) - (3)].el)); }
    break;

  case 222:
#line 1240 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (5)].ival); Code2((yyvsp[(3) - (5)].el),(yyvsp[(1) - (5)].el)); }
    break;

  case 223:
#line 1241 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[(2) - (4)].ival); Code2((yyvsp[(3) - (4)].el),(yyvsp[(1) - (4)].el)); }
    break;

  case 224:
#line 1243 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code1((yyvsp[(1) - (3)].el)); }
    break;

  case 225:
#line 1244 "css_parse.y"
    { (yyval.ival) = (yyvsp[(4) - (5)].ival); Code1((yyvsp[(1) - (5)].el)); }
    break;

  case 226:
#line 1245 "css_parse.y"
    { Code1(cssBI::lor); }
    break;

  case 227:
#line 1246 "css_parse.y"
    { Code1(cssBI::land); }
    break;

  case 228:
#line 1247 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::addr_of); }
    break;

  case 229:
#line 1248 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::de_ptr); }
    break;

  case 230:
#line 1249 "css_parse.y"
    { Code1(cssBI::gt); }
    break;

  case 231:
#line 1250 "css_parse.y"
    { Code1(cssBI::ge); }
    break;

  case 232:
#line 1251 "css_parse.y"
    { Code1(cssBI::lt); }
    break;

  case 233:
#line 1252 "css_parse.y"
    { Code1(cssBI::le); }
    break;

  case 234:
#line 1253 "css_parse.y"
    { Code1(cssBI::eq); }
    break;

  case 235:
#line 1254 "css_parse.y"
    { Code1(cssBI::ne); }
    break;

  case 236:
#line 1255 "css_parse.y"
    { Code1(cssBI::cond); }
    break;

  case 237:
#line 1256 "css_parse.y"
    { Code1(cssBI::add); }
    break;

  case 238:
#line 1257 "css_parse.y"
    { Code1(cssBI::sub); }
    break;

  case 239:
#line 1258 "css_parse.y"
    { Code1(cssBI::mul); }
    break;

  case 240:
#line 1259 "css_parse.y"
    { Code1(cssBI::div); }
    break;

  case 241:
#line 1260 "css_parse.y"
    { Code1(cssBI::modulo); }
    break;

  case 242:
#line 1261 "css_parse.y"
    { Code1(cssBI::lshift); }
    break;

  case 243:
#line 1262 "css_parse.y"
    { Code1(cssBI::rshift); }
    break;

  case 244:
#line 1263 "css_parse.y"
    { Code1(cssBI::bit_and); }
    break;

  case 245:
#line 1264 "css_parse.y"
    { Code1(cssBI::bit_xor); }
    break;

  case 246:
#line 1265 "css_parse.y"
    { Code1(cssBI::bit_or); }
    break;

  case 247:
#line 1266 "css_parse.y"
    { Code1(cssBI::asgn_post_pp); }
    break;

  case 248:
#line 1267 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::asgn_pre_pp); }
    break;

  case 249:
#line 1268 "css_parse.y"
    { Code1(cssBI::asgn_post_mm); }
    break;

  case 250:
#line 1269 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::asgn_pre_mm); }
    break;

  case 251:
#line 1270 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::neg); }
    break;

  case 252:
#line 1271 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::lnot); }
    break;

  case 253:
#line 1272 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::bitneg); }
    break;

  case 254:
#line 1273 "css_parse.y"
    { Code1(cssBI::de_array); }
    break;

  case 255:
#line 1274 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(2) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = (yyvsp[(4) - (4)].ival); Code2((yyvsp[(2) - (4)].el), cssBI::cast); }
    break;

  case 256:
#line 1280 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
  	    (yyval.ival) = (yyvsp[(3) - (4)].ival); Code2((yyvsp[(1) - (4)].el), cssBI::cast); }
    break;

  case 258:
#line 1287 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 264:
#line 1301 "css_parse.y"
    { (yyval.ival) = Code2(cssBI::push_next, (yyvsp[(1) - (1)].el)); }
    break;

  case 265:
#line 1302 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].el_ival).ival; Code1((yyvsp[(1) - (2)].el_ival).el); }
    break;

  case 266:
#line 1303 "css_parse.y"
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

  case 267:
#line 1318 "css_parse.y"
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

  case 268:
#line 1334 "css_parse.y"
    { Code2((yyvsp[(3) - (3)].el), cssBI::points_at);}
    break;

  case 269:
#line 1335 "css_parse.y"
    {
	    int mbno = (yyvsp[(1) - (3)].el).El()->GetMemberNo((const char*)*((yyvsp[(3) - (3)].el).El()));
	    if(mbno < 0) { (yyval.ival) = Code3((yyvsp[(1) - (3)].el), (yyvsp[(3) - (3)].el), cssBI::points_at); }
	    else { cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
		   (yyval.ival) = Code3((yyvsp[(1) - (3)].el), tmpint, cssBI::points_at); } }
    break;

  case 270:
#line 1343 "css_parse.y"
    { (yyval.ival) = Code3(cssBI::push_root, (yyvsp[(2) - (2)].el), cssBI::points_at); }
    break;

  case 271:
#line 1344 "css_parse.y"
    {
	  cssMisc::cur_scope = NULL;
	    cssEl* scp = (yyvsp[(1) - (2)].el).El()->GetScoped((const char*)*((yyvsp[(2) - (2)].el).El()));
	    if(scp != &cssMisc::Void) {  (yyval.ival) = Code1(scp); }
	    else { (yyval.ival) = Code3((yyvsp[(1) - (2)].el), (yyvsp[(2) - (2)].el), cssBI::scoper); } }
    break;

  case 272:
#line 1350 "css_parse.y"
    {
	  /* argstop is put in by member_fun; member_fun skips over end jump, 
	     uses it to find member_call*/
	  (yyval.ival) = (yyvsp[(1) - (3)].el_ival).ival;
	  cssMisc::cur_top->Prog()->insts[(yyvsp[(2) - (3)].ival)-1]->SetJump(Code1(cssBI::member_call)); }
    break;

  case 273:
#line 1355 "css_parse.y"
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

  case 274:
#line 1372 "css_parse.y"
    { Code2((yyvsp[(3) - (4)].el), cssBI::member_fun);
                                                  (yyval.el_ival).ival = (yyvsp[(1) - (4)].ival); (yyval.el_ival).el.Reset(); }
    break;

  case 275:
#line 1374 "css_parse.y"
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

  case 276:
#line 1392 "css_parse.y"
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

  case 277:
#line 1416 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].nm))); }
    break;

  case 278:
#line 1420 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].nm))); }
    break;

  case 279:
#line 1422 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName())); }
    break;

  case 280:
#line 1424 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName())); }
    break;

  case 281:
#line 1426 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 282:
#line 1430 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 283:
#line 1434 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding function:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 284:
#line 1440 "css_parse.y"
    { /* these are treated identically in parsing */
           cssMisc::cur_top->parse_path_expr = true; }
    break;

  case 285:
#line 1442 "css_parse.y"
    {
           cssMisc::cur_top->parse_path_expr = true; }
    break;

  case 286:
#line 1446 "css_parse.y"
    {
   	   String tmpstr = String((yyvsp[(1) - (1)].nm));
           (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr);
           cssMisc::cur_top->parse_path_expr = false; }
    break;

  case 287:
#line 1450 "css_parse.y"
    {
           cssMisc::cur_top->parse_path_expr = false; }
    break;

  case 294:
#line 1462 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 295:
#line 1463 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 297:
#line 1467 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 298:
#line 1470 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 299:
#line 1471 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 301:
#line 1475 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 302:
#line 1476 "css_parse.y"
    { 
	  String tmpstr = String((yyvsp[(1) - (1)].nm));
	  (yyval.ival) = Code1(cssMisc::cur_top->AddLiteral(tmpstr)); }
    break;

  case 303:
#line 1479 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 304:
#line 1482 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); }
    break;

  case 308:
#line 1490 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 309:
#line 1493 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;


/* Line 1267 of yacc.c.  */
#line 4239 "y.tab.c"
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


#line 1497 "css_parse.y"


	/* end of grammar */


void yyerror(const char* s) { 	/* called for yacc syntax error */
  cssMisc::SyntaxError(s);
}

