// Copyright 2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "SetUnitsLit.h"
#include <DataTable>
#include <Program>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(SetUnitsLit);

void SetUnitsLit::Initialize() {
  set_nm = true;
  offset = 0;
}

void SetUnitsLit::Destroy() {
  CutLinks();
}

void SetUnitsLit::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetInputDataVar();
}

void SetUnitsLit::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!input_data_var, quiet, rval, "input_data = NULL");
}

String SetUnitsLit::GetDisplayName() const {
  String rval = "set units lit: ";
  rval += String("unit_1=") + (unit_1.IsSet() ? unit_1.NameVal() : "?") + " ";
  rval += String("unit_2=") + (unit_2.IsSet() ? unit_2.NameVal() : "?") + " ";
  rval += String("unit_3=") + (unit_3.IsSet() ? unit_3.NameVal() : "?") + " ";
  rval += String("unit_4=") + (unit_4.IsSet() ? unit_4.NameVal() : "?") + " ";
  return rval;
}

bool SetUnitsLit::GetInputDataVar() {
  if(input_data_var) return true;
  Program* my_prog = program();
  if(!my_prog) return false;
  input_data_var = my_prog->args.FindVarType(ProgVar::T_Object, &TA_DataTable);
  if(input_data_var) return true;
  input_data_var = my_prog->vars.FindVarType(ProgVar::T_Object, &TA_DataTable);
  if(!input_data_var) {
    input_data_var = (ProgVar*)my_prog->args.New(1, &TA_ProgVar); // make an arg by default
    input_data_var->name = "input_data";
    input_data_var->var_type = ProgVar::T_Object;
    input_data_var->SigEmitUpdated();
  }
  return true;
}

bool SetUnitsLit::GenCss_OneUnit(Program* prog, DynEnum& un, const String& idnm, DataTable* idat) {
  int colno;
  if(un.IsSet()) {
    DynEnumBase* det = un.enum_type.ptr();
    colno = idat->FindColNameIdx(det->name, true);
    if(TestError(colno < 0, "GenCss",
                 "data table column:",det->name,"not found in input data table:",
                 idat->name)) return false;
    if(un.NumVal() >= 0) {      // could be neg
      String rval;
      rval += idnm + ".SetValAsFloatM(1.0, " + String(colno) + ", -1, ";
      if(offset != 0)
        rval += String(offset) + "+" + un.NameVal() + ");";
      else
        rval += un.NameVal() + ");";
      prog->AddLine(this, rval);
      if(set_nm) {
        colno = idat->FindColNameIdx("Name", true);
        if(colno >= 0) {
          String rval2 = "{ String nm = " + idnm + ".GetValAsString(" + String(colno)
            + ", -1); if(!nm.empty()) nm += \"_\"; nm += \"" + un.NameVal().after("_") + "\"; "
            + idnm + ".SetValAsString(nm, " + String(colno) + ", -1); }";
          prog->AddLine(this, rval2);
          // todo: add verbose
        }
      }
      return true;
    }
  }
  return false;
}

bool SetUnitsLit::GenCssBody_impl(Program* prog) {
  if(!input_data_var) {
    prog->AddLine(this, "// input_data_var not set!", ProgLine::MAIN_LINE);
    return false;
  }
  String idnm = input_data_var->name;
  DataTable* idat = (DataTable*)input_data_var->object_val.ptr();
  if(!idat) {
    prog->AddLine(this, "// input_data not set!", ProgLine::MAIN_LINE);
    return false;
  }
  prog->AddLine(this, "// " + GetDisplayName(), ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  GenCss_OneUnit(prog, unit_1, idnm, idat);
  GenCss_OneUnit(prog, unit_2, idnm, idat);
  GenCss_OneUnit(prog, unit_3, idnm, idat);
  GenCss_OneUnit(prog, unit_4, idnm, idat);
  return true;
}

bool SetUnitsLit::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase(); tbn.gsub("\n", " ");
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  return false;
}

bool SetUnitsLit::CvtFmCode(const String& code) {
  String remainder = code.after(":");
  if(remainder.empty()) return true;
    
  NameVar_PArray nv_pairs;
  taMisc::ToNameValuePairs(remainder, nv_pairs);
    
  for (int i=0; i<nv_pairs.size; i++) {
    String name = nv_pairs.FastEl(i).name;
    name.downcase();
    String value = nv_pairs.FastEl(i).value.toString();
    // taMisc::DebugInfo(name, value);
        
    if (name.startsWith("unit_1") && unit_1.IsSet()) {
      unit_1.SetNameVal(value);
    }
    else if (name.startsWith("unit_2") && unit_2.IsSet()) {
      unit_2.SetNameVal(value);
    }
    else if (name.startsWith("unit_3") && unit_3.IsSet()) {
      unit_3.SetNameVal(value);
    }
    else if (name.startsWith("unit_4") && unit_4.IsSet()) {
      unit_4.SetNameVal(value);
    }
  }
    
  SigEmitUpdated();
  return true;
}

