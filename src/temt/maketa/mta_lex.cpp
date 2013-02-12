// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "maketa.h"
#include <taMisc>

#include <ctype.h>

/* note: we handle the following line end types:
  Windows     cr lf
  MacOS	      cr
  Unix	      lf

  NOTE: crlf is treated as a single character -- last_char will be the char
    before the cr, not cr
*/

int MTA::Getc() {
  // char c = fh.get();
  if(strm_pos >= file_str.length())
    return EOF;
  char c = file_str[strm_pos];
  //  strm_pos++;
  strm_pos = strm_pos + streampos(1);
  // now, detect for CRLF and skip to the lf
  if(verbose > 4) // && (state != MTA::Find_Item))
    cerr << "C!!: => " << line << " :\t" << c << "\n";
  if (c == '\r') {
    if (Peekc() == '\n')
      // c = fh.get();
      c = file_str[strm_pos];
      strm_pos = strm_pos + streampos(1);
  }
  if ((c == '\n') || (c == '\r')) {
    MTA::LastLn[col] = '\0';	// always terminate
    if(verbose > 3) // && (state != MTA::Find_Item))
      cerr << "I!!: => " << line << " :\t" << MTA::LastLn << "\n";
    line++;
    st_line_pos = strm_pos;
    col = 0;
  } else {
    if(col >= 8191)
      cout << "W!!: Warning, line length exceeded in line: " << line << "\n";
    else {
      MTA::LastLn[col++] = c;
      MTA::LastLn[col] = '\0';	// always terminate
    }
  }
  return c;
}

int MTA::Peekc() {
  // return fh.peek();
  if(strm_pos >= file_str.length())
    return EOF;
  return file_str[strm_pos];
}

void MTA::unGetc(int c) {
  col--;
  //  strm_pos--;
  strm_pos = strm_pos - streampos(1);
  // fh.putback(c);
}

int MTA::skipwhite() {
  int c;
  while (((c=Getc()) == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
  return c;
}

int MTA::skipwhite_peek() {
  int c;
  while (((c=Peekc()) == ' ') || (c == '\t') || (c == '\n') || (c == '\r')) Getc();
  return c;
}

int MTA::skipwhite_nocr() {
  int c;
  while (((c=Getc()) == ' ') || (c == '\t'));
  return c;
}

int MTA::skipline() {
  int c;
  char lastc = '\0';
  while ((c=Getc()) != EOF) {
     if ( ((c == '\n') || ((c == '\r'))) && lastc != '\\') break;
     lastc = c;
  }
  return c;
}

int MTA::skiplines(int n_lines) {
  int c = '\0';
  for(int i=0; i<n_lines; i++) {
    c = skipline();
    if(c == EOF) break;
  }
  return c;
}

int MTA::skiptocommarb() {
  int c;
  while (((c=Peekc()) != EOF) && !((c == ',') || (c == '}'))) Getc();
  return c;
}


int MTA::readword(int c) {
  LexBuf = "";

  LexBuf += (char)c;
  while ((c=Peekc()) != EOF && (isalnum(c) || (c == '_'))) { LexBuf += (char)c; Getc(); } 
  if(c == EOF)
    return EOF;
  if ((c == '\n') || (c == '\r'))
    Getc();
  return c;
}  

int MTA::readfilename(int c) {
  // if c is a quote, then read to end quote, else assume it is unquoted fname
  // and read until next ws 
  // note that quoted fnames can contain embedded spaces
  LexBuf = "";
  if (c == '\"') { // since c was the opening ", we don't add it to lexbuf
    // note: ws inside will be Very Bad, but we terminate regardless
    while (((c=Getc()) != EOF) && (c != '\t') && (c != '\n') && (c != '\r')) {
      if (c == '\"') { // found terminator, so skip it and break out
        c = Getc();
        break;
      }
      LexBuf += (char)c;
    } 
  } else {
    LexBuf += (char)c; // gotta include the first guy!
    while (((c=Getc()) != EOF) && (c != ' ') && (c != '\t') && (c != '\n') && (c != '\r')) {
      LexBuf += (char)c;
    } 
  }
  if(c == EOF)
    return EOF;
  return c;
}  

int MTA::follow(int expect, int ifyes, int ifno) {
  int c = Peekc();
  
  if(c == expect) {
    Getc();
    return ifyes;
  }
  return ifno;
}

// static int follow2(int expect1, int if1, int expect2, int if2, int ifno) {
//   int c = Getc();
//   
//   if(c == expect1) return if1;
//   if(c == expect2) return if2;
//   unGetc(c);
//   return ifno;
// }
// 
// static int follow3(int expect1, int if1, int expect2, int if2, 
// 		   int expect3, int if3, int ifno) {
//   int c = Getc();
//   
//   if(c == expect1) return if1;
//   if(c == expect2) return if2;
//   if(c == expect3) return if3;
//   unGetc(c);
//   return ifno;
// }

int yylex()
{
  return mta->lex();
}

String MTA::lexCanonical(const String& in) {
#ifdef TA_OS_WIN
  //NOTE: this routine will probably fail if server shares are used -- use mapped drive letters instead
  // ex. \\myserver\myshare\myfile.xx --> map \\myserver\myshare to Z: -> Z:\myfile
  // first, remove any double backslashes
  String rval = in; rval.gsub("\\\\", "\\");
  // then, convert all forward slashes to Windows backslashes
  //  (this is the safest common-denominator)
  rval.gsub("/", "\\");
  // remove any double backslashes AGAIN -- this happens with gcc/maketa/mingw funky: dir\\/file
  rval.gsub("\\\\", "\\");
  // note: we assume that pathing is case-exact (even though win filenames are case-insensitive)
  // but we do convert drive letter to upper case
  String drv = rval.before(":");
  if (drv.nonempty()) {
    drv.upcase();
    rval = drv + rval.from(":");
  }
  //TODO: maybe we should do a case conversion here, but possibly only to drive letter
  return rval;
#else
  // in Unix, we don't use backslashes, and all filename 
  // components are strictly case sensitive, so we are already
  // lexically canonical
/*note: we could do this... but note, not avail on win32/msvc
  char resolved_path[PATH_MAX];
  char* r = realpath(in, resolved_path);
  if (r) {
    return r; // same as our buff
  } */
  return in;
#endif
}


int MTA::lex() {
  int c, nxt, prv;
  TypeDef *itm;
  int bdepth = 0;

  do {
    st_line = line;		// save for burping, if necc.
    st_col = col;
    st_pos = strm_pos;

    c = skipwhite();

    if(c == EOF)
      return YYRet_Exit;
    
    if(c == '/') {
      c = Peekc();
      if((c == '/') || (c == '*')) { 	// comment
	Getc();
	ComBuf = "";
	prv = c;
	ComBuf += (char)' ';
	if(prv == '/') {
	  c = skipwhite_nocr();
	  if((c != EOF) && ((c != '\n') && (c != '\r')))
	    ComBuf += (char)c;
	  while((c != EOF) && ((c != '\n') && (c != '\r'))) {
	    if((c == ' ') || (c == '\t')) {
	      c = skipwhite_nocr();
	    }
	    else
	      c = Getc();
	    if((c != EOF) && ((c != '\n') && (c != '\r')))
	      ComBuf += (char)c;
	  }
	}
	else {
	  c = skipwhite();
	  if(c != EOF)
	    ComBuf += (char)c;
	  while(c != EOF) {
	    if((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r')) {
	      c = skipwhite();
	    }
	    else
	      c = Getc();
	    if(c == '*') {
	      if(Peekc() == '/') {
		c = Getc();
		break;
	      }
	    }
	    if(c != EOF) {
	      if ((c == '\n') || (c == '\r') || (c == '\t'))
		ComBuf += ' ';
	      else
		ComBuf += (char)c;
	    }
	  }
	}

	if(state == Find_Item) {
	  String tmp = ComBuf;
	  if(tmp.contains("#REG_FUN")) { // function follows comment
	    PushState(Parse_fundef);
	    return MP_REGFUN;
	  }
	}
	if((state == Find_Item) || (state == Parse_infun))
	  continue;
	yylval.chr = ComBuf;
	return MP_COMMENT;
      }
      else {
	c = '/';
      }
    }

    if(c == '#') {
      c = skipwhite();
      c = readword(c);
      if((LexBuf == "pragma") || (LexBuf == "ident")) {
	c = skipline();
	continue;
      }
      // note: following for MS VC++
      // VC++ outputs "#line xxx ..." where xxx is lineno
      // gcc simply outputs "# xxx ..." where xxx is lineno
      if ((LexBuf == "line") ) {
        c = skipwhite();
	c = readword(c);
      }

      line = (int)atol(LexBuf); // the line number
      if ((c != '\n') && (c != '\r'))		// readword did not Getc() this char
	c = skipwhite_nocr();
      if ((c == '\n') || (c == '\r'))		// ignore #<lineno><RETURN> directives
	continue;
      else if(c != '\"') {	// "
	cerr << "E!!: Directive Not Recognized: " << LexBuf << (char)c << "\n";
	continue;
      }
      c = readfilename(c);
      // ignore gcc stuff like "<built-in>"
      if (LexBuf.startsWith('<')) {
        if ((c != '\n') && (c != '\r'))
          skipline();
        continue;
      }
      cur_fname = lexCanonical(LexBuf);
      if ((c != '\n') && (c != '\r'))
	skipline();		// might be training stuff to skip here

      cur_fname_only = taMisc::GetFileFmPath(cur_fname);
      if(cur_fname_only == trg_fname_only) {
        cur_is_trg = true;
      }
      else {
        cur_is_trg = false;
      }

      if(verbose >= 1) {
	cout << "file: " << cur_fname << "\n";
      }
      continue;			// now actually parse something new..
    }

    if(state == Find_Item) {
      if(readword(c) == EOF)
	return YYRet_Exit;
      if((itm = spc_keywords.FindName(LexBuf)) != NULL) {
        // only pay attention to interesting items here!
        switch(itm->idx) {
        case MP_TYPEDEF:
        case MP_CLASS:     
        // case MP_STRUCT:    
        // case MP_UNION:     
        case MP_TEMPLATE:  
        // case MP_USING:     
        case MP_NAMESPACE:
          yylval.typ = itm;
          PushState(Found_Item);
          return itm->idx;
          break;
        case MP_ENUM:      
          yylval.typ = itm;
          PushState(Parse_enum); // needs this right away for parsing
          return itm->idx;
          break;
        }
      }
      last_word = LexBuf;
      continue;
    }

    // eliminate any FUNCTION defns inline
    if(state == Parse_infun) {
      if(c == '{')
	bdepth++;
      else if(c == '}') {
	bdepth--;
	if(bdepth == 0) {
	  PopState();
	  c = skipwhite_peek();	// only peek at nonwhite stuff
	  if(c == ';')		// and get trailing semi
	    Getc();
	  return MP_FUNCTION;
	}
      }
      else if(readword(c) == EOF) {
        yyerror("premature end of file");
	return YYRet_Exit;
      }
      continue;
    }

    if((state == Parse_inclass || state == Parse_fundef) && (c == '{')) {
      bdepth = 1;
      PushState(Parse_infun);
      continue;
    }

    // a complex expression we want to avoid
    if(state == Parse_inexpr) {
      if(c == '(')
	bdepth++;
      else if(c == ')') {
	bdepth--;
	if(bdepth == 0) {
	  PopState();
	  return MP_FUNCTION;   // good enough
	}
      }
      else if(readword(c) == EOF) {
        yyerror("premature end of file");
	return YYRet_Exit;
      }
      continue;
    }

    if((state == Parse_enum) && (c == '(')) {
      bdepth = 1;
      PushState(Parse_inexpr);
      continue;
    }
    
    // get rid of things after equals signs inside of classes (always return EQUALS)
    if(c == '=') {
      if((state != Parse_enum) && !in_templ_pars) {
	bdepth = 0;
	EqualsBuf = "";
	do {
	  c = Peekc();
	  if(((bdepth == 0) && ((c == ',') || (c == ')') || (c == '>'))) || (c == ';')) {
	    yylval.chr = EqualsBuf;
	    return MP_EQUALS;
	  }
	  Getc();
	  if ((c != '"') && (c != '\n') && (c != '\r'))
	    EqualsBuf += (char)c;
	  if(c == ')' || c == '>')	bdepth--;
	  if(c == '(' || c == '<')	bdepth++;
	} while (c != EOF);
	return YYRet_Exit;
      }
      yylval.chr = "";
      return MP_EQUALS;
    }

    if(c == '[') {
      bdepth = 0;
      do {
	c = Getc();
	if(((bdepth == 0) && (c == ']')) || (c == ';'))
	  return MP_ARRAY;
	if(c == ')')	bdepth--;
        if(c == '(')	bdepth++;
	if(c == ']')	bdepth--;
        if(c == '[')	bdepth++;
      } while (c != EOF);
      return YYRet_Exit;
    }

    if((state == Parse_inclass) && (thisname) && 
	(constcoln) && (c == '(')) {
      bdepth = 0;
      do {
	c = Getc();
	if(((bdepth == 0) && (c == ')')) || (c == ';'))
	  return MP_FUNCALL;
	if(c == ')')	bdepth--;
        if(c == '(')	bdepth++;
      } while (c != EOF);
      return YYRet_Exit;
    }

    if(c == '.') {
      nxt = Peekc();
      if(!(isdigit(nxt)))
	return '.';		// pointsat
    }

    if((c == '.') || isdigit(c) || (c == '-')) {	// number
      int iv, gotreal = 0;
      LexBuf = (char)c;
      if(c == '.') gotreal = 1;
      
      while(((c=Peekc()) != EOF) &&
	    ((c == '.') || isxdigit(c) || (c == 'x') || (c == 'e') || (c == '-') ||
	     (c == 'X') || (c == 'E')))	{
	LexBuf += (char)c; if(c == '.') gotreal = 1;
	Getc();
      } 

      iv = (int)strtol(LexBuf, NULL, 0);	// only care about ints
      yylval.rval = iv;
      return MP_NUMBER;
    }
    
    if(isalpha(c) || (c == '_')) {
      LexBuf = (char)c;

      while((c=Peekc()) != EOF && (isalnum(c) || (c == '_'))) {
	LexBuf += (char)c;
	Getc();
      } 

      int lx_tok;
      yylval.typ = FindName(LexBuf, lx_tok);
      
      if(yylval.typ == NULL) {
	yylval.chr = LexBuf;
	return MP_NAME;
      }
      if(lx_tok == MP_OPERATOR) {
	while((c=Peekc()) != '(' && (c != ';')) Getc(); // skip to the parens -- also ; terminates
      }
      if(lx_tok == MP_ENUM) {   // must do this right away else enums get eaten by function slurper!
        PushState(Parse_enum);
      }
      return lx_tok;
    }

    if(c == ':') return follow(':', MP_SCOPER, ':');
    return c;

  } while(1);

  return 0;
}
