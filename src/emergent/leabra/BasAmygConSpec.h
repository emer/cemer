  STATE_CLASS(BasAmygLearnSpec)      ba_learn; // basal amygdala learning specs

  inline float  GetDa(float da, bool d2r) {
    if(da < 0.0f) da *= ba_learn.dip_da_gain; else da *= ba_learn.burst_da_gain;
    if(d2r) da = -da;
    return da;
  }
  // get effective dopamine signal taking into account gains and reversal by D2R

  inline void C_Compute_dWt_BasAmyg_Delta
    (float& dwt, const float su_act, const float ru_act, const float ru_act_prv,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    float delta = lrate_eff * su_act * (ru_act - ru_act_prv);
    if(delta < 0.0f)
      delta *= ba_learn.delta_neg_lrate;
    float da_lrate = ba_learn.delta_da_base + ba_learn.delta_da_gain * fabsf(da);
    dwt += da_lrate * delta;
  }
  // #IGNORE basic delta
  
  inline void C_Compute_dWt_BasAmyg_Acq_UsDelta
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    if(us > 0.01f) {
      float delta = lrate_eff * su_act * (us - ru_act);
      if(ba_learn.delta_da) {
        delta *= fabsf(da);
      }
      dwt += delta;
    }
    else { // using us_delta, but no US present this timestep
      dwt += lrate_eff * su_act * ru_act * da;
    }
  }
  // #IGNORE: acq us delta

  inline void C_Compute_dWt_BasAmyg_Acq_DaSuRu
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * su_act * fmaxf(us, ru_act) * da;
  }
  // #IGNORE: not used -- here for reference

  inline void C_Compute_dWt_BasAmyg_Ext_UsDelta
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    if(us > 0.01f) {
      float delta = lrate_eff * su_act * ((1.0f-us) - ru_act); // inverse us..
      if(ba_learn.delta_da) {
        delta *= fabsf(da);
      }
      dwt += delta;
    }
    else {
      dwt += lrate_eff * su_act * ru_act * da;
    }
  }
  // #IGNORE ext us delta
  inline void C_Compute_dWt_BasAmyg_Ext_DaSuRu
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * su_act * ru_act * da;
  }
  // #IGNORE not used -- here for reference

  inline void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->ThrOwnUnState(net, thr_no);
    
    float su_act = su->act_q0;  // previous trial
    float* dwts = cg->OwnCnVar(DWT);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, net, thr_no, clrate, deep_on, bg_lrate, fg_lrate);

    const int sz = cg->size;
    
    if(ba_learn.learn_rule == STATE_CLASS(BasAmygLearnSpec)::DELTA) {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i, net);
        bool d2r = (ru->HasUnitFlag(LEABRA_UNIT_STATE::D2R));
        bool acq = (ru->HasUnitFlag(LEABRA_UNIT_STATE::ACQUISITION));
        if(acq) {
          C_Compute_dWt_BasAmyg_Delta(dwts[i], su_act, ru->act_eq, ru->act_q0, ru->da_p, d2r, clrate);
        }
        else {
          const float ru_act_eff = fmaxf(ru->deep_lrn, ru->act_eq);
          C_Compute_dWt_BasAmyg_Delta(dwts[i], su_act, ru_act_eff, ru->act_q0, ru->da_p, d2r, clrate);
        }
      }
    }
    else {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i, net);
        bool d2r = (ru->HasUnitFlag(LEABRA_UNIT_STATE::D2R));
        bool acq = (ru->HasUnitFlag(LEABRA_UNIT_STATE::ACQUISITION));
        if(acq) {
          C_Compute_dWt_BasAmyg_Acq_UsDelta(dwts[i], su_act, ru->act_eq, ru->deep_raw_net,
                                            ru->da_p, d2r, clrate);
        }
        else {
          // this is the key for learning: up-state or actual ru activation
          const float ru_act_eff = fmaxf(ru->deep_lrn, ru->act_eq);
          C_Compute_dWt_BasAmyg_Ext_UsDelta(dwts[i], su_act, ru_act_eff, ru->deep_raw_net,
                                            ru->da_p, d2r, clrate);
        }
      }
    }
  }

  INLINE void Initialize_core() {
    wt_sig.gain = 1.0f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_BasAmygConSpec; }

