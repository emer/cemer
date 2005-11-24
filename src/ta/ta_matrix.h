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

#include "ta_list.h"
#include "ta_type.h"
#include "ta_TA_type.h"

// externals
class TypeDef;

// forwards this file
class DataChunk;
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


class taMatrix_impl { // #VIRT_BASE ##NO_INSTANCE ##NO_TOKENS ref counted multi-dimensional data array
public:
  int 			size() const;	// number of elements in the matrix (= frames*frameSize)
  bool			canResize() const; // true only if not fixed, initialized, not a view, and refs==1
  int			dims() {return m_geom.size;}
  int 			frameSize() const;	// number of elements in each frame 
  int 			frames() const;	// number of frames currently in use 
  const int_FixedArray&	geom() const {return m_geom;} // dimensions array
  int			geom(int dim) {return m_geom.FastEl(dim);} // note: dim must be in range
  bool			isFixedData() const {return alloc_size < 0;} // true if using fixed (externally managed) data storage
  bool			isView() const {return (m_data_owner != NULL);} // true if we are a view
  
  void			setGeom(const int_FixedArray& geom_) 
    {setGeom_(geom_.size, geom_.el);} 
  // sets geom, doing sanity checks and allocating storage if geom[N-1]!=0
  void			setGeom(int d0)  
    {setGeom(int_FixedArray(1, d0));} 
  void			setGeom(int d1, int d0)  
    {setGeom(int_FixedArray(2, d1, d0));} 
  void			setGeom(int d2, int d1, int d0)  
    {setGeom(int_FixedArray(3, d2, d1, d0));} 
  void			setGeom(int d3, int d2, int d1, int d0)  
    {setGeom(int_FixedArray(4, d3, d2, d1, d0));} 
  
  inline bool		InRange(int idx) const {return ((idx >= 0) && (idx < alloc_size));}
    //NOTE: for efficiency, this only checks if in allocated range, not in actual range
  virtual TypeDef*	GetTypeDef() const = 0; // 
  
  void			Ref()  {++m_ref;}
  void			Unref()  {if (--m_ref == 0) delete this;} //
  
  void			AllocFrames(int n); // make sure space exists for n frames
  void			EnforceFrames(int n); // set size to n frames
public:
  virtual void*		data_() = 0;
  virtual const void*	SafeEl_(int i) const;
  virtual void*		FastEl_(int i) = 0; 
 
protected:
  int			alloc_size; // -1 means fixed (external data)
  uint			m_ref; // number of refs for this object, including 
  int_FixedArray	m_geom; // dimensions array
  int_FixedArray*	m_slice_geom; // if sliced, holds dimensions of slices, else NULL
  int_FixedArray*	m_slice_offset; // if sliced, holds offset of slices, else NULL
  taMatrix_impl*	m_data_owner; // if non-null, then we are a view
  
  int			ElIndex(const int_FixedArray& indices) const; 
  int			ElIndex(int d1, int d0) const; 
  int			ElIndex(int d2, int d1, int d0) const; 
  int			ElIndex(int d3, int d2, int d1, int d0) const; 
  
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
  
  void			InitView_(taMatrix_impl& data_owner_);
  
//TBD  virtual void		Copy_(const taMatrix_impl& cp);
//  virtual bool		Equal_(const taMatrix_impl& src) const; 
    // 'true' if same size and els
  virtual void		setGeom_(int dims_, const int geom_[]); 
  
  taMatrix_impl(); 
  ~taMatrix_impl();
};


template<class T> 
class taMatrix : public taMatrix_impl { 
public:
  T*		el;		// #HIDDEN #NO_SAVE Pointer to actual array memory

  taMatrix()				{el = NULL;}

  void			setFixedData(const int_FixedArray& geom_, T* data_)
    {SetArray_(NULL); alloc_size = -1; setGeom(geom_); el = data_;} 
  // sets external (fixed) data, setting the geom
  
  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  const T&	SafeEl(int i) const {return *((T*)SafeEl_(i));}
  // the element at the given index
  T&		FastEl(int i) 	{ return el[i]; }
  T&		FastEl(int i, int j) { return el[ElIndex(i,j)]; }
  T&		FastEl(int i, int j, int k) { return el[ElIndex(i,j,k)]; }
  T&		FastEl(int i, int j, int k, int l) { return el[ElIndex(i,j,k,l)]; } //
  
//TBD  void		Copy(const taMatrix<T>& cp)	{Copy_(cp);} // #IGNORE maketa bug

public:
  override void*	data_() {return el;}
  override void*	FastEl_(int i)	{ return &(el[i]); } 
protected:
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw) {if ((el != NULL) && (alloc_size > 0)) delete [] el; el = (T*)nw;}
  override bool		El_Equal_(const void* a, const void* b) const
    { return (*((T*)a) == *((T*)b)); }
  override void		El_Copy_(void* to, const void* fm) {*((T*)to) = *((T*)fm); }
  override uint		El_SizeOf_() const	{ return sizeof(T); }

  ~taMatrix()			{ SetArray_(NULL); } //
private: //note: forbid these for now -- if needed, define semantics
  taMatrix(const taMatrix<T>& cp); // not implemented
  taMatrix&		operator =(const taMatrix<T>& cp); // not implemented
};


class MatrixPtr_impl { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS "safe" ptr for Matrix objects -- automatically does ref counts
public:
  taMatrix_impl*	ptr() const {return m_ptr;}
  operator taMatrix_impl*() const {return m_ptr;}
  taMatrix_impl* operator->() const {return m_ptr;} //

  // generic operators that are unsafe for derived classes
  taMatrix_impl* operator=(MatrixPtr_impl& src) {return this->operator=(src.m_ptr);}
  virtual taMatrix_impl* operator=(taMatrix_impl* src) {set(src); return m_ptr;} 
    
  MatrixPtr_impl();
  virtual ~MatrixPtr_impl(); // note: only virtual to shut compiler up
protected:
  taMatrix_impl*	m_ptr;
  void		set(taMatrix_impl* src);
};

template<class T>
class MatrixPtr: public MatrixPtr_impl { 
public:
  T*		ptr() const {return static_cast<T*>(m_ptr);}

  operator T*() const {return static_cast<T*>(m_ptr);}
  T* operator->() const {return static_cast<T*>(m_ptr);}

  override taMatrix_impl* operator=(taMatrix_impl* src) 
    {assert(((src == NULL) || (dynamic_cast<T*>(src) != NULL)) && 
      "MatrixPtr::= incompatible source type");
    MatrixPtr_impl::set(src); return m_ptr;} 

  T* operator=(MatrixPtr<T>& src) {set(src.m_ptr); return static_cast<T*>(m_ptr);}
  T* operator=(T* src) {set(src); return static_cast<T*>(m_ptr);} 
  MatrixPtr() {} 
  MatrixPtr(MatrixPtr<T>& src) {set(src.m_ptr);} 
  MatrixPtr(T* src) {set(src);}
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
public: \
  STATIC_CONST T blank; \
  static TypeDef* StatTypeDef(int) { return &TA_##y; } \
  TypeDef* GetTypeDef() const { return &TA_##y; } \
  explicit y(const int_FixedArray& geom_) {setGeom(geom_);} \
  explicit y(int d0)		{setGeom(d0);} \
  y(const int_FixedArray& geom_, T* data_) {setFixedData(geom_, data_);} \
  y(int d1, int d0)		{setGeom(d1, d0);} \
  y(int d2, int d1, int d0)	{setGeom(d2, d1, d0);} \
  y(int d3, int d2, int d1, int d0) {setGeom(d3, d2, d1, d0);} \
  y() 		{setGeom(0);} \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; } \
private: \
  y(const y& cp);  \
  y& operator=(const y& cp); \

class byte_Matrix: public taMatrix<byte> {
public:
  override TypeDef*	data_type() const {return &TA_unsigned_char;} 
  TA_MATRIX_FUNS(byte_Matrix, byte)
};

typedef MatrixPtr<byte_Matrix> ByteMatrixPtr;


class float_Matrix: public taMatrix<float> {
public:
  override TypeDef*	data_type() const {return &TA_float;} 
  
  TA_MATRIX_FUNS(float_Matrix, float)
};

typedef MatrixPtr<float_Matrix> FloatMatrixPtr;




#endif
