// this is included directly in parent ConSpec class -- has access to ConState and UnitState
// {
  enum ConVars {                // Connection variables -- must align with Connection obj
    WT,                         // the synaptic weight of connection
    DWT,                        // change in synaptic weight as computed by learning mechanism
    N_CON_VARS,                 // #IGNORE number of basic connection variables -- use this as the starting index for any new connection variables
  };

  STATE_CLASS(Random)  rnd;     // #CAT_ConSpec Weight randomization specification.  Note that NONE means no value at all, not the mean, and should be used if some other source is setting the weights, e.g., from a projectionspec or loading from a file etc
  STATE_CLASS(WeightLimits)  wt_limits;      // #CAT_ConSpec limits on weight sign, symmetry
  int           spec_idx;
  // #READ_ONLY #NO_SAVE #NO_INHERIT index of this spec in list of specs


  INLINE virtual int  GetStateSpecType() const { return NETWORK_STATE::T_ConSpec; }
  // #CAT_State derived classes MUST override this and pass correct global type id

  INLINE float&         C_ApplyLimits(float& wt)
  { wt_limits.ApplyLimits(wt); return wt; }
  // #IGNORE #CAT_Learning apply weight limits to single connection
  INLINE virtual void   ApplyLimits(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    float* wts = cg->OwnCnVar(WT);
    if(wt_limits.type != STATE_CLASS(WeightLimits)::NONE) {
      CON_STATE_LOOP(cg, C_ApplyLimits(wts[i]));
    }
  }
  // #IGNORE #CAT_Learning apply weight limits (sign, magnitude) -- automatically enforced during Init_Weights -- this is if needed outside of that

  // NOTE: full Init_Weight support requires ProjectionSpec -- must be done on host main C++ code
  // so we don't worry about supporting symmetry here -- also requires layer, prjn, unit !
  
  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning

  INLINE virtual void  LoadWeightVal(float wtval, CON_STATE* cg, int cidx, NETWORK_STATE* net)
  { cg->Cn(cidx,WT,net) = wtval; }
  // #CAT_State set loaded weight value for given connection index
  
  INLINE void     C_Init_Weight_Rnd(float& wt, int thr_no) {
    wt = rnd.Gen(thr_no);
    C_ApplyLimits(wt);
  }
  // #CAT_Learning initialize the weight according to the random number specs, and apply weight limits (not symmetry)
  INLINE void   C_Init_Weight_AddRndVar(float& wt, int thr_no) {
    wt += rnd.Gen(thr_no) - rnd.mean; // subtract mean
  }
  // #CAT_Learning initialize the weight by adding number according to the random number specs, and apply weight limits (not symmetry)
  INLINE void   C_Init_dWt(float& dwt)
  { dwt = 0.0f; }
  // #CAT_Learning initialize weight-change variable to 0

  INLINE virtual void   SetConScale(float scale, CON_STATE* cg, int cidx, NETWORK_STATE* net, int thr_no) { }
  // #CAT_Learning set the connection scale parameter, for given connection index, for algorithms that support connection scale parameters (Leabra)
  
  INLINE void           Init_Weights_symflag(NETWORK_STATE* net, int thr_no)
  { if(wt_limits.sym) net->needs_wt_sym = true; }
  // #IGNORE must be called during Init_Weights to update net flag for weight symmetrizing

  INLINE virtual void   Init_Weights(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    Init_Weights_symflag(net, thr_no);

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);

    int eff_thr_no = net->HasNetFlag(NETWORK_STATE::INIT_WTS_1_THREAD) ? 0 : thr_no;
  
    const int sz = cg->size;
    if(rnd.type != STATE_CLASS(Random)::NONE) {
      for(int i=0; i<sz; i++) {
        C_Init_Weight_Rnd(wts[i], eff_thr_no);
        C_Init_dWt(dwts[i]);
      }
    }
  }
  // #CAT_Learning initialize connection weights based on random parameter settings (ie. at beginning of training)
  
  // NOTE: it is ESSENTIAL that Init_Weights ONLY does wt, dwt, and scale -- all other vars
  // MUST be initialized in _post -- projections with topo weights ONLY do these specific
  // variables but no others..

  INLINE virtual void ApplySymmetry_r(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    if(!wt_limits.sym) return;
    UNIT_STATE* ru = cg->ThrOwnUnState(net, thr_no);
    const int sz = cg->size;
    for(int i=0; i<sz;i++) {
      int con_idx = -1;
      CON_STATE* rrcg = net->FindRecipRecvCon(con_idx, cg->UnState(i,net), ru);
      if(rrcg && con_idx >= 0) {
        CON_SPEC_CPP* rrcs = rrcg->GetConSpec(net);
        if(rrcs && rrcs->wt_limits.sym) {
          if(wt_limits.sym_fm_top) {
            rrcg->OwnCn(con_idx, WT) = cg->OwnCn(i, WT); // todo: not sure this order is right
          }
          else {
            cg->OwnCn(i, WT) = rrcg->OwnCn(con_idx, WT); // theoretically should be opp of s
          }
        }
      }
    }
  }
  
  // #CAT_Learning apply weight symmetrizing between reciprocal units -- recv owns cons version
  INLINE virtual void ApplySymmetry_s(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    if(!wt_limits.sym) return;
    UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    const int sz = cg->size;
    for(int i=0; i<sz;i++) {
      int con_idx = -1;
      CON_STATE* rscg = net->FindRecipSendCon(con_idx, cg->UnState(i,net), su);
      if(rscg && con_idx >= 0) {
        CON_SPEC_CPP* rscs = rscg->GetConSpec(net);
        if(rscs && rscs->wt_limits.sym) {
          if(wt_limits.sym_fm_top) {
            cg->OwnCn(i, WT) = rscg->OwnCn(con_idx, WT);
          }
          else {
            rscg->OwnCn(con_idx, WT) = cg->OwnCn(i, WT);
          }
        }
      }
    }
  }
  // #CAT_Learning apply weight symmetrizing between reciprocal units -- send owns cons version

  INLINE virtual void Init_Weights_sym_r(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    LAYER_STATE* rlay = cg->GetRecvLayer(net);
    LAYER_STATE* slay = cg->GetSendLayer(net);
    if(rlay->units_flat_idx < slay->units_flat_idx)
      return;    // higher copies from lower, so if we're lower, bail..
    ApplySymmetry_r(cg, net, thr_no);
  }
  // #CAT_State apply symmetry after weight init, recv based

  INLINE virtual void Init_Weights_sym_s(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    LAYER_STATE* rlay = cg->GetRecvLayer(net);
    LAYER_STATE* slay = cg->GetSendLayer(net);
    if(rlay->units_flat_idx < slay->units_flat_idx)
      return;    // higher copies from lower, so if we're lower, bail..
    ApplySymmetry_s(cg, net, thr_no);
  }
  // #CAT_State apply symmetry after weight init, sender based

  INLINE virtual void   Init_Weights_scale(CON_STATE* cg, NETWORK_STATE* net, int thr_no,
                                           float init_wt_val = 1.0f) { };
  // #CAT_Learning only for Leabra: initialize connection weights by setting scale multiplier values to random values, and setting adaptive weights to given constant initial value (weights end up as product of scale * weight)

  // INLINE virtual void   Init_Weights_sym_r(CON_STATE* cg, NETWORK_STATE* net, int thr_no);
  // // #CAT_State apply symmetry after weight init, recv based
  // INLINE virtual void   Init_Weights_sym_s(CON_STATE* cg, NETWORK_STATE* net, int thr_no);
  // // #CAT_State apply symmetry after weight init, sender based

  INLINE virtual void   Init_Weights_post(CON_STATE* cg, NETWORK_STATE* net, int thr_no)
  { };
  // // #CAT_State post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  INLINE virtual void   Init_dWt(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    float* dwts = cg->OwnCnVar(DWT);
    CON_STATE_LOOP(cg, C_Init_dWt(dwts[i]));
  }
  // #CAT_Learning initialize weight-change variables for all cons

  INLINE virtual void   RenormWeights(CON_STATE* cg, NETWORK_STATE* net, int thr_no,
                                      bool mult_norm, float avg_wt) {
    const int sz = cg->size;
    if(sz < 2) return;
    float avg = 0.0f;
    for(int i=0; i<sz; i++) {
      avg += cg->Cn(i, WT, net);
    }
    avg /= (float)sz;
    if(mult_norm) {
      float adj = avg_wt / avg;
      for(int i=0; i<sz; i++) {
        cg->Cn(i, WT, net) *= adj;
      }
    }
    else {
      float adj = avg_wt - avg;
      for(int i=0; i<sz; i++) {
        cg->Cn(i, WT, net) += adj;
      }
    }
  }
  // #CAT_State renormalize the weight values using either multiplicative (for positive-only weight values such as Leabra) or subtractive normalization (for pos/neg weight values, such as backprop) to hit the given average weight value -- only affects wt value -- need to call Init_Weights_post afterward at appropriate level! -- receiver based but uses generic, slow interace so can be called either way

  INLINE virtual void   RenormScales(CON_STATE* cg, NETWORK_STATE* net, int thr_no,
                                     bool mult_norm, float avg_wt)  { };
  // #CAT_State renormalize the connection scale values using either multiplicative (for positive-only weight values such as Leabra) or subtractive normalization (for pos/neg weight values, such as backprop) to hit the given average weight value -- only affects scale value -- only for algorithms that support scale (Leabra) -- need to call Init_Weights_post afterward at appropriate level! -- receiver based but uses generic, slow interace so can be called either way

  INLINE float          C_Compute_Netin(const float wt, const float su_act)
  { return wt * su_act; }
  // #IGNORE 
  INLINE virtual float  Compute_Netin(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    float rval=0.0f;
    float* wts = cg->OwnCnVar(WT);
    CON_STATE_LOOP(cg, rval += C_Compute_Netin(wts[i], cg->UnState(i,net)->act));
    // todo: if unit act is all in a contiguous vector, and with vec chunking, this 
    // could be a very fast vector op
    return rval;
  }
  // #CAT_Activation compute net input for weights in this con state

  INLINE void           C_Send_Netin(const float wt, float* send_netin_vec,
                                     const int ru_idx, const float su_act)
  { send_netin_vec[ru_idx] += wt * su_act; }
  // #IGNORE 
  INLINE virtual void   Send_Netin(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    const float su_act = su->act;
    float* send_netin_vec = net->ThrSendNetinTmp(thr_no);
    float* wts = cg->OwnCnVar(WT);
    CON_STATE_LOOP(cg, C_Send_Netin(wts[i], send_netin_vec, cg->UnIdx(i), su_act));
  }
  // #CAT_Activation sender-based net input for con state (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units -- only for sender-owned connections!
  INLINE virtual void   Send_Netin_PerPrjn(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    const float su_act = su->act;
    float* send_netin_vec = net->ThrSendNetinTmpPerPrjn(thr_no, cg->other_idx);
    float* wts = cg->OwnCnVar(WT);
    CON_STATE_LOOP(cg, C_Send_Netin(wts[i], send_netin_vec, cg->UnIdx(i), su_act));
  }
  // #CAT_Activation sender-based net input, keeping projections separate, for con state (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_SentNetin function on units -- only for sender-owned connections!

  INLINE float          C_Compute_Dist(const float wt, const float su_act)
  { const float tmp = su_act - wt; return tmp * tmp; }
  // #IGNORE
  
  INLINE virtual float  Compute_Dist(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    float rval=0.0f;
    float* wts = cg->OwnCnVar(WT);
    CON_STATE_LOOP(cg, rval += C_Compute_Dist(wts[i], cg->UnState(i,net)->act));
    // todo: if unit act is all in a contiguous vector, and with vec chunking, this 
    // could be a very fast vector op
    return rval;
  }
  // #CAT_Activation compute net distance for con state (ie. euclidean distance)
  
  INLINE void           C_Compute_dWt(float& wt, float& dwt, const float ru_act,
                                      const float su_act)
  { dwt += ru_act * su_act; }
  // #IGNORE define in subclass to take proper args -- this is just for demo -- best to take all the vals as direct floats
  INLINE virtual void   Compute_dWt(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    UNIT_STATE* ru = cg->ThrOwnUnState(net, thr_no);
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    const float ru_act = ru->act; // assume recv based
    CON_STATE_LOOP(cg, C_Compute_dWt(wts[i], dwts[i], ru_act,
                                     cg->UnState(i,net)->act));
  }
  
  // #CAT_Learning compute the delta-weight change -- recv owns cons version

  INLINE void           C_Compute_Weights(float& wt, float& dwt)
  { wt += dwt; dwt = 0.0f; }
  // #IGNORE define in subclass to take proper args -- this is just for demo -- best to take all the vals as direct floats
  INLINE virtual void   Compute_Weights(CON_STATE* cg, NETWORK_STATE* net, int thr_no) {
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    CON_STATE_LOOP(cg, C_Compute_Weights(wts[i], dwts[i]));
    ApplyLimits(cg,net,thr_no); 
    // ApplySymmetry_r(cg,ru);  don't apply symmetry during learning..
  }
  // #CAT_Learning update weights (ie. add delta-wt to wt, zero delta-wt)

  ////////////////////////////////////////////////////////////////////////////////
  //    Bias-Weight versions, operate on UnitState

  INLINE virtual void   B_Init_Weights(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) {
    int eff_thr_no = net->HasNetFlag(NETWORK_STATE::INIT_WTS_1_THREAD) ? 0 : thr_no;
    C_Init_Weight_Rnd(uv->bias_wt, eff_thr_no);
    B_Init_dWt(uv, net, thr_no);  // user not expecting to have to init this -- call virtual
  }
  // #CAT_Learning bias weight: initialize weight state variables (ie. at beginning of training)
  
  INLINE virtual void   B_Init_dWt(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) {
    C_Init_dWt(uv->bias_dwt);
  }
  // #CAT_Learning bias weight: initialize weight-change variables for all cons
  INLINE virtual void   B_Init_Weights_post(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no)
  { };
  // #CAT_Learning bias weight: post-weight init
  INLINE virtual void   B_Compute_dWt(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) {
    // todo: learning rule here..
  }
  // #CAT_Learning bias weight: compute the delta-weight change -- recv owns cons version
  INLINE virtual void   B_Compute_Weights(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) {
    C_Compute_Weights(uv->bias_wt, uv->bias_dwt);
  }
  // #CAT_Learning bias weight: update weights (ie. add delta-wt to wt, zero delta-wt)


  ///////////////////////////////////////////////////////////////////
  //    Infrastructure -- need to be able to save / load weights
  //    without relying on TypeAccess, so we do it manually..

  INLINE virtual bool   SaveVar(CON_STATE* cg, NETWORK_STATE* net, int var_no) const
  { return (var_no == WT); }
  // #CAT_File should given variable be saved?

  INLINE virtual const char* ConVarName(int var_no) const {
    switch(var_no) {
    case WT: return "wt";
    case DWT: return "dwt";
    }
    return "";
  }
  // #CAT_Access name of given connection variable

  INLINE int FindConVar(const CON_STATE* cg, const char* var_nm) {
    for(int i=0; i < cg->n_con_vars; i++) {
      if(strcmp(ConVarName(i), var_nm) == 0) return i;
    }
    return -1;
  }
  // #CAT_Access find index of given connection variable, -1 if not found

  INLINE void Initialize_core() {
    spec_idx = -1;
    rnd.type = STATE_CLASS(Random)::UNIFORM;
    rnd.mean = 0.0f;
    rnd.var = .5f;
  }
  // #IGNORE
