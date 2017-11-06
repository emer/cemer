// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  STATE_CLASS(BFCSAChSpec)     ach;          // parameters for the ach computation

  virtual void  Send_ACh(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // send the ach value to sending projections: start of quarters
  virtual void  Compute_ACh(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // compute ach value from delta change values: at end of trial

  INLINE void  Quarter_Init_Unit(LEABRA_UNIT_STATE* uv, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Quarter_Init_Unit(uv, net, thr_no);
    Send_ACh(uv, net, thr_no);
  }
  INLINE void  Compute_ActTimeAvg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_ActTimeAvg(u, net, thr_no);
    Compute_ACh(u, net, thr_no);
  }
    
  INLINE void  Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override { };
  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    u->act_eq = u->act_nd = u->act = u->net = u->misc_1;
    u->da = 0.0f;
  }
  INLINE void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_Act_Rate(u, net, thr_no);
  }
  
  INLINE void  Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void  Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    // deep_raw_qtr = Q4;
    // act_range.max = 2.0f;   act_range.min = -2.0f;   act_range.UpdateRange();
    // clamp_range.max = 2.0f; clamp_range.min = -2.0f; clamp_range.UpdateRange();
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_BFCSUnitSpec; }

