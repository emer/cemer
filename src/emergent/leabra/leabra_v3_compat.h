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


// leabra_v3_compat.h -- v3 compatibility skeleton classes

#ifndef LEABRA_V3_COMPAT_H
#define LEABRA_V3_COMPAT_H

#include "leabra.h"
#include "v3_compat.h"
#include "leabra_def.h"
#include "leabra_TA_type.h"

// forwards this file
class LeabraCycle;
class LeabraSettle;
class LeabraTrial;

class LeabraMaxDa;

//////////////////////////
// 	Processes	//
//////////////////////////

class LEABRA_API LeabraCycle : public CycleProcess {
  // one Leabra cycle of activation updating
INHERITED(CycleProcess)
public:
  void 	Initialize() {};
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(LeabraCycle);
};

class LEABRA_API LeabraSettle : public SettleProcess {
  // Leabra settling phase of activation updating
INHERITED(SettleProcess)
public:
  int		min_cycles;	// #DEF_15 minimum number of cycles to settle for
  int		min_cycles_phase2; // #DEF_15 minimum number of cycles to settle for in second phase
  int		netin_mod;	// #DEF_1 net input computation modulus: how often to compute netinput vs. activation update (2 = faster)
  bool		send_delta;	// #DEF_false send netin deltas instead of raw netin: more efficient (automatically sets corresponding unitspec flag)

  void 	Initialize() {};
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(LeabraSettle);
};

class LEABRA_API LeabraTrial : public TrialProcess {
  // Leabra trial process, iterates over phases
INHERITED(TrialProcess)
public:
  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize state
    DECAY_STATE			// decay the state
  };
    
  enum Phase {
    MINUS_PHASE = 0,		// minus phase
    PLUS_PHASE = 1,		// plus phase
    MINUS_2 = 2,		// second minus phase
    PLUS_2 = 3			// second plus phase
  };

  enum PhaseOrder {
    MINUS_PLUS,			// standard minus-plus (err and assoc)
    PLUS_ONLY,			// only present the plus phase (hebbian-only)
    MINUS_PLUS_NOTHING,		// auto-encoder version with final 'nothing' minus phase
    PLUS_NOTHING,		// just the auto-encoder (no initial minus phase)
    MINUS_PLUS_PLUS,		// two plus phases for gated context layer updating
    MINUS_PLUS_2		// two minus-plus phases (for pfc/bg system)
  };

  enum FirstPlusdWt {
    NO_FIRST_DWT,		// for three phase cases: don't change weights after first plus
    ONLY_FIRST_DWT,		// for three phase cases: only change weights after first plus
    ALL_DWT			// for three phase cases: change weights after *both* post-minus phases
  };

  PhaseOrder	phase_order;	// [Default: MINUS_PLUS] number and order of phases to present
  Counter	phase_no;	// Current phase number
  Phase		phase;		// Type of current phase: minus or plus
  StateInit	trial_init;	// #DEF_DECAY_STATE how to initialize network state at start of trial
  bool		no_plus_stats;	// #DEF_true don't do stats/logging in the plus phase
  bool		no_plus_test; 	// #DEF_true don't run the plus phase when testing
  FirstPlusdWt	first_plus_dwt;	// #CONDEDIT_ON_phase_order:MINUS_PLUS_PLUS how to change weights on first plus phase if 2 plus phases (applies only to standard leabralayer specs -- others must decide on their own!)
  int		cycle;		// #READ_ONLY #NO_SAVE current cycle value as copied from settle process ONLY VALID DURING PROCESSING

  void	Initialize() {};
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(LeabraTrial);
};

//////////////////////////
// 	Stats		//
//////////////////////////

class LEABRA_API LeabraMaxDa : public Stat {
  // ##COMPUTE_IN_SettleProcess ##LOOP_STAT stat that computes maximum change in activation, used for determining equilibrium to stop settling; also looks for maximum activation on target layers to provide that as an additional stopping criterion
INHERITED(Stat)
public:
  enum dAType {
    DA_ONLY,			// just use da
    INET_ONLY,			// just use inet
    INET_DA			// use inet if no activity, then use da
  };

  dAType	da_type;	// #DEF_INET_DA type of activation change measure to use
  float		inet_scale;	// #DEF_1 how to scale the inet measure to be like da
  float		lay_avg_thr;	// #DEF_0.01 threshold for layer average activation to switch to da fm Inet
  StatVal	da;		// absolute value of activation change -- set the stopping criterion here to stop network settling when change has gone below threshold (typically .005)
  StatVal	trg_max_act;	// target layer(s) maximum activation value -- set the stopping criterion here to stop network settling when activation in target layer exceeds threshold (typically .85)

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(LeabraMaxDa);
};

class LEABRA_API LeabraSE_Stat : public SE_Stat {
  // squared error for leabra, controls when to compute SE 
INHERITED(SE_Stat)
public:
  LeabraNetwork* 	trial_proc;	// #READ_ONLY #NO_SAVE the trial process to get phase info
  Unit::ExtType	targ_or_comp;	// when to compute SE: targ = 1st minus, comp = 2nd minus, both = both
  bool		no_off_err;	// do not count a unit wrong if it is off but target says on -- only count wrong units that are on but should be off

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(LeabraSE_Stat);
};

class LEABRA_API LeabraGoodStat : public Stat {
  // ##COMPUTE_IN_TrialProcess constraint satisfaction goodness statistic
INHERITED(Stat)
public:
  bool		subtr_inhib;	// subtract inhibition from harmony?
  StatVal	hrmny;		// harmony = act * netin = a_i sum_j a_j w_ij
  StatVal	strss;		// stress = act * log(act)
  StatVal	gdnss;		// goodness = harmony + stress

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(LeabraGoodStat);
};

class LEABRA_API LeabraSharpStat : public Stat {
  // ##COMPUTE_IN_TrialProcess layer sharpness statistic: just max / avg
INHERITED(Stat)
public:
  StatVal	sharp;		// sharpness = max / avg 

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(LeabraSharpStat);
};

class LEABRA_API WrongOnStat : public Stat {
  // ##COMPUTE_IN_TrialProcess Reports an error if a unit is on when it shouldn't have been (for multiple output cases)
INHERITED(Stat)
public:
  Layer*	trg_lay;
  // target layer, containing activation pattern for all possible correct responses
  StatVal	wrng;		// wrong on error statistic
  float		threshold;	// activation value to consider unit being on

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(WrongOnStat);
};

class LEABRA_API LeabraPrjnRelNetinStat : public Stat {
  // ##COMPUTE_IN_TrialProcess computes overall relative netinput contributions for the different projections into a layer. Useful for setting wt_scale parameters to achieve desired relative contributions of different inputs.  you MUST set the layer parameter to the layer in question
INHERITED(Stat)
public:
  StatVal_List	relnet;		// relative netinput contributions for the different projections into units in this layer
  float		recv_act_thr;	// #DEF_0.1 only compute netinput for receiving units that are active above this threshold (prevents dilution by varying numbers of inactive units)

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(LeabraPrjnRelNetinStat);
};

class LEABRA_API ExtRew_Stat : public Stat {
  // ##COMPUTE_IN_TrialProcess ##FINAL_STAT external reward statistic
INHERITED(Stat)
public:
  StatVal	rew;		// external reward value

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(ExtRew_Stat);
};

//////////////////////////////////////////
// 	Phase-Order  Environment	//
//////////////////////////////////////////

class LEABRA_API PhaseOrderEventSpec : public EventSpec {
  // event specification including order of phases
INHERITED(EventSpec)
public:
  enum PhaseOrder {
    MINUS_PLUS,			// minus phase, then plus phase
    PLUS_MINUS,			// plus phase, then minus phase
    MINUS_ONLY,			// only present minus
    PLUS_ONLY			// only present plus
  };

  PhaseOrder	phase_order;	// order to present phases of stimuli to network

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(PhaseOrderEventSpec);
};


class LEABRA_API V3LeabraProject : public V3ProjectBase {
INHERITED(V3ProjectBase)
  public:

  override bool	ConvertToV4_impl(); 

  void	Initialize() {};
  void	Destroy() 	{ };
  TA_BASEFUNS(V3LeabraProject);
};

#endif // LEABRA_V3_COMPAT_H
