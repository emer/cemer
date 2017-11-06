// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  float         d_net_gain;     // extra multiplicative gain factor to apply to the net input delta -- brings this delta back into the typical range so that more standard activation functions can be used
  bool          clamp_act;      // clamp the activation directly instead of driving net input -- still applies d_net_gain -- provides a purely linear y-dot reference signal
  float         act_thr;        // #DEF_0 minimum activation to have before passing excitatory signal to VTA layers

  INIMPL void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override;
  INIMPL void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override;
  INLINE void  Quarter_Final(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Quarter_Final(u, net, thr_no);
    if(net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE) {
      //u->misc_2 = fminf(u->act_dif, 0.0f); // save any neg phasic DA to filter rebounds
      u->misc_1 = u->net;       // save new guy for next time -- this is the raw net..
    }
  }
    
  INLINE void Initialize_core() {
    d_net_gain = 1.0f;
    clamp_act = true;
    act_thr = 0.0f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_PPTgUnitSpec; }

