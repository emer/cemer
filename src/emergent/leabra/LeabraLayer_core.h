// this contains core shared code, and is included directly in LeabraLayer* _cpp.h, _cuda.h
//{

  bool           hard_clamped;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation if true, indicates that this layer was actually hard clamped -- this is normally set by the Compute_HardClamp function called by Quarter_Init() or NewInputData_Init() -- see LayerSpec clamp.hard parameter to determine whether layer is hard clamped or not -- this flag is not to be manipulated directly
  float         lrate_mod;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning learning rate modulation factor based on layer_lrate for this layer, and cos_diff.lrate_mod result for this layer
  bool          deep_lrate_mod;  // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning apply deep learning rate modulation to this layer -- set from the unit spec, used by conspec in setting learning rates
  float         adapt_gi;        // #GUI_READ_ONLY #SHOW #CAT_Activation #SAVE_WTS adaptive inhibitory gain value -- this is an *extra* multiplier on top of existing gi value in the layer, unit inhib specs, starts out at 1 and moves from there -- adjusted by adaptive inhibition function -- saved with weight files
  float         mod_avg_l_lrn;  // #GUI_READ_ONLY #SHOW #CAT_Activation avg_l_lrn multiplier -- modulates amount of longer-term average BCM-style hebbian learning -- can be modulated in various ways according to unit spec avg_l_lrn specs
  STATE_CLASS(LeabraMarginVals) margin; // #SAVE_WTS #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning marginal v_m_eq levels -- used for modulating learning rate for those on the margin or even changing sign of learning
  float         da_p;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning positive valence oriented dopamine-like modulatory value (where applicable)
  float         da_n;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning positive valence oriented dopamine-like modulatory value (where applicable)
  float         sev;            // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning serotonin-like modulatory value (where applicable)
  LEABRA_AVG_MAX        avg_netin;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation minus-phase net input values for the layer, averaged over an epoch-level timescale
  LEABRA_AVG_MAX        avg_netin_sum;        // #NO_SAVE #GUI_READ_ONLY #HIDDEN #CAT_Activation #DMEM_AGG_SUM sum of net input values for the layer, for computing average over an epoch-level timescale
  int           avg_netin_n;        // #NO_SAVE #GUI_READ_ONLY #HIDDEN #CAT_Activation #DMEM_AGG_SUM number of times sum is updated for computing average
  float         pre_hog_pct;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic the percentage of units in the layer that have a long-time-averaged activitation level that is above the layerspec pre_hog_thr threshold, indicating that they are potentialy on their way toward 'hogging' the representational space (because this is computed on a time average, there is no epoch average of this statistic)
  float         hog_pct;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic the percentage of units in the layer that have a long-time-averaged activitation level that is above the layerspec hog_thr threshold, indicating that they are 'hogging' the representational space (because this is computed on a time average, there is no epoch average of this statistic)
  float         dead_pct;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic the percentage of units in the layer that have a long-time-averaged activitation level that is below the layerspec dead_thr threshold, indicating that they are effectively 'dead' and not participating in any representations (because this is computed on a time average, there is no epoch average of this statistic)
  float         bin_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic binary  error value for this layer -- 1 if sse > network.stats.cnt_err_thr, else 0 -- this is useful for producing a cnt_err measure by aggregating trial log data
  float         max_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic max activity error value for this layer -- is 0 if the most active unit in the layer (or across unit groups if unit groups are present and inhibition uses them) has a non-zero target value -- otherwise 1 -- in other words, is the most active unit a target unit?  this only really makes sense for localist single-unit activity layers (although multiple units can be set to targets to allow for multiple options).  it is a highly sensitive measure, allowing for any other kinds of activity in the layer
  float         norm_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic normalized binary error value for this layer, computed subject to the parameters on the network
  STATE_CLASS(Average)        avg_norm_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average normalized binary error value (computed over previous epoch)
  float         cos_diff;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) activation difference between act_p and act_m on this trial for this layer -- computed by Compute_CosDiff -- must be called after Quarter_Final in plus phase to get act_p values
  STATE_CLASS(Average)        avg_cos_diff;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) diff (computed over previous epoch)
  float         cos_diff_avg;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic running average of cosine (normalized dot product) difference between act_p and act_m -- computed with layerspec cos_diff.avg_tau time constant in Quarter_Final, and used for modulating hebbian learning (see cos_diff_avg_lrn) and overall learning rate
  float         cos_diff_var;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic running variance of cosine (normalized dot product) difference between act_p and act_m -- computed with layerspec cos_diff.diff_avg_tau time constant in Quarter_Final, used for modulating overall learning rate
  float         cos_diff_avg_lrn; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic 1.0 - cos_diff_avg and 0 for non-HIDDEN layers -- this is the value of cos_diff_avg used for avg_l.err_mod_l modulation of the avg_l_lrn factor if enabled
  float         cos_err_prv;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) error on this trial for this layer, comparing targ on this trial against activations from previous trial (act_q0) -- computed automatically during TI
  STATE_CLASS(Average)        avg_cos_err_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on prv (see cos_err_prv) (computed over previous epoch)
  float         cos_err_vs_prv;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cos_err - cos_err_prv -- how much better is cosine error on this trial relative to just saying the same thing as was output last time -- for TI
  STATE_CLASS(Average)        avg_cos_err_vs_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on vs prv (see cos_err_vs_prv) (computed over previous epoch)
  float         avg_act_diff;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average act_diff (act_p - act_m) for this layer -- this is an important statistic to track overall 'main effect' differences across phases 
  STATE_CLASS(Average)        avg_avg_act_diff;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average avg_act_diff (computed over previous epoch)
  float         trial_cos_diff;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) trial-level activation difference between act_q4 and act_q0 on this trial for this layer -- computed by Compute_TrialCosDiff -- must be called after Quarter_Final in plus phase to get act_q4 values
  STATE_CLASS(Average)        avg_trial_cos_diff;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) trial diff (computed over previous epoch)
  float         net_sd;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail
  STATE_CLASS(Average)        avg_net_sd;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average net_sd (computed over previous epoch) -- standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail

  // following are commonly-used stats that are copied from Layer-level UnGpState so it is easier to
  // monitor for them etc

  float         acts_m_avg;        // #GUI_READ_ONLY #SHOW #CAT_Activation #SAVE_WTS COPIED FROM UNGP STATE: time-averaged minus-phase activation stats for the layer -- this is used for adaptive inhibition tuning (inhib_adapt in LeabraLayerSpec) -- time constant in layer spec avg_act.tau and initialized to avg_act.init -- saved with weights
  float         acts_p_avg;        // #GUI_READ_ONLY #SHOW #CAT_Activation #SAVE_WTS COPIED FROM UNGP STATE: time-averaged plus-phase activation stats for the layer -- time constant in layer spec avg_act.tau and initialized to avg_act.init -- this is used for netinput scaling (via _eff version) and should match reasonably well with act_avg.init value -- saved with weights
  float         acts_p_avg_eff;    // #GUI_READ_ONLY #SHOW #CAT_Activation #SAVE_WTS COPIED FROM UNGP STATE: acts_p_avg * avg_act.adjust factor -- this is the effective value actually used for netinput scaling based on layer activation levels -- saved with weights
  LEABRA_AVG_MAX    netin;         // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum net input values for the layer
  LEABRA_AVG_MAX    netin_m;       // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum net input values for the layer, minus phase
  LEABRA_AVG_MAX    acts;          // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum activation values for the layer
  LEABRA_AVG_MAX    acts_eq;       // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum act_eq values for the layer
  LEABRA_AVG_MAX    acts_m;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum minus-phase activation stats for the layer (from acts_eq at end of minus phase)
  LEABRA_AVG_MAX    acts_p;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum plus-phase activation stats for the layer (from acts_eq at end of plus phase)
  LEABRA_AVG_MAX    am_avg_l;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum long-term average activation, avg_l, used for dynamic floating threshold in BCM hebbian learning 
  LEABRA_AVG_MAX    am_avg_l_lrn;  // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum learning rate based on long-term average activation, avg_l_lrn -- determines how much BCM hebbian learning occurs


  INLINE LEABRA_LAYER_SPEC_CPP* GetLayerSpec(NETWORK_STATE* net) const
  { return (LEABRA_LAYER_SPEC_CPP*)inherited::GetLayerSpec(net); }

  INLINE LEABRA_UNIT_SPEC_CPP* GetUnitSpec(NETWORK_STATE* net) const
  { return (LEABRA_UNIT_SPEC_CPP*)inherited::GetUnitSpec(net); }

  INLINE LEABRA_LAYER_STATE* GetLayerState(NETWORK_STATE* net) const
  { return (LEABRA_LAYER_STATE*)inherited::GetLayerState(net); }

  INLINE LEABRA_UNGP_STATE* GetLayUnGpState(NETWORK_STATE* net) const
  { return (LEABRA_UNGP_STATE*)inherited::GetLayUnGpState(net); }

  INLINE LEABRA_UNGP_STATE* GetUnGpState(NETWORK_STATE* net, int ungp_no) const
  { return (LEABRA_UNGP_STATE*)inherited::GetUnGpState(net, ungp_no); }

  INLINE LEABRA_PRJN_STATE* GetRecvPrjnState(NETWORK_STATE* net, int prjn_no) const
  { return (LEABRA_PRJN_STATE*)inherited::GetRecvPrjnState(net, prjn_no); }

  INLINE LEABRA_PRJN_STATE* GetSendPrjnState(NETWORK_STATE* net, int prjn_no) const
  { return (LEABRA_PRJN_STATE*)inherited::GetSendPrjnState(net, prjn_no); }


  INLINE LEABRA_UNIT_STATE* GetUnitState(NETWORK_STATE* net, int un_no) const
  { return (LEABRA_UNIT_STATE*)inherited::GetUnitState(net, un_no); }

  INLINE LEABRA_UNIT_STATE* GetUnitStateSafe(NETWORK_STATE* net, int un_no) const
  { return (LEABRA_UNIT_STATE*)inherited::GetUnitStateSafe(net, un_no); }

  INLINE LEABRA_UNIT_STATE* GetUnitStateFlatXY(NETWORK_STATE* net, int fl_x, int fl_y) const
  { return (LEABRA_UNIT_STATE*)inherited::GetUnitStateFlatXY(net, fl_x, fl_y); }

  INLINE LEABRA_UNIT_STATE* GetUnitStateGpUnIdx(NETWORK_STATE* net, int gp_dx, int un_dx) const
  { return (LEABRA_UNIT_STATE*)inherited::GetUnitStateGpUnIdx(net, gp_dx, un_dx); }
    
  INLINE LEABRA_UNIT_STATE* GetUnitStateGpXYUnIdx(NETWORK_STATE* net, int gp_x, int gp_y, int un_dx) const
  { return (LEABRA_UNIT_STATE*)inherited::GetUnitStateGpXYUnIdx(net, gp_x, gp_y, un_dx); }
    
  INLINE LEABRA_UNIT_STATE* GetUnitStateGpIdxUnXY(NETWORK_STATE* net, int gp_dx, int un_x, int un_y) const
  { return (LEABRA_UNIT_STATE*)inherited::GetUnitStateGpIdxUnXY(net, gp_dx, un_x, un_y); }
    
  INLINE LEABRA_UNIT_STATE* GetUnitStateGpUnXY(NETWORK_STATE* net, int gp_x, int gp_y, int un_x, int un_y) const
  { return (LEABRA_UNIT_STATE*)inherited::GetUnitStateGpUnXY(net, gp_x, gp_y, un_x, un_y); }


  INLINE float  GetTotalActEq(NETWORK_STATE* net)  {
    LEABRA_UNGP_STATE* lugp = GetLayUnGpState(net);
    return lugp->GetTotalActEq(n_units);
  }
  // Get the total act_eq activation in the layer based on average and number of units
  INLINE float  GetTotalActQ0(NETWORK_STATE* net)  {
    LEABRA_UNGP_STATE* lugp = GetLayUnGpState(net);
    return lugp->GetTotalActQ0(n_units);
  } 
  // Get the total act_q0 activation in the layer based on average and number of units

  INLINE void   CopyFromUnGpState(LEABRA_UNGP_STATE* ugp) {
    acts_m_avg = ugp->acts_m_avg;  acts_p_avg = ugp->acts_p_avg;
    acts_p_avg_eff = ugp->acts_p_avg_eff;
    netin = ugp->netin;  netin_m = ugp->netin_m;  acts = ugp->acts;  acts_eq = ugp->acts_eq;
    acts_m = ugp->acts_m;  acts_p = ugp->acts_p;
    am_avg_l = ugp->am_avg_l;  am_avg_l_lrn = ugp->am_avg_l_lrn;
  }
  // copy stats from unit group state to make avail at layer level

  /////////////////////////////////////////////////////////////////
  //    Layer-level functions that need to be accessible

  INIMPL void        DecayState(float decay);
  // #CAT_Activation decay the state of this layer -- not normally called but available for programs etc to control specific layers

  INIMPL void        ClearDeepActs();
  // #CAT_DeepLeabra clear the deep lamina variables -- can be useful to do at discontinuities of experience

  INLINE void Init_Stats() {
    inherited::Init_Stats();
    avg_netin.InitVals();  avg_netin_sum.InitVals();  avg_netin_n = 0;
    avg_netin.max = 0.0f;  avg_netin_sum.max = 0.0f; // using max in averaging
    pre_hog_pct = 0.0f;    hog_pct = 0.0f;    dead_pct = 0.0f;
    bin_err = 0.0f;    max_err = 0.0f;
    norm_err = 0.0f;   avg_norm_err.ResetAvg();
    cos_err = 0.0f;    avg_cos_err.ResetAvg();
    cos_diff = 0.0f;    avg_cos_diff.ResetAvg();
    cos_diff_avg = 0.0f; cos_diff_var = 0.0f;    cos_diff_avg_lrn = 0.0f;
    cos_err_prv = 0.0f;     avg_cos_err_prv.ResetAvg();
    cos_err_vs_prv = 0.0f;     avg_cos_err_vs_prv.ResetAvg();
    avg_act_diff = 0.0f;    avg_avg_act_diff.ResetAvg();
    trial_cos_diff = 0.0f;    avg_trial_cos_diff.ResetAvg();
    da_p = 0.0f;    da_n = 0.0f;    sev = 0.0f;
    net_sd = 0.0f;    avg_net_sd.ResetAvg();
    netin.InitVals(); netin_m.InitVals(); acts.InitVals(); acts_eq.InitVals();
    acts_m.InitVals(); acts_p.InitVals();  am_avg_l.InitVals(); am_avg_l_lrn.InitVals();
  }
  // initialize all the statistics values

  INLINE void   Initialize_lay_core() {
    hard_clamped = false;
    lrate_mod = 1.0f;    deep_lrate_mod = false;  adapt_gi = 1.0f; mod_avg_l_lrn = 0.0f;
    da_p = 0.0f;    da_n = 0.0f;    sev = 0.0f;
    acts_m_avg = acts_p_avg = acts_p_avg_eff = 0.15f;
    Init_Stats();
  }
  // #IGNORE 
