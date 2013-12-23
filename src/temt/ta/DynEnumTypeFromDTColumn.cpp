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

#include "DynEnumTypeFromDTColumn.h"

#include <taBase>
#include <taMisc>
#include <DataTable>
#include <DataCol>
#include <taSmartRef>

void DynEnumTypeFromDTColumn::Initialize() {
  srcTable = NULL;
}

void DynEnumTypeFromDTColumn::InitLinks() {
  inherited::InitLinks();
  taBase::Own(srcTable,this);
}

void DynEnumTypeFromDTColumn::CutLinks() {
  srcTable.CutLinks();
  inherited::CutLinks();
}

void DynEnumTypeFromDTColumn::Copy_(const DynEnumTypeFromDTColumn& cp) {
  srcTable = cp.srcTable;
  srcColumn = cp.srcColumn;
}

bool DynEnumTypeFromDTColumn::EnumsFromDataTable(DataTable* dt, const Variant& col) {
  if(TestError(!dt, "SetTableAndColumn", "data table is null")) {
    enums.StructUpdate(true);
    enums.Reset();
    enums.StructUpdate(false);
    return false;
  }
  DataCol* da = dt->GetColData(col);
  if(TestError(!da, "SetTableAndColumn", "column not found")) {
    enums.StructUpdate(true);
    enums.Reset();
    enums.StructUpdate(false);
    return false;
  }
  srcTable = dt;
  srcColumn = da->name;

  enums.StructUpdate(true);
  enums.Reset();
  for(int i=0; i<dt->rows; i++) {
    String val = da->GetValAsString(i);
    AddEnum(val, i);
  }
  enums.StructUpdate(false);
  return true;
}

void DynEnumTypeFromDTColumn::SmartRef_SigDestroying(taSmartRef* ref, taBase* obj) {
  inherited::SmartRef_SigDestroying(ref, obj);
  srcTable = NULL;
  enums.Reset();
}

void DynEnumTypeFromDTColumn::SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
    int sls, void* op1_, void* op2_)
{
  // DataTable change - update the enums
  if(TestError(obj != srcTable, "SmartRef_SigEmit", "data table doesn't match the member value - should not happen - report bug"))
    return;
  DataTable* dt = (DataTable*)obj;
  DataCol* dc = dt->FindColName(srcColumn);

  if(TestError(!dc, "DynEnumTypeFromDTColumn::SmartRef_SigEmit", "column not found, did you change the column name? enum items will be deleted")) {
    enums.StructUpdate(true);
    enums.Reset();
    enums.StructUpdate(false);
    return;
  }

  enums.StructUpdate(true);
  enums.Reset();
  if (dc) {
    for(int i=0; i<dt->rows; i++) {
      String val = dc->GetValAsString(i);
      inherited::AddEnum(val, i);
    }
  }
  enums.StructUpdate(false);
}

void DynEnumTypeFromDTColumn::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // any illegal combination of table and column leads to removal of all enums
  if (srcTable && srcColumn != "") {
    EnumsFromDataTable(srcTable, srcColumn);
  }
  else {
    enums.StructUpdate(true);
    enums.Reset();
    enums.StructUpdate(false);
  }
}


