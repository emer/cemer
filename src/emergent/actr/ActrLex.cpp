// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ActrModel.h"

#include "actr_parse.h"

#include <taMisc>

int ActrModel::Getc() {
  if(load_pos >= load_str.length())
    return EOF;
  char c = load_str[load_pos++];
  if(load_debug > 2) {
    if(isprint(c)) {
      taMisc::Info("C!!: =>", taMisc::LeadingZeros(load_line,5), ":\t", String((char)c));
    }
    else {
      taMisc::Info("C!!: =>", taMisc::LeadingZeros(load_line,5), ":\t <np>");
    }
  }
  // now, detect CRLF and skip to the lf
  if (c == '\r') {
    if (Peekc() == '\n')
      c = load_str[load_pos];
    load_pos++;
  }
  if ((c == '\n') || (c == '\r')) {
    // Info(4, "L!!: =>", taMisc::LeadingZeros(line,5), ":\t", LastLn);
    load_line++;
    load_st_line_pos = load_pos;
    load_col = 0;
    load_last_ln.truncate(0);
  }
  else {
    load_last_ln.cat(c);
    load_col++;
  }
  return c;
}

int ActrModel::Peekc() {
  if(load_pos >= load_str.length())
    return EOF;
  return load_str[load_pos];
}

void ActrModel::unGetc(int c) {
  load_col--;
  load_pos--;
}

int ActrModel::skipwhite() {
  int c;
  while (((c=Getc()) == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
  return c;
}

int ActrModel::skipwhite_peek() {
  int c;
  while (((c=Peekc()) == ' ') || (c == '\t') || (c == '\n') || (c == '\r')) Getc();
  return c;
}

int ActrModel::skipwhite_nocr() {
  int c;
  while (((c=Getc()) == ' ') || (c == '\t'));
  return c;
}

int ActrModel::skipline() {
  load_buf = "";

  int c;
  char lastc = '\0';
  while ((c=Getc()) != EOF) {
     if ( ((c == '\n') || ((c == '\r'))) && lastc != '\\') break;
     lastc = c;
     load_buf += (char)c;
  }
  return c;
}

int ActrModel::readword(int c) {
  load_buf = "";

  load_buf += (char)c;
  while ((c=Peekc()) != EOF && (isalnum(c) || (c == '_') || (c == '-'))) {
    if(c == '-') c = '_';       // convert to C++ compatible
    load_buf += (char)c; Getc();
  } 
  if(c == EOF)
    return EOF;
  if ((c == '\n') || (c == '\r'))
    Getc();
  return c;
}  

void ActrModel::ResetParse() {
  load_chtype = NULL;
  load_chunk = NULL;
  load_prod = NULL;
  load_comment = "";
}

int aplex() {
  if(ActrModel::cur_parse) {
    return ActrModel::cur_parse->Lex();
  }
  return ActrModel::YYRet_Exit;
}

int ActrModel::Lex() {
  int c;
  do {
    load_st_line = load_line;		// save for burping, if necc.
    load_st_col = load_col;
    load_st_pos = load_pos;

    c = skipwhite();

    if(c == EOF) {
      load_state = YYRet_Exit;
      return YYRet_Exit;
    }

    if(c == ';') {
      c = skipline();
      load_comment = load_buf;
      continue;
    }

    if((c == '.') || isdigit(c) || (c == '-')) {	// number
      int gotreal = 0;
      load_buf = (char)c;
      if(c == '.') gotreal = 1;
      
      while(((c=Peekc()) != EOF) &&
	    ((c == '.') || isxdigit(c) || (c == 'x') || (c == 'e') || (c == '-') ||
	     (c == 'X') || (c == 'E')))	{
	load_buf += (char)c; if(c == '.') gotreal = 1;
	Getc();
      } 

      double val = (double)load_buf;
      aplval.num = val;
      return AP_NUMBER;
    }

    if(isalpha(c) || (c == '_')) {
      readword(c);
      Variant val = load_keywords.GetVal(load_buf);
      if(!val.isNull()) {
        aplval.rval = val.toInt();
        return val.toInt();
      }
      aplval.chr = load_buf;
      return AP_NAME;
    }
    
    return c;
  }
  while(true);
}

void ActrModel::InitLoadKeywords() {
  if(load_keywords.size > 0) return;
  // note: all - are converted to _ immediately, so use _ here
  load_keywords.Add(NameVar("p", AP_PROD));
  load_keywords.Add(NameVar("P", AP_PROD));
  load_keywords.Add(NameVar("ISA", AP_ISA));
  load_keywords.Add(NameVar("goal", AP_GOAL));
  load_keywords.Add(NameVar("retrieval", AP_RETRIEVAL));
  load_keywords.Add(NameVar("output", AP_OUTPUT));
  load_keywords.Add(NameVar("chunk_type", AP_CHUNK_TYPE));
  load_keywords.Add(NameVar("clear_all", AP_CLEAR_ALL));
  load_keywords.Add(NameVar("define_model", AP_DEFINE_MODEL));
  load_keywords.Add(NameVar("add_dm", AP_ADD_DM));
  load_keywords.Add(NameVar("sgp", AP_SGP));
  load_keywords.Add(NameVar("esc", AP_ESC));
  load_keywords.Add(NameVar("lf", AP_LF));
  load_keywords.Add(NameVar("trace_detail", AP_TRACE_DETAIL));
  load_keywords.Add(NameVar("goal_focus", AP_GOAL_FOCUS));
}
