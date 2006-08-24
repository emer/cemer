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

/* parser for css language */

%{

#include "css_machine.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "css_builtin.h"
#include "ta_css.h"
#ifdef GUI_IV
#include "css_iv.h"
#endif // GUI_IV
#ifdef GUI_QT
#include "css_qt.h"
#include "css_qtdata.h"
#include "css_qtdialog.h"
#endif

#include <sstream>

#if defined(SUN4) && !defined(__GNUG__) && !defined(SOLARIS)
#include <alloca.h>
#endif

#if defined(SGI) || defined(SGIdebug)
#include <alloca.h>
#endif

void yyerror(char* s);
/* int yyparse(); */
int yylex();

#define Code1(x) 	cssMisc::cur_top->Prog()->Code(x)
#define Code2(x,y) 	cssMisc::cur_top->Prog()->Code(x); cssMisc::cur_top->Prog()->Code(y)
#define Code3(x,y,z) 	cssMisc::cur_top->Prog()->Code(x); cssMisc::cur_top->Prog()->Code(y); cssMisc::cur_top->Prog()->Code(z)
#define Code4(w,x,y,z) 	cssMisc::cur_top->Prog()->Code(w); cssMisc::cur_top->Prog()->Code(x); cssMisc::cur_top->Prog()->Code(y); cssMisc::cur_top->Prog()->Code(z);
#define Code5(w,x,y,z,zz) 	cssMisc::cur_top->Prog()->Code(w); cssMisc::cur_top->Prog()->Code(x); cssMisc::cur_top->Prog()->Code(y); cssMisc::cur_top->Prog()->Code(z); cssMisc::cur_top->Prog()->Code(zz);

%}

/* fifteen expected shift-reduce conflicts.. */
%expect 15

%union {
  cssElPlusIVal el_ival;
  cssElPtr     	el;		/* for coding */
  int		ival;		/* for program indexes (progdx) and other ints */
  char*        	nm;
}

/* pre-processor like things */
%token	<el>	CSS_PP_INCLUDE CSS_PP_DEFINE CSS_PP_UNDEF
%token	<el>	CSS_PP_IFDEF CSS_PP_IFNDEF CSS_PP_ELSE CSS_PP_ENDIF

/* basic primitives */
%token	<el> 	CSS_NUMBER CSS_STRING CSS_VAR CSS_FUN CSS_PTR CSS_PP_DEF
%token  <nm> 	CSS_NAME
%token  <el>    CSS_COMMENT

/* types */
%token 	<el>	CSS_TYPE CSS_PTRTYPE CSS_CLASS CSS_ENUM CSS_SCPTYPE

/* control constructs */
%token 	<el>	CSS_WHILE CSS_DO CSS_IF CSS_ELSE
%token  <el>	CSS_SWITCH CSS_CASE CSS_DEFAULT
%token	<el>	CSS_RETURN CSS_BREAK CSS_CONTINUE CSS_FOR
%token	<el>	CSS_NEW CSS_DELETE

/* commands */
%token  <el> 	CSS_COMMAND CSS_ALIAS CSS_HELP

/* storage */
%token	<el>	CSS_EXTERN CSS_STATIC CSS_CONST

/* access */
%token	<el>	CSS_PRIVATE CSS_PUBLIC CSS_PROTECTED CSS_VIRTUAL
%token	<el>	CSS_INLINE

/* top level objects */
%type   <ival>  command
%type   <ival> 	stmt expr defn

/* generic elements */
%type   <ival>	end term crterm
%type   <el>	name undefname

/* pre-procesor elements */
%type	<el>	pproc ppinclude ppdefine ppifdef ppelse ppendif

/* command elements */
%type	<ival>	cmd_args

/* stmt elements */
%type	<ival>  noifstmt ifstmt do doloop nodostmt
%type 	<ival>	forloop whiloop for_cond for_cond_sc for_incr for_incr_sc for_end_paren
%type   <ival>  cond for while if else bra cond_paren cond_end_paren mbr_bra ket
%type	<ival>	switchblock caseitem switch
%type	<ival>  stmtlist stmtel
%type	<el>	caseexpr

/* defn elements */
%type   <ival>  vardefn vardefin arraydefn enumdefn classdefn fundefn
%type	<ival>	arraydims arraydim
%type	<ival>  tynames tyname
%type 	<el>	fundname methdname

/* expr elements */
%type   <ival>	comb_expr exprlist exprlsel cmd_exprlist cmd_exprlsel argstop memb_expr
%type	<ival>	normfuncall
%type   <el_ival> normfun membfun
%type	<el>    primitive type type_el typeonly typeorscp scopetype
%type	<el>    anycmd
%type	<ival>	ptrs

/* function defn elements */
%type	<el> 	argdefn subargdefn
%type	<ival>	funargs arglist

/* enum defn elements */
%type 	<el> 	enumname enums enumline enumitms

/* class defn elements */
%type   <el>    classinh classpar classtyp inh_mods inh_mod classcmt
%type   <el>    membnms membname membline membs
%type   <el>    membdefn membardimstop membardim methname
%type   <ival>  membardims methdefn


/* primitive operators and precidence */
%left	'{' ','
%right	'=' CSS_ASGN_ADD CSS_ASGN_SUB CSS_ASGN_MULT CSS_ASGN_DIV CSS_ASGN_MOD CSS_ASGN_LSHIFT CSS_ASGN_RSHIFT CSS_ASGN_AND CSS_ASGN_XOR CSS_ASGN_OR
%right	'?' ':'
%left	CSS_OR
%left	CSS_AND
%left   '|'
%left 	'^'
%left	'&'
%left	CSS_GT CSS_GE CSS_LT CSS_LE CSS_EQ CSS_NE
%left	CSS_LSHIFT CSS_RSHIFT
%left	'+' '-'
%left	'*' '/'	'%'
%right  CSS_UNARY CSS_PLUSPLUS CSS_MINMIN
%left	CSS_UNARYMINUS CSS_NOT
%left   CSS_POINTSAT '.' CSS_SCOPER '(' '['

%%
toplev:	/* nothing */
        | toplev '\n' 		{ cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Blank; }
        | toplev pproc crterm	{ cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
        | toplev command crterm	{ cssMisc::cur_top->ResetParseFlags(); return $2; }
        | toplev ket 		{ cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
        | toplev defn 		{ cssMisc::cur_top->Prog()->ResetLasts();
				  cssMisc::cur_top->ResetParseFlags();
				  return cssProg::YY_Ok; }
        | toplev stmt 		{ cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Ok; }
	| toplev error crterm	{ cssMisc::cur_top->ResetParseFlags(); return cssProg::YY_Err; }
	;

term:	  ';'			{ $$ = cssMisc::cur_top->Prog()->size; }
        ;

crterm:	  ';'			{ $$ = cssMisc::cur_top->Prog()->size; }
        | '\n' 			{ $$ = cssMisc::cur_top->Prog()->size; }
        ;

pproc:	  ppinclude
        | ppdefine
        | ppifdef
        | ppelse
        | ppendif
/*        | ppdef */
        ;

ppinclude:
          CSS_PP_INCLUDE CSS_STRING	{
            cssEl* tmp = $2.El();
	    cssMisc::cur_top->SetInclude((const char*)*tmp);
	    cssEl::Done($2.El()); }
        ;

ppdefine: CSS_PP_DEFINE CSS_NAME	{
            cssMisc::Defines.Push(new cssDef(0, $2)); }
        | CSS_PP_DEFINE CSS_PP_DEF 	/* maybe flag the redefinition? */
        | CSS_PP_UNDEF CSS_PP_DEF	{ cssMisc::Defines.Remove($2.El()); }
        ;

ppifdef:  CSS_PP_IFDEF CSS_NAME		{ /* if its a name, its not defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; } /* return to escape parsing problems */
        | CSS_PP_IFDEF CSS_PP_DEF
        | CSS_PP_IFNDEF CSS_PP_DEF	{ /* if its a def, its defined.. */
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog());
	    return cssProg::YY_Ok; } /* return to escape parsing problems */
        | CSS_PP_IFNDEF CSS_NAME
        ;

ppelse:	  CSS_PP_ELSE		{ /* if you see it, it applies to u*/
            cssDef::Skip_To_Endif(cssMisc::cur_top->Prog()); }
        ;

ppendif:  CSS_PP_ENDIF
        ;

/* not implemented yet..
ppdef:    CSS_PP_DEF		{
            $1.El()->Do(cssMisc::cur_top->Prog());	}
        | CSS_PP_DEF exprlist	{
            $1.El()->Do(cssMisc::cur_top->Prog());	}
        ;
*/

command:  CSS_COMMAND cmd_args		{
            Code1($1); $$ = cssProg::YY_Ok; }
        | CSS_ALIAS anycmd name 	{
	    $$ = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssRef($2));
	    ($1.El())->Do(cssMisc::cur_top->Prog()); }
        | CSS_ALIAS anycmd anycmd	{
	    $$ = cssProg::YY_NoSrc; }
        | CSS_HELP {
	    Code1($1); $$ = cssProg::YY_Ok; }
        | CSS_HELP argstop anycmd {
	    $$ = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push($3.El());
	    ($1.El())->Do(cssMisc::cur_top->Prog()); }
        | CSS_HELP argstop CSS_FUN {
	    $$ = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push($3.El());
	    ($1.El())->Do(cssMisc::cur_top->Prog()); }
        | CSS_HELP argstop CSS_ALIAS {
	    $$ = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push($3.El());
	    ($1.El())->Do(cssMisc::cur_top->Prog()); }
        | CSS_HELP argstop CSS_HELP {
	    $$ = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push($3.El());
	    ($1.El())->Do(cssMisc::cur_top->Prog()); }
        | CSS_HELP argstop CSS_PTRTYPE {
	    $$ = cssProg::YY_NoSrc;
	    cssMisc::cur_top->Prog()->Stack()->Push($3.El());
	    ($1.El())->Do(cssMisc::cur_top->Prog()); }
        ;

cmd_args: /* nothing */			{ $$ = 0; }
        | argstop cmd_exprlist
        | '(' argstop cmd_exprlist ')'	{ $$ = $2; }
        ;

/* defn's are skipped over when running, so are replaced by jumps */
defn:	  vardefn		/* define variable(s) */
        | vardefin term		/* define and initialize */
        | arraydefn
        | enumdefn
        | classdefn
        | fundefn
        ;

vardefn:  type tynames end term		{
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->insts[$3-1]->SetLine($3);
	    if($2 < 0) $$ = $3-1; /* if no coding, its the end, else not */
	    else $$ = $2; }
        ;

vardefin: type name '=' argstop expr end 	{
 	    ($1.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    /* constants get dealt with differently */
	    if(tmp->ptr.ptr == (void*)&(cssMisc::Constants)) {
	      /* code in ConstExpr space the initializer */
	      cssMisc::ConstExprTop->Prog()->UnCode();	/* undo the end jump coding */
	      Code3(tmp->ptr, cssBI::init_asgn, cssBI::pop);
	      cssMisc::ConstExprTop->Run(); /* run the const expr assigning val */
	      cssMisc::CodeTop(); /* revert to coding using top */
	      $$ = Code1(cssMisc::cur_top->Prog()->size+1); /* need to have an rval */
	    }
	    else {
	      cssMisc::cur_top->Prog()->UnCode();	/* undo the end jump coding */
	      $$ = Code3(tmp->ptr, cssBI::init_asgn, cssBI::pop);
	    }
	    cssEl::Done(tmp); }

        | undefname '=' argstop expr end term {	/* no type information, get from last expr */
	    cssMisc::cur_top->Prog()->UnCode();	/* undo the end jump coding */
	    css_progdx actln = $5-2;
	    if(cssMisc::cur_top->Prog()->insts[actln]->IsJump()) { /* if jmp, get before */
	      actln = $3+1;	/* go for the first thing if the last is no good */
	    }
	    cssEl* extyp = cssMisc::cur_top->Prog()->insts[actln]->inst.El();
 	    extyp->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    $$ = Code3(tmp->ptr, cssBI::init_asgn, cssBI::pop);
	    if(!cssMisc::cur_top->AmCmdProg()) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(),
			       "Warning: created implicit variable:",
			       tmp->ptr.El()->name,
			       "of type: ", extyp->GetTypeName()); }
	    cssEl::Done(tmp); }
        ;

          /* array type */
arraydefn:
          type name argstop arraydims end term {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->UnCode();	/* undo the end jump coding */
	    cssMisc::cur_top->Prog()->Stack()->Push($1.El()); /* type is an arg */
	    cssMisc::cur_top->Prog()->Stack()->Push(new cssInt($4)); /* num of dims */
	    cssMisc::VoidArray.tmp_str = ($1.El())->tmp_str;
	    cssMisc::VoidArray.MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    $$ = Code3(tmp->ptr, cssBI::array_alloc, cssBI::pop);
	    cssEl::Done(tmp); }
        ;

arraydims:
          arraydim			{ $$ = 1; }
        | arraydims arraydim		{ $$ = $1 + 1; }
        ;

arraydim:
          '[' expr ']'			{ $$ = $2; }
        ;

enumdefn: enumname '{' enums '}' end term {
            cssMisc::cur_top->Prog()->insts[$5-1]->SetLine($5); $$ = $5-1;
	    if(cssMisc::cur_enum->name.before(5) == "enum_") /* don't keep anon enums around */
	      delete cssMisc::cur_enum;
	    cssMisc::cur_enum = NULL; }
        ;

enumname: CSS_ENUM name			{
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_enum = new cssEnumType((const char*)*nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum);
	    cssEl::Done(nm); }
        | CSS_ENUM CSS_TYPE		{ /* redefining */
	    if($2.El()->GetType() != cssEl::T_EnumType) {
	      yyerror("Attempt to redefine non-enum type as an enum");
	      return cssProg::YY_Err; }
   	    cssMisc::cur_enum = (cssEnumType*)$2.El();
 	    cssMisc::cur_enum->enums->Reset(); /* prepare for new enums.. */
	    cssMisc::cur_enum->enum_cnt = 0; }
	| CSS_ENUM			{ /* anonymous  */
	    String nm = "anonenum_"; nm += String(cssMisc::anon_type_cnt++); nm += "_";
            cssMisc::cur_enum = new cssEnumType((const char*)nm);
	    if(cssMisc::cur_class != NULL) cssMisc::cur_class->types->Push(cssMisc::cur_enum);
	    /* todo: global keyword?? else cssMisc::TypesSpace.Push(cssMisc::cur_enum); */
	    else cssMisc::cur_top->types.Push(cssMisc::cur_enum); }
        ;

enums:    enumline
        | enums enumline
        ;

enumline: enumitms
        | enumitms ','
        | error				{ $$ = cssMisc::VoidElPtr; }
        ;

enumitms: name			{
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssSpace* en_own;
	    if(cssMisc::cur_class != NULL) en_own = cssMisc::cur_class->types;
	    else en_own = &(cssMisc::Enums);
	    cssElPtr itm_ptr = en_own->FindName((const char*)*nm); 	cssEnum* itm;
	    if(itm_ptr != 0) { /* redef */
	      itm = (cssEnum*)itm_ptr.El();
	      itm->val = cssMisc::cur_enum->enum_cnt;
	      itm->SetEnumType(cssMisc::cur_enum);
	    } else {
	      itm = new cssEnum(cssMisc::cur_enum, cssMisc::cur_enum->enum_cnt,
				(const char*)*nm);
	      en_own->Push(itm);
	    }
	    cssMisc::cur_enum->enum_cnt = itm->val + 1;
	    cssMisc::cur_enum->enums->Push(itm); cssEl::Done(nm); }
        | name '=' CSS_NUMBER		{
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssSpace* en_own;
	    if(cssMisc::cur_class != NULL) en_own = cssMisc::cur_class->types;
	    else en_own = &(cssMisc::Enums);
	    cssElPtr itm_ptr = en_own->FindName((const char*)*nm);	cssEnum* itm;
	    if(itm_ptr != 0) { /* redef */
	      itm = (cssEnum*)itm_ptr.El();
	      itm->val = (int)*($3.El());
	      itm->SetEnumType(cssMisc::cur_enum);
	    }
	    else {
	      itm = new cssEnum(cssMisc::cur_enum, (int)*($3.El()), (const char*)*nm);
	      en_own->Push(itm);
	    }
	    cssMisc::cur_enum->enum_cnt = itm->val + 1;
	    cssMisc::cur_enum->enums->Push(itm); cssEl::Done(nm); }
        ;

classdefn:
          CSS_CLASS classhead '{' classcmt membs '}' end term {
            cssMisc::cur_top->Prog()->insts[$7-1]->SetLine($7); $$ = $7-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, $4);
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
        | CSS_CLASS classhead '{' classcmt '}' end term {
            cssMisc::cur_top->Prog()->insts[$6-1]->SetLine($6); $$ = $6-1;
            cssMisc::cur_class->GetComments(cssMisc::cur_class, $4);
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
        | CSS_CLASS classfwd end term {
            cssMisc::cur_top->Prog()->insts[$3-1]->SetLine($3); $$ = $3-1;
            cssMisc::cur_class = NULL; cssMisc::cur_method = NULL; }
        ;

classhead:
          classnm
        | classnm ':' classinh
        ;

classnm:  name			{
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssMisc::cur_class = new cssClassType((const char*)*nm);
	    cssMisc::cur_class->last_top = cssMisc::cur_top;
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
	    cssEl::Done(nm); }
        | CSS_TYPE		{ /* redefining */
	    if($1.El()->GetType() != cssEl::T_ClassType) {
	      yyerror("Attempt to redefine non-class type as a class");
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_class = (cssClassType*)$1.El();
#ifdef GUI_IV
	    cssivSession::CancelClassEdits(cssMisc::cur_class);
#endif // GUI_IV
	    if(cssMisc::cur_class->last_top != cssMisc::cur_top) {
	      cssMisc::cur_class->last_top = cssMisc::cur_top;
	      cssMisc::cur_class->multi_space = true;
	    }
            cssMisc::cur_class->methods->Reset();
	    cssMisc::cur_class->members->Reset();
            cssMisc::cur_class->member_desc.Reset();
            cssMisc::cur_class->member_opts.Reset();
            cssMisc::cur_class->types->Reset();
	    cssMisc::cur_class->parents->Reset();
	    cssMisc::cur_class->AddBuiltins();  }
        ;

classfwd: name                  {
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_class = new cssClassType((const char*)*nm);
	    /*	todo: keyword for global??    cssMisc::TypesSpace.Push(cssMisc::cur_class); */
	    cssMisc::cur_top->types.Push(cssMisc::cur_class);
            cssEl::Done(nm); }
        | CSS_TYPE              {
            if($1.El()->GetType() != cssEl::T_ClassType) {
              yyerror("Attempt to redefine non-class type as a class");
              return cssProg::YY_Err;
            }
            cssMisc::cur_class = (cssClassType*)$1.El(); }
        ;

classinh: classpar
        | classinh ',' classpar
        ;

classpar: classtyp			{
            if($1.El()->GetType() != cssEl::T_ClassType) {
	      yyerror("Cannot inherit from non-class type");
	      /* cssMisc::TypesSpace.Remove(cssMisc::cur_class); */
	      cssMisc::cur_top->types.Remove(cssMisc::cur_class);
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_class->AddParent((cssClassType*)($1.El())); }
        ;

classtyp: type_el
        | inh_mods type_el		{ $$ = $2; }
        ;

inh_mods: inh_mod
        | inh_mods inh_mod
        ;

inh_mod:  CSS_PUBLIC
        | CSS_VIRTUAL
        | CSS_PRIVATE
        ;

membs:	  membline
        | membs membline
        ;

membline: membdefn
        | methdefn			{ $$ = cssMisc::VoidElPtr; }
        | enumdefn classcmt             { $$ = cssMisc::VoidElPtr; }
        | CSS_PUBLIC ':'
        | CSS_PRIVATE ':'
        | CSS_PROTECTED ':'
        | error				{ $$ = cssMisc::VoidElPtr; }
        ;

membdefn: type name term classcmt       {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
            cssMisc::parsing_membdefn = true;
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssClassMember* mbr = new cssClassMember($1.El(), (const char*)*nm);
            cssMisc::cur_class->members->Push(mbr);
            cssMisc::cur_class->GetComments(mbr, $4);
            cssMisc::parsing_membdefn = false;
            cssEl::Done(nm); }
        | type name membardimstop membardims term classcmt {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
            cssMisc::parsing_membdefn = true;
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt($4)); /* num of dims */
            cssMisc::cur_top->Prog()->Stack()->Push($1.El()); /* type is an arg */
            cssMisc::VoidArrayType.MakeTempToken(cssMisc::cur_top->Prog());
            cssArrayType* tmp = (cssArrayType*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    cssClassMember* mbr = new cssClassMember(tmp, tmp->name);
            cssMisc::cur_class->members->Push(mbr);
            cssMisc::cur_class->GetComments(mbr, $6);
            cssMisc::parsing_membdefn = false; }
        ;

membardimstop:  /* nothing */ {
            /* swap places with void marker */
            cssString* nm = (cssString*)cssMisc::cur_top->Prog()->Stack()->Pop();
            cssMisc::cur_top->Prog()->Stack()->Push(&cssMisc::Void);
            cssMisc::cur_top->Prog()->Stack()->Push(nm); }
        ;

membardims:
          membardim                  { $$ = 1; }
        | membardims membardim    { $$ = $1 + 1; }
        ;

membardim:
        '[' CSS_NUMBER ']' {
            cssMisc::cur_top->Prog()->Stack()->Push(new cssInt((int)*$2.El())); }
        ;

methdefn: methname funargs term classcmt {       /* pre-declare function */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)$1.El();
            cssMisc::cur_class->GetComments(fun, $4);
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr, true); } /* true = in decl */
        | CSS_TYPE funargs term classcmt {       /* constructor */
	    if($1.El() != cssMisc::cur_class) {
	      yyerror("constructor-like method does not match current class type");
	      return cssProg::YY_Err; }
	    cssElPtr fun_ptr = cssMisc::cur_class->methods->FindName((const char*)cssMisc::cur_class->name);
	    cssMbrScriptFun* fun;
	    if(fun_ptr != 0)
	      fun = (cssMbrScriptFun*)fun_ptr.El();
	    else {
	      fun = new cssMbrScriptFun
		((const char*)cssMisc::cur_class->name, cssMisc::cur_class);
	      cssMisc::cur_class->methods->Push(fun);
	    }
	    fun->is_tor = true;
	    fun->argc = $2 + 1;	      	fun->GetArgDefs();
            cssMisc::cur_class->GetComments(fun, $4); }
        | '~' CSS_TYPE funargs term classcmt {       /* destructor */
	    if($2.El() != cssMisc::cur_class) {
	      yyerror("destructor-like method does not match current class type");
	      return cssProg::YY_Err; }
	    String nm = "~";
	    nm += cssMisc::cur_class->name;
	    cssElPtr fun_ptr = cssMisc::cur_class->methods->FindName((const char*)nm);
	    cssMbrScriptFun* fun;
	    if(fun_ptr != 0)
	      fun = (cssMbrScriptFun*)fun_ptr.El();
	    else {
	      fun = new cssMbrScriptFun(nm, cssMisc::cur_class);
	      cssMisc::cur_class->methods->Push(fun);
	    }
	    fun->is_tor = true;
	    fun->argc = $3 + 1;		fun->GetArgDefs();
            cssMisc::cur_class->GetComments(fun, $5);}
        | CSS_VIRTUAL '~' CSS_TYPE funargs term classcmt {       /* destructor */
	    if($3.El() != cssMisc::cur_class) {
	      yyerror("destructor-like method does not match current class type");
	      return cssProg::YY_Err; }
	    String nm = "~";
	    nm += cssMisc::cur_class->name;
	    cssElPtr fun_ptr = cssMisc::cur_class->methods->FindName((const char*)nm);
	    cssMbrScriptFun* fun;
	    if(fun_ptr != 0)
	      fun = (cssMbrScriptFun*)fun_ptr.El();
	    else {
	      fun = new cssMbrScriptFun(nm, cssMisc::cur_class);
	      cssMisc::cur_class->methods->Push(fun);
	    }
	    fun->is_tor = true;
	    fun->is_virtual = true;
	    fun->argc = $4 + 1;		fun->GetArgDefs();
            cssMisc::cur_class->GetComments(fun, $6);}
        ;

methname: type name 			{
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssEl* nm = cssMisc::cur_top->Prog()->Stack()->Pop();  /* get rid of name */
	    cssElPtr fun_ptr = cssMisc::cur_class->methods->FindName((const char*)*nm);
	    cssMbrScriptFun* fun;
	    if(fun_ptr != 0) {
	      fun = (cssMbrScriptFun*)fun_ptr.El();
	      $$ = fun_ptr;
	    }
	    else {
	      fun = new cssMbrScriptFun((const char*)*nm, cssMisc::cur_class);
	      $$ = cssMisc::cur_class->methods->Push(fun);
	    }
	    if($1.El()->tmp_str == "virtual") fun->is_virtual = true;
	    fun->SetRetvType($1.El()); /* preserve type info for later if nec */
            cssEl::Done(nm); }
        ;

classcmt: /* nothing */                 { $$.Reset(); }
        | CSS_COMMENT                   { $$ = $1; }
        | classcmt CSS_COMMENT          { $$ = $1; }
        ;

fundefn:  fundname funargs end term	{	/* pre-declare function */
            cssMisc::cur_top->Prog()->insts[$3-1]->SetLine($3);	$$ = $3-1;
	    cssScriptFun* fun = (cssScriptFun*)$1.El();
	    fun->argc = $2; fun->GetArgDefs(); }

          /* define for the first time (bra creates a cssCodeBlock) */
        | fundname funargs end bra	{
	    cssScriptFun* fun = (cssScriptFun*)$1.El();
	    cssMisc::cur_top->Prog()->insts[$3-1]->SetLine($4+1); $$ = $3-1;
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    cssCodeBlock* bra_blk = (cssCodeBlock*)(cssMisc::cur_top->Prog()->insts[$4]->inst.El());
	    cssMisc::cur_top->Prog()->insts[$4]->SetInst($1); /* replace bra_blk with fun */
	    cssMisc::cur_top->RemoveVar(bra_blk);	/* get rid of the bra_blk */
	    cssMisc::cur_top->SetPush(fun->fun); /* this is the one we want to push */
	    fun->Define(cssMisc::ConstExpr); } /* define vars */

          /* redeclaring pre-declared */
        | type CSS_FUN funargs term {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    $$ = cssMisc::cur_top->Prog()->size; }

          /* define pre-declared */
        | type CSS_FUN funargs end bra	{
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::cur_top->Prog()->insts[$4-1]->SetLine($5+1);	$$ = $4-1;
	    cssScriptFun* fun = (cssScriptFun*)$2.El();
	    fun->SetRetvType($1.El());
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    cssCodeBlock* bra_blk = (cssCodeBlock*)(cssMisc::cur_top->Prog()->insts[$5]->inst.El());
	    cssMisc::cur_top->Prog()->insts[$5]->SetInst($2); /* replace bra with existing one */
	    cssMisc::cur_top->RemoveVar(bra_blk);	/* get rid of the bra_blk */
	    fun->fun->Reset();	/* reset it for new defn */
	    cssMisc::cur_top->SetPush(fun->fun); /* push this one */
	    fun->Define(cssMisc::ConstExpr); } /* define vars */

          /* define mbrfun (method) */
        | methdname funargs end mbr_bra	{
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)$1.El();
	    cssClassType* cls = cssMisc::cur_class;
	    cssMisc::ConstExpr->Stack()->Push(new cssString(cssRetv_Name)); /* the return val */
	    fun->retv_type->MakeToken(cssMisc::ConstExpr); /* create return val w/ name */
	    fun->Define(cssMisc::ConstExpr);
	    cssMisc::cur_method = fun; /* this is now the current method */
	    cssElPtr fun_ptr;  fun_ptr.SetNVirtMethod(cls, cls->methods->GetIndex(fun));
	    css_progdx nxt_ln = Code1(fun_ptr); /* code it so it shows up in a listing.. */
	    cssMisc::cur_top->SetPush(fun->fun); /* put it on the stack.. */
	    cssMisc::cur_top->Prog()->insts[$3-1]->SetLine(nxt_ln+1); $$ = $3-1; }
        ;

fundname:  type name 			{
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssEl* nm = cssMisc::cur_top->Prog()->Stack()->Pop();  /* get rid of name */
	    cssScriptFun* fun = new cssScriptFun((const char*)*nm);
	    fun->SetRetvType($1.El()); /* preserve type info for later if nec */
	    if($1.El()->tmp_str == "extern") $$ = cssMisc::Externs.PushUniqNameOld(fun);
	    else $$ = cssMisc::cur_top->AddStatic(fun);
            cssEl::Done(nm); }
        ;

methdname: type scopetype name	{
            cssMisc::cur_scope = NULL;
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if(($2.El())->GetType() != cssEl::T_ClassType) {
	      yyerror("attempt to define member function in non-class type");
	      return cssProg::YY_Err; }
	    cssClassType* cls = (cssClassType*)$2.El();
	    cssMisc::cur_class = cls; /* this is now the current class */
	    cssEl* nm = cssMisc::cur_top->Prog()->Stack()->Pop();  /* get rid of name */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMethodFmName((const char*)*nm);
	    if(fun == &cssMisc::Void) {
	      yyerror("member function not declared in class type");
	      return cssProg::YY_Err; }
            if(fun->GetType() == cssEl::T_MbrCFun) {
              yyerror("member function is hardcoded");
              return cssProg::YY_Err; }
	    if((fun->retv_type != $1.El())
	       && !(fun->retv_type->IsRef() || fun->retv_type->GetType() == cssEl::T_Ptr))
	      cssMisc::Warning(NULL, "return type does not match that of declaration");
	    fun->SetRetvType($1.El());
	    $$.SetDirect(fun);  cssEl::Done(nm); }

          /* define constructor (shows up as a scoped type object) */
        | scopetype CSS_TYPE	 		{
            cssMisc::cur_scope = NULL;
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if(($1.El())->GetType() != cssEl::T_ClassType) {
	      yyerror("attempt to define constructor member function in non-class type");
	      return cssProg::YY_Err; }
	    cssClassType* cls = (cssClassType*)$1.El();
	    cssMisc::cur_class = cls; /* this is now the current class */
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMethodFmName((const char*)cls->name);
	    if(fun == &cssMisc::Void) {
	      yyerror("constructor member function not declared in class type");
	      return cssProg::YY_Err; }
            if(fun->GetType() == cssEl::T_MbrCFun) {
              yyerror("member function is hardcoded");
              return cssProg::YY_Err; }
	    $$.SetDirect(fun); }

          /* define destructor */
        | scopetype '~' CSS_TYPE 		{
            cssMisc::cur_scope = NULL;
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    if(($1.El())->GetType() != cssEl::T_ClassType) {
	      yyerror("attempt to define destructor member function in non-class type");
	      return cssProg::YY_Err; }
	    cssClassType* cls = (cssClassType*)$1.El();
	    cssMisc::cur_class = cls; /* this is now the current class */
	    String nm = "~"; nm += cls->name;
	    cssMbrScriptFun* fun = (cssMbrScriptFun*)cls->GetMethodFmName((const char*)nm);
	    if(fun == &cssMisc::Void) {
	      yyerror("destructor member function not declared in class type");
              return cssProg::YY_Err; }
            if(fun->GetType() == cssEl::T_MbrCFun) {
              yyerror("member function is hardcoded");
	      return cssProg::YY_Err; }
	    $$.SetDirect(fun); }
        ;

funargs:  '(' ')'			{ $$ = 0; }
        | '(' arglist ')'		{ $$ = $2; }
        | '(' ')' CSS_CONST		{ $$ = 0; }
        | '(' arglist ')' CSS_CONST	{ $$ = $2; }
        ;

arglist:  argdefn			{ $$ = 1; }
        | arglist ',' argdefn		{ $$ = $1 + 1; }
        ;

argdefn:  subargdefn 			{ cssMisc::parsing_args = false; }
        | subargdefn '=' primitive 	{
	    cssMisc::parsing_args = false;
	    cssMisc::default_args.Push($3.El()); }
        ;

subargdefn:
          type CSS_NAME	{
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    $$ = cssMisc::ConstExpr->Stack()->Push(new cssString($2));
	    ($1.El())->MakeToken(cssMisc::ConstExpr); }
        | type CSS_VAR	{
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    cssMisc::parsing_args = true;
	    $$ = cssMisc::ConstExpr->Stack()->Push(new cssString($2.El()->GetName()));
	    ($1.El())->MakeToken(cssMisc::ConstExpr); }
        ;

tynames:  tyname
        | tynames ',' tyname		{
	    if(($1 < 0) && ($3 < 0)) $$ = -1;
	    else if(($1 >= 0) && ($3 < 0)) $$ = $1;
	    else if(($1 < 0) && ($3 >= 0)) $$ = $3;
	    else $$ = -1; }
        ;

tyname:	  name 				{
            (cssMisc::cur_type.El())->MakeToken(cssMisc::cur_top->Prog());
	    cssRef* tmp = (cssRef*)cssMisc::cur_top->Prog()->Stack()->Pop();
	    if(tmp->ptr.El()->GetType() == cssEl::T_Class) {
	      $$ = Code2(tmp->ptr, cssBI::constr); }
	    else $$ = -1;
	    cssEl::Done(tmp); }
        ;

type:	  type_el		{ $1.El()->tmp_str = ""; }
        | CSS_EXTERN type_el	{ $2.El()->tmp_str = "extern"; $$ = $2; }
        | CSS_STATIC type_el	{ $2.El()->tmp_str = "static"; $$ = $2; }
        | CSS_CONST type_el	{ /* use const expr prog to code stuff */
	  $2.El()->tmp_str = "const"; $$ = $2; cssMisc::CodeConstExpr(); }
        | CSS_VIRTUAL type_el	{ $2.El()->tmp_str = "virtual"; $$ = $2; }
        | CSS_INLINE type_el	{ $2.El()->tmp_str = ""; $$ = $2; }
        | CSS_INLINE CSS_VIRTUAL type_el { $3.El()->tmp_str = "virtual"; $$ = $3; }
        | CSS_VIRTUAL CSS_INLINE type_el { $3.El()->tmp_str = "virtual"; $$ = $3; }
        ;

type_el:  typeorscp				{ $$ = $1; cssMisc::cur_type = $$; }
        | typeorscp ptrs			{
	  if(($1.El()->GetParse() != CSS_PTR) || ($2 >= 2)) {
	    $$ = cssMisc::cur_top->AddPtrType($1.El()); cssMisc::cur_type = $$;
	  }
	  else {
	    $$ = $1; cssMisc::cur_type = $$; } }
        | typeorscp '&' %prec CSS_UNARY 	{
	  $$ = cssMisc::cur_top->AddRefType($1.El()); cssMisc::cur_type = $$; }
        | typeorscp ptrs '&' %prec CSS_UNARY	{
	  if(($1.El()->GetParse() != CSS_PTR) || ($2 >= 2)) {
            cssElPtr npt = cssMisc::cur_top->AddPtrType($1.El());
            $$ = cssMisc::cur_top->AddRefType(npt.El()); cssMisc::cur_type = $$; }
	  else {
	    $$ = cssMisc::cur_top->AddRefType($1.El()); cssMisc::cur_type = $$; } }
        ;

typeorscp: CSS_TYPE
        |  CSS_PTRTYPE
	|  scopetype CSS_SCPTYPE {
          cssEl* itm;
          if($1.El()->GetParse() != CSS_PTR)
	    itm = $1.El()->GetScoped((const char*)$2.El()->name);
	  else
	    itm = $2.El();
	  if(itm != &cssMisc::Void)  $$.SetDirect(itm);
	  else			$$.Reset();
	  cssMisc::cur_type = $$; cssMisc::cur_scope = NULL; }
        ;

scopetype: CSS_TYPE CSS_SCOPER			{
          if(($1.El()->GetType() != cssEl::T_ClassType) && ($1.El()->GetType() != cssEl::T_EnumType)) {
	     yyerror("scoping of non-class or enum type");
	     return cssProg::YY_Err; }
           cssMisc::cur_scope = $1.El(); }
        | CSS_PTRTYPE CSS_SCOPER			{
           cssMisc::cur_scope = $1.El(); }
        ;

typeonly: CSS_TYPE
        | CSS_PTRTYPE
        ;

ptrs:	  '*' %prec CSS_UNARY			{ $$ = 1; }
        | ptrs '*' %prec CSS_UNARY		{ $$ = $1 + 1; }
        ;

stmt: 	  noifstmt		{ cssMisc::cur_top->Prog()->lastif = -1; }
        | ifstmt
        ;

noifstmt: nodostmt
        | doloop
        ;

nodostmt: expr term			{ Code1(cssBI::pop); }
        | forloop
        | whiloop
        | CSS_RETURN argstop term	{ Code1($1); $$ = $2; }
        | CSS_RETURN argstop expr term	{ Code1($1); $$ = $2; }
        | CSS_BREAK term		{ $$ = Code1($1); }
        | CSS_CONTINUE term		{ $$ = Code1($1); }
        | switchblock
        | caseitem
        | bra
        ;

caseitem: CSS_CASE caseexpr ':'		{
	    $$ = cssMisc::cur_top->Prog()->size; // next instr is 'it'
  	    if(cssMisc::cur_top->Prog()->name != cssSwitchBlock_Name) {
	      yyerror("case statement not in a switch block");
	      return cssProg::YY_Err;
	    }
	    cssElPtr aryptr = cssMisc::cur_top->Prog()->literals.FindName(cssSwitchJump_Name);
	    cssArray* val_ary = (cssArray*)aryptr.El();
	    val_ary->items->Push(new cssInt($$, (const char*)*($2.El()))); }
        | CSS_DEFAULT ':'		{
	    $$ = cssMisc::cur_top->Prog()->size; // next instr is 'it'
  	    if(cssMisc::cur_top->Prog()->name != cssSwitchBlock_Name) {
	      yyerror("case statement not in a switch block");
	      return cssProg::YY_Err;
	    }
	    cssElPtr aryptr = cssMisc::cur_top->Prog()->literals.FindName(cssSwitchJump_Name);
	    cssArray* val_ary = (cssArray*)aryptr.El();
	    val_ary->items->Push(new cssInt($$, cssSwitchDefault_Name)); }
        ;

caseexpr:
          CSS_NUMBER
        | CSS_STRING
	| CSS_VAR
	;

switchblock:            /* switch is like a little function with one arg */
          switch '(' argstop expr ')' '{' 	{
            $$ = $1;
	    /* value to switch on is already on stack */
            cssCodeBlock* jmp_blk = (cssCodeBlock*)(cssMisc::cur_top->Prog()->owner_blk);
	    /* make the jump-table address array: ints whose name is val, val is adr */
	    jmp_blk->code->Stack()->Push(new cssString(cssSwitchJump_Name));
	    jmp_blk->code->Stack()->Push(new cssInt(1)); /* type of ary element */
	    jmp_blk->code->Stack()->Push(new cssInt(1)); /* number of dimensions */
  	    cssMisc::VoidArray.tmp_str = "literal";
	    cssMisc::VoidArray.MakeToken(jmp_blk->code); /* make the array */
	    cssMisc::VoidArray.tmp_str = "";
	    cssRef* aryptr = (cssRef*) jmp_blk->code->Stack()->Pop();
	    ((cssArray*)aryptr->ptr.El())->items->DelPop(); /* get rid of first el */
	    Code1(cssBI::switch_jump); /* this gets expr as arg! */
	    cssEl::Done(aryptr); } /* switch jump is first instr */
        ;

switch:   CSS_SWITCH 			{ 
            cssCodeBlock* blk = new cssCodeBlock(cssSwitchBlock_Name);
	    blk->loop_type = cssCodeBlock::SWITCH;
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    $$ = Code1(blk); cssMisc::cur_top->Push(blk->code); }
        ;

          /* only an expr condition */
forloop: for '(' for_cond for_incr for_end_paren stmt {
	   cssProg* cp = cssMisc::cur_top->Prog();
	   if(cp->owner_blk == NULL) {
	     yyerror("for loop current prog should have owner_blk, doesnt!");
	   }
	   cssMisc::cur_top->Pop(); /* pop the for_incr block */
	   Code1(cssBI::pop);	/* and code for getting rid of cond val */
	   cp = cssMisc::cur_top->Prog(); /* current guy */
	   cssInst* for_incr = cp->insts[$4];
	   cssInst* for_loop_stmt = cp->insts[$5];
	   /* swap */
	   cp->insts[$4] = for_loop_stmt;
	   cp->insts[$5] = for_incr;
	   cssMisc::cur_top->Pop(); /* pop the whole for loop! */
	   $$ = $1; }
         | for '(' stmtlist for_cond for_incr for_end_paren stmt {
	   cssProg* cp = cssMisc::cur_top->Prog();
	   if(cp->owner_blk == NULL) {
	     yyerror("for loop current prog should have owner_blk, doesnt!");
	   }
	   cssMisc::cur_top->Pop(); /* pop the for_incr block */
	   Code1(cssBI::pop);	/* and code for getting rid of cond val */
	   /* swap the order of these: $6 = for_end_paren = stmt block,  $5 = for_incr = incr block */
	   cp = cssMisc::cur_top->Prog(); /* current guy */
	   cssInst* for_incr = cp->insts[$5];
	   cssInst* for_loop_stmt = cp->insts[$6];
	   /* swap */
	   cp->insts[$5] = for_loop_stmt;
	   cp->insts[$6] = for_incr;
	   /* check if stmt is a new block: if so, then don't pop this guy */
	   
	   cssMisc::cur_top->Pop(); /* pop the whole for loop! */
	   $$ = $1; }
        
        ;

/* todo: manage missing things in these sub-cases, not above */

for:   	  CSS_FOR		{ /* for loop contained within own block */
            cssCodeBlock* blk = new cssCodeBlock(cssForLoop_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    $$ = Code1(blk); cssMisc::cur_top->Push(blk->code); }
        ;

for_cond: 
            for_cond_sc
          | for_cond_sc expr
        ;

for_cond_sc:	  ';'			{
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    $$ = Code1(blk); cssMisc::cur_top->Push(blk->code); }
        ;

for_incr:   for_incr_sc
          | for_incr_sc stmtlist
        ;

for_incr_sc: ';' {
              cssMisc::cur_top->Pop(); /* get rid of cond, push incr */
	      cssMisc::cur_top->ResetParseFlags();
	      cssCodeBlock* blk = new cssCodeBlock(cssForIncr_Name);
	      blk->owner_prog = cssMisc::cur_top->Prog();
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_back = 3; /* go back 3 to the cond */
	      cssMisc::cur_top->AddStatic(blk);
	      $$ = Code1(blk); cssMisc::cur_top->Push(blk->code); }
        ;

for_end_paren: ')' {
              cssMisc::cur_top->Pop(); /* get rid of incr */
	      cssCodeBlock* blk = new cssCodeBlock(cssForLoopStmt_Name);
	      blk->owner_prog = cssMisc::cur_top->Prog();
  	      blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	      blk->loop_type = cssCodeBlock::FOR;
	      cssMisc::cur_top->AddStatic(blk);
	      $$ = Code1(blk); cssMisc::cur_top->Push(blk->code); }
        ;


doloop:   do stmt
        ;

do:   	 CSS_DO 	 	{ 
            cssCodeBlock* blk = new cssCodeBlock(cssDoLoop_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->loop_type = cssCodeBlock::DO;
	    cssMisc::cur_top->AddStatic(blk); /* while is all inside this do! */
	    $$ = Code1(blk); cssMisc::cur_top->Push(blk->code); }

        ;


whiloop:  while cond term  	{
            cssProg* prv_prg = cssMisc::cur_top->PrvProg();
	    if((prv_prg != NULL) && (prv_prg->owner_blk != NULL)
	       && (prv_prg->owner_blk->loop_type == cssCodeBlock::DO)) {
	      cssMisc::cur_top->Pop(); /* pop the if_true block from cond
					  null and should be removed (todo:!) */	      
	      $$ = Code1(cssBI::doloop); /* this is inside of the do block */
	      cssMisc::cur_top->Pop(); /* pop the do_loop from do */
	      
	    }
	    else {
	      cssProg* cp = cssMisc::cur_top->Prog();
	      if(cp->owner_blk == NULL) {
		yyerror("while loop current prog should have owner_blk, doesnt!");
	      }
	      else {
		cp->owner_blk->loop_back = 2; /* jump back 2 steps to cond if loop was run! */
		cp->owner_blk->loop_type = cssCodeBlock::WHILE;
	      }	    
	      cssMisc::cur_top->Pop(); /* pop the if_true block */
	      Code1(cssBI::pop);	/* and code for getting rid of cond val */
	      $$ = $2;
	    } }
        | while cond stmt  	{
	  cssProg* cp = cssMisc::cur_top->Prog();
	  if(cp->owner_blk == NULL) {
	    yyerror("while loop current prog should have owner_blk, doesnt!");
	  }
	  else {
	    cp->owner_blk->loop_back = 2; /* jump back 2 steps to cond if loop was run! */
	    cp->owner_blk->loop_type = cssCodeBlock::WHILE;
	  }	    
	  cssMisc::cur_top->Pop(); /* pop the if_true block */
	  Code1(cssBI::pop);	/* and code for getting rid of cond val */
	  $$ = $2; }
        ;

while:    CSS_WHILE 		{ }
        ;

ifstmt:   if cond noifstmt 	{
          cssMisc::cur_top->Pop(); /* pop the if_true block */
          $$ = $2; }
        | if cond ifstmt 	{
	  cssMisc::cur_top->Pop(); /* pop the if_true block */
	  $$ = $2; }
        | else noifstmt         {
	  cssMisc::cur_top->Pop(); /* pop the if_false block */
	  /* now check for other else blocks that need popping! */
	  while((cssMisc::cur_top->Prog()->owner_blk != NULL) &&
		(cssMisc::cur_top->Prog()->owner_blk->action == cssCodeBlock::ELSE)) {
	    cssMisc::cur_top->Pop(); } /* pop residual elses! */
	  $$ = $1;  }
        | else ifstmt           { /* do not pop the ifstmt here!! */
	  $$ = $1; }
        ;

if:   	  CSS_IF 			{
          cssMisc::cur_top->Prog()->lastif = cssMisc::cur_top->Prog()->size; } /* nxt stmt */
        ;

else:	  CSS_ELSE			{
  	    css_progdx tmp = cssMisc::cur_top->Prog()->lastif;
  	    if(tmp < 0) {
	      yyerror("else without matching if");
	      return cssProg::YY_Err;
	    }
	    cssMisc::cur_top->Prog()->lastif = -1; /* reset it */
            cssCodeBlock* blk = new cssCodeBlock(cssElseBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::ELSE; /* start block of else */
	    cssMisc::cur_top->AddStatic(blk);
	    $$ = Code2(blk, cssBI::pop); /* pop after else to get rid of prev if cond */
	    cssMisc::cur_top->Push(blk->code); }
        ;

cond: 	  cond_paren expr cond_end_paren		{ 
            cssCodeBlock* blk = new cssCodeBlock(cssIfTrueBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::IF_TRUE; /* start block of if-true */
	    cssMisc::cur_top->AddStatic(blk);
	    Code1(blk); cssMisc::cur_top->Push(blk->code); $$ = $1; }
        ;

cond_paren:	  '('			{
            cssCodeBlock* blk = new cssCodeBlock(cssCondBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    blk->action = cssCodeBlock::PUSH_RVAL; /* start conditional */
	    cssMisc::cur_top->AddStatic(blk);
	    $$ = Code1(blk); cssMisc::cur_top->Push(blk->code); }
        ;

cond_end_paren: ')' {
            cssMisc::cur_top->Pop(); }
        ;

bra:	  '{'			{
            cssCodeBlock* blk = new cssCodeBlock(cssBlock_Name);
	    blk->owner_prog = cssMisc::cur_top->Prog();
	    cssMisc::cur_top->AddStatic(blk);
	    $$ = Code1(blk); cssMisc::cur_top->SetPush(blk->code); }
        ;

mbr_bra:  '{'			{  } /* do nothing? */
        ;

ket:	  '}'			{
            cssMisc::cur_top->SetPop();
	    cssScriptFun* sfun = cssMisc::cur_top->Prog()->owner_fun;
	    if((sfun != NULL) && (sfun->GetType() == cssEl::T_MbrScriptFun)) {
	      cssMisc::cur_class = NULL; /* get rid of current class pointer.. */
	      cssMisc::cur_method = NULL; } /* and current method pointer */
	  }
        ;

expr:	  primitive		{ $$ = Code1($1); }
        | comb_expr
        ;

comb_expr:
          memb_expr
        | expr '=' expr 		{ Code1(cssBI::asgn); }
        | expr '=' '{'			{
	    int c; String inp;
	    while(((c = cssMisc::cur_top->Prog()->Getc()) != '}') && (c != EOF)) inp += (char)c;
	    cssSStream* ss = new cssSStream();
	    cssMisc::cur_top->AddLiteral(ss);
 	    stringstream* sss = (stringstream*)ss->GetVoidPtr();
	    *sss << inp;
	    sss->seekg(0, ios::beg);
	    Code3(ss, cssBI::arg_swap, cssBI::rshift);
 	    Code1(cssBI::sstream_rewind); /* rewind stream for next use.. */ }
        | expr CSS_ASGN_ADD expr 	{ Code1(cssBI::asgn_add); }
        | expr CSS_ASGN_SUB expr 	{ Code1(cssBI::asgn_sub); }
        | expr CSS_ASGN_MULT expr 	{ Code1(cssBI::asgn_mult); }
        | expr CSS_ASGN_DIV expr 	{ Code1(cssBI::asgn_div); }
        | expr CSS_ASGN_MOD expr 	{ Code1(cssBI::asgn_mod); }
        | expr CSS_ASGN_LSHIFT expr 	{ Code1(cssBI::asgn_lshift); }
        | expr CSS_ASGN_RSHIFT expr 	{ Code1(cssBI::asgn_rshift); }
        | expr CSS_ASGN_AND expr 	{ Code1(cssBI::asgn_and); }
        | expr CSS_ASGN_XOR expr 	{ Code1(cssBI::asgn_xor); }
        | expr CSS_ASGN_OR expr 	{ Code1(cssBI::asgn_or); }
        | CSS_NEW argstop typeonly	{ $$ = $2; Code2($3,$1); }
        | CSS_NEW argstop typeonly	'(' ')'	{ $$ = $2; Code2($3,$1); }
        | CSS_NEW argstop typeonly arraydim %prec CSS_UNARY {
	  $$ = $2; Code2($3,$1); }
        | CSS_DELETE argstop expr %prec CSS_UNARY { $$ = $2; Code1($1); }
        | CSS_DELETE '[' ']' argstop expr %prec CSS_UNARY { $$ = $4; Code1($1); }
	| expr CSS_OR expr		{ Code1(cssBI::lor); }
	| expr CSS_AND expr		{ Code1(cssBI::land); }
        | '&' expr %prec CSS_UNARY	{ $$ = $2; Code1(cssBI::addr_of); }
        | '*' expr %prec CSS_UNARY	{ $$ = $2; Code1(cssBI::de_ptr); }
	| expr CSS_GT expr		{ Code1(cssBI::gt); }
	| expr CSS_GE expr		{ Code1(cssBI::ge); }
	| expr CSS_LT expr		{ Code1(cssBI::lt); }
	| expr CSS_LE expr		{ Code1(cssBI::le); }
	| expr CSS_EQ expr		{ Code1(cssBI::eq); }
	| expr CSS_NE expr		{ Code1(cssBI::ne); }
        | expr '?' expr ':' expr	{ Code1(cssBI::cond); }
	| expr '+' expr			{ Code1(cssBI::add); }
	| expr '-' expr			{ Code1(cssBI::sub); }
	| expr '*' expr			{ Code1(cssBI::mul); }
	| expr '/' expr			{ Code1(cssBI::div); }
	| expr '%' expr			{ Code1(cssBI::modulo); }
	| expr CSS_LSHIFT expr		{ Code1(cssBI::lshift); }
	| expr CSS_RSHIFT expr		{ Code1(cssBI::rshift); }
	| expr '&' expr			{ Code1(cssBI::bit_and); }
	| expr '^' expr			{ Code1(cssBI::bit_xor); }
	| expr '|' expr			{ Code1(cssBI::bit_or); }
        | expr CSS_PLUSPLUS 		{ Code1(cssBI::asgn_post_pp); }
        | CSS_PLUSPLUS expr		{ $$ = $2; Code1(cssBI::asgn_pre_pp); }
        | expr CSS_MINMIN 		{ Code1(cssBI::asgn_post_mm); }
        | CSS_MINMIN expr		{ $$ = $2; Code1(cssBI::asgn_pre_mm); }
	| '-' expr %prec CSS_UNARYMINUS	{ $$ = $2; Code1(cssBI::neg); }
	| CSS_NOT expr			{ $$ = $2; Code1(cssBI::lnot); }
	| expr '[' expr ']'		{ Code1(cssBI::de_array); }
        | '(' type ')' expr %prec CSS_UNARY {
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($2.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
	    $$ = $4; Code2($2, cssBI::cast); }
        | type '(' expr	')'		{
  	    cssMisc::CodeTop();	/* don't use const expr if const type decl */
	    if($1.El()->tmp_str == "const") {
	      yyerror("const type not accepted in this context");
	      return cssProg::YY_Err; }
  	    $$ = $3; Code2($1, cssBI::cast); }
        | normfuncall
	| '(' expr ')'			{ $$ = $2; }
	;

primitive:
          CSS_NUMBER
        | CSS_STRING
        | CSS_VAR
        | CSS_PTR
        ;

anycmd:   CSS_COMMAND
        ;

normfuncall:
          CSS_FUN 		        { $$ = Code2(cssBI::push_next, $1); }
        | normfun ')'			{ $$ = $1.ival; Code1($1.el); }
	| normfun exprlist ')'		{ $$ = $1.ival; Code1($1.el);
	  if(($1.el.El()->GetType() == cssEl::T_ElCFun) ||
	     /*	     ($1.el.El()->GetType() == cssEl::T_MbrCFun) || */
	     ($1.el.El()->GetType() == cssEl::T_ScriptFun) ||
	     ($1.el.El()->GetType() == cssEl::T_MbrScriptFun)) {
	    cssElFun* fun = (cssElFun*)$1.el.El();
	    int max_args = fun->argc;  int min_args = (fun->argc - fun->arg_defs.size);
	    int act_args = $2;
	    if(fun->GetType() == cssEl::T_MbrScriptFun) { max_args--; min_args--; }
	    if((fun->argc >= 0) && (act_args > max_args)) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(), "Too many arguments for function:",fun->name,", should have at most:", String(max_args), "got:",String(act_args)); }
	    if((fun->argc >= 0) && (act_args < min_args)) {
              cssMisc::Warning(cssMisc::cur_top->Prog(), "Too few arguments for function:",fun->name,", should have at least:",String(min_args),"got:",String(act_args)); } } }
        ;

normfun:  CSS_FUN '('			{
            $$.ival = Code1(cssMisc::VoidElPtr); /* an arg stop */
	    $$.el = $1;	/* save this pointer for later.. */
            if($1.El()->GetType() == cssEl::T_MbrScriptFun)
	      Code1(cssBI::push_cur_this); /* push this as first arg.. */
	    else if($1.El()->GetType() == cssEl::T_MbrCFun) {
	      /* only push this for MbrCFun that are builtins (Load, Save, or InheritsFrom) on a script class! */
	      cssMbrCFun* mbrfun = (cssMbrCFun*)$1.El();
	      if((mbrfun->funp == &cssClassType::InheritsFrom_stub) ||
		 (mbrfun->funp == &cssClassType::Load_stub) ||
		 (mbrfun->funp == &cssClassType::Save_stub))
		Code1(cssBI::push_cur_this); /* push this as first arg.. */
	    } }
        ;

memb_expr:
          comb_expr getmemb membname 	{ Code2($3, cssBI::points_at);}
        | primitive getmemb membname 	{
	    int mbno = $1.El()->GetMemberNo((const char*)*($3.El()));
	    if(mbno < 0) { $$ = Code3($1, $3, cssBI::points_at); }
	    else { cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
		   $$ = Code3($1, tmpint, cssBI::points_at); } }
        /* this syntax is non-standard, but it means "top of some root heierarchy"
	   and you need to define some function push_root which pushes a root element
	   on the stack */
        | '.' membname			{ $$ = Code3(cssBI::push_root, $2, cssBI::points_at); }
	| scopetype membname		{
	  cssMisc::cur_scope = NULL;
	    cssEl* scp = $1.El()->GetScoped((const char*)*($2.El()));
	    if(scp != &cssMisc::Void) {  $$ = Code1(scp); }
	    else { $$ = Code3($1, $2, cssBI::scoper); } }

        | membfun end ')'		{
	  /* argstop is put in by member_fun; member_fun skips over end jump, 
	     uses it to find member_call*/
	  $$ = $1.ival;
	  cssMisc::cur_top->Prog()->insts[$2-1]->SetLine(Code1(cssBI::member_call)); }
        | membfun end exprlist ')'  	{
	  $$ = $1.ival;
	  cssMisc::cur_top->Prog()->insts[$2-1]->SetLine(Code1(cssBI::member_call));
	  if(($1.el.El()->GetType() == cssEl::T_ElCFun) ||
	     ($1.el.El()->GetType() == cssEl::T_MbrCFun) ||
	     ($1.el.El()->GetType() == cssEl::T_ScriptFun) ||
	     ($1.el.El()->GetType() == cssEl::T_MbrScriptFun)) {
	    cssElFun* fun = (cssElFun*)$1.el.El();
	    int max_args = fun->argc;  int min_args = (fun->argc - fun->arg_defs.size);
	    int act_args = $3;
	    if(fun->GetType() == cssEl::T_MbrScriptFun) { max_args--; min_args--; }
	    if((fun->argc >= 0) && (act_args > max_args)) {
	      cssMisc::Warning(cssMisc::cur_top->Prog(), "Too many arguments for function:",fun->name,", should have at most:", String(max_args), "got:",String(act_args)); }
	    if((fun->argc >= 0) && (act_args < min_args)) {
              cssMisc::Warning(cssMisc::cur_top->Prog(), "Too few arguments for function:",fun->name,", should have at least:",String(min_args),"got:",String(act_args)); } } }
        ;

membfun:  comb_expr getmemb membname '(' 	{ Code2($3, cssBI::member_fun);
                                                  $$.ival = $1; $$.el.Reset(); }
        | primitive getmemb membname '('	{ $$.el.Reset();
	    int mbno = $1.El()->GetMethodNo((const char*)*($3.El()));
	    if(mbno < 0) { /* don't complain for pointers and references */
	      if(!$1.El()->MembersDynamic())
		cssMisc::Warning(NULL, "Member Function:",(const char*)*($3.El()),
				 "not found in parent object, will be resolved dynamically");
	      $$.ival = Code3($1, $3, cssBI::member_fun); }
	    else {
	      cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
	      $$.ival = Code3($1, tmpint, cssBI::member_fun);
	      cssEl* ths = $1.El();
	      if(ths->GetType() == cssEl::T_Ptr)
		ths = ((cssPtr*)ths)->el_type;
	      if((ths != NULL) && ((ths->GetType() == cssEl::T_Class) || (ths->GetType() == cssEl::T_ClassType))) {
		cssEl* fun = ths->GetMethodFmName((const char*)*($3.El()));
		if(fun->GetType() != cssEl::T_MbrCFun)
		  $$.el.SetDirect(fun);
	      } } }
        | scopetype membname '(' 		{
	    cssMisc::cur_scope = NULL; $$.el.Reset();
	    int mbno = $1.El()->GetMethodNo((const char*)*($2.El()));
	    if(mbno < 0) { /* don't complain for pointers and references */
	      if(!$1.El()->MembersDynamic())
		cssMisc::Warning(NULL, "Member Function:",(const char*)*($2.El()),
				 "not found in parent object, will be resolved dynamically");
	      $$.ival = Code3($1, $2, cssBI::member_fun); }
	    else {
	      cssElPtr tmpint = cssMisc::cur_top->AddLiteral(mbno);
	      $$.ival = Code3($1, tmpint, cssBI::member_fun);
	      cssEl* ths = $1.El();
	      if(ths->GetType() == cssEl::T_Ptr)
		ths = ((cssPtr*)ths)->el_type;
	      if((ths != NULL) && ((ths->GetType() == cssEl::T_Class) || (ths->GetType() == cssEl::T_ClassType))) {
		cssEl* fun = ths->GetMethodFmName((const char*)*($2.El()));
		if(fun->GetType() != cssEl::T_MbrCFun)
		  $$.el.SetDirect(fun);
	      } } }
        ;

/* all the things that should not match w/out a direct reference below */

undefname:
          CSS_NAME			{
            $$ = cssMisc::cur_top->Prog()->Stack()->Push(new cssString($1)); }
        ;

name:	  CSS_NAME			{
            $$ = cssMisc::cur_top->Prog()->Stack()->Push(new cssString($1)); }
        | CSS_NUMBER			{
	    $$ = cssMisc::cur_top->Prog()->Stack()->Push(new cssString($1.El()->GetName())); }
        | CSS_STRING			{
	    $$ = cssMisc::cur_top->Prog()->Stack()->Push(new cssString($1.El()->GetName())); }
        | CSS_VAR			{
	    $$ = cssMisc::cur_top->Prog()->Stack()->Push(new cssString($1.El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", $1.El()->PrintStr()); }
        | CSS_PTR			{
	    $$ = cssMisc::cur_top->Prog()->Stack()->Push(new cssString($1.El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding variable:", $1.El()->PrintStr()); }
        | CSS_FUN			{
	    $$ = cssMisc::cur_top->Prog()->Stack()->Push(new cssString($1.El()->GetName()));
	    if(cssMisc::cur_top->debug >= 1)
	    cssMisc::Warning(cssMisc::cur_top->Prog(), "Warning: hiding function:", $1.El()->PrintStr()); }
	;

getmemb:  '.'			/* these are treated identically in parsing */
        | CSS_POINTSAT
        ;

membname: CSS_NAME			{
   	    String tmpstr = String($1);
            $$ = cssMisc::cur_top->AddLiteral(tmpstr); }
        | CSS_STRING
        | membnms		{ /* this is source of shift-reduce problems */
	    String tmpstr = String($1.El()->GetName());
	    $$ = cssMisc::cur_top->AddLiteral(tmpstr); }
        ;

membnms:  CSS_VAR
        | CSS_PTR
        | CSS_FUN
	| CSS_PTRTYPE
        | anycmd
        ;

exprlist: exprlsel		{ $$ = 1; } /* the argstop points to where the arguments stop on the stack (begin) */
        | exprlist ',' exprlsel { $$ = $1 + 1; }
        ;

exprlsel: expr
        | CSS_PTRTYPE		{ $$ = Code1($1); }/* allow ta_types to be passed */
        ;

cmd_exprlist: cmd_exprlsel	{ $$ = 1; } /* the argstop points to where the arguments stop on the stack (begin) */
        | cmd_exprlist ',' cmd_exprlsel { $$ = $1 + 1; }
        ;

cmd_exprlsel: expr
        | CSS_PTRTYPE		{ $$ = Code1($1); } /* allow ta_types to be passed */
        | CSS_NAME		{ 
	  String tmpstr = String($1);
	  $$ = Code1(cssMisc::cur_top->AddLiteral(tmpstr)); }
        | CSS_TYPE		{ $$ = Code1($1); }
        ;

argstop:  /* nothing */		{ $$ = Code1(cssMisc::VoidElPtr); }
        ;

stmtlist: stmtel
        | vardefin
        | stmtlist ',' stmtlist
        ;

stmtel:	  expr			{ Code1(cssBI::pop); }
        ;

end:	  /* nothing */		{ Code1(cssInst::Stop); $$ = cssMisc::cur_top->Prog()->size; }
        ;


%%

	/* end of grammar */


void yyerror(char* s) { 	/* called for yacc syntax error */
  int i;

  ostream* fh = &cerr;
  if(cssMisc::cur_top->cmd_shell != NULL)
    fh = cssMisc::cur_top->cmd_shell->ferr;

  if(strcmp(s, "parse error") == 0) {
    String src = cssMisc::cur_top->Prog()->GetSrcLC(cssMisc::cur_top->Prog()->tok_line);
    src.gsub('\t',' ');		// replace tabs
    *(fh) << "Syntax Error, line " << cssMisc::cur_top->src_ln << ":\t"
      << src;
    *(fh) << "\t\t\t";
    for(i=0; i < cssMisc::cur_top->Prog()->tok_col; i++)
      *(fh) << " ";
    *(fh) << "^\n";
  }
  else {
    *(fh) << s << " line " << cssMisc::cur_top->src_ln << ":\t"
      << cssMisc::cur_top->Prog()->GetSrcLC(cssMisc::cur_top->Prog()->tok_line);
  }
  taMisc::FlushConsole();
}
