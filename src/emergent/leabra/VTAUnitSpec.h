// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  enum  DaValence {             // which valence of dopamine should we compute?
    DA_P,                       // positive-valence oriented dopamine -- burst for positive valence outcomes better than expected, dip for worse than expected, dip for negative valence outcomes and (maybe) burst for for less negative than expected
    DA_N,                       // negative-valence oriented dopamine -- burst for negative valence outcomes worse than expected, dip for opposite, and dip for positive outcomes(?)
  };

  DaValence       da_val;       // what valence of dopamine should we compute?
  STATE_CLASS(PVLVDaSpec)      da;           // parameters for the pvlv da computation
  STATE_CLASS(PVLVDaGains)     gains;        // gains for various parameters
  STATE_CLASS(LVBlockSpec)     lv_block;     // how LV signals are blocked by PV and LHbRMTg dip signals -- there are good reasons for these signals to block LV, because they reflect a stronger overall signal about outcomes, compared to the more "speculative" LV signal

  INIMPL virtual void  Send_Da(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // send the da value to sending projections: every cycle
  INIMPL virtual void  Compute_DaP(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // compute positive-valence dopamine 
  INIMPL virtual void  Compute_DaN(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // compute negative-valence dopamine

  INIMPL virtual bool  GetRecvLayers_P
    (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, LEABRA_LAYER_STATE*& pospv_lay,
     LEABRA_LAYER_STATE*& pptg_lay, LEABRA_LAYER_STATE*& lhb_lay,
     LEABRA_LAYER_STATE*& vspatchposd1_lay, LEABRA_LAYER_STATE*& vspatchposd2_lay,
     LEABRA_LAYER_STATE*& vspatchnegd1_lay, LEABRA_LAYER_STATE*& vspatchnegd2_lay);
    // get the recv layers to VTAp (DA_P case)
  
//  INIMPL virtual bool  GetRecvLayers_N
//    (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, LEABRA_LAYER_STATE*& negpv_lay,
//     LEABRA_LAYER_STATE*& pptg_lay_n,  LEABRA_LAYER_STATE*& lhb_lay,
//     LEABRA_LAYER_STATE*& vspatchnegd1_lay, LEABRA_LAYER_STATE*& vspatchnegd2_lay);
  INIMPL virtual bool  GetRecvLayers_N
    (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, LEABRA_LAYER_STATE*& negpv_lay,
     LEABRA_LAYER_STATE*& lhb_lay, LEABRA_LAYER_STATE*& vspatchnegd1_lay,
     LEABRA_LAYER_STATE*& vspatchnegd2_lay);
  // get the recv layers to VTAn (DA_N case)

  INLINE void   Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                   int thr_no) override { };
  INIMPL void   Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                 int thr_no) override;
  INLINE void   Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                  int thr_no) override {
    Compute_Act_Rate(u, net, thr_no);
  }
    
  INLINE void  Compute_Act_Post(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                int thr_no) override {
    inherited::Compute_Act_Post(u, net, thr_no);
    Send_Da(u, net, thr_no);      // note: can only send modulators during post!!
  }

  INLINE void   Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void   Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    da_val = DA_P;    deep_raw_qtr = Q4;
    act_range.max = 2.0f;    act_range.min = -2.0f;    act_range.UpdateRange();
    clamp_range.max = 2.0f;  clamp_range.min = -2.0f;  clamp_range.UpdateRange();
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_VTAUnitSpec; }

