// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  STATE_CLASS(TDRewIntegSpec)	rew_integ;	// misc specs for TDRewIntegUnitSpec

  INIMPL virtual void  Compute_TDRewInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);

  INLINE void	Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_TDRewInteg(u, net, thr_no);
  }
    
  INLINE void	Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_TDRewInteg(u, net, thr_no);
  }    

  // nullify other major routines:
  INLINE void	Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override { };
  INLINE void 	Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void	Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    act_range.min = -100.0f;
    act_range.max = 100.0f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_TDRewIntegUnitSpec; }

