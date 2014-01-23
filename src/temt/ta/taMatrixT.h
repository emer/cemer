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

taTypeDef_Of(taMatrixT);

template<class T>
class TA_API taMatrixT : public taMatrix {
  // #VIRT_BASE #NO_INSTANCE ##CAT_Data
INHERITED(taMatrix)
public:
  T*            el;             // #HIDDEN #NO_SAVE Pointer to actual array memory

  void*        data() const CPP11_OVERRIDE {return el;} // #IGNORE

  void                  SetFixedData(T* data_, const MatrixGeom& geom_,
    fixed_dealloc_fun fixed_dealloc = NULL)
  { SetFixedData_(data_, geom_); }
  // #IGNORE sets external (fixed) data, setting the geom/size

  ////////////////////////////////////////////////
  //    functions that return the type          //
  ////////////////////////////////////////////////

  const T&              FastEl(int d0, int d1, int d2, int d3,
    int d4, int d5=0, int d6=0) const
  { return el[FastElIndex(d0, d1, d2, d3, d4, d5, d6)]; }
  const T&              FastEl1d(int d0) const
  { return el[d0]; }
  const T&              FastEl2d(int d0, int d1) const
  { return el[FastElIndex2d(d0, d1)]; }
  const T&              FastEl3d(int d0, int d1, int d2) const
  { return el[FastElIndex3d(d0, d1, d2)]; }
  const T&              FastEl4d(int d0, int d1, int d2, int d3) const
  { return el[FastElIndex3d(d0, d1, d2)]; }
  const T&              FastElN(const MatrixIndex& indices) const
  { return el[FastElIndexN(indices)]; }
  const T&              FastEl_Flat(int idx) const { return el[idx]; }

  T&                    FastEl(int d0, int d1, int d2, int d3,
                               int d4, int d5=0, int d6=0)
  { return el[FastElIndex(d0, d1, d2, d3, d4, d5, d6)]; }
  // #CAT_Access get element without range checking -- only for > 4d matrices -- for 4d and lower, use specific dimensionality required
  T&                    FastEl1d(int d0)
  { return el[d0]; }
  // #CAT_Access get element without range checking, 1d optimized -- use if you know you've got a 1d matrix
  T&                    FastEl2d(int d0, int d1)
  { return el[FastElIndex2d(d0, d1)]; }
  // #CAT_Access get element without range checking, 2d optimized -- use if you know you've got a 2d matrix
  T&                    FastEl3d(int d0, int d1, int d2)
  { return el[FastElIndex3d(d0, d1, d2)]; }
  // #CAT_Access get element without range checking, 3d optimized -- use if you know you've got a 3d matrix
  T&                    FastEl4d(int d0, int d1, int d2, int d3)
  { return el[FastElIndex4d(d0, d1, d2, d3)]; }
  // #CAT_Access get element without range checking, 4d optimized -- use if you know you've got a 4d matrix
  T&                    FastElN(const MatrixIndex& indices)
  { return el[FastElIndexN(indices)]; }
  // #CAT_Access get element without range checking
  T&                    FastEl_Flat(int idx) { return el[idx]; }
  // #CAT_Access get element without range checking -- treats matrix like a flat array

  T&            operator[](int i) { return el[i]; }
  const T&      operator[](int i) const { return el[i]; }

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

  using taMatrix::operator=;
  TA_TMPLT_ABSTRACT_BASEFUNS(taMatrixT, T)
public:
  void*        FastEl_Flat_(int idx)   CPP11_OVERRIDE { return &(el[idx]); }
  const void*  FastEl_Flat_(int idx) const CPP11_OVERRIDE { return &(el[idx]); }
protected:
  mutable T             tmp; // #IGNORE temporary item

  void*        MakeArray_(int n) const CPP11_OVERRIDE {
    if (fastAlloc())
      return malloc(n * sizeof(T));
    else
      return new T[n];
  }
  void         SetArray_(void* nw)  CPP11_OVERRIDE {
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
  void*        FastRealloc_(int n) CPP11_OVERRIDE {
    T* nwel = (T*)realloc((char*)el, n * sizeof(T));
    if (TestError(!nwel, "FastRealloc_", "could not realloc memory -- matrix is too big! reverting to old size -- could be fatal!"))
      return NULL;
    el = nwel;
    return el;
  }
  bool         El_Equal_(const void* a, const void* b) const CPP11_OVERRIDE {
    return (*((T*)a) == *((T*)b));
  }
  void         El_Copy_(void* to, const void* fm) CPP11_OVERRIDE {
    *((T*)to) = *((T*)fm);
  }
  uint         El_SizeOf_() const CPP11_OVERRIDE {
    return sizeof(T);
  }
  void*        El_GetTmp_() const CPP11_OVERRIDE
  { return (void*)&tmp; }

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
  using taMatrix::operator=; \
  TA_BASEFUNS(y) \
protected: \
  const void*  El_GetBlank_() const    CPP11_OVERRIDE { return (const void*)&blank; }

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
  virtual bool fastAlloc() const {return true;} \
public: \
  TA_MATRIX_FUNS(y,T)

#define TA_MATRIX_FUNS_SLOW(y,T) \
private: \
  MAT_COPY_SAME_SLOW(y,T) \
protected: \
  virtual bool fastAlloc() const {return false;} \
public: \
  TA_MATRIX_FUNS(y,T)

// use this for a derived class of an instantiated matrix type
#define TA_MATRIX_FUNS_DERIVED(y,T) \
  y(int dims_,int d0,int d1=0,int d2=0,int d3=0,int d4=0,int d5=0,int d6=0) \
    {SetGeom(dims_, d0,d1,d2,d3,d4,d5,d6);} \
  explicit y(const MatrixGeom& geom_) {SetGeomN(geom_);} \
  y(T* data_, const MatrixGeom& geom_) {SetFixedData(data_, geom_);} \
  using taMatrix::operator=; \
  TA_BASEFUNS_NOCOPY(y)


#endif // taMatrixT_h
