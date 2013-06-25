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

/* Substitute the variable and function names.  */
#define yyparse apparse
#define yylex   aplex
#define yyerror aperror
#define yylval  aplval
#define yychar  apchar
#define yydebug apdebug
#define yynerrs apnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     AP_CHUNK_TYPE = 258,
     AP_PROD = 259,
     AP_ISA = 260,
     AP_CLEAR_ALL = 261,
     AP_DEFINE_MODEL = 262,
     AP_ADD_DM = 263,
     AP_SGP = 264,
     AP_GOAL_FOCUS = 265,
     AP_SPP = 266,
     AP_NUMBER = 267,
     AP_NAME = 268,
     AP_STRING = 269,
     AP_DEFVAR = 270,
     AP_DEFMETHOD = 271,
     AP_DEFUN = 272,
     AP_SETF = 273
   };
#endif
/* Tokens.  */
#define AP_CHUNK_TYPE 258
#define AP_PROD 259
#define AP_ISA 260
#define AP_CLEAR_ALL 261
#define AP_DEFINE_MODEL 262
#define AP_ADD_DM 263
#define AP_SGP 264
#define AP_GOAL_FOCUS 265
#define AP_SPP 266
#define AP_NUMBER 267
#define AP_NAME 268
#define AP_STRING 269
#define AP_DEFVAR 270
#define AP_DEFMETHOD 271
#define AP_DEFUN 272
#define AP_SETF 273




/* Copy the first part of user declarations.  */
#line 20 "actr_parse.y"


#include <ActrModel>
#include <ActrDeclarativeModule>
#include <ActrProceduralModule>
#include <ActrGoalModule>
#include <ActrSlot>

#include <taMisc>

#define AMCP    ActrModel::cur_parse

void aperror(const char *s);
int aplex();



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
#line 40 "actr_parse.y"
{
  const char*     chr;
  int	   	  rval;
  double          num; 
  ActrChunkType*  chtyp;
  ActrChunk*      chk;
  ActrProduction* prod;
  ActrSlotType*   sltyp;
  ActrSlot*       slt;
}
/* Line 193 of yacc.c.  */
#line 168 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 181 "y.tab.c"

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
#define YYLAST   195

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  30
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  104
/* YYNRULES -- Number of states.  */
#define YYNSTATES  181

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   273

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    26,     2,     2,     2,     2,     2,     2,
      20,    21,    19,    29,     2,    27,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    22,     2,
      28,    23,    24,    25,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    19,    22,
      25,    28,    31,    34,    37,    41,    45,    50,    52,    55,
      58,    61,    67,    70,    74,    78,    80,    83,    85,    90,
      92,    95,    98,   102,   107,   110,   113,   115,   118,   121,
     124,   126,   134,   138,   143,   148,   154,   156,   159,   162,
     165,   169,   173,   177,   181,   184,   186,   189,   192,   195,
     199,   204,   208,   212,   216,   220,   225,   230,   235,   240,
     242,   244,   247,   250,   252,   256,   259,   261,   264,   268,
     272,   276,   280,   283,   285,   288,   291,   294,   298,   301,
     305,   307,   310,   312,   314,   316,   319,   324,   332,   334,
     337,   340,   343,   346,   348
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      31,     0,    -1,    -1,    31,    32,    -1,    31,    33,    -1,
      31,    34,    -1,    31,    38,    -1,    31,    42,    -1,    31,
      50,    -1,    31,    70,    -1,    31,    71,    -1,    31,    73,
      -1,    31,    21,    -1,    31,     1,    -1,    20,     6,    21,
      -1,    20,     7,    13,    -1,    20,     9,    35,    21,    -1,
      36,    -1,    35,    36,    -1,    37,    13,    -1,    37,    74,
      -1,    37,    20,    74,    74,    21,    -1,    22,    13,    -1,
      39,    40,    21,    -1,    20,     3,    13,    -1,    41,    -1,
      40,    41,    -1,    13,    -1,    20,     8,    43,    21,    -1,
      44,    -1,    43,    44,    -1,    43,     1,    -1,    45,    46,
      21,    -1,    45,    46,    47,    21,    -1,    20,    13,    -1,
       5,    13,    -1,    48,    -1,    47,    48,    -1,    49,    13,
      -1,    49,    74,    -1,    13,    -1,    51,    52,    23,    23,
      24,    59,    21,    -1,    20,     4,    13,    -1,    20,     4,
      19,    13,    -1,    20,     4,    13,    14,    -1,    20,     4,
      19,    13,    14,    -1,    53,    -1,    52,    53,    -1,    52,
       1,    -1,    54,    56,    -1,    54,    55,    56,    -1,    23,
      13,    24,    -1,    25,    13,    24,    -1,    26,    13,    26,
      -1,     5,    13,    -1,    57,    -1,    56,    57,    -1,    58,
      13,    -1,    58,    74,    -1,    58,    23,    13,    -1,    27,
      58,    23,    13,    -1,    24,    58,    13,    -1,    24,    58,
      74,    -1,    28,    58,    13,    -1,    28,    58,    74,    -1,
      24,    23,    58,    13,    -1,    24,    23,    58,    74,    -1,
      28,    23,    58,    13,    -1,    28,    23,    58,    74,    -1,
      13,    -1,    60,    -1,    59,    60,    -1,    59,     1,    -1,
      61,    -1,    61,    63,    64,    -1,    61,    64,    -1,    62,
      -1,    62,    66,    -1,    23,    13,    24,    -1,    29,    13,
      24,    -1,    27,    13,    24,    -1,    26,    13,    26,    -1,
       5,    13,    -1,    65,    -1,    64,    65,    -1,    68,    13,
      -1,    68,    74,    -1,    68,    23,    13,    -1,    69,    13,
      -1,    20,    67,    21,    -1,    13,    -1,    23,    13,    -1,
      14,    -1,    12,    -1,    13,    -1,    22,    13,    -1,    20,
      10,    13,    21,    -1,    20,    11,    72,    22,    13,    74,
      21,    -1,    13,    -1,    20,    15,    -1,    20,    16,    -1,
      20,    17,    -1,    20,    18,    -1,    12,    -1,    27,    12,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    75,    75,    79,    81,    83,    85,    87,    89,    91,
      93,    95,    97,    99,   104,   108,   111,   114,   115,   118,
     119,   120,   123,   126,   130,   137,   138,   141,   147,   150,
     151,   152,   155,   156,   159,   167,   176,   177,   180,   181,
     184,   187,   190,   194,   199,   204,   212,   213,   214,   217,
     218,   222,   226,   230,   237,   246,   247,   251,   253,   255,
     257,   259,   261,   263,   265,   267,   269,   271,   273,   277,
     280,   281,   282,   285,   286,   287,   288,   289,   293,   298,
     303,   311,   318,   327,   328,   332,   333,   334,   335,   338,
     341,   342,   343,   344,   347,   350,   353,   363,   367,   374,
     375,   376,   377,   380,   381
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "AP_CHUNK_TYPE", "AP_PROD", "AP_ISA",
  "AP_CLEAR_ALL", "AP_DEFINE_MODEL", "AP_ADD_DM", "AP_SGP",
  "AP_GOAL_FOCUS", "AP_SPP", "AP_NUMBER", "AP_NAME", "AP_STRING",
  "AP_DEFVAR", "AP_DEFMETHOD", "AP_DEFUN", "AP_SETF", "'*'", "'('", "')'",
  "':'", "'='", "'>'", "'?'", "'!'", "'-'", "'<'", "'+'", "$accept",
  "list", "clear_all", "define_model", "sgp", "sgp_params", "sgp_param",
  "sgp_param_nm", "chunktype", "chunktype_nm", "slots", "slot", "add_dm",
  "dm_items", "dm_item", "dm_item_nm", "dm_item_typ", "chunk_vals",
  "chunk_val", "chunk_slot", "prod", "prod_name", "prod_lhs", "prod_cond",
  "prod_cond_name", "prod_cond_type", "prod_cond_vals", "prod_cond_val",
  "cond_slot", "prod_rhs", "prod_act", "prod_act_name", "prod_act_bang",
  "prod_act_type", "prod_act_vals", "prod_act_val", "act_expr", "sub_exp",
  "act_slot", "act_param", "goal_focus", "spp_expr", "spp_prod",
  "def_ignore", "apnum", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,    42,
      40,    41,    58,    61,    62,    63,    33,    45,    60,    43
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    30,    31,    31,    31,    31,    31,    31,    31,    31,
      31,    31,    31,    31,    32,    33,    34,    35,    35,    36,
      36,    36,    37,    38,    39,    40,    40,    41,    42,    43,
      43,    43,    44,    44,    45,    46,    47,    47,    48,    48,
      49,    50,    51,    51,    51,    51,    52,    52,    52,    53,
      53,    54,    54,    54,    55,    56,    56,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    58,
      59,    59,    59,    60,    60,    60,    60,    60,    61,    61,
      61,    62,    63,    64,    64,    65,    65,    65,    65,    66,
      67,    67,    67,    67,    68,    69,    70,    71,    72,    73,
      73,    73,    73,    74,    74
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     3,     4,     1,     2,     2,
       2,     5,     2,     3,     3,     1,     2,     1,     4,     1,
       2,     2,     3,     4,     2,     2,     1,     2,     2,     2,
       1,     7,     3,     4,     4,     5,     1,     2,     2,     2,
       3,     3,     3,     3,     2,     1,     2,     2,     2,     3,
       4,     3,     3,     3,     3,     4,     4,     4,     4,     1,
       1,     2,     2,     1,     3,     2,     1,     2,     3,     3,
       3,     3,     2,     1,     2,     2,     2,     3,     2,     3,
       1,     2,     1,     1,     1,     2,     4,     7,     1,     2,
       2,     2,     2,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    13,     0,    12,     3,     4,     5,     6,
       0,     7,     8,     0,     9,    10,    11,     0,     0,     0,
       0,     0,     0,     0,     0,    99,   100,   101,   102,    27,
       0,    25,     0,     0,     0,     0,    46,     0,    24,    42,
       0,    14,    15,     0,     0,    29,     0,     0,     0,    17,
       0,     0,    98,     0,    23,    26,     0,     0,     0,    48,
       0,    47,     0,    69,     0,     0,     0,     0,    49,    55,
       0,    44,    43,    34,    31,    28,    30,     0,     0,    22,
      16,    18,   103,    19,     0,     0,    20,    96,     0,    51,
      52,    53,     0,    54,     0,     0,     0,     0,     0,    50,
      56,    57,     0,    58,    45,    35,    40,    32,     0,    36,
       0,     0,   104,     0,     0,     0,    61,    62,     0,     0,
      63,    64,    59,    33,    37,    38,    39,     0,     0,     0,
       0,     0,     0,     0,    70,    73,    76,    65,    66,    60,
      67,    68,    21,    97,     0,     0,     0,     0,    72,    41,
      71,     0,    94,     0,     0,    75,    83,     0,     0,     0,
      77,    78,    81,    80,    79,    82,    95,    74,    84,    85,
       0,    86,    88,    93,    90,    92,     0,     0,    87,    91,
      89
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     6,     7,     8,    48,    49,    50,     9,    10,
      30,    31,    11,    44,    45,    46,    78,   108,   109,   110,
      12,    13,    35,    36,    37,    67,    68,    69,    70,   133,
     134,   135,   136,   154,   155,   156,   160,   177,   157,   158,
      14,    15,    53,    16,    86
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -117
static const yytype_int16 yypact[] =
{
    -117,    12,  -117,  -117,    59,  -117,  -117,  -117,  -117,  -117,
      39,  -117,  -117,    82,  -117,  -117,  -117,    43,   106,    51,
      75,    70,    88,    79,    99,  -117,  -117,  -117,  -117,  -117,
      -2,  -117,   107,   110,   115,     4,  -117,    33,  -117,   116,
     118,  -117,  -117,   119,    16,  -117,   124,   120,   105,  -117,
      -4,   113,  -117,   114,  -117,  -117,   111,   117,   112,  -117,
      -3,  -117,   126,  -117,    81,   127,    90,    58,    58,  -117,
      66,  -117,   123,  -117,  -117,  -117,  -117,   129,     5,  -117,
    -117,  -117,  -117,  -117,    -6,   131,  -117,  -117,   132,  -117,
    -117,  -117,   122,  -117,   127,    46,   121,   127,    71,    58,
    -117,  -117,   134,  -117,  -117,  -117,  -117,  -117,    96,  -117,
      84,    -6,  -117,    -6,    95,    87,  -117,  -117,   135,    89,
    -117,  -117,  -117,  -117,  -117,  -117,  -117,   128,   130,   137,
     139,   140,   141,    21,  -117,     2,   136,  -117,  -117,  -117,
    -117,  -117,  -117,  -117,   133,   138,   142,   143,  -117,  -117,
    -117,   145,  -117,   146,    93,    93,  -117,    68,   147,    41,
    -117,  -117,  -117,  -117,  -117,  -117,  -117,    93,  -117,  -117,
     148,  -117,  -117,  -117,  -117,  -117,   149,   144,  -117,  -117,
    -117
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -117,  -117,  -117,  -117,  -117,  -117,   125,  -117,  -117,  -117,
    -117,   150,  -117,  -117,   151,  -117,  -117,  -117,    47,  -117,
    -117,  -117,  -117,   152,  -117,  -117,   101,   -64,   -63,  -117,
      30,  -117,  -117,  -117,    15,  -116,  -117,  -117,  -117,  -117,
    -117,  -117,  -117,  -117,   -70
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
     103,    95,    96,    98,   100,    59,    82,   151,    82,    83,
      56,    29,     2,     3,   111,   152,    84,    74,   106,    54,
      92,    85,   148,    85,   153,   117,   107,    60,   121,    33,
      34,   115,     4,     5,   119,   100,    43,    75,    62,   168,
     126,   127,   149,   128,   129,   138,    63,   130,   131,   141,
     132,   168,    29,   173,   174,   175,    38,    64,    82,   116,
      65,    66,    17,    18,   176,    19,    20,    21,    22,    23,
      24,    63,    41,    85,    25,    26,    27,    28,    82,   101,
      82,   169,    64,    82,   120,    65,    66,   171,    42,   102,
      43,   170,    51,    85,    63,    85,    82,   125,    85,    82,
     137,    82,   140,    63,    94,    32,   152,    33,    34,   106,
      47,    85,    52,    97,    85,   153,    85,   123,   129,    39,
      56,   130,   131,    57,   132,    40,    80,    47,    58,    77,
      71,    72,    73,    79,    87,    89,    88,   104,    91,    93,
      63,    90,   105,   112,   118,   113,   114,   122,   139,   142,
     144,   143,   145,   146,   147,   124,   159,   161,   165,   166,
     172,   178,   179,   150,   162,   180,   163,   164,    99,   167,
       0,     0,     0,    81,     0,     0,     0,     0,     0,     0,
      55,     0,     0,     0,     0,     0,     0,    61,     0,     0,
       0,     0,     0,     0,     0,    76
};

static const yytype_int16 yycheck[] =
{
      70,    64,    65,    66,    68,     1,    12,     5,    12,    13,
      13,    13,     0,     1,    84,    13,    20,     1,    13,    21,
      23,    27,     1,    27,    22,    95,    21,    23,    98,    25,
      26,    94,    20,    21,    97,    99,    20,    21,     5,   155,
     110,   111,    21,   113,    23,   115,    13,    26,    27,   119,
      29,   167,    13,    12,    13,    14,    13,    24,    12,    13,
      27,    28,     3,     4,    23,     6,     7,     8,     9,    10,
      11,    13,    21,    27,    15,    16,    17,    18,    12,    13,
      12,    13,    24,    12,    13,    27,    28,   157,    13,    23,
      20,    23,    13,    27,    13,    27,    12,    13,    27,    12,
      13,    12,    13,    13,    23,    23,    13,    25,    26,    13,
      22,    27,    13,    23,    27,    22,    27,    21,    23,    13,
      13,    26,    27,    13,    29,    19,    21,    22,    13,     5,
      14,    13,    13,    13,    21,    24,    22,    14,    26,    13,
      13,    24,    13,    12,    23,    13,    24,    13,    13,    21,
      13,    21,    13,    13,    13,   108,    20,    24,    13,    13,
      13,    13,    13,   133,    26,    21,    24,    24,    67,   154,
      -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    31,     0,     1,    20,    21,    32,    33,    34,    38,
      39,    42,    50,    51,    70,    71,    73,     3,     4,     6,
       7,     8,     9,    10,    11,    15,    16,    17,    18,    13,
      40,    41,    23,    25,    26,    52,    53,    54,    13,    13,
      19,    21,    13,    20,    43,    44,    45,    22,    35,    36,
      37,    13,    13,    72,    21,    41,    13,    13,    13,     1,
      23,    53,     5,    13,    24,    27,    28,    55,    56,    57,
      58,    14,    13,    13,     1,    21,    44,     5,    46,    13,
      21,    36,    12,    13,    20,    27,    74,    21,    22,    24,
      24,    26,    23,    13,    23,    58,    58,    23,    58,    56,
      57,    13,    23,    74,    14,    13,    13,    21,    47,    48,
      49,    74,    12,    13,    24,    58,    13,    74,    23,    58,
      13,    74,    13,    21,    48,    13,    74,    74,    74,    23,
      26,    27,    29,    59,    60,    61,    62,    13,    74,    13,
      13,    74,    21,    21,    13,    13,    13,    13,     1,    21,
      60,     5,    13,    22,    63,    64,    65,    68,    69,    20,
      66,    24,    26,    24,    24,    13,    13,    64,    65,    13,
      23,    74,    13,    12,    13,    14,    23,    67,    13,    13,
      21
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
#line 75 "actr_parse.y"
    {
            AMCP->ResetParse(); /* AMCP->load_state = ActrModel::YYRet_Exit; */
            /* taMisc::Info("nothing"); */
          }
    break;

  case 3:
#line 79 "actr_parse.y"
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 4:
#line 81 "actr_parse.y"
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 5:
#line 83 "actr_parse.y"
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 6:
#line 85 "actr_parse.y"
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 7:
#line 87 "actr_parse.y"
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 8:
#line 89 "actr_parse.y"
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 9:
#line 91 "actr_parse.y"
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 10:
#line 93 "actr_parse.y"
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 11:
#line 95 "actr_parse.y"
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 12:
#line 97 "actr_parse.y"
    { /* end of define model */
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 13:
#line 99 "actr_parse.y"
    {
            AMCP->ResetParse(); AMCP->load_state = ActrModel::YYRet_NoSrc;
            return AMCP->load_state; }
    break;

  case 14:
#line 104 "actr_parse.y"
    { /* assumed */
        }
    break;

  case 15:
#line 108 "actr_parse.y"
    { AMCP->name = (yyvsp[(3) - (3)].chr); AMCP->SigEmitUpdated(); }
    break;

  case 16:
#line 111 "actr_parse.y"
    { }
    break;

  case 19:
#line 118 "actr_parse.y"
    { AMCP->SetParam((yyvsp[(1) - (2)].chr), (yyvsp[(2) - (2)].chr)); }
    break;

  case 20:
#line 119 "actr_parse.y"
    { AMCP->SetParam((yyvsp[(1) - (2)].chr), (yyvsp[(2) - (2)].num)); }
    break;

  case 21:
#line 120 "actr_parse.y"
    { AMCP->SetParam((yyvsp[(1) - (5)].chr), (yyvsp[(3) - (5)].num), (yyvsp[(4) - (5)].num)); }
    break;

  case 22:
#line 123 "actr_parse.y"
    { AMCP->load_param = (yyvsp[(2) - (2)].chr); (yyval.chr) = AMCP->load_param; }
    break;

  case 23:
#line 126 "actr_parse.y"
    { }
    break;

  case 24:
#line 130 "actr_parse.y"
    {
           bool made_new = false;
           AMCP->load_chtype = AMCP->chunk_types.FindMakeNameType((yyvsp[(3) - (3)].chr), NULL, made_new);
           (yyval.chtyp) = AMCP->load_chtype;
         }
    break;

  case 26:
#line 138 "actr_parse.y"
    { (yyval.sltyp) = (yyvsp[(1) - (2)].sltyp); }
    break;

  case 27:
#line 141 "actr_parse.y"
    {
            bool made_new = false;
            (yyval.sltyp) = AMCP->load_chtype->slots.FindMakeNameType((yyvsp[(1) - (1)].chr), NULL, made_new);
          }
    break;

  case 28:
#line 147 "actr_parse.y"
    {   }
    break;

  case 32:
#line 155 "actr_parse.y"
    { }
    break;

  case 33:
#line 156 "actr_parse.y"
    { }
    break;

  case 34:
#line 159 "actr_parse.y"
    {
           ActrDeclarativeModule* dmod = AMCP->DeclarativeModule();
           bool made_new = false;
           AMCP->load_chunk = dmod->init_chunks.FindMakeNameType((yyvsp[(2) - (2)].chr), NULL, made_new);
           /*           AMCP->load_chunk->name = $2; // should set this! */
           (yyval.chk) = AMCP->load_chunk; }
    break;

  case 35:
#line 167 "actr_parse.y"
    {
           ActrChunkType* ct = AMCP->FindChunkType((yyvsp[(2) - (2)].chr));
           if(ct) {
             AMCP->load_chunk->SetChunkType(ct);
           }
           (yyval.chk) = AMCP->load_chunk; }
    break;

  case 38:
#line 180 "actr_parse.y"
    { if((yyvsp[(1) - (2)].slt)) (yyvsp[(1) - (2)].slt)->val = (yyvsp[(2) - (2)].chr); }
    break;

  case 39:
#line 181 "actr_parse.y"
    { if((yyvsp[(1) - (2)].slt)) (yyvsp[(1) - (2)].slt)->val = (String)(yyvsp[(2) - (2)].num); }
    break;

  case 40:
#line 184 "actr_parse.y"
    { (yyval.slt) = AMCP->load_chunk->FindSlot((yyvsp[(1) - (1)].chr)); }
    break;

  case 41:
#line 187 "actr_parse.y"
    { }
    break;

  case 42:
#line 190 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(3) - (3)].chr), NULL, made_new); }
    break;

  case 43:
#line 194 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(4) - (4)].chr), NULL, made_new);
           AMCP->load_prod->SetProdFlag(ActrProduction::PSTAR); }
    break;

  case 44:
#line 199 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(3) - (4)].chr), NULL, made_new);
           AMCP->load_prod->desc = (yyvsp[(4) - (4)].chr); }
    break;

  case 45:
#line 204 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(4) - (5)].chr), NULL, made_new);
           AMCP->load_prod->desc = (yyvsp[(5) - (5)].chr);
           AMCP->load_prod->SetProdFlag(ActrProduction::PSTAR); }
    break;

  case 49:
#line 217 "actr_parse.y"
    { }
    break;

  case 50:
#line 218 "actr_parse.y"
    { }
    break;

  case 51:
#line 222 "actr_parse.y"
    {
           AMCP->load_cond = (ActrCondition*)AMCP->load_prod->conds.New(1);
           AMCP->load_cond->src = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_cond->cond_src = ActrCondition::BUFFER_EQ; }
    break;

  case 52:
#line 226 "actr_parse.y"
    {
           AMCP->load_cond = (ActrCondition*)AMCP->load_prod->conds.New(1);
           AMCP->load_cond->src = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_cond->cond_src = ActrCondition::BUFFER_QUERY; }
    break;

  case 53:
#line 230 "actr_parse.y"
    {
            /* todo: need to skip expr */
            taMisc::Warning("lisp-specific condition not processed:", (yyvsp[(2) - (3)].chr));
           }
    break;

  case 54:
#line 237 "actr_parse.y"
    {
             ActrChunkType* ct = AMCP->FindChunkType((yyvsp[(2) - (2)].chr));
             if(ct) {
               AMCP->load_cond->cmp_chunk.SetChunkType(ct);
             }
           }
    break;

  case 57:
#line 251 "actr_parse.y"
    {
             AMCP->load_cond->SetVal((yyvsp[(1) - (2)].slt), (yyvsp[(2) - (2)].chr)); }
    break;

  case 58:
#line 253 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(1) - (2)].slt), (String)(yyvsp[(2) - (2)].num)); }
    break;

  case 59:
#line 255 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(1) - (3)].slt), String("=") + (yyvsp[(3) - (3)].chr)); }
    break;

  case 60:
#line 257 "actr_parse.y"
    { /* - only allowed w/ variables? */
           AMCP->load_cond->SetVal((yyvsp[(2) - (4)].slt), String("=") + (yyvsp[(4) - (4)].chr) + String("-")); }
    break;

  case 61:
#line 259 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(2) - (3)].slt), (yyvsp[(3) - (3)].chr), Relation::GREATERTHAN); }
    break;

  case 62:
#line 261 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(2) - (3)].slt), (String)(yyvsp[(3) - (3)].num), Relation::GREATERTHAN); }
    break;

  case 63:
#line 263 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(2) - (3)].slt), (yyvsp[(3) - (3)].chr), Relation::LESSTHAN); }
    break;

  case 64:
#line 265 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(2) - (3)].slt), (String)(yyvsp[(3) - (3)].num), Relation::LESSTHAN); }
    break;

  case 65:
#line 267 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(3) - (4)].slt), (yyvsp[(4) - (4)].chr), Relation::GREATERTHANOREQUAL); }
    break;

  case 66:
#line 269 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(3) - (4)].slt), (String)(yyvsp[(4) - (4)].num), Relation::GREATERTHANOREQUAL); }
    break;

  case 67:
#line 271 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(3) - (4)].slt), (yyvsp[(4) - (4)].chr), Relation::LESSTHANOREQUAL); }
    break;

  case 68:
#line 273 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(3) - (4)].slt), (String)(yyvsp[(4) - (4)].num), Relation::LESSTHANOREQUAL); }
    break;

  case 69:
#line 277 "actr_parse.y"
    { (yyval.slt) = AMCP->load_cond->cmp_chunk.FindSlot((yyvsp[(1) - (1)].chr)); }
    break;

  case 74:
#line 286 "actr_parse.y"
    { }
    break;

  case 75:
#line 287 "actr_parse.y"
    { }
    break;

  case 76:
#line 288 "actr_parse.y"
    { }
    break;

  case 77:
#line 289 "actr_parse.y"
    { }
    break;

  case 78:
#line 293 "actr_parse.y"
    {
           AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
           AMCP->load_act->dest = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_act->action = ActrAction::UPDATE; 
           AMCP->load_act->UpdateAfterEdit_NoGui(); }
    break;

  case 79:
#line 298 "actr_parse.y"
    {
           AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
           AMCP->load_act->dest = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_act->action = ActrAction::REQUEST;
           AMCP->load_act->UpdateAfterEdit_NoGui(); }
    break;

  case 80:
#line 303 "actr_parse.y"
    {
           AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
           AMCP->load_act->dest = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_act->action = ActrAction::CLEAR;
           AMCP->load_act->UpdateAfterEdit_NoGui(); }
    break;

  case 81:
#line 311 "actr_parse.y"
    {
             AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
             AMCP->load_act->SetBangAction((yyvsp[(2) - (3)].chr));
             AMCP->load_act->UpdateAfterEdit_NoGui(); }
    break;

  case 82:
#line 318 "actr_parse.y"
    {
             ActrChunkType* ct = AMCP->FindChunkType((yyvsp[(2) - (2)].chr));
             if(ct) {
               AMCP->load_act->chunk.SetChunkType(ct);
             }
           }
    break;

  case 85:
#line 332 "actr_parse.y"
    { if((yyvsp[(1) - (2)].slt)) (yyvsp[(1) - (2)].slt)->val = (yyvsp[(2) - (2)].chr); }
    break;

  case 86:
#line 333 "actr_parse.y"
    { if((yyvsp[(1) - (2)].slt)) (yyvsp[(1) - (2)].slt)->val = (String)(yyvsp[(2) - (2)].num); }
    break;

  case 87:
#line 334 "actr_parse.y"
    { if((yyvsp[(1) - (3)].slt)) (yyvsp[(1) - (3)].slt)->val = String("=") + (yyvsp[(3) - (3)].chr); }
    break;

  case 88:
#line 335 "actr_parse.y"
    { AMCP->load_act->params += (yyvsp[(2) - (2)].chr); }
    break;

  case 90:
#line 341 "actr_parse.y"
    { AMCP->load_act->val += (yyvsp[(1) - (1)].chr); }
    break;

  case 91:
#line 342 "actr_parse.y"
    { AMCP->load_act->val += String("=") + (yyvsp[(2) - (2)].chr); }
    break;

  case 92:
#line 343 "actr_parse.y"
    { AMCP->load_act->val += (yyvsp[(1) - (1)].chr); }
    break;

  case 93:
#line 344 "actr_parse.y"
    { AMCP->load_act->val += (String)(yyvsp[(1) - (1)].num); }
    break;

  case 94:
#line 347 "actr_parse.y"
    { (yyval.slt) = AMCP->load_act->chunk.FindSlot((yyvsp[(1) - (1)].chr)); }
    break;

  case 95:
#line 350 "actr_parse.y"
    { AMCP->load_act->params = (yyvsp[(2) - (2)].chr); }
    break;

  case 96:
#line 353 "actr_parse.y"
    {
              ActrDeclarativeModule* dmod = AMCP->DeclarativeModule();
              ActrGoalModule* gmod = AMCP->GoalModule();
              ActrChunk* gf = dmod->init_chunks.FindName((yyvsp[(3) - (4)].chr));
              if(!gf) { taMisc::Error("goal-focus cannot find declarative init chunk:", (yyvsp[(3) - (4)].chr)); }
              else    { ActrChunk* ngf = (ActrChunk*)gf->Clone(); ngf->name = gf->name; 
                        gmod->init_chunk.Add(ngf); }
            }
    break;

  case 97:
#line 363 "actr_parse.y"
    {
           AMCP->load_prod->SetParam((yyvsp[(5) - (7)].chr), (yyvsp[(6) - (7)].num)); }
    break;

  case 98:
#line 367 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(1) - (1)].chr), NULL, made_new); }
    break;

  case 104:
#line 381 "actr_parse.y"
    { (yyval.num) = -(yyvsp[(2) - (2)].num); }
    break;


/* Line 1267 of yacc.c.  */
#line 2047 "y.tab.c"
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


#line 384 "actr_parse.y"


	/* end of grammar */

void aperror(const char *s) { 	/* called for yacc syntax error */
  AMCP->ParseErr(s);
}

