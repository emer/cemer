// this contains core shared code, and is included directly in LeabraNetwork* _cpp.h, _cuda.h
//{
  enum Phase {
    MINUS_PHASE = 0,		// minus phase
    PLUS_PHASE = 1,		// plus phase
  };

  enum AvgMaxVars { // variables that we collect average and max values of -- thread optimized -- coordintate with LeabraInhib.h
    AM_NET,
    AM_NET_RAW,
    AM_ACT,
    AM_ACT_EQ,
    AM_ACT_RAW,
    AM_UN_G_I,
    AM_DEEP_RAW,
    AM_DEEP_NET,
    N_AM_VARS,
  };

  enum LeabraLayerSpecTypes { // manual type registry system for all spec types used in state code -- any new spec type MUST be added to this list, extending from N case in the last list, for any derived classes, and each spec must return its appropriate enum in GetStateSpecType() method
    T_LeabraLayerSpec = N_NetworkLayerSpecs,
    N_LeabraLayerSpecs,    // derived classes start from this one -- use class name for subclasses
  };

  enum LeabraUnitSpecTypes { // manual type registry system for all spec types used in state code -- any new spec type MUST be added to this list, extending from N case in the last list, for any derived classes, and each spec must return its appropriate enum in GetStateSpecType() method
    T_LeabraUnitSpec = N_NetworkUnitSpecs,
    N_LeabraUnitSpecs,    // derived classes start from this one -- use class name for subclasses
  };

  enum LeabraConSpecTypes { // manual type registry system for all spec types used in state code -- any new spec type MUST be added to this list, extending from N case in the last list, for any derived classes, and  each spec must return its appropriate enum in GetStateSpecType() method
    T_LeabraConSpec = N_NetworkConSpecs,
    T_LeabraBiasSpec,
    N_LeabraConSpecs,    // derived classes start from this one -- use class name for subclasses
  };


  STATE_CLASS(LeabraTimes)     times;     // #MAIN #CONDEDIT_ON_main_obj #CAT_Learning time parameters
  STATE_CLASS(LeabraNetStats)  lstats;    // #MAIN #CONDEDIT_ON_main_obj #CAT_Statistic leabra network-level statistics parameters
  STATE_CLASS(LeabraNetMisc)    net_misc; // #MAIN #CONDEDIT_ON_main_obj misc network level parameters for leabra -- these determine various algorithm variants, typically auto-detected based on the network configuration in Trial_Init_Specs
  STATE_CLASS(LeabraNetDeep)    deep;     // #MAIN #CONDEDIT_ON_main_obj flags for what elements of deep computation need to be performed -- typically auto-detected based on the network configuration in Trial_Init_Specs
  STATE_CLASS(RelNetinSched)    rel_netin; // #MAIN #CONDEDIT_ON_main_obj #CAT_Learning #CONDSHOW_OFF_flags:NETIN_PER_PRJN schedule for computing relative netinput values for each projection -- this is very important data for tuning the network to ensure that each layer has the relative impact it should on other layers -- however it is expensive (only if not using NETIN_PER_PRJN, otherwise it is automatic and these options are disabled), so this schedules it to happen just enough to get the results you want
  bool          unlearnable_trial; // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning this trial is flagged as being unlearnable -- blocks Compute_dWt and error stats from being computed -- particularly relevant for TI, where the prior context provides no basis for prediction (see also cos_err_lrn_thr) -- flag is automatically reset at start of trial -- must be actively set every trial

  ////////////  everything below here should be a counter or statistic -- no params!

#ifdef __MAKETA__
  int           cycle;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW cycle counter: number of iterations of activation updating (settling) on the current alpha-cycle (100 msec / 10 Hz) trial -- this counts time sequentially through the entire trial, typically from 0 to 99 cycles (equivalent to the old ct_cycle, not cycle within a phase as cycle used to be prior to version 7.1.0) -- updated in CycleRun, not updated by program!
#endif

  int		quarter;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW current gamma-frequency (25 msec / 40 Hz) quarter of alpha-cycle (100 msec / 10 Hz) trial being processed -- updated by State
  Phase		phase;		// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW type of settling phase (MINUS or PLUS) -- minus is first 3 quarters, plus is last quarter -- updated by State
  int           tot_cycle;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW total cycle count -- this increments from last Init_Weights and just keeps going up (unless otherwise reset) -- used for tracking things like spiking times continuously across time -- updated by State

  float		rt_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cycles to generate an output response (reaction time = RT) -- typically lstats.trg_max_act_crit is used as an activation criterion over the trg_max_act value recorded from target output layer(s) to determine this -- updated by State
  STATE_CLASS(Average)  avg_cycles;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average settling cycles in the minus phase (computed over previous epoch)
  float		trg_max_act;	// #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic maximum activation of any unit in a target layer -- compared against lstats.trg_max_act_crit to compute rt_cycles

  float		send_pct;	// #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic proportion of sending units that actually sent activations on this cycle -- only available for non-threading case
  int		send_pct_n;	// #NO_SAVE #GUI_READ_ONLY #CAT_Statistic number of units sending activation this cycle
  int		send_pct_tot;	// #NO_SAVE #GUI_READ_ONLY #CAT_Statistic total number of units that could send activation this cycle
  STATE_CLASS(Average)  avg_send_pct;	// #NO_SAVE #GUI_READ_ONLY #EXPERT #CAT_Statistic #DMEM_AGG_SUM average proportion of units sending activation over an epoch -- only available for non-threading case

  float		ext_rew;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW external reward value (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network
  bool		ext_rew_avail; 	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic actual external reward value is available (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network -- if false then no feedback was provided on this trial
  STATE_CLASS(Average)  avg_ext_rew;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average external reward value (computed over previous epoch)

  float		norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW normalized binary (Hamming) error on this trial: number of units that were incorrectly activated or incorrectly inactivated (see lstats.off_errs, on_errs to exclude each component separately)
  STATE_CLASS(Average)  avg_norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average normalized binary error value (computed over previous epoch)

  float		cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cosine (normalized dot product) error on this trial, comparing targ vs. act_m
  float		cos_err_prv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_net_misc.ti #CAT_Statistic cosine (normalized dot product) error on this trial, comparing targ on this trial against activations on previous trial (act_q0) -- computed automatically during TI
  float		cos_err_vs_prv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_net_misc.ti #CAT_Statistic cos_err - cos_err_prv -- how much better is cosine error on this trial relative to just saying the same thing as was output last time -- for TI
  STATE_CLASS(Average)  avg_cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error (computed over previous epoch)
  STATE_CLASS(Average)  avg_cos_err_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_net_misc.ti #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on prv (see cos_err_prv) (computed over previous epoch)
  STATE_CLASS(Average)  avg_cos_err_vs_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_net_misc.ti #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on vs prv (see cos_err_vs_prv) (computed over previous epoch)

  float		cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) difference between act_p and act_m activations on this trial -- excludes input layers which are represented in the cos_err measure
STATE_CLASS(Average)	avg_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) diff (computed over previous epoch)

  float		avg_act_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average act_diff (act_p - act_m) -- this is an important statistic to track overall 'main effect' differences across phases -- excludes input layers
  STATE_CLASS(Average)	avg_avg_act_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average avg_act_diff (computed over previous epoch)

  float		trial_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) activation difference across trials between act_q4 and act_q0 activations on this trial -- excludes input layers which are represented in the cos_err measure
  STATE_CLASS(Average)	avg_trial_cos_diff; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) trial diff (computed over previous epoch)
  float         net_sd;             // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail
  STATE_CLASS(Average)  avg_net_sd;         // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail
  float         hog_pct;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic the percentage of units in the network that have a long-time-averaged activitation level that is above a layer-specific threshold, indicating that they are 'hogging' the representational space (because this is computed on a time average, there is no epoch average of this statistic)
  float         dead_pct;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic the percentage of units in the network that have a long-time-averaged activitation level that is below a layer-specific threshold, indicating that they are effectively 'dead' and not participating in any representations (because this is computed on a time average, there is no epoch average of this statistic)

  INLINE bool  RecvOwnsCons() const override { return false; }

  INLINE  int  QuarterCycle() const { return cycle - quarter * times.quarter; }
  // #CAT_Cycle cycles within current cycle -- network cycle counter counts for entire trial across 4 quarters


  INLINE void Initialize_net_core() {
    unlearnable_trial = false;    quarter = 0;    phase = MINUS_PHASE;
    tot_cycle = 0;    rt_cycles = 0.0f;
    send_pct = 0.0f;    send_pct_n = send_pct_tot = 0;
    trg_max_act = 0.0f;
    ext_rew = 0.0f;    ext_rew_avail = false;
    norm_err = 0.0f;
    cos_err = 0.0f;    cos_err_prv = 0.0f;    cos_err_vs_prv = 0.0f;
    cos_diff = 0.0f;    trial_cos_diff = 0.0f;
    net_sd = 0.0f;    hog_pct = 0.0f;    dead_pct = 0.0f;
  }
  // #IGNORE


  
