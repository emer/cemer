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
#define YYLAST   602

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  46
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  84
/* YYNRULES -- Number of rules.  */
#define YYNRULES  212
/* YYNRULES -- Number of states.  */
#define YYNSTATES  355

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
     128,   132,   135,   138,   140,   142,   144,   148,   150,   153,
     155,   158,   160,   163,   165,   167,   169,   172,   176,   180,
     183,   187,   191,   197,   199,   201,   205,   208,   211,   214,
     218,   220,   225,   229,   232,   234,   237,   239,   242,   244,
     247,   249,   251,   255,   257,   261,   265,   267,   269,   271,
     273,   276,   278,   280,   282,   284,   287,   289,   291,   294,
     297,   300,   302,   304,   307,   311,   316,   319,   324,   329,
     331,   335,   337,   342,   344,   348,   353,   358,   363,   368,
     374,   380,   386,   391,   395,   400,   405,   411,   416,   418,
     421,   424,   428,   432,   435,   440,   447,   451,   454,   456,
     458,   460,   463,   466,   469,   473,   475,   478,   482,   484,
     486,   489,   492,   496,   498,   501,   503,   507,   512,   514,
     517,   519,   522,   526,   533,   535,   537,   540,   544,   547,
     549,   551,   554,   556,   558,   560,   563,   565,   567,   570,
     572,   575,   577,   580,   582,   585,   588,   592,   596,   600,
     604,   607,   609,   614,   619,   621,   624,   626,   630,   632,
     634,   636,   638,   640,   642,   645,   647,   649,   651,   653,
     655,   657,   659
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
      65,    -1,   128,   115,    -1,   128,    14,    -1,   128,    -1,
     127,    -1,    66,    -1,    65,    37,    66,    -1,   116,    -1,
      67,   116,    -1,    18,    -1,    67,    18,    -1,    68,    -1,
      67,    68,    -1,   126,    -1,    30,    -1,    70,    -1,    71,
     125,    -1,    71,   124,   125,    -1,    72,    87,    34,    -1,
      73,    35,    -1,    73,    19,    35,    -1,    73,    35,    19,
      -1,   129,    74,    75,    38,    63,    -1,    39,    -1,    76,
      -1,    76,    37,    76,    -1,     3,   115,    -1,     4,   115,
      -1,   116,   115,    -1,   116,   115,    22,    -1,    78,    -1,
      29,   114,    79,   100,    -1,    29,    79,   100,    -1,    99,
     103,    -1,    81,    -1,    80,    81,    -1,    82,    -1,    82,
      19,    -1,    83,    -1,    83,    37,    -1,     1,    -1,    86,
      -1,    86,    22,    84,    -1,    85,    -1,    84,    40,    85,
      -1,    84,    41,    85,    -1,    18,    -1,    15,    -1,    18,
      -1,    88,    -1,    87,    88,    -1,    89,    -1,    95,    -1,
      54,    -1,    51,    -1,    61,   125,    -1,     1,    -1,    90,
      -1,    23,    36,    -1,    24,    36,    -1,    25,    36,    -1,
      19,    -1,    91,    -1,     9,    91,    -1,   113,    92,   125,
      -1,   113,    93,    22,   125,    -1,    92,   125,    -1,   113,
      93,    17,   125,    -1,   113,    94,   103,   125,    -1,    93,
      -1,    92,    37,    93,    -1,    18,    -1,    32,    31,    18,
      33,    -1,    96,    -1,   110,   104,   100,    -1,     8,   110,
     104,   100,    -1,    30,   110,   104,   100,    -1,   110,   104,
     108,   100,    -1,    42,   110,   104,   100,    -1,     8,    42,
     110,   104,   100,    -1,    30,    42,   110,   104,   100,    -1,
      27,   114,    26,   103,   100,    -1,    27,    26,   103,   100,
      -1,    27,   114,   125,    -1,    27,     3,   114,   125,    -1,
      27,     3,    18,   125,    -1,    27,   114,    99,   103,   100,
      -1,    27,    99,   103,   100,    -1,    97,    -1,     9,    97,
      -1,    30,    97,    -1,     8,    30,    97,    -1,   113,    98,
     100,    -1,    98,   100,    -1,   113,    26,   103,   100,    -1,
     113,    26,    32,    33,   103,   100,    -1,    26,   103,   100,
      -1,    99,   103,    -1,    18,    -1,   125,    -1,   101,    -1,
     102,   125,    -1,    19,   125,    -1,    19,   101,    -1,    19,
     102,   125,    -1,    16,    -1,    11,    16,    -1,    11,    19,
      16,    -1,    22,    -1,    11,    -1,    11,    22,    -1,    32,
      33,    -1,    32,   105,    33,    -1,    33,    -1,   105,    33,
      -1,   106,    -1,   105,    37,   106,    -1,   105,    43,    43,
      43,    -1,   107,    -1,   107,    22,    -1,   116,    -1,   116,
      18,    -1,   116,    18,    17,    -1,   116,    32,    31,    18,
      33,   103,    -1,    18,    -1,   109,    -1,   108,   109,    -1,
     111,    14,    20,    -1,    28,    32,    -1,    36,    -1,    31,
      -1,   112,    31,    -1,   114,    -1,   116,    -1,     8,    -1,
       8,   116,    -1,    18,    -1,   117,    -1,   117,    44,    -1,
     118,    -1,   118,   112,    -1,   119,    -1,    11,   119,    -1,
     120,    -1,   127,   120,    -1,   127,   115,    -1,    14,    21,
      18,    -1,    28,    21,    18,    -1,    14,    21,    14,    -1,
      28,    21,    14,    -1,    21,    14,    -1,    28,    -1,    14,
      74,   121,    38,    -1,    28,    74,   121,    38,    -1,    14,
      -1,   120,    14,    -1,   122,    -1,   121,    37,   122,    -1,
      14,    -1,    18,    -1,    15,    -1,    18,    -1,    14,    -1,
      18,    -1,    18,    17,    -1,    45,    -1,    23,    -1,    24,
      -1,    25,    -1,     5,    -1,     6,    -1,     3,    -1,    10,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   112,   112,   113,   116,   118,   121,   124,   126,   128,
     130,   135,   138,   146,   147,   151,   152,   156,   160,   166,
     202,   205,   210,   216,   219,   220,   221,   224,   229,   237,
     245,   246,   250,   258,   259,   263,   269,   272,   275,   282,
     283,   286,   291,   295,   301,   310,   312,   319,   320,   321,
     322,   325,   326,   329,   330,   334,   343,   344,   348,   354,
     357,   359,   364,   372,   376,   377,   381,   382,   383,   384,
     387,   393,   395,   399,   404,   405,   408,   409,   412,   413,
     414,   419,   422,   428,   429,   434,   442,   443,   447,   450,
     451,   454,   463,   476,   479,   482,   486,   490,   491,   492,
     493,   494,   514,   515,   521,   522,   523,   524,   534,   538,
     542,   548,   554,   559,   560,   561,   562,   563,   564,   565,
     566,   567,   568,   569,   570,   571,   572,   580,   591,   592,
     593,   595,   600,   601,   602,   603,   604,   607,   614,   618,
     619,   620,   621,   622,   623,   627,   628,   629,   633,   634,
     635,   638,   639,   642,   643,   646,   647,   648,   651,   653,
     664,   668,   672,   676,   680,   687,   688,   691,   695,   699,
     702,   703,   706,   709,   710,   711,   714,   719,   720,   731,
     732,   742,   743,   754,   755,   756,   757,   761,   765,   766,
     767,   768,   769,   786,   792,   793,   805,   806,   811,   812,
     813,   817,   818,   821,   822,   825,   828,   829,   830,   834,
     835,   839,   843
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
  "varname", "term", "access", "structstruct", "classkeyword",
  "templatekeyword", 0
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
      63,    64,    64,    64,    64,    65,    65,    66,    66,    66,
      66,    67,    67,    68,    68,    69,    70,    70,    71,    72,
      72,    72,    73,    74,    75,    75,    76,    76,    76,    76,
      77,    78,    78,    79,    80,    80,    81,    81,    82,    82,
      82,    83,    83,    84,    84,    84,    85,    85,    86,    87,
      87,    88,    88,    88,    88,    88,    88,    89,    89,    89,
      89,    89,    90,    90,    91,    91,    91,    91,    91,    92,
      92,    93,    94,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    96,    96,
      96,    96,    97,    97,    97,    97,    97,    98,    99,   100,
     100,   100,   100,   100,   100,   101,   101,   101,   102,   102,
     102,   103,   103,   104,   104,   105,   105,   105,   106,   106,
     107,   107,   107,   107,   107,   108,   108,   109,   110,   111,
     112,   112,   113,   114,   114,   114,   115,   116,   116,   117,
     117,   118,   118,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   120,   120,   121,   121,   122,   122,
     122,   123,   123,   124,   124,   125,   126,   126,   126,   127,
     127,   128,   129
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     1,     1,     2,     2,     4,     3,     4,     2,
       7,     4,     1,     4,     2,     3,     3,     2,     1,     1,
       2,     3,     1,     2,     3,     3,     2,     3,     3,     1,
       3,     2,     2,     1,     1,     1,     3,     1,     2,     1,
       2,     1,     2,     1,     1,     1,     2,     3,     3,     2,
       3,     3,     5,     1,     1,     3,     2,     2,     2,     3,
       1,     4,     3,     2,     1,     2,     1,     2,     1,     2,
       1,     1,     3,     1,     3,     3,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     2,     1,     1,     2,     2,
       2,     1,     1,     2,     3,     4,     2,     4,     4,     1,
       3,     1,     4,     1,     3,     4,     4,     4,     4,     5,
       5,     5,     4,     3,     4,     4,     5,     4,     1,     2,
       2,     3,     3,     2,     4,     6,     3,     2,     1,     1,
       1,     2,     2,     2,     3,     1,     2,     3,     1,     1,
       2,     2,     3,     1,     2,     1,     3,     4,     1,     2,
       1,     2,     3,     6,     1,     1,     2,     3,     2,     1,
       1,     2,     1,     1,     1,     2,     1,     1,     2,     1,
       2,     1,     2,     1,     2,     2,     3,     3,     3,     3,
       2,     1,     4,     4,     1,     2,     1,     3,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    10,   211,   209,   210,    28,   212,     0,
      11,     0,     9,     3,    12,    13,     4,    22,     0,     0,
       5,    29,     6,    32,     0,     0,     0,    39,     7,    55,
       0,     0,     0,     8,    70,    44,    43,     0,   176,    27,
       0,   194,     0,   191,    15,     0,    39,     0,   177,   179,
     181,   183,    44,   174,   138,     0,     0,     0,   173,     0,
      14,    80,    88,     0,    74,    76,    78,    81,     0,    24,
     203,   205,     0,    33,    96,   174,     0,   111,   101,     0,
       0,     0,     0,     0,   191,     0,     0,    94,    93,     0,
       0,    89,    91,    97,   102,     0,   109,    92,   113,   128,
       0,     0,     0,     0,   172,     0,    36,     0,    30,     0,
      56,     0,     0,    59,    42,    41,    63,     0,   182,     0,
       0,   190,     0,     0,   202,   201,     0,     0,     0,     0,
       0,    19,   178,   170,   180,   195,   194,   185,   184,   175,
     149,   145,     0,   148,    72,   140,     0,   139,     0,    73,
       0,     0,    75,    77,    79,     0,    25,    26,   204,    34,
       0,     0,     0,   103,   129,    98,    99,   100,     0,     0,
       0,     0,     0,   168,     0,   130,     0,     0,     0,     0,
      95,    35,    90,     0,   106,   133,   137,   164,   153,     0,
       0,   155,   158,   160,     0,     0,     0,   109,     0,     0,
      37,    38,    49,   206,   207,   208,    54,    40,    45,     0,
      51,    47,    53,    31,    57,    58,    60,    61,     0,     0,
       0,    64,     0,   188,   186,   198,   200,   199,     0,   196,
     189,   187,     0,    16,     0,     0,     0,    17,   171,   146,
       0,   150,   143,     0,   142,   141,   151,     0,    71,    23,
      87,    86,    82,    83,   131,     0,     0,   136,     0,     0,
       0,     0,     0,     0,   123,     0,     0,     0,   111,   110,
     169,   114,     0,   165,     0,   154,     0,     0,   159,   161,
       0,     0,     0,     0,   104,     0,     0,     0,   132,     0,
      50,    52,    48,    66,    67,     0,     0,    68,     0,   192,
     193,    18,    21,     0,   147,   144,   152,     0,     0,     0,
     115,   125,   124,   122,   127,     0,     0,     0,   116,   118,
     117,   166,     0,   156,     0,   162,     0,   151,   134,     0,
     107,   105,   108,    46,    62,    65,    69,   197,     0,    84,
      85,   119,   121,   126,   120,   167,   157,     0,     0,   112,
       0,     0,   135,    20,   163
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    12,    13,    14,    87,    44,    16,    88,    18,
      19,    20,    21,    22,    23,    89,    25,    26,    46,   207,
     208,   209,   210,    28,    29,    30,    31,    32,   123,   220,
     221,    33,    34,    55,    63,    64,    65,    66,   252,   253,
      67,    90,    91,    92,    93,    94,    95,    96,   198,    97,
      98,    99,   100,   101,   144,   145,   146,   149,   189,   190,
     191,   192,   272,   273,   102,   274,   134,   103,   104,   137,
      58,    48,    49,    50,    51,   228,   229,   126,    72,   147,
     212,    59,    36,    37
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -158
static const yytype_int16 yypact[] =
{
    -158,   192,  -158,  -158,  -158,  -158,  -158,    14,  -158,   486,
    -158,   527,  -158,  -158,  -158,     1,  -158,  -158,    85,    37,
    -158,  -158,  -158,  -158,    22,   368,    65,    81,  -158,  -158,
      22,   368,    97,  -158,  -158,  -158,    61,    56,  -158,  -158,
     203,    60,    39,    62,  -158,   144,    69,   124,    63,   107,
    -158,   126,   157,   574,  -158,    19,   120,   156,  -158,   157,
    -158,  -158,  -158,    28,  -158,   160,   179,   165,   159,   213,
     219,  -158,   193,  -158,  -158,   125,   497,   224,  -158,   222,
     223,   229,   120,   467,    12,    52,   217,  -158,  -158,   193,
     304,  -158,  -158,  -158,  -158,    92,  -158,  -158,  -158,  -158,
      19,   120,   268,   115,  -158,   225,   247,   414,   249,   193,
    -158,   336,   234,   251,  -158,  -158,  -158,   209,  -158,   168,
       3,  -158,   189,     3,  -158,  -158,   193,    14,   240,   244,
     193,  -158,  -158,  -158,   245,  -158,  -158,  -158,   126,  -158,
     228,  -158,    99,  -158,  -158,  -158,   193,  -158,   394,  -158,
      19,   193,  -158,  -158,  -158,   145,  -158,  -158,  -158,  -158,
     516,   217,   268,  -158,  -158,  -158,  -158,  -158,    19,   545,
     120,   120,    43,  -158,   217,  -158,   268,   131,   248,   268,
    -158,  -158,  -158,   263,  -158,  -158,  -158,  -158,  -158,    23,
     133,  -158,   262,    96,   253,   256,    92,    30,   120,    19,
    -158,  -158,  -158,  -158,  -158,  -158,  -158,   255,  -158,   441,
    -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,    14,    14,
     250,   257,    14,  -158,  -158,  -158,  -158,  -158,   163,  -158,
    -158,  -158,   190,  -158,   193,    14,    14,  -158,  -158,  -158,
     274,  -158,  -158,   193,  -158,  -158,  -158,   140,  -158,  -158,
    -158,  -158,   211,  -158,  -158,   268,    19,  -158,   193,   193,
      19,    19,   120,   120,  -158,   268,    19,    19,  -158,  -158,
    -158,  -158,    23,  -158,   279,  -158,   556,   254,  -158,   282,
     269,   412,    19,   284,  -158,   193,   193,   193,  -158,   414,
    -158,  -158,  -158,  -158,  -158,   243,   209,   281,     3,  -158,
    -158,  -158,  -158,   271,  -158,  -158,  -158,   145,   145,    19,
    -158,  -158,  -158,  -158,  -158,    19,    19,    19,  -158,  -158,
    -158,  -158,   286,  -158,   265,  -158,   296,   120,  -158,   287,
    -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,   120,  -158,
    -158,  -158,  -158,  -158,  -158,  -158,  -158,   288,    19,  -158,
     193,   120,  -158,  -158,  -158
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -158,  -158,  -158,  -158,  -158,   316,  -158,  -158,   318,  -158,
    -158,  -158,  -158,  -158,  -158,   150,  -158,    29,   332,  -158,
      46,  -158,   127,  -158,  -158,  -158,  -158,  -158,   181,  -158,
      44,  -158,  -158,   292,  -158,   289,  -158,  -158,  -158,   -53,
    -158,   320,   -62,  -158,  -158,   277,   264,   -90,  -158,  -158,
    -158,   -69,   -88,    -7,   -76,   214,   216,   -74,  -157,  -146,
      89,  -158,  -158,   100,   -49,  -158,  -158,   -73,    -1,     7,
       2,  -158,  -158,   328,   102,   258,    86,  -158,   353,   -24,
    -158,     0,  -158,  -158
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -139
static const yytype_int16 yytable[] =
{
      73,    35,   247,   108,    56,   256,   110,   164,   168,    52,
      57,    47,   177,   197,    39,   199,   175,   225,   226,   266,
      60,   227,   267,   131,   185,    52,   162,   186,   182,    61,
     140,    52,    38,   122,   140,   141,   176,   179,   142,   141,
      70,   143,   142,   115,   173,   143,    62,   285,   159,   182,
      56,   116,   286,   121,   130,   139,    68,     5,     6,   270,
      53,    54,   151,    40,    71,   180,    41,    71,    71,   262,
      54,   184,    69,    42,   248,   114,   171,   139,    82,    38,
      84,   119,   172,   122,   105,   214,    61,   177,    71,   199,
      52,   254,   257,   269,   174,   116,   260,   261,   309,   116,
     106,   116,   233,    62,   193,   107,   237,   132,   317,   211,
     140,    52,   255,   271,   279,   141,   112,   107,   244,   222,
     282,   143,   245,   288,   287,   265,    71,   249,   280,   183,
       5,     6,   113,    77,   234,   247,    40,    71,   133,    41,
     135,   194,    38,   127,    71,   128,    42,   195,   264,    54,
     193,    24,   148,    84,   138,   160,   129,   194,   124,    45,
     250,   138,   125,   251,   193,   263,   275,   161,   259,    71,
     276,   136,   284,   306,    54,    38,   277,   276,   193,   153,
     310,   193,   223,   277,   313,   314,   224,   155,   315,   316,
     318,   319,     2,     3,   156,     4,   320,     5,     6,     7,
     298,   299,     8,   230,     9,    10,   328,   231,     5,     6,
     301,   292,   218,   219,     5,     6,   154,    41,   117,   305,
      40,    11,   120,    41,    42,   293,   294,   298,   300,   297,
      42,    43,   157,   341,   311,   312,   158,    43,    71,   342,
     343,   344,   302,   303,   239,   178,     4,   240,     5,     6,
     241,   307,   308,   348,   339,   340,  -138,   193,   165,   166,
     200,   330,   331,   332,   350,   167,   201,   193,   213,   216,
     217,   235,   352,     5,     6,   236,   238,   354,   193,    40,
     173,   268,    41,   193,   278,   281,   187,   283,   295,    42,
     304,   211,   289,   322,   296,    35,    43,   324,   222,   325,
     326,   188,   329,   336,   338,    74,   345,     4,   346,     5,
       6,     7,    75,    76,   347,    40,     9,    15,    41,    17,
     349,   351,    77,    78,   334,    42,   353,    79,    80,    81,
      82,    83,    84,    27,    85,   333,   291,    74,   181,     4,
     335,     5,     6,     7,    75,    76,    86,    40,     9,   150,
      41,   111,   152,   163,    77,    78,   242,    42,   243,    79,
      80,    81,    82,    83,    84,   323,    85,   196,   118,    74,
     215,     4,   321,     5,     6,     7,    75,    76,    86,    40,
       9,   232,    41,   109,   337,     0,    77,    78,     0,    42,
       0,    79,    80,    81,    82,    83,    84,     0,    85,     5,
       6,     0,     0,     0,     0,    40,     0,     0,    41,     0,
      86,     0,   187,     0,     0,    42,     0,     5,     6,     5,
       6,     0,    43,    40,     0,    40,    41,   246,    41,     0,
     187,     0,   202,    42,     0,    42,     0,   203,   204,   205,
      43,     0,    43,     0,   206,   327,     5,     6,     0,     0,
       0,     0,    40,     0,     0,    41,     0,     0,     0,   290,
       0,     0,    42,     0,   203,   204,   205,     0,     0,    43,
     169,   206,     5,     6,     0,    53,     0,     0,    40,     0,
       0,    41,     0,     0,     0,    54,     0,     0,    42,     4,
       0,     5,     6,   170,     0,    43,     0,    40,     0,     0,
      41,     0,     5,     6,     0,    53,     0,    42,    40,     0,
       0,    41,     0,     0,    43,    77,     0,     0,    42,     0,
       0,     5,     6,    82,    53,    43,     0,    40,     0,     0,
      41,     0,     5,     6,    54,    53,     0,    42,    40,     0,
       0,    41,    82,     0,    43,    54,     0,     0,    42,     0,
       5,     6,     0,    53,     0,    43,    40,     0,     0,    41,
       0,     5,     6,   258,     0,     0,    42,    40,     0,     0,
      41,     0,     0,    43,   187,     0,     0,    42,     0,     5,
       6,     0,     0,     0,    43,    40,     0,     0,    41,     0,
       0,     0,     0,     0,     0,    42,     0,     0,     0,     0,
       0,     0,    43
};

static const yytype_int16 yycheck[] =
{
      24,     1,   148,    27,    11,   162,    30,    76,    82,     9,
      11,     9,    85,   103,     7,   103,    85,    14,    15,   176,
      19,    18,   179,    47,   100,    25,    75,   101,    90,     1,
      11,    31,    18,    21,    11,    16,    85,    86,    19,    16,
      18,    22,    19,    36,    32,    22,    18,    17,    72,   111,
      57,    39,    22,    14,    47,    53,    19,     5,     6,    36,
       8,    18,    34,    11,    45,    89,    14,    45,    45,    26,
      18,    95,    35,    21,   150,    14,    83,    75,    26,    18,
      28,    21,    83,    21,    19,   109,     1,   160,    45,   177,
      90,   160,   168,   183,    42,    39,   170,   171,   255,    39,
      35,    39,   126,    18,   102,    36,   130,    44,   265,   107,
      11,   111,   161,   189,    18,    16,    19,    36,   142,   117,
     194,    22,   146,   199,   198,   174,    45,   151,    32,    37,
       5,     6,    35,    18,   127,   281,    11,    45,    31,    14,
      14,    26,    18,    19,    45,    21,    21,    32,   172,    18,
     148,     1,    32,    28,    52,    30,    32,    26,    14,     9,
      15,    59,    18,    18,   162,   172,    33,    42,   169,    45,
      37,    14,   196,    33,    18,    18,    43,    37,   176,    19,
     256,   179,    14,    43,   260,   261,    18,    22,   262,   263,
     266,   267,     0,     1,    35,     3,   272,     5,     6,     7,
      37,    38,    10,    14,    12,    13,   282,    18,     5,     6,
     234,   209,     3,     4,     5,     6,    37,    14,    37,   243,
      11,    29,    41,    14,    21,   218,   219,    37,    38,   222,
      21,    28,    19,   309,   258,   259,    17,    28,    45,   315,
     316,   317,   235,   236,    16,    28,     3,    19,     5,     6,
      22,    40,    41,   327,   307,   308,    32,   255,    36,    36,
      35,   285,   286,   287,   338,    36,    19,   265,    19,    35,
      19,    31,   348,     5,     6,    31,    31,   351,   276,    11,
      32,    18,    14,   281,    22,    32,    18,    31,    38,    21,
      16,   289,    37,    14,    37,   295,    28,    43,   296,    17,
      31,    33,    18,    22,    33,     1,    20,     3,    43,     5,
       6,     7,     8,     9,    18,    11,    12,     1,    14,     1,
      33,    33,    18,    19,   295,    21,   350,    23,    24,    25,
      26,    27,    28,     1,    30,   289,   209,     1,    34,     3,
     296,     5,     6,     7,     8,     9,    42,    11,    12,    57,
      14,    31,    63,    76,    18,    19,   142,    21,   142,    23,
      24,    25,    26,    27,    28,   276,    30,   103,    40,     1,
      34,     3,   272,     5,     6,     7,     8,     9,    42,    11,
      12,   123,    14,    30,   298,    -1,    18,    19,    -1,    21,
      -1,    23,    24,    25,    26,    27,    28,    -1,    30,     5,
       6,    -1,    -1,    -1,    -1,    11,    -1,    -1,    14,    -1,
      42,    -1,    18,    -1,    -1,    21,    -1,     5,     6,     5,
       6,    -1,    28,    11,    -1,    11,    14,    33,    14,    -1,
      18,    -1,    18,    21,    -1,    21,    -1,    23,    24,    25,
      28,    -1,    28,    -1,    30,    33,     5,     6,    -1,    -1,
      -1,    -1,    11,    -1,    -1,    14,    -1,    -1,    -1,    18,
      -1,    -1,    21,    -1,    23,    24,    25,    -1,    -1,    28,
       3,    30,     5,     6,    -1,     8,    -1,    -1,    11,    -1,
      -1,    14,    -1,    -1,    -1,    18,    -1,    -1,    21,     3,
      -1,     5,     6,    26,    -1,    28,    -1,    11,    -1,    -1,
      14,    -1,     5,     6,    -1,     8,    -1,    21,    11,    -1,
      -1,    14,    -1,    -1,    28,    18,    -1,    -1,    21,    -1,
      -1,     5,     6,    26,     8,    28,    -1,    11,    -1,    -1,
      14,    -1,     5,     6,    18,     8,    -1,    21,    11,    -1,
      -1,    14,    26,    -1,    28,    18,    -1,    -1,    21,    -1,
       5,     6,    -1,     8,    -1,    28,    11,    -1,    -1,    14,
      -1,     5,     6,    18,    -1,    -1,    21,    11,    -1,    -1,
      14,    -1,    -1,    28,    18,    -1,    -1,    21,    -1,     5,
       6,    -1,    -1,    -1,    28,    11,    -1,    -1,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    21,    -1,    -1,    -1,    -1,
      -1,    -1,    28
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    47,     0,     1,     3,     5,     6,     7,    10,    12,
      13,    29,    48,    49,    50,    51,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    69,    70,
      71,    72,    73,    77,    78,   127,   128,   129,    18,   115,
      11,    14,    21,    28,    52,    61,    64,   116,   117,   118,
     119,   120,   127,     8,    18,    79,    99,   114,   116,   127,
      19,     1,    18,    80,    81,    82,    83,    86,    19,    35,
      18,    45,   124,   125,     1,     8,     9,    18,    19,    23,
      24,    25,    26,    27,    28,    30,    42,    51,    54,    61,
      87,    88,    89,    90,    91,    92,    93,    95,    96,    97,
      98,    99,   110,   113,   114,    19,    35,    36,   125,   124,
     125,    87,    19,    35,    14,   115,    39,    74,   119,    21,
      74,    14,    21,    74,    14,    18,   123,    19,    21,    32,
     115,   125,    44,    31,   112,    14,    14,   115,   120,   116,
      11,    16,    19,    22,   100,   101,   102,   125,    32,   103,
      79,    34,    81,    19,    37,    22,    35,    19,    17,   125,
      30,    42,   110,    91,    97,    36,    36,    36,   103,     3,
      26,    99,   114,    32,    42,    97,   110,   113,    28,   110,
     125,    34,    88,    37,   125,   100,   103,    18,    33,   104,
     105,   106,   107,   116,    26,    32,    92,    93,    94,    98,
      35,    19,    18,    23,    24,    25,    30,    65,    66,    67,
      68,   116,   126,    19,   125,    34,    35,    19,     3,     4,
      75,    76,   116,    14,    18,    14,    15,    18,   121,   122,
      14,    18,   121,   125,   115,    31,    31,   125,    31,    16,
      19,    22,   101,   102,   125,   125,    33,   105,   100,   125,
      15,    18,    84,    85,    97,   110,   104,   100,    18,   114,
     103,   103,    26,    99,   125,   110,   104,   104,    18,    93,
      36,   100,   108,   109,   111,    33,    37,    43,    22,    18,
      32,    32,   103,    31,   125,    17,    22,   103,   100,    37,
      18,    68,   116,   115,   115,    38,    37,   115,    37,    38,
      38,   125,   115,   115,    16,   125,    33,    40,    41,   104,
     100,   125,   125,   100,   100,   103,   103,   104,   100,   100,
     100,   109,    14,   106,    43,    17,    31,    33,   100,    18,
     125,   125,   125,    66,    63,    76,    22,   122,    33,    85,
      85,   100,   100,   100,   100,    20,    43,    18,   103,    33,
     103,    33,   100,   125,   103
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
            (yyval.typ) = (yyvsp[(2) - (3)].typ); (yyvsp[(2) - (3)].typ)->AddParent((yyvsp[(1) - (3)].typ)); (yyvsp[(2) - (3)].typ)->ptr = (yyvsp[(1) - (3)].typ)->ptr;
	    (yyvsp[(2) - (3)].typ)->par_formal.BorrowUnique((yyvsp[(1) - (3)].typ)->par_formal);
	    mta->type_stack.Pop(); }
    break;

  case 18:
#line 160 "mta_parse.y"
    { /* annoying place for a comment, but.. */
            (yyval.typ) = (yyvsp[(3) - (4)].typ); (yyvsp[(3) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(3) - (4)].typ)->ptr = (yyvsp[(1) - (4)].typ)->ptr;
	    (yyvsp[(3) - (4)].typ)->par_formal.BorrowUnique((yyvsp[(1) - (4)].typ)->par_formal);
	    mta->type_stack.Pop(); }
    break;

  case 19:
#line 166 "mta_parse.y"
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
#line 202 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(4) - (7)].typ); (yyval.typ)->AddParent(&TA_void_ptr); (yyval.typ)->ptr = 1;
	    mta->type_stack.Pop(); }
    break;

  case 21:
#line 205 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(4) - (4)].typ); (yyval.typ)->AddParent(&TA_void_ptr); (yyval.typ)->ptr = 1;
	    mta->type_stack.Pop(); }
    break;

  case 22:
#line 210 "mta_parse.y"
    {
            TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
            (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("enum", sp, (yyval.typ)); (yyval.typ)->source_end = mta->line-1; } }
    break;

  case 25:
#line 220 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); }
    break;

  case 26:
#line 221 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 27:
#line 224 "mta_parse.y"
    {
  	    (yyval.typ) = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->AddParFormal(&TA_enum); mta->cur_enum = (yyvsp[(2) - (2)].typ);
	    (yyval.typ)->source_file = mta->cur_fname;  (yyval.typ)->source_start = mta->line-1;
	    mta->type_stack.Pop(); }
    break;

  case 28:
#line 229 "mta_parse.y"
    {
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->cur_enum = (yyval.typ);
	    (yyval.typ)->source_file = mta->cur_fname;  (yyval.typ)->source_start = mta->line-1;
	    (yyval.typ)->AddParFormal(&TA_enum); (yyval.typ)->internal = true; }
    break;

  case 29:
#line 237 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("class", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 30:
#line 245 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 31:
#line 246 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 32:
#line 250 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("class", sp, (yyval.typ)); (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 35:
#line 263 "mta_parse.y"
    {
	    if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	    else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 36:
#line 269 "mta_parse.y"
    {
	    (yyvsp[(1) - (2)].typ)->tokens.keep = true; mta->Class_ResetCurPtrs();
	    (yyvsp[(1) - (2)].typ)->source_file = mta->cur_fname;  (yyvsp[(1) - (2)].typ)->source_start = mta->defn_st_line; }
    break;

  case 37:
#line 272 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
	    (yyvsp[(1) - (3)].typ)->source_file = mta->cur_fname;  (yyvsp[(1) - (3)].typ)->source_start = mta->defn_st_line; }
    break;

  case 38:
#line 275 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
	    (yyvsp[(1) - (3)].typ)->source_file = mta->cur_fname;  (yyvsp[(1) - (3)].typ)->source_start = mta->defn_st_line; 
	  }
    break;

  case 39:
#line 282 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 40:
#line 283 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 41:
#line 286 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->AddParFormal(&TA_class);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 42:
#line 291 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 43:
#line 295 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 44:
#line 301 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 45:
#line 310 "mta_parse.y"
    {
	    mta->cur_class->AddParent((yyvsp[(1) - (1)].typ)); }
    break;

  case 46:
#line 312 "mta_parse.y"
    {
	    mta->cur_class->AddParent((yyvsp[(3) - (3)].typ));
	    if(!mta->cur_class->HasOption("MULT_INHERIT"))
	      mta->cur_class->opts.Add("MULT_INHERIT"); }
    break;

  case 48:
#line 320 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 49:
#line 321 "mta_parse.y"
    { (yyval.typ) = &TA_class; }
    break;

  case 50:
#line 322 "mta_parse.y"
    { (yyval.typ) = &TA_class; }
    break;

  case 55:
#line 334 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ));
	      (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 58:
#line 348 "mta_parse.y"
    {
          if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	  else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 59:
#line 354 "mta_parse.y"
    {
            mta->state = MTA::Parse_inclass; (yyvsp[(1) - (2)].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 60:
#line 357 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 61:
#line 359 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 62:
#line 364 "mta_parse.y"
    {
	    (yyvsp[(5) - (5)].typ)->templ_pars.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_pars.Duplicate(mta->cur_templ_pars);
	    (yyvsp[(5) - (5)].typ)->internal = true;
	    (yyvsp[(5) - (5)].typ)->AddParFormal(&TA_template); (yyval.typ) = (yyvsp[(5) - (5)].typ);
	    (yyval.typ)->source_file = mta->cur_fname; (yyval.typ)->source_start = mta->defn_st_line-1; }
    break;

  case 63:
#line 372 "mta_parse.y"
    { mta->cur_templ_pars.Reset(); }
    break;

  case 65:
#line 377 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 66:
#line 381 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 67:
#line 382 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 68:
#line 383 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 69:
#line 384 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (3)].typ)); (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 70:
#line 387 "mta_parse.y"
    {
            if(mta->spc == &(mta->spc_target)) /* only add reg_funs in target space */
	       TA_taRegFun.methods.AddUniqNameNew((yyvsp[(1) - (1)].meth));
	    mta->meth_stack.Pop(); }
    break;

  case 71:
#line 393 "mta_parse.y"
    {
            (yyvsp[(3) - (4)].meth)->type = (yyvsp[(2) - (4)].typ); SETDESC((yyvsp[(3) - (4)].meth),(yyvsp[(4) - (4)].chr)); (yyval.meth) = (yyvsp[(3) - (4)].meth); }
    break;

  case 72:
#line 395 "mta_parse.y"
    {
            (yyvsp[(2) - (3)].meth)->type = &TA_int; SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); (yyval.meth) = (yyvsp[(2) - (3)].meth); }
    break;

  case 73:
#line 399 "mta_parse.y"
    {
            (yyvsp[(1) - (2)].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = true; }
    break;

  case 77:
#line 409 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 80:
#line 414 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 81:
#line 419 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 82:
#line 422 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm)); if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
	    mta->enum_stack.Pop(); }
    break;

  case 84:
#line 429 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 85:
#line 434 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 86:
#line 442 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 88:
#line 447 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 89:
#line 450 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 90:
#line 451 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 91:
#line 454 "mta_parse.y"
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

  case 92:
#line 463 "mta_parse.y"
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

  case 93:
#line 476 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 94:
#line 479 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 95:
#line 482 "mta_parse.y"
    { /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew((yyvsp[(1) - (2)].typ));
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 96:
#line 486 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 98:
#line 491 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 99:
#line 492 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 100:
#line 493 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 101:
#line 494 "mta_parse.y"
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

  case 103:
#line 515 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 104:
#line 521 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 105:
#line 522 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 106:
#line 523 "mta_parse.y"
    { }
    break;

  case 107:
#line 524 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    String nm = (yyvsp[(1) - (4)].typ)->name + "_ary";
	    TypeDef* nty = new TypeDef((char*)nm, true, (yyvsp[(1) - (4)].typ)->ptr + 1);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
	    TypeDef* uty = sp->AddUniqNameOld(nty); 
	    if(uty == nty) { mta->TypeAdded("array", sp, uty); 
	      nty->AddParFormal(&TA_ta_array); nty->AddParent((yyvsp[(1) - (4)].typ)); }
	    else { mta->TypeNotAdded("array", sp, uty, nty); delete nty; }
	    (yyvsp[(2) - (4)].memb)->type = uty; }
    break;

  case 108:
#line 534 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 109:
#line 538 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 110:
#line 542 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(3) - (3)].memb)->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 111:
#line 548 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 112:
#line 554 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 114:
#line 560 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 115:
#line 561 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 116:
#line 562 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 117:
#line 563 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 118:
#line 564 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 119:
#line 565 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 120:
#line 566 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 121:
#line 567 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 122:
#line 568 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 123:
#line 569 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 124:
#line 570 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 125:
#line 571 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 126:
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

  case 127:
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

  case 129:
#line 592 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 130:
#line 593 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 131:
#line 595 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 132:
#line 600 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 133:
#line 601 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
    break;

  case 134:
#line 602 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 135:
#line 603 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 136:
#line 604 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 137:
#line 607 "mta_parse.y"
    {
             (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 138:
#line 614 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 139:
#line 618 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 140:
#line 619 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
    break;

  case 141:
#line 620 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 142:
#line 621 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 143:
#line 622 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 144:
#line 623 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 145:
#line 627 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 146:
#line 628 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 147:
#line 629 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
    break;

  case 148:
#line 633 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 149:
#line 634 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 150:
#line 635 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 151:
#line 638 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 152:
#line 639 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 153:
#line 642 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 154:
#line 643 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 155:
#line 646 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 156:
#line 647 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 157:
#line 648 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 158:
#line 651 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 159:
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

  case 160:
#line 664 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 161:
#line 668 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 162:
#line 672 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 163:
#line 676 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 164:
#line 680 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 168:
#line 695 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 169:
#line 699 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 170:
#line 702 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 171:
#line 703 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 172:
#line 706 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 174:
#line 710 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 175:
#line 711 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 176:
#line 714 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
	                          (yyval.typ)->source_file = mta->cur_fname;
				  (yyval.typ)->source_start = mta->line-1; }
    break;

  case 178:
#line 720 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (2)].typ)->name + "_ref";
	    TypeDef* nty = new TypeDef((char*)nm, true, (yyvsp[(1) - (2)].typ)->ptr, true);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("ref", sp, (yyval.typ)); nty->AddParent((yyvsp[(1) - (2)].typ)); }
	    else { mta->TypeNotAdded("ref", sp, (yyval.typ), nty); delete nty; }
	  }
    break;

  case 180:
#line 732 "mta_parse.y"
    {
 	    int i; String nm = (yyvsp[(1) - (2)].typ)->name; for(i=0; i<(yyvsp[(2) - (2)].rval); i++) nm += "_ptr";
	    TypeDef* nty = new TypeDef((char*)nm, true, (yyvsp[(2) - (2)].rval)); 
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("ptr", sp, (yyval.typ)); nty->AddParent((yyvsp[(1) - (2)].typ)); }
	    else { mta->TypeNotAdded("ptr", sp, (yyval.typ), nty); delete nty; }
	  }
    break;

  case 182:
#line 743 "mta_parse.y"
    {
	    String nm = (yyvsp[(1) - (2)].typ)->name + "_" + (yyvsp[(2) - (2)].typ)->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("const", sp, (yyval.typ));
	                    nty->size = (yyvsp[(2) - (2)].typ)->size; nty->AddParent((yyvsp[(1) - (2)].typ)); nty->AddParent((yyvsp[(2) - (2)].typ)); }
	    else { mta->TypeNotAdded("const", sp, (yyval.typ), nty); delete nty; }
	  }
    break;

  case 184:
#line 755 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 185:
#line 756 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 186:
#line 757 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 187:
#line 761 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 188:
#line 765 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 189:
#line 766 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 190:
#line 767 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 192:
#line 769 "mta_parse.y"
    { /* a template */
 	    if(!((yyvsp[(1) - (4)].typ)->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    if(((yyvsp[(3) - (4)].typ)->owner != NULL) && ((yyvsp[(3) - (4)].typ)->owner->owner != NULL))
	      (yyval.typ) = (yyvsp[(1) - (4)].typ);	/* don't allow internal types with external templates */
	    else {
	      String nm = (yyvsp[(1) - (4)].typ)->GetTemplName(mta->cur_templ_pars);
	      TypeDef* td;
	      int lx_tok;
	      if((td = mta->FindName(nm, lx_tok)) == NULL) {
		td = (yyvsp[(1) - (4)].typ)->Clone(); td->name = nm;
		td->SetTemplType((yyvsp[(1) - (4)].typ), mta->cur_templ_pars);
		TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
 		(yyval.typ) = sp->AddUniqNameOld(td);
		if((yyval.typ) == td) mta->TypeAdded("template instance", sp, (yyval.typ)); }
	      else
		(yyval.typ) = td; } }
    break;

  case 193:
#line 786 "mta_parse.y"
    { /* this template */
	    if(!((yyvsp[(1) - (4)].typ)->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 195:
#line 793 "mta_parse.y"
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

  case 197:
#line 806 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 198:
#line 811 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 199:
#line 812 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 200:
#line 813 "mta_parse.y"
    { (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 202:
#line 818 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 209:
#line 834 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 210:
#line 835 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 211:
#line 839 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 212:
#line 843 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 2961 "y.tab.c"
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

