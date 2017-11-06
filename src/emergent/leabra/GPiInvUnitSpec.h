// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  Quarters       gate_qtr;    // #CAT_GPi quarter(s) during which GPi gating takes place -- typically Q1 and Q3
  STATE_CLASS(GPiGateSpec)    gate;        // #CAT_GPi timing for gating  within gate_qtr
  STATE_CLASS(GPiMiscSpec)    gpi;         // #CAT_GPi parameters controlling the gpi functionality: how to weight the Go vs. NoGo pathway inputs, and gating threshold

  INLINE  bool Quarter_GateNow(int qtr) { return gate_qtr & (1 << qtr); }
  // #CAT_Activation test whether gating happens in this quareter

  INIMPL void  Compute_NetinRaw(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override;

  INIMPL virtual void  Send_Thal(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // send the act value as thal to sending projections: every cycle

  INLINE void  Compute_Act_Post(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Post(u, net, thr_no);
    Send_Thal(u, net, thr_no);    // note: essential to send all modulation in Act_Post
  }

  // no learning in this one..
  INLINE void  Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void  Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    gate_qtr = Q1_Q3;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_GPiInvUnitSpec; }

