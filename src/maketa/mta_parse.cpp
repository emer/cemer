/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

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
     STRUCT = 259,
     UNION = 260,
     ENUM = 261,
     FUNTYPE = 262,
     STATIC = 263,
     TEMPLATE = 264,
     CONST = 265,
     TYPEDEF = 266,
     TA_TYPEDEF = 267,
     TYPE = 268,
     NUMBER = 269,
     FUNCTION = 270,
     ARRAY = 271,
     NAME = 272,
     COMMENT = 273,
     FUNCALL = 274,
     SCOPER = 275,
     EQUALS = 276,
     PUBLIC = 277,
     PRIVATE = 278,
     PROTECTED = 279,
     OPERATOR = 280,
     FRIEND = 281,
     THISNAME = 282,
     REGFUN = 283,
     VIRTUAL = 284
   };
#endif
/* Tokens.  */
#define CLASS 258
#define STRUCT 259
#define UNION 260
#define ENUM 261
#define FUNTYPE 262
#define STATIC 263
#define TEMPLATE 264
#define CONST 265
#define TYPEDEF 266
#define TA_TYPEDEF 267
#define TYPE 268
#define NUMBER 269
#define FUNCTION 270
#define ARRAY 271
#define NAME 272
#define COMMENT 273
#define FUNCALL 274
#define SCOPER 275
#define EQUALS 276
#define PUBLIC 277
#define PRIVATE 278
#define PROTECTED 279
#define OPERATOR 280
#define FRIEND 281
#define THISNAME 282
#define REGFUN 283
#define VIRTUAL 284




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

void yyerror(char *s);
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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 44 "mta_parse.y"
typedef struct YYSTYPE {
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  char*    	chr;
  int	   	rval;
} YYSTYPE;
/* Line 196 of yacc.c.  */
#line 175 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */
#line 187 "y.tab.c"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
struct yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (struct yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
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
      while (0)
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
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   541

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  45
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  83
/* YYNRULES -- Number of rules. */
#define YYNRULES  204
/* YYNRULES -- Number of states. */
#define YYNSTATES  339

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    19,    22,
      25,    28,    30,    32,    34,    37,    40,    45,    49,    54,
      57,    65,    70,    72,    77,    80,    84,    88,    91,    93,
      95,    98,   102,   104,   107,   111,   115,   118,   122,   126,
     128,   132,   135,   138,   140,   142,   144,   148,   150,   153,
     155,   158,   160,   163,   165,   167,   169,   172,   176,   180,
     183,   187,   191,   197,   199,   202,   207,   209,   214,   218,
     221,   223,   226,   228,   231,   233,   236,   238,   240,   244,
     246,   250,   254,   256,   258,   260,   262,   265,   267,   269,
     271,   273,   276,   278,   280,   283,   286,   289,   291,   293,
     296,   300,   303,   308,   313,   315,   319,   321,   326,   328,
     332,   337,   342,   347,   353,   359,   364,   368,   373,   378,
     384,   389,   391,   394,   397,   401,   404,   409,   416,   420,
     423,   425,   427,   429,   432,   435,   438,   442,   444,   447,
     451,   453,   455,   458,   461,   465,   467,   470,   472,   476,
     481,   483,   486,   488,   491,   495,   502,   504,   506,   509,
     513,   516,   518,   520,   523,   525,   527,   529,   532,   534,
     536,   539,   541,   544,   546,   549,   551,   554,   557,   561,
     565,   569,   573,   576,   578,   583,   588,   590,   593,   595,
     599,   601,   603,   605,   608,   610,   612,   614,   617,   619,
     621,   623,   625,   627,   629
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      46,     0,    -1,    -1,    46,    48,    -1,    46,    52,    -1,
      46,    56,    -1,    46,    58,    -1,    46,    68,    -1,    46,
      75,    -1,    46,    47,    -1,    46,     1,    -1,    12,    -1,
      49,    -1,    50,    -1,    50,    18,    -1,    11,    51,    -1,
      11,    60,   120,   124,    -1,   114,   113,   124,    -1,   114,
      18,   113,   124,    -1,   114,   124,    -1,   114,    31,    30,
     113,    32,   101,   124,    -1,   114,    20,    30,   113,    -1,
      53,    -1,    54,    78,    33,   124,    -1,    55,    34,    -1,
      55,    18,    34,    -1,    55,    34,    18,    -1,     6,   113,
      -1,     6,    -1,    57,    -1,    63,   124,    -1,    63,   124,
      18,    -1,    59,    -1,    60,   124,    -1,    60,   123,   124,
      -1,    61,    85,    33,    -1,    62,    34,    -1,    62,    18,
      34,    -1,    62,    34,    18,    -1,    63,    -1,    63,    35,
      64,    -1,   127,   113,    -1,   127,    13,    -1,   127,    -1,
     126,    -1,    65,    -1,    64,    36,    65,    -1,   114,    -1,
      66,   114,    -1,    17,    -1,    66,    17,    -1,    67,    -1,
      66,    67,    -1,   125,    -1,    29,    -1,    69,    -1,    70,
     124,    -1,    70,   123,   124,    -1,    71,    85,    33,    -1,
      72,    34,    -1,    72,    18,    34,    -1,    72,    34,    18,
      -1,     9,    73,    74,    37,    62,    -1,    38,    -1,     3,
     113,    -1,    74,    36,     3,   113,    -1,    76,    -1,    28,
     112,    77,    98,    -1,    28,    77,    98,    -1,    97,   101,
      -1,    79,    -1,    78,    79,    -1,    80,    -1,    80,    18,
      -1,    81,    -1,    81,    36,    -1,     1,    -1,    84,    -1,
      84,    21,    82,    -1,    83,    -1,    82,    39,    83,    -1,
      82,    40,    83,    -1,    17,    -1,    14,    -1,    17,    -1,
      86,    -1,    85,    86,    -1,    87,    -1,    93,    -1,    53,
      -1,    50,    -1,    60,   124,    -1,     1,    -1,    88,    -1,
      22,    35,    -1,    23,    35,    -1,    24,    35,    -1,    18,
      -1,    89,    -1,     8,    89,    -1,   111,    90,   124,    -1,
      90,   124,    -1,   111,    91,    16,   124,    -1,   111,    92,
     101,   124,    -1,    91,    -1,    90,    36,    91,    -1,    17,
      -1,    31,    30,    17,    32,    -1,    94,    -1,   108,   102,
      98,    -1,   121,   108,   102,    98,    -1,   108,   102,   106,
      98,    -1,    41,   108,   102,    98,    -1,   121,    41,   108,
     102,    98,    -1,    26,   112,    25,   101,    98,    -1,    26,
      25,   101,    98,    -1,    26,   112,   124,    -1,    26,     3,
     112,   124,    -1,    26,     3,    17,   124,    -1,    26,   112,
      97,   101,    98,    -1,    26,    97,   101,    98,    -1,    95,
      -1,     8,    95,    -1,    29,    95,    -1,   111,    96,    98,
      -1,    96,    98,    -1,   111,    25,   101,    98,    -1,   111,
      25,    31,    32,   101,    98,    -1,    25,   101,    98,    -1,
      97,   101,    -1,    17,    -1,   124,    -1,    99,    -1,   100,
     124,    -1,    18,   124,    -1,    18,    99,    -1,    18,   100,
     124,    -1,    15,    -1,    10,    15,    -1,    10,    18,    15,
      -1,    21,    -1,    10,    -1,    10,    21,    -1,    31,    32,
      -1,    31,   103,    32,    -1,    32,    -1,   103,    32,    -1,
     104,    -1,   103,    36,   104,    -1,   103,    42,    42,    42,
      -1,   105,    -1,   105,    21,    -1,   114,    -1,   114,    17,
      -1,   114,    17,    16,    -1,   114,    31,    30,    17,    32,
     101,    -1,    17,    -1,   107,    -1,   106,   107,    -1,   109,
      13,    19,    -1,    27,    31,    -1,    35,    -1,    30,    -1,
     110,    30,    -1,   112,    -1,   114,    -1,   121,    -1,   121,
     114,    -1,    17,    -1,   115,    -1,   115,    43,    -1,   116,
      -1,   116,   110,    -1,   117,    -1,    10,   117,    -1,   118,
      -1,   126,   118,    -1,   126,   113,    -1,    13,    20,    17,
      -1,    27,    20,    17,    -1,    13,    20,    13,    -1,    27,
      20,    13,    -1,    20,    13,    -1,    27,    -1,    13,    73,
     119,    37,    -1,    27,    73,   119,    37,    -1,    13,    -1,
     118,    13,    -1,    13,    -1,   119,    36,    13,    -1,    17,
      -1,    13,    -1,   122,    -1,   121,   122,    -1,     7,    -1,
      29,    -1,    17,    -1,    17,    16,    -1,    44,    -1,    22,
      -1,    23,    -1,    24,    -1,     4,    -1,     5,    -1,     3,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   113,   113,   114,   117,   119,   122,   125,   127,   129,
     131,   136,   139,   147,   148,   152,   153,   157,   162,   169,
     199,   202,   207,   213,   216,   217,   218,   221,   225,   232,
     240,   241,   245,   253,   254,   258,   264,   267,   269,   274,
     275,   278,   283,   287,   293,   302,   305,   313,   314,   315,
     316,   319,   320,   323,   324,   328,   336,   337,   341,   347,
     350,   352,   357,   364,   368,   369,   372,   378,   380,   384,
     389,   390,   393,   394,   397,   398,   399,   404,   407,   413,
     414,   419,   427,   428,   432,   435,   436,   439,   448,   461,
     464,   467,   471,   475,   476,   477,   478,   479,   499,   500,
     506,   507,   508,   516,   520,   524,   530,   536,   541,   542,
     543,   544,   545,   546,   547,   548,   549,   550,   551,   552,
     560,   571,   572,   573,   578,   579,   580,   581,   582,   585,
     592,   596,   597,   598,   599,   600,   601,   605,   606,   607,
     611,   612,   613,   616,   617,   620,   621,   624,   625,   626,
     629,   631,   639,   643,   647,   651,   655,   662,   663,   666,
     670,   674,   677,   678,   681,   684,   685,   686,   689,   692,
     693,   703,   704,   712,   713,   722,   723,   724,   725,   729,
     733,   734,   735,   736,   737,   754,   760,   761,   771,   772,
     775,   776,   779,   780,   783,   784,   787,   788,   791,   794,
     795,   796,   800,   801,   805
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CLASS", "STRUCT", "UNION", "ENUM",
  "FUNTYPE", "STATIC", "TEMPLATE", "CONST", "TYPEDEF", "TA_TYPEDEF",
  "TYPE", "NUMBER", "FUNCTION", "ARRAY", "NAME", "COMMENT", "FUNCALL",
  "SCOPER", "EQUALS", "PUBLIC", "PRIVATE", "PROTECTED", "OPERATOR",
  "FRIEND", "THISNAME", "REGFUN", "VIRTUAL", "'*'", "'('", "')'", "'}'",
  "'{'", "':'", "','", "'>'", "'<'", "'+'", "'-'", "'~'", "'.'", "'&'",
  "';'", "$accept", "list", "preparsed", "typedefn", "typedefns",
  "typedsub", "defn", "enumdefn", "enumdsub", "enumname", "enumnm",
  "classdecl", "classdecls", "classdefn", "classdefns", "classdsub",
  "classname", "classhead", "classnm", "classinh", "classpar", "classptyp",
  "classpmod", "templdefn", "templdefns", "templdsub", "templname",
  "templhead", "templopen", "templpars", "fundecl", "funnm", "regfundefn",
  "enums", "enumline", "enumitm", "enumitms", "enummath", "enummathitm",
  "enmitmname", "membs", "membline", "membdefn", "basicmemb", "nostatmemb",
  "membnames", "membname", "membfunp", "methdefn", "basicmeth",
  "nostatmeth", "mbfundefn", "methname", "fundefn", "funsubdefn",
  "funsubdecl", "funargs", "constfun", "args", "argdefn", "subargdefn",
  "constrlist", "constref", "consthsnm", "constcoln", "ptrs", "membtype",
  "ftype", "tyname", "type", "noreftype", "constype", "subtype",
  "combtype", "templtypes", "tdname", "funtspec", "funtsmod", "varname",
  "term", "access", "structstruct", "classkeyword", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
      42,    40,    41,   125,   123,    58,    44,    62,    60,    43,
      45,   126,    46,    38,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    45,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    47,    48,    49,    49,    50,    50,    51,    51,    51,
      51,    51,    52,    53,    54,    54,    54,    55,    55,    56,
      57,    57,    58,    59,    59,    60,    61,    61,    61,    62,
      62,    63,    63,    63,    63,    64,    64,    65,    65,    65,
      65,    66,    66,    67,    67,    68,    69,    69,    70,    71,
      71,    71,    72,    73,    74,    74,    75,    76,    76,    77,
      78,    78,    79,    79,    80,    80,    80,    81,    81,    82,
      82,    82,    83,    83,    84,    85,    85,    86,    86,    86,
      86,    86,    86,    87,    87,    87,    87,    87,    88,    88,
      89,    89,    89,    89,    90,    90,    91,    92,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    94,    94,    94,    95,    95,    95,    95,    95,    96,
      97,    98,    98,    98,    98,    98,    98,    99,    99,    99,
     100,   100,   100,   101,   101,   102,   102,   103,   103,   103,
     104,   104,   105,   105,   105,   105,   105,   106,   106,   107,
     108,   109,   110,   110,   111,   112,   112,   112,   113,   114,
     114,   115,   115,   116,   116,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   118,   118,   119,   119,
     120,   120,   121,   121,   122,   122,   123,   123,   124,   125,
     125,   125,   126,   126,   127
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     1,     1,     2,     2,     4,     3,     4,     2,
       7,     4,     1,     4,     2,     3,     3,     2,     1,     1,
       2,     3,     1,     2,     3,     3,     2,     3,     3,     1,
       3,     2,     2,     1,     1,     1,     3,     1,     2,     1,
       2,     1,     2,     1,     1,     1,     2,     3,     3,     2,
       3,     3,     5,     1,     2,     4,     1,     4,     3,     2,
       1,     2,     1,     2,     1,     2,     1,     1,     3,     1,
       3,     3,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     2,     1,     1,     2,     2,     2,     1,     1,     2,
       3,     2,     4,     4,     1,     3,     1,     4,     1,     3,
       4,     4,     4,     5,     5,     4,     3,     4,     4,     5,
       4,     1,     2,     2,     3,     2,     4,     6,     3,     2,
       1,     1,     1,     2,     2,     2,     3,     1,     2,     3,
       1,     1,     2,     2,     3,     1,     2,     1,     3,     4,
       1,     2,     1,     2,     3,     6,     1,     1,     2,     3,
       2,     1,     1,     2,     1,     1,     1,     2,     1,     1,
       2,     1,     2,     1,     2,     1,     2,     2,     3,     3,
       3,     3,     2,     1,     4,     4,     1,     2,     1,     3,
       1,     1,     1,     2,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,    10,   204,   202,   203,    28,     0,     0,
      11,     0,     9,     3,    12,    13,     4,    22,     0,     0,
       5,    29,     6,    32,     0,     0,     0,    39,     7,    55,
       0,     0,     0,     8,    66,    44,    43,   168,    27,    63,
       0,     0,   186,     0,   183,    15,     0,    39,     0,   169,
     171,   173,   175,    44,   194,   130,   195,     0,     0,     0,
     165,   166,   192,     0,    14,    76,    84,     0,    70,    72,
      74,    77,     0,    24,   196,   198,     0,    33,    92,     0,
     106,    97,     0,     0,     0,     0,     0,   183,   195,     0,
      90,    89,     0,     0,    85,    87,    93,    98,     0,   104,
      88,   108,   121,     0,     0,     0,     0,   164,   166,     0,
      36,     0,    30,     0,    56,     0,     0,    59,    42,    41,
       0,     0,   174,     0,     0,   182,     0,     0,   191,   190,
       0,     0,     0,     0,     0,    19,   170,   162,   172,   187,
     186,   177,   176,   141,   137,     0,   140,    68,   132,     0,
     131,     0,    69,     0,   167,   193,     0,    71,    73,    75,
       0,    25,    26,   197,    34,    99,   122,    94,    95,    96,
       0,     0,     0,     0,     0,   160,   123,     0,     0,     0,
      91,    35,    86,     0,   101,   125,   129,   156,   145,     0,
       0,   147,   150,   152,     0,     0,     0,   104,     0,     0,
       0,     0,    37,    38,    49,   199,   200,   201,    54,    40,
      45,     0,    51,    47,    53,    31,    57,    58,    60,    61,
      64,     0,     0,   180,   178,   188,     0,   181,   179,     0,
      16,     0,     0,     0,    17,   163,   138,     0,   142,   135,
       0,   134,   133,   143,     0,    67,    23,    83,    82,    78,
      79,   128,     0,     0,     0,     0,     0,     0,   116,     0,
     106,   105,   161,   109,     0,   157,     0,   146,     0,     0,
     151,   153,     0,     0,     0,     0,   100,     0,     0,   124,
       0,     0,     0,    50,    52,    48,     0,    62,     0,   184,
     185,    18,    21,     0,   139,   136,   144,     0,     0,   118,
     117,   115,   120,     0,     0,   112,   111,   158,     0,   148,
       0,   154,     0,   143,   126,     0,   102,   103,     0,   110,
      46,    65,   189,     0,    80,    81,   114,   119,   159,   149,
       0,     0,   107,   113,     0,     0,   127,    20,   155
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,    12,    13,    14,    90,    45,    16,    91,    18,
      19,    20,    21,    22,    23,    92,    25,    26,    47,   209,
     210,   211,   212,    28,    29,    30,    31,    32,   127,   121,
      33,    34,    57,    67,    68,    69,    70,   249,   250,    71,
      93,    94,    95,    96,    97,    98,    99,   198,   100,   101,
     102,   103,   104,   147,   148,   149,   152,   189,   190,   191,
     192,   264,   265,   105,   266,   138,   106,   107,   141,    60,
      49,    50,    51,    52,   226,   130,    61,    62,    76,   150,
     214,    63,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -169
static const short int yypact[] =
{
    -169,   130,  -169,  -169,  -169,  -169,  -169,    73,   -23,   495,
    -169,   455,  -169,  -169,  -169,     0,  -169,  -169,    31,     1,
    -169,  -169,  -169,  -169,    13,   350,    83,    49,  -169,  -169,
      13,   350,   104,  -169,  -169,  -169,    98,  -169,  -169,  -169,
      36,   514,    16,   100,    22,  -169,   111,    61,   136,   101,
     145,  -169,   153,   183,  -169,  -169,  -169,    55,   150,   169,
    -169,   484,  -169,   183,  -169,  -169,  -169,    39,  -169,   177,
     165,   190,   180,   208,   225,  -169,   176,  -169,  -169,   418,
     193,  -169,   220,   223,   232,   150,   239,    71,   429,   244,
    -169,  -169,   176,   272,  -169,  -169,  -169,  -169,   113,  -169,
    -169,  -169,  -169,    55,   150,   142,   120,  -169,    18,   247,
     268,   376,   269,   176,  -169,   311,   254,   273,  -169,  -169,
      73,   124,  -169,   204,   280,  -169,   206,   280,  -169,  -169,
     176,    73,   270,   274,   176,  -169,  -169,  -169,   276,  -169,
    -169,  -169,   153,   187,  -169,    34,  -169,  -169,  -169,   176,
    -169,   172,  -169,    55,  -169,  -169,   176,  -169,  -169,  -169,
     219,  -169,  -169,  -169,  -169,  -169,  -169,  -169,  -169,  -169,
      55,   466,   150,   150,    63,  -169,  -169,   123,   271,   142,
    -169,  -169,  -169,   286,  -169,  -169,  -169,  -169,  -169,    48,
     152,  -169,   287,    50,   278,   277,   113,   295,   150,    55,
     244,   142,  -169,  -169,  -169,  -169,  -169,  -169,  -169,   284,
    -169,   397,  -169,  -169,  -169,  -169,  -169,  -169,  -169,  -169,
    -169,   320,   258,  -169,  -169,  -169,   202,  -169,  -169,   211,
    -169,   176,    73,    73,  -169,  -169,  -169,   310,  -169,  -169,
     176,  -169,  -169,  -169,   155,  -169,  -169,  -169,  -169,   230,
    -169,  -169,   176,   176,    55,    55,   150,   150,  -169,    55,
    -169,  -169,  -169,  -169,    48,  -169,   313,  -169,   497,   285,
    -169,   314,   302,   205,    55,   322,  -169,   176,   176,  -169,
     142,    55,   376,  -169,  -169,  -169,    73,  -169,   328,  -169,
    -169,  -169,  -169,   315,  -169,  -169,  -169,   219,   219,  -169,
    -169,  -169,  -169,    55,    55,  -169,  -169,  -169,   323,  -169,
     301,  -169,   329,   150,  -169,   316,  -169,  -169,    55,  -169,
    -169,  -169,  -169,   150,  -169,  -169,  -169,  -169,  -169,  -169,
     317,    55,  -169,  -169,   176,   150,  -169,  -169,  -169
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -169,  -169,  -169,  -169,  -169,   344,  -169,  -169,   349,  -169,
    -169,  -169,  -169,  -169,  -169,   197,  -169,   137,   361,  -169,
      82,  -169,   154,  -169,  -169,  -169,  -169,  -169,    33,  -169,
    -169,  -169,   307,  -169,   304,  -169,  -169,  -169,   -47,  -169,
     338,   -64,  -169,  -169,   299,   279,   -97,  -169,  -169,  -169,
      35,   -90,    -9,   -91,   237,   238,   -78,  -168,  -147,   116,
    -169,  -169,   126,   -81,  -169,  -169,   300,    -1,    -2,    -8,
    -169,  -169,   346,     8,   265,  -169,   -11,   -44,   364,   -24,
    -169,    12,  -169
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -131
static const short int yytable[] =
{
      77,    48,    58,   112,   244,    38,   114,   170,   179,   197,
      59,   259,   185,    35,   108,    39,   199,   155,    64,    72,
     108,    53,     5,     6,   135,    54,   186,   201,    41,   182,
      74,    42,    65,   281,   119,    73,   123,    53,    43,   120,
      65,    40,   126,    53,   143,    87,   134,    56,    66,   144,
      58,   182,   164,   154,    39,   146,    66,    75,   143,   200,
      39,   142,   245,   144,   155,   143,   145,   271,   180,   146,
     144,   142,   156,   145,   184,   124,   146,   173,    75,   251,
      55,   272,   108,   262,   111,   174,   261,   199,   256,   216,
      37,   126,    75,    75,   254,   255,   111,   193,   263,    75,
     154,   109,   175,   213,   108,    53,   230,    75,   279,    39,
     234,   118,   318,   125,   166,    37,   274,   110,   220,   280,
     278,   241,   116,   176,   128,   242,   244,    53,   129,   231,
       2,     3,   246,     4,     5,     6,     7,    80,   117,     8,
      55,     9,    10,   193,   136,   194,     5,     6,   194,   183,
     258,   195,    41,    37,   131,    42,   132,    75,    11,   187,
     221,   222,    43,   301,   302,   257,   139,   133,   305,    44,
     253,   193,   276,   306,   188,   137,     5,     6,   303,   304,
      75,   151,    41,   314,   267,    42,    55,   296,   268,   187,
     319,   268,    43,   193,   269,   158,   140,   269,    24,    44,
      37,   159,   236,   285,   243,   237,    46,   291,   238,     5,
       6,   160,   326,   327,   161,    41,   295,   223,    42,   227,
      75,   224,   187,   228,  -130,    43,   162,   333,   299,   300,
     292,   293,    44,   247,    35,   331,   248,   313,   288,   289,
     336,   163,   171,     5,     6,   334,    54,   288,   290,    41,
     324,   325,    42,   316,   317,   167,    55,   338,   168,    43,
     193,     4,     5,     6,   172,   193,    44,   169,    56,   297,
     298,   178,   193,    78,   213,     4,     5,     6,     7,    54,
      79,   202,    41,     9,   321,    42,   203,   215,   218,    80,
      81,   219,    43,   225,    82,    83,    84,    85,    86,    87,
     232,    88,   175,   260,   233,   181,   235,   275,   270,   273,
     337,   277,    78,    89,     4,     5,     6,     7,    54,    79,
     282,    41,     9,   286,    42,   294,   308,   310,    80,    81,
     311,    43,   312,    82,    83,    84,    85,    86,    87,   315,
      88,   322,   328,   329,   217,    15,   330,   323,   332,   335,
      17,    78,    89,     4,     5,     6,     7,    54,    79,   287,
      41,     9,    27,    42,   320,   284,   153,    80,    81,   115,
      43,   157,    82,    83,    84,    85,    86,    87,   165,    88,
       5,     6,   239,   240,   309,   196,    41,   122,   177,    42,
     307,    89,   229,   204,   113,     0,    43,     0,   205,   206,
     207,     5,     6,    44,     0,   208,     0,    41,     0,     0,
      42,     0,     0,     0,   283,     0,     0,    43,     0,   205,
     206,   207,     5,     6,    44,    54,   208,     0,    41,     0,
       0,    42,     0,     5,     6,    80,    54,     0,    43,    41,
       0,     0,    42,    85,     0,    44,    55,    56,     0,    43,
       0,     0,     0,     0,    85,     0,    44,     0,    56,     5,
       6,     0,    54,     0,     0,    41,     0,     0,    42,     0,
       5,     6,    55,    54,     0,    43,    41,     0,     0,    42,
       0,     0,    44,   252,    56,     0,    43,     0,     5,     6,
       0,    54,     0,    44,    41,    56,     0,    42,     4,     5,
       6,     5,     6,     0,    43,    41,     0,    41,    42,     0,
      42,    44,     0,    56,   187,    43,     0,    43,     5,     6,
       0,     0,    44,     0,    44,     0,     0,    42,     0,     0,
       0,     0,     0,     0,    43,     0,     0,     0,     0,     0,
       0,    44
};

static const short int yycheck[] =
{
      24,     9,    11,    27,   151,     7,    30,    85,    89,   106,
      11,   179,   103,     1,    25,    38,   106,    61,    18,    18,
      31,     9,     4,     5,    48,     7,   104,   108,    10,    93,
      17,    13,     1,   201,    36,    34,    20,    25,    20,     3,
       1,     8,    20,    31,    10,    27,    48,    29,    17,    15,
      59,   115,    76,    61,    38,    21,    17,    44,    10,    41,
      38,    53,   153,    15,   108,    10,    18,    17,    92,    21,
      15,    63,    33,    18,    98,    42,    21,    86,    44,   170,
      17,    31,    93,    35,    35,    86,   183,   177,    25,   113,
      17,    20,    44,    44,   172,   173,    35,   105,   189,    44,
     108,    18,    31,   111,   115,    93,   130,    44,   199,    38,
     134,    13,   280,    13,    79,    17,   194,    34,   120,   200,
     198,   145,    18,    88,    13,   149,   273,   115,    17,   131,
       0,     1,   156,     3,     4,     5,     6,    17,    34,     9,
      17,    11,    12,   151,    43,    25,     4,     5,    25,    36,
     174,    31,    10,    17,    18,    13,    20,    44,    28,    17,
      36,    37,    20,   254,   255,   174,    13,    31,   259,    27,
     171,   179,   196,   264,    32,    30,     4,     5,   256,   257,
      44,    31,    10,   274,    32,    13,    17,    32,    36,    17,
     281,    36,    20,   201,    42,    18,    13,    42,     1,    27,
      17,    36,    15,   211,    32,    18,     9,   231,    21,     4,
       5,    21,   303,   304,    34,    10,   240,    13,    13,    13,
      44,    17,    17,    17,    31,    20,    18,   318,   252,   253,
     232,   233,    27,    14,   222,   313,    17,    32,    36,    37,
     331,    16,     3,     4,     5,   323,     7,    36,    37,    10,
     297,   298,    13,   277,   278,    35,    17,   335,    35,    20,
     268,     3,     4,     5,    25,   273,    27,    35,    29,    39,
      40,    27,   280,     1,   282,     3,     4,     5,     6,     7,
       8,    34,    10,    11,   286,    13,    18,    18,    34,    17,
      18,    18,    20,    13,    22,    23,    24,    25,    26,    27,
      30,    29,    31,    17,    30,    33,    30,    30,    21,    31,
     334,    16,     1,    41,     3,     4,     5,     6,     7,     8,
      36,    10,    11,     3,    13,    15,    13,    42,    17,    18,
      16,    20,    30,    22,    23,    24,    25,    26,    27,    17,
      29,    13,    19,    42,    33,     1,    17,    32,    32,    32,
       1,     1,    41,     3,     4,     5,     6,     7,     8,   222,
      10,    11,     1,    13,   282,   211,    59,    17,    18,    31,
      20,    67,    22,    23,    24,    25,    26,    27,    79,    29,
       4,     5,   145,   145,   268,   106,    10,    41,    88,    13,
     264,    41,   127,    17,    30,    -1,    20,    -1,    22,    23,
      24,     4,     5,    27,    -1,    29,    -1,    10,    -1,    -1,
      13,    -1,    -1,    -1,    17,    -1,    -1,    20,    -1,    22,
      23,    24,     4,     5,    27,     7,    29,    -1,    10,    -1,
      -1,    13,    -1,     4,     5,    17,     7,    -1,    20,    10,
      -1,    -1,    13,    25,    -1,    27,    17,    29,    -1,    20,
      -1,    -1,    -1,    -1,    25,    -1,    27,    -1,    29,     4,
       5,    -1,     7,    -1,    -1,    10,    -1,    -1,    13,    -1,
       4,     5,    17,     7,    -1,    20,    10,    -1,    -1,    13,
      -1,    -1,    27,    17,    29,    -1,    20,    -1,     4,     5,
      -1,     7,    -1,    27,    10,    29,    -1,    13,     3,     4,
       5,     4,     5,    -1,    20,    10,    -1,    10,    13,    -1,
      13,    27,    -1,    29,    17,    20,    -1,    20,     4,     5,
      -1,    -1,    27,    -1,    27,    -1,    -1,    13,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    27
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    46,     0,     1,     3,     4,     5,     6,     9,    11,
      12,    28,    47,    48,    49,    50,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    68,    69,
      70,    71,    72,    75,    76,   126,   127,    17,   113,    38,
      73,    10,    13,    20,    27,    51,    60,    63,   114,   115,
     116,   117,   118,   126,     7,    17,    29,    77,    97,   112,
     114,   121,   122,   126,    18,     1,    17,    78,    79,    80,
      81,    84,    18,    34,    17,    44,   123,   124,     1,     8,
      17,    18,    22,    23,    24,    25,    26,    27,    29,    41,
      50,    53,    60,    85,    86,    87,    88,    89,    90,    91,
      93,    94,    95,    96,    97,   108,   111,   112,   121,    18,
      34,    35,   124,   123,   124,    85,    18,    34,    13,   113,
       3,    74,   117,    20,    73,    13,    20,    73,    13,    17,
     120,    18,    20,    31,   113,   124,    43,    30,   110,    13,
      13,   113,   118,    10,    15,    18,    21,    98,    99,   100,
     124,    31,   101,    77,   114,   122,    33,    79,    18,    36,
      21,    34,    18,    16,   124,    89,    95,    35,    35,    35,
     101,     3,    25,    97,   112,    31,    95,   111,    27,   108,
     124,    33,    86,    36,   124,    98,   101,    17,    32,   102,
     103,   104,   105,   114,    25,    31,    90,    91,    92,    96,
      41,   108,    34,    18,    17,    22,    23,    24,    29,    64,
      65,    66,    67,   114,   125,    18,   124,    33,    34,    18,
     113,    36,    37,    13,    17,    13,   119,    13,    17,   119,
     124,   113,    30,    30,   124,    30,    15,    18,    21,    99,
     100,   124,   124,    32,   103,    98,   124,    14,    17,    82,
      83,    98,    17,   112,   101,   101,    25,    97,   124,   102,
      17,    91,    35,    98,   106,   107,   109,    32,    36,    42,
      21,    17,    31,    31,   101,    30,   124,    16,   101,    98,
     108,   102,    36,    17,    67,   114,     3,    62,    36,    37,
      37,   124,   113,   113,    15,   124,    32,    39,    40,   124,
     124,    98,    98,   101,   101,    98,    98,   107,    13,   104,
      42,    16,    30,    32,    98,    17,   124,   124,   102,    98,
      65,   113,    13,    32,    83,    83,    98,    98,    19,    42,
      17,   101,    32,    98,   101,    32,    98,   124,   101
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
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
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
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
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
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

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
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
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
      size_t yyn = 0;
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

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
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
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

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
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
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
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
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

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


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
	short int *yyss1 = yyss;
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

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

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

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
    { mta->yy_state = MTA::YYRet_Exit; }
    break;

  case 3:
#line 114 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok;
	    return mta->yy_state; }
    break;

  case 4:
#line 117 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 5:
#line 119 "mta_parse.y"
    {
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 6:
#line 122 "mta_parse.y"
    {
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 7:
#line 125 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 8:
#line 127 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 9:
#line 129 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
    break;

  case 10:
#line 131 "mta_parse.y"
    {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
    break;

  case 11:
#line 136 "mta_parse.y"
    { }
    break;

  case 12:
#line 139 "mta_parse.y"
    {
  	  if((yyvsp[0].typ) != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[0].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[0].typ));
	    if((yyval.typ) == (yyvsp[0].typ)) mta->TypeAdded("typedef", sp, (yyval.typ)); } }
    break;

  case 13:
#line 147 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 14:
#line 148 "mta_parse.y"
    {
	    if((yyvsp[-1].typ) != NULL)  SETDESC((yyvsp[-1].typ),(yyvsp[0].chr)); }
    break;

  case 15:
#line 152 "mta_parse.y"
    { (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 16:
#line 153 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[-2].typ); (yyvsp[-2].typ)->name = (yyvsp[-1].chr); mta->type_stack.Pop(); }
    break;

  case 17:
#line 157 "mta_parse.y"
    {
            (yyval.typ) = (yyvsp[-1].typ); (yyvsp[-1].typ)->AddParent((yyvsp[-2].typ)); (yyvsp[-1].typ)->ptr = (yyvsp[-2].typ)->ptr;
	    (yyvsp[-1].typ)->par_formal.BorrowUnique((yyvsp[-2].typ)->par_formal);
	    (yyvsp[-1].typ)->par_cache.BorrowUnique((yyvsp[-2].typ)->par_cache);
	    mta->type_stack.Pop(); }
    break;

  case 18:
#line 162 "mta_parse.y"
    { /* annoying place for a comment, but.. */
            (yyval.typ) = (yyvsp[-1].typ); (yyvsp[-1].typ)->AddParent((yyvsp[-3].typ)); (yyvsp[-1].typ)->ptr = (yyvsp[-3].typ)->ptr;
	    (yyvsp[-1].typ)->par_formal.BorrowUnique((yyvsp[-3].typ)->par_formal);
	    (yyvsp[-1].typ)->par_cache.BorrowUnique((yyvsp[-3].typ)->par_cache);
	    mta->type_stack.Pop(); }
    break;

  case 19:
#line 169 "mta_parse.y"
    {
	    TypeDef* td = (yyvsp[-1].typ)->parents[1]; mta->type_stack.Pop();
	    TypeSpace* sp = (yyvsp[-1].typ)->owner;
	    sp->Remove((yyvsp[-1].typ)); /* get rid of new one, cuz it is bogus */
	    /* not on list that it would be placed on now.. */
	    if((td->owner != mta->spc) && (mta->spc->FindName(td->name) == NULL)) {
	      if(mta->verbose >= 3)
		cerr << "transfered: " << td->name << " from: " << td->owner->name
		     << " to: " << mta->spc->name << "\n";
	      mta->spc->Transfer(td); /* now check for parent which is a combo of basic types */
	      if((td->parents.size == 1) && (td->parents[0]->owner != mta->spc) &&
		 (td->parents[0]->parents.size == 2)) {
		/* has one parent that is a combo-type which might be basic */
		TypeDef* par = td->parents[0];
		if((mta->spc_builtin.FindName(par->parents[0]->name) != NULL) &&
		   (mta->spc_builtin.FindName(par->parents[1]->name) != NULL)) {
		  if(mta->verbose >= 3)
		    cerr << "transfered: " << par->name << " from: " << par->owner->name
			 << " to: " << mta->spc->name << "\n";
		  TypeDef* already_there = mta->spc->FindName(par->name);
		  if(already_there == NULL)
		    mta->spc->Transfer(par); /* move parent to this list too */
		  else
		    td->parents.ReplaceLink(0, already_there);
		}
	      }
	    }
	    (yyval.typ) = td; }
    break;

  case 20:
#line 199 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[-3].typ); (yyval.typ)->AddParent(&TA_void_ptr); (yyval.typ)->ptr = 1;
	    mta->type_stack.Pop(); }
    break;

  case 21:
#line 202 "mta_parse.y"
    {
	    (yyval.typ) = (yyvsp[0].typ); (yyval.typ)->AddParent(&TA_void_ptr); (yyval.typ)->ptr = 1;
	    mta->type_stack.Pop(); }
    break;

  case 22:
#line 207 "mta_parse.y"
    {
            TypeSpace* sp = mta->GetTypeSpace((yyvsp[0].typ));
            (yyval.typ) = sp->AddUniqNameOld((yyvsp[0].typ));
	    if((yyval.typ) == (yyvsp[0].typ)) mta->TypeAdded("enum", sp, (yyval.typ)); }
    break;

  case 25:
#line 217 "mta_parse.y"
    { SETDESC((yyvsp[-2].typ),(yyvsp[-1].chr)); }
    break;

  case 26:
#line 218 "mta_parse.y"
    { SETDESC((yyvsp[-2].typ),(yyvsp[0].chr)); }
    break;

  case 27:
#line 221 "mta_parse.y"
    {
  	    (yyval.typ) = (yyvsp[0].typ);
	    (yyvsp[0].typ)->AddParFormal(&TA_enum); mta->cur_enum = (yyvsp[0].typ);
	    mta->type_stack.Pop(); }
    break;

  case 28:
#line 225 "mta_parse.y"
    {
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->cur_enum = (yyval.typ);
	    (yyval.typ)->AddParFormal(&TA_enum); (yyval.typ)->internal = true; }
    break;

  case 29:
#line 232 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[0].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[0].typ));
	    if((yyval.typ) == (yyvsp[0].typ)) mta->TypeAdded("class", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 30:
#line 240 "mta_parse.y"
    { mta->Burp(); }
    break;

  case 31:
#line 241 "mta_parse.y"
    { SETDESC((yyvsp[-2].typ),(yyvsp[0].chr)); }
    break;

  case 32:
#line 245 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[0].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[0].typ));
	    if((yyval.typ) == (yyvsp[0].typ)) mta->TypeAdded("class", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 35:
#line 258 "mta_parse.y"
    {
	    if((yyvsp[-2].typ)->HasOption("NO_TOKENS")) (yyvsp[-2].typ)->tokens.keep = false;
	    else (yyvsp[-2].typ)->tokens.keep = true; }
    break;

  case 36:
#line 264 "mta_parse.y"
    {
	    (yyvsp[-1].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 37:
#line 267 "mta_parse.y"
    {
	    SETDESC((yyvsp[-2].typ),(yyvsp[-1].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 38:
#line 269 "mta_parse.y"
    {
	    SETDESC((yyvsp[-2].typ),(yyvsp[0].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 39:
#line 274 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 40:
#line 275 "mta_parse.y"
    { mta->state = MTA::Parse_inclass; }
    break;

  case 41:
#line 278 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
            (yyval.typ) = (yyvsp[0].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[0].typ);
	    (yyvsp[0].typ)->AddParFormal(&TA_class);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 42:
#line 283 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    (yyval.typ) = (yyvsp[0].typ); mta->last_class = mta->cur_class; mta->cur_class = (yyvsp[0].typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 43:
#line 287 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[0].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
            mta->cur_mstate = MTA::prvt; }
    break;

  case 44:
#line 293 "mta_parse.y"
    {
            mta->state = MTA::Parse_class;
	    String nm = (yyvsp[0].typ)->name + "_" + (String)mta->anon_no++; nm += "_";
	    (yyval.typ) = new TypeDef(nm); mta->type_stack.Push((yyval.typ));
	    mta->last_class = mta->cur_class; mta->cur_class = (yyval.typ);
	    mta->cur_mstate = MTA::pblc; }
    break;

  case 45:
#line 302 "mta_parse.y"
    {
            if((yyvsp[0].typ)->InheritsFrom(TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent((yyvsp[0].typ)); }
    break;

  case 46:
#line 305 "mta_parse.y"
    {
            if((yyvsp[0].typ)->InheritsFrom(&TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent((yyvsp[0].typ));
	    if(!mta->cur_class->HasOption("MULT_INHERIT"))
	      mta->cur_class->opts.Add("MULT_INHERIT"); }
    break;

  case 48:
#line 314 "mta_parse.y"
    { (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 49:
#line 315 "mta_parse.y"
    { (yyval.typ) = &TA_class; }
    break;

  case 50:
#line 316 "mta_parse.y"
    { (yyval.typ) = &TA_class; }
    break;

  case 55:
#line 328 "mta_parse.y"
    {
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[0].typ));
	    (yyval.typ) = sp->AddUniqNameOld((yyvsp[0].typ));
	    if((yyval.typ) == (yyvsp[0].typ)) mta->TypeAdded("template", sp, (yyval.typ));
	    mta->type_stack.Pop(); }
    break;

  case 58:
#line 341 "mta_parse.y"
    {
          if((yyvsp[-2].typ)->HasOption("NO_TOKENS")) (yyvsp[-2].typ)->tokens.keep = false;
	  else (yyvsp[-2].typ)->tokens.keep = true; }
    break;

  case 59:
#line 347 "mta_parse.y"
    {
            mta->state = MTA::Parse_inclass; (yyvsp[-1].typ)->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
    break;

  case 60:
#line 350 "mta_parse.y"
    {
	    SETDESC((yyvsp[-2].typ),(yyvsp[-1].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 61:
#line 352 "mta_parse.y"
    {
	    SETDESC((yyvsp[-2].typ),(yyvsp[0].chr)); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
    break;

  case 62:
#line 357 "mta_parse.y"
    {
	    (yyvsp[0].typ)->templ_pars.Reset();
	    (yyvsp[0].typ)->templ_pars.Duplicate(mta->cur_templ_pars);
	    (yyvsp[0].typ)->internal = true;
	    (yyvsp[0].typ)->AddParFormal(&TA_template); (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 63:
#line 364 "mta_parse.y"
    { mta->cur_templ_pars.Reset(); }
    break;

  case 64:
#line 368 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[0].typ)); (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 65:
#line 369 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[0].typ)); (yyval.typ) = (yyvsp[-3].typ); }
    break;

  case 66:
#line 372 "mta_parse.y"
    {
            if(mta->spc == &(mta->spc_target)) /* only add reg_funs in target space */
	       TA_taRegFun.methods.AddUniqNameNew((yyvsp[0].meth));
	    mta->meth_stack.Pop(); }
    break;

  case 67:
#line 378 "mta_parse.y"
    {
            (yyvsp[-1].meth)->type = (yyvsp[-2].typ); SETDESC((yyvsp[-1].meth),(yyvsp[0].chr)); (yyval.meth) = (yyvsp[-1].meth); }
    break;

  case 68:
#line 380 "mta_parse.y"
    {
            (yyvsp[-1].meth)->type = &TA_int; SETDESC((yyvsp[-1].meth),(yyvsp[0].chr)); (yyval.meth) = (yyvsp[-1].meth); }
    break;

  case 69:
#line 384 "mta_parse.y"
    {
            (yyvsp[-1].meth)->is_static = true; /* consider these to be static functions */
            (yyvsp[-1].meth)->fun_argc = (yyvsp[0].rval); (yyvsp[-1].meth)->arg_types.size = (yyvsp[0].rval); mta->burp_fundefn = true; }
    break;

  case 73:
#line 394 "mta_parse.y"
    { if((yyvsp[-1].enm) != NULL) SETENUMDESC((yyvsp[-1].enm),(yyvsp[0].chr)); }
    break;

  case 76:
#line 399 "mta_parse.y"
    { /* trying to do some math */
           (yyval.enm) = NULL;
	   mta->skiptocommarb(); }
    break;

  case 77:
#line 404 "mta_parse.y"
    {
            mta->cur_enum->enum_vals.Add((yyvsp[0].enm));
	    mta->enum_stack.Pop(); }
    break;

  case 78:
#line 407 "mta_parse.y"
    { /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add((yyvsp[-2].enm)); if((yyvsp[0].rval) != -424242) (yyvsp[-2].enm)->enum_no = (yyvsp[0].rval);
	    mta->enum_stack.Pop(); }
    break;

  case 80:
#line 414 "mta_parse.y"
    {
	  if(((yyvsp[-2].rval) != -424242) && ((yyvsp[0].rval) != -424242))  (yyval.rval) = (yyvsp[-2].rval) + (yyvsp[0].rval);
	  else if((yyvsp[-2].rval) != -424242)	(yyval.rval) = (yyvsp[-2].rval);
	  else if((yyvsp[0].rval) != -424242)	(yyval.rval) = (yyvsp[0].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 81:
#line 419 "mta_parse.y"
    {
	  if(((yyvsp[-2].rval) != -424242) && ((yyvsp[0].rval) != -424242)) (yyval.rval) = (yyvsp[-2].rval) - (yyvsp[0].rval);
	  else if((yyvsp[-2].rval) != -424242)	(yyval.rval) = (yyvsp[-2].rval);
	  else if((yyvsp[0].rval) != -424242)	(yyval.rval) = (yyvsp[0].rval);
	  else				(yyval.rval) = -424242; }
    break;

  case 82:
#line 427 "mta_parse.y"
    { (yyval.rval) = -424242; }
    break;

  case 84:
#line 432 "mta_parse.y"
    { (yyval.enm) = new EnumDef((yyvsp[0].chr)); mta->enum_stack.Push((yyval.enm)); }
    break;

  case 85:
#line 435 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 86:
#line 436 "mta_parse.y"
    { mta->Class_UpdateLastPtrs(); }
    break;

  case 87:
#line 439 "mta_parse.y"
    {
            if((yyvsp[0].memb) != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !((yyvsp[0].memb)->HasOption("IGNORE"))
		 && !((yyvsp[0].memb)->type->DerivesFrom(TA_const))) {
		mta->cur_class->members.AddUniqNameNew((yyvsp[0].memb));
		if(mta->verbose >= 3)
		  cerr << "member: " << (yyvsp[0].memb)->name << " added to: "
		       << mta->cur_class->name << "\n"; } }
	    mta->memb_stack.Pop(); (yyval.typ) = NULL; }
    break;

  case 88:
#line 448 "mta_parse.y"
    {
            if((yyvsp[0].meth) != NULL) {
	      if(mta->cur_mstate == MTA::pblc) {
		if((yyvsp[0].meth)->HasOption("IGNORE"))
		  mta->cur_class->ignore_meths.AddUnique((yyvsp[0].meth)->name);
		else {
		  mta->cur_class->methods.AddUniqNameNew((yyvsp[0].meth));
		  if(mta->verbose >= 3)
		    cerr << "method: " << (yyvsp[0].meth)->name << " added to: "
			 << mta->cur_class->name << "\n"; } } }
	    else {
	      mta->cur_meth = NULL; }
	    mta->meth_stack.Pop(); (yyval.typ) = NULL; }
    break;

  case 89:
#line 461 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[0].typ));
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 90:
#line 464 "mta_parse.y"
    {
	    mta->cur_class->sub_types.AddUniqNameNew((yyvsp[0].typ));
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 91:
#line 467 "mta_parse.y"
    { /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew((yyvsp[-1].typ));
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; (yyval.typ) = NULL; }
    break;

  case 92:
#line 471 "mta_parse.y"
    { (yyval.typ) = NULL; }
    break;

  case 94:
#line 476 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::pblc; }
    break;

  case 95:
#line 477 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prvt; }
    break;

  case 96:
#line 478 "mta_parse.y"
    { (yyval.memb) = NULL; mta->cur_mstate = MTA::prot; }
    break;

  case 97:
#line 479 "mta_parse.y"
    {
	    (yyval.memb) = NULL;
	    if(mta->last_memb != NULL) {
	      SETDESC(mta->last_memb, (yyvsp[0].chr));
	      if(mta->last_memb->HasOption("IGNORE"))
		mta->cur_class->members.Remove(mta->last_memb); }
	    else if(mta->last_meth != NULL) {
	      SETDESC(mta->last_meth, (yyvsp[0].chr));
	      if(mta->last_meth->HasOption("IGNORE")) {
		mta->cur_class->ignore_meths.AddUnique(mta->last_meth->name);
		mta->cur_class->methods.Remove(mta->last_meth);
		mta->last_meth = NULL; }
	      else if((mta->last_meth->opts.size > 0) || (mta->last_meth->lists.size > 0)) {
		mta->cur_class->methods.AddUniqNameNew(mta->last_meth);
		if(mta->verbose >= 3)
		  cerr << "method: " << mta->last_meth->name << " added to: "
		       << mta->cur_class->name << "\n"; } } }
    break;

  case 99:
#line 500 "mta_parse.y"
    {
	  (yyval.memb) = (yyvsp[0].memb); if((yyvsp[0].memb) != NULL) (yyvsp[0].memb)->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
    break;

  case 100:
#line 506 "mta_parse.y"
    { (yyval.memb) = (yyvsp[-1].memb); }
    break;

  case 101:
#line 507 "mta_parse.y"
    { }
    break;

  case 102:
#line 508 "mta_parse.y"
    {
	    (yyval.memb) = (yyvsp[-2].memb);
	    String nm = (yyvsp[-3].typ)->name + "_ary";
	    TypeDef* nty = new TypeDef((char*)nm, true, (yyvsp[-3].typ)->ptr + 1);
	    nty->AddParFormal(&TA_ta_array); nty->AddParent((yyvsp[-3].typ));
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[-3].typ));
	    TypeDef* uty = sp->AddUniqNameOld(nty); (yyvsp[-2].memb)->type = uty;
	    if(uty == nty) mta->TypeAdded("array", sp, uty); }
    break;

  case 103:
#line 516 "mta_parse.y"
    { (yyvsp[-2].memb)->type = (yyvsp[-3].typ); (yyval.memb) = (yyvsp[-2].memb); }
    break;

  case 104:
#line 520 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[0].memb)->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[0].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 105:
#line 524 "mta_parse.y"
    {
	     if((mta->cur_mstate == MTA::pblc) && !((yyvsp[0].memb)->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew((yyvsp[0].memb));
             mta->memb_stack.Pop(); (yyval.memb) = NULL; }
    break;

  case 106:
#line 530 "mta_parse.y"
    {
            (yyval.memb) = new MemberDef((yyvsp[0].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
            if(mta->cur_memb_type != NULL) (yyval.memb)->type = mta->cur_memb_type;
            else (yyval.memb)->type = &TA_int; }
    break;

  case 107:
#line 536 "mta_parse.y"
    {
	    (yyval.memb) = new MemberDef((yyvsp[-1].chr)); mta->cur_memb = (yyval.memb); mta->memb_stack.Push((yyval.memb));
	    (yyval.memb)->fun_ptr = 1; }
    break;

  case 109:
#line 542 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 110:
#line 543 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 111:
#line 544 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 112:
#line 545 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 113:
#line 546 "mta_parse.y"
    { (yyval.meth) = NULL; mta->thisname = false; }
    break;

  case 114:
#line 547 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 115:
#line 548 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 116:
#line 549 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 117:
#line 550 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 118:
#line 551 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 119:
#line 552 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[0].chr);
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew((yyvsp[-2].meth)); (yyvsp[-2].meth)->type = (yyvsp[-3].typ);
	      mta->meth_stack.Pop();  (yyvsp[-2].meth)->fun_argc = (yyvsp[-1].rval); (yyvsp[-2].meth)->arg_types.size = (yyvsp[-1].rval);
	      (yyvsp[-2].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[-2].meth),(yyvsp[0].chr)); }
	    else { (yyval.meth) = NULL; mta->meth_stack.Pop(); } }
    break;

  case 120:
#line 560 "mta_parse.y"
    {
	    (yyval.meth) = NULL; String tmp = (yyvsp[0].chr);
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew((yyvsp[-2].meth)); (yyvsp[-2].meth)->type = &TA_int;
	      mta->meth_stack.Pop();  (yyvsp[-2].meth)->fun_argc = (yyvsp[-1].rval); (yyvsp[-2].meth)->arg_types.size = (yyvsp[-1].rval);
	      (yyvsp[-2].meth)->is_static = true; /* consider these to be static functions */
	      SETDESC((yyvsp[-2].meth),(yyvsp[0].chr)); }
	    else { (yyval.meth) = 0; mta->meth_stack.Pop(); } }
    break;

  case 122:
#line 572 "mta_parse.y"
    { (yyval.meth) = (yyvsp[0].meth); (yyvsp[0].meth)->is_static = true; }
    break;

  case 123:
#line 573 "mta_parse.y"
    { (yyval.meth) = (yyvsp[0].meth);  if((yyvsp[0].meth) != NULL) (yyvsp[0].meth)->is_static = true;
	  else if(mta->cur_meth) mta->cur_meth->is_static = true; }
    break;

  case 124:
#line 578 "mta_parse.y"
    { (yyval.meth) = (yyvsp[-1].meth); (yyvsp[-1].meth)->type = (yyvsp[-2].typ); SETDESC((yyvsp[-1].meth),(yyvsp[0].chr)); }
    break;

  case 125:
#line 579 "mta_parse.y"
    { (yyvsp[-1].meth)->type = &TA_int; SETDESC((yyvsp[-1].meth),(yyvsp[0].chr)); }
    break;

  case 126:
#line 580 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 127:
#line 581 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 128:
#line 582 "mta_parse.y"
    { (yyval.meth) = NULL; }
    break;

  case 129:
#line 585 "mta_parse.y"
    {
             (yyvsp[-1].meth)->fun_argc = (yyvsp[0].rval); (yyvsp[-1].meth)->arg_types.size = (yyvsp[0].rval); mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if((yyvsp[-1].meth)->fun_argd > (yyvsp[-1].meth)->fun_argc) (yyvsp[-1].meth)->fun_argd = -1; }
    break;

  case 130:
#line 592 "mta_parse.y"
    {
            (yyval.meth) = new MethodDef((yyvsp[0].chr)); mta->cur_meth = (yyval.meth); mta->meth_stack.Push((yyval.meth)); }
    break;

  case 131:
#line 596 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 132:
#line 597 "mta_parse.y"
    { (yyval.chr) = (yyvsp[0].chr); }
    break;

  case 133:
#line 598 "mta_parse.y"
    { (yyval.chr) = (yyvsp[-1].chr); }
    break;

  case 134:
#line 599 "mta_parse.y"
    { (yyval.chr) = (yyvsp[-1].chr); }
    break;

  case 135:
#line 600 "mta_parse.y"
    { (yyval.chr) = (yyvsp[-1].chr); }
    break;

  case 136:
#line 601 "mta_parse.y"
    { (yyval.chr) = (yyvsp[-2].chr); }
    break;

  case 137:
#line 605 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 138:
#line 606 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 139:
#line 607 "mta_parse.y"
    { (yyval.chr) = (yyvsp[-1].chr); }
    break;

  case 140:
#line 611 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 141:
#line 612 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 142:
#line 613 "mta_parse.y"
    { (yyval.chr) = ""; }
    break;

  case 143:
#line 616 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 144:
#line 617 "mta_parse.y"
    { (yyval.rval) = (yyvsp[-1].rval); }
    break;

  case 145:
#line 620 "mta_parse.y"
    { (yyval.rval) = 0; }
    break;

  case 146:
#line 621 "mta_parse.y"
    { (yyval.rval) = (yyvsp[-1].rval); }
    break;

  case 147:
#line 624 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 148:
#line 625 "mta_parse.y"
    { (yyval.rval) = (yyvsp[-2].rval) + 1; }
    break;

  case 149:
#line 626 "mta_parse.y"
    { (yyval.rval) = (yyvsp[-3].rval); }
    break;

  case 150:
#line 629 "mta_parse.y"
    {
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
    break;

  case 151:
#line 631 "mta_parse.y"
    {
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_defs.Add((yyvsp[0].chr));
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
    break;

  case 152:
#line 639 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[0].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 153:
#line 643 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[0].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[-1].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 154:
#line 647 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String((yyvsp[-1].chr)) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[-2].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 155:
#line 651 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = String("(*") + String((yyvsp[-2].chr)) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link((yyvsp[-5].typ)); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 156:
#line 655 "mta_parse.y"
    {
	    (yyval.rval) = 1; String nm = (yyvsp[0].chr);
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
    break;

  case 160:
#line 670 "mta_parse.y"
    {
            mta->thisname = true; mta->constcoln = false; }
    break;

  case 161:
#line 674 "mta_parse.y"
    { mta->constcoln = true; }
    break;

  case 162:
#line 677 "mta_parse.y"
    { (yyval.rval) = 1; }
    break;

  case 163:
#line 678 "mta_parse.y"
    { (yyval.rval) = (yyvsp[-1].rval) + 1; }
    break;

  case 164:
#line 681 "mta_parse.y"
    { mta->cur_memb_type = (yyvsp[0].typ); }
    break;

  case 166:
#line 685 "mta_parse.y"
    { (yyval.typ) = &TA_int; }
    break;

  case 167:
#line 686 "mta_parse.y"
    { (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 168:
#line 689 "mta_parse.y"
    { (yyval.typ) = new TypeDef((yyvsp[0].chr)); mta->type_stack.Push((yyval.typ)); }
    break;

  case 170:
#line 693 "mta_parse.y"
    {
	    String nm = (yyvsp[-1].typ)->name + "_ref";
	    TypeDef* nty = new TypeDef((char*)nm, true, (yyvsp[-1].typ)->ptr, true);
	    nty->AddParent((yyvsp[-1].typ));
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[-1].typ));
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) mta->TypeAdded("ref", sp, (yyval.typ)); }
    break;

  case 172:
#line 704 "mta_parse.y"
    {
 	    int i; String nm = (yyvsp[-1].typ)->name; for(i=0; i<(yyvsp[0].rval); i++) nm += "_ptr";
	    TypeDef* nty = new TypeDef((char*)nm, true, (yyvsp[0].rval)); nty->AddParent((yyvsp[-1].typ));
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[-1].typ));
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) mta->TypeAdded("ptr", sp, (yyval.typ)); }
    break;

  case 174:
#line 713 "mta_parse.y"
    {
	    String nm = (yyvsp[-1].typ)->name + "_" + (yyvsp[0].typ)->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = (yyvsp[0].typ)->size; nty->AddParent((yyvsp[-1].typ)); nty->AddParent((yyvsp[0].typ));
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[0].typ));
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) mta->TypeAdded("const", sp, (yyval.typ)); }
    break;

  case 176:
#line 723 "mta_parse.y"
    { (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 177:
#line 724 "mta_parse.y"
    { (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 178:
#line 725 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[-2].typ)->sub_types.FindName((yyvsp[0].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 179:
#line 729 "mta_parse.y"
    {
	    TypeDef* td; if((td = (yyvsp[-2].typ)->sub_types.FindName((yyvsp[0].chr))) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    (yyval.typ) = td; }
    break;

  case 180:
#line 733 "mta_parse.y"
    { (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 181:
#line 734 "mta_parse.y"
    { (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 182:
#line 735 "mta_parse.y"
    { (yyval.typ) = (yyvsp[0].typ); }
    break;

  case 184:
#line 737 "mta_parse.y"
    { /* a template */
 	    if(!((yyvsp[-3].typ)->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    if(((yyvsp[-1].typ)->owner != NULL) && ((yyvsp[-1].typ)->owner->owner != NULL))
	      (yyval.typ) = (yyvsp[-3].typ);	/* don't allow internal types with external templates */
	    else {
	      String nm = (yyvsp[-3].typ)->GetTemplName(mta->cur_templ_pars);
	      TypeDef* td;
	      int lx_tok;
	      if((td = mta->FindName(nm, lx_tok)) == NULL) {
		td = (yyvsp[-3].typ)->Clone(); td->name = nm;
		td->SetTemplType((yyvsp[-3].typ), mta->cur_templ_pars);
		TypeSpace* sp = mta->GetTypeSpace((yyvsp[-3].typ));
 		(yyval.typ) = sp->AddUniqNameOld(td);
		if((yyval.typ) == td) mta->TypeAdded("template instance", sp, (yyval.typ)); }
	      else
		(yyval.typ) = td; } }
    break;

  case 185:
#line 754 "mta_parse.y"
    { /* this template */
	    if(!((yyvsp[-3].typ)->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    (yyval.typ) = (yyvsp[-3].typ); }
    break;

  case 187:
#line 761 "mta_parse.y"
    {
	    String nm = (yyvsp[-1].typ)->name + "_" + (yyvsp[0].typ)->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = (yyvsp[0].typ)->size; nty->AddParent((yyvsp[-1].typ)); nty->AddParent((yyvsp[0].typ));
	    TypeSpace* sp = mta->GetTypeSpace((yyvsp[0].typ));
	    (yyval.typ) = sp->AddUniqNameOld(nty);
	    if((yyval.typ) == nty) mta->TypeAdded("combo", sp, (yyval.typ)); }
    break;

  case 188:
#line 771 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[0].typ)); }
    break;

  case 189:
#line 772 "mta_parse.y"
    { mta->cur_templ_pars.Link((yyvsp[0].typ)); (yyval.typ) = (yyvsp[-2].typ); }
    break;

  case 191:
#line 776 "mta_parse.y"
    { (yyval.chr) = (yyvsp[0].typ)->name; }
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 2626 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
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
	  int yychecklim = YYLAST - yyn;
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
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
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
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
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
	  yydestruct ("Error: discarding", yytoken, &yylval);
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
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
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


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
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
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 808 "mta_parse.y"


	/* end of grammar */

void yyerror(char *s) { 	/* called for yacc syntax error */
  int i;
  if((mta->verbose < 1) && (mta->spc != &(mta->spc_target)))
    return;

  if(strcmp(s, "parse error") == 0) {
    cerr << "Syntax Error, line " << mta->st_line << ":\t" << MTA::LastLn << "\n";
    cerr << "\t\t\t";
    for(i=0; i < mta->st_col + 1; i++)
      cerr << " ";
    cerr << "^\n";
  }
  else {
    cerr << s << "line " << mta->st_line << ":\t" << MTA::LastLn << "\n";
  }
}

