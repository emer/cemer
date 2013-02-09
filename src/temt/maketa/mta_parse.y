// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

/* parser for maketa: type scanning system */

%{

#include "maketa.h"
#include <taMisc>
#include <BuiltinTypeDefs>

#if defined(SUN4) && !defined(__GNUG__) && !defined(SOLARIS)
#include <alloca.h>
#endif

#if defined(SGI) || defined(SGIdebug)
#include <alloca.h>
#endif

static String_PArray bogus_inh_opts;

#define SETDESC(ty,cm)	mta->SetDesc(cm, ty->desc, ty->inh_opts, ty->opts, ty->lists)

#define SETENUMDESC(ty,cm) mta->SetDesc(cm, ty->desc, bogus_inh_opts, ty->opts, ty->lists)

void yyerror(const char *s);
int yylex();

%}

/* five expected shift-reduce conflicts */
%expect 5

%union {
  TypeDef* 	typ;
  EnumDef* 	enm;
  MemberDef* 	memb;
  MethodDef* 	meth;
  const char*   chr;
  int	   	rval;
}

/* type definition keywords */
%token	<typ> 	MP_CLASS MP_TYPENAME MP_STRUCT MP_UNION MP_ENUM MP_FUNTYPE
%token	<typ>	MP_STATIC MP_TEMPLATE MP_CONST MP_TYPEDEF 

/* basic tokens */
%token	<typ>	MP_TYPE
%token	<rval>	MP_NUMBER MP_FUNCTION MP_ARRAY
%token  <chr>	MP_NAME MP_COMMENT MP_FUNCALL MP_SCOPER MP_EQUALS
%token  <chr>   MP_USING MP_NAMESPACE

/* access categories */
%token	<typ>	MP_PUBLIC MP_PRIVATE MP_PROTECTED

/* class member stuff */
%token	<typ>	MP_OPERATOR MP_FRIEND MP_THISNAME MP_REGFUN MP_VIRTUAL


/* top-level types */
%type 	<typ>	typedefn typedefns enumdefn classdecl classdecls
%type	<typ>	classdefn classdefns templdefn templdefns templdecl templdecls
%type	<meth>	fundecl

/* typedef stuff */
%type 	<typ>	typedsub defn
%type 	<chr>	tdname usenamespc namespc

/* enum stuff */
%type 	<typ>	enumdsub enumname enumnm
%type	<enm> 	enums enumline enumitm enumitms enmitmname
%type	<rval> 	enummath enummathitm

/* class stuff */
%type 	<typ>	classdsub classname classhead classnm classinh
%type 	<typ>	classpar classptyp classpmod
%type   <typ>	access classkeyword structkeyword unionkeyword templatekeyword
%type	<typ>	membs membline membtype
%type   <memb>  membdefn basicmemb nostatmemb membname membnames membfunp
%type 	<meth>	methdefn basicmeth nostatmeth methname mbfundefn

/* template stuff */
%type 	<typ>	templdsub templname templhead templopen templpar templpars
%type	<typ>	templargs templarg

/* function stuff */
%type 	<meth>	funnm regfundefn
%type   <rval>	funargs constfun args argdefn subargdefn
%type   <chr>	fundefn funsubdecl funsubdefn

/* misc stuff */
%type 	<typ>	type noreftype subtype constype combtype tyname ftype
%type   <chr>	varname
%type   <rval>	ptrs

%left	'*'
%left   MP_SCOPER '('

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
        | list templdecl	{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list templdefn	{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list fundecl		{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list usenamespc       {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list namespc           {
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
        | list '}'              {
          // presumably leaving a namespace -- check that..
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_Ok; return mta->yy_state; }
	| list error		{
	    mta->state = MTA::Find_Item; mta->yy_state = MTA::YYRet_NoSrc; return mta->yy_state; }
	;

typedefn: typedefns			{
  	  if($1 != NULL) {
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld($1);
            // a typedef can never be literally a template or a template inst!
            $$->ClearType(TypeDef::TEMPLATE);
            $$->ClearType(TypeDef::TEMPL_INST);
	    if($$ == $1) mta->TypeAdded("typedef", sp, $$); } }
        ;

typedefns:
          typedsub			{ mta->Burp(); }/* read ahead to comment */
        | typedsub MP_COMMENT		{
	    if($1 != NULL)  SETDESC($1,$2); }
        ;

typedsub: MP_TYPEDEF defn		{ $$ = $2; }
        | MP_TYPEDEF classdsub tdname term {
	    $$ = $2; $2->name = $3; mta->type_stack.Pop(); }
        ;

defn:     type tyname term		{
            $$ = $2; $2->AddParent($1); $2->AssignType($1->type);
	    mta->type_stack.Pop(); }
        | type MP_COMMENT tyname term	{ /* annoying place for a comment, but.. */
            $$ = $3; $3->AddParent($1); $3->AssignType($1->type);
	    mta->type_stack.Pop(); }
        /* predeclared type, which gets sucked in by the combtype list
	   the second parent of the new type is the actual new type */
        | type term			{
	  if($1->parents.size < 2) {
	    cerr << "E!!: Error in predeclared type: " << $1->name << " second parent not found!"
		 << endl;
	    $$ = $1;
	  }
	  else {
	    TypeDef* td = $1->parents[1]; mta->type_stack.Pop();
	    $$ = td; } }
        | type '(' '*' tyname ')' funargs term {
            $$ = $4; $$->AssignType(TypeDef::FUN_PTR);
	    mta->type_stack.Pop(); }
        | type MP_SCOPER '*' tyname	{
            $$ = $4; $$->AssignType(TypeDef::METH_PTR);
	    mta->type_stack.Pop(); }
        | tyname tyname term            {
            $$ = $2; }
        | tyname type term            {
            $$ = $2; }
        ;

enumdefn: enumdsub			{
            TypeSpace* sp = mta->GetTypeSpace($1);
            $$ = sp->AddUniqNameOld($1);
	    if($$ == $1) { mta->TypeAdded("enum", sp, $$); $$->source_end = mta->line-1; } }
        ;

enumdsub: enumname enums '}' term
        ;

enumname: enumnm '{'
        | enumnm MP_COMMENT '{'		{ SETDESC($1,$2); }
        | enumnm '{' MP_COMMENT		{ SETDESC($1,$3); }
        ;

enumnm:   MP_ENUM tyname		{
  	    $$ = $2;
	    $2->AssignType(TypeDef::ENUM); mta->cur_enum = $2;
	    mta->SetSource($$, false); mta->type_stack.Pop(); }
        | MP_ENUM 				{
	    String nm = "enum_"; nm += (String)mta->anon_no++; nm += "_";
	    $$ = new TypeDef(nm); mta->cur_enum = $$;
	    mta->SetSource($$, false); $$->AssignType(TypeDef::ENUM); }
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
        | classnm term MP_COMMENT	{ SETDESC($1,$3); }
        ;

classdefn:
          classdefns			{
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld($1);
	    if($$ == $1) { mta->TypeAdded("class", sp, $$); mta->FixClassTypes($$);
              $$->source_end = mta->line-1; }
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
	    $1->tokens.keep = true; mta->Class_ResetCurPtrs();
	    mta->SetSource($1, true); }
        | classhead MP_COMMENT '{'			{
	    SETDESC($1,$2); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
            mta->SetSource($1, true); }
        | classhead '{' MP_COMMENT 		{
	    SETDESC($1,$3); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs();
	    mta->SetSource($1, true); 
	  }
        ;

classhead:
          classnm			{ mta->state = MTA::Parse_inclass; }
        | classnm ':' classinh		{ mta->state = MTA::Parse_inclass; }
        ;

classnm:  classkeyword tyname			{
            mta->state = MTA::Parse_class;
            $$ = $2; mta->last_class = mta->cur_class; mta->cur_class = $2;
	    $2->AssignType(TypeDef::CLASS); mta->ClearSource($2); /* tyname set -- premature */
            mta->cur_mstate = MTA::prvt; } /* classkeyword == class */
        | classkeyword MP_TYPE			{
            mta->state = MTA::Parse_class;
	    $$ = $2; mta->last_class = mta->cur_class; mta->cur_class = $2;
            mta->cur_mstate = MTA::prvt; } /* classkeyword == class */
        | classkeyword				{
            mta->state = MTA::Parse_class;
	    String nm = $1->name + "_" + (String)mta->anon_no++; nm += "_";
	    $$ = new TypeDef(nm); $$->AssignType(TypeDef::CLASS); 
            mta->type_stack.Push($$);
	    mta->last_class = mta->cur_class; mta->cur_class = $$;
            mta->cur_mstate = MTA::prvt; } /* classkeyword == class */
        | structkeyword				{
            mta->state = MTA::Parse_class;
	    String nm = $1->name + "_" + (String)mta->anon_no++; nm += "_";
	    $$ = new TypeDef(nm); $$->AssignType(TypeDef::STRUCT);
            mta->type_stack.Push($$);
	    mta->last_class = mta->cur_class; mta->cur_class = $$;
	    mta->cur_mstate = MTA::pblc; } /* is struct */
        | structkeyword	tyname			{
            mta->state = MTA::Parse_class;
            $$ = $2; mta->last_class = mta->cur_class; mta->cur_class = $2;
	    $2->AssignType(TypeDef::CLASS); mta->ClearSource($2); /* tyname set -- premature */
	    mta->cur_mstate = MTA::pblc; } /* is struct */
        | unionkeyword				{
            mta->state = MTA::Parse_class;
	    String nm = $1->name + "_" + (String)mta->anon_no++; nm += "_";
	    $$ = new TypeDef(nm); $$->AssignType(TypeDef::UNION);
            mta->type_stack.Push($$);
	    mta->last_class = mta->cur_class; mta->cur_class = $$;
	    mta->cur_mstate = MTA::pblc; } /* is struct */
        ;

/* class inheritance */
classinh: classpar			{
            if($1 != NULL) mta->cur_class->AddParent($1); }
        | classinh ',' classpar		{
            if($3 != NULL) {mta->cur_class->AddParent($3);
	      if(!mta->cur_class->HasOption("MULT_INHERIT"))
                mta->cur_class->opts.Add("MULT_INHERIT"); } }
        ;

/* class parent */
classpar: type
        | classptyp type	{ $$ = $2; }
        | MP_NAME		{ $$ = NULL; } /* unknown parent.. */
        | classptyp MP_NAME	{ $$ = NULL; } /* unknown parent.. */
        ;

classptyp: classpmod
        | classptyp classpmod
        ;

classpmod: access
        | MP_VIRTUAL
        ;

templdecl:
          templdecls                 {
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld($1);
	    if($$ == $1) { mta->TypeAdded("template", sp, $$); }
	    mta->type_stack.Pop(); }
        ;

templdecls:
          templhead term                 { mta->Burp(); }
 	| templhead term MP_COMMENT      { SETDESC($1,$3); }
        ;

templdefn:
          templdefns			{
	    TypeSpace* sp = mta->GetTypeSpace($1);
	    $$ = sp->AddUniqNameOld($1);
	    if($$ == $1) { mta->TypeAdded("template", sp, $$);
              mta->FixClassTypes($$);
	      $$->source_end = mta->line-1; }
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
        | templhead MP_COMMENT '{'		{
	    SETDESC($1,$2); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
        | templhead '{' MP_COMMENT 		{
	    SETDESC($1,$3); mta->state = MTA::Parse_inclass; mta->Class_ResetCurPtrs(); }
        ;

templhead:
          templatekeyword templopen templpars '>' classhead	{
	    $5->templ_pars.Reset();
	    $5->templ_pars.Duplicate(mta->cur_templ_pars);
	    $5->SetType(TypeDef::TEMPLATE); $$ = $5;
	    mta->SetSource($$, true); }
        ;

templopen: '<'				{ mta->cur_templ_pars.Reset(); }
        ;

templpars:
          templpar
        | templpar ',' templpar		{ $$ = $1; }
        ;

templpar:
          MP_CLASS tyname		{ mta->cur_templ_pars.Link($2); $$ = $2; }
        | MP_TYPENAME tyname		{ mta->cur_templ_pars.Link($2); $$ = $2; }
        | type tyname			{ mta->cur_templ_pars.Link($2); $$ = $2; }
        | type tyname MP_EQUALS		{ mta->cur_templ_pars.Link($2); $$ = $2; }
        ;

fundecl:  funnm				{
            if(mta->cur_is_trg) { /* only add reg_funs in target space */
              TypeDef* nt = new TypeDef($1->name, TypeDef::FUNCTION,0,0);
              mta->SetSource(nt, false);
              taMisc::types.Add(nt);
              nt->methods.AddUniqNameNew($1);
              taMisc::reg_funs.Link(nt); }
            mta->meth_stack.Pop(); }
        ;

funnm:    MP_REGFUN ftype regfundefn fundefn	{
            $3->type = $2; SETDESC($3,$4); $$ = $3; }
        | MP_REGFUN regfundefn fundefn		{
            $2->type = &TA_int; SETDESC($2,$3); $$ = $2; }
        ;

regfundefn: methname funargs		{
            $1->is_static = true; /* consider these to be static functions */
            $1->fun_argc = $2; $1->arg_types.size = $2; mta->burp_fundefn = true; }
        ;

usenamespc:  MP_USING MP_NAMESPACE MP_NAME term {
            taMisc::Info("using namespace:", $3);
          }
        ;

namespc:  MP_NAMESPACE MP_NAME '{' {
            taMisc::Info("entering namespace:", $2);
          }
        ;

enums:    enumline
        | enums enumline
        ;

enumline: enumitm
        | enumitm MP_COMMENT		{ if($1 != NULL) SETENUMDESC($1,$2); }
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
        | enmitmname MP_EQUALS enummath	{ /* using -424242 as a err code (ugly) */
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
          MP_NAME		{ $$ = -424242; }
        | MP_NUMBER
        ;

enmitmname:
          MP_NAME		{ $$ = new EnumDef($1); mta->enum_stack.Push($$); }
        ;

membs:	  membline		{ mta->Class_UpdateLastPtrs(); }
        | membs membline	{ mta->Class_UpdateLastPtrs(); }
        ;

membline: membdefn			{
            if($1 != NULL) {
	      if((mta->cur_mstate == MTA::pblc) && !($1->HasOption("IGNORE"))
		 && !($1->type->IsConst())) {
		mta->cur_class->members.AddUniqNameNew($1);
		if(mta->verbose >= 3)
		  cerr << "M!!: member: " << $1->name << " added to: "
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
		    cerr << "M!!: method: " << $1->name << " added to: "
			 << mta->cur_class->name << "\n"; } } }
	    else {
	      mta->cur_meth = NULL; }
	    mta->meth_stack.Pop(); $$ = NULL; }
        | enumdsub			{
	    mta->cur_class->sub_types.AddUniqNameNew($1);
            $1->SetType(TypeDef::SUBTYPE);
            mta->TypeAdded("enum", &(mta->cur_class->sub_types), $1);
            $1->source_end = mta->line-1;
	    mta->state = MTA::Parse_inclass; $$ = NULL; }
        | typedsub			{
	    mta->cur_class->sub_types.AddUniqNameNew($1);
            $1->SetType(TypeDef::SUBTYPE);
	    mta->state = MTA::Parse_inclass; $$ = NULL; }
        | classdsub term			{ /* todo: not dealing with sub classes yet.. */
	    mta->last_class->sub_types.AddUniqNameNew($1);
            $1->SetType(TypeDef::SUBTYPE);
  	    mta->cur_class = mta->last_class; /* pop back last class.. */
	    mta->state = MTA::Parse_inclass; $$ = NULL; }
        | error				{ $$ = NULL; }
        ;

membdefn:
          basicmemb
        | MP_PUBLIC ':'			{ $$ = NULL; mta->cur_mstate = MTA::pblc; }
        | MP_PRIVATE ':'			{ $$ = NULL; mta->cur_mstate = MTA::prvt; }
        | MP_PROTECTED ':'			{ $$ = NULL; mta->cur_mstate = MTA::prot; }
        | MP_COMMENT	 		{
	    $$ = NULL;
	    if(mta->last_memb != NULL) {
	      SETDESC(mta->last_memb, $1);
	      if(mta->last_memb->HasOption("IGNORE"))
		mta->cur_class->members.RemoveEl(mta->last_memb); }
	    else if(mta->last_meth != NULL) {
	      SETDESC(mta->last_meth, $1);
	      if(mta->last_meth->HasOption("IGNORE")) {
		mta->cur_class->ignore_meths.AddUnique(mta->last_meth->name);
		mta->cur_class->methods.RemoveEl(mta->last_meth);
		mta->last_meth = NULL; }
	      else if((mta->last_meth->opts.size > 0) || (mta->last_meth->lists.size > 0)) {
		mta->cur_class->methods.AddUniqNameNew(mta->last_meth);
		if(mta->verbose >= 3)
		  cerr << "M!!: method: " << mta->last_meth->name << " added to: "
		       << mta->cur_class->name << "\n"; } } }
        ;

basicmemb:
          nostatmemb
        | MP_STATIC nostatmemb		{
	  $$ = $2; if($2 != NULL) $2->is_static = true;
	  else if(mta->cur_memb != NULL) mta->cur_memb->is_static = true; }
        ;

nostatmemb:
          membtype membnames term	{ $$ = $2; }
        | membtype membname MP_EQUALS term { $$ = $2; }
        | membnames term		{ }
        | membtype membname MP_ARRAY term	{
	    $$ = $2;
	    TypeSpace* sp = mta->GetTypeSpace($1);
            int spsz = sp->size;
            TypeDef* nty = $1->GetArrayType_impl(*sp);
	    if(spsz != sp->size) { mta->TypeAdded("array", sp, nty); }
	    $2->type = nty; }
        | membtype membfunp funargs term { $2->type = $1; $$ = $2; }
        ;

membnames:
           membname			{
	     if((mta->cur_mstate == MTA::pblc) && !($1->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew($1);
             mta->memb_stack.Pop(); $$ = NULL; }
        |  membnames ',' membname       {
	     if((mta->cur_mstate == MTA::pblc) && !($3->type->IsConst()))
	       mta->cur_class->members.AddUniqNameNew($3);
             mta->memb_stack.Pop(); $$ = NULL; }
        ;

membname: MP_NAME			{
            $$ = new MemberDef($1); mta->cur_memb = $$; mta->memb_stack.Push($$);
            if(mta->cur_memb_type != NULL) $$->type = mta->cur_memb_type;
            else $$->type = &TA_int; }
        ;

membfunp: '(' '*' MP_NAME ')'	{
	    $$ = new MemberDef($3); mta->cur_memb = $$; mta->memb_stack.Push($$);
	    $$->fun_ptr = 1; }
        ;

methdefn: basicmeth
        | consthsnm constfun fundefn		{ $$ = NULL; mta->thisname = false; }
        | MP_FUNTYPE consthsnm constfun fundefn	{ $$ = NULL; mta->thisname = false; }
        | MP_VIRTUAL consthsnm constfun fundefn	{ $$ = NULL; mta->thisname = false; }
        | consthsnm constfun constrlist fundefn	{ $$ = NULL; mta->thisname = false; }
        | '~' consthsnm constfun fundefn	{ $$ = NULL; mta->thisname = false; }
        | MP_FUNTYPE '~' consthsnm constfun fundefn { $$ = NULL; mta->thisname = false; }
        | MP_VIRTUAL '~' consthsnm constfun fundefn { $$ = NULL; mta->thisname = false; }
        | MP_FRIEND ftype MP_OPERATOR funargs fundefn	{ $$ = NULL; }
        | MP_FRIEND MP_OPERATOR funargs fundefn	{ $$ = NULL; }
        | MP_FRIEND ftype term			{ $$ = NULL; }
        | MP_FRIEND MP_CLASS ftype term		{ $$ = NULL; }
        | MP_FRIEND MP_CLASS MP_NAME term		{ $$ = NULL; }
        | MP_FRIEND ftype methname funargs fundefn 	{
	    $$ = NULL; String tmp = $5;
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef($3->name, TypeDef::FUNCTION,0,0);
              taMisc::types.Add(nt);
	      nt->methods.AddUniqNameNew($3); $3->type = $2;
	      mta->meth_stack.Pop();  $3->fun_argc = $4; $3->arg_types.size = $4;
	      $3->is_static = true; /* consider these to be static functions */
	      SETDESC($3,$5); }
	    else { $$ = NULL; mta->meth_stack.Pop(); } }
        | MP_FRIEND methname funargs fundefn 	{
	    $$ = NULL; String tmp = $4;
	    if(tmp.contains("REG_FUN") && (mta->cur_is_trg)) {
              TypeDef* nt = new TypeDef($2->name, TypeDef::FUNCTION,0,0);
              taMisc::types.Add(nt);
	      nt->methods.AddUniqNameNew($2); $2->type = &TA_int;
	      mta->meth_stack.Pop();  $2->fun_argc = $3; $2->arg_types.size = $3;
	      $2->is_static = true; /* consider these to be static functions */
	      SETDESC($2,$4); }
	    else { $$ = 0; mta->meth_stack.Pop(); } }
        ;

basicmeth:
          nostatmeth
        | MP_STATIC nostatmeth		{ $$ = $2; $2->is_static = true; }
        | MP_VIRTUAL nostatmeth	{ $$ = $2;  if($2 != NULL) $2->is_virtual = true;
	  else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
        | MP_FUNTYPE MP_VIRTUAL nostatmeth	{ $$ = $3;  if($3 != NULL) $3->is_virtual = true;
	    else if(mta->cur_meth) mta->cur_meth->is_virtual = true; }
        ;

nostatmeth:
          membtype mbfundefn fundefn		{ $$ = $2; $2->type = $1; SETDESC($2,$3); }
        | mbfundefn fundefn			{ $1->type = &TA_int; SETDESC($1,$2); }
        | membtype MP_OPERATOR funargs fundefn	{ $$ = NULL; }
        | membtype MP_OPERATOR '(' ')' funargs fundefn { $$ = NULL; }
        | MP_OPERATOR funargs fundefn		{ $$ = NULL; }
        ;

mbfundefn: methname funargs 		{
             $1->fun_argc = $2; $1->arg_types.size = $2; mta->burp_fundefn = false;
	     /* argd should always be less than argc, but scanner might screw up
  	        (in fact it does in certain cases..) (if so, then just reset!) */
	     if($1->fun_argd > $1->fun_argc) $1->fun_argd = -1; }
        ;

methname: MP_NAME			{
            $$ = new MethodDef($1); mta->cur_meth = $$; mta->meth_stack.Push($$); }
        ;

fundefn:  term				{ $$ = ""; }
        | funsubdefn			{ $$ = $1; }
        | funsubdecl term		{ $$ = $1; }
        | MP_COMMENT term		{ $$ = $1; }
        | MP_COMMENT funsubdefn		{ $$ = $1; }
        | MP_COMMENT funsubdecl term	{ $$ = $1; }
        ;

funsubdefn:
          MP_FUNCTION			{ $$ = ""; }
        | MP_CONST MP_FUNCTION		{ $$ = ""; }
        | MP_CONST MP_COMMENT MP_FUNCTION { $$ = $2; }
        ;

funsubdecl:
          MP_EQUALS			{ $$ = ""; }
        | MP_CONST			{ $$ = ""; }
        | MP_CONST MP_EQUALS		{ $$ = ""; }
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
        | subargdefn MP_EQUALS		{
	    if(mta->cur_meth != NULL) {
	      String ad = $2;
	      ad.gsub("(__null)", "NULL"); /* standardize on NULL */
	      while(isspace(ad.firstchar())) ad = ad.after(0); /* remove leading sp */
	      mta->cur_meth->arg_defs.Add(ad);
	      if(mta->cur_meth->fun_argd < 0)
		mta->cur_meth->fun_argd = mta->cur_meth->arg_types.size - 1; } }
	;

subargdefn:
          type				{
	    $$ = 1; String nm = "na";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link($1); mta->cur_meth->arg_names.Add(nm); } }
        | type MP_NAME			{
	    $$ = 1; String nm = $2;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link($1); mta->cur_meth->arg_names.Add(nm); } }
        | type MP_NAME MP_ARRAY		{
	    $$ = 1; String nm = String($2) + "[]";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link($1); mta->cur_meth->arg_names.Add(nm); } }
        | type '(' '*' MP_NAME ')'	funargs	{
	    $$ = 1; String nm = String("(*") + String($4) + ")";
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link($1); mta->cur_meth->arg_names.Add(nm); } }
        | MP_NAME				{
	    $$ = 1; String nm = $1;
	    if(mta->cur_meth != NULL) {
	      mta->cur_meth->arg_types.Link(&TA_int); mta->cur_meth->arg_names.Add(nm); } }
        ;

constrlist: constcoln constitms
        ;

constitms:
          constref
        | MP_COMMENT
        | constitms constref
        | constitms ',' constref
        ;

constref: MP_TYPE MP_FUNCALL
        | MP_NAME MP_FUNCALL
        ;

consthsnm:
          MP_THISNAME '('	{
            mta->thisname = true; mta->constcoln = false; }
        ;

constcoln: ':'			{ mta->constcoln = true; }
        ;

ptrs:	  '*' 			{ $$ = 1; }
        | ptrs '*'		{ $$ = $1 + 1; }
        ;

membtype:  ftype		{ mta->cur_memb_type = $1; }
        ;

ftype:	  type
        | MP_FUNTYPE		{ $$ = &TA_int; }
        | MP_FUNTYPE type	{ $$ = $2; }
        ;

tyname:	  MP_NAME		{ $$ = new TypeDef($1); mta->type_stack.Push($$);
                                  mta->SetSource($$, false); }
        ;

type:	  noreftype
        | noreftype '&'		{
	    TypeSpace* sp = mta->GetTypeSpace($1);
            int spsz = sp->size;
            $$ = $1->GetRefType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("ref", sp, $$); }
	  }
        ;

noreftype:
	  constype
        | constype ptrs		{
	    TypeSpace* sp = mta->GetTypeSpace($1);
            int spsz = sp->size;
            $$ = $1;
 	    for(int i=0; i<$2; i++) {
              $$ = $$->GetPtrType_impl(*sp);
              if(sp->size != spsz) { mta->TypeAdded("ptr", sp, $$); }
            }
	  }
        ;

constype: subtype
        | MP_CONST subtype	{
	    TypeSpace* sp = mta->GetTypeSpace($2);
            int spsz = sp->size;
            $$ = $2->GetConstType_impl(*sp);
	    if(sp->size != spsz) { mta->TypeAdded("const", sp, $$); }
	  }
        ;

subtype:  combtype
        | structkeyword combtype 	{ $$ = $2; $$->SetType(TypeDef::STRUCT);
          $$->ClearType(TypeDef::VOID); }
        // | structkeyword tyname		{ $$ = $2; $$->AssignType(TypeDef::STRUCT); }
        | unionkeyword combtype		{ $$ = $2; $$->SetType(TypeDef::UNION);
            $$->ClearType(TypeDef::VOID); }
        | unionkeyword tyname		{ $$ = $2; $$->AssignType(TypeDef::UNION); }
	| MP_TYPE MP_SCOPER MP_NAME	{
	    TypeDef* td; if((td = $1->sub_types.FindName($3)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    $$ = td; }
	| MP_THISNAME MP_SCOPER MP_NAME	{
	    TypeDef* td; if((td = $1->sub_types.FindName($3)) == NULL) {
	      yyerror("Subtype not found"); YYERROR; }
	    $$ = td; }
	| MP_TYPE MP_SCOPER MP_TYPE	{ $$ = $3; }
	| MP_THISNAME MP_SCOPER MP_TYPE	{ $$ = $3; }
        | MP_SCOPER MP_TYPE		{ $$ = $2; }
        | MP_THISNAME
        | MP_TYPE templopen templargs '>' { /* a template */
            if(!($1->IsTemplate())) {
	      yyerror("Template syntax error"); YYERROR; }
	    /* if(($3->owner != NULL) && ($3->owner->owner != NULL)) */
	    /*   $$ = $1;	/\* don't allow internal types with external templates *\/ */
	    /* else { */
            String nm = $1->GetTemplInstName(mta->cur_templ_pars);
            TypeDef* td;
            int lx_tok;
            if((td = mta->FindName(nm, lx_tok)) == NULL) {
              td = $1->Clone(); td->name = nm;
              td->SetTemplType($1, mta->cur_templ_pars);
              TypeSpace* sp = mta->GetTypeSpace($1);
              $$ = sp->AddUniqNameOld(td);
              if($$ == td) mta->TypeAdded("template instance", sp, $$); }
            else
              $$ = td; }
	| MP_THISNAME templopen templargs '>'	{ /* this template */
            if(!($1->IsTemplate())) {
	      yyerror("Template syntax error"); YYERROR; }
	    $$ = $1; }
        ;

combtype: MP_TYPE
        | combtype MP_TYPE		{
	    String nm = $1->name + "_" + $2->name;
	    TypeDef* nty = new TypeDef((char*)nm);
            nty->AssignType($1->type); // get from first guy
            nty->SetType($2->type);   // add from second
	    TypeSpace* sp = mta->GetTypeSpace($2);
	    $$ = sp->AddUniqNameOld(nty);
	    if($$ == nty) { mta->TypeAdded("combo", sp, $$);
	      nty->size = $2->size; nty->AddParent($1); nty->AddParent($2); }
	    else { mta->TypeNotAdded("combo", sp, $$, nty); delete nty; }
	  }
        ;

templargs:
          templarg
        | templargs ',' templarg	{ $$ = $1; }
        ;


templarg:
          MP_TYPE		{ mta->cur_templ_pars.Link($1); }
        | MP_NAME		{ $$ = new TypeDef($1); mta->cur_templ_pars.Push($$); }
        | MP_NUMBER		{ $$ = new TypeDef((String)$1); mta->cur_templ_pars.Push($$); }
          /* todo: need to add support for arbitrary strings here, which are not just types */
	;

tdname:   MP_NAME
        | MP_TYPE		{ $$ = $1->name; }
        | MP_THISNAME   	{ $$ = $1->name; }
        ;

varname:  MP_NAME
        | MP_NAME MP_ARRAY
        ;

term:	  ';'
        ;

access:   MP_PUBLIC
        | MP_PRIVATE
        | MP_PROTECTED
        ;

structkeyword:
          MP_STRUCT		{ mta->defn_st_line = mta->line-1; }
	;

unionkeyword:
          MP_UNION		{ mta->defn_st_line = mta->line-1; }
	;

classkeyword:
          MP_CLASS		{ mta->defn_st_line = mta->line-1; }
        ;

templatekeyword:
          MP_TEMPLATE		{ mta->defn_st_line = mta->line-1; }
        ;

%%

	/* end of grammar */

void yyerror(const char *s) { 	/* called for yacc syntax error */
  int i;
  if((mta->verbose < 1) && (!mta->cur_is_trg))
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
