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

#ifndef taMatrix_h
#define taMatrix_h 1

// parent includes:
#include <taNBase>

// for smart ptrs need these:
#include <taSmartPtrT>
#include <taSmartRefT>

// member includes:
#include <MatrixGeom>
#include <BuiltinTypeDefs>

// declare all other types mentioned but not required to include:
class int_Matrix; // 
class MatrixIndex; // 
class iMatrixTableModel; // #IGNORE
class CellRange; //
class taMatrix_PList; //

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

     Xxx(d0, d1, d2, d3, d4, d5, d6) this is the most common way to access the data --
       d's higher than the actual dimension of the matrix are ignored

     XxxN(const MatrixIndex&) -- for any dimensionality -- it is unspecified whether
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
       * the standard taBase siglink-based notification
       * Qt AbstractItemModel notifications

     Changes to data do *not* automatically cause data notifications (this would
     add an unacceptable penalty to most code.) However, data changes that
     are mediated by the Qt model do, so other grid views will automatically stay
     updated.

     We also use Struct/Data Begin/End to communicate changes. When a mat has
     slices, we recursively propogate those notifies to the slices. Note that
     this are almost invariably gui-driven, so don't entail overhead for low-level
     data processing. But if you want your low-level updates to cause gui changes,
     then you must wrap them in a Begin/End block.

   Matrix vs. Array
     'Array' classes are typically 1-d vectors or interpreted as 2-d arrays.
     Array supports dynamic operations, like inserting, sorting, etc.
     Matrix is ref-counted, and intended for sharing/moving raw data around.
     Matrix explicitly supports dimensionality and dimensional access.
     Matrix supports advanced tabular editing operations.
     Matrix forms the basis for most pdp4 data processing constructs.

*/

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

typedef void (*fixed_dealloc_fun)(void*); // function that deallocates fixed data


taTypeDef_Of(taMatrix);

class TA_API taMatrix: public taNBase {
  // #VIRT_BASE #STEM_BASE #NO_INSTANCE ##TOKENS #CAT_Data ref counted multi-dimensional data array
INHERITED(taNBase)

public:
  ///////////////////////////////////////////////////////////////////
  // ITypedObject i/f
// TypeDef*             GetTypeDef() const; // in taBase
  void*                 This() {return (void*)this;}

public:
   enum RenderOp {              // operation for rendering one matrix into another
     COPY,                      // dest = source
     ADD,                       // dest += source
     SUB,                       // dest -= source
     MULT,                      // dest *= source
     DIV,                       // dest /= source
     MAX,                       // dest = MAX(dest, source)
     MIN,                       // dest = MIN(dest, source)
   };

  taBasePtr     el_view;        // #EXPERT #NO_SAVE #CAT_taMatrix matrix with indicies providing view into items in this list, if set -- determines the items and the order in which they are presented for the iteration operations -- otherwise ignored in other contexts
  IndexMode     el_view_mode;   // #EXPERT #NO_SAVE #CAT_taMatrix what kind of information is present in el_view to determine view mode -- only valid cases are IDX_COORDS and IDX_MASK

  ///////////////////////////////////////////////////////////////////
  // IMatrix i/f

  //////////////////////////////////////////////////////////////////
  // Access functions

  inline int            colCount(bool pat_4d = false) const
  { return geom.colCount(pat_4d); }
  // #CAT_Access flat col count, for 2-d grid operations, never 0; by2 puts d0/d1 in same row when dims>=4
  inline int            count() const {return size;}
  // #CAT_Access the number of items
  inline int            dims() const {return geom.dims(); }
  // #CAT_Access the number of dimensions
  inline int            dim(int d) const {return geom.dim(d);}
  // #CAT_Access the value of dimenion d
  inline int_Matrix*    Shape() const { return (int_Matrix*)geom; }
  // #CAT_Access the shape of the matrix -- returns an int matrix with values for the size of each dimension
  int                   frames() const;
  // #CAT_Access number of frames currently in use (value of highest dimension)
  int                   frameSize() const;
  // #CAT_Access number of elements in each frame (product of inner dimensions)
  int                   rowCount(bool pat_4d = false) const
    {return geom.rowCount(pat_4d);}
  // #CAT_Access flat row count, for 2-d grid operations, never 0; by2 puts d0/d1 in same row when dims>=4
  int                   FrameToRow(int f) const;
  // #CAT_Access convert frame number to row number
  inline int            FastElIndex(int d0, int d1=0, int d2=0, int d3=0,
                                    int d4=0, int d5=0, int d6=0) const
  { return geom.IndexFmDims(d0, d1, d2, d3, d4, d5, d6); }
  inline int            FastElIndex2D(int d0, int d1=0) const
  { return (d1 * geom[0]) + d0; }
  // #CAT_Access NO bounds check and return index as if the mat was only 2d -- YOU MUST ABSOLUTELY BE USING DIM-SAFE CODE
  inline int            FastElIndexN(const MatrixIndex& indices) const
  { return geom.IndexFmDimsN(indices); }
  // #CAT_Access NO bounds check and return flat index -- YOU MUST ABSOLUTELY BE USING DIM-SAFE CODE
  inline int            SafeElIndex(int d0, int d1=0, int d2=0, int d3=0,
                                    int d4=0, int d5=0, int d6=0) const
  { return geom.SafeIndexFmDims(d0, d1, d2, d3, d4, d5, d6); }
  // #CAT_Access check bounds and return flat index, -1 if any dim out of bounds
  inline int            SafeElIndexN(const MatrixIndex& indices) const
  { return geom.SafeIndexFmDimsN(indices); }
  // #CAT_Access check bounds and return flat index, -1 if any dim out of bounds
  int                   FrameStartIdx(int fm) const { return fm * frameSize(); }
  // #CAT_Access returns the flat base index of the specified frame

  virtual TypeDef*      GetDataTypeDef() const = 0;
  // #CAT_Access type of data, ex TA_int, TA_float, etc.
  virtual ValType       GetDataValType() const = 0;
  // #CAT_Access val_type of data

  ///////////////////////////////////////
  // Standard Elem and iterator interface

  override bool         IsContainer()   { return true; }
  override taMatrix*    ElView() const  { return (taMatrix*)el_view.ptr(); }
  override IndexMode    ElViewMode() const  { return el_view_mode; }
  override int          ElemCount() const { return size; }
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const;
  override bool         IterFirst_impl(taBaseItr*& itr) const;
  override bool         IterNext_impl(taBaseItr*& itr) const;
  override Variant      IterElem(taBaseItr* itr) const;
  override taBaseItr*   Iter() const;
  override bool         IterValidate(taMatrix* vmat, IndexMode mode, int cont_dims) const;
  virtual bool          SetElView(taMatrix* view_mat, IndexMode md = IDX_COORDS);
  // #CAT_Access #EXPERT set el view to given new case -- just sets the members
  virtual taMatrix*     NewElView(taMatrix* view_mat, IndexMode md = IDX_COORDS) const;
  // #CAT_Access #EXPERT make a new view of this array -- points to items in original
  virtual Variant       ElemFmCoord(int_Matrix* cmat) const;
  // #CAT_Access return element(s) from coordinate matrix that specifies one coordinate -- if full set of coords are specified, then it returns a single element, otherwise it returns a slice of the matrix corresponding to the subset of coordinates present

  ///////////////////////////////////////
  // String

  const String          SafeElAsStr(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const
  { return SafeElAsStr_Flat(SafeElIndex(d0, d1, d2, d3, d4, d5, d6)); }
  // #CAT_Access (safely) returns the element as a string
  const String          SafeElAsStrN(const MatrixIndex& indices) const
  { return SafeElAsStr_Flat(SafeElIndexN(indices)); }
  // #CAT_Access (safely) returns the element as a string
  const String          SafeElAsStr_Flat(int idx) const
  { if (InRange_Flat(idx)) return El_GetStr_(FastEl_Flat_(idx)); else return _nilString; }
  // #CAT_Access treats the matrix like a flat array, returns the element as a string

  // universal string access/set, for flat array
  const String          FastElAsStr_Flat(int idx) const
  { return El_GetStr_(FastEl_Flat_(idx)); }
  // #CAT_Access  treats the matrix like a flat array, returns the element as a string

  void                  SetFmStr_Flat(const String& str, int idx)
  { if (InRange_Flat(idx))  El_SetFmStr_(FastEl_Flat_(idx), str); }
  // #CAT_Modify treats the matrix like a flat array, sets the element as a string

  // Clipboard Support (note: works well unless you have tabs/newlines in your data)

  const String          FlatRangeToTSV(int row_fr, int col_fr, int row_to, int col_to);
  // returns a tab-sep cols, newline-sep rows, well suited to clipboard; the coords are in flat 2-d form, as in the table editors
  const String          FlatRangeToTSV(const CellRange& cr);
  // #IGNORE returns a tab-sep cols, newline-sep rows, well suited to clipboard; the coords are in flat 2-d form, as in the table editors

  ///////////////////////////////////////
  // Variant

  const Variant         SafeElAsVar(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const
  { return SafeElAsVar_Flat(SafeElIndex(d0, d1, d2, d3, d4, d5, d6)); }
  // #CAT_Access (safely) returns the element as a variant
  const Variant         SafeElAsVarN(const MatrixIndex& indices) const
  { return SafeElAsVar_Flat(SafeElIndexN(indices)); }
  // #CAT_Access (safely) returns the element as a variant
  const Variant         SafeElAsVar_Flat(int idx) const
  { if (InRange_Flat(idx)) return El_GetVar_(FastEl_Flat_(idx)); else return _nilVariant; }
  // #CAT_Access treats the matrix like a flat array, returns the element as a variant

  const Variant         FastElAsVar_Flat(int idx) const
  { return El_GetVar_(FastEl_Flat_(idx)); }
  // #CAT_Access treats the matrix like a flat array, returns the element as a variant

  int           FindVal_Flat(const Variant& val, int st_idx = 0) const;
  // #CAT_Access find a value in the matrix -- treats the matrix like a flat array, returns the flat index where the value was found, or -1 if not found

  void          SetFmVar(const Variant& var, int d0, int d1=0, int d2=0,
    int d3=0, int d4=0, int d5=0, int d6=0)
  {
    int idx = SafeElIndex(d0, d1, d2, d3, d4, d5, d6);
    if (idx >= 0) {
      El_SetFmVar_(FastEl_Flat_(idx), var);
    }
  }
  // #CAT_Modify (safely) sets the element as a variant

  void          SetFmVarN(const Variant& var, const MatrixIndex& indices)
  {
    int idx = SafeElIndexN(indices);
    if (idx >= 0) {
      El_SetFmVar_(FastEl_Flat_(idx), var);
    }
  }
  // #CAT_Modify (safely) sets the element as a variant

  void          SetFmVar_Flat(const Variant& var, int idx)
  {
    if (InRange_Flat(idx)) {
      El_SetFmVar_(FastEl_Flat_(idx), var);
    }
  }
  // #CAT_Modify treats the matrix like a flat array, (safely) sets the element as a variant

  void          InitValsFmVar(const Variant& var)
  {
    for (int i = 0; i < size; i++) {
      El_SetFmVar_(FastEl_Flat_(i), var);
    }
  }
  // #CAT_Modify initialize values to given fixed value


public:
  ///////////////////////////////////////////////////////////////////
  // main matrix interface

  int                   size;
  // #SHOW #READ_ONLY #NO_SAVE number of elements in the matrix (= frames*frameSize)
  MatrixGeom            geom;
  // #SHOW #READ_ONLY #NO_SAVE dimensions array -- you cannot change this directly, you have to use API functions to change size

  static bool           GeomIsValid(int dims_, const int geom_[],
    String* err_msg = NULL, bool allow_flex = true);
  // #IGNORE validates proposed geom, ex. dims >=1, and valid values for supplied geoms
  static bool           GeomIsValid(const MatrixGeom& geom_,
    String* err_msg = NULL, bool allow_flex = true)
  { return GeomIsValid(geom_.dims(), geom_.el, err_msg, allow_flex); }
  // #IGNORE validates proposed geom, ex. dims >=1, and valid values for supplied geoms

  bool                  canResize() const;
  // #CAT_Modify true only if not fixed NOTE: may also include additional constraints, tbd
  bool                  isFixedData() const { return alloc_size < 0; }
  // #CAT_Modify true if using fixed (externally managed) data storage
  int                   sliceCount() const;
  // #IGNORE number of extant slices
  taMatrix*             slicePar() const {return slice_par;} // if exists; rarely used

  virtual int           defAlignment() const;
  // #CAT_Display default Qt alignment, left for text, right for nums

  bool                  InRange(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const;
  // #CAT_Access true if indices in range; ignores irrelevant dims
  bool                  InRangeN(const MatrixIndex& indices) const;
  // #CAT_Access true if indices in range; MAY ignore under-supplied dims
  inline bool           InRange_Flat(int idx) const {return ((idx >= 0) && (idx < size));}
  // #CAT_Access checks if in actual range


  ///////////////////////////////////////
  // Float access

  float         SafeElAsFloat(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const
  { return SafeElAsFloat_Flat(SafeElIndex(d0, d1, d2, d3, d4, d5, d6)); }
  // #CAT_Access (safely) returns the element as a float
  float         SafeElAsFloatN(const MatrixIndex& indices) const
  { return SafeElAsFloat_Flat(SafeElIndexN(indices)); }
  // #CAT_Access (safely) returns the element as a float
  float         SafeElAsFloat_Flat(int idx) const
  { if (InRange_Flat(idx)) return El_GetFloat_(FastEl_Flat_(idx)); else return 0.0f; }
  // #CAT_Access (safely) returns the element as a float, flat array model

  float         FastElAsFloat(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const
  { return El_GetFloat_(FastEl_Flat_(FastElIndex(d0, d1, d2, d3, d4, d5, d6))); }
  // #CAT_Access get element as float without range checking
  float         FastElAsFloatN(const MatrixIndex& indices) const
  { return El_GetFloat_(FastEl_Flat_(FastElIndexN(indices))); }
  // #CAT_Access get element as float without range checking
  float         FastElAsFloat_Flat(int idx) const
  { return El_GetFloat_(FastEl_Flat_(idx)); }
  // #CAT_Access get element as float without range checking, flat array model

  ///////////////////////////////////////
  // Double access

  double        SafeElAsDouble(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const
  { return SafeElAsDouble_Flat(SafeElIndex(d0, d1, d2, d3, d4, d5, d6)); }
  // #CAT_Access (safely) returns the element as a double
  double        SafeElAsDoubleN(const MatrixIndex& indices) const
  { return SafeElAsDouble_Flat(SafeElIndexN(indices)); }
  // #CAT_Access (safely) returns the element as a double
  double        SafeElAsDouble_Flat(int idx) const
  { if (InRange_Flat(idx)) return El_GetDouble_(FastEl_Flat_(idx)); else return 0.0f; }
  // #CAT_Access (safely) returns the element as a double, float array model

  double        FastElAsDouble(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const
  { return El_GetDouble_(FastEl_Flat_(FastElIndex(d0, d1, d2, d3, d4, d5, d6))); }
  // #CAT_Access get element as double without range checking
  double        FastElAsDoubleN(const MatrixIndex& indices) const
  { return El_GetDouble_(FastEl_Flat_(FastElIndexN(indices))); }
  // #CAT_Access get element as double without range checking
  double        FastElAsDouble_Flat(int idx) const
  { return El_GetDouble_(FastEl_Flat_(idx)); }
  // #CAT_Access get element as double without range checking, flat array model

  ///////////////////////////////////////
  // sub-matrix reading and writing functions

  static Variant RenderValue(const Variant& dest_val, const Variant& src_val, RenderOp render_op);
  // #CAT_SubMatrix compute render operation on given values

  virtual void  WriteFmSubMatrix(const taMatrix* src, RenderOp render_op = COPY,
                                 int off0=0, int off1=0, int off2=0,
                                 int off3=0, int off4=0, int off5=0, int off6=0);
  // #CAT_SubMatrix write to this matrix from source sub-matrix (typically of smaller size), using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost
  virtual void  ReadToSubMatrix(taMatrix* dest, RenderOp render_op = COPY,
                                int off0=0, int off1=0, int off2=0,
                                int off3=0, int off4=0, int off5=0, int off6=0);
  // #CAT_SubMatrix read from this matrix to dest sub-matrix (typically of smaller size), using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost

  virtual void  WriteFmSubMatrixFrames(taMatrix* src, RenderOp render_op = COPY,
                                       int off0=0, int off1=0, int off2=0,
                                       int off3=0, int off4=0, int off5=0, int off6=0);
  // #CAT_SubMatrix write to each frame of this matrix from source sub-matrix (typically of smaller cell size than this one -- if source has one less dimension, then same values are replicated across frames), using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost
  virtual void  ReadToSubMatrixFrames(taMatrix* dest, RenderOp render_op = COPY,
                                      int off0=0, int off1=0, int off2=0,
                                      int off3=0, int off4=0, int off5=0, int off6=0);
  // #CAT_SubMatrix read from each frame of this matrix to dest sub-matrix (typically of smaller cell size than this one, but must have same number of dimensions and frame count will be set to be same as this matrix), using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost

  ///////////////////////////////////////
  // alloc management

  bool                  AddFrame() {return AddFrames(1);}
  // #MENU #MENU_ON_Matrix #MENU_CONTEXT #CAT_Modify add 1 new blank frame
  virtual bool          AddFrames(int n);
  // #MENU #MENU_ON_Matrix #MENU_CONTEXT #CAT_Modify add n new blank frames -- note that this assumes incremental growth and thus calls AllocFrames in advance
  virtual bool          AllocFrames(int n);
  // #CAT_Modify make sure space exists for n frames: calling this is optional, and is typically done if you know ahead of time how much space you need
  virtual bool          EnforceFrames(int n, bool notify = true);
  // #MENU #MENU_ON_Object #ARGC_1 #CAT_Modify set size to n frames, blanking new elements if added; resizes by 1.5x as needed
  virtual bool          RemoveFrames(int st_fr, int n_frames=1);
  // #MENU #MENU_ON_Matrix #CAT_Modify remove the given number of frames at starting index, copying data backwards if needed.  st_fr = -1 means last frame, and n_frames = -1 means all frames from start to end
  virtual bool          InsertFrames(int st_fr, int n_frames=1);
  // #MENU #CAT_Modify insert n new frames at given frame index, shifting existing data down to make room.  st_fr = -1 means append to end
  virtual bool          CopyFrame(const taMatrix& src, int frame);
  // #CAT_Copy copy the source matrix to the indicated frame; src geom must be our frame geom; optimized for like-type mats

  virtual void          Reset();
  // #MENU #MENU_ON_Matrix #MENU_SEP_BEFORE #CAT_Modify remove all items
  void                  Clear(int fm = 0, int to = -1);
  // #MENU #MENU_ON_Matrix #CAT_Modify optimized clear, defaults to clearing all items

  bool                  SetGeom(int size, int d0, int d1=0, int d2=0,
    int d3=0, int d4=0, int d5=0, int d6=0)
  { int d[TA_MATRIX_DIMS_MAX]; d[0]=d0; d[1]=d1; d[2]=d2; d[3]=d3;
    d[4]=d4; d[5]=d5; d[6]=d6; d[7]=0; return SetGeom_(size, d);}
  // #CAT_Modify set geom for matrix -- if matches current size, it is non-destructive
  bool                  SetGeomN(const MatrixGeom& geom_)
  { return SetGeom_(geom_.dims(), geom_.el); }
  // #MENU #MENU_CONTEXT #MENU_ON_Matrix #MENU_SEP_BEFORE #CAT_Modify #INIT_ARGVAL_ON_geom set geom for any sized matrix -- if matches current size, it is non-destructive
  bool                  SetShape(const taMatrix* mat_shape)
  { MatrixGeom gm; gm = mat_shape; return SetGeomN(gm); }
  // #CAT_Modify set geometry (shape) based on given matrix shape specification

  // Slicing -- NOTES:
  // 1. slices are updated if parent allocation changes -- this could collapse slice to [0]
  // 2. slices are not guaranteed to be unique (i.e. same spec may return same slice ref)
  // 3a. you *must* ref/unref the slice
  // 3b. a slice refs its parent; unrefs it on destroy
  // 4. you may only request "proper slices", i.e., full dimensional subsets
  // 5. if parent is redimensioned, all slices are collapsed to [0]

  virtual taMatrix*     GetSlice_(const MatrixIndex& base,
    int slice_frame_dims = -1, int num_slice_frames = 1);
  // #CAT_Access return a slice, sfd=-1 indicates a frame size slice; base should be a subframe boundary
  virtual taMatrix*     GetFrameSlice_(int frame);
  // #CAT_Access return a slice, of exactly one frame; will have dim-1 of us
  virtual taMatrix*     GetFrameRangeSlice_(int st_frame, int n_frames);
  // #CAT_Access return a slice, of n_frames frames starting at st_frame

  virtual bool          StrValIsValid(const String& str, String* err_msg = NULL) const
  { return true; }
  // #IGNORE validates a proposed string-version of a value, ex. float_Matrix can verify valid floating rep of string

  virtual bool          BinaryFile_Supported() { return false; }
  // indicates if binary file format is supported (default no)
#ifndef __MAKETA__
  virtual int           BinarySave_strm(std::ostream& strm);
  // #CAT_File load binary data from a stream -- simple binary format with same initial ascii header and then items just straight binary write out -- not compatible across different endian processors etc
  virtual int           BinaryLoad_strm(std::istream& strm);
  // #CAT_File load binary data from a stream -- simple binary format with same initial ascii header and then items just straight binary write out -- not compatible across different endian processors etc
#endif
  virtual void          BinarySave(const String& fname="");
  // #CAT_File #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_mat #FILE_DIALOG_SAVE saves data -- leave fname empty to pick from file chooser -- simple binary format with same initial ascii header and then items just straight binary write out -- not compatible across different endian processors etc
  virtual void          BinaryLoad(const String& fname="");
  // #CAT_File #MENU #MENU_ON_Object #EXT_mat #FILE_DIALOG_LOAD loads data -- leave fname empty to pick from file chooser -- simple binary format with same initial ascii header and then items just straight binary write out -- not compatible across different endian processors etc

  String&       Print(String& strm, int indent=0) const;
  override String GetValStr(void* par = NULL, MemberDef* md = NULL,
                            TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                            bool force_inline = false) const;
  override bool  SetValStr(const String& val, void* par = NULL, MemberDef* md = NULL,
                           TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                           bool force_inline = false);
  override int  ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
              void* par = NULL, TypeDef* par_typ=NULL, MemberDef* md = NULL,
              TypeDef::StrContext sc = TypeDef::SC_DEFAULT);

  override void         SetDefaultName() { };
  using inherited::GetOwner;
  override taBase*      GetOwner() const;
  override int          Dump_Save_Value(std::ostream& strm, taBase* par=NULL, int indent = 0);
  override int          Dump_Load_Value(std::istream& strm, taBase* par=NULL);
  override void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);
  override void         InitLinks();
  override void         CutLinks();
  TA_ABSTRACT_BASEFUNS(taMatrix);

public:
  ///////////////////////////////////////////////////////////////////
  // low-level interface, try not to use these, internal use only

  virtual void*         data() const = 0;
  // #IGNORE pointer to the start of the raw data
  virtual void*         FastEl_Flat_(int i) = 0;
  // #IGNORE the raw element in the flat space _
  virtual const void*   FastEl_Flat_(int i) const = 0;
  // #IGNORE const version
  const void*           FastEl_(int d0, int d1=0, int d2=0, int d3=0,
    int d4=0, int d5=0, int d6=0) const
  { return FastEl_Flat_(FastElIndex(d0, d1, d2, d3, d4, d5, d6)); }
  // #IGNORE the raw element in index space -- YOU MUST ABSOLUTELY BE USING DIM-SAFE CODE
  const void*           FastElN_(const MatrixIndex& indices) const
  { return FastEl_Flat_(FastElIndexN(indices));}
  // #IGNORE

  virtual const void*   SafeEl_(int i) const
  {if ((i >= 0) && (i < size)) return FastEl_Flat_(i); else return El_GetBlank_(); }
  // #IGNORE raw element in flat space, else NULL

  // every subclass should implement these:
  virtual float         El_GetFloat_(const void*) const { return 0.0f; }
  // #IGNORE
  virtual double        El_GetDouble_(const void* it) const  { return El_GetFloat_(it); }
  // #IGNORE
  virtual const String  El_GetStr_(const void*) const   { return _nilString; }
  // #IGNORE
  virtual void          El_SetFmStr_(void*, const String&) { };
  // #IGNORE
  virtual const Variant El_GetVar_(const void*) const   { return _nilVariant; }
  // #IGNORE
  virtual void          El_SetFmVar_(void*, const Variant&) { };
  // #IGNORE

  iMatrixTableModel*     GetTableModel(); // gets the table model, making if needed

public:
  ///////////////////////////////////////////////////////////////////
  // common operators

  virtual taMatrix* operator=(const Variant& t);

  virtual taMatrix* operator+(const taMatrix& t) const;
  virtual taMatrix* operator+(const Variant& t) const;

  virtual taMatrix* operator-(const taMatrix& t) const;
  virtual taMatrix* operator-(const Variant& t) const;

  virtual taMatrix* operator*(const taMatrix& t) const;
  virtual taMatrix* operator*(const Variant& t) const;

  virtual taMatrix* operator/(const taMatrix& t) const;
  virtual taMatrix* operator/(const Variant& t) const;

  virtual taMatrix* operator%(const taMatrix& t) const;
  virtual taMatrix* operator%(const Variant& t) const;

  virtual taMatrix* operator^(const taMatrix& t) const;
  virtual taMatrix* operator^(const Variant& t) const;

  virtual taMatrix* operator-() const;

  virtual taMatrix* Max(const taMatrix& t) const;
  // returns new matrix which is max of elements in two matricies
  virtual taMatrix* Max(const Variant& t) const;
  // returns new matrix which is max of elements and given value
  virtual Variant   Max() const;
  // returns maximum value in matrix

  virtual taMatrix* Min(const taMatrix& t) const;
  // returns new matrix which is min of elements in two matricies
  virtual taMatrix* Min(const Variant& t) const;
  // returns new matrix which is min of elements and given value
  virtual Variant   Min() const;
  // returns minimum value in matrix

  virtual taMatrix* Abs() const;
  // return a new matrix containing the absolute value of items in this matrix

  virtual void      operator+=(const taMatrix& t);
  virtual void      operator+=(const Variant& t);

  virtual void      operator-=(const taMatrix& t);
  virtual void      operator-=(const Variant& t);

  virtual void      operator*=(const taMatrix& t);
  virtual void      operator*=(const Variant& t);

  virtual void      operator/=(const taMatrix& t);
  virtual void      operator/=(const Variant& t);

  virtual void      operator%=(const taMatrix& t);
  virtual void      operator%=(const Variant& t);

  // boolean operators return a byte_Matrix

  virtual taMatrix* operator<(const taMatrix& t) const;
  virtual taMatrix* operator<(const Variant& t) const;

  virtual taMatrix* operator>(const taMatrix& t) const;
  virtual taMatrix* operator>(const Variant& t) const;

  virtual taMatrix* operator<=(const taMatrix& t) const;
  virtual taMatrix* operator<=(const Variant& t) const;

  virtual taMatrix* operator>=(const taMatrix& t) const;
  virtual taMatrix* operator>=(const Variant& t) const;

  virtual taMatrix* operator==(const taMatrix& t) const;
  virtual taMatrix* operator==(const Variant& t) const;

  virtual taMatrix* operator!=(const taMatrix& t) const;
  virtual taMatrix* operator!=(const Variant& t) const;

  virtual taMatrix* operator&&(const taMatrix& t) const;
  virtual taMatrix* operator&&(const Variant& t) const;

  virtual taMatrix* operator||(const taMatrix& t) const;
  virtual taMatrix* operator||(const Variant& t) const;

  virtual taMatrix* operator!() const;

  virtual taMatrix* Flatten() const;
  // #CAT_Create returns a flat, 1D list of values in the matrix, subject to filtering by the current view -- this is how to finally extract a subset of values from a view, instead of just continuing to filter the original items
  virtual int_Matrix* Find() const;
  // #CAT_Matrix (synonym for nonzero) returns a 1D matrix of coordinates into given matrix for all values that are non-zero -- the resulting coordinates can be used as a more efficient view onto a matrix of the same shape (the original matrix can also be used directly as a mask view, but it is less efficient, especially as the number of non-zero values is relatively small
  inline int_Matrix* NonZero() const { return Find(); }
  // #CAT_Matrix (synonym for find) returns a 1D matrix of coordinates into given matrix for all values that are non-zero -- the resulting coordinates can be used as a more efficient view onto a matrix of the same shape (the original matrix can also be used directly as a mask view, but it is less efficient, especially as the number of non-zero values is relatively small
  virtual taMatrix* Transpose() const;
  // #CAT_Matrix returns a new matrix that is the transpose of this matrix, where the rows and columns have been switched -- this matrix must be a 2D matrix

protected:
  override void         UpdateAfterEdit_impl();
  override void         BatchUpdate(bool begin, bool struc);

  static void           SliceInitialize(taMatrix* par_slice, taMatrix* child_slice);
   // called after slice created -- static for consistency
  static void           SliceDestroying(taMatrix* par_slice, taMatrix* child_slice);
   //called by child slice on destroy -- static because it can cause destruction

  int                   alloc_size; // -1 means fixed (external data)
  taMatrix_PList*       slices; // list of extant slices -- created on first slice
  taMatrix*             slice_par; // slice parent -- we ref/unref it
  fixed_dealloc_fun     fixed_dealloc; // optional dealloc fun passed in on FixedData
  iMatrixTableModel*     table_model; // created on-demand, then persists for lifetime

  virtual bool          fastAlloc() const {return true;}
  // #IGNORE enables using fast block-based allocations, copies, and skipping reclaims -- for ints,floats, etc.; not for Strings/Variants
  virtual bool          SetGeom_(int dims_, const int geom_[]); //

  virtual bool          Alloc_(int new_alloc);
  // set capacity to n -- should always be in multiples of frames
  virtual void*         MakeArray_(int i) const = 0;
  // #IGNORE make a new array of item type
  virtual void          SetArray_(void* nw) = 0;
  virtual void*         FastRealloc_(int i) = 0;
  // #IGNORE reallocate existing array, only valid for fastAlloc() objects
  virtual void          ReclaimOrphans_(int from, int to) {}
  // called when elements can be reclaimed, ex. for strings
  virtual void          Clear_impl(int fm, int to);

  virtual void          Add_(const void* it);
  // compatibility function -- only valid if dims=1
  virtual bool          El_Equal_(const void*, const void*) const = 0;
  // #IGNORE for finding
  virtual const void*   El_GetBlank_() const = 0;
  // #IGNORE address of a blank element, for initializing empty items -- can be static const
  virtual const void*   El_GetErr_() const      { return El_GetBlank_();}
  // #IGNORE address of an element to return when out of range -- defaults to blank el
  virtual void          El_Copy_(void*, const void*) = 0;
  // #IGNORE
  virtual uint          El_SizeOf_() const = 0;
  // #IGNORE size of element

  virtual void          SetFixedData_(void* el_, const MatrixGeom& geom_,
    fixed_dealloc_fun fixed_dealloc = NULL);
  // initialize fixed data
//  virtual bool                Equal_(const taMatrix& src) const;
    // 'true' if same size and els

  void                  Slice_Collapse();
  void                  Slice_Realloc(ta_intptr_t base_delta);
  taMatrix*             FindSlice(void* el_, const MatrixGeom& geom_);

  virtual void          UpdateGeom();
  // called to potentially update the allocation based on new geom info -- will fix if in error
  void                  UpdateSlices_Collapse();
  // collapses all the slices to []
  void                  UpdateSlices_FramesDeleted(void* deletion_base, int num);
  // called when deleting a frame -- for each slice: update base addr if after delete base, or collapse if doesn't exist any more
  void                  UpdateSlices_Realloc(ta_intptr_t base_delta);
  // called when allocing new mem (more or less) -- for each slice: update base addr; note: not for use if size has changed (FramesDeleted would be called)

#ifndef __MAKETA__
  virtual void          Dump_Save_Item(std::ostream& strm, int idx);
  // dump the value, term with ; generic is fine for numbers, override for strings, variants, etc.
  virtual int           Dump_Load_Item(std::istream& strm, int idx);
  // load the ;-term'ed value ; generic is fine for numbers, override for strings, variants, etc.; ret is last char read, usually ;

  virtual void          BinarySave_Item(std::ostream& strm, int idx) { };
  // binary dump the value -- just straight binary output -- must overload in specific classes
  virtual void          BinaryLoad_Item(std::istream& strm, int idx) { };
  // binary load the value -- just straight binary format -- must overload in specific classes
#endif

  override void         CanCopyCustom_impl(bool to, const taBase* cp, bool quiet,
    bool& allowed, bool& forbidden) const;
  override void         CopyFromCustom_impl(const taBase* cp_fm);
  virtual void          Copy_Matrix_impl(const taMatrix* cp);
   // generic copy using Variant; only called when not same matrix type

private:
  NOCOPY(taMatrix) // there is a generic copy, but not at the direct object level
  void                  Initialize();
  void                  Destroy();
};

TA_SMART_PTRS(taMatrix);

#endif // taMatrix_h
