// Copyright, 1995-2005, Regents of the University of Colorado,
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

#ifndef  cs_v3_compat_h
#define cs_v3_compat_h

#include "cs.h"
#include "v3_compat.h"

class CsSample;
class CsTrial;
class CsSettle;
class CsCycle;

class CsMaxDa;
class CsDistStat;
class CsTIGstat;
class CsTargStat;
class CsGoodStat;

////////////////////////////////
// 	Processes             //
////////////////////////////////

class CsCycle : public CycleProcess {
  // one update cycle of all units (or n_updates in async) in network
INHERITED(CycleProcess)
public:
  enum UpdateMode {
    SYNCHRONOUS,
    ASYNCHRONOUS,
    SYNC_SENDER_BASED 		// needed for IAC send_thresh impl
  };

  UpdateMode 	update_mode;
  // how to update: async = n_updates, sync = all units. sender_based is for IAC
  int		n_updates;
  // #CONDEDIT_ON_update_mode:ASYNCHRONOUS for ASYNC mode, number of updates (with replacement) to perform in one cycle

  TA_SIMPLE_BASEFUNS(CsCycle);
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};


class CsSettle : public SettleProcess {
  // one settle to equilibrium of constrant satsisfaction
INHERITED(SettleProcess)
public:
  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize the network state
    MODIFY_STATE 		// modify state (algorithm specific)
  };

  StateInit	between_phases;	// what to do between phases
  uint 		n_units;	// #HIDDEN for asynchronous update in cycle process
  bool		deterministic;  // only compute stats after the last cycle (deterministic mode)
  int		start_stats;	// #CONDEDIT_ON_deterministic:false the cycle at which to start aggregating dWt
   
  TA_SIMPLE_BASEFUNS(CsSettle);
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};


class CsTrial : public TrialProcess {
  // one minus phase and one plus phase of settling
INHERITED(TrialProcess)
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

  TA_SIMPLE_BASEFUNS(CsTrial);
private:
  void	Initialize();
  void	Destroy() { };
};

class CsSample : public TrialProcess {
  // Samples over Cs Trials (
INHERITED(TrialProcess)
public:
  Counter	sample;

  TA_SIMPLE_BASEFUNS(CsSample);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class CsMaxDa : public Stat {
 // ##COMPUTE_IN_SettleProcess ##LOOP_STAT stat that computes when equilibrium is
INHERITED(Stat)
public:
  StatVal	da;		// delta-activation

  TA_SIMPLE_BASEFUNS(CsMaxDa);
private:
  void 	Initialize();		// set minimums
  void	Destroy()		{ CutLinks(); }
};


class CsDistStat : public Stat {
  /* ##COMPUTE_IN_SettleProcess ##LOOP_STAT gets actual distributions for TIG Stat
     aggregation makes avg of this in phases, TIG stat in trial */
INHERITED(Stat)
public:
  StatVal_List	probs;		// prob of each dist pattern
  float_RArray	act_vals;	// #HIDDEN  the act values read from network
  float		tolerance;	// the tolerance for judging if act=targ
  int		n_updates;	// #HIDDEN  the number of stat cycles so far

  TA_SIMPLE_BASEFUNS(CsDistStat);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class CsTIGstat : public Stat {
  /* ##COMPUTE_IN_CsSample ##FINAL_STAT Total Information Gain statistic,
     needs a dist stat to compute raw stats for this one */
INHERITED(Stat)
public:
  StatVal	tig;		// the Information Gain for the trial
  CsDistStat*	dist_stat; 	// get the actual distributions from this stat

  TA_SIMPLE_BASEFUNS(CsTIGstat);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class CsTargStat : public Stat {
  /* ##COMPUTE_IN_CsSample ##FINAL_STAT computes the pct in target distribution,
     is just like a TIG stat in that it gets raw values from dist stat */
INHERITED(Stat)
public:
  StatVal	trg_pct;	// the pct in target for the trial

  TA_SIMPLE_BASEFUNS(CsTargStat);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class CsGoodStat : public Stat {
  // ##COMPUTE_IN_TrialProcess constraint satisfaction goodness statistic
INHERITED(Stat)
public:
  bool		use_netin;
  // use net-input for harmony instead of computing anew?

  StatVal	hrmny;
  StatVal	strss;
  StatVal	gdnss;
  float		netin_hrmny;	// #READ_ONLY temp variable to hold netin-based harmony

  TA_SIMPLE_BASEFUNS(CsGoodStat);
private:
  void	Initialize();
  void	Destroy() { };
};

class CS_API V3CsProject : public V3ProjectBase {
  // self-organizing learning project
INHERITED(V3ProjectBase)
public:

  override bool	ConvertToV4_impl(); 

  void	Initialize() {};
  void	Destroy() 	{ };
  TA_BASEFUNS(V3CsProject);
};

#endif	// cs_v3_compat_h

