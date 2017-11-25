// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  enum DaModType {
    NO_DA_MOD,                  // do not modulate the learning by recv unit dopamine value (da_p)
    DA_MOD,                     // modulate the learning by multiplying directly by the recv unit dopamine value (da_p) -- this will change the sign of learning as a function of the sign of the dopamine value
    DA_MOD_ABS,                 // modulate the learning by multiplying by the absolute value of the recv unit dopamine (abs(da_p)) -- this does not change the sign, only the magnitude of learning
  };

  DaModType     da_mod;         // how does receiving unit dopamine modulate learning (or not)?

  // everything can use one dwt with post-soft-bound because no hebbian term
  INLINE void C_Compute_dWt_Delta_NoDa(float& dwt, const float ru_act_p, 
                                       const float ru_act_m, const float su_act) {
    dwt += cur_lrate * (ru_act_p - ru_act_m) * su_act;
  }
  // #IGNORE
  INLINE void C_Compute_dWt_Delta_Da(float& dwt, const float ru_act_p, 
                                     const float ru_act_m, const float su_act,
                                     const float da_p) {
    dwt += cur_lrate * da_p * (ru_act_p - ru_act_m) * su_act;
  }
  // #IGNORE dopamine multiplication

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    const float su_act = su->act_m; // note: using act_m
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    if(da_mod == NO_DA_MOD) {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = cg->UnState(i, net);
        if(ru->lesioned()) continue;
        C_Compute_dWt_Delta_NoDa(dwts[i], ru->act_p, ru->act_m, su_act);
      }
    }
    else if(da_mod == DA_MOD) {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = cg->UnState(i, net);
        if(ru->lesioned()) continue;
        C_Compute_dWt_Delta_Da(dwts[i], ru->act_p, ru->act_m, su_act, ru->da_p);
      }
    }
    else {                      // DA_MOD_ABS
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = cg->UnState(i, net);
        if(ru->lesioned()) continue;
        C_Compute_dWt_Delta_Da(dwts[i], ru->act_p, ru->act_m, su_act, fabsf(ru->da_p));
      }
    }
  }

  INLINE void Initialize_core() {
    da_mod = NO_DA_MOD;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_DaDeltaConSpec; }

