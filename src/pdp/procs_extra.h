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



// procs_extra.h
// extra process (stats, schedule, etc) objects derived from basic ones that are
// included in standard pdp library

#ifndef procs_extra_h
#define procs_extra_h

#include "sched_proc.h"

//////////////////////////////////
//	Basic Processes		//
//////////////////////////////////

class ScriptProcess : public Process {
  // a process for use with scripts (has s_args)
public:
  SArg_Array		s_args;		// string-valued arguments to pass to script

  virtual void	Interact();
  // #BUTTON change to this shell in script (terminal) window to interact, debug etc script
  virtual void	Compile();
  // #BUTTON compile script from script file into internal runnable format

  void	UpdateAfterEdit();
  void 	InitLinks();
  void	Copy_(const ScriptProcess& cp);
  COPY_FUNS(ScriptProcess, Process);
  TA_BASEFUNS(ScriptProcess);
};

class SaveNetsProc : public Process {
  // saves networks with network.name + counters (batch, epoch)
public:
  void			C_Code();

  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_BASEFUNS(SaveNetsProc);
};

class SaveWtsProc : public Process {
  // saves weights with network.name + counters (batch, epoch)
public:
  void			C_Code();

  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_BASEFUNS(SaveWtsProc);
};

class LoadWtsProc : public Process {
  // reads in a set of weights from specified file, as a way to initialize weights
public:
  String		weights_file; // the file name for the weights file to read in

  void			C_Code();

  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_BASEFUNS(LoadWtsProc);
};

class InitWtsProc : public Process {
  // initialize the network's weights (InitWtState)
public:
  void			C_Code();

  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_BASEFUNS(InitWtsProc);
};


//////////////////////////////////
//	Schedule Processes	//
//////////////////////////////////

class SyncEpochProc : public EpochProcess {
  // Epoch that runs two different sub-processes
public:
  Network*		second_network;	  // #CONTROL_PANEL the network for the second trial process
  TypeDef*		second_proc_type; // #TYPE_SchedProcess type of second process
  SchedProcess*		second_proc; 	  // #CONTROL_PANEL second process to call

  void		Init();
  void		Init_flag();
  void		Init_force();
  void		Loop();		// do on-line, small_batch here
  void		Final();	// batch wt update here

  SchedProcess*	FindSubProc(TypeDef* td);
  void		CreateSubProcs(bool update);

  void	SetDefaultPNEPtrs();

  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const SyncEpochProc& cp);
  COPY_FUNS(SyncEpochProc, EpochProcess);
  TA_BASEFUNS(SyncEpochProc);
};

class GridSearchBatch : public BatchProcess {
  // Increments param_path parameter over batches to search parameter space in equal increments
public:
  float		start_val;	// #CONTROL_PANEL start value of parameter being searched
  float		inc_val;	// #CONTROL_PANEL increment of parameter being searched
  float		cur_val;	// #CONTROL_PANEL #IV_READ_ONLY current value of parameter based on batch value
  String	param_path;	// path to the parameter (starting at the project)

  virtual void	Compute_CurVal(); // compute the current value from batch counter
  virtual void	SetParamVal();	  // set parameter value to be current value

  void		Init_impl();
  void		Loop();

  void		GetCntrDataItems();
  void		GenCntrLog(LogData* ld, bool gen);

  void	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(GridSearchBatch);
  COPY_FUNS(GridSearchBatch, BatchProcess);
  TA_BASEFUNS(GridSearchBatch);
};

class SameSeedBatch : public BatchProcess {
  // Stores a list of random seeds that are used at start of each batch run -- ensures each batch run starts with the same seed
public:
  taBase_List	rnd_seeds;	// the random seeds
  int		in_goto_val;	// #READ_ONLY #NO_SAVE currently inside a GoTo call (with this val): affects seed usage

  virtual void	InitSeeds(int n_seeds);	// #BUTTON initialize the seeds for subsequent use
  virtual void	UseCurrentSeed(); 	// use the current seed based on batch counter value

  void		Init_impl();
  void		ReInit();
  void		Loop();
  void 		GoTo(int goto_val);
  void		UpdateState();

  void	Initialize();
  void	Destroy();
  void	InitLinks();
  void	CutLinks();
  SIMPLE_COPY(SameSeedBatch);
  COPY_FUNS(SameSeedBatch, BatchProcess);
  TA_BASEFUNS(SameSeedBatch);
};

//////////////////////////////////
//	Fork and Bridge		//
//////////////////////////////////

class ForkProcess : public SchedProcess {
  // Runs two different sub-processes
public:
  Network*		second_network;	  // the network for the second process
  Environment*		second_environment; // the environment for the second process
  TypeDef*		second_proc_type; // #TYPE_SchedProcess #NULL_OK type of second process
  SchedProcess*		second_proc; 	  // second process to call

  void		Init();
  void		Init_flag();
  void		Init_force();
  void		Loop();
  bool		Crit()	{ return true; }

  SchedProcess*	FindSubProc(TypeDef* td);
  void		CreateSubProcs(bool update);

  void	SetDefaultPNEPtrs();

  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ForkProcess& cp);
  COPY_FUNS(ForkProcess, SchedProcess);
  TA_BASEFUNS(ForkProcess);
};

class BridgeProcess : public Process {
  // a bridge that connects two different networks together by copying values
public:
  enum BridgeDirection {
    ONE_TO_TWO,			// first network (network) copies to second_network
    TWO_TO_ONE 			// second_network copies to first one (network)
  };

  Network*	second_network;	  // the other network to bridge to
  Layer*	src_layer;	  // #HIDDEN #NO_SAVE the actual source layer
  Layer*	trg_layer;	  // #HIDDEN #NO_SAVE the actual target layer
  BridgeDirection direction;	  // direction to copy in
  String	src_layer_nm;	  // name of the source layer
  String	trg_layer_nm;	  // name of the target layer
  String	src_variable;	  // the source variable (member) to copy
  String	trg_variable;	  // the target variable (member) to copy
  Unit::ExtType	trg_ext_flag;	  // flag to apply to the target units

  virtual bool	SetLayerPtrs();	// set the layer pointers from names, direction

  void		C_Code();

  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const BridgeProcess& cp);
  COPY_FUNS(BridgeProcess, Process);
  TA_BASEFUNS(BridgeProcess);
};

class MultiEnvProcess : public SchedProcess {
  // run subprocess over multiple environments, indexed by counter
public:
  Environment_List	environments;   // #LINK_GROUP list of environments to process
  bool			use_subctr_max;	// determines whether subctr_max values are actually used
  int_Array		subctr_max; 	// sets the counter max value for the subprocess under this one for each environment (e.g., if subproc is nepochproc, sets numb of epochs per enviro)
  Counter		env;            // current environment number

  void 		Init_impl();
  void 		UpdateState();
  virtual void 	SetCurEnvironment();    // set up environment from the counter

  void UpdateAfterEdit();
  void Initialize();
  void Destroy();
  void InitLinks();
  void CutLinks();
  void Copy_(const MultiEnvProcess& cp);
  COPY_FUNS(MultiEnvProcess, SchedProcess);
  TA_BASEFUNS(MultiEnvProcess);
};

class PatternFlagProcess : public SchedProcess {
  // iteratively sets/resets pattern flag at index = counter for all patterns, e.g. to determine sensitivity to given input
public:
  int          		pattern_no;     // index of pattern to update
  PatternSpec::PatFlags flag;   	// flag to be set (or unset if invert)
  bool			invert;		// unset the flag instead of setting it
  Counter		val_idx;	// index of current pattern value to be flagged

  void 		Init_impl();
  void 		Loop();
  virtual void 	SetCurFlags(int* old_flag);
  // set up flags for current counter (sets old_flag if provided to prev flag val)
  virtual void 	ClearCurFlags(int* old_flag);
  // clear flags for current counter (resetting to old_flag if provided)

  void UpdateAfterEdit();
  void Initialize();
  void Destroy()	{ };
  void InitLinks();
  void Copy_(const PatternFlagProcess& cp);
  COPY_FUNS(PatternFlagProcess, SchedProcess);
  TA_BASEFUNS(PatternFlagProcess);
};


//////////////////////////////////
//	Stats Processes		//
//////////////////////////////////

class ClosestEventStat : public Stat {
  // ##COMPUTE_IN_TrialProcess gets the closest event to the current output activity pattern
public:
  TrialProcess* trial_proc;	// #READ_ONLY #NO_SAVE current trial process
  float_RArray::DistMetric cmp_type;	// comparison type to perform to compute distance
  float		dist_tol;	// #CONDEDIT_ON_cmp_type:SUM_SQUARES,EUCLIDIAN,HAMMING tolerance value for distance functions
  bool		norm;		// #CONDEDIT_OFF_cmp_type:COVAR,CORREL,CROSS_ENTROPY whether to normalize (distance, inner prod)
  int		subgp_no;	// if not -1, then indicates which subgroup of units within layer
  StatVal	dist;		// distance from closest event
  StatVal	ev_nm;		// name of event which was closest
  StatVal	sm_nm;		// 1 or 0 depending on whether the name matched cur event
  float_RArray	lay_act;	// #HIDDEN #NO_SAVE buffer for layer activity

  void		InitStat();
  void		Init();
  bool		Crit();

  void		Layer_Run();
  void		Layer_Stat(Layer* lay);

  void		RecvCon_Run(Unit*)	{}; // don't do these!
  void		SendCon_Run(Unit*)	{};

  void		NameStatVals();

  void		CopyPNEPtrs(Network* net, Environment* env);
  // don't copy the environment, so user can select a different env for testing

  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void 	CutLinks();
  void	Copy_(const ClosestEventStat& cp);
  COPY_FUNS(ClosestEventStat, Stat);
  TA_BASEFUNS(ClosestEventStat);
};

class CyclesToSettle : public Stat {
  // ##COMPUTE_IN_TrialProcess ##LOOP_STAT Records number of cycles it took to settle
public:
  SettleProcess* settle;	// #READ_ONLY #NO_SAVE settle process to record
  StatVal	cycles;		// number of cycles to settle

  virtual void	GetSettleProc();

  void		Layer_Run()	{ }; // don't do anything!!

  void		Network_Stat();
  void		InitStat();
  void		Init();
  bool		Crit();

  void		NameStatVals();

  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const CyclesToSettle& cp);
  COPY_FUNS(CyclesToSettle, Stat);
  TA_BASEFUNS(CyclesToSettle);
};

class ActThreshRTStat : public Stat {
  // ##COMPUTE_IN_SettleProcess ##LOOP_STAT records reaction-time in terms of number of cycles it took for max act in layer to exceed a threshold (doesn't necc stop settling though, unless stopcrit is set!)
public:
  SettleProcess* settle;	// #READ_ONLY #NO_SAVE settle process to record
  float		act_thresh;	// threshold activation level -- rt_cycles are updated until layer max_act >= thresh (also auto sets max_act.stopcrit.val)
  StatVal	max_act;	// maximum activation of units in layer -- computed continuously, if stopcrit set here then process will actually stop at rt threshold
  StatVal	rt_cycles;	// number of cycles of settling prior to max_act.val >= act_tresh
  bool		crossed_thresh;	// #READ_ONLY #NO_SAVE true if already crossed threshold

  virtual void	GetSettleProc();

  void		Layer_Run()	{ }; // don't do anything!!

  void		Network_Stat();
  void		InitStat();
  void		Init();
  bool		Crit();

  void		NameStatVals();

  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const ActThreshRTStat& cp);
  COPY_FUNS(ActThreshRTStat, Stat);
  TA_BASEFUNS(ActThreshRTStat);
};

class ScriptStat : public Stat {
  // Use this stat for custom script-based stats
public:
  StatVal_List	vals;		// put stat results in this group
  SArg_Array	s_args;		// string-valued arguments to pass to script

  void		InitStat();
  void		Init();
  bool		Crit();

  void		NameStatVals();

  virtual void	Interact();
  // #BUTTON change to this shell in script (terminal) window to interact, debug etc script
  virtual void	Compile();
  // #BUTTON compile script from script file into internal runnable format

  void	UpdateAfterEdit();
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void 	InitLinks();
  void	Copy_(const ScriptStat& cp);
  COPY_FUNS(ScriptStat, Stat);
  TA_BASEFUNS(ScriptStat);
};

class CompareStat : public Stat {
  // An aggregator-like stat that compares the output of two other stats
public:
  Stat*		stat_1;		// first comparison stat
  Stat*		stat_2;		// second comparison stat
  float_RArray::DistMetric cmp_type;	// comparison type to perform
  StatVal	cmp;		// comparison value
  float		dist_tol;	// #CONDEDIT_ON_cmp_type:SUM_SQUARES,EUCLIDIAN,HAMMING tolerance value for distance functions
  bool		norm;		// #CONDEDIT_OFF_cmp_type:COVAR,CORREL,CROSS_ENTROPY whether to normalize (distance, inner prod)
  SimpleMathSpec pre_proc_1;	// step 1 of pre-processing to apply before comparision
  SimpleMathSpec pre_proc_2;	// step 2 of pre-processing to apply before comparision
  SimpleMathSpec pre_proc_3;	// step 3 of pre-processing to apply before comparision
  float_RArray	svals_1;	// #HIDDEN #NO_SAVE stat_1 values
  float_RArray	svals_2;	// #HIDDEN #NO_SAVE stat_2 values

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  virtual void	GetStatVals(Stat* st, float_RArray& sv);
  // get stat vals into an array

  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void 	CutLinks();
  void 	Copy_(const CompareStat& cp);
  COPY_FUNS(CompareStat, Stat);
  TA_BASEFUNS(CompareStat);
};

class ProjectionStat : public Stat {
  // #BUTROWS_1 projects values from another stat along a given vector according to given distance metric
public:
  Stat*		stat;		// stat to get values to project
  float_RArray	prjn_vector;	// vector of values to project along
  float_RArray::DistMetric dist_metric;	// distance metric for projection (INNER_PROD = standard metric for projections)
  float		dist_tol;	// #CONDEDIT_ON_dist_metric:SUM_SQUARES,EUCLIDIAN,HAMMING tolerance value for distance metric
  bool		dist_norm;	// #CONDEDIT_OFF_dist_metric:COVAR,CORREL,CROSS_ENTROPY whether to normalize distances (distance, inner prod)
  StatVal	prjn;		// projection value
  float_RArray	svals;		// #HIDDEN #NO_SAVE stat values

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  virtual void	GetStatVals(Stat* st, float_RArray& sv);
  // get stat vals into an array

  virtual void	VecFmPCA(Environment* env, int pat_no, int pca_component);
  // #BUTTON get projection vector from principal components analysis on pattern number pat_no, using specified component
  virtual void	VecFmEvent(Event* event, int pat_no);
  // #BUTTON get projection vector from given event

  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void 	CutLinks();
  void 	Copy_(const ProjectionStat& cp);
  COPY_FUNS(ProjectionStat, Stat);
  TA_BASEFUNS(ProjectionStat);
};

class ComputeStat : public Stat {
  // performs simple math computations on one or two stats, aggs the results according to net_agg
public:
  Stat*		stat_1;		// first comparison stat
  Stat*		stat_2;		// second comparison stat (optional)
  StatVal	cpt;		// aggregated computed value (if not COPY)
  SimpleMathSpec pre_proc_1;	// step 1 of pre-processing to apply to each stat before computing
  SimpleMathSpec pre_proc_2;	// step 2 of pre-processing to apply to each stat before computing
  SimpleMathSpec pre_proc_3;	// step 3 of pre-processing to apply to each stat before computing
  SimpleMathSpec compute_1;	// step 1 of computation: stat1 is val stat2 is arg
  SimpleMathSpec compute_2;	// step 2 of computation: stat1 is val stat2 is arg
  SimpleMathSpec compute_3;	// step 3 of computation: stat1 is val stat2 is arg
  float_RArray	svals_1;	// #HIDDEN #NO_SAVE stat_1 values
  float_RArray	svals_2;	// #HIDDEN #NO_SAVE stat_2 values

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  virtual void	GetStatVals(Stat* st, float_RArray& sv);
  // get stat vals into an array

  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void 	CutLinks();
  void 	Copy_(const ComputeStat& cp);
  COPY_FUNS(ComputeStat, Stat);
  TA_BASEFUNS(ComputeStat);
};

class CopyToEnvStat : public Stat {
  // an aggregator-like stat that copies stat values from source stat to data environment
public:
  enum	AccumScope {		// over what scope of processing to accumulate data
    SUPER,			// accumulate over the sched process just above the one in hierarchy (super proc) that owns this stat
    OWNER,			// accumulate over the sched process that owns this stat
    TRAIN,			// accumulate over the training process (train must be ABOVE this stat in hierarchy)
    EPOCH,			// accumulate over the epoch process in this hierarchy (epoch must be ABOVE this stat in hierarchy)
    SEQUENCE,			// accumulate over the sequence process in this hierarcy (sequence must be ABOVE this stat in hierarchy)
    SETTLE			// accumulate over the settle process in this hierarchy (settle must be ABOVE this stat in hierarchy)
  };

  Stat*		stat;		// stat to get copy vals from
  Environment* 	data_env;	// environment to store data in
  AccumScope	accum_scope;	// over what scope of processing to accumulate data?
  SimpleMathSpec pre_proc_1;	// step 1 of pre-processing to apply before storage
  SimpleMathSpec pre_proc_2;	// step 2 of pre-processing to apply before storage
  SimpleMathSpec pre_proc_3;	// step 3 of pre-processing to apply before storage
  float_RArray	svals;		// #HIDDEN #NO_SAVE stat values
  int		evt_idx;	// #READ_ONLY #NO_SAVE current event index
  int_Array	last_ctr_vals;	// #READ_ONLY #NO_SAVE last accumulation counter values for all procs up to accum proc

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  virtual void	InitEnv();	// #BUTTON initalize environment to hold data
  virtual void	GetStatVals(Stat* st, float_RArray& sv);
  // get stat vals into an array
  virtual SchedProcess*	GetAccumProc();	// get the accumulator-scope process
  virtual bool	InitEvtIdx();	// determine if event index needs to be initialized

  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void 	CutLinks();
  void 	Copy_(const CopyToEnvStat& cp);
  COPY_FUNS(CopyToEnvStat, Stat);
  TA_BASEFUNS(CopyToEnvStat);
};

class EpochCounterStat : public Stat {
  // ##FINAL_STAT gets the epoch counter from the network
public:
  StatVal	epoch;

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  void		NameStatVals();

  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const EpochCounterStat& cp);
  COPY_FUNS(EpochCounterStat, Stat);
  TA_BASEFUNS(EpochCounterStat);
};

class ProcCounterStat : public Stat {
  // ##FINAL_STAT gets the process counters from a different process hierarchy
public:
  SchedProcess*	proc;		// process to get counters from
  StatVal_List	counters;	// group of counters of data

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  virtual bool	NeedsUpdated();

  void		NameStatVals();

  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const ProcCounterStat& cp);
  COPY_FUNS(ProcCounterStat, Stat);
  TA_BASEFUNS(ProcCounterStat);
};

class MaxActTrgStat : public Stat {
  // ##COMPUTE_IN_TrialProcess 0-1 error statistic, 1 if unit with max act has trg = 1
public:
  StatVal	mxat;			// max activation = target 0-1 err value

  void		RecvCon_Run(Unit*)	{ }; // don't do these!
  void		SendCon_Run(Unit*)	{ };

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Init();
  void		Layer_Run();		// only compute on TARG layers
  void 		Layer_Stat(Layer* lay);
  void		Unit_Run(Layer*)	{ }; // don't loop over units

  void		NameStatVals();

  void	Initialize();
  void 	Destroy()		{ };
  SIMPLE_COPY(MaxActTrgStat);
  COPY_FUNS(MaxActTrgStat, Stat);
  TA_BASEFUNS(MaxActTrgStat);
};

class UnitActRFStat : public Stat {
  // ##COMPUTE_IN_TrialProcess unit activity receptive-field stat: gets RF of units in layer from rf_layers by weighting rf_layer acts by unit acts of layer units
public:
  enum NormMode {
    NORM_UNIT,			// normalize each unit separately
    NORM_LAYER,			// normalize over entire layer (divide by layer max of avg_norms)
    NORM_GROUP			// normalize by unit groups (divide by group max of avg_norms)
  };

  Layer_MGroup	rf_layers;	// #LINK_GROUP layers to compute receptive field over: (sending-layers -- receiving layer is in layer member)
  Environment*	data_env;	// environment to store data into
  NormMode	norm_mode;	// how to normalize the values
  float_RArray	avg_norms;	// average normalizers (sum of unit activity vals)

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  virtual void	InitRFVals();	// #BUTTON initalize rf aggregation values

  virtual bool	CheckRFLayersInNet(); // make sure rf layers are all in network.

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void 	CutLinks();
  void 	Copy_(const UnitActRFStat& cp);
  COPY_FUNS(UnitActRFStat, Stat);
  TA_BASEFUNS(UnitActRFStat);
};

class UnitActRFStatResetProc : public Process {
  // resets the accumulated activation-based receptive field information on a UnitActRFStat -- put this at the point in a process heirarchy where RF's should be reset (e.g., Epoch INIT)
public:
  UnitActRFStat*	unit_act_rf_stat; // pointer to the stat to reset time for

  void			C_Code();

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	CutLinks();
  SIMPLE_COPY(UnitActRFStatResetProc);
  COPY_FUNS(UnitActRFStatResetProc, Process);
  TA_BASEFUNS(UnitActRFStatResetProc);
};

class UnitEventRFStat : public Stat {
  // stores one event per unit, with each pattern element representing firing for each trial
public:
  EpochProcess* epoch_proc;	// #READ_ONLY #NO_SAVE epoch process for event count
  Environment*	data_env;	// environment to store data into

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  virtual void	GetEpochProc();
  virtual void	InitRFVals();	// #BUTTON initalize rf aggregation values

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void 	CutLinks();
  void 	Copy_(const UnitEventRFStat& cp);
  COPY_FUNS(UnitEventRFStat, Stat);
  TA_BASEFUNS(UnitEventRFStat);
};

class UniquePatStat : public Stat {
  // An aggregator-like stat that counts the number of unique patterns in given stat
public:
  Stat*		pat_stat;	// stat that is the source of patterns
  Environment*	data_env;	// environment to store patterns into during computation
  float_RArray::DistMetric cmp_type;	// comparison type to perform to determine uniqueness
  StatVal	unq;		// number of unique patterns
  float		dist_tol;	// #CONDEDIT_ON_cmp_type:SUM_SQUARES,EUCLIDIAN,HAMMING tolerance value for distance functions
  bool		norm;		// #CONDEDIT_OFF_cmp_type:COVAR,CORREL,CROSS_ENTROPY whether to normalize (distance, inner prod)
  float		uniq_tol;	// overall distance tolerance to determine if unique or not
  SimpleMathSpec pre_proc_1;	// step 1 of pre-processing to apply before comparision
  SimpleMathSpec pre_proc_2;	// step 2 of pre-processing to apply before comparision
  SimpleMathSpec pre_proc_3;	// step 3 of pre-processing to apply before comparision
  float_RArray	svals;		// #HIDDEN #NO_SAVE stat values

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  virtual void	GetStatVals(Stat* st, float_RArray& sv);
  // get stat vals into an array

  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void 	CutLinks();
  void 	Copy_(const UniquePatStat& cp);
  COPY_FUNS(UniquePatStat, Stat);
  TA_BASEFUNS(UniquePatStat);
};

class TimeCounterStat : public Stat {
  // ##COMPUTE_IN_TrialProcess ##LOOP_STAT continuously incrementing time counter -- useful for an X axis for graphing events across normal counter boundaries
public:
  StatVal	time;

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Stat();
  void		Layer_Run()	{ };	// don't loop over network

  void		NameStatVals();

  virtual void	InitTime();	// #BUTTON reset time to zero

  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const TimeCounterStat& cp);
  COPY_FUNS(TimeCounterStat, Stat);
  TA_BASEFUNS(TimeCounterStat);
};

class TimeCounterStatResetProc : public Process {
  // resets the time on a TimeCounterStat -- put this at the point in a process heirarchy where time should be reset
public:
  TimeCounterStat*	time_ctr_stat; // pointer to the stat to reset time for

  void			C_Code();

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	CutLinks();
  SIMPLE_COPY(TimeCounterStatResetProc);
  COPY_FUNS(TimeCounterStatResetProc, Process);
  TA_BASEFUNS(TimeCounterStatResetProc);
};

class DispDataEnvProc : public Process {
  // displays information contained in a data environment that is being updated by another statistic
public:
  enum DispType {
    RAW_DATA_GRID,		// directly plot the data to a grid log
    DIST_MATRIX,		// distance matrix in grid log
    CLUSTER_PLOT,		// cluster plot of distance matrix in graph log
    CORREL_MATRIX,		// correlation matrix for values across patterns displayed in grid log
    PCA_EIGEN_GRID,		// principal components analysis plot of eigen vectors in grid log
    PCA_PRJN_PLOT,		// principal components analysis projection plot in graph log
    MDS_PRJN_PLOT		// multidimensional scaling on distance matrix in graph log
  };

  Environment*	data_env;	// environment containing data to be plotted (data is assumed to be in pattern 0)
  int		pat_no;		// pattern number in the environment to display
  DispType	disp_type;	// what type of data display to make
  PDPLog*	disp_log;	// log view to contain the display
  float_RArray::DistMetric dist_metric;	// distance metric (where appropriate)
  bool		dist_norm;	// #CONDEDIT_OFF_dist_metric:COVAR,CORREL,CROSS_ENTROPY normalize distances?
  float		dist_tol;	// #CONDEDIT_ON_dist_metric:SUM_SQUARES,EUCLIDIAN,HAMMING tolerance for computing distances (below tol = 0 error)
  int		x_axis_component; // for PCA & MDS Prjn Plots -- use this component for x axis
  int		y_axis_component; // for PCA & MDS Prjn Plots -- use this component for y axis

  virtual bool	DispIsGridLog(); // make sure disp_log is a grid log -- if not or NULL, make one..
  virtual bool	DispIsGraphLog();// make sure disp_log is a graph log -- if not or NULL, make one..

  void		C_Code();

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	CutLinks();
  SIMPLE_COPY(DispDataEnvProc);
  COPY_FUNS(DispDataEnvProc, Process);
  TA_BASEFUNS(DispDataEnvProc);
};

class DispNetWeightsProc : public Process {
  // displays network weight values in a GridLog using GridViewWeights function
public:
  String	recv_layer_nm;	  // name of the receiving layer (plot weights into these units)
  String	send_layer_nm;	  // name of the sending layer (plot weights from these units into recv)
  GridLog*	grid_log;	  // grid log to display weights in
  Layer*	recv_layer;	  // #HIDDEN #NO_SAVE the actual recv layer
  Layer*	send_layer;	  // #HIDDEN #NO_SAVE the actual send layer

  virtual bool	SetPtrs();	  // set the layer pointers from names and the grid_log pointer

  void		C_Code();

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void	CutLinks();
  SIMPLE_COPY(DispNetWeightsProc);
  COPY_FUNS(DispNetWeightsProc, Process);
  TA_BASEFUNS(DispNetWeightsProc);
};

class ClearLogProc : public Process {
  // clear a log display
public:
  PDPLog*	log_to_clear;	// log to clear

  void		C_Code();

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	CutLinks();
  SIMPLE_COPY(ClearLogProc);
  COPY_FUNS(ClearLogProc, Process);
  TA_BASEFUNS(ClearLogProc);
};

#endif	/* procs_extra_h */
