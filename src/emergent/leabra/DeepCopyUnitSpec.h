// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  enum DeepCopyVar {            // variable to copy from
    DEEP_RAW,                   // deep_raw
    DEEP_MOD,                   // deep_mod
    DEEP_LRN,                   // deep_lrn
    DEEP_CTXT,                  // deep_ctxt
  };

  DeepCopyVar   deep_var;        // which variable to copy from
  
  INIMPL virtual void Compute_ActFmSource(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // set current act of deep unit to sending super unit activation

  INLINE void   Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override { };
  INLINE void Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_ActFmSource(u, net, thr_no);
  }
  
  INLINE void Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_ActFmSource(u, net, thr_no);
  }

  INLINE void   Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void   Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    deep_var = DEEP_RAW;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_DeepCopyUnitSpec; }

