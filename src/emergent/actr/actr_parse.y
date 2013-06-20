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

/* parser for actr file loading */

%name-prefix="ap"  /* prevent collision with css yy parser */

%{

#include <ActrModel>
#include <ActrDeclarativeModule>
#include <ActrProceduralModule>
#include <ActrGoalModule>
#include <ActrSlot>

#include <taMisc>

#define AMCP    ActrModel::cur_parse

void aperror(const char *s);
int aplex();

%}

/* zero expected shift-reduce conflicts */
%expect 0

%union {
  const char*     chr;
  int	   	  rval;
  double          num; 
  ActrChunkType*  chtyp;
  ActrChunk*      chk;
  ActrProduction* prod;
  ActrSlotType*   sltyp;
  ActrSlot*       slt;
}

/* misc keywords */
%token  <rval>  AP_CHUNK_TYPE AP_PROD AP_ISA
%token  <rval>  AP_CLEAR_ALL AP_DEFINE_MODEL AP_ADD_DM
%token  <rval>  AP_SGP AP_GOAL_FOCUS AP_SPP
/* basic tokens */
%token	<num>	AP_NUMBER
%token  <chr>	AP_NAME AP_STRING

/* important: any keyword tokens defined after AP_DEFVAR will auto-skip remainder */
%token  <rval>  AP_DEFVAR AP_DEFMETHOD AP_DEFUN AP_SETF

%type   <chtyp>  chunktype chunktype_nm
%type   <sltyp>  slots slot
%type   <rval>   clear_all define_model
%type   <chk>    dm_item_nm dm_item_typ dm_item
%type   <prod>   prod
%type   <slt>    chunk_slot cond_slot act_slot
%type   <num>    apnum

%left	'*'
%left   '('

%%
list:	/* nothing */		{
            AMCP->ResetParse(); /* AMCP->load_state = ActrModel::YYRet_Exit; */
            /* taMisc::Info("nothing"); */
          }
        | list clear_all	{
	    AMCP->ResetParse(); return AMCP->load_state; }
        | list define_model	{
	    AMCP->ResetParse(); return AMCP->load_state; }
        | list sgp	{
	    AMCP->ResetParse(); return AMCP->load_state; }
        | list chunktype	{
	    AMCP->ResetParse(); return AMCP->load_state; }
        | list add_dm	        {
	    AMCP->ResetParse(); return AMCP->load_state; }
        | list prod	        {
	    AMCP->ResetParse(); return AMCP->load_state; }
        | list goal_focus       {
	    AMCP->ResetParse(); return AMCP->load_state; }
        | list spp_expr         {
	    AMCP->ResetParse(); return AMCP->load_state; }
        | list def_ignore       {
	    AMCP->ResetParse(); return AMCP->load_state; }
        | list ')'	        { /* end of define model */
	    AMCP->ResetParse(); return AMCP->load_state; }
	| list error		{
            AMCP->ResetParse(); AMCP->load_state = ActrModel::YYRet_NoSrc;
            return AMCP->load_state; }
	;

clear_all: '(' AP_CLEAR_ALL ')'	{ /* assumed */
        }
        ;

define_model: '(' AP_DEFINE_MODEL AP_NAME { AMCP->name = $3; AMCP->SigEmitUpdated(); }
        ;

sgp:      '(' AP_SGP sgp_params ')' { }
        ;

sgp_params: sgp_param
        | sgp_params sgp_param
        ;

sgp_param: sgp_param_nm AP_NAME  { }
        | sgp_param_nm apnum     { }
        | sgp_param_nm '(' apnum apnum ')' { }
        ;

sgp_param_nm: ':' AP_NAME  { /* todo: write overall param parser */ }
        ;

chunktype: chunktype_nm	slots ')'	{ }
        ;

chunktype_nm:
         '(' AP_CHUNK_TYPE AP_NAME {
           bool made_new = false;
           AMCP->load_chtype = AMCP->chunk_types.FindMakeNameType($3, NULL, made_new);
           $$ = AMCP->load_chtype;
         }
        ;       

slots:    slot
        | slots slot	{ $$ = $1; }
        ;

slot:     AP_NAME  {
            bool made_new = false;
            $$ = AMCP->load_chtype->slots.FindMakeNameType($1, NULL, made_new);
          }
        ;

add_dm:   '(' AP_ADD_DM dm_items ')' {   }
        ;

dm_items: dm_item
        | dm_items dm_item
        | dm_items error
        ;

dm_item:  dm_item_nm dm_item_typ ')' { }
        | dm_item_nm dm_item_typ chunk_vals ')' { }
        ;

dm_item_nm: '(' AP_NAME {
           ActrDeclarativeModule* dmod = AMCP->DeclarativeModule();
           bool made_new = false;
           AMCP->load_chunk = dmod->init_chunks.FindMakeNameType($2, NULL, made_new);
           /*           AMCP->load_chunk->name = $2; // should set this! */
           $$ = AMCP->load_chunk; } 
        ;    

dm_item_typ: AP_ISA AP_NAME  {
           ActrChunkType* ct = AMCP->FindChunkType($2);
           if(ct) {
             AMCP->load_chunk->SetChunkType(ct);
           }
           $$ = AMCP->load_chunk; } 
        ;    

chunk_vals: 
          chunk_val
        | chunk_vals chunk_val
        ;

chunk_val:  chunk_slot AP_NAME   { if($1) $1->val = $2; }
         |  chunk_slot apnum { if($1) $1->val = (String)$2; }
         ; 

chunk_slot: AP_NAME { $$ = AMCP->load_chunk->FindSlot($1); }
         ;

prod:      prod_name prod_lhs '=' '=' '>' prod_rhs ')' { }
         ;

prod_name: '(' AP_PROD AP_NAME {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType($3, NULL, made_new); }

prod_lhs:  prod_cond
         | prod_lhs prod_cond
         | prod_lhs error
         ;

prod_cond: prod_cond_name prod_cond_vals { }
         | prod_cond_name prod_cond_type prod_cond_vals { }
         ;

prod_cond_name:
           '=' AP_NAME '>' {
           AMCP->load_cond = (ActrCondition*)AMCP->load_prod->conds.New(1);
           AMCP->load_cond->src = AMCP->buffers.FindName($2);
           AMCP->load_cond->cond_src = ActrCondition::BUFFER_EQ; }
         | '?' AP_NAME '>' {
           AMCP->load_cond = (ActrCondition*)AMCP->load_prod->conds.New(1);
           AMCP->load_cond->src = AMCP->buffers.FindName($2);
           AMCP->load_cond->cond_src = ActrCondition::BUFFER_QUERY; }
         ;

prod_cond_type:
           AP_ISA AP_NAME {
             ActrChunkType* ct = AMCP->FindChunkType($2);
             if(ct) {
               AMCP->load_cond->cmp_chunk.SetChunkType(ct);
             }
           }
         ;

prod_cond_vals:
           prod_cond_val
         | prod_cond_vals prod_cond_val
         ;

prod_cond_val:  
           cond_slot AP_NAME  {
             AMCP->load_cond->SetVal($1, $2); }
         | cond_slot apnum {
           AMCP->load_cond->SetVal($1, (String)$2); }
         | cond_slot '=' AP_NAME {
           AMCP->load_cond->SetVal($1, String("=") + $3); }
         | '-' cond_slot AP_NAME {
           AMCP->load_cond->SetVal($2, $3 + String("-")); }
         | '-' cond_slot apnum {
           AMCP->load_cond->SetVal($2, (String)$3 + String("-")); }
         | '-' cond_slot '=' AP_NAME {
           AMCP->load_cond->SetVal($2, String("=") + $4 + String("-")); }
         ; 

cond_slot: AP_NAME { $$ = AMCP->load_cond->cmp_chunk.FindSlot($1); }
         ;

prod_rhs:  prod_act
         | prod_rhs prod_act
         | prod_rhs error
         ;

prod_act:  prod_act_name  /* just a clear */
         | prod_act_name prod_act_type prod_act_vals { }
         | prod_act_name prod_act_vals { }
         | prod_act_bang { }
         | prod_act_bang act_expr { }
         ;

prod_act_name:
           '=' AP_NAME '>' {
           AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
           AMCP->load_act->dest = AMCP->buffers.FindName($2);
           AMCP->load_act->action = ActrAction::UPDATE; 
           AMCP->load_act->UpdateAfterEdit_NoGui(); }
         | '+' AP_NAME '>' {
           AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
           AMCP->load_act->dest = AMCP->buffers.FindName($2);
           AMCP->load_act->action = ActrAction::REQUEST;
           AMCP->load_act->UpdateAfterEdit_NoGui(); }
         | '-' AP_NAME '>' {
           AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
           AMCP->load_act->dest = AMCP->buffers.FindName($2);
           AMCP->load_act->action = ActrAction::CLEAR;
           AMCP->load_act->UpdateAfterEdit_NoGui(); }
         ;

prod_act_bang:
           '!' AP_NAME '!' {
             AMCP->load_act = (ActrAction*)AMCP->load_prod->acts.New(1);
             AMCP->load_act->SetBangAction($2);
             AMCP->load_act->UpdateAfterEdit_NoGui(); }
         ;

prod_act_type:
           AP_ISA AP_NAME {
             ActrChunkType* ct = AMCP->FindChunkType($2);
             if(ct) {
               AMCP->load_act->chunk.SetChunkType(ct);
             }
           }
         ;

prod_act_vals:
           prod_act_val
         | prod_act_vals prod_act_val
         ;

prod_act_val:  
            act_slot AP_NAME  { if($1) $1->val = $2; }
         |  act_slot apnum { if($1) $1->val = (String)$2; }
         |  act_slot '=' AP_NAME { if($1) $1->val = String("=") + $3; }
         ; 

act_expr: '(' sub_exp ')'
         ;

sub_exp:   AP_NAME     { AMCP->load_act->val += $1; }
         | '=' AP_NAME { AMCP->load_act->val += String("=") + $2; }
         | AP_STRING   { AMCP->load_act->val += $1; }
         | AP_NUMBER   { AMCP->load_act->val += (String)$1; }
         ;

act_slot: AP_NAME { $$ = AMCP->load_act->chunk.FindSlot($1); }
         ;

goal_focus: '(' AP_GOAL_FOCUS AP_NAME ')' {
              ActrDeclarativeModule* dmod = AMCP->DeclarativeModule();
              ActrGoalModule* gmod = AMCP->GoalModule();
              ActrChunk* gf = dmod->init_chunks.FindName($3);
              if(!gf) { taMisc::Error("goal-focus cannot find declarative init chunk:", $3); }
              else    { ActrChunk* ngf = (ActrChunk*)gf->Clone(); ngf->name = gf->name; 
                        gmod->init_chunk.Add(ngf); }
            }
         ;

spp_expr: '(' AP_SPP spp_prod ':' AP_NAME apnum ')' {
           AMCP->load_prod->SetParam($5, $6); }
         ;

spp_prod: AP_NAME {
           ActrProceduralModule* pmod = AMCP->ProceduralModule();
           bool made_new = false;
           AMCP->load_prod = pmod->productions.FindMakeNameType($1, NULL, made_new); }
         ;

def_ignore:
           '(' AP_DEFVAR
         | '(' AP_DEFMETHOD
         | '(' AP_DEFUN 
         | '(' AP_SETF 
         ;

apnum:       AP_NUMBER
         | '-' AP_NUMBER { $$ = -$2; }
         ;

%%

	/* end of grammar */

void aperror(const char *s) { 	/* called for yacc syntax error */
  AMCP->ParseErr(s);
}
