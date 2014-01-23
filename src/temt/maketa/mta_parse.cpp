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
     MP_VIRTUAL = 286,
     MP_OVERRIDE = 287
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
#define MP_OVERRIDE 287




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
#line 194 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 207 "y.tab.c"

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
#define YYLAST   791

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  98
/* YYNRULES -- Number of rules.  */
#define YYNRULES  297
/* YYNRULES -- Number of states.  */
#define YYNSTATES  499

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   287

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    42,     2,     2,     2,    48,     2,
      34,    35,    33,    43,    39,    44,    47,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    38,    49,
      41,     2,    40,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    37,    45,    36,    46,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32
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
     654,   657,   659,   661,   664,   666,   669,   673,   676,   680,
     685,   688,   692,   694,   697,   701,   704,   708,   713,   715,
     717,   720,   723,   727,   729,   732,   734,   738,   743,   747,
     749,   752,   754,   756,   759,   763,   770,   772,   775,   777,
     779,   782,   786,   789,   792,   795,   797,   799,   802,   804,
     806,   808,   810,   813,   815,   818,   820,   823,   825,   828,
     830,   835,   837,   841,   845,   849,   853,   856,   859,   862,
     864,   869,   874,   877,   880,   883,   886,   888,   891,   893,
     895,   899,   901,   903,   906,   909,   912,   917,   919,   922,
     925,   928,   930,   932,   934,   936,   938,   940,   942,   945,
     948,   950,   952,   954,   956,   958,   960,   962
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      51,     0,    -1,    -1,    51,    52,    -1,    51,    57,    -1,
      51,    61,    -1,    51,    63,    -1,    51,    73,    -1,    51,
      75,    -1,    51,    84,    -1,    51,    87,    -1,    51,    88,
      -1,    51,     1,    -1,    53,    -1,    54,    -1,    54,    18,
      -1,    12,    55,    -1,    12,    65,   139,   142,    -1,    12,
       4,    55,    -1,    12,     4,    65,   139,   142,    -1,    56,
     125,   142,    -1,    56,   125,    16,   142,    -1,   128,    18,
     125,   142,    -1,    56,   142,    -1,    56,    34,    33,   125,
      35,   114,   142,    -1,    56,    20,    33,   125,    -1,   125,
     125,   142,    -1,   125,   125,    16,   142,    -1,   125,    56,
     142,    -1,    56,    56,   142,    -1,   128,    -1,   133,    -1,
      58,    -1,    59,    91,    36,   142,    -1,    60,    37,    -1,
      60,    18,    37,    -1,    60,    37,    18,    -1,     7,   125,
      -1,     7,    -1,    62,    -1,    68,   142,    -1,    68,   142,
      18,    -1,    64,    -1,    65,   142,    -1,    65,   140,   142,
      -1,    66,    98,    36,    -1,    66,    36,    -1,    67,    37,
      -1,    67,    18,    37,    -1,    67,    37,    18,    -1,    68,
      -1,    68,    38,    69,    -1,   146,   125,    -1,   146,   128,
      -1,   146,    -1,   144,    -1,   144,   125,    -1,   144,   128,
      -1,   145,    -1,   145,   125,    -1,   145,   128,    -1,    70,
      -1,    69,    39,    70,    -1,   128,    -1,    71,   128,    -1,
      17,    -1,    17,   135,   136,    40,    -1,    71,    17,    -1,
      72,    -1,    71,    72,    -1,   143,    -1,    31,    -1,    74,
      -1,    79,   142,    -1,    79,   142,    18,    -1,    76,    -1,
      77,   142,    -1,    77,   140,   142,    -1,    80,   108,    -1,
      77,   142,    18,    -1,    77,   140,   142,    18,    -1,    80,
     108,    18,    -1,    78,    98,    36,    -1,    78,    36,    -1,
      79,   142,    -1,    79,    37,    -1,    79,    18,    37,    -1,
      79,    37,    18,    -1,   147,    81,    82,    40,    67,    -1,
     147,    81,    40,    67,    -1,   147,    81,    40,     8,    -1,
     147,    81,    82,    40,     8,    -1,    41,    -1,    83,    -1,
      82,    39,    83,    -1,     3,   125,    -1,     4,   125,    -1,
       4,    -1,    17,    17,    -1,   128,    -1,   128,   125,    -1,
       3,   125,    21,   128,    -1,     4,   125,    21,   128,    -1,
     128,   125,    21,   128,    -1,   128,    21,   128,    -1,     1,
      -1,    85,    -1,    30,   127,    86,   111,    -1,    30,    86,
     111,    -1,   110,   114,    -1,    89,    90,   142,    -1,    89,
      90,    37,    -1,    23,    -1,    17,    -1,    90,    17,    -1,
      90,    20,    17,    -1,    90,    34,    -1,    90,    35,    -1,
      90,    42,    -1,    92,    -1,    91,    92,    -1,    93,    -1,
      93,    18,    -1,    94,    -1,    94,    39,    -1,     1,    -1,
      97,    -1,    97,    21,    95,    -1,    96,    -1,    95,    43,
      96,    -1,    95,    44,    96,    -1,    95,    45,    96,    -1,
      17,    -1,    14,    -1,    15,    -1,    17,    -1,    99,    -1,
      98,    99,    -1,   100,    -1,   106,    -1,    58,    -1,    54,
      -1,    65,   142,    -1,     1,    -1,   101,    -1,    24,    38,
      -1,    25,    38,    -1,    26,    38,    -1,    18,    -1,   102,
      -1,     9,   102,    -1,   126,   103,   142,    -1,   126,   104,
      21,   142,    -1,   103,   142,    -1,   126,   104,    16,   142,
      -1,   126,   105,   114,   142,    -1,   104,    -1,   103,    39,
     104,    -1,    17,    -1,    34,    33,    17,    35,    -1,   107,
      -1,   122,   115,   111,    -1,     8,   122,   115,   111,    -1,
      31,   122,   115,   111,    -1,   122,   115,   119,   111,    -1,
       8,   122,   115,   119,   111,    -1,    31,   122,   115,   119,
     111,    -1,    46,   122,   115,   111,    -1,     8,    46,   122,
     115,   111,    -1,    31,    46,   122,   115,   111,    -1,    28,
     127,    27,   114,   111,    -1,    28,    27,   114,   111,    -1,
      28,   127,   142,    -1,    28,     3,   127,   142,    -1,    28,
       3,    17,   142,    -1,    28,   127,   110,   114,   111,    -1,
      28,   110,   114,   111,    -1,    22,    17,    20,    27,   142,
      -1,    22,    17,    20,    17,   142,    -1,    22,    13,    20,
      27,   142,    -1,    22,    13,    20,    17,   142,    -1,   108,
      -1,     9,   108,    -1,    31,   108,    -1,     8,    31,   108,
      -1,   126,   109,   111,    -1,   109,   111,    -1,   126,    27,
     114,   111,    -1,   126,    27,    34,    35,   114,   111,    -1,
      27,   114,   111,    -1,   110,   114,    -1,    17,    -1,   142,
      -1,    32,   142,    -1,   112,    -1,   113,   142,    -1,    11,
      32,   142,    -1,    18,   142,    -1,    18,    32,   142,    -1,
      18,    11,    32,   142,    -1,    18,   112,    -1,    18,   113,
     142,    -1,    15,    -1,    11,    15,    -1,    11,    18,    15,
      -1,    32,    15,    -1,    11,    32,    15,    -1,    11,    32,
      18,    15,    -1,    21,    -1,    11,    -1,    11,    21,    -1,
      34,    35,    -1,    34,   116,    35,    -1,    35,    -1,   116,
      35,    -1,   117,    -1,   116,    39,   117,    -1,   116,    47,
      47,    47,    -1,    47,    47,    47,    -1,   118,    -1,   118,
      21,    -1,     1,    -1,   128,    -1,   128,    17,    -1,   128,
      17,    16,    -1,   128,    34,    33,    17,    35,   114,    -1,
      17,    -1,   123,   120,    -1,   121,    -1,    18,    -1,   120,
     121,    -1,   120,    39,   121,    -1,    13,    19,    -1,    17,
      19,    -1,    29,    34,    -1,    38,    -1,    33,    -1,   124,
      33,    -1,    17,    -1,   127,    -1,   128,    -1,     8,    -1,
       8,   128,    -1,   129,    -1,   129,    48,    -1,   130,    -1,
     130,   124,    -1,   131,    -1,    11,   131,    -1,   132,    -1,
       4,   132,    20,    17,    -1,   134,    -1,    13,    20,    17,
      -1,    29,    20,    17,    -1,    13,    20,    13,    -1,    29,
      20,    13,    -1,   141,    17,    -1,   141,    13,    -1,    20,
      13,    -1,    29,    -1,    13,   135,   136,    40,    -1,    29,
     135,   136,    40,    -1,   144,   134,    -1,   144,   125,    -1,
     145,   134,    -1,   145,   125,    -1,    13,    -1,   134,    13,
      -1,    41,    -1,   137,    -1,   136,    39,   137,    -1,   131,
      -1,    17,    -1,    17,   124,    -1,    17,    48,    -1,    17,
      16,    -1,    17,    34,    17,    35,    -1,    14,    -1,   138,
      17,    -1,   138,    13,    -1,    17,   138,    -1,     1,    -1,
      11,    -1,     8,    -1,    17,    -1,    13,    -1,    29,    -1,
      17,    -1,    17,    16,    -1,    17,    20,    -1,    49,    -1,
      24,    -1,    25,    -1,    26,    -1,     5,    -1,     6,    -1,
       3,    -1,    10,    -1
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
     499,   505,   511,   515,   516,   517,   518,   519,   520,   523,
     524,   527,   528,   531,   532,   533,   538,   541,   549,   550,
     553,   556,   562,   566,   567,   571,   574,   575,   578,   586,
     598,   604,   608,   618,   622,   623,   624,   625,   626,   645,
     646,   652,   653,   654,   655,   662,   666,   670,   676,   682,
     687,   688,   689,   690,   691,   692,   693,   694,   695,   696,
     697,   698,   699,   700,   701,   702,   715,   728,   729,   730,
     731,   735,   736,   737,   739,   744,   745,   746,   747,   748,
     751,   760,   764,   765,   766,   767,   768,   769,   770,   771,
     772,   773,   777,   778,   779,   780,   781,   782,   786,   787,
     788,   791,   792,   795,   796,   799,   800,   801,   802,   805,
     807,   815,   819,   823,   827,   831,   835,   841,   845,   846,
     847,   848,   851,   852,   856,   860,   863,   864,   867,   871,
     875,   876,   877,   880,   881,   890,   891,   902,   903,   911,
     912,   915,   916,   923,   930,   931,   932,   940,   944,   945,
     946,   961,   969,   971,   972,   974,   977,   978,   990,   995,
     996,  1001,  1003,  1005,  1007,  1009,  1011,  1013,  1016,  1018,
    1020,  1022,  1026,  1027,  1030,  1031,  1032,  1035,  1036,  1039,
    1042,  1045,  1046,  1047,  1051,  1058,  1065,  1072
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
  "MP_VIRTUAL", "MP_OVERRIDE", "'*'", "'('", "')'", "'}'", "'{'", "':'",
  "','", "'>'", "'<'", "'\"'", "'+'", "'-'", "'|'", "'~'", "'.'", "'&'",
  "';'", "$accept", "list", "typedefn", "typedefns", "typedsub", "defn",
  "tdtype", "enumdefn", "enumdsub", "enumname", "enumnm", "classdecl",
  "classdecls", "classdefn", "classdefns", "classdsub", "classname",
  "classhead", "classnm", "classinh", "classpar", "classptyp", "classpmod",
  "templdecl", "templdecls", "templdefn", "templdefns", "templdsub",
  "templname", "templhead", "templfun", "templopen", "templpars",
  "templpar", "fundecl", "funnm", "regfundefn", "usenamespc", "namespc",
  "namespcword", "namespcnms", "enums", "enumline", "enumitm", "enumitms",
  "enummath", "enummathitm", "enmitmname", "membs", "membline", "membdefn",
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
     285,   286,   287,    42,    40,    41,   125,   123,    58,    44,
      62,    60,    34,    43,    45,   124,   126,    46,    38,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    50,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    52,    53,    53,    54,    54,    54,    54,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      56,    56,    57,    58,    59,    59,    59,    60,    60,    61,
      62,    62,    63,    64,    64,    65,    65,    66,    66,    66,
      67,    67,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    69,    69,    70,    70,    70,    70,    70,    71,    71,
      72,    72,    73,    74,    74,    75,    76,    76,    76,    76,
      76,    76,    77,    77,    77,    78,    78,    78,    79,    79,
      80,    80,    81,    82,    82,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    84,    85,    85,    86,
      87,    88,    89,    90,    90,    90,    90,    90,    90,    91,
      91,    92,    92,    93,    93,    93,    94,    94,    95,    95,
      95,    95,    96,    96,    96,    97,    98,    98,    99,    99,
      99,    99,    99,    99,   100,   100,   100,   100,   100,   101,
     101,   102,   102,   102,   102,   102,   103,   103,   104,   105,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   107,   107,   107,   107,   108,   108,   108,   108,   108,
     109,   110,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   112,   112,   112,   112,   112,   112,   113,   113,
     113,   114,   114,   115,   115,   116,   116,   116,   116,   117,
     117,   117,   118,   118,   118,   118,   118,   119,   120,   120,
     120,   120,   121,   121,   122,   123,   124,   124,   125,   126,
     127,   127,   127,   128,   128,   129,   129,   130,   130,   131,
     131,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   133,   133,   133,   133,   134,   134,   135,   136,
     136,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   138,   138,   139,   139,   139,   140,   140,   141,
     142,   143,   143,   143,   144,   145,   146,   147
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
       2,     1,     1,     2,     1,     2,     3,     2,     3,     4,
       2,     3,     1,     2,     3,     2,     3,     4,     1,     1,
       2,     2,     3,     1,     2,     1,     3,     4,     3,     1,
       2,     1,     1,     2,     3,     6,     1,     2,     1,     1,
       2,     3,     2,     2,     2,     1,     1,     2,     1,     1,
       1,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       4,     1,     3,     3,     3,     3,     2,     2,     2,     1,
       4,     4,     2,     2,     2,     2,     1,     2,     1,     1,
       3,     1,     1,     2,     2,     2,     4,     1,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     1,    12,   296,   294,   295,    38,   297,     0,
     112,     0,     3,    13,    14,     4,    32,     0,     0,     5,
      39,     6,    42,     0,     0,     0,    50,     7,    72,     8,
      75,     0,     0,     0,     0,     9,   106,    10,    11,     0,
      55,    58,    54,     0,   238,    37,     0,     0,   266,   238,
       0,   259,    16,     0,     0,    50,     0,    30,   243,   245,
     247,   249,    31,   251,     0,    55,    58,     0,   241,   191,
       0,     0,     0,   240,    15,   125,   135,     0,   119,   121,
     123,   126,     0,    34,   287,   290,     0,    43,   143,   241,
       0,   158,   148,     0,     0,     0,     0,     0,     0,   259,
       0,    46,     0,   141,   140,     0,     0,   136,   138,   144,
     149,     0,   156,   139,   160,   181,     0,     0,     0,     0,
     239,     0,    47,     0,    40,     0,    76,    83,     0,     0,
      85,    73,    78,     0,   113,     0,    56,    57,    59,    60,
      52,    53,    92,     0,    18,     0,   249,     0,   248,     0,
     268,     0,   289,   258,     0,     0,     0,     0,     0,     0,
      30,    23,     0,     0,   285,   284,   286,     0,     0,     0,
       0,   244,   236,   246,   267,   257,   256,   263,   262,   265,
     264,     0,   242,   209,   202,     0,   208,     0,   108,   194,
       0,   192,     0,   109,   191,     0,     0,   120,   122,   124,
       0,    35,    36,   288,    44,     0,     0,     0,   150,   182,
       0,     0,   145,   146,   147,     0,     0,     0,     0,     0,
     234,     0,   183,     0,     0,     0,   142,    45,   137,     0,
     153,   186,   190,   221,   226,   213,     0,     0,     0,   215,
     219,   222,   158,     0,     0,     0,   156,     0,     0,    48,
      49,    65,   291,   292,   293,    71,    51,    61,     0,    68,
      63,    70,    41,    77,    79,    82,    86,    87,    74,    81,
     114,     0,   116,   117,   111,   118,   110,   105,     0,    97,
       0,     0,     0,    93,    99,     0,     0,   254,   252,   281,
     283,   282,   277,   272,   271,     0,   269,     0,   255,   253,
       0,     0,     0,    29,     0,    20,   266,   263,   262,   265,
     264,    17,    28,     0,    26,     0,   237,   203,     0,   210,
       0,   209,     0,   200,     0,   197,   205,   193,   195,   211,
       0,   107,    33,   133,   134,   132,   127,   128,   184,     0,
       0,     0,     0,   189,     0,     0,     0,     0,     0,     0,
     172,     0,     0,     0,   158,   157,     0,   235,   161,     0,
       0,   214,     0,     0,   220,   223,     0,     0,     0,     0,
     151,     0,     0,     0,   185,     0,     0,    67,    69,    64,
      80,   115,    95,    96,    98,    90,    89,     0,     0,     0,
     100,    19,   250,   275,     0,   274,   273,   280,     0,   260,
     279,   278,   261,    25,     0,    21,    27,    22,   204,   206,
       0,   196,     0,   198,   201,   212,     0,     0,     0,     0,
     162,     0,     0,     0,     0,     0,   174,   173,   171,   176,
       0,     0,     0,   163,     0,   167,   218,   164,     0,     0,
     229,   227,   228,   216,     0,   224,     0,   211,   187,     0,
     154,   152,   155,     0,    62,     0,     0,    94,    91,    88,
     104,     0,     0,   270,     0,   207,   199,   129,   130,   131,
     168,   165,   180,   179,   178,   177,   170,   175,   169,   166,
     232,   233,     0,   230,   217,     0,     0,   159,    66,   101,
     102,   103,   276,     0,   231,     0,   188,    24,   225
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    12,    13,   103,    52,    53,    15,   104,    17,
      18,    19,    20,    21,    22,   105,    24,    25,    55,   256,
     257,   258,   259,    27,    28,    29,    30,    31,    32,    33,
      34,   143,   282,   283,    35,    36,    70,    37,    38,    39,
     135,    77,    78,    79,    80,   336,   337,    81,   106,   107,
     108,   109,   110,   111,   112,   247,   113,   114,   115,   116,
     117,   188,   189,   190,   193,   237,   238,   239,   240,   359,
     441,   442,   118,   360,   173,    56,   119,   120,    73,    58,
      59,    60,    61,    62,    63,   155,   295,   296,   297,   167,
      86,    64,   191,   261,    40,    41,    42,    43
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -399
static const yytype_int16 yypact[] =
{
    -399,   591,  -399,  -399,  -399,  -399,  -399,    31,  -399,   675,
    -399,   751,  -399,  -399,   129,  -399,  -399,    88,   140,  -399,
    -399,  -399,  -399,    22,   417,   233,    52,  -399,  -399,  -399,
    -399,    22,   468,   145,   713,  -399,  -399,  -399,  -399,   114,
     314,   314,   314,   149,  -399,  -399,   694,   243,    72,   161,
     186,   156,  -399,   364,   213,   174,   732,   210,   200,   201,
    -399,  -399,  -399,   281,    34,   314,   314,   390,   761,   161,
     119,   269,   292,  -399,  -399,  -399,  -399,    23,  -399,   293,
     305,   311,   309,   334,   341,  -399,   330,  -399,  -399,   189,
     740,   123,  -399,   320,   322,   367,   379,   269,   705,   151,
     278,  -399,   407,  -399,  -399,   330,   504,  -399,  -399,  -399,
    -399,   220,  -399,  -399,  -399,  -399,   119,   269,   194,    70,
    -399,   401,   422,   636,   429,   330,   436,  -399,   540,   423,
     444,    76,   447,   247,  -399,   553,  -399,  -399,  -399,  -399,
    -399,  -399,  -399,   325,  -399,   213,   448,   161,  -399,   342,
    -399,   605,  -399,  -399,   363,   605,   128,   426,   330,    25,
    -399,  -399,   369,   369,  -399,  -399,  -399,   330,   330,    29,
      31,  -399,  -399,   433,  -399,  -399,  -399,   228,   275,   302,
     275,   448,  -399,   346,  -399,   159,  -399,     8,  -399,  -399,
     330,  -399,   232,  -399,  -399,   119,   330,  -399,  -399,  -399,
     441,  -399,  -399,  -399,  -399,   713,   407,   194,  -399,  -399,
     467,   469,  -399,  -399,  -399,   119,   762,   269,   269,    50,
    -399,   407,  -399,   194,   457,   194,  -399,  -399,  -399,   485,
    -399,  -399,  -399,  -399,   161,  -399,   459,   100,   238,  -399,
     502,   125,   491,   493,   501,   220,   234,   269,   119,  -399,
    -399,   163,  -399,  -399,  -399,  -399,   497,  -399,   646,  -399,
    -399,  -399,  -399,   519,  -399,  -399,  -399,  -399,  -399,  -399,
    -399,   521,  -399,  -399,  -399,  -399,  -399,  -399,    31,   564,
     155,   403,   348,  -399,   373,   330,   522,  -399,  -399,  -399,
    -399,  -399,  -399,    75,  -399,   356,  -399,   399,  -399,  -399,
     393,    31,    31,  -399,   330,  -399,  -399,  -399,   281,  -399,
     281,  -399,  -399,   330,  -399,   330,  -399,  -399,   527,  -399,
       7,   446,     8,  -399,   330,  -399,  -399,  -399,  -399,  -399,
     327,  -399,  -399,  -399,  -399,  -399,   439,  -399,  -399,   194,
     100,   283,   324,  -399,    11,   330,   119,   119,   269,   269,
    -399,   194,   100,   119,  -399,  -399,   507,  -399,  -399,   119,
      37,  -399,   626,   508,  -399,   543,   523,   267,   119,   544,
    -399,   330,   330,   330,  -399,   605,   636,   161,  -399,  -399,
    -399,  -399,   542,   551,  -399,  -399,  -399,   625,   495,   761,
     554,  -399,  -399,  -399,   557,  -399,   433,  -399,   605,  -399,
    -399,  -399,  -399,  -399,   545,  -399,  -399,  -399,  -399,  -399,
     563,  -399,     7,  -399,  -399,  -399,   441,   441,   441,   119,
    -399,   119,   330,   330,   330,   330,  -399,  -399,  -399,  -399,
     119,   119,   119,  -399,   119,  -399,  -399,  -399,   560,   566,
    -399,    90,  -399,  -399,   535,  -399,   572,   269,  -399,   548,
    -399,  -399,  -399,   410,  -399,   761,   761,  -399,  -399,  -399,
    -399,   761,   565,  -399,   269,  -399,  -399,  -399,  -399,  -399,
    -399,  -399,  -399,  -399,  -399,  -399,  -399,  -399,  -399,  -399,
    -399,  -399,   414,  -399,  -399,   569,   119,  -399,  -399,  -399,
    -399,  -399,  -399,   330,  -399,   269,  -399,  -399,  -399
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -399,  -399,  -399,  -399,   598,   559,   157,  -399,   606,  -399,
    -399,  -399,  -399,  -399,  -399,    26,  -399,  -264,   607,  -399,
     235,  -399,   352,  -399,  -399,  -399,  -399,  -399,  -399,  -399,
    -399,  -399,  -399,   225,  -399,  -399,   552,  -399,  -399,  -399,
    -399,  -399,   538,  -399,  -399,  -399,   102,  -399,   585,   -53,
    -399,  -399,   530,   512,  -101,  -399,  -399,  -399,   -21,    68,
      -2,  -115,   438,   450,   -91,  -191,  -178,   270,  -399,  -113,
    -399,  -398,   -56,  -399,   340,   113,   -19,     0,    -4,  -399,
    -399,   -45,   -39,  -399,   -46,   -36,  -151,   246,   355,   496,
     620,  -399,   -23,  -399,    12,    20,  -399,  -399
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -252
static const yytype_int16 yytable[] =
{
      87,   231,   148,   124,   300,    57,   215,   146,   126,    71,
     131,    72,   151,   132,   330,   133,   340,   386,   246,   178,
     180,    65,   409,   326,    75,   410,   232,    23,   181,    66,
     161,   152,   352,   207,   353,    54,   137,   139,   141,    84,
      76,   304,    57,   483,   223,   313,   225,   175,    44,   160,
     438,   176,   160,   228,   439,   440,    85,    85,    65,   196,
      85,   137,   139,   204,   182,   162,    66,   194,   162,   209,
      71,    85,   145,   163,    85,   228,   163,   348,    85,   222,
     331,   133,   226,   290,   494,   182,   291,   242,   230,    75,
     123,   393,   149,   -84,   268,   152,   218,   243,   219,    85,
     343,    85,   263,   438,   244,    76,   294,   439,   172,   394,
     294,   183,   276,   150,   241,   184,   308,   310,   185,   260,
      45,   186,   358,   395,   459,   -84,   346,   347,   355,   482,
     183,   134,   187,   374,   184,   303,   305,   185,   357,   284,
     186,   153,   365,   152,   311,   312,   314,    74,   419,    85,
     339,   187,   368,   136,   138,   140,   373,  -191,    82,   366,
     432,   301,   325,   129,   327,   351,   159,   328,    85,   169,
     321,   154,   384,   332,   184,   152,   154,    83,   177,   179,
     186,   152,   130,   152,   338,   220,   133,   248,   241,   330,
     142,   322,   150,    67,    85,   233,   350,   150,    67,   153,
      47,   248,    48,   241,   150,    47,   147,    48,    85,    50,
     158,   234,   123,   168,    50,   375,   345,   349,    99,   241,
     205,   241,   370,    51,   453,   420,   164,   421,   170,   235,
     165,   428,   429,   233,   172,   206,    67,   433,   435,   434,
     181,   236,   166,    47,   437,    48,   -56,    67,   171,   234,
     371,   121,    50,   448,   379,   372,    48,   430,   431,   229,
     147,    51,   391,    50,   194,   -56,   -56,   329,   233,    85,
     122,    67,    51,   361,   243,   307,   309,   362,    47,   236,
      48,   405,    67,   315,   234,   363,    68,    50,   174,    47,
     406,    48,   407,  -251,   174,    69,    51,   411,    50,   413,
     422,   414,   447,   192,   470,    97,   471,    99,  -251,   194,
     423,   198,  -251,  -251,   236,   476,   477,   478,    67,   479,
     -59,   426,   427,  -251,   221,    47,   277,    48,   278,   279,
     294,    49,   200,   210,    50,   241,    47,   211,    48,   -59,
     -59,   424,   280,    51,   199,    50,   201,   241,   450,   451,
     452,   425,   202,   294,    51,   287,   486,   203,   241,   288,
     212,   317,   415,   241,   318,   281,   362,   319,    67,     5,
       6,   496,   260,   493,   363,    47,   298,    48,   320,    85,
     299,    49,   306,   284,   156,   460,    44,   387,   388,   466,
      44,   382,   383,    51,   389,   398,   399,   390,   157,   472,
     473,   474,   475,    48,   498,   213,     4,   147,     5,     6,
      50,   385,   400,    85,   403,   404,   401,   214,    88,    51,
       4,    67,     5,     6,     7,    89,    90,   438,    47,     9,
      48,   439,   398,   402,    91,    92,   224,    50,   249,    93,
     250,    94,    95,    96,    97,    98,    99,   262,   100,   398,
     488,   489,   490,   101,   264,   333,   334,   491,   335,   302,
     266,   317,   267,   102,   318,   269,   316,   319,   286,    88,
     497,     4,    67,     5,     6,     7,    89,    90,   412,    47,
       9,    48,   416,   417,   418,    91,    92,   341,    50,   342,
      93,   220,    94,    95,    96,    97,    98,    99,     4,   100,
       5,     6,   354,   458,   127,    88,   356,     4,    67,     5,
       6,     7,    89,    90,   102,    47,     9,    48,   467,   468,
     469,    91,    92,   364,    50,  -191,    93,   367,    94,    95,
      96,    97,    98,    99,   369,   100,   376,   380,   381,   392,
     227,    88,   408,     4,    67,     5,     6,     7,    89,    90,
     102,    47,     9,    48,   436,   444,   446,    91,    92,   445,
      50,   449,    93,   455,    94,    95,    96,    97,    98,    99,
     270,   100,   456,   271,   462,   461,   265,    48,   465,   480,
     464,    49,   484,   487,    50,   481,   102,   272,   273,   485,
     274,     2,     3,    51,     4,   275,     5,     6,     7,    14,
     492,     8,    85,     9,   495,   144,   289,    16,    26,    67,
     378,   454,   457,   290,    10,   197,   291,   128,    48,   292,
     208,    11,   293,   323,   195,    50,   277,   233,   278,   279,
      67,   245,   443,   396,    51,   324,    47,    47,    48,    48,
      67,   285,   280,   234,   463,    50,    50,    47,   397,    48,
      67,   125,     0,   251,    51,    51,    50,    47,     0,    48,
     252,   253,   254,   377,     0,    51,    50,   255,     0,     0,
     252,   253,   254,     0,     0,    51,     0,   255,     4,    46,
       5,     6,     0,     0,     0,     0,    47,     0,    48,     0,
       0,     0,    49,     0,     0,    50,     0,     4,    67,     5,
       6,     0,     0,     0,    51,    47,     0,    48,   216,    67,
       0,    49,     0,    68,    50,     0,    47,    67,    48,     0,
       0,    68,    69,    51,    47,    50,    48,     0,     0,     0,
      69,     0,   217,    50,    51,     0,    67,     5,     6,     0,
      97,     0,    51,    47,    67,    48,     0,     0,    68,    49,
       0,    47,    50,    48,     0,    67,     0,    91,     0,    68,
      50,    51,    47,     0,    48,    67,    67,    97,    69,    51,
      68,    50,    47,    47,    48,    48,     0,     0,   147,   344,
      51,    50,    50,     0,     0,     0,     0,     0,     0,     0,
      51,    51
};

static const yytype_int16 yycheck[] =
{
      23,   116,    47,    26,   155,     9,    97,    46,    31,    11,
      33,    11,    48,    34,   192,    34,   207,   281,   119,    65,
      66,     9,    15,    15,     1,    18,   117,     1,    67,     9,
      53,    20,   223,    89,   225,     9,    40,    41,    42,    17,
      17,    16,    46,   441,   100,    16,   102,    13,    17,    53,
      13,    17,    56,   106,    17,    18,    49,    49,    46,    36,
      49,    65,    66,    86,    68,    53,    46,    17,    56,    90,
      72,    49,    46,    53,    49,   128,    56,    27,    49,   100,
     195,   100,   105,     8,   482,    89,    11,    17,   111,     1,
      38,    16,    20,    17,    18,    20,    98,    27,    98,    49,
     215,    49,   125,    13,    34,    17,   151,    17,    33,    34,
     155,    11,   135,    41,   118,    15,   162,   163,    18,   123,
       7,    21,   237,    48,   388,    49,   217,   218,   229,    39,
      11,    17,    32,   248,    15,   158,   159,    18,    38,   143,
      21,    13,    17,    20,   167,   168,   169,    18,   339,    49,
     206,    32,   243,    40,    41,    42,   247,    34,    18,    34,
     351,    33,   185,    18,   187,   221,    53,   190,    49,    56,
      11,    20,    17,   196,    15,    20,    20,    37,    65,    66,
      21,    20,    37,    20,   205,    34,   205,   119,   192,   367,
      41,    32,    41,     4,    49,     1,   219,    41,     4,    13,
      11,   133,    13,   207,    41,    11,    17,    13,    49,    20,
      53,    17,    38,    56,    20,   251,   216,   219,    29,   223,
      31,   225,   245,    29,   375,   340,    13,   340,    18,    35,
      17,   346,   347,     1,    33,    46,     4,   352,   353,   352,
     279,    47,    29,    11,   359,    13,    18,     4,    48,    17,
      16,    18,    20,   368,   258,    21,    13,   348,   349,    39,
      17,    29,   285,    20,    17,    37,    38,    35,     1,    49,
      37,     4,    29,    35,    27,   162,   163,    39,    11,    47,
      13,   304,     4,   170,    17,    47,     8,    20,    13,    11,
     313,    13,   315,    18,    13,    17,    29,   320,    20,   322,
      17,   324,    35,    34,   419,    27,   421,    29,    33,    17,
      27,    18,    37,    38,    47,   430,   431,   432,     4,   434,
      18,   344,   345,    48,    46,    11,     1,    13,     3,     4,
     375,    17,    21,    13,    20,   339,    11,    17,    13,    37,
      38,    17,    17,    29,    39,    20,    37,   351,   371,   372,
     373,    27,    18,   398,    29,    13,   447,    16,   362,    17,
      38,    15,    35,   367,    18,    40,    39,    21,     4,     5,
       6,   486,   376,   464,    47,    11,    13,    13,    32,    49,
      17,    17,    13,   387,    20,   389,    17,    39,    40,   412,
      17,   278,   279,    29,    21,    39,    40,   284,    34,   422,
     423,   424,   425,    13,   495,    38,     3,    17,     5,     6,
      20,     8,    13,    49,   301,   302,    17,    38,     1,    29,
       3,     4,     5,     6,     7,     8,     9,    13,    11,    12,
      13,    17,    39,    40,    17,    18,    29,    20,    37,    22,
      18,    24,    25,    26,    27,    28,    29,    18,    31,    39,
      40,   455,   456,    36,    18,    14,    15,   461,    17,    33,
      37,    15,    18,    46,    18,    18,    33,    21,    20,     1,
     493,     3,     4,     5,     6,     7,     8,     9,    32,    11,
      12,    13,    43,    44,    45,    17,    18,    20,    20,    20,
      22,    34,    24,    25,    26,    27,    28,    29,     3,    31,
       5,     6,    17,     8,    36,     1,    47,     3,     4,     5,
       6,     7,     8,     9,    46,    11,    12,    13,   416,   417,
     418,    17,    18,    21,    20,    34,    22,    34,    24,    25,
      26,    27,    28,    29,    33,    31,    39,    18,    17,    17,
      36,     1,    15,     3,     4,     5,     6,     7,     8,     9,
      46,    11,    12,    13,    47,    47,    33,    17,    18,    16,
      20,    17,    22,    21,    24,    25,    26,    27,    28,    29,
      17,    31,    21,    20,    17,    21,    36,    13,    15,    19,
      35,    17,    47,    35,    20,    19,    46,    34,    35,    17,
      37,     0,     1,    29,     3,    42,     5,     6,     7,     1,
      35,    10,    49,    12,    35,    46,     1,     1,     1,     4,
     258,   376,   387,     8,    23,    77,    11,    32,    13,    14,
      90,    30,    17,   185,    72,    20,     1,     1,     3,     4,
       4,   119,   362,   293,    29,   185,    11,    11,    13,    13,
       4,   145,    17,    17,   398,    20,    20,    11,   293,    13,
       4,    31,    -1,    17,    29,    29,    20,    11,    -1,    13,
      24,    25,    26,    17,    -1,    29,    20,    31,    -1,    -1,
      24,    25,    26,    -1,    -1,    29,    -1,    31,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    11,    -1,    13,    -1,
      -1,    -1,    17,    -1,    -1,    20,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    29,    11,    -1,    13,     3,     4,
      -1,    17,    -1,     8,    20,    -1,    11,     4,    13,    -1,
      -1,     8,    17,    29,    11,    20,    13,    -1,    -1,    -1,
      17,    -1,    27,    20,    29,    -1,     4,     5,     6,    -1,
      27,    -1,    29,    11,     4,    13,    -1,    -1,     8,    17,
      -1,    11,    20,    13,    -1,     4,    -1,    17,    -1,     8,
      20,    29,    11,    -1,    13,     4,     4,    27,    17,    29,
       8,    20,    11,    11,    13,    13,    -1,    -1,    17,    17,
      29,    20,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    29
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    51,     0,     1,     3,     5,     6,     7,    10,    12,
      23,    30,    52,    53,    54,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    73,    74,    75,
      76,    77,    78,    79,    80,    84,    85,    87,    88,    89,
     144,   145,   146,   147,    17,   125,     4,    11,    13,    17,
      20,    29,    55,    56,    65,    68,   125,   128,   129,   130,
     131,   132,   133,   134,   141,   144,   145,     4,     8,    17,
      86,   110,   127,   128,    18,     1,    17,    91,    92,    93,
      94,    97,    18,    37,    17,    49,   140,   142,     1,     8,
       9,    17,    18,    22,    24,    25,    26,    27,    28,    29,
      31,    36,    46,    54,    58,    65,    98,    99,   100,   101,
     102,   103,   104,   106,   107,   108,   109,   110,   122,   126,
     127,    18,    37,    38,   142,   140,   142,    36,    98,    18,
      37,   142,   108,   126,    17,    90,   125,   128,   125,   128,
     125,   128,    41,    81,    55,    65,   132,    17,   131,    20,
      41,   135,    20,    13,    20,   135,    20,    34,    56,   125,
     128,   142,   144,   145,    13,    17,    29,   139,    56,   125,
      18,    48,    33,   124,    13,    13,    17,   125,   134,   125,
     134,   132,   128,    11,    15,    18,    21,    32,   111,   112,
     113,   142,    34,   114,    17,    86,    36,    92,    18,    39,
      21,    37,    18,    16,   142,    31,    46,   122,   102,   108,
      13,    17,    38,    38,    38,   114,     3,    27,   110,   127,
      34,    46,   108,   122,    29,   122,   142,    36,    99,    39,
     142,   111,   114,     1,    17,    35,    47,   115,   116,   117,
     118,   128,    17,    27,    34,   103,   104,   105,   109,    37,
      18,    17,    24,    25,    26,    31,    69,    70,    71,    72,
     128,   143,    18,   142,    18,    36,    37,    18,    18,    18,
      17,    20,    34,    35,    37,    42,   142,     1,     3,     4,
      17,    40,    82,    83,   128,   139,    20,    13,    17,     1,
       8,    11,    14,    17,   131,   136,   137,   138,    13,    17,
     136,    33,    33,   142,    16,   142,    13,   125,   134,   125,
     134,   142,   142,    16,   142,   125,    33,    15,    18,    21,
      32,    11,    32,   112,   113,   142,    15,   142,   142,    35,
     116,   111,   142,    14,    15,    17,    95,    96,   108,   122,
     115,    20,    20,   111,    17,   127,   114,   114,    27,   110,
     142,   122,   115,   115,    17,   104,    47,    38,   111,   119,
     123,    35,    39,    47,    21,    17,    34,    34,   114,    33,
     142,    16,    21,   114,   111,   135,    39,    17,    72,   128,
      18,    17,   125,   125,    17,     8,    67,    39,    40,    21,
     125,   142,    17,    16,    34,    48,   124,   138,    39,    40,
      13,    17,    40,   125,   125,   142,   142,   142,    15,    15,
      18,   142,    32,   142,   142,    35,    43,    44,    45,   115,
     111,   119,    17,    27,    17,    27,   142,   142,   111,   111,
     114,   114,   115,   111,   119,   111,    47,   111,    13,    17,
      18,   120,   121,   117,    47,    16,    33,    35,   111,    17,
     142,   142,   142,   136,    70,    21,    21,    83,     8,    67,
     128,    21,    17,   137,    35,    15,   142,    96,    96,    96,
     111,   111,   142,   142,   142,   142,   111,   111,   111,   111,
      19,    19,    39,   121,    47,    17,   114,    35,    40,   128,
     128,   128,    35,   114,   121,    35,   111,   142,   114
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
            (yyvsp[(1) - (2)].meth)->arg_types.size = MIN((yyvsp[(2) - (2)].rval), (yyvsp[(1) - (2)].meth)->arg_types.alloc_size);
            (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(1) - (2)].meth)->arg_types.size; 
            mta->burp_fundefn = true; }
    break;

  case 110:
#line 499 "mta_parse.y"
    { /* using is not parsed */
            String nms = mta->cur_namespcs.AsString(".");
            mta->Namespc_PushNew(nms);
          }
    break;

  case 111:
#line 505 "mta_parse.y"
    {
            String nms = mta->cur_namespcs.AsString(".");
            mta->Namespc_PushNew(nms);
          }
    break;

  case 112:
#line 511 "mta_parse.y"
    {
          mta->cur_namespcs.Reset(); }
    break;

  case 113:
#line 515 "mta_parse.y"
    { mta->cur_namespcs.Add((yyvsp[(1) - (1)].chr)); }
    break;

  case 114:
#line 516 "mta_parse.y"
    { mta->cur_namespcs.Add((yyvsp[(2) - (2)].chr)); }
    break;

  case 115:
#line 517 "mta_parse.y"
    { mta->cur_namespcs.Add(String("::") + (yyvsp[(3) - (3)].chr)); }
    break;

  case 116:
#line 518 "mta_parse.y"
    { mta->cur_namespcs.Add(String("(")); }
    break;

  case 117:
#line 519 "mta_parse.y"
    { mta->cur_namespcs.Add(String(")")); }
    break;

  case 118:
#line 520 "mta_parse.y"
    { mta->cur_namespcs.Add(String("\"")); }
    break;

  case 122:
#line 528 "mta_parse.y"
    { if((yyvsp[(1) - (2)].enm) != NULL) SETENUMDESC((yyvsp[(1) - (2)].enm),(yyvsp[(2) - (2)].chr)); }
    break;

  case 125:
#line 533 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 126:
#line 538 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (1)].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 127:
#line 541 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[(1) - (3)].enm));
            if((yyvsp[(3) - (3)].rval) != -424242) (yyvsp[(1) - (3)].enm)->enum_no = (yyvsp[(3) - (3)].rval);
            else (yyvsp[(1) - (3)].enm)->AddOption("#IGNORE"); // ignore bad math!
	    mta->enum_stack.Pop(); }
    break;

  case 129:
#line 550 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242))  (yyval.rval) = (yyvsp[(1) - (3)].rval) + (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 130:
#line 553 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) - (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 131:
#line 556 "mta_parse.y"
    {
	  if(((yyvsp[(1) - (3)].rval) != -424242) && ((yyvsp[(3) - (3)].rval) != -424242)) (yyval.rval) = (yyvsp[(1) - (3)].rval) | (yyvsp[(3) - (3)].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 132:
#line 562 "mta_parse.y"
    {
            EnumDef* ed = mta->cur_enum->enum_vals.FindName((yyvsp[(1) - (1)].chr));
            if(ed) (yyval.rval) = ed->enum_no;
            else   (yyval.rval) = -424242; }
    break;

  case 134:
#line 567 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 135:
#line 571 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[(1) - (1)].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 136:
#line 574 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 137:
#line 575 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 138:
#line 578 "mta_parse.y"
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
#line 586 "mta_parse.y"
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
#line 598 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
            mta->TypeAdded("enum", &(mta->cur_class->sub_types), (yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->source_end = mta->line-1;
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 141:
#line 604 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[(1) - (1)].typ));
            (yyvsp[(1) - (1)].typ)->SetType(TypeDef::SUBTYPE);
	    mta->PushState(MTA::Parse_inclass); (yyval.typ) = NULL; }
    break;

  case 142:
#line 608 "mta_parse.y"
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
#line 618 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 145:
#line 623 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 146:
#line 624 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 147:
#line 625 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 148:
#line 626 "mta_parse.y"
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
#line 646 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[(2) - (2)].memb); if((yyvsp[(2) - (2)].memb) != NULL) (yyvsp[(2) - (2)].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 151:
#line 652 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (3)].memb); }
    break;

  case 152:
#line 653 "mta_parse.y"
    { (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 153:
#line 654 "mta_parse.y"
    { }
    break;

  case 154:
#line 655 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[(2) - (4)].memb);
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (4)].typ));
            int spsz = sp->size;
            TypeDef* nty = (yyvsp[(1) - (4)].typ)->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    (yyvsp[(2) - (4)].memb)->type = nty; }
    break;

  case 155:
#line 662 "mta_parse.y"
    { (yyvsp[(2) - (4)].memb)->type = (yyvsp[(1) - (4)].typ); (yyval.memb) = (yyvsp[(2) - (4)].memb); }
    break;

  case 156:
#line 666 "mta_parse.y"
    {
	     if(mta->cur_mstate == MTA::pblc)
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(1) - (1)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 157:
#line 670 "mta_parse.y"
    {
	     if(mta->cur_mstate == MTA::pblc)
	       mta->cur_class->members.AddUniqNameNew((yyvsp[(3) - (3)].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 158:
#line 676 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[(1) - (1)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 159:
#line 682 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[(3) - (4)].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 161:
#line 688 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 162:
#line 689 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 163:
#line 690 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 164:
#line 691 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 165:
#line 692 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 166:
#line 693 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 167:
#line 694 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 168:
#line 695 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 169:
#line 696 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 170:
#line 697 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 171:
#line 698 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 172:
#line 699 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 173:
#line 700 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 174:
#line 701 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 175:
#line 702 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(5) - (5)].chr);
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef((yyvsp[(3) - (5)].meth)->name, TypeDef::FUNCTION,0,0);
              taMisc::types.Add(nt);
              mta->Info(5, "added reg fun to types:", nt->name);
	      nt->methods.AddUniqNameNew((yyvsp[(3) - (5)].meth)); (yyvsp[(3) - (5)].meth)->type = (yyvsp[(2) - (5)].typ);
	      mta->meth_stack.Pop();  
              (yyvsp[(3) - (5)].meth)->arg_types.size = MIN((yyvsp[(4) - (5)].rval), (yyvsp[(3) - (5)].meth)->arg_types.alloc_size);
              (yyvsp[(3) - (5)].meth)->fun_argc = (yyvsp[(3) - (5)].meth)->arg_types.size;
	      (yyvsp[(3) - (5)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(3) - (5)].meth),(yyvsp[(5) - (5)].chr)); }
	    else { (yyval.meth) = NULL; mta->meth_stack.Pop(); } }
    break;

  case 176:
#line 715 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[(4) - (4)].chr);
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef((yyvsp[(2) - (4)].meth)->name, TypeDef::FUNCTION,0,0);
              taMisc::types.Add(nt);
              mta->Info(5, "added reg fun to types:", nt->name);
	      nt->methods.AddUniqNameNew((yyvsp[(2) - (4)].meth)); (yyvsp[(2) - (4)].meth)->type = &TA_int;
	      mta->meth_stack.Pop();
              (yyvsp[(2) - (4)].meth)->arg_types.size = MIN((yyvsp[(3) - (4)].rval), (yyvsp[(2) - (4)].meth)->arg_types.alloc_size);
              (yyvsp[(2) - (4)].meth)->fun_argc = (yyvsp[(2) - (4)].meth)->arg_types.size;
	      (yyvsp[(2) - (4)].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[(2) - (4)].meth),(yyvsp[(4) - (4)].chr)); }
	    else { (yyval.meth) = 0; mta->meth_stack.Pop(); } }
    break;

  case 177:
#line 728 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 178:
#line 729 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 179:
#line 730 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 180:
#line 731 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 182:
#line 736 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth); (yyvsp[(2) - (2)].meth)->is_static = true; }
    break;

  case 183:
#line 737 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (2)].meth);  if((yyvsp[(2) - (2)].meth) != NULL) (yyvsp[(2) - (2)].meth)->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 184:
#line 739 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(3) - (3)].meth);  if((yyvsp[(3) - (3)].meth) != NULL) (yyvsp[(3) - (3)].meth)->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
    break;

  case 185:
#line 744 "mta_parse.y"
    { (yyval.meth) = (yyvsp[(2) - (3)].meth); (yyvsp[(2) - (3)].meth)->type = (yyvsp[(1) - (3)].typ); SETDESC((yyvsp[(2) - (3)].meth),(yyvsp[(3) - (3)].chr)); }
    break;

  case 186:
#line 745 "mta_parse.y"
    { (yyvsp[(1) - (2)].meth)->type = &TA_int; SETDESC((yyvsp[(1) - (2)].meth),(yyvsp[(2) - (2)].chr)); }
    break;

  case 187:
#line 746 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 188:
#line 747 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 189:
#line 748 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 190:
#line 751 "mta_parse.y"
    {
           (yyvsp[(1) - (2)].meth)->arg_types.size = MIN((yyvsp[(2) - (2)].rval), (yyvsp[(1) - (2)].meth)->arg_types.alloc_size);
           (yyvsp[(1) - (2)].meth)->fun_argc = (yyvsp[(1) - (2)].meth)->arg_types.size; 
           mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[(1) - (2)].meth)->fun_argd > (yyvsp[(1) - (2)].meth)->fun_argc) (yyvsp[(1) - (2)].meth)->fun_argd = -1; }
    break;

  case 191:
#line 760 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[(1) - (1)].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 192:
#line 764 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 193:
#line 765 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 194:
#line 766 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].chr); }
    break;

  case 195:
#line 767 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 196:
#line 768 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 197:
#line 769 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 198:
#line 770 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 199:
#line 771 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (4)].chr); }
    break;

  case 200:
#line 772 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (2)].chr); }
    break;

  case 201:
#line 773 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (3)].chr); }
    break;

  case 202:
#line 777 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 203:
#line 778 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 204:
#line 779 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(2) - (3)].chr); }
    break;

  case 205:
#line 780 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 206:
#line 781 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 207:
#line 782 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(3) - (4)].chr); }
    break;

  case 208:
#line 786 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 209:
#line 787 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 210:
#line 788 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 211:
#line 791 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 212:
#line 792 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(2) - (3)].rval); }
    break;

  case 213:
#line 795 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 214:
#line 796 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval); }
    break;

  case 215:
#line 799 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 216:
#line 800 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (3)].rval) + 1; }
    break;

  case 217:
#line 801 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (4)].rval); }
    break;

  case 218:
#line 802 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 219:
#line 805 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 220:
#line 807 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      String ad = (yyvsp[(2) - (2)].chr);
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 221:
#line 815 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 222:
#line 819 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (1)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 223:
#line 823 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(2) - (2)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (2)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 224:
#line 827 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[(2) - (3)].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (3)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 225:
#line 831 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[(4) - (6)].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[(1) - (6)].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 226:
#line 835 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[(1) - (1)].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 234:
#line 856 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 235:
#line 860 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 236:
#line 863 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 237:
#line 864 "mta_parse.y"
    { (yyval.rval) = (yyvsp[(1) - (2)].rval) + 1; }
    break;

  case 238:
#line 867 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->type_stack.Push((yyval.typ));
                                  mta->SetSource((yyval.typ), false); }
    break;

  case 239:
#line 871 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[(1) - (1)].typ); }
    break;

  case 241:
#line 876 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 242:
#line 877 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 244:
#line 881 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(1) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(1) - (2)].typ)->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, (yyval.typ)); }
	  }
    break;

  case 246:
#line 891 "mta_parse.y"
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

  case 248:
#line 903 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[(2) - (2)].typ));
            int spsz = sp->size;
            (yyval.typ) = (yyvsp[(2) - (2)].typ)->GetConstType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("const", sp, (yyval.typ)); }
	  }
    break;

  case 250:
#line 912 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (4)].typ); }
    break;

  case 252:
#line 916 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
	    (yyval.typ) = td; }
    break;

  case 253:
#line 923 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[(1) - (3)].typ)->sub_types.FindName((yyvsp[(3) - (3)].chr))) == NULL) {
              TypeDef* nty = new TypeDef((yyvsp[(3) - (3)].chr)); mta->SetSource(nty, false);
              (yyvsp[(1) - (3)].typ)->sub_types.AddUniqNameNew(nty); /* add it -- typename def */
              (yyval.typ) = nty;
            }
            (yyval.typ) = td; }
    break;

  case 254:
#line 930 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 255:
#line 931 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(3) - (3)].typ); }
    break;

  case 256:
#line 932 "mta_parse.y"
    {
            TypeDef* nty = new TypeDef((yyvsp[(2) - (2)].chr)); mta->SetSource(nty, false);
	    TypeSpace* sp = mta->GetTypeSpace(nty);
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) { mta->TypeAdded("namespace new", sp, (yyval.typ));
              (yyval.typ)->namespc = mta->cur_nmspc_tmp; }
	    else { mta->TypeNotAdded("namespace new", sp, (yyval.typ), nty); delete nty; }
          }
    break;

  case 257:
#line 940 "mta_parse.y"
    {
            mta->Info(2, "namespace type:", mta->cur_nmspc_tmp, "::", (yyvsp[(2) - (2)].typ)->name);
            (yyvsp[(2) - (2)].typ)->namespc = mta->cur_nmspc_tmp; /* todo: could check.. */
            (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 258:
#line 944 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); }
    break;

  case 260:
#line 946 "mta_parse.y"
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

  case 261:
#line 961 "mta_parse.y"
    { /* this template */
            if(!((yyvsp[(1) - (4)].typ)->IsTemplate())) {
              mta->Warning(1, "Type:", (yyvsp[(1) - (4)].typ)->name, "used as template but not marked as such",
                           "-- now marking -- probably just an internal type");
              (yyvsp[(1) - (4)].typ)->SetType(TypeDef::TEMPLATE); }
	    (yyval.typ) = (yyvsp[(1) - (4)].typ); }
    break;

  case 262:
#line 969 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::STRUCT);
          (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 263:
#line 971 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::STRUCT); }
    break;

  case 264:
#line 972 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->SetType(TypeDef::UNION);
            (yyval.typ)->ClearType(TypeDef::VOID); }
    break;

  case 265:
#line 974 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(2) - (2)].typ); (yyval.typ)->AssignType(TypeDef::UNION); }
    break;

  case 267:
#line 978 "mta_parse.y"
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

  case 268:
#line 990 "mta_parse.y"
    {
           mta->cur_typ_templ_pars.Reset(); }
    break;

  case 270:
#line 996 "mta_parse.y"
    { (yyval.typ) = (yyvsp[(1) - (3)].typ); }
    break;

  case 271:
#line 1001 "mta_parse.y"
    {
            mta->cur_typ_templ_pars.Link((yyvsp[(1) - (1)].typ)); }
    break;

  case 272:
#line 1003 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (1)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 273:
#line 1005 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 274:
#line 1007 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 275:
#line 1009 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 276:
#line 1011 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((yyvsp[(3) - (4)].chr)); mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 277:
#line 1013 "mta_parse.y"
    {
            (yyval.typ) = new TypeDef((String)(yyvsp[(1) - (1)].rval));
            mta->cur_typ_templ_pars.Push((yyval.typ)); }
    break;

  case 278:
#line 1016 "mta_parse.y"
    {  
          (yyval.typ) = new TypeDef((yyvsp[(2) - (2)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 279:
#line 1018 "mta_parse.y"
    {  
          (yyval.typ) = (yyvsp[(2) - (2)].typ); mta->cur_templ_pars.Link((yyvsp[(2) - (2)].typ)); }
    break;

  case 280:
#line 1020 "mta_parse.y"
    {  
          (yyval.typ) = new TypeDef((yyvsp[(1) - (2)].chr)); mta->cur_templ_pars.Push((yyval.typ)); }
    break;

  case 281:
#line 1022 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 285:
#line 1031 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 286:
#line 1032 "mta_parse.y"
    { (yyval.chr) = (yyvsp[(1) - (1)].typ)->name; }
    break;

  case 289:
#line 1039 "mta_parse.y"
    { mta->cur_nmspc_tmp = (yyvsp[(1) - (2)].chr); (yyval.chr) = mta->cur_nmspc_tmp; }
    break;

  case 294:
#line 1051 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 295:
#line 1058 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 296:
#line 1065 "mta_parse.y"
    { mta->defn_st_line = mta->line-1;
            if(mta->state == MTA::Parse_inclass) 
              mta->state = MTA::Parse_class; // avoid function eater in mta_lex
          }
    break;

  case 297:
#line 1072 "mta_parse.y"
    { mta->defn_st_line = mta->line-1; }
    break;


/* Line 1267 of yacc.c.  */
#line 3609 "y.tab.c"
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


#line 1075 "mta_parse.y"


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

