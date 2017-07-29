// Copyright 2017, Regents of the University of Colorado,
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
#include <NameVar_PArray>
#include <taMisc>

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
  String rval = "Group from: ";
  
  if(src_data_var)
    rval += " src table = " + src_data_var->name;
  else
    rval += " src table = ?";
  
  if(dest_data_var)
    rval +=  " dest table = " + dest_data_var->name;
  else
    rval += " dest table = ?";

  return rval;
}

void DataGroupProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData()) {
    group_spec.CheckConfig(quiet, rval);
  }
}

void DataGroupProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var) {
    prog->AddLine(this, "// DataGroup: src_data_var not set!  cannot run!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataGroupProg* dsp = this" + GetPath(program()) + ";",
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

bool DataGroupProg::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  return CvtFmCodeCheckNames(code);
}

bool DataGroupProg::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String remainder = code.after(":");
  if(remainder.empty()) return true;
  
  NameVar_PArray nv_pairs;
  taMisc::ToNameValuePairs(remainder, nv_pairs);
  
  for (int i=0; i<nv_pairs.size; i++) {
    String name = nv_pairs.FastEl(i).name;
    name.downcase();
    String value = nv_pairs.FastEl(i).value.toString();
    
    if (name.startsWith("src tab") || name.startsWith("src_tab")) {
      src_data_var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("dest tab") || name.startsWith("dest_tab")) {
      dest_data_var = FindVarNameInScope(value, false); // don't make
    }
//    else if (name.startsWith("group")) {
//      group_spec = value;
//    }
  }
  
  SigEmitUpdated();
  return true;
}

