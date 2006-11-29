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

#include <QDir>

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

String DataOpEl::GetName() const {
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
  if(!dt) return;
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
  if(owner && owner->GetOwner() && owner->GetOwner()->InheritsFrom(&TA_DataProg))
    owner->GetOwner()->UpdateAfterEdit();
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

void DataOpList::AddAllColumns(DataTable* dt) {
  if(!dt) return;
  for(int i=0;i<dt->data.size; i++) {
    DataArray_impl* da = dt->data[i];
    DataOpEl* dop = FindName(da->name);
    if(dop) continue;
    dop = (DataOpEl*)New(1);
    dop->col_name = da->name;
  }
  SetDataTable(dt);
}


void DataOpBaseSpec::Initialize() {
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

void DataSortSpec::Initialize() {
  ops.SetBaseType(&TA_DataSortEl);
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
  ops.SetBaseType(&TA_DataGroupEl);
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

void DataSelectSpec::Initialize() {
  ops.SetBaseType(&TA_DataSelectEl);
  comb_op = AND;
}

String DataSelectSpec::GetDisplayName() const {
  return inherited::GetDisplayName() + " " +
    GetTypeDef()->GetEnumString("CombOp", comb_op);
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

///////////////////////////////////////////////////////////////////
// manipulating lists of columns

bool taDataProc::GetCommonCols(DataTable* dest, DataTable* src, DataOpList* dest_cols,
			       DataOpList* src_cols) {
  if(!dest || !src) return false;
  src_cols->Reset(); dest_cols->Reset();
  src_cols->AddAllColumns(src);
  src_cols->GetColumns(src);
  for(int i=0; i<src_cols->size;i++) {
    DataOpEl* sop = src_cols->FastEl(i);
    int d_idx;
    DataArray_impl* dda = dest->FindColName(sop->col_name, d_idx);
    if(!dda) {
      src_cols->RemoveIdx(i); i--;
      continue;
    }
    DataArray_impl* sda = src->data[sop->col_idx];
    if(dda->cell_size() != sda->cell_size()) continue; // incompatible
    if(sda->cell_size() > 1) {
      if(sda->valType() != dda->valType()) continue; // must be compatible var types
    }
    DataOpEl* dop = (DataOpEl*)dest_cols->New(1);
    dop->SetDataTable(dest);
    dop->col_name = dda->name;
    dop->col_idx = d_idx;
  }
  // note: client should do this when done with info:
//   src_cols->ClearColumns();
//   dest_cols->ClearColumns();
  return true;
}

bool taDataProc::GetColIntersection(DataOpList* trg_cols, DataOpList* ref_cols) {
  for(int i=trg_cols->size-1; i>=0; i--) {
    DataOpEl* top = trg_cols->FastEl(i);
    if(!ref_cols->FindName(top->col_name))
      trg_cols->RemoveIdx(i);
  }
  return true;
}


///////////////////////////////////////////////////////////////////
// basic copying and concatenating

bool taDataProc::CopyCommonColsRow(DataTable* dest, DataTable* src, DataOpList* dest_cols,
				   DataOpList* src_cols, int dest_row, int src_row) {
  if(!dest || !src || !dest_cols || !src_cols) return false;
  for(int j=0;j<src_cols->size;j++) {
    DataOpEl* sop = src_cols->FastEl(j);
    DataOpEl* dop = dest_cols->FastEl(j);
    DataArray_impl* sda = src->data[sop->col_idx];
    DataArray_impl* dda = dest->data[dop->col_idx];
    dda->CopyFromRow(dest_row, *sda, src_row);
  }
  return true;
}

bool taDataProc::CopyCommonColData(DataTable* dest, DataTable* src) {
  if(!dest || !src) return false;
  DataOpList dest_cols;
  DataOpList src_cols;
  GetCommonCols(dest, src, &dest_cols, &src_cols);
  for(int i=0;i<src->rows;i++) {
    dest->AddBlankRow();
    for(int j=0;j<src_cols.size;j++) {
      DataOpEl* sop = src_cols.FastEl(j);
      DataOpEl* dop = dest_cols.FastEl(j);
      DataArray_impl* sda = src->data[sop->col_idx];
      DataArray_impl* dda = dest->data[dop->col_idx];
      dda->CopyFromRow(-1, *sda, i);
    }
  }
  return true;
}

bool taDataProc::AppendRows(DataTable* dest, DataTable* src) {
  if(!dest || !src) return false;
  if(dest->data.size != src->data.size) {
    taMisc::Error("taDataProc::AppendRows -- tables do not have same number of columns -- use CopyCommonColData instead!");
    return false;
  }
  for(int i=0;i<src->rows;i++) {
    dest->AddBlankRow();
    for(int j=0;j<src->data.size;j++) {
      DataArray_impl* sda = src->data[j];
      DataArray_impl* dda = dest->data[j];
      dda->CopyFromRow(-1, *sda, i);
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// reordering functions

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
  for(int i=0;i<spec->ops.size; i++) {
    DataSortEl* ds = (DataSortEl*)spec->ops.FastEl(i);
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

  // todo: seems like you can just sort an index and then read that off.
  // but the tmp copy stuff isn't going to work.  

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

bool taDataProc::Permute(DataTable* dest, DataTable* src) {
  GetDest(dest, src, "Permute");
  dest->StructUpdate(true);
  dest->Copy_NoData(*src);		// give it same structure
  // this just uses the index technique..
  int_Array idxs;
  idxs.EnforceSize(src->rows);
  idxs.FillSeq();
  idxs.Permute();
  for(int row=0;row<src->rows; row++) {
    dest->AddBlankRow();
    dest->CopyFromRow(-1, *src, idxs[row]);
  }
  dest->StructUpdate(false);
  return true;
}

bool taDataProc::Group(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
  GetDest(dest, src, "Group");
  dest->StructUpdate(true);
  spec->GetColumns(src);		// cache column pointers & indicies from names
  dest->Reset();
  // add the dest columns
  for(int i=0;i<spec->ops.size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue;
    DataArray_impl* sda = src->data.FastEl(ds->col_idx);
    DataArray_impl* nda = (DataArray_impl*)sda->MakeToken();
    dest->data.Add(nda);
    nda->Copy_NoData(*sda);
    String dst_op = ds->agg.GetAggName();
    dst_op.downcase();
    nda->name += "_" + dst_op;
  }    
  dest->UniqueColNames();	// make them unique!

  // sort by grouped guys, in order
  DataSortSpec sort_spec;
  taBase::Own(sort_spec, NULL);
  for(int i=0;i<spec->ops.size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue;
    if(ds->agg.op != Aggregate::GROUP) continue;
    DataSortEl* ss = (DataSortEl*)sort_spec.ops.New(1, &TA_DataSortEl);
    ss->col_name = ds->col_name;
    ss->col_idx = ds->col_idx;
    taBase::SetPointer((taBase**)&ss->column, ds->column);
    ss->order = DataSortEl::ASCENDING;
  }
  if(sort_spec.ops.size == 0) {
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
  for(int i=0;i<spec->ops.size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
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
  cur_vals.EnforceSize(sort_spec->ops.size);

  // initialize cur vals
  for(int i=0;i<sort_spec->ops.size; i++) {
    DataSortEl* ds = (DataSortEl*)sort_spec->ops.FastEl(i);
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
      for(int i=0;i<sort_spec->ops.size; i++) {
	DataSortEl* ds = (DataSortEl*)sort_spec->ops.FastEl(i);
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
    for(int i=0;i<spec->ops.size; i++) {
      DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
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

///////////////////////////////////////////////////////////////////
// row-wise functions: selecting/splitting

bool taDataProc::SelectRows(DataTable* dest, DataTable* src, DataSelectSpec* spec) {
  GetDest(dest, src, "SelectRows");
  dest->StructUpdate(true);
  dest->Copy_NoData(*src);		// give it same structure
  spec->GetColumns(src);		// cache column pointers & indicies from names
  for(int row=0;row<src->rows; row++) {
    bool incl = false;
    bool not_incl = false;
    for(int i=0; i<spec->ops.size; i++) {
      DataSelectEl* ds = (DataSelectEl*)spec->ops.FastEl(i);
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

bool taDataProc::SplitRows(DataTable* dest_a, DataTable* dest_b, DataTable* src,
			   DataSelectSpec* spec) {
  GetDest(dest_a, src, "SplitRows_a");
  GetDest(dest_b, src, "SplitRows_b");
  dest_a->StructUpdate(true);
  dest_a->Copy_NoData(*src);		// give it same structure
  dest_b->StructUpdate(true);
  dest_b->Copy_NoData(*src);		// give it same structure
  spec->GetColumns(src);		// cache column pointers & indicies from names
  for(int row=0;row<src->rows; row++) {
    bool incl = false;
    bool not_incl = false;
    for(int i=0; i<spec->ops.size; i++) {
      DataSelectEl* ds = (DataSelectEl*)spec->ops.FastEl(i);
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
    bool sel_a = true;
    if(((spec->comb_op == DataSelectSpec::AND) || (spec->comb_op == DataSelectSpec::NOT_AND))
       && not_incl) sel_a = false;
    if(((spec->comb_op == DataSelectSpec::OR) || (spec->comb_op == DataSelectSpec::NOT_OR))
       && !incl) sel_a = false;
    if(sel_a) {
      dest_a->AddBlankRow();
      dest_a->CopyFromRow(-1, *src, row);
    }
    else {
      dest_b->AddBlankRow();
      dest_b->CopyFromRow(-1, *src, row);
    }
  }
  dest_a->StructUpdate(false);
  dest_b->StructUpdate(false);
  spec->ClearColumns();
  return true;
}

bool taDataProc::SplitRowsN(DataTable* src, DataTable* dest_1, int n1, DataTable* dest_2, int n2,
			    DataTable* dest_3, int n3, DataTable* dest_4, int n4,
			    DataTable* dest_5, int n5, DataTable* dest_6, int n6) {
  int nary[6] = {n1, n2, n3, n4, n5, n6};
  DataTable* dary[6] = {dest_1, dest_2, dest_3, dest_4, dest_5, dest_6};
  for(int i=0;i<6;i++) {
    if(nary[i] != 0) {
      GetDest(dary[i], src, "SplitByN_" + String(i));
      dary[i]->StructUpdate(true);
      dary[i]->Copy_NoData(*src);
    }
  }
  int st_n = 0;
  int end_n = n1;
  int ni = 0;
  for(int row=0;row<src->rows; row++) {
    if(row < end_n) {
      dary[ni]->AddBlankRow();
      dary[ni]->CopyFromRow(-1, *src, row);
    }
    else {
      ni++;
      if((nary[ni] == 0) || !dary[ni]) break;
      if(nary[ni] < 0) nary[ni] = src->rows - row;
      st_n = row;
      end_n = st_n + nary[ni];

      dary[ni]->AddBlankRow();
      dary[ni]->CopyFromRow(-1, *src, row);
    }
  }
  for(int i=0;i<6;i++) {
    if(nary[i] != 0) {
      dary[i]->StructUpdate(false);
    }
  }
  return true;
}

bool taDataProc::SplitRowsNPermuted(DataTable* src, DataTable* dest_1, int n1, DataTable* dest_2, int n2,
				    DataTable* dest_3, int n3, DataTable* dest_4, int n4,
				    DataTable* dest_5, int n5, DataTable* dest_6, int n6) {

  int nary[6] = {n1, n2, n3, n4, n5, n6};
  DataTable* dary[6] = {dest_1, dest_2, dest_3, dest_4, dest_5, dest_6};
  for(int i=0;i<6;i++) {
    if(nary[i] != 0) {
      GetDest(dary[i], src, "SplitByN_" + String(i));
      dary[i]->StructUpdate(true);
      dary[i]->Copy_NoData(*src);
    }
  }
  int_Array idxs;
  idxs.EnforceSize(src->rows);
  idxs.FillSeq();
  idxs.Permute();
  int st_n = 0;
  int end_n = n1;
  int ni = 0;
  for(int row=0;row<src->rows; row++) {
    if(row < end_n) {
      dary[ni]->AddBlankRow();
      dary[ni]->CopyFromRow(-1, *src, idxs[row]);
    }
    else {
      ni++;
      if((nary[ni] == 0) || !dary[ni]) break;
      if(nary[ni] < 0) nary[ni] = src->rows - row;
      st_n = row;
      end_n = st_n + nary[ni];

      dary[ni]->AddBlankRow();
      dary[ni]->CopyFromRow(-1, *src, idxs[row]);
    }
  }
  for(int i=0;i<6;i++) {
    if(nary[i] != 0) {
      dary[i]->StructUpdate(false);
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// column-wise functions: selecting, joining

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
  dest->UniqueColNames();	// make them unique!
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

bool taDataProc::JoinByRow(DataTable* dest, DataTable* src_a, DataTable* src_b) {
  GetDest(dest, src_a, "Join");
  dest->StructUpdate(true);
  dest->Reset();
  for(int i=0; i < src_a->data.size; i++) {
    DataArray_impl* sda = src_a->data.FastEl(i);
    DataArray_impl* nda = (DataArray_impl*)sda->MakeToken();
    dest->data.Add(nda);	// todo: AddUniqueName?? + no reset + orphan?
    nda->Copy_NoData(*sda);
  }
  int a_cols = src_a->data.size; // -1 if skipping index value
  for(int i=0; i < src_b->data.size; i++) {
    DataArray_impl* sdb = src_b->data.FastEl(i);
    DataArray_impl* nda = (DataArray_impl*)sdb->MakeToken();
    dest->data.Add(nda);
    nda->Copy_NoData(*sdb);
  }    
  dest->UniqueColNames();	// make them unique!
  for(int row=0;row<src_a->rows;row++) {
    dest->AddBlankRow();
    for(int i=0;i<src_a->data.size; i++) {
      DataArray_impl* sda = src_a->data.FastEl(i);
      DataArray_impl* nda = dest->data.FastEl(i); // todo: change above if uncommented
      nda->CopyFromRow(row, *sda, row); // just copy
    }
    int col_idx = a_cols;
    for(int i=0; i < src_b->data.size; i++) {
      DataArray_impl* sdb = src_b->data.FastEl(i);
      DataArray_impl* nda = dest->data.FastEl(col_idx);
      nda->CopyFromRow(row, *sdb, row); // just copy
      col_idx++;
    }    
  }
  dest->StructUpdate(false);
  return true;
}

///////////////////////////////////////////////////////////////////
// misc data functions

bool taDataProc::GetDirFiles(DataTable* dest, const String& dir_path, 
			     const String& filter, bool recursive,
			     const String& fname_col_nm,
			     const String& path_col_nm) {
  if(!dest) return false;
  dest->StructUpdate(true);

  if(recursive) {
    taMisc::Warning("Warning: GetDirFiles does not yet support the recursive flag!");
  }

  int fname_idx = -1;
  if(!fname_col_nm.empty())
    dest->FindMakeColName(fname_col_nm, fname_idx, DataTable::VT_STRING, 0);

  int path_idx = -1;
  if(!path_col_nm.empty())
    dest->FindMakeColName(path_col_nm, path_idx, DataTable::VT_STRING, 0);

  bool found_some = false;
  QDir dir(dir_path);
  QStringList files = dir.entryList();
  if(files.size() == 0) return false;
  for(int i=0;i<files.size();i++) {
    String fl = files[i];
    if(filter.empty() || fl.contains(filter)) {
      dest->AddBlankRow();
      found_some = true;
      if(fname_idx >= 0) {
	dest->SetValAsString(fl, fname_idx, -1);
      }
      if(path_idx >= 0) {
	dest->SetValAsString(dir_path + "/" + fl, path_idx, -1);
      }
    }
    // todo: deal with recursive flag
  }
  
  return found_some;
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
  if(!src_data) return;
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

void DataSortProg::AddAllColumns() {
  sort_spec.AddAllColumns(src_data);
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
  if(!src_data) return;
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

void DataSelectRowsProg::AddAllColumns() {
  select_spec.AddAllColumns(src_data);
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
  if(!src_data) return;
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

void DataSelectColsProg::AddAllColumns() {
  select_spec.AddAllColumns(src_data);
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
  if(!src_data) return;
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

void DataGroupProg::AddAllColumns() {
  group_spec.AddAllColumns(src_data);
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
  if(!src_data) return;
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
  for(int i=0;i<loop_code.size;i++) {
    ProgEl* pe = loop_code[i];
    if(pe->InheritsFrom(&TA_DataCalcAddDestRow) || pe->InheritsFrom(&TA_DataCalcSetDestRow))
      pe->UpdateAfterEdit();	// get the data tables!
  }
}

void DataCalcLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  // ok! just as long as you don't call set dest!
//   if(!dest_data) {		// not ok!
//     if(!quiet) taMisc::CheckError("Error in DataCalcLoop:",GetPath(),
// 				  "dest_data is NULL");
//     rval = false;
//   }
}

String DataCalcLoop::GetDisplayName() const {
  String rval = "Calc Loop ";
  if(src_data) {
    rval += " from: " + src_data->name;
  }
  else {
    rval += "ERR! src_data is NULL";
  }
  if(dest_data) {
    rval += " to: " + dest_data->name;
  }
  return rval;
}

void DataCalcLoop::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(src_data) {
    src_cols.GetColumns(src_data);
    src_cols.CheckConfig(quiet, rval);
    src_cols.ClearColumns();
  }
  if(dest_data) {
    dest_cols.GetColumns(dest_data);
    dest_cols.CheckConfig(quiet, rval);
    dest_cols.ClearColumns();
  }
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
  if(dest_data) {
    rval += il1 + "dcl->dest_data.ResetData(); // all data ops clear out old existing data\n";
    rval += il1 + "DataOpList common_dest_cols; // pre-initialize, for CopyCommonCols\n";
    rval += il1 + "DataOpList common_src_cols;\n";
    rval += il1 + "DataOpList common_dest_cols_named; // only the cols named in dest_cols\n";
    rval += il1 + "DataOpList common_src_cols_named;  // only the cols named in src_cols\n";
    rval += il1 + "taDataProc::GetCommonCols(dcl->dest_data, dcl->src_data, common_dest_cols, common_src_cols);\n";
    rval += il1 + "common_dest_cols_named = common_dest_cols; common_src_cols_named = common_src_cols;\n";
    rval += il1 + "taDataProc::GetColIntersection(common_dest_cols_named, dcl->dest_cols);\n";
    rval += il1 + "taDataProc::GetColIntersection(common_dest_cols_named, dcl->src_cols);\n";
    rval += il1 + "taDataProc::GetColIntersection(common_src_cols_named, dcl->dest_cols);\n";
    rval += il1 + "taDataProc::GetColIntersection(common_src_cols_named, dcl->src_cols);\n";
  }    
  
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

void DataCalcLoop::AddAllSrcColumns() {
  src_cols.AddAllColumns(src_data);
}
void DataCalcLoop::AddAllDestColumns() {
  dest_cols.AddAllColumns(dest_data);
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
  else {
    rval += "ERR! dest_data is NULL";
  }
  return rval;
}

void DataCalcAddDestRow::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  if(!dcl->src_data || !dcl->src_data->isDestroying())
    src_data = dcl->src_data;
  if(!dcl->dest_data || !dcl->dest_data->isDestroying())
    dest_data = dcl->dest_data;
}

void DataCalcAddDestRow::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetDataPtrsFmLoop();
}

void DataCalcAddDestRow::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}  

void DataCalcAddDestRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    if(!quiet) taMisc::CheckError("DataCalcAddDestRow: parent DataCalcLoop not found in:",
				  GetPath());
    rval = false;
  }
  if(dcl && !dcl->dest_data) {		// not ok!
     if(!quiet) taMisc::CheckError("Error in DataCalcAddDestRow:",GetPath(),
				   "DataCalcLoop::dest_data is NULL, but AddDestRow exists!");
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
//   data calc set dest row
/////////////////////////////////////////////////////////

void DataCalcSetDestRow::Initialize() {
}

String DataCalcSetDestRow::GetDisplayName() const {
  String rval = "Set Row in: ";
  if(dest_data) {
    rval += dest_data->name;
  }
  else {
    rval += "ERR! dest_data is NULL";
  }
  return rval;
}

void DataCalcSetDestRow::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  if(!dcl->src_data || !dcl->src_data->isDestroying())
    src_data = dcl->src_data;
  if(!dcl->dest_data || !dcl->dest_data->isDestroying())
    dest_data = dcl->dest_data;
}

void DataCalcSetDestRow::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetDataPtrsFmLoop();
}

void DataCalcSetDestRow::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}  

void DataCalcSetDestRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    if(!quiet) taMisc::CheckError("DataCalcSetDestRow: parent DataCalcLoop not found in:",
				  GetPath());
    rval = false;
  }
  if(dcl && !dcl->dest_data) {		// not ok!
     if(!quiet) taMisc::CheckError("Error in DataCalcSetDestRow:",GetPath(),
				   "DataCalcLoop::dest_data is NULL, but SetDestRow exists!");
     rval = false;
  }
}

const String DataCalcSetDestRow::GenCssBody_impl(int indent_level) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return "// DataCalcSetDestRow Error -- DataCalcLoop not found!!\n";

  String il = cssMisc::Indent(indent_level);

  String rval;
  rval += il + "if(dcl->dest_data.rows == 0) { taMisc::Error(\"Dest Rows == 0 -- forgot AddDestRow??\"); break; }\n";
  dcl->dest_cols.GetColumns(dcl->dest_data);
  for(int i=0;i<dcl->dest_cols.size; i++) {
    DataOpEl* ds = dcl->dest_cols[i];
    rval += il + "dcl->dest_data.SetValAsVar(" +
      "d_" + ds->col_name + ", " + String(ds->col_idx) + ", -1); // -1 = last row\n";
  }
  dcl->dest_cols.ClearColumns();
  return rval;
}

/////////////////////////////////////////////////////////
//   data calc set src row
/////////////////////////////////////////////////////////

void DataCalcSetSrcRow::Initialize() {
}

String DataCalcSetSrcRow::GetDisplayName() const {
  String rval = "Set Row in: ";
  if(src_data) {
    rval += src_data->name;
  }
  else {
    rval += "ERR! src_data is NULL";
  }
  return rval;
}

void DataCalcSetSrcRow::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  if(!dcl->src_data || !dcl->src_data->isDestroying())
    src_data = dcl->src_data;
  if(!dcl->dest_data || !dcl->dest_data->isDestroying())
    dest_data = dcl->dest_data;
}

void DataCalcSetSrcRow::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetDataPtrsFmLoop();
}

void DataCalcSetSrcRow::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}  

void DataCalcSetSrcRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    if(!quiet) taMisc::CheckError("DataCalcSetSrcRow: parent DataCalcLoop not found in:",
				  GetPath());
    rval = false;
  }
}

const String DataCalcSetSrcRow::GenCssBody_impl(int indent_level) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return "// DataCalcSetSrcRow Error -- DataCalcLoop not found!!\n";

  String rval;
  dcl->src_cols.GetColumns(dcl->src_data);
  for(int i=0;i<dcl->src_cols.size; i++) {
    DataOpEl* ds = dcl->src_cols[i];
    rval += cssMisc::Indent(indent_level) + "dcl->src_data.SetValAsVar(" +
      "s_" + ds->col_name + ", " + String(ds->col_idx) + ", src_row);\n";
  }
  dcl->dest_cols.ClearColumns();
  return rval;
}

/////////////////////////////////////////////////////////
//   data calc copy common cols
/////////////////////////////////////////////////////////

void DataCalcCopyCommonCols::Initialize() {
  only_named_cols = false;
}

String DataCalcCopyCommonCols::GetDisplayName() const {
  String rval = "Copy Common Cols from: ";
  if(src_data)
    rval += src_data->name;
  else
    rval += "ERR! src_data is NULL";
  rval += " to: ";
  if(dest_data)
    rval += dest_data->name;
  else
    rval += "ERR! dest_data is NULL";
  return rval;
}

void DataCalcCopyCommonCols::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  if(!dcl->src_data || !dcl->src_data->isDestroying())
    src_data = dcl->src_data;
  if(!dcl->dest_data || !dcl->dest_data->isDestroying())
    dest_data = dcl->dest_data;
}

void DataCalcCopyCommonCols::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  GetDataPtrsFmLoop();
}

void DataCalcCopyCommonCols::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}  

void DataCalcCopyCommonCols::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    if(!quiet) taMisc::CheckError("DataCalcCopyCommonCols: parent DataCalcLoop not found in:",
				  GetPath());
    rval = false;
  }
  if(dcl && !dcl->dest_data) {		// not ok!
     if(!quiet) taMisc::CheckError("Error in DataCalcCopyCommonCols:",GetPath(),
				   "DataCalcLoop::dest_data is NULL, but CopyCommonCols exists!");
     rval = false;
  }
}

const String DataCalcCopyCommonCols::GenCssBody_impl(int indent_level) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return "// DataCalcCopyCommonCols Error -- DataCalcLoop not found!!\n";

  String il = cssMisc::Indent(indent_level);

  String rval;
  rval += il + "if(dcl->dest_data.rows == 0) { taMisc::Error(\"Dest Rows == 0 -- forgot AddDestRow??\"); break; }\n";
  if(only_named_cols) 
    rval += il + "taDataProc::CopyCommonColsRow(dcl->dest_data, dcl->src_data, common_dest_cols_named, common_src_cols_named, -1, src_row);\n";
  else
    rval += il + "taDataProc::CopyCommonColsRow(dcl->dest_data, dcl->src_data, common_dest_cols, common_src_cols, -1, src_row);\n";
  return rval;
}

