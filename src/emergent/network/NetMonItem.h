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

#ifndef NetMonItem_h
#define NetMonItem_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:
#include <taSmartRef>
#include <MatrixGeom>
#include <DataTable>
#include <DataOpEl>
#include <AggregateSpec>
#include <DataSelectEl>
#include <ChannelSpec_List>
#include <MemberSpace>
#include <voidptr_Array>
#include <SimpleMathSpec>
#include <float_Matrix>
#include <double_Matrix>

// declare all other types mentioned but not required to include:
class MatrixChannelSpec; //
class Network; //
class Layer; //
class Projection; //
class Projection_Group; //
class Unit; //
class Unit_Group; //
class SendCons; //
class RecvCons; //

eTypeDef_Of(NetMonItem);

class E_API NetMonItem: public taNBase {
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
  override void		SmartRef_SigDestroying(taSmartRef* ref, taBase* obj);
  override void		SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
					     int sls, void* op1_, void* op2_);

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

#endif // NetMonItem_h
