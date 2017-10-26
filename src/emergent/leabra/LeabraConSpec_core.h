// this contains core shared code, and is included directly in LeabraConSpec.h, _cpp.h, _cuda.h
//{
  enum LeabraConVars {
    SCALE = N_CON_VARS,        // scaling paramter -- effective weight value is scaled by this factor -- useful for topographic connectivity patterns e.g., to enforce more distant connections to always be lower in magnitude than closer connections -- set by custom weight init code for certain projection specs
    DWAVG,                     // average of absolute value of computed dwt values over time -- serves as an estimate of variance in weight changes over time
    MOMENT,                    // momentum -- time-integrated dwt changes, to accumulate a consistent direction of weight change and cancel out dithering contradictory changes
    FWT,                       // fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, "wt", via sigmoidal contrast enhancement (wt_sig)
    SWT,                       // slow learning linear (underlying) weight value -- learns more slowly from weight changes than fast weights, and fwt decays down to swt over time
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

  bool		inhib;		// #DEF_false #CAT_Activation makes the connection inhibitory (to g_i instead of net)
  STATE_CLASS(WtScaleSpec)	wt_scale;	// #CAT_Activation scale effective weight values to control the overall strength of a projection -- relative shifts balance among different projections, while absolute is a direct multipler

  bool		learn;		// #CAT_Learning #DEF_true individual control over whether learning takes place in this connection spec -- if false, no learning will take place regardless of any other settings -- if true, learning will take place if it is enabled at the network and other relevant levels
  Quarters      learn_qtr;      // #CAT_Learning #CONDSHOW_ON_learn quarters after which learning (Compute_dWt) should take place
  float		lrate;		// #CAT_Learning #CONDSHOW_ON_learn #DEF_0.04 #MIN_0  learning rate -- how fast the weights change per experience -- since version 7.8.5, a hidden factor of 2 has been removed, so this should be 2x what you used to use previously (e.g., default used to be .02, now is .04) -- this governs the rate of change for the fastest adapting weights -- see slow_wts for a more slowly adapting factor
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW #CONDSHOW_ON_learn #CAT_Learning current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  float		lrs_mult;	// #READ_ONLY #NO_INHERIT #CAT_Learning learning rate multiplier obtained from the learning rate schedule
  bool          use_unlearnable; // #CAT_Learning #CONDSHOW_ON_learn #AKA_ignore_unlearnable ignore unlearnable trials

  STATE_CLASS(XCalLearnSpec)	xcal;		// #CAT_Learning #CONDSHOW_ON_learn XCAL (eXtended Contrastive Attractor Learning) learning parameters
  STATE_CLASS(WtSigSpec)	wt_sig;		// #CAT_Learning #CONDSHOW_ON_learn sigmoidal weight function for contrast enhancement: high gain makes weights more binary & discriminative
  STATE_CLASS(LeabraMomentum)   momentum;      // #CAT_Learning #CONDSHOW_ON_learn implements standard, simple momentum and normalization by the overall running-average magnitude of weight changes (which serves as an estimate of the variance in the weight changes, assuming zero net mean overall) -- accentuates consistent directions of weight change and cancels out dithering
  STATE_CLASS(WtBalanceSpec)    wt_bal;         // #CAT_Learning #CONDSHOW_ON_learn weight balance maintenance spec: a soft form of normalization that maintains overall weight balance across units by progressively penalizing weight increases as a function of extent to which average weights exceed target value, and vice-versa when weight average is less than target -- alters rate of weight increases vs. decreases in soft bounding function
  STATE_CLASS(AdaptWtScaleSpec) adapt_scale;	// #CAT_Learning #CONDSHOW_ON_learn parameters to adapt the scale multiplier on weights, as a function of weight value
  STATE_CLASS(SlowWtsSpec)      slow_wts;       // #CAT_Learning #CONDSHOW_ON_learn slow weight specifications -- adds a more slowly-adapting weight factor on top of the standard more rapidly adapting weights
  STATE_CLASS(DeepLrateSpec)    deep;		// #CAT_Learning #CONDSHOW_ON_learn learning rate specs for DeepLeabra learning rate modulation -- effective learning rate can be enhanced for units receiving thalamic modulation vs. those without
  STATE_CLASS(MarginLearnSpec)  margin;	// #CAT_Learning #CONDSHOW_ON_learn learning specs for modulation as a function of marginal activation status -- emphasize learning for units on the margin


  INLINE int  GetStateSpecType() const override { return LEABRA_NETWORK_STATE::T_LeabraConSpec; }


  INLINE float	SigFmLinWt(float lw) { return wt_sig.SigFmLinWt(lw);  }
  // #CAT_Learning get contrast-enhanced weight from linear weight value
  INLINE float	LinFmSigWt(float sig_wt) { return wt_sig.LinFmSigWt(sig_wt); }
  // #CAT_Learning get linear weight value from contrast-enhanced sigmoidal weight value

  INLINE  bool Quarter_LearnNow(int qtr)
  { return learn_qtr & (1 << qtr); }
  // #CAT_Learning test whether to learn at given quarter (pass net->quarter as arg)

  INLINE void Init_Weights(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);

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

  INLINE void Init_Weights_rcgp(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no) {
    cg->wt_avg = 0.5f;
    cg->wb_inc = 1.0f;
    cg->wb_dec = 1.0f;
  }
  // #IGNORE recv con group init weights -- for weight balance params
  
  INLINE void Init_Weights_post(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override {
    Init_Weights_rcgp((LEABRA_CON_STATE*)cg, (LEABRA_NETWORK_STATE*)net, thr_no);
    
    float* wts = cg->OwnCnVar(WT);
    float* swts = cg->OwnCnVar(SWT);
    float* fwts = cg->OwnCnVar(FWT);
    float* scales = cg->OwnCnVar(SCALE);
    float* dwavgs = cg->OwnCnVar(DWAVG);
    float* moments = cg->OwnCnVar(MOMENT);
    for(int i=0; i<cg->size; i++) {
      fwts[i] = LinFmSigWt(wts[i]); // swt, fwt are linear underlying weight values
      dwavgs[i] = 0.0f;
      moments[i] = 0.0f;
      swts[i] = fwts[i];
      wts[i] *= scales[i];
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
  //	Activation: Netinput -- only NetinDelta is supported

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

  INLINE void 	C_Send_NetinDelta(const float wt, float* send_netin_vec,
                                  const int ru_idx, const float su_act_delta_eff)
  { send_netin_vec[ru_idx] += wt * su_act_delta_eff; }
  // #IGNORE
#ifdef TA_VEC_USE
  INLINE void 	Send_NetinDelta_vec(LEABRA_CON_STATE* cg, const float su_act_delta_eff,
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
  INLINE void 	Send_NetinDelta_impl(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net,
                                     int thr_no, const float su_act_delta, const float* wts)  {
    const float su_act_delta_eff = cg->scale_eff * su_act_delta;
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
  INLINE virtual void 	Send_NetinDelta(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net,
                                        int thr_no, const float su_act_delta) {
    // note: _impl is used b/c subclasses replace WT var with another variable
    Send_NetinDelta_impl(cg, net, thr_no, su_act_delta, cg->OwnCnVar(WT));
  }
  // #IGNORE #CAT_Activation sender-based delta-activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thr_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // recv-based also needed for some statistics, but is NOT used for main compute code -- uses act_eq for sender act as well
  INLINE float 	C_Compute_Netin(const float wt, const float su_act)
  { return wt * su_act;	}
  // #IGNORE NOTE: doesn't work with spiking -- need a separate function to use act_eq for that case -- using act_eq does NOT work with scalarval etc
  INLINE float 	Compute_Netin(CON_STATE* rcg, NETWORK_STATE* net, int thr_no) override  {
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)rcg;
    // this is slow b/c going through the PtrCn
    float rval=0.0f;
    CON_STATE_LOOP(cg, rval += C_Compute_Netin(cg->PtrCn(i,WT,net),
                                               cg->UnState(i,net)->act));
    return ((LEABRA_CON_STATE*)cg)->scale_eff * rval;
  }
  // #IGNORE

  ///////////////////////////////////////////////////////////////
  //	Learning

  /////////////////////////////////////
  // CtLeabraXCAL code

  INLINE void   GetLrates(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no,
                          float& clrate, bool& deep_on, float& bg_lrate, float& fg_lrate)  {
    LEABRA_LAYER_STATE* rlay = (LEABRA_LAYER_STATE*)cg->GetPrjnRecvLayer(net);
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

  INLINE float 	C_Compute_dWt_CtLeabraXCAL
    (const float ru_avg_s, const float ru_avg_m, const float su_avg_s, const float su_avg_m,
     const float ru_avg_l, const float ru_avg_l_lrn, const float ru_margin) 
  { float srs = ru_avg_s * su_avg_s;
    float srm = ru_avg_m * su_avg_m;
    float new_dwt = (ru_avg_l_lrn * xcal.dWtFun(srs, ru_avg_l) +
                    xcal.m_lrn * xcal.dWtFun(srs, srm));
    if(margin.sign_dwt) {
      float mdwt = ru_avg_l_lrn * margin.sign_lrn * margin.SignDwt(ru_margin) * su_avg_s;
      new_dwt += mdwt;
    }
    return new_dwt;
  }
  // #IGNORE compute temporally eXtended Contrastive Attractor Learning (XCAL), returning new dwt

  INLINE void	Compute_dWt(CON_STATE* scg, NETWORK_STATE* rnet, int thr_no) override  {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->OwnUnState(net);
    if(su->avg_s < xcal.lrn_thr && su->avg_m < xcal.lrn_thr) return;
    // no need to learn!

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, net, thr_no, clrate, deep_on, bg_lrate, fg_lrate);

    const float su_avg_s = su->avg_s_eff;
    const float su_avg_m = su->avg_m;
    const int sz = cg->size;

    float* dwts = cg->OwnCnVar(DWT);
  
    if(momentum.on) {
      clrate *= momentum.lrate_comp;
      float* dwavgs = cg->OwnCnVar(DWAVG);
      float* moments = cg->OwnCnVar(MOMENT);
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i, net);
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        if(margin.lrate_mod) {
          lrate_eff *= margin.MarginLrate(ru->margin);
        }
        float l_lrn_eff = xcal.LongLrate(ru->avg_l_lrn);
        float new_dwt = C_Compute_dWt_CtLeabraXCAL
          (ru->avg_s_eff, ru->avg_m, su_avg_s, su_avg_m, ru->avg_l, l_lrn_eff, ru->margin);
        new_dwt = momentum.ComputeMoment(moments[i], dwavgs[i], new_dwt);
        dwts[i] += lrate_eff * new_dwt;
      }
    }
    else {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i, net);
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        if(margin.lrate_mod) {
          lrate_eff *= margin.MarginLrate(ru->margin);
        }
        float l_lrn_eff = xcal.LongLrate(ru->avg_l_lrn);
        float new_dwt = C_Compute_dWt_CtLeabraXCAL
          (ru->avg_s_eff, ru->avg_m, su_avg_s, su_avg_m, ru->avg_l, l_lrn_eff, ru->margin);
        dwts[i] += lrate_eff * new_dwt;
      }
    }
  }


  INLINE void	C_Compute_Weights_CtLeabraXCAL
    (float& wt, float& dwt, float& fwt, float& swt, float& scale,
     const float wb_inc, const float wb_dec)
  {
    if(dwt == 0.0f) return;
    if(wt_sig.soft_bound) {
      if(dwt > 0.0f)	dwt *= wb_inc * (1.0f - fwt);
      else		dwt *= wb_dec * fwt;
    }
    else {
      if(dwt > 0.0f)	dwt *= wb_inc;
      else		dwt *= wb_dec;
    }
    fwt += dwt;
    C_ApplyLimits(fwt);
    // swt = fwt;  // leave swt as pristine original weight value -- saves time
    // and is useful for visualization!
    wt = scale * SigFmLinWt(fwt);
    dwt = 0.0f;

    if(adapt_scale.on) {
      adapt_scale.AdaptWtScale(scale, wt);
    }
  }
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- no slow wts

  INLINE void	C_Compute_Weights_CtLeabraXCAL_slow
    (float& wt, float& dwt, float& fwt, float& swt, float& scale,
     const float wb_inc, const float wb_dec)
  { 
    if(dwt > 0.0f)	dwt *= wb_inc * (1.0f - fwt);
    else		dwt *= wb_dec * fwt;
    fwt += dwt;
    float eff_wt = slow_wts.swt_pct * swt + slow_wts.fwt_pct * fwt;
    float nwt = scale * SigFmLinWt(eff_wt);
    wt += slow_wts.wt_dt * (nwt - wt);
    swt += slow_wts.slow_dt * (fwt - swt);
    dwt = 0.0f;
    
    if(adapt_scale.on) {
      adapt_scale.AdaptWtScale(scale, wt);
    }
  }
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- slow wts

  INLINE void	Compute_Weights(CON_STATE* scg, NETWORK_STATE* net, int thr_no) override {
    if(!learn) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    float* wts = cg->OwnCnVar(WT);      float* dwts = cg->OwnCnVar(DWT);    float* fwts = cg->OwnCnVar(FWT);
    float* swts = cg->OwnCnVar(SWT);    float* scales = cg->OwnCnVar(SCALE);
    const int sz = cg->size;

    if(wt_bal.on) {
      if(slow_wts.on) {
        for(int i=0; i<sz; i++) {
          LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i, net);
          LEABRA_CON_STATE* rcg = (LEABRA_CON_STATE*)ru->RecvConState(net, cg->other_idx);
          C_Compute_Weights_CtLeabraXCAL_slow
            (wts[i], dwts[i], fwts[i], swts[i], scales[i],rcg->wb_inc, rcg->wb_dec);
        }
      }
      else {
        for(int i=0; i<sz; i++) {
          LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i, net);
          LEABRA_CON_STATE* rcg = (LEABRA_CON_STATE*)ru->RecvConState(net, cg->other_idx);
          C_Compute_Weights_CtLeabraXCAL
            (wts[i], dwts[i], fwts[i], swts[i], scales[i], rcg->wb_inc, rcg->wb_dec);
        }
      }
    }
    else {
      if(slow_wts.on) {
        for(int i=0; i<sz; i++) {
          C_Compute_Weights_CtLeabraXCAL_slow
            (wts[i], dwts[i], fwts[i], swts[i], scales[i], 1.0f, 1.0f);
        }
      }
      else {
        for(int i=0; i<sz; i++) {
          C_Compute_Weights_CtLeabraXCAL
            (wts[i], dwts[i], fwts[i], swts[i], scales[i], 1.0f, 1.0f);
        }
      }
    }
  }


  INLINE virtual void 	Compute_WtBal(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(!learn || cg->size < 1 || !wt_bal.on) return;
    float sum_wt = 0.0f;
    for(int i=0; i<cg->size; i++) {
      sum_wt += cg->PtrCn(i,WT,net);
    }
    sum_wt /= (float)cg->size;
    cg->wt_avg = sum_wt;
    wt_bal.WtBal(sum_wt, cg->wb_inc, cg->wb_dec);
  }
  // #IGNORE compute weight balance factors

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
  // 	Bias Weights

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
    lrate = .04f;    cur_lrate = .02f;  lrs_mult = 1.0f;
  }
    
