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


#include "ta_matrix.h"
#include "ta_thread.h"

#ifdef TA_GUI
# include "ta_matrix_qt.h"
# include <QMimeData>
#endif


//////////////////////////
//  CellRange		//
//////////////////////////

void CellRange::SetExtent(int wd, int ht) {
  col_to = col_fr + wd - 1;
  row_to = row_fr + ht - 1;
}

void CellRange::LimitExtent(int wd, int ht) {
  if (width() > wd) col_to = col_fr + wd - 1;
  if (height() > ht) row_to = row_fr + ht - 1;
}

void CellRange::LimitRange(int row_to_, int col_to_) {
  if (col_to > col_to_) col_to = col_to_;
  if (row_to > row_to_) row_to = row_to_;
}

void CellRange::SetFromModel(const QModelIndexList& indexes) {
//NOTE: this assumes that we only allow contiguous areas -- the
//  input list could have discontiguous guys, but here we will
//  just make an overal sel based on the max
  // start with first guy, then expand it out
  if (indexes.count() > 0) {
    const QModelIndex& mi = indexes.first();
    row_fr = row_to = mi.row();
    col_fr = col_to = mi.column();
    for (int i = 1; i < indexes.count(); ++i) {
      const QModelIndex& mi = indexes.at(i);
      int r = mi.row();
      int c = mi.column();
      if (r < row_fr) row_fr = r;
      else if (r > row_to) row_to = r;
      if (c < col_fr) col_fr = c;
      else if (c > col_to) col_to = c;
    }
  } else {
    Set(0, 0, -1, -1);
  }
}

//////////////////////////
//  MatrixIndex
//////////////////////////

MatrixIndex::MatrixIndex(int init_n_dims) {
  Initialize();
  SetDims(init_n_dims);
}
  
MatrixIndex::MatrixIndex(int dims, int d0, int d1, int d2, int d3, int d4, int d5, int d6)
{
  Initialize();
  SetIndexes(dims, d0, d1, d2, d3, d4, d5, d6);
}

void MatrixIndex::Initialize() {
  // default minimum geometry is 2d
  n_dims = 0;
  // set all the dim values valid -- other code may use shortcuts to read these
  memset(el, 0, sizeof(el));
}

void MatrixIndex::Destroy() {
#ifdef DEBUG
  // helps detect multi-delete errors
  for (int i = n_dims - 1; i >= 0; el[i--] = 0); 
  n_dims = 0; 
#endif
}

void MatrixIndex::Copy_(const MatrixIndex& cp) {
  SetDims(cp.n_dims);
  for (int i = 0; i < n_dims; ++i) {
    el[i] = cp.el[i];
  }
} 

bool MatrixIndex::Equal(const MatrixIndex& other) const {
  if (n_dims != other.n_dims) return false;
  for (int i = 0; i < n_dims; ++i) {
    if (el[i] != other.el[i]) return false;
  }
  return true;
} 

void MatrixIndex::SetIndexes(int dms, int d0, int d1, int d2, int d3, int d4,
  int d5, int d6) 
{
  SetDims(dms);
  el[0] = d0;
  el[1] = d1;
  el[2] = d2;
  el[3] = d3;
  el[4] = d4;
  el[5] = d5;
  el[6] = d6;
}

void MatrixIndex::GetIndexes(int& dms, int& d0, int& d1, int& d2, int& d3, int& d4,
  int& d5, int& d6) 
{
  dms = dims();
  if(dms >= 1) d0 = dim(0); else d0 = 0;
  if(dms >= 2) d1 = dim(1); else d1 = 0;
  if(dms >= 3) d2 = dim(2); else d2 = 0;
  if(dms >= 4) d3 = dim(3); else d3 = 0;
  if(dms >= 5) d4 = dim(4); else d4 = 0;
  if(dms >= 6) d5 = dim(5); else d5 = 0;
  if(dms >= 7) d6 = dim(6); else d6 = 0;
}

MatrixIndex* MatrixIndex::operator+=(const MatrixIndex& ad) {
  int mxd = MIN(n_dims, ad.n_dims);
  for(int i=0; i<mxd; i++) {
    FastEl(i) += ad.FastEl(i);
  }
  return this;
}

MatrixIndex* MatrixIndex::operator-=(const MatrixIndex& ad) {
  int mxd = MIN(n_dims, ad.n_dims);
  for(int i=0; i<mxd; i++) {
    FastEl(i) -= ad.FastEl(i);
  }
  return this;
}

MatrixIndex* MatrixIndex::operator*=(const MatrixIndex& ad) {
  int mxd = MIN(n_dims, ad.n_dims);
  for(int i=0; i<mxd; i++) {
    FastEl(i) *= ad.FastEl(i);
  }
  return this;
}

MatrixIndex* MatrixIndex::operator/=(const MatrixIndex& ad) {
  int mxd = MIN(n_dims, ad.n_dims);
  for(int i=0; i<mxd; i++) {
    FastEl(i) /= ad.FastEl(i);
  }
  return this;
}

MatrixIndex::operator int_Matrix*() const {
  int_Matrix* r = new int_Matrix(1,n_dims);
  for(int i=0; i<n_dims; i++) {
    r->FastEl_Flat(i) = FastEl(i);
  }
  return r;
}

MatrixIndex* MatrixIndex::operator=(const taMatrix* cp) {
  if(!cp) return this;
  n_dims = MIN(TA_MATRIX_DIMS_MAX, cp->size);
  for(int i=0; i<n_dims; i++) {
    FastEl(i) = cp->FastElAsVar_Flat(i).toInt();
  }
  return this;
}

String MatrixIndex::ToString(const char* ldelim, const char* rdelim) const {
  String rval(ldelim);
  rval += String(n_dims) + ":";
  int i;
  for (i = 0; i < n_dims-1; ++i) {
    rval += String(el[i]) + ",";
  }
  rval += String(el[i]) + rdelim;
  return rval;
}

String& MatrixIndex::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << '[';
  int i;
  for (i = 0; i < n_dims-1; ++i) {
    strm += String(el[i]) + ",";
  }
  strm += String(el[i]) + ']';
  return strm;
}

void MatrixIndex::FromString(const String& str_, const char* ldelim, const char* rdelim) {
  String str = str_.after(ldelim);
  String ds = str.before(':');
  str = str.after(':');
  SetDims((int)ds);
  int i;
  for(i=0;i<n_dims-1;i++) {
    ds = str.before(',');
    str = str.after(',');
    Set(i, (int)ds);
  }
  ds = str.before(rdelim);
  str = str.after(rdelim);
  Set(i, (int)ds);
}

String MatrixIndex::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
			      bool force_inline) const {
  // always inline effectively
  return ToString();
}

bool MatrixIndex::SetValStr(const String& val, void* par, MemberDef* memb_def, 
			   TypeDef::StrContext sc, bool force_inline) {
  // always inline effectively
  FromString(val);
  return true;
}

int MatrixIndex::Dump_Save_Value(ostream& strm, taBase*, int) {
  strm << "{ ";
  int i;
  for (i=0; i < n_dims; i++) {
    strm << FastEl(i) << ";";
  }
  return true;
}

int MatrixIndex::Dump_Load_Value(istream& strm, taBase*) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)    return EOF;
  if(c == ';') // just a path
    return 2;  // signal that just a path was loaded..

  if(c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  c = taMisc::read_till_rb_or_semi(strm);
  int cnt = 0;
  int val;
  while ((c == ';') && (c != EOF)) {
    val = taMisc::LexBuf.toInt();
    if (cnt > n_dims-1)
      AddDim(val);
    else Set(cnt, val);
    ++cnt;
    c = taMisc::read_till_rb_or_semi(strm);
  }
  if (c==EOF)	return EOF;
  SetDims(cnt);			// just to be double sure it is same as loaded size
  return true;
}

void MatrixIndex::AddDim(int value) {
  if (n_dims >= TA_MATRIX_DIMS_MAX) return;
  el[n_dims++] = value;
}

bool MatrixIndex::SetDims(int new_sz) {
  if ((new_sz < 0) || (new_sz >= TA_MATRIX_DIMS_MAX)) return false;
  if(n_dims == new_sz) return false;
  // zero out orphaned old elements
  for (int i = n_dims - 1; i >= new_sz; --i)
    el[i] = 0;
  // zero out new elements
  for (int i = n_dims; i < new_sz; ++i)
    el[i] = 0;
  n_dims = new_sz;
  return true;
}


//////////////////////////
//  MatrixGeom		//
//////////////////////////

MatrixGeom::MatrixGeom(int init_n_dims) {
  Initialize();
  SetDims(init_n_dims);
}
  
MatrixGeom::MatrixGeom(int dims, int d0, int d1, int d2, int d3, int d4, int d5, int d6)
{
  Initialize();
  SetGeom(dims, d0, d1, d2, d3, d4, d5, d6);
}

void MatrixGeom::Initialize() {
  // default minimum geometry is 2d
  n_dims = 0;
  // set all the dim values valid -- other code may use shortcuts to read these
  memset(el, 0, sizeof(el));
}

void MatrixGeom::Destroy() {
#ifdef DEBUG
  // helps detect multi-delete errors
  for (int i = n_dims - 1; i >= 0; el[i--] = 0); 
  n_dims = 0; 
#endif
}

void MatrixGeom::Copy_(const MatrixGeom& cp) {
  SetDims(cp.n_dims);
  for (int i = 0; i < n_dims; ++i) {
    el[i] = cp.el[i];
  }
  UpdateAfterEdit_impl();
} 

void MatrixGeom::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  elprod[0] = el[0];
  for (int i = 1; i < n_dims; ++i) {
    elprod[i] = elprod[i-1] * el[i]; // product of all others
  }
}

void MatrixGeom::AddDim(int value) {
  if (n_dims >= TA_MATRIX_DIMS_MAX) return;
  el[n_dims++] = value;
  UpdateAfterEdit_impl();
}

void MatrixGeom::Set(int i, int value) {
  if (InRange(i)) {
    el[i] = value;
    UpdateAfterEdit_impl();
  }
}

void MatrixGeom::DimsFmIndex(int idx, MatrixIndex& d) const {
  d.SetDims(n_dims);
  div_t qr;
  int dx = idx;			// local register
  for(int i=n_dims-1; i>=1; i--) {
    qr = div(dx, elprod[i-1]);
    d[i] = qr.quot; dx = qr.rem;
  }
  d[0] = dx;
}

int MatrixGeom::SafeIndexFmDimsN(const MatrixIndex& indicies) const {
  if(TestError((indicies.dims() < 1), "SafeIndexFmDimsN",
	       "at least 1 index must be specified"))
    return -1;
  if(TestError((indicies.dims() > dims()), "SafeIndexFmDimsN",
	       "too many indices for matrix"))
    return -1;
  return SafeIndexFmDims_(indicies.el);
}

int MatrixGeom::IndexFmDims_(const int* d) const {
  int rval = d[0];
  for(int i=1; i<n_dims; i++) {
    rval += d[i] * elprod[i-1];
  }
  return rval;
}

int MatrixGeom::SafeIndexFmDims_(const int* d) const {
  if(TestError((dims() < 1), "SafeIndexFmDims",
	       "matrix geometry has not been initialized"))
    return -1;
  int rval = SafeIndex_(d[0], el[0]);
  if(rval < 0) return rval;
  for(int i=1; i<n_dims; i++) {
    int si = SafeIndex_(d[i], el[i]);
    if(si < 0) return si;
    rval += si * elprod[i-1];
  }
  return rval;
}

int MatrixGeom::IndexFmDims2D(int col, int row, bool pat_4d,
    taMisc::MatrixView mat_view) const
{
  if (n_dims < 4) pat_4d = false;
  if (mat_view == taMisc::DEF_ZERO)
    mat_view = taMisc::matrix_view;
  // 0-base the row
  const int rc = rowCount(pat_4d); // cache
  const int cc = colCount(pat_4d);
  if (mat_view == taMisc::BOT_ZERO) {
    row = rc - row - 1;
  }
  if (pat_4d) {
    // factor down the row/col to get d2 and d1
    div_t qr_col = div(col, el[0]); // quot=d2, rem=d0
    div_t qr_row = div(row, el[1]); // quot=rest, rem=d1
    return IndexFmDims(qr_col.rem, qr_row.rem, qr_col.quot, qr_row.quot);
  } else {
    return (row * cc) + col;
  }
  return -1; // compiler food, never executes
}

MatrixGeom* MatrixGeom::operator+=(const MatrixGeom& ad) {
  int mxd = MIN(n_dims, ad.n_dims);
  for(int i=0; i<mxd; i++) {
    Set(i, FastEl(i) + ad.FastEl(i));
  }
  return this;
}

MatrixGeom* MatrixGeom::operator-=(const MatrixGeom& ad) {
  int mxd = MIN(n_dims, ad.n_dims);
  for(int i=0; i<mxd; i++) {
    Set(i, FastEl(i) - ad.FastEl(i));
  }
  return this;
}

MatrixGeom* MatrixGeom::operator*=(const MatrixGeom& ad) {
  int mxd = MIN(n_dims, ad.n_dims);
  for(int i=0; i<mxd; i++) {
    Set(i, FastEl(i) * ad.FastEl(i));
  }
  return this;
}

MatrixGeom* MatrixGeom::operator/=(const MatrixGeom& ad) {
  int mxd = MIN(n_dims, ad.n_dims);
  for(int i=0; i<mxd; i++) {
    Set(i, FastEl(i) / ad.FastEl(i));
  }
  return this;
}

MatrixGeom::operator int_Matrix*() const {
  int_Matrix* r = new int_Matrix(1,n_dims);
  for(int i=0; i<n_dims; i++) {
    r->FastEl_Flat(i) = FastEl(i);
  }
  return r;
}

MatrixGeom* MatrixGeom::operator=(const taMatrix* cp) {
  if(!cp) return this;
  n_dims = MIN(TA_MATRIX_DIMS_MAX, cp->size);
  for(int i=0; i<n_dims; i++) {
    Set(i, cp->FastElAsVar_Flat(i).toInt());
  }
  return this;
}

bool MatrixGeom::Equal(const MatrixGeom& other) const {
  if (n_dims != other.n_dims) return false;
  for (int i = 0; i < n_dims; ++i) {
    if (el[i] != other.el[i]) return false;
  }
  return true;
} 

bool MatrixGeom::IsFrameOf(const MatrixGeom& other) const {
  if (n_dims != (other.n_dims - 1)) return false;
  for (int i = 0; i < n_dims; ++i) {
    if (el[i] != other.el[i]) return false;
  }
  return true;
}

int MatrixGeom::Product() const {
  if (n_dims == 0) return 0;
  return elprod[n_dims-1];
}

void MatrixGeom::SetGeom(int dms, int d0, int d1, int d2, int d3, int d4,
  int d5, int d6) 
{
  SetDims(dms);
  el[0] = d0;
  el[1] = d1;
  el[2] = d2;
  el[3] = d3;
  el[4] = d4;
  el[5] = d5;
  el[6] = d6;
  UpdateAfterEdit_impl();
}

void MatrixGeom::GetGeom(int& dms, int& d0, int& d1, int& d2, int& d3, int& d4,
  int& d5, int& d6) 
{
  dms = dims();
  if(dms >= 1) d0 = dim(0); else d0 = 0;
  if(dms >= 2) d1 = dim(1); else d1 = 0;
  if(dms >= 3) d2 = dim(2); else d2 = 0;
  if(dms >= 4) d3 = dim(3); else d3 = 0;
  if(dms >= 5) d4 = dim(4); else d4 = 0;
  if(dms >= 6) d5 = dim(5); else d5 = 0;
  if(dms >= 7) d6 = dim(6); else d6 = 0;
}

void MatrixGeom::Get2DGeom(int& x, int& y) const {
  if (n_dims == 0) {
    x = 0;
    y = 0;
  }
  else if(n_dims == 1) {
    x = FastEl(0);
    y = 1;
  }
  else if(n_dims >= 2) {
    x = FastEl(0);
    y = FastEl(1);
    for (int i = 2; i < n_dims; ++i)
      y *= FastEl(i);
  }
}

void MatrixGeom::Get2DGeomGui(int& x, int& y, bool odd_y, int spc) const {
  if (spc < 0) spc = 0;
  x = 1;
  y = 1;
  if(n_dims == 1) {
    if(odd_y)
      y = FastEl(0);
    else
      x = FastEl(0);
  }
  else if(n_dims == 2) {
    x = FastEl(0);
    y = FastEl(1);
  }
  else if(n_dims == 3) { // series of 2d guys: layout vert or horiz?? vert!?
    if(odd_y) {
      x = FastEl(0);
      y = (FastEl(1) + spc) * FastEl(2); // assume space with 1
    }
    else {
      x = (FastEl(0) + spc) * FastEl(2);
      y = FastEl(1);
    }
  }
  else if(n_dims == 4) { // matrix of 2d guys
    x = (FastEl(0) + spc) * FastEl(2);
    y = (FastEl(1) + spc) * FastEl(3);
  }
  else if(n_dims == 5) { // matrix of 2d guys + time series: vertical
    if(odd_y) {
      x = (FastEl(0) + spc) * FastEl(2);
      y = (FastEl(1) + spc) * FastEl(3) * FastEl(4);
    }
    else {
      x = (FastEl(0) + spc) * FastEl(2) * FastEl(4);
      y = (FastEl(1) + spc) * FastEl(3);
    }
  }
}

int MatrixGeom::colCount(bool pat_4d) const {
  if (n_dims == 0)
    return 1;
  else if (n_dims < 4)
    return el[0];
  else {	
    if (pat_4d)
      return (el[0] * el[2]);
    else return (el[0]);
  }
}

int MatrixGeom::rowCount(bool pat_4d) const {
  if (n_dims <= 1)
    return 1;
  // more than 2d, return # flat rows
  int rval = el[1];
  if (pat_4d && (n_dims >= 4)) { //note: el[2] is part of cols
    for (int i = 3; i < n_dims; ++i)
      rval *= el[i];
  } else {
    for (int i = 2; i < n_dims; ++i)
      rval *= el[i];
  }
  return rval;
  
}

String MatrixGeom::ToString(const char* ldelim, const char* rdelim) const {
  String rval(ldelim);
  rval += String(n_dims) + ":";
  int i;
  for (i = 0; i < n_dims-1; ++i) {
    rval += String(el[i]) + ",";
  }
  rval += String(el[i]) + rdelim;
  return rval;
}

String& MatrixGeom::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << '[';
  int i;
  for (i = 0; i < n_dims-1; ++i) {
    strm += String(el[i]) + ",";
  }
  strm += String(el[i]) + ']';
  return strm;
}

void MatrixGeom::FromString(const String& str_, const char* ldelim, const char* rdelim) {
  String str = str_.after(ldelim);
  String ds = str.before(':');
  str = str.after(':');
  SetDims((int)ds);
  int i;
  for(i=0;i<n_dims-1;i++) {
    ds = str.before(',');
    str = str.after(',');
    Set(i, (int)ds);
  }
  ds = str.before(rdelim);
  str = str.after(rdelim);
  Set(i, (int)ds);
}

String MatrixGeom::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
			      bool force_inline) const {
  // always inline effectively
  return ToString();
}

bool MatrixGeom::SetValStr(const String& val, void* par, MemberDef* memb_def, 
			   TypeDef::StrContext sc, bool force_inline) {
  // always inline effectively
  FromString(val);
  return true;
}

int MatrixGeom::Dump_Save_Value(ostream& strm, taBase*, int) {
  strm << "{ ";
  int i;
  for (i=0; i < n_dims; i++) {
    strm << FastEl(i) << ";";
  }
  return true;
}

int MatrixGeom::Dump_Load_Value(istream& strm, taBase*) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)    return EOF;
  if(c == ';') // just a path
    return 2;  // signal that just a path was loaded..

  if(c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  c = taMisc::read_till_rb_or_semi(strm);
  int cnt = 0;
  int val;
  while ((c == ';') && (c != EOF)) {
    val = taMisc::LexBuf.toInt();
    if (cnt > n_dims-1)
      AddDim(val);
    else Set(cnt, val);
    ++cnt;
    c = taMisc::read_till_rb_or_semi(strm);
  }
  if (c==EOF)	return EOF;
  SetDims(cnt);			// just to be double sure it is same as loaded size
  UpdateAfterEdit_impl();
  return true;
}

bool MatrixGeom::SetDims(int new_sz) {
  if ((new_sz < 0) || (new_sz >= TA_MATRIX_DIMS_MAX)) return false;
  if(n_dims == new_sz) return false;
  // zero out orphaned old elements
  for (int i = n_dims - 1; i >= new_sz; --i)
    el[i] = 0;
  // zero out new elements
  for (int i = n_dims; i < new_sz; ++i)
    el[i] = 0;
  n_dims = new_sz;
  UpdateAfterEdit_impl();
  return true;
}


//////////////////////////
//  taMatrix		//
//////////////////////////

bool taMatrix::GeomIsValid(int dims_, const int geom_[],
  String* err_msg, bool allow_flex)
{
  if ((dims_ <= 0) || (dims_ >= TA_MATRIX_DIMS_MAX)) { 
    if (err_msg)
      *err_msg = "dims must be: 0 < dims <= " + String(TA_MATRIX_DIMS_MAX) + 
        " was: " + String(dims_);
    return false;
  }
  
  // we only allow a non-zero value in top dim (flex sizing, if enabled)
  for (int i = 0; i < (dims_ - 1) ; ++i) {
    if (geom_[i] < 0) {
      if (err_msg)
        *err_msg = "geoms must be >= 0";
      return false;
    } else if (geom_[i] == 0) {
      if (!(allow_flex && (i == (dims_ - 1)))) {
        if (err_msg)
          *err_msg = "all but highest-most geom must be >0";
        return false;
      }
    }
  }
  
  return true;
}

void taMatrix::SliceDestroying(taMatrix* par_slice, taMatrix* child_slice) {
  par_slice->slices->RemoveEl(child_slice);
  // note: having already sliced, we leave the list now in place
  taBase::UnRef(par_slice);
}

void taMatrix::SliceInitialize(taMatrix* par_slice, taMatrix* child_slice) {
  taBase::Ref(par_slice);
  if (!par_slice->slices) {
    par_slice->slices = new taMatrix_PList;
  }
  par_slice->slices->Add(child_slice);
  child_slice->slice_par = par_slice;
}

void taMatrix::Initialize() {
  size = 0;
  alloc_size = 0;
  slices = NULL;
  slice_par = NULL;
  fixed_dealloc = NULL;
  table_model = NULL;
  el_view_mode = IDX_UNK;
}
 
void taMatrix::Destroy() {
  CutLinks();
  size = 0;
  alloc_size = 0;
  if (slice_par) {
    SliceDestroying(slice_par, this);
    slice_par = NULL; 
  }
  if (sliceCount() > 0) {
    DebugInfo("Destroy", "taMatrix being destroyed with slice_cnt=", String(sliceCount()));
  }
  if (slices) {
    delete slices;
    slices = NULL;
  }
  if (table_model) {
    //note: hopefully dudes won't call us back during this!
    delete table_model;
    table_model = NULL;
  }
}

void taMatrix::InitLinks() {
  inherited::InitLinks();
  taBase::Own(geom, this);
  taBase::Own(el_view, this);
}

void taMatrix::CutLinks() {
  el_view.CutLinks();
  el_view_mode = IDX_UNK;
  geom.CutLinks();
  el_view.CutLinks();
  inherited::CutLinks();
}

void taMatrix::BatchUpdate(bool begin, bool struc) {
  inherited::BatchUpdate(begin, struc);
  // recursively send blocked updates to slices
  if (slices) for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    mat->BatchUpdate(begin, struc); // recursive, of course
  }
}

String& taMatrix::Print(String& strm, int indent) const {
  if(size > 500) {
    strm << GetTypeDef()->name + " geom: ";
    geom.Print(strm);
    return strm;
  }

  const int dm = dims();
  int dim_break = dm / 2;
  if(dm == 2) {
    float dim_rat = (float)dim(1) / dim(0);
    if(dim_rat > 2.0)		// if inner ratio is small, then keep it inline
      dim_break = dm;
  }
  else if(dm % 2 == 1) {
    dim_break++;		// one more outer is probably better for odd
  }

  taMatrix* elv = ElView();
  MatrixIndex idx(dm);
  MatrixIndex lstidx(dm,0,0,0,0,0,0,0);
  for(int d=0; d<dm; d++) {
    strm += "[";
  }
  strm += " ";
  if(!elv || el_view_mode == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(elv);
    for(int i=0; i<size; i++) {
      geom.DimsFmIndex(i, idx);
      int sc = 0;
      int break_level = -1;
      for(int d=0; d<dm; d++) {
	if(idx[d] == 0 && idx[d] != lstidx[d]) {
	  sc++;
	  if(sc == 1) strm += " ";
	  strm += "]";		// end previous
	  if(d+1 == dim_break) {
	    break_level = d+1;
	  }
	}
      }
      if(break_level >= 0) {
	strm += "\n" + String(dm-break_level, 0, ' ');
      }
      for(int s=0; s<sc; s++) {
	strm += "[";		// start new
	if(s == sc-1) strm += " ";
      }
      if(sc == 0 && i > 0) {
	strm += ", ";
      }
      if(cmat && !((bool)cmat->SafeEl_Flat(i))) {
	strm += "- ";		// filtered
      }
      else {
	strm += FastElAsStr_Flat(i);
      }
      lstidx = idx;		// update
    }
    strm += " ";
    for(int d=0; d<dm; d++) {
      strm += "]";
    }
  }
  else {			// coords!
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    int nc = cmat->dim(1);
    strm = "[ ";
    for(int i=0; i<nc; i++) {
      for(int d=0;d<dm;d++) {
	idx.Set(d, cmat->FastEl(d, i));	// outer index is count index
      }
      idx.Print(strm) << ": " << SafeElAsStrN(idx);
      if(i < nc-1)
	strm += ", ";
    }
    strm += " ]";
  }
  return strm;
}

//////////////////////////////////////////////
// 	std accessor interface

bool taMatrix::SetElView(taMatrix* view_mat, IndexMode md) {
  int dm = dims();
  if(!IterValidate(view_mat, md, dm)) return false;
  el_view = view_mat;
  el_view_mode = md;
  return true;
}

taMatrix* taMatrix::NewElView(taMatrix* view_mat, IndexMode md) const {
  int dm = dims();
  if(!IterValidate(view_mat, md, dm)) return NULL;
  taMatrix* rval = (taMatrix*)MakeToken(); // make a token of me
  void* base_el = const_cast<void*>(FastEl_Flat_(0));
  rval->SetFixedData_(base_el, geom);	   // identical geom, same data
  SliceInitialize(const_cast<taMatrix*>(this), rval);
  rval->SetElView(view_mat, md);
  return rval;
}

Variant taMatrix::ElemFmCoord(int_Matrix* cmat) const {
  MatrixIndex idx;
  int dm = dims();
  idx.SetDims(dm);		// set it to our dimensionality
  int mx = MIN(dm, cmat->size);
  int i;
  for(i=0; i<mx; i++) {
    int el_idx = cmat->FastEl_Flat(i);
    if(el_idx < 0) el_idx += dim(i);
    if(el_idx < 0 || el_idx >= dim(i)) {
      return _nilVariant;	// out of bounds!
    }
    idx.Set(i, el_idx);
  }
  for(;i<dm;i++) {		// fill in remaining indicies with 0 
    idx.Set(i, 0);
  }
  return SafeElAsVarN(idx);
}

Variant taMatrix::Elem(const Variant& idx, IndexMode mode) const {
  const int dm = dims();
  if(mode == IDX_UNK) {
    mode = IndexModeDecode(idx, dm);
    if(mode == IDX_UNK) return _nilVariant;
  }
  if(!IndexModeValidate(idx, mode, dm))
    return _nilVariant;
  switch(mode) {
  case IDX_IDX: {
    return SafeElAsVar_Flat(idx.toInt());
    break;
  }
  case IDX_NAME:
  case IDX_MISC:
  case IDX_NAMES: {
    TestError(true, "Elem::IDX_NAME/S or MISC", "index type not support for Matrix");
    return _nilVariant;
    break;
  }
  case IDX_COORD: {
    // always return a matrix so that assignment works!
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    int_Matrix* ccmat = new int_Matrix;
    MatrixGeom ng = cmat->geom;
    ng.AddDim(1);
    ccmat->SetGeomN(ng);
    for(int i=0; i<cmat->size; i++) {
      ccmat->FastEl_Flat(i) = cmat->FastEl_Flat(i);
    }
    // return ElemFmCoord(cmat);
    taMatrix* nwvw = NewElView(ccmat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_COORDS: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    // if(cmat->dim(1) == 1) {
    //   return ElemFmCoord(cmat);
    // }
    taMatrix* nwvw = NewElView(cmat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_SLICE: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    int_Matrix fixsmat;
    fixsmat.SetGeom(2,3,dm); // fixed slice matrix + total n
    MatrixGeom sliceg(dm);		// slice geometry
    for(int i=0;i<dm; i++) {
      int start = 0; int end = -1; int step = 1;
      if(i < cmat->dim(1)) {
	start = cmat->FastEl(0,i);
	end = cmat->FastEl(1,i);
	step = cmat->FastEl(2,i);
      }
      if(step == 0) step = 1;
      if(FixSliceValsFromSize(start, end, dim(i))) {
	int my_n = (end-start) / ABS(step); // number of guys in my slice
	sliceg.Set(i, my_n);
	fixsmat.FastEl(0,i) = start; 
	fixsmat.FastEl(1,i) = end; 
	fixsmat.FastEl(2,i) = step;
      }
      else {
	return _nilVariant;
      }
    }
    int tot_n = sliceg.Product();
    if(TestError(tot_n <= 0, "Elem", "slicing total was empty")) {
      return _nilVariant;
    }
    int_Matrix* imat = new int_Matrix(2,dm,tot_n); // turn into coords list
    MatrixIndex sidx;				   // slice idx
    for(int i=0;i<tot_n; i++) {
      sliceg.DimsFmIndex(i, sidx); // get index into slice vals
      for(int d=0; d<dm; d++) {
	int start = fixsmat.FastEl(0,d);
	int end = fixsmat.FastEl(1,d);
	int step = fixsmat.FastEl(2,d);
	int sc;
	if(step > 0) {
	  sc = start + step * sidx[d];
	}
	else {
	  sc = end-1 + step * sidx[d];
	}
	imat->FastEl(d, i) = sc;
      }
    }
    taMatrix* nwvw = NewElView(imat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_MASK: {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(idx.toMatrix());
    if(TestError(cmat->geom != geom, "Elem::IDX_MASK",
		 "mask matrix geometry:", String(cmat->dim(0)),
		 "is not size of list:", String(size)))
      return false;
    if(el_view && el_view_mode == IDX_MASK) {
      // take intersection of the existing mask
      byte_Matrix* am = (byte_Matrix*)(*cmat && *ElView());
      taBase::Ref(am);
      cmat->Copy(am);
      taBase::UnRef(am);
    }
    taMatrix* nwvw = NewElView(cmat, IDX_MASK);
    return (Variant)nwvw;
    break;
  }
  }
  return _nilVariant;
}

taBaseItr* taMatrix::Iter() const {
  taBaseItr* rval = new taBaseItr;
  taBase::Ref(rval);
  return rval;
}

Variant	taMatrix::IterElem(taBaseItr* itr) const {
  if(!itr) return _nilVariant;
  return SafeElAsVar_Flat(itr->el_idx);
}

bool taMatrix::IterValidate(taMatrix* vmat, IndexMode mode, int cont_dims) const {
  bool rval = inherited::IterValidate(vmat, mode, cont_dims);
  if(!rval) return false;
  if(!vmat) return true;
  if(el_view_mode == IDX_MASK) {
    if(TestError(ElView()->geom != geom, "IterValidate::IDX_MASK",
		 "el_view geom:", ElView()->geom.ToString(),
		 "not equal to size of matrix:",
		 geom.ToString()))
      return false;
  }
  return true;
}

bool taMatrix::IterFirst_impl(taBaseItr*& itr) const {
  if(!itr) return false;
  itr->count = 0;
  itr->el_idx = 0;		// just to be sure
  const int dm = dims();
  if(!ElView()) {
    if(ElemCount() > 0) return true;
    return false;
  }
  if(!IterValidate(ElView(), el_view_mode, dm)) {
    return false;
  }
  if(el_view_mode == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(cmat->size == 0) {
      return false;
    }
    MatrixIndex idx(dm);
    for(int d=0;d<dm;d++) {
      idx.Set(d, cmat->FastEl(d, 0));	// outer index is count index
    }
    itr->el_idx = FastElIndexN(idx);
    if(itr->el_idx < 0 || itr->el_idx >= ElemCount()) {
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=0; i<ElemCount(); i++) {
      if(cmat->FastEl_Flat(i) != 0) {
	itr->el_idx = i;
	return true;
      }
    }
  }
  return false;
}

bool taMatrix::IterNext_impl(taBaseItr*& itr) const {
  if(!itr) return false;
  itr->count++;
  const int dm = dims();
  if(!ElView()) {
    itr->el_idx++;
    if(itr->el_idx >= ElemCount()) {
      return false;
    }
    return true;
  }
  if(el_view_mode == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(cmat->dim(1) <= itr->count) {
      return false;
    }
    MatrixIndex idx(dm);
    for(int d=0;d<dm;d++) {
      idx.Set(d, cmat->FastEl(d, itr->count));	// outer index is count index
    }
    itr->el_idx = FastElIndexN(idx);
    if(itr->el_idx < 0 || itr->el_idx >= ElemCount()) {
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=itr->el_idx+1; i<ElemCount(); i++) { // search for next
      if(cmat->FastEl_Flat(i) != 0) { // true
	itr->el_idx = i;
	return true;
      }
    }
  }
  return false;
}

void taMatrix::Add_(const void* it) {
  if(TestError(!canResize(), "Add", "resizing not allowed")) return;
  if(TestError((dims() != 1), "Add", "only allowed when dims=1")) return;
  int idx = frames();
  if(EnforceFrames(idx + 1))
    El_Copy_(FastEl_Flat_(idx), it);
}

bool taMatrix::AddFrames(int n) {
  return EnforceFrames(n + frames());
}

bool taMatrix::Alloc_(int new_alloc) {
  if(TestError((alloc_size < 0), "Alloc_", "cannot alloc a fixed data matrix")) return false;
  // TODO  Check((slice_cnt == 0), "cannot alloc a sliced data matrix");
  // NOTE: this is a low level allocator; alloc is typically managed in frames
  if (alloc_size < new_alloc)	{
    if(fastAlloc() && (alloc_size > 0)) {		// fast alloc = malloc/realloc
      char* old = (char*)data();
      if(!FastRealloc_(new_alloc))
	return false;
      ta_intptr_t delta = (char*)data() - old;
      UpdateSlices_Realloc(delta);
    }
    else {
      char* nw = (char*)MakeArray_(new_alloc);
      if(TestError(!nw, "Alloc_", "could not allocate matrix memory -- matrix is too big!  reverting to old size -- could be fatal!")) {
	return false;
      }
      for (int i = 0; i < size; ++i) {
	El_Copy_(nw + (El_SizeOf_() * i), FastEl_Flat_(i));
      }
      // calculate delta, in bytes, of the new address and update slices
      ta_intptr_t delta = nw - (char*)data();
      UpdateSlices_Realloc(delta);
      // we can now update ourself
      SetArray_(nw);
    }
    alloc_size = new_alloc;
  }
  return true; // this impl assumes this can never fail!
}

bool taMatrix::AllocFrames(int n) {
  if(TestError((alloc_size < 0), "AllocFrames", "cannot alloc a fixed data matrix")) return false;
  if(TestError((n < 0), "AllocFrames", "n (num frames) must be >= 0")) return false;
  int frsz = frameSize();
  if(frsz == 0) return false;		// not dimensioned yet -- don't bother
  int cur_n = alloc_size / frsz;
  if(cur_n >= n) return true;	// already sufficient!

  int act_n = n;		// actual n frames to request
  // start w/ 4, double up to 64, then 1.5x thereafter
  if (cur_n == 0) act_n = MAX(4, act_n);
  else if (cur_n < 64) act_n = MAX((cur_n * 2), act_n);
  else act_n =  MAX(((cur_n * 3) / 2), act_n);
  return Alloc_(act_n * frsz);
}

bool taMatrix::canResize() const {
  return (alloc_size >= 0);
}

void taMatrix::Clear(int fm, int to) {
  if (size == 0) return;
  if (to < 0) to = size - 1; 
  if (fm > to) return;
  if (TestError((fm >= size), "Clear", "fm exceeds size")) return;
  if (TestError((to >= size), "Clear", "to exceeds size")) return;
  Clear_impl(fm, to);
} 

void taMatrix::Clear_impl(int fm, int to) {
  if (fastAlloc()) {
    size_t sz = ((to - fm) + 1) * El_SizeOf_();
    memset(FastEl_Flat_(fm), 0, sz);
  } else {
    const void* bl = El_GetBlank_();
    for (int i = fm; i <= to; ++i) {
      El_Copy_(FastEl_Flat_(i), bl);
    }
  }
}


void taMatrix::CanCopyCustom_impl(bool to, const taBase* cp, bool quiet,
    bool& allowed, bool& forbidden) const
{
  if (to) return; // no strictures
  if (cp->InheritsFrom(&TA_taMatrix)) {
    allowed = true; // generally allowed
  }
}

void taMatrix::CopyFromCustom_impl(const taBase* cp_fm) {
  if (cp_fm->InheritsFrom(&TA_taMatrix)) {
    Copy_Matrix_impl(static_cast<const taMatrix*>(cp_fm));
  }
  else inherited::CopyFromCustom_impl(cp_fm); // unlikely/prob doesn't happen
}

void taMatrix::Copy_Matrix_impl(const taMatrix* cp) {
  // note: caller has asserted Struct guys
  // assumes copy has been validated
  inherited::Copy_impl(*cp); // do all common generic parent class copying
  SetBaseFlag(COPYING); // note: still have to set/reset here, because not nestable
  SetGeomN(cp->geom);
  for (int i = 0; i < size; ++i) {
    El_SetFmVar_(FastEl_Flat_(i), cp->FastElAsVar_Flat(i));
  }  
  ClearBaseFlag(COPYING);
}

bool taMatrix::CopyFrame(const taMatrix& src, int frame) {
  if (!src.geom.IsFrameOf(geom)) return false;
  if ((frame < 0) || (frame >= frames())) return false;
  int n = frameSize();
  int base = FrameStartIdx(frame);
  // if same data types, we use an optimized copy, else must use variants
  // note that "Inherits" should imply same data type
  DataUpdate(true);
  if (GetTypeDef()->InheritsFrom(src.GetTypeDef())) {
    for (int i = 0; i < n; ++i) {
      El_Copy_(FastEl_Flat_(base + i), src.FastEl_Flat_(i));
    }
  } else {
    for (int i = 0; i < n; ++i) {
      El_SetFmVar_(FastEl_Flat_(base + i), src.El_GetVar_(src.FastEl_Flat_(i)));
    }
  }
  DataUpdate(false);

  return true;
}

void taMatrix::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if (slice_par) {
    slice_par->DataChanged(dcr, op1, op2);
  }
}

taBase* taMatrix::GetOwner() const { 
  if (slice_par) return slice_par; 
  return owner;
}

int taMatrix::defAlignment() const {
  return Qt::AlignRight; // most mats are numeric, so this is the default
}

String taMatrix::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
			      bool force_inline) const {
  // always inline effectively
  String rval = geom.GetValStr(par, memb_def, sc, force_inline);
  rval += " {";
  for(int i=0;i<size;i++) {
    rval += " " + FastElAsStr_Flat(i) + ",";
  }
  rval += "}";
  return rval;
}

bool taMatrix::SetValStr(const String& val, void* par, MemberDef* memb_def, 
			 TypeDef::StrContext sc, bool force_inline) {
  // always inline effectively
  String gmstr = val.before('{');
  MatrixGeom ng;
  ng.SetValStr(gmstr, par, memb_def, sc, force_inline);
  SetGeomN(ng);
  int idx = 0;
  String tmp = val.after('{');
  while(tmp.length() > 0) {
    String el_val = tmp.before(',');
    if(el_val.empty()) {
      el_val = tmp.before('}');
      if (el_val.empty())
	break;
    }
    tmp = tmp.after(',');
    if(TestError(idx >= size, "SetValStr",
		 "more items in val string than shoudl be according to geometry!")) break;
    if (el_val.contains(' '))
      el_val = el_val.after(' ');
    SetFmStr_Flat(el_val, idx);
    idx++;
  }
  return true;
}

int taMatrix::ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
			    void* par, TypeDef* par_typ, MemberDef* memb_def, TypeDef::StrContext sc) {
  int rval = 0;
  String mypath = GetPathNames();
  for(int i=0; i<size; i++) {
    String str = FastElAsStr_Flat(i);
    if(!str.contains(srch)) continue;
    String orig = str;
    rval += str.gsub(srch, repl);
    SetFmStr_Flat(str, i);
    taMisc::Info("Replaced string value in matrix object:",
		 mypath,"orig val:", orig, "new val:", str);
  }
  if(rval > 0)
    UpdateAfterEdit();
  return rval;
}

int taMatrix::Dump_Load_Item(istream& strm, int idx) {
  int c = taMisc::read_till_semi(strm);
  if (c != EOF) {
    SetFmStr_Flat(taMisc::LexBuf, idx);
  }
  return c;
}

int taMatrix::Dump_Load_Value(istream& strm, taBase* par) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)	return EOF;
  if(c == ';')	return 2;	// signal that its a path
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    return 2;
  }

  if (c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  // now, load members (if we have dims, will exit at that point)
  int rval = GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
  // 3 is a hacky code to tell us that it got the [ 
  if ((rval != 3) || (rval == EOF)) return rval;
  
  // we now expect dims, if not completely null
  c = taMisc::skip_white(strm);
  if (c == EOF)    return EOF;
  
  if (c == '[') {
    MatrixGeom ar; // temp, while streaming
    if (strm.peek() == '[')
      strm.get(); // actually gets the [
    do {
      c = taMisc::read_word(strm); // also consumes next char, whether sp or ]
      ar.AddDim(taMisc::LexBuf.toInt());
    } while ((c != ']') && (c != EOF));
    //note: should always be at least one dim if we had [ but we check anyway
    if (ar.dims() > 0) {
      SetGeomN(ar);
      //note: we always write the correct number, so early termination is an error!
      int i = 0;
      while ((i < size) && (c != EOF)) {
        c = Dump_Load_Item(strm, i++);
      }
    }
  }
  // because we had data, we have to clean up
  c = taMisc::read_till_rbracket(strm);
  if (c==EOF)	return EOF;
  c = taMisc::read_till_semi(strm);
  if (c==EOF)	return EOF;
  return true;
}

void taMatrix::Dump_Save_Item(ostream& strm, int idx) {
  strm << FastElAsStr_Flat(idx);
}

int taMatrix::Dump_Save_Value(ostream& strm, taBase* par, int indent) {
  // save the members -- it puts the { out
  //int rval = 
  inherited::Dump_Save_Value(strm, par, indent);
  
  // save data, if not completely null
  int i;
  if (geom.dims() > 0) {
    taMisc::indent(strm, indent);
    // dims
    strm << "[";
    for (i=0; i< geom.dims(); ++i) {
      if (i > 0) strm << " ";
      strm << geom.FastEl(i);
    }
    strm << "] ";
    // values
    for (i=0; i < size; ++i) {
      Dump_Save_Item(strm, i);
      strm <<  ';';
      if((i+1) % 10 == 0)	// this helps a lot with readability and diffs..
	strm << '\n';
    }
  }
 
  return true;
}

/////////////////////////////////////
// 	Binary Dump Format

int taMatrix::BinaryLoad_strm(istream& strm) {
  if(TestError(!BinaryFile_Supported(), "BinaryLoad", "Binary load is not supported for this type of matrix")) return false;
  int c = taMisc::skip_white(strm);
  if(c == EOF)	return EOF;
  if(c == ';')	return 2;	// signal that its a path
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    return 2;
  }

  if (c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  // now, load members (if we have dims, will exit at that point)
  int rval = GetTypeDef()->members.Dump_Load(strm, (void*)this, NULL);
  // 3 is a hacky code to tell us that it got the [ 
  if ((rval != 3) || (rval == EOF)) return rval;
  
  // we now expect dims, if not completely null
  c = taMisc::skip_white(strm);
  if (c == EOF)    return EOF;
  
  if (c == '[') {
    MatrixGeom ar; // temp, while streaming
    if (strm.peek() == '[')
      strm.get(); // actually gets the [
    do {
      c = taMisc::read_word(strm); // also consumes next char, whether sp or ]
      ar.AddDim(taMisc::LexBuf.toInt());
    } while ((c != ']') && (c != EOF));
    //note: should always be at least one dim if we had [ but we check anyway
    if (ar.dims() > 0) {
      SetGeomN(ar);
      //note: we always write the correct number, so early termination is an error!
      int i = 0;
      while (i < size) {
        BinaryLoad_Item(strm, i++);
      }
    }
  }
  // because we had data, we have to clean up
  c = taMisc::read_till_rbracket(strm);
  if (c==EOF)	return EOF;
  c = taMisc::read_till_semi(strm);
  if (c==EOF)	return EOF;
  return true;
}

int taMatrix::BinarySave_strm(ostream& strm) {
  if(TestError(!BinaryFile_Supported(), "BinarySave", "Binary save is not supported for this type of matrix")) return false;
  // save the members -- it puts the { out
  //int rval = 
  inherited::Dump_Save_Value(strm, NULL, 0);
  
  // save data, if not completely null
  int i;
  if (geom.dims() > 0) {
    taMisc::indent(strm, 0);
    // dims
    strm << "[";
    for (i=0; i< geom.dims(); ++i) {
      if (i > 0) strm << " ";
      strm << geom.FastEl(i);
    }
    strm << "]";
    // values
    for (i=0; i < size; ++i) {
      BinarySave_Item(strm, i);
    }
  }
  strm << endl << "};" << endl;	// terminate
  return true;
}

void taMatrix::BinarySave(const String& fname) {
  if(TestError(!BinaryFile_Supported(), "BinarySave", "Binary save is not supported for this type of matrix")) return;
  taFiler* flr = GetSaveFiler(fname, ".mat", false, "Binary Matrix");
  if (flr->ostrm) {
    BinarySave_strm(*flr->ostrm);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

void taMatrix::BinaryLoad(const String& fname) {
  if(TestError(!BinaryFile_Supported(), "BinaryLoad", "Binary load is not supported for this type of matrix")) return;
  taFiler* flr = GetLoadFiler(fname, ".mat", false, "Binary Matrix");
  if(flr->istrm)
    BinaryLoad_strm(*flr->istrm);
  flr->Close();
  taRefN::unRefDone(flr);
}

// This is *the* routine for resizing, so all data change ops/tracking
// can go through this
bool taMatrix::EnforceFrames(int n, bool notify) {
  // note: we enforce the size in terms of underlying cells, for when
  // dimensions are changed (even though that is frowned on...)
  if (!AllocFrames(n)) return false; // does legality test
  int new_size = n * frameSize();
  // avoid spurious notifies -- geom changes do their own notify
  if (new_size == size) {
    geom.Set(geom.dims()-1, n);	
    return true;
  }
  StructUpdate(true);
  if (new_size > size) {
    const void* blank = El_GetBlank_();
    for (int i = size; i < new_size; ++i) {
      El_Copy_(FastEl_Flat_(i), blank);
    }
    size = new_size;
    geom.Set(geom.dims()-1, n);
  } else if (new_size < size) {
    ReclaimOrphans_(new_size, size - 1);
    size = new_size;
    geom.Set(geom.dims()-1, n);	
  }
  StructUpdate(false);
  return true;
}

int taMatrix::FindVal_Flat(const Variant& val, int st_idx) const {
  for(int i=st_idx;i<size; i++) {
    Variant mval = FastElAsVar_Flat(i);
    if(mval == val) return i;
  }
  return -1;
}
 
const String taMatrix::FlatRangeToTSV(int row_fr, int col_fr, int row_to, int col_to) {
  if ((row_fr < 0) || (col_fr < 0) || (row_to < row_fr) || (col_to < col_fr))
    return _nilString;
  // allocate a reasonable best-guess buffer
  STRING_BUF(rval, (col_to - col_fr + 1) * (row_to - row_fr + 1) * 10);
  // to access in 2d, you just ignore the higher dimensions
  for (int row = row_fr; row <= row_to; ++row) {
    if (row > row_fr) rval.cat('\n');
    //int idx = (row * dim(0)) + col_fr; 
    for (int col = col_fr; col <= col_to; ++col/*, ++idx*/) {
      if (col > col_fr) rval.cat('\t');
      // note: we assume range must already have been converted from display to canonical
      // so we explicitly request the TOP_ZERO (no conversion) format
      int idx = geom.IndexFmDims2D(col, row, true, taMisc::TOP_ZERO);
      rval.cat(SafeElAsStr_Flat(idx));
    }
  }
  return rval;
}

int taMatrix::frames() const {
  if (geom.dims() == 0) return 0;
  return geom[geom.dims()-1];
}

int taMatrix::frameSize() const {
  if (geom.dims() == 0) return 0;
  if (geom.dims() == 1) return 1;
  int rval = geom[0];
  for (int i = 1; i < (geom.dims() - 1); ++i) 
    rval *= geom[i];
  return rval;
}

int taMatrix::FrameToRow(int f) const {
  // this needs to work even when dims<=1, so we just return f
  if (dims() <= 1) return f;
  // for dims==2, we just return f, else we multiple by higher dims
  for (int i = dims() - 2; i >= 1; --i)
    f *= dim(i);
  return f;
}

taMatrix* taMatrix::GetFrameSlice_(int frame) {
  int dims_m1 = dims() - 1; //cache
  if(TestError((dims_m1 <= 0),"GetFrameSlice_", "dims must be >1 to GetFrameSlice"))
    return NULL;
  int frames_ = frames(); // cache
  // check frame_base and num_frames in bounds
  if(TestError(((frame < 0) || (frame >= frames_)), "GetFrameSlice_",
	       "frame is out of bounds")) return NULL;
    
  MatrixGeom slice_geom(dims_m1);
  for (int i = 0; i < dims_m1; ++i)
    slice_geom.Set(i, dim(i));
  int sl_i = FrameStartIdx(frame); //note: must be valid because of prior checks
  void* base_el = FastEl_Flat_(sl_i);
  taMatrix* rval = FindSlice(base_el, slice_geom);
  if (rval) return rval;
  
  rval = (taMatrix*)MakeToken(); // an empty instance of our type
  if(TestError((!rval), "GetFrameSlice_", "could not make token of matrix")) return NULL;
  
  rval->SetFixedData_(base_el, slice_geom);
  // we do all the funky ref counting etc. in one place
  SliceInitialize(this, rval);
  return rval;
}

taMatrix* taMatrix::GetSlice_(const MatrixIndex& base, 
    int slice_frame_dims, int num_slice_frames)
{
  if(TestError((num_slice_frames <= 0), "GetSlice_", "num_slice_frames must be >= 1"))
    return NULL;
  // (note: we check resulting slice dims in bounds later)
  if (slice_frame_dims == -1)
    slice_frame_dims = dims() - 1;
  // check dim size in bounds
  if(TestError((slice_frame_dims < 0) || (slice_frame_dims >= dims()),
	       "GetSlice_", "slice_frame_dims must be >= 0 and < parent Matrix"))
    return NULL;
  // check start cell in bounds and legal
  int sl_i = SafeElIndexN(base); // -1 if out of bounds
  if(TestError((sl_i < 0), "GetSlice_", "slice base is out of bounds")) return NULL;

  // create geom of slice, and copy our dims
  // note that since we are talking in frames, the dims = frames+1
  MatrixGeom slice_geom(slice_frame_dims + 1); // geometry of the resulting slice
  for (int i = 0; i < slice_frame_dims; ++i)
    slice_geom.Set(i, dim(i));
  slice_geom.Set(slice_frame_dims, num_slice_frames);
  
  // easiest to now check for frames in bounds
  int sl_tot = slice_geom.Product();
  if(TestError(((sl_i + sl_tot) > size), "GetSlice_", "slice end is out of bounds"))
    return NULL;
    
  void* base_el = FastEl_Flat_(sl_i);
  taMatrix* rval = FindSlice(base_el, slice_geom);
  if (rval) return rval;
  
  rval = (taMatrix*)MakeToken(); // an empty instance of our type
  if(TestError((!rval), "GetSlice_", "could not make token of matrix")) return NULL;
  
  rval->SetFixedData_(base_el, slice_geom);
  // we do all the funky ref counting etc. in one place
  SliceInitialize(this, rval);
  return rval;
}

taMatrix* taMatrix::GetFrameRangeSlice_(int st_frame, int n_frames) {
  const int dm = dims();
  MatrixIndex base(dm);
  for (int i = 0; i < dm-1; ++i)
    base.Set(i, 0);
  base.Set(dm-1, st_frame);
  return GetSlice_(base, dm-1, n_frames);
}

taMatrix* taMatrix::FindSlice(void* el_, const MatrixGeom& geom_) {
  if (slices) for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    if (!mat) continue; // shouldn't happen
    if ((mat->data() == el_) && (mat->geom == geom_))
      return mat;
  }
  return NULL;
} 

bool taMatrix::InRange(int d0, int d1, int d2, int d3, int d4, int d5, int d6) const {
  switch (geom.dims()) {
  case 0: return false; // not initialized
  case 1: return ((d0 >= 0) && (d0 < geom[0]));
  case 2: return ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]));
  case 3: return ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]));
  case 4: return ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]))
    && ((d3 >= 0) && (d3 < geom[3]));
  case 5: ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]))
    && ((d3 >= 0) && (d3 < geom[3]))
    && ((d4 >= 0) && (d4 < geom[4]));
  default: return false;
  }
}
 
 
bool taMatrix::InRangeN(const MatrixIndex& indices) const {
  if (indices.dims() < geom.dims()) return false;
  for (int i = 0; i < indices.dims(); ++i) {
    int di = indices[i];
    if ((di < 0) || (di >= geom[i])) return false;
  }
  return true;
}
 
bool taMatrix::RemoveFrames(int st_fr, int n_fr) {
  if(TestError(!canResize(), "RemoveFrames", "resizing not allowed")) return false;
  int frames_ = frames(); // cache
  if(st_fr < 0) st_fr = frames_ - 1;
  if(TestError(((st_fr < 0) || (st_fr >= frames_)), "RemoveFrames",
	       "starting frame number out of range:", String(st_fr)))
    return false;
  if(n_fr < 0) n_fr = frames_ - st_fr;
  int end_fr = st_fr + n_fr-1;
  if(TestError(((end_fr < 0) || (end_fr >= frames_)), "RemoveFrames",
	       "ending frame number out of range:", String(end_fr)))
    return false;
  // check if we have to copy data
  int frsz = frameSize();
  if(end_fr != (frames_ - 1)) {
    int fm = (end_fr + 1) * frsz;
    int to = st_fr * frsz;
    while (fm < size) {
      El_Copy_(FastEl_Flat_(to), FastEl_Flat_(fm));
      ++fm; ++to;
    }
  }
  // slice updating
  UpdateSlices_FramesDeleted(FastEl_Flat_(st_fr * frsz), n_fr);
  // notifies
  DataUpdate(true);
  // don't notify, because we are doing it (it can't boggle excisions)
  bool rval = EnforceFrames(frames_ - n_fr, false); // this properly resizes, and reclaims orphans
  DataUpdate(false);
  return rval;
}

bool taMatrix::InsertFrames(int st_fr, int n_fr) {
  if(TestError(!canResize(), "InsertFrames", "resizing not allowed")) return false;
  int sz = frames(); // cache
  if(st_fr < 0) st_fr = sz;
  if(TestError((st_fr>sz), "InsertFrames", "starting frame number out of range",
	       String(st_fr))) return false;
  if(TestError((n_fr <= 0), "InsertFrames", "number of frames <= 0")) return false;
  AddFrames(n_fr);
  if(st_fr==sz) {
    return true;		// done!
  }
  int n_mv = sz - st_fr;	// number that must be moved
  sz += n_fr;			// update to added size
  int frsz = frameSize();
  int trg_o = sz-1;
  int src_o = sz-1-n_fr;
  for(int i=0; i<n_mv; i++) {	// shift everyone over
    int trg_o_st = (trg_o - i) * frsz;
    int src_o_st = (src_o - i) * frsz;
    for(int j=0;j<frsz;j++) {
      El_Copy_(FastEl_Flat_(trg_o_st+j), FastEl_Flat_(src_o_st+j));
    }
  }
  // blank out new guys
  const void* blank = El_GetBlank_();
  int st = st_fr * frsz;
  int ed = (st_fr+n_fr) * frsz; 
  for (int i = st; i < ed; ++i) {
    El_Copy_(FastEl_Flat_(i), blank);
  }
  return true;
}

MatrixTableModel* taMatrix::GetTableModel() {
  if (!table_model && !isDestroying()) {
    table_model = new MatrixTableModel(this);
    table_model->setPat4D(true); // always
  }
  return table_model;
}

void taMatrix::Reset() {
  EnforceFrames(0);
  UpdateSlices_Collapse();
}


void taMatrix::SetFixedData_(void* el_, const MatrixGeom& geom_,
    fixed_dealloc_fun fixed_dealloc_) 
{
  // first, clear out any old data, use NULL first to dealloc if needed
  SetArray_(NULL);
  // now, set new data, and the new deallocer
  fixed_dealloc = fixed_dealloc_;
  SetArray_(el_);
  alloc_size = -1; // flag for fixed data
  geom.Reset();
  SetGeomN(geom_);
  
}

bool taMatrix::SetGeom_(int dims_, const int geom_[]) {
  // dims=0 is special case, which is just a reset
  if (dims_ == 0) {
    StructUpdate(true);
    Reset(); // deletes data, and collapses slices
    geom.Reset(); // ok, now you know what 0-d really means!!!
    StructUpdate(false);
    return true;
  } else {
    String err_msg;
    bool valid = GeomIsValid(dims_, geom_, &err_msg);
    if (TestError(!valid, "SetGeom_", err_msg)) return false;
  }
  
  // flex not allowed for fixed data
  if (isFixedData()) {
    if (TestError((geom_[dims_-1] == 0), "SetGeom_", 
      "fixed data cannot use flex sizing")) return false;
  }
  
  // NOTE: following routine is conservative of existing geom
  // if you are using flex sizing, it will collapse any existing!!!
  // only copy bottom N-1 dims, setting 0 frames -- we size frames in next step
  StructUpdate(true);
  // we collapse slices if #dims change, inner dims change, or frames go down
  bool collapse_slices = geom.SetDims(dims_);
  for (int i = 0; i < (dims_ - 1); ++i) {
    if(geom[i] != geom_[i]) {
      collapse_slices = true;
      geom.Set(i, geom_[i]);
    }
  }
  if (geom[dims_ -1] > geom_[dims_ - 1])
    collapse_slices = true;
  
  // assign storage if not fixed
  if (isFixedData()) {
    geom.Set(dims_ -1, geom_[dims_ - 1]);
    size = geom.Product();
  }
  else {
    // next step actually sets last geom
    if (!EnforceFrames(geom_[dims_-1])) {
      StructUpdate(false); // TODO: RAII
      return false; // failure
    }
  }
  if (collapse_slices) {
    UpdateSlices_Collapse();
  }
  StructUpdate(false);
  return true;
}

void taMatrix::Slice_Collapse() {
  // called when our referent has become invalid, for whatever reason
  SetArray_(NULL);
  geom.Reset();
  size = 0;
  // if we have collapsed, so have any of our slices...
  UpdateSlices_Collapse();
  DataChanged(DCR_ITEM_UPDATED);
}

void taMatrix::Slice_Realloc(ta_intptr_t base_delta) {
  // called when the base address of our slice has been moved, for whatever reason
  void* mat_el = data(); // cache
  if (mat_el) { // note already collapsed
    SetArray_((void*) (((char*)mat_el) + base_delta));
    // do it recursively
    UpdateSlices_Realloc(base_delta);
  }
  // note: we recursively updated other slices before doing our own notify
  DataChanged(DCR_ITEM_UPDATED);
}

int taMatrix::sliceCount() const {
  if (slices) return slices->size; // note: allowed to be zero
  else        return 0; // hasn't been needed yet
}

void taMatrix::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //NOTE: you are NOT allowed to change geom this way -- must use the SetGeom api call
  if (taMisc::is_loading) {
    UpdateGeom();
  }
}

void taMatrix::UpdateGeom() {
  // NOTE: this routine is ONLY intended for the case of updating after loading
  // from a stream -- no slice checking because there can't really be any yet!
  // handle legacy/graceful case wherein size is non-zero, but no dims -- 
  // set dims to 1, and dim[0] to the size
  if ((size != 0) && (geom.dims() == 0)) {
    geom.SetDims(1);
    geom.Set(0, size);
  }
  
  // get overall framesize and frames
  int dims_ = geom.dims(); // cache
  // make sure dims are valid (outer dim can be 0, others must be >0)
  int i;
  for (i = 0; i < (dims_ - 1) ; ++i) {
    if (geom[i] <= 0) {
      taMisc::Error(this->GetPathNames(), "geoms[0..N-2] must be > 0; object is now invalid");
      return;
    }
  }
  // assign storage (if not fixed) and set size
  if (isFixedData()) {
    size = frames() * frameSize();
  } else {
    EnforceFrames(geom[dims_-1]); // does nothing if outer dim==0
  }
}

void taMatrix::UpdateSlices_Collapse() {
  if (!slices) return;
  for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    mat->Slice_Collapse();
  }
}

void taMatrix::UpdateSlices_FramesDeleted(void* deletion_base, int num) {
  if (!slices) return;
  // get address of first (old) address beyond the deleted frames
  void* post_deletion = (char*)deletion_base + (num * frameSize() * El_SizeOf_());
  for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    void* mat_el = mat->data(); // cache
    if (!mat_el) continue; // collapsed
    if (mat_el < deletion_base)
      continue; // earlier than deleted frame, nothing to do
    else if ((mat_el >= deletion_base) && (mat_el < post_deletion))
      mat->Slice_Collapse(); // dude, you so don't exist anymore!
    else { // after the delete point, so fix up
      ta_intptr_t base_delta = (char*)deletion_base - (char*)post_deletion;
      mat->Slice_Realloc(base_delta);
    }
  }
}

void taMatrix::UpdateSlices_Realloc(ta_intptr_t base_delta) {
  if (!slices) return;
  for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    mat->Slice_Realloc(base_delta); // note, recursively calls realloc
  }
}
 
////////////////////////////////////////////////////
//   sub-matrix reading and writing functions

Variant taMatrix::RenderValue(const Variant& dest_val, const Variant& src_val, RenderOp render_op) {
  Variant rval;
  switch(render_op) {
  case COPY:
    rval = src_val;
    break;
  case ADD:
    rval = dest_val.toDouble() + src_val.toDouble();
    break;
  case SUB:
    rval = dest_val.toDouble() - src_val.toDouble();
    break;
  case MULT:
    rval = dest_val.toDouble() * src_val.toDouble();
    break;
  case DIV:
    rval = dest_val.toDouble() / src_val.toDouble();
    break;
  case MAX:
    rval = MAX(dest_val.toDouble(), src_val.toDouble());
    break;
  case MIN:
    rval = MIN(dest_val.toDouble(), src_val.toDouble());
    break;
  }
  return rval;
}

void taMatrix::WriteFmSubMatrix(const taMatrix* src, RenderOp render_op,
				int off0, int off1, int off2,
				int off3, int off4, int off5, int off6) {
  if(!src) return;
  MatrixIndex off(dims(), off0, off1, off2, off3, off4, off5, off6);
  MatrixIndex srcp;
  for(int i=0;i<src->size;i++) {
    src->geom.DimsFmIndex(i, srcp);
    MatrixIndex trgp(off);
    trgp += srcp;
    Variant sval = src->FastElAsVar_Flat(i);
    Variant dval = SafeElAsVarN(trgp);
    dval = RenderValue(dval, sval, render_op);
    SetFmVarN(dval, trgp);
  }
}

void taMatrix::ReadToSubMatrix(taMatrix* dest, RenderOp render_op, 
			       int off0, int off1, int off2,
			       int off3, int off4, int off5, int off6) {
  if(!dest) return;
  MatrixIndex off(dims(), off0, off1, off2, off3, off4, off5, off6);
  MatrixIndex srcp;
  for(int i=0;i<dest->size;i++) {
    dest->geom.DimsFmIndex(i, srcp);
    MatrixIndex trgp(off);
    trgp += srcp;
    Variant sval = SafeElAsVarN(trgp);
    if(!sval.isInvalid()) {
      Variant dval = dest->FastElAsVar_Flat(i);
      dval = RenderValue(dval, sval, render_op);
      dest->SetFmVar_Flat(dval, i);
    }
  }
}

void taMatrix::WriteFmSubMatrixFrames(taMatrix* src, RenderOp render_op,
				      int off0, int off1, int off2,
				      int off3, int off4, int off5, int off6) {
  if(!src) return;
  MatrixIndex off(dims(), off0, off1, off2, off3, off4, off5, off6);
  MatrixIndex srcp;
  int fr_max = frames();
  bool src_frames = false;	// source has frames
  if(src->dims() == dims()) {
    fr_max = MIN(fr_max, src->frames());
    src_frames = true;
  }
  for(int fr=0;fr<fr_max;fr++) {
    taMatrixPtr dfr;  dfr = GetFrameSlice_(fr);
    taMatrixPtr sfr;
    if(src_frames)
      sfr = src->GetFrameSlice_(fr);
    else
      sfr = src;
    for(int i=0;i<sfr->size;i++) {
      sfr->geom.DimsFmIndex(i, srcp);
      MatrixIndex trgp(off);
      trgp += srcp;
      Variant sval = sfr->FastElAsVar_Flat(i);
      Variant dval = dfr->SafeElAsVarN(trgp);
      dval = RenderValue(dval, sval, render_op);
      dfr->SetFmVarN(dval, trgp);
    }
  }
}

void taMatrix::ReadToSubMatrixFrames(taMatrix* dest, RenderOp render_op, 
				     int off0, int off1, int off2,
				     int off3, int off4, int off5, int off6) {
  if(!dest) return;
  dest->EnforceFrames(frames()); // match them up
  MatrixIndex off(dims(), off0, off1, off2, off3, off4, off5, off6);
  MatrixIndex srcp;
  int fr_max = frames();
  for(int fr=0;fr<fr_max;fr++) {
    taMatrixPtr sfr;  sfr = GetFrameSlice_(fr);
    taMatrixPtr dfr;  dfr = dest->GetFrameSlice_(fr);
    for(int i=0;i<dfr->size;i++) {
      dfr->geom.DimsFmIndex(i, srcp);
      MatrixIndex trgp(off);
      trgp += srcp;
      Variant sval = sfr->SafeElAsVarN(trgp);
      if(!sval.isInvalid()) {
	Variant dval = dfr->FastElAsVar_Flat(i);
	dval = RenderValue(dval, sval, render_op);
	dfr->SetFmVar_Flat(dval, i);
      }
    }
  }
}

/////////////////////////////////////////////////////////
//		Operators

//////////// op =
// taMatrix* taMatrix::operator=(const taMatrix& t) {
//   if(TestError(geom != t.geom, "=", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
//     return NULL;
//   if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
//     TA_FOREACH_INDEX(i, *this) {
//       ((float_Matrix*)this)->FastEl_Flat(i) = ((float_Matrix*)&t)->FastEl_Flat(i);
//     }
//   }
//   else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
//     TA_FOREACH_INDEX(i, *this) {
//       ((double_Matrix*)this)->FastEl_Flat(i) = ((double_Matrix*)&t)->FastEl_Flat(i);
//     }
//   }
//   else {			// use variants -- no need to optimize
//     TA_FOREACH_INDEX(i, *this) {
//       SetFmVar_Flat(t.FastElAsVar_Flat(i), i);
//     }
//   }
//   return this;
// }

taMatrix* taMatrix::operator=(const Variant& t) {
  if(t.isMatrixType()) {
    Copy(t.toMatrix());
    return this;
  }
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) = vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) = vt;
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(t, i);
    }
  }
  return this;
}

//////////// op +
taMatrix* taMatrix::operator+(const taMatrix& t) const {
  if(TestError(geom != t.geom, "+", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) + ((float_Matrix*)&t)->FastEl_Flat(i);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) + ((double_Matrix*)&t)->FastEl_Flat(i);
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) + t.FastElAsVar_Flat(i), i);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator+(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator+(t.toMatrix());
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) + vt;
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) + vt;
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) + t, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op -
taMatrix* taMatrix::operator-(const taMatrix& t) const {
  if(TestError(geom != t.geom, "-", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) - ((float_Matrix*)&t)->FastEl_Flat(i);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) - ((double_Matrix*)&t)->FastEl_Flat(i);
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) - t.FastElAsVar_Flat(i), i);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator-(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator-(t.toMatrix());
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) - vt;
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) - vt;
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) - t, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op - UNARY
taMatrix* taMatrix::operator-() const {
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = -((float_Matrix*)this)->FastEl_Flat(i);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = -((double_Matrix*)this)->FastEl_Flat(i);
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      Variant var = FastElAsVar_Flat(i);
      rval->SetFmVar_Flat(-var, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op *
taMatrix* taMatrix::operator*(const taMatrix& t) const {
  if(TestError(geom != t.geom, "*", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) * ((float_Matrix*)&t)->FastEl_Flat(i);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) * ((double_Matrix*)&t)->FastEl_Flat(i);
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) * t.FastElAsVar_Flat(i), i);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator*(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator*(t.toMatrix());
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) * vt;
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) * vt;
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) * t, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op /
taMatrix* taMatrix::operator/(const taMatrix& t) const {
  if(TestError(geom != t.geom, "/", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      float den = ((float_Matrix*)&t)->FastEl_Flat(i);
      if(!TestError(den == 0.0f, "/", "Floating Point Exception: Division by Zero"))
	rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) / den;
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      double den = ((double_Matrix*)&t)->FastEl_Flat(i);
      if(!TestError(den == 0.0, "/", "Floating Point Exception: Division by Zero"))
	rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) / den;
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) / t.FastElAsVar_Flat(i), i);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator/(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator/(t.toMatrix());
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    if(!TestError(vt == 0.0, "/", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
	rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) / vt;
      }
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    if(!TestError(vt == 0.0, "/", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
	rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) / vt;
      }
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) / t, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op %
taMatrix* taMatrix::operator%(const taMatrix& t) const {
  if(TestError(geom != t.geom, "%", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return NULL;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    if(TestError(true, "%", "not supported for floating-point types"))
      return NULL;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) % t.FastElAsVar_Flat(i), i);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator%(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator%(t.toMatrix());
  }
  if(GetDataValType() == VT_FLOAT) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return NULL;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return NULL;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) % t, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op ^
taMatrix* taMatrix::operator^(const taMatrix& t) const {
  if(TestError(geom != t.geom, "^", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = powf(((float_Matrix*)this)->FastEl_Flat(i),  ((float_Matrix*)&t)->FastEl_Flat(i));
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = pow(((double_Matrix*)this)->FastEl_Flat(i), ((double_Matrix*)&t)->FastEl_Flat(i));
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
     rval->SetFmVar_Flat(pow(FastElAsVar_Flat(i).toDouble(),
			     t.FastElAsVar_Flat(i).toDouble()), i);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator^(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator^(t.toMatrix());
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = powf(((float_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = pow(((double_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(pow(FastElAsVar_Flat(i).toDouble(), vt), i);
    }
    return rval;
  }
  return NULL;
}

//////////// op max
taMatrix* taMatrix::Max(const taMatrix& t) const {
  if(TestError(geom != t.geom, "Max", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MAX(((float_Matrix*)this)->FastEl_Flat(i),  ((float_Matrix*)&t)->FastEl_Flat(i));
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MAX(((double_Matrix*)this)->FastEl_Flat(i), ((double_Matrix*)&t)->FastEl_Flat(i));
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(MAX(FastElAsVar_Flat(i), t.FastElAsVar_Flat(i)), i);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::Max(const Variant& t) const {
  if(t.isMatrixType()) {
    return Max(t.toMatrix());
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MAX(((float_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MAX(((double_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(MAX(FastElAsVar_Flat(i), t), i);
    }
    return rval;
  }
  return NULL;
}

Variant taMatrix::Max() const {
  if(GetDataValType() == VT_FLOAT) {
    float rval = 0.0f;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
	rval = ((float_Matrix*)this)->FastEl_Flat(i);
	first = false;
      }
      else {
	rval = MAX(((float_Matrix*)this)->FastEl_Flat(i), rval);
      }
    }
    return Variant(rval);
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double rval = 0.0;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
	rval = ((double_Matrix*)this)->FastEl_Flat(i);
	first = false;
      }
      else {
	rval = MAX(((double_Matrix*)this)->FastEl_Flat(i), rval);
      }
    }
    return Variant(rval);
  }
  else {			// use variants -- no need to optimize
    Variant rval;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
	rval = FastElAsVar_Flat(i);
	first = false;
      }
      else {
	rval = MAX(FastElAsVar_Flat(i), rval);
      }
    }
    return rval;
  }
  return _nilVariant;
}

//////////// op min
taMatrix* taMatrix::Min(const taMatrix& t) const {
  if(TestError(geom != t.geom, "min", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MIN(((float_Matrix*)this)->FastEl_Flat(i),  ((float_Matrix*)&t)->FastEl_Flat(i));
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MIN(((double_Matrix*)this)->FastEl_Flat(i), ((double_Matrix*)&t)->FastEl_Flat(i));
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(MIN(FastElAsVar_Flat(i), t.FastElAsVar_Flat(i)), i);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::Min(const Variant& t) const {
  if(t.isMatrixType()) {
    return Min(t.toMatrix());
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MIN(((float_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MIN(((double_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(MIN(FastElAsVar_Flat(i), t), i);
    }
    return rval;
  }
  return NULL;
}

Variant taMatrix::Min() const {
  if(GetDataValType() == VT_FLOAT) {
    float rval = 0.0f;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
	rval = ((float_Matrix*)this)->FastEl_Flat(i);
	first = false;
      }
      else {
	rval = MIN(((float_Matrix*)this)->FastEl_Flat(i), rval);
      }
    }
    return Variant(rval);
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double rval = 0.0;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
	rval = ((double_Matrix*)this)->FastEl_Flat(i);
	first = false;
      }
      else {
	rval = MIN(((double_Matrix*)this)->FastEl_Flat(i), rval);
      }
    }
    return Variant(rval);
  }
  else {			// use variants -- no need to optimize
    Variant rval;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
	rval = FastElAsVar_Flat(i);
	first = false;
      }
      else {
	rval = MIN(FastElAsVar_Flat(i), rval);
      }
    }
    return rval;
  }
  return _nilVariant;
}

//////////// op abs
taMatrix* taMatrix::Abs() const {
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = fabsf(((float_Matrix*)this)->FastEl_Flat(i));
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = fabs(((double_Matrix*)this)->FastEl_Flat(i));
    }
    return rval;
  }
  else if(GetDataValType() == VT_INT) {
    int_Matrix* rval = new int_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ABS(((int_Matrix*)this)->FastEl_Flat(i));
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(ABS(FastElAsVar_Flat(i).toInt()), i);
    }
    return rval;
  }
  return NULL;
}

//////////// op +=
void taMatrix::operator+=(const taMatrix& t) {
  if(TestError(geom != t.geom, "+=", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) += ((float_Matrix*)&t)->FastEl_Flat(i);
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) += ((double_Matrix*)&t)->FastEl_Flat(i);
    }
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t.FastElAsVar_Flat(i), i);
    }
  }
}

void taMatrix::operator+=(const Variant& t) {
  if(t.isMatrixType()) {
    operator+=(t.toMatrix());
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) += vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) += vt;
    }
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t, i);
    }
  }
}

//////////// op -=
void taMatrix::operator-=(const taMatrix& t) {
  if(TestError(geom != t.geom, "-=", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) -= ((float_Matrix*)&t)->FastEl_Flat(i);
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) -= ((double_Matrix*)&t)->FastEl_Flat(i);
    }
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t.FastElAsVar_Flat(i), i);
    }
  }
}

void taMatrix::operator-=(const Variant& t) {
  if(t.isMatrixType()) {
    operator-=(t.toMatrix());
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) -= vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) -= vt;
    }
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t, i);
    }
  }
}

//////////// op *=
void taMatrix::operator*=(const taMatrix& t) {
  if(TestError(geom != t.geom, "*=", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) *= ((float_Matrix*)&t)->FastEl_Flat(i);
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) *= ((double_Matrix*)&t)->FastEl_Flat(i);
    }
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t.FastElAsVar_Flat(i), i);
    }
  }
}

void taMatrix::operator*=(const Variant& t) {
  if(t.isMatrixType()) {
    operator*=(t.toMatrix());
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) *= vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) *= vt;
    }
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t, i);
    }
  }
}

//////////// op /=
void taMatrix::operator/=(const taMatrix& t) {
  if(TestError(geom != t.geom, "/=", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) /= ((float_Matrix*)&t)->FastEl_Flat(i);
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) /= ((double_Matrix*)&t)->FastEl_Flat(i);
    }
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t.FastElAsVar_Flat(i), i);
    }
  }
}

void taMatrix::operator/=(const Variant& t) {
  if(t.isMatrixType()) {
    operator/=(t.toMatrix());
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) /= vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) /= vt;
    }
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t, i);
    }
  }
}

//////////// op %=
void taMatrix::operator%=(const taMatrix& t) {
  if(TestError(geom != t.geom, "%=", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t.FastElAsVar_Flat(i), i);
    }
  }
}

void taMatrix::operator%=(const Variant& t) {
  if(t.isMatrixType()) {
    operator%=(t.toMatrix());
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t, i);
    }
  }
}

//////////// op <
taMatrix* taMatrix::operator<(const taMatrix& t) const {
  if(TestError(geom != t.geom, "<", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((float_Matrix*)this)->FastEl_Flat(i) < ((float_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((double_Matrix*)this)->FastEl_Flat(i) < ((double_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) < t.FastElAsVar_Flat(i);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator<(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator<(t.toMatrix());
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) < vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) < vt;
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) < t;
    }
  }
  return rval;
}

//////////// op >
taMatrix* taMatrix::operator>(const taMatrix& t) const {
  if(TestError(geom != t.geom, ">", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((float_Matrix*)this)->FastEl_Flat(i) > ((float_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((double_Matrix*)this)->FastEl_Flat(i) > ((double_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) > t.FastElAsVar_Flat(i);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator>(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator>(t.toMatrix());
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) > vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) > vt;
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) > t;
    }
  }
  return rval;
}

//////////// op <=
taMatrix* taMatrix::operator<=(const taMatrix& t) const {
  if(TestError(geom != t.geom, "<=", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((float_Matrix*)this)->FastEl_Flat(i) <= ((float_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((double_Matrix*)this)->FastEl_Flat(i) <= ((double_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) <= t.FastElAsVar_Flat(i);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator<=(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator<=(t.toMatrix());
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) <= vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) <= vt;
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) <= t;
    }
  }
  return rval;
}

//////////// op >=
taMatrix* taMatrix::operator>=(const taMatrix& t) const {
  if(TestError(geom != t.geom, ">=", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((float_Matrix*)this)->FastEl_Flat(i) >= ((float_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((double_Matrix*)this)->FastEl_Flat(i) >= ((double_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) >= t.FastElAsVar_Flat(i);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator>=(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator>=(t.toMatrix());
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) >= vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) >= vt;
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) >= t;
    }
  }
  return rval;
}

//////////// op ==
taMatrix* taMatrix::operator==(const taMatrix& t) const {
  if(TestError(geom != t.geom, "==", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((float_Matrix*)this)->FastEl_Flat(i) == ((float_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((double_Matrix*)this)->FastEl_Flat(i) == ((double_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) == t.FastElAsVar_Flat(i);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator==(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator==(t.toMatrix());
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) == vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) == vt;
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) == t;
    }
  }
  return rval;
}

//////////// op !=
taMatrix* taMatrix::operator!=(const taMatrix& t) const {
  if(TestError(geom != t.geom, "!=", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((float_Matrix*)this)->FastEl_Flat(i) != ((float_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((double_Matrix*)this)->FastEl_Flat(i) != ((double_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) != t.FastElAsVar_Flat(i);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator!=(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator!=(t.toMatrix());
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) != vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) != vt;
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) != t;
    }
  }
  return rval;
}

//////////// op &&
taMatrix* taMatrix::operator&&(const taMatrix& t) const {
  if(TestError(geom != t.geom, "&&", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((byte_Matrix*)this)->FastEl_Flat(i) && ((byte_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else if(GetDataValType() == VT_INT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((int_Matrix*)this)->FastEl_Flat(i) && ((int_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i).toBool() && t.FastElAsVar_Flat(i).toBool();
    }
  }
  return rval;
}

taMatrix* taMatrix::operator&&(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator&&(t.toMatrix());
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE) {
    int vt = t.toInt();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((byte_Matrix*)this)->FastEl_Flat(i) && vt;
    }
  }
  else if(GetDataValType() == VT_INT) {
    int vt = t.toInt();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((int_Matrix*)this)->FastEl_Flat(i) && vt;
    }
  }
  else {			// use variants -- no need to optimize
    bool vt = t.toBool();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i).toBool() && vt;
    }
  }
  return rval;
}

//////////// op ||
taMatrix* taMatrix::operator||(const taMatrix& t) const {
  if(TestError(geom != t.geom, "||", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE || GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((byte_Matrix*)this)->FastEl_Flat(i) || ((byte_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else if(GetDataValType() == VT_INT || GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = (((int_Matrix*)this)->FastEl_Flat(i) || ((int_Matrix*)&t)->FastEl_Flat(i));
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i).toBool() || t.FastElAsVar_Flat(i).toBool();
    }
  }
  return rval;
}

taMatrix* taMatrix::operator||(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator||(t.toMatrix());
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE) {
    int vt = t.toInt();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((byte_Matrix*)this)->FastEl_Flat(i) || vt;
    }
  }
  else if(GetDataValType() == VT_INT) {
    int vt = t.toInt();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((int_Matrix*)this)->FastEl_Flat(i) || vt;
    }
  }
  else {			// use variants -- no need to optimize
    bool vt = t.toBool();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i).toBool() || vt;
    }
  }
  return rval;
}

taMatrix* taMatrix::operator!() const {
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = !((byte_Matrix*)this)->FastEl_Flat(i);
    }
  }
  else if(GetDataValType() == VT_INT) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = !((int_Matrix*)this)->FastEl_Flat(i);
    }
  }
  else {			// use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = !FastElAsVar_Flat(i).toBool();
    }
  }
  return rval;
}


//////////// op Flatten
taMatrix* taMatrix::Flatten() const {
  int ic = IterCount();
  int cnt = 0;
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(1, ic);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(cnt++) = ((float_Matrix*)this)->FastEl_Flat(i);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(1, ic);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(cnt++) = ((double_Matrix*)this)->FastEl_Flat(i);
    }
    return rval;
  }
  else if(GetDataValType() == VT_VARIANT) {
    // first go through and flatten any sub-matricies!
    taMatrix* tmp_mat = (taMatrix*)Clone(); // we store modified vals in here
    taBase::Ref(tmp_mat);
    int nc = 0;			// new count with flattend submatricies
    TA_FOREACH_INDEX(i, *this) {
      Variant var = tmp_mat->FastElAsVar_Flat(i);
      if(var.isMatrixType()) {
	taMatrix* smat = var.toMatrix();
	if(smat->ElView()) {	// it is a view
	  taMatrix* flmat = smat->Flatten(); // flatten it!
	  nc += flmat->size;
	  tmp_mat->SetFmVar_Flat((Variant)flmat, i); // replace
	}
	else {
	  nc += smat->size;	// keep
	}
      }
      else {
	nc++;			// just another item
      }
    }

    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeom(1, nc);
    TA_FOREACH_INDEX(i, *this) {
      Variant var = tmp_mat->FastElAsVar_Flat(i);
      if(var.isMatrixType()) {
	taMatrix* smat = var.toMatrix(); // we know it is already flat
	for(int j=0; j < smat->size; j++) {
	  Variant fitm = smat->FastElAsVar_Flat(j);
	  rval->SetFmVar_Flat(fitm, cnt++);
	}
      }
      else {
	rval->SetFmVar_Flat(var, cnt++);
      }
    }
    taBase::UnRef(tmp_mat);
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeom(1, ic);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i), cnt++);
    }
    return rval;
  }
  return NULL;
}

//////////// op Find
int_Matrix* taMatrix::Find() const {
  int n_nonz = 0;
  for(int i=0;i<size;i++) {
    if(FastElAsVar_Flat(i).toBool()) n_nonz++;
  }

  int dm = dims();
  MatrixIndex sidx;				   // slice idx
  int_Matrix* rval = new int_Matrix(2, dm, n_nonz);
  int cnt = 0;
  for(int i=0;i<size;i++) {
    if(!FastElAsVar_Flat(i).toBool()) continue;
    geom.DimsFmIndex(i, sidx);
    for(int d=0; d<dm; d++) {
      rval->FastEl(d,cnt) = sidx[d];
    }
    cnt++;
  }
  return rval;
}

taMatrix* taMatrix::Transpose() const {
  if(TestError(dims() != 2, "Transpose", "can only transpose a 2D matrix"))
    return NULL;
  int d0 = dim(0);
  int d1 = dim(1);
  MatrixGeom tg(2, d1, d0);
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(tg);
    for(int i=0;i<d0;i++) {
      for(int j=0;j<d1;j++) {
	rval->FastEl(j,i) = ((float_Matrix*)this)->FastEl(i,j);
      }
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(tg);
    for(int i=0;i<d0;i++) {
      for(int j=0;j<d1;j++) {
	rval->FastEl(j,i) = ((double_Matrix*)this)->FastEl(i,j);
      }
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(tg);
    for(int i=0;i<d0;i++) {
      for(int j=0;j<d1;j++) {
	rval->SetFmVar(SafeElAsVar(i,j), j,i);
      }
    }
    return rval;
  }
  return NULL;
}


//////////////////////////
//   String_Matrix	//
//////////////////////////

int String_Matrix::defAlignment() const {
  return Qt::AlignLeft;
}

int String_Matrix::Dump_Load_Item(istream& strm, int idx) {
  int c = taMisc::skip_till_start_quote_or_semi(strm);
  if (c == '\"') {
    c = taMisc::read_till_end_quote_semi(strm);
  }
  if (c != EOF) {
    FastEl_Flat(idx) = taMisc::LexBuf;
  }
  return c;
}

void String_Matrix::Dump_Save_Item(ostream& strm, int idx) {
// note: we don't write "" for empty
  taMisc::write_quoted_string(strm, FastEl_Flat(idx));
}

void String_Matrix::ReclaimOrphans_(int from, int to) {
  for (int i = from; i <= to; ++i) {
    el[i] = _nilString;
  }
}

String String_Matrix::ToDelimString(const String& delim) {
  String rval;
  for (int i = 0; i < size; ++i) {
    rval += FastEl_Flat(i);
    if(i < size-1) rval += delim;
  }
  return rval;
}

void String_Matrix::FmDelimString(const String& str, const String& delim) {
  Reset();
  String_Array ar;
  ar.FmDelimString(str, delim);
  if(dims() == 0) {
    SetGeom(1, ar.size);
  }
  else {
    if(size < ar.size) {
      int extra = ((ar.size - size) / frameSize());
      AddFrames(extra);
      while(size < ar.size)
	AddFrames(1);
    }
  }
  int idx = 0;
  String remainder = str;
  while(remainder.nonempty()) {
    if(remainder.contains(delim)) {
      Set_Flat(remainder.before(delim), idx++);
      remainder = remainder.after(delim);
    }
    else {
      Set_Flat(remainder, idx++);
      remainder = _nilString;
    }
  }
}

//////////////////////////
//   float_Matrix	//
//////////////////////////

void float_Matrix::Dump_Save_Item(ostream& strm, int idx) {
  strm << String(FastEl_Flat(idx), "%.8g");
}

bool float_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
#ifdef TA_USE_QT
  str.toFloat(&rval); //discard result
#endif
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid floating point number";
  return rval;
}


//////////////////////////
//   double_Matrix	//
//////////////////////////

void double_Matrix::Dump_Save_Item(ostream& strm, int idx) {
  strm << String(FastEl_Flat(idx), "%.16g");
}

bool double_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
#ifdef TA_USE_QT
  str.toDouble(&rval); //discard result
#endif
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid floating point number";
  return rval;
}

//////////////////////////
//   complex_Matrix	//
//////////////////////////

bool complex_Matrix::CheckComplexGeom(const MatrixGeom& gm, bool err) {
  if(gm.dims() < 2 || gm.dim(0) != 2) {
    if(!err) return false;
    taMisc::Error("CheckComplexGeom: geometry is not correct for representing complex numbers -- inner-most dimension must be size 2, and there must be 2 or more dimensions",
		  gm.PrintStr());
    return false;
  }
  return true;
}

MatrixGeom complex_Matrix::NonComplexGeom(const MatrixGeom& gm) {
  MatrixGeom ngm;
  if(!CheckComplexGeom(gm)) return ngm;
  ngm.SetDims(gm.dims() - 1); // reduce 1 dim
  for (int i = 1; i < gm.dims(); ++i) {
    ngm.Set(i-1, gm.dim(i));
  }
  return ngm;	 
}

MatrixGeom complex_Matrix::ComplexGeom(const MatrixGeom& gm) {
  MatrixGeom ngm;
  ngm.SetDims(gm.dims() + 1); // add 1 dim
  ngm.Set(0,2);		      // inner = 2
  for (int i = 0; i < gm.dims(); ++i) {
    ngm.Set(i+1, gm.dim(i));
  }
  return ngm;	 
}

double_Matrix* complex_Matrix::SqMag() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i< rval->size; ++i) {
    double r = FastEl_Flat(2*i);
    double j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = r*r + j*j;
  }
  return rval;
}

double_Matrix* complex_Matrix::Mag() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    double r = FastEl_Flat(2*i);
    double j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = sqrt(r*r + j*j);
  }
  return rval;
}

double_Matrix* complex_Matrix::Angle() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    double r = FastEl_Flat(2*i);
    double j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = atan2(j, r);
  }
  return rval;
}

double_Matrix* complex_Matrix::Real() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    double r = FastEl_Flat(2*i);
    rval->FastEl_Flat(i) = r;
  }
  return rval;
}

double_Matrix* complex_Matrix::Imag() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    double j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = j;
  }
  return rval;
}

complex_Matrix* complex_Matrix::Conj() const {
  if(!CheckComplexGeom(geom)) return NULL;
  complex_Matrix* rval = new complex_Matrix(this->geom);
  for(int i=0; i < size; i+=2) {
    double tr = FastEl_Flat(i);
    double tj = FastEl_Flat(i+1);
    rval->FastEl_Flat(i) = tr;
    rval->FastEl_Flat(i+1) = -tj;
  }
  return rval;
}

void complex_Matrix::SetReal(const double_Matrix& reals, bool copy_geom) {
  MatrixGeom cgm = ComplexGeom(reals.geom);
  if(copy_geom) {
    SetGeomN(cgm);
  }
  else {
    if(!CheckComplexGeom(geom)) return;
    if(TestError(cgm != geom, "SetReal", "input matrix does not have same geometry as 	destination matrix",reals.geom.PrintStr()))
      return;
  }
  for(int i=0; i < reals.size; ++i) {
    FastEl_Flat(2*i) = reals.FastEl_Flat(i);
  }
}

void complex_Matrix::SetImag(const double_Matrix& imags, bool copy_geom) {
  MatrixGeom cgm = ComplexGeom(imags.geom);
  if(copy_geom) {
    SetGeomN(cgm);
  }
  else {
    if(!CheckComplexGeom(geom)) return;
    if(TestError(cgm != geom, "SetImag", "input matrix does not have same geometry as 	destination matrix",imags.geom.PrintStr()))
      return;
  }
  for(int i=0; i < imags.size; ++i) {
    FastEl_Flat(2*i+1) = imags.FastEl_Flat(i);
  }
}

void complex_Matrix::Complex(const double_Matrix& reals, const double_Matrix& imags,
			     bool copy_geom) {
  if(TestError(reals.geom != imags.geom, "Complex", "input reals and imags matricies do not have same geometry.  reals:",reals.geom.PrintStr(), "imags:", imags.geom.PrintStr()))
    return;
  MatrixGeom cgm = ComplexGeom(reals.geom);
  if(copy_geom) {
    SetGeomN(cgm);
  }
  else {
    if(!CheckComplexGeom(geom)) return;
    if(TestError(cgm != geom, "Complex", "input matrices do not have same geometry as 	destination matrix",reals.geom.PrintStr()))
      return;
  }
  for(int i=0; i < reals.size; ++i) {
    FastEl_Flat(2*i) = reals.FastEl_Flat(i);
    FastEl_Flat(2*i+1) = imags.FastEl_Flat(i);
  }
}

void complex_Matrix::Expi(const double_Matrix& angles, bool copy_geom) {
  MatrixGeom cgm = ComplexGeom(angles.geom);
  if(copy_geom) {
    SetGeomN(cgm);
  }
  else {
    if(!CheckComplexGeom(geom)) return;
    if(TestError(cgm != geom, "Expi", "input matrix does not have same geometry as 	destination matrix",angles.geom.PrintStr()))
      return;
  }
  for(int i=0; i < angles.size; ++i) {
    FastEl_Flat(2*i) = cos(angles.FastEl_Flat(i));
    FastEl_Flat(2*i+1) = sin(angles.FastEl_Flat(i));
  }
}


void complex_Matrix::SetRealAll(double real) {
  if(!CheckComplexGeom(geom)) return;
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i) = real;
  }
}

void complex_Matrix::SetImagAll(double imag) {
  if(!CheckComplexGeom(geom)) return;
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i+1) = imag;
  }
}

void complex_Matrix::ComplexAll(double real, double imag) {
  if(!CheckComplexGeom(geom)) return;
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i) = real;
    FastEl_Flat(2*i+1) = imag;
  }
}

void complex_Matrix::ExpiAll(double angle) {
  if(!CheckComplexGeom(geom)) return;
  double cosv = cos(angle);
  double sinv = sin(angle);
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i) = cosv;
    FastEl_Flat(2*i+1) = sinv;
  }
}


//////////// op *
taMatrix* complex_Matrix::operator*(const taMatrix& t) const {
  if(TestError(geom != t.geom, "*", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(t.GetDataValType() == VT_DOUBLE) {
    complex_Matrix* rval = new complex_Matrix(this->geom);
    for(int i=0; i < size; i+=2) {
      double tr = FastEl_Flat(i);
      double tj = FastEl_Flat(i+1);
      double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
      double oj = ((double_Matrix*)&t)->FastEl_Flat(i+1);
      double nr = tr * orr - tj * oj;
      double nj = tr * oj + tj * orr;
      rval->FastEl_Flat(i) = nr;
      rval->FastEl_Flat(i+1) = nj;
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    complex_Matrix* rval = new complex_Matrix(this->geom);
    for(int i=0; i < size; i+=2) {
      double tr = FastEl_Flat(i);
      double tj = FastEl_Flat(i+1);
      double orr = t.FastElAsVar_Flat(i).toDouble();
      double oj = t.FastElAsVar_Flat(i+1).toDouble();
      double nr = tr * orr - tj * oj;
      double nj = tr * oj + tj * orr;
      rval->FastEl_Flat(i) = nr;
      rval->FastEl_Flat(i+1) = nj;
    }
    return rval;
  }
  return NULL;
}

//////////// op /
taMatrix* complex_Matrix::operator/(const taMatrix& t) const {
  if(TestError(geom != t.geom, "/", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return NULL;
  if(t.GetDataValType() == VT_DOUBLE) {
    complex_Matrix* rval = new complex_Matrix(this->geom);
    for(int i=0; i < size; i+=2) {
      double tr = FastEl_Flat(i);
      double tj = FastEl_Flat(i+1);
      double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
      double oj = ((double_Matrix*)&t)->FastEl_Flat(i+1);
      double n = orr*orr + oj*oj;
      double nr = 0.0f;
      double nj = 0.0f;
      if(!TestError(n == 0.0, "/", "Floating Point Exception: Division by Zero")) {
	nr = (tr * orr + tj * oj) / n;
	nj = (tj * orr - tr * oj) / n;
      }
      rval->FastEl_Flat(i) = nr;
      rval->FastEl_Flat(i+1) = nj;
    }
    return rval;
  }
  else {			// use variants -- no need to optimize
    complex_Matrix* rval = new complex_Matrix(this->geom);
    for(int i=0; i < size; i+=2) {
      double tr = FastEl_Flat(i);
      double tj = FastEl_Flat(i+1);
      double orr = t.FastElAsVar_Flat(i).toDouble();
      double oj = t.FastElAsVar_Flat(i+1).toDouble();
      double n = orr*orr + oj*oj;
      double nr = 0.0f;
      double nj = 0.0f;
      if(!TestError(n == 0.0, "/", "Floating Point Exception: Division by Zero")) {
	nr = (tr * orr + tj * oj) / n;
	nj = (tj * orr - tr * oj) / n;
      }
      rval->FastEl_Flat(i) = nr;
      rval->FastEl_Flat(i+1) = nj;
    }
    return rval;
  }
  return NULL;
}

//////////// op *=
void complex_Matrix::operator*=(const taMatrix& t) {
  if(TestError(geom != t.geom, "*", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return;
  if(t.GetDataValType() == VT_DOUBLE) {
    for(int i=0; i < size; i+=2) {
      double tr = FastEl_Flat(i);
      double tj = FastEl_Flat(i+1);
      double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
      double oj = ((double_Matrix*)&t)->FastEl_Flat(i+1);
      double nr = tr * orr - tj * oj;
      double nj = tr * oj + tj * orr;
      FastEl_Flat(i) = nr;
      FastEl_Flat(i+1) = nj;
    }
  }
  else {			// use variants -- no need to optimize
    for(int i=0; i < size; i+=2) {
      double tr = FastEl_Flat(i);
      double tj = FastEl_Flat(i+1);
      double orr = t.FastElAsVar_Flat(i).toDouble();
      double oj = t.FastElAsVar_Flat(i+1).toDouble();
      double nr = tr * orr - tj * oj;
      double nj = tr * oj + tj * orr;
      FastEl_Flat(i) = nr;
      FastEl_Flat(i+1) = nj;
    }
  }
}

//////////// op /
void complex_Matrix::operator/=(const taMatrix& t) {
  if(TestError(geom != t.geom, "/", "the geometry of the two matricies is not equal -- must be for element-wise operation"))
    return;
  if(t.GetDataValType() == VT_DOUBLE) {
    for(int i=0; i < size; i+=2) {
      double tr = FastEl_Flat(i);
      double tj = FastEl_Flat(i+1);
      double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
      double oj = ((double_Matrix*)&t)->FastEl_Flat(i+1);
      double n = orr*orr + oj*oj;
      double nr = 0.0f;
      double nj = 0.0f;
      if(!TestError(n == 0.0, "/", "Floating Point Exception: Division by Zero")) {
	nr = (tr * orr + tj * oj) / n;
	nj = (tj * orr - tr * oj) / n;
      }
      FastEl_Flat(i) = nr;
      FastEl_Flat(i+1) = nj;
    }
  }
  else {			// use variants -- no need to optimize
    for(int i=0; i < size; i+=2) {
      double tr = FastEl_Flat(i);
      double tj = FastEl_Flat(i+1);
      double orr = t.FastElAsVar_Flat(i).toDouble();
      double oj = t.FastElAsVar_Flat(i+1).toDouble();
      double n = orr*orr + oj*oj;
      double nr = 0.0f;
      double nj = 0.0f;
      if(!TestError(n == 0.0, "/", "Floating Point Exception: Division by Zero")) {
	nr = (tr * orr + tj * oj) / n;
	nj = (tj * orr - tr * oj) / n;
      }
      FastEl_Flat(i) = nr;
      FastEl_Flat(i+1) = nj;
    }
  }
}

taMatrix* complex_Matrix::Transpose() const {
  if(!CheckComplexGeom(geom)) return NULL;
  if(TestError(dims() != 3, "Transpose", "can only transpose a 2D matrix"))
    return NULL;
  int d0 = dim(1);
  int d1 = dim(2);
  complex_Matrix* rval = new complex_Matrix(3,2,d1,d0);
  for(int i=0;i<d0;i++) {
    for(int j=0;j<d1;j++) {
      rval->FastEl(0,j,i) = this->FastEl(0,i,j);
      rval->FastEl(1,j,i) = this->FastEl(1,i,j);
    }
  }
  return rval;
}

//////////////////////////
//   int_Matrix		//
//////////////////////////

bool int_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
#ifdef TA_USE_QT
  str.toInt(&rval, 0); //auto-base sensing; discard result
#endif
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid integer number";
  return rval;
}


//////////////////////////
//   byte_Matrix	//
//////////////////////////

bool byte_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
  ushort val = 0;
#ifdef TA_USE_QT
  val = str.toUShort(&rval, 0); //auto-base sensing
#endif
  // check for overflow
  if (rval && (val > 255)) rval = false;
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid byte value (0-255)";
  return rval;
}

//////////////////////////
//   voidptr_Matrix	//
//////////////////////////

bool voidptr_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
#ifdef TA_USE_QT
  str.toInt(&rval, 0); //auto-base sensing; discard result
#endif
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid integer number";
  return rval;
}

//////////////////////////
//   Variant_Matrix	//
//////////////////////////

int Variant_Matrix::Dump_Load_Item(istream& strm, int idx) {
  int c;
  Variant& val = FastEl_Flat(idx);
  if (!val.Dump_Load_Type(strm, c)) goto end;
  if (c == EOF) return c;
  
  switch (val.type()) {
  case Variant::T_Invalid: // no content
    c = taMisc::skip_white(strm); // should read ;
    break; 
  case Variant::T_Bool:
  case Variant::T_Int:
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
  case Variant::T_Double:
    c = taMisc::read_till_semi(strm);
    val.updateFromString(taMisc::LexBuf);
    break;
  case Variant::T_Char: {// reads chars as ints
    c = taMisc::read_till_semi(strm);
    int sc = taMisc::LexBuf.toInt();
    val.setChar((char)sc, val.isNull());
    }
    break;
  
  case Variant::T_String: 
    //note: an empty string was not written
    c = taMisc::skip_till_start_quote_or_semi(strm);
    if (c == '\"') {
      c = taMisc::read_till_end_quote_semi(strm);
      val.updateFromString(taMisc::LexBuf);
    }
    break;
//  case Variant::T_Ptr:  //not streamable
//TODO: maybe we should issue a warning???
//    break;
  
  case Variant::T_Base: 
  case Variant::T_Matrix:
    //note: an empty string was not written
    c = taMisc::skip_till_start_quote_or_semi(strm);
    if (c == '\"') {
      c = taMisc::read_till_end_quote_semi(strm);
      val.updateFromString(taMisc::LexBuf);
    }
    break;
  default: 
    c = taMisc::read_till_semi(strm);
    break;
  }
  
end:
  return c;
}

void Variant_Matrix::Dump_Save_Item(ostream& strm, int idx) {
  const Variant& val = FastEl_Flat(idx);
  val.Dump_Save_Type(strm);
  switch (val.type()) {
  case Variant::T_Invalid: break; // no content
  case Variant::T_Bool:
  case Variant::T_Int:
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
    strm << ' ' << val.toString();
    break;
  case Variant::T_Double:
    strm << ' ' << String(val.toDouble(), "%.16lg");
    break;
  case Variant::T_Char: // write chars as ints
    strm << ' ' << val.toInt();
    break;
  
  case Variant::T_String: 
    strm << ' ';
    //note: doesn't write empty strings
    taMisc::write_quoted_string(strm, val.toString());
    break;
  case Variant::T_Ptr:  //not streamable
//TODO: maybe we should issue a warning???
    break;
  
  case Variant::T_Base: 
  case Variant::T_Matrix:
    strm << ' ';
    //note: doesn't write empty strings
    taMisc::write_quoted_string(strm, val.toString());
    break;
  
  default: 
    break;
  }
}

void Variant_Matrix::ReclaimOrphans_(int from, int to) {
  for (int i = from; i <= to; ++i) {
    el[i] = _nilVariant;
  }
}


//////////////////////////
//   rgb_Matrix	//
//////////////////////////

bool rgb_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
  ushort val = 0;
#ifdef TA_USE_QT
  val = str.toUShort(&rval, 0); //auto-base sensing
#endif
  // check for overflow
  if (rval && (val > 255)) rval = false;
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid byte value (0-255)";
  return rval;
}

const Variant Variant_Matrix::blank;
const String String_Matrix::blank;
const double double_Matrix::blank = 0.0;
const float float_Matrix::blank = 0.0f;
const int int_Matrix::blank = 0;
const unsigned char byte_Matrix::blank = '\0';
const void* voidptr_Matrix::blank = 0;
const rgb_t rgb_Matrix::blank;


//////////////////////////
//  	CircMatrix	//
//////////////////////////

void CircMatrix::Initialize() {
  st_idx = 0;
  length = 0;
}

void CircMatrix::Reset() {
  st_idx = 0;
  length = 0;
}


