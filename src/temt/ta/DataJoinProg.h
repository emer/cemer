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

#ifndef DataJoinProg_h
#define DataJoinProg_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <ProgVar>
#include <DataJoinSpec>

// declare all other types mentioned but not required to include:
class DataTable; // 


taTypeDef_Of(DataJoinProg);

class TA_API DataJoinProg : public DataSrcDestProg { 
  // joins two datatables (src and src_b) into dest datatable indexed by a common column
INHERITED(DataSrcDestProg)
public:
  ProgVarRef		src_b_data_var;	// #ITEM_FILTER_DataProgVarFilter variable pointing to second source data for operation
  DataJoinSpec		join_spec; 	// #SHOW_TREE data grouping specification

  virtual DataTable* GetSrcBData(); // get source data table pointer from src_data_var (or NULL)

  override void	UpdateSpecDataTable();

  override String GetDisplayName() const;
  override String	GetToolbarName() const { return "join"; }

  PROGEL_SIMPLE_BASEFUNS(DataJoinProg);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataJoinProg_h
