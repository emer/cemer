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


// maketa: Automatic C Definition Access

#ifndef maketa_h
#define maketa_h 1

#include <TypeDef>
#include <MethodDef>
#include <MemberDef>
#include <PropertyDef>
#include <EnumDef>
#include <taHashTable>

#ifndef TYPE
#include "mta_parse.h"
#endif

using namespace std;

int yyparse(void);
void yyerror(const char *s);

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

  TypeSpace	spc_keywords;	// holds some key words for searching

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
  bool		gen_instances;	// generate instances?
  bool		gen_doc;	// generate docs
  bool		class_only;	// parse structs of type "class" only (not struct or union)
  int		verbose;	// level of verbosity
#ifdef TA_OS_WIN
  bool		win_dll;	// if true, use the XXX_API macro for dll linkage
  String	win_dll_str;	// when using win_dll, the macro to use, 
#endif
  int		hash_size;	// hash_table size (default 2000)

  String	trg_header;	// header file to process, full path as provided
  String	trg_fname_only; // target header -- file name only
  String	out_fname;	// output file name to generate maketa type info into
  String        tmp_fname;      // temporary file name for cpp output

  String_PArray	paths;		// paths we check, in order (have final sep)

  int		st_line;	// starting line
  int		st_col;		// column
  int	        strm_pos;	// current stream position (don't use tellg!!)
  int	        st_pos;		// starting (of parse) stream position
  int	        st_line_pos;	// start of line position
  int		line;		// current parsing line
  int		col;		// current parsing column
  String	cur_fname;	// current file name being processed
  String        cur_fname_only; // only file name of current file
  bool          cur_is_trg;     // current filename is target file during parsing
  int		anon_no;	// anonymous type number
  int		defn_st_line;	// starting line of current definition, possibly..

  String        file_str;       // loads entire file in at once as a string and process from there
  States	state;
  YY_Flags	yy_state;	// parser state

  MTA();
  virtual ~MTA();

  int           Main(int argc, char* argv[]);
  // the main function that does everything -- called by overall main

  void		InitKeyWords();
  void		BuildHashTables();
  void 		Burp();
  void          SetSource(TypeDef* td, bool use_defn_st_line);
  // set the source_file and source_start values in type to current vals, optionally using defn_st_line
  void		Class_ResetCurPtrs();
  // reset pointers
  void		Class_UpdateLastPtrs();
  // update last_memb, etc ptrs

  TypeSpace*	GetTypeSpace(TypeDef* td);
  // get appropriate type space for adding a new type derived from this one
  void          FixClassTypes(TypeDef* td);
  // make sure the class type settings are correct for an actual class obj

  void		TypeAdded(const char* typ, TypeSpace* sp, TypeDef* td);
  // report that type was added if verbose is on
  void		TypeNotAdded(const char* typ, TypeSpace* sp, TypeDef* ext_td,
			     TypeDef* new_td);
  // report that type new_td was NOT added because of existing type ext_td (if verbose is on)

  void 		SetDesc(const char* comnt, String& desc, String_PArray& inh_opts,
			String_PArray& opts, String_PArray& lists);
  // use for setting options & lists from user-supplied comments

  TypeDef* 	FindName(const char* nm, int& lex_token);
  // search all the relevant lists for given name, return lex_token of that item

  String	FindFile(const String& fname, bool& ok);
  // find the file, searching on .path if needed; returns full LexCanonical fname, clears ok if not found (fname can have full path)

  // LEX functions (defined in mta_lex.cc)
  String	LexBuf;
  String	ComBuf;
  String	EqualsBuf;
  String	last_word;

  int	Getc();
  int	Peekc();
  void	unGetc(int c);
  int	skipwhite();
  int	skipwhite_peek();
  int	skipwhite_nocr();
  int	skipline();
  int   skiplines(int n_lines);
  int	skiptocommarb();	// skip to comma or right bracket
  int	readword(int c);
  int	readfilename(int c); // read an optionally quoted filename, pass first char (ex ") in c; returns fname without quotes; quotes enables spaces, else ws terminates
  int	follow(int expect, int ifyes, int ifno);
  int	lex();
  static String  lexCanonical(const String& in);
  // canonicalize the path for lexing

public:

//////////////////////////////////
// 	  _TA.cc File		//
//////////////////////////////////

  bool TypeDef_Generate_Test(TypeDef* ths);
  // test if this type should be generated or not

  void TypeSpace_Generate(TypeSpace* ths, ostream& strm);

//////////////////////////////////
// 	TypeDef Constructors	//
//////////////////////////////////
// (part 1 of _TA.cc file)

  void TypeSpace_Generate_Types(TypeSpace* ths, ostream& strm);
  void TypeDef_FixOpts(String_PArray& op);
  void TypeDef_Generate_Types(TypeDef* ths, ostream& strm);


//////////////////////////////////
//   Type Instances & stubs	//
//////////////////////////////////
// (part 2 of _TA.cc file)

  void TypeSpace_Generate_Instances(TypeSpace* ths, ostream& strm);
  void TypeDef_Generate_Instances(TypeDef* ths, ostream& strm);

//////////////////////////////////
// 	  Init Function		//
//////////////////////////////////
// (part 4 of _TA.cc file)

  void TypeSpace_Generate_Init(TypeSpace* ths, ostream& strm);

//////////////////////////////////
// 	  GenDoc		//
//////////////////////////////////

  void GenDoc(TypeSpace* ths, fstream& strm);
  bool TypeDef_Filter_Type(TypeDef* td, TypeSpace* ts);
  String_PArray* TypeDef_Get_Parents(TypeDef* td, String_PArray* bp);
};


extern MTA* mta;

#endif // maketa_h
 
