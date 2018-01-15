// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  float         prec_levels;    // number of levels of precision available in the weight values

  INLINE float  PrecLimitVal(const float val) {
    int tmp = (int)((prec_levels * val) + .5f); // integerize with rounding -- val 0-1
    float rval = (float)tmp / prec_levels;
    if(rval > 1.0f) rval = 1.0f;
    if(rval < 0.0f) rval = 0.0f;
    return rval;
  }

  INLINE void C_Compute_LimPrecWts(float& wt)
  { wt = PrecLimitVal(wt); }
  // #IGNORE

  INLINE void Compute_LimPrecWts(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    float* wts = cg->OwnCnVar(WT);
    CON_STATE_LOOP(cg, C_Compute_LimPrecWts(wts[i]));
  }
  // #IGNORE

  INLINE void Compute_Weights(CON_STATE* scg, NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Weights(scg, net, thr_no);
    Compute_LimPrecWts(scg, (LEABRA_NETWORK_STATE*)net, thr_no);
  }

  // NOTE: bias weights typically not subject to limited precision!

  INLINE void Initialize_core() {
    prec_levels = 1024;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LeabraLimPrecConSpec; }

