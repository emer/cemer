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

#ifndef DataJoinSpec_h
#define DataJoinSpec_h 1

// parent includes:
#include <taINBase>

// member includes:
#include <DataOpEl>

// declare all other types mentioned but not required to include:
class DataTable; // 


taTypeDef_Of(DataJoinSpec);

class TA_API DataJoinSpec : public taINBase {
  // #STEM_BASE ##INLINE ##CAT_Data datatable join specification: combine two tables along matching column values -- tables are both sorted (internally) by join col, so results are in that sort order
  INHERITED(taINBase)
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

  String GetDisplayName() const override;
  TA_SIMPLE_BASEFUNS(DataJoinSpec);
protected:
  void	 CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void  Initialize();
  void 	Destroy()		{ };
};

#endif // DataJoinSpec_h
