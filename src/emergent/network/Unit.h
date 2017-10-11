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

#ifndef Unit_h
#define Unit_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:
#include <UnitRef>
#include <Voxel_List>
#include <taVector3i>
#include <UnitSpec>

#include <ConState_cpp>
#include <UnitState_cpp>

#include <State_main>

// declare all other types mentioned but not required to include:
class Projection; //
class Layer; //
class Network; //
class Unit_Group; //


eTypeDef_Of(Unit);

class E_API Unit : public taNBase {
  // ##NO_TOKENS ##CAT_Network contains all the structural information for units (name, position, etc), while UnitState contains the computationally-relevant variables, which are stored separately for optimized vector-based processing
INHERITED(taNBase)
public: //
  enum UnitFlags { // #BITS misc flags for units
    UF_NONE             = 0,    // #NO_BIT no flags
    LESIONED            = 0x0001, // #READ_ONLY unit is temporarily lesioned (inactivated for all network-level processing functions) -- IMPORTANT: use the Lesion and UnLesion functions to set this flag -- they provide proper updating after changes -- otherwise network dynamics will be wrong and the display will not be properly updated
  };

  enum WtSaveFormat {   // note: must be coordinated with the Network enum
    TEXT,                       // weights are saved as ascii text representation of digits (completely portable)
    BINARY,                     // weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
  };

  UnitFlags     flags;
  // #CAT_State flags controlling various aspects of unit state and function
  float         wt_prjn;
  // #NO_SAVE #CAT_Statistic weight projection value -- computed by Network::ProjectUnitWeights (triggered in GUI by setting wt prjn variable in netview control panel to point to a layer instead of NULL) -- represents weight values projected through any intervening layers from source unit (selected unit in netview or passed to ProjectUnitWeights function directly)
  float         snap;
  // #NO_SAVE #CAT_Statistic current snapshot value, as computed by the Snapshot function -- this can be displayed as a border around the units in the netview
  float         tmp_calc1;
  // #NO_SAVE #READ_ONLY #HIDDEN #CAT_Statistic temporary calculation variable (used for computing wt_prjn and prossibly other things)

  int           n_recv_cons;
  // #CAT_State #READ_ONLY #EXPERT total number of recv connections for this unit
  int           n_send_cons;
  // #CAT_State #READ_ONLY #EXPERT total number of send connections for this unit
  taVector3i    pos;
  // #CAT_State display position in space relative to owning group or layer -- in structural 3D coordinates
  Voxel_List*   voxels;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #NO_VIEW Voxels assigned to this unit in a brain view.
  int           idx;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of this unit within containing unit group -- for virtual unit groups, this is NOT the index within the virtual unit group, but rather the index within the entire layer unit group
  int           flat_idx;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of this unit in a flat array of units (used by parallel threading) -- 0 is special null case -- real idx's start at 1
  int           lay_un_idx;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of this unit within containing layer -- same as idx for virtual unit groups, otherwise the leaf index for real unit groups
  int		gp_idx;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of the unit group this unit belongs in, within the layer -- only if this unit belongs in a unit group (either virtual or real) -- assigned at build by layer
  int		ungp_un_idx;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of unit within owning unit group -- either for a sub-unit-group (at gp_idx) if layer has them, or within master layer units group
  
  int           thread_no;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE thread number that this unit's memory lives in
  int           thr_un_idx;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of this unit in owning thread's array of units in network state
  int           own_lay_idx;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of layer that we live in within networkstate master list of layers
  int           own_ungp_idx;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of unitgroup that we live in within networkstate master list of unit groups (either a sub unit group or the master layer units group)
  bool		in_subgp;
  // #CAT_State #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE determine if unit is in a REAL subgroup -- not true for virtual subgroups

  // UnitState variable access:

  inline UnitState_cpp* GetUnitState() const;
  // #IGNORE get unit state for this unit (stored in thread-specific memory)

  UnitState_cpp*        MyUnitState() const;
  // #CAT_State the unit state for this unit -- this interface is for program-based access to GetUnitState call, for obscure compiling-related issues

#ifndef __MAKETA__
  // instead of marking these all as #IGNORE, we just avoid entirely, so that
  // derived classes can redefine them and have them show up!  just copy directly into
  // algorithm-specific unit classes..
  inline UnitState_cpp::ExtFlags ext_flag() { return GetUnitState()->ext_flag; }
  // #CAT_UnitVar external input flags -- determines whether the unit is receiving an external input (EXT), target (TARG), or comparison value (COMP)
  inline float& targ()  { return GetUnitState()->targ; }
  // #VIEW_HOT #CAT_UnitVar target value: drives learning to produce this activation value
  inline float& ext()   { return GetUnitState()->ext; }
  // #VIEW_HOT #CAT_UnitVar external input: drives activation of unit from outside influences (e.g., sensory input)
  inline float& act()   { return GetUnitState()->act; }
  // #VIEW_HOT #CAT_UnitVar activation value -- what the unit communicates to others
  inline float& net()   { return GetUnitState()->net; }
  // #VIEW_HOT #CAT_UnitVar net input value -- what the unit receives from others (typically sum of sending activations times the weights)
  inline float& bias_wt() { return GetUnitState()->bias_wt; }
  // #VIEW_HOT #CAT_UnitVar bias weight value -- the bias weight acts like a connection from a unit that is always active with a constant value of 1 -- reflects intrinsic excitability from a biological perspective
  inline float& bias_dwt() { return GetUnitState()->bias_dwt; }
  // #VIEW_HOT #CAT_UnitVar change in bias weight value as computed by a learning mechanism
#endif

  inline int            NRecvConGps() const;
  // #IGNORE #CAT_State get number of receiving connection groups (determined by number of active layer projections at time of build)
  inline int            NSendConGps() const;
  // #IGNORE #CAT_State get number of sending connection groups (determined by number of active layer send_prjns at time of build)
  int                   NRecvConGpsSafe() const;
  // #CAT_State get number of receiving connection groups (determined by number of active layer projections at time of build)
  int                   NSendConGpsSafe() const;
  // #CAT_State get number of sending connection groups (determined by number of active layer send_prjns at time of build)
  inline ConState_cpp*  RecvConState(int rcg_idx) const;
  // #IGNORE #CAT_State get receiving connection group at given index -- no safe range checking is applied to rcg_idx!
  inline ConState_cpp*  SendConState(int scg_idx) const;
  // #IGNORE #CAT_State get sendingconnection group at given index -- no safe range checking is applied to scg_idx!
  ConState_cpp*         RecvConStateSafe(int rcg_idx) const;
  // #CAT_State get receiving connection group at given index, with safety checking for index in range
  ConState_cpp*         SendConStateSafe(int scg_idx) const;
  // #CAT_State get sendingconnection group at given index, with safety checking for index in range
  inline ConState_cpp*  RecvConStatePrjn(Projection* prjn) const;
  // #IGNORE get con group at given prjn->recv_idx -- no range checking -- fast
  inline ConState_cpp*  SendConStatePrjn(Projection* prjn) const;
  // #IGNORE get con group at given prjn->send_idx -- no range checking -- fast
  ConState_cpp*         RecvConStatePrjnSafe(Projection* prjn) const;
  // #CAT_State get con group at given prjn->recv_idx -- if it is not in range, emits error message and returns NULL
  ConState_cpp*         SendConStatePrjnSafe(Projection* prjn) const;
  // #CAT_State get con group at given prjn->send_idx -- if it is not in range, emits error message and returns NULL
  ConState_cpp*         FindRecvConStateFrom(Layer* fm_lay) const;
  // #CAT_State get receiving connection group from given sending layer
  ConState_cpp*         FindRecvConStateFromName(const String& fm_nm) const;
  // #CAT_State get receiving connection group from given sending layer name
  ConState_cpp*         FindSendConStateToName(const String& to_nm) const;
  // #CAT_State get sending connection group to given receiving layer name

  bool                  SetUnValName(float val, const String& var_nm);
  // #CAT_Access set unit variable (specified by name, e.g., act, net, bias_wt) to given value -- for use by programs, which cannot assign the value through the direct variable access functions (e.g., act())
  float                 GetUnValName(const String& var_nm);
  // #CAT_Access get unit variable value (specified by name, e.g., act, net, bias_wt) -- for use by programs or other generic access via name variable
  bool                  SetCnValName(float val, const Variant& prjn,
                                     int cn_idx, const String& var_nm);
  // #CAT_Access set recv connection variable (specified by name, e.g., wt, dwt, pdw) in given projection (can be specified by name or index) at given connection index (cn_idx), to given value -- for use by programs, which cannot assign the value through the SafeCn function 
  float                 GetCnValName(const Variant& prjn, int cn_idx, const String& var_nm);
  // #CAT_Access get recv connection variable value (specified by name, e.g., wt, dwt, pdw) in given projection (can be specified by name or index) at given connection index (cn_idx) -- for use by programs
  
  
  inline void           SetUnitFlag(UnitFlags flg)   { flags = (UnitFlags)(flags | flg); }
  // set flag state on
  inline void           ClearUnitFlag(UnitFlags flg) { flags = (UnitFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasUnitFlag(UnitFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetUnitFlagState(UnitFlags flg, bool on)
  { if(on) SetUnitFlag(flg); else ClearUnitFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  inline bool   lesioned() const { return HasUnitFlag(LESIONED); }
  // check if this unit is lesioned -- must check for all processing functions (threaded calls automatically exclude lesioned units)

  virtual void  Lesion();
  // #MENU #MENU_ON_State #DYN1 #MENU_SEP_BEFORE #CAT_State set the lesion flag on unit -- removes it from all processing operations
  virtual void  UnLesion();
  // #MENU #DYN1 #CAT_State un-set the lesion flag on unit -- restores it to engage in normal processing
  virtual void  UpdtAfterNetModIfNecc();
  // #IGNORE call network UpdtAfterNetMod only if it is not otherwise being called at a higher level

  inline bool           lay_lesioned() const;
  // #CAT_State #IGNORE is the layer this unit is in lesioned?
  inline Layer*         own_lay() const;
  // #CAT_State #IGNORE get the owning layer of this unit
  inline Network*       own_net() const
  { return m_own_net; }
  // #CAT_State #IGNORE get the owning network of this unit
  inline Unit_Group*    own_subgp() const
  { if(!in_subgp) return NULL; return (Unit_Group*)owner; }
  // #CAT_State get the owning subgroup of this unit -- NULL if unit lives directly within the layer and not in a subgroup -- note that with virt_groups as default, most units do not have an owning subgroup even if there are logical subgroups
  inline int            UnitGpIdx() const
  { return gp_idx; }
  // #CAT_State #IGNORE unit's subgroup index -- returns -1 if layer does not have unit groups (can be either virtual or real)
  void                  GetInSubGp();
  // #IGNORE determine if unit is in a subgroup -- sets in_subgp flag -- called by InitLinks() -- should be good..

  inline UnitSpec* GetUnitSpec() const { return m_unit_spec; }
  // #CAT_State get the unit spec for this unit -- this is controlled entirely by the layer and all units in the layer have the same unit spec
  void   SetUnitSpec(UnitSpec* us);
  // #CAT_State set the unit spec to given value -- no ref counting or other checking is done

  virtual void  Copy_Weights(const Unit* src, Projection* prjn = NULL);
  // #CAT_ObjectMgmt copies weights from other unit (incl wts assoc with unit bias member) -- if prjn is specified, then it only copies weights for that particular projection on this unit, from the same projection index number on the src unit (assumes that there is a correspondence in the projections across this and source unit!)

  static int    LoadWeights_StartTag(std::istream& strm, const String& tag,
                                     String& val, bool quiet);
  // #IGNORE read in a start tag -- makes sure it matches tag, returns TAG_GOT if got it
  static int    LoadWeights_EndTag(std::istream& strm, const String& trg_tag,
                                   String& cur_tag, int& stat, bool quiet);
  // #IGNORE read in an end tag -- makes sure it matches trg_tag, cur_tag, stat are current read_tag & status (if !END_TAG, will try to read end)
  static int    ConsSkipWeights_strm(std::istream& strm, WtSaveFormat fmt = TEXT,  bool quiet = false);
  // #IGNORE skip over saved weight values -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void  ConsSaveWeights_strm(std::ostream& strm, ConState_cpp* cg, Unit* ru, Network* net,
                                     WtSaveFormat fmt = TEXT);
  // #IGNORE write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   ConsLoadWeights_strm(std::istream& strm, ConState_cpp* cg, Unit* ru, Network* net,
                                     WtSaveFormat fmt = TEXT, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary format) -- rval is taMisc::ReadTagStatus, TAG_END if successful -- the connections for both sides must already be allocated, but it can rearrange connections based on save unit indexes for random connectivity etc

  virtual void  SaveWeights_strm(std::ostream& strm, WtSaveFormat fmt = TEXT, Projection* prjn = NULL);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   LoadWeights_strm(std::istream& strm, WtSaveFormat fmt = TEXT, bool quiet = false, Projection* prjn = NULL);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus, TAG_END if successful
  static int    SkipWeights_strm(std::istream& strm, WtSaveFormat fmt = TEXT,  bool quiet = false);
  // #IGNORE skip over saved weight values -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void  ApplyInputData(float val, UnitState_cpp::ExtFlags act_ext_flags,
                               Random* ran = NULL, bool na_by_range=false);
  // #CAT_Activation apply external input or target value to unit

  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  virtual void  BuildUnits();
  // #CAT_State build unit -- allocate any extra unit-level memory etc
  virtual bool  CheckBuild(bool quiet=false);
  // #CAT_State check if network is built
  virtual void  RecvConsPreAlloc(int no, Projection* prjn);
  // #CAT_State pre-allocate given no of receiving connections -- sufficient connections must be allocated in advance of making specific connections
  virtual void  SendConsPreAlloc(int no, Projection* prjn);
  // #CAT_State pre-allocate given no of sending connections -- sufficient connections must be allocated in advance of making specific connections
  virtual void  SendConsAllocInc(int no, Projection* prjn);
  // #CAT_State increment size by given no of sending connections -- later call SendConsPostAlloc to actually allocate connections
  virtual void  RecvConsAllocInc(int no, Projection* prjn);
  // #CAT_State increment size by given no of recv connections -- later call RecvConsPostAlloc to actually allocate connections
  virtual void  SendConsPostAlloc(Projection* prjn);
  // #CAT_State post-allocate given no of sending connections (calls AllocConsFmSize on send con group) -- if connections were initially made using the alloc_send = true, then this must be called to actually allocate connections -- then routine needs to call ConnectFrom again to make the connections
  virtual void  RecvConsPostAlloc(Projection* prjn);
  // #CAT_State post-allocate given no of recv connections (calls AllocConsFmSize on recv con group) -- if connections were initially made using the alloc_send = true, then this must be called to actually allocate connections -- then routine needs to call ConnectFrom again to make the connections

  virtual int   ConnectFrom(Unit* su, Projection* prjn, bool alloc_send = false,
                            bool ignore_alloc_errs = false, bool set_init_wt = false,
                            float init_wt = 0.0f);
  // #CAT_State make a recv connection from given unit to this unit using given projection -- requires both recv and sender to have sufficient connections allocated already, unless alloc_send is true, then it only allocates connections on the sender -- does NOT make any connection on the receiver -- use this in a loop that runs connections twice, with first pass as allocation (then call SendConstPostAlloc) and second pass as actual connection making -- return val is index of recv connection -- can also optionally set initial weight value
  virtual int   ConnectFromCk(Unit* su, Projection* prjn, bool ignore_alloc_errs = false,
                              bool set_init_wt = false, float init_wt = 0.0f);
  // #CAT_State does ConnectFrom but checks for an existing connection to prevent double-connections -- note that this is expensive -- only use if there is a risk of multiple connections.  This does not support alloc_send option -- can call in 2nd pass if needed -- return val is index of recv connection -- can also optionally set initial weight value
  virtual bool  DisConnectFrom(Unit* su, Projection* prjn=NULL);
  // #CAT_State remove connection from given unit (projection is optional)
  virtual void  DisConnectAll();
  // #MENU #MENU_ON_Actions #CAT_State disconnect unit from all other units
  virtual int   CountCons(Network* net);
  // #CAT_State count total number of owned connections
  virtual void  UpdtActiveCons();
  // #CAT_State update the active state of all connection groups

  virtual bool  ShareRecvConsFrom(Unit* shu, Projection* prjn);
  // #CAT_State share our receiving connection group connection memory for given projection from given other source unit -- shu must appear prior to this layer in the same layer
  
  virtual void  GetLocalistName();
  // #CAT_State look for a receiving projection from a single unit, which has a name: if found, set our name to that name

  virtual void  TransformWeights(const SimpleMathSpec& trans, Projection* prjn = NULL);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights -- must call Init_Weights_post at network level after running this!
  virtual void  RenormWeights(bool mult_norm, float avg_wt, Projection* prjn = NULL);
  // #MENU #CAT_Learning renormalize the weight values using either multiplicative (for positive-only weight values such as Leabra) or subtractive normalization (for pos/neg weight values, such as backprop) to hit the given average weight value -- must call Init_Weights_post at network level after running this!
  virtual void  RescaleWeights(const float rescale_factor, Projection* prjn = NULL);
  // #MENU #CAT_Learning rescale weights by multiplying by given factor -- must call Init_Weights_post at network level after running this!
  virtual void  AddNoiseToWeights(const Random& noise_spec, Projection* prjn = NULL);
  // #MENU #CAT_Learning add noise to weights using given noise specification -- must call Init_Weights_post at network level after running this!
  virtual int   PruneCons(const SimpleMathSpec& pre_proc, Relation::Relations rel,
                             float cmp_val, Projection* prjn = NULL);
  // #MENU #USE_RVAL #CAT_State remove weights that (after pre-proc) meet relation to compare val
  virtual int   LesionCons(float p_lesion, bool permute=true, Projection* prjn = NULL);
  // #MENU #USE_RVAL #CAT_State remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this unit
  virtual bool  Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #BUTTON #CAT_Statistic take a snapshot of given variable: assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.

  virtual DataTable*    VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_State send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual DataTable*    ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
               const String& var3 = "", const String& var4 = "", const String& var5 = "",
               const String& var6 = "", const String& var7 = "", const String& var8 = "",
               const String& var9 = "", const String& var10 = "", const String& var11 = "",
               const String& var12 = "", const String& var13 = "", const String& var14 = "",
               Projection* prjn=NULL);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var" -- prjn restricts to that prjn

  int           GetIndex() const override { return idx; }
  void          SetIndex(int i) override { idx = i; }
  void          GetAbsPos(taVector3i& abs_pos)  { abs_pos = pos; AddRelPos(abs_pos); }
  // #CAT_State get absolute pos, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void          GetAbsPos2d(taVector2i& abs_pos)  { abs_pos = pos; AddRelPos2d(abs_pos); }
  // #CAT_State get absolute pos in 2d, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void          LayerLogPos(taVector2i& log_pos);
  // #CAT_State get logical position of unit within layer, taking into account (virtual) unit groups etc relative to layer flat_geom (no display spacing) -- calls Layer::UnitLogPos on own_lay
  void          UnitGpLogPos(taVector2i& log_pos);
  // #CAT_State get logical position of unit within (virtual) unit group -- calls Layer::UnitInGpLogPos on own_lay
  void          LayerDispPos(taVector2i& disp_pos);
  // #CAT_State get display position of this unit within the layer, taking into account (virtual) unit groups etc relative to layer disp_geom (includes display spacing) -- calls Layer::UnitDispPos on own_lay
  void          AddRelPos(taVector3i& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above
  void          AddRelPos2d(taVector2i& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above

  virtual void	MakeVoxelsList();
  // make the voxels* list if it doesn't yet exist -- otherwise not

  String       GetTypeDecoKey() const override { return "Unit"; }

  void         SetDefaultName() override {} // leave it blank

  bool ChangeMyType(TypeDef* new_type) override;

  void  InitLinks() override;
  void  CutLinks() override;
  void  Copy_(const Unit& cp);
  TA_BASEFUNS(Unit);

protected:
  UnitSpec*     m_unit_spec;    // unit spec that we use: controlled entirely by the layer!
  Network*      m_own_net;      // network that owns us

  void  UpdateAfterEdit_impl() override;
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;
  void  CheckChildConfig_impl(bool quiet, bool& rval) override;
private:
  void  Initialize();
  void  Destroy();
};

#endif // Unit_h
