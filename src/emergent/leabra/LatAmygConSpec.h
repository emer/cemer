// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  STATE_CLASS(LatAmygGains)          lat_amyg;
  // all the parameters for lateral amygdala

  INLINE float  GetDa(float da)
  { da = (da < 0.0f) ? lat_amyg.dip_da_gain * da : lat_amyg.burst_da_gain * da;
    if(lat_amyg.dar == STATE_CLASS(LatAmygGains)::D2R)
      da = -da;
    else if(lat_amyg.dar == STATE_CLASS(LatAmygGains)::D1D2R)
      da = fabsf(da);
    if(da < 0.0f) da *= lat_amyg.neg_lrate;
    return da;
    //if(fwt > lat_amyg.wt_decay_floor) da -= lat_amyg.wt_decay_rate * fwt;
  }
  // get overall dopamine value -- depends on da receptor type etc
  
  INLINE void C_Compute_dWt_LatAmyg(float& dwt, const float su_act, const float da_p, 
                                    const float fwt)
  {
    float wt_above_floor = fwt - lat_amyg.wt_decay_floor;
    wt_above_floor = fmaxf(wt_above_floor,0.0f); // positive-rectify
    dwt += (cur_lrate * su_act * GetDa(da_p)) - lat_amyg.wt_decay_rate * wt_above_floor;
    //dwt += cur_lrate * su_act * GetDa(da_p, fwt);
  }
  // #IGNORE dopamine multiplication

  INLINE void Compute_dWt(CON_STATE* scg, NETWORK_STATE* snet, int thr_no) override {
    LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)snet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)scg;
    LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->ThrOwnUnState(net, thr_no);
    
    float su_act = su->act_q0;
    float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LEABRA_UNIT_STATE* ru = (LEABRA_UNIT_STATE*)cg->UnState(i, net);
      C_Compute_dWt_LatAmyg(dwts[i], su_act, ru->da_p, fwts[i]);
    }
  }

  INLINE void Initialize_core() {
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LatAmygConSpec; }

