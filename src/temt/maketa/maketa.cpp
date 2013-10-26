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
#include <taMisc>
#include <taHashTable>
#include <Variant>
#include <BuiltinTypeDefs>

#include <signal.h>

extern int yydebug;
extern "C" int getpid();
MTA* mta;               // holds mta

MTA::MTA() {
  cur_enum = NULL;
  cur_class = NULL;
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
  gen_doc = false;
  verbose = 0;
  v_trg_only = true;
  filter_errs = false;
  filter_warns = true;
  dbg_constr = false;
#ifdef TA_OS_WIN
  win_dll = false;
#endif
  hash_size = 10000;

  st_line = 0;
  st_col = 0;
  strm_pos = 0;
  st_pos = 0;
  st_line_pos = 0;
  line = 0;
  col = 0;
  anon_no = 0;
  defn_st_line = 0;

  state = Find_Item;
  yy_state = YYRet_Ok;

  cur_is_trg = false;
  ta_lib = false;

  spc_keywords.name = "spc_keywords";
  InitKeyWords();

  tac_AddBuiltinTypeDefs();    // adds to taMisc::types
}

MTA::~MTA() {
}

void MTA::InitKeyWords() {
  TypeDef* ky;
  ky = new TypeDef("typedef");  spc_keywords.Add(ky); ky->idx = MP_TYPEDEF;
  ky = new TypeDef("class");    spc_keywords.Add(ky); ky->idx = MP_CLASS;
  ky = new TypeDef("typename"); spc_keywords.Add(ky); ky->idx = MP_TYPENAME;
  ky = new TypeDef("struct");   spc_keywords.Add(ky); ky->idx = MP_STRUCT;
  ky = new TypeDef("union");    spc_keywords.Add(ky); ky->idx = MP_UNION;
  ky = new TypeDef("template"); spc_keywords.Add(ky); ky->idx = MP_TEMPLATE;
  ky = new TypeDef("enum");     spc_keywords.Add(ky); ky->idx = MP_ENUM;
  ky = new TypeDef("public");   spc_keywords.Add(ky); ky->idx = MP_PUBLIC;
  ky = new TypeDef("private");  spc_keywords.Add(ky); ky->idx = MP_PRIVATE;
  ky = new TypeDef("protected");spc_keywords.Add(ky); ky->idx = MP_PROTECTED;
  ky = new TypeDef("inline");   spc_keywords.Add(ky); ky->idx = MP_FUNTYPE;
  ky = new TypeDef("mutable");  spc_keywords.Add(ky); ky->idx = MP_FUNTYPE;
  ky = new TypeDef("explicit");  spc_keywords.Add(ky); ky->idx = MP_FUNTYPE;
  ky = new TypeDef("volatile");  spc_keywords.Add(ky); ky->idx = MP_FUNTYPE;
  ky = new TypeDef("virtual");  spc_keywords.Add(ky); ky->idx = MP_VIRTUAL;
  ky = new TypeDef("static");   spc_keywords.Add(ky); ky->idx = MP_STATIC;
  ky = new TypeDef("const");    spc_keywords.Add(ky); ky->idx = MP_CONST;
  ky = new TypeDef("operator"); spc_keywords.Add(ky); ky->idx = MP_OPERATOR;
  ky = new TypeDef("friend");   spc_keywords.Add(ky); ky->idx = MP_FRIEND;
  ky = new TypeDef("REG_FUN");  spc_keywords.Add(ky); ky->idx = MP_REGFUN;
  ky = new TypeDef("using");    spc_keywords.Add(ky); ky->idx = MP_USING;
  ky = new TypeDef("namespace"); spc_keywords.Add(ky); ky->idx = MP_NAMESPACE;
}

void MTA::BuildHashTables() {
  taMisc::types.BuildHashTable(hash_size);
  spc_keywords.BuildHashTable(hash_size);
}


bool MTA::VerboseCheckTrg() {
  if(v_trg_only && cur_is_trg) return true;
  if(v_src_trg == cur_fname_only) return true;
  if(v_trg_only || v_src_trg.nonempty()) return false; // exclusive, not met, bail
  return true;  // we don't care anyway
}

void MTA::Info(int v_level, const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
{
  if(!VerboseCheckTrg()) return;
  if(v_level > verbose) return;
  String msg = taMisc::SuperCat(a,b,c,d,e,f,g,h,i);
  cout << "I!!: " << msg << endl;
}

void MTA::Warning(int v_level, const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
{
  if(filter_warns && v_level > 0) {
    if(!VerboseCheckTrg()) return;
  }
  if(v_level > verbose) return;
  String msg = taMisc::SuperCat(a,b,c,d,e,f,g,h,i);
  cerr << "W!!: " << msg << endl;
}

void MTA::Error(int v_level, const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
{
  if(filter_errs && v_level > 0) { // v_level 0 errors are not filtered ever!
    if(!VerboseCheckTrg()) return;
  }
  if(v_level > verbose) return;
  String msg = taMisc::SuperCat(a,b,c,d,e,f,g,h,i);
  cerr << "E!!: " << msg << endl;
}

void MTA::Burp() {
  line = st_line;
  col = st_col;
  strm_pos = st_pos;
}

void MTA::PushState(States new_state) {
  state_stack.Add(new_state);
  state = new_state;
}

MTA::States MTA::PopState() {
  if(state_stack.size == 0) {
    taMisc::Error("pop on empty state_stack!");
    state = Find_Item;
    return state;
  }
  States prv_state = (States)state_stack.Pop();
  if(state_stack.size > 0)
    state = (States)state_stack.Peek();
  else
    state = Find_Item;          // default
  return prv_state;
}

void MTA::ResetState() {
  state_stack.Reset();
  PushState(Find_Item);
  mta->ResetClassStack(); 
  mta->yy_state = MTA::YYRet_Ok;
  type_stack.Reset();
  enum_stack.Reset();
  memb_stack.Reset();
  meth_stack.Reset();
  cur_enum = NULL;
  cur_memb_type = NULL;
  cur_meth = NULL;
  cur_templ_pars.Reset();
  cur_templ_defs.Reset();
  cur_typ_templ_pars.Reset();
  last_memb = NULL;
  last_meth = NULL;
  thisname = false;
  constcoln = false;
  burp_fundefn = false;
  in_templ_pars = false;
}

void MTA::PushClass(TypeDef* new_class, MembState memb_state) {
  class_stack.Link(new_class);
  if(class_mstate_stack.size > 0)
    class_mstate_stack.Peek() = cur_mstate; // update current to current
  class_mstate_stack.Add(memb_state);
  cur_class = new_class;
  cur_mstate = memb_state;
  PushState(Parse_class);
}

TypeDef* MTA::PopClass() {
  if(class_stack.size == 0) {
    taMisc::Error("pop on empty class_stack!");
    cur_class = NULL;
    return NULL;
  }
  TypeDef* prv_class = class_stack.Pop();
  if(class_stack.size > 0) {
    cur_class = class_stack.Peek();
  }
  else {
    cur_class = NULL;
  }
  if(class_mstate_stack.size > 0)
    class_mstate_stack.Pop();
  if(class_mstate_stack.size > 0)
    cur_mstate = (MembState)class_mstate_stack.Peek();

  return prv_class;
}

void MTA::ResetClassStack() {
  class_stack.Reset();
  class_mstate_stack.Reset();
  cur_class = NULL;
}

void MTA::Class_ResetCurPtrs() {
  cur_memb = NULL; cur_memb_type = NULL; cur_meth = NULL;
  last_memb = NULL; last_meth = NULL;
}

void MTA::Class_UpdateLastPtrs() {
  last_memb = cur_memb;
  last_meth = cur_meth;
  cur_memb = NULL; cur_memb_type = NULL; cur_meth = NULL;
}

void MTA::Namespc_PushNew(const char* spc) {
  namespc_stack.Reset();
  // todo: currently have no way to detect exit, so just reset
  namespc_stack.Add(spc);
  Info(2, "entered new namespace:", spc);
}

void MTA::Namespc_Pop() {
  if(namespc_stack.size == 0) {
    taMisc::Error("namespace stack size is 0, cannot pop!");
    return;
  }
  String old = namespc_stack.Pop();
  String cur = "<top level>";
  if(namespc_stack.size > 0)
    cur = namespc_stack.Peek();
  Info(2, "popped off namespace:", old, "current is:", cur);
}

void MTA::StartTemplPars() {
  cur_templ_pars.Reset();
  cur_templ_defs.Reset();
  in_templ_pars = true;
}

void MTA::EndTemplPars() {
  cur_templ_pars.Reset();
  cur_templ_defs.Reset();
  in_templ_pars = false;
}

void MTA::SetSource(TypeDef* td, bool use_defn_st_line) {
  td->source_file = taMisc::PathToUnixSep(cur_fname);
  if(use_defn_st_line) {
    td->source_start = defn_st_line;
  }
  else {
    td->source_start = line-1;
  }
}

void MTA::ClearSource(TypeDef* td) {
  td->source_file = "";
  td->source_start = -1;
  td->source_start = -1;
}

String MTA::FindFile(const String& fname, bool& ok) {
  //NOTE: ok only cleared on error
  // first just check the basic name, may be abs, or in current
  if (taMisc::FileExists(fname)) {
    return fname;
  }
  // otherwise, search paths, unless it is already qualified
  if (!taMisc::IsQualifiedPath(fname)) {
    for (int i = 0; i < paths.size; ++i) {
      String fqfname = paths.FastEl(i) + fname;
      if (taMisc::FileExists(fqfname)) {
        return fqfname;
      }
    }
  }
  // not found -- we return fname, but set error
  Warning(0, "Warning: file could not be found on the include paths: ",
          fname.chars());
  ok = false;
  return fname;
}

TypeSpace* MTA::GetTypeSpace(TypeDef* td) {
  TypeSpace* rval = &taMisc::types;
  // todo: need to detect templates defined in sub-classes
  // if(td->IsTemplate() && cur_class != NULL) {
  //   rval = &(cur_class->sub_types); // put template instances on sub classes
  // }
  TypeDef* partd;
  if((td->owner != NULL) && (td->owner->owner != NULL)) {
    rval = td->owner;
    if(rval->name == "templ_pars") // don't add new types to template parameters!
      rval = &(td->owner->owner->sub_types);
    if(rval->name == "spc_keywords") {
      Error(0, "trying to add to keywords -- not good!", td->name);
      rval = &taMisc::types;
    }
  }
  if(((partd = td->GetParent()) != NULL) && (partd->IsTemplInst())) {
    partd->opts.DupeUnique(td->opts); // inherit the options..
    partd->inh_opts.DupeUnique(td->inh_opts);
  }
  return rval;
}

TypeDef* MTA::TypeAddUniqNameOld(const String& typ, TypeDef* td, TypeSpace* sp) {
  // don't add anything from type traits because it just has a bunch of horrible
  // template crap that nobody needs, and defines various things that interfere
  // with our own types
  if(cur_fname.contains("type_traits")) {
    td = new TypeDef("type_traits");
  }
  if(sp == NULL) sp = GetTypeSpace(td);
  TypeDef* rval = sp->AddUniqNameOld(td);
  if(rval == td) {
    TypeAdded(typ, sp, td);
  }
  return rval;
}

void MTA::FixClassTypes(TypeDef* td) {
  if(td->name == "taString") {
    td->SetType(TypeDef::STRING);
  }
  else if(td->name == "Variant") {
    td->SetType(TypeDef::VARIANT);
  }
  else if(td->HasOption("SMART_POINTER")) {
    td->SetType(TypeDef::SMART_PTR);
  }
  else if(td->HasOption("SMART_INT")) {
    td->SetType(TypeDef::SMART_INT);
  }
  else if(td->InheritsFromName("taBase")) {
    td->SetType(TypeDef::TABASE);
    td->AddOption(TypeDef::opt_instance);       // ta_bases always have an instance
  }
}

void MTA::TypeAdded(const String& typ, TypeSpace* sp, TypeDef* td) {
  if(spc_keywords.FindEl(td) >= 0) {
    Error(0, "adding a keyword type to a new typelist -- this is VERY BAD and indicates a parsing error!", td->name,
	  "to space:", sp->name, "typ:", typ);
  }
  if(!(typ.contains("class") || typ.contains("enum") ||  
       typ == "template")) {
    td->source_file = taMisc::PathToUnixSep(cur_fname);
    td->source_start = line-1;
    td->source_end = line-1;
  }

  Info(3, typ, "added:", td->name, "to:", sp->name,
       "idx:", String(td->idx), "src:", td->source_file  + ":" + String(td->source_start)
       + "-" + String(td->source_end));
}

void MTA::TypeNotAdded(const String& typ, TypeSpace* sp, TypeDef* ext_td, TypeDef* new_td) {
  if(spc_keywords.FindEl(new_td) >= 0) {
    Error(0, "new_td NOT adding a keyword type to a new typelist -- this is VERY BAD and indicates a parsing error!", new_td->name,
	  "to space:", sp->name, "typ:", typ);
  }
  if(spc_keywords.FindEl(ext_td) >= 0) {
    Error(0, "ext_td NOT adding a keyword type to a new typelist -- this is VERY BAD and indicates a parsing error!", ext_td->name,
	  "to space:", sp->name, "typ:", typ);
  }
  if(ext_td->name != new_td->name) {
    Error(0, "Error in hash table name lookup -- names:",
          ext_td->name, "and:", new_td->name, "should be the same!");
  }
  Info(3, typ, "NOT added:", new_td->name, "to:", sp->name,
       "because of existing type:", ext_td->name, "idx:", String(ext_td->idx));
}

void MTA::SetDesc(const char* comnt, String& desc, String_PArray& inh_opts,
                  String_PArray& opts, String_PArray& lists) {
  String tmp = comnt;
  tmp.gsub("\"", "'");          // don't let any quotes get through
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

  lex_token = MP_TYPE;             // this is the default

  if((state == Parse_inclass) && (cur_class != NULL)) {
    if(cur_class->name == nm) {
      lex_token = MP_THISNAME;
      return cur_class;
    }
    if((itm = cur_class->sub_types.FindName(nm)) != NULL) {
      Info(5, "FindName: found MP_TYPE in class sub_types:", nm);
      return itm;
    }
    if((itm = cur_class->templ_pars.FindName(nm)) != NULL) {
      Info(5, "FindName: found MP_TYPE in class templ_pars:", nm);
      return itm;
    }
  }

  if(((itm = spc_keywords.FindName(nm)) != NULL)) {
    lex_token = itm->idx;
    if(String(nm) == "struct") {
      Info(5, "FindName: struct lookup:", String(lex_token), "MP_STRUCT:",
           String(MP_STRUCT), "file:", cur_fname_only, "line:", String(line));
    }
    else {
      Info(5, "FindName: found keyword:", nm);
    }
    return itm;
  }

  TypeDef *rval = NULL;
  if((itm = taMisc::types.FindName(nm)) != NULL) {
    Info(5, "FindName: found type in types list:", nm);
    rval = itm;
  }

  return rval;
}

bool MTA::TypeSpace_Sort_Order(TypeSpace* ths) {
  bool move_occurred = false;
  int i;
  for(i=0; i< ths->size; i++) {
    TypeDef* td = ths->FastEl(i);
    int j;
    for(j=0; j<td->parents.size; j++) {
      TypeDef* par_td = td->parents.FastEl(j);
      if((td->idx < par_td->idx) && (td->owner == ths) && (par_td->owner == ths)) {
        if(dbg_constr) {
          Info(5, "Switching order of:", td->name,
               "fm:", String(td->idx), "to:", String(par_td->idx+1));
        }
        // child comes before parent..
        ths->MoveIdx(td->idx, par_td->idx+1); // move after parent
        move_occurred = true;
      }
    }
  }
  return move_occurred;
}


#if (defined(TA_OS_UNIX))
void mta_cleanup(int err) {
  signal(err, SIG_DFL);
  mta->Error(0, "maketa: exiting and cleaning up temp files from signal:");
  taMisc::Decode_Signal(err);
  cerr << endl;
  String tmp_file = String("/tmp/mta_tmp.") + String(getpid());
  String rm_tmp = String("/bin/rm ") + tmp_file + " >/dev/null 2>&1";
  (void) system(rm_tmp);
  kill(getpid(), err);          // activate signal
}
#endif

void mta_print_args(int argc, char* argv[]) {
  for(int i=0; i<argc; i++) {
    cout << argv[i] << " ";
  }
  cout << endl;
}

void mta_print_usage(int argc, char* argv[]) {
  cout << "Usage:\t" << argv[0]
       << "\n(* indicates default argument)"
       << "\n[-[-]help | -[-]?]  print this argument listing"
       << "\n[-o <out_file>      output file name to generate (default is target file_TA.cxx)"
       << "\n[-w]                wait for input before starting (useful when attaching debugger in Windows)"
       << "\n[-v<level>]         verbosity level, 1-5, 1=results,2=more detail,3=trace,4=source,5=parse"
       << "\n[-css* | -nocss]     generate CSS stub functions"
       << "\n[-instances]        generate instance tokens of types"
       << "\n[-I<include>]...    path to include files (one path per -I)"
       << "\n[-D<define>]...     define a pre-processor macro"
       << "\n[-cpp=<cpp command>] explicit path for c-pre-processor (g++ -E is default)"
       << "\n[-hash<size>]       size of hash tables (default 2000), use -v1 to see actual sizes"
       << "\n[-k]                keep temporary cpp file (useful for debugging)"
    //       << "\n[-gendoc]           generate xml documentation for all types"
       << "\n[-win_dll[=STR]]    use macro for external linkage, default is XXX_API where XXX is proj name (win only)"
       << "\nfile                the target header file to be processed\n";

  mta_print_args(argc, argv);
}

int MTA::Main(int argc, char* argv[]) {
#if 0 // change to 1 for debugging
  mta_print_args(argc, argv);
  verbose = 2;
  v_trg_only = false;
  filter_errs = false;
  filter_warns = false;
  dbg_constr = false;
  v_src_trg = "type_traits";         // set to "" to get everything
  //  bool keep_tmp = true;
  bool keep_tmp = false;
#else
  v_trg_only = true;
  filter_errs = true;
  filter_warns = true;
  dbg_constr = false;
  bool keep_tmp = false;
#endif

  if(argc < 2) { mta_print_usage(argc, argv); return 1;  } // wrong number of arguments

  ////////////////////////////////////////////////
  //    process args

#ifdef CYGWIN
  String cpp = "cpp";
  String rm = "rm ";
#elif (defined(TA_OS_WIN))
  String cpp = "cl.exe /nologo /E /C"; //NOTE: preprocesses, preserving comments, inhibits compilation
  String rm = String("del ");
#else
  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) mta_cleanup);
  String cpp = "g++ -E";
  String rm = String("/bin/rm ");
#endif
  String incs;

  bool wait = false;
  int i;
  String tmp;
  // always search in current directory first...
  paths.Add(taMisc::FinalPathSep("."));
  for(i=1; i<argc; i++) {
    tmp = argv[i];
    // cerr << "processing arg: " << tmp << endl;
    if( (tmp == "-help") || (tmp == "--help")
        || (tmp == "-?") || (tmp == "--?") || (tmp == "/?")) {
      mta_print_usage(argc, argv); return 1;               // EXIT
    }
    if(tmp == "-css") {
      gen_css = true;
    }
    else if(tmp == "-nocss") {
      gen_css = false;
    }
    else if(tmp == "-instances") {
      gen_instances = true;
    }
    else if(tmp == "-gendoc") {
      gen_doc = true;
    }
    else if(tmp == "-o") {
      out_fname = argv[i+1];
      i++;                      // skip
    }
    else if(tmp == "-w") {
      wait = true;
    }
    else if(tmp == "-k") {
      keep_tmp = true;
    }
    else if(tmp(0,2) == "-v") {
      verbose = 1;
      int vl;
      String vls;
      vls = tmp.after(1);
      vl = atoi((char*)vls);
      if(vl > 0)
        verbose = vl;
    }
    else if(tmp(0,5) == "-hash") {
      int vl;
      String vls;
      vls = tmp.after("-hash");
      vl = atoi((char*)vls);
      if(vl > 0)
        hash_size = vl;
    }
    else if(tmp(0,2) == "-I") {
#if (defined(TA_OS_WIN) && !defined(CYGWIN))
      // to avoid space issues, put filename in quotes, and use MSVC style
      incs += String("/I \"") + tmp.from(2) + "\" ";
#else
      incs += tmp + " ";
#endif
      paths.AddUnique(taMisc::FinalPathSep(tmp.from(2)));
    }
    else if(tmp(0,2) == "/I") { // MSVC style, arg is separate
      if ((i + 1) < argc) {
        i++; // get filename, put in quotes in case of spaces
        incs += String("/I \"") + (const char*)argv[i] + "\" ";
        paths.AddUnique(taMisc::FinalPathSep(argv[i]));
      }
    }
    else if(tmp(0,2) == "-D") {
      incs += tmp + " ";
    }
    else if(tmp(0,2) == "/D") { // MSVC style, arg is separate
      if ((i + 1) < argc) {
        i++; // get define
        incs += String("/D ") +  (const char*)argv[i] + " ";
      }
    }
    else if(tmp(0,5) == "-cpp=") {
      if (tmp.length() > 5)
        cpp = tmp.after(4);
      // cerr << "set cpp: " << cpp << endl;
    }
    else if(tmp(0,8) == "-win_dll") {
#ifdef TA_OS_WIN
      win_dll = true;
      if(tmp(8,1) == "=")
        win_dll_str = tmp.after(9);
#endif
    }
    else if(tmp[0] == '-') {
      cerr << argv[0] << " unknown flag: " << tmp << "\n";
    }
    else if(tmp[0] == '+') {
      cerr << argv[0] << " unknown flag: " << tmp << "\n";
    }
    else {
      // add the header file
      bool ok = true;
      String tfl = FindFile(tmp, ok);
      if (!ok) continue; // warning was printed
      trg_header = MTA::lexCanonical(tfl);
    }
  }

  if(trg_header.empty()) {
    Error(0, "maketa ERROR -- no target header file specified! aborting!");
    return 1;
  }

  if (wait) {
    Info(0, "Press Enter key to start...");
    cin.get();
  }

  ////////////////////////////////////////////////
  //    misc prep on filenames and setup

  BuildHashTables();       // after getting any user-spec'd hash size

  trg_fname_only = taMisc::GetFileFmPath(trg_header);
  trg_basename = taMisc::StringCVar(trg_fname_only);
  if(trg_header.contains("src/temt/") || trg_header.contains("\\src\\temt"))
    ta_lib = true;
  else
    ta_lib = false;

  Info(1, "target header file to be parsed:", trg_header);
  if(out_fname.empty()) {
    out_fname = trg_header.before(".h") + "_TA.cxx";
    Info(1, "output file set to:", out_fname);
  }

  ////////////////////////////////////////////////
  //    run preprocessor on file to generate tmp_fname file

  if(cpp.contains("cccp"))
    cpp += " -lang-c++";
  String comnd_base = cpp + " " + incs;
  String comnd;
  tmp_fname = taMisc::FinalPathSep(taMisc::GetTemporaryPath()) +
    taMisc::GetFileFmPath(trg_header) + "." + String(getpid()) + String(".~mta");

#if (defined(TA_OS_WIN) && !defined(CYGWIN))
  comnd = comnd_base + " /D __MAKETA__ " + trg_header + " > " + tmp_fname;
#else
  comnd = comnd_base + " -C -D__MAKETA__ -o " + tmp_fname + " " + trg_header;
#endif

  Info(1, comnd);
  int ret_code = system((char*)comnd);
  if (ret_code != 0) {
    Error(0, "maketa cpp command did not succeed (err code:", String(ret_code),")");
    Error(0, "maketa cpp command was:", comnd.chars());
    return ret_code;
  }

  ////////////////////////////////////////////////
  //    load preprocessed file into string

  fstream strm;
  strm.open(tmp_fname, ios::in | ios::binary); // binary critical when using windows
  file_str.Load_str(strm); // just suck it all up at once into string and go from there
  strm.close();

  if (!keep_tmp) {
    (void) system(rm + tmp_fname);
  }

  ////////////////////////////////////////////////
  //    Parse

  cur_is_trg = true;            // assume we start out in file
  strm_pos=0;
  PushState(Find_Item);         // starting state
  yy_state = YYRet_Ok;
  line = 1;
  col = 0;
  cout << "Running maketa on: " << trg_header << endl;

  while(yy_state != YYRet_Exit) yyparse();

  ////////////////////////////////////////////////
  //    Generate output

  if(dbg_constr) {
    if(verbose > 3) {
      Info(4, "Types processed:");
      String tl;
      taMisc::types.Print(tl);
      cout << tl;
    }
  }

  fstream outc;

  outc.open((char*)out_fname, ios::out);

  TypeSpace_Gen(&taMisc::types, outc);
  outc.close();  outc.clear();

  if((verbose > 0) && (taMisc::types.hash_table != NULL)) {
    Info(1, "TypeSpace size and hash_table bucket_max values:");
    Info(1, "taMisc::types:\t", String(taMisc::types.size), "\t",
         String(taMisc::types.hash_table->bucket_max));
    Info(1, "spc_keywords:\t", String(spc_keywords.size), "\t",
         String(spc_keywords.hash_table->bucket_max));
  }
  return 0;
}

int main(int argc, char* argv[]) {
  mta = new MTA;
  int rval = mta->Main(argc, argv);
  taMisc::quitting = taMisc::QF_FORCE_QUIT; // useful for debugging
  delete mta;
  taMisc::types.RemoveAll();
  return rval;
}

