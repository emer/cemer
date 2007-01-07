// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ta_program.h"

#include "css_machine.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "css_ta.h"

#include <QCoreApplication>
#include <QDir>

#ifdef TA_GUI
# include "ilineedit.h" // for iTextDialog
# include "ta_qt.h"
# include "ta_qtdialog.h"

# include <QMessageBox>
#endif


//////////////////////////
//   ProgVar		//
//////////////////////////

void ProgVar::Initialize() {
  var_type = T_Int;
  int_val = 0;
  real_val = 0.0;
  bool_val = false;
  object_type = &TA_taOBase;
  hard_enum_type = NULL;
  objs_ptr = false;
}

void ProgVar::Destroy() {
  CutLinks();
}

void ProgVar::InitLinks() {
  taBase::Own(object_val, this);
  taBase::Own(dyn_enum_val, this);
  inherited::InitLinks();
}

void ProgVar::CutLinks() {
  object_val.CutLinks();
  dyn_enum_val.Reset();
  dyn_enum_val.CutLinks();
  inherited::CutLinks();
}

void ProgVar::Copy_(const ProgVar& cp) {
  var_type = cp.var_type;
  int_val = cp.int_val;
  real_val = cp.real_val;
  string_val = cp.string_val;
  bool_val = cp.bool_val;
  object_type = cp.object_type;
  object_val = cp.object_val;
  hard_enum_type = cp.hard_enum_type;
  dyn_enum_val = cp.dyn_enum_val;
  objs_ptr = cp.objs_ptr;
  desc = cp.desc;
}

void ProgVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  name = taMisc::StringCVar(name); // make names C legal names
}

void ProgVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if (prg) prognm = prg->name;
  if((var_type == T_Object) && (!object_val)) {
    if(!quiet) taMisc::CheckError("Error in ProgVar in program:", prognm, "var name:",name,
			     "object pointer is NULL");
    rval = false;
  }
}

void ProgVar::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  //  todo: make this conditional on some "obsessive" setting either in program
  // or in overall settings.  too many reduntant checks on the same object
  // triggered by this!!
//   if((var_type == T_Object) && (object_val)) {
//     object_val->CheckConfig(quiet, rval);
//   }
}

TypeDef* ProgVar::act_object_type() const {
  TypeDef* rval = &TA_taBase; // the min return value
  if (object_type)
    if (object_val) rval = object_val->GetTypeDef();
    else rval = object_type;
  return rval;
}

void ProgVar::SetInt(int val) {
  var_type = T_Int;
  int_val = val;
}

void ProgVar::SetReal(double val) {
  var_type = T_Real;
  real_val = val;
}

void ProgVar::SetString(const String& val) {
  var_type = T_String;
  string_val = val;
}

void ProgVar::SetBool(bool val) {
  var_type = T_Bool;
  bool_val = val;
}

void ProgVar::SetObject(taBase* val) {
  var_type = T_Object;
  object_val = val;
  if(object_val) object_type = object_val->GetTypeDef();
}

void ProgVar::SetHardEnum(TypeDef* enum_type, int val) {
  var_type = T_HardEnum;
  int_val = val;
  hard_enum_type = enum_type;
}

void ProgVar::SetDynEnum(int val) {
  var_type = T_DynEnum;
  dyn_enum_val.SetNumVal(val);
}

void ProgVar::SetDynEnumName(const String& val) {
  var_type = T_DynEnum;
  dyn_enum_val.SetNameVal(val);
}


void ProgVar::Cleanup() {
  if (!((var_type == T_Int) || (var_type == T_HardEnum)))
    int_val = 0;
  if (var_type != T_Real)  real_val = 0.0;
  if (var_type != T_String)  string_val = _nilString;
  if (var_type != T_Bool)  bool_val = false;
  if (var_type != T_Object) {
    //note: its ok to leave whatever type is there
    object_val.CutLinks();
  }
  if (var_type != T_HardEnum) {
    hard_enum_type = NULL;
  }
  //TODO: anything about DynEnums???
}

void ProgVar::DataChanged(int dcr, void* op1, void* op2) {
  // dynenum is programmed to send us notifies, we trap those and 
  // turn them into changes of us, to force gui to update (esp enum list)
  if ((dcr == DCR_CHILD_ITEM_UPDATED) && (op1 == &dyn_enum_val)) {
    DataChanged(DCR_ITEM_UPDATED);
    return; // don't send and further
  }
  inherited::DataChanged(dcr, op1, op2);
}

String ProgVar::GetDisplayName() const {
  if(var_type == T_Int)
    return "int " + name + " = " + String(int_val);
  else if(var_type == T_Real)
    return "real " + name + " = " + String(real_val);
  else if(var_type == T_String)
    return "String " + name + " = " + string_val;
  else if(var_type == T_Bool)
    return "bool " + name + " = " + ((bool_val ? "true" : "false"));
  else if(var_type == T_Object) {
    if(!object_type) return "NULL object type";
    return object_type->name + " " + name + " = " + ((object_val ? object_val->GetDisplayName() : "NULL"));
  }
  else if(var_type == T_HardEnum) {
    if(!hard_enum_type) return "NULL hard enum type";
    return hard_enum_type->name + " " + name + " = " + 
      hard_enum_type->Get_C_EnumString(int_val);
  }
  else if(var_type == T_DynEnum) {
    return "dyn enum " + name + " = " + 
      dyn_enum_val.NameVal();
  }
  return "invalid type!";
}

taBase::DumpQueryResult ProgVar::Dump_QuerySaveMember(MemberDef* md) {
  DumpQueryResult rval = DQR_SAVE; // only used for membs we match below
  if (md->name == "int_val")
    rval = ((var_type == T_Int) || (var_type == T_HardEnum)) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "real_val")
    rval = (var_type == T_Real) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "string_val")
    rval = (var_type == T_String) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "bool_val")
    rval = (var_type == T_Bool) ? DQR_SAVE : DQR_NO_SAVE;
  else if ((md->name == "object_type") || (md->name == "object_val"))
    rval = (var_type == T_Object) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "hard_enum_type")
    rval = (var_type == T_HardEnum) ? DQR_SAVE : DQR_NO_SAVE;
  else if (md->name == "dyn_enum_val")
    rval = (var_type == T_DynEnum) ? DQR_SAVE : DQR_NO_SAVE;
  else 
    return inherited::Dump_QuerySaveMember(md);
  return rval;
}

const String ProgVar::GenCss(bool is_arg) {
  return is_arg ? GenCssArg_impl() : GenCssVar_impl() ;
} 

const String ProgVar::GenCssType() const {
  switch(var_type) {
  case T_Int:
    return "int";
  case T_Real:
    return "double";
  case T_String:
    return "String";
  case T_Bool:
    return "bool";
  case T_Object:
    if(object_val)
      return object_val->GetTypeDef()->name + "*";
    else
      return object_type->name + "*";
  case T_HardEnum:
    if(hard_enum_type)
      return hard_enum_type->name;
    else
      return "int";
  case T_DynEnum:
    return "c_DynEnum";
  }
  return "";
}

const String ProgVar::GenCssInitVal() const {
  switch(var_type) {
  case T_Int:
    return int_val;
  case T_Real:
    return real_val;
  case T_String:
    return string_val;
  case T_Bool:
    return bool_val;
  case T_Object:
    if(object_val)
      return object_val->GetPath();
    else
      return "NULL";
  case T_HardEnum:
    if(hard_enum_type)
      return hard_enum_type->GetValStr(&int_val);
    else
      return int_val;
  case T_DynEnum:
    return dyn_enum_val.NameVal();
  }
  return "";
}

// note: *never* initialize variables because they are cptrs to actual current
// value in object..
const String ProgVar::GenCssArg_impl() {
  String rval;
  rval += GenCssType() + " ";
  rval += name;
  return rval;
}

const String ProgVar::GenCssVar_impl() {
  String rval;
  rval += GenCssType() + " ";
  rval += name;
  rval += ";\n";
  return rval;
}

cssEl* ProgVar::NewCssEl() {
  switch(var_type) {
  case T_Int:
    return new cssCPtr_int(&int_val, 0, name);
  case T_Real:
    return new cssCPtr_double(&real_val, 0, name);
  case T_String:
    return new cssCPtr_String(&string_val, 0, name);
  case T_Bool:
    return new cssCPtr_bool(&bool_val, 0, name);
  case T_Object: 
    return new cssSmartRef(&object_val, 0, &TA_taBaseRef, name);
  case T_HardEnum:
    return new cssCPtr_enum(&int_val, 0, name, hard_enum_type);
  case T_DynEnum:
    return new cssCPtr_DynEnum(&dyn_enum_val, 0, name);
  }
  return &cssMisc::Void;
}

cssEl* ProgVar::NewCssType() {
  if(var_type != T_DynEnum)
    return NULL;
  cssEnumType* et = new cssEnumType(dyn_enum_val.name);
  for(int i=0;i<dyn_enum_val.size;i++) {
    DynEnumItem* ev = dyn_enum_val.FastEl(i);
    et->enums->Push(new cssEnum(et, ev->value, ev->name));
  }
  return et;
}


//////////////////////////
//   ProgVar_List	//
//////////////////////////

void ProgVar_List::Initialize() {
  SetBaseType(&TA_ProgVar);
  var_context = VC_ProgVars;
}

void ProgVar_List::Copy_(const ProgVar_List& cp) {
  var_context = cp.var_context;
}

void ProgVar_List::El_SetIndex_(void* it_, int idx) {
  ProgVar* it = (ProgVar*)it_;
  if (it->name.empty()) {
    it->name = "Var_" + (String)idx;
  }
}

const String ProgVar_List::GenCss(int indent_level) const {
  String rval(0, 40 * size, '\0'); // buffer with typical-ish room
  ProgVar* el;
  int cnt = 0;
  for (int i = 0; i < size; ++i) {
    el = FastEl(i);
    bool is_arg = (var_context == VC_FuncArgs);
    if (is_arg) {
      if (cnt > 0)
        rval += ", ";
    } else {
      rval += cssMisc::Indent(indent_level); 
    }
    rval += el->GenCss(is_arg); 
    ++cnt;
  }
  return rval;
}

void ProgVar_List::setDirty(bool value) {
  inherited::setDirty(value);
  // if we are in a program group, dirty all progs
  // note: we have to test if in a prog first, otherwise we'll always get a group
  Program* prog = GET_MY_OWNER(Program);
  if (!prog) {
    Program_Group* grp = GET_MY_OWNER(Program_Group);
    if (grp)
      grp->SetProgsDirty();
  }
}


//////////////////////////
//   ProgArg		//
//////////////////////////

void ProgArg::Initialize() {
}

void ProgArg::Destroy() {
}

void ProgArg::Copy_(const ProgArg& cp) {
  name = cp.name;
  value = cp.value;
}

void ProgArg::Freshen(const ProgVar& cp) {
  // currently a no-op because we don't have any type info ourselves.
} 

String ProgArg::GetDisplayName() const {
  return name + "=" + value;
}

//////////////////////////
//   ProgArg_List	//
//////////////////////////

void ProgArg_List::Initialize() {
  SetBaseType(&TA_ProgArg);
}

void ProgArg_List::ConformToTarget(ProgVar_List& targ) {
  int i;  int ti;
  ProgArg* pa;
  ProgVar* pv;
  // delete args not in target; freshen those that are
  for (i = size - 1; i >= 0; --i) {
    pa = FastEl(i);
    pv = targ.FindName(pa->name, ti);
    if (ti >= 0) {
      pa->Freshen(*pv);
    } else {
      RemoveIdx(i);
    }
  }
  // add args in target not in us, and put in the right order
  for (ti = 0; ti < targ.size; ++ti) {
    pv =targ.FastEl(ti);
    FindName(pv->name, i);
    if (i < 0) {
      pa = new ProgArg();
      pa->name = pv->name;
      Insert(pa, ti);
    } else if (i != ti) {
      MoveIdx(i, ti);
    }
  }
}


//////////////////////////
//  ProgEl		//
//////////////////////////

void ProgEl::Initialize() {
  flags = PEF_NONE;
  off = false;			// todo remove
}

void ProgEl::Destroy() {
}

void ProgEl::Copy_(const ProgEl& cp) {
  desc = cp.desc;
  flags = cp.flags;
  off = cp.off; // todo remove
}

void ProgEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(off) { SetProgFlag(OFF); off = false; } // copy from obs and reset; todo remove
}

bool ProgEl::CheckConfig_impl(bool quiet) {
  if(CheckProgFlag(OFF)) {
    ClearCheckConfig();
    return true;
  }
  return inherited::CheckConfig_impl(quiet);
}

const String ProgEl::GenCss(int indent_level) {
  if(CheckProgFlag(OFF)) return "";
  String rval;
  if (useDesc() && !desc.empty()) {
    // we support multi-lines by using the multi-line form of comments
    if (desc.contains('\n')) {
      rval.cat(cssMisc::IndentLines("/* " + desc + " */\n", indent_level));
    } else {
      rval.cat(cssMisc::Indent(indent_level)).cat("// ").cat(desc).cat("\n");
    }
  }
  rval += GenCssPre_impl(indent_level);
  rval += GenCssBody_impl(indent_level);
  rval += GenCssPost_impl(indent_level);
  return rval;
}

int ProgEl::GetEnabled() const {
  if(CheckProgFlag(OFF)) return 0;
  ProgEl* par = parent();
  if (!par) return 1;
  if (par->GetEnabled())
    return 1;
  else return 0;
}

void ProgEl::PreGen(int& item_id) {
  if(CheckProgFlag(OFF)) return;
  PreGenMe_impl(item_id);
  ++item_id;
  PreGenChildren_impl(item_id);
}

//////////////////////////
//  ProgEl_List	//
//////////////////////////

void ProgEl_List::Initialize() {
  SetBaseType(&TA_ProgEl);
}

void ProgEl_List::Destroy() {
  Reset();
}

const String ProgEl_List::GenCss(int indent_level) {
  String rval;
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    rval += el->GenCss(indent_level); 
  }
  return rval;;
}

String ProgEl_List::GetColHeading(const KeyString& key) const {
  static String col0("El Type");
  static String col1("El Description");
  if (key == key_type)  return col0;
  else if (key == key_disp_name) return col1;
  else return inherited::GetColHeading(key);
}

const KeyString ProgEl_List::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_type;
  case 1: return key_disp_name;
  default: return _nilKeyString;
  }
}


void ProgEl_List::PreGen(int& item_id) {
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    el->PreGen(item_id);
  }
}

//////////////////////////
//  CodeBlock		//
//////////////////////////

void CodeBlock::Initialize() {
}

void CodeBlock::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  prog_code.CheckConfig(quiet, rval);
}

const String CodeBlock::GenCssBody_impl(int indent_level) {
  return prog_code.GenCss(indent_level);
}

String CodeBlock::GetDisplayName() const {
  return "CodeBlock (" + String(prog_code.size) + " items)";
}

void CodeBlock::PreGenChildren_impl(int& item_id) {
  prog_code.PreGen(item_id);
}

//////////////////////////
//  ProgVars		//
//////////////////////////

void ProgVars::Initialize() {
}

void ProgVars::Destroy() {
  CutLinks();
}

void ProgVars::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  local_vars.CheckConfig(quiet, rval);
}

const String ProgVars::GenCssBody_impl(int indent_level) {
  return local_vars.GenCss(indent_level);
}

String ProgVars::GetDisplayName() const {
  String rval;
  rval += "ProgVars (";
  rval += String(local_vars.size);
  rval += " vars)";
  return rval;
}

//////////////////////////
//    UserScript	//
//////////////////////////

void UserScript::Initialize() {
  static String _def_user_script("// TODO: Add your CSS script code here.\n");
  user_script = _def_user_script;
}

const String UserScript::GenCssBody_impl(int indent_level) {
  String rval(cssMisc::IndentLines(user_script, indent_level));
  // strip trailing non-newline ws, and make sure there is a trailing newline
  rval = trimr(rval);
  if (rval.lastchar() != '\n')
    rval += '\n';
  return rval;
}

String UserScript::GetDisplayName() const {
  return user_script;
}

void UserScript::ImportFromFile(istream& strm) {
  user_script = "";
  while(!strm.eof()) {
    char c = strm.get();
    if(c == EOF) break;
    user_script += c;
  }
}

void UserScript::ImportFromFileName(const String& fnm) {
  String full_fnm = taMisc::FindFileOnLoadPath(fnm);
  fstream strm;
  strm.open(full_fnm, ios::in);
  ImportFromFile(strm);
  strm.close();
}

void UserScript::ExportToFile(ostream& strm) {
  strm << user_script;
}

void UserScript::ExportToFileName(const String& fnm) {
  fstream strm;
  strm.open(fnm, ios::out);
  ExportToFile(strm);
  strm.close();
}

//////////////////////////
//  Loop		//
//////////////////////////

void Loop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(loop_test.empty()) {
    if(!quiet) taMisc::CheckError("Error in Loop in program:", program()->name, "loop_test expression is empty");
    rval = false;
  }
}

void Loop::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  loop_code.CheckConfig(quiet, rval);
}

const String Loop::GenCssBody_impl(int indent_level) {
  return loop_code.GenCss(indent_level + 1);
}

void Loop::PreGenChildren_impl(int& item_id) {
  loop_code.PreGen(item_id);
}


//////////////////////////
//  WhileLoop		//
//////////////////////////

const String WhileLoop::GenCssPre_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "while (" + loop_test + ") {\n";
}

const String WhileLoop::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "}\n";
}

String WhileLoop::GetDisplayName() const {
  return "while (" + loop_test + ")";
}

//////////////////////////
//  DoLoop		//
//////////////////////////

const String DoLoop::GenCssPre_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "do {\n";
  return rval; 
}

const String DoLoop::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "} while (" + loop_test + ");\n";
  return rval;
}

String DoLoop::GetDisplayName() const {
  return "do ... while (" + loop_test + ")";
}

//////////////////////////
//  ForLoop		//
//////////////////////////

void ForLoop::Initialize() {
  // the following are just default examples for the user
  init_expr = "int i = 0";
  loop_test = "i < 10";
  loop_iter = "i++";
}

void ForLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(loop_iter.empty()) {
    if(!quiet) taMisc::CheckError("Error in ForLoop in program:", program()->name, "loop_iter expression is empty");
    rval = false;
  }
}

const String ForLoop::GenCssPre_impl(int indent_level) {
  String rval;
  rval = cssMisc::Indent(indent_level) + 
    "for (" + init_expr + "; " + loop_test + "; " + loop_iter + ") {\n";
  return rval; 
}

const String ForLoop::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "}\n";
  return rval;
}

String ForLoop::GetDisplayName() const {
  return "for (" + init_expr + "; " + loop_test + "; " + loop_iter + ")";
}


//////////////////////////
//  IfContinue		//
//////////////////////////

void IfContinue::Initialize() {
}

void IfContinue::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if (condition.empty()) {
    if(!quiet) taMisc::CheckError("Error in IfContinue in program:", program()->name, "condition expression is empty");
    rval = false;
  }
  if((condition.freq('=') == 1) && !(condition.contains(">=") || condition.contains("<=")
				     || condition.contains("!="))) {
    if(!quiet) taMisc::CheckError("Error in IfContinue in program:", program()->name, "condition contains a single '=' assignment operator -- this is not the equals operator: == .  Fixed automatically");
    condition.gsub("=", "==");
    rval = false;
  }
}

const String IfContinue::GenCssBody_impl(int indent_level) {
  String rval;
  rval = cssMisc::Indent(indent_level) + 
    "if(" + condition + ") continue;\n";
  return rval; 
}

String IfContinue::GetDisplayName() const {
  return "if(" + condition + ") continue;";
}


//////////////////////////
//  IfBreak		//
//////////////////////////

void IfBreak::Initialize() {
}

void IfBreak::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if (condition.empty()) {
    if(!quiet) taMisc::CheckError("Error in IfBreak in program:", program()->name, "condition expression is empty");
    rval = false;
  }
  if((condition.freq('=') == 1) && !(condition.contains(">=") || condition.contains("<=")
				     || condition.contains("!="))) {
    if(!quiet) taMisc::CheckError("Error in IfBreak in program:", program()->name, "condition contains a single '=' assignment operator -- this is not the equals operator: == .  Fixed automatically");
    condition.gsub("=", "==");
    rval = false;
  }
}

const String IfBreak::GenCssBody_impl(int indent_level) {
  String rval;
  rval = cssMisc::Indent(indent_level) + 
    "if(" + condition + ") break;\n";
  return rval; 
}

String IfBreak::GetDisplayName() const {
  return "if(" + condition + ") break;";
}

//////////////////////////
//  IfReturn		//
//////////////////////////

void IfReturn::Initialize() {
}

void IfReturn::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if (condition.empty()) {
    if(!quiet) taMisc::CheckError("Error in IfReturn in program:", program()->name, "condition expression is empty");
    rval = false;
  }
  if((condition.freq('=') == 1) && !(condition.contains(">=") || condition.contains("<=")
				     || condition.contains("!="))) {
    if(!quiet) taMisc::CheckError("Error in IfReturn in program:", program()->name, "condition contains a single '=' assignment operator -- this is not the equals operator: == .  Fixed automatically");
    condition.gsub("=", "==");
    rval = false;
  }
}

const String IfReturn::GenCssBody_impl(int indent_level) {
  String rval;
  rval = cssMisc::Indent(indent_level) + 
    "if(" + condition + ") return;\n";
  return rval; 
}

String IfReturn::GetDisplayName() const {
  return "if(" + condition + ") return;";
}

//////////////////////////
//  IfElse		//
//////////////////////////

void IfElse::Initialize() {
  //  condition = "true";
}

void IfElse::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if (condition.empty()) {
    if(!quiet) taMisc::CheckError("Error in IfElse in program:", program()->name, "condition expression is empty");
    rval = false;
  }
  if((condition.freq('=') == 1) && !(condition.contains(">=") || condition.contains("<=")
				     || condition.contains("!="))) {
    if(!quiet) taMisc::CheckError("Error in IfElse in program:", program()->name, "condition contains a single '=' assignment operator -- this is not the equals operator: == .  Fixed automatically");
    condition.gsub("=", "==");
    rval = false;
  }
}

void IfElse::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  true_code.CheckConfig(quiet, rval);
  false_code.CheckConfig(quiet, rval);
}

const String IfElse::GenCssPre_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level);
  rval += "if (" + condition + ") {\n";
  return rval; 
}

const String IfElse::GenCssBody_impl(int indent_level) {
  String rval = true_code.GenCss(indent_level + 1);
  // don't gen 'else' portion unless there are els
  if (false_code.size > 0) {
    rval += cssMisc::Indent(indent_level) + "} else {\n";
    rval += false_code.GenCss(indent_level + 1);
  }
  return rval;
}

const String IfElse::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "}\n";
}

String IfElse::GetDisplayName() const {
  return "if (" + condition + ")";
}

void IfElse::PreGenChildren_impl(int& item_id) {
  true_code.PreGen(item_id);
  false_code.PreGen(item_id);
}

//////////////////////////
//    AssignExpr	//
//////////////////////////

void AssignExpr::Initialize() {
}

void AssignExpr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if(prg) prognm = prg->name;
  if(!result_var) {
    if(!quiet) taMisc::CheckError("Error in AssignExpr in program:", prognm, "result_var is NULL");
    rval = false;
  }
}

const String AssignExpr::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!result_var) {
    rval += "//WARNING: AssignExpr not generated here -- result_var not specified\n";
    return rval;
  }
  
  rval += result_var->name + " = " + expr + ";\n";
  return rval;
}

String AssignExpr::GetDisplayName() const {
  if(!result_var)
    return "(result_var not selected)";
  
  String rval;
  rval += result_var->name + "=" + expr;
  return rval;
}

//////////////////////////
//    MethodCall	//
//////////////////////////

void MethodCall::Initialize() {
  method = NULL;
  obj_type = &TA_taBase; // placeholder
}

void MethodCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(obj)
    obj_type = obj->act_object_type();
  else obj_type = &TA_taBase; // placeholder

  if(!taMisc::is_loading && method)
    UpdateArgs(args, method);
}

void MethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if(prg) prognm = prg->name;
  if(!obj) {
    if(!quiet) taMisc::CheckError("Error in MethodCall in program:", prognm, "obj is NULL");
    rval = false;
  }
  if(!method) {
    if(!quiet) taMisc::CheckError("Error in MethodCall in program:", prognm, "method is NULL");
    rval = false;
  }
}

const String MethodCall::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!(obj && method)) {
    rval += "//WARNING: MethodCall not generated here -- obj or method not specified\n";
   return rval;
  }
  
  if(result_var)
    rval += result_var->name + " = ";
  rval += obj->name;
  rval += "->";
  rval += method->name;
  rval += "(";
    for (int i = 0; i < args.size; ++ i) {
      if (i > 0) rval += ", ";
      rval += args[i];
    }
  rval += ");\n";
  
  return rval;
}

String MethodCall::GetDisplayName() const {
  if (!obj || !method)
    return "(object or method not selected)";
  
  String rval;
  if(result_var)
    rval += result_var->name + "=";
  rval += obj->name;
  rval += "->";
  rval += method->name;
  rval += "(";
  for(int i=0;i<args.size;i++) {
    if (i > 0)
      rval += ", ";
    rval += args.labels[i] + "=" + args[i];
  }
  rval += ")";
  return rval;
}

void MethodCall::UpdateArgs(SArg_Array& ar, MethodDef* md) {
  int i;  int ti;
  // delete args not in md list
  for (i = ar.size - 1; i >= 0; --i) {
    String an = ar.labels[i];
    String mnm = an.after(" ",-1);// past type
    int ti = md->arg_names.FindEl(mnm);
    if (ti < 0) {
      ar.RemoveIdx(i);
      ar.labels.RemoveIdx(i);
    }
  }
  // add args in target not in us, and put in the right order
  for (ti = 0; ti < md->arg_names.size; ++ti) {
    TypeDef* arg_typ = md->arg_types.FastEl(ti);
    String arg_nm = arg_typ->Get_C_Name() + " " + md->arg_names[ti];
    int i = ar.labels.FindEl(arg_nm);
    if (i < 0) {
      ar.labels.Insert(arg_nm, ti);
      String def_val = md->arg_defs.SafeEl(ti);
      def_val.gsub(" ", "");
      if(arg_typ->is_enum() && !def_val.contains("::")) {
	TypeDef* ot = arg_typ->GetOwnerType();
	if(ot)
	  def_val = ot->name + "::" + def_val;
      }
      else if(arg_typ->InheritsFrom(TA_taString)) {
	if(def_val.empty()) def_val = "\"\""; // empty string
      }
      else if(def_val == "__null") {
	def_val = "NULL";
      }
      ar.Insert(def_val, ti);
    } else if (i != ti) {
      ar.labels.MoveIdx(i, ti);
      ar.MoveIdx(i, ti);
    }
  }
}


//////////////////////////
//    StaticMethodCall	//
//////////////////////////

void StaticMethodCall::Initialize() {
  method = NULL;
  min_type = &TA_taBase;
  object_type = &TA_taBase;
}

void StaticMethodCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!taMisc::is_loading && method)
    MethodCall::UpdateArgs(args, method);
}

void StaticMethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if(prg) prognm = prg->name;
  if(!method) {
    if(!quiet) taMisc::CheckError("Error in StaticMethodCall in program:", prognm, "method is NULL");
    rval = false;
  }
}

const String StaticMethodCall::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!method) {
    rval += "//WARNING: StaticMethodCall not generated here -- object or method not specified\n";
    return rval;
  }
  
  if (result_var)
    rval += result_var->name + " = ";
  rval += object_type->name;
  rval += "::";
  rval += method->name;
  rval += "(";
    for (int i = 0; i < args.size; ++ i) {
      if (i > 0) rval += ", ";
      rval += args[i];
    }
  rval += ");\n";
  
  return rval;
}

String StaticMethodCall::GetDisplayName() const {
  if (!method)
    return "(method not selected)";
  
  String rval;
  if(result_var)
    rval += result_var->name + "=";
  rval += object_type->name;
  rval += "::";
  rval += method->name;
  rval += "(";
  for(int i=0;i<args.size;i++) {
    if (i > 0)
      rval += ", ";
    rval += args.labels[i] + "=" + args[i];
  }
  rval += ")";
  return rval;
}

void MathCall::Initialize() {
  min_type = &TA_taMath;
  object_type = &TA_taMath;
}

void RandomCall::Initialize() {
  min_type = &TA_Random;
  object_type = &TA_Random;
}

void MiscCall::Initialize() {
  min_type = &TA_taMisc;
  object_type = &TA_taMisc;
}

//////////////////////////
//      PrintVar	//
//////////////////////////

void PrintVar::Initialize() {
}

void PrintVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if(prg) prognm = prg->name;
  if(!print_var) {
    if(!quiet) taMisc::CheckError("Error in PrintVar in program:", prognm, "print_var is NULL");
    rval = false;
  }
}

const String PrintVar::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!print_var) {
    rval += "//WARNING: PrintVar not generated here -- print_var not specified\n";
    return rval;
  }
  
  rval += "cerr << \"" + print_var->name + " = \" << " + print_var->name + " << endl;\n";
  return rval;
}

String PrintVar::GetDisplayName() const {
  if(!print_var)
    return "(print_var not selected)";
  
  String rval;
  rval += "Print: " + print_var->name;
  return rval;
}


//////////////////////////
//      Comment 	//
//////////////////////////

const String Comment::decorate_key("comment");

void Comment::Initialize() {
  static String _def_comment("TODO: Add your program comment here (multi-lines ok).\n");
  desc = _def_comment;
}

const String Comment::GenCssBody_impl(int indent_level) {
  STRING_BUF(rval, desc.length() + 160);
  rval += cssMisc::Indent(indent_level);
  rval += "/*******************************************************************\n";
  rval += cssMisc::IndentLines(desc, indent_level);
  rval = trimr(rval);
  if (rval.lastchar() != '\n')
    rval += '\n';
  rval += cssMisc::Indent(indent_level);
  rval += "*******************************************************************/\n";
  return rval;
}

String Comment::GetDisplayName() const {
  return desc;
}


//////////////////////////
//      StopStepPoint 	//
//////////////////////////

void StopStepPoint::Initialize() {
}

const String StopStepPoint::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level) + "StopCheck(); // check for Stop or Step button\n";
  return rval;
}

String StopStepPoint::GetDisplayName() const {
  return "Stop/Step Point";
}


//////////////////////////
//   ProgramCall	//
//////////////////////////


void ProgramCall::Initialize() {
  call_init = false;
  taBase::Own(target, this);
}

void ProgramCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateArgs();		// always do this..  nondestructive and sometimes stuff changes anyway
}

void ProgramCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!target) {
    if(!quiet) taMisc::CheckError("Error in ProgramCall in program:", program()->name, "target is NULL");
    rval = false;
  }
}

Program* ProgramCall::GetTarget() {
  if(!target) {
    taMisc::CheckError("Program target is NULL in ProgramCall:",
		       desc, "in program:", program()->name);
  }
  // NOTE: we are assuming that it has been compiled from the Init process
  // calling here causes many recompiles because of the dirty bit propagation
  // during running
//   if(!target->CompileScript()) {
//     taMisc::CheckError("Program target script did not compile correctly in ProgramCall:",
// 		       desc, "in program:", program()->name);
//   }
  return target.ptr();
}

const String ProgramCall::GenCssPre_impl(int indent_level) {
  String rval;
  rval = cssMisc::Indent(indent_level);
  rval += "{ // call program: "; 
  if (target)
    rval += target->name;
  rval += "\n";
  return rval;
}

const String ProgramCall::GenCssBody_impl(int indent_level) {
  if (!target) return _nilString;
  String rval;
  indent_level++;		// everything is indented from outer block
  rval += cssMisc::Indent(indent_level);
  rval += "Program* target = this" + GetPath(NULL, program())+ "->GetTarget();\n";
  rval += cssMisc::Indent(indent_level);
  rval += "if(target != NULL) {\n";
  if (prog_args.size > 0) {
    rval += cssMisc::Indent(indent_level+1);
    rval += "// set global vars of target\n";
  }
  String nm;
  bool set_one = false;
  for (int i = 0; i < prog_args.size; ++i) {
    ProgArg* ths_arg = prog_args.FastEl(i);
    nm = ths_arg->name;
    ProgVar* prg_var = target->args.FindName(nm);
    if (!prg_var || ths_arg->value.empty()) continue;
    set_one = true;
    rval += cssMisc::Indent(indent_level+1);
    rval += "target->SetVar(\"" + prg_var->name + "\", "
      + ths_arg->value + ");\n";
  }
//   if (set_one) {
//     rval += cssMisc::Indent(indent_level+1);
//     rval += "target->DataChanged(DCR_ITEM_UPDATED);\n";
//   }
  rval += cssMisc::Indent(indent_level+1);
  if(call_init)
    rval += "{ target->CallInit(this); }\n";
  else
    rval += "{ target->Call(this); }\n";
  rval += cssMisc::Indent(indent_level);
  rval += "}\n";
  
  return rval;
}

const String ProgramCall::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "} // call program\n";
}

String ProgramCall::GetDisplayName() const {
  String rval = "Call ";
  if (target) {
    rval += target->GetName();
//     if(prog_args.size > 0) {
//     rval += "(";
//     for(int i=0;i<prog_args.size;i++) {
//       ProgArg* pa = prog_args.FastEl(i);
//       rval += pa->GetDisplayName();
//     }
//     rval += ")";
  }
  else
    rval += "(no program set)";
  return rval;
}

void ProgramCall::PreGenMe_impl(int item_id) {
  // register as a subproc, but only if not a recursive call (which is bad anyway!)
  if (!target) return; // not target (yet), nothing to register
  Program* prog = program();
  if (!prog) return; // shouldn't normally happen
  prog->sub_progs.LinkUnique(this);
}

void ProgramCall::UpdateArgs() {
  if (!target) return; // just leave existing stuff for now
  prog_args.ConformToTarget(target->args);
  // now go through and set default value for variables of same name in this program
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return;
  for(int i=0;i<prog_args.size; i++) {
    ProgArg* pa = prog_args.FastEl(i);
    if(!pa->value.empty()) continue; // skip if already set
    ProgVar* arg_chk = prg->args.FindName(pa->name);
    ProgVar* var_chk = prg->vars.FindName(pa->name);
    if(!arg_chk && !var_chk) continue; 
    pa->value = pa->name;	// we found var of same name; set as arg value
  }
}


//////////////////////////
//  Function		//
//////////////////////////

void Function::Initialize() {
  args.var_context = ProgVar_List::VC_FuncArgs;
  return_val.name = "rval";
}

void Function::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  name = taMisc::StringCVar(name); // make names C legal names
}

void Function::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(name.empty()) {
    if(!quiet) taMisc::CheckError("Error in Function: name is empty in program:",
				  program()->name, ".  Functions must be named");
  }
}

void Function::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  args.CheckConfig(quiet, rval);
  fun_code.CheckConfig(quiet, rval);
}

const String Function::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level) + return_val.GenCssType() + " " + name + "(";
  if(args.size > 0) {
    rval += args.GenCss(0);
  }
  rval += ") {\n";
  rval += fun_code.GenCss(indent_level + 1);
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval;
}

String Function::GetDisplayName() const {
  String rval;
  rval += return_val.GenCssType() + " " + name + "(";
  if(args.size > 0) {
    rval += args.GenCss(0);
  }
  rval += ")";
  return rval;
}

void Function::PreGenChildren_impl(int& item_id) {
  fun_code.PreGen(item_id);
}


//////////////////////////
//   FunctionCall	//
//////////////////////////

void FunctionCall::Initialize() {
}

void FunctionCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateArgs();		// always do this.. nondestructive and sometimes stuff changes anyway
}

void FunctionCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!fun) {
    if(!quiet) taMisc::CheckError("Error in FunctionCall in program:", program()->name, "fun is NULL");
    rval = false;
  }
}

const String FunctionCall::GenCssBody_impl(int indent_level) {
  if (!fun) return _nilString;
  String rval = cssMisc::Indent(indent_level);
  if(result_var) {
    rval += result_var->name + "=";
  }
  rval += fun->name + "(";
  for (int i = 0; i < fun_args.size; ++i) {
    ProgArg* ths_arg = fun_args.FastEl(i);
    if(i > 0) rval += ", ";
    rval += ths_arg->value;
  }
  rval += ");\n";
  return rval;
}

String FunctionCall::GetDisplayName() const {
  String rval = "Call ";
  if (fun) {
    rval += fun->name;
    if(fun_args.size > 0) {
      rval += "(";
      for(int i=0;i<fun_args.size;i++) {
        ProgArg* pa = fun_args.FastEl(i);
        rval += pa->GetDisplayName();
      }
      rval += ")";
    }
  } else
    rval += "(no function set)";
  return rval;
}

void FunctionCall::UpdateArgs() {
  if(!fun) return; // just leave existing stuff for now
  fun_args.ConformToTarget(fun->args);
}

//////////////////////////
//    ReturnExpr	//
//////////////////////////

void ReturnExpr::Initialize() {
}

const String ReturnExpr::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level);
  rval += "return " + expr + ";\n";
  return rval;
}

String ReturnExpr::GetDisplayName() const {
  String rval;
  rval += "return " + expr;
  return rval;
}

///////////////////////////////////////////////////////////////
//  	ProgObjList

void ProgObjList::GetVarsForObjs() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return;
  for(int i = 0; i < size; ++i) {
    taBase* obj = FastEl(i);
    String nm = obj->GetName();
    if(nm.empty()) continue;
    ProgVar* var = prog->vars.FindName(nm);
    if(var) {
      if((var->var_type != ProgVar::T_Object) || (var->object_val != obj)) {
	taMisc::Error("Program error: variable named:", nm,
		      "exists, but does not refer to object in objs list -- rename either to avoid conflict");
      }
      else {
	var->objs_ptr = true;	// make sure
	var->object_type = obj->GetTypeDef();
        var->DataChanged(DCR_ITEM_UPDATED);
      }
    }
    else {
      bool found_it = false;
      for(int j=0;j<prog->vars.size; j++) {
	ProgVar* tv = prog->vars[j];
	if((tv->var_type == ProgVar::T_Object) && (tv->object_val == obj)) {
	  found_it = true;
	  tv->name = nm;	// update the name
	  tv->objs_ptr = true;	// make sure
	  tv->object_type = obj->GetTypeDef();
	  tv->DataChanged(DCR_ITEM_UPDATED);
	  break;
	}
      }
      if(!found_it) {
	var = (ProgVar*)prog->vars.New(1, &TA_ProgVar);
	var->name = nm;
	var->var_type = ProgVar::T_Object;
	var->object_val = obj;
	var->objs_ptr = true;
	var->object_type = obj->GetTypeDef();
        var->DataChanged(DCR_ITEM_UPDATED);
      }
    }
  }
  // now cleanup any orphaned 
  for(int i = prog->vars.size-1; i >= 0; --i) {
    ProgVar* var = prog->vars[i];
    if(!var->objs_ptr) continue;
    taBase* obj = FindName(var->name);
    if(obj == NULL)
      prog->vars.RemoveIdx(i);		// get rid of it
  }
}

void ProgObjList::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if(!taMisc::is_loading && !taMisc::is_duplicating)
    GetVarsForObjs();
}

void* ProgObjList::El_Own_(void* it_) {
  // note: gen the name first, so we don't need to do another notify
  // note: setting default name is mostly for things like taMatrix
  // that are normally anon, but that would like a name here in objs
  taBase* it = (taBase*)it_;
  if (it && it->GetName().empty()) {
    // first, try the default token-based naming
    it->SetDefaultName_();
    // if still empty, we could gen a synthetic name
    if (it->GetName().empty()) {
      it->SetName("Obj_" + String(size + 1));
    }
  }
  void* rval = inherited::El_Own_(it_);
  return rval;
}


//////////////////////////
//  Program		//
//////////////////////////

ProgLib* Program::prog_lib = NULL;

void Program::MakeTemplate_fmtype(Program* prog, TypeDef* td) {
  taBase* tok = (taBase*)td->GetInstance();
  if(tok != NULL) {
    taBase* o = tok->MakeToken();
    o->SetName("New" + td->name);
    prog->init_code.Add(o);
  }
  for(int i=0;i<td->children.size;i++) {
    TypeDef* chld = td->children[i];
    MakeTemplate_fmtype(prog, chld);
  }
}

Program* Program::MakeTemplate() {
//TODO: this will probably get nuked and replaced with a generic maker on .root
  Program* prog = new Program;
  {ProgVar* o = new ProgVar; o->SetName("NewProgVar"); prog->vars.Add(o);}
  //note: prog args go into a ProgramCall etc., so we just add the tmpl to the objects
  {ProgArg* o = new ProgArg; o->SetName("NewProgArg"); prog->objs.Add(o);}
  //note: put in .init since that will get searched first
  
  MakeTemplate_fmtype(prog, &TA_ProgEl);
  return prog;
}
  
bool Program::stop_req = false;
bool Program::step_mode = false;

void Program::Initialize() {
  run_state = NOT_INIT;
  flags = PF_NONE;
  objs.SetBaseType(&TA_taNBase);
  ret_val = 0;
  m_dirty = true; 
  prog_gp = NULL;
  m_checked = false;
  if(!prog_lib)
    prog_lib = &Program_Group::prog_lib;
}

void Program::Destroy()	{ 
  CutLinks();
}

void Program::InitLinks() {
  inherited::InitLinks();
  taBase::Own(objs, this);
  taBase::Own(args, this);
  taBase::Own(vars, this);
  taBase::Own(functions, this);
  taBase::Own(init_code, this);
  taBase::Own(prog_code, this);
  taBase::Own(sub_progs, this);
  prog_gp = GET_MY_OWNER(Program_Group);
}

void Program::CutLinks() {
  sub_progs.CutLinks();
  prog_code.CutLinks();
  init_code.CutLinks();
  functions.CutLinks();
  vars.CutLinks();
  args.CutLinks();
  objs.CutLinks();
  prog_gp = NULL;
  inherited::CutLinks();
}

void Program::Reset() {
  sub_progs.Reset();
  prog_code.Reset();
  init_code.Reset();
  functions.Reset();
  vars.Reset();
  args.Reset();
  objs.Reset();
}

void Program::Copy_(const Program& cp) {
  if(script) {			// clear first, before trashing anything!
    script->ClearAll();
    script->prog_vars.Reset();
  }
  desc = cp.desc;
  objs = cp.objs;
  args = cp.args;
  vars = cp.vars;
  functions = cp.functions;
  init_code = cp.init_code;
  prog_code = cp.prog_code;
  ret_val = 0; // redo
  m_dirty = true; // require rebuild/refetch
  m_scriptCache = "";
  m_checked = false; // redo
  sub_progs.RemoveAll();
  UpdatePointers_NewPar((taBase*)&cp, this); // update any pointers within this guy
}

void Program::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //WARNING: the running css prog calls this on any changes to our vars,
  // such as ret_val -- therefore, DO NOT do things here that are incompatible
  // with the runtime, in particular, do NOT invalidate the following state flags:
  //   m_dirty, script_compiled
  
  //TODO: the following *do* affect generated script, so we should probably call
  // setDirty(true) if not running, and these changed:
  // name, (more TBD...)
}

bool Program::CheckConfig_impl(bool quiet) {
  //TODO: global program dependencies and other objects -- check them all here
  bool rval = inherited::CheckConfig_impl(quiet);
  m_checked = true;
  if (!rval) ret_val = RV_CHECK_ERR;
  return rval;
}


void Program::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  args.CheckConfig(quiet, rval);
  vars.CheckConfig(quiet, rval);
  functions.CheckConfig(quiet, rval);
  init_code.CheckConfig(quiet, rval);
  prog_code.CheckConfig(quiet, rval);
  // todo: go through and check that the functions contains valid function els!?
}

int Program::Call(Program* caller) {
  setRunState(RUN);
  int rval = Cont_impl();
  if(stop_req) {
    script->Stop();		// stop us
    caller->script->Stop();	// stop caller!
    caller->script->Prog()->Frame()->pc = 0;
    setRunState(STOP);		// we are done
    // NOTE: this backs up to restart the entire call to fun -- THIS DEPENDS ON THE CODE
    // that generates the call!!!!!  ALWAYS MUST BE IN A SUB-BLOCK of code..
  }
  else {
    script->Restart();		// restart script at beginning if run again	
    setRunState(DONE);		// we are done
  }
  return rval;
} 

int Program::CallInit(Program* caller) {
  setRunState(INIT); // this is redundant if called from an existing INIT but otherwise needed
  int rval = Run_impl();
  script->Restart(); // for init, always restart script at beginning if run again	
  setRunState(DONE);		// always done..
  return rval;
} 

void Program::Init() {
  taMisc::Busy();
  setRunState(INIT);
  DataChanged(DCR_ITEM_UPDATED); // update button state
  taMisc::CheckConfigStart(false);
  CheckConfig(false); //sets ret_val on fail
  Run_impl();
  taMisc::DoneBusy();
  if (ret_val != RV_OK) ShowRunError();
  script->Restart();		// restart script at beginning if run again
  taMisc::CheckConfigEnd(); // no flag, because any nested fails will have set it
  setRunState(DONE);
  DataChanged(DCR_ITEM_UPDATED); // update after macroscopic button-press action..
} 

bool Program::PreCompileScript_impl() {
  // as noted in abstractscriptbase: you must call this first to reset the script
  // because if you mess with the existing variables in prog_vars prior to 
  // resetting the script, it will get all messed up.  vars on this space are referred
  // to by a pointer to the space and an index off of it, which is important for autos
  // but actually not for these guys (but they are/were that way anyway).
  if(!AbstractScriptBase::PreCompileScript_impl()) return false;
  objs.GetVarsForObjs();
  UpdateProgVars();
  //Note: following may be a nested invocation
  if (!CheckConfig(false)) return false; 
  return true;
}

void Program::setRunState(RunState value) {
  if (run_state == value) return;
  run_state = value;
  // DO NOT DO THIS!! Definitely generates too much overhead
  // datachanged called only after macroscopic action
  //  DataChanged(DCR_ITEM_UPDATED);
}

int Program::Run_impl() {
  ret_val = RV_OK;
  if (!CompileScript()) {
    if (ret_val != RV_CHECK_ERR)
      ret_val = RV_COMPILE_ERR;
  }
  if (ret_val == RV_OK) {
    script->Run();
    // DO NOT DO!
    // DataChanged(DCR_ITEM_UPDATED);
  }
  return ret_val;
}

int Program::Cont_impl() {
  // note: cont is fast and does not do any compile or run checks.
  // the user cannot access this without having pressed Init first, and that
  // does all the checks.  this is the standard paradigm for such things --
  // init does checks. run assumes things are ok & thus can be fast.
  script->Cont();
  // note: shared var state likely changed, so update gui
  script_compiled = true; // override any run-generated changes!!
  // do not update this -- too tight -- only at end!
  // DataChanged(DCR_ITEM_UPDATED);
  return ret_val;
}

void Program::Run() {
  stop_req = false;
  step_mode = false;
  taMisc::Busy();
  setRunState(RUN);
  DataChanged(DCR_ITEM_UPDATED); // update button state
  Cont_impl();
  taMisc::DoneBusy();
  if (ret_val != RV_OK) ShowRunError();
  // unless we were stopped, we are done
  if(stop_req) {
    setRunState(STOP);
  }
  else {
    script->Restart();
    setRunState(DONE);
  }
  stop_req = false;
  DataChanged(DCR_ITEM_UPDATED); // update after macroscopic button-press action..
} 

void Program::ShowRunError() {
  //note: if there was a ConfigCheck error, the user already got a dialog
  if (ret_val == RV_CHECK_ERR) return;
  String err_str = "Error: The Program did not run -- ret_val=";
  err_str.cat( GetTypeDef()->GetEnumString("ReturnVal", ret_val));
  if (ret_val == RV_COMPILE_ERR) {
    err_str += " (a program did not compile correctly: check the console for error messages)";
  }
  taMisc::Error(err_str);
}

void Program::Step() {
  if(!prog_gp) return;
  if(!prog_gp->step_prog) {
    prog_gp->step_prog = prog_gp->Peek();
  }
  stop_req = false;
  step_mode = true;
  taMisc::Busy();
  setRunState(RUN);
  DataChanged(DCR_ITEM_UPDATED); // update button state
  Cont_impl();
  taMisc::DoneBusy();
  if (ret_val != 0) {//TODO: use enums and sensible output string
    QMessageBox::warning(NULL, QString("Operation Failed"),
      String(
      "The Program did not run -- ret_val=").cat(String(ret_val)), 
      QMessageBox::Ok, QMessageBox::NoButton);
  }
  step_mode = false;
  if(stop_req) {
    setRunState(STOP);
  }
  else {
    script->Restart();
    setRunState(DONE);
  }
  stop_req = false;
  DataChanged(DCR_ITEM_UPDATED); // update after macroscopic button-press action..
}

void Program::SetAsStep() {
  if(!prog_gp) return;
  prog_gp->step_prog = this;
}

void Program::Stop() {
  stop_req = true;
}

void Program::Abort() {
  stop_req = true;
  Stop_impl();
}

void Program::Stop_impl() {
  script->Stop();
  setRunState(STOP);
  DataChanged(DCR_ITEM_UPDATED); // update button state
}

bool Program::StopCheck() {
  //NOTE: we call event loop even in non-gui compile, since we can presumably
  // have other ways of stopping, such as something from a socket etc.
  QCoreApplication::processEvents();
  // NOTE: the return value of this function is not actually what determines stopping
  // the above processEvents will process any Stop events and this will directly cause
  // css to stop in its tracks.
  if(run_state == STOP) return true;
  if(stop_req) {
    Stop_impl();
    return true;
  }
  if((step_mode) && prog_gp && (prog_gp->step_prog.ptr() == this)) {
    stop_req = true;			// stop everyone else
    Stop_impl();			// time for us to stop
    return true;
  }
  return false;
}

void Program::Compile() {
  CompileScript(true);		// always force if command entered
}

void Program::CmdShell() {
  CmdShellScript();
}

void Program::ExitShell() {
  ExitShellScript();
}

void Program::ScriptCompiled() {
  AbstractScriptBase::ScriptCompiled();
  script_compiled = true;
  ret_val = 0;
//TODO: need to globally solve the never-clean recursive dirty issue
  m_dirty = true; //TEMP: prevents recursion
  DataChanged(DCR_ITEM_UPDATED); // this will call setDirty
  m_dirty = false;
}

void Program::setDirty(bool value) {
  if(run_state == RUN) return;	     // change is likely self-generated during running, don't do it!
  if (m_dirty == value) return; // prevent recursion and spurious inherited calls!!!!
  inherited::setDirty(value); // needed to dirty the Project
  if(value) script_compiled = false; // make sure this always reflects dirty status -- is used as check for compiling..
  m_dirty = value;
  script_compiled = false; // have to assume user changed something
  sub_progs.RemoveAll(); // will need to re-enumerate
  DirtyChanged_impl();
  DataChanged(DCR_ITEM_UPDATED); //note: recurses us, but 
}

bool Program::SetVar(const String& nm, const Variant& value) {
  if(!script) return false;
  cssElPtr& el_ptr = script->prog_vars.FindName(nm);
  if (el_ptr == cssMisc::VoidElPtr) return false;
  cssEl* el = el_ptr.El();
  *el = value;
  return true;
}

const String Program::scriptString() {
  if (m_dirty) {
    // enumerate all the progels, esp. to get subprocs registered
    int item_id = 0;
    prog_code.PreGen(item_id);
    
    // now, build the new script code
    m_scriptCache = "// ";
    m_scriptCache += GetName();
    m_scriptCache += "\n\n/* globals added to hardvars:\n";
    m_scriptCache += "Program::RunState run_state; // our program's run state\n";
    m_scriptCache += "int ret_val;\n";
    if (args.size > 0) {
      m_scriptCache += "// global script parameters\n";
      m_scriptCache += args.GenCss(0);
    }
    if (vars.size > 0) {
      m_scriptCache += "// global (non-param) variables\n";
      m_scriptCache += vars.GenCss(0);
    }
    m_scriptCache += "*/\n\n";

    // Functions
    m_scriptCache += functions.GenCss(0); // ok if empty, returns nothing
    
    // __Init() routine, for our own els, and calls to subprog Init()
    m_scriptCache += "void __Init() {\n";
    m_scriptCache += init_code.GenCss(1); // ok if empty, returns nothing
    if (sub_progs.size > 0) {
      if (init_code.size >0) m_scriptCache += "\n";
      m_scriptCache += "  // init any subprogs that could be called from this one\n";
      m_scriptCache += "  { Program* target;\n";
      // note: this is a list of ProgramCall's, not the actual prog itself!
      for (int i = 0; i < sub_progs.size; ++i) {
        ProgramCall* sp = (ProgramCall*)sub_progs.FastEl(i);
        m_scriptCache += "    if (ret_val != Program::RV_OK) return; // checks previous\n"; 
        m_scriptCache += "    target = this" + sp->GetPath(NULL, this) + "->GetTarget();\n";
        m_scriptCache += "    target->CompileScript(); // needs to be compiled before setting vars\n";
	// set args for guys that are just passing our existing args/vars along
	for (int j = 0; j < sp->prog_args.size; ++j) {
	  ProgArg* ths_arg = sp->prog_args.FastEl(j);
	  ProgVar* prg_var = sp->target->args.FindName(ths_arg->name);
	  if (!prg_var || ths_arg->value.empty()) continue;
	  // check to see if the value of this guy is an arg or var of this guy -- if so, propagate it
	  ProgVar* arg_chk = args.FindName(ths_arg->value);
	  ProgVar* var_chk = vars.FindName(ths_arg->value);
	  if(!arg_chk && !var_chk) continue; 
	  m_scriptCache += "    target->SetVar(\"" + prg_var->name + "\", "
	    + ths_arg->value + ");\n";
	}
        m_scriptCache += "    ret_val = target->CallInit(this);\n"; 
      }
      m_scriptCache += "  }\n";
    }
    m_scriptCache += "}\n\n";
    
    m_scriptCache += "void __Prog() {\n";
    m_scriptCache += prog_code.GenCss(1);
    if(!(flags & NO_STOP)) {
      m_scriptCache += "  StopCheck(); // process pending events, including Stop and Step events\n";
    }
    m_scriptCache += "}\n\n";
    m_scriptCache += "\n";
    
    m_scriptCache += "ret_val = Program::RV_OK; // set elsewise on failure\n";
    m_scriptCache += "if (run_state == Program::INIT) {\n";
    m_scriptCache += "  __Init();\n";
    m_scriptCache += "} else {\n";
    m_scriptCache += "  __Prog();\n";
    m_scriptCache += "}\n";

    m_dirty = false;
  }
  return m_scriptCache;
}

void  Program::UpdateProgVars() {
  // note: this assumes that script has been ClearAll'd
  script->prog_vars.Reset(); // removes/unref-deletes
  script->prog_types.Reset(); // removes/unref-deletes
  
  // add the ones in the object -- note, we use *pointers* to these
  cssEl* el = NULL;
  el = new cssCPtr_enum(&run_state, 1, "run_state",
			TA_Program.sub_types.FindName("RunState"));
  script->prog_vars.Push(el);
  el = new cssCPtr_int(&ret_val, 1, "ret_val");
  script->prog_vars.Push(el);
  el = new cssTA_Base(&objs, 1, objs.GetTypeDef(), "objs");
  script->prog_vars.Push(el);

  // add new in the program
  for (int i = 0; i < args.size; ++i) {
    ProgVar* sv = args.FastEl(i);
    el = sv->NewCssEl();
    script->prog_vars.Push(el);
    el = sv->NewCssType();	// for dynenums
    if(el != NULL)
      script->prog_types.Push(el);
  } 
  for (int i = 0; i < vars.size; ++i) {
    ProgVar* sv = vars.FastEl(i);
    el = sv->NewCssEl();
    script->prog_vars.Push(el); //refs
    el = sv->NewCssType();	// for dynenums
    if(el != NULL)
      script->prog_types.Push(el);
  } 
}

String Program::GetProgLibPath(ProgLibs library) {
  if(library == SEARCH_LIBS) {
    taMisc::Error("Cannot do SEARCH_LIBS for saving -- program saved in local directory!");
    return "";
  }
  String path = "./";
  if(library == USER_LIB)
    path = taMisc::prog_lib_paths.GetVal("UserLib").toString();
  else if(library == SYSTEM_LIB)
    path = taMisc::prog_lib_paths.GetVal("SystemLib").toString();
  else if(library == WEB_LIB)
    path = taMisc::prog_lib_paths.GetVal("WebLib").toString();
  if(library != WEB_LIB) {
    int acc = access(path, F_OK);
    if (acc != 0) {
      QDir qd;
      qd.mkpath(path);		// attempt to make it..
      taMisc::Warning("Note: did mkdir for program library directory:", path);
    }
  }
  return path;
}

void Program::SaveToProgLib(ProgLibs library) {
  String path = GetProgLibPath(library);
  String fname = path + "/" + name + ".prog";
  int acc = access(fname, F_OK);
  if (acc == 0) {
    int chs = taMisc::Choice("Program library file: " + fname + " already exists: Overwrite?",
			     "Ok", "Cancel");
    if(chs == 1) return;
  }
  String tmp_last_dir = taFiler::last_dir;
  SaveAs(fname);
  taFiler::last_dir = tmp_last_dir;
  Program_Group::prog_lib.FindPrograms();
}

void Program::LoadFromProgLib(ProgLibEl* prog_type) {
  if(!prog_type) return;
  Reset();
  prog_type->LoadProgram(this);
}

void Program::SaveScript(ostream& strm) {
  strm << scriptString();
}

#ifdef TA_GUI
void Program::ViewScript() {
  ViewScript_impl();
}

void Program::EditScript() {
  String fnm = name + "_edit.css";
  fstream strm;
  strm.open(fnm, ios::out);
  SaveScript(strm);
  strm.close();

  String edtr = taMisc::edit_cmd; //don't run gsub on the original string!
  edtr.gsub("%s", fnm);
  system(edtr);
}

void Program::ViewScript_impl() {
  iTextEditDialog* dlg = new iTextEditDialog(true); // readonly
  dlg->setText(scriptString());
  dlg->exec();
}
#endif  // TA_GUI


//////////////////////////
//  Program_Group	//
//////////////////////////

void Program_Group::Initialize() {
  SetBaseType(&TA_Program);
}

void Program_Group::InitLinks() {
  inherited::InitLinks();
  taBase::Own(step_prog, this);
  if(prog_lib.not_init) {
    taBase::Ref(prog_lib);
    prog_lib.FindPrograms();
  }
}

void Program_Group::CutLinks() {
  step_prog.CutLinks();
  inherited::CutLinks();
}

void Program_Group::Copy_(const Program_Group& cp) {
  desc = cp.desc;
  if(cp.step_prog)
    step_prog = FindName(cp.step_prog->name);
}

void Program_Group::SaveToProgLib(Program::ProgLibs library) {
  String path = Program::GetProgLibPath(library);
  String fname = path + "/" + name + ".progp";
  int acc = access(fname, F_OK);
  if (acc == 0) {
    int chs = taMisc::Choice("Program library file: " + fname + " already exists: Overwrite?",
			     "Ok", "Cancel");
    if(chs == 1) return;
  }
  String tmp_last_dir = taFiler::last_dir;
  SaveAs(fname);
  taFiler::last_dir = tmp_last_dir;
  Program_Group::prog_lib.FindPrograms();
}

void Program_Group::SetProgsDirty() {
//WARNING: this will cause us to also receive setDirty for each prog call
  taLeafItr itr;
  Program* prog;
  FOR_ITR_EL(Program, prog, this->, itr) {
    prog->setDirty(true);
  }
}

ProgLib Program_Group::prog_lib;

taBase* Program_Group::NewFromLib(ProgLibEl* prog_type) {
  return prog_lib.NewProgram(prog_type, this);
}

taBase* Program_Group::NewFromLibByName(const String& prog_nm) {
  return prog_lib.NewProgramFmName(prog_nm, this);
}

void Program_Group::LoadFromProgLib(ProgLibEl* prog_type) {
  if(!prog_type) return;
  taLeafItr itr;
  Program* prog;
  FOR_ITR_EL(Program, prog, this->, itr) {
    prog->Reset();
  }
  prog_type->LoadProgramGroup(this);
}

//////////////////////////
//  Program_List	//
//////////////////////////

void Program_List::Initialize() {
  SetBaseType(&TA_Program);
}


//////////////////
//  ProgLib	//
//////////////////

void ProgLibEl::Initialize() {
  is_group = false;
}

taBase* ProgLibEl::NewProgram(Program_Group* new_owner) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(is_group) {
    Program_Group* pg = (Program_Group*)new_owner->NewGp(1);
    String tmp_last_dir = taFiler::last_dir;
    pg->Load(path);
    taFiler::last_dir = tmp_last_dir;
    return pg;
  }

  Program* pg = new_owner->NewEl(1, &TA_Program);
  String tmp_last_dir = taFiler::last_dir;
  pg->Load(path);
  taFiler::last_dir = tmp_last_dir;
  return pg;
}

bool ProgLibEl::LoadProgram(Program* prog) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(is_group) {
    taMisc::Error("ProgLibEl::LoadProgram -- cannot load a program group file into a single program!");
    return false;
  }
  String tmp_last_dir = taFiler::last_dir;
  prog->Load(path);
  taFiler::last_dir = tmp_last_dir;
  return true;
}

bool ProgLibEl::LoadProgramGroup(Program_Group* prog_gp) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(!is_group) {
    taMisc::Error("ProgLibEl::LoadProgram -- cannot load a single program file into a program group!");
    return false;
  }
  String tmp_last_dir = taFiler::last_dir;
  prog_gp->Load(path);
  taFiler::last_dir = tmp_last_dir;
  return true;
}

bool ProgLibEl::ParseProgFile(const String& fnm, const String& path) {
  filename = fnm;
  if(filename.contains(".progp"))
    is_group = true;
  else
    is_group = false;
  URL = "file:" + path + "/" + filename;
  String openfnm = path + "/" + filename;
  fstream strm;
  strm.open(openfnm, ios::in);
  if(strm.bad() || strm.eof()) {
    taMisc::Error("ProgLibEl::ParseProgFile: could not open file name:", openfnm);
    return false;
  }
  bool rval = false;
  int c = taMisc::read_till_rb_or_semi(strm); // skips over entire path header!
  while((c != EOF) && !strm.eof() && !strm.bad()) {
    c = taMisc::read_till_eol(strm); // skip next line
    if(c == EOF) break;
    if(taMisc::LexBuf.contains("name=")) {
      name = taMisc::LexBuf.after("name=");
      name.gsub("\"", "");
      if(name.lastchar() == ';') name = name.before(';');
    }
    if(taMisc::LexBuf.contains("desc=")) {
      desc = taMisc::LexBuf.after("desc=");
      desc.gsub("\"", "");
      if(desc.lastchar() == ';') desc = desc.before(';');
      rval = true;
      break;
    }
  }
  strm.close();
  return rval;
}

void ProgLibEl_List::Initialize() {
}

void ProgLib::Initialize() {
  not_init = true;
}

void ProgLib::FindPrograms() {
  Reset();			// clear existing
  for(int pi=0; pi< taMisc::prog_lib_paths.size; pi++) {
    NameVar pathvar = taMisc::prog_lib_paths[pi];
    String path = pathvar.value.toString();
    String lib_name = pathvar.name;
    QDir dir(path);
    QStringList files = dir.entryList();
    for(int i=0;i<files.size();i++) {
      String fl = files[i];
      if(!fl.contains(".prog")) continue;
      ProgLibEl* pe = new ProgLibEl;
      pe->lib_name = lib_name;
      if(pe->ParseProgFile(fl, path))
	Add(pe);
      else
	delete pe;
    }
  }
  not_init = false;
}

taBase* ProgLib::NewProgram(ProgLibEl* prog_type, Program_Group* new_owner) {
  if(prog_type == NULL) return NULL;
  return prog_type->NewProgram(new_owner);
}

taBase* ProgLib::NewProgramFmName(const String& prog_nm, Program_Group* new_owner) {
  return NewProgram(FindName(prog_nm), new_owner);
}
