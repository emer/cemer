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


// bp_v3_compat.h -- v3 compatibility skeleton classes

#ifndef BP_V3_COMPAT_H
#define BP_V3_COMPAT_H

#include "bp.h"
#include "rbp.h"
#include "v3_compat.h"
#include "bp_def.h"
#include "bp_TA_type.h"

class BP_API BpTrial : public TrialProcess {
  // standard Bp feed-forward trial
INHERITED(TrialProcess)
public:
  bool		bp_to_inputs;	// #DEF_false backpropagate errors to input layers (faster if not done, which is the default)

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(BpTrial);
};

//////////////////////////////////////////
//	Additional Stat Types 		//
//////////////////////////////////////////

class BP_API CE_Stat : public Stat {
  // ##COMPUTE_IN_TrialProcess Cross-entropy error statistic (asymmetric divergence)
INHERITED(Stat)
public:
  StatVal	ce;			// cross-entropy error
  float		tolerance;		// if error is less than this, its 0

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(CE_Stat);
};

class BP_API NormDotProd_Stat : public Stat {
  // ##COMPUTE_IN_TrialProcess Normalized Dot Product of act and target values
INHERITED(Stat)
public:
  StatVal	ndp;		 // normalized dot product

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(NormDotProd_Stat);
};

class BP_API VecCor_Stat : public Stat {
  // ##COMPUTE_IN_TrialProcess Vector Correlation of act and target values
INHERITED(Stat)
public:
  StatVal	vcor;		 // vector correlation
  float		dp;		 // #HIDDEN
  float		l1;		 // #HIDDEN
  float		l2;		 // #HIDDEN

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(VecCor_Stat);
};

class BP_API NormVecLen_Stat : public Stat {
  // ##COMPUTE_IN_TrialProcess Normalized Vector Length of act and target values
INHERITED(Stat)
public:
  StatVal	nvl;		 // normalized vector length

  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(NormVecLen_Stat);
};


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

class BP_API RBpTrial : public BpTrial {
  // one presentation of an event to RBp
INHERITED(BpTrial)
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

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(RBpTrial);
};

class BP_API RBpSequence : public SequenceProcess {
  // one sequence of events, handles TimeEvents properly
INHERITED(SequenceProcess)
public:
  void 	Initialize() {}
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(RBpSequence);
};


class BP_API RBpSE_Stat : public SE_Stat {
  // Squared error for recurrent backprop, mulitplies by dt
INHERITED(SE_Stat)
public:
  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(RBpSE_Stat);
};


//////////////////////////////////////////
//	Almeida-Pineda Algorithm	//
//////////////////////////////////////////

class BP_API APBpCycle : public CycleProcess {
  // one cycle of processing in almeida-pineda (either act or bp depending on 'phase')
INHERITED(CycleProcess)
public:
  APBpSettle*	apbp_settle;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent settle proc
  APBpTrial* 	apbp_trial;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent phase trial

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(APBpCycle);
};

class BP_API APBpSettle : public SettleProcess {
  // one settling phase in Almeide-Pineda (either act or bp depending on phase)
INHERITED(SettleProcess)
public:
  APBpTrial* 	apbp_trial;
  // #NO_SUBTYPE #READ_ONLY #NO_SAVE pointer to parent phase trial

  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(APBpSettle);
};


class BP_API APBpTrial : public TrialProcess {
  // one Almeida-Pineda BP Trial
INHERITED(TrialProcess)
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


  void	Initialize();
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(APBpTrial);
};

class BP_API APBpMaxDa_De : public Stat {
  /* ##COMPUTE_IN_SettleProcess ##LOOP_STAT computes max of da and ddE to determine
     when to stop settling in almeida-pineda algorithm */
INHERITED(Stat)
public:
  StatVal	da_de;		// max of delta-activation or delta-error

  void 	Initialize();		// set minimums
  void 	Destroy()		{ CutLinks(); }
  TA_SIMPLE_BASEFUNS(APBpMaxDa_De);
};

class BP_API V3BpProject : public V3ProjectBase {
INHERITED(V3ProjectBase)
  public:

  override bool	ConvertToV4_impl(); 

  void	Initialize() {};
  void	Destroy() 	{ };
  TA_BASEFUNS(V3BpProject);
};


#endif // BP_V3_COMPAT_H
