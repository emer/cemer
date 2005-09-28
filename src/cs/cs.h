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



// cs: constraint satisfaction (grain, boltzmann, etc.)

#ifndef  cs_h
#define cs_h

#include <pdp/base.h>
#include <pdp/netstru.h>
#include <pdp/sched_proc.h>
#include <pdp/enviro.h>
#include <cs/cs_TA_type.h>

// pre-declare

class CsCon;
class CsConSpec;
class CsCon_Group;
class CsUnitSpec;
class SigmoidUnitSpec;
class BoltzUnitSpec;
class IACUnitSpec;
class CsUnit;

class CsSample;
class CsTrial;
class CsSettle;
class CsCycle;

class CsMaxDa;
class CsDistStat;
class CsTIGstat;
class CsTargStat;
class CsGoodStat;

class CsCon : public Connection {
  // connection values for constraint satisfaction
public:
  float 	dwt;		// #NO_SAVE the current change in weight
  float		pdw;		// #NO_SAVE the previous delta-weight (for momentum)
  float		dwt_agg;	// #NO_VIEW #NO_SAVE variable for aggregating the outer-prods

  void 	Initialize()		{ dwt = dwt_agg = pdw= 0.0f; }
  void 	Destroy()		{ };
  void	Copy_(const CsCon& cp)
  { dwt = cp.dwt; dwt_agg = cp.dwt_agg; pdw = cp.pdw; }
  COPY_FUNS(CsCon, Connection);
  TA_BASEFUNS(CsCon);
};

// ConSpec now has 3 versions of some functions, the two regular ones
// (one for the group and one for the connection), and now a third which is a
// virtual version of the C_ for use with the bias weights.
// every time a C_ version is overloaded, it is necessary to overload the B_
// version, where the new B_ just calls the new C_

class CsConSpec : public ConSpec {
  // constraint satisfaction connection specifications
public:
  float		lrate;		// learning rate
  float		momentum;	// momentum for change in weights
  float		momentum_c;	// #READ_ONLY complement of momentum
  float		decay;		// decay rate (before lrate and momentum)
  void 		(*decay_fun)(CsConSpec* spec, CsCon* cn, Unit* ru, Unit* su);
  // #LIST_CsConSpec_WtDecay #CONDEDIT_OFF_decay:0 the weight decay function to use

  void 		C_InitWtDelta(Con_Group* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_InitWtDelta(cg, cn, ru, su); CsCon* c = (CsCon*)cn; c->dwt=0.0f; }

  void 		C_InitWtState(Con_Group* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_InitWtState(cg, cn, ru, su); ((CsCon*)cn)->pdw = 0.0f; }

  inline void		C_Aggregate_dWt(CsCon* cn, CsUnit* ru, 
				      CsUnit* su, float phase);
  inline virtual void 	Aggregate_dWt(CsCon_Group* cg, CsUnit* ru, float phase);
  inline virtual void 	B_Aggregate_dWt(CsCon* cn, CsUnit* ru, float phase);
  // aggregate coproducts

  inline void 		C_Compute_WtDecay(CsCon* cn, Unit* ru, Unit* su);
  // call the decay function 

  inline void		C_Compute_dWt(CsCon* cn, CsUnit* ru, CsUnit* su);
  inline void		Compute_dWt(Con_Group* cg, Unit* ru);
  inline virtual void	B_Compute_dWt(CsCon* cn, CsUnit* ru);
  
  inline void		C_UpdateWeights(CsCon* cn, Unit* ru, Unit* su);
  inline void		UpdateWeights(Con_Group* cg, Unit* ru);
  inline virtual void	B_UpdateWeights(CsCon* cn, Unit* ru);

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(CsConSpec);
  COPY_FUNS(CsConSpec, ConSpec);
  TA_BASEFUNS(CsConSpec);
};

// the following functions are possible weight decay functions

// #REG_FUN
void Cs_Simple_WtDecay(CsConSpec* spec, CsCon* cn, Unit* ru, Unit* su) 
// #LIST_CsConSpec_WtDecay Simple weight decay (subtract decay*wt)
     ;				// term here so scanner picks up comment
// #REG_FUN
void Cs_WtElim_WtDecay(CsConSpec* spec, CsCon* cn, Unit* ru, Unit* su) 
// #LIST_CsConSpec_WtDecay Weight Elimination (Rumelhart) weight decay
     ;				// term here so scanner picks up comment


class CsCon_Group : public Con_Group {
public:
  void		Aggregate_dWt(CsUnit* ru, float phase)
  { ((CsConSpec*)spec.spec)->Aggregate_dWt(this, ru, phase); }
  // compute weight change

  void 	Initialize()		{ };
  void	Destroy()		{ };
  TA_BASEFUNS(CsCon_Group);
};


class CsUnitSpec : public UnitSpec {
  // standard constraint satisfaction unit (uses inverse-logistic activation)
public:
  enum ClampType {
    HARD_CLAMP,			// input sets value, noise is added
    HARD_FAST_CLAMP,		// input fixed for entire settle (much faster)
    SOFT_CLAMP,			// input added to net input
    SOFT_THEN_HARD_CLAMP 	// soft clamping in minus phase, hard clamping in plus
  };

  MinMaxRange	real_range;		// the actual range to use for activations
  Random	noise;			// what kind of noise?
  float		step;			// step size taken to update activations
  float		sqrt_step;		// #HIDDEN square-root of the step size
  float		gain;			// gain of the activation fun (1/T for boltz)
  ClampType	clamp_type;		// type of clamping to use
  float		clamp_gain;		// #CONDEDIT_OFF_clamp_type:HARD_CLAMP,HARD_FAST_CLAMP gain of the soft clamping
  Random	initial_act;		// what to initialize the act to
  float		modify_decay;		// amount to decay acts towards rest for MODIFY_STATE
  bool		use_annealing;		// true if noise sched is used to anneal acts
  Schedule	noise_sched;		// #CONDEDIT_ON_use_annealing:true schedule of noise variance multipliers
  bool		use_sharp;		// true if gain sched is used to sharpen acts
  Schedule	gain_sched;		// #CONDEDIT_ON_use_sharp:true schedule of gain multipliers

  void		InitState(Unit* u);
  void		ModifyState(Unit* u);   // decay unit state towards midpoint (or rest)
  void 		InitWtState(Unit* u); 	// also init aggregation stuff

  virtual void 	Compute_ClampAct(CsUnit* u);
  // hard-fast-clamp inputs (at start of settling)
  virtual void 	Compute_ClampNet(CsUnit* u);
  // compute net input from clamped inputs (at start of settling)

  void 		Compute_Net(Unit* u); 		// add bias
  void		Compute_Act(Unit* u)		// if no cycle is passed
  { Compute_Act(u,-1, 0); }
  virtual void 	Compute_Act(Unit* u, int cycle, int phase);
  virtual void	Compute_Act_impl(CsUnit* u, int cycle, int phase); 
  // actually computes specific activation function 
  
  void		Aggregate_dWt(Unit* u, int phase);
  void		Compute_dWt(Unit* u);
  void		UpdateWeights(Unit* u);		// add update bias weight

  virtual void	PostSettle(CsUnit* u, int phase);
  // set stuff after settling is over

  virtual void	GraphActFun(GraphLog* graph_log, float min = -5.0, float max = 5.0, int ncycles=50);
  // #BUTTON #NULL_OK graph the activation function, "settling" for 50 cycles for each net input (NULL = new graph log)

  void	UpdateAfterEdit();	// update the sqrt_step
  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(CsUnitSpec);
  COPY_FUNS(CsUnitSpec, UnitSpec);
  TA_BASEFUNS(CsUnitSpec);
};

class SigmoidUnitSpec : public CsUnitSpec {
  // Sigmoid (logistic) activation function (float-valued within range)
public:
  enum TimeAvgType {		// type of time-averaging to perform
    ACTIVATION,			// time-average the activations
    NET_INPUT 			// time-average the net inputs
  };
  TimeAvgType	time_avg;	// type of time-averaging to perform

  void		Compute_Act_impl(CsUnit* u,int cycle, int phase);

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(SigmoidUnitSpec);
  COPY_FUNS(SigmoidUnitSpec, CsUnitSpec);
  TA_BASEFUNS(SigmoidUnitSpec);
};

class BoltzUnitSpec : public CsUnitSpec {
  // Boltzmann-machine activation function (binary, probabalistic)
public:
  float		temp;		// temperature (1/gain)

  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  void	UpdateAfterEdit();	// update gain from temp
  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(BoltzUnitSpec);
  COPY_FUNS(BoltzUnitSpec, CsUnitSpec);
  TA_BASEFUNS(BoltzUnitSpec);
};

class IACUnitSpec : public CsUnitSpec {
  // Interactive-Activation & Competition activation function (IAC)
public:
  float		rest;		// rest level of activation
  float		decay;		// decay rate (1/gain)
  bool		use_send_thresh;
  // pay attn to send_thresh? if so, need SYNC_SENDER_BASED in cycle proc, sender based netin
  float		send_thresh;	// #CONDEDIT_ON_use_send_thresh:true threshold below which unit does not send act

  void		Send_Net(Unit* u, Layer* tolay);	// do sender-based stuff
  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  void	UpdateAfterEdit();	// update gain from decay
  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(IACUnitSpec);
  COPY_FUNS(IACUnitSpec, CsUnitSpec);
  TA_BASEFUNS(IACUnitSpec);
};

class LinearCsUnitSpec : public CsUnitSpec {
  // linear version of Cs units with time-averaging on the net inputs
public:
  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(LinearCsUnitSpec);
};

class ThreshLinCsUnitSpec : public CsUnitSpec {
  // threshold-linear version of Cs units with time-averaging on the net inputs
public:
  float		threshold;

  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(ThreshLinCsUnitSpec);
  COPY_FUNS(ThreshLinCsUnitSpec, CsUnitSpec);
  TA_BASEFUNS(ThreshLinCsUnitSpec);
};

class CsUnit : public Unit {
  // constraint satisfaction unit
public:
  float		da;		// delta-activation (or net input) value
  float		prv_net;	// #NO_VIEW previous net input
  float		clmp_net;	// #NO_VIEW net input from hard_fast_clamp
  float		act_m;		// minus phase activation
  float		act_p;		// plus phase activation
  int		n_dwt_aggs;	// number of delta-weight aggregations performed

  void 	InitDelta()	{ net = bias->wt + clmp_net; } // for sender based
  void		Compute_ClampAct() 
  { ((CsUnitSpec*)spec.spec)->Compute_ClampAct(this); }
  void		Compute_ClampNet() 
  { ((CsUnitSpec*)spec.spec)->Compute_ClampNet(this); }
  
  void		Compute_Act(int cycle, int phase)
  { ((CsUnitSpec*)spec.spec)->Compute_Act(this, cycle, phase); }

  void		Compute_Act()
  { Compute_Act(-1, 0); }

  virtual void	Targ_To_Ext();
  void		PostSettle(int phase)
  { ((CsUnitSpec*)spec.spec)->PostSettle(this, phase); }

  void		Aggregate_dWt(int phase)
  { ((CsUnitSpec*)spec.spec)->Aggregate_dWt(this, phase); }

  void 	Initialize();
  void 	InitLinks();
  void	Destroy()		{ };
  SIMPLE_COPY(CsUnit);
  COPY_FUNS(CsUnit, Unit);
  TA_BASEFUNS(CsUnit);
};

////////////////////////////////
// 	In-line functions     //
////////////////////////////////

inline void CsConSpec::C_Aggregate_dWt(CsCon* cn, CsUnit* ru, CsUnit* su,
					float phase) {
    cn->dwt_agg += phase * (su->act * ru->act); 
}
inline void CsConSpec::Aggregate_dWt(CsCon_Group* cg, CsUnit* ru,
				      float phase) {
  CON_GROUP_LOOP(cg, C_Aggregate_dWt((CsCon*)cg->Cn(i), ru, (CsUnit*)cg->Un(i),
				   phase));
}
inline void CsConSpec::B_Aggregate_dWt(CsCon* cn, CsUnit* ru, float phase) {
  cn->dwt_agg += phase * ru->act;
}

inline void CsConSpec::C_Compute_dWt(CsCon* cn, CsUnit* ru, CsUnit*) {
  cn->dwt_agg /= (float)ru->n_dwt_aggs;
  cn->dwt += cn->dwt_agg;
  cn->dwt_agg = 0.0f;
}

inline void CsConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt((CsCon*)cg->Cn(i), (CsUnit*)ru,
				   (CsUnit*)cg->Un(i)));
}
inline void CsConSpec::B_Compute_dWt(CsCon* cn, CsUnit* ru) {
  C_Compute_dWt(cn, ru, NULL);
}

inline void CsConSpec::C_Compute_WtDecay(CsCon* cn, Unit* ru, Unit* su) {
  if(decay_fun != NULL)
    (*decay_fun)(this, cn, ru, su);
}

inline void CsConSpec::C_UpdateWeights(CsCon* cn, Unit* ru, Unit* su) {
  C_Compute_WtDecay(cn, ru, su);
// normalized
//  cn->pdw = (momentum_c * cn->dwt) + (momentum * cn->pdw);
// before lrate
  cn->pdw = cn->dwt + (momentum * cn->pdw);
  cn->wt += lrate * cn->pdw;	
  cn->dwt = 0.0f;
}
inline void CsConSpec::UpdateWeights(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_UpdateWeights((CsCon*)cg->Cn(i), ru, cg->Un(i)));
  ApplyLimits(cg, ru);
}
inline void CsConSpec::B_UpdateWeights(CsCon* cn, Unit* ru) {
  C_UpdateWeights(cn, ru, NULL);
  C_ApplyLimits(cn, ru, NULL);
}

////////////////////////////////
// 	Processes             //
////////////////////////////////

class CsCycle : public CycleProcess {
  // one update cycle of all units (or n_updates in async) in network
public:
  enum UpdateMode {
    SYNCHRONOUS,
    ASYNCHRONOUS,
    SYNC_SENDER_BASED 		// needed for IAC send_thresh impl
  };

  CsSettle*	cs_settle;	// #NO_SUBTYPE #READ_ONLY #NO_SAVE
  CsTrial*	cs_trial;	// #NO_SUBTYPE #READ_ONLY #NO_SAVE
  UpdateMode 	update_mode;
  // how to update: async = n_updates, sync = all units. sender_based is for IAC
  int		n_updates;
  // #CONDEDIT_ON_update_mode:ASYNCHRONOUS for ASYNC mode, number of updates (with replacement) to perform in one cycle

  void		Loop();		// compute activations
  bool		Crit()		{ return true; } // executes only once

  virtual void	Compute_SyncAct();
  virtual void	Compute_AsyncAct();
  virtual void	Aggregate_dWt();

  void	UpdateAfterEdit();
  void	CutLinks();
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	Copy_(const CsCycle& cp);
  COPY_FUNS(CsCycle, CycleProcess);
  TA_BASEFUNS(CsCycle);
};


class CsSettle : public SettleProcess {
  // one settle to equilibrium of constrant satsisfaction
public:
  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize the network state
    MODIFY_STATE 		// modify state (algorithm specific)
  };

  CsTrial*	cs_trial;	// #NO_SUBTYPE #READ_ONLY #NO_SAVE
  StateInit	between_phases;	// what to do between phases
  uint 		n_units;	// #HIDDEN for asynchronous update in cycle process
  bool		deterministic;  // only compute stats after the last cycle (deterministic mode)
  int		start_stats;	// #CONDEDIT_ON_deterministic:false the cycle at which to start aggregating dWt
   
  void	Init_impl();	// initialize start of settling and n_units
  void	Final();	// update acts at end of settling
  void	LoopStats();	// only compute stats after start_stats

  virtual void	Compute_ClampAct();
  virtual void	Compute_ClampNet();
  virtual void	Compute_NUnits();
  virtual void	Targ_To_Ext();
  virtual void	PostSettle();
  virtual void	Aggregate_dWt();

  void	UpdateAfterEdit();
  void	CutLinks();
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  SIMPLE_COPY(CsSettle);
  COPY_FUNS(CsSettle, SettleProcess);
  TA_BASEFUNS(CsSettle);
};


class CsTrial : public TrialProcess {
  // one minus phase and one plus phase of settling
public:
  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize the network state
    MODIFY_STATE 		// modify state (algorithm specific)
  };
    
  enum Phase {
    MINUS_PHASE = -1,
    PLUS_PHASE = 1
  };

  Counter	phase_no;	// current phase number
  Phase		phase;		// state variable for phase
  StateInit	trial_init;	// how to initialize network at start of trial
  bool		no_plus_stats;	// don't do stats/logging in plus phase
  bool		no_plus_test;	// don't do plus phase when testing

  void		C_Code();	// modified to use no_plus_stats flag

  void		Init_impl();
  void 		UpdateState();
  bool		Crit();

  void		Final();	
  // compute the weight changes at the end of the trial (if not under a CsSample)
  virtual void	Compute_dWt();
  // computes weight changes for all units in network

  void		GetCntrDataItems();
  void 		GenCntrLog(LogData* ld, bool gen);

  bool		CheckUnit(Unit* ck);
  // make sure CsCycle::update_mode is correct for IAC send_thresh units..
  bool		CheckNetwork();

  void	UpdateAfterEdit();
  void	Initialize();
  void 	InitLinks();
  void	CutLinks();
  void	Destroy()		{ CutLinks(); }
  void 	Copy_(const CsTrial& cp);
  COPY_FUNS(CsTrial, TrialProcess);
  TA_BASEFUNS(CsTrial);
};

class CsSample : public TrialProcess {
  // Samples over Cs Trials (
public:
  Counter	sample;

  bool		Crit()		{ return SchedProcess::Crit(); }

  void		Final();	
  // compute the weight changes at the end of the trial
  virtual void	Compute_dWt();
  // computes weight changes for all units in network

  bool		CheckUnit(Unit* ck);
  // make sure CsCycle::update_mode is correct for IAC send_thresh units..

  void  Init_impl();
  void 	InitLinks();
  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(CsSample);
  COPY_FUNS(CsSample, TrialProcess);
  TA_BASEFUNS(CsSample);
};

class CsMaxDa : public Stat {
 // ##COMPUTE_IN_SettleProcess ##LOOP_STAT stat that computes when equilibrium is
public:
  SettleProcess* settle;	// #READ_ONLY #NO_SAVE settle process to record
  StatVal	da;		// delta-activation

  void		RecvCon_Run(Unit*)	{ }; // don't do these!
  void		SendCon_Run(Unit*)	{ };

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Init();
  void		Network_Stat();	// don't stop before 5 cycles 
  void 		Unit_Stat(Unit* unit);

  void 	Initialize();		// set minimums
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const CsMaxDa& cp);
  COPY_FUNS(CsMaxDa, Stat);
  TA_BASEFUNS(CsMaxDa);
};


class CsDistStat : public Stat {
  /* ##COMPUTE_IN_SettleProcess ##LOOP_STAT gets actual distributions for TIG Stat
     aggregation makes avg of this in phases, TIG stat in trial */
public:
  CsSettle*	cs_settle;	// #READ_ONLY #NO_SAVE
  StatVal_List	probs;		// prob of each dist pattern
  float_RArray	act_vals;	// #HIDDEN  the act values read from network
  float		tolerance;	// the tolerance for judging if act=targ
  int		n_updates;	// #HIDDEN  the number of stat cycles so far

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Layer_Run()		{ };
  void		Network_Stat();
  void		CreateAggregates(Aggregate::Operator default_op = Aggregate::DEFAULT);
  
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const CsDistStat& cp);
  COPY_FUNS(CsDistStat, Stat);
  TA_BASEFUNS(CsDistStat);
};

class CsTIGstat : public Stat {
  /* ##COMPUTE_IN_CsSample ##FINAL_STAT Total Information Gain statistic,
     needs a dist stat to compute raw stats for this one */
public:
  TrialProcess* trial_proc;	// #READ_ONLY #NO_SAVE need to get cur_event
  StatVal	tig;		// the Information Gain for the trial
  CsDistStat*	dist_stat; 	// get the actual distributions from this stat

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Layer_Run()		{ };
  void		Network_Init();
  void		Network_Stat();

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const CsTIGstat& cp);
  COPY_FUNS(CsTIGstat, Stat);
  TA_BASEFUNS(CsTIGstat);
};

class CsTargStat : public Stat {
  /* ##COMPUTE_IN_CsSample ##FINAL_STAT computes the pct in target distribution,
     is just like a TIG stat in that it gets raw values from dist stat */
public:
  StatVal	trg_pct;	// the pct in target for the trial
  CsDistStat*	dist_stat;	// get the actual distributions from this stat

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Layer_Run()		{ };
  void		Network_Init();
  void		Network_Stat();

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const CsTargStat& cp);
  COPY_FUNS(CsTargStat, Stat);
  TA_BASEFUNS(CsTargStat);
};

class CsGoodStat : public Stat {
  // ##COMPUTE_IN_TrialProcess constraint satisfaction goodness statistic
public:
  bool		use_netin;
  // use net-input for harmony instead of computing anew?

  StatVal	hrmny;
  StatVal	strss;
  StatVal	gdnss;
  float		netin_hrmny;	// #READ_ONLY temp variable to hold netin-based harmony

  void		InitStat();
  void		Init();
  bool		Crit();

  void		Network_Init();
  void		Unit_Init(Unit* un);
  void		Unit_Stat(Unit* un);
  void 		RecvCon_Run(Unit* unit);  // check for use_netin case...
  void		Con_Stat(Unit* ru, Connection* cn, Unit* su);
  void		Network_Stat();

  void	Initialize();
  void	Destroy();
  void	Copy_(const CsGoodStat& cp);
  COPY_FUNS(CsGoodStat, Stat);
  TA_BASEFUNS(CsGoodStat);
};

//////////////////////////////////////////
//	Additional ConSpec Types	//
//////////////////////////////////////////

class HebbCsConSpec : public CsConSpec {
  // Simple Hebbian wt update (send act * recv act), operates only on final activity states
public:
  virtual void 	Aggregate_dWt(CsCon_Group*, CsUnit*, float) 	{ };
  virtual void 	B_Aggregate_dWt(CsCon*, CsUnit*, float)		{ }; 
  // disable both of these functions

  inline void 		C_Compute_dWt(CsCon* cn, CsUnit* ru, CsUnit* su);
  inline void 		Compute_dWt(Con_Group* cg, Unit* ru);

  inline void		B_Compute_dWt(CsCon* cn, CsUnit* ru);

  void	Initialize()		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(HebbCsConSpec);
};

inline void HebbCsConSpec::C_Compute_dWt(CsCon* cn, CsUnit* ru, CsUnit* su) {
  cn->dwt += ((ru->ext_flag & Unit::TARG) ? ru->targ : ru->act) * su->act;
}

inline void HebbCsConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg,C_Compute_dWt((CsCon*)cg->Cn(i), (CsUnit*)ru, (CsUnit*)cg->Un(i)));
}

inline void HebbCsConSpec::B_Compute_dWt(CsCon* cn, CsUnit* ru) {
  cn->dwt += ((ru->ext_flag & Unit::TARG) ? ru->targ : ru->act);
}

#endif	// cs_h

