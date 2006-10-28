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

#include "ta_dataproc.h"
#include "css_machine.h"
#include "ta_project.h"		// for debugging

/////////////////////////////////////////////////////////
//   DataOpEl Base class
/////////////////////////////////////////////////////////

void DataOpEl::Initialize() {
  data_cols = NULL;
  column = NULL;
  col_idx = -1;
}

void DataOpEl::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  if(column) {
    col_name = column->name;
    taBase::SetPointer((taBase**)&column, NULL); // reset as soon as used -- just a temp guy!
  }
  if(!data_table) {
    taBase::SetPointer((taBase**)&data_cols, NULL);
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
    rval = false;
  }
  if(col_idx < 0) {
    if(!quiet) taMisc::CheckError("Error in DataOpEl:",GetPath(),
				  "could not find", col_name,"in datatable");
    rval = false;
  }
}

void DataOpEl::SetDataTable(DataTable* dt) {
  data_table = dt;
  if(!dt)
    taBase::SetPointer((taBase**)&data_cols, NULL);
  else
    taBase::SetPointer((taBase**)&data_cols, &dt->data);
}

void DataOpEl::GetColumns(DataTable* dt) {
  DataArray_impl* da = dt->FindColName(col_name, col_idx);
  if(col_idx < 0) da = NULL;	// just to be sure..
  taBase::SetPointer((taBase**)&column, da);
}

void DataOpEl::ClearColumns() {
  taBase::SetPointer((taBase**)&column, NULL);
}

///////////////////////////

void DataOpList::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if(owner != NULL)
    owner->UpdateAfterEdit();	// this should set data table stuff on new guys
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

void DataSortEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(column) {
    if(column->is_matrix) {
      if(!quiet) taMisc::CheckError("Error in DataSortEl:",GetPath(),
				  "cannot use matrix column to sort");
      rval = false;
    }
  }
}

///////////////////////////

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

void DataSelectEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(column) {
    if(column->is_matrix) {
      if(!quiet) taMisc::CheckError("Error in DataSelectEl:",GetPath(),
				    "cannot use matrix column to select");
      rval = false;
    }
  }
}

String DataSelectSpec::GetDisplayName() const {
  return inherited::GetDisplayName() + " " +
    GetTypeDef()->GetEnumString("CombOp", comb_op);
}

///////////////////////////

void DataGroupEl::Initialize() {
}

String DataGroupEl::GetDisplayName() const {
  return col_name + " " + agg.GetAggName();
}

void DataGroupEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(column) {
    if((agg.op == Aggregate::GROUP) && (column->is_matrix)) {
      if(!quiet) taMisc::CheckError("Error in DataGroupEl:",GetPath(),
				  "cannot use matrix column to GROUP");
      rval = false;
    }
  }
}

void DataGroupSpec::Initialize() {
  SetBaseType(&TA_DataGroupEl);
}

///////////////////////////

void DataJoinSpec::Initialize() {
}

void DataJoinSpec::SetDataTable(DataTable* dt_a, DataTable* dt_b) {
  col_a.SetDataTable(dt_a);
  col_b.SetDataTable(dt_b);
}

void DataJoinSpec::GetColumns(DataTable* dt_a, DataTable* dt_b) {
  col_a.GetColumns(dt_a);
  col_b.GetColumns(dt_b);
}

void DataJoinSpec::ClearColumns() {
  col_a.ClearColumns();
  col_b.ClearColumns();
}

String DataJoinSpec::GetDisplayName() const {
  return "join: col_a:" + col_a.GetDisplayName() + " to col_b: " + col_b.GetDisplayName();
}

void DataJoinSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  col_a.CheckConfig(quiet, rval);
  col_b.CheckConfig(quiet, rval);
}

/////////////////////////////////////////////////////////
//   taDataProc
/////////////////////////////////////////////////////////

bool taDataProc::GetDest(DataTable*& dest, DataTable* src, const String& suffix) {
  if(dest != NULL) return false;
  taProject* proj = GET_OWNER(src, taProject);
  DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("AnalysisData");
  dest = dgp->NewEl(1, &TA_DataTable);
  dest->name = src->name + "_" + suffix;
  return true;
}

bool taDataProc::Sort(DataTable* dest, DataTable* src, DataSortSpec* spec) {
  // just copy and operate on dest
  GetDest(dest, src, "Sort");
  dest->Reset();
  String dnm = dest->name;
  *dest = *src;
  dest->name = dnm;
  return Sort_impl(dest, spec);
}

int taDataProc::Sort_Compare(DataTable* dt_a, int row_a, DataTable* dt_b, int row_b,
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

bool taDataProc::Sort_impl(DataTable* dt, DataSortSpec* spec) {
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

bool taDataProc::SelectRows(DataTable* dest, DataTable* src, DataSelectSpec* spec) {
  GetDest(dest, src, "SelectRows");
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
//       cerr << "cmp: " << ds->col_name << " idx: " << ds->col_idx
//       << " val: " << val << " ev: " << ev << endl;
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
//     cerr << "not_incl: " << not_incl << " incl: " << incl << endl;
    if(((spec->comb_op == DataSelectSpec::AND) || (spec->comb_op == DataSelectSpec::NOT_AND))
       && not_incl) continue;
    if(((spec->comb_op == DataSelectSpec::OR) || (spec->comb_op == DataSelectSpec::NOT_OR))
       && !incl) continue;
    // continuing now..
//     cerr << "added!" << endl;
    dest->AddBlankRow();
    dest->CopyFromRow(-1, *src, row);
  }
  dest->StructUpdate(false);
  spec->ClearColumns();
  return true;
}

bool taDataProc::SelectCols(DataTable* dest, DataTable* src, DataOpList* spec) {
  GetDest(dest, src, "SelectCols");
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

bool taDataProc::Group(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
  GetDest(dest, src, "Group");
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

bool taDataProc::Group_nogp(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
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

bool taDataProc::Group_gp(DataTable* dest, DataTable* src, DataGroupSpec* spec, DataSortSpec* sort_spec) {
  DataTable ssrc;
  taBase::Own(ssrc, NULL);	// activates initlinks, refs
  taDataProc::Sort(&ssrc, src, sort_spec);

  sort_spec->GetColumns(&ssrc);	// re-get columns -- they were nuked by Sort op!

  Variant_Array cur_vals;
  cur_vals.EnforceSize(sort_spec->size);

  // initialize cur vals
  for(int i=0;i<sort_spec->size; i++) {
    DataSortEl* ds = (DataSortEl*)sort_spec->FastEl(i);
    DataArray_impl* sda = ssrc.data.FastEl(ds->col_idx);
    Variant cval = sda->GetValAsVar(0); // start at row 0
    cur_vals[i] = cval;
//     cerr << i << " init val: " << cur_vals[i] << " input: " << cval << endl;
  }

  int st_row = 0;
  int row = 1;
  while(row < ssrc.rows) {
    for(;row < ssrc.rows; row++) {
//       cerr << "row: " << row;
      bool new_val = false;
      for(int i=0;i<sort_spec->size; i++) {
	DataSortEl* ds = (DataSortEl*)sort_spec->FastEl(i);
	DataArray_impl* sda = ssrc.data.FastEl(ds->col_idx);
	Variant cval = sda->GetValAsVar(row);
	if(cval != cur_vals[i]) {
// 	  cerr << " new_val:  oval: " << cur_vals[i] << " nval: " << cval;
	  new_val = true;
	  cur_vals[i] = cval;
	}
      }
//       cerr << endl;
      if(new_val) break;
    }
    int n_rows = row - st_row;
//     cerr << "adding: row: " << row << " st_row: " << st_row << endl;
    // now go through actual group ops!
    dest->AddBlankRow();
    for(int i=0;i<spec->size; i++) {
      DataGroupEl* ds = (DataGroupEl*)spec->FastEl(i);
      if(ds->col_idx < 0) continue;
      DataArray_impl* sda = ssrc.data.FastEl(ds->col_idx);
      DataArray_impl* dda = dest->data.FastEl(i); // index is spec index
      if(ds->agg.op == Aggregate::GROUP) {
	dda->SetValAsVar(sda->GetValAsVar(st_row), -1); // -1 = last row
      }
      else {
	if(sda->valType() == taBase::VT_DOUBLE) {
	  double_Matrix* mat = (double_Matrix*)sda->GetRangeAsMatrix(st_row, n_rows);
	  taBase::Ref(mat);
	  dda->SetValAsDouble(taMath_double::vec_aggregate(mat, ds->agg), -1); // -1 = last row
	  taBase::unRefDone(mat);
	}
      }
    }
    st_row = row;
    row++;			// move on to next row!
  }
  return true;
}

bool taDataProc::Join(DataTable* dest, DataTable* src_a, DataTable* src_b, DataJoinSpec* spec) {
  if((spec->col_a.col_idx < 0) || (spec->col_b.col_idx < 0)) return false;
  GetDest(dest, src_a, "Join");
  dest->StructUpdate(true);
  spec->GetColumns(src_a, src_b);	// cache column pointers & indicies from names
  dest->Reset();
  for(int i=0; i < src_a->data.size; i++) {
    //    if(i == spec->col_a.col_idx) continue; // include first guy..
    DataArray_impl* sda = src_a->data.FastEl(i);
    DataArray_impl* nda = (DataArray_impl*)sda->MakeToken();
    dest->data.Add(nda);	// todo: AddUniqueName?? + no reset + orphan?
    nda->Copy_NoData(*sda);
  }
  int a_cols = src_a->data.size; // -1 if skipping index value
  for(int i=0; i < src_b->data.size; i++) {
    if(i == spec->col_b.col_idx) continue; // don't include common index
    DataArray_impl* sdb = src_b->data.FastEl(i);
    DataArray_impl* nda = (DataArray_impl*)sdb->MakeToken();
    dest->data.Add(nda);
    nda->Copy_NoData(*sdb);
  }    
  for(int row=0;row<src_a->rows;row++) {
    dest->AddBlankRow();
    for(int i=0;i<src_a->data.size; i++) {
      //    if(i == spec->col_a.col_idx) continue; // include first guy..
      DataArray_impl* sda = src_a->data.FastEl(i);
      DataArray_impl* nda = dest->data.FastEl(i); // todo: change above if uncommented
      nda->CopyFromRow(row, *sda, row); // just copy
    }
    DataArray_impl* sda = src_a->data.FastEl(spec->col_a.col_idx);
    Variant val_a = sda->GetValAsVar(row);
    DataArray_impl* sdb = src_b->data.FastEl(spec->col_b.col_idx);
    int b_row = -1;
    // todo: move this to FindVariant() in matrix/datatable?
    for(int j=0;j<sdb->rows();j++) {
      Variant val_b = sdb->GetValAsVar(j);
      if(val_a == val_b) {
	b_row = j; break;
      }
    }
    if(b_row >= 0) {		// copy it over
      int col_idx = a_cols;
      for(int i=0; i < src_b->data.size; i++) {
	if(i == spec->col_b.col_idx) continue; // don't include common index
	DataArray_impl* sdb = src_b->data.FastEl(i);
	DataArray_impl* nda = dest->data.FastEl(col_idx);
	nda->CopyFromRow(row, *sdb, b_row); // just copy
	col_idx++;
      }    
    }
    else {
      taMisc::Warning("taDataProc::Join -- value for src_a:", (String)val_a, "not found in column",
		      spec->col_b.col_name, "of src_b:", src_b->name);
    }
  }
  dest->StructUpdate(false);
  spec->ClearColumns();
  return true;
}

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

void DataProcCall::Initialize() {
  min_type = &TA_taDataProc;
  object_type = &TA_taDataProc;
}

void DataProg::Initialize() {
}

void DataProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!src_data) {
    if(!quiet) taMisc::CheckError("Error in DataProg:",GetPath(),
				  "src_data is NULL");
    rval = false;
  }
  // NULL OK!
//   if(!dest_data) {
//     if(!quiet) taMisc::CheckError("Error in DataProg:",GetPath(),
// 				  "dest_data is NULL");
//     rval = false;
//   }
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

void DataSortProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  sort_spec.GetColumns(src_data);
  sort_spec.CheckConfig(quiet, rval);
  sort_spec.ClearColumns();
}

const String DataSortProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataSortProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataProc::Sort(dsp->dest_data, dsp->src_data, dsp->sort_spec);\n";
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

void DataSelectRowsProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  select_spec.GetColumns(src_data);
  select_spec.CheckConfig(quiet, rval);
  select_spec.ClearColumns();
}

const String DataSelectRowsProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataSelectRowsProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataProc::SelectRows(dsp->dest_data, dsp->src_data, dsp->select_spec);\n";
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

void DataSelectColsProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  select_spec.GetColumns(src_data);
  select_spec.CheckConfig(quiet, rval);
  select_spec.ClearColumns();
}

const String DataSelectColsProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataSelectColsProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataProc::SelectCols(dsp->dest_data, dsp->src_data, dsp->select_spec);\n";
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

void DataGroupProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  group_spec.GetColumns(src_data);
  group_spec.CheckConfig(quiet, rval);
  group_spec.ClearColumns();
}

const String DataGroupProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataGroupProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataProc::Group(dsp->dest_data, dsp->src_data, dsp->group_spec);\n";
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval; 
}

/////////////////////////////////////////////////////////
//   data join prog
/////////////////////////////////////////////////////////

void DataJoinProg::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  join_spec.SetDataTable(src_data, src_b_data);
}

void DataJoinProg::Initialize() {
}

String DataJoinProg::GetDisplayName() const {
  String rval = join_spec.GetDisplayName();
  if(src_data) {
    rval += " a: " + src_data->name;
  }
  if(src_b_data) {
    rval += " b: " + src_b_data->name;
  }
  if(dest_data) {
    rval += " to: " + dest_data->name;
  }
  return rval;
}

void DataJoinProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  join_spec.GetColumns(src_data, src_b_data);
  join_spec.CheckConfig(quiet, rval);
  join_spec.ClearColumns();
}

const String DataJoinProg::GenCssBody_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "{ DataJoinProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  rval += cssMisc::Indent(indent_level+1) +
    "taDataProc::Join(dsp->dest_data, dsp->src_data, dsp->src_b_data, dsp->join_spec);\n";
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval; 
}

/////////////////////////////////////////////////////////
//   data calc loop
/////////////////////////////////////////////////////////

// todo: do checking on valid column names as css variables!

void DataCalcLoop::Initialize() {
}

void DataCalcLoop::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  src_cols.SetDataTable(src_data);
  dest_cols.SetDataTable(src_data);
}

void DataCalcLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!dest_data) {		// not ok!
    if(!quiet) taMisc::CheckError("Error in DataCalcLoop:",GetPath(),
				  "dest_data is NULL");
    rval = false;
  }
}

String DataCalcLoop::GetDisplayName() const {
  String rval = "Calc Loop ";
  if(src_data) {
    rval += " from: " + src_data->name;
  }
  if(dest_data) {
    rval += " to: " + dest_data->name;
  }
  return rval;
}

void DataCalcLoop::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  src_cols.GetColumns(src_data);
  src_cols.CheckConfig(quiet, rval);
  src_cols.ClearColumns();
  dest_cols.GetColumns(dest_data);
  dest_cols.CheckConfig(quiet, rval);
  dest_cols.ClearColumns();
  loop_code.CheckConfig(quiet, rval);
}

void DataCalcLoop::PreGenChildren_impl(int& item_id) {
  loop_code.PreGen(item_id);
}

const String DataCalcLoop::GenCssPre_impl(int indent_level) {
  src_cols.GetColumns(src_data);
  String il1 = cssMisc::Indent(indent_level+1);
  String il2 = cssMisc::Indent(indent_level+2);
  String rval = cssMisc::Indent(indent_level) + "{ DataCalcLoop* dcl = this" + GetPath(NULL, program()) + ";\n";
  rval += il1 + "dcl->dest_data.ResetData(); // all data ops clear out old existing data\n";
  
  rval += il1 + "for(int src_row=0; src_row < dcl->src_data.rows; src_row++) {\n";
  for(int i=0;i<src_cols.size; i++) {
    DataOpEl* ds = src_cols[i];
    rval += il2 + "Variant s_" + ds->col_name + " = dcl->src_data.GetValAsVar(" +
      String(ds->col_idx) + ", src_row);\n";
  }
  src_cols.ClearColumns();
  // dest cols are only activated by DataAddDestRow
  return rval;
}

const String DataCalcLoop::GenCssBody_impl(int indent_level) {
  return loop_code.GenCss(indent_level + 2);
}

const String DataCalcLoop::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level+1) + "} // for loop\n";
  rval += cssMisc::Indent(indent_level) + "} // DataCalcLoop dcl\n";
  return rval;
}

/////////////////////////////////////////////////////////
//   data calc add dest row
/////////////////////////////////////////////////////////

void DataCalcAddDestRow::Initialize() {
}

String DataCalcAddDestRow::GetDisplayName() const {
  String rval = "Add Row to: ";
  if(dest_data) {
    rval += dest_data->name;
  }
  return rval;
}

void DataCalcAddDestRow::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return;
  src_data = dcl->src_data;
  dest_data = dcl->dest_data;
}

void DataCalcAddDestRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    if(!quiet) taMisc::CheckError("DataCalcAddDestRow: parent DataCalcLoop not found in:",
				  GetPath());
    rval = false;
  }
}

const String DataCalcAddDestRow::GenCssBody_impl(int indent_level) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return "// DataCalcAddDestRow Error -- DataCalcLoop not found!!\n";

  dcl->dest_cols.GetColumns(dcl->dest_data);
  String rval = cssMisc::Indent(indent_level) + "dcl->dest_data->AddBlankRow();\n";

  for(int i=0;i<dcl->dest_cols.size; i++) {
    DataOpEl* ds = dcl->dest_cols[i];
    rval += cssMisc::Indent(indent_level) + "Variant d_" + ds->col_name + " = dcl->dest_data.GetValAsVar(" +
      String(ds->col_idx) + ", -1); // -1 = last row\n";
  }
  dcl->dest_cols.ClearColumns();
  return rval;
}

/////////////////////////////////////////////////////////
//   data calc add dest row
/////////////////////////////////////////////////////////

void DataCalcSetDestRow::Initialize() {
}

String DataCalcSetDestRow::GetDisplayName() const {
  String rval = "Set Row in: ";
  if(dest_data) {
    rval += dest_data->name;
  }
  return rval;
}

void DataCalcSetDestRow::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return;
  src_data = dcl->src_data;
  dest_data = dcl->dest_data;
}

void DataCalcSetDestRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    if(!quiet) taMisc::CheckError("DataCalcSetDestRow: parent DataCalcLoop not found in:",
				  GetPath());
    rval = false;
  }
}

const String DataCalcSetDestRow::GenCssBody_impl(int indent_level) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return "// DataCalcSetDestRow Error -- DataCalcLoop not found!!\n";

  String rval;
  dcl->dest_cols.GetColumns(dcl->dest_data);
  for(int i=0;i<dcl->dest_cols.size; i++) {
    DataOpEl* ds = dcl->dest_cols[i];
    rval += cssMisc::Indent(indent_level) + "dcl->dest_data.SetValAsVar(" +
      "d_" + ds->col_name + ", " + String(ds->col_idx) + ", -1); // -1 = last row\n";
  }
  dcl->dest_cols.ClearColumns();
  return rval;
}

