// Copyright 2006-2018, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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


// the lexical analyzer

#include "css_machine.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "css_ta.h"
#ifndef CSS_NUMBER
#include "css_css_parse.h"
#endif

#include <taMisc>

using namespace std;

static int follow(int expect, int ifyes, int ifno) {
  int c = cssMisc::cur_top->Getc();

  if(c == expect) return ifyes;
  cssMisc::cur_top->unGetc();
  return ifno;
}

static int follow2(int expect1, int if1, int expect2, int if2, int ifno) {
  int c = cssMisc::cur_top->Getc();

  if(c == expect1) return if1;
  if(c == expect2) return if2;
  cssMisc::cur_top->unGetc();
  return ifno;
}

static int follow3(int expect1, int if1, int expect2, int if2,
		   int expect3, int if3, int ifno) {
  int c = cssMisc::cur_top->Getc();

  if(c == expect1) return if1;
  if(c == expect2) return if2;
  if(c == expect3) return if3;
  cssMisc::cur_top->unGetc();
  return ifno;
}

static int skip_white_nocr() {
  int c;
  while (isspace(c=cssMisc::cur_top->Getc()) && (c != '\n'));
  return c;
}

static int skip_white() {
  int c;
  while (isspace(c=cssMisc::cur_top->Getc()));
  return c;
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

int yylex()
{
  int c, nxt;
  cssElPtr s;

  // for parsing class comments
  int last_tok_line = cssMisc::cur_top->tok_src_ln;
  char* last_tok = cssMisc::cur_top->CurTokSrc();

  do {
    // filter out cr's only when not parsing commands
    if(cssMisc::cur_top->parsing_command) {
      if(cssMisc::cur_top->debug >= 4)
	cerr << "    parsing command = true!" << endl;
      c = skip_white_nocr();
    }
    else
      c = skip_white();

    // start of the current token
    cssMisc::cur_top->StoreCurTokSrcPos();

    // for parsing class comments
    bool classcmt = ((cssMisc::cur_class != NULL) &&
                     (cssMisc::cur_method == NULL) &&
                     (cssMisc::cur_top->tok_src_ln == last_tok_line ||
                      cssMisc::cur_top->tok_src_ln == last_tok_line + 1) &&
                     (*last_tok == ';' || *last_tok == '{'));

    if(c == EOF) {
      return cssProg::YY_Exit;
    }

    if(c == '#') {
      nxt = cssLex::readword(cssMisc::cur_top->Prog(), cssMisc::cur_top->Getc());
      if(cssLex::Buf == "!") {		// skip line on #!
	while(((c = cssMisc::cur_top->Getc()) != EOF) && (c != '\n'));
	continue;		// restart scan on next line
      }


      if((s = cssMisc::PreProcessor.FindName(cssLex::Buf)) != 0) { // pre-processor
	cssMisc::cur_top->parsing_command = 1;
	yylval.el = s;
        s.El()->prog = cssMisc::cur_top->Prog();
	return (s.El())->GetParse();
      }
      yylval.nm = cssLex::Buf;
      return CSS_NAME;
    }

    if(c == '.') {
      nxt = cssMisc::cur_top->Getc();
      cssMisc::cur_top->unGetc(); // look ahead
      if(!(isdigit(nxt)))
	return '.';		// pointsat
    }

    bool got_minus_num = false;
   if(cssMisc::parsing_args) {	// note this is important because otherwise ##-## is parsed wrong (subtraction)
     if(c == '-') {
	nxt = cssMisc::cur_top->Getc();
	cssMisc::cur_top->unGetc(); // look ahead
	if(!(isdigit(nxt) || (nxt == '.')))
	  return follow3('=', CSS_ASGN_SUB, '-', CSS_MINMIN, '>', CSS_POINTSAT, '-');
	got_minus_num = true;
      }
   }

    if((c == '.') || got_minus_num || isdigit(c)) {	// number
      Real r;
      int iv;
      bool gotreal = false;
      bool gothex = false;
      cssLex::Buf = "";
      int prev = 0;

      do {
        if(c == '.' || ((c == 'e' || c == 'E') && !gothex)) gotreal = true;
        else if((c == 'x' || c == 'X') && prev == '0') gothex = true;
        cssLex::Buf += (char)c;
	prev = c;
      } while(((c=cssMisc::cur_top->Getc()) != EOF) &&
	      ((c == '.') || isxdigit(c) || ((prev == '0') && (c == 'x' || c == 'X')) || 
	       (((prev == 'e') || (prev == 'E')) && (c == '-'))));

      cssMisc::cur_top->unGetc();

      const char* startp = (const char*) cssLex::Buf;
      char* endp = NULL;
      if(gotreal) {
        r = strtod(startp, &endp);
	yylval.el = cssMisc::cur_top->AddLiteral(r);
      }
      else {
        iv = (int)strtol(startp, &endp, 0);
	yylval.el = cssMisc::cur_top->AddLiteral(iv);
      }
      if(*endp == 'f') // float (not double) designator
	endp++;
      while (endp++ < startp + cssLex::Buf.length())
         cssMisc::cur_top->unGetc();

      return CSS_NUMBER;
    }

    if(isalpha(c) || (c == '_')) {

      cssLex::readword(cssMisc::cur_top->Prog(), c);

      // if processing a path, don't go looking stuff up anywhere else!!! 
      // took too long to figure this one out..
      if(cssMisc::cur_top->parse_path_expr || cssMisc::cur_scope) {
	yylval.nm = cssLex::Buf;
	if(cssMisc::cur_scope) { // need to check for SCPTYPE guy -- only special case
	  if(cssMisc::cur_scope->GetType() == cssEl::T_ClassType) {
	    cssClassType* clt = (cssClassType*)cssMisc::cur_scope;
	    if(clt->name == cssLex::Buf) { // special case for constructors!
	      return CSS_TYPE;		 // indicates ctor
	    }
	    if((s = clt->types->FindName(cssLex::Buf)) != 0) {
	      yylval.el = s;
              s.El()->prog = cssMisc::cur_top->Prog();
	      if((s.El()->GetType() != cssEl::T_Enum) && (s.El())->GetParse() != CSS_PTR)
		return CSS_SCPTYPE;
	    }
	  }
	  else if(cssMisc::cur_scope->GetParse() == CSS_PTR) {
	    if((s = cssMisc::cur_scope->GetScoped(cssLex::Buf)) != 0) {
	      yylval.el = s;
              s.El()->prog = cssMisc::cur_top->Prog();
	      cssEl* obj = s.El();
	      if(obj->GetParse() == CSS_PTR) {
		if(obj->GetType() == cssEl::T_TA) {
		  cssTA* tao = (cssTA*)obj->GetNonRefObj();
		  TypeDef* taonrt = tao->GetNonRefTypeDef();
		  if(taonrt && taonrt->IsEnum())
		    return CSS_SCPTYPE;
		}
	      }
	    }
	  }
	}
	return CSS_NAME;
      }

      if(cssMisc::cur_top->ext_parse_fun_pre) {
	int rval = (*(cssMisc::cur_top->ext_parse_fun_pre))
	  (cssMisc::cur_top->ext_parse_user_data, cssLex::Buf, s);
	if(rval != 0) {
	  yylval.el = s;
          s.El()->prog = cssMisc::cur_top->Prog();
	  return rval;
	}
      }

      cssProgSpace* src_prog = cssMisc::cur_top->GetSrcProg(); // also check in src_prog if cmd shell

      if(cssLex::Buf.before(3) == "TA_") {
	// parse as a type name
	String nm = cssLex::Buf.after("TA_");
	TypeDef* td = taMisc::FindTypeName(nm);
	if(td != NULL) {
	  cssTA* tatd = new cssTypeDef((void*)td, 1, &TA_TypeDef);
	  yylval.el = cssMisc::cur_top->AddLiteral(tatd);
	  return CSS_VAR;
	}
      }
      if((s = cssMisc::cur_top->types.FindName(cssLex::Buf)) != 0) {
	yylval.el = s;
        s.El()->prog = cssMisc::cur_top->Prog();
	if((s.El())->GetParse() == CSS_PTR)
	  return CSS_PTRTYPE;
	else
	  return CSS_TYPE;
      }
      if(src_prog) {
	if((s = src_prog->types.FindName(cssLex::Buf)) != 0) {
	  yylval.el = s;
          s.El()->prog = cssMisc::cur_top->Prog();
	  if((s.El())->GetParse() == CSS_PTR)
	    return CSS_PTRTYPE;
	  else
	    return CSS_TYPE;
	}
      }
      if((s = cssMisc::TypesSpace.FindName(cssLex::Buf)) != 0) {
	yylval.el = s;
        s.El()->prog = cssMisc::cur_top->Prog();
	if((s.El())->GetParse() == CSS_PTR)
	  return CSS_PTRTYPE;
	else
	  return CSS_TYPE;
      }
      if((cssMisc::cur_class != NULL) &&
	 ((s = cssMisc::cur_class->types->FindName(cssLex::Buf)) != 0)) {
	yylval.el = s;
        s.El()->prog = cssMisc::cur_top->Prog();
	if(s.El()->GetType() == cssEl::T_Enum)
	  return CSS_VAR;
	else if((s.El())->GetParse() == CSS_PTR)
	  return CSS_PTRTYPE;
	else
	  return CSS_TYPE;
      }
      if((s = cssMisc::cur_top->ParseName(cssLex::Buf)) != 0) {
	yylval.el = s;
        s.El()->prog = cssMisc::cur_top->Prog();
      	if(s.ptr == (void*)&(cssMisc::Constants)) {
	  cssEl::cssTypes typ = s.El()->GetType();
	  if((typ == cssEl::T_Int) || (typ == cssEl::T_Real) ||
	     (typ == cssEl::T_Bool) || (typ == cssEl::T_Enum))
	  {
	    return CSS_NUMBER;
	  }
	  else if(typ == cssEl::T_String) {
	    return CSS_STRING;
	  }
	}
	return s.El()->GetParse();
      }
      if(src_prog) {
	if((s = src_prog->ParseName(cssLex::Buf)) != 0) {
	  yylval.el = s;
          s.El()->prog = src_prog->Prog();
	  if(s.ptr == (void*)&(cssMisc::Constants)) {
	    cssEl::cssTypes typ = s.El()->GetType();
	    if((typ == cssEl::T_Int) || (typ == cssEl::T_Real) ||
	       (typ == cssEl::T_Bool) || (typ == cssEl::T_Enum))
	      {
		return CSS_NUMBER;
	      }
	    else if(typ == cssEl::T_String) {
	      return CSS_STRING;
	    }
	  }
	  return s.El()->GetParse();
	}
      }
      if(cssMisc::cur_top->ext_parse_fun_post) {
	int rval = (*(cssMisc::cur_top->ext_parse_fun_post))
	  (cssMisc::cur_top->ext_parse_user_data, cssLex::Buf, s);
	if(rval != 0) {
	  yylval.el = s;
          s.El()->prog = cssMisc::cur_top->Prog();
	  return rval;
	}
      }
      yylval.nm = cssLex::Buf;
      return CSS_NAME;
    }

    if(c == '\"') {
      cssLex::Buf = "";

      while(((c=cssMisc::cur_top->Getc()) != EOF) && (c != '\"')) {
	if(c == '\\') {
	  c=cssMisc::cur_top->Getc();
	  if((c == '\n') || (c == '\r'))
	    continue;
	  else
	    cssLex::Buf += (char)ctrl_char(c);
	}
	else
	  cssLex::Buf += (char)c;
      }
      yylval.el = cssMisc::cur_top->AddLiteral(cssLex::Buf);
      return CSS_STRING;
    }

    if(c == '\'') {
      int iv = 0;
      int cnt = 0;
      while(((c=cssMisc::cur_top->Getc()) != EOF) && (c != '\'')) {
        if(cnt == 0) {
          if(c == '\\')
            iv = ctrl_char(c=cssMisc::cur_top->Getc());
          else
            iv = c;
        }
        cnt++;
      }
      if(cnt == 0) {
        cssMisc::Warning(NULL, "character expression using single quotes ' was empty -- please specify a single character");
      }
      if(cnt > 1) {
        cssMisc::Warning(NULL, "character expression using single quotes ' is more than a single character in length -- only first char is used -- use double-quotes \" for strings");
      }
      yylval.el = cssMisc::cur_top->AddLiteral(new cssInt(iv));
      return CSS_NUMBER;
    }

    if(c == '/') {
      if(((c = cssMisc::cur_top->Getc()) == '/') || (c == '*')) { 	// comment
        cssLex::Buf = "";
        if(c == '/') {
          while(((c=cssMisc::cur_top->Getc()) != EOF) && (c != '\n'))
            if (classcmt && (c != '\r'))
              cssLex::Buf += (char)c;
        }
	else {
	  while(((c=cssMisc::cur_top->Getc()) != EOF)) {
            if(c == '*') {
              if((c = cssMisc::cur_top->Getc()) == '/')
		break;
              else if (classcmt)
                cssLex::Buf += '*';
	    }
            if (classcmt)
              cssLex::Buf += (char)c;
	  }
        }
        if (classcmt) {
	  yylval.el = cssMisc::cur_top->AddLiteral(cssLex::Buf);
	  return CSS_COMMENT;
	}
	continue;		// redo loop through
      }
      else {
	c = '/';
	cssMisc::cur_top->unGetc();
      }
    }

    if(c == ';') {
      while ((nxt=cssMisc::cur_top->Getc()) == ' ' || (nxt == '\t') || (nxt == '\r'));
      if(nxt != '\n')
	cssMisc::cur_top->unGetc(); // not a newline after semicolon
      return c;
    }

    switch (c) {
    case '<': {
      nxt = cssMisc::cur_top->Getc();
      if(nxt == '=')	return CSS_LE;
      if(nxt != '<')	{ cssMisc::cur_top->unGetc(); return CSS_LT; }
      return follow('=', CSS_ASGN_LSHIFT, CSS_LSHIFT);
      break;
    }
    case '>': {
      nxt = cssMisc::cur_top->Getc();
      if(nxt == '=')	return CSS_GE;
      if(nxt != '>')	{ cssMisc::cur_top->unGetc(); return CSS_GT; }
      return follow('=', CSS_ASGN_RSHIFT, CSS_RSHIFT);
      break;
    }
    case '=':
      return follow('=', CSS_EQ, '=');
      break;
    case '!':
      return follow('=', CSS_NE, CSS_NOT);
      break;
    case '&':
      return follow2('=', CSS_ASGN_AND, '&', CSS_AND, '&');
      break;
    case '^':
      return follow('=', CSS_ASGN_XOR, '^');
      break;
    case '|':
      return follow2('=', CSS_ASGN_OR, '|', CSS_OR, '|');
      break;
    case '+':
      return follow2('=', CSS_ASGN_ADD, '+', CSS_PLUSPLUS, '+');
      break;
    case '-':
      return follow3('=', CSS_ASGN_SUB, '-', CSS_MINMIN, '>', CSS_POINTSAT, '-');
      break;
    case '*':
      return follow('=', CSS_ASGN_MULT, '*');
      break;
    case '/':
      return follow('=', CSS_ASGN_DIV, '/');
      break;
    case ':': 
      if(!cssMisc::parsing_matrix)
	return follow(':', CSS_SCOPER, ':');
      else
	return ':';
      break;
    case '\n':
      return '\n';
      break;
    default:
      return c;
      break;
    }
  } while(1);

  return 0;
}
