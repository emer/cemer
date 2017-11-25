// Copyright 2017, Regents of the University of Colorado,
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
#include <DataTableRef>
#include <DataOpEl>
#include <AggregateSpec>
#include <DataSelectEl>
#include <DataColSpec_List>
#include <MemberSpace>
#include <voidptr_Array>
#include <SimpleMathSpec>
#include <float_Matrix>
#include <double_Matrix>

// declare all other types mentioned but not required to include:
class NetMonitor; //
class Network; //
class Layer; //
class Projection; //
class Projection_List; //
class UnitState_cpp; //
class ConState_cpp; //
class NetworkState_cpp; //

eTypeDef_Of(NetMonItem);

class E_API NetMonItem: public taNBase {
  // #STEM_BASE #NO_TOKENS ##CAT_Network used for monitoring the value of an object\n(special support for network variables, including Layer, Projection, UnitGroup, Unit)
INHERITED(taNBase)
public:
  enum	NameStyle {		// how to name the datatable columns
    AUTO_NAME,			// auto-generate a name based on the object name and the variable name, etc
    MY_NAME,			// always use my (net monitor item) name; if multiple columns, then add a subscript index for later ones (_1 _2, etc.)
  };

  enum MonOptions {            // #BITS flags for network monitor
    NMF_NONE            = 0,   // #NO_BIT
    INPUT_LAYERS        = 0x0001, // for Network layer monitors (variable starts with layers.), include layers with layer_type = INPUT
    HIDDEN_LAYERS       = 0x0002, // for Network layer monitors (variable starts with layers.), include layers with layer_type = HIDDEN
    OUTPUT_LAYERS       = 0x0004, // for Network layer monitors (variable starts with layers.), include layers with layer_type = OUTPUT
    TARGET_LAYERS       = 0x0008, // for Network layer monitors (variable starts with layers.), include layers with layer_type = TARGET
    ALL_LAYERS          = INPUT_LAYERS | HIDDEN_LAYERS | OUTPUT_LAYERS | TARGET_LAYERS,
    // #NO_BIT all the layers
    NO_INPUT            = HIDDEN_LAYERS | OUTPUT_LAYERS | TARGET_LAYERS,
    // #NO_BIT all but the input layers
  };

  NetMonitor*         monitor;
  // #HIDDEN #NO_SAVE #NO_COPY this is my net monitor -- owner's owner
  
  bool                off;              // #NO_SAVE_EMPTY set this to not use this netmon item
  bool                error;            // #NO_SAVE #READ_ONLY an error was detected on this item, preventing this item from generating data -- the error message will be shown at check config time
  String              error_msg;        // #NO_SAVE #CONDSHOW_ON_error #READ_ONLY #SHOW if you are seeing this message then there was an error with this monitor and this tells you what the problem is -- you can turn the item off to ignore the error temporarily
  bool                computed;	// if true, this value is computed separately in a program, and this is here just to make a place for it in the output data (note: computation sold separately -- must be performed elsewhere)
  TypeDef*            object_type;	// #CONDSHOW_OFF_computed #TYPE_taOBase type of object to monitor (narrows down the choices when choosing the object)
  taSmartRef          object;		// #CONDSHOW_OFF_computed #TYPE_ON_object_type #PROJ_SCOPE the network object being monitored
  String              variable;	//  #ADD_COMPLETER_SIMPLE #CONDSHOW_OFF_computed Variable on object to monitor.  Can also be a variable on sub-objects (e.g., act on Layer or Network will get all unit activations); r. and s. indicate recv and send connection vals (e.g., r.wt), projections.varname or prjns.varname gets projection-level variables, and layers.varname specifically targets layer-level variables (important for same-name variables on network and layer); can specify vars on particular unit(s) within a layer as 'units[index<-endidx>].varname' or 'units[gpno][index<-endidx>].varname' where the index value is a leaf in the first case and within a given unit group in the second -- in both cases a range of units can be optionally specified -- also 'ungp[gpno]' accesses UnGpState variables, with gpno=-1 for layer-level group, 0..n-1 for specific unit groups (range avail too) -- use user_data.data_name to access user data variables by name
  String              var_label;	// #CONDSHOW_OFF_computed label to use in place of variable in naming the columns/columns generated from this data (if empty, variable is used)
  NameStyle           name_style;	 // #CONDSHOW_OFF_computed how to name the columns/columns generated from this data?
  int                 max_name_len;	 // #DEF_6 maximum length for any name segment
  MonOptions          options;         // #CONDSHOW_OFF_computed misc options for modifying the way that monitors operate

  ValType             val_type;       // #CONDSHOW_ON_computed type of data column to create (only for computed variables)
  bool                matrix;		// #CONDSHOW_ON_computed if true, create a matrix data column (otherwise scalar)
  MatrixGeom          matrix_geom;	// #CONDSHOW_ON_matrix&&computed geometry of matrix to create if a matrix type

  bool                data_agg; 	// #CONDSHOW_ON_computed compute value automatically from a column of data in another data table
  DataTableRef        data_src;	// #CONDSHOW_ON_data_agg source data for data aggregation operation
  DataOpEl            agg_col;	// #CONDSHOW_ON_data_agg #NO_AUTO_NAME column name in data_src data table to get data to aggregate from
  AggregateSpec       agg;		// #CONDSHOW_ON_computed:false||data_agg:true aggregation operation to perform (reduces vector data down to a single scalar value for network variables, and is aggregation to perform for data_agg aggregation)

  bool                 select_rows;	// #CONDSHOW_ON_data_agg whether to select specific rows of data from the data_src data table to operate on
  DataSelectEl         select_spec;	// #CONDSHOW_ON_select_rows #NO_AUTO_NAME optional selection of rows to perform aggregation on according to the value of items in given column -- for more complex selections and/or greater efficiency, use DataSelectRowsProg to create intermediate data table and operate on that

  DataColSpec_List      val_specs;	// #TREE_HIDDEN #HIDDEN #NO_SAVE specs of the values being monitored
  DataColSpec_List      agg_specs;	// #TREE_HIDDEN #HIDDEN #NO_SAVE specs of the agg values -- these are the matrix values whereas the val_specs contain the agg'd scalar values
  MemberSpace           members;	// #IGNORE memberdefs
  voidptr_Array         ptrs;     	// #HIDDEN #NO_SAVE actual ptrs to values

  SimpleMathSpec 	pre_proc_1;	// first step of pre-processing to perform -- applied prior to aggregation operations
  SimpleMathSpec 	pre_proc_2;	// second step of pre-processing to perform -- applied prior to aggregation operations
  SimpleMathSpec 	pre_proc_3;	// third step of pre-processing to perform -- applied prior to aggregation operations

  inline void           SetMonOption(MonOptions flg)
  { options = (MonOptions)(options | flg); }
  // set flag state on
  inline void           ClearMonOption(MonOptions flg)
  { options = (MonOptions)(options & ~flg); }
  // clear flag state (set off)
  inline bool           HasMonOption(MonOptions flg) const { return (options & flg); }
  // check if flag is set
  inline void           SetMonOptionState(MonOptions flg, bool on)
  { if(on) SetMonOption(flg); else ClearMonOption(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)
  
  String        GetAutoName(const String& obj_nm);
  // get auto-name value based on current values
  String        GetObjName(taBase* obj);
  // get name of object for naming monitored values -- uses GetDisplayName by default but is optimized for various network objects; uses max_name_len constraint
  String        GetColName(taBase* obj, int col_idx, String obj_nm = "");
  // #IGNORE get name for given column of data, taking into account namestyle preferences; col_idx is index within the columnspec_list for this guy

  void          SetMonVals(taBase* obj, const String& var);
  // #CAT_Monitor set object and variable, and update appropriately
  virtual void 	GetMonVals(DataTable* db);
  // #CAT_Monitor get the monitor data and stick it in the current row of the datablock/datatable
  void          ResetMonVals();
  // #CAT_Monitor deletes the cached vars

  void          ScanObject();
  // #CAT_Monitor get the monitor data information from the object
  
  void          SetObject(taBase* obj);
  // #DROP1 #DYN1 #TYPE_taNBase set given object to be monitored -- after this then you can looup variable to monitor using lookup_var
  void          ToggleOffFlag();
  // #MENU #MENU_ON_Object #DYN1 toggle the off flag to opposite of current state: flag indicates whether monitor should be recorded now or not

  bool   MonError(bool test, const String& fun_name,
                  const String& a, const String& b=NULLStr, const String& c=NULLStr,
                  const String& d=NULLStr, const String& e=NULLStr, const String& f=NULLStr,
                  const String& g=NULLStr, const String& h=NULLStr);
  // set a monitor error if test is true
  
  static const KeyString key_obj_name; // #IGNORE
  static const KeyString key_obj_type; // #IGNORE
  static const KeyString key_obj_var; // #IGNORE
  String GetColText(const KeyString& key, int itm_idx = -1) const override;

  void           CollectAllSpecs(NetMonitor* mon);
  // #IGNORE collect all the specs to parent monitor

  void           GetMemberCompletionList(const MemberDef* md, Completions& completions) override;

  int	GetEnabled() const override {return (off) ? 0 : 1;}
  void	SetEnabled(bool value) override {off = !value;}
  void  InitLinks() override;
  void	CutLinks() override;
  void 	Copy_(const NetMonItem& cp);
  TA_BASEFUNS(NetMonItem);//
  
protected:
  float_Matrix		agg_tmp_calc; // temp calc matrix for agg data
  float_Matrix		agg_tmp_calc_2; // temp calc matrix for agg data
  double_Matrix		agg_tmp_calc_d; // temp calc matrix for agg data

  void            UpdateAfterEdit_impl() override;
  void            UpdateAfterMove_impl(taBase* old_owner) override;
  void            UpdatePointersAfterCopy_impl(const taBase& cp) override;

  int             cell_num; // current cell number, when adding mon vals
  void            CheckThisConfig_impl(bool quiet, bool& rval) override;
  void            SmartRef_SigDestroying(taSmartRef* ref, taBase* obj) override;
  void            SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
                                 int sls, void* op1_, void* op2_) override;

  DataColSpec* 		AddScalarCol(const String& valname, ValType vt);
  DataColSpec* 		AddScalarCol_Agg(const String& valname, ValType vt);
  // add to the agg_specs just to keep it consistent
  DataColSpec* 	  AddMatrixCol(const String& valname, ValType vt,
                                     const MatrixGeom* geom = NULL);
  // caller resp for somehow setting geom if NULL; clears cell_num
  bool	 		GetMonVal(int i, Variant& rval); // #IGNORE get the value at i, true if exists
  void 			GetMonVals_Agg(DataTable* db);
  // #IGNORE special version for agg case
  void 			GetMonVals_DataAgg(DataTable* db);
  // #IGNORE special version for data_agg case

  // these are for finding the members and building the stat
  // out of the objects and the variable
  
  bool 	ScanObject_InObject(taBase* obj, String var, taBase* name_obj, bool err_not_found = true);
  // #IGNORE if name_obj == NULL, don't make a column for this guy
  bool 	ScanObject_InNonTAObject(void* obj, TypeDef* typ, String var, taBase* name_obj,
                                 bool err_not_found = true);
  // #IGNORE if name_obj == NULL, don't make a column for this guy
  bool ScanObject_InUserData(taBase* obj, String var, taBase* name_obj);
  // #IGNORE called when an InObject var is "user_data.xxx[.yyy]"
  void	ScanObject_Network(Network* net, String var); // #IGNORE
  void	ScanObject_Network_Layers(Network* net, String var); // #IGNORE
  void	ScanObject_Layer(Layer* lay, String var); // #IGNORE
  void	ScanObject_LayerUnits(Layer* lay, String var);
  // #IGNORE specific subrange of units within a layer
  void	ScanObject_LayerUnGp(Layer* lay, String var);
  // #IGNORE unit group variables -- ungp[x].y
  void	ScanObject_Projection(Projection* p, String var); // #IGNORE
  void	ScanObject_ProjectionList(Projection_List* p, String var); // #IGNORE
  void	ScanObject_Unit(UnitState_cpp* u, String var, String obj_nm, NetworkState_cpp* net);
  // #IGNORE this is only when the object itself is a unit
  bool	CheckVarOnUnit(String var, Network* net);
  // #IGNORE check if the variable is a valid variable on units (including r. / s. con vars)

  // these are only for r. and s. con variables
  void	ScanObject_LayerCons(Layer* lay, String var); // #IGNORE
  void	ScanObject_PrjnCons(Projection* p, String var);
  // #IGNORE known to be a connection variable (r.x or s.x); matrix already allocated

  void	ScanObject_RecvCons(ConState_cpp* cg, String var, String obj_nm, NetworkState_cpp* net);
  // #IGNORE
  void	ScanObject_SendCons(ConState_cpp* cg, String var, String obj_nm, NetworkState_cpp* net);
  // #IGNORE

private:
  void	Initialize();
  void 	Destroy() {CutLinks();}
};

#endif // NetMonItem_h
