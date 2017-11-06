// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  INIMPL virtual void  ClampFromECin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // clamp ECout values from ECin values, in plus phase

  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(net->quarter == 3 && net->train_mode == NETWORK_STATE::TRAIN)
      ClampFromECin(u, net, thr_no);
    else
      inherited::Compute_Act_Rate(u, net, thr_no);
  }
   
  INLINE void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(net->quarter == 3) 
      ClampFromECin(u, net, thr_no);
    else
      inherited::Compute_Act_Spike(u, net, thr_no);
  }
    
  INIMPL float Compute_SSE(UNIT_STATE* u, NETWORK_STATE* net, int thr_no, bool& has_targ) override;

  INLINE void Initialize_core() {
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_ECoutUnitSpec; }

