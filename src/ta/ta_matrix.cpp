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
:m_ref(0)
{
}
 
taMatrix_impl::~taMatrix_impl() {
//TODO: should print warning in debug mode if ref!=0
}
  
void taMatrix_impl::Copy_(const taMatrix_impl& cp) {
  setGeom(cp.m_geom);
  inherited::Copy_(cp);
}

int taMatrix_impl::ElIndex(const int indices[]) const {
  int rval = 0;
  for (int i = 0 ; i < m_geom.size; ++i)
    rval += indices[i] * m_dmx[i];
  return rval;
}
 
int taMatrix_impl::ElIndex(int i, int j) const {
  int rval = i * m_dmx[0];
  rval += j;
  return rval;
}
 
int taMatrix_impl::ElIndex(int i, int j, int k) const {
  int rval = i * m_dmx[0];
  rval += j * m_dmx[1];
  rval += k;
  return rval;
}
 
int taMatrix_impl::ElIndex(int i, int j, int k, int l) const {
  int rval = i * m_dmx[0];
  rval += j * m_dmx[1];
  rval += k * m_dmx[2];
  rval += l;
  return rval;
}
 
bool taMatrix_impl::Equal_(const taFixedArray_impl& src) const {
  const taMatrix_impl* msrc = dynamic_cast<const taMatrix_impl*>(&src);
  if (msrc == NULL) return false;
  else return Equal_(*msrc);
}

bool taMatrix_impl::Equal_(const taMatrix_impl& src) const {
  return ((m_geom == src.m_geom) && inherited::Equal_(src));
}

void taMatrix_impl::setGeom_(int dims_, const int geom_[]) {
  int i;
  assert((dims_ > 0) && "dims_ must be > 0");
  for (i = 1; i < dims_ ; ++i) {
    assert((geom_[i] > 0) && "geoms[>0] must be > 0");
  }
  
  if (m_geom.size <= 0) return; 
  
  m_geom.EnforceSize(dims_);
  for (i = 1; i < dims_ ; ++i) {
    m_geom[i] = geom_[i];
  }

  // recalculate dimension multipliers
  m_dmx[dims_ - 1] = 1;
  for (i = dims_ - 2; i >= 0; --i) {
    m_dmx[i] = m_geom[i + 1] * m_dmx[i + 1];
  }
  
  // set storage size, which will be 0 if geom[0]==0
  EnforceSize(m_geom[0] * m_dmx[0]);
}

/*
taMatrix_impl::taMatrix_impl(int dims_, int geom_[]) 
:m_geom(0)
{
  assert(((dims_ >= 1) && (dims_ <= MAX_MATRIX_DIMS)) &&
    "dims must be >=1 and <= MAX_MATRIX_DIMS");
  m_dims = dims_; 
  m_geom = new int[dims_]; 
  for (int i = 0; i <= dims_; ++i) g_geom[i] = geom_[i];
  m_refs = 0;
  size = 0;
  alloc_size = 0;
} */
/*
taMatrix_impl::~taMatrix_impl() {
  delete[] m_geom;
#ifdef DEBUG 
  m_dims = 0; 
  size = 0;
  alloc_size = 0;
#endif
}
*/


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

