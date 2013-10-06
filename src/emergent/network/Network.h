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

#ifndef Network_h
#define Network_h 1

// parent includes:
#include <taFBase>

// member includes:
#include <NetworkRef>
#include <BaseSpec_Group>
#include <Layer_Group>
#include <taBrainAtlas_List>
#include <TimeUsed>
#include <UnitCallThreadMgr>
#include <UnitPtrList>
#include <float_Matrix>
#include <DMemComm>
#include <DMemAggVars>

// NOTE: by including network, you end up including all the network code:
// #include <Connection>
// #include <ConSpec>
// #include <BaseCons>
// #include <RecvCons>
// #include <RecvCons_List>
// #include <SendCons>
// #include <SendCons_List>
// #include <UnitSpec>
// #include <Unit>
// #include <ProjectionSpec>
// #include <Projection>
// #include <Layer>
// #include <LayerSpec>

// declare all other types mentioned but not required to include:
class ProjectBase; //
class NetView; //
class BrainView; //
class T3Panel; //

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


eTypeDef_Of(Network);

class E_API Network : public taFBase {
  // ##FILETYPE_Network ##EXT_net ##COMPRESS ##CAT_Network ##DEF_NAME_ROOT_Network ##EXPAND_DEF_2 A network, containing layers, units, etc..
INHERITED(taFBase)
public:
  static bool nw_itm_def_arg;   // #IGNORE default arg val for FindMake..

  enum Usr1SaveFmt {            // how to save network on -USR1 signal
    FULL_NET,                   // save the full network (dump file)
    JUST_WEIGHTS                // just do a 'write weights' command
  };

  enum DMem_SyncLevel {
    DMEM_SYNC_NETWORK,          // synchronize the entire network at a time
    DMEM_SYNC_LAYER             // synchronize only layer-by-layer
  };

  enum WtSaveFormat {
    TEXT,                       // weights are saved as ascii text representation of digits (completely portable)
    BINARY,                     // weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
    NET_FMT,                    // use format specified on the network
  };

  enum WtUpdate {
    ON_LINE,                    // update weights on-line (after every event) -- this is not viable for dmem processing across trials and is automatically switched to small_batch in that case
    SMALL_BATCH,                // update weights every small_batch_n trials
    BATCH                       // update weights in batch mode (after every epoch)
  };

  enum TrainMode {
    TEST,                       // network is only being tested; no learning should occur
    TRAIN                       // network is being trained: learning should occur
  };

  enum AutoBuildMode {
    AUTO_BUILD,                 // automatically build the network after loading
    PROMPT_BUILD,               // prompt about building after loading (if run in -nogui mode, it is automatically built without prompting)
    NO_BUILD,                   // do not build network after loading
  };

  enum NetFlags {               // #BITS flags for network
    NF_NONE             = 0,    // #NO_BIT
    SAVE_UNITS          = 0x0001, // save units with the project or other saves (specificaly saving just the network always saves the units)
    SAVE_UNITS_FORCE    = 0x0002, // #NO_SHOW internal flag that forces the saving of units in cases where it is important to do so (e.g., saving just the network, or for a crash recover file)
    MANUAL_POS          = 0x0004, // disables the automatic cleanup/positioning of layers
    NETIN_PER_PRJN      = 0x0008, // compute netinput per projection instead of a single aggregate value across all inputs (which is the default)
  };

  enum NetTextLoc {
    NT_BOTTOM,                  // standard bottom location below network -- extends network "foot" lower below to make text more visible
    NT_TOP_BACK,                // above top-most layer, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_LEFT_BACK,               // at left of network, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_RIGHT_BACK,              // at right of network, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_LEFT_MID,                // at left of network, in the middle depth-wise -- foot is raised as when no net text is visible
    NT_RIGHT_MID,               // at right of network, in the middle depth-wise -- foot is raised as when no net text is visible
  };

  static taBrainAtlas_List* brain_atlases;  // #NO_SAVE #NO_SHOW_TREE atlases available

  BaseSpec_Group specs;         // #CAT_Structure Specifications for network parameters
  Layer_Group   layers;         // #CAT_Structure Layers or Groups of Layers

  NetFlags      flags;          // #CAT_Structure flags controlling various aspects of network function

  AutoBuildMode auto_build;     // #CAT_Structure whether to automatically build the network (make units and connections) after loading or not (if the SAVE_UNITS flag is not on, then auto building makes sense)

  taBrainAtlasRef 	brain_atlas;  // #FROM_GROUP_brain_atlases #NO_SAVE The name of the atlas to use for brain view rendering.  Labels from this atlas can be applied to layers' brain_area member.
  String		brain_atlas_name; // #HIDDEN the name of the brain atlas that we're using -- this is what is actually saved b/c the ref is not saveable

  TrainMode     train_mode;     // #CAT_Learning training mode -- determines whether weights are updated or not (and other algorithm-dependent differences as well).  TEST turns off learning
  WtUpdate      wt_update;      // #CAT_Learning #CONDSHOW_ON_train_mode:TRAIN weight update mode: when are weights updated (only applicable if train_mode = TRAIN)
  int           small_batch_n;  // #CONDSHOW_ON_wt_update:SMALL_BATCH #CAT_Learning number of events for small_batch learning mode (specifies how often weight changes are synchronized in dmem)
  int           small_batch_n_eff; // #GUI_READ_ONLY #EXPERT #NO_SAVE #CAT_Learning effective batch_n value = batch_n except for dmem when it = (batch_n / epc_nprocs) >= 1

  int           batch;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW batch counter: number of times network has been trained over a full sequence of epochs (updated by program)
  int           epoch;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW epoch counter: number of times a complete set of training patterns has been presented (updated by program)
  int           group;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW group counter: optional extra counter to record sequence-level information (sequence = group of trials)
  int           trial;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW trial counter: number of external input patterns that have been presented in the current epoch (updated by program)
  int           tick;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW tick counter: optional extra counter to record a level of organization below the trial level (for cases where trials have multiple component elements)
  int           cycle;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW cycle counter: number of iterations of activation updating (settling) on the current external input pattern (updated by program)
  float         time;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW the current time, relative to some established starting point, in algorithm-specific units (often miliseconds)
  String        group_name;     // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name associated with the current group of trials, if such a grouping is applicable (typically set by a LayerWriter)
  String        trial_name;     // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name associated with the current trial (e.g., name of input pattern, typically set by a LayerWriter)
  String        output_name;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name for the output produced by the network (must be computed by a program)

  bool          sse_unit_avg;   // #CAT_Statistic compute sse as average sse over units (i.e., divide by total number of target units in layer)
  bool          sse_sqrt;       // #CAT_Statistic take the square root of the SSE, producing a Euclidian distance instead of raw sse (note this cannot technically be added across trials in a linear fashion, as raw sse can)
  float         sse;            // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW sum squared error over the network, for the current external input pattern
  float         sum_sse;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic total sum squared error over an epoch or similar larger set of external input patterns
  float         avg_sse;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average sum squared error over an epoch or similar larger set of external input patterns
  float         cnt_err_tol;    // #CAT_Statistic tolerance for computing the count of number of errors over current epoch
  float         cnt_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns
  float         pct_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns (= cnt_err / n)
  float         pct_cor;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was below cnt_err_tol over an epoch or similar larger set of external input patterns (= 1 - pct_err -- just for convenience for whichever you want to plot)

  float         cur_sum_sse;    // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current sum_sse -- used during computation of sum_sse
  int           avg_sse_n;      // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic number of times cur_sum_sse updated: for computing avg_sse
  float         cur_cnt_err;    // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current cnt_err -- used for computing cnt_err

  bool          compute_prerr;  // #CAT_Statistic compute precision and recall error values over units, at same time as computing sum squared error (SSE)
  PRerrVals     prerr;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic precision and recall error values for the entire network, for the current external input pattern
  PRerrVals     sum_prerr;      // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic precision and recall error values for the entire network, over an epoch or similar larger set of external input patterns -- these are always up-to-date as the system is aggregating, given the additive nature of the statistics
  PRerrVals     epc_prerr;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic precision and recall error values for the entire network, over an epoch or similar larger set of external input patterns

  TimeUsed      train_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing entire training (across epochs) (managed entirely by programs -- not always used)
  TimeUsed      epoch_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing an epoch (managed entirely by programs -- not always used)
  TimeUsed      group_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a group, when groups used (managed entirely by programs -- not always used)
  TimeUsed      trial_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a trial (managed entirely by programs -- not always used)
  TimeUsed      settle_time;    // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a settling (managed entirely by programs -- not always used)
  TimeUsed      cycle_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a cycle (managed entirely by programs -- not always used)
  TimeUsed      wt_sync_time;   // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for the DMem_SumDWts operation (trial-level dmem, computed by network)
  TimeUsed      misc_time;      // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic misc timer for ad-hoc use by programs

  UnitCallThreadMgr threads;    // #CAT_Threads parallel threading of network computation
  UnitPtrList   units_flat;     // #NO_SAVE #READ_ONLY #CAT_Threads flat list of units for deploying in threads and for connection indexes -- first (0 index) is a null unit that is skipped over and should never be computed on
  Unit*         null_unit;      // #HIDDEN #NO_SAVE unit for the first null unit in the units_flat list -- created by BuildNullUnit() function, specific to each algorithm
  float_Matrix  send_netin_tmp; // #NO_SAVE #READ_ONLY #CAT_Threads temporary storage for threaded sender-based netinput computation -- dimensions are [un_idx][task] (inner = units, outer = task, such that units per task is contiguous in memory)

  DMem_SyncLevel dmem_sync_level; // #CAT_DMem at what level of network structure should information be synchronized across processes?
  int           dmem_nprocs;    // #CAT_DMem number of processors to use in distributed memory computation of connection-level processing (actual number may be less, depending on processors requested!)
  int           dmem_nprocs_actual; // #READ_ONLY #NO_SAVE actual number of processors being used

  Usr1SaveFmt   usr1_save_fmt;  // #CAT_File save network for -USR1 signal: full net or weights
  WtSaveFormat  wt_save_fmt;    // #CAT_File format to save weights in if saving weights

  int           n_units;        // #READ_ONLY #EXPERT #CAT_Structure total number of units in the network
  int           n_cons;         // #READ_ONLY #EXPERT #CAT_Structure total number of connections in the network
  int           max_prjns;      // #READ_ONLY #EXPERT #CAT_Structure maximum number of prjns per any given layer or unit in the network
  PosVector3i   max_disp_size;  // #AKA_max_size #READ_ONLY #EXPERT #CAT_Structure maximum display size in each dimension of the net
  PosVector2i   max_disp_size2d; // #READ_ONLY #EXPERT #CAT_Structure maximum display size in each dimension of the net -- for 2D display

  ProjectBase*  proj;           // #IGNORE ProjectBase this network is in
  bool          old_load_cons;  // #IGNORE #NO_SAVE special flag (can't use flags b/c it is saved, loaded!) for case when loading a project with old cons file format (no pre-alloc of cons)

  inline void           SetNetFlag(NetFlags flg)   { flags = (NetFlags)(flags | flg); }
  // set flag state on
  inline void           ClearNetFlag(NetFlags flg) { flags = (NetFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasNetFlag(NetFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetNetFlagState(NetFlags flg, bool on)
  { if(on) SetNetFlag(flg); else ClearNetFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  inline Unit*  UnFmIdx(int idx) const { return units_flat.FastEl(idx); }
  // #CAT_Structure get the unit from its flat_idx value
  inline Unit*  UnFmIdx_Safe(int idx) const { return units_flat.SafeEl(idx); }
  // #CAT_Structure get the unit from its flat_idx value, with safe range checking (slow -- generally avoid using if possible)

  void  Build();
  // #BUTTON #CAT_Structure Build the network units and Connect them (calls CheckSpecs/BuildLayers/Units/Prjns and Connect)
    virtual void  CheckSpecs();
    // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig
    virtual void  BuildLayers();
    // #MENU #MENU_ON_Structure #CAT_Structure Build any network layers that are dynamically constructed
    virtual void  BuildUnits();
    // #MENU #CAT_Structure Build the network units in layers according to geometry
    virtual void  BuildNullUnit();
    // #IGNORE make the null_unit unit
    virtual void  BuildUnits_Threads();
    // #IGNORE build unit-level thread information: flat list of units, etc, and update thread structures
    virtual void  BuildPrjns();
    // #MENU #CAT_Structure Build any network prjns that are dynamically constructed
  void  Connect();
  // #MENU #CAT_Structure Connect this network according to projections on Layers

  virtual bool  CheckBuild(bool quiet=false);
  // #CAT_Structure check if network units are built
  virtual bool  CheckConnect(bool quiet=false);
  // #CAT_Structure check if network is connected

  virtual void  UpdtAfterNetMod();
  // #CAT_ObjectMgmt update network after any network modification (calls appropriate functions)
  virtual void  SetUnitType(TypeDef* td);
  // #MENU #TYPE_Unit #CAT_Structure set unit type for all units in layer (created by Build)

  virtual void  SyncSendPrjns();
  // #CAT_Structure synchronize sending projections with the recv projections so everyone's happy
  virtual void  CountRecvCons();
  // #CAT_Structure count recv connections for all units in network
  virtual bool  RecvOwnsCons() { return true; }
  // #CAT_Structure does the receiver own the connections (default) or does the sender?

  virtual void  ConnectUnits(Unit* u_to, Unit* u_from=NULL, bool record=true,
                             ConSpec* conspec=NULL);
  // #CAT_Structure connect u1 so that it recieves from u2. Create projection if necessary

  virtual void  RemoveCons();
  // #MENU #MENU_ON_Structure #CONFIRM #MENU_SEP_BEFORE #CAT_Structure Remove all connections in network
  virtual void  RemoveUnits();
  // #MENU #CONFIRM #CAT_Structure Remove all units in network (preserving unit groups)
  virtual void  RemoveUnitGroups();
  // #MENU #CONFIRM #CAT_Structure Remove all unit groups in network

  virtual void  FixPrjnIndexes();
  // #CAT_Structure fix the projection indexes of the connection groups (recv_idx, send_idx)

  virtual void  Copy_Weights(const Network* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_ObjectMgmt copies weights from other network (incl wts assoc with unit bias member)

  virtual void  SaveWeights_strm(std::ostream& strm, WtSaveFormat fmt = NET_FMT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual bool  LoadWeights_strm(std::istream& strm, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (fmt is read from file)

  virtual void  SaveWeights(const String& fname="", WtSaveFormat fmt = NET_FMT);
  // #BUTTON #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual bool  LoadWeights(const String& fname="", bool quiet = false);
  // #BUTTON #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (fmt is read from file) (leave fname empty to pull up file chooser)
//NOTE: if any of the Build or Connect are to be extended, the code must be rewritten by
//  calling an inner extensible virtual _impl

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning
  //    Many functions operate directly on the units via threads, with
  //    optional call through to the layers for any layer-level subsequent processing

  virtual void  Init_InputData();
  // #CAT_Activation Initializes external and target inputs
  virtual void  Init_Acts();
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Activation initialize the unit activation state variables
  virtual void  Init_dWt();
  // #CAT_Learning Initialize the weight change variables
  virtual void  Init_Weights();
  // #BUTTON #MENU #CONFIRM #CAT_Learning Initialize the weights -- also inits acts, counters and stats
  virtual void  Init_Weights_post();
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  virtual void  Init_Metrics();
  // #CAT_Statistic this is an omnibus guy that initializes every metric: Counters, Stats, and Timers

  virtual void  Init_Counters();
  // #EXPERT #CAT_Counter initialize all counter variables on network (called in Init_Weights; except batch because that loops over inits!)
  virtual void  Init_Stats();
  // #EXPERT #CAT_Statistic initialize statistic variables on network
  virtual void  Init_Timers();
  // #EXPERT #CAT_Statistic initialize statistic variables on network

  virtual void  Init_Sequence() { };
  // #CAT_Activation called by NetGroupedDataLoop at the start of a sequence (group) of input data events -- some algorithms may want to have a flag to optionally initialize activations at this point

  virtual void  Compute_Netin();
  // #CAT_Activation Compute NetInput: weighted activation from other units
  virtual void  Send_Netin();
  // #CAT_Activation sender-based computation of net input: weighted activation from other units
    inline bool NetinPerPrjn() { return HasNetFlag(NETIN_PER_PRJN); }
    // #CAT_Activation is this network configured to compute net input on a per-prjn basis?
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
  virtual void  Compute_Weights_impl();
  // #CAT_Learning just the weight update routine: update weights from delta-weight changes

  virtual void  Compute_SSE(bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activations vs targets over the entire network -- optionally taking the average over units, and square root of the final results
  virtual void  Compute_PRerr();
  // #CAT_Statistic compute precision and recall error statistics over entire network -- true positive, false positive, and false negative -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance on a per unit basis

  virtual Layer* NewLayer();
  // #BUTTON create a new layer in the network, using default layer type

  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this network
  virtual void  RemoveMonitors();
  // #CAT_ObjectMgmt Remove monitoring of all objects in all processes associated with parent project
  virtual void  UpdateMonitors();
  // #CAT_ObjectMgmt Update monitoring of all objects in all processes associated with parent project
  virtual void  NetControlPanel(SelectEdit* editor, const String& extra_label = "",
                                const String& sub_gp_nm = "");
  // #MENU #MENU_ON_SelectEdit #MENU_SEP_BEFORE #NULL_OK_0  #NULL_TEXT_0_NewEditor #CAT_Display add the key network counters and statistics to a select edit dialog (control panel) (if editor is NULL, a new one is created in .edits).  The extra label is prepended to each member name, and if sub_group, then all items are placed in a subgroup with the network's name.  NOTE: be sure to click update_after on NetCounterInit and Incr at appropriate program level(s) to trigger updates of select edit display (typically in Train to update epoch -- auto update of all after Step so only needed for continuous update during runnign)

  virtual bool  SnapVar();
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic take a snapshot of currently selected variable in netview -- copies this value to the snap unit variable
  virtual bool  SnapAnd();
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic do an AND-like MIN computation of the current snap unit variable and the current value of the variable shown in netview -- shows the intersection between current state and previously snap'd state
  virtual bool  SnapOr();
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic do an OR-like MAX computation of the current snap unit variable and the current value of the variable shown in netview -- shows the union between current state and previously snap'd state
  virtual bool  SnapThresh(float thresh_val = 0.5f);
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic take a snapshot of currently selected variable in netview -- copies this value to the snap unit variable, but also applies a thresholding such that values above the thresh_val are set to 1 and values below the thresh_val are set to 0
  virtual bool  Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap=true);
  // #MENU_BUTTON #MENU_ON_Snapshot #CAT_Statistic take a snapshot of given variable (if empty, currently viewed variable in netview is used): assign snap value on unit to given variable value, optionally using simple math operation on that value.  if arg_is_snap is true, then the 'arg' argument to the math operation is the current value of the snap variable.  for example, to compute intersection of variable with snap value, use MIN and arg_is_snap.

#ifdef TA_GUI
  virtual NetView* NewView(T3Panel* fr = NULL);
  // #NULL_OK #NULL_TEXT_0_NewFrame #MENU_BUTTON #MENU_ON_NetView #CAT_Display make a new viewer of this network (NULL=use existing empty frame if any, else make new frame)
  virtual NetView* FindMakeView(T3Panel* fr = NULL);
  // #CAT_Display find existing or make a new viewer of this network (NULL=use existing empty frame if any, else make new frame)
  virtual NetView* FindView();
  // #CAT_Display find (first) existing viewer of this network
  virtual String GetViewVar();
  // #CAT_Display get the currently viewed variable name from netview
  virtual bool  SetViewVar(const String& view_var);
  // #CAT_Display set the variable name to view in the netview
  virtual Unit* GetViewSrcU();
  // #CAT_Display get the currently picked source unit (for viewing weights) from netview
  virtual bool  SetViewSrcU(Unit* src_u);
  // #CAT_Display set the picked source unit (for viewing weights) in netview
#endif

  virtual void  PlaceNetText(NetTextLoc net_text_loc, float scale = 1.0f);
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display locate the network text data display (counters, statistics -- typically shown at bottom of network) in a new standard location (it can also be dragged anywhere in the net view, turn on lay_mv button and click on red arrow) -- can also change the scaling
  virtual BrainView* NewBrainView(T3Panel* fr = NULL);
  // #NULL_OK_0 #NULL_TEXT_0_NewFrame #MENU_BUTTON #MENU_ON_NetView #CAT_Display Create an fMRI-style brain visualization to show activations in defined brain areas.
  virtual void  AssignVoxels();
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display assign voxel coordinates to units in the network according to current atlas_name on the Network and brain_area specifications on the Layers
  virtual void  NetTextUserData();
  // #IGNORE auto-called in InitLinks -- enable the filtering of what information is shown in the network text data display (typically shown at bottom of network, though see PlaceNetText for options on where to locate) -- this function creates entries for each of the viewable items in the UserData for this network -- just click on the UserData button to edit which items to display.
  virtual void          HistMovie(int x_size=640, int y_size=480,
                                  const String& fname_stub = "movie_img_");
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Display record individual frames of the netview display from current position through to the end of the history buffer, as movie frames -- use mjpeg tools http://mjpeg.sourceforge.net/ (pipe png2yuv into mpeg2enc) to compile the individual PNG frames into an MPEG movie, which can then be transcoded (e.g., using VLC) into any number of other formats
  virtual DataTable*    NetStructToTable(DataTable* dt = NULL, bool list_specs = false);
  // #MENU_BUTTON #MENU_ON_NetView #MENU_SEP_BEFORE #NULL_OK_0 NULL_TEXT_0_NewTable #CAT_Structure record the network structure to given data table, including names of layers and layer groups, sizes, and where each layer receives projections from and sends projections to -- if list_specs also include columns for layer and unit specs
  virtual void  NetStructFmTable(DataTable* dt);
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Structure configure network structure (layer and layer group names, sizes, positions, connectivity) from data table (should be in same format as generated by NetStructToTable)
  virtual DataTable*    NetPrjnsToTable(DataTable* dt = NULL);
  // #MENU_BUTTON #MENU_ON_NetView #NULL_OK_0 NULL_TEXT_0_NewTable #CAT_Structure record the network projection structure to given data table, with one row per projection per layer, including the connection and projection specs used

  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

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


  virtual void  LayerZPos_Unitize();
  // #MENU #MENU_ON_Structure #CAT_Structure set layer z axis positions to unitary increments (0, 1, 2.. etc)
  virtual void  LayerPos_Cleanup();
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Structure cleanup the layer positions relative to each other (prevent overlap etc)

  virtual void  Compute_LayerDistances();
  // #MENU #MENU_ON_Structure #MENU_SEP_BEFORE #CONFIRM #CAT_Structure compute distances between layers and input/output layers
  virtual void  Compute_PrjnDirections();
  // #MENU #CONFIRM #CAT_Structure compute the directions of projections based on the relative distances from input/output layers (calls Compute_LayerDistances first)

  virtual void  SetUnitNames(bool force_use_unit_names = false);
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Structure for all layers, set unit names from unit_names matrix (called automatically on Build) -- also ensures unit_names fits geometry of layer -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
  virtual void  SetUnitNamesFromDataTable(DataTable* unit_names_table, int max_unit_chars=-1,
                                          bool propagate_names=false);
  // #MENU #MENU_ON_State #CAT_Structure label units in the network based on unit names table -- also sets the unit_names matrix in the layer so they are persistent -- max_unit_chars is max length of name to apply to unit (-1 = all) -- if propagate_names is set, then names will be propagated along one-to-one projections to other layers that do not have names in the table (GetLocalistName)
  virtual void  GetUnitNames(bool force_use_unit_names = true);
  // #MENU #MENU_ON_State #CAT_Structure for all layers, get unit_names matrix values from current unit name values -- also ensures unit_names fits geometry of layer -- if force_use_unit_names is true, then unit_names will be configured to save values it is not already
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

  virtual void  ProjectUnitWeights(Unit* un, int top_k_un = 5, int top_k_gp=1, bool swt = false,
                                   bool zero_sub_hiddens=false);
  // #CAT_Statistic project given unit's weights (receiving unless swt = true) through all layers (without any loops) -- results stored in wt_prjn on each unit (tmp_calc1 is used as a sum variable).  top_k_un (< 1 = all) is number of strongest units to allow to pass information further down the chain -- lower numbers generally make the info more interpretable.  top_k_gp is number of unit groups to process for filtering through, if layer has sub groups (< 1 = ignore subgroups). values are always normalized at each layer to prevent exponential decrease/increase effects, so results are only relative indications of influence -- if zero_sub_hiddens then intermediate hidden units (indicated by layer_type == HIDDEN) that have sub-threshold values zeroed

  virtual bool  UpdateUnitSpecs(bool force = false);
  // #CAT_Structure update unit specs for entire network (calls layer version of this function)
  virtual bool  UpdateConSpecs(bool force = false);
  // #CAT_Structure update con specs for entire network (calls layer version of this function)
  virtual bool  UpdateAllSpecs(bool force = false);
  // #CAT_Structure update all unit and con specs -- just calls above two functions

  virtual void  ReplaceSpecs(BaseSpec* old_sp, BaseSpec* new_sp);
  // #CAT_Structure replace a spec of any kind, including iterating through any children of that spec
  virtual void  ReplaceSpecs_Gp(const BaseSpec_Group& old_spg, BaseSpec_Group& new_spg);
  // #CAT_Structure replace a specs on two matching spec groups, including iterating through any children of each spec
  virtual int   ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp);
  // #CAT_Structure switch any units/layers using old_sp to using new_sp
  virtual int   ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int   ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp
  virtual int   ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp);
  // #CAT_Structure switch any layers using old_sp to using new_sp

  // wizard construction functions:
  virtual BaseSpec_Group* FindMakeSpecGp(const String& nm, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a given spec group and if not found, make it
  virtual BaseSpec* FindMakeSpec(const String& nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // #CAT_Structure find a given spec and if not found, make it
  virtual BaseSpec* FindSpecName(const String& nm);
  // #CAT_Structure find a given spec by name
  virtual BaseSpec* FindSpecType(TypeDef* td);
  // #CAT_Structure find a given spec by type

  virtual Layer* FindMakeLayer(const String& nm, TypeDef* td = NULL,
                       bool& nw_itm = nw_itm_def_arg, const String& alt_nm = "");
  // #CAT_Structure find a given layer and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
  virtual Layer_Group* FindMakeLayerGroup(const String& nm, TypeDef* td = NULL,
                          bool& nw_itm = nw_itm_def_arg, const String& alt_nm = "");
  // #CAT_Structure find a given layer group and if not found, make it (of default type if NULL) (if nm is not found and alt_nm != NULL, it is searched for)
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

  virtual void  UpdateMaxDispSize();
  // #IGNORE update max_disp_size of network based on current layer layout

  virtual void  SetProjectionDefaultTypes(Projection* prjn);
  // #IGNORE this is called by the projection InitLinks to set its default types: overload in derived algorithm-specific networks to provide appropriate default types

  virtual void  DMemTrialBarrier();
  // #CAT_DMem block all dmem processors at the trial level until everyone reaches this same point in the program flow -- cordinates all the processors at this point -- important for cases where there are interdependencies among processors, where they need to be coordinated going forward -- does nothing if dmem_nprocs <= 1 or not using dmem

#ifdef DMEM_COMPILE
  DMemComm      dmem_net_comm;  // #IGNORE the dmem communicator for the network-level dmem computations (the inner subgroup of units, each of size dmem_nprocs_actual)
  DMemComm      dmem_trl_comm;  // #IGNORE the dmem communicator for the trial-level (each node processes a different set of trials) -- this is the outer subgroup
  DMemShare     dmem_share_units;       // #IGNORE the shared units
  DMemAggVars   dmem_agg_sum;           // #IGNORE aggregation of network variables using SUM op (currently only OP in use -- add others as needed)
  virtual void  DMem_SyncNRecvCons();
  // syncronize number of receiving connections (share set 0)
  virtual void  DMem_SyncNet();
  // #IGNORE synchronize just the netinputs (share set 1)
  virtual void  DMem_SyncAct();
  // #IGNORE synchronize just the activations (share set 2)
  virtual void  DMem_DistributeUnits();
  // #CAT_DMem distribute units to different nodes
  virtual void  DMem_UpdtWtUpdt();
  // #CAT_DMem update wt_update and small_batch parameters for dmem, depending on trl_comm.nprocs
  virtual void  DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void  DMem_PruneNonLocalCons();
  // #IGNORE prune non-local connections from all units: units only have their own local connections
  virtual void  DMem_SumDWts(MPI_Comm comm);
  // #IGNORE sync weights across trial-level dmem by summing delta-weights across processors (prior to computing weight updates)
  virtual void  DMem_AvgWts(MPI_Comm comm);
  // #IGNORE sync weights across trial-level dmem by averaging weight values across processors (this is not mathematically equivalent to anything normally done, but it may be useful in some situations)
  virtual void  DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network variables across procs for trial-level dmem
  virtual void  DMem_SymmetrizeWts();
  // #IGNORE symmetrize the weights (if necessary) by sharing weight values across processors
#else
  virtual void  DMem_SyncNRecvCons() { };
  // #CAT_DMem syncronize number of receiving connections (share set 0)
  virtual void  DMem_DistributeUnits() { };
  // #CAT_DMem distribute units to different nodes
#endif

  override int  Dump_Load_Value(std::istream& strm, taBase* par=NULL);
  override int  Save_strm(std::ostream& strm, taBase* par=NULL, int indent=0);
  override int  Dump_Save_impl(std::ostream& strm, taBase* par=NULL, int indent=0);

  override String       GetTypeDecoKey() const { return "Network"; }

  override bool ChangeMyType(TypeDef* new_type);

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const Network& cp);
  TA_BASEFUNS(Network);

protected:
  override void UpdateAfterEdit_impl();
  override void CheckThisConfig_impl(bool quiet, bool& rval);
  override void CheckChildConfig_impl(bool quiet, bool& rval);

private:
  void  Initialize();
  void  Destroy();
};


// these inline functions depend on having all the structure defined already
// so we include them here, at the very end

#include <BaseCons_inlines>
#include <Unit_inlines>
#include <ConSpec_inlines>

#endif // Network_h
