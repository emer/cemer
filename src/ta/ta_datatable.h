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
class byte_Data;
class DataTableModel;

// note: the ColCalcExpr could be augmented to update with column name changes
// as in ProgExpr
// but it has a catch-22 with requiring a reference to data columns which have
// yet to be defined because they contain this object..

class TA_API ColCalcExpr: public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Data a column calculation expression
INHERITED(taOBase)
public:

  String	expr;		// #EDIT_DIALOG enter the expression here -- column value will be set to this.\nyou can just type in names of other columns (value is corresponding row's value) or literal values, or math expressions, etc.\nenclose strings in double quotes.\ncolumn names will be checked and automatically updated
  DataCol* col_lookup;	// #APPLY_IMMED #NULL_OK #NO_EDIT #NO_SAVE #FROM_GROUP_data_cols lookup a program variable and add it to the current expression (this field then returns to empty/NULL)

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
  COPY_FUNS(ColCalcExpr, inherited);
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
  // #VIRT_BASE ##NO_TOKENS #NO_INSTANCE ##CAT_Data holds a column of data;\n (a scalar cell can generally be treated as a degenerate matrix cell of dim[1])
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
  };

  String		desc; // #NO_SAVE_EMPTY #EDIT_DIALOG optional description to help in documenting the use of this column
  ColFlags		col_flags; // #APPLY_IMMED flags for this column to indicate specific properties 
  short			col_idx; // #READ_ONLY #SHOW #NO_SAVE the index of this column in the table
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
  virtual int		GetCellGeom(int dim) const { return cell_geom.SafeEl(dim); } // #CAT_Access for matrix type, size of given dim
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
  // #CAT_XpertAccess gets the cell as a slice of the entire column (note: not const -- you can write it)
  bool	 	SetValAsMatrix(const taMatrix* val, int row);
  // #CAT_XpertModify set the matrix cell from a same-sized matrix 
  taMatrix*	GetRangeAsMatrix(int st_row, int n_rows);
  // #CAT_XpertAccess gets a slice of the entire column from starting row for n_rows (note: not const -- you can write it)
  bool		GetMinMaxScale(MinMax& mm);
  // #CAT_Display get min-max range of values contained within this column 
  
  void		EnforceRows(int rows);
  // force data to have this many rows

  /////////////////////////////////////////////
  // misc

  virtual String	ColStats();
  // #CAT_DataProc #MENU #USE_RVAL compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).

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
  
  DataTable*		dataTable();
  // root data table this col belongs to

  String	EncodeHeaderName(int d0=0, int d1=0, int d2=0, int d3=0);
  // encode header information for saving to text files
  static void 	DecodeHeaderName(String nm, String& base_nm, int& val_typ,
				 MatrixGeom& mat_idx, MatrixGeom& mat_geom);
  // decode header information for loading from text files 

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); 

  virtual void	Copy_NoData(const DataCol& cp);
  // #CAT_ObjectMgmt copy the structure of the datatable without getting all the data
  virtual void	CopyFromRow(int dest_row, const DataCol& cp, int src_row);
  // #CAT_ObjectMgmt copy one row from source to given row in this object, assumes that the two have the same type and, if matrix, cell_size
  virtual void	CopyFromRow_Robust(int dest_row, const DataCol& cp, int src_row);
  // #CAT_ObjectMgmt copy one row from source to given row in this object, robust to differences in type and format of the cells
  
  override String 	GetTypeDecoKey() const { return "DataTable"; }

  virtual void Init(); // call this *after* creation, or in UAE, to assert matrix geometry
  override int	GetIndex() const {return col_idx;}
  override void	SetIndex(int value) {col_idx = (short)value;}
  override String GetDesc() const {return desc;}
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	InitLinks(); //note: ok to do own AR here, because never called in constructor
  void	CutLinks(); //note: NOT ok to do disown AR here, because called in destructor
  void 	Copy_(const DataCol& cp);
  COPY_FUNS(DataCol, taNBase);
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
  void	Initialize();
  void	Destroy()	{CutLinks(); }; //
};

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
  void	Initialize();
  void	Destroy()		{}
};

SmartRef_Of(DataTableCols,TA_DataTableCols); // DataTableColsRef

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
  // ##TOKENS ##CAT_Data ##FILETYPE_DataTable ##EXT_dtbl ##DEF_CHILD_data ##DEF_CHILDNAME_Columns table of data
INHERITED(DataBlock_Idx)
friend class DataTableCols;
friend class DataTableModel;
public:
  static int		idx_def_arg;
  // default arg val for functions returning index

  enum DataFlags { // #BITS flags for data table
    DF_NONE		= 0, // #NO_BIT
    SAVE_ROWS 		= 0x0001, // save the row data associated with this table when saved with the project (column and other configuration information is always saved)
    HAS_CALCS 		= 0x0002, // #NO_SHOW at least one of the columns has CALC flag set
    AUTO_CALC		= 0x0004, // automatically calculate columns
    AUTO_LOAD		= 0x0008, // automatically load data table from auto_load_file when data table object is loaded (useful when not saving rows of a large table, to make the project file smaller, but the cost is that the project is no longer self contained)
  };

  /////////////////////////////////////////////////////////
  // 	Main datatable interface:
  int 			rows;
  // #READ_ONLY #NO_SAVE #SHOW the number of rows
  DataTableCols		data;
  // all the columns and actual data
  DataFlags		data_flags;
  // flags for various features and state of the data table
  String		auto_load_file;
  // file to load data table from if AUTO_LOAD option is set (if file name has .dtbl extention, it is loaded using internal Load format, otherwise LoadData is used)

  cssProgSpace* 	calc_script;
  // #HIDDEN #NO_SAVE script object for performing column calculations
  taFiler*		log_file;
  // #NO_SAVE #HIDDEN file for logging data incrementally as it is written -- only for output.  a new line is written when WriteClose() (DataSink interface) is called.

  /////////////////////////////////////////////
  // Flags

  inline void		SetDataFlag(DataFlags flg)   { data_flags = (DataFlags)(data_flags | flg); }
  // set data column flag state on
  inline void		ClearDataFlag(DataFlags flg) { data_flags = (DataFlags)(data_flags & ~flg); }
  // clear data column flag state (set off)
  inline bool		HasDataFlag(DataFlags flg) const { return (data_flags & flg); }
  // check if data column flag is set
  inline void		SetDataFlagState(DataFlags flg, bool on)
  { if(on) SetDataFlag(flg); else ClearDataFlag(flg); }
  // set data column flag state according to on bool (if true, set flag, if false, clear it)

  /////////////////////////////////////////////////////////
  // columns

  int			cols() const { return data.size; }
  // #CAT_Columns number of columns
  override taList_impl* children_() {return &data;}

  virtual DataCol* 	NewCol(DataCol::ValType val_type, 
			       const String& col_nm);
  // #MENU #MENU_ON_Columns #ARG_C_2 #CAT_Columns create new scalar column of data of specified type
  virtual DataCol* 	NewColMatrix(DataCol::ValType val_type, const String& col_nm,
    int dims = 1, int d0=0, int d1=0, int d2=0, int d3=0);
  // #MENU #MENU_ON_Columns #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  virtual DataCol* 	NewColMatrixN(DataCol::ValType val_type, 
				      const String& col_nm,  const MatrixGeom& cell_geom);
  // #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  
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

  virtual DataCol* 	FindColName(const String& col_nm, int& col_idx = idx_def_arg, bool err_msg = false) const;
  // #CAT_Columns #ARGC_1 find a column of the given name; if err_msg then generate an error if not found

  DataCol* 		FindMakeCol(const String& col_nm,
	ValType val_type = VT_FLOAT);
  // #CAT_Columns insures that a scalar column of the given name and val type exists, and return that col. 
  DataCol* 		FindMakeColMatrix(const String& col_nm,
	ValType val_type = VT_FLOAT, int dims = 1,
	int d0=0, int d1=0, int d2=0, int d3=0);
  // #CAT_Columns insures that a matrix column of the given name, val type, and dimensions exists, and returns that col. 
  virtual DataCol* 	FindMakeColName(const String& col_nm, int& col_idx = idx_def_arg,
					ValType val_type = VT_FLOAT, int dims = 0,
					int d0=0, int d1=0, int d2=0, int d3=0);
  // #EXPERT #CAT_Columns find a column of the given name, val type, and dimension. if one does not exist, then create it.  Note that dims < 1 means make a scalar column, not a matrix
    
  virtual DataCol* 	GetColData(int col) const {
    if(TestError((col < 0 || col >= cols()), "GetColData",
		 "column number is out of range")) return NULL;
    else return data.FastEl(col);
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
    act_idx = col_size - (rows - row_num); return act_idx >= 0;} 
  // #CAT_Rows calculates an actual index for a col item, based on the current #rows and size of that col; returns 'true' if act_idx >= 0 (i.e., if there is a data item for that column)
  inline bool		idx_err(int row_num, int col_size, int& act_idx) const {
    return !TestError(!idx(row_num, col_size, act_idx), "idx_err", "index out of range"); }
  inline bool		idx_warn(int row_num, int col_size, int& act_idx) const {
    return !TestWarning(!idx(row_num, col_size, act_idx), "idx_err", "index out of range"); }
  virtual bool		RowInRangeNormalize(int& row);
  // #CAT_Rows normalizes row (if -ve) and tests result in range 
  virtual void		AllocRows(int n);
  // #CAT_Rows allocate space for at least n rows
  virtual int		AddBlankRow() 
  { if (AddRows(1)) {wr_itr = rows - 1; return wr_itr;} else return -1; }
  // #MENU #MENU_ON_Rows #CAT_Rows add a new row to the data table, sets write (sink) index to this last row (as in WriteItem), so that subsequent datablock routines refer to this new row, and returns row #
  virtual bool		AddRows(int n);
  // #MENU #CAT_Rows add n rows, returns true if successfully added
  virtual bool		InsertRows(int st_row, int n_rows=1);
  // #MENU #CAT_Rows insert n rows at starting row number, returns true if succesfully inserted
  
  virtual bool		RemoveRows(int st_row, int n_rows=1);
  // #MENU #MENU_ON_Rows #CAT_Rows Remove n rows of data, starting at st_row.  st_row = -1 means last row, and n_rows = -1 means remove from starting row to end
  virtual void		RemoveAllRows() { ResetData(); }
  // #MENU #CAT_Rows #CONFIRM remove all of the rows of data, but keep the column structure
  virtual bool		DuplicateRow(int row_no, int n_copies=1);
  // #MENU #CAT_Rows duplicate given row number, making given number of copies of it (adds new rows at the end)
  const Variant		GetColUserData(const String& name, int col) const;
  // #CAT_Config gets user data from the col
  void			SetColUserData(const String& name, const Variant& value, int col);
  // #CAT_Config sets user data into the col

  /////////////////////////////////////////////////////////
  // Main data value access/modify (Get/Set) routines: for Programs and very general use

  const Variant 	GetVal(int col, int row) const
  { return GetValAsVar(col, row); }
  // #CAT_Access get data of scalar type, in Variant form (any data type, use for Programs), for given column, row
  bool 			SetVal(const Variant& val, int col, int row)
  { return SetValAsVar(val, col, row); }
  // #CAT_Modify set data of scalar type, in Variant form (any data type, use for Programs), for given column, row; returns 'true' if valid access and set is successful

  const Variant 	GetMatrixVal(int col, int row,
				     int d0, int d1=0, int d2=0, int d3=0) const
  { return GetValAsVarMDims(col, row, d0, d1, d2, d3); }
  // #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies
  bool 			SetMatrixVal(const Variant& val, int col, int row, 
				     int d0, int d1=0, int d2=0, int d3=0)
  { return SetValAsVarMDims(val, col, row, d0, d1, d2, d3); }
  // #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies; returns 'true' if valid access and set is successful

  bool	 	InitVals(const Variant& init_val, int col);
  // #CAT_Modify initialize all values in given column to given value
  bool	 	InitValsToRowNo(int col);
  // #CAT_Modify initialize all values in given column to be equal to the row number -- only valid for scalar (not matrix) columns

  int 		FindVal(const Variant& val, int col, int st_row = 0) const;
  // #CAT_Access find row number for given value within column col of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)

  /////////////////////////////
  // column name versions:

  const Variant 	GetValColName(const String& col_name, int row) const;
  // #CAT_Access get data of scalar type, in Variant form (any data type, use for Programs), for given column name, row
  bool 			SetValColName(const Variant& val, const String& col_name, int row);
  // #CAT_Modify set data of scalar type, in Variant form (any data type, use for Programs), for given column name, row; returns 'true' if valid access and set is successful

  const Variant 	GetMatrixValColName(const String& col_name, int row,
					    int d0, int d1=0, int d2=0, int d3=0) const;
  // #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column name, row, and matrix dimension indicies
  bool 			SetMatrixValColName(const Variant& val, const String& col_name,
					    int row, int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies; returns 'true' if valid access and set is successful

  bool	 	InitValsColName(const Variant& init_val, const String& col_name);
  // #CAT_Modify initialize all values in column of given name to given value
  bool	 	InitValsToRowNoColName(const String& col_name);
  // #CAT_Modify initialize all values in column of given name to be equal to the row number -- only valid for scalar (not matrix) columns

  int 		FindValColName(const Variant& val, const String& col_name, int st_row = 0) const;
  // #CAT_Access find row number for given value within column col of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)

  /////////////////////////////
  // column pointer versions, just for the gui:

  bool	 	InitValsCol(DataCol* col, const Variant& init_val)
  { return col->InitVals(init_val); }
  // #CAT_Columns #MENU #MENU_ON_Columns #FROM_GROUP_data initialize all values in given column to given value
  bool	 	InitValsToRowNoCol(DataCol* col) 
  { return col->InitValsToRowNo(); }
  // #CAT_Modify #MENU #FROM_GROUP_data initialize all values in given column to be equal to the row number -- only valid for scalar (not matrix) columns
  int 		FindValCol(DataCol* col, const Variant& val, int st_row = 0) const 
  { return col->FindVal(val, st_row); }
  // #CAT_Access #MENU #FROM_GROUP_data find row number for given value within column col of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)


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
  // Matrix, Cell

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
  // #CAT_XpertAccess get data of matrix type, in Matrix form (one frame), for given column, row; Invalid/NULL if no cell; YOU MUST REF MATRIX; note: not const because you can write it
  bool 			SetValAsMatrix(const taMatrix* val, int col, int row);
  // #CAT_XpertModify  set data of any type, in Variant form, for given column, row; does nothing if no cell; 'true' if set
  taMatrix*	 	GetRangeAsMatrix(int col, int st_row, int n_rows);
  // #CAT_XpertAccess get data as a Matrix for a range of rows, for given column, st_row, and n_rows; row; Invalid/NULL if no cell; YOU MUST REF MATRIX; note: not const because you can write it

  int 			GetMaxCellRows(int col_fr, int col_to); // #IGNORE get the max muber of cell rows in this col range (used for clip operations)
  void			GetFlatGeom(const CellRange& cr, int& tot_cols, 
   int& max_cell_rows); // IGNORE get the total flat cols and max rows per cell; used for TSV output

  String		RangeToTSV(const CellRange& cr); // #IGNORE for clip operations

  /////////////////////////////////////////////////////////
  // saving/loading (file)

  enum Delimiters {
    TAB,
    SPACE,
    COMMA,
  };

  // dumping and loading -- see .cpp file for detailed format information, not saved as standard taBase obj
  virtual void 		SaveData_strm(ostream& strm, Delimiters delim = TAB, bool quote_str = true);
  // #CAT_File #EXT_dat saves data, one line per rec, with delimiter between columns, and optionally quoting strings
  virtual void 		SaveHeader_strm(ostream& strm, Delimiters delim = TAB);
  // #CAT_File #EXT_dat saves header information, with delimiter between columns, and optionally quoting strings
  virtual void 		SaveDataRow_strm(ostream& strm, int row=-1, Delimiters delim = TAB,
					 bool quote_str = true); 
  // #CAT_File #EXT_dat saves one row of data (-1 = last row), with delimiter between columns, and optionally quoting strings
  virtual void 		SaveDataRows_strm(ostream& strm, Delimiters delim = TAB,
					 bool quote_str = true); 
  // #CAT_File #EXT_dat saves all rows of data (no header) with delimiter between columns, and optionally quoting strings

  virtual void 		SaveData(const String& fname="", Delimiters delim = TAB, bool quote_str = true);
  // #CAT_File #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_dat saves data, one line per rec, with delimiter between columns, and optionally quoting strings; leave fname empty to pick from file chooser
  virtual void 		AppendData(const String& fname="", Delimiters delim = TAB,
				   bool quote_str = true); 
  // #CAT_File #MENU #EXT_dat appends all of current datatable data to given file (does not output header; file assumed to be of same data structure
  virtual void 		SaveHeader(const String& fname="", Delimiters delim = TAB);
  // #CAT_File #MENU #EXT_dat saves header information, with delimiter between columns, and optionally quoting strings; leave fname empty to pick from file chooser
  virtual void 		SaveDataRow(const String& fname="", int row=-1, Delimiters delim = TAB,
					 bool quote_str = true); 
  // #CAT_File #MENU #EXT_dat saves one row of data (-1 = last row), with delimiter between columns, and optionally quoting strings; leave fname empty to pick from file chooser

  virtual void		SaveDataLog(const String& fname="", bool append=false,
				    bool dmem_proc_0 = true);
  // #CAT_File #MENU #MENU_SEP_BEFORE #ARGC_0 #EXT_dat incrementally save each new row of data that is written to the datatable (at WriteClose()) to given file.  writes the header first if not appending to existing file.  if running under demem, dmem_proc_0 determines if only the first processor writes to the log file, or if all processors write
  virtual void		CloseDataLog();
  // #CAT_File #MENU close the data log file if it was previously open
  virtual bool		WriteDataLogRow();
  // #CAT_File write the current row to the data log, if it is open (returns true if successfully wrote) -- this is automatically called by WriteClose 

  static char		GetDelim(Delimiters delim);
  // #IGNORE get delimiter from enum
  static int		ReadTillDelim(istream& strm, String& str, const char delim, bool quote_str);
  // #IGNORE util function to read from stream into str until delim or newline or EOF
  static int_Array	load_col_idx; // #IGNORE mapping of column numbers in data load to column indexes based on header name matches
  static int_Array	load_mat_idx; // #IGNORE mapping of column numbers in data to matrix indicies in columns, based on header info

  virtual void 		LoadData_strm(istream& strm, Delimiters delim = TAB,
				 bool quote_str = true, int max_recs = -1);
  // #CAT_File #EXT_dat loads data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings
  virtual int 		LoadHeader_strm(istream& strm, Delimiters delim = TAB);
  // #CAT_File #EXT_dat loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end)
  virtual int 		LoadDataRow_strm(istream& strm, Delimiters delim = TAB, bool quote_str = true);
  // #CAT_File #EXT_dat load one row of data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings (returns EOF if strm is at end)

  virtual void 		LoadData(const String& fname, Delimiters delim = TAB,
				 bool quote_str = true, int max_recs = -1);
  // #CAT_File #MENU #MENU_SEP_BEFORE #EXT_dat loads data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings
  virtual int 		LoadHeader(const String& fname, Delimiters delim = TAB);
  // #CAT_File #EXT_dat loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end)
  virtual int 		LoadDataRow(const String& fname, Delimiters delim = TAB, bool quote_str = true);
  // #CAT_File #MENU #EXT_dat load one row of data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings (returns EOF if strm is at end)
  
 
  /////////////////////////////////////////////////////////
  // calculated column values

  virtual bool		UpdateColCalcs();
  // #CAT_Calc update column calculations
  virtual bool		CalcLastRow();
  // #CAT_Calc if HAS_CALCS, does calculations for last row of data -- called by WriteClose

  virtual bool		CheckForCalcs();
  // #CAT_Calc see if any columns have CALC flag -- sets HAS_CALCS flag -- returns state of flag
  virtual void		InitCalcScript();
  // #IGNORE initialize the calc_script for computing column calculations
  virtual bool		CalcAllRows_impl();
  // #CAT_Calc perform calculations for all rows of data (calls InitCalcScript to make sure)
  virtual bool		CalcAllRows();
  // #CAT_Calc #BUTTON #GHOST_OFF_data_flags:HAS_CALCS perform calculations for all rows of data (updates after)
  virtual void		CalcRowCodeGen(String& code_str);
  // #IGNORE generate code for computing one row worth of data, with assumed 'int row' variable specifying row
  virtual bool		CalcRow(int row);
  // #CAT_Calc perform calculations for given row of data (calls InitCalcScript to make sure)

  /////////////////////////////////////////////////////////
  // core data processing -- see taDataProc for more elaborate options

  virtual void		Sort(DataCol* col1, bool ascending1 = true,
			     DataCol* col2 = NULL, bool ascending2 = true,
			     DataCol* col3 = NULL, bool ascending3 = true);
  // #CAT_DataProc #MENU #MENU_ON_DataProc #FROM_GROUP_data #NULL_OK sort table according to selected columns of data: NOTE that this modifies this table and currently cannot be undone -- make a duplicate table first if you want to save the original data!
  virtual bool		Filter(const String& filter_expr);
  // #CAT_DataProc #MENU #FROM_GROUP_data filter (select) table rows by applying given expression -- if it evaluates to true, the row is included, and otherwise it is removed.  refer to current colum values by name.  NOTE that this modifies this table and currently cannot be undone -- make a duplicate table first if you want to save the original data!
  virtual bool		GroupMeanSEM(DataTable* dest_data,
				     DataCol* gp_col1, DataCol* gp_col2 = NULL,
				     DataCol* gp_col3 = NULL, DataCol* gp_col4 = NULL);
  // #CAT_DataProc #MENU #FROM_GROUP_data #NULL_OK #NULL_TEXT_0_NewDataTable groups data according to given columns in hierarchical fashion (gp_col2 is subgrouped within gp_col1, etc), and compute the Mean and Standard Error of the Mean (SEM) for any other numerical columns of data -- results go in dest_data table (new table created if NULL)
  virtual String	ColStats(DataCol* col);
  // #CAT_DataProc #MENU #FROM_GROUP_data #USE_RVAL compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).

  /////////////////////////////////////////////////////////
  // misc funs

  virtual void		NewGridView(T3DataViewFrame* fr = NULL);
  // #NULL_OK_0 #NULL_TEXT_0_NewFrame #MENU #MENU_ON_View #MENU_CONTEXT #CAT_Display open a grid view (graphical rows and columns) of this table (NULL=use blank if any, else make new frame)
  virtual void		NewGraphView(T3DataViewFrame* fr = NULL);
  // #NULL_OK_0 #NULL_TEXT_0_NewFrame #MENU #MENU_CONTEXT #CAT_Display open a graph view of this table (NULL=use blank if any, else make new frame)

  virtual int  		MinLength();		// #IGNORE
  virtual int  		MaxLength();		// #IGNORE

  virtual DataTableModel* GetDataModel();
  // #IGNORE returns new if none exists, or existing -- enables views to be shared

  virtual void	Copy_NoData(const DataTable& cp);
  // #CAT_ObjectMgmt copy only the column structure, but no data, from other data table
  virtual void	CopyFromRow(int dest_row, const DataTable& cp, int src_row);
  // #CAT_ObjectMgmt copy one row from source to given row in this object: source must have exact same column structure as this!!
  virtual bool	CopyColRow(int dest_col, int dest_row, const DataTable& src, int src_col, int src_row);
  // #CAT_ObjectMgmt copy one column, row from source -- is robust to differences in type and matrix sizing (returns false if not successful)

  virtual void	UniqueColNames();
  // #CAT_ObjectMgmt ensure that the column names are all unique (adds _n for repeats)

  virtual void	DMem_ShareRows(MPI_Comm comm, int n_rows = 1);
  // #CAT_DMem Share the given number of rows from the end of the table (-1 = all rows) across processors in given communicator -- everyone gets the data from all processors as new rows in the table

  virtual bool		AutoLoadData();
  // #IGNORE perform auto loading of data from file when data table is loaded (called by PostLoadAutos) -- true if loaded
  override void		Dump_Load_post();

  override int 		Dump_Load_Value(istream& strm, TAPtr par);
  override String 	GetTypeDecoKey() const { return "DataTable"; }

  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const DataTable& cp);
  COPY_FUNS(DataTable, DataBlock_Idx);
  TA_BASEFUNS(DataTable); //

protected: 
  override void	UpdateAfterEdit_impl();
  /////////////////////////////////////////////////////////
  // IMPL
  

  void			RowsAdding(int n, bool begin);
  // indicate beginning and end of row adding -- you have to pass the same n each time; NOT nestable

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
  DataTableModel*	m_dm; // #IGNORE note: once we create, always exists
  
  DataCol*	NewCol_impl(DataCol::ValType val_type, 
				    const String& col_nm);
  // low-level create routine, shared by scalar and matrix creation, must be wrapped in StructUpdate
  DataCol*	GetColForChannelSpec_impl(ChannelSpec* cs);
  DataCol*	NewColFromChannelSpec_impl(ChannelSpec* cs);
  
private:
  void	Initialize();
  void	Destroy();
};

SmartRef_Of(DataTable,TA_DataTable); // DataTableRef


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
  void	Initialize() 		{ SetBaseType(&TA_DataTable); }
  void 	Destroy()		{ };
};


/////////////////////////////////////////////////////////
//   DataCol templates
/////////////////////////////////////////////////////////

template<class T> 
class DataColTp : public DataCol {
  // #VIRT_BASE #NO_INSTANCE template for common elements
public:
  override taMatrix* 	AR()	{ return &ar; } // the array pointer
  override const taMatrix* AR() const { return &ar; } // the array pointer

  void	CutLinks()
    {ar.CutLinks(); DataCol::CutLinks();}
  void	Copy_(const DataColTp<T>& cp)  { ar = cp.ar; }
  COPY_FUNS(DataColTp<T>, DataCol);
  TA_ABSTRACT_TMPLT_BASEFUNS(DataColTp, T); //
public: //DO NOT ACCESS DIRECTLY
  T		ar;		// #NO_SHOW  the array itself
private:
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

  TA_BASEFUNS(String_Data);

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


class TA_API Variant_Data : public DataColTp<Variant_Matrix> {
  // Variant data
INHERITED(DataColTp<Variant_Matrix>)
friend class DataTable;
public:
  override ValType 	valType() const  {return VT_VARIANT;}

  TA_BASEFUNS(Variant_Data);

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


class TA_API double_Data : public DataColTp<double_Matrix> {
  // doubleing point data
INHERITED(DataColTp<double_Matrix>)
friend class DataTable;
public:
  override bool		isNumeric() const {return true;} 
  override int		maxColWidth() const {return 15;} // assumes sign, int: 15 dig's; double: 14 dig's, decimal point
  override ValType 	valType() const {return VT_DOUBLE;}

  TA_BASEFUNS(double_Data);
  
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

class TA_API float_Data : public DataColTp<float_Matrix> {
  // floating point data
INHERITED(DataColTp<float_Matrix>)
friend class DataTable;
public:
  override bool		isNumeric() const {return true;} 
  override int		maxColWidth() const {return 7;} // assumes sign, int: 6 dig's; float: 5 dig's, decimal point
  override ValType 	valType() const {return VT_FLOAT;}

  TA_BASEFUNS(float_Data);
  
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

class TA_API int_Data : public DataColTp<int_Matrix> {
  // int data
INHERITED(DataColTp<int_Matrix>)
friend class DataTable;
public:
  override bool		isNumeric() const {return true;} // 
  override int		maxColWidth() const {return 11;} // assumes sign, 10 digs
  override ValType 	valType() const {return VT_INT;}

  TA_BASEFUNS(int_Data);
  
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

class TA_API byte_Data : public DataColTp<byte_Matrix> {
  // byte data
INHERITED(DataColTp<byte_Matrix>)
friend class DataTable;
public:
  override bool		isNumeric() const {return true;} // 
  override int		maxColWidth() const {return 3;} // assumes 3 digs
  override ValType 	valType() const {return VT_BYTE;}

  TA_BASEFUNS(byte_Data);
  
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

class TA_API DataTableModel: public QAbstractTableModel,
  public IDataLinkClient
{
  // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for tables;\ncreated and owned by the DataTable
INHERITED(QAbstractTableModel)
friend class DataTableCols;
friend class DataTable;
public:

  DataTable*		dataTable() const {return dt;}
  void			setDataTable(DataTable* value, bool notify = true);
  
  void			refreshViews(); // similar to matrix, issues dataChanged
  
  void			emit_dataChanged(int row_fr = 0, int col_fr = 0,
    int row_to = -1, int col_to = -1);// can be called w/o params to issue global change (for manual refresh)

  DataTableModel(DataTable* owner);
  ~DataTableModel(); //
  
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
  override TypeDef*	GetTypeDef() const {return &TA_DataTableModel;}
  override void		DataLinkDestroying(taDataLink* dl);
  override void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2); 
    
protected:
  bool			ValidateIndex(const QModelIndex& index) const;
#endif
  void			emit_layoutChanged(); // we call this for most schema changes
protected:
  DataTable*		dt;
};

class TA_API DataTableEditorOptions : public taOBase {
  // for specifying and saving params for editing options
INHERITED(taOBase)
public:
  enum ViewMode {
    VM_CellDetail,	// #LABEL_Cell_Detail separate panel for cell detail editing
    VM_Flat		// #LABEL_Flat show all data in the main table, using dummy cells
  };

  TA_BASEFUNS(DataTableEditorOptions);
  
private:
  void	Initialize() {}
  void	Destroy() {}
};


#endif // datatable_h
