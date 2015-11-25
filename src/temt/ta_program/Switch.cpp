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
#include <Program>
#include <taMisc>
#include <tabMisc>
#include <CaseBlock>
#include <EnumDef>
#include <taiWidgetTokenChooser>

TA_BASEFUNS_CTORS_DEFN(Switch);


void Switch::Initialize() {
  cases.SetBaseType(&TA_CaseBlock);
  cases.AddAcceptableType("CaseBlock");
  cases.check_with_parent = false;
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
  return "switch(name_of_variable)";
}

void Switch::PreGenChildren_impl(int& item_id) {
  cases.PreGen(item_id);
}

ProgVar* Switch::FindVarName(const String& var_nm) const {
  return cases.FindVarName(var_nm);
}

void Switch::CasesFmEnum(bool add_default) {
  if(TestError(!switch_var, "CasesFmEnum", "switch_var not set!"))
    return;
  if(TestError(((switch_var->var_type != ProgVar::T_DynEnum) &&
                (switch_var->var_type != ProgVar::T_HardEnum)), "CasesFmEnum", "switch_var is not an enumerated type (either hard-coded enum or a dynamic enum)!"))
    return;

  if(switch_var->var_type == ProgVar::T_HardEnum)
    CasesFmEnum_hard();
  else
    CasesFmEnum_dyn();

  if(add_default) {
    CaseBlock* pe;
    bool got_def = false;
    for (int i = cases.size - 1; i >= 0; --i) {
      pe = dynamic_cast<CaseBlock*>(cases.FastEl(i));
      if(pe->case_val.expr.empty()) { // default
        got_def = true;
        break;
      }
    }
    if(!got_def) {
      pe = (CaseBlock*)cases.New(1, &TA_CaseBlock);
    }
  }
  
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
    pe = dynamic_cast<CaseBlock*>(cases.FastEl(i));
    if(!pe) {
      cases.RemoveIdx(i);
      continue;
    }
    if(pe->case_val.expr.empty()) // default -- leave it
      continue;
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
      pe = dynamic_cast<CaseBlock*>(cases.FastEl(i));
      if(pe->case_val.expr == einm) break;
    }
    if(i==cases.size) {
      pe = new CaseBlock();
      pe->case_val.SetExpr(einm);
      cases.Insert(pe, ti);
    }
    else if (i != ti) {
      cases.MoveIdx(i, ti);
    }
  }
}

void Switch::CasesFmEnum_dyn() {
  if(TestError(!switch_var->dyn_enum_val.enum_type.ptr(), "CasesFmEnum", "switch_var dyn enum_type not set!"))
    return;
  DynEnumBase* et = switch_var->dyn_enum_val.enum_type.ptr();
  int i;  int ti;
  CaseBlock* pe;
  DynEnumItem* ei;
  // delete ones that no longer exist
  for (i = cases.size - 1; i >= 0; --i) {
    pe = dynamic_cast<CaseBlock*>(cases.FastEl(i));
    if(!pe) {
      cases.RemoveIdx(i);
      continue;
    }
    if(pe->case_val.expr.empty()) // default -- leave it
      continue;
    ei = et->enums.FindName(pe->case_val.expr);
    if(!ei) {
      cases.RemoveIdx(i);
    }
  }
  // add new ones and put in order
  for (ti = 0; ti < et->enums.size; ++ti) {
    ei = et->enums.FastEl(ti);
    for(i=0;i<cases.size;i++) {
      pe = dynamic_cast<CaseBlock*>(cases.FastEl(i));
      if(pe->case_val.expr == ei->name) break;
    }
    if(i==cases.size) {
      pe = new CaseBlock();
      pe->case_val.SetExpr(ei->name);
      cases.Insert(pe, ti);
    }
    else if (i != ti) {
      cases.MoveIdx(i, ti);
    }
  }
}

bool Switch::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
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
  if(cd.endsWith(';'))
    cd = cd.before(';',-1);
  if (!cd.contains(' ')) // if the prompt VAR NOT SET! hasn't been changed don't try to create var
    switch_var = FindVarNameInScope(cd, true); // prompt to make if not found
  return true;
}

bool Switch::BrowserEditTest() {
  bool rval = inherited::BrowserEditTest();
  bool rv2 = cases.BrowserEditTest();
  return rval && rv2;
}

bool Switch::ChooseMe() {
  Program* my_program = GET_MY_OWNER(Program);
  // have user choose variable on which to switch
  if (!switch_var && my_program->vars.size > 0) {
    taiWidgetTokenChooser* chooser =  new taiWidgetTokenChooser(&TA_ProgVar, NULL, NULL, NULL, 0, "");
    chooser->SetTitleText("Choose the switch variable");
    Program* scope_program = GET_MY_OWNER(Program);
    chooser->GetImageScoped(NULL, &TA_ProgVar, scope_program, &TA_Program); // scope to this guy
    bool okc = chooser->OpenChooser();
    if(okc && chooser->token()) {
      switch_var = (ProgVar*)chooser->token();
      UpdateAfterEdit();
    }
    delete chooser;
  }
  return true;
}

bool Switch::HasDefaultCase() {
  for (int i=0; i<cases.size; i++) {
    CaseBlock* cb = dynamic_cast<CaseBlock*>(cases[i]);
    if (cb->is_default) {
      return true;
    }
  }
  return false;
}

void Switch::AddCase() {
  CaseBlock* cb = new CaseBlock();
  cb->case_val.expr = "value";
  if (HasDefaultCase()) {
    cases.Insert(cb, cases.size - 1);
  }
  else {
    cases.Insert(cb, cases.size);
  }
}

void Switch::AddDefaultCase() {
  if (!HasDefaultCase()) {
    CaseBlock* cb = new CaseBlock();
    cb->is_default = true;
    cases.Insert(cb, cases.size);
  }
}

