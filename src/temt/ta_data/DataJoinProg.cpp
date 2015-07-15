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

#include "DataJoinProg.h"
#include <Program>
#include <DataTable>
#include <NameVar_PArray>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataJoinProg);


void DataJoinProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  join_spec.name = "join_spec";
  UpdateSpecDataTable();
}

void DataJoinProg::UpdateSpecDataTable() {
  join_spec.SetDataTable(GetSrcData(), GetSrcBData());
}

void DataJoinProg::Initialize() {
}

String DataJoinProg::GetDisplayName() const {
//  String rval = join_spec.GetDisplayName();
  
  String rval = "Join tables: ";
  if(src_data_var)
    rval += " src_a = " + src_data_var->name;
  else
    rval +=  "src_a = ? ";
  
  if (src_b_data_var)
    rval += " src_b = " + src_b_data_var->name;
  else
    rval +=  "src_b = ? ";
  
  if(dest_data_var)
    rval += " dest table = " + dest_data_var->name;
  else
    rval += " dest table = ? ";

  return rval;
}

void DataJoinProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!src_b_data_var, quiet, rval, "src_b_data_var is NULL")) return; // fatal
  // should be done by var, not us
//   CheckError(!src_b_data_var->object_val, quiet, rval, "src_data_var variable NULL");
  CheckError(src_b_data_var->object_type != &TA_DataTable, quiet, rval,
             "src_b_data_var variable does not point to a DataTable object");
}

void DataJoinProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData() && GetSrcBData()) {
    join_spec.GetColumns(GetSrcData(), GetSrcBData());
    join_spec.CheckConfig(quiet, rval);
    join_spec.ClearColumns();
  }
}

DataTable* DataJoinProg::GetSrcBData() {
  if(!src_b_data_var) return NULL;
  if(src_b_data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)src_b_data_var->object_val.ptr();
}

void DataJoinProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var || !src_b_data_var) {
    prog->AddLine(this, "// DataJoin: src_data_var or src_b_data_var not set!  cannot run",
                  ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataJoinProg* dsp = this" + GetPath(NULL, program()) + ";",
                ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, "taDataProc::Join(" + dest_data_var->name + ", " +
                  src_data_var->name + ", " + src_b_data_var->name + ", dsp->join_spec);");
  }
  else {
    prog->AddLine(this, "taDataProc::Join(NULL, " +
                  src_data_var->name + ", " + src_b_data_var->name + ", dsp->join_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
}

bool DataJoinProg::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  if(dc.startsWith("join")) return true;
  return false;
}

bool DataJoinProg::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String remainder = code.after(":");
  if(remainder.empty()) return true;
  
  NameVar_PArray nv_pairs;
  taMisc::ToNameValuePairs(remainder, nv_pairs);
  
  for (int i=0; i<nv_pairs.size; i++) {
    String name = nv_pairs.FastEl(i).name;
    name.downcase();
    String value = nv_pairs.FastEl(i).value.toString();
    
    if (name.startsWith("src a") || name.startsWith("src_a")) {
      src_data_var = FindVarNameInScope(value, false); // don't make
    }
    if (name.startsWith("src b") || name.startsWith("src_b")) {
      src_b_data_var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("dest")) {
      dest_data_var = FindVarNameInScope(value, false); // don't make
    }
  }
  
  SigEmitUpdated();
  return true;
}

