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
     CLASS = 258,
     TYPENAME = 259,
     STRUCT = 260,
     UNION = 261,
     ENUM = 262,
     FUNTYPE = 263,
     STATIC = 264,
     TEMPLATE = 265,
     CONST = 266,
     TYPEDEF = 267,
     TA_TYPEDEF = 268,
     TYPE = 269,
     NUMBER = 270,
     FUNCTION = 271,
     ARRAY = 272,
     NAME = 273,
     COMMENT = 274,
     FUNCALL = 275,
     SCOPER = 276,
     EQUALS = 277,
     PUBLIC = 278,
     PRIVATE = 279,
     PROTECTED = 280,
     OPERATOR = 281,
     FRIEND = 282,
     THISNAME = 283,
     REGFUN = 284,
     VIRTUAL = 285
   };
#endif
/* Tokens.  */
#define CLASS 258
#define TYPENAME 259
#define STRUCT 260
#define UNION 261
#define ENUM 262
#define FUNTYPE 263
#define STATIC 264
#define TEMPLATE 265
#define CONST 266
#define TYPEDEF 267
#define TA_TYPEDEF 268
#define TYPE 269
#define NUMBER 270
#define FUNCTION 271
#define ARRAY 272
#define NAME 273
#define COMMENT 274
#define FUNCALL 275
#define SCOPER 276
#define EQUALS 277
#define PUBLIC 278
#define PRIVATE 279
#define PROTECTED 280
#define OPERATOR 281
#define FRIEND 282
#define THISNAME 283
#define REGFUN 284
#define VIRTUAL 285




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef struct YYSTYPE
#line 44 "mta_parse.y"
{
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  const char*   chr;
  int	   	rval;
}
/* Line 1489 of yacc.c.  */
#line 118 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

