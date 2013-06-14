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

%name-prefix="ap" 

%{

#include <ActrModel>
#include <ActrDeclarativeModule>

#include <taMisc>

#define AMCP    ActrModel::cur_parse

void aperror(const char *s);
int aplex();

%}

/* eleven expected shift-reduce conflicts */
%expect 0

%union {
  const char*     chr;
  int	   	  rval;
  double          num; 
  ActrChunkType*  chtyp;
  ActrChunk*      chk;
  ActrProduction* prod;
  ActrSlotType*   sltyp;
}

/* misc keywords */
%token  <rval>  AP_CHUNK_TYPE AP_PROD AP_ISA AP_GOAL AP_RETRIEVAL AP_OUTPUT
%token  <rval>  AP_CLEAR_ALL AP_DEFINE_MODEL AP_ADD_DM
%token  <rval>  AP_SGP AP_ESC AP_LF
%token  <rval>  AP_TRACE_DETAIL AP_GOAL_FOCUS

/* basic tokens */
%token	<num>	AP_NUMBER
%token  <chr>	AP_NAME AP_COMMENT AP_VALUE

%type   <chtyp>  chunktype chunktype_nm
%type   <sltyp>  slots slot
%type   <rval>   clear_all define_model
%type   <chk>    dm_item_nm dm_item_typ dm_item

%left	'*'
%left   '('

%%
list:	/* nothing */		{
            AMCP->ResetParse(); /* AMCP->load_state = ActrModel::YYRet_Exit; */
            taMisc::Info("nothing");
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
        | list ')'	        { /* end of define model */
	    AMCP->ResetParse(); return AMCP->load_state; }
	| list error		{
            AMCP->ResetParse(); AMCP->load_state = ActrModel::YYRet_NoSrc;
            return AMCP->load_state; }
	;

clear_all: '(' AP_CLEAR_ALL ')'	{ /* assumed */
        }
        ;

define_model: '(' AP_DEFINE_MODEL AP_NAME { 
          AMCP->name = $3;
        }
        ;

sgp:      '(' AP_SGP sgp_params { 
        }
        ;

sgp_params: sgp_param
        | sgp_params sgp_param
        ;

sgp_param: ':' AP_ESC AP_NAME {    }
        | ':' AP_LF AP_NUMBER  {    }
        | ':' AP_TRACE_DETAIL AP_NAME  {    }
        ;

chunktype: chunktype_nm	slots ')'	{
        }
        ;

chunktype_nm: '(' AP_CHUNK_TYPE AP_NAME {
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

dm_items:  dm_item
        | dm_items dm_item
        ;

dm_item:   dm_item_nm dm_item_typ chunk_vals ')' { }
        ;

dm_item_nm: '(' AP_NAME {
           ActrDeclarativeModule* dmod =
             (ActrDeclarativeModule*)AMCP->modules.FindName("declarative");
           bool made_new = false;
           AMCP->load_chunk = dmod->init_chunks.FindMakeNameType($2, NULL, made_new);
           AMCP->load_chunk->name = $2; // should set this!
           $$ = AMCP->load_chunk; } 
        ;    

dm_item_typ: AP_ISA AP_NAME  {
           ActrChunkType* ct = AMCP->FindChunkType($2);
           if(ct) {
             AMCP->load_chunk->SetChunkType(ct);
           }
           $$ = AMCP->load_chunk; } 
        ;    

chunk_vals: chunk_val
        | chunk_vals chunk_val
        ;

chunk_val:  AP_NAME AP_NAME  {
            AMCP->load_chunk->SetSlotVal($1, $2); }
         |  AP_NAME AP_NUMBER {
            AMCP->load_chunk->SetSlotVal($1, (String)$2); }
         ; 

%%

	/* end of grammar */

void aperror(const char *s) { 	/* called for yacc syntax error */
  if(strcmp(s, "parse error") == 0) {
    taMisc::Error("Syntax Error, line:", String(AMCP->load_st_line), ":");
  }
  else {
    taMisc::Error(s, "line:", String(AMCP->load_st_line), ":");
  }
  taMisc::Error(AMCP->load_last_ln);
  String loc;
  for(int i=0; i < AMCP->load_st_col; i++)
    loc << " ";
  loc << "^";
  taMisc::Error(loc);
}
