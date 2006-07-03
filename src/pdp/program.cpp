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
    Program_MGroup* grp = GET_MY_OWNER(Program_MGroup);
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
  rval = trimr(rval);
  if (rval.lastchar() != '\n')
    rval += '\n';
  return rval;
}


//////////////////////////
//  ProgEl_List		//
//////////////////////////

const String ProgEl_List::GenCss(int indent_level) {
  String rval;
  ProgEl* el;
  for (int i = 0; i < size; ++i) {
    el = FastEl(i);
    rval += el->GenCss(indent_level); 
  }
  return rval;;
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
  rval += "for (" + loop_var + " = " + init_val + "; "
    + loop_test + "; " 
    + loop_iter + ") {\n";
  return rval; 
}

const String ForLoopEl::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "}\n";
  rval += inherited::GenCssPost_impl(indent_level);
  return rval;
}


//////////////////////////
//  WhileLoopEl		//
//////////////////////////

const String WhileLoopEl::GenCssPre_impl(int indent_level) {
  String rval = inherited::GenCssPre_impl(indent_level);
  rval += cssMisc::Indent(indent_level);
  rval += "while (" + loop_var + ") {\n";
  return rval; 
}

const String WhileLoopEl::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "}\n";
  rval += inherited::GenCssPost_impl(indent_level);
  return rval;
}


//////////////////////////
//  UntilLoopEl		//
//////////////////////////

const String UntilLoopEl::GenCssPre_impl(int indent_level) {
  String rval = inherited::GenCssPre_impl(indent_level);
  rval += cssMisc::Indent(indent_level);
  rval += "do {\n";
  return rval; 
}

const String UntilLoopEl::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level);
  rval += "} while (" + loop_var + ");\n";
  rval += inherited::GenCssPost_impl(indent_level);
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
  if (!method_spec.script_obj && !method_spec.method) return _nilString;
  
  STRING_BUF(rval, 80); // more allocated if needed
  rval += cssMisc::Indent(indent_level);
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
  taBase::Own(global_args, this);
  taBase::Own(fail_el, this);
  if (!taMisc::is_loading) {
    fail_el.user_script = _def_user_script;
  }
}

void ProgramCallEl::CutLinks() {
  taBase::DelPointer((taBase**)&target);
  global_args.CutLinks();
  old_target = NULL;
  inherited::CutLinks();
}

void ProgramCallEl::Copy_(const ProgramCallEl& cp) {
  taBase::SetPointer((taBase**)&target, cp.target);
  global_args = cp.global_args;
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
  rval += "Program target = ";
  rval += target->GetPath();
  rval += ";\n";
  rval += cssMisc::Indent(indent_level);
  rval += "if (target->CompileScript()) {\n"; 
  ++indent_level;
  
  rval += cssMisc::Indent(indent_level);
  rval += "// set global vars of target\n";
  String nm;
  ProgArg* ths_arg;
  ProgVar* prg_var;
  for (int i = 0; i < global_args.size; ++i) {
    ths_arg = global_args.FastEl(i);
    nm = ths_arg->name;
    prg_var = target->global_vars.FindName(nm);
    if (!prg_var || ths_arg->value.empty()) continue;
    rval += cssMisc::Indent(indent_level);
    rval += "target->SetGlobalVar(\"" + prg_var->name + "\", "
      + ths_arg->value + ");\n";
  }
  
  rval += cssMisc::Indent(indent_level);
  rval += "call_result = target->Run();\n";
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

void ProgramCallEl::UpdateGlobalArgs() {
  if (!target) return; // just leave existing stuff for now
  global_args.ConformToTarget(target->param_vars);
}

//////////////////////////
//  Program		//
//////////////////////////

Program::RunMode Program::run_mode = Program::STOP; 
ProgramRef 	Program::step_prog;

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
}

void Program::CutLinks() {
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
  if (script)
    script->ClearAll();
}

void Program::UpdateAfterEdit() {
  setDirty(true);
  inherited::UpdateAfterEdit();
}

void Program::AddToController(Controller* cont) {
  if (!cont) return;
  cont->AddProgram(this);
}

void Program::InitScriptObj_impl() {
  AbstractScriptBase::InitScriptObj_impl();
  //NOTE: nothing else, could be eliminated
}

void Program::PreCompileScript_impl() {
  AbstractScriptBase::PreCompileScript_impl();
  UpdateProgVars();
}

int Program::RunEx(RunMode rm) {
  ret_val = RV_OK;
  if (!script_compiled) {
    if (!CompileScript()) {
      ret_val = RV_COMPILE_ERR; 
      return ret_val; // deferred or error
    }
  }
  
  RunMode last = run_mode;
  run_mode = rm;
  bool ran_ok = RunScript();
  if (!ran_ok) { //still need to check for init etc.
    ret_val = RV_RUNTIME_ERR;
  }
  run_mode = last;
  //note: shared var state likely changed, so update gui
  DataChanged(DCR_ITEM_UPDATED);
  return ret_val;
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
    m_scriptCache = "// ";
    m_scriptCache += GetName();
    m_scriptCache += "\n\n/* globals added to hardvars:\n";
    m_scriptCache += "Program::RunMode run_mode; //note: global static\n";
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
    
    if (init_els.size > 0) {
      m_scriptCache += "void Init() {\n";
      m_scriptCache += init_els.GenCss(1); // ok if empty, returns nothing
      m_scriptCache += "}\n\n";
    }
    
    m_scriptCache += "ret_val = Program::RV_OK; // set elsewise on failure\n";
    m_scriptCache += "if (run_mode == Program::INIT) {\n";
    if (init_els.size > 0) {
    m_scriptCache += "  Init();\n";
    }
    m_scriptCache += "  return;\n";
    m_scriptCache += "}\n\n";
    
    m_scriptCache += prog_els.GenCss(0);
    m_scriptCache += "\n";
    m_dirty = false;
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
  el = new cssCPtr_enum(&run_mode, 1, "run_mode"); //note: static
  script->prog_vars.Push(el); //refs
  el = new cssCPtr_int(&ret_val, 1, "ret_val");
  script->prog_vars.Push(el); //refs
  el = new cssTA_Base(&prog_objs, 1, prog_objs.GetTypeDef(), "prog_objs");
  script->prog_vars.Push(el); //refs
  
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
  Program_MGroup* grp = GET_MY_OWNER(Program_MGroup);
  while (grp) {
    for (int i = 0; i < grp->global_vars.size; ++i) {
      ProgVar* sv = grp->global_vars.FastEl(i);
      // for group vars, we only add with unique names 
      if (script->prog_vars.IndexOfName(sv->GetName()) >= 0) continue;
      el = sv->NewCssEl();
      script->prog_vars.Push(el); //refs
    } 
    grp = (Program_MGroup*)grp->GetOwner(&TA_Program_MGroup);
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
void Program::InitGui() {
  //TODO: maybe need to put up a progress/cancel dialog
  // TODO: actually need to provide a global strategy for running!!!!!
  int rval = Init();
  //note: init should be quick, so we don't confirm
  if (rval != 0) 
    QMessageBox::warning(NULL, QString("Operation Failed"),
      String(
      "The Program did not run -- ret_val=").cat(String(rval)), 
      QMessageBox::Ok, QMessageBox::NoButton);
 ; 
}

void Program::RunGui() {
  //TODO: need to put up a progress/cancel dialog
  // TODO: actually need to provide a global strategy for running!!!!!
  int rval = Run();
  if (rval == 0) 
    QMessageBox::information(NULL, QString("Operation Succeeded"),
      QString("The Program finished"), QMessageBox::Ok);
  else 
    QMessageBox::warning(NULL, QString("Operation Failed"),
      String(
      "The Program did not run -- ret_val=").cat(String(rval)), 
      QMessageBox::Ok, QMessageBox::NoButton);
 ; 
  
}

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
//  Program_MGroup	//
//////////////////////////

void Program_MGroup::Initialize() {
  SetBaseType(&TA_Program);
}

void Program_MGroup::InitLinks() {
  inherited::InitLinks();
  taBase::Own(global_vars, this);
}

void Program_MGroup::CutLinks() {
  global_vars.CutLinks();
  inherited::CutLinks();
}

void Program_MGroup::Copy_(const Program_MGroup& cp) {
  global_vars = cp.global_vars;
}

void Program_MGroup::SetProgsDirty() {
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


//////////////////////////
//  Controller	//
//////////////////////////

void Controller::Initialize() {
}

void Controller::Destroy() {
  CutLinks();
}

void Controller::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

void Controller::InitLinks() {
  inherited::InitLinks();
  taBase::Own(progs, this);
}

void Controller::CutLinks() {
  progs.CutLinks();
  inherited::CutLinks();
}

void Controller::Copy_(const Controller& cp) {
  progs = cp.progs;
}

void Controller::AddProgram(Program* prog) {
  progs.LinkUnique(prog);
}


#ifdef TA_GUI
void Controller::InitGui() {
  Program* prog = progs.SafeEl(0);
  if (prog)
    prog->InitGui();
}

void Controller::RunGui() {
  Program* prog = progs.SafeEl(0);
  if (prog)
    prog->RunGui();
}
#endif // TA_GUI


//////////////////////////
//  Controller_MGroup	//
//////////////////////////

void Controller_MGroup::Initialize() {
  SetBaseType(&TA_Controller);
}

