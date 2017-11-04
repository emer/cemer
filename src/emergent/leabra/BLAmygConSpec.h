// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  STATE_CLASS(BLAmygLearnSpec)   bla_learn; // central amygdala, lateral learning specs

  INLINE void C_Compute_dWt_BLA_Delta
    (float& dwt, const float su_act, const float ru_act, const float ru_act_prv,
    const float da_p, const float lrate_eff, const float wt) {
      
    float ru_act_delta = ru_act - ru_act_prv;
     
    // filter tiny spurious delta act signals - needed especially for acq guys w/
    // non-zero dalr_base value
    if(fabsf(ru_act_delta) < bla_learn.act_delta_thr) { ru_act_delta = 0.0f; }
    float delta = lrate_eff * su_act * (ru_act_delta);
    
    float da_lrate = bla_learn.dalr_base + bla_learn.dalr_gain * fabsf(da_p);
    dwt += da_lrate * delta;
  }
  // #IGNORE abs(da) modulated delta learning

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->ThrOwnUnState(net, thr_no);
    
    float su_act = su->act_q0;  // previous trial
    float* dwts = cg->OwnCnVar(DWT);
    float* wts = cg->OwnCnVar(WT);
    float* scales = cg->OwnCnVar(SCALE);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, net, thr_no, clrate, deep_on, bg_lrate, fg_lrate);

    const int sz = cg->size;
    
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i, net);
      float lrate_eff = clrate; // start fresh each time thru for_loop
      
      // learning dependent on non-zero deep_lrn
      if(bla_learn.deep_lrn_mod) {
        float eff_deep_lrn = 0.0f;
        if(ru->deep_lrn > bla_learn.deep_lrn_thr) {
          eff_deep_lrn = 1.0f;
        }
        else {
          eff_deep_lrn = 0.0f;
        }
        lrate_eff *= eff_deep_lrn;
      }
      // filter any tiny spurious da signals on t2 & t4 trials - best for ext guys since
      // they have zero dalr_base value
      float ru_da_p = ru->da_p;
      if(fabsf(ru_da_p) < bla_learn.da_lrn_thr) { ru_da_p = 0.0f; }
      
      C_Compute_dWt_BLA_Delta(dwts[i], su_act, ru->act_eq, ru->act_q0, ru_da_p, lrate_eff,
                              wts[i] / scales[i]);
    }
  }

  INLINE void Initialize_core() {
    wt_sig.gain = 1.0f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_BLAmygConSpec; }

