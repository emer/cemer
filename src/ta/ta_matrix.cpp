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


#include "ta_matrix.h"


//////////////////////////
//  MatrixGeom		//
//////////////////////////

MatrixGeom::MatrixGeom(int init_size) {
  Initialize();
  EnforceSize(init_size);
}
  
void MatrixGeom::Initialize() {
  size = 0;
}

void MatrixGeom::Destroy() {
#ifdef DEBUG
  // helps detect multi-delete errors
  for (int i = size - 1; i >= 0; el[i--] = 0); 
  size = 0; 
#endif
}

void MatrixGeom::Copy_(const MatrixGeom& cp) {
  EnforceSize(cp.size);
  for (int i = 0; i < size; ++i) {
    el[i] = cp.el[i];
  }
} 

void MatrixGeom::Add(int value) {
  if (size >= TA_MATRIX_DIMS_MAX) return;
  el[size++] = value;
}

int MatrixGeom::Dump_Save_Value(ostream& strm, TAPtr, int) {
  strm << "{ ";
  int i;
  for (i=0; i < size; i++) {
    strm << FastEl(i) << ";";
  }
  return true;
}

int MatrixGeom::Dump_Load_Value(istream& strm, TAPtr) {
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
    if (cnt > size-1)
      Add(val);
    else Set(cnt, val);
    ++cnt;
    c = taMisc::read_till_rb_or_semi(strm);
  }
  if (c==EOF)	return EOF;
  return true;
}


void MatrixGeom::EnforceSize(int new_sz) {
  if ((new_sz < 0) || (new_sz >= TA_MATRIX_DIMS_MAX)) return;
  // zero out new elements
  for (int i = size; i < new_sz; ++i)
    el[i] = 0;
  size = new_sz;
}

bool MatrixGeom::Equal(const MatrixGeom& other) const {
  if (size != other.size) return false;
  for (int i = 0; i < size; ++i) {
    if (el[i] != other.el[i]) return false;
  }
  return true;
} 

int MatrixGeom::Product() const {
  if (size == 0) return 0;
  int rval = el[0];
  for (int i = 1; i < size; ++i)
    rval *= el[i];
  return rval;
}



//////////////////////////
//  taMatrix		//
//////////////////////////

bool taMatrix::GeomIsValid(int dims_, const int geom_[], String* err_msg) {
  if (dims_ <= 0) { 
    if (err_msg != NULL)
      *err_msg = "dims must be > 0";
    return false;
  }
  
  int i;
  for (i = 0; i < (dims_ - 1) ; ++i) {
    if (geom_[i] <= 0) {
      if (err_msg != NULL)
        *err_msg = "geoms[0..N-2] must be > 0";
      return false;
    }
  }
  
  return true;
}

String taMatrix::GeomToString(const MatrixGeom& geom) {
  String rval("[");
  for (int i = 0; i < geom.size; ++i) {
    if (i > 0) rval += ',';
    rval += i;
  }
  rval += "]";
  return rval;
}


void taMatrix::Initialize()
{
  size = 0;
  alloc_size = 0;
}
 
void taMatrix::Destroy() {
  size = 0;
  alloc_size = 0;
  CutLinks();
}
  
void taMatrix::Add_(const void* it) {
  Check(canResize(), "resizing not allowed");
  Check((dims() == 1), "Add() only allowed when dims=1");
  int idx = frames();
  EnforceFrames(idx + 1);
  El_Copy_(FastEl_(idx), it);
}

void taMatrix::AddFrames(int n) {
  Check(canResize(), "resizing not allowed");
  
  EnforceFrames(n + frames());
}

void taMatrix::Alloc_(int new_alloc) {
  Check((alloc_size >= 0), "cannot alloc a fixed data matrix");
  
//NOTE: this is a low level allocator; alloc is typically managed in frames
  if (alloc_size < new_alloc)	{
    char* nw = (char*)MakeArray_(new_alloc);
    for (int i = 0; i < size; ++i) {
      El_Copy_(nw + (El_SizeOf_() * i), FastEl_(i));
    }
    SetArray_(nw);
    alloc_size = new_alloc;
  }
}

void taMatrix::AllocFrames(int n) {
  Alloc_(n * frameSize());
}

bool taMatrix::canResize() const {
  return ((alloc_size >= 0));
}


void taMatrix::Copy_(const taMatrix& cp) {
  // note: we must inherit from the copy
  Check(GetTypeDef()->InheritsFrom(cp.GetTypeDef()), "cannot copy " +
    String(GetTypeDef()->name) + " from " + String(cp.GetTypeDef()->name));
    
  // first, zero out current, if any
  SetArray_(NULL);
  if (cp.isFixedData()) {
    SetFixedData_(cp.data(), cp.geom); //sets size
  } else {
    alloc_size = 0;
    size = 0;
    geom.Reset();
    SetGeomN(cp.geom);
    SetArray_(const_cast<void*>(cp.data()));
    for (int i = 0; i < size; ++i) {
      El_Copy_(FastEl_(i), cp.FastEl_(i));
    }
  }
}

void taMatrix::List(ostream& strm) const {
  strm << "["; 
  for (int d = 0; d < dims(); ++d) {
    if (d > 0) strm << ",";
    strm << GetGeom(d);
  }
  strm << "] {";
  int i;
  for(i=0;i<size;i++) {
    strm << " " << El_GetStr_(FastEl_(i)) << ",";
  }
  strm << "}";
}

ostream& taMatrix::Output(ostream& strm, int indent) const {
  taMisc::indent(strm, indent);
  List(strm);
  strm << ";\n";
  return strm;
}

int taMatrix::Dump_Save_Value(ostream& strm, TAPtr par, int indent) {
  strm << "{ ";
  int i;
  if (geom.size > 0) {
    strm << "[";
    for (i=0; i< geom.size; ++i) {
      if (i > 0) strm << " ";
      strm << geom.FastEl(i);
    }
    strm << "] ";
  }
  for (i=0; i < size; ++i) {
    strm << FastElAsStr_Flat(i) << ";";
  }
  return true;
}

int taMatrix::Dump_Load_Value(istream& strm, TAPtr par) {
  int c = taMisc::skip_white(strm);
  if (c == EOF)    return EOF;
  if (c == ';') // just a path
    return 2;  // signal that just a path was loaded..

  
  if (c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  // we expect, but don't require, the [..] dims 
  MatrixGeom ar; // temp, while streaming
  c = taMisc::skip_white(strm, true); // use peek mode, until we're sure
  if (c == '[') {
    strm.get(); // actually gets the [
    do {
      c = taMisc::read_word(strm); // also consumes next char, whether sp or ]
      ar.Add(taMisc::LexBuf.toInt());
    } while ((c != ']') && (c != EOF));
  }
  //note: should always be at least one dim if we had [ but we check anyway
  if (ar.size > 0)
    SetGeomN(ar);
  c = taMisc::read_till_rb_or_semi(strm);
  int idx = 0;
  while ((c == ';') && (c != EOF)) {
    if (idx >= size)  {
      taMisc::Error("Too many items in file for type:",GetTypeDef()->name,"\n");
      return false;
    }
    SetFmStr_Flat(idx++, taMisc::LexBuf);
    c = taMisc::read_till_rb_or_semi(strm);
  }
  if (c==EOF)	return EOF;
  return true;
}

void taMatrix::EnforceFrames(int n) {
  Check(canResize(), "resizing not allowed");
  
  // note: we enforce the size in terms of underlying cells, for when
  // dimensions are changed (even though that is frowned on...)
  int new_size = n * frameSize();
  Alloc_(new_size); // makes sure we have enough raw space
  // blank new or reclaim old
  if (new_size > size) {
    const void* blank = El_GetBlank_();
    for (int i = size; i < new_size; ++i) {
      El_Copy_(FastEl_(i), blank);
    }
  } else if (new_size < size) {
    ReclaimOrphans_(new_size, size - 1);
  }
  size = new_size;
  geom.Set(geom.size-1, n);	
}

int taMatrix::FastElIndex(int d0) const {
  Assert((geom.size >= 1), "matrix geometry has not been initialized");
  Assert(((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  Assert((d0 < size), "matrix element is out of bounds");
  return d0;
}
 
int taMatrix::FastElIndex2(int d0, int d1) const {
  Assert((geom.size >= 2), "too many indices for matrix");
  Assert(((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (d1 * geom[0]) + d0;
  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix::FastElIndex3(int d0, int d1, int d2) const {
  Assert((geom.size >= 3), "too many indices for matrix");
  Assert(((d2 >= 0) && (d2 < geom[2])) && ((d1 >= 0) && (d1 < geom[1])) 
    && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (((d2 * geom[1]) + d1) * geom[0]) + d0;
  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix::FastElIndex4(int d0, int d1, int d2, int d3) const {
  Assert((geom.size >= 4), "too many indices for matrix");
  Assert(((d3 >= 0) && (d3 < geom[3])) && ((d2 >= 0) && (d2 < geom[2])) 
    && ((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = ( ( (((d3 * geom[2]) + d2) * geom[1]) + d1) * geom[0]) + d0;
  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix::FastElIndexN(const MatrixGeom& indices) const {
  Assert((geom.size >= 1), "matrix geometry has not been initialized");
  Assert((indices.size >= 1), "at least 1 index must be specified");
  Assert((indices.size <= geom.size), "too many indices for matrix");
  int d0 = indices[0];
  Assert(((d0 >= 0) && (d0 < geom[0])), "matrix index out of bounds");
  
  int rval = 0;
  for (int i = indices.size - 1 ; i > 0; --i) {
    int di = indices[i];
    Assert(((di >= 0) && (di < geom[i])), "matrix index out of bounds");
    rval += di;
    rval *= geom[i-1];
  }
  rval += d0;
  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix::frames() const {
  if (geom.size == 0) return 0;
  return geom[geom.size-1];
}

int taMatrix::frameSize() const {
  if (geom.size == 0) return 0;
  if (geom.size == 1) return 1;
  int rval = geom[0];
  for (int i = 1; i < (geom.size - 1); ++i) 
    rval *= geom[i];
  return rval;
}

bool taMatrix::InRange(int d0) const {
  return (geom.size >= 1)
    && ((d0 >= 0) && (d0 < geom[0]))
    ;
}
 
bool taMatrix::InRange2(int d0, int d1) const {
  return (geom.size >= 2)
    && ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    ;
}
 
bool taMatrix::InRange3(int d0, int d1, int d2) const {
  return (geom.size >= 3)
    && ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]))
    ;
}
 
bool taMatrix::InRange4(int d0, int d1, int d2, int d3) const {
  return (geom.size >= 4)
    && ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]))
    && ((d3 >= 0) && (d3 < geom[3]))
    ;
}
 
bool taMatrix::InRangeN(const MatrixGeom& indices) const {
  if (indices.size < geom.size) return false;
  for (int i = 0; i < indices.size; ++i) {
    int di = indices[i];
    if ((di < 0) || (di >= geom[i])) return false;
  }
  return true;
}
 
void taMatrix::RemoveFrame(int n) {
  int frames_ = frames(); // cache
  Check(((n >= 0) && (n < frames_)), "frame number out of bounds");
  // check if we have to copy data
  if (n != (frames_ - 1)) {
    int fm = n * frameSize();
    int to = fm - frameSize();
    while (fm < size) {
      El_Copy_(FastEl_(to), FastEl_(fm));
      ++fm;
      ++to;
    }
  }
  EnforceFrames(frames_ - 1); // this properly resizes, and reclaims orphans
}

int taMatrix::SafeElIndex(int d0) const {
  Check((geom.size >= 1), "matrix geometry has not been initialized");
  Check(((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  Check((d0 < size), "matrix element is out of bounds");
  return d0;
}
 
int taMatrix::SafeElIndex2(int d0, int d1) const {
  Check((geom.size >= 2), "too many indices for matrix");
  Check(((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (d1 * geom[0]) + d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix::SafeElIndex3(int d0, int d1, int d2) const {
  Check((geom.size >= 3), "too many indices for matrix");
  Check(((d2 >= 0) && (d2 < geom[2])) && ((d1 >= 0) && (d1 < geom[1])) 
    && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (((d2 * geom[1]) + d1) * geom[0]) + d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix::SafeElIndex4(int d0, int d1, int d2, int d3) const {
  Check((geom.size >= 4), "too many indices for matrix");
  Check(((d3 >= 0) && (d3 < geom[3])) && ((d2 >= 0) && (d2 < geom[2])) 
    && ((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = ( ( (((d3 * geom[2]) + d2) * geom[1]) + d1) * geom[0]) + d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix::SafeElIndexN(const MatrixGeom& indices) const {
  Check((geom.size >= 1), "matrix geometry has not been initialized");
  Check((indices.size >= 1), "at least 1 index must be specified");
  Check((indices.size <= geom.size), "too many indices for matrix");
  int d0 = indices[0];
  Check(((d0 >= 0) && (d0 < geom[0])), "matrix index out of bounds");
  
  int rval = 0;
  for (int i = indices.size - 1 ; i > 0; --i) {
    int di = indices[i];
    Assert(((di >= 0) && (di < geom[i])), "matrix index out of bounds");
    rval += di;
    rval *= geom[i-1];
  }
  rval += d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
void taMatrix::SetFixedData_(void* el_, const MatrixGeom& geom_) {
  // first, clear out any old data
  SetArray_(el_);
  alloc_size = -1; // flag for fixed data
  geom.Reset();
  SetGeomN(geom_);
}

void taMatrix::SetGeom_(int dims_, const int geom_[]) {
  String err_msg;
  bool valid = GeomIsValid(dims_, geom_, &err_msg);
  Check(valid, err_msg);
  
  //NOTE: following routine is conservative of existing geom, and will ignore flex sizing if already sized
  // only copy bottom N-1 dims, setting 0 frames -- we size frames in next step
  geom.EnforceSize(dims_);
  for (int i = 0; i < (dims_ - 1) ; ++i) {
    geom[i] = geom_[i];
  }

  // assign storage if not fixed
  if (isFixedData()) {
    geom[dims_-1] = geom_[dims_-1];
  } else {
    // if flex case, we skip this step -- it will stay zero on new, or retain value if data exists
    if (geom_[dims_-1] != 0) {
      // exact value case
      geom[dims_-1] = 0; // next step actually sets
      EnforceFrames(geom_[dims_-1]); // does nothing if outer dim==0
    }
  }
}

void taMatrix::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
//  UpdateGeom();
}

void taMatrix::UpdateGeom() {
  // handle legacy/graceful case wherein size is non-zero, but no dims -- 
  // set dims to 1, and dim[0] to the size
  if ((size != 0) && (geom.size == 0)) {
    geom.EnforceSize(1);
    geom.FastEl(0) = size;
  }
  
  // get overall framesize and frames
  int dims_ = geom.size; // cache
  // make sure dims are valid (outer dim can be 0, others must be >0)
  int i;
  for (i = 0; i < (dims_ - 1) ; ++i) {
    if (geom[i] <= 0) {
      taMisc::Warning(this->GetPath(), "geoms[0..N-2] must be > 0; object is now invalid");
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

//////////////////////////
//   String_Matrix	//
//////////////////////////

void String_Matrix::ReclaimOrphans_(int from, int to) {
  for (int i = from; i <= to; ++i) {
    el[i] = _nilString;
  }
}

//////////////////////////
//   float_Matrix	//
//////////////////////////

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
//   int_Matrix	//
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
//   Variant_Matrix	//
//////////////////////////

void Variant_Matrix::ReclaimOrphans_(int from, int to) {
  for (int i = from; i <= to; ++i) {
    el[i] = _nilVariant;
  }
}

