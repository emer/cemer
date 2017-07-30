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

#include "DataOpList.h"
#include <DataSrcDestProg>
#include <DataOpBaseSpec>
#include <DataTable>
#include <DataSelectEl>
#include <DataCol>
#include <taLeafItr>
#include <Completions>
#include <taMisc>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(DataOpList);


void DataOpList::SigEmit(int sls, void* op1, void* op2) {
  inherited::SigEmit(sls, op1, op2);
  DataSrcDestProg* own_prog = GET_MY_OWNER(DataSrcDestProg);
  if(own_prog && !own_prog->isDestroying())
    own_prog->UpdateSpecDataTable(); // will update col_lookups from data table
}

void DataOpList::SetDataTable(DataTable* dt) {
  for(int i=0;i<size;i++) {
    DataOpEl* ds = FastEl(i);
    ds->SetDataTable(dt);
  }
}

void DataOpList::GetColumns(DataTable* dt) {
  for(int i=0;i<size;i++) {
    DataOpEl* ds = FastEl(i);
    ds->GetColumns(dt);
  }
}

void DataOpList::ClearColumns() {
  for(int i=0;i<size;i++) {
    DataOpEl* ds = FastEl(i);
    ds->ClearColumns();
  }
}

DataOpEl* DataOpList::AddColumn(const String& col_name, DataTable* dt) {
  DataOpEl* dop = (DataOpEl*)New(1);
  dop->SetColName(col_name);
  dop->SetDataTable(dt);
  if (!dt) {
    ((DataSelectEl*)dop)->cmp.setType(Variant::T_Invalid);
  }
  else {
    if (dop->InheritsFrom(&TA_DataSelectEl)) {
      DataCol* data_col = dt->FindColName(col_name);
      if (data_col) {
        ((DataSelectEl*)dop)->cmp.setType(data_col->varType());
      }
      else {
        ((DataSelectEl*)dop)->cmp.setType(Variant::T_Invalid);
      }
    }
  }
  dop->UpdateAfterEdit();
  if(taMisc::gui_active)
    tabMisc::DelayedFunCall_gui(dop, "BrowserSelectMe");
  return dop;
}

void DataOpList::AddAllColumns(DataTable* dt, bool excl_strings) {
  if(!dt) return;
  for(int i=0;i<dt->data.size; i++) {
    DataCol* da = dt->data[i];
    if(excl_strings && da->isString()) continue;
    DataOpEl* dop = FindName(da->name);
    if(dop) continue;
    dop = (DataOpEl*)New(1);
    dop->SetColName(da->name);
    if(dop->InheritsFrom(&TA_DataSelectEl)) {
      ((DataSelectEl*)dop)->cmp.setType(da->varType());
    }
    dop->SigEmitUpdated();
  }
  SetDataTable(dt);
}

void DataOpList::AddAllColumns_gui(DataTable* dt, bool excl_strings) {
  if(!dt) return;
  AddAllColumns(dt, excl_strings);
  if(size > 0 && taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(FastEl(size-1), "BrowserSelectMe");
  }
}

String DataOpList::GetArgForCompletion(const String& method, const String& arg) {
  return "dt";
}

void DataOpList::GetArgCompletionList(const String& method, const String& arg, taBase* arg_obj, Completions& completions) {
  if (arg_obj) {
    if (arg_obj->InheritsFrom(&TA_DataTable)) {
      DataTable* table = (DataTable*)arg_obj;
      FOREACH_ELEM_IN_LIST(DataCol, col, table->data) {
        completions.object_completions.Link(col);
      }
    }
  }
}

