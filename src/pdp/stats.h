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



// stats.h

#ifndef stats_h
#define stats_h

#include "process.h"
#include "netstru.h"
#include "datatable.h"		// for float_RArray and Aggregate

#include <math.h>

#ifdef Aggregate
#undef Aggregate
#endif

class CritParam : public taBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE stopping criteria params
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

class StatVal : public DataItem {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE Statistic value
public:
  float		val;		// value of statistic
  String	str_val;	// #HIDDEN_INLINE value of statistic if its a string
  CritParam	stopcrit;	// Stopping Criteria Parameters

  float		operator=(int r)	{ return val = (float) r; }
  float 	operator=(float r)	{ return val = r; }
  float 	operator+=(float r)	{ return val += r; }
  float 	operator-=(float r)	{ return val -= r; }
  float 	operator*=(float r)	{ return val *= r; }
  float 	operator/=(float r)	{ return val /= r; }
  operator 	float& () 		{ return val; }
  operator 	float* () 		{ return &val; }

  void		InitStat(float value=0.0)	{ val = value; }
  void 		Init()		{ stopcrit.Init(); }
  bool 		Crit()		{ return stopcrit.Evaluate(val); }

  void 	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const StatVal& cp);
  COPY_FUNS(StatVal, DataItem);
  TA_BASEFUNS(StatVal);
};

class StatVal_List : public taBase_List {
  // ##NO_UPDATE_AFTER group of stat values
public:
  void operator=(int r);
  void operator=(float r);

  virtual void	InitStat(float value=0.0); // initialize statistic value
  virtual void 	Init();	  	 	  // initialize process (eg. n_met) (not value!)
  virtual bool 	Crit();		 	  // does an OR of each member

  virtual void	NameStatVals(const char* nm, const char* opts="", bool is_string=false);
  // #MENU #MENU_ON_Actions give statvals default names, options
  virtual bool	HasStopCrit();
  // #MENU #USE_RVAL check if any of the statvals have crit flags set

  virtual void	GenLogData(LogData* ld);  // generate log data

  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Stat); }

  void	Initialize() 		{ SetBaseType(&TA_StatVal); }
  void 	Destroy()		{ };
  TA_BASEFUNS(StatVal_List);
};

class StatValAgg : public Aggregate {
  // #INLINE Aggregation for StatVal-based values
public:
  void 		ComputeAgg(float& to, float fm)
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

  void 	Initialize();
  void	Destroy();
  TA_BASEFUNS(StatValAgg);
};

class AggStat : public StatValAgg {
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

class Stat : public Process {
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

  void		C_Code();
  LogData& 	GenLogData(LogData* ld=NULL); 	// generate log data

  // it is reccommended that you overwrite these two functions to call .Init{Stat}
  // on your StatVal{_Group} members explicitly, and agg.Init(); InitStat(); in Init()
  // these versions require a search through the members, which is slow..
  virtual float	InitStatVal();	// value to pass to the InitStat() function
  virtual void	InitStat_impl(); // #IGNORE inits generic stat vars (not member statvals)
  virtual void 	InitStat();    	// initializes the stat vars, but not the stat process
  virtual void	Init_impl();	// #IGNORE inits generic stats (not member statvals)
  virtual void 	Init();		// initializes the process (including stopcrits), calls InitStat()
  virtual bool	Crit();    	// check criteria on stat values

  // these are looping and checking functions, SET TO {} to not compute over something
  virtual void 	Network_Run(); 			// Compute over the network
  virtual void 	Layer_Run();			// Compute over layers in the network
  virtual void 	Unit_Run(Layer* lay);		// Compute over units in a layer
  virtual void 	RecvCon_Run(Unit* unit);	// Compute over recv conns of a unit
  virtual void 	SendCon_Run(Unit* unit);	// Compute over send conns of a unit

  // these are various initialization points for initing values before computing
  virtual void 	Network_Init()		{ };	// Initialize before computing network
  virtual void 	Layer_Init(Layer*) 	{ };	// Initialize before computing layer
  virtual void 	Unit_Init(Unit*)	{ };	// Initialize before computing unit

  // these are the actual statistics functions (guaranteed to have Min_ from _Run)
  virtual void 	Network_Stat()		{ };	// Compute statistic on the network
  virtual void 	Layer_Stat(Layer*) 	{ };	// Compute statistic on a layer
  virtual void 	Unit_Stat(Unit*)	{ };	// Compute statistic on a unit
  virtual void 	Con_Stat(Unit*, Connection*, Unit*) { };
  // Compute statistic on a recv unit, connection, sending unit

  // aggregation stuff..
  virtual void 	ComputeAggregates();
  // Compute aggregate of agg.from statistic in this stat

  static Aggregate::Operator GetAggOpForProc(SchedProcess* proc, Aggregate::Operator agg_op);
  // get the appropriate aggregate operator for a given process (if agg_op != DEFAULT, its just agg_op)
  virtual void 	CreateAggregates(Aggregate::Operator agg_op = Aggregate::DEFAULT);
  // #NEW_FUN #MENU #MENU_ON_Actions #ARGC_1 #UPDATE_MENUS Create aggregate stats up the processing hiearchy using given agg operator for first level agg (defaults thereafter)
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
  virtual bool	CheckLayerInNet();
  // #IGONRE make sure layer pointer points to a layer in current net, set to one of same name if not

  virtual const char* AltTypeName() { return GetTypeDef()->name; }
  // can substitute a shorter name if desired, for naming the objects

  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Stat); }

  void 	UpdateAfterEdit();

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const Stat& cp);
  COPY_FUNS(Stat, Process);
  TA_BASEFUNS(Stat);
};

class Stat_Group : public taBase_Group {
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

  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Stat); }

  void	Initialize() 		{ SetBaseType(&TA_Stat); }
  void 	Destroy()		{ };
  TA_BASEFUNS(Stat_Group);
};

// SE_Stat and MonitorStat are so basic that they are here, and not in extra

class SE_Stat : public Stat {
  // ##COMPUTE_IN_TrialProcess Squared Error Statistic
public:
  StatVal	se;			// squared errors
  float		tolerance;		// if error is less than this, its 0

  void		RecvCon_Run(Unit*)	{ }; // don't do these!
  void		SendCon_Run(Unit*)	{ };

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Init();
  void		Layer_Run();		// only compute on TARG layers
  void 		Unit_Stat(Unit* unit);

  void		NameStatVals();

  void	Initialize();
  void 	Destroy()		{ };
  SIMPLE_COPY(SE_Stat);
  COPY_FUNS(SE_Stat, Stat);
  TA_BASEFUNS(SE_Stat);
};

class MonitorStat: public Stat {
  // ##COMPUTE_IN_TrialProcess Network Monitor Statistic
public:
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
  bool		Crit();
  void		Layer_Run()	{ }; // don't run the layers
  void		Network_Init();
  void		Network_Stat();

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

  virtual void	SetObjsFmNet();
  // #BUTTON #CONFIRM set ojects from currently selected objects in the default network

  static String GetObjName(TAPtr obj); // get name of object for naming stats, etc

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


#endif // stats_h
