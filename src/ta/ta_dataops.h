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

  void	UpdateAfterEdit();	// set col_name from column
  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataOpEl);
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

  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataSelectEl);
};

class TA_API DataSelectSpec : public DataOpList {
  // #CAT_Data a datatable select specification (list of select elements)
INHERITED(DataOpList)
public:

  TA_BASEFUNS(DataSelectSpec);
private:
  void	Initialize() 		{ SetBaseType(&TA_DataSelectEl); }
  void 	Destroy()		{ };
};

/////////////////////////////////////////////////////////
//   Grouping Spec
/////////////////////////////////////////////////////////

class TA_API DataGroupEl : public DataOpEl {
  // one element of a data grouping specification
  INHERITED(DataOpEl)
public:
  enum Operator {		
    GROUP,			// group by this field
    MIN,			// Minimum
    MAX,			// Maximum
    ABS_MIN,			// Minimum of absolute values
    ABS_MAX,			// Maximum of absolute values
    SUM,			// Summation
    PROD,			// Product
    MEAN,			// Mean of values
    VAR,			// Variance
    MEAN_STDEV,			// Mean and Standard deviation
    MEAN_SEM,			// Mean and Standard error of the mean
    COUNT, 			// Count of the number times count relation was true
  };

  Operator		op;		// operation to perform in grouping this column
  CountParam		count;		// #CONDEDIT_ON_op:COUNT parameters for the COUNT grouping

  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataGroupEl);
};

class TA_API DataGroupSpec : public DataOpList {
  // #CAT_Data a datatable grouping specification (list of group elements)
INHERITED(DataOpList)
public:
  DataGroupEl::Operator		default_op;
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
  
  static bool	CopyDataStru(DataTable* dest, DataTable* src);
  // copy datatable structure (columns)

  static bool	Sort(DataTable* dest, DataTable* src, DataSortSpec* spec);
  // sort data from src into dest according to sorting specifications in spec; dest is completely overwritten

  static void 	Sort_CopyRow(DataTable* dest, int dest_row, DataTable* src, int src_row);
  // helper function for sorting: copy one row from src to dest
  static int 	Sort_Compare(DataTable* dt_a, int row_a, DataTable* dt_b, int row_b,
			     DataSortSpec* spec);
  // helper function for sorting: compare values -1 = a is < b; 1 = a > b; 0 = a == b
  static bool 	Sort_impl(DataTable* dt, DataSortSpec* spec);
  // actually perform sort on data table using specs

  static bool	Select(DataTable* dest, DataTable* src, DataSelectSpec* spec);
  // select data from src into dest according to selection specifications in spec
  static bool	Group(DataTable* dest, DataTable* src, DataGroupSpec* spec);
  // group data from src into dest according to grouping specifications in spec


  void Initialize() { };
  void Destroy() { };
  TA_BASEFUNS(taDataOps);
};

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

class TA_API DataProg : public ProgEl { 
  // a program element for data operations (virtual base class)
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
INHERITED(ProgEl)
public:
  DataSortSpec		sort_spec; // data sorting specification

  TA_SIMPLE_BASEFUNS(DataSortProg);
protected:
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};


#endif // ta_dataops_h
