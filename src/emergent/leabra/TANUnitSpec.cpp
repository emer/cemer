// Copyright 2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "TANUnitSpec.h"

#include <LeabraNetwork>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TANUnitSpec);
TA_BASEFUNS_CTORS_DEFN(TANActSpec);

void TANActSpec::Initialize() {
  plus_fm_pv_vs = true;
  send_plus = true;
}

void TANUnitSpec::Initialize() {
  Defaults_init();
}

void TANUnitSpec::Defaults_init() {
  SetUnique("deep_raw_qtr", true);
  deep_raw_qtr = QALL;
}

void TANUnitSpec::HelpConfig() {
  String help = "TANUnitSpec Computation:\n\
 - Send ACh value to receiving units.\n\
 - Can be driven from OFC or learn itself from MarkerConSpecs from PV, VSPatch units\n\
 \nTANUnitSpec Configuration:\n\
 - For learning, use MarkerConSpecs from PosPV, VSPatchPosD1, or just RewTarg if avail.\n\
 - Learning cons from stimulus predictive inputs should be simple LeabraDeltaConSpec";
  taMisc::Confirm(help);
}

void TANUnitSpec::Compute_PlusPhase_Netin(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  float max_send_act = 0.0f;
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->con_spec;
    if(!cs->IsMarkerCon()) continue;
    const int sz = recv_gp->size;
    for(int i=0; i< sz; i++) {
      // receiving from act_eq must happen outside of Compute_Act stage!
      const float act_eq = ((LeabraUnitState_cpp*)recv_gp->UnState(i,net))->act_eq;
      max_send_act = fmaxf(act_eq, max_send_act);
    }
  }
  u->ext = max_send_act;
  u->net = u->ext;
}

void TANUnitSpec::Compute_PlusPhase_Act(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;
}

void TANUnitSpec::Send_ACh(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  const float snd_val = u->act_eq;
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitState_cpp*)send_gp->UnState(j,net))->ach = snd_val;
    }
  }
}

void TANUnitSpec::Compute_NetinInteg(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  if(tan.plus_fm_pv_vs && (net->phase == LeabraNetwork::PLUS_PHASE)) {
    Compute_PlusPhase_Netin(u, net, thr_no);
  }
  else {
    inherited::Compute_NetinInteg(u, net, thr_no);
  }
}


void TANUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  if(tan.plus_fm_pv_vs && (net->phase == LeabraNetwork::PLUS_PHASE)) {
    Compute_PlusPhase_Act(u, net, thr_no);
  }
  else {
    inherited::Compute_Act_Rate(u, net, thr_no);
  }
}

void TANUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  if(tan.plus_fm_pv_vs && (net->phase == LeabraNetwork::PLUS_PHASE)) {
    Compute_PlusPhase_Act(u, net, thr_no);
  }
  else {
    inherited::Compute_Act_Spike(u, net, thr_no);
  }
}

void TANUnitSpec::Compute_Act_Post(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Post(u, net, thr_no);
  // must send all modulators in act_post
  if(tan.plus_fm_pv_vs && (net->phase == LeabraNetwork::PLUS_PHASE)) {
    if(tan.send_plus) {
      Send_ACh(u, net, thr_no);
    }
  }
  else {
    if(Quarter_DeepRawNow(net->quarter))
      Send_ACh(u, net, thr_no);
  }
}

