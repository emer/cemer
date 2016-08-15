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
 - This layer must be before  TDDeltaUnitSpec layer in list of layers\n\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  taMisc::Confirm(help);
  // inherited::HelpConfig();
}

// bool TDRewIntegUnitSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
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
//   LeabraLayer* rew_pred_lay = NULL;
//   LeabraLayer* ext_rew_lay = NULL;

//   if(lay->units.leaves == 0) return false;
//   LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
//   for(int g=0; g<u->recv.size; g++) {
//     LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
//     if(recv_gp->NotActive()) continue;
//     if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
//       continue;
//     }
//     LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
//     LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.SPtr();
//     if(fls->InheritsFrom(&TA_TDRewPredUnitSpec)) {
//       rew_pred_lay = flay;
//     }
//     else if(fls->InheritsFrom(&TA_ExtRewLayerSpec)) {
//       ext_rew_lay = flay;
//     }
//   }

//   if(lay->CheckError(rew_pred_lay == NULL, quiet, rval,
//                 "requires recv projection from TDRewPredUnitSpec!")) {
//     return false;
//   }
//   int myidx = lay->own_net->layers.FindLeafEl(lay);
//   int rpidx = lay->own_net->layers.FindLeafEl(rew_pred_lay);
//   if(lay->CheckError(rpidx > myidx, quiet, rval,
//                 "reward prediction layer must be *before* this layer in list of layers -- it is now after, won't work")) {
//     return false;
//   }

//   if(lay->CheckError(ext_rew_lay == NULL, quiet, rval,
//                 "TD requires recv projection from ExtRewLayerSpec!")) {
//     return false;
//   }
//   int eridx = lay->own_net->layers.FindLeafEl(ext_rew_lay);
//   if(lay->CheckError(eridx > myidx, quiet, rval,
//                 "external reward layer must be *before* this layer in list of layers -- it is now after, won't work")) {
//     return false;
//   }
//   return true;
// }


void TDRewIntegUnitSpec::Compute_TDRewInteg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float rew_pred_val = 0.0f;
  float ext_rew_val = 0.0f;

  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->IsMarkerCon()) {
      if(us->InheritsFrom(&TA_TDRewPredUnitSpec)) {
        rew_pred_val = fmlay->GetTotalActEq();
      }
      else if(fls->InheritsFrom(&TA_ExtRewLayerSpec)) {
        LeabraUnit* eru = (LeabraUnit*)fmlay->units.Leaf(0);
        ext_rew_val = eru->misc_1(); // readout is in misc_1
      }
    }
  }

  float new_val;
  if(net->phase == LeabraNetwork::MINUS_PHASE) {
    new_val = rew_pred_val; // no discount in minus phase!!!  should only reflect previous V^(t)
  }
  else {
    if(rew_integ.max_r_v) {
      new_val = MAX(rew_integ.discount * rew_pred_val, ext_rew_val);
    }
    else {
      new_val = rew_integ.discount * rew_pred_val + ext_rew_val; // now discount new rewpred!
    }
  }

  u->ext = new_val;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;
}

void TDRewIntegUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_TDRewInteg(u, net, thr_no);
}

void TDRewIntegUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}

