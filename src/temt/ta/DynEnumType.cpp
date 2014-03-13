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

#include "DynEnumType.h"
#include <Program>
#include <taProject>
#include <DataTable>

TA_BASEFUNS_CTORS_DEFN(DynEnumType);
SMARTREF_OF_CPP(DynEnumType); // DynEnumTypeRef

DynEnumItem* DynEnumType::NewEnum() {
  return (DynEnumItem*)enums.New_gui(1); // primarily a gui op
}

void DynEnumType::SeqNumberItems(int first_val) {
  if(bits) {
    int val = 1;
    for(int i=0;i<enums.size;i++) {
      DynEnumItem* it = enums.FastEl(i);
      it->value = val;
      it->SigEmitUpdated();
      val = val << 1;
    }
  }
  else {
    int val = first_val;
    for(int i=0;i<enums.size;i++) {
      DynEnumItem* it = enums.FastEl(i);
      it->value = val;
      it->SigEmitUpdated();
      val++;
    }
  }
}

bool DynEnumType::CopyToAllProgs() {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  FOREACH_ELEM_IN_GROUP(Program, pg, proj->programs) {
    DynEnumType* tp = (DynEnumType*)pg->types.FindName(name); // find my name
    if(!tp || tp == this || !tp->InheritsFrom(&TA_DynEnumType)) continue;
    tp->CopyFrom(this);
  }
  return true;
}

bool DynEnumType::EnumsFromDataTable(DataTable* dt, const Variant& col) {
  if(TestError(!dt, "EnumsFromDataTable", "data table is null")) return false;
  DataCol* da = dt->GetColData(col);
  if(!da) return false;
  enums.StructUpdate(true);
  enums.Reset();
  for(int i=0; i<dt->rows; i++) {
    String val = da->GetValAsString(i);
    AddEnum(val, i);
  }
  enums.StructUpdate(false);
  return true;
}
