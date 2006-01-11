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


/////////////////////////
//  taMatrix_impl      //
/////////////////////////

bool taMatrix_impl::GeomIsValid(int dims_, const int geom_[], String* err_msg) {
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

String taMatrix_impl::GeomToString(const int_Array& geom) {
  String rval("[");
  for (int i = 0; i < geom.size; ++i) {
    if (i > 0) rval += ',';
    rval += i;
  }
  rval += "]";
  return rval;
}


void taMatrix_impl::Initialize()
{
  size = 0;
  alloc_size = 0;
}
 
void taMatrix_impl::Destroy() {
  size = 0;
  alloc_size = 0;
  CutLinks();
}
  
void taMatrix_impl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(geom, this);
}

void taMatrix_impl::CutLinks() {
  inherited::CutLinks();
  geom.CutLinks();
}
  
void taMatrix_impl::Add_(const void* it) {
  Check(canResize(), "resizing not allowed");
  Check((dims() == 1), "Add() only allowed when dims=1");
  int idx = frames();
  EnforceFrames(idx + 1);
  El_Copy_(FastEl_(idx), it);
}

void taMatrix_impl::AddFrames(int n) {
  Check(canResize(), "resizing not allowed");
  
  EnforceFrames(n + frames());
}

void taMatrix_impl::Alloc_(int new_alloc) {
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

void taMatrix_impl::AllocFrames(int n) {
  Alloc_(n * frameSize());
}

bool taMatrix_impl::canResize() const {
  return ((alloc_size >= 0));
}


void taMatrix_impl::Copy_(const taMatrix_impl& cp) {
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

void taMatrix_impl::List(ostream& strm) const {
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

ostream& taMatrix_impl::Output(ostream& strm, int indent) const {
  taMisc::indent(strm, indent);
  List(strm);
  strm << ";\n";
  return strm;
}

int taMatrix_impl::Dump_Save_Value(ostream& strm, TAPtr, int indent) {
  geom.Dump_Save_Value(strm, this, indent); 
  strm << "{ ";
  int i;
  for (i=0; i < size; ++i) {
    strm << El_GetStr_(FastEl_(i)) << ";";
  }
  return true;
}

int taMatrix_impl::Dump_Load_Value(istream& strm, TAPtr) {
  geom.Dump_Load_Value(strm, this); 
  UpdateGeom(); // also allocates space
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
  while ((c == ';') && (c != EOF)) {
    if (cnt > size)  {
      taMisc::Error("Too many items encountered for Matrix:",GetTypeDef()->name,"\n");
      return false;
    }
    El_SetFmStr_(FastEl_(cnt++), taMisc::LexBuf);
    c = taMisc::read_till_rb_or_semi(strm);
  }
  if (c==EOF)	return EOF;
  return true;
}

void taMatrix_impl::EnforceFrames(int n) {
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
  geom[geom.size-1] = n;	
}

int taMatrix_impl::FastElIndex(int d0) const {
  Assert((geom.size >= 1), "matrix geometry has not been initialized");
  Assert(((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  Assert((d0 < size), "matrix element is out of bounds");
  return d0;
}
 
int taMatrix_impl::FastElIndex2(int d0, int d1) const {
  Assert((geom.size >= 2), "too many indices for matrix");
  Assert(((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (d1 * geom[0]) + d0;
  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::FastElIndex3(int d0, int d1, int d2) const {
  Assert((geom.size >= 3), "too many indices for matrix");
  Assert(((d2 >= 0) && (d2 < geom[2])) && ((d1 >= 0) && (d1 < geom[1])) 
    && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (((d2 * geom[1]) + d1) * geom[0]) + d0;
  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::FastElIndex4(int d0, int d1, int d2, int d3) const {
  Assert((geom.size >= 4), "too many indices for matrix");
  Assert(((d3 >= 0) && (d3 < geom[3])) && ((d2 >= 0) && (d2 < geom[2])) 
    && ((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = ( ( (((d3 * geom[2]) + d2) * geom[1]) + d1) * geom[0]) + d0;
  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::FastElIndexN(const int_Array& indices) const {
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
 
int taMatrix_impl::frames() const {
  if (geom.size == 0) return 0;
  return geom[geom.size-1];
}

int taMatrix_impl::frameSize() const {
  if (geom.size == 0) return 0;
  if (geom.size == 1) return 1;
  int rval = geom[0];
  for (int i = 1; i < (geom.size - 1); ++i) 
    rval *= geom[i];
  return rval;
}

bool taMatrix_impl::InRange(int d0) const {
  return (geom.size >= 1)
    && ((d0 >= 0) && (d0 < geom[0]))
    ;
}
 
bool taMatrix_impl::InRange2(int d0, int d1) const {
  return (geom.size >= 2)
    && ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    ;
}
 
bool taMatrix_impl::InRange3(int d0, int d1, int d2) const {
  return (geom.size >= 3)
    && ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]))
    ;
}
 
bool taMatrix_impl::InRange4(int d0, int d1, int d2, int d3) const {
  return (geom.size >= 4)
    && ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]))
    && ((d3 >= 0) && (d3 < geom[3]))
    ;
}
 
bool taMatrix_impl::InRangeN(const int_Array& indices) const {
  if (indices.size < geom.size) return false;
  for (int i = 0; i < indices.size; ++i) {
    int di = indices[i];
    if ((di < 0) || (di >= geom[i])) return false;
  }
  return true;
}
 
void taMatrix_impl::RemoveFrame(int n) {
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

int taMatrix_impl::SafeElIndex(int d0) const {
  Check((geom.size >= 1), "matrix geometry has not been initialized");
  Check(((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  Check((d0 < size), "matrix element is out of bounds");
  return d0;
}
 
int taMatrix_impl::SafeElIndex2(int d0, int d1) const {
  Check((geom.size >= 2), "too many indices for matrix");
  Check(((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (d1 * geom[0]) + d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::SafeElIndex3(int d0, int d1, int d2) const {
  Check((geom.size >= 3), "too many indices for matrix");
  Check(((d2 >= 0) && (d2 < geom[2])) && ((d1 >= 0) && (d1 < geom[1])) 
    && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (((d2 * geom[1]) + d1) * geom[0]) + d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::SafeElIndex4(int d0, int d1, int d2, int d3) const {
  Check((geom.size >= 4), "too many indices for matrix");
  Check(((d3 >= 0) && (d3 < geom[3])) && ((d2 >= 0) && (d2 < geom[2])) 
    && ((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = ( ( (((d3 * geom[2]) + d2) * geom[1]) + d1) * geom[0]) + d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::SafeElIndexN(const int_Array& indices) const {
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
 
void taMatrix_impl::SetFixedData_(void* el_, const int_Array& geom_) {
  // first, clear out any old data
  SetArray_(el_);
  alloc_size = -1; // flag for fixed data
  geom.Reset();
  SetGeomN(geom_);
}

void taMatrix_impl::SetGeom_(int dims_, const int geom_[]) {
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

void taMatrix_impl::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  UpdateGeom();
}

void taMatrix_impl::UpdateGeom() {
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

