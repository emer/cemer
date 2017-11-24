// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(BLAmygUnitSpec)::Init_UnitState
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
}

void STATE_CLASS(BLAmygUnitSpec)::Compute_DeepMod
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {

  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
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
  else if(u->deep_mod_net <= deep.mod_thr) { // per-unit, NOT layer
    u->deep_lrn = 0.0f;    // default is 0!
    if(deep_mod_zero) {
      u->deep_mod = 0.0f; // makes activity dependent on deep_mod_net
    }
    else {
      u->deep_mod = 1.0f;
    }
  }
  else {
    u->deep_lrn = u->deep_mod_net / lgpd->am_deep_mod_net.max;
    u->deep_mod = 1.0f;
  }
  
  if(bla_ach_mod.on) {          // use deep mod to reflect ach modulation
    u->deep_mod = bla_ach_mod.mod_min + bla_ach_mod.mod_min_c * u->ach;
  }
}

float STATE_CLASS(BLAmygUnitSpec)::Compute_DaModNetin
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, float& net_syn) {
  float da_val = u->da_p;
  if(da_val > 0.0f) {
    da_val *= bla_da_mod.burst_da_gain;
  }
  else {
    da_val *= bla_da_mod.dip_da_gain;
  }

  float mod_val = u->act_eq;
  if(bla_da_mod.lrn_mod_act) {
    mod_val = bla_da_mod.pct_act * u->act_eq + (1.0f - bla_da_mod.pct_act) * net_syn;
  }
    
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

float STATE_CLASS(BLAmygUnitSpec)::Compute_NetinExtras
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, float& net_syn) {
  
  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
  LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(net);

  float net_ex = init.netin;
  LEABRA_CON_SPEC_CPP* bs = GetBiasSpec(net);
  if(bs) {
    net_ex += lay->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UNIT_STATE::EXT)) {
    if(ls->clamp.avg)
      net_syn = ls->clamp.ClampAvgNetin(u->ext, net_syn);
    else
      net_ex += u->ext * ls->clamp.gain;
  }
  
  if(deep.ApplyDeepCtxt()) {
    net_ex += u->deep_ctxt;
  }
  
  if(bla_da_mod.lrn_mod_act) {
    net_ex += Compute_DaModNetin(u, net, thr_no, net_syn);
    // similarly for US (PV) inputs (deep_raw_net), if present
    if(u->deep_raw_net > 0.1f) {
      net_syn = bla_da_mod.us_clamp_avg * u->deep_raw_net + (1.0f - bla_da_mod.us_clamp_avg) * net_syn;
    }
  }
  return net_ex;
}

void STATE_CLASS(BLAmygUnitSpec)::Compute_ActFun_Rate
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  inherited::Compute_ActFun_Rate(u, net, thr_no);
  
  // default is to use act_eq for later use by C_Compute__dWt_CEl_Delta() to effect learning *as if* phasic dopamine modulates activations - but without actually doing it!
  if(!bla_da_mod.lrn_mod_act) {
    float da_val = u->da_p;
    if(da_val > 0.0f) {
      da_val *= bla_da_mod.burst_da_gain;
    }
    else {
      da_val *= bla_da_mod.dip_da_gain;
    }
    if(dar == D2R) { da_val = -da_val; } // flip the sign
    u->act_eq *= (1.0f + da_val);
  }
}

