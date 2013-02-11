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
#define YYLAST   706

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  48
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  93
/* YYNRULES -- Number of rules.  */
#define YYNRULES  261
/* YYNRULES -- Number of states.  */
#define YYNSTATES  443

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
      33,    35,    32,    41,    38,    42,    45,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    47,
      40,     2,    39,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    36,    43,    34,    44,     2,     2,     2,
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
      25,    28,    31,    34,    37,    39,    41,    44,    47,    52,
      56,    62,    66,    71,    76,    79,    87,    92,    96,   101,
     105,   107,   112,   115,   119,   123,   126,   128,   130,   133,
     137,   139,   142,   146,   150,   153,   157,   161,   163,   167,
     170,   173,   175,   177,   180,   183,   185,   187,   191,   193,
     196,   198,   203,   206,   208,   211,   213,   215,   217,   220,
     224,   226,   229,   233,   236,   240,   245,   249,   253,   256,
     259,   263,   267,   273,   278,   283,   289,   291,   293,   297,
     300,   303,   306,   310,   314,   318,   320,   325,   329,   332,
     336,   342,   346,   352,   354,   357,   359,   362,   364,   367,
     369,   371,   375,   377,   381,   385,   389,   391,   393,   395,
     397,   399,   402,   404,   406,   408,   410,   413,   415,   417,
     420,   423,   426,   428,   430,   433,   437,   442,   445,   450,
     455,   457,   461,   463,   468,   470,   474,   479,   484,   489,
     495,   501,   506,   512,   518,   524,   529,   533,   538,   543,
     549,   554,   560,   566,   572,   578,   580,   583,   586,   590,
     594,   597,   602,   609,   613,   616,   618,   620,   622,   625,
     628,   631,   635,   637,   640,   644,   646,   648,   651,   654,
     658,   660,   663,   665,   669,   674,   676,   679,   681,   684,
     688,   695,   697,   700,   702,   704,   707,   711,   714,   717,
     720,   722,   724,   727,   729,   731,   733,   736,   738,   740,
     743,   745,   748,   750,   753,   755,   757,   759,   762,   765,
     769,   773,   777,   781,   784,   787,   790,   792,   797,   802,
     805,   808,   810,   813,   815,   819,   821,   823,   825,   827,
     829,   831,   833,   836,   839,   841,   843,   845,   847,   849,
     851,   853
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      49,     0,    -1,    -1,    49,    50,    -1,    49,    54,    -1,
      49,    58,    -1,    49,    60,    -1,    49,    70,    -1,    49,
      72,    -1,    49,    81,    -1,    49,    84,    -1,    49,    85,
      -1,    49,    34,    -1,    49,     1,    -1,    51,    -1,    52,
      -1,    52,    18,    -1,    12,    53,    -1,    12,    62,   132,
     135,    -1,    12,     4,    53,    -1,    12,     4,    62,   132,
     135,    -1,   123,   122,   135,    -1,   123,   122,    16,   135,
      -1,   127,    18,   122,   135,    -1,   123,   135,    -1,   123,
      33,    32,   122,    35,   109,   135,    -1,   123,    20,    32,
     122,    -1,   122,   122,   135,    -1,   122,   122,    16,   135,
      -1,   122,   123,   135,    -1,    55,    -1,    56,    86,    34,
     135,    -1,    57,    36,    -1,    57,    18,    36,    -1,    57,
      36,    18,    -1,     7,   122,    -1,     7,    -1,    59,    -1,
      65,   135,    -1,    65,   135,    18,    -1,    61,    -1,    62,
     135,    -1,    62,   133,   135,    -1,    63,    93,    34,    -1,
      64,    36,    -1,    64,    18,    36,    -1,    64,    36,    18,
      -1,    65,    -1,    65,    37,    66,    -1,   139,   122,    -1,
     139,   123,    -1,   139,    -1,   137,    -1,   137,   122,    -1,
     137,   127,    -1,   138,    -1,    67,    -1,    66,    38,    67,
      -1,   123,    -1,    68,   123,    -1,    17,    -1,    17,    78,
     130,    39,    -1,    68,    17,    -1,    69,    -1,    68,    69,
      -1,   136,    -1,    31,    -1,    71,    -1,    76,   135,    -1,
      76,   135,    18,    -1,    73,    -1,    74,   135,    -1,    74,
     133,   135,    -1,    77,   103,    -1,    74,   135,    18,    -1,
      74,   133,   135,    18,    -1,    77,   103,    18,    -1,    75,
      93,    34,    -1,    76,   135,    -1,    76,    36,    -1,    76,
      18,    36,    -1,    76,    36,    18,    -1,   140,    78,    79,
      39,    64,    -1,   140,    78,    39,    64,    -1,   140,    78,
      39,     8,    -1,   140,    78,    79,    39,     8,    -1,    40,
      -1,    80,    -1,    79,    38,    80,    -1,     3,   122,    -1,
       4,   122,    -1,   123,   122,    -1,     3,   122,    21,    -1,
       4,   122,    21,    -1,   123,   122,    21,    -1,    82,    -1,
      30,   121,    83,   106,    -1,    30,    83,   106,    -1,   105,
     109,    -1,    23,    17,   135,    -1,    23,    17,    20,    17,
     135,    -1,    23,    17,    36,    -1,    23,    17,    20,    17,
      36,    -1,    87,    -1,    86,    87,    -1,    88,    -1,    88,
      18,    -1,    89,    -1,    89,    38,    -1,     1,    -1,    92,
      -1,    92,    21,    90,    -1,    91,    -1,    90,    41,    91,
      -1,    90,    42,    91,    -1,    90,    43,    91,    -1,    17,
      -1,    14,    -1,    15,    -1,    17,    -1,    94,    -1,    93,
      94,    -1,    95,    -1,   101,    -1,    55,    -1,    52,    -1,
      62,   135,    -1,     1,    -1,    96,    -1,    24,    37,    -1,
      25,    37,    -1,    26,    37,    -1,    18,    -1,    97,    -1,
       9,    97,    -1,   120,    98,   135,    -1,   120,    99,    21,
     135,    -1,    98,   135,    -1,   120,    99,    16,   135,    -1,
     120,   100,   109,   135,    -1,    99,    -1,    98,    38,    99,
      -1,    17,    -1,    33,    32,    17,    35,    -1,   102,    -1,
     117,   110,   106,    -1,     8,   117,   110,   106,    -1,    31,
     117,   110,   106,    -1,   117,   110,   114,   106,    -1,     8,
     117,   110,   114,   106,    -1,    31,   117,   110,   114,   106,
      -1,    44,   117,   110,   106,    -1,     8,    44,   117,   110,
     106,    -1,    31,    44,   117,   110,   106,    -1,    28,   121,
      27,   109,   106,    -1,    28,    27,   109,   106,    -1,    28,
     121,   135,    -1,    28,     3,   121,   135,    -1,    28,     3,
      17,   135,    -1,    28,   121,   105,   109,   106,    -1,    28,
     105,   109,   106,    -1,    22,    17,    20,    27,   135,    -1,
      22,    17,    20,    17,   135,    -1,    22,    13,    20,    27,
     135,    -1,    22,    13,    20,    17,   135,    -1,   103,    -1,
       9,   103,    -1,    31,   103,    -1,     8,    31,   103,    -1,
     120,   104,   106,    -1,   104,   106,    -1,   120,    27,   109,
     106,    -1,   120,    27,    33,    35,   109,   106,    -1,    27,
     109,   106,    -1,   105,   109,    -1,    17,    -1,   135,    -1,
     107,    -1,   108,   135,    -1,    18,   135,    -1,    18,   107,
      -1,    18,   108,   135,    -1,    15,    -1,    11,    15,    -1,
      11,    18,    15,    -1,    21,    -1,    11,    -1,    11,    21,
      -1,    33,    35,    -1,    33,   111,    35,    -1,    35,    -1,
     111,    35,    -1,   112,    -1,   111,    38,   112,    -1,   111,
      45,    45,    45,    -1,   113,    -1,   113,    21,    -1,   123,
      -1,   123,    17,    -1,   123,    17,    16,    -1,   123,    33,
      32,    17,    35,   109,    -1,    17,    -1,   118,   115,    -1,
     116,    -1,    18,    -1,   115,   116,    -1,   115,    38,   116,
      -1,    13,    19,    -1,    17,    19,    -1,    29,    33,    -1,
      37,    -1,    32,    -1,   119,    32,    -1,   121,    -1,   123,
      -1,     8,    -1,     8,   123,    -1,    17,    -1,   124,    -1,
     124,    46,    -1,   125,    -1,   125,   119,    -1,   126,    -1,
      11,   126,    -1,   127,    -1,   128,    -1,   129,    -1,   138,
     129,    -1,   138,   122,    -1,    13,    20,    17,    -1,    29,
      20,    17,    -1,    13,    20,    13,    -1,    29,    20,    13,
      -1,   134,    17,    -1,   134,    13,    -1,    20,    13,    -1,
      29,    -1,    13,    78,   130,    39,    -1,    29,    78,   130,
      39,    -1,   137,   129,    -1,   137,   122,    -1,    13,    -1,
     129,    13,    -1,   131,    -1,   130,    38,   131,    -1,    13,
      -1,    17,    -1,    14,    -1,    17,    -1,    13,    -1,    29,
      -1,    17,    -1,    17,    16,    -1,    17,    20,    -1,    47,
      -1,    24,    -1,    25,    -1,    26,    -1,     5,    -1,     6,
      -1,     3,    -1,    10,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   113,   113,   114,   117,   119,   122,   125,   128,   131,
     133,   135,   137,   140,   144,   155,   156,   160,   161,   163,
     165,   169,   172,   175,   181,   190,   193,   196,   198,   200,
     204,   210,   213,   214,   215,   218,   223,   231,   239,   240,
     244,   253,   254,   258,   264,   267,   270,   277,   278,   281,
     286,   290,   297,   304,   309,   313,   323,   325,   332,   333,
     334,   335,   336,   339,   340,   343,   344,   348,   356,   357,
     361,   371,   372,   373,   377,   378,   379,   386,   389,   393,
     396,   398,   403,   408,   415,   419,   424,   428,   429,   433,
     434,   435,   437,   438,   439,   442,   452,   454,   458,   463,
     466,   472,   475,   481,   482,   485,   486,   489,   490,   491,
     496,   499,   507,   508,   511,   514,   520,   524,   525,   529,
     532,   533,   536,   545,   558,   564,   568,   573,   577,   578,
     579,   580,   581,   601,   602,   608,   609,   610,   611,   618,
     622,   626,   632,   638,   643,   644,   645,   646,   647,   648,
     649,   650,   651,   652,   653,   654,   655,   656,   657,   658,
     668,   678,   679,   680,   681,   685,   686,   687,   689,   694,
     695,   696,   697,   698,   701,   708,   712,   713,   714,   715,
     716,   717,   721,   722,   723,   727,   728,   729,   732,   733,
     736,   737,   740,   741,   742,   745,   747,   758,   762,   766,
     770,   774,   780,   784,   785,   786,   787,   790,   791,   795,
     799,   802,   803,   806,   809,   810,   811,   814,   818,   819,
     828,   829,   840,   841,   849,   850,   853,   854,   856,   857,
     864,   871,   872,   873,   881,   887,   888,   889,   906,   915,
     917,   920,   921,   935,   936,   941,   942,   943,   947,   948,
     949,   952,   953,   956,   959,   962,   963,   964,   968,   972,
     976,   980
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
  "'<'", "'+'", "'-'", "'|'", "'~'", "'.'", "'&'", "';'", "$accept",
  "list", "typedefn", "typedefns", "typedsub", "defn", "enumdefn",
  "enumdsub", "enumname", "enumnm", "classdecl", "classdecls", "classdefn",
  "classdefns", "classdsub", "classname", "classhead", "classnm",
  "classinh", "classpar", "classptyp", "classpmod", "templdecl",
  "templdecls", "templdefn", "templdefns", "templdsub", "templname",
  "templhead", "templfun", "templopen", "templpars", "templpar", "fundecl",
  "funnm", "regfundefn", "usenamespc", "namespc", "enums", "enumline",
  "enumitm", "enumitms", "enummath", "enummathitm", "enmitmname", "membs",
  "membline", "membdefn", "basicmemb", "nostatmemb", "membnames",
  "membname", "membfunp", "methdefn", "basicmeth", "nostatmeth",
  "mbfundefn", "methname", "fundefn", "funsubdefn", "funsubdecl",
  "funargs", "constfun", "args", "argdefn", "subargdefn", "constrlist",
  "constitms", "constref", "consthsnm", "constcoln", "ptrs", "membtype",
  "ftype", "tyname", "type", "noreftype", "constype", "subtype",
  "nssubtype", "structype", "combtype", "templargs", "templarg", "tdname",
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
     285,   286,    42,    40,   125,    41,   123,    58,    44,    62,
      60,    43,    45,   124,   126,    46,    38,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    48,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    50,    51,    51,    52,    52,    52,
      52,    53,    53,    53,    53,    53,    53,    53,    53,    53,
      54,    55,    56,    56,    56,    57,    57,    58,    59,    59,
      60,    61,    61,    62,    63,    63,    63,    64,    64,    65,
      65,    65,    65,    65,    65,    65,    66,    66,    67,    67,
      67,    67,    67,    68,    68,    69,    69,    70,    71,    71,
      72,    73,    73,    73,    73,    73,    73,    74,    74,    75,
      75,    75,    76,    76,    77,    77,    78,    79,    79,    80,
      80,    80,    80,    80,    80,    81,    82,    82,    83,    84,
      84,    85,    85,    86,    86,    87,    87,    88,    88,    88,
      89,    89,    90,    90,    90,    90,    91,    91,    91,    92,
      93,    93,    94,    94,    94,    94,    94,    94,    95,    95,
      95,    95,    95,    96,    96,    97,    97,    97,    97,    97,
      98,    98,    99,   100,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   102,   102,   102,   102,   103,
     103,   103,   103,   103,   104,   105,   106,   106,   106,   106,
     106,   106,   107,   107,   107,   108,   108,   108,   109,   109,
     110,   110,   111,   111,   111,   112,   112,   113,   113,   113,
     113,   113,   114,   115,   115,   115,   115,   116,   116,   117,
     118,   119,   119,   120,   121,   121,   121,   122,   123,   123,
     124,   124,   125,   125,   126,   126,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   128,
     128,   129,   129,   130,   130,   131,   131,   131,   132,   132,
     132,   133,   133,   134,   135,   136,   136,   136,   137,   138,
     139,   140
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     1,     2,     2,     4,     3,
       5,     3,     4,     4,     2,     7,     4,     3,     4,     3,
       1,     4,     2,     3,     3,     2,     1,     1,     2,     3,
       1,     2,     3,     3,     2,     3,     3,     1,     3,     2,
       2,     1,     1,     2,     2,     1,     1,     3,     1,     2,
       1,     4,     2,     1,     2,     1,     1,     1,     2,     3,
       1,     2,     3,     2,     3,     4,     3,     3,     2,     2,
       3,     3,     5,     4,     4,     5,     1,     1,     3,     2,
       2,     2,     3,     3,     3,     1,     4,     3,     2,     3,
       5,     3,     5,     1,     2,     1,     2,     1,     2,     1,
       1,     3,     1,     3,     3,     3,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     2,     1,     1,     2,
       2,     2,     1,     1,     2,     3,     4,     2,     4,     4,
       1,     3,     1,     4,     1,     3,     4,     4,     4,     5,
       5,     4,     5,     5,     5,     4,     3,     4,     4,     5,
       4,     5,     5,     5,     5,     1,     2,     2,     3,     3,
       2,     4,     6,     3,     2,     1,     1,     1,     2,     2,
       2,     3,     1,     2,     3,     1,     1,     2,     2,     3,
       1,     2,     1,     3,     4,     1,     2,     1,     2,     3,
       6,     1,     2,     1,     1,     2,     3,     2,     2,     2,
       1,     1,     2,     1,     1,     1,     2,     1,     1,     2,
       1,     2,     1,     2,     1,     1,     1,     2,     2,     3,
       3,     3,     3,     2,     2,     2,     1,     4,     4,     2,
       2,     1,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     1,    13,   260,   258,   259,    36,   261,     0,
       0,     0,    12,     3,    14,    15,     4,    30,     0,     0,
       5,    37,     6,    40,     0,     0,     0,    47,     7,    67,
       8,    70,     0,     0,     0,     0,     9,    95,    10,    11,
      52,    55,    51,     0,   217,    35,     0,     0,   241,   217,
       0,   236,    17,     0,    47,     0,     0,   218,   220,   222,
     224,   225,   226,     0,    52,    55,     0,   215,   175,     0,
       0,     0,   214,   224,     0,     0,    16,   109,   119,     0,
     103,   105,   107,   110,     0,    32,   251,   254,     0,    41,
     127,   215,     0,   142,   132,     0,     0,     0,     0,     0,
       0,   236,     0,     0,   125,   124,     0,     0,   120,   122,
     128,   133,     0,   140,   123,   144,   165,     0,     0,     0,
       0,   213,     0,    44,     0,    38,     0,    71,     0,     0,
      79,    68,    73,     0,    53,    54,    49,    50,    86,     0,
      19,     0,     0,   223,     0,     0,   253,   235,     0,     0,
     249,   248,   250,     0,     0,     0,     0,     0,     0,    24,
     219,   211,   221,     0,   242,   234,   233,   240,   239,   241,
     228,   227,     0,   101,    99,   216,   186,   182,     0,   185,
      97,   177,     0,   176,     0,    98,   175,     0,   240,   239,
       0,   104,   106,   108,     0,    33,    34,   252,    42,     0,
       0,     0,   134,   166,     0,     0,   129,   130,   131,     0,
       0,     0,     0,     0,   209,     0,   167,     0,     0,     0,
     126,    43,   121,     0,   137,   170,   174,   201,   190,     0,
       0,   192,   195,   197,   142,     0,     0,     0,   140,     0,
       0,    45,    46,    60,   255,   256,   257,    66,    48,    56,
       0,    63,    58,    65,    39,    72,    74,    77,    80,    81,
      69,    76,     0,     0,     0,     0,    87,     0,     0,   231,
     229,   245,   247,   246,     0,   243,   232,   230,     0,    18,
       0,    27,    29,     0,     0,     0,    21,   212,     0,     0,
     183,     0,   187,   180,     0,   179,   178,   188,     0,    96,
      31,   117,   118,   116,   111,   112,   168,     0,     0,     0,
       0,   173,     0,     0,     0,     0,     0,     0,   156,     0,
       0,     0,   142,   141,   210,   145,     0,     0,   191,     0,
       0,   196,   198,     0,     0,     0,     0,   135,     0,     0,
       0,   169,     0,     0,    62,    64,    59,    75,    89,    90,
      84,    83,     0,     0,    91,    20,     0,   237,   238,    28,
      26,     0,    22,    23,   102,   100,   184,   181,   189,     0,
       0,     0,     0,   146,     0,     0,     0,     0,     0,   158,
     157,   155,   160,     0,     0,     0,   147,     0,   151,   148,
       0,     0,   204,   202,   203,   193,     0,   199,     0,   188,
     171,     0,   138,   136,   139,     0,    57,    92,    93,    88,
      85,    82,    94,   244,     0,   113,   114,   115,   152,   149,
     164,   163,   162,   161,   154,   159,   153,   150,   207,   208,
       0,   205,   194,     0,     0,   143,    61,     0,   206,     0,
     172,    25,   200
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    13,    14,   104,    52,    16,   105,    18,    19,
      20,    21,    22,    23,   106,    25,    26,    54,   248,   249,
     250,   251,    28,    29,    30,    31,    32,    33,    34,    35,
     149,   265,   266,    36,    37,    69,    38,    39,    79,    80,
      81,    82,   304,   305,    83,   107,   108,   109,   110,   111,
     112,   113,   239,   114,   115,   116,   117,   118,   180,   181,
     182,   185,   229,   230,   231,   232,   326,   393,   394,   119,
     327,   162,   120,   121,    55,    72,    57,    58,    59,    73,
      61,    62,   274,   275,   153,    88,    63,   183,   253,    74,
      75,    42,    43
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -373
static const yytype_int16 yypact[] =
{
    -373,   224,  -373,  -373,  -373,  -373,  -373,    14,  -373,   280,
      43,   622,  -373,  -373,  -373,    27,  -373,  -373,    97,   108,
    -373,  -373,  -373,  -373,    68,   448,   201,   234,  -373,  -373,
    -373,  -373,    68,   448,    63,   293,  -373,  -373,  -373,  -373,
     669,  -373,   648,   102,  -373,  -373,   596,   343,   123,   129,
     157,   156,  -373,   222,   148,   648,    80,   182,   155,  -373,
     184,  -373,   208,   246,   669,   290,    72,   661,   129,    28,
     261,   233,  -373,  -373,   290,   290,  -373,  -373,  -373,    62,
    -373,   223,   219,   300,   295,   316,   324,  -373,   299,  -373,
    -373,   166,   609,    15,  -373,   306,   318,   354,   356,   261,
     564,    85,   140,   332,  -373,  -373,   299,   370,  -373,  -373,
    -373,  -373,   100,  -373,  -373,  -373,  -373,    28,   261,   476,
     216,  -373,   364,   391,   515,   398,   299,   400,   414,   369,
     406,    54,   415,   263,  -373,  -373,  -373,  -373,  -373,   203,
    -373,   222,   129,  -373,   357,   389,  -373,  -373,   367,   389,
    -373,  -373,  -373,   299,     8,   299,   403,   405,    10,  -373,
    -373,  -373,   412,    14,  -373,  -373,  -373,   175,   137,  -373,
    -373,   208,   433,  -373,  -373,  -373,   344,  -373,   179,  -373,
    -373,  -373,   299,  -373,   489,  -373,  -373,    28,  -373,   208,
     299,  -373,  -373,  -373,   393,  -373,  -373,  -373,  -373,   293,
     332,   476,  -373,  -373,   432,   447,  -373,  -373,  -373,    28,
     635,   261,   261,    57,  -373,   332,  -373,   476,   436,   476,
    -373,  -373,  -373,   454,  -373,  -373,  -373,   129,  -373,   141,
     217,  -373,   457,   205,   452,   453,   456,   100,   317,   261,
      28,  -373,  -373,   178,  -373,  -373,  -373,  -373,   442,  -373,
     537,  -373,  -373,  -373,  -373,   472,  -373,  -373,  -373,  -373,
    -373,  -373,    14,    14,   339,   238,  -373,    14,   299,  -373,
    -373,  -373,  -373,  -373,   373,  -373,  -373,  -373,   408,  -373,
     299,  -373,  -373,    14,    14,   299,  -373,  -373,   299,    23,
    -373,   482,  -373,  -373,   299,  -373,  -373,  -373,   273,  -373,
    -373,  -373,  -373,  -373,   387,  -373,  -373,   476,   141,   265,
     285,  -373,    46,   299,    28,    28,   261,   261,  -373,   476,
     141,    28,  -373,  -373,  -373,  -373,    28,   312,  -373,   674,
     446,  -373,   483,   466,   502,    28,   484,  -373,   299,   299,
     299,  -373,   389,   515,   129,  -373,  -373,  -373,   491,   493,
    -373,  -373,   583,   361,   495,  -373,   389,  -373,  -373,  -373,
    -373,   468,  -373,  -373,  -373,  -373,  -373,  -373,  -373,   393,
     393,   393,    28,  -373,    28,   299,   299,   299,   299,  -373,
    -373,  -373,  -373,    28,    28,    28,  -373,    28,  -373,  -373,
     485,   498,  -373,    90,  -373,  -373,   465,  -373,   506,   261,
    -373,   490,  -373,  -373,  -373,   445,  -373,  -373,  -373,  -373,
    -373,  -373,  -373,  -373,   261,  -373,  -373,  -373,  -373,  -373,
    -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,
     372,  -373,  -373,   492,    28,  -373,  -373,   299,  -373,   261,
    -373,  -373,  -373
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -373,  -373,  -373,  -373,   528,   487,  -373,   529,  -373,  -373,
    -373,  -373,  -373,  -373,    32,  -373,  -247,   533,  -373,   193,
    -373,   288,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,
     -39,  -373,   195,  -373,  -373,   474,  -373,  -373,  -373,   470,
    -373,  -373,  -373,    93,  -373,   518,   -12,  -373,  -373,   460,
     435,  -100,  -373,  -373,  -373,   -19,   120,   -10,   -48,   375,
     378,   -71,  -183,  -169,   229,  -373,  -199,  -373,  -372,   -80,
    -373,  -373,    -8,    -9,    12,    -2,  -373,  -373,   512,    16,
    -373,   631,  -137,   204,   423,   539,  -373,   -24,  -373,     4,
       5,  -373,  -373
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -227
static const yytype_int16 yytable[] =
{
      89,    70,    71,   125,   139,    40,    41,    56,   127,   145,
     131,   201,   278,    64,    65,   298,   132,   351,   308,    45,
     238,   431,   217,   219,   280,    60,   285,   133,   209,    64,
      65,    44,   159,    24,   320,   146,   321,    64,    65,   176,
     137,    53,   174,   177,    56,    76,   178,   226,  -175,   179,
      64,    65,   134,   155,   136,    87,   135,    87,   438,   364,
      66,    70,    60,    77,   198,   175,   146,   154,   158,   225,
      87,   -78,   260,   203,   186,    87,   167,   170,   141,    78,
     135,   129,   220,   216,   316,    86,   188,   170,   224,   175,
     212,   213,   172,    87,   133,   222,   190,    44,    77,   130,
     156,   -78,   255,   390,    87,   148,   411,   391,   173,   374,
      87,    64,    65,   157,    78,    87,   222,   233,   214,    87,
     307,   387,   252,   323,   372,   138,    84,    87,   430,   279,
     281,   282,    64,    65,   286,   319,   385,   267,   223,   299,
     314,   315,   138,   144,    85,     5,     6,    87,    67,   146,
     164,    47,   176,    48,   295,  -226,   177,    68,   296,   178,
      50,   311,   179,   138,   335,   298,   300,    99,   340,   101,
     147,     5,     6,  -226,  -226,   288,   148,    47,   324,    48,
     306,   325,   233,   142,   215,   124,    50,   161,    87,   318,
     176,   133,   341,   -53,   177,   101,   138,   199,   146,   233,
     179,   313,   163,   317,   342,   405,   262,   263,     5,     6,
     200,   -53,   -53,   337,    47,   233,    48,   233,   138,   122,
     142,   164,   332,    50,     2,     3,    87,     4,   160,     5,
       6,     7,    51,   234,     8,   150,     9,   123,   333,   151,
     240,   192,   264,   235,   355,   383,   384,    10,   346,   236,
     186,   152,   328,   240,    11,   329,   359,   193,    12,   165,
     373,   362,   330,   166,   363,   365,   381,   382,    40,    41,
     367,   124,   386,   388,   348,   349,   352,   353,   389,   354,
     186,    87,   375,     4,    46,     5,     6,   400,   379,   380,
     235,    47,   376,    48,   184,   360,   361,    49,     5,     6,
      50,    67,   377,   169,    47,   233,    48,    44,   368,    51,
      68,   329,   378,    50,   402,   403,   404,   233,   330,   204,
      99,   194,    51,   205,   418,   390,   419,   233,   434,   391,
     392,   195,   233,   338,   196,   424,   425,   426,   339,   427,
     197,   252,     4,   437,     5,     6,    87,   350,     5,     6,
     267,   420,   421,   422,   423,   206,    48,    40,    41,   290,
     142,   218,   291,    50,     4,   292,     5,     6,   442,   410,
     269,    90,    51,     4,   270,     5,     6,     7,    91,    92,
     276,    47,     9,    48,   277,   390,   440,    93,    94,   391,
      50,   207,    95,   208,    96,    97,    98,    99,   100,   101,
     241,   102,   271,   272,   221,   258,   273,   301,   302,   242,
     303,   356,   357,   441,   103,    90,   254,     4,   256,     5,
       6,     7,    91,    92,   259,    47,     9,    48,   369,   370,
     371,    93,    94,   261,    50,   283,    95,   284,    96,    97,
      98,    99,   100,   101,   287,   102,   356,   358,   257,    90,
     289,     4,   309,     5,     6,     7,    91,    92,   103,    47,
       9,    48,   415,   416,   417,    93,    94,   310,    50,   214,
      95,   322,    96,    97,    98,    99,   100,   101,   331,   102,
     343,     5,     6,   356,   436,  -175,   334,    47,   336,    48,
     347,   396,   103,   227,     5,     6,    50,   366,   398,   397,
      47,   401,    48,   414,   428,    51,   227,     5,     6,    50,
     432,   228,   407,    47,   408,    48,   412,   429,    51,   227,
       5,     6,    50,   433,   297,   435,    47,   439,    48,    15,
      17,    51,   243,   140,    27,    50,   406,   399,   345,   244,
     245,   246,     5,     6,    51,   187,   247,   409,    47,   191,
      48,   128,   202,   293,   344,   237,   294,    50,   395,   143,
     413,   244,   245,   246,   268,     0,    51,   210,   247,     5,
       6,   126,    67,     0,     0,    47,     0,    48,     0,     0,
       0,    68,     0,     0,    50,     0,   262,   263,     5,     6,
       0,   211,     0,    51,    47,     0,    48,     0,     0,     4,
     142,     5,     6,    50,     0,     0,     0,    47,     0,    48,
       0,     0,    51,    49,     5,     6,    50,    67,     0,     0,
      47,     0,    48,     0,     0,    51,    93,     5,     6,    50,
      67,     0,     0,    47,     0,    48,    99,     0,    51,    68,
       5,     6,    50,    67,     0,     0,    47,     0,    48,     0,
       0,    51,   312,     5,     6,    50,     0,     0,     0,    47,
       0,    48,     0,     0,    51,    49,     5,     6,    50,     0,
       0,     0,    47,     0,    48,     6,     0,    51,   142,     5,
       6,    50,    48,     0,     0,    47,    49,    48,     0,    50,
      51,   227,     0,     0,    50,   168,   171,     0,    51,     0,
       0,     0,     0,    51,     0,   189,   171
};

static const yytype_int16 yycheck[] =
{
      24,    11,    11,    27,    43,     1,     1,     9,    32,    48,
      34,    91,   149,     9,     9,   184,    35,   264,   201,     7,
     120,   393,   102,   103,    16,     9,    16,    35,    99,    25,
      25,    17,    56,     1,   217,    20,   219,    33,    33,    11,
      42,     9,    66,    15,    46,    18,    18,   118,    33,    21,
      46,    46,    40,    55,    42,    47,    40,    47,   430,    36,
      17,    71,    46,     1,    88,    67,    20,    55,    56,   117,
      47,    17,    18,    92,    17,    47,    64,    65,    46,    17,
      64,    18,   106,   102,    27,    17,    74,    75,   112,    91,
     100,   100,    20,    47,   102,   107,    34,    17,     1,    36,
      20,    47,   126,    13,    47,    20,   353,    17,    36,   308,
      47,   107,   107,    33,    17,    47,   128,   119,    33,    47,
     200,   320,   124,   223,   307,    40,    18,    47,    38,   153,
     154,   155,   128,   128,   158,   215,   319,   139,    38,   187,
     211,   212,    40,    20,    36,     5,     6,    47,     8,    20,
      13,    11,    11,    13,   178,    18,    15,    17,   182,    18,
      20,   209,    21,    40,   235,   334,   190,    27,   239,    29,
      13,     5,     6,    36,    37,   163,    20,    11,    37,    13,
     199,   229,   184,    17,    44,    37,    20,    32,    47,   213,
      11,   199,   240,    18,    15,    29,    40,    31,    20,   201,
      21,   210,    18,   213,   243,   342,     3,     4,     5,     6,
      44,    36,    37,   237,    11,   217,    13,   219,    40,    18,
      17,    13,    17,    20,     0,     1,    47,     3,    46,     5,
       6,     7,    29,    17,    10,    13,    12,    36,    33,    17,
     120,    18,    39,    27,   268,   316,   317,    23,   250,    33,
      17,    29,    35,   133,    30,    38,   280,    38,    34,    13,
     308,   285,    45,    17,   288,   289,   314,   315,   264,   264,
     294,    37,   320,   321,   262,   263,    38,    39,   326,   267,
      17,    47,    17,     3,     4,     5,     6,   335,   312,   313,
      27,    11,    27,    13,    33,   283,   284,    17,     5,     6,
      20,     8,    17,    13,    11,   307,    13,    17,    35,    29,
      17,    38,    27,    20,   338,   339,   340,   319,    45,    13,
      27,    21,    29,    17,   372,    13,   374,   329,   399,    17,
      18,    36,   334,    16,    18,   383,   384,   385,    21,   387,
      16,   343,     3,   414,     5,     6,    47,     8,     5,     6,
     352,   375,   376,   377,   378,    37,    13,   353,   353,    15,
      17,    29,    18,    20,     3,    21,     5,     6,   439,     8,
      13,     1,    29,     3,    17,     5,     6,     7,     8,     9,
      13,    11,    12,    13,    17,    13,   434,    17,    18,    17,
      20,    37,    22,    37,    24,    25,    26,    27,    28,    29,
      36,    31,    13,    14,    34,    36,    17,    14,    15,    18,
      17,    38,    39,   437,    44,     1,    18,     3,    18,     5,
       6,     7,     8,     9,    18,    11,    12,    13,    41,    42,
      43,    17,    18,    18,    20,    32,    22,    32,    24,    25,
      26,    27,    28,    29,    32,    31,    38,    39,    34,     1,
      17,     3,    20,     5,     6,     7,     8,     9,    44,    11,
      12,    13,   369,   370,   371,    17,    18,    20,    20,    33,
      22,    17,    24,    25,    26,    27,    28,    29,    21,    31,
      38,     5,     6,    38,    39,    33,    33,    11,    32,    13,
      18,    45,    44,    17,     5,     6,    20,    15,    32,    16,
      11,    17,    13,    35,    19,    29,    17,     5,     6,    20,
      45,    35,    21,    11,    21,    13,    21,    19,    29,    17,
       5,     6,    20,    17,    35,    35,    11,    35,    13,     1,
       1,    29,    17,    46,     1,    20,   343,    35,   250,    24,
      25,    26,     5,     6,    29,    71,    31,   352,    11,    79,
      13,    33,    92,   178,    17,   120,   178,    20,   329,    47,
     356,    24,    25,    26,   141,    -1,    29,     3,    31,     5,
       6,    32,     8,    -1,    -1,    11,    -1,    13,    -1,    -1,
      -1,    17,    -1,    -1,    20,    -1,     3,     4,     5,     6,
      -1,    27,    -1,    29,    11,    -1,    13,    -1,    -1,     3,
      17,     5,     6,    20,    -1,    -1,    -1,    11,    -1,    13,
      -1,    -1,    29,    17,     5,     6,    20,     8,    -1,    -1,
      11,    -1,    13,    -1,    -1,    29,    17,     5,     6,    20,
       8,    -1,    -1,    11,    -1,    13,    27,    -1,    29,    17,
       5,     6,    20,     8,    -1,    -1,    11,    -1,    13,    -1,
      -1,    29,    17,     5,     6,    20,    -1,    -1,    -1,    11,
      -1,    13,    -1,    -1,    29,    17,     5,     6,    20,    -1,
      -1,    -1,    11,    -1,    13,     6,    -1,    29,    17,     5,
       6,    20,    13,    -1,    -1,    11,    17,    13,    -1,    20,
      29,    17,    -1,    -1,    20,    64,    65,    -1,    29,    -1,
      -1,    -1,    -1,    29,    -1,    74,    75
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    49,     0,     1,     3,     5,     6,     7,    10,    12,
      23,    30,    34,    50,    51,    52,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    70,    71,
      72,    73,    74,    75,    76,    77,    81,    82,    84,    85,
     137,   138,   139,   140,    17,   122,     4,    11,    13,    17,
      20,    29,    53,    62,    65,   122,   123,   124,   125,   126,
     127,   128,   129,   134,   137,   138,    17,     8,    17,    83,
     105,   121,   123,   127,   137,   138,    18,     1,    17,    86,
      87,    88,    89,    92,    18,    36,    17,    47,   133,   135,
       1,     8,     9,    17,    18,    22,    24,    25,    26,    27,
      28,    29,    31,    44,    52,    55,    62,    93,    94,    95,
      96,    97,    98,    99,   101,   102,   103,   104,   105,   117,
     120,   121,    18,    36,    37,   135,   133,   135,    93,    18,
      36,   135,   103,   120,   122,   127,   122,   123,    40,    78,
      53,    62,    17,   126,    20,    78,    20,    13,    20,    78,
      13,    17,    29,   132,   122,   123,    20,    33,   122,   135,
      46,    32,   119,    18,    13,    13,    17,   122,   129,    13,
     122,   129,    20,    36,   135,   123,    11,    15,    18,    21,
     106,   107,   108,   135,    33,   109,    17,    83,   122,   129,
      34,    87,    18,    38,    21,    36,    18,    16,   135,    31,
      44,   117,    97,   103,    13,    17,    37,    37,    37,   109,
       3,    27,   105,   121,    33,    44,   103,   117,    29,   117,
     135,    34,    94,    38,   135,   106,   109,    17,    35,   110,
     111,   112,   113,   123,    17,    27,    33,    98,    99,   100,
     104,    36,    18,    17,    24,    25,    26,    31,    66,    67,
      68,    69,   123,   136,    18,   135,    18,    34,    36,    18,
      18,    18,     3,     4,    39,    79,    80,   123,   132,    13,
      17,    13,    14,    17,   130,   131,    13,    17,   130,   135,
      16,   135,   135,    32,    32,    16,   135,    32,   122,    17,
      15,    18,    21,   107,   108,   135,   135,    35,   111,   106,
     135,    14,    15,    17,    90,    91,   103,   117,   110,    20,
      20,   106,    17,   121,   109,   109,    27,   105,   135,   117,
     110,   110,    17,    99,    37,   106,   114,   118,    35,    38,
      45,    21,    17,    33,    33,   109,    32,   135,    16,    21,
     109,   106,    78,    38,    17,    69,   123,    18,   122,   122,
       8,    64,    38,    39,   122,   135,    38,    39,    39,   135,
     122,   122,   135,   135,    36,   135,    15,   135,    35,    41,
      42,    43,   110,   106,   114,    17,    27,    17,    27,   135,
     135,   106,   106,   109,   109,   110,   106,   114,   106,   106,
      13,    17,    18,   115,   116,   112,    45,    16,    32,    35,
     106,    17,   135,   135,   135,   130,    67,    21,    21,    80,
       8,    64,    21,   131,    35,    91,    91,    91,   106,   106,
     135,   135,   135,   135,   106,   106,   106,   106,    19,    19,
      38,   116,    45,    17,   109,    35,    39,   109,   116,    35,
     106,   135,   109
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
	    mta->ResetState(); mta->yy_state = MTA::YYRet_Ok;
	    return mta->yy_state; }
    break;

  case 4:
#line 117 "mta_parse.y"
    {
	    mta->ResetState(); mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 5:
#line 119 "mta_parse.y"
    {
	    mta->ResetClassStack(); mta->ResetState();
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 6:
#line 122 "mta_parse.y"
    {
	    mta->ResetClassStack(); mta->ResetState();
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 7:
#line 125 "mta_parse.y"
    {
            mta->ResetState(); mta->ResetClassStack();
            mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 8:
#line 128 "mta_parse.y"
    {
	    mta->ResetState(); mta->ResetClassStack();
            mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 9:
#line 131 "mta_parse.y"
    {
	    mta->ResetState(); mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 10:
#line 133 "mta_parse.y"
    {
	    mta->ResetState(); mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 11:
#line 135 "mta_parse.y"
    {
	    mta->ResetState(); mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 12:
#line 137 "mta_parse.y"
    {
          // presumably leaving a namespace -- check that..
	    mta->ResetState(); mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 13:
#line 140 "mta_parse.y"
    {
	    mta->ResetState(); mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
    break;

  case 14:
#line 144 "mta_parse.y"
    {
  	  if((yyvsp[(1) - (1)].typ) != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
            // a typedef can never be literally a template or a template inst!
            (yyval.typ)->ClearType(TypeDef::TEMPLATE);
            (yyval.typ)->ClearType(TypeDef::TEMPL_INST);
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("typedef", sp, (yyval.typ)); } }
    break;

  case 15:
#line 155 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 16:
#line 156 "mta_parse.y"
    {
	    if((yyvsp[(1) - (2)].typ) != NULL)  SETDESC((yyvsp[(1) - (2)].typ),(yyvsp[(2) - (2)].chr)); }
    break;

  case 17:
#line 160 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 18:
#line 161 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(2) - (4)].typ); (yyvsp[(2) - (4)].typ)->name = (yyvsp[(3) - (4)].chr); mta->type_stack.Pop(); }
    break;

  case 19:
#line 163 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 20:
#line 165 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(3) - (5)].typ); (yyvsp[(3) - (5)].typ)->name = (yyvsp[(4) - (5)].chr); mta->type_stack.Pop(); }
    break;

  case 21:
#line 169 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); if((yyvsp[(1) - (3)].typ)) { (yyvsp[(2) - (3)].typ)->AddParent((yyvsp[(1) - (3)].typ)); (yyvsp[(2) - (3)].typ)->AssignType((yyvsp[(1) - (3)].typ)->type); }
	    mta->type_stack.Pop(); }
    break;

  case 22:
#line 172 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (4)].typ); if((yyvsp[(1) - (4)].typ)) { (yyvsp[(2) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(2) - (4)].typ)->AssignType((yyvsp[(1) - (4)].typ)->type); }
	    mta->type_stack.Pop(); (yyval.typ)->SetType(TypeDef::ARRAY); }
    break;

  case 23:
#line 175 "mta_parse.y"
    {
            /* annoying place for a comment: prevents structype and thus type */
            (yyval.typ) = (yyvsp[(3) - (4)].typ); if((yyvsp[(1) - (4)].typ)) { (yyvsp[(3) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(3) - (4)].typ)->AssignType((yyvsp[(1) - (4)].typ)->type); }
	    mta->type_stack.Pop(); }
    break;

  case 24:
#line 181 "mta_parse.y"
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

  case 25:
#line 190 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (7)].typ); (yyval.typ)->AssignType(TypeDef::FUN_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 26:
#line 193 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (4)].typ); (yyval.typ)->AssignType(TypeDef::METH_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 27:
#line 196 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 28:
#line 198 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (4)].typ); (yyval.typ)->SetType(TypeDef::ARRAY); }
    break;

  case 29:
#line 200 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 30:
#line 204 "mta_parse.y"
    {
            TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
            (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("enum", sp, (yyval.typ)); (yyval.typ)->source_end = mta->line-1; } }
    break;

  case 33:
#line 214 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); }
    break;

  case 34:
#line 215 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 35:
#line 218 "mta_parse.y"
    {
  	    (yyval.typ) = (yyvsp[(2) - (2)].typ);
            /* mta->PushState(MTA::Parse_enum); -- MP_ENUM triggers in mta_lex.cpp */
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::ENUM); mta->cur_enum = (yyvsp[(2) - (2)].typ);
	    mta->SetSource((yyval.typ), false); mta->type_stack.Pop(); }
    break;

  case 36:
#line 223 "mta_parse.y"
    {
            /* mta->PushState(MTA::Parse_enum); -- MP_ENUM triggers in mta_lex.cpp */
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->cur_enum = (yyval.typ);
	    mta->SetSource((yyval.typ), false); (yyval.typ)->AssignType(TypeDef::ENUM); }
    break;

  case 37:
#line 231 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("class", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 38:
#line 239 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 39:
#line 240 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 40:
#line 244 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("class", sp, (yyval.typ)); mta->FixClassTypes((yyval.typ));
              (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 43:
#line 258 "mta_parse.y"
    {
	    if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	    else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 44:
#line 264 "mta_parse.y"
    {
	    (yyvsp[(1) - (2)].typ)->tokens.keep = true; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (2)].typ), true); }
    break;

  case 45:
#line 267 "mta_parse.y"
    {
            SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs();
            mta->SetSource((yyvsp[(1) - (3)].typ), true); }
    break;

  case 46:
#line 270 "mta_parse.y"
    {
            SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (3)].typ), true); 
	  }
    break;

  case 47:
#line 277 "mta_parse.y"
    { mta->PushState(MTA::Parse_inclass); }
    break;

  case 48:
#line 278 "mta_parse.y"
    { mta->PushState(MTA::Parse_inclass); }
    break;

  case 49:
#line 281 "mta_parse.y"
    {
            mta->PushState(MTA::Parse_class);
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ));
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::CLASS); mta->ClearSource((yyvsp[(2) - (2)].typ)); /* tyname set -- premature */
            mta->cur_mstate = MTA::prvt; }
    break;

  case 50:
#line 286 "mta_parse.y"
    {
            mta->PushState(MTA::Parse_class);
	    (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ));
            mta->cur_mstate = MTA::prvt; }
    break;

  case 51:
#line 290 "mta_parse.y"
    {
            mta->PushState(MTA::Parse_class);
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::CLASS); 
            mta->type_stack.Push((yyval.typ));
	    mta->PushClass((yyval.typ));
            mta->cur_mstate = MTA::prvt; }
    break;

  case 52:
#line 297 "mta_parse.y"
    {
            mta->PushState(MTA::Parse_class);
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::STRUCT);
            mta->type_stack.Push((yyval.typ));
	    mta->PushClass((yyval.typ));
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 53:
#line 304 "mta_parse.y"
    {
            mta->PushState(MTA::Parse_class);
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->PushClass((yyvsp[(2) - (2)].typ));
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::CLASS); mta->ClearSource((yyvsp[(2) - (2)].typ)); /* tyname set -- premature */
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 54:
#line 309 "mta_parse.y"
    {
            mta->PushState(MTA::Parse_class);
	    (yyval.typ) = (yyvsp[(1) - (2)].typ); mta->PushClass((yyvsp[(1) - (2)].typ));
            mta->cur_mstate = MTA::pblc; }
    break;

  case 55:
#line 313 "mta_parse.y"
    {
            mta->PushState(MTA::Parse_class);
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::UNION);
            mta->type_stack.Push((yyval.typ));
	    mta->PushClass((yyval.typ));
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 56:
#line 323 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].typ) != NULL) mta->cur_class->AddParent((yyvsp[(1) - (1)].typ)); }
    break;

  case 57:
#line 325 "mta_parse.y"
    {
            if((yyvsp[(3) - (3)].typ) != NULL) {mta->cur_class->AddParent((yyvsp[(3) - (3)].typ));
	      if(!mta->cur_class->HasOption("MULT_INHERIT"))
                mta->cur_class->opts.Add("MULT_INHERIT"); } }
    break;

  case 59:
#line 333 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 60:
#line 334 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 61:
#line 335 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 62:
#line 336 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 67:
#line 348 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ)); }
	    mta->type_stack.Pop(); }
    break;

  case 68:
#line 356 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 69:
#line 357 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 70:
#line 361 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ));
              mta->FixClassTypes((yyval.typ));
	      (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 71:
#line 371 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 72:
#line 372 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 73:
#line 373 "mta_parse.y"
    { mta->Burp(); 
            // todo: could create the template based on function name, and create a 
            // methoddef to hold it.  but we don't care.. :)
            }
    break;

  case 74:
#line 377 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 75:
#line 378 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (4)].typ),(yyvsp[(4) - (4)].chr)); }
    break;

  case 76:
#line 379 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr));
          // todo: could create the template based on function name, and create a 
          // methoddef to hold it.  but we don't care.. :)
          }
    break;

  case 77:
#line 386 "mta_parse.y"
    {
          if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	  else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 79:
#line 393 "mta_parse.y"
    {
            mta->PushState(MTA::Parse_inclass); (yyvsp[(1) - (2)].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 80:
#line 396 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs(); }
    break;

  case 81:
#line 398 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->PushState(MTA::Parse_inclass); mta->Class_ResetCurPtrs(); }
    break;

  case 82:
#line 403 "mta_parse.y"
    {
	    (yyvsp[(5) - (5)].typ)->templ_pars.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_pars.Duplicate(mta->cur_templ_pars);
	    (yyvsp[(5) - (5)].typ)->SetType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(5) - (5)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 83:
#line 408 "mta_parse.y"
    {
	    (yyvsp[(4) - (4)].typ)->templ_pars.Reset();
	    (yyvsp[(4) - (4)].typ)->SetType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(4) - (4)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 84:
#line 415 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(1) - (4)].typ);
            mta->PushState(MTA::Parse_fundef);
          }
    break;

  case 85:
#line 419 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(1) - (5)].typ);
            mta->PushState(MTA::Parse_fundef);
          }
    break;

  case 86:
#line 424 "mta_parse.y"
    { mta->cur_templ_pars.Reset(); }
    break;

  case 88:
#line 429 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 89:
#line 433 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 90:
#line 434 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 91:
#line 435 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 92:
#line 437 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (3)].typ)); (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 93:
#line 438 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (3)].typ)); (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 94:
#line 439 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (3)].typ)); (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 95:
#line 442 "mta_parse.y"
    {
            if(mta->cur_is_trg) { /* only add reg_funs in target space */
              TypeDef* nt = new TypeDef((yyvsp[(1) - (1)].meth)->name, TypeDef::FUNCTION,0,0);
              mta->SetSource(nt, false);
              taMisc::types.Add(nt);
              nt->methods.AddUniqNameNew((yyvsp[(1) - (1)].meth));
              taMisc::reg_funs.Link(nt); }
            mta->meth_stack.Pop(); }
    break;

  case 96:
#line 452 "mta_parse.y"
    {
            (yyvsp[(3) - (4)].meth)->type = (yyvsp[(2) - (4)].typ); SETDESC((yyvsp[(3) - (4)].meth),(yyvsp[(4) - (4)].chr)); (yyval.meth) = (yyvsp[(3) - (4)].meth); }
    break;

  case 97:
#line 454 "mta_parse.y"
    {
            (yyvsp[(2) - (3)].meth)->type = &TA_int; SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); (yyval.meth) = (yyvsp[(2) - (3)].meth); }
    break;

  case 98:
#line 458 "mta_parse.y"
    {
            (yyvsp[(1) - (2)].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = true; }
    break;

  case 99:
#line 463 "mta_parse.y"
    { /* using is not parsed */
            mta->Namespc_PushNew((yyvsp[(2) - (3)].chr));
          }
    break;

  case 100:
#line 466 "mta_parse.y"
    { /* using is not parsed */
            /* todo: first name is not retained */
            mta->Namespc_PushNew((yyvsp[(4) - (5)].chr));
          }
    break;

  case 101:
#line 472 "mta_parse.y"
    {
            mta->Namespc_PushNew((yyvsp[(2) - (3)].chr));
          }
    break;

  case 102:
#line 475 "mta_parse.y"
    {
            /* todo: first name is not retained */
            mta->Namespc_PushNew((yyvsp[(4) - (5)].chr));
          }
    break;

  case 106:
#line 486 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 109:
#line 491 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 110:
#line 496 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 111:
#line 499 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm));
            if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
            else (yyvsp[(1) - (3)].enm)->opts.Add("#IGNORE"); // ignore bad math!
	    mta->enum_stack.Pop(); }
    break;

  case 113:
#line 508 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 114:
#line 511 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 115:
#line 514 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) | (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 116:
#line 520 "mta_parse.y"
    {
            EnumDef* ed = mta->cur_enum->enum_vals.FindName((yyvsp[(1) - (1)].chr));
            if(ed) (yyval.rval) = ed->enum_no;
            else   (yyval.rval) = -424242; }
    break;

  case 118:
#line 525 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 119:
#line 529 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 120:
#line 532 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 121:
#line 533 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 122:
#line 536 "mta_parse.y"
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

  case 123:
#line 545 "mta_parse.y"
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

  case 124:
#line 558 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
            mta->TypeAdded("enum", &(mta->cur_class->sub_types), (yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->source_end = mta->line-1;
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 125:
#line 564 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 126:
#line 568 "mta_parse.y"
    { 
	    mta->PopClass();
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (2)].typ));
            (yyvsp[(1) - (2)].typ)->SetType(TypeDef::SUBTYPE);
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 127:
#line 573 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 129:
#line 578 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 130:
#line 579 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 131:
#line 580 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 132:
#line 581 "mta_parse.y"
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

  case 134:
#line 602 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 135:
#line 608 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 136:
#line 609 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 137:
#line 610 "mta_parse.y"
    { }
    break;

  case 138:
#line 611 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
            int spsz = sp->size;
            TypeDef* nty = (yyvsp[(1) - (4)].typ)->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    (yyvsp[(2) - (4)].memb)->type = nty; }
    break;

  case 139:
#line 618 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 140:
#line 622 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 141:
#line 626 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(3) - (3)].memb)->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 142:
#line 632 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 143:
#line 638 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 145:
#line 644 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 146:
#line 645 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 147:
#line 646 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 148:
#line 647 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 149:
#line 648 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 150:
#line 649 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 151:
#line 650 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 152:
#line 651 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 153:
#line 652 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 154:
#line 653 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 155:
#line 654 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 156:
#line 655 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 157:
#line 656 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 158:
#line 657 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 159:
#line 658 "mta_parse.y"
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

  case 160:
#line 668 "mta_parse.y"
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

  case 161:
#line 678 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 162:
#line 679 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 163:
#line 680 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 164:
#line 681 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 166:
#line 686 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 167:
#line 687 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 168:
#line 689 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 169:
#line 694 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 170:
#line 695 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
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
#line 701 "mta_parse.y"
    {
             (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 175:
#line 708 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 176:
#line 712 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 177:
#line 713 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
    break;

  case 178:
#line 714 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 179:
#line 715 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 180:
#line 716 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 181:
#line 717 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 182:
#line 721 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 183:
#line 722 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 184:
#line 723 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
    break;

  case 185:
#line 727 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 186:
#line 728 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 187:
#line 729 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 188:
#line 732 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 189:
#line 733 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 190:
#line 736 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 191:
#line 737 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 192:
#line 740 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 193:
#line 741 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 194:
#line 742 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 195:
#line 745 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 196:
#line 747 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      String ad = (yyvsp[(2) - (2)].chr);
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 197:
#line 758 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 198:
#line 762 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 199:
#line 766 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 200:
#line 770 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 201:
#line 774 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 209:
#line 795 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 210:
#line 799 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 211:
#line 802 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 212:
#line 803 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 213:
#line 806 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 215:
#line 810 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 216:
#line 811 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 217:
#line 814 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
                                  mta->SetSource((yyval.typ), false); }
    break;

  case 219:
#line 819 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ)->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, (yyval.typ)); }
	  }
    break;

  case 221:
#line 829 "mta_parse.y"
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

  case 223:
#line 841 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(2) - (2)].typ)->GetConstType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("const", sp, (yyval.typ)); }
	  }
    break;

  case 227:
#line 854 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::UNION);
            (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 228:
#line 856 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::UNION); }
    break;

  case 229:
#line 857 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
	    (yyval.typ) = td; }
    break;

  case 230:
#line 864 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
            (yyval.typ) = td; }
    break;

  case 231:
#line 871 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 232:
#line 872 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 233:
#line 873 "mta_parse.y"
    {
            TypeDef* nty = new TypeDef((yyvsp[(2) - (2)].chr)); mta->SetSource(nty, false);
	    TypeSpace* sp = mta->GetTypeSpace(nty);
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("namespace new", sp, (yyval.typ));
              (yyval.typ)->namespc = mta->cur_nmspc_tmp; }
	    else { mta->TypeNotAdded("namespace new", sp, (yyval.typ), nty); delete nty; }
          }
    break;

  case 234:
#line 881 "mta_parse.y"
    {
            if(mta->verbose > 1) {
              taMisc::Info("namespace type:", mta->cur_nmspc_tmp, "::", (yyvsp[(2) - (2)].typ)->name);
            }
            (yyvsp[(2) - (2)].typ)->namespc = mta->cur_nmspc_tmp; /* todo: could check.. */
            (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 235:
#line 887 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 237:
#line 889 "mta_parse.y"
    { /* a template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
              if(mta->verbose > 1) {
                taMisc::Info("Type:", (yyvsp[(1) - (4)].typ)->name, "used as template but not marked as such",
                             "-- now marking -- probably just an internal type"); }
              (yyvsp[(1) - (4)].typ)->SetType(TypeDef::TEMPLATE); }
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

  case 238:
#line 906 "mta_parse.y"
    { /* this template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
              if(mta->verbose > 1) {
                taMisc::Info("Type:", (yyvsp[(1) - (4)].typ)->name, "used as template but not marked as such",
                             "-- now marking -- probably just an internal type"); }
              (yyvsp[(1) - (4)].typ)->SetType(TypeDef::TEMPLATE); }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 239:
#line 915 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::STRUCT);
          (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 240:
#line 917 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::STRUCT); }
    break;

  case 242:
#line 921 "mta_parse.y"
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
#line 936 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 245:
#line 941 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 246:
#line 942 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 247:
#line 943 "mta_parse.y"
    { (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 249:
#line 948 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 250:
#line 949 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 253:
#line 956 "mta_parse.y"
    { mta->cur_nmspc_tmp = (yyvsp[(1) - (2)].chr); (yyval.chr) = mta->cur_nmspc_tmp; }
    break;

  case 258:
#line 968 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 259:
#line 972 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 260:
#line 976 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 261:
#line 980 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 3344 "y.tab.c"
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


#line 983 "mta_parse.y"


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

