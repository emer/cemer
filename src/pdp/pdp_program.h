// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// pdp_program.h -- pdp extensions to program

#ifndef PDP_PROGRAM_H
#define PDP_PROGRAM_H

#include "ta_program.h"
#include "ta_dmem.h"

class Network;

#include "pdp_def.h"
#include "pdp_TA_type.h"

// note: the motivation for supporting dmem within the basic data loops is so that the
// same project can be run transparently in dmem or non-dmem mode without modification

class PDP_API BasicDataLoop: public Loop { 
  // loops over items in a DataTable, in different basic orderings, using index to select current data table item using ReadItem(index) call, so that later processes will access this row of data; Note: assumes that there is a network variable defined in program!!
INHERITED(Loop)
public:
  enum Order {
    SEQUENTIAL,			// present events in sequential order
    PERMUTED,			// permute the order of event presentation
    RANDOM 			// pick an event at random (with replacement)
  };

  ProgVarRef	data_var;	// program variable pointing to the data table to use
  Order		order;		// order to process data items in
  int_Array	item_idx_list;	// #READ_ONLY list of item indicies 
  int		dmem_nprocs;	// number of processors to use for distributed memory processing (input data distributed over nodes) -- computed automatically if dmem is active; else set to 1
  int		dmem_this_proc;	// processor rank for this processor relative to communicator group

  virtual void	DMem_Initialize(Network* net);
  // configure the dmem communicator stuff: depends on dmem setup of network

  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(BasicDataLoop);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
};

class PDP_API GroupedDataLoop: public Loop { 
  // loops over items in a DataTable, in different basic orderings, using index to select current data table item using ReadItem(index) call, so that later processes will access this row of data
INHERITED(Loop)
public:
  enum Order {
    SEQUENTIAL,			// present events in sequential order
    PERMUTED,			// permute the order of event presentation
    RANDOM 			// pick an event at random (with replacement)
  };

  ProgVarRef	data_var;	// program variable pointing to the data table to use
  Order		group_order;	// order to process data groups in
  Order		item_order;	// order to process data items in
  int		group_col;	// column in the data table that contains the group names
  int_Array	group_idx_list;	// #READ_ONLY list of group starting indicies
  int_Array	item_idx_list;	// #READ_ONLY list of item indicies within group

  override String	GetDisplayName() const;

  virtual void	GetGroupList();
  // initialize the group_idx_list from the data: idx's are where group name changes
  virtual void  GetItemList(int group_idx); // 

  TA_SIMPLE_BASEFUNS(GroupedDataLoop);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
};

class PDP_API NetCounterInit: public ProgEl { 
  // initialize a network counter: program keeps a local version of the counter, and updates both this and the network's copy
INHERITED(ProgEl)
public:
  ProgVarRef	network_var;	// #SCOPE_Program_Group variable that points to the network (typically a global_var)
  TypeDef*	network_type;	// #HIDDEN #NO_SAVE just to anchor the memberdef*
  ProgVarRef 	local_ctr_var;	// #SCOPE_Program_Group local version of the counter variable, maintained by the program -- must have same name as the counter!
  MemberDef*	counter;	// #TYPE_ON_network_type #CATDEF_Counter counter variable on network to operate on
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  TA_SIMPLE_BASEFUNS(NetCounterInit);

protected:
  override void	UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  virtual void	GetLocalCtrVar(); // if counter is not empty and local_ctr_var == NULL, then get a local ctr var for it

  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy();
};

class PDP_API NetCounterIncr: public ProgEl { 
  // initialize a network counter: program keeps a local version of the counter, and updates both this and the network's copy
INHERITED(ProgEl)
public:
  ProgVarRef	network_var;	// #SCOPE_Program_Group variable that points to the network (typically a global_var)
  TypeDef*	network_type;	// #HIDDEN #NO_SAVE just to anchor the memberdef*
  ProgVarRef 	local_ctr_var;	// #SCOPE_Program_Group local version of the counter variable, maintained by the program -- must have same name as the counter!
  MemberDef*	counter;	// #TYPE_ON_network_type  #CATDEF_Counter counter variable on network to operate on
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  TA_SIMPLE_BASEFUNS(NetCounterIncr);

protected:
  override void	UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  virtual void	GetLocalCtrVar(); // if counter is not empty and local_ctr_var == NULL, then get a local ctr var for it

  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy();
};

class PDP_API NetUpdateView: public ProgEl { 
  // update the network view, conditional on an update_net_view variable that is created by this progam element
INHERITED(ProgEl)
public:
  ProgVarRef	network_var;	// #SCOPE_Program_Group variable that points to the network (typically a global_var)
  ProgVarRef	update_var;	// #SCOPE_Program_Group variable that controls whether we update the display or not
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Function"; }

  TA_SIMPLE_BASEFUNS(NetUpdateView);

protected:
  override void	UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  virtual void	GetUpdateVar(); // get the update_var variable

  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy();
};

#endif
