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


// mta_constr.cpp

#include "mta_constr.h"
#include <BuiltinTypeDefs>

#include <taMisc>

// this is the offset of arg indexes into css-passed arg strings relative to the actual
// type scanned arg values, which start at 0
// 0 = instruction element??
// 1 = 'this' pointer for methods
// 2+ = actual args
static const int stub_arg_off = 2;

#define PAR_ARG_COUNT   6

bool MTA::TypeDef_Gen_Test(TypeDef* ths) {
  if(ths->IsNotActual()) return false; // only actual types!
  if(trg_fname_only != taMisc::GetFileFmPath(ths->source_file)) {
    if(verbose >= 4) {
      cerr << "mta_constr: skipping out-of-source type: " << ths->name 
           << " src: " << ths->source_file << endl;
    }
    return false; // not from target file!
  }
  if(ths->HasOption("IGNORE")) {
    if(verbose >= 2) {
      cerr << "mta_constr: skipping ignored type: " << ths->name << endl;
    }
    return false;
  }
  if(ths->IsTemplInst()) {
    // if(verbose >= 2) {
    //   cerr << "considering TI: " << ths->name << " chld sz: " << ths->children.size << endl;
    // }
    if(ths->children.size > 0) {
      TypeDef* chld = ths->children[0];
      return TypeDef_Gen_Test(chld); // we get instantiated where first parent lives!
    }
    return false;
  }
  return true;
}

String MTA::TypeDef_Gen_TypeName(TypeDef* ths) {
  if(ths->owner == NULL) {
    cerr << "W!!: Warning: referring to unowned type: " << ths->name << "\n";
    return "void";
  }
  if(ths->IsSubType()) {
    return "\"" + ths->owner->owner->name + "::" + ths->name + "\"";
  }
  else {
    return "\"" + ths->name + "\"";
  }
}

String MTA::TypeDef_Gen_TypeDef_Ptr(TypeDef* ths) {
  String rval = TypeDef_Gen_TypeDef_Ptr_impl(ths);
  if(!ths->IsSubType()) {
    return String("&") + rval;
  }
  return rval;
}

String MTA::TypeDef_Gen_TypeDef_Ptr_impl(TypeDef* ths) {
  if(ths->owner == NULL) {
    cerr << "W!!: Warning: referring to unowned type: " << ths->name << "\n";
    return "TA_void";
  }
  if(ths->IsSubType()) {
    String rval = TypeDef_Gen_TypeDef_Ptr_Path(ths->owner->owner) + ths->owner->name;
    rval += ".FindName(\"" + ths->name + "\")";
    return rval;
  }
  else {
    return "TA_" + ths->name;
  }
}

String MTA::TypeDef_Gen_TypeDef_Ptr_Path(TypeDef* ths) {
  String rval = TypeDef_Gen_TypeDef_Ptr_impl(ths);
  if(ths->IsSubType()) {
    return rval + "->";
  }
  else {
    return rval + ".";
  }
}

void MTA::TypeDef_FixOpts(String_PArray& op) {
  op.RemoveEl("INSTANCE");
  op.RemoveEl("NO_TOKENS");
}

String MTA::VariantToTargetConversion(TypeDef* param_td) {
  String conv;
  if (param_td) { // better exist!
    if (param_td->IsTaBase() && (param_td->IsPointer())) {
      conv = "dynamic_cast<" + param_td->GetNonPtrType()->Get_C_Name() +
        "*>(val.toBase())";
    }
    else { // anything else -- if illegal, will show up as compile error in TA.cpp
//      conv = "(" + param_td->GetNonConstType()->GetNonRefType()->Get_C_Name() + ")VarCvt(val)";
      conv = "VarCvt(val)";
    }
  }
  return conv;
}


//////////////////////////////////////////
// 	Overall Gen Entry Point

void MTA::TypeSpace_Gen(TypeSpace* ths, ostream& strm) {
  strm << "// File Automatically Gend by MakeTA\n"
       << "// DO NOT EDIT\n\n\n";

  TypeSpace_Includes(ths, strm);
  TypeSpace_Gen_Instances(ths, strm);
  TypeSpace_Gen_TypeDefs(ths, strm);
  TypeSpace_Gen_Stubs(ths, strm);
  TypeSpace_Gen_Data(ths, strm);
  TypeSpace_Gen_TypeInit(ths, strm);
  TypeSpace_Gen_DataInit(ths, strm);
  TypeSpace_Gen_InstInit(ths, strm);
}


//////////////////////////////////////////
// 	Includes

void MTA::TypeSpace_Includes(TypeSpace* ths, ostream& strm, bool instances) {
  strm << "#define __TA_COMPILE__\n";
  strm << "#include <TypeDef>\n";
  if(!instances) {
    strm << "#include \"ta_type_constr.h\"\n";
    if(gen_css) {
      strm << "#include \"css_basic_types.h\"\n";
      strm << "#include \"css_c_ptr_types.h\"\n";
      strm << "#include \"css_ta.h\"\n";
    }
  }

  String nstr = trg_fname_only;
  if(nstr.contains("streambuf") || nstr.contains("iostream") ||
     nstr.contains("fstream") || nstr.contains("sstream")) {
  }
  else {
    strm << "#include \"" << nstr << "\"\n";
  }
  strm << "#include <BuiltinTypeDefs>\n";
  strm << "#include <taMisc>\n\n";

  String_PArray ti_incs;

  // include type classes for template instances!  prevents a load of compile errs
  for(int i=0; i<taMisc::types.size; i++) {
    TypeDef* ths = taMisc::types.FastEl(i);
    if(!TypeDef_Gen_Test(ths)) continue;
    if(ths->IsTemplInst() && ths->IsClass()) {
      for(int j=0; j< ths->templ_pars.size; j++) {
        TypeDef* tp = ths->templ_pars[j];
        if(tp->IsActualClass()) {
          String src = taMisc::GetFileFmPath(tp->source_file);
          if(src != trg_fname_only) {
            if(src.empty())
              ti_incs.AddUnique("<" + tp->name + ">");
            else
              ti_incs.AddUnique("\"" + src + "\"");
          }
        }
      }
    }
  }
  if(ti_incs.size > 0) {
    strm << "\n// include template instance paramter files -- fixes a lot of bugs" << endl;
    for(int i=0; i<ti_incs.size; i++) {
      strm << "#include " << ti_incs[i] << "\n";
    }
    strm << "\n";
  }

  strm << "using namespace std;\n";
  strm << "\n\n";
}


//////////////////////////////////////////
// 	Type Instances

void MTA::TypeDef_Gen_Instances(TypeDef* ths, ostream& strm) {
  if(!TypeDef_Gen_Test(ths)) return;
  if(ths->IsTemplate()) return; // no instances for that guy!

  if((this->gen_instances || ths->HasOption("INSTANCE"))
     && !ths->HasOption("NO_INSTANCE")) {
#ifdef TA_OS_WIN
    if (win_dll)
      strm << win_dll_str << " ";
#endif
    strm << ths->Get_C_Name() << "*\t\t TAI_" << ths->name << "=NULL;\n";
  }
}

void MTA::TypeSpace_Gen_Instances(TypeSpace* ths, ostream& strm) {
  strm << "// Instances\n\n";
  for(int i=0; i<ths->size; i++) {
    TypeDef_Gen_Instances(ths->FastEl(i), strm);
  }
}


//////////////////////////////////////////
// 	TypeDef constructors 

void MTA::TypeSpace_Gen_TypeDefs(TypeSpace* ths, ostream& strm) {
  strm << "\n// TypeDefs\n\n";
  for(int i=0; i<ths->size; i++) {
    TypeDef_Gen_TypeDefs(ths->FastEl(i), strm);
  }
}

void MTA::TypeDef_Gen_TypeDefs(TypeDef* ths, ostream& strm) {
  if(!TypeDef_Gen_Test(ths)) return;
#ifdef TA_OS_WIN
  if (win_dll)
    strm << win_dll_str << " ";
#endif
    strm << "TypeDef TA_" << ths->name;
  TypeDef_Gen_TypeDefs_impl(ths, strm);
}

void MTA::TypeDef_Gen_TypeDefs_impl(TypeDef* ths, ostream& strm) {
  String_PArray act_opts = ths->opts;
  String_PArray act_inh_opts = ths->inh_opts;

  TypeDef_FixOpts(act_opts);
  TypeDef_FixOpts(act_inh_opts);

  String str_opts = taMisc::StrArrayToChar(act_opts);
  String str_inh_opts = taMisc::StrArrayToChar(act_inh_opts);
  String str_lists = taMisc::StrArrayToChar(ths->lists);

  strm << "(\"" << ths->name << "\", \"" << ths->desc << "\", ";
  strm << "\n\t\"" << str_inh_opts << "\", \"" << str_opts << "\", \""
       << str_lists << "\",\n";
  strm << "  \"" << ths->source_file << "\", " << String(ths->source_start)
       << ", " << String(ths->source_end) << ",\n";

  // type_flags:
  strm << "  " << ths->GetTypeEnumString() << ", ";

  // size:
  if(ths->IsTemplate() || ths->IsFunction()) {
    strm << "0, 0";
  }
  else {
    if(ths->IsEnum())
      strm << "sizeof(int), ";
    else
      strm << "sizeof(" << ths->Get_C_Name() << "), ";
    strm << "(void**)";
    if((gen_instances || ths->HasOption("INSTANCE"))
       && !ths->HasOption("NO_INSTANCE") && !ths->IsTemplate())
      strm << "&TAI_" << ths->name;
    else
      strm << "0";
  }

  if(ths->tokens.keep)        strm << ", 1";
  else                        strm << ", 0";
  strm << ", 1);\n";           // last true makes it global object
}


/////////////////////////////////////////
//   css method stubs	

void MTA::TypeSpace_Gen_Stubs(TypeSpace* ths, ostream& strm) {
  spc_typedef_gen.Reset();
  strm << "\n// css Method Stubs: extern TypeDef's referred to\n\n";
  for(int i=0; i<ths->size; i++) {
    TypeDef_Gen_Stubs(ths->FastEl(i), strm, true);
  }

  TypeSpace_Gen_TypeDefOf(&spc_typedef_gen, strm);

  strm << "\n// css Method Stubs\n\n";
  for(int i=0; i<ths->size; i++) {
    TypeDef_Gen_Stubs(ths->FastEl(i), strm, false);
  }
}

void MTA::TypeDef_Gen_Stubs(TypeDef* ths, ostream& strm, bool add_typedefs) {
  if(!TypeDef_Gen_Test(ths)) return;
  if(ths->IsTemplate()) return;

  // this is just for reg_fun
  if(ths->IsFunction() && this->gen_css && !ths->HasOption("NO_CSS"))
    MethodSpace_Gen_Stubs(&(ths->methods), ths, strm, add_typedefs);

  if(!ths->IsActualClass()) return;

  if(this->gen_css && !ths->HasOption("NO_CSS"))
    MethodSpace_Gen_Stubs(&(ths->methods), ths, strm, add_typedefs);

  if(!add_typedefs) {
    // generate property stubs
    MemberSpace_Gen_PropStubs(&(ths->members), ths, strm);
    MethodSpace_Gen_PropStubs(&(ths->methods), ths, strm);
  }
}


void MTA::TypeSpace_Gen_TypeDefOf(TypeSpace* ths, ostream& strm) {
  for(int i=0; i<ths->size; i++) {
    TypeDef_Gen_TypeDefOf(ths->FastEl(i), strm);
  }
}

void MTA::TypeDef_Gen_TypeDefOf(TypeDef* ths, ostream& strm) {
  strm << "TypeDef_Of(" << ths->name << ");\n";
}

void MTA::MethodSpace_Gen_Stubs(MethodSpace* ths, TypeDef* ownr, ostream& strm,
                                bool add_typedefs) {
  for(int i=0; i<ths->size; i++) {
    MethodDef* md = ths->FastEl(i);
    if(!MethodSpace_Filter_Method(ths, md))
      continue;

    if(add_typedefs) {
      MethodDef_InitTempArgVars(md, strm, md->fun_argc, add_typedefs);
      MethodDef_GenFunCall(ownr, md, strm, md->fun_argc, add_typedefs);
    }
    else {
      MethodDef_GenStubName(ownr, md, strm);

      /* argd should always be less than argc, but scanner might screw up
         (in fact it does in certain cases..) (if so, then just reset!) */
      if(md->fun_argd > md->fun_argc) md->fun_argd = -1;

      if(md->fun_argd >= 0) {
        int j;
        for(j=md->fun_argd; j<= md->fun_argc; j++) {
          MethodDef_GenFunCall(ownr, md, strm, j);
        }
      }
      else {
        MethodDef_InitTempArgVars(md, strm, md->fun_argc); // declare and init temp arg vars
        MethodDef_GenFunCall(ownr, md, strm, md->fun_argc);
        MethodDef_AssgnTempArgVars(ownr, md, strm, md->fun_argc);
      }

      strm << "  return rval;}\n";
    }
  }
}

void MTA::MethodDef_InitTempArgVars(MethodDef* md, ostream& strm, int act_argc,
                                    bool add_typedefs) {
  bool got_one = false;
  for(int j=0;j < act_argc; j++) {
    TypeDef* argt = md->arg_types[j];
    if(!(argt->IsRef() && !argt->IsConst()))
      continue;
    TypeDef* nrt = argt->GetNonRefType();
    if(nrt == NULL) {
      taMisc::Error("Null NonRefType in GenTempArgVars()", argt->name);
      continue;
    }

    if(nrt->IsNotPtr() && (nrt->IsClass() && !nrt->IsAtomicEff()))
      continue; // css class is implicit ptr anyway

    int args_idx = j + stub_arg_off;

    got_one = true;
    if(!add_typedefs) {
      strm << "    " << nrt->Get_C_Name() << " refarg_" << args_idx << "=";
    }
    MethodDef_GenArgCast(md, nrt, j, strm, add_typedefs);
    if(!add_typedefs) {
      strm << ";";
    }
  }
  if(!add_typedefs && got_one)
    strm << "\n";
}

void MTA::MethodDef_AssgnTempArgVars(TypeDef* ownr, MethodDef* md, ostream& strm,
                                     int act_argc) {
  bool got_one = false;
  int j;
  for(j=0;j < act_argc; j++) {
    TypeDef* argt = md->arg_types[j];
    if(!(argt->IsRef() && !argt->IsConst()))
      continue;

    TypeDef* nrt = argt->GetNonRefType();
    if(nrt == NULL) {
      taMisc::Error("Null NonRefType in GenTempArgVars()", argt->name);
      continue;
    }

    if(nrt->IsNotPtr() && (nrt->IsClass() && !nrt->IsAtomicEff()))
      continue; // css class is implicit ptr anyway

    int args_idx = j + stub_arg_off;

    bool not_mod = true;
    if(nrt->IsNotPtr()) { // harder to do the non-ptr refs
      if(nrt->IsAtomic() || nrt->IsAtomicEff()) {
        strm << "    *arg[" << args_idx << "]=" << MethodDef_GetCSSType(nrt);
        strm << "refarg_" << args_idx << ";";
        not_mod = false;        got_one = true;
      }
    }
    else if(nrt->IsTaBase()) {
      // need to use taBase* to preserve reffing sanity!
      strm << "    *arg[" << args_idx << "]=(taBase" << nrt->GetPtrString() << ")";
      strm << "refarg_" << args_idx << ";";
      not_mod = false;          got_one = true;
    }
    else {                      // all ptrs just done through (void*)..
      strm << "    *arg[" << args_idx << "]=(void" << nrt->GetPtrString() << ")";
      strm << "refarg_" << args_idx << ";";
      not_mod = false;          got_one = true;
    }
    // NOTE: seems to be a conceptual bug, since we should be able to pass
    //  generic pointer to &ref to access the arg by value; therefore, no
    // copying back should be necessary, in fact, that is probably not desirable
    // BUT: the issue here is that css guys do NOT pass by reference -- always by
    // value, so they are not directly compatible -- would need to create a whole
    // set of ref converter functions in cssEl to make this work -- could consider
    if(not_mod && (verbose > 0)) {
      String stargno = String(j);
      String stmbnm = ownr->Get_C_Name() + "::" + md->name + "()";
      taMisc::Error("non-const reference argument value not modified:",
                     md->arg_names[j],"(arg",stargno,") in",stmbnm);
    }
  }
  if(got_one)
    strm << "\n";
}

String MTA::MethodDef_GetCSSType(TypeDef* td) {
  if(td->DerivesFrom(TA_bool))
    return String("(bool)");
  if(td->DerivesFrom(TA_char))
    return String("(Char)");
  if(td->IsVariant() || td->DerivesFrom(TA_int64_t) || td->DerivesFrom(TA_uint64_t))
    return String("(Variant)");
  if(td->IsInt())
    return String("(Int)");
  if(td->IsFloat())
    return String("(Real)");
  if(td->IsString())
    return String("(String)");
  return String("");
}


void MTA::MethodDef_GenArgCast(MethodDef* md, TypeDef* argt, int j, ostream& strm,
                               bool add_typedefs) {
  int args_idx = j + stub_arg_off;

  TypeDef* class_typ = argt->GetActualClassType();

  if(argt->IsRef()) {
    if(!argt->IsConst() && !(argt->IsNotPtr() && (argt->IsClass() &&
                                                  !argt->IsAtomicEff()))) {
      // non-const reference arg..
      if(!add_typedefs) {
        strm << "refarg_" << args_idx;
      }
    }
    else {
      TypeDef* nrt = argt->GetNonRefType();
      if(nrt == NULL) {
        taMisc::Error("Null NonRefType in GenArgCast()", argt->name); return;
      }
      TypeDef* nct = nrt->GetNonConstType();
      if(nct == NULL) {
        taMisc::Error("Null NonConstType in GenArgCast()", nrt->name); return;
      }
      MethodDef_GenArgCast(md, nct, j, strm, add_typedefs); // use the base type for args
      return;
    }
  }
  else if(argt->DerivesFrom(TA_TypeDef)) { // we convert these now
    if(!add_typedefs) {
      strm << "(TypeDef";
      strm << argt->GetPtrString() << ")";
      strm << "*arg[" << args_idx << "]";
    }
  }
  else if(argt->DerivesFrom(TA_MemberDef)) { // we convert these now
    if(!add_typedefs) {
      strm << "(MemberDef";
      strm << argt->GetPtrString() << ")";
      strm << "*arg[" << args_idx << "]";
    }
  }
  else if(argt->DerivesFrom(TA_MethodDef)) { // we convert these now
    if(!add_typedefs) {
      strm << "(MethodDef";
      strm << argt->GetPtrString() << ")";
      strm << "*arg[" << args_idx << "]";
    }
  }
  else if(argt->IsString()) {
    if(!add_typedefs) {
      if(argt->IsNotPtr())
        strm << "arg[" << args_idx << "]->GetStr()";
      else
        strm << "(String" << argt->GetPtrString() << ")*arg[" << args_idx << "]";
    }
  }
  else if(argt->IsVariant()) {
    if(!add_typedefs) {
      if(argt->IsNotPtr())
        strm << "arg[" << args_idx << "]->GetVar()";
      else
        strm << "(Variant" << argt->GetPtrString() << ")*arg[" << args_idx << "]";
    }
  }
  else if(argt->DerivesFromName("ios")) { // cssEl's can cast these directly
    if(!add_typedefs) {
      if(argt->IsNotPtr())
        strm << "*(" << argt->Get_C_Name() << "*)" << "*arg[" << args_idx << "]";
      else
        strm << "(" << argt->Get_C_Name() << ")" << "*arg[" << args_idx << "]";
    }
  }
  else if(argt->IsActualClass()) { // inherits = ptr = 0
    if(add_typedefs) {
      spc_typedef_gen.LinkUniqNameOld(argt);
    }
    else {
      strm << "*(" << argt->Get_C_Name() << "*)arg[" << args_idx << "]"
           << "->GetVoidPtrOfType(&TA_" << argt->name << ")";
    }
  }
  else if((argt->IsPointer()) && (argt->DerivesFrom(TA_char))) {
    if(!add_typedefs) {
      strm << "(" << argt->Get_C_Name() << ")" << "*arg[" << args_idx << "]";
    }
  }
  else if(argt->IsEnum()) {
    if(!add_typedefs) {
      strm << "(" << argt->Get_C_Name() << ")"; // always cast the args
      strm << "(int)*arg[" << args_idx << "]";                // use int conversion
    }
  }
  else if(class_typ) {          // is some kind of class object -- just cast away!
    if(add_typedefs) {
      spc_typedef_gen.LinkUniqNameOld(class_typ);
    }
    else {
      strm << "(" << argt->Get_C_Name() << ")arg[" << args_idx << "]"
           << "->GetVoidPtrOfType(&TA_" << class_typ->name << ")";
    }
  }
  else if(argt->IsAnyPtr()) {
    // this is extremely dangerous, but hopefully will just fail
    if(!add_typedefs) {
      strm << "(" << argt->Get_C_Name() << ")(void";
      strm << argt->GetPtrString() << ")";
      strm << "*arg[" << args_idx << "]";
    }
  }
  else {
    if(!add_typedefs) {
      strm << "(" << argt->Get_C_Name() << ")"; // always cast the args
      strm << "*arg[" << args_idx << "]";
    }
  }
}


void MTA::MethodDef_GenArgs(MethodDef* md, ostream& strm, int act_argc, bool add_typedefs) {
  int j;
  for(j=0;j < act_argc; j++) {
    TypeDef* argt = md->arg_types[j];

    MethodDef_GenArgCast(md, argt, j, strm, add_typedefs);

    if(!add_typedefs) {
      if(j+1 < act_argc)
        strm << ", ";
    }
  }
}

void MTA::MethodDef_GenStubName(TypeDef* ownr, MethodDef* md, ostream& strm) {
  if(ownr->IsFunction())
    strm << "static cssEl* cssElCFun_" << md->name;
  else
    strm << "static cssEl* cssElCFun_" << ownr->name << "_" << md->name;
  strm << "_stub(";
  if(ownr->IsFunction())
    strm << "void*,";
  else
    strm << "void* ths,";
  if(md->fun_argc == 0)
    strm << "int, cssEl**) {\n";
  else if (md->fun_argd >= 0)
    strm << "int na, cssEl** arg) {\n";
  else
    strm << "int, cssEl** arg) {\n";
  strm << "  cssEl* rval=&cssMisc::Void;\n";
}

void MTA::MethodDef_GenStubCall(TypeDef* ownr, MethodDef* md, ostream& strm) {
  if(ownr->IsFunction())
    strm << md->name << "(";
  else
    strm << "((" << ownr->Get_C_Name() << "*)ths)->" << md->name << "(";
}

void MTA::MethodDef_GenFunCall(TypeDef* ownr, MethodDef* md, ostream& strm, int act_argc,
                               bool add_typedefs) {
  String cmd;

  if(md->fun_argd >= 0) {
    if(!add_typedefs) {
      strm << "  if(na == " << act_argc+1 << ") {\n  ";
    }
    MethodDef_InitTempArgVars(md, strm, act_argc, add_typedefs); // declare and init temp arg vars
  }
  if(!add_typedefs) {
    strm << "  ";
  }

  TypeDef* act_typ = md->type->GetActualType();

  bool no_new = false;
  bool has_rval = ((md->type->IsAnyPtr()) || (md->type != &TA_void));
  if (md->type->IsNotPtr()) {
    has_rval = true;
    if (md->type->IsVoid()) { // explicitly not, so ok,
      has_rval = false;
      if(!md->type->name.contains("void")) {
        if(add_typedefs) {
          cerr << "W!!: non-void type is marked void in file: " << cur_fname
               << " method: " << md->name
               << " so I don't know how to handle return type: " << md->type->name
               << " -- it will be ignored!\n";
        }
      }
    }
    else if(md->type->IsVariant() || md->type->DerivesFrom(TA_uint64_t)) {
      cmd = "cssVariant(";
    }
    else if(md->type->DerivesFrom(TA_int64_t)) {
      cmd = "cssInt64(";
    }
    else if (md->type->DerivesFrom(TA_char)) {
      cmd = "cssChar(";
    }
    else if (md->type->IsInt()) {
      cmd = "cssInt((int)";
    }
    else if (md->type->IsBool()) {
      cmd = "cssBool(";
    }
    else if(md->type->IsFloat()) {
      cmd = "cssReal((double)";
    }
    else if (md->type->IsString()) {
      cmd = "cssString(";
    }
    else if (md->type->IsEnum()) {
      cmd = "cssInt((int)";
    }
    else {
      // we don't know how to handle this rval -- not good!!!!
      has_rval = false;
      if(add_typedefs) {
        cerr << "W!!: Warning: in file: " << cur_fname << " method: " << md->name <<
          " don't know how to handle return type: " << md->type->name <<
          " so it will be ignored!\n";
      }
    }
    if(!add_typedefs) {
      if (has_rval)
        strm << "rval=new " << cmd;
    }
    if(!add_typedefs) {
      MethodDef_GenStubCall(ownr, md, strm);
    }
    MethodDef_GenArgs(md, strm, act_argc, add_typedefs);
    if(!add_typedefs) {
      if (has_rval) strm << ")";
      strm << ");";
    }
  }
  else { // IsAnyPtr(), therefore, ptr of some kind
    bool include_td = false;
    // TODO: wrapping unsigned types with a signed wrapper as we do will
    // give wrong behavior when the value is > MAX_xxx (i.e. looks -ve to int type
    // We should have distinct signed/unsigned types
    if(md->type->IsAtomic()) {
      if(md->type->IsInt()) {
        if(md->type->DerivesFrom(TA_int) || md->type->DerivesFrom(TA_unsigned_int))
          cmd = "cssCPtr_int(";
        else if(md->type->DerivesFrom(TA_short) || md->type->DerivesFrom(TA_unsigned_short))
          cmd = "cssCPtr_short(";
        else if(md->type->DerivesFrom(TA_long) || md->type->DerivesFrom(TA_unsigned_long))
          cmd = "cssCPtr_long(";
        //NOTE: slightly wrong conceptually to include the signed/unsigned, since these
        // are really math types, not a char type, but the ptr groks the size, so that is
        // more important, and the cssChar type can grok math, so only output might be affected
        else if(md->type->DerivesFrom(TA_char) || md->type->DerivesFrom(TA_signed_char) ||
                md->type->DerivesFrom(TA_unsigned_char))
          cmd = "cssCPtr_char(";
        //TODO: wrapping uint64 in int64 will give incorrect results for large vals!
        else if(md->type->DerivesFrom(TA_int64_t) || md->type->DerivesFrom(TA_uint64_t))
          cmd = "cssCPtr_long_long(";
      }
      else if(md->type->IsEnum()) {
        cmd = "cssCPtr_int(";
      }
      else if(md->type->IsFloat()) {
        if(md->type->DerivesFrom(TA_double))
          cmd = "cssCPtr_double(";
        else if(md->type->DerivesFrom(TA_float))
          cmd = "cssCPtr_float(";
      }
      else if(md->type->IsBool())
        cmd = "cssCPtr_bool(";
    }
    else if(md->type->IsAtomicEff()) {
      if(md->type->DerivesFrom(TA_taString))
        cmd = "cssCPtr_String(";
      else if(md->type->DerivesFrom(TA_Variant))
        cmd = "cssCPtr_Variant(";
    }
    else if(md->type->IsTaBase()) {
      // NOTE: many times the taBase status of a type is NOT known because it is
      // simply a "class TypeName;" declare with no inheritance information at all
      // therefore, Inherits doesn't work, and so we rely on names..
      include_td = true;
      if(act_typ->name.endsWith("_Matrix")) { // see above note
        cmd = "cssTA_Matrix(";
      }
      else {
        cmd = "cssTA_Base(";
      }
    }
    else if(md->type->DerivesFrom(TA_TypeDef) || act_typ->name == "TypeDef") {
      cmd = "cssTypeDef(";
      include_td = true;
    }
    else if(md->type->DerivesFrom(TA_MethodDef) || act_typ->name == "MethodDef") {
      cmd = "cssMethodDef(";
      include_td = true;
    }
    else if(md->type->DerivesFrom(TA_MemberDef) || act_typ->name == "MemberDef") {
      cmd = "cssMemberDef(";
      include_td = true;
    }
    else {
      // this is the generic fallback for all guys -- dynamic runtime lookup of type!
      cmd = "cssTA::MakeTA(";
      no_new = true;
      include_td = true;
    }
    // everything is cast into a void*!
    if(!add_typedefs) {
      if(no_new) {
        strm << "rval=" << cmd << "(void*)";
      }
      else {
        strm << "rval=new " << cmd << "(void*)";
      }
      MethodDef_GenStubCall(ownr, md, strm);
    }
    MethodDef_GenArgs(md, strm, act_argc, add_typedefs);
    TypeDef* nptd = md->type->GetNonPtrType();
    if(add_typedefs) {
      if(include_td) {
        spc_typedef_gen.LinkUniqNameOld(nptd);
      }
    }
    else {
      strm << ")";
      strm << ", " << (int)(md->type->IsPointer());
      if(include_td) {
        strm << ", " << TypeDef_Gen_TypeDef_Ptr(nptd);
      }
      strm << ");";
    }
  }

  if(!add_typedefs) {
    if(md->fun_argd >= 0) {
      MethodDef_AssgnTempArgVars(ownr, md, strm, act_argc);
      strm << "}\n";
    }
    else
      strm << "\n";
  }
}

void MTA::MemberSpace_Gen_PropStubs(MemberSpace* ths, TypeDef* ownr, ostream& strm) {
  for (int i=0; i<ths->size; i++) {
    MemberDef* md = ths->FastEl(i);
    bool is_new = false;
    // getter stub
    String prop_name = md->OptionAfter("GET_");
    if (prop_name.nonempty()) {
      ownr->properties.AssertProperty(prop_name, is_new, true, md);
      strm << "  Variant ta_" << ownr->name << "_" << prop_name
        << "_get(const void* inst){return Variant(((const "
        << ownr->GetNonPtrType()->Get_C_Name()
        << "*)inst)->" << md->name << ");}\n";
    }
    // setter stub
    prop_name = md->OptionAfter("SET_");
    if (prop_name.nonempty()) {
      // make conversion for the setter param
      ownr->properties.AssertProperty(prop_name, is_new, false, md);
      TypeDef* param_td = md->type;
      if (param_td) { // better exist!
        String conv = VariantToTargetConversion(param_td);
        strm << "  void ta_" << ownr->name << "_" << prop_name
          << "_set(void* inst, const Variant& val) {(("
          << ownr->GetNonPtrType()->Get_C_Name() << "*)inst)->" << md->name
          << " = " << conv << ";}\n";
      }
    }
  }
}

void MTA::MethodSpace_Gen_PropStubs(MethodSpace* ths, TypeDef* ownr, ostream& strm) {
  for (int i=0; i<ths->size; i++) {
    MethodDef* md = ths->FastEl(i);
    bool is_new = false;
    // getter stub
    String prop_name = md->OptionAfter("GET_");
    if (prop_name.nonempty()) {
      ownr->properties.AssertProperty(prop_name, is_new, true, md);
      strm << "  Variant ta_" << ownr->name << "_" << prop_name
        << "_get(const void* inst){return Variant(((const "
        << ownr->GetNonPtrType()->Get_C_Name()
        << "*)inst)->" << md->name << "());}\n";
    }
   // setter stub
    prop_name = md->OptionAfter("SET_");
    if (prop_name.nonempty()) {
      // make conversion for the setter param
      TypeDef* param_td = md->arg_types.SafeEl(0);
      if (param_td) { // better exist!
        ownr->properties.AssertProperty(prop_name, is_new, false, md);
        String conv = VariantToTargetConversion(param_td);
        strm << "  void ta_" << ownr->name << "_" << prop_name
          << "_set(void* inst, const Variant& val) {(("
          << ownr->GetNonPtrType()->Get_C_Name() << "*)inst)->" << md->name
          << "(" << conv << ");}\n";
      } else {
      cerr << "**ERROR " << md->name << "SET method must have at least one arg!\n";
      }
    }
  }
}


///////////////////////////////////////////
//   Data generation: Enum, Member, Method, Property

void MTA::TypeSpace_Gen_Data(TypeSpace* ths, ostream& strm) {
  strm << "\n// Type Data\n\n";
  for(int i=0; i<ths->size; i++) {
    TypeDef_Gen_Data(ths->FastEl(i), strm);
  }
}

void MTA::TypeDef_Gen_Data(TypeDef* ths, ostream& strm) {
  if(!TypeDef_Gen_Test(ths)) return;
  if(ths->IsTemplate()) return;

  if(ths->IsEnum()) {
    TypeDef_Gen_EnumData(ths, strm);
  }
  if(ths->IsActualClass() && !ths->HasOption("NO_MEMBERS")) {
    TypeDef_Gen_EnumData(ths, strm);
    TypeDef_Gen_MemberData(ths, strm);
    TypeDef_Gen_MethodData(ths, strm);
    TypeDef_Gen_PropertyData(ths, strm);
  }
  if(ths->IsFunction() && (gen_css) && !ths->HasOption("NO_CSS")) {
    TypeDef_Gen_MethodData(ths, strm);
  }
}


//////////////////////////////////
//           Enum Data

void MTA::TypeDef_Gen_EnumData(TypeDef* ths, ostream& strm) {
  if(ths->IsActualClass()) {
    for(int i=0; i<ths->sub_types.size; i++) {
      TypeDef* enm = ths->sub_types.FastEl(i);
      if(!(enm->IsEnum()) || (enm->owner != &(ths->sub_types))
         || (enm->enum_vals.size == 0))
        continue;

      strm << "\nstatic EnumDef_data TA_" << ths->name << "_" << enm->name
           << "[]={\n";
      EnumSpace_Gen_Data(&(enm->enum_vals), strm);
    }
  }
  else if(ths->IsEnum()) {
    strm << "\nstatic EnumDef_data TA_" << ths->name << "_EnumDef[]={\n";
    EnumSpace_Gen_Data(&(ths->enum_vals), strm);
  }
}

void MTA::EnumSpace_Gen_Data(EnumSpace* ths, ostream& strm) {
  for(int j=0; j<ths->size; j++) {
    EnumDef* enm = ths->FastEl(j);
    if (enm->HasOption("IGNORE")) continue;
    String str_opts = taMisc::StrArrayToChar(enm->opts);
    strm << "  {\"" << enm->name << "\",\"" << enm->desc << "\",\""
         << str_opts << "\"," << enm->enum_no << "},\n";
  }
  strm << "  {NULL}};\n";
}


void MTA::TypeDef_Init_EnumData(TypeDef* ths, ostream& strm) {
  if(ths->IsActualClass()) {
    for(int i=0; i<ths->sub_types.size; i++) {
      TypeDef* enm = ths->sub_types.FastEl(i);
      if(!(enm->IsEnum()) || (enm->owner != &(ths->sub_types))
         || (enm->enum_vals.size == 0))
        continue;

      String str_opts = taMisc::StrArrayToChar(enm->opts);
      String str_inh_opts = taMisc::StrArrayToChar(enm->inh_opts);
      String str_lists = taMisc::StrArrayToChar(enm->lists);

      strm << "  tac_AddEnum(TA_" << ths->name << ", \"" << enm->name << "\", \""
           << enm->desc  << "\", \"" << str_opts << "\", \"" << str_inh_opts
           << "\", \"" << str_lists << "\", ";
      strm << "\"" << enm->source_file << "\", " << String(enm->source_start)
	   << ", " << String(enm->source_end) << ", ";
      strm << "TA_" << ths->name << "_" << enm->name << ");\n";
    }
  }
  else if(ths->IsEnum()) {
    strm << "  tac_ThisEnum(TA_" << ths->name << ", ";
    strm << "TA_" << ths->name << "_EnumDef);\n";
  }
}


//////////////////////////////////
//         Member Data


void MTA::TypeDef_Gen_MemberData(TypeDef* ths, ostream& strm) {
  int cnt = 0;
  for(int i=0; i<ths->members.size; i++) {
    if(MemberSpace_Filter_Member(&(ths->members), ths->members.FastEl(i))) {
      cnt++;
      break;
    }
  }
  if(cnt <= 0) return;

  MemberSpace_Gen_Data(&(ths->members), ths, strm);
}

bool MTA::MemberSpace_Filter_Member(MemberSpace* ths, MemberDef* md) {
  if((md->owner == ths) && !md->HasOption("IGNORE"))
    return true;
  return false;
}

void MTA::MemberSpace_Gen_Data(MemberSpace* ths, TypeDef* ownr, ostream& strm) {
  String mbr_off_nm;

  int n_non_statics = 0;
  for(int i=0; i<ths->size; i++) {
    MemberDef* md = ths->FastEl(i);
    if(!MemberSpace_Filter_Member(ths, md))
      continue;
    if(!md->is_static)
      n_non_statics++;
  }

  if (n_non_statics > 0) {
    mbr_off_nm = String("TA_") + ownr->name + "_MbrOff";
    strm << "static int " << ownr->Get_C_Name() << "::* " << mbr_off_nm << ";\n";
  }

  strm << "\nstatic MemberDef_data TA_" << ownr->name << "_MemberDef[]={\n";

  for(int i=0; i<ths->size; i++) {
    MemberDef* md = ths->FastEl(i);
    if(!MemberSpace_Filter_Member(ths, md))
      continue;

    String str_opts = taMisc::StrArrayToChar(md->opts);
    String str_lists = taMisc::StrArrayToChar(md->lists);

    String tpfld = TypeDef_Gen_TypeName(md->type);
    strm << "  {" << tpfld << ",\"" << md->name << "\",\"" << md->desc << "\",\""
         << str_opts << "\",\"" << str_lists << "\",\n";

    if (md->is_static) {
      strm << "  (ta_memb_ptr)NULL,1,";
      strm << "(void*)(&" << ownr->Get_C_Name() << "::" << md->name << ")";
    }
    else {
      strm << "  *((ta_memb_ptr*)&(" << mbr_off_nm
            << "=(int " << ownr->Get_C_Name() << "::*)(&"
            << ownr->Get_C_Name() << "::" << md->name << ")))";
      strm << ",0,NULL";
    }
    if(md->fun_ptr)     strm << ",1";
    else                strm << ",0";
    strm << "},\n";
  }
  strm << "  {NULL}};\n";
}

void MTA::TypeDef_Init_MemberData(TypeDef* ths, ostream& strm) {
  int cnt = 0;
  for(int i=0; i<ths->members.size; i++) {
    if(MemberSpace_Filter_Member(&(ths->members), ths->members.FastEl(i))) {
      cnt++;
      break;
    }
  }
  if(cnt == 0) return;

  strm << "  tac_AddMembers(TA_" << ths->name << ","
       << "TA_" << ths->name << "_MemberDef);\n";
}



//////////////////////////////////
//         Method Data


void MTA::TypeDef_Gen_MethodData(TypeDef* ths, ostream& strm) {
  int cnt = 0;
  for(int i=0; i<ths->methods.size; i++) {
    if(MethodSpace_Filter_Method(&(ths->methods), ths->methods.FastEl(i))) {
      cnt++;
      break;
    }
  }
  if(cnt <= 0) return;

  MethodSpace_Gen_ArgData(&(ths->methods), ths, strm);
  MethodSpace_Gen_Data(&(ths->methods), ths, strm);
}

bool MTA::MethodSpace_Filter_Method(MethodSpace* ths, MethodDef* md) {
  // always ignore these
  if((md->name == "Copy_") || md->HasOption("IGNORE")
     || ths->owner->IgnoreMeth(md->name))
    return false;

  if((md->owner != ths) && !ths->owner->HasOption("MULT_INHERIT"))
    return false;               // don't reproduce owners functions (except multi inh)

  // is_static is a problem in mult_inherit cases for compiler
  // what is the problem exactly??
  if(ths->owner->HasOption("MULT_INHERIT")) {
    if(!md->is_static)
      return true;
    // return false;
    return true;
  }

  return true;
}

void MTA::MethodSpace_Gen_ArgData(MethodSpace* ths, TypeDef* ownr, ostream& strm) {
  for(int i=0; i<ths->size; i++) {
    MethodDef* md = ths->FastEl(i);
    if(!MethodSpace_Filter_Method(ths, md))
      continue;

    if(md->fun_argc > 0)
      MethodDef_Gen_ArgData(md, ownr, strm);
  }
}

void MTA::MethodDef_Gen_ArgData(MethodDef* ths, TypeDef* ownr, ostream& strm) {
  strm << "\nstatic MethodArgs_data TA_" << ownr->name << "_" << ths->name
       << "_MethArgs[]={\n";

  for(int i=0; i<ths->arg_types.size; i++) {
    String tpfld = TypeDef_Gen_TypeName(ths->arg_types[i]);
    strm << "  {" << tpfld << ",\"" << ths->arg_names[i] << "\",\""
         << ths->arg_defs[i] << "\"},\n";
  }
  strm << "  {NULL}};\n";
}


void MTA::MethodSpace_Gen_Data(MethodSpace* ths, TypeDef* ownr, ostream& strm) {
  strm << "\nstatic MethodDef_data TA_" << ownr->name << "_MethodDef[]={\n";

  for(int i=0; i<ths->size; i++) {
    MethodDef* md = ths->FastEl(i);
    if(!MethodSpace_Filter_Method(ths, md))
      continue;

    String str_opts = taMisc::StrArrayToChar(md->opts);
    String str_lists = taMisc::StrArrayToChar(md->lists);

    String tpfld = TypeDef_Gen_TypeName(md->type);
    strm << "  {" << tpfld << ",\"" << md->name << "\",\"" << md->desc << "\",\""
         << str_opts << "\",\"" << str_lists << "\",\n    ";

    strm << md->fun_overld << "," << md->fun_argc
         << "," << md->fun_argd;

    if (md->is_virtual) strm << ",1"; else strm << ",0";
    if(md->is_static) {                         // only static gets addr
      strm << ",1,(ta_void_fun)(";
      if(ownr->IsFunction())
        strm <<  md->name << ")";
      else
        strm << ownr->Get_C_Name() << "::" << md->name << ")";
    }
    else
      strm << ",0,NULL";

    if(gen_css && !ownr->HasOption("NO_CSS") && !ownr->IsTemplate()) {
      strm << ",cssElCFun_";
      if(ownr->IsFunction())
        strm << md->name << "_stub";
      else
        strm << ownr->name << "_" << md->name << "_stub";
    }
    else {
      strm << ",NULL";
    }

    if(md->fun_argc > 0)
      strm << ",TA_" << ownr->name << "_" << md->name << "_MethArgs";
    else
      strm << ",NULL";

    strm << "},\n";
  }
  strm << "  {NULL}};\n";
}


void MTA::TypeDef_Init_MethodData(TypeDef* ths, ostream& strm) {
  int cnt = 0;
  for(int i=0; i<ths->methods.size; i++) {
    if(MethodSpace_Filter_Method(&(ths->methods), ths->methods.FastEl(i))) {
      cnt++;
      break;
    }
  }
  if(cnt <= 0) return;

  strm << "  tac_AddMethods(TA_" << ths->name << ","
       << "TA_" << ths->name << "_MethodDef);\n";
}


//////////////////////////////////
//        Property Data

/*NOTE: in maketa we only ever use the .properties to hold PropertyDef objs
*/
void MTA::TypeDef_Gen_PropertyData(TypeDef* ths, ostream& strm) {
  int cnt = 0;
  for(int i=0; i<ths->properties.size; i++) {
    if(PropertySpace_Filter_Property(&(ths->properties),
      dynamic_cast<PropertyDef*>(ths->properties.FastEl(i)))) {
      cnt++;
      break;
    }
  }
  if(cnt <= 0) return;

  PropertySpace_Gen_Data(&(ths->properties), ths, strm);
}

bool MTA::PropertySpace_Filter_Property(PropertySpace* ths, PropertyDef* md) {
  if (!md) return false;
  if((md->owner == ths) && !md->HasOption("IGNORE"))
    return true;
  return false;
}

void MTA::PropertySpace_Gen_Data(PropertySpace* ths, TypeDef* ownr, ostream& strm) {
  String mbr_off_nm;

  int n_non_statics = 0;
  for(int i=0; i<ths->size; i++) {
    PropertyDef* md = dynamic_cast<PropertyDef*>(ths->FastEl(i));
    if(!PropertySpace_Filter_Property(ths, md))
      continue;
    if(!md->is_static)
      n_non_statics++;
  }

  strm << "\nstatic PropertyDef_data TA_" << ownr->name << "_PropertyDef[]={\n";

  for(int i=0; i<ths->size; i++) {
    PropertyDef* md = dynamic_cast<PropertyDef*>(ths->FastEl(i));
    if(!PropertySpace_Filter_Property(ths, md))
      continue;

    String str_opts = taMisc::StrArrayToChar(md->opts);
    String str_lists = taMisc::StrArrayToChar(md->lists);

    String tpfld = TypeDef_Gen_TypeName(md->type);
    strm << "  {" << tpfld << ",\"" << md->name << "\",\"" << md->desc << "\",\""
         << str_opts << "\",\"" << str_lists << "\",\n";

    if (md->is_static) {
      strm << "1,";
    } else {
      strm << "0,";
    }
    String prop_stub;
    if (md->get_mth || md->get_mbr) {
      prop_stub = "ta_" + ownr->name + "_" + md->name + "_get";
      strm << prop_stub << ",";
    } else {
      strm << "NULL,";
    }
    if (md->set_mth || md->set_mbr) {
      prop_stub = "ta_" + ownr->name + "_" + md->name + "_set";
      strm << prop_stub;
    } else {
      strm << "NULL";
    }
    strm << "},\n";
  }
  strm << "  {NULL}};\n";
}

void MTA::TypeDef_Init_PropertyData(TypeDef* ths, ostream& strm) {
  int cnt = 0;
  for(int i=0; i<ths->properties.size; i++) {
    if(PropertySpace_Filter_Property(&(ths->properties),
      dynamic_cast<PropertyDef*>(ths->properties.FastEl(i)))) {
      cnt++;
      break;
    }
  }
  if(cnt == 0) return;

  strm << "  tac_AddProperties(TA_" << ths->name << ","
       << "TA_" << ths->name << "_PropertyDef);\n";
}


//////////////////////////////////
//       TypeInit Function

void MTA::TypeSpace_Gen_TypeInit(TypeSpace* ths, ostream& strm) {
  strm << "\n// TypeDef Init Function\n";

#ifdef TA_OS_WIN
  if (win_dll)
    strm << win_dll_str << " ";
#endif
  strm << "void ta_TypeInit_" << trg_basename << "() {\n";
  strm << "  TypeDef* sbt = NULL;\n\n";

  for(int i=0; i<ths->size; i++) {
    TypeDef_Gen_TypeInit(ths->FastEl(i), strm);
  }
  strm << "} \n";
}

void MTA::TypeDef_Gen_TypeInit(TypeDef* ths, ostream& strm) {
  if(!TypeDef_Gen_Test(ths)) return;

  // get all the type-level info here in first pass -- doesn't depend on anything else
  // but other data later (members, methods) can depend on this type info..

  bool reg_fun = false;
  if(ths->IsFunction() && (gen_css) && !ths->HasOption("NO_CSS")) {
    reg_fun = true;
  }

  strm << "\n  TA_" << ths->name << ".AddNewGlobalType(";
  if(reg_fun) {
    strm << "false";            // don't create all the derivatives on this guy!
  }
  strm << ");\n";
  if(ths->IsEnum()) {
    TypeDef_Init_EnumData(ths, strm);
  }
  if(ths->IsActualClass() && !ths->HasOption("NO_MEMBERS") && !ths->IsTemplate()) {
    TypeDef_Init_EnumData(ths, strm);
    SubTypeSpace_Gen_Init(&(ths->sub_types), ths, strm);
  }
  if(reg_fun) {
    strm << "  tac_AddRegFun(TA_" + ths->name + ");\n";
  }
}


//////////////////////////////////
// 	  DataInit Function

void MTA::TypeSpace_Gen_DataInit(TypeSpace* ths, ostream& strm) {
  strm << "\n// Data Init Function\n";

#ifdef TA_OS_WIN
  if (win_dll)
    strm << win_dll_str << " ";
#endif
  strm << "void ta_DataInit_" << trg_basename << "() {\n";

  for(int i=0; i<ths->size; i++) {
    TypeDef_Gen_DataInit(ths->FastEl(i), strm);
  }
  strm << "} \n\n";
}

void MTA::TypeDef_Gen_DataInit(TypeDef* ths, ostream& strm) {
  if(!TypeDef_Gen_Test(ths)) return;

  strm << "\n";                 // new row for new class

  String ths_ref = "TA_" + ths->name + ".";
  TypeDef_Gen_AddParents(ths, ths_ref, strm);

  if(ths->IsActualClass() && !ths->HasOption("NO_MEMBERS") && !ths->IsTemplate()) {
    TypeDef_Init_MemberData(ths, strm);
    TypeDef_Init_MethodData(ths, strm);
    TypeDef_Init_PropertyData(ths, strm);
  }
  if(ths->IsFunction() && (gen_css) && !ths->HasOption("NO_CSS")) {
    TypeDef_Init_MethodData(ths, strm);
  }
}

void MTA::TypeDef_Gen_AddParents(TypeDef* ths, char* typ_ref, ostream& strm) {
  TypeDef_Gen_AddOtherParents(ths, typ_ref, strm);

  if(ths->parents.size == 0)
    return;

  int cnt=0;
  // see if there are any parents, and also check for too many for us to handle!
  for (int i=0; i < ths->parents.size; i++) {
    TypeDef* ptd = ths->parents.FastEl(i);
    cnt++;
  }
  if (cnt == 0)
    return;
  if (cnt > PAR_ARG_COUNT) {
    taMisc::Error("AddParents(): parents.size > ", String(PAR_ARG_COUNT),
      ", increase number of args to AddParents()", "type name:", ths->name);
    return;
  }

  strm << "  " << typ_ref;
  strm << "AddParentNames(";
  String ths_cnm = ths->Get_C_Name();
  for (int i=0; i < ths->parents.size; ++i) {
    TypeDef* ptd = ths->parents.FastEl(i);
    if (i > 0)
      strm << ", ";
    if(ptd->IsSubType())
      strm << "\"" << ptd->owner->owner->name << "::" << ptd->name << "\"";
    else
      strm << "\"" << ptd->name << "\"";
  }
  strm << ");\n";
}

// this assumes never more than PAR_ARG_COUNT of either

void MTA::TypeDef_Gen_AddOtherParents(TypeDef* ths, char* typ_ref, ostream& strm) {
  // and add template parameters too!
  if(ths->IsTemplInst() && !ths->HasOption("NO_CSS")) {
    if(ths->templ_pars.size > 0) {
      strm << "  " << typ_ref << "AddTemplParNames(";
      for(int i=0; i < ths->templ_pars.size; i++) {
	TypeDef* ptd = ths->templ_pars.FastEl(i);
	if(ptd->HasOption("NO_CSS")) continue;
	strm << "\"" << ptd->name << "\"";
	if(i < ths->templ_pars.size-1)
	  strm << ", ";
      }
      strm << ");\n";
    }
  }
}

// this one is for subtypes
void MTA::TypeDef_Gen_AddAllParents(TypeDef* ths, char* typ_ref, ostream& strm) {
  TypeDef_Gen_AddOtherParents(ths, typ_ref, strm);
  if(ths->parents.size == 0)
    return;

  strm << "  " << typ_ref << "AddParentNames(";
  for(int i=0; i < MIN(ths->parents.size, PAR_ARG_COUNT); i++) {
    TypeDef* ptd = ths->parents.FastEl(i);
    if(ptd->IsSubType())
      strm << "\"" << ptd->owner->owner->name << "::" << ptd->name << "\"";
    else
      strm << "\"" << ptd->name << "\"";
    if(i < ths->parents.size-1)
      strm << ", ";
  }
  strm << ");\n";

  if(ths->parents.size > PAR_ARG_COUNT) {
    taMisc::Error("AddParents(): parents.size > 6, increase number of args to AddParents()",
                   "type name:", ths->name);
  }
}

void MTA::SubTypeSpace_Gen_Init(TypeSpace* ths, TypeDef* ownr, ostream& strm) {
  int i;
  for(i=0; i<ths->size; i++) {
    TypeDef* sbt = ths->FastEl(i);
    if((sbt->owner != ths) || sbt->IsEnum() || sbt->IsNotActual())
      continue;

    String str_opts = taMisc::StrArrayToChar(sbt->opts);
    String str_inh_opts = taMisc::StrArrayToChar(sbt->inh_opts);
    String str_lists = taMisc::StrArrayToChar(sbt->lists);

    strm << "  sbt = new TypeDef(\"" << sbt->name << "\", \"" << sbt->desc << "\", ";
    strm << "\n\t\"" << str_inh_opts << "\", \"" << str_opts << "\", \"";
    strm << str_lists << "\", ";
    strm << "\"" << sbt->source_file << "\", " << String(sbt->source_start)
         << ", " << String(sbt->source_end) << ", ";
    strm << sbt->GetTypeEnumString() << ", ";
    strm << "sizeof(int), (void**)0);\n";

    String sbt_ref = "sbt->";
    TypeDef_Gen_AddAllParents(sbt, sbt_ref, strm);

    strm << "  TA_" << ownr->name << ".sub_types.Add(sbt);\n";
  }
}

//////////////////////////////////
//       InstInit Function

void MTA::TypeSpace_Gen_InstInit(TypeSpace* ths, ostream& strm) {
  strm << "\n// Instance Init Function\n";

#ifdef TA_OS_WIN
  if (win_dll)
    strm << win_dll_str << " ";
#endif
  strm << "void ta_InstInit_" << trg_basename << "() {\n";

  for(int i=0; i<ths->size; i++) {
    TypeDef_Gen_InstInit(ths->FastEl(i), strm);
  }
  strm << "} \n";
  strm << "// Register Init Functions\n\n";
  strm << "TypeDefInitRegistrar ta_tdreg_" << trg_basename
       << "(ta_TypeInit_" << trg_basename << ", "
       << "ta_DataInit_" << trg_basename << ", "
       << "ta_InstInit_" << trg_basename << ");\n\n";
}

void MTA::TypeDef_Gen_InstInit(TypeDef* ths, ostream& strm) {
  if(!TypeDef_Gen_Test(ths)) return;

  if(ths->IsActualClass()) {
    if((gen_instances || (ths->HasOption("INSTANCE")))
       && !ths->HasOption("NO_INSTANCE") && !ths->IsTemplate()) {
      strm << "  TAI_" << ths->name << " = new "<< ths->Get_C_Name() << ";\n";
      if(ths->parents.size>1) {
        strm << "  " << "TA_" << ths->name << ".SetParOffsets(";
        for (int i=0; i < ths->parents.size; ++i) {
          TypeDef* ptd = ths->parents.FastEl(i);
          if (i > 0)
            strm << ", ";
          if(!ptd->IsTemplate()) {
            strm << "(int)((unsigned long)((" << ptd->Get_C_Name() << "*)"
                 << "TAI_" << ths->name << ")-(unsigned long)TAI_" << ths->name
                 << ")";
          }
          else {
            strm << "0";
            taMisc::Error("warning: type:",ths->name,"has mult inherit but one parent is a template (shouldn't happen)!");
          }
        }
        strm << ");\n";
      }
    }
    else {
      if((ths->parents.size > 1) && !ths->InheritsFromName("ios") &&
         !ths->HasOption("NO_MEMBERS")) {
        taMisc::Error("warning: type:",ths->name,"has mult inherit but no instance",
                      "-parent offset cannot be computed!");
      }
    }
  }
}

