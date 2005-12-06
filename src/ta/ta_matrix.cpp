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
 
int taMatrix_impl::FastElIndex2(int d1, int d0) const {
  Assert((geom.size >= 2), "too many indices for matrix");
  Assert(((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (d1 * geom[0]) + d0;
  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::FastElIndex3(int d2, int d1, int d0) const {
  Assert((geom.size >= 3), "too many indices for matrix");
  Assert(((d2 >= 0) && (d2 < geom[2])) && ((d1 >= 0) && (d1 < geom[1])) 
    && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (((d2 * geom[1]) + d1) * geom[0]) + d0;
  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::FastElIndex4(int d3, int d2, int d1, int d0) const {
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

int taMatrix_impl::SafeElIndex(int d0) const {
  Check((geom.size >= 1), "matrix geometry has not been initialized");
  Check(((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  Check((d0 < size), "matrix element is out of bounds");
  return d0;
}
 
int taMatrix_impl::SafeElIndex2(int d1, int d0) const {
  Check((geom.size >= 2), "too many indices for matrix");
  Check(((d1 >= 0) && (d1 < geom[1])) && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (d1 * geom[0]) + d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::SafeElIndex3(int d2, int d1, int d0) const {
  Check((geom.size >= 3), "too many indices for matrix");
  Check(((d2 >= 0) && (d2 < geom[2])) && ((d1 >= 0) && (d1 < geom[1])) 
    && ((d0 >= 0) && (d0 < geom[0])), 
    "matrix index out of bounds");
  
  int rval = (((d2 * geom[1]) + d1) * geom[0]) + d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
int taMatrix_impl::SafeElIndex4(int d3, int d2, int d1, int d0) const {
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
  Check((dims_ > 0), "dims_ must be > 0");
  
  int i;
  for (i = 0; i < (dims_ - 1) ; ++i) {
    Check((geom_[i] > 0), "geoms[0..N-2] must be > 0");
  }
  
  // only copy bottom N-1 dims, setting 0 frames -- we size frames in next step
  geom.EnforceSize(dims_);
  for (i = 0; i < (dims_ - 1) ; ++i) {
    geom[i] = geom_[i];
  }

  // assign storage if not fixed
  if (isFixedData()) {
    geom[dims_-1] = geom_[dims_-1];
  }else {
    geom[dims_-1] = 0;
    EnforceFrames(geom_[dims_-1]); // does nothing if outer dim==0
  }
}

void taMatrix_impl::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  UpdateGeom();
}

void taMatrix_impl::UpdateGeom() {
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


