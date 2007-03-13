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
#include "css_ta.h"
#include "css_basic_types.h"
#include "ta_dataproc.h"

#ifdef TA_GUI
#  include "ta_datatable_qtso.h"
#endif

#include <limits.h>
#include <float.h>
#include <ctype.h>

#include <QColor>

//////////////////////////
//   ColCalcExpr	//
//////////////////////////

void ColCalcExpr::Initialize() {
  col_lookup = NULL;
  data_cols = NULL;
}

void ColCalcExpr::Destroy() {	
  CutLinks();
}

void ColCalcExpr::InitLinks() {
  inherited::InitLinks();
  data_cols = GET_MY_OWNER(DataTableCols);
}

void ColCalcExpr::CutLinks() {
  if(col_lookup) {
    taBase::SetPointer((taBase**)&col_lookup, NULL);
  }
  data_cols = NULL;
  inherited::CutLinks();
}

void ColCalcExpr::Copy_(const ColCalcExpr& cp) {
  if(col_lookup) {
    taBase::SetPointer((taBase**)&col_lookup, NULL);
  }
  expr = cp.expr;
  UpdateAfterEdit_impl();	// gets everything
}

void ColCalcExpr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
//   Program* prg = GET_MY_OWNER(Program);
//   if(!prg || isDestroying() || prg->isDestroying()) return;
  if(col_lookup) {
    if(expr.empty())
      expr += col_lookup->name;
    else
      expr += " " + col_lookup->name;
    taBase::SetPointer((taBase**)&col_lookup, NULL);
  }
}

bool ColCalcExpr::SetExpr(const String& ex) {
  taBase::SetPointer((taBase**)&col_lookup, NULL); // justin case
  expr = ex;
  UpdateAfterEdit();		// does parse
  return true;
}

String ColCalcExpr::GetName() const {
  if(owner) return owner->GetName();
  return _nilString;
}

String ColCalcExpr::GetFullExpr() const {
  return expr;
}


//////////////////////////
//   DataCol	//
//////////////////////////

const String DataCol::udkey_width("WIDTH");
const String DataCol::udkey_narrow("NARROW");
const String DataCol::udkey_hidden("HIDDEN");

void DataCol::Initialize() {
  col_flags = DC_NONE;
  is_matrix = false;
  // default initialize to scalar
  cell_geom.SetSize(1);
  cell_geom.Set(0, 1);
}

void DataCol::InitLinks() {
  inherited::InitLinks();
  taBase::Own(cell_geom, this);
  taBase::Own(calc_expr, this);
  taMatrix* ar = AR();
  if (ar != NULL)
    taBase::Own(ar, this);
}

void DataCol::CutLinks() {
  cell_geom.CutLinks();
  calc_expr.CutLinks();
  inherited::CutLinks();
}

void DataCol::Copy_(const DataCol& cp) {
  desc = cp.desc;
  col_flags = cp.col_flags;
  is_matrix = cp.is_matrix;
  cell_geom = cp.cell_geom;
  calc_expr = cp.calc_expr;
}

void DataCol::Copy_NoData(const DataCol& cp) {
  taNBase::Copy(cp);
  Copy_(cp);
  Init();			// initialize array to new geom
}

void DataCol::CopyFromRow(int dest_row, const DataCol& src, int src_row) {
  if(src.is_matrix) {
    taMatrix* mat = ((DataCol&)src).GetValAsMatrix(src_row);
    if(mat) {
      taBase::Ref(mat);
      SetValAsMatrix(mat, dest_row);
      taBase::unRefDone(mat);
    }
  }
  else {
    SetValAsVar(src.GetValAsVar(src_row), dest_row);
  }
}

void DataCol::CopyFromRow_Robust(int dest_row, const DataCol& src, int src_row) {
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

void DataCol::Init() {
  taMatrix* ar = AR(); //cache
  if (is_matrix) {
    MatrixGeom tdim = cell_geom;
    tdim.SetSize(tdim.size + 1); // leaves the new outer dim = 0, which is flex sizing
    ar->SetGeomN(tdim);
  } else {
    ar->SetGeom(1, 0); // sets to 1-d, with flex sizing
  }
}

bool DataCol::isImage() const {
  return GetUserDataAsBool("IMAGE");
}

int DataCol::imageComponents() const {
  if (cell_geom.size <= 2) return 1;
  else return cell_geom.FastEl(2);
}

void DataCol::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Init();
}

void DataCol::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  //  cerr << name << " dcr: " << dcr << endl;
  // treat item changes here as struct changes to the table
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    DataTable* dt = dataTable();
    if (dt) {
      dt->StructUpdate(true);
      dt->StructUpdate(false);
      dt->UpdateColCalcs();
    }
  }
}

DataTable* DataCol::dataTable() {
  DataTable* rval = GET_MY_OWNER(DataTable);
  return rval;
}

void DataCol::EnforceRows(int rws) {
  taMatrix* mat = AR();
  if (!mat) return;
  mat->EnforceFrames(rws);
}

int DataCol::displayWidth() const {
  // explicit width has highest priority
  int rval = GetUserData(udkey_width).toInt();
  if (rval == 0) {
    if(isMatrix()) {
      int raw_width = 1;
      int raw_height = 1;
      Get2DCellGeom(raw_width, raw_height);
      rval = raw_width;
    }
    else {
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
  }
  if (rval == 0)
    rval = 8; // default
  // include name: not
  //  rval = MAX(rval, name.length());
  
  return rval;
}

taBase::DumpQueryResult DataCol::Dump_QuerySaveMember(MemberDef* md) {
  if (md->name == "ar") {
    // if no save, don't need to check DataTable global
    if (saveToDumpFile()) {
      DataTable* dt = dataTable();
      if (dt && !dt->HasDataFlag(DataTable::NO_SAVE_DATA)) return DQR_SAVE;
    }
    return DQR_NO_SAVE;
  } else return inherited::Dump_QuerySaveMember(md);
}

void DataCol::Get2DCellGeom(int& x, int& y) const {
  if (isMatrix()) {
    cell_geom.Get2DGeom(x, y);
  } else {
    x = 1;
    y = 1;
  }
}

void DataCol::Get2DCellGeomGui(int& x, int& y, bool odd_y, int spc) const {
  if (isMatrix()) {
    cell_geom.Get2DGeomGui(x, y, odd_y, spc);
  } else {
    x = 1;
    y = 1;
  }
}

const KeyString DataCol::key_val_type("val_type");

String DataCol::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_name) return GetDisplayName(); // override
  if (key == key_val_type) return ValTypeToStr(valType());
  else return inherited::GetColText(key, itm_idx);
}

String DataCol::GetDisplayName() const {
  return name;
}

taMatrix* DataCol::GetValAsMatrix(int row) {
  taMatrix* ar = AR(); 
  if(row < 0) row = rows() + row;
  return ar->GetFrameSlice_(row);
}

taMatrix* DataCol::GetRangeAsMatrix(int st_row, int n_rows) {
  taMatrix* ar = AR();
  if(st_row < 0) st_row = rows() + st_row;
  return ar->GetFrameRangeSlice_(st_row, n_rows);
}

const String DataCol::GetValAsString_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsStr_Flat(IndexOfEl_Flat(row, cell));
} 

const Variant DataCol::GetValAsVar_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsVar_Flat(IndexOfEl_Flat(row, cell));
} 

int DataCol::IndexOfEl_Flat(int row, int cell) const {
  if(TestError((cell < 0) || (cell >= cell_size()), "IndexOfEl_Flat",
	       "cell index out of range")) return -1;
  if(row < 0) row = rows() + row; // abs row, if request was from end
  if(TestError((row < 0 || row >= rows()), "IndexOfEl_Flat", "row out of range")) return -1;
  return (row * cell_size()) + cell;
} 

bool DataCol::SetValAsMatrix(const taMatrix* val, int row) {
  if (TestError(!val, "SetValAsMatrix", "val is null")) return false;
  if (row < 0) row = rows() + row; // abs row, if request was from end
  //note: the mat function does most of the parameter checking
  return AR()->CopyFrame(*val, row);
}


bool DataCol::SetValAsString_impl(const String& val, int row, int cell) {
  AR()->SetFmStr_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
} 

bool DataCol::SetValAsVar_impl(const Variant& val, int row, int cell) {
  AR()->SetFmVar_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
} 

bool DataCol::GetMinMaxScale(MinMax& mm) {
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
  else if(valType() == VT_INT) {
    int_Matrix* fm = (int_Matrix*)AR();
    if(fm->size > 0) {
      mm.min = fm->FastEl_Flat(0);
      mm.max = fm->FastEl_Flat(0);
      for(int i=1;i<fm->size;i++) {
	int val = fm->FastEl_Flat(i);
	if(val > mm.max) mm.max = val;
	if(val < mm.min) mm.min = val;
      }
    }
  }
  else {
    return false;		// not worth it!
  }
  return true;
}


String DataCol::EncodeHeaderName(int d0, int d1, int d2, int d3, int d4) {
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

void DataCol::DecodeHeaderName(String nm, String& base_nm, int& vt,
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
  SetBaseType(&TA_DataColTp);
}

void DataTableCols::Copy_NoData(const DataTableCols& cp) {
  Reset();	// get rid of ours
  for(int i=0;i<cp.size; i++) {
    DataCol* sda = cp.FastEl(i);
    DataCol* nda = (DataCol*)sda->MakeToken();
    Add(nda);
    nda->Copy_NoData(*sda);
  }
}

void DataTableCols::CopyFromRow(int dest_row, const DataTableCols& src, int src_row) {
  int mx = MIN(size, src.size);
  for(int i=0; i< mx; i++) {
    DataCol* dar = FastEl(i);
    DataCol* sar = src.FastEl(i);
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
  else if (key == DataCol::key_val_type) return "Data Type";
  else return inherited::GetColHeading(key);
}

const KeyString DataTableCols::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_name;
  case 1: return DataCol::key_val_type;
  default: return _nilKeyString;
  }
}


//////////////////////////
//	DataTable	//
//////////////////////////

int DataTable::idx_def_arg = 0;

void DataTable::Initialize() {
  rows = 0;
  data_flags = DF_NONE;
  save_data = true;		// todo: remove
  m_dm = NULL; // returns new if none exists, or existing -- enables views to be shared
  calc_script = NULL;
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
  if(calc_script) {
    delete calc_script;
    calc_script = NULL;
  }
  inherited::CutLinks();
}

void DataTable::Copy_(const DataTable& cp) {
  data = cp.data;
  rows = cp.rows;
  data_flags = cp.data_flags;
}

void DataTable::Copy_NoData(const DataTable& cp) {
  rows = 0;
  // don't copy the flags!
//   data_flags = cp.data_flags;
  data.Copy_NoData(cp.data);
}

void DataTable::CopyFromRow(int dest_row, const DataTable& src, int src_row) {
  data.CopyFromRow(dest_row, src.data, src_row);
}


bool DataTable::CopyColRow(int dest_col, int dest_row, const DataTable& src, int src_col, int src_row) {
  DataCol* dar = data.SafeEl(dest_col);
  DataCol* sar = src.data.SafeEl(src_col);
  if(TestError(!dar || !sar, "CopyColRow", "column(s) out of range, not copied!"))
    return false;
  dar->CopyFromRow_Robust(dest_row, *sar, src_row);
  return true;
}

void DataTable::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!save_data) {
    SetDataFlag(NO_SAVE_DATA);	// todo: remove this -- backward compat -- only used if false (non default)
    save_data = true;
  }
  UniqueColNames();
  // the following is likely redundant:
  //  UpdateColCalcs();
  CheckForCalcs();
}

bool DataTable::AddRows(int n) {
  if(TestError((cols() == 0), "AddRows", "no columns!")) return false;
  if(TestError((n < 1), "AddRows", "n rows < 1")) return false;
  RowsAdding(n, true);
  for(int i=0;i<data.size;i++) {
    DataCol* ar = data.FastEl(i);
    ar->EnforceRows(ar->rows() + n);
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

int DataTable::Dump_Load_Value(istream& strm, TAPtr par) {
  int c = inherited::Dump_Load_Value(strm, par);
  if (c == EOF) return EOF;
  if (c == 2) return 2; // signal that it was just a path
  // otherwise, if data was loaded, we need to set the rows
  if(!HasDataFlag(NO_SAVE_DATA) && save_data) { // todo: remove save_data part -- obs
    int i;
    DataCol* col;
    for (i = 0; i < cols(); ++i) {
      col = GetColData(i);
      if (!col->saveToDumpFile()) continue;
      int frms = col->AR()->frames();
      // number of rows is going to be = biggest number in individual cols
      rows = max(rows, frms);
    }
  }
  return c;
}

DataCol* DataTable::GetColData(int col) const {
  if(TestError((col < 0 || col >= cols()), "GetColData",
	       "column number is out of range")) return NULL;
  else return data.SafeEl(col);
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

taMatrix* DataTable::GetColMatrix(int col) const {
  DataCol* da = GetColData(col);
  if (da) return da->AR();
  else return NULL;
}

const Variant DataTable::GetColUserData(const String& name,
  int col) const 
{
  DataCol* da = GetColData(col);
  if (da) return da->GetUserData(name);
  else return _nilVariant;
}

void DataTable::SetColUserData(const String& name,
  const Variant& value, int col)
{
  DataCol* da = GetColData(col);
  if (da) da->SetUserData(name, value);
}


DataTableModel* DataTable::GetDataModel() {
  if (!m_dm) {
    m_dm = new DataTableModel(this);
  }
  return m_dm;
}

taMatrix* DataTable::GetMatrixData_impl(int chan) {
  DataCol* da = GetColData(chan);
  if(!da) return NULL;		// err msg already given
  int i;
  if(TestError(!da->is_matrix, "GetMatrixData_impl", "column is not a matrix")) return NULL;
  if(TestError(!idx(rd_itr, da->rows(), i), "GetMatrixData_impl",
	       "read index is out of range")) return NULL;
  return da->AR()->GetFrameSlice_(i);
}

/////////////////////

double DataTable::GetValAsDouble(int col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da && idx_err(row, da->rows(), i))
    return da->GetValAsDouble(i);
  else return 0.0f;
}

float DataTable::GetValAsFloat(int col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsFloat(i);
  else return 0.0f;
}

const String DataTable::GetValAsString(int col, int row) const {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsString(i);
  else return "n/a";
}

const Variant DataTable::GetValAsVar(int col, int row) const {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsVar(i);
  else return _nilVariant;
}

taMatrix* DataTable::GetValAsMatrix(int col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsMatrix(i);
  else return NULL;
}

taMatrix* DataTable::GetRangeAsMatrix(int col, int st_row, int n_rows) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(st_row, da->rows(), i))
    return da->GetRangeAsMatrix(i, n_rows);
  else return NULL;
}

////////////////////////////

bool DataTable::SetValAsDouble(double val, int col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsDouble(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloat(float val, int col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsFloat(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsString(const String& val, int col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsString(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsVar(const Variant& val, int col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsVar(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsMatrix(const taMatrix* val, int col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    return da->SetValAsMatrix(val, i);
  } else return false;
}

////////////////////////

double DataTable::GetValAsDoubleM(int col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsDoubleM(i, cell);
  else return 0.0f;
}

float DataTable::GetValAsFloatM(int col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsFloatM(i, cell);
  else return 0.0f;
}

const String DataTable::GetValAsStringM(int col, int row, int cell, bool na) const {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsStringM(i, cell);
  else 
    return (na) ? String("n/a") : _nilString;
}

const Variant DataTable::GetValAsVarM(int col, int row, int cell) const {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsVarM(i, cell);
  else return _nilVariant;
}

////////////////////////

bool DataTable::SetValAsDoubleM(double val, int col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsDoubleM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloatM(float val, int col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsFloatM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsStringM(const String& val, int col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsStringM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsVarM(const Variant& val, int col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsVarM(val, i, cell);
    return true;
  } else return false;
}

////////////////////////

bool DataTable::hasData(int col, int row) {
  DataCol* da = GetColData(col);
  int i;
  return (da && idx(row, da->rows(), i));
}


void DataTable::MarkCols() {
  for(int i=0;i<data.size;i++) {
    DataCol* da = data.FastEl(i);
    da->SetColFlag(DataCol::MARK);
  }
}

int DataTable::MaxLength() {
  return rows;
}

int DataTable::MinLength() {
  if (cols() == 0) return 0;
  int min = INT_MAX;
  for(int i=0;i<data.size;i++) {
    DataCol* ar = data.FastEl(i);
    min = MIN(min,ar->AR()->frames());
  }
  return min;
}

DataCol* DataTable::NewCol(DataCol::ValType val_type, 
				  const String& col_nm) 
{
  StructUpdate(true);
  DataCol* rval = NewCol_impl(val_type, col_nm);
  rval->Init(); // asserts geom
  rval->EnforceRows(rows);	// new guys always get same # of rows as current table
  StructUpdate(false);
  return rval;
}

DataCol* DataTable::NewCol_impl(DataCol::ValType val_type, 
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
  DataCol* rval = (DataCol*) data.New(1, td);
  rval->name = col_nm;
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
  rval->DataChanged(DCR_ITEM_UPDATED); // because we set name after creation
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

DataCol* DataTable::NewColMatrix(DataCol::ValType val_type, const String& col_nm,
    int dims, int d0, int d1, int d2, int d3, int d4)
{
  if(dims < 1) {		// < 1 is shortcut for not actually a matrix!
    return NewCol(val_type, col_nm);
  }
  MatrixGeom geom(dims, d0, d1, d2, d3, d4);
  String err_msg;
  if(TestError(!taMatrix::GeomIsValid(geom, &err_msg), "NewcolMatrix",
	       "Invalid geom:", err_msg)) return NULL;
  
  DataCol* rval = NewColMatrixN(val_type, col_nm, geom);
  return rval;
}

DataCol* DataTable::NewColMatrixN(DataCol::ValType val_type, 
  const String& col_nm, const MatrixGeom& cell_geom) 
{
  StructUpdate(true);
  DataCol* rval = NewCol_impl(val_type, col_nm);
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
  DataCol* da = GetColData(col);
  if(da) da->name = col_nm;
}

bool DataTable::RenameCol(const String& cur_nm, const String& new_nm) {
  DataCol* da = FindColName(cur_nm);
  if(TestError(!da, "RenameCol", "column named", cur_nm, "not found")) return false;
  da->name = new_nm;
  return true;
}

DataCol* DataTable::FindColName(const String& col_nm, int& col_idx, bool err_msg) {
  DataCol* da = data.FindName(col_nm, col_idx);
  TestError(!da && err_msg, "FindColName",  "could not find column named:", col_nm);
  return da;
}

DataCol* DataTable::FindMakeColName(const String& col_nm, int& col_idx,
					   ValType val_type, int dims,
					   int d0, int d1, int d2, int d3, int d4) {
  DataCol* da = FindColName(col_nm, col_idx);
  if(da) {
    if(da->valType() != (ValType)val_type) {
      StructUpdate(true);
      DataCol* nda;
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
      MatrixGeom mg(dims, d0, d1, d2, d3, d4);
      if(mg != da->cell_geom) {
	StructUpdate(true);
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

void DataTable::RemoveCol(int col) {
  DataCol* da = data.SafeEl(col);
  if(TestError(!da, "RemoveCol", "column number not found")) return;
  StructUpdate(true);
  da->Close();
  StructUpdate(false);
  //note: you can possibly get left with completely NULL rows,
  // but we don't renormalize
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
  if(st_row < 0) st_row = rows;	// end
  if(TestError((st_row < 0 || st_row > rows), "InsertRows",
	       "row not in range:",String(st_row))) return false;
  bool rval = true;
  DataUpdate(true);// only data because for views, no change in column structure
  if (m_dm)
    m_dm->beginInsertRows(QModelIndex(), st_row, st_row + n_rows-1);
  for(int i=0;i<data.size;i++) {
    DataCol* ar = data.FastEl(i);
    if(!ar->AR()->InsertFrames(st_row, n_rows))
      rval = false;
  }
  if(rval)
    rows += n_rows;
  if (m_dm)
    m_dm->endInsertRows();
  DataUpdate(false); 
  return rval;
}

bool DataTable::RemoveRows(int st_row, int n_rows) {
  if(TestError(!RowInRangeNormalize(st_row), "RemoveRows",
	       "start row not in range:",String(st_row)))
    return false;
  int end_row = st_row + n_rows-1;
  if(TestError(!RowInRangeNormalize(end_row), "RemoveRows",
	       "end row not in range:",String(end_row)))
    return false;
  DataUpdate(true);		// only data because for views, no change in column structure
  if (m_dm) m_dm->beginRemoveRows(QModelIndex(), st_row, end_row);
  
  for(int i=0;i<data.size;i++) {
    DataCol* ar = data.FastEl(i);
    int act_row;
    if (idx(st_row, ar->AR()->frames(), act_row))
      ar->AR()->RemoveFrames(act_row, n_rows);
  }
  rows -= n_rows;
  
  if (m_dm) m_dm->endRemoveRows();
  DataUpdate(false);
  return true;
}

bool DataTable::DuplicateRow(int row_no, int n_copies) {
  if(TestError(!RowInRangeNormalize(row_no), "DuplicateRow",
	       "row not in range:",String(row_no)))
    return false;
  DataUpdate(true);// only data because for views, no change in column structure
  for(int k=0;k<n_copies;k++) {
    AddBlankRow();
    for(int j=0;j<data.size;j++) {
      DataCol* sda = data[j];
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
    DataCol* ar = data.FastEl(i);
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
    DataCol* da = data.FastEl(i);
    if(!da->saveToDataFile()) continue;
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
    DataCol* da = data.FastEl(i);
    if(!da->saveToDataFile()) continue;
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
  taFiler* flr = GetSaveFiler(fname, ".dat", false, "Data");
  if(flr->ostrm)
    SaveDataRow_strm(*flr->ostrm, row, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveData(const String& fname, Delimiters delim, bool quote_str) {
  taFiler* flr = GetSaveFiler(fname, ".dat", false, "Data");
  if(flr->ostrm)
    SaveData_strm(*flr->ostrm, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::AppendData(const String& fname, Delimiters delim, bool quote_str) {
  taFiler* flr = GetAppendFiler(fname, ".dat", false, "Data");
  if(flr->ostrm)
    SaveDataRows_strm(*flr->ostrm, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveDataLog(const String& fname, bool append, bool dmem_proc_0) {
  if(!log_file) return;		// shouldn't happen
  if(log_file->isOpen())
    log_file->Close();
#ifdef DMEM_COMPILE
  if((taMisc::dmem_proc > 0) && dmem_proc_0) return; // don't open!
#endif
  log_file->setFileName(fname);
  if(fname.empty()) {
    if(append) 
      log_file->Append();
    else {
      log_file->SaveAs();
      if(log_file->isOpen())
	SaveHeader_strm(*log_file->ostrm);
    }
  }
  else {
    if(append) 
      log_file->open_append();
    else {
      log_file->open_write();
      if(log_file->isOpen())
	SaveHeader_strm(*log_file->ostrm);
    }
  }
}

void DataTable::CloseDataLog() {
  if(!log_file) return;
  log_file->Close();
}

bool DataTable::WriteDataLogRow() {
  if(log_file && log_file->isOpen()) {
    SaveDataRow_strm(*log_file->ostrm);
    return true;
  }
  return false;
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
  while(((c = strm.get()) != EOF) && (c != '\n') && (c != '\r') && !((c == delim) && (depth <= 0))) {
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
      if(c == '\r') { if(strm.peek() == '\n') strm.get(); break; }
      continue;			// for some reason it is empty
    }
    String base_nm;
    int val_typ;
    MatrixGeom mat_idx;
    MatrixGeom mat_geom;
    DataCol::DecodeHeaderName(str, base_nm, val_typ, mat_idx, mat_geom);
    int idx;
    DataCol* da = FindColName(base_nm, idx);
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
    if(c == '\r') { if(strm.peek() == '\n') strm.get(); break; }
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
    if(TestError((use_col >= data.size), "LoadDataRow_strm", "columns exceeded!")) {
      c = EOF;
      break;
    }
    DataCol* da = data.FastEl(use_col);
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
    if(c == '\r') { if(strm.peek() == '\n') strm.get(); break; }
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
  taFiler* flr = GetLoadFiler(fname, ".dat", false, "Data");
  int rval = 0;
  if(flr->istrm)
    rval = LoadDataRow_strm(*flr->istrm, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void DataTable::LoadData(const String& fname, Delimiters delim, bool quote_str, int max_recs) {
  taFiler* flr = GetLoadFiler(fname, ".dat", false, "Data");
  if(flr->istrm)
    LoadData_strm(*flr->istrm, delim, quote_str, max_recs);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::WriteClose_impl() {
  UpdateAllViews();
  WriteDataLogRow();
  CalcLastRow();
}

//////////////////////////////////////////////////////////////////////////////
///		Calculating columns

bool DataTable::UpdateColCalcs() {
  if(!CheckForCalcs()) return false;
  if(HasDataFlag(NO_AUTO_CALC)) return false;
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

void DataTable::Sort(DataCol* col1, bool ascending1,
		     DataCol* col2, bool ascending2,
		     DataCol* col3, bool ascending3) {

  DataSortSpec spec;
  if(col1) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col1->name;
    if(ascending1) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  if(col2) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col2->name;
    if(ascending2) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  if(col3) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col3->name;
    if(ascending3) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  
  taDataProc::Sort_impl(this, &spec);
}

bool DataTable::Filter(const String& filter_expr) {
  if(TestError(filter_expr.empty(), "Filter",
	       "empty filter expression -- must specify a filter condition!"))
    return false;
  InitCalcScript();
  calc_script->ClearAll();

  STRING_BUF(code_str, 2048);
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
  code_str += "this.RemoveRows(row,1);\n";		     // else remove
  CalcRowCodeGen(code_str);
  bool ok = calc_script->CompileCode(code_str);
  if(TestError(!ok, "Filter", "error in filter expression, see console for errors"))
    return false;
  calc_script->Run();
  return true;
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
  AddRows(n_recv_rows);		// make room for new ones

  static char_Array char_send;
  static char_Array char_recv;
  const int max_str_len = 1024;

  for(int i=0;i<data.size; i++) {
    DataCol* da = data.FastEl(i);
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
  RemoveRows(st_send_row, n_rows);
  DataUpdate(false);
#endif  // DMEM_COMPILE
}

//////////////////////////////////
//   DataTableModel		//
//////////////////////////////////

DataTableModel::DataTableModel(DataTable* owner) 
:inherited(NULL)
{
  dt = owner;
  if (dt)
    dt->AddDataClient(this);
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

void DataTableModel::DataLinkDestroying(taDataLink* dl) {
  delete this;
}

void DataTableModel::DataDataChanged(taDataLink* dl, int dcr,
  void* op1, void* op2)
{
  //this is primarily for code-driven changes
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    emit_dataChanged();
  }
  else if (dcr == DCR_STRUCT_UPDATE_END) { // for col insert/deletes
    emit_layoutChanged();
  }
}



QVariant DataTableModel::data(const QModelIndex& index, int role) const {
  if (!dt || !index.isValid()) return QVariant();
  //NOTES:
  // * it would be nice to just italicize the "matrix" text, but we have no
  //   no access to the font being used, and cannot only pass modifiers
  
  DataCol* col = dt->GetColData(index.column());
  // if no col, we really don't care about anything else...
  if (!col) return QVariant(); // nil
  
  switch (role) {
  case Qt::DisplayRole: //note: we may choose to format different for display, ex floats
  case Qt::EditRole: {
    if (col->is_matrix) 
      return QVariant("(matrix)"); // user clicks to edit, or elsewise displayed
    else {
      int dx;
      if(dt->idx(index.row(), col->rows(), dx))
	return col->GetValAsString(dx);
      else
	return QVariant();	// nil
    }
  }
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

void DataTableModel::emit_dataChanged(int row_fr, int col_fr, int row_to, int col_to) {
  if (!dt) return;
  // lookup actual end values when we are called with sentinels
  if (row_to < 0) row_to = rowCount() - 1;
  if (col_to < 0) col_to = columnCount() - 1;  
  
  emit dataChanged(createIndex(row_fr, col_fr), createIndex(row_to, col_to));
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
      DataCol* col = dt->GetColData(index.column());
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
    DataCol* col = dt->GetColData(section);
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
  
  DataCol* col = dt->GetColData(index.column());
  // if no col, we really don't care about anything else...
  if (!col) return false; 
  //we restrict setData for scalars only -- use delegate for matrix
  if (col->is_matrix) return false;
  
  bool rval = false;
  switch (role) {
  case Qt::EditRole:
    dt->SetValAsVar(value, index.column(), index.row());
    emit dataChanged(index, index);
    col->DataChanged(DCR_ITEM_UPDATED); // for calc refresh
    rval = true;
  default: break;
  }
  return rval;
}

bool DataTableModel::ValidateIndex(const QModelIndex& index) const {
  if (!dt) return false;
  return (index.isValid() && (index.row() < dt->rows) && (index.column() < dt->cols()));
}

