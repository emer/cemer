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

#include "DRNUnitSpec.h"

#include <LeabraNetwork>
#include <VTAUnitSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DRNUnitSpec);

TA_BASEFUNS_CTORS_DEFN(DRN5htSpec);

void DRN5htSpec::Initialize() {
  Defaults_init();
}

void DRN5htSpec::Defaults_init() {
  se_out_gain = 1.0f;
  se_base = 0.0f;
  se_inc_tau = 50.0f;
  da_pos_tau = 10.0f;
  da_neg_tau = 10.0f;
  se_pv_tau = 20.0f;
  se_state_tau = 200.0f;
  sub_pos = true;

  se_inc_dt = 1.0f / se_inc_tau;
  da_pos_dt = 1.0f / da_pos_tau;
  da_neg_dt = 1.0f / da_neg_tau;
  se_pv_dt = 1.0f / se_pv_tau;
  se_state_dt = 1.0f / se_state_tau;
}
  
void DRN5htSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  se_inc_dt = 1.0f / se_inc_tau;
  da_pos_dt = 1.0f / da_pos_tau;
  da_neg_dt = 1.0f / da_neg_tau;
  se_pv_dt = 1.0f / se_pv_tau;
  se_state_dt = 1.0f / se_state_tau;
}


void DRNUnitSpec::Initialize() {
}

void DRNUnitSpec::HelpConfig() {
  String help = "DRNUnitSpec (5HT serotonin value) Computation:\n\
 - Computes SE value based on inputs from PV and State layers.\n\
 - No Learning\n\
 \nDRNUnitSpec Configuration:\n\
 - Use the Wizard PVLV button to automatically configure layers.\n\
 - Recv cons marked with a MarkerConSpec from inputs";
  taMisc::Confirm(help);
}

void DRNUnitSpec::Compute_Se(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float pospv = 0.0f;
  int   pospv_n  = 0;
  float negpv = 0.0f;
  int   negpv_n  = 0;
  float posstate = 0.0f;
  int   posstate_n = 0;
  float negstate = 0.0f;
  int   negstate_n = 0;

  const int nrg = u->NRecvConGps(net, thr_no);
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* from = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)from->GetUnitSpec();

    const float act_avg = from->acts_eq.avg;
    if(us->InheritsFrom(&TA_VTAUnitSpec)) {
      continue;                 // skip -- just read from dav
    }
    else {
      if(from->name.contains("Pos")) {
        if(from->name.contains("State")) {
          posstate += act_avg;
          posstate_n++;
        }
        else {                    // PV
          pospv += act_avg;
          pospv_n++;
        }
      }
      else {
        if(from->name.contains("State")) {
          negstate += act_avg;
          negstate_n++;
        }
        else {                    // PV
          negpv += act_avg;
          negpv_n++;
        }
      }
    }
  }

  if(pospv_n > 0) pospv /= (float)pospv_n;
  if(negpv_n > 0) negpv /= (float)negpv_n;
  if(posstate_n > 0) posstate /= (float)posstate_n;
  if(negstate_n > 0) negstate /= (float)negstate_n;

  if(se.sub_pos) {
    negpv -= pospv;
    negstate -= posstate;
  }

  float incd = se.se_inc_dt * (1.0f - u->sev);
  float dapd = 0.0f;
  if(u->da_p > 0.0f) {
    dapd = se.da_pos_dt * u->da_p * (se.se_base - u->sev);
  }
  float dand = 0.0f;
  if(u->da_p < 0.0f) {
    dand = se.da_neg_dt * -u->da_p * (1.0f - u->sev);
  }
  float negpvd = se.se_pv_dt * (negpv - u->sev);
  float negstated = se.se_state_dt * (negstate - u->sev);

  u->sev += incd + dapd + dand + negpvd + negstated;

  if(u->sev < se.se_base) u->sev = se.se_base;
  if(u->sev > 1.0f) u->sev = 1.0f;

  u->ext = u->sev;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;
}

void DRNUnitSpec::Send_Se(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  const float snd_val = se.se_out_gain * u->sev;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitVars*)send_gp->UnVars(j,net))->sev = snd_val;
    }
  }
}

void DRNUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Se(u, net, thr_no);
  Send_Se(u, net, thr_no);
}

void DRNUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Se(u, net, thr_no);
  Send_Se(u, net, thr_no);
}

void DRNUnitSpec::Init_Weights(UnitVars* ru, Network* rnet, int thr_no) {
  inherited::Init_Weights(ru, rnet, thr_no);
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  u->sev = se.se_base;
}
