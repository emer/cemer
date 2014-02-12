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

#include "SelectEditsFmArgs.h"
#include <Program>
#include <ProgVar>
#include <SelectEdit>
#include <EditMbrItem>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(SelectEditsFmArgs);


void SelectEditsFmArgs::Initialize() {
}

void SelectEditsFmArgs::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void SelectEditsFmArgs::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!sel_edit_var, quiet, rval, "sel_edit_var is NULL")) return; // fatal
  CheckError(!sel_edit_var->object_type ||
	     !sel_edit_var->object_type->InheritsFrom(&TA_SelectEdit), quiet, rval,
             "sel_edit_var variable does not point to a SelectEdit object");
}

String SelectEditsFmArgs::GetDisplayName() const {
  String rval = "Select Edits Fm Args";
  SelectEdit* se = GetSelectEdit();
  if(se) {
    rval += " To: " + se->name;
  }
  return rval;
}

SelectEdit* SelectEditsFmArgs::GetSelectEdit() const {
  if(!sel_edit_var) return NULL;
  if(!sel_edit_var->object_type ||
     !sel_edit_var->object_type->InheritsFrom(&TA_SelectEdit)) return NULL;
  return (SelectEdit*)sel_edit_var->object_val.ptr();
}

void SelectEditsFmArgs::GenCssBody_impl(Program* prog) {
  SelectEdit* se = GetSelectEdit();
  if(!se) {
    prog->AddLine(this, "// SelectEditsFmArgs: sel_edit_var not set!", ProgLine::MAIN_LINE);
    return;
  }

  prog->AddLine(this, "{ // SelectEditsFmArgs fm: " + se->name, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, "String sefma_lbl, sefma_argval;");
  prog->AddLine(this, "for(int j=0;j<" + sel_edit_var->name + ".mbrs.leaves;j++) {");
  prog->IncIndent();
  prog->AddLine(this, "EditMbrItem* sei = " + sel_edit_var->name + ".mbrs.Leaf(j);");
  prog->AddLine(this, "if(!sei->is_single) continue;");
  prog->AddLine(this, "sefma_lbl = sei->label;");
  prog->AddLine(this, "sefma_argval = taMisc::FindArgByName(sefma_lbl);");
  prog->AddLine(this, "if(sefma_argval.empty()) continue;");
  prog->AddLine(this, "sei->PSearchCurVal_Set(sefma_argval);");
  if(taMisc::dmem_proc == 0) {
    prog->AddLine(this, String("taMisc::Info(\"Set select edit item: \",sefma_lbl,\" in select edit: \",\"") +
                  se->name + "\",\"to val:\",sefma_argval);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void SelectEditsFmArgs::GenRegArgs(Program* prog) {
  SelectEdit* se = GetSelectEdit();
  if(se) {
    for(int j=0;j<se->mbrs.leaves;j++) {
      EditMbrItem* sei = se->mbrs.Leaf(j);
      if(!sei->is_numeric) continue;
      prog->AddLine(this, "taMisc::AddEqualsArgName(\"" + sei->label + "\");");
      prog->AddLine(this, "taMisc::AddArgNameDesc(\"" + sei->label
                    + "\", \"SelectEditsFmArgs: sel_edit = " + se->name + "\");");
    }
  }
}

