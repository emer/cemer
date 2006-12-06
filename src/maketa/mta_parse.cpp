
/*  A Bison parser, made from mta_parse.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	CLASS	257
#define	STRUCT	258
#define	UNION	259
#define	ENUM	260
#define	FUNTYPE	261
#define	STATIC	262
#define	TEMPLATE	263
#define	CONST	264
#define	TYPEDEF	265
#define	TA_TYPEDEF	266
#define	TYPE	267
#define	NUMBER	268
#define	FUNCTION	269
#define	ARRAY	270
#define	NAME	271
#define	COMMENT	272
#define	FUNCALL	273
#define	SCOPER	274
#define	EQUALS	275
#define	PUBLIC	276
#define	PRIVATE	277
#define	PROTECTED	278
#define	OPERATOR	279
#define	FRIEND	280
#define	THISNAME	281
#define	REGFUN	282
#define	VIRTUAL	283

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


#line 44 "mta_parse.y"
typedef struct {
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  char*    	chr;
  int	   	rval;
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		344
#define	YYFLAG		-32768
#define	YYNTBASE	45

#define YYTRANSLATE(x) ((unsigned)(x) <= 283 ? yytranslate[x] : 125)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    43,     2,    31,
    32,    30,    39,    36,    40,    42,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    35,    44,    38,
     2,    37,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    34,     2,    33,    41,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     7,    10,    13,    16,    19,    22,    25,
    27,    29,    31,    34,    37,    42,    46,    51,    54,    62,
    67,    69,    74,    77,    81,    85,    88,    90,    92,    95,
    99,   101,   104,   108,   112,   115,   119,   123,   125,   129,
   132,   135,   137,   139,   141,   145,   147,   150,   152,   155,
   157,   160,   162,   164,   166,   169,   173,   177,   180,   184,
   188,   194,   196,   199,   204,   206,   211,   215,   218,   220,
   223,   225,   228,   230,   233,   235,   237,   241,   243,   247,
   251,   253,   255,   257,   259,   262,   264,   266,   268,   270,
   273,   275,   277,   280,   283,   286,   288,   290,   293,   297,
   300,   305,   310,   312,   316,   318,   323,   325,   329,   334,
   339,   344,   349,   355,   361,   367,   372,   376,   381,   386,
   392,   397,   399,   402,   405,   409,   413,   416,   421,   428,
   432,   435,   437,   439,   441,   444,   447,   450,   454,   456,
   459,   463,   465,   467,   470,   473,   477,   479,   482,   484,
   488,   493,   495,   498,   500,   503,   507,   514,   516,   518,
   521,   525,   528,   530,   532,   535,   537,   539,   541,   544,
   546,   548,   551,   553,   556,   558,   561,   563,   566,   569,
   573,   577,   581,   585,   588,   590,   595,   600,   602,   605,
   607,   611,   613,   615,   617,   620,   622,   624,   626,   628,
   630,   632
};

static const short yyrhs[] = {    -1,
    45,    47,     0,    45,    51,     0,    45,    55,     0,    45,
    57,     0,    45,    67,     0,    45,    74,     0,    45,    46,
     0,    45,     1,     0,    12,     0,    48,     0,    49,     0,
    49,    18,     0,    11,    50,     0,    11,    59,   119,   121,
     0,   113,   112,   121,     0,   113,    18,   112,   121,     0,
   113,   121,     0,   113,    31,    30,   112,    32,   100,   121,
     0,   113,    20,    30,   112,     0,    52,     0,    53,    77,
    33,   121,     0,    54,    34,     0,    54,    18,    34,     0,
    54,    34,    18,     0,     6,   112,     0,     6,     0,    56,
     0,    62,   121,     0,    62,   121,    18,     0,    58,     0,
    59,   121,     0,    59,   120,   121,     0,    60,    84,    33,
     0,    61,    34,     0,    61,    18,    34,     0,    61,    34,
    18,     0,    62,     0,    62,    35,    63,     0,   124,   112,
     0,   124,    13,     0,   124,     0,   123,     0,    64,     0,
    63,    36,    64,     0,   113,     0,    65,   113,     0,    17,
     0,    65,    17,     0,    66,     0,    65,    66,     0,   122,
     0,    29,     0,    68,     0,    69,   121,     0,    69,   120,
   121,     0,    70,    84,    33,     0,    71,    34,     0,    71,
    18,    34,     0,    71,    34,    18,     0,     9,    72,    73,
    37,    61,     0,    38,     0,     3,   112,     0,    73,    36,
     3,   112,     0,    75,     0,    28,   111,    76,    97,     0,
    28,    76,    97,     0,    96,   100,     0,    78,     0,    77,
    78,     0,    79,     0,    79,    18,     0,    80,     0,    80,
    36,     0,     1,     0,    83,     0,    83,    21,    81,     0,
    82,     0,    81,    39,    82,     0,    81,    40,    82,     0,
    17,     0,    14,     0,    17,     0,    85,     0,    84,    85,
     0,    86,     0,    92,     0,    52,     0,    49,     0,    59,
   121,     0,     1,     0,    87,     0,    22,    35,     0,    23,
    35,     0,    24,    35,     0,    18,     0,    88,     0,     8,
    88,     0,   110,    89,   121,     0,    89,   121,     0,   110,
    90,    16,   121,     0,   110,    91,   100,   121,     0,    90,
     0,    89,    36,    90,     0,    17,     0,    31,    30,    17,
    32,     0,    93,     0,   107,   101,    97,     0,     7,   107,
   101,    97,     0,    29,   107,   101,    97,     0,   107,   101,
   105,    97,     0,    41,   107,   101,    97,     0,     7,    41,
   107,   101,    97,     0,    29,    41,   107,   101,    97,     0,
    26,   111,    25,   100,    97,     0,    26,    25,   100,    97,
     0,    26,   111,   121,     0,    26,     3,   111,   121,     0,
    26,     3,    17,   121,     0,    26,   111,    96,   100,    97,
     0,    26,    96,   100,    97,     0,    94,     0,     8,    94,
     0,    29,    94,     0,     7,    29,    94,     0,   110,    95,
    97,     0,    95,    97,     0,   110,    25,   100,    97,     0,
   110,    25,    31,    32,   100,    97,     0,    25,   100,    97,
     0,    96,   100,     0,    17,     0,   121,     0,    98,     0,
    99,   121,     0,    18,   121,     0,    18,    98,     0,    18,
    99,   121,     0,    15,     0,    10,    15,     0,    10,    18,
    15,     0,    21,     0,    10,     0,    10,    21,     0,    31,
    32,     0,    31,   102,    32,     0,    32,     0,   102,    32,
     0,   103,     0,   102,    36,   103,     0,   102,    42,    42,
    42,     0,   104,     0,   104,    21,     0,   113,     0,   113,
    17,     0,   113,    17,    16,     0,   113,    31,    30,    17,
    32,   100,     0,    17,     0,   106,     0,   105,   106,     0,
   108,    13,    19,     0,    27,    31,     0,    35,     0,    30,
     0,   109,    30,     0,   111,     0,   113,     0,     7,     0,
     7,   113,     0,    17,     0,   114,     0,   114,    43,     0,
   115,     0,   115,   109,     0,   116,     0,    10,   116,     0,
   117,     0,   123,   117,     0,   123,   112,     0,    13,    20,
    17,     0,    27,    20,    17,     0,    13,    20,    13,     0,
    27,    20,    13,     0,    20,    13,     0,    27,     0,    13,
    72,   118,    37,     0,    27,    72,   118,    37,     0,    13,
     0,   117,    13,     0,    13,     0,   118,    36,    13,     0,
    17,     0,    13,     0,    17,     0,    17,    16,     0,    44,
     0,    22,     0,    23,     0,    24,     0,     4,     0,     5,
     0,     3,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   112,   113,   116,   118,   121,   124,   126,   128,   130,   134,
   138,   145,   147,   151,   152,   156,   161,   168,   198,   201,
   206,   212,   215,   216,   217,   220,   224,   230,   238,   240,
   243,   251,   253,   256,   262,   266,   268,   272,   274,   277,
   282,   286,   292,   301,   304,   312,   313,   314,   315,   318,
   319,   322,   323,   326,   334,   336,   339,   345,   349,   351,
   355,   363,   366,   368,   371,   377,   379,   383,   388,   389,
   392,   393,   396,   397,   398,   403,   406,   411,   413,   418,
   425,   427,   430,   434,   435,   438,   447,   460,   463,   466,
   470,   473,   475,   476,   477,   478,   497,   499,   504,   506,
   507,   515,   518,   523,   529,   535,   540,   541,   542,   543,
   544,   545,   546,   547,   548,   549,   550,   551,   552,   553,
   561,   571,   573,   574,   576,   580,   582,   583,   584,   585,
   588,   595,   599,   600,   601,   602,   603,   604,   607,   609,
   610,   613,   615,   616,   619,   620,   623,   624,   627,   628,
   629,   632,   634,   641,   646,   650,   654,   658,   664,   666,
   669,   672,   677,   680,   681,   684,   687,   688,   689,   692,
   695,   696,   705,   707,   715,   716,   725,   726,   727,   728,
   732,   736,   737,   738,   739,   740,   757,   763,   764,   773,
   775,   778,   779,   782,   783,   786,   789,   790,   791,   794,
   796,   799
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","CLASS",
"STRUCT","UNION","ENUM","FUNTYPE","STATIC","TEMPLATE","CONST","TYPEDEF","TA_TYPEDEF",
"TYPE","NUMBER","FUNCTION","ARRAY","NAME","COMMENT","FUNCALL","SCOPER","EQUALS",
"PUBLIC","PRIVATE","PROTECTED","OPERATOR","FRIEND","THISNAME","REGFUN","VIRTUAL",
"'*'","'('","')'","'}'","'{'","':'","','","'>'","'<'","'+'","'-'","'~'","'.'",
"'&'","';'","list","preparsed","typedefn","typedefns","typedsub","defn","enumdefn",
"enumdsub","enumname","enumnm","classdecl","classdecls","classdefn","classdefns",
"classdsub","classname","classhead","classnm","classinh","classpar","classptyp",
"classpmod","templdefn","templdefns","templdsub","templname","templhead","templopen",
"templpars","fundecl","funnm","regfundefn","enums","enumline","enumitm","enumitms",
"enummath","enummathitm","enmitmname","membs","membline","membdefn","basicmemb",
"nostatmemb","membnames","membname","membfunp","methdefn","basicmeth","nostatmeth",
"mbfundefn","methname","fundefn","funsubdefn","funsubdecl","funargs","constfun",
"args","argdefn","subargdefn","constrlist","constref","consthsnm","constcoln",
"ptrs","membtype","ftype","tyname","type","noreftype","constype","subtype","combtype",
"templtypes","tdname","varname","term","access","structstruct","classkeyword", NULL
};
#endif

static const short yyr1[] = {     0,
    45,    45,    45,    45,    45,    45,    45,    45,    45,    46,
    47,    48,    48,    49,    49,    50,    50,    50,    50,    50,
    51,    52,    53,    53,    53,    54,    54,    55,    56,    56,
    57,    58,    58,    59,    60,    60,    60,    61,    61,    62,
    62,    62,    62,    63,    63,    64,    64,    64,    64,    65,
    65,    66,    66,    67,    68,    68,    69,    70,    70,    70,
    71,    72,    73,    73,    74,    75,    75,    76,    77,    77,
    78,    78,    79,    79,    79,    80,    80,    81,    81,    81,
    82,    82,    83,    84,    84,    85,    85,    85,    85,    85,
    85,    86,    86,    86,    86,    86,    87,    87,    88,    88,
    88,    88,    89,    89,    90,    91,    92,    92,    92,    92,
    92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
    92,    93,    93,    93,    93,    94,    94,    94,    94,    94,
    95,    96,    97,    97,    97,    97,    97,    97,    98,    98,
    98,    99,    99,    99,   100,   100,   101,   101,   102,   102,
   102,   103,   103,   104,   104,   104,   104,   104,   105,   105,
   106,   107,   108,   109,   109,   110,   111,   111,   111,   112,
   113,   113,   114,   114,   115,   115,   116,   116,   116,   116,
   116,   116,   116,   116,   116,   116,   116,   117,   117,   118,
   118,   119,   119,   120,   120,   121,   122,   122,   122,   123,
   123,   124
};

static const short yyr2[] = {     0,
     0,     2,     2,     2,     2,     2,     2,     2,     2,     1,
     1,     1,     2,     2,     4,     3,     4,     2,     7,     4,
     1,     4,     2,     3,     3,     2,     1,     1,     2,     3,
     1,     2,     3,     3,     2,     3,     3,     1,     3,     2,
     2,     1,     1,     1,     3,     1,     2,     1,     2,     1,
     2,     1,     1,     1,     2,     3,     3,     2,     3,     3,
     5,     1,     2,     4,     1,     4,     3,     2,     1,     2,
     1,     2,     1,     2,     1,     1,     3,     1,     3,     3,
     1,     1,     1,     1,     2,     1,     1,     1,     1,     2,
     1,     1,     2,     2,     2,     1,     1,     2,     3,     2,
     4,     4,     1,     3,     1,     4,     1,     3,     4,     4,
     4,     4,     5,     5,     5,     4,     3,     4,     4,     5,
     4,     1,     2,     2,     3,     3,     2,     4,     6,     3,
     2,     1,     1,     1,     2,     2,     2,     3,     1,     2,
     3,     1,     1,     2,     2,     3,     1,     2,     1,     3,
     4,     1,     2,     1,     2,     3,     6,     1,     1,     2,
     3,     2,     1,     1,     2,     1,     1,     1,     2,     1,
     1,     2,     1,     2,     1,     2,     1,     2,     2,     3,
     3,     3,     3,     2,     1,     4,     4,     1,     2,     1,
     3,     1,     1,     1,     2,     1,     1,     1,     1,     1,
     1,     1
};

static const short yydefact[] = {     1,
     0,     9,   202,   200,   201,    27,     0,     0,    10,     0,
     8,     2,    11,    12,     3,    21,     0,     0,     4,    28,
     5,    31,     0,     0,     0,    38,     6,    54,     0,     0,
     0,     7,    65,    43,    42,   170,    26,    62,     0,     0,
   188,     0,   185,    14,     0,    38,     0,   171,   173,   175,
   177,    43,   168,   132,     0,     0,     0,   167,     0,    13,
    75,    83,     0,    69,    71,    73,    76,     0,    23,   194,
   196,     0,    32,    91,   168,     0,   105,    96,     0,     0,
     0,     0,     0,   185,     0,     0,    89,    88,     0,     0,
    84,    86,    92,    97,     0,   103,    87,   107,   122,     0,
     0,     0,     0,   166,     0,    35,     0,    29,     0,    55,
     0,     0,    58,    41,    40,     0,     0,   176,     0,     0,
   184,     0,     0,   193,   192,     0,     0,     0,     0,     0,
    18,   172,   164,   174,   189,   188,   179,   178,   169,   143,
   139,     0,   142,    67,   134,     0,   133,     0,    68,     0,
     0,    70,    72,    74,     0,    24,    25,   195,    33,     0,
     0,     0,    98,   123,    93,    94,    95,     0,     0,     0,
     0,     0,   162,     0,   124,     0,     0,     0,     0,    90,
    34,    85,     0,   100,   127,   131,   158,   147,     0,     0,
   149,   152,   154,     0,     0,     0,   103,     0,     0,    36,
    37,    48,   197,   198,   199,    53,    39,    44,     0,    50,
    46,    52,    30,    56,    57,    59,    60,    63,     0,     0,
   182,   180,   190,     0,   183,   181,     0,    15,     0,     0,
     0,    16,   165,   140,     0,   144,   137,     0,   136,   135,
   145,     0,    66,    22,    82,    81,    77,    78,   125,     0,
     0,   130,     0,     0,     0,     0,     0,     0,   117,     0,
     0,     0,   105,   104,   163,   108,     0,   159,     0,   148,
     0,     0,   153,   155,     0,     0,     0,     0,    99,     0,
     0,   126,     0,    49,    51,    47,     0,    61,     0,   186,
   187,    17,    20,     0,   141,   138,   146,     0,     0,     0,
   109,   119,   118,   116,   121,     0,     0,     0,   110,   112,
   111,   160,     0,   150,     0,   156,     0,   145,   128,     0,
   101,   102,    45,    64,   191,     0,    79,    80,   113,   115,
   120,   114,   161,   151,     0,     0,   106,     0,     0,   129,
    19,   157,     0,     0
};

static const short yydefgoto[] = {     1,
    11,    12,    13,    87,    44,    15,    88,    17,    18,    19,
    20,    21,    22,    89,    24,    25,    46,   207,   208,   209,
   210,    27,    28,    29,    30,    31,   123,   117,    32,    33,
    55,    63,    64,    65,    66,   247,   248,    67,    90,    91,
    92,    93,    94,    95,    96,   198,    97,    98,    99,   100,
   101,   144,   145,   146,   149,   189,   190,   191,   192,   267,
   268,   102,   269,   134,   103,   104,   137,    58,    48,    49,
    50,    51,   224,   126,    72,   147,   212,    59,    35
};

static const short yypact[] = {-32768,
   232,-32768,-32768,-32768,-32768,     4,   -19,   445,-32768,   493,
-32768,-32768,-32768,    26,-32768,-32768,    89,    84,-32768,-32768,
-32768,-32768,    17,   134,    90,    29,-32768,-32768,    17,   134,
    96,-32768,-32768,-32768,   157,-32768,-32768,-32768,    30,   541,
    21,    54,    38,-32768,   199,    61,   166,    -4,   102,-32768,
   135,   205,   524,-32768,    35,   112,   136,-32768,   205,-32768,
-32768,-32768,    15,-32768,   175,   119,   186,   183,   202,   218,
-32768,   195,-32768,-32768,   172,   463,   214,-32768,   216,   221,
   234,   112,   434,    67,    58,   243,-32768,-32768,   195,   283,
-32768,-32768,-32768,-32768,    85,-32768,-32768,-32768,-32768,    35,
   112,   348,   100,-32768,   225,   253,   386,   254,   195,-32768,
   322,   240,   257,-32768,-32768,     4,   168,-32768,   206,   263,
-32768,   229,   263,-32768,-32768,   195,     4,   247,   249,   195,
-32768,-32768,-32768,   250,-32768,-32768,-32768,   135,-32768,   150,
-32768,     7,-32768,-32768,-32768,   195,-32768,   366,-32768,    35,
   195,-32768,-32768,-32768,   174,-32768,-32768,-32768,-32768,   474,
   243,   348,-32768,-32768,-32768,-32768,-32768,    35,   504,   112,
   112,    57,-32768,   243,-32768,   348,   170,   261,   348,-32768,
-32768,-32768,   264,-32768,-32768,-32768,-32768,-32768,    25,   130,
-32768,   274,    53,   266,   268,    85,   286,   112,    35,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   275,-32768,   407,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   296,   224,
-32768,-32768,-32768,   213,-32768,-32768,   227,-32768,   195,     4,
     4,-32768,-32768,-32768,   289,-32768,-32768,   195,-32768,-32768,
-32768,   158,-32768,-32768,-32768,-32768,   226,-32768,-32768,   348,
    35,-32768,   195,   195,    35,    35,   112,   112,-32768,   348,
    35,    35,-32768,-32768,-32768,-32768,    25,-32768,   300,-32768,
   522,   272,-32768,   301,   288,   368,    35,   302,-32768,   195,
   195,-32768,   386,-32768,-32768,-32768,     4,-32768,   307,-32768,
-32768,-32768,-32768,   290,-32768,-32768,-32768,   174,   174,    35,
-32768,-32768,-32768,-32768,-32768,    35,    35,    35,-32768,-32768,
-32768,-32768,   312,-32768,   279,-32768,   317,   112,-32768,   304,
-32768,-32768,-32768,-32768,-32768,   112,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   305,    35,-32768,   195,   112,-32768,
-32768,-32768,   338,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   340,-32768,-32768,   342,-32768,-32768,-32768,
-32768,-32768,-32768,   126,-32768,   139,   349,-32768,    71,-32768,
   147,-32768,-32768,-32768,-32768,-32768,    11,-32768,-32768,-32768,
   303,-32768,   294,-32768,-32768,-32768,   -31,-32768,   332,   -54,
-32768,-32768,   291,   271,   -89,-32768,-32768,-32768,   -65,   -88,
    -3,   -53,   222,   235,   -78,  -150,  -143,    95,-32768,-32768,
   115,   -48,-32768,-32768,   -72,    -2,    -5,     2,-32768,-32768,
   329,   137,   269,-32768,   355,   -23,-32768,     1,-32768
};


#define	YYLAST		568


static const short yytable[] = {    73,
    37,    34,   108,   168,   242,   110,    56,    57,    52,    47,
   164,   251,   177,   197,   199,    61,   140,    39,    38,   175,
    36,   141,   186,   131,    52,   261,   162,   143,   262,   115,
    52,    62,   116,    70,   140,   182,   176,   179,   132,   141,
   119,   130,   142,    60,   140,   143,   185,   151,   159,   141,
    71,   120,   142,    56,   139,   143,   182,   122,    38,   265,
    71,     4,     5,   107,    53,   180,   121,    40,    71,   274,
    41,   184,    71,    54,    54,    38,   139,    42,    71,   171,
   172,   257,    82,   275,    84,   214,   122,   177,   199,    61,
    52,   255,   256,   264,   249,   107,   243,   173,   174,   300,
    71,    68,   228,   193,    38,    62,   232,   105,   211,   308,
   218,    52,   250,   112,   252,   277,    77,    69,   239,   281,
   183,   229,   240,   106,   194,   260,    23,   244,    71,   113,
   195,   133,   242,    45,    74,   266,     3,     4,     5,     6,
    75,    76,   148,    40,     8,   282,    41,   135,   259,   193,
    77,    78,    54,    42,   154,    79,    80,    81,    82,    83,
    84,   270,    85,   193,   234,   271,   254,   235,   258,   114,
   236,   272,   279,    36,    86,     4,     5,   193,   306,   307,
   193,    40,    36,   127,    41,   128,    54,   245,   138,   297,
   246,    42,   153,   271,   194,   138,   129,   301,    84,   272,
   160,   304,   305,   219,   220,   292,   155,   309,   310,    71,
   286,   124,   161,   311,   296,   125,   156,   136,   221,   157,
    34,    36,   222,   319,   293,   294,     3,     4,     5,   302,
   303,   343,     2,   158,     3,     4,     5,     6,    71,   336,
     7,   225,     8,     9,  -132,   226,   329,   338,   289,   290,
   165,   193,   330,   331,   332,   166,   321,   322,   200,    10,
   342,   193,   289,   291,   298,   299,   327,   328,   167,   178,
   201,   213,   193,   216,   217,   223,   230,   193,   231,   233,
   263,   324,   340,    74,   211,     3,     4,     5,     6,    75,
    76,   173,    40,     8,   273,    41,   276,   278,   287,    77,
    78,   280,    42,   295,    79,    80,    81,    82,    83,    84,
   283,    85,   313,   315,   341,   181,   316,   317,   320,   325,
   334,   326,    74,    86,     3,     4,     5,     6,    75,    76,
   333,    40,     8,   335,    41,   337,   339,   344,    77,    78,
    14,    42,    16,    79,    80,    81,    82,    83,    84,    26,
    85,     4,     5,   323,   215,   285,   152,    40,   288,   150,
    41,   111,    86,   237,   187,   314,   163,    42,   118,     4,
     5,     4,     5,   196,    43,    40,   238,    40,    41,   188,
    41,   312,   187,   109,   187,    42,     0,    42,     0,     4,
     5,   227,    43,     0,    43,    40,     0,   241,    41,   318,
     0,     0,   202,     0,     0,    42,     0,   203,   204,   205,
     4,     5,    43,     0,   206,     0,    40,     0,     0,    41,
     0,     0,     0,   284,     0,     0,    42,     0,   203,   204,
   205,     0,     0,    43,     0,   206,   169,     4,     5,     0,
    53,     0,     0,    40,     0,     0,    41,     3,     4,     5,
    54,     0,     0,    42,    40,     0,     0,    41,   170,     0,
    43,     0,     0,     0,    42,     0,     4,     5,     0,    53,
     0,    43,    40,     0,     0,    41,     0,     4,     5,    77,
    53,     0,    42,    40,     0,     0,    41,    82,     0,    43,
    54,     0,     0,    42,     0,     0,     4,     5,    82,    53,
    43,     0,    40,     0,     0,    41,     0,     4,     5,    54,
    53,     0,    42,    40,     0,     0,    41,     0,     0,    43,
   253,     0,     0,    42,     0,     4,     5,     4,     5,     0,
    43,    40,     0,    40,    41,     0,    41,     0,   187,     0,
     0,    42,     0,    42,     4,     5,     0,     0,    43,     0,
    43,     0,     0,    41,     0,     0,     0,     0,     0,     0,
    42,     0,     0,     0,     0,     0,     0,    43
};

static const short yycheck[] = {    23,
     6,     1,    26,    82,   148,    29,    10,    10,     8,     8,
    76,   162,    85,   103,   103,     1,    10,     7,    38,    85,
    17,    15,   101,    47,    24,   176,    75,    21,   179,    35,
    30,    17,     3,    17,    10,    90,    85,    86,    43,    15,
    20,    47,    18,    18,    10,    21,   100,    33,    72,    15,
    44,    41,    18,    57,    53,    21,   111,    20,    38,    35,
    44,     4,     5,    35,     7,    89,    13,    10,    44,    17,
    13,    95,    44,    17,    17,    38,    75,    20,    44,    83,
    83,    25,    25,    31,    27,   109,    20,   160,   177,     1,
    90,   170,   171,   183,   160,    35,   150,    31,    41,   250,
    44,    18,   126,   102,    38,    17,   130,    18,   107,   260,
   116,   111,   161,    18,   168,   194,    17,    34,   142,   198,
    36,   127,   146,    34,    25,   174,     1,   151,    44,    34,
    31,    30,   276,     8,     1,   189,     3,     4,     5,     6,
     7,     8,    31,    10,    11,   199,    13,    13,   172,   148,
    17,    18,    17,    20,    36,    22,    23,    24,    25,    26,
    27,    32,    29,   162,    15,    36,   169,    18,   172,    13,
    21,    42,   196,    17,    41,     4,     5,   176,   257,   258,
   179,    10,    17,    18,    13,    20,    17,    14,    52,    32,
    17,    20,    18,    36,    25,    59,    31,   251,    27,    42,
    29,   255,   256,    36,    37,   229,    21,   261,   262,    44,
   209,    13,    41,   267,   238,    17,    34,    13,    13,    18,
   220,    17,    17,   277,   230,   231,     3,     4,     5,   253,
   254,     0,     1,    16,     3,     4,     5,     6,    44,   318,
     9,    13,    11,    12,    31,    17,   300,   326,    36,    37,
    35,   250,   306,   307,   308,    35,   280,   281,    34,    28,
   339,   260,    36,    37,    39,    40,   298,   299,    35,    27,
    18,    18,   271,    34,    18,    13,    30,   276,    30,    30,
    17,   287,   336,     1,   283,     3,     4,     5,     6,     7,
     8,    31,    10,    11,    21,    13,    31,    30,     3,    17,
    18,    16,    20,    15,    22,    23,    24,    25,    26,    27,
    36,    29,    13,    42,   338,    33,    16,    30,    17,    13,
    42,    32,     1,    41,     3,     4,     5,     6,     7,     8,
    19,    10,    11,    17,    13,    32,    32,     0,    17,    18,
     1,    20,     1,    22,    23,    24,    25,    26,    27,     1,
    29,     4,     5,   283,    33,   209,    63,    10,   220,    57,
    13,    30,    41,   142,    17,   271,    76,    20,    40,     4,
     5,     4,     5,   103,    27,    10,   142,    10,    13,    32,
    13,   267,    17,    29,    17,    20,    -1,    20,    -1,     4,
     5,   123,    27,    -1,    27,    10,    -1,    32,    13,    32,
    -1,    -1,    17,    -1,    -1,    20,    -1,    22,    23,    24,
     4,     5,    27,    -1,    29,    -1,    10,    -1,    -1,    13,
    -1,    -1,    -1,    17,    -1,    -1,    20,    -1,    22,    23,
    24,    -1,    -1,    27,    -1,    29,     3,     4,     5,    -1,
     7,    -1,    -1,    10,    -1,    -1,    13,     3,     4,     5,
    17,    -1,    -1,    20,    10,    -1,    -1,    13,    25,    -1,
    27,    -1,    -1,    -1,    20,    -1,     4,     5,    -1,     7,
    -1,    27,    10,    -1,    -1,    13,    -1,     4,     5,    17,
     7,    -1,    20,    10,    -1,    -1,    13,    25,    -1,    27,
    17,    -1,    -1,    20,    -1,    -1,     4,     5,    25,     7,
    27,    -1,    10,    -1,    -1,    13,    -1,     4,     5,    17,
     7,    -1,    20,    10,    -1,    -1,    13,    -1,    -1,    27,
    17,    -1,    -1,    20,    -1,     4,     5,     4,     5,    -1,
    27,    10,    -1,    10,    13,    -1,    13,    -1,    17,    -1,
    -1,    20,    -1,    20,     4,     5,    -1,    -1,    27,    -1,
    27,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,
    20,    -1,    -1,    -1,    -1,    -1,    -1,    27
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 112 "mta_parse.y"
{ mta->yy_state = MTA::YYRet_Exit; ;
    break;}
case 2:
#line 113 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok;
	    return mta->yy_state; ;
    break;}
case 3:
#line 116 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 4:
#line 118 "mta_parse.y"
{
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 5:
#line 121 "mta_parse.y"
{
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 6:
#line 124 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 7:
#line 126 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 8:
#line 128 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 9:
#line 130 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; ;
    break;}
case 10:
#line 135 "mta_parse.y"
{ ;
    break;}
case 11:
#line 138 "mta_parse.y"
{
  	  if(yyvsp[0].typ != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("typedef", sp, yyval.typ); } ;
    break;}
case 12:
#line 146 "mta_parse.y"
{ mta->Burp(); ;
    break;}
case 13:
#line 147 "mta_parse.y"
{
	    if(yyvsp[-1].typ != NULL)  SETDESC(yyvsp[-1].typ,yyvsp[0].chr); ;
    break;}
case 14:
#line 151 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 15:
#line 152 "mta_parse.y"
{
	    yyval.typ = yyvsp[-2].typ; yyvsp[-2].typ->name = yyvsp[-1].chr; mta->type_stack.Pop(); ;
    break;}
case 16:
#line 156 "mta_parse.y"
{
            yyval.typ = yyvsp[-1].typ; yyvsp[-1].typ->AddParent(yyvsp[-2].typ); yyvsp[-1].typ->ptr = yyvsp[-2].typ->ptr;
	    yyvsp[-1].typ->par_formal.BorrowUnique(yyvsp[-2].typ->par_formal);
	    yyvsp[-1].typ->par_cache.BorrowUnique(yyvsp[-2].typ->par_cache);
	    mta->type_stack.Pop(); ;
    break;}
case 17:
#line 161 "mta_parse.y"
{ /* annoying place for a comment, but.. */
            yyval.typ = yyvsp[-1].typ; yyvsp[-1].typ->AddParent(yyvsp[-3].typ); yyvsp[-1].typ->ptr = yyvsp[-3].typ->ptr;
	    yyvsp[-1].typ->par_formal.BorrowUnique(yyvsp[-3].typ->par_formal);
	    yyvsp[-1].typ->par_cache.BorrowUnique(yyvsp[-3].typ->par_cache);
	    mta->type_stack.Pop(); ;
    break;}
case 18:
#line 168 "mta_parse.y"
{
	    TypeDef* td = yyvsp[-1].typ->parents[1]; mta->type_stack.Pop();
	    TypeSpace* sp = yyvsp[-1].typ->owner;
	    sp->RemoveEl(yyvsp[-1].typ); /* get rid of new one, cuz it is bogus */
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
		    td->parents.ReplaceLinkIdx(0, already_there);
		}
	      }
	    }
	    yyval.typ = td; ;
    break;}
case 19:
#line 198 "mta_parse.y"
{
	    yyval.typ = yyvsp[-3].typ; yyval.typ->AddParent(&TA_void_ptr); yyval.typ->ptr = 1;
	    mta->type_stack.Pop(); ;
    break;}
case 20:
#line 201 "mta_parse.y"
{
	    yyval.typ = yyvsp[0].typ; yyval.typ->AddParent(&TA_void_ptr); yyval.typ->ptr = 1;
	    mta->type_stack.Pop(); ;
    break;}
case 21:
#line 206 "mta_parse.y"
{
            TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
            yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("enum", sp, yyval.typ); ;
    break;}
case 24:
#line 216 "mta_parse.y"
{ SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); ;
    break;}
case 25:
#line 217 "mta_parse.y"
{ SETDESC(yyvsp[-2].typ,yyvsp[0].chr); ;
    break;}
case 26:
#line 220 "mta_parse.y"
{
  	    yyval.typ = yyvsp[0].typ;
	    yyvsp[0].typ->AddParFormal(&TA_enum); mta->cur_enum = yyvsp[0].typ;
	    mta->type_stack.Pop(); ;
    break;}
case 27:
#line 224 "mta_parse.y"
{
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->cur_enum = yyval.typ;
	    yyval.typ->AddParFormal(&TA_enum); yyval.typ->internal = true; ;
    break;}
case 28:
#line 231 "mta_parse.y"
{
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("class", sp, yyval.typ);
	    mta->type_stack.Pop(); ;
    break;}
case 29:
#line 239 "mta_parse.y"
{ mta->Burp(); ;
    break;}
case 30:
#line 240 "mta_parse.y"
{ SETDESC(yyvsp[-2].typ,yyvsp[0].chr); ;
    break;}
case 31:
#line 244 "mta_parse.y"
{
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("class", sp, yyval.typ);
	    mta->type_stack.Pop(); ;
    break;}
case 34:
#line 257 "mta_parse.y"
{
	    if(yyvsp[-2].typ->HasOption("NO_TOKENS")) yyvsp[-2].typ->tokens.keep = false;
	    else yyvsp[-2].typ->tokens.keep = true; ;
    break;}
case 35:
#line 263 "mta_parse.y"
{
	    yyvsp[-1].typ->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); ;
    break;}
case 36:
#line 266 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 37:
#line 268 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[0].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 38:
#line 273 "mta_parse.y"
{ mta->state = MTA::Parse_inclass; ;
    break;}
case 39:
#line 274 "mta_parse.y"
{ mta->state = MTA::Parse_inclass; ;
    break;}
case 40:
#line 277 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
            yyval.typ = yyvsp[0].typ; mta->last_class = mta->cur_class; mta->cur_class = yyvsp[0].typ;
	    yyvsp[0].typ->AddParFormal(&TA_class);
            mta->cur_mstate = MTA::prvt; ;
    break;}
case 41:
#line 282 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
	    yyval.typ = yyvsp[0].typ; mta->last_class = mta->cur_class; mta->cur_class = yyvsp[0].typ;
            mta->cur_mstate = MTA::prvt; ;
    break;}
case 42:
#line 286 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
	    String nm = yyvsp[0].typ->name + "_" + (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->type_stack.Push(yyval.typ);
	    mta->last_class = mta->cur_class; mta->cur_class = yyval.typ;
            mta->cur_mstate = MTA::prvt; ;
    break;}
case 43:
#line 292 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
	    String nm = yyvsp[0].typ->name + "_" + (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->type_stack.Push(yyval.typ);
	    mta->last_class = mta->cur_class; mta->cur_class = yyval.typ;
	    mta->cur_mstate = MTA::pblc; ;
    break;}
case 44:
#line 301 "mta_parse.y"
{
            if(yyvsp[0].typ->InheritsFrom(TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent(yyvsp[0].typ); ;
    break;}
case 45:
#line 304 "mta_parse.y"
{
            if(yyvsp[0].typ->InheritsFrom(&TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent(yyvsp[0].typ);
	    if(!mta->cur_class->HasOption("MULT_INHERIT"))
	      mta->cur_class->opts.Add("MULT_INHERIT"); ;
    break;}
case 47:
#line 313 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 48:
#line 314 "mta_parse.y"
{ yyval.typ = &TA_class; ;
    break;}
case 49:
#line 315 "mta_parse.y"
{ yyval.typ = &TA_class; ;
    break;}
case 54:
#line 327 "mta_parse.y"
{
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("template", sp, yyval.typ);
	    mta->type_stack.Pop(); ;
    break;}
case 57:
#line 340 "mta_parse.y"
{
          if(yyvsp[-2].typ->HasOption("NO_TOKENS")) yyvsp[-2].typ->tokens.keep = false;
	  else yyvsp[-2].typ->tokens.keep = true; ;
    break;}
case 58:
#line 346 "mta_parse.y"
{
            mta->state = MTA::Parse_inclass; yyvsp[-1].typ->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); ;
    break;}
case 59:
#line 349 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 60:
#line 351 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[0].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 61:
#line 356 "mta_parse.y"
{
	    yyvsp[0].typ->templ_pars.Reset();
	    yyvsp[0].typ->templ_pars.Duplicate(mta->cur_templ_pars);
	    yyvsp[0].typ->internal = true;
	    yyvsp[0].typ->AddParFormal(&TA_template); yyval.typ = yyvsp[0].typ; ;
    break;}
case 62:
#line 363 "mta_parse.y"
{ mta->cur_templ_pars.Reset(); ;
    break;}
case 63:
#line 367 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[0].typ; ;
    break;}
case 64:
#line 368 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[-3].typ; ;
    break;}
case 65:
#line 371 "mta_parse.y"
{
            if(mta->spc == &(mta->spc_target)) /* only add reg_funs in target space */
	       TA_taRegFun.methods.AddUniqNameNew(yyvsp[0].meth);
	    mta->meth_stack.Pop(); ;
    break;}
case 66:
#line 377 "mta_parse.y"
{
            yyvsp[-1].meth->type = yyvsp[-2].typ; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); yyval.meth = yyvsp[-1].meth; ;
    break;}
case 67:
#line 379 "mta_parse.y"
{
            yyvsp[-1].meth->type = &TA_int; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); yyval.meth = yyvsp[-1].meth; ;
    break;}
case 68:
#line 383 "mta_parse.y"
{
            yyvsp[-1].meth->is_static = true; /* consider these to be static functions */
            yyvsp[-1].meth->fun_argc = yyvsp[0].rval; yyvsp[-1].meth->arg_types.size = yyvsp[0].rval; mta->burp_fundefn = true; ;
    break;}
case 72:
#line 393 "mta_parse.y"
{ if(yyvsp[-1].enm != NULL) SETENUMDESC(yyvsp[-1].enm,yyvsp[0].chr); ;
    break;}
case 75:
#line 398 "mta_parse.y"
{ /* trying to do some math */
           yyval.enm = NULL;
	   mta->skiptocommarb(); ;
    break;}
case 76:
#line 403 "mta_parse.y"
{
            mta->cur_enum->enum_vals.Add(yyvsp[0].enm);
	    mta->enum_stack.Pop(); ;
    break;}
case 77:
#line 406 "mta_parse.y"
{ /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add(yyvsp[-2].enm); if(yyvsp[0].rval != -424242) yyvsp[-2].enm->enum_no = yyvsp[0].rval;
	    mta->enum_stack.Pop(); ;
    break;}
case 79:
#line 413 "mta_parse.y"
{
	  if((yyvsp[-2].rval != -424242) && (yyvsp[0].rval != -424242))  yyval.rval = yyvsp[-2].rval + yyvsp[0].rval;
	  else if(yyvsp[-2].rval != -424242)	yyval.rval = yyvsp[-2].rval;
	  else if(yyvsp[0].rval != -424242)	yyval.rval = yyvsp[0].rval;
	  else				yyval.rval = -424242; ;
    break;}
case 80:
#line 418 "mta_parse.y"
{
	  if((yyvsp[-2].rval != -424242) && (yyvsp[0].rval != -424242)) yyval.rval = yyvsp[-2].rval - yyvsp[0].rval;
	  else if(yyvsp[-2].rval != -424242)	yyval.rval = yyvsp[-2].rval;
	  else if(yyvsp[0].rval != -424242)	yyval.rval = yyvsp[0].rval;
	  else				yyval.rval = -424242; ;
    break;}
case 81:
#line 426 "mta_parse.y"
{ yyval.rval = -424242; ;
    break;}
case 83:
#line 431 "mta_parse.y"
{ yyval.enm = new EnumDef(yyvsp[0].chr); mta->enum_stack.Push(yyval.enm); ;
    break;}
case 84:
#line 434 "mta_parse.y"
{ mta->Class_UpdateLastPtrs(); ;
    break;}
case 85:
#line 435 "mta_parse.y"
{ mta->Class_UpdateLastPtrs(); ;
    break;}
case 86:
#line 438 "mta_parse.y"
{
            if(yyvsp[0].memb != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->HasOption("IGNORE"))
		 && !(yyvsp[0].memb->type->DerivesFrom(TA_const))) {
		mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
		if(mta->verbose >= 3)
		  cerr << "member: " << yyvsp[0].memb->name << " added to: "
		       << mta->cur_class->name << "\n"; } }
	    mta->memb_stack.Pop(); yyval.typ = NULL; ;
    break;}
case 87:
#line 447 "mta_parse.y"
{
            if(yyvsp[0].meth != NULL) {
	      if(mta->cur_mstate == MTA::pblc) {
		if(yyvsp[0].meth->HasOption("IGNORE"))
		  mta->cur_class->ignore_meths.AddUnique(yyvsp[0].meth->name);
		else {
		  mta->cur_class->methods.AddUniqNameNew(yyvsp[0].meth);
		  if(mta->verbose >= 3)
		    cerr << "method: " << yyvsp[0].meth->name << " added to: "
			 << mta->cur_class->name << "\n"; } } }
	    else {
	      mta->cur_meth = NULL; }
	    mta->meth_stack.Pop(); yyval.typ = NULL; ;
    break;}
case 88:
#line 460 "mta_parse.y"
{
	    mta->cur_class->sub_types.AddUniqNameNew(yyvsp[0].typ);
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; ;
    break;}
case 89:
#line 463 "mta_parse.y"
{
	    mta->cur_class->sub_types.AddUniqNameNew(yyvsp[0].typ);
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; ;
    break;}
case 90:
#line 466 "mta_parse.y"
{ /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew(yyvsp[-1].typ);
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; ;
    break;}
case 91:
#line 470 "mta_parse.y"
{ yyval.typ = NULL; ;
    break;}
case 93:
#line 475 "mta_parse.y"
{ yyval.memb = NULL; mta->cur_mstate = MTA::pblc; ;
    break;}
case 94:
#line 476 "mta_parse.y"
{ yyval.memb = NULL; mta->cur_mstate = MTA::prvt; ;
    break;}
case 95:
#line 477 "mta_parse.y"
{ yyval.memb = NULL; mta->cur_mstate = MTA::prot; ;
    break;}
case 96:
#line 478 "mta_parse.y"
{
	    yyval.memb = NULL;
	    if(mta->last_memb != NULL) {
	      SETDESC(mta->last_memb, yyvsp[0].chr);
	      if(mta->last_memb->HasOption("IGNORE"))
		mta->cur_class->members.RemoveEl(mta->last_memb); }
	    else if(mta->last_meth != NULL) {
	      SETDESC(mta->last_meth, yyvsp[0].chr);
	      if(mta->last_meth->HasOption("IGNORE")) {
		mta->cur_class->ignore_meths.AddUnique(mta->last_meth->name);
		mta->cur_class->methods.RemoveEl(mta->last_meth);
		mta->last_meth = NULL; }
	      else if((mta->last_meth->opts.size > 0) || (mta->last_meth->lists.size > 0)) {
		mta->cur_class->methods.AddUniqNameNew(mta->last_meth);
		if(mta->verbose >= 3)
		  cerr << "method: " << mta->last_meth->name << " added to: "
		       << mta->cur_class->name << "\n"; } } ;
    break;}
case 98:
#line 499 "mta_parse.y"
{
	  yyval.memb = yyvsp[0].memb; if(yyvsp[0].memb != NULL) yyvsp[0].memb->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; ;
    break;}
case 99:
#line 505 "mta_parse.y"
{ yyval.memb = yyvsp[-1].memb; ;
    break;}
case 100:
#line 506 "mta_parse.y"
{ ;
    break;}
case 101:
#line 507 "mta_parse.y"
{
	    yyval.memb = yyvsp[-2].memb;
	    String nm = yyvsp[-3].typ->name + "_ary";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[-3].typ->ptr + 1);
	    nty->AddParFormal(&TA_ta_array); nty->AddParent(yyvsp[-3].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-3].typ);
	    TypeDef* uty = sp->AddUniqNameOld(nty); yyvsp[-2].memb->type = uty;
	    if(uty == nty) mta->TypeAdded("array", sp, uty); ;
    break;}
case 102:
#line 515 "mta_parse.y"
{ yyvsp[-2].memb->type = yyvsp[-3].typ; yyval.memb = yyvsp[-2].memb; ;
    break;}
case 103:
#line 519 "mta_parse.y"
{
	     if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
             mta->memb_stack.Pop(); yyval.memb = NULL; ;
    break;}
case 104:
#line 523 "mta_parse.y"
{
	     if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
             mta->memb_stack.Pop(); yyval.memb = NULL; ;
    break;}
case 105:
#line 529 "mta_parse.y"
{
            yyval.memb = new MemberDef(yyvsp[0].chr); mta->cur_memb = yyval.memb; mta->memb_stack.Push(yyval.memb);
            if(mta->cur_memb_type != NULL) yyval.memb->type = mta->cur_memb_type;
            else yyval.memb->type = &TA_int; ;
    break;}
case 106:
#line 535 "mta_parse.y"
{
	    yyval.memb = new MemberDef(yyvsp[-1].chr); mta->cur_memb = yyval.memb; mta->memb_stack.Push(yyval.memb);
	    yyval.memb->fun_ptr = 1; ;
    break;}
case 108:
#line 541 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 109:
#line 542 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 110:
#line 543 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 111:
#line 544 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 112:
#line 545 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 113:
#line 546 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 114:
#line 547 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 115:
#line 548 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 116:
#line 549 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 117:
#line 550 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 118:
#line 551 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 119:
#line 552 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 120:
#line 553 "mta_parse.y"
{
	    yyval.meth = NULL; String tmp = yyvsp[0].chr;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew(yyvsp[-2].meth); yyvsp[-2].meth->type = yyvsp[-3].typ;
	      mta->meth_stack.Pop();  yyvsp[-2].meth->fun_argc = yyvsp[-1].rval; yyvsp[-2].meth->arg_types.size = yyvsp[-1].rval;
	      yyvsp[-2].meth->is_static = true; /* consider these to be static functions */
	      SETDESC(yyvsp[-2].meth,yyvsp[0].chr); }
	    else { yyval.meth = NULL; mta->meth_stack.Pop(); } ;
    break;}
case 121:
#line 561 "mta_parse.y"
{
	    yyval.meth = NULL; String tmp = yyvsp[0].chr;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew(yyvsp[-2].meth); yyvsp[-2].meth->type = &TA_int;
	      mta->meth_stack.Pop();  yyvsp[-2].meth->fun_argc = yyvsp[-1].rval; yyvsp[-2].meth->arg_types.size = yyvsp[-1].rval;
	      yyvsp[-2].meth->is_static = true; /* consider these to be static functions */
	      SETDESC(yyvsp[-2].meth,yyvsp[0].chr); }
	    else { yyval.meth = 0; mta->meth_stack.Pop(); } ;
    break;}
case 123:
#line 573 "mta_parse.y"
{ yyval.meth = yyvsp[0].meth; yyvsp[0].meth->is_static = true; ;
    break;}
case 124:
#line 574 "mta_parse.y"
{ yyval.meth = yyvsp[0].meth;  if(yyvsp[0].meth != NULL) yyvsp[0].meth->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; ;
    break;}
case 125:
#line 576 "mta_parse.y"
{ yyval.meth = yyvsp[0].meth;  if(yyvsp[0].meth != NULL) yyvsp[0].meth->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; ;
    break;}
case 126:
#line 581 "mta_parse.y"
{ yyval.meth = yyvsp[-1].meth; yyvsp[-1].meth->type = yyvsp[-2].typ; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); ;
    break;}
case 127:
#line 582 "mta_parse.y"
{ yyvsp[-1].meth->type = &TA_int; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); ;
    break;}
case 128:
#line 583 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 129:
#line 584 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 130:
#line 585 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 131:
#line 588 "mta_parse.y"
{
             yyvsp[-1].meth->fun_argc = yyvsp[0].rval; yyvsp[-1].meth->arg_types.size = yyvsp[0].rval; mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if(yyvsp[-1].meth->fun_argd > yyvsp[-1].meth->fun_argc) yyvsp[-1].meth->fun_argd = -1; ;
    break;}
case 132:
#line 595 "mta_parse.y"
{
            yyval.meth = new MethodDef(yyvsp[0].chr); mta->cur_meth = yyval.meth; mta->meth_stack.Push(yyval.meth); ;
    break;}
case 133:
#line 599 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 134:
#line 600 "mta_parse.y"
{ yyval.chr = yyvsp[0].chr; ;
    break;}
case 135:
#line 601 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 136:
#line 602 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 137:
#line 603 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 138:
#line 604 "mta_parse.y"
{ yyval.chr = yyvsp[-2].chr; ;
    break;}
case 139:
#line 608 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 140:
#line 609 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 141:
#line 610 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 142:
#line 614 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 143:
#line 615 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 144:
#line 616 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 145:
#line 619 "mta_parse.y"
{ yyval.rval = 0; ;
    break;}
case 146:
#line 620 "mta_parse.y"
{ yyval.rval = yyvsp[-1].rval; ;
    break;}
case 147:
#line 623 "mta_parse.y"
{ yyval.rval = 0; ;
    break;}
case 148:
#line 624 "mta_parse.y"
{ yyval.rval = yyvsp[-1].rval; ;
    break;}
case 149:
#line 627 "mta_parse.y"
{ yyval.rval = 1; ;
    break;}
case 150:
#line 628 "mta_parse.y"
{ yyval.rval = yyvsp[-2].rval + 1; ;
    break;}
case 151:
#line 629 "mta_parse.y"
{ yyval.rval = yyvsp[-3].rval; ;
    break;}
case 152:
#line 632 "mta_parse.y"
{
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } ;
    break;}
case 153:
#line 634 "mta_parse.y"
{
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_defs.Add(yyvsp[0].chr);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } ;
    break;}
case 154:
#line 642 "mta_parse.y"
{
	    yyval.rval = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[0].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 155:
#line 646 "mta_parse.y"
{
	    yyval.rval = 1; String nm = yyvsp[0].chr;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-1].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 156:
#line 650 "mta_parse.y"
{
	    yyval.rval = 1; String nm = String(yyvsp[-1].chr) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-2].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 157:
#line 654 "mta_parse.y"
{
	    yyval.rval = 1; String nm = String("(*") + String(yyvsp[-2].chr) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-5].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 158:
#line 658 "mta_parse.y"
{
	    yyval.rval = 1; String nm = yyvsp[0].chr;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 162:
#line 673 "mta_parse.y"
{
            mta->thisname = true; mta->constcoln = false; ;
    break;}
case 163:
#line 677 "mta_parse.y"
{ mta->constcoln = true; ;
    break;}
case 164:
#line 680 "mta_parse.y"
{ yyval.rval = 1; ;
    break;}
case 165:
#line 681 "mta_parse.y"
{ yyval.rval = yyvsp[-1].rval + 1; ;
    break;}
case 166:
#line 684 "mta_parse.y"
{ mta->cur_memb_type = yyvsp[0].typ; ;
    break;}
case 168:
#line 688 "mta_parse.y"
{ yyval.typ = &TA_int; ;
    break;}
case 169:
#line 689 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 170:
#line 692 "mta_parse.y"
{ yyval.typ = new TypeDef(yyvsp[0].chr); mta->type_stack.Push(yyval.typ); ;
    break;}
case 172:
#line 696 "mta_parse.y"
{
	    String nm = yyvsp[-1].typ->name + "_ref";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[-1].typ->ptr, true);
	    nty->AddParent(yyvsp[-1].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-1].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("ref", sp, yyval.typ); ;
    break;}
case 174:
#line 707 "mta_parse.y"
{
 	    int i; String nm = yyvsp[-1].typ->name; for(i=0; i<yyvsp[0].rval; i++) nm += "_ptr";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[0].rval); nty->AddParent(yyvsp[-1].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-1].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("ptr", sp, yyval.typ); ;
    break;}
case 176:
#line 716 "mta_parse.y"
{
	    String nm = yyvsp[-1].typ->name + "_" + yyvsp[0].typ->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = yyvsp[0].typ->size; nty->AddParent(yyvsp[-1].typ); nty->AddParent(yyvsp[0].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("const", sp, yyval.typ); ;
    break;}
case 178:
#line 726 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 179:
#line 727 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 180:
#line 728 "mta_parse.y"
{
	    TypeDef* td; if((td = yyvsp[-2].typ->sub_types.FindName(yyvsp[0].chr)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    yyval.typ = td; ;
    break;}
case 181:
#line 732 "mta_parse.y"
{
	    TypeDef* td; if((td = yyvsp[-2].typ->sub_types.FindName(yyvsp[0].chr)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    yyval.typ = td; ;
    break;}
case 182:
#line 736 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 183:
#line 737 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 184:
#line 738 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 186:
#line 740 "mta_parse.y"
{ /* a template */
 	    if(!(yyvsp[-3].typ->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    if((yyvsp[-1].typ->owner != NULL) && (yyvsp[-1].typ->owner->owner != NULL))
	      yyval.typ = yyvsp[-3].typ;	/* don't allow internal types with external templates */
	    else {
	      String nm = yyvsp[-3].typ->GetTemplName(mta->cur_templ_pars);
	      TypeDef* td;
	      int lx_tok;
	      if((td = mta->FindName(nm, lx_tok)) == NULL) {
		td = yyvsp[-3].typ->Clone(); td->name = nm;
		td->SetTemplType(yyvsp[-3].typ, mta->cur_templ_pars);
		TypeSpace* sp = mta->GetTypeSpace(yyvsp[-3].typ);
 		yyval.typ = sp->AddUniqNameOld(td);
		if(yyval.typ == td) mta->TypeAdded("template instance", sp, yyval.typ); }
	      else
		yyval.typ = td; } ;
    break;}
case 187:
#line 757 "mta_parse.y"
{ /* this template */
	    if(!(yyvsp[-3].typ->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    yyval.typ = yyvsp[-3].typ; ;
    break;}
case 189:
#line 764 "mta_parse.y"
{
	    String nm = yyvsp[-1].typ->name + "_" + yyvsp[0].typ->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = yyvsp[0].typ->size; nty->AddParent(yyvsp[-1].typ); nty->AddParent(yyvsp[0].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("combo", sp, yyval.typ); ;
    break;}
case 190:
#line 774 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); ;
    break;}
case 191:
#line 775 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[-2].typ; ;
    break;}
case 193:
#line 779 "mta_parse.y"
{ yyval.chr = yyvsp[0].typ->name; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 803 "mta_parse.y"


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
