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


// ta_matrix.h -- implementation header for matrix data (used by data system)

#ifndef TA_MATRIX_H
#define TA_MATRIX_H

#include <assert.h>
#include "ta_stdef.h"

#include "ta_base.h"
#include "ta_TA_type.h"

// externals
class TypeDef;

// forwards this file
class byte_Matrix;
class float_Matrix;

/* Matrix -- a specialized, richer implementation of Array

   Matrix is a ref-counted N-dimensional array of data, 1 <= N <= MAX_MATRIX_DIMS.
   
   Each concrete class holds one specific type of data, ex. byte or float. 
   The number of dimensions is set at create time, and is lifetime-invariant.
   The number of elements in each dimension (except the highest) is fixed.
   Data is stored in row-major order, ex:
     [2][3]: 00, 01, 02, 10, 11, 12
   The value for the highest dimension (geom[n-1]) is special:
     if geom[N-1]=0 then no allocation is made, and the data will resize
     if geom[N-1]!=0, then data is allocated at creation -- note
       that the allocated data is uninitialized
   Space is allocated in full units of the highest dimension, called a *Frame*;
     for example, for a 2-d matrix, each frame is on row (for 1-d, a frame
     is the same as a cell.)
   
   The object supports partially filled arrays, but not ragged arrays.
   Strongly-typed instances support external fixed data, via setFixedData() call.

   NOTE: Matrix in its current form is not streamable.
   
   Matrix vs. Array
   'Array' classes are typically 1-d vectors or interpreted as 2-d arrays.
   Array supports dynamic operations, like inserting, sorting, etc.
   Matrix is ref-counted, and intended for sharing/moving raw data around.
   Matrix explicitly supports dimensionality and dimensional access.
   
*/


class taMatrix_impl: public taBase { // #VIRT_BASE #NO_INSTANCE ##NO_TOKENS ref counted multi-dimensional data array
INHERITED(taBase)
public:
  int 			size;	// #SHOW #READ_ONLY number of elements in the matrix (= frames*frameSize)
  int_Array		geom; // #SHOW #READ_ONLY dimensions array
  
  bool			canResize() const; // true only if not fixed NOTE: may also include additional constraints, tbd
  int			dims() const {return geom.size;}
  int 			frames() const;	// number of frames currently in use (value of highest dimension) 
  int 			frameSize() const;	// number of elements in each frame (product of inner dimensions) 
  
  bool			isFixedData() const {return alloc_size < 0;} // true if using fixed (externally managed) data storage
  
  
  void			AddFrames(int n); // add n new blank frames
  void			AllocFrames(int n); // make sure space exists for n frames
  void			EnforceFrames(int n); // set size to n frames, blanking new elements if added
  
  void			SetGeom(int d0)  
    {int d[1]; d[0]=d0; SetGeom_(1, d);} // set geom for 1-d array
  void			SetGeom2(int d1, int d0)  
    {int d[2]; d[0]=d0; d[1]=d1; SetGeom_(2, d);} // set geom for 12-d array
  void			SetGeom3(int d2, int d1, int d0)  
    {int d[3]; d[0]=d0; d[1]=d1; d[2]=d2; SetGeom_(3, d);} // set geom for 3-d array
  void			SetGeom4(int d3, int d2, int d1, int d0)  
    {int d[4]; d[0]=d0; d[1]=d1; d[2]=d2; d[3]=d3; SetGeom_(4, d);} // set geom for 4-d array
  void			SetGeomN(const int_Array& geom_) 
    {SetGeom_(geom_.size, geom_.el);} // set geom for any sized array
  
  void			InitLinks();
  void			CutLinks();
  void			UpdateAfterEdit(); // esp important to call after changing geom -- note that geom gets fixed if invalid
  void			Copy_(const taMatrix_impl& cp);
  COPY_FUNS(taMatrix_impl, taBase);
  TA_ABSTRACT_BASEFUNS(taMatrix_impl) //
public: // don't use these, internal use only
  virtual void*		data() const = 0;  // #IGNORE
  virtual void*		FastEl_(int i) = 0;   // #IGNORE
  virtual const void*	FastEl_(int i) const = 0;   // #IGNORE
 
protected:
  int			alloc_size; // -1 means fixed (external data)
  
  virtual void		SetGeom_(int dims_, const int geom_[]);
  
  int			ElIndex2(int d1, int d0) const; 
  int			ElIndex3(int d2, int d1, int d0) const; 
  int			ElIndex4(int d3, int d2, int d1, int d0) const; 
  int			ElIndexN(const int_Array& indices) const; 
  
  virtual void		Alloc_(int new_alloc); // set capacity to n -- should always be in multiples of frames 
  virtual void*		MakeArray_(int i) const = 0; // #IGNORE make a new array of item type; raise exception on failure
  virtual void		SetArray_(void* nw) = 0;
  virtual void		ReclaimOrphans_(int from, int to) {} // called when elements can be reclaimed, ex. for strings
  
  virtual bool		El_Equal_(const void*, const void*) const = 0;
  // #IGNORE for finding
  virtual const void*	El_GetBlank_() const = 0;
  // #IGNORE address of a blank element, for initializing empty items -- can be STATIC_CONST
  virtual const void*	El_GetErr_() const	{ return El_GetBlank_();}
  // #IGNORE address of an element to return when out of range -- defaults to blank el
  virtual void		El_Copy_(void*, const void*) = 0;
  // #IGNORE
  virtual uint		El_SizeOf_() const = 0;
  // #IGNORE size of element
  
  inline bool		InRange(int idx) const {return ((idx >= 0) && (idx < size));}
    //NOTE: for efficiency, this only checks if in allocated range, not in actual range
  
  virtual void		SetFixedData_(void* el_, const int_Array& geom_); // initialize fixed data
//  virtual bool		Equal_(const taMatrix_impl& src) const; 
    // 'true' if same size and els
  virtual void		UpdateGeom(); // called to potentially update the allocation based on new geom info -- will fix if in error
private:
  void 			Initialize();
  void			Destroy();
};


template<class T> 
class taMatrix : public taMatrix_impl { // #VIRT_BASE #NO_INSTANCE 
public:
  T*		el;		// #HIDDEN #NO_SAVE Pointer to actual array memory

  override void*	data() const {return el;}
  
  void			SetFixedData(T* data_, const int_Array& geom_) {SetFixedData_(data_, geom_);} 
  // sets external (fixed) data, setting the geom/size
  
  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  T&			FastEl(int d0) 	{ return el[d0]; }
  T&			FastEl2(int d1, int d0) { return el[ElIndex2(d1,d0)]; }
  T&			FastEl3(int d2, int d1, int d0) { return el[ElIndex3(d2,d1,d0)]; }
  T&			FastEl4(int d3, int d2, int d1, int d0) { return el[ElIndex4(d3,d2,d1,d0)]; } //
  T&			FastElN(const int_Array& indices) {return el[ElIndexN(indices)]; }
  
  const T&		FastEl(int d0) const	{ return el[d0]; }
  const T&		FastEl2(int d1, int d0) const { return el[ElIndex2(d1,d0)]; }
  const T&		FastEl3(int d2, int d1, int d0) const { return el[ElIndex3(d2,d1,d0)]; }
  const T&		FastEl4(int d3, int d2, int d1, int d0) const { return el[ElIndex4(d3,d2,d1,d0)]; } //
  const T&		FastElN(const int_Array& indices) const {return el[ElIndexN(indices)]; }
  
  TA_ABSTRACT_TMPLT_BASEFUNS(taMatrix, T)
public:
  override void*	FastEl_(int i)	{ return &(el[i]); } 
  override const void*	FastEl_(int i) const { return &(el[i]); } 
protected:
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw) {if ((el != NULL) && (alloc_size > 0)) delete [] el; el = (T*)nw;}
  override bool		El_Equal_(const void* a, const void* b) const
    { return (*((T*)a) == *((T*)b)); }
  override void		El_Copy_(void* to, const void* fm) {*((T*)to) = *((T*)fm); }
  override uint		El_SizeOf_() const	{ return sizeof(T); }

private: //note: forbid these for now -- if needed, define semantics
  void			Initialize()	{el = NULL;}
  void			Destroy() { SetArray_(NULL);}
};


class taMatrixPtr_impl { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS "safe" ptr for taBase objects -- automatically does ref counts
public:
  taMatrixPtr_impl() {m_ptr = NULL;}
  ~taMatrixPtr_impl() {set(NULL);} //
  
  // WARNING: these permit incorrect assignments to strongly typed pointers, use with caution
  taMatrixPtr_impl(taMatrixPtr_impl& src) {m_ptr = NULL; set(src.m_ptr);} 
  taMatrix_impl* operator=(taMatrixPtr_impl& src) {set(src.m_ptr); return m_ptr;} 
  taMatrix_impl* operator=(taMatrix_impl* src) {set(src); return m_ptr;} 
  operator taMatrix_impl*() const {return m_ptr;}
protected:
  taMatrix_impl*	m_ptr;
  void		set(taMatrix_impl* src) {taBase::SetPointer((taBase**)(&m_ptr), src);}
};


// macro for creating smart ptrs of taMatrix classes
#define taMatrixPtr_Of(T)  class T ## Ptr: public taMatrixPtr_impl { \
public: \
  T* ptr() const {return (T*)m_ptr;} \
  operator T*() const {return (T*)m_ptr;} \
  T* operator->() const {return (T*)m_ptr;} \
  T* operator=(T ## Ptr& src) {set((T*)src.m_ptr); return (T*)m_ptr;} \
  T* operator=(T* src) {set(src); return (T*)m_ptr;} \
  T ## Ptr() {} \
  T ## Ptr(T ## Ptr& src) {set((T*)src.m_ptr);} \
  T ## Ptr(T* src) {set(src);} \
};


/* XxxData
 
   XxxData is an N-dimension array of data.
   Each concrete class holds one specific type of data, ex. byte or float. 
   The number of dimensions is set at create time, and is lifetime-invariant.
   The number of elements in each dimension is usually fixed.
   Data is stored such that the highest dimension items are adjacent, ex:
     [2][3]: 00, 01, 02, 10, 11, 12
   The first dimension may be unspecified (0) -- the data will automatically grow.
   
   The object supports partially filled arrays, but not ragged arrays.

*/

#define TA_MATRIX_FUNS(y,T) \
  STATIC_CONST T blank; \
  explicit y(int d0)		{SetGeom(d0);} \
  y(int d1, int d0)		{SetGeom2(d1, d0);} \
  y(int d2, int d1, int d0)	{SetGeom3(d2, d1, d0);} \
  y(int d3, int d2, int d1, int d0) {SetGeom4(d3, d2, d1, d0);} \
  explicit y(const int_Array& geom_) {SetGeomN(geom_);} \
  y(T* data_, const int_Array& geom_) {SetFixedData(data_, geom_);} \
  TA_BASEFUNS(y) \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; }

class byte_Matrix: public taMatrix<byte> { // #INSTANCE
public:
  override TypeDef*	data_type() const {return &TA_unsigned_char;} 
  
  void			Copy_(const byte_Matrix& cp) {}
  COPY_FUNS(byte_Matrix, taMatrix<byte>)
  TA_MATRIX_FUNS(byte_Matrix, byte)
private:
  void		Initialize() {}
  void		Destroy() {}
};

taMatrixPtr_Of(byte_Matrix)


class float_Matrix: public taMatrix<float> { // #INSTANCE
public:
  override TypeDef*	data_type() const {return &TA_float;} 
  
  void			Copy_(const float_Matrix& cp) {}
  COPY_FUNS(float_Matrix, taMatrix<float>)
  TA_MATRIX_FUNS(float_Matrix, float)
private:
  void		Initialize() {}
  void		Destroy() {}
};

taMatrixPtr_Of(float_Matrix)




#endif
