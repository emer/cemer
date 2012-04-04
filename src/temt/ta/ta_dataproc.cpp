// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "ta_dataproc.h"
#include "css_machine.h"
#include "ta_project.h"		// for debugging

/////////////////////////////////////////////////////////
//   DataOpEl Base class
/////////////////////////////////////////////////////////

void DataOpEl::Initialize() {
  col_lookup = NULL;
  col_idx = -1;
}

void DataOpEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(col_lookup) {
    col_name = col_lookup->name;
    taBase::SetPointer((taBase**)&col_lookup, NULL); // reset as soon as used -- just a temp guy!
  }
  if(!data_table) {
    data_cols.set(NULL);
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
  CheckError(col_name.empty(), quiet, rval,"col_name is empty");
  //  CheckError(col_idx < 0, quiet, rval,"could not find", col_name,"in datatable");
  // note: an error can be too strong and prevent transitional code from running -- sometimes
  // at compile time the names aren't right, but they later end up being ok..
  ProgEl* pel = GET_MY_OWNER(ProgEl);
  if(pel && pel->HasProgFlag(ProgEl::QUIET))
    return;
  TestWarning(col_idx < 0, "CheckConfig", "could not find", col_name,"in datatable");
}

void DataOpEl::SetDataTable(DataTable* dt) {
  data_table = dt;
  if(!dt)
    data_cols.set(NULL);
  else
    data_cols.set(&dt->data);
}

void DataOpEl::GetColumns(DataTable* dt) {
  if(!dt) return;
  col_idx = dt->FindColNameIdx(col_name);
  DataCol* da = NULL;
  if(col_idx >= 0)
    da = dt->data[col_idx];
  taBase::SetPointer((taBase**)&col_lookup, da);
}

void DataOpEl::ClearColumns() {
  taBase::SetPointer((taBase**)&col_lookup, NULL);
}

///////////////////////////

void DataOpList::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
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
  dop->col_name = col_name;
  dop->SetDataTable(dt);
  dop->UpdateAfterEdit();
  if(taMisc::gui_active)
    tabMisc::DelayedFunCall_gui(dop, "BrowserSelectMe");
  return dop;
}

void DataOpList::AddAllColumns(DataTable* dt) {
  if(!dt) return;
  for(int i=0;i<dt->data.size; i++) {
    DataCol* da = dt->data[i];
    DataOpEl* dop = FindName(da->name);
    if(dop) continue;
    dop = (DataOpEl*)New(1);
    dop->col_name = da->name;
    dop->DataChanged(DCR_ITEM_UPDATED);
  }
  SetDataTable(dt);
}

void DataOpList::AddAllColumns_gui(DataTable* dt) {
  if(!dt) return;
  AddAllColumns(dt);
  if(size > 0 && taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(FastEl(size-1), "BrowserSelectMe");
  }
}


void DataOpBaseSpec::Initialize() {
}

void DataOpBaseSpec::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  ops.CheckConfig(quiet, rval);
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
  if(col_lookup) {
    CheckError(col_lookup->is_matrix, quiet, rval,"cannot use matrix column to sort");
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
  if(col_lookup) {
    CheckError((agg.op == Aggregate::GROUP) && col_lookup->isMatrix(), quiet, rval,
	       "cannot use matrix column to GROUP");
    ValType mvt = agg.MinValType();
    CheckError((mvt == Aggregate::VT_INT) && !col_lookup->isNumeric(), quiet, rval,
	       "aggregation operator:", agg.GetAggName(), "requires numeric data to operate on, but column named:", col_name, "is not numeric");
  }
}

void DataGroupSpec::Initialize() {
  ops.SetBaseType(&TA_DataGroupEl);
  append_agg_name = true;
}

///////////////////////////

void DataSelectEl::Initialize() {
  on = true;
  rel = EQUAL;
  use_var = false;
  cmp = 0;
  act_enabled = true;
}

String DataSelectEl::GetDisplayName() const {
  String rval = col_name + " " + 
    GetTypeDef()->GetEnumString("Relations", rel)+ " ";
  if(use_var && (bool)var)
    rval += var->name;
  else
    rval += cmp.toString();
  return rval;
}

bool DataSelectEl::Eval(const Variant& val) {
  if(use_var && (bool)var) {
    cmp = var->GetVar();	// get current val
  }
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

void DataSelectEl::UpdateEnabled() {
  act_enabled = on;
  if((bool)enable_var) {
    act_enabled = on && enable_var->GetVar().toBool();
  }
}

void DataSelectEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(col_lookup) {
    CheckError(col_lookup->is_matrix && col_lookup->cell_size() > 1 && 
	       !(rel == EQUAL || rel == NOTEQUAL), quiet, rval,
	       "matrix column selection only uses EQUAL or NOTEQUAL and checks for a 1 value at matrix cell given by cmp index");
  }
  CheckError(use_var && !var, quiet, rval, "use_var is selected but no var variable is set!");
  if(use_var && (bool)var) {
    CheckError(var->HasVarFlag(ProgVar::LOCAL_VAR), quiet, rval, "var for use_var is a local variable -- must be a global var in .args or .vars");
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

void DataSelectSpec::UpdateEnabled() {
  for(int i=0; i<ops.size; i++) {
    DataSelectEl* el = (DataSelectEl*)ops[i];
    el->UpdateEnabled();
  }
}

///////////////////////////

void DataJoinSpec::Initialize() {
  type = INNER;
  nomatch_warn = true;
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

bool taDataProc::GetDest(DataTable*& dest, const DataTable* src, const String& suffix,
			 bool& in_place_req) {
  if(src == dest) {
    in_place_req = true;
    dest = new DataTable;
    taBase::Ref(dest);
    return true;
  }
  in_place_req = false;
  if(dest) return false;
  taProject* proj = GET_OWNER(src, taProject);
  dest = proj->GetNewAnalysisDataTable(src->name + "_" + suffix, true);
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
    int d_idx = dest->FindColNameIdx(sop->col_name);
    if(d_idx < 0) {
      src_cols->RemoveIdx(i); i--;
      continue;
    }
    DataCol* dda = dest->data[d_idx];
    DataCol* sda = src->data[sop->col_idx];
    if(dda->cell_size() != sda->cell_size()) {
      src_cols->RemoveIdx(i); i--;
      continue; // incompatible
    }
    if(sda->cell_size() > 1) {
      if(sda->valType() != dda->valType()) {
	src_cols->RemoveIdx(i); i--;
	continue; // must be compatible var types
      }
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

bool taDataProc::CopyData(DataTable* dest, DataTable* src) {
  if(!src) { taMisc::Error("taDataProc::CopyData: src is NULL"); return false; }
  bool in_place_req = false;
  GetDest(dest, src, "CopyData", in_place_req);
  if(in_place_req) {
    taMisc::Error("taDataProc::CopyData -- src cannot be same as dest for this operation!");
    delete dest;
    return false;
  }
  dest->StructUpdate(true);
  dest->Reset();
  dest->Copy_NoData(*src);		// give it same structure
  AppendRows(dest, src);
  dest->StructUpdate(false);
  return true;
}

bool taDataProc::CopyCommonColsRow_impl(DataTable* dest, DataTable* src, DataOpList* dest_cols,
				   DataOpList* src_cols, int dest_row, int src_row) {
  if(!dest || !src || !dest_cols || !src_cols) return false;
  if(dest == src) {
    taMisc::Error("taDataProc::CopyCommonColsRow_impl -- src cannot be same as dest for this operation!");
    return false;
  }
  dest->DataUpdate(true);
  for(int j=0;j<src_cols->size;j++) {
    DataOpEl* sop = src_cols->FastEl(j);
    DataOpEl* dop = dest_cols->FastEl(j);
    if(sop->col_idx < 0 || dop->col_idx < 0) continue;
    DataCol* sda = src->data[sop->col_idx];
    DataCol* dda = dest->data[dop->col_idx];
    dda->CopyFromRow(dest_row, *sda, src_row);
  }
  dest->DataUpdate(false);
  return true;
}

bool taDataProc::CopyCommonColsRow(DataTable* dest, DataTable* src, int dest_row, int src_row) {
  if(!dest) { taMisc::Error("taDataProc::CopyCommonColsRow: dest is NULL"); return false; }
  if(!src) { taMisc::Error("taDataProc::CopyCommonColsRow: src is NULL"); return false; }
  if(dest == src) {
    taMisc::Error("taDataProc::CopyCommonColsRow -- src cannot be same as dest for this operation!");
    return false;
  }
  DataOpList dest_cols;
  DataOpList src_cols;
  GetCommonCols(dest, src, &dest_cols, &src_cols);
  return CopyCommonColsRow_impl(dest, src, &dest_cols, &src_cols, dest_row, src_row);
}

bool taDataProc::CopyCommonColData(DataTable* dest, DataTable* src) {
  if(!dest) { taMisc::Error("taDataProc::CopyCommonColData: dest is NULL"); return false; }
  if(!src) { taMisc::Error("taDataProc::CopyCommonColData: src is NULL"); return false; }
  if(dest == src) {
    taMisc::Error("taDataProc::CopyCommonColData -- src cannot be same as dest for this operation!");
    return false;
  }
  DataOpList dest_cols;
  DataOpList src_cols;
  GetCommonCols(dest, src, &dest_cols, &src_cols);
  dest->StructUpdate(true);
  for(int i=0;i<src->rows;i++) {
    dest->AddBlankRow();
    for(int j=0;j<src_cols.size;j++) {
      DataOpEl* sop = src_cols.FastEl(j);
      DataOpEl* dop = dest_cols.FastEl(j);
      if(sop->col_idx < 0 || dop->col_idx < 0) continue;
      DataCol* sda = src->data[sop->col_idx];
      DataCol* dda = dest->data[dop->col_idx];
      dda->CopyFromRow(-1, *sda, i);
    }
  }
  dest->StructUpdate(false);
  return true;
}

bool taDataProc::AppendRows(DataTable* dest, DataTable* src) {
  if(!dest) { taMisc::Error("taDataProc::AppendRows: dest is NULL"); return false; }
  if(!src) { taMisc::Error("taDataProc::AppendRows: src is NULL"); return false; }
  if(dest == src) {
    taMisc::Error("taDataProc::AppendRows -- src cannot be same as dest for this operation!");
    return false;
  }
  if(dest->data.size != src->data.size) {
    taMisc::Error("taDataProc::AppendRows -- tables do not have same number of columns -- use CopyCommonColData instead!");
    return false;
  }
  dest->StructUpdate(true);
  for(int i=0;i<src->rows;i++) {
    dest->AddBlankRow();
    for(int j=0;j<src->data.size;j++) {
      DataCol* sda = src->data[j];
      DataCol* dda = dest->data[j];
      dda->CopyFromRow(-1, *sda, i);
    }
  }
  dest->StructUpdate(false);
  return true;
}

bool taDataProc::ReplicateRows(DataTable* dest, DataTable* src, int n_repl) {
  bool in_place_req = false;
  GetDest(dest, src, "ReplicateRows", in_place_req);
  dest->StructUpdate(true);
  dest->Copy_NoData(*src);		// give it same structure
  for(int i=0;i<src->rows;i++) {
    for(int k=0;k<n_repl;k++) {
      dest->AddBlankRow();
      for(int j=0;j<src->data.size;j++) {
	DataCol* sda = src->data[j];
	DataCol* dda = dest->data[j];
	dda->CopyFromRow(-1, *sda, i);
      }
    }
  }
  dest->StructUpdate(false);
  if(in_place_req) {
    src->Copy_DataOnly(*dest);
    delete dest;
  }
  return true;
}

bool taDataProc::ConcatRows(DataTable* dest, DataTable* src_a, DataTable* src_b, DataTable* src_c,
			    DataTable* src_d, DataTable* src_e, DataTable* src_f) {
  if(!src_a) { taMisc::Error("taDataProc::ConcatRows: src_a is NULL"); return false; }
  if(!src_b) { taMisc::Error("taDataProc::ConcatRows: src_b is NULL"); return false; }
  bool in_place_req = false;
  GetDest(dest, src_a, "ConcatRows", in_place_req);
  if(in_place_req) {
    taMisc::Error("taDataProc::ConcatRows -- src_a cannot be same as dest for this operation!");
    delete dest;
    return false;
  }
  dest->StructUpdate(true);
  dest->Reset();
  String dnm = dest->name;
  *dest = *src_a;
  dest->name = dnm;
  CopyCommonColData(dest, src_b);
  if(src_c) CopyCommonColData(dest, src_c);
  if(src_d) CopyCommonColData(dest, src_d);
  if(src_e) CopyCommonColData(dest, src_e);
  if(src_f) CopyCommonColData(dest, src_f);
  dest->StructUpdate(false);
  return true;
}

bool taDataProc::AllDataToOne2DCell(DataTable* dest, DataTable* src, ValType val_type,
				    const String& col_nm_contains, const String& dest_col_nm) {
  bool in_place_req = false;
  GetDest(dest, src, "AllDataToOneCell", in_place_req);
  if(in_place_req) {
    taMisc::Error("taDataProc::ConcatRows -- src_a cannot be same as dest for this operation!");
    delete dest;
    return false;
  }
  dest->StructUpdate(true);
  int xsize = 0;
  int ysize = 0;
  int n_cols = 0;
  DataCol* first_da = NULL;
  for(int i=0;i<src->data.size; i++) { 
    DataCol* da = src->data[i];
    if(da->valType() != val_type) continue;
    if(col_nm_contains.nonempty() && !da->name.contains(col_nm_contains)) continue;
    n_cols++;
    if(n_cols == 1) first_da = da;
    if(da->isMatrix())
      xsize += da->cell_size();
    else
      xsize++;
  }
  
  if(n_cols == 1 && first_da->isMatrix()) {		// special case
    xsize = first_da->cell_geom[0];
    int src_cel_sz = first_da->cell_size();
    int dst_cel_sz = src_cel_sz / xsize;
    ysize = dst_cel_sz * src->rows;

    DataCol* dda = dest->FindMakeColMatrix(dest_col_nm, val_type, 2, xsize, ysize);
    dest->EnforceRows(1);
    for(int row=0;row<src->rows;row++) {
      for(int cell=0;cell<src_cel_sz;cell++) {
	int src_abs_idx = row * src_cel_sz + cell;
	dda->SetValAsVarM(first_da->GetValAsVarM(row, cell), 0, src_abs_idx);
      }
    }
  }
  else {	
    ysize = src->rows;
    DataCol* dda = dest->FindMakeColMatrix(dest_col_nm, val_type, 2, xsize, ysize);
    dest->EnforceRows(1);

    for(int row=0;row<src->rows;row++) {
      int stoff = row * xsize;
      for(int i=0;i<src->data.size; i++) { 
	DataCol* da = src->data[i];
	if(da->valType() != val_type) continue;
	if(col_nm_contains.nonempty() && !da->name.contains(col_nm_contains)) continue;
	if(da->isMatrix()) {
	  int src_cel_sz = da->cell_size();
	  for(int k=0;k<src_cel_sz;k++) {
	    dda->SetValAsVarM(da->GetValAsVarM(row, k), 0, stoff + k);
	  }
	  stoff += src_cel_sz;
	}
	else {
	  dda->SetValAsVarM(da->GetValAsVar(row), 0, stoff);
	  stoff++;
	}
      }
    }
  }

  dest->StructUpdate(false);
  return true;
}

bool taDataProc::Slice2D(DataTable* dest, DataTable* src, int src_row, String src_col_nm, int dest_row, String dest_col_nm,
			 int d0_start, int d0_end, int d1_start, int d1_end) {
  int src_dims, src_col, d0_src_size, d1_src_size, d0_dest_size, d1_dest_size, i, j, k, l;

  if(dest_col_nm == "") dest_col_nm = "SliceData";
  bool in_place_req = false;
  GetDest(dest, src, dest_col_nm, in_place_req);
  if(in_place_req) {taMisc::Error("taDataProc::Slice2D -- src cannot be same as dest for this operation!");delete dest;return false;}
  dest->StructUpdate(true);

  if(!src) {taMisc::Error("taDataProc::Slice: src is NULL"); return false;}
  if(!dest) {taMisc::Error("taDataProc::Slice: dest is NULL"); return false;}
  if(src_row > src->rows-1) {taMisc::Error("src_data does not have src_row rows."); return false;}

  if(src_col_nm == "") src_col_nm = src->data[0]->name;
  src_col = src->FindColNameIdx(src_col_nm);
  if(src_col < 0) {taMisc::Error("src_col does not exist in src_data."); return false;}

  src_dims = src->data[src_col]->cell_geom.dims();
  if(src_dims == 1) {taMisc::Error("src_col.dim() < 2. To slice from a 1d matrix use DataTable->GetMatrixVal."); return false;}

  d0_src_size = src->data[src_col]->cell_geom.dim(0);
  d1_src_size = src->data[src_col]->cell_geom.dim(1);
  if(d0_end == -1) d0_end = d0_src_size-1;
  if(d1_end == -1) d1_end = d1_src_size-1;
  if(d0_start > d0_src_size || d0_end > d0_src_size || d1_start > d1_src_size || d1_end > d1_src_size) {
    taMisc::Error("One of d0_start/d0_end/d1_start/d1_end is greater than the src_data matrix col dims"); return false;
  }
  d0_dest_size = d0_end-d0_start+1 > 0 ? d0_end-d0_start+1 : 1;
  d1_dest_size = d1_end-d1_start+1 > 0 ? d1_end-d1_start+1 : 1;

  dest->FindMakeColMatrix(dest_col_nm, src->data[src_col]->valType(), 2, d0_dest_size, d1_dest_size);
  if(dest_row == -1) dest->AddRows();
  else if(dest_row > dest->rows-1) dest->EnforceRows(dest_row+1);
  dest_row = dest->rows-1;

  for (i = d0_start,k = 0; i <= d0_end; i++, k++)
    for (j = d1_start,l = 0; j <= d1_end; j++, l++)
      dest->SetMatrixVal(src->GetMatrixVal(src_col_nm, src_row, i, j), dest_col_nm, dest_row, k, l);

  dest->StructUpdate(false);
  return true;
}

///////////////////////////////////////////////////////////////////
// reordering functions

bool taDataProc::Sort(DataTable* dest, DataTable* src, DataSortSpec* spec) {
  if(!src) { taMisc::Error("taDataProc::Sort: src is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataProc::Sort: spec is NULL"); return false; }
  if(src == dest) {
    return Sort_impl(src, spec);
  }
  // just copy and operate on dest
  bool in_place_req = false;
  GetDest(dest, src, "Sort", in_place_req);
  dest->Reset();
  String dnm = dest->name;
  *dest = *src;
  dest->name = dnm;
  return Sort_impl(dest, spec);
}

bool taDataProc::SortInPlace(DataTable* dt, DataSortSpec* spec) {
  if(!dt) { taMisc::Error("taDataProc::Sort: data table is NULL"); return false; }
  return Sort_impl(dt, spec);
}

int taDataProc::Sort_Compare(DataTable* dt_a, int row_a, DataTable* dt_b, int row_b,
			    DataSortSpec* spec) {
  for(int i=0;i<spec->ops.size; i++) {
    DataSortEl* ds = (DataSortEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue;
    DataCol* da_a = dt_a->data.FastEl(ds->col_idx);
    DataCol* da_b = dt_b->data.FastEl(ds->col_idx);
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
  DataTable tmp_data(false);		// temporary buffer to hold vals during swap
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
  if(!src) { taMisc::Error("taDataProc::Permute: src is NULL"); return false; }
  bool in_place_req = false;
  GetDest(dest, src, "Permute", in_place_req);
  dest->StructUpdate(true);
  dest->Copy_NoData(*src);		// give it same structure
  // this just uses the index technique..
  int_Array idxs;
  idxs.SetSize(src->rows);
  idxs.FillSeq();
  idxs.Permute();
  for(int row=0;row<src->rows; row++) {
    dest->AddBlankRow();
    dest->CopyFromRow(-1, *src, idxs[row]);
  }
  dest->StructUpdate(false);
  if(in_place_req) {
    // only copy the data, preserving all other attributes of the table
    src->Copy_DataOnly(*dest);
    delete dest;
  }
  return true;
}

bool taDataProc::Group(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
  if(!src) { taMisc::Error("taDataProc::Group: src is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataProc::Group: spec is NULL"); return false; }
  bool in_place_req = false;
  GetDest(dest, src, "Group", in_place_req);
  dest->StructUpdate(true);
  spec->GetColumns(src);		// cache column pointers & indicies from names
  dest->Reset();
  // add the dest columns
  for(int i=0;i<spec->ops.size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue;
    DataCol* sda = src->data.FastEl(ds->col_idx);
    DataCol* nda;
    // up-convert to float -- always needed for matrix
    if((sda->valType() == VT_INT) && ((ds->agg.MinReturnType() == VT_FLOAT) || sda->isMatrix()))
      nda = new float_Data;
    else if((sda->valType() == VT_STRING) && (ds->agg.MinReturnType() == VT_INT)) // N
      nda = new int_Data;
    else
      nda = (DataCol*)sda->MakeToken();
    dest->data.Add(nda);
    nda->Copy_NoData(*sda);
    if(ds->agg.op == Aggregate::N) {
      nda->name = "N";		// doesn't matter what the column was!
    }
    else if(spec->append_agg_name) {
      String dst_op = ds->agg.GetAggName();
      dst_op.downcase();
      nda->name += "_" + dst_op;
    }
  }    
  dest->UniqueColNames();	// make them unique!

  // sort by grouped guys, in order (also add "LAST" and "FIRST" -- important to be sorted)
  DataSortSpec sort_spec(false);
  taBase::Own(sort_spec, NULL);
  for(int i=0;i<spec->ops.size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue;
    if(ds->agg.op != Aggregate::GROUP) continue;
    DataSortEl* ss = (DataSortEl*)sort_spec.ops.New(1, &TA_DataSortEl);
    ss->col_name = ds->col_name;
    ss->col_idx = ds->col_idx;
    taBase::SetPointer((taBase**)&ss->col_lookup, ds->col_lookup);
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
  if(in_place_req) {
    src->Copy_DataOnly(*dest);
    delete dest;
  }
  return true;
}

bool taDataProc::Group_nogp(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
  float_Matrix float_tmp(false);
  dest->AddBlankRow();
  int dest_idx = 0;
  for(int i=0;i<spec->ops.size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue;
    DataCol* sda = src->data.FastEl(ds->col_idx);
    DataCol* dda = dest->data.FastEl(dest_idx++); // index is spec index
    if(sda->is_matrix) {
      if(sda->valType() == taBase::VT_DOUBLE) {
	double_Matrix* dmat = (double_Matrix*)dda->GetValAsMatrix(0);
	if(dmat) {
	  taBase::Ref(dmat);
	  taMath_double::mat_frame_aggregate(dmat, (double_Matrix*)sda->AR(), ds->agg);
	  taBase::unRefDone(dmat);
	}
      }
      else if(sda->valType() == taBase::VT_FLOAT) {
	float_Matrix* dmat = (float_Matrix*)dda->GetValAsMatrix(0);
	if(dmat) {
	  taBase::Ref(dmat);
	  taMath_float::mat_frame_aggregate(dmat, (float_Matrix*)sda->AR(), ds->agg);
	  taBase::unRefDone(dmat);
	}
      }
      else if(sda->valType() == taBase::VT_INT) {
	float_Matrix* dmat = (float_Matrix*)dda->GetValAsMatrix(0);
	if(dmat) {
	  taBase::Ref(dmat);
	  int_Matrix* mat = (int_Matrix*)sda->AR();
	  taMath_float::vec_fm_ints(&float_tmp, mat);
	  taMath_float::mat_frame_aggregate(dmat, &float_tmp, ds->agg);
	  taBase::unRefDone(dmat);
	}
      }
    }
    else {			// scalar
      if(sda->valType() == taBase::VT_DOUBLE) {
	dda->SetValAsDouble(taMath_double::vec_aggregate((double_Matrix*)sda->AR(), ds->agg), 0);
      }
      else if(sda->valType() == taBase::VT_FLOAT) {
	dda->SetValAsFloat(taMath_float::vec_aggregate((float_Matrix*)sda->AR(), ds->agg), 0);
      }
      else if(sda->valType() == taBase::VT_INT) {
	int_Matrix* mat = (int_Matrix*)sda->AR();
	taMath_float::vec_fm_ints(&float_tmp, mat);
	dda->SetValAsFloat(taMath_float::vec_aggregate(&float_tmp, ds->agg), 0);
      }
      else if(sda->valType() == taBase::VT_STRING) {
	String_Matrix* mat = (String_Matrix*)sda->AR();
	if(ds->agg.op == Aggregate::FIRST) {
	  dda->SetValAsString(mat->SafeElAsVar_Flat(0).toString(), 0);
	}
	else if(ds->agg.op == Aggregate::LAST) {
	  dda->SetValAsString(mat->SafeElAsVar_Flat(mat->size-1).toString(), 0);
	}
	else if(ds->agg.op == Aggregate::N) {
	  dda->SetValAsInt(mat->size, 0);
	}
      }
    }
  }
  return true;
}

bool taDataProc::Group_gp(DataTable* dest, DataTable* src, DataGroupSpec* spec, DataSortSpec* sort_spec) {
  DataTable ssrc(false);
  taBase::Own(ssrc, NULL);	// activates initlinks, refs

  DataSortSpec full_sort_spec(false);
  taBase::Own(full_sort_spec, NULL);
  full_sort_spec = *sort_spec;

  // for [FIND_] LAST and FIRST: need to add a column that has the row index and add that
  // to the sort, to preserve the current table order for these operations.
  bool has_first_last = false;
  for(int i=0;i<spec->ops.size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue;
    if((ds->agg.op == Aggregate::FIRST) || (ds->agg.op == Aggregate::LAST) ||
       (ds->agg.op == Aggregate::FIND_FIRST) || (ds->agg.op == Aggregate::FIND_LAST)) {
      has_first_last = true;
      break;
    }
  }
  DataTable flsrc(false);
  taBase::Own(flsrc, NULL);	// activates initlinks, refs
  DataTable* use_src = src;	// source to actually use -- could be flsrc instead
  if(has_first_last) {
    flsrc = *src;
    use_src = &flsrc;		// new src
    //    sort_spec->GetColumns(&flsrc); // re-get columns for new guy, just to be sure
    int_Data* rowno_col = flsrc.NewColInt("__fl_rowno");
    rowno_col->InitValsToRowNo();
    DataSortEl* ss = (DataSortEl*)full_sort_spec.ops.New(1, &TA_DataSortEl);
    ss->col_name = rowno_col->name;
    ss->col_idx = rowno_col->col_idx;
    //    taBase::SetPointer((taBase**)&ss->col_lookup, ds->col_lookup);
    ss->order = DataSortEl::ASCENDING;
  }

  taDataProc::Sort(&ssrc, use_src, &full_sort_spec);

  sort_spec->GetColumns(&ssrc);	// re-get columns -- they were nuked by Sort op!

  Variant_Array cur_vals;
  cur_vals.SetSize(sort_spec->ops.size);

  // initialize cur vals
  for(int i=0;i<sort_spec->ops.size; i++) {
    DataSortEl* ds = (DataSortEl*)sort_spec->ops.FastEl(i);
    DataCol* sda = ssrc.data.FastEl(ds->col_idx);
    Variant cval = sda->GetValAsVar(0); // start at row 0
    cur_vals[i] = cval;
  }

  float_Matrix float_tmp(false);
  int st_row = 0;
  int row = 1;
  while(row <= ssrc.rows) {
    for(;row <= ssrc.rows; row++) {
      bool new_val = false;
      if(row == ssrc.rows) { 
	new_val = true;
      }
      else {
	for(int i=0;i<sort_spec->ops.size; i++) {
	  DataSortEl* ds = (DataSortEl*)sort_spec->ops.FastEl(i);
	  DataCol* sda = ssrc.data.FastEl(ds->col_idx);
	  Variant cval = sda->GetValAsVar(row);
	  if(cval != cur_vals[i]) {
	    new_val = true;
	    cur_vals[i] = cval;
	  }
	}
      }
      if(new_val) break;
    }
    int n_rows = row - st_row;
    // now go through actual group ops!
    dest->AddBlankRow();
    int dest_idx = 0;
    for(int i=0;i<spec->ops.size; i++) {
      DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
      if(ds->col_idx < 0) continue;
      DataCol* sda = ssrc.data.FastEl(ds->col_idx);
      DataCol* dda = dest->data.FastEl(dest_idx++); // index is spec index
      if(ds->agg.op == Aggregate::GROUP) {
	dda->SetValAsVar(sda->GetValAsVar(st_row), -1); // -1 = last row
      }
      else {
	if(sda->isMatrix()) {
	  if(sda->valType() == taBase::VT_DOUBLE) {
	    double_Matrix* mat = (double_Matrix*)sda->GetRangeAsMatrix(st_row, n_rows);
	    double_Matrix* dmat = (double_Matrix*)dda->GetValAsMatrix(-1);
	    if(mat && dmat) {
	      taBase::Ref(mat); taBase::Ref(dmat);
	      taMath_double::mat_frame_aggregate(dmat, mat, ds->agg);
	      taBase::unRefDone(mat); taBase::unRefDone(dmat);
	    }
	  }
	  else if(sda->valType() == taBase::VT_FLOAT) {
	    float_Matrix* mat = (float_Matrix*)sda->GetRangeAsMatrix(st_row, n_rows);
	    float_Matrix* dmat = (float_Matrix*)dda->GetValAsMatrix(-1);
	    if(mat && dmat) {
	      taBase::Ref(mat); taBase::Ref(dmat);
	      taMath_float::mat_frame_aggregate(dmat, mat, ds->agg);
	      taBase::unRefDone(mat); taBase::unRefDone(dmat);
	    }
	  }
	  else if(sda->valType() == taBase::VT_INT) {
	    int_Matrix* mat = (int_Matrix*)sda->GetRangeAsMatrix(st_row, n_rows);
	    float_Matrix* dmat = (float_Matrix*)dda->GetValAsMatrix(-1);
	    if(mat && dmat) {
	      taBase::Ref(mat); taBase::Ref(dmat);
	      taMath_float::vec_fm_ints(&float_tmp, mat);
	      taMath_float::mat_frame_aggregate(dmat, &float_tmp, ds->agg);
	      taBase::unRefDone(mat); taBase::unRefDone(dmat);
	    }
	  }
	}
	else {			// scalar
	  if(sda->valType() == taBase::VT_DOUBLE) {
	    double_Matrix* mat = (double_Matrix*)sda->GetRangeAsMatrix(st_row, n_rows);
	    if(mat) {
	      taBase::Ref(mat);
	      dda->SetValAsDouble(taMath_double::vec_aggregate(mat, ds->agg), -1); // -1 = last row
	      taBase::unRefDone(mat);
	    }
	  }
	  else if(sda->valType() == taBase::VT_FLOAT) {
	    float_Matrix* mat = (float_Matrix*)sda->GetRangeAsMatrix(st_row, n_rows);
	    if(mat) {
	      taBase::Ref(mat);
	      dda->SetValAsFloat(taMath_float::vec_aggregate(mat, ds->agg), -1); // -1 = last row
	      taBase::unRefDone(mat);
	    }
	  }
	  else if(sda->valType() == taBase::VT_INT) {
	    int_Matrix* mat = (int_Matrix*)sda->GetRangeAsMatrix(st_row, n_rows);
	    if(mat) {
	      taBase::Ref(mat);
	      taMath_float::vec_fm_ints(&float_tmp, mat);
	      dda->SetValAsFloat(taMath_float::vec_aggregate(&float_tmp, ds->agg), -1);
	      taBase::unRefDone(mat);
	    }
	  }
	  else if(sda->valType() == taBase::VT_STRING) {
	    String_Matrix* mat = (String_Matrix*)sda->GetRangeAsMatrix(st_row, n_rows);
	    if(mat) {
	      taBase::Ref(mat);
	      if(ds->agg.op == Aggregate::FIRST) {
		dda->SetValAsString(mat->SafeElAsVar_Flat(0).toString(), -1);
	      }
	      else if(ds->agg.op == Aggregate::LAST) {
		dda->SetValAsString(mat->SafeElAsVar_Flat(mat->size-1).toString(), -1);
	      }
	      else if(ds->agg.op == Aggregate::N) {
		dda->SetValAsInt(mat->size, -1);
	      }
	      taBase::unRefDone(mat);
	    }
	  }
	}
      }
    }
    st_row = row;
    row++;			// move on to next row!
  }
  return true;
}

bool taDataProc::TransposeColsToRows(DataTable* dest, DataTable* src,
	     const Variant& data_col_st, int n_cols, const Variant& col_names_col) {
  bool in_place_req = false;
  GetDest(dest, src, "TransposeColsToRows", in_place_req);
  if(in_place_req) {
    taMisc::Error("taDataProc::TransposeColsToRows -- src cannot be same as dest for this operation!");
    delete dest;
    return false;
  }
  DataCol* st_data = src->GetColData(data_col_st, false); // quiet
  if(!st_data) return false;
  dest->StructUpdate(true);
  dest->RemoveAllCols();
  DataCol* names = src->GetColData(col_names_col, true); // quiet
  // make cols in the image of the st data col
  for(int i=0; i<src->rows; i++) {
    String nm;
    if(names) {
      nm = names->GetValAsString(i);
    }
    else {
      nm = "Row_" + String(i);
    }
    DataCol* nda = (DataCol*)st_data->MakeToken();
    dest->data.Add(nda);
    nda->Copy_NoData(*st_data);
    nda->SetName(nm);
  }
  int st_idx = st_data->GetIndex();
  if(n_cols < 1 || st_idx + n_cols > dest->cols())
    n_cols = dest->cols() - st_idx;
  for(int ci = 0; ci < n_cols; ci++) {
    DataCol* scol = src->GetColData(ci + st_idx);
    if(!scol) continue;		// shouldn't happen
    dest->AddBlankRow();
    for(int i=0; i<src->rows; i++) {
      DataCol* dcol = dest->GetColData(i);
      dcol->CopyFromRow_Robust(-1, *scol, i);
    }
  }
  dest->StructUpdate(false);
  return true;
}


bool taDataProc::TransposeRowsToCols(DataTable* dest, DataTable* src, int st_row, int n_rows,
				     DataCol::ValType val_type) {
  bool in_place_req = false;
  GetDest(dest, src, "TransposeRowsToCols", in_place_req);
  if(in_place_req) {
    taMisc::Error("taDataProc::TransposeRowsToCols -- src cannot be same as dest for this operation!");
    delete dest;
    return false;
  }
  dest->StructUpdate(true);
  dest->RemoveAllCols();
  if(n_rows < 0) 
    n_rows = src->rows - st_row;
  for(int i=0; i<n_rows; i++) {
    String nm = "Row_" + String(i);
    dest->NewCol(val_type, nm);
  }
  dest->EnforceRows(src->cols());
  for(int i = 0; i < n_rows; i++) {
    DataCol* dcol = dest->GetColData(i);
    dest->AddBlankRow();
    for(int ci=0; ci<src->cols(); ci++) {
      DataCol* scol = src->GetColData(ci);
      dcol->CopyFromRow_Robust(ci, *scol, i);
    }
  }
  dest->StructUpdate(false);
  return true;
}


///////////////////////////////////////////////////////////////////
// row-wise functions: selecting/splitting

bool taDataProc::SelectRows(DataTable* dest, DataTable* src, DataSelectSpec* spec) {
  bool in_place_req = false;
  GetDest(dest, src, "SelectRows", in_place_req);
  dest->StructUpdate(true);
  dest->Copy_NoData(*src);		// give it same structure
  spec->GetColumns(src);		// cache column pointers & indicies from names
  // also sets act_enabled flag
  for(int row=0;row<src->rows; row++) {
    bool incl = false;
    bool not_incl = false;
    for(int i=0; i<spec->ops.size; i++) {
      DataSelectEl* ds = (DataSelectEl*)spec->ops.FastEl(i);
      if(!ds->act_enabled || ds->col_idx < 0) continue;
      DataCol* da = src->data.FastEl(ds->col_idx);
      bool ev = false;
      if(da->isMatrix()) {
	if(da->cell_size() == 1) { // degenerate case
	  Variant val = da->GetValAsVarM(row, 0);
	  ev = ds->Eval(val);
	}
	else {
	  if(ds->use_var && (bool)ds->var) {
	    ds->cmp = ds->var->GetVar();	// get current val
	  }
	  int cmpidx = ds->cmp.toInt(); // convert to an integer index
	  int vl = da->GetValAsIntM(row, cmpidx); // use as a cell number
	  if(ds->rel == DataSelectEl::EQUAL)
	    ev = (vl == 1);			  // true if cell value is 1
	  else
	    ev = (vl != 1);			  // true if cell value is NOT 1
	}
      }
      else {
	Variant val = da->GetValAsVar(row);
	ev = ds->Eval(val);
      }
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
  if(in_place_req) {
    src->Copy_DataOnly(*dest);
    delete dest;
  }
  return true;
}

bool taDataProc::SplitRows(DataTable* dest_a, DataTable* dest_b, DataTable* src,
			   DataSelectSpec* spec) {
  if(!src) { taMisc::Error("taDataProc::SplitRows: src is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataProc::SplitRows: spec is NULL"); return false; }
  bool in_place_req = false;
  GetDest(dest_a, src, "SplitRows_a", in_place_req);
  if(in_place_req) {
    taMisc::Error("taDataProc::SplitRows -- src cannot be same as dest for this operation!");
    delete dest_a;
    return false;
  }
  GetDest(dest_b, src, "SplitRows_b", in_place_req);
  if(in_place_req) {
    taMisc::Error("taDataProc::SplitRows -- src cannot be same as dest for this operation!");
    delete dest_b;
    return false;
  }
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
      if(!ds->act_enabled || ds->col_idx < 0) continue;
      DataCol* da = src->data.FastEl(ds->col_idx);
      bool ev = false;
      if(da->isMatrix()) {
	if(da->cell_size() == 1) { // degenerate case
	  Variant val = da->GetValAsVarM(row, 0);
	  ev = ds->Eval(val);
	}
	else {
	  if(ds->use_var && (bool)ds->var) {
	    ds->cmp = ds->var->GetVar();	// get current val
	  }
	  int cmpidx = ds->cmp.toInt(); // convert to an integer index
	  int vl = da->GetValAsIntM(row, cmpidx); // use as a cell number
	  if(ds->rel == DataSelectEl::EQUAL)
	    ev = (vl == 1);			  // true if cell value is 1
	  else
	    ev = (vl != 1);			  // true if cell value is NOT 1
	}
      }
      else {
	Variant val = da->GetValAsVar(row);
	ev = ds->Eval(val);
      }
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
  if(!src) { taMisc::Error("taDataProc::SplitRowsN: src is NULL"); return false; }
  int nary[6] = {n1, n2, n3, n4, n5, n6};
  DataTable* dary[6] = {dest_1, dest_2, dest_3, dest_4, dest_5, dest_6};
  int n_split = 0;
  int rest_idx = -1;
  int n_tot = 0;
  for(int i=0;i<6;i++) {
    if(!dary[i]) break;
    bool in_place_req = false;
    GetDest(dary[i], src, "SplitByN_" + String(i), in_place_req);
    if(in_place_req) {
      taMisc::Error("taDataProc::SplitRowsN -- src cannot be same as dest for this operation!");
      delete dary[i];
      return false;
    }
    dary[i]->StructUpdate(true);
    dary[i]->Copy_NoData(*src);
    if(nary[i] < 0) {
      if(rest_idx >= 0) {
	taMisc::Error("SplitRowsN: cannot have multiple n = -1 = remainder cases");
	break;
      }
      rest_idx = i;
    }
    n_tot += nary[i];
    n_split++;
  }

  if(n_tot > src->rows) {
    taMisc::Warning("SplitRowsN: total N:", String(n_tot), "is > number of source rows:",
		    String(src->rows), "last one will be underfilled");
  }

  if(rest_idx >=0) {
    int n_rest = src->rows - n_tot;
    n_rest = MAX(n_rest, 0);
    nary[rest_idx] = n_rest;
  }

  int st_n = 0;
  int end_n = nary[0];
  int ni = 0;
  for(int row=0;row<src->rows; row++) {
    if(row < end_n) {
      dary[ni]->AddBlankRow();
      dary[ni]->CopyFromRow(-1, *src, row);
    }
    else {
      ni++;
      if(!dary[ni]) break;
      st_n = row;
      end_n = st_n + nary[ni];

      if(nary[ni] > 0) {
	dary[ni]->AddBlankRow();
	dary[ni]->CopyFromRow(-1, *src, row);
      }
      else {
	row--;			// negate this row
      }
    }
  }
  for(int i=0;i<6;i++) {
    if(!dary[i]) break;
    dary[i]->StructUpdate(false);
  }
  return true;
}

bool taDataProc::SplitRowsNPermuted(DataTable* src, DataTable* dest_1, int n1, DataTable* dest_2, int n2,
				    DataTable* dest_3, int n3, DataTable* dest_4, int n4,
				    DataTable* dest_5, int n5, DataTable* dest_6, int n6) {
  if(!src) { taMisc::Error("taDataProc::SplitRowsNPermuted: src is NULL"); return false; }
  int nary[6] = {n1, n2, n3, n4, n5, n6};
  DataTable* dary[6] = {dest_1, dest_2, dest_3, dest_4, dest_5, dest_6};
  int n_split = 0;
  int rest_idx = -1;
  int n_tot = 0;
  for(int i=0;i<6;i++) {
    if(!dary[i]) break;
    bool in_place_req = false;
    GetDest(dary[i], src, "SplitByN_" + String(i), in_place_req);
    if(in_place_req) {
      taMisc::Error("taDataProc::SplitRowsNPermuted -- src cannot be same as dest for this operation!");
      delete dary[i];
      return false;
    }
    dary[i]->StructUpdate(true);
    dary[i]->Copy_NoData(*src);
    if(nary[i] < 0) {
      if(rest_idx >= 0) {
	taMisc::Error("SplitRowsN: cannot have multiple n = -1 = remainder cases");
	break;
      }
      rest_idx = i;
    }
    n_tot += nary[i];
    n_split++;
  }

  if(n_tot > src->rows) {
    taMisc::Warning("SplitRowsN: total N:", String(n_tot), "is > number of source rows:",
		    String(src->rows), "last one will be underfilled");
  }

  if(rest_idx >=0) {
    int n_rest = src->rows - n_tot;
    n_rest = MAX(n_rest, 0);
    nary[rest_idx] = n_rest;
  }

  int_Array idxs;
  idxs.SetSize(src->rows);
  idxs.FillSeq();
  idxs.Permute();
  int st_n = 0;
  int end_n = nary[0];
  int ni = 0;
  for(int row=0;row<src->rows; row++) {
    if(row < end_n) {
      dary[ni]->AddBlankRow();
      dary[ni]->CopyFromRow(-1, *src, idxs[row]);
    }
    else {
      ni++;
      if(!dary[ni]) break;
      st_n = row;
      end_n = st_n + nary[ni];

      if(nary[ni] > 0) {
	dary[ni]->AddBlankRow();
	dary[ni]->CopyFromRow(-1, *src, idxs[row]);
      }
      else {
	row--;			// negate this row
      }
    }
  }
  for(int i=0;i<6;i++) {
    if(!dary[i]) break;
    dary[i]->StructUpdate(false);
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// column-wise functions: selecting, joining

bool taDataProc::SelectCols(DataTable* dest, DataTable* src, DataOpList* spec) {
  if(!src) { taMisc::Error("taDataProc::SelectCols: src is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataProc::SelectCols: spec is NULL"); return false; }
  bool in_place_req = false;
  GetDest(dest, src, "SelectCols", in_place_req);
  dest->StructUpdate(true);
  spec->GetColumns(src);		// cache column pointers & indicies from names
  dest->Reset();
  for(int i=0;i<spec->size; i++) {
    DataOpEl* ds = spec->FastEl(i);
    if(ds->col_idx < 0) continue;
    DataCol* sda = src->data.FastEl(ds->col_idx);
    DataCol* nda = (DataCol*)sda->MakeToken();
    dest->data.Add(nda);
    nda->Copy_NoData(*sda);
  }
  for(int row=0;row<src->rows;row++) {
    dest->AddBlankRow();
    int dest_idx = 0;
    for(int i=0;i<spec->size; i++) {
      DataOpEl* ds = spec->FastEl(i);
      if(ds->col_idx < 0) continue;
      DataCol* sda = src->data.FastEl(ds->col_idx);
      DataCol* nda = dest->data.SafeEl(dest_idx++);
      if(!nda) continue;
      nda->CopyFromRow(row, *sda, row);
    }
  }
  dest->StructUpdate(false);
  spec->ClearColumns();
  if(in_place_req) {
    src->Copy_DataOnly(*dest);
    delete dest;
  }
  return true;
}

bool taDataProc::Join(DataTable* dest, DataTable* src_a, DataTable* src_b,
		      DataJoinSpec* spec) {
  if(!src_a) { taMisc::Error("taDataProc::Join: src_a is NULL"); return false; }
  if(!src_b) { taMisc::Error("taDataProc::Join: src_b is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataProc::Join: spec is NULL"); return false; }
  if((spec->col_a.col_idx < 0) || (spec->col_b.col_idx < 0)) return false;
  bool in_place_req = false;
  GetDest(dest, src_a, "Join", in_place_req);
  if(in_place_req) {
    taMisc::Error("taDataProc::Join -- src_a cannot be same as dest for this operation!");
    delete dest;
    return false;
  }
  dest->StructUpdate(true);
  spec->GetColumns(src_a, src_b);	// cache column pointers & indicies from names
  dest->Reset();
  for(int i=0; i < src_a->data.size; i++) {
    //    if(i == spec->col_a.col_idx) continue; // include first guy..
    DataCol* sda = src_a->data.FastEl(i);
    DataCol* nda = (DataCol*)sda->MakeToken();
    dest->data.Add(nda);	// todo: AddUniqueName?? + no reset + orphan?
    nda->Copy_NoData(*sda);
  }
  int a_cols = src_a->data.size; // -1 if skipping index value
  for(int i=0; i < src_b->data.size; i++) {
    if(i == spec->col_b.col_idx) continue; // don't include common index
    DataCol* sdb = src_b->data.FastEl(i);
    DataCol* nda = (DataCol*)sdb->MakeToken();
    dest->data.Add(nda);
    nda->Copy_NoData(*sdb);
  }
  dest->UniqueColNames();	// make them unique!

  DataSortSpec sort_spec_a(false);
  taBase::Own(sort_spec_a, NULL);
  {
    DataSortEl* ss = (DataSortEl*)sort_spec_a.ops.New(1, &TA_DataSortEl);
    ss->col_name = spec->col_a.col_name;
    ss->col_idx = spec->col_a.col_idx;
    taBase::SetPointer((taBase**)&ss->col_lookup, spec->col_a.col_lookup);
    ss->order = DataSortEl::ASCENDING;
  }
  
  DataSortSpec sort_spec_b(false);
  taBase::Own(sort_spec_b, NULL);
  {
    DataSortEl* ss = (DataSortEl*)sort_spec_b.ops.New(1, &TA_DataSortEl);
    ss->col_name = spec->col_b.col_name;
    ss->col_idx = spec->col_b.col_idx;
    taBase::SetPointer((taBase**)&ss->col_lookup, spec->col_b.col_lookup);
    ss->order = DataSortEl::ASCENDING;
  }

  DataTable ssrc_a(false);
  taBase::Own(ssrc_a, NULL);	// activates initlinks, refs
  taDataProc::Sort(&ssrc_a, src_a, &sort_spec_a);

  DataTable ssrc_b(false);
  taBase::Own(ssrc_b, NULL);	// activates initlinks, refs
  taDataProc::Sort(&ssrc_b, src_b, &sort_spec_b);

  int b_row = 0;
  for(int row=0;row<ssrc_a.rows;row++) {
    DataCol* sda = ssrc_a.data.FastEl(spec->col_a.col_idx);
    Variant val_a = sda->GetValAsVar(row);
    DataCol* sdb = ssrc_b.data.FastEl(spec->col_b.col_idx);
    Variant val_b = sdb->GetValAsVar(b_row);
    bool got_one = false;
    if(val_a == val_b) {
      got_one = true;
    }
    else {
      while(b_row < ssrc_b.rows-1 && val_b < val_a) {
	val_b = sdb->GetValAsVar(++b_row);
      }
      if(val_a == val_b) 
	got_one = true;
    }
    if(got_one) {
      Variant orig_b = val_b;
      int bi = b_row;
      while(val_a == val_b) {
	dest->AddBlankRow();
	for(int i=0;i<ssrc_a.data.size; i++) {
	  //    if(i == spec->col_a.col_idx) continue; // include first guy..
	  DataCol* sda = ssrc_a.data.FastEl(i);
	  DataCol* nda = dest->data.FastEl(i); // todo: change above if uncommented
	  nda->CopyFromRow(-1, *sda, row); // just copy
	}
	int col_idx = a_cols;
	for(int i=0; i < ssrc_b.data.size; i++) {
	  if(i == spec->col_b.col_idx) continue; // don't include common index
	  DataCol* sdb = ssrc_b.data.FastEl(i);
	  DataCol* nda = dest->data.FastEl(col_idx);
	  nda->CopyFromRow(-1, *sdb, bi); // just copy
	  col_idx++;
	}
	if(bi >= ssrc_b.rows-1) break; // done!
	val_b = sdb->GetValAsVar(++bi);
      }
      if(row < ssrc_a.rows-1) {
	Variant nxt_a = sda->GetValAsVar(row+1);
	if(nxt_a != orig_b)
	  b_row = bi;		// otherwise just go through same b's again
	if(b_row > ssrc_b.rows-1 && spec->nomatch_warn) {
	  taMisc::Warning("taDataProc::Join -- at end of src_b table:", src_b->name,
			  "with:",String(ssrc_a.rows-row-1),
			  "rows left in src_a!", src_a->name);
	  break;
	}
      }
    }
    else {
      if(spec->type == DataJoinSpec::LEFT) { // add blank!
	dest->AddBlankRow();
	for(int i=0;i<ssrc_a.data.size; i++) {
	  //    if(i == spec->col_a.col_idx) continue; // include first guy..
	  DataCol* sda = ssrc_a.data.FastEl(i);
	  DataCol* nda = dest->data.FastEl(i); // todo: change above if uncommented
	  nda->CopyFromRow(-1, *sda, row); // just copy
	}
      }
      else {			// left and inner: just skip b's
	if(spec->nomatch_warn) {
	  taMisc::Warning("taDataProc::Join -- value for src_a:", (String)val_a,
			  "from table:", src_a->name, "not found in column",
			  spec->col_b.col_name, "of src_b:", src_b->name);
	}
      }
    }
  }
  dest->StructUpdate(false);
  spec->ClearColumns();
  return true;
}

bool taDataProc::ConcatCols(DataTable* dest, DataTable* src_a, DataTable* src_b) {
  if(!src_a) { taMisc::Error("taDataProc::ConcatCols: src_a is NULL"); return false; }
  if(!src_b) { taMisc::Error("taDataProc::ConcatCols: src_b is NULL"); return false; }
  bool in_place_req = false;

  // This is not good. The in place op here should PRESERVE ALL DATA!
  // concatenation does not involve deleting anything.
  // GetDest(dest, src_a, "ConcatCols", in_place_req);

  if(dest == src_a) {
    src_a->StructUpdate(true);
    if(src_b->rows > src_a->rows) src_a->EnforceRows(src_b->rows);
    int src_cols = src_b->cols();
    for (int i=0; i < src_cols; i++) {
      DataCol* src_col = src_b->data.FastEl(i);
      DataCol* new_col = src_a->NewCol(src_col->valType(), src_col->name);
      new_col->CopyFrom(src_col);
    }
    src_a->StructUpdate(false);
  }
  else {
    dest->StructUpdate(true);
    dest->Reset();
    for(int i=0; i < src_a->data.size; i++) {
      DataCol* sda = src_a->data.FastEl(i);
      DataCol* nda = (DataCol*)sda->MakeToken();
      dest->data.Add(nda);
      nda->Copy_NoData(*sda);
    }
    int a_cols = src_a->data.size;
    for(int i=0; i < src_b->data.size; i++) {
      DataCol* sdb = src_b->data.FastEl(i);
      DataCol* nda = (DataCol*)sdb->MakeToken();
      dest->data.Add(nda);
      nda->Copy_NoData(*sdb);
    }    
    dest->UniqueColNames();	// make them unique!
    int mx_rows = MAX(src_a->rows, src_b->rows);
    for(int row=0;row<mx_rows;row++) {
      dest->AddBlankRow();
      if(src_a->rows > row) {
	for(int i=0;i<src_a->data.size; i++) {
	  DataCol* sda = src_a->data.FastEl(i);
	  DataCol* nda = dest->data.FastEl(i); // todo: change above if uncommented
	  nda->CopyFromRow(row, *sda, row); // just copy
	}
      }
      if(src_b->rows > row) {
	int col_idx = a_cols;
	for(int i=0; i < src_b->data.size; i++) {
	  DataCol* sdb = src_b->data.FastEl(i);
	  DataCol* nda = dest->data.FastEl(col_idx);
	  nda->CopyFromRow(row, *sdb, row); // just copy
	  col_idx++;
	}
      }
    }
    dest->StructUpdate(false);
  }
  return true;
}

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

void DataProcCall::Initialize() {
  min_type = &TA_taDataProc;
  object_type = &TA_taDataProc;
}

bool DataProcCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains("::")) return false;
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm = mthobj.before("::");
  TypeDef* td = taMisc::types.FindName(objnm);
  if(!td) return false;
  if(objnm == "taDataProc") return true;
  return false;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		Data One

void DataOneProg::Initialize() {
}

void DataOneProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!data_var, quiet, rval, "data_var is NULL")) return; // fatal
  // this should actually be done by the var, not us!
  //  CheckError(!data_var->object_val, quiet, rval, "data_var variable is NULL");
  CheckError(data_var->object_type != &TA_DataTable, quiet, rval,
	     "data_var variable does not point to a DataTable object");
}

DataTable* DataOneProg::GetData() const {
  if(!data_var) return NULL;
  if(data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)data_var->object_val.ptr();
}

//////////////////////////
//      DataLoop	//
//////////////////////////

void DataLoop::Initialize() {
  order = SEQUENTIAL;
}

DataBlock* DataLoop::GetData() {
  if(!data_var) return NULL;
  if(data_var->object_type != &TA_DataBlock) return NULL;
  return (DataBlock*)data_var->object_val.ptr();
}
/*DataTable* DataLoop::GetData() {
  if(!data_var) return NULL;
  if(data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)data_var->object_val.ptr();
}*/

void DataLoop::GetOrderVar() {
  Program* my_prog = program();
  if(!my_prog) return;
  if(!order_var) {
    if(!(order_var = my_prog->vars.FindName("data_loop_order"))) {
      order_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      order_var->name = "data_loop_order";
      order_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  order_var->var_type = ProgVar::T_HardEnum;
  order_var->hard_enum_type = TA_DataLoop.sub_types.FindName("Order");
  order = (Order)order_var->int_val;
}

void DataLoop::GetIndexVar() {
  Program* my_prog = program();
  if(!my_prog) return;
  if(!index_var) {
    if(!(index_var = my_prog->vars.FindName("data_loop_index"))) {
      index_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      index_var->name = "data_loop_index";
      index_var->ClearVarFlag(ProgVar::CTRL_PANEL); // generally not needed there
      index_var->DataChanged(DCR_ITEM_UPDATED);
    }
  }
  index_var->var_type = ProgVar::T_Int;
}

void DataLoop::GetOrderVal() {
  if(!order_var) GetOrderVar();
  else order = (Order)order_var->int_val;
}

void DataLoop::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetOrderVar();
  GetOrderVal();
  GetIndexVar();
}

void DataLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!CheckError(!data_var, quiet, rval,  "data_var = NULL")) {
    if(data_var->object_val) {
      CheckError(!data_var->object_val.ptr()->InheritsFrom(&TA_DataBlock), 
		 quiet, rval,"data_var does not point to a data table (or data block)");
    }
  }
  CheckError(!index_var, quiet, rval, "index_var = NULL");
  CheckError(!order_var, quiet, rval, "order_var = NULL");
}

void DataLoop::GenCssPre_impl(Program* prog) {
  if(!data_var || !index_var) {
    prog->AddLine(this, "// DataLoop ERROR vars not set!", ProgLine::MAIN_LINE);
    return;
  }
  String data_nm = data_var->name;
  String idx_nm = index_var->name;

  prog->AddLine(this, String("{ // DataLoop ") + data_nm, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, String("DataLoop* data_loop = this") + GetPath(NULL,program()) + ";");
  prog->AddLine(this, "data_loop->GetOrderVal(); // order_var variable controls order -- make sure we have current value");
  prog->AddLine(this, String("data_loop->item_idx_list.SetSize(") + data_nm + "->ItemCount());");
  prog->AddLine(this, "data_loop->item_idx_list.FillSeq();");
  prog->AddLine(this, "if(data_loop->order == DataLoop::PERMUTED) data_loop->item_idx_list.Permute();");
  prog->AddLine(this, String("for(") + idx_nm + " = 0; " + idx_nm + " < " + data_nm + "->ItemCount(); "
		+ idx_nm + "++) {");
  prog->IncIndent();
  prog->AddLine(this, "int data_row_idx;");
  prog->AddLine(this, "if(data_loop->order == DataLoop::RANDOM) data_row_idx = Random::IntZeroN(data_loop->item_idx_list.size);");
  prog->AddLine(this, "else");
  prog->AddLine(this, String("  data_row_idx = data_loop->item_idx_list[") + idx_nm + "];");
  prog->AddLine(this, String("if(!") + data_nm + "->ReadItem(data_row_idx)) break; // set read index");
  prog->AddLine(this, String("if(!") + data_nm + "->WriteItem(data_row_idx)) break; // set write index too");
}

void DataLoop::GenCssBody_impl(Program* prog) {
  loop_code.GenCss(prog);
}

void DataLoop::GenCssPost_impl(Program* prog) {
  if(!data_var || !index_var) return;
  prog->DecIndent();
  prog->AddLine(this, "} // for loop");
  prog->DecIndent();
  prog->AddLine(this, "} // DataLoop " + data_var->name);
}

String DataLoop::GetDisplayName() const {
  ((DataLoop*)this)->GetOrderVal();
  String ord_str = GetTypeDef()->GetEnumString("Order", order);
  String data_nm;
  if(data_var) data_nm = data_var->name;
  else data_nm = "ERROR: data_var not set!";
  String index_nm;
  if(index_var) index_nm = index_var->name;
  else index_nm = "ERROR: index_var not set!";
  return "DataTable Loop (" + ord_str + " over: " + data_nm + " index: " + index_nm +")";
}

void DataLoop::SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
				    int dcr, void* op1_, void* op2_) {
  GetOrderVal();
  UpdateAfterEdit();
}

///////////////////////////////////////////////////////
//		DataVarProg
///////////////////////////////////////////////////////


void DataVarProg::Initialize() {
  row_spec = CUR_ROW;
  set_data = false;
}

void DataVarProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(row_spec == CUR_ROW)
    row_var = NULL;		// reset to null
}

void DataVarProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(row_spec != CUR_ROW && !row_var, quiet, rval, "row_var is NULL but is required!");
}

String DataVarProg::GetDisplayName() const {
  String rval;
  String row_var_name = "(ERR: not set!)";
  if((bool)row_var)
    row_var_name = row_var->name;
  if(set_data)
    rval = "To: ";
  else
    rval = "Fm: ";
  if(data_var)
    rval += data_var->name;
  else
    rval += "(ERROR: data_var not set!)";
  if(row_spec  == CUR_ROW)
    rval += " cur_row";
  else if(row_spec == ROW_NUM)
    rval += " row_num: " + row_var_name;
  else
    rval += " row_val: " + row_var_name;
  if(set_data)
    rval += " Fm Vars: ";
  else
    rval += " To Vars: ";
  if(var_1) rval += var_1->name + " ";
  if(var_2) rval += var_2->name + " ";
  if(var_3) rval += var_3->name + " ";
  if(var_4) rval += var_4->name + " ";
  return rval;
}

bool DataVarProg::GenCss_OneVar(Program* prog, ProgVarRef& var, const String& idnm, int var_no) {
  if (!var) return false;
  // if the var is a matrix, then delegate to our Mat handler
  if ((var->var_type == ProgVar::T_Object) &&
    var->object_type->InheritsFrom(&TA_taMatrix))
    return GenCss_OneVarMat(prog, var, idnm, var_no);

  // todo: could do some verbose logging here..
    
  DataCol* da = NULL;
  DataTable* dt = GetData();
  String string_cvt = "";
  if(dt) {
    da = dt->FindColName(var->name);
    if(da && da->isString())
      string_cvt = "(String)";	// cast variant value to a string for setting!
  }
  if(row_spec == CUR_ROW) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetDataByName(" + string_cvt + var->name + ", \"" + var->name +"\");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetDataByName(\"" + var->name + "\");");
  }
  else if(row_spec == ROW_NUM) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetValColName(" + string_cvt + var->name + ", \"" + var->name +"\", "
		    + row_var->name + ", " + String(quiet)  + ");");
    else 
      prog->AddLine(this, var->name + " = " + idnm + ".GetValColName(\"" + var->name + "\", "
		    + row_var->name + ", " + String(quiet) + ");");
  }
  else if(row_spec == ROW_VAL) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetValColRowName(" + string_cvt + var->name + ", \"" + var->name+ "\", \""
		    + row_var->name + "\", " + row_var->name + ", " + String(quiet) + ");");
    else 
      prog->AddLine(this, var->name + " = " + idnm + ".GetValColRowName(\"" + var->name +"\", \""
		    + row_var->name + "\", " + row_var->name + ", " + String(quiet) + ");");
  }
  return true;
}

bool DataVarProg::GenCss_OneVarMat(Program* prog, ProgVarRef& var, const String& idnm, int var_no) {
  DataCol* da = NULL;
  DataTable* dt = GetData();
  String string_cvt = "";
  if(dt) {
    da = dt->FindColName(var->name);
    if(da && da->isString())
      string_cvt = "(String)";	// cast variant value to a string for setting!
  }
  // in all cases, we need a temp var that is ref counted, to hold the mat slice for the col
  prog->AddLine(this, "{taMatrix* __tmp_mat;"); 
  prog->IncIndent();
  // first, get the mat slice
  if (row_spec == CUR_ROW) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetMatrixDataByName(\"" + var->name + "\");");
  }
  else if (row_spec == ROW_NUM) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColName(\"" + var->name + "\", "
		  + row_var->name + ", " + String(quiet) + ");");
  }
  else if (row_spec == ROW_VAL) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColRowName(\"" + var->name +"\", \""
		  + row_var->name + "\", " + row_var->name + ", " + String(quiet) + ");");
  }
  if(set_data) {
    prog->AddLine(this, String("__tmp_mat.CopyFrom(") + var->name + ");");
  }
  else {
    prog->AddLine(this, var->name + ".CopyFrom(__tmp_mat);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  return true;
}

void DataVarProg::GenCssBody_impl(Program* prog) {
  if(!data_var) {
    prog->AddLine(this, "// data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  if(row_spec != CUR_ROW && !row_var) {
    prog->AddLine(this, "// row_var not set but needed!", ProgLine::MAIN_LINE);
    return;
  }
  String idnm = data_var->name;
  prog->AddLine(this, "// " + GetDisplayName(), ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  GenCss_OneVar(prog, var_1, idnm, 0);
  GenCss_OneVar(prog, var_2, idnm, 1);
  GenCss_OneVar(prog, var_3, idnm, 2);
  GenCss_OneVar(prog, var_4, idnm, 3);
}

///////////////////////////////////////////
//		DataVarProgMatrix

void DataVarProgMatrix::Initialize() {
}

bool DataVarProgMatrix::GenCss_OneVar(Program* prog, ProgVarRef& var, const String& idnm, int var_no) {
  if(!var) return false;
  DataCol* da = NULL;
  String col_nm = var->name.before('_', -1);
  if(TestError(col_nm.empty(), "GenCss_OneVar", "variable name must contain a '_' with part before that being name of column in data table to get/set value")) {
    return false;
  }
  DataTable* dt = GetData();
  String string_cvt = "";
  if(dt) {
    da = dt->FindColName(col_nm);
    if(da && da->isString())
      string_cvt = "(String)";	// cast variant value to a string for setting!
  }
  if(row_spec == CUR_ROW) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetMatrixCellDataByName(" + string_cvt + var->name
		    + ", \"" + col_nm +"\", " + String(var_no) + ");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetMatrixCellDataByName(\"" + col_nm
		    + "\", " + String(var_no) + ");");
  }
  else if(row_spec == ROW_NUM) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetMatrixFlatValColName(" + string_cvt + var->name
		    + ", \"" + col_nm +"\", " + row_var->name + ", " + String(var_no) + ");");
    else 
      prog->AddLine(this, var->name + " = " + idnm + ".GetMatrixFlatValColName(\"" + col_nm + "\", "
		    + row_var->name + ", " + String(var_no) + ");");
  }
  else if(row_spec == ROW_VAL) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetMatrixFlatValColRowName(" + string_cvt + var->name
		    + ", \"" + col_nm + "\", \"" + row_var->name + "\", " + row_var->name
		    + ", " + String(var_no) + ");");
    else 
      prog->AddLine(this, var->name + " = " + idnm + ".GetMatrixFlatValColRowName(\"" + col_nm +"\", \""
		    + row_var->name + "\", " + row_var->name + ", " + String(var_no) + ");");
  }
  return true;
}

///////////////////////////////////////////
//		Reset Data Rows

void ResetDataRows::Initialize() {
}

String ResetDataRows::GetDisplayName() const {
  String rval = "ResetDataRows of: ";
  if(data_var) rval += data_var->name;
  else rval += "(ERROR: data_var not set!)";
  return rval;
}

void ResetDataRows::GenCssBody_impl(Program* prog) {
  if(!data_var) {
    prog->AddLine(this, "// data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, data_var->name + ".RemoveAllRows();", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

///////////////////////////////////////////
//		Add New Data Row

void AddNewDataRow::Initialize() {
}

String AddNewDataRow::GetDisplayName() const {
  String rval = "AddNewDataRow to: ";
  if(data_var) rval += data_var->name;
  else rval += "(ERROR: data_var not set!)";
  return rval;
}

void AddNewDataRow::GenCssBody_impl(Program* prog) {
  if(!data_var) {
    prog->AddLine(this, "// data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, data_var->name + ".AddBlankRow();", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

/////////////

void DoneWritingDataRow::Initialize() {
}

String DoneWritingDataRow::GetDisplayName() const {
  String rval = "DoneWritingDataRow to: ";
  if(data_var) rval += data_var->name;
  else rval += "(ERROR: data_var not set!)";
  return rval;
}

void DoneWritingDataRow::GenCssBody_impl(Program* prog) {
  if(!data_var) {
    prog->AddLine(this, "// data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, data_var->name + ".WriteClose();");
  prog->AddVerboseLine(this);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//		Src Dest

void DataSrcDestProg::Initialize() {
}

void DataSrcDestProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!src_data_var, quiet, rval, "src_data_var is NULL")) return; // fatal
  // should be done by var, not us
//   CheckError(!src_data_var->object_val, quiet, rval, "src_data_var variable NULL");
  CheckError(src_data_var->object_type != &TA_DataTable, quiet, rval,
	     "src_data_var variable does not point to a DataTable object");
  // NULL OK in dest_data_var!
}

DataTable* DataSrcDestProg::GetSrcData() {
  if(!src_data_var) return NULL;
  if(src_data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)src_data_var->object_val.ptr();
}

DataTable* DataSrcDestProg::GetDestData() {
  if(!dest_data_var) return NULL;
  if(dest_data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)dest_data_var->object_val.ptr();
}


/////////////////////////////////////////////////////////
//   data sort prog
/////////////////////////////////////////////////////////

void DataSortProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sort_spec.name = "sort_spec";
  UpdateSpecDataTable();
}

void DataSortProg::UpdateSpecDataTable() {
  sort_spec.SetDataTable(GetSrcData());
}

void DataSortProg::Initialize() {
}

String DataSortProg::GetDisplayName() const {
  String rval = "Sort ";
  if(src_data_var) {
    rval += " from: " + src_data_var->name;
  }
  if(dest_data_var) {
    rval += " to: " + dest_data_var->name;
  }
  return rval;
}

void DataSortProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData()) {
    sort_spec.GetColumns(GetSrcData());
    sort_spec.CheckConfig(quiet, rval);
    sort_spec.ClearColumns();
  }
}

void DataSortProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var) {
    prog->AddLine(this, "// DataSort: src_data_var not set!  cannot run", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataSortProg* dsp = this" + GetPath(NULL, program()) + ";", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, String("taDataProc::Sort(") + dest_data_var->name + ", " + src_data_var->name
		  + ", dsp->sort_spec);");
  }
  else {
    prog->AddLine(this, String("taDataProc::Sort(NULL, ") + src_data_var->name + ", dsp->sort_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void DataSortProg::AddAllColumns() {
  sort_spec.AddAllColumns(GetSrcData());
}

/////////////////////////////////////////////////////////
//   data selectRows prog
/////////////////////////////////////////////////////////

void DataSelectRowsProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  select_spec.name = "select_spec";
  UpdateSpecDataTable();
}

void DataSelectRowsProg::UpdateSpecDataTable() {
  select_spec.SetDataTable(GetSrcData());
}

void DataSelectRowsProg::Initialize() {
}

String DataSelectRowsProg::GetDisplayName() const {
  String rval = "SelectRows ";
  if(src_data_var) {
    rval += " from: " + src_data_var->name;
  }
  if(dest_data_var) {
    rval += " to: " + dest_data_var->name;
  }
  return rval;
}

void DataSelectRowsProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData()) {
    select_spec.GetColumns(GetSrcData());
    select_spec.CheckConfig(quiet, rval);
    select_spec.ClearColumns();
  }
}

void DataSelectRowsProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var) {
    prog->AddLine(this, "// DataSelectRows: src_data_var not set!  cannot run!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataSelectRowsProg* dsp = this" + GetPath(NULL, program()) + ";",
		ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, "taDataProc::SelectRows(" + dest_data_var->name + ", " + 
		  src_data_var->name + ", dsp->select_spec);");
  }
  else {
    prog->AddLine(this, "taDataProc::SelectRows(NULL, " + 
		  src_data_var->name + ", dsp->select_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void DataSelectRowsProg::AddAllColumns() {
  select_spec.AddAllColumns(GetSrcData());
}

/////////////////////////////////////////////////////////
//   data selectCols prog
/////////////////////////////////////////////////////////

void DataSelectColsProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  select_spec.name = "select_spec";
  UpdateSpecDataTable();
}

void DataSelectColsProg::UpdateSpecDataTable() {
  select_spec.SetDataTable(GetSrcData());
}

void DataSelectColsProg::Initialize() {
}

String DataSelectColsProg::GetDisplayName() const {
  String rval = "SelectCols ";
  if(src_data_var) {
    rval += " from: " + src_data_var->name;
  }
  if(dest_data_var) {
    rval += " to: " + dest_data_var->name;
  }
  return rval;
}

void DataSelectColsProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData()) {
    select_spec.GetColumns(GetSrcData());
    select_spec.CheckConfig(quiet, rval);
    select_spec.ClearColumns();
  }
}

void DataSelectColsProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var) {
    prog->AddLine(this, "// DataSelectCols: src_data_var not set!  cannot run!",
		  ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataSelectColsProg* dsp = this" + GetPath(NULL, program()) + ";",
		ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, "taDataProc::SelectCols(" + dest_data_var->name + ", " + src_data_var->name
		  + ", dsp->select_spec);");
  }
  else {
    prog->AddLine(this, "taDataProc::SelectCols(NULL, " + src_data_var->name
		  + ", dsp->select_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void DataSelectColsProg::AddAllColumns() {
  select_spec.AddAllColumns(GetSrcData());
}

/////////////////////////////////////////////////////////
//   data group prog
/////////////////////////////////////////////////////////

void DataGroupProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  group_spec.name = "group_spec";
  UpdateSpecDataTable();
}

void DataGroupProg::UpdateSpecDataTable() {
  group_spec.SetDataTable(GetSrcData());
}

void DataGroupProg::Initialize() {
}

String DataGroupProg::GetDisplayName() const {
  String rval = "Group ";
  if(src_data_var) {
    rval += " from: " + src_data_var->name;
  }
  if(dest_data_var) {
    rval += " to: " + dest_data_var->name;
  }
  return rval;
}

void DataGroupProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData()) {
    group_spec.GetColumns(GetSrcData());
    group_spec.CheckConfig(quiet, rval);
    group_spec.ClearColumns();
  }
}

void DataGroupProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var) {
    prog->AddLine(this, "// DataGroup: src_data_var not set!  cannot run!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataGroupProg* dsp = this" + GetPath(NULL, program()) + ";",
		ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, "taDataProc::Group(" + dest_data_var->name + ", " + src_data_var->name
		  + ", dsp->group_spec);");
  }
  else {
    prog->AddLine(this, "taDataProc::Group(NULL, " + src_data_var->name
		  + ", dsp->group_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void DataGroupProg::AddAllColumns() {
  group_spec.AddAllColumns(GetSrcData());
}

/////////////////////////////////////////////////////////
//   data join prog
/////////////////////////////////////////////////////////

void DataJoinProg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  join_spec.name = "join_spec";
  UpdateSpecDataTable();
}

void DataJoinProg::UpdateSpecDataTable() {
  join_spec.SetDataTable(GetSrcData(), GetSrcBData());
}

void DataJoinProg::Initialize() {
}

String DataJoinProg::GetDisplayName() const {
  String rval = join_spec.GetDisplayName();
  if(src_data_var) {
    rval += " a: " + src_data_var->name;
  }
  if(src_b_data_var) {
    rval += " b: " + src_b_data_var->name;
  }
  if(dest_data_var) {
    rval += " to: " + dest_data_var->name;
  }
  return rval;
}

void DataJoinProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!src_b_data_var, quiet, rval, "src_b_data_var is NULL")) return; // fatal
  // should be done by var, not us
//   CheckError(!src_b_data_var->object_val, quiet, rval, "src_data_var variable NULL");
  CheckError(src_b_data_var->object_type != &TA_DataTable, quiet, rval,
	     "src_b_data_var variable does not point to a DataTable object");
}

void DataJoinProg::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  if(GetSrcData() && GetSrcBData()) {
    join_spec.GetColumns(GetSrcData(), GetSrcBData());
    join_spec.CheckConfig(quiet, rval);
    join_spec.ClearColumns();
  }
}

DataTable* DataJoinProg::GetSrcBData() {
  if(!src_b_data_var) return NULL;
  if(src_b_data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)src_b_data_var->object_val.ptr();
}

void DataJoinProg::GenCssBody_impl(Program* prog) {
  if(!src_data_var || !src_b_data_var) {
    prog->AddLine(this, "// DataJoin: src_data_var or src_b_data_var not set!  cannot run",
		  ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ DataJoinProg* dsp = this" + GetPath(NULL, program()) + ";",
		ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, "taDataProc::Join(" + dest_data_var->name + ", " + 
		  src_data_var->name + ", " + src_b_data_var->name + ", dsp->join_spec);");
  }
  else {
    prog->AddLine(this, "taDataProc::Join(NULL, " + 
		  src_data_var->name + ", " + src_b_data_var->name + ", dsp->join_spec);");
  }
  if(dest_data_var) {
    prog->AddLine(this, "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
}

/////////////////////////////////////////////////////////
//   data calc loop
/////////////////////////////////////////////////////////

void DataCalcLoop::Initialize() {
  src_row_var.name = "src_row";
  use_col_numbers = false;
}

void DataCalcLoop::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  src_cols.name = "src_cols";
  dest_cols.name = "dest_cols";
  UpdateSpecDataTable();
  for(int i=0;i<loop_code.size;i++) {
    ProgEl* pe = loop_code[i];
    if(pe->InheritsFrom(&TA_DataCalcAddDestRow) || pe->InheritsFrom(&TA_DataCalcSetDestRow))
      pe->UpdateAfterEdit();	// get the data tables!
  }
}

void DataCalcLoop::UpdateSpecDataTable() {
  src_cols.SetDataTable(GetSrcData());
  dest_cols.SetDataTable(GetDestData());
  UpdateColVars();
}

void DataCalcLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  // null ok in dest_data just as long as you don't call set dest!
}


void DataCalcLoop::SetColProgVarFmData(ProgVar* pv, DataOpEl* ds) {
  pv->SetVarFlag(ProgVar::LOCAL_VAR);
  if(!ds->col_lookup) return;	// nothing to do
  ValType vt = ds->col_lookup->valType();
  if(ds->col_lookup->is_matrix) {
    pv->var_type = ProgVar::T_Object;
    pv->object_val = NULL;
    if(vt == VT_FLOAT)
      pv->object_type = &TA_float_Matrix;
    else if(vt == VT_DOUBLE)
      pv->object_type = &TA_double_Matrix;
    else if(vt == VT_INT)
      pv->object_type = &TA_int_Matrix;
    else if(vt == VT_STRING)
      pv->object_type = &TA_String_Matrix;
    else 
      pv->object_type = &TA_taMatrix;
  }
  else {
    if(vt == VT_FLOAT || vt == VT_DOUBLE) {
      pv->SetReal(0.0f);
    }
    else if(vt == VT_INT) {
      pv->SetInt(0);
    }
    else if(vt == VT_STRING) {
      pv->SetString("");
    }
  }
}

void DataCalcLoop::UpdateColVars() {
  src_cols.GetColumns(GetSrcData());
  String srcp = "s_";
  int ti, i;
  for(i = src_col_vars.size - 1; i >= 0; --i) { // delete not used ones
    ProgVar* pv = src_col_vars.FastEl(i);
    ti = src_cols.FindNameIdx(pv->name.after(srcp));
    if (ti >= 0) {
      SetColProgVarFmData(pv, src_cols.FastEl(ti));
      //      pa->UpdateFromVar(*pv);
    } else {
      src_col_vars.RemoveIdx(i);
    }
  }
  // add args in target not in us, and put in the right order
  for (ti = 0; ti < src_cols.size; ++ti) {
    DataOpEl* ds = src_cols.FastEl(ti);
    i = src_col_vars.FindNameIdx(srcp + ds->col_name);
    if(i < 0) {
      ProgVar* pv = new ProgVar();
      pv->name = srcp + ds->col_name;
      SetColProgVarFmData(pv, ds);
      src_col_vars.Insert(pv, ti);
    } else if (i != ti) {
      src_col_vars.MoveIdx(i, ti);
    }
  }
  src_cols.ClearColumns();

  dest_cols.GetColumns(GetDestData());
  srcp = "d_";
  for(i = dest_col_vars.size - 1; i >= 0; --i) { // delete not used ones
    ProgVar* pv = dest_col_vars.FastEl(i);
    ti = dest_cols.FindNameIdx(pv->name.after(srcp));
    if (ti >= 0) {
      SetColProgVarFmData(pv, dest_cols.FastEl(ti));
    } else {
      dest_col_vars.RemoveIdx(i);
    }
  }
  // add args in target not in us, and put in the right order
  for (ti = 0; ti < dest_cols.size; ++ti) {
    DataOpEl* ds = dest_cols.FastEl(ti);
    i = dest_col_vars.FindNameIdx(srcp + ds->col_name);
    if(i < 0) {
      ProgVar* pv = new ProgVar();
      pv->name = srcp + ds->col_name;
      SetColProgVarFmData(pv, ds);
      dest_col_vars.Insert(pv, ti);
    } else if (i != ti) {
      dest_col_vars.MoveIdx(i, ti);
    }
  }
  dest_cols.ClearColumns();
}


String DataCalcLoop::GetDisplayName() const {
  String rval = "Calc Loop ";
  if(src_data_var) {
    rval += " from: " + src_data_var->name;
  }
  else {
    rval += "ERR! src_data_var is NULL";
  }
  if(dest_data_var) {
    rval += " to: " + dest_data_var->name;
  }
  return rval;
}

void DataCalcLoop::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  UpdateColVars();
  if(GetSrcData()) {
    src_cols.GetColumns(GetSrcData());
    src_cols.CheckConfig(quiet, rval);
    src_cols.ClearColumns();
  }
  if(GetDestData()) {
    dest_cols.GetColumns(GetDestData());
    dest_cols.CheckConfig(quiet, rval);
    dest_cols.ClearColumns();
  }
  loop_code.CheckConfig(quiet, rval);
}

ProgVar* DataCalcLoop::FindVarName(const String& var_nm) const {
  if(var_nm == "src_row")
    return (ProgVar*)&src_row_var;
  ProgVar* pv = src_col_vars.FindName(var_nm);
  if(pv) return pv;
  pv = dest_col_vars.FindName(var_nm);
  if(pv) return pv;
  return loop_code.FindVarName(var_nm);
}

void DataCalcLoop::PreGenChildren_impl(int& item_id) {
  loop_code.PreGen(item_id);
}

void DataCalcLoop::GenCssPre_impl(Program* prog) {
  if(!GetSrcData()) {
    prog->AddLine(this, "// no src data!", ProgLine::MAIN_LINE);
    return;
  }
  src_cols.GetColumns(GetSrcData());
  prog->AddLine(this, "{ DataCalcLoop* dcl = this" + GetPath(NULL, program()) + ";",
		ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, dest_data_var->name + ".ResetData(); // all data ops clear out old existing data");
    prog->AddLine(this, "DataOpList common_dest_cols; // pre-initialize, for CopyCommonCols");
    prog->AddLine(this, "DataOpList common_src_cols;");
    prog->AddLine(this, "DataOpList common_dest_cols_named; // only the cols named in dest_cols");
    prog->AddLine(this, "DataOpList common_src_cols_named;  // only the cols named in src_cols");
    prog->AddLine(this, String("taDataProc::GetCommonCols(") + dest_data_var->name + ", "
		  + src_data_var->name + ", common_dest_cols, common_src_cols);");
    prog->AddLine(this, "common_dest_cols_named = common_dest_cols; common_src_cols_named = common_src_cols;");
    prog->AddLine(this, "taDataProc::GetColIntersection(common_dest_cols_named, dcl->dest_cols);");
    prog->AddLine(this, "taDataProc::GetColIntersection(common_dest_cols_named, dcl->src_cols);");
    prog->AddLine(this, "taDataProc::GetColIntersection(common_src_cols_named, dcl->dest_cols);");
    prog->AddLine(this, "taDataProc::GetColIntersection(common_src_cols_named, dcl->src_cols);");
    prog->AddLine(this, dest_data_var->name + "->StructUpdate(true);");
  }    
  prog->AddLine(this, src_data_var->name + "->StructUpdate(true);");

  prog->AddLine(this, String("for(int src_row=0; src_row < ") + src_data_var->name + ".rows; src_row++) {");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"in for loop\"");

  for(int i=0;i<src_cols.size; i++) {
    DataOpEl* ds = src_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = GetSrcData()->data[ds->col_idx];
    String rval;
    if(da->is_matrix) {
      ValType vt = da->valType();
      String mat_type = "taMatrix";
      if(vt == VT_FLOAT)
	mat_type = "float_Matrix";
      else if(vt == VT_DOUBLE)
	mat_type = "double_Matrix";
      else if(vt == VT_INT)
	mat_type = "int_Matrix";
      else if(vt == VT_STRING)
	mat_type = "String_Matrix";
      rval += mat_type + "* s_" + ds->col_name + " = " + src_data_var->name + ".GetValAsMatrix(";
    }
    else {
      rval += "Variant s_" + ds->col_name + " = " + src_data_var->name + ".GetValAsVar(";
    }
    if(use_col_numbers)
      rval += String(ds->col_idx);
    else
      rval += String("\"") + da->name + String("\"");
    rval += ", src_row);";
    prog->AddLine(this, rval);
  }
  src_cols.ClearColumns();
  // dest cols are only activated by DataAddDestRow
}

void DataCalcLoop::GenCssBody_impl(Program* prog) {
  loop_code.GenCss(prog);
}

void DataCalcLoop::GenCssPost_impl(Program* prog) {
  if(!GetSrcData()) return;
  prog->DecIndent();
  prog->AddLine(this, "} // for loop");
  if(dest_data_var)
    prog->AddLine(this, dest_data_var->name + "->StructUpdate(false);");
  prog->AddLine(this, src_data_var->name + "->StructUpdate(false);");
  prog->DecIndent();
  prog->AddLine(this, "} // DataCalcLoop dcl");
}

const String DataCalcLoop::GenListing_children(int indent_level) {
  return loop_code.GenListing(indent_level + 1);
}

void DataCalcLoop::AddAllSrcColumns() {
  src_cols.AddAllColumns_gui(GetSrcData());
  UpdateColVars();
}
void DataCalcLoop::AddAllDestColumns() {
  dest_cols.AddAllColumns_gui(GetDestData());
  UpdateColVars();
}

DataOpEl* DataCalcLoop::AddSrcColumn(const String& col_name) {
  DataOpEl* rval = src_cols.AddColumn(col_name, GetSrcData());
  UpdateColVars();
  return rval;
}
DataOpEl* DataCalcLoop::AddDestColumn(const String& col_name) {
  DataOpEl* rval = dest_cols.AddColumn(col_name, GetDestData());
  UpdateColVars();
  return rval;
}


/////////////////////////////////////////////////////////
//   data calc add dest row
/////////////////////////////////////////////////////////

void DataCalcAddDestRow::Initialize() {
}

String DataCalcAddDestRow::GetDisplayName() const {
  String rval = "Add Row to: ";
  if(dest_data_var) {
    rval += dest_data_var->name;
  }
  else {
    rval += "ERR! dest_data_var is NULL";
  }
  return rval;
}

void DataCalcAddDestRow::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  src_data_var = dcl->src_data_var;
  dest_data_var = dcl->dest_data_var;
}

void DataCalcAddDestRow::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetDataPtrsFmLoop();
}

void DataCalcAddDestRow::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}  

void DataCalcAddDestRow::Copy_(const DataCalcAddDestRow& cp) {
  GetDataPtrsFmLoop();
}  

void DataCalcAddDestRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(CheckError(!dcl, quiet, rval,"parent DataCalcLoop not found")) return;
  if(CheckError(!dcl->dest_data_var, quiet, rval,
		"DataCalcLoop::dest_data_var is NULL, but is needed")) return;
  // should be done by var, not us
//   CheckError(!dcl->dest_data_var->object_val, quiet, rval, "DataCalcLoop::dest_data_var variable NULL");
  CheckError(dcl->dest_data_var->object_type != &TA_DataTable, quiet, rval,
	     "DataCalcLoop::dest_data_var variable does not point to a DataTable object");
}

void DataCalcAddDestRow::GenCssBody_impl(Program* prog) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    prog->AddLine(this, "// DataCalcAddDestRow Error -- DataCalcLoop not found!!",
		  ProgLine::MAIN_LINE);
    return;
  }
  DataTable* dd = dcl->GetDestData();
  if(!dd) {
    prog->AddLine(this, "// DataCalcAddDestRow Error -- dest_data_var not set!!",
		  ProgLine::MAIN_LINE);
    return;
  }

  dcl->dest_cols.GetColumns(dd);
  prog->AddLine(this, dcl->dest_data_var->name + "->AddBlankRow();", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);

  for(int i=0;i<dcl->dest_cols.size; i++) {
    DataOpEl* ds = dcl->dest_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = dd->data[ds->col_idx];
    String rval;
    if(da->is_matrix) {
      ValType vt = da->valType();
      String mat_type = "taMatrix";
      if(vt == VT_FLOAT)
	mat_type = "float_Matrix";
      else if(vt == VT_DOUBLE)
	mat_type = "double_Matrix";
      else if(vt == VT_INT)
	mat_type = "int_Matrix";
      else if(vt == VT_STRING)
	mat_type = "String_Matrix";
      rval += mat_type + "* d_" + ds->col_name + " = " + dcl->dest_data_var->name + ".GetValAsMatrix(";
    }
    else {
      rval += "Variant d_" + ds->col_name + " = " + dcl->dest_data_var->name + ".GetValAsVar(";
    }
    if(dcl->use_col_numbers)
      rval += String(ds->col_idx);
    else
      rval += String("\"") + da->name + String("\"");
    rval += ", -1); // -1 = last row";
    prog->AddLine(this, rval);
  }
  dcl->dest_cols.ClearColumns();
}

/////////////////////////////////////////////////////////
//   data calc set dest row
/////////////////////////////////////////////////////////

void DataCalcSetDestRow::Initialize() {
}

String DataCalcSetDestRow::GetDisplayName() const {
  String rval = "Set Row in: ";
  if(dest_data_var) {
    rval += dest_data_var->name;
  }
  else {
    rval += "ERR! dest_data_var is NULL";
  }
  return rval;
}

void DataCalcSetDestRow::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  src_data_var = dcl->src_data_var;
  dest_data_var = dcl->dest_data_var;
}

void DataCalcSetDestRow::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetDataPtrsFmLoop();
}

void DataCalcSetDestRow::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}  

void DataCalcSetDestRow::Copy_(const DataCalcSetDestRow& cp) {
  GetDataPtrsFmLoop();
}  

void DataCalcSetDestRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(CheckError(!dcl, quiet, rval,"parent DataCalcLoop not found")) return;
  if(CheckError(!dcl->dest_data_var, quiet, rval,
		"DataCalcLoop::dest_data_var is NULL, but is needed")) return;
  // should be done by var, not us
//   CheckError(!dcl->dest_data_var->object_val, quiet, rval, "DataCalcLoop::dest_data_var variable NULL");
  CheckError(dcl->dest_data_var->object_type != &TA_DataTable, quiet, rval,
	     "DataCalcLoop::dest_data_var variable does not point to a DataTable object");
}

void DataCalcSetDestRow::GenCssBody_impl(Program* prog) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    prog->AddLine(this, "// DataCalcSetDestRow Error -- DataCalcLoop not found!!",
		  ProgLine::MAIN_LINE);
    return;
  }
  DataTable* dd = dcl->GetDestData();
  if(!dd) {
    prog->AddLine(this, "// DataCalcSetDestRow Error -- dest_data_var not set!!",
		  ProgLine::MAIN_LINE);
    return;
  }

  prog->AddLine(this, String("if(") + dcl->dest_data_var->name
	+ ".rows == 0) { taMisc::Error(\"Dest Rows == 0 -- forgot AddDestRow??\"); break; }",
		ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  dcl->dest_cols.GetColumns(dd);
  for(int i=0;i<dcl->dest_cols.size; i++) {
    DataOpEl* ds = dcl->dest_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = dd->data[ds->col_idx];
    String rval;
    if(da->is_matrix)
      rval += dcl->dest_data_var->name + ".SetValAsMatrix(";
    else
      rval += dcl->dest_data_var->name + ".SetValAsVar(";
    rval += "d_" + ds->col_name + ", ";
    if(dcl->use_col_numbers)
      rval += String(ds->col_idx);
    else
      rval += String("\"") + da->name + String("\"");
    rval += ", -1); // -1 = last row";
    prog->AddLine(this, rval);
  }
  prog->AddLine(this, dcl->dest_data_var->name + ".WriteClose();");
  dcl->dest_cols.ClearColumns();
}

/////////////////////////////////////////////////////////
//   data calc set src row
/////////////////////////////////////////////////////////

void DataCalcSetSrcRow::Initialize() {
}

String DataCalcSetSrcRow::GetDisplayName() const {
  String rval = "Set Row in: ";
  if(src_data_var) {
    rval += src_data_var->name;
  }
  else {
    rval += "ERR! src_data_var is NULL";
  }
  return rval;
}

void DataCalcSetSrcRow::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  src_data_var = dcl->src_data_var;
  dest_data_var = dcl->dest_data_var;
}

void DataCalcSetSrcRow::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetDataPtrsFmLoop();
}

void DataCalcSetSrcRow::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}  

void DataCalcSetSrcRow::Copy_(const DataCalcSetSrcRow& cp) {
  GetDataPtrsFmLoop();
}  

void DataCalcSetSrcRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  CheckError(!dcl, quiet, rval, "parent DataCalcLoop not found");
}

void DataCalcSetSrcRow::GenCssBody_impl(Program* prog) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    prog->AddLine(this, "// DataCalcSetSrcRow Error -- DataCalcLoop not found!!",
		  ProgLine::MAIN_LINE);
    return;
  }
  DataTable* sd = dcl->GetSrcData();
  if(!sd) {
    prog->AddLine(this, "// DataCalcSetSrcRow Error -- src_data_var not set!!",
		  ProgLine::MAIN_LINE);
    return;
  }

  dcl->src_cols.GetColumns(sd);
  for(int i=0;i<dcl->src_cols.size; i++) {
    DataOpEl* ds = dcl->src_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = sd->data[ds->col_idx];
    String rval;
    if(da->is_matrix)
      rval += dcl->src_data_var->name + ".SetValAsMatrix(";
    else
      rval += dcl->src_data_var->name + ".SetValAsVar(";
    rval += "s_" + ds->col_name + ", ";
    if(dcl->use_col_numbers)
      rval += String(ds->col_idx);
    else
      rval += String("\"") + da->name + String("\"");
    rval += ", src_row);";
    prog->AddLine(this, rval);
  }
  prog->AddLine(this, dcl->src_data_var->name + ".WriteClose();");
  dcl->dest_cols.ClearColumns();
}

/////////////////////////////////////////////////////////
//   data calc copy common cols
/////////////////////////////////////////////////////////

void DataCalcCopyCommonCols::Initialize() {
  only_named_cols = false;
}

String DataCalcCopyCommonCols::GetDisplayName() const {
  String rval = "Copy Common Cols from: ";
  if(src_data_var)
    rval += src_data_var->name;
  else
    rval += "ERR! src_data_var is NULL";
  rval += " to: ";
  if(dest_data_var)
    rval += dest_data_var->name;
  else
    rval += "ERR! dest_data_var is NULL";
  return rval;
}

void DataCalcCopyCommonCols::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  src_data_var = dcl->src_data_var;
  dest_data_var = dcl->dest_data_var;
}

void DataCalcCopyCommonCols::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetDataPtrsFmLoop();
}

void DataCalcCopyCommonCols::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}  

void DataCalcCopyCommonCols::Copy_(const DataCalcCopyCommonCols& cp) {
  CopyInner_(cp);
  GetDataPtrsFmLoop();
}  

void DataCalcCopyCommonCols::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(CheckError(!dcl, quiet, rval,"parent DataCalcLoop not found")) return;
  if(CheckError(!dcl->dest_data_var, quiet, rval,
		"DataCalcLoop::dest_data_var is NULL, but is needed")) return;
  // should be done by var, not us
//   CheckError(!dcl->dest_data_var->object_val, quiet, rval, "DataCalcLoop::dest_data_var variable NULL");
  CheckError(dcl->dest_data_var->object_type != &TA_DataTable, quiet, rval,
	     "DataCalcLoop::dest_data_var variable does not point to a DataTable object");
}

void DataCalcCopyCommonCols::GenCssBody_impl(Program* prog) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    prog->AddLine(this, "// DataCalcCopyCommonCols Error -- DataCalcLoop not found!!",
		  ProgLine::MAIN_LINE);
    return;
  }
  if(!dcl->dest_data_var) {
    prog->AddLine(this, "// DataCalcCopyCommonCols Error -- dest_data_var null in DataCalcLoop!!", ProgLine::MAIN_LINE);
    return;
  }

  prog->AddLine(this, String("if(") + dcl->dest_data_var->name
		+ ".rows == 0) { taMisc::Error(\"Dest Rows == 0 -- forgot AddDestRow??\"); break; }", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  if(only_named_cols) 
    prog->AddLine(this, "taDataProc::CopyCommonColsRow_impl(" + dcl->dest_data_var->name + ", " + 
	  dcl->src_data_var->name + ", common_dest_cols_named, common_src_cols_named, -1, src_row);");
  else
    prog->AddLine(this, "taDataProc::CopyCommonColsRow_impl(" + dcl->dest_data_var->name + ", " + 
		  dcl->src_data_var->name + ", common_dest_cols, common_src_cols, -1, src_row);");
}

