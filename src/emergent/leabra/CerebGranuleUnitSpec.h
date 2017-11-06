  // notes on use of LeabraUnitState_cpp variables for the Granule Unit:
  // we use the stp parameters, and thus Granule units cannot have stp.on set!
  // syn_nr = time-lagged learning activation value -- this is what Purkinje cells use to learn from
  // syn_pr = time since the unit crossed the act_thr threshold activation
  // syn_kre = max activation of unit within activity window 

  // UnitSpec functions:
  // in Compute_NetinExtras -- negate net input for inhibitory competition during "inhibitory refractory period"
  // in Compute_Act, we implement delayed activation for learning

  STATE_CLASS(CerebGranuleSpecs)     cereb;  // specs for cerebellar granule units

  inline float& ActLag(LEABRA_UNIT_STATE* uv)        { return uv->syn_nr; }
  // #CAT_CerebGranule time-lagged activation value, used for learning
  inline float& TimeSinceThr(LEABRA_UNIT_STATE* uv)  { return uv->syn_pr; }
  // #CAT_CerebGranule get time since unit crossed the act_thr threshold activation
  inline float& ActMax(LEABRA_UNIT_STATE* uv)        { return uv->syn_kre; }
  // #CAT_CerebGranule get max activation of unit within activity window
 
  INIMPL virtual void  Compute_GranLearnAct(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                            int thr_no);
  // compute the granule unit learning activation as a function of time

  INLINE float  Compute_NetinExtras(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no,
                                    float& net_syn) override {
    float rval = inherited::Compute_NetinExtras(u, net, thr_no, net_syn);
    int time_since_thr = (int)TimeSinceThr(u);
    if(time_since_thr > cereb.inhib_start_time) {
      // by turning net input down here, we allow other gran cells to win the competition
      net_syn *= cereb.inhib_net_pct;
    }
    return rval;
  }
    
  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Rate(u, net, thr_no);
    Compute_GranLearnAct(u, net, thr_no);
  }
    
  INLINE void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Spike(u, net, thr_no);
    Compute_GranLearnAct(u, net, thr_no);
  }

  INLINE void Initialize_core() {
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_CerebGranuleUnitSpec; }

