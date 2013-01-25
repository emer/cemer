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
     MP_PUBLIC = 277,
     MP_PRIVATE = 278,
     MP_PROTECTED = 279,
     MP_OPERATOR = 280,
     MP_FRIEND = 281,
     MP_THISNAME = 282,
     MP_REGFUN = 283,
     MP_VIRTUAL = 284
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
#define MP_PUBLIC 277
#define MP_PRIVATE 278
#define MP_PROTECTED 279
#define MP_OPERATOR 280
#define MP_FRIEND 281
#define MP_THISNAME 282
#define MP_REGFUN 283
#define MP_VIRTUAL 284




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
#define YYLAST   575

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  45
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  84
/* YYNRULES -- Number of rules.  */
#define YYNRULES  213
/* YYNRULES -- Number of states.  */
#define YYNSTATES  358

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    43,     2,
      31,    32,    30,    39,    36,    40,    42,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    35,    44,
      38,     2,    37,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    34,     2,    33,    41,     2,     2,     2,
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
      25,    26,    27,    28,    29
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    19,    22,
      25,    27,    29,    32,    35,    40,    44,    49,    52,    60,
      65,    67,    72,    75,    79,    83,    86,    88,    90,    93,
      97,    99,   102,   106,   110,   113,   117,   121,   123,   127,
     130,   133,   135,   137,   139,   141,   145,   147,   150,   152,
     155,   157,   160,   162,   164,   166,   169,   173,   177,   180,
     184,   188,   194,   196,   198,   202,   205,   208,   211,   215,
     217,   222,   226,   229,   231,   234,   236,   239,   241,   244,
     246,   248,   252,   254,   258,   262,   264,   266,   268,   270,
     273,   275,   277,   279,   281,   284,   286,   288,   291,   294,
     297,   299,   301,   304,   308,   313,   316,   321,   326,   328,
     332,   334,   339,   341,   345,   350,   355,   360,   365,   371,
     377,   383,   388,   392,   397,   402,   408,   413,   415,   418,
     421,   425,   429,   432,   437,   444,   448,   451,   453,   455,
     457,   460,   463,   466,   470,   472,   475,   479,   481,   483,
     486,   489,   493,   495,   498,   500,   504,   509,   511,   514,
     516,   519,   523,   530,   532,   534,   537,   541,   544,   546,
     548,   551,   553,   555,   557,   560,   562,   564,   567,   569,
     572,   574,   577,   579,   582,   585,   588,   591,   595,   599,
     603,   607,   610,   612,   617,   622,   624,   627,   629,   633,
     635,   637,   639,   641,   643,   645,   648,   650,   652,   654,
     656,   658,   660,   662
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      46,     0,    -1,    -1,    46,    47,    -1,    46,    51,    -1,
      46,    55,    -1,    46,    57,    -1,    46,    67,    -1,    46,
      75,    -1,    46,     1,    -1,    48,    -1,    49,    -1,    49,
      18,    -1,    12,    50,    -1,    12,    59,   121,   123,    -1,
     114,   113,   123,    -1,   114,    18,   113,   123,    -1,   114,
     123,    -1,   114,    31,    30,   113,    32,   101,   123,    -1,
     114,    20,    30,   113,    -1,    52,    -1,    53,    78,    33,
     123,    -1,    54,    34,    -1,    54,    18,    34,    -1,    54,
      34,    18,    -1,     7,   113,    -1,     7,    -1,    56,    -1,
      62,   123,    -1,    62,   123,    18,    -1,    58,    -1,    59,
     123,    -1,    59,   122,   123,    -1,    60,    85,    33,    -1,
      61,    34,    -1,    61,    18,    34,    -1,    61,    34,    18,
      -1,    62,    -1,    62,    35,    63,    -1,   127,   113,    -1,
     127,    13,    -1,   127,    -1,   125,    -1,   126,    -1,    64,
      -1,    63,    36,    64,    -1,   114,    -1,    65,   114,    -1,
      17,    -1,    65,    17,    -1,    66,    -1,    65,    66,    -1,
     124,    -1,    29,    -1,    68,    -1,    69,   123,    -1,    69,
     122,   123,    -1,    70,    85,    33,    -1,    71,    34,    -1,
      71,    18,    34,    -1,    71,    34,    18,    -1,   128,    72,
      73,    37,    61,    -1,    38,    -1,    74,    -1,    74,    36,
      74,    -1,     3,   113,    -1,     4,   113,    -1,   114,   113,
      -1,   114,   113,    21,    -1,    76,    -1,    28,   112,    77,
      98,    -1,    28,    77,    98,    -1,    97,   101,    -1,    79,
      -1,    78,    79,    -1,    80,    -1,    80,    18,    -1,    81,
      -1,    81,    36,    -1,     1,    -1,    84,    -1,    84,    21,
      82,    -1,    83,    -1,    82,    39,    83,    -1,    82,    40,
      83,    -1,    17,    -1,    14,    -1,    17,    -1,    86,    -1,
      85,    86,    -1,    87,    -1,    93,    -1,    52,    -1,    49,
      -1,    59,   123,    -1,     1,    -1,    88,    -1,    22,    35,
      -1,    23,    35,    -1,    24,    35,    -1,    18,    -1,    89,
      -1,     9,    89,    -1,   111,    90,   123,    -1,   111,    91,
      21,   123,    -1,    90,   123,    -1,   111,    91,    16,   123,
      -1,   111,    92,   101,   123,    -1,    91,    -1,    90,    36,
      91,    -1,    17,    -1,    31,    30,    17,    32,    -1,    94,
      -1,   108,   102,    98,    -1,     8,   108,   102,    98,    -1,
      29,   108,   102,    98,    -1,   108,   102,   106,    98,    -1,
      41,   108,   102,    98,    -1,     8,    41,   108,   102,    98,
      -1,    29,    41,   108,   102,    98,    -1,    26,   112,    25,
     101,    98,    -1,    26,    25,   101,    98,    -1,    26,   112,
     123,    -1,    26,     3,   112,   123,    -1,    26,     3,    17,
     123,    -1,    26,   112,    97,   101,    98,    -1,    26,    97,
     101,    98,    -1,    95,    -1,     9,    95,    -1,    29,    95,
      -1,     8,    29,    95,    -1,   111,    96,    98,    -1,    96,
      98,    -1,   111,    25,   101,    98,    -1,   111,    25,    31,
      32,   101,    98,    -1,    25,   101,    98,    -1,    97,   101,
      -1,    17,    -1,   123,    -1,    99,    -1,   100,   123,    -1,
      18,   123,    -1,    18,    99,    -1,    18,   100,   123,    -1,
      15,    -1,    11,    15,    -1,    11,    18,    15,    -1,    21,
      -1,    11,    -1,    11,    21,    -1,    31,    32,    -1,    31,
     103,    32,    -1,    32,    -1,   103,    32,    -1,   104,    -1,
     103,    36,   104,    -1,   103,    42,    42,    42,    -1,   105,
      -1,   105,    21,    -1,   114,    -1,   114,    17,    -1,   114,
      17,    16,    -1,   114,    31,    30,    17,    32,   101,    -1,
      17,    -1,   107,    -1,   106,   107,    -1,   109,    13,    19,
      -1,    27,    31,    -1,    35,    -1,    30,    -1,   110,    30,
      -1,   112,    -1,   114,    -1,     8,    -1,     8,   114,    -1,
      17,    -1,   115,    -1,   115,    43,    -1,   116,    -1,   116,
     110,    -1,   117,    -1,    11,   117,    -1,   118,    -1,   125,
     118,    -1,   125,   113,    -1,   126,   118,    -1,   126,   113,
      -1,    13,    20,    17,    -1,    27,    20,    17,    -1,    13,
      20,    13,    -1,    27,    20,    13,    -1,    20,    13,    -1,
      27,    -1,    13,    72,   119,    37,    -1,    27,    72,   119,
      37,    -1,    13,    -1,   118,    13,    -1,   120,    -1,   119,
      36,   120,    -1,    13,    -1,    17,    -1,    14,    -1,    17,
      -1,    13,    -1,    17,    -1,    17,    16,    -1,    44,    -1,
      22,    -1,    23,    -1,    24,    -1,     5,    -1,     6,    -1,
       3,    -1,    10,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   111,   111,   112,   115,   117,   120,   123,   125,   127,
     131,   139,   140,   144,   145,   149,   152,   157,   191,   194,
     199,   205,   208,   209,   210,   213,   217,   224,   232,   233,
     237,   246,   247,   251,   257,   260,   263,   270,   271,   274,
     279,   283,   290,   297,   307,   309,   316,   317,   318,   319,
     322,   323,   326,   327,   331,   340,   341,   345,   351,   354,
     356,   361,   368,   372,   373,   377,   378,   379,   380,   383,
     392,   394,   398,   403,   404,   407,   408,   411,   412,   413,
     418,   421,   427,   428,   433,   441,   442,   446,   449,   450,
     453,   462,   475,   478,   481,   485,   489,   490,   491,   492,
     493,   513,   514,   520,   521,   522,   523,   530,   534,   538,
     544,   550,   555,   556,   557,   558,   559,   560,   561,   562,
     563,   564,   565,   566,   567,   568,   579,   593,   594,   595,
     597,   602,   603,   604,   605,   606,   609,   616,   620,   621,
     622,   623,   624,   625,   629,   630,   631,   635,   636,   637,
     640,   641,   644,   645,   648,   649,   650,   653,   655,   666,
     670,   674,   678,   682,   689,   690,   693,   697,   701,   704,
     705,   708,   711,   712,   713,   716,   720,   721,   730,   731,
     742,   743,   751,   752,   753,   754,   755,   756,   760,   764,
     765,   766,   767,   768,   785,   791,   792,   804,   805,   810,
     811,   812,   816,   817,   820,   821,   824,   827,   828,   829,
     833,   837,   841,   845
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
  "MP_EQUALS", "MP_PUBLIC", "MP_PRIVATE", "MP_PROTECTED", "MP_OPERATOR",
  "MP_FRIEND", "MP_THISNAME", "MP_REGFUN", "MP_VIRTUAL", "'*'", "'('",
  "')'", "'}'", "'{'", "':'", "','", "'>'", "'<'", "'+'", "'-'", "'~'",
  "'.'", "'&'", "';'", "$accept", "list", "typedefn", "typedefns",
  "typedsub", "defn", "enumdefn", "enumdsub", "enumname", "enumnm",
  "classdecl", "classdecls", "classdefn", "classdefns", "classdsub",
  "classname", "classhead", "classnm", "classinh", "classpar", "classptyp",
  "classpmod", "templdefn", "templdefns", "templdsub", "templname",
  "templhead", "templopen", "templpars", "templpar", "fundecl", "funnm",
  "regfundefn", "enums", "enumline", "enumitm", "enumitms", "enummath",
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
      42,    40,    41,   125,   123,    58,    44,    62,    60,    43,
      45,   126,    46,    38,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    45,    46,    46,    46,    46,    46,    46,    46,    46,
      47,    48,    48,    49,    49,    50,    50,    50,    50,    50,
      51,    52,    53,    53,    53,    54,    54,    55,    56,    56,
      57,    58,    58,    59,    60,    60,    60,    61,    61,    62,
      62,    62,    62,    62,    63,    63,    64,    64,    64,    64,
      65,    65,    66,    66,    67,    68,    68,    69,    70,    70,
      70,    71,    72,    73,    73,    74,    74,    74,    74,    75,
      76,    76,    77,    78,    78,    79,    79,    80,    80,    80,
      81,    81,    82,    82,    82,    83,    83,    84,    85,    85,
      86,    86,    86,    86,    86,    86,    87,    87,    87,    87,
      87,    88,    88,    89,    89,    89,    89,    89,    90,    90,
      91,    92,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    94,    94,    94,
      94,    95,    95,    95,    95,    95,    96,    97,    98,    98,
      98,    98,    98,    98,    99,    99,    99,   100,   100,   100,
     101,   101,   102,   102,   103,   103,   103,   104,   104,   105,
     105,   105,   105,   105,   106,   106,   107,   108,   109,   110,
     110,   111,   112,   112,   112,   113,   114,   114,   115,   115,
     116,   116,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   118,   118,   119,   119,   120,
     120,   120,   121,   121,   122,   122,   123,   124,   124,   124,
     125,   126,   127,   128
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       1,     1,     2,     2,     4,     3,     4,     2,     7,     4,
       1,     4,     2,     3,     3,     2,     1,     1,     2,     3,
       1,     2,     3,     3,     2,     3,     3,     1,     3,     2,
       2,     1,     1,     1,     1,     3,     1,     2,     1,     2,
       1,     2,     1,     1,     1,     2,     3,     3,     2,     3,
       3,     5,     1,     1,     3,     2,     2,     2,     3,     1,
       4,     3,     2,     1,     2,     1,     2,     1,     2,     1,
       1,     3,     1,     3,     3,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     2,     1,     1,     2,     2,     2,
       1,     1,     2,     3,     4,     2,     4,     4,     1,     3,
       1,     4,     1,     3,     4,     4,     4,     4,     5,     5,
       5,     4,     3,     4,     4,     5,     4,     1,     2,     2,
       3,     3,     2,     4,     6,     3,     2,     1,     1,     1,
       2,     2,     2,     3,     1,     2,     3,     1,     1,     2,
       2,     3,     1,     2,     1,     3,     4,     1,     2,     1,
       2,     3,     6,     1,     1,     2,     3,     2,     1,     1,
       2,     1,     1,     1,     2,     1,     1,     2,     1,     2,
       1,     2,     1,     2,     2,     2,     2,     3,     3,     3,
       3,     2,     1,     4,     4,     1,     2,     1,     3,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     9,   212,   210,   211,    26,   213,     0,
       0,     3,    10,    11,     4,    20,     0,     0,     5,    27,
       6,    30,     0,     0,     0,    37,     7,    54,     0,     0,
       0,     8,    69,    42,    43,    41,     0,   175,    25,     0,
     195,     0,   192,    13,     0,    37,     0,   176,   178,   180,
     182,    42,    43,   173,   137,     0,     0,     0,   172,     0,
       0,    12,    79,    87,     0,    73,    75,    77,    80,     0,
      22,   204,   206,     0,    31,    95,   173,     0,   110,   100,
       0,     0,     0,     0,     0,   192,     0,     0,    93,    92,
       0,     0,    88,    90,    96,   101,     0,   108,    91,   112,
     127,     0,     0,     0,     0,   171,     0,    34,     0,    28,
       0,    55,     0,     0,    58,    40,    39,    62,     0,   181,
       0,     0,   191,     0,     0,   203,   202,     0,     0,     0,
       0,     0,    17,   177,   169,   179,   196,   195,   184,   183,
     186,   185,   174,   148,   144,     0,   147,    71,   139,     0,
     138,     0,    72,     0,     0,    74,    76,    78,     0,    23,
      24,   205,    32,     0,     0,     0,   102,   128,    97,    98,
      99,     0,     0,     0,     0,     0,   167,     0,   129,     0,
       0,     0,     0,    94,    33,    89,     0,   105,   132,   136,
     163,   152,     0,     0,   154,   157,   159,     0,     0,     0,
     108,     0,     0,    35,    36,    48,   207,   208,   209,    53,
      38,    44,     0,    50,    46,    52,    29,    56,    57,    59,
      60,     0,     0,     0,    63,     0,   189,   187,   199,   201,
     200,     0,   197,   190,   188,     0,    14,     0,     0,     0,
      15,   170,   145,     0,   149,   142,     0,   141,   140,   150,
       0,    70,    21,    86,    85,    81,    82,   130,     0,     0,
     135,     0,     0,     0,     0,     0,     0,   122,     0,     0,
       0,   110,   109,   168,   113,     0,   164,     0,   153,     0,
       0,   158,   160,     0,     0,     0,     0,   103,     0,     0,
       0,   131,     0,    49,    51,    47,    65,    66,     0,     0,
      67,     0,   193,   194,    16,    19,     0,   146,   143,   151,
       0,     0,     0,   114,   124,   123,   121,   126,     0,     0,
       0,   115,   117,   116,   165,     0,   155,     0,   161,     0,
     150,   133,     0,   106,   104,   107,    45,    61,    64,    68,
     198,     0,    83,    84,   118,   120,   125,   119,   166,   156,
       0,     0,   111,     0,     0,   134,    18,   162
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    11,    12,    88,    43,    14,    89,    16,    17,
      18,    19,    20,    21,    90,    23,    24,    45,   210,   211,
     212,   213,    26,    27,    28,    29,    30,   124,   223,   224,
      31,    32,    55,    64,    65,    66,    67,   255,   256,    68,
      91,    92,    93,    94,    95,    96,    97,   201,    98,    99,
     100,   101,   102,   147,   148,   149,   152,   192,   193,   194,
     195,   275,   276,   103,   277,   135,   104,   105,   138,    58,
      47,    48,    49,    50,   231,   232,   127,    73,   150,   215,
      59,    60,    35,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -155
static const yytype_int16 yypact[] =
{
    -155,   181,  -155,  -155,  -155,  -155,  -155,     2,  -155,   165,
     522,  -155,  -155,    -4,  -155,  -155,    54,    69,  -155,  -155,
    -155,  -155,     4,   367,    79,   105,  -155,  -155,     4,   367,
      99,  -155,  -155,  -155,  -155,   147,     7,  -155,  -155,   208,
      37,   112,    60,  -155,   241,   101,   104,   122,   137,  -155,
     170,   247,   247,   184,  -155,    16,   172,   191,  -155,   247,
     247,  -155,  -155,  -155,    19,  -155,   192,   182,   204,   197,
     225,   254,  -155,   201,  -155,  -155,   132,   496,   242,  -155,
     243,   251,   256,   172,   467,    23,    59,   266,  -155,  -155,
     201,   299,  -155,  -155,  -155,  -155,    41,  -155,  -155,  -155,
    -155,    16,   172,   221,    85,  -155,   260,   277,   422,   278,
     201,  -155,   336,   263,   281,  -155,  -155,  -155,   485,  -155,
     252,   216,  -155,   258,   216,  -155,  -155,   201,     2,   273,
     279,   201,  -155,  -155,  -155,   283,  -155,  -155,  -155,   170,
    -155,   170,  -155,   229,  -155,   135,  -155,  -155,  -155,   201,
    -155,   392,  -155,    16,   201,  -155,  -155,  -155,   245,  -155,
    -155,  -155,  -155,   509,   266,   221,  -155,  -155,  -155,  -155,
    -155,    16,   535,   172,   172,    22,  -155,   266,  -155,   221,
     149,   284,   221,  -155,  -155,  -155,   297,  -155,  -155,  -155,
    -155,  -155,    38,   115,  -155,   306,   111,   287,   290,    41,
      62,   172,    16,  -155,  -155,  -155,  -155,  -155,  -155,  -155,
     293,  -155,   442,  -155,  -155,  -155,  -155,  -155,  -155,  -155,
    -155,     2,     2,   296,   294,     2,  -155,  -155,  -155,  -155,
    -155,    83,  -155,  -155,  -155,   180,  -155,   201,     2,     2,
    -155,  -155,  -155,   319,  -155,  -155,   201,  -155,  -155,  -155,
     210,  -155,  -155,  -155,  -155,   244,  -155,  -155,   221,    16,
    -155,   201,   201,    16,    16,   172,   172,  -155,   221,    16,
      16,  -155,  -155,  -155,  -155,    38,  -155,   322,  -155,   548,
     304,  -155,   320,   308,   405,    16,   333,  -155,   201,   201,
     201,  -155,   422,  -155,  -155,  -155,  -155,  -155,   276,   485,
     330,   216,  -155,  -155,  -155,  -155,   323,  -155,  -155,  -155,
     245,   245,    16,  -155,  -155,  -155,  -155,  -155,    16,    16,
      16,  -155,  -155,  -155,  -155,   338,  -155,   310,  -155,   347,
     172,  -155,   334,  -155,  -155,  -155,  -155,  -155,  -155,  -155,
    -155,   172,  -155,  -155,  -155,  -155,  -155,  -155,  -155,  -155,
     335,    16,  -155,   201,   172,  -155,  -155,  -155
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -155,  -155,  -155,  -155,   370,  -155,  -155,   380,  -155,  -155,
    -155,  -155,  -155,  -155,   193,  -155,    84,   382,  -155,    94,
    -155,   176,  -155,  -155,  -155,  -155,  -155,   240,  -155,    96,
    -155,  -155,   342,  -155,   337,  -155,  -155,  -155,   -21,  -155,
     371,   -49,  -155,  -155,   325,   300,   -94,  -155,  -155,  -155,
     -64,   -87,     5,   -63,   261,   262,   -67,  -154,  -143,   134,
    -155,  -155,   139,   -46,  -155,  -155,   -68,    -3,    -2,    -7,
    -155,  -155,   376,   103,   302,   116,  -155,   393,   -22,  -155,
       0,     3,  -155,  -155
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -138
static const yytype_int16 yytable[] =
{
      74,    33,    46,   109,    34,    38,   111,    57,   250,    51,
     200,   259,    52,   167,    61,    56,   171,   202,   180,    37,
      62,    71,   178,    51,   132,   269,    52,   143,   270,    51,
     165,   144,    52,   116,   145,   189,    63,   146,   188,    54,
     179,   182,   185,   123,   131,   117,   142,   265,    72,   143,
     140,   162,   154,   144,   176,    62,   145,   120,   140,   146,
      72,   117,    56,   185,     5,     6,    72,    53,   183,   142,
      39,    63,    40,   273,   187,   117,    54,   186,   288,    41,
     123,   175,    72,   289,    83,    72,    85,    69,   217,   174,
     251,    51,   272,   202,    52,   180,   196,   106,   117,   257,
     177,   214,    78,    70,   312,   236,   263,   264,   260,   240,
     197,   225,    51,   107,   320,    52,   198,   113,   258,   301,
     302,    37,   128,   247,   129,   122,   237,   248,   282,   274,
     285,   268,   252,   114,   290,   130,   108,     5,     6,   291,
     108,   250,   283,    39,   196,    40,   143,   278,    72,    72,
     144,   279,    41,   267,   139,   141,   146,   280,   196,    85,
     115,   163,   139,   141,    37,   133,    54,   134,     4,   262,
       5,     6,   196,   164,   197,   196,    39,   287,    40,    72,
     266,     2,     3,   136,     4,    41,     5,     6,     7,     5,
       6,     8,    42,     9,    22,    39,   313,    40,   318,   319,
     316,   317,    44,   151,    41,   295,   321,   322,    54,    10,
     156,    42,   323,     5,     6,   304,   301,   303,   157,   296,
     297,    40,   331,   300,   308,   158,     5,     6,    41,   228,
     229,   159,    39,   230,    40,    42,   305,   306,   190,   314,
     315,    41,   309,   160,   242,    72,   279,   243,    42,   344,
     244,   196,   280,   191,   125,   345,   346,   347,   126,   253,
     137,   196,   254,   351,    37,   226,   333,   334,   335,   227,
     161,   233,   196,  -137,   353,   234,   118,   196,   168,     4,
     121,     5,     6,   310,   311,   214,   169,   357,   355,   342,
     343,   170,   225,   181,   203,   204,   216,   219,    33,   220,
      75,    34,     4,   238,     5,     6,     7,    76,    77,   239,
      39,     9,    40,   241,   271,   176,    78,    79,   284,    41,
     286,    80,    81,    82,    83,    84,    85,   281,    86,   292,
     299,   356,   184,   298,   307,   325,   328,    75,   329,     4,
      87,     5,     6,     7,    76,    77,   327,    39,     9,    40,
     332,   339,   349,    78,    79,   341,    41,   348,    80,    81,
      82,    83,    84,    85,   350,    86,   352,   354,    75,   218,
       4,    13,     5,     6,     7,    76,    77,    87,    39,     9,
      40,    15,   337,    25,    78,    79,   336,    41,   294,    80,
      81,    82,    83,    84,    85,   338,    86,     5,     6,   153,
     112,   155,   166,    39,   199,    40,   245,   246,    87,   190,
       5,     6,    41,   326,   324,   119,    39,   340,    40,    42,
       0,   110,   190,     0,   249,    41,   235,     5,     6,     0,
       0,     0,    42,    39,     0,    40,     0,   330,     0,   205,
       0,     0,    41,     0,   206,   207,   208,     5,     6,    42,
       0,   209,     0,    39,     0,    40,     0,     0,     0,   293,
       0,     0,    41,     0,   206,   207,   208,     0,     0,    42,
     172,   209,     5,     6,     0,    53,     0,     0,    39,     0,
      40,     0,     0,     0,    54,     0,     0,    41,   221,   222,
       5,     6,   173,     0,    42,     0,    39,     0,    40,     0,
       0,     5,     6,     0,    53,    41,     0,    39,     0,    40,
       0,     0,    42,    78,     5,     6,    41,    53,     0,     0,
      39,    83,    40,    42,     0,     0,    54,     5,     6,    41,
      53,     0,     0,    39,    83,    40,    42,     0,     0,    54,
       5,     6,    41,    53,     0,     0,    39,     0,    40,    42,
       0,     0,   261,     5,     6,    41,     0,     0,     0,    39,
       0,    40,    42,     0,     0,   190,     0,     0,    41,     0,
       0,     0,     0,     0,     0,    42
};

static const yytype_int16 yycheck[] =
{
      22,     1,     9,    25,     1,     7,    28,    10,   151,     9,
     104,   165,     9,    77,    18,    10,    83,   104,    86,    17,
       1,    17,    86,    23,    46,   179,    23,    11,   182,    29,
      76,    15,    29,    35,    18,   102,    17,    21,   101,    17,
      86,    87,    91,    20,    46,    38,    53,    25,    44,    11,
      52,    73,    33,    15,    31,     1,    18,    20,    60,    21,
      44,    38,    57,   112,     5,     6,    44,     8,    90,    76,
      11,    17,    13,    35,    96,    38,    17,    36,    16,    20,
      20,    84,    44,    21,    25,    44,    27,    18,   110,    84,
     153,    91,   186,   180,    91,   163,   103,    18,    38,   163,
      41,   108,    17,    34,   258,   127,   173,   174,   171,   131,
      25,   118,   112,    34,   268,   112,    31,    18,   164,    36,
      37,    17,    18,   145,    20,    13,   128,   149,    17,   192,
     197,   177,   154,    34,   201,    31,    35,     5,     6,   202,
      35,   284,    31,    11,   151,    13,    11,    32,    44,    44,
      15,    36,    20,   175,    51,    52,    21,    42,   165,    27,
      13,    29,    59,    60,    17,    43,    17,    30,     3,   172,
       5,     6,   179,    41,    25,   182,    11,   199,    13,    44,
     175,     0,     1,    13,     3,    20,     5,     6,     7,     5,
       6,    10,    27,    12,     1,    11,   259,    13,   265,   266,
     263,   264,     9,    31,    20,   212,   269,   270,    17,    28,
      18,    27,   275,     5,     6,   237,    36,    37,    36,   221,
     222,    13,   285,   225,   246,    21,     5,     6,    20,    13,
      14,    34,    11,    17,    13,    27,   238,   239,    17,   261,
     262,    20,    32,    18,    15,    44,    36,    18,    27,   312,
      21,   258,    42,    32,    13,   318,   319,   320,    17,    14,
      13,   268,    17,   330,    17,    13,   288,   289,   290,    17,
      16,    13,   279,    31,   341,    17,    36,   284,    35,     3,
      40,     5,     6,    39,    40,   292,    35,   354,   351,   310,
     311,    35,   299,    27,    34,    18,    18,    34,   298,    18,
       1,   298,     3,    30,     5,     6,     7,     8,     9,    30,
      11,    12,    13,    30,    17,    31,    17,    18,    31,    20,
      30,    22,    23,    24,    25,    26,    27,    21,    29,    36,
      36,   353,    33,    37,    15,    13,    16,     1,    30,     3,
      41,     5,     6,     7,     8,     9,    42,    11,    12,    13,
      17,    21,    42,    17,    18,    32,    20,    19,    22,    23,
      24,    25,    26,    27,    17,    29,    32,    32,     1,    33,
       3,     1,     5,     6,     7,     8,     9,    41,    11,    12,
      13,     1,   298,     1,    17,    18,   292,    20,   212,    22,
      23,    24,    25,    26,    27,   299,    29,     5,     6,    57,
      29,    64,    77,    11,   104,    13,   145,   145,    41,    17,
       5,     6,    20,   279,   275,    39,    11,   301,    13,    27,
      -1,    28,    17,    -1,    32,    20,   124,     5,     6,    -1,
      -1,    -1,    27,    11,    -1,    13,    -1,    32,    -1,    17,
      -1,    -1,    20,    -1,    22,    23,    24,     5,     6,    27,
      -1,    29,    -1,    11,    -1,    13,    -1,    -1,    -1,    17,
      -1,    -1,    20,    -1,    22,    23,    24,    -1,    -1,    27,
       3,    29,     5,     6,    -1,     8,    -1,    -1,    11,    -1,
      13,    -1,    -1,    -1,    17,    -1,    -1,    20,     3,     4,
       5,     6,    25,    -1,    27,    -1,    11,    -1,    13,    -1,
      -1,     5,     6,    -1,     8,    20,    -1,    11,    -1,    13,
      -1,    -1,    27,    17,     5,     6,    20,     8,    -1,    -1,
      11,    25,    13,    27,    -1,    -1,    17,     5,     6,    20,
       8,    -1,    -1,    11,    25,    13,    27,    -1,    -1,    17,
       5,     6,    20,     8,    -1,    -1,    11,    -1,    13,    27,
      -1,    -1,    17,     5,     6,    20,    -1,    -1,    -1,    11,
      -1,    13,    27,    -1,    -1,    17,    -1,    -1,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    27
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    46,     0,     1,     3,     5,     6,     7,    10,    12,
      28,    47,    48,    49,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    67,    68,    69,    70,
      71,    75,    76,   125,   126,   127,   128,    17,   113,    11,
      13,    20,    27,    50,    59,    62,   114,   115,   116,   117,
     118,   125,   126,     8,    17,    77,    97,   112,   114,   125,
     126,    18,     1,    17,    78,    79,    80,    81,    84,    18,
      34,    17,    44,   122,   123,     1,     8,     9,    17,    18,
      22,    23,    24,    25,    26,    27,    29,    41,    49,    52,
      59,    85,    86,    87,    88,    89,    90,    91,    93,    94,
      95,    96,    97,   108,   111,   112,    18,    34,    35,   123,
     122,   123,    85,    18,    34,    13,   113,    38,    72,   117,
      20,    72,    13,    20,    72,    13,    17,   121,    18,    20,
      31,   113,   123,    43,    30,   110,    13,    13,   113,   118,
     113,   118,   114,    11,    15,    18,    21,    98,    99,   100,
     123,    31,   101,    77,    33,    79,    18,    36,    21,    34,
      18,    16,   123,    29,    41,   108,    89,    95,    35,    35,
      35,   101,     3,    25,    97,   112,    31,    41,    95,   108,
     111,    27,   108,   123,    33,    86,    36,   123,    98,   101,
      17,    32,   102,   103,   104,   105,   114,    25,    31,    90,
      91,    92,    96,    34,    18,    17,    22,    23,    24,    29,
      63,    64,    65,    66,   114,   124,    18,   123,    33,    34,
      18,     3,     4,    73,    74,   114,    13,    17,    13,    14,
      17,   119,   120,    13,    17,   119,   123,   113,    30,    30,
     123,    30,    15,    18,    21,    99,   100,   123,   123,    32,
     103,    98,   123,    14,    17,    82,    83,    95,   108,   102,
      98,    17,   112,   101,   101,    25,    97,   123,   108,   102,
     102,    17,    91,    35,    98,   106,   107,   109,    32,    36,
      42,    21,    17,    31,    31,   101,    30,   123,    16,    21,
     101,    98,    36,    17,    66,   114,   113,   113,    37,    36,
     113,    36,    37,    37,   123,   113,   113,    15,   123,    32,
      39,    40,   102,    98,   123,   123,    98,    98,   101,   101,
     102,    98,    98,    98,   107,    13,   104,    42,    16,    30,
      32,    98,    17,   123,   123,   123,    64,    61,    74,    21,
     120,    32,    83,    83,    98,    98,    98,    98,    19,    42,
      17,   101,    32,   101,    32,    98,   123,   101
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
#line 111 "mta_parse.y"
    { mta->yy_state = MTA::YYRet_Exit; }
    break;

  case 3:
#line 112 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok;
	    return mta->yy_state; }
    break;

  case 4:
#line 115 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 5:
#line 117 "mta_parse.y"
    {
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 6:
#line 120 "mta_parse.y"
    {
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 7:
#line 123 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 8:
#line 125 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 9:
#line 127 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
    break;

  case 10:
#line 131 "mta_parse.y"
    {
  	  if((yyvsp[(1) - (1)].typ) != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("typedef", sp, (yyval.typ)); } }
    break;

  case 11:
#line 139 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 12:
#line 140 "mta_parse.y"
    {
	    if((yyvsp[(1) - (2)].typ) != NULL)  SETDESC((yyvsp[(1) - (2)].typ),(yyvsp[(2) - (2)].chr)); }
    break;

  case 13:
#line 144 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 14:
#line 145 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[(2) - (4)].typ); (yyvsp[(2) - (4)].typ)->name = (yyvsp[(3) - (4)].chr); mta->type_stack.Pop(); }
    break;

  case 15:
#line 149 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(2) - (3)].typ); (yyvsp[(2) - (3)].typ)->AddParent((yyvsp[(1) - (3)].typ)); (yyvsp[(2) - (3)].typ)->type = (yyvsp[(1) - (3)].typ)->type;
	    mta->type_stack.Pop(); }
    break;

  case 16:
#line 152 "mta_parse.y"
    { /* annoying place for a comment, but.. */
            (yyval.typ) = (yyvsp[(3) - (4)].typ); (yyvsp[(3) - (4)].typ)->AddParent((yyvsp[(1) - (4)].typ)); (yyvsp[(3) - (4)].typ)->type = (yyvsp[(1) - (4)].typ)->type;
	    mta->type_stack.Pop(); }
    break;

  case 17:
#line 157 "mta_parse.y"
    {
	  if((yyvsp[(1) - (2)].typ)->parents.size < 2) {
	    cerr << "E!!: Error in predeclared type: " << (yyvsp[(1) - (2)].typ)->name << " second parent not found!"
		 << endl;
	    (yyval.typ) = (yyvsp[(1) - (2)].typ);
	  }
	  else {
	    TypeDef* td = (yyvsp[(1) - (2)].typ)->parents[1]; mta->type_stack.Pop();
	    /* TypeSpace* sp = $1->owner; */
	    /* sp->RemoveEl($1); /\* get rid of new one, cuz it is bogus *\/ */
	    /* /\* not on list that it would be placed on now.. *\/ */
	    /* if((td->owner != mta->spc) && (mta->spc->FindName(td->name) == NULL)) { */
	    /*   if(mta->verbose >= 3) */
	    /*     cerr << "M!!: transfered: " << td->name << " from: " << td->owner->name */
	    /*          << " to: " << mta->spc->name << "\n"; */
	    /*   mta->spc->Transfer(td); /\* now check for parent which is a combo of basic types *\/ */
	    /*   if((td->parents.size == 1) && (td->parents[0]->owner != mta->spc) && */
	    /*      (td->parents[0]->parents.size == 2)) { */
	    /*     /\* has one parent that is a combo-type which might be basic *\/ */
	    /*     TypeDef* par = td->parents[0]; */
	    /*     if((mta->spc_builtin.FindName(par->parents[0]->name) != NULL) && */
	    /*        (mta->spc_builtin.FindName(par->parents[1]->name) != NULL)) { */
	    /*       if(mta->verbose >= 3) */
	    /*         cerr << "M!!: transfered: " << par->name << " from: " << par->owner->name */
	    /*     	 << " to: " << mta->spc->name << "\n"; */
	    /*       TypeDef* already_there = mta->spc->FindName(par->name); */
	    /*       if(already_there == NULL) */
	    /*         mta->spc->Transfer(par); /\* move parent to this list too *\/ */
	    /*       else */
	    /*         td->parents.ReplaceLinkIdx(0, already_there); */
	    /*     } */
	    /*   } */
	    /* } */
	    (yyval.typ) = td; } }
    break;

  case 18:
#line 191 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (7)].typ); (yyval.typ)->SetType(TypeDef::FUN_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 19:
#line 194 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[(4) - (4)].typ); (yyval.typ)->SetType(TypeDef::METH_PTR);
	    mta->type_stack.Pop(); }
    break;

  case 20:
#line 199 "mta_parse.y"
    {
            TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
            (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("enum", sp, (yyval.typ)); (yyval.typ)->source_end = mta->line-1; } }
    break;

  case 23:
#line 209 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); }
    break;

  case 24:
#line 210 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 25:
#line 213 "mta_parse.y"
    {
  	    (yyval.typ) = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->SetType(TypeDef::ENUM); mta->cur_enum = (yyvsp[(2) - (2)].typ);
	    mta->SetSource((yyval.typ), false); mta->type_stack.Pop(); }
    break;

  case 26:
#line 217 "mta_parse.y"
    {
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->cur_enum = (yyval.typ);
	    mta->SetSource((yyval.typ), false); (yyval.typ)->SetType(TypeDef::ENUM); }
    break;

  case 27:
#line 224 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) mta->TypeAdded("class", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 28:
#line 232 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 29:
#line 233 "mta_parse.y"
    { SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); }
    break;

  case 30:
#line 237 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("class", sp, (yyval.typ)); mta->FixClassTypes((yyval.typ));
              (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 33:
#line 251 "mta_parse.y"
    {
	    if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	    else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 34:
#line 257 "mta_parse.y"
    {
	    (yyvsp[(1) - (2)].typ)->tokens.keep = true; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (2)].typ), true); }
    break;

  case 35:
#line 260 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
            mta->SetSource((yyvsp[(1) - (3)].typ), true); }
    break;

  case 36:
#line 263 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
	    mta->SetSource((yyvsp[(1) - (3)].typ), true); 
	  }
    break;

  case 37:
#line 270 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 38:
#line 271 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 39:
#line 274 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
            (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
	    (yyvsp[(2) - (2)].typ)->SetType(TypeDef::CLASS);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 40:
#line 279 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[(2) - (2)].typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 41:
#line 283 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->SetType(TypeDef::CLASS); 
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 42:
#line 290 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->SetType(TypeDef::STRUCT);
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 43:
#line 297 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[(1) - (1)].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); (yyval.typ)->SetType(TypeDef::UNION);
            mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 44:
#line 307 "mta_parse.y"
    {
            if((yyvsp[(1) - (1)].typ) != NULL) mta->cur_class->AddParent((yyvsp[(1) - (1)].typ)); }
    break;

  case 45:
#line 309 "mta_parse.y"
    {
            if((yyvsp[(3) - (3)].typ) != NULL) {mta->cur_class->AddParent((yyvsp[(3) - (3)].typ));
	      if(!mta->cur_class->HasOption("MULT_INHERIT"))
                mta->cur_class->opts.Add("MULT_INHERIT"); } }
    break;

  case 47:
#line 317 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 48:
#line 318 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 49:
#line 319 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 54:
#line 331 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (1)].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[(1) - (1)].typ));
	    if((yyval.typ) == (yyvsp[(1) - (1)].typ)) { mta->TypeAdded("template", sp, (yyval.typ));
	      (yyval.typ)->source_end = mta->line-1; }
	    mta->type_stack.Pop(); }
    break;

  case 57:
#line 345 "mta_parse.y"
    {
          if((yyvsp[(1) - (3)].typ)->HasOption("NO_TOKENS")) (yyvsp[(1) - (3)].typ)->tokens.keep = false;
	  else (yyvsp[(1) - (3)].typ)->tokens.keep = true; }
    break;

  case 58:
#line 351 "mta_parse.y"
    {
            mta->state = MTA::Parse_inclass; (yyvsp[(1) - (2)].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 59:
#line 354 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(2) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 60:
#line 356 "mta_parse.y"
    {
	    SETDESC((yyvsp[(1) - (3)].typ),(yyvsp[(3) - (3)].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 61:
#line 361 "mta_parse.y"
    {
	    (yyvsp[(5) - (5)].typ)->templ_pars.Reset();
	    (yyvsp[(5) - (5)].typ)->templ_pars.Duplicate(mta->cur_templ_pars);
	    (yyvsp[(5) - (5)].typ)->SetType(TypeDef::TEMPLATE); (yyval.typ) = (yyvsp[(5) - (5)].typ);
	    mta->SetSource((yyval.typ), true); }
    break;

  case 62:
#line 368 "mta_parse.y"
    { mta->cur_templ_pars.Reset(); }
    break;

  case 64:
#line 373 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 65:
#line 377 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 66:
#line 378 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 67:
#line 379 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 68:
#line 380 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(2) - (3)].typ)); (yyval.typ) = (yyvsp[(2) - (3)].typ); }
    break;

  case 69:
#line 383 "mta_parse.y"
    {
            if(mta->cur_is_trg) { /* only add reg_funs in target space */
              TypeDef* nt = new TypeDef((yyvsp[(1) - (1)].meth)->name, TypeDef::FUNCTION | TypeDef::POINTER,
                                        0,0);
              taMisc::types.Add(nt);
              nt->methods.AddUniqNameNew((yyvsp[(1) - (1)].meth)); }
            mta->meth_stack.Pop(); }
    break;

  case 70:
#line 392 "mta_parse.y"
    {
            (yyvsp[(3) - (4)].meth)->type = (yyvsp[(2) - (4)].typ); SETDESC((yyvsp[(3) - (4)].meth),(yyvsp[(4) - (4)].chr)); (yyval.meth) = (yyvsp[(3) - (4)].meth); }
    break;

  case 71:
#line 394 "mta_parse.y"
    {
            (yyvsp[(2) - (3)].meth)->type = &TA_int; SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); (yyval.meth) = (yyvsp[(2) - (3)].meth); }
    break;

  case 72:
#line 398 "mta_parse.y"
    {
            (yyvsp[(1) - (2)].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = true; }
    break;

  case 76:
#line 408 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 79:
#line 413 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 80:
#line 418 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 81:
#line 421 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm)); if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
	    mta->enum_stack.Pop(); }
    break;

  case 83:
#line 428 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 84:
#line 433 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else if((yyvsp[(1) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(1) - (3)].rval);
	  else if((yyvsp[(3) - (3)].rval) != -424242)	(yyval.rval) = (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 85:
#line 441 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 87:
#line 446 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 88:
#line 449 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 89:
#line 450 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 90:
#line 453 "mta_parse.y"
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

  case 91:
#line 462 "mta_parse.y"
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

  case 92:
#line 475 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 93:
#line 478 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 94:
#line 481 "mta_parse.y"
    { /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew((yyvsp[(1) - (2)].typ));
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 95:
#line 485 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 97:
#line 490 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 98:
#line 491 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 99:
#line 492 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 100:
#line 493 "mta_parse.y"
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

  case 102:
#line 514 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 103:
#line 520 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 104:
#line 521 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 105:
#line 522 "mta_parse.y"
    { }
    break;

  case 106:
#line 523 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
            int spsz = sp->size;
            TypeDef* nty = (yyvsp[(1) - (4)].typ)->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    (yyvsp[(2) - (4)].memb)->type = nty; }
    break;

  case 107:
#line 530 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 108:
#line 534 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(1) - (1)].memb)->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 109:
#line 538 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[(3) - (3)].memb)->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 110:
#line 544 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 111:
#line 550 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 113:
#line 556 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 114:
#line 557 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 115:
#line 558 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 116:
#line 559 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 117:
#line 560 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 118:
#line 561 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 119:
#line 562 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 120:
#line 563 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 121:
#line 564 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 122:
#line 565 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 123:
#line 566 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 124:
#line 567 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 125:
#line 568 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(5) - (5)].chr);
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef((yyvsp[(3) - (5)].meth)->name, TypeDef::FUNCTION | TypeDef::POINTER,
                                        0,0);
              taMisc::types.Add(nt);
	      nt->methods.AddUniqNameNew((yyvsp[(3) - (5)].meth)); (yyvsp[(3) - (5)].meth)->type = (yyvsp[(2) - (5)].typ);
	      mta->meth_stack.Pop();  (yyvsp[(3) - (5)].meth)->fun_argc = (yyvsp[(4) - (5)].rval); (yyvsp[(3) - (5)].meth)->arg_types.size = (yyvsp[(4) - (5)].rval);
	      (yyvsp[(3) - (5)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(3) - (5)].meth),(yyvsp[(5) - (5)].chr)); }
	    else { (yyval.meth) = NULL; mta->meth_stack.Pop(); } }
    break;

  case 126:
#line 579 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(4) - (4)].chr);
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef((yyvsp[(2) - (4)].meth)->name, TypeDef::FUNCTION | TypeDef::POINTER,
                                        0,0);
              taMisc::types.Add(nt);
	      nt->methods.AddUniqNameNew((yyvsp[(2) - (4)].meth)); (yyvsp[(2) - (4)].meth)->type = &TA_int;
	      mta->meth_stack.Pop();  (yyvsp[(2) - (4)].meth)->fun_argc = (yyvsp[(3) - (4)].rval); (yyvsp[(2) - (4)].meth)->arg_types.size = (yyvsp[(3) - (4)].rval);
	      (yyvsp[(2) - (4)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(2) - (4)].meth),(yyvsp[(4) - (4)].chr)); }
	    else { (yyval.meth) = 0; mta->meth_stack.Pop(); } }
    break;

  case 128:
#line 594 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 129:
#line 595 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 130:
#line 597 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 131:
#line 602 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 132:
#line 603 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
    break;

  case 133:
#line 604 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 134:
#line 605 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 135:
#line 606 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 136:
#line 609 "mta_parse.y"
    {
             (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(2) - (2)].rval); (yyvsp[(1) - (2)].meth)->arg_types.size = (yyvsp[(2) - (2)].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 137:
#line 616 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 138:
#line 620 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 139:
#line 621 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
    break;

  case 140:
#line 622 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 141:
#line 623 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 142:
#line 624 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 143:
#line 625 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 144:
#line 629 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 145:
#line 630 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 146:
#line 631 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
    break;

  case 147:
#line 635 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 148:
#line 636 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 149:
#line 637 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 150:
#line 640 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 151:
#line 641 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 152:
#line 644 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 153:
#line 645 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 154:
#line 648 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 155:
#line 649 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 156:
#line 650 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 157:
#line 653 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 158:
#line 655 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      String ad = (yyvsp[(2) - (2)].chr);
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 159:
#line 666 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 160:
#line 670 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 161:
#line 674 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 162:
#line 678 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 163:
#line 682 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 167:
#line 697 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 168:
#line 701 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 169:
#line 704 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 170:
#line 705 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 171:
#line 708 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 173:
#line 712 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 174:
#line 713 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 175:
#line 716 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
                                  mta->SetSource((yyval.typ), false); }
    break;

  case 177:
#line 721 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ)->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, (yyval.typ)); }
	  }
    break;

  case 179:
#line 731 "mta_parse.y"
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

  case 181:
#line 743 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(2) - (2)].typ)->GetConstType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("const", sp, (yyval.typ)); }
	  }
    break;

  case 183:
#line 752 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 184:
#line 753 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 185:
#line 754 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 186:
#line 755 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 187:
#line 756 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 188:
#line 760 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 189:
#line 764 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 190:
#line 765 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 191:
#line 766 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 193:
#line 768 "mta_parse.y"
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

  case 194:
#line 785 "mta_parse.y"
    { /* this template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
	      yyerror("Template syntax error"); YYERROR; }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 196:
#line 792 "mta_parse.y"
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

  case 198:
#line 805 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 199:
#line 810 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 200:
#line 811 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 201:
#line 812 "mta_parse.y"
    { (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 203:
#line 817 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 210:
#line 833 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 211:
#line 837 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 212:
#line 841 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;

  case 213:
#line 845 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 2965 "y.tab.c"
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


#line 848 "mta_parse.y"


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

