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

#include "MatrixGeom.h"

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

int MatrixGeom::IndexFmDimsN(const MatrixIndex& dims) const {
  return IndexFmDims_(dims.el);
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
  int dx = idx;                 // local register
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
                              int mat_view) const
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
  if (c==EOF)   return EOF;
  SetDims(cnt);                 // just to be double sure it is same as loaded size
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
