// Copyright, 1995-2007, Regents of the University of Colorado,
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

#ifndef TA_DATATABLE_H
#define TA_DATATABLE_H

#include "igeometry.h"

#include "ta_fontspec.h"
#include "minmax.h"
#include "ta_group.h"
#include "ta_matrix.h"
#include "ta_data.h"
#include "ta_filer.h"
#include "ta_viewspec.h"
#include "ta_dmem.h"
#include "ta_qtclipdata.h"
#include "ta_TA_type.h"

#ifndef __MAKETA__
# include <QAbstractTableModel> 
#endif

// externals
class T3DataViewFrame;
class GridTableView;
class GraphTableView;
class cssProgSpace;		// #IGNORE

// forwards this file

class DataCol;
class DataTableCols;
class DataTable;
class DataTable_Group;
class String_Data; 
class Variant_Data; 
class float_Data;
class double_Data;
class int_Data;
class byte_Data; //

// note: the ColCalcExpr could be augmented to update with column name changes
// as in ProgExpr
// but it has a catch-22 with requiring a reference to data columns which have
// yet to be defined because they contain this object..

class TA_API ColCalcExpr: public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Data a column calculation expression
INHERITED(taOBase)
public:

  String	expr;		// #EDIT_DIALOG enter the expression here -- column value will be set to this.\nyou can just type in names of other columns (value is corresponding row's value) or literal values, or math expressions, etc.\nenclose strings in double quotes.\ncolumn names will be checked and automatically updated
  DataCol* col_lookup;	// #APPLY_IMMED #NULL_OK #NO_EDIT #NO_SAVE #FROM_GROUP_data_cols #NO_UPDATE_POINTER lookup a program variable and add it to the current expression (this field then returns to empty/NULL)

  DataTableCols*	data_cols;
  // #READ_ONLY #HIDDEN #NO_SAVE data table columns (set from owner field)

  virtual bool	SetExpr(const String& ex);
  // set to use given expression -- use this interface for future compatibility

//   virtual bool	ParseExpr();
//   // parse the current expr for variables and update vars and var_expr accordingly (returns false if there are some bad_vars)
  virtual String GetFullExpr() const;
  // get full expression with variable names substituted appropriately -- use this interface instead of referring to raw expr, for future compatibility

  String 	GetName() const;

  void 	InitLinks();
  void 	CutLinks();
  void	Copy_(const ColCalcExpr& cp);
  TA_BASEFUNS(ColCalcExpr);
protected:

  override void		UpdateAfterEdit_impl();
//   override void 	CheckThisConfig_impl(bool quiet, bool& rval);
//   override void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
//   override void		SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
// 					     int dcr, void* op1_, void* op2_);

private:
  void	Initialize();
  void	Destroy();
};

// specific ones are in the template classes: String_Data, float_Data

/*
  Display Options (subclasses add new ones -- see each class)

  HIDDEN -- forces !visible by default
  TEXT -- sets display_style to TEXT
  NARROW -- sets display_style to TEXT; also, makes it !visible by default

*/

/////////////////////////////////////////////////////////
//   DataCol -- One colum of a datatable
/////////////////////////////////////////////////////////

class TA_API DataCol : public taNBase {
  // #STEM_BASE #VIRT_BASE ##NO_TOKENS #NO_INSTANCE ##CAT_Data holds a column of data;\n (a scalar cell can generally be treated as a degenerate matrix cell of dim[1])
INHERITED(taNBase)
friend class DataTable;
public:
  static const String	udkey_width; // WIDTH=n display width, in chars
  static const String	udkey_narrow; // NARROW=b if narrow (default for ints)
  static const String	udkey_hidden; // HIDDEN=b defaults to not visible
  
  enum ColFlags { // #BITS flags for data table columns
    DC_NONE		= 0, // #NO_BIT
    MARK 		= 0x0001, // #NO_SHOW used internally to mark columns prior to an operation -- columns that remain marked after all operations are unused, and may be removed -- users should not generally mess with this flag
    PIN 		= 0x0002, // #NO_SHOW protect this column from being automatically deleted according to the MARK scheme (see comment).  this is often not very easy for uers to find and use (columns to be saved should be listed explicitly in the context in which others are being used), so we are not exposing it to users, but it can be used internally for some reason
    SAVE_ROWS 		= 0x0004, // save the row data for this column in the internal format used when the entire object is saved (e.g., along with a project or whatever).  the column configuration etc is always saved, just not the rows of data if not on.
    SAVE_DATA 		= 0x0008, // save this column in the 'data' export format used by the SaveData and associated functions (e.g., as recorded in 'logs' of data from running programs)
    CALC 		= 0x0010, // #APPLY_IMMED calculate value of this column based on calc_expr expression
    KEY			= 0x0020, // indicates this is a key column; a table could have a compound key: set all that apply (note: currently not used by pdp, but may in the future)
    AUTO_KEY		= 0x0040, // automatically set a key from next keygen value when a row is added (scalar cols only) -- you will usually set KEY as well
    READ_ONLY		= 0x0080, // this column is read-only in the gui (helps protect keys or programmatically generated data items)
    PAT_4D		= 0x0100, // (4d cells only) displays cells in the same geometry as grouped network layers -- NOTE: data is still copied/pasted in clipboard in a 2d format
  };

  String		desc; // #NO_SAVE_EMPTY #EDIT_DIALOG optional description to help in documenting the use of this column
  ColFlags		col_flags; // #APPLY_IMMED flags for this column to indicate specific properties 
  int			col_idx; // #READ_ONLY #SHOW #NO_SAVE #NO_COPY the index of this column in the table
  bool			is_matrix;
  // #READ_ONLY #SAVE #SHOW 'true' if the cell is a matrix, not a scalar
  MatrixGeom		cell_geom;
  // #READ_ONLY #SAVE #SHOW for matrix cols, the geom of each cell
  ColCalcExpr		calc_expr; // #CONDEDIT_ON_col_flags:CALC expression for computing value of this column (only used if CALC flag is set)
  
  virtual taMatrix* 	AR() = 0;
  // #CAT_Access the matrix pointer -- NOTE: actual member should be called 'ar'
  virtual const taMatrix* 	AR() const = 0;
  // #CAT_Access const version of the matrix pointer

  /////////////////////////////////////////////
  // type of data

  virtual ValType 	valType() const = 0;
  // #CAT_Access the type of data in each element
  inline bool		isMatrix() const {return is_matrix;}
  // #CAT_Access true if data is a matrix
  virtual bool		isNumeric() const {return false;}
  // #CAT_Access true if data is float, int, or byte
  virtual bool		isString() const {return false;}
  // #CAT_Access true if data is string
  virtual bool		isImage() const;
  // #CAT_Access true if the cell contains an image
  virtual int		imageComponents() const;
  // #CAT_Access if an image, then: b&w=1, b&w+a=2, rgb=3, rgba=4

  inline bool		is_matrix_err()
  { return TestError(is_matrix, "is_matrix_err", "column has matrix data -- not supported"); }
  inline bool		not_matrix_err()
  { return TestError(!is_matrix, "not_matrix_err", "column does not have matrix data -- not supported"); }
  
  virtual int		cell_size() const
  { return (is_matrix) ? cell_geom.Product() : 1; }
  // #CAT_Access for matrix type, number of elements in each cell
  virtual int		cell_dims() const { return cell_geom.size; }
  // #CAT_Access for matrix type, number of dimensions in each cell
  virtual int		GetCellGeom(int dim) const { return cell_geom.SafeEl(dim); }
  // #CAT_Access for matrix type, size of given dim
  void			Get2DCellGeom(int& x, int& y) const; // for flat 2D access
  
  void			Get2DCellGeomGui(int& x, int& y,
     bool odd_y = true, int spc = 1) const;
  // #CAT_Access provides standardized 2d geom regardless of dimensionality (includes space for extra dimensions), odd_y = for odd dimension sizes, put extra dimension in y (else x): 3d = x, (y+1) * z (vertical time series of 2d patterns, +1=space), 4d = (x+1)*xx, (y+1)*yy (e.g., 2d groups of 2d patterns), 5d = vertical time series of 4d.
  
  int			rows() const { return AR()->frames(); }
  // #CAT_Access total number of rows of data within this column

  /////////////////////////////////////////////
  // Flags

  inline void		SetColFlag(ColFlags flg)   { col_flags = (ColFlags)(col_flags | flg); }
  // set data column flag state on
  inline void		ClearColFlag(ColFlags flg) { col_flags = (ColFlags)(col_flags & ~flg); }
  // clear data column flag state (set off)
  inline bool		HasColFlag(ColFlags flg) const { return (col_flags & flg); }
  // check if data column flag is set
  inline void		SetColFlagState(ColFlags flg, bool on)
  { if(on) SetColFlag(flg); else ClearColFlag(flg); }
  // set data column flag state according to on bool (if true, set flag, if false, clear it)

  /////////////////////////////////////////////////////////
  // Main data value access/modify (Get/Set) routines: for Programs and very general use

  const Variant GetVal(int row) const { return GetValAsVar_impl(row, 0); }
  // #CAT_Access get data of scalar type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last)
  bool	 	SetVal(const Variant& val, int row) 
  { return SetValAsVar_impl(val, row, 0); } 
  // #CAT_Modify get data of scalar type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last)

  const Variant GetMatrixVal(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsVarMDims(row, d0,d1,d2,d3); }
  // #CAT_Access get value of matrix type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last), d's are matrix dimension indicies
  bool	 	SetMatrixVal(const Variant& val, int row, 
			     int d0, int d1=0, int d2=0, int d3=0)
  { return SetValAsVarMDims(val, row, d0,d1,d2,d3); }
  // #CAT_Modify set value of matrix type, in Variant form (any data type, use for Programs), -ve row is from end (-1=last), d's are matrix dimension indicies

  bool	 	InitVals(const Variant& init_val);
  // #CAT_Modify initialize all values in this column to given value
  bool	 	InitValsToRowNo();
  // #CAT_Modify initialize all values in this column to be equal to the row number -- only valid for scalar (not matrix) columns

  int 		FindVal(const Variant& val, int st_row = 0) const;
  // #CAT_Access find row number for given value within column of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)

  /////////////////////////////////////////////
  // Get and Set access

  // row is absolute row in the matrix, not a DataTable row -- use the DataTable routines
  // -ve values are from end, and are valid for both low-level col access, and DataTable access
  
  const Variant GetValAsVar(int row) const {return GetValAsVar_impl(row, 0);}
  // #CAT_XpertAccess valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsVar(const Variant& val, int row) 
  { return SetValAsVar_impl(val, row, 0);} 
  // #CAT_XpertModify valid for all types, -ve row is from end (-1=last)
  const String 	GetValAsString(int row) const {return GetValAsString_impl(row, 0);}
  // #CAT_XpertAccess valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsString(const String& val, int row) 
  {return SetValAsString_impl(val, row, 0);} 
  // #CAT_XpertModify valid for all types, -ve row is from end (-1=last)
  float 	GetValAsFloat(int row) const {return GetValAsFloat_impl(row, 0);} 
  // #CAT_XpertAccess valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsFloat(float val, int row) 
  // #CAT_XpertModify valid only if type is float, -ve row is from end (-1=last)
  {return SetValAsFloat_impl(val, row, 0);} 
  double 	GetValAsDouble(int row) const {return GetValAsDouble_impl(row, 0);} 
  // #CAT_XpertAccess valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsDouble(double val, int row) 
  // #CAT_XpertModify valid only if type is double, -ve row is from end (-1=last)
  {return SetValAsDouble_impl(val, row, 0);} 
  int	 	GetValAsInt(int row) const {return GetValAsInt_impl(row, 0);} 
  // #CAT_XpertAccess valid if type is int or byte, -ve row is from end (-1=last)
  bool	 	SetValAsInt(int val, int row) 
  // #CAT_XpertModify valid if type is int or float, -ve row is from end (-1=last)
  {return SetValAsInt_impl(val, row, 0);} 
  byte	 	GetValAsByte(int row) const {return GetValAsByte_impl(row, 0);} 
  // #CAT_XpertAccess valid only if type is byte, -ve row is from end (-1=last)
  bool	 	SetValAsByte(byte val, int row) 
  // #CAT_XpertModify valid if type is numeric, -ve row is from end (-1=last)
  {return SetValAsByte_impl(val, row, 0);} 
    
  ///////////////////////////////////////////////////////////////
  // Matrix versions, cell index
  const Variant GetValAsVarM(int row, int cell) const {return GetValAsVar_impl(row, cell);} 
  // #CAT_XpertAccess get value as a variant (safe for all program usage), matrix version, valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsVarM(const Variant& val, int row, int cell) 
  { return SetValAsVar_impl(val, row, cell);} 
  // #CAT_XpertModify set value as a variant, matrix version, valid for all types, -ve row is from end (-1=last)
  const String 	GetValAsStringM(int row, int cell) const {return GetValAsString_impl(row, cell);} 
  // #CAT_XpertAccess get value as a string, matrix version, valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsStringM(const String& val, int row, int cell) 
  {return SetValAsString_impl(val, row, cell);} 
  // #CAT_XpertModify valid for all types, -ve row is from end (-1=last)
  float 	GetValAsFloatM(int row, int cell) const {return GetValAsFloat_impl(row, cell);} 
  // #CAT_XpertAccess valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsFloatM(float val, int row, int cell) 
  // #CAT_XpertModify valid only if type is float, -ve row is from end (-1=last)
  {return SetValAsFloat_impl(val, row, cell);} 
  double 	GetValAsDoubleM(int row, int cell) const {return GetValAsDouble_impl(row, cell);} 
  // #CAT_XpertAccess valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsDoubleM(double val, int row, int cell) 
  // #CAT_XpertModify valid only if type is float, -ve row is from end (-1=last)
  {return SetValAsDouble_impl(val, row, cell);} 
  int	 	GetValAsIntM(int row, int cell) const {return GetValAsInt_impl(row, cell);} 
  // #CAT_XpertAccess valid if type is int or byte, -ve row is from end (-1=last)
  bool	 	SetValAsIntM(int val, int row, int cell) 
  // #CAT_XpertModify valid if type is int or float, -ve row is from end (-1=last)
  {return SetValAsInt_impl(val, row, cell);} 
  byte	 	GetValAsByteM(int row, int cell) const {return GetValAsByte_impl(row, cell);} 
  // #CAT_XpertAccess valid only if type is byte, -ve row is from end (-1=last)
  bool	 	SetValAsByteM(byte val, int row, int cell) 
  // #CAT_XpertModify valid if type is numeric, -ve row is from end (-1=last)
  {return SetValAsByte_impl(val, row, cell);} 

  ///////////////////////////////////////////////////////////////
  // Matrix versions, with dimensions passed
  const Variant GetValAsVarMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsVar_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #CAT_XpertAccess valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsVarMDims(const Variant& val, int row, 
				 int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsVar_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));} 
  // #CAT_XpertModify valid for all types, -ve row is from end (-1=last)
  const String 	GetValAsStringMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsString_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #CAT_XpertAccess valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsStringMDims(const String& val, int row,
				    int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsString_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));} 
  // #CAT_XpertModify valid for all types, -ve row is from end (-1=last)
  float 	GetValAsFloatMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const 
  { return GetValAsFloat_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #CAT_XpertAccess valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsFloatMDims(float val, int row, int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsFloat_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));} 
  // #CAT_XpertModify valid only if type is float, -ve row is from end (-1=last)
  double 	GetValAsDoubleMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsDouble_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #CAT_XpertAccess valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsDoubleMDims(double val, int row, int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsDouble_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));} 
  // #CAT_XpertModify valid only if type is float, -ve row is from end (-1=last)
  int	 	GetValAsIntMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsInt_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #CAT_XpertAccess valid if type is int or byte, -ve row is from end (-1=last)
  bool	 	SetValAsIntMDims(int val, int row, int d0, int d1=0, int d2=0, int d3=0)
  {return SetValAsInt_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));} 
  // #CAT_XpertModify valid if type is int or float, -ve row is from end (-1=last)
  byte	 	GetValAsByteMDims(int row, int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsByte_impl(row, cell_geom.IndexFmDims(d0,d1,d2,d3)); }
  // #CAT_XpertAccess valid only if type is byte, -ve row is from end (-1=last)
  bool	 	SetValAsByteMDims(byte val, int row, int d0, int d1=0, int d2=0, int d3=0) 
  {return SetValAsByte_impl(val, row, cell_geom.IndexFmDims(d0,d1,d2,d3));} 
  // #CAT_XpertModify valid if type is numeric, -ve row is from end (-1=last)

  ///////////////////////////////////////////////////////////////
  // Matrix ops -- you must Ref/UnRef taMatrix return types
  taMatrix*	GetValAsMatrix(int row);
  // #CAT_XpertAccess gets the cell as a slice of the entire column (note: not const -- you can write it) -- must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it
  bool	 	SetValAsMatrix(const taMatrix* val, int row);
  // #CAT_XpertModify set the matrix cell from a same-sized matrix 
  taMatrix*	GetRangeAsMatrix(int st_row, int n_rows);
  // #CAT_XpertAccess gets a slice of the entire column from starting row for n_rows (note: not const -- you can write it) -- must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it
  bool		GetMinMaxScale(MinMax& mm);
  // #CAT_Display get min-max range of values contained within this column 
  
  bool		EnforceRows(int rows);
  // force data to have this many rows
  bool		InsertRows(int st_row, int n_rows);
  // insert n_rows starting at st_row

  /////////////////////////////////////////////
  // misc

  virtual String	ColStats();
  // #CAT_DataProc #MENU #MENU_ON_Column #USE_RVAL compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).

  int			displayWidth() const;
  // #CAT_Display low level display width, in chars, taken from options
  virtual int		maxColWidth() const {return -1;}
  // #CAT_Display aprox max number of columns, in characters, -1 if variable or unknown
  virtual bool		saveToDumpFile() const { return HasColFlag(SAVE_ROWS); }
  // #IGNORE whether to save col to internal dump format
  virtual bool		saveToDataFile() const { return HasColFlag(SAVE_DATA); }
  // #IGNORE whether to save col to external 'data' format

  static const KeyString key_val_type; // "val_type"
  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  override String	GetDisplayName() const; // #IGNORE we strip out the format characters
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return AR()->GetTA_Element(i, eltd); }
  
  DataTable*		dataTable();
  // root data table this col belongs to

  String	EncodeHeaderName(const MatrixGeom& dims) const;
  // encode header information for saving to text files
  static void 	DecodeHeaderName(String nm, String& base_nm, int& val_typ,
				 MatrixGeom& mat_idx, MatrixGeom& mat_geom);
  // decode header information for loading from text files 

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); 

  virtual void	Copy_NoData(const DataCol& cp);
  // #CAT_Copy copy the structure of the datatable without getting all the data
  virtual void 	CopyFromCol_Robust(const DataCol& cp); 
  // #CAT_Copy copy col, but don't change schema; robust to differences in type and format of the cells
  virtual void	CopyFromRow(int dest_row, const DataCol& cp, int src_row);
  // #CAT_Copy copy one row from source to given row in this object, assumes that the two have the same type and, if matrix, cell_size
  virtual void	CopyFromRow_Robust(int dest_row, const DataCol& cp, int src_row);
  // #CAT_Copy copy one row from source to given row in this object, robust to differences in type and format of the cells

  //IMPORTANT: DO NOT CHANGE THE FOLLOWING METHODS TO VIRTUAL
  void		ChangeColType(ValType new_type);
  // #CAT_ObjectMgmt #MENU change the type of the data in this col, without loss of data
  void		ChangeColCellGeom(const MatrixGeom& new_geom);
  // #CAT_ObjectMgmt #MENU #INIT_ARGVAL_ON_cell_geom change the cell geom of a mat col, or change from scalar to mat, without loss of data
  void		ChangeColCellGeomNs(int dims = 1, int d0=0, int d1=0, int d2=0,
				      int d3=0, int d4=0, int d5=0, int d6=0);
  // #CAT_ObjectMgmt change the cell geom of a mat col, or change from scalar to mat, without loss of data -- separate numbers input instead of overall geom object
  void		ChangeColMatToScalar();
  // #CAT_ObjectMgmt #MENU #CONFIRM change from a mat col to a scalar col
  
  override String 	GetTypeDecoKey() const { return "DataTable"; }

  virtual void Init(); // call this *after* creation, or in UAE, to assert matrix geometry
  override int	GetIndex() const {return col_idx;}
  override void	SetIndex(int value) {col_idx = (short)value;}
  override String GetDesc() const {return desc;}
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	InitLinks(); //note: ok to do own AR here, because never called in constructor
  void	CutLinks(); //note: NOT ok to do disown AR here, because called in destructor
  TA_ABSTRACT_BASEFUNS(DataCol);
  
protected:
  override void  UpdateAfterEdit_impl();
  // in all accessor routines, -ve row is from end (-1=last)
  int		IndexOfEl_Flat(int row, int cell) const; 
    // -ve row is from end (-1=last); note: returns -ve value if out of range, so must use with SafeEl_Flat
  int		IndexOfEl_Flat_Dims(int row, int d0, int d1=0, int d2=0, int d3=0) const; 
    // -ve row is from end (-1=last); note: returns -ve value if out of range, so must use with SafeEl_Flat
  virtual const Variant GetValAsVar_impl(int row, int cell) const; 
  virtual const String 	GetValAsString_impl(int row, int cell) const; 
  virtual double 	GetValAsDouble_impl(int row, int cell) const { return 0.0; }
  virtual float 	GetValAsFloat_impl(int row, int cell) const { return 0.0f; }
  virtual int	 	GetValAsInt_impl(int row, int cell) const { return 0; }
  virtual byte	 	GetValAsByte_impl(int row, int cell) const
  { return (byte)GetValAsInt_impl(row, cell); } 

  // these all return true if value is successfully set
  virtual bool	 SetValAsVar_impl(const Variant& val, int row, int cell);
  virtual bool	 SetValAsString_impl(const String& val, int row, int cell);
  virtual bool	 SetValAsDouble_impl(double val, int row, int cell) {return false;} 
  virtual bool	 SetValAsFloat_impl(float val, int row, int cell) { return false; }
  virtual bool	 SetValAsInt_impl(int val, int row, int cell)  { return false; }
  virtual bool	 SetValAsByte_impl(byte val, int row, int cell)
  { return SetValAsInt_impl((int)val, row, cell); } 
  
private:
  void 	Copy_(const DataCol& cp);
  void 	Copy_Common_(const DataCol& cp);
  void	Initialize();
  void	Destroy()	{CutLinks(); }; //
};
TA_SMART_PTRS(DataCol); //


/////////////////////////////////////////////////////////
//   DataTableCols -- group of DataCol
/////////////////////////////////////////////////////////

class TA_API DataTableCols: public taList<DataCol> {
  // ##CAT_Data columns of a datatable 
INHERITED(taList<DataCol>)
friend class DataTable;
public:
  override void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  
  override taBase* New(int n_objs = 1, TypeDef* typ = NULL)
   { return inherited::New(n_objs, typ); }
  // #NO_MENU create new data array -- note, this should not be used by users to create new colums -- use the datatable functions instead (NewCol, NewMatrixCol, etc)

  override int		NumListCols() const {return 3;}
  // name, val_type (float, etc.), disp_opts
  override String	GetColHeading(const KeyString& key) const;
  // header text for the indicated column
  override const KeyString GetListColKey(int col) const;
  
  override String 	GetTypeDecoKey() const { return "DataTable"; }

  TA_BASEFUNS(DataTableCols);
protected: // these guys must only be used by DataTable, but no external guys
  virtual void	Copy_NoData(const DataTableCols& cp);
  // #IGNORE #CAT_ObjectMgmt copy only the column structure, but no data, from other data table
  virtual void	CopyFromRow(int dest_row, const DataTableCols& cp, int src_row);
  // #IGNORE #CAT_ObjectMgmt copy one row from source to given row in this object: source must have exact same column structure as this!!

private:
  NOCOPY(DataTableCols)
  void	Initialize();
  void	Destroy()		{}
};
TA_SMART_PTRS(DataTableCols); //

/////////////////////////////////////////////////////////
//   DataTable
/////////////////////////////////////////////////////////

/*
  DataTable Notifications

  Structural Changes -- DCR_STRUCT_UPDATE_BEGIN..DCR_STRUCT_UPDATE_END
    this includes adding and removing data columns
  Row Adding/Removing -- DCR_DATA_UPDATE_BEGIN..DCR_DATA_UPDATE_END
    this includes adding and removing whole rows, or individual items to a row -- if the client
    code calls RowsAdding/RowsAdded

  Row Numbers
    - all fuctions using row numbers work properly for jagged tables, i.e. those to which
      columns have been added after some rows already exist
    - NOTE: functions with row numbers did NOT have this correct behavior in v3.2
    - unless noted, row<0 means access from the end, ex. -1 is last row
*/
class TA_API DataTable : public DataBlock_Idx {
  // ##TOKENS ##CAT_Data ##FILETYPE_DataTable ##EXT_dtbl ##DEF_CHILD_data ##DEF_CHILDNAME_Columns ##DUMP_LOAD_POST table of data of different types
INHERITED(DataBlock_Idx)
friend class DataTableCols;
friend class DataTableModel;
public:
  static int		idx_def_arg;
  // #HIDDEN #NO_SAVE default arg val for functions returning index

  enum DataFlags { // #BITS flags for data table
    DF_NONE		= 0, // #NO_BIT
    SAVE_ROWS 		= 0x0001, // save the row data associated with this table when saved with the project (column and other configuration information is always saved)
    HAS_CALCS 		= 0x0002, // #NO_SHOW at least one of the columns has CALC flag set
    AUTO_CALC		= 0x0004, // automatically calculate columns
  };

  enum AutoLoadMode {
    NO_AUTO_LOAD,		// do not automatically load data file
    AUTO_LOAD,			// automatically load a data file after loading
    PROMPT_LOAD,		// prompt about loading a data file after loading (if run in -nogui mode, it is automatically loaded)
  };
  
  /////////////////////////////////////////////////////////
  // 	Main datatable interface:
  int 			rows;
  // #READ_ONLY #NO_SAVE #SHOW the number of rows
  DataTableCols		data;
  // all the columns and actual data
  DataFlags		data_flags;
  // #APPLY_IMMED flags for various features and state of the data table
  AutoLoadMode		auto_load;
  // #APPLY_IMMED #CONDEDIT_OFF_data_flags:SAVE_ROWS whether to automatically load a data file from auto_load_file when data table object is loaded (only applicable when SAVE_ROWS is not active -- makes the project file smaller for large data tables, but the cost is that the project is no longer self contained)
  String		auto_load_file;
  // #CONDEDIT_OFF_auto_load:NO_AUTO_LOAD file to load data table from if AUTO_LOAD option is set (if file name has .dtbl extention, it is loaded using internal Load format, otherwise LoadData is used)
  Variant		keygen; // #HIDDEN #VARTYPE_READ_ONLY #GUI_READ_ONLY 64bit int used to generate keys; advance to get next key; only reset if all data reset

  cssProgSpace* 	calc_script;
  // #HIDDEN #NO_SAVE script object for performing column calculations
  taFiler*		log_file;
  // #NO_SAVE #HIDDEN file for logging data incrementally as it is written -- only for output.  a new line is written when WriteClose() (DataSink interface) is called.

  /////////////////////////////////////////////
  // Flags

  inline void		SetDataFlag(DataFlags flg)   { data_flags = (DataFlags)(data_flags | flg); }
  // #CAT_ObjectMgmt set data column flag state on
  inline void		ClearDataFlag(DataFlags flg) { data_flags = (DataFlags)(data_flags & ~flg); }
  // #CAT_ObjectMgmt clear data column flag state (set off)
  inline bool		HasDataFlag(DataFlags flg) const { return (data_flags & flg); }
  // #CAT_ObjectMgmt check if data column flag is set
  inline void		SetDataFlagState(DataFlags flg, bool on)
  { if(on) SetDataFlag(flg); else ClearDataFlag(flg); }
  // #CAT_ObjectMgmt set data column flag state according to on bool (if true, set flag, if false, clear it)

  /////////////////////////////////////////////////////////
  // columns

  int			cols() const { return data.size; }
  // #CAT_Columns number of columns
  override taList_impl* children_() {return &data;}
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return data.GetTA_Element(i, eltd); }

  virtual DataCol* 	NewCol(DataCol::ValType val_type, 
			       const String& col_nm);
  // #CAT_Columns create new scalar column of data of specified type
  virtual DataCol* 	NewCol_gui(DataCol::ValType val_type, 
				   const String& col_nm);
  // #MENU #MENU_ON_Columns #LABEL_NewCol #CAT_Columns create new scalar column of data of specified type
  virtual DataCol* 	NewColMatrix(DataCol::ValType val_type, const String& col_nm,
    int dims = 1, int d0=0, int d1=0, int d2=0, int d3=0, int d4=0, int d5=0, int d6=0);
  // #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  virtual DataCol* 	NewColMatrixN(DataCol::ValType val_type, 
				      const String& col_nm,  const MatrixGeom& cell_geom,
				      int& col_idx = idx_def_arg);
  // #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  virtual DataCol* 	NewColMatrixN_gui(DataCol::ValType val_type, 
					  const String& col_nm,  const MatrixGeom& cell_geom);
  // #MENU #MENU_ON_Columns #LABEL_NewColMatrix #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  
  virtual double_Data*	NewColDouble(const String& col_nm); 
  // #CAT_Columns create new column of double data
  virtual float_Data*	NewColFloat(const String& col_nm); 
  // #CAT_Columns create new column of floating point data
  virtual int_Data*	NewColInt(const String& col_nm); 	 
  // #CAT_Columns create new column of integer-level data (= narrow display, actually stored as float)
  virtual String_Data*	NewColString(const String& col_nm); 
  // #CAT_Columns create new column of string data

  virtual void		SetColName(const String& col_nm, int col);
  // #CAT_Columns set column name for given column
  virtual bool		RenameCol(const String& cur_nm, const String& new_nm);
  // #CAT_Columns rename column with current name cur_nm to new name new_nm (returns false if ccur_nm not found)

  virtual DataCol* 	FindColName(const String& col_nm, int& col_idx = idx_def_arg,
    bool err_msg = false) const;
  // #CAT_Columns #ARGC_1 find a column of the given name; if err_msg then generate an error if not found

  DataCol* 		FindMakeCol(const String& col_nm,
	ValType val_type = VT_FLOAT);
  // #CAT_Columns insures that a scalar column of the given name and val type exists, and return that col. 
  DataCol* 		FindMakeColMatrix(const String& col_nm,
	ValType val_type = VT_FLOAT, int dims = 1,
	int d0=0, int d1=0, int d2=0, int d3=0, int d4=0, int d5=0, int d6=0);
  // #CAT_Columns insures that a matrix column of the given name, val type, and dimensions exists, and returns that col. 
  DataCol* 		FindMakeColMatrixN(const String& col_nm,
	ValType val_type, const MatrixGeom& cell_geom,
	int& col_idx = idx_def_arg); // #IGNORE
  virtual DataCol* 	FindMakeColName(const String& col_nm, int& col_idx = idx_def_arg,
	ValType val_type = VT_FLOAT, int dims = 0,
	int d0=0, int d1=0, int d2=0, int d3=0, int d4=0, int d5=0, int d6=0);
  // #EXPERT #CAT_Columns find a column of the given name, val type, and dimension. if one does not exist, then create it.  Note that dims < 1 means make a scalar column, not a matrix
    
  void 			ChangeColTypeGeom_impl(DataCol* src, ValType val_type,
					       const MatrixGeom& geom);
    // #IGNORE impl func, called by Col or other funcs here; new is diff from exist; geom.dims=0 for scalar, otherwise matrix

  virtual void 		ChangeColTypeGeom(const String& col_nm, ValType val_type, int dims = 0,
					  int d0=0, int d1=0, int d2=0, int d3=0,
					  int d4=0, int d5=0, int d6=0);
  // #CAT_Columns change type and/or geometry of column with given name 
  
  virtual DataCol* 	GetColData(Variant col, bool quiet = false) const {
    if(col.isStringType()) return FindColName(col.toString(), idx_def_arg, !quiet);
    DataCol* rval = data.SafeEl(col.toInt());
    TestError(!quiet && !rval, "GetColData", "column number is out of range", col.toString());
    return rval;
  }
  // #CAT_Columns get col data for given column 
  virtual taMatrix*	GetColMatrix(int col) const
  { DataCol* da = GetColData(col); if (da) return da->AR(); else return NULL; }
  // #CAT_Columns get matrix for given column -- WARNING: this is NOT row-number safe 

  virtual bool 		ColMatchesChannelSpec(const DataCol* da, const ChannelSpec* cs);
  // #CAT_Columns returns 'true' if the col has the same name and a compatible data type
  virtual DataCol* 	NewColFromChannelSpec(ChannelSpec* cs)
  // #MENU_1N #CAT_Columns create new matrix column of data based on name/type in the data item (default is Variant)
  { if (cs) return NewColFromChannelSpec_impl(cs); else return NULL; }
    
  virtual DataCol*	GetColForChannelSpec(ChannelSpec* cs)
  // #MENU_1N #CAT_Columns find existing or create new matrix column of data based on name/type in the data item
    {if (cs) return GetColForChannelSpec_impl(cs); else return NULL;}
    
  virtual void		RemoveCol(int col);
  // #CAT_Columns removes indicated column; 'true' if removed
  void			RemoveAllCols()	{ Reset(); }
  // #CAT_Columns #MENU #MENU_ON_Columns #CONFIRM remove all columns (and data) -- this cannot be undone!
  virtual void		Reset();
  // #CAT_Columns remove all columns (and data) -- this cannot be undone!

  virtual void		MarkCols();
  // #CAT_Columns mark all cols before updating, for orphan deleting
  virtual void		RemoveOrphanCols();
  // #CAT_Columns removes all non-pinned marked cols
  

  /////////////////////////////////////////////////////////
  // rows

  virtual bool		hasData(int col, int row);
  // #CAT_Rows true if data at that cell
  bool			idx(int row_num, int col_size, int& act_idx) const
  { if (row_num < 0) row_num = rows + row_num;
    act_idx = col_size - (rows - row_num); return (act_idx >= 0 && act_idx < col_size); } 
  // #CAT_Rows calculates an actual index for a col item, based on the current #rows and size of that col; returns 'true' if act_idx >= 0 (i.e., if there is a data item for that column)
  inline bool		idx_err(int row_num, int col_size, int& act_idx) const {
    return !TestError(!idx(row_num, col_size, act_idx), "idx_err", "index out of range"); }
  // #IGNORE
  inline bool		idx_warn(int row_num, int col_size, int& act_idx) const {
    return !TestWarning(!idx(row_num, col_size, act_idx), "idx_err", "index out of range"); }
  // #IGNORE 
  virtual bool		RowInRangeNormalize(int& row);
  // #CAT_XpertRows normalizes row (if -ve) and tests result in range 
  virtual void		AllocRows(int n);
  // #CAT_Rows allocate space for at least n rows
  virtual int		AddBlankRow() 
  { if (AddRows(1)) {rd_itr = wr_itr = rows - 1; return wr_itr;} else return -1; }
  // #MENU #MENU_ON_Rows #CAT_Rows add a new row to the data table, sets read (source) and write (sink) index to this last row (as in ReadItem or WriteItem), so that subsequent data routines refer to this new row, and returns row #
  virtual bool		AddRows(int n = 1);
  // #MENU #CAT_Rows add n rows, returns true if successfully added
  virtual bool		InsertRows(int st_row, int n_rows=1);
  // #MENU #CAT_Rows insert n rows at starting row number, returns true if succesfully inserted
  
  virtual bool		RemoveRows(int st_row, int n_rows=1);
  // #MENU #MENU_ON_Rows #CAT_Rows Remove n rows of data, starting at st_row.  st_row = -1 means last row, and n_rows = -1 means remove from starting row to end
  virtual void		RemoveAllRows() { ResetData(); }
  // #MENU #CAT_Rows #CONFIRM remove all of the rows of data, but keep the column structure
  virtual void		EnforceRows(int n_rows);
  // #CAT_Rows ensure that there are exactly n_rows in the table, removing or adding as needed
  virtual bool		DuplicateRow(int row_no, int n_copies=1);
  // #MENU #CAT_Rows duplicate given row number, making given number of copies of it (adds new rows at the end)
  const Variant		GetColUserData(const String& name, int col) const;
  // #CAT_Config gets user data from the col
  void			SetColUserData(const String& name, const Variant& value, int col);
  // #CAT_Config sets user data into the col

  /////////////////////////////////////////////////////////
  // Main data value access/modify (Get/Set) routines: for Programs and very general use

  const Variant 	GetVal(Variant col, int row) const;
  // #CAT_Access get data of scalar type, in Variant form (any data type, use for Programs), for given column, row -- column can be specified as either integer index or a string that is then used to find the given column name
  bool 			SetVal(const Variant& val, Variant col, int row);
  // #CAT_Modify set data of scalar type, in Variant form (any data type, use for Programs), for given column, row -- column can be specified as either integer index or a string that is then used to find the given column name; returns 'true' if valid access and set is successful

  const Variant 	GetMatrixVal(Variant col, int row,
				     int d0, int d1=0, int d2=0, int d3=0) const;
  // #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies -- column can be specified as either integer index or a string that is then used to find the given column name
  bool 			SetMatrixVal(const Variant& val, Variant col, int row, 
				     int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies -- column can be specified as either integer index or a string that is then used to find the given column name; returns 'true' if valid access and set is successful

  const Variant 	GetMatrixFlatVal(Variant col, int row, int cell) const;
  // #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix flat cell index (regardless of dimensions) -- column can be specified as either integer index or a string that is then used to find the given column name
  bool 			SetMatrixFlatVal(const Variant& val, Variant col, int row, int cell);
  // #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix flat cell index (regardless of dimensions) -- column can be specified as either integer index or a string that is then used to find the given column name; returns 'true' if valid access and set is successful

  bool	 	InitVals(const Variant& init_val, Variant col);
  // #CAT_Modify initialize all values in given column to given value -- column can be specified as either integer index or a string that is then used to find the given column name
  bool	 	InitValsToRowNo(Variant col);
  // #CAT_Modify initialize all values in given column to be equal to the row number -- only valid for scalar (not matrix) columns -- column can be specified as either integer index or a string that is then used to find the given column name

  int 		FindVal(const Variant& val, Variant col, int st_row = 0) const;
  // #CAT_Access find row number for given value within column col of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end) -- column can be specified as either integer index or a string that is then used to find the given column name

  int 		FindMultiVal(int st_row, const Variant& val1, Variant col1,
			     const Variant& val2=0, Variant col2="",
			     const Variant& val3=0, Variant col3="",
			     const Variant& val4=0, Variant col4="",
			     const Variant& vall5=0, Variant col5="",
			     const Variant& val6=0, Variant col6="") const;
  // #CAT_Access find row number for multiple values across different columns of scalar type, starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end) -- columns can be specified as either integer index or a string that is then used to find the given column name

  /////////////////////////////
  // column name versions:

  const Variant 	GetValColName(const String& col_name, int row) const;
  // #CAT_XpertAccess get data of scalar type, in Variant form (any data type, use for Programs), for given column name, row
  bool 			SetValColName(const Variant& val, const String& col_name, int row);
  // #CAT_XpertModify set data of scalar type, in Variant form (any data type, use for Programs), for given column name, row; returns 'true' if valid access and set is successful

  const Variant 	GetMatrixValColName(const String& col_name, int row,
					    int d0, int d1=0, int d2=0, int d3=0) const;
  // #CAT_XpertAccess get data of matrix type, in Variant form (any data type, use for Programs), for given column name, row, and matrix dimension indicies
  bool 			SetMatrixValColName(const Variant& val, const String& col_name,
					    int row, int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertModify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies; returns 'true' if valid access and set is successful

  const Variant 	GetMatrixFlatValColName(const String& col_name, int row,
						int cell) const;
  // #CAT_XpertAccess get data of matrix type, in Variant form (any data type, use for Programs), for given column name, row, and flat matrix cell index (flat index into elements of the matrix, regardless of dimensionality)
  bool 			SetMatrixFlatValColName(const Variant& val, const String& col_name,
						int row, int cell);
  // #CAT_XpertModify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and flat matrix cell index (flat index into elements of the matrix, regardless of dimensionality); returns 'true' if valid access and set is successful

  bool	 	InitValsColName(const Variant& init_val, const String& col_name);
  // #CAT_XpertModify initialize all values in column of given name to given value
  bool	 	InitValsToRowNoColName(const String& col_name);
  // #CAT_XpertModify initialize all values in column of given name to be equal to the row number -- only valid for scalar (not matrix) columns

  int 		FindValColName(const Variant& val, const String& col_name, int st_row = 0) const;
  // #CAT_XpertAccess find row number for given value within column col of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)

  int 		FindMultiValColName(int st_row, const Variant& val1, const String& col_name1,
				    const Variant& val2=0, const String& col_name2="",
				    const Variant& val3=0, const String& col_name3="",
				    const Variant& val4=0, const String& col_name4="",
				    const Variant& vall5=0, const String& col_name5="",
				    const Variant& val6=0, const String& col_name6="") const;
  // #CAT_XpertAccess find row number for multiple values across different columns of scalar type, starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)

  /////////////////////////////
  // column and row name versions:

  const Variant 	GetValColRowName(const String& col_name, const String& row_col_name,
					 const Variant& row_value) const;
  // #CAT_XpertAccess get data of scalar type, in Variant form (any data type, use for Programs), for given column name, and row by looking up row_value in column named row_col_name
  bool 			SetValColRowName(const Variant& val, const String& col_name,
				      const String& row_col_name, const Variant& row_value);
  // #CAT_XpertModify set data of scalar type, in Variant form (any data type, use for Programs), for given column name, and row by looking up row_value in column named row_col_name; returns 'true' if valid access and set is successful

  const Variant 	GetMatrixValColRowName(const String& col_name,
				const String& row_col_name, const Variant& row_value,
					    int d0, int d1=0, int d2=0, int d3=0) const;
  // #CAT_XpertAccess get data of matrix type, in Variant form (any data type, use for Programs), for given column name, row, and matrix dimension indicies
  bool 			SetMatrixValColRowName(const Variant& val, const String& col_name,
				    const String& row_col_name, const Variant& row_value,
					       int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertModify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies; returns 'true' if valid access and set is successful

  const Variant 	GetMatrixFlatValColRowName(const String& col_name,
				const String& row_col_name, const Variant& row_value,
					    int cell) const;
  // #CAT_XpertAccess get data of matrix type, in Variant form (any data type, use for Programs), for given column name, row, and matrix cell index (flat index into matrix cells)
  bool 			SetMatrixFlatValColRowName(const Variant& val, const String& col_name,
				    const String& row_col_name, const Variant& row_value,
					       int cell);
  // #CAT_XpertModify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix cell index (flat index into matrix cells); returns 'true' if valid access and set is successful

  /////////////////////////////
  // column pointer versions, just for the gui:

  bool	 	InitValsCol(DataCol* col, const Variant& init_val)
  { return col->InitVals(init_val); }
  // #CAT_XpertColumns #MENU #MENU_ON_Columns #FROM_GROUP_data initialize all values in given column to given value
  bool	 	InitValsToRowNoCol(DataCol* col) 
  { return col->InitValsToRowNo(); }
  // #CAT_XpertModify #MENU #FROM_GROUP_data initialize all values in given column to be equal to the row number -- only valid for scalar (not matrix) columns
  int 		FindValCol(DataCol* col, const Variant& val, int st_row = 0) const 
  { return col->FindVal(val, st_row); }
  // #CAT_XpertAccess #MENU #FROM_GROUP_data #USE_RVAL find row number for given value within column col of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)
  int 		FindMultiValCol(int st_row, const Variant& val1, DataCol* col1,
				const Variant& val2=0, DataCol* col2=NULL,
				const Variant& val3=0, DataCol* col3=NULL,
				const Variant& val4=0, DataCol* col4=NULL,
				const Variant& vall5=0, DataCol* col5=NULL,
				const Variant& val6=0, DataCol* col6=NULL) const;
  // #CAT_XpertAccess #MENU #FROM_GROUP_data #USE_RVAL find row number for multiple values across different columns of scalar type, starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)


  /////////////////////////////////////////////////////////
  // Expert data value access routines: optimized for different types

  ///////////////
  // Scalar

  const Variant 	GetValAsVar(int col, int row) const;
  // #CAT_XpertAccess get data of scalar type, in Variant form, for given column, row
  bool 			SetValAsVar(const Variant& val, int col, int row);
  // #CAT_XpertModify set data of scalar type, in Variant form, for given column, row; returns 'true' if valid access and set is successful
  double 		GetValAsDouble(int col, int row);
  // #CAT_XpertAccess get data of scalar type, in double form, for given col, row; if data is NULL, then 0 is returned
  bool 			SetValAsDouble(double val, int col, int row);
  // #CAT_XpertModify set data of scalar type, in double form, for given column, row; does nothing if no cell' 'true' if set
  float 		GetValAsFloat(int col, int row);
  // #CAT_XpertAccess get data of scalar type, in float form, for given col, row; if data is NULL, then 0 is returned
  bool 			SetValAsFloat(float val, int col, int row);
  // #CAT_XpertModify set data of scalar type, in float form, for given column, row; does nothing if no cell' 'true' if set
  int 			GetValAsInt(int col, int row);
  // #CAT_XpertAccess get data of scalar type, in int form, for given col, row; if data is NULL, then 0 is returned
  bool 			SetValAsInt(int val, int col, int row);
  // #CAT_XpertModify set data of scalar type, in int form, for given column, row; does nothing if no cell' 'true' if set
  const String 		GetValAsString(int col, int row) const;
  // #CAT_XpertAccess get data of scalar type, in String form, for given column, row; if data is NULL, then "n/a" is returned
  bool 			SetValAsString(const String& val, int col, int row);
  // #CAT_XpertModify set data of scalar type, in String form, for given column, row; does nothing if no cell; 'true if set

  ///////////////
  // Matrix, Flat Idx

  const Variant 	GetValAsVarM(int col, int row, int cell) const;
  // #CAT_XpertAccess get data of matrix type, in Variant form, for given column, row, and cell (flat index) in matrix
  bool 			SetValAsVarM(const Variant& val, int col, int row, int cell);
  // #CAT_XpertModify set data of matrix type, in Variant form, for given column, row, and cell (flat index) in matrix; returns 'true' if valid access and set is successful
  double 		GetValAsDoubleM(int col, int row, int cell);
  // #CAT_XpertAccess get data of matrix type, in double form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool 			SetValAsDoubleM(double val, int col, int row, int cell);
  // #CAT_XpertModify set data of matrix type, in double form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  float 		GetValAsFloatM(int col, int row, int cell);
  // #CAT_XpertAccess get data of matrix type, in float form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool 			SetValAsFloatM(float val, int col, int row, int cell);
  // #CAT_XpertModify set data of matrix type, in float form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  int 			GetValAsIntM(int col, int row, int cell);
  // #CAT_XpertAccess get data of matrix type, in int form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool 			SetValAsIntM(int val, int col, int row, int cell);
  // #CAT_XpertModify set data of matrix type, in int form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  const String 		GetValAsStringM(int col, int row, int cell,
     bool na = true) const;
  // #CAT_XpertAccess get data of matrix type, in String form, for given column, row, and cell (flat index) in matrix; if data is NULL, then na="n/a" else "" is returned
  bool 			SetValAsStringM(const String& val, int col, int row, int cell);
  // #CAT_XpertModify set data of matrix type, in String form, for given column, row, and cell (flat index) in matrix; does nothing if no cell; 'true if set

  ///////////////
  // Matrix, Dims

  const Variant 	GetValAsVarMDims(int col, int row,
					 int d0, int d1=0, int d2=0, int d3=0) const;
  // #CAT_XpertAccess get data of matrix type, in Variant form, for given column, row, and matrix dimension indicies
  bool 			SetValAsVarMDims(const Variant& val, int col, int row, 
					 int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertModify set data of matrix type, in Variant form, for given column, row, and matrix dimension indicies; returns 'true' if valid access and set is successful

  double 		GetValAsDoubleMDims(int col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertAccess get data of matrix type, in double form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool 			SetValAsDoubleMDims(double val, int col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertModify set data of matrix type, in double form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  float 		GetValAsFloatMDims(int col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertAccess get data of matrix type, in float form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool 			SetValAsFloatMDims(float val, int col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertModify set data of matrix type, in float form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  int 			GetValAsIntMDims(int col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertAccess get data of matrix type, in int form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool 			SetValAsIntMDims(int val, int col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertModify set data of matrix type, in int form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  const String 		GetValAsStringMDims(int col, int row, int d0, int d1=0, int d2=0, int d3=0,
     bool na = true) const;
  // #CAT_XpertAccess get data of matrix type, in String form, for given column, row, and cell (flat index) in matrix; if data is NULL, then na="n/a" else "" is returned
  bool 			SetValAsStringMDims(const String& val, int col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_XpertModify set data of matrix type, in String form, for given column, row, and cell (flat index) in matrix; does nothing if no cell; 'true if set

  //////////////////////
  // 	Entire Matrix

  taMatrix*	 	GetValAsMatrix(int col, int row);
  // #CAT_XpertAccess get data of matrix type, in Matrix form (one frame), for given column, row; Invalid/NULL if no cell; must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it; note: not const because you can write it
  taMatrix*	 	GetValAsMatrixColName(const String& col_name, int row);
  // #CAT_XpertAccess get data of matrix type, in Matrix form (one frame), for given column, row; Invalid/NULL if no cell; must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it; note: not const because you can write it
  bool 			SetValAsMatrix(const taMatrix* val, int col, int row);
  // #CAT_XpertModify  set data of any type, in Variant form, for given column, row; does nothing if no cell; 'true' if set
  bool 			SetValAsMatrixColName(const taMatrix* val, const String& col_name, int row);
  // #CAT_XpertModify  set data of any type, in Variant form, for given column, row; does nothing if no cell; 'true' if set
  taMatrix*	 	GetRangeAsMatrix(int col, int st_row, int n_rows);
  // #CAT_XpertAccess get data as a Matrix for a range of rows, for given column, st_row, and n_rows; row; Invalid/NULL if no cell; must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it; note: not const because you can write it

  int 			GetMaxCellRows(int col_fr, int col_to); // #IGNORE get the max muber of cell rows in this col range (used for clip operations)
  void			GetFlatGeom(const CellRange& cr, int& tot_cols, 
   int& max_cell_rows); // #IGNORE get the total flat cols and max rows per cell; used for TSV output

  String		HeaderToTSV(); // #IGNORE for tsv save
  String		RangeToTSV(const CellRange& cr); // #IGNORE for clip operations

  /////////////////////////////////////////////////////////
  // saving/loading (file)

  enum Delimiters {
    TAB,
    SPACE,
    COMMA,
  };

  // dumping and loading -- see .cpp file for detailed format information, not saved as standard taBase obj
  virtual void 		SaveData_strm(ostream& strm, Delimiters delim = TAB,
				      bool quote_str = true, bool save_headers=true);
  // #CAT_XpertFile #EXT_dat,tsv,csv,txt,log saves data, one line per rec, with delimiter between columns, and optionally quoting strings -- if save-headers then special _H: formatted column header information is saved and data rows are marked with _D:
  virtual void 		SaveHeader_strm(ostream& strm, Delimiters delim = TAB,
					bool row_mark = true, int col_fr = 0, int col_to = -1);
  // #CAT_XpertFile #EXT_dat,tsv,csv,txt,log saves header information, with delimiter between columns, and optionally quoting strings
  virtual void 		SaveDataRow_strm(ostream& strm, int row=-1,
				 Delimiters delim = TAB, bool quote_str = true,
				 bool row_mark = true, int col_fr = 0, int col_to = -1); 
  // #CAT_XpertFile #EXT_dat,tsv,csv,txt,log saves one row of data (-1 = last row), with delimiter between columns, and optionally quoting strings; -ve cols/rows are relative to end -- if row_mark then mark data rows with _D: at start (to differentiate from _H: headers)
  virtual void 		SaveDataRows_strm(ostream& strm, Delimiters delim = TAB,
					  bool quote_str = true, bool row_mark=true); 
  // #CAT_XpertFile #EXT_dat,tsv,csv,txt,log saves all rows of data (no header) with delimiter between columns, and optionally quoting strings -- if row_mark then mark data rows with _D: at start (to differentiate from _H: headers)

  virtual void 		SaveData(const String& fname="", Delimiters delim = TAB,
				 bool quote_str = true, bool save_headers=true);
  // #CAT_File #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_dat,tsv,csv,txt,log #FILE_DIALOG_SAVE saves data, one line per rec, with delimiter between columns, and optionally quoting strings; leave fname empty to pick from file chooser -- if save-headers then special _H: formatted column header information is saved and data rows are marked with _D:
  virtual void 		AppendData(const String& fname="", Delimiters delim = TAB,
				   bool quote_str = true, bool row_mark=true); 
  // #CAT_File #MENU #EXT_dat,tsv,csv,txt,log  #FILE_DIALOG_APPEND appends all of current datatable data to given file (does not output header; file assumed to be of same data structure -- if data_tag then mark data rows with _D: at start (to differentiate from _H: headers)
  virtual void 		SaveHeader(const String& fname="", Delimiters delim = TAB);
  // #CAT_File #MENU #EXT_dat,tsv,csv,txt,log saves #FILE_DIALOG_SAVE header information, with delimiter between columns, and optionally quoting strings; leave fname empty to pick from file chooser
  virtual void 		SaveDataRow(const String& fname="", int row=-1, Delimiters delim = TAB,
				    bool quote_str = true, bool row_mark=true); 
  // #CAT_File #MENU #EXT_dat,tsv,csv,txt,log #FILE_DIALOG_SAVE saves one row of data (-1 = last row), with delimiter between columns, and optionally quoting strings; leave fname empty to pick from file chooser -- if row_mark then mark data rows with _D: at start (to differentiate from _H: headers)

  virtual void		SaveDataLog(const String& fname="", bool append=false,
				    bool dmem_proc_0 = true);
  // #CAT_File #MENU #MENU_SEP_BEFORE #EXT_dat,tsv,csv,txt,log #FILE_DIALOG_SAVE incrementally save each new row of data that is written to the datatable (at WriteClose()) to given file.  writes the header first if not appending to existing file.  if running under demem, dmem_proc_0 determines if only the first processor writes to the log file, or if all processors write
  virtual void		CloseDataLog();
  // #CAT_File #MENU close the data log file if it was previously open
  virtual bool		WriteDataLogRow();
  // #CAT_File write the current row to the data log, if it is open (returns true if successfully wrote) -- this is automatically called by WriteClose 

  static char		GetDelim(Delimiters delim);
  // #IGNORE get delimiter from enum
  static int		ReadTillDelim(istream& strm, String& str, const char delim, bool quote_str);
  // #IGNORE util function to read from stream into str until delim or newline or EOF

protected:
  static int_Array	load_col_idx; // #IGNORE mapping of column numbers in data load to column indexes based on header name matches
  static int_Array	load_mat_idx; // #IGNORE mapping of column numbers in data to matrix indicies in columns, based on header info

  virtual bool 		CopyCell_impl(DataCol* dar, int dest_row,
    const DataTable& src, DataCol* sar, int src_row); // #IGNORE
  virtual int 		LoadHeader_impl(istream& strm, Delimiters delim = TAB);
  // #IGNORE #CAT_File #EXT_dat,tsv,csv,txt,log loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end)
  virtual int 		LoadDataRow_impl(istream& strm, Delimiters delim = TAB,
					 bool quote_str = true);
  // #IGNORE #CAT_File #EXT_dat,tsv,csv,txt,log load one row of data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings (returns EOF if strm is at end)
  bool 			SetValAsMatrix_impl(const taMatrix* val, DataCol* da, int row);

public:
  virtual void 		LoadData_strm(istream& strm, Delimiters delim = TAB,
				 bool quote_str = true, int max_recs = -1);
  // #CAT_XpertFile #EXT_dat,tsv,csv,txt,log loads data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings
  virtual int 		LoadHeader_strm(istream& strm, Delimiters delim = TAB);
  // #CAT_XpertFile #EXT_dat,tsv,csv,txt,log loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end)
  virtual int 		LoadDataRow_strm(istream& strm, Delimiters delim = TAB,
    bool quote_str = true);
  // #CAT_XpertFile #EXT_dat,tsv,csv,txt,log load one row of data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings (returns EOF if strm is at end)
  virtual void 		LoadData(const String& fname, Delimiters delim = TAB,
				 bool quote_str = true, int max_recs = -1, bool reset_first=false);
  // #CAT_File #MENU #MENU_SEP_BEFORE #EXT_dat,tsv,csv,txt,log  #FILE_DIALOG_LOAD loads data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings, reset_first = remove any existing data prior to loading
  virtual int 		LoadHeader(const String& fname, Delimiters delim = TAB);
  // #CAT_File #EXT_dat,tsv,csv,txt,log loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end)
  virtual int 		LoadDataRow(const String& fname, Delimiters delim = TAB, bool quote_str = true);
  // #CAT_File #MENU #EXT_dat,tsv,csv,txt,log  #FILE_DIALOG_LOAD load one row of data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings (returns EOF if strm is at end)
  
 
  /////////////////////////////////////////////////////////
  // calculated column values

  virtual bool		UpdateColCalcs();
  // #IGNORE update column calculations
  virtual bool		CalcLastRow();
  // #IGNORE if HAS_CALCS, does calculations for last row of data -- called by WriteClose

  virtual bool		CheckForCalcs();
  // #IGNORE see if any columns have CALC flag -- sets HAS_CALCS flag -- returns state of flag
  virtual void		InitCalcScript();
  // #IGNORE initialize the calc_script for computing column calculations
  virtual bool		CalcAllRows_impl();
  // #IGNORE perform calculations for all rows of data (calls InitCalcScript to make sure)
  virtual bool		CalcAllRows();
  // #CAT_Calc #BUTTON #GHOST_OFF_data_flags:HAS_CALCS perform calculations for all rows of data (updates after)
  virtual void		CalcRowCodeGen(String& code_str);
  // #IGNORE generate code for computing one row worth of data, with assumed 'int row' variable specifying row
  virtual bool		CalcRow(int row);
  // #CAT_Calc perform calculations for given row of data (calls InitCalcScript to make sure)

  /////////////////////////////////////////////////////////
  // core data processing -- see taDataProc for more elaborate options

  virtual void		Sort(Variant col1, bool ascending1 = true,
			     Variant col2 = -1, bool ascending2 = true,
			     Variant col3 = -1, bool ascending3 = true,
			     Variant col4 = -1, bool ascending4 = true,
			     Variant col5 = -1, bool ascending5 = true,
			     Variant col6 = -1, bool ascending6 = true);
  // #CAT_DataProc sort table according to selected columns of data: NOTE that this modifies this table and currently cannot be undone -- make a duplicate table first if you want to save the original data!
  virtual void		SortColName(const String& col1, bool ascending1 = true,
				    const String& col2 = "", bool ascending2 = true,
				    const String& col3 = "", bool ascending3 = true,
				    const String& col4 = "", bool ascending4 = true,
				    const String& col5 = "", bool ascending5 = true,
				    const String& col6 = "", bool ascending6 = true);
  // #CAT_XpertDataProc sort table according to selected columns of data: NOTE that this modifies this table and currently cannot be undone -- make a duplicate table first if you want to save the original data!
  virtual void		SortCol(DataCol* col1, bool ascending1 = true,
				DataCol* col2 = NULL, bool ascending2 = true,
				DataCol* col3 = NULL, bool ascending3 = true,
				DataCol* col4 = NULL, bool ascending4 = true,
				DataCol* col5 = NULL, bool ascending5 = true,
				DataCol* col6 = NULL, bool ascending6 = true);
  // #CAT_XpertDataProc #MENU #MENU_ON_DataProc #LABEL_Sort #FROM_GROUP_data #NULL_OK sort table according to selected columns of data: NOTE that this modifies this table and currently cannot be undone -- make a duplicate table first if you want to save the original data!
  virtual bool		Filter(const String& filter_expr);
  // #CAT_DataProc #MENU #FROM_GROUP_data filter (select) table rows by applying given expression -- if it evaluates to true, the row is included, and otherwise it is removed.  refer to current colum values by name.  NOTE that this modifies this table and currently cannot be undone -- make a duplicate table first if you want to save the original data!
  virtual bool		GroupMeanSEM(DataTable* dest_data,
				     DataCol* gp_col1, DataCol* gp_col2 = NULL,
				     DataCol* gp_col3 = NULL, DataCol* gp_col4 = NULL);
  // #CAT_DataProc #MENU #NULL_OK #NULL_TEXT_0_NewDataTable #FROM_GROUP_1_data #FROM_GROUP_2_data #FROM_GROUP_3_data #FROM_GROUP_4_data groups data according to given columns in hierarchical fashion (gp_col2 is subgrouped within gp_col1, etc), and compute the Mean and Standard Error of the Mean (SEM) for any other numerical columns of data -- results go in dest_data table (new table created if NULL)
  virtual String	ColStats(Variant col);
  // #CAT_DataProc compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).-- column can be specified as either integer index or a string that is then used to find the given column name
  virtual String	ColStatsCol(DataCol* col);
  // #CAT_XpertDataProc #MENU #FROM_GROUP_data #LABEL_ColStats #USE_RVAL compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).
  virtual String	ColStatsName(const String& col_name);
  // #CAT_XpertDataProc compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).

  /////////////////////////////////////////////////////////
  // misc funs

  virtual GridTableView* NewGridView(T3DataViewFrame* fr = NULL);
  // #NULL_OK_0 #NULL_TEXT_0_NewFrame #MENU #MENU_ON_View #MENU_CONTEXT #CAT_Display open a grid view (graphical rows and columns) of this table (NULL=use existing empty frame if any, else make new frame)
  virtual GraphTableView* NewGraphView(T3DataViewFrame* fr = NULL);
  // #NULL_OK_0 #NULL_TEXT_0_NewFrame #MENU #MENU_CONTEXT #CAT_Display open a graph view of this table (NULL=use existing empty frame if any, else make new frame)

  virtual GridTableView* FindMakeGridView(T3DataViewFrame* fr = NULL);
  // #CAT_Display find existing or make a new grid view (graphical rows and columns) of this table (NULL=use existing empty frame if any, else make new frame)
  virtual GraphTableView* FindMakeGraphView(T3DataViewFrame* fr = NULL);
  // #CAT_Display find existing or make a new graph view of this table (NULL=use existing empty frame if any, else make new frame)

  virtual int  		MinLength();		// #IGNORE
  virtual int  		MaxLength();		// #IGNORE

  virtual void	Copy_NoData(const DataTable& cp);
  // #CAT_Copy copy only the column structure, but no data, from other data table
  virtual void	CopyFromRow(int dest_row, const DataTable& cp, int src_row);
  // #CAT_Copy copy one row from source to given row in this object: source must have exact same column structure as this!!
  virtual bool	CopyCell(Variant dest_col, int dest_row, const DataTable& src, Variant src_col, int src_row);
  // #CAT_Copy copy one cell (indexed by column, row) from source to this data table in given col,row cell -- is robust to differences in type and matrix sizing (returns false if not successful) -- column can be specified as either integer index or a string that is then used to find the given column name
  virtual bool	CopyColRow(int dest_col, int dest_row, const DataTable& src, int src_col, int src_row) { return CopyCell(dest_col, dest_row, src, src_col, src_row); }
  // #CAT_XpertObsolete this is an obsolete name for CopyCell -- use CopyCell instead
  bool		CopyCellName(const String& dest_col_name, int dest_row, const DataTable& src, const String& src_col_name, int src_row);
  // #CAT_XpertCopy copy one cell (indexed by column, row) from source to this data table in given col,row cell -- is robust to differences in type and matrix sizing (returns false if not successful)

  virtual void	UniqueColNames();
  // #CAT_ObjectMgmt ensure that the column names are all unique (adds _n for repeats)

  virtual void	DMem_ShareRows(MPI_Comm comm, int n_rows = 1);
  // #CAT_DMem #IGNORE Share the given number of rows from the end of the table (-1 = all rows) across processors in given communicator -- everyone gets the data from all processors as new rows in the table

  virtual bool		AutoLoadData();
  // #IGNORE perform auto loading of data from file when data table is loaded (called by PostLoadAutos) -- true if loaded
  override void		Dump_Load_post();

  override void		Dump_Load_pre();
  override int 		Dump_Load_Value(istream& strm, TAPtr par);
  override String 	GetTypeDecoKey() const { return "DataTable"; }

  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(DataTable); //

protected: 
  override void	UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);

  /////////////////////////////////////////////////////////
  // IMPL

  void			RowsAdding(int n, bool begin);
  // indicate beginning and end of row adding -- you have to pass the same n each time; NOT nestable
  bool			NewColValid(const String& col_nm,
    const MatrixGeom* cell_geom = NULL);
  // returns true if valid new col spec; posts modal err dialog if in gui call; geom NULL if scalar col

public:
  /////////////////////////////////////////////////////////
  // DataBlock interface and common routines: see ta_data.h for details

  override DBOptions	dbOptions() const
  { return (DBOptions)(DB_IND_SEQ_SRC_SNK | DB_SINK_DYNAMIC); } 
  override int		ItemCount() const { return rows; } 

protected:
  /////////////////////////////////////////////////////////
  // DataBlock implementation
  inline int		ChannelCount() const {return data.size; }
  inline const String	ChannelName(int chan) const
  { DataCol* da = data.SafeEl(chan);
    if (da) return da->name; else return _nilString; }

public:
  /////////////////////////////////////////////////////////
  // DataSource interface

  override int		SourceChannelCount() const { return ChannelCount(); }
  override const String	SourceChannelName(int chan) const { return ChannelName(chan); }
  override void		ResetData();
  // #CAT_Rows deletes all the data (rows), but keeps the column structure -- this cannot be undone!

protected:
  /////////////////////////////////////////////////////////
  // DataSource implementation
  override const Variant GetData_impl(int chan)
  { return GetValAsVar(chan, rd_itr);}
  override taMatrix*	GetMatrixData_impl(int chan);

public:
  /////////////////////////////////////////////////////////
  // DataSink interface
  override int		SinkChannelCount() const {return ChannelCount();}
  override const String	SinkChannelName(int chan) const {return ChannelName(chan);}
  override bool		AddSinkChannel(ChannelSpec* cs); 
  override bool		AssertSinkChannel(ChannelSpec* cs);

protected:
  /////////////////////////////////////////////////////////
  // DataSink implementation
  override bool		AddItem_impl(int n) {return AddRows(n);}
  override void		DeleteSinkChannel_impl(int chan) {RemoveCol(chan);}
  override taMatrix*	GetSinkMatrix_impl(int chan) 
  { return GetValAsMatrix(chan, wr_itr);} //note: DS refs it
  override bool		SetData_impl(const Variant& data, int chan) 
  { return SetValAsVar(data, chan, wr_itr);}
  override bool		SetMatrixData_impl(const taMatrix* data, int chan) 
  { return SetValAsMatrix(data, chan, wr_itr);}
  override void		WriteClose_impl();

protected:
  DataCol*	NewCol_impl(DataCol::ValType val_type, 
    const String& col_nm, int& col_idx = idx_def_arg);
  // low-level create routine, shared by scalar and matrix creation, must be wrapped in StructUpdate
  DataCol*	GetColForChannelSpec_impl(ChannelSpec* cs);
  DataCol*	NewColFromChannelSpec_impl(ChannelSpec* cs);
  
private:
  void 	Copy_(const DataTable& cp);
  void	Initialize();
  void	Destroy();
};
TA_SMART_PTRS(DataTable); //


/////////////////////////////////////////////////////////
//   DataTable Group
/////////////////////////////////////////////////////////

class TA_API DataTable_Group : public taGroup<DataTable> {
  // ##CAT_Data group of data objects
INHERITED(taGroup<DataTable>)
public:
  override String 	GetTypeDecoKey() const { return "DataTable"; }

  TA_BASEFUNS(DataTable_Group);
private:
  NOCOPY(DataTable_Group)
  void	Initialize() 		{ SetBaseType(&TA_DataTable); }
  void 	Destroy()		{ };
};
TA_SMART_PTRS(DataTable_Group);


/////////////////////////////////////////////////////////
//   DataCol templates
/////////////////////////////////////////////////////////

template<class T> 
class DataColTp : public DataCol {
  // #VIRT_BASE #NO_INSTANCE template for common elements
INHERITED(DataCol)
public:
  override taMatrix* 	AR()	{ return &ar; } // the array pointer
  override const taMatrix* AR() const { return &ar; } // the array pointer

  void	CutLinks()
    {ar.CutLinks(); DataCol::CutLinks();}
  TA_TMPLT_ABSTRACT_BASEFUNS(DataColTp, T); //
public: //DO NOT ACCESS DIRECTLY
  T		ar;		// #NO_SHOW  the array itself
private:
  void	Copy_(const DataColTp<T>& cp)  { ar = cp.ar; }
  void	Initialize()		{}
  void	Destroy()		{ CutLinks(); }
};

class TA_API String_Data : public DataColTp<String_Matrix> {
  // string data
INHERITED(DataColTp<String_Matrix>)
friend class DataTable;
public:
  override bool		isString() const {return true;} 
  override ValType 	valType() const  {return VT_STRING;}

  TA_BASEFUNS_NOCOPY(String_Data);

protected:
  override double 	GetValAsDouble_impl(int row, int cell) const
  { return (double)GetValAsString_impl(row, cell); }
  override float 	GetValAsFloat_impl(int row, int cell) const
  { return (float)GetValAsString_impl(row, cell); }
  override int	 	GetValAsInt_impl(int row, int cell) const
  { return (int)GetValAsString_impl(row, cell); }
  override byte	 	GetValAsByte_impl(int row, int cell) const
  { return GetValAsString_impl(row, cell)[0]; } 

  override bool	 SetValAsDouble_impl(double val, int row, int cell)
  { return SetValAsString_impl((String)val, row, cell); }
  override bool	 SetValAsFloat_impl(float val, int row, int cell)
  { return SetValAsString_impl((String)val, row, cell); }
  override bool	 SetValAsInt_impl(int val, int row, int cell)
  { return SetValAsString_impl((String)val, row, cell); }
  override bool	 SetValAsByte_impl(byte val, int row, int cell)
  { return SetValAsString_impl((String)val, row, cell); }

private:
  void	Initialize() {}
  void	Destroy() {}
};
TA_SMART_PTRS(String_Data);


class TA_API Variant_Data : public DataColTp<Variant_Matrix> {
  // Variant data
INHERITED(DataColTp<Variant_Matrix>)
friend class DataTable;
public:
  override ValType 	valType() const  {return VT_VARIANT;}

  TA_BASEFUNS_NOCOPY(Variant_Data);

protected:
  override const Variant GetValAsVar_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override bool	 SetValAsVar_impl(const Variant& val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }

  override double GetValAsDouble_impl(int row, int cell) const
  { return GetValAsVar_impl(row, cell).toDouble(); }
  override float GetValAsFloat_impl(int row, int cell) const
  { return (float)GetValAsVar_impl(row, cell).toFloat(); }
  override int	 GetValAsInt_impl(int row, int cell) const
  { return (int)GetValAsVar_impl(row, cell).toInt(); }

  override bool	 SetValAsDouble_impl(double val, int row, int cell)
  { return SetValAsVar_impl(val, row, cell); }
  override bool	 SetValAsFloat_impl(float val, int row, int cell)
  { return SetValAsVar_impl(val, row, cell); }
  override bool	 SetValAsInt_impl(int val, int row, int cell)
  { return SetValAsVar_impl(val, row, cell); }

private:
  void	Initialize() {}
  void	Destroy() {}
};
TA_SMART_PTRS(Variant_Data); //


class TA_API double_Data : public DataColTp<double_Matrix> {
  // doubleing point data
INHERITED(DataColTp<double_Matrix>)
friend class DataTable;
public:
  override bool		isNumeric() const {return true;} 
  override int		maxColWidth() const {return 15;} // assumes sign, int: 15 dig's; double: 14 dig's, decimal point
  override ValType 	valType() const {return VT_DOUBLE;}

  TA_BASEFUNS_NOCOPY(double_Data);
  
protected:
  override double GetValAsDouble_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override float GetValAsFloat_impl(int row, int cell) const
  { return (float)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override int 	GetValAsInt_impl(int row, int cell) const
  { return (int)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }

  override bool	SetValAsDouble_impl(double val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	SetValAsFloat_impl(float val, int row, int cell)
  { ar.Set_Flat((double)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	SetValAsInt_impl(int val, int row, int cell)
  { ar.Set_Flat((double)val, IndexOfEl_Flat(row, cell)); return true; }

private:
  void	Initialize() {}
  void	Destroy() {}
};
TA_SMART_PTRS(double_Data); //

class TA_API float_Data : public DataColTp<float_Matrix> {
  // floating point data
INHERITED(DataColTp<float_Matrix>)
friend class DataTable;
public:
  override bool		isNumeric() const {return true;} 
  override int		maxColWidth() const {return 7;} // assumes sign, int: 6 dig's; float: 5 dig's, decimal point
  override ValType 	valType() const {return VT_FLOAT;}

  TA_BASEFUNS_NOCOPY(float_Data);
  
protected:
  override double 	GetValAsDouble_impl(int row, int cell) const
  { return (double)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override float 	GetValAsFloat_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override int 		GetValAsInt_impl(int row, int cell) const
  { return (int)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }

  override bool	 	SetValAsDouble_impl(double val, int row, int cell)
  { ar.Set_Flat((float)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	 	SetValAsFloat_impl(float val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	 	SetValAsInt_impl(int val, int row, int cell)
  { ar.Set_Flat((float)val, IndexOfEl_Flat(row, cell)); return true; }

private:
  void	Initialize() {}
  void	Destroy() {}
};
TA_SMART_PTRS(float_Data); //

class TA_API int_Data : public DataColTp<int_Matrix> {
  // int data
INHERITED(DataColTp<int_Matrix>)
friend class DataTable;
public:
  override bool		isNumeric() const {return true;} // 
  override int		maxColWidth() const {return 11;} // assumes sign, 10 digs
  override ValType 	valType() const {return VT_INT;}

  TA_BASEFUNS_NOCOPY(int_Data);
  
protected:
  override double 	GetValAsDouble_impl(int row, int cell) const
  { return (double)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override float 	GetValAsFloat_impl(int row, int cell) const
  { return (float)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override int 		GetValAsInt_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }

  override bool	 	SetValAsDouble_impl(double val, int row, int cell)
  { ar.Set_Flat((int)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	 	SetValAsFloat_impl(float val, int row, int cell)
  { ar.Set_Flat((int)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	 	SetValAsInt_impl(int val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }

private:
  void	Initialize() {}
  void	Destroy() {}
};
TA_SMART_PTRS(int_Data); //

class TA_API byte_Data : public DataColTp<byte_Matrix> {
  // byte data
INHERITED(DataColTp<byte_Matrix>)
friend class DataTable;
public:
  override bool		isNumeric() const {return true;} // 
  override int		maxColWidth() const {return 3;} // assumes 3 digs
  override ValType 	valType() const {return VT_BYTE;}

  TA_BASEFUNS_NOCOPY(byte_Data);
  
protected:
  override double 	GetValAsDouble_impl(int row, int cell) const
  { return (double)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override float 	GetValAsFloat_impl(int row, int cell) const
  { return (float)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override int 		GetValAsInt_impl(int row, int cell) const
  { return (int)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override byte 	GetValAsByte_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }

  override bool	 	SetValAsDouble_impl(double val, int row, int cell)
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	 	SetValAsFloat_impl(float val, int row, int cell)
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	 	SetValAsInt_impl(int val, int row, int cell)
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	 	SetValAsByte_impl(byte val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }

private:
  void	Initialize() {}
  void	Destroy() {}
};
TA_SMART_PTRS(byte_Data); //

class TA_API DataTableEditorOptions : public taOBase {
  // for specifying and saving params for editing options
INHERITED(taOBase)
public:
  enum ViewMode {
    VM_CellDetail,	// #LABEL_Cell_Detail separate panel for cell detail editing
    VM_Flat		// #LABEL_Flat show all data in the main table, using dummy cells
  };

  TA_BASEFUNS_NOCOPY(DataTableEditorOptions);
  
private:
  void	Initialize() {}
  void	Destroy() {}
};


#endif // datatable_h
