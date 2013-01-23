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

#ifndef taMatrixT_h
#define taMatrixT_h 1

// parent includes:
#include <taMatrix>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(taMatrixT);

template<class T>
class TA_API taMatrixT : public taMatrix {
  // #VIRT_BASE #NO_INSTANCE ##CAT_Data
INHERITED(taMatrix)
public:
  T*            el;             // #HIDDEN #NO_SAVE Pointer to actual array memory

  override void*        data() const {return el;} // #IGNORE

  void                  SetFixedData(T* data_, const MatrixGeom& geom_,
    fixed_dealloc_fun fixed_dealloc = NULL)
  { SetFixedData_(data_, geom_); }
  // #IGNORE sets external (fixed) data, setting the geom/size

  ////////////////////////////////////////////////
  //    functions that return the type          //
  ////////////////////////////////////////////////

  const T&              FastEl(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const
  { return el[FastElIndex(d0, d1, d2, d3, d4, d5, d6)]; }
  const T&              FastElN(const MatrixIndex& indices) const
  { return el[FastElIndexN(indices)]; }
  const T&              FastEl_Flat(int idx) const { return el[idx]; }

  T&                    FastEl(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0)
  { return el[FastElIndex(d0, d1, d2, d3, d4, d5, d6)]; }
  // #CAT_Access get element without range checking
  T&                    FastElN(const MatrixIndex& indices)
  { return el[FastElIndexN(indices)]; }
  // #CAT_Access get element without range checking
  T&                    FastEl_Flat(int idx) { return el[idx]; }
  // #CAT_Access get element without range checking -- treats matrix like a flat array

  const T&              SafeEl(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const
  { return *((T*)(SafeEl_(SafeElIndex(d0, d1, d2, d3, d4, d5, d6)))); }
  // #CAT_Access (safely) access the element for reading
  const T&              SafeElN(const MatrixIndex& indices) const
  { return *((T*)(SafeEl_(SafeElIndexN(indices)))); }
  // #CAT_Access (safely) access the element for reading
  const T&              SafeEl_Flat(int idx) const
  { return *((T*)(SafeEl_(idx))); }
  // #CAT_Access (safely) access the matrix as if it were a flat vector, for reading

  void                  Set(const T& item, int d0, int d1=0, int d2=0,
    int d3=0, int d4=0, int d5=0, int d6=0)
  { int idx = SafeElIndex(d0, d1, d2, d3, d4, d5, d6);
    if (InRange_Flat(idx)) el[idx] = item; }
  // #CAT_Modify safely assign values to items in the matrix
  void                  SetN(const T& item, const MatrixIndex& indices)
  {  int idx = SafeElIndexN(indices);
    if (InRange_Flat(idx)) el[idx] = item; }
  // #CAT_Modify safely assign values to items in the matrix
  void                  Set_Flat(const T& item, int idx)
  { if (InRange_Flat(idx)) el[idx] = item; }
  // #CAT_Modify safely assign values to items in the matrix, treated as a flat vector

  // compatibility functions, for when dims=1
  void                  Add(const T& item) {Add_(&item);}
  // #CAT_Modify only valid when dims=1

  USING(taMatrix::operator=)
  TA_TMPLT_ABSTRACT_BASEFUNS(taMatrixT, T)
public:
  override void*        FastEl_Flat_(int idx)   { return &(el[idx]); }
  override const void*  FastEl_Flat_(int idx) const { return &(el[idx]); }
protected:
  override void*        MakeArray_(int n) const {
    if (fastAlloc())
      return malloc(n * sizeof(T));
    else
      return new T[n];
  }
  override void         SetArray_(void* nw) {
    if (el) {
      if (alloc_size > 0) {
        if(fastAlloc())
          free(el);
        else delete [] el;
      }
      else if (alloc_size < 0) {
        if (fixed_dealloc)
          fixed_dealloc(el);
      }
    }
    el = (T*)nw;
    fixed_dealloc = NULL;
  }
  override void*        FastRealloc_(int n) {
    T* nwel = (T*)realloc((char*)el, n * sizeof(T));
    if (TestError(!nwel, "FastRealloc_", "could not realloc memory -- matrix is too big! reverting to old size -- could be fatal!"))
      return NULL;
    el = nwel;
    return el;
  }
  override bool         El_Equal_(const void* a, const void* b) const {
    return (*((T*)a) == *((T*)b));
  }
  override void         El_Copy_(void* to, const void* fm) {
    *((T*)to) = *((T*)fm);
  }
  override uint         El_SizeOf_() const {
    return sizeof(T);
  }

private:
  TMPLT_NOCOPY(taMatrixT, T)
  void                  Initialize()    {el = NULL;}
  void                  Destroy() { }
};

#define TA_MATRIX_FUNS(y,T) \
  y* GetSlice(const MatrixIndex& base, int sfd = -1, int nsf = 1) \
   {return (y*)GetSlice_(base, sfd, nsf);} \
  y* GetFrameSlice(int frame) {return (y*) GetFrameSlice_(frame);} \
  y* GetFrameRangeSlice(int frame, int n_frames) {return (y*) GetFrameRangeSlice_(frame, n_frames);} \
  y(int dims_,int d0,int d1=0,int d2=0,int d3=0,int d4=0,int d5=0,int d6=0) \
    {SetGeom(dims_, d0,d1,d2,d3,d4,d5,d6);} \
  explicit y(const MatrixGeom& geom_) {SetGeomN(geom_);} \
  y(T* data_, const MatrixGeom& geom_) {SetFixedData(data_, geom_);} \
  void CutLinks() { SetArray_(NULL); taMatrix::CutLinks(); } \
  USING(taMatrix::operator=) \
  TA_BASEFUNS(y) \
protected: \
  override const void*  El_GetBlank_() const    { return (const void*)&blank; }

#define MAT_COPY_SAME_SLOW(y,T) \
  void  Copy_(const y& cp) { if(ElView() || cp.ElView()) { \
      Copy_Matrix_impl(&cp); return; } \
    SetGeomN(cp.geom); \
    for (int i = 0; i < size; ++i) { \
      El_Copy_(FastEl_Flat_(i), cp.FastEl_Flat_(i)); \
    }}

#define MAT_COPY_SAME_FAST(y,T) \
  void  Copy_(const y& cp) {  if(ElView() || cp.ElView()) { \
      Copy_Matrix_impl(&cp); return; } \
    SetGeomN(cp.geom);				 \
    memcpy(data(), cp.data(), size * sizeof(T)); \
    }

#define TA_MATRIX_FUNS_FAST(y,T) \
private: \
  MAT_COPY_SAME_FAST(y,T) \
protected: \
  override bool fastAlloc() const {return true;} \
public: \
  TA_MATRIX_FUNS(y,T)

#define TA_MATRIX_FUNS_SLOW(y,T) \
private: \
  MAT_COPY_SAME_SLOW(y,T) \
protected: \
  override bool fastAlloc() const {return false;} \
public: \
  TA_MATRIX_FUNS(y,T)

// use this for a derived class of an instantiated matrix type
#define TA_MATRIX_FUNS_DERIVED(y,T) \
  y(int dims_,int d0,int d1=0,int d2=0,int d3=0,int d4=0,int d5=0,int d6=0) \
    {SetGeom(dims_, d0,d1,d2,d3,d4,d5,d6);} \
  explicit y(const MatrixGeom& geom_) {SetGeomN(geom_);} \
  y(T* data_, const MatrixGeom& geom_) {SetFixedData(data_, geom_);} \
  USING(taMatrix::operator=) \
  TA_BASEFUNS_NOCOPY(y)


#endif // taMatrixT_h
