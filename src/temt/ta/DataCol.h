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

#ifndef DataCol_h
#define DataCol_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <MatrixGeom>
#include <ColCalcExpr>
#include <String_Matrix>
#include <Relation>

// declare all other types mentioned but not required to include:
class taHashTable; // 
class taMatrix; // 
class TypeDef; // 
class MinMax; // 
class DataTable; // 
class MatrixIndex; // 

/*
  Display Options (subclasses add new ones -- see each class)

  HIDDEN -- forces !visible by default
  TEXT -- sets display_style to TEXT
  NARROW -- sets display_style to TEXT; also, makes it !visible by default

*/

taTypeDef_Of(DataCol);

class TA_API DataCol : public taNBase {
  // #STEM_BASE #VIRT_BASE #NO_INSTANCE ##CAT_Data ##NO_UNDO_SELECT holds a column of data;\n (a scalar cell can generally be treated as a degenerate matrix cell of dim[1])
INHERITED(taNBase)
friend class DataTable;
public:
  static const String   udkey_width; // WIDTH=n display width, in chars
  static const String   udkey_narrow; // NARROW=b if narrow (default for ints)
  static const String   udkey_hidden; // HIDDEN=b defaults to not visible

  enum ColFlags { // #BITS flags for data table columns
    DC_NONE             = 0, // #NO_BIT
    MARK                = 0x0001, // #NO_SHOW used internally to mark columns prior to an operation -- columns that remain marked after all operations are unused, and may be removed -- users should not generally mess with this flag
    PIN                 = 0x0002, // #NO_SHOW protect this column from being automatically deleted according to the MARK scheme (see comment).  this is often not very easy for uers to find and use (columns to be saved should be listed explicitly in the context in which others are being used), so we are not exposing it to users, but it can be used internally for some reason
    SAVE_ROWS           = 0x0004, // save the row data for this column in the internal format used when the entire object is saved (e.g., along with a project or whatever).  the column configuration etc is always saved, just not the rows of data if not on.
    SAVE_DATA           = 0x0008, // save this column in the 'data' export format used by the SaveData and associated functions (e.g., as recorded in 'logs' of data from running programs)
    CALC                = 0x0010, // calculate value of this column based on calc_expr expression
    READ_ONLY           = 0x0020, // this column is read-only in the gui (helps protect keys or programmatically generated data items)
    PAT_4D              = 0x0040, // (4d cells only) displays cells in the same geometry as grouped network layers -- NOTE: data is still copied/pasted in clipboard in a 2d format
  };

  String                desc; // #NO_SAVE_EMPTY #EDIT_DIALOG optional description to help in documenting the use of this column
  ColFlags              col_flags; // flags for this column to indicate specific properties
  int                   col_idx; // #READ_ONLY #SHOW #NO_SAVE #NO_COPY the index of this column in the table
  bool                  is_matrix;
  // #READ_ONLY #SAVE #SHOW 'true' if the cell is a matrix, not a scalar
  MatrixGeom            cell_geom;
  // #READ_ONLY #SAVE #SHOW for matrix cols, the geom of each cell
  ColCalcExpr           calc_expr; // #CONDEDIT_ON_col_flags:CALC expression for computing value of this column (only used if CALC flag is set)
  String_Matrix         dim_names; // special names for the dimensions of a matrix cell -- used for display purposes
  taHashTable*          hash_table; // #READ_ONLY #NO_SAVE #NO_COPY hash table of column (scalar only) values to speed up finding in large fixed tables -- this is created by BuildHashTable() function, and destroyed after any insertion or removal of rows -- it is up to the user to call this when relevant data is all in place -- cannot track value changes


  virtual const taMatrix*       AR() const = 0;
  // #CAT_Access const version of the matrix pointer
  virtual taMatrix*     AR() = 0;
  // #CAT_Access the matrix pointer -- NOTE: actual member should be called 'ar'
  virtual void          SetMatrixViewMode();
  // #IGNORE set the matrix view mode to parent datatable row_indexes 
  virtual void          UnSetMatrixViewMode();
  // #IGNORE set the matrix view mode to NULL -- do this before doing destructive things to underlying array

  /////////////////////////////////////////////
  // type of data

  virtual ValType       valType() const = 0;
  // #CAT_Access the type of data in each element
  virtual TypeDef*      valTypeDef() const = 0;
  // #CAT_Access the type of data in each element, as a TypeDef
  inline bool           isMatrix() const {return is_matrix;}
  // #CAT_Access true if data is a matrix
  virtual bool          isNumeric() const {return false;}
  // #CAT_Access true if data is float, int, or byte
  virtual bool          isFloat() const {return false;}
  // #CAT_Access true if data is float, double
  virtual bool          isString() const {return false;}
  // #CAT_Access true if data is string
  virtual bool          isImage() const;
  // #CAT_Access true if the cell contains an image
  virtual int           imageComponents() const;
  // #CAT_Access if an image, then: b&w=1, b&w+a=2, rgb=3, rgba=4

  inline bool           is_matrix_err()
  { return TestError(is_matrix, "is_matrix_err", "column has matrix data -- not supported"); }
  inline bool           not_matrix_err()
  { return TestError(!is_matrix, "not_matrix_err", "column does not have matrix data -- not supported"); }

  virtual int           cell_size() const
  { return (is_matrix) ? cell_geom.Product() : 1; }
  // #CAT_Access for matrix type, number of elements in each cell
  virtual int           cell_dims() const { return cell_geom.dims(); }
  // #CAT_Access for matrix type, number of dimensions in each cell
  virtual int           GetCellGeom(int dim) const { return cell_geom.dim(dim); }
  // #CAT_Access for matrix type, size of given dim
  void                  Get2DCellGeom(int& x, int& y) const; // for flat 2D access

  void                  Get2DCellGeomGui(int& x, int& y,
     bool odd_y = true, int spc = 1) const;
  // #CAT_Access provides standardized 2d geom regardless of dimensionality (includes space for extra dimensions), odd_y = for odd dimension sizes, put extra dimension in y (else x): 3d = x, (y+1) * z (vertical time series of 2d patterns, +1=space), 4d = (x+1)*xx, (y+1)*yy (e.g., 2d groups of 2d patterns), 5d = vertical time series of 4d.

  int                   rows() const { return AR()->Frames(); }
  // #CAT_Access total number of rows of data within this column

  /////////////////////////////////////////////
  // Flags

  inline void           SetColFlag(ColFlags flg)   { col_flags = (ColFlags)(col_flags | flg); }
  // set data column flag state on
  inline void           ClearColFlag(ColFlags flg) { col_flags = (ColFlags)(col_flags & ~flg); }
  // clear data column flag state (set off)
  inline bool           HasColFlag(ColFlags flg) const { return (col_flags & flg); }
  // check if data column flag is set
  inline void           SetColFlagState(ColFlags flg, bool on)
  { if(on) SetColFlag(flg); else ClearColFlag(flg); }
  // set data column flag state according to on bool (if true, set flag, if false, clear it)

  /////////////////////////////////////////////////////////
  // Main data value access/modify (Get/Set) routines: for Programs and very general use

  const Variant GetVal(int row) const { return GetValAsVar_impl(row, 0); }
  // #CAT_Access get data of scalar type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last)
  bool          SetVal(const Variant& val, int row)
  { return SetValAsVar_impl(val, row, 0); }
  // #CAT_Modify get data of scalar type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last)

  const Variant GetMatrixVal(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsVarMDims(row, d0,d1,d2,d3); }
  // #CAT_Access get value of matrix type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last), d's are matrix dimension indicies
  const Variant GetMatrixFlatVal(int row, int cell) const
  { return GetValAsVarM(row, cell); }
  // #CAT_Access get value of matrix type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last), using flat representation of matrix cell (single cell index)
  bool          SetMatrixVal(const Variant& val, int row,
                             int d0, int d1=0, int d2=0, int d3=0)
  { return SetValAsVarMDims(val, row, d0,d1,d2,d3); }
  // #CAT_Modify set value of matrix type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last), d's are matrix dimension indicies
  bool          SetMatrixFlatVal(const Variant& val, int row, int cell)
  { return SetValAsVar_impl(val, row, cell); }
  // #CAT_Modify set value of matrix type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last), using flat representation of matrix cell (single cell index)

  bool          InitVals(const Variant& init_val);
  // #CAT_Modify #BUTTON #MENU #MENU_CONTEXT #MENU_ON_Column initialize all values in this column to given value
  bool          InitValsToRowNo();
  // #CAT_Modify #MENU #MENU_CONTEXT initialize all values in this column to be equal to the row number -- only valid for scalar (not matrix) columns

  void          SortAscending();
  // #CAT_DataProc #MENU #MENU_CONTEXT #MENU_ON_Column #MENU_SEP_BEFORE
  void          SortDescending();
  // #CAT_DataProc #MENU  #MENU_CONTEXT #MENU_ON_Column
  void          Filter(Relation::Relations operator_1 = Relation::EQUAL, const String& value_1 = "",
      Relation::CombOp comb_op = Relation::AND, Relation::Relations operator_2 = Relation::EQUAL, const String& value_2 = "");
   // #CAT_DataProc #MENU #MENU_CONTEXT #MENU_ON_Column #LABEL_Filter Hide rows not matching the filter

  void          RunClusterAnalysis();
  // #CAT_Stat #MENU  #MENU_CONTEXT #MENU_ON_Column #MENU_SEP_BEFORE
  void          RunPCA2dAnalysis();
  // #CAT_Stat #MENU  #MENU_CONTEXT #MENU_ON_Column

  void          CatRelation(String& expr, Relation::Relations oper);
  // concatenate an relation operator onto the end of the expression
  void          CatConjunction(String& expr, Relation::CombOp conjunction);
  // concatenate a conjunction operator onto the end of the expression
  void          CatValue(String& expr, const String& value);
  // concatenate a value onto the end of the expression
  void          CatColumnName(String& expr);
  // concatenate a value onto the end of the expression

  int           FindVal(const Variant& val, int st_row = 0) const;
  // #CAT_Access find row number for given value within column of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)

  void          BuildHashTable();
  // #CAT_Access build a hash table of column (scalar only) values to speed up finding in large fixed tables -- table is destroyed after any insertion or removal of rows -- it is up to the user to call this when relevant data is all in place -- system cannot track value changes
  void          RemoveHashTable();
  // #CAT_Access remove a hash table for this column (see BuildHashTable)

  /////////////////////////////////////////////
  // Get and Set access

  // row is absolute row in the matrix, not a DataTable row -- use the DataTable routines
  // -ve values are from end, and are valid for both low-level col access, and DataTable access

  const Variant GetValAsVar(int row) const {return GetValAsVar_impl(row, 0);}
  // #EXPERT #CAT_Access valid for all types, -ve row is from end (-1=last)
  bool          SetValAsVar(const Variant& val, int row)
  { return SetValAsVar_impl(val, row, 0);}
  // #EXPERT #CAT_Modify valid for all types, -ve row is from end (-1=last)
  const String  GetValAsString(int row) const {return GetValAsString_impl(row, 0);}
  // #EXPERT #CAT_Access valid for all types, -ve row is from end (-1=last)
  bool          SetValAsString(const String& val, int row)
  {return SetValAsString_impl(val, row, 0);}
  // #EXPERT #CAT_Modify valid for all types, -ve row is from end (-1=last)
  float         GetValAsFloat(int row) const {return GetValAsFloat_impl(row, 0);}
  // #EXPERT #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool          SetValAsFloat(float val, int row)
  // #EXPERT #CAT_Modify valid only if type is float, -ve row is from end (-1=last)
  {return SetValAsFloat_impl(val, row, 0);}
  double        GetValAsDouble(int row) const {return GetValAsDouble_impl(row, 0);}
  // #EXPERT #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool          SetValAsDouble(double val, int row)
  // #EXPERT #CAT_Modify valid only if type is double, -ve row is from end (-1=last)
  {return SetValAsDouble_impl(val, row, 0);}
  int           GetValAsInt(int row) const {return GetValAsInt_impl(row, 0);}
  // #EXPERT #CAT_Access valid if type is int or byte, -ve row is from end (-1=last)
  bool          SetValAsInt(int val, int row)
  // #EXPERT #CAT_Modify valid if type is int or float, -ve row is from end (-1=last)
  {return SetValAsInt_impl(val, row, 0);}
  byte          GetValAsByte(int row) const {return GetValAsByte_impl(row, 0);}
  // #EXPERT #CAT_Access valid only if type is byte, -ve row is from end (-1=last)
  bool          SetValAsByte(byte val, int row)
  // #EXPERT #CAT_Modify valid if type is numeric, -ve row is from end (-1=last)
  {return SetValAsByte_impl(val, row, 0);}

  ///////////////////////////////////////////////////////////////
  // Matrix versions, cell index

  const Variant GetValAsVarM(int row, int cell) const {return GetValAsVar_impl(row, cell);}
  // #EXPERT #CAT_Access get value as a variant (safe for all program usage), matrix version, valid for all types, -ve row is from end (-1=last)
  bool          SetValAsVarM(const Variant& val, int row, int cell)
  { return SetValAsVar_impl(val, row, cell);}
  // #EXPERT #CAT_Modify set value as a variant, matrix version, valid for all types, -ve row is from end (-1=last)
  const String  GetValAsStringM(int row, int cell) const {return GetValAsString_impl(row, cell);}
  // #EXPERT #CAT_Access get value as a string, matrix version, valid for all types, -ve row is from end (-1=last)
  bool          SetValAsStringM(const String& val, int row, int cell)
  {return SetValAsString_impl(val, row, cell);}
  // #EXPERT #CAT_Modify valid for all types, -ve row is from end (-1=last)
  float         GetValAsFloatM(int row, int cell) const {return GetValAsFloat_impl(row, cell);}
  // #EXPERT #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool          SetValAsFloatM(float val, int row, int cell)
  // #EXPERT #CAT_Modify valid only if type is float, -ve row is from end (-1=last)
  {return SetValAsFloat_impl(val, row, cell);}
  double        GetValAsDoubleM(int row, int cell) const {return GetValAsDouble_impl(row, cell);}
  // #EXPERT #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool          SetValAsDoubleM(double val, int row, int cell)
  // #EXPERT #CAT_Modify valid only if type is float, -ve row is from end (-1=last)
  {return SetValAsDouble_impl(val, row, cell);}
  int           GetValAsIntM(int row, int cell) const {return GetValAsInt_impl(row, cell);}
  // #EXPERT #CAT_Access valid if type is int or byte, -ve row is from end (-1=last)
  bool          SetValAsIntM(int val, int row, int cell)
  // #EXPERT #CAT_Modify valid if type is int or float, -ve row is from end (-1=last)
  {return SetValAsInt_impl(val, row, cell);}
  byte          GetValAsByteM(int row, int cell) const {return GetValAsByte_impl(row, cell);}
  // #EXPERT #CAT_Access valid only if type is byte, -ve row is from end (-1=last)
  bool          SetValAsByteM(byte val, int row, int cell)
  // #EXPERT #CAT_Modify valid if type is numeric, -ve row is from end (-1=last)
  {return SetValAsByte_impl(val, row, cell);}

  ///////////////////////////////////////////////////////////////
  // Matrix versions, with dimensions passed
  const Variant GetValAsVarMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsVar_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #EXPERT #CAT_Access valid for all types, -ve row is from end (-1=last)
  bool          SetValAsVarMDims(const Variant& val, int row,
                                 int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsVar_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));}
  // #EXPERT #CAT_Modify valid for all types, -ve row is from end (-1=last)
  const String  GetValAsStringMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsString_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #EXPERT #CAT_Access valid for all types, -ve row is from end (-1=last)
  bool          SetValAsStringMDims(const String& val, int row,
                                    int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsString_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));}
  // #EXPERT #CAT_Modify valid for all types, -ve row is from end (-1=last)
  float         GetValAsFloatMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsFloat_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #EXPERT #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool          SetValAsFloatMDims(float val, int row, int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsFloat_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));}
  // #EXPERT #CAT_Modify valid only if type is float, -ve row is from end (-1=last)
  double        GetValAsDoubleMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsDouble_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #EXPERT #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool          SetValAsDoubleMDims(double val, int row, int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsDouble_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));}
  // #EXPERT #CAT_Modify valid only if type is float, -ve row is from end (-1=last)
  int           GetValAsIntMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsInt_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #EXPERT #CAT_Access valid if type is int or byte, -ve row is from end (-1=last)
  bool          SetValAsIntMDims(int val, int row, int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsInt_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));}
  // #EXPERT #CAT_Modify valid if type is int or float, -ve row is from end (-1=last)
  byte          GetValAsByteMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsByte_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #EXPERT #CAT_Access valid only if type is byte, -ve row is from end (-1=last)
  bool          SetValAsByteMDims(byte val, int row, int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsByte_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));}
  // #EXPERT #CAT_Modify valid if type is numeric, -ve row is from end (-1=last)

  ///////////////////////////////////////////////////////////////
  // Matrix ops -- you must Ref/UnRef taMatrix return types
  taMatrix*     GetValAsMatrix(int row);
  // #CAT_Access gets the cell as a slice of the entire column (note: not const -- you can write it) -- must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it
  bool          SetValAsMatrix(const taMatrix* val, int row);
  // #CAT_Modify set the matrix cell from a same-sized matrix
  taMatrix*     GetRangeAsMatrix(int st_row, int n_rows);
  // #EXPERT #CAT_Access gets a slice of the entire column from starting row for n_rows (note: not const -- you can write it) -- must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it

  ///////////////////////////////////////
  // sub-matrix reading and writing functions

  virtual void  WriteFmSubMatrix(int row, const taMatrix* submat_src,
                                 taMatrix::RenderOp render_op = taMatrix::COPY,
                                 int off0=0, int off1=0, int off2=0,
                                 int off3=0, int off4=0, int off5=0, int off6=0);
  // #CAT_SubMatrix for making larger patterns out of smaller ones (sub-matricies) and vice-versa: write to matrix cell in this table at given row from source sub-matrix (typically of smaller size), using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost
  virtual void  ReadToSubMatrix(int row, taMatrix* submat_dest,
                                taMatrix::RenderOp render_op = taMatrix::COPY,
                                int off0=0, int off1=0, int off2=0,
                                int off3=0, int off4=0, int off5=0, int off6=0);
  // #CAT_SubMatrix for making larger patterns out of smaller ones (sub-matricies) and vice-versa: read from matrix cell in this table at given row to dest sub-matrix (typically of smaller size), using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost

  /////////////////////////
  //    misc funs


  bool          GetMinMaxScale(MinMax& mm);
  // #CAT_Display get min-max range of values contained within this column

  /////////////////////////////////////////////
  // misc

  virtual String        ColStats();
  // #CAT_DataProc #BUTTON #MENU #MENU_ON_Column #MENU_SEP_BEFORE #USE_RVAL compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).

  int                   displayWidth() const;
  // #CAT_Display low level display width, in chars, taken from options
  virtual int           maxColWidth() const {return -1;}
  // #CAT_Display aprox max number of columns, in characters, -1 if variable or unknown
  virtual bool          saveToDumpFile() const { return HasColFlag(SAVE_ROWS); }
  // #IGNORE whether to save col to internal dump format
  virtual bool          saveToDataFile() const { return HasColFlag(SAVE_DATA); }
  // #IGNORE whether to save col to external 'data' format

  static const KeyString key_val_type;  // #IGNORE "val_type"
  override String       GetColText(const KeyString& key, int itm_idx = -1) const;
  override String       GetDisplayName() const; // #IGNORE we strip out the format characters
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const
  { return AR()->Elem(idx, mode); }

  DataTable*            dataTable() const; 	// root data table this col belongs to

  String                EncodeHeaderName(const MatrixIndex& dims) const;
  // encode header information for saving to text files
  static void           DecodeHeaderName(String nm, String& base_nm, int& val_typ,
    MatrixIndex& mat_idx, MatrixGeom& mat_geom);
  // decode header information for loading from text files

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md);

  virtual void  Copy_NoData(const DataCol& cp);
  // #CAT_Copy copy the structure of the datatable without getting all the data
  virtual void  CopyFromCol_Robust(const DataCol& cp);
  // #CAT_Copy copy col, but don't change schema; robust to differences in type and format of the cells
  virtual void  CopyFromRow(int dest_row, const DataCol& cp, int src_row);
  // #CAT_Copy copy one row from source to given row in this object, assumes that the two have the same type and, if matrix, cell_size
  virtual void  CopyFromRow_Robust(int dest_row, const DataCol& cp, int src_row);
  // #CAT_Copy copy one row from source to given row in this object, robust to differences in type and format of the cells

  //IMPORTANT: DO NOT CHANGE THE FOLLOWING METHODS TO VIRTUAL
  void          ChangeColType(ValType new_type);
  // #CAT_ObjectMgmt #BUTTON #MENU #DYN1 change the type of the data in this col, without loss of data
  void          ChangeColCellGeom(const MatrixGeom& new_geom);
  // #CAT_ObjectMgmt #BUTTON #MENU #DYN1 #INIT_ARGVAL_ON_cell_geom change the cell geom of a mat col, or change from scalar to mat, without loss of data
  void          ChangeColCellGeomNs(int dims = 1, int d0=0, int d1=0, int d2=0,
                                      int d3=0, int d4=0, int d5=0, int d6=0);
  // #CAT_ObjectMgmt change the cell geom of a mat col, or change from scalar to mat, without loss of data -- separate numbers input instead of overall geom object
  void          ChangeColMatToScalar();
  // #CAT_ObjectMgmt #MENU #DYN1 #CONFIRM change from a mat col to a scalar col

  override String       GetTypeDecoKey() const { return "DataTable"; }

  virtual void Init(); // call this *after* creation, or in UAE, to assert matrix geometry
  override int  GetSpecialState() const;
  override int  GetIndex() const {return col_idx;}
  override void SetIndex(int value) {col_idx = (short)value;}
  override String GetDesc() const {return desc;}
  override void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);
  override taObjDiffRec* GetObjDiffVal(taObjDiff_List& odl, int nest_lev,
                                       MemberDef* memb_def=NULL, const void* par=NULL,
                                       TypeDef* par_typ=NULL, taObjDiffRec* par_od=NULL) const;

  void  InitLinks(); //note: ok to do own AR here, because never called in constructor
  void  CutLinks(); //note: NOT ok to do disown AR here, because called in destructor
  TA_ABSTRACT_BASEFUNS(DataCol);

protected:
  override void  UpdateAfterEdit_impl();
  // in all accessor routines, -ve row is from end (-1=last)
  int           IndexOfEl_Flat(int row, int cell) const;
    // -ve row is from end (-1=last); note: returns -ve value if out of range, so must use with SafeEl_Flat
  int           IndexOfEl_Flat_Dims(int row, int d0, int d1=0, int d2=0, int d3=0,
                                    int d4=0) const;
    // -ve row is from end (-1=last); note: returns -ve value if out of range, so must use with SafeEl_Flat
  virtual const Variant GetValAsVar_impl(int row, int cell) const;
  virtual const String  GetValAsString_impl(int row, int cell) const;
  virtual double        GetValAsDouble_impl(int row, int cell) const { return 0.0; }
  virtual float         GetValAsFloat_impl(int row, int cell) const { return 0.0f; }
  virtual int           GetValAsInt_impl(int row, int cell) const { return 0; }
  virtual byte          GetValAsByte_impl(int row, int cell) const
  { return (byte)GetValAsInt_impl(row, cell); }

  // these all return true if value is successfully set
  virtual bool   SetValAsVar_impl(const Variant& val, int row, int cell);
  virtual bool   SetValAsString_impl(const String& val, int row, int cell);
  virtual bool   SetValAsDouble_impl(double val, int row, int cell) {return false;}
  virtual bool   SetValAsFloat_impl(float val, int row, int cell) { return false; }
  virtual bool   SetValAsInt_impl(int val, int row, int cell)  { return false; }
  virtual bool   SetValAsByte_impl(byte val, int row, int cell)
  { return SetValAsInt_impl((int)val, row, cell); }

  // note: all row memory management must come directly from the DataTable!

  bool          EnforceRows(int rows);
  // force data to have this many rows

private:
  void  Copy_(const DataCol& cp);
  void  Copy_Common_(const DataCol& cp);
  void  Initialize();
  void  Destroy()       {CutLinks(); }; //
};

TA_SMART_PTRS(DataCol); //

#endif // DataCol_h
