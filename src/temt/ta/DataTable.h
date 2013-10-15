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

#ifndef DataTable_h
#define DataTable_h 1

// parent includes:
#include <DataBlock_Idx>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <DataTableCols>
#include <int_Matrix>
#include <int_Array>
#include <AnalysisRun>

// declare all other types mentioned but not required to include:
class cssProgSpace; // 
class taFiler; // 
class ChannelSpec; // 
class FixedWidthSpec; // 
class DataCol; // 
class MatrixGeom; // 
class taMatrix; // 
class double_Data; // 
class float_Data; // 
class int_Data; // 
class String_Data; // 
class GridTableView; // 
class T3Panel; // 
class GraphTableView; // 
class iDataTableModel; // #IGNORE
class DataSortSpec; //
class DataSelectSpec; //

/*
  DataTable Notifications

  Structural Changes -- SLS_STRUCT_UPDATE_BEGIN..SLS_STRUCT_UPDATE_END
    this includes adding and removing data columns
  Row Adding/Removing -- SLS_DATA_UPDATE_BEGIN..SLS_DATA_UPDATE_END
    this includes adding and removing whole rows, or individual items to a row -- if the client
    code calls RowsAdding/RowsAdded

  Row Numbers
    - all fuctions using row numbers work properly for jagged tables, i.e. those to which
      columns have been added after some rows already exist
    - NOTE: functions with row numbers did NOT have this correct behavior in v3.2
    - unless noted, row<0 means access from the end, ex. -1 is last row
 */

taTypeDef_Of(DataTable);

class TA_API DataTable : public DataBlock_Idx {
  // ##TOKENS ##CAT_Data ##FILETYPE_DataTable ##EXT_dtbl ##DEF_CHILD_data ##DEF_CHILDNAME_Columns ##DUMP_LOAD_POST ##UNDO_BARRIER table of data containing columns of a fixed data type and geometry, with data added row-by-row
  INHERITED(DataBlock_Idx)
    friend class DataTableCols;
  friend class iDataTableModel;
public:
  enum DataFlags { // #BITS flags for data table
    DF_NONE             = 0, // #NO_BIT
    SAVE_ROWS           = 0x0001, // Store this DataTable's row-data directly in the project file.  Uncheck for the option to store row-data externally.  Note: the DataTable's schema (column names, types, and other configuration information) is always saved directly in the project.
    HAS_CALCS           = 0x0002, // #NO_SHOW at least one of the columns has CALC flag set
    AUTO_CALC           = 0x0004, // Automatically calculate columns.
    SAVE_FILE           = 0x0008, // If an AUTO_LOAD filename is set, this option causes row-data to be saved to that file whenever the project is saved.
  };

  enum AutoLoadMode {
    NO_AUTO_LOAD,               // Do not automatically load a data file.
    AUTO_LOAD,                  // Automatically load a data file after loading the project.
    PROMPT_LOAD,                // Prompt to load a data file after loading the project.  In -nogui mode, this is the same as AUTO_LOAD.
  };

  /////////////////////////////////////////////////////////
  //    Main datatable interface:
  int                   rows;
  // #READ_ONLY #NO_SAVE #SHOW The number of rows of data (that are visible - after filtering/removing)
  int                   rows_total;
  // #READ_ONLY #NO_SAVE #HIDDEN The number of rows of actual data (visible or hidden)
  DataTableCols         data;
  // all the columns and actual data
  DataFlags             data_flags;
  // Flags for various features and state of the DataTable:
  AutoLoadMode          auto_load;
  // #CONDEDIT_OFF_data_flags:SAVE_ROWS Whether to automatically load a data file when the DataTable object is loaded.  This option is only available when SAVE_ROWS is unchecked.  Storing row-data externally reduces the project file size (especially for large data tables), but the project is no longer self contained.
  String                auto_load_file;
  // #CONDEDIT_OFF_auto_load:NO_AUTO_LOAD #FILE_DIALOG_LOAD #COMPRESS #FILETYPE_DataTable #EXT_dat,dtbl Where to store and load row-data from if AUTO_LOAD option is set.  (*.dtbl files are loaded using internal Load format, otherwise LoadData is used.)
  Variant               keygen; // #HIDDEN #VARTYPE_READ_ONLY #GUI_READ_ONLY 64bit int used to generate keys; advance to get next key; only reset if all data reset

  int_Matrix            row_indexes;     // #EXPERT #CAT_Access array with indicies providing view into rows in this datatable -- ALL DATA ACCESS GOES THROUGH THESE indexes and it is always kept up to date
  // #READ_ONLY #HIDDEN The number of rows of actual data (visible or hidden)
  cssProgSpace*         calc_script;
  // #IGNORE script object for performing column calculations
  taFiler*              log_file;
  // #NO_SAVE #HIDDEN file for logging data incrementally as it is written -- only for output.  a new line is written when WriteClose() (DataSink interface) is called.

  /////////////////////////////////////////////
  // Flags

  inline void           SetDataFlag(DataFlags flg)   { data_flags = (DataFlags)(data_flags | flg); }
  // #CAT_ObjectMgmt set data column flag state on
  inline void           ClearDataFlag(DataFlags flg) { data_flags = (DataFlags)(data_flags & ~flg); }
  // #CAT_ObjectMgmt clear data column flag state (set off)
  inline bool           HasDataFlag(DataFlags flg) const { return (data_flags & flg); }
  // #CAT_ObjectMgmt check if data column flag is set
  inline void           SetDataFlagState(DataFlags flg, bool on)
  { if(on) SetDataFlag(flg); else ClearDataFlag(flg); }
  // #CAT_ObjectMgmt set data column flag state according to on bool (if true, set flag, if false, clear it)

  /////////////////////////////////////////////////////////
  // saving/loading (file)

  enum Delimiters {
    TAB,
    SPACE,
    COMMA,
  };

  enum LoadDelimiters { // same as Delimiters, only for LoadData gui
    LD_AUTO     = -1, // #LABEL_AUTO
    LD_TAB, // #LABEL_TAB
    LD_SPACE, // #LABEL_SPACE
    LD_COMMA, // #LABEL_COMMA
  };

  enum LoadQuotes { // same as Delimiters, only for LoadData gui
    LQ_AUTO     = -1, //  #LABEL_AUTO
    LQ_NO, // #LABEL_NO
    LQ_YES, // #LABEL_YES
  };

  static char           GetDelim(Delimiters delim);
  // #IGNORE get delimiter from enum

  static int            ReadTillDelim(std::istream& strm, String& str, const char delim,
      bool quote_str, bool& got_quote);
  // #IGNORE util function to read from stream into str until delim or newline or EOF
  static int            ReadTillDelim_Str(const String& istr, int& idx, String& str,
      const char delim, bool quote_str, bool& got_quote);
  // #IGNORE util function to read from String into str until delim or newline or EOF -- idx is current index and is also updated to new index

  // dumping and loading -- see .cpp file for detailed format information, not saved as standard taBase obj
  virtual void          SaveData_strm(std::ostream& strm, Delimiters delim = TAB,
      bool quote_str = true, bool save_headers=true);
  // #EXPERT #CAT_File #EXT_dat,tsv,csv,txt,log saves data, one line per rec, with delimiter between columns, and optionally quoting strings -- if save-headers then special _H: formatted column header information is saved and data rows are marked with _D:
  void                  SaveHeader_strm(std::ostream& strm, Delimiters delim = TAB,
      bool row_mark = true, int col_fr = 0, int col_to = -1)
  {SaveHeader_strm_impl(strm, delim, row_mark, col_fr, col_to, true, false);}
  // #EXPERT #CAT_File #EXT_dat,txt,log saves header information, with delimiter between columns, and optional row_mark _H:
  virtual void          SaveDataRow_strm(std::ostream& strm, int row=-1,
      Delimiters delim = TAB, bool quote_str = true,
      bool row_mark = true, int col_fr = 0, int col_to = -1);
  // #EXPERT #CAT_File #EXT_dat,tsv,csv,txt,log saves one row of data (-1 = last row), with delimiter between columns, and optionally quoting strings; -ve cols/rows are relative to end -- if row_mark then mark data rows with _D: at start (to differentiate from _H: headers)
  virtual void          SaveDataRows_strm(std::ostream& strm, Delimiters delim = TAB,
      bool quote_str = true, bool row_mark=true);
  // #EXPERT #CAT_File #EXT_dat,tsv,csv,txt,log saves all rows of data (no header) with delimiter between columns, and optionally quoting strings -- if row_mark then mark data rows with _D: at start (to differentiate from _H: headers)

  virtual void          SaveData(const String& fname="", Delimiters delim = TAB,
      bool quote_str = true, bool save_headers=true);
  // #CAT_File saves data, one line per rec, with delimiter between columns, and optionally quoting strings; if save_headers then the _H: header line is saved and data lines have _D:, else they are bare
  void                  SaveData_Gui(const String& fname="")
  {SaveData(fname, TAB, true, true);}
  // #CAT_File #LABEL_Save_Data #BUTTON #MENU #MENU_ON_Data #MENU_SEP_BEFORE #EXT_dat,txt,log #FILE_DIALOG_SAVE saves data in the default Emergent file format
  virtual void          AppendData(const String& fname="", Delimiters delim = TAB,
      bool quote_str = true, bool row_mark=true);
  // #CAT_File appends all of current datatable data to given file (does not output header; file assumed to be of same data structure -- if row_mark then mark data rows with _D: at start (to differentiate from _H: headers)
  void                  SaveAppendData_Gui(const String& fname="")
  {AppendData(fname, TAB, true, true);}
  // #CAT_File #LABEL_Save_Append_Data #MENU #EXT_dat,txt,log  #FILE_DIALOG_APPEND appends all of current datatable data to given file in Emergent file format (does not output header; file assumed to be of same data structure)
  virtual void          SaveHeader(const String& fname="", Delimiters delim = TAB);
  // #CAT_File saves header information in Emergent file format, with delimiter between columns
  void                  SaveHeader_Gui(const String& fname="")
  {SaveHeader(fname, TAB);}
  // #CAT_File #MENU #LABEL_Save_Header #EXT_dat,txt,log saves #FILE_DIALOG_SAVE saves header information in Emergent file format
  virtual void          SaveDataRow(const String& fname="", int row=-1,
      Delimiters delim = TAB, bool quote_str = true, bool row_mark=true);
  // #CAT_File saves one row of data (-1 = last row), with delimiter between columns, and optionally quoting strings -- if row_mark then mark data rows with _D: at start (to differentiate from _H: headers)

  virtual void          SaveDataLog(const String& fname="", bool append=false,
      bool dmem_proc_0 = true);
  // #CAT_File #EXT_dat,tsv,csv,txt,log #FILE_DIALOG_SAVE incrementally save each new row of data that is written to the datatable (at WriteClose()) to given file.  writes the header first if not appending to existing file.  if running under demem, dmem_proc_0 determines if only the first processor writes to the log file, or if all processors write
  virtual void          CloseDataLog();
  // #CAT_File close the data log file if it was previously open
  virtual bool          WriteDataLogRow();
  // #CAT_File write the current row to the data log, if it is open (returns true if successfully wrote) -- this is automatically called by WriteClose

  virtual void          LoadData_strm(std::istream& strm, Delimiters delim = TAB,
      bool quote_str = true, int max_recs = -1);
  // #EXPERT #CAT_File #EXT_dat,tsv,csv,txt,log loads data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings
  virtual int           LoadHeader_strm(std::istream& strm, Delimiters delim = TAB);
  // #EXPERT #CAT_File #EXT_dat,tsv,csv,txt,log loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end) -- note: Emergent-native does not use quotes in header line and quote_str is ignore when native=true
  virtual int           LoadDataRow_strm(std::istream& strm, Delimiters delim = TAB,
      bool quote_str = true);
  // #EXPERT #CAT_File #EXT_dat,tsv,csv,txt,log load one row of data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings (returns EOF if strm is at end)
  int                   LoadDataRowEx_strm(std::istream& strm, Delimiters delim = TAB,
      bool quote_str = true, bool reset_load_schema = true); // #IGNORE used by Server

  virtual void          LoadData(const String& fname, Delimiters delim = TAB,
      bool quote_str = true, int max_recs = -1, bool reset_first=false);
  // #CAT_File #MENU #MENU_ON_Data #MENU_SEP_BEFORE #FILE_DIALOG_LOAD #LABEL_Load_Data_Native #EXT_dat,tsv,csv,txt,log load Emergent native format data (ONLY) - has a special header to define columns, up to max num of recs (-1 for all), with delimiter between columns and optionally quoting strings, reset_first = remove any existing data prior to loading -- this is much faster than LoadAnyData, especially noticible for very large data files
  virtual void          LoadAnyData(const String& fname, bool headers = true,
      LoadDelimiters delim = LD_AUTO, LoadQuotes quote_str = LQ_AUTO,
      int max_rows = -1,  bool reset_first=false);
  // #CAT_File #EXT_dat,tsv,csv,txt,log load any kind of data -- either the Emergent native file format (which has a special header to define columns) or delimited import formats -- auto detect works in most cases for delimiters and string quoting, reset_first = reset any existing data before loading (else append) -- headers option MUST be set correctly for non-Emergent files (no auto detect on that), and it is ignored for Emergent native files (which always have headers)

  virtual void          LoadAnyData_stream(std::istream &stream, bool append, bool has_header_line = true);
  // #CAT_File Load any kind of data from an existing stream.  File format, delimitation, and quoting are auto-detected, but whether the file has a header line or not must be set explicitly.  Set append=false to overwrite any existing data.

  virtual void          LoadAnyData_gui(const String& fname, bool headers = true)
  { LoadAnyData(fname, headers, LD_AUTO, LQ_AUTO, -1, true); }
  // #CAT_File #BUTTON #MENU #MENU_ON_Data #MENU_SEP_BEFORE #EXT_dat,tsv,csv,txt,log #FILE_DIALOG_LOAD #LABEL_Load_Any_Data load any kind of data -- either the Emergent native file format (which has a special header to define columns) or delimited import formats -- auto detects type of delimiters and string quoting -- headers option MUST be set correctly for non-Emergent files (no auto detect on that), and it is ignored for Emergent native files (which always have headers).  See ImportData to manually specify delimiters if auto detect doesn't work.  See Load Any Data Append to append to existing data (this function resets data first)
  virtual void          LoadAnyData_Append(const String& fname, bool headers = true)
  { LoadAnyData(fname, headers, LD_AUTO, LQ_AUTO, -1, false); }
  // #CAT_File #MENU #MENU_ON_Data #EXT_dat,tsv,csv,txt,log  #FILE_DIALOG_LOAD load any kind of data -- either the Emergent native file format (which has a special header to define columns) or delimited import formats -- auto detects type of delimiters and string quoting -- headers option MUST be set correctly for non-Emergent files (no auto detect on that), and it is ignored for Emergent native files (which always have headers).  See ImportData to manually specify delimiters if auto detect doesn't work.  See Load Any Data to replace existing data (this function appends to existing)
  virtual void          LoadDataFixed(const String& fname, FixedWidthSpec* fws,
      bool reset_first=false);
  // #CAT_File loads data, using the specified fixed-width spec (usually in a Program), reset_first = remove any existing data prior to loading
  virtual int           LoadHeader(const String& fname, Delimiters delim = TAB);
  // #CAT_File #EXT_dat,tsv,csv,txt,log loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end)
  virtual int           LoadDataRow(const String& fname, Delimiters delim = TAB,
      bool quote_str = true);
  // #CAT_File #EXT_dat,tsv,csv,txt,log  #FILE_DIALOG_LOAD load one row of data, up to max num of recs (-1 for all), with delimiter between columns and optionaly quoting strings (returns EOF if strm is at end)
  void                  ResetLoadSchema() const; // #IGNORE can be used by ex Server to reset the load schema at beginning of a load

  virtual void          ExportHeader_strm(std::ostream& strm, Delimiters delim = TAB,
      bool quote_str = true, int col_fr = 0, int col_to = -1)
  { SaveHeader_strm_impl(strm, delim, false, col_fr, col_to, false, quote_str); }
  // #EXPERT #CAT_File #EXT_csv,tsv,txt,log saves header information, with delimiter between columns
  void                  ExportData_strm(std::ostream& strm, Delimiters delim = COMMA,
      bool quote_str = true, bool headers = true);
  // #EXPERT #CAT_File #EXT_csv,tsv,txt,log exports data with given delimiter and string quoting format options in a format suitable for importing into other applications (spreadsheets, etc) -- does NOT include the emergent native header/data row markers and extended header info, so is not good for loading back into emergent (use SaveData for that)
  void                  ExportData(const String& fname="", Delimiters delim = COMMA,
      bool quote_str = true, bool headers = true);
  // #CAT_File #MENU #MENU_ON_Data #MENU_SEP_BEFORE #EXT_csv,tsv,txt,log #FILE_DIALOG_SAVE exports data with given delimiter and string quoting format options in a format suitable for importing into other applications (spreadsheets, etc) -- does NOT include the emergent native header/data row markers and extended header info, so is not good for loading back into emergent (use SaveData for that)
  virtual void          ImportData(const String& fname="", bool headers = true,
      LoadDelimiters delim = LD_AUTO, LoadQuotes quote_str = LQ_AUTO)
  { LoadAnyData(fname, headers, LD_AUTO, LQ_AUTO, -1, true); }
  // #CAT_File #MENU #MENU_ON_Data #EXT_csv,tsv,txt,log #FILE_DIALOG_LOAD imports externally-generated data in delimited text file format -- if headers is selected, then first row is treated as column headers -- auto defaults are typically fine (see also Load Any Data or Load Any Data Append -- same functionality with all AUTO defaults)
  virtual void          ShowAllRows();
  // #CAT_DataProc #MENU #MENU_ON_DataProc #FROM_GROUP_data #NULL_OK show all available rows of data in the table, in their original raw order, effectively undoing any prior sort, filter, or row removal functions -- the DataTable rows are accessed via a set of indexes that are what is actually sorted and filtered by the relevant functions. Therefore, you can instantly regain access to the original unsorted and unfiltered rows of data by resetting these indexes to be in one-to-one correspondence with the raw data stored in the table.  Note that *all* access to the DataTable rows goes through the indexes -- it is not possible to otherwise access the raw data directly.  See also the Flatten function.
  virtual bool          Flatten();
  // #CAT_DataProc #MENU #MENU_ON_DataProc #LABEL_Flatten permanently removes any currently invisible data rows and arranges raw memory in current order -- useful for optimizing and locking-in data table size after a series of operations.  The DataTable rows are accessed via a set of indexes that are what is actually sorted and filtered by the relevant functions -- *all* access to the DataTable rows goes through the indexes -- so the Flatten function will not change the appearance of the data table, but it can optimize processing in large data tables, and establish the baseline state that the ShowAllRows function will revert to.  See also the FlattenTo and ShowAllRows functions.
  virtual bool          FlattenTo(DataTable* flattened_table);
  // #CAT_DataProc copies currently visible rows of data to given table, without including any currently invisible data rows  and arranges raw memory in current order -- useful for optimizing and locking-in data table size after a series of operations.  The DataTable rows are accessed via a set of indexes that are what is actually sorted and filtered by the relevant functions -- *all* access to the DataTable rows goes through the indexes -- so the FlattenTo function will not change the appearance of the data table, but it can optimize processing in large data tables, and establish the baseline state that the ShowAllRows function will revert to.  See also the FlattenTo and ShowAllRows functions.

  ////////////////////////////////////////////////////////////
  //    protected Load/Save and other implementation code
protected:

  iDataTableModel*      table_model; // #IGNORE for gui view/model stuff
  static int_Array      load_col_idx; // #IGNORE mapping of column numbers in data load to column indexes based on header name matches
  static int_Array      load_mat_idx; // #IGNORE mapping of column numbers in data to matrix indicies in columns, based on header info

  virtual bool          CopyCell_impl(DataCol* dar, int dest_row,
      const DataTable& src, DataCol* sar, int src_row); // #IGNORE

  virtual void          DetermineLoadDataParams(std::istream& strm,
                              bool headers_req, LoadDelimiters delim_req,
                              LoadQuotes quote_str_req, bool& headers,
                              Delimiters& delim, bool& quote_str, bool& native);
  // #IGNORE determine delimiters and other info from file for loading data -- for non-native files, will also decode headers and/or create data columns to fit the data -- the first line of actual data after the header (if any) is used to determine column type info
  void                  ImportData_strm(std::istream& strm, bool headers = true,
                                        Delimiters delim = COMMA, bool quote_str = true, int max_recs = -1);
  // #IGNORE loads simple delimited data with undecorated headers and rows (such as csv files from Excel etc.) -- must be called AFTER ImportHeaderCols -- just skips headers

  void                  SaveHeader_strm_impl(std::ostream& strm, Delimiters delim,
                                             bool row_mark, int col_fr, int col_to,
                                             bool native, bool quote_str);
  // #IGNORE
  virtual int           LoadHeader_impl(std::istream& strm, Delimiters delim,
                                        bool native, bool quote_str = false);
  // #IGNORE #CAT_File #EXT_dat,tsv,csv,txt,log loads header information -- preserves current headers if possible (called from LoadData if header line found) (returns EOF if strm is at end)
  virtual int           LoadDataRow_impl(std::istream& strm, Delimiters delim = TAB,
                                         bool quote_str = true);
  // #IGNORE load one row, impl

  virtual void          ImportHeaderCols(const String& hdr_line, const String& data_line,
                                         Delimiters delim, bool quote_str = false);
  // decode headers and/or create data columns to fit the data -- the first line of actual data after the header (if any) is used to determine column type info
  virtual void          DecodeImportHeaderName(String nm, String& base_nm, int& cell_idx);
  // decode header information for importing from text files -- if cell_idx >= 0 then we found an approprite mat col when nm was <Name>_<int>
  virtual ValType       DecodeImportDataType(const String& dat_str);
  // decode header information for importing from text files -- if cell_idx >= 0 then we found an approprite mat col when nm was <Name>_<int>

  bool                  SetValAsMatrix_impl(const taMatrix* val, DataCol* da, int row,
                                            bool quiet = false);

  virtual void          ComputeRowsTotal();
  // recompute rows_total based on actual data 

public:

  /////////////////////////////////////////////////////////
  // columns

  int                   cols() const { return data.size; }
  // #CAT_Columns number of columns
  int                   CellsPerRow() const;
  // #CAT_Columns compute the total number of cells (single values) used per row in the entire data table (iterates over columns, adds up cell_size())
  int                   Cells() const { return CellsPerRow() * rows; }
  // #CAT_Columns compute the total number of cells used in the entire data table (CellsPerRow() * rows)
  override taList_impl* children_() {return &data;}
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const
  { return data.Elem(idx, mode); }

  virtual DataCol*      NewCol(DataCol::ValType val_type,
                               const String& col_nm);
  // #CAT_Columns create new scalar column of data of specified type
  virtual DataCol*      NewCol_gui(DataCol::ValType val_type,
                                   const String& col_nm);
  // #BUTTON #MENU #MENU_ON_Columns #LABEL_NewCol #CAT_Columns create new scalar column of data of specified type -- void* is NOT currently supported and will fail
  virtual DataCol*      NewColMatrix(DataCol::ValType val_type, const String& col_nm,
                                     int dims = 1, int d0=0, int d1=0, int d2=0, int d3=0, int d4=0, int d5=0, int d6=0);
  // #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  virtual DataCol*      NewColMatrixN(DataCol::ValType val_type,
                                      const String& col_nm,  const MatrixGeom& cell_geom,
                                      int& col_idx);
  // #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  virtual DataCol*      NewColMatrixN_gui(DataCol::ValType val_type,
                                          const String& col_nm,  const MatrixGeom& cell_geom);
  // #BUTTON #MENU #LABEL_NewColMatrix #CAT_Columns create new matrix column of data of specified type, with specified cell geom
  DataCol*      NewColFmMatrix(taMatrix* mat, const String& col_nm);
  // #CAT_Columns Create a new matrix column in table from mat. Creates a row if there aren't any and copies mat into the first cell of the new column.

  virtual double_Data*  NewColDouble(const String& col_nm);
  // #CAT_Columns create new column of double data
  virtual float_Data*   NewColFloat(const String& col_nm);
  // #CAT_Columns create new column of floating point data
  virtual int_Data*     NewColInt(const String& col_nm);
  // #CAT_Columns create new column of integer-level data (= narrow display, actually stored as float)
  virtual String_Data*  NewColString(const String& col_nm);
  // #CAT_Columns create new column of string data

  virtual void          SetColName(const String& col_nm, int col);
  // #CAT_Columns set column name for given column
  virtual bool          RenameCol(const String& cur_nm, const String& new_nm);
  // #CAT_Columns rename column with current name cur_nm to new name new_nm (returns false if ccur_nm not found)

  virtual DataCol*      FindColName(const String& col_nm, bool err_msg = false) const;
  // #CAT_Columns find a column of the given name; if err_msg then generate an error if not found
  virtual int           FindColNameIdx(const String& col_nm, bool err_msg = false) const;
  // #CAT_Columns find a column index of the given name; if err_msg then generate an error if not found

  DataCol*              FindMakeCol(const String& col_nm, ValType val_type);
  // #CAT_Columns insures that a scalar column of the given name and val type exists, and return that col.
  DataCol*              FindMakeColMatrix(const String& col_nm,
                                          ValType val_type, int dims = 1,
                                          int d0=0, int d1=0, int d2=0, int d3=0, int d4=0, int d5=0, int d6=0);
  // #CAT_Columns insures that a matrix column of the given name, val type, and dimensions exists, and returns that col.
  DataCol*              FindMakeColMatrixN(const String& col_nm,
                                           ValType val_type, const MatrixGeom& cell_geom,
                                           int& col_idx); // #IGNORE
  virtual DataCol*      FindMakeColName(const String& col_nm, int& col_idx,
                                        ValType val_type, int dims = 0,
                                        int d0=0, int d1=0, int d2=0, int d3=0, int d4=0, int d5=0, int d6=0);
  // #EXPERT #CAT_Columns find a column of the given name, val type, and dimension. if one does not exist, then create it.  Note that dims < 1 means make a scalar column, not a matrix

  void                  ChangeColTypeGeom_impl(DataCol* src, ValType val_type,
                                               const MatrixGeom& geom);
  // #IGNORE impl func, called by Col or other funcs here; new is diff from exist; geom.dims=0 for scalar, otherwise matrix

  virtual void          ChangeColTypeGeom(const String& col_nm, ValType val_type, int dims = 0,
                                          int d0=0, int d1=0, int d2=0, int d3=0,
                                          int d4=0, int d5=0, int d6=0);
  // #CAT_Columns change type and/or geometry of column with given name -- preserves as much data as possible subject to these changes

  virtual void          ChangeColType(const Variant& col, ValType new_val_type);
  // #CAT_Columns change data type of column -- preserves data subject to constraints of type change
  virtual void          ChangeAllColsOfType(ValType cur_val_type, ValType new_val_type);
  // #CAT_Columns change data type of all columns that are currently of cur_val_type to new_val_type -- preserves data subject to constraints of type change

  using inherited::GetColData;
  virtual DataCol*      GetColData(const Variant& col, bool quiet = false) const {
    if(col.isStringType()) return FindColName(col.toString(), !quiet);
    DataCol* rval = data.SafeEl(col.toInt());
    TestError(!quiet && !rval, "GetColData", "column number is out of range", col.toString());
    return rval;
  }
  // #CAT_Columns get col data for given column
  virtual taMatrix*     GetColMatrix(const Variant& col) const
  { DataCol* da = GetColData(col); if (da) return da->AR(); else return NULL; }
  // #CAT_Columns get matrix for given column -- WARNING: this is NOT row-number safe

  virtual bool          ColMatchesChannelSpec(const DataCol* da, const ChannelSpec* cs);
  // #CAT_Columns returns 'true' if the col has the same name and a compatible data type
  virtual DataCol*      NewColFromChannelSpec(ChannelSpec* cs)
  // #MENU_1N #CAT_Columns create new matrix column of data based on name/type in the data item (default is Variant)
  { if (cs) return NewColFromChannelSpec_impl(cs); else return NULL; }

  virtual DataCol*      GetColForChannelSpec(ChannelSpec* cs)
  // #MENU_1N #CAT_Columns find existing or create new matrix column of data based on name/type in the data item
  {if (cs) return GetColForChannelSpec_impl(cs); else return NULL;}

  void                  MoveCol(int old_index, int new_index);
  // #CAT_Columns move the column from position old to position new
  virtual void          RemoveCol(const Variant& col);
  // #CAT_Columns removes indicated column
  void                  RemoveAllCols() { Reset(); }
  // #CAT_Columns #MENU #MENU_ON_Columns #CONFIRM remove all columns (and data)
  virtual void          Reset();
  // #CAT_Columns remove all columns (and data) -- this cannot be undone!

  virtual void          MarkCols();
  // #CAT_Columns mark all cols before updating, for orphan deleting
  virtual void          RemoveOrphanCols();
  // #CAT_Columns removes all non-pinned marked cols
  virtual void 			DuplicateCol(const Variant& col);
  // #CAT_Columns duplicates indicated column

  /////////////////////////////////////////////////////////
  // rows

  virtual void          ResetRowIndexes();
  // #CAT_Rows reset the row indexes to be 0..rows-1 to reflect the full extent of the natural underlying data in the table

  virtual bool          hasData(int col, int row);
  // #CAT_Rows true if data at that cell
  bool                  idx(int row_num, int& act_idx) const;
  // #CAT_Rows pass in the current row as viewed and get back the "true" row
  inline bool           idx_err(int row_num, int& act_idx,
                                bool quiet = false) const {
    bool rval = idx(row_num, act_idx);
    if(!quiet) TestError(!rval, "idx_err", "index out of range"); return rval; }
  // #IGNORE
  inline bool           idx_warn(int row_num, int& act_idx,
                                 bool quiet = false) const {
    bool rval = idx(row_num, act_idx);
    if(!quiet) TestWarning(!rval, "idx_err", "index out of range"); return rval; }
  // #IGNORE
  virtual bool          RowInRangeNormalize(int& row);
  // #EXPERT #CAT_Rows normalizes row (if -ve) and tests result in range
  virtual void          AllocRows(int n);
  // #CAT_Rows allocate space for at least n rows
  virtual int           AddBlankRow();
  // #CAT_Rows add a new row to the data table, sets read (source) and write (sink) index to this last row (as in ReadItem or WriteItem), so that subsequent data routines refer to this new row, and returns row #
  virtual bool          AddRows(int n = 1);
  // #BUTTON #MENU #MENU_ON_Rows #CAT_Rows add n rows, returns true if successfully added
  virtual bool          InsertRows(int st_row, int n_rows=1);
  // #MENU #MENU_ON_Rows #CAT_Rows insert n rows at starting row number, returns true if successfully inserted
  virtual bool          InsertRowsAfter(int st_row, int n_rows=1);
  // #CAT_Rows insert N rows after st_row + n_rows, returns true if successfully inserted
  virtual bool          RemoveRows(int st_row, int n_rows=1);
  // #MENU #MENU_ON_Rows #CAT_Rows Remove n rows of data, starting at st_row.  st_row = -1 means last row, and n_rows = -1 means remove from starting row to end
  virtual void          RemoveAllRows() { ResetData(); }
  // #MENU #CAT_Rows #CONFIRM remove all of the rows of data, but keep the column structure
  virtual void          EnforceRows(int n_rows);
  // #CAT_Rows ensure that there are exactly n_rows in the table, removing or adding as needed
  virtual bool          DuplicateRow(int row_no, int n_copies=1);
  // #MENU #CAT_Rows duplicate given row number, making given number of copies of it (adds new rows at the end)
  bool                  DuplicateRows(int st_row, int n_rows=1);
  // #CAT_Rows copy the n_rows starting from st_row and insert them immediately after selected rows
  virtual void          MoveRow(int old_index, int new_index);
  // #CAT_Rows should only be called internally
  virtual bool          AppendRows(DataTable* append_from);
  // #CAT_Rows #DROP1 #MENU append rows from another data table -- this is also available in taDataProc and in the GUI as a drag-and-drop action (appends rows of dropped item onto dropped item)
  const Variant         GetColUserData(const String& name, const Variant& col) const;
  // #CAT_Config gets user data from the column (col can be an index or a name)
  void                  SetColUserData(const String& name, const Variant& value, const Variant& col);
  // #CAT_Config sets user data into the column  (col can be an index or a name) -- use this e.g., to configure various parameters that are used by the grid and graph views, such as IMAGE, MIN, MAX, TOP_ZERO

  /////////////////////////////////////////////////////////
  // Main data value access/modify (Get/Set) routines: for Programs and very general use

  const Variant         GetVal(const Variant& col, int row) const;
  // #CAT_Access get data of scalar type, in Variant form (any data type, use for Programs), for given column, row -- column can be specified as either integer index or a string that is then used to find the given column name
  bool                  SetVal(const Variant& val, const Variant& col, int row);
  // #CAT_Modify set data of scalar type, in Variant form (any data type, use for Programs), for given column, row -- column can be specified as either integer index or a string that is then used to find the given column name; returns 'true' if valid access and set is successful

  const Variant         GetMatrixVal(const Variant& col, int row,
                                     int d0, int d1=0, int d2=0, int d3=0) const;
  // #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies -- column can be specified as either integer index or a string that is then used to find the given column name
  bool                  SetMatrixVal(const Variant& val, const Variant& col, int row,
                                     int d0, int d1=0, int d2=0, int d3=0);
  // #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies -- column can be specified as either integer index or a string that is then used to find the given column name; returns 'true' if valid access and set is successful

  const Variant         GetMatrixFlatVal(const Variant& col, int row, int cell) const;
  // #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix flat cell index (regardless of dimensions) -- column can be specified as either integer index or a string that is then used to find the given column name
  bool                  SetMatrixFlatVal(const Variant& val, const Variant& col, int row, int cell);
  // #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix flat cell index (regardless of dimensions) -- column can be specified as either integer index or a string that is then used to find the given column name; returns 'true' if valid access and set is successful

  bool          InitVals(const Variant& init_val, const Variant& col);
  // #CAT_Modify initialize all values in given column to given value -- column can be specified as either integer index or a string that is then used to find the given column name
  bool          InitValsToRowNo(const Variant& col);
  // #CAT_Modify initialize all values in given column to be equal to the row number -- only valid for scalar (not matrix) columns -- column can be specified as either integer index or a string that is then used to find the given column name

  int           FindVal(const Variant& val, const Variant& col, int st_row = 0,
                        bool not_found_err = false) const;
  // #CAT_Access find row number for given value within column col of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end) -- column can be specified as either integer index or a string that is then used to find the given column name.  returns -1 if not found, and issues error if not_found_err is true
  Variant       LookupVal(const Variant& find_val, const Variant& find_in_col,
                          const Variant& value_col, int st_row = 0,
                          bool not_found_err = true) const;
  // #CAT_Access lookup value in a given row of table by finding value in another column -- uses row number from finding find_val in find_in_col column to access value in value_col, which is returned as a variant (must be a scalar).  starts at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end) -- column can be specified as either integer index or a string that is then used to find the given column name.  returns null (invalid) variant if find_val not found, and issues error if not_found_err is true

  int           FindMultiVal(int st_row, const Variant& val1, const Variant& col1,
                             const Variant& val2=0, const Variant& col2="",
                             const Variant& val3=0, const Variant& col3="",
                             const Variant& val4=0, const Variant& col4="",
                             const Variant& vall5=0, const Variant& col5="",
                             const Variant& val6=0, const Variant& col6="") const;
  // #CAT_Access find row number for multiple values across different columns of scalar type, starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end) -- columns can be specified as either integer index or a string that is then used to find the given column name

  /////////////////////////////
  // column name versions:

  const Variant         GetValColName(const String& col_name, int row,
                                      bool quiet = false) const;
  // #EXPERT #CAT_Access get data of scalar type, in Variant form (any data type, use for Programs), for given column name, row  -- quiet = fail quietly
  bool                  SetValColName(const Variant& val, const String& col_name, int row,
                                      bool quiet = false);
  // #EXPERT #CAT_Modify set data of scalar type, in Variant form (any data type, use for Programs), for given column name, row; returns 'true' if valid access and set is successful -- quiet = fail quietly

  const Variant         GetMatrixValColName(const String& col_name, int row,
                                            int d0, int d1=0, int d2=0, int d3=0, bool quiet = false) const;
  // #EXPERT #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column name, row, and matrix dimension indicies -- quiet = fail quietly
  bool                  SetMatrixValColName(const Variant& val, const String& col_name,
                                            int row, int d0, int d1=0, int d2=0, int d3=0, bool quiet = false);
  // #EXPERT #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies; returns 'true' if valid access and set is successful -- quiet = fail quietly

  const Variant         GetMatrixFlatValColName(const String& col_name, int row,
                                                int cell, bool quiet = false) const;
  // #EXPERT #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column name, row, and flat matrix cell index (flat index into elements of the matrix, regardless of dimensionality) -- quiet = fail quietly
  bool                  SetMatrixFlatValColName(const Variant& val, const String& col_name,
                                                int row, int cell, bool quiet = false);
  // #EXPERT #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and flat matrix cell index (flat index into elements of the matrix, regardless of dimensionality); returns 'true' if valid access and set is successful -- quiet = fail quietly

  bool          InitValsColName(const Variant& init_val, const String& col_name);
  // #EXPERT #CAT_Modify initialize all values in column of given name to given value
  bool          InitValsToRowNoColName(const String& col_name);
  // #EXPERT #CAT_Modify initialize all values in column of given name to be equal to the row number -- only valid for scalar (not matrix) columns

  int           FindValColName(const Variant& val, const String& col_name, int st_row = 0) const;
  // #EXPERT #CAT_Access find row number for given value within column col of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)

  int           FindMultiValColName(int st_row, const Variant& val1, const String& col_name1,
                                    const Variant& val2=0, const String& col_name2="",
                                    const Variant& val3=0, const String& col_name3="",
                                    const Variant& val4=0, const String& col_name4="",
                                    const Variant& vall5=0, const String& col_name5="",
                                    const Variant& val6=0, const String& col_name6="") const;
  // #EXPERT #CAT_Access find row number for multiple values across different columns of scalar type, starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)

  /////////////////////////////
  // column and row name versions:

  const Variant         GetValColRowName(const String& col_name, const String& row_col_name,
                                         const Variant& row_value, bool quiet = false) const;
  // #EXPERT #CAT_Access get data of scalar type, in Variant form (any data type, use for Programs), for given column name, and row by looking up row_value in column named row_col_name -- quiet = fail quietly
  bool                  SetValColRowName(const Variant& val, const String& col_name,
                                         const String& row_col_name, const Variant& row_value,
                                         bool quiet = false);
  // #EXPERT #CAT_Modify set data of scalar type, in Variant form (any data type, use for Programs), for given column name, and row by looking up row_value in column named row_col_name; returns 'true' if valid access and set is successful -- quiet = fail quietly

  const Variant         GetMatrixValColRowName(const String& col_name,
                                               const String& row_col_name, const Variant& row_value,
                                               int d0, int d1=0, int d2=0, int d3=0, bool quiet = false) const;
  // #EXPERT #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column name, row, and matrix dimension indicies -- quiet = fail quietly
  bool                  SetMatrixValColRowName(const Variant& val, const String& col_name,
                                               const String& row_col_name, const Variant& row_value,
                                               int d0, int d1=0, int d2=0, int d3=0, bool quiet = false);
  // #EXPERT #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix dimension indicies; returns 'true' if valid access and set is successful -- quiet = fail quietly

  const Variant         GetMatrixFlatValColRowName(const String& col_name,
                                                   const String& row_col_name, const Variant& row_value,
                                                   int cell, bool quiet = false) const;
  // #EXPERT #CAT_Access get data of matrix type, in Variant form (any data type, use for Programs), for given column name, row, and matrix cell index (flat index into matrix cells) -- quiet = fail quietly
  bool                  SetMatrixFlatValColRowName(const Variant& val, const String& col_name,
                                                   const String& row_col_name, const Variant& row_value,
                                                   int cell, bool quiet = false);
  // #EXPERT #CAT_Modify set data of matrix type, in Variant form (any data type, use for Programs), for given column, row, and matrix cell index (flat index into matrix cells); returns 'true' if valid access and set is successful -- quiet = fail quietly

  /////////////////////////////
  // column pointer versions, just for the gui:

  bool          InitValsCol(DataCol* col, const Variant& init_val)
  { return col->InitVals(init_val); }
  // #EXPERT #CAT_Modify #MENU #MENU_ON_Columns #MENU_SEP_BEFORE #LABEL_InitVals #FROM_GROUP_data initialize all values in given column to given value
  bool          InitValsToRowNoCol(DataCol* col)
  { return col->InitValsToRowNo(); }
  // #CAT_Columns #MENU #LABEL_InitValsToRowNo #FROM_GROUP_data initialize all values in given column to be equal to the row number -- only valid for scalar (not matrix) columns
  int           FindValCol(DataCol* col, const Variant& val, int st_row = 0) const
  { return col->FindVal(val, st_row); }
  // #EXPERT #CAT_Access #MENU #FROM_GROUP_data #USE_RVAL find row number for given value within column col of scalar type (use for Programs), starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)
  int           FindMultiValCol(int st_row, const Variant& val1, DataCol* col1,
                                const Variant& val2=0, DataCol* col2=NULL,
                                const Variant& val3=0, DataCol* col3=NULL,
                                const Variant& val4=0, DataCol* col4=NULL,
                                const Variant& vall5=0, DataCol* col5=NULL,
                                const Variant& val6=0, DataCol* col6=NULL) const;
  // #EXPERT #CAT_Access #MENU #FROM_GROUP_data #USE_RVAL find row number for multiple values across different columns of scalar type, starting at given starting row number.  if st_row < 0 then the search proceeds backwards from that many rows from end (-1 = end)


  /////////////////////////////////////////////////////////
  // Expert data value access routines: optimized for different types

  ///////////////
  // Scalar

  const Variant         GetValAsVar(const Variant& col, int row) const;
  // #EXPERT #CAT_Access get data of scalar type, in Variant form, for given column, row
  bool                  SetValAsVar(const Variant& val, const Variant& col, int row);
  // #EXPERT #CAT_Modify set data of scalar type, in Variant form, for given column, row; returns 'true' if valid access and set is successful
  double                GetValAsDouble(const Variant& col, int row);
  // #EXPERT #CAT_Access get data of scalar type, in double form, for given col, row; if data is NULL, then 0 is returned
  bool                  SetValAsDouble(double val, const Variant& col, int row);
  // #EXPERT #CAT_Modify set data of scalar type, in double form, for given column, row; does nothing if no cell' 'true' if set
  float                 GetValAsFloat(const Variant& col, int row);
  // #EXPERT #CAT_Access get data of scalar type, in float form, for given col, row; if data is NULL, then 0 is returned
  bool                  SetValAsFloat(float val, const Variant& col, int row);
  // #EXPERT #CAT_Modify set data of scalar type, in float form, for given column, row; does nothing if no cell' 'true' if set
  int                   GetValAsInt(const Variant& col, int row);
  // #EXPERT #CAT_Access get data of scalar type, in int form, for given col, row; if data is NULL, then 0 is returned
  bool                  SetValAsInt(int val, const Variant& col, int row);
  // #EXPERT #CAT_Modify set data of scalar type, in int form, for given column, row; does nothing if no cell' 'true' if set
  const String          GetValAsString(const Variant& col, int row) const;
  // #EXPERT #CAT_Access get data of scalar type, in String form, for given column, row; if data is NULL, then "n/a" is returned
  bool                  SetValAsString(const String& val, const Variant& col, int row);
  // #EXPERT #CAT_Modify set data of scalar type, in String form, for given column, row; does nothing if no cell; 'true if set

  ///////////////
  // Matrix, Flat Idx

  const Variant         GetValAsVarM(const Variant& col, int row, int cell) const;
  // #EXPERT #CAT_Access get data of matrix type, in Variant form, for given column, row, and cell (flat index) in matrix
  bool                  SetValAsVarM(const Variant& val, const Variant& col, int row, int cell);
  // #EXPERT #CAT_Modify set data of matrix type, in Variant form, for given column, row, and cell (flat index) in matrix; returns 'true' if valid access and set is successful
  double                GetValAsDoubleM(const Variant& col, int row, int cell);
  // #EXPERT #CAT_Access get data of matrix type, in double form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool                  SetValAsDoubleM(double val, const Variant& col, int row, int cell);
  // #EXPERT #CAT_Modify set data of matrix type, in double form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  float                 GetValAsFloatM(const Variant& col, int row, int cell);
  // #EXPERT #CAT_Access get data of matrix type, in float form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool                  SetValAsFloatM(float val, const Variant& col, int row, int cell);
  // #EXPERT #CAT_Modify set data of matrix type, in float form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  int                   GetValAsIntM(const Variant& col, int row, int cell);
  // #EXPERT #CAT_Access get data of matrix type, in int form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool                  SetValAsIntM(int val, const Variant& col, int row, int cell);
  // #EXPERT #CAT_Modify set data of matrix type, in int form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  const String          GetValAsStringM(const Variant& col, int row, int cell,
                                        bool na = true) const;
  // #EXPERT #CAT_Access get data of matrix type, in String form, for given column, row, and cell (flat index) in matrix; if data is NULL, then na="n/a" else "" is returned
  bool                  SetValAsStringM(const String& val, const Variant& col, int row, int cell);
  // #EXPERT #CAT_Modify set data of matrix type, in String form, for given column, row, and cell (flat index) in matrix; does nothing if no cell; 'true if set

  ///////////////
  // Matrix, Dims

  const Variant         GetValAsVarMDims(const Variant& col, int row,
                                         int d0, int d1=0, int d2=0, int d3=0) const;
  // #EXPERT #CAT_Access get data of matrix type, in Variant form, for given column, row, and matrix dimension indicies
  bool                  SetValAsVarMDims(const Variant& val, const Variant& col, int row,
                                         int d0, int d1=0, int d2=0, int d3=0);
  // #EXPERT #CAT_Modify set data of matrix type, in Variant form, for given column, row, and matrix dimension indicies; returns 'true' if valid access and set is successful

  double                GetValAsDoubleMDims(const Variant& col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #EXPERT #CAT_Access get data of matrix type, in double form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool                  SetValAsDoubleMDims(double val, const Variant& col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #EXPERT #CAT_Modify set data of matrix type, in double form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  float                 GetValAsFloatMDims(const Variant& col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #EXPERT #CAT_Access get data of matrix type, in float form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool                  SetValAsFloatMDims(float val, const Variant& col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #EXPERT #CAT_Modify set data of matrix type, in float form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  int                   GetValAsIntMDims(const Variant& col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #EXPERT #CAT_Access get data of matrix type, in int form, for given col, row, and cell (flat index) in matrix; if data is NULL, then 0 is returned
  bool                  SetValAsIntMDims(int val, const Variant& col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #EXPERT #CAT_Modify set data of matrix type, in int form, for given column, row, and cell (flat index) in matrix; does nothing if no cell' 'true' if set
  const String          GetValAsStringMDims(const Variant& col, int row, int d0, int d1=0, int d2=0, int d3=0,
                                            bool na = true) const;
  // #EXPERT #CAT_Access get data of matrix type, in String form, for given column, row, and cell (flat index) in matrix; if data is NULL, then na="n/a" else "" is returned
  bool                  SetValAsStringMDims(const String& val, const Variant& col, int row, int d0, int d1=0, int d2=0, int d3=0);
  // #EXPERT #CAT_Modify set data of matrix type, in String form, for given column, row, and cell (flat index) in matrix; does nothing if no cell; 'true if set

  //////////////////////
  //    Entire Matrix

  taMatrix*             GetValAsMatrix(const Variant& col, int row);
  // #CAT_Access get data of matrix type, in Matrix form (one frame), for given column, row; Invalid/NULL if no cell; must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it; note: not const because you can write it
  taMatrix*             GetValAsMatrixColName(const String& col_name, int row, bool quiet = false);
  // #CAT_Access get data of matrix type, in Matrix form (one frame), for given column, row; Invalid/NULL if no cell; must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it; note: not const because you can write it -- quiet = fail quietly
  taMatrix*             GetValAsMatrixColRowName(const String& col_name,
                                                 const String& row_col_name, const Variant& row_value, bool quiet = false);
  // #EXPERT #CAT_Access get data of matrix type, in Matrix form (one frame), for given column name, and row by looking up row_value in column named row_col_name; Invalid/NULL if no cell; must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it; note: not const because you can write it -- quiet = fail quietly
  bool                  SetValAsMatrix(const taMatrix* val, const Variant& col, int row);
  // #CAT_Modify  set data of any type, in Variant form, for given column, row; does nothing if no cell; 'true' if set
  bool                  SetValAsMatrixColName(const taMatrix* val, const String& col_name,
                                              int row, bool quiet = false);
  // #CAT_Modify  set data of any type, in Variant form, for given column, row; does nothing if no cell; 'true' if set -- quiet = fail quietly
  taMatrix*             GetRangeAsMatrix(const Variant& col, int st_row, int n_rows);
  // #EXPERT #CAT_Access get data as a Matrix for a range of rows, for given column, st_row, and n_rows; row; Invalid/NULL if no cell; must do taBase::Ref(mat) and taBase::unRefDone(mat) on return value surrounding use of it; note: not const because you can write it

  ///////////////////////////////////////
  // sub-matrix reading and writing functions

  virtual void  WriteFmSubMatrix(const Variant& col, int row,
                                 const taMatrix* submat_src, taMatrix::RenderOp render_op = taMatrix::COPY,
                                 int off0=0, int off1=0, int off2=0,
                                 int off3=0, int off4=0, int off5=0, int off6=0);
  // #EXPERT #CAT_SubMatrix for making larger patterns out of smaller ones (sub-matricies) and vice-versa: write to matrix cell in this table at given col, row from source sub-matrix (typically of smaller size), using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost
  virtual void  ReadToSubMatrix(const Variant& col, int row,
                                taMatrix* submat_dest, taMatrix::RenderOp render_op = taMatrix::COPY,
                                int off0=0, int off1=0, int off2=0,
                                int off3=0, int off4=0, int off5=0, int off6=0);
  // #EXPERT #CAT_SubMatrix for making larger patterns out of smaller ones (sub-matricies) and vice-versa: read from matrix cell in this table at given col, row to dest sub-matrix (typically of smaller size), using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost

  virtual void  WriteFmSubMatrixTable(const Variant& col, int row,
                                      const DataTable* submat_src, const Variant& submat_src_col, int submat_src_row,
                                      taMatrix::RenderOp render_op = taMatrix::COPY,
                                      int off0=0, int off1=0, int off2=0,
                                      int off3=0, int off4=0, int off5=0, int off6=0);
  // #CAT_SubMatrix for making larger patterns out of smaller ones (sub-matricies) and vice-versa: write to matrix cell in this table at given col, row, from source sub-matrix cell (typically of smaller size) in submat_src table at given submat_src_col, submat_src_row, using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost
  virtual void  ReadToSubMatrixTable(const Variant& col, int row,
                                     const DataTable* submat_dest, const Variant& submat_dest_col, int submat_dest_row,
                                     taMatrix::RenderOp render_op = taMatrix::COPY,
                                     int off0=0, int off1=0, int off2=0,
                                     int off3=0, int off4=0, int off5=0, int off6=0);
  // #CAT_SubMatrix for making larger patterns out of smaller ones (sub-matricies) and vice-versa: read from matrix cell in this table at given col, row, to dest sub-matrix cell (typically of smaller size) in submat_dest table at submat_dest_col, submat_dest_row, using given render operation to combine source and destination values, starting at given offsets in this matrix (safely manages range issues, clipping out of bounds) -- uses Variant interface, so type conversion between matricies is automatic, with some overhead cost

  virtual void  WriteFmSubMatrixTableLookup(const Variant& col, int row,
                                            const DataTable* submat_src, const Variant& submat_src_col,
                                            Variant submat_lookup_val, const Variant& submat_lookup_col,
                                            taMatrix::RenderOp render_op, const DataTable* offset_lookup,
                                            Variant offset_col, const Variant& offset_lookup_val, const Variant& offset_lookup_col);
  // #CAT_SubMatrix for making larger patterns out of smaller ones (sub-matricies) and vice-versa: write to matrix cell in this table at given col, row, from source sub-matrix cell (typically of smaller size) in submat_src table at given submat_src_col, at row given by looking up submat_loop_val in submat_lookup_col, using given render operation to combine source and destination values, starting at offsets found in a matrix cell in offset_lookup table, searching in lookup_col for value lookup_val to select the row, and getting the offsets from column offset_col
  virtual void  ReadToSubMatrixTableLookup(const Variant& col, int row,
                                           const DataTable* submat_dest, const Variant& submat_dest_col,
                                           Variant submat_lookup_val, const Variant& submat_lookup_col,
                                           taMatrix::RenderOp render_op, const DataTable* offset_lookup,
                                           Variant offset_col, const Variant& offset_lookup_val, const Variant& offset_lookup_col);
  // #CAT_SubMatrix for making larger patterns out of smaller ones (sub-matricies) and vice-versa: read from matrix cell in this table at given col, row, to dest sub-matrix cell (typically of smaller size) in submat_dest table at submat_dest_col, at row given by looking up submat_loop_val in submat_lookup_col, using given render operation to combine source and destination values, starting at offsets found in a matrix cell in offset_lookup table, searching in lookup_col for value lookup_val to select the row, and getting the offsets from column offset_col

  //////////////////////////////
  //    Misc funs

  int                   GetMaxCellRows(int col_fr, int col_to); // #IGNORE get the max muber of cell rows in this col range (used for clip operations)
  void                  GetFlatGeom(const CellRange& cr, int& tot_cols,
                                    int& max_cell_rows); // #IGNORE get the total flat cols and max rows per cell; used for TSV output
  iDataTableModel*       GetTableModel(); // #IGNORE gets or makes the model -- kept around once made

  String                HeaderToTSV(); // #IGNORE for tsv save
  String                RangeToTSV(const CellRange& cr); // #IGNORE for clip operations

  /////////////////////////////////////////////////////////
  // calculated column values

  virtual bool          UpdateColCalcs();
  // #IGNORE update column calculations
  virtual bool          CalcLastRow();
  // #IGNORE if HAS_CALCS, does calculations for last row of data -- called by WriteClose

  virtual bool          CheckForCalcs();
  // #IGNORE see if any columns have CALC flag -- sets HAS_CALCS flag -- returns state of flag
  virtual void          InitCalcScript();
  // #IGNORE initialize the calc_script for computing column calculations
  virtual bool          CalcAllRows_impl();
  // #IGNORE perform calculations for all rows of data (calls InitCalcScript to make sure)
  virtual bool          CalcAllRows();
  // #BUTTON #GHOST_OFF_data_flags:HAS_CALCS #CAT_Calc perform calculations for all rows of data (updates after)
  virtual void          CalcRowCodeGen(String& code_str);
  // #IGNORE generate code for computing one row worth of data, with assumed 'int row' variable specifying row
  virtual bool          CalcRow(int row);
  // #CAT_Calc perform calculations for given row of data (calls InitCalcScript to make sure)

  /////////////////////////////////////////////////////////
  // core data processing -- see taDataProc for more elaborate option
  virtual void          Sort(const Variant& col1, bool ascending1 = true,
                             Variant col2 = -1, bool ascending2 = true,
                             Variant col3 = -1, bool ascending3 = true,
                             Variant col4 = -1, bool ascending4 = true,
                             Variant col5 = -1, bool ascending5 = true,
                             Variant col6 = -1, bool ascending6 = true);
  // #CAT_DataProc sort table according to selected columns of data.  Note: you can instantly recover the original full set of rows, unsorted and unfiltered, by using ShowAllRows on the DataTable -- see that function for more details -- to be be able to undo just this sort you would need to run Flatten first
  virtual void          SortColName(const String& col1, bool ascending1 = true,
                                    const String& col2 = "", bool ascending2 = true,
                                    const String& col3 = "", bool ascending3 = true,
                                    const String& col4 = "", bool ascending4 = true,
                                    const String& col5 = "", bool ascending5 = true,
                                    const String& col6 = "", bool ascending6 = true);
  // #EXPERT #CAT_DataProc sort table according to selected columns of data.  Note: you can instantly recover the original full set of rows, unsorted and unfiltered, by using ShowAllRows on the DataTable -- see that function for more details -- to be be able to undo just this sort you would need to run Flatten first
  virtual void          SortCol(DataCol* col1, bool ascending1 = true,
                                DataCol* col2 = NULL, bool ascending2 = true,
                                DataCol* col3 = NULL, bool ascending3 = true,
                                DataCol* col4 = NULL, bool ascending4 = true,
                                DataCol* col5 = NULL, bool ascending5 = true,
                                DataCol* col6 = NULL, bool ascending6 = true);
  // #CAT_DataProc #MENU #MENU_ON_DataProc #LABEL_Sort #FROM_GROUP_data #NULL_OK sort table according to selected columns of data. Note: you can instantly recover the original full set of rows, unsorted and unfiltered, by using ShowAllRows on the DataTable -- see that function for more details -- to be be able to undo just this sort you would need to run Flatten first

  virtual bool          RunAnalysis(DataCol* column, AnalysisRun::AnalysisType type);
  // #CAT_stats Run an analysis of specified type on the specified column

  virtual void          Filter(Variant& col1, Relation::Relations operator_1,
                               const String& value_1,
                               Relation::CombOp comb_op = Relation::AND,
                               Variant col2 = -1,
                               Relation::Relations operator_2 = Relation::EQUAL,
                               const String& value_2 = "",
                               Variant col3 = -1,
                               Relation::Relations operator_3 = Relation::EQUAL,
                               const String& value_3 = "");
  // #CAT_DataProc Select table rows by specifying up to 3 conditions for which rows to retain in the table (hiding the ones that do not match). Note: you can instantly recover the original full set of rows, unsorted and unfiltered, by using ShowAllRows on the DataTable -- see that function for more details -- to be be able to undo just this Filter you would need to run Flatten first
  virtual void          FilterCol(DataCol* col1, Relation::Relations operator_1,
                                  const String& value_1,
                                  Relation::CombOp comb_op = Relation::AND,
                                  DataCol* col2 = NULL,
                                  Relation::Relations operator_2 = Relation::EQUAL,
                                  const String& value_2 = "",
                                  DataCol* col3 = NULL,
                                  Relation::Relations operator_3 = Relation::EQUAL,
                                  const String& value_3 = "");
  // #CAT_DataProc #MENU #FROM_GROUP_data #LABEL_Filter Select table rows by specifying up to 3 conditions for which rows to retain in the table (hiding the ones that do not match). Note: you can instantly recover the original full set of rows, unsorted and unfiltered, by using ShowAllRows on the DataTable -- see that function for more details -- to be be able to undo just this Filter you would need to run Flatten first
  virtual bool          FilterByScript(const String& filter_expr);
  // #CAT_DataProc #MENU #FROM_GROUP_data #LABEL_Filter_Custom Select table rows by supplying a logical expression -- if it evaluates to true the row remains visible.  Refer to columns by name. Note: you can instantly recover the original full set of rows, unsorted and unfiltered, by using ShowAllRows on the DataTable -- see that function for more details -- to be be able to undo just this Filter you would need to run Flatten first
  virtual bool          FilterBySpec(DataSelectSpec* spec);
  // #CAT_DataProc filter the table rows by specifying which rows to retain in the table (hiding the ones that do not match).  Note: you can instantly recover the original full set of rows, unsorted and unfiltered, by using ShowAllRows on the DataTable -- see that function for more details -- to be be able to undo just this Filter you would need to run Flatten first
  virtual bool          GroupMeanSEM(DataTable* dest_data,
                                     DataCol* gp_col1, DataCol* gp_col2 = NULL,
                                     DataCol* gp_col3 = NULL, DataCol* gp_col4 = NULL);
  // #CAT_DataProc #MENU #NULL_OK #NULL_TEXT_0_NewDataTable #FROM_GROUP_1_data #FROM_GROUP_2_data #FROM_GROUP_3_data #FROM_GROUP_4_data groups data according to given columns in hierarchical fashion (gp_col2 is subgrouped within gp_col1, etc), and compute the Mean and Standard Error of the Mean (SEM) for any other numerical columns of data -- results go in dest_data table (new table created if NULL)
  virtual String        ColStats(const Variant& col);
  // #CAT_DataProc compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).-- column can be specified as either integer index or a string that is then used to find the given column name
  virtual String        ColStatsCol(DataCol* col);
  // #CAT_DataProc #MENU #FROM_GROUP_data #LABEL_ColStats #USE_RVAL compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).
  virtual String        ColStatsName(const String& col_name);
  // #EXPERT #CAT_DataProc compute standard descriptive statistics on given data table column, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).
  virtual void          PermuteRows();
  // #CAT_DataProc #MENU permute the order of rows in the data table -- randomly shuffles the rows -- this is very efficiently implemented by shuffling the row_indexes lookup table, not the actual rows in memory.  Note: you can instantly recover the original full set of rows, unsorted and unfiltered, by using ShowAllRows on the DataTable -- see that function for more details -- to be be able to undo just this Permute you would need to run Flatten first

  virtual bool          MatrixColToScalarsCol(DataCol* mtx_col,
                                              const String& scalar_col_name_stub="");
  // #EXPERT #CAT_Columns #MENU #MENU_ON_Columns #MENU_SEP_BEFORE #FROM_GROUP_data #LABEL_MatrixColToScalars convert a matrix column to a sequence of (new) scalar columns (existing cols are used too) -- if scalar_col_name_stub is non-empty, it will be used as the basis for the column names, which are sequentially numbered by cell index: stub_0 stub_1... -- otherwise, the original column name will be used with these index suffixes
  virtual bool          MatrixColToScalars(const Variant& mtx_col,
                                           const String& scalar_col_name_stub="");
  // #CAT_Columns convert a matrix column to a sequence of (new) scalar columns (existing cols are used too) -- if scalar_col_name_stub is non-empty, it will be used as the basis for the column names, which are sequentially numbered by cell index: stub_0 stub_1... -- otherwise, the original column name will be used with these index suffixes

  virtual bool          MatrixColFmScalarsCol(DataCol* mtx_col,
                                              const String& scalar_col_name_stub="");
  // #EXPERT #CAT_Columns #MENU #FROM_GROUP_data #LABEL_MatrixColFmScalars convert a sequence of scalar columns to a matrix column -- if scalar_col_name_stub is non-empty, it will be used as the basis for the column names, which are sequentially numbered by cell index: stub_0 stub_1... -- otherwise, all non-matrix fields with same value type as the matrix column will be used -- matrix column must already exist and be configured properly
  virtual bool          MatrixColFmScalars(const Variant& mtx_col, const String& scalar_col_name_stub="");
  // #CAT_Columns convert a sequence of scalar columns to a matrix column -- if scalar_col_name_stub is non-empty, it will be used as the basis for the column names, which are sequentially numbered by cell index: stub_0 stub_1... -- otherwise, all non-matrix numeric fields with same value type as the matrix column will be used -- matrix column must already exist and be configured properly
  virtual bool          SplitStringToColsCol(DataCol* string_col,
                                          const String& delim = "_",
                                          const String& col_name_stub="");
  // #EXPERT #CAT_Columns #MENU #FROM_GROUP_data #LABEL_SplitStringToCols split a string column to a sequence of separate string columns, by splitting the string by given delimiter -- this can be very useful for unpacking various conditions built into a single string value, so that they can be separately Group'ed upon (see taDataProc::Group function).  The first row's string value determines how many columns are created.  If col_name_stub is non-empty, it will be used as the basis for the column names, which are sequentially numbered by cell index: stub_0 stub_1... -- otherwise, the original column name will be used with these index suffixes
  virtual bool          SplitStringToCols(const Variant& string_col,
                                          const String& delim = "_",
                                          const String& col_name_stub="");
  // #CAT_Columns split a string column to a sequence of separate string columns, by splitting the string by given delimiter -- this can be very useful for unpacking various conditions built into a single string value, so that they can be separately Group'ed upon (see taDataProc::Group function).  The first row's string value determines how many columns are created.  If col_name_stub is non-empty, it will be used as the basis for the column names, which are sequentially numbered by cell index: stub_0 stub_1... -- otherwise, the original column name will be used with these index suffixes

  /////////////////////////////////////////////////////////
  // misc funs

  virtual GridTableView* NewGridView(T3Panel* fr = NULL);
  // #NULL_OK_0 #NULL_TEXT_0_NewFrame #MENU_BUTTON #MENU #MENU_ON_View #CAT_Display open a grid view (graphical rows and columns) of this table (NULL=use existing empty frame if any, else make new frame)
  virtual GraphTableView* NewGraphView(T3Panel* fr = NULL);
  // #NULL_OK_0 #NULL_TEXT_0_NewFrame #MENU_BUTTON #MENU #MENU_ON_View #CAT_Display open a graph view of this table (NULL=use existing empty frame if any, else make new frame)

  virtual GridTableView* FindMakeGridView(T3Panel* fr = NULL);
  // #CAT_Display find existing or make a new grid view (graphical rows and columns) of this table (NULL=use existing empty frame if any, else make new frame)
  virtual GraphTableView* FindMakeGraphView(T3Panel* fr = NULL);
  // #CAT_Display find existing or make a new graph view of this table (NULL=use existing empty frame if any, else make new frame)

  virtual GridTableView* FindGridView();
  // #CAT_Display find first existing grid view (graphical rows and columns) of this table -- NULL if none
  virtual GraphTableView* FindGraphView();
  // #CAT_Display find first existing graph view of this table -- NULL if none

  virtual bool GridViewGotoRow(int row_no);
  // #MENU_BUTTON #MENU #MENU_SEP_BEFORE #MENU_ON_View #CAT_Display move grid view to display starting at given row number -- operates on first grid view found -- use GridTableView::ViewRow_At on specific view if needed
  virtual bool GraphViewGotoRow(int row_no);
  // #MENU_BUTTON #MENU #MENU_ON_View #CAT_Display move graph view to display starting at given row number -- operates on first graph view found -- use GraphTableView::ViewRow_At on specific view if needed
  virtual void  ScrollEditorsToBottom();
  // #IGNORE scroll all data table editors to the bottom -- called when new blank row added

  virtual void  Copy_NoData(const DataTable& cp);
  // #CAT_Copy copy only the column structure, but no data, from other data table
  virtual void  Copy_DataOnly(const DataTable& cp);
  // #CAT_Copy copy only the data from other data table
  virtual void  CopyFromRow(int dest_row, const DataTable& cp, int src_row);
  // #CAT_Copy copy one row from source to given row in this object: source must have exact same column structure as this!!
  virtual bool  CopyCell(const Variant& dest_col, int dest_row, const DataTable& src, const Variant& src_col, int src_row);
  // #CAT_Copy copy one cell (indexed by column, row) from source to this data table in given col,row cell -- is robust to differences in type and matrix sizing (returns false if not successful) -- column can be specified as either integer index or a string that is then used to find the given column name
  virtual bool  CopyColRow(int dest_col, int dest_row, const DataTable& src, int src_col, int src_row) { return CopyCell(dest_col, dest_row, src, src_col, src_row); }
  // #EXPERT #CAT_Obsolete this is an obsolete name for CopyCell -- use CopyCell instead
  bool          CopyCellName(const String& dest_col_name, int dest_row, const DataTable& src, const String& src_col_name, int src_row);
  // #EXPERT #CAT_Copy copy one cell (indexed by column, row) from source to this data table in given col,row cell -- is robust to differences in type and matrix sizing (returns false if not successful)

  virtual void  UniqueColNames();
  // #CAT_ObjectMgmt ensure that the column names are all unique (adds _n for repeats)

  virtual void  DMem_ShareRows(MPI_Comm comm, int n_rows = 1);
  // #CAT_DMem #IGNORE Share the given number of rows from the end of the table (-1 = all rows) across processors in given communicator -- everyone gets the data from all processors as new rows in the table

  virtual bool          AutoLoadData();
  // #IGNORE perform auto loading of data from file when data table is loaded (called by PostLoadAutos) -- true if loaded
  bool                  AutoSaveData();
  // #IGNORE perform auto saving of data to file when project is saved
  override void         Dump_Load_post();

  override int          Dump_Load_Value(std::istream& strm, taBase* par);
  override void         Dump_Save_pre();
  override String       GetTypeDecoKey() const { return "DataTable"; }
  override int          GetSpecialState() const;
  override taBase*      ChildDuplicate(const taBase* chld);
  override taBase*      ChooseNew(taBase* origin);

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(DataTable); //

protected:
  override void UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);

  /////////////////////////////////////////////////////////
  // IMPL

  void                  RowsAdding(int n, bool begin);
  // indicate beginning and end of row adding -- you have to pass the same n each time; NOT nestable
  bool                  NewColValid(const String& col_nm,
                                    const MatrixGeom* cell_geom = NULL);
  // returns true if valid new col spec; posts modal err dialog if in gui call; geom NULL if scalar col
  int                   LoadDataFixed_impl(std::istream& strm, FixedWidthSpec* fws);

public:
  /////////////////////////////////////////////////////////
  // DataBlock interface and common routines: see ta_data.h for details

  override DBOptions    dbOptions() const
  { return (DBOptions)(DB_IND_SEQ_SRC_SNK | DB_SINK_DYNAMIC); }
  override int          ItemCount() const { return rows; }

protected:
  /////////////////////////////////////////////////////////
  // DataBlock implementation
  inline int            ChannelCount() const {return data.size; }
  inline const String   ChannelName(int chan) const
  { DataCol* da = data.SafeEl(chan);
    if (da) return da->name; else return _nilString; }

public:
  /////////////////////////////////////////////////////////
  // DataSource interface

  override int          SourceChannelCount() const { return ChannelCount(); }
  override const String SourceChannelName(int chan) const { return ChannelName(chan); }
  override void         ResetData();
  // #CAT_Rows deletes all the data (rows), but keeps the column structure -- this cannot be undone!

protected:
  /////////////////////////////////////////////////////////
  // DataSource implementation
  override const Variant GetData_impl(int chan)
  { return GetValAsVar(chan, rd_itr);}
  override taMatrix*    GetMatrixData_impl(int chan);

public:
  /////////////////////////////////////////////////////////
  // DataSink interface
  override int          SinkChannelCount() const {return ChannelCount();}
  override const String SinkChannelName(int chan) const {return ChannelName(chan);}
  override bool         AddSinkChannel(ChannelSpec* cs);
  override bool         AssertSinkChannel(ChannelSpec* cs);

protected:
  /////////////////////////////////////////////////////////
  // DataSink implementation
  override bool         AddItem_impl(int n) {return AddRows(n);}
  override void         DeleteSinkChannel_impl(int chan) {RemoveCol(chan);}
  override taMatrix*    GetSinkMatrix_impl(int chan)
  { return GetValAsMatrix(chan, wr_itr);} //note: DS refs it
  override bool         SetData_impl(const Variant& data, int chan)
  { return SetValAsVar(data, chan, wr_itr);}
  override bool         SetMatrixData_impl(const taMatrix* data, int chan)
  { return SetValAsMatrix(data, chan, wr_itr);}
  override void         WriteClose_impl();

protected:
  DataCol*      NewCol_impl(DataCol::ValType val_type, const String& col_nm, int& col_idx);
  // low-level create routine, shared by scalar and matrix creation, must be wrapped in StructUpdate
  DataCol*      GetColForChannelSpec_impl(ChannelSpec* cs);
  DataCol*      NewColFromChannelSpec_impl(ChannelSpec* cs);

private:
  void  Copy_(const DataTable& cp);
  void  Initialize();
  void  Destroy();
};

TA_SMART_PTRS(DataTable);

#endif // DataTable_h
