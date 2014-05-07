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

#include "ControlPanelsFmArgs.h"
#include <Program>
#include <ProgVar>
#include <ControlPanel>
#include <EditMbrItem>
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
  ControlPanel* se = GetControlPanel();
  if(se) {
    rval += "var = " + se->name + " ";
  }
  else {
    rval += "var = ? ";
  }
  return rval;
}

bool ControlPanelsFmArgs::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
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

void ControlPanelsFmArgs::GenCssBody_impl(Program* prog) {
  ControlPanel* se = GetControlPanel();
  if(!se) {
    prog->AddLine(this, "// ControlPanelsFmArgs: ctrl_panel_var not set!", ProgLine::MAIN_LINE);
    return;
  }

  prog->AddLine(this, "{ // ControlPanelsFmArgs fm: " + se->name, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, "String sefma_lbl, sefma_argval;");
  prog->AddLine(this, "for(int j=0;j<" + ctrl_panel_var->name + ".mbrs.leaves;j++) {");
  prog->IncIndent();
  prog->AddLine(this, "EditMbrItem* sei = " + ctrl_panel_var->name + ".mbrs.Leaf(j);");
  prog->AddLine(this, "if(!sei->is_single) continue;");
  prog->AddLine(this, "sefma_lbl = sei->label;");
  prog->AddLine(this, "sefma_argval = taMisc::FindArgByName(sefma_lbl);");
  prog->AddLine(this, "if(sefma_argval.empty()) continue;");
  prog->AddLine(this, "sei->PSearchCurVal_Set(sefma_argval);");
  if(taMisc::dmem_proc == 0) {
    prog->AddLine(this, String("taMisc::Info(\"Set ctrl panel item: \",sefma_lbl,\" in ctrl panel: \",\"") +
                  se->name + "\",\"to val:\",sefma_argval);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void ControlPanelsFmArgs::GenRegArgs(Program* prog) {
  ControlPanel* se = GetControlPanel();
  if(se) {
    for(int j=0;j<se->mbrs.leaves;j++) {
      EditMbrItem* sei = se->mbrs.Leaf(j);
      if(!sei->is_numeric) continue;
      prog->AddLine(this, "taMisc::AddEqualsArgName(\"" + sei->label + "\");");
      prog->AddLine(this, "taMisc::AddArgNameDesc(\"" + sei->label
                    + "\", \"ControlPanelsFmArgs: ctrl_panel = " + se->name + "\");");
    }
  }
}

