// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef NetGroupedDataLoop_h
#define NetGroupedDataLoop_h 1

// parent includes:
#include "network_def.h"
#include <Loop>
#include <ProgVar>

// member includes:
#include <int_Array>

// declare all other types mentioned but not required to include:

eTypeDef_Of(NetGroupedDataLoop);

class E_API NetGroupedDataLoop: public Loop { 
  // #OBSOLETE (This is obsolete: use NetDataLoop with group flag instead) loops over items in a DataTable, in different basic orderings, using index to select current data table item using ReadItem(index) call, so that later processes will access this row of data.  Note: assumes that there is a 'network' variable defined in program!!
INHERITED(Loop)
public:
  enum Order {
    SEQUENTIAL,			// present events (input data rows) in sequential order
    PERMUTED,			// permute the order of event (input data row) presentation
    RANDOM, 			// pick an event (input data row) at random (with replacement)
  };

  ProgVarRef	data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to the data table to use
  ProgVarRef	group_index_var; // #ITEM_FILTER_StdProgVarFilter program variable for the group index used in the loop -- goes from 0 to number of groups in data table-1
  ProgVarRef	item_index_var; // #ITEM_FILTER_StdProgVarFilter program variable for the item index used in the loop -- goes from 0 to number of items in current group
  ProgVarRef	group_order_var; // #ITEM_FILTER_StdProgVarFilter variable that contains the order to process data groups in -- is automatically created if not set
  ProgVarRef	item_order_var; // #ITEM_FILTER_StdProgVarFilter variable that contains the order to process data items in -- is automatically created if not set
  Order		group_order;	// #READ_ONLY #SHOW order to process data groups in -- set from group_order_var
  Order		item_order;	// #READ_ONLY #SHOW order to process data items in -- set from item_order_var
  int		group_col;	// column in the data table that contains the group names
  bool		update_after;	// call update-after-edit on the network object after changing the trial counter -- this is necessary to update control panels that monitor information at the trial level
  int_Array	group_idx_list;	// #READ_ONLY list of group starting indicies
  int_Array	item_idx_list;	// #READ_ONLY list of item indicies within group

  override String	GetDisplayName() const;

  virtual void	GetOrderVals();
  // get order values from order_var variables
  virtual void	GetGroupList();
  // initialize the group_idx_list from the data: idx's are where group name changes
  virtual void  GetItemList(int group_idx); // 
  override String	GetToolbarName() const { return "gp data lp"; }

  PROGEL_SIMPLE_BASEFUNS(NetGroupedDataLoop);
protected:
  virtual void	GetOrderVars(); // make order variables in program
  virtual void	GetIndexVars(); // make index variables in program if not already set
  override void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		GenCssPre_impl(Program* prog); 
  override void		GenCssPost_impl(Program* prog); 

private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
};

#endif // NetGroupedDataLoop_h
