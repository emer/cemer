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
#include "ta_group.h"
#include "ta_matrix.h"
#include "ta_data.h"
#include "ta_filer.h"
#include "ta_viewspec.h"
#include "ta_dmem.h"
#include "ta_TA_type.h"

#ifndef __MAKETA__
# include <QAbstractTableModel> 
#endif

// externals
class T3DataViewFrame;

// forwards this file

class DataTable;
class DataTable_Group;
class String_Data; 
class Variant_Data; 
class float_Data;
class double_Data;
class int_Data;
class byte_Data;
class DataColViewSpec;
class DataColViewSpecs;
class DataTableViewSpec;
class DataTableModel; //

// specific ones are in the template classes: String_Data, float_Data

/*
  Display Options (subclasses add new ones -- see each class)

  HIDDEN -- forces !visible by default
  TEXT -- sets display_style to TEXT
  NARROW -- sets display_style to TEXT; also, makes it !visible by default

*/

/////////////////////////////////////////////////////////
//   DataArray_impl -- One colum of a datatable
/////////////////////////////////////////////////////////

class TA_API DataArray_impl : public taNBase {
  // #VIRT_BASE ##NO_TOKENS #NO_INSTANCE ##CAT_Data holds a column of data;\n (a scalar cell can generally be treated as a degenerate matrix cell of dim[1])
INHERITED(taNBase)
friend class DataTable;
public:
  static const String	udkey_width; // WIDTH=n display width, in chars
  static const String	udkey_narrow; // NARROW=b if narrow (default for ints)
  static const String	udkey_hidden; // HIDDEN=b defaults to not visible
  
  String		disp_opts;
  // #NO_SHOW #NO_SAVE #OBSOLETE viewer default display options DELETE THIS
  bool			mark;
  // #NO_SHOW #NO_SAVE clear on new and when col confirmed, used to delete orphans
  bool			pin;
  // set true to prevent this column from being deleted on orphan deleting
  bool			is_matrix;
  // #READ_ONLY #SAVE #SHOW 'true' if the cell is a matrix, not a scalar
  MatrixGeom		cell_geom;
  // #READ_ONLY #SAVE #SHOW for matrix cols, the geom of each cell
  
  virtual taMatrix* 	AR() = 0;
  // #CAT_Access the matrix pointer -- NOTE: actual member should be called 'ar'
  virtual const taMatrix* 	AR() const = 0;
  // #CAT_Access const version of the matrix pointer

  /////////////////////////////////////////////
  // type of data

  virtual ValType 	valType() const = 0;
  // #CAT_Access the type of data in each element
  inline bool		isMatrix() const {return is_matrix;} // for consistency
  virtual bool		isNumeric() const {return false;}
  // #CAT_Access true if data is float, int, or byte
  virtual bool		isString() const {return false;}
  // #CAT_Access true if data is string
  virtual bool		isImage() const;
  // #CAT_Access true if the cell contains an image
  virtual int		imageComponents() const;
  // #CAT_Access if an image, then: b&w=1, b&w+a=2, rgb=3, rgba=4
  
  virtual int		cell_size() const
  { return (is_matrix) ? cell_geom.Product() : 1; }
  // #CAT_Access for matrix type, number of elements in each cell
  virtual int		cell_dims() const { return cell_geom.size; }
  // #CAT_Access for matrix type, number of dimensions in each cell
  virtual int		GetCellGeom(int dim) const { return cell_geom.SafeEl(dim); } // #CAT_Access for matrix type, size of given dim
  void			Get2DCellGeom(iVec2i& result); // #IGNORE for rendering routines, provides standardized 2d geom (1x1 for scalar cells)
  
  int			rows() const { return AR()->frames(); }
  // #CAT_Access total number of rows of data within this column

  /////////////////////////////////////////////
  // Get and Set access

  // row is absolute row in the matrix, not a DataTable row -- use the DataTable routines
  // -ve values are from end, and are valid for both low-level col access, and DataTable access
  
  const Variant GetValAsVar(int row) const {return GetValAsVar_impl(row, 0);}
  // #CAT_Access valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsVar(const Variant& val, int row) 
  { return SetValAsVar_impl(val, row, 0);} 
  // #CAT_Modify valid for all types, -ve row is from end (-1=last)
  const String 	GetValAsString(int row) const {return GetValAsString_impl(row, 0);}
  // #CAT_Access valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsString(const String& val, int row) 
  {return SetValAsString_impl(val, row, 0);} 
  // #CAT_Modify valid for all types, -ve row is from end (-1=last)
  float 	GetValAsFloat(int row) const {return GetValAsFloat_impl(row, 0);} 
  // #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsFloat(float val, int row) 
  // #CAT_Modify valid only if type is float, -ve row is from end (-1=last)
  {return SetValAsFloat_impl(val, row, 0);} 
  double 	GetValAsDouble(int row) const {return GetValAsDouble_impl(row, 0);} 
  // #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsDouble(double val, int row) 
  // #CAT_Modify valid only if type is double, -ve row is from end (-1=last)
  {return SetValAsDouble_impl(val, row, 0);} 
  int	 	GetValAsInt(int row) const {return GetValAsInt_impl(row, 0);} 
  // #CAT_Access valid if type is int or byte, -ve row is from end (-1=last)
  bool	 	SetValAsInt(int val, int row) 
  // #CAT_Modify valid if type is int or float, -ve row is from end (-1=last)
  {return SetValAsInt_impl(val, row, 0);} 
  byte	 	GetValAsByte(int row) const {return GetValAsByte_impl(row, 0);} 
  // #CAT_Access valid only if type is byte, -ve row is from end (-1=last)
  bool	 	SetValAsByte(byte val, int row) 
  // #CAT_Modify valid if type is numeric, -ve row is from end (-1=last)
  {return SetValAsByte_impl(val, row, 0);} 
    
  // Matrix versions
  const Variant GetValAsVarM(int row, int cell) const {return GetValAsVar_impl(row, cell);} 
  // #CAT_Access valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsVarM(const Variant& val, int row, int cell) 
  { return SetValAsVar_impl(val, row, cell);} 
  // #CAT_Modify valid for all types, -ve row is from end (-1=last)
  const String 	GetValAsStringM(int row, int cell) const {return GetValAsString_impl(row, cell);} 
  // #CAT_Access valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsStringM(const String& val, int row, int cell) 
  {return SetValAsString_impl(val, row, cell);} 
  // #CAT_Modify valid for all types, -ve row is from end (-1=last)
  float 	GetValAsFloatM(int row, int cell) const {return GetValAsFloat_impl(row, cell);} 
  // #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsFloatM(float val, int row, int cell) 
  // #CAT_Modify valid only if type is float, -ve row is from end (-1=last)
  {return SetValAsFloat_impl(val, row, cell);} 
  double 	GetValAsDoubleM(int row, int cell) const {return GetValAsDouble_impl(row, cell);} 
  // #CAT_Access valid if type is numeric, -ve row is from end (-1=last)
  bool	 	SetValAsDoubleM(double val, int row, int cell) 
  // #CAT_Modify valid only if type is float, -ve row is from end (-1=last)
  {return SetValAsDouble_impl(val, row, cell);} 
  int	 	GetValAsIntM(int row, int cell) const {return GetValAsInt_impl(row, cell);} 
  // #CAT_Access valid if type is int or byte, -ve row is from end (-1=last)
  bool	 	SetValAsIntM(int val, int row, int cell) 
  // #CAT_Modify valid if type is int or float, -ve row is from end (-1=last)
  {return SetValAsInt_impl(val, row, cell);} 
  byte	 	GetValAsByteM(int row, int cell) const {return GetValAsByte_impl(row, cell);} 
  // #CAT_Access valid only if type is byte, -ve row is from end (-1=last)
  bool	 	SetValAsByteM(byte val, int row, int cell) 
  // #CAT_Modify valid if type is numeric, -ve row is from end (-1=last)
  {return SetValAsByte_impl(val, row, cell);} 

  // Matrix ops -- you must Ref/UnRef taMatrix return types
  taMatrix*	GetValAsMatrix(int row);
  // #CAT_Access gets the cell as a slice of the entire column (note: not const -- you can write it)
  bool	 	SetValAsMatrix(const taMatrix* val, int row);
  // #CAT_Modify set the matrix cell from a same-sized matrix 
  taMatrix*	GetRangeAsMatrix(int st_row, int n_rows);
  // #CAT_Access gets a slice of the entire column from starting row for n_rows (note: not const -- you can write it)
  
  void		EnforceRows(int rows);
  // force data to have this many rows

  /////////////////////////////////////////////
  // misc

  int			displayWidth() const;
  // #CAT_Display low level display width, in chars, taken from options
  virtual int		maxColWidth() const {return -1;}
  // #CAT_Display aprox max number of columns, in characters, -1 if variable or unknown
  virtual bool		saveToFile() const {return true;}
  // #IGNORE whether to save col -- currently always true

  static const KeyString key_val_type; // "val_type"
  static const KeyString key_disp_opts; // "disp_opts"
  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  override String	GetDisplayName() const; // #IGNORE we strip out the format characters
  
  DataTable*		dataTable();
  // root data table this col belongs to

  String	EncodeHeaderName(int d0=0, int d1=0, int d2=0, int d3=0, int d4=0);
  // encode header information for saving to text files
  static void 	DecodeHeaderName(String nm, String& base_nm, int& val_typ,
				 MatrixGeom& mat_idx, MatrixGeom& mat_geom);
  // decode header information for loading from text files 

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); 

  virtual void	Copy_NoData(const DataArray_impl& cp);
  // #CAT_ObjectMgmt copy the structure of the datatable without getting all the data
  virtual void	CopyFromRow(int dest_row, const DataArray_impl& cp, int src_row);
  // #CAT_ObjectMgmt copy one row from source to given row in this object, assumes that the two have the same type and, if matrix, cell_size
  virtual void	CopyFromRow_Robust(int dest_row, const DataArray_impl& cp, int src_row);
  // #CAT_ObjectMgmt copy one row from source to given row in this object, robust to differences in type and format of the cells
  
  virtual void Init(); // call this *after* creation, or in UAE, to assert matrix geometry
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	InitLinks(); //note: ok to do own AR here, because never called in constructor
  void	CutLinks(); //note: NOT ok to do disown AR here, because called in destructor
  void 	Copy_(const DataArray_impl& cp);
  COPY_FUNS(DataArray_impl, taNBase);
  TA_ABSTRACT_BASEFUNS(DataArray_impl);
  
protected:
  override void  UpdateAfterEdit_impl();
  // in all accessor routines, -ve row is from end (-1=last)
  int			IndexOfEl_Flat(int row, int cell) const; 
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
//   DataTableCols -- group of DataArray
/////////////////////////////////////////////////////////

class TA_API DataTableCols: public taList<DataArray_impl> {
  // ##CAT_Data columns of a datatable 
INHERITED(taList<DataArray_impl>)
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
  
  virtual void	Copy_NoData(const DataTableCols& cp);
  // #CAT_ObjectMgmt copy only the column structure, but no data, from other data table
  virtual void	CopyFromRow(int dest_row, const DataTableCols& cp, int src_row);
  // #CAT_ObjectMgmt copy one row from source to given row in this object: source must have exact same column structure as this!!

  TA_BASEFUNS(DataTableCols);
private:
  void	Initialize();
  void	Destroy()		{}
};

SmartRef_Of(DataTableCols) // DataTableColsRef

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
  // #NO_UPDATE_AFTER ##TOKENS ##CAT_Data ##FILETYPE_DataTable ##EXT_dtbl ##DEF_CHILD_data table of data
INHERITED(DataBlock_Idx)
friend class DataTableCols;
friend class DataTableModel;
public:
  /////////////////////////////////////////////////////////
  // 	Main datatable interface:

  int 			rows;
  // #READ_ONLY #NO_SAVE #SHOW the number of rows
  bool			save_data;
  // 'true' if data should be saved in project; typically false for logs, true for data patterns
  DataTableCols		data;
  // all the columns and actual data
  taFiler*		log_file;
  // #NO_SAVE #HIDDEN file for logging data incrementally as it is written -- only for output.  a new line is written when WriteClose() (DataSink interface) is called.

  /////////////////////////////////////////////////////////
  // columns

  int			cols() const { return data.size; }
  // #CAT_Columns number of columns
  override taList_impl* children_() {return &data;}

  DataArray_impl*	NewCol(DataArray_impl::ValType val_type, 
			       const String& col_nm);
  // #MENU #MENU_ON_Table #ARG_C_2 #CAT_Columns create new scalar column of data of specified type
  DataArray_impl*	NewColMatrix(DataArray_impl::ValType val_type, const String& col_nm,
    int dims = 1, int d0=0, int d1=0, int d2=0, int d3=0, int d4=0);
  // #MENU #MENU_ON_Table #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  DataArray_impl*	NewColMatrixN(DataArray_impl::ValType val_type, 
				      const String& col_nm,  const MatrixGeom& cell_geom);
  // #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  
  double_Data*		NewColDouble(const String& col_nm); 
  // #CAT_Columns create new column of double data
  float_Data*		NewColFloat(const String& col_nm); 
  // #CAT_Columns create new column of floating point data
  int_Data*		NewColInt(const String& col_nm); 	 
  // #CAT_Columns create new column of integer-level data (= narrow display, actually stored as float)
  String_Data*		NewColString(const String& col_nm); 
  // #CAT_Columns create new column of string data

  void			SetColName(const String& col_nm, int col);
  // #CAT_Columns set column name for given column
  bool			RenameCol(const String& cur_nm, const String& new_nm);
  // #CAT_Columns rename column with current name cur_nm to new name new_nm (returns false if ccur_nm not found)

  DataArray_impl*	FindColName(const String& col_nm, int& col_idx = idx_def_arg, bool err_msg = false);
  // #CAT_Columns find a column of the given name; if err_msg then generate an error if not found

  DataArray_impl*	FindMakeColName(const String& col_nm, int& col_idx = idx_def_arg,
					ValType val_type = VT_FLOAT, int dims = 0,
					int d0=0, int d1=0, int d2=0, int d3=0, int d4=0);
  // #CAT_Columns find a column of the given name, val type, and dimension. if one does not exist, then create it.  Note that dims < 1 means make a scalar column, not a matrix
    
  DataArray_impl* 	GetColData(int col) const;
  // #CAT_Columns get col data for given column 
  taMatrix*		GetColMatrix(int col) const;
  // #CAT_Columns get matrix for given column -- WARNING: this is NOT row-number safe 

  bool 			ColMatchesChannelSpec(const DataArray_impl* da, const ChannelSpec* cs);
  // #CAT_Columns returns 'true' if the col has the same name and a compatible data type
  DataArray_impl*	NewColFromChannelSpec(ChannelSpec* cs)
  // #MENU_1N #CAT_Columns create new matrix column of data based on name/type in the data item (default is Variant)
  { if (cs) return NewColFromChannelSpec_impl(cs); else return NULL; }
    
  DataArray_impl*	GetColForChannelSpec(ChannelSpec* cs)
  // #MENU_1N #CAT_Columns find existing or create new matrix column of data based on name/type in the data item
    {if (cs) return GetColForChannelSpec_impl(cs); else return NULL;}
    
  void			RemoveCol(int col);
  // #CAT_Columns removes indicated column; 'true' if removed
  virtual void		Reset();
  // #CAT_Modify remove all columns and data

  void			MarkCols();
  // #CAT_Columns mark all cols before updating, for orphan deleting
  void			RemoveOrphanCols();
  // #CAT_Columns removes all non-pinned marked cols
  

  /////////////////////////////////////////////////////////
  // rows (access)

  bool			hasData(int col, int row);
  // #CAT_Rows true if data at that cell
  bool			idx(int row_num, int col_size, int& act_idx) const
  { if (row_num < 0) row_num = rows + row_num;
    act_idx = col_size - (rows - row_num); return act_idx >= 0;} 
  // #CAT_Rows calculates an actual index for a col item, based on the current #rows and size of that col; returns 'true' if act_idx >= 0 (i.e., if there is a data item for that column)
  bool			RowInRangeNormalize(int& row);
  // #CAT_Rows normalizes row (if -ve) and tests result in range 
  void			AllocRows(int n);
  // #CAT_Rows allocate space for at least n rows
  int			AddBlankRow() 
    {if (AddRow(1)) {wr_itr = rows - 1; return wr_itr;} else return -1;}
  // #MENU #MENU_ON_Data #CAT_Rows add a new row to the data table, returns row #
  bool			AddRow(int n);
  // #CAT_Rows add n rows, 'true' if added
  void			RemoveRow(int row_num);
  // #MENU #MENU_ON_Data #CAT_Rows Remove an entire row of data
//TODO if needed:  virtual void	ShiftUp(int num_rows);
  // remove indicated number of rows of data at front (typically used by Log to make more room in buffer)
  bool			DuplicateRow(int row_no, int n_copies=1);
  // #MENU #CAT_Rows duplicate given row number, making given number of copies of it
  void			RemoveAllRows() { ResetData(); }
  // #CAT_Rows remove all of the rows, but keep the column structure

  const Variant		GetColUserData(const String& name, int col) const;
  // #CAT_Config gets user data from the col
  void			SetColUserData(const String& name, const Variant& value, int col);
  // #CAT_Config sets user data into the col

  double 		GetValAsDouble(int col, int row);
  // #CAT_Access get data of scalar type, in double form, for given col, row; if data is NULL, then 0 is returned
  bool 			SetValAsDouble(double val, int col, int row);
  // #CAT_Access set data of scalar type, in String form, for given column, row; does nothing if no cell' 'true' if set
  float 		GetValAsFloat(int col, int row);
  // #CAT_Access get data of scalar type, in float form, for given col, row; if data is NULL, then 0 is returned
  bool 			SetValAsFloat(float val, int col, int row);
  // #CAT_Access set data of scalar type, in String form, for given column, row; does nothing if no cell' 'true' if set
  const String 		GetValAsString(int col, int row) const;
  // #CAT_Access get data of scalar type, in String form, for given column, row; if data is NULL, then "n/a" is returned
  bool 			SetValAsString(const String& val, int col, int row);
  // #CAT_Access set data of scalar type, in String form, for given column, row; does nothing if no cell; 'true if set
  const Variant 	GetValAsVar(int col, int row) const;
  // #CAT_Access get data of scalar type, in Variant form, for given column, row; Invalid/NULL if no cell
  bool 			SetValAsVar(const Variant& val, int col, int row);
  // #CAT_Access set data of scalar type, in Variant form, for given column, row; does nothing if no cell; 'true' if set

  double 		GetValAsDoubleM(int col, int row, int cell);
  // #CAT_Access get data of matrix type, in double form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool 			SetValAsDoubleM(double val, int col, int row, int cell);
  // #CAT_Access set data of scalar type, in String form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  float 		GetValAsFloatM(int col, int row, int cell);
  // #CAT_Access get data of scalar type, in float form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool 			SetValAsFloatM(float val, int col, int row, int cell);
  // #CAT_Access set data of scalar type, in String form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  const String 		GetValAsStringM(int col, int row, int cell) const;
  // #CAT_Access get data of scalar type, in String form, for given column, row, and cell (flat index) in matrix; if data is NULL, then "n/a" is returned
  bool 			SetValAsStringM(const String& val, int col, int row, int cell);
  // #CAT_Access set data of scalar type, in String form, for given column, row, and cell (flat index) in matrix; does nothing if no cell; 'true if set
  const Variant 	GetValAsVarM(int col, int row, int cell) const;
  // #CAT_Access get data of scalar type, in Variant form, for given column, row, and cell (flat index) in matrix; Invalid/NULL if no cell
  bool 			SetValAsVarM(const Variant& val, int col, int row, int cell);
  // #CAT_Access set data of scalar type, in Variant form, for given column, row, and cell (flat index) in matrix; does nothing if no cell; 'true' if set

  taMatrix*	 	GetValAsMatrix(int col, int row);
  // #CAT_Access get data of matrix type, in Matrix form (one frame), for given column, row; Invalid/NULL if no cell; YOU MUST REF MATRIX; note: not const because you can write it
  bool 			SetValAsMatrix(const taMatrix* val, int col, int row);
  //#CAT_Access  set data of any type, in Variant form, for given column, row; does nothing if no cell; 'true' if set
  taMatrix*	 	GetRangeAsMatrix(int col, int st_row, int n_rows);
  // #CAT_Access get data as a Matrix for a range of rows, for given column, st_row, and n_rows; row; Invalid/NULL if no cell; YOU MUST REF MATRIX; note: not const because you can write it

  /////////////////////////////////////////////////////////
  // saving/loading (file)

  enum Delimiters {
    TAB,
    SPACE,
    COMMA,
  };

  // dumping and loading -- see .cpp file for detailed format information, not saved as standard taBase obj
  void 			SaveData_strm(ostream& strm, Delimiters delim = TAB, bool quote_str = true);
  // #CAT_File #EXT_dat saves data, one line per rec, with delimiter between columns, and optionally quoting strings
  void 			SaveHeader_strm(ostream& strm, Delimiters delim = TAB);
  // #CAT_File #EXT_dat saves header information, with delimiter between columns, and optionally quoting strings
  void 			SaveDataRow_strm(ostream& strm, int row=-1, Delimiters delim = TAB,
					 bool quote_str = true); 
  // #CAT_File #EXT_dat saves one row of data (-1 = last row), with delimiter between columns, and optionally quoting strings
  void 			SaveDataRows_strm(ostream& strm, Delimiters delim = TAB,
					 bool quote_str = true); 
  // #CAT_File #EXT_dat saves all rows of data (no header) with delimiter between columns, and optionally quoting strings

  void 			SaveData(const String& fname="", Delimiters delim = TAB, bool quote_str = true);
  // #CAT_File #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_dat saves data, one line per rec, with delimiter between columns, and optionally quoting strings; leave fname empty to pick from file chooser
  void 			AppendData(const String& fname="", Delimiters delim = TAB,
				   bool quote_str = true); 
  // #CAT_File #MENU #EXT_dat appends all of current datatable data to given file (does not output header; file assumed to be of same data structure
  void 			SaveHeader(const String& fname="", Delimiters delim = TAB);
  // #CAT_File #MENU #EXT_dat saves header information, with delimiter between columns, and optionally quoting strings; leave fname empty to pick from file chooser
  void 			SaveDataRow(const String& fname="", int row=-1, Delimiters delim = TAB,
					 bool quote_str = true); 
  // #CAT_File #MENU #EXT_dat saves one row of data (-1 = last row), with delimiter between columns, and optionally quoting strings; leave fname empty to pick from file chooser

  void			SaveDataLog(const String& fname="", bool append=false);
  // #CAT_File #MENU #MENU_SEP_BEFORE #ARGC_0 #EXT_dat incrementally save each new row of data that is written to the datatable (at WriteClose()) to given file.  writes the header first if not appending to existing file
  void			CloseDataLog();
  // #CAT_File #MENU #EXT_dat close the data log file if it was previously open

  static char		GetDelim(Delimiters delim);
  // #IGNORE get delimiter from enum
  static int		ReadTillDelim(istream& strm, String& str, const char delim, bool quote_str);
  // #IGNORE util function to read from stream into str until delim or newline or EOF
  static int_Array	load_col_idx; // #IGNORE mapping of column numbers in data load to column indexes based on header name matches
  static int_Array	load_mat_idx; // #IGNORE mapping of column numbers in data to matrix indicies in columns, based on header info

  void 			LoadData_strm(istream& strm, Delimiters delim = TAB,
				 bool quote_str = true, int max_recs = -1);
  // #CAT_File #EXT_dat loads data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings
  int 			LoadHeader_strm(istream& strm, Delimiters delim = TAB);
  // #CAT_File #EXT_dat loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end)
  int 			LoadDataRow_strm(istream& strm, Delimiters delim = TAB, bool quote_str = true);
  // #CAT_File #EXT_dat load one row of data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings (returns EOF if strm is at end)

  void 			LoadData(const String& fname, Delimiters delim = TAB,
				 bool quote_str = true, int max_recs = -1);
  // #CAT_File #MENU #MENU_SEP_BEFORE #EXT_dat loads data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings
  int 			LoadHeader(const String& fname, Delimiters delim = TAB);
  // #CAT_File #EXT_dat loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end)
  int 			LoadDataRow(const String& fname, Delimiters delim = TAB, bool quote_str = true);
  // #CAT_File #MENU #EXT_dat load one row of data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings (returns EOF if strm is at end)
  
 
  /////////////////////////////////////////////////////////
  // misc funs

  void			ShowInViewer(T3DataViewFrame* fr = NULL);
  // #NULL_OK #MENU #MENU_SEP_BEFORE #MENU_CONTEXT #CAT_Display open a view on this table (NULL=use blank if any, else make new frame)

  int  			MinLength();		// #IGNORE
  int  			MaxLength();		// #IGNORE

  DataTableModel*	GetDataModel();
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

  override int 		Dump_Load_Value(istream& strm, TAPtr par);

  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const DataTable& cp);
  COPY_FUNS(DataTable, DataBlock_Idx);
  TA_BASEFUNS(DataTable); //

protected: 
  override void	UpdateAfterEdit_impl();
  /////////////////////////////////////////////////////////
  // IMPL
  
  static int		idx_def_arg;
  // default arg val for functions returning index

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
  { DataArray_impl* da = data.SafeEl(chan);
    if (da) return da->name; else return _nilString; }

public:
  /////////////////////////////////////////////////////////
  // DataSource interface

  override int		SourceChannelCount() const { return ChannelCount(); }
  override const String	SourceChannelName(int chan) const { return ChannelName(chan); }
  override void		ResetData();
  // #MENU #MENU_ON_Data #CAT_Rows deletes all the data (rows), but keeps the column structure

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
  override bool		AddItem_impl(int n) {return AddRow(n);}
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
  
  DataArray_impl*	NewCol_impl(DataArray_impl::ValType val_type, 
				    const String& col_nm);
  // low-level create routine, shared by scalar and matrix creation, must be wrapped in StructUpdate
  DataArray_impl*	GetColForChannelSpec_impl(ChannelSpec* cs);
  DataArray_impl*	NewColFromChannelSpec_impl(ChannelSpec* cs);
  
private:
  void	Initialize();
  void	Destroy();
};

SmartRef_Of(DataTable) // DataTableRef


/////////////////////////////////////////////////////////
//   DataTable Group
/////////////////////////////////////////////////////////

class TA_API DataTable_Group : public taGroup<DataTable> {
  // ##CAT_Data group of data objects
INHERITED(taGroup<DataTable>)
public:
  TA_BASEFUNS(DataTable_Group);
  
private:
  void	Initialize() 		{ SetBaseType(&TA_DataTable); }
  void 	Destroy()		{ };
};


/////////////////////////////////////////////////////////
//   DataArray templates
/////////////////////////////////////////////////////////

template<class T> 
class DataArray : public DataArray_impl {
  // #VIRT_BASE #NO_INSTANCE template for common elements
public:
  override taMatrix* 	AR()	{ return &ar; } // the array pointer
  override const taMatrix* AR() const { return &ar; } // the array pointer

  void	CutLinks()
    {ar.CutLinks(); DataArray_impl::CutLinks();}
  void	Copy_(const DataArray<T>& cp)  { ar = cp.ar; }
  COPY_FUNS(DataArray<T>, DataArray_impl);
  TA_ABSTRACT_TMPLT_BASEFUNS(DataArray, T); //
public: //DO NOT ACCESS DIRECTLY
  T		ar;		// #NO_SHOW  the array itself
private:
  void	Initialize()		{}
  void	Destroy()		{ CutLinks(); }
};

class TA_API String_Data : public DataArray<String_Matrix> {
  // string data
INHERITED(DataArray<String_Matrix>)
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


class TA_API Variant_Data : public DataArray<Variant_Matrix> {
  // Variant data
INHERITED(DataArray<Variant_Matrix>)
friend class DataTable;
public:
  override ValType 	valType() const  {return VT_VARIANT;}

  TA_BASEFUNS(Variant_Data);

protected:
  override const Variant GetValAsVar_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override bool	 SetValAsVar_impl(const Variant& val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }

  override double 	GetValAsDouble_impl(int row, int cell) const
  { return GetValAsVar_impl(row, cell).toDouble(); }
  override float 	GetValAsFloat_impl(int row, int cell) const
  { return (float)GetValAsVar_impl(row, cell).toFloat(); }
  override int	 	GetValAsInt_impl(int row, int cell) const
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


class TA_API double_Data : public DataArray<double_Matrix> {
  // doubleing point data
INHERITED(DataArray<double_Matrix>)
friend class DataTable;
public:
  override bool		isNumeric() const {return true;} 
  override int		maxColWidth() const {return 15;} // assumes sign, int: 15 dig's; double: 14 dig's, decimal point
  override ValType 	valType() const {return VT_DOUBLE;}

  TA_BASEFUNS(double_Data);
  
protected:
  override double 	GetValAsDouble_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override float 	GetValAsFloat_impl(int row, int cell) const
  { return (float)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override int 		GetValAsInt_impl(int row, int cell) const
  { return (int)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }

  override bool	 	SetValAsDouble_impl(double val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	 	SetValAsFloat_impl(float val, int row, int cell)
  { ar.Set_Flat((double)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool	 	SetValAsInt_impl(int val, int row, int cell)
  { ar.Set_Flat((double)val, IndexOfEl_Flat(row, cell)); return true; }

private:
  void	Initialize() {}
  void	Destroy() {}
};

class TA_API float_Data : public DataArray<float_Matrix> {
  // floating point data
INHERITED(DataArray<float_Matrix>)
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

class TA_API int_Data : public DataArray<int_Matrix> {
  // int data
INHERITED(DataArray<int_Matrix>)
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

class TA_API byte_Data : public DataArray<byte_Matrix> {
  // byte data
INHERITED(DataArray<byte_Matrix>)
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

/*
  DataTable ViewSpecs
    "Rendering" for these specs is initializing all the viewable display
      parameters -- override the various Render_xxx routines to implement
*/

class TA_API DataColViewSpec: public ViewSpec {
  // ##SCOPE_DataColViewSpecs base specification for the display of data
INHERITED(ViewSpec)
friend class DataTableViewSpec;
public:
  bool			sticky; // #DEF_false set this to retain this colspec even if its column deletes

  DataArray_impl*	dataCol() const {return (DataArray_impl*)data();}
  void			setDataCol(DataArray_impl* value, bool first_time = false);
  
  DATAVIEW_PARENT(DataTableViewSpec)
//DataTableViewSpec*	parent() const;

  virtual void		setFont(const FontSpec& value) {} // for subclasses that implement
  bool			isVisible() const; // bakes in check for datacol
  
  override void		DataDestroying();
  
  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const DataColViewSpec& cp);
  COPY_FUNS(DataColViewSpec, inherited);
  TA_BASEFUNS(DataColViewSpec);
protected:
  override void		Unbind_impl(); // unbinds col
  virtual void		DataColUnlinked() {} // called if data set to NULL or destroys
  void			UpdateFromDataCol(bool first_time = false); // called if data set to column, or we otherwise need to update
  virtual void		UpdateFromDataCol_impl(bool first_time); 
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataColViewSpecs: public DataView_List {
INHERITED(DataView_List)
friend class DataTableViewSpec;
public:
  TA_DATAVIEWLISTFUNS(DataColViewSpecs, inherited, DataColViewSpec)
private:
  void	Initialize() {}
  void	Destroy() {}
};


class TA_API DataTableViewSpec: public ViewSpec {
  // base class for a viewspec of a datatable
INHERITED(ViewSpec)
public:
  FontSpec		font; // font for text (can generally be customized for cols etc.)
  DataColViewSpecs	col_specs;
  
  DataTable*		dataTable() const {return (DataTable*)data();}
  virtual void		setDataTable(DataTable* dt);
    // #MENU #NO_NULL build the spec from the given table
  
  override void		DataDestroying(); //
  
  void	InitLinks();
  void	CutLinks();
  void Copy_(const DataTableViewSpec& cp);
  COPY_FUNS(DataTableViewSpec, ViewSpec)
  TA_DATAVIEWFUNS(DataTableViewSpec, inherited) //
protected:
  override void 	DoActionChildren_impl(DataViewAction act);
  override void		Unbind_impl(); // unbinds table
  override void		DataStructUpdateEnd_impl();
  void			UpdateFromDataTable(bool first_time = false); // called if data set to table, or needs to be updated; calls _this then _child
  virtual void		UpdateFromDataTable_this(bool first); // does me (before kids)
  virtual void		UpdateFromDataTable_child(bool first);//does kids, usually not overridden
  virtual void		DataTableUnlinked(); // called if data is NULL or destroys
private:
  void Initialize();
  void Destroy();
};


class TA_API DataTableModel: public QAbstractTableModel {
  // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for tables;\ncreated and owned by the DataTable
INHERITED(QAbstractTableModel)
friend class DataTableCols;
friend class DataTable;
public:

  DataTable*		dataTable() const {return dt;}
  void			setDataTable(DataTable* value, bool notify = true);
  
  void			refreshViews(); // similar to matrix, issues dataChanged
  
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
protected:
  bool			ValidateIndex(const QModelIndex& index) const;
#endif
  void			emit_layoutChanged(); // we call this for most schema changes
protected:
  DataTable*		dt;
};


#endif // datatable_h
