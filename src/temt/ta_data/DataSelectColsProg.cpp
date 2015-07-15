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

#include "DataSelectColsProg.h"
#include <Program>
#include <NameVar_PArray>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataSelectColsProg);


void DataSelectColsProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  select_spec.name = "select_spec";
  UpdateSpecDataTable();
}

void DataSelectColsProg::UpdateSpecDataTable() {
  select_spec.SetDataTable(GetSrcData());
}

void DataSelectColsProg::Initialize() {
}

String DataSelectColsProg::GetDisplayName() const {
  String rval = "SelectCols: ";
  
  if(src_data_var)
    rval += " src table = " + src_data_var->name;
  else
    rval += " src table = ? ";
  
  if(dest_data_var)
    rval +=  " dest table = " + dest_data_var->name;
  else
    rval += " dest table = ? ";
  return rval;
}

bool DataSelectColsProg::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  dc.gsub(" ", "");
  if(dc.startsWith("selectcol")) return true;
  return false;
}

bool DataSelectColsProg::CvtFmCode(const String& code) {
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
  }
  
  SigEmitUpdated();
  return true;
}

void DataSelectColsProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData()) {
    select_spec.GetColumns(GetSrcData());
    select_spec.CheckConfig(quiet, rval);
    select_spec.ClearColumns();
  }
}

void DataSelectColsProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var) {
    prog->AddLine(this, "// DataSelectCols: src_data_var not set!  cannot run!",
                  ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataSelectColsProg* dsp = this" + GetPath(NULL, program()) + ";",
                ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, "taDataProc::SelectCols(" + dest_data_var->name + ", " + src_data_var->name
                  + ", dsp->select_spec);");
  }
  else {
    prog->AddLine(this, "taDataProc::SelectCols(NULL, " + src_data_var->name
                  + ", dsp->select_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void DataSelectColsProg::AddAllColumns() {
  select_spec.AddAllColumns(GetSrcData());
}
