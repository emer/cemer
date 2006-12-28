// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

// v3_compat.h -- version 3 compatability objects, 
//  only for converting v3.x files -- no other files should ref this,
//  since conversion routines should be in this file
// NOTE: symbols exported because they are used by value in the Project,
//  so require to be linked externally.

#ifndef V3_COMPAT_H
#define V3_COMPAT_H

#include "pdp_def.h"

#include "ta_script.h"
#include "tatime.h"
#include "minmax.h"
#include "ta_datatable.h"
#include "pdpdeclare.h"
#include "pdp_base.h"
#include "spec.h"
#include "pdp_project.h"
#include "netdata.h"

#include "pdp_TA_type.h"

// forwards this file
// from process.h
class CtrlPanelData;
class Process;
class Process_Group;

// from stats.h
class DataItem;
class StatVal;
class StatVal_List;
class StatValAgg;
class AggStat;
class Stat;
class Stat_Group;
class SE_Stat;
class MonitorStat;

// from sched_proc.h
class Counter;
class StepParams;
class SchedProcess;
class BatchProcess;
class TrainProcess;
class NEpochProcess;
class EpochProcess;
class SequenceProcess;
class TrialProcess;
class SettleProcess;
class CycleProcess;
class SequenceEpoch; //

// from enviro.h
class PatternSpec;
class EventSpec;
class Pattern;
class Event;
class Event_Group;
class Environment; //

// from enviro_extra.h
//TODO class ScriptEnv;
//TODO class TimeEnvironment;

// from pdp_project.h
class V3ProjectBase; //

// from pdplog.h
class PDPLog;
typedef PDPLog TextLog;
typedef TextLog GridLog;
typedef TextLog NetLog;
typedef PDPLog GraphLog;

class TA_API float_RArray : public float_Array {
  // #NO_UPDATE_AFTER float array with range, plus a lot of other mathematical functions
public:
  enum DistMetric {		// generalized distance metrics
    SUM_SQUARES,		// sum of squares:  sum[(x-y)^2]
    EUCLIDIAN,			// Euclidian distance (sqrt of sum of squares)
    HAMMING, 			// Hamming distance: sum[abs(x-y)]
    COVAR,			// covariance: sum[(x-<x>)(y-<y>)]
    CORREL,			// correlation: sum[(x-<x>)(y-<y>)] / sqrt(sum[x^2 y^2])
    INNER_PROD,			// inner product: sum[x y]
    CROSS_ENTROPY		// cross entropy: sum[x ln(x/y) + (1-x)ln((1-x)/(1-y))]
  };

  MinMax	range;		// #NO_SAVE min-max range of the data

  void		Reset(){float_Array::Reset();range.Init(0.0f);}

  void	Initialize()		{ };
  void	Destroy()		{ };
  void	InitLinks();
  void 	Copy_(const float_RArray& cp);
  COPY_FUNS(float_RArray, float_Array);
  TA_BASEFUNS(float_RArray);
};


class PDP_API CritParam : public taBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_v3Compat stopping criteria params
public:
  enum Relation {
    EQUAL,		// #LABEL_=
    NOTEQUAL,		// #LABEL_!=
    LESSTHAN,		// #LABEL_<
    GREATERTHAN,	// #LABEL_>
    LESSTHANOREQUAL,	// #LABEL_<=
    GREATERTHANOREQUAL 	// #LABEL_>=
  };

  bool          flag;           // #LABEL_ whether to use this criterion or not
  Relation	rel;		// #LABEL_ #CONDEDIT_ON_flag:true relation of statistic to target value
  float		val;		// #LABEL_ #CONDEDIT_ON_flag:true target or comparison value
  int		cnt;		// #CONDEDIT_ON_flag:true Number of times criterion must be met before stopping
  int		n_met;		// #READ_ONLY number of times actually met

  bool 	Evaluate(float cmp);

  void	Init()	{ n_met = 0; }	// initialize the process (n_met counter)

  void  Initialize();
  void 	Destroy()		{ };
  void	Copy_(const CritParam& cp);
  COPY_FUNS(CritParam, taBase);
  TA_BASEFUNS(CritParam);
};


#ifdef TA_GUI
class PDP_API CtrlPanelData : public taOBase {
  // ##NO_TOKENS #INLINE #INLINE_DUMP ##CAT_v3Compat data for the control panel
INHERITED(taOBase)
public:
  bool		active;		// is panel active (save this state)
  float		lft;		// panel window left coord
  float		top;		// panel window top coord

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(CtrlPanelData);
};
#endif

class PDP_API V3ScriptFile : public taOBase {
  // ##NO_TOKENS #INLINE #INLINE_DUMP ##CAT_v3Compat V3 compat format for reading script files
INHERITED(taOBase)
public:
  String	fname;

  void 	Initialize() { };
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(V3ScriptFile);
};

class TA_API Script : public taNBase {
  // ##EXT_scr ##CAT_v3Compat an object for maintaining and running arbitrary scripts
INHERITED(taNBase)
public:
  V3ScriptFile	script_file;
  String	script_string;

  // script code to be run, instead of loading from file
  bool		recording;	// #READ_ONLY #NO_SAVE currently recording?
  bool		auto_run;	// run automatically at startup?
  SArg_Array	s_args;		// string-valued arguments to pass to script

//   virtual bool  Run();
//   // #BUTTON #GHOST_OFF_recording run the script (returns false for no scr)
//   virtual void	Record(const char* file_nm = NULL);
//   // #BUTTON #GHOST_OFF_recording #ARGC_0 #NO_SCRIPT record script code for interface actions
//   virtual void	StopRecording();
//   // #BUTTON #LABEL_StopRec #GHOST_ON_recording stop recording script code
//   virtual void	Clear();
//   // #BUTTON #CONFIRM clear script file
//   virtual void	Compile();
//   // #BUTTON #GHOST_OFF_recording compile script from script file into internal runnable format
//   virtual void	CmdShell();
//   // #BUTTON #GHOST_OFF_recording set css command shell to operate on this script, so you can run, debug, etc this script from the command line
//   virtual void	ExitShell();
//   // #BUTTON #GHOST_OFF_recording exit the command shell for this script (shell returns to previous script)

//   virtual void	ScriptAllWinPos();
//   // #MENU #MENU_ON_Actions #NO_SCRIPT record script code to set window positions, iconified

//   virtual void	AutoRun();
//   // run this script if auto_run is set

  TypeDef*	GetThisTypeDef() const	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }

  //  void	UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(Script);
private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
};

class TA_API Script_Group : public taGroup<Script> {
  // ##CAT_v3Compat 
public:
//   virtual void	StopRecording();
//   virtual void	AutoRun();

  void	Initialize();
  void 	Destroy()		{ };
  TA_BASEFUNS(Script_Group);
};

class PDP_API Process : public taNBase {
  // ##EXT_proc ##CAT_v3Compat simple processes for controlling and coordinating execution
INHERITED(taNBase)
public:
  enum Type {
    C_CODE,			// C code (builtin)
    SCRIPT 			// Script (user-defined)
  };

  V3ScriptFile	script_file;
  String	script_string;

  TypeDef* 	min_network;	// #HIDDEN #NO_SAVE #TYPE_Network Minimum acceptable Network type
  TypeDef* 	min_layer;	// #HIDDEN #NO_SAVE #TYPE_Layer Minimum acceptable Layer type
  TypeDef* 	min_unit;	// #HIDDEN #NO_SAVE #TYPE_Unit Minimum acceptable Unit type
  TypeDef* 	min_con_group; 	// #HIDDEN #NO_SAVE #TYPE_Con_Group Min acceptable Con_Group type
  TypeDef* 	min_con; 	// #HIDDEN #NO_SAVE #TYPE_Connection Min acceptable Con type

  RndSeed	rndm_seed;	// #HIDDEN random seed, for NewRun(), ReRun()
  TimeUsed	time_used;	// #HIDDEN accumulated time used during the Run() of this process

  Type    	type;			// process can be builtin c-code or a script
  Modulo	mod;			// flag=run this process or not, m=modulus, run process every m times, off=start runing at this offset
  Network*	network;  		// #CONTROL_PANEL network to act on
  Environment*  environment;		// #CONTROL_PANEL environmnent to act in
#ifdef TA_GUI
  CtrlPanelData	ctrl_panel;		// #HIDDEN data for the control panel display
#endif

  // stuff for script_base
  TypeDef*	GetThisTypeDef() const	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }

  void 	Initialize();
  void 	Destroy() { CutLinks(); }
  TA_SIMPLE_BASEFUNS(Process);
};

class PDP_API Process_Group : public taGroup<Process> {
  // ##NO_TOKENS ##CAT_v3Compat a menu group for processes
INHERITED(taGroup<Process>)
public:

  void	Initialize() {SetBaseType(&TA_Process);}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Process_Group);
};


class PDP_API DataItem : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_v3Compat source of a piece of data
INHERITED(taOBase)
public:

  String	name;		// #HIDDEN_INLINE name of data item
  String	disp_opts;	// #HIDDEN_INLINE default display options for item
  bool		is_string;	// #HIDDEN_INLINE is a string-valued item
  int		vec_n;		// #HIDDEN_INLINE length of vector (0 if not)

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(DataItem);
};

class PDP_API DataItem_List : public taList<DataItem> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_v3Compat list of DataItem objects
INHERITED(taList<DataItem>)
public:
  void	Initialize() 		{SetBaseType(&TA_DataItem); };
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(DataItem_List);
};


class PDP_API StatVal : public DataItem {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_v3Compat Statistic value
INHERITED(DataItem)
public:
  float		val;		// value of statistic
  String	str_val;	// #HIDDEN_INLINE value of statistic if its a string
  CritParam	stopcrit;	// Stopping Criteria Parameters

  void 	Initialize();
  void 	Destroy()		{ CutLinks();};
  TA_SIMPLE_BASEFUNS(StatVal);
};


class PDP_API StatVal_List : public taBase_List {
  // ##NO_UPDATE_AFTER ##CAT_v3Compat group of stat values
INHERITED(taBase_List)
public:
  void	Initialize() 		{ SetBaseType(&TA_StatVal); }
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(StatVal_List);
};


class PDP_API StatValAgg : public Aggregate {
  // #INLINE #INLINE_DUMP ##CAT_v3Compat Aggregation for StatVal-based values
INHERITED(Aggregate)
public:

  void 	Initialize() {}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(StatValAgg);
};

class PDP_API AggStat : public StatValAgg {
  // #INLINE #INLINE_DUMP Aggregate statistics over time (processing levels)
INHERITED(StatValAgg)
public:
  Stat*		real_stat;	// #READ_ONLY #NO_SAVE the 'real' (non-agg) stat
  Stat* 	from;
  // which statisitic to aggregate from (a statistic of a sub process)
  bool		type_safe;
  // #HIDDEN true if from ptr should be kept type-safe

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(AggStat);
};

class PDP_API Stat : public Process {
  // Generic Statistic Process
INHERITED(Process)
public:
  enum LoopInitType {
    INIT_IN_LOOP,		// initialize inside the loop (each time stat is run in loop)
    INIT_START_ONLY,		// #AKA_false initialize only at the start of the loop
    NO_INIT			// never initialize this statistic at all (regardless of whether it is a loop or final stat -- this should only be used for script stats)
  };

  SchedProcess* own_proc;	// #READ_ONLY #NO_SAVE The SchedProcess which owns this stat
  bool		has_stop_crit;	// #READ_ONLY true if any of the stats have a stopping crit
  int		n_copy_vals;	// #READ_ONLY the number of copy values added
  LoopInitType	loop_init;  	// how to initialize stat values of a non-aggregator loop_stats statistic -- if NO_INIT then applies to all stats (never init)
  bool		log_stat;	// flag determines if stat data is logged
  AggStat	time_agg;
  // Aggregation over time (i.e., over loop of lower process), if from != NULL
  StatValAgg	net_agg;
  // #CONDEDIT_ON_time_agg.from:NULL Aggregation over network objects (i.e., over units)
  StatVal_List	copy_vals;	// the values of the stat if using COPY time agg
  Layer*	layer;		// restrict computation to this layer if non-null

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Stat);
};

class PDP_API Stat_Group : public taBase_Group {
  // ##NO_TOKENS ##CAT_v3Compat a group of statistics
INHERITED(taBase_Group)
public:
  void	Initialize() 		{ SetBaseType(&TA_Stat); }
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Stat_Group);
};

// SE_Stat and MonitorStat are so basic that they are here, and not in extra

class PDP_API SE_Stat : public Stat {
  // ##COMPUTE_IN_TrialProcess Squared Error Statistic
INHERITED(Stat)
public:
  StatVal	se;			// squared errors
  float		tolerance;		// if error is less than this, its 0

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(SE_Stat);
};

class PDP_API MonitorStat: public Stat {
  // ##COMPUTE_IN_TrialProcess Network Monitor Statistic
INHERITED(Stat)
public:
  StatVal_List	mon_vals;	// the values of the stat as computed directly
  MemberSpace   members;	// #IGNORE memberdefs
  taBase_List	ptrs;     	// #HIDDEN #NO_SAVE actual ptrs to values
  taBase_List 	objects;	// #LINK_GROUP Group of network objects
  String        variable;	// Variable (member) to monitor
  SimpleMathSpec pre_proc_1;	// first step of pre-processing to perform
  SimpleMathSpec pre_proc_2;	// second step of pre-processing to perform
  SimpleMathSpec pre_proc_3;	// third step of pre-processing to perform

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(MonitorStat);
};


class PDP_API Counter : public taBase {
  // #INLINE #INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_v3Compat Holds the value of a loop counter
INHERITED(taBase)
public:
  String	name;			// #HIDDEN not an taNBase to hide name
  int 		val;			// #GUI_READ_ONLY #SHOW value of the counter
  int 		max;			// maximum value of the counter

  virtual void 	SetMax(int i)	  	{ max = i; }

  void 	Initialize()		{ val = 0; max = 1; }
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Counter);
};


class PDP_API StepParams : public taBase {
  // #INLINE #INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_v3Compat Holds steping process parameters
INHERITED(taBase)
public:
  SchedProcess* owner;		// #READ_ONLY #NO_SAVE use this to find the subtypes
  SchedProcess*	proc;		// #SUBTYPE_SchedProcess process to step
  int		n;		// number of steps to take

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(StepParams);
};


class PDP_API SchedProcess : public Process {
  // ##MEMB_IN_GPMENU Generic scheduling, looping process
INHERITED(Process)
public:
  enum	StatLoc {		// locations for statistics
    DEFAULT,
    LOOP_STATS,
    FINAL_STATS
  };
  enum	ProcLoc {		// locations for processes
    INIT_PROCS,
    LOOP_PROCS,
    FINAL_PROCS
  };
  enum	SchedProcLoc {		// locations for schedule processes
    SUPER_PROC,
    SUB_PROC
  };

  Counter*	cntr;			// #READ_ONLY #NO_SAVE pointer to the counter
  DataItem_List	cntr_items;		// #READ_ONLY data item representations for cntrs
  bool          re_init; 		// #HIDDEN Flag for re-initialization

  SchedProcess*	super_proc;		// #READ_ONLY #NO_SAVE #NO_SUBTYPE Process which calls this
  TypeDef*	sub_proc_type;		// #TYPE_SchedProcess #NULL_OK type of sub-process to use
  SchedProcess*	sub_proc;		// Sub-process called by this
  StepParams	step;			// #CONTROL_PANEL Which process to step and n_steps

  Stat_Group	loop_stats;  		// #IN_GPMENU #SHOW_TREE Statistics Computed in Loop
  Stat_Group	final_stats;  		// #IN_GPMENU #SHOW_TREE Statistics Computed after Loop
  Process_Group	init_procs;		// #IN_GPMENU #SHOW_TREE Misc procs run when process is initialized
  Process_Group loop_procs;		// #IN_GPMENU #SHOW_TREE Misc procs run in the loop, using mod based on loop counter
  Process_Group	final_procs;		// #IN_GPMENU #SHOW_TREE Misc procs run after process is finished
//obs  WinView_Group	displays;	// #LINK_GROUP #HIDDEN  #SHOW_TREE views to update
  taBase_Group	logs;			// #LINK_GROUP #HIDDEN  #SHOW_TREE Logs to log to NOTE: was PDPLog_Group in v3
  bool		log_loop;		// Log the loop state (instead of final state)
  bool		log_counter;		// Log the counter values for this process

  void 	Initialize();
  void 	Destroy() {CutLinks();}
  TA_SIMPLE_BASEFUNS(SchedProcess);
};


//////////////////////////
// 	CycleProcess	//
//////////////////////////

class PDP_API CycleProcess : public SchedProcess {
  // ##AGGOP_SUM Runs one cycle of activation update
INHERITED(SchedProcess)
public:
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(CycleProcess);
};


//////////////////////////
// 	SettleProcess	//
//////////////////////////

class PDP_API SettleProcess : public SchedProcess {
  // ##AGGOP_SUM Settles over cycles of activation propagation
INHERITED(SchedProcess)
public:
  Counter	cycle;			// Current cycle number

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(SettleProcess);
};


//////////////////////////
// 	TrialProcess	//
//////////////////////////

class PDP_API TrialProcess : public SchedProcess {
  // ##AGGOP_SUM Runs a single trial (one event)
INHERITED(SchedProcess)
public:
  Event* 	cur_event;
  // #FROM_GROUP_enviro_group the current event (copied from the EpochProc)
  EpochProcess* epoch_proc;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent epoch_proc
  Event_Group*	enviro_group;	// #READ_ONLY #NO_SAVE main event group on environment

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(TrialProcess);
};


//////////////////////////
// 	EpochProcess	//
//////////////////////////

class PDP_API EpochProcess : public SchedProcess {
  // ##AGGOP_SUM Loops over entire set of trials (events) in the environment.\nIf multiple dmem processors are available (after network dmem_nprocs) events are distributed across\nprocessors, and weights synchronized: every batch_n for SMALL_BATCH (=ONLINE), or at end for BATCH.
INHERITED(SchedProcess)
public:
  enum Order {
    SEQUENTIAL,			// present events in sequential order
    PERMUTED,			// permute the order of event presentation
    RANDOM 			// pick an event at random (with replacement)
  };

  enum WtUpdate {
    TEST,			// don't update weights at all (for testing net)
    ON_LINE,			// update weights on-line (after every event) -- this is not viable for dmem processing and is automatically switched to small_batch
    SMALL_BATCH, 		// update weights every batch_n events (in SequenceEpoch, see small_batch for how to apply to sequences)
    BATCH			// update weights in batch (after every epoch)
  };

  Counter	trial;		// Current trial number (and index into list)
  Event* 	cur_event;	// #FROM_GROUP_enviro_group the current event
  Order		order;		// order to present events in
  WtUpdate	wt_update;	// determines weight update mode
  int		batch_n;	// #CONDEDIT_ON_wt_update:SMALL_BATCH number of events for small_batch learning mode (specifies how often weight changes are synchronized in dmem)
  int		batch_n_eff;	// #READ_ONLY #NO_SAVE effective batch_n value = batch_n except for dmem when it = (batch_n / epc_nprocs) >= 1

  int_Array	event_list;	// #HIDDEN list of events
  Event_Group*	enviro_group;	// #READ_ONLY #NO_SAVE main event group on environment

  int		dmem_nprocs;	// maximum number of processors to use for distributed memory computation of events within the epoch (actual = MIN(dmem_nprocs, nprocs / net dmem_nprocs_actual); may be less)
  
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(EpochProcess);
};

class PDP_API SequenceProcess : public SchedProcess {
  // ##AGGOP_SUM Processes a sequence of trials in one event group (must be under a SequenceEpoch, which loops over event groups)
INHERITED(SchedProcess)
public:
  enum Order {
    SEQUENTIAL,			// present events in sequential order
    PERMUTED,			// permute the order of event presentation
    RANDOM 			// pick an event at random (with replacement)
  };

  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize the network state
    MODIFY_STATE 		// modify state (algorithm specific)
  };

  Counter	tick;		// one unit of time in a sequence
  Event*	cur_event;	// #FROM_GROUP_cur_event_gp current event
  Event_Group*	cur_event_gp;	// #FROM_GROUP_enviro_group event group
  Order		order;		// order to present events in
  StateInit	sequence_init;	// how to initialize network stat at start of sequence

  SequenceEpoch* sequence_epoch;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent epoch process
  int_Array	event_list;	// #HIDDEN list of events
  Event_Group*	enviro_group;	// #READ_ONLY #NO_SAVE main event group on environment

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(SequenceProcess);
};

class PDP_API SequenceEpoch : public EpochProcess {
  // Loops over sequences (groups of events) instead of individual events (enviro must have event groups!).
INHERITED(EpochProcess)
public:
  enum SmallBatchType {
    SEQUENCE,			// at the sequence level (batch_n sequences)
    EVENT			// at the event level (within the sequence, weights updated every batch_n events, with an additional update at end of sequence if needed)
  };

  SmallBatchType small_batch;	// #CONDEDIT_ON_wt_update:SMALL_BATCH how to apply SMALL_BATCH wt_update when using sequences

  Event_Group*	cur_event_gp;	// #FROM_GROUP_enviro_group current event group

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(SequenceEpoch);
};

class PDP_API InteractiveEpoch : public EpochProcess {
  // Loops over events in an environment using the interactive interface of GetNextEvent(), which can generate new events based on current state
INHERITED(EpochProcess)
public:
  int		last_trial_val;	// #READ_ONLY #NO_SAVE last trial.val when GetCurEvent was called -- decide wether its time to get a new event or not

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(InteractiveEpoch);
};

//////////////////////////
// 	TrainProcess	//
//////////////////////////

class PDP_API NEpochProcess : public SchedProcess {
  // ##AGGOP_LAST Runs epochs to train network
INHERITED(SchedProcess)
public:
  Counter	epc_ctr; 	// local epoch counter
  int		epoch;		// epoch count on the network itself
  EpochProcess* epoch_proc;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to epoch process

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(NEpochProcess);
};

class PDP_API TrainProcess : public SchedProcess {
  // ##AGGOP_LAST Runs epochs to train network
INHERITED(SchedProcess)
public:
  Counter	epoch; 			// Epoch Counter
  EpochProcess* epoch_proc;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to epoch process

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(TrainProcess);
};


//////////////////////////
// 	BatchProcess	//
//////////////////////////

class PDP_API BatchProcess : public SchedProcess {
  // ##AGGOP_LAST Runs multiple trainings
INHERITED(SchedProcess)
public:
  Counter	batch;		// number of batches run

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(BatchProcess);
};


////////////////////////////
//   Pattern/EventSpec    //
////////////////////////////

#define FOR_ITR_PAT_SPEC(pT, pel, pgrp, pitr, sT, sel, sgrp, sitr) \
for(pel = (pT*) pgrp FirstEl(pitr), sel = (sT*) sgrp FirstEl(sitr); \
    pel && sel; \
    pel = (pT*) pgrp NextEl(pitr), sel = (sT*) sgrp NextEl(sitr))

// forwards declared this file:
class Pattern;
class Pattern_Group;
class PSChannel; // #IGNORE impl class for SourceChannel

class PDP_API PatternSpec : public BaseSubSpec {
  // ##SCOPE_Environment ##CAT_v3Compat sub-spec for patterns within an eventspec
INHERITED(BaseSubSpec)
friend class Environment;
public:
  enum PatTypes {
    INACTIVE,			// not presented to network
    INPUT,			// input pattern
    TARGET,			// target (output) pattern
    COMPARE 			// comparison pattern (for error only)
  };

  enum PatLayer {
    FIRST,			// first layer in the network
    LAST,			// last layer in the network
    LAY_NAME,			// specify layer by name
    LAY_NUM 			// specify layer by number
  };

  enum PatFlags {		// bit-flags for the flag field on pattern
    NO_FLAGS 		= 0x00,	// no flags on pattern value, apply as normal
    TARG_FLAG 		= 0x01,	// unit's TARG flag is set
    EXT_FLAG 		= 0x02,	// unit's EXT flag is set
    TARG_EXT_FLAG 	= 0x03,	// unit's TARG and EXT flags are set
    COMP_FLAG 		= 0x04,	// unit's COMP flag is set
    COMP_TARG_FLAG	= 0x05,	// unit's COMP and TARG flags are set
    COMP_EXT_FLAG	= 0x06,	// unit's COMP and EXT flags are set
    COMP_TARG_EXT_FLAG  = 0x07,	// unit's COMP, TARG, and EXT flags are set
    TARG_VALUE 		= 0x10,	// pattern value goes to the unit targ field
    EXT_VALUE 		= 0x20,	// pattern value goes to the unit ext field
    TARG_EXT_VALUE 	= 0x30,	// pattern value goes to the unit targ & ext fields
    NO_UNIT_FLAG 	= 0x40,	// no unit flags are set, but value is set as normal
    NO_UNIT_VALUE 	= 0x80,	// don't set the unit's value, but flag as normal
    NO_APPLY 		= 0x100 // don't apply this value to unit (no flags either)
  };

  enum LayerFlags {		// how to flag the layer's external input status
    DEFAULT 		= 0x00,	// set default layer flags based on pattern type
    TARG_LAYER 		= 0x01,	// as a target layer
    EXT_LAYER 		= 0x02,	// as an external input layer
    TARG_EXT_LAYER 	= 0x03,	// as both external input and target layer
    COMP_LAYER		= 0x04,	// as a comparison layer
    COMP_TARG_LAYER	= 0x05,	// as a comparision and target layer
    COMP_EXT_LAYER	= 0x06,	// as a comparison and external input layer
    COMP_TARG_EXT_LAYER = 0x07,	// as a comparison, target, and external input layer
    NO_LAYER_FLAGS	= 0x10 	// don't set any layer flags at all
  };

  enum PatUseFlags {            // control use of the flag field
    USE_NO_FLAGS,               // no flags on pattern value, apply as normal
    USE_PATTERN_FLAGS,          // use flags from pattern
    USE_GLOBAL_FLAGS,           // use global flags from pattern spec
    USE_PAT_THEN_GLOBAL_FLAGS   // use flags from pattern if avail, else global flags
  };

  PatTypes	type;    	// #ENVIROVIEW Type of pattern
  PatLayer  	to_layer;    	// #ENVIROVIEW which network layer to present pattern to
  String        layer_name;  	// #ENVIROVIEW #CONDEDIT_ON_to_layer:LAY_NAME name of layer
  int           layer_num;   	// #ENVIROVIEW #CONDEDIT_ON_to_layer:LAY_NUM number of layer
  Layer* 	layer;		// #READ_ONLY #NO_SAVE Pointer to Layer presented to

  TypeDef*	pattern_type;	// #TYPE_Pattern type of pattern to use
  LayerFlags	layer_flags;	// #ENVIROVIEW how to flag the layer's external input status
  PatUseFlags 	use_flags;	// how to use the flags (on each pattern or global_flags)
  int		n_vals;		// number of values in pattern
  PosTDCoord    geom;		// geometry of pattern in EnviroView
  PosTDCoord    pos;		// position of pattern in EnviroView
  float		initial_val;	// Initial value for pattern values
  Random	noise;		// #ENVIROVIEW Noise added to values when applied
  String_Array  value_names;	// display names of the individual pattern values
  int_Array 	global_flags;	// #CONDEDIT_ON_use_flags:USE_GLOBAL_FLAGS,USE_PAT_THEN_GLOBAL_FLAGS these are global flags for all events (cf use_flags)

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(PatternSpec); //
};

class PDP_API PatternSpec_Group : public taBase_Group {
  // ##SCOPE_Environment ##CAT_v3Compat group of pattern specs (acts like a template for pattern groups)
INHERITED(taBase_Group)
public:
  TypeDef*	pat_gp_type;	// #TYPE_Pattern_Group type of pattern group to use

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(PatternSpec_Group);
};

class PDP_API EventSpec : public BaseSpec {
  // ##SCOPE_Environment ##MEMB_IN_GPMENU ##IMMEDIATE_UPDATE ##CAT_v3Compat event specification
INHERITED(BaseSpec)
public:
  enum PatternLayout {
    DEFAULT,
    HORIZONTAL,
    VERTICAL
  };

  Network*		last_net;
  // #READ_ONLY #NO_SAVE last network events applied to
  PatternSpec_Group 	patterns;
  // #IN_GPMENU #NO_INHERIT group of pattern specs (one-to-one with patterns)
  PatternLayout		pattern_layout;
  // determines the layout of patterns within an event (just for display purposes)

  void	Initialize();
  void 	Destroy() 	{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(EventSpec);
};

class PDP_API EventSpec_SPtr : public SpecPtr<EventSpec> {
  // ##CAT_v3Compat 
INHERITED(SpecPtr<EventSpec>)
public:
  void 	Initialize() 		{ };
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(EventSpec_SPtr);
};


////////////////////////
//   Pattern/Event    //
////////////////////////

class PDP_API Pattern : public taOBase {
  // ##SCOPE_Environment ##EXT_pat ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_v3Compat Contains activation values to be applied to a network layer
INHERITED(taOBase)
public:
  float_RArray 	value;  	// Values of Pattern
  int_Array   	flag;  		// Flags of Pattern
  
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Pattern);
};

class PDP_API Pattern_Group : public taGroup<Pattern> {
  // ##CAT_v3Compat Group of patterns
INHERITED(taGroup<Pattern>)
public:

  void	Initialize() {SetBaseType(&TA_Pattern);}
  void 	Destroy()		{ }
  TA_BASEFUNS(Pattern_Group);
};

class PDP_API Event : public taNBase {
  // ##SCOPE_Environment ##EXT_evt ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_v3Compat Contains patterns of activation for different layers in the network specifying one event
INHERITED(taNBase)
public:
  int			index;		// #NO_SAVE #READ_ONLY Index of this event within group
  Pattern_Group 	patterns;  	// #NO_SAVE_PATH_R group of patterns
  EventSpec_SPtr	spec;		// determines the configuration of patterns and how they are presented to the network

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Event);
};

// environment presents the following model: either a flat list of events
// (interface = EventCount() && GetEvent()) or a set of event-groups (leaf groups)
// (interface = GroupCount() && GetGroup())
// when events are generated algorithmically, the results are put in a set of event
// structures, and the same interface can be used (see ProcEnvironment below)
// other models are definable, but the standard EpochProcess will not
// understand them.

class PDP_API Event_Group : public taGroup<Event> {
  // ##SCOPE_Environment ##CAT_v3Compat Group of events
INHERITED(taGroup<Event>)
protected:
  void	El_SetIndex_(void* base, int idx) { ((Event*)base)->index = idx; }
public:

  void	Initialize() {SetBaseType(&TA_Event);}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Event_Group);
}; //

////////////////////////
//   Environment      //
////////////////////////

class PDP_API Environment : public taNBase {
  // ##EXT_env ##COMPRESS ##CAT_v3Compat basic environment: contains events to present to the network, and can be used to hold data for analysis
INHERITED(taNBase)
public:
  enum TextFmt {
    NAME_FIRST,			// save file with names as first column
    NAME_LAST,			// save file with names as last column
    NO_NAME			// no names at all in file..
  };

  enum DistMatFmt {		// distance matrix format
    STD_PRINT,			// standard printout format
    PRINT_NO_LABELS,		// standard printout with no event names or other labels
    GRID_LOG			// grid log format for importing into a grid log
  };

  BaseSpec_Group 	event_specs;	// specs for events: controls the layout and configuration of events
  Event_Group 		events;		// the events, contain patterns that map onto layers of the network
  int			event_ctr; 	// #READ_ONLY #SHOW counter for interactive interface with environment: number of events processed since last InitEvents()

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Environment); //

};

// note: Environment_Group name is for compatiblity with v3.2 files
class PDP_API Environment_Group : public taGroup<Environment> {
  // ##CAT_v3Compat group of environments
INHERITED(taGroup<Environment>)
public:
  void  Initialize()            { SetBaseType(&TA_Environment); }
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Environment_Group);
};

// note: Environment_Group name is for compatiblity with v3.2 files
class PDP_API Environment_List : public taList<Environment> {
  // ##CAT_v3Compat group of environments
INHERITED(taList<Environment>)
public:
  void  Initialize()            { SetBaseType(&TA_Environment); }
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(Environment_List);
};


class PDP_API ScriptEnv : public Environment {
  // For algorithmically generated events: Initialization of events is done by a script at the start of each epoch through the InitEvents() function
INHERITED(Environment)
public:
  V3ScriptFile	script_file;
  String	script_string;

  SArg_Array	s_args;		// string-valued arguments to pass to script

  // note: this must stay in here to make a non-abstract base class!
  TypeDef*	GetThisTypeDef() const	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ScriptEnv);
};

class PDP_API InteractiveScriptEnv : public ScriptEnv {
  // For interactively-generated environments: Script is called for each event in GetNextEvent function (use with InteractiveEpoch)
INHERITED(ScriptEnv)
public:
  void 	Initialize() {}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(InteractiveScriptEnv);
};


//////////////////////////
//      Frequency 	//
//////////////////////////

class PDP_API FreqEvent : public Event {
  // an event that has a frequency associated with it
INHERITED(Event)
public:
  float 	frequency;	// #ENVIROVIEW_freq frequency of occurance for this event

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(FreqEvent);
};

class FreqEnv;

class PDP_API FreqEvent_Group : public Event_Group {
  // an event group that has a frequency associated with it
INHERITED(Event_Group)
public:
  FreqEnv*	fenv;		// #READ_ONLY #NO_SAVE parent frequency environment
  int_Array	list;		// #HIDDEN list of event indicies to present for GROUP_EVENT
  float		frequency;	// frequency of occurance for this group of events

  void	Initialize();
  void 	Destroy()		{CutLinks(); };
  TA_SIMPLE_BASEFUNS(FreqEvent_Group);
};


class PDP_API FreqEnv : public Environment {
  // environment which has a frequency for each event
INHERITED(Environment)
public:
  enum FreqLevel {
    NO_FREQ,			// don't use frequency
    EVENT,			// use frequency at the event level
    GROUP,			// use frequency at the group level
    GROUP_EVENT 		// frequency at both group and event levels
  };

  enum SampleType {		// type of frequency sampling to use
    RANDOM,			// random sampling (n_samples at freq probability)
    PERMUTED 			// permuted (n_sample * freq evts per epoch)
  };

  int_Array	list;		// #HIDDEN list of event/group indicies to present
  FreqLevel	freq_level;	// level at which to use the frequency information
  int		n_sample;	// #CONDEDIT_OFF_freq_level:NO_FREQ number samples of the events to make per epoch
  SampleType	sample_type;	// #CONDEDIT_OFF_freq_level:NO_FREQ type of sampling (random with freq or permuted n_samples * freq)

  void	Initialize();
  void 	Destroy()		{ CutLinks();};
  TA_SIMPLE_BASEFUNS(FreqEnv);
};


//////////////////////////
//         Time 	//
//////////////////////////

class PDP_API TimeEvent : public Event {
  // an event which occurs at a specific time
INHERITED(Event)
public:
  float		time;		// #ENVIROVIEW time at which it should appear

  void 	Initialize() {time = 0.0f;}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(TimeEvent);
};

class PDP_API TimeEvent_Group : public Event_Group {
  // a group of time-based events
INHERITED(Event_Group)
public:
  enum Interpolate {
    PUNCTATE,			// events appear for a single instant only
    CONSTANT,			// events persist constantly, change discretely
    LINEAR,			// linear interpolation is performed between events
    USE_ENVIRO 			// use interpolation specified in the environment
  };

  Interpolate	interpolate;	// if and how to interpolate between given event times
  float		end_time;	// time this sequence ends at

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(TimeEvent_Group);
};

class PDP_API TimeEnvironment : public Environment {
  // an environment that manages time-based events
INHERITED(Environment)
public:
  enum Interpolate {
    PUNCTATE,			// events appear for a single instant only
    CONSTANT,			// events persist constantly, change discretely
    LINEAR 			// linear interpolation is performed between events
  };

  Interpolate	interpolate;    // if and how to interpolate between given event times

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(TimeEnvironment);
};

//////////////////////////
//     FreqTime 	//
//////////////////////////

class PDP_API FreqTimeEvent : public TimeEvent {
  // a time event that has a frequency associated with it
INHERITED(TimeEvent)
public:
  float 	frequency;	// #ENVIROVIEW_freq frequency of occurance for this event

  void	Initialize() {frequency = 0.0f;}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(FreqTimeEvent);
};

class PDP_API FreqTimeEvent_Group : public TimeEvent_Group {
  // a time event group that has a frequency associated with it
INHERITED(TimeEvent_Group)
public:
  float		frequency;	// frequency of occurance for this group of events

  void	Initialize(){frequency = 0.0f;}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(FreqTimeEvent_Group);
};


class PDP_API FreqTimeEnv : public TimeEnvironment {
  // a time environment which has a frequency for each event
INHERITED(TimeEnvironment)
public:
  enum FreqLevel {
    NO_FREQ,			// don't use frequency
    EVENT,			// use frequency at the event level
    GROUP 			// use frequency at the group level
  };

  enum SampleType {		// type of frequency sampling to use
    RANDOM,			// random sampling (n_samples at freq probability)
    PERMUTED 			// permuted (n_sample * freq evts per epoch)
  };

  int_Array	list;		// #HIDDEN list of event/group indicies to present
  FreqLevel	freq_level;	// level at which to use the frequency information
  int		n_sample;	// #CONDEDIT_OFF_freq_level:NO_FREQ number samples of the events to make per epoch
  SampleType	sample_type;	// #CONDEDIT_OFF_freq_level:NO_FREQ type of sampling (random with freq or permuted n_samples * freq)

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(FreqTimeEnv);
};

//////////////////////////
//     Probability 	//
//////////////////////////

class PDP_API ProbPattern : public Pattern {
  // pattern is chosen from group of patterns with given probability
INHERITED(Pattern)
public:
  float    	prob;		// #ENVIROVIEW probability of showing this pattern
  bool		applied;	// #READ_ONLY #NO_SAVE whether it was applied

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ProbPattern);
};

class PDP_API ProbPatternSpec_Group : public PatternSpec_Group {
  // defines a group of patterns that are chosen according to their probabilities
INHERITED(PatternSpec_Group)
public:

  int 	last_pat; 	 // #HIDDEN #NO_SAVE last pattern chosen

  void Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ProbPatternSpec_Group);
};

class PDP_API ProbEventSpec : public EventSpec {
  // events have probabalistically-chosen patterns contained in ProbPatternSpec_Groups
INHERITED(EventSpec)
public:
  float		default_prob;	// default probability

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ProbEventSpec);
};

//////////////////////////
//     XY Offset	//
//////////////////////////

class PDP_API XYPatternSpec : public PatternSpec {
  // for patterns that are positioned at a particular x,y offset location
INHERITED(PatternSpec)
public:
  bool		wrap;
  // whether to wrap around target layer if pattern extends beyond coords
  bool		apply_background;
  // whether to give all units not in the pattern a background value
  float		background_value;
  // value to apply to all other units in the layer (if applied)

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(XYPatternSpec);
};

class PDP_API XYPattern : public Pattern {
  // specifies the x,y offset location of the pattern in the layer
INHERITED(Pattern)
public:
  TwoDCoord	offset;		// #ENVIROVIEW offset within network layer for pattern

  void	Initialize()	{ };
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(XYPattern);
};


//////////////////////////
//     XY Subset	//
//////////////////////////

class PDP_API XYSubPatternSpec : public PatternSpec {
  // presents rectagular subsets (size of layer) of large patterns at x,y offset
INHERITED(PatternSpec)
public:
  bool		wrap;
  // whether to wrap around pattern if layer extends beyond coords

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(XYSubPatternSpec);
};

class PDP_API XYSubPattern : public Pattern {
  // specifies the x,y offset location of the layer within the pattern
INHERITED(Pattern)
public:
  TwoDCoord	offset;		// #ENVIROVIEW offset within pattern for network layer

  void	Initialize()	{ };
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(XYSubPattern);
};


//////////////////////////
//     GroupPattern	//
//////////////////////////

class PDP_API GroupPatternSpec : public PatternSpec {
  // organizes pattern values into sub-groups for viewing and/or sending to network
INHERITED(PatternSpec)
public:
  PosTDCoord	sub_geom;
  // geometry of the individual sub-groups: must evenly divide into overall geom in both x & y
  PosTDCoord	gp_geom;
  // #READ_ONLY geometry of the groups within overall geom (just geom / sub_geom)
  bool		trans_apply;
  // translate apply of values to network (only if units are flat, not grouped!)

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(GroupPatternSpec);
};

//////////////////////////////////////////
// 	Duration  Events		//
//////////////////////////////////////////

class PDP_API DurEvent : public Event {
  // an event which lasts for a particular amount of time
INHERITED(Event)
public:
  float		duration;	// #ENVIROVIEW length of time (cycles) event should be presented

  void 	Initialize(){ duration = 50.0f;}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(DurEvent);
};

//////////////////////////////////////////
// 	Read from File  		//
//////////////////////////////////////////

class PDP_API FromFileEnv : public Environment {
  // Environment that reads events incrementally from a file into events. NOT SUPPORTED IN CONVERSION
INHERITED(Environment)
public:
  enum 	ReadMode {
    ONE_EPOCH,			// read one epoch at a time, using InitEvents interface
    ONE_EVENT			// read one event at a time, using GetNextEvent interface (requires InteractiveEpoch process)
  };

  ReadMode	read_mode;	// how to read in events: either one epoch or one event at a time (one event requires InteractiveEpoch process)
  taFiler*	event_file;	// file to read events from
  TextFmt	text_file_fmt;	// format of text file
  bool		binary;		// file is binary (written by WriteBinary). Otherwise, its Text as readable by ReadText
  int		events_per_epc;	// how many events to present per epoch
  int		file_pos;	// #READ_ONLY #SHOW #NO_SAVE position (in terms of events) within the file

  void 	Initialize() {}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(FromFileEnv);
};

//////////////////////////////////
//	procs_extra.h		//
//////////////////////////////////

class ScriptProcess : public Process {
  // a process for use with scripts (has s_args)
INHERITED(Process)
public:
  SArg_Array		s_args;		// string-valued arguments to pass to script

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ScriptProcess);
};

class SaveNetsProc : public Process {
  // saves networks with network.name + counters (batch, epoch)
INHERITED(Process)
public:
  
  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_SIMPLE_BASEFUNS(SaveNetsProc);
};

class SaveWtsProc : public Process {
  // saves weights with network.name + counters (batch, epoch)
INHERITED(Process)
public:
  
  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_SIMPLE_BASEFUNS(SaveWtsProc);
};

class LoadWtsProc : public Process {
  // reads in a set of weights from specified file, as a way to initialize weights
INHERITED(Process)
public:
  String		weights_file; // the file name for the weights file to read in

  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_SIMPLE_BASEFUNS(LoadWtsProc);
};

class InitWtsProc : public Process {
  // initialize the network's weights (Init_Weights)
INHERITED(Process)
public:
  
  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_SIMPLE_BASEFUNS(InitWtsProc);
};


//////////////////////////////////
//	Schedule Processes	//
//////////////////////////////////

class SyncEpochProc : public EpochProcess {
  // Epoch that runs two different sub-processes
INHERITED(EpochProcess)
public:
  Network*		second_network;	  // #CONTROL_PANEL the network for the second trial process
  TypeDef*		second_proc_type; // #TYPE_SchedProcess type of second process
  SchedProcess*		second_proc; 	  // #CONTROL_PANEL second process to call

  void 	Initialize();	
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(SyncEpochProc);
};

class GridSearchBatch : public BatchProcess {
  // Increments param_path parameter over batches to search parameter space in equal increments
INHERITED(BatchProcess)
public:
  float		start_val;	// #CONTROL_PANEL start value of parameter being searched
  float		inc_val;	// #CONTROL_PANEL increment of parameter being searched
  float		cur_val;	// #CONTROL_PANEL #GUI_READ_ONLY current value of parameter based on batch value
  String	param_path;	// path to the parameter (starting at the project)
  
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(GridSearchBatch);
};

class SameSeedBatch : public BatchProcess {
  // Stores a list of random seeds that are used at start of each batch run -- ensures each batch run starts with the same seed
INHERITED(BatchProcess)
public:
  taBase_List	rnd_seeds;	// the random seeds
  int		in_goto_val;	// #READ_ONLY #NO_SAVE currently inside a GoTo call (with this val): affects seed usage

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(SameSeedBatch);
};

//////////////////////////////////
//	Fork and Bridge		//
//////////////////////////////////

class ForkProcess : public SchedProcess {
  // Runs two different sub-processes
INHERITED(SchedProcess)
public:
  Network*		second_network;	  // the network for the second process
  Environment*		second_environment; // the environment for the second process
  TypeDef*		second_proc_type; // #TYPE_SchedProcess #NULL_OK type of second process
  SchedProcess*		second_proc; 	  // second process to call

  void 	Initialize();	
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ForkProcess);
};

class BridgeProcess : public Process {
  // a bridge that connects two different networks together by copying values
INHERITED(Process)
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

  void 	Initialize();	
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(BridgeProcess);
};

class MultiEnvProcess : public SchedProcess {
  // run subprocess over multiple environments, indexed by counter
INHERITED(SchedProcess)
public:
  Environment_List	environments;   // #LINK_GROUP list of environments to process
  bool			use_subctr_max;	// determines whether subctr_max values are actually used
  int_Array		subctr_max; 	// sets the counter max value for the subprocess under this one for each environment (e.g., if subproc is nepochproc, sets numb of epochs per enviro)
  Counter		env;            // current environment number
  
  void Initialize();   
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(MultiEnvProcess);
};

class PatternFlagProcess : public SchedProcess {
  // iteratively sets/resets pattern flag at index = counter for all patterns, e.g. to determine sensitivity to given input
INHERITED(SchedProcess)
public:
  int          		pattern_no;     // index of pattern to update
  PatternSpec::PatFlags flag;   	// flag to be set (or unset if invert)
  bool			invert;		// unset the flag instead of setting it
  Counter		val_idx;	// index of current pattern value to be flagged
  
  void Initialize();   
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(PatternFlagProcess);
};

  
//////////////////////////////////
//	Stats Processes		//
//////////////////////////////////

class ClosestEventStat : public Stat {
  // ##COMPUTE_IN_TrialProcess gets the closest event to the current output activity pattern
INHERITED(Stat)
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

  void	Initialize();
  void 	Destroy()	{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ClosestEventStat);
};

class CyclesToSettle : public Stat {
  // ##COMPUTE_IN_TrialProcess ##LOOP_STAT Records number of cycles it took to settle
INHERITED(Stat)
public:
  SettleProcess* settle;	// #READ_ONLY #NO_SAVE settle process to record
  StatVal	cycles;		// number of cycles to settle

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(CyclesToSettle);
};

class ActThreshRTStat : public Stat {
  // ##COMPUTE_IN_SettleProcess ##LOOP_STAT records reaction-time in terms of number of cycles it took for max act in layer to exceed a threshold (doesn't necc stop settling though, unless stopcrit is set!)
INHERITED(Stat)
public:
  SettleProcess* settle;	// #READ_ONLY #NO_SAVE settle process to record
  float		act_thresh;	// threshold activation level -- rt_cycles are updated until layer max_act >= thresh (also auto sets max_act.stopcrit.val)
  StatVal	max_act;	// maximum activation of units in layer -- computed continuously, if stopcrit set here then process will actually stop at rt threshold
  StatVal	rt_cycles;	// number of cycles of settling prior to max_act.val >= act_tresh
  bool		crossed_thresh;	// #READ_ONLY #NO_SAVE true if already crossed threshold

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ActThreshRTStat);
};

class ScriptStat : public Stat {
  // Use this stat for custom script-based stats
INHERITED(Stat)
public:
  StatVal_List	vals;		// put stat results in this group
  SArg_Array	s_args;		// string-valued arguments to pass to script

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ScriptStat);
};

class CompareStat : public Stat {
  // An aggregator-like stat that compares the output of two other stats
INHERITED(Stat)
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

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(CompareStat);
};

class ProjectionStat : public Stat {
  // #BUTROWS_1 projects values from another stat along a given vector according to given distance metric
INHERITED(Stat)
public:
  Stat*		stat;		// stat to get values to project
  float_RArray	prjn_vector;	// vector of values to project along
  float_RArray::DistMetric dist_metric;	// distance metric for projection (INNER_PROD = standard metric for projections)
  float		dist_tol;	// #CONDEDIT_ON_dist_metric:SUM_SQUARES,EUCLIDIAN,HAMMING tolerance value for distance metric
  bool		dist_norm;	// #CONDEDIT_OFF_dist_metric:COVAR,CORREL,CROSS_ENTROPY whether to normalize distances (distance, inner prod)
  StatVal	prjn;		// projection value
  float_RArray	svals;		// #HIDDEN #NO_SAVE stat values 

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ProjectionStat);
};

class ComputeStat : public Stat {
  // performs simple math computations on one or two stats, aggs the results according to net_agg
INHERITED(Stat)
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

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ComputeStat);
};

class CopyToEnvStat : public Stat {
  // an aggregator-like stat that copies stat values from source stat to data environment
INHERITED(Stat)
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

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(CopyToEnvStat);
};

class EpochCounterStat : public Stat {
  // ##FINAL_STAT gets the epoch counter from the network
INHERITED(Stat)
public:
  StatVal	epoch;	

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(EpochCounterStat);
};

class ProcCounterStat : public Stat {
  // ##FINAL_STAT gets the process counters from a different process hierarchy
INHERITED(Stat)
public:
  SchedProcess*	proc;		// process to get counters from
  StatVal_List	counters;	// group of counters of data

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ProcCounterStat);
};

class MaxActTrgStat : public Stat {
  // ##COMPUTE_IN_TrialProcess 0-1 error statistic, 1 if unit with max act has trg = 1
INHERITED(Stat)
public:
  StatVal	mxat;			// max activation = target 0-1 err value

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(MaxActTrgStat);
};

class UnitActRFStat : public Stat {
  // ##COMPUTE_IN_TrialProcess unit activity receptive-field stat: gets RF of units in layer from rf_layers by weighting rf_layer acts by unit acts of layer units
INHERITED(Stat)
public:
  enum NormMode {
    NORM_UNIT,			// normalize each unit separately
    NORM_LAYER,			// normalize over entire layer (divide by layer max of avg_norms)
    NORM_GROUP			// normalize by unit groups (divide by group max of avg_norms)
  };

  Layer_Group	rf_layers;	// #LINK_GROUP layers to compute receptive field over: (sending-layers -- receiving layer is in layer member)
  Environment*	data_env;	// environment to store data into
  NormMode	norm_mode;	// how to normalize the values
  float_RArray	avg_norms;	// average normalizers (sum of unit activity vals)

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(UnitActRFStat);
};

class UnitActRFStatResetProc : public Process {
  // resets the accumulated activation-based receptive field information on a UnitActRFStat -- put this at the point in a process heirarchy where RF's should be reset (e.g., Epoch INIT)
INHERITED(Process)
public:
  UnitActRFStat*	unit_act_rf_stat; // pointer to the stat to reset time for

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(UnitActRFStatResetProc);
};

class UnitEventRFStat : public Stat {
  // stores one event per unit, with each pattern element representing firing for each trial
INHERITED(Stat)
public:
  EpochProcess* epoch_proc;	// #READ_ONLY #NO_SAVE epoch process for event count
  Environment*	data_env;	// environment to store data into

  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(UnitEventRFStat);
};

class UniquePatStat : public Stat {
  // An aggregator-like stat that counts the number of unique patterns in given stat
INHERITED(Stat)
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

  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(UniquePatStat);
};

class TimeCounterStat : public Stat {
  // ##COMPUTE_IN_TrialProcess ##LOOP_STAT continuously incrementing time counter -- useful for an X axis for graphing events across normal counter boundaries
INHERITED(Stat)
public:
  StatVal	time;	

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(TimeCounterStat);
};

class TimeCounterStatResetProc : public Process {
  // resets the time on a TimeCounterStat -- put this at the point in a process heirarchy where time should be reset
INHERITED(Process)
public:
  TimeCounterStat*	time_ctr_stat; // pointer to the stat to reset time for
  
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(TimeCounterStatResetProc);
};

class DispDataEnvProc : public Process {
  // displays information contained in a data environment that is being updated by another statistic
INHERITED(Process)
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

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(DispDataEnvProc);
};

class DispNetWeightsProc : public Process {
  // displays network weight values in a GridLog using GridViewWeights function
INHERITED(Process)
public:
  String	recv_layer_nm;	  // name of the receiving layer (plot weights into these units)
  String	send_layer_nm;	  // name of the sending layer (plot weights from these units into recv)
  GridLog*	grid_log;	  // grid log to display weights in
  Layer*	recv_layer;	  // #HIDDEN #NO_SAVE the actual recv layer
  Layer*	send_layer;	  // #HIDDEN #NO_SAVE the actual send layer

  void 	Initialize();	
  void 	Destroy()	{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(DispNetWeightsProc);
};

class ClearLogProc : public Process {
  // clear a log display
INHERITED(Process)
public:
  PDPLog*	log_to_clear;	// log to clear

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ClearLogProc);
};

/// end: procs_extra.h

class TA_API TypeDefault_Group : public taGroup<TypeDefault> {
  // #DEF_PATH_$PDPDIR$/defaults ##CAT_v3Compat group of type default objects
INHERITED(taGroup<TypeDefault>)
public:
  int	Dump_Load_Value(istream& strm, TAPtr par=NULL);
  // reset members before loading..

  void	Initialize() 		{ SetBaseType(&TA_TypeDefault); }
  void 	Destroy()		{ };
  TA_BASEFUNS(TypeDefault_Group);
};


////////////////////////////////////////////////////////////////////////////////
// PDPLog -- legacy logs

class PDP_API PDPLog : public taNBase {
  // ##CAT_v3Compat v3 obsolete log object
INHERITED(taNBase)
public:
  taFiler*	log_file;	// optional file for saving
  DataTable	data;		// data for the log
  int		data_bufsz;	// #DETAIL how big a data buffer size to keep
  float		data_shift;	// #DETAIL percentage to shift buffer upon overflow
  bool		record_proc_name; // whether to record process name in log file or not
  String_Array	display_labels;	// ordered list of labels to use for views and log files
  TDCoord	pos;  // position of view
  TDCoord	geom;  // size of view
  
  void	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(PDPLog);
};

class PDP_API PDPLog_Group : public taGroup<PDPLog> {
  // ##CAT_v3Compat 
public:
  void	Initialize() 		{SetBaseType(&TA_PDPLog);}
  void 	Destroy()		{ }
  TA_BASEFUNS(PDPLog_Group);
};


////////////////////////////////////////////////////////////////////////////////
// V3ProjectBase -- base class for loading and converting

class PDP_API V3ProjectBase : public ProjectBase {
  // #HIDDEN ##CAT_v3Compat for loading legacy (v3.x) projects only
INHERITED(ProjectBase)
public:
  TypeDefault_Group	defaults;	// #CAT_V3_Compat type defaults
  BaseSpec_Group	specs;		// #CAT_V3_Compat network specifications and parameters (moved to Network in V4)
  Environment_Group	environments;	// #CAT_V3_Compat Environments of patterns to present to networks (replaced with datatables in V4)
  Process_Group		processes;	// #CAT_V3_Compat Processes to coordinate training/testing, etc (replaced with programs in V4)
  PDPLog_Group		logs;		// #CAT_V3_Compat Logs to display statistics in processes (replaced with datatables in V4)
  Script_Group		scripts;	// #CAT_V3_Compat Scripts to control arbitrary actions (replaced with programs in V4)
  String		desc1;
  String		desc2;
  String		desc3;
  String		desc4;
  
  virtual void	ConvertToV4(); 
  // #BUTTON #CAT_Convert convert the project to v4.x format
  virtual bool	ConvertToV4_impl();
  // #CAT_Convert implementation: must be defined by specific type of algorithm

  virtual bool	ConvertToV4_Nets(ProjectBase* nwproj); 
  // #CAT_Convert Convert networks
  virtual bool	ConvertToV4_Enviros(ProjectBase* nwproj); 
  // #CAT_Convert Convert environments to datatables
  virtual bool	ConvertToV4_Scripts(ProjectBase* nwproj); 
  // #CAT_Convert Convert scripts to programs
  virtual bool	ConvertToV4_ProcScripts(ProjectBase* nwproj); 
  // #CAT_Convert Convert process scripts to programs
  virtual bool	ConvertToV4_Edits(ProjectBase* nwproj); 
  // #CAT_Convert Copy selectedits over

  virtual bool 	ConvertToV4_DefaultApplyInputs(ProjectBase* nwproj);
  // #CAT_Convert fix ApplyInputs script for first program group in nwproj, based on first enviro and network in old proj
  virtual bool 	ConvertToV4_ApplyInputs(LayerWriter* lw, EventSpec* es,
					Network* net, DataTable* dt);
  // #CAT_Convert make layerwriter list reflect patterns in given event spec as applied to given network

  virtual bool ConvertToV4_ProcScripts_impl(ProjectBase* nwproj, taBase_Group* gp,
					    const String& nm_extra);
  virtual bool ConvertToV4_Script_impl(Program_Group* pg, const String& objnm, 
			       const String& fname, SArg_Array& s_args);

  void 	InitLinks_impl(); //for this class
  void	CutLinks_impl();
  void	Copy_(const V3ProjectBase& cp);
  COPY_FUNS(V3ProjectBase, ProjectBase);
  TA_BASEFUNS(V3ProjectBase);
private:
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
};

#endif

