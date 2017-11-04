// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  // special!
  INLINE bool  DoesStdNetin() override { return false; }
  INLINE bool  DoesStdDwt() override { return false; }
  INLINE bool  IsDeepRawCon() override { return true; }
  INLINE void  Trial_Init_Specs(LEABRA_NETWORK_STATE* net) override {
    inherited::Trial_Init_Specs(net);
    net->deep.raw_net = true;
  }

  INLINE void Send_DeepRawNetDelta(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net,
                                   int thr_no, const float su_act_delta) {
    const float su_act_delta_eff = cg->scale_eff * su_act_delta;
    float* wts = cg->OwnCnVar(WT);
    float* send_deepnet_vec = net->ThrSendDeepRawNetTmp(thr_no); // no per-prjn
#ifdef TA_VEC_USE
    Send_NetinDelta_vec(cg, su_act_delta_eff, send_deepnet_vec, wts);
#else
    CON_STATE_LOOP(cg, C_Send_NetinDelta(wts[i], send_deepnet_vec,
                                         cg->UnIdx(i), su_act_delta_eff));
#endif
  }
  // #IGNORE sender-based activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thr_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // don't send regular net inputs..
  INLINE void Send_NetinDelta(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no, 
                              const float su_act_delta) override { };
  INLINE float Compute_Netin(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override
  { return 0.0f; }

  INLINE void   Init_Weights_sym_s(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override {
    if(!wt_limits.sym) return;
    UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    LAYER_STATE* slay = cg->GetPrjnSendLayer(net);
    PRJN_STATE* prjn = cg->GetPrjnState(net);
    PRJN_STATE* oprjn = slay->FindSendPrjnTo(net, prjn->recv_lay_idx); // find other prjn to this same layer -- this assumes that deep prjn always comes *after* the other one..
    if(!oprjn || oprjn == prjn) return; // don't do if it is us!
    CON_STATE* ocg = su->SendConStatePrjn(net, oprjn);
    if(!ocg) return;
    int mx = MIN(cg->size, ocg->size);
    for(int i=0; i<mx; i++) {     // assume to be in 1-to-1 corresp..
      cg->OwnCn(i, WT) = ocg->OwnCn(i, WT);
    }
  }
  // #IGNORE we copy our weights from other sending projection to this same recv layer, if wt_limits.sym is active

  INLINE void Initialize_core() {
    learn = false;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_SendDeepRawConSpec; }

