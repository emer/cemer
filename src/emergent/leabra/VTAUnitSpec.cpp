// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {


bool STATE_CLASS(VTAUnitSpec)::GetRecvLayers_P
(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, LEABRA_LAYER_STATE*& pospv_lay,
 LEABRA_LAYER_STATE*& pptg_lay_p, LEABRA_LAYER_STATE*& lhb_lay,
 LEABRA_LAYER_STATE*& vspatchposd1_lay, LEABRA_LAYER_STATE*& vspatchposd2_lay, 
 LEABRA_LAYER_STATE*& vspatchnegd1_lay, LEABRA_LAYER_STATE*& vspatchnegd2_lay) {
  
  pospv_lay = NULL;
  pptg_lay_p = NULL;
  lhb_lay = NULL;
  vspatchposd1_lay = NULL;
  vspatchposd2_lay = NULL;
  vspatchnegd1_lay = NULL;
  vspatchnegd2_lay = NULL;
  
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
    LEABRA_LAYER_STATE* fmlay = recv_gp->GetSendLayer(net);
    LEABRA_UNIT_SPEC_CPP* us =  fmlay->GetUnitSpec(net);
    if(!cs->IsMarkerCon()) continue;

    if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_PPTgUnitSpec) {
      pptg_lay_p = fmlay;
    }
    else if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_LHbRMTgUnitSpec) {
      lhb_lay = fmlay;
    }
    else if(fmlay->LayerNameContains("PV")) {
      pospv_lay = fmlay;
    }
    else if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_MSNUnitSpec) {
      LEABRA_UNIT_STATE* su = fmlay->GetUnitState(net, 0);
      if(su->HasUnitFlag(LEABRA_UNIT_STATE::APPETITIVE) &&
         su->HasUnitFlag(LEABRA_UNIT_STATE::D1R)) {
        vspatchposd1_lay = fmlay;
      }
      else if(su->HasUnitFlag(LEABRA_UNIT_STATE::APPETITIVE) &&
              su->HasUnitFlag(LEABRA_UNIT_STATE::D2R)) {
        vspatchposd2_lay = fmlay;
      }
      else if(su->HasUnitFlag(LEABRA_UNIT_STATE::AVERSIVE) &&
              su->HasUnitFlag(LEABRA_UNIT_STATE::D1R)) {
        vspatchnegd1_lay = fmlay;
      }
      else if(su->HasUnitFlag(LEABRA_UNIT_STATE::AVERSIVE) &&
              su->HasUnitFlag(LEABRA_UNIT_STATE::D2R)) {
        vspatchnegd2_lay = fmlay;
      }
    }
  }
  return true;
}

//bool STATE_CLASS(VTAUnitSpec)::GetRecvLayers_N
//(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, LEABRA_LAYER_STATE*& negpv_lay,
// LEABRA_LAYER_STATE*& pptg_lay_n, LEABRA_LAYER_STATE*& lhb_lay,
// LEABRA_LAYER_STATE*& vspatchnegd1_lay, LEABRA_LAYER_STATE*& vspatchnegd2_lay_n) {
bool STATE_CLASS(VTAUnitSpec)::GetRecvLayers_N
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, LEABRA_LAYER_STATE*& negpv_lay, LEABRA_LAYER_STATE*& lhb_lay, LEABRA_LAYER_STATE*& vspatchnegd1_lay, LEABRA_LAYER_STATE*& vspatchnegd2_lay_n) {
  
  negpv_lay = NULL;
  //pptg_lay_n = NULL;
  lhb_lay = NULL;
  
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
    LEABRA_LAYER_STATE* fmlay = recv_gp->GetSendLayer(net);
    LEABRA_UNIT_SPEC_CPP* us =  fmlay->GetUnitSpec(net);
    if(!cs->IsMarkerCon()) continue;
//    if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_PPTgUnitSpec) {
//      pptg_lay_n = fmlay;
//    }
    else if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_LHbRMTgUnitSpec) {
      lhb_lay = fmlay;
    }
    else if(fmlay->LayerNameContains("PV")) {
      negpv_lay = fmlay;
    }
    else if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_MSNUnitSpec) {
      LEABRA_UNIT_STATE* su = fmlay->GetUnitState(net, 0);
      if(su->HasUnitFlag(LEABRA_UNIT_STATE::AVERSIVE) &&
         su->HasUnitFlag(LEABRA_UNIT_STATE::D2R)) {
        vspatchnegd2_lay_n = fmlay;
      }
      else if(su->HasUnitFlag(LEABRA_UNIT_STATE::AVERSIVE) &&
              su->HasUnitFlag(LEABRA_UNIT_STATE::D1R)) {
        vspatchnegd1_lay = fmlay;
      }
    }
  }
  return true;
}

void STATE_CLASS(VTAUnitSpec)::Compute_DaP
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  LEABRA_LAYER_STATE* pptg_lay_p = NULL;
  LEABRA_LAYER_STATE* lhb_lay = NULL;
  LEABRA_LAYER_STATE* pospv_lay = NULL;
  LEABRA_LAYER_STATE* vspatchposd1_lay = NULL;
  LEABRA_LAYER_STATE* vspatchposd2_lay = NULL;
  LEABRA_LAYER_STATE* vspatchnegd1_lay = NULL;
  LEABRA_LAYER_STATE* vspatchnegd2_lay = NULL;

  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
  
  GetRecvLayers_P(u, net, pospv_lay, pptg_lay_p, lhb_lay, vspatchposd1_lay, vspatchposd2_lay,
                  vspatchnegd1_lay, vspatchnegd2_lay);
    
  // use total activation over whole layer
  float pptg_da_p = pptg_lay_p->GetTotalActEq(net);
  float lhb_da = lhb_lay->GetTotalActEq(net);
  //float lhb_da = lhb_lay->units[0]->act();
  
  float pospv = pospv_lay->GetTotalActEq(net);
  float vspospvi = 0.0f;
  if(da.patch_cur) {
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      vspospvi = (gains.pvi_burst_shunt_gain * vspatchposd1_lay->GetTotalActEq(net)) -
        (gains.pvi_anti_burst_shunt_gain * vspatchposd2_lay->GetTotalActEq(net));
    }
    else {
      vspospvi = gains.pvi_burst_shunt_gain * vspatchposd1_lay->GetTotalActEq(net);
    }
  }
  else {
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      vspospvi = (gains.pvi_burst_shunt_gain * vspatchposd1_lay->GetTotalActQ0(net)) -
        (gains.pvi_anti_burst_shunt_gain * vspatchposd2_lay->GetTotalActQ0(net));
    }
    else {
      vspospvi = gains.pvi_burst_shunt_gain * vspatchposd1_lay->GetTotalActQ0(net);
    }
  }
  
  // vspospvi must be >= 0.0f
  vspospvi = fmaxf(vspospvi, 0.0f);
  
  float vsnegpvi = 0.0f;
  if(da.patch_cur) {
    if(gains.pvi_dip_shunt_gain > 0.0f && gains.pvi_anti_dip_shunt_gain > 0.0f) {
      vsnegpvi = (gains.pvi_dip_shunt_gain * vspatchnegd2_lay->GetTotalActEq(net)) -
        (gains.pvi_anti_dip_shunt_gain * vspatchnegd1_lay->GetTotalActEq(net));
    }
    else if(gains.pvi_dip_shunt_gain > 0.0f) {
      vsnegpvi = gains.pvi_dip_shunt_gain * vspatchnegd2_lay->GetTotalActEq(net);
    }
  }
  else {
    if(gains.pvi_dip_shunt_gain > 0.0f && gains.pvi_anti_dip_shunt_gain > 0.0f) {
      vsnegpvi = (gains.pvi_dip_shunt_gain * vspatchnegd2_lay->GetTotalActQ0(net)) -
        (gains.pvi_anti_dip_shunt_gain * vspatchnegd1_lay->GetTotalActQ0(net));
    }
    else if(gains.pvi_dip_shunt_gain > 0.0f) {
      vsnegpvi = gains.pvi_dip_shunt_gain * vspatchnegd2_lay->GetTotalActQ0(net);
    }
  }

  // ?? vsnegpvi must be >= 0.0f (probably; could be <= so need to think about it...)
  //vsnegpvi = fmaxf(vsnegpvi, 0.0f);
  
  float burst_lhb_da = fminf(lhb_da, 0.0f); // if neg, promotes bursting
  float dip_lhb_da = fmaxf(lhb_da, 0.0f);   // else, promotes dipping
    
  // absorbing PosPV value - prevents double counting
  float tot_burst_da = fmaxf(gains.pv_gain * pospv, gains.pptg_gain * pptg_da_p);
  // likewise for lhb contribution to bursting (burst_lhb_da non-positive)
  tot_burst_da = fmaxf(tot_burst_da, -gains.lhb_gain * burst_lhb_da);
   
  // pos PVi shunting
  float net_burst_da = tot_burst_da - vspospvi;
  net_burst_da = fmaxf(net_burst_da, 0.0f);
  //if(net_burst_da < 0.1f) { net_burst_da = 0.0f; } // debug...

  float tot_dip_da = gains.lhb_gain * dip_lhb_da;

  // neg PVi shunting
  float net_dip_da = tot_dip_da - vsnegpvi;
  net_dip_da = fmaxf(net_dip_da, 0.0f);
  //if(net_dip_da < 0.1f) { net_dip_da = 0.0f; } // debug...
    
  float net_da = net_burst_da - net_dip_da;
  net_da *= gains.da_gain;

  net_da -= da.se_gain * u->sev; // subtract 5HT serotonin -- has its own gain
  
  u->da_p = net_da;
  lay->da_p = u->da_p;
  u->ext = da.tonic_da + u->da_p;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;

  net->ext_rew_avail = true;    // always record pv values -- todo: why??
  net->ext_rew = pospv;

  // TODO: rec data
  // if(da.rec_data) {
  //   lay->SetUserData("pospv", pospv, false); // false=no update
  //   lay->SetUserData("pptg_da_p", pptg_da_p, false); // false=no update
  //   lay->SetUserData("lhb_da", lhb_da, false); // false=no update
  //   lay->SetUserData("tot_burst_da", tot_burst_da, false); // false=no update
  //   lay->SetUserData("vs_patch_burst_shunt_net", vspospvi, false); // false=no update
  //   lay->SetUserData("vs_patch_dip_shunt_net", vsnegpvi, false); // false=no update
  //   lay->SetUserData("net_burst_da", net_burst_da, false); // false=no update
  //   lay->SetUserData("net_da", net_da, false); // false=no update
  // }
}

void STATE_CLASS(VTAUnitSpec)::Compute_DaN
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  LEABRA_LAYER_STATE* negpv_lay = NULL;
  //LEABRA_LAYER_STATE* pptg_lay_n = NULL;
  LEABRA_LAYER_STATE* lhb_lay_n = NULL;
  LEABRA_LAYER_STATE* vspatchnegd1_lay = NULL;
  LEABRA_LAYER_STATE* vspatchnegd2_lay = NULL;
  
  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
  
  //GetRecvLayers_N(u, net, negpv_lay, pptg_lay_n, lhb_lay_n, vspatchnegd1_lay,
  //                vspatchnegd2_lay);
  GetRecvLayers_N(u, net, negpv_lay, lhb_lay_n, vspatchnegd1_lay,
                    vspatchnegd2_lay);
  
  float negpv = negpv_lay->GetTotalActEq(net);
  //float pptg_da_n = pptg_lay_n->GetTotalActEq(net);
  float lhb_da_n = lhb_lay_n->GetTotalActEq(net);

  float vspvi_n = 0.0f;
  if(da.patch_cur) {
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      vspvi_n = (gains.pvi_burst_shunt_gain * vspatchnegd2_lay->GetTotalActEq(net)) -
        (gains.pvi_anti_burst_shunt_gain * vspatchnegd1_lay->GetTotalActEq(net));
    }
    else {
      vspvi_n = gains.pvi_burst_shunt_gain * vspatchnegd2_lay->GetTotalActEq(net);
    }
  }
  else {
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      vspvi_n = (gains.pvi_burst_shunt_gain * vspatchnegd2_lay->GetTotalActQ0(net)) -
        (gains.pvi_anti_burst_shunt_gain * vspatchnegd1_lay->GetTotalActQ0(net));
    }
    else {
      vspvi_n = gains.pvi_burst_shunt_gain * vspatchnegd2_lay->GetTotalActQ0(net);
    }
  }
  float burst_lhb_da_n = fmaxf(lhb_da_n, 0.0f); // if pos, promotes bursting
  float dip_lhb_da_n = fminf(lhb_da_n, 0.0f);   // else, promotes dipping
  
  // absorbing NegPV value - prevents double counting
  float negpv_da = negpv;
  negpv_da = fmaxf(negpv_da, 0.0f); // in case we add PVi-like shunting later...
    
    
  //float tot_burst_da = fmaxf(gains.pv_gain * negpv_da, gains.pptg_gain * pptg_da_n);
  //tot_burst_da = fmaxf(tot_burst_da, gains.lhb_gain * burst_lhb_da_n);
  //float tot_burst_da = gains.pv_gain * negpv_da;
  float tot_burst_da = fmaxf(gains.pv_gain * negpv_da, gains.lhb_gain * burst_lhb_da_n);
  
  // PVi shunting
  float net_burst_da = tot_burst_da - vspvi_n;
  net_burst_da = fmaxf(net_burst_da, 0.0f);
    
  float tot_dip_da = gains.lhb_gain * dip_lhb_da_n;
  
//  float net_da = fmaxf(gains.pv_gain * negpv_da, gains.pptg_gain * pptg_da_n);
  
  float net_da = net_burst_da + tot_dip_da;
  
  net_da *= gains.da_gain;
    
  u->da_n = net_da;
  lay->da_n = u->da_n;
  u->ext = da.tonic_da + u->da_n;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;

  // TODO: rec data
  // if(da.rec_data) {
  //   lay->SetUserData("negpv", negpv, false); // false=no update
  //   lay->SetUserData("pptg_da_n", pptg_da_n, false); // false=no update
  //   lay->SetUserData("lhb_da_n", lhb_da_n, false); // false=no update
  //   lay->SetUserData("tot_burst_da", tot_burst_da, false); // false=no update
  //   lay->SetUserData("tot_dip_da", tot_dip_da, false); // false=no update
  //   lay->SetUserData("net_da", net_da, false); // false=no update
  // }
}

void STATE_CLASS(VTAUnitSpec)::Send_Da
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  const float snd_val = u->act_eq;
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      if(da_val == DA_P) {
        send_gp->UnState(j,net)->da_p = snd_val;
      }
      else {
        send_gp->UnState(j,net)->da_n = snd_val;
      }
    }
  }
}

void STATE_CLASS(VTAUnitSpec)::Compute_Act_Rate
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  if(Quarter_DeepRawNow(net->quarter)) {
    if(da_val == DA_P) {
      Compute_DaP(u, net, thr_no);
    }
    else {
      Compute_DaN(u, net, thr_no);
    }
  }
  else {
    u->act_eq = u->act_nd = u->act = u->net = 0.0f;
  }
  u->da = 0.0f;
}
