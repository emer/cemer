// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// ta_matrix.h -- implementation header for matrix data (used by data system)

#ifndef TA_MATRIX_H
#define TA_MATRIX_H

#include "ta_stdef.h"

#include "ta_list.h"
#include "ta_type.h"
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
   The number of elements in each dimension is usually fixed.
   Data is stored such that the highest dimension items are adjacent, ex:
     [2][3]: 00, 01, 02, 10, 11, 12
   The value for the first dimension (geom[0]) is special:
     if geom[0]=0 then no allocation is made, and the data will resize
     if geom[0]=N, then data is allocated at creation -- note
       that the allocated data is uninitialized
   
   The object supports partially filled arrays, but not ragged arrays.
   Storage in Matrix is based on an instance of the taArray<> template.

   NOTE: Matrix in its current form is not streamable.
   
   Matrix vs. Array
   'Array' classes are typically 1-d vectors or interpreted as 2-d arrays.
   Array supports dynamic operations, like inserting, sorting, etc.
   Matrix is ref-counted, and intended for sharing/moving raw data around.
   Matrix explicitly supports dimensionality and dimensional access.
   
*/

class taMatrix_impl: public taBasicArray_impl { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS ref counted multi-dimensional data array
INHERITED(taBasicArray_impl)
public:
  int			dims() {return m_geom.size;}
  int			geom(int dim) {return m_geom.FastEl(dim);} // note: dim must be in range
  
  void			setGeom(const int_FixedArray& geom) 
    {if (m_geom == geom) return; setGeom_(geom.size, geom.el);} 
  // sets geom, doing sanity checks and allocating storage if geom[0]!=0
  void			setGeom(int d0)  
    {setGeom(int_FixedArray(1, d0));} 
  void			setGeom(int d0, int d1)  
    {setGeom(int_FixedArray(2, d0, d1));} 
  void			setGeom(int d0, int d1, int d2)  
    {setGeom(int_FixedArray(3, d0, d1, d2));} 
  void			setGeom(int d0, int d1, int d2, int d3)  
    {setGeom(int_FixedArray(4, d0, d1, d2, d3));} 
  
  virtual TypeDef*	GetTypeDef() const = 0; // 
  
  void			Ref() {++m_ref;}
  void			Unref() {if (--m_ref == 0) delete this;} //
 
protected:
  int			ElIndex(const int indices[]) const; 
  int			ElIndex(int i, int j) const; 
  int			ElIndex(int i, int j, int k) const; 
  int			ElIndex(int i, int j, int k, int l) const; 
  
  uint			m_ref;
  int_FixedArray	m_geom; // dimensions array
  
  virtual void		Copy_(const taMatrix_impl& cp); // NOTE: taMatrix cannot be accurately copied through taBasicArray_impl pointers because of geom
  override bool		Equal_(const taFixedArray_impl& src) const; 
    // 'true' if same size and els, and src is our type
  virtual bool		Equal_(const taMatrix_impl& src) const; 
    // 'true' if same size and els
  virtual void		setGeom_(int dims_, const int geom_[]); 
  
  taMatrix_impl(); 
  ~taMatrix_impl();
private:
  int_FixedArray	m_dmx; // cached pre-calculated dimension multipliers
};


template<class T> 
class taMatrix : public taMatrix_impl { // #INSTANCE
public:
  T*		el;		// #HIDDEN #NO_SAVE Pointer to actual array memory

  taMatrix()				{el = NULL;}

  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  const T&	SafeEl(int i) const {return *((T*)SafeEl_(i));}
  // the element at the given index
  T&		FastEl(int i) 	{ return el[i]; }
  T&		FastEl(int i, int j) { return el[ElIndex(i,j)]; }
  T&		FastEl(int i, int j, int k) { return el[ElIndex(i,j,k)]; }
  T&		FastEl(int i, int j, int k, int l) { return el[ElIndex(i,j,k,l)]; }
  
  void		Copy(const taMatrix<T>& cp)	{Copy_(cp);} // #IGNORE maketa bug

public:
  override void*	FastEl_(int i)	{ return &(el[i]); } 
protected:
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw) {if (el) delete [] el; el = (T*)nw;}
  override bool		El_Equal_(const void* a, const void* b) const
    { return (*((T*)a) == *((T*)b)); }
  override void		El_Copy_(void* to, const void* fm) {*((T*)to) = *((T*)fm); }
  override uint		El_SizeOf_() const	{ return sizeof(T); }

  ~taMatrix()			{ SetArray_(NULL); } //
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
  y(int d0, int d1)		{setGeom(d0, d1);} \
  y(int d0, int d1, int d2)	{setGeom(d0, d1, d2);} \
  y(int d0, int d1, int d2, int d3) {setGeom(d0, d1, d2, d3);} \
  y() 		{setGeom(0);} \
  y(const y& cp)	{Copy(cp); }  \
  y& operator=(const y& cp) {Copy(cp); return *this;} \
  bool 		operator==(const y& src) const {return Equal_(src);} \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; }

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
