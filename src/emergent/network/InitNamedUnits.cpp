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

#include "InitNamedUnits.h"
#include <taProject>
#include <Network>
#include <GridTableView>
#include <T3Panel>
#include <DynEnumType>
#include <DataTable>

#include <tabMisc>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(InitNamedUnits);


void InitNamedUnits::Initialize() {
  n_lay_name_chars = 1;
  max_unit_chars = -1;
}

void InitNamedUnits::Destroy() {
  CutLinks();
}

void InitNamedUnits::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;

  GetInputDataVar();
  GetUnitNamesVar();
  GetNetworkVar();
  InitNamesTable();
}

// this is really all it does -- no actual code gen!!
void InitNamedUnits::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!input_data_var, quiet, rval, "input_data = NULL");
  CheckError(!unit_names_var, quiet, rval, "unit_names = NULL");
}

String InitNamedUnits::GetDisplayName() const {
  return "Init Named Units";
}

bool InitNamedUnits::GetInputDataVar() {
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
    input_data_var->object_type = &TA_DataTable;
    input_data_var->SigEmitUpdated();
  }
  return true;
}

bool InitNamedUnits::GetUnitNamesVar() {
  Program* my_prog = program();
  if (!my_prog) return false;
  if (!unit_names_var || (unit_names_var->name != "unit_names")) {
    unit_names_var = my_prog->vars.FindName("unit_names");
    if (!unit_names_var) {
      unit_names_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      unit_names_var->name = "unit_names";
      unit_names_var->SigEmitUpdated();
    }
  }
  unit_names_var->var_type = ProgVar::T_Object;

  if(!unit_names_var->object_val) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(!proj) return false;
    DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("InputData");
    DataTable* rval = dgp->FindName("UnitNames");
    if(!rval) {
      rval = dgp->NewEl(1, &TA_DataTable);
      rval->name = "UnitNames";
      taMisc::Info("Note: created new data table named:", rval->name, "in .data.InputData");
      rval->SigEmitUpdated();
      if(taMisc::gui_active) {
        tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe"); // todo: might be too radical.
      }
    }
    unit_names_var->object_val = rval;
    unit_names_var->object_type = &TA_DataTable;
    unit_names_var->SigEmitUpdated();
  }
  return true;
}

bool InitNamedUnits::GetNetworkVar() {
  if((bool)network_var) return true;
  Program* my_prog = program();
  if(!my_prog) return false;
  network_var = my_prog->args.FindName("network");
  if(!network_var) {
    network_var = my_prog->vars.FindName("network");
  }
  if(!network_var) return false;
  network_var->var_type = ProgVar::T_Object;
  network_var->SigEmitUpdated();
  return (bool)network_var->object_val;
}

void InitNamedUnits::GenCssBody_impl(Program* prog) {
  prog->AddLine(this, "// InitNamedUnits: no action taken", ProgLine::MAIN_LINE);
}

bool InitNamedUnits::InitNamesTable() {
  GetInputDataVar();
  if(TestError(!input_data_var->object_val.ptr(), "InitNamesTable", "could not find input_data table -- must set that up to point to your input data table"))
    return false;
  if(TestError(!GetUnitNamesVar(), "InitNamesTable", "could not find unit names data table -- this should not usually happen because it is auto-made if not found"))
    return false;

  bool rval = InitUnitNamesFmInputData((DataTable*)unit_names_var->object_val.ptr(),
                                       (DataTable*)input_data_var->object_val.ptr());
  return rval;
}

bool InitNamedUnits::InitUnitNamesFmInputData(DataTable* unit_names,
                                              const DataTable* input_data) {
  if(!unit_names || !input_data) {
    taMisc::Error("InitUnitNamesFmInputData", "null args -- should not happen -- report bug!");
    return false;
  }
  for(int i=0;i<input_data->cols();i++) {
    DataCol* idc = input_data->data.FastEl(i);
    if(!idc->isNumeric() || !idc->isMatrix()) continue;
    DataCol* ndc;
    if(idc->cell_dims() == 4)
      ndc = unit_names->FindMakeColMatrix(idc->name, VT_STRING,
                                          idc->cell_dims(),
                                          idc->GetCellGeom(0), idc->GetCellGeom(1),
                                          idc->GetCellGeom(2), idc->GetCellGeom(3));
    else
      ndc = unit_names->FindMakeColMatrix(idc->name, VT_STRING,
                                          idc->cell_dims(),
                                          idc->GetCellGeom(0), idc->GetCellGeom(1));
  }
  if(unit_names->rows <= 0)
    unit_names->AddBlankRow();          // only one row ever needed!
  return true;
}

bool InitNamedUnits::InitDynEnums() {
  Program* my_prog = program();
  if(!my_prog) return false;
  if(TestError(!GetUnitNamesVar(), "InitNamesTable", "could not find unit names data table -- this should not usually happen because it is auto-made if not found"))
    return false;

  DataTable* undt = (DataTable*)unit_names_var->object_val.ptr();
  if(!undt) return false;       // should not happen

  for(int i=0;i<undt->cols();i++) {
    DataCol* ndc = undt->data.FastEl(i);
    DynEnumType* det = (DynEnumType*)my_prog->types.FindName(ndc->name);
    if(!det) {
      det = (DynEnumType*)my_prog->types.New(1, &TA_DynEnumType);
      det->name = ndc->name;
    }
    String prefix = ndc->name.before(n_lay_name_chars);
    InitDynEnumFmUnitNames(det, ndc, prefix);
  }
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(&(my_prog->types), "BrowserExpandAll");
  }
  return true;
}

bool InitNamedUnits::InitDynEnumFmUnitNames(DynEnumType* dyn_enum,
                                            const DataCol* unit_names_col,
                                            const String& prefix) {
  if(!dyn_enum || !unit_names_col) {
    taMisc::Error("InitDynEnumFmUnitNames", "null args");
    return false;
  }
  dyn_enum->enums.Reset();
  int i;
  for(i=0;i<unit_names_col->cell_size();i++) {
    String cnm = unit_names_col->GetValAsStringM(-1, i);
    if(cnm.empty()) continue;
    cnm = taMisc::StringCVar(prefix + "_" + cnm); // make it legal!
    dyn_enum->AddEnum(cnm, i);
  }
  String cnm = dyn_enum->name;
  cnm = prefix + "_N" + cnm;
  dyn_enum->AddEnum(cnm, i);
  return true;
}

bool InitNamedUnits::LabelNetwork(bool propagate_names) {
  if(TestError(!GetUnitNamesVar(), "LabelNetwork", "could not find unit names data table -- this should not usually happen because it is auto-made if not found"))
    return false;
  if(TestError(!GetNetworkVar(), "LabelNetwork", "network variable is not set and could not find one -- please set and try again"))
    return false;

  DataTable* undt = (DataTable*)unit_names_var->object_val.ptr();
  if(!undt) return false;       // should not happen
  Network* net = (Network*)network_var->object_val.ptr();
  if(!net) return false;        // should not happen

  net->SetUnitNamesFromDataTable(undt, max_unit_chars, propagate_names);
  return true;
}

bool InitNamedUnits::ViewDataLegend() {
  if(!input_data_var) return false;
  DataTable* idat = (DataTable*)input_data_var->object_val.ptr();
  if(!idat) return false;
  if(!unit_names_var) return false;
  DataTable* ndat = (DataTable*)unit_names_var->object_val.ptr();
  if(!ndat) return false;

  idat->SetUserData("N_ROWS", 5);
  GridTableView* igtv = idat->NewGridView();
  T3Panel* frame = igtv->GetFrame();

  ndat->SetUserData("N_ROWS", 1);
  ndat->SetUserData("MAT_VAL_TEXT", 1);
  ndat->SetUserData("WIDTH", 3.3f);
  GridTableView* ngtv = ndat->NewGridView(frame);

  ngtv->main_xform.translate.y = 1.13f;
  ngtv->main_xform.scale = .33f;
  frame->Render();
  frame->ViewAll();
  // not sure we really want to do this
  //    fr->SaveCurView(0);
  return true;
}

