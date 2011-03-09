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
#include <QFileInfo>

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
//   if(!prg || isDestroying()) return;
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
  cell_geom.SetDims(1);
  cell_geom.Set(0, 1);
  hash_table = NULL;
}

void DataCol::InitLinks() {
  inherited::InitLinks();
  taBase::Own(cell_geom, this);
  taBase::Own(calc_expr, this);
  taBase::Own(dim_names, this);
  taMatrix* ar = AR();
  if (ar != NULL)
    taBase::Own(ar, this);
}

void DataCol::CutLinks() {
  cell_geom.CutLinks();
  calc_expr.CutLinks();
  dim_names.CutLinks();
  RemoveHashTable();
  inherited::CutLinks();
}

void DataCol::Copy_Common_(const DataCol& cp) {
  RemoveHashTable();
  // things common to full and schema-only copy
  name = cp.name; // this is the default we want for DataCol
  desc = cp.desc;
  col_flags = cp.col_flags;
  is_matrix = cp.is_matrix;
  cell_geom = cp.cell_geom;
  calc_expr = cp.calc_expr;
  dim_names = cp.dim_names;
}

void DataCol::Copy_(const DataCol& cp) {
  Copy_Common_(cp);
}

void DataCol::CopyFromCol_Robust(const DataCol& cp) {
  RemoveHashTable();
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
  RemoveHashTable();
  StructUpdate(true);
  inherited::Copy_impl(cp);
  SetBaseFlag(COPYING);
  Copy_Common_(cp);
  Init(); //note: table should have no rows, so won't make any
  ClearBaseFlag(COPYING);
  StructUpdate(false);			// initialize array to new geom
}

void DataCol::CopyFromRow(int dest_row, const DataCol& src, int src_row) {
  RemoveHashTable();
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
  RemoveHashTable();
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
    tdim.SetDims(tdim.dims() + 1);
    tdim.Set(tdim.dims()-1, rows);
    ar->SetGeomN(tdim);
    dim_names.SetGeom(1,cell_geom.dims());
  } else {
    ar->SetGeom(1, rows);
    dim_names.SetGeom(1,0);
  }
  // transfer READ_ONLY to mat
  ar->ChangeBaseFlag(BF_GUI_READ_ONLY, (col_flags & READ_ONLY));
}

bool DataCol::isImage() const {
  return GetUserDataAsBool("IMAGE");
}

int DataCol::imageComponents() const {
  if (cell_geom.dims() <= 2) return 1;
  else return cell_geom.FastEl(2);
}

void DataCol::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Init();
}

void DataCol::DataChanged(int dcr, void* op1, void* op2) {
  RemoveHashTable();
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

int DataCol::GetSpecialState() const {
  if(!HasColFlag(SAVE_ROWS)) return 1;
  if(!HasColFlag(SAVE_DATA)) return 2;
  if(HasColFlag(READ_ONLY)) return 4; // light red -- 3 is green
  return 0;
}

void DataCol::ChangeColType(ValType new_type) {
  if (valType() == new_type) return;
  RemoveHashTable();
  MatrixGeom cell_geom;
  if (is_matrix) cell_geom = this->cell_geom; // because we will be nuked
  dataTable()->ChangeColTypeGeom_impl(this, new_type, cell_geom);
  //NOTE: no more code here, because we've probably been deleted/replaced
}

void DataCol::ChangeColCellGeom(const MatrixGeom& new_geom) {
  if ((!is_matrix && (new_geom.dims() == 0)) ||
    cell_geom.Equal(new_geom)) return;
  RemoveHashTable();
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
  RemoveHashTable();
  MatrixGeom new_geom; //note: 0 dims is key to change to scalar
  dataTable()->ChangeColTypeGeom_impl(this, valType(), new_geom);
  //NOTE: no more code here, because we may have been deleted/replaced
}

DataTable* DataCol::dataTable() {
  DataTable* rval = GET_MY_OWNER(DataTable);
  return rval;
}

bool DataCol::EnforceRows(int rws) {
  RemoveHashTable();
  taMatrix* mat = AR();
  if (!mat) return false;
  bool rval = false;
  rval = mat->EnforceFrames(rws);
  return rval;
}

bool DataCol::InsertRows(int st_row, int n_rows) {
  RemoveHashTable();
  taMatrix* mat = AR();
  if (!mat) return false;
  bool rval = mat->InsertFrames(st_row, n_rows);
  if (!rval) return rval;
  return rval;
}  

int DataCol::FindVal(const Variant& val, int st_row) const {
  if(TestError(isMatrix(), "FindVal", "column must be scalar, not matrix")) return -1;
  if(st_row == 0 && hash_table) {
    return hash_table->FindHashValString(val.toString());
  }
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

void DataCol::BuildHashTable() {
  RemoveHashTable();
  if(TestError(isMatrix(), "BuildHashTable", "column must be scalar, not matrix")) return;
  hash_table = new taHashTable;
  if(!hash_table->Alloc(rows() + 10)) return;
  for(int i=0; i<rows(); i++) {
    String strval = GetVal(i).toString();
    hash_table->AddHash(taHashTable::HashCode_String(strval), i, strval);
  }
}

void DataCol::RemoveHashTable() {
  if(hash_table) {
    delete hash_table;
    hash_table = NULL;
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
    DataTable* dt = dataTable();
    // if no save, don't need to check DataTable global
    if (saveToDumpFile()) {
      if(dt && taMisc::is_undo_saving) {
	// always save for obj itself
	if(tabMisc::cur_undo_mod_obj == dt || tabMisc::cur_undo_mod_obj == this) {
	  if((dt->Cells() > taMisc::undo_data_max_cells) ||
	     !dt->HasDataFlag(DataTable::SAVE_ROWS)) {
	    return DQR_NO_SAVE;	// too big or no save!
	  }
	  return DQR_SAVE;
	}
	if(!tabMisc::cur_undo_save_owner || !IsChildOf(tabMisc::cur_undo_save_owner)) {
	  // no need to save b/c unaffected by changes elsewhere..
	  return DQR_NO_SAVE;
	}
	if(dt->Cells() > taMisc::undo_data_max_cells) {
	  return DQR_NO_SAVE;	// too big!
	}
      }
      if(dt && dt->HasDataFlag(DataTable::SAVE_ROWS)) return DQR_SAVE;
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
  switch(cell_geom.dims()) {
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

void DataCol::WriteFmSubMatrix(int row, 
			       const taMatrix* src, taMatrix::RenderOp render_op,
			       int off0, int off1, int off2,
			       int off3, int off4, int off5, int off6) {
  taMatrix* mat = GetValAsMatrix(row);
  if(mat) {
    taBase::Ref(mat);
    mat->WriteFmSubMatrix(src, render_op, off0, off1, off2, off3, off4, off5, off6);
    taBase::unRefDone(mat);
  }
}

void DataCol::ReadToSubMatrix(int row, 
			      taMatrix* dest, taMatrix::RenderOp render_op, 
			      int off0, int off1, int off2,
			      int off3, int off4, int off5, int off6) {
  taMatrix* mat = GetValAsMatrix(row);
  if(mat) {
    taBase::Ref(mat);
    mat->ReadToSubMatrix(dest, render_op, off0, off1, off2, off3, off4, off5, off6);
    taBase::unRefDone(mat);
  }
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
    if(cell_geom.IndexFmDimsN(dims) == 0) { // first guy
      mat_info += cell_geom.GeomToString("<", ">");
    }
  }
  return typ_info + name + mat_info; // e.g., $StringVecCol[2:2,3]
}

void DataCol::DecodeHeaderName(String nm, String& base_nm, int& vt,
  MatrixGeom& mat_idx, MatrixGeom& mat_geom) 
{
  base_nm = nm;
  vt = -1; // unknown
  mat_idx.SetDims(0);
  mat_geom.SetDims(0);
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

taObjDiffRec* DataCol::GetObjDiffVal(taObjDiff_List& odl, int nest_lev, MemberDef* memb_def, 
			   const void* par, TypeDef* par_typ, taObjDiffRec* par_od) const {
  // this is same as base objdiff but puts children last to make more sense for user
  taObjDiffRec* odr = inherited::GetObjDiffVal(odl, nest_lev, memb_def, par, par_typ, par_od);

  for(int i=0;i<rows();i++) {
    if(isMatrix()) {
      taMatrix* mat = ((DataCol*)this)->GetValAsMatrix(i);
      taBase::Ref(mat);
      // stream each cell separately..
      for(int j=0;j<cell_size();j++) {
	taObjDiffRec* clodr = new taObjDiffRec(odl, nest_lev+1, valTypeDef(), NULL,
					       mat->FastEl_Flat_(j),
					       (void*)this, GetTypeDef(), odr);
	clodr->name = String(i) + "," + String(j);	// row, cell
	clodr->ComputeHashCode();			// need to update
	odl.Add(clodr);
      }
      taBase::unRefDone(mat);
    }
    else {
      taObjDiffRec* clodr = new taObjDiffRec(odl, nest_lev+1, valTypeDef(), NULL,
					   (void*)AR()->FastEl_Flat_(i),
					   (void*)this, GetTypeDef(), odr);
      clodr->name = String(i);	// row
      clodr->ComputeHashCode();	// need to update
      odl.Add(clodr);
    }
  }
  return odr;
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
    // if last col removed then rows must become 0
    if (size == 0) {
      dt->rows = 0;
    }
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

int DataTable::GetSpecialState() const {
  if(!HasDataFlag(SAVE_ROWS)) return 1;
  if(HasDataFlag(HAS_CALCS)) return 2;
  return 0;
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

const Variant DataTable::GetColUserData(const String& name, Variant col) const {
  DataCol* da = GetColData(col);
  if (da) return da->GetUserData(name);
  else return _nilVariant;
}

void DataTable::SetColUserData(const String& name, const Variant& value, Variant col) {
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

const Variant DataTable::GetValColName(const String& col_nm, int row, bool quiet) const {
  DataCol* da = FindColName(col_nm, true);
  int i;
  if (da &&  idx_err(row, da->rows(), i, quiet))
    return da->GetValAsVar(i);
  else return _nilVariant;
}

bool DataTable::SetValColName(const Variant& val, const String& col_nm, int row, bool quiet) {
  DataCol* da = FindColName(col_nm, true);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i, quiet)) {
    da->SetValAsVar(val, i);
    return true;
  } else return false;
}

const Variant DataTable::GetMatrixValColName(const String& col_nm, int row,
		     int d0, int d1, int d2, int d3, bool quiet) const {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i, quiet))
    return da->GetValAsVarMDims(i, d0, d1, d2, d3);
  else return _nilVariant;
}

bool DataTable::SetMatrixValColName(const Variant& val, const String& col_nm,
		    int row, int d0, int d1, int d2, int d3, bool quiet) {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i, quiet)) {
    da->SetValAsVarMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

const Variant DataTable::GetMatrixFlatValColName(const String& col_nm, int row,
						 int cell, bool quiet) const {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i, quiet))
    return da->GetValAsVarM(i, cell);
  else return _nilVariant;
}

bool DataTable::SetMatrixFlatValColName(const Variant& val, const String& col_nm,
					int row, int cell, bool quiet) {
  DataCol* da = FindColName(col_nm, true);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i, quiet)) {
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

double DataTable::GetValAsDouble(Variant col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da && idx_err(row, da->rows(), i))
    return da->GetValAsDouble(i);
  else return 0.0f;
}

float DataTable::GetValAsFloat(Variant col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsFloat(i);
  else return 0.0f;
}

int DataTable::GetValAsInt(Variant col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsInt(i);
  else return 0;
}

const String DataTable::GetValAsString(Variant col, int row) const {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsString(i);
  else return "n/a";
}

const Variant DataTable::GetValAsVar(Variant col, int row) const {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsVar(i);
  else return _nilVariant;
}

taMatrix* DataTable::GetValAsMatrix(Variant col, int row) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsMatrix(i);
  else return NULL;
}

taMatrix* DataTable::GetValAsMatrixColName(const String& col_nm, int row, bool quiet) {
  DataCol* da = FindColName(col_nm, true);
  int i;
  if (da &&  idx_err(row, da->rows(), i, quiet))
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


taMatrix* DataTable::GetRangeAsMatrix(Variant col, int st_row, int n_rows) {
  DataCol* da = GetColData(col);
  int i;
  if (da &&  idx_err(st_row, da->rows(), i))
    return da->GetRangeAsMatrix(i, n_rows);
  else return NULL;
}


///////////////////////////////////////
// sub-matrix reading and writing functions

void DataTable::WriteFmSubMatrix(Variant col, int row, 
				 const taMatrix* src, taMatrix::RenderOp render_op,
				 int off0, int off1, int off2,
				 int off3, int off4, int off5, int off6) {
  DataCol* da = GetColData(col);
  int i;
  if (da && idx_err(row, da->rows(), i))
    da->WriteFmSubMatrix(row, src, render_op, off0, off1, off2, off3, off4, off5, off6);
}

void DataTable::ReadToSubMatrix(Variant col, int row, 
				taMatrix* dest, taMatrix::RenderOp render_op, 
				int off0, int off1, int off2,
				int off3, int off4, int off5, int off6) {
  DataCol* da = GetColData(col);
  int i;
  if (da && idx_err(row, da->rows(), i))
    da->ReadToSubMatrix(row, dest, render_op, off0, off1, off2, off3, off4, off5, off6);
}

void DataTable::WriteFmSubMatrixTable(Variant col, int row, 
				      const DataTable* src, Variant src_col, int src_row,
				      taMatrix::RenderOp render_op,
				      int off0, int off1, int off2,
				      int off3, int off4, int off5, int off6) {
  if(TestError(!src, "WriteFmSubMatrixTable", "submat_src table is NULL"))
    return;
  DataCol* da = GetColData(col);
  int i;
  if(!da || !idx_err(row, da->rows(), i)) return;
  DataCol* sda = src->GetColData(src_col);
  int j;
  if(!sda || !idx_err(src_row, sda->rows(), j)) return;
  taMatrix* mat = sda->GetValAsMatrix(src_row);
  if(!mat) return;
  taBase::Ref(mat);
  da->WriteFmSubMatrix(row, mat, render_op, off0, off1, off2, off3, off4, off5, off6);
  taBase::unRefDone(mat);
}

void DataTable::ReadToSubMatrixTable(Variant col, int row, 
				     const DataTable* dest, Variant dest_col, int dest_row,
				     taMatrix::RenderOp render_op, 
				     int off0, int off1, int off2,
				     int off3, int off4, int off5, int off6) {
  if(TestError(!dest, "ReadToSubMatrixTable", "submat_dest table is NULL"))
    return;
  DataCol* da = GetColData(col);
  int i;
  if(!da || !idx_err(row, da->rows(), i)) return;
  DataCol* sda = dest->GetColData(dest_col);
  int j;
  if(!sda || !idx_err(dest_row, sda->rows(), j)) return;
  taMatrix* mat = sda->GetValAsMatrix(dest_row);
  if(!mat) return;
  taBase::Ref(mat);
  da->ReadToSubMatrix(row, mat, render_op, off0, off1, off2, off3, off4, off5, off6);
  taBase::unRefDone(mat);
}

// lookup versions:

void DataTable::WriteFmSubMatrixTableLookup(Variant col, int row, 
	      const DataTable* submat_src, Variant submat_src_col,
	      Variant submat_lookup_val, Variant submat_lookup_col,
	      taMatrix::RenderOp render_op, const DataTable* offset_lookup,
	      Variant offset_col, Variant offset_lookup_val, Variant offset_lookup_col) {
  if(TestError(!submat_src, "WriteFmSubMatrixTableLookup", "submat_src table is NULL"))
    return;
  if(TestError(!offset_lookup, "WriteFmSubMatrixTableLookup", "offset_lookup table is NULL"))
    return;
  DataCol* da = GetColData(col);
  int i;
  if(!da || !idx_err(row, da->rows(), i)) return;
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

void DataTable::ReadToSubMatrixTableLookup(Variant col, int row, 
		const DataTable* submat_dest, Variant submat_dest_col,
	        Variant submat_lookup_val, Variant submat_lookup_col,
		taMatrix::RenderOp render_op, const DataTable* offset_lookup,
	        Variant offset_col, Variant offset_lookup_val, Variant offset_lookup_col) {
  if(TestError(!submat_dest, "ReadToSubMatrixTableLookup", "submat_dest table is NULL"))
    return;
  if(TestError(!offset_lookup, "ReadToSubMatrixTableLookup", "offset_lookup table is NULL"))
    return;
  DataCol* da = GetColData(col);
  int i;
  if(!da || !idx_err(row, da->rows(), i)) return;
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

bool DataTable::SetValAsDouble(double val, Variant col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsDouble(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloat(float val, Variant col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsFloat(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsInt(int val, Variant col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsInt(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsString(const String& val, Variant col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsString(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsVar(const Variant& val, Variant col, int row) {
  DataCol* da = GetColData(col);
  if (!da) return false;
  if (da->is_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsVar(val, i);
    return true;
  } else return false;
}

bool DataTable::SetValAsMatrix(const taMatrix* val, Variant col, int row) {
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
  if (idx_err(row, da->rows(), i, quiet)) {
    return da->SetValAsMatrix(val, i);
  } else return false;
}

////////////////////////

double DataTable::GetValAsDoubleM(Variant col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsDoubleM(i, cell);
  else return 0.0f;
}

float DataTable::GetValAsFloatM(Variant col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsFloatM(i, cell);
  else return 0.0f;
}

int DataTable::GetValAsIntM(Variant col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsIntM(i, cell);
  else return 0;
}

const String DataTable::GetValAsStringM(Variant col, int row, int cell, bool na) const {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsStringM(i, cell);
  else 
    return (na) ? String("n/a") : _nilString;
}

const Variant DataTable::GetValAsVarM(Variant col, int row, int cell) const {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsVarM(i, cell);
  else return _nilVariant;
}

////////////////////////

bool DataTable::SetValAsDoubleM(double val, Variant col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsDoubleM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloatM(float val, Variant col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsFloatM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsIntM(int val, Variant col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsIntM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsStringM(const String& val, Variant col, int row, int cell) {
  DataCol* da = GetColData(col);
  if (!da || (cell > 0 && da->not_matrix_err())) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsStringM(val, i, cell);
    return true;
  } else return false;
}

bool DataTable::SetValAsVarM(const Variant& val, Variant col, int row, int cell) {
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

double DataTable::GetValAsDoubleMDims(Variant col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsDoubleMDims(i, d0, d1, d2, d3);
  else return 0.0f;
}

float DataTable::GetValAsFloatMDims(Variant col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsFloatMDims(i, d0, d1, d2, d3);
  else return 0.0f;
}

int DataTable::GetValAsIntMDims(Variant col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsIntMDims(i, d0, d1, d2, d3);
  else return 0;
}

const String DataTable::GetValAsStringMDims(Variant col, int row, int d0, int d1, int d2, int d3, bool na) const {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsStringMDims(i, d0, d1, d2, d3);
  else 
    return (na) ? String("n/a") : _nilString;
}

const Variant DataTable::GetValAsVarMDims(Variant col, int row, int d0, int d1, int d2, int d3) const {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (da &&  idx_err(row, da->rows(), i))
    return da->GetValAsVarMDims(i, d0, d1, d2, d3);
  else return _nilVariant;
}

//////////////////////////////

bool DataTable::SetValAsDoubleMDims(double val, Variant col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsDoubleMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsFloatMDims(float val, Variant col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsFloatMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsIntMDims(int val, Variant col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsIntMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsStringMDims(const String& val, Variant col, int row, int d0, int d1, int d2, int d3) {
  DataCol* da = GetColData(col);
  if (!da || da->not_matrix_err()) return false;
  int i;
  if (idx_err(row, da->rows(), i)) {
    da->SetValAsStringMDims(val, i, d0, d1, d2, d3);
    return true;
  } else return false;
}

bool DataTable::SetValAsVarMDims(const Variant& val, Variant col, int row, int d0, int d1, int d2, int d3) {
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

DataCol* DataTable::NewColFmMatrix(taMatrix* mat, const String& col_nm) {
  if(!mat) { taMisc::Error("taDataProc::NewColFmMatrix: mat cannot be NULL"); return false; }  
  if(col_nm == "") { taMisc::Error("taDataProc::NewColFmMatrix: col_nm must be non-empty"); return false; }  

  ValType val_type = mat->GetDataValType();
  MatrixGeom geom(mat->dims(), mat->dim(0), mat->dim(1), mat->dim(2), mat->dim(3), mat->dim(4), mat->dim(5), mat->dim(6));
  int idx;
  DataCol* rval = NewColMatrixN(val_type, col_nm, geom, idx);
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
  if((da->valType() == new_type) && 
     ((!da->is_matrix && (mg.dims() == 0)) || da->cell_geom.Equal(mg))) return;
  ChangeColTypeGeom_impl(da, new_type, mg);
}

void DataTable::ChangeColType(Variant col, ValType new_type) {
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

bool DataTable::MatrixColToScalars(Variant mtx_col, const String& scalar_col_name_stub) {
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

bool DataTable::MatrixColFmScalars(Variant mtx_col, const String& scalar_col_name_stub) {
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
      if(cur_idx >= cls) break;	// all done
    }
  }
  StructUpdate(false);
  return true;
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

void DataTable::RemoveCol(Variant col) {
  DataCol* da = GetColData(col);
  if(!da) return;
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
///	Saving / Loading from Emergent or Plain Text Files

/*
  Emergent Header format: 
  $ = String
  % = float
  # = double
  | = int
  @ = byte
  & = variant

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
  while(((c = strm.get()) != EOF) && (c != '\n') && (c != '\r') && !((c == delim) && (depth <= 0))) {
    if(quote_str && (depth > 0) && (c == '\"'))
      depth--;
    else
      str += (char)c;
  }
  // consume lf of crlf-pair for Windows files
  if ((c == '\r') && (strm.peek() == '\n'))
    c = strm.get();
  return c;
}

int DataTable::ReadTillDelim_Str(const String& istr, int& idx, String& str,
				 const char delim, bool quote_str, bool& got_quote) {
  got_quote = false;
  int c;
  int depth = 0;
  int len = istr.length();
  if(idx >= len) return EOF;
  if(quote_str && (istr[idx] == '\"')) {
    got_quote = true;
    idx++;
    depth++;
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

int DataTable::LoadHeader_impl(istream& strm, Delimiters delim,	bool native, bool quote_str) 
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
      continue;			// for some reason it is empty
    }
    String base_nm;
    int val_typ;
    int col_idx;
    int cell_idx = -1; // mat cell index, or -1 if not a mat
    DataCol* da = NULL;
    // Load vs. Import decoding is sufficiently different we use two subroutines below
    if (native) {
      MatrixGeom mat_idx;
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
    load_mat_idx.Add(cell_idx);	// no matrix info
    
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
    flr->open_read();		// always re-open -- if read goes past EOF, then bad
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

void DataTable::ImportData_strm(istream& strm, bool headers, 
    Delimiters delim, bool quote_str, int max_recs)
{
  StructUpdate(true);
  int c = ~EOF; // just set to something we know is not==EOF
  int st_row = rows;
  if (headers) {
    String hdr;
    readline_auto(strm, hdr);	// discard header
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
  int tabfreq = 0;
  int commafreq = 0;
  int spacefreq = 0;
  bool useln1 = false;
  if(ln1.nonempty()) {		// use 2nd line if poss because header can have weird stuff
    tabfreq = ln1.freq('\t');
    commafreq = ln1.freq(',');
    spacefreq = ln1.freq(' ');
    useln1 = true;
  }
  else {
    useln1 = false;					 // actually not good
    tabfreq = ln0.freq('\t');
    commafreq = ln0.freq(',');
    spacefreq = ln0.freq(' ');
  }

  if(tabfreq > commafreq && tabfreq > spacefreq) 
    delim = TAB;
  else if (commafreq > tabfreq && commafreq > spacefreq)
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
  if(useln1) {			// rely on delimiter parsing for where to look for quotes
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
      continue;			// for some reason it is empty
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

taBase::ValType DataTable::DecodeImportDataType(const String& dat_str) {
  if(dat_str.empty())
    return VT_VARIANT;

  if(dat_str[0] == '\"')
    return VT_STRING;

  int idx = 0;
  int c;
  while(idx < dat_str.length()) {
    c = dat_str[idx++];
    if(isspace(c)) continue;

    if((c == '.') || isdigit(c) || (c == '-')) {	// number
      bool not_num = false;
      bool gotreal = false;
      if(c == '.') gotreal = true;
      
      while(idx < dat_str.length()) {
	c = dat_str[idx++];
	if(isspace(c)) continue;
	if((c == '.') || isxdigit(c) || (c == 'x') || (c == 'e') || (c == '-') ||
	   (c == 'X') || (c == 'E')) {
	  if(c == '.') gotreal = true;
	}
	else {
	  not_num = true;
	  break;
	}
      }
      if(!not_num) {
	if(gotreal) return VT_DOUBLE;
	return VT_INT;
      }
    }
  }
  // if not trapped earlier, then it must be a string..
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

