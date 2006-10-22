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

String DataOpEl::GetDisplayName() const {
  return col_name;
}

void DataOpEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(col_name.empty()) {
    if(!quiet) taMisc::CheckError("Error in DataOpEl:",GetPath(),
				  "col_name is empty");
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
    if(ds->col_idx < 0) da = NULL;
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

String DataSortEl::GetDisplayName() const {
  String rval = col_name + " ";
  if(order == ASCENDING) 
    rval += "up";
  else
    rval += "dn";
  return rval;
}

void DataSelectEl::Initialize() {
  rel = EQUAL;
}

String DataSelectEl::GetDisplayName() const {
  return col_name + " " + 
    GetTypeDef()->GetEnumString("Relation", rel)+ " " +
    cmp.toString();
}

bool DataSelectEl::Eval(const Variant& val) {
  switch(rel) {
  case EQUAL:
    return val == cmp;
  case NOTEQUAL:
    return val != cmp;
  case LESSTHAN:
    return val < cmp;
  case GREATERTHAN:
    return val > cmp;
  case LESSTHANOREQUAL:
    return val <= cmp;
  case GREATERTHANOREQUAL:
    return val >= cmp;
  case CONTAINS:
    return val.toString().contains(cmp.toString());
  case NOT_CONTAINS:
    return !val.toString().contains(cmp.toString());
  }
  return false;
}

String DataSelectSpec::GetDisplayName() const {
  return inherited::GetDisplayName() + " " +
    GetTypeDef()->GetEnumString("CombOp", comb_op);
}

void DataGroupEl::Initialize() {
}

String DataGroupEl::GetDisplayName() const {
  return col_name + " " + agg.GetAggName();
}

void DataGroupSpec::Initialize() {
  SetBaseType(&TA_DataGroupEl);
}

/////////////////////////////////////////////////////////
//   taDataOps
/////////////////////////////////////////////////////////

bool taDataOps::Sort(DataTable* dest, DataTable* src, DataSortSpec* spec) {
  // just copy and operate on dest
  dest->Reset();
  *dest = *src;
  Sort_impl(dest, spec);
}

int taDataOps::Sort_Compare(DataTable* dt_a, int row_a, DataTable* dt_b, int row_b,
			    DataSortSpec* spec) {
  for(int i=0;i<spec->size; i++) {
    DataSortEl* ds = (DataSortEl*)spec->FastEl(i);
    if(ds->col_idx < 0) continue;
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

  dt->StructUpdate(true);
  DataTable tmp_data;		// temporary buffer to hold vals during swap
  taBase::Own(tmp_data, NULL);	// activates initlinks..
  tmp_data.Copy_NoData(*dt);		// give it same structure
  tmp_data.AddBlankRow();		// always just has one row

  spec->GetColumns(dt);		// cache column pointers & indicies from names

  // lets do a heap sort since it requires no secondary storage
  int n = dt->rows;
  int l,j,ir,i;

  l = (n >> 1) + 1;
  ir = n;
  for(;;){
    if(l>1) {
      tmp_data.CopyFromRow(0, *dt, --l -1); // tmp_data = ra[--l]
    }
    else {
      tmp_data.CopyFromRow(0, *dt, ir-1); // tmp_data = ra[ir]
      dt->CopyFromRow(ir-1, *dt, 0); // ra[ir] = ra[1]
      if(--ir == 1) {
	dt->CopyFromRow(0, tmp_data, 0); // ra[1]=tmp_data
	break;
      }
    }
    i=l;
    j=l << 1;
    while(j<= ir) {
      if(j<ir && (Sort_Compare(dt, j-1, dt, j, spec) == -1)) j++; // less-than
      if(Sort_Compare(&tmp_data, 0, dt, j-1, spec) == -1) { // tmp_data < ra[j]
	dt->CopyFromRow(i-1, *dt, j-1); // ra[i]=ra[j];
	j += (i=j);
      }
      else j = ir+1;
    }
    dt->CopyFromRow(i-1, tmp_data, 0); // ra[i] = tmp_data;
  }

  dt->StructUpdate(false);
  spec->ClearColumns();
  return true;
}

bool taDataOps::SelectRows(DataTable* dest, DataTable* src, DataSelectSpec* spec) {
  dest->StructUpdate(true);
  dest->Copy_NoData(*src);		// give it same structure
  spec->GetColumns(src);		// cache column pointers & indicies from names
  for(int row=0;row<src->rows; row++) {
    bool incl = false;
    bool not_incl = false;
    for(int i=0; i<spec->size; i++) {
      DataSelectEl* ds = (DataSelectEl*)spec->FastEl(i);
      if(ds->col_idx < 0) continue;
      DataArray_impl* da = src->data.FastEl(ds->col_idx);
      Variant val = da->GetValAsVar(row);
      bool ev = ds->Eval(val);
      if(spec->comb_op == DataSelectSpec::AND) {
	if(!ev) { not_incl = true;  break; }
      }
      else if(spec->comb_op == DataSelectSpec::OR) {
	if(ev) { incl = true; break; }
      }
      else if(spec->comb_op == DataSelectSpec::NOT_AND) {
	if(ev) { not_incl = true; break; }
      }
      else if(spec->comb_op == DataSelectSpec::NOT_OR) {
	if(!ev) { incl = true; break; }
      }
    }
    if(((spec->comb_op == DataSelectSpec::AND) || (spec->comb_op == DataSelectSpec::NOT_AND))
       && not_incl) continue;
    if(((spec->comb_op == DataSelectSpec::OR) || (spec->comb_op == DataSelectSpec::NOT_OR))
       && !incl) continue;
    // continuing now..
    dest->AddBlankRow();
    dest->CopyFromRow(-1, *src, row);
  }
  dest->StructUpdate(false);
  spec->ClearColumns();
  return true;
}

bool taDataOps::SelectCols(DataTable* dest, DataTable* src, DataOpList* spec) {
  dest->StructUpdate(true);
  spec->GetColumns(src);		// cache column pointers & indicies from names
  dest->Reset();
  for(int i=0;i<spec->size; i++) {
    DataOpEl* ds = spec->FastEl(i);
    if(ds->col_idx < 0) continue;
    DataArray_impl* sda = src->data.FastEl(ds->col_idx);
    DataArray_impl* nda = (DataArray_impl*)sda->MakeToken();
    dest->data.Add(nda);
    nda->Copy_NoData(*sda);
  }    
  for(int row=0;row<src->rows;row++) {
    dest->AddBlankRow();
    for(int i=0;i<spec->size; i++) {
      DataOpEl* ds = spec->FastEl(i);
      if(ds->col_idx < 0) continue;
      DataArray_impl* sda = src->data.FastEl(ds->col_idx);
      DataArray_impl* nda = dest->data.FastEl(i);
      nda->CopyFromRow(row, *sda, row);
    }
  }
  dest->StructUpdate(false);
  spec->ClearColumns();
  return true;
}

bool taDataOps::Group(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
  dest->StructUpdate(true);
  spec->GetColumns(src);		// cache column pointers & indicies from names
  dest->Reset();
  // add the dest columns
  for(int i=0;i<spec->size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->FastEl(i);
    if(ds->col_idx < 0) continue;
    DataArray_impl* sda = src->data.FastEl(ds->col_idx);
    DataArray_impl* nda = (DataArray_impl*)sda->MakeToken();
    dest->data.Add(nda);
    nda->Copy_NoData(*sda);
    String dst_op = ds->agg.GetAggName();
    dst_op.downcase();
    nda->name += "_" + dst_op;
  }    

  // sort by grouped guys, in order
  DataSortSpec sort_spec;
  taBase::Own(sort_spec, NULL);
  for(int i=0;i<spec->size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->FastEl(i);
    if(ds->col_idx < 0) continue;
    if(ds->agg.op != Aggregate::GROUP) continue;
    DataSortEl* ss = (DataSortEl*)sort_spec.New(1, &TA_DataSortEl);
    ss->col_name = ds->col_name;
    ss->col_idx = ds->col_idx;
    taBase::SetPointer((taBase**)&ss->column, ds->column);
    ss->order = DataSortEl::ASCENDING;
  }
  if(sort_spec.size == 0) {
    Group_nogp(dest, src, spec); // no group ops: just simple aggs
  }
  else {
    Group_gp(dest, src, spec, &sort_spec); // grouping.
  }
  dest->StructUpdate(false);
  spec->ClearColumns();
  return true;
}

bool taDataOps::Group_nogp(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
  dest->AddBlankRow();
  for(int i=0;i<spec->size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->FastEl(i);
    if(ds->col_idx < 0) continue;
    DataArray_impl* sda = src->data.FastEl(ds->col_idx);
    DataArray_impl* dda = dest->data.FastEl(i); // index is spec index
    if(sda->valType() == taBase::VT_DOUBLE) {
      dda->SetValAsDouble(taMath_double::vec_aggregate((double_Matrix*)sda->AR(), ds->agg), 0);
    }
  }
  return true;
}

// todo: checkconfig that group is not matrix..

bool taDataOps::Group_gp(DataTable* dest, DataTable* src, DataGroupSpec* spec, DataSortSpec* sort_spec) {
  // first sort by grouping critera
  DataTable ssrc;
  taBase::Own(ssrc, NULL);	// activates initlinks, refs
  taDataOps::Sort(&ssrc, src, sort_spec);

  sort_spec->GetColumns(&ssrc);	// re-get columns -- they were nuked by Sort op!

  Variant_Array cur_vals;
  cur_vals.EnforceSize(sort_spec->size);

  // initialize cur vals
  for(int i=0;i<sort_spec->size; i++) {
    DataSortEl* ds = (DataSortEl*)sort_spec->FastEl(i);
    DataArray_impl* sda = ssrc.data.FastEl(ds->col_idx);
    Variant cval = sda->GetValAsVar(0); // start at row 0
    cur_vals[i] = cval;
  }

  int st_row = 0;
  int row = 1;
  while(row < ssrc.rows) {
    for(;row < ssrc.rows; row++) {
      bool new_val = false;
      for(int i=0;i<sort_spec->size; i++) {
	DataSortEl* ds = (DataSortEl*)sort_spec->FastEl(i);
	DataArray_impl* sda = src->data.FastEl(ds->col_idx);
	Variant cval = sda->GetValAsVar(row);
	if(cval != cur_vals[i]) {
	  new_val = true;
	  cur_vals[i] = cval;
	}
      }
      if(new_val) break;
    }
    int n_rows = row - st_row;
    // now go through actual group ops!
    dest->AddBlankRow();
    for(int i=0;i<spec->size; i++) {
      DataGroupEl* ds = (DataGroupEl*)spec->FastEl(i);
      if(ds->col_idx < 0) continue;
      DataArray_impl* sda = src->data.FastEl(ds->col_idx);
      DataArray_impl* dda = dest->data.FastEl(i); // index is spec index
      if(ds->agg.op == Aggregate::GROUP) {
	dda->SetValAsVar(sda->GetValAsVar(st_row), -1); // -1 = last row
      }
      else {
	if(sda->valType() == taBase::VT_DOUBLE) {
	  double_Matrix* mat = (double_Matrix*)sda->GetRangeAsMatrix(st_row, n_rows);
	  taBase::Ref(mat);
	  dda->SetValAsDouble(taMath_double::vec_aggregate(mat, ds->agg), -1); // -1 = last row
	  taBase::unRef(mat);
	}
      }
    }
    st_row = row;
    row++;			// move on to next row!
  }
  return true;
}

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

void DataProg::Initialize() {
}

/////////////////////////////////////////////////////////
//   data sort prog
/////////////////////////////////////////////////////////

void DataSortProg::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  sort_spec.SetDataTable(src_data);
}

void DataSortProg::Initialize() {
}

String DataSortProg::GetDisplayName() const {
  String rval = "Sort ";
  if(src_data) {
    rval += " from: " + src_data->name;
  }
  if(dest_data) {
    rval += " to: " + dest_data->name;
  }
  return rval;
}

void DataSortProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  sort_spec.SetDataTable(src_data);
  sort_spec.CheckConfig(quiet, rval);
}

const String DataSortProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataSortProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataOps::Sort(dsp->dest_data, dsp->src_data, dsp->sort_spec);\n";
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval; 
}

/////////////////////////////////////////////////////////
//   data selectRows prog
/////////////////////////////////////////////////////////

void DataSelectRowsProg::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  select_spec.SetDataTable(src_data);
}

void DataSelectRowsProg::Initialize() {
}

String DataSelectRowsProg::GetDisplayName() const {
  String rval = "SelectRows ";
  if(src_data) {
    rval += " from: " + src_data->name;
  }
  if(dest_data) {
    rval += " to: " + dest_data->name;
  }
  return rval;
}

void DataSelectRowsProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  select_spec.SetDataTable(src_data);
  select_spec.CheckConfig(quiet, rval);
}

const String DataSelectRowsProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataSelectRowsProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataOps::SelectRows(dsp->dest_data, dsp->src_data, dsp->select_spec);\n";
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval; 
}

/////////////////////////////////////////////////////////
//   data selectCols prog
/////////////////////////////////////////////////////////

void DataSelectColsProg::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  select_spec.SetDataTable(src_data);
}

void DataSelectColsProg::Initialize() {
}

String DataSelectColsProg::GetDisplayName() const {
  String rval = "SelectCols ";
  if(src_data) {
    rval += " from: " + src_data->name;
  }
  if(dest_data) {
    rval += " to: " + dest_data->name;
  }
  return rval;
}

void DataSelectColsProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  select_spec.SetDataTable(src_data);
  select_spec.CheckConfig(quiet, rval);
}

const String DataSelectColsProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataSelectColsProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataOps::SelectCols(dsp->dest_data, dsp->src_data, dsp->select_spec);\n";
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval; 
}

/////////////////////////////////////////////////////////
//   data group prog
/////////////////////////////////////////////////////////

void DataGroupProg::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  group_spec.SetDataTable(src_data);
}

void DataGroupProg::Initialize() {
}

String DataGroupProg::GetDisplayName() const {
  String rval = "Group ";
  if(src_data) {
    rval += " from: " + src_data->name;
  }
  if(dest_data) {
    rval += " to: " + dest_data->name;
  }
  return rval;
}

void DataGroupProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  group_spec.SetDataTable(src_data);
  group_spec.CheckConfig(quiet, rval);
}

const String DataGroupProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataGroupProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataOps::Group(dsp->dest_data, dsp->src_data, dsp->group_spec);\n";
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval; 
}
