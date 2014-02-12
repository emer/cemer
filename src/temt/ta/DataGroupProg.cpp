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

#include "DataGroupProg.h"
#include <Program>

TA_BASEFUNS_CTORS_DEFN(DataGroupProg);


void DataGroupProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  group_spec.name = "group_spec";
  UpdateSpecDataTable();
}

void DataGroupProg::UpdateSpecDataTable() {
  group_spec.SetDataTable(GetSrcData());
}

void DataGroupProg::Initialize() {
}

String DataGroupProg::GetDisplayName() const {
  String rval = "Group ";
  if(src_data_var) {
    rval += " from: " + src_data_var->name;
  }
  if(dest_data_var) {
    rval += " to: " + dest_data_var->name;
  }
  return rval;
}

void DataGroupProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData()) {
    group_spec.GetColumns(GetSrcData());
    group_spec.CheckConfig(quiet, rval);
    group_spec.ClearColumns();
  }
}

void DataGroupProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var) {
    prog->AddLine(this, "// DataGroup: src_data_var not set!  cannot run!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataGroupProg* dsp = this" + GetPath(NULL, program()) + ";",
                ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, "taDataProc::Group(" + dest_data_var->name + ", " + src_data_var->name
                  + ", dsp->group_spec);");
  }
  else {
    prog->AddLine(this, "taDataProc::Group(NULL, " + src_data_var->name
                  + ", dsp->group_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void DataGroupProg::AddAllColumns() {
  group_spec.AddAllColumns(GetSrcData());
}
