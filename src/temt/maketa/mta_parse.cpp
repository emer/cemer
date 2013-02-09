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
#define YYLAST   570

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  86
/* YYNRULES -- Number of rules.  */
#define YYNRULES  218
/* YYNRULES -- Number of states.  */
#define YYNSTATES  368

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
       2,     2,     2,     2,     2,     2,     2,     2,    46,     2,
      33,    35,    32,    42,    38,    43,    45,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    41,
      40,     2,    39,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    36,     2,    34,    44,     2,     2,     2,
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
      58,    61,    69,    74,    76,    81,    84,    88,    92,    95,
      97,    99,   102,   106,   108,   111,   115,   119,   122,   126,
     130,   132,   136,   139,   142,   144,   146,   148,   150,   154,
     156,   159,   161,   164,   166,   169,   171,   173,   175,   178,
     182,   186,   189,   193,   197,   203,   205,   207,   211,   214,
     217,   220,   224,   226,   231,   235,   238,   243,   247,   249,
     252,   254,   257,   259,   262,   264,   266,   270,   272,   276,
     280,   282,   284,   286,   288,   291,   293,   295,   297,   299,
     302,   304,   306,   309,   312,   315,   317,   319,   322,   326,
     331,   334,   339,   344,   346,   350,   352,   357,   359,   363,
     368,   373,   378,   383,   389,   395,   401,   406,   410,   415,
     420,   426,   431,   433,   436,   439,   443,   447,   450,   455,
     462,   466,   469,   471,   473,   475,   478,   481,   484,   488,
     490,   493,   497,   499,   501,   504,   507,   511,   513,   516,
     518,   522,   527,   529,   532,   534,   537,   541,   548,   550,
     552,   555,   559,   562,   564,   566,   569,   571,   573,   575,
     578,   580,   582,   585,   587,   590,   592,   595,   597,   600,
     603,   606,   609,   613,   617,   621,   625,   628,   630,   635,
     640,   642,   645,   647,   651,   653,   655,   657,   659,   661,
     663,   666,   668,   670,   672,   674,   676,   678,   680
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      48,     0,    -1,    -1,    48,    49,    -1,    48,    53,    -1,
      48,    57,    -1,    48,    59,    -1,    48,    69,    -1,    48,
      77,    -1,    48,    80,    -1,    48,    81,    -1,    48,    34,
      -1,    48,     1,    -1,    50,    -1,    51,    -1,    51,    18,
      -1,    12,    52,    -1,    12,    61,   125,   127,    -1,   118,
     117,   127,    -1,   118,    18,   117,   127,    -1,   118,   127,
      -1,   118,    33,    32,   117,    35,   105,   127,    -1,   118,
      20,    32,   117,    -1,    54,    -1,    55,    82,    34,   127,
      -1,    56,    36,    -1,    56,    18,    36,    -1,    56,    36,
      18,    -1,     7,   117,    -1,     7,    -1,    58,    -1,    64,
     127,    -1,    64,   127,    18,    -1,    60,    -1,    61,   127,
      -1,    61,   126,   127,    -1,    62,    89,    34,    -1,    63,
      36,    -1,    63,    18,    36,    -1,    63,    36,    18,    -1,
      64,    -1,    64,    37,    65,    -1,   131,   117,    -1,   131,
      13,    -1,   131,    -1,   129,    -1,   130,    -1,    66,    -1,
      65,    38,    66,    -1,   118,    -1,    67,   118,    -1,    17,
      -1,    67,    17,    -1,    68,    -1,    67,    68,    -1,   128,
      -1,    31,    -1,    70,    -1,    71,   127,    -1,    71,   126,
     127,    -1,    72,    89,    34,    -1,    73,    36,    -1,    73,
      18,    36,    -1,    73,    36,    18,    -1,   132,    74,    75,
      39,    63,    -1,    40,    -1,    76,    -1,    76,    38,    76,
      -1,     3,   117,    -1,     4,   117,    -1,   118,   117,    -1,
     118,   117,    21,    -1,    78,    -1,    30,   116,    79,   102,
      -1,    30,    79,   102,    -1,   101,   105,    -1,    22,    23,
      17,    41,    -1,    23,    17,    36,    -1,    83,    -1,    82,
      83,    -1,    84,    -1,    84,    18,    -1,    85,    -1,    85,
      38,    -1,     1,    -1,    88,    -1,    88,    21,    86,    -1,
      87,    -1,    86,    42,    87,    -1,    86,    43,    87,    -1,
      17,    -1,    14,    -1,    17,    -1,    90,    -1,    89,    90,
      -1,    91,    -1,    97,    -1,    54,    -1,    51,    -1,    61,
     127,    -1,     1,    -1,    92,    -1,    24,    37,    -1,    25,
      37,    -1,    26,    37,    -1,    18,    -1,    93,    -1,     9,
      93,    -1,   115,    94,   127,    -1,   115,    95,    21,   127,
      -1,    94,   127,    -1,   115,    95,    16,   127,    -1,   115,
      96,   105,   127,    -1,    95,    -1,    94,    38,    95,    -1,
      17,    -1,    33,    32,    17,    35,    -1,    98,    -1,   112,
     106,   102,    -1,     8,   112,   106,   102,    -1,    31,   112,
     106,   102,    -1,   112,   106,   110,   102,    -1,    44,   112,
     106,   102,    -1,     8,    44,   112,   106,   102,    -1,    31,
      44,   112,   106,   102,    -1,    28,   116,    27,   105,   102,
      -1,    28,    27,   105,   102,    -1,    28,   116,   127,    -1,
      28,     3,   116,   127,    -1,    28,     3,    17,   127,    -1,
      28,   116,   101,   105,   102,    -1,    28,   101,   105,   102,
      -1,    99,    -1,     9,    99,    -1,    31,    99,    -1,     8,
      31,    99,    -1,   115,   100,   102,    -1,   100,   102,    -1,
     115,    27,   105,   102,    -1,   115,    27,    33,    35,   105,
     102,    -1,    27,   105,   102,    -1,   101,   105,    -1,    17,
      -1,   127,    -1,   103,    -1,   104,   127,    -1,    18,   127,
      -1,    18,   103,    -1,    18,   104,   127,    -1,    15,    -1,
      11,    15,    -1,    11,    18,    15,    -1,    21,    -1,    11,
      -1,    11,    21,    -1,    33,    35,    -1,    33,   107,    35,
      -1,    35,    -1,   107,    35,    -1,   108,    -1,   107,    38,
     108,    -1,   107,    45,    45,    45,    -1,   109,    -1,   109,
      21,    -1,   118,    -1,   118,    17,    -1,   118,    17,    16,
      -1,   118,    33,    32,    17,    35,   105,    -1,    17,    -1,
     111,    -1,   110,   111,    -1,   113,    13,    19,    -1,    29,
      33,    -1,    37,    -1,    32,    -1,   114,    32,    -1,   116,
      -1,   118,    -1,     8,    -1,     8,   118,    -1,    17,    -1,
     119,    -1,   119,    46,    -1,   120,    -1,   120,   114,    -1,
     121,    -1,    11,   121,    -1,   122,    -1,   129,   122,    -1,
     129,   117,    -1,   130,   122,    -1,   130,   117,    -1,    13,
      20,    17,    -1,    29,    20,    17,    -1,    13,    20,    13,
      -1,    29,    20,    13,    -1,    20,    13,    -1,    29,    -1,
      13,    74,   123,    39,    -1,    29,    74,   123,    39,    -1,
      13,    -1,   122,    13,    -1,   124,    -1,   123,    38,   124,
      -1,    13,    -1,    17,    -1,    14,    -1,    17,    -1,    13,
      -1,    17,    -1,    17,    16,    -1,    41,    -1,    24,    -1,
      25,    -1,    26,    -1,     5,    -1,     6,    -1,     3,    -1,
      10,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   112,   112,   113,   116,   118,   121,   124,   126,   128,
     130,   132,   135,   139,   150,   151,   155,   156,   160,   163,
     168,   177,   180,   185,   191,   194,   195,   196,   199,   203,
     210,   218,   219,   223,   232,   233,   237,   243,   246,   249,
     256,   257,   260,   265,   269,   276,   283,   293,   295,   302,
     303,   304,   305,   308,   309,   312,   313,   317,   327,   328,
     332,   338,   341,   343,   348,   355,   359,   360,   364,   365,
     366,   367,   370,   380,   382,   386,   391,   396,   401,   402,
     405,   406,   409,   410,   411,   416,   419,   425,   426,   431,
     439,   440,   444,   447,   448,   451,   460,   473,   479,   483,
     488,   492,   493,   494,   495,   496,   516,   517,   523,   524,
     525,   526,   533,   537,   541,   547,   553,   558,   559,   560,
     561,   562,   563,   564,   565,   566,   567,   568,   569,   570,
     571,   581,   594,   595,   596,   598,   603,   604,   605,   606,
     607,   610,   617,   621,   622,   623,   624,   625,   626,   630,
     631,   632,   636,   637,   638,   641,   642,   645,   646,   649,
     650,   651,   654,   656,   667,   671,   675,   679,   683,   690,
     691,   694,   698,   702,   705,   706,   709,   712,   713,   714,
     717,   721,   722,   731,   732,   743,   744,   752,   753,   755,
     756,   758,   759,   763,   767,   768,   769,   770,   771,   788,
     794,   795,   809,   810,   815,   816,   817,   821,   822,   825,
     826,   829,   832,   833,   834,   838,   842,   846,   850
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
  "MP_VIRTUAL", "'*'", "'('", "'}'", "')'", "'{'", "':'", "','", "'>'",
  "'<'", "';'", "'+'", "'-'", "'~'", "'.'", "'&'", "$accept", "list",
  "typedefn", "typedefns", "typedsub", "defn", "enumdefn", "enumdsub",
  "enumname", "enumnm", "classdecl", "classdecls", "classdefn",
  "classdefns", "classdsub", "classname", "classhead", "classnm",
  "classinh", "classpar", "classptyp", "classpmod", "templdefn",
  "templdefns", "templdsub", "templname", "templhead", "templopen",
  "templpars", "templpar", "fundecl", "funnm", "regfundefn", "usenamespc",
  "namespc", "enums", "enumline", "enumitm", "enumitms", "enummath",
  "enummathitm", "enmitmname", "membs", "membline", "membdefn",
  "basicmemb", "nostatmemb", "membnames", "membname", "membfunp",
  "methdefn", "basicmeth", "nostatmeth", "mbfundefn", "methname",
  "fundefn", "funsubdefn", "funsubdecl", "funargs", "constfun", "args",
  "argdefn", "subargdefn", "constrlist", "constref", "consthsnm",
  "constcoln", "ptrs", "membtype", "ftype", "tyname", "type", "noreftype",
  "constype", "subtype", "combtype", "templargs", "templarg", "tdname",
  "varname", "term", "access", "structkeyword", "structkeyword",
  "classkeyword", "templatekeyword", 0
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
     285,   286,    42,    40,   125,    41,   123,    58,    44,    62,
      60,    59,    43,    45,   126,    46,    38
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    47,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    49,    50,    50,    51,    51,    52,    52,
      52,    52,    52,    53,    54,    55,    55,    55,    56,    56,
      57,    58,    58,    59,    60,    60,    61,    62,    62,    62,
      63,    63,    64,    64,    64,    64,    64,    65,    65,    66,
      66,    66,    66,    67,    67,    68,    68,    69,    70,    70,
      71,    72,    72,    72,    73,    74,    75,    75,    76,    76,
      76,    76,    77,    78,    78,    79,    80,    81,    82,    82,
      83,    83,    84,    84,    84,    85,    85,    86,    86,    86,
      87,    87,    88,    89,    89,    90,    90,    90,    90,    90,
      90,    91,    91,    91,    91,    91,    92,    92,    93,    93,
      93,    93,    93,    94,    94,    95,    96,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    98,    98,    98,    98,    99,    99,    99,    99,
      99,   100,   101,   102,   102,   102,   102,   102,   102,   103,
     103,   103,   104,   104,   104,   105,   105,   106,   106,   107,
     107,   107,   108,   108,   109,   109,   109,   109,   109,   110,
     110,   111,   112,   113,   114,   114,   115,   116,   116,   116,
     117,   118,   118,   119,   119,   120,   120,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     122,   122,   123,   123,   124,   124,   124,   125,   125,   126,
     126,   127,   128,   128,   128,   129,   130,   131,   132
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     1,     2,     2,     4,     3,     4,
       2,     7,     4,     1,     4,     2,     3,     3,     2,     1,
       1,     2,     3,     1,     2,     3,     3,     2,     3,     3,
       1,     3,     2,     2,     1,     1,     1,     1,     3,     1,
       2,     1,     2,     1,     2,     1,     1,     1,     2,     3,
       3,     2,     3,     3,     5,     1,     1,     3,     2,     2,
       2,     3,     1,     4,     3,     2,     4,     3,     1,     2,
       1,     2,     1,     2,     1,     1,     3,     1,     3,     3,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     2,
       1,     1,     2,     2,     2,     1,     1,     2,     3,     4,
       2,     4,     4,     1,     3,     1,     4,     1,     3,     4,
       4,     4,     4,     5,     5,     5,     4,     3,     4,     4,
       5,     4,     1,     2,     2,     3,     3,     2,     4,     6,
       3,     2,     1,     1,     1,     2,     2,     2,     3,     1,
       2,     3,     1,     1,     2,     2,     3,     1,     2,     1,
       3,     4,     1,     2,     1,     2,     3,     6,     1,     1,
       2,     3,     2,     1,     1,     2,     1,     1,     1,     2,
       1,     1,     2,     1,     2,     1,     2,     1,     2,     2,
       2,     2,     3,     3,     3,     3,     2,     1,     4,     4,
       1,     2,     1,     3,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    12,   217,   215,   216,    29,   218,     0,
       0,     0,     0,    11,     3,    13,    14,     4,    23,     0,
       0,     5,    30,     6,    33,     0,     0,     0,    40,     7,
      57,     0,     0,     0,     8,    72,     9,    10,    45,    46,
      44,     0,   180,    28,     0,   200,     0,   197,    16,     0,
      40,     0,   181,   183,   185,   187,    45,    46,     0,     0,
     178,   142,     0,     0,     0,   177,     0,     0,    15,    84,
      92,     0,    78,    80,    82,    85,     0,    25,   209,   211,
       0,    34,   100,   178,     0,   115,   105,     0,     0,     0,
       0,     0,   197,     0,     0,    98,    97,     0,     0,    93,
      95,   101,   106,     0,   113,    96,   117,   132,     0,     0,
       0,     0,   176,     0,    37,     0,    31,     0,    58,     0,
       0,    61,    43,    42,    65,     0,   186,     0,     0,   196,
       0,     0,   208,   207,     0,     0,     0,     0,     0,    20,
     182,   174,   184,   201,   200,   189,   188,   191,   190,     0,
      77,   179,   153,   149,     0,   152,    74,   144,     0,   143,
       0,    75,     0,     0,    79,    81,    83,     0,    26,    27,
     210,    35,     0,     0,     0,   107,   133,   102,   103,   104,
       0,     0,     0,     0,     0,   172,     0,   134,     0,     0,
       0,     0,    99,    36,    94,     0,   110,   137,   141,   168,
     157,     0,     0,   159,   162,   164,     0,     0,     0,   113,
       0,     0,    38,    39,    51,   212,   213,   214,    56,    41,
      47,     0,    53,    49,    55,    32,    59,    60,    62,    63,
       0,     0,     0,    66,     0,   194,   192,   204,   206,   205,
       0,   202,   195,   193,     0,    17,     0,     0,     0,    18,
     175,    76,   150,     0,   154,   147,     0,   146,   145,   155,
       0,    73,    24,    91,    90,    86,    87,   135,     0,     0,
     140,     0,     0,     0,     0,     0,     0,   127,     0,     0,
       0,   115,   114,   173,   118,     0,   169,     0,   158,     0,
       0,   163,   165,     0,     0,     0,     0,   108,     0,     0,
       0,   136,     0,    52,    54,    50,    68,    69,     0,     0,
      70,     0,   198,   199,    19,    22,     0,   151,   148,   156,
       0,     0,     0,   119,   129,   128,   126,   131,     0,     0,
       0,   120,   122,   121,   170,     0,   160,     0,   166,     0,
     155,   138,     0,   111,   109,   112,    48,    64,    67,    71,
     203,     0,    88,    89,   123,   125,   130,   124,   171,   161,
       0,     0,   116,     0,     0,   139,    21,   167
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    14,    15,    95,    48,    17,    96,    19,    20,
      21,    22,    23,    24,    97,    26,    27,    50,   219,   220,
     221,   222,    29,    30,    31,    32,    33,   131,   232,   233,
      34,    35,    62,    36,    37,    71,    72,    73,    74,   265,
     266,    75,    98,    99,   100,   101,   102,   103,   104,   210,
     105,   106,   107,   108,   109,   156,   157,   158,   161,   201,
     202,   203,   204,   285,   286,   110,   287,   142,   111,   112,
     145,    65,    52,    53,    54,    55,   240,   241,   134,    80,
     159,   224,    66,    67,    40,    41
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -161
static const yytype_int16 yypact[] =
{
    -161,   146,  -161,  -161,  -161,  -161,  -161,     2,  -161,   159,
      13,    73,   515,  -161,  -161,  -161,    44,  -161,  -161,   111,
      47,  -161,  -161,  -161,  -161,    -2,   384,    70,   168,  -161,
    -161,    -2,   384,    80,  -161,  -161,  -161,  -161,  -161,  -161,
     207,    41,  -161,  -161,   264,    49,    51,    67,  -161,   213,
      65,   298,    71,    87,  -161,   121,   221,   221,   138,   139,
     239,  -161,    64,   149,   167,  -161,   221,   221,  -161,  -161,
    -161,    24,  -161,   188,   151,   174,   187,   225,   209,  -161,
     200,  -161,  -161,   119,   485,   227,  -161,   244,   259,   261,
     149,   456,   171,    32,   249,  -161,  -161,   200,   316,  -161,
    -161,  -161,  -161,   -17,  -161,  -161,  -161,  -161,    64,   149,
     181,   127,  -161,   268,   289,   412,   290,   200,  -161,   350,
     274,   296,  -161,  -161,  -161,   475,  -161,   254,   164,  -161,
     263,   164,  -161,  -161,   200,     2,   294,   300,   200,  -161,
    -161,  -161,   303,  -161,  -161,  -161,   121,  -161,   121,   311,
    -161,  -161,   122,  -161,    45,  -161,  -161,  -161,   200,  -161,
     222,  -161,    64,   200,  -161,  -161,  -161,   241,  -161,  -161,
    -161,  -161,   502,   249,   181,  -161,  -161,  -161,  -161,  -161,
      64,   528,   149,   149,    50,  -161,   249,  -161,   181,   140,
     297,   181,  -161,  -161,  -161,   320,  -161,  -161,  -161,  -161,
    -161,    33,   155,  -161,   333,    17,   331,   334,   -17,   145,
     149,    64,  -161,  -161,  -161,  -161,  -161,  -161,  -161,   327,
    -161,   429,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
       2,     2,   330,   335,     2,  -161,  -161,  -161,  -161,  -161,
     179,  -161,  -161,  -161,   210,  -161,   200,     2,     2,  -161,
    -161,  -161,  -161,   356,  -161,  -161,   200,  -161,  -161,  -161,
     177,  -161,  -161,  -161,  -161,   223,  -161,  -161,   181,    64,
    -161,   200,   200,    64,    64,   149,   149,  -161,   181,    64,
      64,  -161,  -161,  -161,  -161,    33,  -161,   359,  -161,   541,
     337,  -161,   364,   351,   277,    64,   369,  -161,   200,   200,
     200,  -161,   412,  -161,  -161,  -161,  -161,  -161,   343,   475,
     367,   164,  -161,  -161,  -161,  -161,   363,  -161,  -161,  -161,
     241,   241,    64,  -161,  -161,  -161,  -161,  -161,    64,    64,
      64,  -161,  -161,  -161,  -161,   380,  -161,   355,  -161,   386,
     149,  -161,   370,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,   149,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
     371,    64,  -161,   200,   149,  -161,  -161,  -161
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -161,  -161,  -161,  -161,   406,  -161,  -161,   413,  -161,  -161,
    -161,  -161,  -161,  -161,   114,  -161,   108,   418,  -161,   118,
    -161,   201,  -161,  -161,  -161,  -161,  -161,   258,  -161,   112,
    -161,  -161,   360,  -161,  -161,  -161,   362,  -161,  -161,  -161,
     -34,  -161,   394,   -51,  -161,  -161,   346,   328,  -102,  -161,
    -161,  -161,   -61,   -89,   -11,   -66,   273,   291,   -79,  -160,
    -152,   142,  -161,  -161,   162,   -76,  -161,  -161,   -77,    -7,
       6,    11,  -161,  -161,   400,   235,   317,   141,  -161,   419,
     -25,  -161,     1,     3,  -161,  -161
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -143
static const yytype_int16 yytable[] =
{
      81,    63,    38,   116,    39,    64,   118,   174,   260,   209,
      56,   180,    57,    43,   269,    78,   189,   188,   191,    42,
      51,   195,   211,   176,    79,    69,   139,    56,   279,    57,
     198,   280,   187,    56,   292,    57,    58,     5,     6,    79,
      60,    70,   197,    44,   152,    45,   123,   194,   153,    61,
     293,   154,    46,    63,   155,   171,   152,   138,   163,    90,
     153,    92,    68,   147,   129,    76,   155,    61,   194,   127,
     283,   151,   192,   147,    79,   152,   186,   275,   196,   153,
     183,   124,   154,    77,   184,   155,    79,   130,   113,   124,
      59,    79,   226,   282,   151,   189,   261,   268,   120,    56,
     211,    57,   115,   273,   274,    79,   114,   124,   322,   245,
     278,   267,    69,   249,   270,    25,   121,   140,   330,   141,
      56,   205,    57,    49,     5,     6,   223,   295,    70,   257,
      44,   300,    45,   258,   143,   284,   234,   252,   262,    46,
     253,   246,   260,   254,    85,   301,     2,     3,    92,     4,
     172,     5,     6,     7,   206,   149,     8,    61,     9,   277,
     207,   298,     4,   173,     5,     6,   299,   206,    10,    11,
      44,   205,    45,   276,   272,   150,    12,   237,   238,    46,
      13,   239,   160,   297,    61,   205,     5,     6,    47,   166,
     288,   130,    44,   289,    45,   167,   328,   329,   199,   205,
     290,    46,   205,   323,   185,   115,   165,   326,   327,    79,
      47,   124,   319,   331,   332,   289,   200,   311,   312,   333,
     122,   314,   290,   168,    42,   170,   132,     5,     6,   341,
     133,   318,   305,    44,   144,    45,   306,   307,    42,   199,
     310,    79,    46,   169,     5,     6,   324,   325,   311,   313,
      44,    47,    45,   315,   316,   263,   354,   259,   264,    46,
    -142,   361,   355,   356,   357,   320,   321,   235,    47,     5,
       6,   236,   363,   343,   344,   345,   242,    45,   190,   205,
     243,   177,     5,     6,    46,   367,   352,   353,    44,   205,
      45,   146,   148,    47,   199,   365,   178,    46,   179,   125,
     205,   146,   148,   128,   212,   205,    47,   213,   225,    38,
     228,    39,   340,   223,   229,    42,   135,    82,   136,     4,
     234,     5,     6,     7,    83,    84,   247,    44,     9,    45,
     185,   137,   248,    85,    86,   250,    46,   281,   366,    79,
      87,    88,    89,    90,    91,    92,     4,    93,     5,     6,
     193,    82,   251,     4,   291,     5,     6,     7,    83,    84,
      94,    44,     9,    45,   294,   302,   296,    85,    86,   308,
      46,   317,   335,   309,    87,    88,    89,    90,    91,    92,
     338,    93,   337,   339,   227,    82,   342,     4,   349,     5,
       6,     7,    83,    84,    94,    44,     9,    45,   351,   358,
     359,    85,    86,   360,    46,   362,   364,    16,    87,    88,
      89,    90,    91,    92,    18,    93,   347,     5,     6,    28,
     346,   348,   304,    44,   162,    45,   119,   255,    94,   214,
     175,   336,    46,   164,     5,     6,   215,   216,   217,   208,
      44,    47,    45,   218,   126,   256,   303,   334,   244,    46,
     117,     0,   350,   215,   216,   217,     0,     0,    47,   181,
     218,     5,     6,     0,    60,     0,     0,    44,     0,    45,
       0,     0,     0,    61,     0,     0,    46,     0,   230,   231,
       5,     6,     0,   182,     0,    47,    44,     0,    45,     0,
       5,     6,     0,    60,     0,    46,    44,     0,    45,     0,
       0,     0,    85,     0,    47,    46,     0,     5,     6,     0,
      60,     0,    90,    44,    47,    45,     0,     0,     0,    61,
       5,     6,    46,    60,     0,     0,    44,     0,    45,    90,
       0,    47,    61,     5,     6,    46,    60,     0,     0,    44,
       0,    45,     0,     0,    47,   271,     5,     6,    46,     0,
       0,     0,    44,     0,    45,     0,     0,    47,   199,     0,
       0,    46,     0,     0,     0,     0,     0,     0,     0,     0,
      47
};

static const yytype_int16 yycheck[] =
{
      25,    12,     1,    28,     1,    12,    31,    83,   160,   111,
       9,    90,     9,     7,   174,    17,    93,    93,    94,    17,
       9,    38,   111,    84,    41,     1,    51,    26,   188,    26,
     109,   191,    93,    32,    17,    32,    23,     5,     6,    41,
       8,    17,   108,    11,    11,    13,    40,    98,    15,    17,
      33,    18,    20,    64,    21,    80,    11,    51,    34,    27,
      15,    29,    18,    57,    13,    18,    21,    17,   119,    20,
      37,    60,    97,    67,    41,    11,    44,    27,   103,    15,
      91,    40,    18,    36,    91,    21,    41,    20,    18,    40,
      17,    41,   117,   195,    83,   172,   162,   173,    18,    98,
     189,    98,    37,   182,   183,    41,    36,    40,   268,   134,
     186,   172,     1,   138,   180,     1,    36,    46,   278,    32,
     119,   110,   119,     9,     5,     6,   115,   206,    17,   154,
      11,   210,    13,   158,    13,   201,   125,    15,   163,    20,
      18,   135,   294,    21,    17,   211,     0,     1,    29,     3,
      31,     5,     6,     7,    27,    17,    10,    17,    12,   184,
      33,    16,     3,    44,     5,     6,    21,    27,    22,    23,
      11,   160,    13,   184,   181,    36,    30,    13,    14,    20,
      34,    17,    33,   208,    17,   174,     5,     6,    29,    38,
      35,    20,    11,    38,    13,    21,   275,   276,    17,   188,
      45,    20,   191,   269,    33,    37,    18,   273,   274,    41,
      29,    40,    35,   279,   280,    38,    35,    38,    39,   285,
      13,   246,    45,    36,    17,    16,    13,     5,     6,   295,
      17,   256,   221,    11,    13,    13,   230,   231,    17,    17,
     234,    41,    20,    18,     5,     6,   271,   272,    38,    39,
      11,    29,    13,   247,   248,    14,   322,    35,    17,    20,
      33,   340,   328,   329,   330,    42,    43,    13,    29,     5,
       6,    17,   351,   298,   299,   300,    13,    13,    29,   268,
      17,    37,     5,     6,    20,   364,   320,   321,    11,   278,
      13,    56,    57,    29,    17,   361,    37,    20,    37,    41,
     289,    66,    67,    45,    36,   294,    29,    18,    18,   308,
      36,   308,    35,   302,    18,    17,    18,     1,    20,     3,
     309,     5,     6,     7,     8,     9,    32,    11,    12,    13,
      33,    33,    32,    17,    18,    32,    20,    17,   363,    41,
      24,    25,    26,    27,    28,    29,     3,    31,     5,     6,
      34,     1,    41,     3,    21,     5,     6,     7,     8,     9,
      44,    11,    12,    13,    33,    38,    32,    17,    18,    39,
      20,    15,    13,    38,    24,    25,    26,    27,    28,    29,
      16,    31,    45,    32,    34,     1,    17,     3,    21,     5,
       6,     7,     8,     9,    44,    11,    12,    13,    35,    19,
      45,    17,    18,    17,    20,    35,    35,     1,    24,    25,
      26,    27,    28,    29,     1,    31,   308,     5,     6,     1,
     302,   309,   221,    11,    64,    13,    32,   154,    44,    17,
      84,   289,    20,    71,     5,     6,    24,    25,    26,   111,
      11,    29,    13,    31,    44,   154,    17,   285,   131,    20,
      31,    -1,   311,    24,    25,    26,    -1,    -1,    29,     3,
      31,     5,     6,    -1,     8,    -1,    -1,    11,    -1,    13,
      -1,    -1,    -1,    17,    -1,    -1,    20,    -1,     3,     4,
       5,     6,    -1,    27,    -1,    29,    11,    -1,    13,    -1,
       5,     6,    -1,     8,    -1,    20,    11,    -1,    13,    -1,
      -1,    -1,    17,    -1,    29,    20,    -1,     5,     6,    -1,
       8,    -1,    27,    11,    29,    13,    -1,    -1,    -1,    17,
       5,     6,    20,     8,    -1,    -1,    11,    -1,    13,    27,
      -1,    29,    17,     5,     6,    20,     8,    -1,    -1,    11,
      -1,    13,    -1,    -1,    29,    17,     5,     6,    20,    -1,
      -1,    -1,    11,    -1,    13,    -1,    -1,    29,    17,    -1,
      -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      29
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    48,     0,     1,     3,     5,     6,     7,    10,    12,
      22,    23,    30,    34,    49,    50,    51,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    69,
      70,    71,    72,    73,    77,    78,    80,    81,   129,   130,
     131,   132,    17,   117,    11,    13,    20,    29,    52,    61,
      64,   118,   119,   120,   121,   122,   129,   130,    23,    17,
       8,    17,    79,   101,   116,   118,   129,   130,    18,     1,
      17,    82,    83,    84,    85,    88,    18,    36,    17,    41,
     126,   127,     1,     8,     9,    17,    18,    24,    25,    26,
      27,    28,    29,    31,    44,    51,    54,    61,    89,    90,
      91,    92,    93,    94,    95,    97,    98,    99,   100,   101,
     112,   115,   116,    18,    36,    37,   127,   126,   127,    89,
      18,    36,    13,   117,    40,    74,   121,    20,    74,    13,
      20,    74,    13,    17,   125,    18,    20,    33,   117,   127,
      46,    32,   114,    13,    13,   117,   122,   117,   122,    17,
      36,   118,    11,    15,    18,    21,   102,   103,   104,   127,
      33,   105,    79,    34,    83,    18,    38,    21,    36,    18,
      16,   127,    31,    44,   112,    93,    99,    37,    37,    37,
     105,     3,    27,   101,   116,    33,    44,    99,   112,   115,
      29,   112,   127,    34,    90,    38,   127,   102,   105,    17,
      35,   106,   107,   108,   109,   118,    27,    33,    94,    95,
      96,   100,    36,    18,    17,    24,    25,    26,    31,    65,
      66,    67,    68,   118,   128,    18,   127,    34,    36,    18,
       3,     4,    75,    76,   118,    13,    17,    13,    14,    17,
     123,   124,    13,    17,   123,   127,   117,    32,    32,   127,
      32,    41,    15,    18,    21,   103,   104,   127,   127,    35,
     107,   102,   127,    14,    17,    86,    87,    99,   112,   106,
     102,    17,   116,   105,   105,    27,   101,   127,   112,   106,
     106,    17,    95,    37,   102,   110,   111,   113,    35,    38,
      45,    21,    17,    33,    33,   105,    32,   127,    16,    21,
     105,   102,    38,    17,    68,   118,   117,   117,    39,    38,
     117,    38,    39,    39,   127,   117,   117,    15,   127,    35,
      42,    43,   106,   102,   127,   127,   102,   102,   105,   105,
     106,   102,   102,   102,   111,    13,   108,    45,    16,    32,
      35,   102,    17,   127,   127,   127,    66,    63,    76,    21,
     124,    35,    87,    87,   102,   102,   102,   102,    19,    45,
      17,   105,    35,   105,    35,   102,   127,   105
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
#line 112 "mta_parse.y"
    { mta->yy_state = MTA::YYRet_Exit; }
    break;

  case 3:
#line 113 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok;
	    return mta->yy_state; }
    break;

  case 4:
#line 116 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 5:
#line 118 "mta_parse.y"
    {
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 6:
#line 121 "mta_parse.y"
    {
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 7:
#line 124 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 8:
#line 126 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 9:
#line 128 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 10:
#line 130 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 11:
#line 132 "mta_parse.y"
    {
          // presumably leaving a namespace -- check that..
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 12:
#line 135 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
    break;

  case 13:
#line 139 "mta_parse.y"
    {
  	  if((yyvsp[(1) - (1)].typ) != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
            // a typedef can never be literally a template or a template inst!
            (yyval.typ)->ClearType(TypeDef::TEMPLATE);
            (yyval.typ)->ClearType(TypeDef::TEMPL_INST);
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("typedef", sp, (yyval.typ)); } }
    break;

  case 14:
#line 150 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 15:
#line 151 "mta_parse.y"
    {
	    if((yyvsp[(1) - (2)].typ) != NULL)  SETDESC((yyvsp[(1) - (2)].typ),(yyvsp[(2) - (2)].chr)); }
    break;

  case 16:
#line 155 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 17:
#line 156 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(2) - (4)].typ); (yyvsp[(2) - (4)].typ)->name = (yyvsp[(3) - (4)].chr); mta->type_stack.Pop(); }
    break;

  case 18:
#line 160 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); (yyvsp[(2) - (3)].typ)->AddParent((yyvsp[(1) - (3)].typ)); (yyvsp[(2) - (3)].typ)->AssignType((yyvsp[(1) - (3)].typ)->type);
	    mta->type_stack.Pop(); }
    break;

  case 19:
#line 163 "mta_parse.y"
    { /* annoying place for a comment, but.. */
            (yyval.typ) = (yyvsp[(3) - (4)].typ); (yyvsp[(3) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(3) - (4)].typ)->AssignType((yyvsp[(1) - (4)].typ)->type);
	    mta->type_stack.Pop(); }
    break;

  case 20:
#line 168 "mta_parse.y"
    {
	  if((yyvsp[(1) - (2)].typ)->parents.size < 2) {
	    cerr << "E!!: Error in predeclared type: " << (yyvsp[(1) - (2)].typ)->name << " second parent not found!"
		 << endl;
	    (yyval.typ) = (yyvsp[(1) - (2)].typ);
	  }
	  else {
	    TypeDef* td = (yyvsp[(1) - (2)].typ)->parents[1]; mta->type_stack.Pop();
	    (yyval.typ) = td; } }
    break;

  case 21:
#line 177 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (7)].typ); (yyval.typ)->AssignType(TypeDef::FUN_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 22:
#line 180 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (4)].typ); (yyval.typ)->AssignType(TypeDef::METH_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 23:
#line 185 "mta_parse.y"
    {
            TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
            (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("enum", sp, (yyval.typ)); (yyval.typ)->source_end = mta->line-1; } }
    break;

  case 26:
#line 195 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); }
    break;

  case 27:
#line 196 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 28:
#line 199 "mta_parse.y"
    {
  	    (yyval.typ) = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::ENUM); mta->cur_enum = (yyvsp[(2) - (2)].typ);
	    mta->SetSource((yyval.typ), false); mta->type_stack.Pop(); }
    break;

  case 29:
#line 203 "mta_parse.y"
    {
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->cur_enum = (yyval.typ);
	    mta->SetSource((yyval.typ), false); (yyval.typ)->AssignType(TypeDef::ENUM); }
    break;

  case 30:
#line 210 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("class", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 31:
#line 218 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 32:
#line 219 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 33:
#line 223 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("class", sp, (yyval.typ)); mta->FixClassTypes((yyval.typ));
              (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 36:
#line 237 "mta_parse.y"
    {
	    if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	    else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 37:
#line 243 "mta_parse.y"
    {
	    (yyvsp[(1) - (2)].typ)->tokens.keep = true; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (2)].typ), true); }
    break;

  case 38:
#line 246 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
            mta->SetSource((yyvsp[(1) - (3)].typ), true); }
    break;

  case 39:
#line 249 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (3)].typ), true); 
	  }
    break;

  case 40:
#line 256 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 41:
#line 257 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 42:
#line 260 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::CLASS); mta->ClearSource((yyvsp[(2) - (2)].typ)); /* tyname set -- premature */
            mta->cur_mstate = MTA::prvt; }
    break;

  case 43:
#line 265 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 44:
#line 269 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::CLASS); 
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 45:
#line 276 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::STRUCT);
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 46:
#line 283 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::UNION);
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 47:
#line 293 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].typ) != NULL) mta->cur_class->AddParent((yyvsp[(1) - (1)].typ)); }
    break;

  case 48:
#line 295 "mta_parse.y"
    {
            if((yyvsp[(3) - (3)].typ) != NULL) {mta->cur_class->AddParent((yyvsp[(3) - (3)].typ));
	      if(!mta->cur_class->HasOption("MULT_INHERIT"))
                mta->cur_class->opts.Add("MULT_INHERIT"); } }
    break;

  case 50:
#line 303 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 51:
#line 304 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 52:
#line 305 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 57:
#line 317 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ));
              mta->FixClassTypes((yyval.typ));
	      (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 60:
#line 332 "mta_parse.y"
    {
          if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	  else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 61:
#line 338 "mta_parse.y"
    {
            mta->state = MTA::Parse_inclass; (yyvsp[(1) - (2)].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 62:
#line 341 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 63:
#line 343 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 64:
#line 348 "mta_parse.y"
    {
	    (yyvsp[(5) - (5)].typ)->templ_pars.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_pars.Duplicate(mta->cur_templ_pars);
	    (yyvsp[(5) - (5)].typ)->SetType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(5) - (5)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 65:
#line 355 "mta_parse.y"
    { mta->cur_templ_pars.Reset(); }
    break;

  case 67:
#line 360 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 68:
#line 364 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 69:
#line 365 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 70:
#line 366 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 71:
#line 367 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (3)].typ)); (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 72:
#line 370 "mta_parse.y"
    {
            if(mta->cur_is_trg) { /* only add reg_funs in target space */
              TypeDef* nt = new TypeDef((yyvsp[(1) - (1)].meth)->name, TypeDef::FUNCTION,0,0);
              mta->SetSource(nt, false);
              taMisc::types.Add(nt);
              nt->methods.AddUniqNameNew((yyvsp[(1) - (1)].meth));
              taMisc::reg_funs.Link(nt); }
            mta->meth_stack.Pop(); }
    break;

  case 73:
#line 380 "mta_parse.y"
    {
            (yyvsp[(3) - (4)].meth)->type = (yyvsp[(2) - (4)].typ); SETDESC((yyvsp[(3) - (4)].meth),(yyvsp[(4) - (4)].chr)); (yyval.meth) = (yyvsp[(3) - (4)].meth); }
    break;

  case 74:
#line 382 "mta_parse.y"
    {
            (yyvsp[(2) - (3)].meth)->type = &TA_int; SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); (yyval.meth) = (yyvsp[(2) - (3)].meth); }
    break;

  case 75:
#line 386 "mta_parse.y"
    {
            (yyvsp[(1) - (2)].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = true; }
    break;

  case 76:
#line 391 "mta_parse.y"
    {
            taMisc::Info("using namespace:", (yyvsp[(3) - (4)].chr));
          }
    break;

  case 77:
#line 396 "mta_parse.y"
    {
            taMisc::Info("entering namespace:", (yyvsp[(2) - (3)].chr));
          }
    break;

  case 81:
#line 406 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 84:
#line 411 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 85:
#line 416 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 86:
#line 419 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm)); if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
	    mta->enum_stack.Pop(); }
    break;

  case 88:
#line 426 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 89:
#line 431 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 90:
#line 439 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 92:
#line 444 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 93:
#line 447 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 94:
#line 448 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 95:
#line 451 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].memb) != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->HasOption("IGNORE"))
		 && !((yyvsp[(1) - (1)].memb)->type->IsConst())) {
		mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
		if(mta->verbose >= 3)
		  cerr << "M!!: member: " << (yyvsp[(1) - (1)].memb)->name << " added to: "
		       << mta->cur_class->name << "\n"; } }
	    mta->memb_stack.Pop(); (yyval.typ) = NULL; }
    break;

  case 96:
#line 460 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].meth) != NULL) {
	      if(mta->cur_mstate == MTA::pblc) {
		if((yyvsp[(1) - (1)].meth)->HasOption("IGNORE"))
		  mta->cur_class->ignore_meths.AddUnique((yyvsp[(1) - (1)].meth)->name);
		else {
		  mta->cur_class->methods.AddUniqNameNew((yyvsp[(1) - (1)].meth));
		  if(mta->verbose >= 3)
		    cerr << "M!!: method: " << (yyvsp[(1) - (1)].meth)->name << " added to: "
			 << mta->cur_class->name << "\n"; } } }
	    else {
	      mta->cur_meth = NULL; }
	    mta->meth_stack.Pop(); (yyval.typ) = NULL; }
    break;

  case 97:
#line 473 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
            mta->TypeAdded("enum", &(mta->cur_class->sub_types), (yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->source_end = mta->line-1;
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 98:
#line 479 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 99:
#line 483 "mta_parse.y"
    { /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew((yyvsp[(1) - (2)].typ));
            (yyvsp[(1) - (2)].typ)->SetType(TypeDef::SUBTYPE);
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 100:
#line 488 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 102:
#line 493 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 103:
#line 494 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 104:
#line 495 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 105:
#line 496 "mta_parse.y"
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
		if(mta->verbose >= 3)
		  cerr << "M!!: method: " << mta->last_meth->name << " added to: "
		       << mta->cur_class->name << "\n"; } } }
    break;

  case 107:
#line 517 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 108:
#line 523 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 109:
#line 524 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 110:
#line 525 "mta_parse.y"
    { }
    break;

  case 111:
#line 526 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
            int spsz = sp->size;
            TypeDef* nty = (yyvsp[(1) - (4)].typ)->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    (yyvsp[(2) - (4)].memb)->type = nty; }
    break;

  case 112:
#line 533 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 113:
#line 537 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 114:
#line 541 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(3) - (3)].memb)->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 115:
#line 547 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 116:
#line 553 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 118:
#line 559 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 119:
#line 560 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 120:
#line 561 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 121:
#line 562 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 122:
#line 563 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 123:
#line 564 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 124:
#line 565 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 125:
#line 566 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 126:
#line 567 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 127:
#line 568 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 128:
#line 569 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 129:
#line 570 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 130:
#line 571 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(5) - (5)].chr);
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef((yyvsp[(3) - (5)].meth)->name, TypeDef::FUNCTION,0,0);
              taMisc::types.Add(nt);
	      nt->methods.AddUniqNameNew((yyvsp[(3) - (5)].meth)); (yyvsp[(3) - (5)].meth)->type = (yyvsp[(2) - (5)].typ);
	      mta->meth_stack.Pop();  (yyvsp[(3) - (5)].meth)->fun_argc = (yyvsp[(4) - (5)].rval); (yyvsp[(3) - (5)].meth)->arg_types.size = (yyvsp[(4) - (5)].rval);
	      (yyvsp[(3) - (5)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(3) - (5)].meth),(yyvsp[(5) - (5)].chr)); }
	    else { (yyval.meth) = NULL; mta->meth_stack.Pop(); } }
    break;

  case 131:
#line 581 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(4) - (4)].chr);
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef((yyvsp[(2) - (4)].meth)->name, TypeDef::FUNCTION,0,0);
              taMisc::types.Add(nt);
	      nt->methods.AddUniqNameNew((yyvsp[(2) - (4)].meth)); (yyvsp[(2) - (4)].meth)->type = &TA_int;
	      mta->meth_stack.Pop();  (yyvsp[(2) - (4)].meth)->fun_argc = (yyvsp[(3) - (4)].rval); (yyvsp[(2) - (4)].meth)->arg_types.size = (yyvsp[(3) - (4)].rval);
	      (yyvsp[(2) - (4)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(2) - (4)].meth),(yyvsp[(4) - (4)].chr)); }
	    else { (yyval.meth) = 0; mta->meth_stack.Pop(); } }
    break;

  case 133:
#line 595 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 134:
#line 596 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 135:
#line 598 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 136:
#line 603 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 137:
#line 604 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
    break;

  case 138:
#line 605 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 139:
#line 606 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 140:
#line 607 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 141:
#line 610 "mta_parse.y"
    {
             (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 142:
#line 617 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 143:
#line 621 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 144:
#line 622 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
    break;

  case 145:
#line 623 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 146:
#line 624 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 147:
#line 625 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 148:
#line 626 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 149:
#line 630 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 150:
#line 631 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 151:
#line 632 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
    break;

  case 152:
#line 636 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 153:
#line 637 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 154:
#line 638 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 155:
#line 641 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 156:
#line 642 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 157:
#line 645 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 158:
#line 646 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 159:
#line 649 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 160:
#line 650 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 161:
#line 651 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 162:
#line 654 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 163:
#line 656 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      String ad = (yyvsp[(2) - (2)].chr);
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 164:
#line 667 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 165:
#line 671 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 166:
#line 675 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 167:
#line 679 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 168:
#line 683 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 172:
#line 698 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 173:
#line 702 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 174:
#line 705 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 175:
#line 706 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 176:
#line 709 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 178:
#line 713 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 179:
#line 714 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 180:
#line 717 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
                                  mta->SetSource((yyval.typ), false); }
    break;

  case 182:
#line 722 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ)->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, (yyval.typ)); }
	  }
    break;

  case 184:
#line 732 "mta_parse.y"
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

  case 186:
#line 744 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(2) - (2)].typ)->GetConstType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("const", sp, (yyval.typ)); }
	  }
    break;

  case 188:
#line 753 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::STRUCT);
            (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 189:
#line 755 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::STRUCT); }
    break;

  case 190:
#line 756 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::UNION);
            (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 191:
#line 758 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::UNION); }
    break;

  case 192:
#line 759 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 193:
#line 763 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 194:
#line 767 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 195:
#line 768 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 196:
#line 769 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 198:
#line 771 "mta_parse.y"
    { /* a template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
	      yyerror("Template syntax error"); YYERROR; }
	    /* if(($3->owner != NULL) && ($3->owner->owner != NULL)) */
	    /*   $$ = $1;	/\* don't allow internal types with external templates *\/ */
	    /* else { */
            String nm = (yyvsp[(1) - (4)].typ)->GetTemplInstName(mta->cur_templ_pars);
            TypeDef* td;
            int lx_tok;
            if((td = mta->FindName(nm, lx_tok)) == NULL) {
              td = (yyvsp[(1) - (4)].typ)->Clone(); td->name = nm;
              td->SetTemplType((yyvsp[(1) - (4)].typ), mta->cur_templ_pars);
              TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
              (yyval.typ) = sp->AddUniqNameOld(td);
              if((yyval.typ) == td) mta->TypeAdded("template instance", sp, (yyval.typ)); }
            else
              (yyval.typ) = td; }
    break;

  case 199:
#line 788 "mta_parse.y"
    { /* this template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
	      yyerror("Template syntax error"); YYERROR; }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 201:
#line 795 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (2)].typ)->name + "_" + (yyvsp[(2) - (2)].typ)->name;
	    TypeDef* nty = new TypeDef((char*)nm);
            nty->AssignType((yyvsp[(1) - (2)].typ)->type); // get from first guy
            nty->SetType((yyvsp[(2) - (2)].typ)->type);   // add from second
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("combo", sp, (yyval.typ));
	      nty->size = (yyvsp[(2) - (2)].typ)->size; nty->AddParent((yyvsp[(1) - (2)].typ)); nty->AddParent((yyvsp[(2) - (2)].typ)); }
	    else { mta->TypeNotAdded("combo", sp, (yyval.typ), nty); delete nty; }
	  }
    break;

  case 203:
#line 810 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 204:
#line 815 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 205:
#line 816 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 206:
#line 817 "mta_parse.y"
    { (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 208:
#line 822 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 215:
#line 838 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 216:
#line 842 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 217:
#line 846 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 218:
#line 850 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 2995 "y.tab.c"
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


#line 853 "mta_parse.y"


	/* end of grammar */

void yyerror(const char *s) { 	/* called for yacc syntax error */
  int i;
  if((mta->verbose < 1) && (!mta->cur_is_trg))
    return;

  if(strcmp(s, "parse error") == 0) {
    cerr << "E!!: Syntax Error, line " << mta->st_line << ":\t" << MTA::LastLn << "\n";
    cerr << "\t\t\t";
    for(i=0; i < mta->st_col + 1; i++)
      cerr << " ";
    cerr << "^\n";
  }
  else {
    cerr << "E!!: " << s << "line " << mta->st_line << ":\t" << MTA::LastLn << "\n";
  }
}

