// Copyright 2014-2017, Regents of the University of Colorado,
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

#include "DataVarBase.h"
#include <DataCol>
#include <DataTable>
#include <Program>
#include <NameVar_PArray>

TA_BASEFUNS_CTORS_DEFN(DataVarBase);

void DataVarBase::Initialize() {
  row_spec = CUR_ROW;
  row_var = NULL;             // reset to null
}

void DataVarBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(row_spec == CUR_ROW)
    row_var = NULL;             // reset to null
}

DataVarBase::RowType DataVarBase::StringToRowType(const String& row_type) {
  if (row_type == "row_num" || row_type == "ROW_NUM")
    return DataVarBase::ROW_NUM;
  else if (row_type == "row_val" || row_type == "ROW_VAL")
    return DataVarBase::ROW_VAL;
  else
    return DataVarBase::CUR_ROW;
}