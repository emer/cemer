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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef struct YYSTYPE
#line 44 "mta_parse.y"
{
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  char*    	chr;
  int	   	rval;
}
/* Line 1529 of yacc.c.  */
#line 116 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

