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

#include "ta_math.h"

#ifdef TA_GUI
#  include "ta_datatable_qtso.h"
#endif

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

void DataArray_impl::Initialize() {
  mark = false;
  pin = false;
  is_matrix = false;
  // default initialize to scalar
  cell_geom.SetSize(1);
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

void DataArray_impl::CopyFromRow_Robust(int dest_row, const DataArray_impl& src, int src_row) {
  if(src.is_matrix) {
    if(is_matrix) {
      int mx_sz = MIN(cell_size(), src.cell_size());
      for(int i=0; i<mx_sz; i++) {
	SetValAsVarM(src.GetValAsVarM(src_row, i), dest_row, i);
      }
    }
    else {
      SetValAsVar(src.GetValAsVarM(src_row, 0), dest_row);
    }
  }
  else {
    if(is_matrix) {
      SetValAsVarM(src.GetValAsVar(src_row), dest_row, 0);
    }
    else {
      SetValAsVar(src.GetValAsVar(src_row), dest_row);
    }
  }
}

void DataArray_impl::Init() {
  taMatrix* ar = AR(); //cache
  if (is_matrix) {
    MatrixGeom tdim = cell_geom;
    tdim.SetSize(tdim.size + 1); // leaves the new outer dim = 0, which is flex sizing
    ar->SetGeomN(tdim);
  } else {
    ar->SetGeom(1, 0); // sets to 1-d, with flex sizing
  }
}

bool DataArray_impl::isImage() const {
  return GetUserDataAsBool("IMAGE");
}

int DataArray_impl::imageComponents() const {
  if (cell_geom.size <= 2) return 1;
  else return cell_geom.FastEl(2);
}

void DataArray_impl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Init();
  name = taMisc::StringCVar(name); // make col names C legal names (for data proc ops)
}

void DataArray_impl::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  // treat item changes here as struct changes to the table
  if (dcr == DCR_ITEM_UPDATED) {
    DataTable* dt = dataTable();
    if (dt) {
      dt->StructUpdate(true);
      dt->StructUpdate(false);
    }
  }
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
    switch (valType()) {
    case VT_STRING: rval = 16; break; // maximum width for strings
    case VT_DOUBLE: rval = 16; break;
    case VT_FLOAT: rval = 8; break;
    case VT_INT: rval = 8; break;
    case VT_BYTE: rval = 3; break;
    case VT_VARIANT: rval = 10; break;
    default: break;
    }
  }
  if (rval == 0)
    rval = 8; // default
  // include name
  rval = MAX(rval, name.length());
  return rval;
}

taBase::DumpQueryResult DataArray_impl::Dump_QuerySaveMember(MemberDef* md) {
  if (md->name == "ar") {
    // if no save, don't need to check DataTable global
    if (saveToFile()) {
      DataTable* dt = dataTable();
      if (dt && dt->save_data) return DQR_SAVE;
    }
    return DQR_NO_SAVE;
  } else return inherited::Dump_QuerySaveMember(md);
}

void DataArray_impl::Get2DCellGeom(int& x, int& y, bool odd_y) {
  x = 1;
  y = 1;
  if(isMatrix()) {
    cell_geom.Get2DGeom(x, y, odd_y);
  }
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
  return name;
}

taMatrix* DataArray_impl::GetValAsMatrix(int row) {
  taMatrix* ar = AR(); 
  if(row < 0) row = rows() + row;
  return ar->GetFrameSlice_(row);
}

taMatrix* DataArray_impl::GetRangeAsMatrix(int st_row, int n_rows) {
  taMatrix* ar = AR();
  if(st_row < 0) st_row = rows() + st_row;
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
  if (row < 0) row = rows() + row; // abs row, if request was from end
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

bool DataArray_impl::GetMinMaxScale(MinMax& mm) {
  if(!isNumeric()) return false;
  int idx;
  if(valType() == VT_FLOAT) {
    float_Matrix* fm = (float_Matrix*)AR();
    mm.min = taMath_float::vec_min(fm, idx);
    mm.max = taMath_float::vec_max(fm, idx);
  }
  else if(valType() == VT_DOUBLE) {
    double_Matrix* fm = (double_Matrix*)AR();
    mm.min = taMath_double::vec_min(fm, idx);
    mm.max = taMath_double::vec_max(fm, idx);
  }
  else {
    return false;		// not worth it!
  }
  return true;
}


String DataArray_impl::EncodeHeaderName(int d0, int d1, int d2, int d3, int d4) {
  String typ_info;
  switch (valType()) {
  case VT_STRING: 	typ_info = "$"; break;
  case VT_FLOAT:  	typ_info = "%"; break;
  case VT_DOUBLE:  	typ_info = "#"; break;
  case VT_INT:  	typ_info = "|"; break;
  case VT_BYTE:  	typ_info = "@"; break;
  case VT_VARIANT: 	typ_info = "&"; break;
  }
  String mat_info;
  if(is_matrix) {		// specify which cell in matrix this is [dims:d0,d1,d2..]
    MatrixGeom mg(cell_geom.size, d0, d1, d2, d3, d4);
    mat_info = mg.GeomToString("[", "]");
    if(cell_geom.IndexFmDims(d0, d1, d2, d3, d4) == 0) { // first guy
      mat_info += cell_geom.GeomToString("<", ">");
    }
  }
  return typ_info + name + mat_info; // e.g., $StringVecCol[2:2,3]
}

void DataArray_impl::DecodeHeaderName(String nm, String& base_nm, int& vt,
				      MatrixGeom& mat_idx, MatrixGeom& mat_geom) {
  base_nm = nm;
  vt = -1; // unknown
  mat_idx.SetSize(0);
  mat_geom.SetSize(0);
  if (nm.empty()) return;

  // first check for type info:
  if (nm[0] == '$') {
    nm = nm.after('$');
    vt = VT_STRING;
  } else if (nm[0] == '%') {
    nm = nm.after('%');
    vt = VT_FLOAT;
  } else if (nm[0] == '#') {
    nm = nm.after('#');
    vt = VT_DOUBLE;
  } else if (nm[0] == '|') {
    nm = nm.after('|');
    vt = VT_INT;
  } else if (nm[0] == '@') {
    nm = nm.after('@');
    vt = VT_BYTE;
  } else if (nm[0] == '&') {
    nm = nm.after('&');
    vt = VT_VARIANT;
  } else {
    vt = VT_FLOAT;
  }
  
  if(nm.contains('[')) {
    String mat_info = nm;
    nm = nm.before('[');
    mat_idx.GeomFromString(mat_info, "[", "]");
    if(mat_info.contains('<')) {
      mat_geom.GeomFromString(mat_info, "<", ">");
    }
  }
  base_nm = nm;
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
  int mx = MIN(size, src.size);
  for(int i=0; i< mx; i++) {
    DataArray_impl* dar = FastEl(i);
    DataArray_impl* sar = src.FastEl(i);
    dar->CopyFromRow(dest_row, *sar, src_row);
  }  
}

void DataTableCols::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);

  if ((dcr >= DCR_LIST_MIN) && (dcr <= DCR_LIST_ITEM_MAX)) {
    DataTable* dt = GET_MY_OWNER(DataTable); // cache
    if (!dt) return;
    // treat changes here as Struct changes on the table
    dt->StructUpdate(true);
    dt->StructUpdate(false);
    
    
    // for most schema changes, we just call the datamodel layoutChanged
    // note that groups themselves don't show in the datatable editor
/*wrong    // it seems to handle Adds and Moves ok, but Deletes leave it
    // with the same number of cols, unless we also send a removeColumn notify
    if (dcr == DCR_GROUP_ITEM_REMOVE) {
      // we just tell it the last col is being deleted, since it will refresh all
      // in the layoutChanged notify
      int col = dt->cols() - 1;
      dt->m_dm->beginRemoveColumns(QModelIndex(), col, col);
      dt->m_dm->endRemoveColumns();
    }*/
    if (dt->m_dm) {
      dt->m_dm->emit_layoutChanged();
    }
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

int DataTable::idx_def_arg = 0;

void DataTable::Initialize() {
  rows = 0;
  save_data = true;
  m_dm = NULL; // returns new if none exists, or existing -- enables views to be shared
  log_file = NULL;
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
  log_file = taFiler::New("DataTable", ".dat");
  taRefN::Ref(log_file);
}

void DataTable::CutLinks() {
  data.CutLinks();
  if(log_file) {
    taRefN::unRefDone(log_file);
    log_file = NULL;
  }
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


bool DataTable::CopyColRow(int dest_col, int dest_row, const DataTable& src, int src_col, int src_row) {
  DataArray_impl* dar = data.SafeEl(dest_col);
  DataArray_impl* sar = src.data.SafeEl(src_col);
  if(!dar || !sar) {
    taMisc::Error("CopyColRow: column(s) out of range, not copied!");
    return false;
  }
  dar->CopyFromRow_Robust(dest_row, *sar, src_row);
  return true;
}

void DataTable::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UniqueColNames();
}

bool DataTable::AddRow(int n) {
  if ((cols() == 0) || (n < 1)) return false;
  RowsAdding(n, true);
  for(int i=0;i<data.size;i++) {
    DataArray_impl* ar = data.FastEl(i);
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
  for(int i=0;i<data.size;i++) {
    DataArray_impl* ar = data.FastEl(i);
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
  else return data.SafeEl(col);
}

DataArray_impl* DataTable::GetColForChannelSpec_impl(ChannelSpec* cs) {
  for(int i=data.size-1;i>=0;i--) {
    DataArray_impl* da = data.FastEl(i);
    if (da->name != cs->name) continue;
    // if name matches, but not contents, we need to remake it...
    if (ColMatchesChannelSpec(da, cs)) {
      da->mark = false; // reset mark for orphan tracking
      cs->chan_num = i;
      return da;
    }
    else {
      da->Close();
    }
  }
  DataArray_impl* rval = NewColFromChannelSpec_impl(cs);
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

/////////////////////

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

////////////////////////////

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

bool DataTable::SetValAsMatrix(const taMatrix* val, int col, int row) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (!da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    return da->SetValAsMatrix(val, i);
  } else return false;
}

////////////////////////

double DataTable::GetValAsDoubleM(int col, int row, int cell) {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsDoubleM(i, cell);
  else return 0.0f;
}

float DataTable::GetValAsFloatM(int col, int row, int cell) {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsFloatM(i, cell);
  else return 0.0f;
}

const String DataTable::GetValAsStringM(int col, int row, int cell) const {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsStringM(i, cell);
  else return "n/a";
}

const Variant DataTable::GetValAsVarM(int col, int row, int cell) const {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsVarM(i, cell);
  else return _nilVariant;
}

////////////////////////

bool DataTable::SetValAsDoubleM(double val, int col, int row, int cell) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (!da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsDoubleM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloatM(float val, int col, int row, int cell) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (!da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsFloatM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsStringM(const String& val, int col, int row, int cell) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (!da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsStringM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsVarM(const Variant& val, int col, int row, int cell) {
  DataArray_impl* da = GetColData(col);
  if (!da) return false;
  if (!da->is_matrix) return false;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsVarM(val, i, cell);
    return true;
  } else return false;
}

////////////////////////

bool DataTable::hasData(int col, int row) {
  DataArray_impl* da = GetColData(col);
  int i;
  return (da && idx(row, da->rows(), i));
}


void DataTable::MarkCols() {
  for(int i=0;i<data.size;i++) {
    DataArray_impl* da = data.FastEl(i);
    da->mark = true;
  }
}

int DataTable::MaxLength() {
  return rows;
}

int DataTable::MinLength() {
  if (cols() == 0) return 0;
  int min = INT_MAX;
  for(int i=0;i<data.size;i++) {
    DataArray_impl* ar = data.FastEl(i);
    min = MIN(min,ar->AR()->frames());
  }
  return min;
}

DataArray_impl* DataTable::NewCol(DataArray_impl::ValType val_type, 
				  const String& col_nm) 
{
  StructUpdate(true);
  DataArray_impl* rval = NewCol_impl(val_type, col_nm);
  rval->Init(); // asserts geom
  rval->EnforceRows(rows);	// new guys always get same # of rows as current table
  StructUpdate(false);
  return rval;
}

DataArray_impl* DataTable::NewCol_impl(DataArray_impl::ValType val_type, 
				       const String& col_nm) 
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
  DataArray_impl* rval = (DataArray_impl*) data.New(1, td);
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
  rval->DataChanged(DCR_ITEM_UPDATED); // because we set name after creation
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

void DataTable::SetColName(const String& col_nm, int col) {
  DataArray_impl* da = GetColData(col);
  if(da) da->name = col_nm;
}

bool DataTable::RenameCol(const String& cur_nm, const String& new_nm) {
  DataArray_impl* da = FindColName(cur_nm);
  if(!da) return false;
  da->name = new_nm;
  return true;
}

DataArray_impl* DataTable::FindColName(const String& col_nm, int& col_idx, bool err_msg) {
  DataArray_impl* da = data.FindName(col_nm, col_idx);
  if(!da && err_msg) {
    taMisc::Error("Error -- could not find column named:", col_nm, "in data table named:",
		  name, "path:", GetPath());
  }
  return da;
}

DataArray_impl* DataTable::FindMakeColName(const String& col_nm, int& col_idx,
					   ValType val_type, int dims,
					   int d0, int d1, int d2, int d3, int d4) {
  DataArray_impl* da = FindColName(col_nm, col_idx);
  if(da) {
    if(da->valType() != (ValType)val_type) {
      StructUpdate(true);
      DataArray_impl* nda;
      if(dims > 0)
	nda = NewColMatrix(val_type, col_nm, dims, d0, d1, d2, d3, d4);
      else
	nda = NewCol(val_type, col_nm);
      data.MoveIdx(data.size-1, col_idx);
      data.RemoveEl(da);	// get rid of that guy
      da = nda;
      nda->EnforceRows(rows);	// keep row-constant
      StructUpdate(false);
    } else if(da->cell_dims() != dims) {
      StructUpdate(true);
      da->cell_geom.SetGeom(dims, d0, d1, d2, d3, d4);
      if(dims == 0)
	da->is_matrix = false;
      else
	da->is_matrix = true;
      da->Init();		// asserts geom
      da->EnforceRows(rows);	// keep row-constant
      StructUpdate(false);
    } else {
      StructUpdate(true);
      MatrixGeom mg(dims, d0, d1, d2, d3, d4);
      if(mg != da->cell_geom) {
	da->cell_geom = mg;
	da->Init();
      StructUpdate(false);
      }
    }
    return da;
  }
  else {			// not found -- make one
    col_idx = data.size;	// will be the next guy
    if(dims >= 1)
      return NewColMatrix(val_type, col_nm, dims, d0, d1, d2, d3, d4);
    else
      return NewCol(val_type, col_nm);
  }
}

void DataTable::UniqueColNames() {
  for(int i=0;i<data.size; i++) {
    DataArray_impl* da = data.FastEl(i);
    int dupl = 0;
    for(int j=i+1;j<data.size; j++) {
      DataArray_impl* oda = data.FastEl(j);
      if(da->name == oda->name) {
	dupl++;
	oda->name += "_" + String(dupl);
      }
    }
  }
}

void DataTable::RemoveCol(int col) {
  DataArray_impl* da = data.SafeEl(col);
  if (!da) return;
  StructUpdate(true);
  da->Close();
  StructUpdate(false);
  //note: you can possibly get left with completely NULL rows,
  // but we don't renormalize
}

void DataTable::RemoveOrphanCols() {
  int cls_cnt = 0; // used to prevent spurious struct updates
  for(int i=data.size-1;i>=0;i--) {
    DataArray_impl* da = data.FastEl(i);
    if (da->mark && !da->pin) {
      if(cls_cnt == 0) StructUpdate(true);
      da->Close();
      cls_cnt++;
    }
  }
  if (cls_cnt)  {
    StructUpdate(false);
  }
}
  
void DataTable::RemoveRow(int row) {
  if (!RowInRangeNormalize(row)) return;
  DataUpdate(true);		// only data because for views, no change in column structure
  if (m_dm) m_dm->beginRemoveRows(QModelIndex(), row, row);
  
  for(int i=0;i<data.size;i++) {
    DataArray_impl* ar = data.FastEl(i);
    int act_row;
    if (idx(row, ar->AR()->frames(), act_row))
      ar->AR()->RemoveFrame(act_row);
  }
  --rows;
  
  if (m_dm) m_dm->endRemoveRows();
  DataUpdate(false);
}

bool DataTable::DuplicateRow(int row_no, int n_copies) {
  if(row_no >= rows) return false;
  DataUpdate(true);// only data because for views, no change in column structure
  for(int k=0;k<n_copies;k++) {
    AddBlankRow();
    for(int j=0;j<data.size;j++) {
      DataArray_impl* sda = data[j];
      sda->CopyFromRow(-1, *sda, row_no);
    }
  }
  DataUpdate(false);
  return true;
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
  StructUpdate(true);
  for(int i=0;i<data.size;i++) {
    DataArray_impl* ar = data.FastEl(i);
    int act_num_rows = num_rows - (rows - ar->AR()->frames());
    if (act_num_rows > 0)
      ar->AR()->ShiftLeft(act_num_rows);
  }
  rows -= num_rows;
  StructUpdate(false); 
}*/


void DataTable::Reset() {
  StructUpdate(true);
  data.Reset();
  rows = 0;
  StructUpdate(false);
}

void DataTable::ResetData() {
  if (rows == 0) return; // prevent erroneous m_dm calls
  DataUpdate(true);// only data because for views, no change in column structure
  if (m_dm) m_dm->beginRemoveRows(QModelIndex(), 0, rows - 1);
  
  for(int i=0;i<data.size;i++) {
    DataArray_impl* ar = data.FastEl(i);
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
    DataUpdate(true);// only data because for views, no change in column structure
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

//////////////////////////////////////////////////////////////////////////////
///		Saving / Loading from Plain Text Files

/*
  Header format: 
  $ = String
  % = float
  # = double
  | = int
  @ = byte
  & = variant

  matrix: each cell has [dims:x,y..]
  first mat cell also has <dims:dx,dy..>
*/

void DataTable::SaveHeader_strm(ostream& strm, Delimiters delim) {
  char cdlm = GetDelim(delim);
  strm << "_H:";		// indicates header
  for(int i=0;i<data.size;i++) {
    DataArray_impl* da = data.FastEl(i);
    if(!da->saveToFile()) continue;
    if (da->cell_size() == 0) continue;

    if(da->isMatrix()) {
      for(int j=0;j<da->cell_size(); j++) {
	int d0, d1, d2, d3, d4;
	da->cell_geom.DimsFmIndex(j, d0, d1, d2, d3, d4);
	String hdnm = da->EncodeHeaderName(d0, d1, d2, d3, d4);
	strm << cdlm << hdnm;
      }
    }
    else {
      String hdnm = da->EncodeHeaderName();
      strm << cdlm << hdnm;
    }
  }
  strm << endl;
}

void DataTable::SaveDataRow_strm(ostream& strm, int row, Delimiters delim, bool quote_str) {
  char cdlm = GetDelim(delim);
  strm << "_D:";		// indicates data row
  for(int i=0;i<data.size;i++) {
    DataArray_impl* da = data.FastEl(i);
    if(!da->saveToFile()) continue;
    if (da->cell_size() == 0) continue;

    if(da->isMatrix()) {
      for(int j=0;j<da->cell_size(); j++) {
	String val = da->GetValAsStringM(row, j);
	if(quote_str && (da->valType() == VT_STRING))
	  strm << cdlm << "\"" << val << "\"";
	else
	  strm << cdlm << val;
      }
    }
    else {
      String val = da->GetValAsString(row);
      if(quote_str && (da->valType() == VT_STRING))
	strm << cdlm << "\"" << val << "\"";
      else
	strm << cdlm << val;
    }
  }
  strm << endl;
}

void DataTable::SaveDataRows_strm(ostream& strm, Delimiters delim, bool quote_str) {
  for(int row=0;row <rows; row++) {
    SaveDataRow_strm(strm, row, delim, quote_str);
  }
}

void DataTable::SaveData_strm(ostream& strm, Delimiters delim, bool quote_str) {
  SaveHeader_strm(strm, delim);
  SaveDataRows_strm(strm, delim, quote_str);
}

void DataTable::SaveHeader(const String& fname, Delimiters delim) {
  taFiler* flr = GetSaveFiler(fname, ".dat", false);
  if(flr->ostrm)
    SaveHeader_strm(*flr->ostrm, delim);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveDataRow(const String& fname, int row, Delimiters delim, bool quote_str) {
  taFiler* flr = GetSaveFiler(fname, ".dat", false);
  if(flr->ostrm)
    SaveDataRow_strm(*flr->ostrm, row, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveData(const String& fname, Delimiters delim, bool quote_str) {
  taFiler* flr = GetSaveFiler(fname, ".dat", false);
  if(flr->ostrm)
    SaveData_strm(*flr->ostrm, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::AppendData(const String& fname, Delimiters delim, bool quote_str) {
  taFiler* flr = GetAppendFiler(fname, ".dat", false);
  if(flr->ostrm)
    SaveDataRows_strm(*flr->ostrm, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveDataLog(const String& fname, bool append) {
  if(!log_file) return;
  if(log_file->isOpen())
    log_file->Close();
  log_file->fname = fname;
  if(append) 
    log_file->Append();
  else {
    log_file->SaveAs();
    if(log_file->isOpen())
      SaveHeader_strm(*log_file->ostrm);
  }
}

void DataTable::CloseDataLog() {
  if(!log_file) return;
  log_file->Close();
}

char DataTable::GetDelim(Delimiters delim) {
  if(delim == TAB) return '\t';
  if(delim == SPACE) return ' ';
  if(delim == COMMA) return ',';
  return ','; // must return something
}

int DataTable::ReadTillDelim(istream& strm, String& str, const char delim, bool quote_str) {
  int c;
  int depth = 0;
  if(quote_str && (strm.peek() == '\"')) {
    strm.get();
    depth++;
  }
  while(strm.peek() == delim) {	// consume any immediate delims
    strm.get();
  }
  while(((c = strm.get()) != EOF) && (c != '\n') && !((c == delim) && (depth <= 0))) {
    if(quote_str && (depth > 0) && (c == '\"'))
      depth--;
    else
      str += (char)c;
  }
  return c;
}

int_Array DataTable::load_col_idx;
int_Array DataTable::load_mat_idx;

int DataTable::LoadHeader_strm(istream& strm, Delimiters delim) {
  char cdlm = GetDelim(delim);
  load_col_idx.Reset();
  load_mat_idx.Reset();
  int c;
  while(true) {
    String str;
    c = ReadTillDelim(strm, str, cdlm, false);
    if(c == EOF) break;
    if(str.empty()) {
      if(c == '\n') break;
      continue;			// for some reason it is empty
    }
    String base_nm;
    int val_typ;
    MatrixGeom mat_idx;
    MatrixGeom mat_geom;
    DataArray_impl::DecodeHeaderName(str, base_nm, val_typ, mat_idx, mat_geom);
    int idx;
    DataArray_impl* da = FindColName(base_nm, idx);
    if(!da || (da->valType() != val_typ)) { // only make new one if val type doesn't match
      // geom was made on first col and should not be remade..
      da = FindMakeColName(base_nm, idx, (ValType)val_typ, mat_geom.size,
			   mat_geom[0], mat_geom[1], mat_geom[2],
			   mat_geom[3], mat_geom[4]);
    }
    load_col_idx.Add(idx);
    if(mat_idx.size > 0) {
      int mdx = da->cell_geom.IndexFmDims(mat_idx[0], mat_idx[1], mat_idx[2], mat_idx[3],
					  mat_idx[4]);
      load_mat_idx.Add(mdx);
    }
    else {
      load_mat_idx.Add(-1);	// no matrix info
    }
    if(c == '\n') break;
  }
  return c;
}

int DataTable::LoadDataRow_strm(istream& strm, Delimiters delim, bool quote_str) {
  char cdlm = GetDelim(delim);
  StructUpdate(true);
  bool added_row = false;
  int last_mat_col = -1;
  int col = 0;
  int c;
  while(true) {
    String str;
    c = ReadTillDelim(strm, str, cdlm, quote_str);
    if(c == EOF) break;
    if(str == "_H:") {
      c = LoadHeader_strm(strm, delim);
      if(c == EOF) break;
      continue;
    }
    if(str == "_D:") continue;
    // at this point it is safe to add a row -- load header already called
    if(!added_row) {
      AddBlankRow();		
      added_row = true;
    }
    int use_col = col;
    if(load_col_idx.size > 0) {
      use_col = load_col_idx[col];
    }
    if(use_col >= data.size) {
      taMisc::Error("Error reading data table", name, "columns exceeded!");
      c = EOF;
      break;
    }
    DataArray_impl* da = data.FastEl(use_col);
    if(da->isMatrix()) {
      int mat_idx = 0;
      if(load_mat_idx.size > 0) {
	mat_idx = load_mat_idx[col];
      }
      else {
	if(last_mat_col >= 0)
	  mat_idx = col - last_mat_col;
	else 
	  last_mat_col = col;
      }
      da->SetValAsStringM(str, -1, mat_idx);
    }
    else {
      last_mat_col = -1;
      da->SetValAsString(str, -1);
    }
    col++;
    if(c == '\n') break;
  }
  StructUpdate(false);
  return c;
}

void DataTable::LoadData_strm(istream& strm, Delimiters delim, bool quote_str, int max_recs) {
  StructUpdate(true);
  load_col_idx.Reset();
  load_mat_idx.Reset();
  int st_row = rows;
  while(true) {
    int c = LoadDataRow_strm(strm, delim, quote_str);
    if(c == EOF) break;
    if((max_recs > 0) && (rows - st_row >= max_recs)) break;
  }
  //  RemoveRow(-1);		// last one is empty..
  StructUpdate(false);
}

int DataTable::LoadHeader(const String& fname, Delimiters delim) {
  taFiler* flr = GetLoadFiler(fname, ".dat", false);
  int rval = 0;
  if(flr->istrm)
    rval = LoadHeader_strm(*flr->istrm, delim);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

int DataTable::LoadDataRow(const String& fname, Delimiters delim, bool quote_str) {
  taFiler* flr = GetLoadFiler(fname, ".dat", false);
  int rval = 0;
  if(flr->istrm)
    rval = LoadDataRow_strm(*flr->istrm, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void DataTable::LoadData(const String& fname, Delimiters delim, bool quote_str, int max_recs) {
  taFiler* flr = GetLoadFiler(fname, ".dat", false);
  if(flr->istrm)
    LoadData_strm(*flr->istrm, delim, quote_str, max_recs);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::WriteClose_impl() {
  UpdateAllViews();
  if(log_file && log_file->isOpen()) {
    SaveDataRow_strm(*log_file->ostrm);
  }
}

////////////////////////////////////////////////////////////////////////////
//		DMEM

void DataTable::DMem_ShareRows(MPI_Comm comm, int n_rows) {
#ifdef DMEM_COMPILE
  if(rows == 0) return;
  if(n_rows < 1) n_rows = rows;
  if(n_rows > rows) n_rows = rows;
  int np = 0; MPI_Comm_size(comm, &np);
  int this_proc = 0; MPI_Comm_rank(comm, &this_proc);
  if(np <= 1) return;

  DataUpdate(true);

  int st_send_row = rows - n_rows;
  int st_recv_row = rows;
  int n_recv_rows = np * n_rows;
  AddRow(n_recv_rows);		// make room for new ones

  static char_Array char_send;
  static char_Array char_recv;
  const int max_str_len = 1024;

  for(int i=0;i<data.size; i++) {
    DataArray_impl* da = data.FastEl(i);
    taMatrix* da_mat = da->AR();

    int frsz = da_mat->frameSize();
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
  for(int i=0;i<n_rows;i++) {
    RemoveRow(st_send_row);
  }
  DataUpdate(false);
#endif  // DMEM_COMPILE
}



//////////////////////////
//  DataColViewSpec	//
//////////////////////////

/*
  The main DataColViewSpec operations are:
  * initialize ("bind") a new guy from a DataCol
  * update a bound guy (ex things change in the col)
  * unlink a guy (ex. table unlinking)
  * delete a guy (ex. col deletes in table (but table not deleting)
  * bind a guy to a new col (ex., binding to a new table)
  * calculate view-specific parameters based on current state
    -- this is done in Render_impl
*/

void DataColViewSpec::Initialize(){
  sticky = false;
  data_base = &TA_DataArray_impl;
}

void DataColViewSpec::Copy_(const DataColViewSpec& cp) {
  sticky = cp.sticky;
}

void DataColViewSpec::Unbind_impl() {
  if (m_data) setDataCol(NULL);
  inherited::Unbind_impl();
}

void DataColViewSpec::DataDestroying() {
  inherited::DataDestroying();
  DataColUnlinked();
}

void DataColViewSpec::setDataCol(DataArray_impl* value, bool first_time) {
  if (dataCol() == value) return;
  SetData(value);
  if (value) {
    UpdateFromDataCol(first_time);
  } else {
    DataColUnlinked();
  }
}

void DataColViewSpec::UpdateFromDataCol(bool first) {
  UpdateFromDataCol_impl(first);
  DataChanged(DCR_ITEM_UPDATED);
}

void DataColViewSpec::UpdateFromDataCol_impl(bool first) {
  DataArray_impl* col = dataCol();
  if (name != col->name) {
    name = col->name;
  }
  // only copy display options first time, since user may override in view
  if (first) {
    if (col->GetUserData(DataArray_impl::udkey_hidden).toBool())
      visible = false;
  }
}

bool DataColViewSpec::isVisible() const {
  return (visible && (bool)m_data);
}

//////////////////////////
//  DataColViewSpecs	//
//////////////////////////


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

void DataTableViewSpec::Unbind_impl() {
  if (m_data) { 
    SetData(NULL);
    DataTableUnlinked();
  }
  inherited::Unbind_impl(); // will unlink kids
}

void DataTableViewSpec::setDataTable(DataTable* dt) {
  if (dataTable() == dt) return;
  if (dt) {
    SetData(dt);
    bool first = (col_specs.size == 0);
    UpdateFromDataTable(first);
  } else {
    Unbind(); // also does kids
  }
}

void DataTableViewSpec::DataDestroying() {
  Unbind(); //unlinks everyone
  inherited::DataDestroying();
}

void DataTableViewSpec::DataStructUpdateEnd_impl() {
  UpdateFromDataTable();
}
 
void DataTableViewSpec::DataTableUnlinked() {
}

void DataTableViewSpec::DoActionChildren_impl(DataViewAction act) {
// note: only ever called with one action
  if (act & CONSTR_MASK) {
    inherited::DoActionChildren_impl(act);
    col_specs.DoAction(act);
  } else { // DESTR_MASK
    col_specs.DoAction(act);
    inherited::DoActionChildren_impl(act);
  }
}


void DataTableViewSpec::UpdateFromDataTable(bool first) {
  UpdateFromDataTable_this(first);
  UpdateFromDataTable_child(first);
  DataChanged(DCR_ITEM_UPDATED);
}

void DataTableViewSpec::UpdateFromDataTable_this(bool first) {
  DataTable* dt = dataTable();
  if (first) {
    if (name != dt->name)
      name = dt->name;
  }

}

void DataTableViewSpec::UpdateFromDataTable_child(bool first) {
  DataTableCols* cols = &(dataTable()->data);
  DataTableViewSpec* dts = this;
  DataColViewSpec* dcs = NULL;
  DataArray_impl* dc = NULL;
  int i;
/*TODO: revise algorithm as follows:
(may require intermediate link list, to track guys)
1. identify potential orphans, put them aside
2. update existing guys
3. attempt to rebind missing guys to potential orphans
  ex. if cols are in same position, then probably just name difference
4. delete true orphans
5. add missing guys
ALSO: need to probably revise the scheme for reordering -- maybe user
  wants the data in the new order? (OR: maybe should provide separate
  "view_index" in the spec, so user can reorder)
*/
//  delete orphans (except sticky guys)
  for (i = col_specs.size - 1; i >= 0; --i) {
    dcs = col_specs.FastEl(i);
    // first, see if it is bound to our table, if so, just update it (name may have changed, etc.
    if (cols->FindEl(dcs->dataCol()) >= 0) {
      dcs->UpdateFromDataCol();
      continue;
    }
    // if not bound, try to find it by name
    dc = cols->FindName(dcs->GetName());
    if (dc) {
      // make sure it is this col bound to the guy!
      dcs->setDataCol(dc);
    } else { // no target found in table
      if (dcs->sticky) {
        dcs->setDataCol(NULL); //keep him, but unbind from any col
      } else {
        col_specs.RemoveIdx(i);
      }
    }
  }  
// items: add missing, order correctly, and update existing (will be only action 1st time)
  for (i = 0; i < cols->size; ++i) {
    dc = cols->FastEl(i);
    int fm;
    bool first = false;
    if ((dcs = (DataColViewSpec*)col_specs.FindName(dc->GetName(), fm))) {
      if (fm != i) col_specs.MoveIdx(fm, i);
    } else {
      first = true;
      dcs = (DataColViewSpec*)taBase::MakeToken(col_specs.el_typ); // of correct type for this
      col_specs.Insert(dcs, i);
      dcs->setFont(dts->font);
    }
    dcs->setDataCol(dc, first);
  }  
//   cerr << "data spec cols updated: " << cols->size << " " << col_specs.size << endl;
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
      return dt->GetValAsString(index.column(), index.row());
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

void DataTableModel::refreshViews() {
  emit dataChanged(createIndex(0, 0), 
    createIndex(rowCount() - 1, columnCount() - 1));
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


//////////////////////////////////
//  GridColViewSpec		//
//////////////////////////////////

void GridColViewSpec::Initialize(){
  text_width = 16;
  scale_on = true;
  mat_layout = BOT_ZERO; // typical default for data patterns
  mat_image = false;
  mat_odd_vert = true;
  col_width = 0.0f;
  row_height = 0.0f;
}

void GridColViewSpec::Copy_(const GridColViewSpec& cp){
  text_width = cp.text_width;
  scale_on = cp.scale_on;
  mat_layout = cp.mat_layout;
  mat_image = cp.mat_image;
  mat_odd_vert = cp.mat_odd_vert;
  // others recalced
}

void GridColViewSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (text_width < 2) text_width = 2; // smallest practical
}

void GridColViewSpec::UpdateFromDataCol_impl(bool first){
  inherited::UpdateFromDataCol_impl(first);
  DataArray_impl* dc = dataCol(); //note: exists, because we were called
  if (first) {
    // just get the display width, don't worry about maxwidth
    text_width = dc->displayWidth();
  
    if (dc->isMatrix() && dc->isNumeric()) {
      mat_image = dc->GetUserData("IMAGE").toBool();
    }
  }
}

void GridColViewSpec::DataColUnlinked() {
  col_width = 0.0f;
  row_height = 0.0f;
}

void GridColViewSpec::Render_impl() {
  inherited::Render_impl(); // prob nothing

  // main point here is to compute size of column based on disp params
  // this is then normalized by the gridview display, so units are arbitrary
  // and are set to be characters (why not?)

  GridTableViewSpec* par = parent();
  DataArray_impl* dc = dataCol(); // cache
  col_width = 0.0f;
  row_height = 0.0f;
  if (!dc) return;

  if(dc->isMatrix()) {
    int raw_width = 1;
    int raw_height = 1;
    dc->Get2DCellGeom(raw_width, raw_height, mat_odd_vert);
    // just linear in block size between range
    col_width = par->mat_size_range.Clip(raw_width);
    row_height = par->mat_size_range.Clip(raw_height);
  }
  else {
    row_height = 1.0f;		// always just one char high
    col_width = text_width;
  }
}


//////////////////////////////////
//  GridTableViewSpec		//
//////////////////////////////////

void GridTableViewSpec::Initialize() {
  col_specs.SetBaseType(&TA_GridColViewSpec);

  grid_margin = 0.1f;
  grid_line_size = 0.05f;
  row_num_width = 4;

  mat_size_range.min = 4;
  mat_size_range.max = 16;
  mat_val_text = false;
  mat_block_spc = 0.1f;
  mat_block_height = 0.2f;
  mat_trans = 0.6f;
}

void GridTableViewSpec::InitLinks() {
  inherited::InitLinks();
  taBase::Own(mat_size_range, this);
}

void GridTableViewSpec::Destroy() {
}

void GridTableViewSpec::Copy_(const GridTableViewSpec& cp) {
  grid_margin = cp.grid_margin;
  grid_line_size = cp.grid_line_size;

  mat_size_range = cp.mat_size_range;
  mat_val_text = cp.mat_val_text;
  mat_block_spc = cp.mat_block_spc;
  mat_block_height = cp.mat_block_height;
  mat_trans = cp.mat_trans;
}

void GridTableViewSpec::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  if (grid_margin < 0.0f) grid_margin = 0.0f;
  if (grid_line_size <  0.0f) grid_line_size =  0.0f;
}

void GridTableViewSpec::DataDataChanged_impl(int dcr, void* op1, void* op2) {
  inherited::DataDataChanged_impl(dcr, op1, op2);
  TableView* tv = GET_MY_OWNER(TableView);
  if (!tv) return;
  tv->DataChanged_DataTable(dcr, op1, op2);
}

void GridTableViewSpec::DataUpdateView_impl() {
  inherited::DataUpdateView_impl();
  TableView* tv = GET_MY_OWNER(TableView);
  if (!tv) return;
  tv->DataChanged_DataTable(DCR_UPDATE_VIEWS, NULL, NULL);
}

void GridTableViewSpec::DataUpdateAfterEdit_impl() {
  inherited::DataUpdateAfterEdit_impl();
  TableView* tv = GET_MY_OWNER(TableView);
  if (!tv) return;
  tv->DataChanged_DataTable(DCR_UPDATE_VIEWS, NULL, NULL);
}

void GridTableViewSpec::DataDestroying() {
  TableView* tv = GET_MY_OWNER(TableView);
  if (tv) {
    tv->DataChanged_DataTable(DCR_ITEM_DELETING, NULL, NULL);
  }
  inherited::DataDestroying();
}

void GridTableViewSpec::GetMinMaxScale(MinMax& mm, bool first) {
  int i;
  for (i=0;i< col_specs.size; i++){
    GridColViewSpec* vs = (GridColViewSpec *) col_specs.FastEl(i);
    if(!vs->visible || !vs->scale_on)
      continue;
    DataArray_impl* da = vs->dataCol();
    if(!da->isNumeric() || !da->is_matrix) continue;
    da->GetMinMaxScale(mm);
  }
}


//////////////////////////////////
//  taiTabularDataMimeFactory	//
//////////////////////////////////

const String taiTabularDataMimeFactory::tacss_matrixdesc("tacss/matrixdesc");
const String taiTabularDataMimeFactory::tacss_tabledesc("tacss/tabledesc");

/*
void taiTabularDataMimeFactory::AddSingleObject(QMimeData* md, taBase* obj) {
  if (!obj) return;
  QString str;
  InitHeader(1, str);
  AddHeaderDesc(obj, str);
  md->setData(tacss_objectdesc, StrToByteArray(str));
  AddObjectData(md, obj, 0);
}

void taiTabularDataMimeFactory::AddMultiObjects(QMimeData* md,
   taPtrList_impl* obj_list)
{
  if (!obj_list) return;
  QString str;
  InitHeader(obj_list->size, str);
  // note: prob not necessary, but we iterate twice so header precedes data
  for (int i = 0; i < obj_list->size; ++i) {
    taBase* obj = (taBase*)obj_list->FastEl_(i); // better damn well be a taBase list!!!
    AddHeaderDesc(obj, str);
  }
  md->setData(tacss_objectdesc, StrToByteArray(str));
  for (int i = 0; i < obj_list->size; ++i) {
    taBase* obj = (taBase*)obj_list->FastEl_(i); // better damn well be a taBase list!!!
    AddObjectData(md, obj, i);
  }
}

void taiTabularDataMimeFactory::AddObjectData(QMimeData* md, taBase* obj, int idx) {
    ostringstream ost;
    obj->Save_strm(ost);
    QString str = tacss_objectdata + ";index=" + QString::number(idx);
    md->setData(str, QByteArray(ost.str().c_str()));
}


void taiTabularDataMimeFactory::AddHeaderDesc(taBase* obj, QString& str) {
  //note: can't use Path_Long because path parsing routines don't handle names in paths
  str = str + obj->GetTypeDef()->name.toQString() + ";" +
        obj->GetPath().toQString() + ";\n"; // we put ; at end to facilitate parsing, and for }
}

void taiTabularDataMimeFactory::InitHeader(int cnt, QString& str) {
  str = QString::number(cnt) + ";\n";
}

*/

//////////////////////////////////
//  taiMatDataMimeItem 		//
//////////////////////////////////
/*
taiMatDataMimeItem::taiMatDataMimeItem(int data_type_)
{
  m_data_type = data_type_;
}

bool taiMatDataMimeItem::isMatrix() const {
  return (m_data_type == taiMimeSource::ST_MATRIX_DATA);
}
  
bool taiMatDataMimeItem::isTable() const {
  return (m_data_type == taiMimeSource::ST_TABLE_DATA);
}
  
void taiMatDataMimeItem::GetFormats_impl(QStringList& list, int) const {
  if (isMatrix())
    list.append(taiClipData::tacss_matrixdesc);
  else if (isTable())
    list.append(taiClipData::tacss_tabledesc);
}



//////////////////////////////////
//  taiRcvMatDataMimeItem 	//
//////////////////////////////////

taiRcvMatDataMimeItem::taiRcvMatDataMimeItem(int data_type_)
:inherited(data_type_)
{
  m_cols = 0;
  m_rows = 0;
  m_max_row = 1;
  m_geoms.SetBaseType(&TA_MatrixGeom);
}

void taiRcvMatDataMimeItem::DecodeMatrixDesc(String& arg) {
  // just do it all blind, because supposed to be in correct format
  String tmp = arg.before(';');
  m_cols = tmp.toInt();
  arg = arg.after(';');
  tmp = arg.before(';');
  m_rows = tmp.toInt();
  arg = arg.after(';'); 
}

void taiRcvMatDataMimeItem::DecodeTableDesc(String& arg) {
  DecodeMatrixDesc(arg);
  String tmp;
  for (int i = 0; i < m_cols; ++i) {
    tmp = arg.before(';');
    int col_cols = tmp.toInt();
    arg = arg.after(';');
    tmp = arg.before(';');
    int col_rows = tmp.toInt();
    m_max_row = MAX(m_max_row, col_rows);
    arg = arg.after(';'); 
    MatrixGeom* geom = new MatrixGeom(2, col_cols, col_rows);
    m_geoms.Add(geom);
  }
}

void taiRcvMatDataMimeItem::GetColGeom(int col, int& cols, int& rows) const {
  if (m_data_type == taiMimeSource::ST_MATRIX_DATA) {
    cols = 1;  rows = 1;
  } else {
    MatrixGeom* geom = (MatrixGeom*)m_geoms.SafeEl(col);
    if (geom) {
      cols = geom->SafeEl(0);
      rows = geom->SafeEl(1);
    } else { // bad call!
      cols = 0;  rows = 0;
    }
  }
}

*/
