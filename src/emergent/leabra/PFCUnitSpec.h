// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  STATE_CLASS(PFCGateSpec)   gate;           // PFC specifications for gating-related parameters
  STATE_CLASS(PFCMaintSpec)  maint;          // PFC specifications for maintenance-related parameters
  int           n_dyns;         // number of different temporal dynamic profiles for different PFC units, all triggered by a single gating event -- each row of units within a PFC unit group shares the same dynamics -- there should be an even multiple of n_dyns rows (y unit group size) per unit group
  int           alloc_dyns;  // #READ_ONLY #NO_COPY #NO_SAVE number of dyn elements allocated
  STATE_CLASS(PFCDynEl)*   dyns_m; // #IGNORE list of dynamic elements


  INIMPL void  AllocDyns(int ns);
  // #IGNORE allocate the dyns_m array -- done by parent emergent obj
  INIMPL void  FreeDyns();
  // #IGNORE free any allocated dyns_m

  INIMPL virtual float InitDynVal(int row);
  // get initial dynamic value
  INIMPL virtual float UpdtDynVal(int row, float time_step);
  // get update dynamic value as function of teim step
  
  INIMPL virtual void GetThalCntFromSuper(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #IGNORE deep guys get thal_cnt from super
  INIMPL virtual void Compute_PFCGating(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // called 1/2 way through phase prior to official gating phase, determines if gating happened and sets thal_cnt to 0 if so, and clears existing activations per params -- output gating layers call this 1 cycle prior to maint layers, to allow maint to override clear with new gating if present
  INIMPL virtual void ClearOtherMaint(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // clear maintenance in other layers we project to using MarkerConSpec

  INIMPL float Compute_NetinExtras(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                   int thr_no, float& net_syn) override;
  INIMPL void  Compute_DeepRaw(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override;
  
  INLINE void  Quarter_Init_Deep(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

    // this is first entry point for Quarter_Init -- because of parallel, need to
    // sync with super here and then do rest of updating in next step
    if(deep.IsDeep()) {
      GetThalCntFromSuper(u, net, thr_no); // sync us up with super
    }
  }
    
  INLINE void  SaveGatingAct(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override { };

  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Rate(u, net, thr_no);
    // note: we must compute this here based on thal values we got last cycle in Act_Post
    // and we do it after getting new acts so that we can clear them out when gating happens
    Compute_PFCGating(u, net, thr_no);
  }
    
  INLINE void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Spike(u, net, thr_no);
    Compute_PFCGating(u, net, thr_no);
  }
    
  INIMPL void  Send_DeepCtxtNetin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override;
  INIMPL void  Compute_DeepStateUpdt(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override;

  INLINE void  ClearDeepActs(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::ClearDeepActs(u, net, thr_no);
    u->thal_cnt = -1.0f; // -1.0 is no gating, 0.0 is beginning of gating, so need to initialize to -1
    u->misc_1 = 0.0f;
  }
  
  INLINE void Initialize_core() {
    n_dyns = 0;    alloc_dyns = 0;  dyns_m = NULL;
    deep_raw_qtr = Q2_Q4;    deep.on = true;    deep.raw_thr_rel = 0.1f;
    deep.raw_thr_abs = 0.1f; // todo??
    deep.mod_min = 1.0f;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_PFCUnitSpec; }

