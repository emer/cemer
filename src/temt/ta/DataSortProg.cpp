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

#include "DataSortProg.h"
#include <Program>

TA_BASEFUNS_CTORS_DEFN(DataSortProg);


void DataSortProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sort_spec.name = "sort_spec";
  UpdateSpecDataTable();
}

void DataSortProg::UpdateSpecDataTable() {
  sort_spec.SetDataTable(GetSrcData());
}

void DataSortProg::Initialize() {
}

String DataSortProg::GetDisplayName() const {
  String rval = "Sort ";
  if(src_data_var) {
    rval += " from: " + src_data_var->name;
  }
  if(dest_data_var) {
    rval += " to: " + dest_data_var->name;
  }
  return rval;
}

void DataSortProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData()) {
    sort_spec.GetColumns(GetSrcData());
    sort_spec.CheckConfig(quiet, rval);
    sort_spec.ClearColumns();
  }
}

void DataSortProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var) {
    prog->AddLine(this, "// DataSort: src_data_var not set!  cannot run", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataSortProg* dsp = this" + GetPath(NULL, program()) + ";", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, String("taDataProc::Sort(") + dest_data_var->name + ", " + src_data_var->name
                  + ", dsp->sort_spec);");
  }
  else {
    prog->AddLine(this, String("taDataProc::Sort(NULL, ") + src_data_var->name + ", dsp->sort_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void DataSortProg::AddAllColumns() {
  sort_spec.AddAllColumns(GetSrcData());
}
