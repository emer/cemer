// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
/* -*- C -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than sale or distribution is hereby granted without  //
// fee, provided  that the above copyright notice and this permission notice  //
// appear in all copies of the software and related documentation.            //
//									      //
// PERMISSION TO MARKET, DISTRIBUTE, OR SELL THIS SOFTWARE OR ITS             //
// DOCUMENTATION IS EXPRESSLY *NOT* GRANTED AND IS RETAINED BY THE COPYRIGHT  //
// HOLDERS.					       			      //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR		      //
// ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,    //
// OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,     //
// WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF  //
// LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE     //
// OF THIS SOFTWARE.							      //
//     									      //
==============================================================================*/

/* parser for maketa: type scanning system */

%{

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

%}

/* five expected shift-reduce conflicts */
%expect 5

%union {
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  char*    	chr;
  int	   	rval;
}

/* type definition keywords */
%token	<typ> 	CLASS STRUCT UNION ENUM FUNTYPE STATIC TEMPLATE CONST
%token	<typ>	TYPEDEF

/* pre-defined types */
%token	<typ>	TA_TYPEDEF

/* basic tokens */
%token	<typ>	TYPE
%token	<rval>	NUMBER FUNCTION ARRAY
%token  <chr>	NAME COMMENT FUNCALL SCOPER EQUALS

/* access categories */
%token	<typ>	PUBLIC PRIVATE PROTECTED

/* class member stuff */
%token	<typ>	OPERATOR FRIEND THISNAME REGFUN VIRTUAL


/* top-level types */
%type 	<typ>	typedefn typedefns enumdefn classdecl classdecls
%type	<typ>	classdefn classdefns templdefn templdefns preparsed
%type	<meth>	fundecl

/* typedef stuff */
%type 	<typ>	typedsub defn
%type 	<chr>	tdname

/* enum stuff */
%type 	<typ>	enumdsub enumname enumnm
%type	<enm> 	enums enumline enumitm enumitms enmitmname
%type	<rval> 	enummath enummathitm

/* class stuff */
%type 	<typ>	classdsub classname classhead classnm classinh
%type 	<typ>	classpar classptyp classpmod
%type   <typ>	access classkeyword structunion
%type	<typ>	membs membline membtype
%type   <memb>  membdefn basicmemb nostatmemb membname membnames membfunp
%type 	<meth>	methdefn basicmeth nostatmeth methname mbfundefn

/* template stuff */
%type 	<typ>	templdsub templname templhead templopen templpars
%type	<typ>	templtypes

/* function stuff */
%type   <typ>   funtspec funtsmod
%type 	<meth>	funnm regfundefn
%type   <rval>	funargs constfun args argdefn subargdefn
%type   <chr>	fundefn funsubdecl funsubdefn

/* misc stuff */
%type 	<typ>	type noreftype subtype constype combtype tyname ftype
%type   <chr>	varname
%type   <rval>	ptrs

%left	'*'
%left   SCOPER '('

%%
list:	/* nothing */		{ mta->yy_state = MTA::YYRet_Exit; }
        | list typedefn		{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok;
	    return mta->yy_state; }
        | list enumdefn		{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list classdecl	{
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list classdefn	{
	    mta->cur_class = NULL; mta->state = MTA::Find_Item;
	    mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list templdefn	{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list fundecl		{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list preparsed	{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
	| list error		{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
	;

preparsed:
          TA_TYPEDEF		{ }
        ;

typedefn: typedefns			{
  	  if($1 != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld($1);
	    if($$ == $1) mta->TypeAdded("typedef", sp, $$); } }
        ;

typedefns:
          typedsub			{ mta->Burp(); }/* read ahead to comment */
        | typedsub COMMENT		{
	    if($1 != NULL)  SETDESC($1,$2); }
        ;

typedsub: TYPEDEF defn			{ $$ = $2; }
        | TYPEDEF classdsub tdname term {
	    $$ = $2; $2->name = $3; mta->type_stack.Pop(); }
        ;

defn:     type tyname term		{
            $$ = $2; $2->AddParent($1); $2->ptr = $1->ptr;
	    $2->par_formal.BorrowUnique($1->par_formal);
	    $2->par_cache.BorrowUnique($1->par_cache);
	    mta->type_stack.Pop(); }
        | type COMMENT tyname term	{ /* annoying place for a comment, but.. */
            $$ = $3; $3->AddParent($1); $3->ptr = $1->ptr;
	    $3->par_formal.BorrowUnique($1->par_formal);
	    $3->par_cache.BorrowUnique($1->par_cache);
	    mta->type_stack.Pop(); }
        /* predeclared type, which gets sucked in by the combtype list
	   the second parent of the new type is the actual new type */
        | type term			{
	    TypeDef* td = $1->parents[1]; mta->type_stack.Pop();
	    TypeSpace* sp = $1->owner;
	    sp->Remove($1); /* get rid of new one, cuz it is bogus */
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
		    td->parents.ReplaceLink(0, already_there);
		}
	      }
	    }
	    $$ = td; }
        /* currently, parsing pointer-to-a-function typedefs, but not
	   registering them as such, just giving them void* types */
        | type '(' '*' tyname ')' funargs term {
	    $$ = $4; $$->AddParent(&TA_void_ptr); $$->ptr = 1;
	    mta->type_stack.Pop(); }
        | type SCOPER '*' tyname		{
	    $$ = $4; $$->AddParent(&TA_void_ptr); $$->ptr = 1;
	    mta->type_stack.Pop(); }
        ;

enumdefn: enumdsub			{
            TypeSpace* sp = mta->GetTypeSpace($1);
            $$ = sp->AddUniqNameOld($1);
	    if($$ == $1) mta->TypeAdded("enum", sp, $$); }
        ;

enumdsub: enumname enums '}' term
        ;

enumname: enumnm '{'
        | enumnm COMMENT '{'		{ SETDESC($1,$2); }
        | enumnm '{' COMMENT		{ SETDESC($1,$3); }
        ;

enumnm:   ENUM tyname			{
  	    $$ = $2;
	    $2->AddParFormal(&TA_enum); mta->cur_enum = $2;
	    mta->type_stack.Pop(); }
        | ENUM 				{
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    $$ = new TypeDef(nm); mta->cur_enum = $$;
	    $$->AddParFormal(&TA_enum); $$->internal = true; }
        ;

classdecl:
          classdecls			{
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld($1);
	    if($$ == $1) mta->TypeAdded("class", sp, $$);
	    mta->type_stack.Pop(); }
        ;

classdecls:
          classnm term			{ mta->Burp(); } /* read ahead to comment */
        | classnm term COMMENT		{ SETDESC($1,$3); }
        ;

classdefn:
          classdefns			{
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld($1);
	    if($$ == $1) mta->TypeAdded("class", sp, $$);
	    mta->type_stack.Pop(); }
        ;

classdefns:
          classdsub term
        | classdsub varname term
        ;

classdsub:
          classname membs '}'		{
	    if($1->HasOption("NO_TOKENS")) $1->tokens.keep = false;
	    else $1->tokens.keep = true; }
        ;

classname:
          classhead '{'			{
	    $1->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
        | classhead COMMENT '{'			{
	    SETDESC($1,$2); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
        | classhead '{' COMMENT 		{
	    SETDESC($1,$3); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
        ;

classhead:
          classnm			{ mta->state = MTA::Parse_inclass; }
        | classnm ':' classinh		{ mta->state = MTA::Parse_inclass; }
        ;

classnm:  classkeyword tyname			{
            mta->state = MTA::Parse_class;
            $$ = $2; mta->last_class = mta->cur_class; mta->cur_class = $2;
	    $2->AddParFormal(&TA_class);
            mta->cur_mstate = MTA::prvt; } /* classkeyword == class */
        | classkeyword TYPE			{
            mta->state = MTA::Parse_class;
	    $$ = $2; mta->last_class = mta->cur_class; mta->cur_class = $2;
            mta->cur_mstate = MTA::prvt; } /* classkeyword == class */
        | classkeyword				{
            mta->state = MTA::Parse_class;
	    String nm = $1->name + "_" + (String)mta->anon_no++; nm += "_";
	    $$ = new TypeDef(nm); mta->type_stack.Push($$);
	    mta->last_class = mta->cur_class; mta->cur_class = $$;
            mta->cur_mstate = MTA::prvt; } /* classkeyword == class */
        | structunion				{
            mta->state = MTA::Parse_class;
	    String nm = $1->name + "_" + (String)mta->anon_no++; nm += "_";
	    $$ = new TypeDef(nm); mta->type_stack.Push($$);
	    mta->last_class = mta->cur_class; mta->cur_class = $$;
	    mta->cur_mstate = MTA::pblc; } /* is struct */
        ;

/* class inheritance */
classinh: classpar			{
            if($1->InheritsFrom(TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent($1); }
        | classinh ',' classpar		{
            if($3->InheritsFrom(&TA_taBase)) mta->cur_class->AddParCache(&TA_taBase);
	    mta->cur_class->AddParent($3);
	    if(!mta->cur_class->HasOption("MULT_INHERIT"))
	      mta->cur_class->opts.Add("MULT_INHERIT"); }
        ;

/* class parent */
classpar: type
        | classptyp type	{ $$ = $2; }
        | NAME			{ $$ = &TA_class; } /* unknown parent.. */
        | classptyp NAME	{ $$ = &TA_class; } /* unknown parent.. */
        ;

classptyp: classpmod
        | classptyp classpmod
        ;

classpmod: access
        | VIRTUAL
        ;

templdefn:
          templdefns			{
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld($1);
	    if($$ == $1) mta->TypeAdded("template", sp, $$);
	    mta->type_stack.Pop(); }
        ;

templdefns:
          templdsub term
        | templdsub varname term
        ;

templdsub:
          templname membs '}'		{
          if($1->HasOption("NO_TOKENS")) $1->tokens.keep = false;
	  else $1->tokens.keep = true; }
        ;

templname:
          templhead '{'			{
            mta->state = MTA::Parse_inclass; $1->tokens.keep = true;
	    mta->Class_ResetCurPtrs(); }
        | templhead COMMENT '{'			{
	    SETDESC($1,$2); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
        | templhead '{' COMMENT 		{
	    SETDESC($1,$3); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
        ;

templhead:
          TEMPLATE templopen templpars '>' classhead	{
	    $5->templ_pars.Reset();
	    $5->templ_pars.Duplicate(mta->cur_templ_pars);
	    $5->internal = true;
	    $5->AddParFormal(&TA_template); $$ = $5; }
        ;

templopen: '<'				{ mta->cur_templ_pars.Reset(); }
        ;

templpars:
          CLASS tyname			{ mta->cur_templ_pars.Link($2); $$ = $2; }
        | templpars ',' CLASS tyname	{ mta->cur_templ_pars.Link($4); $$ = $1; }
        ;

fundecl:  funnm				{
            if(mta->spc == &(mta->spc_target)) /* only add reg_funs in target space */
	       TA_taRegFun.methods.AddUniqNameNew($1);
	    mta->meth_stack.Pop(); }
        ;

funnm:    REGFUN ftype regfundefn fundefn	{
            $3->type = $2; SETDESC($3,$4); $$ = $3; }
        | REGFUN regfundefn fundefn		{
            $2->type = &TA_int; SETDESC($2,$3); $$ = $2; }
        ;

regfundefn: methname funargs		{
            $1->is_static = true; /* consider these to be static functions */
            $1->fun_argc = $2; $1->arg_types.size = $2; mta->burp_fundefn = true; }
        ;

enums:    enumline
        | enums enumline
        ;

enumline: enumitm
        | enumitm COMMENT		{ if($1 != NULL) SETENUMDESC($1,$2); }
        ;

enumitm:  enumitms
        | enumitms ','
        | error				{ /* trying to do some math */
           $$ = NULL;
	   mta->skiptocommarb(); }/* skip past junk */
        ;

enumitms: enmitmname			{
            mta->cur_enum->enum_vals.Add($1);
	    mta->enum_stack.Pop(); }
        | enmitmname EQUALS enummath	{ /* using -424242 as a err code (ugly) */
            mta->cur_enum->enum_vals.Add($1); if($3 != -424242) $1->enum_no = $3;
	    mta->enum_stack.Pop(); }
        ;

enummath:
          enummathitm
        | enummath '+' enummathitm	{
	  if(($1 != -424242) && ($3 != -424242))  $$ = $1 + $3;
	  else if($1 != -424242)	$$ = $1;
	  else if($3 != -424242)	$$ = $3;
	  else				$$ = -424242; }
        | enummath '-' enummathitm      {
	  if(($1 != -424242) && ($3 != -424242)) $$ = $1 - $3;
	  else if($1 != -424242)	$$ = $1;
	  else if($3 != -424242)	$$ = $3;
	  else				$$ = -424242; }
        ;

enummathitm:
          NAME				{ $$ = -424242; }
        | NUMBER
        ;

enmitmname:
          NAME			{ $$ = new EnumDef($1); mta->enum_stack.Push($$); }
        ;

membs:	  membline		{ mta->Class_UpdateLastPtrs(); }
        | membs membline	{ mta->Class_UpdateLastPtrs(); }
        ;

membline: membdefn			{
            if($1 != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !($1->HasOption("IGNORE"))
		 && !($1->type->DerivesFrom(TA_const))) {
		mta->cur_class->members.AddUniqNameNew($1);
		if(mta->verbose >= 3)
		  cerr << "member: " << $1->name << " added to: "
		       << mta->cur_class->name << "\n"; } }
	    mta->memb_stack.Pop(); $$ = NULL; }
        | methdefn			{
            if($1 != NULL) {
	      if(mta->cur_mstate == MTA::pblc) {
		if($1->HasOption("IGNORE"))
		  mta->cur_class->ignore_meths.AddUnique($1->name);
		else {
		  mta->cur_class->methods.AddUniqNameNew($1);
		  if(mta->verbose >= 3)
		    cerr << "method: " << $1->name << " added to: "
			 << mta->cur_class->name << "\n"; } } }
	    else {
	      mta->cur_meth = NULL; }
	    mta->meth_stack.Pop(); $$ = NULL; }
        | enumdsub			{
	    mta->cur_class->sub_types.AddUniqNameNew($1);
	    mta->state = MTA::Parse_inclass; $$ = NULL; }
        | typedsub			{
	    mta->cur_class->sub_types.AddUniqNameNew($1);
	    mta->state = MTA::Parse_inclass; $$ = NULL; }
        | classdsub term			{ /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew($1);
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; $$ = NULL; }
        | error				{ $$ = NULL; }
        ;

membdefn:
          basicmemb
        | PUBLIC ':'			{ $$ = NULL; mta->cur_mstate = MTA::pblc; }
        | PRIVATE ':'			{ $$ = NULL; mta->cur_mstate = MTA::prvt; }
        | PROTECTED ':'			{ $$ = NULL; mta->cur_mstate = MTA::prot; }
        | COMMENT	 		{
	    $$ = NULL;
	    if(mta->last_memb != NULL) {
	      SETDESC(mta->last_memb, $1);
	      if(mta->last_memb->HasOption("IGNORE"))
		mta->cur_class->members.Remove(mta->last_memb); }
	    else if(mta->last_meth != NULL) {
	      SETDESC(mta->last_meth, $1);
	      if(mta->last_meth->HasOption("IGNORE")) {
		mta->cur_class->ignore_meths.AddUnique(mta->last_meth->name);
		mta->cur_class->methods.Remove(mta->last_meth);
		mta->last_meth = NULL; }
	      else if((mta->last_meth->opts.size > 0) || (mta->last_meth->lists.size > 0)) {
		mta->cur_class->methods.AddUniqNameNew(mta->last_meth);
		if(mta->verbose >= 3)
		  cerr << "method: " << mta->last_meth->name << " added to: "
		       << mta->cur_class->name << "\n"; } } }
        ;

basicmemb:
          nostatmemb
        | STATIC nostatmemb		{
	  $$ = $2; if($2 != NULL) $2->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
        ;

nostatmemb:
          membtype membnames term	{ $$ = $2; }
        | membnames term		{ }
        | membtype membname ARRAY term	{
	    $$ = $2;
	    String nm = $1->name + "_ary";
	    TypeDef* nty = new TypeDef((char*)nm, true, $1->ptr + 1);
	    nty->AddParFormal(&TA_ta_array); nty->AddParent($1);
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    TypeDef* uty = sp->AddUniqNameOld(nty); $2->type = uty;
	    if(uty == nty) mta->TypeAdded("array", sp, uty); }
        | membtype membfunp funargs term { $2->type = $1; $$ = $2; }
        ;

membnames:
           membname			{
	     if((mta->cur_mstate == MTA::pblc) && !($1->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew($1);
             mta->memb_stack.Pop(); $$ = NULL; }
        |  membnames ',' membname       {
	     if((mta->cur_mstate == MTA::pblc) && !($3->type->DerivesFrom(TA_const)))
	       mta->cur_class->members.AddUniqNameNew($3);
             mta->memb_stack.Pop(); $$ = NULL; }
        ;

membname: NAME			{
            $$ = new MemberDef($1); mta->cur_memb = $$; mta->memb_stack.Push($$);
            if(mta->cur_memb_type != NULL) $$->type = mta->cur_memb_type;
            else $$->type = &TA_int; }
        ;

membfunp: '(' '*' NAME ')'	{
	    $$ = new MemberDef($3); mta->cur_memb = $$; mta->memb_stack.Push($$);
	    $$->fun_ptr = 1; }
        ;

methdefn: basicmeth
        | consthsnm constfun fundefn		{ $$ = NULL; mta->thisname = false; }
        | funtspec consthsnm constfun fundefn	{ $$ = NULL; mta->thisname = false; }
        | consthsnm constfun constrlist fundefn	{ $$ = NULL; mta->thisname = false; }
        | '~' consthsnm constfun fundefn	{ $$ = NULL; mta->thisname = false; }
        | funtspec '~' consthsnm constfun fundefn { $$ = NULL; mta->thisname = false; }
        | FRIEND ftype OPERATOR funargs fundefn	{ $$ = NULL; }
        | FRIEND OPERATOR funargs fundefn	{ $$ = NULL; }
        | FRIEND ftype term			{ $$ = NULL; }
        | FRIEND CLASS ftype term		{ $$ = NULL; }
        | FRIEND CLASS NAME term		{ $$ = NULL; }
        | FRIEND ftype methname funargs fundefn 	{
	    $$ = NULL; String tmp = $5;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew($3); $3->type = $2;
	      mta->meth_stack.Pop();  $3->fun_argc = $4; $3->arg_types.size = $4;
	      $3->is_static = true; /* consider these to be static functions */
	      SETDESC($3,$5); }
	    else { $$ = NULL; mta->meth_stack.Pop(); } }
        | FRIEND methname funargs fundefn 	{
	    $$ = NULL; String tmp = $4;
	    if(tmp.contains("REG_FUN") && (mta->spc == &(mta->spc_target))) {
	      TA_taRegFun.methods.AddUniqNameNew($2); $2->type = &TA_int;
	      mta->meth_stack.Pop();  $2->fun_argc = $3; $2->arg_types.size = $3;
	      $2->is_static = true; /* consider these to be static functions */
	      SETDESC($2,$4); }
	    else { $$ = 0; mta->meth_stack.Pop(); } }
        ;

basicmeth:
          nostatmeth
        | STATIC nostatmeth		{ $$ = $2; $2->is_static = true; }
        ;

nostatmeth:
          membtype mbfundefn fundefn		{ $$ = $2; $2->type = $1; SETDESC($2,$3); }
        | mbfundefn fundefn			{ $1->type = &TA_int; SETDESC($1,$2); }
        | membtype OPERATOR funargs fundefn	{ $$ = NULL; }
        | membtype OPERATOR '(' ')' funargs fundefn { $$ = NULL; }
        | OPERATOR funargs fundefn		{ $$ = NULL; }
        ;

mbfundefn: methname funargs 		{
             $1->fun_argc = $2; $1->arg_types.size = $2; mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if($1->fun_argd > $1->fun_argc) $1->fun_argd = -1; }
        ;

methname: NAME			{
            $$ = new MethodDef($1); mta->cur_meth = $$; mta->meth_stack.Push($$); }
        ;

fundefn:  term				{ $$ = ""; }
        | funsubdefn			{ $$ = $1; }
        | funsubdecl term		{ $$ = $1; }
        | COMMENT term			{ $$ = $1; }
        | COMMENT funsubdefn		{ $$ = $1; }
        | COMMENT funsubdecl term	{ $$ = $1; }
        ;

funsubdefn:
          FUNCTION			{ $$ = ""; }
        | CONST FUNCTION		{ $$ = ""; }
        | CONST COMMENT FUNCTION	{ $$ = $2; }
        ;

funsubdecl:
          EQUALS			{ $$ = ""; }
        | CONST				{ $$ = ""; }
        | CONST EQUALS			{ $$ = ""; }
        ;

funargs:  '(' ')'			{ $$ = 0; }
        | '(' args ')'			{ $$ = $2; }
        ;

constfun: ')'				{ $$ = 0; }
        | args ')'			{ $$ = $1; }
        ;

args:	  argdefn			{ $$ = 1; }
        | args ',' argdefn		{ $$ = $1 + 1; }
        | args '.' '.' '.'		{ $$ = $1; } /* todo: flag this.. */
        ;

argdefn:  subargdefn			{
            if(mta->cur_meth != NULL) { mta->cur_meth->arg_defs.Add(""); } }
        | subargdefn EQUALS		{
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_defs.Add($2);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
	;

subargdefn:
          type				{
	    $$ = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link($1); mta->cur_meth->arg_names.Add(nm); } }
        | type NAME			{
	    $$ = 1; String nm = $2;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link($1); mta->cur_meth->arg_names.Add(nm); } }
        | type NAME ARRAY		{
	    $$ = 1; String nm = String($2) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link($1); mta->cur_meth->arg_names.Add(nm); } }
        | type '(' '*' NAME ')'	funargs	{
	    $$ = 1; String nm = String("(*") + String($4) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link($1); mta->cur_meth->arg_names.Add(nm); } }
        | NAME				{
	    $$ = 1; String nm = $1;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
        ;

constrlist:
          constref
        | constrlist constref
        ;

constref: constcoln TYPE FUNCALL
        ;

consthsnm:
          THISNAME '('			{
            mta->thisname = true; mta->constcoln = false; }
        ;

constcoln: ':'				{ mta->constcoln = true; }
        ;

ptrs:	  '*' 				{ $$ = 1; }
        | ptrs '*'			{ $$ = $1 + 1; }
        ;

membtype:  ftype		{ mta->cur_memb_type = $1; }
        ;

ftype:	  type
        | funtspec		{ $$ = &TA_int; }
        | funtspec type		{ $$ = $2; }
        ;

tyname:	  NAME			{ $$ = new TypeDef($1); mta->type_stack.Push($$); }
        ;

type:	  noreftype
        | noreftype '&'			{
	    String nm = $1->name + "_ref";
	    TypeDef* nty = new TypeDef((char*)nm, true, $1->ptr, true);
	    nty->AddParent($1);
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld(nty);
	    if($$ == nty) mta->TypeAdded("ref", sp, $$); }
        ;

noreftype:
	  constype
        | constype ptrs			{
 	    int i; String nm = $1->name; for(i=0; i<$2; i++) nm += "_ptr";
	    TypeDef* nty = new TypeDef((char*)nm, true, $2); nty->AddParent($1);
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld(nty);
	    if($$ == nty) mta->TypeAdded("ptr", sp, $$); }
        ;

constype: subtype
        | CONST subtype			{
	    String nm = $1->name + "_" + $2->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = $2->size; nty->AddParent($1); nty->AddParent($2);
	    TypeSpace* sp = mta->GetTypeSpace($2);
	    $$ = sp->AddUniqNameOld(nty);
	    if($$ == nty) mta->TypeAdded("const", sp, $$); }
        ;

subtype:  combtype
        | structunion combtype		{ $$ = $2; }
        | structunion tyname		{ $$ = $2; }
	| TYPE SCOPER NAME		{
	    TypeDef* td; if((td = $1->sub_types.FindName($3)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    $$ = td; }
	| THISNAME SCOPER NAME		{
	    TypeDef* td; if((td = $1->sub_types.FindName($3)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    $$ = td; }
	| TYPE SCOPER TYPE		{ $$ = $3; }
	| THISNAME SCOPER TYPE		{ $$ = $3; }
        | SCOPER TYPE			{ $$ = $2; }
        | THISNAME
        | TYPE templopen templtypes '>'		{ /* a template */
 	    if(!($1->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    if(($3->owner != NULL) && ($3->owner->owner != NULL))
	      $$ = $1;	/* don't allow internal types with external templates */
	    else {
	      String nm = $1->GetTemplName(mta->cur_templ_pars);
	      TypeDef* td;
	      int lx_tok;
	      if((td = mta->FindName(nm, lx_tok)) == NULL) {
		td = $1->Clone(); td->name = nm;
		td->SetTemplType($1, mta->cur_templ_pars);
		TypeSpace* sp = mta->GetTypeSpace($1);
 		$$ = sp->AddUniqNameOld(td);
		if($$ == td) mta->TypeAdded("template instance", sp, $$); }
	      else
		$$ = td; } }
	| THISNAME templopen templtypes '>'	{ /* this template */
	    if(!($1->InheritsFormal(TA_template))) {
	      yyerror("Template syntax error"); YYERROR; }
	    $$ = $1; }
        ;

combtype: TYPE
        | combtype TYPE			{
	    String nm = $1->name + "_" + $2->name;
	    TypeDef* nty = new TypeDef((char*)nm, true);
	    nty->size = $2->size; nty->AddParent($1); nty->AddParent($2);
	    TypeSpace* sp = mta->GetTypeSpace($2);
	    $$ = sp->AddUniqNameOld(nty);
	    if($$ == nty) mta->TypeAdded("combo", sp, $$); }
        ;

templtypes:
          TYPE			{ mta->cur_templ_pars.Link($1); }
        | templtypes ',' TYPE	{ mta->cur_templ_pars.Link($3); $$ = $1; }
        ;

tdname:   NAME
        | TYPE			{ $$ = $1->name; }
        ;

funtspec: funtsmod
        | funtspec funtsmod
        ;

funtsmod: FUNTYPE
        | VIRTUAL
        ;

varname:  NAME
        | NAME ARRAY
        ;

term:	  ';'
        ;

access:   PUBLIC
        | PRIVATE
        | PROTECTED
        ;

structunion:
          STRUCT
        | UNION
	;

classkeyword:
          CLASS
        ;

%%

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
