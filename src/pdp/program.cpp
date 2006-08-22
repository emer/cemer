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
  object_val = NULL;
  hard_enum_type = NULL;
}

void ProgVar::Destroy() {
  CutLinks();
}

void ProgVar::InitLinks() {
  taBase::Own(&dyn_enum_val, this);
  inherited::InitLinks();
}

void ProgVar::CutLinks() {
  taBase::DelPointer((taBase**)&object_val);
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
  taBase::SetPointer((taBase**)&object_val, cp.object_val);
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

TypeDef* ProgVar::act_object_type() const {
  TypeDef* rval = &TA_taBase; // the min return value
  if (object_type)
    if (object_val) rval = object_val->GetTypeDef();
    else rval = object_type;
  return rval;
}

void ProgVar::Cleanup() {
  if (!((var_type == T_Int) || (var_type == T_HardEnum)))
    int_val = 0;
  if (var_type != T_Real)  real_val = 0.0;
  if (var_type != T_String)  string_val = _nilString;
  if (var_type != T_Bool)  bool_val = false;
  if (var_type != T_Object) {
    //note: its ok to leave whatever type is there
    taBase::DelPointer((taBase**)&object_val);
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
    return new cssCPtr_int(&int_val, 1, name);
  case T_Real:
    return new cssCPtr_double(&real_val, 1, name);
  case T_String:
    return new cssCPtr_String(&string_val, 1, name);
  case T_Bool:
    return new cssCPtr_bool(&bool_val, 1, name);
  case T_Object: 
    if(object_val)
      return new cssTA_Base(&object_val, 2, object_val->GetTypeDef(), name);
    else
      return new cssTA_Base(&object_val, 2, object_type, name);
  case T_HardEnum:
    return new cssCPtr_enum(&int_val, 1, name, hard_enum_type);
  case T_DynEnum:
    return new cssCPtr_DynEnum(&dyn_enum_val, 1, name);
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
  //TODO: need to look at both the ProgVar type, as well as value type, to get right value
//   value = cp.value.toCssLiteral();
} 
/*
void ProgVar::Freshen(const ProgVar& cp) {
  value = cp.value;
} 
void EnumProgVar::Freshen(const ProgVar& cp_) {
  inherited::Freshen(cp_);
  if (cp_.GetTypeDef()->InheritsFrom(&TA_EnumProgVar)) {
    const EnumProgVar& cp = (const EnumProgVar&)(cp_);
    enum_type = cp.enum_type;
    init = cp.init;
  }
} 
void ObjectProgVar::Freshen(const ProgVar& cp_) {
  inherited::Freshen(cp_);
  if (cp_.GetTypeDef()->InheritsFrom(&TA_ObjectProgVar)) {
    const ObjectProgVar& cp = (const ObjectProgVar&)(cp_);
    val_type = cp.val_type;
    make_new = cp.make_new;
  }
} */



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

void ProgEl::Copy_(const ProgEl& cp) {
  desc = cp.desc;
  off = cp.off;
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

String ProgEl::GetColText(int col, int /*itm_idx*/) {
  switch (col) {
  case 0: return GetTypeDef()->name;
  case 1: return GetDisplayName();
  default: return String("");
  }
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

String ProgEl_List::GetColHeading(int col) {
  static String col0("El Type");
  static String col1("El Description");
  switch (col) {
  case 0: return col0;
  case 1: return col1;
  default: return _nilString;
  }
}

void ProgEl_List::PreGen(int& item_id) {
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    el->PreGen(item_id);
  }
}

//////////////////////////
//  UserScriptEl	//
//////////////////////////

void UserScriptEl::Initialize() {
  static String _def_user_script("// TODO: Add your CSS script code here.\n");
  user_script = _def_user_script;
}

void UserScriptEl::Copy_(const UserScriptEl& cp) {
  user_script = cp.user_script;
}

const String UserScriptEl::GenCssBody_impl(int indent_level) {
  String rval(cssMisc::IndentLines(user_script, indent_level));
  // strip trailing non-newline ws, and make sure there is a trailing newline
//TEMP  rval = trimr(rval);
  if (rval.lastchar() != '\n')
    rval += '\n';
  return rval;
}

String UserScriptEl::GetDisplayName() const {
  if (desc.empty()) {
    // use first line, if any
    String rval = user_script.before('\n');
    if (rval.empty()) {
      rval = user_script;
      if (rval.empty())
        rval = "(empty)";
    }
    return rval;
  } else return desc;
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
//  ProgList		//
//////////////////////////

void ProgList::Initialize() {
}

void ProgList::InitLinks() {
  inherited::InitLinks();
  taBase::Own(prog_els, this);
}

void ProgList::CutLinks() {
  prog_els.CutLinks();
  inherited::CutLinks();
}

void ProgList::Copy_(const ProgList& cp) {
  prog_els = cp.prog_els; //TODO: need to make sure this is a value copy
}

const String ProgList::GenCssBody_impl(int indent_level) {
  return prog_els.GenCss(indent_level);
}

String ProgList::GetDisplayName() const {
  return "ProgList (" + String(prog_els.size) + " items)";
}

void ProgList::PreGenChildren_impl(int& item_id) {
  prog_els.PreGen(item_id);
}

//////////////////////////
//  LoopEl		//
//////////////////////////

void LoopEl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(loop_els, this);
}

void LoopEl::CutLinks() {
  loop_els.CutLinks();
  inherited::CutLinks();
}

void LoopEl::Copy_(const LoopEl& cp) {
  loop_els = cp.loop_els;
  loop_var_type = cp.loop_var_type;
  loop_var = cp.loop_var;
  init_val = cp.init_val;
}

const String LoopEl::GenCssPre_impl(int indent_level) {
  //NOTE: ForLoopEl replaces this, since it inits its var in the for()
  if (loop_var_type.empty()) return _nilString;
  String rval = cssMisc::Indent(indent_level);
  rval += "{" + loop_var_type + " " + loop_var;
  if (!init_val.empty())
    rval += " = " + init_val;
  rval += ";\n";
  return rval; 
}

const String LoopEl::GenCssBody_impl(int indent_level) {
  return loop_els.GenCss(indent_level + 1);
}

const String LoopEl::GenCssPost_impl(int indent_level) {
  if (loop_var_type.empty()) return _nilString;
  
  return cssMisc::Indent(indent_level) + "}\n";
}

String LoopEl::GetDisplayName() const {
  return loopHeader(true);
}

void LoopEl::PreGenChildren_impl(int& item_id) {
  loop_els.PreGen(item_id);
}


//////////////////////////
//  ForLoopEl		//
//////////////////////////

void ForLoopEl::Initialize() {
  // the following are just default examples for the user
  loop_var_type = "int";
  loop_var = "i";
  init_val = "0";
  loop_test = "i < 10";
  loop_iter = "i += 1";
}

void ForLoopEl::Copy_(const ForLoopEl& cp) {
  loop_test = cp.loop_test;
  loop_iter = cp.loop_iter;
}

const String ForLoopEl::GenCssPre_impl(int indent_level) {
//NOTE: we replace the default LoopEl routine
  String rval;
  if (!loop_var_type.empty()) {
    rval += cssMisc::Indent(indent_level);
    rval += "{" + loop_var_type + " " + loop_var + ";\n";
  }
  rval += cssMisc::Indent(indent_level);
  rval += loopHeader();
  rval += " {\n";
  return rval; 
}

const String ForLoopEl::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "}\n";
  rval += inherited::GenCssPost_impl(indent_level);
  return rval;
}

const String ForLoopEl::loopHeader(bool) const {
  STRING_BUF(rval, 60);
  rval += "for (" + loop_var + " = " + init_val + "; "
    + loop_test + "; " 
    + loop_iter + ")";
  return rval;
}

//////////////////////////
//  WhileLoopEl		//
//////////////////////////

const String WhileLoopEl::GenCssPre_impl(int indent_level) {
  String rval = inherited::GenCssPre_impl(indent_level);
  rval += cssMisc::Indent(indent_level);
  rval += loopHeader();
  rval += " {\n";
  return rval; 
}

const String WhileLoopEl::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "}\n";
  rval += inherited::GenCssPost_impl(indent_level);
  return rval;
}

const String WhileLoopEl::loopHeader(bool) const {
  STRING_BUF(rval, 60);
  rval += "while (";
  rval += loop_var;
  rval += + ")";
  return rval;
}

//////////////////////////
//  DoLoopEl		//
//////////////////////////

const String DoLoopEl::GenCssPre_impl(int indent_level) {
  String rval = inherited::GenCssPre_impl(indent_level);
  rval += cssMisc::Indent(indent_level);
  rval += "do {\n";
  return rval; 
}

const String DoLoopEl::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level);
  rval += "} while (" + loop_var + ");\n";
  rval += inherited::GenCssPost_impl(indent_level);
  return rval;
}

const String DoLoopEl::loopHeader(bool display) const {
  STRING_BUF(rval, 60);
  if (display)
    rval += "do ... while (" + loop_var + ")";
  else
    rval += "while (" + loop_var + ")";
  return rval;
}


//////////////////////////
//  CondEl		//
//////////////////////////

void CondEl::Initialize() {
  cond_test = "true";
}

void CondEl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(true_els, this);
  taBase::Own(false_els, this);
}

void CondEl::CutLinks() {
  false_els.CutLinks();
  true_els.CutLinks();
  inherited::CutLinks();
}

void CondEl::Copy_(const CondEl& cp) {
  cond_test = cp.cond_test;
  true_els = cp.true_els; //TODO: need to make sure this is a value copy
  false_els = cp.false_els; //TODO: need to make sure this is a value copy
}

const String CondEl::GenCssPre_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level);
  rval += "if (" + cond_test + ") {\n";
  return rval; 
}

const String CondEl::GenCssBody_impl(int indent_level) {
  String rval = true_els.GenCss(indent_level + 1);
  // don't gen 'else' portion unless there are els
  if (false_els.size > 0) {
    rval += cssMisc::Indent(indent_level) + "} else {\n";
    rval += false_els.GenCss(indent_level + 1);
  }
  return rval;
}

const String CondEl::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "}\n";
}

String CondEl::GetDisplayName() const {
  return "if (" + cond_test + ")";
}

void CondEl::PreGenChildren_impl(int& item_id) {
  true_els.PreGen(item_id);
  false_els.PreGen(item_id);
}


//////////////////////////
//  MethodSpec	//
//////////////////////////

void MethodSpec::Initialize() {
  script_obj = NULL;
  method = NULL;
  object_type = &TA_taBase; // placeholder
}

void MethodSpec::CutLinks() {
  taBase::DelPointer((taBase**)&script_obj);
  method = NULL;
  inherited::CutLinks();
}

void MethodSpec::Copy_(const MethodSpec& cp) {
  taBase::SetPointer((taBase**)&script_obj, cp.script_obj);
  method = cp.method;
}

void MethodSpec::UpdateAfterEdit() {
//TODO: maybe update owner MethodCallEl
  if (script_obj)
    object_type = script_obj->act_object_type();
  else object_type = &TA_taBase; // placeholder
  inherited::UpdateAfterEdit();
  if (taMisc::is_loading) return;
  MethodCallEl* own = GET_MY_OWNER(MethodCallEl);
  if (own) own->UpdateAfterEdit();
}


//////////////////////////
//  MethodCallEl	//
//////////////////////////

void MethodCallEl::Initialize() {
  lst_script_obj = NULL;
  lst_method = NULL;
}

void MethodCallEl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(method_spec, this);
  taBase::Own(args, this);
}

void MethodCallEl::CutLinks() {
  args.CutLinks();
  method_spec.CutLinks();
  lst_script_obj = NULL;
  lst_method = NULL;
  inherited::CutLinks();
}

void MethodCallEl::Copy_(const MethodCallEl& cp) {
  args = cp.args;
  method_spec = cp.method_spec;
  lst_script_obj = cp.lst_script_obj;
  lst_method = cp.lst_method;
}

void MethodCallEl::UpdateAfterEdit() {
  if (taMisc::is_loading) goto inh;
  CheckUpdateArgs();
  
inh:
  lst_script_obj = method_spec.script_obj; //note: don't ref
  lst_method = method_spec.method;
  inherited::UpdateAfterEdit();
}

const String MethodCallEl::GenCssBody_impl(int indent_level) {
  STRING_BUF(rval, 80); // more allocated if needed
  rval += cssMisc::Indent(indent_level);
  if (!(method_spec.script_obj && method_spec.method)) {
    rval += "//WARNING: MethodCallEl not generated here -- obj or method not specified\n";
   return rval;
  }
  
  if (!result_var.empty())
    rval += result_var + " = ";
  rval += method_spec.script_obj->name;
  rval += "->";
  rval += method_spec.method->name;
  rval += "(";
    for (int i = 0; i < args.size; ++ i) {
      if (i > 0) rval += ", ";
      rval += args[i];
    }
  rval += ");\n";
  
  return rval;
}

String MethodCallEl::GetDisplayName() const {
  if (!(method_spec.script_obj && method_spec.method))
    return "(object or method not selected)";
  
  STRING_BUF(rval, 40); // more allocated if needed
  rval += method_spec.script_obj->name;
  rval += "->";
  rval += method_spec.method->name;
  //TODO: nicer if this descriptor had the param names and/or types
  rval += "()";
  return rval;
}

void MethodCallEl::CheckUpdateArgs(bool force) {
  if ((method_spec.method == lst_method) && (!force)) return;
  args.Reset(); args.labels.Reset();
  if (!method_spec.method) return;
  MethodDef* md = method_spec.method; //cache
  String arg_nm;
  for (int i = 0; i < md->arg_types.size; ++i) {
    TypeDef* arg_typ = md->arg_types.FastEl(i);
    arg_nm = arg_typ->Get_C_Name() + " " + md->arg_names[i] ;
    args.labels.Add(arg_nm);
    // preseed the arg value with the default
    args.Add(md->arg_defs.SafeEl(i)); 
  }
  args.UpdateAfterEdit(); // note: arrays don't have very good data notify
  lst_method = method_spec.method;
}


//////////////////////////
//  ProgramCallEl	//
//////////////////////////


void ProgramCallEl::Initialize() {
  old_target = NULL;
}

void ProgramCallEl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(prog_args, this);
  taBase::Own(target, this);
}

void ProgramCallEl::CutLinks() {
  target.CutLinks();
  prog_args.CutLinks();
  old_target = NULL;
  inherited::CutLinks();
}

void ProgramCallEl::Copy_(const ProgramCallEl& cp) {
  target = cp.target;
  prog_args = cp.prog_args;
}

void ProgramCallEl::UpdateAfterEdit() {
  if (target.ptr() != old_target) {
    old_target = target.ptr(); // note: we don't ref, because we just need to check ptr addr
    UpdateGlobalArgs();
  }
  inherited::UpdateAfterEdit();
}

Program* ProgramCallEl::GetTarget() {
  if(!target) {
    taMisc::Error("Program target is NULL in ProgramCallEl:",
		  desc, "in program:", program()->name);
  }
  if(!target->CompileScript()) {
    taMisc::Error("Program target script did not compile correctly in ProgramCallEl:",
		  desc, "in program:", program()->name);
  }
  return target.ptr();
}

const String ProgramCallEl::GenCssPre_impl(int indent_level) {
  STRING_BUF(rval, 50);
  rval = cssMisc::Indent(indent_level);
  rval += "{ // call program: "; 
  if (target)
    rval += target->name;
  rval += "\n";
  return rval;
}

const String ProgramCallEl::GenCssBody_impl(int indent_level) {
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
  rval += "{ target->Call(this); }\n";
  rval += cssMisc::Indent(indent_level);
  rval += "}\n";
  
  return rval;
}

const String ProgramCallEl::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "} // call program\n";
}

String ProgramCallEl::GetDisplayName() const {
  String rval = "Call ";
  if (target)
    rval += target->GetName();
  else
    rval += "(no program set)";
  return rval;
}

void ProgramCallEl::PreGenMe_impl(int item_id) {
  // register as a subproc, but only if not a recursive call (which is bad anyway!)
  if (!target) return; // not target (yet), nothing to register
  Program* prog = program();
  if (!prog) return; // shouldn't normally happen
  prog->sub_progs.LinkUnique(this);
}

void ProgramCallEl::UpdateGlobalArgs() {
  if (!target) return; // just leave existing stuff for now
  prog_args.ConformToTarget(target->args);
}

//////////////////////////
//  Program		//
//////////////////////////

Program::RunState 	Program::run_state = Program::NOT_INIT; 
ProgramRef 		Program::top_prog;
ProgramRef 		Program::step_prog;

void Program::Initialize() {
  flags = PF_NONE;
  objs.SetBaseType(&TA_taOBase);
  ret_val = 0;
  m_dirty = true; 
}

void Program::Destroy()	{ 
  CutLinks();
}

void Program::InitLinks() {
  inherited::InitLinks();
  taBase::Own(objs, this);
  taBase::Own(args, this);
  taBase::Own(vars, this);
  taBase::Own(init_els, this);
  taBase::Own(prog_els, this);
  taBase::Own(sub_progs, this);
}

void Program::CutLinks() {
  sub_progs.CutLinks();
  prog_els.CutLinks();
  init_els.CutLinks();
  vars.CutLinks();
  args.CutLinks();
  objs.CutLinks();
  inherited::CutLinks();
}


void Program::Copy_(const Program& cp) {
  if(script) {			// clear first, before trashing anything!
    script->ClearAll();
    script->prog_vars.Reset();
  }
  objs = cp.objs;
  args = cp.args;
  vars = cp.vars;
  init_els = cp.init_els;
  prog_els = cp.prog_els;
  ret_val = 0; // redo
  m_dirty = true; // require rebuild/refetch
  m_scriptCache = "";
  sub_progs.RemoveAll();
}

void Program::UpdateAfterEdit() {
  //WARNING: the running css prog calls this on any changes to our vars,
  // such as ret_val -- therefore, DO NOT do things here that are incompatible
  // with the runtime, in particular, do NOT invalidate the following state flags:
  //   m_dirty, script_compiled
  
  //TODO: the following *do* affect generated script, so we should probably call
  // setDirty(true) if not running, and these changed:
  // name, (more TBD...)
  inherited::UpdateAfterEdit();
}

int Program::Call(Program* caller) {
  int rval = Cont_impl();
  if(run_state == STOP) {
    script->Stop();		// stop us
    caller->script->Stop();	// stop caller!
    caller->script->Prog()->Frame()->pc = 0;
    // NOTE: this backs up to restart the entire call to fun -- THIS DEPENDS ON THE CODE
    // that generates the call!!!!!  ALWAYS MUST BE IN A SUB-BLOCK of code..
  }
  else {
    script->Restart();		// restart script at beginning if run again	
  }
  return rval;
} 

int Program::CallInit(Program* caller) {
  setRunState(INIT); // this is redundant if called from an existing INIT but otherwise needed
  int rval = Run_impl();
  script->Restart();		// for init, always restart script at beginning if run again	
  return rval;
} 

void Program::Init() {
  top_prog = this;
  taMisc::Busy();
  setRunState(INIT);
  Run_impl();
  taMisc::DoneBusy();
  if (ret_val != 0) //TODO: use enums and sensible output string
    QMessageBox::warning(NULL, QString("Operation Failed"),
      String(
      "The Program did not run -- ret_val=").cat(String(ret_val)), 
      QMessageBox::Ok, QMessageBox::NoButton);
  setRunState(DONE);
  script->Restart();		// restart script at beginning if run again
} 

void Program::InitScriptObj_impl() {
  AbstractScriptBase::InitScriptObj_impl();
  //NOTE: nothing else, could be eliminated
}

void Program::PreCompileScript_impl() {
  // as noted in abstractscriptbase: you must call this first to reset the script
  // because if you mess with the existing variables in prog_vars prior to 
  // resetting the script, it will get all messed up.  vars on this space are referred
  // to by a pointer to the space and an index off of it, which is important for autos
  // but actually not for these guys (but they are/were that way anyway).
  AbstractScriptBase::PreCompileScript_impl();
  UpdateProgVars();
}

void Program::setRunState(RunState value) {
  //note: run_state is static/global, but its gui effects are instance
  if (run_state == value) return;
  run_state = value;
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
  DataChanged(DCR_ITEM_UPDATED);
  return ret_val;
}

void Program::Run() {
  top_prog = this;
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
  if (run_state != STOP) {
    script->Restart();
    setRunState(DONE);
  }
} 

void Program::Step() {
  if(step_prog.ptr() == NULL) {
    if(sub_progs.size > 0) {	// set to last guy as a default!
      ProgramCallEl* prg = (ProgramCallEl*)sub_progs.Peek();
      step_prog = prg->target.ptr();
    }
  }
  top_prog = this;
  taMisc::Busy();
  setRunState(STEP);
  Cont_impl();
  taMisc::DoneBusy();
  if (ret_val != 0) {//TODO: use enums and sensible output string
    QMessageBox::warning(NULL, QString("Operation Failed"),
      String(
      "The Program did not run -- ret_val=").cat(String(ret_val)), 
      QMessageBox::Ok, QMessageBox::NoButton);
  }
  if (run_state != STOP) {	// if not stopped (stepping causes a stop)
    script->Restart();
    setRunState(DONE);
  }
}

void Program::Stop() {
  run_state = STOP_REQ;
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
  if(run_state == STOP_REQ) {
    Stop_impl();
    return true;
  }
  if((run_state == STEP) && (step_prog.ptr() == this)) {
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
    prog_els.PreGen(item_id);
    
    // now, build the new script code
    m_scriptCache = "// ";
    m_scriptCache += GetName();
    m_scriptCache += "\n\n/* globals added to hardvars:\n";
    m_scriptCache += "Program::RunState run_state; //note: global static\n";
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
    m_scriptCache += init_els.GenCss(1); // ok if empty, returns nothing
    if (sub_progs.size > 0) {
      if (init_els.size >0) m_scriptCache += "\n";
      m_scriptCache += "  // init any subprogs that could be called from this one\n";
      m_scriptCache += "  { Program* target;\n";
      // todo: this needs to be a list of ProgramCallEl's, not the actual prog itself!
      for (int i = 0; i < sub_progs.size; ++i) {
        ProgramCallEl* sp = (ProgramCallEl*)sub_progs.FastEl(i);
        m_scriptCache += "    if (ret_val != Program::RV_OK) return; // checks previous\n"; 
        m_scriptCache += "    target = this" + sp->GetPath(NULL, this) + "->GetTarget();\n";
        m_scriptCache += "    ret_val = target->CallInit(this);\n"; 
      }
      m_scriptCache += "  }\n";
    }
    m_scriptCache += "}\n\n";
    
    m_scriptCache += "void __Prog() {\n";
    m_scriptCache += prog_els.GenCss(1);
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
  Program_Group* grp = GET_MY_OWNER(Program_Group);
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

#ifdef TA_GUI

void Program::ViewScript() {
  ViewScript_impl();
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
  taBase::Own(global_vars, this);
}

void Program_Group::CutLinks() {
  global_vars.CutLinks();
  inherited::CutLinks();
}

void Program_Group::Copy_(const Program_Group& cp) {
  global_vars = cp.global_vars;
}

void Program_Group::SetProgsDirty() {
  taLeafItr itr;
  Program* prog;
  FOR_ITR_EL(Program, prog, this->, itr) {
    prog->setDirty(true);
  }
}


//////////////////////////
//  Program_List	//
//////////////////////////

void Program_List::Initialize() {
  SetBaseType(&TA_Program);
}


//////////////////////////////////////////////////////////////////////////
//  	misc extra ProgEl's: should be in pdp proper and not ta/css, which is where all above belongs

//////////////////////////
//  Network Counters	//
//////////////////////////

void NetCounterInitEl::Initialize() {
  network_var = NULL;
  local_ctr_var = NULL;
}

void NetCounterInitEl::Destroy() {
  taBase::DelPointer((taBase**)&network_var);
  taBase::DelPointer((taBase**)&local_ctr_var);
}

void NetCounterInitEl::Copy_(const NetCounterInitEl& cp) {
  taBase::SetPointer((taBase**)&network_var, cp.network_var);
  taBase::SetPointer((taBase**)&local_ctr_var, cp.local_ctr_var);
  counter = cp.counter;
} 

void NetCounterInitEl::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetLocalCtrVar();
  if(counter.empty() && local_ctr_var) {
    counter = local_ctr_var->name;
  }
}

String NetCounterInitEl::GetDisplayName() const {
  return "Net Counter Init: " + counter;
}

void NetCounterInitEl::GetLocalCtrVar() {
  if(counter.empty()) return;
  if(local_ctr_var) return;
  Program* my_prog = GET_MY_OWNER(Program);
  if(!my_prog) return;
  if(!(local_ctr_var = my_prog->vars.FindName(counter))) {
    local_ctr_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    local_ctr_var->name = counter;
  }
  local_ctr_var->var_type = ProgVar::T_Int;
}

const String NetCounterInitEl::GenCssBody_impl(int indent_level) {
  if(network_var == NULL) {
    taMisc::Warning("NetCounterInitEl: network_var = NULL -- no code generated!");
    return cssMisc::Indent(indent_level) + "// NetCounterInitEl: Error, network_var = NULL!\n";
  }
  if(local_ctr_var == NULL) {
    taMisc::Warning("NetCounterInitEl: local_ctr_var = NULL -- no code generated!");
    return cssMisc::Indent(indent_level) + "// NetCounterInitEl: Error, local_ctr_var = NULL!\n";
  }
  String rval = cssMisc::Indent(indent_level) + counter + " = 0;\n";
  rval += cssMisc::Indent(indent_level) + network_var->name + "->" + counter + " = " + counter + ";\n";
  return rval;
}

//////////////////////////////////////
// incr

void NetCounterIncrEl::Initialize() {
  network_var = NULL;
  local_ctr_var = NULL;
}

void NetCounterIncrEl::Destroy() {
  taBase::DelPointer((taBase**)&network_var);
  taBase::DelPointer((taBase**)&local_ctr_var);
}

void NetCounterIncrEl::Copy_(const NetCounterIncrEl& cp) {
  taBase::SetPointer((taBase**)&network_var, cp.network_var);
  taBase::SetPointer((taBase**)&local_ctr_var, cp.local_ctr_var);
  counter = cp.counter;
} 

void NetCounterIncrEl::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetLocalCtrVar();
  if(counter.empty() && local_ctr_var) {
    counter = local_ctr_var->name;
  }
}

String NetCounterIncrEl::GetDisplayName() const {
  return "Net Counter Incr: " + counter;
}

void NetCounterIncrEl::GetLocalCtrVar() {
  if(counter.empty()) return;
  if(local_ctr_var) return;
  Program* my_prog = GET_MY_OWNER(Program);
  if(!my_prog) return;
  if(!(local_ctr_var = my_prog->vars.FindName(counter))) {
    local_ctr_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    local_ctr_var->name = counter;
  }
  local_ctr_var->var_type = ProgVar::T_Int;
}

const String NetCounterIncrEl::GenCssBody_impl(int indent_level) {
  if(network_var == NULL) {
    taMisc::Warning("NetCounterIncrEl: network_var = NULL -- no code generated!");
    return cssMisc::Indent(indent_level) + "// NetCounterIncrEl: Error, network_var = NULL!\n";
  }
  if(local_ctr_var == NULL) {
    taMisc::Warning("NetCounterIncrEl: local_ctr_var = NULL -- no code generated!");
    return cssMisc::Indent(indent_level) + "// NetCounterIncrEl: Error, local_ctr_var = NULL!\n";
  }
  String rval = cssMisc::Indent(indent_level) + counter + "++;\n";
  rval += cssMisc::Indent(indent_level) + network_var->name + "->" + counter + " = " + counter + ";\n";
  return rval;
}

