// this is included directly in LeabraExtraLayerSpecs_cpp / _cuda
// {

// rep 1.5.  ugp_size = 4, incr = 1.5 / 3 = .5
// 0  .5   1
// oooo111122222 = val / incr

// 0 .5  1  val = .8, incr = .5
// 0 .4 .6
// (.4 * .5 + .6 * 1) / (.6 + .4) = .8

// act = 1.0 - (fabs(val - cur) / incr)


void STATE_CLASS(ScalarValLayerSpec)::Compute_BiasVal(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  if(bias_val.un != STATE_CLASS(ScalarValBias)::NO_UN) {
    if(bias_val.un_shp == STATE_CLASS(ScalarValBias)::VAL) {
      UNIT_GP_ITR(lay, Compute_UnBias_Val(lay, net, gpidx, bias_val.val); );
    }
    else if(bias_val.un_shp == STATE_CLASS(ScalarValBias)::NEG_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_NegSlp(lay, net, gpidx); );
    }
    else if(bias_val.un_shp == STATE_CLASS(ScalarValBias)::POS_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_PosSlp(lay, net, gpidx); );
    }
  }
  if(bias_val.wt == STATE_CLASS(ScalarValBias)::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(lay, net, gpidx, bias_val.val); );
  }
}

void STATE_CLASS(ScalarValLayerSpec)::Compute_WtBias_Val
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, float val) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  const int nunits = ug->n_units;
  if(nunits < 2) return;        // must be at least a few units..
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    float act = .03f * bias_val.wt_gain * scalar.GetUnitAct(i);
    const int nrg = u->NRecvConGps(net);
    for(int g=0; g<nrg; g++) {
      LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
      if(recv_gp->NotActive()) continue;
      LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
      if(cs->IsMarkerCon()) continue;
      PRJN_STATE* prjn = recv_gp->GetPrjnState(net);
      PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(net);
      if(pspec->GetStateSpecType() == LEABRA_NETWORK_STATE::T_ScalarValSelfPrjnSpec) continue;
      for(int ci=0;ci<recv_gp->size;ci++) {
        float& wt = recv_gp->PtrCn(ci, CON_STATE::WT, net);
        wt += act;
        if(wt < cs->wt_limits.min) wt = cs->wt_limits.min;
        if(wt > cs->wt_limits.max) wt = cs->wt_limits.max;
      }
    }
  }
}

void STATE_CLASS(ScalarValLayerSpec)::Compute_UnBias_Val
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, float val) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  const int nunits = ug->n_units;
  if(nunits < 2) return;        // must be at least a few units..
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    float act = bias_val.un_gain * scalar.GetUnitAct(i);
    u->bias_wt = act;
  }
}

void STATE_CLASS(ScalarValLayerSpec)::Compute_UnBias_NegSlp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  const int nunits = ug->n_units;
  if(nunits < 2) return;        // must be at least a few units..
  float incr = bias_val.un_gain / (float)(nunits - 1);
  float val = 0.0f;
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    u->bias_wt = -val;
    val += incr;
  }
}

void STATE_CLASS(ScalarValLayerSpec)::Compute_UnBias_PosSlp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  const int nunits = ug->n_units;
  if(nunits < 2) return;        // must be at least a few units..
  float val = bias_val.un_gain;
  float incr = bias_val.un_gain / (float)(nunits - 1);
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    u->bias_wt = val;
    val += incr;
  }
}

void STATE_CLASS(ScalarValLayerSpec)::ClampValue_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, float rescale) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return;        // must be at least a few units..
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, 0);
  if(!clamp.hard)
    u->ClearExtFlag(UNIT_STATE::EXT);
  else
    u->SetExtFlag(UNIT_STATE::EXT);
  float val = u->ext_orig;     // have to use the orig value b/c ext gets computed!
  if(scalar.clip_val)
    val = val_range.Clip(val);          // first unit has the value to clamp
  u->misc_1 = val;        // record this val
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);

  float avg_act = 0.0f;
  if(scalar.rep == STATE_CLASS(ScalarValSpec)::AVG_ACT) {
    avg_act = avg_act_range.Project(unit_range.Normalize(val));
  }

  for(int i=0;i<nunits;i++) {
    u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    float act;
    if(scalar.rep == STATE_CLASS(ScalarValSpec)::AVG_ACT) {
      act = avg_act;
    }
    else {
      act = rescale * scalar.GetUnitAct(i);
    }
    if(act < us->opt_thresh.send)
      act = 0.0f;
    u->SetExtFlag(UNIT_STATE::EXT);
    u->ext = act;
  }
}

float STATE_CLASS(ScalarValLayerSpec)::ClampAvgAct(int ugp_size) {
  if(ugp_size < 2) return 0.0f;
  float val = val_range.min + .5f * val_range.Range(); // half way
  scalar.InitVal(val, ugp_size, unit_range.min, unit_range.range);
  float sum = 0.0f;
  for(int i=0;i<ugp_size;i++) {
    float act = scalar.GetUnitAct(i);
    sum += act;
  }
  sum /= (float)(ugp_size - 1);
  return sum;
}

float STATE_CLASS(ScalarValLayerSpec)::ReadValue_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return 0.0f;        // must be at least a few units..
  scalar.InitVal(0.0f, nunits, unit_range.min, unit_range.range);
  float avg = 0.0f;
  float sum_act = 0.0f;
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    float cur = scalar.GetUnitVal(i);
    float act_val = 0.0f;
    const float act_eq = u->act_eq;
    if(!scalar.send_thr || (act_eq >= us->opt_thresh.send)) // only if over sending thresh!
      act_val = us->clamp_range.Clip(act_eq) / us->clamp_range.max; // clipped & normalized!
    avg += cur * act_val;
    sum_act += act_val;
  }
  sum_act = fmaxf(sum_act, scalar.min_sum_act);
  if(scalar.rep == STATE_CLASS(ScalarValSpec)::AVG_ACT) {
    sum_act /= (float)nunits;
    avg = val_range.Project(avg_act_range.Normalize(sum_act));
  }
  else {
    if(sum_act > 0.0f)
      avg /= sum_act;
  }
  // set the first unit in the group to represent the value
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, 0);
  u->misc_1 = avg;
  return avg;
}

void STATE_CLASS(ScalarValLayerSpec)::ReadValue(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  UNIT_GP_ITR(lay, ReadValue_ugp(lay, net, gpidx); );
}

void STATE_CLASS(ScalarValLayerSpec)::Compute_ExtToPlus_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return;        // must be at least a few units..
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    u->act_p = us->clamp_range.Clip(u->ext);
    u->act_dif = u->act_p - u->act_m;
    // important to clear ext stuff, otherwise it will get added into netin next time around!!
    u->ext = 0.0f;
    u->ClearExtFlag(UNIT_STATE::COMP_TARG_EXT);
  }
}

void STATE_CLASS(ScalarValLayerSpec)::Compute_ExtToAct_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return;        // must be at least a few units..
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    u->act_eq = u->act = us->clamp_range.Clip(u->ext);
    u->ext = 0.0f;
    u->ClearExtFlag(UNIT_STATE::COMP_TARG_EXT);
  }
}

void STATE_CLASS(ScalarValLayerSpec)::HardClampExt_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return;        // must be at least a few units..
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    us->Compute_HardClamp(u, net, u->thread_no);
  }
}

void STATE_CLASS(ScalarValLayerSpec)::HardClampExt(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  UNIT_GP_ITR(lay, HardClampExt_ugp(lay, net, gpidx); );
}

void STATE_CLASS(ScalarValLayerSpec)::Quarter_Init_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  inherited::Quarter_Init_Layer(lay, net);
  if(bias_val.un == STATE_CLASS(ScalarValBias)::BWT) {
    LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
    LEABRA_CON_SPEC_CPP* bspec = (LEABRA_CON_SPEC_CPP*)us->GetBiasSpec(net);
    if(!bspec) return;
    for(int ui = 0; ui < lay->n_units; ui++) {
      LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, ui);
      u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
      u->bias_scale /= 100.0f;
    }
  }
}

void STATE_CLASS(ScalarValLayerSpec)::Quarter_Init_Layer_Post
  (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  if(scalar.clamp_pat) {
    return;
  }
  if(!lay->HasExtFlag(UNIT_STATE::EXT)) {
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, ClampValue_ugp(lay, net, gpidx); );
  // now check for actual hard clamping
  if(!clamp.hard) {
    return;
  }
  HardClampExt(lay, net);
}

float STATE_CLASS(ScalarValLayerSpec)::Compute_SSE_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                          int gpidx, int& n_vals) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return 0.0f;        // must be at least a few units..
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, 0);
  // only count if target value is within range -- otherwise considered a non-target
  if(u->HasExtFlag(UNIT_STATE::COMP_TARG) && val_range.RangeTestEq(u->targ)) {
    n_vals++;
    float uerr = u->targ - u->misc_2;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return uerr * uerr;
  }
  return 0.0f;
}

float STATE_CLASS(ScalarValLayerSpec)::Compute_SSE
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  lay->sse = 0.0f;
  lay->bin_err = 0.0f;
  if(!(lay->HasExtFlag(UNIT_STATE::COMP_TARG))) return 0.0f;
  if(lay->layer_type == LAYER_STATE::HIDDEN) return 0.0f;
  UNIT_GP_ITR(lay, lay->sse += Compute_SSE_ugp(lay, net, gpidx, n_vals); );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  lay->avg_sse.Increment(lay->sse);
  if(lay->sse > net->stats.cnt_err_tol)
    lay->cur_cnt_err += 1.0;
  lay->bin_err = (lay->sse > net->stats.cnt_err_tol) ? 1.0f : 0.0f;
  if(lay->HasLayerFlag(LAYER_STATE::NO_ADD_SSE) ||
     (lay->HasExtFlag(UNIT_STATE::COMP) && lay->HasLayerFlag(LAYER_STATE::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float STATE_CLASS(ScalarValLayerSpec)::Compute_NormErr_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return 0.0f;        // must be at least a few units..
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, 0);
  // only count if target value is within range -- otherwise considered a non-target
  if(u->HasExtFlag(UNIT_STATE::COMP_TARG) && val_range.RangeTestEq(u->targ)) {
    float uerr = u->targ - u->misc_2;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return fabsf(uerr);
  }
  return 0.0f;
}

float STATE_CLASS(ScalarValLayerSpec)::Compute_NormErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!lay->HasExtFlag(UNIT_STATE::COMP_TARG)) return -1.0f; // indicates not applicable
  if(lay->layer_type == LAYER_STATE::HIDDEN) return -1.0f;

  float nerr = 0.0f;
  float ntot = 0;
  if(HasUnitGpInhib(lay)) {
    for(int g=0; g < lay->gp_geom_n; g++) {
      nerr += Compute_NormErr_ugp(lay, net, g);
      ntot += unit_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, net, -1);
    ntot += unit_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  lay->avg_norm_err.Increment(lay->norm_err);
  
  if(lay->HasLayerFlag(LAYER_STATE::NO_ADD_SSE) ||
     (lay->HasExtFlag(UNIT_STATE::COMP) && lay->HasLayerFlag(LAYER_STATE::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}

