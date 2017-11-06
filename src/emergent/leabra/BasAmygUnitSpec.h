// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  enum AcqExt { // acquisition vs. extinction type
    ACQ,        // acquisition neurons -- learn from LatAmyg and da_p
    EXT,        // extinction neurons -- learn from context / pfc input and ACQ up-state signal and da_p (using D2 receptors)
  };

  AcqExt        acq_ext;        // acquisition vs. extinction sub-type
  Valence       valence;        // US valence coding -- appetitive vs. aversive
  DAReceptor    dar;            // #READ_ONLY #SHOW type of dopamine receptor: D1 vs. D2 -- computed automatically from acq_ext and valence

  INIMPL void  Init_Weights(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override;

  INIMPL float Compute_DaModNetin(LEABRA_UNIT_STATE* uv, LEABRA_NETWORK_STATE* net,
                                  int thr_no, float& net_syn) override;

  INIMPL void  Compute_DeepMod(LEABRA_UNIT_STATE* uv, LEABRA_NETWORK_STATE* net,
                               int thr_no) override;

  INLINE void Initialize_core() {
    acq_ext = ACQ;    valence = APPETITIVE;    dar = D1R;
    deep.on = true;    deep.role = STATE_CLASS(DeepSpec)::DEEP;
    deep.raw_thr_rel = 0.1f;    deep.raw_thr_abs = 0.1f;
    deep.mod_thr = 0.01f;         // default is .1
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_BasAmygUnitSpec; }

