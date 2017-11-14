// this is included directly in LeabraExtraLayerSpecs_cpp / _cuda
// {


void STATE_CLASS(TwoDValLayerSpec)::Compute_WtBias_Val
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, float x_val, float y_val) {

  LEABRA_UNGP_STATE* ug = lay->GetUnGpState(net, gpidx);
  const int nunits = ug->n_units;
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(x_val, y_val, lay->un_geom_x, lay->un_geom_y, x_range.min, x_range.range, y_range.min,
               y_range.range);
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    float act = .03f * bias_val.wt_gain * twod.GetUnitAct(i);
    int rsz = u->NRecvConGps(net);
    for(int g=0; g < rsz; g++) {
      LEABRA_CON_STATE* recv_gp = u->RecvConState(net, g);
      if(recv_gp->NotActive()) continue;
      LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
      if(cs->IsMarkerCon()) continue;
      for(int ci=0;ci<recv_gp->size;ci++) {
        float& wt = recv_gp->PtrCn(ci, CON_STATE::WT, net);
        wt += act;
        if(wt < cs->wt_limits.min) wt = cs->wt_limits.min;
        if(wt > cs->wt_limits.max) wt = cs->wt_limits.max;
      }
    }
  }
}

void STATE_CLASS(TwoDValLayerSpec)::Compute_UnBias_Val
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, float x_val, float y_val) {
  
  LEABRA_UNGP_STATE* ug = lay->GetUnGpState(net, gpidx);
  const int nunits = ug->n_units;
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(x_val, y_val, lay->un_geom_x, lay->un_geom_y, x_range.min, x_range.range, y_range.min,
               y_range.range);
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    float act = bias_val.un_gain * twod.GetUnitAct(i);
    u->bias_wt = act;
  }
}

void STATE_CLASS(TwoDValLayerSpec)::Compute_BiasVal(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  if(bias_val.un != STATE_CLASS(TwoDValBias)::NO_UN) {
    UNIT_GP_ITR(lay, Compute_UnBias_Val(lay, net, gpidx, bias_val.x_val, bias_val.y_val););
  }
  if(bias_val.wt == STATE_CLASS(TwoDValBias)::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(lay, net, gpidx, bias_val.x_val, bias_val.y_val););
  }
}

void STATE_CLASS(TwoDValLayerSpec)::ClampValue_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, float rescale) {
  LEABRA_UNGP_STATE* ug = lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 3) return;        // must be at least a few units..
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    u->SetExtFlag(UNIT_STATE::EXT);
    u->ext = 0.0;
  }
  // todo: multiple values.. rarely used..
  // for(int k=0;k<twod.n_vals;k++) {
  float x_val = ug->twod.ext_x;
  float y_val = ug->twod.ext_y;
  if(twod.clip_val) {
    x_val = x_val_range.Clip(x_val);
    y_val = y_val_range.Clip(y_val);
  }
  twod.InitVal(x_val, y_val, lay->un_geom_x, lay->un_geom_y, x_range.min, x_range.range, y_range.min,
               y_range.range);
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    float act = rescale * twod.GetUnitAct(i);
    if(act < us->opt_thresh.send)
      act = 0.0f;
    u->ext += act;
  }
  // }
}

void STATE_CLASS(TwoDValLayerSpec)::ReadValue(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  UNIT_GP_ITR(lay, ReadValue_ugp(lay, net, gpidx); );
}

void STATE_CLASS(TwoDValLayerSpec)::ReadValue_ugp
  (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  
  LEABRA_UNGP_STATE* ug = lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(0.0f, 0.0f, lay->un_geom_x, lay->un_geom_y, x_range.min, x_range.range, y_range.min,
               y_range.range);
  if(twod.n_vals == 1) {        // special case
    float x_avg = 0.0f; float y_avg = 0.0f;
    float sum_act = 0.0f;
    for(int i=0;i<nunits;i++) {
      LEABRA_UNIT_STATE* u = ug->GetUnitState(net, i);
      if(u->lesioned()) continue;
      float x_cur, y_cur;  twod.GetUnitVal(i, x_cur, y_cur);
      float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
      x_avg += x_cur * act_val;
      y_avg += y_cur * act_val;
      sum_act += act_val;
    }
    sum_act = fmaxf(sum_act, twod.min_sum_act);
    if(sum_act > 0.0f) {
      x_avg /= sum_act; y_avg /= sum_act;
    }
    // encode the value
    ug->twod.act_x = x_avg;
    ug->twod.act_y = y_avg;
  }
  // else {                        // todo: multiple items
  //   // first find the max values, using sum of -1..+1 region
  //   static ValIdx_Array sort_ary;
  //   sort_ary.Reset();
  //   for(int i=0;i<nunits;i++) {
  //     float sum = 0.0f;
  //     float nsum = 0.0f;
  //     for(int x=-1;x<=1;x++) {
  //       for(int y=-1;y<=1;y++) {
  //         int idx = i + y * lay->un_geom_x + x;
  //         if(idx < 0 || idx >= nunits) continue;
  //         LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(idx, gpidx);
  //         if(u->lesioned()) continue;
  //         float act_val = us->clamp_range.Clip(u->act_eq()) / us->clamp_range.max; // clipped & normalized!
  //         nsum += 1.0f;
  //         sum += act_val;
  //       }
  //     }
  //     if(nsum > 0.0f) sum /= nsum;
  //     ValIdx vi(sum, i);
  //     sort_ary.Add(vi);
  //   }
  //   sort_ary.Sort();
  //   float mn_x = twod.mn_dst * twod.un_width_x * x_range.Range();
  //   float mn_y = twod.mn_dst * twod.un_width_y * y_range.Range();
  //   float mn_dist = mn_x * mn_x + mn_y * mn_y;
  //   int outi = 0;  int j = 0;
  //   while((outi < twod.n_vals) && (j < sort_ary.size)) {
  //     ValIdx& vi = sort_ary[sort_ary.size - j - 1]; // going backward through sort_ary
  //     float x_cur, y_cur;  twod.GetUnitVal(vi.idx, x_cur, y_cur);
  //     // check distance from all previous!
  //     float my_mn = x_range.Range() + y_range.Range();
  //     for(int k=0; k<j; k++) {
  //       ValIdx& vo = sort_ary[sort_ary.size - k - 1];
  //       if(vo.val == -1.0f) continue; // guy we skipped over before
  //       float x_prv, y_prv;  twod.GetUnitVal(vo.idx, x_prv, y_prv);
  //       float x_d = x_cur - x_prv; float y_d = y_cur - y_prv;
  //       float dist = x_d * x_d + y_d * y_d;
  //       my_mn = fminf(dist, my_mn);
  //     }
  //     if(my_mn < mn_dist) { vi.val = -1.0f; j++; continue; } // mark with -1 so we know we skipped it

  //     // encode the value
  //     lay->SetTwoDVal(x_cur, LEABRA_LAYER_STATE::TWOD_X, LEABRA_LAYER_STATE::TWOD_ACT,
  //                     0, gp_geom_pos.x, gp_geom_pos.y);
  //     lay->SetTwoDVal(y_cur, LEABRA_LAYER_STATE::TWOD_Y, LEABRA_LAYER_STATE::TWOD_ACT,
  //                     0, gp_geom_pos.x, gp_geom_pos.y);
  //     j++; outi++;
  //   }
  // }
}

// todo:
// void STATE_CLASS(TwoDValLayerSpec)::HardClampExt(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
//   inherited::Compute_HardClamp(lay, net);
// }

void STATE_CLASS(TwoDValLayerSpec)::Quarter_Init_TargFlags_Layer_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  
  LEABRA_UNGP_STATE* ug = lay->GetUnGpState(net, gpidx);

  // todo:
  // for(int k=0;k<twod.n_vals;k++) {
  float x_val = ug->twod.targ_x;
  float y_val = ug->twod.targ_y;
  ug->twod.ext_x = x_val;
  ug->twod.ext_y = y_val;
  // }
}


void STATE_CLASS(TwoDValLayerSpec)::Quarter_Init_TargFlags_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  inherited::Quarter_Init_TargFlags_Layer(lay, net);
  // need to actually copy over targ to ext vals!
  if(lay->HasExtFlag(UNIT_STATE::TARG)) {     // only process target layers..
    if(net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE) {
      UNIT_GP_ITR(lay, Quarter_Init_TargFlags_Layer_ugp(lay, net, gpidx); );
    }
  }
}


void STATE_CLASS(TwoDValLayerSpec)::Quarter_Init_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  inherited::Quarter_Init_Layer(lay, net);

  if(bias_val.un == STATE_CLASS(TwoDValBias)::BWT) {
    // if using bias-weight bias, keep a constant scaling (independent of layer size)
    LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
    LEABRA_CON_SPEC_CPP* bspec = us->GetBiasSpec(net);
    if(!bspec) return;
    for(int ui = 0; ui < lay->n_units; ui++) {
      LEABRA_UNIT_STATE* u = lay->GetUnitState(net, ui);
      u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
      u->bias_scale /= 100.0f;
    }
  }
}

void STATE_CLASS(TwoDValLayerSpec)::HardClampExt_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  
  LEABRA_UNGP_STATE* ug = lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return;        // must be at least a few units..
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    us->Compute_HardClamp(u, net, u->thread_no);
  }
}

void STATE_CLASS(TwoDValLayerSpec)::HardClampExt(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  UNIT_GP_ITR(lay, HardClampExt_ugp(lay, net, gpidx); );
}

void STATE_CLASS(TwoDValLayerSpec)::Quarter_Init_Layer_Post
  (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  
  if(twod.clamp_pat) {
    return;
  }
  if(!(lay->ext_flag & UNIT_STATE::EXT)) {
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

void STATE_CLASS(TwoDValLayerSpec)::Quarter_Final_ugp
  (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {

  LEABRA_UNGP_STATE* ug = lay->GetUnGpState(net, gpidx);
  
  // todo:
  // for(int k=0;k<twod.n_vals;k++) {
  float x_val = ug->twod.act_x;
  float y_val = ug->twod.act_y;
  float& x_m = ug->twod.act_m_x;
  float& y_m = ug->twod.act_m_y;
  float& x_p = ug->twod.act_p_x;
  float& y_p = ug->twod.act_p_y;

  if(net->phase == LEABRA_NETWORK_STATE::MINUS_PHASE) {
    x_m = x_val; y_m = y_val;
  }
  else {
    x_p = x_val; y_p = y_val;
    ug->twod.act_dif_x = x_p - x_m;
    ug->twod.act_dif_y = y_p - y_m;
  }
  // }
}

float STATE_CLASS(TwoDValLayerSpec)::Compute_SSE_ugp
  (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, int& n_vals) {

  LEABRA_UNGP_STATE* ug = lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return 0.0f;        // must be at least a few units..
  float rval = 0.0f;
  // for(int k=0;k<twod.n_vals;k++) { // first loop over and find potential target values
  float x_targ = ug->twod.targ_x;
  float y_targ = ug->twod.targ_y;
  // only count if target value is within range -- otherwise considered a non-target
  if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
    n_vals++;
    // now find minimum dist actual activations
    // float mn_dist = taMath::flt_max;
    // for(int j=0;j<twod.n_vals;j++) {
    float x_act_m = ug->twod.act_m_x;
    float y_act_m = ug->twod.act_m_y;
    float dx = x_targ - x_act_m;
    float dy = y_targ - y_act_m;
    if(fabsf(dx) < us->sse_tol) dx = 0.0f;
    if(fabsf(dy) < us->sse_tol) dy = 0.0f;
    float dist = dx * dx + dy * dy;
    // if(dist < mn_dist) {
    //   mn_dist = dist;
    ug->twod.err_x = dx;  ug->twod.err_y = dy;
    ug->twod.sqerr_x = dx * dx;
    ug->twod.sqerr_y = dy * dy;
    rval += dist;
  }
  // }
  return rval;
}

float STATE_CLASS(TwoDValLayerSpec)::Compute_SSE(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                                 int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  lay->sse = 0.0f;
  lay->bin_err = 0.0f;
  if(!(lay->ext_flag & (UNIT_STATE::COMP_TARG))) return 0.0f;
  if(lay->layer_type == LAYER_STATE::HIDDEN) return 0.0f;
  UNIT_GP_ITR(lay,
              lay->sse += Compute_SSE_ugp(lay, net, gpidx, n_vals);
              );
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
     ((lay->ext_flag & UNIT_STATE::COMP) && lay->HasLayerFlag(LAYER_STATE::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float STATE_CLASS(TwoDValLayerSpec)::Compute_NormErr_ugp
  (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {

  LEABRA_UNGP_STATE* ug = lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  if(nunits < 1) return 0.0f;        // must be at least a few units..
  float rval = 0.0f;
  // for(int k=0;k<twod.n_vals;k++) { // first loop over and find potential target values
  float x_targ = ug->twod.targ_x;
  float y_targ = ug->twod.targ_y;
  // only count if target value is within range -- otherwise considered a non-target
  if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
    // now find minimum dist actual activations
    // float mn_dist = taMath::flt_max;
    // for(int j=0;j<twod.n_vals;j++) {
    float x_act_m = ug->twod.act_m_x;
    float y_act_m = ug->twod.act_m_y;
    float dx = x_targ - x_act_m;
    float dy = y_targ - y_act_m;
    if(fabsf(dx) < us->sse_tol) dx = 0.0f;
    if(fabsf(dy) < us->sse_tol) dy = 0.0f;
    float dist = fabsf(dx) + fabsf(dy); // only diff from sse!
    // if(dist < mn_dist)
    //   mn_dist = dist;
    rval += dist;
  }
  return rval;
}

float STATE_CLASS(TwoDValLayerSpec)::Compute_NormErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!(lay->ext_flag & (UNIT_STATE::COMP_TARG))) return -1.0f; // indicates not applicable
  if(lay->layer_type == LAYER_STATE::HIDDEN) return -1.0f;

  float nerr = 0.0f;
  float ntot = 0;
  if(HasUnitGpInhib(lay)) {
    for(int g=0; g < lay->gp_geom_n; g++) {
      nerr += Compute_NormErr_ugp(lay, net, g);
      ntot += x_range.range + y_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, net, -1);
    ntot += x_range.range + y_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(LAYER_STATE::NO_ADD_SSE) ||
     ((lay->ext_flag & UNIT_STATE::COMP) && lay->HasLayerFlag(LAYER_STATE::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}
