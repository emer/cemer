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

#include "DataTableCols.h"
#include <DataTable>

taTypeDef_Of(DataColT);

#include <SigLinkSignal>

void DataTableCols::Initialize() {
  SetBaseType(&TA_DataColT);
}

void DataTableCols::Copy_NoData(const DataTableCols& cp) {
  Reset();      // get rid of ours
  for(int i=0;i<cp.size; i++) {
    DataCol* sda = cp.FastEl(i);
    DataCol* nda = (DataCol*)sda->MakeToken();
    Add(nda);
    nda->Copy_NoData(*sda);
  }
}

void DataTableCols::CopyFromRow(int dest_row, const DataTableCols& src, int src_row) {
  int mx = MIN(size, src.size);
  for(int i=0; i< mx; i++) {
    DataCol* dar = FastEl(i);
    DataCol* sar = src.FastEl(i);
    dar->CopyFromRow(dest_row, *sar, src_row);
  }
}

void DataTableCols::SigEmit(int sls, void* op1, void* op2) {
  inherited::SigEmit(sls, op1, op2);

  if ((sls >= SLS_LIST_ORDER_MIN) && (sls <= SLS_LIST_ORDER_MAX)) {
    DataTable* dt = GET_MY_OWNER(DataTable); // cache
    if (!dt) return;
    // if last col removed then rows must become 0
    if (size == 0) {
      dt->rows = 0;
    }
    // treat changes here as Struct changes on the table
    dt->StructUpdate(true);
    dt->StructUpdate(false);
  }

}

String DataTableCols::GetColHeading(const KeyString& key) const {
  if (key == key_name) return "Col Name"; // override
  else if (key == DataCol::key_val_type) return "Data Type";
  else if (key == key_desc) return "Description";
  else return inherited::GetColHeading(key);
}

const KeyString DataTableCols::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_name;
  case 1: return DataCol::key_val_type;
  case 2: return key_desc;
  default: return _nilKeyString;
  }
}

