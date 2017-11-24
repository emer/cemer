// this contains core shared code, and is included directly in LeabraLayerSpec.h, _cpp.h, _cuda.h
//{
  STATE_CLASS(LeabraInhibSpec) lay_inhib;	// #CAT_Activation #AKA_inhib how to compute layer-wide inhibition -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- net inhibition is MAX of all operative inhibition -- any inhibitory unit inhibition is just added on top of this computed inhibition
  STATE_CLASS(LeabraInhibSpec) unit_gp_inhib; // #CAT_Activation how to compute unit-group-level inhibition (only relevant if layer actually has unit groups -- net inhibition is MAX of all operative inhibition -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- any inhibitory unit inhibition is just added on top of this computed inhibition
  STATE_CLASS(LeabraInhibSpec) lay_gp_inhib;	// #CAT_Activation inhibition computed across layers within layer groups -- only applicable if the layer actually lives in a subgroup with other layers (and only in a first-level subgroup, not a sub-sub-group) -- only the specs of the FIRST layer in the layer group are used for computing inhib -- net inhibition is MAX of all operative inhibition -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- any inhibitory unit inhibition is just added on top of this computed inhibition
  STATE_CLASS(LayerAvgActSpec) avg_act;	// #CAT_Activation expected average activity levels in the layer -- used for computing running-average computation that is then used for netinput scaling (also specifies time constant for updating average), and for the target value for adapting inhibition in inhib_adapt
  STATE_CLASS(LeabraAdaptInhib) inhib_adapt; // #CAT_Activation adapt an extra inhibitory gain value to keep overall layer activation within a given target range, based on avg_act.targ_init target value (TARGET or deep TRC layers use the running-average plus phase average actitvation) -- gain applies to all forms of inhibition (layer, unit group) that are in effect
  STATE_CLASS(LeabraInhibMisc) inhib_misc;	// #CAT_Activation extra parameters for special forms of inhibition beyond the basic FFFB dynamic specified in inhib
  STATE_CLASS(LeabraClampSpec) clamp;        // #CAT_Activation how to clamp external inputs to units (hard vs. soft)
  STATE_CLASS(LayerDecaySpec)  decay;        // #CAT_Activation decay of activity state vars between trials
  STATE_CLASS(LeabraDelInhib)  del_inhib;	// #CAT_Activation delayed inhibition, as a function of per-unit net input on prior trial and/or phase -- produces temporal derivative effects
  STATE_CLASS(LeabraActMargin) margin;	// #CAT_Activation marginal activation computation -- detects those units that are on the edges of an attractor and focuses extra learning on them
  float           lay_lrate;    // #CAT_Statistic layer-level learning rate modulator, multiplies learning rates for all connections coming into layer(s) that this spec applies to -- sets lrate_mod value on layer -- see also cos_diff for additional lrate modulation on top of this
  STATE_CLASS(LeabraCosDiffMod) cos_diff;    // #CAT_Statistic leabra layer-level cosine of difference between plus and minus phase activations -- used to modulate amount of hebbian learning, and overall learning rate
  STATE_CLASS(LeabraLayStats)  lstats;       // #CAT_Statistic layer-level statistics parameters


// Two levels of UnGpState:
  // GetLayUnGpState -- the unit group state for whole layer -- layer level data
  // UnGpState for unit groups: per unit group, replacing LeabraUnGpData from before

  ///////////////////////////////////////////////////////////////////////
  //	Access, status functions

  inline bool   HasUnitGpInhib(LEABRA_LAYER_STATE* lay)
  { return (unit_gp_inhib.on && lay->n_ungps > 0); }
  // does this layer have unit-group level inhibition?
  inline bool   HasLayerInhib()
  { return (lay_inhib.on); }
  // does this layer have layer level inhibition

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  // IMPORTANT: because we cannot have virtual functions in LayerState, any override of layer
  // compute methods MUST be accompanied by a rewrite of the parent NetworkState function

  INLINE virtual void  Init_Weights_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->bias_scale = 1.0f;
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    lgpd->Init_UnGp_State(avg_act.targ_init, avg_act.targ_init, avg_act.AvgEffInit());
    for(int g=0; g < lay->n_ungps; g++) {
      LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
      gpd->Init_UnGp_State(avg_act.targ_init, avg_act.targ_init, avg_act.AvgEffInit());
    }
    for(int i=0;i<lay->n_recv_prjns;i++) {
      LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(net, i);
      prjn->Init_Weights_State();
    }
    Init_AdaptInhib(lay, net);         // initialize inhibition at start..
  }
  // #CAT_Learning layer-level initialization taking place after Init_Weights on units
  // IMPORTANT: above requires special call by LeabraNetworkState -- not part of base Init_Weights
  
  INLINE virtual void  Init_AdaptInhib(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    lay->adapt_gi = 1.0f;
    lay->lrate_mod = lay_lrate;
    lay->margin.low_thr = margin.low_thr;
    lay->margin.med_thr = margin.med_thr;
    lay->margin.hi_thr = margin.hi_thr;
    float eff_p_avg = lgpd->acts_p_avg / margin.avg_act;
    lay->margin.low_avg = eff_p_avg;
    lay->margin.med_avg = margin.MedTarg(eff_p_avg);
    lay->margin.hi_avg = margin.HiTarg(eff_p_avg);
    lay->laygp_i_val.InitVals(); lay->laygp_netin.InitVals(); lay->laygp_acts_eq.InitVals();
  }
  // #CAT_Activation called in Init_Weights_Layer initialize the adaptive inhibitory state values
  
  INLINE virtual void  Init_Stats(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->Init_Stats();
    for(int i=0;i<lay->n_recv_prjns;i++) {
      LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(net, i);
      prjn->Init_Stats();
    }
  }
  // #CAT_Statistic called in Init_Weights_Layer intialize statistic variables

  INLINE virtual void  Init_Acts_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->ext_flag = LAYER_STATE::NO_EXTERNAL;
    lay->hard_clamped = false;
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    lgpd->UnGp_Init_Acts();
    for(int g=0; g < lay->n_ungps; g++) {
      LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
      gpd->UnGp_Init_Acts();
    }
  }    
  // #CAT_Activation initialize unit-level dynamic state variables (activations, etc)

  INIMPL virtual void  DecayState(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, float decay);
  // #CAT_Activation decay the state of this layer -- not normally called but available for programs etc to control specific layers

  ///////////////////////////////////////////////////////////////////////
  //	Trial_Init -- at start of trial

  INIMPL virtual void  Trial_Init_Specs(LEABRA_NETWORK_STATE* net);
  // #CAT_Learning initialize specs and specs update network flags
  
  INIMPL virtual void  Trial_Init_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_Learning layer level trial init -- overload where needed

  ///////////////////////////////////////////////////////////////////////
  //	Quarter_Init -- at start of settling

  INIMPL virtual float Compute_AvgExt(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_Activation compute average of unit ext or targ values, depending on ext flags
  
  INLINE virtual void  Quarter_Init_TargFlags_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net)  {
    if(lay->HasExtFlag(LAYER_STATE::TARG)) {     // only process target layers..
      if(net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE)
        lay->SetExtFlag(LAYER_STATE::EXT);
    }
  }
  // #IGNORE layer-level initialize start of a setting phase, set input flags appropriately, etc
  
  INLINE virtual void  Quarter_Init_AvgAct_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net)  {
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    if(avg_act.fixed) {
      lgpd->acts_p_avg_eff = avg_act.targ_init;
    }
    else if(lay->layer_type != LAYER_STATE::HIDDEN && avg_act.use_ext_act) {
      if(!(lay->HasExtFlag(LAYER_STATE::EXT) || lay->HasExtFlag(LAYER_STATE::TARG))) {
        lgpd->acts_p_avg_eff = avg_act.targ_init;
      }
      else {
        float avg_ext = Compute_AvgExt(lay, net);
        if(avg_ext == 0.0f) {
          lgpd->acts_p_avg_eff = avg_act.targ_init;
        }
        else {
          lgpd->acts_p_avg_eff = avg_ext;
        }
      }
    }
  }
  // #IGNORE layer-level init avg_act based on fixed, use_ext_act

  INLINE virtual void  Quarter_Init_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net)  {
    Quarter_Init_TargFlags_Layer(lay, net);
    Quarter_Init_AvgAct_Layer(lay, net);
  }
  // #CAT_Activation initialize start of a setting phase: all layer-level misc init takes place here (calls TargFlags_Layer) -- other stuff all done directly in Quarter_Init_Units call


  INLINE virtual void  Compute_HardClamp_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    if(!(clamp.hard && lay->HasExtFlag(LAYER_STATE::EXT))) {
      lay->hard_clamped = false;
      return;
    }
    lay->hard_clamped = true;     // cache this flag
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    lgpd->UnGp_SetVals(0.5f);            // assume 0 - 1 clamped inputs
  }
  // #CAT_Activation prior to settling: hard-clamp inputs

  INLINE virtual void  Quarter_Init_Layer_Post(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {  };
  // #CAT_Activation extra layer-level step -- last thing in Quarter_Init sequence -- e.g., ScalarVal uses this for clamping actual activation pattern..

  INLINE virtual void  ExtToComp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    if(!lay->HasExtFlag(LAYER_STATE::EXT))       // only process ext
      return;
    lay->ext_flag = LAYER_STATE::COMP;   // totally reset to comparison
  }
  // #CAT_Activation change external inputs to comparisons (remove input)
  
  INLINE virtual void  TargExtToComp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    if(!lay->HasExtFlag(LAYER_STATE::TARG_EXT))  // only process w/ external input
      return;
    lay->ext_flag = LAYER_STATE::COMP;   // totally reset to comparison
  }
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  // main computation is direct Send_NetinDelta call on units through threading mechanism
  // followed by Compute_NetinInteg on units


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  INLINE virtual void  Compute_Inhib_FfFb
    (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, STATE_CLASS_CPP(LeabraInhibVals)& i_val, const float netin_avg,
     const float netin_max, const float acts_avg, STATE_CLASS(LeabraInhibSpec)& ispec) {
    if(!ispec.on) {
      i_val.ffi = 0.0f;
      i_val.fbi = 0.0f;
      i_val.g_i = 0.0f;
      return;
    }

    float nw_ffi = ispec.FFInhib(netin_avg, netin_max);
    float nw_fbi = ispec.FBInhib(acts_avg);

    i_val.ffi = nw_ffi;

    // dt only on fbi part
    if(inhib_misc.fb_up_immed) {
      if(nw_fbi > i_val.fbi) { // up_immed case --- best for spiking
        i_val.fbi = nw_fbi;
      }
      else {
        ispec.FBUpdt(i_val.fbi, nw_fbi);
      }
    }
    else {
      ispec.FBUpdt(i_val.fbi, nw_fbi);
    }

    i_val.g_i = lay->adapt_gi * ispec.gi * (nw_ffi + i_val.fbi); // combine
    i_val.g_i_orig = i_val.g_i; // retain original values..
  }
  // #IGNORE implementation of feed-forward, feed-back inhibition computation

  INLINE virtual void  Compute_Inhib(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    if(lay->hard_clamped) return; // say no more..

    if(HasUnitGpInhib(lay)) {
      for(int g=0; g < lay->n_ungps; g++) {
        LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
        Compute_Inhib_FfFb(lay, net, gpd->i_val, gpd->netin.avg, gpd->netin.max,
                           gpd->acts.avg, unit_gp_inhib);
      }
    }
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    if(HasLayerInhib()) {
      Compute_Inhib_FfFb(lay, net, lgpd->i_val, lgpd->netin.avg, lgpd->netin.max,
                         lgpd->acts.avg, lay_inhib);
    }
    else {                        // initialize lay inhib -- otherwise it will interfere!
      lgpd->i_val.ffi = 0.0f;
      lgpd->i_val.fbi = 0.0f;
      lgpd->i_val.g_i = 0.0f;
    }
    Compute_LayInhibToGps(lay, net); // sync it all up..
  }
  // #CAT_Activation compute the inhibition for layer -- this is the main call point into this stage of processing
  
  INLINE virtual void  Compute_LayInhibToGps(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    if(lay_gp_inhib.on) {
      lgpd->i_val.g_i = fmaxf(lgpd->i_val.g_i, lay->laygp_i_val.g_i);
    }
  
    if(lay->n_ungps == 0) return;
  
    if(unit_gp_inhib.on) {
      for(int g=0; g < lay->n_ungps; g++) {
        LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
        gpd->i_val.lay_g_i = lgpd->i_val.g_i;
        gpd->i_val.g_i = fmaxf(gpd->i_val.g_i, lgpd->i_val.g_i);
      }
    }
    else {                        // always do this..
      // propagate layer-level g_i to all subgroups 
      for(int g=0; g < lay->n_ungps; g++) {
        LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
        gpd->i_val.g_i = lgpd->i_val.g_i;
      }
    }
  }
  // #CAT_Activation Stage 2.2: for layer groups, need to propagate inhib out to unit groups

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  // called directly on units through threading mechanism

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  INLINE virtual void  Compute_CycleStats_Pre(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) { };
  // #CAT_Statistic pre-cycle-stats -- done in single thread prior to cycle stats -- good place to intervene for whole-layer dynamics

  ///////////////////////////////////////////////////////////////////////
  //	Quarter_Final

  INLINE virtual void  Quarter_Final_Pre(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) { };
  // #CAT_Activation perform computations in layers at end of settling -- this is a pre-stage that occurs prior to final Quarter_Final -- use this for anything that needs to happen prior to the standard Quarter_Final across layers (called by network Quarter_Final)

  INLINE virtual void Quarter_Final_GetMinus_UnGp(LEABRA_UNGP_STATE* gpd, LEABRA_NETWORK_STATE* net) {
    gpd->acts_m = gpd->acts_eq;
    if(avg_act.use_first && gpd->acts_m_avg == avg_act.targ_init) {
      gpd->acts_m_avg += 0.5f * (gpd->acts_m.avg - gpd->acts_m_avg);
    }
    else {
      gpd->acts_m_avg += avg_act.dt * (gpd->acts_m.avg - gpd->acts_m_avg);
    }
  }
  // #IGNORE

  INLINE virtual void Quarter_Final_GetMinus(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    Quarter_Final_GetMinus_UnGp(lgpd, net);
    for(int g=0; g < lay->n_ungps; g++) {
      LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
      Quarter_Final_GetMinus_UnGp(gpd, net);
    }
  }
  // #CAT_Activation get minus phase act stats
  
  INLINE virtual void Quarter_Final_GetPlus_UnGp(LEABRA_UNGP_STATE* gpd, LEABRA_NETWORK_STATE* net) {
    gpd->acts_p = gpd->acts_eq;
    if(avg_act.use_first && gpd->acts_p_avg == avg_act.targ_init) {
      gpd->acts_p_avg += 0.5f * (gpd->acts_p.avg - gpd->acts_p_avg);
    }
    else {
      gpd->acts_p_avg += avg_act.dt * (gpd->acts_p.avg - gpd->acts_p_avg);
    }
    if(avg_act.fixed) {
      gpd->acts_p_avg_eff = avg_act.targ_init;
    }
    else if(avg_act.use_ext_act) {
      // nop -- already set during Quarter_Init
    }
    else {
      gpd->acts_p_avg_eff = avg_act.adjust * gpd->acts_p_avg;
    }
  }
  // #IGNORE

  INLINE virtual void Quarter_Final_GetPlus(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net)  {
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    Quarter_Final_GetPlus_UnGp(lgpd, net);
    for(int g=0; g < lay->n_ungps; g++) {
      LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
      Quarter_Final_GetPlus_UnGp(gpd, net);
    }
  }
  // #CAT_Activation get plus phase act stats

  INLINE virtual void  Quarter_Final_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    lgpd->acts_prvq = lgpd->acts_eq;
    for(int g=0; g < lay->n_ungps; g++) {
      LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
      gpd->acts_prvq = gpd->acts_eq;
    }
    if(net->quarter == 2) {       // quarter still pre-increment?
      Quarter_Final_GetMinus(lay, net);
    }
    else if(net->quarter == 3) {
      Quarter_Final_GetPlus(lay, net);
    }
  }
  // #CAT_Activation after settling, keep track of phase variables, etc.
  
  ///////////////////////////////////////////////////////////////////////
  //	Learning

  INLINE virtual void  Compute_dWt_Layer_pre(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) { };
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  ///////////////////////////////////////////////////////////////////////
  //	Trial-level Stats

  INLINE virtual float  Compute_SSE(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                    int& n_vals, bool unit_avg = false, bool sqrt = false) {
    // use default, but allow subclasses to override in layerspec
    float rval = net->NETWORK_STATE::Compute_SSE_Layer(lay, n_vals, unit_avg, sqrt);
    lay->bin_err = 0.0f;
    if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG)) return rval;
    if(lay->layer_type == LAYER_STATE::HIDDEN) return rval;
    lay->bin_err = (lay->sse > net->stats.cnt_err_tol) ? 1.0f : 0.0f;
    return rval;
  }
  // #CAT_Statistic compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value
  // IMPORTANT: above requires special call by LeabraNetworkState to override base Layer function!

  INLINE virtual float  Compute_MaxErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    lay->max_err = 0.0f;
    if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG)) return 0.0f;
    if(HasUnitGpInhib(lay)) {
      int merr_sum = 0;
      for(int g=0; g < lay->n_ungps; g++) {
        LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
        bool max_err = true;
        if(gpd->acts_m.max_i >= 0) {
          LEABRA_UNIT_STATE* un = net->GetUnitState(gpd->acts_m.max_i);
          max_err = (un->targ < 0.1f);
        }
        gpd->max_err = (float)max_err;
        merr_sum += (int)max_err;
      }
      lay->max_err = (merr_sum > 0);
    }
    else {
      bool max_err = true;
      if(lgpd->acts_m.max_i >= 0) {
        LEABRA_UNIT_STATE* un = net->GetUnitState(lgpd->acts_m.max_i);
        max_err = (un->targ < 0.1f);
      }
      lay->max_err = (float)max_err;
    }
    return lay->max_err;
  }
  // #CAT_Statistic compute max_err, across unit groups (if present and used) and the entire layer

  INLINE virtual float  Compute_NormErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->norm_err = -1.0f;        // assume not contributing
    if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG)) return -1.0f; // indicates not applicable
    if(lay->layer_type == LAYER_STATE::HIDDEN) return -1.0f;

    float nerr = 0.0f;
    int ntrg_act = 0;

    const int li = lay->layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& lay_nerr = net->ThrLayStats(thr_no, li, 0, LEABRA_NETWORK_STATE::NORMERR);
      float& lay_trg_n = net->ThrLayStats(thr_no, li, 1, LEABRA_NETWORK_STATE::NORMERR);

      nerr += lay_nerr;
      ntrg_act += (int)lay_trg_n;
    }

    if(net->lstats.on_errs && net->lstats.off_errs)
      ntrg_act *= 2;              // double count

    if(ntrg_act == 0) return -1.0f;

    lay->norm_err = nerr / (float)ntrg_act;
    if(lay->norm_err > 1.0f) lay->norm_err = 1.0f; // shouldn't happen...

    lay->avg_norm_err.Increment(lay->norm_err);
  
    if(lay->HasLayerFlag(LAYER_STATE::NO_ADD_SSE) ||
       (lay->HasExtFlag(LAYER_STATE::COMP) && lay->HasLayerFlag(LAYER_STATE::NO_ADD_COMP_SSE)))
      return -1.0f;               // no contributarse

    return lay->norm_err;
  }
  // #CAT_Statistic compute normalized binary error of unit targ vs. act_m -- layer-level value is already normalized, and network just averages across the layers (each layer contributes equally to overal normalized value, instead of contributing in proportion to number of units) -- returns -1 if not an err target defined in same way as sse -- per unit: if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0; normalization is based on k value per layer: total possible err for both on and off errs is 2 * k (on or off alone is just k)

  INLINE virtual float  Compute_CosErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int& n_vals)  {
    lay->cos_err = 0.0f;
    lay->cos_err_prv = 0.0f;
    lay->cos_err_vs_prv = 0.0f;
    n_vals = 0;
    if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG)) return 0.0f;
    if(lay->layer_type == LAYER_STATE::HIDDEN) return 0.0f;
    float cosv = 0.0f;
    float cosvp = 0.0f;
    float ssm = 0.0f;
    float ssp = 0.0f;
    float sst = 0.0f;
    const int li = lay->layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& lcosv = net->ThrLayStats(thr_no, li, 0, LEABRA_NETWORK_STATE::COSERR);
      float& lcosvp = net->ThrLayStats(thr_no, li, 1, LEABRA_NETWORK_STATE::COSERR);
      float& lssm = net->ThrLayStats(thr_no, li, 2, LEABRA_NETWORK_STATE::COSERR);
      float& lssp = net->ThrLayStats(thr_no, li, 3, LEABRA_NETWORK_STATE::COSERR);
      float& lsst = net->ThrLayStats(thr_no, li, 4, LEABRA_NETWORK_STATE::COSERR);
      float& lnvals = net->ThrLayStats(thr_no, li, 5, LEABRA_NETWORK_STATE::COSERR);

      n_vals += lnvals;
      cosv += lcosv;
      ssm += lssm;
      sst += lsst;
      if(net->net_misc.deep) {
        cosvp += lcosvp;
        ssp += lssp;
      }
    }
    if(n_vals == 0) return 0.0f;
    float dist = sqrtf(ssm * sst);
    if(dist != 0.0f)
      cosv /= dist;
    lay->cos_err = cosv;

    lay->avg_cos_err.Increment(lay->cos_err);

    if(net->net_misc.deep) {
      float pdist = sqrtf(ssp * sst);
      if(pdist != 0.0f) {
        cosvp /= pdist;
      }
      lay->cos_err_prv = cosvp;
      lay->cos_err_vs_prv = lay->cos_err - lay->cos_err_prv;

      lay->avg_cos_err_prv.Increment(lay->cos_err_prv);
      lay->avg_cos_err_vs_prv.Increment(lay->cos_err_vs_prv);
    }
    if(lay->HasLayerFlag(LAYER_STATE::NO_ADD_SSE) ||
       (lay->HasExtFlag(LAYER_STATE::COMP) && lay->HasLayerFlag(LAYER_STATE::NO_ADD_COMP_SSE))) {
      n_vals = 0;
      return 0.0f;
    }
    return cosv;
  }
  // #CAT_Statistic compute cosine (normalized dot product) of target compared to act_m over the layer -- n_vals is number of units contributing

  INLINE virtual float  Compute_CosDiff(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net)  {
    lay->cos_diff = 0.0f;
    float cosv = 0.0f;
    float ssm = 0.0f;
    float sst = 0.0f;

    const int li = lay->layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& lcosv = net->ThrLayStats(thr_no, li, 0, LEABRA_NETWORK_STATE::COSDIFF);
      float& lssm = net->ThrLayStats(thr_no, li, 1, LEABRA_NETWORK_STATE::COSDIFF);
      float& lsst = net->ThrLayStats(thr_no, li, 2, LEABRA_NETWORK_STATE::COSDIFF);

      cosv += lcosv;
      ssm += lssm;
      sst += lsst;
    }
    float dist = sqrtf(ssm * sst);
    if(dist != 0.0f)
      cosv /= dist;
    lay->cos_diff = cosv;

    cos_diff.UpdtDiffAvgVar(lay->cos_diff_avg, lay->cos_diff_var, lay->cos_diff);
    lay->lrate_mod = lay_lrate;

    if(cos_diff.lrate_mod && !cos_diff.lrmod_fm_trc) {
      lay->lrate_mod *= cos_diff.CosDiffLrateMod(lay->cos_diff, lay->cos_diff_avg,
                                                 lay->cos_diff_var);
      if(cos_diff.set_net_unlrn && lay->lrate_mod == 0.0f) {
        net->unlearnable_trial = true;
      }
    }
  
    if(lay->layer_type == LAYER_STATE::HIDDEN) {
      lay->cos_diff_avg_lrn = 1.0f - lay->cos_diff_avg;
    }
    else {
      lay->cos_diff_avg_lrn = 0.0f; // no mix for TARGET layers; irrelevant for INPUT
    }

    lay->avg_cos_diff.Increment(lay->cos_diff);
  
    return cosv;
  }
  // #CAT_Statistic compute cosine (normalized dot product) of phase activation difference in this layer: act_p compared to act_m -- must be called after Quarter_Final for plus phase to get the act_p values
  
  INIMPL virtual void   Compute_CosDiff_post(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_Statistic post step of cos_diff -- needed for sharing cos_diff based lrate mod
  
  INLINE virtual float  Compute_AvgActDiff(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->avg_act_diff = 0.0f;
    float adiff = 0.0f;
    int nd = 0;

    const int li = lay->layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& ladiff = net->ThrLayStats(thr_no, li, 0, LEABRA_NETWORK_STATE::AVGACTDIFF);
      float& lnd = net->ThrLayStats(thr_no, li, 1, LEABRA_NETWORK_STATE::AVGACTDIFF);
      adiff += ladiff;
      nd += (int)lnd;
    }
  
    if(nd > 0)
      adiff /= (float)nd;
    lay->avg_act_diff = adiff;

    lay->avg_avg_act_diff.Increment(lay->avg_act_diff);
  
    return adiff;
  }
  // #CAT_Statistic compute average act_diff (act_p - act_m) for this layer -- must be called after Quarter_Final for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases
  
  INLINE virtual float  Compute_TrialCosDiff(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->trial_cos_diff = 0.0f;
    float cosv = 0.0f;
    float ssm = 0.0f;
    float sst = 0.0f;

    const int li = lay->layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& lcosv = net->ThrLayStats(thr_no, li, 0, LEABRA_NETWORK_STATE::TRIALCOSDIFF);
      float& lssm = net->ThrLayStats(thr_no, li, 1, LEABRA_NETWORK_STATE::TRIALCOSDIFF);
      float& lsst = net->ThrLayStats(thr_no, li, 2, LEABRA_NETWORK_STATE::TRIALCOSDIFF);

      cosv += lcosv;
      ssm += lssm;
      sst += lsst;
    }
  
    float dist = sqrtf(ssm * sst);
    if(dist != 0.0f)
      cosv /= dist;
    lay->trial_cos_diff = cosv;

    lay->avg_trial_cos_diff.Increment(lay->trial_cos_diff);
  
    return cosv;
  }
  // #CAT_Statistic compute cosine (normalized dot product) of trial activation difference in this layer: act_q4 compared to act_q0 -- must be called after Quarter_Final for plus phase to get the act_q4 values
  
  INLINE virtual void   Compute_ActMargin(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    float low_avg = 0.0f;
    float med_avg = 0.0f;
    float hi_avg = 0.0f;

    if(lay->n_units <= 1) return;
  
    const int li = lay->layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& lowv = net->ThrLayStats(thr_no, li, 0, LEABRA_NETWORK_STATE::ACTMARGIN);
      float& medv = net->ThrLayStats(thr_no, li, 1, LEABRA_NETWORK_STATE::ACTMARGIN);
      float& hiv = net->ThrLayStats(thr_no, li, 2, LEABRA_NETWORK_STATE::ACTMARGIN);

      low_avg += lowv;
      med_avg += medv;
      hi_avg += hiv;
    }

    low_avg /= (float)lay->n_units;
    med_avg /= (float)lay->n_units;
    hi_avg /= (float)lay->n_units;

    margin.IncrAvgVal(lay->margin.low_avg, low_avg);
    margin.IncrAvgVal(lay->margin.med_avg, med_avg);
    margin.IncrAvgVal(lay->margin.hi_avg, hi_avg);

    float eff_p_avg = lgpd->acts_p_avg / margin.avg_act;
  
    float low_del = margin.AdaptThr(lay->margin.low_thr, lay->margin.low_avg, eff_p_avg, 1.0f);
    // sign = same direction as diff - if avg > targ, increase thr
    lay->margin.med_thr += low_del; // med automatically tracks low!
    margin.AdaptThr(lay->margin.med_thr, lay->margin.med_avg,
                    margin.MedTarg(eff_p_avg), -1.0f);
    // sign = opposite direction as diff - if avg > targ, *decrease* thr
    margin.AdaptThr(lay->margin.hi_thr, lay->margin.hi_avg,
                    margin.HiTarg(eff_p_avg), 1.0f);
    // sign = same direction as diff - if avg > targ, *increase* thr
    // preserve ordering!
    if(lay->margin.med_thr < lay->margin.low_thr) {
      lay->margin.med_thr = lay->margin.low_thr + 0.001f;
    }
    if(lay->margin.hi_thr < lay->margin.med_thr) {
      lay->margin.hi_thr = lay->margin.med_thr + 0.001f;
    }
  }
  // #CAT_Statistic compute activation margin stats and adapt thresholds
  
  INLINE virtual float   Compute_NetSd(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->net_sd = 0.0f;
    float var = 0.0f;

    const int li = lay->layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& lvar = net->ThrLayStats(thr_no, li, 0, LEABRA_NETWORK_STATE::NETSD);
      var += lvar;
    }
    lay->net_sd = sqrtf(var);
    lay->avg_net_sd.Increment(lay->net_sd);
    return var;
  }
  // #CAT_Statistic compute standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail -- must be called at end of minus phase
  
  INLINE virtual void   Compute_HogDeadPcts(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->hog_pct = 0.0f;
    lay->dead_pct = 0.0f;
    float hog = 0.0f;
    float dead = 0.0f;
    float nu = 0.0f;

    const int li = lay->layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& lhog = net->ThrLayStats(thr_no, li, 0, LEABRA_NETWORK_STATE::HOGDEAD);
      float& ldead = net->ThrLayStats(thr_no, li, 1, LEABRA_NETWORK_STATE::HOGDEAD);
      float& lnu = net->ThrLayStats(thr_no, li, 2, LEABRA_NETWORK_STATE::HOGDEAD);

      hog += lhog;
      dead += ldead;
      nu += lnu;
    }

    if(nu > 0.0f) {
      lay->hog_pct = hog / nu;
      lay->dead_pct = dead / nu;
    }
  }
  // #CAT_Statistic compute percentage of units in the layer that have a long-time-averaged activitation level that is above or below hog / dead thresholds, indicating that they are either 'hogging' the representational space, or 'dead' and not participating in any representations

  INLINE virtual void  Compute_AvgNormErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->avg_norm_err.GetAvg_Reset();
  }
  // #CAT_Statistic compute average norm_err (at an epoch-level timescale)
  INLINE virtual void  Compute_AvgCosErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->avg_cos_err.GetAvg_Reset();

    if(net->net_misc.deep) {
      lay->avg_cos_err_prv.GetAvg_Reset();
      lay->avg_cos_err_vs_prv.GetAvg_Reset();
    }
  }
  // #CAT_Statistic compute average cos_err (at an epoch-level timescale)
  INLINE virtual void  Compute_AvgCosDiff(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->avg_cos_diff.GetAvg_Reset();
  }
  // #CAT_Statistic compute average cos_diff (at an epoch-level timescale)
  INLINE virtual void  Compute_AvgTrialCosDiff(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->avg_trial_cos_diff.GetAvg_Reset();
  }
  // #CAT_Statistic compute average trial_cos_diff (at an epoch-level timescale)
  INLINE virtual void  Compute_AvgAvgActDiff(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->avg_avg_act_diff.GetAvg_Reset();
  }
  // #CAT_Statistic compute average avg_act_diff (at an epoch-level timescale)
  INLINE virtual void  Compute_AvgNetSd(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    lay->avg_net_sd.GetAvg_Reset();
  }
  // #CAT_Statistic compute average net_sd (at an epoch-level timescale)

  INLINE virtual void  Compute_EpochStats(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    net->NETWORK_STATE::Compute_EpochStats_Layer(lay);
    Compute_AvgNormErr(lay, net);
    Compute_AvgCosErr(lay, net);
    Compute_AvgCosDiff(lay, net);
    Compute_AvgAvgActDiff(lay, net);
    Compute_AvgTrialCosDiff(lay, net);
    Compute_AvgNetSd(lay, net);
  }
  // #CAT_Statistic compute epoch-level statistics (averages)
  // IMPORTANT: above requires special call by LeabraNetworkState to override base Layer function!

  INIMPL virtual void Compute_AbsRelNetin(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into this layer

  INIMPL virtual void Compute_AvgAbsRelNetin(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_Statistic compute time-average relative netinput from different projections into this layer (e.g., every epoch)

  INIMPL virtual void ClearDeepActs(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_Deep clear the deep layer variables -- can be useful to do at discontinuities of experience


  INLINE void   Initialize_core() {
    unit_gp_inhib.on = false;
    lay_gp_inhib.on = false;
    lay_lrate = 1.0f;
  }

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LeabraLayerSpec; }

