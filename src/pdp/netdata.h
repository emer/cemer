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



// netdata.h

#ifndef NETDATA_H
#define NETDATA_H

#include "pdp_def.h"
#include "datatable.h"
#include "netstru.h"

#include "pdp_TA_type.h"

//////////////////////////////////////////
//	Layer Reader / Writer		//
//////////////////////////////////////////

class PDP_API LayerDataEl: public taOBase  {
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

  DataArray_impl*	column;
  // #NO_SAVE #FROM_GROUP_data_cols column/channel in data table use -- just to lookup the chan_name, which is what is actually used -- this is reset to NULL after column is selected
  String		chan_name;
  // name of the channel/column in the data to use 

  NetTarget		net_target;
  // what to read/write from on the network

  NetworkRef 		network;
  // #READ_ONLY #HIDDEN #NO_SAVE the network to operate on -- managed by owner
  Layer_Group* 		layer_group;
  // #READ_ONLY #HIDDEN #NO_SAVE the group of layers on the network -- just for choosing the layer from a list
  LayerRef 		layer;
  // #NO_SAVE #CONDEDIT_ON_net_target:LAYER #FROM_GROUP_layer_group the Layer that will get read or written -- this is just for choosing layer_name from a list -- will be reset after selection is applied
  String 		layer_name;
  // #CONDEDIT_ON_net_target:LAYER the name of the Layer that will get read or written

  PosTwoDCoord		offset;
  // #EXPERT offset in layer or unit group at which to start reading/writing
  
  virtual void 	SetDataNetwork(DataBlock* db, Network* net);
  // #CAT_LayerData set the data table and network pointers enable looking up columns/layer names

  virtual int	GetChanIdx(DataBlock* db) { return -1; }
  // #CAT_LayerData get channel index from data block: depends on source or sink (override in subclass)

  override String	GetDisplayName() const;

  void  UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(LayerDataEl);
protected:
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);

private:
  void	Initialize();
  void 	Destroy();
};

class PDP_API LayerDataEl_List: public taList<LayerDataEl> {
  // ##CAT_Network list of individual LayerDataEl objects
INHERITED(taList<LayerDataEl>)
public:

  virtual void 	SetDataNetwork(DataBlock* db, Network* net);
  // #CAT_LayerData set the data table and network pointers enable looking up columns/layer names

  virtual LayerDataEl* FindChanName(const String& chn_name);
  // #CAT_LayerData find (first) layer data that applies to given data channel name
  virtual LayerDataEl* FindMakeChanName(const String& chn_name);
  // #CAT_LayerData find (first) layer data that applies to given data channel name -- make it if it doesn't exist

  virtual LayerDataEl* FindLayerName(const String& lay_name);
  // #CAT_LayerData find (first) layer data that applies to given layer name
  virtual LayerDataEl* FindMakeLayerName(const String& lay_name);
  // #CAT_LayerData find (first) layer data that applies to given layer name -- make it if it doesn't exist 

  virtual LayerDataEl* FindLayerData(const String& chn_name, const String& lay_name);
  // #CAT_LayerData find (first) layer data that applies to given data channel name and layer name
  virtual LayerDataEl* FindMakeLayerData(const String& chn_name, const String& lay_name);
  // #CAT_LayerData find (first) layer data that applies to given data channel name and layer name -- make it if it doesn't exist

  TA_BASEFUNS(LayerDataEl_List);
private:
  void	Initialize() { SetBaseType(&TA_LayerDataEl); }
  void 	Destroy() {}
};

//////////////////////////////////////////////
//		Layer Writer

class PDP_API LayerWriterEl : public LayerDataEl {
  // controls the writing of input data from a data source to a network layer
INHERITED(LayerDataEl)
public: 
  bool		use_layer_type; // #DEF_true use layer_type information on the layer to determine flags to set (if false, turn on EXPERT showing to view flags)
  Unit::ExtType	ext_flags;	// #EXPERT #CONDEDIT_OFF_use_layer_type:true how to flag the unit/layer's external input status
  Random	noise;		// #EXPERT noise optionally added to values when applied
  String_Array  value_names;	// #EXPERT display names of the individual pattern values

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

class PDP_API LayerWriter : public taNBase {
  // ##CAT_Network controls the writing of input data from a data source to network layers
INHERITED(taNBase)
public:
  DataBlockRef	data;
  // the data object with input data to present to the network
  NetworkRef	network;
  // the network to present the input data to
  LayerDataEl_List	layer_data;
  // the layers/input data channel mappings to present to the network

  virtual void 	SetDataNetwork(DataBlock* db, Network* net);
  // #CAT_LayerWriter set the data table and network pointers -- convenience function for programs 

  virtual void	AutoConfig(bool reset_existing = true);
  // #MENU_ON_Actions #MENU_CONTEXT #BUTTON #MENU_SEP_BEFORE #CAT_LayerWriter do a 'best guess' configuration of items by matching up like-named data Channels and network Layers

  virtual bool	ApplyInputData();
  // #CAT_LayerWriter apply data to the layers, using the network's current context settings (TEST,TRAIN,etc) -- returns success

  String	GetDisplayName() const;

  void	UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(LayerWriter);
protected:
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void CheckChildConfig_impl(bool quiet, bool& rval);
    
private:
  void	Initialize();
  void 	Destroy() {}
};


//////////////////////////////////////////////
//		Layer Reader

/*TODO
class PDP_API LayerReader: public LayerDataEl {
  // object that reads data from a Layer to a DataSink 
INHERITED(LayerDataEl)
public:

  
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const LayerReader& cp);
  COPY_FUNS(LayerReader, LayerDataEl);
  TA_BASEFUNS(LayerReader);
  
private:
  void	Initialize();
  void 	Destroy();
};


class PDP_API LayerReader_List: public LayerDataEl_List {
  // ##TOKENS #INSTANCE list of LayerReader objects
INHERITED(LayerDataEl_List)
public:
  TA_BASEFUNS(LayerReader_List); //

protected:
  override void		AutoConfig_impl(DataBlock* db, Network* net,
    bool freshen, Layer::LayerType lt);
    
private:
  void	Initialize() {SetBaseType(&TA_LayerReader);}
  void 	Destroy() {}
};
*/

/////////////////////////////////////////////////////////////////
//	Network Monitor: record values from objects!


class PDP_API NetMonItem: public taNBase {
  // #NO_TOKENS ##CAT_Network used for monitoring the value of an object\n(special support for network variables, including Layer, Projection, UnitGroup, Unit)
INHERITED(taNBase)
public:
  enum	NameStyle {		// how to name the datatable columns
    AUTO_NAME,			// auto-generate a name based on the object name etc
    MY_NAME,			// always use my (net monitor item) name; if multiple columns, then add a subscript index for later ones (_1 _2, etc.)
  };

  taSmartRef 		object;		// the network object being monitored
  TypeDef*		object_type;	// #HIDDEN #NO_SAVE just to anchor the memberdef*
  MemberDef*		member_var;	// #TYPE_ON_object_type #NULL_OK member variable to monitor -- you can also just type variable for non-members (r.wt, etc)
  String        	variable;	// Variable on object to monitor.  Can also be a variable on sub-objects (e.g., act on Layer or Network will get all unit activations); r. and s. indicate recv and send connection vals (e.g., r.wt)
  ValType		real_val_type;	 // type of values to create for real-valued monitored data (note: double has more support in the math library)
  NameStyle		name_style;	 // how to name the columns/channels generated from this data?
  int			max_name_len;	 // #DEF_6 #EXPERT maximum length for any name segment

  ChannelSpec_List	val_specs;	// #HIDDEN_TREE #NO_SAVE specs of the values being monitored 
  MemberSpace   	members;	// #IGNORE memberdefs
  voidptr_Array		ptrs;     	// #HIDDEN #NO_SAVE actual ptrs to values
  SimpleMathSpec 	pre_proc_1;	// #EXPERT first step of pre-processing to perform
  SimpleMathSpec 	pre_proc_2;	// #EXPERT second step of pre-processing to perform
  SimpleMathSpec 	pre_proc_3;	// #EXPERT third step of pre-processing to perform
  
  String  	GetObjName(TAPtr obj); 
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

  void  InitLinks();
  void	CutLinks();
  void	UpdateAfterEdit();
  void 	Copy_(const NetMonItem& cp);
  COPY_FUNS(NetMonItem, taNBase);
  TA_BASEFUNS(NetMonItem);//
  
protected:
  int			cell_num; // current cell number, when adding mon vals
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
  override void		SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
					     int dcr, void* op1_, void* op2_);

  ChannelSpec* 		AddScalarChan(const String& valname, ValType vt);
  MatrixChannelSpec* 	AddMatrixChan(const String& valname, ValType vt,
				      const MatrixGeom* geom = NULL);
  // caller resp for somehow setting geom if NULL; clears cell_num
  bool	 		GetMonVal(int i, Variant& rval); // get the value at i, true if exists

  // these are for finding the members and building the stat
  // out of the objects and the variable
  
  // mk_col means add an entirely new column; otherwise it just adds cell to existing matrix
  bool 	ScanObject_InObject(TAPtr obj, String var, bool mk_col = true);
  void	ScanObject_Network(Network* net, String var);
  void	ScanObject_Layer(Layer* lay, String var);
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

private:
  void	Initialize();
  void 	Destroy() {CutLinks();}
};


class PDP_API NetMonItem_List: public taList<NetMonItem> { 
  // ##CAT_Network list of network monitor items
INHERITED(taList<NetMonItem>)
public:

  int	NumListCols() const {return 3;} 
  const KeyString GetListColKey(int col) const;
  String GetColHeading(const KeyString&) const; // header text for the indicated column
  TA_BASEFUNS(NetMonItem_List);
  
private:
  void		Initialize() {SetBaseType(&TA_NetMonItem);}
  void		Destroy() {}
};

class PDP_API NetMonitor: public taNBase {
  // ##TOKENS #NO_UPDATE_AFTER ##CAT_Network monitors values from network (or other) objects and sends them to a data table/sink
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
  // #MENU #MENU_ON_Action #MENU_CONTEXT #BUTTON #CAT_Monitor set the overall network -- will update any sub-objects to corresponding ones on this network
  void		SetDataTable(DataTable* dt);
  // #MENU #MENU_CONTEXT #BUTTON #CAT_Monitor set the data table used
  void		SetDataNetwork(DataTable* dt, Network* net);
  // #MENU #MENU_CONTEXT #BUTTON #CAT_Monitor set both the data table and network -- convenient for programs

  void		AddNetwork(Network* net, const String& variable)
  { AddObject(net, variable);}
  // #MENU #MENU_ON_Action #MENU_SEP_BEFORE #CAT_Monitor monitor a value in the Network or its subobjects
  void		AddLayer(Layer* lay, const String& variable)
  { AddObject(lay, variable);}
  // #MENU #CAT_Monitor monitor a value in the Layer or its subobjects
  void		AddProjection(Projection* prj, const String& variable)
  { AddObject(prj, variable);}
  // #MENU #CAT_Monitor monitor a value in the Projection or its subobjects
  void		AddUnitGroup(Unit_Group* ug, const String& variable)
  { AddObject(ug, variable);}
  // #CAT_Monitor monitor a value in the UnitGroup or its subobjects
  void		AddUnit(Unit* un, const String& variable)
  { AddObject(un, variable);}
  // #CAT_Monitor monitor a value in the Unit or its subobjects
  
  void		AddObject(TAPtr obj, const String& variable);
  // #CAT_Monitor monitor a value in the object or its subobjects
  void 		UpdateMonitors(bool reset_first = false);
  // #MENU #MENU_SEP_BEFORE #CAT_Monitor create or update the channels -- call this during Init.\n if reset_first, then existing data columns are removed first

  void 		GetMonVals();
  // #CAT_Monitor get all the values and store in current row of data table -- call in program to get new data
  void		RemoveMonitors();
  // #IGNORE called by the network to remove the objs from lists
  void		UpdateNetworkPtrs();
  // #IGNORE update pointers to objects within current network

  String	GetDisplayName() const;
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const NetMonitor& cp);
  void	UpdateAfterEdit();
  COPY_FUNS(NetMonitor, taNBase);
  TA_BASEFUNS(NetMonitor);
  
protected:
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void	CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void		Initialize();
  void		Destroy() {CutLinks();}
};

#endif // NETDATA_H
