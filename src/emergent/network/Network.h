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
#include <Weights_List>
#include <TimeUsed>
#include <NetworkThreadMgr>
#include <UnitPtrList>
#include <int_Array>
#include <float_Matrix>
#include <DMemComm>
#include <DMemAggVars>
#include <DataTable_Group>

// NOTE: by including network, you end up including all the network code:
// #include <Connection>
// #include <ConSpec>
// #include <ConGroup>
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

////////////////////////////////////////////////////////
//      Memory structure for version 8.0.0 
//
// The network now owns and manages all the unit vars and the connections
// which are allocated into separate memory structures organized by thread.
//
// The Unit has been split into the structural aspects (name, position, etc) 
// which remain in the Unit class, and UnitVars
// UnitVars contains the algorithm-specific variables -- this must
// contain exclusively floats in a flat list of members -- no subclasses, etc
// it also now directly contains the bias variables -- no point in all the overhead 
// to deal with those separately -- connection code should operate directly on the
// float& variables, so it doesn't care if these are in a connection structure or not

// There can only be one type of UnitVars used within the network -- allocation is
// fully vector based into a big pool of memory per thread.  Computation should
// now also be fully vector-based over units (and connections) where possible
//
// send, recv connection structures are all allocated in separate memory
// not within the Unit structure at all -- methods are avail to access
//
// the ConGroup (formerly BaseCons) object is now just a data structure -- not a taBase
//
// There are 3 levels of memory allocation, all done at the thread level:
// * UnitVars -- array[n_units+1]
// * ConGroups -- one for recv, the other for send -- each in a flat array
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


// this macro can be used instead of NET_THREAD_CALL to just loop directly
// over a thread-level method call -- for cases that are not thread safe
#define NET_THREAD_LOOP(meth) { NetworkThreadCall meth_call((NetworkThreadMethod)(&meth));\
    for(int thr_no=0; thr_no < n_thrs_built; thr_no++) \
      meth_call.call(this, thr_no); }

eTypeDef_Of(NetTiming);

class E_API NetTiming : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Network timers for different network functions
INHERITED(taOBase)
public:
  TimeUsedHR   netin;  // Compute_Netin net input
  TimeUsedHR   act;    // Compute_Act activation
  TimeUsedHR   dwt;    // Compute_dWt weight changes (learning)
  TimeUsedHR   wt;     // Compute_Weights update weights

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(NetTiming);
private:
  void	Initialize()    { };
  void 	Destroy()	{ };
};

eTypeDef_Of(NetTiming_List);

class E_API NetTiming_List : public taList<NetTiming> {
  // #NO_TOKENS #NO_UPDATE_AFTER List of NetTiming objects
INHERITED(taList<NetTiming>)
public:

  TA_BASEFUNS_NOCOPY(NetTiming_List);
private:
  void Initialize()  { SetBaseType(&TA_NetTiming); };
  void Destroy()     { };
};

eTypeDef_Of(NetStatsSpecs);

class E_API NetStatsSpecs : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Network parameters for how stats are computed
INHERITED(taOBase)
public:
  bool          sse_unit_avg;   // #CAT_Statistic compute sse as average sse over units (i.e., divide by total number of target units in layer)
  bool          sse_sqrt;       // #CAT_Statistic take the square root of the SSE, producing a Euclidian distance instead of raw sse (note this cannot technically be added across trials in a linear fashion, as raw sse can)
  float         cnt_err_tol;    // #CAT_Statistic tolerance for computing the count of number of errors over current epoch
  bool          prerr;          // #CAT_Statistic compute precision and recall error values over units, at same time as computing sum squared error (SSE)

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(NetStatsSpecs);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

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

  enum AutoLoadMode {
    NO_AUTO_LOAD,               // Do not automatically load a weights file
    AUTO_LOAD_WTS_0,            // Automatically load weights from the first weights saved weights entry -- that should have the save_with_proj or auto_load flags set
    AUTO_LOAD_FILE,             // Automatically load a weights file named in auto_load_file after loading the project.
  };

  enum NetFlags {               // #BITS flags for network
    NF_NONE             = 0,    // #NO_BIT
    MANUAL_POS          = 0x0001, // disables the automatic cleanup/positioning of layers
    ABS_POS             = 0x0002, // always use absolute positions for layers as the primary positioning, otherwise if not set then layer positions relative to owning layer group are primary and absolute positions are computed relative to them
    NETIN_PER_PRJN      = 0x0004, // compute netinput per projection instead of a single aggregate value across all inputs (which is the default)
    BUILD_INIT_WTS      = 0x0008, // initialize the weights after building the network -- for very large networks, may want to turn this off to save some redundant time
    SAVE_KILLED_WTS     = 0x0010, // if the project is killed while running in a non-interactive mode (e.g., on cluster), save this network's weights (only if network is built and epoch > 0)
    BUILT               = 0x1000, // #READ_ONLY #NO_SAVE is the network built -- all memory allocated, etc
    INTACT              = 0x2000, // #READ_ONLY #NO_SAVE if the network is built, is it also still intact, with all the current params set as they were when it was built?
    BUILT_INTACT        = BUILT | INTACT // #NO_BIT built and intact
  };

  enum NetTextLoc {
    NT_BOTTOM,                  // standard bottom location below network -- extends network "foot" lower below to make text more visible
    NT_TOP_BACK,                // above top-most layer, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_LEFT_BACK,               // at left of network, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_RIGHT_BACK,              // at right of network, at the back of the network depth-wise -- foot is raised as when no net text is visible
    NT_LEFT_MID,                // at left of network, in the middle depth-wise -- foot is raised as when no net text is visible
    NT_RIGHT_MID,               // at right of network, in the middle depth-wise -- foot is raised as when no net text is visible
  };

  enum NetThrLayStats {         // stats that require holding threaded layer-level variables for subsequent aggregation
    SSE,
    PRERR,
    N_NetThrLayStats,
  };

  static taBrainAtlas_List* brain_atlases;  // #HIDDEN #READ_ONLY #NO_SAVE #NO_SHOW_TREE atlases available
  
  DataTable_Group spec_tables;  // #CAT_Structure Tables comparing parent and child specs
  BaseSpec_Group specs;         // #CAT_Structure Specifications for network parameters
  Layer_Group   layers;         // #CAT_Structure Layers or Groups of Layers
  Weights_List  weights;        // #CAT_Structure saved weights objects

  NetFlags      flags;          // #CAT_Structure flags controlling various aspects of network function

  TypeDef*      unit_vars_type; // #CAT_Structure #TYPE_UnitVars type of unit variables object to create in the network -- there can only be ONE type of UnitVars in the entire network, because it is allocated globally!
  TypeDef*      con_group_type; // #CAT_Structure #TYPE_ConGroup type of connection group objects to create in the network -- there can only be ONE type of ConGroup in the entire network, because it is allocated globally!

  AutoBuildMode auto_build;     // #CAT_Structure whether to automatically build the network (make units and connections) after loading or not
  AutoLoadMode  auto_load_wts;
  // #CONDEDIT_OFF_auto_build:NO_BUILD Whether to automatically load a weights file when the Network object is loaded.  It is not possible to save the units, so this can be used to provide pre-configured network for the user (must auto_build network first)
  String        auto_load_file;
  // #CONDSHOW_ON_auto_load_wts:AUTO_LOAD_FILE #FILE_DIALOG_LOAD #COMPRESS #FILETYPE_Weights #EXT_wts file name to auto-load weights file from (any path must be relative to project file)

  taBrainAtlasRef brain_atlas;  // #FROM_LIST_brain_atlases #NO_SAVE The name of the atlas to use for brain view rendering.  Labels from this atlas can be applied to layers' brain_area member.
  String        brain_atlas_name; // #HIDDEN the name of the brain atlas that we're using -- this is what is actually saved b/c the ref is not saveable

  TrainMode     train_mode;     // #CAT_Learning training mode -- determines whether weights are updated or not (and other algorithm-dependent differences as well).  TEST turns off learning
  WtUpdate      wt_update;      // #CAT_Learning #CONDSHOW_ON_train_mode:TRAIN weight update mode: when are weights updated (only applicable if train_mode = TRAIN)
  int           small_batch_n;  // #CONDSHOW_ON_wt_update:SMALL_BATCH #CAT_Learning number of events for small_batch learning mode (specifies how often weight changes are synchronized in dmem)
  int           small_batch_n_eff; // #GUI_READ_ONLY #EXPERT #NO_SAVE #CAT_Learning effective batch_n value = batch_n except for dmem when it = (batch_n / epc_nprocs) >= 1
  NetStatsSpecs stats;          // #CAT_Statistic parameters controling the computation of statistics
  NetworkThreadMgr threads;    // #CAT_Threads parallel threading of network computation
  NetTiming_List net_timing; // #CAT_Statistic timing for different network-level functions -- per thread, plus one summary item at the end

  int           batch;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW batch counter: number of times network has been trained over a full sequence of epochs (updated by program)
  int           epoch;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW epoch counter: number of times a complete set of training patterns has been presented (updated by program)
  int           group;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW group counter: optional extra counter to record sequence-level information (sequence = group of trials)
  int           trial;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW trial counter: number of external input patterns that have been presented in the current epoch (updated by program)
  int           tick;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW tick counter: optional extra counter to record a level of organization below the trial level (for cases where trials have multiple component elements)
  int           cycle;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW cycle counter: number of iterations of activation updating (settling) on the current external input pattern (updated by program)
  float         time;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW the current time, relative to some established starting point, in algorithm-specific units (often miliseconds)
  int           total_trials;   // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW total number of trials counter: number of external input patterns that have been presented since the weights were initialized
  String        group_name;     // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name associated with the current group of trials, if such a grouping is applicable (typically set by a LayerWriter)
  String        trial_name;     // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name associated with the current trial (e.g., name of input pattern, typically set by a LayerWriter)
  String        output_name;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name for the output produced by the network (must be computed by a program)

  float         sse;            // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW sum squared error over the network, for the current external input pattern
  float         sum_sse;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic total sum squared error over an epoch or similar larger set of external input patterns
  Average	avg_sse;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average sum squared error over an epoch or similar larger set of external input patterns
  float         cnt_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns
  float         cur_cnt_err;    // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current cnt_err -- used for computing cnt_err
  float         pct_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns (= cnt_err / n)
  float         pct_cor;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was below cnt_err_tol over an epoch or similar larger set of external input patterns (= 1 - pct_err -- just for convenience for whichever you want to plot)

  PRerrVals     prerr;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_stats.prerr #CAT_Statistic precision and recall error values for the entire network, for the current external input pattern
  PRerrVals     sum_prerr;      // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic precision and recall error values for the entire network, over an epoch or similar larger set of external input patterns -- these are always up-to-date as the system is aggregating, given the additive nature of the statistics
  PRerrVals     epc_prerr;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_stats.prerr #CAT_Statistic precision and recall error values for the entire network, over an epoch or similar larger set of external input patterns

  TimeUsed      train_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing entire training (across epochs) (managed entirely by programs -- not always used)
  TimeUsed      epoch_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing an epoch (managed entirely by programs -- not always used)
  TimeUsed      group_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a group, when groups used (managed entirely by programs -- not always used)
  TimeUsed      trial_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a trial (managed entirely by programs -- not always used)
  TimeUsed      settle_time;    // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a settling (managed entirely by programs -- not always used)
  TimeUsed      cycle_time;     // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for computing a cycle (managed entirely by programs -- not always used)
  TimeUsed      wt_sync_time;   // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic time used for the DMem_SumDWts operation (trial-level dmem, computed by network)
  TimeUsed      misc_time;      // #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic misc timer for ad-hoc use by programs

  bool          needs_wt_sym;   // #HIDDEN #NO_SAVE tmp flag managed by Init_Weights to determine if any connections have the wt_limits.sym flag checked and thus need weight symmetrizing to happen

  Usr1SaveFmt   usr1_save_fmt;  // #CAT_File #EXPERT save network for -USR1 signal: full net or weights
  WtSaveFormat  wt_save_fmt;    // #CAT_File #EXPERT format to save weights in if saving weights

  int           n_units;        // #READ_ONLY #SHOW #CAT_Structure total number of units in the network
  int64_t       n_cons;         // #READ_ONLY #SHOW #CAT_Structure total number of connections in the network
  int           max_prjns;      // #READ_ONLY #EXPERT #CAT_Structure maximum number of prjns per any given layer or unit in the network
  PosVector3i   max_disp_size;  // #AKA_max_size #READ_ONLY #EXPERT #CAT_Structure maximum display size in each dimension of the net
  PosVector2i   max_disp_size2d; // #READ_ONLY #EXPERT #CAT_Structure maximum display size in each dimension of the net -- for 2D display

  //////////////////////////////////////////////////////////////////////////////////
  // Following is full memory structure for the units and connections in the network

  int_Array     active_layers;
  // #NO_SAVE #HIDDEN #CAT_Activation leaf indicies of the active (non-lesioned) layers in the network
  int_Array     active_ungps;
  // #NO_SAVE #HIDDEN #CAT_Activation unit group indicies of the active (non-lesioned) unit groups in the network -- these just count up 0..n-1 for unit groups within layers -- see _layers for relevant layer
  int_Array     active_ungps_layers;
  // #NO_SAVE #HIDDEN #CAT_Activation layer leaf indicies of the active (non-lesioned) unit groups in the network

  UnitPtrList   units_flat;     // #NO_SAVE #READ_ONLY #CAT_Structure flat list of structural Unit's -- first (0 index) is a null unit that is skipped over and should never be computed on -- this is ALL units regardless of unit-level lesion status (but it does account for layer-level lesion status)
  Unit*         null_unit;      // #HIDDEN #NO_SAVE #CAT_Structure unit for the first null unit in the units_flat list -- created by BuildNullUnit() function, specific to each algorithm

  int           n_thrs_built; // #NO_SAVE #READ_ONLY #CAT_Threads number of threads that the network was built for -- must use this number of threads for running network, and rebuild if the number changes
  TypeDef*      unit_vars_built; // #NO_SAVE #READ_ONLY #CAT_Structure #TYPE_UnitVars type of unit variables objects actually built
  TypeDef*      con_group_built; // #NO_SAVE #READ_ONLY #CAT_Structure #TYPE_ConGroup type of con group objects actually built
  int           con_group_size;  // #NO_SAVE #READ_ONLY #CAT_Structure size in bytes of con group objects actually built 
  int           unit_vars_size;  // #NO_SAVE #READ_ONLY #CAT_Threads number of float variables in the unit_vars_built UnitVars
  int           n_units_built;  // #NO_SAVE #READ_ONLY #CAT_Threads number of units built -- actually the n+1 size of units_flat
  int           n_layers_built; // #NO_SAVE #READ_ONLY #CAT_Threads number of active layers when built -- size of active_layers array
  int           n_ungps_built;  // #NO_SAVE #READ_ONLY #CAT_Threads number of active unit groups when built -- size of active_ungpss array
  int           max_thr_n_units; // #NO_SAVE #READ_ONLY #CAT_Threads maximum number of units assigned to any one thread
  int*          units_thrs;   // #IGNORE allocation of units to threads -- array of int[n_units+1], indexed starting at 1 (i.e., flat_idx with 0 = null unit) with each int indicating which thread is responsible for that unit
  int*          units_thr_un_idxs; // #IGNORE allocation of units to threads, thread-specific index for this unit -- array of int[n_units+1], indexed starting at 1 (i.e., flat_idx with 0 = null unit) with each int indicating index in thread-specific memory where that unit lives
  int*          thrs_n_units; // #IGNORE number of units assigned to each thread -- array of int[n_threads_built]
  int**         thrs_unit_idxs; // #IGNORE allocation of units to threads -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing unit->flat_idx indexes of units processed by a given thread (thread-centric complement to units_thrs)
  char**       thrs_units_mem;  // #IGNORE actual memory allocation of UnitVars variables, organized by thread -- array of char*[n_thrs_built], pointing to arrays of char[thrs_n_units[thr_no] * unit_vars_size], containing the units processed by a given thread -- this is the primary memory allocation of units
  int**        thrs_lay_unit_idxs; // #IGNORE allocation of units to layers by threads -- array of int**[n_thrs_built], pointing to arrays of int[n_layers_built * 2], containing  start and end thr_un_idx indexes of units processed by a given thread and a given layer
  int**        thrs_ungp_unit_idxs; // #IGNORE allocation of units to unit groups by threads -- array of int**[n_thrs_built], pointing to arrays of int[n_ungps_built * 2], containing  start and end thr_un_idx indexes of units processed by a given thread and a given unit group
  int          n_lay_stats;     // #IGNORE #DEF_6 number of thread-specific layer-level statistics that require variable memory storage
  int          n_lay_stats_vars; // #IGNORE #DEF_6 number of thread-specific layer-level statistic variables, per stat, available for stats algorithms
  float**      thrs_lay_stats;  // #IGNORE thread-specific layer-level stats variables available for stats routines to do efficient initial pre-computation across units at the thread level, followed by a main-thread integration of the thread-specific values -- array of float*[n_thrs_built] of float[n_lay_stats * n_lay_stats_vars * n_layers_built] -- n_lay_stats_vars is accessed as the inner dimension, then n_layers_built, then n_lay_stats as outer

  int*          units_n_recv_cgps;  // #IGNORE number of receiving connection groups per unit (flat_idx unit indexing, starts at 1)
  int*          units_n_send_cgps;  // #IGNORE number of sending connection groups per unit (flat_idx unit indexing, starts at 1)
  int           n_recv_cgps; // #IGNORE total number of units * recv con groups per unit
  int           n_send_cgps; // #IGNORE total number of units * send con groups per unit

  int**         thrs_units_n_recv_cgps;   // #IGNORE number of receiving connection groups per unit, per thread units (organized 1-to-1 with thrs_units*) -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing number of recv groups per unit
  int**         thrs_units_n_send_cgps;   // #IGNORE number of sending connection groups per unit, per thread units (organized 1-to-1 with thrs_units*) -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing number of send groups per unit
  int*          thrs_n_recv_cgps;       // #IGNORE total number of units * recv con groups per unit, per thread: array of int[n_thrs_built] of units * thrs_units_n_recv_cgps 
  int*          thrs_n_send_cgps;       // #IGNORE total number of units * send con groups per unit, per thread: array of int[n_thrs_built] of units * thrs_units_n_send_cgps 

  char**        thrs_recv_cgp_mem; // #IGNORE memory allocation for ConGroup for all recv connection group objects, by thread -- array of char*[n_thrs_built], pointing to arrays of char[n_recv_cgps[thr_no] * con_group_size], containing the recv ConGroup processed by a given thread -- this is the primary memory allocation of recv ConGroups
  char**        thrs_send_cgp_mem; // #IGNORE memory allocation for ConGroup for all send connection group objects, by thread -- array of char*[n_thrs_built], pointing to arrays of char[n_send_cgps[thr_no] * con_group_size], containing the send ConGroup processed by a given thread -- this is the primary memory allocation of send ConGroups
  int**         thrs_recv_cgp_start; // #IGNORE starting indexes into thrs_recv_cgp_mem, per thread units (organized 1-to-1 with thrs_units*) -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing indexes into thrs_recv_cgp_mem for first recv gp for given unit -- contains 0 for units that have none 
  int**         thrs_send_cgp_start; // #IGNORE starting indexes into thrs_send_cgp_mem, per thread units (organized 1-to-1 with thrs_units*) -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing indexes into thrs_send_cgp_mem for send recv gp for given unit -- contains 0 for units that have none 

  int64_t*      thrs_recv_cons_cnt; // #IGNORE number of floats to allocate to thrs_recv_cons_mem
  int64_t*      thrs_send_cons_cnt; // #IGNORE number of floats to allocate to thrs_send_cons_mem
  float**       thrs_recv_cons_mem; // #IGNORE bulk memory allocated for all of the recv connections, by thread -- array of float*[thrs_recv_cons_cnt[thr_no]]
  float**       thrs_send_cons_mem; // #IGNORE bulk memory allocated for all of the recv connections, by thread -- array of float*[thrs_recv_cons_cnt[thr_no]]

  int*          thrs_own_cons_max_size; // #IGNORE maximum alloc_size of any owning connection group, by thread -- for allocating temp structures..
  int64_t*      thrs_own_cons_tot_size; // #IGNORE total number of owned connections, by thread
  int*          thrs_own_cons_avg_size; // #IGNORE average size of any owning connection group, by thread -- for optimizing computation
  int*          thrs_own_cons_max_vars; // #IGNORE maximum NConVars of any owning connection group, by thread -- for allocating temp structures..
  float*        thrs_pct_cons_vec_chunked; // #IGNORE average percent of connections that are vector chunked (across owned projections and units), by thread
  float         pct_cons_vec_chunked; // #NO_SAVE #READ_ONLY #EXPERT average percent of connections that are vector chunked (across owned projections and units)

  int**         thrs_tmp_chunks;      // #IGNORE tmp con vec chunking memory
  int**         thrs_tmp_not_chunks;  // #IGNORE tmp con vec chunking memory
  float**       thrs_tmp_con_mem;     // #IGNORE tmp con vec chunking memory

  float**       thrs_send_netin_tmp; // #IGNORE #CAT_Threads temporary storage for threaded sender-based netinput computation -- float*[threads] array of float[n_units]

#ifdef DMEM_COMPILE
  int64_t       all_dmem_sum_dwts_size; // #IGNORE #CAT_Threads size of temporary storage for threaded dmem sum dwts sync operation -- master block of all the mem -- this is what is actually allocated
  // float*        all_dmem_sum_dwts_send; // #IGNORE #CAT_Threads temporary storage for threaded dmem sum dwts sync operation -- master block of all the mem -- this is what is actually allocated
  // float*        all_dmem_sum_dwts_recv; // #IGNORE #CAT_Threads temporary storage for threaded dmem sum dwts sync operation -- master block of all the mem -- this is what is actually allocated
  float**       thrs_dmem_sum_dwts_send; // #IGNORE #CAT_Threads temporary storage for threaded dmem sum dwts sync operation -- float*[threads] array of float[thrs_own_cons_tot_size + thrs_n_units] per thread (n_units for bias weights)
  float**       thrs_dmem_sum_dwts_recv; // #IGNORE #CAT_Threads temporary storage for threaded dmem sum dwts sync operation -- float*[threads] array of float[thrs_own_cons_tot_size + thrs_n_units] per thread (n_units for bias weights)
#endif

  ProjectBase*  proj;           // #IGNORE ProjectBase this network is in

  inline void           SetNetFlag(NetFlags flg)   { flags = (NetFlags)(flags | flg); }
  // set flag state on
  inline void           ClearNetFlag(NetFlags flg) { flags = (NetFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasNetFlag(NetFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetNetFlagState(NetFlags flg, bool on)
  { if(on) SetNetFlag(flg); else ClearNetFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  inline bool   ThrInRange(int thr_no, bool err_msg = true) const
  { if(thr_no >= 0 && thr_no < n_thrs_built) return true;
    TestError(err_msg, "ThrInRange", "thread number:", String(thr_no), "out of range");
    return false; }
  // #CAT_Structure test if thread number is in range
  inline bool   UnFlatIdxInRange(int flat_idx, bool err_msg = true) const
  { if(flat_idx >= 1 && flat_idx < n_units_built) return true;
    TestError(err_msg, "UnFlatIdxInRange", "unit flat index number:", String(flat_idx),
              "out of range"); return false; }
  // #CAT_Structure test if unit flat index is in range
  inline bool   ThrUnIdxInRange(int thr_no, int thr_un_idx, bool err_msg = true) const
  { if(ThrInRange(thr_no) && thr_un_idx >= 0 && thr_un_idx < ThrNUnits(thr_no))
      return true;
    TestError(err_msg, "ThrUnIdxInRange", "unit thread index number:", String(thr_un_idx),
              "out of range in thread:", String(thr_no)); return false; }
  // #CAT_Structure test if thread-based unit index is in range

  inline bool   UnRecvConGpInRange(int flat_idx, int recv_idx, bool err_msg = true) const
  { if(UnFlatIdxInRange(flat_idx) && recv_idx >= 0 && recv_idx < UnNRecvConGps(flat_idx))
      return true;
    TestError(err_msg, "UnRecvConGpInRange", "unit recv con group index number:",
              String(recv_idx),
              "out of range in unit flat idx:", String(flat_idx)); return false; }
  // #CAT_Structure test if unit recv con group index is in range
  inline bool   UnSendConGpInRange(int flat_idx, int send_idx, bool err_msg = true) const
  { if(UnFlatIdxInRange(flat_idx) && send_idx >= 0 && send_idx < UnNSendConGps(flat_idx))
      return true;
    TestError(err_msg, "UnSendConGpInRange", "unit send con group index number:",
              String(send_idx),
              "out of range in unit flat idx:", String(flat_idx)); return false; }
  // #CAT_Structure test if unit send con group index is in range
  inline bool   ThrUnRecvConGpInRange(int thr_no, int thr_un_idx, int recv_idx,
                                      bool err_msg = true) const
  { if(ThrUnIdxInRange(thr_no, thr_un_idx)
       && recv_idx >= 0 && recv_idx < ThrUnNRecvConGps(thr_no, thr_un_idx))
      return true;
    TestError(err_msg, "ThrUnRecvConGpInRange", "unit recv con group index number:",
              String(recv_idx),
              "out of range in thread unit idx:", String(thr_un_idx),
              "in thread:", String(thr_no)); return false; }
  // #CAT_Structure test if thread-specified unit recv con group index is in range
  inline bool   ThrUnSendConGpInRange(int thr_no, int thr_un_idx, int send_idx,
                                      bool err_msg = true) const
  { if(ThrUnIdxInRange(thr_no, thr_un_idx)
       && send_idx >= 0 && send_idx < ThrUnNSendConGps(thr_no, thr_un_idx))
      return true;
    TestError(err_msg, "ThrUnSendConGpInRange", "unit send con group index number:",
              String(send_idx),
              "out of range in thread unit idx:", String(thr_un_idx),
              "in thread:", String(thr_no)); return false; }
  // #CAT_Structure test if thread-specified unit send con group index is in range


  inline Unit*  UnFmIdx(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return NULL;
#endif
    return units_flat.FastEl(flat_idx); }
  // #CAT_Structure get the unit from its flat_idx value
  inline Unit*  UnFmIdx_Safe(int flat_idx) const { return units_flat.SafeEl(flat_idx); }
  // #CAT_Structure get the unit from its flat_idx value, with safe range checking (slow -- generally avoid using if possible)

  inline int    UnThr(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_thrs[flat_idx]; }
  // #CAT_Structure thread that owns and processes the given unit (flat_idx)
  inline int    UnThrUnIdx(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_thr_un_idxs[flat_idx]; }
  // #CAT_Structure index in thread-specific memory where that unit lives for given unit (flat_idx)
  inline int    ThrNUnits(int thr_no) const {
#ifdef DEBUG
    if(!ThrInRange(thr_no)) return 0;
#endif
    return thrs_n_units[thr_no]; }
  // #CAT_Structure number of units processed by given thread
  inline int    ThrUnitIdx(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
#endif
    return thrs_unit_idxs[thr_no][thr_un_idx]; }
  // #CAT_Structure flat_idx of unit at given thread, thread-specific unit index (max ThrNUnits()-1)
  inline UnitVars*  ThrUnitVars(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return NULL;
#endif
    return (UnitVars*)(thrs_units_mem[thr_no] + (thr_un_idx * unit_vars_size)); }
  // #CAT_Structure unit variables for unit at given thread, thread-specific unit index (max ThrNUnits()-1)
  inline Unit*      ThrUnit(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return NULL;
#endif
    return UnFmIdx(ThrUnitIdx(thr_no, thr_un_idx)); }
  // #CAT_Structure structural Unit object at given thread, thread-specific unit index (max ThrNUnits()-1)
  inline UnitVars*  UnUnitVars(int flat_idx) const
  { return ThrUnitVars(UnThr(flat_idx), UnThrUnIdx(flat_idx)); }
  // #CAT_Structure unit variables for unit at given unit at flat_idx 
  inline int    ThrLayUnStart(int thr_no, int lay_no)
  { return thrs_lay_unit_idxs[thr_no][2*lay_no]; }
  // #CAT_Structure starting thread-specific unit index for given layer (from active_layers list)
  inline int    ThrLayUnEnd(int thr_no, int lay_no)
  { return thrs_lay_unit_idxs[thr_no][2*lay_no + 1]; }
  // #CAT_Structure ending thread-specific unit index for given layer (from active_layers list) -- this is like the max in a for loop -- valid indexes are < end
  inline Layer* ActiveLayer(int lay_no)
  { return layers.Leaf(active_layers[lay_no]); }
  // #CAT_Structure retrieve actual layer from active_layers list for given layer index
  inline int ActiveUnGp(int ungp_idx)
  { return active_ungps[ungp_idx]; }
  // #CAT_Structure retrieve active unit group index from list of active unit groups
  inline Layer* ActiveUnGpLayer(int ungp_idx)
  { return layers.Leaf(active_ungps_layers[ungp_idx]); }
  // #CAT_Structure retrieve layer for active unit group index from list of active unit groups
  inline int    ThrUnGpUnStart(int thr_no, int lay_no)
  { return thrs_ungp_unit_idxs[thr_no][2*lay_no]; }
  // #CAT_Structure starting thread-specific unit index for given unit group (from active_ungps list)
  inline int    ThrUnGpUnEnd(int thr_no, int lay_no)
  { return thrs_ungp_unit_idxs[thr_no][2*lay_no + 1]; }
  // #CAT_Structure ending thread-specific unit index for given unit group (from active_ungps list) -- this is like the max in a for loop -- valid indexes are < end
  inline float& ThrLayStats(int thr_no, int lay_idx, int stat_var, int stat_type) 
  { return thrs_lay_stats[thr_no]
      [stat_type * n_layers_built * n_lay_stats_vars + lay_idx * n_lay_stats_vars +
       stat_var]; }
  // #IGNORE get layer statistic value for given thread, layer (active layer index), stat variable number (0..n_lay_stats_vars-1 max), and stat type (SSE, PRERR, etc)

  inline int    UnNRecvConGps(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_n_recv_cgps[flat_idx]; }
  // #CAT_Structure number of recv connection groups for given unit at flat_idx
  inline int    UnNSendConGps(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_n_send_cgps[flat_idx]; }
  // #CAT_Structure number of send connection groups for given unit at flat_idx
  inline int    UnNRecvConGpsSafe(int flat_idx) const {
    if(!UnFlatIdxInRange(flat_idx)) return 0;
    return units_n_recv_cgps[flat_idx]; }
  // #CAT_Structure number of recv connection groups for given unit at flat_idx
  inline int    UnNSendConGpsSafe(int flat_idx) const {
    if(!UnFlatIdxInRange(flat_idx)) return 0;
    return units_n_send_cgps[flat_idx]; }
  // #CAT_Structure number of send connection groups for given unit at flat_idx

  inline int    ThrUnNRecvConGps(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
#endif
    return thrs_units_n_recv_cgps[thr_no][thr_un_idx]; }
  // #CAT_Structure number of recv connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  inline int    ThrUnNSendConGps(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
#endif
    return thrs_units_n_send_cgps[thr_no][thr_un_idx]; }
  // #CAT_Structure number of send connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  inline int    ThrUnNRecvConGpsSafe(int thr_no, int thr_un_idx) const {
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
    return thrs_units_n_recv_cgps[thr_no][thr_un_idx]; }
  // #CAT_Structure number of recv connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  inline int    ThrUnNSendConGpsSafe(int thr_no, int thr_un_idx) const {
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
    return thrs_units_n_send_cgps[thr_no][thr_un_idx]; }
  // #CAT_Structure number of send connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  
  inline int    ThrNRecvConGps(int thr_no) const {
#ifdef DEBUG
    if(!ThrInRange(thr_no)) return 0;
#endif
    return thrs_n_recv_cgps[thr_no]; }
  // #CAT_Structure number of recv connection groups as a flat list across all units processed by given thread
  inline int    ThrNSendConGps(int thr_no) const {
#ifdef DEBUG
    if(!ThrInRange(thr_no)) return 0;
#endif
    return thrs_n_send_cgps[thr_no]; }
  // #CAT_Structure number of send connection groups as a flat list across all units processed by given thread

  inline ConGroup* ThrRecvConGroup(int thr_no, int thr_cgp_idx) const
  { return (ConGroup*)(thrs_recv_cgp_mem[thr_no] + (thr_cgp_idx * con_group_size)); }
  // #CAT_Structure recv ConGroup for given thread, thread-specific con-group index 
  inline ConGroup* ThrSendConGroup(int thr_no, int thr_cgp_idx) const
  { return (ConGroup*)(thrs_send_cgp_mem[thr_no] + (thr_cgp_idx * con_group_size)); }
  // #CAT_Structure send ConGroup for given thread, thread-specific con-group index 

  inline ConGroup* ThrUnRecvConGroup(int thr_no, int thr_un_idx, int recv_idx) const {
#ifdef DEBUG
    if(!ThrUnRecvConGpInRange(thr_no, thr_un_idx, recv_idx)) return NULL;
#endif
    return ThrRecvConGroup(thr_no, thrs_recv_cgp_start[thr_no][thr_un_idx] + recv_idx); }
  // #CAT_Structure recv ConGroup for given thread, thread-specific unit index, and recv group index
  inline ConGroup* ThrUnSendConGroup(int thr_no, int thr_un_idx, int send_idx) const {
#ifdef DEBUG
    if(!ThrUnSendConGpInRange(thr_no, thr_un_idx, send_idx)) return 0;
#endif
    return ThrSendConGroup(thr_no, thrs_send_cgp_start[thr_no][thr_un_idx] + send_idx); }
  // #CAT_Structure send ConGroup for given thread, thread-specific unit index, and send group index

  inline ConGroup* RecvConGroup(int flat_idx, int recv_idx) const {
#ifdef DEBUG
    if(!UnRecvConGpInRange(flat_idx, recv_idx)) return NULL;
#endif
    int thr_no = UnThr(flat_idx); 
    return ThrUnRecvConGroup(thr_no, UnThrUnIdx(flat_idx), recv_idx); }
  // #CAT_Structure recv ConGroup for given flat unit index and recv group index number
  inline ConGroup* SendConGroup(int flat_idx, int send_idx) const {
#ifdef DEBUG
    if(!UnSendConGpInRange(flat_idx, send_idx)) return 0;
#endif
    int thr_no = UnThr(flat_idx); 
    return ThrUnSendConGroup(thr_no, UnThrUnIdx(flat_idx), send_idx); }
  // #CAT_Structure send ConGroup for given flat unit index and send index number

  inline ConGroup* ThrUnRecvConGroupSafe(int thr_no, int thr_un_idx, int recv_idx) const {
    if(!ThrUnRecvConGpInRange(thr_no, thr_un_idx, recv_idx)) return NULL;
    return ThrRecvConGroup(thr_no, thrs_recv_cgp_start[thr_no][thr_un_idx] + recv_idx); }
  // #CAT_Structure recv ConGroup for given thread, thread-specific unit index, and recv group index
  inline ConGroup* ThrUnSendConGroupSafe(int thr_no, int thr_un_idx, int send_idx) const {
    if(!ThrUnSendConGpInRange(thr_no, thr_un_idx, send_idx)) return 0;
    return ThrSendConGroup(thr_no, thrs_send_cgp_start[thr_no][thr_un_idx] + send_idx); }
  // #CAT_Structure send ConGroup for given thread, thread-specific unit index, and send group index

  inline ConGroup* RecvConGroupSafe(int flat_idx, int recv_idx) const {
    if(!UnRecvConGpInRange(flat_idx, recv_idx)) return NULL;
    int thr_no = UnThr(flat_idx); 
    return ThrUnRecvConGroup(thr_no, UnThrUnIdx(flat_idx), recv_idx); }
  // #CAT_Structure recv ConGroup for given flat unit index and recv group index number
  inline ConGroup* SendConGroupSafe(int flat_idx, int send_idx) const {
    if(!UnSendConGpInRange(flat_idx, send_idx)) return 0;
    int thr_no = UnThr(flat_idx); 
    return ThrUnSendConGroup(thr_no, UnThrUnIdx(flat_idx), send_idx); }
  // #CAT_Structure send ConGroup for given flat unit index and send index number

  inline float* ThrSendNetinTmp(int thr_no) const 
  { return thrs_send_netin_tmp[thr_no]; }
  // #CAT_Structure temporary sending netinput memory for given thread -- no NETIN_PER_PRJN version
  inline float* ThrSendNetinTmpPerPrjn(int thr_no, int recv_idx) const 
  { return thrs_send_netin_tmp[thr_no] + recv_idx * n_units_built; }
  // #CAT_Structure temporary sending netinput memory for given thread -- NETIN_PER_PRJN version


  static bool net_aligned_malloc(void** ptr, size_t sz);
  // #IGNORE properly (maximally) aligned memory allocation routine to given pointer of given number of bytes -- alignment is (currently) 64 bytes
  static bool net_free(void** ptr);
  // #IGNORE free previously malloc'd memory, and set *ptr = NULL

  virtual void  Build();
  // #BUTTON #CAT_Structure Build the network units and Connect them (calls CheckSpecs/BuildLayers/Units/Prjns and Connect)
    virtual void  CheckSpecs();
    // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig
    virtual void  BuildLayers();
    // #MENU #MENU_ON_Structure #CAT_Structure Build any network layers that are dynamically constructed
    virtual void  BuildPrjns();
    // #MENU #CAT_Structure Build any network prjns that are dynamically constructed
    virtual void  BuildUnits();
    // #IGNORE Build the network units in layers according to geometry
    virtual void  BuildNullUnit();
    // #IGNORE make the null_unit unit
    virtual void  BuildUnitsFlatList();
    // #IGNORE build flat list of units
    virtual void  FreeUnitConGpThreadMem();
    // #IGNORE free all of the thread-based unit, connection-group memory -- also calls FreeConThreadMem()
    virtual void  FreeConThreadMem();
    // #IGNORE free all of the thread-based connection memory
    virtual void  AllocUnitConGpThreadMem();
    // #IGNORE allocate all of the thread-based unit, connection group memory
    virtual void  InitUnitThreadIdxs(int thr_no);
    // #IGNORE initialize thread-specific index structures
    virtual void  InitUnitConGpThreadMem(int thr_no);
    // #IGNORE initialize thread-based unit and con group memory -- should assign to thread
    virtual int   FindActiveLayerIdx(Layer* lay, const int st_idx);
    // #IGNORE find the index in active_layers of this layer -- uses st_idx to seed a bidirectional search, so if you have any hint as to where it might be found, this results in considerable speedup
    virtual int   FindActiveUnGpIdx(Layer* lay, const int ungp_idx, const int st_idx);
    // #IGNORE find the index in active_ungps of this unit group within layer -- uses st_idx to seed a bidirectional search, so if you have any hint as to where it might be found, this results in considerable speedup
    virtual void  AllocSendNetinTmp();
    // #IGNORE allocate send_netin_tmp for netin computation
    virtual void  InitSendNetinTmp_Thr(int thr_no);
    // #IGNORE init send_netin_tmp for netin computation
  virtual void  Connect();
  // #IGNORE Connect this network according to projections on Layers -- must be done as part of Build to ensure proper sync
    virtual void  Connect_Sizes();
    // #IGNORE first pass of connecting -- sets up all the Cons objects within units, and computes all the target allocation size information (done by projection specs)
    virtual void  Connect_Alloc();
    // #IGNORE second pass of connecting -- allocate all the memory for all the connections -- managed by the Network and done by thread
    virtual void  Connect_AllocSizes_Thr(int thr_no);
    // #IGNORE second pass of connecting -- allocate all the memory for all the connections -- get the total sizes needed
    virtual void  Connect_Alloc_Thr(int thr_no);
    // #IGNORE second pass of connecting -- dole out the allocated memory to con groups
    virtual void  Connect_Cons();
    // #IGNORE third pass of connecting -- actually make the connections -- done by projection specs
    virtual void  Connect_VecChunk_Thr(int thr_no);
    // #IGNORE fourth pass of connecting -- organize connections into optimal vectorizable chunks
    virtual void  Connect_UpdtActives_Thr(int thr_no);
    // #IGNORE update the active flag status of all connections

  virtual void  UnBuild();
  // #BUTTON #CAT_Structure un-build the network -- remove all units and connections -- network configuration is much faster when operating on an un-built network
  
  virtual bool    AutoBuild();
  // #CAT_Structure called by ProjectBase::AutoBuildNets() -- does auto-building and loading of weight files after project is loaded
    
  virtual String  MemoryReport(bool print = true);
  // #CAT_Statistic report about memory allocation for the network

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
  virtual void  CountCons();
  // #CAT_Structure count connections for all units in network
  virtual bool  RecvOwnsCons() { return true; }
  // #CAT_Structure does the receiver own the connections (default) or does the sender?

  virtual void  ConnectUnits(Unit* u_to, Unit* u_from=NULL, bool record=true,
                             ConSpec* conspec=NULL);
  // #CAT_Structure connect u1 so that it recieves from u2. Create projection if necessary

  virtual void  RemoveCons();
  // #MENU #MENU_ON_Structure #CONFIRM #MENU_SEP_BEFORE #CAT_Structure Remove all connections in network
    virtual void  RemoveCons_Thr(int thr_no);
    // #IGNORE thread level remove cons
  virtual void  RemoveUnits();
  // #MENU #CONFIRM #CAT_Structure Remove all units in network -- also calls RemoveCons()

  virtual void  UpdatePrjnIdxs();
  // #CAT_Structure fix the projection indexes of the connection groups (recv_idx, send_idx)

  virtual void  GetWeightsFromGPU() { };
  // #IGNORE this is called before any network-level function that operates on the weights (except Init_Weights()) -- overload to get weights back from a GPU device (e.g., CUDA)
  virtual void  SendWeightsToGPU() { };
  // #IGNORE this is called after any network-level function that operates on the weights -- overload to send weights to the GPU device (e.g., CUDA)

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

  virtual void  SaveToWeights(Weights* wts);
  // #BUTTON #MENU #NULL_OK #NULL_TEXT_NewWeightsObj write weight values out to given weights object (NULL = make a new one)
  virtual bool  LoadFmWeights(Weights* wts, bool quiet = false);
  // #MENU #FROM_LIST_weights load weight values from given weights object

  virtual void  SaveToFirstWeights();
  // write weight values out to the first Weights object in the weights list -- if it does not yet exist, then create it -- useful for basic save and load of one cached set of weights, as compared to a situation where you need to manage multiple different weight sets
  virtual bool  LoadFmFirstWeights(bool quiet = false);
  // load weight values from first Weights object -- if it does not yet exist, emit an error message -- useful for basic save and load of one cached set of weights, as compared to a situation where you need to manage multiple different weight sets

//NOTE: if any of the Build or Connect are to be extended, the code must be rewritten by
//  calling an inner extensible virtual _impl

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning
  //    Many functions operate directly on the units via threads, with
  //    optional call through to the layers for any layer-level subsequent processing

  virtual void  Init_InputData();
  // #CAT_Activation Initializes external and target inputs
    virtual void  Init_InputData_Thr(int thr_no);
    // #IGNORE
  virtual void  Init_Acts();
  // #MENU #MENU_ON_State #MENU_SEP_BEFORE #CAT_Activation initialize the unit activation state variables
    virtual void  Init_Acts_Thr(int thr_no);
    // #IGNORE
  virtual void  Init_dWt();
  // #CAT_Learning Initialize the weight change variables
    virtual void  Init_dWt_Thr(int thr_no);
    // #IGNORE
  virtual void  Init_Weights();
  // #BUTTON #MENU #CONFIRM #CAT_Learning Initialize the weights -- also inits acts, counters and stats -- does unit level threaded and then does Layers after
    virtual void  Init_Weights_Thr(int thr_no);
    // #IGNORE
    virtual void Init_Weights_renorm();
    // #IGNORE renormalize weights after init, before sym
    virtual void Init_Weights_renorm_Thr(int thr_no);
    // #IGNORE renormalize weights after init, before sym
    virtual void Init_Weights_sym(int thr_no);
    // #IGNORE symmetrize weights after first init pass, called when needed
    virtual void Init_Weights_post();
    // #CAT_Learning post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc) -- this MUST be called after any external modifications to the weights, e.g., the TransformWeights or AddNoiseToWeights calls on any lower-level objects (layers, units, con groups)
    virtual void Init_Weights_post_Thr(int thr_no);
    // #IGNORE
    virtual void Init_Weights_Layer();
    // #CAT_Learning call layer-level init weights functions -- after all unit-level inits

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
    virtual void  Compute_Netin_Thr(int thr_no);
    // #IGNORE
  virtual void  Send_Netin();
  // #CAT_Activation sender-based computation of net input: weighted activation from other units
    virtual void  Send_Netin_Thr(int thr_no);
    // #IGNORE
    inline bool NetinPerPrjn() { return HasNetFlag(NETIN_PER_PRJN); }
    // #CAT_Activation is this network configured to compute net input on a per-prjn basis?
  virtual void  Compute_Act();
  // #CAT_Activation Compute Activation based on net input
    virtual void  Compute_Act_Thr(int thr_no);
    // #IGNORE
  virtual void  Compute_NetinAct();
  // #CAT_Activation compute net input from other units and then our own activation value based on that -- use this for feedforward networks to propagate activation through network in one compute cycle
    virtual void  Compute_NetinAct_Thr(int thr_no);
    // #IGNORE

  virtual void  Compute_dWt();
  // #CAT_Learning compute weight changes -- the essence of learning
    virtual void  Compute_dWt_Thr(int thr_no);
    // #IGNORE

  virtual bool  Compute_Weights_Test(int trial_no);
  // #CAT_Learning check to see if it is time to update the weights based on the given number of completed trials (typically trial counter + 1): if ON_LINE, always true; if SMALL_BATCH, only if trial_no % batch_n_eff == 0; if BATCH, never (check at end of epoch and run then)
  virtual void  Compute_Weights();
  // #CAT_Learning update weights for whole net: calls DMem_SumDWts before doing update if in dmem mode
    virtual void  Compute_Weights_Thr(int thr_no);
    // #IGNORE

  virtual void  Compute_SSE(bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activations vs targets over the entire network -- optionally taking the average over units, and square root of the final results
    virtual void Compute_SSE_Thr(int thr_no);
    // #IGNORE
    virtual void Compute_SSE_Agg(bool unit_avg = false, bool sqrt = false);
    // #IGNORE
  virtual void  Compute_PRerr();
  // #CAT_Statistic compute precision and recall error statistics over entire network -- true positive, false positive, and false negative -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r), specificity, fall-out, mcc.
    virtual void Compute_PRerr_Thr(int thr_no);
    // #IGNORE
    virtual void Compute_PRerr_Agg();
    // #IGNORE

  virtual Layer* NewLayer();
  // #BUTTON create a new layer in the network, using default layer type

  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this network
  virtual void  RemoveMonitors();
  // #CAT_ObjectMgmt Remove monitoring of all objects in all processes associated with parent project
  virtual void  UpdateMonitors();
  // #CAT_ObjectMgmt Update monitoring of all objects in all processes associated with parent project
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
  virtual void  LayerPos_GridLayout_2d(int x_space = 2, int y_space = 3,
                                       int gp_grid_x = -1, int lay_grid_x = -1);
  // #MENU_BUTTON #MENU_ON_NetView #CAT_Structure for the 2D layer positions: arrange layers and layer subgroups into a grid with given spacing, and you can optionally constrain the x (horizontal) dimension of the grid for the subgroups within the network or layers within groups (or just the layers if no subgroups) if gp_grid_x > 0 or layer_grid_x > 0
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

  virtual void      SpecCompare(BaseSpec* parent_spec);
  // creates a table with a column of values for the parent spec and each child spec - values are shown if #CONDSHOW is on and if a child spec also checks override - if both are true the value is displayed
  virtual void      AddChildToSpecCompareTable(DataTable* spec_table, BaseSpec* spec);
  // #IGNORE called recursively to add a column for all child specs to spec data table -- called by SpecCompare() -- this table is not updated -- call again if you change specs!
  virtual bool      ShowSpecMember(MemberDef* spec_md, MemberDef* spec_member_md);
  // #IGNORE returns true for members that are user editable and are visible in spec panel
  virtual bool      ShowSpecMemberValue(MemberDef* spec_member_md, TypeDef* typ, taBase* base);
  // #IGNORE checks CONDSHOW
  virtual void      WriteSpecMbrNamesToTable(DataTable* spec_table, BaseSpec* spec);
  // #IGNORE writes spec member names to a spec compare table -- See SpecCompare()
  virtual void      WriteSpecMbrValsToTable(DataTable* spec_table, BaseSpec* spec, bool is_child);
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
  virtual void  DMem_SumDWts_ToTmp_Thr(int thr_no);
  // #IGNORE copy to temp send buffer for sending, per thread
  virtual void  DMem_SumDWts_FmTmp_Thr(int thr_no);
  // #IGNORE copy from temp recv buffer, per thread
  virtual void  DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network variables across procs for trial-level dmem
#endif

  virtual void  BgRunKilled();
  // #IGNORE called when program is quitting prematurely and is not in an interactive mode - save network if SAVE_KILLED_WTS flag is set
  
  int  Dump_Load_Value(std::istream& strm, taBase* par=NULL) override;
  int  Save_strm(std::ostream& strm, taBase* par=NULL, int indent=0) override;
  int  Dump_Save_impl(std::ostream& strm, taBase* par=NULL, int indent=0) override;

  String       GetTypeDecoKey() const override { return "Network"; }
  String       GetToolbarName() const override { return "network"; }

  bool         ChangeMyType(TypeDef* new_type) override;
  taBase*      ChooseNew(taBase* origin) override;
  bool         HasChooseNew() override { return true; }

  void  InitLinks() override;
  void  CutLinks() override;
  void  Copy_(const Network& cp);
  TA_BASEFUNS(Network);

protected:
  void UpdateAfterEdit_impl() override;
  void CheckThisConfig_impl(bool quiet, bool& rval) override;
  void CheckChildConfig_impl(bool quiet, bool& rval) override;

private:
  void  Initialize();
  void  Destroy();
};


// these inline functions depend on having all the structure defined already
// so we include them here, at the very end

#include <ConGroup_inlines>
#include <Unit_inlines>
#include <ConSpec_inlines>

#endif // Network_h
