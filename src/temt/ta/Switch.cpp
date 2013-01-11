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

#include "Switch.h"


void Switch::Initialize() {
  cases.SetBaseType(&TA_CaseBlock);
}

void Switch::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!switch_var, quiet, rval, "switch_var is NULL");
}

void Switch::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  cases.CheckConfig(quiet, rval);
}

void Switch::GenCssPre_impl(Program* prog) {
  if(!switch_var) return;
  prog->AddLine(this, "switch(" + switch_var->name + ") {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
}

void Switch::GenCssBody_impl(Program* prog) {
  if(!switch_var) return;
  cases.GenCss(prog);
}

void Switch::GenCssPost_impl(Program* prog) {
  if(!switch_var) return;
  prog->DecIndent();
  prog->AddLine(this, "}");
}

const String Switch::GenListing_children(int indent_level) {
  return cases.GenListing(indent_level+1);
}

String Switch::GetDisplayName() const {
  if(switch_var)
    return "switch(" + switch_var->name + ")";
  return "switch( VAR NOT SET!)";
}

void Switch::PreGenChildren_impl(int& item_id) {
  cases.PreGen(item_id);
}

ProgVar* Switch::FindVarName(const String& var_nm) const {
  return cases.FindVarName(var_nm);
}

void Switch::CasesFmEnum() {
  if(TestError(!switch_var, "CasesFmEnum", "switch_var not set!"))
    return;
  if(TestError(((switch_var->var_type != ProgVar::T_DynEnum) &&
                (switch_var->var_type != ProgVar::T_HardEnum)), "CasesFmEnum", "switch_var is not an enumerated type (either hard-coded enum or a dynamic enum)!"))
    return;

  if(switch_var->var_type == ProgVar::T_HardEnum)
    CasesFmEnum_hard();
  else
    CasesFmEnum_dyn();

  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
  }
}

void Switch::CasesFmEnum_hard() {
  if(TestError(!switch_var->hard_enum_type, "CasesFmEnum", "switch_var hard_enum_type not set!"))
    return;
  TypeDef* et = switch_var->hard_enum_type;
  String enm = et->name + "::";
  int i;  int ti;
  CaseBlock* pe;
  EnumDef* ei;
  // delete ones that no longer exist
  for (i = cases.size - 1; i >= 0; --i) {
    pe = (CaseBlock*)cases.FastEl(i);
    ei = et->enum_vals.FindName(pe->case_val.expr);
    if(!ei) {
      cases.RemoveIdx(i);
    }
  }
  // add new ones and put in order
  for (ti = 0; ti < et->enum_vals.size; ++ti) {
    ei = et->enum_vals.FastEl(ti);
    String einm = enm + ei->name;
    for(i=0;i<cases.size;i++) {
      pe = (CaseBlock*)cases.FastEl(i);
      if(pe->case_val.expr == einm) break;
    }
    if(i==cases.size) {
      pe = new CaseBlock();
      pe->case_val.SetExpr(einm);
      cases.Insert(pe, ti);
    } else if (i != ti) {
      cases.MoveIdx(i, ti);
    }
  }
}

void Switch::CasesFmEnum_dyn() {
  if(TestError(!switch_var->dyn_enum_val.enum_type.ptr(), "CasesFmEnum", "switch_var dyn enum_type not set!"))
    return;
  DynEnumType* et = switch_var->dyn_enum_val.enum_type.ptr();
  int i;  int ti;
  CaseBlock* pe;
  DynEnumItem* ei;
  // delete ones that no longer exist
  for (i = cases.size - 1; i >= 0; --i) {
    pe = (CaseBlock*)cases.FastEl(i);
    ei = et->enums.FindName(pe->case_val.expr);
    if(!ei) {
      cases.RemoveIdx(i);
    }
  }
  // add new ones and put in order
  for (ti = 0; ti < et->enums.size; ++ti) {
    ei = et->enums.FastEl(ti);
    for(i=0;i<cases.size;i++) {
      pe = (CaseBlock*)cases.FastEl(i);
      if(pe->case_val.expr == ei->name) break;
    }
    if(i==cases.size) {
      pe = new CaseBlock();
      pe->case_val.SetExpr(ei->name);
      cases.Insert(pe, ti);
    } else if (i != ti) {
      cases.MoveIdx(i, ti);
    }
  }
}

bool Switch::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("switch")) return true;
  return false;
}

bool Switch::CvtFmCode(const String& code) {
  String cd = trim(code.after("switch"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  switch_var = FindVarNameInScope(cd, true); // prompt to make if not found
  return true;
}
