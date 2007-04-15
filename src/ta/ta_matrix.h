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

#include "ta_stdef.h"

#include "ta_variant.h"
#include "ta_base.h"
#include "ta_group.h"
#include "ta_TA_type.h"

#ifndef __MAKETA__
#  include <QAbstractTableModel>
#endif

#include <string.h>

// externals
class TypeDef;

// forwards this file
class taMatrix_PList;
class byte_Matrix; //
class float_Matrix; //
class MatrixTableModel; //

/* Matrix -- a specialized, richer implementation of Array

   Matrix is a ref-counted N-dimensional array of data, 1 <= N <= MAX_MATRIX_DIMS.
   
   End-user mats should only use up to N-1 dims, since 1 is reserved for 
   DataTable rows. Explicit accessors are only provided for 1:N-2
   
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

#define TA_MATRIX_DIMS_MAX 8
#define IMatrix taMatrix

class TA_API CellRange { // specifies a range of cells
public:
  int row_fr;
  int col_fr;
  int row_to;
  int col_to;

  bool		empty() const {return ((row_to < row_fr) || (col_to < col_fr));}
  inline bool	nonempty() const {return !empty();}
  int 		height() const {return row_to - row_fr + 1;}
  int 		width() const {return col_to - col_fr + 1;}
  bool		single() const {return ((row_to == row_fr) && (col_to == col_fr));}

  void		SetExtent(int wd, int ht); // set w/h based on current fr values
  void		LimitExtent(int wd, int ht); // limit width and height to these values
  void		LimitRange(int row_to_, int col_to_); 
   // limit range to within these values
  void 		Set(int row_fr_, int col_fr_, int row_to_, int col_to_)
    {row_fr = row_fr_; col_fr = col_fr_; row_to = row_to_; col_to = col_to_;}
  void 		SetFromModel(const QModelIndexList& indexes); //#IGNORE
  
  CellRange() {Set(0, 0, -1, -1);} // note that default is a null selection
  CellRange(int row_fr_, int col_fr_) // single cell
    {Set(row_fr_, col_fr_, row_fr_, col_fr_);}
  CellRange(int row_fr_, int col_fr_, int row_to_, int col_to_)
    {Set(row_fr_, col_fr_, row_to_, col_to_);}
   explicit CellRange(const QModelIndexList& indexes) 
     {SetFromModel(indexes);}
};


///////////////////////////////////
// 	Matrix Geometry
///////////////////////////////////

class TA_API MatrixGeom: public taBase  { 
  // matrix geometry, similar to an array of int
INHERITED(taBase)
friend class taMatrix;
friend class DataCol;
public:
  int			size; // DO NOT SET DIRECTLY, use SetSize
  
  bool			Equal(const MatrixGeom& other) const;
  inline bool		InRange(int idx) const {return ((idx >= 0) && (idx < size));}
  bool			IsFrameOf(const MatrixGeom& other) const; 
    // 'true' if this is a proper frame of other
  int 			Product() const; // returns product of all elements
  
  bool			SetSize(int sz); // sets to size, zeroing orphaned or new dims (true if changed size; false if not)
  int			SafeEl(int i) const {if (InRange(i)) return el[i]; else return 0;}
    // the element at the given index
  
  void			Add(int value); // safely add a new element
  void			Set(int i, int value) // safely set an element
    {if (InRange(i)) el[i] = value;}
  void			SetGeom(int dims, int d0, int d1=0, int d2=0,
    int d3=0, int d4=0, int d5=0, int d6=0);
    
  inline void		Reset() {SetSize(0);} // set size to 0, and clear all dims

  int 			IndexFmDims(const MatrixGeom& dims) const;
    // get index from dimension values, based on geometry
  int			IndexFmDims(int d0, int d1=0, int d2=0,
    int d3=0, int d4=0, int d5=0, int d6=0) const;
    // get index from dimension values, based on geometry
  void 			DimsFmIndex(int idx, MatrixGeom& dims) const;
  // get dimension values from index, based on geometry

  void			Get2DGeom(int& x, int& y) const; // for flat2d views
  void			Get2DGeomGui(int& x, int& y, bool odd_y, int spc) const;
  // for rendering routines, provides standardized 2d geom regardless of dimensionality (includes space for extra dimensions), odd_y = for odd dimension sizes, put extra dimension in y (else x): 3d = x, (y+1) * z (vertical time series of 2d patterns, +1=space), 4d = (x+1)*xx, (y+1)*yy (e.g., 2d groups of 2d patterns), 5d = vertical time series of 4d.
  
  String		GeomToString(const char* ldelim = "[", const char* rdelim = "]") const;
  // returns human-friendly text in form: "[dims:{dim}{,dim}]"
  void			GeomFromString(String& str, const char* ldelim = "[", const char* rdelim = "]");
  // reads geometry from string (consuming text) in form: "[dims:{dim}{,dim}]"

  override int		Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  override int		Dump_Load_Value(istream& strm, TAPtr par=NULL);
  void			Copy_(const MatrixGeom& cp);
  explicit MatrixGeom(int init_size);
  MatrixGeom(int dims, int d0, int d1=0, int d2=0, int d3=0, int d4=0, int d5=0, int d6=0);
  COPY_FUNS(MatrixGeom, taBase);
  TA_BASEFUNS_LITE(MatrixGeom); //

public: // functions for internal/trusted use only
  inline int		FastEl(int i) const {return el[i];} // #IGNORE
  inline int&		FastEl(int i) {return el[i];} // #IGNORE
    
  inline int&		operator [](int i) {return el[i];}  // #IGNORE 

protected:
  static MatrixGeom	td; // temp
  void	UpdateAfterEdit_impl();
  int			el[TA_MATRIX_DIMS_MAX];
  
  inline int		operator [](int i) const {return el[i];}  

  int 			IndexFmDims_(const int* d) const;
    // get index from dimension values, based on geometry

private:
  void			Initialize();
  void			Destroy();
};

inline bool operator ==(const MatrixGeom& a, const MatrixGeom& b)
  {return a.Equal(b);}
inline bool operator !=(const MatrixGeom& a, const MatrixGeom& b)
  {return !a.Equal(b);}


///////////////////////////////////
// 	Base taMatrix
///////////////////////////////////

class TA_API taMatrix: public taNBase {
  // #VIRT_BASE #NO_INSTANCE ##TOKENS #CAT_Data ref counted multi-dimensional data array
INHERITED(taNBase)
friend class MatrixTableModel;

public:
  ///////////////////////////////////////////////////////////////////
  // ITypedObject i/f
// TypeDef*		GetTypeDef() const; // in taBase
  void*			This() {return (void*)this;}

public: 
  ///////////////////////////////////////////////////////////////////
  // IMatrix i/f  

  //////////////////////////////////////////////////////////////////
  // Access functions
  
  inline int		count() const {return size;}
  // #CAT_Access the number of items
  inline int		dims() const {return geom.size;}
  // #CAT_Access the number of dimensions
  inline int		dim(int d) const {return geom.el[d];}
  // #CAT_Access the value of dimenion d -- MUST BE IN_RANGE
  int 			frames() const;
  // #CAT_Access number of frames currently in use (value of highest dimension) 
  int 			frameSize() const;
  // #CAT_Access number of elements in each frame (product of inner dimensions) 
  int			rowCount();
  // #CAT_Access flat row count, for 2-d grid operations, only 0 if empty
  int			FrameToRow(int f) const;
  // #CAT_Access convert frame number to row number
  int			FastElIndex(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const; 
  int			FastElIndex2D(int d0, int d1=0) const; 
  // #CAT_Access NO bounds check and return index as if the mat was only 2d -- YOU MUST ABSOLUTELY BE USING DIM-SAFE CODE
  int			FastElIndexN(const MatrixGeom& indices) const;
  // #CAT_Access NO bounds check and return flat index -- YOU MUST ABSOLUTELY BE USING DIM-SAFE CODE
  int			SafeElIndex(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const; 
  // #CAT_Access check bounds and return flat index, -1 if any dim out of bounds
  int			SafeElIndexN(const MatrixGeom& indices) const; 
  // #CAT_Access check bounds and return flat index, -1 if any dim out of bounds
  int			FrameStartIdx(int fm) const { return fm * frameSize(); }
  // #CAT_Access returns the flat base index of the specified frame
  
  virtual TypeDef*	GetDataTypeDef() const = 0;
  // #CAT_Access type of data, ex TA_int, TA_float, etc.
  virtual ValType	GetDataValType() const = 0;
  // #CAT_Access val_type of data
  void*   		GetTA_Element(int i, TypeDef*& eltd)
  { eltd = GetDataTypeDef(); if(InRange_Flat(i)) return (void*)FastEl_Flat_(i); return NULL; }

  ///////////////////////////////////////
  // String

  const String		SafeElAsStr(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const
  { return SafeElAsStr_Flat(SafeElIndex(d0, d1, d2, d3, d4)); }
  // #CAT_Access (safely) returns the element as a string
  const String		SafeElAsStrN(const MatrixGeom& indices) const
  { return SafeElAsStr_Flat(SafeElIndexN(indices)); }  
  // #CAT_Access (safely) returns the element as a string
  const String		SafeElAsStr_Flat(int idx) const	
  { if (InRange_Flat(idx)) return El_GetStr_(FastEl_Flat_(idx)); else return _nilString; } 
  // #CAT_Access treats the matrix like a flat array, returns the element as a string
  
  // universal string access/set, for flat array
  const String		FastElAsStr_Flat(int idx) const	
  { return El_GetStr_(FastEl_Flat_(idx)); } 
  // #CAT_Access  treats the matrix like a flat array, returns the element as a string

  void			SetFmStr_Flat(const String& str, int idx) 	
  { if (InRange_Flat(idx))  El_SetFmStr_(FastEl_Flat_(idx), str); } 
  // #CAT_Modify treats the matrix like a flat array, sets the element as a string
  
  // Clipboard Support (note: works well unless you have tabs/newlines in your data)
  
  const String		FlatRangeToTSV(int row_fr, int col_fr, int row_to, int col_to);
    // returns a tab-sep cols, newline-sep rows, well suited to clipboard; the coords are in flat 2-d form, as in the table editors
  const String		FlatRangeToTSV(const CellRange& cr) // #IGNORE
    {return FlatRangeToTSV(cr.row_fr, cr.col_fr, cr.row_to, cr.col_to);}
  
  ///////////////////////////////////////
  // Variant

  const Variant		SafeElAsVar(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const
  { return SafeElAsVar_Flat(SafeElIndex(d0, d1, d2, d3, d4)); } 
  // #CAT_Access (safely) returns the element as a variant
  const Variant		SafeElAsVarN(const MatrixGeom& indices) const	
  { return SafeElAsVar_Flat(SafeElIndexN(indices)); }   
  // #CAT_Access (safely) returns the element as a variant
  const Variant		SafeElAsVar_Flat(int idx) const	
  { if (InRange_Flat(idx)) return El_GetVar_(FastEl_Flat_(idx)); else return _nilVariant; } 
  // #CAT_Access treats the matrix like a flat array, returns the element as a variant

  const Variant		FastElAsVar_Flat(int idx) const
  { return El_GetVar_(FastEl_Flat_(idx)); } 
  // #CAT_Access treats the matrix like a flat array, returns the element as a variant
    
  void		SetFmVar(const Variant& var, int d0, int d1=0, int d2=0, int d3=0, int d4=0)
  { int idx; if ((idx = SafeElIndex(d0, d1, d2, d3, d4)) >= 0)
	       El_SetFmVar_(FastEl_Flat_(idx), var); } 
  // #CAT_Modify (safely) sets the element as a variant
  void		SetFmVarN(const Variant& var, const MatrixGeom& indices) 	
  { int idx; if ((idx = SafeElIndexN(indices)) >= 0)
	       El_SetFmVar_(FastEl_Flat_(idx), var); } 
  // #CAT_Modify (safely) sets the element as a variant
  void		SetFmVar_Flat(const Variant& var, int idx) 	
  { if (InRange_Flat(idx))  El_SetFmVar_(FastEl_Flat_(idx), var); } 
  // #CAT_Modify treats the matrix like a flat array, (safely) sets the element as a variant

  void		InitValsFmVar(const Variant& var)
  { for(int i=0;i<size;i++) El_SetFmVar_(FastEl_Flat_(i), var); }
  // #CAT_Modify initialize values to given fixed value

   	
public:
  ///////////////////////////////////////////////////////////////////
  // main matrix interface

  int 			size;
  // #SHOW #READ_ONLY number of elements in the matrix (= frames*frameSize)
  MatrixGeom		geom;
  // #SHOW #READ_ONLY dimensions array -- you cannot change this directly, you have to use API functions to change size
  
  static bool		GeomIsValid(int dims_, const int geom_[], String* err_msg = NULL);
  // #IGNORE validates proposed geom, ex. dims >=1, and valid values for supplied geoms
  static bool		GeomIsValid(const MatrixGeom& geom_, String* err_msg = NULL)
  { return GeomIsValid(geom_.size, geom_.el, err_msg); }
  // #IGNORE validates proposed geom, ex. dims >=1, and valid values for supplied geoms
  
  bool			canResize() const; 
  // #CAT_Modify true only if not fixed NOTE: may also include additional constraints, tbd
  bool			isFixedData() const { return alloc_size < 0; } 
  // #CAT_Modify true if using fixed (externally managed) data storage
  int			sliceCount() const;
  // #IGNORE number of extant slices

  virtual int		defAlignment() const;
  // #CAT_Display default Qt alignment, left for text, right for nums
    
  bool			InRange(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const; 
  // #CAT_Access true if indices in range; ignores irrelevant dims
  bool			InRangeN(const MatrixGeom& indices) const;  
  // #CAT_Access true if indices in range; MAY ignore under-supplied dims
  inline bool		InRange_Flat(int idx) const {return ((idx >= 0) && (idx < size));}
  // #CAT_Access checks if in actual range
  
  
  ///////////////////////////////////////
  // Float access

  float		SafeElAsFloat(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const
  { return SafeElAsFloat_Flat(SafeElIndex(d0, d1, d2, d3, d4)); } 
  // #CAT_Access (safely) returns the element as a float
  float		SafeElAsFloatN(const MatrixGeom& indices) const	
  { return SafeElAsFloat_Flat(SafeElIndexN(indices)); }   
  // #CAT_Access (safely) returns the element as a float
  float		SafeElAsFloat_Flat(int idx) const	
  { if (InRange_Flat(idx)) return El_GetFloat_(FastEl_Flat_(idx)); else return 0.0f; } 
  // #CAT_Access (safely) returns the element as a float, flat array model

  float		FastElAsFloat(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const
  { return El_GetFloat_(FastEl_Flat_(FastElIndex(d0, d1, d2, d3, d4))); }
  // #CAT_Access get element as float without range checking 
  float		FastElAsFloatN(const MatrixGeom& indices) const	
  { return El_GetFloat_(FastEl_Flat_(FastElIndexN(indices))); }   
  // #CAT_Access get element as float without range checking 
  float		FastElAsFloat_Flat(int idx) const 
  { return El_GetFloat_(FastEl_Flat_(idx)); } 
  // #CAT_Access get element as float without range checking, flat array model

  ///////////////////////////////////////
  // Double access

  double	SafeElAsDouble(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const
  { return SafeElAsDouble_Flat(SafeElIndex(d0, d1, d2, d3, d4)); } 
  // #CAT_Access (safely) returns the element as a double
  double	SafeElAsDoubleN(const MatrixGeom& indices) const	
  { return SafeElAsDouble_Flat(SafeElIndexN(indices)); }   
  // #CAT_Access (safely) returns the element as a double
  double	SafeElAsDouble_Flat(int idx) const	
  { if (InRange_Flat(idx)) return El_GetDouble_(FastEl_Flat_(idx)); else return 0.0f; } 
  // #CAT_Access (safely) returns the element as a double, float array model

  double	FastElAsDouble(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const
  { return El_GetDouble_(FastEl_Flat_(FastElIndex(d0, d1, d2, d3, d4))); }
  // #CAT_Access get element as double without range checking 
  double	FastElAsDoubleN(const MatrixGeom& indices) const	
  { return El_GetDouble_(FastEl_Flat_(FastElIndexN(indices))); }   
  // #CAT_Access get element as double without range checking 
  double	FastElAsDouble_Flat(int idx) const 
  { return El_GetDouble_(FastEl_Flat_(idx)); } 
  // #CAT_Access get element as double without range checking, flat array model

  ///////////////////////////////////////
  // alloc management

  bool			AddFrame() {return AddFrames(1);}
  // #MENU #MENU_ON_Matrix #MENU_CONTEXT #CAT_Modify add 1 new blank frame
  virtual bool		AddFrames(int n);
  // #MENU #MENU_ON_Matrix #MENU_CONTEXT #CAT_Modify add n new blank frames -- note that this assumes incremental growth and thus calls AllocFrames in advance
  virtual bool		AllocFrames(int n);
  // #CAT_Modify make sure space exists for n frames: calling this is optional, and is typically done if you know ahead of time how much space you need 
  virtual bool		EnforceFrames(int n, bool notify = true); 
  // #MENU #MENU_ON_Object #ARGC_1 #CAT_Modify set size to n frames, blanking new elements if added; resizes by 1.5x as needed 
  virtual bool		RemoveFrames(int st_fr, int n_frames=1);
  // #MENU #MENU_ON_Matrix #CAT_Modify remove the given number of frames at starting index, copying data backwards if needed.  st_fr = -1 means last frame, and n_frames = -1 means all frames from start to end
  virtual bool		InsertFrames(int st_fr, int n_frames=1);
  // #MENU #CAT_Modify insert n new frames at given frame index, shifting existing data down to make room.  st_fr = -1 means append to end
  virtual bool		CopyFrame(const taMatrix& src, int frame);
  // #CAT_Copy copy the source matrix to the indicated frame; src geom must be our frame geom; optimized for like-type mats

  virtual void		Reset();
  // #CAT_Modify remove all items
  
  void			SetGeom(int size, int d0, int d1=0, int d2=0, int d3=0, int d4=0)  
  { int d[5]; d[0]=d0; d[1]=d1; d[2]=d2; d[3]=d3; d[4]=d4; SetGeom_(size, d);} 
  // #CAT_Modify set geom for matrix -- if matches current size, it is non-destructive 
  void			SetGeomN(const MatrixGeom& geom_) 
  { SetGeom_(geom_.size, geom_.el);}
  // #MENU #MENU_ON_Matrix #MENU_SEP_BEFORE #CAT_Modify set geom for any sized matrix -- if matches current size, it is non-destructive 
  
  // Slicing -- NOTES: 
  // 1. slices are updated if parent allocation changes -- this could collapse slice to [0]
  // 2. slices are not guaranteed to be unique (i.e. same spec may return same slice ref)
  // 3a. you *must* ref/unref the slice
  // 3b. a slice refs its parent; unrefs it on destroy
  // 4. you may only request "proper slices", i.e., full dimensional subsets
  // 5. if parent is redimensioned, all slices are collapsed to [0]
  
  virtual taMatrix*	GetSlice_(const MatrixGeom& base, 
    int slice_frame_dims = -1, int num_slice_frames = 1);
  // #CAT_Access return a slice, sfd=-1 indicates a frame size slice; base should be a subframe boundary
  virtual taMatrix*	GetFrameSlice_(int frame);
  // #CAT_Access return a slice, of exactly one frame; will have dim-1 of us
  virtual taMatrix*	GetFrameRangeSlice_(int st_frame, int n_frames);
  // #CAT_Access return a slice, of n_frames frames starting at st_frame
  
#ifdef TA_GUI
  MatrixTableModel*	GetDataModel(); // #IGNORE returns new if none exists, or existing -- enables views to be shared
#endif
  
  virtual void 		List(ostream& strm=cout) const; 	// List the items
  
  virtual bool		StrValIsValid(const String& str, String* err_msg = NULL) const
  { return true; }
  // #IGNORE validates a proposed string-version of a value, ex. float_Matrix can verify valid floating rep of string 

  void			SetDefaultName() { };
  ostream& 		Output(ostream& strm, int indent = 0) const;
  ostream& 		OutputR(ostream& strm, int indent = 0) const
    { return Output(strm, indent); }
  int			Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  int			Dump_Load_Value(istream& strm, TAPtr par=NULL);
  COPY_FUNS(taMatrix, taNBase);
  TA_ABSTRACT_BASEFUNS(taMatrix) //

public:
  ///////////////////////////////////////////////////////////////////
  // low-level interface, try not to use these, internal use only

  virtual void*		data() const = 0;
  // #IGNORE pointer to the start of the raw data
  virtual void*		FastEl_Flat_(int i) = 0;
  // #IGNORE the raw element in the flat space _
  virtual const void*	FastEl_Flat_(int i) const = 0;
  // #IGNORE const version
  const void*		FastEl_(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const 
  { return FastEl_Flat_(FastElIndex(d0, d1, d2, d3, d4)); } 
  // #IGNORE the raw element in index space -- YOU MUST ABSOLUTELY BE USING DIM-SAFE CODE
  const void*		FastElN_(const MatrixGeom& indices) const 
  { return FastEl_Flat_(FastElIndexN(indices));} 
  // #IGNORE 
  
  virtual const void*	SafeEl_(int i) const 
  {if ((i >= 0) && (i < size)) return FastEl_Flat_(i); else return El_GetBlank_(); }
  // #IGNORE raw element in flat space, else NULL

  // every subclass should implement these:
  virtual float		El_GetFloat_(const void*) const	{ return 0.0f; }
  // #IGNORE
  virtual double	El_GetDouble_(const void* it) const  { return El_GetFloat_(it); }
  // #IGNORE
  virtual const String	El_GetStr_(const void*) const	{ return _nilString; }
  // #IGNORE
  virtual void		El_SetFmStr_(void*, const String&) { };
  // #IGNORE
  virtual const Variant	El_GetVar_(const void*) const	{ return _nilVariant; }
  // #IGNORE
  virtual void		El_SetFmVar_(void*, const Variant&) { };
  // #IGNORE
 
protected:
  void			UpdateAfterEdit_impl(); 
  static void		SliceInitialize(taMatrix* par_slice, taMatrix* child_slice); 
   // called after slice created -- static for consistency
  static void		SliceDestroying(taMatrix* par_slice, taMatrix* child_slice); 
   //called by child slice on destroy -- static because it can cause destruction
  
  int			alloc_size; // -1 means fixed (external data)
  taMatrix_PList*	slices; // list of extant slices -- created on first slice
  taMatrix*		slice_par; // slice parent -- we ref/unref it
  MatrixTableModel*	m_dm; // #IGNORE instance of dm; persists once created
  
  virtual bool		fastAlloc() const {return true;}
  // #IGNORE enables using fast block-based allocations, copies, and skipping reclaims -- for ints,floats, etc.; not for Strings/Variants
  virtual void		SetGeom_(int dims_, const int geom_[]); //
  
  virtual bool		Alloc_(int new_alloc);
  // set capacity to n -- should always be in multiples of frames 
  virtual void*		MakeArray_(int i) const = 0;
  // #IGNORE make a new array of item type
  virtual void		SetArray_(void* nw) = 0;
  virtual void*		FastRealloc_(int i) = 0;
  // #IGNORE reallocate existing array, only valid for fastAlloc() objects
  virtual void		ReclaimOrphans_(int from, int to) {}
  // called when elements can be reclaimed, ex. for strings
  
  virtual void		Add_(const void* it);
  // compatibility function -- only valid if dims=1
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
  
  virtual void		SetFixedData_(void* el_, const MatrixGeom& geom_);
  // initialize fixed data
//  virtual bool		Equal_(const taMatrix& src) const; 
    // 'true' if same size and els
    
  void 			Slice_Collapse();
  void			Slice_Realloc(ta_intptr_t base_delta);
  
  virtual void		UpdateGeom();
  // called to potentially update the allocation based on new geom info -- will fix if in error
  void			UpdateSlices_Collapse();
  // collapses all the slices to []
  void			UpdateSlices_FramesDeleted(void* deletion_base, int num);
  // called when deleting a frame -- for each slice: update base addr if after delete base, or collapse if doesn't exist any more
  void			UpdateSlices_Realloc(ta_intptr_t base_delta);
  // called when allocing new mem (more or less) -- for each slice: update base addr; note: not for use if size has changed (FramesDeleted would be called)
    
  virtual void		Dump_Save_Item(ostream& strm, int idx); 
  // dump the value, term with ; generic is fine for numbers, override for strings, variants, etc.
  virtual int		Dump_Load_Item(istream& strm, int idx); 
  // load the ;-term'ed value ; generic is fine for numbers, override for strings, variants, etc.; ret is last char read, usually ;

private:
  void			Copy_(const taMatrix& cp);
  void 			Initialize();
  void			Destroy();
};

typedef taMatrix* ptaMatrix_impl;
SmartPtr_Of(taMatrix); // taMatrixPtr


class TA_API taMatrix_PList: public taPtrList<taMatrix> {
  // simple list for keeping track of slices
INHERITED(taPtrList<taMatrix>)
public:

  taMatrix_PList() {}
};


class TA_API taMatrix_Group: public taGroup<taMatrix> {
  // ##CAT_Data group that can hold matrix items -- typically used for dataset elements
INHERITED(taGroup<taMatrix>)
public:

  TA_BASEFUNS_NCOPY(taMatrix_Group);
private:
  void		Initialize() {SetBaseType(&TA_taMatrix);}
  void		Destroy() {}
};

///////////////////////////////////
// 	Matrix Template
///////////////////////////////////

template<class T> 
class taMatrixT : public taMatrix {
  // #VIRT_BASE #NO_INSTANCE ##CAT_Data 
  INHERITED(taMatrix)
public:
  T*		el;		// #HIDDEN #NO_SAVE Pointer to actual array memory

  override void*	data() const {return el;} // #IGNORE
  
  void			SetFixedData(T* data_, const MatrixGeom& geom_)
  { SetFixedData_(data_, geom_); } 
  // #IGNORE sets external (fixed) data, setting the geom/size
  
  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  const T&		FastEl(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const
  { return el[FastElIndex(d0,d1,d2,d3,d4)]; }
  const T&		FastElN(const MatrixGeom& indices) const
  { return el[FastElIndexN(indices)]; } 
  const T&		FastEl_Flat(int idx) const { return el[idx]; }
  
  T&			FastEl(int d0, int d1=0, int d2=0, int d3=0, int d4=0)
  { return el[FastElIndex(d0, d1, d2, d3, d4)]; }
  // #CAT_Access get element without range checking
  T&			FastElN(const MatrixGeom& indices)
  { return el[FastElIndexN(indices)]; }
  // #CAT_Access get element without range checking
  T&			FastEl_Flat(int idx) { return el[idx]; }
  // #CAT_Access get element without range checking -- treats matrix like a flat array
  
  const T&		SafeEl(int d0, int d1=0, int d2=0, int d3=0, int d4=0) const 	
  { return *((T*)(SafeEl_(SafeElIndex(d0,d1,d2,d3,d4)))); } 
  // #CAT_Access (safely) access the element for reading
  const T&		SafeElN(const MatrixGeom& indices) const  
  { return *((T*)(SafeEl_(SafeElIndexN(indices)))); }  
  // #CAT_Access (safely) access the element for reading
  const T&		SafeEl_Flat(int idx) const
  { return *((T*)(SafeEl_(idx))); }
  // #CAT_Access (safely) access the matrix as if it were a flat vector, for reading
  
  void			Set(const T& item, int d0, int d1=0, int d2=0, int d3=0, int d4=0)
  { int idx = SafeElIndex(d0,d1,d2,d3,d4); 
    if (InRange_Flat(idx)) el[idx] = item; }
  // #CAT_Modify safely assign values to items in the matrix
  void			SetN(const T& item, const MatrixGeom& indices) 	
  {  int idx = SafeElIndexN(indices); 
    if (InRange_Flat(idx)) el[idx] = item; }
  // #CAT_Modify safely assign values to items in the matrix
  void			Set_Flat(const T& item, int idx) 	
  { if (InRange_Flat(idx)) el[idx] = item; }
  // #CAT_Modify safely assign values to items in the matrix, treated as a flat vector
  
  // compatibility functions, for when dims=1
  void			Add(const T& item) {Add_(&item);}
  // #CAT_Modify only valid when dims=1

  void	CutLinks() 	{SetArray_(NULL); taMatrix::CutLinks();}
  TA_TMPLT_ABSTRACT_BASEFUNS(taMatrixT, T)
public:
  override void*	FastEl_Flat_(int idx)	{ return &(el[idx]); } 
  override const void*	FastEl_Flat_(int idx) const { return &(el[idx]); } 
protected:
  override void*	MakeArray_(int n) const	
  { if(fastAlloc()) return malloc(n * sizeof(T)); else return new T[n]; }
  override void		SetArray_(void* nw) 
  { if (el && (alloc_size > 0)) { if(fastAlloc()) free(el); else delete [] el; }
    el = (T*)nw; }
  override void*	FastRealloc_(int n)
  { el = (T*)realloc((char*)el, n * sizeof(T)); return el; }
  override bool		El_Equal_(const void* a, const void* b) const
    { return (*((T*)a) == *((T*)b)); }
  override void		El_Copy_(void* to, const void* fm) {*((T*)to) = *((T*)fm); }
  override uint		El_SizeOf_() const	{ return sizeof(T); }

private: 
  TMPLT_NCOPY(taMatrixT, T)
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
  y* GetFrameRangeSlice(int frame, int n_frames) {return (y*) GetFrameRangeSlice_(frame, n_frames);} \
  y(int dims_, int d0, int d1=0, int d2=0, int d3=0, int d4=0) \
    {SetGeom(dims_, d0,d1,d2,d3,d4);} \
  explicit y(const MatrixGeom& geom_) {SetGeomN(geom_);} \
  y(T* data_, const MatrixGeom& geom_) {SetFixedData(data_, geom_);} \
  TA_BASEFUNS(y) \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; }

#define MAT_COPY_SAME_SLOW(y,T) \
  void	Copy_(const y& cp) { SetGeomN(cp.geom); \
    for (int i = 0; i < size; ++i) { \
      El_Copy_(FastEl_Flat_(i), cp.FastEl_Flat_(i)); \
    }}

#define MAT_COPY_SAME_FAST(y,T) \
  void	Copy_(const y& cp) { SetGeomN(cp.geom); \
    memcpy(data(), cp.data(), size * sizeof(T)); \
    }
    
    
#define TA_MATRIX_FUNS_FAST(y,T) \
private: \
  MAT_COPY_SAME_FAST(y,T) \
protected: \
  override bool	fastAlloc() const {return true;} \
public: \
  TA_MATRIX_FUNS(y,T)
  
#define TA_MATRIX_FUNS_SLOW(y,T) \
private: \
  MAT_COPY_SAME_SLOW(y,T) \
protected: \
  override bool	fastAlloc() const {return false;} \
public: \
  TA_MATRIX_FUNS(y,T)
  
  
///////////////////////////////////
// 	String_Matrix
///////////////////////////////////

class TA_API String_Matrix: public taMatrixT<String> {
  // #INSTANCE a matrix of strings
INHERITED(taMatrixT<String>)
public:
  override int		defAlignment() const;
  override TypeDef*	GetDataTypeDef() const {return &TA_taString;} 
  override ValType	GetDataValType() const {return VT_STRING;} 
  
  COPY_FUNS(String_Matrix, taMatrixT<String>)
  TA_MATRIX_FUNS_SLOW(String_Matrix, String)
  
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


///////////////////////////////////
// 	float_Matrix
///////////////////////////////////

class TA_API float_Matrix: public taMatrixT<float> {
  // #INSTANCE a matrix of floats
INHERITED(taMatrixT<float>)
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_float;} 
  override ValType	GetDataValType() const {return VT_FLOAT;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts valid format for float

  virtual void		InitVals(float val=0.0) { for(int i=0;i<size;i++) FastEl_Flat(i) = val; }
  // initialize values to given fixed value
    
  TA_MATRIX_FUNS_FAST(float_Matrix, float);
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

///////////////////////////////////
// 	double_Matrix
///////////////////////////////////

class TA_API double_Matrix: public taMatrixT<double> {
  // #INSTANCE a matrix of doubles
INHERITED(taMatrixT<double>)
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_double;} 
  override ValType	GetDataValType() const {return VT_DOUBLE;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts valid format for double
    
  virtual void		InitVals(double val=0.0) { for(int i=0;i<size;i++) FastEl_Flat(i) = val; }
  // initialize values to given fixed value
    
  TA_MATRIX_FUNS_FAST(double_Matrix, double);
  
public:
  override float	El_GetFloat_(const void* it) const 
    { return (float)*((double*)it); } 
    // #IGNORE warning: loss of precision and/or under/overflow possible
  override double	El_GetDouble_(const void* it) const 
    { return *((double*)it); } // #IGNORE
  override const String	El_GetStr_(const void* it) const 
    { return (String)*((double*)it); } // #IGNORE
  override void		El_SetFmStr_(void* it, const String& str) 
    {*((double*)it) = (double)str;}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const 
    {return Variant(*((double*)it));} // #IGNORE
  override void		El_SetFmVar_(void* it, const Variant& var) 
    {*((double*)it) = var.toDouble(); };  // #IGNORE
protected:
  STATIC_CONST double	blank; // #IGNORE
  override void		Dump_Save_Item(ostream& strm, int idx); // stream in full precision
private:
  void		Initialize() {}
  void		Destroy() {}
};

//nn?? SmartPtr_Of(double_Matrix)

///////////////////////////////////
// 	int_Matrix
///////////////////////////////////

class TA_API int_Matrix: public taMatrixT<int> {
  // #INSTANCE a matrix of ints
INHERITED(taMatrixT<int>)
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_int;} 
  override ValType	GetDataValType() const {return VT_INT;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts in-range for 32bit int
  
  virtual void		InitVals(int val=0) { for(int i=0;i<size;i++) FastEl_Flat(i) = val; }
  // initialize values to given fixed value

  TA_MATRIX_FUNS_FAST(int_Matrix, int);
  
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

///////////////////////////////////
// 	byte_Matrix
///////////////////////////////////

class TA_API byte_Matrix: public taMatrixT<byte> {
  // #INSTANCE a matrix of bytes
INHERITED(taMatrixT<byte>)
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_unsigned_char;} 
  override ValType	GetDataValType() const {return VT_BYTE;} 
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts 0-255 or octal or hex forms
  
  TA_MATRIX_FUNS_FAST(byte_Matrix, byte);
  
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

///////////////////////////////////
// 	Variant_Matrix
///////////////////////////////////

class TA_API Variant_Matrix: public taMatrixT<Variant> {
  // #INSTANCE a matrix of variants
INHERITED(taMatrixT<Variant>)
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_Variant;} 
  override ValType	GetDataValType() const {return VT_VARIANT;} 
  
  TA_MATRIX_FUNS_SLOW(Variant_Matrix, Variant);
  
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

///////////////////////////////////
// 	rgb_Matrix
///////////////////////////////////

class TA_API rgb_Matrix: public taMatrixT<rgb_t> {
  // #INSTANCE a matrix of rgb values
INHERITED(taMatrixT<rgb_t>)
public:
  override TypeDef*	GetDataTypeDef() const {return &TA_rgb_t;} 
  override ValType	GetDataValType() const {return VT_INT;} // note: not quite right.
  
  override bool		StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts in form: "r g b" or RRGGBB in hex
  
  TA_MATRIX_FUNS_FAST(rgb_Matrix, rgb_t);
  
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

class TA_API MatrixTableModel: public QAbstractTableModel,
  public IDataLinkClient
{
  // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for matrices; we extend it to support N-d, but only 2-d cell display
friend class taMatrix;
INHERITED(QAbstractTableModel)
  Q_OBJECT
public:
#ifndef __MAKETA__
  int			matIndex(const QModelIndex& idx) const; // #IGNORE flat matrix data index
  override QMimeData* 	mimeData (const QModelIndexList& indexes) const;
  override QStringList	mimeTypes () const;
  taMisc::MatrixView 	matView() const;
#endif //note: bugs in maketa necessitated these sections
  taMatrix*		mat() const {return m_mat;}
  
  void			emit_dataChanged(int row_fr = 0, int col_fr = 0,
    int row_to = -1, int col_to = -1);// can be called w/o params to issue global change (for manual refresh)
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

public: // IDataLinkClient i/f
  override void*	This() {return this;}
  override TypeDef*	GetTypeDef() const {return &TA_MatrixTableModel;}
  override void		DataLinkDestroying(taDataLink* dl);
  override void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2); 
  
protected:
  static MatrixGeom	tgeom; // #IGNORE to avoid cost of allocation in index ops, we use this for non-reentrant
 
  taMatrix*		m_mat;
  taMisc::MatrixView	m_view_layout; //#IGNORE #DEF_TOP_ZERO
  
  bool			ValidateIndex(const QModelIndex& index) const;
  bool			ValidateTranslateIndex(const QModelIndex& index, MatrixGeom& tr_index) const;
    // translates index into matrix coords; true if the index is valid
#endif
};



#endif
