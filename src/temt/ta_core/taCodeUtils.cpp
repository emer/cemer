// Copyright 2013-2017, Regents of the University of Colorado,
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
#include <SubversionClient>
#include <taDateTime>

#include <taMisc>

#include <QDir>

using namespace std;

String_Array taCodeUtils::emergent_src_paths;

bool taCodeUtils::InitSrcPaths() {
  if(emergent_src_paths.size > 0)
    return false;
  
  emergent_src_paths.Add("src/temt/ta_core");
  emergent_src_paths.Add("src/temt/ta_math");
  emergent_src_paths.Add("src/temt/ta_program");
  emergent_src_paths.Add("src/temt/ta_data");
  emergent_src_paths.Add("src/temt/ta_proj");
  emergent_src_paths.Add("src/temt/ta_gui");
  emergent_src_paths.Add("src/temt/ta_3d");
  emergent_src_paths.Add("src/temt/css");
  emergent_src_paths.Add("src/emergent/network");
  emergent_src_paths.Add("src/emergent/leabra");
  emergent_src_paths.Add("src/emergent/bp");
  emergent_src_paths.Add("src/emergent/cs");
  emergent_src_paths.Add("src/emergent/so");
  emergent_src_paths.Add("src/emergent/actr");
  emergent_src_paths.Add("src/emergent/virt_env");
  return true;
}

bool taCodeUtils::CreateNewSrcFiles(const String& type_nm, const String& top_path,
			       const String& src_dir) {
  String src_path = top_path + PATH_SEP + src_dir + PATH_SEP;
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
    str << "\n#include \"" << type_nm << ".h\"\n\n"
        << "TA_BASEFUNS_CTORS_DEFN(" << type_nm << ");\n\n";
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
    str << "#include \"../" << src_dir << PATH_SEP << type_nm << ".h\"\n";
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

bool taCodeUtils::CreateNewSpecFiles(const String& type_nm, const String& top_path,
			       const String& src_dir) {
  String src_path = top_path + PATH_SEP + src_dir + PATH_SEP;
  String crfile = src_path + "COPYRIGHT.txt";
  String cmfile = src_path + "CMakeFiles.txt";
  String hfile = src_path + type_nm + ".h";
  String cppfile = src_path + type_nm + ".cpp";
  String incfile = top_path + "/include/" + type_nm;
  String incfileh = incfile + ".h";

  String corehfile = src_path + type_nm + "_core.h";
  String corecppfile = src_path + type_nm + "_core.cpp";
  String coreincfile = top_path + "/include/" + type_nm + "_core";
  String coreincfileh = coreincfile + ".h";
  
  String mbrshfile = src_path + type_nm + "_mbrs.h";
  String mbrscppfile = src_path + type_nm + "_mbrs.cpp";
  String mbrsincfile = top_path + "/include/" + type_nm + "_mbrs";
  String mbrsincfileh = mbrsincfile + ".h";
  
  String cpphfile = src_path + type_nm + "_cpp.h";
  String cppcppfile = src_path + type_nm + "_cpp.cpp";
  String cppincfile = top_path + "/include/" + type_nm + "_cpp";
  String cppincfileh = cppincfile + ".h";

  String cudahfile = src_path + type_nm + "_cuda.h";
  String cudacppfile = src_path + type_nm + "_cuda.cpp";
  String cudaincfile = top_path + "/include/" + type_nm + "_cuda";
  String cudaincfileh = cudaincfile + ".h";
  
  if(!taMisc::FileExists(crfile)) {
    taMisc::Error("CreateNewSrcFiles: copyright file:", crfile, "not found -- paths must be wrong -- aborting");
    return false;
  }

  String crstr;
  crstr.LoadFromFile(crfile);

  bool got_one = false;

  //////////////////////////////////////////////
  // regular "main" headers
  
  if(taMisc::FileExists(hfile)) {
    taMisc::Warning("header file:", hfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n#ifndef " << type_nm << "_h\n"
        << "#define " << type_nm << "_h 1\n\n"
        << "// parent includes:\n"
        << "#include <UnitSpec> // replace with actual parent\n\n"
        << "// member includes:\n\n"
        << "// full standalone C++ implementation State code (include algo specific one)\n"
        << "#include <NetworkState_cpp>\n\n"
        << "#include <State_main>\n\n"
        << "// declare all other types mentioned but not required to include:\n\n"
        << "eTypeDef_Of(" << type_nm << ");\n\n"
        << "class E_API " << type_nm << " : public UnitSpec {\n"
        << "  // <describe here in full detail in one extended line comment>\n"
        << "INHERITED(UnitSpec)\n"
        << "public:\n\n"
        << "#include <" << type_nm << "_core>\n\n"
        << "  TA_SIMPLE_BASEFUNS(" << type_nm << ");\n"
        << "protected:\n"
        << "  SPEC_DEFAULTS;\n"
        << "private:\n"
        << "  void Initialize()  {\n"
        << "    Defaults_init(); // also init any non-defaults\n"
        << "  }\n"
        << "  void Defaults_init() {\n"
        << "  };\n"
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
    str << "\n#include \"" << type_nm << ".h\"\n\n"
        << "TA_BASEFUNS_CTORS_DEFN(" << type_nm << ");\n\n";
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
    str << "#include \"../" << src_dir << PATH_SEP << type_nm << ".h\"\n";
    str.SaveToFile(incfileh);
    taMisc::ExecuteCommand("svn add " + incfileh);
    got_one = true;
  }

  //////////////////////////////////////////////
  // CORE files
  
  if(taMisc::FileExists(corehfile)) {
    taMisc::Warning("header file:", corehfile, "already exists, not changing");
  }
  else {
    String str = "";            // no copyright!
    str << "// this contains core shared code, and is included directly in " << type_nm << ".h, _cpp.h, _cuda.h\n"
        << "//{\n"
        << "  INLINE virtual void SampleInlineFun() { do something; }\n"
        << "  // #CAT_State does something..\n\n"
        << "  INIMPL virtual void SampleNotInlineFun();\n"
        << "  // #CAT_State impl in _core.cpp\n\n"
        << "private:\n"
        << "  INLINE void Initialize_core() {\n"
        << "    // init all members -- called by constructors\n"
        << "  };\n\n";
    str.SaveToFile(corehfile);
    taMisc::ExecuteCommand("svn add " + corehfile);
    got_one = true;
  }

  if(taMisc::FileExists(corecppfile)) {
    taMisc::Warning("cpp file:", corecppfile, "already exists, not changing");
  }
  else {
    String str = ""; // no copyright
    str << "// contains core non-inline (INIMPL) functions from _core.h\n"
        << "// if used, include directly in " << type_nm << ".cpp, _cpp.cpp, _cuda.cpp\n\n";
    str.SaveToFile(corecppfile);
    taMisc::ExecuteCommand("svn add " + corecppfile);
    got_one = true;
  }

  if(taMisc::FileExists(coreincfile)) {
    taMisc::Warning("include file:", coreincfile, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"" << type_nm << "_core.h\"\n";
    str.SaveToFile(coreincfile);
    taMisc::ExecuteCommand("svn add " + coreincfile);
    got_one = true;
  }

  if(taMisc::FileExists(coreincfileh)) {
    taMisc::Warning("include file:", coreincfileh, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << PATH_SEP << type_nm << "_core.h\"\n";
    str.SaveToFile(coreincfileh);
    taMisc::ExecuteCommand("svn add " + coreincfileh);
    got_one = true;
  }

  //////////////////////////////////////////////
  // MBRS files

  if(taMisc::FileExists(mbrshfile)) {
    taMisc::Warning("header file:", mbrshfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n// this contains all full classes that appear as members of _core specs\n"
        << "// it must be included directly in " << type_nm << ".h, _cpp.h, _cuda.h\n"
        << "// the STATE_CLASS macro will define a _cpp _cuda or plain (no suffix) version\n\n"
        << "// this pragma ensures that maketa properly grabs this type information even though\n"
        << "// this file is included in the other files -- we get ta info for main and _cpp, not cuda\n"
        << "#pragma maketa_file_is_target " << type_nm << "\n"
        << "#pragma maketa_file_is_target " << type_nm << "_cpp\n\n"
        << "class STATE_CLASS(MySpecMember) : public STATE_CLASS(SpecMemberBase) {\n"
        << "  // ##NO_TOKENS ##INLINE #NO_UPDATE_AFTER ##CAT_Network specifies ??\n"
        << "INHERITED(SpecMemberBase)\n"
        << "public:\n\n"
        << "  INLINE virtual void SampleInlineFun() { do something; }\n"
        << "  // does something..\n\n"
        << "  INIMPL virtual void SampleNotInlineFun();\n"
        << "  // in _mbrs.cpp -- generally try to put all inline\n\n"
        << "  STATE_DECO_KEY(\"UnitSpec\");\n"
        << "  STATE_TA_STD_CODE_SPEC(MySpecMember);\n"
        << "  // STATE_UAE( update after edit code here; );\n"
        << "private:\n"
        << "  void  Initialize() {\n"
        << "    mbr = 1.0f; // initialize all vals here\n"
        << "    Defaults_init();\n"
        << "  }\n"
        << "  void  Defaults_init() {\n"
        << "    def_mbr = 1.0f; // all strong defaults go here\n"
        << "  }\n"
        << "};\n\n";

    str.SaveToFile(mbrshfile);
    taMisc::ExecuteCommand("svn add " + mbrshfile);
    got_one = true;
  }

  if(taMisc::FileExists(mbrscppfile)) {
    taMisc::Warning("cpp file:", mbrscppfile, "already exists, not changing");
  }
  else {
    String str = ""; // no copyright
    str << "// contains mbrs non-inline (INIMPL) functions from _mbrs.h\n"
        << "// if used, must be included directly in " << type_nm << ".cpp, _cpp.cpp, _cuda.cpp\n\n"
        << "// NOTE: the main " << type_nm << ".cpp file must have TA_BASEFUNS_CTORS_DEFN(MySpecMember);\n"
        << "// for all classes defined in _mbrs.h\n\n";
    str.SaveToFile(mbrscppfile);
    taMisc::ExecuteCommand("svn add " + mbrscppfile);
    got_one = true;
  }

  if(taMisc::FileExists(mbrsincfile)) {
    taMisc::Warning("include file:", mbrsincfile, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"" << type_nm << "_mbrs.h\"\n";
    str.SaveToFile(mbrsincfile);
    taMisc::ExecuteCommand("svn add " + mbrsincfile);
    got_one = true;
  }

  if(taMisc::FileExists(mbrsincfileh)) {
    taMisc::Warning("include file:", mbrsincfileh, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << PATH_SEP << type_nm << "_mbrs.h\"\n";
    str.SaveToFile(mbrsincfileh);
    taMisc::ExecuteCommand("svn add " + mbrsincfileh);
    got_one = true;
  }

  
  //////////////////////////////////////////////
  // CPP files

  if(taMisc::FileExists(cpphfile)) {
    taMisc::Warning("header file:", cpphfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n#ifndef " << type_nm << "_cpp_h\n"
        << "#define " << type_nm << "_cpp_h 1\n\n"
        << "// raw C++ (cpp) version of spec -- ideally no emergent / ta dependencies\n\n"
        << "// parent includes:\n"
        << "#include <UnitSpec_cpp> // replace with actual parent\n\n"
        << "#include <State_cpp>\n\n"
        << "// member includes:\n\n"
        << "#include <" << type_nm << "_mbrs.h>\n\n"
        << "// declare all other types mentioned but not required to include:\n\n"
        << "class " << type_nm << "_cpp : public UnitSpec_cpp {\n"
        << "  // <describe here in full detail in one extended line comment>\n"
        << "INHERITED(UnitSpec)\n"
        << "public:\n\n"
        << "#include <" << type_nm << "_core>\n\n"
        << "  " << type_nm << "_cpp() { Initialize_core(); }\n"
        << "};\n\n"
        << "#endif // " << type_nm << "_cpp_h\n";

    str.SaveToFile(cpphfile);
    taMisc::ExecuteCommand("svn add " + cpphfile);
    got_one = true;
  }

  if(taMisc::FileExists(cppcppfile)) {
    taMisc::Warning("cpp file:", cppcppfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n// contains cpp non-inline (INIMPL) functions from _cpp.h\n\n"
        << "#include \"" << type_nm << "_cpp.h\"\n\n"
        << "// #include \"" << type_nm << "_mbrs.cpp\"   // include member impls if defined\n\n"
        << "// #include \"" << type_nm << "_core.cpp\"   // include member impls if defined\n\n";
    str.SaveToFile(cppcppfile);
    taMisc::ExecuteCommand("svn add " + cppcppfile);
    got_one = true;
  }

  if(taMisc::FileExists(cppincfile)) {
    taMisc::Warning("include file:", cppincfile, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"" << type_nm << "_cpp.h\"\n";
    str.SaveToFile(cppincfile);
    taMisc::ExecuteCommand("svn add " + cppincfile);
    got_one = true;
  }

  if(taMisc::FileExists(cppincfileh)) {
    taMisc::Warning("include file:", cppincfileh, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << PATH_SEP << type_nm << "_cpp.h\"\n";
    str.SaveToFile(cppincfileh);
    taMisc::ExecuteCommand("svn add " + cppincfileh);
    got_one = true;
  }

  
  //////////////////////////////////////////////
  // CUDA files

  if(taMisc::FileExists(cudahfile)) {
    taMisc::Warning("header file:", cudahfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n#ifndef " << type_nm << "_cuda_h\n"
        << "#define " << type_nm << "_cuda_h 1\n\n"
        << "// raw C++ (cuda) version of spec -- no emergent / ta dependencies\n\n"
        << "// parent includes:\n"
        << "#include <UnitSpec_cuda> // replace with actual parent\n\n"
        << "#include <State_cuda>\n\n"
        << "// member includes:\n\n"
        << "#include <" << type_nm << "_mbrs.h>\n\n"
        << "// declare all other types mentioned but not required to include:\n\n"
        << "class " << type_nm << "_cuda : public UnitSpec_cuda {\n"
        << "  // <describe here in full detail in one extended line comment>\n"
        << "INHERITED(UnitSpec)\n"
        << "public:\n\n"
        << "#include <" << type_nm << "_core>\n\n"
        << "  " << type_nm << "_cuda() { Initialize_core(); }\n"
        << "};\n\n"
        << "#endif // " << type_nm << "_cuda_h\n";

    str.SaveToFile(cudahfile);
    taMisc::ExecuteCommand("svn add " + cudahfile);
    got_one = true;
  }

  if(taMisc::FileExists(cudacppfile)) {
    taMisc::Warning("cuda file:", cudacppfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n// contains cuda non-inline (INIMPL) functions from _cuda.h\n\n"
        << "#include \"" << type_nm << "_cuda.h\"\n\n"
        << "// #include \"" << type_nm << "_mbrs.cpp\"   // include member impls if defined\n\n"
        << "// #include \"" << type_nm << "_core.cpp\"   // include member impls if defined\n\n";
    str.SaveToFile(cudacppfile);
    taMisc::ExecuteCommand("svn add " + cudacppfile);
    got_one = true;
  }

  if(taMisc::FileExists(cudaincfile)) {
    taMisc::Warning("include file:", cudaincfile, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"" << type_nm << "_cuda.h\"\n";
    str.SaveToFile(cudaincfile);
    taMisc::ExecuteCommand("svn add " + cudaincfile);
    got_one = true;
  }

  if(taMisc::FileExists(cudaincfileh)) {
    taMisc::Warning("include file:", cudaincfileh, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << PATH_SEP << type_nm << "_cuda.h\"\n";
    str.SaveToFile(cudaincfileh);
    taMisc::ExecuteCommand("svn add " + cudaincfileh);
    got_one = true;
  }

  
  //////////////////////////////////////////////
  // CmakeFiles.txt files
  
  String cmstr;
  cmstr.LoadFromFile(cmfile);
  bool changed = false;

  if(!cmstr.contains(type_nm + ".h")) {
    if(cmstr.contains(".h\n)")) {
      String cmrest = cmstr.after(".h\n)");
      cmstr = cmstr.before(".h\n)");
      cmstr << ".h\n  " << type_nm << "_cpp.h\n"
            << "# note: include _core.h, _mbrs.h in non-maketa headers, cuda.h in cuda section\n"
            << "  " << type_nm << ".h\n)" << cmrest;
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
      cmstr << ".cpp\n  " << type_nm << "_cpp.cpp\n"
            << "# note: include cuda.cpp in cuda section -- need .cu symlinks -- ignore _core.cpp, _mbrs.cpp\n"
            << "  " << type_nm << ".cpp\n)" << cmrest;
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

bool taCodeUtils::CreateNewStateFiles(const String& type_nm, const String& top_path,
                                      const String& src_dir) {
  String src_path = top_path + PATH_SEP + src_dir + PATH_SEP;
  String crfile = src_path + "COPYRIGHT.txt";
  String cmfile = src_path + "CMakeFiles.txt";

  String corehfile = src_path + type_nm + "_core.h";
  String corecppfile = src_path + type_nm + "_core.cpp";
  String coreincfile = top_path + "/include/" + type_nm + "_core";
  String coreincfileh = coreincfile + ".h";
  
  String cpphfile = src_path + type_nm + "_cpp.h";
  String cppcppfile = src_path + type_nm + "_cpp.cpp";
  String cppincfile = top_path + "/include/" + type_nm + "_cpp";
  String cppincfileh = cppincfile + ".h";

  String cudahfile = src_path + type_nm + "_cuda.h";
  String cudacppfile = src_path + type_nm + "_cuda.cpp";
  String cudaincfile = top_path + "/include/" + type_nm + "_cuda";
  String cudaincfileh = cudaincfile + ".h";
  
  if(!taMisc::FileExists(crfile)) {
    taMisc::Error("CreateNewSrcFiles: copyright file:", crfile, "not found -- paths must be wrong -- aborting");
    return false;
  }

  String crstr;
  crstr.LoadFromFile(crfile);

  bool got_one = false;

  //////////////////////////////////////////////
  // CORE files
  
  if(taMisc::FileExists(corehfile)) {
    taMisc::Warning("header file:", corehfile, "already exists, not changing");
  }
  else {
    String str = "";            // no copyright!
    str << "// this contains core shared code, and is included directly in " << type_nm << " _cpp.h, _cuda.h\n"
        << "//{\n"
        << "  float     state_var; // this is my state variable\n\n"
        << "  INLINE virtual void SampleInlineFun() { do something; }\n"
        << "  // #CAT_State does something..\n\n"
        << "  INIMPL virtual void SampleNotInlineFun();\n"
        << "  // #CAT_State impl in _core.cpp\n\n"
        << "  INLINE void Initialize_core() {\n"
        << "    // init all members -- called by constructors\n"
        << "  };\n\n";
    str.SaveToFile(corehfile);
    taMisc::ExecuteCommand("svn add " + corehfile);
    got_one = true;
  }

  if(taMisc::FileExists(corecppfile)) {
    taMisc::Warning("cpp file:", corecppfile, "already exists, not changing");
  }
  else {
    String str = ""; // no copyright
    str << "// contains core non-inline (INIMPL) functions from _core.h\n"
        << "// if used, include directly in " << type_nm << ".cpp, _cpp.cpp, _cuda.cpp\n\n";
    str.SaveToFile(corecppfile);
    taMisc::ExecuteCommand("svn add " + corecppfile);
    got_one = true;
  }

  if(taMisc::FileExists(coreincfile)) {
    taMisc::Warning("include file:", coreincfile, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"" << type_nm << "_core.h\"\n";
    str.SaveToFile(coreincfile);
    taMisc::ExecuteCommand("svn add " + coreincfile);
    got_one = true;
  }

  if(taMisc::FileExists(coreincfileh)) {
    taMisc::Warning("include file:", coreincfileh, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << PATH_SEP << type_nm << "_core.h\"\n";
    str.SaveToFile(coreincfileh);
    taMisc::ExecuteCommand("svn add " + coreincfileh);
    got_one = true;
  }

  //////////////////////////////////////////////
  // CPP files

  if(taMisc::FileExists(cpphfile)) {
    taMisc::Warning("header file:", cpphfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n#ifndef " << type_nm << "_cpp_h\n"
        << "#define " << type_nm << "_cpp_h 1\n\n"
        << "// raw C++ (cpp) version of state -- no emergent / ta dependencies\n\n"
        << "// parent includes:\n"
        << "#include <UnitState_cpp> // replace with actual parent\n\n"
        << "#include <State_cpp>\n\n"
        << "// member includes:\n\n"
        << "// declare all other types mentioned but not required to include:\n\n"
        << "class " << type_nm << "_cpp : public UnitState_cpp {\n"
        << "  // <describe here in full detail in one extended line comment>\n"
        << "INHERITED(UnitState)\n"
        << "public:\n\n"
        << "#include <" << type_nm << "_core>\n\n"
        << "  " << type_nm << "_cpp() { Initialize_core(); }\n"
        << "};\n\n"
        << "#endif // " << type_nm << "_cpp_h\n";

    str.SaveToFile(cpphfile);
    taMisc::ExecuteCommand("svn add " + cpphfile);
    got_one = true;
  }

  if(taMisc::FileExists(cppcppfile)) {
    taMisc::Warning("cpp file:", cppcppfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n// contains cpp non-inline (INIMPL) functions from _cpp.h\n\n"
        << "#include \"" << type_nm << "_cpp.h\"\n\n"
        << "// #include \"" << type_nm << "_core.cpp\"   // include member impls if defined\n\n";
    str.SaveToFile(cppcppfile);
    taMisc::ExecuteCommand("svn add " + cppcppfile);
    got_one = true;
  }

  if(taMisc::FileExists(cppincfile)) {
    taMisc::Warning("include file:", cppincfile, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"" << type_nm << "_cpp.h\"\n";
    str.SaveToFile(cppincfile);
    taMisc::ExecuteCommand("svn add " + cppincfile);
    got_one = true;
  }

  if(taMisc::FileExists(cppincfileh)) {
    taMisc::Warning("include file:", cppincfileh, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << PATH_SEP << type_nm << "_cpp.h\"\n";
    str.SaveToFile(cppincfileh);
    taMisc::ExecuteCommand("svn add " + cppincfileh);
    got_one = true;
  }

  
  //////////////////////////////////////////////
  // CUDA files

  if(taMisc::FileExists(cudahfile)) {
    taMisc::Warning("header file:", cudahfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n#ifndef " << type_nm << "_cuda_h\n"
        << "#define " << type_nm << "_cuda_h 1\n\n"
        << "// raw C++ (cuda) version of state -- no emergent / ta dependencies\n\n"
        << "// parent includes:\n"
        << "#include <UnitState_cuda> // replace with actual parent\n\n"
        << "#include <State_cuda>\n\n"
        << "// member includes:\n\n"
        << "// declare all other types mentioned but not required to include:\n\n"
        << "class " << type_nm << "_cuda : public UnitState_cuda {\n"
        << "  // <describe here in full detail in one extended line comment>\n"
        << "INHERITED(UnitState)\n"
        << "public:\n\n"
        << "#include <" << type_nm << "_core>\n\n"
        << "  INLINE " << type_nm << "_cuda() { Initialize_core(); }\n"
        << "};\n\n"
        << "#endif // " << type_nm << "_cuda_h\n";

    str.SaveToFile(cudahfile);
    taMisc::ExecuteCommand("svn add " + cudahfile);
    got_one = true;
  }

  if(taMisc::FileExists(cudacppfile)) {
    taMisc::Warning("cuda file:", cudacppfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n// contains cuda non-inline (INIMPL) functions from _cuda.h\n\n"
        << "#include \"" << type_nm << "_cuda.h\"\n\n"
        << "// #include \"" << type_nm << "_core.cpp\"   // include impls if defined\n\n";
    str.SaveToFile(cudacppfile);
    taMisc::ExecuteCommand("svn add " + cudacppfile);
    got_one = true;
  }

  if(taMisc::FileExists(cudaincfile)) {
    taMisc::Warning("include file:", cudaincfile, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"" << type_nm << "_cuda.h\"\n";
    str.SaveToFile(cudaincfile);
    taMisc::ExecuteCommand("svn add " + cudaincfile);
    got_one = true;
  }

  if(taMisc::FileExists(cudaincfileh)) {
    taMisc::Warning("include file:", cudaincfileh, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << PATH_SEP << type_nm << "_cuda.h\"\n";
    str.SaveToFile(cudaincfileh);
    taMisc::ExecuteCommand("svn add " + cudaincfileh);
    got_one = true;
  }

  
  //////////////////////////////////////////////
  // CmakeFiles.txt files
  
  String cmstr;
  cmstr.LoadFromFile(cmfile);
  bool changed = false;

  if(!cmstr.contains(type_nm + ".h")) {
    if(cmstr.contains(".h\n)")) {
      String cmrest = cmstr.after(".h\n)");
      cmstr = cmstr.before(".h\n)");
      cmstr << ".h\n  " << type_nm << "_cpp.h\n)" << cmrest;
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
      cmstr << ".cpp\n  " << type_nm << "_cpp.cpp\n)" << cmrest;
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

  String src_path = top_path + PATH_SEP + src_dir + PATH_SEP;
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
  String src_path = top_path + PATH_SEP + src_dir + PATH_SEP;

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

  InitSrcPaths();
  for(int i=0; i<emergent_src_paths.size; i++) {
    String path = emergent_src_paths[i];
    ReplaceInDir(search_nm, repl_nm, top_path, path);
  }
  ReplaceInDir(search_nm, repl_nm, top_path, "include");
  return true;
}

bool taCodeUtils::RemoveType(const String& type_nm,
                             const String& top_path, const String& src_dir) {
  TypeDef* td = TypeDef::FindGlobalTypeName(type_nm);
  if(!td) {
    taMisc::Error("RemoveType: type not found:", type_nm);
    return false;
  }

  String src_path = top_path + PATH_SEP + src_dir + PATH_SEP;
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

  InitSrcPaths();
  QDir dir(inc_path);
  QStringList filters;
  filters << "*";
  QStringList files = dir.entryList(filters);
  for(int i=0; i<files.count(); i++) {
    String fnm = files[i];
    if(fnm.endsWith(".h")) continue; // only not-h guys
    int rpl = 0;
    for(int j=0; j<emergent_src_paths.size; j++) {
      String path = emergent_src_paths[j];
      rpl += taMisc::ReplaceStringInFile(inc_path + fnm, "../" + path, "./");
    }
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

  String src_path = top_path + PATH_SEP + src_dir + PATH_SEP;
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

String taCodeUtils::GetCurSvnRevYear(const String& filename, int& last_change_rev) {
  String yr = QDate::currentDate().toString("yyyy");

  SubversionClient svn_client;
  int rev = 0;
  int kind = 0;
  String root_url;
  last_change_rev = 0;
  int last_changed_date = 0;
  String last_changed_author;
  int64_t sz = 0;
  try {
    svn_client.GetInfo(filename, rev, kind, root_url, last_change_rev, last_changed_date,
                        last_changed_author, sz);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in GetInfo\n", ex.what());
    return yr;
  }

  if(last_changed_date == 0) {
    taMisc::Info("Subversion client did not return proper date info for:",filename);
    return yr;
  }    
  
  yr = taDateTime::fmTimeToString(last_changed_date, "yyyy");
  // taMisc::Info("svn info:", filename, "rev:", String(last_change_rev), "author:",
  //              last_changed_author, "year:", yr);
  
  return yr;
}

String taCodeUtils::GetFirstSvnRevYear(const String& filename, int last_change_rev) {
  String yr = "2017";

  SubversionClient svn_client;
  // now try to find prior revision!
  int_PArray    revs_good;       // one per rev -- revision number
  int_PArray    revs;            // one per rev -- revision number
  String_PArray commit_msgs;     // one per rev -- the commit message
  String_PArray authors;         // one per rev -- author of rev
  int_PArray    times_good;      // one per rev -- time as secs since 1970
  int_PArray    times;           // one per rev -- time as secs since 1970
  int_PArray    files_start_idx; // one per rev -- starting index in files/actions
  int_PArray    files_n;    // one per rev -- number of files in files/actions
  String_PArray files;      // raw list of all files for all logs
  String_PArray actions; // one-to-one with files, mod action for each file

  String url;
  try {
    svn_client.GetUrlFromPath(url, filename);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in GetUrl\n", ex.what());
    return yr;
  }

  int prev_rev = last_change_rev - 1;
  int search_chunk = 100;
  while(true) {
    try {
      svn_client.GetLogs(revs, commit_msgs, authors, times, files_start_idx, files_n,
                          files, actions, url, prev_rev, search_chunk);
    }
    catch (const SubversionClient::Exception &ex) {
      break;
    }
    prev_rev -= search_chunk;
    if(prev_rev < search_chunk) {
      break;
    }
    revs_good = revs;
    times_good = times;
  }

  if(revs_good.size == 0) {
    taMisc::Info("No first revision found!");
    return yr;
  }

  int lowest_rev = revs_good[0];
  int lowest_idx = 0;
  for(int i=0; i<revs_good.size; i++) {
    int revt = revs_good[i];
    if(revt < lowest_rev) {
      lowest_rev = revt;
      lowest_idx = i;
    }
  }
  if(lowest_idx < 0) return yr;
  
  yr = taDateTime::fmTimeToString(times_good[lowest_idx], "yyyy");
  // taMisc::Info("svn info:", filename, "rev:", String(last_change_rev), "author:",
  //              last_changed_author, "year:", yr);
  
  return yr;
}

bool taCodeUtils::CopyrightUpdateFile(const String& filename) {
  String srcstr;
  srcstr.LoadFromFile(filename);

  bool mod = false;
  bool st_updt = false;
  bool cur_updt = false;

  String cpyright = "// Copyright";
  int cpyidx = srcstr.index(cpyright);
  if(cpyidx < 0) {
    taMisc::Info("could not find copyright string in file:", filename);
    return false;
  }

  int nxtidx = cpyidx + cpyright.length();
  if(srcstr[nxtidx] == ',') {
    srcstr.del(nxtidx, 1);      // don't include , anymore
    mod = true;
  }
  nxtidx++;

  int ecomma = srcstr.index(',', nxtidx);
  String curyr = srcstr.at(nxtidx, ecomma-nxtidx);
  String styr;
  int dashidx = curyr.index('-');
  if(dashidx > 0) {             // deal with dashed-year
    int est = nxtidx + dashidx;
    styr = curyr.before('-');
    curyr = curyr.after('-');
  }

  int last_change_rev = 0;
  String yr = GetCurSvnRevYear(filename, last_change_rev);
  
  if(styr.empty()) {
    styr = GetFirstSvnRevYear(filename, last_change_rev);
    srcstr = srcstr.at(0, nxtidx) + styr + "-" + srcstr.at(nxtidx, srcstr.length()-nxtidx);
    mod = true;
    dashidx = nxtidx + 5;
    st_updt = true;
  }

  if(yr != curyr) {
    for(int i=0; i<yr.length();i++) {
      srcstr[dashidx+1 + i] = yr[i];
    }
    mod = true;
    cur_updt = true;
  }

  if(mod) {
    srcstr.SaveToFile(filename);
    if(st_updt) {
      taMisc::Info("Updated copyright start year to:", styr, "in:",filename);
    }
    if(cur_updt) {
      taMisc::Info("Updated copyright end year to:", yr, "in:",filename);
    }
  }
  else {
    taMisc::Info("copyright was already up-to-date:", yr, "in:",filename);
  }

  return mod;
}

bool taCodeUtils::CopyrightUpdateDir(const String& top_path, const String& src_dir) {
  String src_path = top_path + PATH_SEP + src_dir + PATH_SEP;

  QDir dir(src_path);
  QStringList filters;
  filters << "*.cpp" << "*.h" << "*.y";
  QStringList files = dir.entryList(filters);
  for(int i=0; i<files.count(); i++) {
    String fnm = files[i];

    if(fnm.endsWith("_parse.cpp") || fnm.endsWith("_parse.h")) continue;
    if(fnm == "y.tab.h") continue;
    if(fnm.endsWith("_qrc.cpp")) continue;
    if(fnm.startsWith("vector") && fnm.endsWith(".h")) continue;
    if(fnm == "instrset.h") continue;
    if(fnm.startsWith("nifti")) continue;
    if(fnm == "znzlib.h") continue;
    
    CopyrightUpdateFile(src_path + fnm);
  }
  return true;
}

bool taCodeUtils::CopyrightUpdateAllFiles(const String& top_path) {
  // taMisc::Info("display width:", String(taMisc::display_width));

  InitSrcPaths();
  for(int i=0; i<emergent_src_paths.size; i++) {
    String path = emergent_src_paths[i];
    CopyrightUpdateDir(top_path, path);
  }
  CopyrightUpdateDir(top_path, "src/temt/maketa");
  return true;
}

