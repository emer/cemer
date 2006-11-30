
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



#define	YYFINAL		339
#define	YYFLAG		-32768
#define	YYNTBASE	45

#define YYTRANSLATE(x) ((unsigned)(x) <= 283 ? yytranslate[x] : 127)

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
   339,   344,   350,   356,   361,   365,   370,   375,   381,   386,
   388,   391,   394,   398,   401,   406,   413,   417,   420,   422,
   424,   426,   429,   432,   435,   439,   441,   444,   448,   450,
   452,   455,   458,   462,   464,   467,   469,   473,   478,   480,
   483,   485,   488,   492,   499,   501,   503,   506,   510,   513,
   515,   517,   520,   522,   524,   526,   529,   531,   533,   536,
   538,   541,   543,   546,   548,   551,   554,   558,   562,   566,
   570,   573,   575,   580,   585,   587,   590,   592,   596,   598,
   600,   602,   605,   607,   609,   611,   614,   616,   618,   620,
   622,   624,   626
};

static const short yyrhs[] = {    -1,
    45,    47,     0,    45,    51,     0,    45,    55,     0,    45,
    57,     0,    45,    67,     0,    45,    74,     0,    45,    46,
     0,    45,     1,     0,    12,     0,    48,     0,    49,     0,
    49,    18,     0,    11,    50,     0,    11,    59,   119,   123,
     0,   113,   112,   123,     0,   113,    18,   112,   123,     0,
   113,   123,     0,   113,    31,    30,   112,    32,   100,   123,
     0,   113,    20,    30,   112,     0,    52,     0,    53,    77,
    33,   123,     0,    54,    34,     0,    54,    18,    34,     0,
    54,    34,    18,     0,     6,   112,     0,     6,     0,    56,
     0,    62,   123,     0,    62,   123,    18,     0,    58,     0,
    59,   123,     0,    59,   122,   123,     0,    60,    84,    33,
     0,    61,    34,     0,    61,    18,    34,     0,    61,    34,
    18,     0,    62,     0,    62,    35,    63,     0,   126,   112,
     0,   126,    13,     0,   126,     0,   125,     0,    64,     0,
    63,    36,    64,     0,   113,     0,    65,   113,     0,    17,
     0,    65,    17,     0,    66,     0,    65,    66,     0,   124,
     0,    29,     0,    68,     0,    69,   123,     0,    69,   122,
   123,     0,    70,    84,    33,     0,    71,    34,     0,    71,
    18,    34,     0,    71,    34,    18,     0,     9,    72,    73,
    37,    61,     0,    38,     0,     3,   112,     0,    73,    36,
     3,   112,     0,    75,     0,    28,   111,    76,    97,     0,
    28,    76,    97,     0,    96,   100,     0,    78,     0,    77,
    78,     0,    79,     0,    79,    18,     0,    80,     0,    80,
    36,     0,     1,     0,    83,     0,    83,    21,    81,     0,
    82,     0,    81,    39,    82,     0,    81,    40,    82,     0,
    17,     0,    14,     0,    17,     0,    85,     0,    84,    85,
     0,    86,     0,    92,     0,    52,     0,    49,     0,    59,
   123,     0,     1,     0,    87,     0,    22,    35,     0,    23,
    35,     0,    24,    35,     0,    18,     0,    88,     0,     8,
    88,     0,   110,    89,   123,     0,    89,   123,     0,   110,
    90,    16,   123,     0,   110,    91,   100,   123,     0,    90,
     0,    89,    36,    90,     0,    17,     0,    31,    30,    17,
    32,     0,    93,     0,   107,   101,    97,     0,   120,   107,
   101,    97,     0,   107,   101,   105,    97,     0,    41,   107,
   101,    97,     0,   120,    41,   107,   101,    97,     0,    26,
   111,    25,   100,    97,     0,    26,    25,   100,    97,     0,
    26,   111,   123,     0,    26,     3,   111,   123,     0,    26,
     3,    17,   123,     0,    26,   111,    96,   100,    97,     0,
    26,    96,   100,    97,     0,    94,     0,     8,    94,     0,
    29,    94,     0,   110,    95,    97,     0,    95,    97,     0,
   110,    25,   100,    97,     0,   110,    25,    31,    32,   100,
    97,     0,    25,   100,    97,     0,    96,   100,     0,    17,
     0,   123,     0,    98,     0,    99,   123,     0,    18,   123,
     0,    18,    98,     0,    18,    99,   123,     0,    15,     0,
    10,    15,     0,    10,    18,    15,     0,    21,     0,    10,
     0,    10,    21,     0,    31,    32,     0,    31,   102,    32,
     0,    32,     0,   102,    32,     0,   103,     0,   102,    36,
   103,     0,   102,    42,    42,    42,     0,   104,     0,   104,
    21,     0,   113,     0,   113,    17,     0,   113,    17,    16,
     0,   113,    31,    30,    17,    32,   100,     0,    17,     0,
   106,     0,   105,   106,     0,   108,    13,    19,     0,    27,
    31,     0,    35,     0,    30,     0,   109,    30,     0,   111,
     0,   113,     0,   120,     0,   120,   113,     0,    17,     0,
   114,     0,   114,    43,     0,   115,     0,   115,   109,     0,
   116,     0,    10,   116,     0,   117,     0,   125,   117,     0,
   125,   112,     0,    13,    20,    17,     0,    27,    20,    17,
     0,    13,    20,    13,     0,    27,    20,    13,     0,    20,
    13,     0,    27,     0,    13,    72,   118,    37,     0,    27,
    72,   118,    37,     0,    13,     0,   117,    13,     0,    13,
     0,   118,    36,    13,     0,    17,     0,    13,     0,   121,
     0,   120,   121,     0,     7,     0,    29,     0,    17,     0,
    17,    16,     0,    44,     0,    22,     0,    23,     0,    24,
     0,     4,     0,     5,     0,     3,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   113,   114,   117,   119,   122,   125,   127,   129,   131,   135,
   139,   146,   148,   152,   153,   157,   162,   169,   199,   202,
   207,   213,   216,   217,   218,   221,   225,   231,   239,   241,
   244,   252,   254,   257,   263,   267,   269,   273,   275,   278,
   283,   287,   293,   302,   305,   313,   314,   315,   316,   319,
   320,   323,   324,   327,   335,   337,   340,   346,   350,   352,
   356,   364,   367,   369,   372,   378,   380,   384,   389,   390,
   393,   394,   397,   398,   399,   404,   407,   412,   414,   419,
   426,   428,   431,   435,   436,   439,   448,   461,   464,   467,
   471,   474,   476,   477,   478,   479,   498,   500,   505,   507,
   508,   516,   519,   524,   530,   536,   541,   542,   543,   544,
   545,   546,   547,   548,   549,   550,   551,   552,   560,   570,
   572,   573,   577,   579,   580,   581,   582,   585,   592,   596,
   597,   598,   599,   600,   601,   604,   606,   607,   610,   612,
   613,   616,   617,   620,   621,   624,   625,   626,   629,   631,
   638,   643,   647,   651,   655,   661,   663,   666,   669,   674,
   677,   678,   681,   684,   685,   686,   689,   692,   693,   702,
   704,   712,   713,   722,   723,   724,   725,   729,   733,   734,
   735,   736,   737,   754,   760,   761,   770,   772,   775,   776,
   779,   780,   783,   784,   787,   788,   791,   794,   795,   796,
   799,   801,   804
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
"templtypes","tdname","funtspec","funtsmod","varname","term","access","structstruct",
"classkeyword", NULL
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
    92,    92,    92,    92,    92,    92,    92,    92,    92,    93,
    93,    93,    94,    94,    94,    94,    94,    95,    96,    97,
    97,    97,    97,    97,    97,    98,    98,    98,    99,    99,
    99,   100,   100,   101,   101,   102,   102,   102,   103,   103,
   104,   104,   104,   104,   104,   105,   105,   106,   107,   108,
   109,   109,   110,   111,   111,   111,   112,   113,   113,   114,
   114,   115,   115,   116,   116,   116,   116,   116,   116,   116,
   116,   116,   116,   116,   117,   117,   118,   118,   119,   119,
   120,   120,   121,   121,   122,   122,   123,   124,   124,   124,
   125,   125,   126
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
     4,     5,     5,     4,     3,     4,     4,     5,     4,     1,
     2,     2,     3,     2,     4,     6,     3,     2,     1,     1,
     1,     2,     2,     2,     3,     1,     2,     3,     1,     1,
     2,     2,     3,     1,     2,     1,     3,     4,     1,     2,
     1,     2,     3,     6,     1,     1,     2,     3,     2,     1,
     1,     2,     1,     1,     1,     2,     1,     1,     2,     1,
     2,     1,     2,     1,     2,     2,     3,     3,     3,     3,
     2,     1,     4,     4,     1,     2,     1,     3,     1,     1,
     1,     2,     1,     1,     1,     2,     1,     1,     1,     1,
     1,     1,     1
};

static const short yydefact[] = {     1,
     0,     9,   203,   201,   202,    27,     0,     0,    10,     0,
     8,     2,    11,    12,     3,    21,     0,     0,     4,    28,
     5,    31,     0,     0,     0,    38,     6,    54,     0,     0,
     0,     7,    65,    43,    42,   167,    26,    62,     0,     0,
   185,     0,   182,    14,     0,    38,     0,   168,   170,   172,
   174,    43,   193,   129,   194,     0,     0,     0,   164,   165,
   191,     0,    13,    75,    83,     0,    69,    71,    73,    76,
     0,    23,   195,   197,     0,    32,    91,     0,   105,    96,
     0,     0,     0,     0,     0,   182,   194,     0,    89,    88,
     0,     0,    84,    86,    92,    97,     0,   103,    87,   107,
   120,     0,     0,     0,     0,   163,   165,     0,    35,     0,
    29,     0,    55,     0,     0,    58,    41,    40,     0,     0,
   173,     0,     0,   181,     0,     0,   190,   189,     0,     0,
     0,     0,     0,    18,   169,   161,   171,   186,   185,   176,
   175,   140,   136,     0,   139,    67,   131,     0,   130,     0,
    68,     0,   166,   192,     0,    70,    72,    74,     0,    24,
    25,   196,    33,    98,   121,    93,    94,    95,     0,     0,
     0,     0,     0,   159,   122,     0,     0,     0,    90,    34,
    85,     0,   100,   124,   128,   155,   144,     0,     0,   146,
   149,   151,     0,     0,     0,   103,     0,     0,     0,     0,
    36,    37,    48,   198,   199,   200,    53,    39,    44,     0,
    50,    46,    52,    30,    56,    57,    59,    60,    63,     0,
     0,   179,   177,   187,     0,   180,   178,     0,    15,     0,
     0,     0,    16,   162,   137,     0,   141,   134,     0,   133,
   132,   142,     0,    66,    22,    82,    81,    77,    78,   127,
     0,     0,     0,     0,     0,     0,   115,     0,   105,   104,
   160,   108,     0,   156,     0,   145,     0,     0,   150,   152,
     0,     0,     0,     0,    99,     0,     0,   123,     0,     0,
     0,    49,    51,    47,     0,    61,     0,   183,   184,    17,
    20,     0,   138,   135,   143,     0,     0,   117,   116,   114,
   119,     0,     0,   111,   110,   157,     0,   147,     0,   153,
     0,   142,   125,     0,   101,   102,     0,   109,    45,    64,
   188,     0,    79,    80,   113,   118,   158,   148,     0,     0,
   106,   112,     0,     0,   126,    19,   154,     0,     0
};

static const short yydefgoto[] = {     1,
    11,    12,    13,    89,    44,    15,    90,    17,    18,    19,
    20,    21,    22,    91,    24,    25,    46,   208,   209,   210,
   211,    27,    28,    29,    30,    31,   126,   120,    32,    33,
    56,    66,    67,    68,    69,   248,   249,    70,    92,    93,
    94,    95,    96,    97,    98,   197,    99,   100,   101,   102,
   103,   146,   147,   148,   151,   188,   189,   190,   191,   263,
   264,   104,   265,   137,   105,   106,   140,    59,    48,    49,
    50,    51,   225,   129,    60,    61,    75,   149,   213,    62,
    35
};

static const short yypact[] = {-32768,
   130,-32768,-32768,-32768,-32768,    37,   -20,   507,-32768,   467,
-32768,-32768,-32768,    66,-32768,-32768,    90,    83,-32768,-32768,
-32768,-32768,     6,   350,   106,    12,-32768,-32768,     6,   350,
   110,-32768,-32768,-32768,   144,-32768,-32768,-32768,   134,   239,
    39,   138,    43,-32768,   219,   125,   136,   132,   154,-32768,
   185,   224,-32768,-32768,-32768,    55,   174,   191,-32768,   496,
-32768,   224,-32768,-32768,-32768,    38,-32768,   192,   188,   197,
   187,   208,   222,-32768,   204,-32768,-32768,   430,   232,-32768,
   215,   220,   246,   174,   377,   107,   441,   237,-32768,-32768,
   204,   272,-32768,-32768,-32768,-32768,   -14,-32768,-32768,-32768,
-32768,    55,   174,   142,   160,-32768,    31,   252,   269,   388,
   270,   204,-32768,   311,   257,   275,-32768,-32768,    37,    86,
-32768,   234,   287,-32768,   245,   287,-32768,-32768,   204,    37,
   273,   274,   204,-32768,-32768,-32768,   276,-32768,-32768,-32768,
   185,   202,-32768,    65,-32768,-32768,-32768,   204,-32768,   182,
-32768,    55,-32768,-32768,   204,-32768,-32768,-32768,    97,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    55,   478,
   174,   174,     4,-32768,-32768,    88,   271,   142,-32768,-32768,
-32768,   290,-32768,-32768,-32768,-32768,-32768,    22,   164,-32768,
   288,    11,   277,   281,   -14,   304,   174,    55,   237,   142,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   289,-32768,   409,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   320,
   264,-32768,-32768,-32768,   112,-32768,-32768,   129,-32768,   204,
    37,    37,-32768,-32768,-32768,   312,-32768,-32768,   204,-32768,
-32768,-32768,   183,-32768,-32768,-32768,-32768,   231,-32768,-32768,
   204,   204,    55,    55,   174,   174,-32768,    55,-32768,-32768,
-32768,-32768,    22,-32768,   313,-32768,   184,   297,-32768,   314,
   302,   229,    55,   324,-32768,   204,   204,-32768,   142,    55,
   388,-32768,-32768,-32768,    37,-32768,   329,-32768,-32768,-32768,
-32768,   315,-32768,-32768,-32768,    97,    97,-32768,-32768,-32768,
-32768,    55,    55,-32768,-32768,-32768,   326,-32768,   301,-32768,
   331,   174,-32768,   317,-32768,-32768,    55,-32768,-32768,-32768,
-32768,   174,-32768,-32768,-32768,-32768,-32768,-32768,   318,    55,
-32768,-32768,   204,   174,-32768,-32768,-32768,   346,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   358,-32768,-32768,   361,-32768,-32768,-32768,
-32768,-32768,-32768,    82,-32768,   143,   364,-32768,    85,-32768,
   159,-32768,-32768,-32768,-32768,-32768,     8,-32768,-32768,-32768,
   325,-32768,   305,-32768,-32768,-32768,  -115,-32768,   348,   -47,
-32768,-32768,   307,   283,   -86,-32768,-32768,-32768,    -9,   -89,
     3,   -90,   242,   251,   -77,  -167,  -146,   122,-32768,-32768,
   133,   -80,-32768,-32768,   316,     0,    -1,    -7,-32768,-32768,
   359,    40,   292,-32768,   -10,   -43,   371,   -23,-32768,     1,
-32768
};


#define	YYLAST		534


static const short yytable[] = {    76,
    47,    34,   111,   243,    37,   113,   169,   178,    52,    58,
   258,   184,    57,   107,    39,   198,   154,    38,   196,   107,
    54,   182,    73,   134,    52,   185,   200,   270,   255,    74,
    52,   142,   280,   118,     4,     5,   143,    53,    64,   144,
    40,   271,   145,    41,   181,   133,   110,    74,   123,    74,
    42,   163,   153,    36,    65,    74,   261,    86,   122,    55,
    57,   244,   125,   154,   142,    74,   181,   179,   165,   143,
   155,   199,   144,   183,   142,   145,    38,   175,   250,   143,
    38,   107,    23,    63,   173,   145,   198,   172,   215,    45,
    64,   141,    52,   253,   254,   260,   192,   262,    74,   153,
    71,   141,   212,   107,    54,   229,    65,   278,    74,   233,
   246,   317,   193,   247,    52,   273,    72,   219,   279,   277,
   240,   220,   221,   108,   241,   243,   125,   115,   230,   338,
     2,   245,     3,     4,     5,     6,   119,   174,     7,   109,
     8,     9,   192,   116,    38,     4,     5,   287,   288,   257,
   124,    40,    36,   130,    41,   131,   117,    10,   186,   110,
    36,    42,   300,   301,   287,   289,   132,   304,    43,   252,
   192,   275,   305,   187,   135,   256,    79,   302,   303,    74,
   323,   324,   313,   136,   193,     4,     5,     4,     5,   318,
   194,    40,   192,    40,    41,   266,    41,   138,   186,   267,
   186,    42,   284,    42,   150,   268,   290,    54,    43,   157,
    43,   325,   326,   242,   295,   294,   235,   159,   267,   236,
   160,    34,   237,   158,   268,   161,   332,   298,   299,   291,
   292,   127,     4,     5,   330,   128,   139,   162,    40,   335,
    36,    41,     4,     5,   333,   186,   222,    74,    42,   166,
   223,    41,   315,   316,   167,    43,   337,   226,    42,   192,
   312,   227,  -129,   177,   192,    43,     3,     4,     5,   296,
   297,   192,    77,   212,     3,     4,     5,     6,    53,    78,
   168,    40,     8,   320,    41,   201,   202,   214,    79,    80,
   217,    42,   218,    81,    82,    83,    84,    85,    86,   224,
    87,   174,   231,   232,   180,   234,   259,   272,   269,   336,
   274,    77,    88,     3,     4,     5,     6,    53,    78,   276,
    40,     8,   285,    41,   281,   307,   293,    79,    80,   310,
    42,   311,    81,    82,    83,    84,    85,    86,   309,    87,
   314,   321,   328,   216,   327,   339,   322,   329,   331,   334,
    77,    88,     3,     4,     5,     6,    53,    78,    14,    40,
     8,    16,    41,   286,    26,   319,    79,    80,   283,    42,
   156,    81,    82,    83,    84,    85,    86,   114,    87,   170,
     4,     5,   152,    53,   164,   238,    40,   195,   308,    41,
    88,     4,     5,    54,   239,   306,    42,    40,   121,   112,
    41,   171,   176,    43,   203,    55,     0,    42,     0,   204,
   205,   206,     4,     5,    43,     0,   207,   228,    40,     0,
     0,    41,     0,     0,     0,   282,     0,     0,    42,     0,
   204,   205,   206,     4,     5,    43,    53,   207,     0,    40,
     0,     0,    41,     0,     4,     5,    79,    53,     0,    42,
    40,     0,     0,    41,    84,     0,    43,    54,    55,     0,
    42,     0,     0,     0,     0,    84,     0,    43,     0,    55,
     4,     5,     0,    53,     0,     0,    40,     0,     0,    41,
     0,     4,     5,    54,    53,     0,    42,    40,     0,     0,
    41,     0,     0,    43,   251,    55,     0,    42,     0,     4,
     5,     0,    53,     0,    43,    40,    55,     0,    41,     3,
     4,     5,     0,     0,     0,    42,    40,     0,     0,    41,
     0,     0,    43,     0,    55,     0,    42,     0,     0,     0,
     0,     0,     0,    43
};

static const short yycheck[] = {    23,
     8,     1,    26,   150,     6,    29,    84,    88,     8,    10,
   178,   102,    10,    24,     7,   105,    60,    38,   105,    30,
    17,    36,    17,    47,    24,   103,   107,    17,    25,    44,
    30,    10,   200,    35,     4,     5,    15,     7,     1,    18,
    10,    31,    21,    13,    92,    47,    35,    44,    41,    44,
    20,    75,    60,    17,    17,    44,    35,    27,    20,    29,
    58,   152,    20,   107,    10,    44,   114,    91,    78,    15,
    33,    41,    18,    97,    10,    21,    38,    87,   169,    15,
    38,    92,     1,    18,    85,    21,   176,    85,   112,     8,
     1,    52,    92,   171,   172,   182,   104,   188,    44,   107,
    18,    62,   110,   114,    17,   129,    17,   198,    44,   133,
    14,   279,    25,    17,   114,   193,    34,   119,   199,   197,
   144,    36,    37,    18,   148,   272,    20,    18,   130,     0,
     1,   155,     3,     4,     5,     6,     3,    31,     9,    34,
    11,    12,   150,    34,    38,     4,     5,    36,    37,   173,
    13,    10,    17,    18,    13,    20,    13,    28,    17,    35,
    17,    20,   253,   254,    36,    37,    31,   258,    27,   170,
   178,   195,   263,    32,    43,   173,    17,   255,   256,    44,
   296,   297,   273,    30,    25,     4,     5,     4,     5,   280,
    31,    10,   200,    10,    13,    32,    13,    13,    17,    36,
    17,    20,   210,    20,    31,    42,   230,    17,    27,    18,
    27,   302,   303,    32,    32,   239,    15,    21,    36,    18,
    34,   221,    21,    36,    42,    18,   317,   251,   252,   231,
   232,    13,     4,     5,   312,    17,    13,    16,    10,   330,
    17,    13,     4,     5,   322,    17,    13,    44,    20,    35,
    17,    13,   276,   277,    35,    27,   334,    13,    20,   267,
    32,    17,    31,    27,   272,    27,     3,     4,     5,    39,
    40,   279,     1,   281,     3,     4,     5,     6,     7,     8,
    35,    10,    11,   285,    13,    34,    18,    18,    17,    18,
    34,    20,    18,    22,    23,    24,    25,    26,    27,    13,
    29,    31,    30,    30,    33,    30,    17,    31,    21,   333,
    30,     1,    41,     3,     4,     5,     6,     7,     8,    16,
    10,    11,     3,    13,    36,    13,    15,    17,    18,    16,
    20,    30,    22,    23,    24,    25,    26,    27,    42,    29,
    17,    13,    42,    33,    19,     0,    32,    17,    32,    32,
     1,    41,     3,     4,     5,     6,     7,     8,     1,    10,
    11,     1,    13,   221,     1,   281,    17,    18,   210,    20,
    66,    22,    23,    24,    25,    26,    27,    30,    29,     3,
     4,     5,    58,     7,    78,   144,    10,   105,   267,    13,
    41,     4,     5,    17,   144,   263,    20,    10,    40,    29,
    13,    25,    87,    27,    17,    29,    -1,    20,    -1,    22,
    23,    24,     4,     5,    27,    -1,    29,   126,    10,    -1,
    -1,    13,    -1,    -1,    -1,    17,    -1,    -1,    20,    -1,
    22,    23,    24,     4,     5,    27,     7,    29,    -1,    10,
    -1,    -1,    13,    -1,     4,     5,    17,     7,    -1,    20,
    10,    -1,    -1,    13,    25,    -1,    27,    17,    29,    -1,
    20,    -1,    -1,    -1,    -1,    25,    -1,    27,    -1,    29,
     4,     5,    -1,     7,    -1,    -1,    10,    -1,    -1,    13,
    -1,     4,     5,    17,     7,    -1,    20,    10,    -1,    -1,
    13,    -1,    -1,    27,    17,    29,    -1,    20,    -1,     4,
     5,    -1,     7,    -1,    27,    10,    29,    -1,    13,     3,
     4,     5,    -1,    -1,    -1,    20,    10,    -1,    -1,    13,
    -1,    -1,    27,    -1,    29,    -1,    20,    -1,    -1,    -1,
    -1,    -1,    -1,    27
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
#line 113 "mta_parse.y"
{ mta->yy_state = MTA::YYRet_Exit; ;
    break;}
case 2:
#line 114 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok;
	    return mta->yy_state; ;
    break;}
case 3:
#line 117 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 4:
#line 119 "mta_parse.y"
{
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 5:
#line 122 "mta_parse.y"
{
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 6:
#line 125 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 7:
#line 127 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 8:
#line 129 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; ;
    break;}
case 9:
#line 131 "mta_parse.y"
{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; ;
    break;}
case 10:
#line 136 "mta_parse.y"
{ ;
    break;}
case 11:
#line 139 "mta_parse.y"
{
  	  if(yyvsp[0].typ != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("typedef", sp, yyval.typ); } ;
    break;}
case 12:
#line 147 "mta_parse.y"
{ mta->Burp(); ;
    break;}
case 13:
#line 148 "mta_parse.y"
{
	    if(yyvsp[-1].typ != NULL)  SETDESC(yyvsp[-1].typ,yyvsp[0].chr); ;
    break;}
case 14:
#line 152 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 15:
#line 153 "mta_parse.y"
{
	    yyval.typ = yyvsp[-2].typ; yyvsp[-2].typ->name = yyvsp[-1].chr; mta->type_stack.Pop(); ;
    break;}
case 16:
#line 157 "mta_parse.y"
{
            yyval.typ = yyvsp[-1].typ; yyvsp[-1].typ->AddParent(yyvsp[-2].typ); yyvsp[-1].typ->ptr = yyvsp[-2].typ->ptr;
	    yyvsp[-1].typ->par_formal.BorrowUnique(yyvsp[-2].typ->par_formal);
	    yyvsp[-1].typ->par_cache.BorrowUnique(yyvsp[-2].typ->par_cache);
	    mta->type_stack.Pop(); ;
    break;}
case 17:
#line 162 "mta_parse.y"
{ /* annoying place for a comment, but.. */
            yyval.typ = yyvsp[-1].typ; yyvsp[-1].typ->AddParent(yyvsp[-3].typ); yyvsp[-1].typ->ptr = yyvsp[-3].typ->ptr;
	    yyvsp[-1].typ->par_formal.BorrowUnique(yyvsp[-3].typ->par_formal);
	    yyvsp[-1].typ->par_cache.BorrowUnique(yyvsp[-3].typ->par_cache);
	    mta->type_stack.Pop(); ;
    break;}
case 18:
#line 169 "mta_parse.y"
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
#line 199 "mta_parse.y"
{
	    yyval.typ = yyvsp[-3].typ; yyval.typ->AddParent(&TA_void_ptr); yyval.typ->ptr = 1;
	    mta->type_stack.Pop(); ;
    break;}
case 20:
#line 202 "mta_parse.y"
{
	    yyval.typ = yyvsp[0].typ; yyval.typ->AddParent(&TA_void_ptr); yyval.typ->ptr = 1;
	    mta->type_stack.Pop(); ;
    break;}
case 21:
#line 207 "mta_parse.y"
{
            TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
            yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("enum", sp, yyval.typ); ;
    break;}
case 24:
#line 217 "mta_parse.y"
{ SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); ;
    break;}
case 25:
#line 218 "mta_parse.y"
{ SETDESC(yyvsp[-2].typ,yyvsp[0].chr); ;
    break;}
case 26:
#line 221 "mta_parse.y"
{
  	    yyval.typ = yyvsp[0].typ;
	    yyvsp[0].typ->AddParFormal(&TA_enum); mta->cur_enum = yyvsp[0].typ;
	    mta->type_stack.Pop(); ;
    break;}
case 27:
#line 225 "mta_parse.y"
{
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->cur_enum = yyval.typ;
	    yyval.typ->AddParFormal(&TA_enum); yyval.typ->internal = true; ;
    break;}
case 28:
#line 232 "mta_parse.y"
{
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("class", sp, yyval.typ);
	    mta->type_stack.Pop(); ;
    break;}
case 29:
#line 240 "mta_parse.y"
{ mta->Burp(); ;
    break;}
case 30:
#line 241 "mta_parse.y"
{ SETDESC(yyvsp[-2].typ,yyvsp[0].chr); ;
    break;}
case 31:
#line 245 "mta_parse.y"
{
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("class", sp, yyval.typ);
	    mta->type_stack.Pop(); ;
    break;}
case 34:
#line 258 "mta_parse.y"
{
	    if(yyvsp[-2].typ->HasOption("NO_TOKENS")) yyvsp[-2].typ->tokens.keep = false;
	    else yyvsp[-2].typ->tokens.keep = true; ;
    break;}
case 35:
#line 264 "mta_parse.y"
{
	    yyvsp[-1].typ->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); ;
    break;}
case 36:
#line 267 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 37:
#line 269 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[0].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 38:
#line 274 "mta_parse.y"
{ mta->state = MTA::Parse_inclass; ;
    break;}
case 39:
#line 275 "mta_parse.y"
{ mta->state = MTA::Parse_inclass; ;
    break;}
case 40:
#line 278 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
            yyval.typ = yyvsp[0].typ; mta->last_class = mta->cur_class; mta->cur_class = yyvsp[0].typ;
	    yyvsp[0].typ->AddParFormal(&TA_class);
            mta->cur_mstate = MTA::prvt; ;
    break;}
case 41:
#line 283 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
	    yyval.typ = yyvsp[0].typ; mta->last_class = mta->cur_class; mta->cur_class = yyvsp[0].typ;
            mta->cur_mstate = MTA::prvt; ;
    break;}
case 42:
#line 287 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
	    String nm = yyvsp[0].typ->name + "_" + (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->type_stack.Push(yyval.typ);
	    mta->last_class = mta->cur_class; mta->cur_class = yyval.typ;
            mta->cur_mstate = MTA::prvt; ;
    break;}
case 43:
#line 293 "mta_parse.y"
{
            mta->state = MTA::Parse_class;
	    String nm = yyvsp[0].typ->name + "_" + (String)mta->anon_no++; nm += "_";
	    yyval.typ = new TypeDef(nm); mta->type_stack.Push(yyval.typ);
	    mta->last_class = mta->cur_class; mta->cur_class = yyval.typ;
	    mta->cur_mstate = MTA::pblc; ;
    break;}
case 44:
#line 302 "mta_parse.y"
{
            if(yyvsp[0].typ->InheritsFrom(TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent(yyvsp[0].typ); ;
    break;}
case 45:
#line 305 "mta_parse.y"
{
            if(yyvsp[0].typ->InheritsFrom(&TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent(yyvsp[0].typ);
	    if(!mta->cur_class->HasOption("MULT_INHERIT"))
	      mta->cur_class->opts.Add("MULT_INHERIT"); ;
    break;}
case 47:
#line 314 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 48:
#line 315 "mta_parse.y"
{ yyval.typ = &TA_class; ;
    break;}
case 49:
#line 316 "mta_parse.y"
{ yyval.typ = &TA_class; ;
    break;}
case 54:
#line 328 "mta_parse.y"
{
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(yyvsp[0].typ);
	    if(yyval.typ == yyvsp[0].typ) mta->TypeAdded("template", sp, yyval.typ);
	    mta->type_stack.Pop(); ;
    break;}
case 57:
#line 341 "mta_parse.y"
{
          if(yyvsp[-2].typ->HasOption("NO_TOKENS")) yyvsp[-2].typ->tokens.keep = false;
	  else yyvsp[-2].typ->tokens.keep = true; ;
    break;}
case 58:
#line 347 "mta_parse.y"
{
            mta->state = MTA::Parse_inclass; yyvsp[-1].typ->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); ;
    break;}
case 59:
#line 350 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[-1].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 60:
#line 352 "mta_parse.y"
{
	    SETDESC(yyvsp[-2].typ,yyvsp[0].chr); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); ;
    break;}
case 61:
#line 357 "mta_parse.y"
{
	    yyvsp[0].typ->templ_pars.Reset();
	    yyvsp[0].typ->templ_pars.Duplicate(mta->cur_templ_pars);
	    yyvsp[0].typ->internal = true;
	    yyvsp[0].typ->AddParFormal(&TA_template); yyval.typ = yyvsp[0].typ; ;
    break;}
case 62:
#line 364 "mta_parse.y"
{ mta->cur_templ_pars.Reset(); ;
    break;}
case 63:
#line 368 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[0].typ; ;
    break;}
case 64:
#line 369 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[-3].typ; ;
    break;}
case 65:
#line 372 "mta_parse.y"
{
            if(mta->spc == &(mta->spc_target)) /* only add reg_funs in target space */
	       TA_taRegFun.methods.AddUniqNameNew(yyvsp[0].meth);
	    mta->meth_stack.Pop(); ;
    break;}
case 66:
#line 378 "mta_parse.y"
{
            yyvsp[-1].meth->type = yyvsp[-2].typ; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); yyval.meth = yyvsp[-1].meth; ;
    break;}
case 67:
#line 380 "mta_parse.y"
{
            yyvsp[-1].meth->type = &TA_int; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); yyval.meth = yyvsp[-1].meth; ;
    break;}
case 68:
#line 384 "mta_parse.y"
{
            yyvsp[-1].meth->is_static = true; /* consider these to be static functions */
            yyvsp[-1].meth->fun_argc = yyvsp[0].rval; yyvsp[-1].meth->arg_types.size = yyvsp[0].rval; mta->burp_fundefn = true; ;
    break;}
case 72:
#line 394 "mta_parse.y"
{ if(yyvsp[-1].enm != NULL) SETENUMDESC(yyvsp[-1].enm,yyvsp[0].chr); ;
    break;}
case 75:
#line 399 "mta_parse.y"
{ /* trying to do some math */
           yyval.enm = NULL;
	   mta->skiptocommarb(); ;
    break;}
case 76:
#line 404 "mta_parse.y"
{
            mta->cur_enum->enum_vals.Add(yyvsp[0].enm);
	    mta->enum_stack.Pop(); ;
    break;}
case 77:
#line 407 "mta_parse.y"
{ /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add(yyvsp[-2].enm); if(yyvsp[0].rval != -424242) yyvsp[-2].enm->enum_no = yyvsp[0].rval;
	    mta->enum_stack.Pop(); ;
    break;}
case 79:
#line 414 "mta_parse.y"
{
	  if((yyvsp[-2].rval != -424242) && (yyvsp[0].rval != -424242))  yyval.rval = yyvsp[-2].rval + yyvsp[0].rval;
	  else if(yyvsp[-2].rval != -424242)	yyval.rval = yyvsp[-2].rval;
	  else if(yyvsp[0].rval != -424242)	yyval.rval = yyvsp[0].rval;
	  else				yyval.rval = -424242; ;
    break;}
case 80:
#line 419 "mta_parse.y"
{
	  if((yyvsp[-2].rval != -424242) && (yyvsp[0].rval != -424242)) yyval.rval = yyvsp[-2].rval - yyvsp[0].rval;
	  else if(yyvsp[-2].rval != -424242)	yyval.rval = yyvsp[-2].rval;
	  else if(yyvsp[0].rval != -424242)	yyval.rval = yyvsp[0].rval;
	  else				yyval.rval = -424242; ;
    break;}
case 81:
#line 427 "mta_parse.y"
{ yyval.rval = -424242; ;
    break;}
case 83:
#line 432 "mta_parse.y"
{ yyval.enm = new EnumDef(yyvsp[0].chr); mta->enum_stack.Push(yyval.enm); ;
    break;}
case 84:
#line 435 "mta_parse.y"
{ mta->Class_UpdateLastPtrs(); ;
    break;}
case 85:
#line 436 "mta_parse.y"
{ mta->Class_UpdateLastPtrs(); ;
    break;}
case 86:
#line 439 "mta_parse.y"
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
#line 448 "mta_parse.y"
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
#line 461 "mta_parse.y"
{
	    mta->cur_class->sub_types.AddUniqNameNew(yyvsp[0].typ);
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; ;
    break;}
case 89:
#line 464 "mta_parse.y"
{
	    mta->cur_class->sub_types.AddUniqNameNew(yyvsp[0].typ);
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; ;
    break;}
case 90:
#line 467 "mta_parse.y"
{ /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew(yyvsp[-1].typ);
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; yyval.typ = NULL; ;
    break;}
case 91:
#line 471 "mta_parse.y"
{ yyval.typ = NULL; ;
    break;}
case 93:
#line 476 "mta_parse.y"
{ yyval.memb = NULL; mta->cur_mstate = MTA::pblc; ;
    break;}
case 94:
#line 477 "mta_parse.y"
{ yyval.memb = NULL; mta->cur_mstate = MTA::prvt; ;
    break;}
case 95:
#line 478 "mta_parse.y"
{ yyval.memb = NULL; mta->cur_mstate = MTA::prot; ;
    break;}
case 96:
#line 479 "mta_parse.y"
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
#line 500 "mta_parse.y"
{
	  yyval.memb = yyvsp[0].memb; if(yyvsp[0].memb != NULL) yyvsp[0].memb->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; ;
    break;}
case 99:
#line 506 "mta_parse.y"
{ yyval.memb = yyvsp[-1].memb; ;
    break;}
case 100:
#line 507 "mta_parse.y"
{ ;
    break;}
case 101:
#line 508 "mta_parse.y"
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
#line 516 "mta_parse.y"
{ yyvsp[-2].memb->type = yyvsp[-3].typ; yyval.memb = yyvsp[-2].memb; ;
    break;}
case 103:
#line 520 "mta_parse.y"
{
	     if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
             mta->memb_stack.Pop(); yyval.memb = NULL; ;
    break;}
case 104:
#line 524 "mta_parse.y"
{
	     if((mta->cur_mstate == MTA::pblc) && !(yyvsp[0].memb->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew(yyvsp[0].memb);
             mta->memb_stack.Pop(); yyval.memb = NULL; ;
    break;}
case 105:
#line 530 "mta_parse.y"
{
            yyval.memb = new MemberDef(yyvsp[0].chr); mta->cur_memb = yyval.memb; mta->memb_stack.Push(yyval.memb);
            if(mta->cur_memb_type != NULL) yyval.memb->type = mta->cur_memb_type;
            else yyval.memb->type = &TA_int; ;
    break;}
case 106:
#line 536 "mta_parse.y"
{
	    yyval.memb = new MemberDef(yyvsp[-1].chr); mta->cur_memb = yyval.memb; mta->memb_stack.Push(yyval.memb);
	    yyval.memb->fun_ptr = 1; ;
    break;}
case 108:
#line 542 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 109:
#line 543 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 110:
#line 544 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 111:
#line 545 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 112:
#line 546 "mta_parse.y"
{ yyval.meth = NULL; mta->thisname = false; ;
    break;}
case 113:
#line 547 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 114:
#line 548 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 115:
#line 549 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 116:
#line 550 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 117:
#line 551 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 118:
#line 552 "mta_parse.y"
{
	    yyval.meth = NULL; String tmp = yyvsp[0].chr;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew(yyvsp[-2].meth); yyvsp[-2].meth->type = yyvsp[-3].typ;
	      mta->meth_stack.Pop();  yyvsp[-2].meth->fun_argc = yyvsp[-1].rval; yyvsp[-2].meth->arg_types.size = yyvsp[-1].rval;
	      yyvsp[-2].meth->is_static = true; /* consider these to be static functions */
	      SETDESC(yyvsp[-2].meth,yyvsp[0].chr); }
	    else { yyval.meth = NULL; mta->meth_stack.Pop(); } ;
    break;}
case 119:
#line 560 "mta_parse.y"
{
	    yyval.meth = NULL; String tmp = yyvsp[0].chr;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew(yyvsp[-2].meth); yyvsp[-2].meth->type = &TA_int;
	      mta->meth_stack.Pop();  yyvsp[-2].meth->fun_argc = yyvsp[-1].rval; yyvsp[-2].meth->arg_types.size = yyvsp[-1].rval;
	      yyvsp[-2].meth->is_static = true; /* consider these to be static functions */
	      SETDESC(yyvsp[-2].meth,yyvsp[0].chr); }
	    else { yyval.meth = 0; mta->meth_stack.Pop(); } ;
    break;}
case 121:
#line 572 "mta_parse.y"
{ yyval.meth = yyvsp[0].meth; yyvsp[0].meth->is_static = true; ;
    break;}
case 122:
#line 573 "mta_parse.y"
{ yyval.meth = yyvsp[0].meth;  if(yyvsp[0].meth != NULL) yyvsp[0].meth->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; ;
    break;}
case 123:
#line 578 "mta_parse.y"
{ yyval.meth = yyvsp[-1].meth; yyvsp[-1].meth->type = yyvsp[-2].typ; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); ;
    break;}
case 124:
#line 579 "mta_parse.y"
{ yyvsp[-1].meth->type = &TA_int; SETDESC(yyvsp[-1].meth,yyvsp[0].chr); ;
    break;}
case 125:
#line 580 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 126:
#line 581 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 127:
#line 582 "mta_parse.y"
{ yyval.meth = NULL; ;
    break;}
case 128:
#line 585 "mta_parse.y"
{
             yyvsp[-1].meth->fun_argc = yyvsp[0].rval; yyvsp[-1].meth->arg_types.size = yyvsp[0].rval; mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if(yyvsp[-1].meth->fun_argd > yyvsp[-1].meth->fun_argc) yyvsp[-1].meth->fun_argd = -1; ;
    break;}
case 129:
#line 592 "mta_parse.y"
{
            yyval.meth = new MethodDef(yyvsp[0].chr); mta->cur_meth = yyval.meth; mta->meth_stack.Push(yyval.meth); ;
    break;}
case 130:
#line 596 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 131:
#line 597 "mta_parse.y"
{ yyval.chr = yyvsp[0].chr; ;
    break;}
case 132:
#line 598 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 133:
#line 599 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 134:
#line 600 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 135:
#line 601 "mta_parse.y"
{ yyval.chr = yyvsp[-2].chr; ;
    break;}
case 136:
#line 605 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 137:
#line 606 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 138:
#line 607 "mta_parse.y"
{ yyval.chr = yyvsp[-1].chr; ;
    break;}
case 139:
#line 611 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 140:
#line 612 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 141:
#line 613 "mta_parse.y"
{ yyval.chr = ""; ;
    break;}
case 142:
#line 616 "mta_parse.y"
{ yyval.rval = 0; ;
    break;}
case 143:
#line 617 "mta_parse.y"
{ yyval.rval = yyvsp[-1].rval; ;
    break;}
case 144:
#line 620 "mta_parse.y"
{ yyval.rval = 0; ;
    break;}
case 145:
#line 621 "mta_parse.y"
{ yyval.rval = yyvsp[-1].rval; ;
    break;}
case 146:
#line 624 "mta_parse.y"
{ yyval.rval = 1; ;
    break;}
case 147:
#line 625 "mta_parse.y"
{ yyval.rval = yyvsp[-2].rval + 1; ;
    break;}
case 148:
#line 626 "mta_parse.y"
{ yyval.rval = yyvsp[-3].rval; ;
    break;}
case 149:
#line 629 "mta_parse.y"
{
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } ;
    break;}
case 150:
#line 631 "mta_parse.y"
{
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_defs.Add(yyvsp[0].chr);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } ;
    break;}
case 151:
#line 639 "mta_parse.y"
{
	    yyval.rval = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[0].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 152:
#line 643 "mta_parse.y"
{
	    yyval.rval = 1; String nm = yyvsp[0].chr;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-1].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 153:
#line 647 "mta_parse.y"
{
	    yyval.rval = 1; String nm = String(yyvsp[-1].chr) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-2].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 154:
#line 651 "mta_parse.y"
{
	    yyval.rval = 1; String nm = String("(*") + String(yyvsp[-2].chr) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(yyvsp[-5].typ); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 155:
#line 655 "mta_parse.y"
{
	    yyval.rval = 1; String nm = yyvsp[0].chr;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } ;
    break;}
case 159:
#line 670 "mta_parse.y"
{
            mta->thisname = true; mta->constcoln = false; ;
    break;}
case 160:
#line 674 "mta_parse.y"
{ mta->constcoln = true; ;
    break;}
case 161:
#line 677 "mta_parse.y"
{ yyval.rval = 1; ;
    break;}
case 162:
#line 678 "mta_parse.y"
{ yyval.rval = yyvsp[-1].rval + 1; ;
    break;}
case 163:
#line 681 "mta_parse.y"
{ mta->cur_memb_type = yyvsp[0].typ; ;
    break;}
case 165:
#line 685 "mta_parse.y"
{ yyval.typ = &TA_int; ;
    break;}
case 166:
#line 686 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 167:
#line 689 "mta_parse.y"
{ yyval.typ = new TypeDef(yyvsp[0].chr); mta->type_stack.Push(yyval.typ); ;
    break;}
case 169:
#line 693 "mta_parse.y"
{
	    String nm = yyvsp[-1].typ->name + "_ref";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[-1].typ->ptr, true);
	    nty->AddParent(yyvsp[-1].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-1].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("ref", sp, yyval.typ); ;
    break;}
case 171:
#line 704 "mta_parse.y"
{
 	    int i; String nm = yyvsp[-1].typ->name; for(i=0; i<yyvsp[0].rval; i++) nm += "_ptr";
	    TypeDef* nty = new TypeDef((char*)nm, true, yyvsp[0].rval); nty->AddParent(yyvsp[-1].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[-1].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("ptr", sp, yyval.typ); ;
    break;}
case 173:
#line 713 "mta_parse.y"
{
	    String nm = yyvsp[-1].typ->name + "_" + yyvsp[0].typ->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = yyvsp[0].typ->size; nty->AddParent(yyvsp[-1].typ); nty->AddParent(yyvsp[0].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("const", sp, yyval.typ); ;
    break;}
case 175:
#line 723 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 176:
#line 724 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 177:
#line 725 "mta_parse.y"
{
	    TypeDef* td; if((td = yyvsp[-2].typ->sub_types.FindName(yyvsp[0].chr)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    yyval.typ = td; ;
    break;}
case 178:
#line 729 "mta_parse.y"
{
	    TypeDef* td; if((td = yyvsp[-2].typ->sub_types.FindName(yyvsp[0].chr)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    yyval.typ = td; ;
    break;}
case 179:
#line 733 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 180:
#line 734 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 181:
#line 735 "mta_parse.y"
{ yyval.typ = yyvsp[0].typ; ;
    break;}
case 183:
#line 737 "mta_parse.y"
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
case 184:
#line 754 "mta_parse.y"
{ /* this template */
	    if(!(yyvsp[-3].typ->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    yyval.typ = yyvsp[-3].typ; ;
    break;}
case 186:
#line 761 "mta_parse.y"
{
	    String nm = yyvsp[-1].typ->name + "_" + yyvsp[0].typ->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = yyvsp[0].typ->size; nty->AddParent(yyvsp[-1].typ); nty->AddParent(yyvsp[0].typ);
	    TypeSpace* sp = mta->GetTypeSpace(yyvsp[0].typ);
	    yyval.typ = sp->AddUniqNameOld(nty);
	    if(yyval.typ == nty) mta->TypeAdded("combo", sp, yyval.typ); ;
    break;}
case 187:
#line 771 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); ;
    break;}
case 188:
#line 772 "mta_parse.y"
{ mta->cur_templ_pars.Link(yyvsp[0].typ); yyval.typ = yyvsp[-2].typ; ;
    break;}
case 190:
#line 776 "mta_parse.y"
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
