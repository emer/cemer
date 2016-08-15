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

#include "TDRewPredUnitSpec.h"
#include <LeabraNetwork>
#include <TDRewPredConSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TDRewPredUnitSpec);


void TDRewPredUnitSpec::Initialize() {
  // ??
}

void TDRewPredUnitSpec::HelpConfig() {
  String help = "TDRewPredUnitSpec Computation:\n\
 Computes expected rewards according to the TD algorithm: predicts V(t+1) at time t. \n\
 - Minus phase = previous expected reward V^(t) clamped\
 - Plus phase = free-running expected reward computed (over settlng, fm recv wts)\n\
 - Learning is (act_p - act_m) * act_q0: delta on recv units times sender activations at (t-1).\n\
 \nTDRewPredUnitSpec Configuration:\n\
 - Sending connection to a TDRewIntegUnitSpec to integrate predictions with external rewards\n\
 - Recv connection from TDDeltaUnitspec to receive da_p TD training signal";
  taMisc::Confirm(help);
  // inherited::HelpConfig();
}

// bool TDRewPredUnitSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
//   LeabraLayer* lay = (LeabraLayer*)ly;
//   if(!inherited::CheckConfig_Layer(lay, quiet))
//     return false;

// //  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
//   bool rval = true;

//   if(lay->CheckError(!lay->units.el_typ->InheritsFrom(TA_LeabraTdUnit), quiet, rval,
//                 "must have LeabraTdUnits!")) {
//     return false;
//   }

//   LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
//   if(lay->CheckError(!us->InheritsFrom(TA_LeabraTdUnitSpec), quiet, rval,
//                 "UnitSpec must be LeabraTdUnitSpec!")) {
//     return false;
//   }
//   us->UpdateAfterEdit();

//   // check for conspecs with correct params
//   if(lay->units.leaves == 0) return false;
//   LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
//   for(int g=0; g<u->recv.size; g++) {
//     LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
//     if(recv_gp->NotActive()) continue;
//     if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) continue; // self projection
//     if(recv_gp->GetConSpec()->IsMarkerCon()) continue;
//     LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
//     if(lay->CheckError(!cs->InheritsFrom(TA_TDRewPredConSpec), quiet, rval,
//                   "requires recv connections to be of type TDRewPredConSpec")) {
//       return false;
//     }
//     if(lay->CheckError(cs->wt_limits.sym != false, quiet, rval,
//                   "requires recv connections to have wt_limits.sym=false, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
//       cs->SetUnique("wt_limits", true);
//       cs->wt_limits.sym = false;
//     }
//   }
//   return true;
// }

void TDRewPredUnitSpec::Init_Acts(UnitVars* ru, Network* rnet, int thr_no) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  inherited::Init_Acts(u, net, thr_no);
  u->misc_1 = 0.0f;             // reset..
}
  
void TDRewPredUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(Quarter_DeepRawNow(net->quarter)) { // plus phase marker..
    u->ext = u->misc_1;                  // clamp to previous prediction
    u->act_eq = u->act_nd = u->act = u->net = u->ext;
    u->da = 0.0f;
  }
  else {
    inherited::Compute_Act_Rate(u, net, thr_no);
  }
}

void TDRewPredUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}

void TDRewPredUnitSpec::Quarter_Final(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->quarter == 3) {       // plus phase
    float cur_pred = u->act_eq;
    float prev_pred = u->misc_1;
    float train_act = prev_pred + u->da_p; // plus phase training = previous pred + da
    // i.e., just da.. not clear we need this but just do it..
    u->misc_1 = cur_pred;              // now save cur for next time
    u->act_nd = u->act_eq = train_act; // this will be act_p either way..
  }
  inherited::Quarter_Final(u, net, thr_no); // this will record as act_p
}

