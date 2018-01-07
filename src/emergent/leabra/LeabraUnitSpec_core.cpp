// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in LeabraUnitSpec.cpp, _cpp.cpp, _cuda.cpp

void LEABRA_UNIT_SPEC::Init_Acts(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) {
  inherited::Init_Acts(uv, net, thr_no);
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
  Init_Netins(u, (LEABRA_NETWORK_STATE*)net, thr_no);

  u->ClearExtFlag(UNIT_STATE::COMP_TARG_EXT);
  u->ext = 0.0f;
  u->targ = 0.0f;

  u->act = init.act;
  u->net = init.netin;

  u->act_eq = u->act;
  u->act_nd = u->act_eq;
  u->spike = 0.0f;
  u->spike_isi = 0.0f;
  u->act_q0 = 0.0f;
  u->act_q1 = 0.0f;
  u->act_q2 = 0.0f;
  u->act_q3 = 0.0f;
  u->act_q4 = 0.0f;
  u->act_g = 0.0f;
  u->act_m = 0.0f;
  u->act_p = 0.0f;
  u->act_dif = 0.0f;
  u->net_prv_q = 0.0f;        // note: init acts clears this kind of history..
  u->net_prv_trl = 0.0f;

  u->da = 0.0f;
  u->avg_ss = act_misc.avg_init;
  u->avg_s = act_misc.avg_init;
  u->avg_s_eff = u->avg_s;
  u->avg_m = act_misc.avg_init;
  u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
  // not avg_l
  // not act_avg
  u->margin = 0.0f;
  u->act_raw = 0.0f;
  u->deep_raw = 0.0f;
  u->deep_raw_prv = 0.0f;
  u->deep_mod = 1.0f;
  u->deep_lrn = 1.0f;
  u->deep_ctxt = 0.0f;

  u->thal = 0.0f;
  u->thal_gate = 0.0f;
  u->thal_cnt = -1.0f;
  u->gc_i = 0.0f;
  u->gc_kna_f = 0.0f;
  u->gc_kna_m = 0.0f;
  u->gc_kna_s = 0.0f;
  u->I_net = 0.0f;
  u->v_m = init.v_m;
  u->v_m_eq = u->v_m;
  u->gi_syn = 0.0f;
  u->gi_self = 0.0f;
  u->gi_ex = 0.0f;
  u->syn_tr = 1.0f;
  u->syn_nr = 1.0f;
  u->syn_pr = stp.p0;
  u->syn_kre = 0.0f;
  u->noise = 0.0f;
  u->da_p = 0.0f;
  u->da_n = 0.0f;
  u->ach = 0.0f;
  u->shunt = 0.0f;
  // u->sev = 0.0f; // longer time-course

  // not the scales
  // init netin gets act_sent, net_raw, etc
  u->spk_t = -1;

  STATE_CLASS(CircBufferIndex)::Reset(u->spike_e_st, u->spike_e_len);
  STATE_CLASS(CircBufferIndex)::Reset(u->spike_i_st, u->spike_i_len);

  // if(syn_delay.on) {
  //   u->act_buf->Reset();
  // }
}
  
void LEABRA_UNIT_SPEC::Init_UnitState(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) {
  inherited::Init_UnitState(uv, net, thr_no);
    
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
  u->ext_flag = UNIT_STATE::NO_EXTERNAL;
  u->unit_flag = UNIT_STATE::NO_UNIT_FLAG;
  u->bias_wt = 0.0f;
  u->bias_dwt = 0.0f;
  u->bias_fwt = 0.0f;
  u->bias_swt = 0.0f;
  u->ext_orig = 0.0f;
  u->avg_l = avg_l.init;
  u->act_avg = 0.15f;
  u->deep_mod_net = 0.0f;
  u->deep_raw_net = 0.0f;
  u->sev = 0.0f;
  u->ach = 0.0f;
  u->misc_1 = 0.0f;
  u->misc_2 = 0.0f;
  u->act_sent = 0.0f;
  u->net_raw = 0.0f;
  u->gi_raw = 0.0f;
  u->deep_raw_sent = 0.0f;

  // note: also spike_e_st, spike_e_len, etc
    
  u->act = init.act;
  u->net = init.netin;

  u->act_eq = u->act;
  u->act_nd = u->act_eq;
  u->spike = 0.0f;
  u->spike_isi = 0.0f;
  u->act_q0 = 0.0f;
  u->act_q1 = 0.0f;
  u->act_q2 = 0.0f;
  u->act_q3 = 0.0f;
  u->act_q4 = 0.0f;
  u->act_g = 0.0f;
  u->act_m = 0.0f;
  u->act_p = 0.0f;
  u->act_dif = 0.0f;
  u->net_prv_q = 0.0f;        // note: init acts clears this kind of history..
  u->net_prv_trl = 0.0f;

  u->da = 0.0f;
  u->avg_ss = act_misc.avg_init;
  u->avg_s = act_misc.avg_init;
  u->avg_s_eff = u->avg_s;
  u->avg_m = act_misc.avg_init;
  u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
  // not avg_l
  // not act_avg
  u->margin = 0.0f;
  u->act_raw = 0.0f;
  u->deep_raw = 0.0f;
  u->deep_raw_prv = 0.0f;
  u->deep_mod = 1.0f;
  u->deep_lrn = 1.0f;
  u->deep_ctxt = 0.0f;

  u->thal = 0.0f;
  u->thal_gate = 0.0f;
  u->thal_cnt = -1.0f;
  u->gc_i = 0.0f;
  u->gc_kna_f = 0.0f;
  u->gc_kna_m = 0.0f;
  u->gc_kna_s = 0.0f;
  u->I_net = 0.0f;
  u->v_m = init.v_m;
  u->v_m_eq = u->v_m;
  u->gi_syn = 0.0f;
  u->gi_self = 0.0f;
  u->gi_ex = 0.0f;
  u->syn_tr = 1.0f;
  u->syn_nr = 1.0f;
  u->syn_pr = stp.p0;
  u->syn_kre = 0.0f;
  u->noise = 0.0f;
  u->da_p = 0.0f;
  u->da_n = 0.0f;
  u->ach = 0.0f;
  u->shunt = 0.0f;
  // u->sev = 0.0f; // longer time-course

  // not the scales
  // init netin gets act_sent, net_raw, etc
  u->spk_t = -1;

  if(deep.IsSuper()) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::SUPER);
  }
  if(deep.IsDeep()) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::DEEP);
  }
  if(deep.IsTRC()) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::TRC);
  }
}


void LEABRA_UNIT_SPEC::DecayState(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, float decay) {
  if(decay > 0.0f) {            // no need to reset netin if not decaying at all
    u->act -= decay * (u->act - init.act);
    u->net -= decay * (u->net - init.netin);
    u->act_eq -= decay * (u->act_eq - init.act);
    u->act_nd -= decay * (u->act_nd - init.act);
    u->act_raw -= decay * (u->act_raw - init.act);
    u->gc_i -= decay * u->gc_i;
    u->v_m -= decay * (u->v_m - init.v_m);
    u->v_m_eq -= decay * (u->v_m_eq - init.v_m);
    if(kna_adapt.on) {
      if(kna_adapt.f_on) u->gc_kna_f -= decay * u->gc_kna_f;
      if(kna_adapt.m_on) u->gc_kna_m -= decay * u->gc_kna_m;
      if(kna_adapt.s_on) u->gc_kna_s -= decay * u->gc_kna_s;
    }

    u->gi_syn -= decay * u->gi_syn;
    u->gi_self -= decay * u->gi_self;
    u->gi_ex -= decay * u->gi_ex;

    if(stp.on && (stp.algorithm == STATE_CLASS(ShortPlastSpec)::CYCLES)) {
      u->syn_tr -= decay * (u->syn_tr - 1.0f);
      u->syn_nr -= decay * (u->syn_nr - 1.0f);
      u->syn_pr -= decay * (u->syn_tr - stp.p0);
      u->syn_kre -= decay * u->syn_kre;
    }
  }
  u->da = 0.0f;
  u->I_net = 0.0f;
  // note: for decay = 1, spike buffers are reset
  
  if(decay == 1.0f) {
    if(act_fun == SPIKE) {
      STATE_CLASS(CircBufferIndex)::Reset(u->spike_e_st, u->spike_e_len);
      STATE_CLASS(CircBufferIndex)::Reset(u->spike_i_st, u->spike_i_len);
    }
    // if(syn_delay.on) {
    //   u->act_buf->Reset();
    // }
  }
}

void LEABRA_UNIT_SPEC::Trial_DecayState(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
  LEABRA_LAYER_SPEC_CPP* ls = (LEABRA_LAYER_SPEC_CPP*)lay->GetLayerSpec(net);

  DecayState(u, net, thr_no, ls->decay.trial);
  
  // note: theoretically you could avoid doing Init_Netins if there is no decay between
  // trials, and save some compute time, but the delta-based netin has enough
  // error accumulation that this does NOT work well in practice, so we always do it here
  Init_Netins(u, net, thr_no); 
  u->da_p = 0.0f;
  u->da_n = 0.0f;
  u->ach = 0.0f;
  u->thal = 0.0f;
  //  u->sev = 0.0f; // longer time-course
  // reset all the time vars so it isn't ambiguous as these update
  u->act_q1 = u->act_q2 = u->act_q3 = u->act_q4 = 0.0f;
  u->act_g = 0;
  u->act_m = u->act_p = u->act_dif = 0.0f;
}


void LEABRA_UNIT_SPEC::Compute_NetinScale(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  // this is all receiver-based and done only at beginning of each quarter
  bool plus_phase = (net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE);
  float net_scale = 0.0f;
  float inhib_net_scale = 0.0f;
  float deep_raw_scale = 0.0f;
  float deep_mod_scale = 0.0f;
  
  int n_recv_cons = 0;
  const int nrg = lay->n_recv_prjns;
  for(int g=0; g< nrg; g++) {
    LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(net, g);
    if(!prjn->IsActive(net)) continue;
    LEABRA_CON_SPEC_CPP* cs = prjn->GetConSpec(net);
    LEABRA_LAYER_STATE* from = prjn->GetSendLayer(net);
    LEABRA_UNGP_STATE* fmugps = from->GetLayUnGpState(net);
    LEABRA_UNIT_SPEC_CPP* fmus = from->GetUnitSpec(net);

    float savg = fmugps->acts_p_avg_eff * fmus->act.avg_correct;
    float from_sz = (float)from->n_units;
    float n_cons = (float)prjn->recv_con_stats.max_size;
    prjn->scale_eff = cs->wt_scale.FullScale(savg, from_sz, n_cons);
    float rel_scale = cs->wt_scale.rel;
    n_recv_cons += n_cons;
    
    if(cs->inhib) {
      inhib_net_scale += rel_scale;
    }
    else if(cs->IsDeepRawCon()) {
      deep_raw_scale += rel_scale;
    }
    else if(cs->IsDeepModCon()) {
      deep_mod_scale += rel_scale;
    }
    else {                      // DeepCtxtCon gets included in overall netin!
      net_scale += rel_scale;
    }
  }
  
  // add the bias weight into the netinput, scaled by 1/n
  LEABRA_CON_SPEC_CPP* bs = GetBiasSpec(net);
  if(bs) {
    lay->bias_scale = bs->wt_scale.abs;  // still have absolute scaling if wanted..
    if(n_recv_cons > 0) {
      lay->bias_scale /= (float)n_recv_cons; // one over n scaling for bias!
    }
  }

  // now renormalize, each one separately..
  for(int g=0; g< nrg; g++) {
    LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(net, g);
    if(!prjn->IsActive(net)) continue;
    LEABRA_CON_SPEC_CPP* cs = prjn->GetConSpec(net);
    if(cs->inhib) {
      if(inhib_net_scale > 0.0f)
        prjn->scale_eff /= inhib_net_scale;
    }
    else if(cs->IsDeepRawCon()) {
      if(deep_raw_scale > 0.0f)
        prjn->scale_eff /= deep_raw_scale;
    }
    else if(cs->IsDeepModCon()) {
      if(deep_mod_scale > 0.0f)
        prjn->scale_eff /= deep_mod_scale;
    }
    else {
      if(net_scale > 0.0f)
        prjn->scale_eff /= net_scale;
    }
  }
}

void LEABRA_UNIT_SPEC::Send_DeepCtxtNetin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  float act_ts = u->deep_raw;
  if(act_ts > opt_thresh.send) {
    const int nsg = u->NSendConGps(net); 
    for(int g=0; g< nsg; g++) {
      LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
      if(send_gp->NotActive()) continue;
      // LEABRA_LAYER_STATE* tol =  send_gp->GetRecvLayer(net);
      // if(tol->hard_clamped)      continue;
      LEABRA_CON_SPEC_CPP* cs = send_gp->GetConSpec(net);
      if(cs->IsDeepCtxtCon()) {
        STATE_CLASS(DeepCtxtConSpec)* sp = (STATE_CLASS(DeepCtxtConSpec)*)cs;
        sp->Send_DeepCtxtNetin(send_gp, net, thr_no, act_ts);
      }
    }
  }
}

void LEABRA_UNIT_SPEC::Send_NetinDelta(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  if(net->n_thrs_built == 1) {
    net->send_pct_tot++;        // only safe for non-thread case
  }
  float act_ts = u->act;
  // if(syn_delay.on) { // todo!
  //   if(!u->act_buf)
  //     Init_ActBuff(u);
  //   act_ts = u->act_buf->CircSafeEl(0); // get first logical element..
  // }

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->act_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      if(net->n_thrs_built == 1) {
        net->send_pct_n++;
      }
      const int nsg = u->NSendConGps(net); 
      for(int g=0; g< nsg; g++) {
        LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
        if(send_gp->NotActive()) continue;
        LEABRA_CON_SPEC_CPP* cs = send_gp->GetConSpec(net);
        if(cs->IsDeepModCon()) {
          STATE_CLASS(SendDeepModConSpec)* sp = (STATE_CLASS(SendDeepModConSpec)*)cs;
          sp->Send_DeepModNetDelta(send_gp, net, thr_no, act_delta);
        }
        if(!cs->DoesStdNetin()) continue;
        LEABRA_LAYER_STATE* tol =  send_gp->GetRecvLayer(net);
        if(tol->hard_clamped)      continue;
        if(cs->wt_scale.no_plus_net && net->quarter == 3) {
          // netin typically reset at start of plus phase, so we need to send minus phase
          // activation for first cycle and then stop sending after that
          const int cyc_per_qtr = net->times.quarter;
          int qtr_cyc = net->cycle;
          if(net->cycle > cyc_per_qtr)             // just in case cycle being reset
            qtr_cyc -= net->quarter * cyc_per_qtr; // quarters into this cyc
          if(qtr_cyc > 0) continue;
          float actm_delta = u->act_m - u->act_sent;
          cs->Send_NetinDelta(send_gp, net, thr_no, actm_delta);
          continue;
        }
        cs->Send_NetinDelta(send_gp, net, thr_no, act_delta);
      }
      u->act_sent = act_ts;     // cache the last sent value
    }
  }
  else if(u->act_sent > opt_thresh.send) {
    if(net->n_thrs_built == 1) {
      net->send_pct_n++;
    }
    float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
    const int nsg = u->NSendConGps(net); 
    for(int g=0; g< nsg; g++) {
      LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
      if(send_gp->NotActive()) continue;
      LEABRA_CON_SPEC_CPP* cs = send_gp->GetConSpec(net);
      if(cs->IsDeepModCon()) {
        STATE_CLASS(SendDeepModConSpec)* sp = (STATE_CLASS(SendDeepModConSpec)*)cs;
        sp->Send_DeepModNetDelta(send_gp, net, thr_no, act_delta);
      }
      if(!cs->DoesStdNetin()) continue;
      LEABRA_LAYER_STATE* tol =  send_gp->GetRecvLayer(net);
      if(tol->hard_clamped)        continue;
      if(cs->wt_scale.no_plus_net && net->quarter == 3) {
        // netin typically reset at start of plus phase, so we need to send minus phase
        // activation for first cycle and then stop sending after that
        const int cyc_per_qtr = net->times.quarter;
        int qtr_cyc = net->cycle;
        if(net->cycle > cyc_per_qtr)
          qtr_cyc -= net->quarter * cyc_per_qtr; // quarters into this cyc
        if(qtr_cyc > 0) continue;
        if(u->act_m > opt_thresh.send) continue; // not actually off!
      }
      cs->Send_NetinDelta(send_gp, net, thr_no, act_delta);
    }
    u->act_sent = 0.0f;         // now it effectively sent a 0..
  }
}

void LEABRA_UNIT_SPEC::Compute_NetinRaw(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  // this integrates from SendDelta into net_raw and gi_syn
  const int nt = net->n_thrs_built;
  const int flat_idx = u->flat_idx;
  float net_delta = 0.0f;
  float gi_delta = 0.0f;
  if(net->NetinPerPrjn()) {
    const int nrg = u->NRecvConGps(net); 
    for(int g=0; g< nrg; g++) {
      LEABRA_CON_STATE* recv_gp = u->RecvConState(net, g);
      if(recv_gp->NotActive()) continue;
      float g_net_delta = 0.0f;
      for(int j=0;j<nt;j++) {
        float& ndval = net->ThrSendNetinTmpPerPrjn(j, g)[flat_idx]; 
	g_net_delta += ndval;
      }
      LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
      recv_gp->net_raw += g_net_delta; // note: direct assignment to raw, no time integ
      if(cs->inhib) {
        gi_delta += g_net_delta;
      }
      else {
        net_delta += g_net_delta;
      }
    }
    u->net_raw += net_delta;
  }
  else {
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendNetinTmp(j)[flat_idx];
      net_delta += ndval;
    }
    u->net_raw += net_delta;
  }

  if(net->net_misc.inhib_cons) {
    u->gi_raw += gi_delta;
    if(act_fun == SPIKE) {
      u->gi_syn = fmaxf(u->gi_syn, 0.0f);
      Compute_NetinInteg_Spike_i(u, net, thr_no);
    }
    else {
      u->gi_syn += dt.integ * dt.net_dt * (u->gi_raw - u->gi_syn);
      u->gi_syn = fmaxf(u->gi_syn, 0.0f); // negative netin doesn't make any sense
    }
  }
  else {
    // clear so automatic inhibition can add to these values!
    // this is an unnec memory write!
    // u->gi_syn = 0.0f;
    // u->gi_raw = 0.0f;
  }
}

void LEABRA_UNIT_SPEC::Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  if(net->deep.mod_net) {       // if anyone's doing it, we need to integrate!
    DeepModNetin_Integ(u, net, thr_no);
  }
  
  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
  if(lay->hard_clamped) {
    return;
  }

  Compute_NetinRaw(u, net, thr_no);
  // u->net_raw and u->gi_syn now have proper values integrated from deltas

  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
  float net_syn = act_misc.net_gain * u->net_raw;
  float net_ex = 0.0f;
  if(deep.IsTRC() && Quarter_DeepRawNow(net->quarter)) {
    if(lgpd->am_deep_raw_net.max > 0.1f) { // have to get some input to clamp
      if(trc.p_only_m) {
        LEABRA_UNGP_STATE* gpd = u->GetOwnUnGp(net);
        if(gpd->acts_prvq.max > 0.1f) {
          // only activate if we got prior and current activation
          net_syn = trc.TRCClampNet(u->deep_raw_net, net_syn); // u->net_prv_q); 
        }
        else {
          net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);
        }
      }
      else {                       // always do it
        net_syn = trc.TRCClampNet(u->deep_raw_net, net_syn); // u->net_prv_q); 
      }
    }
    else {
      net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);
    }
  }
  else {
    net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);  // this could modify net_syn if it wants..
  }
  
  float net_tot = net_syn + net_ex;

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = fmaxf(net_tot, 0.0f); // store directly for integration
    Compute_NetinInteg_Spike_e(u, net, thr_no);
  }
  else {
    u->net += dt.integ * dt.net_dt * (net_tot - u->net);
    if(u->net < 0.0f) u->net = 0.0f; // negative netin doesn't make any sense
  }

  // first place noise is required -- generate here!
  if(noise_type.type != STATE_CLASS(LeabraNoiseSpec)::NO_NOISE && !noise_type.trial_fixed &&
     (noise.type != STATE_CLASS(Random)::NONE)) {
    u->noise = noise.Gen(thr_no);
  }
  
  // add after all the other stuff is done..
  if((noise_type.type == STATE_CLASS(LeabraNoiseSpec)::NETIN_NOISE)
     && (noise.type != STATE_CLASS(Random)::NONE)) {
    u->net += u->noise;
  }
  else if((noise_type.type == STATE_CLASS(LeabraNoiseSpec)::NET_MULT_NOISE)
          && (noise.type != STATE_CLASS(Random)::NONE)) {
    u->net *= u->noise;
  }
}

float LEABRA_UNIT_SPEC::Compute_NetinExtras
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, float& net_syn) {
  
  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
  LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(net);

  float net_ex = init.netin;
  LEABRA_CON_SPEC_CPP* bs = GetBiasSpec(net);
  if(bs) {
    net_ex += lay->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UNIT_STATE::EXT)) {
    if(ls->clamp.avg) {
      net_syn = ls->clamp.ClampAvgNetin(u->ext, net_syn);
    }
    else {
      net_ex += u->ext * ls->clamp.gain;
    }
  }
  if(deep.ApplyDeepCtxt()) {
    net_ex += u->deep_ctxt;
  }
  if(da_mod.DoDaModNetin()) {
    net_ex += Compute_DaModNetin(u, net, thr_no, net_syn);
  }
  return net_ex;
}

void LEABRA_UNIT_SPEC::Compute_NetinInteg_Spike_e(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                                int thr_no) {
  // netin gets added at the end of the spike_buf -- 0 time is the end
  STATE_CLASS(CircBufferIndex)::CircAddShift_float
    (u->net, u->spike_e_buf, u->spike_e_st, u->spike_e_len, spike.window);
  int mx = u->spike_e_len;
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net_dt
    for(int t=0;t<mx;t++) {
      sum += STATE_CLASS(CircBufferIndex)::CircEl_float(t, u->spike_e_buf, u->spike_e_st, spike.window);
    }
    sum /= (float)spike.window; // normalize over window -- todo: what if it is smaller!?
    u->net += dt.integ * (spike.gg_decay * sum - (u->net * spike.oneo_decay));
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = STATE_CLASS(CircBufferIndex)::CircEl_float(t, u->spike_e_buf, u->spike_e_st,
                                                               spike.window);
      if(spkin > 0.0f) {
        sum += spkin * spike.ComputeAlpha(mx-t-1);
      }
    }
    // from compute_netinavg
    u->net += dt.integ * dt.net_dt * (sum - u->net);
  }
  u->net = fmaxf(u->net, 0.0f); // negative netin doesn't make any sense
}

void LEABRA_UNIT_SPEC::Compute_NetinInteg_Spike_i(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                                int thr_no) {
  // netin gets added at the end of the spike_i_buf -- 0 time is the end
  STATE_CLASS(CircBufferIndex)::CircAddShift_float
    (u->gi_raw, u->spike_i_buf, u->spike_i_st, u->spike_i_len, spike.window);
  int mx = u->spike_i_len;
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net_dt
    for(int t=0;t<mx;t++) {
      sum += STATE_CLASS(CircBufferIndex)::CircEl_float(t, u->spike_i_buf, u->spike_i_st, spike.window);
    }
    sum /= (float)spike.window; // normalize over window
    u->gi_syn += dt.integ * (spike.gg_decay * sum - (u->gi_syn * spike.oneo_decay));
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = STATE_CLASS(CircBufferIndex)::CircEl_float(t, u->spike_i_buf, u->spike_i_st,
                                                               spike.window);
      if(spkin > 0.0f) {
        sum += spkin * spike.ComputeAlpha(mx-t-1);
      }
    }
    u->gi_syn += dt.integ * dt.net_dt * (sum - u->gi_syn);
  }
  u->gi_syn = fmaxf(u->gi_syn, 0.0f); // negative netin doesn't make any sense
}


void LEABRA_UNIT_SPEC::Compute_ApplyInhib
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, LEABRA_LAYER_STATE* lay, float ival) {
  LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(net);
  Compute_SelfInhib_impl(u, ls->inhib_misc.self_fb, ls->inhib_misc.self_dt);
  float gi_ex = 0.0f;
  if(ls->del_inhib.on) {
    gi_ex = ls->del_inhib.DelInhib(u->net_prv_trl, u->net_prv_q);
  }
  Compute_ApplyInhib_impl(u, ival, gi_ex, lay->adapt_gi);
}


void LEABRA_UNIT_SPEC::Compute_Vm(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  bool updt_spk_vm = true;
  if(spike_misc.t_r > 0 && u->spk_t > 0) {
    int spkdel = net->tot_cycle - u->spk_t;
    if(spkdel >= 0 && spkdel <= spike_misc.t_r)
      updt_spk_vm = false;    // don't update the spiking vm during refract
  }

  if(net->cycle < dt.fast_cyc) {
    // directly go to equilibrium value
    float new_v_m = Compute_EqVm(u);
    float I_net = new_v_m - u->v_m; // time integrate: not really I_net but hey
    u->v_m += I_net;
    u->v_m_eq = u->v_m;
    u->I_net = I_net;
  }
  else {
    const float net_eff = u->net * g_bar.e;
    const float gc_i = u->gc_i * g_bar.i;
    const float gc_k = g_bar.k * (u->gc_kna_f + u->gc_kna_m + u->gc_kna_s);
    if(updt_spk_vm) { // first compute v_m, using midpoint method:
      float v_m = u->v_m;
      float I_net = 0.0f;
      for(int ti=0; ti < dt.vm_cyc; ti++) {
        float v_m_eff = v_m;
        // midpoint method: take a half-step:
        float I_net_1 = Compute_INet_impl(u, v_m_eff, net_eff, gc_i, gc_k);
        v_m_eff += .5f * dt.integ * dt.vm_dt_cyc * I_net_1; // go half way
        float I_net_2 = Compute_INet_impl(u, v_m_eff, net_eff, gc_i, gc_k);
        // add spike current if relevant
        if((act_fun == SPIKE) && spike_misc.ex) {
          I_net_2 += g_bar.l * spike_misc.exp_slope *
            expf((v_m_eff - act.thr) / spike_misc.exp_slope); // todo: exp_fast
        }
        v_m += dt.integ * dt.vm_dt_cyc * I_net_2;
        I_net += I_net_2;
      }
      u->v_m = v_m;
      u->I_net = I_net;
    }
    // always compute v_m_eq with simple integration -- used for rate code subthreshold
    float I_net_r = Compute_INet_impl(u, u->v_m_eq, net_eff, gc_i, gc_k);
    u->v_m_eq += dt.integ * dt.vm_dt * I_net_r;
  }

  if((noise_type.type == STATE_CLASS(LeabraNoiseSpec)::VM_NOISE) &&
     (noise.type != STATE_CLASS(Random)::NONE) && (net->cycle >= 0)) {
    float ns = u->noise;
    u->v_m += ns;
    u->v_m_eq += ns;
  }

  if(u->v_m < vm_range.min) u->v_m = vm_range.min;
  if(u->v_m > vm_range.max) u->v_m = vm_range.max;
  if(u->v_m_eq < vm_range.min) u->v_m_eq = vm_range.min;
  if(u->v_m_eq > vm_range.max) u->v_m_eq = vm_range.max;
}

void LEABRA_UNIT_SPEC::Send_DeepRawNetin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawNow(net->quarter)) return;

  float act_ts = u->deep_raw; // note: no delay for deep

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->deep_raw_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      const int nsg = u->NSendConGps(net); 
      for(int g=0; g< nsg; g++) {
        LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
        if(send_gp->NotActive()) continue;
        // LEABRA_LAYER_STATE* tol =  send_gp->GetRecvLayer(net);
        // if(tol->hard_clamped)      continue;
        LEABRA_CON_SPEC_CPP* cs = send_gp->GetConSpec(net);
        if(cs->IsDeepRawCon()) {
          STATE_CLASS(SendDeepRawConSpec)* sp = (STATE_CLASS(SendDeepRawConSpec)*)cs;
          sp->Send_DeepRawNetDelta(send_gp, net, thr_no, act_delta);
        }
      }
      u->deep_raw_sent = act_ts;     // cache the last sent value
    }
  }
  else if(u->deep_raw_sent > opt_thresh.send) {
    float act_delta = - u->deep_raw_sent; // un-send the last above-threshold activation to get back to 0
    const int nsg = u->NSendConGps(net); 
    for(int g=0; g< nsg; g++) {
      LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
      if(send_gp->NotActive()) continue;
      // LEABRA_LAYER_STATE* tol =  send_gp->GetRecvLayer(net);
      // if(tol->hard_clamped)        continue;
      LEABRA_CON_SPEC_CPP* cs = send_gp->GetConSpec(net);
      if(cs->IsDeepRawCon()) {
        STATE_CLASS(SendDeepRawConSpec)* sp = (STATE_CLASS(SendDeepRawConSpec)*)cs;
        sp->Send_DeepRawNetDelta(send_gp, net, thr_no, act_delta);
      }
    }
    u->deep_raw_sent = 0.0f;         // now it effectively sent a 0..
  }
}


void LEABRA_UNIT_SPEC::Compute_ActFun_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  float new_act;
  if(da_mod.DoDaModGain()) {
    float gain_eff = da_mod.DaModGain(u->da_p, act.gain,
                                      net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE);
    if(u->act_raw < act.vm_act_thr && u->v_m_eq <= act.thr) {
      new_act = act.NoisyXX1_gain(u->v_m_eq - act.thr, gain_eff);
    }
    else {
      float g_e_thr = Compute_EThresh(u);
      new_act = act.NoisyXX1_gain((u->net * g_bar.e) - g_e_thr, gain_eff);
    }
  }
  else {
    if(u->act_raw < act.vm_act_thr && u->v_m_eq <= act.thr) {
      // note: this is quite important -- if you directly use the gelin
      // the whole time, then units are active right away -- need v_m_eq dynamics to
      // drive subthreshold activation behavior
      new_act = Compute_ActFun_Rate_fun(u->v_m_eq - act.thr);
    }
    else {
      float g_e_thr = Compute_EThresh(u);
      new_act = Compute_ActFun_Rate_fun((u->net * g_bar.e) - g_e_thr);
    }
  }
  bool do_kna_invert =
    (kna_adapt.on && kna_misc.invert_nd &&
     !(kna_misc.no_targ && (deep.IsTRC() || u->HasExtFlag(LEABRA_UNIT_STATE::TARG))));
  float cur_act;
  if(do_kna_invert) {
    cur_act = u->act_eq;
  }
  else {
    cur_act = u->act_nd;
  }
    
  if(net->cycle >= dt.fast_cyc) {
    new_act = cur_act + dt.integ * dt.vm_dt * (new_act - cur_act); // time integral with dt.vm_dt  -- use nd to avoid synd problems
  }
  if(deep.IsTRC() && Quarter_DeepRawNow(net->quarter) && trc.clamp_net) {
    new_act = u->net;
  }

  u->da = new_act - cur_act;
  if((noise_type.type == STATE_CLASS(LeabraNoiseSpec)::ACT_NOISE) &&
     (noise.type != STATE_CLASS(Random)::NONE) && (net->cycle >= 0)) {
    new_act += u->noise;
  }

  u->act_raw = new_act;
  if(deep.ApplyDeepMod()) { // apply attention directly to act
    new_act *= u->deep_mod;
  }

  if(do_kna_invert) {
    u->act_eq = act_range.Clip(new_act);
    if(stp.on) {                   // short term plasticity, depression
      u->act = u->act_eq * u->syn_tr; // overall probability of transmission
    }
    else {
      u->act = u->act_eq;
    }
    u->act_nd = kna_misc.Compute_ActNd(u->act_eq, u->gc_kna_f, u->gc_kna_m, u->gc_kna_s);
  }
  else {
    u->act_nd = act_range.Clip(new_act);
    if(stp.on) {                   // short term plasticity, depression
      u->act = u->act_nd * u->syn_tr; // overall probability of transmission
    }
    else {
      u->act = u->act_nd;
    }
    u->act_eq = u->act;           // for rate code, eq == act
  }

  // we now use the exact same vm-based dynamics as in SPIKE model, for full consistency!
  // note that v_m_eq is NOT reset here:
  if(u->v_m > spike_misc.eff_spk_thr) {
    Compute_ActFun_Spiked(u, net, thr_no);
  }
  else {
    Compute_ActFun_NotSpiked(u, net, thr_no);
  }
}
