// this contains core shared code, and is included directly in LeabraNetworkState _cpp.h, _cuda.h
//{

  enum LeabraThrLayStats {         // stats that require holding threaded layer-level variables for subsequent aggregation
    NORMERR = N_NetThrLayStats,
    COSERR,
    COSDIFF,
    AVGACTDIFF,
    TRIALCOSDIFF,
    ACTMARGIN,
    NETSD,
    HOGDEAD,
    N_LeabraThrLayStats,
  };

  float**       thrs_send_deeprawnet_tmp;
  // #IGNORE #CAT_State temporary storage for threaded sender-based deep netinput computation -- float*[threads] array of float[n_units]
  float**       thrs_send_deepmodnet_tmp;
  // #IGNORE #CAT_State temporary storage for threaded sender-based deep netinput computation -- float*[threads] array of float[n_units]
  char**        thrs_ungp_avg_max_vals;
  // #IGNORE LeabraAvgMaxRaw data for unit groups, by thread
  float         tmp_arg1;        // #IGNORE for passing args through threaded call

  INLINE float* ThrSendDeepRawNetTmp(int thr_no) const 
  { return thrs_send_deeprawnet_tmp[thr_no]; }
  // #IGNORE temporary sending deep_raw_net netinput memory for given thread -- also used for deep_ctxt
  INLINE float* ThrSendDeepModNetTmp(int thr_no) const 
  { return thrs_send_deepmodnet_tmp[thr_no]; }
  // #IGNORE temporary sending deep_mod_net netinput memory for given thread 

  INLINE STATE_CLASS(LeabraAvgMax)* ThrUnGpAvgMax(int thr_no, int ungp_idx, AvgMaxVars var) 
  { return (STATE_CLASS(LeabraAvgMax)*)
      (thrs_ungp_avg_max_vals[thr_no] +(n_ungps_built * var + ungp_idx) *
       sizeof(STATE_CLASS(LeabraAvgMax))); }
  // #IGNORE get AvgMax data for given thread, unit group, and variable


  INIMPL virtual void AllocLeabraStatsMem();
  // #IGNORE allocate memory for stats aggregation
  INIMPL void AllocSendNetinTmpState() override;
  // #IGNORE allocate temp buffers for sender-based netinput
  INIMPL virtual void InitLeabraThreadMem_Thr(int thr_no);
  INIMPL void  InitSendNetinTmp_Thr(int thr_no) override;

  INIMPL void FreeStateMem() override;
  // #IGNORE free all state memory
  
  inline bool AggPerfStats() {
    if(!unlearnable_trial) return true;
    return lstats.agg_unlearnable;
  }
  // #CAT_Statistic test for whether to aggregate (into the epoch level averages) performance evaluating statistics on this trial -- returns false for unlearnable trials if lstats.agg_unlearnable is false, otherwise always true (pending further development)


  ///////////////////////////////////////////////////////////////////////
  //	Overloaded accessors for Leabra type, only most popular

  INLINE LEABRA_LAYER_STATE* GetLayerState(int lay_idx) const {
    return (LEABRA_LAYER_STATE*)inherited::GetLayerState(lay_idx); 
  }
  INLINE LEABRA_PRJN_STATE* GetPrjnState(int prjn_idx) const {
    return (LEABRA_PRJN_STATE*)inherited::GetPrjnState(prjn_idx);
  }
  INLINE LEABRA_PRJN_STATE* GetSendPrjnState(int prjn_idx) const {
    return (LEABRA_PRJN_STATE*)inherited::GetSendPrjnState(prjn_idx);
  }
  INLINE LEABRA_UNGP_STATE* GetUnGpState(int ungp_idx) const {
    return (LEABRA_UNGP_STATE*)inherited::GetUnGpState(ungp_idx);
  }
  INLINE LEABRA_UNIT_STATE*  ThrUnitState(int thr_no, int thr_un_idx) const {
    return (LEABRA_UNIT_STATE*)inherited::ThrUnitState(thr_no, thr_un_idx);
  }
  INLINE LEABRA_UNIT_STATE*  GetUnitState(int flat_idx) const {
    return (LEABRA_UNIT_STATE*)inherited::GetUnitState(flat_idx);
  }
  INLINE LEABRA_CON_STATE* ThrRecvConState(int thr_no, int thr_cgp_idx) const {
    return (LEABRA_CON_STATE*)inherited::ThrRecvConState(thr_no, thr_cgp_idx);
  }
  INLINE LEABRA_CON_STATE* ThrSendConState(int thr_no, int thr_cgp_idx) const {
    return (LEABRA_CON_STATE*)inherited::ThrSendConState(thr_no, thr_cgp_idx);
  }
  INLINE LEABRA_CON_STATE* ThrUnRecvConState(int thr_no, int thr_un_idx, int recv_idx) const {
    return (LEABRA_CON_STATE*)inherited::ThrUnRecvConState(thr_no, thr_un_idx, recv_idx);
  }
  INLINE LEABRA_CON_STATE* ThrUnSendConState(int thr_no, int thr_un_idx, int recv_idx) const {
    return (LEABRA_CON_STATE*)inherited::ThrUnSendConState(thr_no, thr_un_idx, recv_idx);
  }

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  INIMPL void Init_Weights_Layers() override;
  INIMPL void Init_Stats() override;
  INIMPL void Init_Stats_Layers() override;
  INIMPL void Init_Counters_State() override;

  INIMPL virtual void Init_AdaptInhib();
  // #CAT_Activation Initialize adaptive inhibition gain value on all the layers -- undoes any adaptation that has taken place (including from loaded weights - the adaptive gain value is saved with the weights)

  INIMPL virtual void Init_Acts_Layers();
  // #CAT_TrialInit layer-level acts init 

  INIMPL virtual void Init_Netins_Thr(int thr_no);
  // #IGNORE initialize netinput computation variables (delta-based requires several intermediate variables)
  
  INIMPL virtual void DecayState_Thr(int thr_no);
  // #IGNORE decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)
  
  INIMPL virtual void ResetSynTR_Thr(int thr_no);
  // #IGNORE


  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial


  INIMPL virtual void Trial_Init_Counters();
  // #CAT_TrialInit init counters -- network only
  INIMPL virtual void Trial_Init_Specs();
  // #CAT_TrialInit initialize specs and specs update network flags
  INIMPL virtual void Trial_Init_Unit_Thr(int thr_no);
  // #IGNORE trial unit-level initialization functions: Trial_Init_SRAvg, DecayState, NoiseInit
  INIMPL virtual void Trial_Init_Layers();
  // #CAT_TrialInit layer-level trial init (used in base code to init layer-level sravg, can be overloaded)


  ///////////////////////////////////////////////////////////////////////
  //	QuarterInit -- at start of a given quarter trial of processing

  INIMPL virtual void Quarter_Init_TargFlags_Thr(int thr_no);
  // #IGNORE initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
  INIMPL virtual void Quarter_Init_Counters();
  // #CAT_QuarterInit initialize counters for upcoming quarter -- network only
  INIMPL virtual void Quarter_Init_Layers();
  // #CAT_QuarterInit quarter layer-level initialization hook -- default calls TargFlags_Layer, and can be used for hook for other guys
  INIMPL virtual void Compute_HardClamp_Layers();
  // #IGNORE set hard clamp flags etc based on clamp settings and external input -- layer version called automatically by Quarter_Init_layer so this is not called directly
  INIMPL virtual void Quarter_Init_Unit();
  // #CAT_QuarterInit quarter unit-level initialization functions: Init_TargFlags, NetinScale
    INIMPL virtual void Quarter_Init_Unit_Thr(int thr_no);
    // #IGNORE quarter unit-level initialization functions: Init_TargFlags, NetinScale
  INIMPL virtual void Quarter_Init_TargFlags_Layers();
  // #IGNORE initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
    INIMPL virtual void Quarter_Init_Deep_Thr(int thr_no);
    // #IGNORE quarter deep leabra init: deep_ctxt compute and deep state update
  INIMPL virtual void Compute_NetinScale_Thr(int thr_no);
  // #IGNORE compute netinput scaling values by projection -- not called by default -- direct to unit-level function
    INIMPL virtual void Compute_NetinScale_Senders_Thr(int thr_no);
    // #IGNORE compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the
  INIMPL virtual void Compute_HardClamp_Thr(int thr_no);
  // #IGNORE compute hard clamping from external inputs
  INIMPL virtual void InitDeepRawNetinTmp_Thr(int thr_no);
  // #IGNORE initialize deep_raw netin temp buffer
  INIMPL virtual void Compute_DeepCtxtStats_Thr(int thr_no);
  // #IGNORE compute layer and unit-group level stats on deep_ctxt
  INIMPL virtual void Compute_DeepCtxtStats_Post();
  // #IGNORE compute layer and unit-group level stats on deep_ctxt
  INIMPL virtual void Quarter_Init_Layer_Post();
  // #CAT_QuarterInit quarter layer-level initialization hook -- post = last step in Quarter_Init

  INIMPL virtual void ExtToComp_Layers();
  // #IGNORE
  INIMPL virtual void ExtToComp_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual void TargExtToComp_Layers();
  // #IGNORE
  INIMPL virtual void TargExtToComp_Thr(int thr_no);
  // #IGNORE


  ////////////////////////////////////////////////////////////////
  //	Cycle_Run
  
  INIMPL virtual void Cycle_Run_Thr(int thr_no);
  // #IGNORE compute cycle(s) of updating: netinput, inhibition, activations -- multiple cycles can be run depending on lthreads.n_cycles setting and whether multiple 
  INIMPL virtual void  Cycle_IncrCounters();
  // #CAT_Cycle increment the cycle-level counters -- called internally during Cycle_Run()
  
  ///////////////////////////////////////////////////////
  //	Cycle Stage 1: netinput

  INIMPL void Send_Netin_Thr(int thr_no) override;
  // #IGNORE compute netinputs -- sender-delta based -- only send when sender activations change -- sends into tmp array that is then integrated into net_raw, gi_raw
  INIMPL virtual void Compute_NetinInteg_Thr(int thr_no);
  // #IGNORE integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)
  INIMPL virtual void Compute_NetinStats_Thr(int thr_no);
  // #IGNORE compute layer and unit-group level stats on net input levels -- needed for inhibition
  INIMPL virtual void Compute_NetinStats_Post();
  // #IGNORE compute layer and unit-group level stats on net input levels -- needed for inhibition
  INIMPL virtual void Compute_DeepModStats_Thr(int thr_no);
  // #IGNORE compute layer and unit-group level stats on deep_mod_net input levels
  INIMPL virtual void Compute_DeepModStats_Post();
  // #IGNORE compute layer and unit-group level stats on deep_mod_net input levels
  INIMPL virtual void InitCycleNetinTmp_Thr(int thr_no);
  // #IGNORE initialize deep_raw netin temp buffer

  
  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  INIMPL virtual void	Compute_Inhib();
  // #IGNORE compute inhibitory conductances via inhib functions (FFFB) -- calls Compute_NetinStats and LayInhibToGps to coordinate group-level inhibition sharing

  
  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation
  
  INIMPL void	Compute_Act_Thr(int thr_no) override;
  // #IGNORE compute activations
  INIMPL virtual void	Compute_Act_Rate_Thr(int thr_no);
  // #IGNORE rate coded activations
  INIMPL virtual void	Compute_Act_Spike_Thr(int thr_no);
  // #IGNORE spiking activations
  INIMPL virtual void	Compute_Act_Post_Thr(int thr_no);
  // #IGNORE post processing after activations have been computed -- special algorithm code takes advantage of this stage to send modulator variables -- all such vars should EXCLUSIVELY be sent during this stage, and running average activations (SRAvg) also computed


  INIMPL virtual void   ThalGatedNow();
  // #IGNORE record current cycle as gating cycle -- called by enabled thalamic gating layer(s)


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  INIMPL virtual void	Compute_CycleStats_Pre();
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, OutputName, etc -- network-level pre-step -- happens after Compute_Act and prior to Compute_Act_Post -- good place to insert any layer-level modification of unit-level activations
  INIMPL virtual void	Compute_CycleStats_Thr(int thr_no);
  // #IGNORE compute cycle-level stats -- acts AvgMax -- fast layer level computation
  INIMPL virtual void	Compute_ActEqStats_Thr(int thr_no);
  // #IGNORE compute cycle-level stats -- acts AvgMax -- fast layer level computation
  INIMPL virtual void	Compute_CycleStats_Post();
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, OutputName, etc -- network-level post-step
  INIMPL virtual void   Compute_RTCycles();
  // #CAT_Statistic compute the rt_cycles statistic based on trg_max_act and trg_max_act_crit criterion, only in the minus phase -- this is a good measure for computing the reaction time (RT) of the network, as in a psychological experiment -- called automatically in Compute_CycleStats_Post()

  INIMPL virtual void	Compute_GcIStats_Thr(int thr_no);
  // #IGNORE compute cycle-level stats -- inhibitory conductance AvgMax -- fast layer level computation
  INIMPL virtual void	Compute_GcIStats_Post();
  // #CAT_Cycle compute cycle-level stats -- inhibitory conductance AvgMax -- single thread post-step

  
  ///////////////////////////////////////////////////////////////////////
  //	DeepLeabra deep_raw Updating -- called after superficial layer updating

  INIMPL virtual void Compute_DeepRaw_Thr(int thr_no);
  // #IGNORE update deep_raw variables, using the proper sequence of unit-level calls

  INIMPL virtual void Compute_DeepRawStats_Thr(int thr_no);
  // #IGNORE compute layer and unit-group level stats on deep_raw vars
  INIMPL virtual void Compute_DeepRawStats_Post();
  // #IGNORE compute layer and unit-group level stats on deep_raw vars

  INIMPL virtual void ClearDeepActs_Thr(int thr_no);
  // #IGNORE clear all the deep lamina variables -- can be useful to do at discontinuities of experience
  INIMPL virtual void ClearMSNTrace_Thr(int thr_no);
  // #IGNORE clear the synaptic trace for MSN connections (Medium Spiny Neurons in the Striatum)

  ///////////////////////////////////////////////////////////////////////
  //	Quarter Final

  INIMPL virtual void Quarter_Final_Pre();
  // #CAT_QuarterFinal perform computations in layers at end of quarter -- this is a pre-stage that occurs prior to final Quarter_Final_impl -- use this for anything that needs to happen prior to the standard Quarter_Final across units and layers (called by Quarter_Final)
  INIMPL virtual void Quarter_Final_Unit_Thr(int thr_no);
  // #IGNORE #CAT_QuarterFinal perform Quarter_Final computations in units at end of quarter (called by Quarter_Final) -- also does CosDiff_Thr

  INIMPL virtual void Quarter_Final_Layers();
  // #CAT_QuarterFinal perform computations in layers at end of quarter (called by Quarter_Final)
  INIMPL virtual void Quarter_Final_Counters();
  // #CAT_QuarterFinal update counters at end of quarter

  ///////////////////////////////////////////////////////////////////////
  //	Trial Update and Final

  INIMPL virtual void	Compute_AbsRelNetin();
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into layers in network -- this should NOT be called from programs (although previously it was) -- it is automatically called in Trial_Final now, and projection-level netin data is subjected to settings of rel_netin if NETIN_PER_PRJN flag is not set
  INIMPL virtual void	Compute_AvgAbsRelNetin();
  // #CAT_Statistic compute time-average absolute layer-level and relative netinput from different projections into layers in network (e.g. over epoch timescale)


  ///////////////////////////////////////////////////////////////////////
  //	Learning

  INIMPL virtual void Compute_dWt_Layer_pre();
  // #IGNORE do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  INIMPL void Compute_dWt_Thr(int thr_no) override;
  INIMPL void Compute_Weights_Thr(int thr_no) override;
  
  INIMPL virtual void Compute_WtBal_Thr(int thr_no);
  // #IGNORE compute weight balance factors
  INIMPL virtual void Compute_WtBalStats();
  // #IGNORE compute weight balance statistics


  ///////////////////////////////////////////////////////////////////////
  //	Stats

  INIMPL virtual void Set_ExtRew(bool avail, float ext_rew_val);
  // #CAT_Statistic set ext_rew_avail and ext_rew value -- for script access to these values
  INIMPL virtual void Compute_ExtRew();
  // #CAT_Statistic compute external reward information: called in plus phase stats

  INIMPL float Compute_SSE_Layer(LAYER_STATE* lay, int& n_vals, bool unit_avg = false, bool sqrt = false) override;

  INIMPL virtual void Compute_NormErr_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual void Compute_NormErr_Agg();
  // #IGNORE
  INIMPL virtual void Compute_CosErr_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual float Compute_CosErr_Agg();
  // #IGNORE
  INIMPL virtual void Compute_CosDiff_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual float Compute_CosDiff_Agg();
  // #IGNORE
  INIMPL virtual void Compute_AvgActDiff_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual float Compute_AvgActDiff_Agg();
  // #IGNORE
  INIMPL virtual void Compute_TrialCosDiff_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual float Compute_TrialCosDiff_Agg();
  // #IGNORE
  INIMPL virtual void Compute_ActMargin_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual void Compute_ActMargin_Agg();
  // #IGNORE
  INIMPL virtual void Compute_RTCycles_Agg();
  // #IGNORE
  INIMPL virtual void Compute_NetSd_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual float Compute_NetSd_Agg();
  // #IGNORE
  INIMPL virtual void Compute_HogDeadPcts_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual void Compute_HogDeadPcts_Agg();
  // #IGNORE

  INIMPL virtual void Compute_PlusStats_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual void Compute_PlusStats_Agg();
  // #IGNORE
  
  INIMPL virtual void  Compute_EpochWeights();
  // #CAT_Learning perform any epoch-level weight updates or adjustments..
  INIMPL virtual void Compute_EpochWeights_Thr(int thr_no);
  // #IGNORE

  INIMPL void Compute_EpochStats_Layer(LAYER_STATE* lay) override;

  INIMPL virtual void	Compute_AvgCycles();
  // #CAT_Statistic compute average cycles (at an epoch-level timescale)
  INIMPL   virtual void	Compute_AvgExtRew();
  // #CAT_Statistic compute average external reward information (at an epoch-level timescale)
  INIMPL virtual void	Compute_AvgNormErr();
  // #CAT_Statistic compute average norm_err (at an epoch-level timescale)
  INIMPL virtual void	Compute_AvgCosErr();
  // #CAT_Statistic compute average cos_err (at an epoch-level timescale)
  INIMPL virtual void	Compute_AvgSendPct();
  // #CAT_Statistic compute average sending pct (at an epoch-level timescale)
  INIMPL virtual void	Compute_AvgCosDiff();
  // #CAT_Statistic compute average cos_diff (at an epoch-level timescale)
  INIMPL virtual void	Compute_AvgTrialCosDiff();
  // #CAT_Statistic compute average trial_cos_diff (at an epoch-level timescale)
  INIMPL virtual void	Compute_AvgAvgActDiff();
  // #CAT_Statistic compute average avg_act_diff (at an epoch-level timescale)
  INIMPL virtual void	Compute_AvgNetSd();
  // #CAT_Statistic compute average net_sd (at an epoch-level timescale)


  INIMPL LAYER_SPEC_CPP*    NewLayerSpec(int spec_type) const override;
  INIMPL UNIT_SPEC_CPP*     NewUnitSpec(int spec_type) const override;
  INIMPL CON_SPEC_CPP*      NewConSpec(int spec_type) const override;

  INLINE void Initialize_core() {
    n_lay_stats = N_LeabraThrLayStats;

    thrs_send_deeprawnet_tmp = NULL;
    thrs_send_deepmodnet_tmp = NULL;
    thrs_ungp_avg_max_vals = NULL;
  }
