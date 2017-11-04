// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  bool          use_trace_act_avg;
  // if true, use act_avg value as sending activation in learning rule -- else uses prior activation state, act_q0

  INLINE void C_Compute_dWt_TD(float& dwt, const float ru_da_p, const float su_act) {
    dwt += cur_lrate * ru_da_p * su_act;
  }
  // #IGNORE

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->ThrOwnUnState(net, thr_no);

    float su_act;
    if(use_trace_act_avg)
      su_act = su->act_avg;
    else
      su_act = su->act_q0;
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i,net);
      C_Compute_dWt_TD(dwts[i], ru->da_p, su_act);
    }
  }

  INLINE void  C_Compute_Weights_LinNoBound(float& wt, float& dwt, float& fwt) {
    if(dwt != 0.0f) {
      wt += dwt;
      fwt = wt;
      dwt = 0.0f;
    }
  }
  // #IGNORE compute weights -- linear, no bounds

  INLINE void Compute_Weights(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    if(!learn) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      C_Compute_Weights_LinNoBound(wts[i], dwts[i], fwts[i]);
    }
  }

  INLINE void Initialize_core() {
    use_trace_act_avg = false;    rnd.mean = 0.0f;    rnd.var = 0.0f;
    wt_limits.type = STATE_CLASS(WeightLimits)::NONE;    wt_limits.sym = false;
    wt_sig.gain = 1.0f;    wt_sig.off = 1.0f;    wt_bal.on = false;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_TDRewPredConSpec; }

