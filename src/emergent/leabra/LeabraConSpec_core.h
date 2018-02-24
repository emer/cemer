// this contains core shared code, and is included directly in LeabraConSpec.h, _cpp.h, _cuda.h
//{
  enum LeabraConVars {
    SCALE = N_CON_VARS,        // scaling paramter -- effective weight value is scaled by this factor -- useful for topographic connectivity patterns e.g., to enforce more distant connections to always be lower in magnitude than closer connections -- set by custom weight init code for certain projection specs
    DWNORM,                    // dwt normalization factor -- reset to max of abs value of dwt, decays slowly down over time -- serves as an estimate of variance in weight changes over time
    MOMENT,                    // momentum -- time-integrated dwt changes, to accumulate a consistent direction of weight change and cancel out dithering contradictory changes
    FWT,                       // fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, "wt", via sigmoidal contrast enhancement (wt_sig)
    SWT,                       // slow learning linear (underlying) weight value -- learns more slowly from weight changes than fast weights, and fwt decays down to swt over time
    WB_INC,                    // rate of weight increase from adaptive weight balance -- computed receiver based and so needs to be stored in the connection to optimize speed
    WB_DEC,                    // rate of weight increase from adaptive weight balance -- computed receiver based and so needs to be stored in the connection to optimize speed
    N_LEABRA_CON_VARS,         // #IGNORE number of leabra con vars
  };

  enum Quarters {              // #BITS specifies gamma frequency quarters within an alpha-frequency trial on which to do things
    QNULL = 0x00,              // #NO_BIT no quarter (yeah..)
    Q1 = 0x01,                 // first quarter
    Q2 = 0x02,                 // second quarter
    Q3 = 0x04,                 // third quarter -- posterior cortical minus phase
    Q4 = 0x08,                 // fourth quarter -- posterior cortical plus phase
    Q2_Q4 = Q2 | Q4,           // #NO_BIT standard beta frequency option, for bg, pfc
  };

  bool          inhib;          // #DEF_false #CAT_Activation makes the connection inhibitory (to g_i instead of net)
  bool          feedback;       // is this a feedback projection?  critical for DELTA_FF_FB to apply the correct form of delta rule learning
  STATE_CLASS(WtScaleSpec)      wt_scale;       // #CAT_Activation scale effective weight values to control the overall strength of a projection -- relative shifts balance among different projections, while absolute is a direct multipler

  bool          learn;          // #CAT_Learning #DEF_true individual control over whether learning takes place in this connection spec -- if false, no learning will take place regardless of any other settings -- if true, learning will take place if it is enabled at the network and other relevant levels
  Quarters      learn_qtr;      // #CAT_Learning #CONDSHOW_ON_learn quarters after which learning (Compute_dWt) should take place
  float         lrate;          // #CAT_Learning #CONDSHOW_ON_learn #DEF_0.04 #MIN_0  learning rate -- how fast the weights change per experience -- since version 7.8.5, a hidden factor of 2 has been removed, so this should be 2x what you used to use previously (e.g., default used to be .02, now is .04) -- this governs the rate of change for the fastest adapting weights -- see slow_wts for a more slowly adapting factor
  float         cur_lrate;      // #READ_ONLY #NO_INHERIT #SHOW #CONDSHOW_ON_learn #CAT_Learning current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  float         lrs_mult;       // #READ_ONLY #NO_INHERIT #CAT_Learning learning rate multiplier obtained from the learning rate schedule
  bool          use_unlearnable; // #CAT_Learning #CONDSHOW_ON_learn #AKA_ignore_unlearnable ignore unlearnable trials

  STATE_CLASS(LeabraLearnSpec)  rule;           // #CAT_Learning #CONDSHOW_ON_learn overall learning rule form and parameters
  STATE_CLASS(XCalLearnSpec)    xcal;           // #CAT_Learning #CONDSHOW_ON_learn XCAL (eXtended Contrastive Attractor Learning) learning parameters
  STATE_CLASS(WtSigSpec)        wt_sig;         // #CAT_Learning #CONDSHOW_ON_learn sigmoidal weight function for contrast enhancement: high gain makes weights more binary & discriminative
  STATE_CLASS(LeabraDwtNorm)    dwt_norm;       // #CAT_Learning #CONDSHOW_ON_learn normalization error-driven learning component by the overall running-average magnitude of weight changes across projection (which serves as an estimate of the variance in the weight changes, assuming zero net mean overall)
  STATE_CLASS(LeabraMomentum)   momentum;      // #CAT_Learning #CONDSHOW_ON_learn implements standard, simple momentum and normalization by the overall running-average magnitude of weight changes (which serves as an estimate of the variance in the weight changes, assuming zero net mean overall) -- accentuates consistent directions of weight change and cancels out dithering
  STATE_CLASS(WtBalanceSpec)    wt_bal;         // #CAT_Learning #CONDSHOW_ON_learn weight balance maintenance spec: a soft form of normalization that maintains overall weight balance across units by progressively penalizing weight increases as a function of extent to which average weights exceed target value, and vice-versa when weight average is less than target -- alters rate of weight increases vs. decreases in soft bounding function
  STATE_CLASS(AdaptWtScaleSpec) adapt_scale;    // #CAT_Learning #CONDSHOW_ON_learn parameters to adapt the scale multiplier on weights, as a function of weight value
  STATE_CLASS(SlowWtsSpec)      slow_wts;       // #CAT_Learning #CONDSHOW_ON_learn slow weight specifications -- adds a more slowly-adapting weight factor on top of the standard more rapidly adapting weights
  STATE_CLASS(DeepLrateSpec)    deep;           // #CAT_Learning #CONDSHOW_ON_learn learning rate specs for DeepLeabra learning rate modulation -- effective learning rate can be enhanced for units receiving thalamic modulation vs. those without
  STATE_CLASS(MarginLearnSpec)  margin;         // #CAT_Learning #CONDSHOW_ON_learn learning specs for modulation as a function of marginal activation status -- emphasize learning for units on the margin
  STATE_CLASS(DwtShareSpec)     dwt_share;      // #CAT_Learning #CONDSHOW_ON_learn share dwt changes across different neighboring connections -- a kind of structured randomness within a long-term relationship..


  INLINE float  SigFmLinWt(float lw) { return wt_sig.SigFmLinWt(lw);  }
  // #CAT_Learning get contrast-enhanced weight from linear weight value
  INLINE float  LinFmSigWt(float sig_wt) { return wt_sig.LinFmSigWt(sig_wt); }
  // #CAT_Learning get linear weight value from contrast-enhanced sigmoidal weight value

  INLINE  bool Quarter_LearnNow(int qtr)
  { return learn_qtr & (1 << qtr); }
  // #CAT_Learning test whether to learn at given quarter (pass net->quarter as arg)

  INLINE void Init_Weights(CON_STATE* pcg, NETWORK_STATE* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)pcg;

    cg->err_dwt_max = 0.0f;    cg->bcm_dwt_max = 0.0f; cg->dwt_max = 0.0f;
    cg->wb_inc = 1.0f;         cg->wb_dec = 1.0f;
    
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* scales = cg->OwnCnVar(SCALE);
    // NOTE: it is ESSENTIAL that Init_Weights ONLY does wt, dwt, and scale -- all other vars
    // MUST be initialized in post -- projections with topo weights ONLY do these specific
    // variables but no others..

    int eff_thr_no = net->HasNetFlag(NETWORK_STATE::INIT_WTS_1_THREAD) ? 0 : thr_no;
    
    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      scales[i] = 1.0f;         // default -- must be set in prjn spec if different
    }
    
    for(int i=0; i<sz; i++) {
      if(rnd.type != STATE_CLASS(Random)::NONE) {
        C_Init_Weight_Rnd(wts[i], eff_thr_no);
      }
      C_Init_dWt(dwts[i]);
    }
  }

  INLINE void Init_Weights_scale(CON_STATE* rcg, NETWORK_STATE* net, int thr_no, float init_wt_val) override {
    Init_Weights_symflag(net, thr_no);

    // this is called *receiver based*!!!

    int eff_thr_no = net->HasNetFlag(NETWORK_STATE::INIT_WTS_1_THREAD) ? 0 : thr_no;
    
    const int sz = rcg->size;
    for(int i=0; i<sz; i++) {
      if(rnd.type != STATE_CLASS(Random)::NONE) {
        C_Init_Weight_Rnd(rcg->PtrCn(i, SCALE, net), eff_thr_no);
      }
      rcg->PtrCn(i, WT, net) = init_wt_val;
      C_Init_dWt(rcg->PtrCn(i, DWT, net));
    }
  }
  
  INLINE void  ApplySymmetry_s(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override {
    if(!wt_limits.sym) return;
    UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    const int sz = cg->size;
    for(int i=0; i<sz;i++) {
      int con_idx = -1;
      CON_STATE* rscg = net->FindRecipSendCon(con_idx, cg->UnState(i,net), su);
      if(rscg && con_idx >= 0) {
        CON_SPEC_CPP* rscs = rscg->GetConSpec(net);
        if(rscs && rscs->wt_limits.sym) {
          if(wt_limits.sym_fm_top) {
            cg->OwnCn(i, WT) = rscg->OwnCn(con_idx, WT);
            cg->OwnCn(i, SCALE) = rscg->OwnCn(con_idx, SCALE); // only diff: sync scales!
          }
          else {
            rscg->OwnCn(con_idx, WT) = cg->OwnCn(i, WT);
            rscg->OwnCn(con_idx, SCALE) = cg->OwnCn(i, SCALE);
          }
        }
      }
    }
  }

  INLINE void Init_Weights_post(CON_STATE* pcg, NETWORK_STATE* net, int thr_no) override {
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)pcg;
    cg->Init_ConState();
    
    float* wts = cg->OwnCnVar(WT);
    float* swts = cg->OwnCnVar(SWT);
    float* fwts = cg->OwnCnVar(FWT);
    float* scales = cg->OwnCnVar(SCALE);
    float* dwnorms = cg->OwnCnVar(DWNORM);
    float* moments = cg->OwnCnVar(MOMENT);
    float* wbincs = cg->OwnCnVar(WB_INC);
    float* wbdecs = cg->OwnCnVar(WB_DEC);
    for(int i=0; i<cg->size; i++) {
      fwts[i] = LinFmSigWt(wts[i]); // swt, fwt are linear underlying weight values
      dwnorms[i] = 0.0f;
      moments[i] = 0.0f;
      swts[i] = fwts[i];
      wts[i] *= scales[i];
      wbincs[i] = wbdecs[i] = 1.0f;
      
      LEABRA_CON_STATE* rcg = cg->UnCons(i, net);
      rcg->Init_ConState();    // recv based otherwise doesn't get initialized!
    }
  }

  INLINE void  LoadWeightVal(float wtval, CON_STATE* cg, int cidx, NETWORK_STATE* net) override {
    cg->Cn(cidx, WT, net) = wtval;
    float linwt = LinFmSigWt(wtval / cg->Cn(cidx, SCALE, net));
    cg->Cn(cidx, SWT, net) = linwt;
    cg->Cn(cidx, FWT, net) = linwt;
  }

  INLINE void SetConScale(float scale, CON_STATE* cg, int cidx, NETWORK_STATE* net, int thr_no) override {
    cg->Cn(cidx, SCALE, net) = scale;
  }

  INLINE virtual void  Trial_Init_Specs(LEABRA_NETWORK_STATE* net) {
    if(wt_bal.on) {
      net->net_misc.wt_bal = true;
    }
    if(dwt_norm.RecvConsAgg()) {
      net->net_misc.recv_con_dwnorm = true;
    }
    if(dwt_norm.RecvUnitAgg()) {
      net->net_misc.recv_unit_dwnorm = true;
    }      
  }
  // #CAT_Learning initialize specs and specs update network flags -- e.g., set current learning rate based on schedule given epoch (or error value)

  INLINE void  RenormScales(CON_STATE* cg, NETWORK_STATE* net, int thr_no, bool mult_norm,
                            float avg_wt) override {
    const int sz = cg->size;
    if(sz < 2) return;
    float avg = 0.0f;
    for(int i=0; i<sz; i++) {
      avg += cg->Cn(i, SCALE, net);
    }
    avg /= (float)sz;
    if(mult_norm) {
      float adj = avg_wt / avg;
      for(int i=0; i<sz; i++) {
        cg->Cn(i, SCALE, net) *= adj;
      }
    }
    else {
      float adj = avg_wt - avg;
      for(int i=0; i<sz; i++) {
        cg->Cn(i, SCALE, net) += adj;
      }
    }
  }
    
  
  ///////////////////////////////////////////////////////////////
  //    Activation: Netinput -- only NetinDelta is supported

  INLINE virtual bool  DoesStdNetin() { return true; }
  // #IGNORE does this connection send standard netinput? if so, it will be included in the CUDA send netin computation -- otherwise a separate function is required
  INLINE virtual bool  DoesStdDwt() { return true; }
  // #IGNORE does this connection compute a standard XCAL dWt function? if so, it will be included in the CUDA Compute_dWt computation -- otherwise a separate function is required
  INLINE virtual bool  IsMarkerCon() { return false; }
  // #IGNORE is this a marker con (MarkerConSpec) -- optimized check for higher speed
  INLINE virtual bool  IsDeepCtxtCon() { return false; }
  // #IGNORE is this a deep context connection (DeepCtxtConSpec) -- optimized check for higher speed
  INLINE virtual bool  IsDeepRawCon() { return false; }
  // #IGNORE is this a send deep_raw connection (SendDeepRawConSpec) -- optimized check for higher speed
  INLINE virtual bool  IsDeepModCon() { return false; }
  // #IGNORE is this a send deep_mod connection (SendDeepModConSpec) -- optimized check for higher speed

  INLINE void   C_Send_NetinDelta(const float wt, float* send_netin_vec,
                                  const int ru_idx, const float su_act_delta_eff)
  { send_netin_vec[ru_idx] += wt * su_act_delta_eff; }
  // #IGNORE
#ifdef TA_VEC_USE
  INLINE void   Send_NetinDelta_vec(LEABRA_CON_STATE* cg, const float su_act_delta_eff,
                                    float* send_netin_vec, const float* wts) {
    VECF sa(su_act_delta_eff);
    const int sz = cg->size;
    const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts+i);
      VECF dp = wt * sa;
      VECF rnet;
      float* stnet = send_netin_vec + cg->UnIdx(i);
      rnet.load(stnet);
      rnet += dp;
      rnet.store(stnet);
    }

    // remainder of non-vector chunkable ones
    for(; i<sz; i++) {
      send_netin_vec[cg->UnIdx(i)] += wts[i] * su_act_delta_eff;
    }
  }
  // #IGNORE vectorized version
#endif
  INLINE void   Send_NetinDelta_impl(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net,
                                     int thr_no, const float su_act_delta, const float* wts)  {
    LEABRA_PRJN_STATE* prjn = cg->GetPrjnState(net);
    const float su_act_delta_eff = prjn->scale_eff * su_act_delta;
    if(net->NetinPerPrjn()) {
      float* send_netin_vec = net->ThrSendNetinTmpPerPrjn(thr_no, cg->other_idx);
#ifdef TA_VEC_USE
      Send_NetinDelta_vec(cg, su_act_delta_eff, send_netin_vec, wts);
#else
      CON_STATE_LOOP(cg, C_Send_NetinDelta(wts[i], send_netin_vec,
                                           cg->UnIdx(i), su_act_delta_eff));
#endif
    }
    else {
      float* send_netin_vec = net->ThrSendNetinTmp(thr_no);
#ifdef TA_VEC_USE
      Send_NetinDelta_vec(cg, su_act_delta_eff, send_netin_vec, wts);
#else
      CON_STATE_LOOP(cg, C_Send_NetinDelta(wts[i], send_netin_vec,
                                           cg->UnIdx(i), su_act_delta_eff));
#endif
    }
  }

  // #IGNORE implementation that uses specified weights -- typically only diff in different subclasses is the weight variables used
  INLINE virtual void   Send_NetinDelta(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net,
                                        int thr_no, const float su_act_delta) {
    // note: _impl is used b/c subclasses replace WT var with another variable
    Send_NetinDelta_impl(cg, net, thr_no, su_act_delta, cg->OwnCnVar(WT));
  }
  // #IGNORE #CAT_Activation sender-based delta-activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thr_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // recv-based also needed for some statistics, but is NOT used for main compute code -- uses act_eq for sender act as well
  INLINE float  C_Compute_Netin(const float wt, const float su_act)
  { return wt * su_act; }
  // #IGNORE NOTE: doesn't work with spiking -- need a separate function to use act_eq for that case -- using act_eq does NOT work with scalarval etc
  INLINE float  Compute_Netin(CON_STATE* rcg, NETWORK_STATE* net, int thr_no) override  {
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)rcg;
    LEABRA_PRJN_STATE* prjn = cg->GetPrjnState(net);
    // this is slow b/c going through the PtrCn
    float rval=0.0f;
    CON_STATE_LOOP(cg, rval += C_Compute_Netin(cg->PtrCn(i,WT,net),
                                               cg->UnState(i,net)->act));
    return prjn->scale_eff * rval;
  }
  // #IGNORE

  ///////////////////////////////////////////////////////////////
  //    Learning

  /////////////////////////////////////
  // CtLeabraXCAL code

  INLINE void   GetLrates(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no,
                          float& clrate, bool& deep_on, float& bg_lrate, float& fg_lrate)  {
    LEABRA_LAYER_STATE* rlay = cg->GetRecvLayer(net);
    clrate = cur_lrate * rlay->lrate_mod;
    deep_on = deep.on;
    if(deep_on) {
      if(!rlay->deep_lrate_mod)
        deep_on = false;          // only applicable to deep_norm active layers
    }
    if(deep_on) {
      bg_lrate = deep.bg_lrate;
      fg_lrate = deep.fg_lrate;
    }
  }
  // #IGNORE get the current learning rates including layer-specific and potential deep modulations

  // todo: should go back and explore this at some point:
  // if(xcal.one_thr) {
  //   float eff_thr = ru_avg_l_lrn * ru_avg_l + (1.0f - ru_avg_l_lrn) * srm;
  //   eff_thr = fminf(eff_thr, 1.0f);
  //   dwt += clrate * xcal.dWtFun(srs, eff_thr);
  // }
  // also: fminf(ru_avg_l,1.0f) for threshold as an option..

  INLINE void  C_Compute_dWt_CtLeabraXCAL_Expt
  (float& err, float& bcm, float ru_ru_avg_s_lrn, float ru_su_avg_s_lrn, float ru_avg_m,
   float su_su_avg_s_lrn, float su_ru_avg_s_lrn, float su_avg_m,
   float ru_avg_l, float wt_lin)
  {    
    float srs = su_su_avg_s_lrn * ru_ru_avg_s_lrn;
    float srm = su_avg_m * ru_avg_m;

    switch(rule.bcmrule) {
    case STATE_CLASS(LeabraLearnSpec)::SRS:
      bcm = xcal.dWtFun(srs, ru_avg_l);
      break;
    case STATE_CLASS(LeabraLearnSpec)::RS:
      bcm = su_su_avg_s_lrn * xcal.dWtFun(ru_ru_avg_s_lrn, ru_avg_l);
      break;
    case STATE_CLASS(LeabraLearnSpec)::RS_SIN:
      bcm = xcal.dWtFun(su_su_avg_s_lrn * ru_ru_avg_s_lrn, su_su_avg_s_lrn * ru_avg_l);
      break;
    case STATE_CLASS(LeabraLearnSpec)::CPCA:
      bcm = ru_ru_avg_s_lrn * ((rule.cp_gain * su_su_avg_s_lrn) - wt_lin);
      break;
    }
    
    switch(rule.errule) {
    case STATE_CLASS(LeabraLearnSpec)::ERR_DELTA_FF_FB:
      if(feedback) {
        err = ru_su_avg_s_lrn * (su_ru_avg_s_lrn - su_avg_m);
      }
      else {
        err = su_su_avg_s_lrn * (ru_ru_avg_s_lrn - ru_avg_m);
      }
      break;
    case STATE_CLASS(LeabraLearnSpec)::XCAL:
      err = xcal.dWtFun(srs, srm);
      break;
    case STATE_CLASS(LeabraLearnSpec)::DELTA:
      err = su_su_avg_s_lrn * (ru_ru_avg_s_lrn - ru_avg_m);
      break;
    case STATE_CLASS(LeabraLearnSpec)::XCAL_DELTA:
      err = su_su_avg_s_lrn * xcal.dWtFun(ru_ru_avg_s_lrn, ru_avg_m);
      break;
    case STATE_CLASS(LeabraLearnSpec)::XCAL_DELTA_SIN:
      err = xcal.dWtFun(su_su_avg_s_lrn * ru_ru_avg_s_lrn, su_su_avg_s_lrn * ru_avg_m);
      break;
    case STATE_CLASS(LeabraLearnSpec)::REV_DELTA:
      err = ru_su_avg_s_lrn * (su_ru_avg_s_lrn - su_avg_m);
      break;
    case STATE_CLASS(LeabraLearnSpec)::REV_XCAL_DELTA:
      err = ru_su_avg_s_lrn * xcal.dWtFun(su_ru_avg_s_lrn, su_avg_m);
      break;
    case STATE_CLASS(LeabraLearnSpec)::REV_XCAL_DELTA_SIN:
      err = xcal.dWtFun(ru_su_avg_s_lrn * su_ru_avg_s_lrn, ru_su_avg_s_lrn * su_avg_m);
      break;
    case STATE_CLASS(LeabraLearnSpec)::CHL:
      err = srs - srm;
      break;
    }
  }
  // #IGNORE compute temporally eXtended Contrastive Attractor Learning (XCAL), experimental version, returning new dwt


  INLINE void  C_Compute_dWt_CtLeabraXCAL_CHL
  (float& err, float& bcm, float ru_ru_avg_s_lrn, float ru_avg_m, float su_su_avg_s_lrn,
   float su_avg_m, float ru_avg_l)
  {
    float srs = su_su_avg_s_lrn * ru_ru_avg_s_lrn;
    float srm = su_avg_m * ru_avg_m;
    
    bcm = xcal.dWtFun(srs, ru_avg_l);
    err = xcal.dWtFun(srs, srm);
  }
  // #IGNORE compute temporally eXtended Contrastive Attractor Learning (XCAL), CHL version, returning new dwt

  INLINE void  C_Compute_dWt_CtLeabraXCAL_Delta
  (float& err, float& bcm, float ru_ru_avg_s_lrn, float ru_su_avg_s_lrn, float ru_avg_m,
   float su_su_avg_s_lrn, float su_ru_avg_s_lrn, float su_avg_m,
   float ru_avg_l)
  {
    bcm = xcal.dWtFun(su_su_avg_s_lrn * ru_ru_avg_s_lrn, su_su_avg_s_lrn * ru_avg_l);
    if(feedback) {
      err = ru_su_avg_s_lrn * (su_ru_avg_s_lrn - su_avg_m);
    }
    else {
      err = su_su_avg_s_lrn * (ru_ru_avg_s_lrn - ru_avg_m);
    }
  }
  // #IGNORE compute temporally eXtended Contrastive Attractor Learning (XCAL), DELTA_FF_FB version, returning new dwt

  INLINE float  C_Compute_dWt_CtLeabraXCAL_MarginSign(float ru_margin, float su_su_avg_s_lrn) {
    return margin.sign_lrn * margin.SignDwt(ru_margin) * su_su_avg_s_lrn;
  }
  // #IGNORE margin sign_dwt -- only if margin.sign_dwt


  INLINE void  C_Compute_dWt_CtLeabraXCAL
  (float& err, float& bcm, float ru_ru_avg_s_lrn, float ru_su_avg_s_lrn, float ru_avg_m,
   float su_su_avg_s_lrn, float su_ru_avg_s_lrn, float su_avg_m, float ru_avg_l, float wt_lin)
  {
    switch(rule.rule) {
    case STATE_CLASS(LeabraLearnSpec)::DELTA_FF_FB:
      C_Compute_dWt_CtLeabraXCAL_Delta
        (err, bcm, ru_ru_avg_s_lrn, ru_su_avg_s_lrn, ru_avg_m, su_su_avg_s_lrn,
         su_ru_avg_s_lrn, su_avg_m, ru_avg_l);
      break;
    case STATE_CLASS(LeabraLearnSpec)::XCAL_CHL:
      C_Compute_dWt_CtLeabraXCAL_CHL
        (err, bcm, ru_ru_avg_s_lrn, ru_avg_m, su_su_avg_s_lrn, su_avg_m, ru_avg_l);
      break;
    case STATE_CLASS(LeabraLearnSpec)::EXPT:
      C_Compute_dWt_CtLeabraXCAL_Expt
        (err, bcm, ru_ru_avg_s_lrn, ru_su_avg_s_lrn, ru_avg_m, su_su_avg_s_lrn,
         su_ru_avg_s_lrn, su_avg_m, ru_avg_l, wt_lin);
      break;
    }
  }
  // #IGNORE compute temporally eXtended Contrastive Attractor Learning (XCAL)

  INLINE void   Compute_dWt(CON_STATE* scg, NETWORK_STATE* rnet, int thr_no) override  {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    if(su->avg_s < xcal.lrn_thr && su->avg_m < xcal.lrn_thr) return;
    // no need to learn!

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, net, thr_no, clrate, deep_on, bg_lrate, fg_lrate);

    const float su_su_avg_s_lrn = su->su_avg_s_lrn;
    const float su_ru_avg_s_lrn = su->ru_avg_s_lrn;
    const float su_avg_s = su->avg_s;
    const float su_avg_m = su->avg_m;
    const float su_avg_l = su->avg_l;
    const int sz = cg->size;

    LEABRA_PRJN_STATE* prjn = cg->GetPrjnState(net);
    if(momentum.on) {
      clrate *= momentum.lr_comp;
    }
    
    float err_dwt_max = 0.0f;
    float bcm_dwt_max = 0.0f;
    float dwt_max = 0.0f;
    float err_dwt_avg = 0.0f;
    float bcm_dwt_avg = 0.0f;
    float dwt_avg = 0.0f;

    float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);
    float* dwnorms = cg->OwnCnVar(DWNORM);
    float* moments = cg->OwnCnVar(MOMENT);
    
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = cg->UnState(i, net);
      if(ru->lesioned()) continue;
      float lrate_eff = clrate;
      if(deep_on) {
        lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
      }
      if(margin.lrate_mod) {
        lrate_eff *= margin.MarginLrate(ru->margin);
      }
      float l_lrn_eff = xcal.LongLrate(ru->avg_l_lrn);
      float err, bcm;
      C_Compute_dWt_CtLeabraXCAL
        (err, bcm, ru->ru_avg_s_lrn, ru->su_avg_s_lrn, ru->avg_m,
         su_su_avg_s_lrn, su_ru_avg_s_lrn, su_avg_m, ru->avg_l, fwts[i]);

      if(margin.sign_dwt) {
        bcm += C_Compute_dWt_CtLeabraXCAL_MarginSign(ru->margin, su_su_avg_s_lrn);
      }
      
      bcm *= l_lrn_eff;
      err *= xcal.m_lrn;

      float abserr = fabsf(err);
      if(dwt_norm.stats) {
        float absbcm = fabsf(bcm);
        err_dwt_max = fmaxf(abserr, err_dwt_max);
        bcm_dwt_max = fmaxf(absbcm, bcm_dwt_max);
        err_dwt_avg += abserr;
        bcm_dwt_avg += absbcm;
      }

      if(dwt_norm.on && dwt_norm.err_only) {
        dwt_norm.UpdateAvg(dwnorms[i], abserr); // always update our syn factor -- later aggregated -- must be second loop to avoid order effects!
        err *= dwt_norm.EffNormFactor(dwnorms[i]);
      }
      
      float new_dwt = bcm + err;
      if(dwt_norm.on && !dwt_norm.err_only) {
        dwt_norm.UpdateAvg(dwnorms[i], fabsf(new_dwt)); // always update
        new_dwt *= dwt_norm.EffNormFactor(dwnorms[i]);
      }
      
      if(momentum.on) {
        new_dwt = momentum.ComputeMoment(moments[i], new_dwt);
      }
      dwts[i] += lrate_eff * new_dwt;

      if(dwt_norm.stats) {
        float absdwt = fabsf(new_dwt);
        dwt_max = fmaxf(absdwt, dwt_max); 
        dwt_avg += absdwt; 
      }
    }
    
    if(dwt_norm.stats) {
      cg->err_dwt_max = err_dwt_max;
      cg->bcm_dwt_max = bcm_dwt_max;
      cg->dwt_max = dwt_max;

      if(sz > 0) {
        float nrm = 1.0f / (float)sz;
        cg->err_dwt_avg = err_dwt_avg * nrm;
        cg->bcm_dwt_avg = bcm_dwt_avg * nrm;
        cg->dwt_avg = dwt_avg * nrm;
      }
    }

    if(dwt_norm.SendConsAgg() || dwt_norm.PrjnAgg()) {
      DwtNorm_SendCons(cg, net, thr_no);
    }
  }


  INLINE void   C_Compute_Weights_CtLeabraXCAL
    (float& wt, float dwt, float& fwt, float& swt, float& scale,
     const float wb_inc, const float wb_dec, int thr_no)
  {
    if(dwt == 0.0f) return;
    if(wt_sig.soft_bound) {
      if(dwt > 0.0f)    dwt *= wb_inc * (1.0f - fwt);
      else              dwt *= wb_dec * fwt;
    }
    else {
      if(dwt > 0.0f)    dwt *= wb_inc;
      else              dwt *= wb_dec;
    }
    fwt += dwt;
    C_ApplyLimits(fwt);
    // swt = fwt;  // leave swt as pristine original weight value -- saves time
    // and is useful for visualization!
    wt = scale * SigFmLinWt(fwt);
    // dwt = 0.0f;

    if(adapt_scale.on) {
      adapt_scale.AdaptWtScale(scale, wt);
    }
  }
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- no slow wts

  INLINE void   C_Compute_Weights_CtLeabraXCAL_slow
    (float& wt, float dwt, float& fwt, float& swt, float& scale,
     const float wb_inc, const float wb_dec, int thr_no)
  { 
    if(wt_sig.soft_bound) {
      if(dwt > 0.0f)    dwt *= wb_inc * (1.0f - fwt);
      else              dwt *= wb_dec * fwt;
    }
    else {
      if(dwt > 0.0f)    dwt *= wb_inc;
      else              dwt *= wb_dec;
    }
    fwt += dwt;
    float eff_wt = slow_wts.swt_pct * swt + slow_wts.fwt_pct * fwt;
    float nwt = scale * SigFmLinWt(eff_wt);
    wt += slow_wts.wt_dt * (nwt - wt);
    swt += slow_wts.slow_dt * (fwt - swt);
    // dwt = 0.0f;
    
    if(adapt_scale.on) {
      adapt_scale.AdaptWtScale(scale, wt);
    }
  }
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- slow wts


  INLINE float C_Compute_Weights_dwtshare
  (bool dwt_sh, float* dwts, const int i, const int neigh, const int sz) {
    if(dwt_sh) {
      float dwt = 0.0f;
      for(int ni = -neigh; ni <= neigh; ni++) {
        int j = i + ni;
        if(j < 0)         j += sz;
        else if(j >= sz)  j -= sz;
        dwt += dwts[j];
      }
      return dwt;
    }
    else {
      return dwts[i];
    }
  }
  // #IGNORE do dwt sharing or just dwt, depending on dwt_sh

  INLINE void   Compute_Weights(CON_STATE* scg, NETWORK_STATE* net, int thr_no) override {
    if(!learn) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    float* wts = cg->OwnCnVar(WT);      float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);    float* swts = cg->OwnCnVar(SWT);
    float* scales = cg->OwnCnVar(SCALE);
    const int sz = cg->size;

    int neigh = dwt_share.neigh;
    bool dwt_sh = (dwt_share.on && sz > 2 * neigh &&
                   (dwt_share.p_share == 1.0f || Random::BoolProb(dwt_share.p_share, thr_no)));

    if(wt_bal.on) {
      // note: MUST get these from ru -- diff for each con -- can't copy to sender!
      // storing in synapses is about 2x faster and essentially no overhead vs. no wtbal
      float* wbincs = cg->OwnCnVar(WB_INC);
      float* wbdecs = cg->OwnCnVar(WB_DEC);

      if(slow_wts.on) {
        for(int i=0; i<sz; i++) {
          float dwt = C_Compute_Weights_dwtshare(dwt_sh, dwts, i, neigh, sz);
          C_Compute_Weights_CtLeabraXCAL_slow
            (wts[i], dwt, fwts[i], swts[i], scales[i], wbincs[i], wbdecs[i], thr_no);
        }
      }
      else {
        for(int i=0; i<sz; i++) {
          float dwt = C_Compute_Weights_dwtshare(dwt_sh, dwts, i, neigh, sz);
          C_Compute_Weights_CtLeabraXCAL
            (wts[i], dwt, fwts[i], swts[i], scales[i], wbincs[i], wbdecs[i], thr_no);
        }
      }
    }
    else {
      if(slow_wts.on) {
        for(int i=0; i<sz; i++) {
          float dwt = C_Compute_Weights_dwtshare(dwt_sh, dwts, i, neigh, sz);
          C_Compute_Weights_CtLeabraXCAL_slow
            (wts[i], dwt, fwts[i], swts[i], scales[i], 1.0f, 1.0f, thr_no);
        }
      }
      else {
        for(int i=0; i<sz; i++) {
          float dwt = C_Compute_Weights_dwtshare(dwt_sh, dwts, i, neigh, sz);
          C_Compute_Weights_CtLeabraXCAL
            (wts[i], dwt, fwts[i], swts[i], scales[i], 1.0f, 1.0f, thr_no);
        }
      }
    }
    // reset dwts after updating -- dwtshare requires doing this after the fact
    for(int i=0; i<sz; i++) {
      dwts[i] = 0.0f;
    }
  }

  INLINE virtual void DwtNorm_SendCons(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net,
                                               int thr_no) {
    float* dwnorms = cg->OwnCnVar(DWNORM);
    const int sz = cg->size;
    float max_dwnorm = 0.0f;
    for(int i=0; i<sz; i++) {
      max_dwnorm = fmaxf(max_dwnorm, dwnorms[i]); // simple max
    }
    cg->cons_dwnorm = max_dwnorm;
    for(int i=0; i<sz; i++) {
      dwnorms[i] = max_dwnorm;
    }
  }
  // #IGNORE compute dwt_norm sender-based con group level dwnorm factor

  INLINE virtual void   Compute_WtBal_DwtNormRecv(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no) {
    bool do_wb = wt_bal.on;
    bool do_norm = dwt_norm.RecvConsAgg();
    if(!learn || cg->size < 1 || !(do_wb || do_norm)) return;
    LEABRA_UNIT_STATE* ru = cg->ThrOwnUnState(net, thr_no);
    if(wt_bal.no_targ &&
       (ru->HasUnitFlag(LEABRA_UNIT_STATE::TRC) || ru->HasExtFlag(LEABRA_UNIT_STATE::TARG))) {
      do_wb = false;
      if(!do_norm) return;      // no need
    }
    
    float sum_wt = 0.0f;
    int sum_n = 0;
    float max_dwnorm = 0.0f;
    
    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      if(do_wb) {
        float wt = cg->PtrCn(i,WT,net);
        if(wt >= wt_bal.avg_thr) {
          sum_wt += wt;
          sum_n++;
        }
      }
      if(do_norm) {
        float dwnorm = cg->PtrCn(i,DWNORM,net);
        max_dwnorm = fmaxf(max_dwnorm, dwnorm);
      }
    }

    if(do_norm) {
      cg->cons_dwnorm = max_dwnorm;
    }

    if(do_wb) {
      if(sum_n > 0)
        sum_wt /= (float)sum_n;
      else
        sum_wt = 0.0f;
      cg->wb_avg = sum_wt;
      wt_bal.WtBal(sum_wt, ru->act_avg, cg->wb_fact, cg->wb_inc, cg->wb_dec);
    }
    // note: these are specific to recv unit and cannot be copied to sender!
    // BUT can copy to synapses:

    for(int i=0; i<sz; i++) {
      if(do_wb) {
        cg->PtrCn(i,WB_INC,net) = cg->wb_inc;
        cg->PtrCn(i,WB_DEC,net) = cg->wb_dec;
      }
      if(do_norm) {
        cg->PtrCn(i,DWNORM,net) = max_dwnorm;
      }
    }
  }
  // #IGNORE compute weight balance factors and / or DwtNorm at a recv level

  INLINE virtual void Compute_EpochWeights(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net,
                                           int thr_no) { };
  // #IGNORE compute epoch-level weights

  inline void Compute_CopyWeights(LEABRA_CON_STATE* cg, LEABRA_CON_STATE* src_cg, LEABRA_NETWORK_STATE* net) {
    const int mx = MIN(cg->size, src_cg->size);
    float* wts = cg->OwnCnVar(WT);
    float* src_wts = src_cg->OwnCnVar(WT);
    for(int i=0; i<mx; i++) {
      wts[i] = src_wts[i];
    }
  }
  // #IGNORE copy weights from src_cg to cg -- typically used to compute synchronization of weights thought to take place during sleep -- typically in TI mode, where the Thal pathway synchronizes with the Super weights -- can be useful for any plus phase conveying weights to avoid positive feedback loop dynamics

  
  /////////////////////////////////////
  //    Bias Weights

  // same as original:
  INLINE void B_Init_dWt(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override {
    C_Init_dWt(uv->bias_dwt);
  }

  INLINE void B_Init_Weights_post(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    LEABRA_UNIT_STATE* uv = (LEABRA_UNIT_STATE*)u;
    float wt = uv->bias_wt; uv->bias_swt = wt; uv->bias_fwt = wt;
  }

  INLINE void B_Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    if(!learn) return;
    LEABRA_UNIT_STATE* uv = (LEABRA_UNIT_STATE*)u;
    // only err is useful contributor to this learning
    float dw = uv->avg_s - uv->avg_m;
    uv->bias_dwt += cur_lrate * dw;
  }

  INLINE void B_Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    if(!learn) return;
    LEABRA_UNIT_STATE* uv = (LEABRA_UNIT_STATE*)u;
    float& wt =  uv->bias_wt;
    float& dwt = uv->bias_dwt;
    float& fwt = uv->bias_fwt;
    float& swt = uv->bias_swt;
    fwt += dwt;
    if(slow_wts.on) {
      float eff_wt = slow_wts.swt_pct * swt + slow_wts.fwt_pct * fwt;
      wt += slow_wts.wt_dt * (eff_wt - wt);
      swt += slow_wts.slow_dt * (fwt - swt);
    }
    else {
      swt = fwt;
      wt = fwt;
    }
    dwt = 0.0f;
    C_ApplyLimits(wt);
  }
  
  INIMPL bool   SaveVar(CON_STATE* cg, NETWORK_STATE* net, int var_no) const override;

  INIMPL const char*  ConVarName(int var_no) const override;

  INLINE void Initialize_core() {
    inhib = false;  learn = true;  learn_qtr = Q4;  use_unlearnable = true;
    wt_limits.min = 0.0f;  wt_limits.max = 1.0f;  wt_limits.sym = true;
    wt_limits.type = STATE_CLASS(WeightLimits)::MIN_MAX;
    rnd.mean = .5f;  rnd.var = .25f;
    lrate = .04f;    cur_lrate = .04f;  lrs_mult = 1.0f;
  }
    
  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LeabraConSpec; }

