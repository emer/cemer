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
     CSS_NOT = 325,
     CSS_UNARYMINUS = 326,
     CSS_SCOPER = 327,
     CSS_POINTSAT = 328
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
#include "css_ta.h"
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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef struct YYSTYPE
#line 59 "css_parse.y"
{
  cssElPlusIVal el_ival;
  cssElPtr     	el;		/* for coding */
  int		ival;		/* for program indexes (progdx) and other ints */
  char*        	nm;
}
/* Line 193 of yacc.c.  */
#line 287 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 300 "y.tab.c"

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
#define YYLAST   1879

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  306
/* YYNRULES -- Number of states.  */
#define YYNSTATES  510

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   328

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
     390,   394,   396,   400,   403,   406,   408,   412,   414,   416,
     419,   422,   425,   428,   431,   435,   439,   441,   444,   447,
     451,   453,   455,   458,   461,   464,   466,   468,   470,   473,
     475,   477,   479,   481,   483,   486,   488,   490,   492,   496,
     501,   504,   507,   509,   511,   515,   518,   520,   522,   524,
     531,   533,   540,   548,   550,   552,   555,   557,   559,   562,
     564,   566,   569,   571,   575,   579,   581,   585,   589,   593,
     595,   597,   599,   602,   607,   611,   612,   614,   616,   620,
     622,   624,   626,   628,   630,   632,   634,   636,   640,   644,
     648,   652,   656,   660,   664,   668,   672,   676,   680,   684,
     688,   694,   699,   703,   709,   713,   717,   720,   723,   727,
     731,   735,   739,   743,   747,   753,   757,   761,   765,   769,
     773,   777,   781,   785,   789,   793,   796,   799,   802,   805,
     808,   811,   816,   821,   826,   828,   832,   834,   836,   838,
     840,   842,   844,   847,   851,   854,   858,   862,   865,   868,
     872,   877,   882,   887,   891,   893,   895,   897,   899,   901,
     903,   905,   907,   909,   911,   913,   915,   917,   919,   921,
     923,   925,   927,   931,   933,   935,   937,   941,   943,   945,
     947,   949,   950,   952,   954,   958,   960
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      97,     0,    -1,    -1,    97,    90,    -1,    97,   100,    99,
      -1,    97,   106,    99,    -1,    97,   182,    -1,    97,   108,
      -1,    97,   152,    -1,    97,     1,    99,    -1,    91,    -1,
      91,    -1,    90,    -1,   101,    -1,   102,    -1,   103,    -1,
     104,    -1,   105,    -1,     3,    11,    -1,     4,    16,    -1,
       4,    15,    -1,     5,    15,    -1,     6,    16,    -1,     6,
      15,    -1,     7,    15,    -1,     7,    16,    -1,     8,    -1,
       9,    -1,    36,   107,    -1,    37,   186,   192,    -1,    37,
     186,   186,    -1,    38,    -1,    38,   200,   186,    -1,    38,
     200,    13,    -1,    38,   200,    37,    -1,    38,   200,    38,
      -1,    38,   200,    19,    -1,    -1,   200,   198,    -1,    86,
     200,   198,    92,    -1,   109,    -1,   110,    98,    -1,   111,
      -1,   114,    -1,   119,    -1,   137,    -1,   146,   144,   203,
      98,    -1,   146,   192,    49,   200,   183,   203,    -1,   191,
      49,   200,   183,   203,    98,    -1,   146,   192,   200,   112,
     203,    98,    -1,   113,    -1,   112,   113,    -1,    87,   183,
      93,    -1,   115,    47,   116,    94,   203,    98,    -1,    21,
     192,    -1,    21,    18,    -1,    21,    -1,   117,    -1,   116,
     117,    -1,   118,    -1,   118,    48,    -1,     1,    -1,   192,
      -1,   192,    49,    10,    -1,    20,   120,    47,   136,   128,
      94,   203,    98,    -1,    20,   120,    47,   136,    94,   203,
      98,    -1,    20,   122,   203,    98,    -1,   121,    -1,   121,
      61,   123,    -1,   192,    -1,    18,    -1,   192,    -1,    18,
      -1,   124,    -1,   123,    48,   124,    -1,   125,    -1,   147,
      -1,   126,   147,    -1,   127,    -1,   126,   127,    -1,    43,
      -1,    45,    -1,    42,    -1,   129,    -1,   128,   129,    -1,
     130,    -1,   134,    -1,   114,   136,    -1,    43,    61,    -1,
      42,    61,    -1,    44,    61,    -1,     1,    -1,   146,   192,
      98,   136,    -1,   146,   192,   131,   132,    98,   136,    -1,
      -1,   133,    -1,   132,   133,    -1,    87,    10,    93,    -1,
     135,   140,    98,   136,    -1,    18,   140,    98,   136,    -1,
      95,    18,   140,    98,   136,    -1,    45,    95,    18,   140,
      98,   136,    -1,   146,   192,    -1,    -1,    17,    -1,   136,
      17,    -1,   138,   140,   203,    98,    -1,   138,   140,   203,
     180,    -1,   146,    13,   140,    98,    -1,   146,    13,   140,
     203,   180,    -1,   139,   140,   203,   181,    -1,   146,   192,
      -1,   146,   149,   192,    -1,   149,    18,    -1,   149,    95,
      18,    -1,    86,    92,    -1,    86,   141,    92,    -1,    86,
      92,    41,    -1,    86,   141,    92,    41,    -1,   142,    -1,
     141,    48,   142,    -1,   143,    -1,   143,    49,   185,    -1,
     146,    16,    -1,   146,   195,    -1,   145,    -1,   144,    48,
     145,    -1,   192,    -1,   147,    -1,    39,   147,    -1,    40,
     147,    -1,    41,   147,    -1,    45,   147,    -1,    46,   147,
      -1,    46,    45,   147,    -1,    45,    46,   147,    -1,   148,
      -1,   148,   151,    -1,   148,    66,    -1,   148,   151,    66,
      -1,    18,    -1,    19,    -1,   149,    22,    -1,    18,    88,
      -1,    19,    88,    -1,    18,    -1,    19,    -1,    77,    -1,
     151,    77,    -1,   170,    -1,   173,    -1,   153,    -1,   154,
      -1,   180,    -1,   183,    98,    -1,   159,    -1,   168,    -1,
     166,    -1,    30,   200,    98,    -1,    30,   200,   183,    98,
      -1,    31,    98,    -1,    32,    98,    -1,   157,    -1,   155,
      -1,    28,   156,    61,    -1,    29,    61,    -1,    10,    -1,
      11,    -1,    12,    -1,   158,    86,   200,   183,    92,    47,
      -1,    27,    -1,   160,    86,   161,   163,   165,   152,    -1,
     160,    86,   201,   161,   163,   165,   152,    -1,    33,    -1,
     162,    -1,   162,   183,    -1,    91,    -1,   164,    -1,   164,
     201,    -1,    91,    -1,    92,    -1,   167,   152,    -1,    24,
      -1,   169,   177,    98,    -1,   169,   177,   152,    -1,    23,
      -1,   175,   177,   153,    -1,   175,   177,   154,    -1,   175,
     177,   180,    -1,   170,    -1,   153,    -1,   182,    -1,   176,
     153,    -1,   176,   171,   174,   172,    -1,   176,   171,   173,
      -1,    -1,    25,    -1,    26,    -1,   178,   183,   179,    -1,
      86,    -1,    92,    -1,    47,    -1,    47,    -1,    94,    -1,
     185,    -1,   184,    -1,   189,    -1,   183,    49,   183,    -1,
     183,    49,    47,    -1,   183,    59,   183,    -1,   183,    58,
     183,    -1,   183,    57,   183,    -1,   183,    56,   183,    -1,
     183,    55,   183,    -1,   183,    54,   183,    -1,   183,    53,
     183,    -1,   183,    52,   183,    -1,   183,    51,   183,    -1,
     183,    50,   183,    -1,    34,   200,   150,    -1,    34,   200,
     150,    86,    92,    -1,    34,   200,   150,   113,    -1,    35,
     200,   183,    -1,    35,    87,    93,   200,   183,    -1,   183,
      62,   183,    -1,   183,    63,   183,    -1,    66,   183,    -1,
      77,   183,    -1,   183,    72,   183,    -1,   183,    71,   183,
      -1,   183,    70,   183,    -1,   183,    69,   183,    -1,   183,
      68,   183,    -1,   183,    67,   183,    -1,   183,    60,   183,
      61,   183,    -1,   183,    75,   183,    -1,   183,    76,   183,
      -1,   183,    77,   183,    -1,   183,    78,   183,    -1,   183,
      79,   183,    -1,   183,    74,   183,    -1,   183,    73,   183,
      -1,   183,    66,   183,    -1,   183,    65,   183,    -1,   183,
      64,   183,    -1,   183,    81,    -1,    81,   183,    -1,   183,
      80,    -1,    80,   183,    -1,    76,   183,    -1,    83,   183,
      -1,   183,    87,   183,    93,    -1,    86,   146,    92,   183,
      -1,   146,    86,   183,    92,    -1,   187,    -1,    86,   183,
      92,    -1,    10,    -1,    11,    -1,    12,    -1,    14,    -1,
      36,    -1,    13,    -1,   188,    92,    -1,   188,   196,    92,
      -1,    13,    86,    -1,   184,   193,   194,    -1,   185,   193,
     194,    -1,   193,   194,    -1,   149,   194,    -1,   190,   203,
      92,    -1,   190,   203,   196,    92,    -1,   184,   193,   194,
      86,    -1,   185,   193,   194,    86,    -1,   149,   194,    86,
      -1,    16,    -1,    16,    -1,    10,    -1,    11,    -1,    12,
      -1,    14,    -1,    13,    -1,    85,    -1,    89,    -1,    16,
      -1,    11,    -1,    12,    -1,    14,    -1,    13,    -1,    19,
      -1,    10,    -1,   186,    -1,   197,    -1,   196,    48,   197,
      -1,   183,    -1,    19,    -1,   199,    -1,   198,    48,   199,
      -1,   183,    -1,    19,    -1,    16,    -1,    18,    -1,    -1,
     202,    -1,   110,    -1,   201,    48,   201,    -1,   183,    -1,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   162,   162,   163,   164,   165,   166,   169,   172,   173,
     176,   179,   180,   183,   184,   185,   186,   187,   192,   198,
     200,   201,   204,   207,   208,   211,   214,   218,   229,   231,
     235,   237,   239,   243,   247,   251,   255,   261,   262,   263,
     267,   268,   269,   270,   271,   272,   275,   285,   303,   323,
     339,   340,   344,   347,   355,   363,   371,   381,   382,   385,
     386,   387,   390,   408,   429,   433,   437,   443,   444,   447,
     454,   476,   482,   490,   491,   494,   504,   505,   508,   509,
     512,   513,   514,   517,   518,   521,   522,   523,   524,   525,
     526,   527,   530,   542,   558,   566,   567,   571,   575,   581,
     597,   614,   634,   655,   656,   657,   660,   666,   678,   686,
     704,   717,   730,   756,   777,   799,   800,   801,   802,   805,
     806,   809,   810,   816,   824,   834,   835,   842,   851,   852,
     853,   854,   856,   857,   858,   859,   862,   863,   865,   867,
     872,   873,   874,   885,   890,   894,   895,   898,   899,   902,
     903,   904,   907,   908,   911,   912,   913,   914,   915,   916,
     917,   918,   919,   920,   923,   932,   944,   945,   946,   950,
     967,   976,   991,  1014,  1022,  1023,  1026,  1034,  1035,  1038,
    1049,  1060,  1063,  1073,  1096,  1110,  1113,  1118,  1122,  1128,
    1129,  1130,  1133,  1138,  1140,  1144,  1149,  1153,  1172,  1180,
    1188,  1192,  1199,  1202,  1225,  1226,  1230,  1231,  1232,  1242,
    1243,  1244,  1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,
    1253,  1254,  1256,  1257,  1258,  1259,  1260,  1261,  1262,  1263,
    1264,  1265,  1266,  1267,  1268,  1269,  1270,  1271,  1272,  1273,
    1274,  1275,  1276,  1277,  1278,  1279,  1280,  1281,  1282,  1283,
    1284,  1285,  1286,  1292,  1298,  1299,  1303,  1304,  1305,  1306,
    1309,  1313,  1314,  1315,  1330,  1346,  1347,  1355,  1356,  1362,
    1367,  1384,  1386,  1404,  1428,  1432,  1434,  1436,  1438,  1442,
    1446,  1452,  1454,  1458,  1462,  1466,  1467,  1468,  1469,  1470,
    1471,  1474,  1475,  1478,  1479,  1482,  1483,  1486,  1487,  1488,
    1491,  1494,  1497,  1498,  1499,  1502,  1505
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
  "typeonly", "ptrs", "stmt", "miscbrastmt", "miscstmt", "caseitem",
  "caseexpr", "switchblock", "switch", "forloop", "for", "for_cond",
  "for_cond_sc", "for_incr", "for_incr_sc", "for_end_paren", "doloop",
  "do", "whiloop", "while", "ifstmt", "elseifstmt", "noelsestmt",
  "elsestmt", "popelse", "if", "else", "cond", "cond_paren",
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
     322,   323,   324,   325,   326,    46,    40,    91,   327,   328,
      10,    59,    41,    93,   125,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
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
     152,   152,   153,   153,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   155,   155,   156,   156,   156,   157,
     158,   159,   159,   160,   161,   161,   162,   163,   163,   164,
     165,   166,   167,   168,   168,   169,   170,   171,   171,   172,
     172,   172,   173,   173,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   183,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   185,   185,   185,   185,
     186,   187,   187,   187,   188,   189,   189,   189,   189,   189,
     189,   190,   190,   190,   191,   192,   192,   192,   192,   192,
     192,   193,   193,   194,   194,   195,   195,   195,   195,   195,
     195,   196,   196,   197,   197,   198,   198,   199,   199,   199,
     199,   200,   201,   201,   201,   202,   203
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
       3,     1,     3,     2,     2,     1,     3,     1,     1,     2,
       2,     2,     2,     2,     3,     3,     1,     2,     2,     3,
       1,     1,     2,     2,     2,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     3,     4,
       2,     2,     1,     1,     3,     2,     1,     1,     1,     6,
       1,     6,     7,     1,     1,     2,     1,     1,     2,     1,
       1,     2,     1,     3,     3,     1,     3,     3,     3,     1,
       1,     1,     2,     4,     3,     0,     1,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       5,     4,     3,     5,     3,     3,     2,     2,     3,     3,
       3,     3,     3,     3,     5,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     2,     2,
       2,     4,     4,     4,     1,     3,     1,     1,     1,     1,
       1,     1,     2,     3,     2,     3,     3,     2,     2,     3,
       4,     4,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       1,     0,     1,     1,     3,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,    26,
      27,   256,   257,   258,   261,   259,   274,   140,   141,     0,
      56,   185,   182,   196,   197,   170,     0,     0,   301,     0,
       0,   173,   301,   301,   301,     0,   301,     0,     0,     0,
       0,     0,   201,     0,     0,     0,     0,     0,     0,   281,
       0,   282,     3,   203,     0,    13,    14,    15,    16,    17,
       0,     7,    40,     0,    42,    43,     0,    44,    45,     0,
       0,     0,   128,   136,     0,     8,   151,   152,   163,   162,
       0,   155,     0,   157,     0,   156,     0,   149,   150,     0,
       0,   153,     6,     0,   205,   204,   254,     0,   206,   306,
       0,     0,    12,    11,     9,    18,    20,    19,    21,    23,
      22,    24,    25,   264,   143,   144,   276,   277,   278,   280,
     279,   275,    70,     0,    67,   306,    69,    55,    54,   166,
     167,   168,     0,   165,     0,    10,   160,   161,     0,     0,
       0,   301,    28,     0,   260,     0,     0,   129,     0,   130,
     131,     0,   132,     0,   133,     0,     0,   226,   249,   227,
     248,   246,   250,     0,     0,     4,     5,    41,     0,     0,
     306,   306,   280,     0,     0,     0,   306,   125,     0,   127,
     138,   147,   137,   284,   283,   113,   142,     0,   268,   301,
       0,   181,   199,     0,     0,     0,   192,   195,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   247,
     245,     0,   154,     0,     0,   141,   262,   293,     0,   291,
       0,   301,   267,   103,     0,     0,   164,   158,     0,   145,
     146,   219,   301,   222,     0,   299,   300,   298,   297,    38,
     295,    30,    29,    33,    36,    34,    35,    32,   135,   134,
       0,   255,    61,     0,    57,    59,    62,   115,     0,   119,
     121,     0,     0,     0,   306,     0,     0,     0,   112,   301,
       0,   139,   148,   114,   273,     0,   176,   303,     0,     0,
     174,   305,     0,   302,   183,   184,     0,   186,   194,     0,
       0,   208,   207,   218,   217,   216,   215,   214,   213,   212,
     211,   210,   209,     0,   224,   225,   244,   243,   242,   233,
     232,   231,   230,   229,   228,   241,   240,   235,   236,   237,
     238,   239,     0,   265,   266,     0,   263,   269,     0,     0,
     104,     0,    82,    80,    81,    68,    73,    75,     0,    78,
      76,    66,   159,     0,     0,   221,     0,     0,     0,   252,
     306,    58,    60,     0,   117,     0,   116,     0,   289,   285,
     287,   286,   123,   288,   290,   124,   106,   107,   202,   110,
     108,     0,   253,   126,   127,    46,     0,   306,    50,     0,
       0,   179,     0,   177,   175,     0,     0,   200,   198,   190,
     189,   193,   191,   187,   188,     0,   251,   271,   272,   292,
     270,   306,    91,   105,   140,     0,     0,     0,     0,   306,
       0,   103,     0,    83,    85,    86,     0,     0,     0,    79,
      77,   220,     0,   223,    39,   296,     0,    63,   120,   118,
     122,   109,   306,    51,     0,     0,   180,     0,   178,   304,
       0,   234,     0,     0,    89,    88,    90,     0,     0,     0,
      87,   306,    84,     0,    94,    74,    52,    53,    47,    49,
     169,   171,     0,    48,   103,     0,    65,     0,     0,   103,
     103,     0,   172,    99,     0,   103,    64,    98,    92,     0,
       0,    95,   103,   100,     0,   103,    96,   101,    97,    93
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,   136,   104,    54,    55,    56,    57,    58,    59,
      60,   142,    61,    62,   297,    64,   397,   365,   431,    66,
     273,   274,   275,    67,   123,   124,   125,   355,   356,   357,
     358,   359,   432,   433,   434,   491,   500,   501,   435,   436,
     351,    68,    69,    70,   170,   278,   279,   280,   176,   177,
     155,    72,    73,   156,   251,   182,    75,    76,    77,    78,
     132,    79,    80,    81,    82,   299,   300,   402,   403,   457,
      83,    84,    85,    86,    87,   197,   411,    88,   309,    89,
      90,   193,   194,   408,    91,   389,    92,    93,    94,    95,
     145,    96,    97,    98,    99,   100,   276,   101,   188,   385,
     238,   239,   259,   260,   134,   302,   303,   240
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -274
static const yytype_int16 yypact[] =
{
    -274,   514,  -274,    42,    15,   201,    21,   258,   267,  -274,
    -274,  -274,  -274,  -274,   -20,  -274,  -274,   -10,    -7,   212,
     489,  -274,  -274,  -274,  -274,  -274,   286,    30,  -274,    51,
      51,  -274,  -274,    63,   180,   120,   203,   312,   312,   312,
      12,   191,  -274,  1323,  1323,  1323,  1323,  1323,  1323,  -274,
    1323,  -274,  -274,  -274,    42,  -274,  -274,  -274,  -274,  -274,
      42,  -274,  -274,    51,  -274,  -274,   119,  -274,  -274,    90,
      90,    36,  -274,    50,    16,  -274,  -274,  -274,  -274,  -274,
     109,  -274,   113,  -274,   967,  -274,   122,  -274,  -274,   122,
    1047,  -274,  -274,  1710,    45,    45,  -274,   684,  -274,  -274,
     171,   170,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,
    -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,
    -274,  -274,   123,   190,   186,  -274,   159,  -274,  -274,  -274,
    -274,  -274,   199,  -274,   885,  -274,  -274,  -274,   319,   185,
    1323,  -274,  -274,  1207,  -274,   251,   175,  -274,   277,  -274,
    -274,   312,  -274,   312,  -274,   248,   167,   188,   263,   188,
     188,   188,   263,   -41,  1534,  -274,  -274,  -274,   356,   151,
    -274,  -274,    90,   -10,    -7,  1323,   303,  -274,   694,    53,
    -274,  -274,    88,  -274,  -274,  -274,  -274,   341,   275,  -274,
     867,  -274,  -274,   785,  1323,  1127,  -274,   337,   122,  1225,
    1323,  1323,  1323,  1323,  1323,  1323,  1323,  1323,  1323,  1323,
    1323,  1323,  1323,  1323,  1323,  1323,  1323,  1323,  1323,  1323,
    1323,  1323,  1323,  1323,  1323,  1323,  1323,  1323,  1323,  -274,
    -274,  1323,  -274,   170,   170,   -13,  -274,  1792,    17,  -274,
     702,  -274,  -274,   348,   142,    51,  -274,  -274,  1710,  -274,
    -274,   253,  -274,   188,  1207,  -274,   165,   270,  1792,   325,
    -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,
    1323,  -274,  -274,    58,  -274,   327,   338,   347,    29,  -274,
     340,    87,   -24,   343,    51,  1578,   694,    51,  -274,  -274,
     305,  -274,  -274,  -274,  -274,  1323,  -274,  -274,    82,   302,
    1323,  1792,    28,  -274,  -274,  -274,  1622,  -274,  -274,   599,
    1127,  -274,  1792,  1792,  1792,  1792,  1792,  1792,  1792,  1792,
    1792,  1792,  1792,  1753,  1387,   498,   582,   767,   948,  1022,
    1022,  1022,  1022,  1022,  1022,   304,   304,   265,   265,   188,
     188,   188,  1444,   309,   311,  1403,  -274,  -274,    32,  1323,
    -274,    43,  -274,  -274,  -274,   350,  -274,  -274,   142,  -274,
    -274,  -274,  -274,   308,  1323,  -274,  1323,    67,  1207,   188,
    -274,  -274,  -274,   392,  -274,   240,   363,   278,  -274,  -274,
    -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,
    -274,   358,  -274,  -274,  -274,  -274,  1323,   305,  -274,  1666,
     361,  -274,   315,  1305,  1792,  1305,   302,  -274,  -274,  -274,
    -274,  -274,  -274,  -274,  -274,  1323,  -274,  -274,  -274,  -274,
    -274,  1792,  -274,  -274,   -69,   351,   353,   354,    10,  -274,
     393,   348,   161,  -274,  -274,  -274,    90,   694,   142,  -274,
    -274,  -274,  1489,   188,  -274,  -274,    51,  -274,  -274,  -274,
    -274,  -274,  1792,  -274,    51,   371,  -274,   967,   372,  -274,
     315,   411,    51,    51,  -274,  -274,  -274,   401,    51,    90,
     404,  -274,  -274,    51,   148,  -274,  -274,  -274,  -274,  -274,
    -274,  -274,   967,  -274,   348,    90,  -274,    51,    51,   348,
     348,   335,  -274,   404,    51,   348,  -274,   404,   404,   416,
      26,  -274,   348,   404,   334,   348,  -274,   404,  -274,   404
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -274,  -274,   -30,    54,  -274,  -274,  -274,  -274,  -274,  -274,
    -274,  -274,  -274,  -274,   427,  -274,  -274,  -272,   428,  -274,
    -274,   158,  -274,  -274,  -274,  -274,  -274,  -274,    -1,  -274,
    -274,    81,  -274,     9,  -274,  -274,  -274,   -58,  -274,  -274,
    -257,  -274,  -274,  -274,   -60,  -274,    70,  -274,  -274,   162,
       3,   -25,  -274,     2,  -274,  -274,   -83,   -88,   137,  -274,
    -274,  -274,  -274,  -274,  -274,   147,  -274,    46,  -274,    -9,
    -274,  -274,  -274,  -274,   144,  -274,  -274,   257,  -274,   365,
    -274,   -78,  -274,  -274,  -273,  -274,   150,   101,  -274,    79,
    -138,  -274,  -274,  -274,  -274,  -274,   -14,   254,   -76,  -274,
     220,   116,   208,    95,   -12,  -230,  -274,   -35
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -302
static const yytype_int16 yytable[] =
{
     137,   191,   196,    74,    71,   126,   128,   261,   267,   387,
     171,   195,   147,   149,   150,   152,   154,   169,   398,   114,
     138,   140,   143,    42,   146,   242,   105,   183,    17,    18,
      17,    18,   184,   167,   185,  -294,   108,   414,   186,   148,
     148,   148,   148,   148,   422,   175,   116,   117,   118,   172,
     120,   270,   121,   163,   173,   174,   151,   179,   151,   272,
     423,   424,    18,   232,    20,   345,   113,   135,   116,   117,
     118,   119,   120,   178,   121,   115,   405,   375,   114,  -294,
     345,   115,    37,    38,    39,   425,   426,   427,   428,    41,
     245,   133,   116,   117,   118,   119,   120,   378,   121,   379,
     380,   381,   289,   382,   247,   467,   383,   307,   165,   346,
     305,   187,   284,   499,   166,   368,   180,   135,   451,   296,
     310,   376,   175,   144,   420,   453,   268,   181,   269,   254,
      49,   262,   102,   103,    51,   282,   283,   429,   430,  -111,
    -301,   287,   135,   384,   157,   158,   159,   160,   161,   162,
     139,   164,   370,   148,   291,   148,   144,   343,   344,   444,
      17,    18,   422,   304,   288,   292,   168,   290,   175,    17,
      18,   148,   281,   458,   470,   459,   169,   295,   183,   424,
      18,   183,    20,   184,   352,   353,   184,   354,   263,   186,
      37,    38,    39,   298,   264,   189,    40,    41,   237,   190,
      37,    38,    39,   425,   426,   427,   428,    41,   192,    17,
      18,   144,   265,   266,   -72,   361,   106,   107,   362,   360,
     241,   409,   116,   117,   118,   119,   120,   493,   121,   349,
     122,  -140,   497,   498,  -102,   248,   153,   243,   503,   135,
     366,   253,  -140,   277,   258,   507,   148,   244,   509,   391,
     -71,  -140,   386,   114,   390,   471,   430,   395,    17,    18,
     246,   116,   117,   118,   119,   120,   141,   121,   229,   230,
     -37,   -37,   394,   109,   110,   231,   285,   396,   252,    37,
      38,    39,   111,   112,   400,    40,    41,   144,    11,    12,
      13,   301,    15,   -31,   -31,   306,   129,   130,   131,   186,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
      17,    18,   342,   440,   175,   446,  -141,   249,   250,   363,
     364,   237,   226,   227,   228,   229,   230,  -141,   233,   234,
     231,   286,   231,   148,   437,   258,  -141,   272,   115,   293,
     148,   294,   454,    24,   463,   350,   116,   117,   118,   119,
     120,   369,   121,   368,   481,   372,   473,   148,   281,   224,
     225,   226,   227,   228,   229,   230,   462,   373,   374,   377,
     388,   231,   364,   401,   468,   417,   399,   418,   438,   492,
     441,   404,   447,   152,   449,    42,   298,   456,   298,   487,
     289,   469,   464,   360,   465,   466,   477,   478,   480,   485,
     405,   423,   499,   474,   479,   494,   504,   508,    63,    65,
     148,   371,   483,   484,   148,   437,   488,   475,   486,   439,
     148,   472,   506,   489,   490,   448,   237,   413,   393,   406,
     421,   482,   460,   410,   308,   198,   450,   495,   496,   412,
     348,   419,   367,   445,   502,   442,     0,   443,     0,   258,
     505,   210,     0,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,     0,     0,     0,     0,   452,   231,   116,
     117,   118,   119,   120,   301,   121,   301,   127,     0,     0,
       0,     0,     0,     0,     2,     3,   461,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,     0,
      16,     0,    17,    18,    19,    20,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,     0,     0,     0,    40,
      41,    42,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
      43,     0,     0,     0,     0,   231,     0,     0,     0,     0,
      44,    45,     0,     0,    46,    47,     0,    48,     0,    49,
      50,     0,     0,    51,    52,     0,     0,     0,    53,    11,
      12,    13,    14,    15,     0,     0,     0,    17,    18,     0,
       0,     0,    21,    22,    23,     0,    25,    26,    27,    28,
      29,    30,    31,    32,    33,     0,     0,     0,    37,    38,
      39,     0,     0,     0,    40,    41,    42,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,     0,    43,     0,     0,     0,   231,
       0,     0,     0,     0,     0,    44,    45,     0,     0,    46,
      47,     0,    48,     0,    49,    50,     0,     0,    51,     0,
       0,     0,     0,    53,    11,    12,    13,    14,    15,     0,
       0,     0,    17,   235,   116,   117,   118,   119,   120,     0,
     121,     0,    11,    12,    13,    14,    15,     0,    32,    33,
      17,   235,     0,    37,    38,    39,     0,     0,     0,    40,
      41,     0,     0,     0,     0,     0,    32,    33,     0,     0,
       0,    37,    38,    39,     0,     0,     0,    40,    41,     0,
      43,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      44,    45,     0,     0,    46,    47,     0,    48,    43,    49,
      50,     0,     0,    51,     0,     0,   236,     0,    44,    45,
       0,     0,    46,    47,     0,    48,     0,    49,    50,     0,
       0,    51,     0,     0,   347,    11,    12,    13,    14,    15,
       0,     0,     0,    17,    18,     0,     0,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     0,     0,     0,    37,    38,    39,     0,     0,     0,
      40,    41,    42,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,     0,
       0,    43,     0,     0,   231,     0,     0,     0,     0,     0,
       0,    44,    45,     0,     0,    46,    47,     0,    48,     0,
      49,    50,     0,     0,    51,     0,   135,    11,    12,    13,
      14,    15,     0,    16,     0,    17,    18,     0,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
       0,    32,    33,    17,    18,     0,    37,    38,    39,     0,
       0,     0,    40,    41,     0,     0,     0,     0,     0,    32,
      33,     0,     0,     0,    37,    38,    39,     0,     0,     0,
      40,    41,     0,    43,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    44,    45,     0,     0,    46,    47,     0,
      48,    43,    49,    50,     0,     0,    51,     0,   296,     0,
       0,    44,    45,     0,     0,    46,    47,     0,    48,     0,
      49,    50,     0,     0,    51,     0,   135,    11,    12,    13,
      14,    15,     0,     0,     0,    17,    18,     0,     0,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,     0,     0,     0,    37,    38,    39,     0,
       0,     0,    40,    41,    42,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
       0,     0,     0,    43,     0,   231,     0,     0,     0,     0,
       0,     0,     0,    44,    45,     0,     0,    46,    47,     0,
      48,     0,    49,    50,     0,     0,    51,    11,    12,    13,
      14,    15,     0,     0,     0,    17,    18,     0,     0,     0,
      21,    22,    23,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,     0,     0,     0,    37,    38,    39,     0,
       0,     0,    40,    41,    42,   222,   223,   224,   225,   226,
     227,   228,   229,   230,     0,     0,     0,     0,     0,   231,
       0,     0,     0,    43,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    44,    45,     0,     0,    46,    47,     0,
      48,     0,    49,    50,     0,     0,    51,    11,    12,    13,
      14,    15,     0,     0,     0,    17,    18,     0,     0,     0,
      21,    22,     0,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,     0,     0,     0,    37,    38,    39,     0,
       0,     0,    40,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    44,    45,     0,     0,    46,    47,     0,
      48,     0,    49,    50,     0,     0,    51,    11,    12,    13,
      14,    15,     0,   255,     0,   256,   257,     0,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
       0,    32,    33,    17,    18,     0,    37,    38,    39,     0,
       0,     0,    40,    41,     0,     0,     0,     0,     0,    32,
      33,     0,     0,     0,    37,    38,    39,     0,     0,     0,
      40,    41,   311,    43,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    44,    45,     0,     0,    46,    47,     0,
      48,    43,    49,    50,     0,     0,    51,     0,     0,     0,
       0,    44,    45,     0,     0,    46,    47,     0,    48,     0,
      49,    50,     0,     0,    51,    11,    12,    13,    14,    15,
       0,    16,     0,    17,    18,     0,     0,     0,     0,     0,
       0,     0,     0,    11,    12,    13,    14,    15,     0,    32,
      33,    17,    18,     0,    37,    38,    39,     0,     0,     0,
      40,    41,     0,     0,     0,     0,     0,    32,    33,     0,
       0,     0,    37,    38,    39,     0,     0,     0,    40,    41,
       0,    43,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    44,    45,     0,     0,    46,    47,     0,    48,    43,
      49,    50,     0,     0,    51,     0,     0,     0,     0,    44,
      45,     0,     0,    46,    47,     0,    48,     0,    49,    50,
       0,     0,    51,    11,    12,    13,    14,    15,     0,     0,
       0,    17,   235,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    32,    33,     0,
       0,     0,    37,    38,    39,     0,     0,     0,    40,    41,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,    43,
       0,     0,     0,     0,   231,     0,     0,     0,     0,    44,
      45,     0,     0,    46,    47,     0,    48,     0,    49,    50,
       0,     0,    51,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,     0,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,     0,     0,     0,     0,
       0,   231,     0,     0,     0,     0,     0,   416,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
       0,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,     0,     0,     0,     0,     0,   231,     0,     0,     0,
       0,     0,   476,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,     0,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,     0,     0,     0,     0,
       0,   231,     0,     0,     0,     0,   271,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,     0,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
       0,     0,     0,     0,     0,   231,     0,     0,     0,     0,
     392,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,     0,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,     0,     0,     0,     0,     0,   231,
       0,     0,     0,     0,   407,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,     0,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,     0,     0,
       0,     0,     0,   231,     0,     0,     0,     0,   455,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,     0,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,     0,     0,     0,     0,     0,   231,     0,     0,
       0,   135,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   415,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,     0,     0,     0,     0,     0,
     231,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,     0,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,     0,     0,     0,     0,     0,   231
};

static const yytype_int16 yycheck[] =
{
      30,    84,    90,     1,     1,    19,    20,   145,   146,   282,
      70,    89,    37,    38,    39,    40,    41,    86,   290,    88,
      32,    33,    34,    47,    36,   101,    11,    11,    18,    19,
      18,    19,    16,    63,    18,    48,    15,   310,    22,    37,
      38,    39,    40,    41,     1,    86,    10,    11,    12,    13,
      14,    92,    16,    50,    18,    19,    46,    71,    46,     1,
      17,    18,    19,    93,    21,    48,    86,    91,    10,    11,
      12,    13,    14,    71,    16,    88,    48,    48,    88,    92,
      48,    88,    39,    40,    41,    42,    43,    44,    45,    46,
     125,    61,    10,    11,    12,    13,    14,    10,    16,    12,
      13,    14,    49,    16,   134,    95,    19,   195,    54,    92,
     193,    95,   172,    87,    60,    48,    66,    91,   391,    91,
     198,    92,    86,    36,    92,   397,   151,    77,   153,   141,
      85,   145,    90,    91,    89,   170,   171,    94,    95,    86,
      87,   176,    91,   281,    43,    44,    45,    46,    47,    48,
      87,    50,    94,   151,    66,   153,    36,   233,   234,    92,
      18,    19,     1,   193,   178,    77,    47,   179,    86,    18,
      19,   169,   169,   403,   431,   405,    86,   189,    11,    18,
      19,    11,    21,    16,    42,    43,    16,    45,    13,    22,
      39,    40,    41,   190,    19,    86,    45,    46,    97,    86,
      39,    40,    41,    42,    43,    44,    45,    46,    86,    18,
      19,    36,    37,    38,    91,   245,    15,    16,   248,   244,
      49,   309,    10,    11,    12,    13,    14,   484,    16,   241,
      18,    66,   489,   490,    86,   134,    45,    47,   495,    91,
     252,   140,    77,    92,   143,   502,   244,    61,   505,   284,
      91,    86,   282,    88,   284,    94,    95,   287,    18,    19,
      61,    10,    11,    12,    13,    14,    86,    16,    80,    81,
      90,    91,   286,    15,    16,    87,   175,   289,    93,    39,
      40,    41,    15,    16,   298,    45,    46,    36,    10,    11,
      12,   190,    14,    90,    91,   194,    10,    11,    12,    22,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
      18,    19,   231,   358,    86,   370,    66,    18,    19,    86,
      87,   240,    77,    78,    79,    80,    81,    77,    94,    95,
      87,    48,    87,   351,   351,   254,    86,     1,    88,    18,
     358,    86,   397,    26,   424,    17,    10,    11,    12,    13,
      14,   270,    16,    48,   457,    48,   436,   375,   375,    75,
      76,    77,    78,    79,    80,    81,   421,    49,    41,    49,
      47,    87,    87,    91,   429,    86,   295,    86,    48,   482,
      92,   300,    10,   428,    41,    47,   403,    92,   405,   469,
      49,    18,    61,   438,    61,    61,   446,   452,    47,    18,
      48,    17,    87,   437,   454,   485,    10,    93,     1,     1,
     428,   273,   462,   463,   432,   432,   471,   438,   468,   358,
     438,   432,   500,   473,   474,   375,   345,   310,   286,   302,
     349,   460,   406,   309,   197,    90,   377,   487,   488,   309,
     240,   345,   254,   368,   494,   364,    -1,   366,    -1,   368,
     500,    60,    -1,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    -1,    -1,    -1,    -1,   396,    87,    10,
      11,    12,    13,    14,   403,    16,   405,    18,    -1,    -1,
      -1,    -1,    -1,    -1,     0,     1,   415,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      16,    -1,    18,    19,    20,    21,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    47,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      66,    -1,    -1,    -1,    -1,    87,    -1,    -1,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,    -1,    83,    -1,    85,
      86,    -1,    -1,    89,    90,    -1,    -1,    -1,    94,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    18,    19,    -1,
      -1,    -1,    23,    24,    25,    -1,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    39,    40,
      41,    -1,    -1,    -1,    45,    46,    47,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    66,    -1,    -1,    -1,    87,
      -1,    -1,    -1,    -1,    -1,    76,    77,    -1,    -1,    80,
      81,    -1,    83,    -1,    85,    86,    -1,    -1,    89,    -1,
      -1,    -1,    -1,    94,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    19,    10,    11,    12,    13,    14,    -1,
      16,    -1,    10,    11,    12,    13,    14,    -1,    34,    35,
      18,    19,    -1,    39,    40,    41,    -1,    -1,    -1,    45,
      46,    -1,    -1,    -1,    -1,    -1,    34,    35,    -1,    -1,
      -1,    39,    40,    41,    -1,    -1,    -1,    45,    46,    -1,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,    -1,    83,    66,    85,
      86,    -1,    -1,    89,    -1,    -1,    92,    -1,    76,    77,
      -1,    -1,    80,    81,    -1,    83,    -1,    85,    86,    -1,
      -1,    89,    -1,    -1,    92,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,
      45,    46,    47,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      -1,    66,    -1,    -1,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    77,    -1,    -1,    80,    81,    -1,    83,    -1,
      85,    86,    -1,    -1,    89,    -1,    91,    10,    11,    12,
      13,    14,    -1,    16,    -1,    18,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,    14,
      -1,    34,    35,    18,    19,    -1,    39,    40,    41,    -1,
      -1,    -1,    45,    46,    -1,    -1,    -1,    -1,    -1,    34,
      35,    -1,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,
      45,    46,    -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    80,    81,    -1,
      83,    66,    85,    86,    -1,    -1,    89,    -1,    91,    -1,
      -1,    76,    77,    -1,    -1,    80,    81,    -1,    83,    -1,
      85,    86,    -1,    -1,    89,    -1,    91,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    39,    40,    41,    -1,
      -1,    -1,    45,    46,    47,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      -1,    -1,    -1,    66,    -1,    87,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    80,    81,    -1,
      83,    -1,    85,    86,    -1,    -1,    89,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      23,    24,    25,    -1,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    39,    40,    41,    -1,
      -1,    -1,    45,    46,    47,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,    87,
      -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    80,    81,    -1,
      83,    -1,    85,    86,    -1,    -1,    89,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      23,    24,    -1,    -1,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    39,    40,    41,    -1,
      -1,    -1,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    80,    81,    -1,
      83,    -1,    85,    86,    -1,    -1,    89,    10,    11,    12,
      13,    14,    -1,    16,    -1,    18,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,    14,
      -1,    34,    35,    18,    19,    -1,    39,    40,    41,    -1,
      -1,    -1,    45,    46,    -1,    -1,    -1,    -1,    -1,    34,
      35,    -1,    -1,    -1,    39,    40,    41,    -1,    -1,    -1,
      45,    46,    47,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    80,    81,    -1,
      83,    66,    85,    86,    -1,    -1,    89,    -1,    -1,    -1,
      -1,    76,    77,    -1,    -1,    80,    81,    -1,    83,    -1,
      85,    86,    -1,    -1,    89,    10,    11,    12,    13,    14,
      -1,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,    34,
      35,    18,    19,    -1,    39,    40,    41,    -1,    -1,    -1,
      45,    46,    -1,    -1,    -1,    -1,    -1,    34,    35,    -1,
      -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,    46,
      -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    77,    -1,    -1,    80,    81,    -1,    83,    66,
      85,    86,    -1,    -1,    89,    -1,    -1,    -1,    -1,    76,
      77,    -1,    -1,    80,    81,    -1,    83,    -1,    85,    86,
      -1,    -1,    89,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    35,    -1,
      -1,    -1,    39,    40,    41,    -1,    -1,    -1,    45,    46,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    66,
      -1,    -1,    -1,    -1,    87,    -1,    -1,    -1,    -1,    76,
      77,    -1,    -1,    80,    81,    -1,    83,    -1,    85,    86,
      -1,    -1,    89,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    -1,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,
      -1,    87,    -1,    -1,    -1,    -1,    -1,    93,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,    -1,
      -1,    -1,    93,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    -1,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,
      -1,    87,    -1,    -1,    -1,    -1,    92,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,    -1,    -1,
      92,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,    87,
      -1,    -1,    -1,    -1,    92,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    -1,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
      -1,    -1,    -1,    87,    -1,    -1,    -1,    -1,    92,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,
      -1,    91,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,
      87,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,    87
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    97,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    16,    18,    19,    20,
      21,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      45,    46,    47,    66,    76,    77,    80,    81,    83,    85,
      86,    89,    90,    94,   100,   101,   102,   103,   104,   105,
     106,   108,   109,   110,   111,   114,   115,   119,   137,   138,
     139,   146,   147,   148,   149,   152,   153,   154,   155,   157,
     158,   159,   160,   166,   167,   168,   169,   170,   173,   175,
     176,   180,   182,   183,   184,   185,   187,   188,   189,   190,
     191,   193,    90,    91,    99,    11,    15,    16,    15,    15,
      16,    15,    16,    86,    88,    88,    10,    11,    12,    13,
      14,    16,    18,   120,   121,   122,   192,    18,   192,    10,
      11,    12,   156,    61,   200,    91,    98,    98,   200,    87,
     200,    86,   107,   200,    36,   186,   200,   147,   149,   147,
     147,    46,   147,    45,   147,   146,   149,   183,   183,   183,
     183,   183,   183,   146,   183,    99,    99,    98,    47,    86,
     140,   140,    13,    18,    19,    86,   144,   145,   149,   192,
      66,    77,   151,    11,    16,    18,    22,    95,   194,    86,
      86,   152,    86,   177,   178,   177,   153,   171,   175,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    87,    98,   193,   193,    19,    92,   183,   196,   197,
     203,    49,   194,    47,    61,   203,    61,    98,   183,    18,
      19,   150,    93,   183,   200,    16,    18,    19,   183,   198,
     199,   186,   192,    13,    19,    37,    38,   186,   147,   147,
      92,    92,     1,   116,   117,   118,   192,    92,   141,   142,
     143,   146,   203,   203,   140,   183,    48,   203,   192,    49,
     200,    66,    77,    18,    86,   200,    91,   110,   146,   161,
     162,   183,   201,   202,    98,   152,   183,   153,   173,   174,
     177,    47,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   194,   194,    48,    92,    92,   196,   200,
      17,   136,    42,    43,    45,   123,   124,   125,   126,   127,
     147,    98,    98,    86,    87,   113,   200,   198,    48,   183,
      94,   117,    48,    49,    41,    48,    92,    49,    10,    12,
      13,    14,    16,    19,   186,   195,    98,   180,    47,   181,
      98,   203,    92,   145,   192,    98,   200,   112,   113,   183,
     192,    91,   163,   164,   183,    48,   161,    92,   179,   153,
     170,   172,   182,   154,   180,    61,    93,    86,    86,   197,
      92,   183,     1,    17,    18,    42,    43,    44,    45,    94,
      95,   114,   128,   129,   130,   134,   135,   146,    48,   127,
     147,    92,   183,   183,    92,   199,   203,    10,   142,    41,
     185,   180,   183,   113,   203,    92,    92,   165,   201,   201,
     163,   183,   203,   140,    61,    61,    61,    95,   203,    18,
     136,    94,   129,   140,   192,   124,    93,    98,   203,    98,
      47,   152,   165,    98,    98,    18,    98,   140,   203,    98,
      98,   131,   152,   136,   140,    98,    98,   136,   136,    87,
     132,   133,    98,   136,    10,    98,   133,   136,    93,   136
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
#line 163 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Blank; }
    break;

  case 4:
#line 164 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 5:
#line 165 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return (yyvsp[(2) - (3)].ival); }
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
            cssEl* tmp = (yyvsp[(2) - (2)].el).El();
	    cssMisc::cur_top->SetInclude((const char*)*tmp);
	    cssEl::Done((yyvsp[(2) - (2)].el).El()); }
    break;

  case 19:
#line 198 "css_parse.y"
    {
            cssMisc::Defines.Push(new cssDef(0, (yyvsp[(2) - (2)].nm))); }
    break;

  case 21:
#line 201 "css_parse.y"
    { cssMisc::Defines.Remove((yyvsp[(2) - (2)].el).El()); }
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
            Code1((yyvsp[(1) - (2)].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 29:
#line 231 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssRef((yyvsp[(2) - (3)].el)));
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 30:
#line 235 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc; }
    break;

  case 31:
#line 237 "css_parse.y"
    {
	    Code1((yyvsp[(1) - (1)].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 32:
#line 239 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 33:
#line 243 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 34:
#line 247 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 35:
#line 251 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 36:
#line 255 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 37:
#line 261 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 39:
#line 263 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 46:
#line 275 "css_parse.y"
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
#line 285 "css_parse.y"
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
#line 303 "css_parse.y"
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
#line 323 "css_parse.y"
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
#line 339 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 51:
#line 340 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 52:
#line 344 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 53:
#line 347 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(5) - (6)].ival)-1]->SetJump((yyvsp[(5) - (6)].ival)); (yyval.ival) = (yyvsp[(5) - (6)].ival)-1;
	    if(cssMisc::cur_enum->name.before(5) == "enum_") /* don't keep anon enums around */
	      delete cssMisc::cur_enum;
	    cssMisc::parsing_args = false; /* needed for - numbers */
	    cssMisc::cur_enum = NULL; }
    break;

  case 54:
#line 355 "css_parse.y"
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
#line 363 "css_parse.y"
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
#line 371 "css_parse.y"
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
#line 387 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 62:
#line 390 "css_parse.y"
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
#line 408 "css_parse.y"
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
#line 429 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(7) - (8)].ival)-1]->SetJump((yyvsp[(7) - (8)].ival)); (yyval.ival) = (yyvsp[(7) - (8)].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[(4) - (8)].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 65:
#line 433 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(6) - (7)].ival)-1]->SetJump((yyvsp[(6) - (7)].ival)); (yyval.ival) = (yyvsp[(6) - (7)].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[(4) - (7)].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 66:
#line 437 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(3) - (4)].ival)); (yyval.ival) = (yyvsp[(3) - (4)].ival)-1;
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 69:
#line 447 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssMisc::cur_class = new cssClassType((const char*)*nm);
	    cssMisc::cur_class->last_top = cssMisc::cur_top;
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
	    cssEl::Done(nm); }
    break;

  case 70:
#line 454 "css_parse.y"
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
#line 476 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_class = new cssClassType((const char*)*nm);
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
            cssEl::Done(nm); }
    break;

  case 72:
#line 482 "css_parse.y"
    {
            if((yyvsp[(1) - (1)].el).El()->GetType() != cssEl::T_ClassType) {
              yyerror("Attempt to redefine non-class type as a class");
              return cssProg::YY_Err;
            }
            cssMisc::cur_class = (cssClassType*)(yyvsp[(1) - (1)].el).El(); }
    break;

  case 75:
#line 494 "css_parse.y"
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
#line 505 "css_parse.y"
    { (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 86:
#line 522 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 87:
#line 523 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 91:
#line 527 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 92:
#line 530 "css_parse.y"
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
#line 542 "css_parse.y"
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
#line 558 "css_parse.y"
    {
            /* swap places with void marker */
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_top->Prog()->Stack()->Push(&cssMisc::Void);
            cssMisc::cur_top->Prog()->Stack()->Push(nm); }
    break;

  case 95:
#line 566 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 96:
#line 567 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 97:
#line 571 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((int)*(yyvsp[(2) - (3)].el).El())); }
    break;

  case 98:
#line 575 "css_parse.y"
    {       /* pre-declare function */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[(1) - (4)].el).El();
            cssMisc::cur_class->GetComments(fun, (yyvsp[(4) - (4)].el));
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr, true); }
    break;

  case 99:
#line 581 "css_parse.y"
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
#line 597 "css_parse.y"
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
#line 614 "css_parse.y"
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
#line 634 "css_parse.y"
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
#line 655 "css_parse.y"
    { (yyval.el).Reset(); }
    break;

  case 104:
#line 656 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (1)].el); }
    break;

  case 105:
#line 657 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (2)].el); }
    break;

  case 106:
#line 660 "css_parse.y"
    {	/* pre-declare function */
            cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(3) - (4)].ival));	(yyval.ival) = (yyvsp[(3) - (4)].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[(1) - (4)].el).El();
	    fun->argc = (yyvsp[(2) - (4)].ival); fun->GetArgDefs(); }
    break;

  case 107:
#line 666 "css_parse.y"
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
#line 678 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 109:
#line 686 "css_parse.y"
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
#line 704 "css_parse.y"
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
#line 717 "css_parse.y"
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
#line 730 "css_parse.y"
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
#line 756 "css_parse.y"
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
#line 777 "css_parse.y"
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
#line 799 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 116:
#line 800 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 117:
#line 801 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 118:
#line 802 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 119:
#line 805 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 120:
#line 806 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 121:
#line 809 "css_parse.y"
    { cssMisc::parsing_args = false; }
    break;

  case 122:
#line 810 "css_parse.y"
    {
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push((yyvsp[(3) - (3)].el).El()); }
    break;

  case 123:
#line 816 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (2)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[(2) - (2)].nm)));
	    ((yyvsp[(1) - (2)].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 124:
#line 824 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (2)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    (yyval.el) = cssMisc::ConstExpr->Stack()->Push(new cssString((yyvsp[(2) - (2)].el).El()->GetName()));
	    ((yyvsp[(1) - (2)].el).El())->MakeToken(cssMisc::ConstExpr); }
    break;

  case 126:
#line 835 "css_parse.y"
    {
	    if(((yyvsp[(1) - (3)].ival) < 0) && ((yyvsp[(3) - (3)].ival) < 0)) (yyval.ival) = -1;
	    else if(((yyvsp[(1) - (3)].ival) >= 0) && ((yyvsp[(3) - (3)].ival) < 0)) (yyval.ival) = (yyvsp[(1) - (3)].ival);
	    else if(((yyvsp[(1) - (3)].ival) < 0) && ((yyvsp[(3) - (3)].ival) >= 0)) (yyval.ival) = (yyvsp[(3) - (3)].ival);
	    else (yyval.ival) = -1; }
    break;

  case 127:
#line 842 "css_parse.y"
    {
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      (yyval.ival) = Code2(tmp->ptr, cssBI::constr); }
	    else (yyval.ival) = -1;
	    cssEl::Done(tmp); }
    break;

  case 128:
#line 851 "css_parse.y"
    { (yyvsp[(1) - (1)].el).El()->tmp_str = ""; }
    break;

  case 129:
#line 852 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "extern"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 130:
#line 853 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "static"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 131:
#line 854 "css_parse.y"
    { /* use const expr prog to code stuff */
	  (yyvsp[(2) - (2)].el).El()->tmp_str = "const"; (yyval.el) = (yyvsp[(2) - (2)].el); cssMisc::CodeConstExpr(); }
    break;

  case 132:
#line 856 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 133:
#line 857 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = ""; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 134:
#line 858 "css_parse.y"
    { (yyvsp[(3) - (3)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(3) - (3)].el); }
    break;

  case 135:
#line 859 "css_parse.y"
    { (yyvsp[(3) - (3)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(3) - (3)].el); }
    break;

  case 136:
#line 862 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (1)].el); cssMisc::cur_type = (yyval.el); }
    break;

  case 137:
#line 863 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->GetPtrType((yyvsp[(1) - (2)].el).El(), (yyvsp[(2) - (2)].ival)); cssMisc::cur_type = (yyval.el); }
    break;

  case 138:
#line 865 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->GetRefType((yyvsp[(1) - (2)].el).El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 139:
#line 867 "css_parse.y"
    {
	    cssElPtr pt = cssMisc::cur_top->GetPtrType((yyvsp[(1) - (3)].el).El(), (yyvsp[(2) - (3)].ival));
            (yyval.el) = cssMisc::cur_top->GetRefType(pt.El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 142:
#line 874 "css_parse.y"
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

  case 143:
#line 885 "css_parse.y"
    {
          if(((yyvsp[(1) - (2)].el).El()->GetType() != cssEl::T_ClassType) && ((yyvsp[(1) - (2)].el).El()->GetType() != cssEl::T_EnumType)) {
	     yyerror("scoping of non-class or enum type");
	     return cssProg::YY_Err; }
           cssMisc::cur_scope = (yyvsp[(1) - (2)].el).El(); }
    break;

  case 144:
#line 890 "css_parse.y"
    {
           cssMisc::cur_scope = (yyvsp[(1) - (2)].el).El(); }
    break;

  case 147:
#line 898 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 148:
#line 899 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 151:
#line 904 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastif = -1; }
    break;

  case 154:
#line 911 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 158:
#line 915 "css_parse.y"
    { Code1((yyvsp[(1) - (3)].el)); (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 159:
#line 916 "css_parse.y"
    { Code1((yyvsp[(1) - (4)].el)); (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 160:
#line 917 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (2)].el)); }
    break;

  case 161:
#line 918 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (2)].el)); }
    break;

  case 164:
#line 923 "css_parse.y"
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

  case 165:
#line 932 "css_parse.y"
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

  case 169:
#line 950 "css_parse.y"
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

  case 170:
#line 967 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssSwitchBlock_Name);
	    blk->loop_type = cssCodeBlock::SWITCH;
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 171:
#line 976 "css_parse.y"
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

  case 172:
#line 991 "css_parse.y"
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

  case 173:
#line 1014 "css_parse.y"
    { /* for loop contained within own block */
            cssCodeBlock* blk = new cssCodeBlock(cssForLoop_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 176:
#line 1026 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 179:
#line 1038 "css_parse.y"
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

  case 180:
#line 1049 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of incr */
	      cssCodeBlock* blk = new cssCodeBlock(cssForLoopStmt_Name);
	      blk->owner_prog = cssMisc::cur_top->Prog();
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_type = cssCodeBlock::FOR;
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 182:
#line 1063 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssDoLoop_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->loop_type = cssCodeBlock::DO;
	    cssMisc::cur_top->AddStatic(blk); /* while is all inside this do! */
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 183:
#line 1073 "css_parse.y"
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

  case 184:
#line 1096 "css_parse.y"
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

  case 185:
#line 1110 "css_parse.y"
    { }
    break;

  case 186:
#line 1113 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 187:
#line 1118 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          cssMisc::cur_top->Prog()->lastelseif = false; /* use this flag! */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 188:
#line 1122 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          cssMisc::cur_top->Prog()->lastelseif = true; /* be on guard for another else! */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); cssMisc::cur_top->DoCompileCtrl(); /* need to do the bra! */ }
    break;

  case 192:
#line 1133 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); /* pop the if_false block */
	    /* now check for other else blocks that need popping! */
	    cssMisc::cur_top->PopElseBlocks(); /* pop residual elses! */
	    (yyval.ival) = (yyvsp[(1) - (2)].ival);  }
    break;

  case 193:
#line 1138 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[(1) - (4)].ival);  }
    break;

  case 194:
#line 1140 "css_parse.y"
    { /* do not pop if going into another else */
	  (yyval.ival) = (yyvsp[(1) - (3)].ival);  }
    break;

  case 195:
#line 1144 "css_parse.y"
    {
	  if(!cssMisc::cur_top->Prog()->lastelseif) {
	    cssMisc::cur_top->PopElseBlocks(); } }
    break;

  case 196:
#line 1149 "css_parse.y"
    {
          cssMisc::cur_top->Prog()->lastif = cssMisc::cur_top->Prog()->size; }
    break;

  case 197:
#line 1153 "css_parse.y"
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
            cssCodeBlock* blk = new cssCodeBlock(cssElseBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::ELSE; /* start block of else */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code2(blk, cssBI::pop); /* pop after else to get rid of prev if cond */
	    cssMisc::cur_top->Push(blk->code); }
    break;

  case 198:
#line 1172 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssIfTrueBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	    cssMisc::cur_top->AddStatic(blk);
	    Code1(blk); cssMisc::cur_top->Push(blk->code); (yyval.ival) = (yyvsp[(1) - (3)].ival); }
    break;

  case 199:
#line 1180 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 200:
#line 1188 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); }
    break;

  case 201:
#line 1192 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->SetPush(blk->code); }
    break;

  case 202:
#line 1199 "css_parse.y"
    {  }
    break;

  case 203:
#line 1202 "css_parse.y"
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

  case 204:
#line 1225 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 207:
#line 1231 "css_parse.y"
    { Code1(cssBI::asgn); }
    break;

  case 208:
#line 1232 "css_parse.y"
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

  case 209:
#line 1242 "css_parse.y"
    { Code1(cssBI::asgn_add); }
    break;

  case 210:
#line 1243 "css_parse.y"
    { Code1(cssBI::asgn_sub); }
    break;

  case 211:
#line 1244 "css_parse.y"
    { Code1(cssBI::asgn_mult); }
    break;

  case 212:
#line 1245 "css_parse.y"
    { Code1(cssBI::asgn_div); }
    break;

  case 213:
#line 1246 "css_parse.y"
    { Code1(cssBI::asgn_mod); }
    break;

  case 214:
#line 1247 "css_parse.y"
    { Code1(cssBI::asgn_lshift); }
    break;

  case 215:
#line 1248 "css_parse.y"
    { Code1(cssBI::asgn_rshift); }
    break;

  case 216:
#line 1249 "css_parse.y"
    { Code1(cssBI::asgn_and); }
    break;

  case 217:
#line 1250 "css_parse.y"
    { Code1(cssBI::asgn_xor); }
    break;

  case 218:
#line 1251 "css_parse.y"
    { Code1(cssBI::asgn_or); }
    break;

  case 219:
#line 1252 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code2((yyvsp[(3) - (3)].el),(yyvsp[(1) - (3)].el)); }
    break;

  case 220:
#line 1253 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (5)].ival); Code2((yyvsp[(3) - (5)].el),(yyvsp[(1) - (5)].el)); }
    break;

  case 221:
#line 1254 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[(2) - (4)].ival); Code2((yyvsp[(3) - (4)].el),(yyvsp[(1) - (4)].el)); }
    break;

  case 222:
#line 1256 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code1((yyvsp[(1) - (3)].el)); }
    break;

  case 223:
#line 1257 "css_parse.y"
    { (yyval.ival) = (yyvsp[(4) - (5)].ival); Code1((yyvsp[(1) - (5)].el)); }
    break;

  case 224:
#line 1258 "css_parse.y"
    { Code1(cssBI::lor); }
    break;

  case 225:
#line 1259 "css_parse.y"
    { Code1(cssBI::land); }
    break;

  case 226:
#line 1260 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::addr_of); }
    break;

  case 227:
#line 1261 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::de_ptr); }
    break;

  case 228:
#line 1262 "css_parse.y"
    { Code1(cssBI::gt); }
    break;

  case 229:
#line 1263 "css_parse.y"
    { Code1(cssBI::ge); }
    break;

  case 230:
#line 1264 "css_parse.y"
    { Code1(cssBI::lt); }
    break;

  case 231:
#line 1265 "css_parse.y"
    { Code1(cssBI::le); }
    break;

  case 232:
#line 1266 "css_parse.y"
    { Code1(cssBI::eq); }
    break;

  case 233:
#line 1267 "css_parse.y"
    { Code1(cssBI::ne); }
    break;

  case 234:
#line 1268 "css_parse.y"
    { Code1(cssBI::cond); }
    break;

  case 235:
#line 1269 "css_parse.y"
    { Code1(cssBI::add); }
    break;

  case 236:
#line 1270 "css_parse.y"
    { Code1(cssBI::sub); }
    break;

  case 237:
#line 1271 "css_parse.y"
    { Code1(cssBI::mul); }
    break;

  case 238:
#line 1272 "css_parse.y"
    { Code1(cssBI::div); }
    break;

  case 239:
#line 1273 "css_parse.y"
    { Code1(cssBI::modulo); }
    break;

  case 240:
#line 1274 "css_parse.y"
    { Code1(cssBI::lshift); }
    break;

  case 241:
#line 1275 "css_parse.y"
    { Code1(cssBI::rshift); }
    break;

  case 242:
#line 1276 "css_parse.y"
    { Code1(cssBI::bit_and); }
    break;

  case 243:
#line 1277 "css_parse.y"
    { Code1(cssBI::bit_xor); }
    break;

  case 244:
#line 1278 "css_parse.y"
    { Code1(cssBI::bit_or); }
    break;

  case 245:
#line 1279 "css_parse.y"
    { Code1(cssBI::asgn_post_pp); }
    break;

  case 246:
#line 1280 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::asgn_pre_pp); }
    break;

  case 247:
#line 1281 "css_parse.y"
    { Code1(cssBI::asgn_post_mm); }
    break;

  case 248:
#line 1282 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::asgn_pre_mm); }
    break;

  case 249:
#line 1283 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::neg); }
    break;

  case 250:
#line 1284 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::lnot); }
    break;

  case 251:
#line 1285 "css_parse.y"
    { Code1(cssBI::de_array); }
    break;

  case 252:
#line 1286 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(2) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = (yyvsp[(4) - (4)].ival); Code2((yyvsp[(2) - (4)].el), cssBI::cast); }
    break;

  case 253:
#line 1292 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
  	    (yyval.ival) = (yyvsp[(3) - (4)].ival); Code2((yyvsp[(1) - (4)].el), cssBI::cast); }
    break;

  case 255:
#line 1299 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 261:
#line 1313 "css_parse.y"
    { (yyval.ival) = Code2(cssBI::push_next, (yyvsp[(1) - (1)].el)); }
    break;

  case 262:
#line 1314 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].el_ival).ival; Code1((yyvsp[(1) - (2)].el_ival).el); }
    break;

  case 263:
#line 1315 "css_parse.y"
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

  case 264:
#line 1330 "css_parse.y"
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

  case 265:
#line 1346 "css_parse.y"
    { Code2((yyvsp[(3) - (3)].el), cssBI::points_at);}
    break;

  case 266:
#line 1347 "css_parse.y"
    {
	    int mbno = (yyvsp[(1) - (3)].el).El()->GetMemberNo((const char*)*((yyvsp[(3) - (3)].el).El()));
	    if(mbno < 0) { (yyval.ival) = Code3((yyvsp[(1) - (3)].el), (yyvsp[(3) - (3)].el), cssBI::points_at); }
	    else { cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
		   (yyval.ival) = Code3((yyvsp[(1) - (3)].el), tmpint, cssBI::points_at); } }
    break;

  case 267:
#line 1355 "css_parse.y"
    { (yyval.ival) = Code3(cssBI::push_root, (yyvsp[(2) - (2)].el), cssBI::points_at); }
    break;

  case 268:
#line 1356 "css_parse.y"
    {
	  cssMisc::cur_scope = NULL;
	    cssEl* scp = (yyvsp[(1) - (2)].el).El()->GetScoped((const char*)*((yyvsp[(2) - (2)].el).El()));
	    if(scp != &cssMisc::Void) {  (yyval.ival) = Code1(scp); }
	    else { (yyval.ival) = Code3((yyvsp[(1) - (2)].el), (yyvsp[(2) - (2)].el), cssBI::scoper); } }
    break;

  case 269:
#line 1362 "css_parse.y"
    {
	  /* argstop is put in by member_fun; member_fun skips over end jump, 
	     uses it to find member_call*/
	  (yyval.ival) = (yyvsp[(1) - (3)].el_ival).ival;
	  cssMisc::cur_top->Prog()->insts[(yyvsp[(2) - (3)].ival)-1]->SetJump(Code1(cssBI::member_call)); }
    break;

  case 270:
#line 1367 "css_parse.y"
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

  case 271:
#line 1384 "css_parse.y"
    { Code2((yyvsp[(3) - (4)].el), cssBI::member_fun);
                                                  (yyval.el_ival).ival = (yyvsp[(1) - (4)].ival); (yyval.el_ival).el.Reset(); }
    break;

  case 272:
#line 1386 "css_parse.y"
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

  case 273:
#line 1404 "css_parse.y"
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

  case 274:
#line 1428 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].nm))); }
    break;

  case 275:
#line 1432 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].nm))); }
    break;

  case 276:
#line 1434 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName())); }
    break;

  case 277:
#line 1436 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName())); }
    break;

  case 278:
#line 1438 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 279:
#line 1442 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 280:
#line 1446 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding function:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 281:
#line 1452 "css_parse.y"
    { /* these are treated identically in parsing */
           cssMisc::cur_top->parse_path_expr = true; }
    break;

  case 282:
#line 1454 "css_parse.y"
    {
           cssMisc::cur_top->parse_path_expr = true; }
    break;

  case 283:
#line 1458 "css_parse.y"
    {
   	   String tmpstr = String((yyvsp[(1) - (1)].nm));
           (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr);
           cssMisc::cur_top->parse_path_expr = false; }
    break;

  case 284:
#line 1462 "css_parse.y"
    {
           cssMisc::cur_top->parse_path_expr = false; }
    break;

  case 291:
#line 1474 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 292:
#line 1475 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 294:
#line 1479 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 295:
#line 1482 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 296:
#line 1483 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 298:
#line 1487 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 299:
#line 1488 "css_parse.y"
    { 
	  String tmpstr = String((yyvsp[(1) - (1)].nm));
	  (yyval.ival) = Code1(cssMisc::cur_top->AddLiteral(tmpstr)); }
    break;

  case 300:
#line 1491 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 301:
#line 1494 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); }
    break;

  case 305:
#line 1502 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 306:
#line 1505 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;


/* Line 1267 of yacc.c.  */
#line 4182 "y.tab.c"
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


#line 1509 "css_parse.y"


	/* end of grammar */


void yyerror(char* s) { 	/* called for yacc syntax error */
  int i;

  ostream* fh = &cerr;
  if(cssMisc::cur_top->cmd_shell != NULL)
    fh = cssMisc::cur_top->cmd_shell->ferr;

  String src = cssMisc::cur_top->CurFullTokSrc();
  if(strcmp(s, "parse error") == 0) {
    src.gsub('\t',' ');		// replace tabs
    *fh << "Syntax Error " << src;
    for(i=0; i < cssMisc::cur_top->tok_src_col; i++)
      *fh << " ";
    *fh << "^\n";
  }
  else {
    *fh << s << " " << src;
  }
  taMisc::FlushConsole();
}

