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
#ifdef TA_GUI
#  include "ta_qttype.h"
#  include "ta_qtdata.h"
#  include "ta_qtdialog.h"
#endif

#include "ta_script.h"
#include "tarandom.h"
#include "minmax.h"
#include "datatable.h"
#include "pdpdeclare.h"
#include "pdpbase.h"
#include "spec.h"
#include "pdpshell.h"

#include "pdp_TA_type.h"

// forwards this file
// from process.h
class CtrlPanelData;
class Process;
class Process_Group;
class Process_MGroup; //

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
class Event_MGroup;
class Environment; //

// from enviro_extra.h
//TODO class ScriptEnv;
//TODO class TimeEnvironment;

// from pdpshell.h
class Project; //

#ifdef TA_GUI
class PDP_API CtrlPanelData : public taOBase {
  // ##NO_TOKENS #INLINE data for the control panel
INHERITED(taOBase)
public:
  bool		active;		// is panel active (save this state)
  float		lft;		// panel window left coord
  float		top;		// panel window top coord

  void 	Initialize();
  void 	Destroy()		{ };
  void 	Copy_(const CtrlPanelData& cp);
  COPY_FUNS(CtrlPanelData, taOBase);
  TA_BASEFUNS(CtrlPanelData);
};
class PDP_API ProcessDialog : public taiEditDataHost {
  // #IGNORE
  Q_OBJECT
public:
  ProcessDialog(void* base, TypeDef* tp, bool readonly = false)
    : taiEditDataHost(base, tp, readonly) { };
  ProcessDialog()				{ };
  ~ProcessDialog();

  virtual bool	CtrlPanel()	{ return false; }

public slots:
  void		Ok();		// override - stop the process when these are hit..
  void		Cancel(); // override
};

class PDP_API Process_RunDlg : public ProcessDialog {
  // #IGNORE
public:
  Process_RunDlg(void* base, TypeDef* tp, bool readonly = false)
    : ProcessDialog(base, tp, readonly) {};
  Process_RunDlg() {};
  ~Process_RunDlg();

  bool	CtrlPanel()	{ return true; }

  bool ShowMember(MemberDef* md);
};

class PDP_API taiProcess : public taiEdit {
  // adds control buttons to the edit window
public:
  taiProcess*	run_ie;

  int		BidForEdit(TypeDef* td);
  override int 	Edit(void* base, bool readonly = false, const iColor* bgclr = NULL); // unusual case, so we override

  void Initialize();
  void Destroy();
  TAQT_EDIT_INSTANCE(taiProcess, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


class PDP_API taiProcessRunBox : public taiProcess {
  // just has the control buttons
public:
  int		BidForEdit(TypeDef*) 	{ return 0; }
  TAQT_EDIT_INSTANCE(taiProcessRunBox, taiProcess);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};

#endif

class PDP_API Process : public taNBase, public ScriptBase {
  // ##EXT_proc simple processes for controlling and coordinating execution
INHERITED(taNBase)
public:
  enum Type {
    C_CODE,			// C code (builtin)
    SCRIPT 			// Script (user-defined)
  };

  RndSeed	rndm_seed;	// #HIDDEN random seed, for NewRun(), ReRun()
  TimeUsed	time_used;	// #HIDDEN accumulated time used during the Run() of this process

  Type    	type;			// process can be builtin c-code or a script
  Modulo	mod;			// flag=run this process or not, m=modulus, run process every m times, off=start runing at this offset
  Project*	project;  		// #READ_ONLY #NO_SAVE project to act on
  Network*	network;  		// #CONTROL_PANEL network to act on
  Environment*  environment;		// #CONTROL_PANEL environmnent to act in
#ifdef TA_GUI
  CtrlPanelData	ctrl_panel;		// #HIDDEN data for the control panel display
#endif

  virtual void	Init() {}
  virtual void	CopyPNEPtrs(Network* net, Environment* env);
  // #IGNORE copy the project, network, env ptrs from
  virtual void	SetDefaultPNEPtrs();
  // #IGNORE get default project, network, env ptrs
  
  // stuff for script_base
  TypeDef*	GetThisTypeDef()	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }
  void		LoadScript(const char* file_nm = NULL);
  bool		RunScript();

  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Process& cp);
  COPY_FUNS(Process, taNBase);
  TA_BASEFUNS(Process);
};

class PDP_API Process_Group : public taBase_Group {
  // ##NO_TOKENS a regular group of processes
public:
  bool		Close_Child(TAPtr obj);

  void	Initialize() 		{ SetBaseType(&TA_Process); }
  void 	Destroy()		{ };
  TA_BASEFUNS(Process_Group);
};


class PDP_API Process_MGroup : public taGroup<Process> {
  // ##NO_TOKENS a menu group for processes
public:

  // support special structure of sched procs here
  bool		Close_Child(TAPtr obj);
  bool		DuplicateEl(TAPtr obj);
  void	Initialize() {SetBaseType(&TA_Process);}
  void 	Destroy()		{ };
  TA_BASEFUNS(Process_MGroup);
};


class PDP_API DataItem : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE source of a piece of data
public:

  String	name;		// #HIDDEN_INLINE name of data item
  String	disp_opts;	// #HIDDEN_INLINE default display options for item
  bool		is_string;	// #HIDDEN_INLINE is a string-valued item
  int		vec_n;		// #HIDDEN_INLINE length of vector (0 if not)

  virtual void	SetStringName(const char* nm);
  virtual void	SetNarrowName(const char* nm);
  virtual void	SetFloatVecNm(const char* nm, int n);
  virtual void	SetStringVecNm(const char* nm, int n);
  
  virtual void 	AddDispOption(const char* opt);
  bool		HasDispOption(const char* opt) const
  { return disp_opts.contains(opt); } // check if a given display option is set
  
  bool 		SetName(const char* nm) 	{ name = nm; return true; }
  bool 		SetName(const String& nm) 	{ name = nm; return true; }
  String	GetName() const			{ return name; }

  void	Initialize();
  void	Destroy()	{ };
  void	Copy_(const DataItem& cp);
  COPY_FUNS(DataItem, taOBase);
  TA_BASEFUNS(DataItem);
};

class PDP_API DataItem_List : public taList<DataItem> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of DataItem objects
public:
  void	Initialize() 		{SetBaseType(&TA_DataItem); };
  void 	Destroy()		{ };
  TA_BASEFUNS(DataItem_List);
};


class PDP_API StatVal : public DataItem {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE Statistic value
public:
  float		val;		// value of statistic
  String	str_val;	// #HIDDEN_INLINE value of statistic if its a string
  CritParam	stopcrit;	// Stopping Criteria Parameters

  void		InitStat(float value=0.0)	{ val = value; }
  void 		Init()		{ stopcrit.Init(); }

  void 	Initialize();
  void 	Destroy()		{ CutLinks();};
  void	InitLinks();
  void	CutLinks() {stopcrit.CutLinks(); DataItem::CutLinks();}
  void	Copy_(const StatVal& cp);
  COPY_FUNS(StatVal, DataItem);
  TA_BASEFUNS(StatVal);
};


class PDP_API StatVal_List : public taBase_List {
  // ##NO_UPDATE_AFTER group of stat values
public:
  virtual bool	HasStopCrit();
  virtual void 	Init();	  	 	  // initialize process (eg. n_met) (not value!)
  virtual void	InitStat(float value=0.0); // initialize statistic value
  virtual void	NameStatVals(const char* nm, const char* opts="", bool is_string=false);
  void	Initialize() 		{ SetBaseType(&TA_StatVal); }
  void 	Destroy()		{ };
  TA_BASEFUNS(StatVal_List);
};


class PDP_API StatValAgg : public Aggregate {
  // #INLINE Aggregation for StatVal-based values
public:
/*  void 		ComputeAgg(float& to, float fm)
  { Aggregate::ComputeAgg(to, fm); }
  bool		ComputeAggNoUpdt(float& to, float fm)
  { return Aggregate::ComputeAggNoUpdt(to, fm); }

  void		NewCopyAgg(StatVal* fm);
  // make a new copy aggregate

  void		ComputeAgg(StatVal* to, StatVal* fm);
  // compute aggregation into `to' based on current op from `fm'
  void		ComputeAgg(StatVal* to, float fm_val);
  // compute aggregation into `to' based on current op from value fm_val
  bool		ComputeAggNoUpdt(StatVal* to, StatVal* fm);
  // compute aggregation but don't update the n_updt counter (for lists) (returns false if 0, else true)

  virtual void	ComputeAggs(StatVal_List* to, StatVal_List* fm);
  // compute aggregation for whole list of statvals (updt after list)
  virtual bool	ComputeAggsNoUpdt(StatVal_List* to, StatVal_List* fm);
  // compute aggregation for whole list of statvals (no updt at all) (returns false if all 0, else true)

  virtual bool	AggFromCopy(StatVal_List* fm);
  // from list is the copy_vals of another stat, goes to copy_vals in owner stat
*/
  void 	Initialize() {}
  void	Destroy() {}
  TA_BASEFUNS(StatValAgg);
};

class PDP_API AggStat : public StatValAgg {
  // #INLINE Aggregate statistics over time (processing levels)
public:
  Stat*		real_stat;	// #READ_ONLY #NO_SAVE the 'real' (non-agg) stat
  Stat* 	from;
  // which statisitic to aggregate from (a statistic of a sub process)
  bool		type_safe;
  // #HIDDEN true if from ptr should be kept type-safe

  virtual void	FindRealStat();	// find and set the real_stat

  virtual void	SetFrom(Stat* frm); // set the from field (convenience)
  String	AppendAggName(const char* nm) const;
  String	PrependAggName(const char* nm) const;

  void 	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	CutLinks();
  void	Copy_(const AggStat& cp);
  COPY_FUNS(AggStat, StatValAgg);
  TA_BASEFUNS(AggStat);
};

class PDP_API Stat : public Process {
  // Generic Statistic Process
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

  virtual float	InitStatVal();	// value to pass to the InitStat() function
  virtual void	InitStat_impl(); // #IGNORE inits generic stat vars (not member statvals)
  virtual void 	InitStat();    	// initializes the stat vars, but not the stat process
  virtual void	Init_impl();	// #IGNORE inits generic stats (not member statvals)
  virtual void 	Init();		// initializes the process (including stopcrits), calls InitStat()


  virtual void	DeleteAggregates();
  // #MENU #UPDATE_MENUS delete aggregators of this stat
  virtual void	UpdateAggregates();
  // #MENU #UPDATE_MENUS update aggregators of this stat (i.e., so they get the layer name or other change)
  virtual Stat*	FindAggregator();
  // #MENU #MENU_SEP_BEFORE #USE_RVAL Find the aggregator for this stat in the next-highest level proc

  virtual bool 	FindOwnAggFrom(const Stat& cp);
  // set our time_agg.from to stat in same hierarch position as cp.time_agg.from (for copying/duplicating)

  virtual void	NameStatVals();
  // #IGNORE give default names to statvals (overload to set specific options, etc.)
  virtual bool	HasStopCrit();
  // #IGNORE check if any of the statvals have crit flags set, set has_stop_crit
  virtual const char* AltTypeName() { return GetTypeDef()->name; }
  // can substitute a shorter name if desired, for naming the objects

  void 	UpdateAfterEdit();

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const Stat& cp);
  COPY_FUNS(Stat, Process);
  TA_BASEFUNS(Stat);
};

class PDP_API Stat_Group : public taBase_Group {
  // ##NO_TOKENS a group of statistics
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  bool		Close_Child(TAPtr obj);

  virtual Stat*		FindMakeStat(TypeDef* td, const char* nm = NULL, bool& nw_itm = nw_itm_def_arg);
  // find a stat of the given type and name (if non-null), and if not found, make it
  virtual Stat*		FindAggregator(Stat* of_stat, Aggregate::Operator agg_op = Aggregate::DEFAULT);
  // find a statistic that is aggregating a given statistic using specified agg_op (DEFAULT = don't care about agg_op)
  virtual MonitorStat*	FindMonitor(TAPtr of_obj, const char* of_var = NULL);
  // find a statistic that is monitoring given object and variable
  virtual MonitorStat*	FindMakeMonitor(TAPtr of_obj, const char* of_var, bool& nw_itm = nw_itm_def_arg);
  // find a statistic that is monitoring given object and variable, and if not found, make one

  void	Initialize() 		{ SetBaseType(&TA_Stat); }
  void 	Destroy()		{ };
  TA_BASEFUNS(Stat_Group);
};

// SE_Stat and MonitorStat are so basic that they are here, and not in extra

class PDP_API SE_Stat : public Stat {
  // ##COMPUTE_IN_TrialProcess Squared Error Statistic
public:
  StatVal	se;			// squared errors
  float		tolerance;		// if error is less than this, its 0

  void		InitStat();
  void		Init();

  void		NameStatVals();

  void	Initialize();
  void 	Destroy()		{ };
  SIMPLE_COPY(SE_Stat);
  COPY_FUNS(SE_Stat, Stat);
  TA_BASEFUNS(SE_Stat);
};

class PDP_API MonitorStat: public Stat {
  // ##COMPUTE_IN_TrialProcess Network Monitor Statistic
public:
  static String GetObjName(TAPtr obj); // get name of object for naming stats, etc
  
  StatVal_List	mon_vals;	// the values of the stat as computed directly
  MemberSpace   members;	// #IGNORE memberdefs
  taBase_List	ptrs;     	// #HIDDEN #NO_SAVE actual ptrs to values
  taBase_List 	objects;	// #LINK_GROUP Group of network objects
  String        variable;	// Variable (member) to monitor
  SimpleMathSpec pre_proc_1;	// first step of pre-processing to perform
  SimpleMathSpec pre_proc_2;	// second step of pre-processing to perform
  SimpleMathSpec pre_proc_3;	// third step of pre-processing to perform

  void		InitStat();
  void		Init();

  // these are for finding the members and building the stat
  // out of the objects and the variable
  void		ScanObjects();	// #IGNORE
  void		ScanLayer(Layer* lay); // #IGNORE
  void		ScanUnitGroup(Unit_Group* ug);	// #IGNORE
  void		ScanUnit(Unit* u,Projection* p=NULL); // #IGNORE
  void		ScanConGroup(Con_Group* cg,char* varname,Projection* p=NULL); // #IGNORE
  void		ScanProjection(Projection* p); // #IGNORE

  virtual void	SetVariable(const char* varnm); // set variable and update appropriately
  virtual void	SetObject(TAPtr obj); // clear any existing objects and set to obj and update
  virtual void	AddObject(TAPtr obj); // add obj and update


  void	NameStatVals();
  void	UpdateAfterEdit();

  const char* AltTypeName() { return "Mon"; }

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const MonitorStat& cp);
  COPY_FUNS(MonitorStat, Stat);
  TA_BASEFUNS(MonitorStat);
};


class PDP_API Counter : public taBase {
  // #INLINE ##NO_TOKENS #NO_UPDATE_AFTER Holds the value of a loop counter
public:
  String	name;			// #HIDDEN not an taNBase to hide name
  int 		val;			// #IV_READ_ONLY #SHOW value of the counter
  int 		max;			// maximum value of the counter

  void  SetMax(int m) {max = m;}
  void 	Initialize()		{ val = 0; max = 1; }
  void 	Destroy()		{ };
  void	Copy_(const Counter& cp)	{ name = cp.name; val = cp.val; max = cp.max; }
  COPY_FUNS(Counter, taBase);
  TA_BASEFUNS(Counter);
};


class PDP_API StepParams : public taBase {
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
  TypeDef*	sub_proc_type;		// #DETAIL #TYPE_SchedProcess #NULL_OK type of sub-process to use
  SchedProcess*	sub_proc;		// #DETAIL Sub-process called by this
  StepParams	step;			// #CONTROL_PANEL Which process to step and n_steps

  Stat_Group	loop_stats;  		// #DETAIL #IN_GPMENU #BROWSE Statistics Computed in Loop
  Stat_Group	final_stats;  		// #DETAIL #IN_GPMENU #BROWSE Statistics Computed after Loop
  Process_Group	init_procs;		// #DETAIL #IN_GPMENU #BROWSE Misc procs run when process is initialized
  Process_Group loop_procs;		// #DETAIL #IN_GPMENU #BROWSE Misc procs run in the loop, using mod based on loop counter
  Process_Group	final_procs;		// #DETAIL #IN_GPMENU #BROWSE Misc procs run after process is finished
//obs  WinView_Group	displays;		// #LINK_GROUP #HIDDEN  #BROWSE views to update
  taBase_Group	logs;			// #LINK_GROUP #HIDDEN  #BROWSE Logs to log to NOTE: was PDPLog_Group in v3
  bool		log_loop;		// Log the loop state (instead of final state)
  bool		log_counter;		// Log the counter values for this process

  virtual void	Init_impl();		// 

  void 		GetCntrDataItems();
  void 		UpdateLogUpdaters();

  // simple process manipulation subroutines to make life a little easier
  virtual SchedProcess* FindSubProc(TypeDef* td); // find of a given type
  virtual SchedProcess* FindSuperProc(TypeDef* td); // find of a given type
  virtual SchedProcess* FindProcOfType(TypeDef* td);
  // find of a given type, including this proc, super and sub procs
  virtual SchedProcess* GetTopProc(); // #IGNORE get the highest-level process
  virtual void	UnLinkSubProc(); // #IGNORE disconnect from sub process
  virtual void	UnLinkFmSuperProc(); // #IGNORE disconnect me from super process
  virtual void	LinkSubProc(SchedProcess* proc); // #IGNORE link proc object as new sub process (not deleting old one)
  virtual void	LinkSuperProc(SchedProcess* proc); // #IGNORE link proc object as new super process (not deleting old one)
  virtual void	GetAggsFmSuperProc(); // #IGNORE fill in my aggregators from aggs on my super proc
  virtual void	SetAggsAfterRmvSubProc(); // #IGNORE set my aggs to point to my new sub proc stats after removing old one
  virtual void	RemoveSuperProc();
  //  remove super process above this one in hierarchy
  virtual void	RemoveSubProc();
  //  remove sub process below this one in hierarchy
  virtual void	CreateSubProcs(bool update=true);
  virtual void  RemoveFromLogs();//
  
  void 		SetDefaultName();			    // #IGNORE
  void 	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy() {CutLinks();}
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const SchedProcess& cp);
  COPY_FUNS(SchedProcess, Process);
  TA_BASEFUNS(SchedProcess);
};


//////////////////////////
// 	CycleProcess	//
//////////////////////////

class PDP_API CycleProcess : public SchedProcess {
  // ##AGGOP_SUM Runs one cycle of activation update
public:
  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(CycleProcess);
};


//////////////////////////
// 	SettleProcess	//
//////////////////////////

class PDP_API SettleProcess : public SchedProcess {
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

class PDP_API TrialProcess : public SchedProcess {
  // ##AGGOP_SUM Runs a single trial (one event)
public:
  Event* 	cur_event;
  // #FROM_GROUP_enviro_group the current event (copied from the EpochProc)
  EpochProcess* epoch_proc;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent epoch_proc
  Event_MGroup*	enviro_group;	// #READ_ONLY #NO_SAVE main event group on environment

  void 		Init_impl();

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

class PDP_API EpochProcess : public SchedProcess {
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
  
  void 		Init_impl();

  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const EpochProcess& cp);
  COPY_FUNS(EpochProcess, SchedProcess);
  TA_BASEFUNS(EpochProcess);
};

class PDP_API SequenceProcess : public SchedProcess {
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

  virtual void	GetEventList();	// get list of events from environment
  
  void 	UpdateAfterEdit();	// update the sequence_epoch
  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const SequenceProcess& cp);
  COPY_FUNS(SequenceProcess, SchedProcess);
  TA_BASEFUNS(SequenceProcess);
};

class PDP_API SequenceEpoch : public EpochProcess {
  // Loops over sequences (groups of events) instead of individual events (enviro must have event groups!).
public:
  enum SmallBatchType {
    SEQUENCE,			// at the sequence level (batch_n sequences)
    EVENT			// at the event level (within the sequence, weights updated every batch_n events, with an additional update at end of sequence if needed)
  };

  SmallBatchType small_batch;	// #CONDEDIT_ON_wt_update:SMALL_BATCH how to apply SMALL_BATCH wt_update when using sequences

  Event_MGroup*	cur_event_gp;	// #FROM_GROUP_enviro_group current event group

  void		Init_impl();

  void		GetEventList();

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy();
  void	CutLinks();
  void	Copy_(const SequenceEpoch& cp);
  COPY_FUNS(SequenceEpoch, EpochProcess);
  TA_BASEFUNS(SequenceEpoch);
};

class PDP_API InteractiveEpoch : public EpochProcess {
  // Loops over events in an environment using the interactive interface of GetNextEvent(), which can generate new events based on current state
public:
  int		last_trial_val;	// #READ_ONLY #NO_SAVE last trial.val when GetCurEvent was called -- decide wether its time to get a new event or not

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(InteractiveEpoch);
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

  void		Init_impl();		// get epoch
  void		GetCntrDataItems();

  void	UpdateAfterEdit();		// get the epoch_proc
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const NEpochProcess& cp)		{ epoch = cp.epoch; }
  COPY_FUNS(NEpochProcess, SchedProcess);
  TA_BASEFUNS(NEpochProcess);
};

class PDP_API TrainProcess : public SchedProcess {
  // ##AGGOP_LAST Runs epochs to train network
public:
  Counter	epoch; 			// Epoch Counter
  EpochProcess* epoch_proc;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to epoch process

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

class PDP_API BatchProcess : public SchedProcess {
  // ##AGGOP_LAST Runs multiple trainings
public:
  Counter	batch;		// number of batches run

  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const BatchProcess& cp)		{ batch = cp.batch; }
  COPY_FUNS(BatchProcess, SchedProcess);
  TA_BASEFUNS(BatchProcess);
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
  // ##SCOPE_Environment sub-spec for patterns within an eventspec
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

  virtual float Value(Pattern* pat, int index);
  // return value at given index from pattern (order can be changed, eg GroupPatternSpec)
  virtual int	Flag(PatUseFlags flag_type, Pattern* pat, int index);
  // return flag at given index from pattern (order can be changed, eg GroupPatternSpec)

  void	UpdateAfterEdit();	// gets information off of the layer, etc
  void	Initialize();
  void 	Destroy();
  void  InitLinks();
  void	CutLinks();
  void 	Copy_(const PatternSpec& cp);
  COPY_FUNS(PatternSpec, BaseSubSpec);
  TA_BASEFUNS(PatternSpec); //
};

class PDP_API PatternSpec_Group : public taBase_Group {
  // ##SCOPE_Environment group of pattern specs (acts like a template for pattern groups)
INHERITED(taBase_Group)
public:
  TypeDef*	pat_gp_type;	// #TYPE_Pattern_Group type of pattern group to use

  void	Initialize();
  void 	Destroy();
  void	CutLinks();
  void 	Copy_(const PatternSpec_Group& cp);
  COPY_FUNS(PatternSpec_Group, taBase_Group);
  TA_BASEFUNS(PatternSpec_Group);
};

class PDP_API EventSpec : public BaseSpec {
  // ##SCOPE_Environment ##MEMB_IN_GPMENU ##IMMEDIATE_UPDATE event specification
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
  void	InitLinks();
  void	CutLinks();
  void 	Copy(const EventSpec& cp);
  TA_BASEFUNS(EventSpec);
};

class PDP_API EventSpec_SPtr : public SpecPtr<EventSpec> {
public:
  BaseSpec_MGroup*	GetSpecGroup();	// event specs go in environment
  void 	Initialize() 		{ };
  void	Destroy()		{ };
  TA_BASEFUNS(EventSpec_SPtr);
};


////////////////////////
//   Pattern/Event    //
////////////////////////

class PDP_API Pattern : public taOBase {
  // ##SCOPE_Environment ##EXT_pat ##NO_TOKENS ##NO_UPDATE_AFTER Contains activation values to be applied to a network layer
public:
  float_RArray 	value;  	// Values of Pattern
  int_Array   	flag;  		// Flags of Pattern
  
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const Pattern& cp);
  COPY_FUNS(Pattern, taOBase);
  TA_BASEFUNS(Pattern);
};

BaseGroup_of(Pattern);


class PDP_API Event : public taNBase {
  // ##SCOPE_Environment ##EXT_evt ##NO_TOKENS ##NO_UPDATE_AFTER Contains patterns of activation for different layers in the network specifying one event
public:
  int			index;		// #NO_SAVE #READ_ONLY Index of this event within group
  Pattern_Group 	patterns;  	// #NO_SAVE_PATH_R group of patterns
  EventSpec_SPtr	spec;		// determines the configuration of patterns and how they are presented to the network

  void	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void 	Copy(const Event& cp);
  TA_BASEFUNS(Event);
};

// environment presents the following model: either a flat list of events
// (interface = EventCount() && GetEvent()) or a set of event-groups (leaf groups)
// (interface = GroupCount() && GetGroup())
// when events are generated algorithmically, the results are put in a set of event
// structures, and the same interface can be used (see ProcEnvironment below)
// other models are definable, but the standard EpochProcess will not
// understand them.

class PDP_API Event_MGroup : public taGroup<Event> {
  // ##SCOPE_Environment Group of events
protected:
  void	El_SetIndex_(void* base, int idx) { ((Event*)base)->index = idx; }
public:

  void	Initialize() {SetBaseType(&TA_Event);}
  void 	Destroy()		{ };
  TA_BASEFUNS(Event_MGroup);
}; //

////////////////////////
//   Environment      //
////////////////////////

class PDP_API Environment : public taNBase {
  // ##EXT_env ##COMPRESS basic environment: contains events to present to the network, and can be used to hold data for analysis
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

  BaseSpec_MGroup 	event_specs;	// specs for events: controls the layout and configuration of events
  Event_MGroup 		events;		// the events, contain patterns that map onto layers of the network
  int			event_ctr; 	// #READ_ONLY #SHOW counter for interactive interface with environment: number of events processed since last InitEvents()

  // the flat event list model of the environment
  virtual int	EventCount()		{ return events.leaves; }
  // #MENU #MENU_ON_Actions #USE_RVAL #MENU_SEP_BEFORE number of events in environment
  virtual Event* GetEvent(int ev_index)	{ return (Event*)events.Leaf_(ev_index); }
  // get the event at given index in a flat list of all events in the environment

  // the leaf-group model of the environment
  virtual int	GroupCount();
  // #MENU #USE_RVAL number of event groups in environment
  virtual Event_MGroup* GetGroup(int gp_index);
  // get the event group (collection of events) at the specified index of all groups in the environment

  void	UpdateAfterEdit();

  void	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void 	Copy(const Environment& cp);
  TA_BASEFUNS(Environment); //

};

// note: Environment_MGroup name is for compatiblity with v3.2 files
class PDP_API Environment_MGroup : public taGroup<Environment> {
  // group of environments
public:
  void  Initialize()            { SetBaseType(&TA_Environment); }
  void  Destroy()               { };
  TA_BASEFUNS(Environment_MGroup);
};


class PDP_API ScriptEnv : public Environment, public ScriptBase {
  // For algorithmically generated events: Initialization of events is done by a script at the start of each epoch through the InitEvents() function
public:
  SArg_Array	s_args;		// string-valued arguments to pass to script


  void	UpdateAfterEdit();

  void 	Initialize()	{ };
  void 	Destroy()	{ };
  void	InitLinks();
  void	Copy_(const ScriptEnv& cp);
  COPY_FUNS(ScriptEnv, Environment);
  TA_BASEFUNS(ScriptEnv);
};

class PDP_API InteractiveScriptEnv : public ScriptEnv {
  // For interactively-generated environments: Script is called for each event in GetNextEvent function (use with InteractiveEpoch)
public:
  void 	Initialize() {}
  void 	Destroy()	{ };
  TA_BASEFUNS(InteractiveScriptEnv); //
  
};


//////////////////////////
//      Frequency 	//
//////////////////////////

class PDP_API FreqEvent : public Event {
  // an event that has a frequency associated with it
public:
  float 	frequency;	// #ENVIROVIEW_freq frequency of occurance for this event

  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const FreqEvent& cp);
  COPY_FUNS(FreqEvent, Event);
  TA_BASEFUNS(FreqEvent);
};

class FreqEnv;

class PDP_API FreqEvent_Group : public Event_MGroup {
  // an event group that has a frequency associated with it
public:
  FreqEnv*	fenv;		// #READ_ONLY #NO_SAVE parent frequency environment
  int_Array	list;		// #HIDDEN list of event indicies to present for GROUP_EVENT
  float		frequency;	// frequency of occurance for this group of events

  void	Initialize();
  void 	Destroy()		{CutLinks(); };
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const FreqEvent_Group& cp);
  COPY_FUNS(FreqEvent_Group, Event_MGroup);
  TA_BASEFUNS(FreqEvent_Group);
};


class PDP_API FreqEnv : public Environment {
  // environment which has a frequency for each event
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

  // the event model of the environment
  int	 	EventCount();
  Event* 	GetEvent(int i);

  // the group model of the environment
  int	 	GroupCount();		// number of groups in the environment
  Event_MGroup* GetGroup(int i); 	// return the ith event group

  void	Initialize();
  void 	Destroy()		{ CutLinks();};
  void	InitLinks();
  void 	CutLinks();
  void	Copy_(const FreqEnv& cp);
  COPY_FUNS(FreqEnv, Environment);
  TA_BASEFUNS(FreqEnv);
};


//////////////////////////
//         Time 	//
//////////////////////////

class PDP_API TimeEvent : public Event {
  // an event which occurs at a specific time
public:
  float		time;		// #ENVIROVIEW time at which it should appear

  void 	Initialize() {time = 0.0f;}
  void	Destroy()		{ };
  void	Copy_(const TimeEvent& cp) {time = cp.time;}
  COPY_FUNS(TimeEvent, Event);
  TA_BASEFUNS(TimeEvent);
};

class PDP_API TimeEvent_MGroup : public Event_MGroup {
  // a group of time-based events
public:
  enum Interpolate {
    PUNCTATE,			// events appear for a single instant only
    CONSTANT,			// events persist constantly, change discretely
    LINEAR,			// linear interpolation is performed between events
    USE_ENVIRO 			// use interpolation specified in the environment
  };

  Interpolate	interpolate;	// if and how to interpolate between given event times
  float		end_time;	// time this sequence ends at

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void	Copy_(const TimeEvent_MGroup& cp);
  COPY_FUNS(TimeEvent_MGroup, Event_MGroup);
  TA_BASEFUNS(TimeEvent_MGroup);
};

class PDP_API TimeEnvironment : public Environment {
  // an environment that manages time-based events
public:
  enum Interpolate {
    PUNCTATE,			// events appear for a single instant only
    CONSTANT,			// events persist constantly, change discretely
    LINEAR 			// linear interpolation is performed between events
  };

  Interpolate	interpolate;    // if and how to interpolate between given event times

  void 	Initialize();
  void	Destroy()		{ };
  void 	InitLinks();
  void	Copy_(const TimeEnvironment& cp);
  COPY_FUNS(TimeEnvironment, Environment);
  TA_BASEFUNS(TimeEnvironment);
};

//////////////////////////
//     FreqTime 	//
//////////////////////////

class PDP_API FreqTimeEvent : public TimeEvent {
  // a time event that has a frequency associated with it
public:
  float 	frequency;	// #ENVIROVIEW_freq frequency of occurance for this event

  void	Initialize() {frequency = 0.0f;}
  void 	Destroy()		{ };
  void	Copy_(const FreqTimeEvent& cp){frequency = cp.frequency;}
  COPY_FUNS(FreqTimeEvent, TimeEvent);
  TA_BASEFUNS(FreqTimeEvent);
};

class PDP_API FreqTimeEvent_Group : public TimeEvent_MGroup {
  // a time event group that has a frequency associated with it
public:
  float		frequency;	// frequency of occurance for this group of events

  void	Initialize(){frequency = 0.0f;}
  void 	Destroy()		{ };
  void	Copy_(const FreqTimeEvent_Group& cp){frequency = cp.frequency;}
  COPY_FUNS(FreqTimeEvent_Group, TimeEvent_MGroup);
  TA_BASEFUNS(FreqTimeEvent_Group);
};


class PDP_API FreqTimeEnv : public TimeEnvironment {
  // a time environment which has a frequency for each event
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

  // the event model of the environment
  int	 	EventCount();
  Event* 	GetEvent(int i);

  // the group model of the environment
  int	 	GroupCount();		// number of groups in the environment
  Event_MGroup* GetGroup(int i); 	// return the ith event group

  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const FreqTimeEnv& cp);
  COPY_FUNS(FreqTimeEnv, TimeEnvironment);
  TA_BASEFUNS(FreqTimeEnv);
};


//////////////////////////
//     Probability 	//
//////////////////////////

class PDP_API ProbPattern : public Pattern {
  // pattern is chosen from group of patterns with given probability
public:
  float    	prob;		// #ENVIROVIEW probability of showing this pattern
  bool		applied;	// #READ_ONLY #NO_SAVE whether it was applied

  void 	Initialize();
  void	Destroy()	{ };
  void	InitLinks();
  void 	Copy_(const ProbPattern& cp);
  COPY_FUNS(ProbPattern, Pattern);
  TA_BASEFUNS(ProbPattern);
};

class PDP_API ProbPatternSpec_Group : public PatternSpec_Group {
  // defines a group of patterns that are chosen according to their probabilities
public:

  int 	last_pat; 	 // #HIDDEN #NO_SAVE last pattern chosen

  void UpdateAfterEdit();
  void CutLinks();
  void Initialize();
  void Destroy()	{ };
  TA_BASEFUNS(ProbPatternSpec_Group);
};

class PDP_API ProbEventSpec : public EventSpec {
  // events have probabalistically-chosen patterns contained in ProbPatternSpec_Groups
public:
  float		default_prob;	// default probability

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(ProbEventSpec);
};

//////////////////////////
//     XY Offset	//
//////////////////////////

class PDP_API XYPatternSpec : public PatternSpec {
  // for patterns that are positioned at a particular x,y offset location
public:
  bool		wrap;
  // whether to wrap around target layer if pattern extends beyond coords
  bool		apply_background;
  // whether to give all units not in the pattern a background value
  float		background_value;
  // value to apply to all other units in the layer (if applied)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(XYPatternSpec);
  COPY_FUNS(XYPatternSpec, PatternSpec);
  TA_BASEFUNS(XYPatternSpec);
};

class PDP_API XYPattern : public Pattern {
  // specifies the x,y offset location of the pattern in the layer
public:
  TwoDCoord	offset;		// #ENVIROVIEW offset within network layer for pattern

  void	Initialize()	{ };
  void	Destroy()	{ };
  SIMPLE_COPY(XYPattern);
  COPY_FUNS(XYPattern, Pattern);
  TA_BASEFUNS(XYPattern);
};


//////////////////////////
//     XY Subset	//
//////////////////////////

class PDP_API XYSubPatternSpec : public PatternSpec {
  // presents rectagular subsets (size of layer) of large patterns at x,y offset
public:
  bool		wrap;
  // whether to wrap around pattern if layer extends beyond coords

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(XYSubPatternSpec);
  COPY_FUNS(XYSubPatternSpec, PatternSpec);
  TA_BASEFUNS(XYSubPatternSpec);
};

class PDP_API XYSubPattern : public Pattern {
  // specifies the x,y offset location of the layer within the pattern
public:
  TwoDCoord	offset;		// #ENVIROVIEW offset within pattern for network layer

  void	Initialize()	{ };
  void	Destroy()	{ };
  SIMPLE_COPY(XYSubPattern);
  COPY_FUNS(XYSubPattern, Pattern);
  TA_BASEFUNS(XYSubPattern);
};


//////////////////////////
//     GroupPattern	//
//////////////////////////

class PDP_API GroupPatternSpec : public PatternSpec {
  // organizes pattern values into sub-groups for viewing and/or sending to network
public:
  PosTDCoord	sub_geom;
  // geometry of the individual sub-groups: must evenly divide into overall geom in both x & y
  PosTDCoord	gp_geom;
  // #READ_ONLY geometry of the groups within overall geom (just geom / sub_geom)
  bool		trans_apply;
  // translate apply of values to network (only if units are flat, not grouped!)

  int	FlatToValueIdx(int index);
  // translate given index from a flat view into the value index taking into account groups
  int	CoordToValueIdx(const TwoDCoord& gp_coord, const TwoDCoord& sub_coord);
  // get index into actual values from given coordinates of group and sub-group
  int	ValueToFlatIdx(int index);
  // translate given index of a value into a flat view taking into account groups
  int	CoordToFlatIdx(const TwoDCoord& gp_coord, const TwoDCoord& sub_coord);
  // get index into flat structure from given coordinates of group and sub-group

  float Value(Pattern* pat, int index);
  int	Flag(PatUseFlags flag_type, Pattern* pat, int index);

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(GroupPatternSpec);
  COPY_FUNS(GroupPatternSpec, PatternSpec);
  TA_BASEFUNS(GroupPatternSpec);
};

//////////////////////////////////////////
// 	Duration  Events		//
//////////////////////////////////////////

class PDP_API DurEvent : public Event {
  // an event which lasts for a particular amount of time
public:
  float		duration;	// #ENVIROVIEW length of time (cycles) event should be presented

  void 	Initialize(){ duration = 50.0f;}
  void	Destroy()		{ };
  void	Copy_(const DurEvent& cp) {duration = cp.duration;}
  COPY_FUNS(DurEvent, Event);
  TA_BASEFUNS(DurEvent);
};

//////////////////////////////////////////
// 	Read from File  		//
//////////////////////////////////////////

class PDP_API FromFileEnv : public Environment {
  // Environment that reads events incrementally from a file into events. NOT SUPPORTED IN CONVERSION
public:
/*  enum 	ReadMode {
    ONE_EPOCH,			// read one epoch at a time, using InitEvents interface
    ONE_EVENT			// read one event at a time, using GetNextEvent interface (requires InteractiveEpoch process)
  };

  ReadMode	read_mode;	// how to read in events: either one epoch or one event at a time (one event requires InteractiveEpoch process)
  taFiler*	event_file;	// file to read events from
  TextFmt	text_file_fmt;	// format of text file
  bool		binary;		// file is binary (written by WriteBinary). Otherwise, its Text as readable by ReadText
  int		events_per_epc;	// how many events to present per epoch
  int		file_pos;	// #READ_ONLY #SHOW #NO_SAVE position (in terms of events) within the file

  virtual void	ReadEvent(Event* ev);
  // read from file into one event (assumes file is open, etc)

  void		InitEvents();
  Event* 	GetEvent(int ev_index);
  Event* 	GetNextEvent();
*/
  void 	Initialize() {}
  void	Destroy() {}
//  void 	InitLinks();
//  void	Copy_(const FromFileEnv& cp);
//  COPY_FUNS(FromFileEnv, Environment);
  TA_BASEFUNS(FromFileEnv);
};

class PDP_API Project : public ProjectBase {
  // #HIDDEN for loading legacy (v3.x) projects only
INHERITED(ProjectBase)
public:
  Environment_MGroup	environments;	// #SHOW #NO_SAVE Environments of patterns to present to networks //TODO: legacy, make hidden
  Process_MGroup	processes;	// #SHOW #NO_SAVE Processes to coordinate training/testing, etc//TODO: legacy, make hidden

  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const Project& cp);
  COPY_FUNS(Project, ProjectBase);
  TA_BASEFUNS(Project);
};


#endif

