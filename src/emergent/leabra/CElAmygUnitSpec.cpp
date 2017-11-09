// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(CElAmygUnitSpec)::Init_UnitState
  (UNIT_STATE* uv, NETWORK_STATE* net, int thr_no)  {

  inherited::Init_UnitState(uv, net, thr_no);
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
  if(dar == D1R) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::D1R);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::D2R);
  }
  else {
    u->SetUnitFlag(LEABRA_UNIT_STATE::D2R);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::D1R);
  }
  if(acq_ext == ACQ) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::ACQUISITION);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::EXTINCTION);
  }
  else {
    u->SetUnitFlag(LEABRA_UNIT_STATE::EXTINCTION);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::ACQUISITION);
  }
  if(valence == APPETITIVE) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::APPETITIVE);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::AVERSIVE);
  }
  else {
    u->SetUnitFlag(LEABRA_UNIT_STATE::AVERSIVE);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::APPETITIVE);
  }
}

void STATE_CLASS(CElAmygUnitSpec)::Compute_DeepMod
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
  if(deep.SendDeepMod()) {
    u->deep_lrn = u->deep_mod = u->act;      // record what we send!
  }
  else if(deep.IsTRC()) {
    u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
    if(trc.thal_gate) {
      u->net *= u->thal;
    }
  }
  // must be SUPER units at this point
  // else if(lay->am_deep_mod_net.max <= deep.mod_thr) { // not enough yet
  else if(u->deep_mod_net <= deep.mod_thr) { // not enough yet
    u->deep_lrn = 0.0f;    // default is 0!
    if(deep_mod_zero) {
      u->deep_mod = 0.0f;
    }
    else {
      u->deep_mod = 1.0f;
    }
  }
  else {
    u->deep_lrn = u->deep_mod_net / lgpd->am_deep_mod_net.max;
    u->deep_mod = 1.0f;                               // do not modulate with deep_mod!
  }
}

float STATE_CLASS(CElAmygUnitSpec)::Compute_DaModNetin
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, float& net_syn) {
  
  float da_val = u->da_p;
  if(da_val > 0.0f) {
    da_val *= cel_da_mod.burst_da_gain;
  }
  else {
    da_val *= cel_da_mod.dip_da_gain;
  }
  
  float mod_val = net_syn;

  if(acq_ext == ACQ) {
    da_val *= mod_val;
    if(dar == D2R)
      da_val = -da_val;           // flip the sign
    if(net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE) {
      return da_mod.plus * da_val;
    }
    else {                      // MINUS_PHASE
      return da_mod.minus * da_val;
    }
  }
  else {                        // extinction -- gets from Acq/Go inhib
    if(cel_da_mod.acq_deep_mod) {
      da_val *= fmaxf(u->deep_mod_net, mod_val);
    }
    else {
      da_val *= fmaxf(u->gi_syn, mod_val);
    }
    if(dar == D2R)
      da_val = -da_val;           // flip the sign
    if(net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE) {
      return da_mod.plus * da_val;
    }
    else {                      // MINUS_PHASE
      return da_mod.minus * da_val;
    }
  }
}

float STATE_CLASS(CElAmygUnitSpec)::Compute_NetinExtras
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, float& net_syn) {
  
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
  LEABRA_LAYER_SPEC_CPP* ls = (LEABRA_LAYER_SPEC_CPP*)lay->GetLayerSpec(net);

  float net_ex = init.netin;
  LEABRA_CON_SPEC_CPP* bs = (LEABRA_CON_SPEC_CPP*)GetBiasSpec(net);
  if(bs) {
    net_ex += u->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UNIT_STATE::EXT)) {
    if(ls->clamp.avg)
      net_syn = ls->clamp.ClampAvgNetin(u->ext, net_syn);
    else
      net_ex += u->ext * ls->clamp.gain;
  }
  // if not using act_eq for learning, use deep_raw_net (i.e., US-input) to modulate net_syn
  //  if it exists!
  if(cel_da_mod.lrn_mod_act) {
    
  }
  if(deep.ApplyDeepCtxt()) {
    net_ex += u->deep_ctxt;
  }
  if(cel_da_mod.lrn_mod_act) {
    net_ex += Compute_DaModNetin(u, net, thr_no, net_syn);
    // also need to modulate net_syn for US (PV) inputs, if present
    if(u->deep_raw_net > 0.1f) {
      net_syn = cel_da_mod.us_clamp_avg * u->deep_raw_net + (1.0f - cel_da_mod.us_clamp_avg) * net_syn;
    }
  }
  return net_ex;
}

void STATE_CLASS(CElAmygUnitSpec)::Compute_ActFun_Rate
(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  inherited::Compute_ActFun_Rate(u, net, thr_no);
  // default is to use act_eq for later use by C_Compute__dWt_CEl_Delta() to effect learning *as if* phasic dopamine modulates activations - but without actually doing it!
  if(!cel_da_mod.lrn_mod_act) {
    float da_val = u->da_p;
    if(da_val > 0.0f) {
      da_val *= cel_da_mod.burst_da_gain;
    }
    else {
      da_val *= cel_da_mod.dip_da_gain;
    }
    if(dar == D2R) { da_val = -da_val; } // flip the sign
    u->act_eq *= (1.0f + da_val); // co-opt act_eq variable for wt changes
    // similarly, clamp act_eq to US (PV) inputs (deep_raw_net), if present
    if(u->deep_raw_net > 0.01f) {
      u->act_eq = u->deep_raw_net;
    }
  }
  // ELSE: actually *DOES* modulate actual activations - CAUTION - very brittle!
}

