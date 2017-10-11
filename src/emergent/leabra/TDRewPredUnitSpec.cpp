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

#include "TDRewPredUnitSpec.h"
#include <LeabraNetwork>
#include <TDRewPredConSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TDRewPredUnitSpec);


void TDRewPredUnitSpec::Initialize() {
  Defaults_init();
}

void TDRewPredUnitSpec::Defaults_init() {
  SetUnique("act_range", true);
  act_range.min = -100.0f;
  act_range.max = 100.0f;
}

void TDRewPredUnitSpec::HelpConfig() {
  String help = "TDRewPredUnitSpec Computation:\n\
 Computes expected rewards according to the TD algorithm: predicts V(t+1) at time t. \n\
 - Minus phase = previous expected reward V^(t) clamped\
 - Plus phase = free-running expected reward computed (over settlng, fm recv wts)\n\
 - Learning is da_p * act_q0: dopamine from TDDeltaUnitSpec times sender activations at (t-1).\n\
 \nTDRewPredUnitSpec Configuration:\n\
 - Sending connection to a TDRewIntegUnitSpec to integrate predictions with external rewards\n\
 - Recv connection from TDDeltaUnitSpec to receive da_p TD training signal\n\
 - UnitSpec for this layer must have act_range set to -100 and 100 \
     (because negative td = negative activation signal here)";
  taMisc::Confirm(help);
}

bool TDRewPredUnitSpec::CheckConfig_Unit(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Unit(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // check for conspecs with correct params
  if(lay->units.leaves == 0) return rval;
  LeabraUnit* un = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  
  const int nrg = un->NRecvConGps(); 
  for(int g=0; g< nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)un->RecvConState(g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(cs->IsMarkerCon()) continue;
    if(lay->CheckError(!cs->InheritsFrom(TA_TDRewPredConSpec), quiet, rval,
                  "requires recv connections to be of type TDRewPredConSpec")) {
      return false;
    }
  }
  return true;
}

void TDRewPredUnitSpec::Init_Acts(UnitState* ru, Network* rnet, int thr_no) {
  LeabraUnitState_cpp* u = (LeabraUnitState_cpp*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  inherited::Init_Acts(u, net, thr_no);
  u->misc_1 = 0.0f;             // reset..
}
  
void TDRewPredUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  if(!Quarter_DeepRawNow(net->quarter)) { // plus phase marker..
    u->ext = u->misc_1;                  // clamp to previous prediction
    u->act_eq = u->act_nd = u->act = u->net = u->ext;
    u->da = 0.0f;
  }
  else {
    u->act_eq = u->act_nd = u->act = u->net;            // linear!
    u->da = 0.0f;
  }
}

void TDRewPredUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}

void TDRewPredUnitSpec::Quarter_Final(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  if(Quarter_DeepRawNow(net->quarter)) { // plus phase marker..
    u->misc_1 = u->act_eq;               // save current prediction for next trial
  }
  inherited::Quarter_Final(u, net, thr_no); // this will record as act_p
}

