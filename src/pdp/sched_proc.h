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



#ifndef sched_proc_h
#define sched_proc_h

#include "stats.h"
#include "enviro.h"
#include "pdplog.h"


class Counter : public taBase {
  // #INLINE ##NO_TOKENS #NO_UPDATE_AFTER Holds the value of a loop counter
public:
  String	name;			// #HIDDEN not an taNBase to hide name
  int 		val;			// #IV_READ_ONLY #SHOW value of the counter
  int 		max;			// maximum value of the counter

  virtual bool 	Crit()
  { if(max >= 0) return (val >= max); else return false; }
  virtual void 	Inc()		  	{ val++; }
  virtual void 	operator=(int i) 	{ val = i; }
  virtual bool  operator==(int i)	{ return val==i; }
  virtual bool  operator<(int i) 	{ return val<i; }
  virtual bool  operator>(int i) 	{ return val>i; }
  virtual bool  operator>=(int i)	{ return val>=i; }  virtual bool  operator<=(int i)	{ return val<=i; }
  virtual bool  operator!=(int i)	{ return val!=i; }
  virtual void 	SetMax(int i)	  	{ max = i; }

  void 	Initialize()		{ val = 0; max = 1; }
  void 	Destroy()		{ };
  void	Copy_(const Counter& cp)	{ name = cp.name; val = cp.val; max = cp.max; }
  COPY_FUNS(Counter, taBase);
  TA_BASEFUNS(Counter);
};


class StepParams : public taBase {
  // #INLINE ##NO_TOKENS #NO_UPDATE_AFTER Holds steping process parameters
public:
  SchedProcess* owner;		// #READ_ONLY #NO_SAVE use this to find the subtypes
  SchedProcess*	proc;		// #SUBTYPE_SchedProcess process to step
  int		n;		// number of steps to take

  TAPtr 	GetOwner() const;
  TAPtr		GetOwner(TypeDef* tp) const;
  TAPtr 	SetOwner(TAPtr ta);

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const StepParams& cp);
  COPY_FUNS(StepParams, taBase);
  TA_BASEFUNS(StepParams);
};


class SchedProcess : public Process {
  // ##MEMB_IN_GPMENU Generic scheduling, looping process
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

  static bool	stop_flag;	// #READ_ONLY #NO_SAVE the Stop button was hit: stop running!
  static bool	stepping;	// #READ_ONLY #NO_SAVE the Step button started us running
  static bool	bailing;	// #READ_ONLY #NO_SAVE abort all further processing: stop was triggered below
  static bool	running;	// #READ_ONLY #NO_SAVE some process is running

  bool		im_step_proc;	// #READ_ONLY #NO_SAVE i'm the stepping process
  bool		im_run_proc; 	// #READ_ONLY #NO_SAVE i'm the process that initiated the interactive run

  Counter*	cntr;			// #READ_ONLY #NO_SAVE pointer to the counter
  DataItem_List	cntr_items;		// #READ_ONLY data item representations for cntrs
  bool          re_init; 		// #HIDDEN Flag for re-initialization

  SchedProcess*	super_proc;		// #READ_ONLY #NO_SAVE #NO_SUBTYPE Process which calls this
  TypeDef*	sub_proc_type;		// #DETAIL #TYPE_SchedProcess #NULL_OK type of sub-process to use
  SchedProcess*	sub_proc;		// #DETAIL Sub-process called by this
  StepParams	step;			// #CONTROL_PANEL Which process to step and n_steps

  Stat_Group	loop_stats;  		// #DETAIL #IN_GPMENU #BROWSE Statistics Computed in Loop
  Stat_Group	final_stats;  		// #DETAIL #IN_GPMENU #BROWSE Statistics Computed after Loop
  Process_Group	init_procs;		// #DETAIL #IN_GPMENU #BROWSE Misc procs run when process is initialized
  Process_Group loop_procs;		// #DETAIL #IN_GPMENU #BROWSE Misc procs run in the loop, using mod based on loop counter
  Process_Group	final_procs;		// #DETAIL #IN_GPMENU #BROWSE Misc procs run after process is finished
//obs  WinView_Group	displays;		// #LINK_GROUP #HIDDEN  #BROWSE views to update
  PDPLog_Group	logs;			// #LINK_GROUP #HIDDEN  #BROWSE Logs to log to
  bool		log_loop;		// Log the loop state (instead of final state)
  bool		log_counter;		// Log the counter values for this process

  virtual int  	GetCounter()		{ if (cntr) return cntr->val; else return 0; }
  void 		C_Code(); 		// orchestrates the whole loop

  // main user interface functions for running:
  void		NewInit();
  void		ReInit();
  void		Run_gui();
  // #BUTTON #LABEL_Run #GHOST_OFF_running #NO_SCRIPT run the process in an interactive fashion: can be stopped and sets flags for only running one process
  virtual void	Step();
  // #BUTTON #GHOST_OFF_running #NO_SCRIPT runs the next step of process
  virtual void	Step_gui() { Step(); }	// backwards compatibility: gui is same as step: step implies interactive processing
  virtual void	Stop();
  // #BUTTON #NO_APPLY_BEFORE #GHOST_ON_running #NO_SCRIPT stops any interactively running process
  virtual void	GoTo(int goto_val);	// #BUTTON #GHOST_OFF_running go to given specific counter value
  virtual void	StepUp();		// #BUTTON move up one step in hierarchy, remove updater to network at previous level
  virtual void	StepDn();		// #BUTTON move down one step in hierarchy, add updater to network at new level
  virtual void	NotifyNetObjects();	// #IGNORE notify all network objects of impending modification prior to running a process
  virtual void	NotifyProcDialogs();	// #IGNORE notify all process dialogs when starting or stopping running

  // initialization: before the loop
  void 		Init();			// initializes process state variables (and sub_procs)
  virtual void	InitProcs();		// run the initialize processes (init_procs)

  virtual void	Init_impl();		// #IGNORE this actually has the init code
  virtual void	Init_flag();		// flag this process and all sub procs for re_init
  virtual void	Init_force();		// force re_init of all processes
  virtual void	Init_step_flags();	// initialize step flags

  // inside the loop
  virtual void	Loop();			// called in the do loop (the main looping code)
  virtual void  UpdateCounters();	// update process counters (right after loop)
  virtual void 	LoopProcs();		// call the loop_procs
  virtual void 	LoopStats();		// stats computed inside the loop
  virtual void	UpdateLogs();		// update the logs
  virtual void  UpdateState();		// update misc state variables (after logs)
  virtual bool  Crit();			// returns true if stop criterion was met
  virtual bool	StopCheck();		// check for stop flag or stepping stopping: true if so, false if not
  static  bool 	HandleEvents(); 	// process gui events

  // finalization: after the loop
  virtual void 	FinalProcs();		// call the final_procs
  virtual void	FinalStats();		// compute the final statistics
  virtual void	Final() {};		// any misc things to do after loop
  virtual void	UpdateDisplays();	// update displays after end of loop
  virtual void	SetReInit(bool ri_val)
  { re_init = ri_val; }			// set re-init flag to given value
  virtual bool	FinalStepCheck();	// if this process is step process, always set stop_flag in final so you always step at end

  // log generation functions
  LogData& 	GenLogData(LogData* ld=NULL); // generate log data
  virtual void	GetCntrDataItems();	      // #IGNORE get data items for counters
  virtual void 	GenCntrLog(LogData* ld, bool gen); // generate log data for the counters

  // actions to coordinate with logs, processes, etc
  virtual void	InitMyLogs();
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE clear all logs associated with this process
  virtual void	InitAllLogs();
  // #MENU #CONFIRM clear all logs that exist in the project
  virtual void	InitNetwork();
  // #MENU #CONFIRM initialize network's weights
  virtual void	InitAll();
  // #MENU #CONFIRM initialize process, network weights, and logs
  virtual void	RemoveFromLogs();
  // #MENU #CONFIRM #MENU_SEP_BEFORE remove this from any logs
  virtual void	RemoveFromDisplays();
  // #MENU #CONFIRM remove this from any displays
  virtual bool	CheckAllTypes();
  // #MENU #USE_RVAL #MENU_SEP_BEFORE Check all minimum types for all processes and sub_procs
  virtual bool	CheckAllTypes_impl();
  // #IGNORE implementation of check all types

  virtual void	UpdateLogUpdaters();
  // make sure logs have us in their updater lists
  virtual void	UpdateDisplayUpdaters();
  // make sure logs have us in their updater lists

  // simple process manipulation subroutines to make life a little easier
  virtual SchedProcess* FindSubProc(TypeDef* td); // find of a given type
  virtual SchedProcess* FindSuperProc(TypeDef* td); // find of a given type
  virtual SchedProcess* FindProcOfType(TypeDef* td);
  // find of a given type, including this proc, super and sub procs
  virtual SchedProcess* GetTopProc(); // #IGNORE get the highest-level process

  ////////////////////////////////////////////
  //	Structure manipulation functions:

  virtual void	UnLinkSubProc(); // #IGNORE disconnect from sub process
  virtual void	UnLinkFmSuperProc(); // #IGNORE disconnect me from super process
  virtual void	LinkSubProc(SchedProcess* proc); // #IGNORE link proc object as new sub process (not deleting old one)
  virtual void	LinkSuperProc(SchedProcess* proc); // #IGNORE link proc object as new super process (not deleting old one)
  virtual void	GetAggsFmSuperProc(); // #IGNORE fill in my aggregators from aggs on my super proc
  virtual void	SetAggsAfterRmvSubProc(); // #IGNORE set my aggs to point to my new sub proc stats after removing old one

  virtual void	CreateSubProcs(bool update=true);
  // #NEW_FUN #ARGC_0 create sub-processes of this one

  virtual void	MoveToSubGp(const char* gp_name);
  // #MENU #MENU_ON_Structure Move this process and all sub-procs to a new sub group (and name it gp_name)
  virtual void	ChangeNameSuffix(const char* new_name_sufx);
  // #MENU change the suffix (after _ ) for each process name
  virtual void	AddSuperProc(TypeDef* proc_type);
  // #MENU #TYPE_SchedProcess insert a new super process of given type above this one in hierarchy
  virtual void	AddSubProc(TypeDef* proc_type);
  // #MENU #TYPE_SchedProcess insert a new sub process of given type below this one in hierarchy
  virtual void	RemoveSuperProc();
  // #MENU #CONFIRM remove super process above this one in hierarchy
  virtual void	RemoveSubProc();
  // #MENU #CONFIRM remove sub process below this one in hierarchy

  virtual Stat*	NewStat(TypeDef* type, StatLoc loc=DEFAULT, int num=1, bool create_aggs=true);
  // #MENU #USE_RVAL_RMB #TYPE_Stat create num new statistic(s) of given type in this process at location loc, creating time-aggregates if checked
  virtual Process* NewProcess(TypeDef* type, ProcLoc loc=INIT_PROCS, int num=1);
  // #MENU #USE_RVAL_RMB #TYPE_Process create num new process(es) of given type in this process at location loc
  virtual SchedProcess* NewSchedProc(TypeDef* type, SchedProcLoc loc=SUPER_PROC);
  // #MENU #USE_RVAL_RMB #TYPE_SchedProcess create a new sub/super process of given type below/above the current one

  static SchedProcess* 	Default_StatProc(TypeDef* stat_td, Process_MGroup* procs);
  // returns the default process to create given type of stat in
  static Stat_Group* 	Default_StatGroup(TypeDef* stat_td, SchedProcess* proc);
  // returns the default stat group within sched proc to create stat in
  virtual Stat_Group*	GetStatGroup(TypeDef* stat_td, StatLoc loc);
  // get stat group of specified location (if DEFAULT, use stat_td to determine location)
  virtual Process_Group* GetProcGroup(ProcLoc loc);
  // get process group of specified location
  virtual Stat*		MakeAggregator(Stat* of_stat, StatLoc in_loc, Aggregate::Operator agg_op = Aggregate::DEFAULT);
  // make an aggregator of given stat in given location of this sched proc, using given agg operator

  void		ReplacePointersHook(TAPtr old); // #IGNORE for change my type -- redo all the linking!
  virtual void	DuplicateElHook(SchedProcess* cp); // #IGNORE duplicate all the structure from cp (this is duplicate of sp)

  void 		SetDefaultName();			    // #IGNORE
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_SchedProcess); }
#endif
  void 	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy();
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const SchedProcess& cp);
  COPY_FUNS(SchedProcess, Process);
  TA_BASEFUNS(SchedProcess);
};


//////////////////////////
// 	CycleProcess	//
//////////////////////////

class CycleProcess : public SchedProcess {
  // ##AGGOP_SUM Runs one cycle of activation update
public:
  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(CycleProcess);
};


//////////////////////////
// 	SettleProcess	//
//////////////////////////

class SettleProcess : public SchedProcess {
  // ##AGGOP_SUM Settles over cycles of activation propagation
public:
  Counter	cycle;			// Current cycle number

  void 	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const SettleProcess& cp)	{ cycle = cp.cycle; }
  COPY_FUNS(SettleProcess, SchedProcess);
  TA_BASEFUNS(SettleProcess);
};


//////////////////////////
// 	TrialProcess	//
//////////////////////////

class TrialProcess : public SchedProcess {
  // ##AGGOP_SUM Runs a single trial (one event)
public:
  Event* 	cur_event;
  // #FROM_GROUP_enviro_group the current event (copied from the EpochProc)
  EpochProcess* epoch_proc;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent epoch_proc
  Event_MGroup*	enviro_group;	// #READ_ONLY #NO_SAVE main event group on environment

  void 		Init_impl();
  bool		Crit()		{ return true; } // prevent endless looping

  bool		CheckAllTypes_impl();
  // do an extra pass of checking at the trial process level

  void 	UpdateAfterEdit();	// modify to update the epoch_proc
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	CutLinks();
  void	Copy_(const TrialProcess& cp);
  COPY_FUNS(TrialProcess, SchedProcess);
  TA_BASEFUNS(TrialProcess);
};


//////////////////////////
// 	EpochProcess	//
//////////////////////////

class EpochProcess : public SchedProcess {
  // ##AGGOP_SUM Loops over entire set of trials (events) in the environment.\nIf multiple dmem processors are available (after network dmem_nprocs) events are distributed across\nprocessors, and weights synchronized: every batch_n for SMALL_BATCH (=ONLINE), or at end for BATCH.
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
  Event_MGroup*	enviro_group;	// #READ_ONLY #NO_SAVE main event group on environment

  int		dmem_nprocs;	// maximum number of processors to use for distributed memory computation of events within the epoch (actual = MIN(dmem_nprocs, nprocs / net dmem_nprocs_actual); may be less)
  int		dmem_nprocs_actual; // #READ_ONLY #NO_SAVE actual number of processors being used = MIN(dmem_nprocs, nprocs / net dmem_nprocs_actual)
  int		epc_gp;		// #IGNORE dmem epoch-level group
  int		epc_comm;	// #IGNORE dmem epoch-level communicator

  void 		Init_impl();
  void		Loop();
  void		UpdateState();
  bool 		Crit();
  void		Final();

  virtual void	GetEventList();	// get list of events from environment
  virtual void	GetCurEvent();	// get current event
  virtual void	Loop_UpdateWeights(); // update the weights within the loop (online or small batch)
  virtual void	Final_UpdateWeights(); // update the weights at end of epoch (batch mode or small batch)

  Event*	GetMyNextEvent();

  void		GetCntrDataItems();
  void		GenCntrLog(LogData* ld, bool gen);

  void		NewSeed();
  void		OldSeed();

#ifdef DMEM_COMPILE
  virtual void	AllocProcs(); 	// allocate processors to different tasks..
  static  void	SyncLoopStats(SchedProcess* sp, MPI_Comm ec, int n_stats); // #IGNORE synchronize loop stats across dmem procs
  virtual void	DMem_UpdateWeights(); // synchronize across dmem procs and update the weights
#else
  virtual void	AllocProcs()	{ }; 	// allocate processors to different tasks..
#endif

  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const EpochProcess& cp);
  COPY_FUNS(EpochProcess, SchedProcess);
  TA_BASEFUNS(EpochProcess);
};

class SequenceProcess : public SchedProcess {
  // ##AGGOP_SUM Processes a sequence of trials in one event group (must be under a SequenceEpoch, which loops over event groups)
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
  Event_MGroup*	cur_event_gp;	// #FROM_GROUP_enviro_group event group
  Order		order;		// order to present events in
  StateInit	sequence_init;	// how to initialize network stat at start of sequence

  SequenceEpoch* sequence_epoch;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent epoch process
  int_Array	event_list;	// #HIDDEN list of events
  Event_MGroup*	enviro_group;	// #READ_ONLY #NO_SAVE main event group on environment

  void		Init_impl();
  void		Loop();
  void		UpdateState();
  bool 		Crit();
  void		Final();

  virtual void	GetEventList();	// get list of events from environment
  virtual void	GetCurEvent();
  virtual void	InitNetState();
  virtual void	Loop_UpdateWeights();
  virtual void	Final_UpdateWeights();

  Event*	GetMyNextEvent();

#ifdef DMEM_COMPILE
  virtual void	DMem_UpdateWeights(); // #IGNORE
#endif

  void		GetCntrDataItems();
  void		GenCntrLog(LogData* ld, bool gen);

  void 	UpdateAfterEdit();	// update the sequence_epoch
  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const SequenceProcess& cp);
  COPY_FUNS(SequenceProcess, SchedProcess);
  TA_BASEFUNS(SequenceProcess);
};

class SequenceEpoch : public EpochProcess {
  // Loops over sequences (groups of events) instead of individual events (enviro must have event groups!).
public:
  enum SmallBatchType {
    SEQUENCE,			// at the sequence level (batch_n sequences)
    EVENT			// at the event level (within the sequence, weights updated every batch_n events, with an additional update at end of sequence if needed)
  };

  SmallBatchType small_batch;	// #CONDEDIT_ON_wt_update:SMALL_BATCH how to apply SMALL_BATCH wt_update when using sequences

  Event_MGroup*	cur_event_gp;	// #FROM_GROUP_enviro_group current event group

  void		Init_impl();
  void		Loop();
  void		UpdateState();
  bool		Crit();
  void		Final();

  void		Loop_UpdateWeights();
  void		Final_UpdateWeights();

  void		GetEventList();
  void		GetCurEvent();

  Event*	GetMyNextEvent();

  void		GetCntrDataItems();
  void		GenCntrLog(LogData* ld, bool gen);

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy();
  void	CutLinks();
  void	Copy_(const SequenceEpoch& cp);
  COPY_FUNS(SequenceEpoch, EpochProcess);
  TA_BASEFUNS(SequenceEpoch);
};

class InteractiveEpoch : public EpochProcess {
  // Loops over events in an environment using the interactive interface of GetNextEvent(), which can generate new events based on current state
public:
  int		last_trial_val;	// #READ_ONLY #NO_SAVE last trial.val when GetCurEvent was called -- decide wether its time to get a new event or not

  void		GetEventList();
  void		Loop();
  void		GetCurEvent();

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(InteractiveEpoch);
};

//////////////////////////
// 	TrainProcess	//
//////////////////////////

class NEpochProcess : public SchedProcess {
  // ##AGGOP_LAST Runs epochs to train network
public:
  Counter	epc_ctr; 	// local epoch counter
  int		epoch;		// epoch count on the network itself
  EpochProcess* epoch_proc;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to epoch process

  void		Init_impl();		// get epoch
  void 		UpdateCounters();	// also update the network epoch
  void		GetCntrDataItems();
  void		GenCntrLog(LogData* ld, bool gen);

  void		NewSeed();
  void		OldSeed();

  void	UpdateAfterEdit();		// get the epoch_proc
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const NEpochProcess& cp)		{ epoch = cp.epoch; }
  COPY_FUNS(NEpochProcess, SchedProcess);
  TA_BASEFUNS(NEpochProcess);
};

class TrainProcess : public SchedProcess {
  // ##AGGOP_LAST Runs epochs to train network
public:
  Counter	epoch; 			// Epoch Counter
  EpochProcess* epoch_proc;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to epoch process

  void		Init_impl();		// also initialize network
  void		SetReInit(bool ri_val);
  void 		UpdateCounters();	// also update the network epoch
  void		GetCntrDataItems();
  void		GenCntrLog(LogData* ld, bool gen);

  void		NewSeed();
  void		OldSeed();

  void		InitAll();

  void		Run_gui();
  void		Step();

  void	UpdateAfterEdit();		// get the epoch_proc
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const TrainProcess& cp)		{ epoch = cp.epoch; }
  COPY_FUNS(TrainProcess, SchedProcess);
  TA_BASEFUNS(TrainProcess);
};


//////////////////////////
// 	BatchProcess	//
//////////////////////////

class BatchProcess : public SchedProcess {
  // ##AGGOP_LAST Runs multiple trainings
public:
  Counter	batch;		// number of batches run

  void		NewSeed();
  void		OldSeed();

  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const BatchProcess& cp)		{ batch = cp.batch; }
  COPY_FUNS(BatchProcess, SchedProcess);
  TA_BASEFUNS(BatchProcess);
};

#endif // sched_proc_h
