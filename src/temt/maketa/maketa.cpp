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

char MTA::LastLn[8192];

MTA::MTA() {
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
  gen_doc = false;
  class_only = true;
  verbose = 0;
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

  spc_keywords.name = "Key Words for Searching";
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

void MTA::Burp() {
  line = st_line;
  col = st_col;
  strm_pos = st_pos;
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

  cerr <<  "W!!: Warning: file could not be found on the include paths:: " << fname.chars() << "\n";
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
  }
  if(((partd = td->GetParent()) != NULL) && (partd->IsTemplInst())) {
    partd->opts.DupeUnique(td->opts); // inherit the options..
    partd->inh_opts.DupeUnique(td->inh_opts);
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
    td->opts.AddUnique(TypeDef::opt_instance);       // ta_bases always have an instance
  }
}

void MTA::TypeAdded(const char* typ, TypeSpace* sp, TypeDef* td) {
  String typstr = typ;
  if(typstr != "class" && typstr != "enum" && typstr != "template") {
    td->source_file = taMisc::PathToUnixSep(cur_fname);
    td->source_start = line-1;
    td->source_end = line-1;
  }

  if(verbose <= 2)      return;
  cerr << "M!!: " << typ << " added: " << td->name << " to: "
       << sp->name << " idx: " << td->idx << " src: "
       << td->source_file << ":" << td->source_start << "-" << td->source_end
       << endl;
}

void MTA::TypeNotAdded(const char* typ, TypeSpace* sp, TypeDef* ext_td, TypeDef* new_td) {
  if(ext_td->name != new_td->name) {
    cerr << "E!!: Error in hash table name lookup -- names: " << ext_td->name << " and: "
         << new_td->name << " should be the same!" << endl;
  }
  if(verbose <= 2)      return;
  cerr << "M!!: " << typ << " NOT added: " << new_td->name << " to: "
       << sp->name << " because of existing type: " << ext_td->name
       << " idx: " << ext_td->idx << endl;
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
    if((itm = cur_class->sub_types.FindName(nm)) != NULL)
      return itm;
    if((itm = cur_class->templ_pars.FindName(nm)) != NULL)
      return itm;
  }

  if(((itm = spc_keywords.FindName(nm)) != NULL)) {
    lex_token = itm->idx;
    return itm;
  }

  TypeDef *rval = NULL;
  if((itm = taMisc::types.FindName(nm)) != NULL)
    rval = itm;

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
        if(mta->verbose > 4) {
          cerr << "M!!: Switching order of: " << td->name << " fm: " << td->idx << " to: "
               << par_td->idx+1 << "\n";
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
  cerr << "E!!: maketa: exiting and cleaning up temp files from signal: ";
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
    cerr << argv[i] << " ";
  }
  cerr << endl;
}

void mta_print_usage(int argc, char* argv[]) {
  cerr << "Usage:\t" << argv[0]
       << "\n(* indicates default argument)"
       << "\n[-[-]help | -[-]?]  print this argument listing"
       << "\n[-o <out_file>      output file name to generate (default is target file_TA.cxx)"
       << "\n[-w]                wait for input before starting (useful when attaching debugger in Windows)"
       << "\n[-v<level>]         verbosity level, 1-5, 1=results,2=more detail,3=trace,4=source,5=parse"
       << "\n[-css* | -nocss]     generate CSS stub functions"
       << "\n[-instances]        generate instance tokens of types"
       << "\n[-class_only | -struct_union] only scan for class types (else struct and unions)"
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
  // mta_print_args(argc, argv);
#if 0 // change to 1 for debugging
  verbose = 2;
  //  bool keep_tmp = true;
  bool keep_tmp = false;
#else
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
    else if(tmp == "-class_only") {
      class_only = true;
    }
    else if(tmp == "-struct_union") {
      class_only = false;
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
    cerr << "maketa ERROR -- no target header file specified! aborting!" << endl;
    return 1;
  }

  if (wait) {
    cerr << "Press Enter key to start...";
    cin.get();
  }

  ////////////////////////////////////////////////
  //    misc prep on filenames and setup

  BuildHashTables();       // after getting any user-spec'd hash size

  trg_fname_only = taMisc::GetFileFmPath(trg_header);
  trg_basename = taMisc::StringCVar(trg_fname_only);

  if(verbose > 0) {
    cerr << "M!!: target header file to be parsed: " << trg_header << endl;
  }
  if(out_fname.empty()) {
    out_fname = trg_header.before(".h") + "_TA.cxx";
    if(verbose > 0) {
      cerr << "M!!: output file set to: " << out_fname << endl;
    }
  }

  if(verbose > 4)
    yydebug = 1;                        // debug it.
  else
    yydebug = 0;

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

  if(verbose > 0) {
    cerr << "M!!: " << comnd << "\n";
  }
  cout.flush();
  int ret_code = system((char*)comnd);
  if (ret_code != 0) {
    cout << "**maketa cpp command did not succeed (err code  " << ret_code << ")\n";
    cout << "\n**maketa cpp command was: " << comnd.chars() << "\n\n";
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
  state = MTA::Find_Item;
  yy_state = MTA::YYRet_Ok;
  line = 1;
  col = 0;
  cout << "Running maketa on: " << trg_header << endl;

  while(yy_state != MTA::YYRet_Exit) yyparse();

  ////////////////////////////////////////////////
  //    Generate output

  // give it 10 passes through to try to get everything in order..
  // int swp_cnt = 0;
  // if(mta->verbose > 0)
  //   cerr << "M!!: Sorting: Pass " << swp_cnt << "\n";
  // while ((swp_cnt < 10) && TypeSpace_Sort_Order(&(taMisc::types))) {
  //   swp_cnt++;
  //   if(mta->verbose > 0)
  //     cerr << "M!!: Sorting: Pass " << swp_cnt << "\n";
  // }

  if(verbose > 3) {
    String tl;
    taMisc::types.Print(tl);
    cout << tl;
  }

  fstream outc;

  outc.open((char*)out_fname, ios::out);

  TypeSpace_Gen(&taMisc::types, outc);
  outc.close();  outc.clear();

  if((verbose > 0) && (taMisc::types.hash_table != NULL)) {
    cerr << "\nM!!: TypeSpace size and hash_table bucket_max values:\n"
         << "taMisc::types:\t" << taMisc::types.size << "\t" << taMisc::types.hash_table->bucket_max << "\n"
         << "spc_keywords:\t" << spc_keywords.size << "\t" << spc_keywords.hash_table->bucket_max << "\n";
  }
  if(verbose > 1) {
    String st;
    cout << "\nTarget Types\n";   taMisc::types.Print(st); cout << st;
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

