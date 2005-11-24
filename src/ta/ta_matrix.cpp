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

taMatrix_impl::taMatrix_impl()
{
  alloc_size = 0;
  m_ref = 0;
  m_slice_geom = NULL;
  m_slice_offset = NULL;
  m_data_owner = NULL; 
}
 
taMatrix_impl::~taMatrix_impl() {
//TODO: should print warning in debug mode if ref!=0
  if (m_data_owner != NULL) {
    m_data_owner->Unref(); // note: creating a view does a ref on dataowner
    m_data_owner = NULL;
  }
  if (m_slice_geom != NULL) {
    delete m_slice_geom;
    m_slice_geom = NULL;
  }
  if (m_slice_offset != NULL) {
    delete m_slice_offset;
    m_slice_offset = NULL;
  }
}
  
void taMatrix_impl::Alloc_(int new_alloc) {
  assert((alloc_size >= 0) && "ERROR: attempt to realloc a fixed data matrix");
  
//NOTE: this is a low level allocator; alloc is typically managed in frames
  if (alloc_size < new_alloc)	{
    char* nw = (char*)MakeArray_(new_alloc);
    int size_ = size();
    for (int i = 0; i < size_; ++i) {
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
  return ((alloc_size >= 0) && (m_ref <= 1) && (m_data_owner == NULL));
}

/*TBD
void taMatrix_impl::Copy_(const taMatrix_impl& cp) {
  setGeom(cp.m_geom);
} */

int taMatrix_impl::ElIndex(const int_FixedArray& indices) const {
  assert((indices.size == m_geom.size) && "indices do not match");
  
  int rval = 0;
  for (int i = indices.size - 1 ; i > 0; --i) {
    rval += indices[i];
    rval *= m_geom[i-1];
  }
  rval += indices[0];
  return rval;
}
 
int taMatrix_impl::ElIndex(int d1, int d0) const {
  int rval = (d1 * m_geom[0]) + d0;
  return rval;
}
 
int taMatrix_impl::ElIndex(int d2, int d1, int d0) const {
  int rval = (((d2 * m_geom[1]) + d1) * m_geom[0]) + d0;
  return rval;
}
 
int taMatrix_impl::ElIndex(int d3, int d2, int d1, int d0) const {
  int rval = ( ( (((d3 * m_geom[2]) + d2) * m_geom[1]) + d1) * m_geom[0]) + d0;
  return rval;
}
 
void taMatrix_impl::EnforceFrames(int n) {
  assert(canResize() && "resizing not allowed");
  
  int frames_ = frames(); // cache
  if (n == frames_) return;
  else if (n > frames_) {
    AllocFrames(n);
    const void* blank = El_GetBlank_();
    int upto = frameSize() * n;
    for (int i = frameSize() * frames_; i < upto; ++i) {
      El_Copy_(FastEl_(i), blank);
    }
  } else if (n < frames_) {
    ReclaimOrphans_(frameSize() * n, (frameSize() * frames_) - 1);
  }
  m_geom[m_geom.size-1] = n;	
}

int taMatrix_impl::frames() const {
  if (m_geom.size == 0) return 0;
  return m_geom[m_geom.size-1];
}

int taMatrix_impl::frameSize() const {
  if (m_geom.size == 0) return 0;
  if (m_geom.size == 1) return 1;
  int rval = m_geom[0];
  for (int i = 1; i < (m_geom.size - 1); ++i) 
    rval *= m_geom[i];
  return rval;
}

void taMatrix_impl::InitView_(taMatrix_impl& data_owner_) {
  data_owner_.Ref();
  m_data_owner = &data_owner_;
  SetArray_(data_owner_.data_());
  alloc_size = data_owner_.alloc_size;
  m_geom = data_owner_.m_geom;
  // note: view defaults to no slicing -- may be overridden
}

const void* taMatrix_impl::SafeEl_(int i) const {
  if (InRange(i)) return ((taMatrix_impl*)this)->FastEl_(i); 
  else            return El_GetErr_();
}

void taMatrix_impl::setGeom_(int dims_, const int geom_[]) {
  assert((m_geom.size == 0) && "geom has already been set");
  assert((dims_ > 0) && "dims_ must be > 0");
  
  int i;
  for (i = 0; i < (dims_ - 1) ; ++i) {
    assert((geom_[i] > 0) && "geoms[0..N-2] must be > 0");
  }
  
  // only copy bottom N-1 dims, setting 0 frames -- we size frames in next step
  m_geom.EnforceSize(dims_);
  for (i = 0; i < (dims_ - 1) ; ++i) {
    m_geom[i] = geom_[i];
  }
  m_geom[dims_-1] = 0;

  // assign storage if not fixed and not a view
  if (!isFixedData() && !isView()) {
    EnforceFrames(geom_[dims_-1]); // does nothing if outer dim==0
  }
}

int taMatrix_impl::size() const {
  if (m_geom.size == 0) return 0;
  int rval = m_geom[0];
  for (int i = 1; i < m_geom.size; ++i) 
    rval *= m_geom[i];
  return rval;
}


/////////////////////////
//  MatrixPtr_impl       //
/////////////////////////

MatrixPtr_impl::MatrixPtr_impl() 
: m_ptr(0)
{}

MatrixPtr_impl::~MatrixPtr_impl() {
  set(NULL);
}

void MatrixPtr_impl::set(taMatrix_impl* src) {
  // note: we implicitly handle us===src
  if (src != NULL) src->Ref();
  if (m_ptr != NULL) m_ptr->Unref();
  m_ptr = src;
}


const byte byte_Matrix::blank = 0;


const float float_Matrix::blank = 0.0f;
