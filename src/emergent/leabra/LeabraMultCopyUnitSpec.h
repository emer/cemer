// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  bool		one_minus;	// if true, use 1-mult activation as the multiplier -- this is useful with mutually exclusive options in the multipliers, where you hook each up to the *other* alternative, such that this other alternative inhibits this option
  float		mult_gain;	// multiplier gain -- multiply the mult act value by this gain, with a max overall resulting net mult value of 1

  INIMPL virtual void  Compute_MultCopy(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  
  INLINE void  Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override { };
  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_MultCopy(u, net, thr_no);
  }
  INLINE void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_MultCopy(u, net, thr_no);
  }
  INLINE void  Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void  Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    one_minus = false;
    mult_gain = 1.0f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LeabraMultCopyUnitSpec; }
