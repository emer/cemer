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

#ifndef MP_TYPE
#include "mta_parse.h"
#endif

using namespace std;

int yyparse(void);
void yyerror(const char *s);

class MTA {
public:
  enum States {
    Find_Item,			// looking for something interesting to process: class, enum, typedef
    Found_Item,                 // found some kind of interesting item to process -- lexer now active and returns stuff..
    Parse_typedef,              // parsing a typedef
    Parse_enum,                 // parsing an enum
    Parse_class,                // parsing a class header -- inheritance, etc
    Parse_inclass,		// parsing inside the class defn
    Parse_infun,		// parsing inside a fun defn
    Parse_fundef, 		// parsing a REG_FUN or other such function defn
    Parse_inexpr,		// parsing inside an expression that is too complex for us
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

  String 	LastLn;	        // last line parsed

  TypeSpace	spc_keywords;	// holds some key words for searching
  TypeSpace	spc_typedef_gen; // space of types to generate typedefs for because they are referred to in methods, etc

  States	state;          // current state -- read-only: use push/pop interface to set
  YY_Flags	yy_state;	// parser state
  TypeSpace	type_stack;	// for storing names, etc.
  EnumSpace	enum_stack;	// for storing names, etc.
  MemberSpace	memb_stack;	// for storing names, etc.
  MethodSpace	meth_stack;	// for storing names, etc.
  String_PArray namespc_stack;  // stack of name spaces
  String_PArray cur_namespcs;   // name spaces during parsing
  int_PArray    state_stack;    // stack of State states
  TypeSpace	class_stack;	// stack of classes -- can be nested..
  int_PArray    class_mstate_stack; // stack of MembState states for classes

  TypeDef*	cur_enum;	// holds current enum
  TypeDef*	cur_class;	// holds current class -- read-only -- use push/pop to update
  MembState 	cur_mstate;	// current member state
  MemberDef*	cur_memb;	// holds current memberdef
  TypeDef*	cur_memb_type;	// current member type
  MethodDef*	cur_meth;	// holds current methoddef
  TypeSpace	cur_templ_pars;	// current template parameters
  TypeSpace	cur_templ_defs;	// current template defaults
  TypeSpace	cur_typ_templ_pars; // current template parameters for a type, not a defn
  MemberDef*	last_memb;	// holds previous memberdef
  MethodDef*	last_meth;	// holds previous methoddef
  String        cur_nmspc_tmp;  // temporary current namespace during parsing
  bool		thisname;	// true if currently doing a "thisname" operation
  bool		constcoln;	// true if a constructor colon was encountered
  bool		burp_fundefn;	// true if needs to burp after parsing fundefn
  bool          in_templ_pars;   // currently parsing template parameters -- special case..

  bool		gen_css;	// generate css stuff?
  bool		gen_instances;	// generate instances?
  bool		gen_doc;	// generate docs
  int		verbose;	// level of verbosity
  bool          v_trg_only;     // verbosity only on for target header (cur_is_trg)
  bool          filter_errs;    // filter error messages just like others (otherwise always show)
  bool          filter_warns;   // filter warning messages just like others (otherwise always show)
  bool          dbg_constr;     // provide debugging on construction side (else just parsing)
  String        v_src_trg;      // if non-empty, verbosity only on for this header file (file name only)

#ifdef TA_OS_WIN
  bool		win_dll;	// if true, use the XXX_API macro for dll linkage
  String	win_dll_str;	// when using win_dll, the macro to use, 
#endif
  int		hash_size;	// hash_table size (default 2000)

  String	trg_header;	// header file to process, full path as provided
  String	trg_fname_only; // target header -- file name only
  String	trg_basename;   // basic unique name for target -- 
  String	out_fname;	// output file name to generate maketa type info into
  String        tmp_fname;      // temporary file name for cpp output

  String_PArray	paths;		// paths we check, in order (have final sep)

  int		st_line;	// starting line
  int		st_col;		// column
  int	        strm_pos;	// current stream position
  int	        st_pos;		// starting (of parse) stream position
  int	        st_line_pos;	// start of line position
  int		line;		// current parsing line
  int		col;		// current parsing column
  String	cur_fname;	// current file name being processed
  String        cur_fname_only; // only file name of current file
  bool          cur_is_trg;     // current filename is target file during parsing
  bool          ta_lib;         // this file is in the ta library -- windows needs to know.
  int		anon_no;	// anonymous type number
  int		defn_st_line;	// starting line of current definition, possibly..

  String        file_str;       // loads entire file in at once as a string and process from there

  MTA();
  virtual ~MTA();

  int           Main(int argc, char* argv[]);
  // the main function that does everything -- called by overall main

  void		InitKeyWords();
  // initialize keyword lookup table
  void		BuildHashTables();
  // build hash tables at startup

  bool		VerboseCheckTrg();
  // check if current file is target for verbose output

  void          Info(int v_level, const char* a, const char* b=0, const char* c=0,
                     const char* d=0, const char* e=0, const char* f=0,
                     const char* g=0, const char* h=0, const char* i=0);
  // displays informational message, subject to relevant filtering, including target verbosity level as given (0 = always, etc..)
  void          Warning(int v_level, const char* a, const char* b=0, const char* c=0,
                     const char* d=0, const char* e=0, const char* f=0,
                     const char* g=0, const char* h=0, const char* i=0);
  // displays warning, subject to relevant filtering, including target verbosity level as given (0 = always, etc..)
  void          Error(int v_level, const char* a, const char* b=0, const char* c=0,
                     const char* d=0, const char* e=0, const char* f=0,
                     const char* g=0, const char* h=0, const char* i=0);
  // displays error, subject to relevant filtering, including target verbosity level as given (0 = always, etc..)

  void 		Burp();
  // un-read previous input -- for look-ahead parsing
  void          PushState(States new_state);
  // push given state onto state stack and set current state variable
  States        PopState();
  // pop state to prior state, returning old state and setting current state to previous
  void          ResetState();
  // reset back to starting Find_Item state and reset all other parsing vars, including class stack, and everything except namespace stack..
  void          PushClass(TypeDef* new_class, MembState memb_state);
  // push new class and member state on stack -- becomes the current class
  TypeDef*      PopClass();
  // pop current class off the stack and return, previous becomes new current class, along with its previous member state
  void          ResetClassStack();
  // reset class stack back to empty
  void          SetSource(TypeDef* td, bool use_defn_st_line);
  // set the source_file and source_start values in type to current vals, optionally using defn_st_line
  void          ClearSource(TypeDef* td);
  // clear the source_file and source_start values from type -- premature..
  void		Class_ResetCurPtrs();
  // reset pointers
  void		Class_UpdateLastPtrs();
  // update last_memb, etc ptrs
  void          Namespc_PushNew(const char* spc);
  // push a new namespace on the stack
  void          Namespc_Pop();
  // pop off namespace
  void          StartTemplPars();
  // start defining template parameters
  void          EndTemplPars();
  // done defining template parameters

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

  bool          TypeSpace_Sort_Order(TypeSpace* ths);
  // sort the order of the types so children come after parents..

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

  ///////////////////////////////////////////////////////////////////
  //            Construct Types
  //            the following are all defined in mta_constr.cpp

  /////////////////////////
  //    global helpers

  bool TypeDef_Gen_Test(TypeDef* ths);
  // test if this type should be generated or not
  String TypeDef_Gen_TypeName(TypeDef* ths);
  // generate the type name as a string for string lookup in Data fields
  String TypeDef_Gen_TypeDef_Ptr(TypeDef* ths);
  // generate a string expression for a pointer to this typedef -- e.g., &TA_taBase
  // deals with issues on on windows, etc
  String TypeDef_Gen_TypeDef_Ptr_impl(TypeDef* ths);
  // generate a reference to this typedef -- helper impl
  String TypeDef_Gen_TypeDef_Ptr_Path(TypeDef* ths);
  // generate a reference to this typedef -- helper includes path based on subtype
  void TypeDef_FixOpts(String_PArray& op);
  // fix options
  String VariantToTargetConversion(TypeDef* param_td);
  // convert variant to target typedef -- for propstubs

  //////////////////////////////////////////
  // 	Overall Gen Entry Point

  void TypeSpace_Gen(TypeSpace* ths, ostream& strm);
  // top-level entry point to generating the output -- calls everything below


  //////////////////////////////////////////
  // 	Includes

  void TypeSpace_Includes(TypeSpace* ths, ostream& strm, bool instances=false);
  // top-level generate Includes

  //////////////////////////////////////////
  // 	Type Instances

  void TypeSpace_Gen_Instances(TypeSpace* ths, ostream& strm);
  // top-level generate Instances
  void TypeDef_Gen_Instances(TypeDef* ths, ostream& strm);

  //////////////////////////////////////////
  // 	TypeDef constructors 

  void TypeSpace_Gen_TypeDefs(TypeSpace* ths, ostream& strm);
  // top-level generate TypeDef constructors
  void TypeDef_Gen_TypeDefs(TypeDef* ths, ostream& strm);
  void TypeDef_Gen_TypeDefs_impl(TypeDef* ths, ostream& strm);

  /////////////////////////////////////////
  //   css method stubs	

  void TypeSpace_Gen_Stubs(TypeSpace* ths, ostream& strm);
  // top-level generate css stubs
  void TypeDef_Gen_Stubs(TypeDef* ths, ostream& strm, bool add_typedefs = false);

  void TypeSpace_Gen_TypeDefOf(TypeSpace* ths, ostream& strm);
  // generate TypeDef_Of() for all items on the list
  void TypeDef_Gen_TypeDefOf(TypeDef* ths, ostream& strm);

  void MethodSpace_Gen_Stubs(MethodSpace* ths, TypeDef* ownr, ostream& strm,
                                  bool add_typedefs = false);

  void MethodDef_InitTempArgVars(MethodDef* md, ostream& strm, int act_argc,
                                 bool add_typedefs = false);
  void MethodDef_AssgnTempArgVars(TypeDef* ownr, MethodDef* md, ostream& strm,
                                  int act_argc);
  String MethodDef_GetCSSType(TypeDef* td);
  void MethodDef_GenArgCast(MethodDef* md, TypeDef* argt, int j, ostream& strm,
                            bool add_typedefs = false);
  void MethodDef_GenArgs(MethodDef* md, ostream& strm, int act_argc,
                         bool add_typedefs = false);
  void MethodDef_GenStubName(TypeDef* ownr, MethodDef* md, ostream& strm);
  void MethodDef_GenStubCall(TypeDef* ownr, MethodDef* md, ostream& strm);
  void MethodDef_GenFunCall(TypeDef* ownr, MethodDef* md, ostream& strm,
                            int act_argc, bool add_typedefs = false);

  void MemberSpace_Gen_PropStubs(MemberSpace* ths, TypeDef* ownr, ostream& strm);
  void MethodSpace_Gen_PropStubs(MethodSpace* ths, TypeDef* ownr, ostream& strm);


  ///////////////////////////////////////////
  //   Data generation: Enum, Member, Method, Property

  void TypeSpace_Gen_Data(TypeSpace* ths, ostream& strm);
  // top-level generate data
  void TypeDef_Gen_Data(TypeDef* ths, ostream& strm);

  //////////////////////////////////
  //   	     Enum Data

  void TypeDef_Gen_EnumData(TypeDef* ths, ostream& strm);
  void TypeDef_Init_EnumData(TypeDef* ths, ostream& strm);

  void EnumSpace_Gen_Data(EnumSpace* ths, ostream& strm);


  //////////////////////////////////
  // 	   Member Data

  void TypeDef_Gen_MemberData(TypeDef* ths, ostream& strm);
  void TypeDef_Init_MemberData(TypeDef* ths, ostream& strm);

  bool MemberSpace_Filter_Member(MemberSpace* ths, MemberDef* md);
  void MemberSpace_Gen_Data(MemberSpace* ths, TypeDef* ownr, ostream& strm);


  //////////////////////////////////
  // 	   Method Data

  void TypeDef_Gen_MethodData(TypeDef* ths, ostream& strm);
  void TypeDef_Init_MethodData(TypeDef* ths, ostream& strm);

  bool MethodSpace_Filter_Method(MethodSpace* ths, MethodDef* md);

  void MethodSpace_Gen_ArgData(MethodSpace* ths, TypeDef* ownr, ostream& strm);
  void MethodDef_Gen_ArgData(MethodDef* ths, TypeDef* ownr, ostream& strm);

  void MethodSpace_Gen_Data(MethodSpace* ths, TypeDef* ownr, ostream& strm);


  //////////////////////////////////
  // 	   Property Data

  void TypeDef_Gen_PropertyData(TypeDef* ths, ostream& strm);
  void TypeDef_Init_PropertyData(TypeDef* ths, ostream& strm);

  bool PropertySpace_Filter_Property(PropertySpace* ths, PropertyDef* md);
  void PropertySpace_Gen_Data(PropertySpace* ths, TypeDef* ownr, ostream& strm);


  //////////////////////////////////
  // 	  TypeInit Function

  void TypeSpace_Gen_TypeInit(TypeSpace* ths, ostream& strm);
  void TypeDef_Gen_TypeInit(TypeDef* ths, ostream& strm);

  //////////////////////////////////
  // 	  DataInit Function

  void TypeSpace_Gen_DataInit(TypeSpace* ths, ostream& strm);
  void TypeDef_Gen_DataInit(TypeDef* ths, ostream& strm);

  void TypeDef_Gen_AddParents(TypeDef* ths, char* typ_ref, ostream& strm);
  void TypeDef_Gen_AddAllParents(TypeDef* ths, char* typ_ref, ostream& strm);
  void TypeDef_Gen_AddOtherParents(TypeDef* ths, char* typ_ref, ostream& strm);
  void SubTypeSpace_Gen_Init(TypeSpace* ths, TypeDef* ownr, ostream& strm);

  //////////////////////////////////
  // 	  InstInit Function

  void TypeSpace_Gen_InstInit(TypeSpace* ths, ostream& strm);
  void TypeDef_Gen_InstInit(TypeDef* ths, ostream& strm);

  //////////////////////////////////
  // 	  GenDoc (not currently used)

  void GenDoc(TypeSpace* ths, fstream& strm);
  bool TypeDef_Filter_Type(TypeDef* td, TypeSpace* ts);
  String_PArray* TypeDef_Get_Parents(TypeDef* td, String_PArray* bp);
};


extern MTA* mta;

#endif // maketa_h
 
