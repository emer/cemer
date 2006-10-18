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

#include "program.h"

#include "css_machine.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "ta_css.h"

#include <QCoreApplication>
#ifdef TA_GUI
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
}

void ProgVar::UpdateAfterEdit() {
  if (!cssMisc::IsNameValid(name)) {
    taMisc::Error("'", name, "' is not a valid name in css scripts; must be alphanums or underscores");
//TODO: should revert
  }
  inherited::UpdateAfterEdit();
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

bool ProgVar::Dump_QuerySaveMember(MemberDef* md) {
  if (md->name == "int_val")
    return ((var_type == T_Int) || (var_type == T_HardEnum));
  else if (md->name == "real_val")
    return (var_type == T_Real);
  else if (md->name == "string_val")
    return (var_type == T_String);
  else if (md->name == "bool_val")
    return (var_type == T_Bool);
  else if ((md->name == "object_type") || (md->name == "object_val"))
    return (var_type == T_Object);
  else if (md->name == "hard_enum_type")
    return (var_type == T_HardEnum);
  else if (md->name == "dyn_enum_val")
    return (var_type == T_DynEnum);
  else 
    return inherited::Dump_QuerySaveMember(md);
}

const String ProgVar::GenCss(bool is_arg) {
  return is_arg ? GenCssArg_impl() : GenCssVar_impl() ;
} 

const String ProgVar::GenCssType() {
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
}

const String ProgVar::GenCssInitVal() {
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
}

// note: *never* initialize variables because they are cptrs to actual current
// value in object..
const String ProgVar::GenCssArg_impl() {
  String rval(0, 80, '\0'); //note: buffer will extend if needed
  rval += GenCssType() + " ";
  rval += name;
  return rval;
}

const String ProgVar::GenCssVar_impl() {
  STRING_BUF(rval, 80); //note: buffer will extend if needed
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

void ProgVar_List::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  // if we are in a prog, dirty prog
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    prog->setDirty(true);
  } else {
  // if we are in a group, dirty all progs
    Program_Group* grp = GET_MY_OWNER(Program_Group);
    if (grp)
      grp->SetProgsDirty();
  }
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

//////////////////////////
//   ProgArg_List	//
//////////////////////////

void ProgArg_List::Initialize() {
  SetBaseType(&TA_ProgArg);
}

void ProgArg_List::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    prog->setDirty(true);
  }
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
      Remove(i);
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
      Move(i, ti);
    }
  }
}


//////////////////////////
//  ProgEl		//
//////////////////////////

void ProgEl::Initialize() {
  off = false;
}

void ProgEl::Destroy() {
}

void ProgEl::Copy_(const ProgEl& cp) {
  desc = cp.desc;
  off = cp.off;
}

bool ProgEl::CheckConfig_impl(bool quiet) {
  if (off) return true;
  return inherited::CheckConfig_impl(quiet);
}

void ProgEl::ChildUpdateAfterEdit(TAPtr child, bool& handled) {
  inherited::ChildUpdateAfterEdit(child, handled);
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    prog->setDirty(true);
  }
}

void ProgEl::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if (!(dcr == DCR_ITEM_UPDATED)) return;
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    prog->setDirty(true);
  }
}

const String ProgEl::GenCss(int indent_level) {
  if(off) return "";
  STRING_BUF(rval, 120); // grows if needed, but may be good for many cases
  if (!desc.empty())
    rval.cat(cssMisc::Indent(indent_level)).cat("//").cat(desc).cat("\n");
  rval += GenCssPre_impl(indent_level);
  rval += GenCssBody_impl(indent_level);
  rval += GenCssPost_impl(indent_level);
  return rval;
}

void ProgEl::PreGen(int& item_id) {
  if(off) return;
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

void ProgEl_List::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if ((dcr < DCR_LIST_ITEM_MIN) || (dcr > DCR_LIST_ITEM_MAX)) return;
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    prog->setDirty(true);
  }
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
//  ProgList		//
//////////////////////////

void ProgList::Initialize() {
}

void ProgList::InitLinks() {
  inherited::InitLinks();
  taBase::Own(prog_code, this);
}

void ProgList::CutLinks() {
  prog_code.CutLinks();
  inherited::CutLinks();
}

void ProgList::Copy_(const ProgList& cp) {
  prog_code = cp.prog_code; //TODO: need to make sure this is a value copy
}

void ProgList::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  prog_code.CheckConfig(quiet, rval);
}

const String ProgList::GenCssBody_impl(int indent_level) {
  return prog_code.GenCss(indent_level);
}

String ProgList::GetDisplayName() const {
  return "ProgList (" + String(prog_code.size) + " items)";
}

void ProgList::PreGenChildren_impl(int& item_id) {
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

void ProgVars::InitLinks() {
  inherited::InitLinks();
  taBase::Own(script_vars, this);
}

void ProgVars::CutLinks() {
  script_vars.CutLinks();
  inherited::CutLinks();
}

void ProgVars::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  script_vars.CheckConfig(quiet, rval);
}

const String ProgVars::GenCssBody_impl(int indent_level) {
  return script_vars.GenCss(indent_level);
}

String ProgVars::GetDisplayName() const {
  STRING_BUF(rval, 30);
  rval += "ProgVars (";
  rval += String(script_vars.size);
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

void UserScript::Copy_(const UserScript& cp) {
  user_script = cp.user_script;
}

const String UserScript::GenCssBody_impl(int indent_level) {
  String rval(cssMisc::IndentLines(user_script, indent_level));
  // strip trailing non-newline ws, and make sure there is a trailing newline
//TEMP  rval = trimr(rval);
  if (rval.lastchar() != '\n')
    rval += '\n';
  return rval;
}

String UserScript::GetDisplayName() const {
  // use first line, if any
  String rval = user_script.before('\n');
  if (rval.empty()) {
    rval = user_script;
    if (rval.empty())
      rval = "(empty)";
  }
  if(rval.length() > 25) rval = rval.before(25) + "...";
  return rval;
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
  String full_fnm = taMisc::FindFileInclude(fnm);
  fstream strm;
  strm.open(full_fnm, ios::in);
  ImportFromFile(strm);
  strm.close();
}

void UserScript::ExportToFile(ostream& strm) {
  strm << user_script;
}

void UserScript::ExportToFileName(const String& fnm) {
  String full_fnm = taMisc::FindFileInclude(fnm);
  fstream strm;
  strm.open(full_fnm, ios::out);
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
  return "while (" + loop_test + ") ...";
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
//  IfElse		//
//////////////////////////

void IfElse::Initialize() {
  //  condition = "true";
}

void IfElse::InitLinks() {
  inherited::InitLinks();
  taBase::Own(true_code, this);
  taBase::Own(false_code, this);
}

void IfElse::CutLinks() {
  false_code.CutLinks();
  true_code.CutLinks();
  inherited::CutLinks();
}

void IfElse::Copy_(const IfElse& cp) {
  condition = cp.condition;
  true_code = cp.true_code;
  false_code = cp.false_code;
}

void IfElse::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if (condition.empty()) {
    if(!quiet) taMisc::CheckError("Error in IfElse in program:", program()->name, "condition expression is empty");
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
//    MethodCall	//
//////////////////////////

void MethodCall::Initialize() {
  method = NULL;
  object_type = &TA_taBase; // placeholder
  lst_script_obj = NULL;
  lst_method = NULL;
}

void MethodCall::InitLinks() {
  inherited::InitLinks();
  taBase::Own(script_obj, this);
  taBase::Own(args, this);
}

void MethodCall::CutLinks() {
  args.CutLinks();
  script_obj.CutLinks();
  method = NULL;
  lst_script_obj = NULL;
  lst_method = NULL;
  inherited::CutLinks();
}

void MethodCall::Copy_(const MethodCall& cp) {
  script_obj = cp.script_obj;
  method = cp.method;
  args = cp.args;
  lst_script_obj = cp.lst_script_obj;
  lst_method = cp.lst_method;
}

void MethodCall::UpdateAfterEdit() {
  if(script_obj)
    object_type = script_obj->act_object_type();
  else object_type = &TA_taBase; // placeholder

  if(!taMisc::is_loading)
    CheckUpdateArgs();

  lst_script_obj = script_obj; //note: don't ref
  lst_method = method;
  inherited::UpdateAfterEdit();
}

void MethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if(prg) prognm = prg->name;
  if(!script_obj) {
    if(!quiet) taMisc::CheckError("Error in MethodCall in program:", prognm, "script_obj is NULL");
    rval = false;
  }
  if(!method) {
    if(!quiet) taMisc::CheckError("Error in MethodCall in program:", prognm, "method is NULL");
    rval = false;
  }
}

const String MethodCall::GenCssBody_impl(int indent_level) {
  STRING_BUF(rval, 80); // more allocated if needed
  rval += cssMisc::Indent(indent_level);
  if (!(script_obj && method)) {
    rval += "//WARNING: MethodCall not generated here -- obj or method not specified\n";
   return rval;
  }
  
  if (!result_var.empty())
    rval += result_var + " = ";
  rval += script_obj->name;
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
  if (!script_obj || !method)
    return "(object or method not selected)";
  
  STRING_BUF(rval, 40); // more allocated if needed
  rval += script_obj->name;
  rval += "->";
  rval += method->name;
  rval += "(";
  for(int i=0;i<args.size;i++) {
    rval += args.labels[i] + "=" + args[i];
    if(i < args.size-1)
      rval += ", ";
  }
  rval += ")";
  return rval;
}

void MethodCall::CheckUpdateArgs(bool force) {
  if ((method == lst_method) && (!force)) return;
  args.Reset(); args.labels.Reset();
  if (!method) return;
  MethodDef* md = method; //cache
  String arg_nm;
  for (int i = 0; i < md->arg_types.size; ++i) {
    TypeDef* arg_typ = md->arg_types.FastEl(i);
    arg_nm = arg_typ->Get_C_Name() + " " + md->arg_names[i] ;
    args.labels.Add(arg_nm);
    // preseed the arg value with the default
    args.Add(md->arg_defs.SafeEl(i)); 
  }
  args.UpdateAfterEdit(); // note: arrays don't have very good data notify
  lst_method = method;
}

//////////////////////////
//    StaticMethodCall	//
//////////////////////////

void StaticMethodCall::Initialize() {
  method = NULL;
  min_type = &TA_taBase;
  object_type = &TA_taBase;
  lst_method = NULL;
}

void StaticMethodCall::InitLinks() {
  inherited::InitLinks();
  taBase::Own(args, this);
}

void StaticMethodCall::CutLinks() {
  args.CutLinks();
  method = NULL;
  lst_method = NULL;
  inherited::CutLinks();
}

void StaticMethodCall::Copy_(const StaticMethodCall& cp) {
  min_type = cp.min_type;
  object_type = cp.object_type;
  method = cp.method;
  args = cp.args;
  lst_method = cp.lst_method;
}

void StaticMethodCall::UpdateAfterEdit() {
  if(!taMisc::is_loading)
    CheckUpdateArgs();

  lst_method = method;
  inherited::UpdateAfterEdit();
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
  STRING_BUF(rval, 80); // more allocated if needed
  rval += cssMisc::Indent(indent_level);
  if (!method) {
    rval += "//WARNING: StaticMethodCall not generated here -- obj or method not specified\n";
    return rval;
  }
  
  if (!result_var.empty())
    rval += result_var + " = ";
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
  
  STRING_BUF(rval, 40); // more allocated if needed
  rval += object_type->name;
  rval += "::";
  rval += method->name;
  rval += "(";
  for(int i=0;i<args.size;i++) {
    rval += args.labels[i] + "=" + args[i];
    if(i < args.size-1)
      rval += ", ";
  }
  rval += ")";
  return rval;
}

void StaticMethodCall::CheckUpdateArgs(bool force) {
  if ((method == lst_method) && (!force)) return;
  args.Reset(); args.labels.Reset();
  if (!method) return;
  MethodDef* md = method; //cache
  String arg_nm;
  for (int i = 0; i < md->arg_types.size; ++i) {
    TypeDef* arg_typ = md->arg_types.FastEl(i);
    arg_nm = arg_typ->Get_C_Name() + " " + md->arg_names[i] ;
    args.labels.Add(arg_nm);
    // preseed the arg value with the default
    args.Add(md->arg_defs.SafeEl(i)); 
  }
  args.UpdateAfterEdit(); // note: arrays don't have very good data notify
  lst_method = method;
}

void MathCall::Initialize() {
  min_type = &TA_taMath;
  object_type = &TA_taMath;
}

//////////////////////////
//   ProgramCall	//
//////////////////////////


void ProgramCall::Initialize() {
  old_target = NULL;
  call_init = false;
}

void ProgramCall::InitLinks() {
  inherited::InitLinks();
  taBase::Own(prog_args, this);
  taBase::Own(target, this);
}

void ProgramCall::CutLinks() {
  target.CutLinks();
  prog_args.CutLinks();
  old_target = NULL;
  inherited::CutLinks();
}

void ProgramCall::Copy_(const ProgramCall& cp) {
  target = cp.target;
  call_init = cp.call_init;
  prog_args = cp.prog_args;
}

void ProgramCall::UpdateAfterEdit() {
  if (target.ptr() != old_target) {
    old_target = target.ptr(); // note: we don't ref, because we just need to check ptr addr
    UpdateGlobalArgs();
  }
  inherited::UpdateAfterEdit();
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
    taMisc::Error("Program target is NULL in ProgramCall:",
		  desc, "in program:", program()->name);
  }
  if(!target->CompileScript()) {
    taMisc::Error("Program target script did not compile correctly in ProgramCall:",
		  desc, "in program:", program()->name);
  }
  return target.ptr();
}

const String ProgramCall::GenCssPre_impl(int indent_level) {
  STRING_BUF(rval, 50);
  rval = cssMisc::Indent(indent_level);
  rval += "{ // call program: "; 
  if (target)
    rval += target->name;
  rval += "\n";
  return rval;
}

const String ProgramCall::GenCssBody_impl(int indent_level) {
  if (!target) return _nilString;
  STRING_BUF(rval, 250);
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
    rval += "target->SetGlobalVar(\"" + prg_var->name + "\", "
      + ths_arg->value + ");\n";
  }
  if (set_one) {
    rval += cssMisc::Indent(indent_level+1);
    rval += "target->DataChanged(DCR_ITEM_UPDATED);\n";
  }
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
  if (target)
    rval += target->GetName();
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

void ProgramCall::UpdateGlobalArgs() {
  if (!target) return; // just leave existing stuff for now
  prog_args.ConformToTarget(target->args);
}

//////////////////////////
//  Program		//
//////////////////////////

Program* Program::MakeTemplate() {
//TODO: this will probably get nuked and replaced with a generic maker on .root
  Program* prog = new Program;
  {ProgVar* o = new ProgVar; o->SetName("NewProgVar"); prog->vars.Add(o);}
  //note: prog args go into a ProgramCall etc., so we just add the tmpl to the objects
  {ProgArg* o = new ProgArg; o->SetName("NewProgArg"); prog->objs.Add(o);}
  //note: put in .init since that will get searched first
  {ProgVars* o = new ProgVars; o->SetName("NewProgVars"); prog->init_code.Add(o);}
  {ProgList* o = new ProgList; o->SetName("NewProgList"); prog->init_code.Add(o);}
  {UserScript* o = new UserScript; o->SetName("NewUserScript"); prog->init_code.Add(o);}
  {ForLoop* o = new ForLoop; o->SetName("NewForLoop"); prog->init_code.Add(o);}
  {DoLoop* o = new DoLoop; o->SetName("NewDoLoop"); prog->init_code.Add(o);}
  {WhileLoop* o = new WhileLoop; o->SetName("NewWhileLoop"); prog->init_code.Add(o);}
  {IfElse* o = new IfElse; o->SetName("NewIfElse"); prog->init_code.Add(o);}
  {IfContinue* o = new IfContinue; o->SetName("NewIfContinue"); prog->init_code.Add(o);}
  {IfBreak* o = new IfBreak; o->SetName("NewIfBreak"); prog->init_code.Add(o);}
  {MethodCall* o = new MethodCall; o->SetName("NewMethodCall"); prog->init_code.Add(o);}
  {MathCall* o = new MathCall; o->SetName("NewMathCall"); prog->init_code.Add(o);}
  {ProgramCall* o = new ProgramCall; o->SetName("NewProgramCall"); prog->init_code.Add(o);}
  return prog;
}
  
bool Program::stop_req = false;
bool Program::step_mode = false;

void Program::Initialize() {
  run_state = NOT_INIT;
  flags = PF_NONE;
  objs.SetBaseType(&TA_taOBase);
  ret_val = 0;
  m_dirty = true; 
  prog_gp = NULL;
}

void Program::Destroy()	{ 
  CutLinks();
}

void Program::InitLinks() {
  inherited::InitLinks();
  taBase::Own(objs, this);
  taBase::Own(args, this);
  taBase::Own(vars, this);
  taBase::Own(init_code, this);
  taBase::Own(prog_code, this);
  taBase::Own(sub_progs, this);
  prog_gp = GET_MY_OWNER(Program_Group);
}

void Program::CutLinks() {
  sub_progs.CutLinks();
  prog_code.CutLinks();
  init_code.CutLinks();
  vars.CutLinks();
  args.CutLinks();
  objs.CutLinks();
  prog_gp = NULL;
  inherited::CutLinks();
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
  init_code = cp.init_code;
  prog_code = cp.prog_code;
  ret_val = 0; // redo
  m_dirty = true; // require rebuild/refetch
  m_scriptCache = "";
  sub_progs.RemoveAll();
  UpdatePointers_NewPar((taBase*)&cp, this); // update any pointers within this guy
}

void Program::UpdateAfterEdit() {
  //WARNING: the running css prog calls this on any changes to our vars,
  // such as ret_val -- therefore, DO NOT do things here that are incompatible
  // with the runtime, in particular, do NOT invalidate the following state flags:
  //   m_dirty, script_compiled
  
  //TODO: the following *do* affect generated script, so we should probably call
  // setDirty(true) if not running, and these changed:
  // name, (more TBD...)
  GetVarsForObjs();
  inherited::UpdateAfterEdit();
}

void Program::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  args.CheckConfig(quiet, rval);
  vars.CheckConfig(quiet, rval);
  init_code.CheckConfig(quiet, rval);
  prog_code.CheckConfig(quiet, rval);
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
  Run_impl();
  taMisc::DoneBusy();
  if(ret_val != RV_OK) {
    String err_str = GetTypeDef()->GetEnumString("ReturnVal", ret_val);
    if(ret_val == RV_COMPILE_ERR) {
      err_str += " a program did not compile correctly: check the console for error messages";
    }
    taMisc::Error("Error: The Program did not run -- ret_val=", err_str);
  }
  script->Restart();		// restart script at beginning if run again
  setRunState(DONE);
} 

bool Program::PreCompileScript_impl() {
  // as noted in abstractscriptbase: you must call this first to reset the script
  // because if you mess with the existing variables in prog_vars prior to 
  // resetting the script, it will get all messed up.  vars on this space are referred
  // to by a pointer to the space and an index off of it, which is important for autos
  // but actually not for these guys (but they are/were that way anyway).
  if(!AbstractScriptBase::PreCompileScript_impl()) return false;
  GetVarsForObjs();
  UpdateProgVars();
// BA 10/18/06 -- should not have CheckConfig w/gui buried deep inside worker-bee
// routines like this!!! Also, this may not even be necessary at all
// since we should now be doing CheckConfig before running...
//  if(!CheckConfig(false)) return false; // not quiet
  return true;
}

void Program::setRunState(RunState value) {
  if (run_state == value) return;
  run_state = value;
  // todo: this might be adding unnec overhead:?
  DataChanged(DCR_ITEM_UPDATED);
}

int Program::Run_impl() {
  ret_val = RV_OK;
  if(!CompileScript())
    ret_val = RV_COMPILE_ERR;
  else
    script->Run();
  //note: shared var state likely changed, so update gui
  DataChanged(DCR_ITEM_UPDATED);
  return ret_val;
}

int Program::Cont_impl() {
  ret_val = RV_OK;
  if(!CompileScript())
    ret_val = RV_COMPILE_ERR;
  else
    script->Cont();
  //note: shared var state likely changed, so update gui
  // DataChanged(DCR_ITEM_UPDATED);
  script_compiled = true; // override any run-generated changes!!
  return ret_val;
}

void Program::Run() {
  stop_req = false;
  step_mode = false;
  taMisc::Busy();
  setRunState(RUN);
  Cont_impl();
  taMisc::DoneBusy();
  if (ret_val != 0) //TODO: use enums and sensible output string
    QMessageBox::warning(NULL, QString("Operation Failed"),
      String(
      "The Program did not run -- ret_val=").cat(String(ret_val)), 
      QMessageBox::Ok, QMessageBox::NoButton);
  // unless we were stopped, we are done
  if(stop_req) {
    setRunState(STOP);
  }
  else {
    script->Restart();
    setRunState(DONE);
  }
  stop_req = false;
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
}

void Program::Stop() {
  stop_req = true;
}

void Program::Stop_impl() {
  script->Stop();
  setRunState(STOP);
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
  CompileScript();
}

void Program::CmdShell() {
  CmdShellScript();
}

void Program::ExitShell() {
  ExitShellScript();
}

void Program::ScriptCompiled() {
  AbstractScriptBase::ScriptCompiled();
  m_dirty = false;
  script_compiled = true;
  ret_val = 0;
  DataChanged(DCR_ITEM_UPDATED);
}

void Program::setDirty(bool value) {
  if (m_dirty == value) return;
  m_dirty = value;
  script_compiled = false; // have to assume user changed something
  sub_progs.RemoveAll(); // will need to re-enumerate
  DirtyChanged_impl();
  DataChanged(DCR_ITEM_UPDATED);
}

bool Program::SetGlobalVar(const String& nm, const Variant& value) {
  cssElPtr& el_ptr = script->prog_vars.FindName(nm);
  if (el_ptr == cssMisc::VoidElPtr) return false;
  cssEl* el = el_ptr.El();
//   if(el->GetType() == cssEl::T_Variant) {
//     ((cssVariant*)el)->val.setVariantData(value); // only copy data, preserve type!
//   }
  if((el->GetType() == cssEl::T_C_Ptr) && (el->GetPtrType() == cssEl::T_Variant)) {
    ((Variant*)((cssCPtr_Variant*)el)->GetNonNullVoidPtr())->setVariantData(value);
  }
  else {
    *el = value;
  }
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
    
    // __Init() routine, for our own els, and calls to subprog Init()
    m_scriptCache += "void __Init() {\n";
    m_scriptCache += init_code.GenCss(1); // ok if empty, returns nothing
    if (sub_progs.size > 0) {
      if (init_code.size >0) m_scriptCache += "\n";
      m_scriptCache += "  // init any subprogs that could be called from this one\n";
      m_scriptCache += "  { Program* target;\n";
      // todo: this needs to be a list of ProgramCall's, not the actual prog itself!
      for (int i = 0; i < sub_progs.size; ++i) {
        ProgramCall* sp = (ProgramCall*)sub_progs.FastEl(i);
        m_scriptCache += "    if (ret_val != Program::RV_OK) return; // checks previous\n"; 
        m_scriptCache += "    target = this" + sp->GetPath(NULL, this) + "->GetTarget();\n";
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
    m_dirty = false;
    
    m_scriptCache += "ret_val = Program::RV_OK; // set elsewise on failure\n";
    m_scriptCache += "if (run_state == Program::INIT) {\n";
    m_scriptCache += "  __Init();\n";
    m_scriptCache += "} else {\n";
    m_scriptCache += "  __Prog();\n";
    m_scriptCache += "}\n";
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
  
  // add new (with unique names) from our groups, starting at most inner
  Program_Group* grp = prog_gp;
  while (grp) {
    for (int i = 0; i < grp->global_vars.size; ++i) {
      ProgVar* sv = grp->global_vars.FastEl(i);
      // for group vars, we only add with unique names 
      if (script->prog_vars.IndexOfName(sv->GetName()) >= 0) continue;
      el = sv->NewCssEl();
      script->prog_vars.Push(el); //refs
      el = sv->NewCssType();	// for dynenums
      if(el != NULL)
	script->prog_types.Push(el);
    } 
    grp = (Program_Group*)grp->GetOwner(&TA_Program_Group);
  }
}

void Program::GetVarsForObjs() {
  for(int i = 0; i < objs.size; ++i) {
    taBase* obj = objs[i];
    String nm = obj->GetName();
    if(nm.empty()) continue;
    ProgVar* var = vars.FindName(nm);
    if(var) {
      if((var->var_type != ProgVar::T_Object) || (var->object_val != obj)) {
	taMisc::Error("Program error: variable named:", nm,
		      "exists, but does not refer to object in objs list -- rename either to avoid conflict");
      }
      else {
	var->objs_ptr = true;	// make sure
	var->object_type = obj->GetTypeDef();
      }
    }
    else {
      bool found_it = false;
      for(int j=0;j<vars.size; j++) {
	ProgVar* tv = vars[j];
	if((tv->var_type == ProgVar::T_Object) && (tv->object_val == obj)) {
	  found_it = true;
	  tv->name = nm;	// update the name
	  tv->objs_ptr = true;	// make sure
	  tv->object_type = obj->GetTypeDef();
	  break;
	}
      }
      if(!found_it) {
	var = (ProgVar*)vars.New(1, &TA_ProgVar);
	var->name = nm;
	var->var_type = ProgVar::T_Object;
	var->object_val = obj;
	var->objs_ptr = true;
	var->object_type = obj->GetTypeDef();
      }
    }
  }
  // now cleanup any orphaned 
  for(int i = vars.size-1; i >= 0; --i) {
    ProgVar* var = vars[i];
    if(!var->objs_ptr) continue;
    taBase* obj = objs.FindName(var->name);
    if(obj == NULL)
      vars.Remove(i);		// get rid of it
  }
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
  taBase::Own(global_vars, this);
  if(prog_lib.not_init) {
    taBase::Ref(prog_lib);
    prog_lib.paths.Add("../../prog_lib"); // todo: hack for testing from pdp_lib location!
    prog_lib.FindPrograms();
  }
}

void Program_Group::CutLinks() {
  step_prog.CutLinks();
  global_vars.CutLinks();
  inherited::CutLinks();
}

void Program_Group::Copy_(const Program_Group& cp) {
  desc = cp.desc;
  global_vars = cp.global_vars;
  if(cp.step_prog)
    step_prog = FindName(cp.step_prog->name);
}

void Program_Group::SetProgsDirty() {
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
    fstream strm;
    strm.open(path, ios::in);
    pg->Load(strm);
    strm.close();
    return pg;
  }

  Program* pg = new_owner->NewEl(1, &TA_Program);
  fstream strm;
  strm.open(path, ios::in);
  pg->Load(strm);
  strm.close();
  return pg;
}

#include "css_misc_funs.h"

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

#include <QDir>

void ProgLib::FindPrograms() {
  Reset();			// clear existing
  for(int pi=0; pi< paths.size; pi++) {
    String path = paths[pi];
    QDir dir(path);
    QStringList files = dir.entryList();
    for(int i=0;i<files.size();i++) {
      String fl = files[i];
      if(!fl.contains(".prog")) continue;
      ProgLibEl* pe = new ProgLibEl;
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
