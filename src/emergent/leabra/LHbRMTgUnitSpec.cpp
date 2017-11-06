// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

bool STATE_CLASS(LHbRMTgUnitSpec)::GetRecvLayers
(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, 
 LEABRA_LAYER_STATE*& pv_pos_lay, LEABRA_LAYER_STATE*& vspatch_pos_D1_lay,
 LEABRA_LAYER_STATE*& vspatch_pos_D2_lay, LEABRA_LAYER_STATE*& vsmatrix_pos_D1_lay,
 LEABRA_LAYER_STATE*& vsmatrix_pos_D2_lay, LEABRA_LAYER_STATE*& pv_neg_lay,
 LEABRA_LAYER_STATE*& vspatch_neg_D1_lay, LEABRA_LAYER_STATE*& vspatch_neg_D2_lay,
 LEABRA_LAYER_STATE*& vsmatrix_neg_D1_lay, LEABRA_LAYER_STATE*& vsmatrix_neg_D2_lay) {
  
  pv_pos_lay = NULL;
  vspatch_pos_D1_lay = NULL;
  vspatch_pos_D2_lay = NULL;
  vsmatrix_pos_D1_lay = NULL;
  vsmatrix_pos_D2_lay = NULL;
  pv_neg_lay = NULL;
  vspatch_neg_D1_lay = NULL;
  vspatch_neg_D2_lay = NULL;
  vsmatrix_neg_D1_lay = NULL;
  vsmatrix_neg_D2_lay = NULL;
  
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_LAYER_STATE* fmlay = (LEABRA_LAYER_STATE*) recv_gp->GetSendLayer(net);
    LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*) fmlay->GetUnitSpec(net);
    if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_MSNUnitSpec) {
      if(u->HasExtFlag(LEABRA_UNIT_STATE::PATCH)) {
        if(u->HasExtFlag(LEABRA_UNIT_STATE::D2R)) {
          if(u->HasExtFlag(LEABRA_UNIT_STATE::APPETITIVE)) { vspatch_pos_D2_lay = fmlay; }
          else { vspatch_neg_D2_lay = fmlay; }
        }
        else { // D1R
          if(u->HasExtFlag(LEABRA_UNIT_STATE::APPETITIVE)) { vspatch_pos_D1_lay = fmlay; }
          else { vspatch_neg_D1_lay = fmlay; }
        }
      }
      else if(u->HasExtFlag(LEABRA_UNIT_STATE::MATRIX)) {
        if(u->HasExtFlag(LEABRA_UNIT_STATE::D2R)) {
          if (u->HasExtFlag(LEABRA_UNIT_STATE::APPETITIVE)) { vsmatrix_pos_D2_lay = fmlay; }
          else { vsmatrix_neg_D2_lay = fmlay; }
        }
        else { // D1R
          if (u->HasExtFlag(LEABRA_UNIT_STATE::APPETITIVE)) { vsmatrix_pos_D1_lay = fmlay; }
          else { vsmatrix_neg_D1_lay = fmlay; }
        }
      }
    }
    else if(fmlay->LayerNameContains("PosPV")) {
      pv_pos_lay = fmlay;
    }
    else if(fmlay->LayerNameContains("NegPV")) {
      pv_neg_lay = fmlay;
    }
  }
  return true;
}

void STATE_CLASS(LHbRMTgUnitSpec)::Compute_Lhb(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  LEABRA_LAYER_STATE* pv_pos_lay = NULL;
  LEABRA_LAYER_STATE* vspatch_pos_D1_lay = NULL;
  LEABRA_LAYER_STATE* vspatch_pos_D2_lay = NULL;
  LEABRA_LAYER_STATE* vsmatrix_pos_D1_lay = NULL;
  LEABRA_LAYER_STATE* vsmatrix_pos_D2_lay = NULL;
  LEABRA_LAYER_STATE* pv_neg_lay = NULL;
  LEABRA_LAYER_STATE* vspatch_neg_D1_lay = NULL;
  LEABRA_LAYER_STATE* vspatch_neg_D2_lay = NULL;
  LEABRA_LAYER_STATE* vsmatrix_neg_D1_lay = NULL;
  LEABRA_LAYER_STATE* vsmatrix_neg_D2_lay = NULL;
  
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
  
  GetRecvLayers(u, net, pv_pos_lay, vspatch_pos_D1_lay, vspatch_pos_D2_lay,
                vsmatrix_pos_D1_lay, vsmatrix_pos_D2_lay, pv_neg_lay,
                vspatch_neg_D1_lay, vspatch_neg_D2_lay,
                vsmatrix_neg_D1_lay, vsmatrix_neg_D2_lay);
  
  // use avg act over layer..
  // by default, patch uses acts_q0 for patch to reflect previous trial..
  float vspatch_pos_D1;
  if(lhb.patch_cur) { // use current timestep's acts instead...
    vspatch_pos_D1 = vspatch_pos_D1_lay->GetTotalActEq(net);
  }
  else {
    vspatch_pos_D1 = vspatch_pos_D1_lay->GetTotalActQ0(net);
  }
  
  float vspatch_pos_D2;
  if(lhb.patch_cur) {
    vspatch_pos_D2 = vspatch_pos_D2_lay->GetTotalActEq(net);
  }
  else {
    vspatch_pos_D2 = vspatch_pos_D2_lay->GetTotalActQ0(net);
  }
  
  float vspatch_pos_net = (gains.vspatch_pos_D1 * vspatch_pos_D1) - (gains.vspatch_pos_D2 * vspatch_pos_D2); // positive number net excitatory in LHb, i.e., the "dipper"
  if (vspatch_pos_net < 0.0f) {
    vspatch_pos_net *= gains.vspatch_pos_disinhib_gain;
  }
  
  // repeat for AVERSIVE guys...
  float vspatch_neg_D1;
  if(lhb.patch_cur) {
    vspatch_neg_D1 = vspatch_neg_D1_lay->GetTotalActEq(net);
  }
  else {
    vspatch_neg_D1 = vspatch_neg_D1_lay->GetTotalActQ0(net);
  }
  
  float vspatch_neg_D2;
  if(lhb.patch_cur) {
    vspatch_neg_D2 = vspatch_neg_D2_lay->GetTotalActEq(net);
  }
  else {
    vspatch_neg_D2 = vspatch_neg_D2_lay->GetTotalActQ0(net);
  }
  
  float vspatch_neg_net = (gains.vspatch_neg_D2 * vspatch_neg_D2) - (gains.vspatch_neg_D1 * vspatch_neg_D1); // positive number is net inhibitory in LHb - disinhibitory "burster"
  
  float vsmatrix_pos_D1 = 0.0f;
  if(vsmatrix_pos_D1_lay)
    vsmatrix_pos_D1 = vsmatrix_pos_D1_lay->GetTotalActEq(net);
  float vsmatrix_pos_D2 = 0.0f;
  if(vsmatrix_pos_D2_lay)
    vsmatrix_pos_D2 = vsmatrix_pos_D2_lay->GetTotalActEq(net);

  float vsmatrix_neg_D1 = 0.0f;
  if(vsmatrix_neg_D1_lay) {
    vsmatrix_neg_D1 = vsmatrix_neg_D1_lay->GetTotalActEq(net);
  }
  float vsmatrix_neg_D2 = 0.0f;
  if(vsmatrix_neg_D2_lay) {
    vsmatrix_neg_D2 = vsmatrix_neg_D2_lay->GetTotalActEq(net);
  }
  float pv_pos = pv_pos_lay->GetTotalActEq(net);
  float pv_neg = pv_neg_lay->GetTotalActEq(net);
  
  // pvneg_discount - should not fully predict away an expected punishment
  if (vspatch_neg_net > 0.0f) {
    //vspatch_neg_net = fminf(vspatch_neg_net,pv_neg); // helps mag .05, but
    // prevents burst after mag 1.0 training, then test 0.5
    vspatch_neg_net *= lhb.pvneg_discount;
  }
  
  // net out the VS matrix D1 versus D2 pairs...WATCH the signs - double negatives!
  float vsmatrix_pos_net = (gains.vsmatrix_pos_D1 * vsmatrix_pos_D1) - (gains.vsmatrix_pos_D2 * vsmatrix_pos_D2); // positive number net inhibitory!
  //vsmatrix_pos_net = fmaxf(0.0f, vsmatrix_pos_net); // restrict to positive net values
  float vsmatrix_neg_net = (gains.vsmatrix_neg_D2 * vsmatrix_neg_D2) - (gains.vsmatrix_neg_D1 * vsmatrix_neg_D1); // positive number net excitatory!
  //vsmatrix_neg_net = fmaxf(0.0f, vsmatrix_neg_net); // restrict to positive net values
  
  // don't double count pv going through the matrix guys
  float net_pos = vsmatrix_pos_net;
  if(pv_pos) { net_pos = fmaxf(pv_pos, vsmatrix_pos_net); }
  float net_neg = vsmatrix_neg_net;
  if(pv_neg) {
    // below can arise when same CS can predict either pos_pv or neg_pv probalistically
    if(vsmatrix_pos_net < 0.0f) {
      net_neg = fmaxf(net_neg, fabsf(vsmatrix_pos_net));
      net_pos = 0.0f; // don't double-count since transferred to net_neg in this case only
    }
    net_neg = fmaxf(pv_neg, net_neg);
  }
  
  float net_lhb = net_neg - net_pos + vspatch_pos_net - vspatch_neg_net;
  
  net_lhb *= gains.all;
  
  u->act_eq = u->act_nd = u->act = u->net = u->ext = net_lhb;

  // HMM.. going to need to add this somehow!!
  // if(lhb.rec_data) {
  //   LEABRA_LAYER_STATE* lay = un->own_lay();
  //   lay->SetUserData("pv_pos", pv_pos, false); // false=no update
  //   lay->SetUserData("vsmatrix_pos_D1", vsmatrix_pos_D1, false); // false=no update
  //   lay->SetUserData("vsmatrix_pos_D2", vsmatrix_pos_D2, false); // false=no update
  //   lay->SetUserData("net_pos", net_pos, false); // false=no update
  //   lay->SetUserData("vspatch_pos_D1", vspatch_pos_D1, false); // false=no update
  //   lay->SetUserData("vspatch_pos_D2", vspatch_pos_D2, false); // false=no update
    
  //   lay->SetUserData("pv_neg", pv_neg, false); // false=no update
  //   lay->SetUserData("vsmatrix_neg_D1", vsmatrix_neg_D1, false); // false=no update
  //   lay->SetUserData("vsmatrix_neg_D2", vsmatrix_neg_D2, false); // false=no update
  //   lay->SetUserData("net_neg", net_neg, false); // false=no update
  //   lay->SetUserData("vspatch_neg_D1", vspatch_neg_D1, false); // false=no update
  //   lay->SetUserData("vspatch_neg_D2", vspatch_neg_D2, false); // false=no update
  //   lay->SetUserData("vspatch_neg_net", vspatch_neg_net, false); // false=no update
    
  //   lay->SetUserData("net_lhb", net_lhb, false); // false=no update
  // }
}

