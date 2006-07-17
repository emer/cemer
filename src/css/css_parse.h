/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CSS_PP_INCLUDE = 258,
     CSS_PP_DEFINE = 259,
     CSS_PP_UNDEF = 260,
     CSS_PP_IFDEF = 261,
     CSS_PP_IFNDEF = 262,
     CSS_PP_ELSE = 263,
     CSS_PP_ENDIF = 264,
     CSS_NUMBER = 265,
     CSS_STRING = 266,
     CSS_VAR = 267,
     CSS_FUN = 268,
     CSS_PTR = 269,
     CSS_PP_DEF = 270,
     CSS_NAME = 271,
     CSS_COMMENT = 272,
     CSS_TYPE = 273,
     CSS_PTRTYPE = 274,
     CSS_CLASS = 275,
     CSS_ENUM = 276,
     CSS_SCPTYPE = 277,
     CSS_WHILE = 278,
     CSS_DO = 279,
     CSS_IF = 280,
     CSS_ELSE = 281,
     CSS_SWITCH = 282,
     CSS_CASE = 283,
     CSS_DEFAULT = 284,
     CSS_RETURN = 285,
     CSS_BREAK = 286,
     CSS_CONTINUE = 287,
     CSS_FOR = 288,
     CSS_NEW = 289,
     CSS_DELETE = 290,
     CSS_COMMAND = 291,
     CSS_ALIAS = 292,
     CSS_REMOVE = 293,
     CSS_TYPECMD = 294,
     CSS_HELP = 295,
     CSS_EXTERN = 296,
     CSS_STATIC = 297,
     CSS_CONST = 298,
     CSS_PRIVATE = 299,
     CSS_PUBLIC = 300,
     CSS_PROTECTED = 301,
     CSS_VIRTUAL = 302,
     CSS_INLINE = 303,
     CSS_ASGN_OR = 304,
     CSS_ASGN_XOR = 305,
     CSS_ASGN_AND = 306,
     CSS_ASGN_RSHIFT = 307,
     CSS_ASGN_LSHIFT = 308,
     CSS_ASGN_MOD = 309,
     CSS_ASGN_DIV = 310,
     CSS_ASGN_MULT = 311,
     CSS_ASGN_SUB = 312,
     CSS_ASGN_ADD = 313,
     CSS_OR = 314,
     CSS_AND = 315,
     CSS_NE = 316,
     CSS_EQ = 317,
     CSS_LE = 318,
     CSS_LT = 319,
     CSS_GE = 320,
     CSS_GT = 321,
     CSS_RSHIFT = 322,
     CSS_LSHIFT = 323,
     CSS_MINMIN = 324,
     CSS_PLUSPLUS = 325,
     CSS_UNARY = 326,
     CSS_NOT = 327,
     CSS_UNARYMINUS = 328,
     CSS_SCOPER = 329,
     CSS_POINTSAT = 330
   };
#endif
#define CSS_PP_INCLUDE 258
#define CSS_PP_DEFINE 259
#define CSS_PP_UNDEF 260
#define CSS_PP_IFDEF 261
#define CSS_PP_IFNDEF 262
#define CSS_PP_ELSE 263
#define CSS_PP_ENDIF 264
#define CSS_NUMBER 265
#define CSS_STRING 266
#define CSS_VAR 267
#define CSS_FUN 268
#define CSS_PTR 269
#define CSS_PP_DEF 270
#define CSS_NAME 271
#define CSS_COMMENT 272
#define CSS_TYPE 273
#define CSS_PTRTYPE 274
#define CSS_CLASS 275
#define CSS_ENUM 276
#define CSS_SCPTYPE 277
#define CSS_WHILE 278
#define CSS_DO 279
#define CSS_IF 280
#define CSS_ELSE 281
#define CSS_SWITCH 282
#define CSS_CASE 283
#define CSS_DEFAULT 284
#define CSS_RETURN 285
#define CSS_BREAK 286
#define CSS_CONTINUE 287
#define CSS_FOR 288
#define CSS_NEW 289
#define CSS_DELETE 290
#define CSS_COMMAND 291
#define CSS_ALIAS 292
#define CSS_REMOVE 293
#define CSS_TYPECMD 294
#define CSS_HELP 295
#define CSS_EXTERN 296
#define CSS_STATIC 297
#define CSS_CONST 298
#define CSS_PRIVATE 299
#define CSS_PUBLIC 300
#define CSS_PROTECTED 301
#define CSS_VIRTUAL 302
#define CSS_INLINE 303
#define CSS_ASGN_OR 304
#define CSS_ASGN_XOR 305
#define CSS_ASGN_AND 306
#define CSS_ASGN_RSHIFT 307
#define CSS_ASGN_LSHIFT 308
#define CSS_ASGN_MOD 309
#define CSS_ASGN_DIV 310
#define CSS_ASGN_MULT 311
#define CSS_ASGN_SUB 312
#define CSS_ASGN_ADD 313
#define CSS_OR 314
#define CSS_AND 315
#define CSS_NE 316
#define CSS_EQ 317
#define CSS_LE 318
#define CSS_LT 319
#define CSS_GE 320
#define CSS_GT 321
#define CSS_RSHIFT 322
#define CSS_LSHIFT 323
#define CSS_MINMIN 324
#define CSS_PLUSPLUS 325
#define CSS_UNARY 326
#define CSS_NOT 327
#define CSS_UNARYMINUS 328
#define CSS_SCOPER 329
#define CSS_POINTSAT 330




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 59 "css_parse.y"
typedef struct YYSTYPE {
  cssElPlusIVal el_ival;
  cssElPtr     	el;		/* for coding */
  int		ival;		/* for program indexes (progdx) and other ints */
  char*        	nm;
} YYSTYPE;
/* Line 1318 of yacc.c.  */
#line 194 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



