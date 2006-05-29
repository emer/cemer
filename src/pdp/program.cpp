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
  ignore = false;
}

void ProgVar::Destroy() {
}

void ProgVar::Copy_(const ProgVar& cp) {
  ignore = cp.ignore;
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
  if (ignore) return _nilString;
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
  make_new = false;
}

void ObjectProgVar::Destroy() {
}

void ObjectProgVar::Copy_(const ObjectProgVar& cp) {
  val_type = cp.val_type;
  make_new = cp.make_new;
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
  if (make_new) { 
    rval += " = new ";
    rval += val_type->GetPathName() + "()";
  } else { 
    bool init = !value.isDefault();
    if (init) {
      rval += " = ";
      rval += value.toCssLiteral();
    }
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
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    prog->setDirty(true);
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
    if (el->ignore) continue;
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
  // we could just return the string, but that wouldn't indent...
// TODO: indent every line by the indent amount
//  String rval(user_script.length() + 20, 0, '\0');
  return user_script;
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

void LoopEl::Initialize() {
  loop_var_type = "Int"; // the loop variable
  loop_var = "i"; // the loop variable
  init_val = "0"; // initial value of loop variable
  loop_test = "i < 20"; // the test each time
  loop_iter = "i += 1"; // the iteration operation
}

void LoopEl::Copy_(const LoopEl& cp) {
  loop_var = cp.loop_var;
  init_val = cp.init_val;
  loop_test = cp.loop_test;
  loop_iter = cp.loop_iter;
}

const String LoopEl::GenCssPre_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level);
  rval += "{" + loop_var_type + " " + loop_var + ";\n";
  rval += "for (" + loop_var + " = " + init_val + "; "
    + loop_test + "; " 
    + loop_iter + ") {\n";
  return rval; 
}

const String LoopEl::GenCssBody_impl(int indent_level) {
  return inherited::GenCssBody_impl(indent_level + 1);
}

const String LoopEl::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "}}\n";
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
  static String _def_user_script("cerr << \"Program Call failed--Stopping\\n\";\nthis->StopScript();\n");
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
  rval += "Bool call_succeeded = false;\n";
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
    if (!prg_var || prg_var->ignore) continue;
    rval += cssMisc::Indent(indent_level);
    rval += "target->SetGlobalVar(\"" + prg_var->name + "\", "
      + ths_arg->value + ");\n";
  }
  
  rval += cssMisc::Indent(indent_level);
  rval += "call_succeeded = target->Run();\n";
  --indent_level;
  rval += cssMisc::Indent(indent_level);
  rval += "}\n";
  rval += cssMisc::Indent(indent_level);
  rval += "if (!call_succeeded) {\n";
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
  global_args.ConformToTarget(target->global_vars);
}

//////////////////////////
//  Program		//
//////////////////////////

void Program::Initialize() {
  m_dirty = true; 
}

void Program::Destroy()	{ 
  CutLinks();
}

void Program::InitLinks() {
  inherited::InitLinks();
  taBase::Own(global_vars, this);
}

void Program::CutLinks() {
  global_vars.CutLinks();
  inherited::CutLinks();
}


void Program::Copy_(const Program& cp) {
  m_dirty = true; // require rebuild/refetch
  m_scriptCache = "";
  if (script)
    script->ClearAll();
}

void Program::UpdateAfterEdit() {
  setDirty(true);
  inherited::UpdateAfterEdit();
}

void Program::InitScriptObj_impl() {
  AbstractScriptBase::InitScriptObj_impl();
  //NOTE: nothing else, could be eliminated
}

void Program::PreCompileScript_impl() {
  AbstractScriptBase::PreCompileScript_impl();
  UpdateProgVars();
}

bool Program::Run() {
  if (!script_compiled) {
    if (!CompileScript())
      return false; // deferred or error
  }
  return RunScript();
}

void Program::ScriptCompiled() {
  AbstractScriptBase::ScriptCompiled();
  m_dirty = false;
  script_compiled = true;
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

void  Program::UpdateProgVars() {
  //NOTE: if we have to nuke any or change any types then we have to recompile!
  // but currently, we only do this before recompiling anyway, so no worries!
// easiest is just to nuke and recreate...
  // nuke existing
  script->prog_vars.Reset(); // removes/unref-deletes
    
  // add new
  for (int i = 0; i < global_vars.size; ++i) {
    ProgVar* sv = global_vars.FastEl(i);
    if (sv->ignore) continue;
    cssEl* el = sv->NewCssEl();
    script->prog_vars.Push(el); //refs
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
void Program::RunGui() {
  //TODO: need to put up a progress/cancel dialog
  // TODO: actually need to provide a global strategy for running!!!!!
  bool ran = Run();
  if (ran) 
    QMessageBox::information(NULL, QString("Operation Succeeded"),
      QString("The Program finished"), QMessageBox::Ok);
  else 
    QMessageBox::warning(NULL, QString("Operation Failed"),
      QString("The Program did not run -- check that there is script source, and that there were no compile errors"), QMessageBox::Ok, QMessageBox::NoButton);
 ; 
  
}

void Program::ViewScript() {
  ViewScript_impl();
}
#endif  // TA_GUI


//////////////////////////
//  Program_MGroup	//
//////////////////////////

void Program_MGroup::Initialize() {
  SetBaseType(&TA_Program);
}



//////////////////////////
//  ProgElProgram	//
//////////////////////////

void ProgElProgram::Initialize() {
}

void ProgElProgram::Destroy()	{ 
  CutLinks();
}

void ProgElProgram::InitLinks() {
  inherited::InitLinks();
  taBase::Own(prog_els, this);
}

void ProgElProgram::CutLinks() {
  prog_els.CutLinks();
  inherited::CutLinks();
}

void ProgElProgram::Copy_(const ProgElProgram& cp) {
  prog_els = cp.prog_els;
}

void ProgElProgram::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

const String ProgElProgram::scriptString() {
  if (m_dirty) {
    m_scriptCache = "// ";
    m_scriptCache += GetName();
    m_scriptCache += "\n\n/* globals added to hardvars:\n";
    m_scriptCache += global_vars.GenCss(0);
    m_scriptCache += "*/\n\n";
    
    m_scriptCache += prog_els.GenCss(0);
    m_scriptCache += "\n";
    m_dirty = false;
  }
  return m_scriptCache;
}

#ifdef TA_GUI
void ProgElProgram::ViewScript_impl() {
  iTextEditDialog* dlg = new iTextEditDialog(true); // readonly
  dlg->setText(scriptString());
  dlg->exec();
}
#endif // TA_GUI

//////////////////////////
//  FileProgram	//
//////////////////////////

void FileProgram::Initialize() {
  script_file = taFiler_CreateInstance(".","*.css",false);
  script_file->compress = false;	// don't compress
  script_file->mode = taFiler::NO_AUTO;
  taRefN::Ref(script_file);
}


void FileProgram::Destroy() { 
  if (script_file) {
    taRefN::unRefDone(script_file);
    script_file = NULL;
  }
}

void FileProgram::Copy_(const FileProgram& cp) {
  if (script_file && cp.script_file)
    *script_file = *(cp.script_file);
}

const String FileProgram::scriptFilename() {
  return script_file->fname;
}

AbstractScriptBase::ScriptSource FileProgram::scriptSource() {
  if (script_file->fname.empty())
    return NoScript;
  else
    return ScriptFile;
}

#ifdef TA_GUI
void FileProgram::ViewScript_impl() {
  //TODO: edit in editor
}
#endif // TA_GUI

