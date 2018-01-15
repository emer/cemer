// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
//{

  INIMPL virtual void   Compute_TD(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // compute the td value based on recv projections: every cycle in 1+ phases

  INIMPL virtual void  Send_TD(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // send the TD value to sending projections in da_p: every cycle

  INLINE void   Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_TD(u, net, thr_no);
  }
    
  INLINE void   Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_TD(u, net, thr_no);
  }
    
  INLINE void  Compute_Act_Post(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Post(u, net, thr_no);
    Send_TD(u, net, thr_no);      // note: can only send modulators during post!!
  }

  INLINE void Initialize_core() {
    act_range.min = -100.0f;
    act_range.max = 100.0f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_TDDeltaUnitSpec; }

