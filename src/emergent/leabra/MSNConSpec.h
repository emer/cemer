// this is included directly in LeabraExtraConSpecs_cpp / _cuda
//{

  enum MSNConVars {
    NTR = N_LEABRA_CON_VARS,    // new trace -- drives updates to trace value -- thal * ru * su
    TR,                         // current ongoing trace of activations using ru * su, which drive learning -- adds ntr and clears after learning on current values -- includes both thal gated (+ and other nongated, - inputs)
    N_MSN_CON_VARS,
  };

  enum LearnActVal {            // activation value to use for learning
    PREV_TRIAL,                 // previous trial
    ACT_P,                      // act_p from current trial
    ACT_M,                      // act_m from current trial
    ACT_EQ,                     // act_eq from current trial -- use this for PBWM Dorsal matrix
    ACT_G,                      // act_g from current trial -- activation on cycle of gating -- use this for PBWM Dorsal matrix
  };
  
  enum LearningRule {           // type of learning rule to use
    DA_HEBB,                    // immediate use of dopamine * send * recv activation triplet to drive learning
    DA_HEBB_VS,                 // ventral striatum version of DA_HEBB, which uses MAX(deep_lrn, ru_act) for recv term in dopamine * send * recv activation triplet to drive learning
    TRACE_THAL,                 // send * recv activation establishes a trace (long-lasting synaptic tag), with thalamic activation determining sign of the trace (if thal active (gated) then sign is positive, else sign is negative) -- when dopamine later arrives, the trace is applied * dopamine, and the amount of dopamine and/or any above-threshold ach from TAN units resets the trace
    TRACE_NO_THAL,              // send * recv activation establishes a trace (long-lasting synaptic tag), with no influence of thalamic gating signal -- when dopamine later arrives, the trace is applied * dopamine, and any above-threshold ach from TAN units resets the trace
    TRACE_NO_THAL_VS,           // ventral striatum version of TRACE_NO_THAL, which uses MAX(MIN(deep_mod_net, max_vs_deep_mod), ru_act) for recv term to set trace
    WM_DEPENDENT,               // learning depends on a working memory trace.. 
  };
    
  LearnActVal        su_act_var;     // what variable to use for sending unit activation
  LearnActVal        ru_act_var;     // what variable to use for recv unit activation
  LearningRule       learn_rule;     // what kind of learning rule to use
  STATE_CLASS(MSNTraceSpec)       trace;          // #AKA_matrix #CONDSHOW_ON_learn_rule:TRACE_THAL,TRACE_NO_THAL,TRACE_NO_THAL_VS parameters for trace-based learning 
  STATE_CLASS(MSNTraceThalLrates) tr_thal;        // #CONDSHOW_ON_learn_rule:TRACE_THAL gain parameters for trace-based thalamic-gated learning 
  float              burst_da_gain;  // #MIN_0 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float              dip_da_gain;    // #MIN_0 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!


  INLINE float  GetDa(float da, bool d2r) {
    if(da < 0.0f) da *= dip_da_gain; else da *= burst_da_gain;
    if(d2r) da = -da;
    return da;
  }
  // get effective dopamine signal taking into account gains and reversal by D2R
  
  INLINE void Init_Weights(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* scales = cg->OwnCnVar(SCALE);
    
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);

    int eff_thr_no = net->HasNetFlag(NETWORK_STATE::INIT_WTS_1_THREAD) ? 0 : thr_no;
  
    for(int i=0; i<cg->size; i++) {
      scales[i] = 1.0f;         // default -- must be set in prjn spec if different
    }
    
    if(rnd.type != STATE_CLASS(Random)::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i], eff_thr_no);
        C_Init_dWt(dwts[i]);
        ntrs[i] = 0.0f;
        trs[i] = 0.0f;
      }
    }
  }

  INLINE float GetActVal(LEABRA_UNIT_STATE* u, const LearnActVal& val) {
    switch(val) {
    case PREV_TRIAL:
      return u->act_q0;
    case ACT_P:
      return u->act_p;
    case ACT_M:
      return u->act_m;
    case ACT_EQ:
      return u->act_eq;
    case ACT_G:
      return u->act_g;
    }
    return 0.0f;
  }    
  
  // IMPORTANT: need to always build in a temporal asymmetry so LV-level gating does not
  // disrupt the trace right as it is established..
  
  INLINE void C_Compute_dWt_DaHebb
    (float& dwt, const float da_p, const bool d2r, const float ru_act, const float su_act,
     const float lrate_eff) {
    dwt += lrate_eff * GetDa(da_p, d2r) * ru_act * su_act;
  }
  // #IGNORE
  INLINE void C_Compute_dWt_DaHebbVS
    (float& dwt, const float da_p, const bool d2r, const float ru_act,
     const float deep_mod_net, const float su_act, const float lrate_eff) {
    float eff_ru_act = fmaxf(ru_act, fminf(deep_mod_net, trace.max_vs_deep_mod));
    dwt += lrate_eff * GetDa(da_p, d2r) * eff_ru_act * su_act;
  }
  // #IGNORE

  INLINE void C_Compute_dWt_Trace_Thal
    (float& dwt, float& ntr, float& tr, const float da_p,
     const float ach, const bool d2r, const float ru_thal, const float ru_act,
     const float su_act, const float lrate_eff, const float ru_deep_raw_net) {

    const float da = GetDa(da_p, d2r);
    const bool pos_da = (da_p > 0.0f); // raw da
    if(da != 0.0f) {
      dwt += tr_thal.Lrate((tr > 0.0f), d2r, pos_da) * lrate_eff * da * tr;
    }

    if(ach >= trace.ach_reset_thr) {
      tr = 0.0f;
    }

    float new_ntr = trace.MsnActLrnFactor(ru_act) * su_act;
    if(ru_thal > 0.0f) {        // gated
      ntr = new_ntr;
    }
    else {                      // not-gated
      ntr = -new_ntr;           // opposite sign for non-gated
    }

    float decay_factor = fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;
    tr += ntr - decay_factor * tr;
  }
  // #IGNORE

  INLINE void C_Compute_dWt_Trace_NoThal
    (float& dwt, float& ntr, float& tr, const float da_p, const float ach, const bool d2r,
     const float ru_act, const float su_act, const float lrate_eff) {

    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * da * tr;

    if(ach >= trace.ach_reset_thr) {
      tr = 0.0f;
    }
    
    ntr = trace.MsnActLrnFactor(ru_act) * su_act;
    
    float decay_factor = fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;
    tr += ntr - decay_factor * tr;
  }
  // #IGNORE
  
  INLINE void C_Compute_dWt_Trace_NoThalVS
  (float& dwt, float& ntr, float& tr, const float da_p, const float ach, const bool d2r,
   const float ru_act, const float deep_mod_net, const float su_act, const float lrate_eff) {
    
    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * da * tr;
    
    if(ach >= trace.ach_reset_thr) {
      tr = 0.0f;
    }
    
    float eff_ru_act = fmaxf(ru_act, fminf(deep_mod_net, trace.max_vs_deep_mod));
    
    ntr = trace.MsnActLrnFactor(eff_ru_act) * su_act;
    
    float decay_factor = fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;
    tr += ntr - decay_factor * tr;
  }
  // #IGNORE

  INLINE void ClearMSNTrace(LEABRA_CON_STATE* scg, LEABRA_NETWORK_STATE* net, int thr_no) {
    float* trs = scg->OwnCnVar(TR);
    const int sz = scg->size;
    for(int i=0; i<sz; i++) {
      trs[i] = 0.0f;
    }
  }
  // #IGNORE clear the trace value

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->ThrOwnUnState(net, thr_no);

    const float su_act = GetActVal(su, su_act_var);
    const bool q4 = (net->quarter == 3);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, net, thr_no, clrate, deep_on, bg_lrate, fg_lrate);
    
    float* dwts = cg->OwnCnVar(DWT);
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);
    
    const int sz = cg->size;
    switch(learn_rule) {
    case DA_HEBB: {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i,net);
        bool d2r = (ru->HasUnitFlag(LEABRA_UNIT_STATE::D2R));
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        C_Compute_dWt_DaHebb(dwts[i], ru->da_p, d2r, ru_act, su_act, lrate_eff);
      }
      break;
    }
    case DA_HEBB_VS: {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i,net);
        bool d2r = (ru->HasUnitFlag(LEABRA_UNIT_STATE::D2R));
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        C_Compute_dWt_DaHebbVS(dwts[i], ru->da_p, d2r, ru_act, ru->deep_mod_net,
                               su_act, lrate_eff);
      }
      break;
    }
    case TRACE_THAL: {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i,net);
        bool d2r = (ru->HasUnitFlag(LEABRA_UNIT_STATE::D2R));
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        const float ach = q4 ? ru->ach : 0.0f;
        C_Compute_dWt_Trace_Thal
          (dwts[i], ntrs[i], trs[i], ru->da_p, ach, d2r, ru->thal_cnt,
           ru_act, su_act, lrate_eff, ru->deep_raw_net);
      }
      break;
    }
    case TRACE_NO_THAL: {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i,net);
        bool d2r = (ru->HasUnitFlag(LEABRA_UNIT_STATE::D2R));
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        const float ach = q4 ? ru->ach : 0.0f;
        C_Compute_dWt_Trace_NoThal(dwts[i], ntrs[i], trs[i],
                            ru->da_p, ach, d2r, ru_act, su_act, lrate_eff);
      }
      break;
    }
    case TRACE_NO_THAL_VS: {
      for(int i=0; i<sz; i++) {
        LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i,net);
        bool d2r = (ru->HasUnitFlag(LEABRA_UNIT_STATE::D2R));
        float lrate_eff = clrate;
        if(deep_on) {
          //            lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
          lrate_eff *= (bg_lrate + fg_lrate); // TODO: deep_lrn was turning off before phaDA hits
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        const float ach = q4 ? ru->ach : 0.0f;
        C_Compute_dWt_Trace_NoThalVS(dwts[i], ntrs[i], trs[i], ru->da_p, ach, d2r,
                                     ru_act, ru->deep_mod_net, su_act, lrate_eff);
      }
      break;
    }
    case WM_DEPENDENT: {
      // todo!
      break;
    }
    }
  }
  
  INLINE void Initialize_core() {
    su_act_var = PREV_TRIAL;
    ru_act_var = ACT_P;
    learn_rule = DA_HEBB_VS;
    burst_da_gain = 1.0f;
    dip_da_gain = 1.0f;
    wt_limits.sym = false;
    lrate = 0.005f;
    learn_qtr = Q2_Q4;            // beta frequency default
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_MSNConSpec; }

