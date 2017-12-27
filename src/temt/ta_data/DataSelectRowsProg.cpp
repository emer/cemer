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

#include "DataSelectRowsProg.h"
#include <Program>
#include <NameVar_PArray>
#include <taMisc>
#include <MemberDef>
#include <DataCol>
#include <DataTable>
#include <Completions>


TA_BASEFUNS_CTORS_DEFN(DataSelectRowsProg);

void DataSelectRowsProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  select_spec.name = "select_spec";
  UpdateSpecDataTable();
}

void DataSelectRowsProg::UpdateSpecDataTable() {
  select_spec.SetDataTable(GetSrcData());
}

void DataSelectRowsProg::Initialize() {
}

String DataSelectRowsProg::GetDisplayName() const {
  String rval = "SelectRows: ";
  
  if(src_data_var)
    rval += " src table = " + src_data_var->name + " ";
  else
    rval += " src table = ? ";
  
  if(dest_data_var)
    rval +=  " dest table = " + dest_data_var->name + " ";
  else
    rval += " dest table = ? ";

  rval += " comb_op = " + TA_Relation.GetEnumLabel("CombOp", select_spec.comb_op);
  return rval;
}

bool DataSelectRowsProg::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if (CvtFmCodeCheckNames(code))
    return true;
  
  String dc = code;  dc.downcase();
  dc.gsub(" ", ""); 
  if(dc.startsWith("selectrow")) return true;
  return false;
}

bool DataSelectRowsProg::CvtFmCode(const String& code) {
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
    else if (name.startsWith("comb")) {
      MemberDef* opmd = TA_DataSelectSpec.members.FindName("comb_op");
      if(opmd) {
        opmd->SetValStr(value, (void*)&select_spec);
      }
    }
  }
  
  SigEmitUpdated();
  return true;
}

void DataSelectRowsProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData()) {
    select_spec.GetColumns(GetSrcData());
    select_spec.CheckConfig(quiet, rval);
    select_spec.ClearColumns();
  }
}

bool DataSelectRowsProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var) {
    prog->AddLine(this, "// DataSelectRows: src_data_var not set!  cannot run!", ProgLine::MAIN_LINE);
    return false;
  }
  prog->AddLine(this, "{ DataSelectRowsProg* dsp = this" + GetPath(program()) + ";",
                ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, "taDataProc::SelectRows(" + dest_data_var->name + ", " +
                  src_data_var->name + ", dsp->select_spec);");
  }
  else {
    prog->AddLine(this, "taDataProc::SelectRows(NULL, " +
                  src_data_var->name + ", dsp->select_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  return true;
}

void DataSelectRowsProg::AddAllColumns() {
  select_spec.AddAllColumns(GetSrcData());
}

String DataSelectRowsProg::GetArgForCompletion(const String& method, const String& arg) {
  return "dt";
}

void DataSelectRowsProg::GetArgCompletionList(const String& method, const String& arg, taBase* arg_obj, const String& cur_txt, Completions& completions) {
  if (arg_obj) {
    if (arg_obj->InheritsFrom(&TA_DataTable)) {
      DataTable* table = (DataTable*)arg_obj;
      FOREACH_ELEM_IN_LIST(DataCol, col, table->data) {
        completions.object_completions.Link(col);
      }
    }
  }
}

