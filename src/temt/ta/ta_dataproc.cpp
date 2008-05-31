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
  data_cols = NULL;
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
  CheckError(col_name.empty(), quiet, rval,"col_name is empty");
  //  CheckError(col_idx < 0, quiet, rval,"could not find", col_name,"in datatable");
  // note: an error can be too strong and prevent transitional code from running -- sometimes
  // at compile time the names aren't right, but they later end up being ok..
  TestWarning(col_idx < 0, "CheckConfig", "could not find", col_name,"in datatable");
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
  DataCol* da = dt->FindColName(col_name, col_idx);
  if(col_idx < 0) da = NULL;	// just to be sure..
  taBase::SetPointer((taBase**)&col_lookup, da);
}

void DataOpEl::ClearColumns() {
  taBase::SetPointer((taBase**)&col_lookup, NULL);
}

///////////////////////////

void DataOpList::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  DataSrcDestProg* own_prog = GET_MY_OWNER(DataSrcDestProg);
  if(own_prog)
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
  rel = EQUAL;
  use_var = false;
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
  if(col_lookup) {
    CheckError(col_lookup->is_matrix, quiet, rval, "cannot use matrix column to select");
  }
  CheckError(use_var && !var, quiet, rval, "use_var is selected but no var variable is set!");
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

bool taDataProc::GetDest(DataTable*& dest, const DataTable* src, const String& suffix) {
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
    int d_idx;
    DataCol* dda = dest->FindColName(sop->col_name, d_idx);
    if(!dda) {
      src_cols->RemoveIdx(i); i--;
      continue;
    }
    DataCol* sda = src->data[sop->col_idx];
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

bool taDataProc::CopyData(DataTable* dest, DataTable* src) {
  if(!src) { taMisc::Error("taDataProc::CopyData: src is NULL"); return false; }
  GetDest(dest, src, "CopyData");
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
  DataOpList dest_cols;
  DataOpList src_cols;
  GetCommonCols(dest, src, &dest_cols, &src_cols);
  return CopyCommonColsRow_impl(dest, src, &dest_cols, &src_cols, dest_row, src_row);
}

bool taDataProc::CopyCommonColData(DataTable* dest, DataTable* src) {
  if(!dest) { taMisc::Error("taDataProc::CopyCommonColData: dest is NULL"); return false; }
  if(!src) { taMisc::Error("taDataProc::CopyCommonColData: src is NULL"); return false; }
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
  GetDest(dest, src, "ReplicateRows");
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
  return true;
}

bool taDataProc::ConcatRows(DataTable* dest, DataTable* src_a, DataTable* src_b, DataTable* src_c,
			    DataTable* src_d, DataTable* src_e, DataTable* src_f) {
  if(!src_a) { taMisc::Error("taDataProc::ConcatRows: src_a is NULL"); return false; }
  if(!src_b) { taMisc::Error("taDataProc::ConcatRows: src_b is NULL"); return false; }
  GetDest(dest, src_a, "ConcatRows");
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


///////////////////////////////////////////////////////////////////
// reordering functions

bool taDataProc::Sort(DataTable* dest, DataTable* src, DataSortSpec* spec) {
  if(!src) { taMisc::Error("taDataProc::Sort: src is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataProc::Sort: spec is NULL"); return false; }
  // just copy and operate on dest
  GetDest(dest, src, "Sort");
  dest->Reset();
  String dnm = dest->name;
  *dest = *src;
  dest->name = dnm;
  return Sort_impl(dest, spec);
}

bool taDataProc::SortInPlace(DataTable* dt, DataSortSpec* spec) {
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
  if(!src) { taMisc::Error("taDataProc::Permute: src is NULL"); return false; }
  GetDest(dest, src, "Permute");
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
  return true;
}

bool taDataProc::Group(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
  if(!src) { taMisc::Error("taDataProc::Group: src is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataProc::Group: spec is NULL"); return false; }
  GetDest(dest, src, "Group");
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
  DataSortSpec sort_spec;
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
  return true;
}

bool taDataProc::Group_nogp(DataTable* dest, DataTable* src, DataGroupSpec* spec) {
  float_Matrix float_tmp;
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
  DataTable ssrc;
  taBase::Own(ssrc, NULL);	// activates initlinks, refs

  DataSortSpec full_sort_spec;
  taBase::Own(full_sort_spec, NULL);
  full_sort_spec = *sort_spec;

  // add [FIND_] LAST and FIRST to end of sort spec
  for(int i=0;i<spec->ops.size; i++) {
    DataGroupEl* ds = (DataGroupEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue;
    if((ds->agg.op != Aggregate::FIRST) && (ds->agg.op != Aggregate::LAST) &&
       (ds->agg.op != Aggregate::FIND_FIRST) && (ds->agg.op != Aggregate::FIND_LAST)) continue;
    DataSortEl* ss = (DataSortEl*)full_sort_spec.ops.New(1, &TA_DataSortEl);
    ss->col_name = ds->col_name;
    ss->col_idx = ds->col_idx;
    taBase::SetPointer((taBase**)&ss->col_lookup, ds->col_lookup);
    ss->order = DataSortEl::ASCENDING;
  }

  taDataProc::Sort(&ssrc, src, &full_sort_spec);

  sort_spec->GetColumns(&ssrc);	// re-get columns -- they were nuked by Sort op!

  Variant_Array cur_vals;
  cur_vals.SetSize(sort_spec->ops.size);

  // initialize cur vals
  for(int i=0;i<sort_spec->ops.size; i++) {
    DataSortEl* ds = (DataSortEl*)sort_spec->ops.FastEl(i);
    DataCol* sda = ssrc.data.FastEl(ds->col_idx);
    Variant cval = sda->GetValAsVar(0); // start at row 0
    cur_vals[i] = cval;
//     cerr << i << " init val: " << cur_vals[i] << " input: " << cval << endl;
  }

  float_Matrix float_tmp;
  int st_row = 0;
  int row = 1;
  while(row <= ssrc.rows) {
    for(;row <= ssrc.rows; row++) {
//       cerr << "row: " << row;
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
// 	    cerr << " new_val:  oval: " << cur_vals[i] << " nval: " << cval;
	    new_val = true;
	    cur_vals[i] = cval;
	  }
	}
// 	cerr << endl;
      }
      if(new_val) break;
    }
    int n_rows = row - st_row;
//     cerr << "adding: row: " << row << " st_row: " << st_row << endl;
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
      DataCol* da = src->data.FastEl(ds->col_idx);
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
  if(!src) { taMisc::Error("taDataProc::SplitRows: src is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataProc::SplitRows: spec is NULL"); return false; }
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
      DataCol* da = src->data.FastEl(ds->col_idx);
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
  if(!src) { taMisc::Error("taDataProc::SplitRowsN: src is NULL"); return false; }
  int nary[6] = {n1, n2, n3, n4, n5, n6};
  DataTable* dary[6] = {dest_1, dest_2, dest_3, dest_4, dest_5, dest_6};
  int n_split = 0;
  int rest_idx = -1;
  int n_tot = 0;
  for(int i=0;i<6;i++) {
    if(!dary[i]) break;
    GetDest(dary[i], src, "SplitByN_" + String(i));
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
    GetDest(dary[i], src, "SplitByN_" + String(i));
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
  GetDest(dest, src, "SelectCols");
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
    for(int i=0;i<spec->size; i++) {
      DataOpEl* ds = spec->FastEl(i);
      if(ds->col_idx < 0) continue;
      DataCol* sda = src->data.FastEl(ds->col_idx);
      DataCol* nda = dest->data.FastEl(i);
      nda->CopyFromRow(row, *sda, row);
    }
  }
  dest->StructUpdate(false);
  spec->ClearColumns();
  return true;
}

bool taDataProc::Join(DataTable* dest, DataTable* src_a, DataTable* src_b,
		      DataJoinSpec* spec) {
  if(!src_a) { taMisc::Error("taDataProc::Join: src_a is NULL"); return false; }
  if(!src_b) { taMisc::Error("taDataProc::Join: src_b is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataProc::Join: spec is NULL"); return false; }
  if((spec->col_a.col_idx < 0) || (spec->col_b.col_idx < 0)) return false;
  GetDest(dest, src_a, "Join");
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

  DataSortSpec sort_spec_a;
  taBase::Own(sort_spec_a, NULL);
  {
    DataSortEl* ss = (DataSortEl*)sort_spec_a.ops.New(1, &TA_DataSortEl);
    ss->col_name = spec->col_a.col_name;
    ss->col_idx = spec->col_a.col_idx;
    taBase::SetPointer((taBase**)&ss->col_lookup, spec->col_a.col_lookup);
    ss->order = DataSortEl::ASCENDING;
  }
  
  DataSortSpec sort_spec_b;
  taBase::Own(sort_spec_b, NULL);
  {
    DataSortEl* ss = (DataSortEl*)sort_spec_b.ops.New(1, &TA_DataSortEl);
    ss->col_name = spec->col_b.col_name;
    ss->col_idx = spec->col_b.col_idx;
    taBase::SetPointer((taBase**)&ss->col_lookup, spec->col_b.col_lookup);
    ss->order = DataSortEl::ASCENDING;
  }

  DataTable ssrc_a;
  taBase::Own(ssrc_a, NULL);	// activates initlinks, refs
  taDataProc::Sort(&ssrc_a, src_a, &sort_spec_a);

  DataTable ssrc_b;
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
  GetDest(dest, src_a, "ConcatCols");
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
  int mx_rows = MIN(src_a->rows, src_b->rows);
  for(int row=0;row<mx_rows;row++) {
    dest->AddBlankRow();
    for(int i=0;i<src_a->data.size; i++) {
      DataCol* sda = src_a->data.FastEl(i);
      DataCol* nda = dest->data.FastEl(i); // todo: change above if uncommented
      nda->CopyFromRow(row, *sda, row); // just copy
    }
    int col_idx = a_cols;
    for(int i=0; i < src_b->data.size; i++) {
      DataCol* sdb = src_b->data.FastEl(i);
      DataCol* nda = dest->data.FastEl(col_idx);
      nda->CopyFromRow(row, *sdb, row); // just copy
      col_idx++;
    }    
  }
  dest->StructUpdate(false);
  return true;
}

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

void DataProcCall::Initialize() {
  min_type = &TA_taDataProc;
  object_type = &TA_taDataProc;
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

const String DataLoop::GenCssPre_impl(int indent_level) {
  String id1 = cssMisc::Indent(indent_level+1);
  String id2 = cssMisc::Indent(indent_level+2);
  String id3 = cssMisc::Indent(indent_level+3);
  if(!data_var || !index_var) return id1 + "// DataLoop ERROR vars not set!\n";
  String data_nm = data_var->name;
  String idx_nm = index_var->name;

  String rval = cssMisc::Indent(indent_level) + "{ // DataLoop " + data_nm + "\n";
  rval += id1 + "DataLoop* data_loop = this" + GetPath(NULL,program()) + ";\n";
  rval += id1 + "data_loop->GetOrderVal(); // order_var variable controls order -- make sure we have current value\n";
  rval += id1 + "data_loop->item_idx_list.SetSize(" + data_nm + "->ItemCount());\n";
  rval += id1 + "data_loop->item_idx_list.FillSeq();\n";
  rval += id1 + "if(data_loop->order == DataLoop::PERMUTED) data_loop->item_idx_list.Permute();\n";
  rval += id1 + "for(" + idx_nm + " = 0; " + idx_nm + " < " + data_nm + "->ItemCount(); "
    + idx_nm + "++) {\n";
  rval += id2 + "int data_row_idx;\n";
  rval += id2 + "if(data_loop->order == DataLoop::RANDOM) data_row_idx = Random::IntZeroN(data_loop->item_idx_list.size);\n";
  rval += id2 + "else\n";
  rval += id3 + "data_row_idx = data_loop->item_idx_list[" + idx_nm + "];\n";
  rval += id2 + "if(!" + data_nm + "->ReadItem(data_row_idx)) break; // set read index\n";
  rval += id2 + "if(!" + data_nm + "->WriteItem(data_row_idx)) break; // set write index too\n";
  return rval;
}

const String DataLoop::GenCssBody_impl(int indent_level) {
  return loop_code.GenCss(indent_level + 2);
}

const String DataLoop::GenCssPost_impl(int indent_level) {
  if(!data_var || !index_var) return cssMisc::Indent(indent_level+1) + "// DataLoop ERROR vars not set!\n";
  String rval = cssMisc::Indent(indent_level+1) + "} // for loop\n";
  rval += cssMisc::Indent(indent_level) + "} // DataLoop " + data_var->name + "\n";
  return rval;
}

String DataLoop::GetDisplayName() const {
  String ord_str = GetTypeDef()->GetEnumString("Order", order);
  String data_nm;
  if(data_var) data_nm = data_var->name;
  else data_nm = "ERROR: data_var not set!";
  return "DataTable Loop (" + ord_str + " over: " + data_nm + ")";
}


///////////////////////////////////////////////////////
//		DataVarProg
///////////////////////////////////////////////////////


void DataVarProg::Initialize() {
  row_spec = CUR_ROW;
  set_data = false;
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

bool DataVarProg::GenCss_OneVar(String& rval, ProgVarRef& var, const String& idnm, 
				const String& il, int var_no) {
  if(!var) return false;
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
      rval +=  il + idnm + ".SetDataByName(" + string_cvt + var->name + ", \"" + var->name +"\");\n";
    else
      rval += il + var->name + " = " + idnm + ".GetDataByName(\"" + var->name + "\");\n";
  }
  else if(row_spec == ROW_NUM) {
    if(set_data)
      rval +=  il + idnm + ".SetValColName(" + string_cvt + var->name + ", \"" + var->name +"\", "
	+ row_var->name + ");\n";
    else 
      rval += il + var->name + " = " + idnm + ".GetValColName(\"" + var->name + "\", "
	+ row_var->name + ");\n";
  }
  else if(row_spec == ROW_VAL) {
    if(set_data)
      rval +=  il + idnm + ".SetValColRowName(" + string_cvt + var->name + ", \"" + var->name+ "\", \""
	+ row_var->name + "\", " + row_var->name + ");\n";
    else 
      rval += il + var->name + " = " + idnm + ".GetValColRowName(\"" + var->name +"\", \""
	+ row_var->name + "\", " + row_var->name + ");\n";
  }
  return true;
}

const String DataVarProg::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  if(!data_var) return il + "// data_var not set!\n";
  if(row_spec != CUR_ROW && !row_var) return il + "// row_var not set but needed!\n";
  String idnm = data_var->name;
  String rval;
  rval += il + "// " + GetDisplayName() + "\n";
  GenCss_OneVar(rval, var_1, idnm, il, 0);
  GenCss_OneVar(rval, var_2, idnm, il, 1);
  GenCss_OneVar(rval, var_3, idnm, il, 2);
  GenCss_OneVar(rval, var_4, idnm, il, 3);
  return rval;
}

///////////////////////////////////////////
//		DataVarProgMatrix

void DataVarProgMatrix::Initialize() {
}

bool DataVarProgMatrix::GenCss_OneVar(String& rval, ProgVarRef& var, const String& idnm, 
				      const String& il, int var_no) {
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
      rval +=  il + idnm + ".SetMatrixCellDataByName(" + string_cvt + var->name
	+ ", \"" + col_nm +"\", " + String(var_no) + ");\n";
    else
      rval += il + var->name + " = " + idnm + ".GetMatrixCellDataByName(\"" + col_nm
	+ "\", " + String(var_no) + ");\n";
  }
  else if(row_spec == ROW_NUM) {
    if(set_data)
      rval +=  il + idnm + ".SetMatrixFlatValColName(" + string_cvt + var->name
	+ ", \"" + col_nm +"\", " + row_var->name + ", " + String(var_no) + ");\n";
    else 
      rval += il + var->name + " = " + idnm + ".GetMatrixFlatValColName(\"" + col_nm + "\", "
	+ row_var->name + ", " + String(var_no) + ");\n";
  }
  else if(row_spec == ROW_VAL) {
    if(set_data)
      rval +=  il + idnm + ".SetMatrixFlatValColRowName(" + string_cvt + var->name
	+ ", \"" + col_nm + "\", \"" + row_var->name + "\", " + row_var->name
	+ ", " + String(var_no) + ");\n";
    else 
      rval += il + var->name + " = " + idnm + ".GetMatrixFlatValColRowName(\"" + col_nm +"\", \""
	+ row_var->name + "\", " + row_var->name
	+ ", " + String(var_no) + ");\n";
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

const String ResetDataRows::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  if(!data_var) return il + "// data_var not set!\n";
  return il + data_var->name + ".RemoveAllRows();\n";
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

const String AddNewDataRow::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  if(!data_var) return il + "// data_var not set!\n";
  return il + data_var->name + ".AddBlankRow();\n";
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

const String DoneWritingDataRow::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  if(!data_var) return il + "// data_var not set!\n";
  return il + data_var->name + ".WriteClose();\n";
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

const String DataSortProg::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  String il1 = cssMisc::Indent(indent_level+1);
  if(!src_data_var)
    return il + "// DataSort: src_data_var not set!  cannot run\n";
  String rval = il + "{ DataSortProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  if(dest_data_var) {
    rval += il1 + "taDataProc::Sort(" + dest_data_var->name + ", " + src_data_var->name
      + ", dsp->sort_spec);\n";
  }
  else {
    rval += il1 + "taDataProc::Sort(NULL, " + src_data_var->name
      + ", dsp->sort_spec);\n";
  }
  if(dest_data_var) {
    rval += il1 + "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL\n";
  }
  rval += il + "}\n";
  return rval; 
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

const String DataSelectRowsProg::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  String il1 = cssMisc::Indent(indent_level+1);
  if(!src_data_var)
    return il + "// DataSelectRows: src_data_var not set!  cannot run!\n";
  String rval = il + "{ DataSelectRowsProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  for(int i=0;i<select_spec.ops.size; i++) {
    DataSelectEl* el = (DataSelectEl*)select_spec.ops[i];
    if(el->use_var && (bool)el->var) {
      rval += il1 + "dsp->select_spec.ops[" + String(i) + "].cmp = " + el->var->name + ";\n";
    }
  }
  if(dest_data_var) {
    rval += il1 + "taDataProc::SelectRows(" + dest_data_var->name + ", " + 
      src_data_var->name + ", dsp->select_spec);\n";
  }
  else {
    rval += il1 + "taDataProc::SelectRows(NULL, " + 
      src_data_var->name + ", dsp->select_spec);\n";
  }
  if(dest_data_var) {
    rval += il1 + "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL\n";
  }
  rval += il + "}\n";
  return rval; 
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

const String DataSelectColsProg::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  String il1 = cssMisc::Indent(indent_level+1);
  if(!src_data_var)
    return il + "// DataSelectCols: src_data_var not set!  cannot run!\n";
  String rval = il + "{ DataSelectColsProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  if(dest_data_var) {
    rval += il1 + "taDataProc::SelectCols(" + dest_data_var->name + ", " + src_data_var->name
      + ", dsp->select_spec);\n";
  }
  else {
    rval += il1 + "taDataProc::SelectCols(NULL, " + src_data_var->name
      + ", dsp->select_spec);\n";
  }
  if(dest_data_var) {
    rval += il1 + "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL\n";
  }
  rval += il + "}\n";
  return rval; 
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

const String DataGroupProg::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  String il1 = cssMisc::Indent(indent_level+1);
  if(!src_data_var)
    return il + "// DataGroup: src_data_var not set!  cannot run!\n";
  String rval = il + "{ DataGroupProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  if(dest_data_var) {
    rval += il1 + "taDataProc::Group(" + dest_data_var->name + ", " + src_data_var->name
      + ", dsp->group_spec);\n";
  }
  else {
    rval += il1 + "taDataProc::Group(NULL, " + src_data_var->name
      + ", dsp->group_spec);\n";
  }
  if(dest_data_var) {
    rval += il1 + "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL\n";
  }
  rval += il + "}\n";
  return rval; 
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

const String DataJoinProg::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  String il1 = cssMisc::Indent(indent_level+1);
  if(!src_data_var || !src_b_data_var)
    return il + "// DataJoin: src_data_var or src_b_data_var not set!  cannot run\n";
  String rval = il + "{ DataJoinProg* dsp = this" + GetPath(NULL, program()) + ";\n";
  if(dest_data_var) {
    rval += il1 + "taDataProc::Join(" + dest_data_var->name + ", " + 
      src_data_var->name + ", " + src_b_data_var->name + ", dsp->join_spec);\n";
  }
  else {
    rval += il1 + "taDataProc::Join(NULL, " + 
      src_data_var->name + ", " + src_b_data_var->name + ", dsp->join_spec);\n";
  }
  if(dest_data_var) {
    rval += il1 + "if(!dsp->GetDestData()) dsp->dest_data_var.SetObject(.data.gp.AnalysisData.Peek()); // get new one if NULL\n";
  }
  rval += il + "}\n";
  return rval; 
}

/////////////////////////////////////////////////////////
//   data calc loop
/////////////////////////////////////////////////////////

void DataCalcLoop::Initialize() {
  src_row_var.name = "src_row";
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
  if(!ds->col_lookup) return;	// nothing to do
  ValType vt = ds->col_lookup->valType();
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

void DataCalcLoop::UpdateColVars() {
  src_cols.GetColumns(GetSrcData());
  String srcp = "s_";
  int ti, i;
  for(i = src_col_vars.size - 1; i >= 0; --i) { // delete not used ones
    ProgVar* pv = src_col_vars.FastEl(i);
    src_cols.FindName(pv->name.after(srcp), ti);
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
    src_col_vars.FindName(srcp + ds->col_name, i);
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
    dest_cols.FindName(pv->name.after(srcp), ti);
    if (ti >= 0) {
      SetColProgVarFmData(pv, dest_cols.FastEl(ti));
    } else {
      dest_col_vars.RemoveIdx(i);
    }
  }
  // add args in target not in us, and put in the right order
  for (ti = 0; ti < dest_cols.size; ++ti) {
    DataOpEl* ds = dest_cols.FastEl(ti);
    dest_col_vars.FindName(srcp + ds->col_name, i);
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

const String DataCalcLoop::GenCssPre_impl(int indent_level) {
  if(!GetSrcData()) return cssMisc::Indent(indent_level) + "// no src data!\n";
  src_cols.GetColumns(GetSrcData());
  String il1 = cssMisc::Indent(indent_level+1);
  String il2 = cssMisc::Indent(indent_level+2);
  String rval = cssMisc::Indent(indent_level) + "{ DataCalcLoop* dcl = this" + GetPath(NULL, program()) + ";\n";
  if(dest_data_var) {
    rval += il1 + dest_data_var->name + ".ResetData(); // all data ops clear out old existing data\n";
    rval += il1 + "DataOpList common_dest_cols; // pre-initialize, for CopyCommonCols\n";
    rval += il1 + "DataOpList common_src_cols;\n";
    rval += il1 + "DataOpList common_dest_cols_named; // only the cols named in dest_cols\n";
    rval += il1 + "DataOpList common_src_cols_named;  // only the cols named in src_cols\n";
    rval += il1 + "taDataProc::GetCommonCols(" + dest_data_var->name + ", " + src_data_var->name
      + ", common_dest_cols, common_src_cols);\n";
    rval += il1 + "common_dest_cols_named = common_dest_cols; common_src_cols_named = common_src_cols;\n";
    rval += il1 + "taDataProc::GetColIntersection(common_dest_cols_named, dcl->dest_cols);\n";
    rval += il1 + "taDataProc::GetColIntersection(common_dest_cols_named, dcl->src_cols);\n";
    rval += il1 + "taDataProc::GetColIntersection(common_src_cols_named, dcl->dest_cols);\n";
    rval += il1 + "taDataProc::GetColIntersection(common_src_cols_named, dcl->src_cols);\n";
    rval += il1 + dest_data_var->name + "->StructUpdate(true);\n";
  }    
  rval += il1 + src_data_var->name + "->StructUpdate(true);\n";

  rval += il1 + "for(int src_row=0; src_row < " + src_data_var->name + ".rows; src_row++) {\n";
  for(int i=0;i<src_cols.size; i++) {
    DataOpEl* ds = src_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = GetSrcData()->data[ds->col_idx];
    if(da->is_matrix)
      rval += il2 + "taMatrix* s_" + ds->col_name + " = " + src_data_var->name + ".GetValAsMatrix(" +
	String(ds->col_idx) + ", src_row);\n";
    else
      rval += il2 + "Variant s_" + ds->col_name + " = " + src_data_var->name + ".GetValAsVar(" +
	String(ds->col_idx) + ", src_row);\n";
  }
  src_cols.ClearColumns();
  // dest cols are only activated by DataAddDestRow
  return rval;
}

const String DataCalcLoop::GenCssBody_impl(int indent_level) {
  return loop_code.GenCss(indent_level + 2);
}

const String DataCalcLoop::GenListing_children(int indent_level) {
  return loop_code.GenListing(indent_level + 1);
}

const String DataCalcLoop::GenCssPost_impl(int indent_level) {
  if(!GetSrcData()) return "";
  String il1 = cssMisc::Indent(indent_level+1);
  String rval = il1 + "} // for loop\n";
  if(dest_data_var)
    rval += il1 + dest_data_var->name + "->StructUpdate(false);\n";
  rval += il1 + src_data_var->name + "->StructUpdate(false);\n";
  rval += cssMisc::Indent(indent_level) + "} // DataCalcLoop dcl\n";
  return rval;
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

const String DataCalcAddDestRow::GenCssBody_impl(int indent_level) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return "// DataCalcAddDestRow Error -- DataCalcLoop not found!!\n";
  DataTable* dd = dcl->GetDestData();
  if(!dd) return "// DataCalcAddDestRow Error -- dest_data_var not set!!\n";

  String il = cssMisc::Indent(indent_level);

  dcl->dest_cols.GetColumns(dd);
  String rval = il + dcl->dest_data_var->name + "->AddBlankRow();\n";

  for(int i=0;i<dcl->dest_cols.size; i++) {
    DataOpEl* ds = dcl->dest_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = dd->data[ds->col_idx];
    if(da->is_matrix)
      rval += il + "taMatrix* d_" + ds->col_name + " = " + dcl->dest_data_var->name + ".GetValAsMatrix(" +
	String(ds->col_idx) + ", -1); // -1 = last row\n";
    else
      rval += il + "Variant d_" + ds->col_name + " = " + dcl->dest_data_var->name + ".GetValAsVar(" +
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

const String DataCalcSetDestRow::GenCssBody_impl(int indent_level) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return "// DataCalcSetDestRow Error -- DataCalcLoop not found!!\n";
  DataTable* dd = dcl->GetDestData();
  if(!dd) return "// DataCalcSetDestRow Error -- dest_data_var not set!!\n";

  String il = cssMisc::Indent(indent_level);

  String rval;
  rval += il + "if(" + dcl->dest_data_var->name + ".rows == 0) { taMisc::Error(\"Dest Rows == 0 -- forgot AddDestRow??\"); break; }\n";
  dcl->dest_cols.GetColumns(dd);
  for(int i=0;i<dcl->dest_cols.size; i++) {
    DataOpEl* ds = dcl->dest_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = dd->data[ds->col_idx];
    if(da->is_matrix)
      rval += il + dcl->dest_data_var->name + ".SetValAsMatrix(" + 
	"d_" + ds->col_name + ", " + String(ds->col_idx) + ", -1); // -1 = last row\n";
    else
      rval += il + dcl->dest_data_var->name + ".SetValAsVar(" +
	"d_" + ds->col_name + ", " + String(ds->col_idx) + ", -1); // -1 = last row\n";
  }
  rval += il + dcl->dest_data_var->name + ".WriteClose();\n";
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

const String DataCalcSetSrcRow::GenCssBody_impl(int indent_level) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return "// DataCalcSetSrcRow Error -- DataCalcLoop not found!!\n";
  DataTable* sd = dcl->GetSrcData();
  if(!sd) return "// DataCalcSetSrcRow Error -- src_data_var not set!!\n";

  String il = cssMisc::Indent(indent_level);

  String rval;
  dcl->src_cols.GetColumns(sd);
  for(int i=0;i<dcl->src_cols.size; i++) {
    DataOpEl* ds = dcl->src_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = sd->data[ds->col_idx];
    if(da->is_matrix)
      rval += il + dcl->src_data_var->name + ".SetValAsMatrix(" + 
	"s_" + ds->col_name + ", " + String(ds->col_idx) + ", src_row);\n";
    else
      rval += il + dcl->src_data_var->name + ".SetValAsVar(" +
	"s_" + ds->col_name + ", " + String(ds->col_idx) + ", src_row);\n";
  }
  rval += il + dcl->src_data_var->name + ".WriteClose();\n";
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

const String DataCalcCopyCommonCols::GenCssBody_impl(int indent_level) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) return "// DataCalcCopyCommonCols Error -- DataCalcLoop not found!!\n";
  if(!dcl->dest_data_var) return "// DataCalcCopyCommonCols Error -- dest_data_var null in DataCalcLoop!!\n";

  String il = cssMisc::Indent(indent_level);

  String rval;
  rval += il + "if(" + dcl->dest_data_var->name + ".rows == 0) { taMisc::Error(\"Dest Rows == 0 -- forgot AddDestRow??\"); break; }\n";
  if(only_named_cols) 
    rval += il + "taDataProc::CopyCommonColsRow_impl(" + dcl->dest_data_var->name + ", " + 
      dcl->src_data_var->name + ", common_dest_cols_named, common_src_cols_named, -1, src_row);\n";
  else
    rval += il + "taDataProc::CopyCommonColsRow_impl(" + dcl->dest_data_var->name + ", " + 
      dcl->src_data_var->name + ", common_dest_cols, common_src_cols, -1, src_row);\n";
  return rval;
}

