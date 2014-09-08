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
#include <Average>
#include <CtSRAvgVals>
#include <LeabraLayer>
#include <LeabraPrjn>
#include <LeabraThreadMgr>

// declare all other types mentioned but not required to include:
class DataTable; // 

eTypeDef_Of(LeabraNetMisc);

class E_API LeabraNetMisc : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra misc network-level parameters for Leabra
INHERITED(taOBase)
public:
  bool		dwt_norm_used; // #READ_ONLY #SHOW dwt_norm is used -- this must be done as a separate step -- LeabraConSpec will set this flag if LeabraConSpec::wt_sig.dwt_norm flag is on, and off if not -- updated in Trial_Init_Specs call
  bool          kwta_used;        // #READ_ONLY #SHOW do any layers use kwta for computing inhibition (otherwise FF_FB which is the default and is more computationally efficient)?  set automatically in Trial_Init_Specs, based on LayerSpec.inhib settings
  bool          lay_gp_inhib;     // #READ_ONLY #SHOW layer group level inhibition is active for some layer groups -- may cause some problems with asynchronous threading operation
  bool		cyc_syn_dep;	// #READ_ONLY #SHOW if true, enable synaptic depression calculations at the synapse level (also need conspecs to implement this -- this just enables computation) -- automatically set if needed in Trial_Init_Specs call
  int		syn_dep_int;	// [20] #CONDSHOW_ON_cyc_syn_dep synaptic depression interval -- how frequently to actually perform synaptic depression within a trial (uses ct_cycle variable which counts up continously through trial)

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(LeabraNetMisc);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(CtTrialTiming);

class E_API CtTrialTiming : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra timing parameters for a single stimulus input trial of ct learning algorithm
INHERITED(taOBase)
public:
  bool		use;		// whether to use these parameters to determine trial timing -- on by default for all Ct (continuous time) algorithms, and off for CHL by default
  int		minus;		// #DEF_50:200 #CONDSHOW_ON_use number of cycles to run in the minus phase with only inputs and no targets (used by CtLeabraSettle program), sets cycle_max -- can be 0
  int		plus;		// #DEF_20:200 #CONDSHOW_ON_use number of cycles to run in the plus phase with input and target activations (used by CtLeabraSettle program), sets cycle_max -- must be > 0

  int		total_cycles;	// #READ_ONLY computed total number of cycles per trial

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(CtTrialTiming);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(CtSRAvgSpec);

class E_API CtSRAvgSpec : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra how to compute the sravg value as a function of cycles 
INHERITED(taOBase)
public:
  bool		manual;		// the determination of when to compute sravg is made externally (e.g., by a program) -- sravg_vals.state flag must be set prior to calling Network::Cycle_Run() to take effect -- by default layers will copy network sravg_vals.state flag setting, so you only need to set it globally for the network (see layer-level manual_sravg flag if you want to set everything manually)
  int		start;		// #CONDSHOW_OFF_manual #DEF_30:60 number of cycles from the start of a new pattern to start computing sravg value -- avoid transitional states that are too far away from attractor state
  int		interval;	// #CONDSHOW_OFF_manual #DEF_1;5 #MIN_1 (1 for XCAL, 5 for CAL) how frequently to compute sravg -- in XCAL this is not expensive so do it every cycle, but for CAL more infrequent updating saves computational costs as sravg is expensive
  int		plus_s_st;	// #CONDSHOW_OFF_manual [10 for spiking, else plus-1, typically 19] how many cycles into the plus phase should the short time scale sravg computation start (only for TRIAL sravg computation)
  bool		plus_s_only;	// #CONDSHOW_OFF_manual plus increments short-term only, not short and medium term activations
  bool		force_con;	// #DEF_false force connection-level SRAvg computation -- only use for experimental algorithms that need this -- otherwise needlessly slows computation

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(CtSRAvgSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};


eTypeDef_Of(RelNetinSched);

class E_API RelNetinSched : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra schedule for computing relative netinput values for each projection -- this is very important data for tuning the network to ensure that each layer has the relative impact it should on other layers -- however it is expensive (only if not using NETIN_PER_PRJN, otherwise it is automatic and these options are disabled), so this schedules it to happen just enough to get the results you want
INHERITED(taOBase)
public:
  bool		on;		// #DEF_true whether to compute relative netinput at all
  int		trl_skip;	// #DEF_10 #MIN_1 #CONDSHOW_ON_on skip every this many trials for epochs where it is being computed -- typically do not need sample all the trials -- adjust this depending on how many trials are typical per epoch
  int		epc_skip;	// #DEF_10 #MIN_1 #CONDSHOW_ON_on skip every this many epochs -- typically just need to see rel_netin stats for 1st epoch and then every so often thereafter

  bool          ComputeNow(int net_epc, int net_trl)
  { if(on && (net_epc % epc_skip == 0) && (net_trl % trl_skip == 0)) return true;
    return false; }
  // should we compute relative netin now, based on network epoch and trial counters?

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(RelNetinSched);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(LeabraNetwork);

class E_API LeabraNetwork : public Network {
  // #STEM_BASE ##CAT_Leabra network that uses the Leabra algorithms and objects
INHERITED(Network)
public:

  // IMPORTANT programming note: this enum must be same as in LeabraConSpec
  enum LearnRule {
    CTLEABRA_XCAL,		// Continuous-Time Leabra temporally eXtended Contrastive Attractor Learning rule, trial-based version, which has two time scales of contrasts: short-vs-medium (svm) and medium-vs-long (mvl): (<sr>_s - <sr>_m) + (<sr>_m - <r>_l) -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales.  svm is basically error-driven learning, and mvl is BCM-style self-organizing learning.
    LEABRA_CHL,			// standard Leabra Contrastive Hebbian Learning rule with hebbian self-organizing factor: (s+r+) - (s-r-) + r+(s+ - w) -- s=sender,r=recv +=plus phase, -=minus phase, w= weight
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
    PLUS_ONLY,			// only present the plus phase (hebbian-only)
  };

  enum UnitVecVars {            // unit variables that have special vectorized storage
    AVG_S,
    AVG_M,
    AVG_L,
    THAL,
    N_VEC_VARS,
  };

  enum TmpConVars {             // temporary connection variables
    TCV1,
    TCV2,
    N_CON_VARS,
  };
  

#ifdef __MAKETA__
  UnitCallThreadMgr threads;    // #HIDDEN unit-call threading mechanism -- lthreads used instead
#endif

  LeabraThreadMgr lthreads;     // #CAT_Threads parallel threading for leabra algorithm -- handles majority of computation within threads that are kept active and ready to go
  LearnRule	learn_rule;	// The variant of Leabra learning rule to use 
  bool          ti_mode;        // turn on LeabraTI (temporal integration) processing and learning mechanisms -- if used, requires LeabraTICtxtConSpec SELF prjns in layers to perform optimized single-layer TI context activation at end of plus phase -- must have this flag on for TI to work!
  PhaseOrder	phase_order;	// [Default: MINUS_PLUS] #CAT_Counter number and order of phases to present
  bool		no_plus_test;	// #DEF_true #CAT_Counter don't run the plus phase when testing
  StateInit	sequence_init;	// #DEF_DO_NOTHING #CAT_Activation how to initialize network state at start of a sequence of trials

  Phase		phase;		// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW type of settling phase
  int		phase_no;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW phase as an ordinal number (regular phase is Phase enum)
  int		phase_max;	// #CAT_Counter maximum number of phases to run (note: this is set by Trial_Init depending on phase_order)

  int		ct_cycle;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW continuous time cycle counter: counts up from start of trial 
  float		time_inc;	// how much to increment the network time variable every cycle -- this goes monotonically up from the last weight init or manual reset

  LeabraNetMisc	net_misc;	// misc network level parameters for leabra

  int		cycle_max;	// #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL #DEF_50;60 maximum number of cycles to settle for: note for CtLeabra_X/CAL this is overridden by phase specific settings by the settle process
  int		mid_minus_cycle; // #CAT_Counter #DEF_-1:30 cycle number for computations that take place roughly mid-way through the minus phase -- used for PBWM algorithm -- effective min_cycles for minus phase will be this value + min_cycles -- set to -1 to disable
  int		min_cycles;	// #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL #DEF_0:35 minimum number of cycles to settle for
  int		min_cycles_phase2; // #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL #DEF_0;35 minimum number of cycles to settle for in second phase

  CtTrialTiming	 ct_time;	// #CAT_Learning timing parameters for ct leabra trial: Settle_Init sets the cycle_max based on these values
  CtSRAvgSpec	 ct_sravg;	// #CAT_Learning #CONDSHOW_OFF_learn_rule:LEABRA_CHL parameters controlling computation of sravg value as a function of cycles
  CtSRAvgVals	 sravg_vals;	// #NO_SAVE #CAT_Learning sender-receiver average computation values, e.g., for normalizing sravg values
  RelNetinSched	 rel_netin;	// #CAT_Learning #CONDSHOW_OFF_flags:NETIN_PER_PRJN schedule for computing relative netinput values for each projection -- this is very important data for tuning the network to ensure that each layer has the relative impact it should on other layers -- however it is expensive (only if not using NETIN_PER_PRJN, otherwise it is automatic and these options are disabled), so this schedules it to happen just enough to get the results you want

  float		minus_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cycles to settle in the minus phase -- this is the typical settling time statistic to record
  Average	avg_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average settling cycles in the minus phase (computed over previous epoch)

  String	minus_output_name; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW output_name in the minus phase -- for recording in logs as network's response (output_name in plus phase is clamped target value)

  float		send_pct;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic proportion of sending units that actually sent activations on this cycle
  int		send_pct_n;	// #NO_SAVE #READ_ONLY #CAT_Statistic number of units sending activation this cycle
  int		send_pct_tot;	// #NO_SAVE #READ_ONLY #CAT_Statistic total number of units that could send activation this cycle
  Average	avg_send_pct;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average proportion of units sending activation over an epoch

  float		maxda_stopcrit;	// #DEF_0.005;-1 #CAT_Statistic stopping criterion for max da
  float		maxda;		// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW maximum change in activation (delta-activation) over network; used in stopping settling

  float		trg_max_act_stopcrit;	// #CAT_Statistic stopping criterion for target-layer maximum activation (can be used for stopping settling)
  float		trg_max_act;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic target-layer maximum activation (can be used for stopping settling)

  float		ext_rew;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW external reward value (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network -- equals PVe value in PVLV framework
  bool		ext_rew_avail; 	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic actual external reward value is available (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network -- if false then no feedback was provided on this trial
  float		norew_val; 	// #GUI_READ_ONLY #CAT_Statistic no-reward value (serves as a baseline against which ext_rew can be compared against -- if greater, then positive reward, if less, then negative reward -- typically 0.5 but can vary
  Average	avg_ext_rew;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average external reward value (computed over previous epoch)
  float		pvlv_pvi;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV primary reward prediction value PVi for the current trial -- updated on a cycle-by-cycle basis -- used for noise modulation among perhaps other things
  float		pvlv_pvr;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV primary reward availability prediction value PVr for the current trial -- updated on a cycle-by-cycle basis
  float		pvlv_lve;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV learned reward prediction value LVe (excitatory, rapidly adapting) for the current trial -- updated on a cycle-by-cycle basis -- used for noise modulation among perhaps other things
  float		pvlv_lvi;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV learned reward prediction value LVi (inhibitory, slowly adapting) for the current trial -- updated on a cycle-by-cycle basis -- used for noise modulation among perhaps other things
  float		pvlv_nv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV novelty value for the current trial -- updated on a cycle-by-cycle basis
  float		pvlv_dav;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV phasic dopamine value for the current trial -- updated on a cycle-by-cycle basis
  float		pvlv_tonic_da;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV tonic dopamine level for the current trial -- updated on a cycle-by-cycle basis
  float		pvlv_sev;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV serotonin (5HT) value for the current trial -- updated on a cycle-by-cycle basis
  bool		pv_detected;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV detected a situation where primary reward value is expected to be available, based on learned encoding of similar such situations in the past -- computed by the PVrLayerSpec continuously in the minus phase

  bool		off_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly off (should have been on but were actually off) -- either 1 or both of off_errs and on_errs must be set
  bool		on_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly on (should have been off but were actually on) -- either 1 or both of off_errs and on_errs must be set
  bool          unlearnable_trial; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning this trial is flagged as being unlearnable -- blocks Compute_dWt and error stats from being computed -- particularly relevant for TI, where the prior context provides no basis for prediction (see also cos_err_lrn_thr) -- flag is automatically reset at start of trial -- must be actively set every trial

  float		norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW normalized binary (Hamming) error on this trial: number of units that were incorrectly activated or incorrectly inactivated (see off_errs to exclude latter)
  Average	avg_norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average normalized binary error value (computed over previous epoch)

  float		cos_err_lrn_thr; // #CAT_Learning learning threshold for cos_err -- if cos err is below this value, then no learning occurs -- prevents learning when things are too far away from expectations -- esp useful for leabra ti (see also unlearnable_trial flag)
  float		cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cosine (normalized dot product) error on this trial -- cosine between act_m and act_p target values
  float		cos_err_prv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) error on this trial, for activations on previous trial (p_act_p) -- computed automatically during ti_mode
  float		cos_err_vs_prv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cos_err - cos_err_prv -- how much better is cosine error on this trial relative to just saying the same thing as was output last time -- for ti_mode
  Average	avg_cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error (computed over previous epoch)
  Average	avg_cos_err_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on prv (see cos_err_prv) (computed over previous epoch)
  Average	avg_cos_err_vs_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on vs prv (see cos_err_vs_prv) (computed over previous epoch)

  bool		cos_diff_on;      // #CAT_Learning manually enable Compute_CosDiff call at end of plus phase (if conspec cos_diff_lrate or X_COS_DIFF is set, then cos_diff will automatically be computed regardless)
  bool		cos_diff_auto;    // #CAT_Learning #READ_ONLY enable Compute_CosDiff call at end of plus phase -- LeabraConSpec will auto set this flag if LeabraConSpec::cos_diff_lrate or X_COS_DIFF is set, and off if not -- updated in Trial_Init_Specs call
  float		cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cosine (normalized dot product) difference between act_p and act_m activations on this trial -- excludes input layers which are represented in the cos_err measure
  Average	avg_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) diff (computed over previous epoch)

  float		avg_act_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW average act_diff (act_p - act_m) -- this is an important statistic to track overall 'main effect' differences across phases -- excludes input layers
  Average	avg_avg_act_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average avg_act_diff (computed over previous epoch)

  float		trial_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cosine (normalized dot product) activation difference across trials between act_p and p_act_p activations on this trial -- excludes input layers which are represented in the cos_err measure
  Average	avg_trial_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) trial diff (computed over previous epoch)

  bool		inhib_cons_used; // #NO_SAVE #READ_ONLY #CAT_Threads inhibitory connections are being used in this network -- detected during buildunits_threads to determine how netinput is computed -- sets NETIN_PER_PRJN flag
  bool		init_netins_cycle_stat; // #NO_SAVE #HIDDEN #CAT_Activation flag to trigger the call of Init_Netins at the end of the Compute_CycleStats function -- this is needed for specialized cases where projection scaling parameters have changed, and thus the net inputs are all out of whack and need to be recomputed -- flag is set to false at start of Compute_CycleStats and checked at end, so layers just need to set it

  int_Array     active_layer_idx;
  // #NO_SAVE #HIDDEN #CAT_Activation leaf indicies of the active (non-lesioned, non-hard clamped input) layers in the network
  float_Matrix  unit_vec_vars;
  // #NO_SAVE #HIDDEN #CAT_Activation vectorized versions of unit variables -- 2d matrix outer dim is N_VEC_VARS, and inner is flat_units.size


  inline float*  UnVecVar(UnitVecVars var)
  { return unit_vec_vars.el + var * units_flat.size; }
  // #IGNORE get start of given unit vector variable array

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  void	Init_Acts() override;
  void	Init_Counters() override;
  void	Init_Stats() override;
  void	Init_Sequence() override;
  void  Init_Weights() override;

  virtual void  Init_Netins();
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)

  virtual void	DecayState(float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  virtual void	SetLearnRule_ConSpecs(BaseSpec_Group* spgp);
  // #IGNORE set the current learning rule into all conspecs in given spec group (recursive)
  virtual void	SetLearnRule();
  // #CAT_ObjectMgmt set the current learning rule into the conspecs on this network (done by network UAE only when rule changed)

  virtual void	CheckInhibCons();
  void	BuildUnits_Threads() override;
  void  BuildUnits_Threads_send_netin_tmp() override;
  bool  RecvOwnsCons() override { return false; }
  void  CountCons() override;

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void 	Trial_Init();
  // #CAT_TrialInit initialize at start of trial (init specs, set phase_max, Decay state)
    virtual void Trial_Init_Phases();
    // #CAT_TrialInit init phase_max and current phase based on phase_order -- network-only
    virtual void Trial_Init_Specs();
    // #CAT_TrialInit initialize specs and specs update network flags

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
    // #CAT_SettleInit move target and external input values to comparison

  virtual void  NewInputData_Init();
  // #CAT_SettleInit perform initialization stuff needed to update external input data signals so they actually show up as activations in the network: Settle_Init_Layer, Settle_Init_TrgFlags, Compute_HardClamp

  ////////////////////////////////////////////////////////////////
  //	Cycle_Run

  virtual void	Cycle_Run();
  // #CAT_Cycle compute one cycle of updating: netinput, inhibition, activations

  ///////////////////////////////////////////////////////
  //	Cycle Stage 1: netinput

  void	Send_Netin() override;
  // #CAT_Cycle compute netinputs (sender-delta based -- only send when sender activations change) -- new values go in net_delta or g_i_delta (summed up from tmp array for threaded case)
  virtual void Compute_NetinInteg();
  // #CAT_Cycle Stage 1.2 integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  virtual void	Compute_Inhib();
  // #CAT_Cycle compute inhibitory conductances via inhib functions (FFFB, kWTA) -- calls Compute_NetinStats and LayInhibToGps to coordinate group-level inhibition sharing
    virtual void Compute_Inhib_LayGp();
    // #CAT_Cycle compute inhibition across layer groups -- if layer spec lay_gp_inhib flag is on anywhere

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  void	Compute_Act() override;
  // #CAT_Cycle compute activations

  virtual void 	Compute_SRAvg_State();
  // #CAT_Learning compute state flag setting for sending-receiving activation coproduct averages (CtLeabra_X/CAL) -- called at the Cycle_Run level -- just updates the sravg_vals.state flag for network and layers -- called at start of cycle
  virtual bool 	Compute_SRAvg_Cons_Test();
  // #CAT_Learning test if sravg cons level should be run -- not used for XCAL typically -- just for CtLeabra_CAL -- not for testing
  virtual void 	Compute_SRAvg_Cons();
  // #CAT_Learning compute sending-receiving activation coproduct averages for the connections -- not used for XCAL typically -- just for CtLeabra_CAL

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  virtual void	Compute_CycleStats_Pre();
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, MaxDa, OutputName, etc -- network-level pre-step
  virtual void	Compute_CycleStats_Layer();
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, MaxDa -- layer level computation
  virtual void	Compute_CycleStats_Post();
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, MaxDa, OutputName, etc -- network-level post-step

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

  ///////////////////////////////////////////////////////////////////////
  //	LeabraTI Special code

  virtual void TI_CtxtUpdate();
  // #CAT_TI called if ti_mode is true -- updates context activation at end of plus phase (called from PostSettle())
    virtual void TI_Compute_Deep5bAct();
    // #CAT_TI compute deep 5b activations from thal and act_eq
    virtual void TI_Send_Netins();
    // #CAT_TI send deep5b and context netinputs
    virtual void TI_Send_Deep5bNetin();
    // #CAT_TI send deep 5b netinput
    virtual void TI_Send_CtxtNetin();
    // #CAT_TI send context netinput
    virtual void TI_Compute_CtxtAct();
    // #CAT_TI compute context activations from context netinput

  virtual void TI_ClearContext();
  // #CAT_TI clear the TI context state from all units in the network -- can be useful to do at clear discontinuities of experience

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

  virtual void	Compute_dWt_Layer_pre();
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  virtual void  Compute_dWt_vecvars();
  // #CAT_Learning copy over the vectorized variables for learning

  void	Compute_dWt() override;
  // #CAT_Learning compute weight change after first plus phase has been encountered
  virtual void	Compute_dWt_Norm();
  // #CAT_Learning compute normalization of weight changes -- must be done as a second pass after initial weight changes

  void Compute_Weights_impl() override;

  void	Compute_StableWeights() { }
  // #CAT_OBSOLETE this function is now obsolete and should be removed from your code -- it does nothing anymore, as this feature has been removed in favor of the new fast_wts functionality

  ///////////////////////////////////////////////////////////////////////
  //	Stats

  virtual void	Set_ExtRew(bool avail, float ext_rew_val);
  // #CAT_Statistic set ext_rew_avail and ext_rew value -- for script access to these values
  virtual void	Compute_ExtRew();
  // #CAT_Statistic compute external reward information: Must be called in plus phase (phase_no == 1)
  virtual void	Compute_NormErr();
  // #CAT_Statistic compute normalized binary error between act_m and targ unit values: called in TrialStats -- per unit: if (net->on_errs && act_m > .5 && targ < .5) return 1; if (net->off_errs && act_m < .5 && targ > .5) return 1; else return 0; normalization is per layer based on k value: total possible err for both on and off errs is 2 * k (on or off alone is just k)
  virtual float	Compute_CosErr();
  // #CAT_Statistic compute cosine (normalized dot product) error between act_m and targ unit values
  virtual float	Compute_CosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) phase difference between act_m and act_p unit values -- must be called after PostSettle (SettleFinal) for plus phase to get the act_p values
  virtual float	Compute_AvgActDiff();
  // #CAT_Statistic compute average act_diff (act_p - act_m) -- must be called after PostSettle (SettleFinal) for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
  virtual float	Compute_TrialCosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) trial activation difference between p_act_p and act_p unit values -- must be called after PostSettle (SettleFinal) for plus phase to get the act_p values
  virtual void	Compute_MinusCycles();
  // #CAT_Statistic compute minus-phase cycles (and increment epoch sums) -- at the end of the minus phase (of course)
  void	Compute_TrialStats() override;
  // #CAT_Statistic compute trial-level statistics, including SSE and minus cycles -- to be called at end of minus phase -- use Compute_TrialStats_Test() to determine when -- HOWEVER: it is much better to call Compute_PhaseStats(), which calls Compute_MinusStats() and Compute_PlustStats() separately at end of each phase (respectively) to get the appropriate stats at each point
  virtual bool	Compute_TrialStats_Test();
  // #CAT_Statistic determine whether it is time to run trial stats -- typically the minus phase but it depends on network phase_order settings etc

  virtual void  Compute_PhaseStats();
  // #CAT_Statistic compute MinusStats at the end of the minus phase, and PlusStats at the end of the plus phase -- this is preferred over the previous implementation of calling TrialStats only at the end of the minus phase, which required targets to be present in the minus phase, which is not always the case
  virtual void  Compute_MinusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the minus phase: MinusCycles and minus_output_name -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase
  virtual void  Compute_PlusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the plus phase: all the error stats: SSE, PRerr, NormErr, CosErr -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase

  virtual void	Compute_AbsRelNetin();
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into layers in network -- this should NOT be called from programs (although previously it was) -- it is automatically called in Trial_Final now, and is subjected to settings of rel_netin if NETIN_PER_PRJN flag is not set
  virtual void	Compute_AbsRelNetin_impl();
  // #CAT_Statistic actually compute the absolute layer-level and relative netinput from different projections into layers in network (no tests)
  virtual void	Compute_AvgAbsRelNetin();
  // #CAT_Statistic compute time-average absolute layer-level and relative netinput from different projections into layers in network (e.g. over epoch timescale)
  virtual void	Compute_AdaptRelNetin() { };
  // #CAT_Learning #OBSOLETE NOTE: this is now obsolete and does not do anything -- please remove from your program!
  virtual void	Compute_AdaptAbsNetin() { };
  // #CAT_Learning #OBSOLETE NOTE: this is now obsolete and does not do anything -- please remove from your program!

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
  virtual void	Compute_AvgCosDiff();
  // #CAT_Statistic compute average cos_diff (at an epoch-level timescale)
  virtual void	Compute_AvgTrialCosDiff();
  // #CAT_Statistic compute average trial_cos_diff (at an epoch-level timescale)
  virtual void	Compute_AvgAvgActDiff();
  // #CAT_Statistic compute average avg_act_diff (at an epoch-level timescale)
  void	Compute_EpochStats() override;
  // #CAT_Statistic compute epoch-level statistics, including SSE, AvgExtRew and AvgCycles
  void	SetProjectionDefaultTypes(Projection* prjn) override;

  void  BuildNullUnit() override;

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
