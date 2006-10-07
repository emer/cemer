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

#include "ta_variant.h"
#include "ta_base.h"
#include "ta_group.h"
#include "ta_TA_type.h"

#ifndef __MAKETA__
#  include <QAbstractTableModel>
#endif


// externals
class TypeDef;

// forwards this file
class taMatrix_PList;
class byte_Matrix; //
class float_Matrix; //
class MatrixTableModel;

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
   
   The object does NOT supports partially filled arrays.
   Strongly-typed instances support external fixed data, via setFixedData() call.
   If fixed data is used, the matrix is NOT resizable.
   
   Frames
     a "frame" is a set of data comprising the outermost dimension;
     for example, for a 2-d matrix, each frame is one row (for 1-d, a frame
     is the same as a cell.)
     
   Accessors
     most routines provide three accessor variants:
     
     Xxx(d0, d1, d2, d3, d4) this is the most common way to access the data --
       d's higher than the actual dimension of the matrix are ignored
       
     XxxN(const MatrixGeom&) -- for any dimensionality -- it is unspecified whether
       the dims may be higher, but there must be at least the correct amount
       
     Xxx_Flat(int idx) -- treats the elements as a flat 1-d array -- storage is
       in *row-major order*, i.e., the innermost dimension changes most rapidly
       
     "Safe" accessors do bounds checks on the individual indices, as well
        as the final flat index -- a blank value is returned for out-of-bound
        values -- it is acceptable (and expected) for out-of-bounds indexes
        to occur
     "Fast" accessors do not check bounds and may not check flat indexes -- they
        must only be used in "guaranteed" index-safe code (i.e., where
        index values are being driven directly from the matrix itself.) 

   Slicing
     A "slice" is a reference to one frame of a matrix. Slices are used for 
     things like viewing the cell content of a Matrix Column in a DataTable, 
     passing a single data pattern as an event to a network, and so on. A slice
     is created by making a new instance of the parent matrix type, and initializing
     it with a fixed data pointer to the parent data, and the appropriate
     geometry (which is always 1-d less than the parent.) Each slice adds one
     to the ref count of its parent, so as long as correct ref semantics are
     used, it is not possible to delete a parent prior to its slice children.
     
     Slices are updated when a parent matrix is resized or redimensioned. 
     HOWEVER it is important that slice clients are aware of when parent 
     resizing may occur, and insure they are not in the process of iterating
     the data that is being replaced.
     
   Resizing
     A matrix can be expanded or shrunk in units of frames.
     
     A matrix object can be redimensioned, however this is discouraged -- the 
     supported paradigm is that a matrix should retain a specific geometry for
     its lifetime. If a matrix is redimensioned, all slices will be "collapsed",
     meaning the will be set to 0 size.
     
   Generic vs. Strongly Typed Access
     Matrix objects are always strongly typed, and can be accessed using 
     strongly typed accessor functions -- the "Fast" versions of these are
     particularly fast, and "_Flat" can be extremely efficient.
     
     All matrix objects can also use Variant and String accessors to access
     values generically, or polymorphically. Variant values will use the 
     underlying type, where possible (ex int_Matrix::GetVar return int Variant).
     
     The String value is used for streaming and file save/load.
     
   Notifications
     Matrix objects maintain two parallel notification mechanisms:
       * the standard taBase datalink-based notification
       * Qt AbstractItemModel notifications
       
     Changes to data do *not* automatically cause data notifications (this would
     add an unacceptable penalty to most code.) However, data changes that
     are mediated by the Qt model do, so other grid views will automatically stay
     updated.
     
   Matrix vs. Array
     'Array' classes are typically 1-d vectors or interpreted as 2-d arrays.
     Array supports dynamic operations, like inserting, sorting, etc.
     Matrix is ref-counted, and intended for sharing/moving raw data around.
     Matrix explicitly supports dimensionality and dimensional access.
     Matrix supports advanced tabular editing operations.
     Matrix forms the basis for most pdp4 data processing constructs.
   
*/

#define TA_MATRIX_DIMS_MAX 5
#define IMatrix taMatrix

class TA_API MatrixGeom: public taBase  { 
  // matrix geometry, similar to an array of int
INHERITED(taBase)
friend class taMatrix;
public:
  int			size; // DO NOT SET DIRECTLY, use EnforceSize
  
  bool			Equal(const MatrixGeom& other) const;
  inline bool		InRange(int idx) const {return ((idx >= 0) && (idx < size));}
  bool			IsFrameOf(const MatrixGeom& other) const; 
    // 'true' if this is a proper frame of other
  int 			Product() const; // returns product of all elements
  
  void			EnforceSize(int sz); // sets to size, zeroing orphaned or new dims
  int			SafeEl(int i) const {if (InRange(i)) return el[i]; else return 0;}
    // the element at the given index
  
  void			Add(int value); // safely add a new element
  void			Set(int i, int value) // safely set an element
    {if (InRange(i)) el[i] = value;}
  void			SetGeom(int dims, int d0, int d1=0, int d2=0, int d3=0, int d4=0);
    
  inline void		Reset() {EnforceSize(0);} // set size to 0, and clear all dims
  
  override int		Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  override int		Dump_Load_Value(istream& strm, TAPtr par=NULL);
  void			Copy_(const MatrixGeom& cp);
  void	UpdateAfterEdit(); // paranoically set unused vals to 0
  explicit MatrixGeom(int init_size);
  MatrixGeom(int dims, int d0, int d1=0, int d2=0, int d3=0, int d4=0);
  COPY_FUNS(MatrixGeom, taBase);
  TA_BASEFUNS_LITE(MatrixGeom); //

public: // functions for internal/trusted use only
  inline int		FastEl(int i) const {return el[i];} // #IGNORE
  inline int&		FastEl(int i) {return el[i];} // #IGNORE
    
  inline int&		operator [](int i) {return el[i];}  // #IGNORE 

protected:
  int			el[TA_MATRIX_DIMS_MAX];
  
  inline int		operator [](int i) const {return el[i];}  

private:
  void			Initialize();
  void			Destroy();
};

inline bool operator ==(const MatrixGeom& a, const MatrixGeom& b)
  {return a.Equal(b);}
inline bool operator !=(const MatrixGeom& a, const MatrixGeom& b)
  {return !a.Equal(b);}

class TA_API taMatrix: public taOBase { // #VIRT_BASE #NO_INSTANCE ##TOKENS #CAT_Data ref counted multi-dimensional data array
INHERITED(taOBase)
friend class MatrixTableModel;

public: // ITypedObject i/f
// TypeDef*		GetTypeDef() const; // in taBase
  void*			This() {return (void*)this;}
  
public: // IMatrix i/f
  inline int		count() const {return size;} // the number of items
  inline int		dims() const {return geom.size;} // the number of dimensions
  inline int		dim(int d) const {return geom.el[d];} // the value of dimenion d -- MUST BE IN_RANGE
  int 			frames() const;	// number of frames currently in use (value of highest dimension) 
  int 			frameSize() const;	// number of elements in each frame (product of inner dimensions) 
  int			rowCount(); // flat row count, for 2-d grid operations, only 0 if empty
   	
  int			FrameToRow(int f) const; // convert frame number to row number
  
  virtual TypeDef*	GetDataTypeDef() const = 0; // type of data, ex TA_int, TA_float, etc.
   	
  const String		SafeElAsStr(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const	
    {return SafeElAsStr_Flat(SafeElIndex(d0, d1, d2, d3, d4)); }
    // (safely) returns the element as a string
  const String		SafeElAsStrN(const MatrixGeom& indices) const	
    {return SafeElAsStr_Flat(SafeElIndexN(indices)); }  
    // (safely) returns the element as a string
  const String		SafeElAsStr_Flat(int idx) const	
    { if (InRange_Flat(idx)) return El_GetStr_(FastEl_Flat_(idx)); else return _nilString; } 
    // treats the matrix like a flat array, returns the element as a string
  
  void			SetFmStr_Flat(const String& str, int idx) 	
    {if (InRange_Flat(idx))  El_SetFmStr_(FastEl_Flat_(idx), str); } 
    // treats the matrix like a flat array, sets the element as a string
  
  const Variant		SafeElAsVar(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const	
    {return SafeElAsVar_Flat(SafeElIndex(d0, d1, d2, d3, d4)); } 
    // (safely) returns the element as a variant
  const Variant		SafeElAsVarN(const MatrixGeom& indices) const	
    {return SafeElAsVar_Flat(SafeElIndexN(indices)); }   
    // (safely) returns the element as a variant
  const Variant		SafeElAsVar_Flat(int idx) const	
    { if (InRange_Flat(idx)) return El_GetVar_(FastEl_Flat_(idx)); else return _nilVariant; } 
    // treats the matrix like a flat array, returns the element as a variant
    
  void			SetFmVar(const Variant& var, int d0, int d1=0, int d2=0, int d3=0, int d4=0) 	
    {int idx; if ((idx = SafeElIndex(d0, d1, d2, d3, d4)) >= 0)
       El_SetFmVar_(FastEl_Flat_(idx), var); } 
    // (safely) sets the element as a variant
  void			SetFmVarN(const Variant& var, const MatrixGeom& indices) 	
    {int idx; if ((idx = SafeElIndexN(indices)) >= 0)
       El_SetFmVar_(FastEl_Flat_(idx), var); } 
    // (safely) sets the element as a variant
  void			SetFmVar_Flat(const Variant& var, int idx) 	
    {if (InRange_Flat(idx))  El_SetFmVar_(FastEl_Flat_(idx), var); } 
    // treats the matrix like a flat array, (safely) sets the element as a variant
   	
public:
  static bool		GeomIsValid(int dims_, const int geom_[], String* err_msg = NULL);
    // #IGNORE validates proposed geom, ex. dims >=1, and valid values for supplied geoms
  static bool		GeomIsValid(const MatrixGeom& geom_, String* err_msg = NULL)
    {return GeomIsValid(geom_.size, geom_.el, err_msg);}
    // validates proposed geom, ex. dims >=1, and valid values for supplied geoms
  static String		GeomToString(const MatrixGeom& geom); // returns human-friendly text in form: "[{dim}{,dim}]"
  
  int 			size;	// #SHOW #READ_ONLY number of elements in the matrix (= frames*frameSize)
  MatrixGeom		geom; // #SHOW #READ_ONLY dimensions array -- you cannot change this directly, you have to use API functions to change size
  
  bool			canResize() const; 
    // true only if not fixed NOTE: may also include additional constraints, tbd
  virtual int		defAlignment() const; // default Qt alignment, left for text, right for nums
  bool			isFixedData() const {return alloc_size < 0;} 
    // true if using fixed (externally managed) data storage
  int			sliceCount() const; // number of extant slices
  	
  int			BaseIndexOfFrame(int fm) {return fm * frameSize();}
    // returns the flat base index of the specified frame
  
  // universal string access/set, for flat array
  const String		FastElAsStr_Flat(int idx) const	{ return El_GetStr_(FastEl_Flat_(idx)); } 
    // treats the matrix like a flat array, returns the element as a string
    
  // universal Variant access/set, for flat array
  const Variant		FastElAsVar_Flat(int idx) const	{ return El_GetVar_(FastEl_Flat_(idx)); } 
    // treats the matrix like a flat array, returns the element as a variant
    
  // universal numeric access -- primarily to make numeric ops efficient
  float			FastElAsFloat(int d0, int d1=0, int d2=0, int d3=0, int d4=0)
    const {return El_GetFloat_(FastEl_Flat_(FastElIndex(d0, d1, d2, d3, d4))); }
  float			FastElAsFloatN(const MatrixGeom& indices) const	
    {return El_GetFloat_(FastEl_Flat_(FastElIndexN(indices))); }   
  float			FastElAsFloat_Flat(int idx) const 
    {return El_GetFloat_(FastEl_Flat_(idx)); } 
  float			SafeElAsFloat(int d0, int d1=0, int d2=0, int d3=0, int d4=0)
    const {return SafeElAsFloat_Flat(SafeElIndex(d0, d1, d2, d3, d4)); } 
    // (safely) returns the element as a variant
  float			SafeElAsFloatN(const MatrixGeom& indices) const	
    {return SafeElAsFloat_Flat(SafeElIndexN(indices)); }   
    // (safely) returns the element as a variant
  float			SafeElAsFloat_Flat(int idx) const	
    { if (InRange_Flat(idx)) return El_GetFloat_(FastEl_Flat_(idx)); else return 0.0f; } 
    
  virtual bool		StrValIsValid(const String& str, String* err_msg = NULL) const
    {return true;}
    // validates a proposed string-version of a value, ex. float_Matrix can verify valid floating rep of string 
     

  void			AddFrame() {AddFrames(1);} // #MENU #MENU_ON_Matrix #MENU_CONTEXT  add 1 new blank frame
  virtual void		AddFrames(int n); // #MENU #MENU_ON_Matrix #MENU_CONTEXT add n new blank frames
  virtual void		AllocFrames(int n); // make sure space exists for n frames
  virtual void		EnforceFrames(int n, bool notify = true); 
    // #MENU #MENU_ON_Object #ARGC_1 set size to n frames, blanking new elements if added
  virtual void		RemoveFrame(int n); // #MENU #MENU_ON_Matrix remove the given frame, copying data backwards if needed
  virtual bool		CopyFrame(const taMatrix& src, int frame); // copy the source matrix to the indicated frame; src geom must be our frame geom; optimized for like-type mats
  
  virtual void		Reset();
  
  bool			InRange(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const; 
    // 'true' if indices in range; ignores irrelevant dims
  bool			InRangeN(const MatrixGeom& indices) const;  
    // 'true' if indices in range; MAY ignore under-supplied dims
  
  void			SetGeom(int size, int d0, int d1=0, int d2=0, int d3=0, int d4=0)  
    {int d[5]; d[0]=d0; d[1]=d1; d[2]=d2; d[3]=d3; d[4]=d4; SetGeom_(size, d);} 
    // set geom for matrix
  void			SetGeomN(const MatrixGeom& geom_) 
    {SetGeom_(geom_.size, geom_.el);} // #MENU #MENU_ON_Matrix #MENU_SEP_BEFORE set geom for any sized array
  
  // Slicing -- NOTES: 
  // 1. slices are updated if parent allocation changes -- this could collapse slice to [0]
  // 2. slices are not guaranteed to be unique (i.e. same spec may return same slice ref)
  // 3a. you *must* ref/unref the slice
  // 3b. a slice refs its parent; unrefs it on destroy
  // 4. you may only request "proper slices", i.e., full dimensional subsets
  // 5. if parent is redimensioned, all slices are collapsed to [0]
  
  virtual taMatrix*	GetSlice_(const MatrixGeom& base, 
    int slice_frame_dims = -1, int num_slice_frames = 1);
    // return a slice, sfd=-1 indicates a frame size slice; base should be a subframe boundary
  virtual taMatrix*	GetFrameSlice_(int frame);
    // return a slice, of exactly one frame; will have dim-1 of us
  
#ifdef TA_GUI
  MatrixTableModel*	GetDataModel(); // #IGNORE returns new if none exists, or existing -- enables views to be shared
#endif
  
  virtual void 		List(ostream& strm=cout) const; 	// List the items
  
  ostream& 		Output(ostream& strm, int indent = 0) const;
  ostream& 		OutputR(ostream& strm, int indent = 0) const
    { return Output(strm, indent); }
  int			Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  int			Dump_Load_Value(istream& strm, TAPtr par=NULL);
  void			UpdateAfterEdit(); 
  void			Copy_(const taMatrix& cp);
  COPY_FUNS(taMatrix, taOBase);
  TA_ABSTRACT_BASEFUNS(taMatrix) //
public: // low-level, try not to use these, internal use only
  virtual void*		data() const = 0;  // #IGNORE
  virtual void*		FastEl_Flat_(int i) = 0;   // #IGNORE the raw element in the flat space _
  virtual const void*	FastEl_Flat_(int i) const = 0;   // #IGNORE
  const void*		FastEl_(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const 
    {return FastEl_Flat_(FastElIndex(d0, d1, d2, d3, d4));} 
    // the raw element in index space -- YOU MUST ABSOLUTELY BE USING DIM-SAFE CODE
  const void*		FastElN_(const MatrixGeom& indices) const 
    {return FastEl_Flat_(FastElIndexN(indices));} 
  
  virtual const void*	SafeEl_(int i) const 
    {if ((i > 0) && (i < size)) return FastEl_Flat_(i); else return El_GetBlank_();}   // #IGNORE raw element in flat space, else NULL
  // every subclass should implement these:
  virtual float		El_GetFloat_(const void*) const	{ return 0.0f; } // #IGNORE
  virtual const String	El_GetStr_(const void*) const	{ return _nilString; } // #IGNORE
  virtual void		El_SetFmStr_(void*, const String&) { }; // #IGNORE
  virtual const Variant	El_GetVar_(const void*) const	{ return _nilVariant; } // #IGNORE
  virtual void		El_SetFmVar_(void*, const Variant&) { };  // #IGNORE
 
protected:
  static void		SliceInitialize(taMatrix* par_slice, taMatrix* child_slice); 
   // called after slice created -- static for consistency
  static void		SliceDestroying(taMatrix* par_slice, taMatrix* child_slice); 
   //called by child slice on destroy -- static because it can cause destruction
  
  int			alloc_size; // -1 means fixed (external data)
  taMatrix_PList*	slices; // list of extant slices -- created on first slice
  taMatrix*		slice_par; // slice parent -- we ref/unref it
  MatrixTableModel*	m_dm; // #IGNORE instance of dm; persists once created
  
  
  virtual void		SetGeom_(int dims_, const int geom_[]); //
  
  int			FastElIndex(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const; 
    // NO bounds check and return flat index -- YOU MUST ABSOLUTELY BE USING DIM-SAFE CODE
  int			FastElIndexN(const MatrixGeom& indices) const; //
    // NO bounds check and return flat index -- YOU MUST ABSOLUTELY BE USING DIM-SAFE CODE
  
  int			SafeElIndex(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const; 
    // check bounds and return flat index, -1 if any dim out of bounds
  int			SafeElIndexN(const MatrixGeom& indices) const; 
    // check bounds and return flat index, -1 if any dim out of bounds
  
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
  
  virtual void		SetFixedData_(void* el_, const MatrixGeom& geom_); // initialize fixed data
//  virtual bool		Equal_(const taMatrix& src) const; 
    // 'true' if same size and els
    
  void 			Slice_Collapse();
  void			Slice_Realloc(ta_intptr_t base_delta);
  
  virtual void		UpdateGeom(); // called to potentially update the allocation based on new geom info -- will fix if in error
  void			UpdateSlices_Collapse(); // collapses all the slices to []
  void			UpdateSlices_FramesDeleted(void* deletion_base, int num); // called when deleting a frame -- for each slice: update base addr if after delete base, or collapse if doesn't exist any more
  void			UpdateSlices_Realloc(ta_intptr_t base_delta); // called when allocing new mem (more or less) -- for each slice: update base addr; note: not for use if size has changed (FramesDeleted would be called)
    
  virtual void		Dump_Save_Item(ostream& strm, int idx); 
    // dump the value, term with ; generic is fine for numbers, override for strings, variants, etc.
  virtual int		Dump_Load_Item(istream& strm, int idx); 
    // load the ;-term'ed value ; generic is fine for numbers, override for strings, variants, etc.; ret is last char read, usually ;
private:
  void 			Initialize();
  void			Destroy();
};

typedef taMatrix* ptaMatrix_impl;
SmartPtr_Of(taMatrix); // taMatrixPtr


class TA_API taMatrix_PList: public taPtrList<taMatrix> { // simple list for keeping track of slices
INHERITED(taPtrList<taMatrix>)
public:

  taMatrix_PList() {}
};


class TA_API taMatrix_Group: public taGroup<taMatrix> { // group that can hold matrix items -- typically used for dataset elements
INHERITED(taGroup<taMatrix>)
public:

  TA_BASEFUNS(taMatrix_Group);
private:
  void		Initialize() {SetBaseType(&TA_taMatrix);}
  void		Destroy() {}
};


template<class T> 
class taMatrixT : public taMatrix { // #VIRT_BASE #NO_INSTANCE #CAT_Data 
public:
  T*		el;		// #HIDDEN #NO_SAVE Pointer to actual array memory

  override void*	data() const {return el;}
  
  void			SetFixedData(T* data_, const MatrixGeom& geom_) {SetFixedData_(data_, geom_);} 
  // sets external (fixed) data, setting the geom/size
  
  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  T&			FastEl_Flat(int idx) // #IGNORE	treats matrix like a flat array
    { return el[idx]; }
  T&			FastEl(int d0, int d1=0, int d2=0, int d3=0, int d4=0) // #IGNORE 	
    { return el[FastElIndex(d0)]; }
  T&			FastElN(const MatrixGeom& indices) // #IGNORE 
    {return el[FastElIndexN(indices)]; }
  
  const T&		FastEl_Flat(int idx) const // #IGNORE	treats matrix like a flat array
    { return el[idx]; }
  const T&		FastEl(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const // 	
    { return el[FastElIndex(d0,d1,d2,d3,d4)]; }
  const T&		FastElN(const MatrixGeom& indices) const //  
    { return el[FastElIndexN(indices)]; } 
  
  const T&		SafeEl_Flat(int idx) const 	
    { return *((T*)(SafeEl_(idx))); } // access the matrix as if it were a flat vector, for reading
  const T&		SafeEl(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const 	
    { return *((T*)(SafeEl_(SafeElIndex(d0,d1,d2,d3,d4)))); } 
     // access the element for reading
  const T&		SafeElN(const MatrixGeom& indices) const  
    { return *((T*)(SafeElIndexN(indices))); }  
    // access the element for reading
  
  void			Set_Flat(const T& item, int idx) 	
    { if (InRange_Flat(idx)) el[idx] = item; }
  // use this for safely assigning values to items in the matrix, treated as a flat vector
  void			Set(const T& item, int d0) 	
    { el[SafeElIndex(d0)] = item; }
  // (TODO: add remaining d's) use this for safely assigning values to items in the matrix, esp. from script code
  void			SetN(const T& item, const MatrixGeom& indices) 	
    {  el[SafeElIndexN(indices)] = item; }
  // use this for safely assigning values to items in the matrix, esp. from script code
  
  // compatibility functions, for when dims=1
  void			Add(const T& item) {Add_(&item);}  // only valid when dims=1

  void	CutLinks() 	{SetArray_(NULL); taMatrix::CutLinks();}
  TA_ABSTRACT_TMPLT_BASEFUNS(taMatrixT, T)
public:
  override void*	FastEl_Flat_(int idx)	{ return &(el[idx]); } 
  override const void*	FastEl_Flat_(int idx) const { return &(el[idx]); } 
protected:
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw) {if ((el != NULL) && (alloc_size > 0)) delete [] el; el = (T*)nw;}
  override bool		El_Equal_(const void* a, const void* b) const
    { return (*((T*)a) == *((T*)b)); }
  override void		El_Copy_(void* to, const void* fm) {*((T*)to) = *((T*)fm); }
  override uint		El_SizeOf_() const	{ return sizeof(T); }

private: //note: forbid these for now -- if needed, define semantics
  void			Initialize()	{el = NULL;}
  void			Destroy() { CutLinks();}
};

/*obs
class TA_API taMatrixPtr { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS "safe" ptr for Matrix objects -- automatically does ref counts
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


// macro for creating smart ptrs of taMatrixT classes
#define taMatrixPtr_Of(T)  class TA_API T ## Ptr: public taMatrixPtr { \
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


class TA_API MatrixPtr_Array : public taArray<taMatrixPtr> {
  // #NO_UPDATE_AFTER array (list) of matrix pointers -- used typically for multi params in data processing
public:
  STATIC_CONST taMatrixPtr blank; // #HIDDEN #READ_ONLY 

  override void*	GetTA_Element(int i, TypeDef*& eltd) 
  { eltd = &TA_taMatrixPtr; return FastEl_Flat_(i); }
  void Initialize()	{};
  void Destroy()	{ };
  TA_BASEFUNS(MatrixPtr_Array);
  TA_ARRAY_FUNS(MatrixPtr_Array, taMatrixPtr)
};*/


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
  y* GetSlice(const MatrixGeom& base, int sfd = -1, int nsf = 1) \
   {return (y*)GetSlice_(base, sfd, nsf);} \
  y* GetFrameSlice(int frame) {return (y*) GetFrameSlice_(frame);} \
  y(int dims_, int d0, int d1=0, int d2=0, int d3=0, int d4=0) \
    {SetGeom(dims_, d0,d1,d2,d3,d4);} \
  explicit y(const MatrixGeom& geom_) {SetGeomN(geom_);} \
  y(T* data_, const MatrixGeom& geom_) {SetFixedData(data_, geom_);} \
  TA_BASEFUNS(y) \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; }

class TA_API String_Matrix: public taMatrixT<String> { // #INSTANCE #CAT_Data
public:
  override int		defAlignment() const; // default Qt alignment, left for text, right for nums	
  override TypeDef*	GetDataTypeDef() const {return &TA_taString;} 
  
  void			Copy_(const String_Matrix& cp) {}
  COPY_FUNS(String_Matrix, taMatrixT<String>)
  TA_MATRIX_FUNS(String_Matrix, String)
  
public:
  override float	El_GetFloat_(const void* it) const	
    { return ((String*)it)->toFloat(); } // #IGNORE
  override const String	El_GetStr_(const void* it) const {return *((String*)it); } // #IGNORE
  override void		El_SetFmStr_(void* it, const String& str) {*((String*)it) = str;}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(*((String*)it));} // #IGNORE
  override void		El_SetFmVar_(void* it, const Variant& var) {*((String*)it) = var.toString(); };  // #IGNORE
protected:
  STATIC_CONST String	blank; // #IGNORE
  override void		Dump_Save_Item(ostream& strm, int idx);
  override int		Dump_Load_Item(istream& strm, int idx); 
  override void		ReclaimOrphans_(int from, int to); // called when elements can be reclaimed, ex. for strings

private:
  void		Initialize() {}
  void		Destroy() {}
};

//nn?? SmartPtr_Of(String_Matrix); // String_MatrixPtr


class TA_API float_Matrix: public taMatrixT<float> { // #INSTANCE #CAT_Data
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_float;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts valid format for float
    
  void			Copy_(const float_Matrix& cp) {}
  COPY_FUNS(float_Matrix, taMatrixT<float>)
  TA_MATRIX_FUNS(float_Matrix, float)
  
public:
  override float	El_GetFloat_(const void* it) const { return *((float*)it); } // #IGNORE
  override const String	El_GetStr_(const void* it) const { return (String)*((float*)it); } // #IGNORE
  override void		El_SetFmStr_(void* it, const String& str) {*((float*)it) = (float)str;}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(*((float*)it));} // #IGNORE
  override void		El_SetFmVar_(void* it, const Variant& var) {*((float*)it) = var.toFloat(); };  // #IGNORE
protected:
  STATIC_CONST float	blank; // #IGNORE
  override void		Dump_Save_Item(ostream& strm, int idx); // stream in full precision
private:
  void		Initialize() {}
  void		Destroy() {}
};

//nn?? SmartPtr_Of(float_Matrix)


class TA_API int_Matrix: public taMatrixT<int> { // #INSTANCE #CAT_Data
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_int;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts in-range for 32bit int
  
  void			Copy_(const int_Matrix& cp) {}
  COPY_FUNS(int_Matrix, taMatrixT<int>)
  TA_MATRIX_FUNS(int_Matrix, int)
  
public:
  override float	El_GetFloat_(const void* it) const { return (float)*((int*)it); } // #IGNORE
  override const String	El_GetStr_(const void* it) const { return *((int*)it); } // #IGNORE note: implicit conversion avoids problems on some compilers
  override void		El_SetFmStr_(void* it, const String& str) {*((int*)it) = (int)str;}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(*((int*)it));} // #IGNORE
  override void		El_SetFmVar_(void* it, const Variant& var) {*((int*)it) = var.toInt(); };  // #IGNORE
protected:
  STATIC_CONST int	blank; // #IGNORE
private:
  void		Initialize() {}
  void		Destroy() {} //
};

//nn? SmartPtr_Of(int_Matrix)


class TA_API byte_Matrix: public taMatrixT<byte> { // #INSTANCE #CAT_Data
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_unsigned_char;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts 0-255 or octal or hex forms
  
  void			Copy_(const byte_Matrix& cp) {}
  COPY_FUNS(byte_Matrix, taMatrixT<byte>)
  TA_MATRIX_FUNS(byte_Matrix, byte)
  
public: //
  override float	El_GetFloat_(const void* it) const { return (float)*((byte*)it); } // #IGNORE
  override const String	El_GetStr_(const void* it) const { return String(((int)*((byte*)it))); } // #IGNORE
  override void		El_SetFmStr_(void* it, const String& str) {*((byte*)it) = (byte)str.toInt();}       // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(*((byte*)it));} // #IGNORE
  override void		El_SetFmVar_(void* it, const Variant& var) {*((byte*)it) = var.toByte(); };  // #IGNORE
protected:
  STATIC_CONST byte	blank; // #IGNORE
private:
  void		Initialize() {}
  void		Destroy() {} //
};

//nn? SmartPtr_Of(byte_Matrix)


class TA_API Variant_Matrix: public taMatrixT<Variant> { // #INSTANCE #CAT_Data
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_Variant;} 
  
  void			Copy_(const Variant_Matrix& cp) {}
  COPY_FUNS(Variant_Matrix, taMatrixT<Variant>)
  TA_MATRIX_FUNS(Variant_Matrix, Variant) //
  
public:
  //NOTE: setString may not be exactly what is wanted -- that will change variant to String
  // what we may want is to set the current value as its type, from a string
  override float	El_GetFloat_(const void* it) const { return ((Variant*)it)->toFloat(); } // #IGNORE
  override const String	El_GetStr_(const void* it) const { return ((Variant*)it)->toString(); } // #IGNORE
  override void		El_SetFmStr_(void* it, const String& str) {((Variant*)it)->setString(str);}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return *((Variant*)it);} // #IGNORE
  override void		El_SetFmVar_(void* it, const Variant& var) {*((Variant*)it) = var; };  // #IGNORE
protected:
  STATIC_CONST Variant	blank; // #IGNORE
  override void		Dump_Save_Item(ostream& strm, int idx);
  override int		Dump_Load_Item(istream& strm, int idx); // ret is last char read, s/b ;
  override void		ReclaimOrphans_(int from, int to); // called when elements can be reclaimed, ex. for strings

private:
  void		Initialize() {}
  void		Destroy() {}
};

//nn? SmartPtr_Of(Variant_Matrix) //

class TA_API rgb_Matrix: public taMatrixT<rgb_t> { // #INSTANCE #CAT_Data
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_rgb_t;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts in form: "r g b" or RRGGBB in hex
  
  void			Copy_(const rgb_Matrix& cp) {}
  COPY_FUNS(rgb_Matrix, taMatrixT<rgb_t>)
  TA_MATRIX_FUNS(rgb_Matrix, rgb_t)
  
public: //
  //note: for streaming, we use web RGB hex value
  override const String	El_GetStr_(const void* it) const { return *((rgb_t*)it); } // #IGNORE implicit, to hex web format
  override void		El_SetFmStr_(void* it, const String& str) {((rgb_t*)it)->setString(str);}       // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(((rgb_t*)it)->toInt());} // #IGNORE we use the int rep for variants
  override void		El_SetFmVar_(void* it, const Variant& var) {((rgb_t*)it)->setInt(var.toInt()); };  // #IGNORE
protected:
  STATIC_CONST rgb_t	blank; // #IGNORE
private:
  void		Initialize() {}
  void		Destroy() {} //
};

//nn? SmartPtr_Of(byte_Matrix)

class TA_API MatrixTableModel: public QAbstractTableModel { // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for matrices; we extend it to support N-d, but only 2-d cell display
friend class taMatrix;
INHERITED(QAbstractTableModel)
public:
#ifndef __MAKETA__
  int			matIndex(const QModelIndex& idx) const; // #IGNORE flat matrix data index
#endif //note: bugs in maketa necessitated these sections
  taMatrix*		mat() const {return m_mat;}
  
  void			emit_dataChanged(int row_fr, int col_fr, int row_to, int col_to);
  void			emit_layoutChanged();
  
  MatrixTableModel(taMatrix* mat_); // note: mat is always valid, we destroy this on mat dest
  ~MatrixTableModel(); //
  
public: // required implementations
#ifndef __MAKETA__
  int 			columnCount(const QModelIndex& parent = QModelIndex()) const; // override
  QVariant 		data(const QModelIndex& index, int role = Qt::DisplayRole) const; // override
  Qt::ItemFlags 	flags(const QModelIndex& index) const; // override, for editing
  QVariant 		headerData(int section, Qt::Orientation orientation, 
    int role = Qt::DisplayRole) const; // override
  int 			rowCount(const QModelIndex& parent = QModelIndex()) const; // override
  bool 			setData(const QModelIndex& index, const QVariant& value, 
    int role = Qt::EditRole); // override, for editing

protected:
  static MatrixGeom	tgeom; // #IGNORE to avoid cost of allocation in index ops, we use this for non-reentrant
 
  void			MatrixDestroying(); // clears our instance
  bool			ValidateIndex(const QModelIndex& index) const;
  bool			ValidateTranslateIndex(const QModelIndex& index, MatrixGeom& tr_index) const;
    // translates index into matrix coords; true if the index is valid
  taMatrix*		m_mat;
#endif
};



#endif
