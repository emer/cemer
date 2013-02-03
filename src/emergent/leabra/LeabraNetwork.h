// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef LeabraNetwork_h
#define LeabraNetwork_h 1

// parent includes:
#include <Network>

// member includes:
#include <taMath_float>
#include <CtSRAvgVals>
#include <LeabraLayer>
#include <LeabraPrjn>

// declare all other types mentioned but not required to include:
class DataTable; // 

TypeDef_Of(LeabraNetMisc);

class LEABRA_API LeabraNetMisc : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra misc network-level parameters for Leabra
INHERITED(taOBase)
public:
  bool		cyc_syn_dep;	// if true, enable synaptic depression calculations at the synapse level (also need conspecs to implement this -- this just enables computation)
  int		syn_dep_int;	// [20] #CONDSHOW_ON_cyc_syn_dep synaptic depression interval -- how frequently to actually perform synaptic depression within a trial (uses ct_cycle variable which counts up continously through trial)

  override String       GetTypeDecoKey() const { return "Network"; }

  SIMPLE_COPY(LeabraNetMisc);
  TA_BASEFUNS(LeabraNetMisc);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

 TypeDef_Of(CtTrialTiming);

class LEABRA_API CtTrialTiming : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra timing parameters for a single stimulus input trial of ct learning algorithm
INHERITED(taOBase)
public:
  bool		use;		// whether to use these parameters to determine trial timing -- on by default for all Ct (continuous time) algorithms, and off for CHL by default
  int		minus;		// #DEF_50:200 #CONDSHOW_ON_use number of cycles to run in the minus phase with only inputs and no targets (used by CtLeabraSettle program), sets cycle_max -- can be 0
  int		plus;		// #DEF_20:200 #CONDSHOW_ON_use number of cycles to run in the plus phase with input and target activations (used by CtLeabraSettle program), sets cycle_max -- must be > 0
  int		inhib;		// #DEF_0;1 #CONDSHOW_ON_use number of cycles to run in the final inhibitory phase -- only relevant for MINUS_PLUS_NOTHING case
  int		n_avg_only_epcs; // #DEF_0 #CONDSHOW_ON_use number of epochs during which time only ravg values are accumulated, and no learning occurs

  int		total_cycles;	// #READ_ONLY computed total number of cycles per trial
  int		inhib_start;	// #READ_ONLY computed start of inhib phase (=minus + plus)

  override String       GetTypeDecoKey() const { return "Network"; }

  SIMPLE_COPY(CtTrialTiming);
  TA_BASEFUNS(CtTrialTiming);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

TypeDef_Of(CtSRAvgSpec);

class LEABRA_API CtSRAvgSpec : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra how to compute the sravg value as a function of cycles 
INHERITED(taOBase)
public:
  bool		manual;		// the determination of when to compute sravg is made externally (e.g., by a program) -- sravg_vals.state flag must be set prior to calling Network::Cycle_Run() to take effect -- by default layers will copy network sravg_vals.state flag setting, so you only need to set it globally for the network (see layer-level manual_sravg flag if you want to set everything manually)
  int		start;		// #CONDSHOW_OFF_manual #DEF_30:60 number of cycles from the start of a new pattern to start computing sravg value -- avoid transitional states that are too far away from attractor state
  int		end;		// #CONDSHOW_OFF_manual #DEF_0;1 number of cycles from the start of the final inhibitory phase to continue recording sravg
  int		interval;	// #CONDSHOW_OFF_manual #DEF_1;5 #MIN_1 (1 for XCAL, 5 for CAL) how frequently to compute sravg -- in XCAL this is not expensive so do it every cycle, but for CAL more infrequent updating saves computational costs as sravg is expensive
  int		plus_s_st;	// #CONDSHOW_OFF_manual [10 for spiking, else plus-1, typically 19] how many cycles into the plus phase should the short time scale sravg computation start (only for TRIAL sravg computation)
  bool		plus_s_only;	// #CONDSHOW_OFF_manual plus increments short-term only, not short and medium term activations
  bool		force_con;	// #DEF_false force connection-level SRAvg computation -- only use for experimental algorithms that need this -- otherwise needlessly slows computation

  override String       GetTypeDecoKey() const { return "Network"; }

  SIMPLE_COPY(CtSRAvgSpec);
  TA_BASEFUNS(CtSRAvgSpec);
  //protected:
  //  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};


TypeDef_Of(CtSineInhibMod);

class LEABRA_API CtSineInhibMod : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra sinusoidal inhibitory modulation parameters simulating initial burst of activation and subsequent oscillatory ringing
INHERITED(taOBase)
public:
  int		start;		// #DEF_30:60 number of cycles from onset of new input to start applying sinusoidal inhibitory modulation
  int		duration;	// #DEF_20 number of cycles from start to apply modulation
  float		n_pi;		// #DEF_2 number of multiples of PI to produce within duration of modulation (1.0 = positive only wave, 2.0 = full pos/neg wave, 4.0 = two waves, etc)
  float		burst_i;	// #DEF_0.02;0 maximum reduction in inhibition as a proportion of computed kwta value to subtract for positive activation (burst) phase of wave -- value should be a positive number
  float		trough_i;	// #DEF_0.02;0 maximum extra inhibition as proportion of computed kwta value to add for negative activation (trough) phase of wave -- value shoudl be a positive number

  float		GetInhibMod(int ct_cycle, float bi, float ti) {
    if((ct_cycle < start) || (ct_cycle >= (start + duration))) return 0.0f;
    float rads = (((float)(ct_cycle - start) / (float)duration) * taMath_float::pi * n_pi);
    float sinval = -taMath_float::sin(rads);
    if(sinval < 0.0f) 	sinval *= bi; // signs are reversed for inhib vs activation
    else		sinval *= ti;
    return sinval;
  }
  // returns inhibitory modulation to apply as a fraction of computed kwta value

  override String       GetTypeDecoKey() const { return "Network"; }

  SIMPLE_COPY(CtSineInhibMod);
  TA_BASEFUNS(CtSineInhibMod);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

TypeDef_Of(CtFinalInhibMod);

class LEABRA_API CtFinalInhibMod : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra extra inhibition to apply at end of stimulus processing during inhib phase, to clear out existing pattern
INHERITED(taOBase)
public:
  int		start;		// number of cycles into inhib phase for inhibition ramp to start
  int		end;		// number of cycles into inhib phase for inhibition ramp to end -- remains at full inhibition level from end to end of inhib phase
  float		inhib_i;	// [.05 when in use] maximum extra inhibition as proportion of computed kwta value to add during final inhib phase

  float		GetInhibMod(int inh_cyc, float ii) {
    if(inh_cyc < start) return 0.0f;
    if(inh_cyc >= end) return ii;
    float slp = (float)(inh_cyc - start) / (float)(end - start);
    return slp * ii;
  }
  // returns inhibitory modulation to apply as a fraction of computed kwta value

  override String       GetTypeDecoKey() const { return "Network"; }

  SIMPLE_COPY(CtFinalInhibMod);
  TA_BASEFUNS(CtFinalInhibMod);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};


TypeDef_Of(CtLrnTrigSpec);

class LEABRA_API CtLrnTrigSpec : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra continuous-time learning trigger -- based on overall rate of change of the short-term average activation in the layer
INHERITED(taOBase)
public:
  int		plus_lrn_cyc;	// #DEF_-1 if this is > 0, then do learning at this number of cycles into the plus phase, instead of what would be computed by the parameters here -- allows for debugging of other network parameters and comparison with an 'optimal' learning trigger
  float		davg_dt;	// #DEF_0.1 #MIN_0 #MAX_1 time constant (rate) for continuously updating the delta-average activation value (davg) -- provides some level of initial smoothing over the instantaneous delta-avg_s value, which can otherwise be somewhat noisy
  float		davg_s_dt;	// #DEF_0.05 #MIN_0 #MAX_1 time constant (rate) for continuously updating the short-time frame average of the davg value -- this is contrasted with davg_m to give a smooth acceleration term to measure jolt
  float		davg_m_dt;	// #DEF_0.03 #MIN_0 #MAX_1 time constant (rate) for continuously updating the medium-time frame average of the davg value -- this is contrasted with davg_s to give a smooth acceleration term to measure jolt
  float		davg_l_dt;	// #DEF_0.0005 #MIN_0 #MAX_1 time constant (rate) for continuously updating the long-term average of davg_smd (davg_l) and the long-term maximum (davg_max) -- provides the range for the threshold value computation
  float		thr_min;	// #DEF_-0.15 minimum threshold for learning as a proportion of distance between davg_l and davg_max -- current local maximum davg_smd value is compared to this threshold
  float		thr_max;	// #DEF_0.5 maximum threshold for learning as a proportion of distance between davg_l and davg_max -- current local maximum davg_smd value is compared to this threshold -- changes can be too big for learning -- typically representing transitions between disparate events, so this value should be < 1
  int		loc_max_cyc;	// #DEF_8 #MIN_1 how many cycles of downward-going davg_smd values are required past a local peak, before that peak value is used for checking against the thresholds -- this is an exact number -- if doesn't meet criteria at this point, it is skipped until next peak
  float		loc_max_dec;	// #DEF_0.01 #MIN_0 how much must the current davg_smd value drop below the local max value before learning can take place -- this ensures that the peak was not just a tiny wiggle -- expressed as a positive number 
  int		lrn_delay;	// #DEF_40 #MIN_1 how many cycles after learning is triggered does it actually take place?
  int		lrn_refract;	// #DEF_100 #MIN_1 refractory period after learning before any learning can take place again
  float		davg_l_init; 	// #DEF_0 #MIN_0 initial value for davg_l
  float		davg_max_init; 	// #DEF_0.001 #MIN_0 initial value for davg_max

  float		davg_time;	// #READ_ONLY #SHOW time constant (in cycles, 1/davg_dt) for continuously updating davg
  float		davg_s_time; 	// #READ_ONLY #SHOW time constant (in cycles, 1/davg_m_dt) for continuously updating davg_m
  float		davg_m_time; 	// #READ_ONLY #SHOW time constant (in cycles, 1/davg_m_dt) for continuously updating davg_m
  float		davg_l_time; 	// #READ_ONLY #SHOW time constant (in cycles, 1/davg_l_dt) for continuously updating davg_l

  float		lrn_delay_inc;	// #READ_ONLY #HIDDEN 1.0f / lrn_delay_inc -- increment per count to compute normalized lrn_trig
  float		lrn_refract_inc; // #READ_ONLY #HIDDEN 1.0f / lrn_refract_inc -- increment per count to compute normalized lrn_trig

  override String       GetTypeDecoKey() const { return "Network"; }

  TA_SIMPLE_BASEFUNS(CtLrnTrigSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
};


TypeDef_Of(CtLrnTrigVals);

class LEABRA_API CtLrnTrigVals : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra state variables for continuous-time learning trigger -- based on overall rate of change of the short-term average activation in the layer
INHERITED(taOBase)
public:
  float		davg;		// average absolute value of davg delta average activation change across entire network
  float		davg_s;		// shorter time average of the davg value 
  float		davg_m;		// medium time average of the davg value
  float		davg_smd;	// davg_s - davg_m -- local acceleration of the curve
  float		davg_l; 	// long-term average of davg_smd -- used for enabling normalized thresholds between this long-term average and the long-term max davg_max
  float		davg_max; 	// long-term maximum of davg_smd -- used for enabling normalized thresholds between davg_l and this long-term max 
  int		cyc_fm_inc;	// number of cycles since an increase was detected
  int		cyc_fm_dec;	// number of cycles since a decrease was detected
  float		loc_max;	// local maximum value of davg_smd -- jolt detection happens on the local peak of davg_m
  float		lrn_max;	// local maximum value of davg_smd that actually drove learning -- recorded for stats purposes
  float		lrn_trig;	// learning trigger variable -- determines when to learn -- starts counting up from 0 to 1 when threshold is met -- learning happens at 1
  int		lrn;		// did layer learn on this cycle -- 1 if true, 0 if false

  float		lrn_min;	// #NO_SAVE #CAT_Statistic what proportion of times did it learn in first minus phase -- computed every epoch
  float		lrn_min_cyc;	// #NO_SAVE #CAT_Statistic how many cycles into first minus phase did it learn on average -- computed every epoch
  float		lrn_min_thr;	// #NO_SAVE #CAT_Statistic average value of threshold-determining variable (davg_smd) for first minus phase learning -- computed every epoch
  float		lrn_min_sum;	// #NO_SAVE #CAT_Statistic #READ_ONLY sum
  float		lrn_min_cyc_sum; // #NO_SAVE #CAT_Statistic #READ_ONLY sum
  float		lrn_min_thr_sum; // #NO_SAVE #CAT_Statistic #READ_ONLY sum

  float		lrn_plus;	// #NO_SAVE #CAT_Statistic what proportion of times did it learn in plus phase -- computed every epoch
  float		lrn_plus_cyc;	// #NO_SAVE #CAT_Statistic how many cycles into plus phase did it learn on average -- computed every epoch
  float		lrn_plus_thr;	// #NO_SAVE #CAT_Statistic average value of threshold-determining variable (davg_smd) for plus phase learning -- computed every epoch
  float		lrn_plus_sum;	// #NO_SAVE #CAT_Statistic #READ_ONLY sum
  float		lrn_plus_cyc_sum; // #NO_SAVE #CAT_Statistic #READ_ONLY sum
  float		lrn_plus_thr_sum; // #NO_SAVE #CAT_Statistic #READ_ONLY sum

  float		lrn_noth;	// #NO_SAVE #CAT_Statistic what proportion of times did it learn in nothing phase -- computed every epoch
  float		lrn_noth_cyc;	// #NO_SAVE #CAT_Statistic how many cycles into nothing phase did it learn on average -- computed every epoch
  float		lrn_noth_thr;	// #NO_SAVE #CAT_Statistic average value of threshold-determining variable (davg_smd) for nothing phase learning -- computed every epoch
  float		lrn_noth_sum;	// #NO_SAVE #CAT_Statistic #READ_ONLY sum
  float		lrn_noth_cyc_sum; // #NO_SAVE #CAT_Statistic #READ_ONLY sum
  float		lrn_noth_thr_sum; // #NO_SAVE #CAT_Statistic #READ_ONLY sum

  int		lrn_stats_n;	// #NO_SAVE #CAT_Statistic #READ_ONLY count of number of times stats have been incremented

  void		Init_Stats();	// initialize stats vars (all to 0)
  void		Init_Stats_Sums(); // initialize stats sums (all to 0)

  override String       GetTypeDecoKey() const { return "Network"; }

  TA_SIMPLE_BASEFUNS(CtLrnTrigVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};


TypeDef_Of(LeabraNetwork);

class LEABRA_API LeabraNetwork : public Network {
  // #STEM_BASE ##CAT_Leabra network that uses the Leabra algorithms and objects
INHERITED(Network)
public:
  // IMPORTANT programming note: this enum must be same as in LeabraConSpec

  enum LearnRule {
    CTLEABRA_XCAL,		// Continuous-Time Leabra temporally eXtended Contrastive Attractor Learning rule, trial-based version, which has two time scales of contrasts: short-vs-medium (svm) and medium-vs-long (mvl): (<sr>_s - <sr>_m) + (<sr>_m - <r>_l) -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales.  svm is basically error-driven learning, and mvl is BCM-style self-organizing learning.
    LEABRA_CHL,			// standard Leabra Contrastive Hebbian Learning rule with hebbian self-organizing factor: (s+r+) - (s-r-) + r+(s+ - w) -- s=sender,r=recv +=plus phase, -=minus phase, w= weight
    CTLEABRA_XCAL_C,		// Continuous-Time Leabra temporally eXtended Contrastive Attractor Learning rule, fully continuous version, which has two time scales of contrasts: short-vs-medium (svm) and medium-vs-long (mvl): (<sr>_s - <sr>_m) + (<sr>_m - <r>_l) -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales.  svm is basically error-driven learning, and mvl is BCM-style self-organizing learning.
    CTLEABRA_CAL,		// Continuous-Time Leabra Contrastive Attractor Learning rule: <sr>_s - <sr>_m -- s=sender, r=recv, <> = avg over short (plus phase) and medium (trial) time scales -- purely error-driven but inhibitory oscillations can drive self-organizing component -- requires LeabraSRAvgCon connections
  };

  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize state
    DECAY_STATE,		// decay the state
  };
    
  enum Phase {
    MINUS_PHASE = 0,		// minus phase
    PLUS_PHASE = 1,		// plus phase
  };

  enum PhaseOrder {
    MINUS_PLUS,			// standard minus-plus (err and assoc)
    PLUS_MINUS,			// reverse order: plus phase first
    PLUS_ONLY,			// only present the plus phase (hebbian-only)
    PLUS_NOTHING,		// just an auto-encoder (no initial minus phase)
    MINUS_PLUS_NOTHING,		// standard for CtLeabra_X/CAL and auto-encoder version with final 'nothing' minus phase
    MINUS_PLUS_MINUS,		// alternative version for CtLeabra_X/CAL with input still in final phase -- this 2nd minus is also marked as a nothing_phase 
  };

  enum ThreadFlags { // #BITS flags for controlling the parallel threading process (which functions are threaded)
    TF_NONE	= 0x00,	// #NO_BIT no thread flags set
    NETIN 	= 0x01,	// ~20% of compute time, norm comp val = 1.0, the net input computation (sender-based), computed per cycle
    NETIN_INTEG	= 0x02,	// ~20% of compute time, norm comp val = 1.0, the net input computation (sender-based), computed per cycle
    SRAVG 	= 0x04,	// ~12% of compute time, norm comp val = 0.9, the sender-receiver average activation (cal only), computed per ct_sravg.interval (typically every 5 cycles)
    ACT		= 0x08,	// ~7% of compute time, norm comp val = 0.4, activation, computed per cycle
    WEIGHTS	= 0x10,	// ~7% of compute time, norm comp val = 1.0, weight update from dwt changes, computed per trial (and still that expensive)
    DWT		= 0x20,	// ~3% of compute time, norm comp val = 0.6, delta-weight changes (learning), computed per trial
    TRIAL_INIT	= 0x40,	// ~2% of compute time, norm comp val = 0.2, trial-level initialization -- includes SRAvg init over connections if using xcal, which can be expensive
    SETTLE_INIT	= 0x80,	// ~.5% of compute time, norm comp val = 0.1, settle-level initialization -- only at unit level and the most lightweight function -- may not be worth it in general to parallelize
    TF_ALL	= 0xFF,	// #NO_BIT all thread flags set
  };

  LearnRule	learn_rule;	// The variant of Leabra learning rule to use 
  PhaseOrder	phase_order;	// [Default: MINUS_PLUS] #CAT_Counter number and order of phases to present
  bool		no_plus_test;	// #DEF_true #CAT_Counter don't run the plus phase when testing
  StateInit	sequence_init;	// #DEF_DO_NOTHING #CAT_Activation how to initialize network state at start of a sequence of trials

  Phase		phase;		// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW type of settling phase
  bool		nothing_phase;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter the current phase is a NOTHING phase (phase will indicate MINUS for learning purposes)
  int		phase_no;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW phase as an ordinal number (regular phase is Phase enum)
  int		phase_max;	// #CAT_Counter maximum number of phases to run (note: this is set by Trial_Init depending on phase_order)

  int		ct_cycle;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW continuous time cycle counter: counts up from start of trial 
  float		time_inc;	// how much to increment the network time variable every cycle -- this goes monotonically up from the last weight init or manual reset

  int		cycle_max;	// #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL #DEF_60 maximum number of cycles to settle for: note for CtLeabra_X/CAL this is overridden by phase specific settings by the settle process
  int		mid_minus_cycle; // #CAT_Counter #DEF_-1:30 cycle number for computations that take place roughly mid-way through the minus phase -- used for PBWM algorithm -- effective min_cycles for minus phase will be this value + min_cycles -- set to -1 to disable
  int		min_cycles;	// #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL #DEF_15:35 minimum number of cycles to settle for
  int		min_cycles_phase2; // #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL #DEF_35 minimum number of cycles to settle for in second phase
  bool		dwt_norm_enabled; // #CAT_Learning enable dwt_norm computation -- this must be done as a separate step -- LeabraConSpec will set this flag if LeabraConSpec::wt_sig.dwt_norm flag is on, but it does not turn it back off, so if this is not being used anymore, save time by turning this flag off

  CtTrialTiming	 ct_time;	// #CAT_Learning timing parameters for ct leabra trial: Settle_Init sets the cycle_max based on these values
  CtSRAvgSpec	 ct_sravg;	// #CAT_Learning #CONDSHOW_OFF_learn_rule:LEABRA_CHL parameters controlling computation of sravg value as a function of cycles
  CtSineInhibMod ct_sin_i;	// #CAT_Learning #CONDSHOW_OFF_learn_rule:LEABRA_CHL sinusoidal inhibition parameters for inhibitory modulations during trial, simulating oscillations resulting from imperfect inhibtory set point behavior
  CtFinalInhibMod ct_fin_i;	// #CAT_Learning #CONDSHOW_OFF_learn_rule:LEABRA_CHL final inhibition parameters for extra inhibition to apply during final inhib phase, simulating slow-onset GABA currents
  CtSRAvgVals	sravg_vals;	// #NO_SAVE #CAT_Learning sender-receiver average computation values, e.g., for normalizing sravg values
  CtLrnTrigSpec	ct_lrn_trig;	// #CAT_Learning #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL_C learning trigger parameters based on changes in short-term average activation value -- determines when CTLEABRA_XCAL_C learns
  CtLrnTrigVals	lrn_trig; 	// #CAT_Learning #EXPERT #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL_C learning trigger values -- based on changes in short-term average activation value -- determines when CTLEABRA_XCAL_C learns
  ThreadFlags	thread_flags;	// #NO_SAVE #CAT_Structure #EXPERT flags for controlling the parallel threading process (which functions are threaded) -- this is just for testing and debugging purposes, and not for general use -- they are not saved

  float		minus_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cycles to settle in the minus phase -- this is the typical settling time statistic to record
  float		avg_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average settling cycles in the minus phase (computed over previous epoch)
  float		avg_cycles_sum; // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average cycles in this epoch
  int		avg_cycles_n;	// #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average cycles computation for this epoch

  String	minus_output_name; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW output_name in the minus phase -- for recording in logs as network's response (output_name in plus phase is clamped target value)

  LeabraNetMisc	net_misc;	// misc network level parameters for leabra

  float		send_pct;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic proportion of sending units that actually sent activations on this cycle
  int		send_pct_n;	// #NO_SAVE #READ_ONLY #CAT_Statistic number of units sending activation this cycle
  int		send_pct_tot;	// #NO_SAVE #READ_ONLY #CAT_Statistic total number of units that could send activation this cycle
  float		avg_send_pct;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average proportion of units sending activation over an epoch
  float		avg_send_pct_sum; // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average send_pct per epoch (integrates over cycles and trials etc)
  int		avg_send_pct_n; // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average send_pct per epoch (integrates over cycles and trials etc)

  float		maxda_stopcrit;	// #DEF_0.005;-1 #CAT_Statistic stopping criterion for max da
  float		maxda;		// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW maximum change in activation (delta-activation) over network; used in stopping settling

  float		trg_max_act_stopcrit;	// #CAT_Statistic stopping criterion for target-layer maximum activation (can be used for stopping settling)
  float		trg_max_act;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic target-layer maximum activation (can be used for stopping settling)

  float		ext_rew;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW external reward value (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network -- equals PVe value in PVLV framework
  bool		ext_rew_avail; 	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic actual external reward value is available (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network -- if false then no feedback was provided on this trial
  float		norew_val; 	// #GUI_READ_ONLY #CAT_Statistic no-reward value (serves as a baseline against which ext_rew can be compared against -- if greater, then positive reward, if less, then negative reward -- typically 0.5 but can vary
  float		avg_ext_rew;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average external reward value (computed over previous epoch)
  float		pvlv_pvi;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV primary reward prediction value PVi for the current trial -- updated on a cycle-by-cycle basis -- used for noise modulation among perhaps other things
  float		pvlv_pvr;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV primary reward availability prediction value PVr for the current trial -- updated on a cycle-by-cycle basis
  float		pvlv_lve;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV learned reward prediction value LVe (excitatory, rapidly adapting) for the current trial -- updated on a cycle-by-cycle basis -- used for noise modulation among perhaps other things
  float		pvlv_lvi;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV learned reward prediction value LVi (inhibitory, slowly adapting) for the current trial -- updated on a cycle-by-cycle basis -- used for noise modulation among perhaps other things
  float		pvlv_nv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV novelty value for the current trial -- updated on a cycle-by-cycle basis
  float		pvlv_dav;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV phasic dopamine value for the current trial -- updated on a cycle-by-cycle basis
  float		pvlv_tonic_da;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV tonic dopamine level for the current trial -- updated on a cycle-by-cycle basis
  bool		pv_detected;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV detected a situation where primary reward value is expected to be available, based on learned encoding of similar such situations in the past -- computed by the PVrLayerSpec continuously in the minus phase
  float		avg_ext_rew_sum; // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average external reward value in this epoch
  int		avg_ext_rew_n;	// #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average external reward value computation for this epoch

  bool		off_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly off (should have been on but were actually off) -- either 1 or both of off_errs and on_errs must be set
  bool		on_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly on (should have been off but were actually on) -- either 1 or both of off_errs and on_errs must be set

  float		norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW normalized binary (Hamming) error on this trial: number of units that were incorrectly activated or incorrectly inactivated (see off_errs to exclude latter)
  float		avg_norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average normalized binary error value (computed over previous epoch)
  float		avg_norm_err_sum; // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average norm err in this epoch
  int		avg_norm_err_n;	// #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average norm err value computation for this epoch

  float		cos_err_lrn_thr; // #CAT_Learning learning threshold for cos_err -- if cos err is below this value, then no learning occurs -- prevents learning when things are too far away from expectations -- esp useful for leabra ti
  float		cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cosine (normalized dot product) error on this trial -- cosine between act_m and act_p target values
  float		avg_cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average cosine (normalized dot product) error (computed over previous epoch)
  float		avg_cos_err_sum; // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average cos err in this epoch
  int		avg_cos_err_n;	// #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average cos err value computation for this epoch

  bool		inhib_cons_used; // #NO_SAVE #READ_ONLY #CAT_Threads inhibitory connections are being used in this network -- detected during buildunits_threads to determine how netinput is computed -- sets NETIN_PER_PRJN flag
  bool		init_netins_cycle_stat; // #NO_SAVE #HIDDEN #CAT_Activation flag to trigger the call of Init_Netins at the end of the Compute_CycleStats function -- this is needed for specialized cases where projection scaling parameters have changed, and thus the net inputs are all out of whack and need to be recomputed -- flag is set to false at start of Compute_CycleStats and checked at end, so layers just need to set it

  ///////////////////////////////////////////////////////////////////////
  //	Thread Flags

  inline void	SetThreadFlag(ThreadFlags flg)
  { thread_flags = (ThreadFlags)(thread_flags | flg); }
  // set flag state on
  inline void	ClearThreadFlag(ThreadFlags flg)
  { thread_flags = (ThreadFlags)(thread_flags & ~flg); }
  // clear flag state (set off)
  inline bool	HasThreadFlag(ThreadFlags flg) const { return (thread_flags & flg); }
  // check if flag is set
  inline void	SetThreadFlagState(ThreadFlags flg, bool on)
  { if(on) SetThreadFlag(flg); else ClearThreadFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  override void	Init_Acts();
  override void	Init_Counters();
  override void	Init_Stats();
  override void	Init_Sequence();
  override void Init_Weights();

  virtual void  Init_Netins();
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)

  virtual void	DecayState(float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  virtual void	SetLearnRule_ConSpecs(BaseSpec_Group* spgp);
  // #IGNORE set the current learning rule into all conspecs in given spec group (recursive)
  virtual void	SetLearnRule();
  // #CAT_ObjectMgmt set the current learning rule into the conspecs on this network (done by network UAE only when rule changed)

  virtual void	CheckInhibCons();
  override void	BuildUnits_Threads();
  override bool RecvOwnsCons() { return false; }

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void 	Trial_Init();
  // #CAT_TrialInit initialize at start of trial (SetCurLrate, set phase_max, Decay state)
    virtual void Trial_Init_Phases();
    // #CAT_TrialInit init phase_max and current phase based on phase_order -- network-only
    virtual void SetCurLrate();
    // #CAT_TrialInit set the current learning rate according to the LeabraConSpec parameters -- goes through projections

    virtual void Trial_Init_Unit();
    // #CAT_TrialInit trial unit-level initialization functions: DecayState, NoiseInit, Trial_Init_SRAvg -- replaces those functions
      virtual void Trial_DecayState();
      // #CAT_TrialInit decay the state in between trials (params in LayerSpec) -- goes to units via layers -- not typically used, Trial_Init_Unit instead
      virtual void Trial_NoiseInit();
      // #CAT_TrialInit initialize various noise factors at start of trial -- goes to units via layers
      virtual void Trial_Init_SRAvg();
      // #CAT_Learning initialize sending-receiving activation coproduct averages (CtLeabra_X/CAL) -- goes to connections via units, layers
    virtual void Trial_Init_Layer();
    // #CAT_TrialInit layer-level trial init (used in base code to init layer-level sravg, can be overloaded)

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  virtual void  Settle_Init();
  // #CAT_SettleInit initialize network for settle-level processing (decay, active k, hard clamp, netscale)
    virtual void Settle_Init_CtTimes();
    // #CAT_SettleInit initialize cycles based on network phases for CtLeabra_X/CAL
    virtual void Compute_Active_K();
    // #CAT_SettleInit determine the active k values for each layer based on pcts, etc (called by Settle_Init) -- must have hard clamp called first 
    virtual void Settle_Init_Unit();
    // #CAT_TrialInit settle unit-level initialization functions: Init_TargFlags, DecayState, NetinScale
    virtual void Settle_Init_Layer();
    // #CAT_TrialInit settle layer-level initialization hook -- default calls TargFlags_Layer, and can be used for hook for other guys
      virtual void Settle_Init_TargFlags();
      // #CAT_SettleInit initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
      virtual void Settle_DecayState();
      // #CAT_SettleInit logic for performing decay and updating external input settings as a function of phase
      virtual void Compute_NetinScale();
      // #CAT_SettleInit compute netinput scaling values by projection

    virtual void Compute_NetinScale_Senders();
    // #CAT_SettleInit compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the Settle_Init_Unit stage after all the recv ones are computed

    virtual void Compute_HardClamp();
    // #CAT_SettleInit compute hard clamping from external inputs

    virtual void ExtToComp();
    // #CAT_SettleInit move external input values to comparison values (not currently used)
    virtual void TargExtToComp();
    // #CAT_SettleInit move target and external input values to comparison (for PLUS_NOTHING, called by Settle_Init)

  virtual void  NewInputData_Init();
  // #CAT_SettleInit perform initialization stuff needed to update external input data signals so they actually show up as activations in the network: Settle_Init_Layer, Settle_Init_TrgFlags, Compute_HardClamp

  ////////////////////////////////////////////////////////////////
  //	Cycle_Run

  virtual void	Cycle_Run();
  // #CAT_Cycle compute one cycle of updating: netinput, inhibition, activations

  ///////////////////////////////////////////////////////
  //	Cycle Stage 1: netinput

  override void	Send_Netin();
  // #CAT_Cycle compute netinputs (sender-delta based -- only send when sender activations change) -- new values go in net_delta or g_i_delta (summed up from tmp array for threaded case)
  virtual void Compute_ExtraNetin();
  // #CAT_Cycle Stage 1.2 compute extra netinput based on any kind of algorithmic computation -- goes to the layerspec and stops there -- not much overhead if not used
  virtual void Compute_NetinInteg();
  // #CAT_Cycle Stage 1.2 integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)

  virtual void Compute_NetinStats();
  // #CAT_Cycle Stage 1.3 compute AvgMax stats on netin and i_thr values computed during netin computation -- used for various regulatory and monitoring functions -- not threadable


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  virtual void	Compute_Inhib();
  // #CAT_Cycle compute inhibitory conductances (kwta) -- also calls LayInhibToGps to coordinate group-level inhibition sharing
  virtual void	Compute_ApplyInhib();
  // #CAT_Cycle Stage 2.3 apply inhibitory conductances from kwta to individual units -- separate step after all inhib is integrated and computed

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  override void	Compute_Act();
  // #CAT_Cycle compute activations

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  virtual void	Compute_CycleStats();
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, MaxDa, OutputName, etc

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  virtual void 	Compute_CycSynDep();
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on -- threaded direct to units
  virtual void	Compute_MidMinus();
  // #CAT_Activation do special processing midway through the minus phase, as determined by the mid_minus_cycles parameter, if > 0 -- currently used for the PBWM algorithm

  ///////////////////////////////////////////////////////////////////////
  //	Settle Final

  virtual void	Settle_Final();
  // #CAT_SettleFinal do final processing after settling (postsettle, Compute_dWt if needed)
    virtual void PostSettle_Pre();
    // #CAT_SettleFinal perform computations in layers at end of settling -- this is a pre-stage that occurs prior to final PostSettle -- use this for anything that needs to happen prior to the standard PostSettle across layers (called by Settle_Final)
    virtual void PostSettle();
    // #CAT_SettleFinal perform computations in layers at end of settling  (called by Settle_Final)
    virtual void Settle_Compute_dWt();
    // #CAT_SettleFinal compute weight changes at end of settling as needed depending on phase order -- all weight changes are computed here for consistency
    virtual void AdaptKWTAPt();
    // #CAT_SettleFinal adapt the kwta point based on average activation values

  ///////////////////////////////////////////////////////////////////////
  //	Trial Update and Final

  virtual void Trial_UpdatePhase();
  // #CAT_TrialInit update phase based on phase_no -- typically called by program in Trial loop over settling

  virtual void	Trial_Final();
  // #CAT_TrialFinal do final processing after trial (Compute_dWt, EncodeState)
    virtual void EncodeState();
    // #CAT_TrialFinal encode final state information at end of trial for time-based learning across trials
    virtual void Compute_SelfReg_Trial();
    // #CAT_TrialFinal update self-regulation (accommodation, hysteresis) at end of trial

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  virtual void 	Compute_SRAvg_State();
  // #CAT_Learning compute state flag setting for sending-receiving activation coproduct averages (CtLeabra_X/CAL) -- called at the Cycle_Run level -- just updates the sravg_vals.state flag for network and layers, does nothing if 
  virtual void 	Compute_SRAvg();
  // #CAT_Learning compute sending-receiving activation coproduct averages (CtLeabra_X/CAL) -- called at the Cycle_Run level -- calls Compute_SRAvg_Layer on layers, and then threaded down to unit level -- behavior is completely determined by sravg_vals.state flag setting

  virtual void 	Compute_XCalC_dWt();
  // #CAT_Learning compute CT_LEABRA_XCA_C learning rule

  virtual void	Compute_dWt_Layer_pre();
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  virtual void	Compute_dWt_FirstMinus();
  // #CAT_Learning compute weight change after first minus phase has been encountered: for out-of-phase LeabraTI context layers (or anything similar)
  virtual void	Compute_dWt_FirstPlus();
  // #CAT_Learning compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  virtual void	Compute_dWt_Nothing();
  // #CAT_Learning compute weight change after final nothing phase: standard layers do a weight change here under both learning rules
  virtual void	Compute_dWt_Norm();
  // #CAT_Learning compute normalization of weight changes -- must be done as a second pass after initial weight changes

  override void Compute_Weights_impl();

  ///////////////////////////////////////////////////////////////////////
  //	Stats

  virtual void	Compute_ExtRew();
  // #CAT_Statistic compute external reward information: Must be called in plus phase (phase_no == 1)
  virtual void	Compute_NormErr();
  // #CAT_Statistic compute normalized binary error between act_m and targ unit values: called in TrialStats
  virtual float  Compute_M2SSE(bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of act_m2 activation vs target over the entire network
  virtual float  Compute_M2SSE_Recon(bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of act_m2 activation vs target over the entire network, only on input layers -- this is typically a reconstruction error signal
  virtual float	Compute_CosErr();
  // #CAT_Statistic compute cosine (normalized dot product) error between act_m and targ unit values
  virtual float	Compute_M2CosErr();
  // #CAT_Statistic compute cosine (normalized dot product) error between act_m2 and targ unit values
  virtual float	Compute_M2CosErr_Recon();
  // #CAT_Statistic compute cosine (normalized dot product) error between act_m2 and targ unit values
  virtual float	Compute_CosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) phase difference between act_m and act_p unit values
  virtual float	Compute_CosDiff2();
  // #CAT_Statistic compute cosine (normalized dot product) phase difference 2 between act_m2 and act_p unit values
  virtual void	Compute_MinusCycles();
  // #CAT_Statistic compute minus-phase cycles (and increment epoch sums) -- at the end of the minus phase (of course)
  override void	Compute_TrialStats();
  // #CAT_Statistic compute trial-level statistics, including SSE and minus cycles -- to be called at end of minus phase
  virtual bool	Compute_TrialStats_Test();
  // #CAT_Statistic determine whether it is time to run trial stats -- typically the minus phase but it depends on network phase_order settings etc

  virtual void	Compute_AbsRelNetin();
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into layers in network
  virtual void	Compute_AvgAbsRelNetin();
  // #CAT_Statistic compute time-average absolute layer-level and relative netinput from different projections into layers in network (e.g. over epoch timescale)
  virtual void	Compute_TrgRelNetin();
  // #MENU #MENU_SEP_BEFORE #CONFIRM #CAT_Learning compute target rel netin based on projection direction information plus the adapt_rel_net values in the conspec
  virtual void	Compute_AdaptRelNetin();
  // #CAT_Learning adapt the relative input values by changing the conspec wt_scale.rel parameter; See Compute_AdaptAbsNetin for adaptation of wt_scale.abs parameters to achieve good netinput values overall
  virtual void	Compute_AdaptAbsNetin();
  // #CAT_Learning adapt the absolute net input values by changing the conspec wt_scale.abs parameter

  virtual void	Compute_AvgCycles();
  // #CAT_Statistic compute average cycles (at an epoch-level timescale)
  virtual void	Compute_AvgExtRew();
  // #CAT_Statistic compute average external reward information (at an epoch-level timescale)
  virtual void	Compute_AvgNormErr();
  // #CAT_Statistic compute average norm_err (at an epoch-level timescale)
  virtual void	Compute_AvgCosErr();
  // #CAT_Statistic compute average cos_err (at an epoch-level timescale)
  virtual void	Compute_AvgSendPct();
  // #CAT_Statistic compute average sending pct (at an epoch-level timescale)
  virtual void	Compute_CtLrnTrigAvgs();
  // #CAT_Statistic compute Ct learning trigger stats averages (at an epoch-level timescale)
  override void	Compute_EpochStats();
  // #CAT_Statistic compute epoch-level statistics, including SSE, AvgExtRew and AvgCycles
  override void	SetProjectionDefaultTypes(Projection* prjn);

  virtual void	GraphInhibMod(bool flip_sign = true, DataTable* graph_data = NULL);
  // #MENU #MENU_SEP_BEFORE #NULL_OK #NULL_TEXT_NewGraphData graph the overall inhibitory modulation curve, including sinusoidal and final -- if flip_sign is true, then sign is reversed so that graph looks like the activation profile instead of the inhibition profile

  TA_SIMPLE_BASEFUNS(LeabraNetwork);
protected:
  int	prv_learn_rule;		// previous learning rule for triggering updates
  void 	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{}
};

#include <LeabraUnitSpec_inlines>
#include <LeabraConSpec_inlines>

#endif // LeabraNetwork_h
