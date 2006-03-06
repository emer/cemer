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


//////////////////////////
//  ProgEl		//
//////////////////////////

String ProgEl::indent(int indent_level) {
  if (indent_level == 0) return _nilString;
  else return String(indent_level * 2, 0, ' ');
}

void ProgEl::Initialize() {
}

String ProgEl::GenCss(int indent_level) {
  String rval;
  rval = GenCssPre_impl(indent_level) + GenCssBody_impl(indent_level) + GenCssPost_impl(indent_level);
  return rval;
}


//////////////////////////
//  UserScriptEl	//
//////////////////////////

void UserScriptEl::Initialize() {
  user_script = "// TODO: Add your CSS script code here.\n";
}

void UserScriptEl::Copy_(const UserScriptEl& cp) {
  user_script = cp.user_script;
}

String UserScriptEl::GenCssBody_impl(int indent_level) {
// TODO: maybe indent every line by the indent amount
  return user_script;
}


//////////////////////////
//  ProgEl_List		//
//////////////////////////

String ProgEl_List::GenCss(int indent_level) {
  String rval;
  taListItr itr;
  ProgEl* el;
  FOR_ITR_EL(ProgEl, el, this->, itr) {
    rval += el->GenCss(indent_level); 
  }
  return rval;;
}


//////////////////////////
//  ProgVar		//
//////////////////////////

void ProgVar::Initialize() {
  var_type = &TA_Variant;
}

void ProgVar::Copy_(const ProgVar& cp) {
  var_name = cp.var_name;
  var_type = cp.var_type;
  init_val = cp.init_val;
}

String ProgVar::GenCssBody_impl(int) {//indent ignored
  String rval = var_type->name + " " + var_name;
  //TODO: maybe should check for missing "" for
  // strings, and automatically provide; or could do in UAE
  if (!init_val.empty())
    rval += " = " + init_val;
  return rval;
}


//////////////////////////
//  ProgVar_List		//
//////////////////////////

void ProgVar_List::Initialize() {
  SetBaseType(&TA_ProgVar);
  var_context = VC_ProgVars;
}

String ProgVar_List::GenCss(int indent_level) {
  String rval;
  ProgVar* el;
  for (int i = 0; i < size; ++i) {
    el = (ProgVar*)FastEl(i);
    if (var_context == VC_FuncArgs) {
      if (i > 0)
        rval += ", ";
    } else {
      rval += ProgEl::indent(indent_level); 
    }
    rval += el->GenCss(0); //note: indent not used by ProgVar
    if (var_context == VC_ProgVars)
      rval += ";\n";
  }
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

String ProgList::GenCssBody_impl(int indent_level) {
  return prog_els.GenCss(indent_level);
}


//////////////////////////
//  LoopEl		//
//////////////////////////

void LoopEl::Initialize() {
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

String LoopEl::GenCssPre_impl(int indent_level) {
  String rval = indent(indent_level);
  rval += "{Int " + loop_var + ";\n";
  rval += "for (" + loop_var + " = " + init_val + "; "
    + loop_test + "; " 
    + loop_iter + ") {\n";
  return rval; 
}

String LoopEl::GenCssBody_impl(int indent_level) {
  return inherited::GenCssBody_impl(indent_level + 1);
}

String LoopEl::GenCssPost_impl(int indent_level) {
  return indent(indent_level) + "}}\n";
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

String CondEl::GenCssPre_impl(int indent_level) {
  String rval = indent(indent_level);
  rval += "if (" + cond_test + ") {\n";
  return rval; 
}

String CondEl::GenCssBody_impl(int indent_level) {
  String rval = true_els.GenCss(indent_level + 1);
  rval += indent(indent_level) + "} else {\n";
  rval += false_els.GenCss(indent_level + 1);
  return rval;
}

String CondEl::GenCssPost_impl(int indent_level) {
  return indent(indent_level) + "}\n";
}



//////////////////////////
//  Program		//
//////////////////////////

void Program::Initialize() {
}

void Program::InitLinks() {
  inherited::InitLinks();
}

void Program::CutLinks() {
  inherited::CutLinks();
}

void Program::Copy_(const Program& cp) {
  name = cp.name;
}

void Program::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}


//////////////////////////
//  Program_MGroup	//
//////////////////////////

void Program_MGroup::Initialize() {
  SetBaseType(&TA_Program);
}
