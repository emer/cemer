// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "SetUnitsVar.h"
#include <DataTable>

#include <taMisc>

void SetUnitsVar::Initialize() {
  set_nm = true;
}

void SetUnitsVar::Destroy() {
  CutLinks();
}

void SetUnitsVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetInputDataVar();
}

void SetUnitsVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!input_data_var, quiet, rval, "input_data = NULL");
  if(!CheckError((bool)unit_1 && unit_1->var_type != ProgVar::T_DynEnum, quiet, rval,
                 "unit_1 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError((bool)unit_1 && !unit_1->dyn_enum_val.enum_type, quiet, rval,
               "unit_1 does not have enum_type set -- must be set to one associated with layer unit names!");
  if(!CheckError((bool)unit_2 && unit_2->var_type != ProgVar::T_DynEnum, quiet, rval,
                 "unit_2 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError((bool)unit_2 && !unit_2->dyn_enum_val.enum_type, quiet, rval,
               "unit_2 does not have enum_type set -- must be set to one associated with layer unit names!");
  if(!CheckError((bool)unit_3 && unit_3->var_type != ProgVar::T_DynEnum, quiet, rval,
                 "unit_3 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError((bool)unit_3 && !unit_3->dyn_enum_val.enum_type, quiet, rval,
               "unit_3 does not have enum_type set -- must be set to one associated with layer unit names!");
  if(!CheckError((bool)unit_4 && unit_4->var_type != ProgVar::T_DynEnum, quiet, rval,
                 "unit_4 is not a DynEnum variable -- must be one associated with layer unit names!"))
    CheckError((bool)unit_4 && !unit_4->dyn_enum_val.enum_type, quiet, rval,
               "unit_4 does not have enum_type set -- must be set to one associated with layer unit names!");
}

String SetUnitsVar::GetDisplayName() const {
  String rval = "Set Units Vars: ";
  if(unit_1) rval += unit_1->name + " ";
  if(unit_2) rval += unit_2->name + " ";
  if(unit_3) rval += unit_3->name + " ";
  if(unit_4) rval += unit_4->name + " ";
  return rval;
}

bool SetUnitsVar::GetInputDataVar() {
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

bool SetUnitsVar::GenCss_OneUnit(Program* prog, ProgVarRef& un, const String& idnm, DataTable* idat) {
  int colno;
  if((bool)un && (bool)un->dyn_enum_val.enum_type) {
    DynEnumBase* det = un->dyn_enum_val.enum_type.ptr();
    colno = idat->FindColNameIdx(det->name, true);
    if(TestError(colno < 0, "GenCss",
                 "data table column:",det->name,"not found in input data table:",
                 idat->name)) return false;
    // if var has ability to go negative, check..
    bool neg_chk = false;
    if(det->enums.SafeEl(0) && det->enums.SafeEl(0)->value < 0) {
      prog->AddLine(this, "if(" + un->name + " >= 0) {");
      prog->IncIndent();
      neg_chk = true;
    }
    String rval;
    rval += idnm + ".SetValAsFloatM(1.0, " + String(colno) + ", -1, ";
    if(offset)
      rval += offset->name + "+" + un->name + ");";
    else
      rval += un->name + ");";
    prog->AddLine(this, rval);
    if(set_nm) {
      colno = idat->FindColNameIdx("Name", true);
      if(colno >= 0) {
        String rval2 = "{ String nm = " + idnm + ".GetValAsString(" + String(colno)
          + ", -1); if(!nm.empty()) nm += \"_\"; String nwnm = " + un->name
          + "; nwnm = nwnm.after(\"_\"); nm += nwnm; "
          + idnm + ".SetValAsString(nm, " + String(colno) + ", -1); }";
        prog->AddLine(this, rval2);
      }
    }
    if(neg_chk) {
      prog->DecIndent();
      prog->AddLine(this, "}");
    }
    return true;
  }
  return false;
}

void SetUnitsVar::GenCssBody_impl(Program* prog) {
  if(!input_data_var) {
    prog->AddLine(this, "// input_data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  String idnm = input_data_var->name;
  DataTable* idat = (DataTable*)input_data_var->object_val.ptr();
  if(!idat) {
    prog->AddLine(this, "// input_data not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "// " + GetDisplayName(), ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  GenCss_OneUnit(prog, unit_1, idnm, idat);
  GenCss_OneUnit(prog, unit_2, idnm, idat);
  GenCss_OneUnit(prog, unit_3, idnm, idat);
  GenCss_OneUnit(prog, unit_4, idnm, idat);
}

