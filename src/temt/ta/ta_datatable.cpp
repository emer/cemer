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
//     DataCol  	//
//////////////////////////

const String DataCol::udkey_width("WIDTH");
const String DataCol::udkey_narrow("NARROW");
const String DataCol::udkey_hidden("HIDDEN");

void DataCol::Initialize() {
  col_flags = (ColFlags)(SAVE_ROWS | SAVE_DATA);
  col_idx = -1;
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

void DataCol::Copy_Common_(const DataCol& cp) {
  // things common to full and schema-only copy
  name = cp.name; // this is the default we want for DataCol
  desc = cp.desc;
  col_flags = cp.col_flags;
  is_matrix = cp.is_matrix;
  cell_geom = cp.cell_geom;
  calc_expr = cp.calc_expr;
}

void DataCol::Copy_(const DataCol& cp) {
  Copy_Common_(cp);
}

void DataCol::CopyFromCol_Robust(const DataCol& cp) {
  // note: caller has asserted Struct guys, but ok for us to do it again
  // assumes copy has been validated
  StructUpdate(true);
  inherited::Copy_impl(cp); // do all common generic class copying
  SetBaseFlag(COPYING); // note: still have to set/reset here, because not nestable
  // assume name already copied, else don't want it overwritten
  // don't do these -- just the data!!
//   desc = cp.desc;
//   col_flags = cp.col_flags;
//   calc_expr = cp.calc_expr;

  //NOTE: we only copy data, leaving geom and type as is (since we can't change anyway)
  int rows = cp.rows();
  EnforceRows(rows);
  for (int i = 0; i < rows; ++i) {
    CopyFromRow_Robust(i, cp, i);
  }  
  ClearBaseFlag(COPYING);
  StructUpdate(false);
}

void DataCol::Copy_NoData(const DataCol& cp) {
  StructUpdate(true);
  inherited::Copy_impl(cp);
  SetBaseFlag(COPYING);
  Copy_Common_(cp);
  Init(); //note: table should have no rows, so won't make any
  ClearBaseFlag(COPYING);
  StructUpdate(false);			// initialize array to new geom
}

void DataCol::CopyFromRow(int dest_row, const DataCol& src, int src_row) {
  if (dest_row < 0) dest_row = rows() + dest_row; // abs row, if request was from end
  if (src_row < 0) src_row = src.rows() + src_row; // abs row, if request was from end
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
  int rows = 0; // rows, based on table (not our frames, which may have changed)
  DataTable* tab = dataTable();
  if (tab) rows = tab->rows;
  if (is_matrix) {
    MatrixGeom tdim = cell_geom;
    tdim.SetSize(tdim.size + 1);
    tdim.Set(tdim.size-1, rows);
    ar->SetGeomN(tdim);
  } else {
    ar->SetGeom(1, rows);
  }
  // transfer READ_ONLY to mat
  ar->ChangeBaseFlag(BF_GUI_READ_ONLY, (col_flags & READ_ONLY));
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
  if (is_matrix) // autokey only allowed on scalar cols
    col_flags = (ColFlags)(col_flags & ~AUTO_KEY);
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

void DataCol::ChangeColType(ValType new_type) {
  if (valType() == new_type) return;
  MatrixGeom cell_geom;
  if (is_matrix) cell_geom = this->cell_geom; // because we will be nuked
  dataTable()->ChangeColTypeGeom_impl(this, new_type, cell_geom);
  //NOTE: no more code here, because we've probably been deleted/replaced
}

void DataCol::ChangeColCellGeom(const MatrixGeom& new_geom) {
  if ((!is_matrix && (new_geom.dims() == 0)) ||
    cell_geom.Equal(new_geom)) return;
  dataTable()->ChangeColTypeGeom_impl(this, valType(), new_geom);
  //NOTE: no more code here, because we may have been deleted/replaced
}

void DataCol::ChangeColCellGeomNs(int dims, int d0, int d1, int d2, int d3,
				  int d4, int d5, int d6) {
  MatrixGeom mg;
  mg.SetGeom(dims, d0, d1, d2, d3, d4, d5, d6);
  ChangeColCellGeom(mg);
}

void DataCol::ChangeColMatToScalar() {
  if (!is_matrix) return;
  MatrixGeom new_geom; //note: 0 dims is key to change to scalar
  dataTable()->ChangeColTypeGeom_impl(this, valType(), new_geom);
  //NOTE: no more code here, because we may have been deleted/replaced
}

DataTable* DataCol::dataTable() {
  DataTable* rval = GET_MY_OWNER(DataTable);
  return rval;
}

bool DataCol::EnforceRows(int rws) {
  taMatrix* mat = AR();
  if (!mat) return false;
  bool rval = false;
  if (col_flags & AUTO_KEY) {
    int a_rows = mat->frames();
    rval = mat->EnforceFrames(rws);
    int b_rows = mat->frames();
    for (int i = a_rows; i < b_rows; ++i) {
      SetVal(++(dataTable()->keygen), i);
    }
  } else {
    rval = mat->EnforceFrames(rws);
  }
  return rval;
}

bool DataCol::InsertRows(int st_row, int n_rows) {
  taMatrix* mat = AR();
  if (!mat) return false;
  bool rval = mat->InsertFrames(st_row, n_rows);
  if (!rval) return rval;
  if (col_flags & AUTO_KEY) {
    for (int i = st_row; i < st_row + n_rows; ++i) {
      SetVal(++(dataTable()->keygen), i);
    }
  }
  return rval;
}  

int DataCol::FindVal(const Variant& val, int st_row) const {
  if(TestError(isMatrix(), "FindVal", "column must be scalar, not matrix")) return -1;
  if(st_row >= 0) {
    for(int i=st_row; i<rows(); i++) {
      if(GetVal(i) == val) return i;
    }
    return -1;
  }
  else {
    for(int i=rows()-st_row; i>=0; i--) {
      if(GetVal(i) == val) return i;
    }
    return -1;
  }
}

String DataCol::ColStats() {
  if(valType() == VT_DOUBLE) {
    return taMath_double::vec_stats((double_Matrix*)AR());
  }
  else if(valType() == VT_FLOAT) {
    return taMath_float::vec_stats((float_Matrix*)AR());
  }
  else if(valType() == VT_INT) {
    float_Matrix float_tmp(false);
    int_Matrix* mat = (int_Matrix*)AR();
    taMath_float::vec_fm_ints(&float_tmp, mat);
    return taMath_float::vec_stats(&float_tmp);
  }
  TestError(true, "ColStats", "column data type is not double, float or int");
  return _nilString;
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
//       if(taMisc::is_undo_saving) return DQR_NO_SAVE; // don't save rows for general undo!
      // with the super diff undo, we can now deal with data tables too!
      DataTable* dt = dataTable();
      if (dt && dt->HasDataFlag(DataTable::SAVE_ROWS)) return DQR_SAVE;
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

/////////////////////
// Access Index

int DataCol::IndexOfEl_Flat(int row, int cell) const {
  if(TestError((cell < 0) || (cell >= cell_size()), "IndexOfEl_Flat",
	       "cell index out of range")) return -1;
  if(row < 0) row = rows() + row; // abs row, if request was from end
  if(TestError((row < 0 || row >= rows()), "IndexOfEl_Flat", "row out of range")) return -1;
  return (row * cell_size()) + cell;
} 

int DataCol::IndexOfEl_Flat_Dims(int row, int d0, int d1, int d2, int d3) const {
  if(row < 0) row = rows() + row; // abs row, if request was from end
  if(TestError((row < 0 || row >= rows()), "IndexOfEl_Flat", "row out of range")) return -1;
  switch(cell_geom.size) {
  case 0:
    return AR()->SafeElIndex(row);
  case 1:
    return AR()->SafeElIndex(d0, row);
  case 2:
    return AR()->SafeElIndex(d0, d1, row);
  case 3:
    return AR()->SafeElIndex(d0, d1, d2, row);
  case 4:
    return AR()->SafeElIndex(d0, d1, d2, d3, row);
  }
  return -1;
} 

/////////////////////
// Access Var/String

const String DataCol::GetValAsString_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsStr_Flat(IndexOfEl_Flat(row, cell));
} 

const Variant DataCol::GetValAsVar_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsVar_Flat(IndexOfEl_Flat(row, cell));
} 

bool DataCol::SetValAsString_impl(const String& val, int row, int cell) {
  AR()->SetFmStr_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
} 

bool DataCol::SetValAsVar_impl(const Variant& val, int row, int cell) {
  AR()->SetFmVar_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
} 

bool DataCol::InitVals(const Variant& init_val)  {
  AR()->InitValsFmVar(init_val);
  return true;
} 

bool DataCol::InitValsToRowNo()  {
  if(TestError(is_matrix, "InitValsToRowNo", "column is a matrix")) return false;
  for(int i=0; i<rows(); i++) {
    SetValAsInt(i, i);
  }
  return true;
} 

////////////////////
// Access: Matrix

taMatrix* DataCol::GetValAsMatrix(int row) {
  taMatrix* ar = AR(); 
  if(row < 0) row = rows() + row;
  taMatrix* rval = ar->GetFrameSlice_(row);
  if (col_flags & READ_ONLY)
    rval->SetBaseFlag(BF_READ_ONLY | BF_GUI_READ_ONLY);
  return rval;
}

taMatrix* DataCol::GetRangeAsMatrix(int st_row, int n_rows) {
  taMatrix* ar = AR();
  if(st_row < 0) st_row = rows() + st_row;
  taMatrix* rval = ar->GetFrameRangeSlice_(st_row, n_rows);
  if (col_flags & READ_ONLY)
    rval->SetBaseFlag(BF_READ_ONLY | BF_GUI_READ_ONLY);
  return rval;
}

bool DataCol::SetValAsMatrix(const taMatrix* val, int row) {
  if (TestError(!val, "SetValAsMatrix", "val is null")) return false;
  if (row < 0) row = rows() + row; // abs row, if request was from end
  //note: the mat function does most of the parameter checking
  return AR()->CopyFrame(*val, row);
}

//////////////////

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


String DataCol::EncodeHeaderName(const MatrixGeom& dims) const {
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
    mat_info = dims.GeomToString("[", "]");
    if(cell_geom.IndexFmDims(dims) == 0) { // first guy
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
  } /*no: caller must set default else {
    vt = VT_FLOAT;
  }*/
  
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

  if ((dcr >= DCR_LIST_ORDER_MIN) && (dcr <= DCR_LIST_ORDER_MAX)) {
    DataTable* dt = GET_MY_OWNER(DataTable); // cache
    if (!dt) return;
    // treat changes here as Struct changes on the table
    dt->StructUpdate(true);
    dt->StructUpdate(false);
  }
  
}

String DataTableCols::GetColHeading(const KeyString& key) const {
  if (key == key_name) return "Col Name"; // override
  else if (key == DataCol::key_val_type) return "Data Type";
  else if (key == key_desc) return "Description";
  else return inherited::GetColHeading(key);
}

const KeyString DataTableCols::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_name;
  case 1: return DataCol::key_val_type;
  case 2: return key_desc;
  default: return _nilKeyString;
  }
}


//////////////////////////
//  FixedWidthColSpec	//
//////////////////////////

void FixedWidthColSpec::Initialize() {
  start_col = 1;
  col_width = 0;
  col = NULL;
}

void FixedWidthColSpec::WriteData(const String& val) {
  if (col)
    col->SetValAsString(val, -1);
}


//////////////////////////
//  FixedWidthSpec	//
//////////////////////////

void FixedWidthColSpec_List::Initialize() {
  SetBaseType(&TA_FixedWidthColSpec);
}

void FixedWidthSpec::Load_Init(DataTable* dat_) {
  dat = dat_;
  for (int i = 0; i < col_specs.size; ++i) {
    FixedWidthColSpec* fws = col_specs.FastEl(i);
    fws->col = dat->FindColName(fws->name);
    TestError(!fws->col, "Load_Init", "col name not in table:",
      fws->name); 
  }
}

void FixedWidthSpec::Initialize() {
  n_skip_lines = 0;
  dat = NULL;
}

void FixedWidthSpec::AddRow(const String& ln) {
  dat->AddBlankRow();
  for (int i = 0; i < col_specs.size; ++i) {
    FixedWidthColSpec* fws = col_specs.FastEl(i);
    if (!fws) continue;
    String s = ln.from(fws->start_col - 1);
    if (fws->col_width >= 0)
      s.truncate(fws->col_width);
    fws->WriteData(s); 
  }
}



//////////////////////////
//	DataTable	//
//////////////////////////

void DataTable::Initialize() {
  rows = 0;
  data_flags = (DataFlags)(SAVE_ROWS | AUTO_CALC);
  auto_load = NO_AUTO_LOAD;
  keygen.setType(Variant::T_Int64);
  calc_script = NULL;
  log_file = NULL;
  table_model = NULL;
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
  auto_load = cp.auto_load;
  auto_load_file = cp.auto_load_file;
  keygen = cp.keygen;
}

void DataTable::Copy_NoData(const DataTable& cp) {
  // note: CANNOT just set rows=0, because we must reclaim mat space (ex strings)
  // and must insure data model is sync'ed propery
  //  ResetData();
  RemoveAllCols();
  // don't copy the flags!
//   data_flags = cp.data_flags;
  data.Copy_NoData(cp.data);
}

void DataTable::CopyFromRow(int dest_row, const DataTable& src, int src_row) {
  data.CopyFromRow(dest_row, src.data, src_row);
}


bool DataTable::CopyCell(Variant dest_col, int dest_row, const DataTable& src,
  Variant src_col, int src_row) 
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

void DataTable::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  data.CheckConfig(quiet, rval);
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
  if(HasDataFlag(SAVE_ROWS)) return false;
  if(auto_load == NO_AUTO_LOAD) return false;
  
  if(taMisc::gui_active && (auto_load == PROMPT_LOAD)) {
    int chs = taMisc::Choice("Load data file: " + auto_load_file + " into data table: " + name, "Yes", "No");
    if(chs == 1) return false;
  }

  if(TestError(auto_load_file.empty(), "AutoLoadData", "auto_load_file is empty!"))
    return false;
  if(TestError(access(auto_load_file, F_OK) != 0, "AutoLoadData",
	       "could not access auto_load_file:", auto_load_file))
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
    LoadData(auto_load_file);
  }
  return true;
}

bool DataTable::AutoSaveData() {
  if(HasDataFlag(SAVE_ROWS) || !HasDataFlag(SAVE_FILE)) return false;
  
  if(TestError(auto_load_file.empty(), "AutoSaveData", "auto_load_file is empty!"))
    return false;
  // can't save to .dtbl type because that would recurse us -- TODO: could workaround somehow
  if (TestError(auto_load_file.contains(".dtbl"), "AutoSaveData", "auto_load_file can only be of .dat type, not .dtbl type, sorry!")) {
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

int DataTable::Dump_Load_Value(istream& strm, TAPtr par) {
  int c = inherited::Dump_Load_Value(strm, par);
  if (c == EOF) return EOF;
  if (c == 2) return 2; // signal that it was just a path
  // otherwise, if data was loaded, we need to set the rows
  rows = 0;
  for (int i = 0; i < cols(); ++i) {
    DataCol* col = data.FastEl(i);
    int frms = col->AR()->frames();
    // number of rows is going to be = biggest number in individual cols
    rows = max((int)rows, frms);
  }
  return c;
}

void DataTable::Dump_Save_pre() {
  inherited::Dump_Save_pre();
  AutoSaveData();
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


taMatrix* DataTable::GetMatrixData_impl(int chan) {
  DataCol* da = GetColData(chan);
  if(!da) return NULL;		// err msg already given
  int i;
  if(TestError(!da->is_matrix, "GetMatrixData_impl", "column is not a matrix")) return NULL;
  if(TestError(!idx(rd_itr, da->rows(), i), "GetMatrixData_impl",
	       "read index is out of range -- need to set with ReadItem or ReadFirst/Next")) return NULL;
  return da->AR()->GetFrameSlice_(i);
}

///////////////////////////////////////////
//	Basic Access/Modify with Variant col spec

const Variant DataTable::GetVal(Variant col, int row) const {
  if(col.isStringType())
    return GetValColName(col.toString(), row);
  return GetValAsVar(col.toInt(), row);
}

bool DataTable::SetVal(const Variant& val, Variant col, int row) {
  if(col.isStringType())
    return SetValColName(val, col.toString(), row);
  return SetValAsVar(val, col.toInt(), row);
}


const Variant DataTable::GetMatrixVal(Variant col, int row,
				      int d0, int d1, int d2, int d3) const {
  if(col.isStringType())
    return GetMatrixValColName(col.toString(), row, d0, d1, d2, d3);
  return GetValAsVarMDims(col.toInt(), row, d0, d1, d2, d3);
}

bool DataTable::SetMatrixVal(const Variant& val, Variant col, int row,
			     int d0, int d1, int d2, int d3)  {
  if(col.isStringType())
    return SetMatrixValColName(val, col.toString(), row, d0, d1, d2, d3);
  return SetValAsVarMDims(val, col.toInt(), row, d0, d1, d2, d3);
}

const Variant DataTable::GetMatrixFlatVal(Variant col, int row, int cell) const {
  if(col.isStringType())
    return GetMatrixFlatValColName(col.toString(), row, cell);
  return GetValAsVarM(col.toInt(), row, cell);
}

bool DataTable::SetMatrixFlatVal(const Variant& val, Variant col, int row, int cell) {
  if(col.isStringType())
    return SetMatrixFlatValColName(val, col.toString(), row, cell);
  return SetValAsVarM(val, col.toInt(), row, cell);
}

///////////////////////////////////////////
//	Column Name Access

const Variant DataTable::GetValColName(const String& col_nm, int row) const {
  DataCol* da = FindColName(col_nm, true);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsVar(i);
  else return _nilVariant;
}

bool DataTable::SetValColName(const Variant& val, const String& col_nm, int row) {
  DataCol* da = FindColName(col_nm, true);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsVar(val, i);
    return true;
  } else return false;
}

const Variant DataTable::GetMatrixValColName(const String& col_nm, int row,
					     int d0, int d1, int d2, int d3) const {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsVarMDims(i, d0, d1, d2, d3);
  else return _nilVariant;
}

bool DataTable::SetMatrixValColName(const Variant& val, const String& col_nm,
				    int row, int d0, int d1, int d2, int d3) {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsVarMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

const Variant DataTable::GetMatrixFlatValColName(const String& col_nm, int row,
						 int cell) const {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsVarM(i, cell);
  else return _nilVariant;
}

bool DataTable::SetMatrixFlatValColName(const Variant& val, const String& col_nm,
					int row, int cell) {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsVarM(val, i, cell);
    return true;
  } else return false;
}

/////////////////////

const Variant DataTable::GetValColRowName(const String& col_nm, const String& row_col_name,
					  const Variant& row_value) const {
  DataCol* cda = FindColName(col_nm, true);
  if(!cda) return _nilVariant;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return _nilVariant;
  int row = rda->FindVal(row_value);
  if(TestError(row < 0, "GetValColRowName", "value:", row_value.toString(),
	    "of column named:", row_col_name, "not found"))
    return _nilVariant;
  return cda->GetValAsVar(row);
}

bool DataTable::SetValColRowName(const Variant& val, const String& col_nm,
				 const String& row_col_name,
				 const Variant& row_value) {
  DataCol* cda = FindColName(col_nm, true);
  if(!cda || cda->is_matrix_err()) return false;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return false;
  int row = rda->FindVal(row_value);
  if(TestError(row < 0, "SetValColRowName", "value:", row_value.toString(),
	    "of column named:", row_col_name, "not found"))
    return false;
  cda->SetValAsVar(val, row);
  return true;
}

const Variant DataTable::GetMatrixValColRowName(const String& col_nm, 
						const String& row_col_name,
						const Variant& row_value,
						int d0, int d1, int d2, int d3) const {
  DataCol* cda = FindColName(col_nm, true);
  if(!cda || cda->not_matrix_err()) return _nilVariant;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return _nilVariant;
  int row = rda->FindVal(row_value);
  if(TestError(row < 0, "GetMatrixValColRowName", "value:", row_value.toString(),
	       "of column named:", row_col_name, "not found"))
    return _nilVariant;
  return cda->GetValAsVarMDims(row, d0, d1, d2, d3);
}

bool DataTable::SetMatrixValColRowName(const Variant& val, const String& col_nm,
				       const String& row_col_name,
				       const Variant& row_value,
				       int d0, int d1, int d2, int d3) {
  DataCol* cda = FindColName(col_nm, true);
  if (!cda || cda->not_matrix_err()) return false;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return false;
  int row = rda->FindVal(row_value);
  if(TestError(row < 0, "SetMatrixValColRowName", "value:", row_value.toString(),
	       "of column named:", row_col_name, "not found"))
    return false;
  cda->SetValAsVarMDims(val, row, d0, d1, d2, d3);
  return true;
}

const Variant DataTable::GetMatrixFlatValColRowName(const String& col_nm, 
						const String& row_col_name,
						const Variant& row_value,
						int cell) const {
  DataCol* cda = FindColName(col_nm, true);
  if(!cda || cda->not_matrix_err()) return _nilVariant;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return _nilVariant;
  int row = rda->FindVal(row_value);
  if(TestError(row < 0, "GetMatrixFlatValColRowName", "value:", row_value.toString(),
	       "of column named:", row_col_name, "not found"))
    return _nilVariant;
  return cda->GetValAsVarM(row, cell);
}

bool DataTable::SetMatrixFlatValColRowName(const Variant& val, const String& col_nm,
				       const String& row_col_name,
				       const Variant& row_value,
				       int cell) {
  DataCol* cda = FindColName(col_nm, true);
  if (!cda || cda->not_matrix_err()) return false;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return false;
  int row = rda->FindVal(row_value);
  if(TestError(row < 0, "SetMatrixFlatValColRowName", "value:", row_value.toString(),
	       "of column named:", row_col_name, "not found"))
    return false;
  cda->SetValAsVarMDims(val, row, cell);
  return true;
}

/////////////////////

bool DataTable::InitVals(const Variant& init_val, Variant col) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  da->InitVals(init_val);
  return true;
}

bool DataTable::InitValsToRowNo(Variant col) {
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

int DataTable::FindVal(const Variant& val, Variant col, int st_row) const {
  DataCol* da = GetColData(col);
  if (!da) return false;
  return da->FindVal(val, st_row);
}

int DataTable::FindValColName(const Variant& val, const String& col_nm, int st_row) const {
  DataCol* da = FindColName(col_nm, true);
  if (!da) return false;
  return da->FindVal(val, st_row);
}

int DataTable::FindMultiVal(int st_row, const Variant& val1, Variant col1,
			    const Variant& val2, Variant col2,
			    const Variant& val3, Variant col3,
			    const Variant& val4, Variant col4,
			    const Variant& val5, Variant col5,
			    const Variant& val6, Variant col6) const {
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
    for(int i=rows-st_row; i>=0; i--) {
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

DataTableModel* DataTable::GetTableModel() {
  if (!table_model && !isDestroying()) {
    table_model = new DataTableModel(this);
    AddDataClient(table_model);
    //table_model->setPat4D(true); // always
  }
  return table_model;
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

int DataTable::GetValAsInt(int col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsInt(i);
  else return 0;
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

taMatrix* DataTable::GetValAsMatrixColName(const String& col_nm, int row) {
  DataCol* da = FindColName(col_nm, true);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsMatrix(i);
  else return NULL;
}

taMatrix* DataTable::GetValAsMatrixColRowName(const String& col_nm, const String& row_col_name,
	const Variant& row_value)
{
  DataCol* cda = FindColName(col_nm, true);
  if(!cda) return NULL;
  DataCol* rda = FindColName(row_col_name, true);
  if(!rda) return NULL;
  int row = rda->FindVal(row_value);
  if(row >= 0)
    return cda->GetValAsMatrix(row);
  return NULL;
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

bool DataTable::SetValAsInt(int val, int col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsInt(val, i);
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
  return SetValAsMatrix_impl(val, da, row);
}

bool DataTable::SetValAsMatrixColName(const taMatrix* val, const String& col_nm, int row) {
  DataCol* da = FindColName(col_nm, true);
  return SetValAsMatrix_impl(val, da, row);
}

bool DataTable::SetValAsMatrix_impl(const taMatrix* val, DataCol* da, int row) {
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

int DataTable::GetValAsIntM(int col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsIntM(i, cell);
  else return 0;
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

bool DataTable::SetValAsIntM(int val, int col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsIntM(val, i, cell);
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
//	Matrix Dims

double DataTable::GetValAsDoubleMDims(int col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsDoubleMDims(i, d0, d1, d2, d3);
  else return 0.0f;
}

float DataTable::GetValAsFloatMDims(int col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsFloatMDims(i, d0, d1, d2, d3);
  else return 0.0f;
}

int DataTable::GetValAsIntMDims(int col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsIntMDims(i, d0, d1, d2, d3);
  else return 0;
}

const String DataTable::GetValAsStringMDims(int col, int row, int d0, int d1, int d2, int d3, bool na) const {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsStringMDims(i, d0, d1, d2, d3);
  else 
    return (na) ? String("n/a") : _nilString;
}

const Variant DataTable::GetValAsVarMDims(int col, int row, int d0, int d1, int d2, int d3) const {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsVarMDims(i, d0, d1, d2, d3);
  else return _nilVariant;
}

//////////////////////////////

bool DataTable::SetValAsDoubleMDims(double val, int col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsDoubleMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloatMDims(float val, int col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsFloatMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsIntMDims(int val, int col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsIntMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsStringMDims(const String& val, int col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsStringMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsVarMDims(const Variant& val, int col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsVarMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

//////////////////////////

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

DataCol* DataTable::NewCol(DataCol::ValType val_type, const String& col_nm) {
  if (!NewColValid(col_nm)) return NULL;
  StructUpdate(true);
  int idx;
  DataCol* rval = NewCol_impl(val_type, col_nm, idx);
  rval->Init(); // asserts geom
  rval->EnforceRows(rows);	// new guys always get same # of rows as current table
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
  int idx;
  if (cs->isMatrix()) {
    rval = NewColMatrixN(cs->val_type, cs->name, cs->cellGeom(), idx);
  } else {
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
  if(dims < 1) {		// < 1 is shortcut for not actually a matrix!
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
  rval->is_matrix = true;
  rval->cell_geom = cell_geom;
  rval->Init(); // asserts geom
  rval->EnforceRows(rows);	// new guys always get same # of rows as current table
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
      data.MoveIdx(data.size-1, col_idx);
      data.RemoveEl(da);	// get rid of that guy
      da = nda;
      nda->EnforceRows(rows);	// keep row-constant
      StructUpdate(false);
    } else if ((!da->cell_geom.Equal(cell_geom)) || (!da->is_matrix)) {
      StructUpdate(true);
      da->cell_geom = cell_geom;
      da->is_matrix = true;
      da->Init();		// asserts geom
      da->EnforceRows(rows);	// keep row-constant
      StructUpdate(false);
    } else {
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
      data.MoveIdx(data.size-1, col_idx);
      da->EnforceRows(rows);	// keep row-constant
      StructUpdate(false);
    } else if(da->cell_dims() != dims) {
      StructUpdate(true);
      da->cell_geom.SetGeom(dims, d0, d1, d2, d3, d4, d5, d6);
      if(dims == 0)
	da->is_matrix = false;
      else
	da->is_matrix = true;
      da->Init();		// asserts geom
      da->EnforceRows(rows);	// keep row-constant
      StructUpdate(false);
    } else {
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
  else {			// not found -- make one
    col_idx = data.size;	// will be the next guy
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
  StructUpdate(true);
  // note:
  int tmp_idx;
  if (g.dims() == 0) {
    new_col = NewCol(new_type, col_nm);
    tmp_idx = new_col->GetIndex();
  } else {
    new_col = NewColMatrixN(new_type, col_nm, g, tmp_idx);
  }
  // copy all data -- the generic copy dude copies user data, and robustly copies data
  new_col->CopyFromCol_Robust(*src);
  // move to right place, and nuke old guy
  data.MoveIdx(tmp_idx, old_idx);
  data.RemoveEl(src);
  StructUpdate(false);
}

void DataTable::ChangeColTypeGeom(const String& col_nm, ValType new_type,
				  int dims, int d0, int d1, int d2, int d3,
				  int d4, int d5, int d6) {
  DataCol* da = FindColName(col_nm, true);
  if(!da) return;
  MatrixGeom mg;
  mg.SetGeom(dims, d0, d1, d2, d3, d4, d5, d6);
  if((!da->is_matrix && (mg.dims() == 0)) ||
     da->cell_geom.Equal(mg)) return;
  ChangeColTypeGeom_impl(da, new_type, mg);
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
  DataUpdate(true);
  for(int i=0;i<data.size;i++) {
    DataCol* ar = data.FastEl(i);
    rval = ar->InsertRows(st_row, n_rows);
  }
  if(rval)
    rows += n_rows;
  DataUpdate(false); 
  return rval;
}

bool DataTable::RemoveRows(int st_row, int n_rows) {
  if(st_row < 0) st_row = rows-1;	// end
  if(TestError(!RowInRangeNormalize(st_row), "RemoveRows",
	       "start row not in range:",String(st_row)))
    return false;
  if(n_rows < 0) n_rows = rows - st_row;
  int end_row = st_row + n_rows-1;
  if(TestError(!RowInRangeNormalize(end_row), "RemoveRows",
	       "end row not in range:",String(end_row)))
    return false;
  DataUpdate(true);
  for(int i=0;i<data.size;i++) {
    DataCol* ar = data.FastEl(i);
    int act_row;
    if (idx(st_row, ar->AR()->frames(), act_row))
      ar->AR()->RemoveFrames(act_row, n_rows);
  }
  rows -= n_rows;
  if (rows == 0)  keygen.setInt64(0);
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
    data.CopyFromRow(-1, data, row_no);
  }
  DataUpdate(false);
  return true;
}

bool DataTable::DuplicateRows(int st_row, int n_rows) {
  DataUpdate(true);// only data because for views, no change in column structure
  // first insert blank rows, then copy
  bool rval = InsertRows(st_row + n_rows, n_rows);
  if (rval) {
    for (int i = 0; i < n_rows; i++) {
      data.CopyFromRow(st_row + n_rows + i, data, st_row +i);
    }
  }
  DataUpdate(false);
  return rval;
}


bool DataTable::RowInRangeNormalize(int& row) {
  if (row < 0) row = rows + row;
  return ((row >= 0) && (row < rows));
} 

void DataTable::Reset() {
  StructUpdate(true);
  data.Reset();
  rows = 0;
  keygen.setInt64(0);
  StructUpdate(false);
}

void DataTable::ResetData() {
  if (rows == 0) return; // prevent erroneous m_dm calls
  StructUpdate(true);
  for(int i=0;i<data.size;i++) {
    DataCol* ar = data.FastEl(i);
    ar->AR()->Reset();
  }
  rows = 0;
  keygen.setInt64(0);
  StructUpdate(false);
  // also update itrs in case using simple itr mode
  ReadItrInit();
  WriteItrInit();
}

void DataTable::RowsAdding(int n, bool begin) {
  if (begin) {
    DataUpdate(true);// only data because for views, no change in column structure
  } else { // end
    rows += n;
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

void DataTable::SaveHeader_strm(ostream& strm, Delimiters delim,
    bool row_mark, int col_fr, int col_to) 
{
  char cdlm = GetDelim(delim);
  bool need_delim = false; // goes true after first col written
  if (row_mark) {
    strm << "_H:";		// indicates header
    need_delim = true;
  }
  // validate and adjust col range
  if (col_fr < 0) col_fr = data.size + col_fr;
  if (col_fr < 0) col_fr = 0;
  if (col_to < 0) col_to = data.size + col_to;
  if ((col_to < 0) || (col_to >= data.size)) 
    col_to = data.size - 1;
  MatrixGeom dims;
  for(int i=0;i<data.size;i++) {
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
	da->cell_geom.DimsFmIndex(j, dims);
	String hdnm = da->EncodeHeaderName(dims);
	strm << hdnm;
      }
    }
    else {
      String hdnm = da->EncodeHeaderName(dims); //note: dims ignored
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
    strm << "_D:";		// indicates data row
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
	//TODO: 1) need to check for Variant.String type
	// 2) prob need to use fully escaped format, or at least escape "
	if(quote_str && (da->valType() == VT_STRING))
	  strm << "\"" << val << "\"";
	else
	  strm << val;
      }
    }
    else {
      String val = da->GetValAsString(row);
      //TODO: see above in mat
      if(quote_str && (da->valType() == VT_STRING))
	strm << "\"" << val << "\"";
      else
	strm << val;
    }
  }
  strm << endl;
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

void DataTable::SaveHeader(const String& fname, Delimiters delim) {
  taFiler* flr = GetSaveFiler(fname, ".dat,.tsv,.csv,.txt,.log", false);
  if(flr->ostrm)
    SaveHeader_strm(*flr->ostrm, delim);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveDataRow(const String& fname, int row, Delimiters delim, bool quote_str, bool row_mark) {
  taFiler* flr = GetSaveFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  if(flr->ostrm)
    SaveDataRow_strm(*flr->ostrm, row, delim, quote_str, row_mark);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveData(const String& fname, Delimiters delim, bool quote_str, bool save_headers) {
  taFiler* flr = GetSaveFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  if (flr->ostrm) {
    SaveData_strm(*flr->ostrm, delim, quote_str, save_headers);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

// redundant with existing save -- perhaps useful to test the TSV stuff used by clip guy?
// void DataTable::SaveDataTSV(const String& fname) {
//   taFiler* flr = GetSaveFiler(fname, ".tsv", false, "Data");
//   if(flr->ostrm) {
//     *flr->ostrm << HeaderToTSV();
// //    int tot_col = 0; // total flat cols
// //    int max_cell_rows = 0; // max flat rows per cell
//     CellRange cr;
//     cr.SetExtent(cols(), rows);
// //    GetFlatGeom(cr, tot_col, max_cell_rows);
//     *flr->ostrm << RangeToTSV(cr);
//   }
//   flr->Close();
//   taRefN::unRefDone(flr);
// }

void DataTable::AppendData(const String& fname, Delimiters delim, bool quote_str, bool row_mark) {
  taFiler* flr = GetAppendFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  if(flr->ostrm)
    SaveDataRows_strm(*flr->ostrm, delim, quote_str, row_mark);
  flr->Close();
  taRefN::unRefDone(flr);
}

void DataTable::SaveDataLog(const String& fname, bool append, bool dmem_proc_0) {
  if(!log_file) return;		// shouldn't happen
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
      log_file->SaveAs(false);	// no save to tmp
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

int DataTable::LoadHeader_impl(istream& strm, Delimiters delim) {
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
    // val_typ =-1 means type not explicitly supplied -- we'll use existing if name found
    DataCol::DecodeHeaderName(str, base_nm, val_typ, mat_idx, mat_geom);
    int idx = FindColNameIdx(base_nm);
    DataCol* da = NULL;
    if(idx >= 0) da = data.FastEl(idx);
    if (val_typ < 0) {
      if (da) val_typ = da->valType(); // the actual type
      else val_typ = VT_FLOAT; // the default type
    }
    if(!da || (da->valType() != val_typ)) { // only make new one if val type doesn't match
      // mat_geom is only decorated onto first col and should not be remade...
      // if none was supplied, then set it for scalar col (the default)
      if (mat_geom.size == 0) {
	da = FindMakeColName(base_nm, idx, (ValType)val_typ, mat_geom.size,
			     mat_geom[0], mat_geom[1], mat_geom[2],
			     mat_geom[3]);
      }
    }
    load_col_idx.Add(idx);
    if(mat_idx.size > 0) {
      int mdx = da->cell_geom.IndexFmDims(mat_idx[0], mat_idx[1], mat_idx[2], mat_idx[3]);
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

int DataTable::LoadDataRow_impl(istream& strm, Delimiters delim, bool quote_str) {
  static int last_row_err_msg = -1;
  char cdlm = GetDelim(delim);
  StructUpdate(true);
  bool added_row = false;
  int last_mat_col = -1;
  int load_col = 0;		// loading column (always incr)
  int data_col = 0;		// data column (datacol index in data table)
  int c;
  while(true) {
    STRING_BUF(str, 32); // provide a buff so numbers and short strings are efficient
    c = ReadTillDelim(strm, str, cdlm, quote_str);
    if(c == EOF) break;
    if(str == "_H:") {
      c = LoadHeader_impl(strm, delim);
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
    if(c == '\n') break;
    if(c == '\r') { if(strm.peek() == '\n') strm.get(); break; }
  }
  StructUpdate(false);
  return c;
}

int DataTable::LoadDataFixed_impl(istream& strm, FixedWidthSpec* fws) {
  fws->Load_Init(this); // caches cols and indicates any missing cols
  string line;
  int n_skip = fws->n_skip_lines;
  StructUpdate(true);
  while (getline(strm, line)) {
    if (n_skip > 0) {
      --n_skip;
      continue;
    }
    // note: data() is not null terminated!
    String ln(line.data(), line.length());
    fws->AddRow(ln);
  }
  
  StructUpdate(false);
  return true;
}

int DataTable::LoadHeader_strm(istream& strm, Delimiters delim) {
  return LoadHeader_impl(strm, delim);
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

void DataTable::LoadData_strm(istream& strm, Delimiters delim, bool quote_str, int max_recs) {
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
    rval = LoadHeader_impl(*flr->istrm, delim);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

int DataTable::LoadDataRow(const String& fname, Delimiters delim, bool quote_str) {
  load_col_idx.Reset();
  load_mat_idx.Reset();
  taFiler* flr = GetLoadFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  int rval = 0;
  if(flr->istrm)
    rval = LoadDataRow_impl(*flr->istrm, delim, quote_str);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void DataTable::LoadData(const String& fname, Delimiters delim, bool quote_str, int max_recs, bool reset_first) {
  taFiler* flr = GetLoadFiler(fname, ".dat,.tsv,.csv,.txt,.log", false, "Data");
  if(flr->istrm) {
    if(reset_first)
      RemoveAllRows();
    LoadData_strm(*flr->istrm, delim, quote_str, max_recs);
  }
  flr->Close();
  taRefN::unRefDone(flr);
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
///		Calculating columns

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

void DataTable::Sort(Variant col1, bool ascending1,
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
      sp->col_name = da->name;
      if(ascending1) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
    }
  }
  if(col2.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col2);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      sp->col_name = da->name;
      if(ascending2) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
    }
  }
  if(col3.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col3);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      sp->col_name = da->name;
      if(ascending3) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
    }
  }
  if(col4.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col4);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      sp->col_name = da->name;
      if(ascending4) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
    }
  }
  if(col5.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col5);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      sp->col_name = da->name;
      if(ascending5) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
    }
  }
  if(col6.isStringType() || col1.toInt() >= 0) {
    DataCol* da = GetColData(col6);
    if(da) {
      DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
      sp->col_name = da->name;
      if(ascending6) sp->order = DataSortEl::ASCENDING;
      else sp->order = DataSortEl::DESCENDING;
    }
  }
  
  taDataProc::Sort_impl(this, &spec);
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
    sp->col_name = col1;
    if(ascending1) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  if(col2.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col2;
    if(ascending2) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  if(col3.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col3;
    if(ascending3) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  if(col4.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col4;
    if(ascending4) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  if(col5.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col5;
    if(ascending5) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  if(col6.nonempty()) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col6;
    if(ascending6) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  
  taDataProc::Sort_impl(this, &spec);
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
  if(col4) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col4->name;
    if(ascending4) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  if(col5) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col5->name;
    if(ascending5) sp->order = DataSortEl::ASCENDING;
    else sp->order = DataSortEl::DESCENDING;
  }
  if(col6) {
    DataSortEl* sp = (DataSortEl*)spec.ops.New(1);
    sp->col_name = col6->name;
    if(ascending6) sp->order = DataSortEl::ASCENDING;
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
  code_str += "this.RemoveRows(row,1);\n";		     // else remove
  code_str += "}\n";
  code_str += "this.DataUpdate(false);\n";
  bool ok = calc_script->CompileCode(code_str);
  if(TestError(!ok, "Filter", "error in filter expression, see console for errors"))
    return false;
  calc_script->Run();
  return true;
}

bool DataTable::GroupMeanSEM(DataTable* dest_data, DataCol* col1,
			     DataCol* col2, DataCol* col3, DataCol* col4) {
  DataGroupSpec spec;
  if(col1) {
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->col_name = col1->name;
    sp->agg.op = Aggregate::GROUP;
  }
  if(col2) {
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->col_name = col2->name;
    sp->agg.op = Aggregate::GROUP;
  }
  if(col3) {
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->col_name = col3->name;
    sp->agg.op = Aggregate::GROUP;
  }
  if(col4) {
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->col_name = col4->name;
    sp->agg.op = Aggregate::GROUP;
  }
  for(int i=0;i<data.size;i++) {
    DataCol* da = data.FastEl(i);
    if(da == col1 || da == col2 || da == col3 || da == col4) continue;
    if(!da->isNumeric()) continue;
    DataGroupEl* sp = (DataGroupEl*)spec.ops.New(1);
    sp->col_name = da->name;
    sp->agg.op = Aggregate::MEAN;
    sp = (DataGroupEl*)spec.ops.New(1);
    sp->col_name = da->name;
    sp->agg.op = Aggregate::SEM;
  }
  return taDataProc::Group(dest_data, this, &spec);
}

String DataTable::ColStats(Variant col) {
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

