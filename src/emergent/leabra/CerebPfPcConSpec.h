// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  float         nerr_lrate;      // no-error learning rate value -- weights increase at this rate * cur_lrate (i.e., this is relative to the basic learning rate -- typically smaller)

  INLINE void C_Compute_dWt_PfPc(float& dwt, const float gran_act,
                                 const float purk_minus, const float purk_plus)
  { if(purk_plus != 0.0f) dwt += -cur_lrate * purk_plus * gran_act * purk_minus;
    else dwt += cur_lrate * nerr_lrate * gran_act;  }
  // #IGNORE

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);

    const float gran_act = su->syn_nr; // special lagged act value for CerebGranuleUnitSpec
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = cg->UnState(i,net);
      if(ru->lesioned()) continue;
      C_Compute_dWt_PfPc(dwts[i], gran_act, ru->act_eq, ru->targ);
      // target activation trains relative to act_eq
    }
  }

  INLINE void Initialize_core() {
    nerr_lrate = 0.1f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_CerebPfPcConSpec; }

  
