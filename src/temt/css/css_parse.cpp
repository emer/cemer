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
     CSS_FOREACH = 289,
     CSS_IN = 290,
     CSS_NEW = 291,
     CSS_DELETE = 292,
     CSS_COMMAND = 293,
     CSS_ALIAS = 294,
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
     CSS_BITNEG = 327,
     CSS_NOT = 328,
     CSS_UNARYMINUS = 329,
     CSS_SCOPER = 330,
     CSS_POINTSAT = 331
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
#define CSS_FOREACH 289
#define CSS_IN 290
#define CSS_NEW 291
#define CSS_DELETE 292
#define CSS_COMMAND 293
#define CSS_ALIAS 294
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
#define CSS_BITNEG 327
#define CSS_NOT 328
#define CSS_UNARYMINUS 329
#define CSS_SCOPER 330
#define CSS_POINTSAT 331




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
#line 285 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 298 "y.tab.c"

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
#define YYLAST   2515

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  99
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  122
/* YYNRULES -- Number of rules.  */
#define YYNRULES  339
/* YYNRULES -- Number of states.  */
#define YYNSTATES  555

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   331

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      93,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    81,    68,     2,
      89,    95,    79,    77,    50,    78,    88,    80,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    63,    94,
       2,    51,     2,    62,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    90,     2,    96,    67,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    49,    66,    97,    98,     2,     2,     2,
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
      73,    74,    75,    76,    82,    83,    84,    85,    86,    87,
      91,    92
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
     420,   422,   425,   428,   431,   434,   437,   441,   445,   447,
     450,   453,   457,   459,   461,   464,   467,   470,   472,   474,
     476,   479,   481,   483,   485,   487,   489,   492,   494,   496,
     498,   500,   504,   509,   512,   515,   517,   519,   523,   526,
     528,   530,   532,   534,   541,   543,   550,   558,   560,   562,
     565,   567,   569,   572,   574,   576,   583,   585,   589,   591,
     593,   595,   598,   600,   603,   605,   609,   613,   615,   619,
     623,   627,   629,   631,   633,   636,   641,   645,   646,   648,
     650,   654,   656,   658,   660,   662,   664,   666,   668,   670,
     674,   678,   682,   686,   690,   694,   698,   702,   706,   710,
     714,   718,   722,   728,   733,   737,   743,   747,   751,   754,
     757,   761,   765,   769,   773,   777,   781,   787,   791,   795,
     799,   803,   807,   811,   815,   819,   823,   827,   830,   833,
     836,   839,   842,   845,   848,   850,   853,   858,   863,   865,
     869,   871,   873,   875,   877,   879,   883,   885,   887,   889,
     891,   893,   895,   897,   900,   903,   907,   911,   916,   922,
     927,   931,   933,   937,   941,   943,   946,   950,   953,   957,
     961,   964,   967,   971,   976,   981,   986,   990,   992,   994,
     996,   998,  1000,  1002,  1004,  1006,  1008,  1010,  1012,  1014,
    1016,  1018,  1020,  1022,  1024,  1026,  1030,  1032,  1034,  1036,
    1040,  1042,  1044,  1046,  1048,  1049,  1051,  1053,  1057,  1059
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     100,     0,    -1,    -1,   100,    93,    -1,   100,   103,   102,
      -1,   100,   109,   102,    -1,   100,   192,    -1,   100,   111,
      -1,   100,   156,    -1,   100,     1,   102,    -1,    94,    -1,
      94,    -1,    93,    -1,   104,    -1,   105,    -1,   106,    -1,
     107,    -1,   108,    -1,     3,    11,    -1,     4,    16,    -1,
       4,    15,    -1,     5,    15,    -1,     6,    16,    -1,     6,
      15,    -1,     7,    15,    -1,     7,    16,    -1,     8,    -1,
       9,    -1,    38,   110,    -1,    39,   196,   209,    -1,    39,
     196,   196,    -1,    40,    -1,    40,   217,   196,    -1,    40,
     217,    13,    -1,    40,   217,    39,    -1,    40,   217,    40,
      -1,    40,   217,    19,    -1,    -1,   217,   215,    -1,    89,
     217,   215,    95,    -1,   112,    -1,   113,   101,    -1,   114,
      -1,   117,    -1,   122,    -1,   140,    -1,   150,   147,   220,
     101,    -1,   150,   209,    51,   217,   193,   220,    -1,   208,
      51,   217,   193,   220,    -1,   150,   209,   217,   115,   220,
     101,    -1,   116,    -1,   115,   116,    -1,    90,   193,    96,
      -1,   118,    49,   119,    97,   220,   101,    -1,    21,   209,
      -1,    21,    18,    -1,    21,    -1,   120,    -1,   119,   120,
      -1,   121,    -1,   121,    50,    -1,     1,    -1,   209,    -1,
     209,    51,    10,    -1,    20,   123,    49,   139,   131,    97,
     220,   101,    -1,    20,   123,    49,   139,    97,   220,   101,
      -1,    20,   125,   220,   101,    -1,   124,    -1,   124,    63,
     126,    -1,   209,    -1,    18,    -1,   209,    -1,    18,    -1,
     127,    -1,   126,    50,   127,    -1,   128,    -1,   151,    -1,
     129,   151,    -1,   130,    -1,   129,   130,    -1,    45,    -1,
      47,    -1,    44,    -1,   132,    -1,   131,   132,    -1,   133,
      -1,   137,    -1,   117,   139,    -1,    45,    63,    -1,    44,
      63,    -1,    46,    63,    -1,     1,    -1,   150,   209,   101,
     139,    -1,   150,   209,   134,   135,   101,   139,    -1,    -1,
     136,    -1,   135,   136,    -1,    90,    10,    96,    -1,   138,
     143,   101,   139,    -1,    18,   143,   101,   139,    -1,    98,
      18,   143,   101,   139,    -1,    47,    98,    18,   143,   101,
     139,    -1,   150,   209,    -1,    -1,    17,    -1,   139,    17,
      -1,   141,   143,   220,   101,    -1,   141,   143,   220,   190,
      -1,   150,    13,   143,   101,    -1,   150,    13,   143,   220,
     190,    -1,   142,   143,   220,   191,    -1,   150,   209,    -1,
     150,   153,   209,    -1,   153,    18,    -1,   153,    98,    18,
      -1,    89,    95,    -1,    89,   144,    95,    -1,    89,    95,
      43,    -1,    89,   144,    95,    43,    -1,   145,    -1,   144,
      50,   145,    -1,   146,    -1,   146,    51,   195,    -1,   146,
      51,   153,    16,    -1,   150,    16,    -1,   150,   212,    -1,
     148,    -1,   147,    50,   148,    -1,   209,    -1,   209,    -1,
     151,    -1,    41,   151,    -1,    42,   151,    -1,    43,   151,
      -1,    47,   151,    -1,    48,   151,    -1,    48,    47,   151,
      -1,    47,    48,   151,    -1,   152,    -1,   152,   155,    -1,
     152,    68,    -1,   152,   155,    68,    -1,    18,    -1,    19,
      -1,   153,    22,    -1,    18,    91,    -1,    19,    91,    -1,
      18,    -1,    19,    -1,    79,    -1,   155,    79,    -1,   180,
      -1,   183,    -1,   157,    -1,   158,    -1,   190,    -1,   193,
     101,    -1,   163,    -1,   170,    -1,   178,    -1,   176,    -1,
      30,   217,   101,    -1,    30,   217,   193,   101,    -1,    31,
     101,    -1,    32,   101,    -1,   161,    -1,   159,    -1,    28,
     160,    63,    -1,    29,    63,    -1,    10,    -1,    11,    -1,
      12,    -1,    14,    -1,   162,    89,   217,   193,    95,    49,
      -1,    27,    -1,   164,    89,   165,   167,   169,   156,    -1,
     164,    89,   218,   165,   167,   169,   156,    -1,    33,    -1,
     166,    -1,   166,   193,    -1,    94,    -1,   168,    -1,   168,
     218,    -1,    94,    -1,    95,    -1,   171,    89,   172,   174,
     175,   156,    -1,    34,    -1,   150,   149,   220,    -1,    12,
      -1,    14,    -1,    35,    -1,   173,   193,    -1,    95,    -1,
     177,   156,    -1,    24,    -1,   179,   187,   101,    -1,   179,
     187,   156,    -1,    23,    -1,   185,   187,   157,    -1,   185,
     187,   158,    -1,   185,   187,   190,    -1,   180,    -1,   157,
      -1,   192,    -1,   186,   157,    -1,   186,   181,   184,   182,
      -1,   186,   181,   183,    -1,    -1,    25,    -1,    26,    -1,
     188,   193,   189,    -1,    89,    -1,    95,    -1,    49,    -1,
      49,    -1,    97,    -1,   195,    -1,   194,    -1,   206,    -1,
     193,    51,   193,    -1,   193,    51,    49,    -1,   193,    61,
     193,    -1,   193,    60,   193,    -1,   193,    59,   193,    -1,
     193,    58,   193,    -1,   193,    57,   193,    -1,   193,    56,
     193,    -1,   193,    55,   193,    -1,   193,    54,   193,    -1,
     193,    53,   193,    -1,   193,    52,   193,    -1,    36,   217,
     154,    -1,    36,   217,   154,    89,    95,    -1,    36,   217,
     154,   116,    -1,    37,   217,   193,    -1,    37,    90,    96,
     217,   193,    -1,   193,    64,   193,    -1,   193,    65,   193,
      -1,    68,   193,    -1,    79,   193,    -1,   193,    74,   193,
      -1,   193,    73,   193,    -1,   193,    72,   193,    -1,   193,
      71,   193,    -1,   193,    70,   193,    -1,   193,    69,   193,
      -1,   193,    62,   193,    63,   193,    -1,   193,    77,   193,
      -1,   193,    78,   193,    -1,   193,    79,   193,    -1,   193,
      80,   193,    -1,   193,    81,   193,    -1,   193,    76,   193,
      -1,   193,    75,   193,    -1,   193,    68,   193,    -1,   193,
      67,   193,    -1,   193,    66,   193,    -1,   193,    83,    -1,
      83,   193,    -1,   193,    82,    -1,    82,   193,    -1,    78,
     193,    -1,    86,   193,    -1,    98,   193,    -1,   197,    -1,
     193,   197,    -1,    89,   150,    95,   193,    -1,   150,    89,
     193,    95,    -1,   204,    -1,    89,   193,    95,    -1,    10,
      -1,    11,    -1,    12,    -1,    14,    -1,    38,    -1,   198,
     203,    96,    -1,    90,    -1,    63,    -1,    50,    -1,    94,
      -1,   193,    -1,    19,    -1,   199,    -1,   193,   199,    -1,
     199,   193,    -1,   199,   199,   193,    -1,   193,   199,   193,
      -1,   193,   199,   193,   199,    -1,   193,   199,   193,   199,
     193,    -1,   193,   199,   199,   193,    -1,   193,   199,   199,
      -1,   202,    -1,   203,   200,   202,    -1,   203,   201,   202,
      -1,    13,    -1,   205,    95,    -1,   205,   213,    95,    -1,
      13,    89,    -1,   194,   210,   211,    -1,   195,   210,   211,
      -1,   210,   211,    -1,   153,   211,    -1,   207,   220,    95,
      -1,   207,   220,   213,    95,    -1,   194,   210,   211,    89,
      -1,   195,   210,   211,    89,    -1,   153,   211,    89,    -1,
      16,    -1,    16,    -1,    10,    -1,    11,    -1,    12,    -1,
      14,    -1,    13,    -1,    88,    -1,    92,    -1,    16,    -1,
      11,    -1,    12,    -1,    14,    -1,    13,    -1,    19,    -1,
      10,    -1,   196,    -1,   214,    -1,   213,    50,   214,    -1,
     193,    -1,    19,    -1,   216,    -1,   215,    50,   216,    -1,
     193,    -1,    19,    -1,    16,    -1,    18,    -1,    -1,   219,
      -1,   113,    -1,   218,    50,   218,    -1,   193,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   159,   159,   160,   161,   162,   163,   166,   169,   170,
     173,   176,   177,   180,   181,   182,   183,   184,   189,   195,
     197,   198,   201,   204,   205,   208,   211,   215,   226,   228,
     232,   234,   236,   240,   244,   248,   252,   258,   259,   260,
     264,   265,   266,   267,   268,   269,   272,   282,   300,   333,
     349,   350,   354,   357,   365,   373,   381,   391,   392,   395,
     396,   397,   400,   418,   439,   443,   447,   453,   454,   457,
     464,   486,   492,   500,   501,   504,   514,   515,   518,   519,
     522,   523,   524,   527,   528,   531,   532,   533,   534,   535,
     536,   537,   540,   552,   568,   576,   577,   581,   585,   591,
     607,   624,   644,   665,   666,   667,   670,   676,   688,   699,
     720,   733,   746,   772,   793,   815,   816,   817,   818,   821,
     822,   825,   826,   829,   836,   844,   854,   855,   862,   871,
     880,   881,   882,   883,   885,   886,   887,   888,   891,   892,
     894,   896,   901,   902,   903,   914,   919,   923,   924,   927,
     928,   931,   932,   933,   936,   937,   940,   941,   942,   943,
     944,   945,   946,   947,   948,   949,   950,   953,   962,   974,
     975,   976,   977,   981,   998,  1006,  1021,  1041,  1048,  1049,
    1052,  1059,  1060,  1063,  1073,  1082,  1101,  1111,  1119,  1122,
    1127,  1134,  1148,  1161,  1164,  1173,  1196,  1210,  1213,  1218,
    1222,  1228,  1229,  1230,  1233,  1238,  1240,  1244,  1249,  1253,
    1271,  1278,  1285,  1289,  1295,  1298,  1321,  1322,  1326,  1327,
    1328,  1338,  1339,  1340,  1341,  1342,  1343,  1344,  1345,  1346,
    1347,  1348,  1349,  1350,  1352,  1353,  1354,  1355,  1356,  1357,
    1358,  1359,  1360,  1361,  1362,  1363,  1364,  1365,  1366,  1367,
    1368,  1369,  1370,  1371,  1372,  1373,  1374,  1375,  1376,  1377,
    1378,  1379,  1380,  1381,  1382,  1383,  1385,  1391,  1397,  1398,
    1402,  1403,  1404,  1405,  1408,  1411,  1420,  1424,  1427,  1430,
    1434,  1435,  1436,  1437,  1438,  1439,  1440,  1441,  1442,  1443,
    1444,  1447,  1448,  1449,  1453,  1454,  1455,  1470,  1486,  1487,
    1495,  1496,  1502,  1507,  1524,  1526,  1544,  1568,  1572,  1574,
    1576,  1578,  1582,  1586,  1592,  1594,  1598,  1602,  1606,  1607,
    1608,  1609,  1610,  1611,  1614,  1615,  1618,  1619,  1622,  1623,
    1626,  1627,  1628,  1631,  1634,  1637,  1638,  1639,  1642,  1645
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
  "CSS_RETURN", "CSS_BREAK", "CSS_CONTINUE", "CSS_FOR", "CSS_FOREACH",
  "CSS_IN", "CSS_NEW", "CSS_DELETE", "CSS_COMMAND", "CSS_ALIAS",
  "CSS_HELP", "CSS_EXTERN", "CSS_STATIC", "CSS_CONST", "CSS_PRIVATE",
  "CSS_PUBLIC", "CSS_PROTECTED", "CSS_VIRTUAL", "CSS_INLINE", "'{'", "','",
  "'='", "CSS_ASGN_OR", "CSS_ASGN_XOR", "CSS_ASGN_AND", "CSS_ASGN_RSHIFT",
  "CSS_ASGN_LSHIFT", "CSS_ASGN_MOD", "CSS_ASGN_DIV", "CSS_ASGN_MULT",
  "CSS_ASGN_SUB", "CSS_ASGN_ADD", "'?'", "':'", "CSS_OR", "CSS_AND", "'|'",
  "'^'", "'&'", "CSS_NE", "CSS_EQ", "CSS_LE", "CSS_LT", "CSS_GE", "CSS_GT",
  "CSS_RSHIFT", "CSS_LSHIFT", "'+'", "'-'", "'*'", "'/'", "'%'",
  "CSS_MINMIN", "CSS_PLUSPLUS", "CSS_UNARY", "CSS_BITNEG", "CSS_NOT",
  "CSS_UNARYMINUS", "'.'", "'('", "'['", "CSS_SCOPER", "CSS_POINTSAT",
  "'\\n'", "';'", "')'", "']'", "'}'", "'~'", "$accept", "toplev", "term",
  "crterm", "pproc", "ppinclude", "ppdefine", "ppifdef", "ppelse",
  "ppendif", "command", "cmd_args", "defn", "vardefn", "vardefin",
  "arraydefn", "arraydims", "arraydim", "enumdefn", "enumname", "enums",
  "enumline", "enumitms", "classdefn", "classhead", "classnm", "classfwd",
  "classinh", "classpar", "classtyp", "inh_mods", "inh_mod", "membs",
  "membline", "membdefn", "membardimstop", "membardims", "membardim",
  "methdefn", "methname", "classcmt", "fundefn", "fundname", "methdname",
  "funargs", "arglist", "argdefn", "subargdefn", "tynames", "tyname",
  "tynamel", "type", "type_el", "typeorscp", "scopetype", "typeonly",
  "ptrs", "stmt", "miscbrastmt", "miscstmt", "caseitem", "caseexpr",
  "switchblock", "switch", "forloop", "for", "for_cond", "for_cond_sc",
  "for_incr", "for_incr_sc", "for_end_paren", "foreachloop", "foreach",
  "foreach_var", "foreach_in", "foreach_in_expr", "foreach_end_paren",
  "doloop", "do", "whiloop", "while", "ifstmt", "elseifstmt", "noelsestmt",
  "elsestmt", "popelse", "if", "else", "cond", "cond_paren",
  "cond_end_paren", "bra", "mbr_bra", "ket", "expr", "comb_expr",
  "primitive", "anycmd", "matrixarray", "startmatrix", "matcolon",
  "matcomma", "matsemicolon", "matexprlsel", "matexprlist", "normfuncall",
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   123,
      44,    61,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,    63,    58,   314,   315,   124,    94,    38,   316,
     317,   318,   319,   320,   321,   322,   323,    43,    45,    42,
      47,    37,   324,   325,   326,   327,   328,   329,    46,    40,
      91,   330,   331,    10,    59,    41,    93,   125,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    99,   100,   100,   100,   100,   100,   100,   100,   100,
     101,   102,   102,   103,   103,   103,   103,   103,   104,   105,
     105,   105,   106,   106,   106,   106,   107,   108,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   110,   110,   110,
     111,   111,   111,   111,   111,   111,   112,   113,   113,   114,
     115,   115,   116,   117,   118,   118,   118,   119,   119,   120,
     120,   120,   121,   121,   122,   122,   122,   123,   123,   124,
     124,   125,   125,   126,   126,   127,   128,   128,   129,   129,
     130,   130,   130,   131,   131,   132,   132,   132,   132,   132,
     132,   132,   133,   133,   134,   135,   135,   136,   137,   137,
     137,   137,   138,   139,   139,   139,   140,   140,   140,   140,
     140,   141,   142,   142,   142,   143,   143,   143,   143,   144,
     144,   145,   145,   145,   146,   146,   147,   147,   148,   149,
     150,   150,   150,   150,   150,   150,   150,   150,   151,   151,
     151,   151,   152,   152,   152,   153,   153,   154,   154,   155,
     155,   156,   156,   156,   157,   157,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   159,   159,   160,
     160,   160,   160,   161,   162,   163,   163,   164,   165,   165,
     166,   167,   167,   168,   169,   170,   171,   172,   172,   172,
     173,   174,   175,   176,   177,   178,   178,   179,   180,   181,
     181,   182,   182,   182,   183,   183,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   193,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     195,   195,   195,   195,   196,   197,   198,   199,   200,   201,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   203,   203,   203,   204,   204,   204,   205,   206,   206,
     206,   206,   206,   206,   207,   207,   207,   208,   209,   209,
     209,   209,   209,   209,   210,   210,   211,   211,   212,   212,
     212,   212,   212,   212,   213,   213,   214,   214,   215,   215,
     216,   216,   216,   216,   217,   218,   218,   218,   219,   220
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
       1,     2,     2,     2,     2,     2,     3,     3,     1,     2,
       2,     3,     1,     1,     2,     2,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     3,     4,     2,     2,     1,     1,     3,     2,     1,
       1,     1,     1,     6,     1,     6,     7,     1,     1,     2,
       1,     1,     2,     1,     1,     6,     1,     3,     1,     1,
       1,     2,     1,     2,     1,     3,     3,     1,     3,     3,
       3,     1,     1,     1,     2,     4,     3,     0,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     5,     4,     3,     5,     3,     3,     2,     2,
       3,     3,     3,     3,     3,     3,     5,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
       2,     2,     2,     2,     1,     2,     4,     4,     1,     3,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     3,     4,     5,     4,
       3,     1,     3,     3,     1,     2,     3,     2,     3,     3,
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
      27,   270,   271,   272,   294,   273,   307,   142,   143,     0,
      56,   197,   194,   208,   209,   174,     0,     0,   334,     0,
       0,   177,   186,   334,   334,   334,     0,   334,     0,     0,
       0,     0,     0,   213,     0,     0,     0,     0,     0,     0,
     314,     0,   276,   315,     3,   215,     0,     0,    13,    14,
      15,    16,    17,     0,     7,    40,     0,    42,    43,     0,
      44,    45,     0,     0,     0,   130,   138,     0,     8,   153,
     154,   166,   165,     0,   157,     0,   158,     0,   160,     0,
     159,     0,   151,   152,     0,     0,   155,     6,     0,   217,
     216,   264,     0,   268,     0,   218,   339,     0,     0,    12,
      11,     9,    18,    20,    19,    21,    23,    22,    24,    25,
     297,   145,   146,   309,   310,   311,   313,   312,   308,    70,
       0,    67,   339,    69,    55,    54,   169,   170,   171,   172,
       0,   168,     0,    10,   163,   164,     0,     0,     0,   334,
      28,     0,   274,     0,     0,   131,     0,   132,   133,     0,
     134,     0,   135,     0,     0,   238,   261,   239,   260,   258,
     262,     0,     0,   263,     4,     5,    41,     0,     0,   339,
     339,   313,     0,     0,     0,   339,   126,     0,   128,   140,
     149,   139,   317,   316,   113,   144,     0,   301,   334,     0,
       0,   193,   211,     0,     0,     0,   204,   207,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   259,
     257,   156,   265,     0,     0,   143,   277,   280,   282,   291,
       0,   143,   295,   326,     0,   324,     0,   334,   300,   103,
       0,     0,   167,   161,     0,   147,   148,   231,   334,   234,
       0,   332,   333,   331,   330,    38,   328,    30,    29,    33,
      36,    34,    35,    32,   137,   136,     0,   269,    61,     0,
      57,    59,    62,   115,     0,   119,   121,     0,     0,     0,
     339,     0,     0,     0,   112,   334,     0,   141,   150,   114,
     306,     0,   180,   336,     0,     0,   178,   338,     0,   335,
     188,   189,     0,     0,   195,   196,     0,   198,   206,     0,
       0,   220,   219,   230,   229,   228,   227,   226,   225,   224,
     223,   222,   221,     0,   236,   237,   256,   255,   254,   245,
     244,   243,   242,   241,   240,   253,   252,   247,   248,   249,
     250,   251,   298,   299,   283,   284,     0,   278,   279,   275,
       0,     0,     0,   296,   302,     0,     0,   104,     0,    82,
      80,    81,    68,    73,    75,     0,    78,    76,    66,   162,
       0,     0,   233,     0,     0,     0,   266,   339,    58,    60,
       0,   117,     0,   116,     0,   322,   318,   320,   319,   124,
     321,   323,   125,   106,   107,   214,   110,   108,     0,   267,
     127,   128,    46,     0,   339,    50,     0,     0,   183,     0,
     181,   179,     0,     0,   339,   129,   190,     0,     0,   212,
     210,   202,   201,   205,   203,   199,   200,     0,   304,   305,
     286,   290,   285,   292,   293,   325,   303,   339,    91,   105,
     142,     0,     0,     0,     0,   339,     0,   103,     0,    83,
      85,    86,     0,     0,     0,    79,    77,   232,     0,   235,
      39,   329,     0,    63,   120,   118,     0,   122,   109,   339,
      51,     0,     0,   184,     0,   182,   337,     0,   187,   191,
     192,     0,   246,   287,   289,    48,     0,    89,    88,    90,
       0,     0,     0,    87,   339,    84,     0,    94,    74,    52,
      53,   123,    47,    49,   173,   175,     0,   185,   288,   103,
       0,    65,     0,     0,   103,   103,     0,   176,    99,     0,
     103,    64,    98,    92,     0,     0,    95,   103,   100,     0,
     103,    96,   101,    97,    93
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,   144,   111,    57,    58,    59,    60,    61,    62,
      63,   150,    64,    65,   313,    67,   424,   392,   467,    69,
     289,   290,   291,    70,   130,   131,   132,   382,   383,   384,
     385,   386,   468,   469,   470,   536,   545,   546,   471,   472,
     378,    71,    72,    73,   179,   294,   295,   296,   185,   186,
     434,   163,    75,    76,   164,   267,   191,    78,    79,    80,
      81,   140,    82,    83,    84,    85,   315,   316,   429,   430,
     494,    86,    87,   323,   437,   438,   501,    88,    89,    90,
      91,    92,   207,   443,    93,   329,    94,    95,   203,   204,
     440,    96,   416,    97,    98,    99,   100,   153,   101,   102,
     248,   370,   371,   249,   250,   103,   104,   105,   106,   107,
     292,   108,   197,   412,   254,   255,   275,   276,   142,   318,
     319,   256
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -312
static const yytype_int16 yypact[] =
{
    -312,   829,  -312,   153,    12,   264,    30,   277,   279,  -312,
    -312,  -312,  -312,  -312,   -58,  -312,  -312,   -43,   -31,   515,
     590,  -312,  -312,  -312,  -312,  -312,   427,    65,  -312,    -7,
      -7,  -312,  -312,  -312,    21,   195,   112,   248,   326,   326,
     326,    66,   179,  -312,  1944,  1944,  1944,  1944,  1944,  1944,
    -312,  1944,  -312,  -312,  -312,  -312,  1944,   153,  -312,  -312,
    -312,  -312,  -312,   153,  -312,  -312,    -7,  -312,  -312,   113,
    -312,  -312,    94,    94,   174,  -312,   -17,    39,  -312,  -312,
    -312,  -312,  -312,   123,  -312,   126,  -312,   147,  -312,   679,
    -312,   152,  -312,  -312,   152,  1007,  -312,  -312,  2301,   125,
     125,  -312,  1262,  -312,  1345,  -312,  -312,   114,   133,  -312,
    -312,  -312,  -312,  -312,  -312,  -312,  -312,  -312,  -312,  -312,
    -312,  -312,  -312,  -312,  -312,  -312,  -312,  -312,  -312,   107,
     127,   167,  -312,   155,  -312,  -312,  -312,  -312,  -312,  -312,
     203,  -312,  1434,  -312,  -312,  -312,   344,   163,  1944,  -312,
    -312,  1523,  -312,   194,   321,  -312,   252,  -312,  -312,   326,
    -312,   326,  -312,   201,   141,   293,   238,   293,   293,   293,
     238,   -42,  2121,   238,  -312,  -312,  -312,   338,   177,  -312,
    -312,    94,   -43,   -31,  1944,   282,  -312,   572,   -18,  -312,
    -312,    72,  -312,  -312,  -312,  -312,   319,   258,  -312,  1173,
     209,  -312,  -312,   531,  1944,  1090,  -312,   327,   152,  1606,
    1944,  1944,  1944,  1944,  1944,  1944,  1944,  1944,  1944,  1944,
    1944,  1944,  1944,  1944,  1944,  1944,  1944,  1944,  1944,  1944,
    1944,  1944,  1944,  1944,  1944,  1944,  1944,  1944,  1944,  -312,
    -312,  -312,  -312,   133,   133,    33,  -312,  2345,  1689,  -312,
      13,   -13,  -312,  2425,   -25,  -312,  1772,  -312,  -312,   339,
     135,    -7,  -312,  -312,  2301,  -312,  -312,   307,  -312,   293,
    1523,  -312,   -33,   196,  2425,   308,  -312,  -312,  -312,  -312,
    -312,  -312,  -312,  -312,  -312,  -312,  1944,  -312,  -312,    16,
    -312,   314,   329,   331,     2,  -312,   333,   317,    26,   328,
      -7,  2166,   572,    -7,  -312,  -312,   316,  -312,  -312,  -312,
    -312,  1944,  -312,  -312,   156,   320,  1944,  2425,    14,  -312,
    -312,  -312,   572,   381,  -312,  -312,  2211,  -312,  -312,   918,
    1090,  -312,  2425,  2425,  2425,  2425,  2425,  2425,  2425,  2425,
    2425,  2425,  2425,  2385,  1157,   663,   812,   900,   988,   311,
     311,   311,   311,   311,   311,   289,   289,   330,   330,   293,
     293,   293,   336,   340,  1689,  2425,  1944,  -312,  -312,  -312,
    1262,  1262,  2027,  -312,  -312,    23,  1944,  -312,    48,  -312,
    -312,  -312,   369,  -312,  -312,   135,  -312,  -312,  -312,  -312,
     332,  1944,  -312,  1944,    76,  1523,   293,  -312,  -312,  -312,
     418,  -312,   235,   389,   412,  -312,  -312,  -312,  -312,  -312,
    -312,  -312,  -312,  -312,  -312,  -312,  -312,  -312,   385,  -312,
    -312,  -312,  -312,  1944,   316,  -312,  2256,   391,  -312,   341,
    1861,  2425,  1861,   320,  -312,  -312,  -312,  1944,   348,  -312,
    -312,  -312,  -312,  -312,  -312,  -312,  -312,  1944,  -312,  -312,
    2345,  1944,  2425,  -312,  -312,  -312,  -312,  2425,  -312,  -312,
      -3,   382,   383,   386,    62,  -312,   430,   339,    58,  -312,
    -312,  -312,    94,   572,   135,  -312,  -312,  -312,  2075,   293,
    -312,  -312,    -7,  -312,  -312,  -312,   434,  -312,  -312,  2425,
    -312,    -7,   406,  -312,   679,   410,  -312,   341,  -312,  2425,
    -312,   679,   717,  1944,  2425,  -312,    -7,  -312,  -312,  -312,
     438,    -7,    94,   445,  -312,  -312,    -7,   105,  -312,  -312,
    -312,  -312,  -312,  -312,  -312,  -312,   679,  -312,  2425,   339,
      94,  -312,    -7,    -7,   339,   339,   376,  -312,   445,    -7,
     339,  -312,   445,   445,   457,   175,  -312,   339,   445,   373,
     339,  -312,   445,  -312,   445
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -312,  -312,   -30,   159,  -312,  -312,  -312,  -312,  -312,  -312,
    -312,  -312,  -312,  -312,   469,  -312,  -312,  -282,   471,  -312,
    -312,   184,  -312,  -312,  -312,  -312,  -312,  -312,     1,  -312,
    -312,    92,  -312,    15,  -312,  -312,  -312,   -61,  -312,  -312,
    -292,  -312,  -312,  -312,   -65,  -312,    83,  -312,  -312,   192,
    -312,     3,   -20,  -312,     0,  -312,  -312,   -86,   -90,   168,
    -312,  -312,  -312,  -312,  -312,  -312,   181,  -312,    67,  -312,
       4,  -312,  -312,  -312,  -312,  -312,  -312,  -312,  -312,  -312,
    -312,   176,  -312,  -312,   297,  -312,   413,  -312,   -78,  -312,
    -312,  -296,  -312,   178,    87,  -312,   108,  -139,   323,  -312,
    -241,  -312,  -312,    29,  -312,  -312,  -312,  -312,  -312,  -312,
      24,   318,   -96,  -312,   255,   142,   243,   128,   -24,  -311,
    -312,    82
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -335
static const yytype_int16 yytable[] =
{
     145,    77,   414,   201,    74,   206,   364,   366,   180,   146,
     148,   151,   258,   154,   277,   283,   205,   288,   155,   157,
     158,   160,   162,   112,   425,   372,   123,   124,   125,   126,
     127,   120,   128,   305,   446,  -142,   176,  -327,   156,   156,
     156,   156,   156,   133,   135,   115,  -142,   184,   121,   458,
     192,   189,   402,   286,   171,   193,  -142,   194,   121,   458,
     122,   195,   190,   367,   432,   459,   460,    18,   241,    20,
     373,  -111,  -334,   372,   187,    43,   460,    18,   122,    20,
      17,    18,  -327,  -281,    17,    18,   178,   143,   121,    38,
      39,    40,   461,   462,   463,   464,    42,   403,   188,    38,
      39,    40,   461,   462,   463,   464,    42,   368,   312,   369,
     159,   147,   263,   397,   159,   327,   300,   325,   456,   495,
     143,   496,   488,   451,   122,   270,   395,  -281,   141,  -281,
     330,   165,   166,   167,   168,   169,   170,   196,   172,   284,
     307,   285,   490,   173,   192,   465,   466,   362,   363,   193,
     152,   308,   192,    17,    18,   514,   466,   193,   411,   156,
     510,   156,   177,   195,   306,   257,   123,   124,   125,   126,
     127,   480,   128,   324,   311,   513,   259,   278,   156,   379,
     380,   297,   381,   178,   123,   124,   125,   181,   127,   247,
     128,   253,   182,   183,  -102,    17,    18,    17,    18,   143,
     156,   -72,   314,   322,   123,   124,   125,   126,   127,   503,
     128,   304,   198,    50,   261,   199,   174,    53,    38,    39,
      40,   320,   175,   321,    41,    42,   161,    17,    18,   264,
     260,   388,   152,   376,   389,   269,   200,   538,   274,   441,
     387,   202,   542,   543,   393,   184,   109,   110,   548,   -71,
      38,    39,    40,    17,    18,   552,    41,    42,   554,   268,
     156,   298,   299,   184,  -143,   544,   262,   303,   413,   143,
     417,   301,   293,   422,   195,  -143,    38,    39,    40,   113,
     114,   423,    41,    42,   149,  -143,   317,   122,   -37,   -37,
     184,   326,   116,   117,   118,   119,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   421,   405,    52,   406,
     407,   408,   302,   409,   279,   365,   410,   309,   427,   288,
     280,   -31,   -31,   253,    17,    18,   435,   310,   123,   124,
     125,   126,   127,    24,   128,   152,   377,   274,   395,   152,
     281,   282,   265,   266,   399,   476,   234,   235,   236,   237,
     238,   239,   240,   396,   401,   239,   240,   415,   156,    52,
     400,   473,   418,    52,   404,   156,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   506,   390,   391,   426,   453,
     454,    52,   156,   431,   486,   297,   391,   516,   525,   236,
     237,   238,   239,   240,   428,   527,   436,   243,   244,   474,
      52,   242,    11,    12,    13,   448,    15,   477,   483,   449,
     182,   183,   485,   314,    43,   314,   493,   136,   137,   138,
     537,   139,   305,   500,   160,   507,   508,   532,   512,   509,
     521,   450,   520,   452,   387,   524,   530,   247,   247,   253,
     432,   523,   459,   457,   156,   539,   544,   549,   156,   553,
      66,   473,    68,   398,   156,   518,   529,   475,   478,   482,
     479,   531,   274,   515,   551,   484,   534,   535,   242,   242,
     242,   242,   242,   242,   420,   242,   242,   517,   445,   433,
     497,   526,   540,   541,   328,   442,   491,   444,   208,   547,
     489,   375,   487,   394,   455,   550,   498,   317,     0,   317,
       0,     0,     0,   481,   499,   123,   124,   125,   126,   127,
       0,   128,     0,   129,   502,     0,     0,     0,   504,   505,
       0,    11,    12,    13,    14,    15,     0,   511,     0,    17,
      18,     0,     0,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,     0,    33,    34,     0,
     242,   522,    38,    39,    40,     0,   242,     0,    41,    42,
      43,     0,   123,   124,   125,   126,   127,   242,   128,     0,
     528,     0,   242,     0,     0,     0,   533,   242,     0,    44,
     123,   124,   125,   126,   127,     0,   128,     0,   134,    45,
      46,     0,     0,    47,    48,     0,     0,    49,     0,    50,
      51,    52,     0,    53,   242,   143,     0,     0,     0,    56,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     242,     0,     0,     0,     0,     0,     0,     0,     0,   242,
       0,     0,     0,     0,     0,   242,   242,   242,   242,   242,
     242,   242,   242,   242,   242,   242,   242,   242,   242,   242,
     242,   242,   242,   242,   242,   242,   242,   242,   242,   242,
     242,   242,   242,   242,   242,     0,     0,     0,   242,    11,
      12,    13,    14,    15,     0,     0,     0,    17,    18,     0,
       0,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,     0,    33,    34,     0,     0,   242,
      38,    39,    40,     0,     0,     0,    41,    42,    43,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,    44,     0,   242,
       0,     0,     0,    52,   242,     0,     0,    45,    46,     0,
       0,    47,    48,     0,     0,    49,     0,    50,    51,    52,
       0,    53,     0,   242,     0,   242,     0,    56,     0,   220,
     242,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   242,   242,     0,     0,     0,     0,    52,     0,     0,
       0,     0,   242,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   242,     0,     0,   242,     0,   242,     0,     2,
       3,     0,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,     0,    16,     0,    17,    18,    19,
      20,   242,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,     0,    33,    34,    35,    36,    37,
      38,    39,    40,     0,     0,     0,    41,    42,    43,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,     0,    44,     0,     0,
       0,     0,    52,     0,     0,     0,     0,    45,    46,     0,
       0,    47,    48,     0,     0,    49,     0,    50,    51,    52,
       0,    53,    54,     0,     0,     0,    55,    56,    11,    12,
      13,    14,    15,     0,     0,     0,    17,    18,     0,     0,
       0,    21,    22,    23,     0,    25,    26,    27,    28,    29,
      30,    31,    32,     0,    33,    34,     0,     0,     0,    38,
      39,    40,     0,     0,     0,    41,    42,    43,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,     0,     0,    44,     0,     0,     0,
      52,     0,     0,     0,     0,     0,    45,    46,     0,     0,
      47,    48,     0,     0,    49,     0,    50,    51,    52,     0,
      53,     0,     0,     0,     0,    55,    56,    11,    12,    13,
      14,    15,     0,     0,     0,    17,    18,     0,     0,     0,
      21,    22,    23,     0,    25,    26,    27,    28,    29,    30,
      31,    32,     0,    33,    34,     0,     0,     0,    38,    39,
      40,     0,     0,     0,    41,    42,    43,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,     0,     0,     0,    44,     0,     0,    52,     0,
       0,     0,     0,     0,     0,    45,    46,     0,     0,    47,
      48,     0,     0,    49,     0,    50,    51,    52,     0,    53,
      11,    12,    13,    14,    15,    56,     0,     0,    17,    18,
       0,     0,     0,    21,    22,     0,     0,    25,    26,    27,
      28,    29,    30,    31,    32,     0,    33,    34,     0,     0,
       0,    38,    39,    40,     0,     0,     0,    41,    42,    43,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    44,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    45,    46,
       0,     0,    47,    48,     0,     0,    49,     0,    50,    51,
      52,     0,    53,    11,    12,    13,    14,    15,    56,    16,
       0,    17,    18,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    33,
      34,     0,     0,     0,    38,    39,    40,     0,     0,     0,
      41,    42,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,    44,     0,     0,     0,     0,     0,    52,     0,     0,
       0,    45,    46,     0,     0,    47,    48,     0,     0,    49,
       0,    50,    51,    52,     0,    53,     0,   312,     0,     0,
       0,    56,    11,    12,    13,    14,    15,     0,     0,     0,
      17,   245,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    34,
       0,     0,     0,    38,    39,    40,     0,     0,     0,    41,
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   246,     0,     0,     0,     0,
      44,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,     0,     0,    47,    48,     0,     0,    49,     0,
      50,    51,    52,     0,    53,    11,    12,    13,    14,    15,
      56,     0,     0,    17,   251,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,     0,     0,     0,    38,    39,    40,     0,
       0,     0,    41,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    44,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    45,    46,     0,     0,    47,    48,     0,
       0,    49,     0,    50,    51,    52,     0,    53,     0,     0,
     252,     0,     0,    56,    11,    12,    13,    14,    15,     0,
       0,     0,    17,    18,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,     0,     0,     0,    38,    39,    40,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    44,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    45,    46,     0,     0,    47,    48,     0,     0,
      49,     0,    50,    51,    52,     0,    53,     0,   143,     0,
       0,     0,    56,    11,    12,    13,    14,    15,     0,   271,
       0,   272,   273,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    33,
      34,     0,     0,     0,    38,    39,    40,     0,     0,     0,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    44,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    45,    46,     0,     0,    47,    48,     0,     0,    49,
       0,    50,    51,    52,     0,    53,    11,    12,    13,    14,
      15,    56,     0,     0,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,     0,     0,     0,    38,    39,    40,
       0,     0,     0,    41,    42,   331,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    44,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    45,    46,     0,     0,    47,    48,
       0,     0,    49,     0,    50,    51,    52,     0,    53,    11,
      12,    13,    14,    15,    56,     0,     0,    17,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,    34,     0,     0,     0,
      38,    39,    40,     0,     0,     0,    41,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   246,     0,     0,     0,     0,    44,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    45,    46,     0,
       0,    47,    48,     0,     0,    49,     0,    50,    51,    52,
       0,    53,    11,    12,    13,    14,    15,    56,     0,     0,
      17,   251,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    34,
       0,     0,     0,    38,    39,    40,     0,     0,     0,    41,
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      44,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,     0,     0,    47,    48,     0,     0,    49,     0,
      50,    51,    52,     0,    53,     0,     0,   374,     0,     0,
      56,    11,    12,    13,    14,    15,     0,    16,     0,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    33,    34,     0,
       0,     0,    38,    39,    40,     0,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    44,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    45,
      46,     0,     0,    47,    48,     0,     0,    49,     0,    50,
      51,    52,     0,    53,    11,    12,    13,    14,    15,    56,
       0,     0,    17,    18,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,     0,     0,     0,    38,    39,    40,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    44,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    45,    46,     0,     0,    47,    48,     0,     0,
      49,     0,    50,    51,    52,     0,    53,    11,    12,    13,
      14,    15,    56,     0,     0,    17,   251,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    33,    34,     0,     0,     0,    38,    39,
      40,     0,     0,     0,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    44,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    45,    46,     0,     0,    47,
      48,     0,     0,    49,     0,    50,    51,    52,     0,    53,
       0,     0,     0,     0,     0,    56,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,     0,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,     0,
       0,     0,     0,     0,     0,    52,     0,     0,     0,     0,
       0,   519,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,     0,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,     0,     0,     0,     0,     0,
       0,    52,     0,     0,     0,     0,   287,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,     0,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
       0,     0,     0,     0,     0,     0,    52,     0,     0,     0,
       0,   419,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,     0,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,     0,     0,     0,     0,     0,
       0,    52,     0,     0,     0,     0,   439,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,     0,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
       0,     0,     0,     0,     0,     0,    52,     0,     0,     0,
       0,   492,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,     0,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,     0,     0,     0,     0,     0,
       0,    52,     0,     0,     0,   143,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   246,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,     0,
       0,     0,     0,     0,     0,    52,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   447,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,     0,
       0,     0,     0,     0,     0,    52,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,     0,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,     0,
       0,     0,     0,     0,     0,    52
};

static const yytype_int16 yycheck[] =
{
      30,     1,   298,    89,     1,    95,   247,   248,    73,    33,
      34,    35,   108,    37,   153,   154,    94,     1,    38,    39,
      40,    41,    42,    11,   306,    50,    10,    11,    12,    13,
      14,    89,    16,    51,   330,    68,    66,    50,    38,    39,
      40,    41,    42,    19,    20,    15,    79,    89,    91,     1,
      11,    68,    50,    95,    51,    16,    89,    18,    91,     1,
      91,    22,    79,    50,    50,    17,    18,    19,    98,    21,
      95,    89,    90,    50,    74,    49,    18,    19,    91,    21,
      18,    19,    95,    50,    18,    19,    89,    94,    91,    41,
      42,    43,    44,    45,    46,    47,    48,    95,    74,    41,
      42,    43,    44,    45,    46,    47,    48,    94,    94,    96,
      48,    90,   142,    97,    48,   205,   181,   203,    95,   430,
      94,   432,   418,   364,    91,   149,    50,    94,    63,    96,
     208,    44,    45,    46,    47,    48,    49,    98,    51,   159,
      68,   161,   424,    56,    11,    97,    98,   243,   244,    16,
      38,    79,    11,    18,    19,    97,    98,    16,   297,   159,
      98,   161,    49,    22,   188,    51,    10,    11,    12,    13,
      14,    95,    16,   203,   198,   467,    49,   153,   178,    44,
      45,   178,    47,    89,    10,    11,    12,    13,    14,   102,
      16,   104,    18,    19,    89,    18,    19,    18,    19,    94,
     200,    94,   199,   200,    10,    11,    12,    13,    14,   450,
      16,   187,    89,    88,   132,    89,    57,    92,    41,    42,
      43,    12,    63,    14,    47,    48,    47,    18,    19,   142,
      63,   261,    38,   257,   264,   148,    89,   529,   151,   329,
     260,    89,   534,   535,   268,    89,    93,    94,   540,    94,
      41,    42,    43,    18,    19,   547,    47,    48,   550,    96,
     260,   179,   180,    89,    68,    90,    63,   185,   298,    94,
     300,   184,    95,   303,    22,    79,    41,    42,    43,    15,
      16,   305,    47,    48,    89,    89,   199,    91,    93,    94,
      89,   204,    15,    16,    15,    16,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   302,    10,    90,    12,
      13,    14,    50,    16,    13,   248,    19,    18,   314,     1,
      19,    93,    94,   256,    18,    19,   322,    89,    10,    11,
      12,    13,    14,    26,    16,    38,    17,   270,    50,    38,
      39,    40,    18,    19,    50,   385,    77,    78,    79,    80,
      81,    82,    83,   286,    43,    82,    83,    49,   378,    90,
      51,   378,   300,    90,    51,   385,    75,    76,    77,    78,
      79,    80,    81,    82,    83,   460,    89,    90,   311,   370,
     371,    90,   402,   316,   404,   402,    90,   472,   494,    79,
      80,    81,    82,    83,    94,   501,    35,    99,   100,    50,
      90,    98,    10,    11,    12,    89,    14,    95,    10,    89,
      18,    19,    43,   430,    49,   432,    95,    10,    11,    12,
     526,    14,    51,    95,   464,    63,    63,   512,    18,    63,
      16,   364,   482,   366,   474,    49,    18,   370,   371,   372,
      50,   491,    17,   376,   464,   530,    90,    10,   468,    96,
       1,   468,     1,   289,   474,   474,   506,   385,   391,   397,
     393,   511,   395,   468,   545,   402,   516,   517,   165,   166,
     167,   168,   169,   170,   302,   172,   173,   473,   330,   318,
     433,   497,   532,   533,   207,   329,   424,   329,    95,   539,
     423,   256,   404,   270,   372,   545,   434,   430,    -1,   432,
      -1,    -1,    -1,   395,   437,    10,    11,    12,    13,    14,
      -1,    16,    -1,    18,   447,    -1,    -1,    -1,   451,   457,
      -1,    10,    11,    12,    13,    14,    -1,   465,    -1,    18,
      19,    -1,    -1,    -1,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    -1,    36,    37,    -1,
     247,   489,    41,    42,    43,    -1,   253,    -1,    47,    48,
      49,    -1,    10,    11,    12,    13,    14,   264,    16,    -1,
     503,    -1,   269,    -1,    -1,    -1,   514,   274,    -1,    68,
      10,    11,    12,    13,    14,    -1,    16,    -1,    18,    78,
      79,    -1,    -1,    82,    83,    -1,    -1,    86,    -1,    88,
      89,    90,    -1,    92,   301,    94,    -1,    -1,    -1,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     317,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   326,
      -1,    -1,    -1,    -1,    -1,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,   342,   343,   344,   345,   346,
     347,   348,   349,   350,   351,   352,   353,   354,   355,   356,
     357,   358,   359,   360,   361,    -1,    -1,    -1,   365,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    18,    19,    -1,
      -1,    -1,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    36,    37,    -1,    -1,   396,
      41,    42,    43,    -1,    -1,    -1,    47,    48,    49,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    68,    -1,   426,
      -1,    -1,    -1,    90,   431,    -1,    -1,    78,    79,    -1,
      -1,    82,    83,    -1,    -1,    86,    -1,    88,    89,    90,
      -1,    92,    -1,   450,    -1,   452,    -1,    98,    -1,    62,
     457,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,   478,   479,    -1,    -1,    -1,    -1,    90,    -1,    -1,
      -1,    -1,   489,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   499,    -1,    -1,   502,    -1,   504,    -1,     0,
       1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    16,    -1,    18,    19,    20,
      21,   528,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    47,    48,    49,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    -1,    68,    -1,    -1,
      -1,    -1,    90,    -1,    -1,    -1,    -1,    78,    79,    -1,
      -1,    82,    83,    -1,    -1,    86,    -1,    88,    89,    90,
      -1,    92,    93,    -1,    -1,    -1,    97,    98,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    18,    19,    -1,    -1,
      -1,    23,    24,    25,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    -1,    -1,    -1,    41,
      42,    43,    -1,    -1,    -1,    47,    48,    49,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    -1,    -1,    68,    -1,    -1,    -1,
      90,    -1,    -1,    -1,    -1,    -1,    78,    79,    -1,    -1,
      82,    83,    -1,    -1,    86,    -1,    88,    89,    90,    -1,
      92,    -1,    -1,    -1,    -1,    97,    98,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      23,    24,    25,    -1,    27,    28,    29,    30,    31,    32,
      33,    34,    -1,    36,    37,    -1,    -1,    -1,    41,    42,
      43,    -1,    -1,    -1,    47,    48,    49,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    -1,    -1,    -1,    68,    -1,    -1,    90,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    79,    -1,    -1,    82,
      83,    -1,    -1,    86,    -1,    88,    89,    90,    -1,    92,
      10,    11,    12,    13,    14,    98,    -1,    -1,    18,    19,
      -1,    -1,    -1,    23,    24,    -1,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    36,    37,    -1,    -1,
      -1,    41,    42,    43,    -1,    -1,    -1,    47,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    82,    83,    -1,    -1,    86,    -1,    88,    89,
      90,    -1,    92,    10,    11,    12,    13,    14,    98,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,    -1,
      47,    48,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    68,    -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,
      -1,    78,    79,    -1,    -1,    82,    83,    -1,    -1,    86,
      -1,    88,    89,    90,    -1,    92,    -1,    94,    -1,    -1,
      -1,    98,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      -1,    -1,    -1,    41,    42,    43,    -1,    -1,    -1,    47,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      78,    79,    -1,    -1,    82,    83,    -1,    -1,    86,    -1,
      88,    89,    90,    -1,    92,    10,    11,    12,    13,    14,
      98,    -1,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    -1,    -1,    82,    83,    -1,
      -1,    86,    -1,    88,    89,    90,    -1,    92,    -1,    -1,
      95,    -1,    -1,    98,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    37,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    78,    79,    -1,    -1,    82,    83,    -1,    -1,
      86,    -1,    88,    89,    90,    -1,    92,    -1,    94,    -1,
      -1,    -1,    98,    10,    11,    12,    13,    14,    -1,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,    -1,
      47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    78,    79,    -1,    -1,    82,    83,    -1,    -1,    86,
      -1,    88,    89,    90,    -1,    92,    10,    11,    12,    13,
      14,    98,    -1,    -1,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,
      -1,    -1,    -1,    47,    48,    49,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    68,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    78,    79,    -1,    -1,    82,    83,
      -1,    -1,    86,    -1,    88,    89,    90,    -1,    92,    10,
      11,    12,    13,    14,    98,    -1,    -1,    18,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    37,    -1,    -1,    -1,
      41,    42,    43,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    63,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    -1,
      -1,    82,    83,    -1,    -1,    86,    -1,    88,    89,    90,
      -1,    92,    10,    11,    12,    13,    14,    98,    -1,    -1,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      -1,    -1,    -1,    41,    42,    43,    -1,    -1,    -1,    47,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      78,    79,    -1,    -1,    82,    83,    -1,    -1,    86,    -1,
      88,    89,    90,    -1,    92,    -1,    -1,    95,    -1,    -1,
      98,    10,    11,    12,    13,    14,    -1,    16,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    -1,
      -1,    -1,    41,    42,    43,    -1,    -1,    -1,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,
      79,    -1,    -1,    82,    83,    -1,    -1,    86,    -1,    88,
      89,    90,    -1,    92,    10,    11,    12,    13,    14,    98,
      -1,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    37,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    78,    79,    -1,    -1,    82,    83,    -1,    -1,
      86,    -1,    88,    89,    90,    -1,    92,    10,    11,    12,
      13,    14,    98,    -1,    -1,    18,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    36,    37,    -1,    -1,    -1,    41,    42,
      43,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    79,    -1,    -1,    82,
      83,    -1,    -1,    86,    -1,    88,    89,    90,    -1,    92,
      -1,    -1,    -1,    -1,    -1,    98,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,    -1,
      -1,    96,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    -1,    -1,    -1,    -1,    95,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    -1,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,
      -1,    95,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    -1,    -1,    -1,    -1,    95,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    -1,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,
      -1,    95,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    -1,    -1,    -1,    94,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    -1,    90
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   100,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    16,    18,    19,    20,
      21,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    36,    37,    38,    39,    40,    41,    42,
      43,    47,    48,    49,    68,    78,    79,    82,    83,    86,
      88,    89,    90,    92,    93,    97,    98,   103,   104,   105,
     106,   107,   108,   109,   111,   112,   113,   114,   117,   118,
     122,   140,   141,   142,   150,   151,   152,   153,   156,   157,
     158,   159,   161,   162,   163,   164,   170,   171,   176,   177,
     178,   179,   180,   183,   185,   186,   190,   192,   193,   194,
     195,   197,   198,   204,   205,   206,   207,   208,   210,    93,
      94,   102,    11,    15,    16,    15,    15,    16,    15,    16,
      89,    91,    91,    10,    11,    12,    13,    14,    16,    18,
     123,   124,   125,   209,    18,   209,    10,    11,    12,    14,
     160,    63,   217,    94,   101,   101,   217,    90,   217,    89,
     110,   217,    38,   196,   217,   151,   153,   151,   151,    48,
     151,    47,   151,   150,   153,   193,   193,   193,   193,   193,
     193,   150,   193,   193,   102,   102,   101,    49,    89,   143,
     143,    13,    18,    19,    89,   147,   148,   153,   209,    68,
      79,   155,    11,    16,    18,    22,    98,   211,    89,    89,
      89,   156,    89,   187,   188,   187,   157,   181,   185,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,   101,   197,   210,   210,    19,    63,   193,   199,   202,
     203,    19,    95,   193,   213,   214,   220,    51,   211,    49,
      63,   220,    63,   101,   193,    18,    19,   154,    96,   193,
     217,    16,    18,    19,   193,   215,   216,   196,   209,    13,
      19,    39,    40,   196,   151,   151,    95,    95,     1,   119,
     120,   121,   209,    95,   144,   145,   146,   150,   220,   220,
     143,   193,    50,   220,   209,    51,   217,    68,    79,    18,
      89,   217,    94,   113,   150,   165,   166,   193,   218,   219,
      12,    14,   150,   172,   101,   156,   193,   157,   183,   184,
     187,    49,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   211,   211,   199,   193,   199,    50,    94,    96,
     200,   201,    50,    95,    95,   213,   217,    17,   139,    44,
      45,    47,   126,   127,   128,   129,   130,   151,   101,   101,
      89,    90,   116,   217,   215,    50,   193,    97,   120,    50,
      51,    43,    50,    95,    51,    10,    12,    13,    14,    16,
      19,   196,   212,   101,   190,    49,   191,   101,   220,    95,
     148,   209,   101,   217,   115,   116,   193,   209,    94,   167,
     168,   193,    50,   165,   149,   209,    35,   173,   174,    95,
     189,   157,   180,   182,   192,   158,   190,    63,    89,    89,
     193,   199,   193,   202,   202,   214,    95,   193,     1,    17,
      18,    44,    45,    46,    47,    97,    98,   117,   131,   132,
     133,   137,   138,   150,    50,   130,   151,    95,   193,   193,
      95,   216,   220,    10,   145,    43,   153,   195,   190,   193,
     116,   220,    95,    95,   169,   218,   218,   167,   220,   193,
      95,   175,   193,   199,   193,   220,   143,    63,    63,    63,
      98,   220,    18,   139,    97,   132,   143,   209,   127,    96,
     101,    16,   220,   101,    49,   156,   169,   156,   193,   101,
      18,   101,   143,   220,   101,   101,   134,   156,   139,   143,
     101,   101,   139,   139,    90,   135,   136,   101,   139,    10,
     101,   136,   139,    96,   139
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
#line 160 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Blank; }
    break;

  case 4:
#line 161 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 5:
#line 162 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return (yyvsp[(2) - (3)].ival); }
    break;

  case 6:
#line 163 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 7:
#line 166 "css_parse.y"
    { cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
    break;

  case 8:
#line 169 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
    break;

  case 9:
#line 170 "css_parse.y"
    { cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Err; }
    break;

  case 10:
#line 173 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 11:
#line 176 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 12:
#line 177 "css_parse.y"
    { (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 18:
#line 189 "css_parse.y"
    {
            cssEl* tmp = (yyvsp[(2) - (2)].el).El();
	    cssMisc::cur_top->SetInclude((const char*)*tmp);
	    cssEl::Done((yyvsp[(2) - (2)].el).El()); }
    break;

  case 19:
#line 195 "css_parse.y"
    {
            cssMisc::Defines.Push(new cssDef(0, (yyvsp[(2) - (2)].nm))); }
    break;

  case 21:
#line 198 "css_parse.y"
    { cssMisc::Defines.Remove((yyvsp[(2) - (2)].el).El()); }
    break;

  case 22:
#line 201 "css_parse.y"
    { /* if its a name, its not defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 24:
#line 205 "css_parse.y"
    { /* if its a def, its defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; }
    break;

  case 26:
#line 211 "css_parse.y"
    { /* if you see it, it applies to u*/
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog()); }
    break;

  case 28:
#line 226 "css_parse.y"
    {
            Code1((yyvsp[(1) - (2)].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 29:
#line 228 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssRef((yyvsp[(2) - (3)].el)));
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 30:
#line 232 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc; }
    break;

  case 31:
#line 234 "css_parse.y"
    {
	    Code1((yyvsp[(1) - (1)].el)); (yyval.ival) = cssProg::YY_Ok; }
    break;

  case 32:
#line 236 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 33:
#line 240 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 34:
#line 244 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 35:
#line 248 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 36:
#line 252 "css_parse.y"
    {
	    (yyval.ival) = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push((yyvsp[(3) - (3)].el).El());
	    ((yyvsp[(1) - (3)].el).El())->Do(cssMisc::cur_top->Prog()); }
    break;

  case 37:
#line 258 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 39:
#line 260 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 46:
#line 272 "css_parse.y"
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
#line 282 "css_parse.y"
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
#line 300 "css_parse.y"
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
	    else if(extyp->GetType() == cssEl::T_MbrCFun) {
	      if(((cssMbrCFun*)extyp)->methdef) {
		/* todo: could have a much more comprehensive system here */
		MethodDef* md = ((cssMbrCFun*)extyp)->methdef;
		if(md->type->GetClassType()->InheritsFrom(&TA_taMatrix)) {
		  extyp = cssBI::matrix_inst;
		}
	      }
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
#line 333 "css_parse.y"
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
#line 349 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 51:
#line 350 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 52:
#line 354 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 53:
#line 357 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(5) - (6)].ival)-1]->SetJump((yyvsp[(5) - (6)].ival)); (yyval.ival) = (yyvsp[(5) - (6)].ival)-1;
	    if(cssMisc::cur_enum->name.before(5) == "enum_") /* don't keep anon enums around */
	      delete cssMisc::cur_enum;
	    cssMisc::parsing_args = false; /* needed for - numbers */
	    cssMisc::cur_enum = NULL; }
    break;

  case 54:
#line 365 "css_parse.y"
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
#line 373 "css_parse.y"
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
#line 381 "css_parse.y"
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
#line 397 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 62:
#line 400 "css_parse.y"
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
#line 418 "css_parse.y"
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
#line 439 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(7) - (8)].ival)-1]->SetJump((yyvsp[(7) - (8)].ival)); (yyval.ival) = (yyvsp[(7) - (8)].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[(4) - (8)].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 65:
#line 443 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(6) - (7)].ival)-1]->SetJump((yyvsp[(6) - (7)].ival)); (yyval.ival) = (yyvsp[(6) - (7)].ival)-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, (yyvsp[(4) - (7)].el));
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 66:
#line 447 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(3) - (4)].ival)); (yyval.ival) = (yyvsp[(3) - (4)].ival)-1;
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
    break;

  case 69:
#line 457 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssMisc::cur_class = new cssClassType((const char*)*nm);
	    cssMisc::cur_class->last_top = cssMisc::cur_top;
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
	    cssEl::Done(nm); }
    break;

  case 70:
#line 464 "css_parse.y"
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
#line 486 "css_parse.y"
    {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_class = new cssClassType((const char*)*nm);
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
            cssEl::Done(nm); }
    break;

  case 72:
#line 492 "css_parse.y"
    {
            if((yyvsp[(1) - (1)].el).El()->GetType() != cssEl::T_ClassType) {
              yyerror("Attempt to redefine non-class type as a class");
              return cssProg::YY_Err;
            }
            cssMisc::cur_class = (cssClassType*)(yyvsp[(1) - (1)].el).El(); }
    break;

  case 75:
#line 504 "css_parse.y"
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
#line 515 "css_parse.y"
    { (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 86:
#line 532 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 87:
#line 533 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 91:
#line 537 "css_parse.y"
    { (yyval.el) = cssMisc::VoidElPtr; }
    break;

  case 92:
#line 540 "css_parse.y"
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
#line 552 "css_parse.y"
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
#line 568 "css_parse.y"
    {
            /* swap places with void marker */
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_top->Prog()->Stack()->Push(&cssMisc::Void);
            cssMisc::cur_top->Prog()->Stack()->Push(nm); }
    break;

  case 95:
#line 576 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 96:
#line 577 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 97:
#line 581 "css_parse.y"
    {
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((int)*(yyvsp[(2) - (3)].el).El())); }
    break;

  case 98:
#line 585 "css_parse.y"
    {       /* pre-declare function */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)(yyvsp[(1) - (4)].el).El();
            cssMisc::cur_class->GetComments(fun, (yyvsp[(4) - (4)].el));
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr, true); }
    break;

  case 99:
#line 591 "css_parse.y"
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
#line 607 "css_parse.y"
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
#line 624 "css_parse.y"
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
#line 644 "css_parse.y"
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
#line 665 "css_parse.y"
    { (yyval.el).Reset(); }
    break;

  case 104:
#line 666 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (1)].el); }
    break;

  case 105:
#line 667 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (2)].el); }
    break;

  case 106:
#line 670 "css_parse.y"
    {	/* pre-declare function */
            cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (4)].ival)-1]->SetJump((yyvsp[(3) - (4)].ival));	(yyval.ival) = (yyvsp[(3) - (4)].ival)-1;
	    cssScriptFun* fun = (cssScriptFun*)(yyvsp[(1) - (4)].el).El();
	    fun->argc = (yyvsp[(2) - (4)].ival); fun->GetArgDefs(); }
    break;

  case 107:
#line 676 "css_parse.y"
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
#line 688 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if((yyvsp[(2) - (4)].el).El()->GetType() != cssEl::T_ScriptFun) {
	      yyerror(String("attempt to declare a css script function using the same name as an internal function -- not allowed: ") + (yyvsp[(2) - (4)].el).El()->name);
	      return cssProg::YY_Err; }
	    (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;

  case 109:
#line 699 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (5)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if((yyvsp[(2) - (5)].el).El()->GetType() != cssEl::T_ScriptFun) {
	      yyerror(String("attempt to declare a css script function using the same name as an internal function -- not allowed: ") + (yyvsp[(2) - (5)].el).El()->name);
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
#line 720 "css_parse.y"
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
#line 733 "css_parse.y"
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
#line 746 "css_parse.y"
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
#line 772 "css_parse.y"
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
#line 793 "css_parse.y"
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
#line 815 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 116:
#line 816 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 117:
#line 817 "css_parse.y"
    { (yyval.ival) = 0; }
    break;

  case 118:
#line 818 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 119:
#line 821 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 120:
#line 822 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 121:
#line 825 "css_parse.y"
    { cssMisc::parsing_args = false; }
    break;

  case 122:
#line 826 "css_parse.y"
    {
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push((yyvsp[(3) - (3)].el).El()); }
    break;

  case 123:
#line 829 "css_parse.y"
    {
            cssEl* itm = (yyvsp[(3) - (4)].el).El()->GetScoped((yyvsp[(4) - (4)].nm));
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push(itm); cssMisc::cur_scope = NULL; }
    break;

  case 124:
#line 836 "css_parse.y"
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
#line 844 "css_parse.y"
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
#line 855 "css_parse.y"
    {
	    if(((yyvsp[(1) - (3)].ival) < 0) && ((yyvsp[(3) - (3)].ival) < 0)) (yyval.ival) = -1;
	    else if(((yyvsp[(1) - (3)].ival) >= 0) && ((yyvsp[(3) - (3)].ival) < 0)) (yyval.ival) = (yyvsp[(1) - (3)].ival);
	    else if(((yyvsp[(1) - (3)].ival) < 0) && ((yyvsp[(3) - (3)].ival) >= 0)) (yyval.ival) = (yyvsp[(3) - (3)].ival);
	    else (yyval.ival) = -1; }
    break;

  case 128:
#line 862 "css_parse.y"
    {
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      (yyval.ival) = Code2(tmp->ptr, cssBI::constr); }
	    else (yyval.ival) = -1;
	    cssEl::Done(tmp); }
    break;

  case 129:
#line 871 "css_parse.y"
    {
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    (yyval.el) = tmp->ptr;
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      Code2(tmp->ptr, cssBI::constr); }
	    cssEl::Done(tmp); }
    break;

  case 130:
#line 880 "css_parse.y"
    { (yyvsp[(1) - (1)].el).El()->tmp_str = ""; }
    break;

  case 131:
#line 881 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "extern"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 132:
#line 882 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "static"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 133:
#line 883 "css_parse.y"
    { /* use const expr prog to code stuff */
	  (yyvsp[(2) - (2)].el).El()->tmp_str = "const"; (yyval.el) = (yyvsp[(2) - (2)].el); cssMisc::CodeConstExpr(); }
    break;

  case 134:
#line 885 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 135:
#line 886 "css_parse.y"
    { (yyvsp[(2) - (2)].el).El()->tmp_str = ""; (yyval.el) = (yyvsp[(2) - (2)].el); }
    break;

  case 136:
#line 887 "css_parse.y"
    { (yyvsp[(3) - (3)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(3) - (3)].el); }
    break;

  case 137:
#line 888 "css_parse.y"
    { (yyvsp[(3) - (3)].el).El()->tmp_str = "virtual"; (yyval.el) = (yyvsp[(3) - (3)].el); }
    break;

  case 138:
#line 891 "css_parse.y"
    { (yyval.el) = (yyvsp[(1) - (1)].el); cssMisc::cur_type = (yyval.el); }
    break;

  case 139:
#line 892 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->GetPtrType((yyvsp[(1) - (2)].el).El(), (yyvsp[(2) - (2)].ival)); cssMisc::cur_type = (yyval.el); }
    break;

  case 140:
#line 894 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->GetRefType((yyvsp[(1) - (2)].el).El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 141:
#line 896 "css_parse.y"
    {
	    cssElPtr pt = cssMisc::cur_top->GetPtrType((yyvsp[(1) - (3)].el).El(), (yyvsp[(2) - (3)].ival));
            (yyval.el) = cssMisc::cur_top->GetRefType(pt.El()); cssMisc::cur_type = (yyval.el); }
    break;

  case 144:
#line 903 "css_parse.y"
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

  case 145:
#line 914 "css_parse.y"
    {
          if(((yyvsp[(1) - (2)].el).El()->GetType() != cssEl::T_ClassType) && ((yyvsp[(1) - (2)].el).El()->GetType() != cssEl::T_EnumType)) {
	     yyerror("scoping of non-class or enum type");
	     return cssProg::YY_Err; }
           cssMisc::cur_scope = (yyvsp[(1) - (2)].el).El(); }
    break;

  case 146:
#line 919 "css_parse.y"
    {
           cssMisc::cur_scope = (yyvsp[(1) - (2)].el).El(); }
    break;

  case 149:
#line 927 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 150:
#line 928 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 153:
#line 933 "css_parse.y"
    { cssMisc::cur_top->Prog()->lastif = -1; }
    break;

  case 156:
#line 940 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 161:
#line 945 "css_parse.y"
    { Code1((yyvsp[(1) - (3)].el)); (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 162:
#line 946 "css_parse.y"
    { Code1((yyvsp[(1) - (4)].el)); (yyval.ival) = (yyvsp[(2) - (4)].ival); }
    break;

  case 163:
#line 947 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (2)].el)); }
    break;

  case 164:
#line 948 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (2)].el)); }
    break;

  case 167:
#line 953 "css_parse.y"
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

  case 168:
#line 962 "css_parse.y"
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
#line 981 "css_parse.y"
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

  case 174:
#line 998 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssSwitchBlock_Name, cssMisc::cur_top->Prog());
	    blk->loop_type = cssCodeBlock::SWITCH;
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 175:
#line 1006 "css_parse.y"
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

  case 176:
#line 1021 "css_parse.y"
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

  case 177:
#line 1041 "css_parse.y"
    { /* for loop contained within own block */
            cssCodeBlock* blk = new cssCodeBlock(cssForLoop_Name, cssMisc::cur_top->Prog());
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 180:
#line 1052 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 183:
#line 1063 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of cond, push incr */
	      cssMisc::cur_top->ResetParseFlags();
	      cssCodeBlock* blk = new cssCodeBlock(cssForIncr_Name, cssMisc::cur_top->Prog());
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_back = 3; /* go back 3 to the cond */
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 184:
#line 1073 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of incr */
	      cssCodeBlock* blk = new cssCodeBlock(cssForLoopStmt_Name, cssMisc::cur_top->Prog());
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_type = cssCodeBlock::FOR;
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 185:
#line 1082 "css_parse.y"
    {
	   cssProg* cp = cssMisc::cur_top->Prog();
	   if(cp->owner_blk == NULL) {
	     yyerror("foreach loop current prog should have owner_blk, doesnt!");
	   }
	   cssMisc::cur_top->Pop(); /* pop the for_incr block */
	   Code1(cssBI::pop);	/* and code for getting rid of cond val */
	   /* swap the order of these: $5 = foreach_end_paren = stmt block, $4 = foreach_in = in block */
	   cp = cssMisc::cur_top->Prog(); /* current guy */
	   cssInst* foreach_in = cp->insts[(yyvsp[(4) - (6)].ival)];
	   cssInst* foreach_loop_stmt = cp->insts[(yyvsp[(5) - (6)].ival)];
	   /* swap */
	   cp->insts[(yyvsp[(4) - (6)].ival)] = foreach_loop_stmt;
	   cp->insts[(yyvsp[(5) - (6)].ival)] = foreach_in;
	   /* check if stmt is a new block: if so, then don't pop this guy */
	   cssMisc::cur_top->Pop(); /* pop the whole for loop! */
	   (yyval.ival) = (yyvsp[(1) - (6)].ival); }
    break;

  case 186:
#line 1101 "css_parse.y"
    { /* foreach loop contained within own block */
            cssCodeBlock* blk = new cssCodeBlock(cssForeachLoop_Name, cssMisc::cur_top->Prog());
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code);
	    // create the iterator and add it
	    cssTA_Base* itr = new cssTA_Base(NULL, 1, &TA_taBaseItr, "FOREACH_itr");
	    cssMisc::cur_foreach_itr = cssMisc::cur_top->AddVar(itr);
	    }
    break;

  case 187:
#line 1111 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (3)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->insts[(yyvsp[(3) - (3)].ival)-1]->SetJump((yyvsp[(3) - (3)].ival));
	    (yyval.el) = (yyvsp[(2) - (3)].el);  cssMisc::cur_foreach_var = (yyvsp[(2) - (3)].el);
	    }
    break;

  case 188:
#line 1119 "css_parse.y"
    {
	      cssMisc::cur_foreach_var = (yyvsp[(1) - (1)].el);
	    }
    break;

  case 189:
#line 1122 "css_parse.y"
    {
	      cssMisc::cur_foreach_var = (yyvsp[(1) - (1)].el);
	    }
    break;

  case 190:
#line 1127 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 191:
#line 1134 "css_parse.y"
    {
            /* code the cond opr */
            Code3(cssMisc::cur_foreach_itr, cssMisc::cur_foreach_var, cssBI::foreach_cond);
            cssMisc::cur_top->Pop(); /* get rid of cond, push incr */
	    cssMisc::cur_top->ResetParseFlags();
	    /* incr is just an empty shell for IF_TRUE case */
	    cssCodeBlock* incblk = new cssCodeBlock(cssForeachIncr_Name, cssMisc::cur_top->Prog());
	    incblk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	    incblk->loop_back = 3; /* go back 3 to the cond */
	    cssMisc::cur_top->AddStatic(incblk);
	    (yyval.ival) = Code1(incblk); cssMisc::cur_top->Push(incblk->code);
	    }
    break;

  case 192:
#line 1148 "css_parse.y"
    {
              cssMisc::cur_top->Pop(); /* get rid of incr */
	      cssCodeBlock* blk = new cssCodeBlock(cssForeachLoopStmt_Name, cssMisc::cur_top->Prog());
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_type = cssCodeBlock::FOREACH;
	      cssMisc::cur_top->AddStatic(blk);
	      (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code);
	      cssMisc::cur_foreach_var.Reset();
	      cssMisc::cur_foreach_itr.Reset();
	    }
    break;

  case 194:
#line 1164 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssDoLoop_Name, cssMisc::cur_top->Prog());
	    blk->loop_type = cssCodeBlock::DO;
	    cssMisc::cur_top->AddStatic(blk); /* while is all inside this do! */
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 195:
#line 1173 "css_parse.y"
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

  case 196:
#line 1196 "css_parse.y"
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

  case 197:
#line 1210 "css_parse.y"
    { }
    break;

  case 198:
#line 1213 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 199:
#line 1218 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          cssMisc::cur_top->Prog()->lastelseif = false; /* use this flag! */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 200:
#line 1222 "css_parse.y"
    {
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          cssMisc::cur_top->Prog()->lastelseif = true; /* be on guard for another else! */
          (yyval.ival) = (yyvsp[(2) - (3)].ival); cssMisc::cur_top->DoCompileCtrl(); /* need to do the bra! */ }
    break;

  case 204:
#line 1233 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); /* pop the if_false block */
	    /* now check for other else blocks that need popping! */
	    cssMisc::cur_top->PopElseBlocks(); /* pop residual elses! */
	    (yyval.ival) = (yyvsp[(1) - (2)].ival);  }
    break;

  case 205:
#line 1238 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[(1) - (4)].ival);  }
    break;

  case 206:
#line 1240 "css_parse.y"
    { /* do not pop if going into another else */
	  (yyval.ival) = (yyvsp[(1) - (3)].ival);  }
    break;

  case 207:
#line 1244 "css_parse.y"
    {
	  if(!cssMisc::cur_top->Prog()->lastelseif) {
	    cssMisc::cur_top->PopElseBlocks(); } }
    break;

  case 208:
#line 1249 "css_parse.y"
    {
          cssMisc::cur_top->Prog()->lastif = cssMisc::cur_top->Prog()->size; }
    break;

  case 209:
#line 1253 "css_parse.y"
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

  case 210:
#line 1271 "css_parse.y"
    { 
            cssCodeBlock* blk = new cssCodeBlock(cssIfTrueBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	    cssMisc::cur_top->AddStatic(blk);
	    Code1(blk); cssMisc::cur_top->Push(blk->code); (yyval.ival) = (yyvsp[(1) - (3)].ival); }
    break;

  case 211:
#line 1278 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name, cssMisc::cur_top->Prog());
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->Push(blk->code); }
    break;

  case 212:
#line 1285 "css_parse.y"
    {
            cssMisc::cur_top->Pop(); }
    break;

  case 213:
#line 1289 "css_parse.y"
    {
            cssCodeBlock* blk = new cssCodeBlock(cssBlock_Name, cssMisc::cur_top->Prog());
	    cssMisc::cur_top->AddStatic(blk);
	    (yyval.ival) = Code1(blk); cssMisc::cur_top->SetPush(blk->code); }
    break;

  case 214:
#line 1295 "css_parse.y"
    {  }
    break;

  case 215:
#line 1298 "css_parse.y"
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

  case 216:
#line 1321 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 219:
#line 1327 "css_parse.y"
    { Code1(cssBI::asgn); }
    break;

  case 220:
#line 1328 "css_parse.y"
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

  case 221:
#line 1338 "css_parse.y"
    { Code1(cssBI::asgn_add); }
    break;

  case 222:
#line 1339 "css_parse.y"
    { Code1(cssBI::asgn_sub); }
    break;

  case 223:
#line 1340 "css_parse.y"
    { Code1(cssBI::asgn_mult); }
    break;

  case 224:
#line 1341 "css_parse.y"
    { Code1(cssBI::asgn_div); }
    break;

  case 225:
#line 1342 "css_parse.y"
    { Code1(cssBI::asgn_mod); }
    break;

  case 226:
#line 1343 "css_parse.y"
    { Code1(cssBI::asgn_lshift); }
    break;

  case 227:
#line 1344 "css_parse.y"
    { Code1(cssBI::asgn_rshift); }
    break;

  case 228:
#line 1345 "css_parse.y"
    { Code1(cssBI::asgn_and); }
    break;

  case 229:
#line 1346 "css_parse.y"
    { Code1(cssBI::asgn_xor); }
    break;

  case 230:
#line 1347 "css_parse.y"
    { Code1(cssBI::asgn_or); }
    break;

  case 231:
#line 1348 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code2((yyvsp[(3) - (3)].el),(yyvsp[(1) - (3)].el)); }
    break;

  case 232:
#line 1349 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (5)].ival); Code2((yyvsp[(3) - (5)].el),(yyvsp[(1) - (5)].el)); }
    break;

  case 233:
#line 1350 "css_parse.y"
    {
	  (yyval.ival) = (yyvsp[(2) - (4)].ival); Code2((yyvsp[(3) - (4)].el),(yyvsp[(1) - (4)].el)); }
    break;

  case 234:
#line 1352 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code1((yyvsp[(1) - (3)].el)); }
    break;

  case 235:
#line 1353 "css_parse.y"
    { (yyval.ival) = (yyvsp[(4) - (5)].ival); Code1((yyvsp[(1) - (5)].el)); }
    break;

  case 236:
#line 1354 "css_parse.y"
    { Code1(cssBI::lor); }
    break;

  case 237:
#line 1355 "css_parse.y"
    { Code1(cssBI::land); }
    break;

  case 238:
#line 1356 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::addr_of); }
    break;

  case 239:
#line 1357 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::de_ptr); }
    break;

  case 240:
#line 1358 "css_parse.y"
    { Code1(cssBI::gt); }
    break;

  case 241:
#line 1359 "css_parse.y"
    { Code1(cssBI::ge); }
    break;

  case 242:
#line 1360 "css_parse.y"
    { Code1(cssBI::lt); }
    break;

  case 243:
#line 1361 "css_parse.y"
    { Code1(cssBI::le); }
    break;

  case 244:
#line 1362 "css_parse.y"
    { Code1(cssBI::eq); }
    break;

  case 245:
#line 1363 "css_parse.y"
    { Code1(cssBI::ne); }
    break;

  case 246:
#line 1364 "css_parse.y"
    { Code1(cssBI::cond); }
    break;

  case 247:
#line 1365 "css_parse.y"
    { Code1(cssBI::add); }
    break;

  case 248:
#line 1366 "css_parse.y"
    { Code1(cssBI::sub); }
    break;

  case 249:
#line 1367 "css_parse.y"
    { Code1(cssBI::mul); }
    break;

  case 250:
#line 1368 "css_parse.y"
    { Code1(cssBI::div); }
    break;

  case 251:
#line 1369 "css_parse.y"
    { Code1(cssBI::modulo); }
    break;

  case 252:
#line 1370 "css_parse.y"
    { Code1(cssBI::lshift); }
    break;

  case 253:
#line 1371 "css_parse.y"
    { Code1(cssBI::rshift); }
    break;

  case 254:
#line 1372 "css_parse.y"
    { Code1(cssBI::bit_and); }
    break;

  case 255:
#line 1373 "css_parse.y"
    { Code1(cssBI::bit_xor); }
    break;

  case 256:
#line 1374 "css_parse.y"
    { Code1(cssBI::bit_or); }
    break;

  case 257:
#line 1375 "css_parse.y"
    { Code1(cssBI::asgn_post_pp); }
    break;

  case 258:
#line 1376 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::asgn_pre_pp); }
    break;

  case 259:
#line 1377 "css_parse.y"
    { Code1(cssBI::asgn_post_mm); }
    break;

  case 260:
#line 1378 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::asgn_pre_mm); }
    break;

  case 261:
#line 1379 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::neg); }
    break;

  case 262:
#line 1380 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::lnot); }
    break;

  case 263:
#line 1381 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (2)].ival); Code1(cssBI::bitneg); }
    break;

  case 264:
#line 1382 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (1)].ival); }
    break;

  case 265:
#line 1383 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival); Code1(cssBI::de_array); }
    break;

  case 266:
#line 1385 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(2) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    (yyval.ival) = (yyvsp[(4) - (4)].ival); Code2((yyvsp[(2) - (4)].el), cssBI::cast); }
    break;

  case 267:
#line 1391 "css_parse.y"
    {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if((yyvsp[(1) - (4)].el).El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
  	    (yyval.ival) = (yyvsp[(3) - (4)].ival); Code2((yyvsp[(1) - (4)].el), cssBI::cast); }
    break;

  case 269:
#line 1398 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); }
    break;

  case 275:
#line 1411 "css_parse.y"
    { (yyval.ival) = (yyvsp[(2) - (3)].ival); Code1(cssBI::make_matrix);
	    int act_args = (yyvsp[(2) - (3)].ival);
	    cssMisc::parsing_matrix = false;
	    if(act_args > cssElFun::ArgMax) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(), "Too many initializer values for matrix, should have at most:", String(cssElFun::ArgMax), "got:",String(act_args)); }
	  }
    break;

  case 276:
#line 1420 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); /* an arg stop */
	    cssMisc::parsing_matrix = true; }
    break;

  case 277:
#line 1424 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::Internal.FindName("colon_mark")); }
    break;

  case 278:
#line 1427 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::Internal.FindName("comma_mark")); }
    break;

  case 279:
#line 1430 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::Internal.FindName("semicolon_mark")); }
    break;

  case 281:
#line 1435 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 282:
#line 1436 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (1)].ival); }
    break;

  case 283:
#line 1437 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival); }
    break;

  case 284:
#line 1438 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival); }
    break;

  case 285:
#line 1439 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival); }
    break;

  case 286:
#line 1440 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival); }
    break;

  case 287:
#line 1441 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (4)].ival); }
    break;

  case 288:
#line 1442 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (5)].ival); }
    break;

  case 289:
#line 1443 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (4)].ival); }
    break;

  case 290:
#line 1444 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival); }
    break;

  case 291:
#line 1447 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 292:
#line 1448 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 293:
#line 1449 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 2; }
    break;

  case 294:
#line 1453 "css_parse.y"
    { (yyval.ival) = Code2(cssBI::push_next, (yyvsp[(1) - (1)].el)); }
    break;

  case 295:
#line 1454 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].el_ival).ival; Code1((yyvsp[(1) - (2)].el_ival).el); }
    break;

  case 296:
#line 1455 "css_parse.y"
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

  case 297:
#line 1470 "css_parse.y"
    {
            (yyval.el_ival).ival = Code1(cssMisc::VoidElPtr); /* an arg stop */
	    (yyval.el_ival).el = (yyvsp[(1) - (2)].el);	/* save this pointer for later.. */
            if((yyvsp[(1) - (2)].el).El()->GetType() == cssEl::T_MbrScriptFun)
	      Code1(cssBI::push_cur_this); /* push this as first arg.. */
	    else if((yyvsp[(1) - (2)].el).El()->GetType() == cssEl::T_MbrCFun) {
	      cssMbrCFun* fun = (cssMbrCFun*)(yyvsp[(1) - (2)].el).El();
	      if(fun->HasFunFlag(cssMbrCFun::MBR_NO_THIS)) {
		// don't push this for this guy..
	      }
	      else {
		Code1(cssBI::push_cur_this); /* push this as first arg.. */
	      } } }
    break;

  case 298:
#line 1486 "css_parse.y"
    { Code2((yyvsp[(3) - (3)].el), cssBI::points_at);}
    break;

  case 299:
#line 1487 "css_parse.y"
    {
	    int mbno = (yyvsp[(1) - (3)].el).El()->GetMemberNo((const char*)*((yyvsp[(3) - (3)].el).El()));
	    if(mbno < 0) { (yyval.ival) = Code3((yyvsp[(1) - (3)].el), (yyvsp[(3) - (3)].el), cssBI::points_at); }
	    else { cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
		   (yyval.ival) = Code3((yyvsp[(1) - (3)].el), tmpint, cssBI::points_at); } }
    break;

  case 300:
#line 1495 "css_parse.y"
    { (yyval.ival) = Code3(cssBI::push_root, (yyvsp[(2) - (2)].el), cssBI::points_at); }
    break;

  case 301:
#line 1496 "css_parse.y"
    {
	  cssMisc::cur_scope = NULL;
	    cssEl* scp = (yyvsp[(1) - (2)].el).El()->GetScoped((const char*)*((yyvsp[(2) - (2)].el).El()));
	    if(scp != &cssMisc::Void) {  (yyval.ival) = Code1(scp); }
	    else { (yyval.ival) = Code3((yyvsp[(1) - (2)].el), (yyvsp[(2) - (2)].el), cssBI::scoper); } }
    break;

  case 302:
#line 1502 "css_parse.y"
    {
	  /* argstop is put in by member_fun; member_fun skips over end jump, 
	     uses it to find member_call*/
	  (yyval.ival) = (yyvsp[(1) - (3)].el_ival).ival;
	  cssMisc::cur_top->Prog()->insts[(yyvsp[(2) - (3)].ival)-1]->SetJump(Code1(cssBI::member_call)); }
    break;

  case 303:
#line 1507 "css_parse.y"
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

  case 304:
#line 1524 "css_parse.y"
    { Code2((yyvsp[(3) - (4)].el), cssBI::member_fun);
                                                  (yyval.el_ival).ival = (yyvsp[(1) - (4)].ival); (yyval.el_ival).el.Reset(); }
    break;

  case 305:
#line 1526 "css_parse.y"
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

  case 306:
#line 1544 "css_parse.y"
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

  case 307:
#line 1568 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].nm))); }
    break;

  case 308:
#line 1572 "css_parse.y"
    {
            (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].nm))); }
    break;

  case 309:
#line 1574 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName())); }
    break;

  case 310:
#line 1576 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName())); }
    break;

  case 311:
#line 1578 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 312:
#line 1582 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 313:
#line 1586 "css_parse.y"
    {
	    (yyval.el) = cssMisc::cur_top->Prog()->Stack()->Push(new cssString((yyvsp[(1) - (1)].el).El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding function:", (yyvsp[(1) - (1)].el).El()->PrintStr()); }
    break;

  case 314:
#line 1592 "css_parse.y"
    { /* these are treated identically in parsing */
           cssMisc::cur_top->parse_path_expr = true; }
    break;

  case 315:
#line 1594 "css_parse.y"
    {
           cssMisc::cur_top->parse_path_expr = true; }
    break;

  case 316:
#line 1598 "css_parse.y"
    {
   	   String tmpstr = String((yyvsp[(1) - (1)].nm));
           (yyval.el) = cssMisc::cur_top->AddLiteral(tmpstr);
           cssMisc::cur_top->parse_path_expr = false; }
    break;

  case 317:
#line 1602 "css_parse.y"
    {
           cssMisc::cur_top->parse_path_expr = false; }
    break;

  case 324:
#line 1614 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 325:
#line 1615 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 327:
#line 1619 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 328:
#line 1622 "css_parse.y"
    { (yyval.ival) = 1; }
    break;

  case 329:
#line 1623 "css_parse.y"
    { (yyval.ival) = (yyvsp[(1) - (3)].ival) + 1; }
    break;

  case 331:
#line 1627 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 332:
#line 1628 "css_parse.y"
    { 
	  String tmpstr = String((yyvsp[(1) - (1)].nm));
	  (yyval.ival) = Code1(cssMisc::cur_top->AddLiteral(tmpstr)); }
    break;

  case 333:
#line 1631 "css_parse.y"
    { (yyval.ival) = Code1((yyvsp[(1) - (1)].el)); }
    break;

  case 334:
#line 1634 "css_parse.y"
    { (yyval.ival) = Code1(cssMisc::VoidElPtr); }
    break;

  case 338:
#line 1642 "css_parse.y"
    { Code1(cssBI::pop); }
    break;

  case 339:
#line 1645 "css_parse.y"
    { Code1(cssInst::Stop); (yyval.ival) = cssMisc::cur_top->Prog()->size; }
    break;


/* Line 1267 of yacc.c.  */
#line 4584 "y.tab.c"
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


#line 1649 "css_parse.y"


	/* end of grammar */


void yyerror(const char* s) { 	/* called for yacc syntax error */
  cssMisc::SyntaxError(s);
}

