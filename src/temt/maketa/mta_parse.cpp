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
     MP_CLASS = 258,
     MP_TYPENAME = 259,
     MP_STRUCT = 260,
     MP_UNION = 261,
     MP_ENUM = 262,
     MP_FUNTYPE = 263,
     MP_STATIC = 264,
     MP_TEMPLATE = 265,
     MP_CONST = 266,
     MP_TYPEDEF = 267,
     MP_TYPE = 268,
     MP_NUMBER = 269,
     MP_FUNCTION = 270,
     MP_ARRAY = 271,
     MP_NAME = 272,
     MP_COMMENT = 273,
     MP_FUNCALL = 274,
     MP_SCOPER = 275,
     MP_EQUALS = 276,
     MP_USING = 277,
     MP_NAMESPACE = 278,
     MP_PUBLIC = 279,
     MP_PRIVATE = 280,
     MP_PROTECTED = 281,
     MP_OPERATOR = 282,
     MP_FRIEND = 283,
     MP_THISNAME = 284,
     MP_REGFUN = 285,
     MP_VIRTUAL = 286
   };
#endif
/* Tokens.  */
#define MP_CLASS 258
#define MP_TYPENAME 259
#define MP_STRUCT 260
#define MP_UNION 261
#define MP_ENUM 262
#define MP_FUNTYPE 263
#define MP_STATIC 264
#define MP_TEMPLATE 265
#define MP_CONST 266
#define MP_TYPEDEF 267
#define MP_TYPE 268
#define MP_NUMBER 269
#define MP_FUNCTION 270
#define MP_ARRAY 271
#define MP_NAME 272
#define MP_COMMENT 273
#define MP_FUNCALL 274
#define MP_SCOPER 275
#define MP_EQUALS 276
#define MP_USING 277
#define MP_NAMESPACE 278
#define MP_PUBLIC 279
#define MP_PRIVATE 280
#define MP_PROTECTED 281
#define MP_OPERATOR 282
#define MP_FRIEND 283
#define MP_THISNAME 284
#define MP_REGFUN 285
#define MP_VIRTUAL 286




/* Copy the first part of user declarations.  */
#line 18 "mta_parse.y"


#include "maketa.h"
#include <taMisc>
#include <BuiltinTypeDefs>

#if defined(SUN4) && !defined(__GNUG__) && !defined(SOLARIS)
#include <alloca.h>
#endif

#if defined(SGI) || defined(SGIdebug)
#include <alloca.h>
#endif

static String_PArray bogus_inh_opts;

#define SETDESC(ty,cm)	mta->SetDesc(cm, ty->desc, ty->inh_opts, ty->opts, ty->lists)

#define SETENUMDESC(ty,cm) mta->SetDesc(cm, ty->desc, bogus_inh_opts, ty->opts, ty->lists)

void yyerror(const char *s);
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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef struct YYSTYPE
#line 46 "mta_parse.y"
{
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  const char*   chr;
  int	   	rval;
}
/* Line 193 of yacc.c.  */
#line 192 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 205 "y.tab.c"

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
#define YYLAST   741

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  49
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  98
/* YYNRULES -- Number of rules.  */
#define YYNRULES  286
/* YYNRULES -- Number of states.  */
#define YYNSTATES  480

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   286

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    41,     2,     2,     2,    47,     2,
      33,    34,    32,    42,    38,    43,    46,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    48,
      40,     2,    39,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    36,    44,    35,    45,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    19,    22,
      25,    28,    31,    34,    36,    38,    41,    44,    49,    53,
      59,    63,    68,    73,    76,    84,    89,    93,    98,   102,
     106,   108,   110,   112,   117,   120,   124,   128,   131,   133,
     135,   138,   142,   144,   147,   151,   155,   158,   161,   165,
     169,   171,   175,   178,   181,   183,   185,   188,   191,   193,
     196,   199,   201,   205,   207,   210,   212,   217,   220,   222,
     225,   227,   229,   231,   234,   238,   240,   243,   247,   250,
     254,   259,   263,   267,   270,   273,   276,   280,   284,   290,
     295,   300,   306,   308,   310,   314,   317,   320,   322,   325,
     327,   330,   335,   340,   345,   349,   351,   356,   360,   363,
     367,   371,   373,   375,   378,   382,   385,   388,   391,   393,
     396,   398,   401,   403,   406,   408,   410,   414,   416,   420,
     424,   428,   430,   432,   434,   436,   438,   441,   443,   445,
     447,   449,   452,   454,   456,   459,   462,   465,   467,   469,
     472,   476,   481,   484,   489,   494,   496,   500,   502,   507,
     509,   513,   518,   523,   528,   534,   540,   545,   551,   557,
     563,   568,   572,   577,   582,   588,   593,   599,   605,   611,
     617,   619,   622,   625,   629,   633,   636,   641,   648,   652,
     655,   657,   659,   661,   664,   667,   670,   674,   676,   679,
     683,   685,   687,   690,   693,   697,   699,   702,   704,   708,
     713,   715,   718,   720,   723,   727,   734,   736,   739,   741,
     743,   746,   750,   753,   756,   759,   761,   763,   766,   768,
     770,   772,   774,   777,   779,   782,   784,   787,   789,   792,
     794,   799,   801,   805,   809,   813,   817,   820,   823,   826,
     828,   833,   838,   841,   844,   847,   850,   852,   855,   857,
     859,   863,   865,   867,   870,   873,   876,   881,   883,   886,
     889,   892,   894,   896,   898,   900,   902,   904,   907,   910,
     912,   914,   916,   918,   920,   922,   924
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      50,     0,    -1,    -1,    50,    51,    -1,    50,    56,    -1,
      50,    60,    -1,    50,    62,    -1,    50,    72,    -1,    50,
      74,    -1,    50,    83,    -1,    50,    86,    -1,    50,    87,
      -1,    50,     1,    -1,    52,    -1,    53,    -1,    53,    18,
      -1,    12,    54,    -1,    12,    64,   138,   141,    -1,    12,
       4,    54,    -1,    12,     4,    64,   138,   141,    -1,    55,
     124,   141,    -1,    55,   124,    16,   141,    -1,   127,    18,
     124,   141,    -1,    55,   141,    -1,    55,    33,    32,   124,
      34,   113,   141,    -1,    55,    20,    32,   124,    -1,   124,
     124,   141,    -1,   124,   124,    16,   141,    -1,   124,    55,
     141,    -1,    55,    55,   141,    -1,   127,    -1,   132,    -1,
      57,    -1,    58,    90,    35,   141,    -1,    59,    36,    -1,
      59,    18,    36,    -1,    59,    36,    18,    -1,     7,   124,
      -1,     7,    -1,    61,    -1,    67,   141,    -1,    67,   141,
      18,    -1,    63,    -1,    64,   141,    -1,    64,   139,   141,
      -1,    65,    97,    35,    -1,    65,    35,    -1,    66,    36,
      -1,    66,    18,    36,    -1,    66,    36,    18,    -1,    67,
      -1,    67,    37,    68,    -1,   145,   124,    -1,   145,   127,
      -1,   145,    -1,   143,    -1,   143,   124,    -1,   143,   127,
      -1,   144,    -1,   144,   124,    -1,   144,   127,    -1,    69,
      -1,    68,    38,    69,    -1,   127,    -1,    70,   127,    -1,
      17,    -1,    17,   134,   135,    39,    -1,    70,    17,    -1,
      71,    -1,    70,    71,    -1,   142,    -1,    31,    -1,    73,
      -1,    78,   141,    -1,    78,   141,    18,    -1,    75,    -1,
      76,   141,    -1,    76,   139,   141,    -1,    79,   107,    -1,
      76,   141,    18,    -1,    76,   139,   141,    18,    -1,    79,
     107,    18,    -1,    77,    97,    35,    -1,    77,    35,    -1,
      78,   141,    -1,    78,    36,    -1,    78,    18,    36,    -1,
      78,    36,    18,    -1,   146,    80,    81,    39,    66,    -1,
     146,    80,    39,    66,    -1,   146,    80,    39,     8,    -1,
     146,    80,    81,    39,     8,    -1,    40,    -1,    82,    -1,
      81,    38,    82,    -1,     3,   124,    -1,     4,   124,    -1,
       4,    -1,    17,    17,    -1,   127,    -1,   127,   124,    -1,
       3,   124,    21,   127,    -1,     4,   124,    21,   127,    -1,
     127,   124,    21,   127,    -1,   127,    21,   127,    -1,    84,
      -1,    30,   126,    85,   110,    -1,    30,    85,   110,    -1,
     109,   113,    -1,    88,    89,   141,    -1,    88,    89,    36,
      -1,    23,    -1,    17,    -1,    89,    17,    -1,    89,    20,
      17,    -1,    89,    33,    -1,    89,    34,    -1,    89,    41,
      -1,    91,    -1,    90,    91,    -1,    92,    -1,    92,    18,
      -1,    93,    -1,    93,    38,    -1,     1,    -1,    96,    -1,
      96,    21,    94,    -1,    95,    -1,    94,    42,    95,    -1,
      94,    43,    95,    -1,    94,    44,    95,    -1,    17,    -1,
      14,    -1,    15,    -1,    17,    -1,    98,    -1,    97,    98,
      -1,    99,    -1,   105,    -1,    57,    -1,    53,    -1,    64,
     141,    -1,     1,    -1,   100,    -1,    24,    37,    -1,    25,
      37,    -1,    26,    37,    -1,    18,    -1,   101,    -1,     9,
     101,    -1,   125,   102,   141,    -1,   125,   103,    21,   141,
      -1,   102,   141,    -1,   125,   103,    16,   141,    -1,   125,
     104,   113,   141,    -1,   103,    -1,   102,    38,   103,    -1,
      17,    -1,    33,    32,    17,    34,    -1,   106,    -1,   121,
     114,   110,    -1,     8,   121,   114,   110,    -1,    31,   121,
     114,   110,    -1,   121,   114,   118,   110,    -1,     8,   121,
     114,   118,   110,    -1,    31,   121,   114,   118,   110,    -1,
      45,   121,   114,   110,    -1,     8,    45,   121,   114,   110,
      -1,    31,    45,   121,   114,   110,    -1,    28,   126,    27,
     113,   110,    -1,    28,    27,   113,   110,    -1,    28,   126,
     141,    -1,    28,     3,   126,   141,    -1,    28,     3,    17,
     141,    -1,    28,   126,   109,   113,   110,    -1,    28,   109,
     113,   110,    -1,    22,    17,    20,    27,   141,    -1,    22,
      17,    20,    17,   141,    -1,    22,    13,    20,    27,   141,
      -1,    22,    13,    20,    17,   141,    -1,   107,    -1,     9,
     107,    -1,    31,   107,    -1,     8,    31,   107,    -1,   125,
     108,   110,    -1,   108,   110,    -1,   125,    27,   113,   110,
      -1,   125,    27,    33,    34,   113,   110,    -1,    27,   113,
     110,    -1,   109,   113,    -1,    17,    -1,   141,    -1,   111,
      -1,   112,   141,    -1,    18,   141,    -1,    18,   111,    -1,
      18,   112,   141,    -1,    15,    -1,    11,    15,    -1,    11,
      18,    15,    -1,    21,    -1,    11,    -1,    11,    21,    -1,
      33,    34,    -1,    33,   115,    34,    -1,    34,    -1,   115,
      34,    -1,   116,    -1,   115,    38,   116,    -1,   115,    46,
      46,    46,    -1,   117,    -1,   117,    21,    -1,   127,    -1,
     127,    17,    -1,   127,    17,    16,    -1,   127,    33,    32,
      17,    34,   113,    -1,    17,    -1,   122,   119,    -1,   120,
      -1,    18,    -1,   119,   120,    -1,   119,    38,   120,    -1,
      13,    19,    -1,    17,    19,    -1,    29,    33,    -1,    37,
      -1,    32,    -1,   123,    32,    -1,    17,    -1,   126,    -1,
     127,    -1,     8,    -1,     8,   127,    -1,   128,    -1,   128,
      47,    -1,   129,    -1,   129,   123,    -1,   130,    -1,    11,
     130,    -1,   131,    -1,     4,   131,    20,    17,    -1,   133,
      -1,    13,    20,    17,    -1,    29,    20,    17,    -1,    13,
      20,    13,    -1,    29,    20,    13,    -1,   140,    17,    -1,
     140,    13,    -1,    20,    13,    -1,    29,    -1,    13,   134,
     135,    39,    -1,    29,   134,   135,    39,    -1,   143,   133,
      -1,   143,   124,    -1,   144,   133,    -1,   144,   124,    -1,
      13,    -1,   133,    13,    -1,    40,    -1,   136,    -1,   135,
      38,   136,    -1,   130,    -1,    17,    -1,    17,   123,    -1,
      17,    47,    -1,    17,    16,    -1,    17,    33,    17,    34,
      -1,    14,    -1,   137,    17,    -1,   137,    13,    -1,    17,
     137,    -1,    11,    -1,     8,    -1,    17,    -1,    13,    -1,
      29,    -1,    17,    -1,    17,    16,    -1,    17,    20,    -1,
      48,    -1,    24,    -1,    25,    -1,    26,    -1,     5,    -1,
       6,    -1,     3,    -1,    10,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   113,   113,   114,   116,   118,   120,   122,   124,   126,
     128,   130,   135,   139,   148,   149,   153,   154,   156,   158,
     162,   165,   168,   174,   183,   186,   189,   191,   193,   195,
     199,   200,   203,   208,   211,   212,   213,   216,   221,   229,
     235,   236,   240,   247,   248,   252,   255,   261,   264,   267,
     274,   275,   278,   282,   284,   289,   294,   298,   300,   305,
     309,   314,   316,   322,   323,   324,   325,   326,   329,   330,
     333,   334,   338,   344,   345,   349,   356,   357,   358,   369,
     370,   371,   386,   389,   392,   396,   400,   403,   409,   417,
     426,   431,   437,   441,   442,   446,   447,   448,   450,   452,
     453,   456,   460,   464,   468,   474,   485,   487,   491,   496,
     502,   508,   512,   513,   514,   515,   516,   517,   520,   521,
     524,   525,   528,   529,   530,   535,   538,   546,   547,   550,
     553,   559,   563,   564,   568,   571,   572,   575,   583,   595,
     601,   605,   615,   619,   620,   621,   622,   623,   642,   643,
     649,   650,   651,   652,   659,   663,   667,   673,   679,   684,
     685,   686,   687,   688,   689,   690,   691,   692,   693,   694,
     695,   696,   697,   698,   699,   710,   721,   722,   723,   724,
     728,   729,   730,   732,   737,   738,   739,   740,   741,   744,
     751,   755,   756,   757,   758,   759,   760,   764,   765,   766,
     770,   771,   772,   775,   776,   779,   780,   783,   784,   785,
     788,   790,   801,   805,   809,   813,   817,   823,   827,   828,
     829,   830,   833,   834,   838,   842,   845,   846,   849,   853,
     857,   858,   859,   862,   863,   872,   873,   884,   885,   893,
     894,   897,   898,   905,   912,   913,   914,   922,   926,   927,
     928,   943,   951,   953,   954,   956,   959,   960,   972,   977,
     978,   983,   985,   987,   989,   991,   993,   995,   998,  1000,
    1002,  1007,  1008,  1011,  1012,  1013,  1016,  1017,  1020,  1023,
    1026,  1027,  1028,  1032,  1039,  1046,  1053
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "MP_CLASS", "MP_TYPENAME", "MP_STRUCT",
  "MP_UNION", "MP_ENUM", "MP_FUNTYPE", "MP_STATIC", "MP_TEMPLATE",
  "MP_CONST", "MP_TYPEDEF", "MP_TYPE", "MP_NUMBER", "MP_FUNCTION",
  "MP_ARRAY", "MP_NAME", "MP_COMMENT", "MP_FUNCALL", "MP_SCOPER",
  "MP_EQUALS", "MP_USING", "MP_NAMESPACE", "MP_PUBLIC", "MP_PRIVATE",
  "MP_PROTECTED", "MP_OPERATOR", "MP_FRIEND", "MP_THISNAME", "MP_REGFUN",
  "MP_VIRTUAL", "'*'", "'('", "')'", "'}'", "'{'", "':'", "','", "'>'",
  "'<'", "'\"'", "'+'", "'-'", "'|'", "'~'", "'.'", "'&'", "';'",
  "$accept", "list", "typedefn", "typedefns", "typedsub", "defn", "tdtype",
  "enumdefn", "enumdsub", "enumname", "enumnm", "classdecl", "classdecls",
  "classdefn", "classdefns", "classdsub", "classname", "classhead",
  "classnm", "classinh", "classpar", "classptyp", "classpmod", "templdecl",
  "templdecls", "templdefn", "templdefns", "templdsub", "templname",
  "templhead", "templfun", "templopen", "templpars", "templpar", "fundecl",
  "funnm", "regfundefn", "usenamespc", "namespc", "namespcword",
  "namespcnms", "enums", "enumline", "enumitm", "enumitms", "enummath",
  "enummathitm", "enmitmname", "membs", "membline", "membdefn",
  "basicmemb", "nostatmemb", "membnames", "membname", "membfunp",
  "methdefn", "basicmeth", "nostatmeth", "mbfundefn", "methname",
  "fundefn", "funsubdefn", "funsubdecl", "funargs", "constfun", "args",
  "argdefn", "subargdefn", "constrlist", "constitms", "constref",
  "consthsnm", "constcoln", "ptrs", "tyname", "membtype", "ftype", "type",
  "noreftype", "constype", "typenmtyp", "subtype", "structype", "combtype",
  "typtemplopen", "templargs", "templarg", "templargmisc", "tdname",
  "varname", "namespctyp", "term", "access", "structkeyword",
  "structkeyword", "classkeyword", "templatekeyword", 0
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
     285,   286,    42,    40,    41,   125,   123,    58,    44,    62,
      60,    34,    43,    45,   124,   126,    46,    38,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    49,    50,    50,    50,    50,    50,    50,    50,    50,
      50,    50,    50,    51,    52,    52,    53,    53,    53,    53,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      55,    55,    56,    57,    58,    58,    58,    59,    59,    60,
      61,    61,    62,    63,    63,    64,    64,    65,    65,    65,
      66,    66,    67,    67,    67,    67,    67,    67,    67,    67,
      67,    68,    68,    69,    69,    69,    69,    69,    70,    70,
      71,    71,    72,    73,    73,    74,    75,    75,    75,    75,
      75,    75,    76,    76,    76,    77,    77,    77,    78,    78,
      79,    79,    80,    81,    81,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    83,    84,    84,    85,    86,
      87,    88,    89,    89,    89,    89,    89,    89,    90,    90,
      91,    91,    92,    92,    92,    93,    93,    94,    94,    94,
      94,    95,    95,    95,    96,    97,    97,    98,    98,    98,
      98,    98,    98,    99,    99,    99,    99,    99,   100,   100,
     101,   101,   101,   101,   101,   102,   102,   103,   104,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     106,   106,   106,   106,   107,   107,   107,   107,   107,   108,
     109,   110,   110,   110,   110,   110,   110,   111,   111,   111,
     112,   112,   112,   113,   113,   114,   114,   115,   115,   115,
     116,   116,   117,   117,   117,   117,   117,   118,   119,   119,
     119,   119,   120,   120,   121,   122,   123,   123,   124,   125,
     126,   126,   126,   127,   127,   128,   128,   129,   129,   130,
     130,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   132,   132,   132,   132,   133,   133,   134,   135,
     135,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   137,   137,   138,   138,   138,   139,   139,   140,   141,
     142,   142,   142,   143,   144,   145,   146
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     1,     2,     2,     4,     3,     5,
       3,     4,     4,     2,     7,     4,     3,     4,     3,     3,
       1,     1,     1,     4,     2,     3,     3,     2,     1,     1,
       2,     3,     1,     2,     3,     3,     2,     2,     3,     3,
       1,     3,     2,     2,     1,     1,     2,     2,     1,     2,
       2,     1,     3,     1,     2,     1,     4,     2,     1,     2,
       1,     1,     1,     2,     3,     1,     2,     3,     2,     3,
       4,     3,     3,     2,     2,     2,     3,     3,     5,     4,
       4,     5,     1,     1,     3,     2,     2,     1,     2,     1,
       2,     4,     4,     4,     3,     1,     4,     3,     2,     3,
       3,     1,     1,     2,     3,     2,     2,     2,     1,     2,
       1,     2,     1,     2,     1,     1,     3,     1,     3,     3,
       3,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     2,     1,     1,     2,     2,     2,     1,     1,     2,
       3,     4,     2,     4,     4,     1,     3,     1,     4,     1,
       3,     4,     4,     4,     5,     5,     4,     5,     5,     5,
       4,     3,     4,     4,     5,     4,     5,     5,     5,     5,
       1,     2,     2,     3,     3,     2,     4,     6,     3,     2,
       1,     1,     1,     2,     2,     2,     3,     1,     2,     3,
       1,     1,     2,     2,     3,     1,     2,     1,     3,     4,
       1,     2,     1,     2,     3,     6,     1,     2,     1,     1,
       2,     3,     2,     2,     2,     1,     1,     2,     1,     1,
       1,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       4,     1,     3,     3,     3,     3,     2,     2,     2,     1,
       4,     4,     2,     2,     2,     2,     1,     2,     1,     1,
       3,     1,     1,     2,     2,     2,     4,     1,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     1,    12,   285,   283,   284,    38,   286,     0,
     111,     0,     3,    13,    14,     4,    32,     0,     0,     5,
      39,     6,    42,     0,     0,     0,    50,     7,    72,     8,
      75,     0,     0,     0,     0,     9,   105,    10,    11,     0,
      55,    58,    54,     0,   228,    37,     0,     0,   256,   228,
       0,   249,    16,     0,     0,    50,     0,    30,   233,   235,
     237,   239,    31,   241,     0,    55,    58,     0,   231,   190,
       0,     0,     0,   230,    15,   124,   134,     0,   118,   120,
     122,   125,     0,    34,   276,   279,     0,    43,   142,   231,
       0,   157,   147,     0,     0,     0,     0,     0,     0,   249,
       0,    46,     0,   140,   139,     0,     0,   135,   137,   143,
     148,     0,   155,   138,   159,   180,     0,     0,     0,     0,
     229,     0,    47,     0,    40,     0,    76,    83,     0,     0,
      85,    73,    78,     0,   112,     0,    56,    57,    59,    60,
      52,    53,    92,     0,    18,     0,   239,     0,   238,     0,
     258,     0,   278,   248,     0,     0,     0,     0,     0,     0,
      30,    23,     0,     0,   274,   273,   275,     0,     0,     0,
       0,   234,   226,   236,   257,   247,   246,   253,   252,   255,
     254,     0,   232,   201,   197,     0,   200,   107,   192,     0,
     191,     0,   108,   190,     0,     0,   119,   121,   123,     0,
      35,    36,   277,    44,     0,     0,     0,   149,   181,     0,
       0,   144,   145,   146,     0,     0,     0,     0,     0,   224,
       0,   182,     0,     0,     0,   141,    45,   136,     0,   152,
     185,   189,   216,   205,     0,     0,   207,   210,   212,   157,
       0,     0,     0,   155,     0,     0,    48,    49,    65,   280,
     281,   282,    71,    51,    61,     0,    68,    63,    70,    41,
      77,    79,    82,    86,    87,    74,    81,   113,     0,   115,
     116,   110,   117,   109,     0,    97,     0,     0,     0,    93,
      99,     0,     0,   244,   242,   272,   271,   267,   262,   261,
       0,   259,     0,   245,   243,     0,     0,     0,    29,     0,
      20,   256,   253,   252,   255,   254,    17,    28,     0,    26,
       0,   227,   198,     0,   202,   195,     0,   194,   193,   203,
       0,   106,    33,   132,   133,   131,   126,   127,   183,     0,
       0,     0,     0,   188,     0,     0,     0,     0,     0,     0,
     171,     0,     0,     0,   157,   156,   225,   160,     0,     0,
     206,     0,     0,   211,   213,     0,     0,     0,     0,   150,
       0,     0,     0,   184,     0,     0,    67,    69,    64,    80,
     114,    95,    96,    98,    90,    89,     0,     0,     0,   100,
      19,   240,   265,     0,   264,   263,   270,     0,   250,   269,
     268,   251,    25,     0,    21,    27,    22,   199,   196,   204,
       0,     0,     0,     0,   161,     0,     0,     0,     0,     0,
     173,   172,   170,   175,     0,     0,     0,   162,     0,   166,
     163,     0,     0,   219,   217,   218,   208,     0,   214,     0,
     203,   186,     0,   153,   151,   154,     0,    62,     0,     0,
      94,    91,    88,   104,     0,     0,   260,     0,   128,   129,
     130,   167,   164,   179,   178,   177,   176,   169,   174,   168,
     165,   222,   223,     0,   220,   209,     0,     0,   158,    66,
     101,   102,   103,   266,     0,   221,     0,   187,    24,   215
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    12,    13,   103,    52,    53,    15,   104,    17,
      18,    19,    20,    21,    22,   105,    24,    25,    55,   253,
     254,   255,   256,    27,    28,    29,    30,    31,    32,    33,
      34,   143,   278,   279,    35,    36,    70,    37,    38,    39,
     135,    77,    78,    79,    80,   326,   327,    81,   106,   107,
     108,   109,   110,   111,   112,   244,   113,   114,   115,   116,
     117,   187,   188,   189,   192,   234,   235,   236,   237,   348,
     424,   425,   118,   349,   173,    56,   119,   120,    73,    58,
      59,    60,    61,    62,    63,   155,   290,   291,   292,   167,
      86,    64,   190,   258,    40,    41,    42,    43
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -403
static const yytype_int16 yypact[] =
{
    -403,   555,  -403,  -403,  -403,  -403,  -403,   105,  -403,   616,
    -403,   681,  -403,  -403,   106,  -403,  -403,   158,    89,  -403,
    -403,  -403,  -403,    62,   388,    98,    25,  -403,  -403,  -403,
    -403,    62,   449,   103,   643,  -403,  -403,  -403,  -403,   144,
     691,   691,   691,   169,  -403,  -403,   635,   183,    -2,   195,
     236,   113,  -403,   188,   230,   161,   662,   238,   239,   241,
    -403,  -403,  -403,   302,   284,   691,   691,   168,   702,   195,
      90,   327,   350,  -403,  -403,  -403,  -403,    20,  -403,   354,
     366,   353,   373,   404,   419,  -403,   416,  -403,  -403,   250,
     670,   224,  -403,   286,   433,   435,   442,   327,   323,   143,
     221,  -403,   454,  -403,  -403,   416,   484,  -403,  -403,  -403,
    -403,   178,  -403,  -403,  -403,  -403,    90,   327,   296,   245,
    -403,   450,   480,   570,   481,   416,   482,  -403,   519,   467,
     487,    81,   489,   277,  -403,   194,  -403,  -403,  -403,  -403,
    -403,  -403,  -403,   285,  -403,   230,   494,   195,  -403,   336,
    -403,   417,  -403,  -403,   356,   417,    58,   501,   416,    27,
    -403,  -403,   364,   364,  -403,  -403,  -403,   416,   416,    56,
     105,  -403,  -403,   502,  -403,  -403,  -403,   134,   310,   142,
     310,   494,  -403,   411,  -403,    43,  -403,  -403,  -403,   416,
    -403,   407,  -403,  -403,    90,   416,  -403,  -403,  -403,   318,
    -403,  -403,  -403,  -403,   643,   454,   296,  -403,  -403,   515,
     518,  -403,  -403,  -403,    90,   692,   327,   327,    79,  -403,
     454,  -403,   296,   488,   296,  -403,  -403,  -403,   523,  -403,
    -403,  -403,   195,  -403,    66,   237,  -403,   521,   185,   516,
     520,   525,   178,   261,   327,    90,  -403,  -403,   129,  -403,
    -403,  -403,  -403,   513,  -403,   587,  -403,  -403,  -403,  -403,
     534,  -403,  -403,  -403,  -403,  -403,  -403,  -403,   542,  -403,
    -403,  -403,  -403,  -403,   105,   346,   362,   437,   -13,  -403,
     381,   416,   556,  -403,  -403,  -403,  -403,  -403,   157,  -403,
     226,  -403,   390,  -403,  -403,   349,   105,   105,  -403,   416,
    -403,  -403,  -403,   302,  -403,   302,  -403,  -403,   416,  -403,
     416,  -403,  -403,   560,  -403,  -403,   416,  -403,  -403,  -403,
     246,  -403,  -403,  -403,  -403,  -403,   405,  -403,  -403,   296,
      66,   293,   331,  -403,     4,   416,    90,    90,   327,   327,
    -403,   296,    66,    90,  -403,  -403,  -403,  -403,    90,    15,
    -403,   712,   531,  -403,   564,   550,   559,    90,   567,  -403,
     416,   416,   416,  -403,   417,   570,   195,  -403,  -403,  -403,
    -403,   565,   568,  -403,  -403,  -403,   351,   563,   702,   571,
    -403,  -403,  -403,   580,  -403,   502,  -403,   417,  -403,  -403,
    -403,  -403,  -403,   569,  -403,  -403,  -403,  -403,  -403,  -403,
     318,   318,   318,    90,  -403,    90,   416,   416,   416,   416,
    -403,  -403,  -403,  -403,    90,    90,    90,  -403,    90,  -403,
    -403,   583,   586,  -403,   102,  -403,  -403,   562,  -403,   589,
     327,  -403,   575,  -403,  -403,  -403,   443,  -403,   702,   702,
    -403,  -403,  -403,  -403,   702,   576,  -403,   327,  -403,  -403,
    -403,  -403,  -403,  -403,  -403,  -403,  -403,  -403,  -403,  -403,
    -403,  -403,  -403,   446,  -403,  -403,   581,    90,  -403,  -403,
    -403,  -403,  -403,  -403,   416,  -403,   327,  -403,  -403,  -403
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -403,  -403,  -403,  -403,   613,   577,   412,  -403,   623,  -403,
    -403,  -403,  -403,  -403,  -403,    48,  -403,  -257,   624,  -403,
     252,  -403,   371,  -403,  -403,  -403,  -403,  -403,  -403,  -403,
    -403,  -403,  -403,   254,  -403,  -403,   572,  -403,  -403,  -403,
    -403,  -403,   551,  -403,  -403,  -403,   116,  -403,   599,    22,
    -403,  -403,   544,   524,  -102,  -403,  -403,  -403,   -20,   104,
       2,   -97,   447,   452,   -86,  -155,  -176,   291,  -403,  -175,
    -403,  -402,   -73,  -403,   347,    -6,   -22,    -5,     0,  -403,
    -403,   -42,   -44,  -403,   -21,   -41,  -151,   262,   365,   505,
     626,  -403,   -23,  -403,    30,    39,  -403,  -403
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -242
static const yytype_int16 yytable[] =
{
      87,    45,   146,   124,   295,   148,    72,   151,   126,    57,
     131,   214,   133,    71,   132,   320,   206,   243,   149,   230,
     375,    75,   464,   181,   152,   376,   377,   222,   421,   224,
     161,   231,   422,   423,   136,   138,   140,    76,   150,    65,
     137,   139,   141,   299,   178,   180,    57,   159,    66,    23,
     169,   330,    85,   160,   183,   195,   160,    54,   184,   177,
     179,   475,   123,   203,   186,   137,   139,   342,   182,   343,
     208,   153,   308,    85,    71,    85,    65,   183,   133,    84,
     221,   184,   225,   162,   185,    66,   162,   186,   229,   182,
     296,    85,   163,   218,   145,   163,   193,   321,   -84,   265,
     217,   183,   260,   346,    85,   184,   338,    82,   185,   289,
      85,   186,   273,   289,    85,   421,   121,   333,   238,   422,
     442,   129,    44,   257,    74,    83,   345,    85,   227,   -84,
     336,   337,   329,   154,   122,   298,   300,   347,    85,   130,
     463,   303,   305,   280,   306,   307,   309,   341,   363,   152,
     227,    85,   -56,   150,   357,   405,   302,   304,   362,    75,
     -59,   134,   317,   154,   310,   285,   318,   418,   286,   150,
     -56,   -56,   322,   382,   403,    76,   219,   152,   -59,   -59,
     320,    48,   133,   150,   328,   147,   416,    67,    50,   172,
     383,   238,    67,     5,     6,   340,    48,    51,   123,    47,
     147,    48,   354,    50,   384,    49,   238,   364,   156,   142,
     335,   267,    51,   436,   268,   152,   228,    51,   355,   359,
     339,   157,   238,   245,   238,    67,    85,   269,   270,    68,
     271,   181,    47,   404,    48,   272,    85,   245,    69,   412,
     413,    50,    85,   164,   152,   417,   419,   165,    97,   153,
      99,   420,   414,   415,    67,   368,   170,  -190,   380,   166,
     431,    47,   239,    48,   387,   388,   220,   147,   371,   372,
      50,   350,   240,   172,   379,   351,   394,   360,   241,    99,
     399,   204,   361,   352,   351,   395,   171,   396,   274,   275,
     392,   393,   352,   398,   193,   205,    47,   175,    48,   209,
      67,   176,   276,   210,   240,    50,   451,    47,   452,    48,
     406,   410,   411,   232,    51,   174,    50,   457,   458,   459,
     407,   460,   289,   174,   277,    51,   215,    67,  -241,   238,
     233,    68,   323,   324,    47,   325,    48,   433,   434,   435,
      69,   238,  -241,    50,   467,   289,  -241,  -241,   408,   283,
     216,   238,    51,   284,   274,   275,   238,  -241,   409,    48,
     191,   474,    47,    49,    48,   257,    50,   193,   276,   293,
     477,    50,   197,   294,   199,    51,   280,   301,   443,   373,
      51,    44,   152,   453,   454,   455,   456,   387,   391,    88,
     479,     4,    67,     5,     6,     7,    89,    90,    44,    47,
       9,    48,   378,   389,   198,    91,    92,   390,    50,   200,
      93,    67,    94,    95,    96,    97,    98,    99,    47,   100,
      48,    67,   201,   101,   232,   285,   312,    50,   286,   313,
      48,   287,   314,   102,   288,   202,    51,    50,   470,   471,
       4,   319,     5,     6,   472,   374,    51,   400,   401,   402,
      88,   478,     4,    67,     5,     6,     7,    89,    90,   421,
      47,     9,    48,   422,    85,   158,    91,    92,   168,    50,
     211,    93,   212,    94,    95,    96,    97,    98,    99,   213,
     100,   387,   469,   223,   127,    88,   246,     4,    67,     5,
       6,     7,    89,    90,   102,    47,     9,    48,   247,   259,
     261,    91,    92,   263,    50,   264,    93,   266,    94,    95,
      96,    97,    98,    99,   282,   100,   448,   449,   450,   226,
      88,   219,     4,    67,     5,     6,     7,    89,    90,   102,
      47,     9,    48,   297,   311,   331,    91,    92,   332,    50,
     344,    93,   353,    94,    95,    96,    97,    98,    99,  -190,
     100,   365,   369,   356,   262,     2,     3,   358,     4,   370,
       5,     6,     7,    67,   102,     8,     4,     9,     5,     6,
      47,   441,    48,   381,    67,   397,   232,   427,    10,    50,
     428,    47,   429,    48,   432,    11,   438,   248,    51,   439,
      50,    67,   444,   430,   249,   250,   251,   445,    47,    51,
      48,   252,   461,   447,   366,   462,   466,    50,   465,   468,
     473,   249,   250,   251,    14,   476,    51,   437,   252,     4,
      46,     5,     6,   144,    16,    26,   367,    47,   196,    48,
     440,   128,   315,    49,   207,   385,    50,   316,     4,    67,
       5,     6,   426,   242,   194,    51,    47,    67,    48,   446,
     281,    68,    49,   386,    47,    50,    48,   125,     0,     0,
      69,     0,     0,    50,    51,     0,    67,     5,     6,     0,
      97,     0,    51,    47,    67,    48,     0,     0,    68,    49,
       0,    47,    50,    48,     0,    67,     0,    91,     0,    68,
      50,    51,    47,     0,    48,    67,    67,    97,    69,    51,
      68,    50,    47,    47,    48,    48,    67,     0,    49,   334,
      51,    50,    50,    47,     0,    48,    67,     0,     0,   147,
      51,    51,    50,    47,     0,    48,     0,     0,     0,   232,
       0,    51,    50,     0,     0,     0,     0,     0,     0,     0,
       0,    51
};

static const yytype_int16 yycheck[] =
{
      23,     7,    46,    26,   155,    47,    11,    48,    31,     9,
      33,    97,    34,    11,    34,   191,    89,   119,    20,   116,
     277,     1,   424,    67,    20,    38,    39,   100,    13,   102,
      53,   117,    17,    18,    40,    41,    42,    17,    40,     9,
      40,    41,    42,    16,    65,    66,    46,    53,     9,     1,
      56,   206,    48,    53,    11,    35,    56,     9,    15,    65,
      66,   463,    37,    86,    21,    65,    66,   222,    68,   224,
      90,    13,    16,    48,    72,    48,    46,    11,   100,    17,
     100,    15,   105,    53,    18,    46,    56,    21,   111,    89,
      32,    48,    53,    98,    46,    56,    17,   194,    17,    18,
      98,    11,   125,    37,    48,    15,    27,    18,    18,   151,
      48,    21,   135,   155,    48,    13,    18,   214,   118,    17,
     377,    18,    17,   123,    18,    36,   228,    48,   106,    48,
     216,   217,   205,    20,    36,   158,   159,   234,    48,    36,
      38,   162,   163,   143,   167,   168,   169,   220,   245,    20,
     128,    48,    18,    40,   240,   330,   162,   163,   244,     1,
      18,    17,   185,    20,   170,     8,   189,   342,    11,    40,
      36,    37,   195,    16,   329,    17,    33,    20,    36,    37,
     356,    13,   204,    40,   204,    17,   341,     4,    20,    32,
      33,   191,     4,     5,     6,   218,    13,    29,    37,    11,
      17,    13,    17,    20,    47,    17,   206,   248,    20,    40,
     215,    17,    29,   364,    20,    20,    38,    29,    33,   242,
     218,    33,   222,   119,   224,     4,    48,    33,    34,     8,
      36,   275,    11,   330,    13,    41,    48,   133,    17,   336,
     337,    20,    48,    13,    20,   342,   343,    17,    27,    13,
      29,   348,   338,   339,     4,   255,    18,    33,   281,    29,
     357,    11,    17,    13,    38,    39,    45,    17,   274,   275,
      20,    34,    27,    32,   280,    38,   299,    16,    33,    29,
      34,    31,    21,    46,    38,   308,    47,   310,     3,     4,
     296,   297,    46,   316,    17,    45,    11,    13,    13,    13,
       4,    17,    17,    17,    27,    20,   403,    11,   405,    13,
      17,   334,   335,    17,    29,    13,    20,   414,   415,   416,
      27,   418,   364,    13,    39,    29,     3,     4,    18,   329,
      34,     8,    14,    15,    11,    17,    13,   360,   361,   362,
      17,   341,    32,    20,   430,   387,    36,    37,    17,    13,
      27,   351,    29,    17,     3,     4,   356,    47,    27,    13,
      33,   447,    11,    17,    13,   365,    20,    17,    17,    13,
     467,    20,    18,    17,    21,    29,   376,    13,   378,    17,
      29,    17,    20,   406,   407,   408,   409,    38,    39,     1,
     476,     3,     4,     5,     6,     7,     8,     9,    17,    11,
      12,    13,    21,    13,    38,    17,    18,    17,    20,    36,
      22,     4,    24,    25,    26,    27,    28,    29,    11,    31,
      13,     4,    18,    35,    17,     8,    15,    20,    11,    18,
      13,    14,    21,    45,    17,    16,    29,    20,   438,   439,
       3,    34,     5,     6,   444,     8,    29,    42,    43,    44,
       1,   474,     3,     4,     5,     6,     7,     8,     9,    13,
      11,    12,    13,    17,    48,    53,    17,    18,    56,    20,
      37,    22,    37,    24,    25,    26,    27,    28,    29,    37,
      31,    38,    39,    29,    35,     1,    36,     3,     4,     5,
       6,     7,     8,     9,    45,    11,    12,    13,    18,    18,
      18,    17,    18,    36,    20,    18,    22,    18,    24,    25,
      26,    27,    28,    29,    20,    31,   400,   401,   402,    35,
       1,    33,     3,     4,     5,     6,     7,     8,     9,    45,
      11,    12,    13,    32,    32,    20,    17,    18,    20,    20,
      17,    22,    21,    24,    25,    26,    27,    28,    29,    33,
      31,    38,    18,    33,    35,     0,     1,    32,     3,    17,
       5,     6,     7,     4,    45,    10,     3,    12,     5,     6,
      11,     8,    13,    17,     4,    15,    17,    46,    23,    20,
      16,    11,    32,    13,    17,    30,    21,    17,    29,    21,
      20,     4,    21,    34,    24,    25,    26,    17,    11,    29,
      13,    31,    19,    34,    17,    19,    17,    20,    46,    34,
      34,    24,    25,    26,     1,    34,    29,   365,    31,     3,
       4,     5,     6,    46,     1,     1,   255,    11,    77,    13,
     376,    32,   185,    17,    90,   288,    20,   185,     3,     4,
       5,     6,   351,   119,    72,    29,    11,     4,    13,   387,
     145,     8,    17,   288,    11,    20,    13,    31,    -1,    -1,
      17,    -1,    -1,    20,    29,    -1,     4,     5,     6,    -1,
      27,    -1,    29,    11,     4,    13,    -1,    -1,     8,    17,
      -1,    11,    20,    13,    -1,     4,    -1,    17,    -1,     8,
      20,    29,    11,    -1,    13,     4,     4,    27,    17,    29,
       8,    20,    11,    11,    13,    13,     4,    -1,    17,    17,
      29,    20,    20,    11,    -1,    13,     4,    -1,    -1,    17,
      29,    29,    20,    11,    -1,    13,    -1,    -1,    -1,    17,
      -1,    29,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    29
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    50,     0,     1,     3,     5,     6,     7,    10,    12,
      23,    30,    51,    52,    53,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    72,    73,    74,
      75,    76,    77,    78,    79,    83,    84,    86,    87,    88,
     143,   144,   145,   146,    17,   124,     4,    11,    13,    17,
      20,    29,    54,    55,    64,    67,   124,   127,   128,   129,
     130,   131,   132,   133,   140,   143,   144,     4,     8,    17,
      85,   109,   126,   127,    18,     1,    17,    90,    91,    92,
      93,    96,    18,    36,    17,    48,   139,   141,     1,     8,
       9,    17,    18,    22,    24,    25,    26,    27,    28,    29,
      31,    35,    45,    53,    57,    64,    97,    98,    99,   100,
     101,   102,   103,   105,   106,   107,   108,   109,   121,   125,
     126,    18,    36,    37,   141,   139,   141,    35,    97,    18,
      36,   141,   107,   125,    17,    89,   124,   127,   124,   127,
     124,   127,    40,    80,    54,    64,   131,    17,   130,    20,
      40,   134,    20,    13,    20,   134,    20,    33,    55,   124,
     127,   141,   143,   144,    13,    17,    29,   138,    55,   124,
      18,    47,    32,   123,    13,    13,    17,   124,   133,   124,
     133,   131,   127,    11,    15,    18,    21,   110,   111,   112,
     141,    33,   113,    17,    85,    35,    91,    18,    38,    21,
      36,    18,    16,   141,    31,    45,   121,   101,   107,    13,
      17,    37,    37,    37,   113,     3,    27,   109,   126,    33,
      45,   107,   121,    29,   121,   141,    35,    98,    38,   141,
     110,   113,    17,    34,   114,   115,   116,   117,   127,    17,
      27,    33,   102,   103,   104,   108,    36,    18,    17,    24,
      25,    26,    31,    68,    69,    70,    71,   127,   142,    18,
     141,    18,    35,    36,    18,    18,    18,    17,    20,    33,
      34,    36,    41,   141,     3,     4,    17,    39,    81,    82,
     127,   138,    20,    13,    17,     8,    11,    14,    17,   130,
     135,   136,   137,    13,    17,   135,    32,    32,   141,    16,
     141,    13,   124,   133,   124,   133,   141,   141,    16,   141,
     124,    32,    15,    18,    21,   111,   112,   141,   141,    34,
     115,   110,   141,    14,    15,    17,    94,    95,   107,   121,
     114,    20,    20,   110,    17,   126,   113,   113,    27,   109,
     141,   121,   114,   114,    17,   103,    37,   110,   118,   122,
      34,    38,    46,    21,    17,    33,    33,   113,    32,   141,
      16,    21,   113,   110,   134,    38,    17,    71,   127,    18,
      17,   124,   124,    17,     8,    66,    38,    39,    21,   124,
     141,    17,    16,    33,    47,   123,   137,    38,    39,    13,
      17,    39,   124,   124,   141,   141,   141,    15,   141,    34,
      42,    43,    44,   114,   110,   118,    17,    27,    17,    27,
     141,   141,   110,   110,   113,   113,   114,   110,   118,   110,
     110,    13,    17,    18,   119,   120,   116,    46,    16,    32,
      34,   110,    17,   141,   141,   141,   135,    69,    21,    21,
      82,     8,    66,   127,    21,    17,   136,    34,    95,    95,
      95,   110,   110,   141,   141,   141,   141,   110,   110,   110,
     110,    19,    19,    38,   120,    46,    17,   113,    34,    39,
     127,   127,   127,    34,   113,   120,    34,   110,   141,   113
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
        case 2:
#line 113 "mta_parse.y"
    { mta->ResetState(); mta->yy_state = MTA::YYRet_Exit; }
    break;

  case 3:
#line 114 "mta_parse.y"
    {
	    mta->ResetState();  return mta->yy_state; }
    break;

  case 4:
#line 116 "mta_parse.y"
    {
	    mta->ResetState(); return mta->yy_state; }
    break;

  case 5:
#line 118 "mta_parse.y"
    {
	    mta->ResetState(); return mta->yy_state; }
    break;

  case 6:
#line 120 "mta_parse.y"
    {
            mta->ResetState(); return mta->yy_state; }
    break;

  case 7:
#line 122 "mta_parse.y"
    {
            mta->ResetState(); return mta->yy_state; }
    break;

  case 8:
#line 124 "mta_parse.y"
    {
	    mta->ResetState(); return mta->yy_state; }
    break;

  case 9:
#line 126 "mta_parse.y"
    {
	    mta->ResetState(); return mta->yy_state; }
    break;

  case 10:
#line 128 "mta_parse.y"
    {
	    mta->ResetState(); return mta->yy_state; }
    break;

  case 11:
#line 130 "mta_parse.y"
    {
	    mta->ResetState(); return mta->yy_state; }
    break;

  case 12:
#line 135 "mta_parse.y"
    {
            mta->ResetState(); mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
    break;

  case 13:
#line 139 "mta_parse.y"
    {
  	  if((yyvsp[(1) - (1)].typ) != NULL) {
	    (yyval.typ) = mta->TypeAddUniqNameOld("typedef", (yyvsp[(1) - (1)].typ));
            // a typedef can never be literally a template or a template inst!
            if((yyval.typ)) { (yyval.typ)->ClearType(TypeDef::TEMPLATE);
              (yyval.typ)->ClearType(TypeDef::TEMPL_INST); } } }
    break;

  case 14:
#line 148 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 15:
#line 149 "mta_parse.y"
    {
	    if((yyvsp[(1) - (2)].typ) != NULL)  SETDESC((yyvsp[(1) - (2)].typ),(yyvsp[(2) - (2)].chr)); }
    break;

  case 16:
#line 153 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 17:
#line 154 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(2) - (4)].typ); (yyvsp[(2) - (4)].typ)->name = (yyvsp[(3) - (4)].chr); mta->type_stack.Pop(); }
    break;

  case 18:
#line 156 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 19:
#line 158 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(3) - (5)].typ); (yyvsp[(3) - (5)].typ)->name = (yyvsp[(4) - (5)].chr); mta->type_stack.Pop(); }
    break;

  case 20:
#line 162 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); if((yyvsp[(1) - (3)].typ)) { (yyvsp[(2) - (3)].typ)->AddParent((yyvsp[(1) - (3)].typ)); (yyvsp[(2) - (3)].typ)->AssignType((yyvsp[(1) - (3)].typ)->type); }
	    mta->type_stack.Pop(); }
    break;

  case 21:
#line 165 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (4)].typ); if((yyvsp[(1) - (4)].typ)) { (yyvsp[(2) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(2) - (4)].typ)->AssignType((yyvsp[(1) - (4)].typ)->type); }
	    mta->type_stack.Pop(); (yyval.typ)->SetType(TypeDef::ARRAY); }
    break;

  case 22:
#line 168 "mta_parse.y"
    {
            /* annoying place for a comment: prevents structype, so use plain type */
            (yyval.typ) = (yyvsp[(3) - (4)].typ); if((yyvsp[(1) - (4)].typ)) { (yyvsp[(3) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(3) - (4)].typ)->AssignType((yyvsp[(1) - (4)].typ)->type); }
	    mta->type_stack.Pop(); }
    break;

  case 23:
#line 174 "mta_parse.y"
    {
	  if((yyvsp[(1) - (2)].typ)->parents.size < 2) {
	    mta->Error(0, "Error in predeclared type:", (yyvsp[(1) - (2)].typ)->name,
                       "second parent not found!");
	    (yyval.typ) = (yyvsp[(1) - (2)].typ);
	  }
	  else {
	    TypeDef* td = (yyvsp[(1) - (2)].typ)->parents[1]; mta->type_stack.Pop();
	    (yyval.typ) = td; } }
    break;

  case 24:
#line 183 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (7)].typ); (yyval.typ)->AssignType(TypeDef::FUN_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 25:
#line 186 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (4)].typ); (yyval.typ)->AssignType(TypeDef::METH_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 26:
#line 189 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 27:
#line 191 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (4)].typ); (yyval.typ)->SetType(TypeDef::ARRAY); }
    break;

  case 28:
#line 193 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 29:
#line 195 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 32:
#line 203 "mta_parse.y"
    {
            (yyval.typ) = mta->TypeAddUniqNameOld("enum", (yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { (yyval.typ)->source_end = mta->line-1; } }
    break;

  case 35:
#line 212 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); }
    break;

  case 36:
#line 213 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 37:
#line 216 "mta_parse.y"
    {
  	    (yyval.typ) = (yyvsp[(2) - (2)].typ);
            /* mta->PushState(MTA::Parse_enum); -- MP_ENUM triggers in mta_lex.cpp */
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::ENUM); mta->cur_enum = (yyvsp[(2) - (2)].typ);
	    mta->SetSource((yyval.typ), false); mta->type_stack.Pop(); }
    break;

  case 38:
#line 221 "mta_parse.y"
    {
            /* mta->PushState(MTA::Parse_enum); -- MP_ENUM triggers in mta_lex.cpp */
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->cur_enum = (yyval.typ);
	    mta->SetSource((yyval.typ), false); (yyval.typ)->AssignType(TypeDef::ENUM); }
    break;

  case 39:
#line 229 "mta_parse.y"
    {
	    (yyval.typ) = mta->TypeAddUniqNameOld("class", (yyvsp[(1) - (1)].typ));
	    mta->type_stack.Pop(); }
    break;

  case 40:
#line 235 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 41:
#line 236 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 42:
#line 240 "mta_parse.y"
    {
	    (yyval.typ) = mta->TypeAddUniqNameOld("class", (yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->FixClassTypes((yyval.typ)); (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 45:
#line 252 "mta_parse.y"
    {
	    if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	    else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 46:
#line 255 "mta_parse.y"
    {
	    if((yyvsp[(1) - (2)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (2)].typ)->tokens.keep = false;
	    else (yyvsp[(1) - (2)].typ)->tokens.keep = true; }
    break;

  case 47:
#line 261 "mta_parse.y"
    {
	    (yyvsp[(1) - (2)].typ)->tokens.keep = true; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (2)].typ), true); }
    break;

  case 48:
#line 264 "mta_parse.y"
    {
            SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs();
            mta->SetSource((yyvsp[(1) - (3)].typ), true); }
    break;

  case 49:
#line 267 "mta_parse.y"
    {
            SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (3)].typ), true); 
	  }
    break;

  case 50:
#line 274 "mta_parse.y"
    { mta->PushState(MTA::Parse_inclass); }
    break;

  case 51:
#line 275 "mta_parse.y"
    { mta->PushState(MTA::Parse_inclass); }
    break;

  case 52:
#line 278 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::prvt);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::CLASS); mta->ClearSource((yyvsp[(2) - (2)].typ));
            /* tyname set -- premature */ }
    break;

  case 53:
#line 282 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::prvt); }
    break;

  case 54:
#line 284 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::CLASS); 
            mta->type_stack.Push((yyval.typ));
	    mta->PushClass((yyval.typ), MTA::prvt); }
    break;

  case 55:
#line 289 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::STRUCT);
            mta->type_stack.Push((yyval.typ));
	    mta->PushClass((yyval.typ), MTA::pblc); }
    break;

  case 56:
#line 294 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::pblc);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::STRUCT); mta->ClearSource((yyvsp[(2) - (2)].typ));
            /* tyname set -- premature */ }
    break;

  case 57:
#line 298 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::pblc); }
    break;

  case 58:
#line 300 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::UNION);
            mta->type_stack.Push((yyval.typ));
	    mta->PushClass((yyval.typ), MTA::pblc); }
    break;

  case 59:
#line 305 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::pblc);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::UNION); mta->ClearSource((yyvsp[(2) - (2)].typ));
            /* tyname set -- premature */ }
    break;

  case 60:
#line 309 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::pblc); }
    break;

  case 61:
#line 314 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].typ) != NULL) mta->cur_class->AddParent((yyvsp[(1) - (1)].typ)); }
    break;

  case 62:
#line 316 "mta_parse.y"
    {
            if((yyvsp[(3) - (3)].typ) != NULL) { mta->cur_class->AddParent((yyvsp[(3) - (3)].typ));
	      mta->cur_class->AddOption("MULT_INHERIT"); } }
    break;

  case 64:
#line 323 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 65:
#line 324 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 66:
#line 325 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 67:
#line 326 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 72:
#line 338 "mta_parse.y"
    {
	    (yyval.typ) = mta->TypeAddUniqNameOld("template", (yyvsp[(1) - (1)].typ));
	    mta->type_stack.Pop(); }
    break;

  case 73:
#line 344 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 74:
#line 345 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 75:
#line 349 "mta_parse.y"
    {
	    (yyval.typ) = mta->TypeAddUniqNameOld("template", (yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->FixClassTypes((yyval.typ)); (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 76:
#line 356 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 77:
#line 357 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 78:
#line 358 "mta_parse.y"
    { mta->Burp(); 
            String nwnm;  if((yyvsp[(2) - (2)].meth) != NULL) nwnm = (yyvsp[(2) - (2)].meth)->name;
            nwnm += "_templ_fun_" + String(taMisc::types.size);
	    TypeDef* tmpl = new TypeDef(nwnm);
	    (yyval.typ) = tmpl;
	    tmpl->AssignType(TypeDef::FUNCTION);
	    tmpl->SetType(TypeDef::TEMPLATE);
	    tmpl->AddOption("IGNORE"); /* bad news */
            if((yyvsp[(2) - (2)].meth) != NULL)
              tmpl->methods.AddUniqNameNew((yyvsp[(2) - (2)].meth));
	  }
    break;

  case 79:
#line 369 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 80:
#line 370 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (4)].typ),(yyvsp[(4) - (4)].chr)); }
    break;

  case 81:
#line 371 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr));
            String nwnm;  if((yyvsp[(2) - (3)].meth) != NULL) nwnm = (yyvsp[(2) - (3)].meth)->name;
            nwnm += "_templ_fun_" + String(taMisc::types.size);
	    TypeDef* tmpl = new TypeDef(nwnm);
	    (yyval.typ) = tmpl;
	    SETDESC((yyval.typ),(yyvsp[(3) - (3)].chr));
	    tmpl->AssignType(TypeDef::FUNCTION);
	    tmpl->SetType(TypeDef::TEMPLATE);
	    tmpl->AddOption("IGNORE"); /* bad news */
            if((yyvsp[(2) - (3)].meth) != NULL)
              tmpl->methods.AddUniqNameNew((yyvsp[(2) - (3)].meth));
          }
    break;

  case 82:
#line 386 "mta_parse.y"
    {
          if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	  else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 83:
#line 389 "mta_parse.y"
    {
          if((yyvsp[(1) - (2)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (2)].typ)->tokens.keep = false;
	  else (yyvsp[(1) - (2)].typ)->tokens.keep = true; }
    break;

  case 85:
#line 396 "mta_parse.y"
    {
	    if(mta->state != MTA::Parse_enum) /* could have triggered earlier -- need to keep */
	      mta->PushState(MTA::Parse_inclass); (yyvsp[(1) - (2)].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 86:
#line 400 "mta_parse.y"
    {
	    if(mta->state != MTA::Parse_enum) /* could have triggered earlier -- need to keep */
	      mta->PushState(MTA::Parse_inclass); SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->Class_ResetCurPtrs(); }
    break;

  case 87:
#line 403 "mta_parse.y"
    {
	    if(mta->state != MTA::Parse_enum) /* could have triggered earlier -- need to keep */
	      mta->PushState(MTA::Parse_inclass); SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->Class_ResetCurPtrs(); }
    break;

  case 88:
#line 409 "mta_parse.y"
    {
	    (yyvsp[(5) - (5)].typ)->templ_pars.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_pars.Duplicate(mta->cur_templ_pars);
	    (yyvsp[(5) - (5)].typ)->templ_defs.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_defs.Duplicate(mta->cur_templ_defs);
            mta->EndTemplPars();
	    (yyvsp[(5) - (5)].typ)->SetType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(5) - (5)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 89:
#line 417 "mta_parse.y"
    {
	    (yyvsp[(4) - (4)].typ)->templ_pars.Reset();
	    (yyvsp[(4) - (4)].typ)->templ_defs.Reset();
            mta->EndTemplPars();
	    (yyvsp[(4) - (4)].typ)->SetType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(4) - (4)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 90:
#line 426 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(1) - (4)].typ);
            mta->PushState(MTA::Parse_fundef);
            mta->EndTemplPars();
          }
    break;

  case 91:
#line 431 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(1) - (5)].typ);
            mta->PushState(MTA::Parse_fundef);
            mta->EndTemplPars();
          }
    break;

  case 92:
#line 437 "mta_parse.y"
    { mta->StartTemplPars(); }
    break;

  case 94:
#line 442 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 95:
#line 446 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 96:
#line 447 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 97:
#line 448 "mta_parse.y"
    {
          (yyval.typ) = new TypeDef("typename"); mta->cur_templ_pars.Add((yyval.typ)); }
    break;

  case 98:
#line 450 "mta_parse.y"
    {  /* $1 is probably an unrecognized type name.. */
          (yyval.typ) = new TypeDef((yyvsp[(2) - (2)].chr)); mta->cur_templ_pars.Add((yyval.typ)); }
    break;

  case 99:
#line 452 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(1) - (1)].typ)); (yyval.typ) = (yyvsp[(1) - (1)].typ); }
    break;

  case 100:
#line 453 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 101:
#line 456 "mta_parse.y"
    {
            mta->cur_templ_pars.Link((yyvsp[(2) - (4)].typ)); (yyval.typ) = (yyvsp[(2) - (4)].typ);
            mta->cur_templ_defs.Link((yyvsp[(4) - (4)].typ));
          }
    break;

  case 102:
#line 460 "mta_parse.y"
    {
            mta->cur_templ_pars.Link((yyvsp[(2) - (4)].typ)); (yyval.typ) = (yyvsp[(2) - (4)].typ);
            mta->cur_templ_defs.Link((yyvsp[(4) - (4)].typ));
          }
    break;

  case 103:
#line 464 "mta_parse.y"
    {
            mta->cur_templ_pars.Link((yyvsp[(2) - (4)].typ)); (yyval.typ) = (yyvsp[(2) - (4)].typ);
            mta->cur_templ_defs.Link((yyvsp[(4) - (4)].typ));
          }
    break;

  case 104:
#line 468 "mta_parse.y"
    {
            mta->cur_templ_pars.Link((yyvsp[(1) - (3)].typ)); (yyval.typ) = (yyvsp[(1) - (3)].typ);
            mta->cur_templ_defs.Link((yyvsp[(3) - (3)].typ));
          }
    break;

  case 105:
#line 474 "mta_parse.y"
    {
            if(mta->cur_is_trg) { /* only add reg_funs in target space */
              TypeDef* nt = new TypeDef((yyvsp[(1) - (1)].meth)->name, TypeDef::FUNCTION,0,0);
              mta->SetSource(nt, false);
              taMisc::types.Add(nt);
              mta->Info(5, "added reg fun to types:", nt->name);
              nt->methods.AddUniqNameNew((yyvsp[(1) - (1)].meth));
              taMisc::reg_funs.Link(nt); }
            mta->meth_stack.Pop(); }
    break;

  case 106:
#line 485 "mta_parse.y"
    {
            (yyvsp[(3) - (4)].meth)->type = (yyvsp[(2) - (4)].typ); SETDESC((yyvsp[(3) - (4)].meth),(yyvsp[(4) - (4)].chr)); (yyval.meth) = (yyvsp[(3) - (4)].meth); }
    break;

  case 107:
#line 487 "mta_parse.y"
    {
            (yyvsp[(2) - (3)].meth)->type = &TA_int; SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); (yyval.meth) = (yyvsp[(2) - (3)].meth); }
    break;

  case 108:
#line 491 "mta_parse.y"
    {
            (yyvsp[(1) - (2)].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = true; }
    break;

  case 109:
#line 496 "mta_parse.y"
    { /* using is not parsed */
            String nms = mta->cur_namespcs.AsString(".");
            mta->Namespc_PushNew(nms);
          }
    break;

  case 110:
#line 502 "mta_parse.y"
    {
            String nms = mta->cur_namespcs.AsString(".");
            mta->Namespc_PushNew(nms);
          }
    break;

  case 111:
#line 508 "mta_parse.y"
    {
          mta->cur_namespcs.Reset(); }
    break;

  case 112:
#line 512 "mta_parse.y"
    { mta->cur_namespcs.Add((yyvsp[(1) - (1)].chr)); }
    break;

  case 113:
#line 513 "mta_parse.y"
    { mta->cur_namespcs.Add((yyvsp[(2) - (2)].chr)); }
    break;

  case 114:
#line 514 "mta_parse.y"
    { mta->cur_namespcs.Add(String("::") + (yyvsp[(3) - (3)].chr)); }
    break;

  case 115:
#line 515 "mta_parse.y"
    { mta->cur_namespcs.Add(String("(")); }
    break;

  case 116:
#line 516 "mta_parse.y"
    { mta->cur_namespcs.Add(String(")")); }
    break;

  case 117:
#line 517 "mta_parse.y"
    { mta->cur_namespcs.Add(String("\"")); }
    break;

  case 121:
#line 525 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 124:
#line 530 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 125:
#line 535 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 126:
#line 538 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm));
            if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
            else (yyvsp[(1) - (3)].enm)->AddOption("#IGNORE"); // ignore bad math!
	    mta->enum_stack.Pop(); }
    break;

  case 128:
#line 547 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 129:
#line 550 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 130:
#line 553 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) | (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 131:
#line 559 "mta_parse.y"
    {
            EnumDef* ed = mta->cur_enum->enum_vals.FindName((yyvsp[(1) - (1)].chr));
            if(ed) (yyval.rval) = ed->enum_no;
            else   (yyval.rval) = -424242; }
    break;

  case 133:
#line 564 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 134:
#line 568 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 135:
#line 571 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 136:
#line 572 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 137:
#line 575 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].memb) != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->HasOption("IGNORE"))
		 && !((yyvsp[(1) - (1)].memb)->type->IsConst())) {
		mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
                mta->Info(3, "member:", (yyvsp[(1) - (1)].memb)->name, "added to class:",
                          mta->cur_class->name); } }
	    mta->memb_stack.Pop(); (yyval.typ) = NULL; }
    break;

  case 138:
#line 583 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].meth) != NULL) {
	      if(mta->cur_mstate == MTA::pblc) {
		if((yyvsp[(1) - (1)].meth)->HasOption("IGNORE"))
		  mta->cur_class->ignore_meths.AddUnique((yyvsp[(1) - (1)].meth)->name);
		else {
		  mta->cur_class->methods.AddUniqNameNew((yyvsp[(1) - (1)].meth));
                  mta->Info(3, "method:", (yyvsp[(1) - (1)].meth)->name, "added to class:",
                            mta->cur_class->name); } } }
	    else {
	      mta->cur_meth = NULL; }
	    mta->meth_stack.Pop(); (yyval.typ) = NULL; }
    break;

  case 139:
#line 595 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
            mta->TypeAdded("enum", &(mta->cur_class->sub_types), (yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->source_end = mta->line-1;
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 140:
#line 601 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 141:
#line 605 "mta_parse.y"
    { 
	    mta->PopClass();
            (yyvsp[(1) - (2)].typ)->SetType(TypeDef::SUBTYPE);
            (yyvsp[(1) - (2)].typ)->source_end = mta->line-1;
            mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (2)].typ));
            if(mta->cur_mstate != MTA::pblc) {
              (yyvsp[(1) - (2)].typ)->AddOption("IGNORE");
            }
            mta->TypeAdded("class subtype", &(mta->cur_class->sub_types), (yyvsp[(1) - (2)].typ));
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 142:
#line 615 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 144:
#line 620 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 145:
#line 621 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 146:
#line 622 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 147:
#line 623 "mta_parse.y"
    {
	    (yyval.memb) = NULL;
	    if(mta->last_memb != NULL) {
	      SETDESC(mta->last_memb, (yyvsp[(1) - (1)].chr));
	      if(mta->last_memb->HasOption("IGNORE"))
		mta->cur_class->members.RemoveEl(mta->last_memb); }
	    else if(mta->last_meth != NULL) {
	      SETDESC(mta->last_meth, (yyvsp[(1) - (1)].chr));
	      if(mta->last_meth->HasOption("IGNORE")) {
		mta->cur_class->ignore_meths.AddUnique(mta->last_meth->name);
		mta->cur_class->methods.RemoveEl(mta->last_meth);
		mta->last_meth = NULL; }
	      else if((mta->last_meth->opts.size > 0) || (mta->last_meth->lists.size > 0)) {
		mta->cur_class->methods.AddUniqNameNew(mta->last_meth);
                mta->Info(3, "method:", mta->last_meth->name, "added to class:",
                          mta->cur_class->name); } } }
    break;

  case 149:
#line 643 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 150:
#line 649 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 151:
#line 650 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 152:
#line 651 "mta_parse.y"
    { }
    break;

  case 153:
#line 652 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
            int spsz = sp->size;
            TypeDef* nty = (yyvsp[(1) - (4)].typ)->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    (yyvsp[(2) - (4)].memb)->type = nty; }
    break;

  case 154:
#line 659 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 155:
#line 663 "mta_parse.y"
    {
	     if(mta->cur_mstate == MTA::pblc)
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 156:
#line 667 "mta_parse.y"
    {
	     if(mta->cur_mstate == MTA::pblc)
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 157:
#line 673 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 158:
#line 679 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 160:
#line 685 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 161:
#line 686 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 162:
#line 687 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 163:
#line 688 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 164:
#line 689 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 165:
#line 690 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 166:
#line 691 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 167:
#line 692 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 168:
#line 693 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 169:
#line 694 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 170:
#line 695 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 171:
#line 696 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 172:
#line 697 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 173:
#line 698 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 174:
#line 699 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(5) - (5)].chr);
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef((yyvsp[(3) - (5)].meth)->name, TypeDef::FUNCTION,0,0);
              taMisc::types.Add(nt);
              mta->Info(5, "added reg fun to types:", nt->name);
	      nt->methods.AddUniqNameNew((yyvsp[(3) - (5)].meth)); (yyvsp[(3) - (5)].meth)->type = (yyvsp[(2) - (5)].typ);
	      mta->meth_stack.Pop();  (yyvsp[(3) - (5)].meth)->fun_argc = (yyvsp[(4) - (5)].rval); (yyvsp[(3) - (5)].meth)->arg_types.size = (yyvsp[(4) - (5)].rval);
	      (yyvsp[(3) - (5)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(3) - (5)].meth),(yyvsp[(5) - (5)].chr)); }
	    else { (yyval.meth) = NULL; mta->meth_stack.Pop(); } }
    break;

  case 175:
#line 710 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(4) - (4)].chr);
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef((yyvsp[(2) - (4)].meth)->name, TypeDef::FUNCTION,0,0);
              taMisc::types.Add(nt);
              mta->Info(5, "added reg fun to types:", nt->name);
	      nt->methods.AddUniqNameNew((yyvsp[(2) - (4)].meth)); (yyvsp[(2) - (4)].meth)->type = &TA_int;
	      mta->meth_stack.Pop();  (yyvsp[(2) - (4)].meth)->fun_argc = (yyvsp[(3) - (4)].rval); (yyvsp[(2) - (4)].meth)->arg_types.size = (yyvsp[(3) - (4)].rval);
	      (yyvsp[(2) - (4)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(2) - (4)].meth),(yyvsp[(4) - (4)].chr)); }
	    else { (yyval.meth) = 0; mta->meth_stack.Pop(); } }
    break;

  case 176:
#line 721 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 177:
#line 722 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 178:
#line 723 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 179:
#line 724 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 181:
#line 729 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 182:
#line 730 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 183:
#line 732 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 184:
#line 737 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 185:
#line 738 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
    break;

  case 186:
#line 739 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 187:
#line 740 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 188:
#line 741 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 189:
#line 744 "mta_parse.y"
    {
             (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 190:
#line 751 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 191:
#line 755 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 192:
#line 756 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
    break;

  case 193:
#line 757 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 194:
#line 758 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 195:
#line 759 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 196:
#line 760 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 197:
#line 764 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 198:
#line 765 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 199:
#line 766 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
    break;

  case 200:
#line 770 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 201:
#line 771 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 202:
#line 772 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 203:
#line 775 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 204:
#line 776 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 205:
#line 779 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 206:
#line 780 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 207:
#line 783 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 208:
#line 784 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 209:
#line 785 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 210:
#line 788 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 211:
#line 790 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      String ad = (yyvsp[(2) - (2)].chr);
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 212:
#line 801 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 213:
#line 805 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 214:
#line 809 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 215:
#line 813 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 216:
#line 817 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 224:
#line 838 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 225:
#line 842 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 226:
#line 845 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 227:
#line 846 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 228:
#line 849 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
                                  mta->SetSource((yyval.typ), false); }
    break;

  case 229:
#line 853 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 231:
#line 858 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 232:
#line 859 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 234:
#line 863 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ)->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, (yyval.typ)); }
	  }
    break;

  case 236:
#line 873 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ);
 	    for(int i=0; i<(yyvsp[(2) - (2)].rval); i++) {
              (yyval.typ) = (yyval.typ)->GetPtrType_impl(*sp);
              if(sp->size != spsz) { mta->TypeAdded("ptr", sp, (yyval.typ)); }
            }
	  }
    break;

  case 238:
#line 885 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(2) - (2)].typ)->GetConstType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("const", sp, (yyval.typ)); }
	  }
    break;

  case 240:
#line 894 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (4)].typ); }
    break;

  case 242:
#line 898 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
	    (yyval.typ) = td; }
    break;

  case 243:
#line 905 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
            (yyval.typ) = td; }
    break;

  case 244:
#line 912 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 245:
#line 913 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 246:
#line 914 "mta_parse.y"
    {
            TypeDef* nty = new TypeDef((yyvsp[(2) - (2)].chr)); mta->SetSource(nty, false);
	    TypeSpace* sp = mta->GetTypeSpace(nty);
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("namespace new", sp, (yyval.typ));
              (yyval.typ)->namespc = mta->cur_nmspc_tmp; }
	    else { mta->TypeNotAdded("namespace new", sp, (yyval.typ), nty); delete nty; }
          }
    break;

  case 247:
#line 922 "mta_parse.y"
    {
            mta->Info(2, "namespace type:", mta->cur_nmspc_tmp, "::", (yyvsp[(2) - (2)].typ)->name);
            (yyvsp[(2) - (2)].typ)->namespc = mta->cur_nmspc_tmp; /* todo: could check.. */
            (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 248:
#line 926 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 250:
#line 928 "mta_parse.y"
    { /* a template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
              mta->Warning(1, "Type:", (yyvsp[(1) - (4)].typ)->name, "used as template but not marked as such",
                           "-- now marking -- probably just an internal type");
              (yyvsp[(1) - (4)].typ)->SetType(TypeDef::TEMPLATE); }
            String nm = (yyvsp[(1) - (4)].typ)->GetTemplInstName(mta->cur_typ_templ_pars);
            TypeDef* td;
            int lx_tok;
            if((td = mta->FindName(nm, lx_tok)) == NULL) {
              td = (yyvsp[(1) - (4)].typ)->Clone(); td->name = nm;
              td->SetTemplType((yyvsp[(1) - (4)].typ), mta->cur_typ_templ_pars);
              TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
              (yyval.typ) = mta->TypeAddUniqNameOld("template instance", td, sp); }
            else
              (yyval.typ) = td; }
    break;

  case 251:
#line 943 "mta_parse.y"
    { /* this template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
              mta->Warning(1, "Type:", (yyvsp[(1) - (4)].typ)->name, "used as template but not marked as such",
                           "-- now marking -- probably just an internal type");
              (yyvsp[(1) - (4)].typ)->SetType(TypeDef::TEMPLATE); }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 252:
#line 951 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::STRUCT);
          (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 253:
#line 953 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::STRUCT); }
    break;

  case 254:
#line 954 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::UNION);
            (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 255:
#line 956 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::UNION); }
    break;

  case 257:
#line 960 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (2)].typ)->name + "_" + (yyvsp[(2) - (2)].typ)->name;
	    TypeDef* nty = new TypeDef((char*)nm);
            nty->AssignType((yyvsp[(1) - (2)].typ)->type); // get from first guy
            nty->SetType((yyvsp[(2) - (2)].typ)->type);   // add from second
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
	    (yyval.typ) = mta->TypeAddUniqNameOld("combo", nty, sp);
	    if((yyval.typ) == nty) { nty->size = (yyvsp[(2) - (2)].typ)->size; nty->AddParent((yyvsp[(1) - (2)].typ)); nty->AddParent((yyvsp[(2) - (2)].typ)); }
	    else { mta->TypeNotAdded("combo", sp, (yyval.typ), nty); delete nty; }
	  }
    break;

  case 258:
#line 972 "mta_parse.y"
    {
           mta->cur_typ_templ_pars.Reset(); }
    break;

  case 260:
#line 978 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 261:
#line 983 "mta_parse.y"
    {
            mta->cur_typ_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 262:
#line 985 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 263:
#line 987 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 264:
#line 989 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 265:
#line 991 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 266:
#line 993 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(3) - (4)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 267:
#line 995 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval));
            mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 268:
#line 998 "mta_parse.y"
    {  
          (yyval.typ) = new TypeDef((yyvsp[(2) - (2)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 269:
#line 1000 "mta_parse.y"
    {  
          (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); }
    break;

  case 270:
#line 1002 "mta_parse.y"
    {  
          (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 274:
#line 1012 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 275:
#line 1013 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 278:
#line 1020 "mta_parse.y"
    { mta->cur_nmspc_tmp = (yyvsp[(1) - (2)].chr); (yyval.chr) = mta->cur_nmspc_tmp; }
    break;

  case 283:
#line 1032 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 284:
#line 1039 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 285:
#line 1046 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 286:
#line 1053 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 3520 "y.tab.c"
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


#line 1056 "mta_parse.y"


	/* end of grammar */

void yyerror(const char *s) { 	/* called for yacc syntax error */
  bool trg = mta->VerboseCheckTrg();
  int v_level = trg ? 0 : 1;    /* use 0 for targets and 1 for else */
  if(strcmp(s, "parse error") == 0) {
    mta->Error(v_level, "Syntax Error, line:", String(mta->st_line), ":");
  }
  else {
    mta->Error(v_level, s, "line:", String(mta->st_line), ":");
  }
  mta->Error(v_level, mta->LastLn);
  String loc;
  for(int i=0; i < mta->st_col; i++)
    loc << " ";
  loc << "^";
  mta->Error(v_level, loc);
}

