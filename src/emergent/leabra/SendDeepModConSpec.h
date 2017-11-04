// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  INLINE bool  DoesStdNetin() override { return false; }
  INLINE bool  IsDeepModCon() override { return true; }
  INLINE void  Trial_Init_Specs(LEABRA_NETWORK_STATE* net) override {
    inherited::Trial_Init_Specs(net);
    net->deep.mod_net = true;
  }

  INLINE void Send_DeepModNetDelta(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net,
                                    int thr_no, const float su_act_delta) {
    const float su_act_delta_eff = cg->scale_eff * su_act_delta;
    float* wts = cg->OwnCnVar(WT);
    float* send_deepnet_vec = net->ThrSendDeepModNetTmp(thr_no);
#ifdef TA_VEC_USE
    Send_NetinDelta_vec(cg, su_act_delta_eff, send_deepnet_vec, wts);
#else
    CON_STATE_LOOP(cg, C_Send_NetinDelta(wts[i], send_deepnet_vec,
                                         cg->UnIdx(i), su_act_delta_eff));
#endif
  }
  // #IGNORE sender-based activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // don't send regular net inputs..
  INLINE void Send_NetinDelta(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no, 
                              const float su_act_delta) override { };
  INLINE float Compute_Netin(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override
  { return 0.0f; }

  INLINE void Initialize_core() {
    learn = false;
    rnd.mean = 0.8f;
    rnd.var = 0.0f;
    wt_limits.sym = false;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_SendDeepModConSpec; }

