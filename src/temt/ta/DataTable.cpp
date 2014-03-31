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

#include "DataTable.h"
#include <iDataTableModel>
#include <Program>
#include <taFiler>
#include <ChannelSpec>
#include <CellRange>
#include <MatrixIndex>
#include <FixedWidthSpec>
#include <DataSortSpec>
#include <DataSortEl>
#include <taDataProc>
#include <DataGroupSpec>
#include <DataGroupEl>
#include <DataTable_Group>
#include <int_Array>
#include <float_Array>
#include <double_Array>
#include <String_Array>
#include <Variant_Array>
#include <DataSelectSpec>
#include <DataSelectEl>
#include <AnalysisRun>
#include <taProject>
#include <iDialogChoice>

taTypeDef_Of(float_Data);
taTypeDef_Of(double_Data);
taTypeDef_Of(int_Data);
taTypeDef_Of(byte_Data);
taTypeDef_Of(String_Data);
taTypeDef_Of(Variant_Data);

#include <taMisc>
#include <tabMisc>

#include <QFileInfo>

#ifdef DMEM_COMPILE
#include <char_Array>
#include <DMemShare>
#include <float_Matrix>
#include <double_Matrix>
#include <int_Matrix>
#include <byte_Matrix>
#endif

#include <css_machine.h>
#include <css_ta.h>

#include <libjson>
#include <JSONNode>
#include "../json/JSONDefs.h"

TA_BASEFUNS_CTORS_DEFN(DataTable);

using namespace std;

void DataTable::Initialize() {
  rows = 0;
  rows_total = 0;
  data_flags = (DataFlags)(SAVE_ROWS | AUTO_CALC);
  auto_load = NO_AUTO_LOAD;
  keygen.setType(Variant::T_Int64);
  calc_script = NULL;
  log_file = NULL;
  table_model = NULL;
  row_indexes.SetGeom(1,0);  // always should be set to 1d
  base_diff_row = -1;  // no base comparison row at start
  change_col = NULL;
  change_col_type = -1;
}

void DataTable::Destroy() {
  CutLinks();
  if (table_model) {
    delete table_model;
    table_model = NULL;
  }
}

void DataTable::InitLinks() {
  data.name = "data"; // for the viewspec routines
  inherited::InitLinks();
  taBase::Own(data, this);
  taBase::Own(row_indexes, this);
  taBase::Own(diff_row_list, this);
  taBase::Own(change_col_geom, this);
  log_file = taFiler::New("DataTable", ".dat");
  taRefN::Ref(log_file);
}

void DataTable::CutLinks() {
  data.CutLinks();
  row_indexes.CutLinks();
  diff_row_list.CutLinks();
  if(log_file) {
    taRefN::unRefDone(log_file);
    log_file = NULL;
  }
  if(calc_script) {
    delete calc_script;
    calc_script = NULL;
  }
  inherited::CutLinks();
}

void DataTable::Copy_(const DataTable& cp) {
  ResetData();                  // full reset because matrix copy cannot enforce geom for view'ed copy of view'ed source
  data = cp.data;               // matrix level copy will effectively flatten source
  ComputeRowsTotal();           // update to what we actually got
  ResetRowIndexes();            // so we reset our indexes to match the flattened source
  ClearCompareRows();
  data_flags = cp.data_flags;
  auto_load = cp.auto_load;
  auto_load_file = cp.auto_load_file;
  keygen = cp.keygen;
}

void DataTable::Copy_NoData(const DataTable& cp) {
  // note: CANNOT just set rows=0, because we must reclaim mat space (ex strings)
  // and must insure data model is sync'ed property
  //  ResetData();
  RemoveAllCols();
  // don't copy the flags!
  //   data_flags = cp.data_flags;
  data.Copy_NoData(cp.data);
}

void DataTable::Copy_DataOnly(const DataTable& cp) {
  StructUpdate(true); // ala Copy_impl
  SetBaseFlag(COPYING); // ala Copy__
  ResetRowIndexes();            // key to reset our indexes before copying data
  data = cp.data;               // matrix level copy will effectively flatten source
  ComputeRowsTotal();           // update to what we actually got
  ResetRowIndexes();            // so we reset our indexes to match the flattened source
  ClearBaseFlag(COPYING);
  ClearCompareRows();
  StructUpdate(false);
}

void DataTable::CopyFromRow(int dest_row, const DataTable& src, int src_row) {
  data.CopyFromRow(dest_row, src.data, src_row);
}


bool DataTable::CopyCell(const Variant& dest_col, int dest_row, const DataTable& src,
    const Variant& src_col, int src_row)
{
  DataCol* dar = GetColData(dest_col);
  DataCol* sar = src.GetColData(src_col);
  return CopyCell_impl(dar, dest_row, src, sar, src_row);
}

bool DataTable::CopyCellName(const String& dest_col_name, int dest_row,
    const DataTable& src, const String& src_col_name, int src_row)
{
  DataCol* dar = FindColName(dest_col_name);
  DataCol* sar = src.FindColName(src_col_name);
  return CopyCell_impl(dar, dest_row, src, sar, src_row);
}

bool DataTable::CopyCell_impl(DataCol* dar, int dest_row, const DataTable& src,
    DataCol* sar, int src_row)
{
  if(TestError(!dar || !sar, "CopyCell", "column(s) out of range, not copied!"))
    return false;
  dar->CopyFromRow_Robust(dest_row, *sar, src_row);
  return true;
}

void DataTable::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  UniqueColNames();
  // the following is likely redundant:
  //  UpdateColCalcs();
  CheckForCalcs();
}

int DataTable::GetSpecialState() const {
  if(!HasDataFlag(SAVE_ROWS)) return 1;
  if(HasDataFlag(HAS_CALCS)) return 2;
  return 0;
}

void DataTable::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  data.CheckConfig(quiet, rval);
}

void DataTable::ComputeRowsTotal() {
  rows_total = 0;
  for (int i = 0; i < cols(); ++i) {
    DataCol* col = data.FastEl(i);
    col->SetMatrixViewMode();   // just for good measure
    int frms = col->AR()->FramesRaw();
    rows_total = max((int)rows_total, frms); // these should all be the same!!
  }
}

void DataTable::ResetRowIndexes() {
  rows = rows_total;
  row_indexes.SetGeom(1,rows);
  row_indexes.FillSeq();        // 0...n-1
}

bool DataTable::AddRows(int n) {
  if(TestError((cols() == 0), "AddRows", "no columns!")) return false;
  if(TestError((n < 1), "AddRows", "n rows < 1")) return false;
  RowsAdding(n, true);
  for(int i=0;i<data.size;i++) {
    DataCol* dc = data.FastEl(i);
    dc->EnforceRows(rows_total + n); // key to use rows_total here, not rows() which is filtered
  }
  for(int r=rows_total; r<rows_total+n; r++) {
    row_indexes.Add(r);
  }
  RowsAdding(n, false);
  return true;
}

bool DataTable::AddSinkChannel(ChannelSpec* cs) {
  if (TestError(!cs, "AddSinkChannel", "channel spec is null")) return false;
  DataCol* da = NewColFromChannelSpec_impl(cs);
  return (da);
}

void DataTable::AllocRows(int n) {
  for(int i=0;i<data.size;i++) {
    DataCol* ar = data.FastEl(i);
    ar->AR()->AllocFrames(n); //noop if already has more alloc'ed
  }
}

int DataTable::AddBlankRow() {
  if(AddRows(1)) {
    ScrollEditorsToBottom();
    rd_itr = wr_itr = rows - 1;
    return wr_itr;
  }
  else return -1;
}

void DataTable::EnforceRows(int nr) {
  if(rows > nr) {
    RemoveRows(nr, rows - nr);
  }
  else if(rows < nr) {
    AddRows(nr - rows);
  }
}

bool DataTable::AssertSinkChannel(ChannelSpec* cs) {
  if (TestError(!cs, "AssertSinkChannel", "channel spec is null")) return false;
  DataCol* da = GetColForChannelSpec_impl(cs);
  return (da);
}

bool DataTable::ColMatchesChannelSpec(const DataCol* da, const ChannelSpec* cs) {
  if(TestError(!da || !cs, "ColMatchesChannelSpec", "col or spec are null")) return false;
  //NOTE: make sure the algorithm in this routine matches NewColFromChannelSpec_impl
  // match matrix-ness
  if (da->is_matrix != cs->isMatrix()) return false;
  if (da->valType() != cs->val_type) return false;

  if (cs->isMatrix()) {
    // geoms the same is only remaining criteria
    return da->cell_geom.Equal(cs->cellGeom());
  }
  return true;
}

bool DataTable::AutoLoadData() {
  if(HasDataFlag(SAVE_ROWS) || taMisc::is_undo_loading) return false;
  if(auto_load == NO_AUTO_LOAD) return false;

  if(taMisc::gui_active && (auto_load == PROMPT_LOAD)) {
    int chs = taMisc::Choice("Load data file: " + auto_load_file + " into data table: " + name, "Yes", "No");
    if(chs == 1) return false;
  }

  if(TestError(auto_load_file.empty(), "AutoLoadData", "auto_load_file is empty!"))
    return false;
  QFileInfo qfi(auto_load_file);
  if(TestError(!(qfi.isFile() && qfi.isReadable()), "AutoLoadData",
      "auto_load_file is not a valid file that is readable:", auto_load_file))
    return false;

  taMisc::Info("DataTable:", name, "auto loading data from:", auto_load_file);

  if(auto_load_file.contains(".dtbl")) {
    String cur_nm = name;
    DataFlags cur_data_flags = data_flags;
    String cur_alf = auto_load_file;
    Load(auto_load_file);
    name = cur_nm;
    data_flags = cur_data_flags;
    auto_load_file = cur_alf;
  }
  else {
    LoadAnyData(auto_load_file, true, LD_AUTO, LQ_AUTO, -1, true);
  }
  return true;
}

bool DataTable::AutoSaveData() {
  if(HasDataFlag(SAVE_ROWS) || !HasDataFlag(SAVE_FILE)) return false;

  if(TestError(auto_load_file.empty(), "AutoSaveData", "auto_load_file is empty!"))
    return false;
  // can't save to .dtbl type because that would recurse us -- TODO: could workaround somehow
  if (TestError(!auto_load_file.contains(".dat"), "AutoSaveData", "auto_load_file can only be of .dat type, sorry!")) {
    return false;
  } else {
    SaveData(auto_load_file); // TODO: should really give error if fails...
  }
  return true;
}

void DataTable::Dump_Load_post() {
  inherited::Dump_Load_post();
  if(taMisc::is_undo_loading) return; // none of this.
  // this is an attempt to fix BugID:66, but it leads to problems when loading the project
  //  StructUpdate(true);
  AutoLoadData();
  //  StructUpdate(false);
}

int DataTable::Dump_Load_Value(istream& strm, taBase* par) {
  int c = inherited::Dump_Load_Value(strm, par);
  if (c == EOF) return EOF;
  if (c == 2) return 2; // signal that it was just a path
  // otherwise, if data was loaded, we need to set the rows
  ComputeRowsTotal();
  if(row_indexes.size > 0) {    // existing row indexes
    if(rows_total == 0)         // no data saved / loaded
      row_indexes.Reset();     // nuke the indexes to match
    rows = row_indexes.size; // this is the number of rows visible (i.e. not filtered out)
  }
  else { // should always have something visible..
    ResetRowIndexes();
  }
  this->SigEmitUpdated();
  return c;
}


void DataTable::Dump_Save_pre() {
  inherited::Dump_Save_pre();
  AutoSaveData();
}

taBase* DataTable::ChildDuplicate(const taBase* child) {
  taBase* newChild;
  taBase* oldChild = const_cast<taBase*>(child);
  dynamic_cast<DataCol*>(oldChild)->UnSetMatrixViewMode();  // we want to copy all rows hidden and visible
  newChild = inherited::ChildDuplicate(child);
  dynamic_cast<DataCol*>(oldChild)->SetMatrixViewMode();
  return newChild;
}

DataCol* DataTable::GetColForChannelSpec_impl(ChannelSpec* cs) {
  for(int i=data.size-1;i>=0;i--) {
    DataCol* da = data.FastEl(i);
    if (da->name != cs->name) continue;
    // if name matches, but not contents, we need to remake it...
    if (ColMatchesChannelSpec(da, cs)) {
      da->ClearColFlag(DataCol::MARK); // reset mark for orphan tracking
      cs->chan_num = i;
      return da;
    }
    else {
      da->Close();
    }
  }
  DataCol* rval = NewColFromChannelSpec_impl(cs);
  return rval;
}

const Variant DataTable::GetColUserData(const String& name, const Variant& col) const {
  DataCol* da = GetColData(col);
  if (da) return da->GetUserData(name);
  else return _nilVariant;
}

void DataTable::SetColUserData(const String& name, const Variant& value, const Variant& col) {
  DataCol* da = GetColData(col);
  if (da) da->SetUserData(name, value);
}


taMatrix* DataTable::GetMatrixData_impl(int chan) {
  DataCol* da = GetColData(chan);
  if(!da) return NULL;          // err msg already given
  int i;
  if(TestError(!da->is_matrix, "GetMatrixData_impl", "column is not a matrix")) return NULL;
  if(TestError(!idx(rd_itr, i), "GetMatrixData_impl",
      "read index is out of range -- need to set with ReadItem or ReadFirst/Next")) return NULL;
  return da->AR()->GetFrameSlice_(i);
}

///////////////////////////////////////////
//      Basic Access/Modify with Variant col spec

const Variant DataTable::GetVal(const Variant& col, int row) const {
  if(col.isStringType())
    return GetValColName(col.toString(), row);
  return GetValAsVar(col.toInt(), row);
}

bool DataTable::SetVal(const Variant& val, const Variant& col, int row) {
  if(col.isStringType())
    return SetValColName(val, col.toString(), row);
  return SetValAsVar(val, col.toInt(), row);
}


const Variant DataTable::GetMatrixVal(const Variant& col, int row,
    int d0, int d1, int d2, int d3) const {
  if(col.isStringType())
    return GetMatrixValColName(col.toString(), row, d0, d1, d2, d3);
  return GetValAsVarMDims(col.toInt(), row, d0, d1, d2, d3);
}

bool DataTable::SetMatrixVal(const Variant& val, const Variant& col, int row,
    int d0, int d1, int d2, int d3)  {
  if(col.isStringType())
    return SetMatrixValColName(val, col.toString(), row, d0, d1, d2, d3);
  return SetValAsVarMDims(val, col.toInt(), row, d0, d1, d2, d3);
}

const Variant DataTable::GetMatrixFlatVal(const Variant& col, int row, int cell) const {
  if(col.isStringType())
    return GetMatrixFlatValColName(col.toString(), row, cell);
  return GetValAsVarM(col.toInt(), row, cell);
}

bool DataTable::SetMatrixFlatVal(const Variant& val, const Variant& col, int row, int cell) {
  if(col.isStringType())
    return SetMatrixFlatValColName(val, col.toString(), row, cell);
  return SetValAsVarM(val, col.toInt(), row, cell);
}

///////////////////////////////////////////
//      Column Name Access

const Variant DataTable::GetValColName(const String& col_nm, int row, bool quiet) const {
  DataCol* da = FindColName(col_nm, true);
  int i;
  if (da &&  idx_err(row, i, quiet))
    return da->GetValAsVar(i);
  else return _nilVariant;
}

bool DataTable::SetValColName(const Variant& val, const String& col_nm, int row, bool quiet) {
  DataCol* da = FindColName(col_nm, true);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, i, quiet)) {
    da->SetValAsVar(val, i);
    return true;
  } else return false;
}

const Variant DataTable::GetMatrixValColName(const String& col_nm, int row,
    int d0, int d1, int d2, int d3, bool quiet) const {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, i, quiet))
    return da->GetValAsVarMDims(i, d0, d1, d2, d3);
  else return _nilVariant;
}

bool DataTable::SetMatrixValColName(const Variant& val, const String& col_nm,
    int row, int d0, int d1, int d2, int d3, bool quiet) {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, i, quiet)) {
    da->SetValAsVarMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

const Variant DataTable::GetMatrixFlatValColName(const String& col_nm, int row,
    int cell, bool quiet) const {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, i, quiet))
    return da->GetValAsVarM(i, cell);
  else return _nilVariant;
}

bool DataTable::SetMatrixFlatValColName(const Variant& val, const String& col_nm,
    int row, int cell, bool quiet) {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, i, quiet)) {
    da->SetValAsVarM(val, i, cell);
    return true;
  } else return false;
}

/////////////////////

const Variant DataTable::GetValColRowName(const String& col_nm, const String& row_col_name,
    const Variant& row_value, bool quiet) const {
  DataCol* cda = FindColName(col_nm, true);
  if(!cda) return _nilVariant;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return _nilVariant;
  int row = rda->FindVal(row_value);
  if(quiet) {
    if(row < 0) return _nilVariant;
  }
  else {
    if(TestError(row < 0, "GetValColRowName", "value:", row_value.toString(),
        "of column named:", row_col_name, "not found"))
      return _nilVariant;
  }
  return cda->GetValAsVar(row);
}

bool DataTable::SetValColRowName(const Variant& val, const String& col_nm,
    const String& row_col_name, const Variant& row_value,
    bool quiet) {
  DataCol* cda = FindColName(col_nm, true);
  if(!cda || cda->is_matrix_err()) return false;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return false;
  int row = rda->FindVal(row_value);
  if(quiet) {
    if(row < 0) return false;
  }
  else {
    if(TestError(row < 0, "SetValColRowName", "value:", row_value.toString(),
        "of column named:", row_col_name, "not found"))
      return false;
  }
  cda->SetValAsVar(val, row);
  return true;
}

const Variant DataTable::GetMatrixValColRowName(const String& col_nm,
    const String& row_col_name, const Variant& row_value,
    int d0, int d1, int d2, int d3, bool quiet) const {
  DataCol* cda = FindColName(col_nm, true);
  if(!cda || cda->not_matrix_err()) return _nilVariant;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return _nilVariant;
  int row = rda->FindVal(row_value);
  if(quiet) {
    if(row < 0) return _nilVariant;
  }
  else {
    if(TestError(row < 0, "GetMatrixValColRowName", "value:", row_value.toString(),
        "of column named:", row_col_name, "not found"))
      return _nilVariant;
  }
  return cda->GetValAsVarMDims(row, d0, d1, d2, d3);
}

bool DataTable::SetMatrixValColRowName(const Variant& val, const String& col_nm,
    const String& row_col_name, const Variant& row_value,
    int d0, int d1, int d2, int d3, bool quiet) {
  DataCol* cda = FindColName(col_nm, true);
  if (!cda || cda->not_matrix_err()) return false;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return false;
  int row = rda->FindVal(row_value);
  if(quiet) {
    if(row < 0) return false;
  }
  else {
    if(TestError(row < 0, "SetMatrixValColRowName", "value:", row_value.toString(),
        "of column named:", row_col_name, "not found"))
      return false;
  }
  cda->SetValAsVarMDims(val, row, d0, d1, d2, d3);
  return true;
}

const Variant DataTable::GetMatrixFlatValColRowName(const String& col_nm,
    const String& row_col_name, const Variant& row_value,
    int cell, bool quiet) const {
  DataCol* cda = FindColName(col_nm, true);
  if(!cda || cda->not_matrix_err()) return _nilVariant;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return _nilVariant;
  int row = rda->FindVal(row_value);
  if(quiet) {
    if(row < 0) return _nilVariant;
  }
  else {
    if(TestError(row < 0, "GetMatrixFlatValColRowName", "value:", row_value.toString(),
        "of column named:", row_col_name, "not found"))
      return _nilVariant;
  }
  return cda->GetValAsVarM(row, cell);
}

bool DataTable::SetMatrixFlatValColRowName(const Variant& val, const String& col_nm,
    const String& row_col_name, const Variant& row_value,
    int cell, bool quiet) {
  DataCol* cda = FindColName(col_nm, true);
  if (!cda || cda->not_matrix_err()) return false;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return false;
  int row = rda->FindVal(row_value);
  if(quiet) {
    if(row < 0) return false;
  }
  else {
    if(TestError(row < 0, "SetMatrixFlatValColRowName", "value:", row_value.toString(),
        "of column named:", row_col_name, "not found"))
      return false;
  }
  cda->SetValAsVarMDims(val, row, cell);
  return true;
}

/////////////////////

bool DataTable::InitVals(const Variant& init_val, const Variant& col) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  da->InitVals(init_val);
  return true;
}

bool DataTable::InitValsToRowNo(const Variant& col) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  da->InitValsToRowNo();
  return true;
}

bool DataTable::InitValsColName(const Variant& init_val, const String& col_nm) {
  DataCol* da = FindColName(col_nm, true);
  if (!da) return false;
  da->InitVals(init_val);
  return true;
}

bool DataTable::InitValsToRowNoColName(const String& col_nm) {
  DataCol* da = FindColName(col_nm, true);
  if (!da) return false;
  da->InitValsToRowNo();
  return true;
}

/////////////////////

int DataTable::FindVal(const Variant& val, const Variant& col, int st_row,
    bool not_found_err) const {
  DataCol* da = GetColData(col);
  if (!da) return false;
  int rval = da->FindVal(val, st_row);
  if(rval < 0) {
    TestError(not_found_err, "FindVal", "val:", val.toString(),
        "not found in col:", col.toString());
  }
  return rval;
}

Variant DataTable::LookupVal(const Variant& find_val, const Variant& find_in_col, 
    const Variant& value_col, int st_row, bool not_found_err) const {
  int row_num = FindVal(find_val, find_in_col, st_row, false); // we'll do the err msg
  if(row_num < 0) {
    TestError(not_found_err, "LookupVal", "find_val:", find_val.toString(),
        "not found in find_in_col:", find_in_col.toString());
    return _nilVariant;
  }

  DataCol* da = GetColData(value_col);
  if (!da) return _nilVariant;
  return da->GetVal(row_num);
}

int DataTable::FindValColName(const Variant& val, const String& col_nm, int st_row) const {
  DataCol* da = FindColName(col_nm, true);
  if (!da) return false;
  return da->FindVal(val, st_row);
}

int DataTable::FindMultiVal(int st_row, const Variant& val1, const Variant& col1,
    const Variant& val2, const Variant& col2,
    const Variant& val3, const Variant& col3,
    const Variant& val4, const Variant& col4,
    const Variant& val5, const Variant& col5,
    const Variant& val6, const Variant& col6) const {
  DataCol* cold1=GetColData(col1,true); // quiet
  DataCol* cold2=GetColData(col2,true);
  DataCol* cold3=GetColData(col3,true);
  DataCol* cold4=GetColData(col4,true);
  DataCol* cold5=GetColData(col5,true);
  DataCol* cold6=GetColData(col6,true);

  return FindMultiValCol(st_row, val1, cold1, val2, cold2, val3, cold3, val4, cold4,
      val5, cold5, val6, cold6);
}

int DataTable::FindMultiValColName(int st_row, const Variant& val1, const String& col_nm1,
    const Variant& val2, const String& col_nm2,
    const Variant& val3, const String& col_nm3,
    const Variant& val4, const String& col_nm4,
    const Variant& val5, const String& col_nm5,
    const Variant& val6, const String& col_nm6) const {
  DataCol* col1=NULL;
  DataCol* col2=NULL;
  DataCol* col3=NULL;
  DataCol* col4=NULL;
  DataCol* col5=NULL;
  DataCol* col6=NULL;

  if(col_nm1.nonempty()) col1 = FindColName(col_nm1, true);
  if(col_nm2.nonempty()) col2 = FindColName(col_nm2, true);
  if(col_nm3.nonempty()) col3 = FindColName(col_nm3, true);
  if(col_nm4.nonempty()) col4 = FindColName(col_nm4, true);
  if(col_nm5.nonempty()) col5 = FindColName(col_nm5, true);
  if(col_nm6.nonempty()) col6 = FindColName(col_nm6, true);

  return FindMultiValCol(st_row, val1, col1, val2, col2, val3, col3, val4, col4,
      val5, col5, val6, col6);
}

int DataTable::FindMultiValCol(int st_row, const Variant& val1, DataCol* col1,
    const Variant& val2, DataCol* col2,
    const Variant& val3, DataCol* col3,
    const Variant& val4, DataCol* col4,
    const Variant& val5, DataCol* col5,
    const Variant& val6, DataCol* col6) const {
  if(st_row >= 0) {
    for(int i=st_row; i<rows; i++) {
      if(col1 && col1->GetVal(i) != val1) continue;
      if(col2 && col2->GetVal(i) != val2) continue;
      if(col3 && col3->GetVal(i) != val3) continue;
      if(col4 && col4->GetVal(i) != val4) continue;
      if(col5 && col5->GetVal(i) != val5) continue;
      if(col6 && col6->GetVal(i) != val6) continue;
      return i;
    }
    return -1;
  }
  else {
    for(int i=rows+st_row; i>=0; i--) {
      if(col1 && col1->GetVal(i) != val1) continue;
      if(col2 && col2->GetVal(i) != val2) continue;
      if(col3 && col3->GetVal(i) != val3) continue;
      if(col4 && col4->GetVal(i) != val4) continue;
      if(col5 && col5->GetVal(i) != val5) continue;
      if(col6 && col6->GetVal(i) != val6) continue;
      return i;
    }
  }
  return -1;
}

iDataTableModel* DataTable::GetTableModel() {
  if (!table_model && !isDestroying()) {
    table_model = new iDataTableModel(this);
    AddSigClient(table_model);
    //table_model->setPat4D(true); // always
  }
  return table_model;
}

/////////////////////

double DataTable::GetValAsDouble(const Variant& col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da && idx_err(row, i))
    return da->GetValAsDouble(i);
  else return 0.0f;
}

float DataTable::GetValAsFloat(const Variant& col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsFloat(i);
  else return 0.0f;
}

int DataTable::GetValAsInt(const Variant& col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsInt(i);
  else return 0;
}

const String DataTable::GetValAsString(const Variant& col, int row) const {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsString(i);
  else return "n/a";
}

const Variant DataTable::GetValAsVar(const Variant& col, int row) const {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsVar(i);
  else return _nilVariant;
}

taMatrix* DataTable::GetValAsMatrix(const Variant& col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsMatrix(i);
  else return NULL;
}

taMatrix* DataTable::GetValAsMatrixColName(const String& col_nm, int row, bool quiet) {
  DataCol* da = FindColName(col_nm, true);
  int i;
  if (da &&  idx_err(row, i, quiet))
    return da->GetValAsMatrix(i);
  else return NULL;
}

taMatrix* DataTable::GetValAsMatrixColRowName(const String& col_nm, const String& row_col_name,
    const Variant& row_value, bool quiet)
{
  DataCol* cda = FindColName(col_nm, true);
  if(!cda) return NULL;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return NULL;
  int row = rda->FindVal(row_value);
  if(quiet) {
    if(row < 0) return NULL;
  }
  else {
    if(TestError(row < 0, "GetValAsMatrixColRowName", "value:", row_value.toString(),
        "of column named:", row_col_name, "not found"))
      return NULL;
  }
  return cda->GetValAsMatrix(row);
}


taMatrix* DataTable::GetRangeAsMatrix(const Variant& col, int st_row, int n_rows) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(st_row, i))
    return da->GetRangeAsMatrix(i, n_rows);
  else return NULL;
}


///////////////////////////////////////
// sub-matrix reading and writing functions

void DataTable::WriteFmSubMatrix(const Variant& col, int row,
    const taMatrix* src, taMatrix::RenderOp render_op,
    int off0, int off1, int off2,
    int off3, int off4, int off5, int off6) {
  DataCol* da = GetColData(col);
  int i;
  if (da && idx_err(row, i))
    da->WriteFmSubMatrix(row, src, render_op, off0, off1, off2, off3, off4, off5, off6);
}

void DataTable::ReadToSubMatrix(const Variant& col, int row,
    taMatrix* dest, taMatrix::RenderOp render_op,
    int off0, int off1, int off2,
    int off3, int off4, int off5, int off6) {
  DataCol* da = GetColData(col);
  int i;
  if (da && idx_err(row, i))
    da->ReadToSubMatrix(row, dest, render_op, off0, off1, off2, off3, off4, off5, off6);
}

void DataTable::WriteFmSubMatrixTable(const Variant& col, int row,
    const DataTable* src, const Variant& src_col, int src_row,
    taMatrix::RenderOp render_op,
    int off0, int off1, int off2,
    int off3, int off4, int off5, int off6) {
  if(TestError(!src, "WriteFmSubMatrixTable", "submat_src table is NULL"))
    return;
  DataCol* da = GetColData(col);
  int i;
  if(!da || !idx_err(row, i)) return;
  DataCol* sda = src->GetColData(src_col);
  int j;
  if(!sda || !idx_err(src_row, j)) return;
  taMatrix* mat = sda->GetValAsMatrix(src_row);
  if(!mat) return;
  taBase::Ref(mat);
  da->WriteFmSubMatrix(row, mat, render_op, off0, off1, off2, off3, off4, off5, off6);
  taBase::unRefDone(mat);
}

void DataTable::ReadToSubMatrixTable(const Variant& col, int row,
    const DataTable* dest, const Variant& dest_col, int dest_row,
    taMatrix::RenderOp render_op,
    int off0, int off1, int off2,
    int off3, int off4, int off5, int off6) {
  if(TestError(!dest, "ReadToSubMatrixTable", "submat_dest table is NULL"))
    return;
  DataCol* da = GetColData(col);
  int i;
  if(!da || !idx_err(row, i)) return;
  DataCol* sda = dest->GetColData(dest_col);
  int j;
  if(!sda || !idx_err(dest_row, j)) return;
  taMatrix* mat = sda->GetValAsMatrix(dest_row);
  if(!mat) return;
  taBase::Ref(mat);
  da->ReadToSubMatrix(row, mat, render_op, off0, off1, off2, off3, off4, off5, off6);
  taBase::unRefDone(mat);
}

// lookup versions:

void DataTable::WriteFmSubMatrixTableLookup(const Variant& col, int row,
    const DataTable* submat_src, const Variant& submat_src_col,
    Variant submat_lookup_val, const Variant& submat_lookup_col,
    taMatrix::RenderOp render_op, const DataTable* offset_lookup,
    Variant offset_col, const Variant& offset_lookup_val, const Variant& offset_lookup_col) {
  if(TestError(!submat_src, "WriteFmSubMatrixTableLookup", "submat_src table is NULL"))
    return;
  if(TestError(!offset_lookup, "WriteFmSubMatrixTableLookup", "offset_lookup table is NULL"))
    return;
  DataCol* da = GetColData(col);
  int i;
  if(!da || !idx_err(row, i)) return;
  DataCol* sda = submat_src->GetColData(submat_src_col);
  DataCol* slda = submat_src->GetColData(submat_lookup_col);
  if(!slda) return;
  int slrow = slda->FindVal(submat_lookup_val);
  if(TestError(slrow < 0, "WriteFmSubMatrixTableLookup", "cannot find submat_lookup_val",
      submat_lookup_val.toString(),
      "in submat_src table column:",slda->name)) return;
  DataCol* olda = offset_lookup->GetColData(offset_lookup_col);
  if(!olda) return;
  DataCol* oda = offset_lookup->GetColData(offset_col);
  if(!oda) return;
  int olrow = olda->FindVal(offset_lookup_val);
  if(TestError(olrow < 0, "WriteFmSubMatrixTableLookup", "cannot find offset_lookup_val",
      offset_lookup_val.toString(),
      "in offset_lookup table column:",olda->name)) return;
  int offs[7] = {0,0,0,0,0,0,0};
  int mx = MIN(oda->cell_size(), 7);
  for(int k=0;k<mx;k++) {
    offs[k] = oda->GetValAsIntM(olrow, k);
  }
  taMatrix* mat = sda->GetValAsMatrix(slrow);
  if(!mat) return;
  taBase::Ref(mat);
  da->WriteFmSubMatrix(row, mat, render_op, offs[0], offs[1], offs[2], offs[3], offs[4],
      offs[5], offs[6]);
  taBase::unRefDone(mat);
}

void DataTable::ReadToSubMatrixTableLookup(const Variant& col, int row,
    const DataTable* submat_dest, const Variant& submat_dest_col,
    Variant submat_lookup_val, const Variant& submat_lookup_col,
    taMatrix::RenderOp render_op, const DataTable* offset_lookup,
    Variant offset_col, const Variant& offset_lookup_val, const Variant& offset_lookup_col) {
  if(TestError(!submat_dest, "ReadToSubMatrixTableLookup", "submat_dest table is NULL"))
    return;
  if(TestError(!offset_lookup, "ReadToSubMatrixTableLookup", "offset_lookup table is NULL"))
    return;
  DataCol* da = GetColData(col);
  int i;
  if(!da || !idx_err(row, i)) return;
  DataCol* sda = submat_dest->GetColData(submat_dest_col);
  DataCol* slda = submat_dest->GetColData(submat_lookup_col);
  if(!slda) return;
  int slrow = slda->FindVal(submat_lookup_val);
  if(TestError(slrow < 0, "ReadToSubMatrixTableLookup", "cannot find submat_lookup_val",
      submat_lookup_val.toString(),
      "in submat_src table column:",slda->name)) return;
  DataCol* olda = offset_lookup->GetColData(offset_lookup_col);
  if(!olda) return;
  DataCol* oda = offset_lookup->GetColData(offset_col);
  if(!oda) return;
  int olrow = olda->FindVal(offset_lookup_val);
  if(TestError(olrow < 0, "WriteFmSubMatrixTableLookup", "cannot find offset_lookup_val",
      offset_lookup_val.toString(),
      "in offset_lookup table column:",olda->name)) return;
  int offs[7] = {0,0,0,0,0,0,0};
  int mx = MIN(oda->cell_size(), 7);
  for(int k=0;k<mx;k++) {
    offs[k] = oda->GetValAsIntM(olrow, k);
  }
  taMatrix* mat = sda->GetValAsMatrix(slrow);
  if(!mat) return;
  taBase::Ref(mat);
  da->ReadToSubMatrix(row, mat, render_op, offs[0], offs[1], offs[2], offs[3], offs[4],
      offs[5], offs[6]);
  taBase::unRefDone(mat);
}


////////////////////////////

bool DataTable::SetValAsDouble(double val, const Variant& col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsDouble(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloat(float val, const Variant& col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsFloat(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsInt(int val, const Variant& col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsInt(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsString(const String& val, const Variant& col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsString(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsVar(const Variant& val, const Variant& col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsVar(val, i);
    return true;
  }
  else
    return false;
}

bool DataTable::SetValAsMatrix(const taMatrix* val, const Variant& col, int row) {
  DataCol* da = GetColData(col);
  return SetValAsMatrix_impl(val, da, row);
}

bool DataTable::SetValAsMatrixColName(const taMatrix* val, const String& col_nm, int row,
    bool quiet) {
  DataCol* da = FindColName(col_nm, true);
  return SetValAsMatrix_impl(val, da, row, quiet);
}

bool DataTable::SetValAsMatrix_impl(const taMatrix* val, DataCol* da, int row, bool quiet) {
  if (!da) return false;
  if (da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, i, quiet)) {
    return da->SetValAsMatrix(val, i);
  }
  else {
    return false;
  }
}

////////////////////////

double DataTable::GetValAsDoubleM(const Variant& col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsDoubleM(i, cell);
  else return 0.0f;
}

float DataTable::GetValAsFloatM(const Variant& col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsFloatM(i, cell);
  else return 0.0f;
}

int DataTable::GetValAsIntM(const Variant& col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsIntM(i, cell);
  else return 0;
}

const String DataTable::GetValAsStringM(const Variant& col, int row, int cell, bool na) const {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsStringM(i, cell);
  else
    return (na) ? String("n/a") : _nilString;
}

const Variant DataTable::GetValAsVarM(const Variant& col, int row, int cell) const {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsVarM(i, cell);
  else return _nilVariant;
}

////////////////////////

bool DataTable::SetValAsDoubleM(double val, const Variant& col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsDoubleM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloatM(float val, const Variant& col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsFloatM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsIntM(int val, const Variant& col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsIntM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsStringM(const String& val, const Variant& col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsStringM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsVarM(const Variant& val, const Variant& col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsVarM(val, i, cell);
    return true;
  } else return false;
}

////////////////////////
//      Matrix Dims

double DataTable::GetValAsDoubleMDims(const Variant& col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsDoubleMDims(i, d0, d1, d2, d3);
  else return 0.0f;
}

float DataTable::GetValAsFloatMDims(const Variant& col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsFloatMDims(i, d0, d1, d2, d3);
  else return 0.0f;
}

int DataTable::GetValAsIntMDims(const Variant& col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsIntMDims(i, d0, d1, d2, d3);
  else return 0;
}

const String DataTable::GetValAsStringMDims(const Variant& col, int row, int d0, int d1, int d2, int d3, bool na) const {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsStringMDims(i, d0, d1, d2, d3);
  else
    return (na) ? String("n/a") : _nilString;
}

const Variant DataTable::GetValAsVarMDims(const Variant& col, int row, int d0, int d1, int d2, int d3) const {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, i))
    return da->GetValAsVarMDims(i, d0, d1, d2, d3);
  else return _nilVariant;
}

//////////////////////////////

bool DataTable::SetValAsDoubleMDims(double val, const Variant& col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsDoubleMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloatMDims(float val, const Variant& col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsFloatMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsIntMDims(int val, const Variant& col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsIntMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsStringMDims(const String& val, const Variant& col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsStringMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsVarMDims(const Variant& val, const Variant& col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, i)) {
    da->SetValAsVarMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

//////////////////////////

bool DataTable::hasData(int col, int row) {
  DataCol* da = GetColData(col);
  int i;
  return (da && idx(row, i));
}


void DataTable::MarkCols() {
  for(int i=0;i<data.size;i++) {
    DataCol* da = data.FastEl(i);
    da->SetColFlag(DataCol::MARK);
  }
}

int DataTable::CellsPerRow() const {
  int rval = 0;
  for(int i=0;i<data.size;i++) {
    DataCol* ar = data.FastEl(i);
    rval += ar->cell_size();
  }
  return rval;
}

DataCol* DataTable::NewCol(DataCol::ValType val_type, const String& col_nm) {
  if (!NewColValid(col_nm)) return NULL;
  StructUpdate(true);
  int idx;
  DataCol* rval = NewCol_impl(val_type, col_nm, idx);
  if(!rval) {
    StructUpdate(false);
    return NULL;
  }
  rval->Init(); // asserts geom
  rval->EnforceRows(rows_total);      // new guys always get same # of rows as current table
  StructUpdate(false);
  return rval;
}

DataCol* DataTable::NewCol_gui(DataCol::ValType val_type, const String& col_nm) {
  DataCol* rval = NewCol(val_type, col_nm);
  if(rval && taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
  }
  return rval;
}

DataCol* DataTable::NewCol_impl(DataCol::ValType val_type,
    const String& col_nm, int& col_idx)
{
  TypeDef* td;
  switch (val_type) {
  case VT_STRING: td = &TA_String_Data; break;
  case VT_FLOAT:  td = &TA_float_Data; break;
  case VT_DOUBLE:  td = &TA_double_Data; break;
  case VT_INT:  td = &TA_int_Data; break;
  case VT_BYTE:  td = &TA_byte_Data; break;
  case VT_VARIANT:  td = &TA_Variant_Data; break;
  default: return NULL; // compiler food
  }
  col_idx = data.size; // next idx
  DataCol* rval = (DataCol*) data.New(1, td, col_nm);
  // additional specialized initialization
  switch (val_type) {
  case VT_STRING:
    break;
  case VT_FLOAT:
  case VT_DOUBLE:
    break;
  case VT_INT:
    rval->SetUserData(DataCol::udkey_narrow, true);
    break;
  case VT_BYTE:
    break;
  default: break; // compiler food
  }
  // DPF: Shouldn't need this line now that col_nm gets passed into New?
  // Name isn't made unique in New call since owner isn't set, and doesn't
  // happen here either.
  rval->SigEmitUpdated(); // because we set name after creation
  return rval;
}

DataCol* DataTable::NewColToken_impl(DataCol::ValType val_type, const String& col_nm) {
  TypeDef* td;
  switch (val_type) {
  case VT_STRING: td = &TA_String_Data; break;
  case VT_FLOAT:  td = &TA_float_Data; break;
  case VT_DOUBLE:  td = &TA_double_Data; break;
  case VT_INT:  td = &TA_int_Data; break;
  case VT_BYTE:  td = &TA_byte_Data; break;
  case VT_VARIANT:  td = &TA_Variant_Data; break;
  default: return NULL; // compiler food
  }
  DataCol* rval = (DataCol*) taBase::MakeToken(td);
  if(!rval) return rval;
  rval->SetName(col_nm);
  // additional specialized initialization
  switch (val_type) {
  case VT_STRING:
    break;
  case VT_FLOAT:
  case VT_DOUBLE:
    break;
  case VT_INT:
    rval->SetUserData(DataCol::udkey_narrow, true);
    break;
  case VT_BYTE:
    break;
  default: break; // compiler food
  }
  return rval;
}

float_Data* DataTable::NewColFloat(const String& col_nm) {
  return (float_Data*)NewCol(VT_FLOAT, col_nm);
}

double_Data* DataTable::NewColDouble(const String& col_nm) {
  return (double_Data*)NewCol(VT_DOUBLE, col_nm);
}

DataCol* DataTable::NewColFromChannelSpec_impl(ChannelSpec* cs) {
  DataCol* rval = NULL;
  int idx;
  if (cs->isMatrix()) {
    rval = NewColMatrixN(cs->val_type, cs->name, cs->cellGeom(), idx);
  }
  else {
    rval = NewCol(cs->val_type, cs->name);
  }
  if (rval) cs->chan_num = cols() - 1;
  return rval;
}

int_Data* DataTable::NewColInt(const String& col_nm) {
  return (int_Data*)NewCol(VT_INT, col_nm);
}

bool DataTable::NewColValid(const String& col_nm,
    const MatrixGeom* cell_geom)
{
  String err_msg;
  if (TestError((data.FindName(col_nm) != NULL), "NewCol",
      "Column with name '" + col_nm + "' already exists")) return false;
  if (cell_geom) {
    // note: no flex-sizing allowed in cell geoms, must be fully spec'ed
    // note: concat err msg to reduce chance it will be interpreted as duplicate
    bool ok = taMatrix::GeomIsValid(*cell_geom, &err_msg, false);
    String msg("Invalid geom: " + err_msg);
    if (TestError(!ok, "NewColMatrix", msg)) return false;
  }
  return true;
}

DataCol* DataTable::NewColMatrix(DataCol::ValType val_type, const String& col_nm,
    int dims, int d0, int d1, int d2, int d3, int d4, int d5, int d6)
{
  if(dims < 1) {                // < 1 is shortcut for not actually a matrix!
    return NewCol(val_type, col_nm);
  }
  MatrixGeom geom(dims, d0, d1, d2, d3, d4, d5, d6);
  int idx;
  DataCol* rval = NewColMatrixN(val_type, col_nm, geom, idx);
  return rval;
}

DataCol* DataTable::NewColMatrixN(DataCol::ValType val_type,
    const String& col_nm, const MatrixGeom& cell_geom, int& col_idx)
{
  if (!NewColValid(col_nm, &cell_geom)) return NULL;
  StructUpdate(true);
  DataCol* rval = NewCol_impl(val_type, col_nm, col_idx);
  if(!rval) {
    StructUpdate(false);
    return NULL;
  }
  rval->is_matrix = true;
  rval->cell_geom = cell_geom;
  rval->Init(); // asserts geom
  rval->EnforceRows(rows);      // new guys always get same # of rows as current table
  StructUpdate(false);
  return rval;
}

DataCol* DataTable::NewColMatrixN_gui(DataCol::ValType val_type, const String& col_nm,
    const MatrixGeom& cell_geom) {
  int idx;
  DataCol* rval = NewColMatrixN(val_type, col_nm, cell_geom, idx);
  if(rval && taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
  }
  return rval;
}

DataCol* DataTable::NewColFmMatrix(taMatrix* mat, const String& col_nm) {
  if(!mat) { taMisc::Error("taDataProc::NewColFmMatrix: mat cannot be NULL"); return NULL; }
  if(col_nm == "") { taMisc::Error("taDataProc::NewColFmMatrix: col_nm must be non-empty"); return NULL; }

  ValType val_type = mat->GetDataValType();
  MatrixGeom geom(mat->dims(), mat->dim(0), mat->dim(1), mat->dim(2), mat->dim(3), mat->dim(4), mat->dim(5), mat->dim(6));
  int idx;
  DataCol* rval = NewColMatrixN(val_type, col_nm, geom, idx);
  if(!rval) return NULL;
  if(rows == 0) AddRows();
  SetValAsMatrix(mat, idx, 0);
  return rval;
}

String_Data* DataTable::NewColString(const String& col_nm) {
  return (String_Data*)NewCol(VT_STRING, col_nm);
}

void DataTable::SetColName(const String& col_nm, int col) {
  DataCol* da = GetColData(col);
  if(da) da->name = col_nm;
}

bool DataTable::RenameCol(const String& cur_nm, const String& new_nm) {
  DataCol* da = FindColName(cur_nm);
  if(TestError(!da, "RenameCol", "column named", cur_nm, "not found")) return false;
  da->name = new_nm;
  return true;
}

DataCol* DataTable::FindColName(const String& col_nm, bool err_msg) const {
  if(col_nm.empty()) return NULL;
  DataCol* da = data.FindName(col_nm);
  TestError(!da && err_msg, "FindColName",  "could not find column named:", col_nm);
  return da;
}

int DataTable::FindColNameIdx(const String& col_nm, bool err_msg) const {
  if(col_nm.empty()) return -1;
  int idx = data.FindNameIdx(col_nm);
  TestError(idx < 0 && err_msg, "FindColNameIdx",  "could not find column named:", col_nm);
  return idx;
}

DataCol* DataTable::FindMakeCol(const String& col_nm,
    ValType val_type)
{
  int idx;
  return FindMakeColName(col_nm, idx, val_type, 0);
}

DataCol* DataTable::FindMakeColMatrix(const String& col_nm,
    ValType val_type, int dims,
    int d0, int d1, int d2, int d3, int d4, int d5, int d6)
{
  int idx;
  return FindMakeColName(col_nm, idx, val_type,
      dims, d0, d1, d2, d3, d4, d5, d6);
}

DataCol* DataTable::FindMakeColMatrixN(const String& col_nm,
    ValType val_type, const MatrixGeom& cell_geom, int& col_idx)
{
  DataCol* da = FindColName(col_nm);
  if(da) {
    if(da->valType() != (ValType)val_type) {
      StructUpdate(true);
      DataCol* nda = NewColMatrixN(val_type, col_nm, cell_geom, col_idx);
      if(!nda) {
        StructUpdate(false);
        return NULL;
      }
      data.MoveIdx(data.size-1, col_idx);
      data.RemoveEl(da);        // get rid of that guy
      da = nda;
      nda->EnforceRows(rows);   // keep row-constant
      StructUpdate(false);
    }
    else if ((!da->cell_geom.Equal(cell_geom)) || (!da->is_matrix)) {
      StructUpdate(true);
      da->cell_geom = cell_geom;
      da->is_matrix = true;
      da->Init();               // asserts geom
      da->EnforceRows(rows);    // keep row-constant
      StructUpdate(false);
    }
    else {
      if (cell_geom != da->cell_geom) {
        StructUpdate(true);
        da->cell_geom = cell_geom;
        da->Init();
        StructUpdate(false);
      }
    }
    return da;
  }
  else { // not found -- make one
    return NewColMatrixN(val_type, col_nm, cell_geom, col_idx);
  }
}

DataCol* DataTable::FindMakeColName(const String& col_nm, int& col_idx,
    ValType val_type, int dims, int d0, int d1, int d2,
    int d3, int d4, int d5, int d6)
{
  if (dims < 0) dims = 0; // causes invalid results if -ve; 0=flag for scalar
  col_idx = FindColNameIdx(col_nm);
  if(col_idx >= 0) {
    DataCol* da = data.FastEl(col_idx);
    if(da->valType() != (ValType)val_type) {
      StructUpdate(true);
      // need to remove old guy first, because validate doesn't permit dupl names
      data.RemoveIdx(col_idx); // get rid of that guy
      if(dims > 0)
        da = NewColMatrix(val_type, col_nm, dims, d0, d1, d2, d3, d4, d5, d6);
      else
        da = NewCol(val_type, col_nm);
      if(!da) {
        StructUpdate(false);
        return NULL;
      }
      data.MoveIdx(data.size-1, col_idx);
      da->EnforceRows(rows);    // keep row-constant
      StructUpdate(false);
    }
    else if(da->cell_dims() != dims) {
      StructUpdate(true);
      da->cell_geom.SetGeom(dims, d0, d1, d2, d3, d4, d5, d6);
      if(dims == 0)
        da->is_matrix = false;
      else
        da->is_matrix = true;
      da->Init();               // asserts geom
      da->EnforceRows(rows);    // keep row-constant
      StructUpdate(false);
    }
    else {
      MatrixGeom mg(dims, d0, d1, d2, d3, d4, d5, d6);
      if(mg != da->cell_geom) {
        StructUpdate(true);
        da->cell_geom = mg;
        da->Init();
        StructUpdate(false);
      }
    }
    return da;
  }
  else {                        // not found -- make one
    col_idx = data.size;        // will be the next guy
    if(dims >= 1)
      return NewColMatrix(val_type, col_nm, dims, d0, d1, d2, d3, d4, d5, d6);
    else
      return NewCol(val_type, col_nm);
  }
}

void DataTable::ChangeColTypeGeom_impl(DataCol* src, ValType new_type, const MatrixGeom& g) {
  if (!src) return;
  // must validate geom first, before we start making the col!
  String err_msg;
  if (g.dims() != 0) {
    bool valid = taMatrix::GeomIsValid(g, &err_msg, false); // no flex
    if (TestError(!valid, "ChangeColTypeGeom_impl", err_msg)) return;
  }
  // make a new col of right type
  int old_idx = src->GetIndex();
  String col_nm = src->name;
  src->name = _nilString; // for new
  DataCol* new_col = NULL;
  new_col = NewColToken_impl(new_type, col_nm);
  if(!new_col) {
    return;
  }
  if (g.dims() > 0) {
    new_col->is_matrix = true;
    new_col->cell_geom = g;
  }
  new_col->Init(); // asserts geom
  new_col->EnforceRows(rows_total);
  // copy all data -- the generic copy dude copies user data, and robustly copies data
  new_col->CopyFromCol_Robust(*src);
  data.ReplaceIdx(old_idx, new_col); // atomic replace is clean..
  tabMisc::DelayedFunCall_gui(new_col, "BrowserSelectMe");
}

void DataTable::ChangeColTypeGeom(const String& col_nm, ValType new_type,
    int dims, int d0, int d1, int d2, int d3,
    int d4, int d5, int d6) {
  DataCol* da = FindColName(col_nm, true);
  if(!da) return;
  MatrixGeom mg;
  mg.SetGeom(dims, d0, d1, d2, d3, d4, d5, d6);
  if((da->valType() == new_type) &&
      ((!da->is_matrix && (mg.dims() == 0)) || da->cell_geom.Equal(mg))) return;
  ChangeColTypeGeom_impl(da, new_type, mg);
}

void DataTable::ChangeColType(const Variant& col, ValType new_type) {
  DataCol* da = GetColData(col);
  if(!da) return;
  if(da->valType() == new_type) return;
  MatrixGeom cell_geom;
  if(da->is_matrix) cell_geom = da->cell_geom;
  ChangeColTypeGeom_impl(da, new_type, cell_geom);
}

void DataTable::ChangeAllColsOfType(ValType cur_val_type, ValType new_val_type) {
  for(int i=data.size-1; i>=0; i--) {
    DataCol* da = data[i];
    if(da->valType() != cur_val_type) continue;
    ChangeColType(i, new_val_type);
  }
}

void DataTable::ChangeColType_impl() {
  if(!change_col || change_col_type < 0) return;
  DataCol* dc = change_col;
  ValType new_type = (ValType)change_col_type;
  change_col = NULL;
  change_col_type = -1; // reset
  MatrixGeom cell_geom;
  if (dc->is_matrix) cell_geom = dc->cell_geom;
  ChangeColTypeGeom_impl(dc, new_type, cell_geom);
  // note: dc is now dead..
}

void DataTable::ChangeColCellGeom_impl() {
  if(!change_col || change_col_geom.dims() == 0) return;
  DataCol* dc = change_col;
  MatrixGeom new_geom = change_col_geom;
  change_col = NULL;
  change_col_geom.Reset();
  ChangeColTypeGeom_impl(dc, dc->valType(), new_geom);
  // note: dc is now dead..
}

void DataTable::ChangeColMatToScalar_impl() {
  if(!change_col) return;
  DataCol* dc = change_col;
  change_col = NULL;
  MatrixGeom new_geom; //note: 0 dims is key to change to scalar
  ChangeColTypeGeom_impl(dc, dc->valType(), new_geom);
  // note: dc is now dead..
}


void DataTable::UniqueColNames() {
  for(int i=0;i<data.size; i++) {
    DataCol* da = data.FastEl(i);
    int dupl = 0;
    for(int j=i+1;j<data.size; j++) {
      DataCol* oda = data.FastEl(j);
      if(da->name == oda->name) {
        dupl++;
        oda->name += "_" + String(dupl);
      }
    }
  }
}

int DataTable::GetMaxCellRows(int col_fr, int col_to) {
  // metrics
  int max_cell_rows = 0; // max flat rows per cell
  for (int col = col_fr; col <= col_to; ++col) {
    DataCol* da = GetColData(col);
    if (!da) continue;
    int x; int y;
    da->Get2DCellGeom(x, y);
    max_cell_rows = MAX(max_cell_rows, y);
  }
  return max_cell_rows;
}

void DataTable::GetFlatGeom(const CellRange& cr, int& tot_cols, int& max_cell_rows) {
  // metrics
  tot_cols = 0; // total flat cols
  max_cell_rows = 0; // max flat rows per cell
  for (int col = cr.col_fr; col <= cr.col_to; ++col) {
    DataCol* da = GetColData(col);
    if (!da) continue;
    int x; int y;
    da->Get2DCellGeom(x, y);
    tot_cols += x;
    max_cell_rows = MAX(max_cell_rows, y);
  }
}

String DataTable::HeaderToTSV() {
  // metrics
  int tot_col = 0; // total flat cols
  int max_cell_rows = 0; // max flat rows per cell
  CellRange cr;
  cr.SetExtent(cols(), 1);
  GetFlatGeom(cr, tot_col, max_cell_rows);

  // allocate a reasonable best-guess buffer
  STRING_BUF(rval, (tot_col * (cr.row_to - cr.row_fr + 1)) * 15);
  int flat_col = 0; // for tabs
  for (int col = cr.col_fr; col <= cr.col_to; ++col) {
    DataCol* da = GetColData(col);
    int cell_cols; int cell_rows;
    da->Get2DCellGeom(cell_cols, cell_rows);
    if (cell_cols == 1) {
      if (flat_col++ > 0) rval.cat('\t');
      rval.cat(da->name);
    } else { // mat, so deco all
      for (int cell_col = 0; cell_col < cell_cols; ++cell_col) {
        if (flat_col++ > 0) rval.cat('\t');
        rval.cat(da->name).cat("_").cat(String(cell_col));
      }
    }
  }
  rval.cat("\n");
  return rval;
}

String DataTable::RangeToTSV(const CellRange& cr) {
  // metrics
  int tot_col = 0; // total flat cols
  int max_cell_rows = 0; // max flat rows per cell
  GetFlatGeom(cr, tot_col, max_cell_rows);

  // allocate a reasonable best-guess buffer
  STRING_BUF(rval, (tot_col * max_cell_rows * (cr.row_to - cr.row_fr + 1)) * 10);

  int flat_row = 0; // for newlines
  for (int row = cr.row_fr; row <= cr.row_to; ++row) {
    for (int cell_row = 0; cell_row < max_cell_rows; ++cell_row) {
      if (flat_row > 0) rval.cat('\n');
      int flat_col = 0; // for tabs
      for (int col = cr.col_fr; col <= cr.col_to; ++col) {
        DataCol* da = GetColData(col);
        int cell_cols; int cell_rows;
        da->Get2DCellGeom(cell_cols, cell_rows);
        for (int cell_col = 0; cell_col < cell_cols; ++cell_col) {
          if (flat_col > 0) rval.cat('\t');
          // if a real inner cell grab the value, otherwise it will be empty
          if (cell_row < cell_rows) {
            int cell = (cell_row * cell_cols) + cell_col;
            rval.cat(GetValAsStringM(col, row, cell, false));
          }
          ++flat_col;
        }
      }
      ++flat_row;
    }
  }
  return rval;
}

void DataTable::RemoveCol(const Variant& col) {
  DataCol* da = GetColData(col);
  if(!da) return;
  StructUpdate(true);
  da->Close();
  StructUpdate(false);
  //note: you can possibly get left with completely NULL rows,
  // but we don't renormalize
}

void DataTable::DuplicateCol(const Variant& col) {
  DataCol* da = GetColData(col);
  if(!da) return;
  StructUpdate(true);
  data.ChildDuplicate(da);
  StructUpdate(false);
}

void DataTable::MoveCol(int old_index, int new_index) {
  StructUpdate(true);
  data.MoveIdx(old_index, new_index);
  StructUpdate(false);
}

void DataTable::MoveRow(int old_index, int new_index) {
  StructUpdate(true);
  row_indexes.Move(old_index, new_index);
  StructUpdate(false);
}

bool DataTable::AppendRows(DataTable* append_from) {
  return taDataProc::AppendRows(this, append_from);
}

void DataTable::RemoveOrphanCols() {
  int cls_cnt = 0; // used to prevent spurious struct updates
  for(int i=data.size-1;i>=0;i--) {
    DataCol* da = data.FastEl(i);
    if(da->HasColFlag(DataCol::MARK) && !da->HasColFlag(DataCol::PIN)) {
      if(cls_cnt == 0) StructUpdate(true);
      da->Close();
      cls_cnt++;
    }
  }
  if (cls_cnt)  {
    StructUpdate(false);
  }
}

bool DataTable::InsertRows(int st_row, int n_rows) {
  if(st_row < 0) st_row = rows; // end
  if(TestError((st_row < 0 || st_row > rows), "InsertRows",
      "row not in range:",String(st_row))) return false;
  bool rval = true;
  DataUpdate(true);
  for(int i=0;i<data.size;i++) {
    DataCol* dc = data.FastEl(i);
    rval = dc->EnforceRows(rows_total + n_rows); // key to use rows_total not rows()
  }
  if(rval) {
    rows += n_rows;
    // insert into row_indexes
    for (int r=st_row; r<st_row+n_rows; r++) {
      row_indexes.InsertFrames(r, 1);
      row_indexes.FastEl_Flat(r) = rows_total;
      rows_total++;
    }
  }
  DataUpdate(false);
  return rval;
}

bool DataTable::InsertRowsAfter(int st_row, int n_rows) {
  return InsertRows(st_row + n_rows, n_rows);  // InsertRows does the range check
}

bool DataTable::RemoveRows(int st_row, int n_rows) {
  if(st_row < 0)
    st_row = rows-1;       // end

  if(TestError(!RowInRangeNormalize(st_row), "RemoveRows",
      "start row not in range:",String(st_row)))
    return false;
  if(n_rows < 0) n_rows = rows - st_row;
  int end_row = st_row + n_rows-1;
  if(TestError(!RowInRangeNormalize(end_row), "RemoveRows",
      "end row not in range:",String(end_row)))
    return false;
  DataUpdate(true);
  row_indexes.RemoveFrames(st_row, n_rows);
  rows -= n_rows;		// the number of rows not hidden by filtering or hiding
  if (rows == 0)  keygen.setInt64(0);
  DataUpdate(false);
  return true;
}

// duplicates a single row n times and places rows at end of table
bool DataTable::DuplicateRow(int row_no, int n_copies) {
  if(TestError(!RowInRangeNormalize(row_no), "DuplicateRow",
      "row not in range:",String(row_no)))
    return false;

  int src_dx;
  if(!idx(row_no, src_dx))
    return false;

  DataUpdate(true);   // only data because for views, no change in column structure
  for(int k=0;k<n_copies;k++) {
    AddBlankRow();
    data.CopyFromRow(-1, data, src_dx);
  }
  DataUpdate(false);
  return true;
}

// duplicates multiple rows and inserts them after the selected rows
bool DataTable::DuplicateRows(int st_row, int n_rows) {
  DataUpdate(true);   // only data, no change in column structure
  bool rval = InsertRows(st_row + n_rows, n_rows);
  if (rval) {
    for (int i = 0; i < n_rows; i++) {
      int src_dx, dest_dx;
      idx(st_row + i, src_dx);
      idx(st_row + n_rows + i, dest_dx);
      data.CopyFromRow(dest_dx, data, src_dx);
    }
  }
  DataUpdate(false);
  return rval;
}

bool DataTable::RowInRangeNormalize(int& row) {
  if (row < 0) row = rows + row;
  return ((row >= 0) && (row < rows));
}

// jar 5/19/13 - look into these different resets
void DataTable::Reset() {
  StructUpdate(true);
  data.Reset();
  rows_total = 0;
  row_indexes.Reset();
  ClearCompareRows();

  keygen.setInt64(0);
  StructUpdate(false);
}

void DataTable::ResetData() {  // this permanently deletes all row data!
  if (rows == 0 && rows_total == 0) return; // prevent erroneous m_dm calls
  StructUpdate(true);
  for(int i=0;i<data.size;i++) {
    DataCol* dc = data.FastEl(i);
    dc->UnSetMatrixViewMode();
    dc->AR()->Reset();
    dc->SetMatrixViewMode();
  }
  rows = 0;
  rows_total = 0;
  row_indexes.Reset();
  ClearCompareRows();

  keygen.setInt64(0);
  StructUpdate(false);
  // also update itrs in case using simple itr mode
  ReadItrInit();
  WriteItrInit();
}

void DataTable::RowsAdding(int n, bool begin) {
  if (begin) {
    DataUpdate(true);// only data because for views, no change in column structure
  }
  else { // end
    rows += n;
    rows_total +=n;
    DataUpdate(false);
  }
}

//////////////////////////////////////////////////////////////////////////////
///     Saving / Loading from Emergent or Plain Text Files

/*
  Emergent Header format:
  $ = String
  % = float
  # = double
  | = int
  @ = byte
  & = variant
 * = void*

  matrix: each cell has [dims:x,y..]
  first mat cell also has <dims:dx,dy..>

  Plain Header Format
 */

void DataTable::SaveHeader_strm_impl(ostream& strm, Delimiters delim,
    bool row_mark, int col_fr, int col_to, bool native, bool quote_str)
{
  char cdlm = GetDelim(delim);
  bool need_delim = false; // goes true after first col written
  if (row_mark) {
    strm << "_H:";              // indicates header
    need_delim = true;
  }
  // validate and adjust col range
  if (col_fr < 0) col_fr = data.size + col_fr;
  if (col_fr < 0) col_fr = 0;
  if (col_to < 0) col_to = data.size + col_to;
  if ((col_to < 0) || (col_to >= data.size))
    col_to = data.size - 1;
  MatrixIndex dims;
  for(int i=0;i<data.size;i++) {
    DataCol* da = data.FastEl(i);
    if(!da->saveToDataFile()) continue;
    if (da->cell_size() == 0) continue;

    if (need_delim)
      strm << cdlm;
    need_delim = true;
    String hdnm;
    if(da->isMatrix()) {
      for(int j=0;j<da->cell_size(); j++) {
        if (j > 0)
          strm << cdlm;
        if (native) {
          da->cell_geom.DimsFmIndex(j, dims);
          hdnm = da->EncodeHeaderName(dims);
        } else {
          hdnm = da->name + "_" + String(j);
          // note: for Export, we enclose headers w/ quotes to be standard
          if (quote_str)
            hdnm = "\"" + hdnm + "\"";
        }
        strm << hdnm;
      }
    }
    else {
      if (native) {
        hdnm = da->EncodeHeaderName(dims); //note: dims ignored
      } else
        hdnm = da->name;
      strm << hdnm;
    }
  }
  strm << endl;
}

void DataTable::SaveDataRow_strm(ostream& strm, int row, Delimiters delim,
    bool quote_str, bool row_mark, int col_fr, int col_to)
{
  char cdlm = GetDelim(delim);
  bool need_delim = false; // goes true after first col written
  if (row_mark) {
    strm << "_D:";              // indicates data row
    need_delim = true;
  }
  // validate and adjust col range
  if (col_fr < 0) col_fr = data.size + col_fr;
  if (col_fr < 0) col_fr = 0;
  if (col_to < 0) col_to = data.size + col_to;
  if ((col_to < 0) || (col_to >= data.size))
    col_to = data.size - 1;
  for(int i = col_fr; i <= col_to;i++) {
    DataCol* da = data.FastEl(i);
    if(!da->saveToDataFile()) continue;
    if (da->cell_size() == 0) continue;
    if (need_delim)
      strm << cdlm;
    need_delim = true;
    if(da->isMatrix()) {
      for(int j=0;j<da->cell_size(); j++) {
        if (j > 0)
          strm << cdlm;
        String val = da->GetValAsStringM(row, j);
        // TODO: 1) need to check for Variant.String type -- this is very difficult..
        if(quote_str && (da->valType() == VT_STRING))
          strm << "\"" << val.quote_esc() << "\"";
        else
          strm << val;
      }
    }
    else {
      String val = da->GetValAsString(row);
      //TODO: see above in mat
      if(quote_str && (da->valType() == VT_STRING))
        strm << "\"" << val.quote_esc() << "\"";
      else
        strm << val;
    }
  }
  strm << endl;
}

void DataTable::ExportDataJSON(const String& fname) {  // write the entire table to file
  // note: don't get file name when exporting
  taFiler* flr = GetSaveFiler(fname, ".json", false);
  if (flr->ostrm) {
    GetDataAsJSON(*flr->ostrm );
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

bool DataTable::GetDataMatrixCellAsJSON(ostream& strm, const String& column_name, int row, int cell) {
  if (row < 0) {
    row = rows + row;  // so for -1 you get the last row
  }

  if (row <0 || row >= rows) {
    error_msg = "row out of range";
    return false;
  }


  int col_idx = this->FindColNameIdx(column_name);
  if (col_idx == -1) {
    error_msg = "column not found";
    return false;
  }

  DataCol* dc = data.FastEl(col_idx);
  if (!dc->is_matrix) {
    error_msg = "asking for a cell of non-matrix column ";
    return false;
  }

  if (cell <0 || cell >= dc->cell_size()) {
    error_msg = "cell out of range error";
    return false;
  }

  JSONNode root(JSON_NODE);
  switch (dc->valType()) {
  case VT_STRING:
    root.push_back(JSONNode("value", json_string(dc->GetValAsStringM(row, cell).chars())));
    break;
  case VT_DOUBLE:
    root.push_back(JSONNode("value", dc->GetValAsDoubleM(row, cell)));
    break;
  case VT_FLOAT:
    root.push_back(JSONNode("value", dc->GetValAsFloatM(row, cell)));
    break;
  case VT_INT:
    root.push_back(JSONNode("value", dc->GetValAsIntM(row, cell)));
    break;
  case VT_BYTE:
    root.push_back(JSONNode("value", dc->GetValAsByteM(row, cell)));
    break;
  case VT_VARIANT:
    root.push_back(JSONNode("value", json_string(dc->GetValAsStringM(row, cell).chars())));
    break;
  default:
    root.push_back(JSONNode("value", json_string(dc->GetValAsStringM(row, cell).chars())));
    taMisc::Info("DataTable::ExportDataJSON_impl -- column type undefined - should not happen");
  }
  std::string theString = root.write_formatted();
  taMisc::DebugInfo(theString.c_str());
  strm << theString;
  strm << endl;

  return true;
}

bool DataTable::GetDataAsJSON(ostream& strm, const String& column_name, int start_row, int n_rows) {
  int stop_row;

  JSONNode root(JSON_NODE);
  JSONNode columns(JSON_ARRAY);
  columns.set_name("columns");

  if (start_row < 0) {
    start_row = rows + start_row;  // so for -1 you get the last row
  }

  if (start_row < 0 || start_row >= rows) {  // start_row could still be negative if we were passed -20 when there were only 10 rows
    error_msg = "row out of range";
    return false;
  }

  // do something reasonable
  if (n_rows < 0 || (start_row + n_rows) > rows) {
    stop_row = rows;
  }
  else {
    stop_row = start_row + n_rows;
  }

  int_Array columnList;
  if (column_name.empty()) {  // single column not specified - gen a list of all columns
    for (int i=0; i<data.size; i++) {
      columnList.Add(i);
    }
  }
  else {
    int col_idx = this->FindColNameIdx(column_name);
    if (col_idx == -1) {
      error_msg = "column not found";
      return false;
    }
    else {
      columnList.Add(col_idx);
    }
  }

  for (int i=0; i<columnList.size; i++) {
    JSONNode aColumn(JSON_NODE);
    DataCol* dc = data.FastEl(columnList[i]);
    aColumn.push_back(JSONNode("name", json_string(dc->name.chars())));
    String val = ValTypeToStr(dc->valType());
    aColumn.push_back(JSONNode("type", val.chars()));
    aColumn.push_back(JSONNode("matrix", dc->is_matrix));
    if (dc->is_matrix) {
      JSONNode dimensions(JSON_ARRAY);
      dimensions.set_name("dimensions");
      for (int d=0; d<dc->cell_geom.n_dims; d++) {
        dimensions.push_back(JSONNode("", dc->cell_geom.size(d)));
      }
      aColumn.push_back(dimensions);
    }
    JSONNode values(JSON_ARRAY);
    values.set_name("values");
    if (!dc->is_matrix) {  // single array of values - row 1 to row n
      for (int j=start_row; j < stop_row; j++) {
        switch (dc->valType()) {
        case VT_STRING:
          values.push_back(JSONNode("", json_string(dc->GetValAsString(j).chars())));
          break;
        case VT_DOUBLE:
          values.push_back(JSONNode("", dc->GetValAsDouble(j)));
          break;
        case VT_FLOAT:
          values.push_back(JSONNode("", dc->GetValAsFloat(j)));
          break;
        case VT_INT:
          values.push_back(JSONNode("", dc->GetValAsInt(j)));
          break;
        case VT_BYTE:
          values.push_back(JSONNode("", dc->GetValAsByte(j)));
          break;
        case VT_VARIANT:
          values.push_back(JSONNode("", json_string(dc->GetValAsString(j).chars())));
          break;
        default:
          values.push_back(JSONNode("", json_string(dc->GetValAsString(j).chars())));
          taMisc::Info("DataTable::ExportDataJSON_impl -- column type undefined - should not happen");
        }
      }
    }

    if(dc->is_matrix) {
      if (dc->cell_geom.dims() == 2) {
        for (int row=start_row; row < stop_row; row++) {
          JSONNode matrixValues(JSON_ARRAY);
          for(int k = 0; k < dc->cell_geom.size(1); k++) {
            JSONNode matrixDim_1Values(JSON_ARRAY);
            for(int j = 0; j < dc->cell_geom.size(0); j++) {
              switch (dc->valType()) {
              case VT_STRING:
                matrixDim_1Values.push_back(JSONNode("", json_string(dc->GetValAsStringMDims(row, j, k).chars())));
                break;
              case VT_DOUBLE:
                matrixDim_1Values.push_back(JSONNode("", dc->GetValAsDoubleMDims(row, j, k)));
                break;
              case VT_FLOAT:
                matrixDim_1Values.push_back(JSONNode("", dc->GetValAsFloatMDims(row, j, k)));
                break;
              case VT_INT:
                matrixDim_1Values.push_back(JSONNode("", dc->GetValAsIntMDims(row, j, k)));
                break;
              case VT_BYTE:
                matrixDim_1Values.push_back(JSONNode("", dc->GetValAsByteMDims(row, j, k)));
                break;
              case VT_VARIANT:
                matrixDim_1Values.push_back(JSONNode("", json_string(dc->GetValAsStringMDims(row, j, k).chars())));
                break;
              default:
                matrixDim_1Values.push_back(JSONNode("", json_string(dc->GetValAsStringMDims(row, j, k).chars())));
              }
            }
            matrixValues.push_back(matrixDim_1Values);
          }
          values.push_back(matrixValues);
        }
      }
      if (dc->cell_geom.dims() == 3) {
        for (int row=start_row; row < stop_row; row++) {
          JSONNode matrixValues(JSON_ARRAY);
          for(int l = 0; l < dc->cell_geom.size(2); l++) {
            JSONNode matrixDim_2Values(JSON_ARRAY);
            for(int k = 0; k < dc->cell_geom.size(1); k++) {
              JSONNode matrixDim_1Values(JSON_ARRAY);
              for(int j = 0; j < dc->cell_geom.size(0); j++) {
                switch (dc->valType()) {
                case VT_STRING:
                  matrixDim_1Values.push_back(JSONNode("", json_string(dc->GetValAsStringMDims(row,j, k, l).chars())));
                  break;
                case VT_DOUBLE:
                  matrixDim_1Values.push_back(JSONNode("", dc->GetValAsDoubleMDims(row,j, k, l)));
                  break;
                case VT_FLOAT:
                  matrixDim_1Values.push_back(JSONNode("", dc->GetValAsFloatMDims(row,j, k, l)));
                  break;
                case VT_INT:
                  matrixDim_1Values.push_back(JSONNode("", dc->GetValAsIntMDims(row,j, k, l)));
                  break;
                case VT_BYTE:
                  matrixDim_1Values.push_back(JSONNode("", dc->GetValAsByteMDims(row,j, k, l)));
                  break;
                case VT_VARIANT:
                  matrixDim_1Values.push_back(JSONNode("", json_string(dc->GetValAsStringMDims(row,j, k, l).chars())));
                  break;
                default:
                  matrixDim_1Values.push_back(JSONNode("", json_string(dc->GetValAsStringMDims(row,j, k, l).chars())));
                }
              }
              matrixDim_2Values.push_back(matrixDim_1Values);
            }
            matrixValues.push_back(matrixDim_2Values);
          }
          values.push_back(matrixValues);
        }
      }
      if (dc->cell_geom.dims() == 4) {
        for (int row=start_row; row < stop_row; row++) {
          JSONNode matrixValues(JSON_ARRAY);
          for(int m = 0; m < dc->cell_geom.size(3); m++) {
            JSONNode matrixDim_3Values(JSON_ARRAY);
            for(int l = 0; l < dc->cell_geom.size(2); l++) {
              JSONNode matrixDim_2Values(JSON_ARRAY);
              for(int k = 0; k < dc->cell_geom.size(1); k++) {
                JSONNode matrixDim_1Values(JSON_ARRAY);
                for(int j = 0; j < dc->cell_geom.size(0); j++) {
                  switch (dc->valType()) {
                  case VT_STRING:
                    matrixDim_1Values.push_back(JSONNode("", json_string(dc->GetValAsStringMDims(row, j, k, l, m).chars())));
                    break;
                  case VT_DOUBLE:
                    matrixDim_1Values.push_back(JSONNode("", dc->GetValAsDoubleMDims(row, j, k, l, m)));
                    break;
                  case VT_FLOAT:
                    matrixDim_1Values.push_back(JSONNode("", dc->GetValAsFloatMDims(row, j, k, l, m)));
                    break;
                  case VT_INT:
                    matrixDim_1Values.push_back(JSONNode("", dc->GetValAsIntMDims(row, j, k, l, m)));
                    break;
                  case VT_BYTE:
                    matrixDim_1Values.push_back(JSONNode("", dc->GetValAsByteMDims(row, j, k, l, m)));
                    break;
                  case VT_VARIANT:
                    matrixDim_1Values.push_back(JSONNode("", json_string(dc->GetValAsStringMDims(row, j, k, l, m).chars())));
                    break;
                  default:
                    matrixDim_1Values.push_back(JSONNode("", json_string(dc->GetValAsStringMDims(row, j, k, l, m).chars())));
                  }
                }
                matrixDim_2Values.push_back(matrixDim_1Values);
              }
              matrixDim_3Values.push_back(matrixDim_2Values);
            }
            matrixValues.push_back(matrixDim_3Values);
          }
          values.push_back(matrixValues);
        }
      }
    }
    aColumn.push_back(values);
    columns.push_back(aColumn);
  }
  root.push_back(columns);
  std::string theString = root.write_formatted();
  strm << theString;
  strm << endl;

  return true;
}

void DataTable::SaveDataRows_strm(ostream& strm, Delimiters delim, bool quote_str, bool row_mark) {
  for(int row=0;row <rows; row++) {
    SaveDataRow_strm(strm, row, delim, quote_str, row_mark);
  }
}

void DataTable::SaveData_strm(ostream& strm, Delimiters delim, bool quote_str, bool save_headers) {
  if(save_headers)
    SaveHeader_strm(strm, delim);
  SaveDataRows_strm(strm, delim, quote_str, save_headers); // last arg is row mark -- only if headers
}

void DataTable::ExportData_strm(ostream& strm, Delimiters delim,
    bool quote_str, bool headers)
{
  if (headers)
    ExportHeader_strm(strm, delim);
  SaveDataRows_strm(strm, delim, quote_str, false); // last arg is row mark -- never
}

void DataTable::SaveHeader(const String& fname, Delimiters delim) {
  taFiler* flr = GetSaveFiler(fname, ".dat,.tsv,.csv,.txt,.log", false);
  if(flr->ostrm)
    SaveHeader_strm(*flr->ostrm, delim);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveDataRow(const String& fname, int row, Delimiters delim, bool quote_str, bool row_mark) {
  taFiler* flr = GetSaveFiler(fname, ".dat,.log", false, "Data");
  if(flr->ostrm)
    SaveDataRow_strm(*flr->ostrm, row, delim, quote_str, row_mark);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveData(const String& fname, Delimiters delim,
    bool quote_str, bool save_headers)
{
  taFiler* flr = GetSaveFiler(fname, ".dat,.log", false, "Data");
  if (flr->ostrm) {
    SaveData_strm(*flr->ostrm, delim, quote_str, save_headers);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::ExportData(const String& fname, Delimiters delim,
    bool quote_str, bool save_headers)
{
  // note: don't get file name when exporting
  taFiler* flr = GetSaveFiler(fname, ".csv,.tsv,.txt,.log", false, "Data", false);
  if (flr->ostrm) {
    ExportData_strm(*flr->ostrm, delim, quote_str, save_headers);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::AppendData(const String& fname, Delimiters delim, bool quote_str, bool row_mark) {
  taFiler* flr = GetAppendFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  if(flr->ostrm)
    SaveDataRows_strm(*flr->ostrm, delim, quote_str, row_mark);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveDataLog(const String& fname, bool append, bool dmem_proc_0) {
  if(!log_file) return;         // shouldn't happen
  if(log_file->IsOpen())
    log_file->Close();
#ifdef DMEM_COMPILE
  if((taMisc::dmem_proc > 0) && dmem_proc_0) return; // don't open!
#endif
  log_file->SetFileName(fname);
  if(fname.empty()) {
    if(append)
      log_file->Append();
    else {
      log_file->SaveAs(false);  // no save to tmp
      if(log_file->IsOpen())
        SaveHeader_strm(*log_file->ostrm);
    }
  }
  else {
    if(append)
      log_file->open_append();
    else {
      log_file->open_write();
      if(log_file->IsOpen())
        SaveHeader_strm(*log_file->ostrm);
    }
  }
}

void DataTable::CloseDataLog() {
  if(!log_file) return;
  log_file->Close();
}

bool DataTable::WriteDataLogRow() {
  if(log_file && log_file->IsOpen()) {
    SaveDataRow_strm(*log_file->ostrm);
    return true;
  }
  return false;
}

void DataTable::ShowAllRows() {
  StructUpdate(true);
  ResetRowIndexes();
  ClearCompareRows();
  StructUpdate(false);
}

bool DataTable::Flatten() {
  return FlattenTo(this);
}

bool DataTable::FlattenTo(DataTable* flattened_table) {
  bool made_new = false;
  bool in_place = false;
  if(flattened_table == NULL) {
    DataTable_Group* group = GET_OWNER(this, DataTable_Group);
    flattened_table = group->NewEl(1, NULL);   // add a new data table to the group
    made_new = true;   // note: set the name of new tables
  }
  else if(flattened_table == this) { // in place
    in_place = true;
    flattened_table = new DataTable;
  }

  flattened_table->StructUpdate(true);
  flattened_table->Copy_NoData(*this);
  if(made_new) {
    flattened_table->SetName(name + "_flattened");
  }
  flattened_table->EnforceRows(this->rows); // more efficient to allocate up-front
  for (int row=0; row < this->rows; row++) {
    flattened_table->CopyFromRow(row, *this, row);
  }
  flattened_table->StructUpdate(false);

  if(in_place) {
    this->CopyFrom(flattened_table);
    delete flattened_table;
  }
  return true;
}

char DataTable::GetDelim(Delimiters delim) {
  if(delim == TAB) return '\t';
  if(delim == SPACE) return ' ';
  if(delim == COMMA) return ',';
  return ','; // must return something
}

int DataTable::ReadTillDelim(istream& strm, String& str, const char delim, bool quote_str,
    bool& got_quote) {
  got_quote = false;
  int c;
  int depth = 0;
  if(quote_str && (strm.peek() == '\"')) {
    got_quote = true;
    strm.get();
    depth++;
  }
  
  // remove leading spaces
  while ((c = strm.peek()) == ' ') {
    strm.get()  ;
  }

  bool bs = false;
  while(((c = strm.get()) != EOF) &&
      !(((c == delim) || (c == '\n') || (c == '\a') || (c == '\r')) && (depth <= 0))) {
    if(c == '\\') {
      if(!bs) {                  // actual bs
        bs = true;
        continue;
      }
    }
    if(bs && c == 'n') c = '\n';
    if(bs && c == 'r') c = '\r';
    if(bs && c == 't') c = '\t';
    if(quote_str && (depth > 0) && !bs && (c == '\"'))
      depth--;
    else
      str += (char)c;
    bs = false;                 // no longer true
  }
  // consume lf of crlf-pair for Windows files
  if ((c == '\r') && (strm.peek() == '\n'))
    c = strm.get();
  return c;
}

int DataTable::ReadTillDelim_Str(const String& istr, int& idx, String& str,
    const char delim, bool quote_str, bool& got_quote) {
  got_quote = false;
  int c = EOF;
  int depth = 0;
  int len = istr.length();
  if(idx >= len) return EOF;
  if(quote_str && (istr[idx] == '\"')) {
    got_quote = true;
    idx++;
    depth++;
  }
  // remove leading spaces
  while (idx < len && (c = istr[idx]) == ' ') {
    idx++;
  }
  while(idx < len) {
    c = istr[idx++];
    if((c != '\n') && (c != '\r') && !((c == delim) && (depth <= 0))) {
      if(quote_str && (depth > 0) && (c == '\"'))
        depth--;
      else
        str += (char)c;
    }
    else {
      break;
    }
  }
  // consume lf of crlf-pair for Windows files
  if((idx < len) && (c == '\r') && (istr[idx] == '\n'))
    c = istr[idx++];
  return c;
}

int_Array DataTable::load_col_idx;
int_Array DataTable::load_mat_idx;

int DataTable::LoadHeader_impl(istream& strm, Delimiters delim, bool native, bool quote_str)
{
  if (native) quote_str = false; // never quotes for native headers
  char cdlm = GetDelim(delim);
  load_col_idx.Reset();
  load_mat_idx.Reset();
  int c;
  bool cont = true;
  bool got_quote = false;
  while(cont) {
    String str;
    c = ReadTillDelim(strm, str, cdlm, quote_str, got_quote);
    cont = !((c == '\n') || (c == '\r') || (c == EOF));
    if (str.empty()) {
      if (!cont) break;
      continue;                 // for some reason it is empty
    }
    String base_nm;
    int val_typ;
    int col_idx;
    int cell_idx = -1; // mat cell index, or -1 if not a mat
    DataCol* da = NULL;
    // Load vs. Import decoding is sufficiently different we use two subroutines below
    if (native) {
      MatrixIndex mat_idx;
      MatrixGeom mat_geom;
      // val_typ =-1 means type not explicitly supplied -- we'll use existing if name found
      // for import cols, if nm is like <Name>_<int> and Name is a mat col, then assumed to
      // be a mat column (ex. as previously Exported) otherwise a literal scalar column
      DataCol::DecodeHeaderName(str, base_nm, val_typ, mat_idx, mat_geom);
      col_idx = FindColNameIdx(base_nm);
      if (col_idx >= 0) da = data.FastEl(col_idx);
      if (val_typ < 0) {
        if (da) val_typ = da->valType(); // the actual type
        else  val_typ = VT_FLOAT; // default for Emergent-native
      }
      if(!da || (da->valType() != val_typ)) { // only make new one if val type doesn't match
        // mat_geom is only decorated onto first col and should not be remade...
        // if none was supplied, then set it for scalar col (the default)
        if ((mat_idx.dims() == 0) || mat_geom.dims() != 0) {
          da = FindMakeColName(base_nm, col_idx, (ValType)val_typ, mat_geom.dims(),
              mat_geom[0], mat_geom[1], mat_geom[2],
              mat_geom[3]);
        }
      }
      if(mat_idx.dims() > 0) {
        cell_idx = da->cell_geom.IndexFmDims(mat_idx[0], mat_idx[1], mat_idx[2], mat_idx[3]);
      } // else is default=-1
    } else { // Import
      DecodeImportHeaderName(str, base_nm, cell_idx); // strips final _<int> leaves base_nm
      col_idx = FindColNameIdx(base_nm);
      if (col_idx >= 0) da = data.FastEl(col_idx);
      // we only accept _xxx as mat col designator if col already a mat, and cell in bounds
      if (!(da && (cell_idx >= 0) && (da->is_matrix) &&
          (cell_idx < da->cell_geom.Product())))
      {
        da = FindMakeCol(str, VT_VARIANT);
        col_idx = da->col_idx;
        cell_idx = -1;
      }
    }
    load_col_idx.Add(col_idx);
    load_mat_idx.Add(cell_idx); // no matrix info

  }
  return c;
}

int DataTable::LoadDataRow_impl(istream& strm, Delimiters delim, bool quote_str) {
  static int last_row_err_msg = -1;
  char cdlm = GetDelim(delim);
  StructUpdate(true);
  bool added_row = false;
  int last_mat_col = -1;
  int load_col = 0;             // loading column (always incr)
  int data_col = 0;             // data column (datacol index in data table)
  bool got_quote = false;
  int c;
  while(true) {
    STRING_BUF(str, 32); // provide a buff so numbers and short strings are efficient
    c = ReadTillDelim(strm, str, cdlm, quote_str, got_quote);
    if (str.empty() && (c == EOF)) break;
    if(str == "_H:") {
      c = LoadHeader_impl(strm, delim, true);
      if(c == EOF) break;
      continue;
    }
    if(str == "_D:") continue;
    // at this point it is safe to add a row -- load header already called
    if(!added_row) {
      AddBlankRow();
      added_row = true;
    }
    if(load_col_idx.size > 0) {
      data_col = load_col_idx[load_col];
    }
    if(data_col >= data.size) {
      if(last_row_err_msg != rows-1) // don't repeat err msg a zillion times!
        TestWarning(true, "LoadDataRow_strm", "columns exceeded!");
      last_row_err_msg = rows;
      c = '\n';
      break;
    }
    DataCol* da = data.FastEl(data_col);
    if(da->isMatrix()) {
      int mat_idx = 0;
      if(load_mat_idx.size > 0) {
        mat_idx = load_mat_idx[load_col];
        da->SetValAsStringM(str, -1, mat_idx);
      }
      else {
        if(last_mat_col >= 0)
          mat_idx = load_col - last_mat_col;
        else
          last_mat_col = load_col;
        if(mat_idx >= da->cell_size()) { // filled up the matrix!
          data_col++;
          if(data_col >= data.size) {
            if(last_row_err_msg != rows-1) // don't repeat err msg a zillion times!
              TestWarning(true, "LoadDataRow_strm", "matrix columns exceeded!");
            last_row_err_msg = rows;
            c = '\n';
            break;
          }
          da = data.FastEl(data_col);
          last_mat_col = -1;
          da->SetValAsString(str, -1);
        }
        else {
          da->SetValAsStringM(str, -1, mat_idx);
        }
      }
    }
    else {
      last_mat_col = -1;
      da->SetValAsString(str, -1);
      data_col++;
    }
    load_col++;
    if ((c == '\n') || (c == '\r') || (c == EOF)) break;
  }
  StructUpdate(false);

  return c;
}

int DataTable::LoadDataFixed_impl(istream& strm, FixedWidthSpec* fws) {
  fws->Load_Init(this); // caches cols and indicates any missing cols
  string line;
  int n_skip = fws->n_skip_lines;
  StructUpdate(true);
  while (std::getline(strm, line)) {
    if (n_skip > 0) {
      --n_skip;
      continue;
    }
    // note: data() is not null terminated!
    String ln(line.data(), static_cast<int>(line.length()));
    fws->AddRow(ln);
  }

  StructUpdate(false);
  return true;
}

int DataTable::LoadHeader_strm(istream& strm, Delimiters delim) {
  return LoadHeader_impl(strm, delim, true);
}

int DataTable::LoadDataRow_strm(istream& strm, Delimiters delim, bool quote_str) {
  ResetLoadSchema();
  return LoadDataRow_impl(strm, delim, quote_str);
}

int DataTable::LoadDataRowEx_strm(istream& strm, Delimiters delim,
    bool quote_str, bool reset_load_schema)
{
  if (reset_load_schema)
    ResetLoadSchema();
  return LoadDataRow_impl(strm, delim, quote_str);
}

void DataTable::ResetLoadSchema() const {
  load_col_idx.Reset();
  load_mat_idx.Reset();
}

void DataTable::LoadData_strm(istream& strm, Delimiters delim, bool quote_str,
    int max_recs)
{
  StructUpdate(true);
  ResetLoadSchema();
  int st_row = rows;
  while(true) {
    int c = LoadDataRow_impl(strm, delim, quote_str);
    if(c == EOF) break;
    if((max_recs > 0) && (rows - st_row >= max_recs)) break;
  }
  StructUpdate(false);
}

int DataTable::LoadHeader(const String& fname, Delimiters delim) {
  taFiler* flr = GetLoadFiler(fname, ".dat,.tsv,.csv,.txt,.log", false);
  int rval = 0;
  if(flr->istrm)
    rval = LoadHeader_impl(*flr->istrm, delim, true);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

int DataTable::LoadDataRow(const String& fname, Delimiters delim, bool quote_str) {
  ResetLoadSchema();
  taFiler* flr = GetLoadFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  int rval = 0;
  if(flr->istrm)
    rval = LoadDataRow_impl(*flr->istrm, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void DataTable::LoadData(const String& fname, Delimiters delim, bool quote_str,
    int max_recs, bool reset_first)
{
  taFiler* flr = GetLoadFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  if(flr->istrm) {
    if(reset_first)
      RemoveAllRows();
    LoadData_strm(*flr->istrm, delim, quote_str, max_recs);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::LoadAnyData(const String& fname, bool headers_req,
    LoadDelimiters delim_req, LoadQuotes quote_str_req, int max_recs,
    bool reset_first)
{
  taFiler* flr = GetLoadFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  if (flr->istrm) {
    bool headers;
    Delimiters delim;
    bool quote_str;
    bool native;
    DetermineLoadDataParams(*flr->istrm, headers_req, delim_req, quote_str_req,
        headers, delim, quote_str, native);
    if (reset_first)
      RemoveAllRows();
    flr->open_read();           // always re-open -- if read goes past EOF, then bad
    // things happen, and needed for compressed .gz
    if (native) {
      LoadData_strm(*flr->istrm, delim, quote_str, max_recs);
    } else {
      ImportData_strm(*flr->istrm, headers, delim, quote_str, max_recs);
    }
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

DataCol::ValType DataTable::StrToValType(String valTypeStr) {
  if (valTypeStr == "String")
    return VT_STRING;
  else if (valTypeStr == "int")
    return VT_INT;
  else if (valTypeStr == "float")
    return VT_FLOAT;
  else if (valTypeStr == "double")
    return VT_DOUBLE;
  else if (valTypeStr == "byte")
    return VT_BYTE;
  else if (valTypeStr == "var")
    return VT_VARIANT;
  else {
    taMisc::Info("DataTable::StrToValType -- json column type string not found -- using variant type");
    return VT_VARIANT;  // best we can do - should also print out info message
  }
}

void DataTable::ImportDataJSON(const String& fname) {
  bool reset_first = true;
  json_string contents;

  taFiler* flr = GetLoadFiler(fname, ".json", false, "Data");
  if(flr->istrm) {
    if(reset_first)
      flr->istrm->seekg(0, std::ios::end);
    contents.resize(flr->istrm->tellg());
    flr->istrm->seekg(0, std::ios::beg);
    flr->istrm->read(&contents[0], contents.size());
  }
  flr->Close();
  taRefN::unRefDone(flr);

  if (libjson::is_valid(contents)) {
    JSONNode n = libjson::parse(contents);
    SetDataFromJSON(n);
  }
  else {
    taMisc::Error("ImportDataJSON: ", "The json file has a format error, look for missing/extra bracket, brace or quotation");
  }
}

void DataTable::ImportDataJSONString(const String& json_as_string) {
  if (libjson::is_valid(json_string(json_as_string.chars()))) {
    JSONNode n = libjson::parse(json_string(json_as_string.chars()));
    SetDataFromJSON(n);
  }
  else {
    taMisc::Error("ImportDataJSON: ", "The json file has a format error, look for missing/extra bracket, brace or quotation");
  }
}

bool DataTable::SetDataFromJSON(const JSONNode& n, int start_row, int start_cell) { // // row -1 means append, anything else overwrites starting at row
  bool rval = true;
  JSONNode::const_iterator i = n.begin();
  while (i != n.end()){
    // recursively call ourselves to dig deeper into the tree
    if (i->type() == JSON_ARRAY || i->type() == JSON_NODE) {
      rval = SetDataFromJSON(*i);
      if (rval == false) {
        return rval;
      }
    }

    // get the node name and value as a string
    std::string node_name = i->name();

    if (node_name == "columns") {
      // calc the start_row so that when working backwards it doesn't change each time we loop
      if (start_row < 0) {
        start_row = rows + start_row + 1;
      }
      JSONNode::const_iterator columns = i->begin();
      while (columns != i->end() && rval == true) {
        const JSONNode aCol = *columns;
        rval = SetColumnFromJSON(aCol, start_row, start_cell);
        columns++;
      }
    }
    ++i;
  }
  return rval;
}

bool DataTable::SetColumnFromJSON(const JSONNode& aCol, int start_row, int start_cell) { // row -1 means append, anything else overwrites starting at row
  JSONNode theValues;
  JSONNode theDimensions;
  String columnName("");
  DataCol::ValType columnType = VT_STRING;
  DataCol* dc;
  bool isMatrix = false;
  MatrixGeom mg;

  JSONNode::const_iterator columnData = aCol.begin();
  while (columnData != aCol.end()) {
    std::string node_name = columnData->name();
    if (node_name == "name") {
      columnName = columnData->as_string().c_str();
    }
    else if (node_name == "type") {
      columnType = StrToValType((String)columnData->as_string().c_str());
    }
    else if (node_name == "values") {
      theValues = columnData->as_array();
    }
    else if (node_name == "matrix") {
      isMatrix = columnData->as_bool();
    }
    else if (node_name == "dimensions") {
      theDimensions = columnData->as_array();
    }
    columnData++;
  }
  
  if (theValues.empty()) {
    error_msg = "values empty";
    return false;
  }
  
  // make/find columns & write to the data table
  bool makeNew = true;
  dc = this->data.FindName(columnName);
  if (!dc) {
    if (!isMatrix) {
      dc = this->NewCol(columnType, columnName);
    }
    else {
      if (theDimensions.empty()) {
        error_msg = "matrix dimensions empty";
        return false;
      }

      int colIdx;
      JSONNode::const_iterator dims = theDimensions.begin();
      while (dims != theDimensions.end()) {
        mg.AddDim(dims->as_int());
        dims++;
      }
      dc = this->NewColMatrixN(columnType, columnName, mg, colIdx);
    }
  }
  
  // we have a column (either new or existing)
  isMatrix = dc->is_matrix;
  if (isMatrix) {
    mg = dc->cell_geom;
  }
  columnType = dc->valType();
  
  int row;
  int rowCount = theValues.size();  // row count to write to table

  if (start_row == -1 || start_row > rows) { // means append!
    row = this->rows;
    AddRows(rowCount);
  }
  else if (start_row < 0) {  // consider error for now - later implement from tail
    row = 0;
  }
  else {
    row = start_row;  // the first row to write to
  }

  if (rowCount > (this->rows - row)) { // else overwrite - but make sure we have enough rows
    AddRows(rowCount - (this->rows - row));
  }

  if (!isMatrix) {
    JSONNode::const_iterator values = theValues.begin();
    while (values != theValues.end()) {
      switch (columnType) {
      case VT_STRING:
        dc->SetValAsString(values->as_string().c_str(), row);
        break;
      case VT_DOUBLE:
        dc->SetValAsDouble(values->as_float(), row);
        break;
      case VT_FLOAT:
        dc->SetValAsFloat(values->as_float(), row);
        break;
      case VT_INT:
        dc->SetValAsInt(values->as_int(), row);
        break;
      case VT_BYTE:
        dc->SetValAsInt(values->as_int(), row);
        break;
      case VT_VARIANT:
        dc->SetValAsVar(values->as_string().c_str(), row);
        break;
      default:
        dc->SetValAsString(values->as_string().c_str(), row);
        taMisc::Info("DataTable::ParseJSONColumn -- column type undefined - should not happen");
      }
      row++;
      values++;
    }
  }

  if (isMatrix) {
    int valueCount = 0;  // how many values were passed in
    const JSONNode matrixArray = theValues;
    int_Array intValues;
    float_Array floatValues;
    double_Array doubleValues;
    String_Array stringValues;
    Variant_Array variantValues;
    
    // create a flat array of values
    switch (dc->valType()) {
      case VT_STRING:
        ParseJSONMatrixStringToFlat(matrixArray, stringValues);
        valueCount = stringValues.size;
        break;
      case VT_DOUBLE:
        ParseJSONMatrixDoubleToFlat(matrixArray, doubleValues);
        valueCount = doubleValues.size;
        break;
      case VT_FLOAT:
        ParseJSONMatrixFloatToFlat(matrixArray, floatValues);
        valueCount = floatValues.size;
        break;
      case VT_INT:
        ParseJSONMatrixIntToFlat(matrixArray, intValues);
        valueCount = intValues.size;
        break;
      case VT_BYTE:
        ParseJSONMatrixIntToFlat(matrixArray, intValues);
        valueCount = intValues.size;
        break;
      case VT_VARIANT:
        ParseJSONMatrixVariantToFlat(matrixArray, variantValues);
        valueCount = variantValues.size;
        break;
      default:
        ParseJSONMatrixStringToFlat(matrixArray, stringValues);
        valueCount = stringValues.size;
        taMisc::Info("DataTable::ParseJSONColumn -- column type undefined - should not happen");
    }
    
    
    if (start_cell < 0 || start_cell > mg.Product()) {
      error_msg = "cell range error";
      return false;
    }
    if (valueCount + start_cell > mg.Product()) {
      error_msg = "more values than cells";
      return false;
    }

    // store flat array of values into matrix cells
    for (int i = 0, r = row; i < rowCount; i++, r++) {
      for (int j = 0, k = start_cell; j < valueCount; j++, k++) {
        switch (dc->valType()) {
        case VT_STRING:
          SetValAsStringM(stringValues[i*mg.Product() + j], columnName, r, k);
          break;
        case VT_DOUBLE:
          SetValAsDoubleM(doubleValues[i*mg.Product() + j], columnName, r, k);
          break;
        case VT_FLOAT:
          SetValAsFloatM(floatValues[i*mg.Product() + j], columnName, r, k);
          break;
        case VT_INT:
          SetValAsIntM(intValues[i*mg.Product() + j], columnName, r, k);
          break;
        case VT_BYTE:
          SetValAsIntM(intValues[i*mg.Product() + j], columnName, r, k);
          break;
        case VT_VARIANT:
          SetValAsVarM(variantValues[i*mg.Product() + j], columnName, r, k);
          break;
        default:
          SetValAsVarM(variantValues[i*mg.Product() + j], columnName, r, k);
          taMisc::Info("DataTable::ParseJSONColumn -- column type undefined - should not happen");
        }
      }
    }
  }
  return true;
}

void DataTable::ParseJSONMatrixIntToFlat(const JSONNode& aMatrix, int_Array& values) { // not const we want to fill the array
  JSONNode::const_iterator valueArray = aMatrix.begin();
  while (valueArray != aMatrix.end()) {
    if (valueArray->type() == JSON_ARRAY) {
      ParseJSONMatrixIntToFlat(valueArray->as_array(), values);
    }
    else {
      values.Insert(valueArray->as_int(), -1);
    }
    valueArray++;
  }
}

void DataTable::ParseJSONMatrixDoubleToFlat(const JSONNode& aMatrix, double_Array& values) { // not const we want to fill the array
  JSONNode::const_iterator valueArray = aMatrix.begin();
  while (valueArray != aMatrix.end()) {
    if (valueArray->type() == JSON_ARRAY) {
      ParseJSONMatrixDoubleToFlat(valueArray->as_array(), values);
    }
    else {
      values.Insert(valueArray->as_float(), -1);
    }
    valueArray++;
  }
}

void DataTable::ParseJSONMatrixFloatToFlat(const JSONNode& aMatrix, float_Array& values) { // not const we want to fill the array
  JSONNode::const_iterator valueArray = aMatrix.begin();
  while (valueArray != aMatrix.end()) {
    if (valueArray->type() == JSON_ARRAY) {
      ParseJSONMatrixFloatToFlat(valueArray->as_array(), values);
    }
    else {
      values.Insert(valueArray->as_float(), -1);
    }
    valueArray++;
  }
}

void DataTable::ParseJSONMatrixStringToFlat(const JSONNode& aMatrix, String_Array& values) { // not const we want to fill the array
  JSONNode::const_iterator valueArray = aMatrix.begin();
  while (valueArray != aMatrix.end()) {
    if (valueArray->type() == JSON_ARRAY) {
      ParseJSONMatrixStringToFlat(valueArray->as_array(), values);
    }
    else {
      values.Insert(valueArray->as_string().c_str(), -1);
    }
    valueArray++;
  }
}

void DataTable::ParseJSONMatrixVariantToFlat(const JSONNode& aMatrix, Variant_Array& values) { // not const we want to fill the array
  JSONNode::const_iterator valueArray = aMatrix.begin();
  while (valueArray != aMatrix.end()) {
    if (valueArray->type() == JSON_ARRAY) {
      ParseJSONMatrixVariantToFlat(valueArray->as_array(), values);
    }
    else {
      values.Insert(valueArray->as_string().c_str(), -1);
    }
    valueArray++;
  }
}

void DataTable::LoadAnyData_stream(istream &stream, bool append, bool has_header_line)
{
  if (!stream.good())
  {
    taMisc::Warning("Stream is not good prior to auto-detection.");
  }

  // Get the current stream position so we can reset back to that
  // position after auto-detecting the file format.
  std::ios::streampos cur_stream_pos = stream.tellg();

  // These are passed as out-parameters to an auto-detection function.
  Delimiters delimeter_type = COMMA;
  bool has_quoted_strings = false;
  bool is_native_file = false;

  // Auto-detect file format.
  DetermineLoadDataParams(stream, has_header_line, LD_AUTO, LQ_AUTO,
      has_header_line, delimeter_type, has_quoted_strings, is_native_file);

  if (!stream.good())
  {
    taMisc::Warning("Stream is not good after auto-detection.");
  }

  // Clear any errors in the stream and reset it to the beginning.
  stream.clear();
  stream.seekg(cur_stream_pos);

  if (!stream.good())
  {
    taMisc::Warning("Stream is not good after rewinding.");
  }

  // Clear existing data if append flag wasn't set.
  if (!append)
  {
    RemoveAllRows();
  }

  int max_records = -1; // Get them all.
  if (is_native_file)
  {
    LoadData_strm(stream, delimeter_type, has_quoted_strings, max_records);
  }
  else
  {
    ImportData_strm(
        stream, has_header_line, delimeter_type, has_quoted_strings, max_records);
  }

  // We know we reached EOF, so no point in checking stream.good().
  // When doing character-based input, EOF isn't detected until after
  // reading past the end of the stream, so the failbit is expected
  // to be set.  Thus, only need to verify EOF and badbit states.
  if (!stream.eof())
  {
    taMisc::Warning("Stream did not reach EOF while reading data.");
  }
  if (stream.bad())
  {
    taMisc::Warning("Stream is bad after reading data.");
  }
}

void DataTable::ImportData_strm(istream& strm, bool headers,
    Delimiters delim, bool quote_str, int max_recs)
{
  StructUpdate(true);
  int c = ~EOF; // just set to something we know is not==EOF
  int st_row = rows;
  if (headers) {
    String hdr;
    readline_auto(strm, hdr);   // discard header
  }
  while (c != EOF) {
    c = LoadDataRow_impl(strm, delim, quote_str);
    if ((max_recs > 0) && (rows - st_row >= max_recs)) break;
  }
  StructUpdate(false);
}

void DataTable::DetermineLoadDataParams(istream& strm,
    bool headers_req, LoadDelimiters delim_req, LoadQuotes quote_str_req,
    bool& headers, Delimiters& delim, bool& quote_str, bool& native)
{
  // get first line -- always need this to differentiate Emergent vs. simple
  String ln0;
  readline_auto(strm, ln0);
  String ln1;
  readline_auto(strm, ln1); // need this for non-native column format parsing

  bool native_h = ln0.startsWith("_H:");
  bool native_d = ln0.startsWith("_D:") || ln1.startsWith("_D:");
  native = native_h || native_d;

  // headers is actually same as LH since the Emergent version is auto regardless
  // but we also set if we see the _H: so later quote check knows if 1st line is data
  headers = headers_req || native_h;

  // for Emergent files, the default is TAB and will almost certainly be used
  // but regardless, Emergent files have at least one delim due to marker col

  // note: we guess the delims and quotes so we can warn if override seems wrong
  // if it has TABS or COMMA then almost guaranteed that is delim, else assume SPACE
  // don't use ln1 because it could have a lot of spaces but that doesn't mean they are
  // delimters -- string fields with a lot of spaces CAN happen, and if it does,
  // that totally messes with the results here -- headers never have this problem and
  // SHOULD use the same delimiters as the rest, so let's use them always..

  int tabfreq = ln0.freq('\t');
  int commafreq = ln0.freq(',');
  int spacefreq = ln0.freq(' ');

  if(ln1.nonempty()) {          // commas can occur in _H headers -- use ln1 where possible
    commafreq = ln1.freq(',');
  }

  if(tabfreq >= commafreq && tabfreq >= spacefreq)
    delim = TAB;
  else if (commafreq >= tabfreq && commafreq >= spacefreq)
    delim = COMMA;
  else {
    // note: in rare case of only one col, the delim should NOT default to SPACE!
    if (spacefreq > 0)
      delim = SPACE;
    else
      // must be a simple file with only one col, so default to most likely ','
      delim = COMMA;
  }

  if (delim_req != LD_AUTO) {
    if ((int)delim != (int)delim_req) {
      taMisc::Warning("requested delim and the delim in the file do not seem the same... consider using delim=AUTO");
    }
    delim = (Delimiters)delim_req;
  }

  // determining quotes is a bit trickier...
  // native headers will not have quotes so we have to check data line (default is quoted)
  // imported headers *may* be quoted as well as data
  // OpenOffice puts quotes around everything by default
  // Excel 2004 (Mac) doesn't quote headers, and only quotes data sometimes, ex. when it has a comma
  int quotefreq = 0;
  if(ln1.nonempty()) {                  // rely on delimiter parsing for where to look for quotes
    quotefreq = ln1.freq('"');
  }
  else {
    quotefreq = ln0.freq('"');
  }
  quote_str = (quotefreq > 0);

  if (quote_str_req != LQ_AUTO) {
    // NOTE: this warning may be annoying if quoted string requested but there are no string cols
    if (quote_str != (quote_str_req == LQ_YES)) {
      taMisc::Warning("requested quotes and the quote status in the file do not seem the same... consider using quote_str=AUTO");
    }
    quote_str = (quote_str_req == LQ_YES);
  }

  if(!native) {
    // get headers/columns
    if(!headers)
      ImportHeaderCols(_nilString, ln0, delim, quote_str);
    else
      ImportHeaderCols(ln0, ln1, delim, quote_str);
  }
}

void DataTable::ImportHeaderCols(const String& hdr_ln, const String& dat_ln,
    Delimiters delim, bool quote_str) {
  char cdlm = GetDelim(delim);
  ResetLoadSchema();
  int hc; int dc;
  int hdr_idx = 0;
  int dat_idx = 0;
  int nohdr_col_idx = -1;
  DataCol* last_mat_da = NULL;
  int last_mat_cell = -1;
  bool cont = true;
  bool hdr_got_quote = false;
  bool dat_got_quote = false;
  while(cont) {
    String hstr;
    if(hdr_ln.nonempty())
      hc = ReadTillDelim_Str(hdr_ln, hdr_idx, hstr, cdlm, quote_str, hdr_got_quote);
    String dstr;
    dc = ReadTillDelim_Str(dat_ln, dat_idx, dstr, cdlm, quote_str, dat_got_quote);

    cont = !((dc == '\n') || (dc == '\r') || (dc == EOF));
    if(dstr.empty() && hstr.empty()) {
      if (!cont) break;
      continue;                 // for some reason it is empty
    }

    ValType val_typ = DecodeImportDataType(dstr);
    if(dat_got_quote)
      val_typ = VT_STRING;
    DataCol* da = NULL;
    int col_idx;
    int cell_idx = -1; // mat cell index, or -1 if not a mat
    if(hdr_ln.nonempty()) {
      String base_nm;
      DecodeImportHeaderName(hstr, base_nm, cell_idx); // strips final _<int> leaves base_nm
      col_idx = FindColNameIdx(base_nm);
      if (col_idx >= 0) da = data.FastEl(col_idx);
      // we only accept _xxx as mat col designator if col already a mat, and cell in bounds
      if(!(da && (cell_idx >= 0) && (da->is_matrix) && (cell_idx < da->cell_size()))) {
        da = FindMakeCol(hstr, val_typ);
        col_idx = da->col_idx;
        cell_idx = -1;
      }
    }
    else {
      // just make a dummy header if needed, else use existing
      if(last_mat_da && (++last_mat_cell < last_mat_da->cell_size())) {
        da = last_mat_da;
        col_idx = nohdr_col_idx;
        cell_idx = last_mat_cell;
      }
      else {
        if(++nohdr_col_idx < data.size)
          da = data[nohdr_col_idx];
        else
          da = FindMakeCol("col_" + String(nohdr_col_idx), val_typ);
        if(da->isMatrix()) {
          last_mat_da = da;
          last_mat_cell = 0;
          cell_idx = 0;
        }
        else {
          last_mat_da = NULL;
          last_mat_cell = -1;
          cell_idx = -1;
        }
      }
    }
    if(da->valType() != val_typ) {
      if(da->isNumeric() && val_typ == VT_STRING) {
        taMisc::Warning("Import data for data table:", name, "column:", da->name,
            "is numeric but first row of laded data is string format -- import may be bad.");
      }
      else if((da->isString() && (val_typ != VT_STRING && val_typ != VT_VARIANT))) {
        taMisc::Warning("Import data for data table:", name, "column:", da->name,
            "is String but first row of laded data is a numeric format -- import may be bad.");
      }
    }
    load_col_idx.Add(da->col_idx);
    load_mat_idx.Add(cell_idx);
  }
}

void DataTable::DecodeImportHeaderName(String nm, String& base_nm, int& cell_idx) {
  int pos_und = nm.index('_', -1); // first from end
  // note: must be at least one char before _ otherwise can't be a mat name
  if (pos_und > 0) {
    String sidx = nm.after(pos_und);
    bool ok;
    cell_idx = sidx.toInt(&ok);
    if (ok) {
      base_nm = nm.before(pos_und);
      return;
    }
  }
  base_nm = nm;
  cell_idx = -1;
}

namespace { // anonymous
bool isWhitespace(const char *str)
{
  // Returns true if string is empty or is all whitespace characters.
  if (!str) return false;
  for (; *str; ++str) {
    if (!isspace(*str)) {
      return false;
    }
  }
  return true;
}
}

taBase::ValType DataTable::DecodeImportDataType(const String& dat_str) {
  // Check for empty strings and strings containing only whitespace.
  if (isWhitespace(dat_str.chars())) {
    return VT_VARIANT;
  }

  // Check for quoted strings.
  if (dat_str[0] == '"') {
    return VT_STRING;
  }

  // Test if it's an integer.
  // This will fail if the string is actually a floating point value.
  char *endptr = 0;
  long int li = strtol(dat_str.chars(), &endptr, 0);
  if (isWhitespace(endptr)) return VT_INT;

  // Not an integer, test if it's a floating point value.
  double d = strtod(dat_str.chars(), &endptr);
  if (isWhitespace(endptr)) return VT_DOUBLE;

  // Otherwise a string.
  return VT_STRING;
}

void DataTable::LoadDataFixed(const String& fname, FixedWidthSpec* fws,
    bool reset_first)
{
  if (!fws) return;
  taFiler* flr = GetLoadFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  if(flr->istrm) {
    if(reset_first)
      RemoveAllRows();
    LoadDataFixed_impl(*flr->istrm, fws);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::WriteClose_impl() {
  CalcLastRow();
  UpdateAllViews();
  WriteDataLogRow();
}

//////////////////////////////////////////////////////////////////////////////
///             Calculating columns

bool DataTable::UpdateColCalcs() {
  if(!CheckForCalcs()) return false;
  if(!HasDataFlag(AUTO_CALC)) return false;
  if(taMisc::is_loading) return false;
  return CalcAllRows_impl();
}

bool DataTable::CalcLastRow() {
  if(!HasDataFlag(HAS_CALCS)) return false;
  return CalcRow(-1);
}

bool DataTable::CheckForCalcs() {
  ClearDataFlag(HAS_CALCS);
  for(int i=0;i<data.size; i++) {
    DataCol* da = data.FastEl(i);
    if(da->HasColFlag(DataCol::CALC) && !da->calc_expr.expr.empty()) {
      SetDataFlag(HAS_CALCS);
      break;
    }
  }
  return HasDataFlag(HAS_CALCS);
}

void DataTable::InitCalcScript() {
  if(calc_script) return;
  calc_script = new cssProgSpace;
  cssTA_Base* ths = new cssTA_Base(this, 1, GetTypeDef(), "this");
  ths->InstallThis(calc_script);
}

void DataTable::CalcRowCodeGen(String& code_str) {
  taBase_PtrList calc_rows;
  for(int i=0;i<data.size; i++) {
    DataCol* da = data.FastEl(i);
    if(da->HasColFlag(DataCol::CALC) && !da->calc_expr.expr.empty()) {
      calc_rows.Link(da);
    }
  }
  if(calc_rows.size == 0) return; // shouldn't happen!
  for(int i=0;i<data.size; i++) {
    DataCol* da = data.FastEl(i);
    // only gen if possibly used -- todo: replace with ref-based mech!
    bool is_used = false;
    for(int j=0;j<calc_rows.size; j++) {
      DataCol* cda = (DataCol*)calc_rows.FastEl(j);
      if(cda == da || cda->calc_expr.expr.contains(da->name)) {
        is_used = true;
        break;
      }
    }
    if(!is_used) continue;
    if(da->is_matrix)
      code_str += "taMatrix* " + da->name + " = this.GetValAsMatrix(" +
      String(i) + ", row);\n";
    else
      code_str += "Variant " + da->name + " = this.GetValAsVar(" +
      String(i) + ", row);\n";
  }

  for(int i=0;i<data.size; i++) { // need i to be in data for code gen: not calc_rows!
    DataCol* da = data.FastEl(i);
    if(da->HasColFlag(DataCol::CALC) && !da->calc_expr.expr.empty()) {
      code_str += da->name + " = " + da->calc_expr.GetFullExpr() + ";\n";
      if(da->is_matrix)
        code_str += "this.SetValAsMatrix(" + da->name
        + ", " + String(i) + ", row);\n";
      else
        code_str += "this.SetValAsVar("  + da->name
        + ", " + String(i) + ", row);\n";
    }
  }
  calc_rows.Reset();
}

bool DataTable::CalcRow(int row) {
  InitCalcScript();
  calc_script->ClearAll();

  STRING_BUF(code_str, 2048);
  code_str += "int row = " + String(row) + ";\n";
  CalcRowCodeGen(code_str);
  bool ok = calc_script->CompileCode(code_str);
  if(TestError(!ok, "CalcRow", "error in column calculation, see console for errors"))
    return false;
  calc_script->Run();
  return true;
}

bool DataTable::CalcAllRows_impl() {
  InitCalcScript();
  calc_script->ClearAll();

  STRING_BUF(code_str, 2048);
  code_str += "for(int row=0;row <this.rows; row++) {\n";
  CalcRowCodeGen(code_str);
  code_str += "}\n";
  bool ok = calc_script->CompileCode(code_str);
  if(TestError(!ok, "CalcAllRows_impl", "error in column calculation, see console for errors"))
    return false;
  calc_script->Run();
  return true;
}

bool DataTable::CalcAllRows() {
  bool rval = CalcAllRows_impl();
  UpdateAfterEdit();
  return rval;
}

/////////////////////////////////////////////////////////
// core data processing -- see taDataProc for more elaborate options

void DataTable::Sort(const Variant& col1, bool ascending1,
    Variant col2, bool ascending2,
    Variant col3, bool ascending3,
    Variant col4, bool ascending4,
    Variant col5, bool ascending5,
    Variant col6, bool ascending6) {

  DataSortSpec spec;
  if(col1.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col1);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      if(ascending1) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
      sp->SetColName(da->name);
    }
  }
  if(col2.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col2);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      if(ascending2) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
      sp->SetColName(da->name);
    }
  }
  if(col3.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col3);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      if(ascending3) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
      sp->SetColName(da->name);
    }
  }
  if(col4.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col4);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      if(ascending4) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
      sp->SetColName(da->name);
    }
  }
  if(col5.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col5);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      if(ascending5) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
      sp->SetColName(da->name);
    }
  }
  if(col6.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col6);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      if(ascending6) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
      sp->SetColName(da->name);
    }
  }
  taDataProc::SortThruIndex(this, &spec);
}

void DataTable::SortColName(const String& col1, bool ascending1,
    const String& col2, bool ascending2,
    const String& col3, bool ascending3,
    const String& col4, bool ascending4,
    const String& col5, bool ascending5,
    const String& col6, bool ascending6) {

  DataSortSpec spec;
  if(col1.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending1) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col1);
  }
  if(col2.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending2) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col2);
  }
  if(col3.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending3) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col3);
  }
  if(col4.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending4) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col4);
  }
  if(col5.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending5) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col5);
  }
  if(col6.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending6) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col6);
  }
  taDataProc::SortThruIndex(this, &spec);
}

void DataTable::SortCol(DataCol* col1, bool ascending1,
    DataCol* col2, bool ascending2,
    DataCol* col3, bool ascending3,
    DataCol* col4, bool ascending4,
    DataCol* col5, bool ascending5,
    DataCol* col6, bool ascending6) {

  DataSortSpec spec;
  if(col1) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending1) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col1->name);
  }
  if(col2) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending2) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col2->name);
  }
  if(col3) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending3) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col3->name);
  }
  if(col4) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending4) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col4->name);
  }
  if(col5) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending5) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col5->name);
  }
  if(col6) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    if(ascending6) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
    sp->SetColName(col6->name);
  }
  taDataProc::SortThruIndex(this, &spec);
}

void DataTable::Filter(Variant& col1, Relation::Relations operator_1,
    const String& value_1, Relation::CombOp comb_op,
    Variant col2, Relation::Relations operator_2,
    const String& value_2,
    Variant col3, Relation::Relations operator_3,
    const String& value_3) {
  DataSelectSpec* select_spec = new DataSelectSpec; taBase::Ref(select_spec);
  select_spec->comb_op = comb_op;
  if(col1.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col1);
    if(da) {
      DataSelectEl* select_el_1 = (DataSelectEl*)select_spec->AddColumn(da->name, this);
      if(da->isNumeric())
        select_el_1->cmp = (double)value_1;
      else
        select_el_1->cmp = value_1;
      select_el_1->rel = operator_1;
    }
  }
  if(col2.isStringType() || col2.toInt() >= 0) {
    DataCol* da = GetColData(col2);
    if(da) {
      DataSelectEl* select_el_2 = (DataSelectEl*)select_spec->AddColumn(da->name, this);
      if(da->isNumeric())
        select_el_2->cmp = (double)value_2;
      else
        select_el_2->cmp = value_2;
      select_el_2->rel = operator_2;
    }
  }
  if(col3.isStringType() || col3.toInt() >= 0) {
    DataCol* da = GetColData(col3);
    if(da) {
      DataSelectEl* select_el_3 = (DataSelectEl*)select_spec->AddColumn(da->name, this);
      if(da->isNumeric())
        select_el_3->cmp = (double)value_3;
      else
        select_el_3->cmp = value_3;
      select_el_3->rel = operator_3;
    }
  }
  this->FilterBySpec(select_spec);
  taBase::unRefDone(select_spec);
}

void DataTable::FilterCol(DataCol* col1, Relation::Relations operator_1,
    const String& value_1, Relation::CombOp comb_op,
    DataCol* col2, Relation::Relations operator_2,
    const String& value_2,
    DataCol* col3, Relation::Relations operator_3,
    const String& value_3) {
  if(TestError(!col1, "Filter",
      "column 1 is NULL-- must have at least one column"))
    return;
  DataSelectSpec* select_spec = new DataSelectSpec; taBase::Ref(select_spec);
  if(col1) {
    DataSelectEl* select_el_1 = (DataSelectEl*)select_spec->AddColumn(col1->name, this);
    if(col1->isNumeric())
      select_el_1->cmp = (double)value_1;
    else
      select_el_1->cmp = value_1;
    select_el_1->rel = operator_1;
    select_spec->comb_op = comb_op;
  }
  if (col2 != NULL) {
    DataSelectEl* select_el_2 = (DataSelectEl*)select_spec->AddColumn(col2->name, this);
    if(col2->isNumeric())
      select_el_2->cmp = (double)value_2;
    else
      select_el_2->cmp = value_2;
    select_el_2->rel = operator_2;
  }
  if (col3 != NULL) {
    DataSelectEl* select_el_3 = (DataSelectEl*)select_spec->AddColumn(col3->name, this);
    if(col3->isNumeric())
      select_el_3->cmp = (double)value_3;
    else
      select_el_3->cmp = value_3;
    select_el_3->rel = operator_3;
  }
  this->FilterBySpec(select_spec);
  taBase::unRefDone(select_spec);
}

bool DataTable::FilterByScript(const String& filter_expr) {
  if(TestError(filter_expr.empty(), "Filter",
      "empty filter expression -- must specify a filter condition!"))
    return false;
  InitCalcScript();
  calc_script->ClearAll();

  STRING_BUF(code_str, 2048);
  code_str += "this.DataUpdate(true);\n";
  code_str += "for(int row=this.rows-1;row >= 0; row--) {\n";
  for(int i=0;i<data.size; i++) {
    DataCol* da = data.FastEl(i);
    // only gen if possibly used
    if(!filter_expr.contains(da->name)) continue;
    if(da->is_matrix)
      code_str += "taMatrix* " + da->name + " = this.GetValAsMatrix(" +
      String(i) + ", row);\n";
    else
      code_str += "Variant " + da->name + " = this.GetValAsVar(" +
      String(i) + ", row);\n";
  }

  code_str += "if(" + filter_expr + ") continue;\n"; // if ok, continue
  code_str += "this.RemoveRows(row,1);\n";                   // else remove
  code_str += "}\n";
  code_str += "this.DataUpdate(false);\n";

  bool ok = calc_script->CompileCode(code_str);
  if(TestError(!ok, "Filter", "error in filter expression, see console for errors"))
    return false;
  calc_script->Run();
  return true;
}

bool DataTable::FilterBySpec(DataSelectSpec* spec) {
  return taDataProc::SelectRows(this, this, spec);
}

void DataTable::CompareRows(int st_row, int n_rows) {
  if (n_rows > 1) {
    DataUpdate(true);
    ClearCompareRows();
    base_diff_row = st_row;
    for (int i = st_row + 1; i < st_row+n_rows; i++) {
      diff_row_list.Add(i);
    }
    DataUpdate(false);
  }
}

void DataTable::ClearCompareRows() {
  DataUpdate(true);
  base_diff_row = -1;
  diff_row_list.Reset();
  DataUpdate(false);
}

bool DataTable::CompareRowsState() {
  return (base_diff_row != -1);  // if base_diff_rows == -1 we are not comparing - true means comparing
}

bool DataTable::GroupMeanSEM(DataTable* dest_data, DataCol* col1,
    DataCol* col2, DataCol* col3, DataCol* col4) {
  DataGroupSpec spec;
  if(col1) {
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->agg.op = Aggregate::GROUP;
    sp->SetColName(col1->name);
  }
  if(col2) {
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->agg.op = Aggregate::GROUP;
    sp->SetColName(col2->name);
  }
  if(col3) {
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->agg.op = Aggregate::GROUP;
    sp->SetColName(col3->name);
  }
  if(col4) {
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->agg.op = Aggregate::GROUP;
    sp->SetColName(col4->name);
  }
  for(int i=0;i<data.size;i++) {
    DataCol* da = data.FastEl(i);
    if(da == col1 || da == col2 || da == col3 || da == col4) continue;
    if(!da->isNumeric()) continue;
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->agg.op = Aggregate::MEAN;
    sp->SetColName(da->name);
    sp = (DataGroupEl*)spec.ops.New(1);
    sp->agg.op = Aggregate::SEM;
    sp->SetColName(da->name);
  }
  return taDataProc::Group(dest_data, this, &spec);
}

String DataTable::ColStats(const Variant& col) {
  DataCol* cda = GetColData(col);
  if (!cda) return _nilString;
  return cda->ColStats();
}


String DataTable::ColStatsCol(DataCol* col) {
  if(TestError(!col, "ColStats", "column is null")) return _nilString;
  return col->ColStats();
}

String DataTable::ColStatsName(const String& col_nm) {
  DataCol* cda = FindColName(col_nm, true);
  if (!cda) return _nilString;
  return cda->ColStats();
}

void DataTable::PermuteRows() {
  StructUpdate(true);
  row_indexes.Permute();
  StructUpdate(false);
}

bool DataTable::MatrixColToScalars(const Variant& mtx_col, const String& scalar_col_name_stub) {
  return MatrixColToScalarsCol(GetColData(mtx_col), scalar_col_name_stub);
}

bool DataTable::MatrixColToScalarsCol(DataCol* da, const String& scalar_col_name_stub) {
  if(!da || da->not_matrix_err()) return false;
  String clstub;
  if(scalar_col_name_stub.nonempty())
    clstub = scalar_col_name_stub;
  else
    clstub = da->name;
  clstub += "_";
  int cls = da->cell_size();
  StructUpdate(true);
  for(int i=0;i<cls;i++) {
    DataCol* scda = FindMakeCol(clstub + String(i), da->valType());
    for(int j=0;j<rows;j++) {
      scda->SetVal(da->GetMatrixFlatVal(j, i), j);
    }
  }
  StructUpdate(false);
  return true;
}

bool DataTable::MatrixColFmScalars(const Variant& mtx_col, const String& scalar_col_name_stub) {
  return MatrixColFmScalarsCol(GetColData(mtx_col), scalar_col_name_stub);
}

bool DataTable::MatrixColFmScalarsCol(DataCol* da, const String& scalar_col_name_stub) {
  if(!da || da->not_matrix_err()) return false;
  StructUpdate(true);
  if(scalar_col_name_stub.nonempty()) {
    String clstub = scalar_col_name_stub;
    clstub += "_";
    int cls = da->cell_size();
    for(int i=0;i<cls;i++) {
      DataCol* scda = FindColName(clstub + String(i));
      if(scda) {
        for(int j=0;j<rows;j++) {
          Variant var = scda->GetVal(j);
          da->SetMatrixFlatVal(var, j, i);
        }
      }
    }
  }
  else {
    int cls = da->cell_size();
    int cur_idx = 0;
    for(int i=0;i<data.size;i++) {
      DataCol* scda = data[i];
      if(scda->isMatrix() || scda->valType() != da->valType()) continue;
      for(int j=0;j<rows;j++) {
        Variant var = scda->GetVal(j);
        da->SetMatrixFlatVal(var, j, cur_idx);
      }
      cur_idx++;
      if(cur_idx >= cls) break; // all done
    }
  }
  StructUpdate(false);
  return true;
}

bool DataTable::SplitStringToCols(const Variant& string_col,
    const String& delim,
    const String& col_name_stub) {
  return SplitStringToColsCol(GetColData(string_col), delim, col_name_stub);
}

bool DataTable::SplitStringToColsCol(DataCol* da, 
    const String& delim,
    const String& col_name_stub) {
  if(TestError(!da || !da->isString(), "SplitStringToCols",
      "column is NULL or is not a String type")) {
    return false;
  }
  if(rows <= 0) return false;
  String clstub;
  if(col_name_stub.nonempty())
    clstub = col_name_stub;
  else
    clstub = da->name;
  clstub += "_";
  String_Array ar;
  String fc = da->GetValAsString(0);
  ar.FmDelimString(fc, delim);
  if(TestError(ar.size <= 1, "SplitStringToCols",
      "first row string value:", fc,
      "does not contain multiple substrings delimted by:", delim)) {
    return false;
  }

  int cls = ar.size;
  StructUpdate(true);
  // make the cols first
  for(int i=0;i<cls;i++) {
    DataCol* scda = FindMakeCol(clstub + String(i), da->valType());
  }
  for(int j=0;j<rows;j++) {
    String sc = da->GetValAsString(j);
    ar.FmDelimString(sc, delim);
    int mx = MIN(cls, ar.size);
    for(int i=0;i<mx;i++) {
      DataCol* scda = FindMakeCol(clstub + String(i), da->valType());
      scda->SetVal(ar[i], j);
    }
  }
  StructUpdate(false);
  return true;
}

////////////////////////////////////////////////////////////////////////////
//              DMEM

void DataTable::DMem_ShareRows(MPI_Comm comm, int n_rows) {
#ifdef DMEM_COMPILE
  if(rows == 0) return;
  if(n_rows < 1) n_rows = rows;
  if(n_rows > rows) n_rows = rows;
  int np = 0; MPI_Comm_size(comm, &np);
  int this_proc = 0; MPI_Comm_rank(comm, &this_proc);
  if(np <= 1) return;

  DataUpdate(true);

  int st_send_row_logical = rows - n_rows;
  int st_send_row = row_indexes.SafeEl(st_send_row_logical); // de-index -- assume contigous!
  int st_recv_row = rows_total; // raw memory at end
  int n_recv_rows = np * n_rows;
  AddRows(n_recv_rows);         // make room for new ones

  static char_Array char_send;
  static char_Array char_recv;
  const int max_str_len = 1024;

  for(int i=0;i<data.size; i++) {
    DataCol* da = data.FastEl(i);
    taMatrix* da_mat = da->AR();

    int frsz = da_mat->FrameSize();
    int send_idx = st_send_row * frsz;
    int n_send = n_rows * frsz;
    int recv_idx = st_recv_row * frsz;

    switch(da->valType()) {
    case VT_FLOAT:
      DMEM_MPICALL(MPI_Allgather(&(((float_Matrix*)da_mat)->el[send_idx]), n_send, MPI_FLOAT,
          &(((float_Matrix*)da_mat)->el[recv_idx]), n_send, MPI_FLOAT,
          comm), "DataTable::DMem_ShareRows", "Allgather");
      break;
    case VT_DOUBLE:
      DMEM_MPICALL(MPI_Allgather(&(((double_Matrix*)da_mat)->el[send_idx]), n_send, MPI_DOUBLE,
          &(((double_Matrix*)da_mat)->el[recv_idx]), n_send, MPI_DOUBLE,
          comm), "DataTable::DMem_ShareRows", "Allgather");
      break;
    case VT_INT:
      DMEM_MPICALL(MPI_Allgather(&(((int_Matrix*)da_mat)->el[send_idx]), n_send, MPI_INT,
          &(((int_Matrix*)da_mat)->el[recv_idx]), n_send, MPI_INT,
          comm), "DataTable::DMem_ShareRows", "Allgather");
      break;
    case VT_BYTE:
      DMEM_MPICALL(MPI_Allgather(&(((byte_Matrix*)da_mat)->el[send_idx]), n_send, MPI_BYTE,
          &(((byte_Matrix*)da_mat)->el[recv_idx]), n_send, MPI_BYTE,
          comm), "DataTable::DMem_ShareRows", "Allgather");
      break;
    case VT_STRING: {
      int n_recv = n_recv_rows * frsz;
      char_send.SetSize(n_send * max_str_len);
      char_recv.SetSize(n_recv * max_str_len);
      for(int i=0;i<n_send;i++) {
        String& str = ((String_Matrix*)da_mat)->FastEl_Flat(send_idx + i);
        int st_idx = i * max_str_len;
        int mxln = MIN(str.length(), max_str_len-1);
        int j;
        for(j=0;j<mxln;j++) {
          char_send[st_idx + j] = str[j];
        }
        char_send[st_idx + j] = '\0';
      }
      DMEM_MPICALL(MPI_Allgather(char_send.el, n_send * max_str_len, MPI_CHAR,
          char_recv.el, n_send * max_str_len, MPI_CHAR,
          comm), "DataTable::DMem_ShareRows", "Allgather");
      for(int i=0;i<n_recv;i++) {
        String& str = ((String_Matrix*)da_mat)->FastEl_Flat(recv_idx + i);
        int st_idx = i * max_str_len;
        str = &(char_recv[st_idx]);
      }
      break;
    }
    }
    // todo: deal with variant?
  }

  // remove sending rows -- they were all received already!
  RemoveRows(st_send_row_logical, n_rows);
  DataUpdate(false);
#endif  // DMEM_COMPILE
}

bool DataTable::idx(int row_num, int& act_idx) const {
  act_idx = row_num;
  if (act_idx < 0)
    act_idx = rows + act_idx;
  if(act_idx < 0 || act_idx >= row_indexes.size) return false;
  return true;
}

bool DataTable::RunAnalysis(DataCol* column, AnalysisRun::AnalysisType type) {
  bool rval = false;

  AnalysisRun analysis;
  DataTable* result_data_table = NULL;
  DataTable_Group* group = NULL;
  ProgObjList* objList = NULL;

  if (analysis.RequiresResultsTable(type)) {
    // create a table for the analysis results

    group = GET_OWNER(this, DataTable_Group);
    if (group != NULL) { // The table belongs to the data section of the project
      taProject* proj = GET_OWNER(this, taProject);
      group = (DataTable_Group*)proj->data.FindMakeGpName("AnalysisData");
      result_data_table = group->NewEl(1, NULL);   // add a new data table to the group
    }
    else {  // not in a datatable group so must be an object in a program
      Program* program = NULL;
      program = GET_OWNER(this, Program);
      objList = &program->objs;
      result_data_table = objList->NewDataTable();   // add a new data table to the object group of the program
    }
  }

  rval = analysis.Init(type, this, column->name, result_data_table);
  if (rval) {
    rval = analysis.Run();
  }
  if (rval == false) {
    if (group != NULL) {
      group->RemoveEl(result_data_table);
    }
    else if (objList != NULL) {
      objList->RemoveEl(result_data_table);
    }
  }
  return rval;
}

taBase* DataTable::ChooseNew(taBase* origin) {
  // location of new DataTable can be the current program, if one, and any subgroup of project data tables
  if (origin == NULL)
    return NULL;
  
  taProject* prj = GET_OWNER(origin, taProject);  // who initiated the choice/new datatable call?
  Program* pgrm = GET_OWNER(origin, Program);     // who initiated the choice/new datatable call?
  DataTable_Group root_group = prj->data;  // top level

  // create a string with the names of all subgroups
  String delimiter = iDialogChoice::delimiter;
  String chstr = delimiter;
  taGroup_List* sub_groups = root_group.EditSubGps();
  int n_sub_groups = sub_groups->size;
  for (int i=0; i<n_sub_groups; i++) {
    chstr += root_group.FastGp(i)->name + delimiter;
  }
  // add program to string of choices
  //  Program* pgrm = GET_OWNER(origin, Program);
  if (pgrm) {
    String str = "Program " + pgrm->name;
    chstr += str;
  }
  // should we add a cancel option?
  int chs = iDialogChoice::ChoiceDialog(NULL, "Create new DataTable in:", chstr);

  DataTable* dt = NULL;
  if (chs < n_sub_groups) { // one of the DataTable groups
    DataTable_Group* grp = (DataTable_Group*)prj->data.FindMakeGpName(root_group.FastGp(chs)->name);
    dt = grp->NewEl(1, NULL);   // add a new data table to the group
  }
  else {
    if (pgrm) {
      ProgObjList* objList = &pgrm->objs;
      tabMisc::DelayedFunCall_gui(objList, "OneNewTable");
      // note: can't do a new call here because we're being called from same guy
      // so we can't trample the editor in place
      //      dt = (DataTable*)objList->New(1, &TA_DataTable);
    }
  }

  return dt;
}
