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
  load_bang_expr = false;
}

int ActrModel::skip_till_rp(int init_depth) {
  int c = skipwhite();
  String st = load_last_ln;
  load_buf = (char)c;
  int depth = init_depth;
  while (((c = Peekc()) != EOF) && !(((c == ')')) && (depth <= 0))) {
    load_buf += (char)c;
    if(c == '(')      depth++;
    if(c == ')')      depth--;
    Getc();
  }
  TestWarning(true, "Skip",
              "skipping over lisp code that cannot be imported:\n", st,
              load_buf);
  return c;
}

int aplex() {
  if(ActrModel::cur_parse) {
    return ActrModel::cur_parse->Lex();
  }
  return ActrModel::YYRet_Exit;
}

static int ctrl_char(int c) {
  int rval = ' ';
  switch (c) {
  case 'n':
    rval = '\n';	break;
  case 't':
    rval = '\t';	break;
  case 'v':
    rval = '\v';	break;
  case 'b':
    rval = '\b';	break;
  case 'r':
    rval= '\r';		break;
  case 'f':
    rval = '\f';	break;
  case '\\':
    rval = '\\';	break;
  case '?':
    rval = '\?';	break;
  case '\'':
    rval = '\'';	break;
  case '\"':
    rval = '\"';	break;	// "
  case '0':
    rval = '\0';	break;
  }
  return rval;
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

    if(load_bang_expr) {
      load_bang_expr = false;   // only true once
      if(c == '(') {
        skip_till_rp(0);       // skip it all -- depth is 0
        Getc();                // suck up the final )
        aplval.chr = load_buf;
        return AP_BANG_EXPR;
      }
      else if(c == '=') {
        skip_till_rp(-1);       // skip it all -- depth is -1
        Getc();                // suck up the final )
        aplval.chr = load_buf;
        return AP_BANG_EXPR;
      }
    }

    if((c == '.') || isdigit(c)) {	// number: note no -
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

    if(c == '\"') {
      load_string = "";
      while(((c=Getc()) != EOF) && (c != '\"')) {
	if(c == '\\') {
	  c=Getc();
	  if((c == '\n') || (c == '\r'))
	    continue;
	  else
	    load_string += (char)ctrl_char(c);
	}
	else
	  load_string += (char)c;
      }
      aplval.chr = load_string;
      return AP_STRING;
    }

    if(isalpha(c) || (c == '_')) {
      readword(c);
      Variant val = load_keywords.GetVal(load_buf);
      if(!val.isNull()) {
        aplval.rval = val.toInt();
        if(aplval.rval >= AP_DEFVAR) {
          skip_till_rp();       // skip it all..
        }
        return val.toInt();
      }
      load_name = load_buf;
      aplval.chr = load_name;
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
  load_keywords.Add(NameVar("isa", AP_ISA));
  load_keywords.Add(NameVar("chunk_type", AP_CHUNK_TYPE));
  load_keywords.Add(NameVar("clear_all", AP_CLEAR_ALL));
  load_keywords.Add(NameVar("define_model", AP_DEFINE_MODEL));
  load_keywords.Add(NameVar("add_dm", AP_ADD_DM));
  load_keywords.Add(NameVar("sgp", AP_SGP));
  load_keywords.Add(NameVar("goal_focus", AP_GOAL_FOCUS));
  load_keywords.Add(NameVar("spp", AP_SPP));
  load_keywords.Add(NameVar("defvar", AP_DEFVAR));
  load_keywords.Add(NameVar("defmethod", AP_DEFMETHOD));
  load_keywords.Add(NameVar("defun", AP_DEFUN));
  load_keywords.Add(NameVar("setf", AP_SETF));
}

void ActrModel::ParseErr(const char* er) {
  String erm = String(er) + " line: " + String(load_st_line) + ":";
  String src = load_last_ln;
  src.gsub('\t',' ');         // replace tabs
  String msg = erm + "\n" + src;
  if(erm.startsWith("syntax error")) {
    msg += "\n";
    for(int i=0; i <= load_st_col; i++)
      msg += " ";
    msg += "^";
  }
  if(taMisc::ErrorCancelCheck()) {
    taMisc::Info(msg);
  }
  else {
    taMisc::Error(msg);
  }
}
