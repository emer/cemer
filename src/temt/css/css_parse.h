/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
     CSS_HELP = 293,
     CSS_EXTERN = 294,
     CSS_STATIC = 295,
     CSS_CONST = 296,
     CSS_PRIVATE = 297,
     CSS_PUBLIC = 298,
     CSS_PROTECTED = 299,
     CSS_VIRTUAL = 300,
     CSS_INLINE = 301,
     CSS_ASGN_OR = 302,
     CSS_ASGN_XOR = 303,
     CSS_ASGN_AND = 304,
     CSS_ASGN_RSHIFT = 305,
     CSS_ASGN_LSHIFT = 306,
     CSS_ASGN_MOD = 307,
     CSS_ASGN_DIV = 308,
     CSS_ASGN_MULT = 309,
     CSS_ASGN_SUB = 310,
     CSS_ASGN_ADD = 311,
     CSS_OR = 312,
     CSS_AND = 313,
     CSS_NE = 314,
     CSS_EQ = 315,
     CSS_LE = 316,
     CSS_LT = 317,
     CSS_GE = 318,
     CSS_GT = 319,
     CSS_RSHIFT = 320,
     CSS_LSHIFT = 321,
     CSS_MINMIN = 322,
     CSS_PLUSPLUS = 323,
     CSS_UNARY = 324,
     CSS_BITNEG = 325,
     CSS_NOT = 326,
     CSS_UNARYMINUS = 327,
     CSS_SCOPER = 328,
     CSS_POINTSAT = 329
   };
#endif
/* Tokens.  */
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
#define CSS_HELP 293
#define CSS_EXTERN 294
#define CSS_STATIC 295
#define CSS_CONST 296
#define CSS_PRIVATE 297
#define CSS_PUBLIC 298
#define CSS_PROTECTED 299
#define CSS_VIRTUAL 300
#define CSS_INLINE 301
#define CSS_ASGN_OR 302
#define CSS_ASGN_XOR 303
#define CSS_ASGN_AND 304
#define CSS_ASGN_RSHIFT 305
#define CSS_ASGN_LSHIFT 306
#define CSS_ASGN_MOD 307
#define CSS_ASGN_DIV 308
#define CSS_ASGN_MULT 309
#define CSS_ASGN_SUB 310
#define CSS_ASGN_ADD 311
#define CSS_OR 312
#define CSS_AND 313
#define CSS_NE 314
#define CSS_EQ 315
#define CSS_LE 316
#define CSS_LT 317
#define CSS_GE 318
#define CSS_GT 319
#define CSS_RSHIFT 320
#define CSS_LSHIFT 321
#define CSS_MINMIN 322
#define CSS_PLUSPLUS 323
#define CSS_UNARY 324
#define CSS_BITNEG 325
#define CSS_NOT 326
#define CSS_UNARYMINUS 327
#define CSS_SCOPER 328
#define CSS_POINTSAT 329




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef struct YYSTYPE
#line 51 "css_parse.y"
{
  cssElPlusIVal el_ival;
  cssElPtr     	el;		/* for coding */
  int		ival;		/* for program indexes (progdx) and other ints */
  char*        	nm;
}
/* Line 1529 of yacc.c.  */
#line 204 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

