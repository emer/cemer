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
#include "ta_group.h"
#include "ta_TA_type.h"

// externals
class TypeDef;

// forwards this file
class byte_Matrix; //
class float_Matrix; //

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


class taMatrix: public taOBase { // #VIRT_BASE #NO_INSTANCE ##NO_TOKENS ref counted multi-dimensional data array
INHERITED(taOBase)
public:
  static bool		GeomIsValid(int dims_, const int geom_[], String* err_msg = NULL);
    // #IGNORE validates proposed geom, ex. dims >=1, and valid values for supplied geoms
  static bool		GeomIsValid(const int_Array& geom_, String* err_msg = NULL)
    {return GeomIsValid(geom_.size, geom_.el, err_msg);}
    // validates proposed geom, ex. dims >=1, and valid values for supplied geoms
  static String		GeomToString(const int_Array& geom); // returns human-friendly text in form: "[{dim}{,dim}]"
  
  int 			size;	// #SHOW #READ_ONLY number of elements in the matrix (= frames*frameSize)
  int_Array		geom; // #SHOW #READ_ONLY dimensions array
  
  bool			canResize() const; // true only if not fixed NOTE: may also include additional constraints, tbd
  virtual TypeDef*	data_type() const = 0; // type of data, ex TA_int, TA_float, etc.
  int			dims() const {return geom.size;}
  int			GetGeom(int dim) const {return geom.FastEl(dim);}
  int 			frames() const;	// number of frames currently in use (value of highest dimension) 
  int 			frameSize() const;	// number of elements in each frame (product of inner dimensions) 
  
  bool			isFixedData() const {return alloc_size < 0;} // true if using fixed (externally managed) data storage
  
  int			BaseIndexOfFrame(int fm) {return fm * frameSize();}
    // returns the flat base index of the specified frame
  
  // universal string access/set, for flat array
  String		FastElAsStr_Flat(int idx) const	{ return El_GetStr_(FastEl_(idx)); } 
    // treats the matrix like a flat array, returns the element as a string
  String		SafeElAsStr_Flat(int idx) const	
    { if (InRange_Flat(idx)) return El_GetStr_(FastEl_(idx)); else return _nilString; } 
    // treats the matrix like a flat array, returns the element as a string
  void			SetFmStr_Flat(int idx, const String& str) 	
    {if (InRange_Flat(idx))  El_SetFmStr_(FastEl_(idx), str); } 
    // treats the matrix like a flat array, sets the element as a string
  virtual bool		StrValIsValid(const String& str, String* err_msg = NULL) const
    {return true;}
    // validates a proposed string-version of a value, ex. float_Matrix can verify valid floating rep of string 
     

  virtual void		AddFrames(int n); // add n new blank frames
  virtual void		AllocFrames(int n); // make sure space exists for n frames
  virtual void		EnforceFrames(int n); // set size to n frames, blanking new elements if added
  virtual void		RemoveFrame(int n); // remove the given frame, copying data backwards if needed
  virtual void		Reset() {EnforceFrames(0);}
  
  bool			InRange(int d0) const; // 'true' if >= 1-d and index in range
  bool			InRange2(int d0, int d1) const;  // 'true' if >= 2-d and indices in range
  bool			InRange3(int d0, int d1, int d2) const;  // 'true' if >= 3-d and indices in range
  bool			InRange4(int d0, int d1, int d2, int d3) const;  // 'true' if >= 4-d and indices in range
  bool			InRangeN(const int_Array& indices) const;  // 'true' if >= indices-d and indices in range
  
  void			SetGeom(int d0)  
    {int d[1]; d[0]=d0; SetGeom_(1, d);} // set geom for 1-d array
  void			SetGeom2(int d0, int d1)  
    {int d[2]; d[0]=d0; d[1]=d1; SetGeom_(2, d);} // set geom for 12-d array
  void			SetGeom3(int d0, int d1, int d2)  
    {int d[3]; d[0]=d0; d[1]=d1; d[2]=d2; SetGeom_(3, d);} // set geom for 3-d array
  void			SetGeom4(int d0, int d1, int d2, int d3)  
    {int d[4]; d[0]=d0; d[1]=d1; d[2]=d2; d[3]=d3; SetGeom_(4, d);} // set geom for 4-d array
  void			SetGeomN(const int_Array& geom_) 
    {SetGeom_(geom_.size, geom_.el);} // set geom for any sized array
  
  virtual void 		List(ostream& strm=cout) const; 	// List the items
  
  ostream& 		Output(ostream& strm, int indent = 0) const;
  ostream& 		OutputR(ostream& strm, int indent = 0) const
    { return Output(strm, indent); }
  int			Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  int			Dump_Load_Value(istream& strm, TAPtr par=NULL);
  void			InitLinks();
  void			CutLinks();
  void			UpdateAfterEdit(); // esp important to call after changing geom -- note that geom gets fixed if invalid
  void			Copy_(const taMatrix& cp);
  COPY_FUNS(taMatrix, taOBase);
  TA_ABSTRACT_BASEFUNS(taMatrix) //
public: // don't use these, internal use only
  virtual void*		data() const = 0;  // #IGNORE
  virtual void*		FastEl_(int i) = 0;   // #IGNORE the raw element in the flat space
  virtual const void*	FastEl_(int i) const = 0;   // #IGNORE
  virtual const void*	SafeEl_(int i) const 
    {if ((i > 0) && (i < size)) return FastEl_(i); else return NULL;}   // #IGNORE raw element in flat space, else NULL
  virtual String	El_GetStr_(const void*) const		{ return _nilString; } // #IGNORE
  virtual void		El_SetFmStr_(void*, const String&) 	{ };       // #IGNORE
 
protected:
  int			alloc_size; // -1 means fixed (external data)
  
  virtual void		SetGeom_(int dims_, const int geom_[]); //
  
  // the FastElIndex functions only check the bounds in debug version
  int			FastElIndex(int d0) const; 
  int			FastElIndex2(int d0, int d1) const; 
  int			FastElIndex3(int d0, int d1, int d2) const; 
  int			FastElIndex4(int d0, int d1, int d2, int d3) const; 
  int			FastElIndexN(const int_Array& indices) const; 
  
  // the SafeElIndex functions always check the bounds
  int			SafeElIndex(int d0) const; 
  int			SafeElIndex2(int d0, int d1) const; 
  int			SafeElIndex3(int d0, int d1, int d2) const; 
  int			SafeElIndex4(int d0, int d1, int d2, int d3) const; 
  int			SafeElIndexN(const int_Array& indices) const; 
  
  virtual void		Alloc_(int new_alloc); // set capacity to n -- should always be in multiples of frames 
  virtual void*		MakeArray_(int i) const = 0; // #IGNORE make a new array of item type; raise exception on failure
  virtual void		SetArray_(void* nw) = 0;
  virtual void		ReclaimOrphans_(int from, int to) {} // called when elements can be reclaimed, ex. for strings
  
  virtual void		Add_(const void* it); // compatibility function -- only valid if dims=1
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
  
  inline bool		InRange_Flat(int idx) const {return ((idx >= 0) && (idx < size));}
    // checks if in actual range
  
  virtual void		SetFixedData_(void* el_, const int_Array& geom_); // initialize fixed data
//  virtual bool		Equal_(const taMatrix& src) const; 
    // 'true' if same size and els
  virtual void		UpdateGeom(); // called to potentially update the allocation based on new geom info -- will fix if in error
private:
  void 			Initialize();
  void			Destroy();
};

typedef taMatrix* ptaMatrix_impl;

class taMatrix_Group: public taGroup<taMatrix> { // group that can hold matrix items -- typically used for dataset elements
INHERITED(taGroup<taMatrix>)
public:

  TA_BASEFUNS(taMatrix_Group);
private:
  void		Initialize() {SetBaseType(&TA_taMatrix);}
  void		Destroy() {}
};


template<class T> 
class taMatrixT : public taMatrix { // #VIRT_BASE #NO_INSTANCE 
public:
  T*		el;		// #HIDDEN #NO_SAVE Pointer to actual array memory

  override void*	data() const {return el;}
  
  void			SetFixedData(T* data_, const int_Array& geom_) {SetFixedData_(data_, geom_);} 
  // sets external (fixed) data, setting the geom/size
  
  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  T&			FastEl_Flat(int idx) // #IGNORE	treats matrix like a flat array
    { return el[idx]; }
  T&			FastEl(int d0) // #IGNORE 	
    { return el[FastElIndex(d0)]; }
  T&			FastEl2(int d0, int d1) // #IGNORE 
    { return el[FastElIndex2(d0,d1)]; }
  T&			FastEl3(int d0, int d1, int d2) // #IGNORE 
    { return el[FastElIndex3(d0,d1,d2)]; }
  T&			FastEl4(int d0, int d1, int d2, int d3) // #IGNORE 
    { return el[FastElIndex4(d0,d1,d2,d3)]; } 
  T&			FastElN(const int_Array& indices) // #IGNORE 
    {return el[FastElIndexN(indices)]; }
  
  const T&		FastEl_Flat(int idx) const // #IGNORE	treats matrix like a flat array
    { return el[idx]; }
  const T&		FastEl(int d0) const // 	
    { return el[FastElIndex(d0)]; }
  const T&		FastEl2(int d0, int d1) const //  
    { return el[FastElIndex2(d0,d1)]; }
  const T&		FastEl3(int d0, int d1, int d2) const //  
    { return el[FastElIndex3(d0,d1,d2)]; }
  const T&		FastEl4(int d0, int d1, int d2, int d3) const //  
    { return el[FastElIndex4(d0,d1,d2,d3)]; } 
  const T&		FastElN(const int_Array& indices) const //  
    { return el[FastElIndexN(indices)]; } 
  
  const T&		SafeEl_Flat(int idx) const 	
    { return *((T*)(SafeEl_(idx))); } // access the matrix as if it were a flat vector, for reading
  const T&		SafeEl(int d0) const 	
    { return el[SafeElIndex(d0)]; } // access the element for reading
  const T&		SafeEl2(int d0, int d1) const  
    { return el[SafeElIndex2(d0,d1)]; } // access the element for reading
  const T&		SafeEl3(int d0, int d1, int d2) const  
    { return el[SafeElIndex3(d0,d1,d2)]; } // access the element for reading
  const T&		SafeEl4(int d0, int d1, int d2, int d3) const  
    { return el[SafeElIndex4(d0,d1,d2,d3)]; }  // access the element for reading
  const T&		SafeElN(const int_Array& indices) const  
    { return el[SafeElIndexN(indices)]; }  // access the element for reading
  
  void			Set_Flat(int idx, const T& item) 	
    { if (InRange_Flat(idx)) el[idx] = item; }
  // use this for safely assigning values to items in the matrix, treated as a flat vector
  void			Set(int d0, const T& item) 	
    { el[SafeElIndex(d0)] = item; }
  // use this for safely assigning values to items in the matrix, esp. from script code
  void			Set2(int d0, int d1, const T& item) 	
    { el[SafeElIndex2(d0,d1)] = item; }
  // use this for safely assigning values to items in the matrix, esp. from script code
  void			Set3(int d0, int d1, int d2, const T& item) 	
    {  el[SafeElIndex3(d0,d1,d2)] = item; }
  // use this for safely assigning values to items in the matrix, esp. from script code
  void			Set4(int d0, int d1, int d2, int d3, const T& item) 	
    {  el[SafeElIndex4(d0,d1,d2,d3)] = item; }
  // use this for safely assigning values to items in the matrix, esp. from script code
  void			SetN(const int_Array& indices, const T& item) 	
    {  el[SafeElIndexN(indices)] = item; }
  // use this for safely assigning values to items in the matrix, esp. from script code
  
  // compatibility functions, for when dims=1
  void			Add(const T& item) {Add_(&item);}  // only valid when dims=1

  TA_ABSTRACT_TMPLT_BASEFUNS(taMatrixT, T)
public:
  override void*	FastEl_(int idx)	{ return &(el[idx]); } 
  override const void*	FastEl_(int idx) const { return &(el[idx]); } 
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


class taMatrixPtr { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS "safe" ptr for Matrix objects -- automatically does ref counts
public:
  taMatrix*	ptr() {return m_ptr;} //note: strong types define strongly typed version
  const taMatrix*	ptr() const {return m_ptr;} //note: strong types define strongly typed version
  
  taMatrixPtr() {m_ptr = NULL;}
  ~taMatrixPtr() {set(NULL);} //
  
  taMatrix* operator->() const {return m_ptr;} 
  operator taMatrix*() const {return m_ptr;} //
  
  // WARNING: these permit incorrect assignments to strongly typed pointers, use with caution
  taMatrixPtr(const taMatrixPtr& src) {m_ptr = NULL; set(src.m_ptr);} 
  taMatrix* operator=(taMatrixPtr& src) {set(src.m_ptr); return m_ptr;} 
  taMatrix* operator=(taMatrix* src) {set(src); return m_ptr;}  //
  
  // WARNING: these are bogus operators required to enable creating an array of items (legacy issue)
  operator taString() const {return _nilString;}
  explicit taMatrixPtr(const String& ignored) {m_ptr = NULL;}
  
protected:
  taMatrix*	m_ptr;
  void		set(taMatrix* src) {taBase::SetPointer((taBase**)(&m_ptr), src);} //
}; //

// operators for doing NULL testing on the smart pointers 
// note: these operators are only defined when int==0 i.e. NULL
// note: use '0' instead of 'NULL' in your tests to avoid a compiler warning
inline bool operator ==(const taMatrixPtr& a, int b)
  {return ((b== 0) && (a.ptr() == NULL));} 
inline bool operator ==(int a, const taMatrixPtr& b)
  {return ((a == 0) && (NULL == b.ptr()));} 
inline bool operator !=(const taMatrixPtr& a, int b)
  {return ((b == 0) && (a.ptr() != NULL));}
inline bool operator !=(int a, const taMatrixPtr& b)
  {return ((a == 0) && (NULL != b.ptr()));} //

// operators for doing equality testing on the smart pointers 
inline bool operator ==(const taMatrixPtr& a, const taMatrixPtr& b)
  {return (a.ptr() == b.ptr());} 
inline bool operator ==(const taMatrixPtr& a, const taMatrix* b)
  {return (a.ptr() == b);} 
inline bool operator ==(const taMatrix* a, const taMatrixPtr& b)
  {return (a == b.ptr());} 
inline bool operator !=(const taMatrixPtr& a, const taMatrixPtr& b)
  {return (a.ptr() != b.ptr());}
inline bool operator !=(const taMatrixPtr& a, const taMatrix* b)
  {return (a.ptr() != b);}
inline bool operator !=(const taMatrix* a, const taMatrixPtr& b)
  {return (a != b.ptr());}


/*nn // bogus operators
inline bool operator <(const taMatrixPtr& a, const taMatrixPtr& b)
  {return false;} 
inline bool operator >(const taMatrixPtr& a, const taMatrixPtr& b)
  {return false;}  */

// macro for creating smart ptrs of taMatrixT classes
#define taMatrixPtr_Of(T)  class T ## Ptr: public taMatrixPtr { \
public: \
  T* ptr() const {return (T*)m_ptr;} \
  operator T*() const {return (T*)m_ptr;} \
  T* operator->() const {return (T*)m_ptr;} \
  T* operator=(T ## Ptr& src) {set((T*)src.m_ptr); return (T*)m_ptr;} \
  T* operator=(T* src) {set(src); return (T*)m_ptr;} \
  T ## Ptr() {} \
  T ## Ptr(const T ## Ptr& src) {set((T*)src.m_ptr);} \
  T ## Ptr(T* src) {set(src);} \
};


class MatrixPtr_Array : public taArray<taMatrixPtr> {
  // #NO_UPDATE_AFTER array (list) of matrix pointers -- used typically for multi params in data processing
public:
  STATIC_CONST taMatrixPtr blank; // #HIDDEN #READ_ONLY 

  override void*	GetTA_Element(int i, TypeDef*& eltd) 
  { eltd = &TA_taMatrixPtr; return FastEl_(i); }
  void Initialize()	{};
  void Destroy()	{ };
  TA_BASEFUNS(MatrixPtr_Array);
  TA_ARRAY_FUNS(MatrixPtr_Array, taMatrixPtr)
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
  y(int d0, int d1)		{SetGeom2(d0,d1);} \
  y(int d0, int d1, int d2)	{SetGeom3(d0,d1,d2);} \
  y(int d0, int d1, int d2, int d3) {SetGeom4(d0,d1,d2,d3);} \
  explicit y(const int_Array& geom_) {SetGeomN(geom_);} \
  y(T* data_, const int_Array& geom_) {SetFixedData(data_, geom_);} \
  TA_BASEFUNS(y) \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; }

class String_Matrix: public taMatrixT<String> { // #INSTANCE
public:
  override TypeDef*	data_type() const {return &TA_taString;} 
  
  void			Copy_(const String_Matrix& cp) {}
  COPY_FUNS(String_Matrix, taMatrixT<String>)
  TA_MATRIX_FUNS(String_Matrix, String)
  
public:
  override String	El_GetStr_(const void* it) const { return *((String*)it); } // #IGNORE
  override void		El_SetFmStr_(void* it, const String& str) {*((String*)it) = str;}  // #IGNORE
private:
  void		Initialize() {}
  void		Destroy() {}
};

taMatrixPtr_Of(String_Matrix)
//


class float_Matrix: public taMatrixT<float> { // #INSTANCE
public:
  override TypeDef*	data_type() const {return &TA_float;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts valid format for float
    
  void			Copy_(const float_Matrix& cp) {}
  COPY_FUNS(float_Matrix, taMatrixT<float>)
  TA_MATRIX_FUNS(float_Matrix, float)
  
public:
  override String	El_GetStr_(const void* it) const { return (String)*((float*)it); } // #IGNORE
  override void		El_SetFmStr_(void* it, const String& str) {*((float*)it) = (float)str;}  // #IGNORE
private:
  void		Initialize() {}
  void		Destroy() {}
};

taMatrixPtr_Of(float_Matrix)


class int_Matrix: public taMatrixT<int> { // #INSTANCE
public:
  override TypeDef*	data_type() const {return &TA_int;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts in-range for 32bit int
  
  void			Copy_(const int_Matrix& cp) {}
  COPY_FUNS(int_Matrix, taMatrixT<int>)
  TA_MATRIX_FUNS(int_Matrix, int)
  
public:
  override String	El_GetStr_(const void* it) const { return *((int*)it); } // #IGNORE note: implicit conversion avoids problems on some compilers
  override void		El_SetFmStr_(void* it, const String& str) {*((int*)it) = (int)str;}  // #IGNORE
private:
  void		Initialize() {}
  void		Destroy() {}
};

taMatrixPtr_Of(int_Matrix)


class byte_Matrix: public taMatrixT<byte> { // #INSTANCE
public:
  override TypeDef*	data_type() const {return &TA_unsigned_char;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts 0-255 or octal or hex forms
  
  void			Copy_(const byte_Matrix& cp) {}
  COPY_FUNS(byte_Matrix, taMatrixT<byte>)
  TA_MATRIX_FUNS(byte_Matrix, byte)
  
public: //
  //note: for streaming, we convert to hex, rather than char
  override String	El_GetStr_(const void* it) const { return String(((int)*((byte*)it)), "x"); } // #IGNORE
  override void		El_SetFmStr_(void* it, const String& str) {*((byte*)it) = (byte)str.HexToInt();}       // #IGNORE
private:
  void		Initialize() {}
  void		Destroy() {}
};

taMatrixPtr_Of(byte_Matrix)



#endif
