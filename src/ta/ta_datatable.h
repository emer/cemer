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

#include "ta_group.h"
#include "ta_matrix.h"
#include "ta_data.h"
#include "ta_TA_type.h"

#ifndef __MAKETA__
# include <QAbstractTableModel> 
#endif


// forwards this file

class DataTable;
class DataTable_Group;
class String_Data; 
class Variant_Data; 
class float_Data;
class int_Data;
class byte_Data;
class DataTableModel;

// specific ones are in the template classes: String_Data, float_Data

/*
  Display Options (subclasses add new ones -- see each class)

  HIDDEN -- forces !visible by default
  TEXT -- sets display_style to TEXT
  NARROW -- sets display_style to TEXT; also, makes it !visible by default

*/

class TA_API DataArray_impl : public taNBase {
  // #VIRT_BASE ##NO_TOKENS #NO_INSTANCE holds a column of data;\n (a scalar cell can generally be treated as a degenerate matrix cell of dim[1])
INHERITED(taNBase)
friend class DataTable;
public:
  static void 		DecodeName(String nm, String& base_nm, int& vt, int& vec_col, int& col_cnt);
    // note: vt is -1 if unknown

  String		disp_opts;	// viewer default display options
  bool			mark; // #NO_SHOW #NO_SAVE clear on new and when col confirmed, used to delete orphans
  bool			pin; // set true to prevent this column from being deleted on orphan deleting
  bool			save_to_file;	// save this data to a file (e.g., to a log file in PDP++)?
  bool			is_matrix; // #READ_ONLY #SAVE #SHOW 'true' if the cell is a matrix, not a scalar
  MatrixGeom		cell_geom; //  #READ_ONLY #SAVE #SHOW for matrix cols, the geom of each cell
  
  virtual taMatrix* 	AR() = 0; // the matrix pointer -- NOTE: actual member should be called 'ar'
  virtual const taMatrix* 	AR() const = 0; // const version of the matrix pointer
  virtual bool		is_numeric() const {return false;} // true if data is float, int, or byte
  virtual bool		is_string() const {return false;}// true if data is string
  virtual int		cell_size() const // for matrix type, number of elements in each cell
     {return (is_matrix) ? cell_geom.Product() : 1;}
  virtual int		cell_dims() const // for matrix type, number of dimensions in each cell
     {return cell_geom.size;}
  virtual int		GetCellGeom(int dim) const  // for matrix type, size of given dim
    {return cell_geom.SafeEl(dim);}
  int			rows() const {return AR()->frames();}

  int			displayWidth() const; // low level display width, in tabs (8 chars/tab), taken from spec
  DataTable*		dataTable(); // root data table this col belongs to
  virtual int		maxColWidth() const {return -1;} // aprox max number of columns, in characters, -1 if variable or unknown
  virtual ValType 	valType() const = 0; // the type of data in each element

  static const KeyString key_val_type; // "val_type"
  static const KeyString key_disp_opts; // "disp_opts"
  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  override String	GetDisplayName() const; // #IGNORE we strip out the format characters
  
  // for accessor routines, row is absolute row in the matrix, not a DataTable row -- use the DataTable routines
  // -ve values are from end, and are valid for both low-level col access, and DataTable access
  const Variant GetValAsVar(int row) const {return GetValAsVar_impl(row, 0);}
    // valid for all types, -ve row is from end (-1=last)
  const String 	GetValAsString(int row) const {return GetValAsString_impl(row, 0);}
    // valid for all types, -ve row is from end (-1=last)
  float 	GetValAsFloat(int row) const {return GetValAsFloat_impl(row, 0);} 
    // valid if type is numeric, -ve row is from end (-1=last)
  int	 	GetValAsInt(int row) const {return GetValAsInt_impl(row, 0);} 
    // valid if type is int or byte, -ve row is from end (-1=last)
  byte	 	GetValAsByte(int row) const {return GetValAsByte_impl(row, 0);} 
    // valid only if type is byte, -ve row is from end (-1=last)
    
  // Matrix versions
  const Variant GetValAsVarM(int row, int cell) const {return GetValAsVar_impl(row, cell);} 
    // valid for all types, -ve row is from end (-1=last)
  const String 	GetValAsStringM(int row, int cell) const {return GetValAsString_impl(row, cell);} 
    // valid for all types, -ve row is from end (-1=last)
  float 	GetValAsFloatM(int row, int cell) const {return GetValAsFloat_impl(row, cell);} 
    // valid if type is numeric, -ve row is from end (-1=last)
  int	 	GetValAsIntM(int row, int cell) const {return GetValAsInt_impl(row, cell);} 
    // valid if type is int or byte, -ve row is from end (-1=last)
  byte	 	GetValAsByteM(int row, int cell) const {return GetValAsByte_impl(row, cell);} 
    // valid only if type is byte, -ve row is from end (-1=last)

  bool	 	SetValAsVar(const Variant& val, int row) 
    {return SetValAsVar_impl(val, row, 0);} 
    // valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsString(const String& val, int row) 
    {return SetValAsString_impl(val, row, 0);} 
    // valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsFloat(float val, int row) 
    // valid only if type is float, -ve row is from end (-1=last)
    {return SetValAsFloat_impl(val, row, 0);} 
  bool	 	SetValAsInt(int val, int row) 
    // valid if type is int or float, -ve row is from end (-1=last)
    {return SetValAsInt_impl(val, row, 0);} 
  bool	 	SetValAsByte(byte val, int row) 
    // valid if type is numeric, -ve row is from end (-1=last)
    {return SetValAsByte_impl(val, row, 0);} 
    
  // Matrix versions of scalar data 
  bool	 	SetValAsVarM(const Variant& val, int row, int cell) 
    {return SetValAsVar_impl(val, row, cell);} 
    // valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsStringM(const String& val, int row, int cell) 
    {return SetValAsString_impl(val, row, cell);} 
    // valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsFloatM(float val, int row, int cell) 
    // valid only if type is float, -ve row is from end (-1=last)
    {return SetValAsFloat_impl(val, row, cell);} 
  bool	 	SetValAsIntM(int val, int row, int cell) 
    // valid if type is int or float, -ve row is from end (-1=last)
    {return SetValAsInt_impl(val, row, cell);} 
  bool	 	SetValAsByteM(byte val, int row, int cell) 
    // valid if type is numeric, -ve row is from end (-1=last)
    {return SetValAsByte_impl(val, row, cell);} 

  void		EnforceRows(int rows);
  // force data to have this many rows

  // Matrix ops
  taMatrix*	GetValAsMatrix(int row);
    // gets the cell as a slice of the entire column (note: not const -- you can write it)
  bool	 	SetValAsMatrix(const taMatrix* val, int row);
    // set the matrix cell from a same-sized matrix 
  
  bool		HasDispOption(const String& opt) const
  { return disp_opts.contains(opt); } // check if a given display option is set
  const String 	DispOptionAfter(const String& opt) const;
  void		AddDispOption(const String& opt);

  override bool		Dump_QuerySaveMember(MemberDef* md); // dynamically check for saving data 
  
  virtual void Init(); // call this *after* creation, or in UAE, to assert matrix geometry
  void  UpdateAfterEdit();
  void	InitLinks(); //note: ok to do own AR here, because never called in constructor
  void	CutLinks(); //note: NOT ok to do disown AR here, because called in destructor
  void 	Copy_(const DataArray_impl& cp);
  COPY_FUNS(DataArray_impl, taNBase);
  TA_ABSTRACT_BASEFUNS(DataArray_impl);
  
protected:
  // in all accessor routines, -ve row is from end (-1=last)
  int			IndexOfEl_Flat(int row, int cell) const; 
    // -ve row is from end (-1=last); note: returns -ve value if out of range, so must use with SafeEl_Flat
  virtual const Variant GetValAsVar_impl(int row, int cell) const; 
  virtual const String 	GetValAsString_impl(int row, int cell) const; 
  virtual float 	GetValAsFloat_impl(int row, int cell) const 
    {return (float)GetValAsInt_impl(row, cell);}
  virtual int	 	GetValAsInt_impl(int row, int cell) const 
    {return (int)GetValAsByte_impl(row, cell);}
  virtual byte	 	GetValAsByte_impl(int row, int cell) const {return 0;} 
  
  virtual bool	 	SetValAsVar_impl(const Variant& val, int row, int cell); // true if set 
  virtual bool	 	SetValAsString_impl(const String& val, int row, int cell);  // true if set 
  virtual bool	 	SetValAsFloat_impl(float val, int row, int cell) {return false;}  // true if set 
  virtual bool	 	SetValAsInt_impl(int val, int row, int cell)  // true if set 
    {return SetValAsFloat_impl((float)val, row, cell);} 
  virtual bool	 	SetValAsByte_impl(byte val, int row, int cell)  // true if set 
    {return SetValAsInt_impl((int)val, row, cell);} 
  
private:
  void	Initialize();
  void	Destroy()	{CutLinks(); }; //
};


class TA_API DataTableCols: public taGroup<DataArray_impl> {
INHERITED(taGroup<DataArray_impl>)
public:
  override void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  
  override int		NumListCols() const {return 3;} // name, val_type (float, etc.), disp_opts
  override String	GetColHeading(const KeyString& key) const; // header text for the indicated column
  override const KeyString GetListColKey(int col) const;
  
  TA_BASEFUNS(DataTableCols); //
private:
  void	Initialize();
  void	Destroy()		{}
};

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
  // #NO_UPDATE_AFTER ##TOKENS ##CAT_Data table of data
INHERITED(DataBlock_Idx)
friend class DataTableCols;
friend class DataTableModel;
public:
  int 			rows; // #READ_ONLY #NO_SAVE #SHOW the number of rows
  bool			save_data; // 'true' if data should be saved in project; typically false for logs, true for data patterns
  DataTableCols	data; // all the columns and actual data
  
  int			cols() const {return data.leaves;}
  bool			hasData(int col, int row); // true if data at that cell
  bool			idx(int row_num, int col_size, int& act_idx) const
    {if (row_num < 0) row_num = rows + row_num;
    act_idx = col_size - (rows - row_num); return act_idx >= 0;} 
    // calculates an actual index for a col item, based on the current #rows and size of that col; returns 'true' if act_idx >= 0 (i.e., if there is a data item for that column)
  bool			RowInRangeNormalize(int& row); // normalizes row (if -ve) and tests result in range 
  virtual void		Reset();
  void			RemoveRow(int row_num);
    // #MENU Remove an entire row of data
//TODO if needed:  virtual void	ShiftUp(int num_rows);
  // remove indicated number of rows of data at front (typically used by Log to make more room in buffer)
  void			AddBlankRow() {if (AddRow(1)) wr_itr = rows - 1;}
  // #MENU add a new row to the data table
  bool			AddRow(int n); // add n rows, 'true' if added
  void			AllocRows(int n); // allocate space for at least n rows

  void			SetSaveToFile(bool save_to_file);
  // #MENU set the save_to_file flag for entire group of data elements

  void			UpdateAllRanges();
  // update all min-max range data for all float_Data elements in log

  void			RemoveCol(int col); // removes indicated column; 'true' if removed
  void			MarkCols(); // mark all cols before updating, for orphan deleting
  void			RemoveOrphanCols(); // removes all non-pinned marked cols
  
  bool 			ColMatchesChannelSpec(const DataArray_impl* da, const ChannelSpec* cs);
    // returns 'true' if the col has the same name and a compatible data type
  DataArray_impl*	NewCol(DataArray_impl::ValType val_type, 
    const String& col_nm, DataTableCols* col_gp = NULL);
   // #MENU #MENU_ON_Table #ARG_C_2 create new scalar column of data of specified type
  DataArray_impl*	NewColMatrix(DataArray_impl::ValType val_type, const String& col_nm,
    int dims = 1, int d0=0, int d1=0, int d2=0, int d3=0, int d4=0);
   // #MENU #MENU_ON_Table create new matrix column of data of specified type, with specified cell geom
  DataArray_impl*	NewColMatrixN(DataArray_impl::ValType val_type, 
    const String& col_nm,  const MatrixGeom& cell_geom);
   // create new matrix column of data of specified type, with specified cell geom
  
  float_Data*		NewColFloat(const String& col_nm); 
    // create new column of floating point data
  int_Data*		NewColInt(const String& col_nm); 	 
    // create new column of integer-level data (= narrow display, actually stored as float)
  String_Data*		NewColString(const String& col_nm); 
    // create new column of string data

  DataArray_impl*	FindColName(const String& col_nm, int& col_idx, int val_type = -1,  
    int dims = -1, int d0=0, int d1=0, int d2=0, int d3=0, int d4=0);
  // find a column of the given name.  if further parameters are specified (val_type of type taBase::ValType, number of dimensions, etc) they are also matching criteria

  DataArray_impl*	FindMakeColName(const String& col_nm, int& col_idx, DataArray_impl::ValType val_type,  int dims = 1, int d0=0, int d1=0, int d2=0, int d3=0, int d4=0);
  // find a column of the given name, val type, and dimension. if one does not exist, then create it.  Note that dims < 1 means make a scalar column, not a matrix
    
  DataTableCols*	NewGroupFloat(const String& base_nm, int n); 
    // OBS create new sub-group of floats of size n, named as base_nm_index
  DataTableCols*	NewGroupInt(const String& base_nm, int n); 
    // OBS create new sub-group of ints of size n, named as base_nm_index
  DataTableCols*	NewGroupString(const String& base_nm, int n); 
    // OBS create new sub-group of strings of size n, named as base_nm_index

  DataArray_impl*	NewColFromChannelSpec(ChannelSpec* cs)
   // #MENU_1N create new matrix column of data based on name/type in the data item (default is Variant)
    {if (cs) return NewColFromChannelSpec_impl(cs); else return NULL;}
    
  DataArray_impl*	GetColForChannelSpec(ChannelSpec* cs)
   // #MENU_1N find existing or create new matrix column of data based on name/type in the data item
    {if (cs) return GetColForChannelSpec_impl(cs); else return NULL;}
    
  DataArray_impl* 	GetColData(int col) const;
    // get col data for given leaf column 
  taMatrix*		GetColMatrix(int col) const;
    // get matrix for given leaf column -- WARNING: this is NOT row-number safe 

  void			SetColName(const String& col_nm, int col);
  // set column name for given column
  void			AddColDispOpt(const String& dsp_opt, int col);
  // add display option for given leaf column
  
  float 		GetValAsFloat(int col, int row);
  // get data of scalar type, in float form, for given leaf col, row; if data is NULL, then 0 is returned
  bool 			SetValAsFloat(float val, int col, int row);
  // set data of scalar type, in String form, for given leaf column, row; does nothing if no cell' 'true' if set
  const String 		GetValAsString(int col, int row) const;
  // get data of scalar type, in String form, for given leaf column, row; if data is NULL, then "n/a" is returned
  bool 			SetValAsString(const String& val, int col, int row);
  // set data of scalar type, in String form, for given leaf column, row; does nothing if no cell; 'true if set

  const Variant 	GetValAsVar(int col, int row) const;
  // get data of scalar type, in Variant form, for given column, row; Invalid/NULL if no cell
  bool 			SetValAsVar(const Variant& val, int col, int row);
  // set data of scalar type, in Variant form, for given leaf column, row; does nothing if no cell; 'true' if set

  taMatrix*	 	GetValAsMatrix(int col, int row);
  // get data of matrix type, in Matrix form (one frame), for given column, row; Invalid/NULL if no cell; YOU MUST REF MATRIX; note: not const because you can write it
  bool 			SetValAsMatrix(const taMatrix* val, int col, int row);
  // set data of any type, in Variant form, for given leaf column, row; does nothing if no cell; 'true' if set

  // dumping and loading -- see .cpp file for detailed format information, not saved as standard taBase obj
  void 			SaveHeader(ostream& strm); // saves header information, tab-separated, 
  void 			SaveData(ostream& strm); // saves data, one line per rec, tab-separated
  void 			LoadHeader(istream& strm); // loads header information -- preserves current headers if possible
  void 			LoadData(istream& strm, int max_recs = -1); // loads data, up to max num of recs (-1 for all)
  
  int  			MinLength();		// #IGNORE
  int  			MaxLength();		// #IGNORE

  DataTableModel*	GetDataModel(); // #IGNORE returns new if none exists, or existing -- enables views to be shared

  override int 		Dump_Load_Value(istream& strm, TAPtr par);

  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const DataTable& cp);
  COPY_FUNS(DataTable, DataBlock_Idx);
  TA_BASEFUNS(DataTable); //

protected: 
  void			RowsAdding(int n, bool begin); // indicate beginning and end of row adding -- you have to pass the same n each time; NOT nestable

public: // DataBlock i/f and common routines
  override DBOptions	dbOptions() const // options the instance type support
    {return (DBOptions)(DB_IND_SEQ_SRC_SNK | DB_SINK_DYNAMIC);} 
  override int		ItemCount() const {return rows;} 
protected: // DataBlock i/f and common routines
  inline int		channelCount() const {return data.leaves;} // #IGNORE
  inline const String	channelName(int chan) const  // #IGNORE
    {DataArray_impl* da = data.Leaf(chan);
     if (da) return da->name; else return _nilString;}

public: // DataSource i/f
  override int		sourceChannelCount() const {return channelCount();}
    // for combo src/sinks where channels are all the same
  override const String	sourceChannelName(int chan) const 
    {return channelName(chan);}
  override void		ResetData();
    // #MENU #MENU_ON_Actions deletes all the data, but keeps the column structure
protected: // DataSource i/f
  override const Variant GetData_impl(int chan)
  {return GetValAsVar(chan, rd_itr);}
  override taMatrix*	GetMatrixData_impl(int chan);//
//  virtual bool		ReadItem_impl() {return true;} 

public: // DataSink i/f
  override int		sinkChannelCount() const {return channelCount();}
  override const String	sinkChannelName(int chan) const {return channelName(chan);}//
  override bool		AddSinkChannel(ChannelSpec* cs); 
  override bool		AssertSinkChannel(ChannelSpec* cs); //
protected: // DataSink i/f
  override bool		AddItem_impl(int n) {return AddRow(n);} // adds n items
  override void		DeleteSinkChannel_impl(int chan) {RemoveCol(chan);}
  override taMatrix*	GetSinkMatrix_impl(int chan) 
    {return GetValAsMatrix(chan, wr_itr);} //note: DS refs it
  override bool		SetData_impl(const Variant& data, int chan) 
    {return SetValAsVar(data, chan, wr_itr);}
  override bool		SetMatrixData_impl(const taMatrix* data, int chan) 
    {return SetValAsMatrix(data, chan, wr_itr);}

protected:
  DataTableModel*	m_dm; // #IGNORE note: once we create, always exists
  
  DataArray_impl*	NewCol_impl(DataArray_impl::ValType val_type, 
    const String& col_nm, DataTableCols* col_gp = NULL);
   // low-level create routine, shared by scalar and matrix creation, must be wrapped in StructUpdate, col_gp=NULL means data
  DataArray_impl*	GetColForChannelSpec_impl(ChannelSpec* cs);
  DataArray_impl*	NewColFromChannelSpec_impl(ChannelSpec* cs);
  
private:
  void	Initialize();
  void	Destroy();
};

SmartRef_Of(DataTable) // DataTableRef


class TA_API DataTable_Group : public taGroup<DataTable> {
  // group of data objects
INHERITED(taGroup<DataTable>)
public:
  TA_BASEFUNS(DataTable_Group);
  
private:
  void	Initialize() 		{ SetBaseType(&TA_DataTable); }
  void 	Destroy()		{ };
};


template<class T> 
class DataArray : public DataArray_impl { // #VIRT_BASE #NO_INSTANCE template for common elements
public:
  override taMatrix* 	AR()	{ return &ar; } // the array pointer
  override const taMatrix* AR() const { return &ar; } // the array pointer

  void	CutLinks()
    {ar.CutLinks(); DataArray_impl::CutLinks();}
  void	Copy_(const DataArray<T>& cp)  {ar = cp.ar;}
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
  override bool		is_string() const {return true;} 
  override ValType 	valType() const  {return VT_STRING;}

  TA_BASEFUNS(String_Data);

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
    {return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell));}
  override bool	 	SetValAsVar_impl(const Variant& val, int row, int cell)
    {ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true;}

private:
  void	Initialize() {}
  void	Destroy() {}
};


class TA_API float_Data : public DataArray<float_Matrix> {
  // floating point data
INHERITED(DataArray<float_Matrix>)
friend class DataTable;
public:
  override bool		is_numeric() const {return true;} 
  override int		maxColWidth() const {return 7;} // assumes sign, int: 6 dig's; float: 5 dig's, decimal point
  override ValType 	valType() const {return VT_FLOAT;}

  TA_BASEFUNS(float_Data);
  
protected:
  override float 	GetValAsFloat_impl(int row, int cell) const
    {return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell));}
  override bool	 	SetValAsFloat_impl(float val, int row, int cell)
    {ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true;}

private:
  void	Initialize() {}
  void	Destroy() {}
};

class TA_API int_Data : public DataArray<int_Matrix> {
  // int data
INHERITED(DataArray<int_Matrix>)
friend class DataTable;
public:
  override bool		is_numeric() const {return true;} // 
  override int		maxColWidth() const {return 11;} // assumes sign, 10 digs
  override ValType 	valType() const {return VT_INT;}

  TA_BASEFUNS(int_Data);
  
protected:
  override int 		GetValAsInt_impl(int row, int cell) const
    {return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell));}
  override bool	 	SetValAsInt_impl(int val, int row, int cell)
    {ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true;}

private:
  void	Initialize() {}
  void	Destroy() {}
};

class TA_API byte_Data : public DataArray<byte_Matrix> {
  // byte data
INHERITED(DataArray<byte_Matrix>)
friend class DataTable;
public:
  override bool		is_numeric() const {return true;} // 
  override int		maxColWidth() const {return 3;} // assumes 3 digs
  override ValType 	valType() const {return VT_BYTE;}

  TA_BASEFUNS(byte_Data);
  
protected:
  override byte 	GetValAsByte_impl(int row, int cell) const
    {return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell));}
  override bool	 	SetValAsByte_impl(byte val, int row, int cell)
    {ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true;}

private:
  void	Initialize() {}
  void	Destroy() {}
};


class TA_API DataTableModel: public QAbstractTableModel { // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for tables;\ncreated and owned by the DataTable
INHERITED(QAbstractTableModel)
friend class DataTableCols;
friend class DataTable;
public:

  DataTable*		dataTable() const {return dt;}
  void			setDataTable(DataTable* value, bool notify = true);
  
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
