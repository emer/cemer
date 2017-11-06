// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  enum UpdateCriteria {
    UC_TRIAL = 0, // updates every trial (traditional "ContextLayer" behavior)
    UC_MANUAL, // manually updated via TriggerUpdate
    UC_N_TRIAL, // updates every n trials
  };
  
  UpdateCriteria update_criteria; // #DEF_UC_TRIAL #NO_SAVE_EMPTY how to determine when to copy the sending layer
  STATE_CLASS(CtxtUpdateSpec) updt;   // ctxt updating constants: from hidden, from previous values (hysteresis), outputs from context (n/a on simple gate layer)
  STATE_CLASS(CtxtNSpec)      n_spec; // #CONDSHOW_ON_update_criteria:UC_N_TRIAL trials per update and optional offset for multi
  
  INIMPL virtual void Compute_Context(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Context compute context activation
  INIMPL virtual bool ShouldUpdateNow(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // #CAT_Context test whether it is time to update context rep now..

  INLINE void  Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override { };
  
  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_Context(u, net, thr_no);
  } 
  INLINE void  Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_Context(u, net, thr_no);
  }
  
  INLINE void  Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void  Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    updt.fm_prv = 0.0f;  updt.fm_hid = 1.0f;  updt.to_out = 1.0f;  update_criteria = UC_TRIAL;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LeabraContextUnitSpec; }

