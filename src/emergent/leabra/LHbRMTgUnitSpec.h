// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
//{

  STATE_CLASS(LHbRMTgSpecs)   lhb;         // lhb options and misc specs
  STATE_CLASS(LHbRMTgGains)   gains;       // gain parameters (multiplicative constants) for various sources of inputs

  INIMPL virtual void	Compute_Lhb(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // compute the LHb value based on recv projections from VSMatrix_dir/ind, VSPatch_dir/ind, and PV_pos/neg
  
  INIMPL virtual bool  GetRecvLayers
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, 
   LEABRA_LAYER_STATE*& pv_pos_lay,   LEABRA_LAYER_STATE*& vspatch_pos_D1_lay,
   LEABRA_LAYER_STATE*& vspatch_pos_D2_lay,   LEABRA_LAYER_STATE*& vsmatrix_pos_D1_lay,
   LEABRA_LAYER_STATE*& vsmatrix_pos_D2_lay,   LEABRA_LAYER_STATE*& pv_neg_lay,
   LEABRA_LAYER_STATE*& vspatch_neg_D1_lay,   LEABRA_LAYER_STATE*& vspatch_neg_D2_lay,
   LEABRA_LAYER_STATE*& vsmatrix_neg_D1_lay,   LEABRA_LAYER_STATE*& vsmatrix_neg_D2_lay);
  // get the recv layers..
  
  INLINE void  Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override { };
  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(Quarter_DeepRawNow(net->quarter)) {
      Compute_Lhb(u, net, thr_no);
    }
    else {
      // todo: why not all the time?
    }
  }

  INLINE void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_Act_Rate(u, net, thr_no);
  }

  INLINE void 	Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void	Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    deep_raw_qtr = Q4;
    act_range.max = 2.0f;    act_range.min = -2.0f;    act_range.UpdateRange();
    clamp_range.max = 2.0f;  clamp_range.min = -2.0f;  clamp_range.UpdateRange();
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LHbRMTgUnitSpec; }
