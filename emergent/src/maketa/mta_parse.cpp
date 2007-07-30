
/*  A Bison parser, made from mta_parse.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	CLASS	257
#define	TYPENAME	258
#define	STRUCT	259
#define	UNION	260
#define	ENUM	261
#define	FUNTYPE	262
#define	STATIC	263
#define	TEMPLATE	264
#define	CONST	265
#define	TYPEDEF	266
#define	TA_TYPEDEF	267
#define	TYPE	268
#define	NUMBER	269
#define	FUNCTION	270
#define	ARRAY	271
#define	NAME	272
#define	COMMENT	273
#define	FUNCALL	274
#define	SCOPER	275
#define	EQUALS	276
#define	PUBLIC	277
#define	PRIVATE	278
#define	PROTECTED	279
#define	OPERATOR	280
#define	FRIEND	281
#define	THISNAME	282
#define	REGFUN	283
#define	VIRTUAL	284

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



#define	YYFINAL		354
#define	YYFLAG		-32768
#define	YYNTBASE	46

#define YYTRANSLATE(x) ((unsigned)(x) <= 284 ? yytranslate[x] : 128)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    44,     2,    32,
    33,    31,    40,    37,    41,    43,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    36,    45,    39,
     2,    38,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    35,     2,    34,    42,     2,     2,     2,     2,
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
    27,    28,    29,    30
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     7,    10,    13,    16,    19,    22,    25,
    27,    29,    31,    34,    37,    42,    46,    51,    54,    62,
    67,    69,    74,    77,    81,    85,    88,    90,    92,    95,
    99,   101,   104,   108,   112,   115,   119,   123,   125,   129,
   132,   135,   137,   139,   141,   145,   147,   150,   152,   155,
   157,   160,   162,   164,   166,   169,   173,   177,   180,   184,
   188,   194,   196,   198,   202,   205,   208,   211,   215,   217,
   222,   226,   229,   231,   234,   236,   239,   241,   244,   246,
   248,   252,   254,   258,   262,   264,   266,   268,   270,   273,
   275,   277,   279,   281,   284,   286,   288,   291,   294,   297,
   299,   301,   304,   308,   313,   316,   321,   326,   328,   332,
   334,   339,   341,   345,   350,   355,   360,   365,   371,   377,
   383,   388,   392,   397,   402,   408,   413,   415,   418,   421,
   425,   429,   432,   437,   444,   448,   451,   453,   455,   457,
   460,   463,   466,   470,   472,   475,   479,   481,   483,   486,
   489,   493,   495,   498,   500,   504,   509,   511,   514,   516,
   519,   523,   530,   532,   534,   537,   541,   544,   546,   548,
   551,   553,   555,   557,   560,   562,   564,   567,   569,   572,
   574,   577,   579,   582,   585,   589,   593,   597,   601,   604,
   606,   611,   616,   618,   621,   623,   627,   629,   631,   633,
   635,   637,   639,   642,   644,   646,   648,   650,   652,   654
};

static const short yyrhs[] = {    -1,
    46,    48,     0,    46,    52,     0,    46,    56,     0,    46,
    58,     0,    46,    68,     0,    46,    76,     0,    46,    47,
     0,    46,     1,     0,    13,     0,    49,     0,    50,     0,
    50,    19,     0,    12,    51,     0,    12,    60,   122,   124,
     0,   115,   114,   124,     0,   115,    19,   114,   124,     0,
   115,   124,     0,   115,    32,    31,   114,    33,   102,   124,
     0,   115,    21,    31,   114,     0,    53,     0,    54,    79,
    34,   124,     0,    55,    35,     0,    55,    19,    35,     0,
    55,    35,    19,     0,     7,   114,     0,     7,     0,    57,
     0,    63,   124,     0,    63,   124,    19,     0,    59,     0,
    60,   124,     0,    60,   123,   124,     0,    61,    86,    34,
     0,    62,    35,     0,    62,    19,    35,     0,    62,    35,
    19,     0,    63,     0,    63,    36,    64,     0,   127,   114,
     0,   127,    14,     0,   127,     0,   126,     0,    65,     0,
    64,    37,    65,     0,   115,     0,    66,   115,     0,    18,
     0,    66,    18,     0,    67,     0,    66,    67,     0,   125,
     0,    30,     0,    69,     0,    70,   124,     0,    70,   123,
   124,     0,    71,    86,    34,     0,    72,    35,     0,    72,
    19,    35,     0,    72,    35,    19,     0,    10,    73,    74,
    38,    62,     0,    39,     0,    75,     0,    75,    37,    75,
     0,     3,   114,     0,     4,   114,     0,   115,   114,     0,
   115,   114,    22,     0,    77,     0,    29,   113,    78,    99,
     0,    29,    78,    99,     0,    98,   102,     0,    80,     0,
    79,    80,     0,    81,     0,    81,    19,     0,    82,     0,
    82,    37,     0,     1,     0,    85,     0,    85,    22,    83,
     0,    84,     0,    83,    40,    84,     0,    83,    41,    84,
     0,    18,     0,    15,     0,    18,     0,    87,     0,    86,
    87,     0,    88,     0,    94,     0,    53,     0,    50,     0,
    60,   124,     0,     1,     0,    89,     0,    23,    36,     0,
    24,    36,     0,    25,    36,     0,    19,     0,    90,     0,
     9,    90,     0,   112,    91,   124,     0,   112,    92,    22,
   124,     0,    91,   124,     0,   112,    92,    17,   124,     0,
   112,    93,   102,   124,     0,    92,     0,    91,    37,    92,
     0,    18,     0,    32,    31,    18,    33,     0,    95,     0,
   109,   103,    99,     0,     8,   109,   103,    99,     0,    30,
   109,   103,    99,     0,   109,   103,   107,    99,     0,    42,
   109,   103,    99,     0,     8,    42,   109,   103,    99,     0,
    30,    42,   109,   103,    99,     0,    27,   113,    26,   102,
    99,     0,    27,    26,   102,    99,     0,    27,   113,   124,
     0,    27,     3,   113,   124,     0,    27,     3,    18,   124,
     0,    27,   113,    98,   102,    99,     0,    27,    98,   102,
    99,     0,    96,     0,     9,    96,     0,    30,    96,     0,
     8,    30,    96,     0,   112,    97,    99,     0,    97,    99,
     0,   112,    26,   102,    99,     0,   112,    26,    32,    33,
   102,    99,     0,    26,   102,    99,     0,    98,   102,     0,
    18,     0,   124,     0,   100,     0,   101,   124,     0,    19,
   124,     0,    19,   100,     0,    19,   101,   124,     0,    16,
     0,    11,    16,     0,    11,    19,    16,     0,    22,     0,
    11,     0,    11,    22,     0,    32,    33,     0,    32,   104,
    33,     0,    33,     0,   104,    33,     0,   105,     0,   104,
    37,   105,     0,   104,    43,    43,    43,     0,   106,     0,
   106,    22,     0,   115,     0,   115,    18,     0,   115,    18,
    17,     0,   115,    32,    31,    18,    33,   102,     0,    18,
     0,   108,     0,   107,   108,     0,   110,    14,    20,     0,
    28,    32,     0,    36,     0,    31,     0,   111,    31,     0,
   113,     0,   115,     0,     8,     0,     8,   115,     0,    18,
     0,   116,     0,   116,    44,     0,   117,     0,   117,   111,
     0,   118,     0,    11,   118,     0,   119,     0,   126,   119,
     0,   126,   114,     0,    14,    21,    18,     0,    28,    21,
    18,     0,    14,    21,    14,     0,    28,    21,    14,     0,
    21,    14,     0,    28,     0,    14,    73,   120,    38,     0,
    28,    73,   120,    38,     0,    14,     0,   119,    14,     0,
   121,     0,   120,    37,   121,     0,    14,     0,    18,     0,
    15,     0,    18,     0,    14,     0,    18,     0,    18,    17,
     0,    45,     0,    23,     0,    24,     0,    25,     0,     5,
     0,     6,     0,     3,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   112,   113,   116,   118,   121,   124,   126,   128,   130,   134,
   138,   145,   147,   151,   152,   156,   161,   168,   204,   207,
   212,   218,   221,   222,   223,   226,   230,   236,   244,   246,
   249,   257,   259,   262,   268,   272,   274,   278,   280,   283,
   288,   292,   298,   307,   310,   318,   319,   320,   321,   324,
   325,   328,   329,   332,   340,   342,   345,   351,   355,   357,
   361,   369,   372,   374,   377,   379,   380,   381,   384,   390,
   392,   396,   401,   402,   405,   406,   409,   410,   411,   416,
   419,   424,   426,   431,   438,   440,   443,   447,   448,   451,
   460,   473,   476,   479,   483,   486,   488,   489,   490,   491,
   510,   512,   517,   519,   520,   521,   531,   534,   539,   545,
   551,   556,   557,   558,   559,   560,   561,   562,   563,   564,
   565,   566,   567,   568,   569,   577,   587,   589,   590,   592,
   596,   598,   599,   600,   601,   604,   611,   615,   616,   617,
   618,   619,   620,   623,   625,   626,   629,   631,   632,   635,
   636,   639,   640,   643,   644,   645,   648,   650,   660,   665,
   669,   673,   677,   683,   685,   688,   691,   696,   699,   700,
   703,   706,   707,   708,   711,   714,   715,   725,   727,   737,
   738,   749,   750,   751,   752,   756,   760,   761,   762,   763,
   764,   781,   787,   788,   799,   801,   805,   807,   808,   812,
   813,   816,   817,   820,   823,   824,   825,   828,   830,   833
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","CLASS",
"TYPENAME","STRUCT","UNION","ENUM","FUNTYPE","STATIC","TEMPLATE","CONST","TYPEDEF",
"TA_TYPEDEF","TYPE","NUMBER","FUNCTION","ARRAY","NAME","COMMENT","FUNCALL","SCOPER",
"EQUALS","PUBLIC","PRIVATE","PROTECTED","OPERATOR","FRIEND","THISNAME","REGFUN",
"VIRTUAL","'*'","'('","')'","'}'","'{'","':'","','","'>'","'<'","'+'","'-'",
"'~'","'.'","'&'","';'","list","preparsed","typedefn","typedefns","typedsub",
"defn","enumdefn","enumdsub","enumname","enumnm","classdecl","classdecls","classdefn",
"classdefns","classdsub","classname","classhead","classnm","classinh","classpar",
"classptyp","classpmod","templdefn","templdefns","templdsub","templname","templhead",
"templopen","templpars","templpar","fundecl","funnm","regfundefn","enums","enumline",
"enumitm","enumitms","enummath","enummathitm","enmitmname","membs","membline",
"membdefn","basicmemb","nostatmemb","membnames","membname","membfunp","methdefn",
"basicmeth","nostatmeth","mbfundefn","methname","fundefn","funsubdefn","funsubdecl",
"funargs","constfun","args","argdefn","subargdefn","constrlist","constref","consthsnm",
"constcoln","ptrs","membtype","ftype","tyname","type","noreftype","constype",
"subtype","combtype","templargs","templarg","tdname","varname","term","access",
"structstruct","classkeyword", NULL
};
#endif

static const short yyr1[] = {     0,
    46,    46,    46,    46,    46,    46,    46,    46,    46,    47,
    48,    49,    49,    50,    50,    51,    51,    51,    51,    51,
    52,    53,    54,    54,    54,    55,    55,    56,    57,    57,
    58,    59,    59,    60,    61,    61,    61,    62,    62,    63,
    63,    63,    63,    64,    64,    65,    65,    65,    65,    66,
    66,    67,    67,    68,    69,    69,    70,    71,    71,    71,
    72,    73,    74,    74,    75,    75,    75,    75,    76,    77,
    77,    78,    79,    79,    80,    80,    81,    81,    81,    82,
    82,    83,    83,    83,    84,    84,    85,    86,    86,    87,
    87,    87,    87,    87,    87,    88,    88,    88,    88,    88,
    89,    89,    90,    90,    90,    90,    90,    91,    91,    92,
    93,    94,    94,    94,    94,    94,    94,    94,    94,    94,
    94,    94,    94,    94,    94,    94,    95,    95,    95,    95,
    96,    96,    96,    96,    96,    97,    98,    99,    99,    99,
    99,    99,    99,   100,   100,   100,   101,   101,   101,   102,
   102,   103,   103,   104,   104,   104,   105,   105,   106,   106,
   106,   106,   106,   107,   107,   108,   109,   110,   111,   111,
   112,   113,   113,   113,   114,   115,   115,   116,   116,   117,
   117,   118,   118,   118,   118,   118,   118,   118,   118,   118,
   118,   118,   119,   119,   120,   120,   121,   121,   121,   122,
   122,   123,   123,   124,   125,   125,   125,   126,   126,   127
};

static const short yyr2[] = {     0,
     0,     2,     2,     2,     2,     2,     2,     2,     2,     1,
     1,     1,     2,     2,     4,     3,     4,     2,     7,     4,
     1,     4,     2,     3,     3,     2,     1,     1,     2,     3,
     1,     2,     3,     3,     2,     3,     3,     1,     3,     2,
     2,     1,     1,     1,     3,     1,     2,     1,     2,     1,
     2,     1,     1,     1,     2,     3,     3,     2,     3,     3,
     5,     1,     1,     3,     2,     2,     2,     3,     1,     4,
     3,     2,     1,     2,     1,     2,     1,     2,     1,     1,
     3,     1,     3,     3,     1,     1,     1,     1,     2,     1,
     1,     1,     1,     2,     1,     1,     2,     2,     2,     1,
     1,     2,     3,     4,     2,     4,     4,     1,     3,     1,
     4,     1,     3,     4,     4,     4,     4,     5,     5,     5,
     4,     3,     4,     4,     5,     4,     1,     2,     2,     3,
     3,     2,     4,     6,     3,     2,     1,     1,     1,     2,
     2,     2,     3,     1,     2,     3,     1,     1,     2,     2,
     3,     1,     2,     1,     3,     4,     1,     2,     1,     2,
     3,     6,     1,     1,     2,     3,     2,     1,     1,     2,
     1,     1,     1,     2,     1,     1,     2,     1,     2,     1,
     2,     1,     2,     2,     3,     3,     3,     3,     2,     1,
     4,     4,     1,     2,     1,     3,     1,     1,     1,     1,
     1,     1,     2,     1,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     1,
     0,     9,   210,   208,   209,    27,     0,     0,    10,     0,
     8,     2,    11,    12,     3,    21,     0,     0,     4,    28,
     5,    31,     0,     0,     0,    38,     6,    54,     0,     0,
     0,     7,    69,    43,    42,   175,    26,    62,     0,     0,
   193,     0,   190,    14,     0,    38,     0,   176,   178,   180,
   182,    43,   173,   137,     0,     0,     0,   172,     0,    13,
    79,    87,     0,    73,    75,    77,    80,     0,    23,   202,
   204,     0,    32,    95,   173,     0,   110,   100,     0,     0,
     0,     0,     0,   190,     0,     0,    93,    92,     0,     0,
    88,    90,    96,   101,     0,   108,    91,   112,   127,     0,
     0,     0,     0,   171,     0,    35,     0,    29,     0,    55,
     0,     0,    58,    41,    40,     0,     0,     0,    63,     0,
   181,     0,     0,   189,     0,     0,   201,   200,     0,     0,
     0,     0,     0,    18,   177,   169,   179,   194,   193,   184,
   183,   174,   148,   144,     0,   147,    71,   139,     0,   138,
     0,    72,     0,     0,    74,    76,    78,     0,    24,    25,
   203,    33,     0,     0,     0,   102,   128,    97,    98,    99,
     0,     0,     0,     0,     0,   167,     0,   129,     0,     0,
     0,     0,    94,    34,    89,     0,   105,   132,   136,   163,
   152,     0,     0,   154,   157,   159,     0,     0,     0,   108,
     0,     0,    36,    37,    48,   205,   206,   207,    53,    39,
    44,     0,    50,    46,    52,    30,    56,    57,    59,    60,
    65,    66,     0,     0,    67,   187,   185,   197,   199,   198,
     0,   195,   188,   186,     0,    15,     0,     0,     0,    16,
   170,   145,     0,   149,   142,     0,   141,   140,   150,     0,
    70,    22,    86,    85,    81,    82,   130,     0,     0,   135,
     0,     0,     0,     0,     0,     0,   122,     0,     0,     0,
   110,   109,   168,   113,     0,   164,     0,   153,     0,     0,
   158,   160,     0,     0,     0,     0,   103,     0,     0,     0,
   131,     0,    49,    51,    47,    61,    64,    68,     0,   191,
   192,    17,    20,     0,   146,   143,   151,     0,     0,     0,
   114,   124,   123,   121,   126,     0,     0,     0,   115,   117,
   116,   165,     0,   155,     0,   161,     0,   150,   133,     0,
   106,   104,   107,    45,   196,     0,    83,    84,   118,   120,
   125,   119,   166,   156,     0,     0,   111,     0,     0,   134,
    19,   162,     0,     0
};

static const short yydefgoto[] = {     1,
    11,    12,    13,    87,    44,    15,    88,    17,    18,    19,
    20,    21,    22,    89,    24,    25,    46,   210,   211,   212,
   213,    27,    28,    29,    30,    31,   126,   118,   119,    32,
    33,    55,    63,    64,    65,    66,   255,   256,    67,    90,
    91,    92,    93,    94,    95,    96,   201,    97,    98,    99,
   100,   101,   147,   148,   149,   152,   192,   193,   194,   195,
   275,   276,   102,   277,   137,   103,   104,   140,    58,    48,
    49,    50,    51,   231,   232,   129,    72,   150,   215,    59,
    35
};

static const short yypact[] = {-32768,
   384,-32768,-32768,-32768,-32768,    41,   -28,   476,-32768,   518,
-32768,-32768,-32768,    11,-32768,-32768,    91,     0,-32768,-32768,
-32768,-32768,    30,   353,    66,   101,-32768,-32768,    30,   353,
    84,-32768,-32768,-32768,   170,-32768,-32768,-32768,   464,   236,
    48,    65,    52,-32768,   177,    45,   106,    61,   105,-32768,
   134,   206,   565,-32768,     6,   130,   147,-32768,   206,-32768,
-32768,-32768,    37,-32768,   148,   182,   199,   191,   210,   223,
-32768,   204,-32768,-32768,   129,   488,   212,-32768,   216,   222,
   224,   130,   445,    44,    56,   231,-32768,-32768,   204,   281,
-32768,-32768,-32768,-32768,   -22,-32768,-32768,-32768,-32768,     6,
   130,   176,    81,-32768,   227,   244,   398,   249,   204,-32768,
   321,   235,   252,-32768,-32768,    41,    41,   237,   239,    41,
-32768,   213,    42,-32768,   214,    42,-32768,-32768,   204,    41,
   242,   243,   204,-32768,-32768,-32768,   247,-32768,-32768,-32768,
   134,-32768,   194,-32768,    18,-32768,-32768,-32768,   204,-32768,
   197,-32768,     6,   204,-32768,-32768,-32768,   138,-32768,-32768,
-32768,-32768,   507,   231,   176,-32768,-32768,-32768,-32768,-32768,
     6,   536,   130,   130,    13,-32768,   231,-32768,   176,    86,
   248,   176,-32768,-32768,-32768,   261,-32768,-32768,-32768,-32768,
-32768,   144,   131,-32768,   259,   115,   262,   260,   -22,    94,
   130,     6,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   264,
-32768,   419,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   240,   464,   274,-32768,-32768,-32768,-32768,-32768,
   155,-32768,-32768,-32768,   161,-32768,   204,    41,    41,-32768,
-32768,-32768,   282,-32768,-32768,   204,-32768,-32768,-32768,   140,
-32768,-32768,-32768,-32768,   193,-32768,-32768,   176,     6,-32768,
   204,   204,     6,     6,   130,   130,-32768,   176,     6,     6,
-32768,-32768,-32768,-32768,   144,-32768,   283,-32768,   547,   267,
-32768,   286,   285,   387,     6,   294,-32768,   204,   204,   204,
-32768,   398,-32768,-32768,-32768,-32768,-32768,-32768,    42,-32768,
-32768,-32768,-32768,   280,-32768,-32768,-32768,   138,   138,     6,
-32768,-32768,-32768,-32768,-32768,     6,     6,     6,-32768,-32768,
-32768,-32768,   297,-32768,   271,-32768,   300,   130,-32768,   287,
-32768,-32768,-32768,-32768,-32768,   130,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   288,     6,-32768,   204,   130,-32768,
-32768,-32768,   319,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   330,-32768,-32768,   333,-32768,-32768,-32768,
-32768,-32768,-32768,   141,-32768,   113,   336,-32768,    46,-32768,
   145,-32768,-32768,-32768,-32768,-32768,     9,-32768,   117,-32768,
-32768,   293,-32768,   289,-32768,-32768,-32768,  -130,-32768,   313,
   -64,-32768,-32768,   277,   263,   -89,-32768,-32768,-32768,   -67,
   -91,    -5,   -63,   225,   228,   -80,  -138,  -143,    89,-32768,
-32768,   100,   -32,-32768,-32768,   -75,    -3,    -2,    -7,-32768,
-32768,   329,   102,   256,    87,-32768,   359,   -23,-32768,    12,
-32768
};


#define	YYLAST		593


static const short yytable[] = {    73,
    47,   171,   108,    37,    56,   110,    57,   250,   167,   180,
    38,   202,    34,   200,   186,    39,   143,   178,    68,    52,
   189,   144,    71,   134,   145,   185,   259,   146,   143,    60,
    54,   120,   115,   144,    69,    52,   188,    61,   265,   146,
   269,    52,   165,   270,   133,   142,   185,    70,   162,   123,
    71,    56,   179,   182,    62,   228,   229,    71,    36,   230,
     4,     5,    71,    53,   125,   183,    40,   142,   122,    41,
   154,   187,   125,    54,    71,   176,    42,   174,   124,   175,
   107,    82,    38,    84,   105,   217,    38,   180,   202,   251,
    38,    61,   263,   264,   196,   257,   272,   177,    77,   214,
   106,    52,   112,    54,   135,   236,   197,   260,    62,   240,
   288,   197,   198,   221,   222,   289,   285,   225,   113,   310,
   290,   247,    52,    36,   130,   248,   131,   237,   274,   318,
   252,   258,   282,     4,     5,   136,   107,   132,   291,    40,
   250,    23,    41,   196,   268,    71,   283,   138,    45,    42,
    71,   267,   253,   141,   143,   254,    84,   196,   163,   144,
   141,   151,   145,   278,    54,   146,   156,   279,   262,   266,
   164,   196,   307,   280,   196,   287,   279,   337,   338,   273,
     4,     5,   280,   114,   316,   317,    40,    36,    71,    41,
   127,   299,   300,   190,   128,   311,    42,   299,   301,   314,
   315,     4,     5,    43,   295,   319,   320,    40,   191,   242,
    41,   321,   243,   302,   190,   244,   120,    42,   157,   139,
   158,   329,   306,    36,    43,   159,   226,   233,   160,   249,
   227,   234,   308,   309,    34,   303,   304,   312,   313,   161,
     4,     5,     3,  -137,     4,     5,   339,   346,    71,    41,
   196,   168,   340,   341,   342,   348,    42,   169,   181,   170,
   196,   203,   204,    43,   331,   332,   333,   216,   352,   219,
   220,   196,   238,   239,   223,   224,   196,   241,   271,   176,
   281,    74,   350,     3,   214,     4,     5,     6,    75,    76,
   286,    40,     8,   284,    41,   298,   323,   305,    77,    78,
   292,    42,   326,    79,    80,    81,    82,    83,    84,   325,
    85,   330,   336,   344,   184,   327,   343,   345,   354,   347,
   349,    74,    86,     3,   351,     4,     5,     6,    75,    76,
    14,    40,     8,    16,    41,   296,    26,   334,    77,    78,
   297,    42,   111,    79,    80,    81,    82,    83,    84,   153,
    85,   155,   166,    74,   218,     3,   294,     4,     5,     6,
    75,    76,    86,    40,     8,   199,    41,   324,   121,   245,
    77,    78,   246,    42,   322,    79,    80,    81,    82,    83,
    84,   235,    85,   353,     2,   335,     3,   109,     4,     5,
     6,     4,     5,     7,    86,     8,     9,    40,     0,     0,
    41,     0,     4,     5,   190,     0,     0,    42,    40,     0,
     0,    41,    10,     0,    43,   205,     0,     0,    42,   328,
   206,   207,   208,     4,     5,    43,     0,   209,     0,    40,
     0,     0,    41,     0,     0,     0,   293,     0,     0,    42,
     0,   206,   207,   208,     0,     0,    43,   172,   209,     4,
     5,     0,    53,     0,     0,    40,     0,     0,    41,     0,
     0,     0,    54,     0,     0,    42,   116,   117,     4,     5,
   173,     0,    43,     0,    40,     0,     0,    41,     3,     0,
     4,     5,     0,     0,    42,     0,    40,     0,     0,    41,
     0,    43,     4,     5,     0,    53,    42,     0,    40,     0,
     0,    41,     0,    43,     0,    77,     0,     0,    42,     0,
     0,     4,     5,    82,    53,    43,     0,    40,     0,     0,
    41,     0,     4,     5,    54,    53,     0,    42,    40,     0,
     0,    41,    82,     0,    43,    54,     0,     0,    42,     0,
     4,     5,     0,    53,     0,    43,    40,     0,     0,    41,
     0,     4,     5,   261,     0,     0,    42,    40,     0,     0,
    41,     0,     0,    43,   190,     0,     0,    42,     0,     4,
     5,     0,     0,     0,    43,    40,     0,     0,    41,     0,
     0,     0,     0,     0,     0,    42,     0,     0,     0,     0,
     0,     0,    43
};

static const short yycheck[] = {    23,
     8,    82,    26,     6,    10,    29,    10,   151,    76,    85,
    39,   103,     1,   103,    37,     7,    11,    85,    19,     8,
   101,    16,    45,    47,    19,    90,   165,    22,    11,    19,
    18,    39,    35,    16,    35,    24,   100,     1,    26,    22,
   179,    30,    75,   182,    47,    53,   111,    18,    72,    41,
    45,    57,    85,    86,    18,    14,    15,    45,    18,    18,
     5,     6,    45,     8,    21,    89,    11,    75,    21,    14,
    34,    95,    21,    18,    45,    32,    21,    83,    14,    83,
    36,    26,    39,    28,    19,   109,    39,   163,   180,   153,
    39,     1,   173,   174,   102,   163,   186,    42,    18,   107,
    35,    90,    19,    18,    44,   129,    26,   171,    18,   133,
    17,    26,    32,   116,   117,    22,   197,   120,    35,   258,
   201,   145,   111,    18,    19,   149,    21,   130,   192,   268,
   154,   164,    18,     5,     6,    31,    36,    32,   202,    11,
   284,     1,    14,   151,   177,    45,    32,    14,     8,    21,
    45,   175,    15,    52,    11,    18,    28,   165,    30,    16,
    59,    32,    19,    33,    18,    22,    19,    37,   172,   175,
    42,   179,    33,    43,   182,   199,    37,   308,   309,    36,
     5,     6,    43,    14,   265,   266,    11,    18,    45,    14,
    14,    37,    38,    18,    18,   259,    21,    37,    38,   263,
   264,     5,     6,    28,   212,   269,   270,    11,    33,    16,
    14,   275,    19,   237,    18,    22,   224,    21,    37,    14,
    22,   285,   246,    18,    28,    35,    14,    14,    19,    33,
    18,    18,    40,    41,   223,   238,   239,   261,   262,    17,
     5,     6,     3,    32,     5,     6,   310,   328,    45,    14,
   258,    36,   316,   317,   318,   336,    21,    36,    28,    36,
   268,    35,    19,    28,   288,   289,   290,    19,   349,    35,
    19,   279,    31,    31,    38,    37,   284,    31,    18,    32,
    22,     1,   346,     3,   292,     5,     6,     7,     8,     9,
    31,    11,    12,    32,    14,    22,    14,    16,    18,    19,
    37,    21,    17,    23,    24,    25,    26,    27,    28,    43,
    30,    18,    33,    43,    34,    31,    20,    18,     0,    33,
    33,     1,    42,     3,   348,     5,     6,     7,     8,     9,
     1,    11,    12,     1,    14,   223,     1,   292,    18,    19,
   224,    21,    30,    23,    24,    25,    26,    27,    28,    57,
    30,    63,    76,     1,    34,     3,   212,     5,     6,     7,
     8,     9,    42,    11,    12,   103,    14,   279,    40,   145,
    18,    19,   145,    21,   275,    23,    24,    25,    26,    27,
    28,   126,    30,     0,     1,   299,     3,    29,     5,     6,
     7,     5,     6,    10,    42,    12,    13,    11,    -1,    -1,
    14,    -1,     5,     6,    18,    -1,    -1,    21,    11,    -1,
    -1,    14,    29,    -1,    28,    18,    -1,    -1,    21,    33,
    23,    24,    25,     5,     6,    28,    -1,    30,    -1,    11,
    -1,    -1,    14,    -1,    -1,    -1,    18,    -1,    -1,    21,
    -1,    23,    24,    25,    -1,    -1,    28,     3,    30,     5,
     6,    -1,     8,    -1,    -1,    11,    -1,    -1,    14,    -1,
    -1,    -1,    18,    -1,    -1,    21,     3,     4,     5,     6,
    26,    -1,    28,    -1,    11,    -1,    -1,    14,     3,    -1,
     5,     6,    -1,    -1,    21,    -1,    11,    -1,    -1,    14,
    -1,    28,     5,     6,    -1,     8,    21,    -1,    11,    -1,
    -1,    14,    -1,    28,    -1,    18,    -1,    -1,    21,    -1,
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
	  if(yyvsp[-1].typ->parents.size < 2) {
	    cerr << "E!!: Error in predeclared type: " << yyvsp[-1].typ->name << " second parent not found!"
		 << endl;
	    yyval.typ = yyvsp[-1].typ;
	  }
	  else {
	    TypeDef* td = yyvsp[-1].typ->parents[1]; mta->type_stack.Pop();
	    TypeSpace* sp = yyvsp[-1].typ->owner;
	    sp->RemoveEl(yyvsp[-1].typ); /* get rid of new one, cuz it is bogus */
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
	    yyval.typ = td; } ;
    break;}
case 19:
#line 204 "mta_parse.y"
{
	    yyval.typ = yyvsp[-3].typ; yyval.typ->AddParent(&TA_void_ptr); yyval.typ->ptr = 1;
	    mta->type_stack.Pop(); ;
    break;}
case 20:
#line 207 "mta_parse.y"
{
	    yyval.typ = yyvsp[0].typ; yyval.typ->AddParent(&TA_void_ptr); yyval.typ->ptr = 1;
	    mta->type_stack.Pop(); ;
    break;}
case 21:
#line 212 "mta_parse.y"
{
            TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
            yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("enum", sp, yyval.typ); ;
    break;}
case 24:
#line 222 "mta_parse.y"
{ SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); ;
    break;}
case 25:
#line 223 "mta_parse.y"
{ SETDESC(yyvsp[-2].typ,yyvsp[0].chr); ;
    break;}
case 26:
#line 226 "mta_parse.y"
{
  	    yyval.typ = yyvsp[0].typ;
	    yyvsp[0].typ->AddParFormal(&TA_enum); mta->cur_enum = yyvsp[0].typ;
	    mta->type_stack.Pop(); ;
    break;}
case 27:
#line 230 "mta_parse.y"
{
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->cur_enum = yyval.typ;
	    yyval.typ->AddParFormal(&TA_enum); yyval.typ->internal = true; ;
    break;}
case 28:
#line 237 "mta_parse.y"
{
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("class", sp, yyval.typ);
	    mta->type_stack.Pop(); ;
    break;}
case 29:
#line 245 "mta_parse.y"
{ mta->Burp(); ;
    break;}
case 30:
#line 246 "mta_parse.y"
{ SETDESC(yyvsp[-2].typ,yyvsp[0].chr); ;
    break;}
case 31:
#line 250 "mta_parse.y"
{
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("class", sp, yyval.typ);
	    mta->type_stack.Pop(); ;
    break;}
case 34:
#line 263 "mta_parse.y"
{
	    if(yyvsp[-2].typ->HasOption("NO_TOKENS")) yyvsp[-2].typ->tokens.keep = false;
	    else yyvsp[-2].typ->tokens.keep = true; ;
    break;}
case 35:
#line 269 "mta_parse.y"
{
	    yyvsp[-1].typ->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); ;
    break;}
case 36:
#line 272 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 37:
#line 274 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[0].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 38:
#line 279 "mta_parse.y"
{ mta->state = MTA::Parse_inclass; ;
    break;}
case 39:
#line 280 "mta_parse.y"
{ mta->state = MTA::Parse_inclass; ;
    break;}
case 40:
#line 283 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
            yyval.typ = yyvsp[0].typ; mta->last_class = mta->cur_class; mta->cur_class = yyvsp[0].typ;
	    yyvsp[0].typ->AddParFormal(&TA_class);
            mta->cur_mstate = MTA::prvt; ;
    break;}
case 41:
#line 288 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
	    yyval.typ = yyvsp[0].typ; mta->last_class = mta->cur_class; mta->cur_class = yyvsp[0].typ;
            mta->cur_mstate = MTA::prvt; ;
    break;}
case 42:
#line 292 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
	    String nm = yyvsp[0].typ->name + "_" + (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->type_stack.Push(yyval.typ);
	    mta->last_class = mta->cur_class; mta->cur_class = yyval.typ;
            mta->cur_mstate = MTA::prvt; ;
    break;}
case 43:
#line 298 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
	    String nm = yyvsp[0].typ->name + "_" + (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->type_stack.Push(yyval.typ);
	    mta->last_class = mta->cur_class; mta->cur_class = yyval.typ;
	    mta->cur_mstate = MTA::pblc; ;
    break;}
case 44:
#line 307 "mta_parse.y"
{
            if(yyvsp[0].typ->InheritsFrom(TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent(yyvsp[0].typ); ;
    break;}
case 45:
#line 310 "mta_parse.y"
{
            if(yyvsp[0].typ->InheritsFrom(&TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent(yyvsp[0].typ);
	    if(!mta->cur_class->HasOption("MULT_INHERIT"))
	      mta->cur_class->opts.Add("MULT_INHERIT"); ;
    break;}
case 47:
#line 319 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 48:
#line 320 "mta_parse.y"
{ yyval.typ = &TA_class; ;
    break;}
case 49:
#line 321 "mta_parse.y"
{ yyval.typ = &TA_class; ;
    break;}
case 54:
#line 333 "mta_parse.y"
{
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("template", sp, yyval.typ);
	    mta->type_stack.Pop(); ;
    break;}
case 57:
#line 346 "mta_parse.y"
{
          if(yyvsp[-2].typ->HasOption("NO_TOKENS")) yyvsp[-2].typ->tokens.keep = false;
	  else yyvsp[-2].typ->tokens.keep = true; ;
    break;}
case 58:
#line 352 "mta_parse.y"
{
            mta->state = MTA::Parse_inclass; yyvsp[-1].typ->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); ;
    break;}
case 59:
#line 355 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 60:
#line 357 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[0].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 61:
#line 362 "mta_parse.y"
{
	    yyvsp[0].typ->templ_pars.Reset();
	    yyvsp[0].typ->templ_pars.Duplicate(mta->cur_templ_pars);
	    yyvsp[0].typ->internal = true;
	    yyvsp[0].typ->AddParFormal(&TA_template); yyval.typ = yyvsp[0].typ; ;
    break;}
case 62:
#line 369 "mta_parse.y"
{ mta->cur_templ_pars.Reset(); ;
    break;}
case 64:
#line 374 "mta_parse.y"
{ yyval.typ = yyvsp[-2].typ; ;
    break;}
case 65:
#line 378 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[0].typ; ;
    break;}
case 66:
#line 379 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[0].typ; ;
    break;}
case 67:
#line 380 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[0].typ; ;
    break;}
case 68:
#line 381 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[-1].typ); yyval.typ = yyvsp[-1].typ; ;
    break;}
case 69:
#line 384 "mta_parse.y"
{
            if(mta->spc == &(mta->spc_target)) /* only add reg_funs in target space */
	       TA_taRegFun.methods.AddUniqNameNew(yyvsp[0].meth);
	    mta->meth_stack.Pop(); ;
    break;}
case 70:
#line 390 "mta_parse.y"
{
            yyvsp[-1].meth->type = yyvsp[-2].typ; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); yyval.meth = yyvsp[-1].meth; ;
    break;}
case 71:
#line 392 "mta_parse.y"
{
            yyvsp[-1].meth->type = &TA_int; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); yyval.meth = yyvsp[-1].meth; ;
    break;}
case 72:
#line 396 "mta_parse.y"
{
            yyvsp[-1].meth->is_static = true; /* consider these to be static functions */
            yyvsp[-1].meth->fun_argc = yyvsp[0].rval; yyvsp[-1].meth->arg_types.size = yyvsp[0].rval; mta->burp_fundefn = true; ;
    break;}
case 76:
#line 406 "mta_parse.y"
{ if(yyvsp[-1].enm != NULL) SETENUMDESC(yyvsp[-1].enm,yyvsp[0].chr); ;
    break;}
case 79:
#line 411 "mta_parse.y"
{ /* trying to do some math */
           yyval.enm = NULL;
	   mta->skiptocommarb(); ;
    break;}
case 80:
#line 416 "mta_parse.y"
{
            mta->cur_enum->enum_vals.Add(yyvsp[0].enm);
	    mta->enum_stack.Pop(); ;
    break;}
case 81:
#line 419 "mta_parse.y"
{ /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add(yyvsp[-2].enm); if(yyvsp[0].rval != -424242) yyvsp[-2].enm->enum_no = yyvsp[0].rval;
	    mta->enum_stack.Pop(); ;
    break;}
case 83:
#line 426 "mta_parse.y"
{
	  if((yyvsp[-2].rval != -424242) && (yyvsp[0].rval != -424242))  yyval.rval = yyvsp[-2].rval + yyvsp[0].rval;
	  else if(yyvsp[-2].rval != -424242)	yyval.rval = yyvsp[-2].rval;
	  else if(yyvsp[0].rval != -424242)	yyval.rval = yyvsp[0].rval;
	  else				yyval.rval = -424242; ;
    break;}
case 84:
#line 431 "mta_parse.y"
{
	  if((yyvsp[-2].rval != -424242) && (yyvsp[0].rval != -424242)) yyval.rval = yyvsp[-2].rval - yyvsp[0].rval;
	  else if(yyvsp[-2].rval != -424242)	yyval.rval = yyvsp[-2].rval;
	  else if(yyvsp[0].rval != -424242)	yyval.rval = yyvsp[0].rval;
	  else				yyval.rval = -424242; ;
    break;}
case 85:
#line 439 "mta_parse.y"
{ yyval.rval = -424242; ;
    break;}
case 87:
#line 444 "mta_parse.y"
{ yyval.enm = new EnumDef(yyvsp[0].chr); mta->enum_stack.Push(yyval.enm); ;
    break;}
case 88:
#line 447 "mta_parse.y"
{ mta->Class_UpdateLastPtrs(); ;
    break;}
case 89:
#line 448 "mta_parse.y"
{ mta->Class_UpdateLastPtrs(); ;
    break;}
case 90:
#line 451 "mta_parse.y"
{
            if(yyvsp[0].memb != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->HasOption("IGNORE"))
		 && !(yyvsp[0].memb->type->DerivesFrom(TA_const))) {
		mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
		if(mta->verbose >= 3)
		  cerr << "M!!: member: " << yyvsp[0].memb->name << " added to: "
		       << mta->cur_class->name << "\n"; } }
	    mta->memb_stack.Pop(); yyval.typ = NULL; ;
    break;}
case 91:
#line 460 "mta_parse.y"
{
            if(yyvsp[0].meth != NULL) {
	      if(mta->cur_mstate == MTA::pblc) {
		if(yyvsp[0].meth->HasOption("IGNORE"))
		  mta->cur_class->ignore_meths.AddUnique(yyvsp[0].meth->name);
		else {
		  mta->cur_class->methods.AddUniqNameNew(yyvsp[0].meth);
		  if(mta->verbose >= 3)
		    cerr << "M!!: method: " << yyvsp[0].meth->name << " added to: "
			 << mta->cur_class->name << "\n"; } } }
	    else {
	      mta->cur_meth = NULL; }
	    mta->meth_stack.Pop(); yyval.typ = NULL; ;
    break;}
case 92:
#line 473 "mta_parse.y"
{
	    mta->cur_class->sub_types.AddUniqNameNew(yyvsp[0].typ);
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; ;
    break;}
case 93:
#line 476 "mta_parse.y"
{
	    mta->cur_class->sub_types.AddUniqNameNew(yyvsp[0].typ);
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; ;
    break;}
case 94:
#line 479 "mta_parse.y"
{ /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew(yyvsp[-1].typ);
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; ;
    break;}
case 95:
#line 483 "mta_parse.y"
{ yyval.typ = NULL; ;
    break;}
case 97:
#line 488 "mta_parse.y"
{ yyval.memb = NULL; mta->cur_mstate = MTA::pblc; ;
    break;}
case 98:
#line 489 "mta_parse.y"
{ yyval.memb = NULL; mta->cur_mstate = MTA::prvt; ;
    break;}
case 99:
#line 490 "mta_parse.y"
{ yyval.memb = NULL; mta->cur_mstate = MTA::prot; ;
    break;}
case 100:
#line 491 "mta_parse.y"
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
		  cerr << "M!!: method: " << mta->last_meth->name << " added to: "
		       << mta->cur_class->name << "\n"; } } ;
    break;}
case 102:
#line 512 "mta_parse.y"
{
	  yyval.memb = yyvsp[0].memb; if(yyvsp[0].memb != NULL) yyvsp[0].memb->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; ;
    break;}
case 103:
#line 518 "mta_parse.y"
{ yyval.memb = yyvsp[-1].memb; ;
    break;}
case 104:
#line 519 "mta_parse.y"
{ yyval.memb = yyvsp[-2].memb; ;
    break;}
case 105:
#line 520 "mta_parse.y"
{ ;
    break;}
case 106:
#line 521 "mta_parse.y"
{
	    yyval.memb = yyvsp[-2].memb;
	    String nm = yyvsp[-3].typ->name + "_ary";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[-3].typ->ptr + 1);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-3].typ);
	    TypeDef* uty = sp->AddUniqNameOld(nty); 
	    if(uty == nty) { mta->TypeAdded("array", sp, uty); 
	      nty->AddParFormal(&TA_ta_array); nty->AddParent(yyvsp[-3].typ); }
	    else { mta->TypeNotAdded("array", sp, uty, nty); delete nty; }
	    yyvsp[-2].memb->type = uty; ;
    break;}
case 107:
#line 531 "mta_parse.y"
{ yyvsp[-2].memb->type = yyvsp[-3].typ; yyval.memb = yyvsp[-2].memb; ;
    break;}
case 108:
#line 535 "mta_parse.y"
{
	     if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
             mta->memb_stack.Pop(); yyval.memb = NULL; ;
    break;}
case 109:
#line 539 "mta_parse.y"
{
	     if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
             mta->memb_stack.Pop(); yyval.memb = NULL; ;
    break;}
case 110:
#line 545 "mta_parse.y"
{
            yyval.memb = new MemberDef(yyvsp[0].chr); mta->cur_memb = yyval.memb; mta->memb_stack.Push(yyval.memb);
            if(mta->cur_memb_type != NULL) yyval.memb->type = mta->cur_memb_type;
            else yyval.memb->type = &TA_int; ;
    break;}
case 111:
#line 551 "mta_parse.y"
{
	    yyval.memb = new MemberDef(yyvsp[-1].chr); mta->cur_memb = yyval.memb; mta->memb_stack.Push(yyval.memb);
	    yyval.memb->fun_ptr = 1; ;
    break;}
case 113:
#line 557 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 114:
#line 558 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 115:
#line 559 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 116:
#line 560 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 117:
#line 561 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 118:
#line 562 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 119:
#line 563 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 120:
#line 564 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 121:
#line 565 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 122:
#line 566 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 123:
#line 567 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 124:
#line 568 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 125:
#line 569 "mta_parse.y"
{
	    yyval.meth = NULL; String tmp = yyvsp[0].chr;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew(yyvsp[-2].meth); yyvsp[-2].meth->type = yyvsp[-3].typ;
	      mta->meth_stack.Pop();  yyvsp[-2].meth->fun_argc = yyvsp[-1].rval; yyvsp[-2].meth->arg_types.size = yyvsp[-1].rval;
	      yyvsp[-2].meth->is_static = true; /* consider these to be static functions */
	      SETDESC(yyvsp[-2].meth,yyvsp[0].chr); }
	    else { yyval.meth = NULL; mta->meth_stack.Pop(); } ;
    break;}
case 126:
#line 577 "mta_parse.y"
{
	    yyval.meth = NULL; String tmp = yyvsp[0].chr;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew(yyvsp[-2].meth); yyvsp[-2].meth->type = &TA_int;
	      mta->meth_stack.Pop();  yyvsp[-2].meth->fun_argc = yyvsp[-1].rval; yyvsp[-2].meth->arg_types.size = yyvsp[-1].rval;
	      yyvsp[-2].meth->is_static = true; /* consider these to be static functions */
	      SETDESC(yyvsp[-2].meth,yyvsp[0].chr); }
	    else { yyval.meth = 0; mta->meth_stack.Pop(); } ;
    break;}
case 128:
#line 589 "mta_parse.y"
{ yyval.meth = yyvsp[0].meth; yyvsp[0].meth->is_static = true; ;
    break;}
case 129:
#line 590 "mta_parse.y"
{ yyval.meth = yyvsp[0].meth;  if(yyvsp[0].meth != NULL) yyvsp[0].meth->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; ;
    break;}
case 130:
#line 592 "mta_parse.y"
{ yyval.meth = yyvsp[0].meth;  if(yyvsp[0].meth != NULL) yyvsp[0].meth->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; ;
    break;}
case 131:
#line 597 "mta_parse.y"
{ yyval.meth = yyvsp[-1].meth; yyvsp[-1].meth->type = yyvsp[-2].typ; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); ;
    break;}
case 132:
#line 598 "mta_parse.y"
{ yyvsp[-1].meth->type = &TA_int; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); ;
    break;}
case 133:
#line 599 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 134:
#line 600 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 135:
#line 601 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 136:
#line 604 "mta_parse.y"
{
             yyvsp[-1].meth->fun_argc = yyvsp[0].rval; yyvsp[-1].meth->arg_types.size = yyvsp[0].rval; mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if(yyvsp[-1].meth->fun_argd > yyvsp[-1].meth->fun_argc) yyvsp[-1].meth->fun_argd = -1; ;
    break;}
case 137:
#line 611 "mta_parse.y"
{
            yyval.meth = new MethodDef(yyvsp[0].chr); mta->cur_meth = yyval.meth; mta->meth_stack.Push(yyval.meth); ;
    break;}
case 138:
#line 615 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 139:
#line 616 "mta_parse.y"
{ yyval.chr = yyvsp[0].chr; ;
    break;}
case 140:
#line 617 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 141:
#line 618 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 142:
#line 619 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 143:
#line 620 "mta_parse.y"
{ yyval.chr = yyvsp[-2].chr; ;
    break;}
case 144:
#line 624 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 145:
#line 625 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 146:
#line 626 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 147:
#line 630 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 148:
#line 631 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 149:
#line 632 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 150:
#line 635 "mta_parse.y"
{ yyval.rval = 0; ;
    break;}
case 151:
#line 636 "mta_parse.y"
{ yyval.rval = yyvsp[-1].rval; ;
    break;}
case 152:
#line 639 "mta_parse.y"
{ yyval.rval = 0; ;
    break;}
case 153:
#line 640 "mta_parse.y"
{ yyval.rval = yyvsp[-1].rval; ;
    break;}
case 154:
#line 643 "mta_parse.y"
{ yyval.rval = 1; ;
    break;}
case 155:
#line 644 "mta_parse.y"
{ yyval.rval = yyvsp[-2].rval + 1; ;
    break;}
case 156:
#line 645 "mta_parse.y"
{ yyval.rval = yyvsp[-3].rval; ;
    break;}
case 157:
#line 648 "mta_parse.y"
{
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } ;
    break;}
case 158:
#line 650 "mta_parse.y"
{
	    if(mta->cur_meth != NULL) {
	      String ad = yyvsp[0].chr;
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } ;
    break;}
case 159:
#line 661 "mta_parse.y"
{
	    yyval.rval = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[0].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 160:
#line 665 "mta_parse.y"
{
	    yyval.rval = 1; String nm = yyvsp[0].chr;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-1].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 161:
#line 669 "mta_parse.y"
{
	    yyval.rval = 1; String nm = String(yyvsp[-1].chr) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-2].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 162:
#line 673 "mta_parse.y"
{
	    yyval.rval = 1; String nm = String("(*") + String(yyvsp[-2].chr) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-5].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 163:
#line 677 "mta_parse.y"
{
	    yyval.rval = 1; String nm = yyvsp[0].chr;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 167:
#line 692 "mta_parse.y"
{
            mta->thisname = true; mta->constcoln = false; ;
    break;}
case 168:
#line 696 "mta_parse.y"
{ mta->constcoln = true; ;
    break;}
case 169:
#line 699 "mta_parse.y"
{ yyval.rval = 1; ;
    break;}
case 170:
#line 700 "mta_parse.y"
{ yyval.rval = yyvsp[-1].rval + 1; ;
    break;}
case 171:
#line 703 "mta_parse.y"
{ mta->cur_memb_type = yyvsp[0].typ; ;
    break;}
case 173:
#line 707 "mta_parse.y"
{ yyval.typ = &TA_int; ;
    break;}
case 174:
#line 708 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 175:
#line 711 "mta_parse.y"
{ yyval.typ = new TypeDef(yyvsp[0].chr); mta->type_stack.Push(yyval.typ); ;
    break;}
case 177:
#line 715 "mta_parse.y"
{
	    String nm = yyvsp[-1].typ->name + "_ref";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[-1].typ->ptr, true);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-1].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) { mta->TypeAdded("ref", sp, yyval.typ); nty->AddParent(yyvsp[-1].typ); }
	    else { mta->TypeNotAdded("ref", sp, yyval.typ, nty); delete nty; }
	  ;
    break;}
case 179:
#line 727 "mta_parse.y"
{
 	    int i; String nm = yyvsp[-1].typ->name; for(i=0; i<yyvsp[0].rval; i++) nm += "_ptr";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[0].rval); 
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-1].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) { mta->TypeAdded("ptr", sp, yyval.typ); nty->AddParent(yyvsp[-1].typ); }
	    else { mta->TypeNotAdded("ptr", sp, yyval.typ, nty); delete nty; }
	  ;
    break;}
case 181:
#line 738 "mta_parse.y"
{
	    String nm = yyvsp[-1].typ->name + "_" + yyvsp[0].typ->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) { mta->TypeAdded("const", sp, yyval.typ);
	      nty->size = yyvsp[0].typ->size; nty->AddParent(yyvsp[-1].typ); nty->AddParent(yyvsp[0].typ); }
	    else { mta->TypeNotAdded("const", sp, yyval.typ, nty); delete nty; }
	  ;
    break;}
case 183:
#line 750 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 184:
#line 751 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 185:
#line 752 "mta_parse.y"
{
	    TypeDef* td; if((td = yyvsp[-2].typ->sub_types.FindName(yyvsp[0].chr)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    yyval.typ = td; ;
    break;}
case 186:
#line 756 "mta_parse.y"
{
	    TypeDef* td; if((td = yyvsp[-2].typ->sub_types.FindName(yyvsp[0].chr)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    yyval.typ = td; ;
    break;}
case 187:
#line 760 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 188:
#line 761 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 189:
#line 762 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 191:
#line 764 "mta_parse.y"
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
case 192:
#line 781 "mta_parse.y"
{ /* this template */
	    if(!(yyvsp[-3].typ->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    yyval.typ = yyvsp[-3].typ; ;
    break;}
case 194:
#line 788 "mta_parse.y"
{
	    String nm = yyvsp[-1].typ->name + "_" + yyvsp[0].typ->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) { mta->TypeAdded("combo", sp, yyval.typ);
	      nty->size = yyvsp[0].typ->size; nty->AddParent(yyvsp[-1].typ); nty->AddParent(yyvsp[0].typ); }
	    else { mta->TypeNotAdded("combo", sp, yyval.typ, nty); delete nty; }
	  ;
    break;}
case 196:
#line 801 "mta_parse.y"
{ yyval.typ = yyvsp[-2].typ; ;
    break;}
case 197:
#line 806 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); ;
    break;}
case 198:
#line 807 "mta_parse.y"
{ yyval.typ = new TypeDef(yyvsp[0].chr); mta->cur_templ_pars.Push(yyval.typ); ;
    break;}
case 199:
#line 808 "mta_parse.y"
{ yyval.typ = new TypeDef((String)yyvsp[0].rval); mta->cur_templ_pars.Push(yyval.typ); ;
    break;}
case 201:
#line 813 "mta_parse.y"
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
#line 837 "mta_parse.y"


	/* end of grammar */

void yyerror(char *s) { 	/* called for yacc syntax error */
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
