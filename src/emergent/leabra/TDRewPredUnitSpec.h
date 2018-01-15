// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  INLINE void  Init_Acts(UNIT_STATE* ru, NETWORK_STATE* rnet, int thr_no) override {
    inherited::Init_Acts(ru, rnet, thr_no);
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ru;
    u->misc_1 = 0.0f;             // reset..
  }
  
  INLINE void   Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(!Quarter_DeepRawNow(net->quarter)) { // plus phase marker..
      u->ext = u->misc_1;                  // clamp to previous prediction
      u->act_eq = u->act_nd = u->act = u->net = u->ext;
      u->da = 0.0f;
    }
    else {
      u->act_eq = u->act_nd = u->act = u->net;            // linear!
      u->da = 0.0f;
    }
  }
    
  INLINE void   Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no)  override {
    Compute_Act_Rate(u, net, thr_no);
  }

  INLINE void  Quarter_Final(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(Quarter_DeepRawNow(net->quarter)) { // plus phase marker..
      u->misc_1 = u->act_eq;               // save current prediction for next trial
    }
    inherited::Quarter_Final(u, net, thr_no); // this will record as act_p
  }
  

  INLINE void Initialize_core() {
    act_range.min = -100.0f;
    act_range.max = 100.0f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_TDRewPredUnitSpec; }

