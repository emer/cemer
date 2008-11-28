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


// maketa: Make TypeAccess Data Structures

#include "maketa.h"
#include "mta_constr.h"
#include "ta_platform.h"
#include "ta_variant.h"

#include <signal.h>
//nn #include <malloc.h>
#ifdef TA_OS_WIN
//#include "stdafx.h"
#endif
//NOTE on TypeDef.size -- size is irrelevant when building maketa, and is basically inaccessible
// when running maketa -- therefore, the only thing we use sz for in building maketa is to
// control how we want size information generated -- the following are the codes:
// sz=0: don't generate sizeof info, unless a real class
// sz=1: generate sizeof info, ex. "sizeof(int)"

//                               s nm, b intrnl, i ptrs, b ref, b forml, b global, u sz, s c_nm
TypeDef TA_void			("void", 	1, 0, 0, 0, 1);
TypeDef TA_char			("char", 	1, 0, 0, 0, 1, 1, "char");
TypeDef TA_signed_char		("signed_char", 1, 0, 0, 0, 1, 1, "signed char");
TypeDef TA_int8_t		("int8_t",      1, 0, 0, 0, 1, 1);//note, derives char on MSVC
TypeDef TA_unsigned_char      ("unsigned_char", 1, 0, 0, 0, 1, 1, "unsigned char");
TypeDef TA_uint8_t		("uint8_t",     1, 0, 0, 0, 1, 1);//note, derives char on MSVC
TypeDef TA_short		("short", 	1, 0, 0, 0, 1, 1, "short");
TypeDef TA_signed_short	       ("signed_short", 1, 0, 0, 0, 1, 1, "signed short");
TypeDef TA_short_int	       ("short_int"   , 1, 0, 0, 0, 1, 1, "short int");
TypeDef TA_signed_short_int("signed_short_int", 1, 0, 0, 0, 1, 1, "signed short int");
TypeDef TA_int16_t		("int16_t",     1, 0, 0, 0, 1, 1);
TypeDef TA_unsigned_short    ("unsigned_short", 1, 0, 0, 0, 1, 1, "unsigned short");
TypeDef TA_unsigned_short_int("unsigned_short_int", 1, 0, 0, 0, 1, 1, "unsigned short int");
TypeDef TA_uint16_t		("uint16_t",    1, 0, 0, 0, 1, 1);
TypeDef TA_int			("int", 	1, 0, 0, 0, 1, 1, "int");
TypeDef TA_signed_int		("signed_int", 	1, 0, 0, 0, 1, 1, "signed int");
TypeDef TA_signed		("signed", 	1, 0, 0, 0, 1, 1);
TypeDef TA_int32_t		("int32_t", 	1, 0, 0, 0, 1, 1);
TypeDef TA_unsigned_int	       ("unsigned_int", 1, 0, 0, 0, 1, 1, "unsigned int");
TypeDef TA_unsigned		("unsigned", 	1, 0, 0, 0, 1, 1);
TypeDef TA_uint		("uint", 	1, 0, 0, 0, 1, 1);
TypeDef TA_uint32_t		("uint32_t",    1, 0, 0, 0, 1, 1);
TypeDef TA_int64_t		("int64_t", 	1, 0, 0, 0, 1, 1);
TypeDef TA_long_long		("long_long", 	1, 0, 0, 0, 1, 1, "long long");
TypeDef TA_signed_long_long("signed_long_long", 1, 0, 0, 0, 1, 1, "signed long long");
TypeDef TA_uint64_t		("uint64_t", 	1, 0, 0, 0, 1, 1);
TypeDef 
    TA_unsigned_long_long("unsigned_long_long", 1, 0, 0, 0, 1, 1, "unsigned long long");
// all the long types will get parented to either int (typical) or int64 types
TypeDef TA_long			("long", 	1, 0, 0, 0, 1, 1); 
TypeDef TA_signed_long		("signed_long", 1, 0, 0, 0, 1, 1, "signed long");
TypeDef TA_long_int		("long_int", 1, 0, 0, 0, 1, 1, "long int");
TypeDef TA_signed_long_int	("signed_long_int", 1, 0, 0, 0, 1, 1, "signed long int");
TypeDef TA_unsigned_long	("unsigned_long", 1, 0, 0, 0, 1, 1, "unsigned long");
TypeDef TA_unsigned_long_int    ("unsigned_long_int", 1, 0, 0, 0, 1, 1, "unsigned long int");
TypeDef TA_ulong	("ulong", 1, 0, 0, 0, 1, 1, "ulong");
// (u)intptr_t gets parented to either (u)int or (u)int64
TypeDef TA_intptr_t		("intptr_t", 	1, 0, 0, 0, 1, 1);
TypeDef TA_uintptr_t		("uintptr_t", 	1, 0, 0, 0, 1, 1); 
TypeDef TA_float		("float", 	1, 0, 0, 0, 1, 1);
TypeDef TA_double		("double", 	1, 0, 0, 0, 1, 1);
TypeDef TA_bool			("bool", 	1, 0, 0, 0, 1, 1);
TypeDef TA_const		("const", 	1, 0, 0, 1, 1); // formal
TypeDef TA_enum			("enum", 	1, 0, 0, 1, 1); // formal
TypeDef TA_struct		("struct", 	1, 0, 0, 1, 1);	// formal
TypeDef TA_union		("union", 	1, 0, 0, 1, 1);	// formal
TypeDef TA_class		("class", 	1, 0, 0, 1, 1);	// formal
TypeDef TA_template		("template", 	1, 0, 0, 1, 1); // formal
TypeDef TA_templ_inst		("templ_inst", 	1, 0, 0, 1, 1);	// formal
TypeDef TA_ta_array		("ta_array", 	1, 0, 0, 1, 1);	// formal
TypeDef TA_taRegFun		("taRegFun", 	1, 0, 0, 0, 1); // pseudo formal, for global functions
TypeDef TA_TypeItem		("TypeItem", 	1, 0, 0, 0, 1, 1);
TypeDef TA_TypeDef		("TypeDef", 	1, 0, 0, 0, 1, 1);
TypeDef TA_EnumDef		("EnumDef", 	1, 0, 0, 0, 1, 1);
TypeDef TA_MemberDefBase	("MemberDefBase", 	1, 0, 0, 0, 1, 1);
TypeDef TA_MemberDef		("MemberDef", 	1, 0, 0, 0, 1, 1);
TypeDef TA_PropertyDef		("PropertyDef",	1, 0, 0, 0, 1, 1);
TypeDef TA_MethodDef		("MethodDef", 	1, 0, 0, 0, 1, 1);
TypeDef TA_void_ptr		("void_ptr", 	1, 1, 0, 1, 1, 1, "void*");

//                               s nm, s dsc, 
//                               s inop, s op, s lis, u siz, i ptrs, b ref, b global
//NOTE: the actual versions generated into the target code dynamically compute their size in the runtime
TypeDef TA_taString		("taString", "", "", "", "", 0, 0, 0, 1);
TypeDef TA_Variant		("Variant", "", "", "", "", 0, 0, 0, 1);
//NOTE: taBase is never actually encountered while building maketa, so its size is irrelevant...
// it is only here (and in ta_type.h header) because it is referenced in ta_type.cpp 
TypeDef TA_taBase("taBase", " Base type for all type-aware classes",
	"", "", "", 0, 0, 0, 1); 
	  
extern int yydebug;
extern "C" int getpid();
MTA* mta;		// holds mta

// copy the source to the destination, returning true if successful
bool copy_file(const char* src, const char* dst) {
  bool rval = false;
  fstream fsrc, fdst;
  fsrc.open(src, ios::in | ios::binary);
  if (!fsrc.is_open()) return false;
  fdst.open(dst, ios::out | ios::binary);
  if (!fdst.is_open()) goto exit1;
  char c1;
  // from stroustrup
  while (fsrc.get(c1)) fdst.put(c1);
  rval = true;
//exit2:
  fdst.close();
exit1:
  fsrc.close();
  return rval;
}

// returns 'true' if both files exist and are the same
bool files_same(const char* fname1, const char* fname2) {
  bool rval = false;
  fstream in1, in2;

  in1.open(fname1, ios::in | ios::binary);
  if (!in1.is_open()) goto exit2;
  in2.open(fname2, ios::in | ios::binary);
  if (!in2.is_open()) goto exit1;
  char c1;  char c2;
  bool g1;  bool g2;
  while (true) {
    g1 = in1.get(c1);
    g2 = in2.get(c2);
    if (!g1 && !g2) break; // same size, done 
    if (!(g1 && g2)) goto exit; // different sizes
    if (c1 != c2) goto exit;  // different content
  }
  rval = true; 

exit:
  in2.close();
exit1:
  in1.close();
exit2:
  return rval;
}

char MTA::LastLn[8192];

MTA::MTA() {
  spc = NULL;

  cur_enum = NULL;
  cur_class = NULL;
  last_class = NULL;
  cur_mstate = prvt;
  cur_memb = NULL;
  cur_memb_type = NULL;
  cur_meth = NULL;
  last_memb = NULL;
  last_meth = NULL;

  thisname = false;
  constcoln = false;
  burp_fundefn = false;
  gen_css = true;
  gen_instances = false;
  make_hx = false;
  auto_hx = false;
  class_only = true;
  verbose = 0;
#ifdef TA_OS_WIN
  win_dll = false;
#endif
  hash_size = 2000;

  st_line = 0;
  st_col = 0;
  strm_pos = 0;
  st_pos = 0;
  st_line_pos = 0;
  line = 0;
  col = 0;
  anon_no = 0;

  state = Find_Item;
  yy_state = YYRet_Ok;

  spc_other.name = "Types in Non-Target Header Files";
  spc_extern.name = "External Types Included";
  spc_ignore.name = "Types Ignored";
  spc_pre_parse.name = "Pre-Parsed Types Excluded";
  spc_keywords.name = "Key Words for Searching";
  spc_builtin.name = "Builtin Types";

  InitKeyWords();
  InitBuiltIn();

  InitTypeSpace(spc_other);	// when target not being searched (need base types)
  InitTypeSpace(spc_target);

  // only add a subset of things to spc_builtin
  TypeSpace& ts = spc_builtin;
  AddBuiltIn(ts);
}

MTA::~MTA() {
//   type_stack.Reset();
//   enum_stack.Reset();
//   memb_stack.Reset();
//   meth_stack.Reset();
//
//   spc_target.Reset();
//   spc_other.Reset();
//   spc_extern.Reset();
//   spc_ignore.Reset();
//   spc_pre_parse.Reset();
//   spc_keywords.Reset();
//   spc_builtin.Reset();

//  if(verbose < 1)	return;
//  cerr << "mta object destroyed\n";
}

void MTA::AddBuiltIn(TypeSpace& ts) { // common code
  ts.Add(&TA_void);
  ts.Add(&TA_char);
  ts.Add(&TA_signed_char);
  ts.Add(&TA_int8_t);
  ts.Add(&TA_unsigned_char);
  ts.Add(&TA_uint8_t);
  ts.Add(&TA_short);
  ts.Add(&TA_signed_short);
  ts.Add(&TA_short_int);
  ts.Add(&TA_signed_short_int);
  ts.Add(&TA_int16_t);
  ts.Add(&TA_unsigned_short);
  ts.Add(&TA_unsigned_short_int);
  ts.Add(&TA_uint16_t);
  ts.Add(&TA_int);
  ts.Add(&TA_signed_int);
  ts.Add(&TA_signed);
  ts.Add(&TA_int32_t);
  ts.Add(&TA_unsigned_int);
  ts.Add(&TA_unsigned);
  ts.Add(&TA_uint);
  ts.Add(&TA_uint32_t);
  ts.Add(&TA_int64_t);
  ts.Add(&TA_long_long);
  ts.Add(&TA_signed_long_long);
  ts.Add(&TA_uint64_t);
  ts.Add(&TA_unsigned_long_long);
  ts.Add(&TA_long);
  ts.Add(&TA_signed_long);
  ts.Add(&TA_long_int);
  ts.Add(&TA_signed_long_int);
  ts.Add(&TA_unsigned_long);
  ts.Add(&TA_unsigned_long_int);
  ts.Add(&TA_ulong);
  ts.Add(&TA_intptr_t);
  ts.Add(&TA_uintptr_t);
  ts.Add(&TA_float);
  ts.Add(&TA_double);
#ifndef NO_BUILTIN_BOOL
  ts.Add(&TA_bool);
#endif
  ts.Add(&TA_const);
  ts.Add(&TA_enum);
}

void MTA::InitKeyWords() {
  TypeDef* ky;
  ky = new TypeDef("typedef");  spc_keywords.Add(ky); ky->idx = TYPEDEF;
  ky = new TypeDef("class"); 	spc_keywords.Add(ky); ky->idx = CLASS;
  ky = new TypeDef("typename");	spc_keywords.Add(ky); ky->idx = TYPENAME;
  ky = new TypeDef("struct"); 	spc_keywords.Add(ky); ky->idx = STRUCT;
  ky = new TypeDef("union"); 	spc_keywords.Add(ky); ky->idx = UNION;
  ky = new TypeDef("template"); spc_keywords.Add(ky); ky->idx = TEMPLATE;
  ky = new TypeDef("enum"); 	spc_keywords.Add(ky); ky->idx = ENUM;
  ky = new TypeDef("TypeDef"); 	spc_keywords.Add(ky); ky->idx = TA_TYPEDEF;
  ky = new TypeDef("public"); 	spc_keywords.Add(ky); ky->idx = PUBLIC;
  ky = new TypeDef("private"); 	spc_keywords.Add(ky); ky->idx = PRIVATE;
  ky = new TypeDef("protected");spc_keywords.Add(ky); ky->idx = PROTECTED;
  ky = new TypeDef("inline"); 	spc_keywords.Add(ky); ky->idx = FUNTYPE;
  ky = new TypeDef("mutable"); 	spc_keywords.Add(ky); ky->idx = FUNTYPE;
  ky = new TypeDef("virtual"); 	spc_keywords.Add(ky); ky->idx = VIRTUAL;
  ky = new TypeDef("static"); 	spc_keywords.Add(ky); ky->idx = STATIC;
  ky = new TypeDef("operator");	spc_keywords.Add(ky); ky->idx = OPERATOR;
  ky = new TypeDef("friend"); 	spc_keywords.Add(ky); ky->idx = FRIEND;
  ky = new TypeDef("REG_FUN"); 	spc_keywords.Add(ky); ky->idx = REGFUN;
}

void MTA::InitBuiltIn() {
  TA_void_ptr.AddParents(&TA_void);
#if (defined(TA_OS_WIN) && defined(_MSC_VER))
  TA_int8_t.AddParents(&TA_char);
#else
  TA_int8_t.AddParents(&TA_signed_char);
#endif
  TA_uint8_t.AddParents(&TA_unsigned_char); // note: doesn't exist per se on MSVC
  TA_signed_short.AddParents(&TA_short);
  TA_short_int.AddParents(&TA_short);
  TA_signed_short_int.AddParents(&TA_short);
  TA_int16_t.AddParents(&TA_short);
  TA_unsigned_short_int.AddParents(&TA_unsigned_short);
  TA_uint16_t.AddParents(&TA_unsigned_short);
  TA_signed_int.AddParents(&TA_int); 
  TA_signed.AddParents(&TA_int); 
  TA_int32_t.AddParents(&TA_int); 
  TA_unsigned.AddParents(&TA_unsigned_int);
  TA_uint.AddParents(&TA_unsigned_int); 
  TA_uint32_t.AddParents(&TA_unsigned_int); 
  TA_signed_long.AddParents(&TA_long);
  TA_long_int.AddParents(&TA_long);
  TA_signed_long_int.AddParents(&TA_long);
  TA_unsigned_long_int.AddParents(&TA_unsigned_long);
  TA_ulong.AddParents(&TA_unsigned_long);
  TA_long_long.AddParents(&TA_int64_t);
  TA_signed_long_long.AddParents(&TA_int64_t);
  TA_unsigned_long_long.AddParents(&TA_uint64_t);
  //note: (u)intptr_t has special runtime code to test size and add to either (u)int or (u)int64_t
}

void MTA::InitTypeSpace(TypeSpace& ts) {
  AddBuiltIn(ts);
  ts.Add(&TA_struct);
  ts.Add(&TA_union);
  ts.Add(&TA_class);
  ts.Add(&TA_template);
  ts.Add(&TA_templ_inst);
  ts.Add(&TA_ta_array);
  ts.Add(&TA_taBase);
  TA_taBase.AddParFormal(&TA_class);
  ts.Add(&TA_taRegFun);
  ts.Add(&TA_TypeItem);
  TA_TypeItem.AddParFormal(&TA_class);
  ts.Add(&TA_EnumDef);
  TA_EnumDef.AddParFormal(&TA_class);
  TA_EnumDef.AddParents(&TA_TypeItem);
  ts.Add(&TA_TypeDef);
  TA_TypeDef.AddParFormal(&TA_class);
  TA_TypeDef.AddParents(&TA_TypeItem);
  ts.Add(&TA_MemberDefBase);
  TA_MemberDefBase.AddParFormal(&TA_class);
  TA_MemberDefBase.AddParents(&TA_TypeItem);
  ts.Add(&TA_MemberDef);
  TA_MemberDef.AddParFormal(&TA_class);
  TA_MemberDef.AddParents(&TA_MemberDefBase);
  ts.Add(&TA_PropertyDef);
  TA_PropertyDef.AddParFormal(&TA_class);
  TA_PropertyDef.AddParents(&TA_MemberDefBase);
  ts.Add(&TA_MethodDef);
  TA_MethodDef.AddParFormal(&TA_class);
  TA_MethodDef.AddParFormal(&TA_TypeItem);
  ts.Add(&TA_taString);
  TA_taString.AddParFormal(&TA_class);
  ts.Add(&TA_Variant);
  TA_Variant.AddParFormal(&TA_class);
  ts.Add(&TA_void_ptr);
}

void MTA::BuildHashTables() {
  spc_target.BuildHashTable(hash_size);
  spc_other.BuildHashTable(hash_size);
  spc_extern.BuildHashTable(hash_size);
  spc_ignore.BuildHashTable(hash_size);
  spc_pre_parse.BuildHashTable(hash_size);
  spc_keywords.BuildHashTable(100);
}

void MTA::Burp() {
  line = st_line;
  col = st_col;
  fh.seekg(st_pos);
  strm_pos = st_pos;
}

void MTA::Class_ResetCurPtrs() {
  mta->cur_memb = NULL; mta->cur_memb_type = NULL; mta->cur_meth = NULL;
  mta->last_memb = NULL; mta->last_meth = NULL;
}

void MTA::Class_UpdateLastPtrs() {
  mta->last_memb = mta->cur_memb;
  mta->last_meth = mta->cur_meth;
  mta->cur_memb = NULL; mta->cur_memb_type = NULL; mta->cur_meth = NULL;
}

String MTA::FindFile(const String& fname, bool& ok) {
  //NOTE: ok only cleared on error
  // first just check the basic name, may be abs, or in current
  if (taPlatform::fileExists(fname)) {
    return fname;
  }
  // otherwise, search paths, unless it is already qualified
  if (!taPlatform::isQualifiedPath(fname)) {
    for (int i = 0; i < paths.size; ++i) {
      String fqfname = paths.FastEl(i) + fname;
      if (taPlatform::fileExists(fqfname)) {
        return fqfname;
      }
    }
  }
  // not found -- we return fname, but set error
  
  cerr <<  "W!!: Warning: file could not be found on the include paths:: " << fname.chars() << "\n";
  ok = false;
  return fname;
}

TypeSpace* MTA::GetTypeSpace(TypeDef* td) {
  TypeSpace* rval = mta->spc;
  TypeDef* partd;
  if(td->HasOption("IGNORE")) {
    rval = &(spc_ignore);
    if(((partd = td->GetParent()) != NULL) && (partd->InheritsFormal(TA_templ_inst)))
      spc_ignore.Transfer(partd); // put this on the ignore list too
  }
  else if((td->owner != NULL) && (td->owner->owner != NULL)) {
    rval = td->owner;
    if(rval->name == "templ_pars") // don't add new types to template parameters!
      rval = &(td->owner->owner->sub_types);
  }
  if(((partd = td->GetParent()) != NULL) && (partd->InheritsFormal(TA_templ_inst))) {
    partd->opts.DupeUnique(td->opts); // inherit the options..
    partd->inh_opts.DupeUnique(td->inh_opts);
  }
  return rval;
}

void MTA::TypeAdded(const char* typ, TypeSpace* sp, TypeDef* td) {
  if(verbose <= 2)	return;
  cerr << "M!!: " << typ << " added: " << td->name << " to: "
       << sp->name << " idx: " << td->idx << endl;
}

void MTA::TypeNotAdded(const char* typ, TypeSpace* sp, TypeDef* ext_td, TypeDef* new_td) {
  if(ext_td->name != new_td->name) {
    cerr << "E!!: Error in hash table name lookup -- names: " << ext_td->name << " and: " 
	 << new_td->name << " should be the same!" << endl;
  }
  if(verbose <= 2)	return;
  cerr << "M!!: " << typ << " NOT added: " << new_td->name << " to: "
       << sp->name << " because of existing type: " << ext_td->name
       << " idx: " << ext_td->idx << endl;
}

void MTA::SetDesc(const char* comnt, String& desc, String_PArray& inh_opts,
		  String_PArray& opts, String_PArray& lists) {
  String tmp = comnt;
  tmp.gsub("\"", "'");		// don't let any quotes get through
  String ud;
  while(tmp.contains('#')) {
    desc += tmp.before('#');
    tmp = tmp.after('#');
    if(tmp.contains(' '))
      ud = tmp.before(' ');
    else
      ud = tmp;
    tmp = tmp.after(' ');
    if(ud(0,5) == "LIST_") {
      ud = ud.after("LIST_");
      lists.AddUnique(ud);
    }
    else {
      if(ud(0,1) == '#') {
	ud = ud.after('#');
	inh_opts.AddUnique(ud);
      }
      opts.AddUnique(ud);
    }
  }
  desc += tmp;
}

TypeDef* MTA::FindName(const char* nm, int& lex_token) {
  TypeDef* itm;

  lex_token = TYPE;		// this is the default

  if((state == Parse_inclass) && (cur_class != NULL)) {
    if(cur_class->name == nm) {
      lex_token = THISNAME;
      return cur_class;
    }
    if((itm = cur_class->sub_types.FindName(nm)) != NULL)
      return itm;
    if((itm = cur_class->templ_pars.FindName(nm)) != NULL)
      return itm;
  }

  if(((itm = spc_keywords.FindName(nm)) != NULL) && (itm->idx != TA_TYPEDEF)) {
    lex_token = itm->idx;
    return itm;
  }

  TypeDef *rval = NULL;
  if((itm = spc->FindName(nm)) != NULL)
    rval = itm;
  else if((spc != &spc_other) && (itm = spc_other.FindName(nm)) != NULL)
    rval = itm;
  else if((itm = spc_ignore.FindName(nm)) != NULL)
    rval = itm;

  if(rval != NULL) {
    if(rval == &TA_const)
      lex_token = CONST;
  }
  return rval;
}


// sets the pre-parsed flag if item is on the pre_parse list...
void MTA::SetPreParseFlag(TypeSpace& aspc, TypeSpace& pplist) {
  int i;
  for(i=0; i<aspc.size; i++) {
    TypeDef* td = aspc.FastEl(i);
    TypeDef* ttd;
    if((ttd = pplist.FindName(td->name)) != NULL)
      td->pre_parsed = true;
  }
}

#if (defined(TA_OS_UNIX))
void mta_cleanup(int err) {
  signal(err, SIG_DFL);
  cerr << "E!!: maketa: exiting and cleaning up temp files from signal: ";
  taMisc::Decode_Signal(err);
  cerr << endl;
  String tmp_file = String("/tmp/mta_tmp.") + String(getpid());
  String rm_tmp = String("/bin/rm ") + tmp_file + " >/dev/null 2>&1";
  int res = system(rm_tmp);
  kill(getpid(), err);		// activate signal
}
#endif

void mta_print_commandline_args(char* argv[]) {
    cerr << "Usage:\t" << argv[0]
      << "\n(* indicates default argument)"
      << "\n[-[-]help | -[-]?]  print this argument listing"
      << "\n[-w]                wait for input before starting (useful when attaching debugger in Windows)"
      << "\n[-v<level>]         verbosity level, 1-5, 1=results,2=more detail,3=trace,4=source,5=parse"
      << "\n[-hx | -nohx*]      generate .hx, .ccx files instead of .h, .cpp (for cmp-based updating)"
      << "\n[-autohx | -noautohx*] if making hx files, update h files if changed (autohx implies hx)"
      << "\n[-css* | -nocss]     generate CSS stub functions"
      << "\n[-instances]        generate instance tokens of types"
      << "\n[-class_only | -struct_union] only scan for class types (else struct and unions)"
      << "\n[-I<include>]...    path to include files (one path per -I)"
      << "\n[-D<define>]...     define a pre-processor macro"
      << "\n[-cpp=<cpp command>] explicit path for c-pre-processor (g++ -E is default)"
      << "\n[-hash<size>]       size of hash tables (default 2000), use -v1 to see actual sizes"
      << "\n[-f <filename>]     read list of header files from given file"
      << "\n[-k]                keep temporary files (useful for debugging)"
      << "\n[-gendoc]           generate xml documentation for all types"
      << "\n[-win_dll[=STR]]    use macro for external linkage, default is XXX_API where XXX is proj name (win only)"
      << "\nproject             stub project name (generates project_TA[.cpp|_type.h|_inst.h])"
      << "\nfiles...            the header files to be processed\n";
}

int main(int argc, char* argv[])
{
  mta = new MTA;

  mta->spc = &(mta->spc_other);

  if(argc < 2) { mta_print_commandline_args(argv); return 1;  } // wrong number of arguments
#ifdef CYGWIN
  String cpp = "cpp";
  String rm = "rm ";
#elif (defined(TA_OS_WIN))
  String cpp = "cl.exe /E /C"; //NOTE: preprocesses, preserving comments, inhibits compilation
  String rm = String("del ");
#else
  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) mta_cleanup);
  String cpp = "g++ -E";
  String rm = String("/bin/rm ");
#endif
  String incs;
  mta->basename = "";		// initialize

  bool wait = false;
  bool keep_tmp = false;
  int i;
  String tmp;
  // always search in current directory first...
  mta->paths.Add(taPlatform::finalSep("."));
  for(i=1; i<argc; i++) {
    tmp = argv[i];
    if( (tmp == "-help") || (tmp == "--help") 
      || (tmp == "-?") || (tmp == "--?") || (tmp == "/?")
    ) {
      mta_print_commandline_args(argv); return 1; 		// EXIT
    }
    mta->gen_doc = false;
    if(tmp == "-css")
      mta->gen_css = true;
    else if(tmp == "-nocss")
      mta->gen_css = false;
    else if(tmp == "-instances")
      mta->gen_instances = true;
    else if(tmp == "-nohx") 
      mta->make_hx = false;
    else if(tmp == "-hx")
      mta->make_hx = true;
    else if(tmp == "-noautohx")
      mta->auto_hx = false;
    else if(tmp == "-gendoc")
      mta->gen_doc = true;
    else if(tmp == "-autohx") {
      mta->make_hx = true;
      mta->auto_hx = true;
    } else if(tmp == "-class_only")
      mta->class_only = true;
    else if(tmp == "-struct_union")
      mta->class_only = false;
    else if(tmp == "-w") 
      wait = true;
    else if(tmp == "-k") 
      keep_tmp = true;
    else if(tmp(0,2) == "-v") {
      mta->verbose = 1;
      int vl;
      String vls;
      vls = tmp.after(1);
      vl = atoi((char*)vls);
      if(vl > 0)
	mta->verbose = vl;
    }
    else if(tmp(0,5) == "-hash") {
      int vl;
      String vls;
      vls = tmp.after("-hash");
      vl = atoi((char*)vls);
      if(vl > 0)
	mta->hash_size = vl;
    }
    else if(tmp(0,2) == "-I") {
#if (defined(TA_OS_WIN) && !defined(CYGWIN))
      // to avoid space issues, put filename in quotes, and use MSVC style
      incs += String("/I \"") + tmp.from(2) + "\" ";
#else
      incs += tmp + " ";
#endif
      mta->paths.AddUnique(taPlatform::finalSep(tmp.from(2)));
    } else if(tmp(0,2) == "/I") { // MSVC style, arg is separate
      if ((i + 1) < argc) {
        i++; // get filename, put in quotes in case of spaces
        incs += String("/I \"") + (const char*)argv[i] + "\" ";
        mta->paths.AddUnique(taPlatform::finalSep(argv[i]));
      }
    } else if(tmp(0,2) == "-D")
      incs += tmp + " ";
    else if(tmp(0,2) == "/D") { // MSVC style, arg is separate
      if ((i + 1) < argc) {
        i++; // get define
        incs += String("/D ") +  (const char*)argv[i] + " ";
      }
    } else if(tmp(0,5) == "-cpp=") {
      if (tmp.length() > 5)
        cpp = tmp.after(4);
    } else if(tmp(0,8) == "-win_dll") {
#ifdef TA_OS_WIN
      mta->win_dll = true;
      if(tmp(8,1) == "=")
        mta->win_dll_str = tmp.after(9);
#endif
    } else if(tmp(0,2) == "-f") {
      fstream fh(argv[i+1], ios::in);
      if(fh.bad() || fh.eof()) {
	cerr << argv[0] << " could not open -f file: " << argv[i+1] << "\n";
      }
      else {
	while(fh.good() && !fh.eof()) {
	  String fl;
	  fh >> fl;
	  fl = trim(fl);
	  if (fl.empty()) continue; // maybe last line
	  //note: warn on duplicates, because these cause havoc if they slip in
	  // but order is also sometimes important in the master input list
	  // so we shouldn't just ignore them
	  bool ok = true;
	  String tfl = mta->FindFile(fl, ok);
	  if (!ok) continue; // warning was printed
	  if (!mta->headv.AddUnique(taPlatform::lexCanonical(tfl))) {
	    cerr <<  "W!!: Warning: duplicate file specified, duplicate ignored:: " << fl.chars() << "\n";
	  }
	}
      }
      fh.close(); fh.clear();
      i++;			// skip to next one
    }
    else if(tmp[0] == '-')
      cerr << argv[0] << " unknown flag: " << tmp << "\n";
    else if(tmp[0] == '+')
      cerr << argv[0] << " unknown flag: " << tmp << "\n";
    else if(mta->basename.empty())
      mta->basename = tmp;
    else {
      // add the header file; see comments in loop above about duplicates
      bool ok = true;
      String tfl = mta->FindFile(tmp, ok);
      if (!ok) continue; // warning was printed
      if (!mta->headv.AddUnique(taPlatform::lexCanonical(tfl))) {
	cerr <<  "**WARNING: duplicate file specified, duplicate ignored:: " << tmp.chars() << "\n";
      }
    }
  }
  
#ifdef TA_OS_WIN
  if (mta->win_dll) { // make sure macro value is set
    if (mta->win_dll_str.empty())
      mta->win_dll_str = upcase(mta->basename) + "_API";
  }
#endif
  if (wait) {
    cerr << "Press Enter key to start...";
    cin.get();
  }

  mta->BuildHashTables();	// after getting any user-spec'd hash size
  // parse the file names a bit

  for(i=0; i<mta->headv.size; i++) {
    String nstr = taPlatform::getFileName(mta->headv.FastEl(i));
    mta->head_fn_only.Add(nstr);
  }

  if(mta->verbose > 0) {
    cerr << "M!!: header files to be parsed:\n";
    mta->headv.List(cout);
    cerr << "\nM!!: header files to be parsed (file-name-only):\n";
    mta->head_fn_only.List(cout);
    cerr << endl;
  }

  if(mta->verbose > 4)
    yydebug = 1;			// debug it.
  else
    yydebug = 0;

  if(cpp.contains("cccp"))
    cpp += " -lang-c++";
  String comnd_base = cpp + " " + incs;

  mta->spc_target.name = mta->basename;
  //note: even for hx mode, the filenames need to be proper here
  // during the scan, because code does some kind of funky comparisons
  // so we fix them up later
  mta->ta_type_h = mta->basename + "_TA_type.h";
  mta->ta_inst_h = mta->basename + "_TA_inst.h";
  mta->ta_ccname = mta->basename + "_TA.cpp";
  
  // create stub _type.h file if doesn't exist, so compiles don't fail
  FILE* dummy = fopen(mta->ta_type_h, "r");
  if (!dummy) {
    dummy = fopen(mta->ta_type_h, "w");
  }
  fclose(dummy);

  String comnd;
  for(i=0; i<mta->headv.size; i++) {
    String tmp_file = taPlatform::finalSep(taPlatform::getTempPath()) + 
      taPlatform::getFileName(mta->headv.FastEl(i)) + "." + String(getpid()) + String(".~mta");
    mta->fname = mta->headv.FastEl(i);
#if (defined(TA_OS_WIN) && !defined(CYGWIN))
//    mta->fname.makeUnique();
//    mta->fname.gsub("/", "\\");
//mta->fname.gsub(":", ":\\");
    comnd = comnd_base + " /D __MAKETA__ " + mta->fname + " > " + tmp_file;
#else
    comnd = comnd_base + " -C -D__MAKETA__ -o " + tmp_file + " " + mta->fname;
//    comnd = comnd_base + " -C -D__MAKETA__ " + mta->fname + " > " + tmp_file;
#endif

    if(mta->verbose > 0)
      cerr << "M!!: " << comnd << "\n";
    cout.flush();
    int ret_code;
    if ((ret_code = system((char*)comnd)) != 0) {
      cout << "**maketa command did not succeed (err code  " << ret_code << ")\n";
      return ret_code;
    }
#if (defined(TA_OS_WIN) && !defined(CYGWIN))
/*    String lef = "lef.exe " + String(tmp_file) + " " + String(tmp_file) + ".1";
    if ((ret_code = system(lef)) != 0) {
      cout << "**lef command did not succeed (err code  " << ret_code << ")\n";
      return ret_code;
    }
    // hack to convert the crlf to lf's so the lexer works properly
    mta->fh.open(String(tmp_file) + ".1", ios::in | ios::binary); // binary critical when using windows */
    mta->fh.open(tmp_file, ios::in | ios::binary); // binary critical when using windows
#else
    mta->fh.open(tmp_file, ios::in | ios::binary); // binary critical when using windows
#endif
    mta->state = MTA::Find_Item;
    mta->yy_state = MTA::YYRet_Ok;
    mta->line = 1;
    mta->col = 0;
    mta->strm_pos=0;
    cout << "Processing: " << mta->fname << "\n";
    cout.flush();
    // NOTE: we start by assuming target space -- 
    // mta_lex will modify this as/if #[line] xxx "..." directives are encountered in preprocessed file
    mta->spc = &(mta->spc_target);
    while(mta->yy_state != MTA::YYRet_Exit) yyparse();
    mta->fh.close(); mta->fh.clear();
    mta->included.DupeUnique(mta->tmp_include); // copy over
    if (!keep_tmp) {
      int res = system(rm + tmp_file);
    }
  }

  TypeSpace_Generate_LinkRefs(&(mta->spc_target), &(mta->spc_extern));
  if(mta->verbose > 0)
    mta->spc_extern.List();
  mta->spc_target.BorrowUniqNameOld(mta->spc_extern); // get those types

  cout << "List of pre-parsed files processed:\n";
  mta->pre_parse_inits.List(cout);
  cout << "\n";

  if(mta->verbose > 1) {
    cout << "\nPreParsed Types\n";   mta->spc_pre_parse.List();
  }
  mta->SetPreParseFlag(mta->spc_target, mta->spc_pre_parse);

  // give it 5 passes through to try to get everything in order..
  int swp_cnt = 0;
  if(mta->verbose > 0)
    cerr << "M!!: Sorting: Pass " << swp_cnt << "\n";
  while ((swp_cnt < 10) && TypeSpace_Sort_Order(&(mta->spc_target))) {
    swp_cnt++;
    if(mta->verbose > 0)
      cerr << "M!!: Sorting: Pass " << swp_cnt << "\n";
  }

  if(mta->verbose > 3) {
    mta->spc_target.List();
  }
  
  // if using hx mode, modify filenames now at this point for output
  if(mta->make_hx) {
    mta->ta_type_h = mta->basename + "_TA_type.hx";
    mta->ta_inst_h = mta->basename + "_TA_inst.hx";
    mta->ta_ccname = mta->basename + "_TA.ccx";
  }
  fstream out_type_h, out_inst_h, outc;

  if(mta->gen_doc) {
    fstream gen_doc_xml;
    gen_doc_xml.open(mta->basename + "_TA_doc.xml", ios::out);
    mta->GenDoc(&(mta->spc_target), gen_doc_xml);
    gen_doc_xml.close();  gen_doc_xml.clear();
  }
  else {
    out_type_h.open((char*)mta->ta_type_h, ios::out);
    out_inst_h.open((char*)mta->ta_inst_h, ios::out);
    outc.open((char*)mta->ta_ccname, ios::out);

    mta->TypeSpace_Declare_Types(&(mta->spc_target), out_type_h, mta->headv);
    out_type_h.close();  out_type_h.clear();
    mta->TypeSpace_Declare_Instances(&(mta->spc_target), out_inst_h, mta->headv);
    out_inst_h.close();  out_inst_h.clear();
    mta->TypeSpace_Generate(&(mta->spc_target), outc, mta->headv, mta->pre_parse_inits);
    outc.close();  outc.clear();

    /* update times...why do we have to do this?? */
#if (defined(TA_OS_WIN) && !defined(CYGWIN))
    //TODO: fails on Windows (no "touch" command)
#else
    comnd = String("touch ") + mta->ta_type_h;
    int res = system(comnd);
    comnd = String("touch ") + mta->ta_inst_h;
    res = system(comnd);
    comnd = String("touch ") + mta->ta_ccname;
    res = system(comnd);
#endif

    // if in autohx mode, then update files that changed
    if (mta->make_hx && mta->auto_hx) {
      String fin, fout;
      fin = mta->basename + "_TA_type.hx";
      fout = mta->basename + "_TA_type.h";
      if (!files_same(fin.chars(), fout.chars())) {
	copy_file(fin.chars(), fout.chars());
      }
      fin = mta->basename + "_TA_inst.hx";
      fout = mta->basename + "_TA_inst.h";
      if (!files_same(fin.chars(), fout.chars())) {
	copy_file(fin.chars(), fout.chars());
      }
      fin = mta->basename + "_TA.ccx";
      fout = mta->basename + "_TA.cpp";
      if (!files_same(fin.chars(), fout.chars())) {
	copy_file(fin.chars(), fout.chars());
      }
    }
  }
  
  if((mta->verbose > 0) && (mta->spc_target.hash_table != NULL)) {
    cerr << "\nM!!: TypeSpace size and hash_table bucket_max values:\n"
	 << "spc_target:\t" << mta->spc_target.size << "\t" << mta->spc_target.hash_table->bucket_max << "\n"
	 << "spc_other:\t" << mta->spc_other.size << "\t" << mta->spc_other.hash_table->bucket_max << "\n"
	 << "spc_extern:\t" << mta->spc_extern.size << "\t" << mta->spc_extern.hash_table->bucket_max << "\n"
	 << "spc_ignore:\t" << mta->spc_ignore.size << "\t" << mta->spc_ignore.hash_table->bucket_max << "\n"
	 << "spc_pre_parse:\t" << mta->spc_pre_parse.size << "\t" << mta->spc_pre_parse.hash_table->bucket_max << "\n"
	 << "spc_keywords:\t" << mta->spc_keywords.size << "\t" << mta->spc_keywords.hash_table->bucket_max << "\n";
  }
  if(mta->verbose > 1) {
    cout << "\nPreParsed Types\n";   mta->spc_pre_parse.List();
    cout << "\nTarget Types\n";   mta->spc_target.List();
    cout << "\nExtern Types\n";   mta->spc_extern.List();
    cout << "\nIgnored Types\n";   mta->spc_ignore.List();
    cout << "\nOther Types\n";   mta->spc_other.List();
  }
  taMisc::quitting = taMisc::QF_FORCE_QUIT; // useful for debugging
  delete mta;
  taMisc::types.RemoveAll();
  return 0;
}
