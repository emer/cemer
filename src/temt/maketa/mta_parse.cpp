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
#define YYLAST   596

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  89
/* YYNRULES -- Number of rules.  */
#define YYNRULES  229
/* YYNRULES -- Number of states.  */
#define YYNSTATES  384

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
       2,     2,     2,     2,     2,     2,     2,     2,    45,     2,
      33,    35,    32,    41,    38,    42,    44,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    46,
      40,     2,    39,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    36,     2,    34,    43,     2,     2,     2,
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
      56,    61,    64,    72,    77,    81,    85,    87,    92,    95,
      99,   103,   106,   108,   110,   113,   117,   119,   122,   126,
     130,   133,   137,   141,   143,   147,   150,   153,   155,   157,
     160,   162,   164,   168,   170,   173,   175,   178,   180,   183,
     185,   187,   189,   192,   196,   198,   201,   205,   209,   212,
     216,   220,   226,   228,   230,   234,   237,   240,   243,   247,
     249,   254,   258,   261,   266,   270,   272,   275,   277,   280,
     282,   285,   287,   289,   293,   295,   299,   303,   305,   307,
     309,   311,   314,   316,   318,   320,   322,   325,   327,   329,
     332,   335,   338,   340,   342,   345,   349,   354,   357,   362,
     367,   369,   373,   375,   380,   382,   386,   391,   396,   401,
     406,   412,   418,   424,   429,   433,   438,   443,   449,   454,
     456,   459,   462,   466,   470,   473,   478,   485,   489,   492,
     494,   496,   498,   501,   504,   507,   511,   513,   516,   520,
     522,   524,   527,   530,   534,   536,   539,   541,   545,   550,
     552,   555,   557,   560,   564,   571,   573,   576,   578,   580,
     583,   587,   590,   593,   596,   598,   600,   603,   605,   607,
     609,   612,   614,   616,   619,   621,   624,   626,   629,   631,
     634,   637,   640,   644,   648,   652,   656,   659,   661,   666,
     671,   673,   676,   678,   682,   684,   686,   688,   690,   692,
     694,   696,   699,   701,   703,   705,   707,   709,   711,   713
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      48,     0,    -1,    -1,    48,    49,    -1,    48,    53,    -1,
      48,    57,    -1,    48,    59,    -1,    48,    69,    -1,    48,
      71,    -1,    48,    79,    -1,    48,    82,    -1,    48,    83,
      -1,    48,    34,    -1,    48,     1,    -1,    50,    -1,    51,
      -1,    51,    18,    -1,    12,    52,    -1,    12,    61,   128,
     130,    -1,   121,   120,   130,    -1,   121,    18,   120,   130,
      -1,   121,   130,    -1,   121,    33,    32,   120,    35,   107,
     130,    -1,   121,    20,    32,   120,    -1,   120,   120,   130,
      -1,   120,   121,   130,    -1,    54,    -1,    55,    84,    34,
     130,    -1,    56,    36,    -1,    56,    18,    36,    -1,    56,
      36,    18,    -1,     7,   120,    -1,     7,    -1,    58,    -1,
      64,   130,    -1,    64,   130,    18,    -1,    60,    -1,    61,
     130,    -1,    61,   129,   130,    -1,    62,    91,    34,    -1,
      63,    36,    -1,    63,    18,    36,    -1,    63,    36,    18,
      -1,    64,    -1,    64,    37,    65,    -1,   134,   120,    -1,
     134,    13,    -1,   134,    -1,   132,    -1,   132,   120,    -1,
     133,    -1,    66,    -1,    65,    38,    66,    -1,   121,    -1,
      67,   121,    -1,    17,    -1,    67,    17,    -1,    68,    -1,
      67,    68,    -1,   131,    -1,    31,    -1,    70,    -1,    75,
     130,    -1,    75,   130,    18,    -1,    72,    -1,    73,   130,
      -1,    73,   129,   130,    -1,    74,    91,    34,    -1,    75,
      36,    -1,    75,    18,    36,    -1,    75,    36,    18,    -1,
     135,    76,    77,    39,    63,    -1,    40,    -1,    78,    -1,
      78,    38,    78,    -1,     3,   120,    -1,     4,   120,    -1,
     121,   120,    -1,   121,   120,    21,    -1,    80,    -1,    30,
     119,    81,   104,    -1,    30,    81,   104,    -1,   103,   107,
      -1,    22,    23,    17,   130,    -1,    23,    17,    36,    -1,
      85,    -1,    84,    85,    -1,    86,    -1,    86,    18,    -1,
      87,    -1,    87,    38,    -1,     1,    -1,    90,    -1,    90,
      21,    88,    -1,    89,    -1,    88,    41,    89,    -1,    88,
      42,    89,    -1,    17,    -1,    14,    -1,    17,    -1,    92,
      -1,    91,    92,    -1,    93,    -1,    99,    -1,    54,    -1,
      51,    -1,    61,   130,    -1,     1,    -1,    94,    -1,    24,
      37,    -1,    25,    37,    -1,    26,    37,    -1,    18,    -1,
      95,    -1,     9,    95,    -1,   118,    96,   130,    -1,   118,
      97,    21,   130,    -1,    96,   130,    -1,   118,    97,    16,
     130,    -1,   118,    98,   107,   130,    -1,    97,    -1,    96,
      38,    97,    -1,    17,    -1,    33,    32,    17,    35,    -1,
     100,    -1,   115,   108,   104,    -1,     8,   115,   108,   104,
      -1,    31,   115,   108,   104,    -1,   115,   108,   112,   104,
      -1,    43,   115,   108,   104,    -1,     8,    43,   115,   108,
     104,    -1,    31,    43,   115,   108,   104,    -1,    28,   119,
      27,   107,   104,    -1,    28,    27,   107,   104,    -1,    28,
     119,   130,    -1,    28,     3,   119,   130,    -1,    28,     3,
      17,   130,    -1,    28,   119,   103,   107,   104,    -1,    28,
     103,   107,   104,    -1,   101,    -1,     9,   101,    -1,    31,
     101,    -1,     8,    31,   101,    -1,   118,   102,   104,    -1,
     102,   104,    -1,   118,    27,   107,   104,    -1,   118,    27,
      33,    35,   107,   104,    -1,    27,   107,   104,    -1,   103,
     107,    -1,    17,    -1,   130,    -1,   105,    -1,   106,   130,
      -1,    18,   130,    -1,    18,   105,    -1,    18,   106,   130,
      -1,    15,    -1,    11,    15,    -1,    11,    18,    15,    -1,
      21,    -1,    11,    -1,    11,    21,    -1,    33,    35,    -1,
      33,   109,    35,    -1,    35,    -1,   109,    35,    -1,   110,
      -1,   109,    38,   110,    -1,   109,    44,    44,    44,    -1,
     111,    -1,   111,    21,    -1,   121,    -1,   121,    17,    -1,
     121,    17,    16,    -1,   121,    33,    32,    17,    35,   107,
      -1,    17,    -1,   116,   113,    -1,   114,    -1,    18,    -1,
     113,   114,    -1,   113,    38,   114,    -1,    13,    19,    -1,
      17,    19,    -1,    29,    33,    -1,    37,    -1,    32,    -1,
     117,    32,    -1,   119,    -1,   121,    -1,     8,    -1,     8,
     121,    -1,    17,    -1,   122,    -1,   122,    45,    -1,   123,
      -1,   123,   117,    -1,   124,    -1,    11,   124,    -1,   125,
      -1,   132,   125,    -1,   133,   125,    -1,   133,   120,    -1,
      13,    20,    17,    -1,    29,    20,    17,    -1,    13,    20,
      13,    -1,    29,    20,    13,    -1,    20,    13,    -1,    29,
      -1,    13,    76,   126,    39,    -1,    29,    76,   126,    39,
      -1,    13,    -1,   125,    13,    -1,   127,    -1,   126,    38,
     127,    -1,    13,    -1,    17,    -1,    14,    -1,    17,    -1,
      13,    -1,    29,    -1,    17,    -1,    17,    16,    -1,    46,
      -1,    24,    -1,    25,    -1,    26,    -1,     5,    -1,     6,
      -1,     3,    -1,    10,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   112,   112,   113,   116,   118,   121,   124,   126,   128,
     130,   132,   134,   137,   141,   152,   153,   157,   158,   162,
     165,   170,   179,   182,   185,   187,   191,   197,   200,   201,
     202,   205,   209,   216,   224,   225,   229,   238,   239,   243,
     249,   252,   255,   262,   263,   266,   271,   275,   282,   289,
     294,   304,   306,   313,   314,   315,   316,   319,   320,   323,
     324,   328,   336,   337,   341,   351,   352,   356,   362,   365,
     367,   372,   379,   383,   384,   388,   389,   390,   391,   394,
     404,   406,   410,   415,   420,   425,   426,   429,   430,   433,
     434,   435,   440,   443,   449,   450,   455,   463,   464,   468,
     471,   472,   475,   484,   497,   503,   507,   512,   516,   517,
     518,   519,   520,   540,   541,   547,   548,   549,   550,   557,
     561,   565,   571,   577,   582,   583,   584,   585,   586,   587,
     588,   589,   590,   591,   592,   593,   594,   595,   605,   618,
     619,   620,   622,   627,   628,   629,   630,   631,   634,   641,
     645,   646,   647,   648,   649,   650,   654,   655,   656,   660,
     661,   662,   665,   666,   669,   670,   673,   674,   675,   678,
     680,   691,   695,   699,   703,   707,   713,   717,   718,   719,
     720,   723,   724,   728,   732,   735,   736,   739,   742,   743,
     744,   747,   751,   752,   761,   762,   773,   774,   782,   783,
     786,   788,   789,   793,   797,   798,   799,   800,   801,   818,
     824,   825,   839,   840,   845,   846,   847,   851,   852,   853,
     856,   857,   860,   863,   864,   865,   869,   873,   877,   881
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
  "'<'", "'+'", "'-'", "'~'", "'.'", "'&'", "';'", "$accept", "list",
  "typedefn", "typedefns", "typedsub", "defn", "enumdefn", "enumdsub",
  "enumname", "enumnm", "classdecl", "classdecls", "classdefn",
  "classdefns", "classdsub", "classname", "classhead", "classnm",
  "classinh", "classpar", "classptyp", "classpmod", "templdecl",
  "templdecls", "templdefn", "templdefns", "templdsub", "templname",
  "templhead", "templopen", "templpars", "templpar", "fundecl", "funnm",
  "regfundefn", "usenamespc", "namespc", "enums", "enumline", "enumitm",
  "enumitms", "enummath", "enummathitm", "enmitmname", "membs", "membline",
  "membdefn", "basicmemb", "nostatmemb", "membnames", "membname",
  "membfunp", "methdefn", "basicmeth", "nostatmeth", "mbfundefn",
  "methname", "fundefn", "funsubdefn", "funsubdecl", "funargs", "constfun",
  "args", "argdefn", "subargdefn", "constrlist", "constitms", "constref",
  "consthsnm", "constcoln", "ptrs", "membtype", "ftype", "tyname", "type",
  "noreftype", "constype", "subtype", "combtype", "templargs", "templarg",
  "tdname", "varname", "term", "access", "structkeyword", "structkeyword",
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
      60,    43,    45,   126,    46,    38,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    47,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    49,    50,    50,    51,    51,    52,
      52,    52,    52,    52,    52,    52,    53,    54,    55,    55,
      55,    56,    56,    57,    58,    58,    59,    60,    60,    61,
      62,    62,    62,    63,    63,    64,    64,    64,    64,    64,
      64,    65,    65,    66,    66,    66,    66,    67,    67,    68,
      68,    69,    70,    70,    71,    72,    72,    73,    74,    74,
      74,    75,    76,    77,    77,    78,    78,    78,    78,    79,
      80,    80,    81,    82,    83,    84,    84,    85,    85,    86,
      86,    86,    87,    87,    88,    88,    88,    89,    89,    90,
      91,    91,    92,    92,    92,    92,    92,    92,    93,    93,
      93,    93,    93,    94,    94,    95,    95,    95,    95,    95,
      96,    96,    97,    98,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,   100,
     100,   100,   100,   101,   101,   101,   101,   101,   102,   103,
     104,   104,   104,   104,   104,   104,   105,   105,   105,   106,
     106,   106,   107,   107,   108,   108,   109,   109,   109,   110,
     110,   111,   111,   111,   111,   111,   112,   113,   113,   113,
     113,   114,   114,   115,   116,   117,   117,   118,   119,   119,
     119,   120,   121,   121,   122,   122,   123,   123,   124,   124,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     125,   125,   126,   126,   127,   127,   127,   128,   128,   128,
     129,   129,   130,   131,   131,   131,   132,   133,   134,   135
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     1,     2,     2,     4,     3,
       4,     2,     7,     4,     3,     3,     1,     4,     2,     3,
       3,     2,     1,     1,     2,     3,     1,     2,     3,     3,
       2,     3,     3,     1,     3,     2,     2,     1,     1,     2,
       1,     1,     3,     1,     2,     1,     2,     1,     2,     1,
       1,     1,     2,     3,     1,     2,     3,     3,     2,     3,
       3,     5,     1,     1,     3,     2,     2,     2,     3,     1,
       4,     3,     2,     4,     3,     1,     2,     1,     2,     1,
       2,     1,     1,     3,     1,     3,     3,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     2,     1,     1,     2,
       2,     2,     1,     1,     2,     3,     4,     2,     4,     4,
       1,     3,     1,     4,     1,     3,     4,     4,     4,     4,
       5,     5,     5,     4,     3,     4,     4,     5,     4,     1,
       2,     2,     3,     3,     2,     4,     6,     3,     2,     1,
       1,     1,     2,     2,     2,     3,     1,     2,     3,     1,
       1,     2,     2,     3,     1,     2,     1,     3,     4,     1,
       2,     1,     2,     3,     6,     1,     2,     1,     1,     2,
       3,     2,     2,     2,     1,     1,     2,     1,     1,     1,
       2,     1,     1,     2,     1,     2,     1,     2,     1,     2,
       2,     2,     3,     3,     3,     3,     2,     1,     4,     4,
       1,     2,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    13,   228,   226,   227,    32,   229,     0,
       0,     0,     0,    12,     3,    14,    15,     4,    26,     0,
       0,     5,    33,     6,    36,     0,     0,     0,    43,     7,
      61,     8,    64,     0,     0,     0,     9,    79,    10,    11,
      48,    50,    47,     0,   191,    31,     0,   210,     0,   207,
      17,     0,    43,     0,     0,   192,   194,   196,   198,    48,
      50,     0,     0,   189,   149,     0,     0,     0,   188,     0,
       0,    16,    91,    99,     0,    85,    87,    89,    92,     0,
      28,   220,   222,     0,    37,   107,   189,     0,   122,   112,
       0,     0,     0,     0,     0,   207,     0,     0,   105,   104,
       0,     0,   100,   102,   108,   113,     0,   120,   103,   124,
     139,     0,     0,     0,     0,   187,     0,    40,     0,    34,
       0,    65,     0,     0,    68,    62,    49,    46,    45,    72,
       0,   197,     0,     0,   206,     0,     0,   218,   217,   219,
       0,     0,     0,     0,     0,     0,     0,    21,   193,   185,
     195,   211,   210,   199,   201,   200,     0,    84,   190,   160,
     156,     0,   159,    81,   151,     0,   150,     0,    82,     0,
       0,    86,    88,    90,     0,    29,    30,   221,    38,     0,
       0,     0,   114,   140,   109,   110,   111,     0,     0,     0,
       0,     0,   183,     0,   141,     0,     0,     0,     0,   106,
      39,   101,     0,   117,   144,   148,   175,   164,     0,     0,
     166,   169,   171,     0,     0,     0,   120,     0,     0,    41,
      42,    55,   223,   224,   225,    60,    44,    51,     0,    57,
      53,    59,    35,    66,    67,    69,    70,    63,     0,     0,
       0,    73,     0,   204,   202,   214,   216,   215,     0,   212,
     205,   203,     0,    18,    24,    25,     0,     0,     0,    19,
     186,    83,   157,     0,   161,   154,     0,   153,   152,   162,
       0,    80,    27,    98,    97,    93,    94,   142,     0,     0,
     147,     0,     0,     0,     0,     0,     0,   134,     0,     0,
       0,   122,   121,   184,   125,     0,     0,   165,     0,     0,
     170,   172,     0,     0,     0,     0,   115,     0,     0,     0,
     143,     0,    56,    58,    54,    75,    76,     0,     0,    77,
       0,   208,   209,    20,    23,     0,   158,   155,   163,     0,
       0,     0,   126,   136,   135,   133,   138,     0,     0,     0,
     127,   129,   128,     0,     0,   178,   176,   177,   167,     0,
     173,     0,   162,   145,     0,   118,   116,   119,    52,    71,
      74,    78,   213,     0,    95,    96,   130,   132,   137,   131,
     181,   182,     0,   179,   168,     0,     0,   123,     0,   180,
       0,   146,    22,   174
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    14,    15,    98,    50,    17,    99,    19,    20,
      21,    22,    23,    24,   100,    26,    27,    52,   226,   227,
     228,   229,    29,    30,    31,    32,    33,    34,    35,   136,
     240,   241,    36,    37,    65,    38,    39,    74,    75,    76,
      77,   275,   276,    78,   101,   102,   103,   104,   105,   106,
     107,   217,   108,   109,   110,   111,   112,   163,   164,   165,
     168,   208,   209,   210,   211,   295,   346,   347,   113,   296,
     150,   114,   115,   126,    68,    55,    56,    57,    58,   248,
     249,   140,    83,   166,   231,    69,    70,    42,    43
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -329
static const yytype_int16 yypact[] =
{
    -329,   431,  -329,  -329,  -329,  -329,  -329,    49,  -329,   256,
       1,    76,   196,  -329,  -329,  -329,    73,  -329,  -329,    31,
     101,  -329,  -329,  -329,  -329,    25,   399,   121,   140,  -329,
    -329,  -329,  -329,    25,   399,    46,  -329,  -329,  -329,  -329,
      49,  -329,   116,   127,  -329,  -329,   213,    34,   119,    74,
    -329,   155,   144,   554,    92,   128,   185,  -329,   190,   197,
     197,   205,   191,   331,  -329,    22,   199,   220,  -329,   217,
     197,  -329,  -329,  -329,    19,  -329,   221,   222,   247,   244,
     268,   286,  -329,   264,  -329,  -329,   165,   515,   275,  -329,
     283,   298,   303,   199,   502,    80,   136,   296,  -329,  -329,
     264,   321,  -329,  -329,  -329,  -329,    17,  -329,  -329,  -329,
    -329,    22,   199,    67,   142,  -329,   295,   325,   458,   336,
     264,  -329,   360,   326,   352,   356,  -329,  -329,  -329,  -329,
     294,  -329,   257,   299,  -329,   262,   299,  -329,  -329,  -329,
     264,   264,   264,    49,   343,   344,   264,  -329,  -329,  -329,
     347,  -329,  -329,   190,  -329,   190,   264,  -329,  -329,   288,
    -329,    24,  -329,  -329,  -329,   264,  -329,   223,  -329,    22,
     264,  -329,  -329,  -329,   198,  -329,  -329,  -329,  -329,   528,
     296,    67,  -329,  -329,  -329,  -329,  -329,    22,   541,   199,
     199,    30,  -329,   296,  -329,    67,   135,   348,    67,  -329,
    -329,  -329,   365,  -329,  -329,  -329,  -329,  -329,   143,   200,
    -329,   362,   113,   357,   361,    17,   171,   199,    22,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,   354,  -329,   475,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,    49,    49,
     358,   363,    49,  -329,  -329,  -329,  -329,  -329,   -16,  -329,
    -329,  -329,   318,  -329,  -329,  -329,   264,    49,    49,  -329,
    -329,  -329,  -329,   380,  -329,  -329,   264,  -329,  -329,  -329,
     228,  -329,  -329,  -329,  -329,   253,  -329,  -329,    67,    22,
    -329,   264,   264,    22,    22,   199,   199,  -329,    67,    22,
      22,  -329,  -329,  -329,  -329,    22,   180,  -329,   567,   355,
    -329,   382,   364,   439,    22,   392,  -329,   264,   264,   264,
    -329,   458,  -329,  -329,  -329,  -329,  -329,   245,   294,   393,
     299,  -329,  -329,  -329,  -329,   378,  -329,  -329,  -329,   198,
     198,    22,  -329,  -329,  -329,  -329,  -329,    22,    22,    22,
    -329,  -329,  -329,   396,   401,  -329,    52,  -329,  -329,   374,
    -329,   404,   199,  -329,   387,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  -329,   199,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,   276,  -329,  -329,   394,    22,  -329,   264,  -329,
     199,  -329,  -329,  -329
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -329,  -329,  -329,  -329,   432,  -329,  -329,   434,  -329,  -329,
    -329,  -329,  -329,  -329,    88,  -329,   122,   445,  -329,   129,
    -329,   219,  -329,  -329,  -329,  -329,  -329,  -329,  -329,   272,
    -329,   130,  -329,  -329,   384,  -329,  -329,  -329,   375,  -329,
    -329,  -329,    29,  -329,   421,   -39,  -329,  -329,   370,   346,
     -95,  -329,  -329,  -329,   -75,   -97,    -8,   -84,   297,   301,
     -62,  -170,  -160,   168,  -329,  -329,  -329,  -328,   -45,  -329,
    -329,   -81,    -6,     7,    -7,  -329,  -329,   424,   218,   337,
     147,  -329,   443,   -25,  -329,     0,     4,  -329,  -329
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -150
static const yytype_int16 yytable[] =
{
      84,    40,    54,   119,    66,    41,    67,   270,   121,    59,
     125,   279,   183,    60,    45,   196,    53,   218,   373,   216,
      72,   194,   320,   321,    61,   289,    59,   204,   290,   147,
      60,   187,    72,   159,    59,   159,    73,   160,    60,   160,
     161,   181,    81,   162,   379,   162,   142,    64,    73,   128,
     205,   195,   198,   170,   132,   202,   158,   285,   178,    66,
     141,   146,   201,    82,   123,   343,    44,   154,    82,   344,
      82,    82,     5,     6,   129,   199,    82,   154,    46,   158,
      47,   203,   124,   201,   206,   271,   190,    48,   191,    25,
     372,    71,    82,    62,   135,   233,    49,    51,   196,   218,
     135,    59,   207,   280,   277,    60,   212,   292,   331,    44,
     143,   230,   144,   192,   129,   253,   254,   255,   339,    79,
     129,   259,    59,   242,   294,   145,    60,   283,   284,   127,
     301,   261,   134,    44,   310,   278,   267,    80,    82,   116,
     268,     5,     6,   270,    63,   272,   302,    46,   288,    47,
     256,   304,    64,    64,   159,   309,    48,   117,   160,    88,
     212,   161,   213,    93,   162,    95,   287,   129,   137,   213,
       5,     6,   138,   148,   212,   214,    46,   118,    47,   193,
     293,   118,   282,   286,   139,    48,    82,   307,   212,    82,
     306,   212,   308,   343,    95,   332,   179,   344,   345,   335,
     336,     5,     6,   151,    63,   340,   341,    46,   180,    47,
     152,   342,   273,    64,    44,   274,    48,   149,     5,     6,
     353,   314,   156,   337,   338,    49,    47,   157,     5,     6,
     152,   323,   167,    48,    46,   297,    47,    64,   298,   172,
     206,   327,    49,    48,   299,   315,   316,   366,     4,   319,
       5,     6,    49,   367,   368,   369,   333,   334,   269,     4,
     173,     5,     6,   328,   324,   325,   298,    46,   174,    47,
     243,   212,   299,    44,   244,   250,    48,   153,   155,   251,
     175,   212,   355,   356,   357,    49,   176,   153,   155,   343,
     376,   212,   381,   344,   329,   330,   212,   238,   239,     5,
       6,   378,   177,   262,   230,    46,   263,    47,  -149,   264,
      82,   242,   245,   246,    48,   130,   247,    40,   383,   133,
     184,    41,    85,    49,     4,   197,     5,     6,     7,    86,
      87,   219,    46,     9,    47,   185,     5,     6,    88,    89,
     186,    48,    46,   220,    47,    90,    91,    92,    93,    94,
      95,    48,    96,   382,   232,   200,   320,   322,   364,   365,
      49,    85,   235,     4,    97,     5,     6,     7,    86,    87,
     236,    46,     9,    47,   237,   257,   258,    88,    89,   260,
      48,   192,   291,   300,    90,    91,    92,    93,    94,    95,
     303,    96,   311,   305,   234,   326,   351,   317,   350,   349,
      85,   318,     4,    97,     5,     6,     7,    86,    87,   354,
      46,     9,    47,   363,   361,   370,    88,    89,   374,    48,
     371,   375,   377,    90,    91,    92,    93,    94,    95,   380,
      96,     2,     3,    16,     4,    18,     5,     6,     7,   359,
     358,     8,    97,     9,     5,     6,    28,   313,   360,   171,
      46,   169,    47,    10,    11,   122,   206,   182,   265,    48,
     215,    12,   266,     5,     6,    13,   348,   362,    49,    46,
     131,    47,     0,   252,   352,   221,   120,     0,    48,     0,
       5,     6,   222,   223,   224,     0,    46,    49,    47,   225,
       0,     0,   312,     0,     0,    48,     0,     0,     0,   222,
     223,   224,     0,     0,    49,   188,   225,     5,     6,     0,
      63,     0,     0,    46,     0,    47,     0,     0,     0,    64,
       5,     6,    48,    63,     0,     0,    46,     0,    47,   189,
       0,    49,    88,     5,     6,    48,    63,     0,     0,    46,
       0,    47,    93,     0,    49,    64,     5,     6,    48,    63,
       0,     0,    46,     0,    47,    93,     0,    49,   281,     5,
       6,    48,     0,     0,     0,    46,     0,    47,     0,     0,
      49,    44,     5,     6,    48,     0,     0,     0,    46,     0,
      47,     0,     0,    49,   206,     0,     0,    48,     0,     0,
       0,     0,     0,     0,     0,     0,    49
};

static const yytype_int16 yycheck[] =
{
      25,     1,     9,    28,    12,     1,    12,   167,    33,     9,
      35,   181,    87,     9,     7,    96,     9,   114,   346,   114,
       1,    96,    38,    39,    23,   195,    26,   111,   198,    54,
      26,    93,     1,    11,    34,    11,    17,    15,    34,    15,
      18,    86,    17,    21,   372,    21,    53,    17,    17,    42,
     112,    96,    97,    34,    20,    38,    63,    27,    83,    67,
      53,    54,   101,    46,    18,    13,    17,    60,    46,    17,
      46,    46,     5,     6,    40,   100,    46,    70,    11,    86,
      13,   106,    36,   122,    17,   169,    94,    20,    94,     1,
      38,    18,    46,    17,    20,   120,    29,     9,   179,   196,
      20,   101,    35,   187,   179,   101,   113,   202,   278,    17,
      18,   118,    20,    33,    40,   140,   141,   142,   288,    18,
      40,   146,   122,   130,   208,    33,   122,   189,   190,    13,
      17,   156,    13,    17,   218,   180,   161,    36,    46,    18,
     165,     5,     6,   303,     8,   170,    33,    11,   193,    13,
     143,   213,    17,    17,    11,   217,    20,    36,    15,    17,
     167,    18,    27,    27,    21,    29,   191,    40,    13,    27,
       5,     6,    17,    45,   181,    33,    11,    37,    13,    43,
      37,    37,   188,   191,    29,    20,    46,    16,   195,    46,
     215,   198,    21,    13,    29,   279,    31,    17,    18,   283,
     284,     5,     6,    13,     8,   289,   290,    11,    43,    13,
      13,   295,    14,    17,    17,    17,    20,    32,     5,     6,
     304,   228,    17,   285,   286,    29,    13,    36,     5,     6,
      13,   256,    33,    20,    11,    35,    13,    17,    38,    18,
      17,   266,    29,    20,    44,   238,   239,   331,     3,   242,
       5,     6,    29,   337,   338,   339,   281,   282,    35,     3,
      38,     5,     6,    35,   257,   258,    38,    11,    21,    13,
      13,   278,    44,    17,    17,    13,    20,    59,    60,    17,
      36,   288,   307,   308,   309,    29,    18,    69,    70,    13,
     352,   298,   376,    17,    41,    42,   303,     3,     4,     5,
       6,   363,    16,    15,   311,    11,    18,    13,    33,    21,
      46,   318,    13,    14,    20,    43,    17,   317,   380,    47,
      37,   317,     1,    29,     3,    29,     5,     6,     7,     8,
       9,    36,    11,    12,    13,    37,     5,     6,    17,    18,
      37,    20,    11,    18,    13,    24,    25,    26,    27,    28,
      29,    20,    31,   378,    18,    34,    38,    39,   329,   330,
      29,     1,    36,     3,    43,     5,     6,     7,     8,     9,
      18,    11,    12,    13,    18,    32,    32,    17,    18,    32,
      20,    33,    17,    21,    24,    25,    26,    27,    28,    29,
      33,    31,    38,    32,    34,    15,    32,    39,    16,    44,
       1,    38,     3,    43,     5,     6,     7,     8,     9,    17,
      11,    12,    13,    35,    21,    19,    17,    18,    44,    20,
      19,    17,    35,    24,    25,    26,    27,    28,    29,    35,
      31,     0,     1,     1,     3,     1,     5,     6,     7,   317,
     311,    10,    43,    12,     5,     6,     1,   228,   318,    74,
      11,    67,    13,    22,    23,    34,    17,    87,   161,    20,
     114,    30,   161,     5,     6,    34,   298,   320,    29,    11,
      46,    13,    -1,   136,    35,    17,    33,    -1,    20,    -1,
       5,     6,    24,    25,    26,    -1,    11,    29,    13,    31,
      -1,    -1,    17,    -1,    -1,    20,    -1,    -1,    -1,    24,
      25,    26,    -1,    -1,    29,     3,    31,     5,     6,    -1,
       8,    -1,    -1,    11,    -1,    13,    -1,    -1,    -1,    17,
       5,     6,    20,     8,    -1,    -1,    11,    -1,    13,    27,
      -1,    29,    17,     5,     6,    20,     8,    -1,    -1,    11,
      -1,    13,    27,    -1,    29,    17,     5,     6,    20,     8,
      -1,    -1,    11,    -1,    13,    27,    -1,    29,    17,     5,
       6,    20,    -1,    -1,    -1,    11,    -1,    13,    -1,    -1,
      29,    17,     5,     6,    20,    -1,    -1,    -1,    11,    -1,
      13,    -1,    -1,    29,    17,    -1,    -1,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    29
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    48,     0,     1,     3,     5,     6,     7,    10,    12,
      22,    23,    30,    34,    49,    50,    51,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    69,
      70,    71,    72,    73,    74,    75,    79,    80,    82,    83,
     132,   133,   134,   135,    17,   120,    11,    13,    20,    29,
      52,    61,    64,   120,   121,   122,   123,   124,   125,   132,
     133,    23,    17,     8,    17,    81,   103,   119,   121,   132,
     133,    18,     1,    17,    84,    85,    86,    87,    90,    18,
      36,    17,    46,   129,   130,     1,     8,     9,    17,    18,
      24,    25,    26,    27,    28,    29,    31,    43,    51,    54,
      61,    91,    92,    93,    94,    95,    96,    97,    99,   100,
     101,   102,   103,   115,   118,   119,    18,    36,    37,   130,
     129,   130,    91,    18,    36,   130,   120,    13,   120,    40,
      76,   124,    20,    76,    13,    20,    76,    13,    17,    29,
     128,   120,   121,    18,    20,    33,   120,   130,    45,    32,
     117,    13,    13,   125,   120,   125,    17,    36,   121,    11,
      15,    18,    21,   104,   105,   106,   130,    33,   107,    81,
      34,    85,    18,    38,    21,    36,    18,    16,   130,    31,
      43,   115,    95,   101,    37,    37,    37,   107,     3,    27,
     103,   119,    33,    43,   101,   115,   118,    29,   115,   130,
      34,    92,    38,   130,   104,   107,    17,    35,   108,   109,
     110,   111,   121,    27,    33,    96,    97,    98,   102,    36,
      18,    17,    24,    25,    26,    31,    65,    66,    67,    68,
     121,   131,    18,   130,    34,    36,    18,    18,     3,     4,
      77,    78,   121,    13,    17,    13,    14,    17,   126,   127,
      13,    17,   126,   130,   130,   130,   120,    32,    32,   130,
      32,   130,    15,    18,    21,   105,   106,   130,   130,    35,
     109,   104,   130,    14,    17,    88,    89,   101,   115,   108,
     104,    17,   119,   107,   107,    27,   103,   130,   115,   108,
     108,    17,    97,    37,   104,   112,   116,    35,    38,    44,
      21,    17,    33,    33,   107,    32,   130,    16,    21,   107,
     104,    38,    17,    68,   121,   120,   120,    39,    38,   120,
      38,    39,    39,   130,   120,   120,    15,   130,    35,    41,
      42,   108,   104,   130,   130,   104,   104,   107,   107,   108,
     104,   104,   104,    13,    17,    18,   113,   114,   110,    44,
      16,    32,    35,   104,    17,   130,   130,   130,    66,    63,
      78,    21,   127,    35,    89,    89,   104,   104,   104,   104,
      19,    19,    38,   114,    44,    17,   107,    35,   107,   114,
      35,   104,   130,   107
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
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 12:
#line 134 "mta_parse.y"
    {
          // presumably leaving a namespace -- check that..
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 13:
#line 137 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
    break;

  case 14:
#line 141 "mta_parse.y"
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
#line 152 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 16:
#line 153 "mta_parse.y"
    {
	    if((yyvsp[(1) - (2)].typ) != NULL)  SETDESC((yyvsp[(1) - (2)].typ),(yyvsp[(2) - (2)].chr)); }
    break;

  case 17:
#line 157 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 18:
#line 158 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(2) - (4)].typ); (yyvsp[(2) - (4)].typ)->name = (yyvsp[(3) - (4)].chr); mta->type_stack.Pop(); }
    break;

  case 19:
#line 162 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); (yyvsp[(2) - (3)].typ)->AddParent((yyvsp[(1) - (3)].typ)); (yyvsp[(2) - (3)].typ)->AssignType((yyvsp[(1) - (3)].typ)->type);
	    mta->type_stack.Pop(); }
    break;

  case 20:
#line 165 "mta_parse.y"
    { /* annoying place for a comment, but.. */
            (yyval.typ) = (yyvsp[(3) - (4)].typ); (yyvsp[(3) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(3) - (4)].typ)->AssignType((yyvsp[(1) - (4)].typ)->type);
	    mta->type_stack.Pop(); }
    break;

  case 21:
#line 170 "mta_parse.y"
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

  case 22:
#line 179 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (7)].typ); (yyval.typ)->AssignType(TypeDef::FUN_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 23:
#line 182 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (4)].typ); (yyval.typ)->AssignType(TypeDef::METH_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 24:
#line 185 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 25:
#line 187 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 26:
#line 191 "mta_parse.y"
    {
            TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
            (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("enum", sp, (yyval.typ)); (yyval.typ)->source_end = mta->line-1; } }
    break;

  case 29:
#line 201 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); }
    break;

  case 30:
#line 202 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 31:
#line 205 "mta_parse.y"
    {
  	    (yyval.typ) = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::ENUM); mta->cur_enum = (yyvsp[(2) - (2)].typ);
	    mta->SetSource((yyval.typ), false); mta->type_stack.Pop(); }
    break;

  case 32:
#line 209 "mta_parse.y"
    {
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->cur_enum = (yyval.typ);
	    mta->SetSource((yyval.typ), false); (yyval.typ)->AssignType(TypeDef::ENUM); }
    break;

  case 33:
#line 216 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("class", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 34:
#line 224 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 35:
#line 225 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 36:
#line 229 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("class", sp, (yyval.typ)); mta->FixClassTypes((yyval.typ));
              (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 39:
#line 243 "mta_parse.y"
    {
	    if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	    else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 40:
#line 249 "mta_parse.y"
    {
	    (yyvsp[(1) - (2)].typ)->tokens.keep = true; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (2)].typ), true); }
    break;

  case 41:
#line 252 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
            mta->SetSource((yyvsp[(1) - (3)].typ), true); }
    break;

  case 42:
#line 255 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (3)].typ), true); 
	  }
    break;

  case 43:
#line 262 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 44:
#line 263 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 45:
#line 266 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::CLASS); mta->ClearSource((yyvsp[(2) - (2)].typ)); /* tyname set -- premature */
            mta->cur_mstate = MTA::prvt; }
    break;

  case 46:
#line 271 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 47:
#line 275 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::CLASS); 
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 48:
#line 282 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::STRUCT);
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 49:
#line 289 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->AssignType(TypeDef::CLASS); mta->ClearSource((yyvsp[(2) - (2)].typ)); /* tyname set -- premature */
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 50:
#line 294 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->AssignType(TypeDef::UNION);
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 51:
#line 304 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].typ) != NULL) mta->cur_class->AddParent((yyvsp[(1) - (1)].typ)); }
    break;

  case 52:
#line 306 "mta_parse.y"
    {
            if((yyvsp[(3) - (3)].typ) != NULL) {mta->cur_class->AddParent((yyvsp[(3) - (3)].typ));
	      if(!mta->cur_class->HasOption("MULT_INHERIT"))
                mta->cur_class->opts.Add("MULT_INHERIT"); } }
    break;

  case 54:
#line 314 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 55:
#line 315 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 56:
#line 316 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 61:
#line 328 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ)); }
	    mta->type_stack.Pop(); }
    break;

  case 62:
#line 336 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 63:
#line 337 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 64:
#line 341 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ));
              mta->FixClassTypes((yyval.typ));
	      (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 67:
#line 356 "mta_parse.y"
    {
          if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	  else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 68:
#line 362 "mta_parse.y"
    {
            mta->state = MTA::Parse_inclass; (yyvsp[(1) - (2)].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 69:
#line 365 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 70:
#line 367 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 71:
#line 372 "mta_parse.y"
    {
	    (yyvsp[(5) - (5)].typ)->templ_pars.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_pars.Duplicate(mta->cur_templ_pars);
	    (yyvsp[(5) - (5)].typ)->SetType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(5) - (5)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 72:
#line 379 "mta_parse.y"
    { mta->cur_templ_pars.Reset(); }
    break;

  case 74:
#line 384 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 75:
#line 388 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 76:
#line 389 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 77:
#line 390 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 78:
#line 391 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (3)].typ)); (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 79:
#line 394 "mta_parse.y"
    {
            if(mta->cur_is_trg) { /* only add reg_funs in target space */
              TypeDef* nt = new TypeDef((yyvsp[(1) - (1)].meth)->name, TypeDef::FUNCTION,0,0);
              mta->SetSource(nt, false);
              taMisc::types.Add(nt);
              nt->methods.AddUniqNameNew((yyvsp[(1) - (1)].meth));
              taMisc::reg_funs.Link(nt); }
            mta->meth_stack.Pop(); }
    break;

  case 80:
#line 404 "mta_parse.y"
    {
            (yyvsp[(3) - (4)].meth)->type = (yyvsp[(2) - (4)].typ); SETDESC((yyvsp[(3) - (4)].meth),(yyvsp[(4) - (4)].chr)); (yyval.meth) = (yyvsp[(3) - (4)].meth); }
    break;

  case 81:
#line 406 "mta_parse.y"
    {
            (yyvsp[(2) - (3)].meth)->type = &TA_int; SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); (yyval.meth) = (yyvsp[(2) - (3)].meth); }
    break;

  case 82:
#line 410 "mta_parse.y"
    {
            (yyvsp[(1) - (2)].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = true; }
    break;

  case 83:
#line 415 "mta_parse.y"
    {
            taMisc::Info("using namespace:", (yyvsp[(3) - (4)].chr));
          }
    break;

  case 84:
#line 420 "mta_parse.y"
    {
            taMisc::Info("entering namespace:", (yyvsp[(2) - (3)].chr));
          }
    break;

  case 88:
#line 430 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 91:
#line 435 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 92:
#line 440 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 93:
#line 443 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm)); if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
	    mta->enum_stack.Pop(); }
    break;

  case 95:
#line 450 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 96:
#line 455 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 97:
#line 463 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 99:
#line 468 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 100:
#line 471 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 101:
#line 472 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 102:
#line 475 "mta_parse.y"
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

  case 103:
#line 484 "mta_parse.y"
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

  case 104:
#line 497 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
            mta->TypeAdded("enum", &(mta->cur_class->sub_types), (yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->source_end = mta->line-1;
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 105:
#line 503 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 106:
#line 507 "mta_parse.y"
    { /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew((yyvsp[(1) - (2)].typ));
            (yyvsp[(1) - (2)].typ)->SetType(TypeDef::SUBTYPE);
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 107:
#line 512 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 109:
#line 517 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 110:
#line 518 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 111:
#line 519 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 112:
#line 520 "mta_parse.y"
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

  case 114:
#line 541 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 115:
#line 547 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 116:
#line 548 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 117:
#line 549 "mta_parse.y"
    { }
    break;

  case 118:
#line 550 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
            int spsz = sp->size;
            TypeDef* nty = (yyvsp[(1) - (4)].typ)->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    (yyvsp[(2) - (4)].memb)->type = nty; }
    break;

  case 119:
#line 557 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 120:
#line 561 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 121:
#line 565 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(3) - (3)].memb)->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 122:
#line 571 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 123:
#line 577 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 125:
#line 583 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 126:
#line 584 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 127:
#line 585 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 128:
#line 586 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 129:
#line 587 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 130:
#line 588 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 131:
#line 589 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 132:
#line 590 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 133:
#line 591 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 134:
#line 592 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 135:
#line 593 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 136:
#line 594 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 137:
#line 595 "mta_parse.y"
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

  case 138:
#line 605 "mta_parse.y"
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

  case 140:
#line 619 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 141:
#line 620 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 142:
#line 622 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 143:
#line 627 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 144:
#line 628 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
    break;

  case 145:
#line 629 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 146:
#line 630 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 147:
#line 631 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 148:
#line 634 "mta_parse.y"
    {
             (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 149:
#line 641 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 150:
#line 645 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 151:
#line 646 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
    break;

  case 152:
#line 647 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 153:
#line 648 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 154:
#line 649 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 155:
#line 650 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 156:
#line 654 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 157:
#line 655 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 158:
#line 656 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
    break;

  case 159:
#line 660 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 160:
#line 661 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 161:
#line 662 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 162:
#line 665 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 163:
#line 666 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 164:
#line 669 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 165:
#line 670 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 166:
#line 673 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 167:
#line 674 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 168:
#line 675 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 169:
#line 678 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 170:
#line 680 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      String ad = (yyvsp[(2) - (2)].chr);
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 171:
#line 691 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 172:
#line 695 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 173:
#line 699 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 174:
#line 703 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 175:
#line 707 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 183:
#line 728 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 184:
#line 732 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 185:
#line 735 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 186:
#line 736 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 187:
#line 739 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 189:
#line 743 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 190:
#line 744 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 191:
#line 747 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
                                  mta->SetSource((yyval.typ), false); }
    break;

  case 193:
#line 752 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ)->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, (yyval.typ)); }
	  }
    break;

  case 195:
#line 762 "mta_parse.y"
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

  case 197:
#line 774 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(2) - (2)].typ)->GetConstType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("const", sp, (yyval.typ)); }
	  }
    break;

  case 199:
#line 783 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::STRUCT);
          (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 200:
#line 786 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::UNION);
            (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 201:
#line 788 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::UNION); }
    break;

  case 202:
#line 789 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 203:
#line 793 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 204:
#line 797 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 205:
#line 798 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 206:
#line 799 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 208:
#line 801 "mta_parse.y"
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

  case 209:
#line 818 "mta_parse.y"
    { /* this template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
	      yyerror("Template syntax error"); YYERROR; }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 211:
#line 825 "mta_parse.y"
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

  case 213:
#line 840 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 214:
#line 845 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 215:
#line 846 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 216:
#line 847 "mta_parse.y"
    { (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 218:
#line 852 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 219:
#line 853 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 226:
#line 869 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 227:
#line 873 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 228:
#line 877 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 229:
#line 881 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 3058 "y.tab.c"
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


#line 884 "mta_parse.y"


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

