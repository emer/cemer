// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in LeabraLayerSpec.cpp, _cpp.cpp, _cuda.cpp


void LEABRA_LAYER_SPEC::DecayState(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, float decay_val) {
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  for(int ui = 0; ui < lay->n_units; ui++) {
    LEABRA_UNIT_STATE* u = lay->GetUnitState(net, ui);
    if(u->lesioned()) continue;
    us->DecayState(u, net, u->thread_no, decay_val);
  }
}

void LEABRA_LAYER_SPEC::Trial_Init_Specs(LEABRA_NETWORK_STATE* net) {
  if(lay_gp_inhib.on) {
    net->net_misc.lay_gp_inhib = true;
  }
  if(decay.trial > 0.0f) {
    net->net_misc.trial_decay = true;
  }
}


float LEABRA_LAYER_SPEC::Compute_AvgExt(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  float avg_ext = 0.0f;
  int avg_n = 0;
  for(int ui = 0; ui < lay->n_units; ui++) {
    LEABRA_UNIT_STATE* u = lay->GetUnitState(net, ui);
    if(u->lesioned()) continue;
    if(lay->HasExtFlag(UnitState_cpp::TARG)) { // targ comes first b/c not copied to ext at this point yet!
      avg_ext += u->targ;
    }
    else if(lay->HasExtFlag(UnitState_cpp::EXT)) {
      avg_ext += u->ext;
    }
    avg_n++;
  }
  if(avg_n > 0) {
    avg_ext /= (float)avg_n;
  }
  return avg_ext;
}

void LEABRA_LAYER_SPEC::Init_AdaptParams(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
  lay->adapt_gi = 1.0f;
  lay->lrate_mod = lay_lrate;
  lay->margin.low_thr = margin.low_thr;
  lay->margin.med_thr = margin.med_thr;
  lay->margin.hi_thr = margin.hi_thr;
  float eff_p_avg = lgpd->acts_p_avg / margin.avg_act;
  lay->margin.low_avg = eff_p_avg;
  lay->margin.med_avg = margin.MedTarg(eff_p_avg);
  lay->margin.hi_avg = margin.HiTarg(eff_p_avg);
  lay->laygp_i_val.InitVals(); lay->laygp_netin.InitVals(); lay->laygp_acts_eq.InitVals();
  lay->mod_avg_l_lrn = 0.0f;    // start at 0 -- safer than alternative -- will get updated soon enough
}

void LEABRA_LAYER_SPEC::Trial_Init_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
  lay->deep_lrate_mod = us->deep.ApplyDeepMod();
  lgpd->acts_q0 = lgpd->acts_p;
  for(int g=0; g < lay->n_ungps; g++) {
    LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
    gpd->acts_q0 = gpd->acts_p;
  }
  
  if(decay.trial > 0.0f) {
    lay->laygp_i_val.ffi -= decay.trial * lay->laygp_i_val.ffi;
    lay->laygp_i_val.fbi -= decay.trial * lay->laygp_i_val.fbi;
    lgpd->i_val.ffi -= decay.trial * lgpd->i_val.ffi;
    lgpd->i_val.fbi -= decay.trial * lgpd->i_val.fbi;
    for(int g=0; g < lay->n_ungps; g++) {
      LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
      gpd->i_val.ffi -= decay.trial * gpd->i_val.ffi;
      gpd->i_val.fbi -= decay.trial * gpd->i_val.fbi;
    }
  }
  int eff_trial_interval = inhib_adapt.trial_interval;
  if(net->dmem_nprocs > 1) {
    eff_trial_interval /= net->dmem_nprocs;
  }
  if(inhib_adapt.on && ((net->total_trials+1) % eff_trial_interval == 0)) {
    if((lay->layer_type == LAYER_STATE::TARGET) || us->deep.IsTRC()) {
      // use plus-phase running average as the target!
      inhib_adapt.AdaptInhib(lay->adapt_gi, lgpd->acts_p_avg, lgpd->acts_m_avg);
    }
    else {
      inhib_adapt.AdaptInhib(lay->adapt_gi, avg_act.targ_init, lgpd->acts_m_avg);
    }
  }
}


float LEABRA_LAYER_SPEC::Compute_MaxErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
  lay->max_err = 0.0f;
  if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG)) return 0.0f;
  if(HasUnitGpInhib(lay)) {
    LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
    int merr_sum = 0;
    for(int g=0; g < lay->n_ungps; g++) {
      if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_LeabraTickDecodeUnitSpec) {
        if(g != net->tick) continue; // only operate on the one tick
      }
      LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
      bool max_err = true;
      if(gpd->acts_m.max_i >= 0) {
        LEABRA_UNIT_STATE* un = net->GetUnitState(gpd->acts_m.max_i);
        max_err = (un->targ < 0.001f); // close enough to zero -- tickdecode may be low
      }
      gpd->max_err = (float)max_err;
      merr_sum += (int)max_err;
    }
    lay->max_err = (merr_sum > 0);
  }
  else {
    bool max_err = true;
    if(lgpd->acts_m.max_i >= 0) {
      LEABRA_UNIT_STATE* un = net->GetUnitState(lgpd->acts_m.max_i);
      max_err = (un->targ < 0.1f);
    }
    lay->max_err = (float)max_err;
  }
  return lay->max_err;
}

float LEABRA_LAYER_SPEC::Compute_CosDiff(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net)  {
  lay->cos_diff = 0.0f;
  float cosv = 0.0f;
  float ssm = 0.0f;
  float sst = 0.0f;

  const int li = lay->layer_idx;
  for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
    // integrate over thread raw data
    float& lcosv = net->ThrLayStats(thr_no, li, 0, LEABRA_NETWORK_STATE::COSDIFF);
    float& lssm = net->ThrLayStats(thr_no, li, 1, LEABRA_NETWORK_STATE::COSDIFF);
    float& lsst = net->ThrLayStats(thr_no, li, 2, LEABRA_NETWORK_STATE::COSDIFF);

    cosv += lcosv;
    ssm += lssm;
    sst += lsst;
  }
  float dist = sqrtf(ssm * sst);
  if(dist != 0.0f)
    cosv /= dist;
  lay->cos_diff = cosv;

  cos_diff.UpdtDiffAvgVar(lay->cos_diff_avg, lay->cos_diff_var, lay->cos_diff);
  lay->lrate_mod = lay_lrate;

  if(cos_diff.lrate_mod && !cos_diff.lrmod_fm_trc) {
    lay->lrate_mod *= cos_diff.CosDiffLrateMod(lay->cos_diff, lay->cos_diff_avg,
                                               lay->cos_diff_var);
    if(cos_diff.set_net_unlrn && lay->lrate_mod == 0.0f) {
      net->unlearnable_trial = true;
    }
  }
  
  LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
  if((lay->layer_type != LAYER_STATE::HIDDEN) || us->deep.IsTRC()) {
    lay->cos_diff_avg_lrn = 0.0f; // no bcm for TARGET layers; irrelevant for INPUT
    lay->mod_avg_l_lrn = 0.0f;
  }
  else {
    lay->cos_diff_avg_lrn = 1.0f - lay->cos_diff_avg;
    lay->mod_avg_l_lrn = us->avg_l_mod.GetMod(lay->cos_diff_avg_lrn, net->avg_cos_err.avg);
  }

  lay->avg_cos_diff.Increment(lay->cos_diff);
  
  return cosv;
}

void LEABRA_LAYER_SPEC::Compute_CosDiff_post(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  if(cos_diff.lrate_mod && cos_diff.lrmod_fm_trc) {
    for(int i=0;i<lay->n_recv_prjns;i++) {
      LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(net, i);
      if(prjn->NotActive(net)) continue;
      LEABRA_LAYER_STATE* from = prjn->GetSendLayer(net);
      LEABRA_UNIT_SPEC_CPP* frus = from->GetUnitSpec(net);
      if(frus->deep.IsTRC()) {
        LEABRA_LAYER_SPEC_CPP* frls = from->GetLayerSpec(net);
        lay->lrate_mod *= (from->lrate_mod / frls->lay_lrate); // deconfound sender lrate
      }
    }
  }
}

void LEABRA_LAYER_SPEC::Compute_MaxDwts(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  for(int i = 0; i < lay->n_send_prjns; i++) {
    LEABRA_PRJN_STATE* prjn = lay->GetSendPrjnState(net, i);
    if(prjn->NotActive(net)) continue;

    LEABRA_CON_SPEC_CPP* cs = prjn->GetConSpec(net);

    prjn->am_err_dwt.InitVals();
    prjn->am_bcm_dwt.InitVals();
    prjn->am_dwt.InitVals();

    for(int ui = 0; ui < lay->n_units; ui++) {
      LEABRA_UNIT_STATE* u = lay->GetUnitState(net, ui);
      if(u->lesioned()) continue;
      LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)u->SendConStatePrjn(net, prjn);
      if(!cg) continue;
      const int flat_idx = u->flat_idx;
      prjn->am_err_dwt.UpdtSepAvgMax(cg->err_dwt_avg, cg->err_dwt_max, flat_idx);
      prjn->am_bcm_dwt.UpdtSepAvgMax(cg->bcm_dwt_avg, cg->bcm_dwt_max, flat_idx);
      prjn->am_dwt.UpdtSepAvgMax(cg->dwt_avg, cg->dwt_max, flat_idx);
    }

    prjn->am_err_dwt.CalcAvg();
    prjn->am_bcm_dwt.CalcAvg();
    prjn->am_dwt.CalcAvg();

    cs->dwt_norm.UpdateAvg(prjn->err_dwt_max_avg, prjn->am_err_dwt.max);
    cs->dwt_norm.UpdateAvg(prjn->bcm_dwt_max_avg, prjn->am_bcm_dwt.max);
    cs->dwt_norm.UpdateAvg(prjn->dwt_max_avg, prjn->am_dwt.max);
  }
}

void LEABRA_LAYER_SPEC::Compute_AbsRelNetin(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
  LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);

  Compute_MaxDwts(lay, net);
  
  if(lgpd->netin.max < 0.01f) return; // not getting enough activation to count!

  // layer is automatic
  lay->avg_netin_sum.avg += lgpd->netin_m.avg;
  lay->avg_netin_sum.max += lgpd->netin_m.max;
  lay->avg_netin_n++;

  if(lay->prjn_start_idx < 0 || lay->n_recv_prjns == 0) return;
  
  // but projection level is not
  if(net->NetinPerPrjn() || net->rel_netin.ComputeNow(net->epoch, net->trial)) {
    float sum_net = 0.0f;
    for(int i = 0; i < lay->n_recv_prjns; i++) {
      LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(net, i);
      if(prjn->NotActive(net)) continue;
      LEABRA_CON_SPEC_CPP* cs = prjn->GetConSpec(net);
      prjn->netin_avg = 0.0f;
      int netin_avg_n = 0;
      for(int ui = 0; ui < lay->n_units; ui++) {
        LEABRA_UNIT_STATE* u = lay->GetUnitState(net, ui);
        if(u->lesioned()) continue;
        if(u->act_eq < us->opt_thresh.send) continue; // ignore if not above sending thr
        LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)u->RecvConStatePrjn(net, prjn);
        if(!cg) continue;
        float netin;
        if(net->NetinPerPrjn()) {
          netin = cg->net_raw;
        }
        else {          // otherwise have to compute it
          netin = cs->Compute_Netin(cg, net, u->thread_no);
        }
        cg->net = netin;
        prjn->netin_avg += netin;
        netin_avg_n++;
      }
      if(netin_avg_n > 0)
        prjn->netin_avg /= (float)netin_avg_n;
      sum_net += prjn->netin_avg;
    }

    for(int i = 0; i < lay->n_recv_prjns; i++) {
      LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(net, i);
      if(prjn->NotActive(net)) continue;
      if(sum_net > 0.0f)
        prjn->netin_rel = prjn->netin_avg / sum_net;
      // increment epoch-level
      prjn->avg_netin_avg_sum += prjn->netin_avg;
      prjn->avg_netin_rel_sum += prjn->netin_rel;
      prjn->avg_netin_n++;
    }
  }
}

void LEABRA_LAYER_SPEC::Compute_AvgAbsRelNetin(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  if(lay->avg_netin_n > 0) {
    lay->avg_netin.avg = lay->avg_netin_sum.avg / (float)lay->avg_netin_n;
    lay->avg_netin.max = lay->avg_netin_sum.max / (float)lay->avg_netin_n;
  }
  lay->avg_netin_sum.avg = 0.0f;
  lay->avg_netin_sum.max = 0.0f;

  lay->avg_netin_n = 0;
  for(int i = 0; i < lay->n_recv_prjns; i++) {
    LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(net, i);
    if(prjn->NotActive(net)) continue;
    if(prjn->avg_netin_n > 0) {
      prjn->avg_netin_avg = prjn->avg_netin_avg_sum / (float)prjn->avg_netin_n;
      prjn->avg_netin_rel = prjn->avg_netin_rel_sum / (float)prjn->avg_netin_n;
    }
    prjn->avg_netin_n = 0;
    prjn->avg_netin_avg_sum = 0.0f;
    prjn->avg_netin_rel_sum = 0.0f;
  }
}

void LEABRA_LAYER_SPEC::ClearDeepActs(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(net);
  for(int ui = 0; ui < lay->n_units; ui++) {
    LEABRA_UNIT_STATE* u = lay->GetUnitState(net, ui);
    if(u->lesioned()) continue;
    us->ClearDeepActs(u, net, u->thread_no);
  }
}


