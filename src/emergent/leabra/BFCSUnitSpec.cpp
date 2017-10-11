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

#include "BFCSUnitSpec.h"

#include <LeabraNetwork>
#include <CElAmygUnitSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(BFCSAChSpec);
TA_BASEFUNS_CTORS_DEFN(BFCSUnitSpec);

void BFCSAChSpec::Initialize() {
  Defaults_init();
}

void BFCSAChSpec::Defaults_init() {
  tonic_ach = 0.5f;
  tau = 10.0f;
  cea_gain = 1.0f;
  vs_gain = 1.0f;
  dt = 1.0f / tau;
}

void BFCSAChSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dt = 1.0f / tau;
}

void BFCSUnitSpec::Initialize() {
  // SetUnique("deep_raw_qtr", true);
  // deep_raw_qtr = Q4;
  // SetUnique("act_range", true);
  // act_range.max = 2.0f;
  // act_range.min = -2.0f;
  // act_range.UpdateAfterEdit();
  // SetUnique("clamp_range", true);
  // clamp_range.max = 2.0f;
  // clamp_range.min = -2.0f;
  // clamp_range.UpdateAfterEdit();
}

void BFCSUnitSpec::HelpConfig() {
  String help = "BFCSUnitSpec (ACh value) Computation:\n\
 - Computes ACh value based on inputs from CEL, VS.\n\
 - No Learning\n\
 \nBFCSUnitSpec Configuration:\n\
 - Use the Wizard PVLV button to automatically configure layers.\n\
 - Recv cons marked with a MarkerConSpec from inputs";
  taMisc::Confirm(help);
}

bool BFCSUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;
  bool rval = true;

  if(lay->units.leaves == 0) return rval;
  // LeabraUnit* un = (LeabraUnit*)lay->units.Leaf(0); // take first one
  
  // if(lay->CheckError((act_range.max != 2.0f) || (act_range.min != -2.0f), quiet, rval,
  //               "requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:",
  //               name,"(make sure this is appropriate for all layers that use this spec!)")) {
  //   SetUnique("act_range", true);
  //   act_range.max = 2.0f;
  //   act_range.min = -2.0f;
  //   act_range.UpdateAfterEdit();
  // }
  // if(lay->CheckError((clamp_range.max != 2.0f) || (clamp_range.min != -2.0f), quiet, rval,
  //               "requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:",
  //               name,"(make sure this is appropriate for all layers that use this spec!)")) {
  //   SetUnique("clamp_range", true);
  //   clamp_range.max = 2.0f;
  //   clamp_range.min = -2.0f;
  //   clamp_range.UpdateAfterEdit();
  // }
  return rval;
}

void BFCSUnitSpec::Compute_ACh(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  float sum_delta = 0.0f;
  int sum_n = 0;
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    // LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->IsMarkerCon()) {
      float avg_del = 0.0f;
      if(us->InheritsFrom(&TA_CElAmygUnitSpec)) {
        for(int j=0;j<recv_gp->size; j++) {
          LeabraUnitState_cpp* suv = (LeabraUnitState_cpp*)recv_gp->UnState(j,net);
          float del = suv->act_eq - suv->act_q0; // trial level delta
          if(del < 0.0f) del = 0.0f;             // positive rectification!?
          avg_del += fabsf(del);
        }
        avg_del *= ach.cea_gain;
      }
      else {                    // assume vs, within-trial delta
        for(int j=0;j<recv_gp->size; j++) {
          LeabraUnitState_cpp* suv = (LeabraUnitState_cpp*)recv_gp->UnState(j,net);
          float del = suv->act_eq * suv->da_p; // act * dopamine!
          // if(del < 0.0f) del = 0.0f;             // positive rectification!
          avg_del += fabsf(del);
        }
        avg_del *= ach.vs_gain;
      }
      avg_del /= (float)recv_gp->size;
      sum_delta += avg_del;
      sum_n++;
    }
  }

  float avg_delta = 0.0f;
  if(sum_n > 0) {
    avg_delta = sum_delta / (float)sum_n;
  }

  u->misc_2 = avg_delta;
  u->misc_1 += ach.dt * (avg_delta - u->misc_1); // time integrate
  u->ach = u->misc_1;

  // if(da.rec_data) {
  //   // lay->SetUserData("negpv", negpv, false); // false=no update
  // }
}

void BFCSUnitSpec::Compute_ActTimeAvg(LeabraUnitState_cpp* uv, LeabraNetwork* net, int thr_no) {
  inherited::Compute_ActTimeAvg(uv, net, thr_no);
  Compute_ACh(uv, net, thr_no);
}

void BFCSUnitSpec::Quarter_Init_Unit(LeabraUnitState_cpp* uv, LeabraNetwork* net, int thr_no) {
  inherited::Quarter_Init_Unit(uv, net, thr_no);
  Send_ACh(uv, net, thr_no);
}


void BFCSUnitSpec::Send_ACh(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  float snd_val = u->misc_1;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->SendConState(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitState_cpp*)send_gp->UnState(j,net))->ach = snd_val;
    }
  }
}

void BFCSUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  u->act_eq = u->act_nd = u->act = u->net = u->misc_1;
  u->da = 0.0f;
}

void BFCSUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}

