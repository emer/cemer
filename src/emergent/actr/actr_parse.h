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
     AP_BANG_EXPR = 270,
     AP_DEFVAR = 271,
     AP_DEFMETHOD = 272,
     AP_DEFUN = 273,
     AP_SETF = 274
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
#define AP_BANG_EXPR 270
#define AP_DEFVAR 271
#define AP_DEFMETHOD 272
#define AP_DEFUN 273
#define AP_SETF 274




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
/* Line 1529 of yacc.c.  */
#line 98 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE aplval;

