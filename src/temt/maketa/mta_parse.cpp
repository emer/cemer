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
#define YYLAST   795

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  49
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  98
/* YYNRULES -- Number of rules.  */
#define YYNRULES  289
/* YYNRULES -- Number of states.  */
#define YYNSTATES  483

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
     327,   330,   335,   340,   345,   349,   351,   353,   358,   362,
     365,   369,   373,   375,   377,   380,   384,   387,   390,   393,
     395,   398,   400,   403,   405,   408,   410,   412,   416,   418,
     422,   426,   430,   432,   434,   436,   438,   440,   443,   445,
     447,   449,   451,   454,   456,   458,   461,   464,   467,   469,
     471,   474,   478,   483,   486,   491,   496,   498,   502,   504,
     509,   511,   515,   520,   525,   530,   536,   542,   547,   553,
     559,   565,   570,   574,   579,   584,   590,   595,   601,   607,
     613,   619,   621,   624,   627,   631,   635,   638,   643,   650,
     654,   657,   659,   661,   663,   666,   669,   672,   676,   678,
     681,   685,   687,   689,   692,   695,   699,   701,   704,   706,
     710,   715,   717,   720,   722,   724,   727,   731,   738,   740,
     743,   745,   747,   750,   754,   757,   760,   763,   765,   767,
     770,   772,   774,   776,   778,   781,   783,   786,   788,   791,
     793,   796,   798,   803,   805,   809,   813,   817,   821,   824,
     827,   830,   832,   837,   842,   845,   848,   851,   854,   856,
     859,   861,   863,   867,   869,   871,   874,   877,   880,   885,
     887,   890,   893,   896,   898,   900,   902,   904,   906,   908,
     910,   913,   916,   918,   920,   922,   924,   926,   928,   930
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
     127,   124,    21,   127,    -1,   127,    21,   127,    -1,     1,
      -1,    84,    -1,    30,   126,    85,   110,    -1,    30,    85,
     110,    -1,   109,   113,    -1,    88,    89,   141,    -1,    88,
      89,    36,    -1,    23,    -1,    17,    -1,    89,    17,    -1,
      89,    20,    17,    -1,    89,    33,    -1,    89,    34,    -1,
      89,    41,    -1,    91,    -1,    90,    91,    -1,    92,    -1,
      92,    18,    -1,    93,    -1,    93,    38,    -1,     1,    -1,
      96,    -1,    96,    21,    94,    -1,    95,    -1,    94,    42,
      95,    -1,    94,    43,    95,    -1,    94,    44,    95,    -1,
      17,    -1,    14,    -1,    15,    -1,    17,    -1,    98,    -1,
      97,    98,    -1,    99,    -1,   105,    -1,    57,    -1,    53,
      -1,    64,   141,    -1,     1,    -1,   100,    -1,    24,    37,
      -1,    25,    37,    -1,    26,    37,    -1,    18,    -1,   101,
      -1,     9,   101,    -1,   125,   102,   141,    -1,   125,   103,
      21,   141,    -1,   102,   141,    -1,   125,   103,    16,   141,
      -1,   125,   104,   113,   141,    -1,   103,    -1,   102,    38,
     103,    -1,    17,    -1,    33,    32,    17,    34,    -1,   106,
      -1,   121,   114,   110,    -1,     8,   121,   114,   110,    -1,
      31,   121,   114,   110,    -1,   121,   114,   118,   110,    -1,
       8,   121,   114,   118,   110,    -1,    31,   121,   114,   118,
     110,    -1,    45,   121,   114,   110,    -1,     8,    45,   121,
     114,   110,    -1,    31,    45,   121,   114,   110,    -1,    28,
     126,    27,   113,   110,    -1,    28,    27,   113,   110,    -1,
      28,   126,   141,    -1,    28,     3,   126,   141,    -1,    28,
       3,    17,   141,    -1,    28,   126,   109,   113,   110,    -1,
      28,   109,   113,   110,    -1,    22,    17,    20,    27,   141,
      -1,    22,    17,    20,    17,   141,    -1,    22,    13,    20,
      27,   141,    -1,    22,    13,    20,    17,   141,    -1,   107,
      -1,     9,   107,    -1,    31,   107,    -1,     8,    31,   107,
      -1,   125,   108,   110,    -1,   108,   110,    -1,   125,    27,
     113,   110,    -1,   125,    27,    33,    34,   113,   110,    -1,
      27,   113,   110,    -1,   109,   113,    -1,    17,    -1,   141,
      -1,   111,    -1,   112,   141,    -1,    18,   141,    -1,    18,
     111,    -1,    18,   112,   141,    -1,    15,    -1,    11,    15,
      -1,    11,    18,    15,    -1,    21,    -1,    11,    -1,    11,
      21,    -1,    33,    34,    -1,    33,   115,    34,    -1,    34,
      -1,   115,    34,    -1,   116,    -1,   115,    38,   116,    -1,
     115,    46,    46,    46,    -1,   117,    -1,   117,    21,    -1,
       1,    -1,   127,    -1,   127,    17,    -1,   127,    17,    16,
      -1,   127,    33,    32,    17,    34,   113,    -1,    17,    -1,
     122,   119,    -1,   120,    -1,    18,    -1,   119,   120,    -1,
     119,    38,   120,    -1,    13,    19,    -1,    17,    19,    -1,
      29,    33,    -1,    37,    -1,    32,    -1,   123,    32,    -1,
      17,    -1,   126,    -1,   127,    -1,     8,    -1,     8,   127,
      -1,   128,    -1,   128,    47,    -1,   129,    -1,   129,   123,
      -1,   130,    -1,    11,   130,    -1,   131,    -1,     4,   131,
      20,    17,    -1,   133,    -1,    13,    20,    17,    -1,    29,
      20,    17,    -1,    13,    20,    13,    -1,    29,    20,    13,
      -1,   140,    17,    -1,   140,    13,    -1,    20,    13,    -1,
      29,    -1,    13,   134,   135,    39,    -1,    29,   134,   135,
      39,    -1,   143,   133,    -1,   143,   124,    -1,   144,   133,
      -1,   144,   124,    -1,    13,    -1,   133,    13,    -1,    40,
      -1,   136,    -1,   135,    38,   136,    -1,   130,    -1,    17,
      -1,    17,   123,    -1,    17,    47,    -1,    17,    16,    -1,
      17,    33,    17,    34,    -1,    14,    -1,   137,    17,    -1,
     137,    13,    -1,    17,   137,    -1,     1,    -1,    11,    -1,
       8,    -1,    17,    -1,    13,    -1,    29,    -1,    17,    -1,
      17,    16,    -1,    17,    20,    -1,    48,    -1,    24,    -1,
      25,    -1,    26,    -1,     5,    -1,     6,    -1,     3,    -1,
      10,    -1
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
     453,   456,   460,   464,   468,   472,   475,   486,   488,   492,
     497,   503,   509,   513,   514,   515,   516,   517,   518,   521,
     522,   525,   526,   529,   530,   531,   536,   539,   547,   548,
     551,   554,   560,   564,   565,   569,   572,   573,   576,   584,
     596,   602,   606,   616,   620,   621,   622,   623,   624,   643,
     644,   650,   651,   652,   653,   660,   664,   668,   674,   680,
     685,   686,   687,   688,   689,   690,   691,   692,   693,   694,
     695,   696,   697,   698,   699,   700,   711,   722,   723,   724,
     725,   729,   730,   731,   733,   738,   739,   740,   741,   742,
     745,   752,   756,   757,   758,   759,   760,   761,   765,   766,
     767,   771,   772,   773,   776,   777,   780,   781,   784,   785,
     786,   789,   791,   799,   803,   807,   811,   815,   819,   825,
     829,   830,   831,   832,   835,   836,   840,   844,   847,   848,
     851,   855,   859,   860,   861,   864,   865,   874,   875,   886,
     887,   895,   896,   899,   900,   907,   914,   915,   916,   924,
     928,   929,   930,   945,   953,   955,   956,   958,   961,   962,
     974,   979,   980,   985,   987,   989,   991,   993,   995,   997,
    1000,  1002,  1004,  1006,  1010,  1011,  1014,  1015,  1016,  1019,
    1020,  1023,  1026,  1029,  1030,  1031,  1035,  1042,  1049,  1056
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
      82,    82,    82,    82,    82,    82,    83,    84,    84,    85,
      86,    87,    88,    89,    89,    89,    89,    89,    89,    90,
      90,    91,    91,    92,    92,    92,    93,    93,    94,    94,
      94,    94,    95,    95,    95,    96,    97,    97,    98,    98,
      98,    98,    98,    98,    99,    99,    99,    99,    99,   100,
     100,   101,   101,   101,   101,   101,   102,   102,   103,   104,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   106,   106,   106,   106,   107,   107,   107,   107,   107,
     108,   109,   110,   110,   110,   110,   110,   110,   111,   111,
     111,   112,   112,   112,   113,   113,   114,   114,   115,   115,
     115,   116,   116,   116,   117,   117,   117,   117,   117,   118,
     119,   119,   119,   119,   120,   120,   121,   122,   123,   123,
     124,   125,   126,   126,   126,   127,   127,   128,   128,   129,
     129,   130,   130,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   132,   132,   132,   132,   133,   133,
     134,   135,   135,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   137,   137,   138,   138,   138,   139,
     139,   140,   141,   142,   142,   142,   143,   144,   145,   146
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
       2,     4,     4,     4,     3,     1,     1,     4,     3,     2,
       3,     3,     1,     1,     2,     3,     2,     2,     2,     1,
       2,     1,     2,     1,     2,     1,     1,     3,     1,     3,
       3,     3,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     2,     1,     1,     2,     2,     2,     1,     1,
       2,     3,     4,     2,     4,     4,     1,     3,     1,     4,
       1,     3,     4,     4,     4,     5,     5,     4,     5,     5,
       5,     4,     3,     4,     4,     5,     4,     5,     5,     5,
       5,     1,     2,     2,     3,     3,     2,     4,     6,     3,
       2,     1,     1,     1,     2,     2,     2,     3,     1,     2,
       3,     1,     1,     2,     2,     3,     1,     2,     1,     3,
       4,     1,     2,     1,     1,     2,     3,     6,     1,     2,
       1,     1,     2,     3,     2,     2,     2,     1,     1,     2,
       1,     1,     1,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     4,     1,     3,     3,     3,     3,     2,     2,
       2,     1,     4,     4,     2,     2,     2,     2,     1,     2,
       1,     1,     3,     1,     1,     2,     2,     2,     4,     1,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     1,    12,   288,   286,   287,    38,   289,     0,
     112,     0,     3,    13,    14,     4,    32,     0,     0,     5,
      39,     6,    42,     0,     0,     0,    50,     7,    72,     8,
      75,     0,     0,     0,     0,     9,   106,    10,    11,     0,
      55,    58,    54,     0,   230,    37,     0,     0,   258,   230,
       0,   251,    16,     0,     0,    50,     0,    30,   235,   237,
     239,   241,    31,   243,     0,    55,    58,     0,   233,   191,
       0,     0,     0,   232,    15,   125,   135,     0,   119,   121,
     123,   126,     0,    34,   279,   282,     0,    43,   143,   233,
       0,   158,   148,     0,     0,     0,     0,     0,     0,   251,
       0,    46,     0,   141,   140,     0,     0,   136,   138,   144,
     149,     0,   156,   139,   160,   181,     0,     0,     0,     0,
     231,     0,    47,     0,    40,     0,    76,    83,     0,     0,
      85,    73,    78,     0,   113,     0,    56,    57,    59,    60,
      52,    53,    92,     0,    18,     0,   241,     0,   240,     0,
     260,     0,   281,   250,     0,     0,     0,     0,     0,     0,
      30,    23,     0,     0,   277,   276,   278,     0,     0,     0,
       0,   236,   228,   238,   259,   249,   248,   255,   254,   257,
     256,     0,   234,   202,   198,     0,   201,   108,   193,     0,
     192,     0,   109,   191,     0,     0,   120,   122,   124,     0,
      35,    36,   280,    44,     0,     0,     0,   150,   182,     0,
       0,   145,   146,   147,     0,     0,     0,     0,     0,   226,
       0,   183,     0,     0,     0,   142,    45,   137,     0,   153,
     186,   190,   213,   218,   206,     0,     0,   208,   211,   214,
     158,     0,     0,     0,   156,     0,     0,    48,    49,    65,
     283,   284,   285,    71,    51,    61,     0,    68,    63,    70,
      41,    77,    79,    82,    86,    87,    74,    81,   114,     0,
     116,   117,   111,   118,   110,   105,     0,    97,     0,     0,
       0,    93,    99,     0,     0,   246,   244,   273,   275,   274,
     269,   264,   263,     0,   261,     0,   247,   245,     0,     0,
       0,    29,     0,    20,   258,   255,   254,   257,   256,    17,
      28,     0,    26,     0,   229,   199,     0,   203,   196,     0,
     195,   194,   204,     0,   107,    33,   133,   134,   132,   127,
     128,   184,     0,     0,     0,     0,   189,     0,     0,     0,
       0,     0,     0,   172,     0,     0,     0,   158,   157,   227,
     161,     0,     0,   207,     0,     0,   212,   215,     0,     0,
       0,     0,   151,     0,     0,     0,   185,     0,     0,    67,
      69,    64,    80,   115,    95,    96,    98,    90,    89,     0,
       0,     0,   100,    19,   242,   267,     0,   266,   265,   272,
       0,   252,   271,   270,   253,    25,     0,    21,    27,    22,
     200,   197,   205,     0,     0,     0,     0,   162,     0,     0,
       0,     0,     0,   174,   173,   171,   176,     0,     0,     0,
     163,     0,   167,   164,     0,     0,   221,   219,   220,   209,
       0,   216,     0,   204,   187,     0,   154,   152,   155,     0,
      62,     0,     0,    94,    91,    88,   104,     0,     0,   262,
       0,   129,   130,   131,   168,   165,   180,   179,   178,   177,
     170,   175,   169,   166,   224,   225,     0,   222,   210,     0,
       0,   159,    66,   101,   102,   103,   268,     0,   223,     0,
     188,    24,   217
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    12,    13,   103,    52,    53,    15,   104,    17,
      18,    19,    20,    21,    22,   105,    24,    25,    55,   254,
     255,   256,   257,    27,    28,    29,    30,    31,    32,    33,
      34,   143,   280,   281,    35,    36,    70,    37,    38,    39,
     135,    77,    78,    79,    80,   329,   330,    81,   106,   107,
     108,   109,   110,   111,   112,   245,   113,   114,   115,   116,
     117,   187,   188,   189,   192,   235,   236,   237,   238,   351,
     427,   428,   118,   352,   173,    56,   119,   120,    73,    58,
      59,    60,    61,    62,    63,   155,   293,   294,   295,   167,
      86,    64,   190,   259,    40,    41,    42,    43
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -405
static const yytype_int16 yypact[] =
{
    -405,   589,  -405,  -405,  -405,  -405,  -405,    55,  -405,   661,
    -405,   737,  -405,  -405,   161,  -405,  -405,   233,   181,  -405,
    -405,  -405,  -405,    19,   393,   209,     7,  -405,  -405,  -405,
    -405,    19,   452,   116,   699,  -405,  -405,  -405,  -405,   214,
     747,   747,   747,   158,  -405,  -405,   680,   766,    87,   256,
     253,   129,  -405,   154,   160,   249,   718,   280,   257,   281,
    -405,  -405,  -405,   311,    32,   747,   747,   220,   758,   256,
      83,   293,   317,  -405,  -405,  -405,  -405,   112,  -405,   329,
     325,   359,   347,   366,   361,  -405,   355,  -405,  -405,   260,
     726,   241,  -405,   246,   372,   375,   377,   293,   691,   130,
     201,  -405,   387,  -405,  -405,   355,   487,  -405,  -405,  -405,
    -405,    67,  -405,  -405,  -405,  -405,    83,   293,   356,   177,
    -405,   399,   411,   419,   413,   355,   422,  -405,   522,   401,
     428,     4,   431,   124,  -405,   234,  -405,  -405,  -405,  -405,
    -405,  -405,  -405,   326,  -405,   160,   432,   256,  -405,   299,
    -405,   596,  -405,  -405,   339,   596,    96,   430,   355,     3,
    -405,  -405,   357,   357,  -405,  -405,  -405,   355,   355,    27,
      55,  -405,  -405,   434,  -405,  -405,  -405,   164,   179,   207,
     179,   432,  -405,   310,  -405,   175,  -405,  -405,  -405,   355,
    -405,   551,  -405,  -405,    83,   355,  -405,  -405,  -405,   270,
    -405,  -405,  -405,  -405,   699,   387,   356,  -405,  -405,   447,
     448,  -405,  -405,  -405,    83,   748,   293,   293,    60,  -405,
     387,  -405,   356,   418,   356,  -405,  -405,  -405,   454,  -405,
    -405,  -405,  -405,   256,  -405,    82,   263,  -405,   461,   222,
     440,   442,   453,    67,   262,   293,    83,  -405,  -405,   136,
    -405,  -405,  -405,  -405,   446,  -405,   632,  -405,  -405,  -405,
    -405,   468,  -405,  -405,  -405,  -405,  -405,  -405,  -405,   472,
    -405,  -405,  -405,  -405,  -405,  -405,    55,   252,   212,   330,
      40,  -405,   341,   355,   484,  -405,  -405,  -405,  -405,  -405,
    -405,   106,  -405,   254,  -405,   358,  -405,  -405,   369,    55,
      55,  -405,   355,  -405,  -405,  -405,   311,  -405,   311,  -405,
    -405,   355,  -405,   355,  -405,  -405,   488,  -405,  -405,   355,
    -405,  -405,  -405,   265,  -405,  -405,  -405,  -405,  -405,   349,
    -405,  -405,   356,    82,   151,   176,  -405,     9,   355,    83,
      83,   293,   293,  -405,   356,    82,    83,  -405,  -405,  -405,
    -405,    83,   289,  -405,   622,   456,  -405,   490,   476,   559,
      83,   493,  -405,   355,   355,   355,  -405,   596,   419,   256,
    -405,  -405,  -405,  -405,   496,   498,  -405,  -405,  -405,   617,
     345,   758,   499,  -405,  -405,  -405,   504,  -405,   434,  -405,
     596,  -405,  -405,  -405,  -405,  -405,   502,  -405,  -405,  -405,
    -405,  -405,  -405,   270,   270,   270,    83,  -405,    83,   355,
     355,   355,   355,  -405,  -405,  -405,  -405,    83,    83,    83,
    -405,    83,  -405,  -405,   505,   518,  -405,    73,  -405,  -405,
     492,  -405,   524,   293,  -405,   509,  -405,  -405,  -405,   395,
    -405,   758,   758,  -405,  -405,  -405,  -405,   758,   511,  -405,
     293,  -405,  -405,  -405,  -405,  -405,  -405,  -405,  -405,  -405,
    -405,  -405,  -405,  -405,  -405,  -405,   365,  -405,  -405,   520,
      83,  -405,  -405,  -405,  -405,  -405,  -405,   355,  -405,   293,
    -405,  -405,  -405
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -405,  -405,  -405,  -405,   555,   512,   308,  -405,   558,  -405,
    -405,  -405,  -405,  -405,  -405,    30,  -405,  -259,   560,  -405,
     197,  -405,   313,  -405,  -405,  -405,  -405,  -405,  -405,  -405,
    -405,  -405,  -405,   187,  -405,  -405,   501,  -405,  -405,  -405,
    -405,  -405,   497,  -405,  -405,  -405,    22,  -405,   543,   -59,
    -405,  -405,   491,   458,  -108,  -405,  -405,  -405,   -20,   119,
       1,   -98,   397,   398,   -84,  -190,  -184,   224,  -405,  -295,
    -405,  -404,   -65,  -405,   295,    18,   -19,    -7,     0,  -405,
    -405,   -45,   -41,  -405,   -38,   -42,  -154,   194,   296,   457,
     567,  -405,   -23,  -405,     8,    39,  -405,  -405
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -244
static const yytype_int16 yytable[] =
{
      87,   298,   148,   124,    72,   146,   151,   323,   126,    57,
     131,   244,    71,   214,   132,   133,   333,    65,   230,   302,
     378,   -84,   266,   467,   206,    45,   181,   178,   180,   152,
     161,    23,   345,   231,   346,   222,    84,   224,   408,    54,
     137,   139,   141,   311,   123,   175,    57,   227,    66,   176,
     421,    85,   -84,   160,    65,    85,   160,    85,   136,   138,
     140,   162,   478,   203,   162,   137,   139,    85,   182,   227,
     208,   159,    44,    71,   169,    85,   145,   193,   379,   380,
     221,   133,   225,   177,   179,    66,   424,   341,   229,   182,
     425,   218,   163,   183,   183,   163,   324,   184,   184,   217,
     185,   185,   261,   186,   186,   228,   292,   149,    85,   153,
     292,   466,   274,    75,   288,    85,   336,   289,   239,   349,
     348,   445,   385,   258,   306,   308,   152,   150,   299,    76,
      85,    85,   339,   340,   129,   301,   303,   350,   172,   386,
     332,   193,   406,   282,   309,   310,   312,   195,   366,   154,
     154,   241,   130,   387,   419,   344,   152,   360,    67,     5,
       6,   365,   320,   219,    85,    47,   321,    48,   409,   150,
     150,    49,   325,   164,   156,   323,   150,   165,   410,    74,
     305,   307,   -56,    51,   331,   133,   183,   157,   313,   166,
     184,   239,   174,   411,   240,   343,   186,  -243,   142,    82,
     -56,   -56,    85,   412,   241,    67,   239,   367,   338,    68,
     242,  -243,    47,   439,    48,  -243,  -243,    83,    69,   342,
     362,    50,   239,    85,   239,   -59,  -243,   121,    97,   376,
      99,   134,   152,    48,    75,   407,   181,   147,   246,   357,
      50,   415,   416,   -59,   -59,   122,   220,   420,   422,    51,
      76,   268,   246,   423,   269,   358,   371,   417,   418,   209,
     383,   152,   434,   210,    67,    48,   153,   270,   271,    49,
     272,    47,    50,    48,  -191,   273,   152,   147,   363,   397,
      50,    51,    85,   364,   326,   327,   123,   328,   398,    99,
     399,   204,   390,   391,   374,   375,   401,   353,   170,   402,
     382,   354,   424,   354,   171,   205,   425,   426,   454,   355,
     455,   355,   285,   172,   413,   414,   286,   395,   396,   460,
     461,   462,   292,   463,   174,   315,   191,   275,   316,   276,
     277,   317,   239,     4,   193,     5,     6,    47,   377,    48,
     436,   437,   438,   278,   239,   292,    50,   197,     4,   470,
       5,     6,   296,   444,   239,    51,   297,   232,    44,   239,
      67,   158,   381,   198,   168,   279,   477,    47,   258,    48,
     304,   392,   480,   233,    44,   393,    50,   202,   424,   282,
     199,   446,   425,   200,   201,    51,   456,   457,   458,   459,
     234,   403,   404,   405,    88,   482,     4,    67,     5,     6,
       7,    89,    90,    85,    47,     9,    48,   390,   394,   211,
      91,    92,   212,    50,   213,    93,   223,    94,    95,    96,
      97,    98,    99,    67,   100,   451,   452,   453,   101,   248,
      47,   260,    48,   390,   472,   247,   249,   264,   102,    50,
     262,   473,   474,   250,   251,   252,   265,   475,    51,   267,
     253,   219,   284,    88,   481,     4,    67,     5,     6,     7,
      89,    90,   300,    47,     9,    48,   314,   334,   335,    91,
      92,   347,    50,  -191,    93,   359,    94,    95,    96,    97,
      98,    99,   356,   100,   368,   361,   372,   127,    88,   373,
       4,    67,     5,     6,     7,    89,    90,   102,    47,     9,
      48,   384,   430,   400,    91,    92,   431,    50,   432,    93,
     435,    94,    95,    96,    97,    98,    99,   441,   100,   442,
     447,   448,   226,    88,   464,     4,    67,     5,     6,     7,
      89,    90,   102,    47,     9,    48,   450,   465,   468,    91,
      92,   469,    50,   471,    93,   476,    94,    95,    96,    97,
      98,    99,   232,   100,   479,    67,    14,   263,   144,    16,
     232,    26,    47,    67,    48,   440,   443,   102,   233,   370,
      47,    50,    48,   194,   196,   128,   233,   243,   429,    50,
      51,   207,   318,   319,   449,   322,   388,   389,    51,     2,
       3,     0,     4,   433,     5,     6,     7,   287,   125,     8,
      67,     9,   283,     0,   288,     0,     0,   289,     0,    48,
     290,     0,    10,   291,     0,     0,    50,     0,   275,    11,
     276,   277,     0,   232,     0,    51,    67,     0,    47,     0,
      48,     0,     0,    47,   278,    48,    67,    50,     0,   233,
       0,     0,    50,    47,     0,    48,    51,     0,     0,   369,
       0,    51,    50,     0,     0,     0,   250,   251,   252,     0,
       0,    51,     0,   253,     4,    46,     5,     6,     0,     0,
       0,     0,    47,     0,    48,     0,     0,     0,    49,     0,
       0,    50,     0,     4,    67,     5,     6,     0,     0,     0,
      51,    47,     0,    48,   215,    67,     0,    49,     0,    68,
      50,     0,    47,    67,    48,     0,     0,    68,    69,    51,
      47,    50,    48,     0,     0,     0,    69,     0,   216,    50,
      51,     0,    67,     5,     6,     0,    97,     0,    51,    47,
      67,    48,     0,     0,    68,    49,     0,    47,    50,    48,
       0,    67,     0,    91,     0,    68,    50,    51,    47,     0,
      48,    67,    67,    97,    69,    51,    68,    50,    47,    47,
      48,    48,    67,     0,    49,   337,    51,    50,    50,    47,
      67,    48,     0,     0,     0,   147,    51,    51,    50,    48,
       0,     0,     0,   147,     0,     0,    50,    51,     0,     0,
       0,     0,     0,     0,     0,    51
};

static const yytype_int16 yycheck[] =
{
      23,   155,    47,    26,    11,    46,    48,   191,    31,     9,
      33,   119,    11,    97,    34,    34,   206,     9,   116,    16,
     279,    17,    18,   427,    89,     7,    67,    65,    66,    20,
      53,     1,   222,   117,   224,   100,    17,   102,   333,     9,
      40,    41,    42,    16,    37,    13,    46,   106,     9,    17,
     345,    48,    48,    53,    46,    48,    56,    48,    40,    41,
      42,    53,   466,    86,    56,    65,    66,    48,    68,   128,
      90,    53,    17,    72,    56,    48,    46,    17,    38,    39,
     100,   100,   105,    65,    66,    46,    13,    27,   111,    89,
      17,    98,    53,    11,    11,    56,   194,    15,    15,    98,
      18,    18,   125,    21,    21,    38,   151,    20,    48,    13,
     155,    38,   135,     1,     8,    48,   214,    11,   118,    37,
     228,   380,    16,   123,   162,   163,    20,    40,    32,    17,
      48,    48,   216,   217,    18,   158,   159,   235,    32,    33,
     205,    17,   332,   143,   167,   168,   169,    35,   246,    20,
      20,    27,    36,    47,   344,   220,    20,   241,     4,     5,
       6,   245,   185,    33,    48,    11,   189,    13,    17,    40,
      40,    17,   195,    13,    20,   359,    40,    17,    27,    18,
     162,   163,    18,    29,   204,   204,    11,    33,   170,    29,
      15,   191,    13,    17,    17,   218,    21,    18,    40,    18,
      36,    37,    48,    27,    27,     4,   206,   249,   215,     8,
      33,    32,    11,   367,    13,    36,    37,    36,    17,   218,
     243,    20,   222,    48,   224,    18,    47,    18,    27,    17,
      29,    17,    20,    13,     1,   333,   277,    17,   119,    17,
      20,   339,   340,    36,    37,    36,    45,   345,   346,    29,
      17,    17,   133,   351,    20,    33,   256,   341,   342,    13,
     283,    20,   360,    17,     4,    13,    13,    33,    34,    17,
      36,    11,    20,    13,    33,    41,    20,    17,    16,   302,
      20,    29,    48,    21,    14,    15,    37,    17,   311,    29,
     313,    31,    38,    39,   276,   277,   319,    34,    18,    34,
     282,    38,    13,    38,    47,    45,    17,    18,   406,    46,
     408,    46,    13,    32,   337,   338,    17,   299,   300,   417,
     418,   419,   367,   421,    13,    15,    33,     1,    18,     3,
       4,    21,   332,     3,    17,     5,     6,    11,     8,    13,
     363,   364,   365,    17,   344,   390,    20,    18,     3,   433,
       5,     6,    13,     8,   354,    29,    17,     1,    17,   359,
       4,    53,    21,    38,    56,    39,   450,    11,   368,    13,
      13,    13,   470,    17,    17,    17,    20,    16,    13,   379,
      21,   381,    17,    36,    18,    29,   409,   410,   411,   412,
      34,    42,    43,    44,     1,   479,     3,     4,     5,     6,
       7,     8,     9,    48,    11,    12,    13,    38,    39,    37,
      17,    18,    37,    20,    37,    22,    29,    24,    25,    26,
      27,    28,    29,     4,    31,   403,   404,   405,    35,    18,
      11,    18,    13,    38,    39,    36,    17,    36,    45,    20,
      18,   441,   442,    24,    25,    26,    18,   447,    29,    18,
      31,    33,    20,     1,   477,     3,     4,     5,     6,     7,
       8,     9,    32,    11,    12,    13,    32,    20,    20,    17,
      18,    17,    20,    33,    22,    33,    24,    25,    26,    27,
      28,    29,    21,    31,    38,    32,    18,    35,     1,    17,
       3,     4,     5,     6,     7,     8,     9,    45,    11,    12,
      13,    17,    46,    15,    17,    18,    16,    20,    32,    22,
      17,    24,    25,    26,    27,    28,    29,    21,    31,    21,
      21,    17,    35,     1,    19,     3,     4,     5,     6,     7,
       8,     9,    45,    11,    12,    13,    34,    19,    46,    17,
      18,    17,    20,    34,    22,    34,    24,    25,    26,    27,
      28,    29,     1,    31,    34,     4,     1,    35,    46,     1,
       1,     1,    11,     4,    13,   368,   379,    45,    17,   256,
      11,    20,    13,    72,    77,    32,    17,   119,   354,    20,
      29,    90,   185,   185,   390,    34,   291,   291,    29,     0,
       1,    -1,     3,    34,     5,     6,     7,     1,    31,    10,
       4,    12,   145,    -1,     8,    -1,    -1,    11,    -1,    13,
      14,    -1,    23,    17,    -1,    -1,    20,    -1,     1,    30,
       3,     4,    -1,     1,    -1,    29,     4,    -1,    11,    -1,
      13,    -1,    -1,    11,    17,    13,     4,    20,    -1,    17,
      -1,    -1,    20,    11,    -1,    13,    29,    -1,    -1,    17,
      -1,    29,    20,    -1,    -1,    -1,    24,    25,    26,    -1,
      -1,    29,    -1,    31,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    11,    -1,    13,    -1,    -1,    -1,    17,    -1,
      -1,    20,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      29,    11,    -1,    13,     3,     4,    -1,    17,    -1,     8,
      20,    -1,    11,     4,    13,    -1,    -1,     8,    17,    29,
      11,    20,    13,    -1,    -1,    -1,    17,    -1,    27,    20,
      29,    -1,     4,     5,     6,    -1,    27,    -1,    29,    11,
       4,    13,    -1,    -1,     8,    17,    -1,    11,    20,    13,
      -1,     4,    -1,    17,    -1,     8,    20,    29,    11,    -1,
      13,     4,     4,    27,    17,    29,     8,    20,    11,    11,
      13,    13,     4,    -1,    17,    17,    29,    20,    20,    11,
       4,    13,    -1,    -1,    -1,    17,    29,    29,    20,    13,
      -1,    -1,    -1,    17,    -1,    -1,    20,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    29
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
     110,   113,     1,    17,    34,   114,   115,   116,   117,   127,
      17,    27,    33,   102,   103,   104,   108,    36,    18,    17,
      24,    25,    26,    31,    68,    69,    70,    71,   127,   142,
      18,   141,    18,    35,    36,    18,    18,    18,    17,    20,
      33,    34,    36,    41,   141,     1,     3,     4,    17,    39,
      81,    82,   127,   138,    20,    13,    17,     1,     8,    11,
      14,    17,   130,   135,   136,   137,    13,    17,   135,    32,
      32,   141,    16,   141,    13,   124,   133,   124,   133,   141,
     141,    16,   141,   124,    32,    15,    18,    21,   111,   112,
     141,   141,    34,   115,   110,   141,    14,    15,    17,    94,
      95,   107,   121,   114,    20,    20,   110,    17,   126,   113,
     113,    27,   109,   141,   121,   114,   114,    17,   103,    37,
     110,   118,   122,    34,    38,    46,    21,    17,    33,    33,
     113,    32,   141,    16,    21,   113,   110,   134,    38,    17,
      71,   127,    18,    17,   124,   124,    17,     8,    66,    38,
      39,    21,   124,   141,    17,    16,    33,    47,   123,   137,
      38,    39,    13,    17,    39,   124,   124,   141,   141,   141,
      15,   141,    34,    42,    43,    44,   114,   110,   118,    17,
      27,    17,    27,   141,   141,   110,   110,   113,   113,   114,
     110,   118,   110,   110,    13,    17,    18,   119,   120,   116,
      46,    16,    32,    34,   110,    17,   141,   141,   141,   135,
      69,    21,    21,    82,     8,    66,   127,    21,    17,   136,
      34,    95,    95,    95,   110,   110,   141,   141,   141,   141,
     110,   110,   110,   110,    19,    19,    38,   120,    46,    17,
     113,    34,    39,   127,   127,   127,    34,   113,   120,    34,
     110,   141,   113
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
#line 472 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 106:
#line 475 "mta_parse.y"
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

  case 107:
#line 486 "mta_parse.y"
    {
            (yyvsp[(3) - (4)].meth)->type = (yyvsp[(2) - (4)].typ); SETDESC((yyvsp[(3) - (4)].meth),(yyvsp[(4) - (4)].chr)); (yyval.meth) = (yyvsp[(3) - (4)].meth); }
    break;

  case 108:
#line 488 "mta_parse.y"
    {
            (yyvsp[(2) - (3)].meth)->type = &TA_int; SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); (yyval.meth) = (yyvsp[(2) - (3)].meth); }
    break;

  case 109:
#line 492 "mta_parse.y"
    {
            (yyvsp[(1) - (2)].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = true; }
    break;

  case 110:
#line 497 "mta_parse.y"
    { /* using is not parsed */
            String nms = mta->cur_namespcs.AsString(".");
            mta->Namespc_PushNew(nms);
          }
    break;

  case 111:
#line 503 "mta_parse.y"
    {
            String nms = mta->cur_namespcs.AsString(".");
            mta->Namespc_PushNew(nms);
          }
    break;

  case 112:
#line 509 "mta_parse.y"
    {
          mta->cur_namespcs.Reset(); }
    break;

  case 113:
#line 513 "mta_parse.y"
    { mta->cur_namespcs.Add((yyvsp[(1) - (1)].chr)); }
    break;

  case 114:
#line 514 "mta_parse.y"
    { mta->cur_namespcs.Add((yyvsp[(2) - (2)].chr)); }
    break;

  case 115:
#line 515 "mta_parse.y"
    { mta->cur_namespcs.Add(String("::") + (yyvsp[(3) - (3)].chr)); }
    break;

  case 116:
#line 516 "mta_parse.y"
    { mta->cur_namespcs.Add(String("(")); }
    break;

  case 117:
#line 517 "mta_parse.y"
    { mta->cur_namespcs.Add(String(")")); }
    break;

  case 118:
#line 518 "mta_parse.y"
    { mta->cur_namespcs.Add(String("\"")); }
    break;

  case 122:
#line 526 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 125:
#line 531 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 126:
#line 536 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 127:
#line 539 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm));
            if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
            else (yyvsp[(1) - (3)].enm)->AddOption("#IGNORE"); // ignore bad math!
	    mta->enum_stack.Pop(); }
    break;

  case 129:
#line 548 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 130:
#line 551 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 131:
#line 554 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) | (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 132:
#line 560 "mta_parse.y"
    {
            EnumDef* ed = mta->cur_enum->enum_vals.FindName((yyvsp[(1) - (1)].chr));
            if(ed) (yyval.rval) = ed->enum_no;
            else   (yyval.rval) = -424242; }
    break;

  case 134:
#line 565 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 135:
#line 569 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 136:
#line 572 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 137:
#line 573 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 138:
#line 576 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].memb) != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->HasOption("IGNORE"))
		 && !((yyvsp[(1) - (1)].memb)->type->IsConst())) {
		mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
                mta->Info(3, "member:", (yyvsp[(1) - (1)].memb)->name, "added to class:",
                          mta->cur_class->name); } }
	    mta->memb_stack.Pop(); (yyval.typ) = NULL; }
    break;

  case 139:
#line 584 "mta_parse.y"
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

  case 140:
#line 596 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
            mta->TypeAdded("enum", &(mta->cur_class->sub_types), (yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->source_end = mta->line-1;
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 141:
#line 602 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 142:
#line 606 "mta_parse.y"
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

  case 143:
#line 616 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 145:
#line 621 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 146:
#line 622 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 147:
#line 623 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 148:
#line 624 "mta_parse.y"
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

  case 150:
#line 644 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 151:
#line 650 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 152:
#line 651 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 153:
#line 652 "mta_parse.y"
    { }
    break;

  case 154:
#line 653 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
            int spsz = sp->size;
            TypeDef* nty = (yyvsp[(1) - (4)].typ)->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    (yyvsp[(2) - (4)].memb)->type = nty; }
    break;

  case 155:
#line 660 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 156:
#line 664 "mta_parse.y"
    {
	     if(mta->cur_mstate == MTA::pblc)
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 157:
#line 668 "mta_parse.y"
    {
	     if(mta->cur_mstate == MTA::pblc)
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 158:
#line 674 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 159:
#line 680 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
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
    { (yyval.meth) = NULL; mta->thisname = false; }
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
    { (yyval.meth) = NULL; }
    break;

  case 175:
#line 700 "mta_parse.y"
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

  case 176:
#line 711 "mta_parse.y"
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

  case 180:
#line 725 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 182:
#line 730 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 183:
#line 731 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 184:
#line 733 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 185:
#line 738 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 186:
#line 739 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
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
#line 742 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 190:
#line 745 "mta_parse.y"
    {
             (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 191:
#line 752 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 192:
#line 756 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 193:
#line 757 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
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
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 197:
#line 761 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 198:
#line 765 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 199:
#line 766 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 200:
#line 767 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
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
#line 773 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 204:
#line 776 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 205:
#line 777 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 206:
#line 780 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 207:
#line 781 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 208:
#line 784 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 209:
#line 785 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 210:
#line 786 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 211:
#line 789 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 212:
#line 791 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      String ad = (yyvsp[(2) - (2)].chr);
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 213:
#line 799 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 214:
#line 803 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 215:
#line 807 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 216:
#line 811 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 217:
#line 815 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 218:
#line 819 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 226:
#line 840 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 227:
#line 844 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 228:
#line 847 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 229:
#line 848 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 230:
#line 851 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
                                  mta->SetSource((yyval.typ), false); }
    break;

  case 231:
#line 855 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 233:
#line 860 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 234:
#line 861 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 236:
#line 865 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ)->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, (yyval.typ)); }
	  }
    break;

  case 238:
#line 875 "mta_parse.y"
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

  case 240:
#line 887 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(2) - (2)].typ)->GetConstType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("const", sp, (yyval.typ)); }
	  }
    break;

  case 242:
#line 896 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (4)].typ); }
    break;

  case 244:
#line 900 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
	    (yyval.typ) = td; }
    break;

  case 245:
#line 907 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
            (yyval.typ) = td; }
    break;

  case 246:
#line 914 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 247:
#line 915 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 248:
#line 916 "mta_parse.y"
    {
            TypeDef* nty = new TypeDef((yyvsp[(2) - (2)].chr)); mta->SetSource(nty, false);
	    TypeSpace* sp = mta->GetTypeSpace(nty);
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("namespace new", sp, (yyval.typ));
              (yyval.typ)->namespc = mta->cur_nmspc_tmp; }
	    else { mta->TypeNotAdded("namespace new", sp, (yyval.typ), nty); delete nty; }
          }
    break;

  case 249:
#line 924 "mta_parse.y"
    {
            mta->Info(2, "namespace type:", mta->cur_nmspc_tmp, "::", (yyvsp[(2) - (2)].typ)->name);
            (yyvsp[(2) - (2)].typ)->namespc = mta->cur_nmspc_tmp; /* todo: could check.. */
            (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 250:
#line 928 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 252:
#line 930 "mta_parse.y"
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

  case 253:
#line 945 "mta_parse.y"
    { /* this template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
              mta->Warning(1, "Type:", (yyvsp[(1) - (4)].typ)->name, "used as template but not marked as such",
                           "-- now marking -- probably just an internal type");
              (yyvsp[(1) - (4)].typ)->SetType(TypeDef::TEMPLATE); }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 254:
#line 953 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::STRUCT);
          (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 255:
#line 955 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::STRUCT); }
    break;

  case 256:
#line 956 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::UNION);
            (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 257:
#line 958 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::UNION); }
    break;

  case 259:
#line 962 "mta_parse.y"
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

  case 260:
#line 974 "mta_parse.y"
    {
           mta->cur_typ_templ_pars.Reset(); }
    break;

  case 262:
#line 980 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 263:
#line 985 "mta_parse.y"
    {
            mta->cur_typ_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 264:
#line 987 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 265:
#line 989 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 266:
#line 991 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 267:
#line 993 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 268:
#line 995 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(3) - (4)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 269:
#line 997 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval));
            mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 270:
#line 1000 "mta_parse.y"
    {  
          (yyval.typ) = new TypeDef((yyvsp[(2) - (2)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 271:
#line 1002 "mta_parse.y"
    {  
          (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); }
    break;

  case 272:
#line 1004 "mta_parse.y"
    {  
          (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 273:
#line 1006 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 277:
#line 1015 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 278:
#line 1016 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 281:
#line 1023 "mta_parse.y"
    { mta->cur_nmspc_tmp = (yyvsp[(1) - (2)].chr); (yyval.chr) = mta->cur_nmspc_tmp; }
    break;

  case 286:
#line 1035 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 287:
#line 1042 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 288:
#line 1049 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 289:
#line 1056 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 3549 "y.tab.c"
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


#line 1059 "mta_parse.y"


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

