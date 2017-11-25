// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  STATE_CLASS(ChlSpecs)	chl;		// #CAT_Learning CHL learning parameters

  INLINE float Compute_SAvgCor(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no) {
    LEABRA_LAYER_STATE* slay = cg->GetSendLayer(net);
    float savg = .5f + chl.savg_cor * (slay->acts_p_avg_eff - .5f);
    savg = fmaxf(chl.savg_thresh, savg); // keep this computed value within bounds
    return .5f / savg;
  }
  // #IGNORE compute sending average activation, corrected

  INLINE float	C_Compute_Hebb(const float cg_savg_cor, const float lin_wt,
			       const float ru_act, const float su_act) 
  { return ru_act * (su_act * (cg_savg_cor - lin_wt) - (1.0f - su_act) * lin_wt); }
  // #IGNORE compute Hebbian associative learning

  INLINE float 	C_Compute_Err_LeabraCHL(const float lin_wt,
					const float ru_act_p, const float ru_act_m,
					const float su_act_p, const float su_act_m)
  { float err = (ru_act_p * su_act_p) - (ru_act_m * su_act_m);
    if(err > 0.0f)	err *= (1.0f - lin_wt);
    else		err *= lin_wt;
    return err;
  }
  // #IGNORE compute generec error term, sigmoid case

  INLINE void 	C_Compute_dWt_LeabraCHL(float& dwt, const float heb, const float err)
  {  dwt += cur_lrate * (chl.err * err + chl.hebb * heb); }
  // #IGNORE combine associative and error-driven weight change, actually update dwt

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    if(!chl.use) {
      Compute_dWt(scg, snet, thr_no);
      return;
    }
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = cg->ThrOwnUnState(net, thr_no);
    LEABRA_LAYER_STATE* slay = cg->GetSendLayer(net);
    LEABRA_UNGP_STATE* lgpd = slay->GetLayUnGpState(net);
 
    if(lgpd->acts_p.avg < chl.savg_thresh) return;

    const float savg_cor = Compute_SAvgCor(cg, net, thr_no);
    const float su_act_m = su->act_m;
    const float su_act_p = su->act_p;
    float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = cg->UnState(i,net);
      if(ru->lesioned()) continue;
      const float lin_wt = fwts[i];
      C_Compute_dWt_LeabraCHL
        (dwts[i],
         C_Compute_Hebb(savg_cor, lin_wt, ru->act_p, su_act_p),
         C_Compute_Err_LeabraCHL(lin_wt, ru->act_p, ru->act_m, su_act_p, su_act_m));
    }
  }

  INLINE void	C_Compute_Weights_LeabraCHL
    (float& wt, float& dwt, float& fwt, float& swt, float& scale)
  { if(dwt != 0.0f) {
      fwt += dwt;
      swt = fwt;                // keep sync'd -- not tech necc..
      wt = scale * SigFmLinWt(fwt);
      dwt = 0.0f;
    }
  }
  // #IGNORE 

  INLINE void	C_Compute_Weights_LeabraCHL_slow
    (float& wt, float& dwt, float& fwt, float& swt, float& scale)
  { 
    fwt += dwt;
    float eff_wt = slow_wts.swt_pct * swt + slow_wts.fwt_pct * fwt;
    float nwt = scale * SigFmLinWt(eff_wt);
    wt += slow_wts.wt_dt * (nwt - wt);
    swt += slow_wts.slow_dt * (fwt - swt);
    dwt = 0.0f;
  }
  // #IGNORE 

  INLINE void Compute_Weights(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    if(!chl.use) {
      Compute_Weights(scg, snet, thr_no);
      return;
    }
    if(!learn) return;

    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);
    float* swts = cg->OwnCnVar(SWT);
    float* scales = cg->OwnCnVar(SCALE);

    if(slow_wts.on) {
      CON_STATE_LOOP(cg, C_Compute_Weights_LeabraCHL_slow
                     (wts[i], dwts[i], fwts[i], swts[i], scales[i]));
    }
    else {
      CON_STATE_LOOP(cg, C_Compute_Weights_LeabraCHL
                     (wts[i], dwts[i], fwts[i], swts[i], scales[i]));
    }
  }

  INLINE void Initialize_core() {
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_CHLConSpec; }

