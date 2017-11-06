// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  STATE_CLASS(TANActSpec)      tan;             // parameters for computing TAN activation

  INIMPL virtual void  Send_ACh(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // send the ach value to sending projections: every cycle
  INIMPL virtual void  Compute_PlusPhase_Netin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // compute netin from plus phase activations from marker cons inputs

  INLINE virtual void  Compute_PlusPhase_Act(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    u->act_eq = u->act_nd = u->act = u->net = u->ext;
    u->da = 0.0f;
  }
  // activation from netin

  INLINE void  Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(tan.plus_fm_pv_vs && (net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE)) {
      Compute_PlusPhase_Netin(u, net, thr_no);
    }
    else {
      inherited::Compute_NetinInteg(u, net, thr_no);
    }
  }
  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(tan.plus_fm_pv_vs && (net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE)) {
      Compute_PlusPhase_Act(u, net, thr_no);
    }
    else {
      inherited::Compute_Act_Rate(u, net, thr_no);
    }
  }
    
  INLINE void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(tan.plus_fm_pv_vs && (net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE)) {
      Compute_PlusPhase_Act(u, net, thr_no);
    }
    else {
      inherited::Compute_Act_Spike(u, net, thr_no);
    }
  }
    
  INLINE void  Compute_Act_Post(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Post(u, net, thr_no);
    // must send all modulators in act_post
    if(tan.plus_fm_pv_vs && (net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE)) {
      if(tan.send_plus) {
        Send_ACh(u, net, thr_no);
      }
    }
    else {
      if(Quarter_DeepRawNow(net->quarter))
        Send_ACh(u, net, thr_no);
    }
  }
  
  INLINE void Initialize_core() {
    deep_raw_qtr = QALL;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_TANUnitSpec; }

