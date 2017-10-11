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

#include "TDDeltaUnitSpec.h"
#include <LeabraNetwork>
#include <TDRewIntegUnitSpec>
#include <OneToOnePrjnSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TDDeltaUnitSpec);


void TDDeltaUnitSpec::Initialize() {
  Defaults_init();
}

void TDDeltaUnitSpec::Defaults_init() {
  SetUnique("act_range", true);
  act_range.min = -100.0f;
  act_range.max = 100.0f;
}

void TDDeltaUnitSpec::HelpConfig() {
  String help = "TDDeltaUnitSpec Computation:\n\
 - act of unit(s) = act_dif of unit(s) in reward integration layer we recv from\n\
 - td is dynamically computed in plus phaes and sent all layers that recv from us\n\
 - No Learning\n\
 \nTDDeltaUnitSpec Configuration:\n\
 - Single recv connection marked with a MarkerConSpec from reward integration layer\
     (computes expectations and actual reward signals)\n\
 - This layer must be after corresp. reward integration layer in list of layers\n\
 - Sending connections must connect to units of type LeabraTdUnit/Spec \
     (td signal from this layer put directly into td var on units)\n\
 - UnitSpec for this layer must have act_range set to -100 and 100 \
     (because negative td = negative activation signal here)";
  taMisc::Confirm(help);
}

bool TDDeltaUnitSpec::CheckConfig_Unit(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // check recv connection
  if(lay->units.leaves == 0) return false;
  LeabraUnit* un = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  
  LeabraLayer* rewinteg_lay = NULL;
  
  const int nrg = un->NRecvConGps(); 
  for(int g=0; g< nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)un->RecvConState(g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(!cs->IsMarkerCon()) continue;
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(us->InheritsFrom(TA_TDRewIntegUnitSpec)) {
      rewinteg_lay = fmlay;
    }
  }

  if(lay->CheckError(rewinteg_lay == NULL, quiet, rval,
                "did not find layer with TDRewIntegUnitSpec units to get TD from!")) {
    return false;
  }

  // int myidx = lay->own_net->layers.FindLeafEl(lay);
  // int rpidx = lay->own_net->layers.FindLeafEl(rewinteg_lay);
  // if(lay->CheckError(rpidx > myidx, quiet, rval,
  //               "reward integration layer must be *before* this layer in list of layers -- it is now after, won't work")) {
  //   return false;
  // }
  return true;
}

void TDDeltaUnitSpec::Compute_TD(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  if(!Quarter_DeepRawNow(net->quarter)) { // plus phase marker..
    u->da_p = 0.0f;
    u->ext = u->da_p;
    u->act_eq = u->act_nd = u->act = u->net = u->ext;
    return;
  }
  float rew_integ_minus = 0.0f;
  float rew_integ_cur = 0.0f;
  const int nrg = u->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->IsMarkerCon()) {
      if(us->InheritsFrom(&TA_TDRewIntegUnitSpec)) {
        LeabraUnit* su = (LeabraUnit*)fmlay->units.SafeEl(0);
        rew_integ_minus = su->act_m();
        rew_integ_cur = su->act_eq();
        break;
      }
    }
  }
  float delta = rew_integ_cur - rew_integ_minus;
  u->da_p = delta;
  u->ext = u->da_p;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
}

void TDDeltaUnitSpec::Send_TD(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  float snd_val = u->act_eq;
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitState_cpp*)send_gp->UnState(j,net))->da_p = snd_val;
    }
  }
}

void TDDeltaUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_TD(u, net, thr_no);
}

void TDDeltaUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}

void TDDeltaUnitSpec::Compute_Act_Post(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Post(u, net, thr_no);
  Send_TD(u, net, thr_no);      // note: can only send modulators during post!!
}

