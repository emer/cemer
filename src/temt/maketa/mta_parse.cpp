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
#define YYLAST   625

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  48
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  94
/* YYNRULES -- Number of rules.  */
#define YYNRULES  263
/* YYNRULES -- Number of states.  */
#define YYNSTATES  450

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
      33,    34,    32,    41,    38,    42,    45,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    47,
      40,     2,    39,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    36,    43,    35,    44,     2,     2,     2,
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
     104,   106,   108,   113,   116,   120,   124,   127,   129,   131,
     134,   138,   140,   143,   147,   151,   154,   158,   162,   164,
     168,   171,   174,   176,   178,   181,   184,   186,   189,   192,
     194,   198,   200,   203,   205,   210,   213,   215,   218,   220,
     222,   224,   227,   231,   233,   236,   240,   243,   247,   252,
     256,   260,   263,   266,   270,   274,   280,   285,   290,   296,
     298,   300,   304,   307,   310,   313,   318,   323,   328,   330,
     335,   339,   342,   346,   352,   356,   362,   364,   367,   369,
     372,   374,   377,   379,   381,   385,   387,   391,   395,   399,
     401,   403,   405,   407,   409,   412,   414,   416,   418,   420,
     423,   425,   427,   430,   433,   436,   438,   440,   443,   447,
     452,   455,   460,   465,   467,   471,   473,   478,   480,   484,
     489,   494,   499,   505,   511,   516,   522,   528,   534,   539,
     543,   548,   553,   559,   564,   570,   576,   582,   588,   590,
     593,   596,   600,   604,   607,   612,   619,   623,   626,   628,
     630,   632,   635,   638,   641,   645,   647,   650,   654,   656,
     658,   661,   664,   668,   670,   673,   675,   679,   684,   686,
     689,   691,   694,   698,   705,   707,   710,   712,   714,   717,
     721,   724,   727,   730,   732,   734,   737,   739,   741,   743,
     745,   748,   750,   753,   755,   758,   760,   763,   765,   769,
     773,   777,   781,   784,   787,   790,   792,   797,   802,   805,
     808,   811,   814,   816,   819,   821,   823,   827,   829,   831,
     833,   835,   837,   839,   841,   844,   847,   849,   851,   853,
     855,   857,   859,   861
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      49,     0,    -1,    -1,    49,    50,    -1,    49,    55,    -1,
      49,    59,    -1,    49,    61,    -1,    49,    71,    -1,    49,
      73,    -1,    49,    82,    -1,    49,    85,    -1,    49,    86,
      -1,    49,     1,    -1,    51,    -1,    52,    -1,    52,    18,
      -1,    12,    53,    -1,    12,    63,   133,   136,    -1,    12,
       4,    53,    -1,    12,     4,    63,   133,   136,    -1,    54,
     121,   136,    -1,    54,   121,    16,   136,    -1,   124,    18,
     121,   136,    -1,    54,   136,    -1,    54,    33,    32,   121,
      34,   110,   136,    -1,    54,    20,    32,   121,    -1,   121,
     121,   136,    -1,   121,   121,    16,   136,    -1,   121,    54,
     136,    -1,   124,    -1,   128,    -1,    56,    -1,    57,    87,
      35,   136,    -1,    58,    36,    -1,    58,    18,    36,    -1,
      58,    36,    18,    -1,     7,   121,    -1,     7,    -1,    60,
      -1,    66,   136,    -1,    66,   136,    18,    -1,    62,    -1,
      63,   136,    -1,    63,   134,   136,    -1,    64,    94,    35,
      -1,    65,    36,    -1,    65,    18,    36,    -1,    65,    36,
      18,    -1,    66,    -1,    66,    37,    67,    -1,   140,   121,
      -1,   140,   124,    -1,   140,    -1,   138,    -1,   138,   121,
      -1,   138,   124,    -1,   139,    -1,   139,   121,    -1,   139,
     124,    -1,    68,    -1,    67,    38,    68,    -1,   124,    -1,
      69,   124,    -1,    17,    -1,    17,   130,   131,    39,    -1,
      69,    17,    -1,    70,    -1,    69,    70,    -1,   137,    -1,
      31,    -1,    72,    -1,    77,   136,    -1,    77,   136,    18,
      -1,    74,    -1,    75,   136,    -1,    75,   134,   136,    -1,
      78,   104,    -1,    75,   136,    18,    -1,    75,   134,   136,
      18,    -1,    78,   104,    18,    -1,    76,    94,    35,    -1,
      77,   136,    -1,    77,    36,    -1,    77,    18,    36,    -1,
      77,    36,    18,    -1,   141,    79,    80,    39,    65,    -1,
     141,    79,    39,    65,    -1,   141,    79,    39,     8,    -1,
     141,    79,    80,    39,     8,    -1,    40,    -1,    81,    -1,
      80,    38,    81,    -1,     3,   121,    -1,     4,   121,    -1,
     124,   121,    -1,     3,   121,    21,   124,    -1,     4,   121,
      21,   124,    -1,   124,   121,    21,   124,    -1,    83,    -1,
      30,   123,    84,   107,    -1,    30,    84,   107,    -1,   106,
     110,    -1,    23,    17,   136,    -1,    23,    17,    20,    17,
     136,    -1,    23,    17,    36,    -1,    23,    17,    20,    17,
      36,    -1,    88,    -1,    87,    88,    -1,    89,    -1,    89,
      18,    -1,    90,    -1,    90,    38,    -1,     1,    -1,    93,
      -1,    93,    21,    91,    -1,    92,    -1,    91,    41,    92,
      -1,    91,    42,    92,    -1,    91,    43,    92,    -1,    17,
      -1,    14,    -1,    15,    -1,    17,    -1,    95,    -1,    94,
      95,    -1,    96,    -1,   102,    -1,    56,    -1,    52,    -1,
      63,   136,    -1,     1,    -1,    97,    -1,    24,    37,    -1,
      25,    37,    -1,    26,    37,    -1,    18,    -1,    98,    -1,
       9,    98,    -1,   122,    99,   136,    -1,   122,   100,    21,
     136,    -1,    99,   136,    -1,   122,   100,    16,   136,    -1,
     122,   101,   110,   136,    -1,   100,    -1,    99,    38,   100,
      -1,    17,    -1,    33,    32,    17,    34,    -1,   103,    -1,
     118,   111,   107,    -1,     8,   118,   111,   107,    -1,    31,
     118,   111,   107,    -1,   118,   111,   115,   107,    -1,     8,
     118,   111,   115,   107,    -1,    31,   118,   111,   115,   107,
      -1,    44,   118,   111,   107,    -1,     8,    44,   118,   111,
     107,    -1,    31,    44,   118,   111,   107,    -1,    28,   123,
      27,   110,   107,    -1,    28,    27,   110,   107,    -1,    28,
     123,   136,    -1,    28,     3,   123,   136,    -1,    28,     3,
      17,   136,    -1,    28,   123,   106,   110,   107,    -1,    28,
     106,   110,   107,    -1,    22,    17,    20,    27,   136,    -1,
      22,    17,    20,    17,   136,    -1,    22,    13,    20,    27,
     136,    -1,    22,    13,    20,    17,   136,    -1,   104,    -1,
       9,   104,    -1,    31,   104,    -1,     8,    31,   104,    -1,
     122,   105,   107,    -1,   105,   107,    -1,   122,    27,   110,
     107,    -1,   122,    27,    33,    34,   110,   107,    -1,    27,
     110,   107,    -1,   106,   110,    -1,    17,    -1,   136,    -1,
     108,    -1,   109,   136,    -1,    18,   136,    -1,    18,   108,
      -1,    18,   109,   136,    -1,    15,    -1,    11,    15,    -1,
      11,    18,    15,    -1,    21,    -1,    11,    -1,    11,    21,
      -1,    33,    34,    -1,    33,   112,    34,    -1,    34,    -1,
     112,    34,    -1,   113,    -1,   112,    38,   113,    -1,   112,
      45,    45,    45,    -1,   114,    -1,   114,    21,    -1,   124,
      -1,   124,    17,    -1,   124,    17,    16,    -1,   124,    33,
      32,    17,    34,   110,    -1,    17,    -1,   119,   116,    -1,
     117,    -1,    18,    -1,   116,   117,    -1,   116,    38,   117,
      -1,    13,    19,    -1,    17,    19,    -1,    29,    33,    -1,
      37,    -1,    32,    -1,   120,    32,    -1,    17,    -1,   123,
      -1,   124,    -1,     8,    -1,     8,   124,    -1,   125,    -1,
     125,    46,    -1,   126,    -1,   126,   120,    -1,   127,    -1,
      11,   127,    -1,   129,    -1,    13,    20,    17,    -1,    29,
      20,    17,    -1,    13,    20,    13,    -1,    29,    20,    13,
      -1,   135,    17,    -1,   135,    13,    -1,    20,    13,    -1,
      29,    -1,    13,   130,   131,    39,    -1,    29,   130,   131,
      39,    -1,   138,   129,    -1,   138,   121,    -1,   139,   129,
      -1,   139,   121,    -1,    13,    -1,   129,    13,    -1,    40,
      -1,   132,    -1,   131,    38,   132,    -1,    13,    -1,    17,
      -1,    14,    -1,    17,    -1,    13,    -1,    29,    -1,    17,
      -1,    17,    16,    -1,    17,    20,    -1,    47,    -1,    24,
      -1,    25,    -1,    26,    -1,     5,    -1,     6,    -1,     3,
      -1,    10,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   113,   113,   114,   116,   118,   120,   122,   124,   126,
     128,   130,   135,   139,   150,   151,   155,   156,   158,   160,
     164,   167,   170,   176,   185,   188,   191,   193,   195,   199,
     200,   203,   209,   212,   213,   214,   217,   222,   230,   238,
     239,   243,   252,   253,   257,   263,   266,   269,   276,   277,
     280,   284,   286,   291,   296,   300,   302,   307,   311,   316,
     318,   324,   325,   326,   327,   328,   331,   332,   335,   336,
     340,   348,   349,   353,   363,   364,   365,   369,   370,   371,
     378,   381,   385,   388,   390,   395,   403,   412,   417,   423,
     427,   428,   432,   433,   434,   437,   441,   445,   451,   461,
     463,   467,   472,   475,   481,   484,   490,   491,   494,   495,
     498,   499,   500,   505,   508,   516,   517,   520,   523,   529,
     533,   534,   538,   541,   542,   545,   553,   565,   571,   575,
     585,   589,   590,   591,   592,   593,   612,   613,   619,   620,
     621,   622,   629,   633,   637,   643,   649,   654,   655,   656,
     657,   658,   659,   660,   661,   662,   663,   664,   665,   666,
     667,   668,   669,   679,   689,   690,   691,   692,   696,   697,
     698,   700,   705,   706,   707,   708,   709,   712,   719,   723,
     724,   725,   726,   727,   728,   732,   733,   734,   738,   739,
     740,   743,   744,   747,   748,   751,   752,   753,   756,   758,
     769,   773,   777,   781,   785,   791,   795,   796,   797,   798,
     801,   802,   806,   810,   813,   814,   817,   821,   825,   826,
     827,   830,   831,   840,   841,   852,   853,   861,   862,   869,
     876,   877,   878,   886,   890,   891,   892,   908,   916,   918,
     919,   921,   924,   925,   938,   943,   944,   949,   951,   953,
     958,   959,   960,   963,   964,   967,   970,   973,   974,   975,
     979,   986,   993,  1000
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
  "'<'", "'+'", "'-'", "'|'", "'~'", "'.'", "'&'", "';'", "$accept",
  "list", "typedefn", "typedefns", "typedsub", "defn", "tdtype",
  "enumdefn", "enumdsub", "enumname", "enumnm", "classdecl", "classdecls",
  "classdefn", "classdefns", "classdsub", "classname", "classhead",
  "classnm", "classinh", "classpar", "classptyp", "classpmod", "templdecl",
  "templdecls", "templdefn", "templdefns", "templdsub", "templname",
  "templhead", "templfun", "templopen", "templpars", "templpar", "fundecl",
  "funnm", "regfundefn", "usenamespc", "namespc", "enums", "enumline",
  "enumitm", "enumitms", "enummath", "enummathitm", "enmitmname", "membs",
  "membline", "membdefn", "basicmemb", "nostatmemb", "membnames",
  "membname", "membfunp", "methdefn", "basicmeth", "nostatmeth",
  "mbfundefn", "methname", "fundefn", "funsubdefn", "funsubdecl",
  "funargs", "constfun", "args", "argdefn", "subargdefn", "constrlist",
  "constitms", "constref", "consthsnm", "constcoln", "ptrs", "tyname",
  "membtype", "ftype", "type", "noreftype", "constype", "subtype",
  "structype", "combtype", "typtemplopen", "templargs", "templarg",
  "tdname", "varname", "namespctyp", "term", "access", "structkeyword",
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
      60,    43,    45,   124,   126,    46,    38,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    48,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    50,    51,    51,    52,    52,    52,    52,
      53,    53,    53,    53,    53,    53,    53,    53,    53,    54,
      54,    55,    56,    57,    57,    57,    58,    58,    59,    60,
      60,    61,    62,    62,    63,    64,    64,    64,    65,    65,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    67,
      67,    68,    68,    68,    68,    68,    69,    69,    70,    70,
      71,    72,    72,    73,    74,    74,    74,    74,    74,    74,
      75,    75,    76,    76,    76,    77,    77,    78,    78,    79,
      80,    80,    81,    81,    81,    81,    81,    81,    82,    83,
      83,    84,    85,    85,    86,    86,    87,    87,    88,    88,
      89,    89,    89,    90,    90,    91,    91,    91,    91,    92,
      92,    92,    93,    94,    94,    95,    95,    95,    95,    95,
      95,    96,    96,    96,    96,    96,    97,    97,    98,    98,
      98,    98,    98,    99,    99,   100,   101,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   103,   103,
     103,   103,   104,   104,   104,   104,   104,   105,   106,   107,
     107,   107,   107,   107,   107,   108,   108,   108,   109,   109,
     109,   110,   110,   111,   111,   112,   112,   112,   113,   113,
     114,   114,   114,   114,   114,   115,   116,   116,   116,   116,
     117,   117,   118,   119,   120,   120,   121,   122,   123,   123,
     123,   124,   124,   125,   125,   126,   126,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   128,   128,
     128,   128,   129,   129,   130,   131,   131,   132,   132,   132,
     133,   133,   133,   134,   134,   135,   136,   137,   137,   137,
     138,   139,   140,   141
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     1,     2,     2,     4,     3,     5,
       3,     4,     4,     2,     7,     4,     3,     4,     3,     1,
       1,     1,     4,     2,     3,     3,     2,     1,     1,     2,
       3,     1,     2,     3,     3,     2,     3,     3,     1,     3,
       2,     2,     1,     1,     2,     2,     1,     2,     2,     1,
       3,     1,     2,     1,     4,     2,     1,     2,     1,     1,
       1,     2,     3,     1,     2,     3,     2,     3,     4,     3,
       3,     2,     2,     3,     3,     5,     4,     4,     5,     1,
       1,     3,     2,     2,     2,     4,     4,     4,     1,     4,
       3,     2,     3,     5,     3,     5,     1,     2,     1,     2,
       1,     2,     1,     1,     3,     1,     3,     3,     3,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     2,
       1,     1,     2,     2,     2,     1,     1,     2,     3,     4,
       2,     4,     4,     1,     3,     1,     4,     1,     3,     4,
       4,     4,     5,     5,     4,     5,     5,     5,     4,     3,
       4,     4,     5,     4,     5,     5,     5,     5,     1,     2,
       2,     3,     3,     2,     4,     6,     3,     2,     1,     1,
       1,     2,     2,     2,     3,     1,     2,     3,     1,     1,
       2,     2,     3,     1,     2,     1,     3,     4,     1,     2,
       1,     2,     3,     6,     1,     2,     1,     1,     2,     3,
       2,     2,     2,     1,     1,     2,     1,     1,     1,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     3,     3,
       3,     3,     2,     2,     2,     1,     4,     4,     2,     2,
       2,     2,     1,     2,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     1,    12,   262,   260,   261,    37,   263,     0,
       0,     0,     3,    13,    14,     4,    31,     0,     0,     5,
      38,     6,    41,     0,     0,     0,    48,     7,    70,     8,
      73,     0,     0,     0,     0,     9,    98,    10,    11,    53,
      56,    52,     0,   216,    36,     0,     0,   242,   216,     0,
     235,    16,     0,     0,    48,     0,    29,   221,   223,   225,
      30,   227,     0,    53,    56,     0,   219,   178,     0,     0,
       0,   218,    15,   112,   122,     0,   106,   108,   110,   113,
       0,    33,   253,   256,     0,    42,   130,   219,     0,   145,
     135,     0,     0,     0,     0,     0,     0,   235,     0,     0,
     128,   127,     0,     0,   123,   125,   131,   136,     0,   143,
     126,   147,   168,     0,     0,     0,     0,   217,     0,    45,
       0,    39,     0,    74,     0,     0,    82,    71,    76,     0,
      54,    55,    57,    58,    50,    51,    89,     0,    18,     0,
       0,   226,     0,   244,     0,   255,   234,     0,     0,     0,
       0,     0,    23,   251,   250,   252,     0,     0,     0,    29,
       0,     0,     0,   222,   214,   224,   243,   233,   232,   239,
     227,   241,   227,     0,   104,   102,   220,   189,   185,     0,
     188,   100,   180,     0,   179,     0,   101,   178,     0,     0,
     107,   109,   111,     0,    34,    35,   254,    43,     0,     0,
       0,   137,   169,     0,     0,   132,   133,   134,     0,     0,
       0,     0,     0,   212,     0,   170,     0,     0,     0,   129,
      44,   124,     0,   140,   173,   177,   204,   193,     0,     0,
     195,   198,   200,   145,     0,     0,     0,   143,     0,     0,
      46,    47,    63,   257,   258,   259,    69,    49,    59,     0,
      66,    61,    68,    40,    75,    77,    80,    83,    84,    72,
      79,     0,     0,     0,     0,    90,     0,     0,   230,   228,
     247,   249,   248,     0,   245,   231,   229,     0,     0,     0,
       0,    20,    17,    28,     0,    26,   242,   239,   238,   241,
     240,     0,   215,     0,   186,     0,   190,   183,     0,   182,
     181,   191,     0,    99,    32,   120,   121,   119,   114,   115,
     171,     0,     0,     0,     0,   176,     0,     0,     0,     0,
       0,     0,   159,     0,     0,     0,   145,   144,   213,   148,
       0,     0,   194,     0,     0,   199,   201,     0,     0,     0,
       0,   138,     0,     0,     0,   172,     0,     0,    65,    67,
      62,    78,    92,    93,    87,    86,     0,     0,    94,    19,
       0,   236,   237,    25,     0,    21,    27,    22,   105,   103,
     187,   184,   192,     0,     0,     0,     0,   149,     0,     0,
       0,     0,     0,   161,   160,   158,   163,     0,     0,     0,
     150,     0,   154,   151,     0,     0,   207,   205,   206,   196,
       0,   202,     0,   191,   174,     0,   141,   139,   142,     0,
      60,     0,     0,    91,    88,    85,     0,   246,     0,   116,
     117,   118,   155,   152,   167,   166,   165,   164,   157,   162,
     156,   153,   210,   211,     0,   208,   197,     0,     0,   146,
      64,    95,    96,    97,     0,   209,     0,   175,    24,   203
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    12,    13,   100,    51,    52,    15,   101,    17,
      18,    19,    20,    21,    22,   102,    24,    25,    54,   247,
     248,   249,   250,    27,    28,    29,    30,    31,    32,    33,
      34,   137,   264,   265,    35,    36,    68,    37,    38,    75,
      76,    77,    78,   308,   309,    79,   103,   104,   105,   106,
     107,   108,   109,   238,   110,   111,   112,   113,   114,   181,
     182,   183,   186,   228,   229,   230,   231,   330,   397,   398,
     115,   331,   165,    55,   116,   117,    71,    57,    58,    59,
      60,    61,   148,   273,   274,   156,    84,    62,   184,   252,
      39,    40,    41,    42
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -377
static const yytype_int16 yypact[] =
{
    -377,   185,  -377,  -377,  -377,  -377,  -377,    19,  -377,   294,
      20,   539,  -377,  -377,    22,  -377,  -377,   152,    13,  -377,
    -377,  -377,  -377,    34,   485,   158,    -2,  -377,  -377,  -377,
    -377,    34,   485,    64,   515,  -377,  -377,  -377,  -377,   586,
     586,   586,   106,  -377,  -377,   322,   216,    92,    49,   154,
     111,  -377,   121,   230,   135,   357,   157,   153,   220,  -377,
    -377,   259,   293,   586,   586,   127,   591,    49,    77,   255,
     273,  -377,  -377,  -377,  -377,    27,  -377,   278,   263,   282,
     272,   304,   328,  -377,   290,  -377,  -377,   238,   538,   215,
    -377,   300,   310,   330,   346,   255,   358,   163,    76,   364,
    -377,  -377,   290,   405,  -377,  -377,  -377,  -377,   245,  -377,
    -377,  -377,  -377,    77,   255,   321,   229,  -377,   365,   382,
     559,   384,   290,   406,   445,   390,   410,    29,   417,   236,
    -377,  -377,  -377,  -377,  -377,  -377,  -377,   221,  -377,   230,
      49,  -377,   375,  -377,   359,  -377,  -377,   377,   359,   388,
     411,     7,  -377,  -377,  -377,  -377,   290,   290,    23,  -377,
     386,   386,    19,  -377,  -377,   413,  -377,  -377,  -377,   183,
     132,   194,   167,   420,  -377,  -377,  -377,   297,  -377,   108,
    -377,  -377,  -377,   290,  -377,   504,  -377,  -377,    77,   290,
    -377,  -377,  -377,   464,  -377,  -377,  -377,  -377,   515,   364,
     321,  -377,  -377,   427,   435,  -377,  -377,  -377,    77,   549,
     255,   255,    36,  -377,   364,  -377,   321,   431,   321,  -377,
    -377,  -377,   421,  -377,  -377,  -377,    49,  -377,    57,   239,
    -377,   447,   188,   433,   442,   453,   245,    86,   255,    77,
    -377,  -377,   178,  -377,  -377,  -377,  -377,   439,  -377,   569,
    -377,  -377,  -377,  -377,   469,  -377,  -377,  -377,  -377,  -377,
    -377,    19,    19,   340,   201,  -377,    19,   290,  -377,  -377,
    -377,  -377,  -377,   227,  -377,  -377,  -377,   291,    19,    19,
     290,  -377,  -377,  -377,   290,  -377,  -377,  -377,   259,  -377,
     259,   290,  -377,   114,  -377,   480,  -377,  -377,   290,  -377,
    -377,  -377,   257,  -377,  -377,  -377,  -377,  -377,   418,  -377,
    -377,   321,    57,   237,   254,  -377,    54,   290,    77,    77,
     255,   255,  -377,   321,    57,    77,  -377,  -377,  -377,  -377,
      77,   347,  -377,   596,   454,  -377,   484,   472,   514,    77,
     489,  -377,   290,   290,   290,  -377,   359,   559,    49,  -377,
    -377,  -377,   487,   497,  -377,  -377,   378,   436,   498,  -377,
     359,  -377,  -377,  -377,   467,  -377,  -377,  -377,  -377,  -377,
    -377,  -377,  -377,   464,   464,   464,    77,  -377,    77,   290,
     290,   290,   290,  -377,  -377,  -377,  -377,    77,    77,    77,
    -377,    77,  -377,  -377,   501,   503,  -377,   126,  -377,  -377,
     491,  -377,   513,   255,  -377,   505,  -377,  -377,  -377,   315,
    -377,   591,   591,  -377,  -377,  -377,   591,  -377,   255,  -377,
    -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,
    -377,  -377,  -377,  -377,   402,  -377,  -377,   506,    77,  -377,
    -377,  -377,  -377,  -377,   290,  -377,   255,  -377,  -377,  -377
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -377,  -377,  -377,  -377,   536,   496,   490,  -377,   552,  -377,
    -377,  -377,  -377,  -377,  -377,    21,  -377,  -239,   553,  -377,
     214,  -377,   314,  -377,  -377,  -377,  -377,  -377,  -377,  -377,
    -377,  -377,  -377,   208,  -377,  -377,   507,  -377,  -377,  -377,
     499,  -377,  -377,  -377,   109,  -377,   541,   -51,  -377,  -377,
     483,   459,  -100,  -377,  -377,  -377,   -21,   -19,    -6,  -102,
     408,   412,   -94,  -175,  -176,   248,  -377,  -203,  -377,  -376,
     -72,  -377,  -377,   118,   -27,    -5,    -7,  -377,  -377,   546,
    -377,   -46,   -35,  -144,   241,   457,   574,  -377,   -23,  -377,
       5,    10,  -377,  -377
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -241
static const yytype_int16 yytable[] =
{
      85,   208,    56,   121,   277,    69,    70,   129,   123,   302,
     127,   224,   144,   128,    63,   200,   237,   170,   172,    64,
     225,   435,    23,   280,   355,   312,   216,   218,    73,   152,
      53,    80,   131,   133,   135,   120,    43,    65,    56,   284,
      72,   324,   175,   325,    74,    83,   -81,   259,   159,    81,
      63,    82,   221,   187,    83,    64,   131,   133,   445,   176,
     160,   197,   189,   320,    69,   161,   139,   202,   177,   145,
      83,   129,   178,   221,   145,   179,   -81,   215,   180,   219,
     176,    83,   125,    83,    66,   223,   303,    46,   177,    47,
     211,   212,   178,    67,   328,   179,    49,   239,   180,   254,
     126,    83,   342,    95,    83,    97,   315,   343,   232,   378,
     239,    83,   142,   251,   288,   290,   318,   319,   415,   177,
     214,   391,   327,   178,    83,    44,   329,   311,   281,   180,
     266,   147,   143,   282,   283,   285,   376,   345,    43,   394,
     339,   149,   323,   395,   344,   166,   136,   173,   389,  -238,
     368,   143,  -238,    73,   150,    83,   299,   130,   132,   134,
     300,    83,   302,   174,   434,  -238,   304,   146,    83,    74,
     151,   129,   120,   158,    83,   162,   118,   310,   232,  -238,
     166,   169,   171,   147,  -240,     2,     3,  -240,     4,   322,
       5,     6,     7,   232,   119,     8,   213,     9,   145,   163,
    -240,   -54,   409,   143,   317,   336,   321,   346,    10,   232,
     377,   232,   -57,   341,  -240,    11,   385,   386,   143,   -54,
     -54,   337,   390,   392,   261,   262,   387,   388,   393,    47,
     -57,   -57,    46,   140,    47,   145,    49,   404,   140,   356,
     357,    49,   350,   153,   359,    50,   233,   154,  -178,    46,
      50,    47,   164,   187,   379,   140,   234,   365,    49,   155,
     263,   366,   235,   234,   380,   360,   361,    97,   367,   198,
     369,   381,   166,   332,   422,   371,   423,   333,   287,   289,
     291,   382,   199,   222,   334,   428,   429,   430,   185,   431,
     187,   372,    83,   383,   384,   333,   191,     4,    45,     5,
       6,   192,   334,   193,   232,    46,   167,    47,   194,   438,
     168,    48,   294,   203,    49,   295,   232,   204,   296,   406,
     407,   408,   195,    50,   444,     4,   232,     5,     6,   360,
     362,   232,    46,    46,    47,    47,   447,    83,   226,    48,
     251,    49,    49,     4,   196,     5,     6,   205,   354,   266,
      50,    50,   449,   360,   440,   227,   424,   425,   426,   427,
     394,   209,     5,     6,   395,   396,    66,   206,    46,    46,
      47,    47,   270,   271,    48,    67,   272,    49,    49,   352,
     353,   261,   262,   207,   358,   210,    50,    50,   268,    46,
     275,    47,   269,   217,   276,   140,   363,   364,    49,   286,
     241,   240,   253,    43,   441,   442,    86,    50,     4,   443,
       5,     6,     7,    87,    88,   394,    46,     9,    47,   395,
     278,   448,    89,    90,   255,    49,   257,    91,   258,    92,
      93,    94,    95,    96,    97,   260,    98,   293,   326,     4,
     220,     5,     6,   279,   414,   292,    86,   313,     4,    99,
       5,     6,     7,    87,    88,   314,    46,     9,    47,   373,
     374,   375,    89,    90,   213,    49,  -178,    91,   335,    92,
      93,    94,    95,    96,    97,   338,    98,   347,   305,   306,
     256,   307,   419,   420,   421,   340,    86,   351,     4,    99,
       5,     6,     7,    87,    88,   370,    46,     9,    47,   400,
     401,   418,    89,    90,   402,    49,   405,    91,   411,    92,
      93,    94,    95,    96,    97,    46,    98,    47,   412,   416,
     432,   226,   433,    66,    49,    46,    46,    47,    47,    99,
     437,   226,    67,    50,    49,    49,   436,    14,   301,   439,
     446,   138,    95,    50,    50,   157,    66,    66,   403,    46,
      46,    47,    47,    16,    26,    89,    67,    66,    49,    49,
      46,   410,    47,   349,   413,    95,   316,    50,    50,    49,
      46,   201,    47,   124,   190,   236,   242,   188,    50,    49,
      46,   399,    47,   243,   244,   245,   348,   297,    50,    49,
     246,   298,   141,   243,   244,   245,   267,    46,    50,    47,
     246,   417,    46,    48,    47,   122,    49,    46,   140,    47,
       0,    49,     0,   226,     0,    50,    49,     0,     0,     0,
      50,     0,     0,     0,     0,    50
};

static const yytype_int16 yycheck[] =
{
      23,    95,     9,    26,   148,    11,    11,    34,    31,   185,
      33,   113,    47,    34,     9,    87,   116,    63,    64,     9,
     114,   397,     1,    16,   263,   200,    98,    99,     1,    52,
       9,    18,    39,    40,    41,    37,    17,    17,    45,    16,
      18,   216,    65,   218,    17,    47,    17,    18,    55,    36,
      45,    17,   103,    17,    47,    45,    63,    64,   434,    66,
      55,    84,    35,    27,    70,    55,    45,    88,    11,    20,
      47,    98,    15,   124,    20,    18,    47,    98,    21,   102,
      87,    47,    18,    47,     8,   108,   188,    11,    11,    13,
      96,    96,    15,    17,    37,    18,    20,   116,    21,   122,
      36,    47,    16,    27,    47,    29,   208,    21,   115,   312,
     129,    47,    20,   120,   160,   161,   210,   211,   357,    11,
      44,   324,   222,    15,    47,     7,   228,   199,   151,    21,
     137,    20,    40,   156,   157,   158,   311,   239,    17,    13,
     234,    20,   214,    17,   238,    13,    40,    20,   323,    17,
      36,    40,    20,     1,    33,    47,   179,    39,    40,    41,
     183,    47,   338,    36,    38,    33,   189,    13,    47,    17,
      52,   198,    37,    55,    47,    18,    18,   198,   185,    47,
      13,    63,    64,    20,    17,     0,     1,    20,     3,   212,
       5,     6,     7,   200,    36,    10,    33,    12,    20,    46,
      33,    18,   346,    40,   209,    17,   212,   242,    23,   216,
     312,   218,    18,   236,    47,    30,   318,   319,    40,    36,
      37,    33,   324,   325,     3,     4,   320,   321,   330,    13,
      36,    37,    11,    17,    13,    20,    20,   339,    17,    38,
      39,    20,   249,    13,   267,    29,    17,    17,    33,    11,
      29,    13,    32,    17,    17,    17,    27,   280,    20,    29,
      39,   284,    33,    27,    27,    38,    39,    29,   291,    31,
     293,    17,    13,    34,   376,   298,   378,    38,   160,   161,
     162,    27,    44,    38,    45,   387,   388,   389,    33,   391,
      17,    34,    47,   316,   317,    38,    18,     3,     4,     5,
       6,    38,    45,    21,   311,    11,    13,    13,    36,   403,
      17,    17,    15,    13,    20,    18,   323,    17,    21,   342,
     343,   344,    18,    29,   418,     3,   333,     5,     6,    38,
      39,   338,    11,    11,    13,    13,   438,    47,    17,    17,
     347,    20,    20,     3,    16,     5,     6,    37,     8,   356,
      29,    29,   446,    38,    39,    34,   379,   380,   381,   382,
      13,     3,     5,     6,    17,    18,     8,    37,    11,    11,
      13,    13,    13,    14,    17,    17,    17,    20,    20,   261,
     262,     3,     4,    37,   266,    27,    29,    29,    13,    11,
      13,    13,    17,    29,    17,    17,   278,   279,    20,    13,
      18,    36,    18,    17,   411,   412,     1,    29,     3,   416,
       5,     6,     7,     8,     9,    13,    11,    12,    13,    17,
      32,   444,    17,    18,    18,    20,    36,    22,    18,    24,
      25,    26,    27,    28,    29,    18,    31,    17,    17,     3,
      35,     5,     6,    32,     8,    32,     1,    20,     3,    44,
       5,     6,     7,     8,     9,    20,    11,    12,    13,    41,
      42,    43,    17,    18,    33,    20,    33,    22,    21,    24,
      25,    26,    27,    28,    29,    33,    31,    38,    14,    15,
      35,    17,   373,   374,   375,    32,     1,    18,     3,    44,
       5,     6,     7,     8,     9,    15,    11,    12,    13,    45,
      16,    34,    17,    18,    32,    20,    17,    22,    21,    24,
      25,    26,    27,    28,    29,    11,    31,    13,    21,    21,
      19,    17,    19,     8,    20,    11,    11,    13,    13,    44,
      17,    17,    17,    29,    20,    20,    45,     1,    34,    34,
      34,    45,    27,    29,    29,    55,     8,     8,    34,    11,
      11,    13,    13,     1,     1,    17,    17,     8,    20,    20,
      11,   347,    13,   249,   356,    27,    17,    29,    29,    20,
      11,    88,    13,    32,    75,   116,    17,    70,    29,    20,
      11,   333,    13,    24,    25,    26,    17,   179,    29,    20,
      31,   179,    46,    24,    25,    26,   139,    11,    29,    13,
      31,   360,    11,    17,    13,    31,    20,    11,    17,    13,
      -1,    20,    -1,    17,    -1,    29,    20,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    29
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    49,     0,     1,     3,     5,     6,     7,    10,    12,
      23,    30,    50,    51,    52,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    71,    72,    73,
      74,    75,    76,    77,    78,    82,    83,    85,    86,   138,
     139,   140,   141,    17,   121,     4,    11,    13,    17,    20,
      29,    53,    54,    63,    66,   121,   124,   125,   126,   127,
     128,   129,   135,   138,   139,    17,     8,    17,    84,   106,
     123,   124,    18,     1,    17,    87,    88,    89,    90,    93,
      18,    36,    17,    47,   134,   136,     1,     8,     9,    17,
      18,    22,    24,    25,    26,    27,    28,    29,    31,    44,
      52,    56,    63,    94,    95,    96,    97,    98,    99,   100,
     102,   103,   104,   105,   106,   118,   122,   123,    18,    36,
      37,   136,   134,   136,    94,    18,    36,   136,   104,   122,
     121,   124,   121,   124,   121,   124,    40,    79,    53,    63,
      17,   127,    20,    40,   130,    20,    13,    20,   130,    20,
      33,   121,   136,    13,    17,    29,   133,    54,   121,   124,
     138,   139,    18,    46,    32,   120,    13,    13,    17,   121,
     129,   121,   129,    20,    36,   136,   124,    11,    15,    18,
      21,   107,   108,   109,   136,    33,   110,    17,    84,    35,
      88,    18,    38,    21,    36,    18,    16,   136,    31,    44,
     118,    98,   104,    13,    17,    37,    37,    37,   110,     3,
      27,   106,   123,    33,    44,   104,   118,    29,   118,   136,
      35,    95,    38,   136,   107,   110,    17,    34,   111,   112,
     113,   114,   124,    17,    27,    33,    99,   100,   101,   105,
      36,    18,    17,    24,    25,    26,    31,    67,    68,    69,
      70,   124,   137,    18,   136,    18,    35,    36,    18,    18,
      18,     3,     4,    39,    80,    81,   124,   133,    13,    17,
      13,    14,    17,   131,   132,    13,    17,   131,    32,    32,
      16,   136,   136,   136,    16,   136,    13,   121,   129,   121,
     129,   121,    32,    17,    15,    18,    21,   108,   109,   136,
     136,    34,   112,   107,   136,    14,    15,    17,    91,    92,
     104,   118,   111,    20,    20,   107,    17,   123,   110,   110,
      27,   106,   136,   118,   111,   111,    17,   100,    37,   107,
     115,   119,    34,    38,    45,    21,    17,    33,    33,   110,
      32,   136,    16,    21,   110,   107,   130,    38,    17,    70,
     124,    18,   121,   121,     8,    65,    38,    39,   121,   136,
      38,    39,    39,   121,   121,   136,   136,   136,    36,   136,
      15,   136,    34,    41,    42,    43,   111,   107,   115,    17,
      27,    17,    27,   136,   136,   107,   107,   110,   110,   111,
     107,   115,   107,   107,    13,    17,    18,   116,   117,   113,
      45,    16,    32,    34,   107,    17,   136,   136,   136,   131,
      68,    21,    21,    81,     8,    65,    21,   132,    34,    92,
      92,    92,   107,   107,   136,   136,   136,   136,   107,   107,
     107,   107,    19,    19,    38,   117,    45,    17,   110,    34,
      39,   124,   124,   124,   110,   117,    34,   107,   136,   110
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
#line 158 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 19:
#line 160 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(3) - (5)].typ); (yyvsp[(3) - (5)].typ)->name = (yyvsp[(4) - (5)].chr); mta->type_stack.Pop(); }
    break;

  case 20:
#line 164 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); if((yyvsp[(1) - (3)].typ)) { (yyvsp[(2) - (3)].typ)->AddParent((yyvsp[(1) - (3)].typ)); (yyvsp[(2) - (3)].typ)->AssignType((yyvsp[(1) - (3)].typ)->type); }
	    mta->type_stack.Pop(); }
    break;

  case 21:
#line 167 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (4)].typ); if((yyvsp[(1) - (4)].typ)) { (yyvsp[(2) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(2) - (4)].typ)->AssignType((yyvsp[(1) - (4)].typ)->type); }
	    mta->type_stack.Pop(); (yyval.typ)->SetType(TypeDef::ARRAY); }
    break;

  case 22:
#line 170 "mta_parse.y"
    {
            /* annoying place for a comment: prevents structype, so use plain type */
            (yyval.typ) = (yyvsp[(3) - (4)].typ); if((yyvsp[(1) - (4)].typ)) { (yyvsp[(3) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(3) - (4)].typ)->AssignType((yyvsp[(1) - (4)].typ)->type); }
	    mta->type_stack.Pop(); }
    break;

  case 23:
#line 176 "mta_parse.y"
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
#line 185 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (7)].typ); (yyval.typ)->AssignType(TypeDef::FUN_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 25:
#line 188 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (4)].typ); (yyval.typ)->AssignType(TypeDef::METH_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 26:
#line 191 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 27:
#line 193 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (4)].typ); (yyval.typ)->SetType(TypeDef::ARRAY); }
    break;

  case 28:
#line 195 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 31:
#line 203 "mta_parse.y"
    {
            TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
            (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("enum", sp, (yyval.typ)); (yyval.typ)->source_end = mta->line-1; } }
    break;

  case 34:
#line 213 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); }
    break;

  case 35:
#line 214 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 36:
#line 217 "mta_parse.y"
    {
  	    (yyval.typ) = (yyvsp[(2) - (2)].typ);
            /* mta->PushState(MTA::Parse_enum); -- MP_ENUM triggers in mta_lex.cpp */
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::ENUM); mta->cur_enum = (yyvsp[(2) - (2)].typ);
	    mta->SetSource((yyval.typ), false); mta->type_stack.Pop(); }
    break;

  case 37:
#line 222 "mta_parse.y"
    {
            /* mta->PushState(MTA::Parse_enum); -- MP_ENUM triggers in mta_lex.cpp */
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->cur_enum = (yyval.typ);
	    mta->SetSource((yyval.typ), false); (yyval.typ)->AssignType(TypeDef::ENUM); }
    break;

  case 38:
#line 230 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("class", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 39:
#line 238 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 40:
#line 239 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 41:
#line 243 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("class", sp, (yyval.typ)); mta->FixClassTypes((yyval.typ));
              (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 44:
#line 257 "mta_parse.y"
    {
	    if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	    else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 45:
#line 263 "mta_parse.y"
    {
	    (yyvsp[(1) - (2)].typ)->tokens.keep = true; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (2)].typ), true); }
    break;

  case 46:
#line 266 "mta_parse.y"
    {
            SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs();
            mta->SetSource((yyvsp[(1) - (3)].typ), true); }
    break;

  case 47:
#line 269 "mta_parse.y"
    {
            SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (3)].typ), true); 
	  }
    break;

  case 48:
#line 276 "mta_parse.y"
    { mta->PushState(MTA::Parse_inclass); }
    break;

  case 49:
#line 277 "mta_parse.y"
    { mta->PushState(MTA::Parse_inclass); }
    break;

  case 50:
#line 280 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::prvt);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::CLASS); mta->ClearSource((yyvsp[(2) - (2)].typ));
            /* tyname set -- premature */ }
    break;

  case 51:
#line 284 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::prvt); }
    break;

  case 52:
#line 286 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::CLASS); 
            mta->type_stack.Push((yyval.typ));
	    mta->PushClass((yyval.typ), MTA::prvt); }
    break;

  case 53:
#line 291 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::STRUCT);
            mta->type_stack.Push((yyval.typ));
	    mta->PushClass((yyval.typ), MTA::pblc); }
    break;

  case 54:
#line 296 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::pblc);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::STRUCT); mta->ClearSource((yyvsp[(2) - (2)].typ));
            /* tyname set -- premature */ }
    break;

  case 55:
#line 300 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(1) - (2)].typ); mta->PushClass((yyvsp[(1) - (2)].typ), MTA::pblc); }
    break;

  case 56:
#line 302 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::UNION);
            mta->type_stack.Push((yyval.typ));
	    mta->PushClass((yyval.typ), MTA::pblc); }
    break;

  case 57:
#line 307 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ), MTA::pblc);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::UNION); mta->ClearSource((yyvsp[(2) - (2)].typ));
            /* tyname set -- premature */ }
    break;

  case 58:
#line 311 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(1) - (2)].typ); mta->PushClass((yyvsp[(1) - (2)].typ), MTA::pblc); }
    break;

  case 59:
#line 316 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].typ) != NULL) mta->cur_class->AddParent((yyvsp[(1) - (1)].typ)); }
    break;

  case 60:
#line 318 "mta_parse.y"
    {
            if((yyvsp[(3) - (3)].typ) != NULL) { mta->cur_class->AddParent((yyvsp[(3) - (3)].typ));
	      mta->cur_class->AddOption("MULT_INHERIT"); } }
    break;

  case 62:
#line 325 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 63:
#line 326 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 64:
#line 327 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 65:
#line 328 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 70:
#line 340 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ)); }
	    mta->type_stack.Pop(); }
    break;

  case 71:
#line 348 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 72:
#line 349 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 73:
#line 353 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ));
              mta->FixClassTypes((yyval.typ));
	      (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 74:
#line 363 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 75:
#line 364 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 76:
#line 365 "mta_parse.y"
    { mta->Burp(); 
            // todo: could create the template based on function name, and create a 
            // methoddef to hold it.  but we don't care.. :)
            }
    break;

  case 77:
#line 369 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 78:
#line 370 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (4)].typ),(yyvsp[(4) - (4)].chr)); }
    break;

  case 79:
#line 371 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr));
          // todo: could create the template based on function name, and create a 
          // methoddef to hold it.  but we don't care.. :)
          }
    break;

  case 80:
#line 378 "mta_parse.y"
    {
          if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	  else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 82:
#line 385 "mta_parse.y"
    {
            mta->PushState(MTA::Parse_inclass); (yyvsp[(1) - (2)].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 83:
#line 388 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs(); }
    break;

  case 84:
#line 390 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs(); }
    break;

  case 85:
#line 395 "mta_parse.y"
    {
	    (yyvsp[(5) - (5)].typ)->templ_pars.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_pars.Duplicate(mta->cur_templ_pars);
	    (yyvsp[(5) - (5)].typ)->templ_defs.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_defs.Duplicate(mta->cur_templ_defs);
            mta->EndTemplPars();
	    (yyvsp[(5) - (5)].typ)->SetType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(5) - (5)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 86:
#line 403 "mta_parse.y"
    {
	    (yyvsp[(4) - (4)].typ)->templ_pars.Reset();
	    (yyvsp[(4) - (4)].typ)->templ_defs.Reset();
            mta->EndTemplPars();
	    (yyvsp[(4) - (4)].typ)->SetType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(4) - (4)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 87:
#line 412 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(1) - (4)].typ);
            mta->PushState(MTA::Parse_fundef);
            mta->EndTemplPars();
          }
    break;

  case 88:
#line 417 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(1) - (5)].typ);
            mta->PushState(MTA::Parse_fundef);
            mta->EndTemplPars();
          }
    break;

  case 89:
#line 423 "mta_parse.y"
    { mta->StartTemplPars(); }
    break;

  case 91:
#line 428 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 92:
#line 432 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 93:
#line 433 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 94:
#line 434 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 95:
#line 437 "mta_parse.y"
    {
            mta->cur_templ_pars.Link((yyvsp[(2) - (4)].typ)); (yyval.typ) = (yyvsp[(2) - (4)].typ);
            mta->cur_templ_defs.Link((yyvsp[(4) - (4)].typ));
          }
    break;

  case 96:
#line 441 "mta_parse.y"
    {
            mta->cur_templ_pars.Link((yyvsp[(2) - (4)].typ)); (yyval.typ) = (yyvsp[(2) - (4)].typ);
            mta->cur_templ_defs.Link((yyvsp[(4) - (4)].typ));
          }
    break;

  case 97:
#line 445 "mta_parse.y"
    {
            mta->cur_templ_pars.Link((yyvsp[(2) - (4)].typ)); (yyval.typ) = (yyvsp[(2) - (4)].typ);
            mta->cur_templ_defs.Link((yyvsp[(4) - (4)].typ));
          }
    break;

  case 98:
#line 451 "mta_parse.y"
    {
            if(mta->cur_is_trg) { /* only add reg_funs in target space */
              TypeDef* nt = new TypeDef((yyvsp[(1) - (1)].meth)->name, TypeDef::FUNCTION,0,0);
              mta->SetSource(nt, false);
              taMisc::types.Add(nt);
              nt->methods.AddUniqNameNew((yyvsp[(1) - (1)].meth));
              taMisc::reg_funs.Link(nt); }
            mta->meth_stack.Pop(); }
    break;

  case 99:
#line 461 "mta_parse.y"
    {
            (yyvsp[(3) - (4)].meth)->type = (yyvsp[(2) - (4)].typ); SETDESC((yyvsp[(3) - (4)].meth),(yyvsp[(4) - (4)].chr)); (yyval.meth) = (yyvsp[(3) - (4)].meth); }
    break;

  case 100:
#line 463 "mta_parse.y"
    {
            (yyvsp[(2) - (3)].meth)->type = &TA_int; SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); (yyval.meth) = (yyvsp[(2) - (3)].meth); }
    break;

  case 101:
#line 467 "mta_parse.y"
    {
            (yyvsp[(1) - (2)].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = true; }
    break;

  case 102:
#line 472 "mta_parse.y"
    { /* using is not parsed */
            mta->Namespc_PushNew((yyvsp[(2) - (3)].chr));
          }
    break;

  case 103:
#line 475 "mta_parse.y"
    { /* using is not parsed */
            /* todo: first name is not retained */
            mta->Namespc_PushNew((yyvsp[(4) - (5)].chr));
          }
    break;

  case 104:
#line 481 "mta_parse.y"
    {
            mta->Namespc_PushNew((yyvsp[(2) - (3)].chr));
          }
    break;

  case 105:
#line 484 "mta_parse.y"
    {
            /* todo: first name is not retained */
            mta->Namespc_PushNew((yyvsp[(4) - (5)].chr));
          }
    break;

  case 109:
#line 495 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 112:
#line 500 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 113:
#line 505 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 114:
#line 508 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm));
            if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
            else (yyvsp[(1) - (3)].enm)->AddOption("#IGNORE"); // ignore bad math!
	    mta->enum_stack.Pop(); }
    break;

  case 116:
#line 517 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 117:
#line 520 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 118:
#line 523 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) | (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 119:
#line 529 "mta_parse.y"
    {
            EnumDef* ed = mta->cur_enum->enum_vals.FindName((yyvsp[(1) - (1)].chr));
            if(ed) (yyval.rval) = ed->enum_no;
            else   (yyval.rval) = -424242; }
    break;

  case 121:
#line 534 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 122:
#line 538 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 123:
#line 541 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 124:
#line 542 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 125:
#line 545 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].memb) != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->HasOption("IGNORE"))
		 && !((yyvsp[(1) - (1)].memb)->type->IsConst())) {
		mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
                mta->Info(3, "member:", (yyvsp[(1) - (1)].memb)->name, "added to class:",
                          mta->cur_class->name); } }
	    mta->memb_stack.Pop(); (yyval.typ) = NULL; }
    break;

  case 126:
#line 553 "mta_parse.y"
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

  case 127:
#line 565 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
            mta->TypeAdded("enum", &(mta->cur_class->sub_types), (yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->source_end = mta->line-1;
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 128:
#line 571 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 129:
#line 575 "mta_parse.y"
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

  case 130:
#line 585 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 132:
#line 590 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 133:
#line 591 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 134:
#line 592 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 135:
#line 593 "mta_parse.y"
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

  case 137:
#line 613 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 138:
#line 619 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 139:
#line 620 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 140:
#line 621 "mta_parse.y"
    { }
    break;

  case 141:
#line 622 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
            int spsz = sp->size;
            TypeDef* nty = (yyvsp[(1) - (4)].typ)->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    (yyvsp[(2) - (4)].memb)->type = nty; }
    break;

  case 142:
#line 629 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 143:
#line 633 "mta_parse.y"
    {
	     if(mta->cur_mstate == MTA::pblc)
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 144:
#line 637 "mta_parse.y"
    {
	     if(mta->cur_mstate == MTA::pblc)
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 145:
#line 643 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 146:
#line 649 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 148:
#line 655 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 149:
#line 656 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 150:
#line 657 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 151:
#line 658 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 152:
#line 659 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 153:
#line 660 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 154:
#line 661 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 155:
#line 662 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 156:
#line 663 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 157:
#line 664 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 158:
#line 665 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 159:
#line 666 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 160:
#line 667 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 161:
#line 668 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 162:
#line 669 "mta_parse.y"
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

  case 163:
#line 679 "mta_parse.y"
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

  case 164:
#line 689 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 165:
#line 690 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 166:
#line 691 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 167:
#line 692 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 169:
#line 697 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 170:
#line 698 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 171:
#line 700 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 172:
#line 705 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 173:
#line 706 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
    break;

  case 174:
#line 707 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 175:
#line 708 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 176:
#line 709 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 177:
#line 712 "mta_parse.y"
    {
             (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 178:
#line 719 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 179:
#line 723 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 180:
#line 724 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
    break;

  case 181:
#line 725 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 182:
#line 726 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 183:
#line 727 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 184:
#line 728 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 185:
#line 732 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 186:
#line 733 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 187:
#line 734 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
    break;

  case 188:
#line 738 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 189:
#line 739 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 190:
#line 740 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 191:
#line 743 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 192:
#line 744 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 193:
#line 747 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 194:
#line 748 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 195:
#line 751 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 196:
#line 752 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 197:
#line 753 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 198:
#line 756 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 199:
#line 758 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      String ad = (yyvsp[(2) - (2)].chr);
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 200:
#line 769 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 201:
#line 773 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 202:
#line 777 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 203:
#line 781 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 204:
#line 785 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 212:
#line 806 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 213:
#line 810 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 214:
#line 813 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 215:
#line 814 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 216:
#line 817 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
                                  mta->SetSource((yyval.typ), false); }
    break;

  case 217:
#line 821 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 219:
#line 826 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 220:
#line 827 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 222:
#line 831 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ)->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, (yyval.typ)); }
	  }
    break;

  case 224:
#line 841 "mta_parse.y"
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

  case 226:
#line 853 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(2) - (2)].typ)->GetConstType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("const", sp, (yyval.typ)); }
	  }
    break;

  case 228:
#line 862 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
	    (yyval.typ) = td; }
    break;

  case 229:
#line 869 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
            (yyval.typ) = td; }
    break;

  case 230:
#line 876 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 231:
#line 877 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 232:
#line 878 "mta_parse.y"
    {
            TypeDef* nty = new TypeDef((yyvsp[(2) - (2)].chr)); mta->SetSource(nty, false);
	    TypeSpace* sp = mta->GetTypeSpace(nty);
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("namespace new", sp, (yyval.typ));
              (yyval.typ)->namespc = mta->cur_nmspc_tmp; }
	    else { mta->TypeNotAdded("namespace new", sp, (yyval.typ), nty); delete nty; }
          }
    break;

  case 233:
#line 886 "mta_parse.y"
    {
            mta->Info(2, "namespace type:", mta->cur_nmspc_tmp, "::", (yyvsp[(2) - (2)].typ)->name);
            (yyvsp[(2) - (2)].typ)->namespc = mta->cur_nmspc_tmp; /* todo: could check.. */
            (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 234:
#line 890 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 236:
#line 892 "mta_parse.y"
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
              (yyval.typ) = sp->AddUniqNameOld(td);
              if((yyval.typ) == td) mta->TypeAdded("template instance", sp, (yyval.typ)); }
            else
              (yyval.typ) = td; }
    break;

  case 237:
#line 908 "mta_parse.y"
    { /* this template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
              mta->Warning(1, "Type:", (yyvsp[(1) - (4)].typ)->name, "used as template but not marked as such",
                           "-- now marking -- probably just an internal type");
              (yyvsp[(1) - (4)].typ)->SetType(TypeDef::TEMPLATE); }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 238:
#line 916 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::STRUCT);
          (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 239:
#line 918 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::STRUCT); }
    break;

  case 240:
#line 919 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::UNION);
            (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 241:
#line 921 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::UNION); }
    break;

  case 243:
#line 925 "mta_parse.y"
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

  case 244:
#line 938 "mta_parse.y"
    {
           mta->cur_typ_templ_pars.Reset(); }
    break;

  case 246:
#line 944 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 247:
#line 949 "mta_parse.y"
    {
            mta->cur_typ_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 248:
#line 951 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 249:
#line 953 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval));
            mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 251:
#line 959 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 252:
#line 960 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 255:
#line 967 "mta_parse.y"
    { mta->cur_nmspc_tmp = (yyvsp[(1) - (2)].chr); (yyval.chr) = mta->cur_nmspc_tmp; }
    break;

  case 260:
#line 979 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 261:
#line 986 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 262:
#line 993 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 263:
#line 1000 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 3351 "y.tab.c"
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


#line 1003 "mta_parse.y"


	/* end of grammar */

void yyerror(const char *s) { 	/* called for yacc syntax error */
  if(strcmp(s, "parse error") == 0) {
    mta->Error(1, "Syntax Error, line:", String(mta->st_line), ":");
    mta->Error(1, mta->LastLn);
    String loc;
    for(int i=0; i < mta->st_col + 1; i++)
      loc << " ";
    loc << "^";
    mta->Error(1, loc);
  }
  else {
    mta->Error(1, s, "line:", String(mta->st_line), ":");
    mta->Error(1, mta->LastLn);
  }
}

