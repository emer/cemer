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

#include "pdp_base.h"
#include "netstru.h"
#include "pdp_project.h"

#include "cs_def.h"
#include "cs_TA_type.h"

// pre-declare

class CsCon;
class CsConSpec;
class CsRecvCons;
class CsUnitSpec;
class SigmoidUnitSpec;
class BoltzUnitSpec;
class IACUnitSpec;
class CsUnit;

class CS_API CsCon : public Connection {
  // ##CAT_Cs connection values for constraint satisfaction
public:
  float		pdw;		// #NO_SAVE the previous delta-weight (for momentum)
  float		dwt_agg;	// #NO_VIEW #NO_SAVE variable for aggregating the outer-prods

  CsCon() { pdw = dwt_agg = 0.0f; }
};

// ConSpec now has 3 versions of some functions, the two regular ones
// (one for the group and one for the connection), and now a third which is a
// virtual version of the C_ for use with the bias weights.
// every time a C_ version is overloaded, it is necessary to overload the B_
// version, where the new B_ just calls the new C_

class CS_API CsConSpec : public ConSpec {
  // ##CAT_Cs constraint satisfaction connection specifications
INHERITED(ConSpec)
public:
  float		lrate;		// learning rate
  float		momentum;	// momentum for change in weights
  float		momentum_c;	// #READ_ONLY complement of momentum
  float		decay;		// decay rate (before lrate and momentum)
  void 		(*decay_fun)(CsConSpec* spec, CsCon* cn, Unit* ru, Unit* su);
  // #LIST_CsConSpec_WtDecay #CONDEDIT_OFF_decay:0 the weight decay function to use

  void 		C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_Weights(cg, cn, ru, su); ((CsCon*)cn)->pdw = 0.0f; }

  inline void		C_Aggregate_dWt(CsCon* cn, CsUnit* ru, 
				      CsUnit* su, float phase);
  inline virtual void 	Aggregate_dWt(CsRecvCons* cg, CsUnit* ru, float phase);
  inline virtual void 	B_Aggregate_dWt(CsCon* cn, CsUnit* ru, float phase);
  // aggregate coproducts

  inline void 		C_Compute_WtDecay(CsCon* cn, Unit* ru, Unit* su);
  // call the decay function 

  inline void		C_Compute_dWt(CsCon* cn, CsUnit* ru, CsUnit* su);
  inline void		Compute_dWt(RecvCons* cg, Unit* ru);
  inline virtual void	B_Compute_dWt(CsCon* cn, CsUnit* ru);
  
  inline void		C_Compute_Weights(CsCon* cn, Unit* ru, Unit* su);
  inline void		Compute_Weights(RecvCons* cg, Unit* ru);
  inline virtual void	B_Compute_Weights(CsCon* cn, Unit* ru);

  void	InitLinks();
  SIMPLE_COPY(CsConSpec);
  TA_BASEFUNS(CsConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
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


class CS_API CsRecvCons : public RecvCons {
  // ##CAT_Cs group of constraint-satisfaction receiving connections
INHERITED(RecvCons)
public:
  void		Aggregate_dWt(CsUnit* ru, float phase)
  { ((CsConSpec*)GetConSpec())->Aggregate_dWt(this, ru, phase); }
  // compute weight change

  TA_BASEFUNS_NOCOPY(CsRecvCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class CS_API CsSendCons : public SendCons {
  // ##CAT_Cs group of constraint-satisfaction sending connections
INHERITED(SendCons)
public:
  TA_BASEFUNS_NOCOPY(CsSendCons);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class CS_API CsUnitSpec : public UnitSpec {
  // ##CAT_Cs standard constraint satisfaction unit (uses inverse-logistic activation)
INHERITED(UnitSpec)
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

  void		Init_Acts(Unit* u);
  void 		Init_Weights(Unit* u); 	// also init aggregation stuff

  virtual void 	Compute_ClampAct(CsUnit* u);
  // hard-fast-clamp inputs (at start of settling)
  virtual void 	Compute_ClampNet(CsUnit* u);
  // compute net input from clamped inputs (at start of settling)

  void 		Compute_Netin(Unit* u); 		// add bias
  void		Compute_Act(Unit* u)		// if no cycle is passed
  { Compute_Act(u,-1, 0); }
  virtual void 	Compute_Act(Unit* u, int cycle, int phase);
  virtual void	Compute_Act_impl(CsUnit* u, int cycle, int phase); 
  // actually computes specific activation function 
  
  void		Aggregate_dWt(Unit* u, int phase);
  void		Compute_dWt(Unit* u);
  void		Compute_Weights(Unit* u);		// add update bias weight

  virtual void	PostSettle(CsUnit* u, int phase);
  // set stuff after settling is over

  virtual void	GraphActFun(DataTable* graph_data, float min = -5.0, float max = 5.0, int ncycles=50);
  // #BUTTON #NULL_OK graph the activation function, settling for 50 cycles for each net input (NULL = new graph data)

  void	InitLinks();
  SIMPLE_COPY(CsUnitSpec);
  TA_BASEFUNS(CsUnitSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class CS_API SigmoidUnitSpec : public CsUnitSpec {
  // Sigmoid (logistic) activation function (float-valued within range)
INHERITED(CsUnitSpec)
public:
  enum TimeAvgType {		// type of time-averaging to perform
    ACTIVATION,			// time-average the activations
    NET_INPUT 			// time-average the net inputs
  };
  TimeAvgType	time_avg;	// type of time-averaging to perform

  void		Compute_Act_impl(CsUnit* u,int cycle, int phase);

  TA_SIMPLE_BASEFUNS(SigmoidUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class CS_API BoltzUnitSpec : public CsUnitSpec {
  // Boltzmann-machine activation function (binary, probabalistic)
INHERITED(CsUnitSpec)
public:
  float		temp;		// temperature (1/gain)

  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  TA_SIMPLE_BASEFUNS(BoltzUnitSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class CS_API IACUnitSpec : public CsUnitSpec {
  // Interactive-Activation & Competition activation function (IAC)
INHERITED(CsUnitSpec)
public:
  float		rest;		// rest level of activation
  float		decay;		// decay rate (1/gain)
  bool		use_send_thresh;
  // pay attn to send_thresh? if so, need SYNC_SENDER_BASED in cycle proc, sender based netin
  float		send_thresh;	// #CONDEDIT_ON_use_send_thresh:true threshold below which unit does not send act

  void		Send_Netin(Unit* u, Layer* tolay);	// do sender-based stuff
  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  TA_SIMPLE_BASEFUNS(IACUnitSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class CS_API LinearCsUnitSpec : public CsUnitSpec {
  // linear version of Cs units with time-averaging on the net inputs
INHERITED(CsUnitSpec)
public:
  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  TA_BASEFUNS_NOCOPY(LinearCsUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class CS_API ThreshLinCsUnitSpec : public CsUnitSpec {
  // threshold-linear version of Cs units with time-averaging on the net inputs
INHERITED(CsUnitSpec)
public:
  float		threshold;

  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  TA_SIMPLE_BASEFUNS(ThreshLinCsUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class CS_API CsUnit : public Unit {
  // ##CAT_Cs constraint satisfaction unit
INHERITED(Unit)
public:
  float		da;		// delta-activation (or net input) value
  float		prv_net;	// #NO_VIEW previous net input
  float		clmp_net;	// #NO_VIEW net input from hard_fast_clamp
  float		act_m;		// minus phase activation
  float		act_p;		// plus phase activation
  int		n_dwt_aggs;	// number of delta-weight aggregations performed

  void 	Init_Netin()	{ net = bias.Cn(0)->wt + clmp_net; } // for sender based
  void		Compute_ClampAct() 
  { ((CsUnitSpec*)GetUnitSpec())->Compute_ClampAct(this); }
  void		Compute_ClampNet() 
  { ((CsUnitSpec*)GetUnitSpec())->Compute_ClampNet(this); }
  
  void		Compute_Act(int cycle, int phase)
  { ((CsUnitSpec*)GetUnitSpec())->Compute_Act(this, cycle, phase); }

  void		Compute_Act()
  { Compute_Act(-1, 0); }

  virtual void	Targ_To_Ext();
  void		PostSettle(int phase)
  { ((CsUnitSpec*)GetUnitSpec())->PostSettle(this, phase); }

  void		Aggregate_dWt(int phase)
  { ((CsUnitSpec*)GetUnitSpec())->Aggregate_dWt(this, phase); }

  void	Copy_(const CsUnit& cp);
  TA_BASEFUNS(CsUnit);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////
// 	In-line functions     //
////////////////////////////////

inline void CsConSpec::C_Aggregate_dWt(CsCon* cn, CsUnit* ru, CsUnit* su,
				       float phase) {
  cn->dwt_agg += phase * (su->act * ru->act); 
}
inline void CsConSpec::Aggregate_dWt(CsRecvCons* cg, CsUnit* ru,
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

inline void CsConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
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

inline void CsConSpec::C_Compute_Weights(CsCon* cn, Unit* ru, Unit* su) {
  C_Compute_WtDecay(cn, ru, su);
// normalized
//  cn->pdw = (momentum_c * cn->dwt) + (momentum * cn->pdw);
// before lrate
  cn->pdw = cn->dwt + (momentum * cn->pdw);
  cn->wt += lrate * cn->pdw;	
  cn->dwt = 0.0f;
}
inline void CsConSpec::Compute_Weights(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_Weights((CsCon*)cg->Cn(i), ru, cg->Un(i)));
  ApplyLimits(cg, ru);
}
inline void CsConSpec::B_Compute_Weights(CsCon* cn, Unit* ru) {
  C_Compute_Weights(cn, ru, NULL);
  C_ApplyLimits(cn, ru, NULL);
}

//////////////////////////////////////////
//	Additional ConSpec Types	//
//////////////////////////////////////////

class CS_API HebbCsConSpec : public CsConSpec {
  // Simple Hebbian wt update (send act * recv act), operates only on final activity states
INHERITED(CsConSpec)
public:
  virtual void 	Aggregate_dWt(CsRecvCons*, CsUnit*, float) 	{ };
  virtual void 	B_Aggregate_dWt(CsCon*, CsUnit*, float)		{ }; 
  // disable both of these functions

  inline void 		C_Compute_dWt(CsCon* cn, CsUnit* ru, CsUnit* su);
  inline void 		Compute_dWt(RecvCons* cg, Unit* ru);

  inline void		B_Compute_dWt(CsCon* cn, CsUnit* ru);

  TA_BASEFUNS_NOCOPY(HebbCsConSpec);
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
};

inline void HebbCsConSpec::C_Compute_dWt(CsCon* cn, CsUnit* ru, CsUnit* su) {
  cn->dwt += ((ru->ext_flag & Unit::TARG) ? ru->targ : ru->act) * su->act;
}

inline void HebbCsConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg,C_Compute_dWt((CsCon*)cg->Cn(i), (CsUnit*)ru, (CsUnit*)cg->Un(i)));
}

inline void HebbCsConSpec::B_Compute_dWt(CsCon* cn, CsUnit* ru) {
  cn->dwt += ((ru->ext_flag & Unit::TARG) ? ru->targ : ru->act);
}

/////////////////////////////////////////////////////////////////////////

class CS_API CsLayer : public Layer {
  // ##CAT_Cs A constraint-satisfaction layer
INHERITED(Layer)
public:

  TA_BASEFUNS_NOCOPY(CsLayer);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

//////////////////////////////////
//	Cs Network		//
//////////////////////////////////

class CS_API CsNetwork : public Network {
  // ##CAT_Cs network for constraint statisfaction
INHERITED(Network)
public:
  enum UpdateMode {
    SYNCHRONOUS,
    ASYNCHRONOUS,
    SYNC_SENDER_BASED 		// needed for IAC send_thresh impl
  };
  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize the network state
    MODIFY_STATE 		// modify state (algorithm specific)
  };
  enum Phase {
    MINUS_PHASE = -1,
    PLUS_PHASE = 1
  };

  UpdateMode 	update_mode;
  // how to update: async = n_updates, sync = all units. sender_based is for IAC
  int		n_updates;
  // #CONDEDIT_ON_update_mode:ASYNCHRONOUS for ASYNC mode, number of updates (with replacement) to perform in one cycle
  StateInit	trial_init;	// how to initialize network at start of trial
  StateInit	between_phases;	// what to do between phases
  bool		deterministic;  // only compute stats after the last cycle (deterministic mode)
  int		start_stats;	// #CONDEDIT_ON_deterministic:false the cycle at which to start aggregating dWt
  int		cycle_max;	// #CAT_Counter maximum number of cycles to compute

  int		sample;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW current sample number
  int		phase_max;	// #CAT_Counter maximum number of phases
  Phase		phase;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW current phase name
  int		phase_no;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW current phase number

  float		maxda_stopcrit;	// #DEF_0.01 #CAT_Statistic stopping criterion for max da
  float		maxda;		// #GUI_READ_ONLY #SHOW maximum #CAT_Statistic #VIEW change in activation (delta-activation) over network; used in stopping settling

  float		minus_cycles;	// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cycles to settle in the minus phase -- this is the typical settling time statistic to record
  float		avg_cycles;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average settling cycles in the minus phase (computed over previous epoch)
  float		avg_cycles_sum; // #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average cycles in this epoch
  int		avg_cycles_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average cycles computation for this epoch

//   // cs dist stat
//   StatVal_List	probs;		// prob of each dist pattern
//   float_RArray	act_vals;	// #HIDDEN  the act values read from network
//   float		tolerance;	// the tolerance for judging if act=targ
//   int		n_updates;	// #HIDDEN  the number of stat cycles so far

//   // cs tig stat
//   StatVal	tig;		// the Information Gain for the trial

//   // cs targ stat
//   StatVal	trg_pct;	// the pct in target for the trial

//   // goodness stat
//   bool		use_netin;
//   // use net-input for harmony instead of computing anew?
//   StatVal	hrmny;
//   StatVal	strss;
//   StatVal	gdnss;
//   float		netin_hrmny;	// #READ_ONLY temp variable to hold netin-based harmony

  override void	Init_Counters();
  override void	Init_Stats();

  // cycle
  virtual void	Compute_SyncAct();
  // #CAT_Cycle compute synchronous activations: first pass is netin, second pass is activations, for all units
  virtual void	Compute_AsyncAct();
  // #CAT_Cycle compute asynchronous activations: select units at random to update
  virtual void	Aggregate_dWt();
  // #CAT_Cycle aggregate weight changes (for probabilistic sampling)
  virtual void	Cycle_Run();
  // #CAT_Cycle compuate one cyle of updating

  // settle
  virtual void	Compute_ClampAct();
  // #CAT_Settle compute activations of hard clamped units
  virtual void	Compute_ClampNet();
  // #CAT_Settle compute fixed netinputs from hard clamped units (optimizes computation)
  virtual void	Targ_To_Ext();
  // #CAT_Settle compute activations of hard clamped units
  virtual void	PostSettle();
  // #CAT_Settle get activation states after settling
  virtual void	Settle_Init();
  // #CAT_Settle run various initialization steps at the start of settling
  virtual void	Settle_Final();
  // #CAT_Settle run final steps of processing after settling
  
  virtual void	Trial_Init(); // run one trial of Cs
  virtual void	Trial_Final(); // run one trial of Cs
  
  override void	SetProjectionDefaultTypes(Projection* prjn);

  TA_SIMPLE_BASEFUNS(CsNetwork);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{}
};

class CS_API CsProject : public ProjectBase {
  // ##CAT_Cs project for constraint satisfaction networks
INHERITED(ProjectBase)
public:

  TA_BASEFUNS_NOCOPY(CsProject);
private:
  void	Initialize();
  void 	Destroy()		{}
};

//////////////////////////////////
//	Cs Wizard		//
//////////////////////////////////

class CS_API CsWizard : public Wizard {
  // ##CAT_Cs constraint satisfaction specific wizard for automating construction of simulation objects
INHERITED(Wizard)
public:

  TA_BASEFUNS_NOCOPY(CsWizard);
private:
  void 	Initialize();
  void 	Destroy()	{ };
};

#endif	// cs_h

