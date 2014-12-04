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

#include "GPiUnitSpec.h"

#include <LeabraNetwork>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(GPiUnitSpec);
TA_BASEFUNS_CTORS_DEFN(GPiMiscSpec);

void GPiMiscSpec::Initialize() {
  Defaults_init();
}

void GPiMiscSpec::Defaults_init() {
  nogo = 0.01f;
  thr_act = true;
  gate_thr = 0.5f;
  net_gain = 2.0f;
  tot_gain = net_gain + nogo;
}

void GPiMiscSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_gain = net_gain + nogo;
}  

void GPiUnitSpec::Initialize() {
}

void GPiUnitSpec::Defaults_init() {
}

void GPiUnitSpec::HelpConfig() {
  String help = "GPiUnitSpec Computation:\n\
 - Computes BG output as a positive activation signal with competition inhibition\n\
 select best gating candidate -- should receive inputs from MatrixGo and NoGo layers.\n\
 NoGo is indicated just by the presence of NoGo in the layer name.\n\
 \nGPiUnitSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure layers.\n\
 - Recv cons should be standard, with no learning.\n\
 - Should send back to Matrix Go and NoGo to deliver thal gating signal to drive learning.";
  taMisc::Confirm(help);
}

bool GPiUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  LeabraUnit* u = (LeabraUnit*)un;
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  LeabraNetwork* net = (LeabraNetwork*)un->own_net();
  net->SetNetFlag(Network::NETIN_PER_PRJN); // this is required for this computation!

  bool rval = true;

  return rval;
}

void GPiUnitSpec::Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  int nt = net->lthreads.n_threads_act;
  int flat_idx = u->UnFlatIdx(net, thr_no);
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif

  // note: REQUIRES NetinPerPrjn!  Set automatically in CheckConfig
  float go_in = 0.0f;
  float nogo_in = 0.0f;
  const int nrg = u->NRecvConGps(net, thr_no);
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();

    float g_nw_nt = 0.0f;
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendNetinTmpPerPrjn(j, g)[flat_idx]; 
      g_nw_nt += ndval;
#ifndef CUDA_COMPILE
        ndval = 0.0f;           // zero immediately upon use -- for threads
#endif
    }

    recv_gp->net_raw += g_nw_nt;

    if(from->name.contains("NoGo")) {
      nogo_in += recv_gp->net_raw;
    }
    else {
      go_in += recv_gp->net_raw;
    }
  }

  float gpi_net = 0.0f;
  gpi_net = gpi.tot_gain * (go_in - gpi.nogo * nogo_in);
  gpi_net = MAX(gpi_net, 0.0f);
  u->net_raw = gpi_net;
}

void GPiUnitSpec::Send_Thal(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float snd_val;
  if(gpi.thr_act) {
    if(u->act_eq <= gpi.gate_thr) u->act_eq = 0.0f;
    snd_val = u->act_eq;
  }
  else {
    snd_val = (u->act_eq > gpi.gate_thr ? u->act_eq : 0.0f);
  }

  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitVars*)send_gp->UnVars(j,net))->thal = snd_val;
    }
  }
}

void GPiUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Rate(u, net, thr_no);
  Send_Thal(u, net, thr_no);
}

void GPiUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Spike(u, net, thr_no);
  Send_Thal(u, net, thr_no);
}

