// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "ControlPanelsFmArgs.h"
#include <Program>
#include <ProgVar>
#include <ControlPanel>
#include <ControlPanelMember>
#include <NameVar_PArray>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ControlPanelsFmArgs);


void ControlPanelsFmArgs::Initialize() {
}

void ControlPanelsFmArgs::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void ControlPanelsFmArgs::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!ctrl_panel_var, quiet, rval, "ctrl_panel_var is NULL")) return; // fatal
  CheckError(!ctrl_panel_var->object_type ||
	     !ctrl_panel_var->object_type->InheritsFrom(&TA_ControlPanel), quiet, rval,
             "ctrl_panel_var variable does not point to a ControlPanel object");
}

String ControlPanelsFmArgs::GetDisplayName() const {
  String rval = "CtrlPanel Fm Args: ";
  // ControlPanel* se = GetControlPanel();
  if(ctrl_panel_var) {
    rval += "var = " + ctrl_panel_var->name + " ";
  }
  else {
    rval += "var = ? ";
  }
  return rval;
}

bool ControlPanelsFmArgs::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if (CvtFmCodeCheckNames(code))
    return true;

  String dc = code;  dc.downcase();
  if(dc.startsWith("ctrlpan") || dc.startsWith("ctrl pan") || dc.startsWith("ctrl="))
    return true;
  return false;
}

bool ControlPanelsFmArgs::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String remainder = code.after(":");
  if(remainder.empty()) return true;
  
  NameVar_PArray nv_pairs;
  taMisc::ToNameValuePairs(remainder, nv_pairs);
  
  for (int i=0; i<nv_pairs.size; i++) {
    String name = nv_pairs.FastEl(i).name;
    name.downcase();
    String value = nv_pairs.FastEl(i).value.toString();
    
    if (name.startsWith("var") || name.startsWith("ctrl_pan")) {
      ctrl_panel_var = FindVarNameInScope(value, false); // don't make
    }
  }
  
  SigEmitUpdated();
  return true;
}

ControlPanel* ControlPanelsFmArgs::GetControlPanel() const {
  if(!ctrl_panel_var) return NULL;
  if(!ctrl_panel_var->object_type ||
     !ctrl_panel_var->object_type->InheritsFrom(&TA_ControlPanel)) return NULL;
  return (ControlPanel*)ctrl_panel_var->object_val.ptr();
}

bool ControlPanelsFmArgs::GenCssBody_impl(Program* prog) {
  ControlPanel* cpan = GetControlPanel();
  if(!cpan) {
    prog->AddLine(this, "// ControlPanelsFmArgs: ctrl_panel_var not set!", ProgLine::MAIN_LINE);
    return false;
  }

  prog->AddLine(this, ctrl_panel_var->name + ".SetMembersFromArgs();");
  prog->AddVerboseLine(this);
  return true;
}

void ControlPanelsFmArgs::GenRegArgs(Program* prog) {
  ControlPanel* cpan = GetControlPanel();
  if(cpan) {
    prog->AddLine(this, ctrl_panel_var->name + ".AddMembersAsArgs();");
  }
}

