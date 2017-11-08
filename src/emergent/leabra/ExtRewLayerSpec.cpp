// this is included directly in LeabraExtraLayerSpecs_cpp / _cuda
// {

bool STATE_CLASS(ExtRewLayerSpec)::OutErrRewAvail(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  bool got_some = false;
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 1); // taking 2nd unit as representative
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    LEABRA_LAYER_STATE* rew_lay = (LEABRA_LAYER_STATE*)recv_gp->GetSendLayer(net);
    if(!rew_lay->LayerNameIs("RewTarg")) continue;
    LEABRA_UNIT_STATE* rtu = (LEABRA_UNIT_STATE*)rew_lay->GetUnitState(net, 0);
    if(rtu->act_eq > 0.5f) {
      got_some = true;
      break;
    }
  }
  return got_some;
}

float STATE_CLASS(ExtRewLayerSpec)::GetOutErrRew(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 1);

  // first pass: find the layers: use COMP if no TARG is found
  int   n_targs = 0;            // number of target layers
  int   n_comps = 0;            // number of comp layers
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    LEABRA_LAYER_STATE* rew_lay = (LEABRA_LAYER_STATE*)recv_gp->GetSendLayer(net);
    if(rew_lay->LayerNameIs("RewTarg")) continue;

    if(rew_lay->HasExtFlag(UNIT_STATE::TARG)) n_targs++;
    else if(rew_lay->HasExtFlag(UNIT_STATE::COMP)) n_comps++;
  }

  int rew_chk_flag = UNIT_STATE::TARG;
  if(n_targs == 0) rew_chk_flag |= UNIT_STATE::COMP; // also check comp if no targs!

  float totposs = 0.0f;         // total possible error (unitwise)
  float toterr = 0.0f;          // total error
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    LEABRA_LAYER_STATE* rew_lay = (LEABRA_LAYER_STATE*)recv_gp->GetSendLayer(net);
    if(rew_lay->LayerNameIs("RewTarg")) continue;

    if(!rew_lay->HasExtFlag(rew_chk_flag)) continue; // only proceed if valid
    //  toterr += rew_lay->norm_err;        // now using norm err
    // this is now no longer computed at the point where we need it!  must compute ourselves!

    LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)rew_lay->GetUnitSpec(net);
    
    float this_err = 0.0f;
    for(int ui = 0; ui < rew_lay->n_units; ui++) {
      LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)rew_lay->GetUnitState(net, ui);
      if(un->lesioned()) continue;
      bool targ_active = false;
      float unerr = us->Compute_NormErr(un, net, 0, targ_active);
      if(unerr > 0.0f) {
        this_err = 1.0f;
        break;
      }
    }
    toterr += this_err;
    totposs += 1.0f;
  }
  if(totposs == 0.0f)
    return -1.0f;               // -1 = no reward signal at all
  if(out_err.graded) {
    float nrmerr = toterr / totposs;
    if(nrmerr > 1.0f) nrmerr = 1.0f;
    return 1.0f - nrmerr;
  }
  if(toterr > 0.0f) return 0.0f; // 0 = wrong, 1 = correct
  return 1.0f;
}

void STATE_CLASS(ExtRewLayerSpec)::Compute_OutErrRew(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  if(!OutErrRewAvail(lay, net)) {
    Compute_NoRewAct(lay, net);
    return;
  }

  float er = GetOutErrRew(lay, net);
  // starts out 0-1, transform into correct range
  er = (rew.rew_val - rew.err_val) * er + rew.err_val;

  UNIT_GP_ITR
    (lay,
     LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
     LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, 0);
     Compute_UnitDa(lay, net, gpidx, er, u);
     );

  net->ext_rew = er;
  net->ext_rew_avail = true;
}

void STATE_CLASS(ExtRewLayerSpec)::Compute_ExtRew(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  if(!lay->HasExtFlag(UNIT_STATE::TARG)) {
    Compute_NoRewAct(lay, net);
    return;
  }

  int n_rew = 0;
  float er_avg = 0.0f;

  UNIT_GP_ITR
    (lay,
     LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
     LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, 0);
     float er = u->ext;
     if(er == rew.norew_val) {
       u->ext = u->ext_orig = rew.norew_val;  // this is appropriate to set here..
       ClampValue_ugp(lay, net, gpidx);
     }
     else {
       er_avg += er;
       n_rew++;
       Compute_UnitDa(lay, net, gpidx, er, u);
     }
     );

  if(n_rew > 0) {
    net->ext_rew = er_avg / (float)n_rew;
    net->ext_rew_avail = true;
  }
  else {
    net->ext_rew = -1.1f;       // indicates no rew
    net->ext_rew_avail = false;
  }
}

void STATE_CLASS(ExtRewLayerSpec)::Compute_DaRew(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  int n_rew = 0;
  float er_avg = 0.0f;

  UNIT_GP_ITR
    (lay,
     LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
     LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, 0);
     float er = u->da_p;
     if(er == rew.norew_val) {
       u->ext = u->ext_orig = rew.norew_val;  // this is appropriate to set here..
       ClampValue_ugp(lay, net, gpidx);
     }
     else {
       er_avg += er;
       n_rew++;
       Compute_UnitDa(lay, net, gpidx, er, u);
     }
     );

  if(n_rew > 0) {
    net->ext_rew = er_avg / (float)n_rew;
    net->ext_rew_avail = true;
  }
  else {
    net->ext_rew = -1.1f;       // indicates no rew
    net->ext_rew_avail = false;
  }
}

void STATE_CLASS(ExtRewLayerSpec)::Compute_UnitDa
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, float er, LEABRA_UNIT_STATE* u) {
  u->da_p = er;
  u->ext = u->ext_orig = u->da_p;
  ClampValue_ugp(lay, net, gpidx);
}

void STATE_CLASS(ExtRewLayerSpec)::Compute_NoRewAct(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  net->ext_rew = -1.1f; // indicates no rew
  net->ext_rew_avail = false;

  UNIT_GP_ITR
    (lay,
     LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
     LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, 0);
     u->ext = u->ext_orig = rew.norew_val;
     ClampValue_ugp(lay, net, gpidx);
     );
}

void STATE_CLASS(ExtRewLayerSpec)::Compute_HardClamp_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  if(net->phase == LEABRA_NETWORK_STATE::MINUS_PHASE) {
    lay->SetExtFlag(UNIT_STATE::EXT);
    Compute_NoRewAct(lay, net); // no reward in minus
    HardClampExt(lay, net);
  }
  else {
    lay->SetExtFlag(UNIT_STATE::EXT);
    Compute_Rew(lay, net);
    HardClampExt(lay, net);
  }
}
