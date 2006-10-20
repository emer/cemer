// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_dataops.h"
#include "css_machine.h"

/////////////////////////////////////////////////////////
//   DataOpEl Base class
/////////////////////////////////////////////////////////

void DataOpEl::Initialize() {
  cols = NULL;
  column = NULL;
  col_idx = -1;
}

void DataOpEl::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  if(column) {
    col_name = column->name;
    taBase::SetPointer((taBase**)&column, NULL); // reset as soon as used -- just a temp guy!
  }
}

void DataOpList::SetDataTable(DataTable* dt) {
  for(int i=0;i<size;i++) {
    DataOpEl* ds = FastEl(i);
    ds->cols = &dt->data;
  }
}

void DataOpList::GetColumns(DataTable* dt) {
  for(int i=0;i<size;i++) {
    DataOpEl* ds = FastEl(i);
    DataArray_impl* da = dt->FindColName(ds->col_name, ds->col_idx);
    taBase::SetPointer((taBase**)&ds->column, da);
  }
}

void DataOpList::ClearColumns() {
  for(int i=0;i<size;i++) {
    DataOpEl* ds = FastEl(i);
    taBase::SetPointer((taBase**)&ds->column, NULL);
  }
}

/////////////////////////////////////////////////////////
//   Specific el's
/////////////////////////////////////////////////////////

void DataSortEl::Initialize() {
  order = ASCENDING;
}

void DataSelectEl::Initialize() {
  rel = EQUAL;
}

void DataGroupEl::Initialize() {
  op = MEAN_SEM;
}

/////////////////////////////////////////////////////////
//   taDataOps
/////////////////////////////////////////////////////////

bool taDataOps::CopyDataStru(DataTable* dest, DataTable* src) {
  // todo: this could be done to preserve structure in dest where possible, but
  // this gets the job done for now, but is very expensive in copying all data
  // so it must be replaced!!!
  dest->Reset();
  dest->CopyFrom(src);
  dest->RemoveAllRows();
}

bool taDataOps::Sort(DataTable* dest, DataTable* src, DataSortSpec* spec) {
  // just copy and operate on dest
  dest->Reset();
  dest->CopyFrom(src);
  Sort_impl(dest, spec);
}

void taDataOps::Sort_CopyRow(DataTable* dest, int dest_row, DataTable* src, int src_row) {
  // todo: only works on flat, ungrouped guys -- not too hard to extend to leaves
  for(int i=0;i<src->data.size;i++) {
    DataArray_impl* s_da = src->data.FastEl(i);
    DataArray_impl* d_da = dest->data.FastEl(i);
    if(s_da->is_matrix)
      d_da->SetValAsMatrix(s_da->GetValAsMatrix(src_row), dest_row);
    else
      d_da->SetValAsVar(s_da->GetValAsVar(src_row), dest_row);
  }
}

int taDataOps::Sort_Compare(DataTable* dt_a, int row_a, DataTable* dt_b, int row_b,
			    DataSortSpec* spec) {
  for(int i=0;i<spec->size; i++) {
    DataSortEl* ds = (DataSortEl*)spec->FastEl(i);
    if(!ds->column) continue;
    DataArray_impl* da_a = dt_a->data.FastEl(ds->col_idx);
    DataArray_impl* da_b = dt_b->data.FastEl(ds->col_idx);
    Variant va = da_a->GetValAsVar(row_a);
    Variant vb = da_b->GetValAsVar(row_b);
    if(va < vb) {
      if(ds->order == DataSortEl::ASCENDING) return -1; else return 1;
    }
    else if(va > vb) {
      if(ds->order == DataSortEl::ASCENDING) return 1; else return -1;
    }
  }
  return 0;			// must be all equal!
}

bool taDataOps::Sort_impl(DataTable* dt, DataSortSpec* spec) {
  if(dt->rows <= 1) return false;

  DataTable tmp;		// temporary buffer to hold vals during swap
  taBase::Ref(&tmp);		// keep it from getting trashed somewhere..
  CopyDataStru(&tmp, dt);	// give it same structure
  tmp.AddBlankRow();		// always just has one row

  spec->GetColumns(dt);		// cache column pointers & indicies from names

  // lets do a heap sort since it requires no secondary storage
  int n = dt->rows;
  int l,j,ir,i;

  l = (n >> 1) + 1;
  ir = n;
  for(;;){
    if(l>1) {
      Sort_CopyRow(&tmp, 0, dt, --l -1); // tmp = ra[--l]
    }
    else {
      Sort_CopyRow(&tmp, 0, dt, ir-1); // tmp = ra[ir]
      Sort_CopyRow(dt, ir-1, dt, 0); // ra[ir] = ra[1]
      if(--ir == 1) {
	Sort_CopyRow(dt, 0, &tmp, 0); // ra[1]=tmp
	break;
      }
    }
    i=l;
    j=l << 1;
    while(j<= ir) {
      if(j<ir && (Sort_Compare(dt, j-1, dt, j, spec) == -1)) j++; // less-than
      if(Sort_Compare(&tmp, 0, dt, j-1, spec) == -1) { // tmp < ra[j]
	Sort_CopyRow(dt, i-1, dt, j-1); // ra[i]=ra[j];
	j += (i=j);
      }
      else j = ir+1;
    }
    Sort_CopyRow(dt, i-1, &tmp, 0); // ra[i] = tmp;
  }

  spec->ClearColumns();
  return true;
}

bool taDataOps::Select(DataTable* dest, DataTable* src, DataSelectSpec* spec) {
  return true;
}

bool taDataOps::Group(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
  return true;
}

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

void DataProg::Initialize() {
}

void DataSortProg::Initialize() {
}

const String DataSortProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataSortProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataOps::Sort(dsp->dest_data, dsp->src_data, dsp->sort_spec);\n";
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval; 
}
