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

#include "TDRewIntegUnitSpec.h"
#include <LeabraNetwork>
#include <TDRewPredUnitSpec>
#include <ExtRewLayerSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TDRewIntegSpec);
TA_BASEFUNS_CTORS_DEFN(TDRewIntegUnitSpec);


void TDRewIntegSpec::Initialize() {
  discount = .8f;
  max_r_v = false;
}

void TDRewIntegUnitSpec::Initialize() {
  Defaults_init();
}

void TDRewIntegUnitSpec::Defaults_init() {
  SetUnique("act_range", true);
  act_range.min = -100.0f;
  act_range.max = 100.0f;
}

void TDRewIntegUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  rew_integ.UpdateAfterEdit_NoGui();
}

void TDRewIntegUnitSpec::HelpConfig() {
  String help = "TDRewIntegUnitSpec Computation:\n\
 Integrates reward predictions from TDRewPred layer, and external actual rewards from\
 ExtRew layer.  Plus-minus phase difference is td value.\n\
 - Minus phase = previous expected reward V^(t) copied directly from TDRewPred\n\
 - Plus phase = integration of ExtRew r(t) and new TDRewPred computing V^(t+1)).\n\
 - No learning.\n\
 \nTDRewIntegUnitSpec Configuration:\n\
 - Requires 2 input projections, from TDRewPred, ExtRew layers.\n\
 - Sending connection to TDDeltaUnitSpec(s) (marked with MarkerConSpec)\n\
 (to compute the td change in expected rewards as computed by this layer)\n\
 - UnitSpec for this layer must have act_range set to -100 and 100 \
     (because negative td = negative activation signal here)";
  taMisc::Confirm(help);
}

bool TDRewIntegUnitSpec::CheckConfig_Unit(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Unit(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // check for conspecs with correct params
  LeabraLayer* rew_pred_lay = NULL;
  LeabraLayer* ext_rew_lay = NULL;

  if(lay->units.leaves == 0) return false;
  LeabraUnit* un = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  
  const int nrg = un->NRecvConGps(); 
  for(int g=0; g< nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)un->RecvConState(g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(!cs->IsMarkerCon()) continue;
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(us->InheritsFrom(&TA_TDRewPredUnitSpec)) {
      rew_pred_lay = fmlay;
    }
    else if(fls->InheritsFrom(&TA_ExtRewLayerSpec)) {
      ext_rew_lay = fmlay;
    }
  }

  if(lay->CheckError(rew_pred_lay == NULL, quiet, rval,
                "requires recv projection from layer with TDRewPredUnitSpec units!")) {
    return false;
  }

  // not required..
  // int myidx = lay->own_net->layers.FindLeafEl(lay);
  // int rpidx = lay->own_net->layers.FindLeafEl(rew_pred_lay);
  // if(lay->CheckError(rpidx > myidx, quiet, rval,
  //               "reward prediction layer must be *before* this layer in list of layers -- it is now after, won't work")) {
  //   return false;
  // }

  if(lay->CheckError(ext_rew_lay == NULL, quiet, rval,
                "TD requires recv projection from layer with ExtRewLayerSpec!")) {
    return false;
  }
  // int eridx = lay->own_net->layers.FindLeafEl(ext_rew_lay);
  // if(lay->CheckError(eridx > myidx, quiet, rval,
  //               "external reward layer must be *before* this layer in list of layers -- it is now after, won't work")) {
  //   return false;
  // }
  return true;
}


void TDRewIntegUnitSpec::Compute_TDRewInteg(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  float rew_pred_val = 0.0f;
  float ext_rew_val = 0.0f;

  const int nrg = u->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(!cs->IsMarkerCon()) continue;
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(us->InheritsFrom(&TA_TDRewPredUnitSpec)) {
      rew_pred_val = fmlay->GetTotalActEq();
    }
    else if(fls->InheritsFrom(&TA_ExtRewLayerSpec)) {
      LeabraUnit* eru = (LeabraUnit*)fmlay->units.Leaf(0);
      ext_rew_val = eru->misc_1(); // readout is in misc_1
    }
  }

  float new_val;
  if(!Quarter_DeepRawNow(net->quarter)) { // plus phase marker..
    new_val = rew_pred_val; // no discount in minus phase!!!  should only reflect previous V^(t)
  }
  else {
    if(rew_integ.max_r_v) {
      new_val = fmaxf(rew_integ.discount * rew_pred_val, ext_rew_val);
    }
    else {
      new_val = rew_integ.discount * rew_pred_val + ext_rew_val; // now discount new rewpred!
    }
  }

  u->ext = new_val;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;
}

void TDRewIntegUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_TDRewInteg(u, net, thr_no);
}

void TDRewIntegUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}

