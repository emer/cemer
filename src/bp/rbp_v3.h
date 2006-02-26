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



// rbp.h

#ifndef rbp_v3_h
#define rbp_v3_h

#include "bp.h"
#include "pdpshell.h"	// needed for Wizard

class RBpConSpec;
class RBpUnit;
class RBpUnitSpec;

class RBpTrial;
class RBpSequence;

class APBpCycle;
class APBpSettle;
class APBpTrial;
class APBpMaxDa_De;

// _

// the unit maintains activation and other state information automatically
// and the size of this list is used to determine when to backpropagate
// depending on the parameters in the unit spec (time window, bp window).
// also, the process can excplicitly backprop at any given point

// instead of keeping big lists of all sorts of things, only the essential forward
// pass values are maintained, and the rest is propagated sequentially during the
// backpropagation phase
// thus, prv_xxx values are kept of all variables, and arrays of only the activations
// are kept.  at each act step, the time is shifted forward (cur becomes prv)
// and at each bp step, time is shifted backwards (prv becomes cur)

// forward passes: do Compute_Net() on whole net, then Compute_Act()
// store resulting act values, etc.

// backward passes: do Compute_Error, then Compute_Send_dEdNet, the Step_Back
// and iterate..

class RBpConSpec : public BpConSpec {
  // Recurrent Backprop Con Spec
public:
  inline void 		C_Compute_dWt(BpCon* cn, RBpUnit* ru, RBpUnit* su);
  inline void 		Compute_dWt(Con_Group* cg, Unit* ru);
  // Compute dE with respect to the weights (using prv_act) as sender

  void 	Initialize()		{ };
  void	Destroy()		{ };
  TA_BASEFUNS(RBpConSpec);
};

class SymRBpConSpec : public RBpConSpec {
  // Recurrent Backprop Con Spec: option to maintain weight symmetry through simple averaging of two weight changes
public:
  bool	sym_wt_updt;		// if true, use symmetric weight updates

  inline void 		C_Compute_dWt_Sym(BpCon* cn, RBpUnit* ru, RBpUnit* su);
  inline void 		Compute_dWt(Con_Group* cg, Unit* ru);
  // Compute dE with respect to the weights (using prv_act) as sender

  void 	Initialize()		{ sym_wt_updt = true; }
  void	Destroy()		{ };
  TA_BASEFUNS(SymRBpConSpec);
};


class RBpUnitSpec : public BpUnitSpec {
  // Recurrent Backprop Unit Specification
public:
  enum TimeAvgType {	// type of time-averaging to perform
    ACTIVATION,		// time-average the activations
    NET_INPUT 		// time-average the net inputs
  };

  float		dt;		// grain of time computing on (must be in [0..1] range)
  TimeAvgType	time_avg;	// type of time-averaging to perform
  bool		soft_clamp;
  bool		fast_hard_clamp_net; // #CONDEDIT_OFF_soft_clamp:true compute netin from hard clamped layers only once: ONLY IF ALL LAYERS HARD CLAMPED
  float		soft_clamp_gain; // #CONDEDIT_ON_soft_clamp:true gain on the soft clamping
  bool		teacher_force;	 // use teacher forcing
  bool		store_states;    // store activity states (usually true, except in AP mode)
  Random	initial_act;	 // initial activation value
  bool		updt_clamped_wts; // update weights for clamped units: need this for symmetric cons!

  virtual void	ResetStored(RBpUnit* u);
  // reset the stored state values, which clears bp sequence and resets time, but leaves current state intact
  virtual void  Compute_ClampExt(RBpUnit* u);
  // compute activations resulting from clamped external input (for initial state)

  void		InitState(Unit* u);
  virtual void	Compute_HardClampNet(RBpUnit* u); // for fast-hard-clamp-net: call this first
  void		Compute_Net(Unit* u);
  virtual void	Compute_Act_impl(RBpUnit* u);
  void		Compute_Act(Unit* u);
  void		Compute_dEdA(BpUnit* u);
  void 		Compute_dEdNet(BpUnit* u);
  void 		Compute_dWt(Unit* u);
  void 		UpdateWeights(Unit* u);

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(RBpUnitSpec);
  COPY_FUNS(RBpUnitSpec, UnitSpec);
  TA_BASEFUNS(RBpUnitSpec);
};

class float_CircBuffer : public float_Array {
 // Circular buffer for holding state information
public:
  int		st_idx;		// starting index
  int		length;		// logical length of the list

  int		CircIdx(uint idx) const
  { int rval = idx+st_idx; if(rval >= size) rval -= size; return rval; }
  // gets real index based on circular buffer given logical index

  void		ShiftLeft(int nshift)
  { st_idx = CircIdx(nshift); length -= nshift; }

  void		Add(const float& item);
  // adds in the framework of a circular buffer

  const float&	Peek() const {return SafeEl(CircIdx(length-1));}

  void		Reset();

  void 	Initialize();
  void	Destroy()		{ };
  void 	Copy_(const float_CircBuffer& cp);
  COPY_FUNS(float_CircBuffer, float_Array);
  TA_BASEFUNS(float_CircBuffer);
};


class RBpUnit : public BpUnit {
  // recurrent BP unit
public:
  float		da;		// delta-activation (change in activation value)
  float		ddE;		// delta-delta-Error (change in error derivative)

  ExtType	prv_ext_flag;	// #HIDDEN previous external input flag
  float		prv_targ;	// #NO_VIEW #HIDDEN previous target value
  float		prv_ext;	// #NO_VIEW #HIDDEN previous external input value
  float		prv_act;	// #NO_VIEW #HIDDEN previous activation value
  float		act_raw;	// #NO_VIEW current raw activation value
  float		prv_net;	// #NO_VIEW #HIDDEN previous net-input value
  float		prv_dEdA;	// #NO_VIEW #HIDDEN previous dEdA value
  float		prv_dEdNet;	// #NO_VIEW #HIDDEN previous dEdNet value
  float		clmp_net;	// #NO_VIEW #HIDDEN net input from hard-clamped input layers

  float_CircBuffer ext_flags;	// #NO_VIEW array of external_flag values
  float_CircBuffer targs;	// #NO_VIEW array of target values
  float_CircBuffer exts;	// #NO_VIEW array of external input values
  float_CircBuffer acts;	// #NO_VIEW array of activation values

  void 		InitExterns();	// keep prv_values..
  virtual void	StoreState();
  // store state information in buffers
  void		InitForBP()	{ prv_dEdA = prv_dEdNet = 0.0f; }
  // initialization prior to backpropagation (prv_dEdA, prv_dEdNet)
  virtual bool	StepBack(int tick);
  // move units backwards in time at given tick position in array
  virtual bool	RestoreState(int tick);
  // restore state of units to current values at given tick
  virtual void	ShiftBuffers(int ticks);
  // shift unit data buffers by given number of ticks

  // spec functions
  void    ResetStored() { ((RBpUnitSpec*)spec.spec)->ResetStored(this); }
  void    Compute_ClampExt() { ((RBpUnitSpec*)spec.spec)->Compute_ClampExt(this); }
  void    Compute_HardClampNet() { ((RBpUnitSpec*)spec.spec)->Compute_HardClampNet(this); }

  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const RBpUnit& cp);
  COPY_FUNS(RBpUnit, BpUnit);
  TA_BASEFUNS(RBpUnit);
};

// use previous activation value
inline void RBpConSpec::C_Compute_dWt(BpCon* cn, RBpUnit* ru, RBpUnit* su) {
  cn->dEdW += su->prv_act * ru->dEdNet;
}
inline void RBpConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg,C_Compute_dWt((BpCon*)cg->Cn(i), (RBpUnit*)ru, (RBpUnit*)cg->Un(i)));
}

// use previous activation value
inline void SymRBpConSpec::C_Compute_dWt_Sym(BpCon* cn, RBpUnit* ru, RBpUnit* su) {
  // just take the average of the two different weight changes
  cn->dEdW += 0.5f * (su->prv_act * ru->dEdNet + ru->prv_act * su->dEdNet);
}
inline void SymRBpConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  if(sym_wt_updt) {
    CON_GROUP_LOOP(cg,C_Compute_dWt_Sym((BpCon*)cg->Cn(i), (RBpUnit*)ru, (RBpUnit*)cg->Un(i)));
  }
  else {
    CON_GROUP_LOOP(cg,C_Compute_dWt((BpCon*)cg->Cn(i), (RBpUnit*)ru, (RBpUnit*)cg->Un(i)));
  }
}

// perform a bp whenever the units have time_window amount of information in
// their buffers, then, shift left those buffers by bp_gap amount
// then, they'll be ready to go again when buffers have time_window in them again..
// this makes it so that the units keep track of themselves, and the process
// will interface nicely without requiring firm alignment between sequence periods
// and various time windows..
// indeed, one can run just the RBpTrial with a flat environment and it will work
// just fine..

// time gets set/reset by checking the buffers on the units: when they are 0
// at start of Compute_Act then time gets reset to 0.  otherwise, time increases
// by dt each step of processing (thus, it is not tied to a specific sequence length)

class RBpTrial : public BpTrial {
  // one presentation of an event to RBp
public:
  float		time;
  // #READ_ONLY #SHOW current time (relative to start of sequence)
  float		dt;
  // #READ_ONLY #SHOW this is made to correspond to the dt used by units
  float		time_window;	// time window to pay attention to derivatives for
  float		bp_gap;		// time period to go before performing a bp
  bool		real_time;
  // use 'real time' model (else time_window = length of sequence)
  bool		bp_performed;	// #READ_ONLY true if bp was just performed last step

  int		time_win_ticks;	// #READ_ONLY time window in ticks
  int		bp_gap_ticks;	// #READ_ONLY bp window in ticks

  void		Init_impl();
  void		Loop();
  bool		Crit()		{ return true; }

  virtual void  Compute_ClampExt();
  // compute clamped external activations (for zero time step)

  void		Compute_Act();
  void		Compute_dEdA_dEdNet();

  virtual void	StoreState();
  // store current state of network in buffers
  virtual void 	InitForBP();
  // initialize prior to performing BP
  virtual void	StepBack(int tick);
  // step back during BP process at given tick
  virtual void	RestoreState(int tick);
  // restore state to values at given point in time
  virtual void	ShiftBuffers();
  // shift buffers back by bp_gap_ticks
  virtual bool	PerformBP();
  // checks if its time to perform bp, does so, returns if it did or not
  virtual void	PerformBP_impl();
  // actually do the bp
  virtual int	GetUnitBufSize(bool in_updt_after = false);
  // finds first unit in first layer and gets current buffer size of that unit

  virtual void	ResetStored();
  // reset the stored state values (clears bp sequence, resets time, but doesn't fully clear unit states -- callable by script if needed)
  virtual void	CopyContext();
  // copy the SRN context layer info (must be called by a script at appropriate times)

  void		GetCntrDataItems();
  void		GenCntrLog(LogData* ld, bool gen);
  // output time information

  bool		CheckNetwork();
  bool		CheckUnit(Unit* ck); // check for store_states too

  void	UpdateAfterEdit();
  void 	Initialize();
  void	InitLinks();
  void	Destroy()		{ };
  SIMPLE_COPY(RBpTrial);
  COPY_FUNS(RBpTrial, BpTrial);
  TA_BASEFUNS(RBpTrial);
};

class RBpSequence : public SequenceProcess {
  // one sequence of events, handles TimeEvents properly
public:
  virtual int	GetMaxTick(float& last_time);	// get maximum tick val based on current group

  void		Init_impl();
  void		GetEventList();
  void		GetCurEvent();
  void		Loop();

  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(RBpSequence);
};


class RBpSE_Stat : public SE_Stat {
  // Squared error for recurrent backprop, mulitplies by dt
public:
  void		Network_Stat();

  void	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(RBpSE_Stat);
};

class RBpContextSpec : public RBpUnitSpec {
  // RBp version of context units in simple recurrent nets (SRN), expects one-to-one prjn from layer it copies, Trial->CopyContext() must be called by script to update!
public:
  float		hysteresis;	 // hysteresis factor: (1-hyst)*new + hyst*old
  float		hysteresis_c;	 // #READ_ONLY complement of hysteresis
  String	variable;	 // name of unit variable to copy into
  Unit::ExtType	unit_flags;	 // flags to set on the unit after copying value
  MemberDef*	var_md;		 // #IGNORE memberdef of variable

  virtual void	CopyContext(RBpUnit* u); // copy the values in to the context units: called by trial CopyContext under control of a script..
  void 		Compute_Act(Unit* u);

  // nullify all other functions..
  void 		Compute_Net(Unit*) 	{ };
  void 		InitWtDelta(Unit*) 	{ };
  void 		Compute_dWt(Unit*) 	{ };
  void 		UpdateWeights(Unit*) 	{ };

  // bp special functions
  void	Compute_HardClampNet(RBpUnit*) { };
  void 	Compute_Error(BpUnit*)		{ };
  void 	Compute_dEdA(BpUnit*)		{ };
  void 	Compute_dEdNet(BpUnit*)		{ };

  bool  CheckConfig(Unit* un, Layer* lay, TrialProcess* tp);

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void	Copy_(const RBpContextSpec& cp);
  COPY_FUNS(RBpContextSpec, RBpUnitSpec);
  TA_BASEFUNS(RBpContextSpec);
};

//////////////////////////////////////////
//	Almeida-Pineda Algorithm	//
//////////////////////////////////////////

class APBpCycle : public CycleProcess {
  // one cycle of processing in almeida-pineda (either act or bp depending on 'phase')
public:
  APBpSettle*	apbp_settle;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent settle proc
  APBpTrial* 	apbp_trial;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent phase trial

  void		Loop();
  bool		Crit()		{ return true; }

  virtual void	Compute_Act();
  virtual void	Compute_Error();
  virtual void	Compute_dEdA_dEdNet();

  void 	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	CutLinks();
  TA_BASEFUNS(APBpCycle);
};

class APBpSettle : public SettleProcess {
  // one settling phase in Almeide-Pineda (either act or bp depending on phase)
public:
  APBpTrial* 	apbp_trial;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent phase trial

  void		Init_impl();	// initialize start of settling

  virtual void  Compute_ClampExt();
  // compute clamped external activations (for zero time step)
  virtual void	Compute_HardClampNet();

  void 	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  SIMPLE_COPY(APBpSettle);
  COPY_FUNS(APBpSettle, SettleProcess);
  TA_BASEFUNS(APBpSettle);
};


class APBpTrial : public TrialProcess {
  // one Almeida-Pineda BP Trial
public:
  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE 			// initialize state
  };

  enum Phase {
    ACT_PHASE,			// activation phase
    BP_PHASE 			// backpropagation phase
  };

  Counter	phase_no;	// Current phase number
  Phase		phase;		// state variable for phase
  StateInit	trial_init;	// how to initialize network state at start of trial
  bool		no_bp_stats;	// don't do stats/logging in the bp phase
  bool		no_bp_test; 	// don't run the bp phase when testing


  virtual void	SetCurLrate();
  virtual void	Compute_dWt();	// compute weight changes

  void		C_Code();	// modify to use the no_plus_stats flag

  void		Init_impl();
  void		UpdateState();
  bool		Crit()		{ return SchedProcess::Crit(); }
  void		Final();	// compute weight changes at end..

  void		GetCntrDataItems();
  void		GenCntrLog(LogData* ld, bool gen);

  bool		CheckNetwork();
  bool		CheckUnit(Unit* ck); // check for store_states too

  void	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const APBpTrial& cp);
  COPY_FUNS(APBpTrial, TrialProcess);
  TA_BASEFUNS(APBpTrial);
};

class APBpMaxDa_De : public Stat {
  /* ##COMPUTE_IN_SettleProcess ##LOOP_STAT computes max of da and ddE to determine
     when to stop settling in almeida-pineda algorithm */
public:
  StatVal	da_de;		// max of delta-activation or delta-error

  void		RecvCon_Run(Unit*)	{ }; // don't do these!
  void		SendCon_Run(Unit*)	{ };

  void		InitStat();
  void		Init();
  bool		Crit();
  void		Network_Init();
  void 		Unit_Stat(Unit* unit);

  void 	Initialize();		// set minimums
  void	Destroy()		{ };
  SIMPLE_COPY(APBpMaxDa_De);
  COPY_FUNS(APBpMaxDa_De, Stat);
  TA_BASEFUNS(APBpMaxDa_De);
};

//////////////////////////////////////////
//	Additional Unit Types		//
//////////////////////////////////////////

class NoisyRBpUnitSpec : public RBpUnitSpec {
  // RBp with noisy output signal (act plus noise)
public:
  Random	noise;		// what kind of noise to add to activations
  float		sqrt_dt; 	// #HIDDEN square-root of dt for noise

  void 		Compute_Act_impl(RBpUnit* u);

  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ };
  void 	InitLinks();
  SIMPLE_COPY(NoisyRBpUnitSpec);
  COPY_FUNS(NoisyRBpUnitSpec, RBpUnitSpec);
  TA_BASEFUNS(NoisyRBpUnitSpec);
};

//////////////////////////////////
//	Bp Wizard		//
//////////////////////////////////

class BpWizard : public Wizard {
  // backprop-specific wizard for automating construction of simulation objects
public:
  virtual void	SRNContext(Network* net);
  // #MENU_BUTTON #MENU_ON_Network #MENU_SEP_BEFORE configure a simple-recurrent-network context layer in the network

  virtual bool	ToTimeEvents(Environment* env);
  // #MENU_BUTTON #MENU_ON_Environment #MENU_SEP_BEFORE convert events, groups, and environment to TimeEvent format
  virtual void	ToRBPEvents(Environment* env, int targ_time = 2);
  // #MENU_BUTTON convert events to format suitable for training by RBP, with inputs coming on first, and then targets coming on after targ_time time steps

  void 	Initialize();
  void 	Destroy()	{ };
//   SIMPLE_COPY(BpWizard);
//   COPY_FUNS(BpWizard, Wizard);
  TA_BASEFUNS(BpWizard);
};

#endif // rbp_h

