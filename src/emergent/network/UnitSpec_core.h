// this contains core shared code, and is included directly in UnitSpec.h, _cpp.h, _cuda.h
//{
  STATE_CLASS(MinMaxRange)   act_range;
  // #CAT_Activation range of activation for units
  float         sse_tol;
  // #CAT_Statistic tolerance for computing sum-squared error and other error metrics on a per-unit basis
  int           spec_idx;
  // #READ_ONLY #NO_SAVE #NO_INHERIT index of this spec in list of specs
  int           bias_spec_idx;
  // #READ_ONLY #NO_SAVE #NO_INHERIT index of bias con spec in list of specs


  INLINE virtual int  GetStateSpecType() const { return NETWORK_STATE::T_UnitSpec; }
  // #CAT_State derived classes MUST override this and pass correct global type id

  INLINE CON_SPEC* GetBiasSpec(NETWORK_STATE* net) {
#ifdef STATE_MAIN
    return bias_spec;
#else    
    if(bias_spec_idx < 0) return NULL;
    return net->GetConSpec(bias_spec_idx);
#endif    
  }
  // #CAT_Structure get the bias con spec 

  INLINE virtual void  Init_UnitState(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    u->ext_flag = UNIT_STATE::NO_EXTERNAL;
    u->ext = 0.0f;
    u->targ = 0.0f;
    u->net = 0.0f;
    u->act = 0.0f;
    u->bias_wt = 0.0f;
    u->bias_dwt = 0.0f;
  }
  // #CAT_Activation initialize all unit state -- do NOT access anything else, e.g., at congroup level

  INLINE virtual void  Init_InputData(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    u->ClearExtFlag(UNIT_STATE::COMP_TARG_EXT);
    u->ext = 0.0f;
    u->targ = 0.0f;
  }
  // #CAT_Activation initialize unit activation state variables for input data

  INLINE virtual void  Init_Acts(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    u->ClearExtFlag(UNIT_STATE::COMP_TARG_EXT);
    u->ext = 0.0f;
    u->targ = 0.0f;
    u->net = 0.0f;
    u->act = 0.0f;
  }
  // #CAT_Activation initialize unit activation state variables

  INLINE virtual void  Init_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    CON_SPEC* bs = GetBiasSpec(net);
    if(bs) {
      bs->B_Init_dWt(u, net, thr_no);
    }
  }
  // #CAT_Learning initialize the weight change variables

  INLINE virtual void  Init_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    CON_SPEC* bs = GetBiasSpec(net);
    if(bs) {
      bs->B_Init_Weights(u, net, thr_no);
    }
  }
  // #CAT_Learning init weight state variables
  INLINE virtual void  Init_Weights_post(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    CON_SPEC* bs = GetBiasSpec(net);
    if(bs) {
      bs->B_Init_Weights_post(u, net, thr_no);
    }
  }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc) -- threaded for speed

  INLINE virtual void  LoadBiasWtVal(float bwt, UNIT_STATE* u, NETWORK_STATE* net) {
    u->bias_wt = bwt;
  }
  // #CAT_Structure load bias weight value into bias weights of given unit -- handles any post-loading updates directly


  INLINE virtual void ApplyInputData(UNIT_STATE* u, NETWORK_STATE* net, float val,
                                     UnitState_cpp::ExtFlags act_ext_flags, bool na_by_range) {
    // note: not all flag values are valid, so following is a fuzzy cascade
    if(na_by_range) {
      if(!act_range.RangeTestEq(val)) {
        return;
      }
    }
    if (act_ext_flags & UnitState_cpp::EXT) {
      u->ext = val;
      u->SetExtFlag(UnitState_cpp::EXT);
    }
    else {
      u->targ = val;
      if (act_ext_flags & UnitState_cpp::TARG)
        u->SetExtFlag(UnitState_cpp::TARG);
      else if (act_ext_flags & UnitState_cpp::COMP)
        u->SetExtFlag(UnitState_cpp::COMP);
    }
  }
  // #CAT_Activation apply input data value according to ext flags

  INLINE virtual void  Compute_Netin(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    float new_net = 0.0f;
    const int nrcg = u->NRecvConGps(net);
    for(int g=0; g<nrcg; g++) {
      CON_STATE* rgp = u->RecvConState(net, g);
      if(rgp->NotActive()) continue;
      new_net += rgp->GetConSpec(net)->Compute_Netin(rgp, net, thr_no);
    }
    CON_SPEC* bs = GetBiasSpec(net);
    if(bs) {
      new_net += u->bias_wt;
    }
    u->net = new_net;
  }
  // #CAT_Activation compute net input: activations times weights (receiver based)

  INLINE virtual void  Compute_SentNetin(UNIT_STATE* u, NETWORK_STATE* net, float sent_netin) {
    // called by network-level Send_Netin function to integrate sent netin value
    // with current net input value -- default is just to set to net val + bias wt if avail
    u->net = sent_netin;
    CON_SPEC* bs = GetBiasSpec(net);
    if(bs) {
      u->net += u->bias_wt;
    }
  }
  // #CAT_Activation called by network-level Send_Netin function to integrate sent netin value with current net input value -- default is just to set to net val + bias wt if avail

  INLINE virtual void  Compute_Act(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    if(u->HasExtFlag(UNIT_STATE::EXT))
      u->act = u->ext;
    else
      u->act = u->net;
  }
  // #CAT_Activation compute the activation value of the unit: what it sends to other units

  INLINE virtual void  Compute_NetinAct(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    Compute_Netin(u, net, thr_no);
    Compute_Act(u, net, thr_no);
  }
  // #CAT_Activation compute the netinput (receiver-based) and then activation value of the unit: what it sends to other units

  INLINE virtual void  Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    CON_SPEC* bs = GetBiasSpec(net);
    if(bs) {
      bs->B_Compute_dWt(u, net, thr_no);
    }
  }
  // #CAT_Learning compute change in weights: the mechanism of learning

  INLINE virtual void  Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) {
    CON_SPEC* bs = GetBiasSpec(net);
    if(bs) {
      bs->B_Compute_Weights(u, net, thr_no);
    }
  }
  // #CAT_Learning update weights from deltas

  INLINE virtual float Compute_SSE(UNIT_STATE* u, NETWORK_STATE* net, int thr_no, bool& has_targ) {
    float sse = 0.0f;
    has_targ = false;
    if(u->HasExtFlag(UNIT_STATE::COMP_TARG)) {
      has_targ = true;
      float uerr = u->targ - u->act;
      if(fabsf(uerr) >= sse_tol)
        sse = uerr * uerr;
    }
    return sse;
  }    
  // #CAT_Statistic compute sum squared error for this unit -- uses sse_tol so error is 0 if within tolerance -- has_targ indicates if there is actually a target value (else the return value is 0)

  INLINE virtual bool  Compute_PRerr
    (UNIT_STATE* u, NETWORK_STATE* net, int thr_no,
     float& true_pos, float& false_pos, float& false_neg, float& true_neg) {
    true_pos = 0.0f; false_pos = 0.0f; false_neg = 0.0f; true_neg = 0.0f;
    bool has_targ = false;
    if(u->HasExtFlag(UNIT_STATE::COMP_TARG)) {
      has_targ = true;
      if(u->targ > u->act) {
        true_pos = u->act;
        true_neg = 1.0 - u->targ;
        false_neg = u->targ - u->act;
      }
      else {
        true_pos = u->targ;
        false_pos = u->act - u->targ;
        true_neg = 1.0 - u->act;
      }
    }
    return has_targ;
  }
  // #CAT_Statistic compute precision and recall error statistics for this unit -- true positive, false positive, false negative and true negative -- returns true if unit actually has a target value specified (otherwise everything is 0) -- precision = tp / (tp + fp), recall = tp / (tp + fn), fmeasure = 2 * p * r / (p + r), mcc = ((tp*tn) - (fp*fn)) / sqrt((tp+fp)*(tp+fn)*(tn+fp)*(tn+fn))

  INLINE void Initialize_core() {
    act_range.max = 1.0f; act_range.min = 0.0f;
    act_range.range = 1.0f; act_range.scale = 1.0f;
    sse_tol = 0.0f;
    spec_idx = -1;
    bias_spec_idx = -1;
  }
  
