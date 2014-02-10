// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "taCodeUtils.h"
#include <TypeDef>
#include <MemberDef>
#include <MethodDef>
#include <int_Array>

#include <taMisc>

#include <QDir>

using namespace std;

bool taCodeUtils::CreateNewSrcFiles(const String& type_nm, const String& top_path,
			       const String& src_dir) {
  String src_path = top_path + "/" + src_dir + "/";
  String crfile = src_path + "COPYRIGHT.txt";
  String cmfile = src_path + "CMakeFiles.txt";
  String hfile = src_path + type_nm + ".h";
  String cppfile = src_path + type_nm + ".cpp";
  String incfile = top_path + "/include/" + type_nm;
  String incfileh = incfile + ".h";

  if(!taMisc::FileExists(crfile)) {
    taMisc::Error("CreateNewSrcFiles: copyright file:", crfile, "not found -- paths must be wrong -- aborting");
    return false;
  }

  String crstr;
  crstr.LoadFromFile(crfile);

  bool got_one = false;

  if(taMisc::FileExists(hfile)) {
    taMisc::Warning("header file:", hfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n#ifndef " << type_nm << "_h\n"
        << "#define " << type_nm << "_h 1\n\n"
        << "// parent includes:\n"
        << "#include <taNBase>\n\n"
        << "// member includes:\n\n"
        << "// declare all other types mentioned but not required to include:\n\n";
    if(src_dir.contains("temt/ta")) {
      str << "taTypeDef_Of(" << type_nm << ");\n\n" << "class TA_API ";
    }
    else if(src_dir.contains("emergent/")) {
      str << "eTypeDef_Of(" << type_nm << ");\n\n" << "class E_API ";
    }
    else {
      str << "TypeDef_Of(" << type_nm << ");\n\n" << "class ";
    }
    str << type_nm << " : public taNBase {\n"
        << "  // <describe here in full detail in one extended line comment>\n"
        << "INHERITED(taNBase)\n"
        << "public:\n\n"
        << "  TA_SIMPLE_BASEFUNS(" << type_nm << ");\n"
        << "private:\n"
        << "  void Initialize()  { };\n"
        << "  void Destroy()     { };\n"
        << "};\n\n"
        << "#endif // " << type_nm << "_h\n";
    str.SaveToFile(hfile);
    taMisc::ExecuteCommand("svn add " + hfile);
    got_one = true;
  }

  if(taMisc::FileExists(cppfile)) {
    taMisc::Warning("cpp file:", cppfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n#include \"" << type_nm << ".h\"\n\n";
    str.SaveToFile(cppfile);
    taMisc::ExecuteCommand("svn add " + cppfile);
    got_one = true;
  }

  if(taMisc::FileExists(incfile)) {
    taMisc::Warning("include file:", incfile, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"" << type_nm << ".h\"\n";
    str.SaveToFile(incfile);
    taMisc::ExecuteCommand("svn add " + incfile);
    got_one = true;
  }

  if(taMisc::FileExists(incfileh)) {
    taMisc::Warning("include file:", incfileh, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << "/" << type_nm << ".h\"\n";
    str.SaveToFile(incfileh);
    taMisc::ExecuteCommand("svn add " + incfileh);
    got_one = true;
  }

  String cmstr;
  cmstr.LoadFromFile(cmfile);
  bool changed = false;

  if(!cmstr.contains(type_nm + ".h")) {
    if(cmstr.contains(".h\n)")) {
      String cmrest = cmstr.after(".h\n)");
      cmstr = cmstr.before(".h\n)");
      cmstr << ".h\n  " << type_nm << ".h\n)" << cmrest;
      changed = true;
    }
    else {
      taMisc::Warning("key sequence of: .h\\n) not found in CMakeFiles.txt -- needed for updating file -- please fix file and add your new .h file manually");
    }
  }
  if(!cmstr.contains(type_nm + ".cpp")) {
    if(cmstr.contains(".cpp\n)")) {
      String cmrest = cmstr.after(".cpp\n)");
      cmstr = cmstr.before(".cpp\n)");
      cmstr << ".cpp\n  " << type_nm << ".cpp\n)" << cmrest;
      changed = true;
    }
    else {
      taMisc::Warning("key sequence of: .cpp\\n) not found in CMakeFiles.txt -- needed for updating file -- please fix file and add your new .cpp file manually");
    }
  }

  if(changed) {
    cmstr.SaveToFile(cmfile);
  }

  return got_one;
}

bool taCodeUtils::RenameFileSVN(const String& old_fn, const String& new_fn) {
  String cmd = "svn mv " + old_fn + " " + new_fn;
  taMisc::Info(cmd);
  taMisc::ExecuteCommand(cmd);
  return true;
}

bool taCodeUtils::RemoveFileSVN(const String& fname) {
  String cmd = "svn rm " + fname;
  taMisc::Info(cmd);
  taMisc::ExecuteCommand(cmd);
  return true;
}

bool taCodeUtils::RenameType(const String& type_nm, const String& new_nm,
                             const String& top_path, const String& src_dir) {
  TypeDef* td = TypeDef::FindGlobalTypeName(type_nm);
  if(!td) {
    taMisc::Warning("type not found:", type_nm);
  }
  TypeDef* new_td = TypeDef::FindGlobalTypeName(new_nm, false);
  if(new_td) {
    taMisc::Error("ooops: new type name already exists!", new_nm);
    return false;
  }

  String src_path = top_path + "/" + src_dir + "/";
  String inc_path = top_path + "/include/";

  if(taMisc::FileExists(src_path + type_nm + ".h")) {
    RenameFileSVN(src_path + type_nm + ".h", src_path + new_nm + ".h");
    RenameFileSVN(src_path + type_nm + ".cpp", src_path + new_nm + ".cpp");
    RenameFileSVN(inc_path + type_nm + ".h", inc_path + new_nm + ".h");
    RenameFileSVN(inc_path + type_nm, inc_path + new_nm);

    taMisc::ReplaceStringInFile(inc_path + new_nm, type_nm, new_nm); // update include
    taMisc::ReplaceStringInFile(inc_path + new_nm + ".h", type_nm, new_nm); // update include
  }
  else {
    taMisc::Warning("Type file name not found:", src_path + type_nm + ".h");
  }
  return ReplaceInAllFiles(type_nm, new_nm, top_path);
}

bool taCodeUtils::ReplaceInDir(const String& search_nm, const String& repl_nm,
                               const String& top_path, const String& src_dir) {
  String src_path = top_path + "/" + src_dir + "/";

  QDir dir(src_path);
  QStringList filters;
  filters << "*.cpp" << "*.h" << "CMakeFiles.txt" << "CMakeLists.txt";
  QStringList files = dir.entryList(filters);
  for(int i=0; i<files.count(); i++) {
    String fnm = files[i];
    int rpl = taMisc::ReplaceStringInFile(src_path + fnm, search_nm, repl_nm);
    if(rpl > 0) {
      taMisc::Info("replaced:", search_nm,"->",repl_nm,"in:",fnm,"count:",String(rpl));
    }
  }
  return true;
}

bool taCodeUtils::ReplaceInAllFiles(const String& search_nm, const String& repl_nm,
                                    const String& top_path) {

  ReplaceInDir(search_nm, repl_nm, top_path, "src/temt/ta");
  ReplaceInDir(search_nm, repl_nm, top_path, "src/temt/css");
  ReplaceInDir(search_nm, repl_nm, top_path, "include");
  ReplaceInDir(search_nm, repl_nm, top_path, "src/emergent/network");
  ReplaceInDir(search_nm, repl_nm, top_path, "src/emergent/leabra");
  ReplaceInDir(search_nm, repl_nm, top_path, "src/emergent/bp");
  ReplaceInDir(search_nm, repl_nm, top_path, "src/emergent/cs");
  ReplaceInDir(search_nm, repl_nm, top_path, "src/emergent/so");
  return true;
}

bool taCodeUtils::RemoveType(const String& type_nm,
                             const String& top_path, const String& src_dir) {
  TypeDef* td = TypeDef::FindGlobalTypeName(type_nm);
  if(!td) {
    taMisc::Error("RemoveType: type not found:", type_nm);
    return false;
  }

  String src_path = top_path + "/" + src_dir + "/";
  String inc_path = top_path + "/include/";

  String fnmo = taMisc::GetFileFmPath(td->source_file);
  if(fnmo.startsWith(type_nm)) {
    RemoveFileSVN(src_path + type_nm + ".h");
    RemoveFileSVN(src_path + type_nm + ".cpp");
    RemoveFileSVN(inc_path + type_nm + ".h");
    RemoveFileSVN(inc_path + type_nm);
  }

  taMisc::ReplaceStringInFile("CMakeFiles.txt", type_nm + ".h\n", "");
  taMisc::ReplaceStringInFile("CMakeFiles.txt", type_nm + ".cpp\n", "");

  QDir dir(src_path);
  QStringList filters;
  filters << "*.cpp" << "*.h";
  QStringList files = dir.entryList(filters);
  for(int i=0; i<files.count(); i++) {
    String fnm = files[i];
    String fstr;
    fstr.LoadFromFile(fnm);
    int rpl = fstr.freq(type_nm);
    if(rpl > 0) {
      taMisc::Info("file name:", fnm, "refers to:",type_nm,"count:",String(rpl));
    }
  }
  return true;
}

bool taCodeUtils::FixIncludes(const String& top_path) {
  String inc_path = top_path + "/include/";

  QDir dir(inc_path);
  QStringList filters;
  filters << "*";
  QStringList files = dir.entryList(filters);
  for(int i=0; i<files.count(); i++) {
    String fnm = files[i];
    if(fnm.endsWith(".h")) continue; // only not-h guys
    int rpl = 0;
    rpl += taMisc::ReplaceStringInFile(inc_path + fnm, "../src/temt/ta/", "./");
    rpl += taMisc::ReplaceStringInFile(inc_path + fnm, "../src/temt/css/", "./");
    rpl += taMisc::ReplaceStringInFile(inc_path + fnm, "../src/emergent/network/", "./");
    rpl += taMisc::ReplaceStringInFile(inc_path + fnm, "../src/emergent/bp/", "./");
    rpl += taMisc::ReplaceStringInFile(inc_path + fnm, "../src/emergent/cs/", "./");
    rpl += taMisc::ReplaceStringInFile(inc_path + fnm, "../src/emergent/so/", "./");
    rpl += taMisc::ReplaceStringInFile(inc_path + fnm, "../src/emergent/leabra/", "./");
    if(rpl > 0) {
      taMisc::Info("replaced in:",fnm,"count:",String(rpl));
    }
  }
  return true;
}

bool taCodeUtils::ListAllInherits(const String& type_nm) {
  TypeDef* trgtp = TypeDef::FindGlobalTypeName(type_nm);
  if(!trgtp) return false;

  taMisc::Info("Types that inherit from:", trgtp->name);
  for(int i=0; i< taMisc::types.size; i++) {
    TypeDef* td = taMisc::types.FastEl(i);
    if(!td->IsActualClass()) continue;
    if(td->InheritsFrom(trgtp)) {
      taMisc::Info(td->name);
    }
  }
  return true;
}

String taCodeUtils::TypeIncludes(TypeDef* td) {
  String inc_str;
  TypeSpace inc_list;

  inc_list.Link(td);      // don't redo ourselves!
  inc_list.Link(&TA_Variant);	// exclude common things
  inc_list.Link(&TA_taString);	

  inc_str << "\n// parent includes:";
  for(int i=0; i< td->parents.size; i++) {
    TypeDef* par = td->parents[i];
    if(par->IsTemplInst()) {
      for(int j=0; j<par->templ_pars.size; j++) {
	TypeDef* tp = par->templ_pars[j];
	if(tp->IsActualClass()) {
          if(inc_list.FindEl(tp) < 0) {
            inc_str << "\n#include <" << tp->name << ">";
            inc_list.Link(tp);
          }
	}
      }
      par = par->parents[0];	// now go to template itself
    }
    if(inc_list.FindEl(par) < 0) {
      inc_str << "\n#include <" << par->name << ">";
      inc_list.Link(par);
    }
  }

  inc_str << "\n\n// member includes:";
  for(int i=0; i< td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(md->GetOwnerType() != td) continue;
    TypeDef* mtyp = md->type;
    if(mtyp->IsActualClassNoEff()) {
      if(inc_list.FindEl(mtyp) < 0) {
	inc_str << "\n#include <" << mtyp->name << ">";
	inc_list.Link(mtyp);
      }
    }
  }

  inc_str << "\n\n// declare all other types mentioned but not required to include:";
  for(int i=0; i< td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(md->GetOwnerType() != td) continue;
    TypeDef* cltyp = md->type->GetActualClassType();
    if(cltyp) {
      if(inc_list.FindEl(cltyp) < 0) {
	inc_str << "\nclass " << cltyp->name << "; // ";
	inc_list.Link(cltyp);
      }
    }
  }
  for(int i=0; i< td->methods.size; i++) {
    MethodDef* md = td->methods[i];
    if(md->GetOwnerType() != td) continue;
    { // return type
      TypeDef* argt = md->type;
      TypeDef* cltyp = argt->GetActualClassType();
      if(cltyp) {
	if(inc_list.FindEl(cltyp) < 0) {
	  inc_str << "\nclass " << cltyp->name << "; // ";
	  inc_list.Link(cltyp);
	}
      }
    }
    for(int j=0; j< md->arg_types.size; j++) {
      TypeDef* argt = md->arg_types[j];
      TypeDef* cltyp = argt->GetActualClassType();
      if(cltyp) {
	if(inc_list.FindEl(cltyp) < 0) {
	  inc_str << "\nclass " << cltyp->name << "; // ";
	  inc_list.Link(cltyp);
	}
      }
    }
  }
  inc_str << "\n\n\n";
  return inc_str;
}


bool taCodeUtils::CreateNewSrcFilesExisting(const String& type_nm, const String& top_path,
                                            const String& src_dir) {
  TypeDef* td = TypeDef::FindGlobalTypeName(type_nm);
  if(!td) {
    taMisc::Error("type not found:", type_nm);
    return false;
  }

  bool new_file = CreateNewSrcFiles(td->name, top_path, src_dir);
  String fname = td->name;

  String src_path = top_path + "/" + src_dir + "/";
  String hfile = src_path + fname + ".h";

  String hstr;
  hstr.LoadFromFile(hfile);

  if(hstr.contains("\n// parent includes:"))
    hstr = hstr.before("\n// parent includes:");
  else if(hstr.contains("\n#include"))
    hstr = hstr.before("\n#include");
  else
    hstr = hstr.before("\n#endif");

  String incs = TypeIncludes(td);

  hstr << incs;

  String cmd;
  cmd << "sed -n " << td->source_start << "," << td->source_end << "p "
      << td->source_file << " > " << src_path << "create_new_src_src";
  taMisc::ExecuteCommand(cmd);

  String srcstr;
  srcstr.LoadFromFile(src_path + "create_new_src_src");

  hstr << srcstr << "\n";

  hstr << "#endif // " << td->name << "_h\n";

  hstr.SaveToFile(src_path + fname + ".h");
  return true;
}

void taCodeUtils::CreateAllNewSrcFiles() {
  int_Array iary;
  int i=0;
  while(i < taMisc::types.size) {
    TypeDef* typ = taMisc::types.FastEl(i);
    // bool dbg = false;
    if(!typ->IsActualClass()) {
      // if(dbg) taMisc::Info("fail class, anchor");
      i++;
      continue;
    }
    if(!typ->source_file.contains("emergent/leabra")) {
      // if(dbg) taMisc::Info("fail src file");
      i++;
      continue;
    }
    if(typ->HasOption("INLINE")) {
      // if(dbg) taMisc::Info("fail inline");
      i++;
      continue;
    }
    int chs = taMisc::Choice("Fix new source file for: " + typ->name,
			     "Yes", "No", "Back", "Cancel");
    if(chs == 3) break;
    if(chs == 2) {
      if(iary.size > 0) {
	i = iary.Pop();
      }
      continue;
    }
    iary.Add(i);
    if(chs == 1) {
      i++;
      continue;
    }
    if(chs == 0) {
      CreateNewSrcFilesExisting(typ->name, "/home/oreilly/emergent",
                                "src/emergent/leabra");
      i++;
    }
  }
}

