// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  enum LearnActVal {            // activation value to use for learning
    PREV_TRIAL,                 // previous trial
    ACT_P,                      // act_p from current trial
    ACT_M,                      // act_m from current trial
  };
  
  LearnActVal         su_act_var;     // what variable to use for sending unit activation
  LearnActVal         ru_act_var;     // what variable to use for recv unit activation
  
  bool                d2r;            // if true, exhibits inverted D2 receptor-driven learning - LTD from phaDA bursts; LTP from dips; i.e., NoGo- / indirect-like pattern
  float               da_dip_gain;    // multply phasic dips to increase or decrease learning effect

  INLINE float GetActVal(LEABRA_UNIT_STATE* u, const LearnActVal& val) {
    switch(val) {
    case PREV_TRIAL:
      return u->act_q0;
      break;
    case ACT_P:
      return u->act_p;
      break;
    case ACT_M:
      return u->act_m;
      break;
    }
    return 0.0f;
  }    
  
  INLINE void C_Compute_dWt_Hebb_Da(float& dwt, const float ru_act, const float su_act,
                                    const float da_p, const float lrate_eff) {
    float eff_da = da_p;
    if(d2r) eff_da = -eff_da; // invert direction of learning
    if(eff_da < 0.0f) { eff_da *= da_dip_gain; }
    dwt += lrate_eff * eff_da * ru_act * su_act;
  }
  // #IGNORE dopamine multiplication

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->ThrOwnUnState(net, thr_no);
    float* dwts = cg->OwnCnVar(DWT);

    float su_act = GetActVal(su, su_act_var);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, net, thr_no, clrate, deep_on, bg_lrate, fg_lrate);
    
    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i, net);
      float lrate_eff = clrate;
      if(deep_on) {
        lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
      }
      float ru_act = GetActVal(ru, ru_act_var);
      C_Compute_dWt_Hebb_Da(dwts[i], ru_act, su_act, ru->da_p, lrate_eff);
    }
  }

  INLINE void Initialize_core() {
    su_act_var = PREV_TRIAL;
    ru_act_var = ACT_P;
    d2r = false;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_DaHebbConSpec; }

