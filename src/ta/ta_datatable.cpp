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

#include "ta_datatable.h"

#include <limits.h>
#include <float.h>
#include <ctype.h>

#include <QColor>

//////////////////////////
//   DataArray_impl	//
//////////////////////////

const String DataArray_impl::udkey_width("WIDTH");
const String DataArray_impl::udkey_narrow("NARROW");
const String DataArray_impl::udkey_hidden("HIDDEN");

void DataArray_impl::DecodeName(String nm, String& base_nm, int& vt, int& vec_col, int& col_cnt) {
  base_nm = nm;
  vt = -1; // unknown
  vec_col = -1;
  if (nm.empty()) return;
  // check type character, float has none
  if (nm[0] == '|') {
    nm = nm.after('|');
    vt = VT_INT;
  } else if (nm[0] == '$') {
    nm = nm.after('$');
    vt = VT_STRING;
  } else {
    vt = VT_FLOAT;
  }
  base_nm = nm;
  if (nm.empty()) return;
  // check if a vec start
  if (nm[0] == '<') {
    nm = nm.after('<');
    col_cnt = (int)nm.before('>');
    base_nm = nm.after('>');
    vec_col = 0;
    return;
  }
  // check if a non-col0 vector column -- NOTE: this is not 100% bulletproof
  String col_str = nm;
  if (col_str.contains('_')) { // may be vec column
    while (col_str.contains('_'))
      col_str = col_str.after('_');
    if (col_str.isInt()) {
      vec_col = (int)col_str;
      return;
    }
  }
}

void DataArray_impl::Initialize() {
  mark = false;
  pin = false;
  is_matrix = false;
  // default initialize to scalar
  cell_geom.EnforceSize(1);
  cell_geom.Set(0, 1);
}

void DataArray_impl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(cell_geom, this);
  taMatrix* ar = AR();
  if (ar != NULL)
    taBase::Own(ar, this);
}

void DataArray_impl::CutLinks() {
  cell_geom.CutLinks();
  inherited::CutLinks();
}

void DataArray_impl::Copy_(const DataArray_impl& cp) {
  disp_opts = cp.disp_opts;
  mark = cp.mark;
  pin = cp.pin;
  is_matrix = cp.is_matrix;
  cell_geom = cp.cell_geom;
}

void DataArray_impl::Copy_NoData(const DataArray_impl& cp) {
  taNBase::Copy(cp);
  Copy_(cp);
  Init();			// initialize array to new geom
}

void DataArray_impl::CopyFromRow(int dest_row, const DataArray_impl& src, int src_row) {
  if(src.is_matrix) {
    taMatrix* mat = ((DataArray_impl&)src).GetValAsMatrix(src_row);
    taBase::Ref(mat);
    SetValAsMatrix(mat, dest_row);
    taBase::unRefDone(mat);
  }
  else {
    SetValAsVar(src.GetValAsVar(src_row), dest_row);
  }
}

void DataArray_impl::Init() {
  taMatrix* ar = AR(); //cache
  if (is_matrix) {
    MatrixGeom tdim = cell_geom;
    tdim.EnforceSize(tdim.size + 1); // leaves the new outer dim = 0, which is flex sizing
    ar->SetGeomN(tdim);
  } else {
    ar->SetGeom(1, 0); // sets to 1-d, with flex sizing
  }
}

void DataArray_impl::UpdateAfterEdit() {
  //TODO: is_matrix needs to be true if framesize > 1
  // TODO: check if frame size has changed (ie, dim change, or matrix change)
  //TEMP: maybe this is enough???
  Init();
  inherited::UpdateAfterEdit();
}

DataTable* DataArray_impl::dataTable() {
  DataTable* rval = GET_MY_OWNER(DataTable);
  return rval;
}

void DataArray_impl::EnforceRows(int rws) {
  taMatrix* mat = AR();
  if (!mat) return;
  mat->EnforceFrames(rws);
}

int DataArray_impl::displayWidth() const {
  // explicit width has highest priority
  int rval = GetUserData(udkey_width).toInt();
  if (rval == 0) {
    // NARROW implies width of 8
     if (GetUserData(udkey_narrow).toBool())
       rval = 8;
  }
  if (rval == 0)
    rval = 16; // default
  return rval;
}

bool DataArray_impl::Dump_QuerySaveMember(MemberDef* md) {
  if (md->name == "ar") {
    // if no save, don't need to check DataTable global
    if (!saveToFile()) return false;
    DataTable* dt = dataTable();
    if (dt)
      return dt->save_data;
    else return true;
  } else return inherited::Dump_QuerySaveMember(md);
}

const KeyString DataArray_impl::key_val_type("val_type");
const KeyString DataArray_impl::key_disp_opts("disp_opts");

String DataArray_impl::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_name) return GetDisplayName(); // override
  if (key == key_val_type) return ValTypeToStr(valType());
  if (key == key_disp_opts) return disp_opts;
  else return inherited::GetColText(key, itm_idx);
}

String DataArray_impl::GetDisplayName() const {
  String rval; int vt; int vec_col; int col_cnt;
  DecodeName(name, rval, vt, vec_col, col_cnt);
  return rval;
}

taMatrix* DataArray_impl::GetValAsMatrix(int row) {
  taMatrix* ar = AR(); 
  return ar->GetFrameSlice_(row);
}

taMatrix* DataArray_impl::GetRangeAsMatrix(int st_row, int n_rows) {
  taMatrix* ar = AR();
  return ar->GetFrameRangeSlice_(st_row, n_rows);
}

const String DataArray_impl::GetValAsString_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsStr_Flat(IndexOfEl_Flat(row, cell));
} 

const Variant DataArray_impl::GetValAsVar_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsVar_Flat(IndexOfEl_Flat(row, cell));
} 

int DataArray_impl::IndexOfEl_Flat(int row, int cell) const {
  if ((cell < 0) || (cell >= cell_size())) return -1;
  if (row < 0) row = rows() + row; // abs row, if request was from end
  if (row < 0) return -1;
  return (row * cell_size()) + cell;
} 

bool DataArray_impl::SetValAsMatrix(const taMatrix* val, int row) {
  if (!val) return false;
  //note: the mat function does most of the parameter checking
  return AR()->CopyFrame(*val, row);
}


bool DataArray_impl::SetValAsString_impl(const String& val, int row, int cell) {
  AR()->SetFmStr_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
} 

bool DataArray_impl::SetValAsVar_impl(const Variant& val, int row, int cell) {
  AR()->SetFmVar_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
} 




//////////////////////////
//  DataTableCols	//
//////////////////////////

void DataTableCols::Initialize() {
  SetBaseType(&TA_DataArray);
}

void DataTableCols::Copy_NoData(const DataTableCols& cp) {
  Reset();	// get rid of ours
  for(int i=0;i<cp.size; i++) {
    DataArray_impl* sda = cp.FastEl(i);
    DataArray_impl* nda = (DataArray_impl*)sda->MakeToken();
    Add(nda);
    nda->Copy_NoData(*sda);
  }
}

void DataTableCols::CopyFromRow(int dest_row, const DataTableCols& src, int src_row) {
  taLeafItr di, si;
  DataArray_impl* dar, *sar;
  for(dar = FirstEl(di), sar = src.FirstEl(si);
      dar && sar;
      dar = NextEl(di), sar = src.NextEl(si)) {
    dar->CopyFromRow(dest_row, *sar, src_row);
  }  
}

void DataTableCols::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  // we only do the notifies in the context of the root group, so we exit if we aren't root
  if (!IsRoot()) return;

  // for most schema changes, we just call the datamodel layoutChanged
  // note that groups themselves don't show in the datatable editor
  if ((dcr >= DCR_LIST_MIN) && (dcr <= DCR_GROUP_ITEM_MAX)) {
    DataTable* dt = GET_MY_OWNER(DataTable);
    if (!dt || !dt->m_dm) return;
/*wrong    // it seems to handle Adds and Moves ok, but Deletes leave it
    // with the same number of cols, unless we also send a removeColumn notify
    if (dcr == DCR_GROUP_ITEM_REMOVE) {
      // we just tell it the last col is being deleted, since it will refresh all
      // in the layoutChanged notify
      int col = dt->cols() - 1;
      dt->m_dm->beginRemoveColumns(QModelIndex(), col, col);
      dt->m_dm->endRemoveColumns();
    }*/
    dt->m_dm->emit_layoutChanged();
  }
  
}

String DataTableCols::GetColHeading(const KeyString& key) const {
  if (key == key_name) return "Col Name"; // override
  else if (key == DataArray_impl::key_val_type) return "Data Type";
  else if (key == DataArray_impl::key_disp_opts) return "Disp Opts";
  else return inherited::GetColHeading(key);
}

const KeyString DataTableCols::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_name;
  case 1: return DataArray_impl::key_val_type;
  case 2: return DataArray_impl::key_disp_opts;
  default: return _nilKeyString;
  }
}


//////////////////////////
//	DataTable	//
//////////////////////////

void DataTable::Initialize() {
  rows = 0;
  save_data = true;
  m_dm = NULL; // returns new if none exists, or existing -- enables views to be shared
}

void DataTable::Destroy() {
  if (m_dm) {
   delete m_dm;
   m_dm = NULL;
  }
  CutLinks();
}

void DataTable::InitLinks() {
  data.name = "data"; // for the viewspec routines
  inherited::InitLinks();
  taBase::Own(data, this);
}

void DataTable::CutLinks() {
  data.CutLinks();
  inherited::CutLinks();
}

void DataTable::Copy_(const DataTable& cp) {
  data = cp.data;
  rows = cp.rows;
  save_data = cp.save_data;
}

void DataTable::Copy_NoData(const DataTable& cp) {
  rows = 0;
  save_data = cp.save_data;
  data.Copy_NoData(cp.data);
}

void DataTable::CopyFromRow(int dest_row, const DataTable& src, int src_row) {
  data.CopyFromRow(dest_row, src.data, src_row);
}

bool DataTable::AddRow(int n) {
  if ((cols() == 0) || (n < 1)) return false;
  RowsAdding(n, true);
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    ar->EnforceRows(ar->rows() + n);
  }
  RowsAdding(n, false);
  return true;
}

bool DataTable::AddSinkChannel(ChannelSpec* cs) {
  if (!cs) return false;
  DataArray_impl* da = NewColFromChannelSpec_impl(cs);
  return (da);
}

void DataTable::AllocRows(int n) {
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    ar->AR()->AllocFrames(n); //noop if already has more alloc'ed
  }
}

bool DataTable::AssertSinkChannel(ChannelSpec* cs) {
  if (!cs) return false;
  DataArray_impl* da = GetColForChannelSpec_impl(cs);
  return (da);
}

bool DataTable::ColMatchesChannelSpec(const DataArray_impl* da, const ChannelSpec* cs) {
  if (!da && !cs) return false;
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

int DataTable::Dump_Load_Value(istream& strm, TAPtr par) {
  int c = inherited::Dump_Load_Value(strm, par);
  if (c == EOF) return EOF;
  if (c == 2) return 2; // signal that it was just a path
  // otherwise, if data was loaded, we need to set the rows
  if (save_data) {
    int i;
    DataArray_impl* col;
    for (i = 0; i < cols(); ++i) {
      col = GetColData(i);
      if (!col->saveToFile()) continue;
      int frms = col->AR()->frames();
      // number of rows is going to be = biggest number in individual cols
      rows = max(rows, frms);
    }
  }
  return c;
}

DataArray_impl* DataTable::GetColData(int col) const {
  if (col >= cols()) return NULL;
  else return data.Leaf(col);
}

DataArray_impl* DataTable::GetColForChannelSpec_impl(ChannelSpec* cs) {
  DataArray_impl* rval;
  taLeafItr itr;
  cs->chan_num = -1; // we incr at beginning of loop
  FOR_ITR_EL(DataArray_impl, rval, data., itr) {
    ++(cs->chan_num);
    if (rval->name != cs->name) continue;
    // if name matches, but not contents, we need to remake it...
    if (ColMatchesChannelSpec(rval, cs)) {
      rval->mark = false; // reset mark for orphan tracking
      return rval;
    } else {
      rval->Close();
    }
  }
  rval = NewColFromChannelSpec_impl(cs);
  return rval;
}

taMatrix* DataTable::GetColMatrix(int col) const {
  DataArray_impl* da = GetColData(col);
  if (da) return da->AR();
  else return NULL;
}

const Variant DataTable::GetColUserData(const String& name,
  int col) const 
{
  DataArray_impl* da = GetColData(col);
  if (da) return da->GetUserData(name);
  else return _nilVariant;
}

void DataTable::SetColUserData(const String& name,
  const Variant& value, int col)
{
  DataArray_impl* da = GetColData(col);
  if (da) da->SetUserData(name, value);
}


DataTableModel* DataTable::GetDataModel() {
  if (!m_dm) {
    m_dm = new DataTableModel(this);
  }
  return m_dm;
}

taMatrix* DataTable::GetMatrixData_impl(int chan) {
  DataArray_impl* da = GetColData(chan);
  int i;
  if (!da || !da->is_matrix ||
    !idx(rd_itr, da->rows(), i)) return NULL;
  return da->AR()->GetFrameSlice_(i);
}

double DataTable::GetValAsDouble(int col, int row) {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsDouble(i);
  else return 0.0f;
}

float DataTable::GetValAsFloat(int col, int row) {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsFloat(i);
  else return 0.0f;
}

taMatrix* DataTable::GetValAsMatrix(int col, int row) {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsMatrix(i);
  else return NULL;
}

taMatrix* DataTable::GetRangeAsMatrix(int col, int st_row, int n_rows) {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(st_row, da->rows(), i))
    return da->GetRangeAsMatrix(i, n_rows);
  else return NULL;
}

const String DataTable::GetValAsString(int col, int row) const {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsString(i);
  else return "n/a";
}

const Variant DataTable::GetValAsVar(int col, int row) const {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsVar(i);
  else return _nilVariant;
}

bool DataTable::hasData(int col, int row) {
  DataArray_impl* da = GetColData(col);
  int i;
  return (da && idx(row, da->rows(), i));
}


void DataTable::MarkCols() {
  DataArray_impl* da;
  taLeafItr itr;
  FOR_ITR_EL(DataArray_impl, da, data., itr) {
    da->mark = true;
  }
}

int DataTable::MaxLength() {
  return rows;
}
/*obs int DataTable::MaxLength() {
  int max = 0;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    max = MAX(max,ar->AR()->frames());
  }
  return max;
} */

int DataTable::MinLength() {
  if (cols() == 0) return 0;
  int min = INT_MAX;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    min = MIN(min,ar->AR()->frames());
  }
  return min;
}

DataArray_impl* DataTable::NewCol(DataArray_impl::ValType val_type, 
  const String& col_nm, DataTableCols* col_gp) 
{
  if (!col_gp) col_gp = &data;
  StructUpdate(true);
  DataArray_impl* rval = NewCol_impl(val_type, col_nm);
  rval->Init(); // asserts geom
  rval->EnforceRows(rows);	// new guys always get same # of rows as current table
  StructUpdate(false);
  return rval;
}

DataArray_impl* DataTable::NewCol_impl(DataArray_impl::ValType val_type, 
  const String& col_nm, DataTableCols* col_gp) 
{
  if (!col_gp) col_gp = &data;
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
  DataArray_impl* rval = (DataArray_impl*) col_gp->NewEl(1, td);
  rval->name = col_nm;
  // additional specialized initialization
  switch (val_type) {
  case VT_STRING: 
    break;
  case VT_FLOAT: 
  case VT_DOUBLE:
    break;
  case VT_INT: 
    rval->SetUserData(DataArray_impl::udkey_narrow, true);
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

DataArray_impl* DataTable::NewColFromChannelSpec_impl(ChannelSpec* cs) {
  DataArray_impl* rval = NULL;
  if (cs->isMatrix()) {
    rval = NewColMatrixN(cs->val_type, cs->name, cs->cellGeom());
  } else {
    rval = NewCol(cs->val_type, cs->name); 
  }
  if (rval) cs->chan_num = cols() - 1;
  return rval;
}

int_Data* DataTable::NewColInt(const String& col_nm) {
  return (int_Data*)NewCol(VT_INT, col_nm);
}

DataArray_impl* DataTable::NewColMatrix(DataArray_impl::ValType val_type, const String& col_nm,
    int dims, int d0, int d1, int d2, int d3, int d4)
{
  if(dims < 1) {		// < 1 is shortcut for not actually a matrix!
    return NewCol(val_type, col_nm);
  }
  MatrixGeom geom(dims, d0, d1, d2, d3, d4);
  String err_msg;
  if (!taMatrix::GeomIsValid(geom, &err_msg)) {
    taMisc::Error("Invalid geom:", err_msg);
    return NULL;
  }
  
  DataArray_impl* rval = NewColMatrixN(val_type, col_nm, geom);
  return rval;
}

DataArray_impl* DataTable::NewColMatrixN(DataArray_impl::ValType val_type, 
  const String& col_nm, const MatrixGeom& cell_geom) 
{
  StructUpdate(true);
  DataArray_impl* rval = NewCol_impl(val_type, col_nm);
  rval->is_matrix = true;
  rval->cell_geom = cell_geom;
  rval->Init(); // asserts geom
  rval->EnforceRows(rows);	// new guys always get same # of rows as current table
  StructUpdate(false);
  return rval;
}

String_Data* DataTable::NewColString(const String& col_nm) {
  return (String_Data*)NewCol(VT_STRING, col_nm);
}

DataArray_impl* DataTable::FindColName(const String& col_nm, int& col_idx, int val_type, 
				       int dims, int d0, int d1, int d2, int d3, int d4) {
  if(val_type < 0 && dims < 0) {
    DataArray_impl* rval = data.FindLeafName(col_nm, col_idx);
    return rval;
  }
  int lfi = 0;
  taLeafItr i;
  DataArray_impl* ar;
  for(ar = (DataArray_impl*)data.FirstEl(i); ar; ar = (DataArray_impl*)data.NextEl(i), lfi++) {
    if(ar->name != col_nm) continue;
    if((val_type >= 0) && (ar->valType() != (ValType)val_type)) continue;
    if((dims > 0 ) && (ar->cell_dims() != dims)) continue;
    // todo: set geom
    col_idx = lfi;
    return ar;			// found it!
  }
  col_idx = -1;
  return NULL;
}

DataArray_impl* DataTable::FindMakeColName(const String& col_nm, int& col_idx,
					   DataArray_impl::ValType val_type, 
					   int dims, int d0, int d1, int d2, int d3, int d4) {
  DataArray_impl* da = FindColName(col_nm, col_idx, val_type, dims, d0, d1, d2, d3, d4);
  if(da) return da;
  if(dims >= 1)
    return NewColMatrix(val_type, col_nm, dims, d0, d1, d2, d3, d4);
  else
    return NewCol(val_type, col_nm);
}

DataTableCols* DataTable::NewGroupFloat(const String& col_nm, int n) {
  StructUpdate(true);
  DataTableCols* rval = (DataTableCols*)data.NewGp(1);
  rval->el_typ = &TA_float_Data;
  rval->EnforceSize(n);
  rval->name = col_nm;
  if(n > 0) {
    float_Data* da = (float_Data*)NewCol(VT_FLOAT, col_nm, rval);
    da->name = String("<") + (String)n + ">" + col_nm + "_0"; // <n> indicates vector
  }
  int i;
  for(i=1;i<n;i++) {
    float_Data* da = (float_Data*)NewCol(VT_FLOAT, col_nm, rval);
    da->name = String(col_nm) + "_" + String(i);
  }
  StructUpdate(false);
  return rval;
}

DataTableCols* DataTable::NewGroupDouble(const String& col_nm, int n) {
  StructUpdate(true);
  DataTableCols* rval = (DataTableCols*)data.NewGp(1);
  rval->el_typ = &TA_double_Data;
  rval->EnforceSize(n);
  rval->name = col_nm;
  if(n > 0) {
    double_Data* da = (double_Data*)NewCol(VT_DOUBLE, col_nm, rval);
    da->name = String("<") + (String)n + ">" + col_nm + "_0"; // <n> indicates vector
  }
  int i;
  for(i=1;i<n;i++) {
    double_Data* da = (double_Data*)NewCol(VT_DOUBLE, col_nm, rval);
    da->name = String(col_nm) + "_" + String(i);
  }
  StructUpdate(false);
  return rval;
}

DataTableCols* DataTable::NewGroupInt(const String& col_nm, int n) {
  StructUpdate(true);
  DataTableCols* rval = (DataTableCols*)data.NewGp(1);
  rval->el_typ = &TA_int_Data;
  rval->EnforceSize(n);
  rval->name = String("|") + col_nm;
  if(n > 0) {
    int_Data* da = (int_Data*)NewCol(VT_INT, col_nm, rval);
    da->name = String("|<") + (String)n + ">" + col_nm + "_0"; // <n> indicates vector
    da->SetUserData(DataArray_impl::udkey_narrow, true);
  }
  int i;
  for(i=1;i<n;i++) {
    int_Data* da = (int_Data*)NewCol(VT_INT, col_nm, rval);
    da->name = String("|") + String(col_nm) + "_" + String(i);
    da->SetUserData(DataArray_impl::udkey_narrow, true);
  }
  StructUpdate(false);
  return rval;
}

DataTableCols* DataTable::NewGroupString(const String& col_nm, int n) {
//TODO: obs
  StructUpdate(true);
  DataTableCols* rval = (DataTableCols*)data.NewGp(1);
  rval->el_typ = &TA_String_Data;
  rval->EnforceSize(n);
  rval->name = String("$") + col_nm;
  if(n > 0) {
    String_Data* da = (String_Data*)NewCol(VT_STRING, col_nm, rval);
    da->name = String("$<") + (String)n + ">" + col_nm + "_0"; // <n> indicates vector
  }
  int i;
  for(i=1;i<n;i++) {
    String_Data* da = (String_Data*)NewCol(VT_STRING, col_nm, rval);
    da->name = String("$") + String(col_nm) + "_" + String(i);
  }
  StructUpdate(false);
  return rval;
}

void DataTable::RemoveCol(int col) {
  DataArray_impl* da = data.Leaf(col);
  if (!da) return;
  StructUpdate(true);
  da->Close();
  StructUpdate(false);
  //note: you can possibly get left with completely NULL rows,
  // but we don't renormalize
}

void DataTable::RemoveOrphanCols() {
  int cls_cnt = 0; // used to prevent spurious struct updates
  DataArray_impl* da;
  taLeafItr itr;
  FOR_ITR_EL_REV(DataArray_impl, da, data., itr) {
    if (da->mark && !da->pin) {
      if (cls_cnt++ == 0) StructUpdate(true);
      da->Close();
    }
  }
  if (cls_cnt)  {
    StructUpdate(false);
  }
}
  
void DataTable::RemoveRow(int row) {
  if (!RowInRangeNormalize(row)) return;
  DataUpdate(true);
  if (m_dm) m_dm->beginRemoveRows(QModelIndex(), row, row);
  
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    int act_row;
    if (idx(row, ar->AR()->frames(), act_row))
      ar->AR()->RemoveFrame(act_row);
  }
  --rows;
  
  if (m_dm) m_dm->endRemoveRows();
  DataUpdate(false);
}

bool DataTable::RowInRangeNormalize(int& row) {
  if (row < 0) row = rows + row;
  return ((row >= 0) && (row < rows));
} 


/*TODO void DataTable::ShiftUp(int num_rows) {

  if (num_rows >= rows) {
    ResetData();
    return;
  }
  DataUpdate(true);
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    int act_num_rows = num_rows - (rows - ar->AR()->frames());
    if (act_num_rows > 0)
      ar->AR()->ShiftLeft(act_num_rows);
  }
  rows -= num_rows;
  DataUpdate(false); 
}*/


void DataTable::Reset() {
  StructUpdate(true);
  data.Reset();
  rows = 0;
  StructUpdate(false);
}

void DataTable::ResetData() {
  if (rows == 0) return; // prevent erroneous m_dm calls
  DataUpdate(true);
  if (m_dm) m_dm->beginRemoveRows(QModelIndex(), 0, rows - 1);
  
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    ar->AR()->Reset();
  }
  rows = 0;
  
  if (m_dm) m_dm->endRemoveRows();
  DataUpdate(false);
  // also update itrs in case using simple itr mode
  ReadItrInit();
  WriteItrInit();
}

void DataTable::RowsAdding(int n, bool begin) {
  if (begin) {
    DataUpdate(true);
    if (m_dm) {
      m_dm->beginInsertRows(QModelIndex(), rows, rows + n);
    }
  } else { // end
    rows += n;
    
    if (m_dm) {
      m_dm->endInsertRows();
    }
    DataUpdate(false);
  }
}


/*
  Header format: 
    "+" indicates concatenated elements
    {} indicates optionally repeated elements
    format is tab-separated, newline-terminated
    data-type codes are as follows:
      | - int (aka "Narrow")
      $ - String
      (none) - float
      @ - byte
    Scalar Header name is as follows:
      type-code+name
    Matrix Header master name is as follows:
      <dimcount{,dimcount}>[0{,0}]+type-code+name
    Matrix Header slave names are as follows:
      [dimval{,dimval}]+type-code+name
      
    ex:
    &StrCol	|IntCol	FloatCol	<1,2>[0,0]@ByteMat [0,1]@ByteMat
*/
void DataTable::SaveHeader(ostream& strm) {
/*TODO
  bool first = true;
  DataArray_impl* da;
  taLeafItr itr;
  String rootnm;
  FOR_ITR_EL(DataArray_impl, da, data., itr) {
    if (!da->save_to_file) goto cont1;
    // we must precheck for invalid matrix types
    if (da->cell_size() == 0) goto cont1; // TODO: should probably issue a warning
    
    // get root name, which has type info
    rootnm = "";
    switch (da->valType()) {
    case VT_STRING:
      rootnm = "$";
      break;
    case VT_FLOAT:
      break;
    case VT_INT:
      rootnm = "|";
      break;
    case VT_BYTE:
      rootnm = "@";
      break;
    default: goto cont1; // unknown
    }
    rootnm = rootnm + da->name;
    if (!first) {
      ostrm << '\t';
      first = false;
    }
    if (da->is_matrix()) { // if matrix, we output master col, then slave cols
      String hdnm = "<"; // for each col, esp for matrix
      
    } else {
      ostrm << rootnm;
    }
    if ((display_labels.size > i) && !display_labels[i].empty())
      hdnm = display_labels[i];
    int wdth = 2;
    if (da->HasDispOption(" NARROW,"))
      wdth = 1;
    LogColumn(strm, hdnm, wdth);
cont1: ;
  }
  strm << "\n";
  strm.flush();
*/
}

void DataTable::SaveData(ostream& strm) {
}

void DataTable::LoadHeader(istream& strm) {
}

void DataTable::LoadData(istream& strm, int max_recs) {
  ResetData();
}

void DataTable::SetColName(const String& col_nm, int col) {
  DataArray_impl* da = GetColData(col);
  if(da != NULL) da->name = col_nm;
}

/*obs void DataTable::SetCols(LogData& ld) {
  int cur_i = 0;		// current item at top level
  int subgp_gpi = 0;		// current subgroup index (of the group)
  int subgp_i = 0;		// current subgroup index (of the items in the group)
  int subgp_max = 0;		// max for current subgroup
  DataTable* subgp = NULL;	// the subgroup

  int ldi;
  StructUpdate(true);
  for (ldi=0; ldi< ld.items.size; ldi++) {
    ChannelSpec* ditem = ld.items.FastEl(ldi);
    if (ditem->vec_n > 1) {
      if (gp.size > subgp_gpi)
	subgp = (DataTable*)gp[subgp_gpi];
      else
	subgp = (DataTable*)NewGp(1);
      subgp_gpi++;
      subgp_max = ditem->vec_n;
      subgp_i = 0;

      subgp->name = ditem->name;	// group gets name of first element
      if(!subgp->name.empty() && (subgp->name[0] == '<'))
	subgp->name = subgp->name.after('>'); // get rid of vector notation

      if(subgp->size > subgp_max)
	subgp->EnforceSize(subgp_max); // trim excess (but don't add -- could be wrong)
      SetFieldHead(ditem_i);
    } else {
      if (subgp != NULL) {	// in a subgroup
	subgp_i++;		// increment the index
	if (subgp_i >= subgp_max) { // done with this group
	  subgp = NULL;
	  SetFieldHead(ditem, this, cur_i);
	  cur_i++;
	} else {			// get item from this group
	  SetFieldHead(ditem_i);
	}
      } else {			// in top-level group
	SetFieldHead(ditem, this, cur_i);
	cur_i++;
      }
    }
  }
  if(size > cur_i)		// keep it the same size
    EnforceSize(cur_i);
  if (gp.size > subgp_gpi)	// keep it the same size
    gp.EnforceSize(subgp_gpi);
  StructUpdate(false);
} */

bool DataTable::SetValAsDouble(double val, int col, int row) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsDouble(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloat(float val, int col, int row) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsFloat(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsMatrix(const taMatrix* val, int col, int row) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (!da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    return da->SetValAsMatrix(val, i);
  } else return false;
}

bool DataTable::SetValAsString(const String& val, int col, int row) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsString(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsVar(const Variant& val, int col, int row) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsVar(val, i);
    return true;
  } else return false;
}


//////////////////////////
//  DataColViewSpec	//
//////////////////////////

void DataColViewSpec::Initialize(){
  data_base = &TA_DataArray_impl;
}

bool DataColViewSpec::BuildFromDataArray(DataArray_impl* nda) {
  // first time if null or changing
  bool first_time = (dataCol() != nda);
  SetData(nda);
  if (!dataCol()) {
    return false;
  }
  BuildFromDataArray_impl(first_time);
  return true;
}

void DataColViewSpec::BuildFromDataArray_impl(bool first_time) {
  DataArray_impl* data_array = dataCol();
  if (name != data_array->name) {
//NO, don't mess with data    if (!first_time)   data_array->AR()->Reset();
    name = data_array->name;
  }
  // only copy display options first time, since user may override in view
  if (first_time) {
    if (data_array->GetUserData(DataArray_impl::udkey_hidden).toBool())
      visible = false;
  }
}


//////////////////////////
//  DataColViewSpecs	//
//////////////////////////

void DataColViewSpecs::ReBuildFromDataTable(DataTableCols* data_cols) {
  int i;
  DataTableViewSpec* dts = GET_MY_OWNER(DataTableViewSpec);
  DataColViewSpec* dcs = NULL;
  DataArray_impl* da = NULL;
//  delete orphans
  for (i = size - 1; i >= 0; --i) {
    dcs = FastEl(i);
    if ((da = data_cols->FindName(dcs->GetName()))) {
      ; //nothing
    } else {
      Remove(i);
    }
  }  
// items: add missing, order correctly, and update existing (will be only action 1st time)
  for (i = 0; i < data_cols->size; ++i) {
    da = data_cols->FastEl(i);
    int fm;
    if ((dcs = (DataColViewSpec*)FindName(da->GetName(), fm))) {
      if (fm != i) Move(fm, i);
    } else {
      dcs = (DataColViewSpec*)taBase::MakeToken(el_typ); // of correct type for this
      Insert(dcs, i);
      dcs->setFont(dts->font);
    }
    dcs->BuildFromDataArray(da);
  }  
}


//////////////////////////
// DataTableViewSpec	//
//////////////////////////

void DataTableViewSpec::Initialize() {
  data_base = &TA_DataTable;
  font.pointSize = 8;
}

void DataTableViewSpec::Destroy(){
  CutLinks();
}

void DataTableViewSpec::InitLinks() {
  inherited::InitLinks();
  taBase::Own(col_specs, this);
  taBase::Own(font, this);
}

void DataTableViewSpec::CutLinks() {
  font.CutLinks();
  col_specs.CutLinks();
  inherited::CutLinks();
}

void DataTableViewSpec::Copy_(const DataTableViewSpec& cp) {
  font = cp.font;
  col_specs = cp.col_specs;
}

void DataTableViewSpec::UpdateAfterEdit_impl(){
  BuildFromDataTable(dataTable());
  inherited::UpdateAfterEdit_impl();
}

bool DataTableViewSpec::BuildFromDataTable(DataTable* tdt, bool force) {
  if (dataTable() != tdt) {
    force = true;
    SetData(tdt);
  }
  if (!dataTable()) {
    col_specs.Reset();
    return false;
  }
  DataTable* data_table = dataTable(); // cache
  if (!force) {
    // compare sizes
    bool same_size = (col_specs.size == data_table->data.size);
    force = !same_size;
  }
  if (!force) {
    bool same_name = (name == data_table->name);
    force = !same_name;
  }

  if (!force) return false;

  ReBuildFromDataTable_impl();
  return true;
}

void DataTableViewSpec::ReBuildFromDataTable() {
  if (dataTable())
    ReBuildFromDataTable_impl();
}

void DataTableViewSpec::ReBuildFromDataTable_impl() {
  //note: only called if data
  DataTableCols* data_cols = &dataTable()->data;
  col_specs.ReBuildFromDataTable(data_cols);
}


//////////////////////////////////
//   DataTableModel		//
//////////////////////////////////

DataTableModel::DataTableModel(DataTable* owner) 
:inherited(NULL)
{
  dt = owner;
}

DataTableModel::~DataTableModel() {
  if (dt) {
    dt->m_dm = NULL;
    dt = NULL;
  }
}

int DataTableModel::columnCount(const QModelIndex& parent) const {
  return dt->cols();
}

QVariant DataTableModel::data(const QModelIndex& index, int role) const {
  if (!dt || !index.isValid()) return QVariant();
  //NOTES:
  // * it would be nice to just italicize the "matrix" text, but we have no
  //   no access to the font being used, and cannot only pass modifiers
  
  DataArray_impl* col = dt->GetColData(index.column());
  // if no col, we really don't care about anything else...
  if (!col) return QVariant(); // nil
  
  switch (role) {
  case Qt::DisplayRole: //note: we may choose to format different for display, ex floats
  case Qt::EditRole:
    if (col->is_matrix) 
      return QVariant("(matrix)"); // user clicks to edit, or elsewise displayed
    else
      return dt->GetValAsVar(index.column(), index.row());
// Qt::FontRole: //  QFont: font for the text
//Qt::DecorationRole
//Qt::ToolTipRole
//Qt::StatusTipRole
//Qt::WhatsThisRole
//Qt::SizeHintRole -- QSize
//Qt::FontRole--  QFont: font for the text
  case Qt::TextAlignmentRole: {
    if (col->is_matrix)
      return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
    else if (col->isNumeric())
      return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
      return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } break;
//Qt::BackgroundColorRole -- QColor
//  but* only used when !(option.showDecorationSelected && (option.state & QStyle::State_Selected))
  case Qt::TextColorRole: { // QColor: color of text
    if (col->is_matrix)
      return QColor(Qt::blue);
    } break;
//Qt::CheckStateRole
  default: break;
  }
  return QVariant();
}

void DataTableModel::emit_layoutChanged() {
  emit layoutChanged();
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex& index) const {
  if (!dt || !index.isValid()) return 0;
  Qt::ItemFlags rval = 0;
  if (ValidateIndex(index)) {
    // don't enable null cells
    if (dt->hasData(index.column(), index.row() )) {
      rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
      //TODO: determine if not editable, ex. maybe for matrix types
      DataArray_impl* col = dt->GetColData(index.column());
      if (col && !col->is_matrix)  
        rval |= Qt::ItemIsEditable;
    }
  }
  return rval;
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();
  if (orientation == Qt::Horizontal) {
    DataArray_impl* col = dt->GetColData(section);
    if (col)  
      return QString(col->GetDisplayName().chars());
    else 
      return QString();
  } else {
    return QString::number(section);
  }
}

int DataTableModel::rowCount(const QModelIndex& parent) const {
  return dt->rows;
}

bool DataTableModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  if (!dt || !index.isValid()) return false;
  
  DataArray_impl* col = dt->GetColData(index.column());
  // if no col, we really don't care about anything else...
  if (!col) return false; 
  //we restrict setData for scalars only -- use delegate for matrix
  if (col->is_matrix) return false;
  
  bool rval = false;
  switch (role) {
  case Qt::EditRole:
    dt->SetValAsVar(value, index.column(), index.row());
    emit dataChanged(index, index);
    rval = true;
  default: break;
  }
  return rval;
}

bool DataTableModel::ValidateIndex(const QModelIndex& index) const {
  if (!dt) return false;
  return (index.isValid() && (index.row() < dt->rows) && (index.column() < dt->cols()));
}


