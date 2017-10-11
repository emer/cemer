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

#include "GPiInvUnitSpec.h"

#include <LeabraNetwork>
#include <PFCUnitSpec>
#include <MSNUnitSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(GPiGateSpec);
TA_BASEFUNS_CTORS_DEFN(GPiMiscSpec);
TA_BASEFUNS_CTORS_DEFN(GPiInvUnitSpec);

void GPiGateSpec::Initialize() {
  updt_net = true;
  Defaults_init();
}

void GPiGateSpec::Defaults_init() {
  gate_cyc = 18;
}

void GPiMiscSpec::Initialize() {
  Defaults_init();
}

void GPiMiscSpec::Defaults_init() {
  net_gain = 3.0f;
  nogo = 1.0f;
  gate_thr = 0.2f;
  thr_act = true;
  min_thal = 0.2f;
  tot_gain = net_gain + nogo;
  thal_rescale = (1.0f - min_thal) / (1.0f - gate_thr);
}

void GPiMiscSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_gain = net_gain + nogo;
}  

void GPiInvUnitSpec::Initialize() {
  Defaults_init();
}

void GPiInvUnitSpec::Defaults_init() {
  gate_qtr = Q1_Q3;
}

void GPiInvUnitSpec::HelpConfig() {
  String help = "GPiInvUnitSpec Computation:\n\
 - Computes BG output as a positive activation signal with competition inhibition\n\
 select best gating candidate -- should receive inputs from MatrixGo and NoGo layers.\n\
 NoGo is indicated just by the presence of NoGo in the layer name.\n\
 \nGPiInvUnitSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure layers.\n\
 - Recv cons should be standard, with no learning.\n\
 - Should send to PFC, and back to Matrix Go and NoGo, to deliver thal gating signal to drive learning.";
  taMisc::Confirm(help);
}

bool GPiInvUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  net->SetNetFlag(Network::NETIN_PER_PRJN); // this is required for this computation!

  bool rval = true;

  return rval;
}

void GPiInvUnitSpec::Compute_NetinRaw(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  int nt = net->n_thrs_built;
  int flat_idx = u->flat_idx;

  // note: REQUIRES NetinPerPrjn!  Set automatically in CheckConfig
  float go_in = 0.0f;
  float nogo_in = 0.0f;
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)from->GetUnitSpec();

    float g_nw_nt = 0.0f;
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendNetinTmpPerPrjn(j, g)[flat_idx]; 
      g_nw_nt += ndval;
    }

    recv_gp->net_raw += g_nw_nt;

    if(us->InheritsFrom(&TA_MSNUnitSpec)) {
      MSNUnitSpec* msu = (MSNUnitSpec*)us;
      if(msu->dorsal_ventral == MSNUnitSpec::DORSAL) {
        if(msu->dar == MSNUnitSpec::D1R) {
          go_in += recv_gp->net_raw;
        }
        else { // D2R
          nogo_in += recv_gp->net_raw;
        }
      }
      else { // ventral
        if(msu->valence == MSNUnitSpec::APPETITIVE) {
          if(msu->dar == MSNUnitSpec::D1R) {
            go_in += recv_gp->net_raw;
          }
          else { // D2R
            nogo_in += recv_gp->net_raw;
          }
        }
        else {                  // aversive
          if(msu->dar == MSNUnitSpec::D2R) { // FLIPPED!!!
            go_in += recv_gp->net_raw;
          }
          else { // D1R
            nogo_in += recv_gp->net_raw;
          }
        }
      }
    }
    else { // non-MSNUnitSpec inputs.. just go with the name..
      if(from->name.contains("NoGo") || from->name.contains("D2")) {
        nogo_in += recv_gp->net_raw;
      }
      else {
        go_in += recv_gp->net_raw;
      }
    }
  }

  float gpi_net = 0.0f;
  gpi_net = gpi.tot_gain * (go_in - gpi.nogo * nogo_in);
  gpi_net = fmaxf(gpi_net, 0.0f);
  u->net_raw = gpi_net;
}

void GPiInvUnitSpec::Send_Thal(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  bool gate_now = Quarter_GateNow(net->quarter);
  int qtr_cyc = net->QuarterCycle();

  float snd_val = 0.0f;
  float gate_val = 0.0f;
  if(net->quarter == 0 && qtr_cyc <= 1) { // reset
    u->thal_cnt = 0.0f;
  }

  if(gate_now && qtr_cyc == gate.gate_cyc) {
    gate_val = 1.0f;            // gated!
    if(gate.updt_net) {
      LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
      if(un->idx == 0) {        // choose only first unit to do this -- not based on threads b/c we don't know for sure that thread 0 will be represented here!
        net->ThalGatedNow();      // record 
      }
    }
    if(gpi.thr_act) {
      if(u->act_eq <= gpi.gate_thr) u->act_eq = 0.0f;
      snd_val = u->act_eq;
    }
    else {
      snd_val = (u->act_eq > gpi.gate_thr ? u->act_eq : 0.0f);
    }

    if(snd_val > 0.0f && gpi.min_thal > gpi.gate_thr) {
      if(gpi.min_thal == 1.0f) {
        snd_val = 1.0f;
      }
      else {
        snd_val = gpi.min_thal + (snd_val - gpi.gate_thr) * gpi.thal_rescale;
      }
    }
    u->thal_cnt = snd_val;      // save gating value!
  }
  else {
    snd_val = u->thal_cnt;
  }
  
  u->thal = snd_val;            // record what we send, always
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->SendConState(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
    if(!cs->IsMarkerCon()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitState_cpp*)send_gp->UnState(j,net))->thal = snd_val;
      ((LeabraUnitState_cpp*)send_gp->UnState(j,net))->thal_gate = gate_val;
    }
  }
}

void GPiInvUnitSpec::Compute_Act_Post(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Post(u, net, thr_no);
  Send_Thal(u, net, thr_no);    // note: essential to send all modulation in Act_Post
}

