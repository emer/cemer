// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  INIMPL virtual void  Send_Thal(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // send the act value as thal to sending projections: every cycle

  INLINE void  Compute_Act_Post(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Post(u, net, thr_no);
    Send_Thal(u, net, thr_no);
  }

  // no learning in this one..
  INLINE void 	Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void	Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    deep.role = STATE_CLASS(DeepSpec)::TRC;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_ThalSendUnitSpec; }

