// this contains core shared code, and is included directly in BpConSpec.h, _cpp.h, _cuda.h
//{
  enum BpConVars {
    PDW = N_CON_VARS,           // previous delta weight
    N_BP_CON_VARS,              // #IGNORE number of bp con vars
  };

  enum DecayType {              // type of weight decay -- previously set by a function, but this prevents optimization and use in CUDA, etc
    NO_DECAY,                   // do not perform any form of weight decay
    SIMPLE_DECAY,               // apply simple weight decay -- subtract a constant factor of the weight value with every update to the weights
    ELIMINATION,                // weight elimination weight decay -- subtract: (decay * w) / (1 + wt^2)^2 -- this eliminates small weights but weights that are larger (around 1 or higher) are subject to a much smaller amount of decay -- based on a bimodal underlying distribution -- developed by Weigend et al (1990) -- note that prior to 8.0, this included a 2.0 factor and incorrectly used wt in the numerator which was kind of a deadly mistake!
  };

  enum  WtUpdtType {             // overall type of weight update to perform, based on parameters that are set -- for optimizing the code to use the most appropriate function
    WU_DWT_ONLY,                 // just straight dwt, no decay, no momentum
    WU_SIMPLE_DECAY,             // simple decay
    WU_ELIMINATION,              // weight elimination
    WU_MOMENT,                   // momentum only
    WU_MOMENT_SIMPLE,            // momentum and simple decay
    WU_MOMENT_ELIM,              // momentum and weight elimination
  };
  
  float         lrate;          // learning rate
  float         cur_lrate;      // #READ_ONLY #NO_INHERIT #SHOW current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  float         momentum;       // momentum factor -- as of 8.0 this is standardized to NOT include an additional learning rate factor (previous AFTER_LRATE option -- momentum is effectively after the learning rate)
  DecayType     decay_type;     // type of weight decay to apply (before 8.0 this was set by selecting a function, but this prevents optimization)
  float         decay;          // #CONDSHOW_OFF_decay_type:NO_DECAY decay rate -- the learning rate is also applied to the decay -- i.e., decay comes before the learning rate factor
  WtUpdtType    wt_updt;        // #READ_ONLY type of weight update to perform -- computed from other parameters set -- used to optimize computation
  bool          no_bp;          // #DEF_false if true, do not backpropagate error through this connection -- it still learns, but does not send error back to layer it receives from -- this is useful for decoder connections that attempt to decode from a layer without influencing its learning

  INLINE int  GetStateSpecType() const override { return BP_NETWORK_STATE::T_BpConSpec; }

  INLINE void   Init_dWt(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override {
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    for(int i=0; i<cg->size; i++) {
      C_Init_dWt(dwts[i]);
      pdws[i] = 0.0f;
    }
  }

  INLINE void   Init_Weights_post(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override {
    float* pdws = cg->OwnCnVar(PDW);
    for(int i=0; i<cg->size; i++) {
      pdws[i] = 0.0f;
    }
  }
  // all non-wt, pdw vars MUST be initialized in _post!
  
  // vectorization notes: Compute_Weights is fully vectorized and easy, as everything
  // is in the connections
  // other functions require unit access -- the sending units are vector chunked,
  // but due to the thread organization, their ram is discontinuous and cannot
  // be sequentially accessed.  The gather<> load function is potentially usable, but
  // it takes a common offset and has different indexes -- not workable with the current
  // setup -- so the only way to make it work would be to copy all the acts into a
  // single common flat array -- probably that takes more time than we save.. but..
  // could give it a try.. would only work for compute_dwt -- overall unlikely to be
  // worth it though, so postponing in favor of CUDA etc at this point
  
  INLINE float          C_Compute_dEdA(const float wt, const float ru_dEdNet)
  { return wt * ru_dEdNet; }
  // #IGNORE 
  INLINE virtual float  Compute_dEdA(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    // this is ptr-con based and thus very slow..
    float rval = 0.0f;
    if (!no_bp) {
      CON_STATE_LOOP(cg, rval += C_Compute_dEdA(cg->PtrCn(i,WT,net),
                                                ((BP_UNIT_STATE*)cg->UnState(i,net))->dEdNet));
    }
    return rval;
  }
  // get error from units I send to

  INLINE void           C_Compute_dWt(float& dwt, const float ru_dEdNet,
                                     const float su_act)
  { dwt += su_act * ru_dEdNet; }
  // #IGNORE
  INLINE void           Compute_dWt(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override {
    BP_UNIT_STATE* ru = (BP_UNIT_STATE*)cg->ThrOwnUnState(net, thr_no);
    const float ru_dEdNet = ru->dEdNet;
    float* dwts = cg->OwnCnVar(DWT);
    CON_STATE_LOOP(cg, C_Compute_dWt(dwts[i], ru_dEdNet, cg->UnState(i,net)->act));
    // todo: if unit act is all in a contiguous vector, and with vec chunking, this 
    // could be a very fast vector op
  }
  // Compute dE with respect to the weights

  // the different forms of weight update -- also implemented in vectorized
  INLINE void C_Compute_Weights_dWtOnly(float& wt, float& dwt) {
    wt += cur_lrate * dwt;
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  INLINE void Compute_Weights_dWtOnly_vec(CON_STATE* cg, float* wts, float* dwts) {
    VECF zeros(0.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      wt += cur_lrate * dwt;
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_dWtOnly(wts[i], dwts[i]);
    }
  }
#endif
  // no momentum, no decay
  INLINE void C_Compute_Weights_SimpleDecay(float& wt, float& dwt) {
    wt += cur_lrate * (dwt - decay * wt);
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  INLINE void Compute_Weights_SimpleDecay_vec(CON_STATE* cg, float* wts, float* dwts) {
    VECF zeros(0.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      wt += cur_lrate * (dwt - decay * wt);
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_SimpleDecay(wts[i], dwts[i]);
    }
  }
#endif
  INLINE void C_Compute_Weights_Elimination(float& wt, float& dwt) {
    float denom = (1.0f + wt * wt);
    wt += cur_lrate * (dwt - ((decay * wt) / (denom * denom)));
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  INLINE void Compute_Weights_Elimination_vec(CON_STATE* cg, float* wts, float* dwts) {
    VECF zeros(0.0f);
    VECF ones(1.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      VECF denom = ones + wt * wt;
      wt += cur_lrate * (dwt - ((decay * wt) / (denom * denom)));
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_Elimination(wts[i], dwts[i]);
    }
  }
#endif
  INLINE void C_Compute_Weights_Moment(float& wt, float& dwt, float& pdw) {
    pdw = cur_lrate * dwt + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  INLINE void Compute_Weights_Moment_vec(CON_STATE* cg, float* wts, float* dwts, float* pdws) {
    VECF zeros(0.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      VECF pdw; pdw.load(pdws+i);
      pdw = cur_lrate * dwt + momentum * pdw;
      wt += pdw;
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
      pdw.store(pdws+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_Moment(wts[i], dwts[i], pdws[i]);
    }
  }
#endif
  INLINE void C_Compute_Weights_Moment_Simple(float& wt, float& dwt, float& pdw) {
    pdw = cur_lrate * (dwt - decay * wt) + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  INLINE void Compute_Weights_Moment_Simple_vec(CON_STATE* cg, float* wts, float* dwts, float* pdws) {
    VECF zeros(0.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      VECF pdw; pdw.load(pdws+i);
      pdw = cur_lrate * (dwt - decay * wt) + momentum * pdw;
      wt += pdw;
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
      pdw.store(pdws+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_Moment_Simple(wts[i], dwts[i], pdws[i]);
    }
  }
#endif
  INLINE void C_Compute_Weights_Moment_Elim(float& wt, float& dwt, float& pdw) {
    float denom = (1.0f + wt * wt);
    pdw = cur_lrate * (dwt - ((decay * wt) / (denom * denom))) + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  INLINE void Compute_Weights_Moment_Elim_vec(CON_STATE* cg, float* wts, float* dwts, float* pdws) {
    VECF zeros(0.0f);
    VECF ones(1.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      VECF pdw; pdw.load(pdws+i);
      VECF denom = ones + wt * wt;
      pdw = cur_lrate * (dwt - ((decay * wt) / (denom * denom))) + momentum * pdw;
      wt += pdw;
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
      pdw.store(pdws+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_Moment_Elim(wts[i], dwts[i], pdws[i]);
    }
  }
#endif

  INLINE void   Compute_Weights(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override {
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);

#ifdef TA_VEC_USE  
    switch(wt_updt) {
    case WU_DWT_ONLY:
      Compute_Weights_dWtOnly_vec(cg, wts, dwts);
      break;
    case WU_SIMPLE_DECAY:
      Compute_Weights_SimpleDecay_vec(cg, wts, dwts);
      break;
    case WU_ELIMINATION:
      Compute_Weights_Elimination_vec(cg, wts, dwts);
      break;
    case WU_MOMENT:
      Compute_Weights_Moment_vec(cg, wts, dwts, pdws);
      break;
    case WU_MOMENT_SIMPLE:
      Compute_Weights_Moment_Simple_vec(cg, wts, dwts, pdws);
      break;
    case WU_MOMENT_ELIM:
      Compute_Weights_Moment_Elim_vec(cg, wts, dwts, pdws);
      break;
    }
#else
    switch(wt_updt) {
    case WU_DWT_ONLY: {
      CON_STATE_LOOP(cg, C_Compute_Weights_dWtOnly(wts[i], dwts[i]));
      break;
    }
    case WU_SIMPLE_DECAY: {
      CON_STATE_LOOP(cg, C_Compute_Weights_SimpleDecay(wts[i], dwts[i]));
      break;
    }
    case WU_ELIMINATION: {
      CON_STATE_LOOP(cg, C_Compute_Weights_Elimination(wts[i], dwts[i]));
      break;
    }
    case WU_MOMENT: {
      CON_STATE_LOOP(cg, C_Compute_Weights_Moment(wts[i], dwts[i], pdws[i]));
      break;
    }
    case WU_MOMENT_SIMPLE: {
      CON_STATE_LOOP(cg, C_Compute_Weights_Moment_Simple(wts[i], dwts[i], pdws[i]));
      break;
    }
    case WU_MOMENT_ELIM: {
      CON_STATE_LOOP(cg, C_Compute_Weights_Moment_Elim(wts[i], dwts[i], pdws[i]));
      break;
    }
    }
#endif

    ApplyLimits(cg, net, thr_no);
  }

  INLINE void B_Init_dWt(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override {
    C_Init_dWt(uv->bias_dwt);
    ((BP_UNIT_STATE*)uv)->bias_pdw = 0.0f;
  }

  INLINE void B_Compute_dWt(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override {
    uv->bias_dwt += ((BP_UNIT_STATE*)uv)->dEdNet;
  }
    
  INLINE void B_Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    BP_UNIT_STATE* uv = (BP_UNIT_STATE*)u;
    switch(wt_updt) {
    case WU_DWT_ONLY:
      C_Compute_Weights_dWtOnly(uv->bias_wt, uv->bias_dwt);
      break;
    case WU_SIMPLE_DECAY:
      C_Compute_Weights_SimpleDecay(uv->bias_wt, uv->bias_dwt);
      break;
    case WU_ELIMINATION:
      C_Compute_Weights_Elimination(uv->bias_wt, uv->bias_dwt);
      break;
    case WU_MOMENT:
      C_Compute_Weights_Moment(uv->bias_wt, uv->bias_dwt, uv->bias_pdw);
      break;
    case WU_MOMENT_SIMPLE:
      C_Compute_Weights_Moment_Simple(uv->bias_wt, uv->bias_dwt, uv->bias_pdw);
      break;
    case WU_MOMENT_ELIM:
      C_Compute_Weights_Moment_Elim(uv->bias_wt, uv->bias_dwt, uv->bias_pdw);
      break;
    }
    C_ApplyLimits(uv->bias_wt);
  }
    
  INLINE void Initialize_core() {
    lrate = .2f;
    cur_lrate = .2f;
    momentum = 0.0f;
    decay_type = NO_DECAY;
    decay = 0.0f;
    no_bp = false;
  }
