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
  nogo = 0.5f;
  thr_act = true;
  Defaults_init();
}

void GPiMiscSpec::Defaults_init() {
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

void GPiUnitSpec::Compute_NetinRaw(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  int nt = net->lthreads.n_threads_act;

  // note: REQUIRES NetinPerPrjn!  Set automatically in CheckConfig
  float go_in = 0.0f;
  float nogo_in = 0.0f;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();

    float g_nw_nt = 0.0f;
    for(int j=0;j<nt;j++) {
      float& ndval = net->send_netin_tmp.FastEl3d(u->flat_idx, g, j); 
      g_nw_nt += ndval;
      ndval = 0.0f;           // zero immediately upon use -- for threads
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

  u->net_delta = 0.0f;  // clear for next use
  u->gi_delta = 0.0f;  // clear for next use
}

void GPiUnitSpec::Send_Thal(LeabraUnit* u, LeabraNetwork* net) {
  float snd_val;
  if(gpi.thr_act) {
    if(u->act_eq <= gpi.gate_thr) u->act_eq = 0.0f;
    snd_val = u->act_eq;
  }
  else {
    snd_val = (u->act_eq > gpi.gate_thr ? u->act_eq : 0.0f);
  }

  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnit*)send_gp->Un(j,net))->thal = snd_val;
    }
  }
}

void GPiUnitSpec::Compute_Act(Unit* ru, Network* rnet, int thread_no) {
  inherited::Compute_Act(ru, rnet, thread_no);
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  Send_Thal(u, net);
}

