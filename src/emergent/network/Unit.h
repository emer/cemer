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

#ifndef Unit_h
#define Unit_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:
#include <UnitRef>
#include <UnitSpec>
#include <Voxel_List>
#include <RecvCons_List>
#include <SendCons_List>
#include <taVector3i>

// declare all other types mentioned but not required to include:
class Projection; //
class Layer; //
class Network; //
class Unit_Group; //


eTypeDef_Of(Unit);

class E_API Unit: public taNBase {
  // ##NO_TOKENS ##DMEM_SHARE_SETS_3 ##CAT_Network Generic unit -- basic computational unit of a neural network (e.g., a neuron-like processing unit)
INHERITED(taNBase)
public: //
  enum ExtType {// #BITS indicates type of external input; some flags used in Layer to control usage
    NO_EXTERNAL         = 0x00, // #NO_BIT no input
    TARG                = 0x01, // a target value used to train the network (value goes in targ field of unit)
    EXT                 = 0x02, // an external input value that drives activations (value goes in ext field of unit)
    COMP                = 0x04, // a comparison value used for computing satistics but not training the network (value goes in targ field of unit)
    TARG_EXT            = 0x03, // #NO_BIT as both external input and target value
    COMP_TARG           = 0x05, // #NO_BIT as a comparision and target layer
    COMP_EXT            = 0x06, // #NO_BIT as a comparison and external input layer
    COMP_TARG_EXT       = 0x07  // #NO_BIT as a comparison, target, and external input layer
  };

  enum UnitFlags { // #BITS misc flags for units
    UF_NONE             = 0,    // #NO_BIT no flags
    LESIONED            = 0x0001, // unit is temporarily lesioned (inactivated for all network-level processing functions) -- IMPORTANT: use the Lesion and UnLesion functions to set this flag -- they provide proper updating after changes -- otherwise network dynamics will be wrong and the display will not be properly updated
  };

  UnitFlags     flags;
  // #CAT_Structure flags controlling various aspects of unit state and function
  ExtType       ext_flag;
  // #GUI_READ_ONLY #SHOW #CAT_Activation tells what kind of external input unit received -- this is normally set by the ApplyInputData function -- it is not to be manipulated directly
  float         targ;
  // #VIEW_HOT #CAT_Activation target value: drives learning to produce this activation value
  float         ext;
  // #VIEW_HOT #CAT_Activation external input: drives activation of unit from outside influences (e.g., sensory input)
  float         act;
  // #DMEM_SHARE_SET_2 #VIEW_HOT #CAT_Activation activation value -- what the unit communicates to others
  float         net;
  // #DMEM_SHARE_SET_1 #VIEW_HOT #CAT_Activation net input value -- what the unit receives from others (typically sum of sending activations times the weights)
  float         wt_prjn;
  // #NO_SAVE #CAT_Statistic weight projection value -- computed by Network::ProjectUnitWeights (triggered in GUI by setting wt prjn variable in netview control panel to point to a layer instead of NULL) -- represents weight values projected through any intervening layers from source unit (selected unit in netview or passed to ProjectUnitWeights function directly)
  float         snap;
  // #NO_SAVE #CAT_Statistic current snapshot value, as computed by the Snapshot function -- this can be displayed as a border around the units in the netview
  float         tmp_calc1;
  // #NO_SAVE #READ_ONLY #HIDDEN #CAT_Statistic temporary calculation variable (used for computing wt_prjn and prossibly other things)

  RecvCons_List recv;
  // #CAT_Structure Receiving connections, one set of connections for each projection (collection of connections) received from other units
  SendCons_List send;
  // #CAT_Structure Sending connections, one set of connections for each projection (collection of connections) sent from other units
  RecvCons      bias;
  // #CAT_Structure bias weight connection (type determined in unit spec) -- provides intrinsic activation in absence of other inputs

  int           n_recv_cons;
  // #CAT_Structure #DMEM_SHARE_SET_0 #READ_ONLY #EXPERT total number of receiving connections
  taVector3i       pos;
  // #CAT_Structure display position in space relative to owning group or layer -- in structural 3D coordinates
  Voxel_List*    voxels;
  // #CAT_Structure #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #NO_VIEW Voxels assigned to this unit in a brain view.
  int           idx;
  // #CAT_Structure #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of this unit within containing unit group
  int           flat_idx;
  // #CAT_Structure #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of this unit in a flat array of units (used by parallel threading) -- 0 is special null case -- real idx's start at 1

#ifdef DMEM_COMPILE
  static int    dmem_this_proc; // #IGNORE processor rank for this processor RELATIVE TO COMMUNICATOR for the network
  int           dmem_local_proc; // #IGNORE processor on which these units are local
  virtual bool  DMem_IsLocalProc(int proc)      { return dmem_local_proc == proc; } // #IGNORE
  virtual bool  DMem_IsLocal()                  { return dmem_local_proc == dmem_this_proc; }  // #IGNORE
  virtual int   DMem_GetLocalProc()             { return dmem_local_proc; } // #IGNORE
  virtual void  DMem_SetLocalProc(int lproc)    { dmem_local_proc = lproc; } // #IGNORE
  virtual void  DMem_SetThisProc(int proc)      { dmem_this_proc = proc; } // #IGNORE
#endif

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
  // #MENU #MENU_ON_Structure #DYN1 #MENU_SEP_BEFORE #CAT_Structure set the lesion flag on unit -- removes it from all processing operations
  virtual void  UnLesion();
  // #MENU #DYN1 #CAT_Structure un-set the lesion flag on unit -- restores it to engage in normal processing
  virtual void  UpdtAfterNetModIfNecc();
  // #IGNORE call network UpdtAfterNetMod only if it is not otherwise being called at a higher level

  inline bool   lay_lesioned() const;
  // #CAT_Structure #IGNORE is the layer this unit is in lesioned?
  inline Layer* own_lay() const;
  // #CAT_Structure #IGNORE get the owning layer of this unit
  Network*      own_net() const;
  // #CAT_Structure get the owning network of this unit
  Unit_Group*   own_subgp() const;
  // #CAT_Structure get the owning subgroup of this unit -- NULL if unit lives directly within the layer and not in a subgroup -- note that with virt_groups as default, most units do not have an owning subgroup even if there are logical subgroups
  int           UnitGpIdx() const;
  // #CAT_Structure get unit's subgroup index -- returns -1 if layer does not have unit groups -- directly from info avail on unit itself

  inline UnitSpec* GetUnitSpec() const { return m_unit_spec; }
  // #CAT_Structure get the unit spec for this unit -- this is controlled entirely by the layer and all units in the layer have the same unit spec
  inline void   SetUnitSpec(UnitSpec* us) { m_unit_spec = us; if(us) bias.SetConSpec(us->bias_spec.SPtr()); }
  // #CAT_Structure set the unit spec to given value -- no ref counting or other checking is done

  virtual void  Copy_Weights(const Unit* src, Projection* prjn = NULL);
  // #CAT_ObjectMgmt copies weights from other unit (incl wts assoc with unit bias member) -- if prjn is specified, then it only copies weights for that particular projection on this unit, from the same projection index number on the src unit (assumes that there is a correspondence in the projections across this and source unit!)

  virtual void  SaveWeights_strm(std::ostream& strm, Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   LoadWeights_strm(std::istream& strm, Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus, TAG_END if successful
  static int    SkipWeights_strm(std::istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
                                 bool quiet = false);
  // #IGNORE skip over saved weight values -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void  SaveWeights(const String& fname="", Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int   LoadWeights(const String& fname="", Projection* prjn = NULL, RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  void          SetExtFlag(ExtType flg) { ext_flag = (ExtType)(ext_flag | flg); }
  // #CAT_Activation set ext flag for what type of input data we receive
  void          UnSetExtFlag(ExtType flg) { ext_flag = (ExtType)(ext_flag & ~flg); }
  // #CAT_Activation un-set ext flag for what type of input data we receive
  bool          HasExtFlag(int flg) { return ext_flag & flg; }
  // #CAT_Activation check if has given ext flag value

  virtual void  ApplyInputData(float val, ExtType act_ext_flags, Random* ran = NULL,
                               bool na_by_range=false);
  // #CAT_Activation apply external input or target value to unit

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning
  //    Many functions operate directly on the units via threads, and then
  //    call through to the layers for any layer-level subsequent processing
  //    units typically call spec versions except for basic stuff

  //    Init functions are NOT threaded, while Compute functions are

  virtual void  Init_InputData() { ext = targ = 0.0f; ext_flag = NO_EXTERNAL; }
  // #MENU #MENU_ON_Actions initialize unit external input data variables

  void  Init_Acts(Network* net) { GetUnitSpec()->Init_Acts(this, net); }
  // #MENU #CAT_Activation initialize unit state variables
  void  Init_dWt(Network* net)  { GetUnitSpec()->Init_dWt(this, net); }
  // #MENU #CAT_Learning initialze weight change variables
  void  Init_Weights(Network* net) { GetUnitSpec()->Init_Weights(this, net); }
  // #MENU #CAT_Learning Initialize weight values
  void  Init_Weights_post(Network* net) { GetUnitSpec()->Init_Weights_post(this, net); }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  void  Compute_Netin(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Netin(this, net, thread_no); }
  // #CAT_Activation compute net input from other units
  void  Send_Netin(Network* net, int thread_no=-1)
  { GetUnitSpec()->Send_Netin(this, net, thread_no); }
  // #CAT_Activation send net input to other units
  void  Compute_SentNetin(Network* net, float sent_netin)
  { GetUnitSpec()->Compute_SentNetin(this, net, sent_netin); }
  // #CAT_Activation compute net input for unit based on sent_netin value from Send_Netin
  void  Compute_Act(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Act(this, net, thread_no); }
  // #CAT_Activation compute activation value: what we send to others
  void  Compute_NetinAct(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Netin(this, net, thread_no);
    GetUnitSpec()->Compute_Act(this, net, thread_no); }
  // #CAT_Activation compute net input from other units and then our own activation value based on that -- use this for feedforward networks to propagate activation through network in one compute cycle

  void  Compute_dWt(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_dWt(this, net, thread_no); }
  // #CAT_Learning compute weight changes: the essence of learning
  void  Compute_Weights(Network* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Weights(this, net, thread_no); }
  // #CAT_Learning update weight values from weight change variables

  float Compute_SSE(Network* net, bool& has_targ)
  { return GetUnitSpec()->Compute_SSE(this, net, has_targ); }
  // #CAT_Statistic compute sum-squared-error of activations versus target values (standard measure of performance) -- not threadable due to integration requirements at higher levels
  bool  Compute_PRerr(Network* net, float& true_pos, float& false_pos, float& false_neg)
  { return GetUnitSpec()->Compute_PRerr(this, net, true_pos, false_pos, false_neg); }
  // #CAT_Statistic compute precision and recall error statistics for this unit -- true positive, false positive, and false negative -- returns true if unit actually has a target value specified (otherwise everything is 0) -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance


  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  virtual bool  BuildUnits();
  // #CAT_Structure build unit: make sure bias connection is created and right type
  virtual bool  CheckBuild(bool quiet=false);
  // #CAT_Structure check if network is built
  virtual void  RemoveCons();
  // #IGNORE remove all of unit's sending and receiving connections -- since this doesn't affect other units, it should not be called individually
  virtual void  RecvConsPreAlloc(int no, Projection* prjn);
  // #CAT_Structure pre-allocate given no of receiving connections -- sufficient connections must be allocated in advance of making specific connections
  virtual void  SendConsPreAlloc(int no, Projection* prjn);
  // #CAT_Structure pre-allocate given no of sending connections -- sufficient connections must be allocated in advance of making specific connections
  virtual void  SendConsAllocInc(int no, Projection* prjn);
  // #CAT_Structure increment size by given no of sending connections -- later call SendConsPostAlloc to actually allocate connections
  virtual void  RecvConsAllocInc(int no, Projection* prjn);
  // #CAT_Structure increment size by given no of recv connections -- later call RecvConsPostAlloc to actually allocate connections
  virtual void  SendConsPostAlloc(Projection* prjn);
  // #CAT_Structure post-allocate given no of sending connections (calls AllocConsFmSize on send con group) -- if connections were initially made using the alloc_send = true, then this must be called to actually allocate connections -- then routine needs to call ConnectFrom again to make the connections
  virtual void  RecvConsPostAlloc(Projection* prjn);
  // #CAT_Structure post-allocate given no of recv connections (calls AllocConsFmSize on recv con group) -- if connections were initially made using the alloc_send = true, then this must be called to actually allocate connections -- then routine needs to call ConnectFrom again to make the connections
  virtual Connection*   ConnectFrom(Unit* su, Projection* prjn, bool alloc_send = false,
                                    bool ignore_alloc_errs = false);
  // #CAT_Structure make a recv connection from given unit to this unit using given projection -- requires both recv and sender to have sufficient connections allocated already, unless alloc_send is true, then it only allocates connections on the sender -- does NOT make any connection on the receiver -- use this in a loop that runs connections twice, with first pass as allocation (then call SendConstPostAlloc) and second pass as actual connection making
  virtual Connection*   ConnectFromCk(Unit* su, Projection* prjn,
                                      bool ignore_alloc_errs = false);
  // #CAT_Structure does ConnectFrom but checks for an existing connection to prevent double-connections -- note that this is expensive -- only use if there is a risk of multiple connections.  This does not support alloc_send option -- can call in 2nd pass if needed
  virtual bool  DisConnectFrom(Unit* su, Projection* prjn=NULL);
  // #CAT_Structure remove connection from given unit (projection is optional)
  virtual void  DisConnectAll();
  // #MENU #MENU_ON_Actions #CAT_Structure disconnect unit from all other units
  virtual int   CountRecvCons();
  // #CAT_Structure count total number of receiving connections

  virtual void  GetLocalistName();
  // #CAT_Structure look for a receiving projection from a single unit, which has a name: if found, set our name to that name

  virtual void  TransformWeights(const SimpleMathSpec& trans, Projection* prjn = NULL);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void  AddNoiseToWeights(const Random& noise_spec, Projection* prjn = NULL);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int   PruneCons(const SimpleMathSpec& pre_proc, Relation::Relations rel,
                             float cmp_val, Projection* prjn = NULL);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int   LesionCons(float p_lesion, bool permute=true, Projection* prjn = NULL);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this unit
  virtual bool  Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #BUTTON #CAT_Statistic take a snapshot of given variable: assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.

  virtual DataTable*    VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual DataTable*    ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
               const String& var3 = "", const String& var4 = "", const String& var5 = "",
               const String& var6 = "", const String& var7 = "", const String& var8 = "",
               const String& var9 = "", const String& var10 = "", const String& var11 = "",
               const String& var12 = "", const String& var13 = "", const String& var14 = "",
               Projection* prjn=NULL);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var" -- prjn restricts to that prjn

  virtual void  LinkPtrCons();
  // #IGNORE link pointer connections from the corresponding owned connections -- only needed after a Copy

  override int  GetIndex() const { return idx; }
  override void SetIndex(int i) { idx = i; }
  virtual int   GetMyLeafIndex();
  // #CAT_Structure compute leaf index from my individual index in an efficient manner
  void          GetAbsPos(taVector3i& abs_pos)  { abs_pos = pos; AddRelPos(abs_pos); }
  // #CAT_Structure get absolute pos, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void          GetAbsPos2d(taVector2i& abs_pos)  { abs_pos = pos; AddRelPos2d(abs_pos); }
  // #CAT_Structure get absolute pos in 2d, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void          LayerLogPos(taVector2i& log_pos);
  // #CAT_Structure get logical position of unit within layer, taking into account (virtual) unit groups etc relative to layer flat_geom (no display spacing) -- calls Layer::UnitLogPos on own_lay
  void          LayerDispPos(taVector2i& disp_pos);
  // #CAT_Structure get display position of this unit within the layer, taking into account (virtual) unit groups etc relative to layer disp_geom (includes display spacing) -- calls Layer::UnitDispPos on own_lay
  void          AddRelPos(taVector3i& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above
  void          AddRelPos2d(taVector2i& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above

  virtual void	MakeVoxelsList();
  // make the voxels* list if it doesn't yet exist -- otherwise not

  override String       GetTypeDecoKey() const { return "Unit"; }

  override void         SetDefaultName() {} // leave it blank

  override bool ChangeMyType(TypeDef* new_type);

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const Unit& cp);
  TA_BASEFUNS(Unit);

protected:
  UnitSpec*     m_unit_spec;    // unit spec that we use: controlled entirely by the layer!

  override void  UpdateAfterEdit_impl();
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
  override void  CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void  Initialize();
  void  Destroy();
};

#endif // Unit_h
