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
#include <LeabraUnit>
#include <LeabraLayer>
#include <LeabraPrjn>
#include <int_Array>

// declare all other types mentioned but not required to include:
class DataTable; // 

#ifdef CUDA_COMPILE
class LeabraConSpecCuda; // #IGNORE
#endif

eTypeDef_Of(LeabraNetTiming);

class E_API LeabraNetTiming : public NetTiming {
  // timers for Leabra network functions
INHERITED(NetTiming)
public:
  TimeUsedHR   netin_integ;     // Compute_NetinInteg integrate net inputs
  TimeUsedHR   netin_stats;     // Compute_NetinStats netin stats
  TimeUsedHR   inhib;           // Compute_Inhib inhibition
  TimeUsedHR   act_post;        // Compute_Act_Post post integration
  TimeUsedHR   cycstats;        // Compute_CycleStats cycle statistics

  TA_SIMPLE_BASEFUNS(LeabraNetTiming);
private:
  void	Initialize()    { };
  void 	Destroy()	{ };
};


eTypeDef_Of(LeabraCudaSpec);

class E_API LeabraCudaSpec : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra parameters for NVIDA CUDA GPU implementation -- only applicable for CUDA_COMPILE binaries
INHERITED(taOBase)
public:
  bool          on;             // #READ_ONLY #SHOW #NO_SAVE is CUDA active?  this is true when running in an executable compiled with CUDA_COMPILE defined -- it will automatically use the cuda GPU for all connection-level computations
  bool          get_wts;        // #DEF_false #NO_SAVE [this setting is NOT saved -- must set explicitly] get the dwt and wt values back from the GPU after they are updated there -- keeps the host and device sync'd for debugging and display purposes only -- GREATLY slows down processing
  int           min_threads;    // #DEF_32 #MIN_32 minuimum number of CUDA threads to allocate per block (sending group of connections) -- must be a multiple of 32 (i.e., min of 32) -- actual size will be determined at run-time as function of max number of connections per connection group / cons_per_thread -- to specify an exact number of threads, just set min and max_threads to the same number
  int           max_threads;    // #DEF_1024 #MAX_1024 maximum number of CUDA threads to allocate per block (sending group of connections) -- actual size will be determined at run-time as function of max number of connections per connection group / cons_per_thread -- for modern cards (compute capability 2.0 or higher) the max is 1024, but in general you might need to experiment to find the best performing number for your card and network, and interactionwith cons_per_thread -- to specify an exact number of threads, just set min and max_threads to the same number
  int           cons_per_thread; // #DEF_1:8 when computing number of threads to use, divide max number of connections per unit by this number, and then round to nearest multiple of 32, subject to the min and max_threads constraints
  bool          timers_on;      // Accumulate timing information for each step of processing -- for debugging / optimizing threading

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(LeabraCudaSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(LeabraTimes);

class E_API LeabraTimes : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra phase parameters for what phases are run and how long the 
INHERITED(taOBase)
public:
  int		quarter;	// #DEF_25 number of cycles to run per each quarter of a trial -- typically, a trial is one alpha cycle of 100 msec (10 Hz), and we run at 1 cycle = 1 msec, so that is 25 cycles per quarter, which gives the ubiquitous gamma frequency power of 40 Hz -- a traditional minus phase takes the first 3 quarters, and the last quarter is the plus phase -- use CycleRunMax_Minus and CycleRunMax_Plus to get proper minus and plus phase cycles values to use in programs, taking into account lthreads.quarter setting
  bool          cycle_qtr;      // #DEF_true #NO_SAVE one CycleRun runs for a full quarter number of actual cycles -- this greatly speeds up processing by reducing threading overhead, but prevents e.g., interactive viewing at the individual cycle level -- this is not saved -- have to re-engage it when needed, to prevent unintentionally slowing everything down
  float		time_inc;	// #DEF_0.001 in units of seconds -- how much to increment the network time variable every cycle -- this goes monotonically up from the last weight init or manual reset -- default is .001 which means one cycle = 1 msec -- MUST also coordinate this with LeabraUnitSpec.dt.integ for most accurate time constants -- also affects rate-code computed spiking intervals in unit spec

  int		minus;	        // #READ_ONLY computed total number of cycles per minus phase = 3 * quarter
  int		plus;	        // #READ_ONLY computed total number of cycles per plus phase = quarter
  int		total_cycles;	// #READ_ONLY computed total number of cycles per trial

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(LeabraTimes);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(LeabraNetStats);

class E_API LeabraNetStats : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra leabra network-level statistics parameters
INHERITED(taOBase)
public:
  float		trg_max_act_crit; // #CAT_Statistic criterion for target-layer maximum activation (trg_max_act) -- rt_cycles is recorded when trg_max_act first exceeds this criterion
  bool		off_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly off (should have been on but were actually off) -- either 1 or both of off_errs and on_errs must be set
  bool		on_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly on (should have been off but were actually on) -- either 1 or both of off_errs and on_errs must be set
  float		cos_err_lrn_thr; // #CAT_Learning learning threshold for cos_err -- if cos err is below this value, then no learning occurs -- prevents learning when things are too far away from expectations -- esp useful for leabra ti (see also unlearnable_trial flag)

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(LeabraNetStats);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(LeabraNetMisc);

class E_API LeabraNetMisc : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra misc network-level parameters for Leabra
INHERITED(taOBase)
public:
  bool          spike;         // #READ_ONLY #SHOW using discrete spiking -- all units must be either rate code or spiking, to optimize the computation -- updated in Trial_Init_Specs call
  bool          ti;            // #READ_ONLY #SHOW LeabraTI (temporal integration) processing and learning mechanisms are engaged, because LeabraTICtxtConSpec SELF prjns are present in layers to perform optimized single-layer TI context activation at end of plus phase -- updated in Trial_Init_Specs call
  bool          deep5b_cons; // #READ_ONLY #SHOW Deep5bConSpec prjns are present in layers to send deep5b activations instead of superficial activations -- updated in Trial_Init_Specs call
  bool		bias_learn;     // #READ_ONLY #SHOW do any of the bias connections have learning enabled?  if true, then an extra unit-level computational step is required -- bias learning is now OFF by default, as it has no obvious benefits in large models, but may be useful for smaller networks
  bool          trial_decay;   // #READ_ONLY #SHOW at least one layer spec has a non-zero level of trial decay -- if all layers have 0 trial decay, then the net input does not need to be reset between trials, yielding significantly faster performance
  bool          diff_scale_p;   // #READ_ONLY #SHOW at least one connection spec uses diff_act_p setting to drive a differential wt scaling in the plus phase relative to the minus phase -- this requires initializing the net inputs between these phases
  bool		dwt_norm;       // #READ_ONLY #SHOW dwt_norm is being used -- this must be done as a separate step -- LeabraConSpec will set this flag if LeabraConSpec::wt_sig.dwt_norm flag is on, and off if not -- updated in Trial_Init_Specs call
  bool          lay_gp_inhib;     // #READ_ONLY #SHOW layer group level inhibition is active for some layer groups -- may cause some problems with asynchronous threading operation -- updated in Trial_Init_Specs call
  bool		inhib_cons;     // #READ_ONLY #SHOW inhibitory connections are being used in this network -- detected during buildunits_threads to determine how netinput is computed -- sets NETIN_PER_PRJN flag
  bool          lrate_updtd;
  // #IGNORE flag used to determine when the learning rate was updated -- e.g., needed for CUDA to update parameters 

  String       GetTypeDecoKey() const override { return "Network"; }

  TA_SIMPLE_BASEFUNS(LeabraNetMisc);
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
  enum Phase {
    MINUS_PHASE = 0,		// minus phase
    PLUS_PHASE = 1,		// plus phase
  };

  // IMPORTANT: coordinate this with LeabraConSpec_cuda.h!
  enum UnitVecVars {            // unit variables that have special vectorized storage, encoded at end of trial, in time for compute_dwt function
    AVG_S,
    AVG_M,
    AVG_L,
    THAL,
    ACT_Q0,
    COS_DIFF_LMIX,              // from recv layer
    N_VEC_VARS,
  };
  // ACT_M, // note: could add these to unit vec vars if needed
  // ACT_P,

  enum AvgMaxVars { // variables that we collect average and max values of -- thread optimized -- coordintate with LeabraInhib.h
    AM_NET,
    AM_ACT,
    AM_ACT_EQ,
    AM_UN_G_I,
    N_AM_VARS,
  };

  LeabraCudaSpec  cuda;         // #CAT_CUDA parameters for NVIDA CUDA GPU implementation -- only applicable for CUDA_COMPILE binaries
  LeabraTimes     times;        // #CAT_Learning time parameters
  LeabraNetStats  lstats;       // #CAT_Statistic leabra network-level statistics parameters
  LeabraNetMisc	  net_misc;	// misc network level parameters for leabra -- these determine various algorithm variants, typically auto-detected based on the network configuration in Trial_Init_Specs
  RelNetinSched	  rel_netin;	// #CAT_Learning #CONDSHOW_OFF_flags:NETIN_PER_PRJN schedule for computing relative netinput values for each projection -- this is very important data for tuning the network to ensure that each layer has the relative impact it should on other layers -- however it is expensive (only if not using NETIN_PER_PRJN, otherwise it is automatic and these options are disabled), so this schedules it to happen just enough to get the results you want
  bool          unlearnable_trial; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning this trial is flagged as being unlearnable -- blocks Compute_dWt and error stats from being computed -- particularly relevant for TI, where the prior context provides no basis for prediction (see also cos_err_lrn_thr) -- flag is automatically reset at start of trial -- must be actively set every trial

  ////////////  everything below here should be a counter or statistic -- no params!

#ifdef __MAKETA__
  int           cycle;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW cycle counter: number of iterations of activation updating (settling) on the current alpha-cycle (100 msec / 10 Hz) trial -- this counts time sequentially through the entire trial, typically from 0 to 99 cycles (equivalent to the old ct_cycle, not cycle within a phase as cycle used to be prior to version 7.1.0)
#endif

  int		quarter;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW current gamma-frequency (25 msec / 40 Hz) quarter of alpha-cycle (100 msec / 10 Hz) trial being processed
  Phase		phase;		// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW type of settling phase (MINUS or PLUS) -- minus is first 3 quarters, plus is last quarter
  int           tot_cycle;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW total cycle count -- this increments from last Init_Weights and just keeps going up (unless otherwise reset) -- used for tracking things like spiking times continuously across time

  float		rt_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cycles to generate an output response (reaction time = RT) -- typically lstats.trg_max_act_crit is used as an activation criterion over the trg_max_act value recorded from target output layer(s) to determine this
  Average	avg_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average settling cycles in the minus phase (computed over previous epoch)
  String	minus_output_name; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW output_name in the minus phase -- for recording in logs as network's response (output_name in plus phase is clamped target value)
  float		trg_max_act;	// #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic maximum activation of any unit in a target layer -- compared against lstats.trg_max_act_crit to compute rt_cycles

  float		send_pct;	// #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic proportion of sending units that actually sent activations on this cycle -- only available for non-threading case
  int		send_pct_n;	// #NO_SAVE #READ_ONLY #CAT_Statistic number of units sending activation this cycle
  int		send_pct_tot;	// #NO_SAVE #READ_ONLY #CAT_Statistic total number of units that could send activation this cycle
  Average	avg_send_pct;	// #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic #DMEM_AGG_SUM average proportion of units sending activation over an epoch -- only available for non-threading case

  float		ext_rew;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW external reward value (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network
  bool		ext_rew_avail; 	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic actual external reward value is available (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network -- if false then no feedback was provided on this trial
  Average	avg_ext_rew;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average external reward value (computed over previous epoch)

  float		norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW normalized binary (Hamming) error on this trial: number of units that were incorrectly activated or incorrectly inactivated (see lstats.off_errs, on_errs to exclude each component separately)
  Average	avg_norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average normalized binary error value (computed over previous epoch)

  float		cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cosine (normalized dot product) error on this trial, comparing targ vs. act_m
  float		cos_err_prv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_net_misc.ti #CAT_Statistic cosine (normalized dot product) error on this trial, comparing targ on this trial against activations on previous trial (act_q0) -- computed automatically during TI
  float		cos_err_vs_prv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_net_misc.ti #CAT_Statistic cos_err - cos_err_prv -- how much better is cosine error on this trial relative to just saying the same thing as was output last time -- for TI
  Average	avg_cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error (computed over previous epoch)
  Average	avg_cos_err_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_net_misc.ti #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on prv (see cos_err_prv) (computed over previous epoch)
  Average	avg_cos_err_vs_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_net_misc.ti #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on vs prv (see cos_err_vs_prv) (computed over previous epoch)

  float		cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cosine (normalized dot product) difference between act_p and act_m activations on this trial -- excludes input layers which are represented in the cos_err measure
  Average	avg_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) diff (computed over previous epoch)

  float		avg_act_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW average act_diff (act_p - act_m) -- this is an important statistic to track overall 'main effect' differences across phases -- excludes input layers
  Average	avg_avg_act_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average avg_act_diff (computed over previous epoch)

  float		trial_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cosine (normalized dot product) activation difference across trials between act_q4 and act_q0 activations on this trial -- excludes input layers which are represented in the cos_err measure
  Average	avg_trial_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) trial diff (computed over previous epoch)

  bool		init_netins_cycle_stat; // #NO_SAVE #HIDDEN #CAT_Activation flag to trigger the call of Init_Netins at the end of the Compute_CycleStats function -- this is needed for specialized cases where projection scaling parameters have changed, and thus the net inputs are all out of whack and need to be recomputed -- flag is set to false at start of Compute_CycleStats and checked at end, so layers just need to set it - todo: can we eliminate this please??

  float**       unit_vec_vars;
  // #IGNORE vectorized versions of unit variables -- 2d matrix outer dim is N_VEC_VARS, and inner is flat_units.size -- note that mem access is more efficient if vars are inner dimension, but vectorization load operator only operates on contiguous memory..  can try it both ways and see..
  float**       thrs_send_d5bnet_tmp;
  // #IGNORE #CAT_Threads temporary storage for threaded sender-based deep5b netinput computation -- float*[threads] array of float[n_units]
  char**        thrs_lay_avg_max_vals;
  // #IGNORE AvgMaxValsRaw data for layers, by thread
  char**        thrs_ungp_avg_max_vals;
  // #IGNORE AvgMaxValsRaw data for unit groups, by thread
  float         tmp_arg1;        // #IGNORE for passing args through threaded call

#ifdef CUDA_COMPILE
  LeabraConSpecCuda* cudai;      // #IGNORE internal structure for cuda specific code
  RunWaitTime        cuda_send_netin_time;  // #IGNORE
  RunWaitTime        cuda_compute_dwt_time;  // #IGNORE
  RunWaitTime        cuda_compute_wt_time;  // #IGNORE
#endif

  inline float*  UnVecVar(int thr_no, UnitVecVars var)
  { return unit_vec_vars[thr_no] + var * n_units_built; }
  // #IGNORE get start of given unit vector variable array

  inline float* ThrSendD5bNetTmp(int thr_no) const 
  { return thrs_send_d5bnet_tmp[thr_no]; }
  // #IGNORE temporary sending deep5b netinput memory for given thread 

  inline AvgMaxValsRaw* ThrLayAvgMax(int thr_no, int lay_idx, AvgMaxVars var) 
  { return (AvgMaxValsRaw*)(thrs_lay_avg_max_vals[thr_no] +
                            (n_layers_built * var + lay_idx) * sizeof(AvgMaxValsRaw)); }
  // #IGNORE get AvgMax data for given thread, layer, and variable

  inline AvgMaxValsRaw* ThrUnGpAvgMax(int thr_no, int ungp_idx, AvgMaxVars var) 
  { return (AvgMaxValsRaw*)(thrs_ungp_avg_max_vals[thr_no] +
                            (n_ungps_built * var + ungp_idx) * sizeof(AvgMaxValsRaw)); }
  // #IGNORE get AvgMax data for given thread, unit group, and variable


  ///////////////////////////////////////////////////////////////////////
  //    Build functionality

  bool  RecvOwnsCons() override { return false; }

  virtual void	CheckInhibCons();
  void	Build() override;
  void  AllocSendNetinTmp() override;
  void  InitSendNetinTmp_Thr(int thr_no) override;
  void  BuildNullUnit() override;
  void  FreeUnitConGpThreadMem() override;
  virtual void BuildLeabraThreadMem();
  // #IGNORE
  virtual void InitLeabraThreadMem_Thr(int thr_no);
  // #IGNORE
  

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  void	Init_Acts() override;
  void	Init_Counters() override;
  void	Init_Stats() override;

  virtual void  Init_Netins();
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)
    virtual void  Init_Netins_Thr(int thr_no);
    // #IGNORE initialize netinput computation variables (delta-based requires several intermediate variables)

  virtual void	DecayState(float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)
    virtual void DecayState_Thr(int thr_no);
    // #IGNORE decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void 	Trial_Init();
  // #CAT_TrialInit initialize at start of trial (init specs, Decay state)
    virtual void Trial_Init_Counters();
    // #CAT_TrialInit init counters -- network only
    virtual void Trial_Init_Specs();
    // #CAT_TrialInit initialize specs and specs update network flags

    virtual void Trial_Init_Unit();
    // #CAT_TrialInit trial unit-level initialization functions: Trial_Init_SRAvg, DecayState, NoiseInit
    virtual void Trial_Init_Unit_Thr(int thr_no);
    // #IGNORE trial unit-level initialization functions: Trial_Init_SRAvg, DecayState, NoiseInit
    virtual void Trial_Init_Layer();
    // #CAT_TrialInit layer-level trial init (used in base code to init layer-level sravg, can be overloaded)

  ///////////////////////////////////////////////////////////////////////
  //	QuarterInit -- at start of a given quarter trial of processing

  virtual void  Quarter_Init();
  // #CAT_QuarterInit initialize network for quarter-level processing (hard clamp, netscale)
    virtual void Quarter_Init_Counters();
    // #CAT_QuarterInit initialize counters for upcoming quarter -- network only
    virtual void Quarter_Init_Unit();
    // #CAT_QuarterInit quarter unit-level initialization functions: Init_TargFlags, NetinScale
      virtual void Quarter_Init_Unit_Thr(int thr_no);
      //IGNORE quarter unit-level initialization functions: Init_TargFlags, NetinScale
    virtual void Quarter_Init_Layer();
    // #CAT_QuarterInit quarter layer-level initialization hook -- default calls TargFlags_Layer, and can be used for hook for other guys
      virtual void Quarter_Init_TargFlags();
      // #CAT_QuarterInit initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
      virtual void Quarter_Init_TargFlags_Thr(int thr_no);
      // #IGNORE initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
      virtual void Compute_NetinScale();
      // #CAT_QuarterInit compute netinput scaling values by projection -- not called by default -- direct to unit-level function
      virtual void Compute_NetinScale_Thr(int thr_no);
      // #IGNORE compute netinput scaling values by projection -- not called by default -- direct to unit-level function
      virtual void Compute_NetinScale_Senders();
      // #CAT_QuarterInit compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the Quarter_Init_Unit stage after all the recv ones are computed
      virtual void Compute_NetinScale_Senders_Thr(int thr_no);
      // #IGNORE compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the

    virtual void Compute_HardClamp();
    // #CAT_QuarterInit compute hard clamping from external inputs
      virtual void Compute_HardClamp_Thr(int thr_no);
      // #IGNORE compute hard clamping from external inputs
      virtual void Compute_HardClamp_Layer();
      // #IGNORE compute hard clamping from external inputs

    virtual void ExtToComp();
    // #CAT_QuarterInit move external input values to comparison values (not currently used)
      virtual void ExtToComp_Thr(int thr_no);
      // #IGNORE
    virtual void TargExtToComp();
    // #CAT_QuarterInit move target and external input values to comparison
      virtual void TargExtToComp_Thr(int thr_no);
      // #IGNORE

  virtual void  NewInputData_Init();
  // #CAT_QuarterInit perform initialization stuff needed to update external input data signals so they actually show up as activations in the network: Quarter_Init_Layer, Quarter_Init_TrgFlags, Compute_HardClamp

  ////////////////////////////////////////////////////////////////
  //	Cycle_Run
  
  inline  int   CycleRunMax()
  { if(times.cycle_qtr) return 1; return times.quarter; }
  // #CAT_Quarter max loop counter for running cycles in a gamma quarter of processing, taking into account the fact that threading can run multiple cycles per Cycle_Run call if quarter flag is set

  virtual void	Cycle_Run();
  // #CAT_Cycle compute cycle(s) of updating: netinput, inhibition, activations -- multiple cycles can be run depending on lthreads.n_cycles setting and whether multiple threads are actually being used -- see lthreads.n_threads_act
    virtual void Cycle_Run_Thr(int thr_no);
    // #IGNORE compute cycle(s) of updating: netinput, inhibition, activations -- multiple cycles can be run depending on lthreads.n_cycles setting and whether multiple 

  virtual void  Cycle_IncrCounters();
  // #CAT_Cycle increment the cycle-level counters -- called internally during Cycle_Run()

  ///////////////////////////////////////////////////////
  //	Cycle Stage 1: netinput

  void	Send_Netin_Thr(int thr_no) override;
  // #CAT_Cycle compute netinputs -- sender-delta based -- only send when sender activations change -- sends into tmp array that is then integrated into net_raw, gi_raw
  virtual void Compute_NetinInteg_Thr(int thr_no);
  // #CAT_Cycle integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)
  virtual void Compute_NetinStats_Thr(int thr_no);
  // #IGNORE compute layer and unit-group level stats on net input levels -- needed for inhibition
  virtual void Compute_NetinStats_Post();
  // #IGNORE compute layer and unit-group level stats on net input levels -- needed for inhibition

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  virtual void	Compute_Inhib_Thr(int thr_no);
  // #CAT_Cycle compute inhibitory conductances via inhib functions (FFFB, kWTA) -- calls Compute_NetinStats and LayInhibToGps to coordinate group-level inhibition sharing
    virtual void Compute_Inhib_LayGp();
    // #CAT_Cycle compute inhibition across layer groups -- if layer spec lay_gp_inhib flag is on anywhere

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  void	Compute_Act_Thr(int thr_no) override;
  // #CAT_Cycle compute activations
  virtual void	Compute_Act_Rate_Thr(int thr_no);
  // #CAT_Cycle rate coded activations
  virtual void	Compute_Act_Spike_Thr(int thr_no);
  // #CAT_Cycle spiking activations

  virtual void	Compute_Act_Post_Thr(int thr_no);
  // #CAT_Cycle post processing after activations have been computed -- special algorithm code takes advantage of this stage, and running average activations (SRAvg) also computed

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  virtual void	Compute_CycleStats_Pre();
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, OutputName, etc -- network-level pre-step
  virtual void	Compute_CycleStats_Thr(int thr_no);
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax -- fast layer level computation
  virtual void	Compute_ActEqStats_Thr(int thr_no);
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax -- fast layer level computation
  virtual void	Compute_CycleStats_Post();
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, OutputName, etc -- network-level post-step

    virtual void  Compute_OutputName();
    // #CAT_Statistic compute the output name for layers and the network, based on the unit names of the most-active units in each target layer -- only works if you provide names to the units -- called automatically in Compute_CycleStats_Post()
    virtual void  Compute_RTCycles();
    // #CAT_Statistic compute the rt_cycles statistic based on trg_max_act and trg_max_act_crit criterion, only in the minus phase -- this is a good measure for computing the reaction time (RT) of the network, as in a psychological experiment -- called automatically in Compute_CycleStats_Post()

  virtual void	Compute_GcIStats_Thr(int thr_no);
  // #CAT_Cycle compute cycle-level stats -- inhibitory conductance AvgMax -- fast layer level computation
  virtual void	Compute_GcIStats_Post();
  // #CAT_Cycle compute cycle-level stats -- inhibitory conductance AvgMax -- single thread post-step

  ///////////////////////////////////////////////////////////////////////
  //	Quarter Final

  virtual void	 Quarter_Final();
  // #CAT_QuarterFinal do final processing after each quarter: 
    virtual void Quarter_Final_Pre();
    // #CAT_QuarterFinal perform computations in layers at end of quarter -- this is a pre-stage that occurs prior to final Quarter_Final_impl -- use this for anything that needs to happen prior to the standard Quarter_Final across units and layers (called by Quarter_Final)
    virtual void Quarter_Final_Unit_Thr(int thr_no);
    // #CAT_QuarterFinal perform Quarter_Final computations in units at end of quarter (called by Quarter_Final)
    virtual void Quarter_Final_Layer();
    // #CAT_QuarterFinal perform computations in layers at end of quarter (called by Quarter_Final)
    virtual void Quarter_Compute_dWt();
    // #CAT_QuarterFinal compute weight changes at end of each quarter -- units decide when this actually happens
    virtual void Quarter_Final_Counters();
    // #CAT_QuarterFinal update counters at end of quarter


  ///////////////////////////////////////////////////////////////////////
  //	LeabraTI Special code

  virtual void Send_TICtxtNetin_Thr(int thr_no);
  // #CAT_TI send TI context netinput -- calls corresponding functions at unit level, first basic send, then _Post that rolls-up send

  virtual void ClearTICtxt();
  // #CAT_TI clear the TI context state from all units in the network -- can be useful to do at discontinuities of experience

  ///////////////////////////////////////////////////////////////////////
  //	Trial Update and Final

  virtual void	Trial_Final();
  // #CAT_TrialFinal do final processing after trial: Compute_AbsRelNetin

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  virtual void	Compute_dWt_Layer_pre();
  // #IGNORE do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  virtual void  Compute_dWt_VecVars_Thr(int thr_no);
  // #IGNORE copy over the vectorized variables for learning

  void	Compute_dWt() override;
    void Compute_dWt_Thr(int thr_no) override;
  virtual void	Compute_dWt_Norm_Thr(int thr_no);
  // #CAT_Learning compute normalization of weight changes -- must be done as a second pass after initial weight changes

  virtual void Compute_Weights_impl();
    void Compute_Weights_Thr(int thr_no) override;

  ///////////////////////////////////////////////////////////////////////
  //	Stats

  virtual void  LayerAvgAct(DataTable* report_table = NULL,
                            LeabraLayerSpec* lay_spec = NULL);
  // #BUTTON #ARGC_1 #NULL_OK #NULL_TEXT_NewReportData create a data table with the current layer average activations (acts_m_avg) and the values specified in the layerspec avg_act.init -- this is useful for setting the .init values accurately based on actual levels 

  virtual void	Set_ExtRew(bool avail, float ext_rew_val);
  // #CAT_Statistic set ext_rew_avail and ext_rew value -- for script access to these values
  virtual void	Compute_ExtRew();
  // #CAT_Statistic compute external reward information: called in plus phase stats
  virtual void	Compute_NormErr();
  // #CAT_Statistic compute normalized binary error between act_m and targ unit values: called in TrialStats -- per unit: if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0; normalization is per layer based on k value: total possible err for both on and off errs is 2 * k (on or off alone is just k)
    virtual void Compute_NormErr_Thr(int thr_no);
    // #IGNORE
  virtual float	Compute_CosErr();
  // #CAT_Statistic compute cosine (normalized dot product) error between act_m and targ unit values
    virtual void Compute_CosErr_Thr(int thr_no);
    // #IGNORE
  virtual float	Compute_CosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) phase difference between act_m and act_p unit values -- must be called after PostQuarter (QuarterFinal) for plus phase to get the act_p values
    virtual void Compute_CosDiff_Thr(int thr_no);
    // #IGNORE
  virtual float	Compute_AvgActDiff();
  // #CAT_Statistic compute average act_diff (act_p - act_m) -- must be called after PostQuarter (QuarterFinal) for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
    virtual void Compute_AvgActDiff_Thr(int thr_no);
    // #IGNORE
  virtual float	Compute_TrialCosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) trial activation difference between act_q0 and act_q4 unit values -- must be called after Quarter_Final for plus phase to get the act_q4 values
    virtual void Compute_TrialCosDiff_Thr(int thr_no);
    // #IGNORE
  void	Compute_TrialStats() override;
  // #CAT_Statistic #OBSOLETE do not call this function anymore -- it is obsolete -- please use Compute_PhaseStats or Compute_MinusStats / Compute_PlusStats for more appropriate stats computation at the right time

  virtual void  Compute_PhaseStats();
  // #CAT_Statistic compute MinusStats at the end of the minus phase, and PlusStats at the end of the plus phase -- this is preferred over the previous implementation of calling TrialStats only at the end of the minus phase, which required targets to be present in the minus phase, which is not always the case
  virtual void  Compute_MinusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the minus phase: minus_output_name -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase
  virtual void  Compute_PlusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the plus phase: all the error stats: SSE, PRerr, NormErr, CosErr, ExtRew -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase

  virtual void	Compute_AbsRelNetin();
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into layers in network -- this should NOT be called from programs (although previously it was) -- it is automatically called in Trial_Final now, and projection-level netin data is subjected to settings of rel_netin if NETIN_PER_PRJN flag is not set
  virtual void	Compute_AvgAbsRelNetin();
  // #CAT_Statistic compute time-average absolute layer-level and relative netinput from different projections into layers in network (e.g. over epoch timescale)

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

  virtual String   TimingReport(DataTable& dt, bool print = true);
  // #CAT_Statistic report detailed timing data to data table, and print a summary -- only collected if thread.get_timing engaged (e.g., call threads.get_timing)

#ifdef CUDA_COMPILE
  void  Cuda_BuildUnits_Threads(); // update device data after net mods
  void  Cuda_UpdateConParams();
  void  Cuda_Send_Netin();
  void  Cuda_Send_Deep5bNetin();
  void  Cuda_Send_TICtxtNetin();
  void  Cuda_Compute_dWt();
  void  Cuda_Compute_dWt_TICtxt();
  void  Cuda_Compute_Weights();
  void  GetWeightsFromGPU() override { Cuda_ConStateToHost(); }
  void  SendWeightsToGPU() override { Cuda_ConStateToDevice(); }
#endif

  void    Cuda_ConStateToHost();
  // #CAT_CUDA get all the connection state variables (weights, dwts, etc) back from the GPU device to the host -- this is done automatically before SaveWeights*, and if cuda.get_wts is set, then host connections are always kept sync'd with the device
  void    Cuda_ConStateToDevice();
  // #CAT_CUDA send all the connection state variables (weights, dwts, etc) to the GPU device from the host -- this is done automatically after Init_Weights and LoadWeights*
  String  Cuda_MemoryReport(bool print = true);
  // #CAT_CUDA report about memory allocation required on CUDA device (only does something for cuda compiled version)
  String  Cuda_TimingReport(bool print = true);
  // #CAT_CUDA report time used statistics for CUDA operations (only does something for cuda compiled version)

  TA_SIMPLE_BASEFUNS(LeabraNetwork);
protected:
  void 	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{}
};

#include <LeabraUnitSpec_inlines>
#include <LeabraConSpec_inlines>

#endif // LeabraNetwork_h
