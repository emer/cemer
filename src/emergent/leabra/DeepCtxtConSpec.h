// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  // special!
  INLINE bool  DoesStdNetin() override { return false; }
  INLINE bool  DoesStdDwt() override { return false; }
  INLINE bool  IsDeepCtxtCon() override { return true; }
  INLINE void  Trial_Init_Specs(LEABRA_NETWORK_STATE* net) override {
    inherited::Trial_Init_Specs(net);
    net->deep.ctxt = true;
  }

  INLINE void Send_DeepCtxtNetin(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net,
                                 int thr_no, const float su_act) {
    LEABRA_PRJN_STATE* prjn = cg->GetPrjnState(net);
    const float su_act_eff = prjn->scale_eff * su_act;
    float* wts = cg->OwnCnVar(WT);
    float* send_deepnet_vec = net->ThrSendDeepRawNetTmp(thr_no);
#ifdef TA_VEC_USE
    Send_NetinDelta_vec(cg, su_act_eff, send_deepnet_vec, wts);
#else
    CON_STATE_LOOP(cg, C_Send_NetinDelta(wts[i], send_deepnet_vec,
                                         cg->UnIdx(i), su_act_eff));
#endif
  }
  // #IGNORE sender-based activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_DeepCtxt function on units

  // don't send regular net inputs..
  INLINE void Send_NetinDelta(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no, 
                              const float su_act_delta) override { };
  INLINE float Compute_Netin(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override
  { return 0.0f; }

  INLINE float C_Compute_dWt_Delta
    (const float ru_avg_s, const float ru_avg_m, const float su_deep_prv) {
    float new_dwt = su_deep_prv * (ru_avg_s - ru_avg_m);
    return new_dwt;
  }
  // #IGNORE

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, net, thr_no, clrate, deep_on, bg_lrate, fg_lrate);

    const float su_avg_s = su->deep_raw_prv; // value sent on prior trial..
    const float su_avg_m = su->deep_raw_prv;
    
    LEABRA_PRJN_STATE* prjn = cg->GetPrjnState(net);
    if(momentum.on) {
      clrate *= momentum.lr_comp;
    }

    float err_dwt_max = 0.0f;
    float bcm_dwt_max = 0.0f;
    float dwt_max = 0.0f;
    float err_dwt_avg = 0.0f;
    float bcm_dwt_avg = 0.0f;
    float dwt_avg = 0.0f;

    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;

    float* dwnorms = cg->OwnCnVar(DWNORM);
    float* moments = cg->OwnCnVar(MOMENT);
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = cg->UnState(i, net);
      if(ru->lesioned()) continue;
      // note: applying opt_thresh.xcal_lrn here does NOT work well for dwt_zone..
      float lrate_eff = clrate;
      if(deep_on) {
        lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
      }
      if(margin.lrate_mod) {
        lrate_eff *= margin.MarginLrate(ru->margin);
      }
      float err = C_Compute_dWt_Delta(ru->avg_s, ru->avg_m, su_avg_s);

      float abserr = fabsf(err);
      if(dwt_norm.stats) {
        err_dwt_max = fmaxf(abserr, err_dwt_max);
        err_dwt_avg += abserr;
      }

      float norm = 1.0f;
      if(dwt_norm.on) {
        norm = dwt_norm.ComputeNorm(dwnorms[i], abserr); // always update
      }

      if(momentum.on) {
        err = norm * momentum.ComputeMoment(moments[i], err);
      } else {
        err *= norm;
      }

      if(dwt_norm.stats) {
        float absdwt = fabsf(err);
        dwt_max = fmaxf(absdwt, dwt_max); 
        dwt_avg += absdwt; 
      }

      dwts[i] += lrate_eff * err; // lrate always at the end!
    }
    
    if(dwt_norm.stats) {
      cg->err_dwt_max = err_dwt_max;
      cg->bcm_dwt_max = bcm_dwt_max;
      cg->dwt_max = dwt_max;

      if(sz > 0) {
        float nrm = 1.0f / (float)sz;
        cg->err_dwt_avg = err_dwt_avg * nrm;
        cg->bcm_dwt_avg = bcm_dwt_avg * nrm;
        cg->dwt_avg = dwt_avg * nrm;
      }
    }

    if(dwt_norm.SendConsAgg()) {
      DwtNorm_SendCons(cg, net, thr_no);
    }
  }

  INLINE void Initialize_core() {
    wt_scale.rel = 1.0;
    momentum.on = false;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_DeepCtxtConSpec; }

