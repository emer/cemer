// Copyright, 1995-2007, Regents of the University of Colorado,
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



// netdata.h

#ifndef NETDATA_H
#define NETDATA_H

#include "ta_dataproc.h"
#include "netstru.h"

// This is a workaround for a problem that can occur depending on the order of
// includes.  The problem is that the windows header file "Nb30.h" contains
// "#define GROUP_NAME 0x80".  If that header gets #included before this one,
// you get all sorts of cryptic compiler errors, since a literal 0x80 appears
// in the NetTarget enum definition.  This #undef is relatively safe, since
// the "Nb30.h" header is related to NetBIOS stuff which Emergent doesn't use.
//
// First make sure the GROUP_NAME macro has been defined, then undefine it.
// It's necessary to ensure it has been defined first, otherwise if some .cpp
// file #includes this file, then "t3viewer.h", it would end up with the macro
// defined, and would fail to compile if it used LayerDataEl::GROUP_NAME.
// The include chain, starting at t3viewer.h, is:
//   t3viewer.h -> QuarterWidget.h -> QGLWidget -> qgl.h -> qt_windows.h
//     -> windows.h -> nb30.h
#include "t3viewer.h"
#ifdef GROUP_NAME
  #pragma message("Warning: undefining GROUP_NAME macro")
  #undef GROUP_NAME
#endif

//////////////////////////////////////////
//	Layer Reader / Writer		//
//////////////////////////////////////////

class EMERGENT_API LayerDataEl: public taOBase  {
  // #VIRT_BASE #NO_INSTANCE #NO_TOKENS ##CAT_Network controls the reading/writing of information to/from layers and data blocks/tables
friend class LayerDataEl_List;
friend class LayerWriter_List;
INHERITED(taOBase)
public:
  enum NetTarget {
    LAYER,			// read/write the layer information
    TRIAL_NAME,			// read/write the network trial_name field
    GROUP_NAME,			// read/write the network group_name field
  };

  DataBlockRef		data;
  // #READ_ONLY #HIDDEN #NO_SAVE source or sink of the data to apply to layer, or store layer output (as appropriate) -- managed by owner
  DataTableCols*	data_cols;
  // #READ_ONLY #HIDDEN #NO_SAVE data table columns -- gets set dynamically if data is a datatable, just for choosing column..

  DataCol*		col_lookup;
  // #NO_SAVE #FROM_GROUP_data_cols #NO_EDIT #NO_UPDATE_POINTER column/channel in data table use -- just to lookup the chan_name, which is what is actually used -- this is reset to NULL after column is selected
  String		chan_name;
  // name of the channel/column in the data to use 

  NetTarget		net_target;
  // what to read/write from on the network

  NetworkRef 		network;
  // #READ_ONLY #HIDDEN #NO_SAVE the network to operate on -- managed by owner
  Layer_Group* 		layer_group;
  // #READ_ONLY #HIDDEN #NO_SAVE the group of layers on the network -- just for choosing the layer from a list
  LayerRef 		layer;
  // #NO_SAVE #CONDSHOW_ON_net_target:LAYER #FROM_GROUP_layer_group #PROJ_SCOPE the Layer that will get read or written -- this is just for choosing layer_name from a list -- will be reset after selection is applied
  String 		layer_name;
  // #CONDSHOW_ON_net_target:LAYER the name of the Layer that will get read or written

  PosTwoDCoord		offset;
  // #EXPERT offset in layer or unit group at which to start reading/writing
  
  virtual void 	SetDataNetwork(DataBlock* db, Network* net);
  // #CAT_LayerData set the data table and network pointers enable looking up columns/layer names

  virtual int	GetChanIdx(DataBlock* db) { return -1; }
  // #CAT_LayerData get channel index from data block: depends on source or sink (override in subclass)

  String	GetName() const			{ return chan_name; }
  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(LayerDataEl);
protected:
  void  UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);

private:
  void	Initialize();
  void 	Destroy();
};

class EMERGENT_API LayerDataEl_List: public taList<LayerDataEl> {
  // ##CAT_Network list of individual LayerDataEl objects
INHERITED(taList<LayerDataEl>)
public:

  virtual void 	SetDataNetwork(DataBlock* db, Network* net);
  // #CAT_LayerData set the data table and network pointers enable looking up columns/layer names

  virtual LayerDataEl* FindChanName(const String& chn_name);
  // #CAT_LayerData find (first) layer data that applies to given data channel name
  virtual LayerDataEl* FindMakeChanName(const String& chn_name, bool& made_new);
  // #CAT_LayerData find (first) layer data that applies to given data channel name -- make it if it doesn't exist

  virtual LayerDataEl* FindLayerName(const String& lay_name);
  // #CAT_LayerData find (first) layer data that applies to given layer name
  virtual LayerDataEl* FindMakeLayerName(const String& lay_name, bool& made_new);
  // #CAT_LayerData find (first) layer data that applies to given layer name -- make it if it doesn't exist 

  virtual LayerDataEl* FindLayerData(const String& chn_name, const String& lay_name);
  // #CAT_LayerData find (first) layer data that applies to given data channel name and layer name
  virtual LayerDataEl* FindMakeLayerData(const String& chn_name, const String& lay_name,
					 bool& made_new);
  // #CAT_LayerData find (first) layer data that applies to given data channel name and layer name -- make it if it doesn't exist

  TA_BASEFUNS_NOCOPY(LayerDataEl_List);
private:
  void	Initialize() { SetBaseType(&TA_LayerDataEl); }
  void 	Destroy() {}
};

//////////////////////////////////////////////
//		Layer Writer

class EMERGENT_API LayerWriterEl : public LayerDataEl {
  // #STEM_BASE controls the writing of input data from a data source to a network layer
INHERITED(LayerDataEl)
public: 
  bool		use_layer_type; // #DEF_true use layer_type information on the layer to determine flags to set (if false, turn on EXPERT showing to view flags)
  bool		quiet;		// turn off warning messages (e.g., about layer_type = HIDDEN)
  bool		na_by_range;	// #EXPERT use act_range on the unitspec for the units to determine inputs that are not appplicable (n/a) and thus do not get relevant flags or values set: those that have input values outside the range are n/a
  Unit::ExtType	ext_flags;	// #EXPERT #CONDSHOW_OFF_use_layer_type:true how to flag the unit/layer's external input status
  RandomSpec	noise;		// #EXPERT noise optionally added to values when applied

  virtual bool	ApplyInputData(DataBlock* db, Network* net);
  // #CAT_LayerWriter apply data to the layer on network, using the network's current context settings (TEST,TRAIN,etc) -- returns success

  override int	GetChanIdx(DataBlock* db) { return db->GetSourceChannelByName(chan_name); }

  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(LayerWriterEl);
protected:
  override void	CheckThisConfig_impl(bool quiet, bool& rval);

private:
  void	Initialize();
  void 	Destroy();
};

class EMERGENT_API LayerWriter : public taNBase {
  // #STEM_BASE ##CAT_Network ##DEF_CHILD_layer_data #DEF_CHILDNAME_LayerDataEls controls the writing of input data from a data source to network layers
INHERITED(taNBase)
public:
  DataBlockRef		data;
  // the data object with input data to present to the network
  NetworkRef		network;
  // the network to present the input data to
  LayerDataEl_List	layer_data;
  // the layers/input data channel mappings to present to the network

  virtual LayerDataEl*	AddLayerData()	{ return (LayerDataEl*)layer_data.New(1); }
  // #BUTTON add a new layer data item

  virtual void 	SetDataNetwork(DataBlock* db, Network* net);
  // #CAT_LayerWriter set the data table and network pointers -- convenience function for programs 

  virtual void	AutoConfig(bool remove_unused = true);
  // #BUTTON #CAT_LayerWriter do a 'best guess' configuration of items by matching up like-named data Channels and network Layers -- if remove_unused is true, then layer writer elements that existed previously but were not found in input data and network are removed

  virtual bool	ApplyInputData();
  // #CAT_LayerWriter apply data to the layers, using the network's current context settings (TEST,TRAIN,etc) -- returns success

  override taList_impl*	children_() {return &layer_data;}
  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(LayerWriter);
protected:
  void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void CheckChildConfig_impl(bool quiet, bool& rval);
    
private:
  void	Initialize();
  void 	Destroy() {}
};


/////////////////////////////////////////////////////////////////
//	Network Monitor: record values from objects!


class EMERGENT_API NetMonItem: public taNBase {
  // #STEM_BASE #NO_TOKENS ##CAT_Network used for monitoring the value of an object\n(special support for network variables, including Layer, Projection, UnitGroup, Unit)
INHERITED(taNBase)
public:
  enum	NameStyle {		// how to name the datatable columns
    AUTO_NAME,			// auto-generate a name based on the object name and the variable name, etc
    MY_NAME,			// always use my (net monitor item) name; if multiple columns, then add a subscript index for later ones (_1 _2, etc.)
  };

  bool			off; // #NO_SAVE_EMPTY set this to not use this netmon item
  bool			computed;	// if true, this value is computed separately in a program, and this is here just to make a place for it in the output data (note: computation sold separately -- must be performed elsewhere)
  TypeDef*		object_type;	// #CONDSHOW_OFF_computed LAYER #TYPE_taOBase type of object to monitor (narrows down the choices when choosing the object)
  taSmartRef 		object;		// #CONDSHOW_OFF_computed #TYPE_ON_object_type #PROJ_SCOPE the network object being monitored
  MemberDef*		lookup_var;	// #CONDSHOW_OFF_computed #TYPE_ON_object_type #NULL_OK #NO_SAVE #NO_EDIT lookup a member variable to monitor -- this just enters the name into the variable field and then auto-resets to NULL.  you can also just type variable directly, esp for non-members (r.wt, etc)
  String        	variable;	// #CONDSHOW_OFF_computed Variable on object to monitor.  Can also be a variable on sub-objects (e.g., act on Layer or Network will get all unit activations); r. and s. indicate recv and send connection vals (e.g., r.wt), projections or prjns gets projection-level variables; can specify vars on particular unit(s) within a layer as 'units[index<-endidx>].varname' or 'units[gpno][index<-endidx>].varname' where the index value is a leaf in the first case and within a given unit group in the second -- in both cases a range of units can be optionally specified
  String		var_label;	// #CONDSHOW_OFF_computed label to use in place of variable in naming the columns/channels generated from this data (if empty, variable is used)
  NameStyle		name_style;	 // #CONDSHOW_OFF_computed how to name the columns/channels generated from this data?
  int			max_name_len;	 // #DEF_6 maximum length for any name segment

  ValType		val_type;       // #CONDSHOW_ON_computed type of data column to create (only for computed variables)
  bool			matrix;		// #CONDSHOW_ON_computed if true, create a matrix data column (otherwise scalar)
  MatrixGeom		matrix_geom;	// #CONDSHOW_ON_matrix geometry of matrix to create if a matrix type

  bool			data_agg; 	// #CONDSHOW_ON_computed compute value automatically from a column of data in another data table
  DataTableRef		data_src;	// #CONDSHOW_ON_data_agg source data for data aggregation operation
  DataOpEl		agg_col;	// #CONDSHOW_ON_data_agg column name in data_src data table to get data to aggregate from
  
  AggregateSpec		agg;		// #CONDSHOW_ON_computed:false||data_agg:true aggregation operation to perform (reduces vector data down to a single scalar value for network variables, and is aggregation to perform for data_agg aggregation)

  bool			select_rows;	// #CONDSHOW_ON_data_agg whether to select specific rows of data from the data_src data table to operate on
  DataSelectEl		select_spec;	// #CONDSHOW_ON_select_rows optional selection of rows to perform aggregation on according to the value of items in given column -- for more complex selections and/or greater efficiency, use DataSelectRowsProg to create intermediate data table and operate on that

  ChannelSpec_List	val_specs;	// #HIDDEN_TREE #NO_SAVE specs of the values being monitored 
  ChannelSpec_List	agg_specs;	// #HIDDEN_TREE #NO_SAVE specs of the agg values -- these are the matrix values whereas the val_specs contain the agg'd scalar values
  MemberSpace   	members;	// #IGNORE memberdefs
  voidptr_Array		ptrs;     	// #HIDDEN #NO_SAVE actual ptrs to values

  SimpleMathSpec 	pre_proc_1;	// #EXPERT first step of pre-processing to perform
  SimpleMathSpec 	pre_proc_2;	// #EXPERT second step of pre-processing to perform
  SimpleMathSpec 	pre_proc_3;	// #EXPERT third step of pre-processing to perform
  
  String  	GetAutoName(taBase* obj); 
  // get auto-name value based on current values
  String  	GetObjName(taBase* obj); 
  // get name of object for naming monitored values -- uses GetDisplayName by default but is optimized for various network objects; uses max_name_len constraint
  String	GetChanName(taBase* obj, int chan_idx);
  // get name for given column/channel of data, taking into account namestyle preferences; chan_idx is index within the channelspec_list for this guy

  void		SetMonVals(taBase* obj, const String& var); 
  // #CAT_Monitor set object and variable, and update appropriately
  virtual void 	GetMonVals(DataBlock* db);
  // #CAT_Monitor get the monitor data and stick it in the current row of the datablock/datatable
  void		ResetMonVals();
  // #CAT_Monitor deletes the cached vars

  void		ScanObject();
  // #CAT_Monitor get the monitor data information from the object

  static const KeyString key_obj_name;
  static const KeyString key_obj_type;
  static const KeyString key_obj_var;
  String GetColText(const KeyString& key, int itm_idx = -1) const;

  int		GetEnabled() const {return (off) ? 0 : 1;}
  void		SetEnabled(bool value) {off = !value;}
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const NetMonItem& cp);
  TA_BASEFUNS(NetMonItem);//
  
protected:
  float_Matrix		agg_tmp_calc; // temp calc matrix for agg data
  float_Matrix		agg_tmp_calc_2; // temp calc matrix for agg data
  double_Matrix		agg_tmp_calc_d; // temp calc matrix for agg data

  void	UpdateAfterEdit_impl();
  int			cell_num; // current cell number, when adding mon vals
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
  override void		SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
					     int dcr, void* op1_, void* op2_);

  ChannelSpec* 		AddScalarChan(const String& valname, ValType vt);
  ChannelSpec* 		AddScalarChan_Agg(const String& valname, ValType vt);
  // add to the agg_specs just to keep it consistent
  MatrixChannelSpec* 	AddMatrixChan(const String& valname, ValType vt,
				      const MatrixGeom* geom = NULL);
  // caller resp for somehow setting geom if NULL; clears cell_num
  bool	 		GetMonVal(int i, Variant& rval); // get the value at i, true if exists
  void 			GetMonVals_Agg(DataBlock* db);
  // special version for agg case
  void 			GetMonVals_DataAgg(DataBlock* db);
  // special version for data_agg case

  // these are for finding the members and building the stat
  // out of the objects and the variable
  
  bool 	ScanObject_InObject(taBase* obj, String var, taBase* name_obj);
  // if name_obj == NULL, don't make a column for this guy
  bool ScanObject_InUserData(taBase* obj, String var, taBase* name_obj);
  // called when an InObject var is "user_data.xxx[.yyy]"
  void	ScanObject_Network(Network* net, String var);
  void	ScanObject_Layer(Layer* lay, String var);
  void	ScanObject_LayerUnits(Layer* lay, String var);
  // specific subrange of units within a layer
  void	ScanObject_Projection(Projection* p, String var);
  void	ScanObject_ProjectionGroup(Projection_Group* p, String var);
  void	ScanObject_UnitGroup(Unit_Group* ug, String var);
  void	ScanObject_Unit(Unit* u, String var);
  // this is only when the object itself is a unit

  // these are only for r. and s. con variables
  void	ScanObject_LayerCons(Layer* lay, String var);
  void	ScanObject_PrjnCons(Projection* p, String var);
  // known to be a connection variable (r.x or s.x); matrix already allocated

  void	ScanObject_RecvCons(RecvCons* cg, String var);
  void	ScanObject_SendCons(SendCons* cg, String var);
  void	ScanObject_BiasCon(RecvCons* cg, String var, taBase* name_obj);
  // called for bias.x where bias is a RecvCons

private:
  void	Initialize();
  void 	Destroy() {CutLinks();}
};


class EMERGENT_API NetMonItem_List: public taList<NetMonItem> { 
  // ##CAT_Network list of network monitor items
INHERITED(taList<NetMonItem>)
public:

  int	NumListCols() const {return 3;} 
  const KeyString GetListColKey(int col) const;
  String GetColHeading(const KeyString&) const; // header text for the indicated column
  TA_BASEFUNS_NOCOPY(NetMonItem_List);
  
private:
  void		Initialize() {SetBaseType(&TA_NetMonItem);}
  void		Destroy() {}
};

class EMERGENT_API NetMonitor: public taNBase {
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
  NetMonItem* 	AddNetwork(Network* net, const String& variable)
  { return AddObject(net, variable);}
  // #BUTTON #CAT_Monitor monitor a value in the Network or its subobjects
  NetMonItem* 	AddLayer(Layer* lay, const String& variable)
  { return AddObject(lay, variable);}
  // #BUTTON #CAT_Monitor #PROJ_SCOPE monitor a value in the Layer or its subobjects
  NetMonItem* 	AddProjection(Projection* prj, const String& variable)
  { return AddObject(prj, variable);}
  // #BUTTON #CAT_Monitor #PROJ_SCOPE monitor a value in the Projection or its subobjects
  NetMonItem* 	AddUnitGroup(Unit_Group* ug, const String& variable)
  { return AddObject(ug, variable);}
  // #CAT_Monitor #PROJ_SCOPE monitor a value in the UnitGroup or its subobjects
  NetMonItem* 	AddUnit(Unit* un, const String& variable)
  { return AddObject(un, variable);}
  // #CAT_Monitor #PROJ_SCOPE monitor a value in the Unit or its subobjects
  
  NetMonItem* 	AddObject(taBase* obj, const String& variable);
  // #CAT_Monitor #PROJ_SCOPE monitor a value in the object or its subobjects

  NetMonItem* 	AddNetMax();
  // #BUTTON #CAT_Monitor For Leabra only: add a monitor item to monitor the avg_netin.max variable at the network level (for all layers in the network), which is very useful in Leabra for tuning the network parameters to ensure a proper range of max netinput values -- must also turn on compute_rel_netin flags at the Trial and Epoch programs for this data to be computed in the first place
  NetMonItem* 	AddNetRel();
  // #BUTTON #CAT_Monitor add a monitor item to monitor the prjns.avg_netin_rel variable at the network level (for all layers and projections in the network), which is very useful in Leabra for tuning the network parameters to achieve desired relative netinput contributions across different projections -- must also turn on compute_rel_netin flags at the Trial and Epoch programs for this data to be computed in the first place

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

  override taList_impl*	children_() {return &items;}
  override String	GetDisplayName() const;
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const NetMonitor& cp);
  TA_BASEFUNS(NetMonitor);
  
protected:
  void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void	CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void		Initialize();
  void		Destroy() {CutLinks();}
};

class EMERGENT_API ActBasedRF : public taNBase {
  // #STEM_BASE ##CAT_Network computes an activation-based receptive field of network unit activations weighted by the activation of target units within selected target layer across a range of testing patterns: effectively shows what patterns of network activation are associated with the activation of target units, regardless of direct connectivity: columns of data table are for each network layer, and rows are the units within the target layer
INHERITED(taNBase)
public:
  enum NormMode {		// how to normalize the final rf data after computing the weighted averages
    NO_NORM,			// no normalization at all
    NORM_TRG_UNIT,		// normalize across each target unit separately: each row of the rf_data table has a max value of 1
    NORM_TRG_LAYER,		// normalize across entire target layer of units: entire rf_data table has a max value of 1
    NORM_RF_LAY,		// normalize across each receptive field layer (i.e., all the layers in the network) separately: each column of the rf_data table has a max value of 1
    NORM_TRG_UNIT_RF_LAY,	// normalize across each target unit for each receptive field layer: each cell of the rf_data table has a max value of 1
  };

  DataTableRef	rf_data;
  // the data table containing the results of the receptive field computation (is completely configured by this object!) -- columns are layers of the network, and rows are units of the trg_layer -- this is computed from sum_data / wt_array followed by normalization
  DataTable	sum_data;
  // #READ_ONLY #HIDDEN #NO_SAVE auxiliary data table in same format as rf_data for holding the sum of target unit activation-weighted activations: rf_data is sum_data / wt_array followed by normalization
  float_Matrix	wt_array;
  // #READ_ONLY #HIDDEN #NO_SAVE array of length = number of units in trg_layer -- contains accumulated weights for each unit
  NetworkRef	network;
  // the network to operate on -- all layers (except lesioned or iconified) are computed, with each layer getting a column of the data table
  LayerRef	trg_layer;
  // #PROJ_SCOPE the target layer to compute receptive fields for: each unit in the layer gets a row of the data table, and the columns in that row show the activation based receptive field for that unit for all the other layers in the network
  NormMode	norm_mode;
  // how to normalize the resulting values
  float		threshold;
  // threshold on absolute value of target unit activation for including in overall average -- can produce sharper results by increasing the threshold to only include cases where the unit is strongly active

  virtual void	ConfigDataTable(DataTable* dt, Network* net);
  // #CAT_ActBasedRF configure data table based on current network (called internally for rf_data, sum_data, and wt_data)

  virtual void	InitData();
  // #BUTTON #CAT_ActBasedRF initialize the data based on current network settings (calls ConfigDataTable on rf_data, sum_data, and wt_data, and intializes all values to 0)

  virtual void 	InitAll(DataTable* dt, Network* net, Layer* tlay);
  // #CAT_ActBasedRF set the data table, network, trg_lay pointers and initialize everything -- convenience function for init_code of programs -- resets all data first and then calls InitData after setting

  virtual bool	IncrementSums();
  // #BUTTON #CAT_ActBasedRF update the receptive field data based on current network activations (computes sum_data and wt_data, does NOT compute rf_data based on those: see ComputeRF)

  virtual bool	ComputeRF();
  // #BUTTON #CAT_ActBasedRF compute the rf_data based on currently accumulated sum_data and wt_data from multiple previous calls to IncrementSums function

  virtual bool	CopyRFtoNetWtPrjn(int trg_unit_no);
  // #BUTTON #CAT_ActBasedRF copy given target unit number's rf_data to the network's wt_prjn field (weight projection), for easy visualization in the network context

  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(ActBasedRF);
protected:
  void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
    
private:
  void	Initialize();
  void 	Destroy() {}
};

#endif // NETDATA_H
