// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
//{

  enum AcqExt { // acquisition (Go, On) vs. extinction (NoGo, Off) type
    ACQ,        // acquisition (Go, On) neurons -- get direct US projections and learn CS / context etc associations based on phasic dopamine modulation
    EXT,        // extinction (NoGo, Off) neurons -- get indirect US estimates from Acq inhibitory projections and learn CS / context etc associations based phasic dopamine modulatio
  };

  AcqExt        acq_ext;        // acquisition vs. extinction sub-type
  Valence       valence;        // US valence coding -- appetitive vs. aversive
  DAReceptor    dar;            // #READ_ONLY #SHOW type of dopamine receptor: D1 vs. D2 -- computed automatically from acq_ext and valence
  STATE_CLASS(CElAmygDaMod)  cel_da_mod;     // extra parameters for dopamine modulation of activation for CEl amyg units
  bool          deep_mod_zero;  // for unit group-based extinction-coding layers; modulation coming from the corresponding acquisition layer via deep_mod_net -- when this modulation signal is below deep.mod_thr, does it have the ability to zero out the extinction activations?  i.e., is the modulation required to enable extinction firing?
  
  INIMPL void  Init_UnitState(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override;

  INIMPL float Compute_DaModNetin(LEABRA_UNIT_STATE* uv, LEABRA_NETWORK_STATE* net,
                                  int thr_no, float& net_syn) override;

  INIMPL void  Compute_DeepMod(LEABRA_UNIT_STATE* uv, LEABRA_NETWORK_STATE* net,
                               int thr_no) override;
  INIMPL float Compute_NetinExtras(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                   int thr_no, float& net_syn) override;
  INIMPL void  Compute_ActFun_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                   int thr_no) override;
  INLINE void  Quarter_Final_RecVals(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                     int thr_no) override {
    inherited::Quarter_Final_RecVals(u, net, thr_no);
    if(net->quarter == 3) {
      u->act_dif = u->act_p - u->act_q0; // prior trial -- this is learning delta
    }
  }

  INLINE void Initialize_core() {
    acq_ext = ACQ;    valence = APPETITIVE;    dar = D1R;
    deep.on = true;    deep.role = STATE_CLASS(DeepSpec)::DEEP;
    deep.raw_thr_rel = 0.1f;    deep.raw_thr_abs = 0.1f;
    deep.mod_thr = 0.01f;         // default .1
    deep_mod_zero = true;
    // these are params to produce low-level baseline tonic activation
    init.v_m = 0.55f;    g_bar.l = 0.6f;               // todo: maybe higher?
    e_rev.l = 0.55f;    e_rev.i = 0.4f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_CElAmygUnitSpec; }

