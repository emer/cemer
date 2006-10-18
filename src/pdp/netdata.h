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

class PDP_API LayerRWBase: public taOBase  {
  // #VIRT_BASE #NO_INSTANCE #NO_TOKENS controls the reading/writing of information to/from layers and data blocks/tables
friend class LayerRWBase_List;
friend class LayerWriter_List;
INHERITED(taOBase)
public:
  enum NetTarget {
    LAYER,			// read/write the layer information
    TRIAL_NAME,			// read/write the network trial_name field
    GROUP_NAME,			// read/write the network group_name field
  };

  DataBlockRef		data; // #AKA_data_block source or sink of the data to apply to layer, or store layer output (as appropriate)
  String		chan_name; // name of the channel/column in the data table use 
  NetTarget		net_target;  // what to read/write from on the network
  NetworkRef 		network;  // the network to operate on
  LayerRef 		layer;	// #CONDEDIT_ON_net_target:LAYER the Layer that will get read or written
  PosTwoDCoord		offset;	// #EXPERT offset in layer or unit group at which to start reading/writing
  
  override String	GetDisplayName() const;

  void  UpdateAfterEdit();
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const LayerRWBase& cp); 
  COPY_FUNS(LayerRWBase, taOBase);
  TA_BASEFUNS(LayerRWBase);
  
protected:
  int			chan_idx; // cached col, -1 = not looked up, or invalid
  
  override void 	CheckConfig_impl(bool quiet, bool& rval);
  int			GetChanIdx(bool force_lookup = false); // recalcs if needed
private:
  void	Initialize();
  void 	Destroy();
};


class PDP_API LayerRWBase_List: public taList<LayerRWBase> {
  // #VIRT_BASE #NO_INSTANCE list of individual LayerRWBase objects
INHERITED(taList<LayerRWBase>)
public:
  virtual LayerRWBase*	FindByDataBlockLayer(DataBlock* db, Layer* lay);
// find the item by source and target -- note: finds first if multiple the same

  virtual void	FillFromDataBlock(DataBlock* db, Network* net, bool freshen_only);
  // #MENU_ON_Data #MENU #MENU_CONTEXT #BUTTON #MENU_SEP_BEFORE do a 'best guess' fill of items by matching up like-named Channels and Layers
  virtual void	FillFromTable(DataTable* dt, Network* net, bool freshen_only);
  // #MENU #MENU_CONTEXT #BUTTON do a 'best guess' fill of items by matching up like-named Columns and Layers

  virtual void	SetAllData(DataBlock* db);
  // #MENU #MENU_CONTEXT #BUTTON set all the data pointers for list elements to given datablock/data table
  virtual void	SetAllNetwork(Network* net);
  // #MENU #MENU_CONTEXT #BUTTON set all the network pointers for list elements
  virtual void	SetAllDataNetwork(DataBlock* db, Network* net);
  // set all the data and network pointers for list elements

  TA_ABSTRACT_BASEFUNS(LayerRWBase_List);
protected:
  virtual void		FillFromDataBlock_impl(DataBlock* db, Network* net,
    bool freshen, Layer::LayerType lt){}
    
private:
  void	Initialize() { SetBaseType(&TA_LayerRWBase); }
  void 	Destroy() {}
};


class PDP_API LayerWriter: public LayerRWBase {
  // object that writes data from a DataSource to a layer
INHERITED(LayerRWBase)
public: 
  bool		use_layer_type; // #DEF_true use layer_type information on the layer to determine flags to set (if false, turn on EXPERT showing to view flags)
  Unit::ExtType	ext_flags;	// #EXPERT #CONDEDIT_OFF_use_layer_type:true how to flag the unit/layer's external input status
  Random	noise;		// #EXPERT noise optionally added to values when applied
  String_Array  value_names;	// #EXPERT display names of the individual pattern values
  // todo: not sure if above belongs here..

  virtual void 		ApplyExternal(int context);
  // apply data to the layers, using the supplied context (TEST, TRAIN, etc)

  override String	GetDisplayName() const;

  void	UpdateAfterEdit();
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const LayerWriter& cp);
  COPY_FUNS(LayerWriter, LayerRWBase);
  TA_BASEFUNS(LayerWriter); //
  
private:
  void	Initialize();
  void 	Destroy();
};

class PDP_API LayerWriter_List: public LayerRWBase_List {
  // ##TOKENS #INSTANCE list of individual LayerWriter objects
INHERITED(LayerRWBase_List)
public:
  
  inline LayerWriter*	FastEl(int i) {return (LayerWriter*)FastEl_(i);}
  virtual void		ApplyExternal(int context);
  // apply data to the layers, using the supplied context (TEST, TRAIN, etc)

  TA_BASEFUNS(LayerWriter_List);
protected:
  override void		FillFromDataBlock_impl(DataBlock* db, Network* net,
    bool freshen, Layer::LayerType lt);
    
private:
  void	Initialize() {SetBaseType(&TA_LayerWriter);}
  void 	Destroy() {}
};

/*TODO
class PDP_API LayerReader: public LayerRWBase {
  // object that reads data from a Layer to a DataSink 
INHERITED(LayerRWBase)
public:

  
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const LayerReader& cp);
  COPY_FUNS(LayerReader, LayerRWBase);
  TA_BASEFUNS(LayerReader);
  
private:
  void	Initialize();
  void 	Destroy();
};


class PDP_API LayerReader_List: public LayerRWBase_List {
  // ##TOKENS #INSTANCE list of LayerReader objects
INHERITED(LayerRWBase_List)
public:
  TA_BASEFUNS(LayerReader_List); //

protected:
  override void		FillFromDataBlock_impl(DataBlock* db, Network* net,
    bool freshen, Layer::LayerType lt);
    
private:
  void	Initialize() {SetBaseType(&TA_LayerReader);}
  void 	Destroy() {}
};
*/

class PDP_API NetMonItem: public taNBase {
  // #NO_TOKENS used for monitoring the value of an object\n(special support for network variables, including Layer, Projection, UnitGroup, Unit)
INHERITED(taNBase)
public:
  static const String 	GetObjName(TAPtr obj, TAPtr own = NULL); 
   // get name of object for naming stats, etc. qualifies up to Layer; looks up own if NULL
  static const String	DotCat(const String& lhs, const String& rhs); 
    // cat with . except if either empty, just return the other
  
  taSmartRef 		object;		// the network object being monitored
  TypeDef*		object_type;	// #HIDDEN #NO_SAVE just to anchor the memberdef*
  MemberDef*		member_var;	// #TYPE_ON_object_type #NULL_OK member variable to monitor -- you can also just type variable for non-members (r.wt, etc)
  String        	variable;	// Variable on object to monitor.  Can also be a variable on sub-objects (e.g., act on Layer or Network will get all unit activations); r. and s. indicate recv and send connection vals (e.g., r.wt)

  ChannelSpec_List	val_specs;	// #HIDDEN_TREE #NO_SAVE specs of the values being monitored 
  MemberSpace   	members;	// #IGNORE memberdefs
  taBase_List		ptrs;     	// #HIDDEN #NO_SAVE actual ptrs to values
  SimpleMathSpec 	pre_proc_1;	// #EXPERT first step of pre-processing to perform
  SimpleMathSpec 	pre_proc_2;	// #EXPERT second step of pre-processing to perform
  SimpleMathSpec 	pre_proc_3;	// #EXPERT third step of pre-processing to perform
  

  void		SetMonVals(TAPtr obj, const String& var); 
    // set object and variable, and update appropriately
//TODO: add funcs for specific object types, and put in gui directives

  void		ScanObject();	// #IGNORE update the schema
  virtual void 	GetMonVals(DataBlock* db);
  // get the monitor data and stick it in the current row of the datablock/datatable
  void		ResetMonVals(); // deletes the cached vars

  
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
  override void		CheckConfig_impl(bool quiet, bool& rval);
  override void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
  override void		SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
    int dcr, void* op1_, void* op2_);

  ChannelSpec* 		AddScalarChan(const String& valname, ValType val_type);
  MatrixChannelSpec* 	AddMatrixChan(const String& valname, ValType val_type,
    const MatrixGeom* geom = NULL);
    // caller resp for somehow setting geom if NULL; clears cell_num
  bool 			AddCellName(const String& cellname);
  bool	 		GetMonVal(int i, Variant& rval); // get the value at i, true if exists
  // these are for finding the members and building the stat
  // out of the objects and the variable
  
/*  void 			ScanObject_InObject(TAPtr obj, String var);
  void			ScanObject_IterLayer(Layer* lay, String var); // #IGNORE
  void			ScanObject_IterUnitGroup(Unit_Group* ug, String var);	// #IGNORE
  void			ScanObject_IterGroup(taGroup_impl* gp, String var);	// #IGNORE
  void			ScanObject_IterList(taList_impl* list, String var);	// #IGNORE
*/  
  
  bool 			ScanObject_InObject(TAPtr obj, String var, 
    bool mk_col = false, TAPtr own = NULL);
  void			ScanObject_Network(Network* net, String var); // #IGNORE
  void			ScanObject_Layer(Layer* lay, String var); // #IGNORE
  void			ScanObject_Projection(Projection* p, String var); // #IGNORE
  void			ScanObject_UnitGroup(Unit_Group* ug, String var, 
    bool mk_col = false);	// #IGNORE
  void			ScanObject_Unit(Unit* u, String var,
    Projection* p = NULL, bool mk_col = false); // #IGNORE
  void			ScanObject_ConGroup(Con_Group* cg, String var,
    Projection* p = NULL); // #IGNORE note: always makes a col
  
// following return 'true' if the routine handled making the colspecs

private:
  void	Initialize();
  void 	Destroy() {CutLinks();}
};


class PDP_API NetMonItem_List: public taList<NetMonItem> { 
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

class PDP_API NetMonitor: public taNBase { // ##TOKENS #NO_UPDATE_AFTER used for monitoring values of network objects
INHERITED(taNBase)
public:
  NetMonItem_List	items; // the list of items being monitored
  DataTableRef		data; // the data table that will be used to hold the monitor data
  bool			rmv_orphan_cols; // #DEF_true remove orphan columns when updating table schema

  void		SetDataTable(DataTable* dt); // #MENU #MENU_ON_Action set the data table used
  void		AddNetwork(Network* net, const String& variable)
    {AddObject(net, variable);}
    // #MENU #MENU_ON_Action #MENU_SEP_BEFORE monitor a value in the Network or its subobjects
  void		AddLayer(Layer* lay, const String& variable)
    {AddObject(lay, variable);}
    // #MENU monitor a value in the Layer or its subobjects
  void		AddProjection(Projection* prj, const String& variable)
    {AddObject(prj, variable);}
    // #MENU monitor a value in the Projection or its subobjects
  void		AddUnitGroup(Unit_Group* ug, const String& variable)
    {AddObject(ug, variable);}
    // monitor a value in the UnitGroup or its subobjects
  void		AddUnit(Unit* un, const String& variable)
    {AddObject(un, variable);}
    // monitor a value in the Unit or its subobjects
  
  void		AddObject(TAPtr obj, const String& variable);
    // monitor a value in the object or its subobjects
  void 		UpdateMonitors(bool reset_first = false);
  // #MENU #MENU_SEP_BEFORE create or update the channels -- call this during Init.\n  if reset_first, then existing data columns are removed first
  void 		GetMonVals();
  // get all the values and store in current row of data table -- call in program to get new data
  
  void		RemoveMonitors();
  // #IGNORE called by the network to remove the objs from lists

  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const NetMonitor& cp);
  void	UpdateAfterEdit();
  COPY_FUNS(NetMonitor, taNBase);
  TA_BASEFUNS(NetMonitor);
  
protected:
  override void	CheckConfig_impl(bool quiet, bool& rval);
  override void	CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void		Initialize();
  void		Destroy() {CutLinks();}
};

#endif // NETDATA_H
