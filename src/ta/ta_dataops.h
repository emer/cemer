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

#ifndef ta_dataops_h
#define ta_dataops_h

#include "ta_datatable.h"
#include "ta_math.h"
#include "ta_program.h"

class TA_API DataOpEl : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##INLINE ##CAT_Data base class for data operations spec element
  INHERITED(taBase)
public:
  DataTableCols*	cols;	// #READ_ONLY #NO_SAVE data table columns -- gets set dynamically
  DataArray_impl*	column;	// #NO_SAVE #FROM_GROUP_cols column in data table to sort on -- just to lookup the name, which is what is actually used
  String		col_name;	// name of column in data table to sort on (either enter directly or lookup from column)
  int			col_idx;	// #READ_ONLY #NO_SAVE column idx (from GetColumns)

  override String GetDisplayName() const;
  void	UpdateAfterEdit();	// set col_name from column
  TA_SIMPLE_BASEFUNS(DataOpEl);
protected:
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
  // set the data table to enable looking up columns

  virtual void 	GetColumns(DataTable* dt);
  // get the column pointers for given data table (looking up by name)
  virtual void 	ClearColumns();
  // clear column pointers (don't keep these guys hanging around)

  TA_BASEFUNS(DataOpList);
private:
  void	Initialize() 		{ SetBaseType(&TA_DataOpEl); }
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

  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataSortEl);
};

class TA_API DataSortSpec : public DataOpList {
  // a datatable sort specification (list of sort elements)
INHERITED(DataOpList)
public:

  TA_BASEFUNS(DataSortSpec);
private:
  void	Initialize() 		{ SetBaseType(&TA_DataSortEl); }
  void 	Destroy()		{ };
};

/////////////////////////////////////////////////////////
//   Selecting Spec
/////////////////////////////////////////////////////////

class TA_API DataSelectEl : public DataOpEl {
  // one element of a data selection specification
  INHERITED(DataOpEl)
public:
  enum Relation {
    EQUAL,		// #LABEL_=
    NOTEQUAL,		// #LABEL_!=
    LESSTHAN,		// #LABEL_<
    GREATERTHAN,	// #LABEL_>
    LESSTHANOREQUAL,	// #LABEL_<=
    GREATERTHANOREQUAL 	// #LABEL_>=
  };

  Relation		rel;		// relation of column to expression for selection
  String		expr;		// expression to compare column value to for selection

  bool	Eval(const String& val); // evaluate expression

  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataSelectEl);
};

class TA_API DataSelectSpec : public DataOpList {
  // #CAT_Data a datatable select specification (list of select elements)
  INHERITED(DataOpList)
public:
  enum CombOp {
    AND,			// include only if all of the columns are true
    OR,				// include if any one (or more) of the columns are true
    NOT_AND,			// include only if all of the columns are false
    NOT_OR,			// include if any of (or more) the columns are false
  };

  CombOp	comb_op;	// how to combine individual expressions for each column

  TA_SIMPLE_BASEFUNS(DataSelectSpec);
private:
  void	Initialize() 		{ SetBaseType(&TA_DataSelectEl); comb_op = AND; }
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

  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataGroupEl);
};

class TA_API DataGroupSpec : public DataOpList {
  // #CAT_Data a datatable grouping specification (list of group elements)
INHERITED(DataOpList)
public:
  Aggregate::Operator		default_op;
  // default operation to perform on columns not specifically listed

  TA_SIMPLE_BASEFUNS(DataGroupSpec);
private:
  void	Initialize() 		{ SetBaseType(&TA_DataGroupEl); }
  void 	Destroy()		{ };
};


/////////////////////////////////////////////////////////
//   data operations
/////////////////////////////////////////////////////////

class TA_API taDataOps : public taOBase {
  // ##CAT_Data collection of commonly-used datatable operations
public:
  
  static bool	Sort(DataTable* dest, DataTable* src, DataSortSpec* spec);
  // sort data from src into dest according to sorting specifications in spec; dest is completely overwritten

  static int 	Sort_Compare(DataTable* dt_a, int row_a, DataTable* dt_b, int row_b,
			     DataSortSpec* spec);
  // helper function for sorting: compare values -1 = a is < b; 1 = a > b; 0 = a == b
  static bool 	Sort_impl(DataTable* dt, DataSortSpec* spec);
  // #IGNORE actually perform sort on data table using specs

  static bool	SelectRows(DataTable* dest, DataTable* src, DataSelectSpec* spec);
  // select rows of data from src into dest according to selection specifications in spec (all columns are copied)
  static bool	SelectCols(DataTable* dest, DataTable* src, DataOpList* spec);
  // select columns of data from src into dest according to list of columnns in spec (all rows are copied)

  static bool	Group(DataTable* dest, DataTable* src, DataGroupSpec* spec);
  // group data from src into dest according to grouping specifications in spec

  static bool	Group_nogp(DataTable* dest, DataTable* src, DataGroupSpec* spec);
  // #IGNORE helper function to do grouping when there are no GROUP items
  static bool	Group_gp(DataTable* dest, DataTable* src, DataGroupSpec* spec,
			 DataSortSpec* sort_spec);
  // #IGNORE helper function to do grouping when there are GROUP items, as spec'd in sort_spec

  void Initialize() { };
  void Destroy() { };
  TA_BASEFUNS(taDataOps);
};

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

class TA_API DataProg : public ProgEl { 
  // #VIRT_BASE #NO_INSTANCE a program element for data operations (virtual base class -- do not use)
INHERITED(ProgEl)
public:
  DataTableRef	    src_data;	// source data for operation
  DataTableRef	    dest_data;	// destination (result) data for operation

  TA_SIMPLE_BASEFUNS(DataProg);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataSortProg : public DataProg { 
  // sorts src_data into dest_data according to sort_spec
INHERITED(DataProg)
public:
  DataSortSpec		sort_spec; // data sorting specification

  override String GetDisplayName() const;
  void 	UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(DataSortProg);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataSelectRowsProg : public DataProg { 
  // selects rows from src_data into dest_data according to select_spec
INHERITED(DataProg)
public:
  DataSelectSpec	select_spec; // data selection specification

  override String GetDisplayName() const;
  void 	UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(DataSelectRowsProg);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataSelectColsProg : public DataProg { 
  // selects rows from src_data into dest_data according to select_spec
INHERITED(DataProg)
public:
  DataOpList		select_spec; // columns to select

  override String GetDisplayName() const;
  void 	UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(DataSelectColsProg);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API DataGroupProg : public DataProg { 
  // sorts src_data into dest_data according to sort_spec
INHERITED(DataProg)
public:
  DataGroupSpec		group_spec; // data grouping specification

  override String GetDisplayName() const;
  void 	UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(DataGroupProg);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};


#endif // ta_dataops_h
