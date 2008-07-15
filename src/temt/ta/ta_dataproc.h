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

#ifndef ta_dataproc_h
#define ta_dataproc_h

#include "ta_datatable.h"
#include "ta_math.h"
#include "ta_program.h"

// data processing operations on datatables

class TA_API DataOpEl : public taOBase {
  // #STEM_BASE ##NO_TOKENS ##NO_UPDATE_AFTER ##INLINE ##CAT_Data base class for data operations spec element
  INHERITED(taOBase)
public:
  DataTableRef		data_table;
  // #READ_ONLY #HIDDEN #NO_SAVE data table -- gets set dynamically
  DataTableCols*	data_cols;
  // #READ_ONLY #HIDDEN #NO_SAVE data table columns -- gets set dynamically -- just to lookup column
  DataCol*		col_lookup;
  // #APPLY_IMMED #NULL_OK #NO_SAVE #FROM_GROUP_data_cols #NO_EDIT #NO_UPDATE_POINTER lookup column in data table to sort on -- sets col_name field (which is what is actually used) and returns to NULL after selection is applied
  String		col_name;	// name of column in data table to sort on (either enter directly or lookup from col_lookup)
  int			col_idx;	// #READ_ONLY #NO_SAVE column idx (from GetColumns)

  virtual void 	SetDataTable(DataTable* dt);
  // #CAT_DataOp set the data table to enable looking up columns

  virtual void 	GetColumns(DataTable* dt);
  // #CAT_DataOp get the column pointers for given data table (looking up by name)
  virtual void 	ClearColumns();
  // #CAT_DataOp clear column pointers (don't keep these guys hanging around)

  override String GetName() const;
  override String GetDisplayName() const;
  override String GetTypeDecoKey() const { return "ProgArg"; }
  TA_SIMPLE_BASEFUNS(DataOpEl);
protected:
  override void	UpdateAfterEdit_impl();	// set col_name from col_lookup
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void  Initialize();
  void 	Destroy()		{ };
};

class TA_API DataOpList : public taList<DataOpEl> {
  // ##CAT_Data a list of data table operations
INHERITED(taList<DataOpEl>)
public:

  virtual void 	SetDataTable(DataTable* dt);
  // #CAT_DataOp set the data table to enable looking up columns

  virtual void 	GetColumns(DataTable* dt);
  // #CAT_DataOp get the column pointers for given data table (looking up by name)
  virtual void 	ClearColumns();
  // #CAT_DataOp clear column pointers (don't keep these guys hanging around)

  virtual DataOpEl* AddColumn(const String& col_name, DataTable* dt);
  // #CAT_DataOp #BUTTON add a new column to operate on from given data table
  virtual void	AddAllColumns(DataTable* dt);
  // #CAT_DataOp add all columns from given data table
  virtual void	AddAllColumns_gui(DataTable* dt);
  // #CAT_DataOp #BUTTON #LABEL_AddAllColumns add all columns from given data table

  void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);

  override String 	GetTypeDecoKey() const { return "ProgArg"; }

  TA_BASEFUNS_NOCOPY(DataOpList);
private:
  void	Initialize() 		{ SetBaseType(&TA_DataOpEl); }
  void 	Destroy()		{ };
};

class TA_API DataOpBaseSpec : public taNBase {
  // #STEM_BASE ##CAT_Data ##DEF_CHILD_ops ##DEF_CHILDNAME_Operators a datatable operation specification -- contains a list of operation elements associated with different data columns
INHERITED(taNBase)
public:
  DataOpList	ops;		// #SHOW_TREE the list of operation elements, associated with different data columns

  virtual void 	SetDataTable(DataTable* dt) { ops.SetDataTable(dt); }
  // #CAT_DataOp set the data table to enable looking up columns

  virtual void 	GetColumns(DataTable* dt) { ops.GetColumns(dt); }
  // #CAT_DataOp get the column pointers for given data table (looking up by name)
  virtual void 	ClearColumns() { ops.ClearColumns(); }
  // #CAT_DataOp clear column pointers (don't keep these guys hanging around)

  virtual DataOpEl* AddColumn(const String& col_name, DataTable* dt)
  { return ops.AddColumn(col_name, dt); }
  // #CAT_DataOp #BUTTON add a new column to operate on from given data table
  virtual void	AddAllColumns(DataTable* dt) { ops.AddAllColumns_gui(dt); }
  // #CAT_DataOp #BUTTON add all columns from given data table

  override taList_impl*	children_() {return &ops;}	
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return ops.GetTA_Element(i, eltd); }
  override String 	GetTypeDecoKey() const { return "DataTable"; }
  TA_SIMPLE_BASEFUNS(DataOpBaseSpec);
protected:
  override void	CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void 	Destroy()		{ };
};


/////////////////////////////////////////////////////////
//   Sorting Spec
/////////////////////////////////////////////////////////

class TA_API DataSortEl : public DataOpEl {
  // one element of a data sorting specification
  INHERITED(DataOpEl)
public:
  enum SortOrder {
    ASCENDING,
    DESCENDING,
  };

  SortOrder		order;		// order to sort this in

  override String GetDisplayName() const;
  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataSortEl);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
};

class TA_API DataSortSpec : public DataOpBaseSpec {
  // a datatable sort specification (list of sort elements)
INHERITED(DataOpBaseSpec)
public:

  TA_BASEFUNS(DataSortSpec);
private:
 void	Initialize();
  void 	Destroy()		{ };
};

/////////////////////////////////////////////////////////
//   Grouping Spec
/////////////////////////////////////////////////////////

class TA_API DataGroupEl : public DataOpEl {
  // one element of a data grouping specification
  INHERITED(DataOpEl)
public:
  Aggregate	agg;		// how to aggregate this information

  override String GetDisplayName() const;
  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataGroupEl);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
};

class TA_API DataGroupSpec : public DataOpBaseSpec {
  // #CAT_Data a datatable grouping specification (list of group elements)
INHERITED(DataOpBaseSpec)
public:
  bool	append_agg_name;	// if true, append aggregation operator name (e.g., group, mean, last, etc) to column names of destination data table (otherwise, just keep same names as source)

  TA_SIMPLE_BASEFUNS(DataGroupSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

/////////////////////////////////////////////////////////
//   Selecting Spec
/////////////////////////////////////////////////////////

class TA_API DataSelectEl : public DataOpEl {
  // one element of a data selection specification
  INHERITED(DataOpEl)
public:
  enum Relations {
    EQUAL,		// #LABEL_=
    NOTEQUAL,		// #LABEL_!=
    LESSTHAN,		// #LABEL_<
    GREATERTHAN,	// #LABEL_>
    LESSTHANOREQUAL,	// #LABEL_<=
    GREATERTHANOREQUAL, // #LABEL_>=
    CONTAINS,		// for strings: contains this value
    NOT_CONTAINS,	// for strings: doesn't contain this value
  };

  Relations	rel;		// relation of column to expression for selection
  bool		use_var;	// #APPLY_IMMED if true, use a program variable to specify the selection value
  Variant	cmp;		// #CONDEDIT_ON_use_var:false literal compare value of column to this comparison value
  ProgVarRef	var;		// #CONDEDIT_ON_use_var:true variable that contains the comparison value: note -- this MUST be a global var in vars or args, not in local vars!

  bool	Eval(const Variant& val); // evaluate expression

  override String GetDisplayName() const;
  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataSelectEl);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
};

class TA_API DataSelectSpec : public DataOpBaseSpec {
  // #CAT_Data a datatable select specification (list of select elements)
  INHERITED(DataOpBaseSpec)
public:
  enum CombOp {
    AND,			// include only if all of the columns are true
    OR,				// include if any one (or more) of the columns are true
    NOT_AND,			// include only if all of the columns are false
    NOT_OR,			// include if any of (or more) the columns are false
  };

  CombOp	comb_op;	// how to combine individual expressions for each column

  override String GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(DataSelectSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

/////////////////////////////////////////////////////////
//   Join Spec
/////////////////////////////////////////////////////////

class TA_API DataJoinSpec : public taNBase {
  // #STEM_BASE ##INLINE ##CAT_Data datatable join specification: combine two tables along matching column values -- tables are both sorted (internally) by join col, so results are in that sort order
  INHERITED(taNBase)
public:
  enum JoinType {
    LEFT,			// each row of the first table is included, with blanks for nonmatches
    INNER,			// only matching rows from both tables are included
  };

  DataOpEl	col_a;		// column from first (a) source datatable to join on (values match those in col_b)
  DataOpEl	col_b;		// column from second (b) source datatable to join on (values match those in col_a)
  JoinType	type;		// type of join to perfrom (determines what to do with nonmatches -- matches are always included)
  bool		nomatch_warn;	// #CONDEDIT_ON_type:INNER for INNER join, issue a warning for row values in A that do not have a matching value in B

  virtual void 	SetDataTable(DataTable* dt_a, DataTable* dt_b);
  // set the data table to enable looking up columns

  virtual void 	GetColumns(DataTable* dt_a, DataTable* dt_b);
  // get the column pointers for given data table (looking up by name)
  virtual void 	ClearColumns();
  // clear column pointers (don't keep these guys hanging around)

  override String GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(DataJoinSpec);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void  Initialize();
  void 	Destroy()		{ };
};


/////////////////////////////////////////////////////////
//   data operations
/////////////////////////////////////////////////////////

class TA_API taDataProc : public taNBase {
  // #STEM_BASE ##CAT_Data collection of commonly-used datatable processing operations (database-style)
INHERITED(taNBase)
public:
  
  static bool	GetDest(DataTable*& dest, const DataTable* src, const String& suffix);
  // #IGNORE helper function: if dest is NULL, a new one is created in proj.data.AnalysisData, with name from source + suffix

  ///////////////////////////////////////////////////////////////////
  // manipulating lists of columns

  static bool	GetCommonCols(DataTable* dest, DataTable* src, DataOpList* dest_cols, DataOpList* src_cols);
  // #CAT_ColumnLists find common columns between dest and src by name, cell_size, and type if matrix 
  static bool	GetColIntersection(DataOpList* trg_cols, DataOpList* ref_cols);
  // #CAT_ColumnLists get the intersection (common columns) between two lists of columns: trg_cols and ref_cols (based only on name) -- i.e., remove any columns in trg_cols that are not in the ref_cols list

  ///////////////////////////////////////////////////////////////////
  // basic copying and concatenating

  static bool	CopyData(DataTable* dest, DataTable* src);
  // #CAT_Copy #MENU_BUTTON #MENU_ON_Copy #NULL_OK_0 #NULL_TEXT_0_NewDataTable just copy the data from source to destination, completely removing any existing data in the destination, but not changing anything else about the dest (e.g., its name) (if dest is NULL, a new one is created in proj.data.AnalysisData)
  static bool	CopyCommonColsRow_impl(DataTable* dest, DataTable* src, DataOpList* dest_cols, DataOpList* src_cols, int dest_row, int src_row);
  // #CAT_Copy copy one row of data from src to dest for the lists of common columns generated by GetCommonCols
  static bool	CopyCommonColsRow(DataTable* dest, DataTable* src, int dest_row, int src_row);
  // #CAT_Copy copy one row of data from src to dest for the cols that are in common between the two tables (by name)
  static bool	CopyCommonColData(DataTable* dest, DataTable* src);
  // #CAT_Copy #MENU_BUTTON copy data from src to dest for all columns that are common between the two (adds to end of dest rows)
  static bool	AppendRows(DataTable* dest, DataTable* src);
  // #CAT_Copy #MENU_BUTTON append rows of src to the end of dest (structure must be the same -- more efficient than CopyCommonColData when this is true)
  static bool	ReplicateRows(DataTable* dest, DataTable* src, int n_repl);
  // #CAT_Copy #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable replicate each row of src n_repl times in dest -- dest is completely overwritten (if dest is NULL, a new one is created in proj.data.AnalysisData)
  static bool	ConcatRows(DataTable* dest, DataTable* src_a, DataTable* src_b, DataTable* src_c=NULL,
			   DataTable* src_d=NULL, DataTable* src_e=NULL, DataTable* src_f=NULL);
  // #CAT_Copy #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable concatenate rows of data from all the source data tables into the destination, which is completely overwritten with the new data.  (if dest is NULL, a new one is created in proj.data.AnalysisData).  just a sequence of calls to CopyCommonColData

  ///////////////////////////////////////////////////////////////////
  // reordering functions

  static bool	Sort(DataTable* dest, DataTable* src, DataSortSpec* spec);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Order #MENU_BUTTON #MENU_ON_Order sort data from src into dest according to sorting specifications in spec; dest is completely overwritten (if dest is NULL, a new one is created in proj.data.AnalysisData)
  static bool 	SortInPlace(DataTable* dt, DataSortSpec* spec);
  // #CAT_Order #MENU_BUTTON #MENU_ON_Order sort given data table in place (modifies data table) according to sorting specifications in spec

  static int 	Sort_Compare(DataTable* dt_a, int row_a, DataTable* dt_b, int row_b,
			     DataSortSpec* spec);
  // #IGNORE helper function for sorting: compare values -1 = a is < b; 1 = a > b; 0 = a == b
  static bool 	Sort_impl(DataTable* dt, DataSortSpec* spec);
  // #IGNORE actually perform sort on data table using specs

  static bool	Permute(DataTable* dest, DataTable* src);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Order #MENU_BUTTON permute (randomly reorder) the rows of the data table -- note that it is typically much more efficient to just use a permuted index to access the data rather than physically permuting the items

  static bool	Group(DataTable* dest, DataTable* src, DataGroupSpec* spec);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Order #MENU_BUTTON group data from src into dest according to grouping specifications in spec (if dest is NULL, a new one is created in proj.data.AnalysisData)

  static bool	Group_nogp(DataTable* dest, DataTable* src, DataGroupSpec* spec);
  // #IGNORE helper function to do grouping when there are no GROUP items
  static bool	Group_gp(DataTable* dest, DataTable* src, DataGroupSpec* spec,
			 DataSortSpec* sort_spec);
  // #IGNORE helper function to do grouping when there are GROUP items, as spec'd in sort_spec

  ///////////////////////////////////////////////////////////////////
  // row-wise functions: selecting/splitting

  static bool	SelectRows(DataTable* dest, DataTable* src, DataSelectSpec* spec);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Select #MENU_BUTTON #MENU_ON_Select select rows of data from src into dest according to selection specifications in spec (all columns are copied) (if dest is NULL, a new one is created in proj.data.AnalysisData)

  static bool	SplitRows(DataTable* dest_a, DataTable* dest_b, DataTable* src,
			  DataSelectSpec* spec);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Select #MENU_BUTTON splits the source datatable rows into two sets, those that match the selection specifications go into dest_a, else dest_b (if dest are NULL, new ones are created in proj.data.AnalysisData)

  static bool	SplitRowsN(DataTable* src, DataTable* dest_1, int n1, DataTable* dest_2, int n2=-1,
			 DataTable* dest_3=NULL, int n3=0, DataTable* dest_4=NULL, int n4=0,
			 DataTable* dest_5=NULL, int n5=0, DataTable* dest_6=NULL, int n6=0);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Select #MENU_BUTTON splits the source datatable rows into distinct non-overlapping sets, with specific number of elements (sequentially) in each (-1 = the remainder, can appear *only once* anywhere) (new dest datatables are created if NULL)
  static bool	SplitRowsNPermuted(DataTable* src, DataTable* dest_1, int n1, DataTable* dest_2, int n2=-1,
				   DataTable* dest_3=NULL, int n3=0, DataTable* dest_4=NULL, int n4=0,
				   DataTable* dest_5=NULL, int n5=0, DataTable* dest_6=NULL, int n6=0);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Select #MENU_BUTTON splits the source datatable rows into distinct non-overlapping sets, with specific number of elements (order permuted efficiently via an index list) in each (-1 = the remainder, can appear *only once* anywhere) (new dest datatables are created if NULL).  this is good for creating random training/testing subsets

  ///////////////////////////////////////////////////////////////////
  // column-wise functions: selecting, joining

  static bool	SelectCols(DataTable* dest, DataTable* src, DataOpList* spec);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Columns #MENU_BUTTON #MENU_ON_Columns select columns of data from src into dest according to list of columnns in spec (all rows are copied)

  static bool	Join(DataTable* dest, DataTable* src_a, DataTable* src_b, DataJoinSpec* spec);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Columns #MENU_BUTTON joins two datatables (src_a and src_b) into dest datatable.  tables are internally sorted first according to the join column.  all matching row values from both tables are included in the result.  for the left join, all rows of src_a are included even if src_b does not match, and vice-versa for the right join.  inner only includes the matches.  all columns are included (without repeating the common column)

  static bool	ConcatCols(DataTable* dest, DataTable* src_a, DataTable* src_b);
  // #NULL_OK_0 #NULL_TEXT_0_NewDataTable #CAT_Columns #MENU_BUTTON concatenate two datatables into one datatable by adding both sets of columns together, merging data on a row-by-row basis (number of rows = MIN(src_a->rows, src_b_rows)).

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  TA_BASEFUNS(taDataProc);
private:
  NOCOPY(taDataProc)
  void Initialize() { };
  void Destroy() { };
};

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

class TA_API DataProcCall : public StaticMethodCall { 
  // call a taDataProc (data processing/database) function
INHERITED(StaticMethodCall)
public:
  TA_BASEFUNS_NOCOPY(DataProcCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

/////////////////////////////////////////////////////////////////////////
//		Data One Prog -- Basic Data operations (loop, var, etc)

class TA_API DataOneProg : public ProgEl { 
  // #VIRT_BASE #NO_INSTANCE a program element for operations on one data table (virtual base class -- do not use)
INHERITED(ProgEl)
public:
  ProgVarRef	    data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to data table for operation

  virtual DataTable* GetData() const;
  // get actual data table pointer from variable

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  PROGEL_SIMPLE_BASEFUNS(DataOneProg);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataLoop: public Loop { 
  // For any kind of data table: loops over items in a DataTable, in different basic orderings, using index to select current data table item using ReadItem(index) call, so that later processes will access this row of data
INHERITED(Loop)
public:
  enum Order {
    SEQUENTIAL,			// present events in sequential order
    PERMUTED,			// permute the order of event presentation
    RANDOM 			// pick an event at random (with replacement)
  };

  ProgVarRef	data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to the data table to use
  ProgVarRef	index_var;	// #ITEM_FILTER_StdProgVarFilter program variable for the index used in the loop -- goes from 0 to number of rows in data table-1
  ProgVarRef	order_var;	// #ITEM_FILTER_StdProgVarFilter variable that contains the order to process data items (rows) in -- is automatically created if not set
  Order		order;		// #READ_ONLY #SHOW order to process data items (rows) in -- set from order_var
  int_Array	item_idx_list;	// #READ_ONLY list of item indicies (permuted if permuted, otherwise in sequential order)

  virtual DataBlock* GetData();
  // get actual data table pointer from variable
  virtual void	GetOrderVal();
  // get order value from order_var variable

  override String	GetDisplayName() const;

  PROGEL_SIMPLE_BASEFUNS(DataLoop);
protected:
  virtual void	GetOrderVar(); // make an order variable in program if not already set
  virtual void	GetIndexVar(); // make an index variable in program if not already set
  override void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
};

class TA_API DataVarProg : public DataOneProg { 
  // A program element for exchanging information between program variables and data table values in columns with the same names as the variables -- scalar only variables
INHERITED(DataOneProg)
public:
  enum RowType {
    CUR_ROW,			// use the current row (i.e., the last one added or specifically set by Read or Write operation)
    ROW_NUM,			// row_var variable contains the row number to operate on
    ROW_VAL,			// row_var variable contains a value that is used to find the row number by searching within data table column with the same name as the row_var variable
  };

  bool		set_data;	// if true, values in data table are set according to current variable values, otherwise, it gets data from the data table into the variables
  RowType	row_spec;	// #APPLY_IMMED how the row number within data table is specified
  ProgVarRef	row_var;	// #CONDEDIT_OFF_row_spec:CUR_ROW #ITEM_FILTER_StdProgVarFilter program variable containing information about which row to operate on (depends on row_spec for what this information is)

  ProgVarRef	var_1;		// #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!
  ProgVarRef	var_2;		// #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!
  ProgVarRef	var_3;		// #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!
  ProgVarRef	var_4;		// #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!

  override String	GetDisplayName() const;

  PROGEL_SIMPLE_BASEFUNS(DataVarProg);
protected:
  override void	CheckThisConfig_impl(bool quiet, bool& rval);

  override const String	GenCssBody_impl(int indent_level);
  virtual bool	GenCss_OneVar(String& rval, ProgVarRef& var, const String& idnm,
			      const String& il, int var_no);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataVarProgMatrix : public DataVarProg { 
  // A program element for exchanging information between program variables and data table values in columns with the same names as the variables -- for matrix variables up to 4 cells (var_1 is first cell, var_2 is second cell..), name of column is var name before last '_'
INHERITED(DataVarProg)
public:
  TA_BASEFUNS_NOCOPY(DataVarProgMatrix);
protected:
  override bool	GenCss_OneVar(String& rval, ProgVarRef& var, const String& idnm,
			      const String& il, int var_no);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API ResetDataRows: public DataOneProg { 
  // clear out all existing rows of data in data table
INHERITED(DataOneProg)
public:
  override String	GetDisplayName() const;
  TA_BASEFUNS_NOCOPY(ResetDataRows);
protected:
  override const String	GenCssBody_impl(int indent_level);
private:
  void	Initialize();
  void	Destroy()	{ }
}; 

class TA_API AddNewDataRow: public DataOneProg { 
  // add a new row to data table (just calls AddBlankRow() on data table var object)
INHERITED(DataOneProg)
public:
  override String	GetDisplayName() const;
  TA_BASEFUNS_NOCOPY(AddNewDataRow);
protected:
  override const String	GenCssBody_impl(int indent_level);
private:
  void	Initialize();
  void	Destroy()	{ }
}; 

class TA_API DoneWritingDataRow: public DataOneProg { 
  // add this after you are done writing everything to the current row of the data table, and it will update displays and write to log files, etc (just calls WriteClose() on data table var object)
INHERITED(DataOneProg)
public:
  override String	GetDisplayName() const;
  TA_BASEFUNS_NOCOPY(DoneWritingDataRow);
protected:
  override const String	GenCssBody_impl(int indent_level);
private:
  void	Initialize();
  void	Destroy()	{ }
}; 


/////////////////////////////////////////////////////////////////////////
//	Data Proc Programs -- Src Dest Tables for major proc ops

class TA_API DataSrcDestProg : public ProgEl { 
  // #VIRT_BASE #NO_INSTANCE a program element for data operations involving a source and destination (virtual base class -- do not use)
INHERITED(ProgEl)
public:
  ProgVarRef	    src_data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to source data for operation
  ProgVarRef	    dest_data_var;	// #NULL_OK #ITEM_FILTER_DataProgVarFilter program variable pointing to destination (result) data for operation (if NULL, a new one will be automatically created)

  virtual DataTable* GetSrcData(); // get source data table pointer from src_data_var (or NULL)
  virtual DataTable* GetDestData(); // get dsource data table pointer from dest_data_var (or NULL)

  virtual void	UpdateSpecDataTable() { };
  // #CAT_Data update the data table pointer(s) for the spec in this prog (so the user can choose columns from the appropriate data table)

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  PROGEL_SIMPLE_BASEFUNS(DataSrcDestProg);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataSortProg : public DataSrcDestProg { 
  // sorts src_data into dest_data according to sort_spec
INHERITED(DataSrcDestProg)
public:
  DataSortSpec		sort_spec; // #SHOW_TREE data sorting specification

  virtual DataOpEl* AddColumn(const String& col_name) { return sort_spec.AddColumn(col_name, GetSrcData()); }
  // #CAT_Data #BUTTON add a new column to operate on
  virtual void	AddAllColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the sort_spec list of ops columns 

  override void	UpdateSpecDataTable();

  override String GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(DataSortProg);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataGroupProg : public DataSrcDestProg { 
  // groups src_data into dest_data according to group_spec
INHERITED(DataSrcDestProg)
public:
  DataGroupSpec		group_spec; // #SHOW_TREE data grouping specification

  virtual DataOpEl* AddColumn(const String& col_name) { return group_spec.AddColumn(col_name, GetSrcData()); }
  // #CAT_Data #BUTTON add a new column to operate on
  virtual void	AddAllColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the group_spec list of ops columns 
  override void	UpdateSpecDataTable();

  override String GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(DataGroupProg);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataSelectRowsProg : public DataSrcDestProg { 
  // selects rows from src_data into dest_data according to select_spec
INHERITED(DataSrcDestProg)
public:
  DataSelectSpec	select_spec; // #SHOW_TREE data selection specification

  virtual DataOpEl* AddColumn(const String& col_name) { return select_spec.AddColumn(col_name, GetSrcData()); }
  // #CAT_Data #BUTTON add a new column to operate on
  virtual void	AddAllColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the select_spec list of ops columns 
  override void	UpdateSpecDataTable();

  override String GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(DataSelectRowsProg);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataSelectColsProg : public DataSrcDestProg { 
  // ##DEF_CHILD_select_spec ##DEF_CHILDNAME_Select_Spec selects rows from src_data into dest_data according to select_spec
INHERITED(DataSrcDestProg)
public:
  DataOpList		select_spec; // #SHOW_TREE columns to select

  virtual DataOpEl* AddColumn(const String& col_name) { return select_spec.AddColumn(col_name, GetSrcData()); }
  // #CAT_Data #BUTTON add a new column to operate on
  virtual void	AddAllColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the select_spec list of ops columns 
  override void	UpdateSpecDataTable();

  override taList_impl*	children_() {return &select_spec; }	
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return select_spec.GetTA_Element(i, eltd); }
  override String GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(DataSelectColsProg);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataJoinProg : public DataSrcDestProg { 
  // joins two datatables (src and src_b) into dest datatable indexed by a common column
INHERITED(DataSrcDestProg)
public:
  ProgVarRef		src_b_data_var;	// #ITEM_FILTER_DataProgVarFilter variable pointing to second source data for operation
  DataJoinSpec		join_spec; 	// #SHOW_TREE data grouping specification

  virtual DataTable* GetSrcBData(); // get source data table pointer from src_data_var (or NULL)

  override void	UpdateSpecDataTable();

  override String GetDisplayName() const;
  PROGEL_SIMPLE_BASEFUNS(DataJoinProg);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataCalcLoop : public DataSrcDestProg { 
  // enables arbitrary calculations and operations on data by looping row-by-row through the src_data table; can either just operate on src_data (using SetSrcRow) or generate new dest_data (using AddDestRow and SetDestRow)
INHERITED(DataSrcDestProg)
public:
  DataOpList		src_cols;
  // source columns to operate on (variables are labeled as s_xxx where xxx is col_name)
  DataOpList		dest_cols;
  // destination columns to operate on (variables are labeled as d_xxx where xxx is col_name)
  ProgEl_List		loop_code; // #SHOW_TREE the items to execute in the loop
  ProgVar_List		src_col_vars;  // #READ_ONLY source column variables
  ProgVar_List		dest_col_vars;  // #READ_ONLY dest column variables
  ProgVar		src_row_var;	// #HIDDEN #READ_ONLY #NO_SAVE variable for FindVarName rval for src_row loop variable

//no  override taList_impl*	children_() {return &loop_code;}	

  virtual DataOpEl* AddSrcColumn(const String& col_name);
  // #CAT_DataOp #BUTTON add a new source column to operate on
  virtual DataOpEl* AddDestColumn(const String& col_name);
  // #CAT_DataOp #BUTTON add a new dest column to operate on
  virtual void	AddAllSrcColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the src_cols list of columns 
  virtual void	AddAllDestColumns();
  // #BUTTON #CAT_Data add all columns from dest_data to the dest_cols list of columns 
  override void	UpdateSpecDataTable();

  virtual ProgEl*	AddLoopCode(TypeDef* el_type)	{ return (ProgEl*)loop_code.New(1, el_type); }
  // #BUTTON #TYPE_ProgEl add a new loop code element

  override ProgVar*	FindVarName(const String& var_nm) const;

  override String GetDisplayName() const;
  PROGEL_SIMPLE_BASEFUNS(DataCalcLoop);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override void	PreGenChildren_impl(int& item_id);

  virtual void	SetColProgVarFmData(ProgVar* pv, DataOpEl* ds);
  virtual void	UpdateColVars();
  // sync col vars from cols

  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 
  override const String	GenListing_children(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataCalcAddDestRow : public DataSrcDestProg { 
  // add a new blank row into the dest data table (used ONLY within a DataCalcLoop to add new data -- automatically gets dest_data from outer DataCalcLoop object)
INHERITED(DataSrcDestProg)
public:
#ifdef __MAKETA__
  ProgVarRef	    src_data_var;	// #READ_ONLY #HIDDEN #ITEM_FILTER_DataProgVarFilter source data for operation
  ProgVarRef	    dest_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter destination table to add row in -- automatically updated from DataCalcLoop
#endif

  virtual void	GetDataPtrsFmLoop();
  // get my data table ptrs from parent calc loop obj

  override String GetDisplayName() const;
  void	InitLinks();
  TA_BASEFUNS(DataCalcAddDestRow);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Copy_(const DataCalcAddDestRow& cp);
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataCalcSetDestRow : public DataSrcDestProg { 
  // set all the current values into the dest data table (used ONLY within a DataCalcLoop -- automatically gets dest_data from outer DataCalcLoop object)
INHERITED(DataSrcDestProg)
public:
#ifdef __MAKETA__
  ProgVarRef	    src_data_var;	// #READ_ONLY #HIDDEN #ITEM_FILTER_DataProgVarFilter source data for operation
  ProgVarRef	    dest_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter destination table to add row in -- automatically updated from DataCalcLoop
#endif

  virtual void	GetDataPtrsFmLoop();
  // get my data table ptrs from parent calc loop obj

  override String GetDisplayName() const;
  void 	InitLinks();
  TA_BASEFUNS(DataCalcSetDestRow);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Copy_(const DataCalcSetDestRow& cp);
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataCalcSetSrcRow : public DataSrcDestProg { 
  // set all the current values into the src data table (used ONLY within a DataCalcLoop -- automatically gets src_data from outer DataCalcLoop object)
INHERITED(DataSrcDestProg)
public:
#ifdef __MAKETA__
  ProgVarRef	    src_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter source table to set values in -- automatically updated from DataCalcLoop
  ProgVarRef	    dest_data_var;	// #READ_ONLY #HIDDEN #ITEM_FILTER_DataProgVarFilter destination table -- automatically updated from DataCalcLoop
#endif

  virtual void	GetDataPtrsFmLoop();
  // get my data table ptrs from parent calc loop obj

  override String GetDisplayName() const;
  void 	InitLinks();
  TA_BASEFUNS(DataCalcSetSrcRow);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Copy_(const DataCalcSetSrcRow& cp);
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataCalcCopyCommonCols : public DataSrcDestProg { 
  // copy all of the columns from source to dest that have the same name and type (used ONLY within a DataCalcLoop -- automatically gets src_data from outer DataCalcLoop object)
INHERITED(DataSrcDestProg)
public:
#ifdef __MAKETA__
  ProgVarRef	    src_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter source data for copying -- automatically updated from DataCalcLoop
  ProgVarRef	    dest_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter destination table for copying -- automatically updated from DataCalcLoop
#endif
  bool		only_named_cols;
  // only copy columns that are named in src_cols and dest_cols (otherwise just operates on all the datatable columns)

  virtual void	GetDataPtrsFmLoop();
  // get my data table ptrs from parent calc loop obj

  override String GetDisplayName() const;
  void 	InitLinks();
  SIMPLE_COPY_EX(DataCalcCopyCommonCols,CopyInner_);
  TA_BASEFUNS(DataCalcCopyCommonCols);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Copy_(const DataCalcCopyCommonCols& cp);
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // ta_dataproc_h
