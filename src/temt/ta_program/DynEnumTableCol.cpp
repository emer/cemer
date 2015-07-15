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

#include "DynEnumTableCol.h"

#include <taBase>
#include <taMisc>
#include <DataTable>
#include <DataCol>
#include <taSmartRef>

TA_BASEFUNS_CTORS_DEFN(DynEnumTableCol);
SMARTREF_OF_CPP(DynEnumTableCol); // DynEnumTypeRef

void DynEnumTableCol::Initialize() {
  src_table = NULL;
}

void DynEnumTableCol::InitLinks() {
  inherited::InitLinks();
  taBase::Own(src_table,this);
}

void DynEnumTableCol::CutLinks() {
  src_table.CutLinks();
  inherited::CutLinks();
}

void DynEnumTableCol::Copy_(const DynEnumTableCol& cp) {
  src_table = cp.src_table;
  src_column = cp.src_column;
}

bool DynEnumTableCol::EnumsFromDataTable(DataTable* dt, const Variant& col) {
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
  src_table = dt;
  src_column = da->name;

  enums.StructUpdate(true);
  enums.Reset();
  for(int i=0; i<dt->rows; i++) {
    String val = da->GetValAsString(i);
    AddEnum(val, i);
  }
  enums.StructUpdate(false);
  return true;
}

void DynEnumTableCol::SmartRef_SigDestroying(taSmartRef* ref, taBase* obj) {
  inherited::SmartRef_SigDestroying(ref, obj);
  src_table = NULL;
  enums.Reset();
}

void DynEnumTableCol::SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
    int sls, void* op1_, void* op2_)
{
  // DataTable change - update the enums
  if(TestError(obj != src_table, "SmartRef_SigEmit", "data table doesn't match the member value - should not happen - report bug"))
    return;
  DataTable* dt = (DataTable*)obj;
  DataCol* dc = dt->FindColName(src_column);

  if(TestError(!dc, "DynEnumTableCol::SmartRef_SigEmit", "column not found, did you change the column name? enum items will be deleted")) {
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

void DynEnumTableCol::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // any illegal combination of table and column leads to removal of all enums
  if (src_table && src_column != "") {
    EnumsFromDataTable(src_table, src_column);
  }
  else {
    enums.StructUpdate(true);
    enums.Reset();
    enums.StructUpdate(false);
  }
}


