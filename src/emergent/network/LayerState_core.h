// this is included directly in LayerState_cpp and LayerState_cuda -- only things in addition to Layer_core
// mark all methods as IGNORE -- no direct access to functions on this or any State objects
// {

  ////////////////////////////////////////////////////////////////////////////////
  //    Below are the primary computational interface to the Network Objects
  //    for performing algorithm-specific activation and learning
  //    Many functions operate directly on the units via threads, and then
  //    call through to the layers for any layer-level subsequent processing

  INLINE void  Init_InputData(NETWORK_STATE* net)
  { ext_flag = NO_EXTERNAL; }
  // #IGNORE Initializes external and target inputs (layer level only)
  INLINE void  Init_Acts(NETWORK_STATE* net)
  { ext_flag = NO_EXTERNAL; }
  // #IGNORE Initialize the unit state variables (layer level only)
  INLINE void  Init_Weights_Layer(NETWORK_STATE* net) { };
  // #IGNORE #IGNORE layer-level initialization taking place after Init_Weights on units

  INLINE float Compute_SSE(NETWORK_STATE* net, int& n_vals, bool unit_avg = false, bool sqrt = false)
  {
    n_vals = 0;
    sse = 0.0f;
    if(!HasExtFlag(COMP_TARG)) return 0.0f;
    if(layer_type == HIDDEN) return 0.0f;
  
    const int li = layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& lay_sse = net->ThrLayStats(thr_no, li, 0, NETWORK_STATE::SSE);
      float& lay_n = net->ThrLayStats(thr_no, li, 1, NETWORK_STATE::SSE);

      sse += lay_sse;
      n_vals += (int)lay_n;
    }
  
    float rval = sse;
    if(unit_avg && n_vals > 0) {
      sse /= (float)n_vals;
    }
    if(sqrt) {
      sse = sqrtf(sse);
    }
    avg_sse.Increment(sse);
    if(sse > net->stats.cnt_err_tol)
      cur_cnt_err += 1.0;
    if(HasLayerFlag(NO_ADD_SSE) || (HasExtFlag(COMP) &&
                                    HasLayerFlag(NO_ADD_COMP_SSE))) {
      rval = 0.0f;
      n_vals = 0;
    }
    return rval;
  }
  // #IGNORE compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value -- uses sse_tol so error is 0 if within tolerance on a per unit basis
  
  INLINE int   Compute_PRerr(NETWORK_STATE* net) {
    int n_vals = 0;
    prerr.InitVals();
    if(!HasExtFlag(COMP_TARG)) return 0;
    if(layer_type == HIDDEN) return 0;

    const int li = layer_idx;
    for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
      // integrate over thread raw data
      float& true_pos = net->ThrLayStats(thr_no, li, 0, NETWORK_STATE::PRERR);
      float& false_pos = net->ThrLayStats(thr_no, li, 1, NETWORK_STATE::PRERR);
      float& false_neg = net->ThrLayStats(thr_no, li, 2, NETWORK_STATE::PRERR);
      float& true_neg = net->ThrLayStats(thr_no, li, 3, NETWORK_STATE::PRERR);
      float& lay_n = net->ThrLayStats(thr_no, li, 4, NETWORK_STATE::PRERR);

      n_vals += (int)lay_n;
      prerr.true_pos += true_pos;
      prerr.false_pos += false_pos;
      prerr.false_neg += false_neg;
      prerr.true_neg += true_neg;
    }
    prerr.ComputePR();
    if(HasLayerFlag(NO_ADD_SSE) || (HasExtFlag(COMP) &&
                                    HasLayerFlag(NO_ADD_COMP_SSE))) {
      n_vals = 0;
    }
    return n_vals;
  }    
  // #IGNORE compute precision and recall error statistics over entire layer -- true positive, false positive, and false negative -- returns number of values entering into computation (depends on number of targets) -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance on a per unit basis -- results are stored in prerr values on layer

  INLINE void  Compute_EpochSSE(NETWORK_STATE* net) {
    cnt_err = cur_cnt_err;
    if(avg_sse.n > 0) {
      pct_err = cnt_err / (float)avg_sse.n;
      pct_cor = 1.0f - pct_err;
    }
    avg_sse.GetAvg_Reset();

    cur_cnt_err = 0.0f;
  }
  // #IGNORE compute epoch-level sum squared error and related statistics
  
  INLINE void  Compute_EpochPRerr(NETWORK_STATE* net) {
    epc_prerr = sum_prerr;
    epc_prerr.ComputePR();        // make sure, in case of dmem summing
    sum_prerr.InitVals();         // reset!
  }
  // #IGNORE compute epoch-level precision and recall statistics
  
  INLINE void  Compute_EpochStats(NETWORK_STATE* net) {
    Compute_EpochSSE(net);
    if(net->stats.prerr)
      Compute_EpochPRerr(net);
  }
  // #IGNORE compute epoch-level statistics; calls DMem_ComputeAggs (if dmem) and EpochSSE -- specific algos may add more

  INLINE void Initialize_core() {
  }
  // #IGNORE
