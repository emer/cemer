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

#include "MatrixIndex.h"
#include <int_Matrix>
#include <taMisc>

using namespace std;

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
  if (c==EOF)   return EOF;
  SetDims(cnt);                 // just to be double sure it is same as loaded size
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
