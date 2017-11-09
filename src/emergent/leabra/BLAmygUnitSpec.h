// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  DAReceptor    dar;            // type of predominant dopamine receptor: D1 vs. D2 -- determines whether primarily appetitive / relief (D1) or aversive / disappointment (D2)
  STATE_CLASS(BLAmygDaMod)   bla_da_mod;     // extra parameters for dopamine modulation of activation for BLA amyg units; or, just modulate learning (wt changes) directly w/o affecting actual unit activations
  STATE_CLASS(BLAmygAChMod)  bla_ach_mod;   // ach modulation of activation for BLA amyg units
  bool          deep_mod_zero;  // for unit group-based extinction-coding layers; modulation coming from the corresponding BLA acquisition layer via deep_mod_net -- when this modulation signal is below deep.mod_thr, does it have the ability to zero out the extinction activations?  i.e., is the modulation required to enable extinction firing?
  

  INIMPL void  Init_UnitState(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override;

  INIMPL float Compute_DaModNetin(LEABRA_UNIT_STATE* uv, LEABRA_NETWORK_STATE* net,
                                  int thr_no, float& net_syn) override;

  INIMPL void  Compute_DeepMod(LEABRA_UNIT_STATE* uv, LEABRA_NETWORK_STATE* net,
                               int thr_no) override;
  INIMPL float Compute_NetinExtras(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                   int thr_no, float& net_syn) override;
  
  INIMPL void Compute_ActFun_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                  int thr_no) override;
  
  INLINE void  Quarter_Final_RecVals(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                     int thr_no) override {
    inherited::Quarter_Final_RecVals(u, net, thr_no);
    if(net->quarter == 3) {
      u->act_dif = u->act_p - u->act_q0; // prior trial -- this is learning delta
    }
  }
  
  INLINE void Initialize_core() {
    dar = D1R;    deep.on = true;    deep.role = STATE_CLASS(DeepSpec)::DEEP;
    deep.raw_thr_rel = 0.1f;    deep.raw_thr_abs = 0.1f;
    deep.mod_thr = 0.1f;          // note: was .01 in hard code
    deep_mod_zero = true;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_BLAmygUnitSpec; }

