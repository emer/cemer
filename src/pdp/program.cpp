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
}

void ProgVar::Destroy() {
}

void ProgVar::Copy_(const ProgVar& cp) {
  value = cp.value;
}

void ProgVar::UpdateAfterEdit() {
  if (!cssMisc::IsNameValid(name)) {
    taMisc::Error("'", name, "' is not a valid name in css scripts; must be alphanums or underscores");
//TODO: should revert
  }
  inherited::UpdateAfterEdit();
}

int ProgVar::cssType() {
  return cssEl::T_Variant;
}

const String ProgVar::GenCss(bool is_arg) {
  return is_arg ? GenCssArg_impl() : GenCssVar_impl() ;
} 


const String ProgVar::GenCssArg_impl() {
  String rval(0, 80, '\0'); //note: buffer will extend if needed
  rval += "Variant ";
  rval += name;
  bool init = !value.isDefault();
  if (init) {
    rval += " = ";
    rval += value.toCssLiteral();
  }
  return rval;
}
/*new???
const String ProgVar::GenCssVar_impl(bool make_new, TypeDef* val_type) {
  String rval(0, 80, '\0'); //note: buffer will extend if needed
  rval += "Variant ";
  rval += name;
  if (make_new && val_type) { //note: val_type should always be supplied if make_new true
    rval += " = new ";
    rval += val_type->name + "()";
  } else { 
    bool init = !value.isDefault();
    if (init) {
      rval += " = ";
      rval += value.toCssLiteral();
    } else if (!value.isInvalid()) {
      rval += ";  " + name + ".setType(" + String((int)value.type()) + ")";
    }
  }
  rval += ";\n";
  return rval;
}

cssEl* ProgVar::NewCssEl_impl() {
  cssEl* rval;
  switch (value.type()) {
  //case T_Invalid: 
  case T_Bool:
    rval = new cssBool(value.toBool(), name);
    break;
  case T_Int:
  case T_UInt:
    rval = new cssInt(value.toInt(), name);
    break;
  case T_Int64: 
  case T_UInt64:
    rval = new cssInt64(value.toInt64(), name);
    break;
  case T_Double:
    rval = new cssReal(value.toDouble(), name);
    break;
  case T_Char:
    rval = new cssChar(value.toChar(), name);
    break;
  case T_String: 
    rval = new cssString(value.toString(), name);
    break;
  default: 
    taMisc::Warning("ProgVar: unexpected can't create cssEl for VarType: ", String(value.type()));
    rval = &cssMisc::Void ;
  }
  return rval;
} */

const String ProgVar::GenCssVar_impl() {
  STRING_BUF(rval, 80); //note: buffer will extend if needed
  rval += "Variant ";
  rval += name;
  bool init = !value.isDefault();
  if (init) {
    rval += " = ";
    rval += value.toCssLiteral();
  } else if (!value.isInvalid()) {
    rval += ";  " + name + ".setType(" + String((int)value.type()) + ")";
  }
  rval += ";\n";
  return rval;
}

cssEl* ProgVar::NewCssEl() {
 //TODO: maybe we should cache???
  return NewCssEl_impl();
}

cssEl* ProgVar::NewCssEl_impl() {
  cssVariant* rval = new cssVariant(value, name);
  return rval;
}


//////////////////////////
//   EnumProgVar	//
//////////////////////////

void EnumProgVar::Initialize() {
  value.setType(Variant::T_Int);
  enum_type = NULL;
  init = true;
}

void EnumProgVar::Destroy() {
}

void EnumProgVar::Copy_(const EnumProgVar& cp) {
  enum_type = cp.enum_type;
  init = cp.init;
}

int EnumProgVar::cssType() {
  return cssEl::T_Enum;
}

const String EnumProgVar::enumName() {
  if (enum_type) {
    //TODO: need to make sure it is type::enum
    return enum_type->Get_C_Name();
  } else return _nilString;
}

const String EnumProgVar::GenCssArg_impl() {
  STRING_BUF(rval, 80); //note: buffer will extend if needed
  rval += enumName() + " ";
  rval += name;
  if (init) {
    rval += " = ";
    rval += ValToId(value.toInt());
  }
  return rval;
}

const String EnumProgVar::GenCssVar_impl() {
  String rval = GenCssArg_impl();
  rval += ";\n";
  return rval;
}

cssEl* EnumProgVar::NewCssEl_impl() {
  cssEnum* rval = new cssEnum(value.toInt(), name);
  return rval;
}

const String EnumProgVar::ValToId(int val) {
  if (enum_type) {
    return enum_type->Get_C_EnumString(val);
  } else return _nilString;
}


//////////////////////////
//   ObjectProgVar	//
//////////////////////////

void ObjectProgVar::Initialize() {
  val_type = &TA_taOBase; //note: < taOBase generally not interesting
}

void ObjectProgVar::Destroy() {
}

void ObjectProgVar::Copy_(const ObjectProgVar& cp) {
  val_type = cp.val_type;
}

int ObjectProgVar::cssType() {
  return cssEl::T_TA;
}

const String ObjectProgVar::GenCssArg_impl() {
  if (!val_type) return _nilString; // shouldn't happen...
  STRING_BUF(rval, 80); //note: buffer will extend if needed
  rval += val_type->GetPathName() + "* ";
  rval += name;
  //NOTE: make_new not supported
  return rval;
}

const String ObjectProgVar::GenCssVar_impl() {
  if (!val_type) return _nilString; // shouldn't happen...
  STRING_BUF(rval, 80); //note: buffer will extend if needed
  rval += val_type->GetPathName() + "* ";
  rval += name;
  bool init = !value.isDefault();
  if (init) {
    rval += " = ";
    rval += value.toCssLiteral();
  }
  rval += ";\n";
  return rval;
}

cssEl* ObjectProgVar::NewCssEl_impl() {
  // note: use the val_type, not the actual type, in case user
  // wanted the var to be more generic than current instance
  taBase* tab = value.toBase();
  cssTA* rval = new cssTA(tab, 1, val_type, name);
  return rval;
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
  value = cp.value.toCssLiteral();
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
//  PreTestLoopEl		//
//////////////////////////

const String PreTestLoopEl::GenCssPre_impl(int indent_level) {
  String rval = inherited::GenCssPre_impl(indent_level);
  rval += cssMisc::Indent(indent_level);
  rval += loopHeader();
  rval += " {\n";
  return rval; 
}

const String PreTestLoopEl::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "}\n";
  rval += inherited::GenCssPost_impl(indent_level);
  return rval;
}

const String PreTestLoopEl::loopHeader(bool) const {
  STRING_BUF(rval, 60);
  rval += "while (";
  rval += loop_var;
  rval += + ")";
  return rval;
}

//////////////////////////
//  PostTestLoopEl		//
//////////////////////////

const String PostTestLoopEl::GenCssPre_impl(int indent_level) {
  String rval = inherited::GenCssPre_impl(indent_level);
  rval += cssMisc::Indent(indent_level);
  rval += "do {\n";
  return rval; 
}

const String PostTestLoopEl::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level);
  rval += "} while (" + loop_var + ");\n";
  rval += inherited::GenCssPost_impl(indent_level);
  return rval;
}

const String PostTestLoopEl::loopHeader(bool display) const {
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
  var_type = &TA_taBase; // placeholder
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
  if (script_obj && script_obj->val_type)
    var_type = script_obj->val_type;
  else var_type = &TA_taBase; // placeholder
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
  target = NULL;
  old_target = NULL;
}

void ProgramCallEl::InitLinks() {
  static String _def_user_script("cerr << \"Program Call failed--Stopping\\n\";\nret_val = Program::RV_PROG_CALL_FAILED;\nthis->StopScript();\n");
  inherited::InitLinks();
  taBase::Own(prog_args, this);
  taBase::Own(fail_el, this);
  if (!taMisc::is_loading) {
    fail_el.user_script = _def_user_script;
  }
}

void ProgramCallEl::CutLinks() {
  taBase::DelPointer((taBase**)&target);
  prog_args.CutLinks();
  old_target = NULL;
  inherited::CutLinks();
}

void ProgramCallEl::Copy_(const ProgramCallEl& cp) {
  taBase::SetPointer((taBase**)&target, cp.target);
  prog_args = cp.prog_args;
  fail_el = cp.fail_el;
}

void ProgramCallEl::UpdateAfterEdit() {
  if (target != old_target) {
    old_target = target; // note: we don't ref, because we just need to check ptr addr
    UpdateGlobalArgs();
  }
  inherited::UpdateAfterEdit();
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
  rval += cssMisc::Indent(indent_level);
  rval += "Int call_result = Program::RV_COMPILE_ERR;\n";
  rval += cssMisc::Indent(indent_level);
  rval += "Program* target = ";
  rval += target->GetPath();
  rval += ";\n";
  rval += cssMisc::Indent(indent_level);
  rval += "if (target->CompileScript()) {\n"; 
  ++indent_level;
  
  rval += cssMisc::Indent(indent_level);
  if (prog_args.size > 0)
    rval += "// set global vars of target\n";
  String nm;
  bool set_one = false;
  for (int i = 0; i < prog_args.size; ++i) {
    ProgArg* ths_arg = prog_args.FastEl(i);
    nm = ths_arg->name;
    ProgVar* prg_var = target->param_vars.FindName(nm);
    if (!prg_var || ths_arg->value.empty()) continue;
    set_one = true;
    rval += cssMisc::Indent(indent_level);
    rval += "target->SetGlobalVar(\"" + prg_var->name + "\", "
      + ths_arg->value + ");\n";
  }
  if (set_one) {
    rval += cssMisc::Indent(indent_level);
    rval += "target->DataChanged(DCR_ITEM_UPDATED);\n";
  }
  rval += cssMisc::Indent(indent_level);
  rval += "call_result = target->Call(this);\n";
  --indent_level;
  rval += cssMisc::Indent(indent_level);
  rval += "}\n";
  rval += cssMisc::Indent(indent_level);
  rval += "if (call_result != 0) {\n";
  rval += fail_el.GenCss(indent_level + 1);
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
  prog->sub_progs.LinkUnique(target);
}

void ProgramCallEl::UpdateGlobalArgs() {
  if (!target) return; // just leave existing stuff for now
  prog_args.ConformToTarget(target->param_vars);
}

//////////////////////////
//  Program		//
//////////////////////////

Program::RunState 	Program::run_state = Program::NOT_INIT; 
ProgramRef 		Program::top_prog;
ProgramRef 		Program::step_prog;

void Program::Initialize() {
  flags = PF_NONE;
  prog_objs.SetBaseType(&TA_taOBase);
  ret_val = 0;
  m_dirty = true; 
}

void Program::Destroy()	{ 
  CutLinks();
}

void Program::InitLinks() {
  inherited::InitLinks();
  taBase::Own(prog_objs, this);
  taBase::Own(param_vars, this);
  taBase::Own(global_vars, this);
  taBase::Own(init_els, this);
  taBase::Own(prog_els, this);
  taBase::Own(sub_progs, this);
}

void Program::CutLinks() {
  sub_progs.CutLinks();
  prog_els.CutLinks();
  init_els.CutLinks();
  global_vars.CutLinks();
  param_vars.CutLinks();
  prog_objs.CutLinks();
  inherited::CutLinks();
}


void Program::Copy_(const Program& cp) {
  prog_objs = cp.prog_objs;
  param_vars = cp.param_vars;
  global_vars = cp.global_vars; //TODO: prob should do fixups for refs to prog_objs
  init_els = cp.init_els;
  prog_els = cp.prog_els;
  ret_val = 0; // redo
  m_dirty = true; // require rebuild/refetch
  m_scriptCache = "";
  sub_progs.RemoveAll();
  if (script)
    script->ClearAll();
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
      step_prog = sub_progs.Peek();
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
  *el = value;
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
    if (param_vars.size > 0) {
      m_scriptCache += "// global script parameters\n";
      m_scriptCache += param_vars.GenCss(0);
    }
    if (global_vars.size > 0) {
      m_scriptCache += "// global (non-param) variables\n";
      m_scriptCache += global_vars.GenCss(0);
    }
    m_scriptCache += "*/\n\n";
    
    // __Init() routine, for our own els, and calls to subprog Init()
    m_scriptCache += "void __Init() {\n";
    m_scriptCache += init_els.GenCss(1); // ok if empty, returns nothing
    if (sub_progs.size > 0) {
      if (init_els.size >0) m_scriptCache += "\n";
      m_scriptCache += "  // init any subprogs that could be called from this one\n";
      m_scriptCache += "  { Program* target;\n";
      for (int i = 0; i < sub_progs.size; ++i) {
        Program* target = sub_progs.FastEl(i);
        m_scriptCache += "    if (ret_val != Program::RV_OK) return; // checks previous\n"; 
        m_scriptCache += "    target = " + target->GetPath() + ";\n";
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
  //NOTE: if we have to nuke any or change any types then we have to recompile!
  // but currently, we only do this before recompiling anyway, so no worries!
// easiest is just to nuke and recreate...
  // nuke existing
  script->prog_vars.Reset(); // removes/unref-deletes
  
  // add the ones in the object -- note, we use *pointers* to these
  cssEl* el = NULL;
//buggy??  el = new cssCPtr_enum(&run_state, 1, "run_state"); //note: static
//BA 7/5/06, prob not buggy, issue before was caused by Init() same name 
  el = new cssCPtr_int(&run_state, 1, "run_state"); //note: static
  script->prog_vars.Push(el); //refs
  el = new cssCPtr_int(&ret_val, 1, "ret_val");
  script->prog_vars.Push(el); //refs
  el = new cssTA_Base(&prog_objs, 1, prog_objs.GetTypeDef(), "prog_objs");
  script->prog_vars.Push(el); //refs
  
//TODO: these should probably be refs, not values
  // add new in the program
  for (int i = 0; i < param_vars.size; ++i) {
    ProgVar* sv = param_vars.FastEl(i);
    el = sv->NewCssEl();
    script->prog_vars.Push(el); //refs
  } 
  for (int i = 0; i < global_vars.size; ++i) {
    ProgVar* sv = global_vars.FastEl(i);
    el = sv->NewCssEl();
    script->prog_vars.Push(el); //refs
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
    } 
    grp = (Program_Group*)grp->GetOwner(&TA_Program_Group);
  }
/*old  int i = 0;
  ProgVar* sv;
  cssEl* el;
  // update names and values of existing (if any)
  while ((m_our_hardvar_base_index + i) < script->hard_vars.size) {
    sv = global_vars.FastEl(i);
    if (sv->ignore) continue;
    el = script->hard_vars.FastEl(m_our_hardvar_base_index + i);
    // easiest (harmless) is to just update, even if not changed 
    el->name = sv->name;
    *el = sv->value;
    ++i;
  }
  
  // add new
  while (i < global_vars.size) {
    sv = global_vars.FastEl(i);
    if (sv->ignore) continue;
    el = sv->NewCssEl();
    script->hard_vars.Push(el); //refs
    ++i;
  } */
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


