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

#ifndef NetMonitor_h
#define NetMonitor_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <NetMonItem_List>
#include <Network>
#include <DataTable>

// declare all other types mentioned but not required to include:

eTypeDef_Of(NetMonitor);

class E_API NetMonitor: public taNBase {
  // #STEM_BASE ##TOKENS #NO_UPDATE_AFTER ##CAT_Network  ##DEF_CHILD_items monitors values from network (or other) objects and sends them to a data table/sink
INHERITED(taNBase)
public:
  NetMonItem_List	items;
  // the list of items being monitored
  NetworkRef		network;
  // the overall network object that is being monitored -- if changed, any sub-objects will be updated based on path to new network
  DataTableRef		data;
  // the data table that will be used to hold the monitor data
  bool			rmv_orphan_cols;
  // #DEF_true remove orphan columns when updating table schema

  void		SetNetwork(Network* net);
  // #MENU #MENU_ON_Action #MENU_CONTEXT #CAT_Monitor #INIT_ARGVAL_ON_network set the overall network -- also will update any sub-objects to corresponding ones on this network -- calls ResetDataTableCols too -- don't call if columns are manually added or this is adding additional rows to a table
  void		SetDataTable(DataTable* dt);
  // #MENU #MENU_CONTEXT #CAT_Monitor #INIT_ARGVAL_ON_data set the data table used
  void		SetDataNetwork(DataTable* dt, Network* net);
  // #CAT_Monitor set both the data table and network -- convenient for programs -- calls ResetDataTableCols too -- don't call if columns are manually added or this is adding additional rows to a table

  NetMonItem* 	AddBlank();
  // #BUTTON #CAT_Monitor add a new blank monitor to be filled in
  NetMonItem* 	AddNetwork(Network* net, const String& variable);
  // #BUTTON #CAT_Monitor monitor a value in the Network or its subobjects
  NetMonItem* 	AddLayer(Layer* lay, const String& variable);
  // #BUTTON #CAT_Monitor #PROJ_SCOPE monitor a value in the Layer or its subobjects
  NetMonItem* 	AddProjection(Projection* prj, const String& variable);
  // #BUTTON #CAT_Monitor #PROJ_SCOPE monitor a value in the Projection or its subobjects
  NetMonItem* 	AddUnitGroup(Unit_Group* ug, const String& variable);
  // #CAT_Monitor #PROJ_SCOPE monitor a value in the UnitGroup or its subobjects
  NetMonItem* 	AddUnit(Unit* un, const String& variable);
  // #CAT_Monitor #PROJ_SCOPE monitor a value in the Unit or its subobjects
  
  NetMonItem* 	AddObject(taBase* obj, const String& variable);
  // #CAT_Monitor #PROJ_SCOPE monitor a value in the object or its subobjects

  NetMonItem* 	AddNetMax();
  // #BUTTON #CAT_Monitor For Leabra only: add a monitor item to monitor the avg_netin.max variable at the network level (for all layers in the network), which is very useful in Leabra for tuning the network parameters to ensure a proper range of max netinput values -- must also turn on compute_rel_netin flags at the Trial and Epoch programs for this data to be computed in the first place
  NetMonItem* 	AddNetRel();
  // #BUTTON #CAT_Monitor add a monitor item to monitor the prjns.avg_netin_rel variable at the network level (for all layers and projections in the network), which is very useful in Leabra for tuning the network parameters to achieve desired relative netinput contributions across different projections -- must also turn on compute_rel_netin flags at the Trial and Epoch programs for this data to be computed in the first place
  NetMonItem* 	AddLayActAvg();
  // #BUTTON #CAT_Monitor For Leabra only: add a monitor item to monitor the acts_m_avg.avg variable at the network level (for all layers in the network), which is very useful in Leabra for tuning the network parameters -- this value tells you how much activity on average there is in each layer over time -- it should match the target pct activity value relatively closely, for the netinput scaling and other calculations to be accurate

  void 		UpdateDataTable(bool reset_first = false);
  // #BUTTON #CAT_Monitor update the datatable configuration to match current set of monitored items -- call this during Init. if reset_first, then existing data rows are removed first
  void 		UpdateMonitors(bool reset_first = false) { UpdateDataTable(reset_first); }
  // #CAT_Monitor old name for UpdateDataTable
  void 		ResetDataTableCols();
  // #BUTTON #CONFIRM #CAT_Monitor reset the data table columns, and then call UpdateDataTable -- this is useful when new monitor items have been inserted in the middle of the list (they will appear at the end of the data table unless this function is called) -- WARNING: removes any columns that might have been manually added!!

  void 		GetMonVals();
  // #CAT_Monitor get all the values and store in current row of data table -- call in program to get new data
  void		RemoveMonitors();
  // #IGNORE called by the network to remove the objs from lists
  void		UpdateNetworkPtrs();
  // #IGNORE update pointers to objects within current network

  taList_impl*	children_() override {return &items;}
  String	GetDisplayName() const override;
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const NetMonitor& cp);
  TA_BASEFUNS(NetMonitor);
  
protected:
  void	UpdateAfterEdit_impl();
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;
  void	CheckChildConfig_impl(bool quiet, bool& rval) override;
private:
  void		Initialize();
  void		Destroy() {CutLinks();}
};

#endif // NetMonitor_h
