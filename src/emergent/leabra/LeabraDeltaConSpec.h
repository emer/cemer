// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  inline void C_Compute_dWt_Delta(float& dwt, const float ru_act_p, 
                                  const float ru_act_m, const float su_act) {
    dwt += cur_lrate * (ru_act_p - ru_act_m) * su_act;
  }
  // #IGNORE

  inline void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    const float su_act = su->act_m; // note: using act_m
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = cg->UnState(i, net);
      C_Compute_dWt_Delta(dwts[i], ru->act_p, ru->act_m, su_act);
    }
  }

  INLINE void Initialize_core() {
    wt_limits.sym = false;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LeabraDeltaConSpec; }

