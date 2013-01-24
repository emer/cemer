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
     CLASS = 258,
     TYPENAME = 259,
     STRUCT = 260,
     UNION = 261,
     ENUM = 262,
     FUNTYPE = 263,
     STATIC = 264,
     TEMPLATE = 265,
     CONST = 266,
     TYPEDEF = 267,
     TA_TYPEDEF = 268,
     TYPE = 269,
     NUMBER = 270,
     FUNCTION = 271,
     ARRAY = 272,
     NAME = 273,
     COMMENT = 274,
     FUNCALL = 275,
     SCOPER = 276,
     EQUALS = 277,
     PUBLIC = 278,
     PRIVATE = 279,
     PROTECTED = 280,
     OPERATOR = 281,
     FRIEND = 282,
     THISNAME = 283,
     REGFUN = 284,
     VIRTUAL = 285
   };
#endif
/* Tokens.  */
#define CLASS 258
#define TYPENAME 259
#define STRUCT 260
#define UNION 261
#define ENUM 262
#define FUNTYPE 263
#define STATIC 264
#define TEMPLATE 265
#define CONST 266
#define TYPEDEF 267
#define TA_TYPEDEF 268
#define TYPE 269
#define NUMBER 270
#define FUNCTION 271
#define ARRAY 272
#define NAME 273
#define COMMENT 274
#define FUNCALL 275
#define SCOPER 276
#define EQUALS 277
#define PUBLIC 278
#define PRIVATE 279
#define PROTECTED 280
#define OPERATOR 281
#define FRIEND 282
#define THISNAME 283
#define REGFUN 284
#define VIRTUAL 285




/* Copy the first part of user declarations.  */
#line 18 "mta_parse.y"


#include "maketa.h"

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
#line 44 "mta_parse.y"
{
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  const char*   chr;
  int	   	rval;
}
/* Line 193 of yacc.c.  */
#line 188 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 201 "y.tab.c"

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
#define YYNTOKENS  46
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  85
/* YYNRULES -- Number of rules.  */
#define YYNRULES  215
/* YYNRULES -- Number of states.  */
#define YYNSTATES  360

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   285

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    44,     2,
      32,    33,    31,    40,    37,    41,    43,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    36,    45,
      39,     2,    38,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    35,     2,    34,    42,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    19,    22,
      25,    28,    30,    32,    34,    37,    40,    45,    49,    54,
      57,    65,    70,    72,    77,    80,    84,    88,    91,    93,
      95,    98,   102,   104,   107,   111,   115,   118,   122,   126,
     128,   132,   135,   138,   140,   142,   144,   146,   150,   152,
     155,   157,   160,   162,   165,   167,   169,   171,   174,   178,
     182,   185,   189,   193,   199,   201,   203,   207,   210,   213,
     216,   220,   222,   227,   231,   234,   236,   239,   241,   244,
     246,   249,   251,   253,   257,   259,   263,   267,   269,   271,
     273,   275,   278,   280,   282,   284,   286,   289,   291,   293,
     296,   299,   302,   304,   306,   309,   313,   318,   321,   326,
     331,   333,   337,   339,   344,   346,   350,   355,   360,   365,
     370,   376,   382,   388,   393,   397,   402,   407,   413,   418,
     420,   423,   426,   430,   434,   437,   442,   449,   453,   456,
     458,   460,   462,   465,   468,   471,   475,   477,   480,   484,
     486,   488,   491,   494,   498,   500,   503,   505,   509,   514,
     516,   519,   521,   524,   528,   535,   537,   539,   542,   546,
     549,   551,   553,   556,   558,   560,   562,   565,   567,   569,
     572,   574,   577,   579,   582,   584,   587,   590,   593,   596,
     600,   604,   608,   612,   615,   617,   622,   627,   629,   632,
     634,   638,   640,   642,   644,   646,   648,   650,   653,   655,
     657,   659,   661,   663,   665,   667
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      47,     0,    -1,    -1,    47,    49,    -1,    47,    53,    -1,
      47,    57,    -1,    47,    59,    -1,    47,    69,    -1,    47,
      77,    -1,    47,    48,    -1,    47,     1,    -1,    13,    -1,
      50,    -1,    51,    -1,    51,    19,    -1,    12,    52,    -1,
      12,    61,   123,   125,    -1,   116,   115,   125,    -1,   116,
      19,   115,   125,    -1,   116,   125,    -1,   116,    32,    31,
     115,    33,   103,   125,    -1,   116,    21,    31,   115,    -1,
      54,    -1,    55,    80,    34,   125,    -1,    56,    35,    -1,
      56,    19,    35,    -1,    56,    35,    19,    -1,     7,   115,
      -1,     7,    -1,    58,    -1,    64,   125,    -1,    64,   125,
      19,    -1,    60,    -1,    61,   125,    -1,    61,   124,   125,
      -1,    62,    87,    34,    -1,    63,    35,    -1,    63,    19,
      35,    -1,    63,    35,    19,    -1,    64,    -1,    64,    36,
      65,    -1,   129,   115,    -1,   129,    14,    -1,   129,    -1,
     127,    -1,   128,    -1,    66,    -1,    65,    37,    66,    -1,
     116,    -1,    67,   116,    -1,    18,    -1,    67,    18,    -1,
      68,    -1,    67,    68,    -1,   126,    -1,    30,    -1,    70,
      -1,    71,   125,    -1,    71,   124,   125,    -1,    72,    87,
      34,    -1,    73,    35,    -1,    73,    19,    35,    -1,    73,
      35,    19,    -1,   130,    74,    75,    38,    63,    -1,    39,
      -1,    76,    -1,    76,    37,    76,    -1,     3,   115,    -1,
       4,   115,    -1,   116,   115,    -1,   116,   115,    22,    -1,
      78,    -1,    29,   114,    79,   100,    -1,    29,    79,   100,
      -1,    99,   103,    -1,    81,    -1,    80,    81,    -1,    82,
      -1,    82,    19,    -1,    83,    -1,    83,    37,    -1,     1,
      -1,    86,    -1,    86,    22,    84,    -1,    85,    -1,    84,
      40,    85,    -1,    84,    41,    85,    -1,    18,    -1,    15,
      -1,    18,    -1,    88,    -1,    87,    88,    -1,    89,    -1,
      95,    -1,    54,    -1,    51,    -1,    61,   125,    -1,     1,
      -1,    90,    -1,    23,    36,    -1,    24,    36,    -1,    25,
      36,    -1,    19,    -1,    91,    -1,     9,    91,    -1,   113,
      92,   125,    -1,   113,    93,    22,   125,    -1,    92,   125,
      -1,   113,    93,    17,   125,    -1,   113,    94,   103,   125,
      -1,    93,    -1,    92,    37,    93,    -1,    18,    -1,    32,
      31,    18,    33,    -1,    96,    -1,   110,   104,   100,    -1,
       8,   110,   104,   100,    -1,    30,   110,   104,   100,    -1,
     110,   104,   108,   100,    -1,    42,   110,   104,   100,    -1,
       8,    42,   110,   104,   100,    -1,    30,    42,   110,   104,
     100,    -1,    27,   114,    26,   103,   100,    -1,    27,    26,
     103,   100,    -1,    27,   114,   125,    -1,    27,     3,   114,
     125,    -1,    27,     3,    18,   125,    -1,    27,   114,    99,
     103,   100,    -1,    27,    99,   103,   100,    -1,    97,    -1,
       9,    97,    -1,    30,    97,    -1,     8,    30,    97,    -1,
     113,    98,   100,    -1,    98,   100,    -1,   113,    26,   103,
     100,    -1,   113,    26,    32,    33,   103,   100,    -1,    26,
     103,   100,    -1,    99,   103,    -1,    18,    -1,   125,    -1,
     101,    -1,   102,   125,    -1,    19,   125,    -1,    19,   101,
      -1,    19,   102,   125,    -1,    16,    -1,    11,    16,    -1,
      11,    19,    16,    -1,    22,    -1,    11,    -1,    11,    22,
      -1,    32,    33,    -1,    32,   105,    33,    -1,    33,    -1,
     105,    33,    -1,   106,    -1,   105,    37,   106,    -1,   105,
      43,    43,    43,    -1,   107,    -1,   107,    22,    -1,   116,
      -1,   116,    18,    -1,   116,    18,    17,    -1,   116,    32,
      31,    18,    33,   103,    -1,    18,    -1,   109,    -1,   108,
     109,    -1,   111,    14,    20,    -1,    28,    32,    -1,    36,
      -1,    31,    -1,   112,    31,    -1,   114,    -1,   116,    -1,
       8,    -1,     8,   116,    -1,    18,    -1,   117,    -1,   117,
      44,    -1,   118,    -1,   118,   112,    -1,   119,    -1,    11,
     119,    -1,   120,    -1,   127,   120,    -1,   127,   115,    -1,
     128,   120,    -1,   128,   115,    -1,    14,    21,    18,    -1,
      28,    21,    18,    -1,    14,    21,    14,    -1,    28,    21,
      14,    -1,    21,    14,    -1,    28,    -1,    14,    74,   121,
      38,    -1,    28,    74,   121,    38,    -1,    14,    -1,   120,
      14,    -1,   122,    -1,   121,    37,   122,    -1,    14,    -1,
      18,    -1,    15,    -1,    18,    -1,    14,    -1,    18,    -1,
      18,    17,    -1,    45,    -1,    23,    -1,    24,    -1,    25,
      -1,     5,    -1,     6,    -1,     3,    -1,    10,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   112,   112,   113,   116,   118,   121,   124,   126,   128,
     130,   135,   138,   146,   147,   151,   152,   156,   159,   164,
     198,   201,   206,   212,   215,   216,   217,   220,   224,   231,
     239,   240,   244,   253,   254,   258,   264,   267,   270,   277,
     278,   281,   286,   290,   297,   304,   314,   316,   323,   324,
     325,   326,   329,   330,   333,   334,   338,   347,   348,   352,
     358,   361,   363,   368,   375,   379,   380,   384,   385,   386,
     387,   390,   396,   398,   402,   407,   408,   411,   412,   415,
     416,   417,   422,   425,   431,   432,   437,   445,   446,   450,
     453,   454,   457,   466,   479,   482,   485,   489,   493,   494,
     495,   496,   497,   517,   518,   524,   525,   526,   527,   534,
     538,   542,   548,   554,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   580,   591,
     592,   593,   595,   600,   601,   602,   603,   604,   607,   614,
     618,   619,   620,   621,   622,   623,   627,   628,   629,   633,
     634,   635,   638,   639,   642,   643,   646,   647,   648,   651,
     653,   664,   668,   672,   676,   680,   687,   688,   691,   695,
     699,   702,   703,   706,   709,   710,   711,   714,   718,   719,
     728,   729,   740,   741,   749,   750,   751,   752,   753,   754,
     758,   762,   763,   764,   765,   766,   783,   789,   790,   802,
     803,   808,   809,   810,   814,   815,   818,   819,   822,   825,
     826,   827,   831,   835,   839,   843
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CLASS", "TYPENAME", "STRUCT", "UNION",
  "ENUM", "FUNTYPE", "STATIC", "TEMPLATE", "CONST", "TYPEDEF",
  "TA_TYPEDEF", "TYPE", "NUMBER", "FUNCTION", "ARRAY", "NAME", "COMMENT",
  "FUNCALL", "SCOPER", "EQUALS", "PUBLIC", "PRIVATE", "PROTECTED",
  "OPERATOR", "FRIEND", "THISNAME", "REGFUN", "VIRTUAL", "'*'", "'('",
  "')'", "'}'", "'{'", "':'", "','", "'>'", "'<'", "'+'", "'-'", "'~'",
  "'.'", "'&'", "';'", "$accept", "list", "preparsed", "typedefn",
  "typedefns", "typedsub", "defn", "enumdefn", "enumdsub", "enumname",
  "enumnm", "classdecl", "classdecls", "classdefn", "classdefns",
  "classdsub", "classname", "classhead", "classnm", "classinh", "classpar",
  "classptyp", "classpmod", "templdefn", "templdefns", "templdsub",
  "templname", "templhead", "templopen", "templpars", "templpar",
  "fundecl", "funnm", "regfundefn", "enums", "enumline", "enumitm",
  "enumitms", "enummath", "enummathitm", "enmitmname", "membs", "membline",
  "membdefn", "basicmemb", "nostatmemb", "membnames", "membname",
  "membfunp", "methdefn", "basicmeth", "nostatmeth", "mbfundefn",
  "methname", "fundefn", "funsubdefn", "funsubdecl", "funargs", "constfun",
  "args", "argdefn", "subargdefn", "constrlist", "constref", "consthsnm",
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
     285,    42,    40,    41,   125,   123,    58,    44,    62,    60,
      43,    45,   126,    46,    38,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    46,    47,    47,    47,    47,    47,    47,    47,    47,
      47,    48,    49,    50,    50,    51,    51,    52,    52,    52,
      52,    52,    53,    54,    55,    55,    55,    56,    56,    57,
      58,    58,    59,    60,    60,    61,    62,    62,    62,    63,
      63,    64,    64,    64,    64,    64,    65,    65,    66,    66,
      66,    66,    67,    67,    68,    68,    69,    70,    70,    71,
      72,    72,    72,    73,    74,    75,    75,    76,    76,    76,
      76,    77,    78,    78,    79,    80,    80,    81,    81,    82,
      82,    82,    83,    83,    84,    84,    84,    85,    85,    86,
      87,    87,    88,    88,    88,    88,    88,    88,    89,    89,
      89,    89,    89,    90,    90,    91,    91,    91,    91,    91,
      92,    92,    93,    94,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    96,
      96,    96,    96,    97,    97,    97,    97,    97,    98,    99,
     100,   100,   100,   100,   100,   100,   101,   101,   101,   102,
     102,   102,   103,   103,   104,   104,   105,   105,   105,   106,
     106,   107,   107,   107,   107,   107,   108,   108,   109,   110,
     111,   112,   112,   113,   114,   114,   114,   115,   116,   116,
     117,   117,   118,   118,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   120,   120,   121,
     121,   122,   122,   122,   123,   123,   124,   124,   125,   126,
     126,   126,   127,   128,   129,   130
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     1,     1,     2,     2,     4,     3,     4,     2,
       7,     4,     1,     4,     2,     3,     3,     2,     1,     1,
       2,     3,     1,     2,     3,     3,     2,     3,     3,     1,
       3,     2,     2,     1,     1,     1,     1,     3,     1,     2,
       1,     2,     1,     2,     1,     1,     1,     2,     3,     3,
       2,     3,     3,     5,     1,     1,     3,     2,     2,     2,
       3,     1,     4,     3,     2,     1,     2,     1,     2,     1,
       2,     1,     1,     3,     1,     3,     3,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     2,     1,     1,     2,
       2,     2,     1,     1,     2,     3,     4,     2,     4,     4,
       1,     3,     1,     4,     1,     3,     4,     4,     4,     4,
       5,     5,     5,     4,     3,     4,     4,     5,     4,     1,
       2,     2,     3,     3,     2,     4,     6,     3,     2,     1,
       1,     1,     2,     2,     2,     3,     1,     2,     3,     1,
       1,     2,     2,     3,     1,     2,     1,     3,     4,     1,
       2,     1,     2,     3,     6,     1,     1,     2,     3,     2,
       1,     1,     2,     1,     1,     1,     2,     1,     1,     2,
       1,     2,     1,     2,     1,     2,     2,     2,     2,     3,
       3,     3,     3,     2,     1,     4,     4,     1,     2,     1,
       3,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    10,   214,   212,   213,    28,   215,     0,
      11,     0,     9,     3,    12,    13,     4,    22,     0,     0,
       5,    29,     6,    32,     0,     0,     0,    39,     7,    56,
       0,     0,     0,     8,    71,    44,    45,    43,     0,   177,
      27,     0,   197,     0,   194,    15,     0,    39,     0,   178,
     180,   182,   184,    44,    45,   175,   139,     0,     0,     0,
     174,     0,     0,    14,    81,    89,     0,    75,    77,    79,
      82,     0,    24,   206,   208,     0,    33,    97,   175,     0,
     112,   102,     0,     0,     0,     0,     0,   194,     0,     0,
      95,    94,     0,     0,    90,    92,    98,   103,     0,   110,
      93,   114,   129,     0,     0,     0,     0,   173,     0,    36,
       0,    30,     0,    57,     0,     0,    60,    42,    41,    64,
       0,   183,     0,     0,   193,     0,     0,   205,   204,     0,
       0,     0,     0,     0,    19,   179,   171,   181,   198,   197,
     186,   185,   188,   187,   176,   150,   146,     0,   149,    73,
     141,     0,   140,     0,    74,     0,     0,    76,    78,    80,
       0,    25,    26,   207,    34,     0,     0,     0,   104,   130,
      99,   100,   101,     0,     0,     0,     0,     0,   169,     0,
     131,     0,     0,     0,     0,    96,    35,    91,     0,   107,
     134,   138,   165,   154,     0,     0,   156,   159,   161,     0,
       0,     0,   110,     0,     0,    37,    38,    50,   209,   210,
     211,    55,    40,    46,     0,    52,    48,    54,    31,    58,
      59,    61,    62,     0,     0,     0,    65,     0,   191,   189,
     201,   203,   202,     0,   199,   192,   190,     0,    16,     0,
       0,     0,    17,   172,   147,     0,   151,   144,     0,   143,
     142,   152,     0,    72,    23,    88,    87,    83,    84,   132,
       0,     0,   137,     0,     0,     0,     0,     0,     0,   124,
       0,     0,     0,   112,   111,   170,   115,     0,   166,     0,
     155,     0,     0,   160,   162,     0,     0,     0,     0,   105,
       0,     0,     0,   133,     0,    51,    53,    49,    67,    68,
       0,     0,    69,     0,   195,   196,    18,    21,     0,   148,
     145,   153,     0,     0,     0,   116,   126,   125,   123,   128,
       0,     0,     0,   117,   119,   118,   167,     0,   157,     0,
     163,     0,   152,   135,     0,   108,   106,   109,    47,    63,
      66,    70,   200,     0,    85,    86,   120,   122,   127,   121,
     168,   158,     0,     0,   113,     0,     0,   136,    20,   164
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    12,    13,    14,    90,    45,    16,    91,    18,
      19,    20,    21,    22,    23,    92,    25,    26,    47,   212,
     213,   214,   215,    28,    29,    30,    31,    32,   126,   225,
     226,    33,    34,    57,    66,    67,    68,    69,   257,   258,
      70,    93,    94,    95,    96,    97,    98,    99,   203,   100,
     101,   102,   103,   104,   149,   150,   151,   154,   194,   195,
     196,   197,   277,   278,   105,   279,   137,   106,   107,   140,
      60,    49,    50,    51,    52,   233,   234,   129,    75,   152,
     217,    61,    62,    37,    38
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -149
static const yytype_int16 yypact[] =
{
    -149,   184,  -149,  -149,  -149,  -149,  -149,    10,  -149,   216,
    -149,   513,  -149,  -149,  -149,    52,  -149,  -149,    18,    84,
    -149,  -149,  -149,  -149,    17,   373,   123,    20,  -149,  -149,
      17,   373,   132,  -149,  -149,  -149,  -149,    27,    -5,  -149,
    -149,    64,    51,    72,    85,  -149,    59,    62,   129,    60,
     144,  -149,   140,   243,   243,   249,  -149,   117,   150,   177,
    -149,   243,   243,  -149,  -149,  -149,    15,  -149,   188,   180,
     209,   198,   217,   221,  -149,   219,  -149,  -149,    33,   483,
     241,  -149,   248,   250,   263,   150,   472,    81,   138,   261,
    -149,  -149,   219,   309,  -149,  -149,  -149,  -149,    13,  -149,
    -149,  -149,  -149,   117,   150,   276,   147,  -149,   267,   286,
     419,   287,   219,  -149,   341,   272,   289,  -149,  -149,  -149,
     197,  -149,   251,   238,  -149,   257,   238,  -149,  -149,   219,
      10,   280,   282,   219,  -149,  -149,  -149,   288,  -149,  -149,
    -149,   140,  -149,   140,  -149,   141,  -149,    44,  -149,  -149,
    -149,   219,  -149,   399,  -149,   117,   219,  -149,  -149,  -149,
     208,  -149,  -149,  -149,  -149,   502,   261,   276,  -149,  -149,
    -149,  -149,  -149,   117,   531,   150,   150,     3,  -149,   261,
    -149,   276,   111,   290,   276,  -149,  -149,  -149,   306,  -149,
    -149,  -149,  -149,  -149,    65,    97,  -149,   303,    93,   294,
     298,    13,   159,   150,   117,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,   301,  -149,   446,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,    10,    10,   302,   304,    10,  -149,  -149,
    -149,  -149,  -149,   168,  -149,  -149,  -149,   213,  -149,   219,
      10,    10,  -149,  -149,  -149,   329,  -149,  -149,   219,  -149,
    -149,  -149,   135,  -149,  -149,  -149,  -149,   218,  -149,  -149,
     276,   117,  -149,   219,   219,   117,   117,   150,   150,  -149,
     276,   117,   117,  -149,  -149,  -149,  -149,    65,  -149,   340,
    -149,   542,   313,  -149,   344,   326,   417,   117,   345,  -149,
     219,   219,   219,  -149,   419,  -149,  -149,  -149,  -149,  -149,
     273,   197,   336,   238,  -149,  -149,  -149,  -149,   337,  -149,
    -149,  -149,   208,   208,   117,  -149,  -149,  -149,  -149,  -149,
     117,   117,   117,  -149,  -149,  -149,  -149,   352,  -149,   330,
    -149,   359,   150,  -149,   353,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,   150,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,   355,   117,  -149,   219,   150,  -149,  -149,  -149
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -149,  -149,  -149,  -149,  -149,   388,  -149,  -149,   389,  -149,
    -149,  -149,  -149,  -149,  -149,   211,  -149,    95,   392,  -149,
     108,  -149,   192,  -149,  -149,  -149,  -149,  -149,   253,  -149,
     106,  -149,  -149,   349,  -149,   343,  -149,  -149,  -149,   -20,
    -149,   380,   -50,  -149,  -149,   333,   308,   -92,  -149,  -149,
    -149,   -66,   -91,    -7,   -73,   269,   271,   -58,  -144,  -148,
     145,  -149,  -149,   142,   -71,  -149,  -149,   -68,    -3,     5,
       2,  -149,  -149,   393,   181,   295,   126,  -149,   406,   -24,
    -149,     0,     1,  -149,  -149
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -140
static const yytype_int16 yytable[] =
{
      76,    35,    36,   111,    58,   252,   113,   167,    59,    53,
      54,    48,    40,   169,   202,   204,    64,   181,   184,    64,
     182,    56,   180,   261,   134,    53,    54,   173,    39,   267,
     190,    53,    54,    65,   119,    73,    65,   271,     5,     6,
     272,   117,   118,   187,    41,    39,   191,    42,    74,   156,
     188,   164,    58,   133,    43,   145,   110,   144,    74,   142,
     146,    87,    74,   165,   187,    74,   148,   142,   185,     5,
       6,    63,   122,   127,   189,   166,   145,   128,    42,   176,
     144,   146,   253,   177,   147,    43,   124,   148,   219,    74,
     119,   204,    44,    53,    54,   260,   274,   182,   110,   259,
     262,   275,   125,    71,   135,   238,   125,   198,   270,   242,
      74,   284,   216,   178,    53,    54,   314,   265,   266,    72,
     119,   276,   227,   249,   119,   285,   322,   250,   145,    56,
     280,   293,   254,   146,   281,   239,   147,   199,   252,   148,
     282,   287,   108,     5,     6,   292,    55,    39,   130,    41,
     131,   115,    42,   269,   138,   198,    56,   244,   109,    43,
     245,   132,    74,   246,    85,    80,    87,   116,   311,   198,
     268,   264,   281,   199,    74,   136,   290,   289,   282,   200,
     179,   291,   153,   198,     2,     3,   198,     4,   315,     5,
       6,     7,   318,   319,     8,    56,     9,    10,   323,   324,
     223,   224,     5,     6,   325,   303,   304,   158,    41,   320,
     321,    42,    24,    11,   333,   306,   297,   159,    43,     4,
      46,     5,     6,   255,   310,    44,   256,    41,   298,   299,
      42,   160,   302,   161,   141,   143,   162,    43,   163,   316,
     317,   346,   141,   143,    44,   307,   308,   347,   348,   349,
     303,   305,   230,   231,     5,     6,   232,   139,   312,   313,
      41,    39,   198,    42,    74,   228,   335,   336,   337,   229,
      43,   235,   198,  -139,   353,   236,     4,    44,     5,     6,
     357,     5,     6,   198,   170,   355,   171,    41,   198,   183,
      42,   120,   344,   345,   192,   123,   216,    43,   359,   172,
      35,    36,   205,   227,    44,   206,   218,   221,   222,   193,
      77,   240,     4,   241,     5,     6,     7,    78,    79,   243,
      41,     9,   178,    42,   273,   283,   286,    80,    81,   288,
      43,   358,    82,    83,    84,    85,    86,    87,   294,    88,
     300,   301,    77,   186,     4,   309,     5,     6,     7,    78,
      79,    89,    41,     9,   327,    42,   329,   331,   341,    80,
      81,   330,    43,   334,    82,    83,    84,    85,    86,    87,
     343,    88,   350,   351,    77,   220,     4,   352,     5,     6,
       7,    78,    79,    89,    41,     9,   354,    42,   356,    15,
      17,    80,    81,    27,    43,   339,    82,    83,    84,    85,
      86,    87,   338,    88,     5,     6,   296,   340,   155,   157,
      41,   114,   168,    42,   201,    89,   247,   192,   248,   326,
      43,   237,     5,     6,     5,     6,   328,    44,    41,   342,
      41,    42,   251,    42,   121,   192,   112,   207,    43,     0,
      43,     0,   208,   209,   210,    44,     0,    44,     0,   211,
     332,     5,     6,     0,     0,     0,     0,    41,     0,     0,
      42,     0,     0,     0,   295,     0,     0,    43,     0,   208,
     209,   210,     0,     0,    44,   174,   211,     5,     6,     0,
      55,     0,     0,    41,     0,     0,    42,     0,     5,     6,
      56,    55,     0,    43,    41,     0,     0,    42,   175,     0,
      44,    80,     0,     0,    43,     0,     0,     5,     6,    85,
      55,    44,     0,    41,     0,     0,    42,     0,     5,     6,
      56,    55,     0,    43,    41,     0,     0,    42,    85,     0,
      44,    56,     0,     0,    43,     0,     5,     6,     0,    55,
       0,    44,    41,     0,     0,    42,     0,     5,     6,   263,
       0,     0,    43,    41,     0,     0,    42,     0,     0,    44,
     192,     0,     0,    43,     0,     0,     0,     0,     0,     0,
      44
};

static const yytype_int16 yycheck[] =
{
      24,     1,     1,    27,    11,   153,    30,    78,    11,     9,
       9,     9,     7,    79,   106,   106,     1,    88,    89,     1,
      88,    18,    88,   167,    48,    25,    25,    85,    18,    26,
     103,    31,    31,    18,    39,    18,    18,   181,     5,     6,
     184,    14,    37,    93,    11,    18,   104,    14,    45,    34,
      37,    75,    59,    48,    21,    11,    36,    55,    45,    54,
      16,    28,    45,    30,   114,    45,    22,    62,    92,     5,
       6,    19,    21,    14,    98,    42,    11,    18,    14,    86,
      78,    16,   155,    86,    19,    21,    14,    22,   112,    45,
      39,   182,    28,    93,    93,   166,   188,   165,    36,   165,
     173,    36,    21,    19,    44,   129,    21,   105,   179,   133,
      45,    18,   110,    32,   114,   114,   260,   175,   176,    35,
      39,   194,   120,   147,    39,    32,   270,   151,    11,    18,
      33,   204,   156,    16,    37,   130,    19,    26,   286,    22,
      43,   199,    19,     5,     6,   203,     8,    18,    19,    11,
      21,    19,    14,   177,    14,   153,    18,    16,    35,    21,
      19,    32,    45,    22,    26,    18,    28,    35,    33,   167,
     177,   174,    37,    26,    45,    31,    17,   201,    43,    32,
      42,    22,    32,   181,     0,     1,   184,     3,   261,     5,
       6,     7,   265,   266,    10,    18,    12,    13,   271,   272,
       3,     4,     5,     6,   277,    37,    38,    19,    11,   267,
     268,    14,     1,    29,   287,   239,   214,    37,    21,     3,
       9,     5,     6,    15,   248,    28,    18,    11,   223,   224,
      14,    22,   227,    35,    53,    54,    19,    21,    17,   263,
     264,   314,    61,    62,    28,   240,   241,   320,   321,   322,
      37,    38,    14,    15,     5,     6,    18,    14,    40,    41,
      11,    18,   260,    14,    45,    14,   290,   291,   292,    18,
      21,    14,   270,    32,   332,    18,     3,    28,     5,     6,
     353,     5,     6,   281,    36,   343,    36,    11,   286,    28,
      14,    38,   312,   313,    18,    42,   294,    21,   356,    36,
     300,   300,    35,   301,    28,    19,    19,    35,    19,    33,
       1,    31,     3,    31,     5,     6,     7,     8,     9,    31,
      11,    12,    32,    14,    18,    22,    32,    18,    19,    31,
      21,   355,    23,    24,    25,    26,    27,    28,    37,    30,
      38,    37,     1,    34,     3,    16,     5,     6,     7,     8,
       9,    42,    11,    12,    14,    14,    43,    31,    22,    18,
      19,    17,    21,    18,    23,    24,    25,    26,    27,    28,
      33,    30,    20,    43,     1,    34,     3,    18,     5,     6,
       7,     8,     9,    42,    11,    12,    33,    14,    33,     1,
       1,    18,    19,     1,    21,   300,    23,    24,    25,    26,
      27,    28,   294,    30,     5,     6,   214,   301,    59,    66,
      11,    31,    79,    14,   106,    42,   147,    18,   147,   277,
      21,   126,     5,     6,     5,     6,   281,    28,    11,   303,
      11,    14,    33,    14,    41,    18,    30,    18,    21,    -1,
      21,    -1,    23,    24,    25,    28,    -1,    28,    -1,    30,
      33,     5,     6,    -1,    -1,    -1,    -1,    11,    -1,    -1,
      14,    -1,    -1,    -1,    18,    -1,    -1,    21,    -1,    23,
      24,    25,    -1,    -1,    28,     3,    30,     5,     6,    -1,
       8,    -1,    -1,    11,    -1,    -1,    14,    -1,     5,     6,
      18,     8,    -1,    21,    11,    -1,    -1,    14,    26,    -1,
      28,    18,    -1,    -1,    21,    -1,    -1,     5,     6,    26,
       8,    28,    -1,    11,    -1,    -1,    14,    -1,     5,     6,
      18,     8,    -1,    21,    11,    -1,    -1,    14,    26,    -1,
      28,    18,    -1,    -1,    21,    -1,     5,     6,    -1,     8,
      -1,    28,    11,    -1,    -1,    14,    -1,     5,     6,    18,
      -1,    -1,    21,    11,    -1,    -1,    14,    -1,    -1,    28,
      18,    -1,    -1,    21,    -1,    -1,    -1,    -1,    -1,    -1,
      28
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    47,     0,     1,     3,     5,     6,     7,    10,    12,
      13,    29,    48,    49,    50,    51,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    69,    70,
      71,    72,    73,    77,    78,   127,   128,   129,   130,    18,
     115,    11,    14,    21,    28,    52,    61,    64,   116,   117,
     118,   119,   120,   127,   128,     8,    18,    79,    99,   114,
     116,   127,   128,    19,     1,    18,    80,    81,    82,    83,
      86,    19,    35,    18,    45,   124,   125,     1,     8,     9,
      18,    19,    23,    24,    25,    26,    27,    28,    30,    42,
      51,    54,    61,    87,    88,    89,    90,    91,    92,    93,
      95,    96,    97,    98,    99,   110,   113,   114,    19,    35,
      36,   125,   124,   125,    87,    19,    35,    14,   115,    39,
      74,   119,    21,    74,    14,    21,    74,    14,    18,   123,
      19,    21,    32,   115,   125,    44,    31,   112,    14,    14,
     115,   120,   115,   120,   116,    11,    16,    19,    22,   100,
     101,   102,   125,    32,   103,    79,    34,    81,    19,    37,
      22,    35,    19,    17,   125,    30,    42,   110,    91,    97,
      36,    36,    36,   103,     3,    26,    99,   114,    32,    42,
      97,   110,   113,    28,   110,   125,    34,    88,    37,   125,
     100,   103,    18,    33,   104,   105,   106,   107,   116,    26,
      32,    92,    93,    94,    98,    35,    19,    18,    23,    24,
      25,    30,    65,    66,    67,    68,   116,   126,    19,   125,
      34,    35,    19,     3,     4,    75,    76,   116,    14,    18,
      14,    15,    18,   121,   122,    14,    18,   121,   125,   115,
      31,    31,   125,    31,    16,    19,    22,   101,   102,   125,
     125,    33,   105,   100,   125,    15,    18,    84,    85,    97,
     110,   104,   100,    18,   114,   103,   103,    26,    99,   125,
     110,   104,   104,    18,    93,    36,   100,   108,   109,   111,
      33,    37,    43,    22,    18,    32,    32,   103,    31,   125,
      17,    22,   103,   100,    37,    18,    68,   116,   115,   115,
      38,    37,   115,    37,    38,    38,   125,   115,   115,    16,
     125,    33,    40,    41,   104,   100,   125,   125,   100,   100,
     103,   103,   104,   100,   100,   100,   109,    14,   106,    43,
      17,    31,    33,   100,    18,   125,   125,   125,    66,    63,
      76,    22,   122,    33,    85,    85,   100,   100,   100,   100,
      20,    43,    18,   103,    33,   103,    33,   100,   125,   103
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
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
    break;

  case 11:
#line 135 "mta_parse.y"
    { }
    break;

  case 12:
#line 138 "mta_parse.y"
    {
  	  if((yyvsp[(1) - (1)].typ) != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("typedef", sp, (yyval.typ)); } }
    break;

  case 13:
#line 146 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 14:
#line 147 "mta_parse.y"
    {
	    if((yyvsp[(1) - (2)].typ) != NULL)  SETDESC((yyvsp[(1) - (2)].typ),(yyvsp[(2) - (2)].chr)); }
    break;

  case 15:
#line 151 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 16:
#line 152 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(2) - (4)].typ); (yyvsp[(2) - (4)].typ)->name = (yyvsp[(3) - (4)].chr); mta->type_stack.Pop(); }
    break;

  case 17:
#line 156 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); (yyvsp[(2) - (3)].typ)->AddParent((yyvsp[(1) - (3)].typ)); (yyvsp[(2) - (3)].typ)->type = (yyvsp[(1) - (3)].typ)->type;
	    mta->type_stack.Pop(); }
    break;

  case 18:
#line 159 "mta_parse.y"
    { /* annoying place for a comment, but.. */
            (yyval.typ) = (yyvsp[(3) - (4)].typ); (yyvsp[(3) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(3) - (4)].typ)->type = (yyvsp[(1) - (4)].typ)->type;
	    mta->type_stack.Pop(); }
    break;

  case 19:
#line 164 "mta_parse.y"
    {
	  if((yyvsp[(1) - (2)].typ)->parents.size < 2) {
	    cerr << "E!!: Error in predeclared type: " << (yyvsp[(1) - (2)].typ)->name << " second parent not found!"
		 << endl;
	    (yyval.typ) = (yyvsp[(1) - (2)].typ);
	  }
	  else {
	    TypeDef* td = (yyvsp[(1) - (2)].typ)->parents[1]; mta->type_stack.Pop();
	    TypeSpace* sp = (yyvsp[(1) - (2)].typ)->owner;
	    sp->RemoveEl((yyvsp[(1) - (2)].typ)); /* get rid of new one, cuz it is bogus */
	    /* not on list that it would be placed on now.. */
	    if((td->owner != mta->spc) && (mta->spc->FindName(td->name) == NULL)) {
	      if(mta->verbose >= 3)
		cerr << "M!!: transfered: " << td->name << " from: " << td->owner->name
		     << " to: " << mta->spc->name << "\n";
	      mta->spc->Transfer(td); /* now check for parent which is a combo of basic types */
	      if((td->parents.size == 1) && (td->parents[0]->owner != mta->spc) &&
		 (td->parents[0]->parents.size == 2)) {
		/* has one parent that is a combo-type which might be basic */
		TypeDef* par = td->parents[0];
		if((mta->spc_builtin.FindName(par->parents[0]->name) != NULL) &&
		   (mta->spc_builtin.FindName(par->parents[1]->name) != NULL)) {
		  if(mta->verbose >= 3)
		    cerr << "M!!: transfered: " << par->name << " from: " << par->owner->name
			 << " to: " << mta->spc->name << "\n";
		  TypeDef* already_there = mta->spc->FindName(par->name);
		  if(already_there == NULL)
		    mta->spc->Transfer(par); /* move parent to this list too */
		  else
		    td->parents.ReplaceLinkIdx(0, already_there);
		}
	      }
	    }
	    (yyval.typ) = td; } }
    break;

  case 20:
#line 198 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (7)].typ); (yyval.typ)->SetType(TypeDef::FUN_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 21:
#line 201 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (4)].typ); (yyval.typ)->SetType(TypeDef::METH_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 22:
#line 206 "mta_parse.y"
    {
            TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
            (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("enum", sp, (yyval.typ)); (yyval.typ)->source_end = mta->line-1; } }
    break;

  case 25:
#line 216 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); }
    break;

  case 26:
#line 217 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 27:
#line 220 "mta_parse.y"
    {
  	    (yyval.typ) = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->SetActualType(TypeDef::ENUM); mta->cur_enum = (yyvsp[(2) - (2)].typ);
	    mta->SetSource((yyval.typ), false); mta->type_stack.Pop(); }
    break;

  case 28:
#line 224 "mta_parse.y"
    {
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->cur_enum = (yyval.typ);
	    mta->SetSource((yyval.typ), false); (yyval.typ)->SetActualType(TypeDef::ENUM); }
    break;

  case 29:
#line 231 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("class", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 30:
#line 239 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 31:
#line 240 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 32:
#line 244 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("class", sp, (yyval.typ)); (yyval.typ)->FixClassTypes();
              (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 35:
#line 258 "mta_parse.y"
    {
	    if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	    else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 36:
#line 264 "mta_parse.y"
    {
	    (yyvsp[(1) - (2)].typ)->tokens.keep = true; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (2)].typ), true); }
    break;

  case 37:
#line 267 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
            mta->SetSource((yyvsp[(1) - (3)].typ), true); }
    break;

  case 38:
#line 270 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (3)].typ), true); 
	  }
    break;

  case 39:
#line 277 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 40:
#line 278 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 41:
#line 281 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->SetActualType(TypeDef::CLASS);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 42:
#line 286 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 43:
#line 290 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->SetActualType(TypeDef::CLASS); 
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 44:
#line 297 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->SetActualType(TypeDef::STRUCT);
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 45:
#line 304 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->SetActualType(TypeDef::UNION);
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 46:
#line 314 "mta_parse.y"
    {
	    mta->cur_class->AddParent((yyvsp[(1) - (1)].typ)); }
    break;

  case 47:
#line 316 "mta_parse.y"
    {
	    mta->cur_class->AddParent((yyvsp[(3) - (3)].typ));
	    if(!mta->cur_class->HasOption("MULT_INHERIT"))
	      mta->cur_class->opts.Add("MULT_INHERIT"); }
    break;

  case 49:
#line 324 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 50:
#line 325 "mta_parse.y"
    { (yyval.typ) = &TA_class; }
    break;

  case 51:
#line 326 "mta_parse.y"
    { (yyval.typ) = &TA_class; }
    break;

  case 56:
#line 338 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ));
	      (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 59:
#line 352 "mta_parse.y"
    {
          if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	  else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 60:
#line 358 "mta_parse.y"
    {
            mta->state = MTA::Parse_inclass; (yyvsp[(1) - (2)].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 61:
#line 361 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 62:
#line 363 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 63:
#line 368 "mta_parse.y"
    {
	    (yyvsp[(5) - (5)].typ)->templ_pars.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_pars.Duplicate(mta->cur_templ_pars);
	    (yyvsp[(5) - (5)].typ)->SetActualType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(5) - (5)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 64:
#line 375 "mta_parse.y"
    { mta->cur_templ_pars.Reset(); }
    break;

  case 66:
#line 380 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 67:
#line 384 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 68:
#line 385 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 69:
#line 386 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 70:
#line 387 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (3)].typ)); (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 71:
#line 390 "mta_parse.y"
    {
            if(mta->spc == &(mta->spc_target)) /* only add reg_funs in target space */
	       TA_taRegFun.methods.AddUniqNameNew((yyvsp[(1) - (1)].meth));
	    mta->meth_stack.Pop(); }
    break;

  case 72:
#line 396 "mta_parse.y"
    {
            (yyvsp[(3) - (4)].meth)->type = (yyvsp[(2) - (4)].typ); SETDESC((yyvsp[(3) - (4)].meth),(yyvsp[(4) - (4)].chr)); (yyval.meth) = (yyvsp[(3) - (4)].meth); }
    break;

  case 73:
#line 398 "mta_parse.y"
    {
            (yyvsp[(2) - (3)].meth)->type = &TA_int; SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); (yyval.meth) = (yyvsp[(2) - (3)].meth); }
    break;

  case 74:
#line 402 "mta_parse.y"
    {
            (yyvsp[(1) - (2)].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = true; }
    break;

  case 78:
#line 412 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 81:
#line 417 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 82:
#line 422 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 83:
#line 425 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm)); if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
	    mta->enum_stack.Pop(); }
    break;

  case 85:
#line 432 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 86:
#line 437 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 87:
#line 445 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 89:
#line 450 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 90:
#line 453 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 91:
#line 454 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 92:
#line 457 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].memb) != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->HasOption("IGNORE"))
		 && !((yyvsp[(1) - (1)].memb)->type->DerivesFrom(TA_const))) {
		mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
		if(mta->verbose >= 3)
		  cerr << "M!!: member: " << (yyvsp[(1) - (1)].memb)->name << " added to: "
		       << mta->cur_class->name << "\n"; } }
	    mta->memb_stack.Pop(); (yyval.typ) = NULL; }
    break;

  case 93:
#line 466 "mta_parse.y"
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

  case 94:
#line 479 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 95:
#line 482 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 96:
#line 485 "mta_parse.y"
    { /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew((yyvsp[(1) - (2)].typ));
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 97:
#line 489 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 99:
#line 494 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 100:
#line 495 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 101:
#line 496 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 102:
#line 497 "mta_parse.y"
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

  case 104:
#line 518 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 105:
#line 524 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 106:
#line 525 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 107:
#line 526 "mta_parse.y"
    { }
    break;

  case 108:
#line 527 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
            int spsz = sp->size;
            TypeDef* nty = (yyvsp[(1) - (4)].typ)->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    (yyvsp[(2) - (4)].memb)->type = nty; }
    break;

  case 109:
#line 534 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 110:
#line 538 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 111:
#line 542 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(3) - (3)].memb)->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 112:
#line 548 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 113:
#line 554 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 115:
#line 560 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 116:
#line 561 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 117:
#line 562 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 118:
#line 563 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 119:
#line 564 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 120:
#line 565 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 121:
#line 566 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 122:
#line 567 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 123:
#line 568 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 124:
#line 569 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 125:
#line 570 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 126:
#line 571 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 127:
#line 572 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(5) - (5)].chr);
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew((yyvsp[(3) - (5)].meth)); (yyvsp[(3) - (5)].meth)->type = (yyvsp[(2) - (5)].typ);
	      mta->meth_stack.Pop();  (yyvsp[(3) - (5)].meth)->fun_argc = (yyvsp[(4) - (5)].rval); (yyvsp[(3) - (5)].meth)->arg_types.size = (yyvsp[(4) - (5)].rval);
	      (yyvsp[(3) - (5)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(3) - (5)].meth),(yyvsp[(5) - (5)].chr)); }
	    else { (yyval.meth) = NULL; mta->meth_stack.Pop(); } }
    break;

  case 128:
#line 580 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(4) - (4)].chr);
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew((yyvsp[(2) - (4)].meth)); (yyvsp[(2) - (4)].meth)->type = &TA_int;
	      mta->meth_stack.Pop();  (yyvsp[(2) - (4)].meth)->fun_argc = (yyvsp[(3) - (4)].rval); (yyvsp[(2) - (4)].meth)->arg_types.size = (yyvsp[(3) - (4)].rval);
	      (yyvsp[(2) - (4)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(2) - (4)].meth),(yyvsp[(4) - (4)].chr)); }
	    else { (yyval.meth) = 0; mta->meth_stack.Pop(); } }
    break;

  case 130:
#line 592 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 131:
#line 593 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 132:
#line 595 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 133:
#line 600 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 134:
#line 601 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
    break;

  case 135:
#line 602 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 136:
#line 603 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 137:
#line 604 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 138:
#line 607 "mta_parse.y"
    {
             (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 139:
#line 614 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 140:
#line 618 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 141:
#line 619 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
    break;

  case 142:
#line 620 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 143:
#line 621 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 144:
#line 622 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 145:
#line 623 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 146:
#line 627 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 147:
#line 628 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 148:
#line 629 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
    break;

  case 149:
#line 633 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 150:
#line 634 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 151:
#line 635 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 152:
#line 638 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 153:
#line 639 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 154:
#line 642 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 155:
#line 643 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 156:
#line 646 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 157:
#line 647 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 158:
#line 648 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 159:
#line 651 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 160:
#line 653 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      String ad = (yyvsp[(2) - (2)].chr);
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 161:
#line 664 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 162:
#line 668 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 163:
#line 672 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 164:
#line 676 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 165:
#line 680 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 169:
#line 695 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 170:
#line 699 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 171:
#line 702 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 172:
#line 703 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 173:
#line 706 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 175:
#line 710 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 176:
#line 711 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 177:
#line 714 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
                                  mta->SetSource((yyval.typ), false); }
    break;

  case 179:
#line 719 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ)->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, (yyval.typ)); }
	  }
    break;

  case 181:
#line 729 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ);
 	    for(int i=0; i<(yyvsp[(2) - (2)].rval); i++) {
              (yyval.typ) = (yyval.typ)->GetPtrType_impl(*sp);
              if(sp->size != spcz) { mta->TypeAdded("ptr", sp, (yyval.typ)); }
            }
	  }
    break;

  case 183:
#line 741 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = GetConstType_impl(*sp);
	    if(sp->size != spcz) { mta->TypeAdded("const", sp, (yyval.typ)); }
	  }
    break;

  case 185:
#line 750 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 186:
#line 751 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 187:
#line 752 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 188:
#line 753 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 189:
#line 754 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 190:
#line 758 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 191:
#line 762 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 192:
#line 763 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 193:
#line 764 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 195:
#line 766 "mta_parse.y"
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

  case 196:
#line 783 "mta_parse.y"
    { /* this template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
	      yyerror("Template syntax error"); YYERROR; }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 198:
#line 790 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (2)].typ)->name + "_" + (yyvsp[(2) - (2)].typ)->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("combo", sp, (yyval.typ));
	      nty->size = (yyvsp[(2) - (2)].typ)->size; nty->AddParent((yyvsp[(1) - (2)].typ)); nty->AddParent((yyvsp[(2) - (2)].typ)); }
	    else { mta->TypeNotAdded("combo", sp, (yyval.typ), nty); delete nty; }
	  }
    break;

  case 200:
#line 803 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 201:
#line 808 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 202:
#line 809 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 203:
#line 810 "mta_parse.y"
    { (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 205:
#line 815 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 212:
#line 831 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 213:
#line 835 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 214:
#line 839 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 215:
#line 843 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 2966 "y.tab.c"
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


#line 846 "mta_parse.y"


	/* end of grammar */

void yyerror(const char *s) { 	/* called for yacc syntax error */
  int i;
  if((mta->verbose < 1) && (mta->spc != &(mta->spc_target)))
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

