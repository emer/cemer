// Copyright, 1995-2007, Regents of the University of Colorado,
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

// cs: constraint satisfaction (grain, boltzmann, etc.)

#ifndef  cs_h
#define cs_h

#include <Network>
#include <Schedule>
#include <ProjectBase>
#include <Wizard>

#include "network_def.h"

// pre-declare

class CsCon;
class CsConSpec;
class CsRecvCons;
class CsUnitSpec;
class SigmoidUnitSpec;
class BoltzUnitSpec;
class IACUnitSpec;
class CsUnit;
class CsLayer;
class CsNetwork;
class CsProject; //

eTypeDef_Of(CsCon);

class E_API CsCon : public Connection {
  // #STEM_BASE ##CAT_Cs connection values for constraint satisfaction
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

eTypeDef_Of(CsConSpec);

class E_API CsConSpec : public ConSpec {
  // #STEM_BASE ##CAT_Cs constraint satisfaction connection specifications
INHERITED(ConSpec)
public:
  enum CsConVars {
    PDW = DWT+1,                // previous delta weight
    DWT_AGG,                    // for aggregating the outer products
  };

  float		lrate;		// learning rate
  float		momentum;	// momentum for change in weights
  float		momentum_c;	// #READ_ONLY complement of momentum
  float		decay;		// decay rate (before lrate and momentum)
  void 		(*decay_fun)(CsConSpec* spec, float& wt, float& dwt);
  // #LIST_CsConSpec_WtDecay #CONDEDIT_OFF_decay:0 the weight decay function to use

  inline void 	C_Init_Weights(RecvCons* cg, const int idx, Unit* ru, Unit* su,
                                       Network* net) override
  { inherited::C_Init_Weights(cg, idx, ru, su, net); cg->OwnCn(idx,PDW) = 0.0f; 
    cg->OwnCn(idx,DWT_AGG) = 0.0f; }

  inline void 	C_Init_dWt(RecvCons* cg, const int idx, Unit* ru, Unit* su,
                                   Network* net) override
  { inherited::C_Init_dWt(cg, idx, ru, su, net); cg->OwnCn(idx,DWT_AGG) = 0.0f; }

  inline void		C_Aggregate_dWt(float& dwt_agg, const float su_act,
                                        const float ru_act, const float phase)
  { dwt_agg += phase * (su_act * ru_act); }
  inline virtual void 	Aggregate_dWt(CsRecvCons* cg, CsUnit* ru, CsNetwork* net, 
                                      const float phase);
  inline virtual void 	B_Aggregate_dWt(RecvCons* bias, CsUnit* ru, const float phase);
  // aggregate coproducts

  inline void 		C_Compute_WtDecay(float& wt, float& dwt)
  { if(decay_fun != NULL)
      (*decay_fun)(this, wt, dwt); }
  // call the decay function 

  inline void		C_Compute_dWt(float& dwt_agg, float& dwt, const float n_dwt_aggs) 
  { dwt_agg /= n_dwt_aggs;  dwt += dwt_agg;  dwt_agg = 0.0f; }
  inline void		Compute_dWt(RecvCons* cg, Unit* ru, Network* net);

  inline virtual void	B_Compute_dWt(RecvCons* bias, CsUnit* ru);
 
  inline void		C_Compute_Weights(float& wt, float& dwt, float& pdw)
  { C_Compute_WtDecay(wt, dwt);
    pdw = dwt + (momentum * pdw);
    wt += lrate * pdw;	
    dwt = 0.0f; }

  inline void		Compute_Weights(RecvCons* cg, Unit* ru, Network* net);
  inline virtual void	B_Compute_Weights(RecvCons* bias, Unit* ru) {
    C_Compute_Weights(bias->OwnCn(0,WT), bias->OwnCn(0,DWT),
                      bias->OwnCn(0,PDW));
    C_ApplyLimits(bias->OwnCn(0,WT), ru, NULL);
  }

  void	InitLinks();
  SIMPLE_COPY(CsConSpec);
  TA_BASEFUNS(CsConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

// the following functions are possible weight decay functions

// #REG_FUN
void Cs_Simple_WtDecay(CsConSpec* spec, float& wt, float& dwt) 
// #LIST_CsConSpec_WtDecay Simple weight decay (subtract decay*wt)
     ;				// term here so scanner picks up comment
// #REG_FUN
void Cs_WtElim_WtDecay(CsConSpec* spec, float& wt, float& dwt) 
// #LIST_CsConSpec_WtDecay Weight Elimination (Rumelhart) weight decay
     ;				// term here so scanner picks up comment


eTypeDef_Of(CsRecvCons);

class E_API CsRecvCons : public RecvCons {
  // #STEM_BASE ##CAT_Cs group of constraint-satisfaction receiving connections
INHERITED(RecvCons)
public:
  void		Aggregate_dWt(CsUnit* ru, CsNetwork* net, float phase)
  { ((CsConSpec*)GetConSpec())->Aggregate_dWt(this, ru, net, phase); }
  // compute weight change

  TA_BASEFUNS_NOCOPY(CsRecvCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

eTypeDef_Of(CsSendCons);

class E_API CsSendCons : public SendCons {
  // #STEM_BASE ##CAT_Cs group of constraint-satisfaction sending connections
INHERITED(SendCons)
public:
  TA_BASEFUNS_NOCOPY(CsSendCons);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

eTypeDef_Of(CsUnitSpec);

class E_API CsUnitSpec : public UnitSpec {
  // #STEM_BASE ##CAT_Cs standard constraint satisfaction unit (uses inverse-logistic activation)
INHERITED(UnitSpec)
public:
  enum ClampType {
    HARD_CLAMP,			// input sets value, noise is added
    HARD_FAST_CLAMP,		// input fixed for entire settle (much faster)
    SOFT_CLAMP,			// input added to net input
    SOFT_THEN_HARD_CLAMP 	// soft clamping in minus phase, hard clamping in plus
  };

  MinMaxRange	real_range;		// the actual range to use for activations
  RandomSpec	noise;			// what kind of noise?
  float		step;			// step size taken to update activations
  float		sqrt_step;		// #HIDDEN square-root of the step size
  float		gain;			// gain of the activation fun (1/T for boltz)
  ClampType	clamp_type;		// type of clamping to use
  float		clamp_gain;		// #CONDEDIT_OFF_clamp_type:HARD_CLAMP,HARD_FAST_CLAMP gain of the soft clamping
  RandomSpec	initial_act;		// what to initialize the act to
  float		state_decay;		// #AKA_modify_decay amount to decay acts towards rest for DECAY_STATE initialization between phases or trials
  bool		use_annealing;		// true if noise sched is used to anneal acts
  Schedule	noise_sched;		// #CONDEDIT_ON_use_annealing:true schedule of noise variance multipliers
  bool		use_sharp;		// true if gain sched is used to sharpen acts
  Schedule	gain_sched;		// #CONDEDIT_ON_use_sharp:true schedule of gain multipliers

  void	Init_Acts(Unit* u, Network* net) override;
  void Init_Weights(Unit* u, Network* net) override; 	// also init aggregation stuff

  virtual void 	Compute_ClampAct(CsUnit* u, CsNetwork* net);
  // hard-fast-clamp inputs (at start of settling)
  virtual void 	Compute_ClampNet(CsUnit* u, CsNetwork* net);
  // compute net input from clamped inputs (at start of settling)

  void Compute_Netin(Unit* u, Network* net, int thread_no=-1) override;
  void Compute_Act(Unit* u, Network* net, int thread_no=-1) override;
  virtual void	Compute_Act_impl(CsUnit* u, int cycle, int phase); 
  // actually computes specific activation function 

  virtual void	DecayState(CsUnit* u, CsNetwork* net);
  // decay activation state information
  virtual void	PhaseInit(CsUnit* u, CsNetwork* net);
  // initialize external inputs based on phase information
  
  virtual void	Aggregate_dWt(CsUnit* u, CsNetwork* net, int thread_no=-1);
  void	Compute_dWt(Unit* u, Network* net, int thread_no=-1) override;
  void	Compute_Weights(Unit* u, Network* net, int thread_no=-1) override;

  virtual void	PostSettle(CsUnit* u, CsNetwork* net);
  // set stuff after settling is over

  virtual void	GraphActFun(DataTable* graph_data, float min = -5.0, float max = 5.0, int ncycles=50);
  // #BUTTON #NULL_OK graph the activation function, settling for 50 cycles for each net input (NULL = new graph data)

  void	InitLinks();
  SIMPLE_COPY(CsUnitSpec);
  TA_BASEFUNS(CsUnitSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

eTypeDef_Of(SigmoidUnitSpec);

class E_API SigmoidUnitSpec : public CsUnitSpec {
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

eTypeDef_Of(BoltzUnitSpec);

class E_API BoltzUnitSpec : public CsUnitSpec {
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

eTypeDef_Of(IACUnitSpec);

class E_API IACUnitSpec : public CsUnitSpec {
  // Interactive-Activation & Competition activation function (IAC)
INHERITED(CsUnitSpec)
public:
  float		rest;		// rest level of activation
  float		decay;		// decay rate (1/gain) (continuous decay -- not between phases or trials, which is state_decay)

  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  TA_SIMPLE_BASEFUNS(IACUnitSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

eTypeDef_Of(LinearCsUnitSpec);

class E_API LinearCsUnitSpec : public CsUnitSpec {
  // linear version of Cs units with time-averaging on the net inputs
INHERITED(CsUnitSpec)
public:
  void		Compute_Act_impl(CsUnit* u, int cycle, int phase);

  TA_BASEFUNS_NOCOPY(LinearCsUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

eTypeDef_Of(ThreshLinCsUnitSpec);

class E_API ThreshLinCsUnitSpec : public CsUnitSpec {
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

eTypeDef_Of(CsUnit);

class E_API CsUnit : public Unit {
  // #STEM_BASE ##CAT_Cs constraint satisfaction unit
INHERITED(Unit)
public:
  float		da;		// #VIEW_HOT delta-activation (or net input) value
  float		prv_net;	// #NO_VIEW previous net input
  float		clmp_net;	// #NO_VIEW net input from hard_fast_clamp
  float		act_m;		// #VIEW_HOT minus phase activation
  float		act_p;		// #VIEW_HOT plus phase activation
  int		n_dwt_aggs;	// number of delta-weight aggregations performed

  void	Compute_ClampAct(CsNetwork* net)
  { ((CsUnitSpec*)GetUnitSpec())->Compute_ClampAct(this, net); }
  void	Compute_ClampNet(CsNetwork* net)
  { ((CsUnitSpec*)GetUnitSpec())->Compute_ClampNet(this, net); }
  void	DecayState(CsNetwork* net)
  { ((CsUnitSpec*)GetUnitSpec())->DecayState(this, net); }
  void	PhaseInit(CsNetwork* net)
  { ((CsUnitSpec*)GetUnitSpec())->PhaseInit(this, net); }
  void	PostSettle(CsNetwork* net)
  { ((CsUnitSpec*)GetUnitSpec())->PostSettle(this, net); }
  void	Aggregate_dWt(CsNetwork* net, int thread_no=-1)
  { ((CsUnitSpec*)GetUnitSpec())->Aggregate_dWt(this, net, thread_no); }

  void	Copy_(const CsUnit& cp);
  TA_BASEFUNS(CsUnit);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

#ifndef __MAKETA__
typedef void (CsUnit::*CsUnitMethod)(CsNetwork*, int);
// this is required to disambiguate unit thread method guys -- double casting
#endif 


/////////////////////////////////////////////////////////////////////////

eTypeDef_Of(CsLayer);

class E_API CsLayer : public Layer {
  // #STEM_BASE ##CAT_Cs A constraint-satisfaction layer
INHERITED(Layer)
public:
  void  Init_Acts(Network* net) override;

  TA_BASEFUNS_NOCOPY(CsLayer);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

//////////////////////////////////
//	Cs Network		//
//////////////////////////////////

eTypeDef_Of(CsNetwork);

class E_API CsNetwork : public Network {
  // #STEM_BASE ##CAT_Cs network for constraint statisfaction
INHERITED(Network)
public:
  enum UpdateMode {
    SYNCHRONOUS,
    ASYNCHRONOUS,
  };

  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize the network state
    DECAY_STATE, 		// decay the activations from prior state
  };

  enum Phase {
    MINUS_PHASE = -1,
    PLUS_PHASE = 1,
  };

  UpdateMode 	update_mode;
  // how to update: async = n_updates, sync = all units
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
  float		maxda;		// #NO_SAVE #GUI_READ_ONLY #SHOW maximum #CAT_Statistic #VIEW change in activation (delta-activation) over network; used in stopping settling

  float		minus_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cycles to settle in the minus phase -- this is the typical settling time statistic to record
  float		avg_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average settling cycles in the minus phase (computed over previous epoch)
  float		avg_cycles_sum; // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average cycles in this epoch
  int		avg_cycles_n;	// #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average cycles computation for this epoch

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

  void	Init_Counters() override;
  void	Init_Stats() override;

  ///////////////////////////
  // 	cycle
  virtual void	Compute_SyncAct();
  // #CAT_Cycle compute synchronous activations: first pass is netin, second pass is activations, for all units
  virtual void	Compute_AsyncAct();
  // #CAT_Cycle compute asynchronous activations: select units at random to update
  virtual void	Compute_MaxDa();
  // #CAT_Cycle get maxda value from units
  virtual void	Aggregate_dWt();
  // #CAT_Cycle aggregate weight changes (for probabilistic sampling)
  virtual void	Cycle_Run();
  // #CAT_Cycle compuate one cyle of updating

  ///////////////////////////
  // 	settle
  virtual void	PhaseInit();
  // #CAT_Settle initialize at start of settling phase -- sets external input flags based on phase (called by Settle_Init)
  virtual void	DecayState();
  // #CAT_Settle decay state at start of settling phase (called by Settle_Init)
  virtual void	Compute_ClampAct();
  // #CAT_Settle compute activations of hard clamped units (called by Settle_Init)
  virtual void	Compute_ClampNet();
  // #CAT_Settle compute fixed netinputs from hard clamped units (optimizes computation) (called by Settle_Init)
  virtual void	PostSettle();
  // #CAT_Settle get activation states after settling
  virtual void	Settle_Init();
  // #CAT_Settle run various initialization steps at the start of settling
  virtual void	Settle_Final();
  // #CAT_Settle run final steps of processing after settling

  ///////////////////////////
  // 	trial
  virtual void	Compute_MinusCycles();
  // #CAT_Statistic compute minus-phase cycles (and increment epoch sums) -- at the end of the minus phase (of course)
  void	Compute_TrialStats() override;
  // #CAT_Statistic compute trial-level statistics, including SSE and minus cycles -- to be called at end of minus phase
  virtual void	Trial_Init();
  // #CAT_Trial initialize at start of trial: initializes activations and phase counters
  virtual void	Trial_Final();
  // #CAT_Trial at end of trial, calls Compute_dWt if not testing
  virtual void	Trial_UpdatePhase();
  // #CAT_Trial update phase after one settle -- just sets phase to plus phase
  
  ///////////////////////////
  // 	epoch
  virtual void	Compute_AvgCycles();
  // #CAT_Statistic compute average cycles (at an epoch-level timescale)
  void	Compute_EpochStats() override;
  // #CAT_Statistic compute epoch-level statistics, including SSE and AvgCycles

  void	SetProjectionDefaultTypes(Projection* prjn) override;
  void  BuildNullUnit() override;

  TA_SIMPLE_BASEFUNS(CsNetwork);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{}
};

////////////////////////////////
// 	In-line functions     //
////////////////////////////////

inline void CsConSpec::Aggregate_dWt(CsRecvCons* cg, CsUnit* ru, CsNetwork* net, 
                                     const float phase) {
  float* aggs = cg->OwnCnVar(DWT_AGG);
  const float ru_act = ru->act;
  CON_GROUP_LOOP(cg, C_Aggregate_dWt(aggs[i], cg->Un(i,net)->act,
                                     ru_act, phase));
}
inline void CsConSpec::B_Aggregate_dWt(RecvCons* bias, CsUnit* ru, float phase) {
  bias->OwnCn(0,DWT_AGG) += phase * ru->act;
}

inline void CsConSpec::Compute_dWt(RecvCons* cg, Unit* ru, Network* net) {
  float* aggs = cg->OwnCnVar(DWT_AGG);
  float* dwts = cg->OwnCnVar(DWT);
  const float n_dwt_aggs = ((CsUnit*)ru)->n_dwt_aggs; // already guaranteed to be non-0
  CON_GROUP_LOOP(cg, C_Compute_dWt(aggs[i], dwts[i], n_dwt_aggs));
}

inline void CsConSpec::B_Compute_dWt(RecvCons* bias, CsUnit* ru) {
  C_Compute_dWt(bias->OwnCn(0,DWT_AGG), bias->OwnCn(0,DWT),
                (float)ru->n_dwt_aggs);
}

inline void CsConSpec::Compute_Weights(RecvCons* cg, Unit* ru, Network* net) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  float* pdws = cg->OwnCnVar(PDW);
  CON_GROUP_LOOP(cg, C_Compute_Weights(wts[i], dwts[i], pdws[i]));
  ApplyLimits(cg, ru, net);
}


//////////////////////////////////////////
//	Additional ConSpec Types	//
//////////////////////////////////////////

eTypeDef_Of(HebbCsConSpec);

class E_API HebbCsConSpec : public CsConSpec {
  // Simple Hebbian wt update (send act * recv act), operates only on final activity states
INHERITED(CsConSpec)
public:
  void 	Aggregate_dWt(CsRecvCons*, CsUnit*, CsNetwork*, float) 	override { };
  void 	B_Aggregate_dWt(RecvCons*, CsUnit*, float)		override { }; 
  // disable both of these functions

  inline void 		C_Compute_dWt_Hebb(float& dwt, const float ru_act,
                                           const float su_act) 
  { dwt += ru_act * su_act; }
  inline void 		Compute_dWt(RecvCons* cg, Unit* ru, Network* net) {
    float* dwts = cg->OwnCnVar(DWT);
    const float ru_act = (ru->ext_flag & Unit::TARG) ? ru->targ : ru->act;
    CON_GROUP_LOOP(cg, C_Compute_dWt_Hebb(dwts[i], ru_act, cg->Un(i,net)->act));
  }

  inline void		B_Compute_dWt(RecvCons* bias, CsUnit* ru) 
  { bias->OwnCn(0,DWT) += ((ru->ext_flag & Unit::TARG) ? ru->targ : ru->act); }

  TA_BASEFUNS_NOCOPY(HebbCsConSpec);
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
};

////////////////////////////////////////////////////

eTypeDef_Of(CsProject);

class E_API CsProject : public ProjectBase {
  // #STEM_BASE ##CAT_Cs project for constraint satisfaction networks
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

eTypeDef_Of(CsWizard);

class E_API CsWizard : public Wizard {
  // #STEM_BASE ##CAT_Cs constraint satisfaction specific wizard for automating construction of simulation objects
INHERITED(Wizard)
public:

  bool	StdProgs() override;
  bool	TestProgs(Program* call_test_from, bool call_in_loop=true, int call_modulus=1) override;

  TA_BASEFUNS_NOCOPY(CsWizard);
private:
  void 	Initialize();
  void 	Destroy()	{ };
};

#endif	// cs_h

