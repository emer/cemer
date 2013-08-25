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
     AP_DEF_CHUNKS = 264,
     AP_SGP = 265,
     AP_GOAL_FOCUS = 266,
     AP_SPP = 267,
     AP_NUMBER = 268,
     AP_NAME = 269,
     AP_STRING = 270,
     AP_BANG_EXPR = 271,
     AP_DEFVAR = 272,
     AP_DEFMETHOD = 273,
     AP_DEFUN = 274,
     AP_SETF = 275
   };
#endif
/* Tokens.  */
#define AP_CHUNK_TYPE 258
#define AP_PROD 259
#define AP_ISA 260
#define AP_CLEAR_ALL 261
#define AP_DEFINE_MODEL 262
#define AP_ADD_DM 263
#define AP_DEF_CHUNKS 264
#define AP_SGP 265
#define AP_GOAL_FOCUS 266
#define AP_SPP 267
#define AP_NUMBER 268
#define AP_NAME 269
#define AP_STRING 270
#define AP_BANG_EXPR 271
#define AP_DEFVAR 272
#define AP_DEFMETHOD 273
#define AP_DEFUN 274
#define AP_SETF 275




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
#line 172 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 185 "y.tab.c"

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
#define YYLAST   203

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  54
/* YYNRULES -- Number of rules.  */
#define YYNRULES  120
/* YYNRULES -- Number of states.  */
#define YYNSTATES  212

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   275

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    30,     2,     2,     2,     2,     2,     2,
      22,    23,    21,    31,     2,    27,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    24,     2,
      28,    25,    26,    29,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    19,    22,
      25,    28,    31,    34,    37,    40,    44,    48,    53,    55,
      58,    61,    64,    70,    73,    77,    81,    83,    86,    88,
      93,    95,    98,   101,   105,   110,   113,   116,   118,   121,
     124,   127,   129,   134,   136,   139,   142,   146,   151,   154,
     162,   166,   171,   176,   182,   184,   187,   190,   193,   196,
     200,   202,   205,   207,   210,   214,   217,   219,   222,   225,
     229,   233,   237,   242,   247,   249,   251,   254,   256,   260,
     262,   265,   268,   272,   276,   281,   283,   287,   289,   292,
     295,   297,   301,   304,   306,   309,   313,   317,   321,   325,
     328,   330,   333,   336,   340,   344,   348,   353,   358,   361,
     363,   365,   368,   373,   381,   383,   386,   389,   392,   395,
     397
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      33,     0,    -1,    -1,    33,    34,    -1,    33,    35,    -1,
      33,    36,    -1,    33,    40,    -1,    33,    44,    -1,    33,
      52,    -1,    33,    56,    -1,    33,    81,    -1,    33,    82,
      -1,    33,    84,    -1,    33,    23,    -1,    33,     1,    -1,
      22,     6,    23,    -1,    22,     7,    14,    -1,    22,    10,
      37,    23,    -1,    38,    -1,    37,    38,    -1,    39,    14,
      -1,    39,    85,    -1,    39,    22,    85,    85,    23,    -1,
      24,    14,    -1,    41,    42,    23,    -1,    22,     3,    14,
      -1,    43,    -1,    42,    43,    -1,    14,    -1,    22,     8,
      45,    23,    -1,    46,    -1,    45,    46,    -1,    45,     1,
      -1,    47,    48,    23,    -1,    47,    48,    49,    23,    -1,
      22,    14,    -1,     5,    14,    -1,    50,    -1,    49,    50,
      -1,    51,    14,    -1,    51,    85,    -1,    14,    -1,    22,
       9,    53,    23,    -1,    54,    -1,    53,    54,    -1,    53,
       1,    -1,    55,    48,    23,    -1,    55,    48,    49,    23,
      -1,    22,    14,    -1,    57,    58,    25,    25,    26,    71,
      23,    -1,    22,     4,    14,    -1,    22,     4,    21,    14,
      -1,    22,     4,    14,    15,    -1,    22,     4,    21,    14,
      15,    -1,    59,    -1,    58,    59,    -1,    58,     1,    -1,
      60,    62,    -1,    60,    61,    -1,    60,    61,    62,    -1,
      66,    -1,    66,    67,    -1,    70,    -1,    70,    78,    -1,
      25,    14,    26,    -1,     5,    14,    -1,    63,    -1,    62,
      63,    -1,    64,    65,    -1,    27,    64,    65,    -1,    26,
      64,    65,    -1,    28,    64,    65,    -1,    26,    25,    64,
      65,    -1,    28,    25,    64,    65,    -1,    14,    -1,    14,
      -1,    25,    14,    -1,    85,    -1,    29,    14,    26,    -1,
      68,    -1,    67,    68,    -1,    69,    14,    -1,    27,    69,
      14,    -1,    69,    25,    14,    -1,    27,    69,    25,    14,
      -1,    14,    -1,    30,    14,    30,    -1,    72,    -1,    71,
      72,    -1,    71,     1,    -1,    73,    -1,    73,    75,    76,
      -1,    73,    76,    -1,    74,    -1,    74,    78,    -1,    25,
      14,    26,    -1,    31,    14,    26,    -1,    27,    14,    26,
      -1,    30,    14,    30,    -1,     5,    14,    -1,    77,    -1,
      76,    77,    -1,    79,    65,    -1,    27,    79,    65,    -1,
      26,    79,    65,    -1,    28,    79,    65,    -1,    26,    25,
      79,    65,    -1,    28,    25,    79,    65,    -1,    80,    65,
      -1,    16,    -1,    14,    -1,    24,    14,    -1,    22,    11,
      14,    23,    -1,    22,    12,    83,    24,    14,    85,    23,
      -1,    14,    -1,    22,    17,    -1,    22,    18,    -1,    22,
      19,    -1,    22,    20,    -1,    13,    -1,    27,    13,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    75,    75,    79,    81,    83,    85,    87,    89,    91,
      93,    95,    97,    99,   101,   106,   110,   113,   116,   117,
     120,   121,   122,   125,   128,   132,   139,   140,   143,   149,
     152,   153,   154,   157,   158,   161,   169,   178,   179,   182,
     183,   186,   189,   192,   193,   194,   197,   198,   201,   208,
     211,   215,   220,   225,   233,   234,   235,   238,   239,   240,
     241,   242,   243,   244,   248,   255,   264,   265,   269,   271,
     273,   275,   277,   279,   283,   291,   292,   293,   297,   304,
     305,   309,   311,   313,   316,   321,   325,   331,   332,   333,
     336,   337,   338,   339,   340,   344,   349,   354,   362,   370,
     379,   380,   384,   386,   388,   390,   392,   394,   402,   405,
     408,   411,   414,   424,   428,   435,   436,   437,   438,   441,
     442
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "AP_CHUNK_TYPE", "AP_PROD", "AP_ISA",
  "AP_CLEAR_ALL", "AP_DEFINE_MODEL", "AP_ADD_DM", "AP_DEF_CHUNKS",
  "AP_SGP", "AP_GOAL_FOCUS", "AP_SPP", "AP_NUMBER", "AP_NAME", "AP_STRING",
  "AP_BANG_EXPR", "AP_DEFVAR", "AP_DEFMETHOD", "AP_DEFUN", "AP_SETF",
  "'*'", "'('", "')'", "':'", "'='", "'>'", "'-'", "'<'", "'?'", "'!'",
  "'+'", "$accept", "list", "clear_all", "define_model", "sgp",
  "sgp_params", "sgp_param", "sgp_param_nm", "chunktype", "chunktype_nm",
  "slots", "slot", "add_dm", "dm_items", "dm_item", "dm_item_nm",
  "dm_item_typ", "chunk_vals", "chunk_val", "chunk_slot", "def_chunks",
  "dc_items", "dc_item", "dc_item_nm", "prod", "prod_name", "prod_lhs",
  "prod_cond", "prod_cond_name", "prod_cond_type", "prod_cond_vals",
  "prod_cond_val", "cond_slot", "slot_val", "prod_query_name",
  "prod_query_vals", "prod_query_val", "query_first", "prod_cond_bang",
  "prod_rhs", "prod_act", "prod_act_name", "prod_act_bang",
  "prod_act_type", "prod_act_vals", "prod_act_val", "bang_expr",
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
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    42,    40,    41,    58,    61,    62,    45,    60,    63,
      33,    43
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    32,    33,    33,    33,    33,    33,    33,    33,    33,
      33,    33,    33,    33,    33,    34,    35,    36,    37,    37,
      38,    38,    38,    39,    40,    41,    42,    42,    43,    44,
      45,    45,    45,    46,    46,    47,    48,    49,    49,    50,
      50,    51,    52,    53,    53,    53,    54,    54,    55,    56,
      57,    57,    57,    57,    58,    58,    58,    59,    59,    59,
      59,    59,    59,    59,    60,    61,    62,    62,    63,    63,
      63,    63,    63,    63,    64,    65,    65,    65,    66,    67,
      67,    68,    68,    68,    68,    69,    70,    71,    71,    71,
      72,    72,    72,    72,    72,    73,    73,    73,    74,    75,
      76,    76,    77,    77,    77,    77,    77,    77,    77,    78,
      79,    80,    81,    82,    83,    84,    84,    84,    84,    85,
      85
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     3,     4,     1,     2,
       2,     2,     5,     2,     3,     3,     1,     2,     1,     4,
       1,     2,     2,     3,     4,     2,     2,     1,     2,     2,
       2,     1,     4,     1,     2,     2,     3,     4,     2,     7,
       3,     4,     4,     5,     1,     2,     2,     2,     2,     3,
       1,     2,     1,     2,     3,     2,     1,     2,     2,     3,
       3,     3,     4,     4,     1,     1,     2,     1,     3,     1,
       2,     2,     3,     3,     4,     1,     3,     1,     2,     2,
       1,     3,     2,     1,     2,     3,     3,     3,     3,     2,
       1,     2,     2,     3,     3,     3,     4,     4,     2,     1,
       1,     2,     4,     7,     1,     2,     2,     2,     2,     1,
       2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    14,     0,    13,     3,     4,     5,     6,
       0,     7,     8,     9,     0,    10,    11,    12,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   115,   116,   117,
     118,    28,     0,    26,     0,     0,     0,     0,    54,     0,
      60,    62,    25,    50,     0,    15,    16,     0,     0,    30,
       0,     0,     0,    43,     0,     0,     0,    18,     0,     0,
     114,     0,    24,    27,     0,     0,     0,    56,     0,    55,
       0,    74,     0,     0,     0,    58,    57,    66,     0,    85,
       0,    61,    79,     0,   109,    63,    52,    51,    35,    32,
      29,    31,     0,     0,    48,    45,    42,    44,     0,    23,
      17,    19,   119,    20,     0,     0,    21,   112,     0,    64,
      78,    86,     0,    65,     0,     0,     0,     0,     0,    59,
      67,    75,     0,    68,    77,     0,    80,    81,     0,    53,
      36,    41,    33,     0,    37,     0,    46,     0,     0,   120,
       0,     0,     0,    70,    69,     0,    71,    76,    82,     0,
      83,    34,    38,    39,    40,    47,     0,     0,     0,     0,
       0,     0,     0,    87,    90,    93,    72,    73,    84,    22,
     113,     0,     0,     0,     0,    89,    49,    88,     0,   110,
       0,     0,     0,     0,     0,    92,   100,     0,     0,    94,
      95,    97,    98,    96,    99,   111,     0,     0,     0,     0,
       0,    91,   101,   102,   108,     0,   104,   103,     0,   105,
     106,   107
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     6,     7,     8,    56,    57,    58,     9,    10,
      32,    33,    11,    48,    49,    50,    93,   133,   134,   135,
      12,    52,    53,    54,    13,    14,    37,    38,    39,    75,
      76,    77,    78,   123,    40,    81,    82,    83,    41,   162,
     163,   164,   165,   184,   185,   186,    85,   187,   188,    15,
      16,    61,    17,   124
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -181
static const yytype_int16 yypact[] =
{
    -181,    65,  -181,  -181,    88,  -181,  -181,  -181,  -181,  -181,
      30,  -181,  -181,  -181,   120,  -181,  -181,  -181,    35,   119,
      31,    38,    52,    54,    55,    63,   100,  -181,  -181,  -181,
    -181,  -181,    57,  -181,   105,   106,   122,    11,  -181,    32,
      -1,   121,  -181,   128,   132,  -181,  -181,   137,    41,  -181,
     147,   139,    46,  -181,   147,   140,    -5,  -181,    48,   133,
    -181,   131,  -181,  -181,   134,   135,   127,  -181,     3,  -181,
     144,  -181,    53,   145,    90,    75,    75,  -181,    96,  -181,
     148,    -1,  -181,    99,  -181,  -181,  -181,   149,  -181,  -181,
    -181,  -181,   151,   108,  -181,  -181,  -181,  -181,   112,  -181,
    -181,  -181,  -181,  -181,    -2,   150,  -181,  -181,   152,  -181,
    -181,  -181,   141,  -181,   145,    96,    96,   145,    96,    75,
    -181,  -181,   154,  -181,  -181,   102,  -181,  -181,   155,  -181,
    -181,  -181,  -181,   116,  -181,    98,  -181,   118,    -2,  -181,
      -2,   117,    96,  -181,  -181,    96,  -181,  -181,  -181,   156,
    -181,  -181,  -181,  -181,  -181,  -181,   153,   157,   158,   159,
     160,   161,     8,  -181,    29,   121,  -181,  -181,  -181,  -181,
    -181,   162,   163,   164,   165,  -181,  -181,  -181,   167,  -181,
     168,   103,   169,   104,    -4,    -4,  -181,    96,    96,  -181,
    -181,  -181,  -181,  -181,  -181,  -181,   169,    96,    96,   169,
      96,    -4,  -181,  -181,  -181,    96,  -181,  -181,    96,  -181,
    -181,  -181
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -181,  -181,  -181,  -181,  -181,  -181,   115,  -181,  -181,  -181,
    -181,   146,  -181,  -181,   129,  -181,   125,    86,     1,  -181,
    -181,  -181,   138,  -181,  -181,  -181,  -181,   166,  -181,  -181,
     110,   -74,   -66,  -115,  -181,  -181,   111,   107,  -181,  -181,
      24,  -181,  -181,  -181,     9,  -180,    33,  -167,  -181,  -181,
    -181,  -181,  -181,   -54
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
     143,   144,   120,   146,   106,   202,   115,   116,   118,   175,
     179,   102,    67,    79,   197,   198,   200,    64,   100,    55,
     180,   202,   181,   182,   183,   105,    80,   166,   112,   205,
     167,   176,   208,   158,   178,   159,    68,    70,   160,   161,
      35,    36,    89,   179,    31,   120,    71,    95,   142,    42,
     138,   145,    46,   180,    45,   181,   182,   183,    72,    73,
      74,   102,   103,    47,    90,     2,     3,    71,    51,    96,
     104,    31,   203,   204,    47,   105,    51,    59,   114,    55,
      62,   154,   206,   207,   156,   209,   157,     4,     5,    71,
     210,    18,    19,   211,    20,    21,    22,    23,    24,    25,
      26,    72,    73,    74,    71,    27,    28,    29,    30,   102,
     121,   102,   153,   127,    60,   117,   148,   179,   179,    64,
      65,   122,   131,   105,   128,   105,   131,   149,   196,   199,
     131,   132,   131,    43,   152,   136,    66,    84,   152,   151,
      44,   155,   158,    86,   159,    34,    87,   160,   161,    35,
      36,    88,    92,    94,    99,   108,   107,   111,   113,    71,
     109,   110,    79,   139,   129,   130,   140,   141,   147,   150,
     168,   101,   171,   172,   173,   174,   169,    91,    63,    98,
     170,   194,   195,   179,   137,   119,   177,   125,   190,   191,
      97,   193,   126,   201,   192,     0,     0,     0,   189,     0,
       0,     0,     0,    69
};

static const yytype_int16 yycheck[] =
{
     115,   116,    76,   118,    58,   185,    72,    73,    74,     1,
      14,    13,     1,    14,   181,   182,   183,    14,    23,    24,
      24,   201,    26,    27,    28,    27,    27,   142,    25,   196,
     145,    23,   199,    25,     5,    27,    25,     5,    30,    31,
      29,    30,     1,    14,    14,   119,    14,     1,   114,    14,
     104,   117,    14,    24,    23,    26,    27,    28,    26,    27,
      28,    13,    14,    22,    23,     0,     1,    14,    22,    23,
      22,    14,   187,   188,    22,    27,    22,    14,    25,    24,
      23,   135,   197,   198,   138,   200,   140,    22,    23,    14,
     205,     3,     4,   208,     6,     7,     8,     9,    10,    11,
      12,    26,    27,    28,    14,    17,    18,    19,    20,    13,
      14,    13,    14,    14,    14,    25,    14,    14,    14,    14,
      14,    25,    14,    27,    25,    27,    14,    25,    25,    25,
      14,    23,    14,    14,   133,    23,    14,    16,   137,    23,
      21,    23,    25,    15,    27,    25,    14,    30,    31,    29,
      30,    14,     5,    14,    14,    24,    23,    30,    14,    14,
      26,    26,    14,    13,    15,    14,    14,    26,    14,    14,
      14,    56,    14,    14,    14,    14,    23,    48,    32,    54,
      23,    14,    14,    14,    98,    75,   162,    80,    26,    26,
      52,    26,    81,   184,    30,    -1,    -1,    -1,   165,    -1,
      -1,    -1,    -1,    37
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    33,     0,     1,    22,    23,    34,    35,    36,    40,
      41,    44,    52,    56,    57,    81,    82,    84,     3,     4,
       6,     7,     8,     9,    10,    11,    12,    17,    18,    19,
      20,    14,    42,    43,    25,    29,    30,    58,    59,    60,
      66,    70,    14,    14,    21,    23,    14,    22,    45,    46,
      47,    22,    53,    54,    55,    24,    37,    38,    39,    14,
      14,    83,    23,    43,    14,    14,    14,     1,    25,    59,
       5,    14,    26,    27,    28,    61,    62,    63,    64,    14,
      27,    67,    68,    69,    16,    78,    15,    14,    14,     1,
      23,    46,     5,    48,    14,     1,    23,    54,    48,    14,
      23,    38,    13,    14,    22,    27,    85,    23,    24,    26,
      26,    30,    25,    14,    25,    64,    64,    25,    64,    62,
      63,    14,    25,    65,    85,    69,    68,    14,    25,    15,
      14,    14,    23,    49,    50,    51,    23,    49,    85,    13,
      14,    26,    64,    65,    65,    64,    65,    14,    14,    25,
      14,    23,    50,    14,    85,    23,    85,    85,    25,    27,
      30,    31,    71,    72,    73,    74,    65,    65,    14,    23,
      23,    14,    14,    14,    14,     1,    23,    72,     5,    14,
      24,    26,    27,    28,    75,    76,    77,    79,    80,    78,
      26,    26,    30,    26,    14,    14,    25,    79,    79,    25,
      79,    76,    77,    65,    65,    79,    65,    65,    79,    65,
      65,    65
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
    {
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 13:
#line 99 "actr_parse.y"
    { /* end of define model */
	    AMCP->ResetParse(); return AMCP->load_state; }
    break;

  case 14:
#line 101 "actr_parse.y"
    {
            AMCP->ResetParse(); AMCP->load_state = ActrModel::YYRet_NoSrc;
            return AMCP->load_state; }
    break;

  case 15:
#line 106 "actr_parse.y"
    { /* assumed */
        }
    break;

  case 16:
#line 110 "actr_parse.y"
    { AMCP->name = (yyvsp[(3) - (3)].chr); AMCP->SigEmitUpdated(); }
    break;

  case 17:
#line 113 "actr_parse.y"
    { }
    break;

  case 20:
#line 120 "actr_parse.y"
    { AMCP->SetParam((yyvsp[(1) - (2)].chr), (yyvsp[(2) - (2)].chr)); }
    break;

  case 21:
#line 121 "actr_parse.y"
    { AMCP->SetParam((yyvsp[(1) - (2)].chr), (yyvsp[(2) - (2)].num)); }
    break;

  case 22:
#line 122 "actr_parse.y"
    { AMCP->SetParam((yyvsp[(1) - (5)].chr), (yyvsp[(3) - (5)].num), (yyvsp[(4) - (5)].num)); }
    break;

  case 23:
#line 125 "actr_parse.y"
    { AMCP->load_param = (yyvsp[(2) - (2)].chr); (yyval.chr) = AMCP->load_param; }
    break;

  case 24:
#line 128 "actr_parse.y"
    { }
    break;

  case 25:
#line 132 "actr_parse.y"
    {
           bool made_new = false;
           AMCP->load_chtype = AMCP->chunk_types.FindMakeNameType((yyvsp[(3) - (3)].chr), NULL, made_new);
           (yyval.chtyp) = AMCP->load_chtype;
         }
    break;

  case 27:
#line 140 "actr_parse.y"
    { (yyval.sltyp) = (yyvsp[(1) - (2)].sltyp); }
    break;

  case 28:
#line 143 "actr_parse.y"
    {
            bool made_new = false;
            (yyval.sltyp) = AMCP->load_chtype->slots.FindMakeNameType((yyvsp[(1) - (1)].chr), NULL, made_new);
          }
    break;

  case 29:
#line 149 "actr_parse.y"
    {   }
    break;

  case 33:
#line 157 "actr_parse.y"
    { }
    break;

  case 34:
#line 158 "actr_parse.y"
    { }
    break;

  case 35:
#line 161 "actr_parse.y"
    {
           ActrDeclarativeModule* dmod = AMCP->DeclarativeModule();
           bool made_new = false;
           AMCP->load_chunk = dmod->init_chunks.FindMakeNameType((yyvsp[(2) - (2)].chr), NULL, made_new);
           /*           AMCP->load_chunk->name = $2; // should set this! */
           (yyval.chk) = AMCP->load_chunk; }
    break;

  case 36:
#line 169 "actr_parse.y"
    {
           ActrChunkType* ct = AMCP->FindChunkType((yyvsp[(2) - (2)].chr));
           if(ct) {
             AMCP->load_chunk->SetChunkType(ct);
           }
           (yyval.chk) = AMCP->load_chunk; }
    break;

  case 39:
#line 182 "actr_parse.y"
    { if((yyvsp[(1) - (2)].slt)) (yyvsp[(1) - (2)].slt)->val = (yyvsp[(2) - (2)].chr); }
    break;

  case 40:
#line 183 "actr_parse.y"
    { if((yyvsp[(1) - (2)].slt)) (yyvsp[(1) - (2)].slt)->val = (String)(yyvsp[(2) - (2)].num); }
    break;

  case 41:
#line 186 "actr_parse.y"
    { (yyval.slt) = AMCP->load_chunk->FindSlot((yyvsp[(1) - (1)].chr)); }
    break;

  case 42:
#line 189 "actr_parse.y"
    {   }
    break;

  case 46:
#line 197 "actr_parse.y"
    { }
    break;

  case 47:
#line 198 "actr_parse.y"
    { }
    break;

  case 48:
#line 201 "actr_parse.y"
    {
           bool made_new = false;
           AMCP->load_chunk = AMCP->chunks.FindMakeNameType((yyvsp[(2) - (2)].chr), NULL, made_new);
           /*           AMCP->load_chunk->name = $2; // should set this! */
           (yyval.chk) = AMCP->load_chunk; }
    break;

  case 49:
#line 208 "actr_parse.y"
    { }
    break;

  case 50:
#line 211 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(3) - (3)].chr), NULL, made_new); }
    break;

  case 51:
#line 215 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(4) - (4)].chr), NULL, made_new);
           AMCP->load_prod->SetProdFlag(ActrProduction::PSTAR); }
    break;

  case 52:
#line 220 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(3) - (4)].chr), NULL, made_new);
           AMCP->load_prod->desc = (yyvsp[(4) - (4)].chr); }
    break;

  case 53:
#line 225 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(4) - (5)].chr), NULL, made_new);
           AMCP->load_prod->desc = (yyvsp[(5) - (5)].chr);
           AMCP->load_prod->SetProdFlag(ActrProduction::PSTAR); }
    break;

  case 57:
#line 238 "actr_parse.y"
    { }
    break;

  case 58:
#line 239 "actr_parse.y"
    { }
    break;

  case 59:
#line 240 "actr_parse.y"
    { }
    break;

  case 60:
#line 241 "actr_parse.y"
    { }
    break;

  case 61:
#line 242 "actr_parse.y"
    { }
    break;

  case 62:
#line 243 "actr_parse.y"
    { AMCP->load_bang_expr = false; }
    break;

  case 63:
#line 244 "actr_parse.y"
    { AMCP->load_bang_expr = false; }
    break;

  case 64:
#line 248 "actr_parse.y"
    {
           AMCP->load_cond = (ActrCondition*)AMCP->load_prod->conds.New(1);
           AMCP->load_cond->src = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_cond->cond_src = ActrCondition::BUFFER_EQ; }
    break;

  case 65:
#line 255 "actr_parse.y"
    {
             ActrChunkType* ct = AMCP->FindChunkType((yyvsp[(2) - (2)].chr));
             if(ct) {
               AMCP->load_cond->cmp_chunk.SetChunkType(ct);
             }
           }
    break;

  case 68:
#line 269 "actr_parse.y"
    {
             AMCP->load_cond->SetVal((yyvsp[(1) - (2)].slt), (yyvsp[(2) - (2)].chr)); }
    break;

  case 69:
#line 271 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(2) - (3)].slt), String((yyvsp[(3) - (3)].chr)) + String("-")); }
    break;

  case 70:
#line 273 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(2) - (3)].slt), (yyvsp[(3) - (3)].chr), Relation::GREATERTHAN); }
    break;

  case 71:
#line 275 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(2) - (3)].slt), (yyvsp[(3) - (3)].chr), Relation::LESSTHAN); }
    break;

  case 72:
#line 277 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(3) - (4)].slt), (yyvsp[(4) - (4)].chr), Relation::GREATERTHANOREQUAL); }
    break;

  case 73:
#line 279 "actr_parse.y"
    {
           AMCP->load_cond->SetVal((yyvsp[(3) - (4)].slt), (yyvsp[(4) - (4)].chr), Relation::LESSTHANOREQUAL); }
    break;

  case 74:
#line 283 "actr_parse.y"
    { 
             if((String)(yyvsp[(1) - (1)].chr) == "buffer") {
               AMCP->ParseErr("Warning: use of 'buffer' state query in =buffer (BUFFER_EQ) tests not supported");
               (yyval.slt) = NULL;
             }
             else { (yyval.slt) = AMCP->load_cond->cmp_chunk.FindSlot((yyvsp[(1) - (1)].chr)); } }
    break;

  case 76:
#line 292 "actr_parse.y"
    { AMCP->load_tmp = String("=") + (yyvsp[(2) - (2)].chr); (yyval.chr) = AMCP->load_tmp; }
    break;

  case 77:
#line 293 "actr_parse.y"
    { AMCP->load_tmp = (String)(yyvsp[(1) - (1)].num); (yyval.chr) = AMCP->load_tmp; }
    break;

  case 78:
#line 297 "actr_parse.y"
    {
           AMCP->load_cond = (ActrCondition*)AMCP->load_prod->conds.New(1);
           AMCP->load_cond->src = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_cond->cond_src = ActrCondition::BUFFER_QUERY; }
    break;

  case 81:
#line 309 "actr_parse.y"
    {
             AMCP->load_cond->SetQuery((yyvsp[(1) - (2)].chr), (yyvsp[(2) - (2)].chr)); }
    break;

  case 82:
#line 311 "actr_parse.y"
    {
           AMCP->load_cond->SetQuery((yyvsp[(2) - (3)].chr), (yyvsp[(3) - (3)].chr), true); /* neg = true */ }
    break;

  case 83:
#line 313 "actr_parse.y"
    {
           AMCP->ParseErr("Warning: use of variable expression (=varname) query tests not supported");
           AMCP->load_cond->SetQuery((yyvsp[(1) - (3)].chr), String("=") + (yyvsp[(3) - (3)].chr)); /* neg = true */ }
    break;

  case 84:
#line 316 "actr_parse.y"
    {
           AMCP->ParseErr("Warning: use of variable expression (=varname) query tests not supported");
           AMCP->load_cond->SetQuery((yyvsp[(2) - (4)].chr), String("=") + (yyvsp[(4) - (4)].chr), true); /* neg = true */ }
    break;

  case 85:
#line 321 "actr_parse.y"
    { AMCP->load_tmp = (yyvsp[(1) - (1)].chr); (yyval.chr) = AMCP->load_tmp; }
    break;

  case 86:
#line 325 "actr_parse.y"
    { 
             AMCP->ParseErr(String("Warning: use of !") + (yyvsp[(2) - (3)].chr) + "! expression not supported for conditions");
             AMCP->load_bang_expr = true; /* get expr */
           }
    break;

  case 91:
#line 337 "actr_parse.y"
    { }
    break;

  case 92:
#line 338 "actr_parse.y"
    { }
    break;

  case 93:
#line 339 "actr_parse.y"
    { AMCP->load_bang_expr = false; }
    break;

  case 94:
#line 340 "actr_parse.y"
    { AMCP->load_act->val += (yyvsp[(2) - (2)].chr); AMCP->load_bang_expr = false; }
    break;

  case 95:
#line 344 "actr_parse.y"
    {
           AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
           AMCP->load_act->dest = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_act->action = ActrAction::UPDATE; 
           AMCP->load_act->UpdateAfterEdit_NoGui(); }
    break;

  case 96:
#line 349 "actr_parse.y"
    {
           AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
           AMCP->load_act->dest = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_act->action = ActrAction::REQUEST;
           AMCP->load_act->UpdateAfterEdit_NoGui(); }
    break;

  case 97:
#line 354 "actr_parse.y"
    {
           AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
           AMCP->load_act->dest = AMCP->buffers.FindName((yyvsp[(2) - (3)].chr));
           AMCP->load_act->action = ActrAction::CLEAR;
           AMCP->load_act->UpdateAfterEdit_NoGui(); }
    break;

  case 98:
#line 362 "actr_parse.y"
    {
             AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
             AMCP->load_act->SetBangAction((yyvsp[(2) - (3)].chr));
             AMCP->load_act->UpdateAfterEdit_NoGui();
             AMCP->load_bang_expr = true; /* get expr */ }
    break;

  case 99:
#line 370 "actr_parse.y"
    {
             ActrChunkType* ct = AMCP->FindChunkType((yyvsp[(2) - (2)].chr));
             if(ct) {
               AMCP->load_act->chunk.SetChunkType(ct);
             }
           }
    break;

  case 102:
#line 384 "actr_parse.y"
    {
             if((yyvsp[(1) - (2)].slt)) (yyvsp[(1) - (2)].slt)->SetVal((yyvsp[(2) - (2)].chr)); }
    break;

  case 103:
#line 386 "actr_parse.y"
    {
           if((yyvsp[(2) - (3)].slt)) (yyvsp[(2) - (3)].slt)->SetVal(String((yyvsp[(3) - (3)].chr)) + String("-")); }
    break;

  case 104:
#line 388 "actr_parse.y"
    {
           if((yyvsp[(2) - (3)].slt)) (yyvsp[(2) - (3)].slt)->SetVal((yyvsp[(3) - (3)].chr), Relation::GREATERTHAN); }
    break;

  case 105:
#line 390 "actr_parse.y"
    {
           if((yyvsp[(2) - (3)].slt)) (yyvsp[(2) - (3)].slt)->SetVal((yyvsp[(3) - (3)].chr), Relation::LESSTHAN); }
    break;

  case 106:
#line 392 "actr_parse.y"
    {
           if((yyvsp[(3) - (4)].slt)) (yyvsp[(3) - (4)].slt)->SetVal((yyvsp[(4) - (4)].chr), Relation::GREATERTHANOREQUAL); }
    break;

  case 107:
#line 394 "actr_parse.y"
    {
           if((yyvsp[(3) - (4)].slt)) (yyvsp[(3) - (4)].slt)->SetVal((yyvsp[(4) - (4)].chr), Relation::LESSTHANOREQUAL); }
    break;

  case 108:
#line 402 "actr_parse.y"
    { AMCP->load_act->params += String(" ") + (yyvsp[(2) - (2)].chr); }
    break;

  case 110:
#line 408 "actr_parse.y"
    { (yyval.slt) = AMCP->load_act->chunk.FindSlot((yyvsp[(1) - (1)].chr)); }
    break;

  case 111:
#line 411 "actr_parse.y"
    { AMCP->load_act->params = (yyvsp[(2) - (2)].chr); }
    break;

  case 112:
#line 414 "actr_parse.y"
    {
              ActrDeclarativeModule* dmod = AMCP->DeclarativeModule();
              ActrGoalModule* gmod = AMCP->GoalModule();
              ActrChunk* gf = dmod->init_chunks.FindName((yyvsp[(3) - (4)].chr));
              if(!gf) { taMisc::Error("goal-focus cannot find declarative init chunk:", (yyvsp[(3) - (4)].chr)); }
              else    { ActrChunk* ngf = (ActrChunk*)gf->Clone(); ngf->name = gf->name; 
                        gmod->init_chunk.Add(ngf); }
            }
    break;

  case 113:
#line 424 "actr_parse.y"
    {
           AMCP->load_prod->SetParam((yyvsp[(5) - (7)].chr), (yyvsp[(6) - (7)].num)); }
    break;

  case 114:
#line 428 "actr_parse.y"
    {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType((yyvsp[(1) - (1)].chr), NULL, made_new); }
    break;

  case 120:
#line 442 "actr_parse.y"
    { (yyval.num) = -(yyvsp[(2) - (2)].num); }
    break;


/* Line 1267 of yacc.c.  */
#line 2147 "y.tab.c"
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


#line 445 "actr_parse.y"


	/* end of grammar */

void aperror(const char *s) { 	/* called for yacc syntax error */
  AMCP->ParseErr(s);
}

