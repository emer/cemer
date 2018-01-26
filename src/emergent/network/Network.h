// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef Network_h
#define Network_h 1

// parent includes:
#include <taFBase>

#include "network_def.h"

// member includes:
#include <NetworkRef>
#include <BaseSpec_Group>
#include <Layer_Group>
#include <taBrainAtlas_List>
#include <PRerrVals>
#include <Weights_List>
#include <TimeUsed>
#include <int_Array>
#include <float_Matrix>
#include <DMemComm>
#include <DMemAggVars>
#include <DataTable_Group>
#include <ParamSeq_Group>
#include <taMarkUp>
#include <taBase_RefList>
#include <byte_Array>
#include <NetMonitor>
#include <DataTable>
#include <T3DataView_List>

// declare all other types mentioned but not required to include:
class ProjectBase; //
class NetView; //
class BrainView; //
class T3Panel; //
class ProjectionSpec; //

#ifdef CUDA_COMPILE
class NetworkState_cuda; //
#endif  


////////////////////////////////////////////////////////
//      Memory structure for version 8.0.0 
//
// The network now owns and manages all the unit state and the connections
// which are allocated into separate memory structures organized by thread.
//
// The Unit has been split into the structural aspects (name, position, etc) 
// which remain in the Unit class, and UnitState
// UnitState contains the algorithm-specific variables -- this must
// contain exclusively floats in a flat list of members -- no subclasses, etc
// it also now directly contains the bias variables -- no point in all the overhead 
// to deal with those separately -- connection code should operate directly on the
// float& variables, so it doesn't care if these are in a connection structure or not

// There can only be one type of UnitState used within the network -- allocation is
// fully vector based into a big pool of memory per thread.  Computation should
// now also be fully vector-based over units (and connections) where possible
//
// send, recv connection structures are all allocated in separate memory
// not within the Unit structure at all -- methods are avail to access
//
// the ConState (formerly BaseCons) object is now just a data structure -- not a taBase
//
// There are 3 levels of memory allocation, all done at the thread level:
// * UnitState -- array[n_units+1]
// * ConStates -- one for recv, the other for send -- each in a flat array
// * Connections -- each thread has its own chunk and indexes into it..


// on functions in the spec:
// only those functions that relate to the computational processing done by
// a given object should be defined in the spec, while the "structural" or
// other "administrative" functions should be defined in the object itself
// so that the spec can be invariant with respect to these kinds of differences
// with the idea being that the spec defines _functional_ aspects while
// the object defines various implementational aspects of an object's function
// (i.e. how the connections are arranged, etc..)

// note that with the projection spec, its job is to define connectivity, so
// it does have structural functions in the spec..

eTypeDef_Of(NetworkState_cpp);
eTypeDef_Of(LayerState_cpp);
eTypeDef_Of(PrjnState_cpp);
eTypeDef_Of(UnGpState_cpp);
eTypeDef_Of(UnitState_cpp);
eTypeDef_Of(ConState_cpp);

eTypeDef_Of(ConSpec_cpp);
eTypeDef_Of(UnitSpec_cpp);
eTypeDef_Of(LayerSpec_cpp);

// full standalone C++ implementation State code -- we build these structures
#include <NetworkState_cpp>
#include <UnGpState_cpp>
#include <UnitSpec_cpp>

// re-establish main context
#include <State_main>

eTypeDef_Of(NetStatsSpecs);
eTypeDef_Of(NetworkCudaSpec);

// this defines shared member state variables with NetworkState, in main context
#include <Network_mbrs>

// this macro can be used instead of NET_THREAD_CALL to just loop directly
// over a thread-level method call -- for cases that are not thread safe
#define NET_THREAD_LOOP(meth) { NetworkThreadCall meth_call((NetworkThreadMethod)(&meth));\
    for(int thr_no=0; thr_no < n_thrs_built; thr_no++) \
      meth_call.call(this, thr_no); }


// use this for calling all major State compute code -- runs on proper device!
#ifdef CUDA_COMPILE
#define NET_STATE_RUN(typ, meth) \
  if(cuda.on) \
    ((CLASS_SUFFIXED(typ,_cuda)*)cuda_state)->meth;\
  else \
    ((CLASS_SUFFIXED(typ,_cpp)*)net_state)->meth
#else
#define NET_STATE_RUN(typ, meth) ((CLASS_SUFFIXED(typ,_cpp)*)net_state)->meth
#endif


eTypeDef_Of(NetNetMonitor);

class E_API NetNetMonitor: public NetMonitor {
  // ##CHOOSE_AS_MEMBER special network monitor for network itself -- hides network, data members
INHERITED(NetMonitor)
public:

#ifdef __MAKETA__  
  NetworkRef            network;
  // #HIDDEN the overall network object that is being monitored -- if changed, any sub-objects will be updated based on path to new network
  DataTableRef          data;
  // #HIDDEN the data table that will be used to hold the monitor data
#endif
  
  void    ItemsUpdated() override;
  // #CAT_Display an item was added or removed from list of monitored items

  TA_BASEFUNS_NOCOPY(NetNetMonitor);
private:
  void Initialize() { };
  void Destroy() { };
};


eTypeDef_Of(NetStateSync);

class E_API NetStateSync : public taOBase {
  // ##NO_TOKENS ##CAT_Network state synchronization variables
INHERITED(taOBase)
public:
  bool          main_to_state;  // copy main to state -- otherwise state to main
  int           main_off;       // net offset for address of main item
  int           state_off;      // net offset for address of state item
  int           size;           // number of bytes
  MemberDef*    main_md;        // member def for main item
  MemberDef*    state_md;       // member def for state item

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(NetStateSync);
private:
  void  Initialize()    { };
  void  Destroy()       { };
};

eTypeDef_Of(NetStateSync_List);

class E_API NetStateSync_List : public taList<NetStateSync> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##NO_EXPAND List of NetStateSync objects
INHERITED(taList<NetStateSync>)
public:

  virtual void  ParseTypes_impl(TypeDef* main_td, TypeDef* state_td, int main_off, int state_off,
                                bool main_to_st);
  // #IGNORE parse main and state types to get state info
  virtual void  ParseTypes(TypeDef* main_td, TypeDef* state_td);
  // parse main and state types to get state info
  virtual void  DoSync(void* main_addr, void* state_addr);
  // do the sync between two objects
  
  TA_BASEFUNS_NOCOPY(NetStateSync_List);
private:
  void Initialize()  { SetBaseType(&TA_NetStateSync); };
  void Destroy()     { };
};

eTypeDef_Of(Network);

class E_API Network : public taFBase {
  // ##FILETYPE_Network ##EXT_net ##COMPRESS ##CAT_Network ##DEF_NAME_ROOT_Network ##EXPAND_NAV A network, containing layers, units, etc..
INHERITED(taFBase)
public:
  static bool nw_itm_def_arg;   // #IGNORE default arg val for FindMake..

  enum WtSaveFormat {
    TEXT,                       // weights are saved as ascii text representation of digits (completely portable)
    BINARY,                     // weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
    NET_FMT,                    // use format specified on the network
  };

  enum AutoBuildMode {
    AUTO_BUILD,                 // automatically build the network after loading
    PROMPT_BUILD,               // prompt about building after loading (if run in -nogui mode, it is automatically built without prompting)
    NO_BUILD,                   // do not build network after loading
  };

  enum AutoLoadMode {
    NO_AUTO_LOAD,               // Do not automatically load a weights file
    AUTO_LOAD_WTS_0,            // Automatically load weights from the first weights saved weights entry -- that should have the save_with_proj or auto_load flags set
    AUTO_LOAD_FILE,             // Automatically load a weights file named in auto_load_file after loading the project.
  };

  enum NetTextLoc {
    NT_BOTTOM,                  // standard bottom location below network -- extends network "foot" lower below to make text more visible
    NT_TOP_BACK,                // above top-most layer, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_LEFT_BACK,               // at left of network, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_RIGHT_BACK,              // at right of network, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_LEFT_MID,                // at left of network, in the middle depth-wise -- foot is raised as when no net text is visible
    NT_RIGHT_MID,               // at right of network, in the middle depth-wise -- foot is raised as when no net text is visible
  };

  static taBrainAtlas_List* brain_atlases;  // #HIDDEN #READ_ONLY #NO_SAVE #TREE_HIDDEN atlases available

  NetworkState_cpp*  net_state; // #HIDDEN #NO_SAVE our C++ network state -- handles full implementation
#ifdef CUDA_COMPILE
  NetworkState_cuda* cuda_state; // #HIDDEN #NO_SAVE our NVIDIA CUDA network state -- this obj lives in c++ but its state pointers point into allocated device memory  -- it also has a copy of itself that is entirely within device memory
#endif
  

// this directly defines a bunch of shared vars with NetworkState
#include <Network_core>
 
  DataTable_Group spec_tables;  // #CAT_Structure #NO_SAVE Tables comparing parent and child specs
  BaseSpec_Group specs;         // #CAT_Structure Specifications for network parameters
  ParamSeq_Group param_seqs;    // #CAT_Structure parameter sequences keyed off of epoch -- supports automatic arbitrary parameter changes whenver the network epoch is incremented
  NetNetMonitor monitor;        // #CAT_Statistic #TREE_SHOW special monitor automatically run at the finest grain of network updating (e.g., trial for backprop, cycle for Leabra) -- values can be viewed in the network state text in network view (e.g., for monitoring key activations during processing)
  DataTable     mon_data;       // #CAT_Statistic #TREE_SHOW data for the monitor -- only single current row is kept at a time -- use a standard monitor program to accumulate data over longer time periods
  Layer_Group   layers;         // #CAT_Structure Layers or Groups of Layers
  Weights_List  weights;        // #CAT_Structure saved weights objects

  AutoBuildMode auto_build;     // #CAT_Structure whether to automatically build the network (make units and connections) after loading or not
  AutoLoadMode  auto_load_wts;
  // #CONDEDIT_OFF_auto_build:NO_BUILD Whether to automatically load a weights file when the Network object is loaded.  It is not possible to save the units, so this can be used to provide pre-configured network for the user (must auto_build network first)
  String        auto_load_file;
  // #CONDSHOW_ON_auto_load_wts:AUTO_LOAD_FILE #FILE_DIALOG_LOAD #COMPRESS #FILETYPE_Weights #EXT_wts file name to auto-load weights file from (any path must be relative to project file)

  taBrainAtlasRef brain_atlas;  // #FROM_LIST_brain_atlases #NO_SAVE The name of the atlas to use for brain view rendering.  Labels from this atlas can be applied to layers' brain_area member.
  String        brain_atlas_name; // #HIDDEN the name of the brain atlas that we're using -- this is what is actually saved b/c the ref is not saveable

  int           n_threads;      // #NO_SAVE #CAT_State number of CPU threads to use -- defaults to value in preferences, but can be overridden.  is copied to net_state->threads.n_threads which is actual thread impl
  NetworkCudaSpec  cuda;        // #CAT_CUDA parameters for NVIDA CUDA GPU implementation -- only applicable for CUDA_COMPILE binaries

  String        group_name;     // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW #VIEW_WIDTH_12 name associated with the current group of trials, if such a grouping is applicable (typically set by a LayerWriter)
  String        trial_name;     // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW #VIEW_WIDTH_12 name associated with the current trial (e.g., name of input pattern, typically set by a LayerWriter)
  String        output_name;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW #VIEW_WIDTH_12 name for the output produced by the network (must be computed by a program)

  TimeUsed      train_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing entire training (across epochs) (managed entirely by programs -- not always used)
  TimeUsed      epoch_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing an epoch (managed entirely by programs -- not always used)
  TimeUsed      group_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a group, when groups used (managed entirely by programs -- not always used)
  TimeUsed      trial_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a trial (managed entirely by programs -- not always used)
  TimeUsed      settle_time;    // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a settling (managed entirely by programs -- not always used)
  TimeUsed      cycle_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a cycle (managed entirely by programs -- not always used)
  TimeUsed      wt_sync_time;   // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for the DMem_SumDWts operation (trial-level dmem, computed by network)
  TimeUsed      misc_time;      // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic misc timer for ad-hoc use by programs

  WtSaveFormat  wt_save_fmt;    // #CAT_File format to save weights in if saving weights

  PosVector3i   max_disp_size;  // #AKA_max_size #READ_ONLY #EXPERT #CAT_Structure maximum display size in each dimension of the net
  PosVector2i   max_disp_size2d; // #READ_ONLY #EXPERT #CAT_Structure maximum display size in each dimension of the net -- for 2D display

  //////////////////////////////////////////////////////////////////////////////////
  // Following are used to cache the built structure encoded in state representations

  taBase_RefList state_layers;
  // #NO_SAVE #HIDDEN #CAT_State layers that have been built for running network
  taBase_RefList state_prjns;
  // #NO_SAVE #HIDDEN #CAT_State prjns that have been built for running network
  NetStateSync_List net_state_sync;
  // #NO_SAVE #HIDDEN #CAT_State handles optimized state sync for network object
  NetStateSync_List layer_state_sync;
  // #NO_SAVE #HIDDEN #CAT_State handles optimized state sync for layer object
  NetStateSync_List prjn_state_sync;
  // #NO_SAVE #HIDDEN #CAT_State handles optimized state sync for projection object

  taBase_RefList state_layer_specs;
  // #NO_SAVE #HIDDEN #CAT_State layer_specs that have been built for running network
  taBase_RefList state_prjn_specs;
  // #NO_SAVE #HIDDEN #CAT_State prjn_specs that have been built for running network
  taBase_RefList state_unit_specs;
  // #NO_SAVE #HIDDEN #CAT_State unit_specs that have been built for running network
  taBase_RefList state_con_specs;
  // #NO_SAVE #HIDDEN #CAT_State con_specs that have been built for running network

  byte_Array     hash_value;
  // #NO_SAVE #HIDDEN #CAT_State unique hash code value of network including the indexes, sizes, connectivity, and optionally weights -- used to guarantee identical state of networks across dmem / mpi for example
  String_Array  net_timer_names;
  // #NO_SAVE #HIDDEN #CAT_State names of the timers -- for generating report -- overloaded must add as appropriate

  ProjectBase*  proj;           // #IGNORE ProjectBase this network is in
  
  virtual void    ClearIntact();
  // call this when any element in the network is updated such that the current built status is no longer valid 

  inline Layer* StateLayer(int idx) const { return (Layer*)state_layers[idx]; }
  // #CAT_State layer at given index in list of state objects built in network state for running
  inline Projection* StatePrjn(int idx) const { return (Projection*)state_prjns[idx]; }
  // #CAT_State prjn at given index in list of state objects built in network state for running

  inline LayerSpec* StateLayerSpec(int idx) const { return (LayerSpec*)state_layer_specs[idx]; }
  // #CAT_State layer_spec at given index in list of state objects built in network state for running
  inline ProjectionSpec* StatePrjnSpec(int idx) const { return (ProjectionSpec*)state_prjn_specs[idx]; }
  // #CAT_State prjn_spec at given index in list of state objects built in network state for running
  inline UnitSpec* StateUnitSpec(int idx) const { return (UnitSpec*)state_unit_specs[idx]; }
  // #CAT_State unit_spec at given index in list of state objects built in network state for running
  inline ConSpec* StateConSpec(int idx) const { return (ConSpec*)state_con_specs[idx]; }
  // #CAT_State con_spec at given index in list of state objects built in network state for running

  inline Layer* LayerFromState(LayerState_cpp* state) const
  { return StateLayer(state->layer_idx); }
  // #CAT_State get layer corresponding to given layer state
  inline Projection* PrjnFromState(PrjnState_cpp* state) const
  { return StatePrjn(state->prjn_idx); }
  // #CAT_State get projection corresponding to given prjn state
  
  inline UnitSpec* UnitSpecFromState(UnitSpec_cpp* state) const
  { return StateUnitSpec(state->spec_idx); }
  // #CAT_State get unit spec corresponding to given state
  inline ConSpec* ConSpecFromState(ConSpec_cpp* state) const
  { return StateConSpec(state->spec_idx); }
  // #CAT_State get con spec corresponding to given state

  virtual NetworkState_cpp* NewNetworkState() const;
  // #IGNORE each type of Network MUST override this to create a new State of correct type 

#ifdef CUDA_COMPILE
  virtual NetworkState_cuda* NewCudaState() const;
  // #IGNORE each type of Network MUST override this to create a new State of correct type
#endif  

  virtual String  GetUnitStatePath(UnitState_cpp* unit);
  // #CAT_State get a path to unit state (owning layer plus index) -- can be decoded by GetUnitStateFromPath
  virtual UnitState_cpp*  GetUnitStateFromPath(const String& path);
  // #CAT_State get unit state from path (owning layer plus index) -- generated by GetUnitStatePath
  
  virtual TypeDef*      NetworkStateType() const;
  // each type of Network MUST override this to match type of state it uses
  virtual TypeDef*      LayerStateType() const;
  // each type of Network MUST override this to match type of state it uses
  virtual TypeDef*      PrjnStateType() const;
  // each type of Network MUST override this to match type of state it uses
  virtual TypeDef*      UnGpStateType() const;
  // each type of Network MUST override this to match type of state it uses
  virtual TypeDef*      UnitStateType() const;
  // each type of Network MUST override this to match type of state it uses
  virtual TypeDef*      ConStateType() const;
  // each type of Network MUST override this to match type of state it uses
  
  /////////////////////////////////////////////////////////
  //  State access -- these all refer directly to NetworkState_cpp versions

  inline bool   UnFlatIdxInRange(int flat_idx, bool err_msg = true) const
  { return net_state->UnFlatIdxInRange(flat_idx, err_msg); }
  // #CAT_State test if unit flat index is in range

  inline bool   ThrInRange(int thr_no, bool err_msg = true) const
  { return net_state->ThrInRange(thr_no, err_msg); }
  // #CAT_State test if thread number is in range

  inline bool   LayerInRange(int lay_idx, bool err_msg = true) const
  { return net_state->LayerInRange(lay_idx, err_msg); }
  // #CAT_State test if layer number is in range
  
  inline bool   PrjnInRange(int prjn_idx, bool err_msg = true) const
  { return net_state->PrjnInRange(prjn_idx, err_msg); }
  // #CAT_State test if prjn number is in range
  
  inline bool   UnGpInRange(int ungp_idx, bool err_msg = true) const
  { return net_state->UnGpInRange(ungp_idx, err_msg); }
  // #CAT_State test if ungp number is in range
  
  inline LayerState_cpp* GetLayerState(int lay_idx) {
    return net_state->GetLayerState(lay_idx);
  }
  // #CAT_State get layer state for given layer index
  inline PrjnState_cpp* GetPrjnState(int prjn_idx) {
    return net_state->GetPrjnState(prjn_idx);
  }
  // #CAT_State get prjn state for given prjn index
  inline UnGpState_cpp* GetUnGpState(int ungp_idx) {
    return net_state->GetUnGpState(ungp_idx);
  }
  // #CAT_State get unit group state for given index

  inline bool   ThrUnIdxInRange(int thr_no, int thr_un_idx, bool err_msg = true) const
  { return net_state->ThrUnIdxInRange(thr_no, thr_un_idx, err_msg); }
  // #CAT_State test if thread-based unit index is in range

  inline bool   UnRecvConGpInRange(int flat_idx, int recv_idx, bool err_msg = true) const
  { return net_state->UnRecvConGpInRange(flat_idx, recv_idx, err_msg); }
  // #CAT_State test if unit recv con group index is in range
  inline bool   UnSendConGpInRange(int flat_idx, int send_idx, bool err_msg = true) const
  { return net_state->UnSendConGpInRange(flat_idx, send_idx, err_msg); }
  // #CAT_State test if unit send con group index is in range
  inline bool   ThrUnRecvConGpInRange(int thr_no, int thr_un_idx, int recv_idx,
                                      bool err_msg = true) const
  { return net_state->ThrUnRecvConGpInRange(thr_no, thr_un_idx, recv_idx, err_msg); }
  // #CAT_State test if thread-specified unit recv con group index is in range
  inline bool   ThrUnSendConGpInRange(int thr_no, int thr_un_idx, int send_idx,
                                      bool err_msg = true) const
  { return net_state->ThrUnSendConGpInRange(thr_no, thr_un_idx, send_idx, err_msg); }
  // #CAT_State test if thread-specified unit send con group index is in range

  inline int    UnThr(int flat_idx) const
  { return net_state->UnThr(flat_idx); }
  // #CAT_State thread that owns and processes the given unit (flat_idx)
  inline int    UnThrUnIdx(int flat_idx) const
  { return net_state->UnThrUnIdx(flat_idx); }
  // #CAT_State index in thread-specific memory where that unit lives for given unit (flat_idx)
  inline int    ThrNUnits(int thr_no) const
  { return net_state->ThrNUnits(thr_no);  }
  // #CAT_State number of units processed by given thread
  inline int    ThrUnitIdx(int thr_no, int thr_un_idx) const
  { return net_state->ThrUnitIdx(thr_no, thr_un_idx); }
  // #CAT_State flat_idx of unit at given thread, thread-specific unit index (max ThrNUnits()-1)
  inline UnitState_cpp*  ThrUnitState(int thr_no, int thr_un_idx) const
  { return net_state->ThrUnitState(thr_no, thr_un_idx);}
  // #CAT_State unit state for unit at given thread, thread-specific unit index (max ThrNUnits()-1)
  inline UnitState_cpp*  GetUnitState(int flat_idx) const
  { return net_state->GetUnitState(flat_idx); }
  // #CAT_State unit state for given flat_idx 
  inline int    ThrLayUnStart(int thr_no, int lay_no)
  { return net_state->ThrLayUnStart(thr_no, lay_no); }
  // #CAT_State starting thread-specific unit index for given layer (from state_layers list)
  inline int    ThrLayUnEnd(int thr_no, int lay_no)
  { return net_state->ThrLayUnEnd(thr_no, lay_no); }
  // #CAT_State ending thread-specific unit index for given layer (from state_layers list) -- this is like the max in a for loop -- valid indexes are < end
  inline int    ThrUnGpUnStart(int thr_no, int lay_no)
  { return net_state->ThrUnGpUnStart(thr_no, lay_no); }
  // #CAT_State starting thread-specific unit index for given unit group 
  inline int    ThrUnGpUnEnd(int thr_no, int lay_no)
  { return net_state->ThrUnGpUnEnd(thr_no, lay_no); }
  // #CAT_State ending thread-specific unit index for given unit group -- this is like the max in a for loop -- valid indexes are < end
  inline float& ThrLayStats(int thr_no, int lay_idx, int stat_var, int stat_type) 
  { return net_state->ThrLayStats(thr_no, lay_idx, stat_var, stat_type); }
  // #IGNORE get layer statistic value for given thread, layer (state layer index), stat variable number (0..n_lay_stats_vars-1 max), and stat type (SSE, PRERR, etc)

  inline int    UnNRecvConGps(int flat_idx) const
  { return net_state->UnNRecvConGps(flat_idx); }
  // #CAT_State number of recv connection groups for given unit at flat_idx
  inline int    UnNSendConGps(int flat_idx) const
  { return net_state->UnNSendConGps(flat_idx); }
  // #CAT_State number of send connection groups for given unit at flat_idx
  inline int    UnNRecvConGpsSafe(int flat_idx) const
  { return net_state->UnNRecvConGpsSafe(flat_idx); }
  // #CAT_State number of recv connection groups for given unit at flat_idx
  inline int    UnNSendConGpsSafe(int flat_idx) const
  { return net_state->UnNSendConGpsSafe(flat_idx); }
  // #CAT_State number of send connection groups for given unit at flat_idx

  inline int    ThrUnNRecvConGps(int thr_no, int thr_un_idx) const
  { return net_state->ThrUnNRecvConGps(thr_no, thr_un_idx); }
  // #CAT_State number of recv connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  inline int    ThrUnNSendConGps(int thr_no, int thr_un_idx) const
  { return net_state->ThrUnNSendConGps(thr_no, thr_un_idx); }
  // #CAT_State number of send connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  inline int    ThrUnNRecvConGpsSafe(int thr_no, int thr_un_idx) const
  { return net_state->ThrUnNRecvConGpsSafe(thr_no, thr_un_idx); }
  // #CAT_State number of recv connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  inline int    ThrUnNSendConGpsSafe(int thr_no, int thr_un_idx) const
  { return net_state->ThrUnNSendConGpsSafe(thr_no, thr_un_idx); }
  // #CAT_State number of send connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  
  inline int    ThrNRecvConGps(int thr_no) const
  { return net_state->ThrNRecvConGps(thr_no); }
  // #CAT_State number of recv connection groups as a flat list across all units processed by given thread
  inline int    ThrNSendConGps(int thr_no) const
  { return net_state->ThrNSendConGps(thr_no); }
  // #CAT_State number of send connection groups as a flat list across all units processed by given thread

  inline ConState_cpp* ThrRecvConState(int thr_no, int thr_cgp_idx) const
  { return net_state->ThrRecvConState(thr_no, thr_cgp_idx); }
  // #CAT_State recv ConState for given thread, thread-specific con-group index 
  inline ConState_cpp* ThrSendConState(int thr_no, int thr_cgp_idx) const
  { return net_state->ThrSendConState(thr_no, thr_cgp_idx); }
  // #CAT_State send ConState for given thread, thread-specific con-group index 

  inline ConState_cpp* ThrUnRecvConState(int thr_no, int thr_un_idx, int recv_idx) const
  { return net_state->ThrUnRecvConState(thr_no, thr_un_idx, recv_idx); }
  // #CAT_State recv ConState for given thread, thread-specific unit index, and recv group index
  inline ConState_cpp* ThrUnSendConState(int thr_no, int thr_un_idx, int send_idx) const
  { return net_state->ThrUnSendConState(thr_no, thr_un_idx, send_idx); }
  // #CAT_State send ConState for given thread, thread-specific unit index, and send group index

  inline ConState_cpp* RecvConState(int flat_idx, int recv_idx) const
  { return net_state->RecvConState(flat_idx, recv_idx); }
  // #CAT_State recv ConState for given flat unit index and recv group index number
  inline ConState_cpp* SendConState(int flat_idx, int send_idx) const
  { return net_state->SendConState(flat_idx, send_idx); }
  // #CAT_State send ConState for given flat unit index and send index number

  inline ConState_cpp* ThrUnRecvConStateSafe(int thr_no, int thr_un_idx, int recv_idx) const
  { return net_state->ThrUnRecvConStateSafe(thr_no, thr_un_idx, recv_idx); }
  // #CAT_State recv ConState for given thread, thread-specific unit index, and recv group index
  inline ConState_cpp* ThrUnSendConStateSafe(int thr_no, int thr_un_idx, int send_idx) const
  { return net_state->ThrUnSendConStateSafe(thr_no, thr_un_idx, send_idx); }
  // #CAT_State send ConState for given thread, thread-specific unit index, and send group index

  inline ConState_cpp* RecvConStateSafe(int flat_idx, int recv_idx) const
  { return net_state->RecvConStateSafe(flat_idx, recv_idx); }
  // #CAT_State recv ConState for given flat unit index and recv group index number
  inline ConState_cpp* SendConStateSafe(int flat_idx, int send_idx) const 
  { return net_state->SendConStateSafe(flat_idx, send_idx); }
  // #CAT_State send ConState for given flat unit index and send index number

  inline float* ThrSendNetinTmp(int thr_no) const 
  { return net_state->ThrSendNetinTmp(thr_no); }
  // #CAT_State temporary sending netinput memory for given thread -- no NETIN_PER_PRJN version
  inline float* ThrSendNetinTmpPerPrjn(int thr_no, int recv_idx) const 
  { return net_state->ThrSendNetinTmpPerPrjn(thr_no, recv_idx); }
  // #CAT_State temporary sending netinput memory for given thread -- NETIN_PER_PRJN version


  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning
  //    Many functions operate directly on the units via threads, with
  //    optional call through to the layers for any layer-level subsequent processing

  virtual void  SyncAllState();
  // #CAT_State synchronize all state -- net and layer
  virtual void  SyncNetState();
  // #CAT_State synchronize our main state with NetworkState computational state objects -- each variable is either on one side or the other, and sync copies in proper direction
  virtual void  SyncLayerState();
  // #CAT_State synchronize all layer main state with LayerState computational state objects -- each variable is either on one side or the other, and sync copies in proper direction
  virtual void  SyncLayerState_Layer(Layer* lay);
  // #CAT_State synchronize one layer main state with LayerState computational state object -- each variable is either on one side or the other, and sync copies in proper direction
  virtual void  SyncPrjnState();
  // #CAT_State synchronize all prjn main state with PrjnState computational state objects -- each variable is either on one side or the other, and sync copies in proper direction
  virtual void  SyncPrjnState_Prjn(Projection* lay);
  // #CAT_State synchronize one projection main state with PrjnState computational state object -- each variable is either on one side or the other, and sync copies in proper direction

  virtual void  Init_Epoch();
  // #CAT_Activation Initializes network state at the start of a new epoch -- updates parameters according to param_seq for example

  virtual void  Init_InputData();
  // #CAT_Activation Initializes external and target inputs
  virtual void  Init_Acts();
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Activation initialize the unit activation state variables
  virtual void  Init_dWt();
  // #CAT_Learning Initialize the weight change variables
  virtual void  Init_Weights();
  // #BUTTON #MENU #CONFIRM #ENABLE_ON_flags:BUILT,INTACT #CAT_Learning Initialize the weights -- also inits acts, counters and stats -- does unit level threaded and then does Layers after
    virtual void Init_Weights_renorm();
    // #IGNORE renormalize weights after init, before sym
    virtual void Init_Weights_post();
    // #CAT_Learning post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc) -- this MUST be called after any external modifications to the weights, e.g., the TransformWeights or AddNoiseToWeights calls on any lower-level objects (layers, units, con groups)
    virtual void Init_Weights_AutoLoad();
    // #CAT_Learning auto-load weights from Weights object, if it has auto_load set..

  virtual void  Init_Metrics();
  // #CAT_Statistic this is an omnibus guy that initializes every metric: Counters, Stats, and Timers
  virtual void  Init_Counters();
  // #EXPERT #CAT_Counter initialize all counter variables on network (called in Init_Weights; except batch because that loops over inits!)
  inline void  Init_Stats() { net_state->Init_Stats(); }
  // #EXPERT #CAT_Statistic initialize statistic variables on network
  virtual void  Init_Timers();
  // #EXPERT #CAT_Statistic initialize statistic variables on network

  virtual void  Init_Sequence() { };
  // #CAT_Activation called by NetGroupedDataLoop at the start of a sequence (group) of input data events -- some algorithms may want to have a flag to optionally initialize activations at this point

  virtual void  Compute_Netin();
  // #CAT_Activation Compute NetInput: weighted activation from other units
  virtual void  Send_Netin();
  // #CAT_Activation sender-based computation of net input: weighted activation from other units
  virtual void  Compute_Act();
  // #CAT_Activation Compute Activation based on net input
  virtual void  Compute_NetinAct();
  // #CAT_Activation compute net input from other units and then our own activation value based on that -- use this for feedforward networks to propagate activation through network in one compute cycle

  virtual void  Compute_dWt();
  // #CAT_Learning compute weight changes -- the essence of learning

  virtual bool  Compute_Weights_Test(int trial_no);
  // #CAT_Learning check to see if it is time to update the weights based on the given number of completed trials (typically trial counter + 1): if ON_LINE, always true; if SMALL_BATCH, only if trial_no % batch_n_eff == 0; if BATCH, never (check at end of epoch and run then)
  virtual void  Compute_Weights();
  // #CAT_Learning update weights for whole net: calls DMem_SumDWts before doing update if in dmem mode

  virtual void  Compute_SSE(bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activations vs targets over the entire network -- optionally taking the average over units, and square root of the final results
  virtual void  Compute_PRerr();
  // #CAT_Statistic compute precision and recall error statistics over entire network -- true positive, false positive, and false negative -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r), specificity, fall-out, mcc.

  virtual void  Compute_TrialStats();
  // #CAT_Statistic compute trial-level statistics (SSE and others defined by specific algorithms)
  virtual void  DMem_ShareTrialData(DataTable* dt, int n_rows = 1);
  // #CAT_DMem share trial data from given datatable across the trial-level dmem communicator (outer loop) -- each processor gets data from all other processors; if called every trial with n_rows = 1, data will be identical to non-dmem; if called at end of epoch with n_rows = -1 data will be grouped by processor but this is more efficient

  virtual void  Compute_EpochSSE();
  // #CAT_Statistic compute epoch-level sum squared error and related statistics
  virtual void  Compute_EpochPRerr();
  // #CAT_Statistic compute epoch-level precision and recall statistics
  virtual void  Compute_EpochStats();
  // #CAT_Statistic compute epoch-level statistics; calls DMem_ComputeAggs (if dmem) and EpochSSE -- specific algos may add more


  ////////////////////////////////////////////////////////////////////////////////
  //    Network Infrastructure etc

  virtual void  Build();
  // #BUTTON #CAT_Structure Build the network units and Connect them (calls CheckSpecs/BuildLayers/Units/Prjns and Connect)
    virtual void  BuildNetState();
    // #IGNORE build network state object
    virtual void  BuildIndexesSizes();
    // #IGNORE compute the indexes and sizes of all network objects
    virtual void  BuildSpecs();
    // #IGNORE Build the specs on the State
    virtual void  CheckSpecs();
    // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig
    virtual void  UpdateAllStateSpecs();
    // #CAT_State update all the State-side specs based on current settings in main specs
    virtual void  UpdateAllStateLayerSpecs();
    // #CAT_State update all the State-side specs based on current settings in main specs
    virtual void  UpdateAllStatePrjnSpecs();
    // #CAT_State update all the State-side specs based on current settings in main specs
    virtual void  UpdateAllStateUnitSpecs();
    // #CAT_State update all the State-side specs based on current settings in main specs
    virtual void  UpdateAllStateConSpecs();
    // #CAT_State update all the State-side specs based on current settings in main specs
    virtual void  BuildLayerUnitState();
    // #IGNORE initialize layer, projection, unit group, and unit state from corresponding network objects
    virtual void  BuildStateSizes();
    // #IGNORE set the sizes of all state objects based on type information
    virtual void  BuildLayerState_FromNet();
    // #IGNORE initialize layer, projection, and unit group state from corresponding network objects
    virtual void  BuildConState();
    // #IGNORE allocate and build ConState objects to prepare for connecting
    virtual void  BuildSendNetinTmp();
    // #IGNORE build sender-based netinput temporary memory -- called after connect
  virtual void  Connect();
  // #IGNORE Connect this network according to projections on Layers -- must be done as part of Build to ensure proper sync
    
#if 0 // turn off when not in need for debugging    
    virtual bool CompareNetThrVal_int
      (Network* oth_net, const String& nm, const int our, const int their);
    // #IGNORE
    virtual bool CompareNetThrVal_int64
      (Network* oth_net, const String& nm, const int64_t our, const int64_t their);
    // #IGNORE
    virtual bool CompareNetThrVal_ints
      (Network* oth_net, const String& nm, const int* our, const int* their, int n);
    // #IGNORE
    virtual bool CompareNetThrVal_mem
      (Network* oth_net, const String& nm, const char* our, const char* their, int n);
    // #IGNORE
    virtual bool CompareNetThrVal_memf
      (Network* oth_net, const String& nm, const float* our, const float* their, int64_t n);
    // #IGNORE
    virtual bool CompareNetThrVals(Network* oth_net);
    // Compare network threading values across networks -- to debug differences..
#endif // CompareNetThrVals debugging -- turn off when not needed
    
  virtual void  UnBuild();
  // #BUTTON #CAT_Structure un-build the network -- remove all units and connections -- network configuration is much faster when operating on an un-built network
  
  virtual bool    AutoBuild();
  // #CAT_Structure called by ProjectBase::AutoBuildNets() -- does auto-building and loading of weight files after project is loaded
    
  virtual bool  CheckBuild(bool quiet=false);
  // #CAT_Structure check if network units are built

  virtual void  UpdtAfterNetMod();
  // #CAT_ObjectMgmt update network after any network modification (calls appropriate functions)

  virtual void  SyncSendPrjns();
  // #CAT_Structure synchronize sending projections with the recv projections so everyone's happy

  virtual void  RemoveCons();
  // #CAT_Structure Remove all connections in network -- generally should not be called separately -- use UnBuild() to cleanly remove everything
    virtual void  RemoveCons_impl();
    // #IGNORE implementation, with no gui updates etc
  virtual void  RemoveUnits();
  // #CAT_Structure synonym for UnBuild -- remove all units in network -- also calls RemoveCons()

  virtual void  UpdatePrjnIdxs();
  // #CAT_Structure fix the projection indexes of the connection groups (recv_idx, send_idx)

  virtual void  Copy_Weights(const Network* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_ObjectMgmt copies weights from other network (incl wts assoc with unit bias member)

  virtual void  SaveWeights(const String& fname="", WtSaveFormat fmt = NET_FMT);
  // #BUTTON #MENU #EXT_wts #COMPRESS #ENABLE_ON_flags:BUILT,INTACT #CAT_File #FILETYPE_Weights #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual bool  LoadWeights(const String& fname="", bool quiet = false);
  // #BUTTON #MENU #EXT_wts #COMPRESS #ENABLE_ON_flags:BUILT,INTACT #CAT_File #FILETYPE_Weights #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (fmt is read from file) (leave fname empty to pull up file chooser)

  virtual void  SaveToWeights(Weights* wts);
  // #BUTTON #MENU #NULL_OK #ENABLE_ON_flags:BUILT,INTACT #NULL_TEXT_NewWeightsObj write weight values out to given weights object (NULL = make a new one)
  virtual bool  LoadFmWeights(Weights* wts, bool quiet = false);
  // #MENU #FROM_LIST_weights load weight values from given weights object

  virtual void  SaveToFirstWeights();
  // #CAT_File write weight values out to the first Weights object in the weights list -- if it does not yet exist, then create it -- useful for basic save and load of one cached set of weights, as compared to a situation where you need to manage multiple different weight sets
  virtual bool  LoadFmFirstWeights(bool quiet = false);
  // #CAT_File load weight values from first Weights object -- if it does not yet exist, emit an error message -- useful for basic save and load of one cached set of weights, as compared to a situation where you need to manage multiple different weight sets
  
  virtual void  SaveWeights_Tagged();
  // #CAT_File save weights using standard naming format as generted with the SaveWeights program, based on the tag environment variable
  virtual bool  SaveWeights_ClusterRunCmd();
  // #CAT_File check if user has sent a specific command to save weights through jobs_running_cmd.dat file -- called at end of epoch in Compute_EpochStats
  virtual bool  SaveWeights_ClusterRunTerm();
  // #CAT_File update cluster run job info and check if it is time to save weights before job terminates -- called in Compute_Weights

  virtual bool  ComputeHash(bool incl_weights = true);
  // #CAT_Structure create a unique hash code of the entire state of the network, including all indexes, sizes, connectivity, and optionally the weight values -- used for testing identicality of different networks, e.g., across DMem / mpi nodes

#ifdef DMEM_COMPILE
  virtual bool  DMem_ConfirmHash(bool incl_weights = true);
  // #CAT_DMem create a unique hash code of the entire state of the network, and confirm that it is the same across all nodes in current DMem group -- triggers an error and returns false if they do not match
#else
  virtual bool  DMem_ConfirmHash(bool incl_weights = true) { return true; }
  // #CAT_DMem create a unique hash code of the entire state of the network, and confirm that it is the same across all nodes in current DMem group -- triggers an error and returns false if they do not match (null function if not dmem)
#endif  
  
  virtual Layer* NewLayer();
  // #CAT_Structure #MENU #MENU_ON_Structure create a new layer in the network, using default layer type

  virtual bool EditState();
  // #BUTTON edit the network state values that drive actual C++ computation

  virtual void  ReplaceSpecs_Gp(const BaseSpec_Group& old_spg, BaseSpec_Group& new_spg, bool prompt = true);
  // #CAT_Structure replace a specs on two matching spec groups, including iterating through any children of each spec
  virtual int   ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp, bool prompt = true);
  // #CAT_Structure #MENU_BUTTON #MENU_ON_Specs switch any units/layers using old_sp to using new_sp
  virtual int   ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp, bool prompt = true);
  // #CAT_Structure #MENU_BUTTON #MENU_ON_Specs switch any connections/projections using old_sp to using new_sp
  virtual int   ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp, bool prompt = true);
  // #CAT_Structure #MENU_BUTTON #MENU_ON_Specs switch any projections using old_sp to using new_sp
  virtual int   ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp, bool prompt = true);
  // #CAT_Structure #MENU_BUTTON #MENU_ON_Specs switch any layers using old_sp to using new_spec -- optionally prompt for each replacement
  virtual void  ReplaceSpecs(BaseSpec* old_sp, BaseSpec* new_sp, bool prompt = true);
  // #CAT_Structure #MENU_BUTTON #MENU_ON_Specs replace a spec of any kind, including iterating through any children of that spec and replacing all those with corresponding child in new spec

  virtual void  MonitorItemChange();
  // #CAT_Display some item has been added or deleted from NetNetMonitor (the network owned monitor)
  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this network
  virtual void  RemoveMonitors();
  // #CAT_ObjectMgmt Remove monitoring of all objects in all processes associated with parent project
  virtual void  UpdateMonitors();
  // #CAT_ObjectMgmt Update monitoring of all objects in all processes associated with parent project
  virtual void  MonitorData();
  // #CAT_Statistic run our own monitor to update data in mon_data -- algorithms should call this at the finest update frequency -- only runs if gui active
  virtual void  NetControlPanel(ControlPanel* editor, const String& extra_label = "",
                                const String& sub_gp_nm = "");
  // #MENU #MENU_ON_ControlPanels #MENU_SEP_BEFORE #NULL_OK_0  #NULL_TEXT_0_NewCtrlPanel #CAT_Display add the key network counters and statistics to a project control panel (if ctrl_panel is NULL, a new one is created in .ctrl_panels).  The extra label is prepended to each member name, and if sub_group, then all items are placed in a subgroup with the network's name.  NOTE: be sure to click update_after on NetCounterInit and Incr at appropriate program level(s) to trigger updates of select edit display (typically in Train to update epoch -- auto update of all after Step so only needed for continuous update during runnign)

  virtual bool  SnapVar(const String& variable = "");
  // #MENU_BUTTON #MENU_ON_Snapshot #ARGC_0 #CAT_Statistic take a snapshot of specified variable (or currently selected variable in netview if empty or using from the gui) -- copies this value to the snap unit variable
  virtual bool  SnapAnd(const String& variable = "");
  // #MENU_BUTTON #MENU_ON_Snapshot #ARGC_0 #CAT_Statistic do an AND-like MIN computation of the current snap unit variable and the current value of the specified variable (or currently selected variable in netview if empty or using from the gui) -- shows the intersection between current state and previously snap'd state
  virtual bool  SnapOr(const String& variable = "");
  // #MENU_BUTTON #MENU_ON_Snapshot #ARGC_0 #CAT_Statistic do an OR-like MAX computation of the current snap unit variable and the current value of the specified variable (or currently selected variable in netview if empty or using from the gui) -- shows the union between current state and previously snap'd state
  virtual bool  SnapThresh(float thresh_val = 0.5f, const String& variable = "");
  // #MENU_BUTTON #MENU_ON_Snapshot #ARGC_1 #CAT_Statistic take a snapshot of specified variable (or currently selected variable if empty) in netview -- copies this value to the snap unit variable, but also applies a thresholding such that values above the thresh_val are set to 1 and values below the thresh_val are set to 0
  virtual bool  Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic take a snapshot of given variable (if empty, currently viewed variable in netview is used): assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.

#ifdef TA_GUI
  virtual NetView* NewView(T3Panel* fr = NULL);
  // #NULL_OK #NULL_TEXT_0_NewFrame #MENU_BUTTON #MENU_ON_NetView #CAT_Display make a new viewer of this network (NULL=use existing empty frame if any, else make new frame)
  virtual NetView* FindMakeView(T3Panel* fr = NULL);
  // #CAT_Display find existing or make a new viewer of this network (NULL=use existing empty frame if any, else make new frame)
  virtual NetView* FindView();
  // #CAT_Display find (first) existing viewer of this network
  virtual void FindViews(T3DataView_List* dv_list);
  // #CAT_Display find all existing viewers of this network
  virtual BrainView* FindBrainView();
  // #CAT_Display find (first) existing viewer of this network
  virtual void FindBrainViews(T3DataView_List* dv_list);
  // #CAT_Display find all existing viewers of this network
  virtual String GetViewVar();
  // #CAT_Display get the currently viewed variable name from netview
  virtual bool  SetViewVar(const String& view_var);
  // #CAT_Display set the variable name to view in the netview
  virtual UnitState_cpp* GetViewSrcU();
  // #CAT_Display get the currently picked source unit (for viewing weights) from netview
  virtual bool  SetViewSrcU(UnitState_cpp* src_u);
  // #CAT_Display set the picked source unit (for viewing weights) in netview
#endif

  virtual BrainView*    NewBrainView(T3Panel* fr = NULL);
  // #NULL_OK_0 #NULL_TEXT_0_NewFrame #MENU_BUTTON #MENU_ON_NetView #CAT_Display Create an fMRI-style brain visualization to show activations in defined brain areas.
  virtual void          AssignVoxels();
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display assign voxel coordinates to units in the network according to current atlas_name on the Network and brain_area specifications on the Layers
  virtual void          HistMovie(int x_size=640, int y_size=480,
                                  const String& fname_stub = "movie_img_");
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display record individual frames of the netview display from current position through to the end of the history buffer, as movie frames -- use mjpeg tools http://mjpeg.sourceforge.net/ (pipe png2yuv into mpeg2enc) to compile the individual PNG frames into an MPEG movie, which can then be transcoded (e.g., using VLC) into any number of other formats
  virtual DataTable*    NetStructToTable(DataTable* dt = NULL, bool list_specs = false);
  // #MENU_BUTTON #MENU_ON_NetView #MENU_SEP_BEFORE #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure record the network structure to given data table, including names of layers and layer groups, sizes, and where each layer receives projections from and sends projections to -- if list_specs also include columns for layer and unit specs
  virtual void          NetStructFmTable(DataTable* dt);
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Structure configure network structure (layer and layer group names, sizes, positions, connectivity) from data table (should be in same format as generated by NetStructToTable)
  virtual DataTable*    NetPrjnsToTable(DataTable* dt = NULL, bool include_off = false);
  // #MENU_BUTTON #MENU_ON_NetView #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure record the network projection structure to given data table, with one row per projection per layer, including the connection and projection specs used and notes -- optional whether to include lesioned layers and projections that have the off flag marked
  virtual String        NetPrjnsToList(taMarkUp::Format fmt, bool include_off = false);
  // #MENU_BUTTON #MENU_ON_NetView #USE_RVAL #CAT_Structure record the network projection structure to a mark-up formatted list, indented by layer group (if present), layer, then projections including the connection and projection specs used -- optional whether to include lesioned layers and projections that have the off flag marked
  virtual void          NetPrjnsToList_gp
    (Layer_Group* gp, String& rval, taMarkUp::Format fmt, bool include_off, int& indent);
  // #IGNORE impl list of projections for all layers in layer group

  
  virtual void  LayerPos_RelPos();
  // #CAT_Structure update relative positioning of units in the layer according to any active pos_rel settings -- also checks for any loops and breaks them -- called automatically during build
  virtual void  LayerZPos_Unitize();
  // #MENU #MENU_ON_Structure #CAT_Structure set layer z axis positions to unitary increments (0, 1, 2.. etc)
  virtual void  LayerPos_Cleanup();
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Structure cleanup the layer positions relative to each other (prevent overlap etc) -- sets layers to use relative positioning based on their current relative positions if they overlap
  virtual void  LayerPos_GridLayout_3d(int x_space = 2, int y_space = 3,
                                       int z_size = 3, int gp_grid_x = -1,
                                       int lay_grid_x = -1);
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Structure for the 3D layer positions: arrange layers and layer subgroups into a grid with given spacing, distributed across given number of z (vertical) layers, and you can optionally constrain the x (horizontal) dimension of the grid for the subgroups within the network or layers within groups (or just the layers if no subgroups) if gp_grid_x > 0 or layer_grid_x > 0

  virtual void  Compute_LayerDistances();
  // #MENU #MENU_ON_Structure #MENU_SEP_BEFORE #CONFIRM #CAT_Structure compute distances between layers and input/output layers
  virtual void  Compute_PrjnDirections();
  // #MENU #CONFIRM #CAT_Structure compute the directions of projections based on the relative distances from input/output layers (calls Compute_LayerDistances first)

  virtual void  SetUnitNames(bool force_use_unit_names = false);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Structure for all layers, set unit names from unit_names matrix (called automatically on Build) -- also ensures unit_names fits geometry of layer -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
  virtual void  SetUnitNamesFromDataTable(DataTable* unit_names_table, int max_unit_chars=-1,
                                          bool propagate_names=false);
  // #MENU #MENU_ON_State #CAT_Structure label units in the network based on unit names table -- also sets the unit_names matrix in the layer so they are persistent -- max_unit_chars is max length of name to apply to unit (-1 = all) -- if propagate_names is set, then names will be propagated along one-to-one projections to other layers that do not have names in the table (GetLocalistName)
  virtual void  GetLocalistName();
  // #CAT_Structure look for a receiving projection from a single unit, which has a name: if found, set our name to that name

  virtual void  TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void  AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int   PruneCons(const SimpleMathSpec& pre_proc,
                             Relation::Relations rel, float cmp_val);
  // #MENU #MENU_ON_Structure #MENU_SEP_BEFORE #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int   ProbAddCons(float p_add_con, float init_wt = 0.0);
  // #MENU #USE_RVAL #CAT_Structure probabilistically add new connections (assuming prior pruning), init_wt = initial weight value of new connection
  virtual int   LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)
  virtual int   LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure turn on unit LESIONED flags with prob p_lesion (permute = fixed no. lesioned)
  virtual void  UnLesionUnits();
  // #MENU #USE_RVAL #CAT_Structure un-lesion units: turn off all unit LESIONED flags
  virtual void  LesionAllLayers();
  // #MENU #CAT_Structure #MENU_SEP_BEFORE lesion all of the layers in the network (turns on LESIONED flag)
  virtual void  IconifyAllLayers();
  // #MENU #CAT_Structure iconify all of the layers in the network (turns on ICONIFIED flag, shrinks layers to size of 1 unit in the network display, or makes them invisible if lesioned)
  virtual void  UnLesionAllLayers();
  // #MENU #CAT_Structure un-lesion all of the layers in the network (turns off LESIONED flag)
  virtual void  DeIconifyAllLayers();
  // #MENU #CAT_Structure de-iconify all of the layers in the network (turns off ICONIFIED flag, makes them show up in the network display)

  virtual DataTable*    WeightsToTable(DataTable* dt, Layer* recv_lay, Layer* send_lay);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send entire set of weights from sending layer to recv layer in given table (e.g., for analysis), with one row per receiving unit, and the pattern in the event reflects the weights into that unit
  virtual DataTable*    VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (if a network, one col, if a layer, number of layers, etc).  for projection data, specify: prjns.xxx  for weight values, specify r. or s. (e.g., r.wt) -- this uses a NetMonitor internally (just does AddNetwork with variable, then gets data), so see documentation there for more information
  virtual DataTable*    ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
               const String& var3 = "", const String& var4 = "", const String& var5 = "",
               const String& var6 = "", const String& var7 = "", const String& var8 = "",
               const String& var9 = "", const String& var10 = "", const String& var11 = "",
               const String& var12 = "", const String& var13 = "", const String& var14 = "");
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var"
  virtual bool  VarToVarCopy(const String& dest_var, const String& src_var);
  // #CAT_Structure copy one unit variable to another (un->dest_var = un->src_var) for all units within this network (must be a float type variable)
  virtual bool  VarToVal(const String& dest_var, float val);
  // #CAT_Structure set variable to given value for all units within this network (must be a float type variable)

  virtual void  ProjectUnitWeights(UnitState_cpp* un, int top_k_un = 5, int top_k_gp=1, bool swt = false,
                                   bool zero_sub_hiddens=false);
  // #CAT_Statistic project given unit's weights (receiving unless swt = true) through all layers (without any loops) -- results stored in wt_prjn on each unit (tmp_calc1 is used as a sum variable).  top_k_un (< 1 = all) is number of strongest units to allow to pass information further down the chain -- lower numbers generally make the info more interpretable.  top_k_gp is number of unit groups to process for filtering through, if layer has sub groups (< 1 = ignore subgroups). values are always normalized at each layer to prevent exponential decrease/increase effects, so results are only relative indications of influence -- if zero_sub_hiddens then intermediate hidden units (indicated by layer_type == HIDDEN) that have sub-threshold values zeroed

  // wizard construction functions:
  virtual BaseSpec_Group* FindMakeSpecGp(const String& nm, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a given spec group and if not found, make it
  virtual BaseSpec* FindMakeSpec(const String& nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a given spec and if not found, make it
  virtual BaseSpec* FindSpecName(const String& nm);
  // #CAT_Structure find a given spec by name
  virtual BaseSpec* FindSpecType(TypeDef* td);
  // #CAT_Structure find a given spec by type

  virtual void      SpecCompareWithChildren(BaseSpec* parent_spec);
  // creates a table with a column of values for the parent spec and each child spec - values are shown if #CONDSHOW is on and if a child spec also checks override - if both are true the value is displayed
  virtual void      SpecComparePeers(BaseSpec* key_spec, BaseSpec* peer_spec);
  // creates a table with a column of values for key_spec and another column for peer_spec - values for peer_spec are shown if different from key_spec - if table with key_spec exists a call with a new peer adds a column to the table
  virtual void      AddChildToSpecCompareTable(DataTable* spec_table, BaseSpec* spec);
  // #IGNORE called recursively to add a column for all child specs to spec data table -- called by SpecCompare() -- this table is not automatically updated -- call again if you change specs!
  virtual void      AddPeerToSpecCompareTable(DataTable* spec_table, BaseSpec* spec);
  // #IGNORE called to add a column for a single spec to spec data table -- called by SpecComparePeers() -- this table is not automatically updated -- call again if you change specs!
  
  virtual bool      ShowSpecMember(MemberDef* spec_md, MemberDef* spec_member_md);
  // #IGNORE returns true for members that are user editable and are visible in spec panel
  virtual bool      ShowSpecMemberValue(MemberDef* spec_member_md, TypeDef* typ, taBase* base);
  // #IGNORE checks CONDSHOW
  virtual void      WriteSpecMbrNamesToTable(DataTable* spec_table, BaseSpec* spec);
  // #IGNORE writes spec member names to a spec compare table -- See SpecCompare()
  virtual void      WriteSpecMbrValsToTable(DataTable* spec_table, BaseSpec* spec, bool is_child, bool is_peer);
  // #IGNORE writes spec member values to a spec compare table -- See SpecCompare()
  
  virtual Layer*    FindMakeLayer(const String& nm, TypeDef* td = NULL,
                       bool& nw_itm = nw_itm_def_arg, const String& alt_nm = "");
  // #CAT_Structure find a given layer and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
  virtual Layer_Group* FindMakeLayerGroup(const String& nm, TypeDef* td = NULL,
                          bool& nw_itm = nw_itm_def_arg, const String& alt_nm = "");
  // #CAT_Structure find a given layer group and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
  virtual Layer_Group* FindLayerGroup(const String& nm);
  // #CAT_Structure find a given layer group -- only searches in top-level layer groups
  virtual Layer* FindLayer(const String& nm) { return (Layer*)layers.FindLeafName(nm); }
  // #CAT_Structure find layer by name
  virtual Projection* FindMakePrjn(Layer* recv, Layer* send, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a projection between two layers using given specs, make it if not found; if existing prjn between layers exists, it will be modified with current specs
  virtual Projection* FindMakePrjnAdd(Layer* recv, Layer* send, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a projection between two layers using given specs, make it if not found; if existing prjn between layers exists but has diff specs, a new prjn is made
  virtual Projection* FindMakeSelfPrjn(Layer* recv, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a self projection using given specs, make it if not found; if existing self prjn exists, it will be modified with current specs
  virtual Projection* FindMakeSelfPrjnAdd(Layer* recv, ProjectionSpec* ps = NULL, ConSpec* cs = NULL, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a self projection using given specs, make it if not found; if existing self prjn exists but has diff specs, a new prjn is made
  virtual bool   RemovePrjn(Layer* recv, Layer* send, ProjectionSpec* ps = NULL, ConSpec* cs = NULL);
  // #CAT_Structure remove a projection between two layers, if it exists
  virtual bool   RemoveLayer(const String& nm) { return layers.RemoveName(nm); }
  // #CAT_Structure remove layer with given name, if it exists

  virtual String  MemoryReport(bool print = true);
  // #CAT_Statistic #MENU #MENU_ON_Structure #MENU_SEP_BEFORE #USE_RVAL report about memory allocation for the network

  virtual void    StartTiming();
  // #CAT_Statistic #MENU #MENU_ON_Structure start recording timing information -- turns on the TIMING flag and starts recording timing from parallel threading mechanisms
  virtual void    StopTiming();
  // #CAT_Statistic #MENU #MENU_ON_Structure stop recording timing information -- turns off the TIMING flag and stops recording timing from parallel threading mechanisms
  virtual String  TimingReport(DataTable* table, bool print = true);
  // #CAT_Statistic #MENU #MENU_ON_Structure #USE_RVAL #NULL_OK_0 #NULL_TEXT_0_NewTable report about timing of various steps of computation -- used for optimizing code etc -- table has detailed info per thread -- calls StopTiming to finalize data
  virtual void    TimingReportInitNames();
  // #IGNORE initialize net_timing_names string array for report -- derived classes need to add

  virtual void  UpdateLayerGroupGeom();
  // #IGNORE update layer group geometries (max_disp_size, positions) and max_disp_size of of network based on current layer layout

  virtual void  SetProjectionDefaultTypes(Projection* prjn);
  // #IGNORE this is called by the projection InitLinks to set its default types: overload in derived algorithm-specific networks to provide appropriate default types

  virtual void  DMemTrialBarrier();
  // #CAT_DMem block all dmem processors at the trial level until everyone reaches this same point in the program flow -- cordinates all the processors at this point -- important for cases where there are interdependencies among processors, where they need to be coordinated going forward -- does nothing if dmem_nprocs <= 1 or not using dmem

#ifdef DMEM_COMPILE
  DMemComm      dmem_trl_comm;  // #IGNORE the dmem communicator for the trial-level (each node processes a different set of trials)
  DMemAggVars   dmem_agg_sum;           // #IGNORE aggregation of network variables using SUM op (currently only OP in use -- add others as needed)
  virtual void  DMem_UpdtWtUpdt();
  // #CAT_DMem update wt_update and small_batch parameters for dmem, depending on trl_comm.nprocs
  virtual void  DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void  DMem_SumDWts(MPI_Comm comm);
  // #IGNORE sync weights across trial-level dmem by summing delta-weights across processors (prior to computing weight updates)
  virtual void  DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network and layer variables across procs for trial-level dmem
#endif

#ifdef CUDA_COMPILE
  virtual bool  Cuda_MakeCudaNet();
  // #IGNORE make the cuda_net object if currently null -- override in specific algos!
  
  virtual void  Cuda_BuildNet();
  // #IGNORE build all the network structures for cuda -- called after regular network build
  virtual void  Cuda_FreeNet();
  // #IGNORE build all the network structures for cuda -- called after regular network build
  virtual void  Cuda_InitConStates();
  // #IGNORE transfer C++ con group info over to cuda con groups -- they are different!  called in Cuda_BuildNet()

  virtual void  Cuda_CopyUnitSpec(void* cuda_us, const UnitSpec* source) { };
  // #IGNORE copy parameters from the source unitspec into the cuda unitspec -- algorithms define this
  virtual void  Cuda_MakeUnitSpecs();
  // #IGNORE generate the unique_unitspecs list of unique con specs that are actually used in the model -- and then allocate cuda_unit_specs memory block
  virtual void  Cuda_UpdateUnitSpecs();
  // #IGNORE copy each of the cuda unitspecs from associated C++ UnitSpecs, and then sync over to device memory
  
  taBase_RefList cuda_con_specs;
  // #IGNORE list of unique conspecs actually used in projections -- source for the cuda versions of the conspecs
  
  virtual void  Cuda_CopyConSpec(void* cuda_cs, const ConSpec* source) { };
  // #IGNORE copy parameters from the source conspec into the cuda conspec -- algorithms define this
  virtual void  Cuda_MakeConSpecs();
  // #IGNORE generate the unique_conspecs list of unique con specs that are actually used in the model -- and then allocate cuda_con_specs memory block
  virtual void  Cuda_UpdateConSpecs();
  // #IGNORE copy each of the cuda conspecs from associated C++ ConSpecs, and then sync over to device memory
  
#endif
  // these methods can be put in program code, so we always expose -- just dummies
  // if not actually running cuda..
  
  virtual void  Cuda_UnitStateToHost();
  // #CAT_CUDA get all the unit state variables (acts etc) back from the GPU device to the host
  virtual void  Cuda_UnitStateToDevice();
  // #CAT_CUDA send all the unit state variables (acts etc) to the GPU device from the host
  virtual void  Cuda_ConStateToHost();
  // #CAT_CUDA get all the connection state variables (weights, dwts, etc) back from the GPU device to the host -- this is done automatically before SaveWeights*
  virtual void  Cuda_ConStateToDevice();
  // #CAT_CUDA send all the connection state variables (weights, dwts, etc) to the GPU device from the host -- this is done automatically after Init_Weights and LoadWeights*
  virtual void  Cuda_UpdateSpecs();
  // #CAT_CUDA update all the specs stored in the cuda device, based on current settings -- called automatically after Init_Weights, but needs to be called manually when specs are changed
  virtual String Cuda_TimingReport(bool print = true);
  // #CAT_CUDA report time used statistics for CUDA operations (only does something for cuda compiled version)

  virtual void  BgRunKilled();
  // #IGNORE called when program is quitting prematurely and is not in an interactive mode - save network if SAVE_KILLED_WTS flag is set
  
  int  Dump_Load_Value(std::istream& strm, taBase* par=NULL) override;
  int  Save_strm(std::ostream& strm, taBase* par=NULL, int indent=0) override;
  int  Dump_Save_impl(std::ostream& strm, taBase* par=NULL, int indent=0) override;

  String       GetTypeDecoKey() const override { return "Network"; }
  String       GetToolbarName() const override { return "network"; }

  bool         ChangeMyType(TypeDef* new_type) override;
  taBase*      ChooseNew(taBase* origin, const String& choice_text) override;
  bool         HasChooseNew() override { return true; }
  String       GetArgForCompletion(const String& method, const String& arg) override;
  void         GetArgCompletionList(const String& method, const String& arg, const String_Array& arg_values, taBase* arg_obj, const String& cur_txt, Completions& completions) override;

  void         setStale() override;

  void  InitLinks() override;
  void  CutLinks() override;
  void  Copy_(const Network& cp);
  TA_BASEFUNS(Network);

protected:
  void UpdateAfterEdit_impl() override;
  void CheckThisConfig_impl(bool quiet, bool& rval) override;
  void CheckChildConfig_impl(bool quiet, bool& rval) override;
  void UpdatePointersAfterCopy_impl(const taBase& cp) override;

private:
  void  Initialize();
  void  Destroy();
};

#endif // Network_h
