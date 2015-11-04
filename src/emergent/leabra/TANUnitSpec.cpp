// Copyright, 1995-2013, Regents of the University of Colorado,
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
  send_plus = false;
}

void TANUnitSpec::Initialize() {
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

void TANUnitSpec::Compute_PlusPhase(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float max_send_act = 0.0f;
  const int nrg = u->NRecvConGps(net, thr_no);
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->con_spec;
    if(!cs->IsMarkerCon()) continue;
    const int sz = recv_gp->size;
    for(int i=0; i< sz; i++) {
      const float act_eq = ((LeabraUnitVars*)recv_gp->UnVars(i,net))->act_eq;
      max_send_act = MAX(act_eq, max_send_act);
    }
  }
  u->ext = max_send_act;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;
}

void TANUnitSpec::Send_ACh(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  const float snd_val = u->act_eq;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitVars*)send_gp->UnVars(j,net))->ach = snd_val;
    }
  }
}

void TANUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(tan.plus_fm_pv_vs && (net->phase == LeabraNetwork::PLUS_PHASE)) {
    Compute_PlusPhase(u, net, thr_no);
    if(tan.send_plus) {
      Send_ACh(u, net, thr_no);
    }
  }
  else {
    inherited::Compute_Act_Rate(u, net, thr_no);
    Send_ACh(u, net, thr_no);
  }
}

void TANUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(tan.plus_fm_pv_vs && (net->phase == LeabraNetwork::PLUS_PHASE)) {
    Compute_PlusPhase(u, net, thr_no);
    if(tan.send_plus) {
      Send_ACh(u, net, thr_no);
    }
  }
  else {
    inherited::Compute_Act_Spike(u, net, thr_no);
    Send_ACh(u, net, thr_no);
  }
}

