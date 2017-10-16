// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in LeabraUnitSpec.cpp, _cpp.cpp, _cuda.cpp

void LEABRA_UNIT_SPEC::Trial_DecayState(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
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


void LEABRA_UNIT_SPEC::Compute_NetinScale(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  // this is all receiver-based and done only at beginning of each quarter
  bool plus_phase = (net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE);
  float net_scale = 0.0f;
  float inhib_net_scale = 0.0f;
  float deep_raw_scale = 0.0f;
  float deep_mod_scale = 0.0f;
  
  // important: count all projections so it is uniform across all units
  // in the layer!  if a unit does not have a connection in a given projection,
  // then it counts as a zero, but it counts in overall normalization!
  const int nrg = u->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    // todo: why!!!???
    if(!recv_gp->PrjnIsActive(net)) continue; // key!! just check for prjn, not con group!
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    LEABRA_LAYER_STATE* from = (LEABRA_LAYER_STATE*)recv_gp->GetPrjnRecvLayer(net);
    LEABRA_UNGP_STATE* fmugps = (LEABRA_UNGP_STATE*)from->GetLayUnGpState(net);

    float savg = fmugps->acts_p_avg_eff;
    float from_sz = (float)from->n_units;
    float n_cons = (float)recv_gp->size;
    recv_gp->scale_eff = cs->wt_scale.FullScale(savg, from_sz, n_cons);
    float rel_scale = cs->wt_scale.rel;
    
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
  LEABRA_CON_SPEC* bs = (LEABRA_CON_SPEC*)GetBiasSpec(net);
  if(bs) {
    u->bias_scale = bs->wt_scale.abs;  // still have absolute scaling if wanted..
    if(nrg > 0) {
      u->bias_scale /= (float)nrg; // one over n scaling for bias!
    }
  }

  // now renormalize, each one separately..
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(!recv_gp->PrjnIsActive(net)) continue; // key!! just check for prjn, not con group!
    LEABRA_LAYER_STATE* from = (LEABRA_LAYER_STATE*)recv_gp->GetPrjnRecvLayer(net);
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    if(cs->inhib) {
      if(inhib_net_scale > 0.0f)
        recv_gp->scale_eff /= inhib_net_scale;
    }
    else if(cs->IsDeepRawCon()) {
      if(deep_raw_scale > 0.0f)
        recv_gp->scale_eff /= deep_raw_scale;
    }
    else if(cs->IsDeepModCon()) {
      if(deep_mod_scale > 0.0f)
        recv_gp->scale_eff /= deep_mod_scale;
    }
    else {
      if(net_scale > 0.0f)
        recv_gp->scale_eff /= net_scale;
    }
  }
}

void LEABRA_UNIT_SPEC::Send_DeepCtxtNetin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  float act_ts = u->deep_raw;
  if(act_ts > opt_thresh.send) {
    const int nsg = u->NSendConGps(net); 
    for(int g=0; g< nsg; g++) {
      LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
      if(send_gp->NotActive()) continue;
      // LEABRA_LAYER_STATE* tol = (LEABRA_LAYER_STATE*) send_gp->prjn->layer;
      // if(tol->hard_clamped)      continue;
      LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)send_gp->GetConSpec(net);
      if(cs->IsDeepCtxtCon()) {
        // DeepCtxtConSpec* sp = (DeepCtxtConSpec*)cs;
        // sp->Send_DeepCtxtNetin(send_gp, net, thr_no, act_ts);
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
        LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
        if(send_gp->NotActive()) continue;
        LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)send_gp->GetConSpec(net);
        if(cs->IsDeepModCon()) {
          // SendDeepModConSpec* sp = (SendDeepModConSpec*)cs;
          // sp->Send_DeepModNetDelta(send_gp, net, thr_no, act_delta);
        }
        if(!cs->DoesStdNetin()) continue;
        LEABRA_LAYER_STATE* tol = (LEABRA_LAYER_STATE*) send_gp->GetPrjnRecvLayer(net);
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
      LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
      if(send_gp->NotActive()) continue;
      LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)send_gp->GetConSpec(net);
      if(cs->IsDeepModCon()) {
        // SendDeepModConSpec* sp = (SendDeepModConSpec*)cs;
        // sp->Send_DeepModNetDelta(send_gp, net, thr_no, act_delta);
      }
      if(!cs->DoesStdNetin()) continue;
      LEABRA_LAYER_STATE* tol = (LEABRA_LAYER_STATE*) send_gp->GetPrjnRecvLayer(net);
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
  int nt = net->n_thrs_built;
  const int flat_idx = u->flat_idx;
  float net_delta = 0.0f;
  float gi_delta = 0.0f;
  if(net->NetinPerPrjn()) {
    const int nrg = u->NRecvConGps(net); 
    for(int g=0; g< nrg; g++) {
      LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
      if(recv_gp->NotActive()) continue;
      float g_net_delta = 0.0f;
      for(int j=0;j<nt;j++) {
        float& ndval = net->ThrSendNetinTmpPerPrjn(j, g)[flat_idx]; 
	g_net_delta += ndval;
      }
      LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
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
  
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
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
        LEABRA_UNGP_STATE* gpd = (LEABRA_UNGP_STATE*)u->GetOwnUnGp(net);
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
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
  LEABRA_LAYER_SPEC* ls = (LEABRA_LAYER_SPEC*)lay->GetLayerSpec(net);

  float net_ex = init.netin;
  LEABRA_CON_SPEC* bs = (LEABRA_CON_SPEC*)GetBiasSpec(net);
  if(bs) {
    net_ex += u->bias_scale * u->bias_wt;
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
  if(da_mod.on) {
    net_ex += Compute_DaModNetin(u, net, thr_no, net_syn);
  }
  return net_ex;
}

void LEABRA_UNIT_SPEC::Compute_NetinInteg_Spike_e(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net,
                                                int thr_no) {
  // netin gets added at the end of the spike_buf -- 0 time is the end
  // STATE_CLASS(CircBufferIndex)::CircAddShift_float
  //   (u->net, u->spike_e_buf, u->spike_e_st, u->spike_e_len, spike.window);
  int mx = u->spike_e_len;
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net_dt
    for(int t=0;t<mx;t++) {
      // sum += STATE_CLASS(CircBufferIndex)::CircEl_float(t, u->spike_e_buf, u->spike_e_st, spike.window);
    }
    sum /= (float)spike.window; // normalize over window -- todo: what if it is smaller!?
    u->net += dt.integ * (spike.gg_decay * sum - (u->net * spike.oneo_decay));
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = 0.0f;
      // float spkin = STATE_CLASS(CircBufferIndex)::CircEl_float(t, u->spike_e_buf, u->spike_e_st,
      //                                             spike.window);
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
  // STATE_CLASS(CircBufferIndex)::CircAddShift_float
  //   (u->gi_raw, u->spike_i_buf, u->spike_i_st, u->spike_i_len, spike.window);
  int mx = u->spike_i_len;
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net_dt
    for(int t=0;t<mx;t++) {
      // sum += STATE_CLASS(CircBufferIndex)::CircEl_float(t, u->spike_i_buf, u->spike_i_st, spike.window);
    }
    sum /= (float)spike.window; // normalize over window
    u->gi_syn += dt.integ * (spike.gg_decay * sum - (u->gi_syn * spike.oneo_decay));
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = 0.0f;
      // float spkin = STATE_CLASS(CircBufferIndex)::CircEl_float(t, u->spike_i_buf, u->spike_i_st,
      //                                             spike.window);
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
  LEABRA_LAYER_SPEC* ls = (LEABRA_LAYER_SPEC*)lay->GetLayerSpec(net);
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
    float net_eff = u->net * g_bar.e;
    float E_i;
    if(adapt.on && adapt.Ei_dyn) {
      // update the E_i reversal potential as function of inhibitory current
      // key to assume that this is driven by backpropagating AP's
      E_i = u->E_i;
      u->E_i += adapt.Ei_gain * u->act_eq + adapt.Ei_dt * (e_rev.i - u->E_i);
    }
    else {
      E_i = e_rev.i;
    }

    float gc_l = g_bar.l;
    float gc_i = u->gc_i * g_bar.i;

    if(updt_spk_vm) {
      // first compute v_m, using midpoint method:
      float v_m_eff = u->v_m;
      // midpoint method: take a half-step:
      float I_net_1 =
        (net_eff * (e_rev.e - v_m_eff)) + (gc_l * (e_rev.l - v_m_eff)) +
        (gc_i * (E_i - v_m_eff)) - u->adapt;
      v_m_eff += .5f * dt.integ * dt.vm_dt * I_net_1; // go half way
      float I_net = (net_eff * (e_rev.e - v_m_eff)) + (gc_l * (e_rev.l - v_m_eff))
        + (gc_i * (E_i - v_m_eff)) - u->adapt;
      // add spike current if relevant
      if(spike_misc.ex) {
        I_net += gc_l * spike_misc.exp_slope *
          expf((v_m_eff - act.thr) / spike_misc.exp_slope); // todo: exp_fast
      }
      u->v_m += dt.integ * dt.vm_dt * I_net;
      u->I_net = I_net;
    }

    // always compute v_m_eq with simple integration -- used for rate code subthreshold
    float I_net_r = (net_eff * (e_rev.e - u->v_m_eq)) 
      + (gc_l * (e_rev.l - u->v_m_eq)) +  (gc_i * (E_i - u->v_m_eq));
    u->v_m_eq += dt.integ * dt.vm_dt * (I_net_r - u->adapt);
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
        LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
        if(send_gp->NotActive()) continue;
        // LEABRA_LAYER_STATE* tol = (LEABRA_LAYER_STATE*) send_gp->GetPrjnRecvLayer(net);
        // if(tol->hard_clamped)      continue;
        LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)send_gp->GetConSpec(net);
        if(cs->IsDeepRawCon()) {
          // SendDeepRawConSpec* sp = (SendDeepRawConSpec*)cs;
          // sp->Send_DeepRawNetDelta(send_gp, net, thr_no, act_delta);
        }
      }
      u->deep_raw_sent = act_ts;     // cache the last sent value
    }
  }
  else if(u->deep_raw_sent > opt_thresh.send) {
    float act_delta = - u->deep_raw_sent; // un-send the last above-threshold activation to get back to 0
    const int nsg = u->NSendConGps(net); 
    for(int g=0; g< nsg; g++) {
      LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
      if(send_gp->NotActive()) continue;
      // LEABRA_LAYER_STATE* tol = (LEABRA_LAYER_STATE*) send_gp->GetPrjnRecvLayer(net);
      // if(tol->hard_clamped)        continue;
      LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)send_gp->GetConSpec(net);
      if(cs->IsDeepRawCon()) {
        // SendDeepRawConSpec* sp = (SendDeepRawConSpec*)cs;
        // sp->Send_DeepRawNetDelta(send_gp, net, thr_no, act_delta);
      }
    }
    u->deep_raw_sent = 0.0f;         // now it effectively sent a 0..
  }
}


