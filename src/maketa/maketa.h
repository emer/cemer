/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
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
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// maketa: Automatic C Definition Access

#ifndef maketa_h
#define maketa_h 1

class TypeDef;

extern TypeDef TA_void;
extern TypeDef TA_int;
extern TypeDef TA_long;
extern TypeDef TA_short;
extern TypeDef TA_char;
extern TypeDef TA_unsigned;
extern TypeDef TA_signed;
extern TypeDef TA_float;
extern TypeDef TA_double;
extern TypeDef TA_bool;
extern TypeDef TA_const;	// const is not formal...
extern TypeDef TA_enum;		// par_formal
extern TypeDef TA_struct;	// par_formal
extern TypeDef TA_union;	// par_formal
extern TypeDef TA_class;	// par_formal
extern TypeDef TA_template;	// par_formal
extern TypeDef TA_templ_inst;	// par_formal template instantiation
extern TypeDef TA_ta_array;	// par_formal (indicates "arrayness")
extern TypeDef TA_taBase;
extern TypeDef TA_taRegFun;	// registered functions
extern TypeDef TA_TypeDef;	// give these to the user...
extern TypeDef TA_MemberDef;
extern TypeDef TA_MethodDef;
extern TypeDef TA_ta_Globals;
extern TypeDef TA_taString;
extern TypeDef TA_taSubString;
extern TypeDef TA_void_ptr;

#include "ta_type.h"

#ifndef TYPE
#include "mta_parse.h"
#endif

int yyparse(void);
void yyerror(char *s);

class MTA {
public:
  enum States {
    Find_Item,			// looking for a class, enum, typedef
    Skip_File,
    Parse_typedef,
    Parse_enum,
    Parse_class,
    Parse_inclass,		// inside the class defn
    Parse_infun,		// inside a fun defn
    Parse_fundef 		// parse a REG_FUN function defn
  };

  enum MembState {		// to maintain state about members (don't add privats)
    prvt,
    pblc,
    prot
  };

  enum YY_Flags {
    YYRet_Exit	= 0,	// script is done being parsed
    YYRet_Ok	= 1,	// everything is fine
    YYRet_NoSrc	= -2,	// don't code last line as source
    YYRet_Err	= -3,	// error
    YYRet_Blank = -4,	// blank line
    YYRet_Parse = -5 	// need to parse more
  };

  static char 	LastLn[8192];	// last line parsed

  TypeSpace* 	spc;		// holds general defns
  TypeSpace	spc_target;	// holds the defns for target header files
  TypeSpace	spc_other;	// holds defns for non-target headers
  TypeSpace	spc_extern;	// external defs
  TypeSpace	spc_ignore;	// put ignored types here
  TypeSpace	spc_pre_parse;	// pre-parsed but not-yet reparsed things here
  TypeSpace	spc_keywords;	// holds some key words for searching
  TypeSpace	spc_builtin;	// holds builtin types (which are always available)

  TypeSpace	type_stack;	// for storing names, etc.
  EnumSpace	enum_stack;	// for storing names, etc.
  MemberSpace	memb_stack;	// for storing names, etc.
  MethodSpace	meth_stack;	// for storing names, etc.

  TypeDef*	cur_enum;	// holds current enum
  TypeDef*	cur_class;	// holds current class
  TypeDef*	last_class;	// holds previous class
  MembState 	cur_mstate;	// current member state
  MemberDef*	cur_memb;	// holds current memberdef
  TypeDef*	cur_memb_type;	// current member type
  MethodDef*	cur_meth;	// holds current methoddef
  TypeSpace	cur_templ_pars;	// current template parameters
  MemberDef*	last_memb;	// holds previous memberdef
  MethodDef*	last_meth;	// holds previous methoddef

  bool		thisname;	// true if currently doing a "thisname" operation
  bool		constcoln;	// true if a constructor colon was encountered
  bool		burp_fundefn;	// true if needs to burp after parsing fundefn
  bool		gen_css;	// generate css stuff?
  bool		gen_iv;		// generate iv stuff (currently not used)
  bool		gen_instances;	// generate instances?
  bool		make_hx;	// make an hx file instead of an h file
  bool		class_only;	// parse structs of type "class" only (not struct or union)
  bool		old_cfront;	// support old cfront member pointer initializer
  int		verbose;	// level of verbosity
  int		hash_size;	// hash_table size (default 2000)

  String	fname;		// file name
  String	basename;	// base file name
  String	ta_type_h;	// header output name
  String	ta_inst_h;	// header output name
  String	ta_ccname;	// header output name

  String_PArray	pre_parse_inits; // init commands to be called for pre-parsed files
  String_PArray	included;	// files already processed as includes, no need to rpt
  String_PArray	tmp_include;	// temp holder

  String_PArray	headv;		// list of header files
  String_PArray	head_fn_only; 	// only the file names of the headers

  int		st_line;	// starting line
  int		st_col;		// column
  streampos	strm_pos;	// current stream position (don't use tellg!!)
  streampos	st_pos;		// starting (of parse) stream position
  streampos	st_line_pos;	// start of line position
  int		line;
  int		col;
  String	cur_fname;	// current file name being processed
  int		anon_no;	// anonymous type number

  fstream	fh;
  States	state;
  YY_Flags	yy_state;	// parser state

  MTA();
  virtual ~MTA();

  void		InitKeyWords();
  void		InitTypeSpace(TypeSpace& ts);
  void		BuildHashTables();
  void 		Burp();
  void		Class_ResetCurPtrs();
  // reset pointers
  void		Class_UpdateLastPtrs();
  // update last_memb, etc ptrs

  TypeSpace*	GetTypeSpace(TypeDef* td);
  // get appropriate type space for adding a new type derived from this one

  void		TypeAdded(const char* typ, TypeSpace* sp, TypeDef* td);
  // report that type was added if verbose is on

  void 		SetDesc(const char* comnt, String& desc, String_PArray& inh_opts,
			String_PArray& opts, String_PArray& lists);
  // use for setting options & lists from user-supplied comments

  TypeDef* 	FindName(const char* nm, int& lex_token);
  // search all the relevant lists for given name, return lex_token of that item

  void		SetPreParseFlag(TypeSpace& spc, TypeSpace& pplist);
  // set pre-parse flag for all types in spc that are on the pplist

  // LEX functions (defined in mta_lex.cc)
  String	LexBuf;
  String	ComBuf;
  String	EqualsBuf;
  String	last_word;

  int	Getc();
  int	Peekc() { return fh.peek(); }
  void	unGetc(int c);
  int	skipwhite();
  int	skipwhite_peek();
  int	skipwhite_nocr();
  int	skipline();
  int	skiptocommarb();	// skip to comma or right bracket
  int	readword(int c);
  int	readfilename(int c);
  int	follow(int expect, int ifyes, int ifno);
  int	lex();
};


extern MTA* mta;

#endif // maketa_h
