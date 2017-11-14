// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  STATE_CLASS(CElAmygLearnSpec)   cel_learn; // central amygdala, lateral learning specs

  INLINE void C_Compute_dWt_CEl_Delta
    (float& dwt, const float su_act, const float ru_act, const float ru_act_prv,
     const float da_p, const float lrate_eff) {
    
    float ru_act_delta = ru_act - ru_act_prv;
    if(fabsf(ru_act_delta) < cel_learn.act_delta_thr) { ru_act_delta = 0.0f; }
    float delta = lrate_eff * su_act * (ru_act_delta);
    // dopamine signal further modulates learning
    float da_lrate = cel_learn.dalr_base + cel_learn.dalr_gain * fabsf(da_p);
    dwt += da_lrate * delta;
  }
  // #IGNORE abs(da) modulated delta learning

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    
    float su_act = su->act_q0;  // previous trial
    float* dwts = cg->OwnCnVar(DWT);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, net, thr_no, clrate, deep_on, bg_lrate, fg_lrate);

    const int sz = cg->size;
    
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = cg->UnState(i, net);
      // screen out spurious da signals due to tiny VSPatch-to-LHb signals
      float ru_da_p = ru->da_p;
      if(fabsf(ru_da_p) < cel_learn.da_lrn_thr) { ru_da_p = 0.0f; }
      
      C_Compute_dWt_CEl_Delta(dwts[i], su_act, ru->act_eq, ru->act_q0, ru_da_p, clrate);
    }
  }

  INLINE void Initialize_core() {
    wt_sig.gain = 1.0f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_CElAmygConSpec; }

