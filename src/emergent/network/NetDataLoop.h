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

#ifndef NetDataLoop_h
#define NetDataLoop_h 1

// parent includes:
#include "network_def.h"
#include <DataLoop>

// member includes:
#include <int_Array>

// declare all other types mentioned but not required to include:
class Network; //

eTypeDef_Of(NetDataLoop);

// note: the motivation for supporting dmem within the basic data loops is so that the
// same project can be run transparently in dmem or non-dmem mode without modification

class E_API NetDataLoop: public DataLoop { 
  // For network input data: loops over items in a DataTable, in different basic orderings, using index to select current data table item using ReadItem(index) call, so that later processes will access this row of data. Note: assumes that there is a 'network' object pointer variable and an int 'trial' counter variable defined in the program!!
INHERITED(DataLoop)
public:
  bool		update_after;	// call update-after-edit on the network object after changing the trial counter -- this is necessary to update control panels that monitor information at the trial level
  int		dmem_nprocs;	// #READ_ONLY number of processors to use for distributed memory processing (input data distributed over nodes) -- computed automatically if dmem is active; else set to 1
  int		dmem_this_proc;	// #READ_ONLY processor rank for this processor relative to communicator group
  bool		grouped;	// present items according to group ordering, where all the items in a group have the same value in the designated group column of the input data table -- order can be independently selected for the groups as compared to the items (plain order specifies items within the group)
  Variant	group_col;	// #CONDSHOW_ON_grouped column in the data table that contains the group names -- can specify either by column name (set variant type to String) or index (set variant type to Int)
  ProgVarRef	group_index_var; // #CONDSHOW_ON_grouped #ITEM_FILTER_StdProgVarFilter program variable for the group index used in the loop -- goes from 0 to number of groups in data table-1
  ProgVarRef	group_order_var; // #CONDSHOW_ON_grouped #ITEM_FILTER_StdProgVarFilter variable that contains the order to process data groups in -- is automatically created if not set
  Order		group_order;	// #CONDSHOW_ON_grouped #READ_ONLY #SHOW order to process data groups in -- set from group_order_var
  int_Array	group_idx_list;	// #READ_ONLY list of group starting indicies

  virtual void	DMem_Initialize(Network* net);
  // configure the dmem communicator stuff: depends on dmem setup of network

  void	GetOrderVal() override;
  // get order values from order_var variables
  virtual void	GetGroupList();
  // initialize the group_idx_list from the data: idx's are where group name changes
  virtual void  GetItemList(int group_idx);
  // for grouped case, get items per group

  String GetDisplayName() const override;
  String GetToolbarName() const override { return "net data\nloop"; }

  PROGEL_SIMPLE_BASEFUNS(NetDataLoop);
protected:
  void	GetOrderVar() override; // make an order variable in program if not already set
  void	GetIndexVar() override; // make an index variable in program if not already set
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		GenCssPre_impl(Program* prog) override; 
  void		GenCssPost_impl(Program* prog) override; 

private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
};

#endif // NetDataLoop_h
