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

#include "TDRewIntegLayerSpec.h"
#include <LeabraNetwork>
#include <MarkerConSpec>
#include <LeabraTdUnit>
#include <LeabraTdUnitSpec>
#include <TDRewPredLayerSpec>
#include <ExtRewLayerSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TDRewIntegSpec);

TA_BASEFUNS_CTORS_DEFN(TDRewIntegLayerSpec);


void TDRewIntegSpec::Initialize() {
  discount = .8f;
  max_r_v = false;
}

void TDRewIntegLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase2 = 0.0f;
  unit_range.min = 0.0f;
  unit_range.max = 3.0f;
  unit_range.UpdateAfterEdit_NoGui();
  val_range.UpdateAfterEdit_NoGui();
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void TDRewIntegLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  rew_integ.UpdateAfterEdit_NoGui();
}

void TDRewIntegLayerSpec::HelpConfig() {
  String help = "TDRewIntegLayerSpec Computation:\n\
 Integrates reward predictions from TDRewPred layer, and external actual rewards from\
 ExtRew layer.  Plus-minus phase difference is td value.\n\
 - Minus phase = previous expected reward V^(t) copied directly from TDRewPred\n\
 - Plus phase = integration of ExtRew r(t) and new TDRewPred computing V^(t+1)).\n\
 - No learning.\n\
 \nTDRewIntegLayerSpec Configuration:\n\
 - Requires 2 input projections, from TDRewPred, ExtRew layers.\n\
 - Sending connection to TdLayerSpec(s) (marked with MarkerConSpec)\n\
 (to compute the td change in expected rewards as computed by this layer)\n\
 - This layer must be before  TdLayerSpec layer in list of layers\n\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool TDRewIntegLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(!lay->units.el_typ->InheritsFrom(TA_LeabraTdUnit), quiet, rval,
                "must have LeabraTdUnits!")) {
    return false;
  }

  SetUnique("decay", true);
  decay.phase2 = 0.0f;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(!us->InheritsFrom(TA_LeabraTdUnitSpec), quiet, rval,
                "UnitSpec must be LeabraTdUnitSpec!")) {
    return false;
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* rew_pred_lay = NULL;
  LeabraLayer* ext_rew_lay = NULL;

  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.SPtr();
    if(fls->InheritsFrom(&TA_TDRewPredLayerSpec)) {
      rew_pred_lay = flay;
    }
    else if(fls->InheritsFrom(&TA_ExtRewLayerSpec)) {
      ext_rew_lay = flay;
    }
  }

  if(lay->CheckError(rew_pred_lay == NULL, quiet, rval,
                "requires recv projection from TDRewPredLayerSpec!")) {
    return false;
  }
  int myidx = lay->own_net->layers.FindLeafEl(lay);
  int rpidx = lay->own_net->layers.FindLeafEl(rew_pred_lay);
  if(lay->CheckError(rpidx > myidx, quiet, rval,
                "reward prediction layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }

  if(lay->CheckError(ext_rew_lay == NULL, quiet, rval,
                "TD requires recv projection from ExtRewLayerSpec!")) {
    return false;
  }
  int eridx = lay->own_net->layers.FindLeafEl(ext_rew_lay);
  if(lay->CheckError(eridx > myidx, quiet, rval,
                "external reward layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }
  return true;
}

// this is last stage before compute_act, so doing computation here
void TDRewIntegLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  lay->SetExtFlag(Unit::EXT);

  float rew_pred_val = 0.0f;
  float ext_rew_val = 0.0f;
  bool ext_rew_avail = true;

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(!recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
    LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.SPtr();
    if(fls->InheritsFrom(&TA_TDRewPredLayerSpec)) {
      LeabraUnit* rpu = (LeabraUnit*)flay->units.Leaf(0); // todo; base on connections..
      rew_pred_val = rpu->act_eq; // use current input
    }
    else if(fls->InheritsFrom(&TA_ExtRewLayerSpec)) {
      LeabraUnit* eru = (LeabraUnit*)flay->units.Leaf(0);
      ext_rew_val = eru->act_eq;
      if(flay->acts.max < .1f)  // indication of no reward available!
        ext_rew_avail = false;
    }
  }

  float new_val;
  if(net->phase_no == 0) {
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

  UNIT_GP_ITR(lay,
              LeabraTdUnit* u = (LeabraTdUnit*)lay->UnitAccess(acc_md, 0, gpidx);
              u->ext = new_val;
              ClampValue_ugp(lay, acc_md, gpidx, net);
              );
  HardClampExt(lay, net);
}
