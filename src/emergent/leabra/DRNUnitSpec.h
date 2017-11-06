// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  STATE_CLASS(DRN5htSpec)      se;             // parameters for computing serotonin values

  INIMPL virtual void  Send_Se(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // send the se value to sending projections: every cycle
  INIMPL virtual void  Compute_Se(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // compute the se value based on recv projections from PV and bodily state layers

  INLINE void  Init_Weights(UNIT_STATE* ru, NETWORK_STATE* rnet, int thr_no) override {
    inherited::Init_Weights(ru, rnet, thr_no);
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ru;
    u->sev = se.se_base;
  }

  INLINE void  Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thread_no)
    override { };
  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_Se(u, net, thr_no);
  }
    
  INLINE void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_Se(u, net, thr_no);
  }    
  INLINE void  Compute_Act_Post(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Post(u, net, thr_no);
    Send_Se(u, net, thr_no);      // note: can only send modulators during post!!
  }

  INLINE void  Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void  Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_DRNUnitSpec; }

