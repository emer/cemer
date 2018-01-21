// this contains core shared code, and is included directly in LeabraConSpec.h, _cpp.h, _cuda.h
//{

  enum ActFun {
    NOISY_XX1,                 // x over x plus 1 convolved with Gaussian noise (noise is nvar)
    SPIKE,                     // discrete spiking activations (spike when > thr) -- default params produce adaptive exponential (AdEx) model
    SIGMOID,                   // simple sigmoidal function of net input -- e.g., for decoder layers
  };

  enum Quarters {               // #BITS specifies gamma frequency quarters within an alpha-frequency trial on which to do things
    QNULL = 0x00,              // #NO_BIT no quarter (yeah..)
    Q1 = 0x01,                 // first quarter
    Q2 = 0x02,                 // second quarter
    Q3 = 0x04,                 // third quarter -- posterior cortical minus phase
    Q4 = 0x08,                 // fourth quarter -- posterior cortical plus phase
    Q2_Q4 = Q2 | Q4,           // #NO_BIT standard beta frequency option, for pfc
    Q1_Q3 = Q1 | Q3,           // #NO_BIT standard beta frequency option, for bg
    QALL = Q1 | Q2 | Q3 | Q4,  // #NO_BIT all quarters
  };

  ActFun            act_fun;        // #CAT_Activation activation function to use -- typically NOISY_XX1 or SPIKE -- others are for special purposes or testing
  STATE_CLASS(LeabraActFunSpec)  act;            // #CAT_Activation activation function parameters -- very important for determining the shape of the selected act_fun
  STATE_CLASS(LeabraActMiscSpec) act_misc;       // #CAT_Activation miscellaneous activation parameters
  STATE_CLASS(SpikeFunSpec)     spike;           // #CONDSHOW_ON_act_fun:SPIKE #CAT_Activation spiking function specs (only for act_fun = SPIKE)
  STATE_CLASS(SpikeMiscSpec)    spike_misc;      // #CONDSHOW_ON_act_fun:SPIKE #CAT_Activation misc extra spiking function specs (only for act_fun = SPIKE)
  STATE_CLASS(OptThreshSpec)    opt_thresh;      // #CAT_Learning optimization thresholds for speeding up processing when units are basically inactive
  STATE_CLASS(MinMaxRange)      clamp_range;     // #CAT_Activation range of clamped activation values (min, max, 0, .95 std), don't clamp to 1 because acts can't reach, so .95 instead
  STATE_CLASS(MinMaxRange)      vm_range;        // #CAT_Activation membrane potential range (min, max, 0-2 for normalized)
  STATE_CLASS(LeabraInitSpec)   init;            // #CAT_Activation initial starting values for various key neural parameters
  STATE_CLASS(LeabraDtSpec)     dt;              // #CAT_Activation time constants (rate of updating): membrane potential (vm) and net input (net)
  STATE_CLASS(LeabraActAvgSpec) act_avg;         // #CAT_Learning time constants (rate of updating) for computing activation averages -- used in XCAL learning rules
  STATE_CLASS(LeabraAvgLSpec)   avg_l;           // #CAT_Learning parameters for computing the avg_l long-term floating average that drives BCM-style hebbian learning
  STATE_CLASS(LeabraAvgL2Spec)  avg_l_2;         // #CAT_Learning additional parameters for computing the avg_l long-term floating average that drives BCM-style hebbian learning
  STATE_CLASS(LeabraChannels)   g_bar;           // #CAT_Activation [Defaults: 1, .1, 1] maximal conductances for channels
  STATE_CLASS(LeabraChannels)   e_rev;           // #CAT_Activation [Defaults: 1, .3, .25] reversal potentials for each channel
  STATE_CLASS(KNaAdaptSpec)     kna_adapt;       // #CAT_Activation sodium-gated potassium channel adaptation mechanism -- evidence supports at least 3 different time constants: M-type (fast), Slick (medium), and Slack (slow)
  STATE_CLASS(KNaAdaptMiscSpec) kna_misc;         // #CAT_Activation #CONDSHOW_ON_kna_adapt.on misc extra params for sodium-gated potassium channel adaptation mechanism
  STATE_CLASS(ShortPlastSpec)   stp;             // #CAT_Activation short term presynaptic plasticity specs -- can implement full range between facilitating vs. depresssion
  STATE_CLASS(SynDelaySpec)     syn_delay;       // #CAT_Activation synaptic delay -- if active, activation sent to other units is delayed by a given amount
  Quarters         deep_raw_qtr;    // #CAT_Learning #AKA_deep_qtr quarter(s) during which deep_raw layer 5 intrinsic bursting activations should be updated -- deep_raw is updated and sent to deep_raw_net during this quarter, and deep_ctxt is updated right after this quarter (wrapping around to the first quarter for the 4th quarter)
  STATE_CLASS(DeepSpec)         deep;            // #CAT_Learning specs for DeepLeabra deep neocortical layer dynamics, which capture attentional, thalamic auto-encoder, and temporal integration mechanisms 
  STATE_CLASS(TRCSpec)          trc;             // #CAT_Learning #CONDSHOW_ON_deep.on&&deep.role:TRC specs for DeepLeabra TRC thalamic relay cells
  STATE_CLASS(DaModSpec)        da_mod;          // #CAT_Learning da modulation of activations (for da-based learning, and other effects)
  STATE_CLASS(LeabraNoiseSpec)  noise_type;      // #CAT_Activation random noise in the processing parameters
  STATE_CLASS(Random)           noise;           // #CONDSHOW_OFF_noise_type.type:NO_NOISE #CAT_Activation distribution parameters for random added noise
  
  STATE_CLASS(LeabraChannels) e_rev_sub_thr;     // #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN e_rev - act.thr for each item -- used for compute_ithresh
  float          thr_sub_e_rev_i;   // #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN (act.thr - e_rev.i) used for compute_ithresh -- does NOT include g_bar.i
  float          thr_sub_e_rev_e;   // #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN g_bar.e * (act.thr - e_rev.e) used for compute_ethresh

  
  INLINE LEABRA_CON_SPEC_CPP* GetBiasSpec(NETWORK_STATE* net) {
    return (LEABRA_CON_SPEC_CPP*)inherited::GetBiasSpec(net); 
  }

  ///////////////////////////////////////////////////////////////////////
  //        General Init functions

  INLINE void  Init_InputData(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override {
    inherited::Init_InputData(uv, net, thr_no);
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
    u->ext_orig = 0.0f;
  }

  INLINE virtual void Init_Netins(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    u->act_sent = 0.0f;
    u->net_raw = 0.0f;
    u->gi_raw = 0.0f;
    // u->gi_syn = 0.0f;
    // u->net = 0.0f;
    u->deep_raw_net = 0.0f;
    u->deep_mod_net = 0.0f;
    u->deep_raw_sent = 0.0f;
    const int nrg = u->NRecvConGps(net); 
    for(int g=0; g< nrg; g++) {
      LEABRA_CON_STATE* recv_gp = u->RecvConState(net, g);
      recv_gp->net = 0.0f;
      recv_gp->net_raw = 0.0f;
    }
  }
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)
    
  INIMPL void  Init_Acts(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override;
  
  INIMPL void  Init_UnitState(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override;

  INLINE virtual void Init_ActAvg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(act_misc.avg_trace) {
      u->act_avg = 0.0f;
    }
    else {
      u->act_avg = act_misc.avg_init;
    }
    u->avg_l = avg_l.init;
    u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
  }
  // #CAT_Activation initialize average activation values, used to control learning
  
  INLINE void  Init_Weights(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override {
    inherited::Init_Weights(uv, net, thr_no);
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
    
    u->net_prv_q = 0.0f;
    u->net_prv_trl = 0.0f;
    u->misc_1 = 0.0f;
    u->misc_2 = 0.0f;

    Init_ActAvg(u, (LEABRA_NETWORK_STATE*)net, thr_no);
  }

  INLINE void LoadBiasWtVal(float bwt, UNIT_STATE* uv, NETWORK_STATE* net) override {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
    u->bias_wt = bwt;
    u->bias_fwt = bwt;
    u->bias_swt = bwt;
  }

  
  INIMPL virtual void DecayState(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)
  
  INLINE virtual void ResetSynTR(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    u->syn_tr = 1.0;
    u->syn_kre = 0.0;
  }
  // #IGNORE


  ///////////////////////////////////////////////////////////////////////
  //        TrialInit -- at start of trial

  INLINE virtual void Trial_Init_Specs(LEABRA_NETWORK_STATE* net) {
    if(act_fun == SPIKE) {
      net->net_misc.spike = true;
    }
    else {
      if(net->net_misc.spike) {
        net->StateError("Trial_Init_Specs:",
                        "detected a mix of SPIKE and NOISY_XX1 activation functions -- due to code optimizations, must all be either one or the other!");
      }
    }
    if(deep.on) {
      net->deep.on = true;
      net->deep.raw_qtr = (STATE_CLASS_CPP(LeabraNetDeep)::Quarters)(net->deep.raw_qtr | deep_raw_qtr);
    }
  }
  // #CAT_Learning initialize specs and specs update network flags 

  INLINE virtual void Trial_Init_Unit(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    Trial_STP_TrialBinary_Updt(u, net, thr_no); //do this b4 decay, because using act_q3 from previous trial
    Trial_Init_PrvVals(u, net, thr_no);   // do this b4 decay, so vals are intact
    Trial_Init_SRAvg(u, net, thr_no);     // do this b4 decay..
    Trial_DecayState(u, net, thr_no);
    Trial_NoiseInit(u, net, thr_no);
  }
  
  INLINE virtual void Trial_Init_PrvVals(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    u->net_prv_trl = u->net; 
    u->act_q0 = u->act_q4;
  }
  // #CAT_Learning save previous trial values at start of new trial -- allow values at end of trial to be valid for visualization..
  
  INLINE virtual void Trial_Init_SRAvg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    if(lgpd->acts_p_avg >= avg_l_2.lay_act_thr) {
      avg_l.UpdtAvgL(u->avg_l, u->avg_m);
    }
    u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
    if(avg_l_2.err_mod) {
      float eff_err = fmaxf(lay->cos_diff_avg_lrn, avg_l_2.err_min);
      u->avg_l_lrn *= eff_err;
    }
    if((lay->layer_type != LAYER_STATE::HIDDEN) || deep.IsTRC()) {
      u->avg_l_lrn = 0.0f;        // no self organizing in non-hidden layers!
    }
  }
  // #CAT_Learning reset the sender-receiver coproduct average -- call at start of trial
    
  INIMPL virtual void Trial_DecayState(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Activation decay activation states towards initial values: at trial-level boundary

  INLINE virtual void Trial_NoiseInit(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(noise_type.type != STATE_CLASS(LeabraNoiseSpec)::NO_NOISE && noise_type.trial_fixed &&
       (noise.type != STATE_CLASS(Random)::NONE)) {
      u->noise = noise.Gen(thr_no);
    }
  }
  // #CAT_Activation init trial-level noise -- ONLY called if noise_type.trial_fixed is set

  INLINE virtual void Trial_STP_TrialBinary_Updt(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!stp.on || stp.algorithm != STATE_CLASS(ShortPlastSpec)::TRIAL_BINARY) return;

    if (u->syn_tr > 0.0f) { // if the unit isn't currently depressed
      if (u->act_q3 > stp.thresh) {
        u->syn_kre += 1.0f;       // note: ++ not generally defined for floats
      }
      else {
        u->syn_kre = 0.0f;
      }
      if (u->syn_kre >= stp.n_trials) {
        u->syn_tr  = 0.0f;
      }
    }
    else { // this unit is currently depressed
      bool recover = STATE_CLASS(Random)::BoolProb(stp.rec_prob, thr_no);
      if (recover) {
        u->syn_tr = 1.0f;
        u->syn_kre = 0.0f;
      }
    }
  }
  // #CAT_Activation calculates short term depression of units by a trial by trial basis
  
  INLINE void ApplyInputData(UNIT_STATE* u, NETWORK_STATE* net, float val,
                             UnitState_cpp::ExtFlags act_ext_flags, bool na_by_range) override {
    inherited::ApplyInputData(u, net, val, act_ext_flags, na_by_range);
    ApplyInputData_post((LEABRA_UNIT_STATE*)u, net);
  }
  // #CAT_Activation apply input data value according to ext flags

  INLINE virtual void ApplyInputData_post(LEABRA_UNIT_STATE* u, NETWORK_STATE* net) {
    if(!u->HasExtFlag(UNIT_STATE::EXT))
      return;
    u->ext_orig = u->ext;
  }
  // #CAT_Activation post-apply input data -- cache the ext value b/c it might get overwritten in transforms of the input data, as in ScalarValLayerSpec

  INLINE virtual void ExtToComp(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no)  {
    if(!u->HasExtFlag(UNIT_STATE::EXT))
      return;
    u->ClearExtFlag(UNIT_STATE::EXT);
    u->SetExtFlag(UNIT_STATE::COMP);
    u->targ = u->ext_orig;        // orig is safer
    u->ext = 0.0f;
  }
  // #CAT_Activation change external inputs to comparisons (remove input)
  
  INLINE virtual void TargExtToComp(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!u->HasExtFlag(UNIT_STATE::TARG_EXT))
      return;
    if(u->HasExtFlag(UNIT_STATE::EXT))
      u->targ = u->ext_orig;      // orig is safer
    u->ext = 0.0f;
    u->ClearExtFlag(UNIT_STATE::TARG_EXT);
    u->SetExtFlag(UNIT_STATE::COMP);
  }
  // #IGNORE change target & external inputs to comparisons (remove targ & input)

  ///////////////////////////////////////////////////////////////////////
  //        QuarterInit -- at start of new gamma-quarter

  INLINE  bool Quarter_DeepRawNow(int qtr)
  { return deep_raw_qtr & (1 << qtr); }
  // #CAT_Activation test whether to compute deep_raw activations and send deep_raw_net netintput at given quarter (pass net->quarter as arg)
  INLINE  bool Quarter_DeepRawPrevQtr(int qtr)
  { if(qtr == 0) qtr = 3; else qtr--; return deep_raw_qtr & (1 << qtr); }
  // #CAT_Activation test whether the previous quarter was when deep_raw was updated
  INLINE  bool Quarter_DeepRawNextQtr(int qtr)
  { return deep_raw_qtr & (1 << (qtr + 1)); }
  // #CAT_Activation test whether the next quarter will be when deep_raw is updated


  INLINE virtual void Quarter_Init_Unit(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    Quarter_Init_TargFlags(u, net, thr_no);
    Quarter_Init_PrvVals(u, net, thr_no);
    Compute_HardClamp(u, net, thr_no);
  }
  // #CAT_Activation quarter unit-level initialization functions: Init_TargFlags, Init_PrvNet, NetinScale

  INLINE virtual void Quarter_Init_TargFlags(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!u->HasExtFlag(UNIT_STATE::TARG))
      return;

    if(net->phase == LEABRA_NETWORK_STATE::MINUS_PHASE) {
      u->ext = 0.0f;
      u->ext_orig = u->ext;
      u->ClearExtFlag(UNIT_STATE::EXT);
    }
    else {
      u->ext = u->targ;
      u->ext_orig = u->ext;
      u->SetExtFlag(UNIT_STATE::EXT);
    }
  }
  // #CAT_Activation initialize external input flags based on phase

  INLINE virtual void Quarter_Init_PrvVals(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(deep.on && (deep_raw_qtr & Q2)) {
      // if using beta rhythm, this happens at that interval
      if(Quarter_DeepRawPrevQtr(net->quarter)) {
        if(net->quarter == 0) {
          u->net_prv_q = u->net_prv_trl; // net was cleared
        }
        else {
          u->net_prv_q = u->net;
        }
      }
    }
    else {
      if(net->quarter == 0) {
        u->net_prv_q = u->net_prv_trl; // net was cleared
      }
      else {
        u->net_prv_q = u->net;
      }
    }
  }
  // #CAT_Activation update the previous values: e.g., netinput variables (prv_net_q) based on current counters

  INIMPL virtual void Compute_NetinScale(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_Activation compute net input scaling values -- call at start of quarter just to be sure -- computed on projections in layer -- single value for all units

  INLINE void Compute_HardClamp_impl(LEABRA_UNIT_STATE* u, int cycle, bool clip) {
    float ext_in = u->ext;
    u->act_raw = ext_in;
    if(cycle > 0 && deep.ApplyDeepMod()) {
      ext_in *= u->deep_mod;
    }
    bool do_kna_clamp =
      (kna_adapt.on && kna_misc.clamp &&
       !(kna_misc.no_targ && (deep.IsTRC() || u->HasExtFlag(LEABRA_UNIT_STATE::TARG))));
    if(do_kna_clamp) {
      u->net = kna_misc.Compute_Clamped(ext_in, u->gc_kna_f, u->gc_kna_m, u->gc_kna_s);
    }
    else {
      u->net = ext_in;
    }
    u->thal = u->net;
    float new_act = u->net;
    if(clip) {
      new_act = clamp_range.Clip(new_act);
    }
    u->act_eq = u->act = new_act;
    if(do_kna_clamp && kna_misc.invert_nd) {
      if(clip) {
        u->act_nd = clamp_range.Clip(ext_in); // original non-adapted ext val
      }
      else {
        u->act_nd = ext_in;
      }
    }
    else {
      u->act_nd = new_act;
    }
    if(u->act_eq == 0.0f) {
      u->v_m = e_rev.l;
    }
    else {
      u->v_m = act.thr + ext_in / act.gain;
    }
    u->v_m_eq = u->v_m;
    u->da = u->I_net = 0.0f;
  }
  // #IGNORE force units to external values provided by environment

  INLINE virtual void Compute_HardClamp(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!u->HasExtFlag(UNIT_STATE::EXT))
      return;
    LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
    if(!(lay->hard_clamped && lay->HasExtFlag(LAYER_STATE::EXT))) {
      // note: must always use layer state -- if this doesn't work, fix hard_clamped flag!
      // if(!(ls->clamp.hard && lay->HasExtFlag(UNIT_STATE::EXT))) {
      return;
    }
    Compute_HardClamp_impl(u, net->cycle, true); // true = clip
    // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);
    // u->AddToActBuf(syn_delay);
  }
  // #CAT_Activation force units to external values provided by environment

  INLINE virtual void Compute_HardClampNoClip(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!u->HasExtFlag(UNIT_STATE::EXT))
      return;
    LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
    if(!(lay->hard_clamped && lay->HasExtFlag((LAYER_STATE::ExtFlags)UNIT_STATE::EXT))) {
      // note: must always use layer state -- if this doesn't work, fix hard_clamped flag!
      // if(!(ls->clamp.hard && lay->HasExtFlag(UNIT_STATE::EXT))) {
      return;
    }
    Compute_HardClamp_impl(u, net->cycle, false); // false = no clip

    // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);
    // u->AddToActBuf(syn_delay);
  }
  // #CAT_Activation hard-clamp units without clipping values to clamp_range (use for freezing activation states for example, e.g., in second plus phase)
  
  
  virtual void Quarter_Init_Deep(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) { };
  // #CAT_Deep first entry point into quarter init deep for deep -- needed in subclasses (PFCUnitSpec) for updates prior to sending DeepCtxtNetin

  INLINE virtual void Compute_DeepStateUpdt(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no)  {
    if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;
    u->deep_raw_prv = u->deep_raw; // keep track of what we sent here, for context learning
  }
  // #CAT_Deep state update for deep leabra -- typically at start of new alpha trial
  
  INIMPL virtual void Send_DeepCtxtNetin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Deep send deep_raw to deep_ctxt netinput, using deepraw netin temp buffer -- not delta based
  
  INLINE virtual void Compute_DeepCtxt(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;
    // note: this is OK to only integrate selectively b/c not using delta-based netin

    int flat_idx = u->flat_idx;
    int nt = net->n_thrs_built;
    float net_sum = 0.0f;
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendDeepRawNetTmp(j)[flat_idx];
      net_sum += ndval;
    }
    if(deep.tick_updt >= 0) {
      if(net->tick == deep.tick_updt) {
        u->deep_ctxt = deep.ctxt_prv * u->deep_ctxt + deep.ctxt_new * net_sum;
      }
      else {
        u->deep_ctxt = deep.else_prv * u->deep_ctxt + deep.else_new * net_sum;
      }
    }
    else {
      u->deep_ctxt = deep.ctxt_prv * u->deep_ctxt + deep.ctxt_new * net_sum;
    }
  }
  // #CAT_Deep integrate deep_ctxt from deepraw netin temp sent previously by Send_DeepCtxtNetin


  ///////////////////////////////////////////////////////////////////////
  //        Cycle Step 1: Netinput 


  INIMPL virtual void Send_NetinDelta(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Activation send netinput; sender based and only when act changes above a threshold -- only this delta form is supported

  INIMPL virtual void  Compute_NetinRaw(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #IGNORE called by Compute_NetinInteg -- roll up the deltas into net_raw and gi_syn values (or compute net_raw by some other means for special algorithms)

  INIMPL virtual void Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Activation integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)

  INLINE virtual void DeepModNetin_Integ(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    int flat_idx = u->flat_idx;
    int nt = net->n_thrs_built;
    float net_delta = 0.0f;
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendDeepModNetTmp(j)[flat_idx];
      net_delta += ndval;
    }
    u->deep_mod_net += net_delta;
  }
  // #IGNORE integrate deep_mod_net values

  
  INLINE virtual float Compute_DaModNetin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                          int thr_no, float& net_syn) {
    if(net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE) {
      return da_mod.plus * u->da_p * net_syn;
    }
    else {                      // MINUS_PHASE
      return da_mod.minus * u->da_p * net_syn;
    }
  }
  // #IGNORE compute the da_mod netinput extra contribution -- only called if da_mod.on is true so this doesn't need to check that flag -- subtypes can do things to change the function (e.g., D1 vs D2 effects)

  INIMPL virtual float Compute_NetinExtras(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                           int thr_no, float& net_syn);
  // #IGNORE called by Compute_NetinInteg -- get extra excitatory net input factors to add on top of regular synapticaly-generated net inputs, passed as net_syn -- standard items include: bias weights, external soft-clamp input, TI extras (ti_ctxt, d5b_net), CIFER extras: thal (which multiplies net_syn), and da_mod (which multiplies net_syn) -- specialized algorithms can even overwrite net_syn if they need too..

  INIMPL virtual void Compute_NetinInteg_Spike_e(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #IGNORE called by Compute_NetinInteg for spiking units: compute actual excitatory netin conductance value for spiking units by integrating over spike

  INIMPL virtual void Compute_NetinInteg_Spike_i(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #IGNORE called by Compute_NetinInteg for spiking units: compute actual inhibitory netin conductance value for spiking units by integrating over spike
  
  
  INLINE float Compute_EThresh(LEABRA_UNIT_STATE* u) {
    const float gc_l = g_bar.l;
    const float gc_k = g_bar.k * (u->gc_kna_f + u->gc_kna_m + u->gc_kna_s);
    return ((g_bar.i * u->gc_i * e_rev_sub_thr.i + gc_l * e_rev_sub_thr.l +
             gc_k * e_rev_sub_thr.k) / thr_sub_e_rev_e);
  }
  // #IGNORE compute excitatory value that would place unit directly at threshold

  ///////////////////////////////////////////////////////////////////////
  //        Cycle Step 2: Inhibition: these are actually called by Compute_Act to integrate
  //            inhibition computed at the layer level

  INLINE void  Compute_SelfInhib_impl(LEABRA_UNIT_STATE* u, float self_fb, float self_dt) {
    float self = self_fb * u->act;
    u->gi_self += self_dt * (self - u->gi_self);
  }
  // #IGNORE compute self inhibition value

  INLINE void  Compute_ApplyInhib_impl(LEABRA_UNIT_STATE* u, float ival, float gi_ex, float lay_adapt_gi) {
    u->gc_i = ival + lay_adapt_gi * (u->gi_syn + u->gi_self) + gi_ex;
    u->gi_ex = gi_ex;
  }
  // #IGNORE apply computed inhibition value to unit inhibitory conductance -- called by Compute_Act functions -- this is not a separate step in computation

  INIMPL virtual void  Compute_ApplyInhib
    (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, LEABRA_LAYER_STATE* lay, float ival);
  // #IGNORE apply computed inhibition value to unit inhibitory conductance -- called by Compute_Act functions -- this is not a separate step in computation


  ///////////////////////////////////////////////////////////////////////
  //        Cycle Step 3: Activation

  // main function is Compute_Act_Rate or _Spike which calls all the various sub-functions
  // below derived types that send activation directly to special unit variables (e.g.,
  // VTAUnitSpec -> da_p) should do this here, so they can be processed in Compute_Act_Post


  // IMPORTANT: the following function is NOT called -- NETWORK_STATE calls _Rate or _Spike
  // directly!!
  inline void  Compute_Act(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override {
    if(act_fun == SPIKE) {
      Compute_Act_Spike((LEABRA_UNIT_STATE*)uv, (LEABRA_NETWORK_STATE*)net, thr_no);
    }
    else {
      Compute_Act_Rate((LEABRA_UNIT_STATE*)uv, (LEABRA_NETWORK_STATE*)net, thr_no);
    }
  }

  
  INLINE virtual void SaveGatingAct(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(net->cycle == net->times.thal_gate_cycle+1) { // happened last trial
      u->act_g = GetRecAct(u);
    }
  }
  // #CAT_Activation save act_eq to act_g based on network.times.thal_gate_cycle

  INLINE virtual void Compute_DeepMod(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
    if(deep.SendDeepMod()) {
      u->deep_lrn = u->deep_mod = u->act;      // record what we send!
      return;
    }
    else if(deep.IsTRC()) {
      u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
      if(trc.thal_gate) {
        u->net *= u->thal;
      }
      return;
    }
    // must be SUPER units at this point
    else if(lgpd->am_deep_mod_net.max <= deep.mod_thr) { // not enough yet 
      u->deep_lrn = u->deep_mod = 1.0f;         // everybody gets 100%
    }
    else {
      u->deep_lrn = u->deep_mod_net / lgpd->am_deep_mod_net.max;
      u->deep_mod = deep.mod_min + deep.mod_range * u->deep_lrn;
    }
  }
  // #CAT_Activation compute deep_lrn and deep_mod values
    
  INLINE void Compute_DeepModClampAct_impl(LEABRA_UNIT_STATE* u) {
    float ext_in = u->ext * u->deep_mod;
    u->net = u->thal = ext_in;
    ext_in = clamp_range.Clip(ext_in);
    u->act_eq = u->act_nd = u->act = ext_in;
  }
  // #IGNORE called for hard_clamped ApplyDeepMod() layers


  
  ///////////////////////////////////////////////////////////////////////
  //        Rate Code

  INLINE float Compute_ActFun_Rate_fun(float val_sub_thr) {
    return act.NoisyXX1(val_sub_thr);
  }
  // #IGNORE raw activation function: computes an activation value from given value subtracted from its relevant threshold value

  INIMPL virtual void Compute_ActFun_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Activation compute the activation from g_e vs. threshold -- rate code functions

  INLINE virtual void Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);

    SaveGatingAct(u, net, thr_no);
  
    // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);

    if(deep.on) {
      Compute_DeepMod(u, net, thr_no);
    }

    if((net->cycle >= 0) && lay->hard_clamped) {
      // Compute_HardClamp happens before deep_mod is available due to timing of updates
      if(kna_adapt.on && kna_misc.clamp &&
         !(kna_misc.no_targ && (deep.IsTRC() || u->HasExtFlag(LEABRA_UNIT_STATE::TARG)))) {
        float ext_in = u->ext;
        if(deep.ApplyDeepMod())
          ext_in *= u->deep_mod;
        Compute_ActAdapt_Cycle(u, net, thr_no);
        u->act = kna_misc.Compute_Clamped(ext_in, u->gc_kna_f, u->gc_kna_m, u->gc_kna_s);
        u->act_raw = u->act_eq = u->act;
        if(kna_misc.invert_nd) {
          u->act_nd = ext_in;
        }
        else {
          u->act_nd = u->act;
        }
      }
      else {
        if(deep.ApplyDeepMod() && net->cycle == 0) {
          // sync this with Compute_HardClamp:
          Compute_DeepModClampAct_impl(u);
        }
      }
      return; // don't re-compute
    }

    // first, apply inhibition
    LEABRA_UNGP_STATE* ugs = (LEABRA_UNGP_STATE*)u->GetOwnUnGp(net);
    Compute_ApplyInhib(u, net, thr_no, lay, ugs->i_val.g_i);

    Compute_Vm(u, net, thr_no);
    Compute_ActFun_Rate(u, net, thr_no);
    Compute_SelfReg_Cycle(u, net, thr_no);

    // u->AddToActBuf(syn_delay);
  }
  // #CAT_Activation Rate coded activation

  INIMPL virtual void Compute_ActFun_Sigmoid(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Activation compute the activation using simple sigmoidal activation function of net input

  
  ///////////////////////////////////////////////////////////////////////
  //        Spiking

  INLINE virtual void Compute_RateCodeSpike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    // use act_nd here so it isn't a self-fulfilling function!
    // note: this is only used for clamped layers -- dynamic layers use SPIKE-based mechanisms
    u->spike = 0.0f;
    if(u->act_nd <= opt_thresh.send) { // no spiking below threshold..
      u->spk_t = -1;
      return;
    }
    if(u->spk_t < 0) {            // start counting from first time above threshold
      u->spk_t = net->tot_cycle;
      return;
    }
    int interval = spike.ActToInterval(net->times.time_inc, dt.integ, u->act_nd);
    if((net->tot_cycle - u->spk_t) >= interval) {
      u->spike = 1.0f;
      u->v_m = spike_misc.vm_r;   // reset vm when we spike -- now we can use it just like spiking!
      u->spk_t = net->tot_cycle;
      u->I_net = 0.0f;
    }
  }    
  // #CAT_Activation compute spiking activation (u->spike) based off of rate-code activation value

  INLINE void Compute_ActFun_Spiked(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    u->spike = 1.0f;
    u->v_m = spike_misc.vm_r;
    u->I_net = 0.0f;
    if(u->spk_t > 0) {
      float cur_int = net->tot_cycle - u->spk_t;
      spike.UpdateSpikeInterval(u->spike_isi, cur_int);
    }
    u->spk_t = net->tot_cycle;
  }
  // #IGNORE what to do when a spike has been triggered -- common between rate and spike

  INLINE void Compute_ActFun_NotSpiked(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    u->spike = 0.0f;
    if(u->spk_t > 0) {
      float cur_int = net->tot_cycle - u->spk_t;
      if(cur_int > 1.2f * u->spike_isi) { // some kind of estimate of when it exceeds est
        spike.UpdateSpikeInterval(u->spike_isi, cur_int);
      }
    }
  }
  // #IGNORE what to do when a spike has NOT been triggered

  INLINE virtual void Compute_ActFun_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(u->v_m > spike_misc.eff_spk_thr) {
      u->act = 1.0f;
      Compute_ActFun_Spiked(u, net, thr_no);
    }
    else {
      u->act = 0.0f;
      Compute_ActFun_NotSpiked(u, net, thr_no);
    }
    float act_nd = spike.ActFromInterval(u->spike_isi, net->times.time_inc, dt.integ);
    act_nd = act_range.Clip(act_nd);
    act_nd = u->act_nd + dt.vm_dt * (act_nd - u->act_nd); // time integral
    u->da = act_nd - u->act_nd;   // da is on equilibrium activation
    u->act_nd = act_nd;
    if(stp.on) {
      u->act *= u->syn_tr;
      u->act_eq = u->syn_tr * u->act_nd; // act_eq is depressed rate code
    }
    else {
      u->act_eq = u->act_nd;      // eq = nd
    }
  }
  // #CAT_Activation compute the activation from membrane potential -- discrete spiking
  

  INLINE virtual void Compute_ClampSpike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no,
                                         float spike_p) {
    bool fire_now = false;
    switch(spike_misc.clamp_type) {
    case STATE_CLASS(SpikeMiscSpec)::POISSON:
      if(STATE_CLASS(Random)::Poisson(spike_p, thr_no) > 0.0f) fire_now = true;
      break;
    case STATE_CLASS(SpikeMiscSpec)::UNIFORM:
      fire_now = STATE_CLASS(Random)::BoolProb(spike_p, thr_no);
      break;
    case STATE_CLASS(SpikeMiscSpec)::REGULAR: {
      if(spike_p > 0.0f) {
        int cyc_int = (int)((1.0f / spike_p) + 0.5f);
        fire_now = (net->cycle % cyc_int == 0);
      }
      break;
    }
    case STATE_CLASS(SpikeMiscSpec)::CLAMPED:
      return;                     // do nothing further
    }
    if(fire_now) {
      u->v_m = spike_misc.eff_spk_thr + 0.1f; // make it fire
    }
    else {
      u->v_m = e_rev.l;           // make it not fire
    }

    Compute_ActFun_Spike(u, net, thr_no); // then do normal spiking computation
  }
  // #CAT_Activation compute spiking activation according to spike.clamp_type with given probability (typically spike.clamp_max_p * u->ext) -- includes depression and other active factors as done in Compute_ActFun_spike -- used for hard clamped inputs in spiking nets


  INLINE virtual void Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);

    SaveGatingAct(u, net, thr_no);
    // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);

    if(deep.on) {
      Compute_DeepMod(u, net, thr_no);
    }

    if((net->cycle >= 0) && lay->hard_clamped) {
      Compute_ClampSpike(u, net, thr_no, u->ext * spike_misc.clamp_max_p);
      // u->AddToActBuf(syn_delay);
      return; // don't re-compute
    }

    // first, apply inhibition
    LEABRA_UNGP_STATE* ugs = (LEABRA_UNGP_STATE*)u->GetOwnUnGp(net);
    Compute_ApplyInhib(u, net, thr_no, lay, ugs->i_val.g_i);

    Compute_Vm(u, net, thr_no);
    Compute_ActFun_Spike(u, net, thr_no);
    Compute_SelfReg_Cycle(u, net, thr_no);

    // u->AddToActBuf(syn_delay);
  }
  // #CAT_Activation Spiking activation

  
  INLINE float Compute_EqVm(LEABRA_UNIT_STATE* u) {
    const float gc_l = g_bar.l;
    const float gc_k = g_bar.k * (u->gc_kna_f + u->gc_kna_m + u->gc_kna_s);
    float new_v_m = (((u->net * e_rev.e) + (gc_l * e_rev.l) + (gc_k * e_rev.k)
                      + (g_bar.i * u->gc_i * e_rev.i)) /
                     (u->net + gc_l + g_bar.i * u->gc_i));
    return new_v_m;
  }
  // #IGNORE compute the equilibrium (asymptotic) membrante potential from input conductances (assuming they remain fixed as they are)

  INLINE float Compute_INet_impl(LEABRA_UNIT_STATE* u, const float v_m_eff, const float net_eff,
                                 const float gc_i, const float gc_k) {
    return net_eff * (e_rev.e - v_m_eff) + g_bar.l * (e_rev.l - v_m_eff) +
      gc_k * (e_rev.k - v_m_eff) + gc_i * (e_rev.i - v_m_eff);
  }
  // #IGNORE compute the net current given effective v_m -- impl with precomputed vals

  INLINE float Compute_INet(LEABRA_UNIT_STATE* u, const float v_m_eff) {
    const float net_eff = u->net * g_bar.e;
    const float gc_i = u->gc_i * g_bar.i;
    const float gc_k = g_bar.k * (u->gc_kna_f + u->gc_kna_m + u->gc_kna_s);
    return Compute_INet_impl(u, v_m_eff, net_eff, gc_i, gc_k);
  }
  // #IGNORE compute the net current given effective v_m

  INIMPL void Compute_Vm(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Activation Act Step 2: compute the membrane potential from input conductances

  
  ////////////////////////////////////////////////////////////////////
  //              Self reg / adapt / depress

  INLINE virtual void Compute_ActAdapt_Cycle(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!kna_adapt.on) return;
    if(kna_misc.no_targ && (deep.IsTRC() || u->HasExtFlag(LEABRA_UNIT_STATE::TARG)))
      return;
    if(act_fun == SPIKE) {
      kna_adapt.Compute_dKNa_spike(u->spike > 0.1f, u->gc_kna_f, u->gc_kna_m, u->gc_kna_s);
    }
    else {
      kna_adapt.Compute_dKNa_rate(u->act_raw, u->gc_kna_f, u->gc_kna_m, u->gc_kna_s);
    }
  }
  // #CAT_Activation compute the activation-based adaptation value based on spiking and membrane potential
  
  INLINE virtual void Compute_ShortPlast_Cycle(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!stp.on) {
      if(u->syn_tr != 1.0f) {
        u->syn_tr = 1.0f;
        u->syn_nr = 1.0f;
        u->syn_pr = stp.p0;
        u->syn_kre = 0.0f;
      }
    }
    else {
      if (stp.on&&(stp.algorithm == STATE_CLASS(ShortPlastSpec)::CYCLES)) {
        float dnr = stp.dNR(dt.integ, u->syn_kre, u->syn_nr, u->syn_pr, u->spike);
        float dpr = stp.dPR(dt.integ, u->syn_pr, u->spike);
        float dkre = stp.dKRE(dt.integ, u->syn_kre, u->spike);
        u->syn_nr += dnr;        u->syn_pr += dpr;        u->syn_kre += dkre;
        if(u->spike > 0.0f) {                                     // only update at spike
          u->syn_tr = stp.TR(u->syn_nr, u->syn_pr);
        }
      }
    }
  }
  // #CAT_Activation compute whole-neuron (presynaptic) short-term plasticity at the cycle level, using the stp parameters -- updates the syn_* unit variables

  INLINE virtual void Compute_SelfReg_Cycle(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    Compute_ActAdapt_Cycle(u, net, thr_no);
    Compute_ShortPlast_Cycle(u, net, thr_no);
  }
  // #CAT_Activation Act Step 3: compute self-regulatory dynamics at the cycle time scale -- adapt, etc
  
  

  ///////////////////////////////////////////////////////////////////////
  //        Post Activation Step

  INLINE virtual void Compute_SRAvg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    float ru_act;
    if(act_misc.avg_nd) {
      ru_act = u->act_nd;
    }
    else {
      ru_act = u->act_eq;
    }

    u->avg_ss += dt.integ * act_avg.ss_dt * (ru_act - u->avg_ss);
    u->avg_s += dt.integ * act_avg.s_dt * (u->avg_ss - u->avg_s);
    u->avg_m += dt.integ * act_avg.m_dt * (u->avg_s - u->avg_m);

    u->avg_s_eff = act_avg.s_in_s * u->avg_s + act_avg.m_in_s * u->avg_m;
  }
  // #CAT_Learning compute sending-receiving running activation averages (avg_ss, avg_s, avg_m) -- only for this unit (SR name is a hold-over from connection-level averaging that is no longer used) -- unit level only, used for XCAL -- called by Compute_Act_Post
  
  INLINE virtual void Compute_Margin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
    const float v_m_eq = u->v_m_eq;
    if(v_m_eq >= lay->margin.low_thr) {
      if(v_m_eq > lay->margin.hi_thr) {
        u->margin = 2.0f;
      }
      else if(v_m_eq > lay->margin.med_thr) {
        u->margin = 1.0;
      }
      else {
        u->margin = -1.0f;
      }
    }
    else {
      u->margin = -2.0f;
    }
  }
  // #CAT_Learning compute margin status of acivation relative to layer thresholds

  INLINE virtual void Compute_Act_Post(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    Compute_SRAvg(u, net, thr_no);
    Compute_Margin(u, net, thr_no);
  }
  // #CAT_Activation post-processing step after activations are computed -- calls  Compute_SRAvg and Compute_Margin by default -- this is also when any modulatory signals should be sent -- NEVER send any such signals during Compute_Act as they might be consumed by other layers during that time

  
  ///////////////////////////////////////////////////////////////////////
  //        Cycle Stats


  ///////////////////////////////////////////////////////////////////////
  //        Deep Leabra Computations -- after superifical acts updated

  INLINE virtual void Compute_DeepRaw(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!deep.on || !Quarter_DeepRawNow(net->quarter)) return;
    LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);

    // must use act_raw to compute deep_raw because deep_raw is then the input to deep_norm
    // -- if we use act which is already modulated by deep_norm then we get a compounding
    // effect which doesn't work well at all in practice -- does not allow for dynamic
    // deep_mod updating -- just gets stuck in its own positive feedback cycle.

    // todo: revisit this!!
  
    float thr_cmp = lgpd->acts_raw.avg +
      deep.raw_thr_rel * (lgpd->acts_raw.max - lgpd->acts_raw.avg);
    thr_cmp = fmaxf(thr_cmp, deep.raw_thr_abs);
    float draw = 0.0f;
    if(u->act_raw >= thr_cmp) {
      draw = u->act_raw;
    }
    u->deep_raw = draw;
  }
  // #CAT_Activation update the deep_raw activations -- assumes checks have already been done
  
  INIMPL virtual void Send_DeepRawNetin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Deep send deep5b netinputs through SendDeepRawConSpec connections
  
  INLINE virtual void DeepRawNetin_Integ(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    // note: no tests here -- ALWAYS run this (network already filters on general need)
    // so that delta can always be tracked!
    int flat_idx = u->flat_idx;
    int nt = net->n_thrs_built;
    float net_delta = 0.0f;
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendDeepRawNetTmp(j)[flat_idx];
      net_delta += ndval;
    }
    u->deep_raw_net += net_delta;
  }
  // #CAT_Deep send context netinputs through SendDeepRawConSpec connections -- post processing rollup -- checks deeptest

  INLINE virtual void ClearDeepActs(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    u->deep_raw = 0.0f;    u->deep_raw_prv = 0.0f;    u->deep_ctxt = 0.0f;
    u->deep_mod = 1.0f;    u->deep_lrn = 1.0f;        u->deep_raw_net = 0.0f;
    u->deep_mod_net = 0.0f;    u->deep_raw_sent = 0.0f;
  }
  // #CAT_Deep clear all the deep lamina variables -- can be useful to do at discontinuities of experience

  ///////////////////////////////////////////////////////////////////////
  //        Quarter Final

  INLINE float GetRecAct(LEABRA_UNIT_STATE* u) {
    if(act_misc.rec_nd) return u->act_nd;
    return u->act_eq;
  }
  // #IGNORE get activation to record
  
  INLINE virtual void Compute_ActTimeAvg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(act_misc.avg_trace) {
      u->act_avg = act_misc.lambda * u->act_avg + u->act_q0; // using prior act to be compatible with std td learning mechanism
    }
    else {
      if(act_misc.avg_dt <= 0.0f) return;
      u->act_avg += act_misc.avg_dt * (u->act_nd - u->act_avg);
    }
  }
  // #CAT_Activation compute time-averaged activation of unit (using act.avg_dt time constant), typically done at end of settling in Quarter_Final function

  INLINE virtual void Quarter_Final_RecVals(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    float use_act = GetRecAct(u);
    switch(net->quarter) {        // this has not advanced yet -- still 0-3
    case 0: {
      u->act_q1 = use_act;
      break;
    }
    case 1: {
      u->act_q2 = use_act;
      break;
    }
    case 2: {
      u->act_q3 = use_act;
      u->act_m = use_act;
      break;
    }
    case 3: {
      u->act_q4 = use_act;
      u->act_p = use_act;
      if(act_misc.dif_avg) {
        u->act_dif = u->avg_s_eff - u->avg_m;
      }
      else {
        u->act_dif = u->act_p - u->act_m;
      }
      Compute_ActTimeAvg(u, net, thr_no);
      break;
    }
    }
  }
  // #CAT_Activation record state variables after each gamma-frequency quarter-trial of processing

  INLINE virtual void Quarter_Final(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    Quarter_Final_RecVals(u, net, thr_no);
  }
  // #CAT_Activation record state variables after each gamma-frequency quarter-trial of processing
  

  ///////////////////////////////////////////////////////////////////////
  //        Stats

  INLINE float Compute_SSE(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no, bool& has_targ) override {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
    float sse = 0.0f;
    has_targ = false;
    if(u->HasExtFlag(UNIT_STATE::COMP_TARG)) {
      has_targ = true;
      float uerr = u->targ - u->act_m;
      if(fabsf(uerr) >= sse_tol)
        sse = uerr * uerr;
    }
    return sse;
  }
  
  INLINE bool  Compute_PRerr
    (UNIT_STATE* uv, NETWORK_STATE* net, int thr_no, float& true_pos, float& false_pos,
     float& false_neg, float& true_neg) override {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
    true_pos = 0.0f; false_pos = 0.0f; false_neg = 0.0f; true_neg = 0.0f;
    bool has_targ = false;
    if(u->HasExtFlag(UNIT_STATE::COMP_TARG)) {
      has_targ = true;
      if(u->targ > u->act_m) {
        true_pos = u->act_m;
        true_neg = 1.0 - u->targ;
        false_neg = u->targ - u->act_m;
      }
      else {
        true_pos = u->targ;
        false_pos = u->act_m - u->targ;
        true_neg = 1.0 - u->act_m;
      }
    }
    return has_targ;
  }
  
  INLINE virtual float  Compute_NormErr(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no,
                                 bool& targ_active)  {
    targ_active = false;
    if(!u->HasExtFlag(UNIT_STATE::COMP_TARG)) return 0.0f;

    targ_active = (u->targ > 0.5f);   // use this for counting expected activity level
    
    if(net->lstats.on_errs) {
      if(u->act_m > 0.5f && u->targ < 0.5f) return 1.0f;
    }
    if(net->lstats.off_errs) {
      if(u->act_m < 0.5f && u->targ > 0.5f) return 1.0f;
    }
    return 0.0f;
  }
  // #CAT_Statistic compute normalized binary error (0-1 as function of bits off of act_m vs target) according to settings on the network (returns a 1 or 0) -- if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0
  

  INLINE void   UpdateChannels() {
    e_rev_sub_thr.e = e_rev.e - act.thr;
    e_rev_sub_thr.l = e_rev.l - act.thr;
    e_rev_sub_thr.i = e_rev.i - act.thr;
    e_rev_sub_thr.k = e_rev.i - act.thr;
    thr_sub_e_rev_i = (act.thr - e_rev.i); // not multiplied by g_bar.i here..
    thr_sub_e_rev_e = (act.thr - e_rev.e);
  }

  INLINE void   Initialize_core() {
    act_fun = NOISY_XX1;
    deep_raw_qtr = Q4;
    sse_tol = .5f;
    clamp_range.min = .0f;
    clamp_range.max = .95f;
    // clamp_range.UpdateAfterEdit_NoGui();

    vm_range.max = 2.0f;
    vm_range.min = 0.0f;
    // vm_range.UpdateAfterEdit_NoGui();

    g_bar.e = 1.0f;
    g_bar.l = 0.2f;
    g_bar.i = 1.0f;
    g_bar.k = 0.2f;
    
    e_rev.e = 1.0f;
    e_rev.l = 0.3f;
    e_rev.i = 0.25f;
    e_rev.k = 0.10f;

    UpdateChannels();

    noise.type = STATE_CLASS(Random)::GAUSSIAN;
    noise.var = .001f;
  }

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LeabraUnitSpec; }
