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

#ifndef DataLoop_h
#define DataLoop_h 1

// parent includes:
#include <Loop>

// member includes:
#include <ProgVar>
#include <int_Array>

// declare all other types mentioned but not required to include:
class DataBlock; // 


taTypeDef_Of(DataLoop);

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
  int_Array	item_idx_list;	// #READ_ONLY #NO_SAVE list of item indicies (permuted if permuted, otherwise in sequential order)

  virtual DataBlock* GetData();
  // get actual data table pointer from variable
  virtual void	GetOrderVal();
  // get order value from order_var variable

  String	GetDisplayName() const CPP11_OVERRIDE;
  String	GetToolbarName() const CPP11_OVERRIDE { return "data loop"; }

  PROGEL_SIMPLE_BASEFUNS(DataLoop);
protected:
  virtual void	GetOrderVar(); // make an order variable in program if not already set
  virtual void	GetIndexVar(); // make an index variable in program if not already set
  void	UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void	CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		GenCssPre_impl(Program* prog) CPP11_OVERRIDE; 
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE; 
  void		GenCssPost_impl(Program* prog) CPP11_OVERRIDE; 
  void	SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
				     int sls, void* op1_, void* op2_) CPP11_OVERRIDE;

private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
};

#endif // DataLoop_h
