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

#include "TDRewPredLayerSpec.h"
#include <LeabraNetwork>
#include <MarkerConSpec>
#include <LeabraTdUnit>
#include <LeabraTdUnitSpec>
#include <TDRewPredConSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TDRewPredLayerSpec);


void TDRewPredLayerSpec::Initialize() {
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

void TDRewPredLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void TDRewPredLayerSpec::HelpConfig() {
  String help = "TDRewPredLayerSpec Computation:\n\
 Computes expected rewards according to the TD algorithm: predicts V(t+1) at time t. \n\
 - Minus phase = previous expected reward V^(t) clamped\
 - Plus phase = free-running expected reward computed (over settlng, fm recv wts)\n\
 - Learning is (act_p - act_m) * p_act_p: delta on recv units times sender activations at (t-1).\n\
 \nTDRewPredLayerSpec Configuration:\n\
 - All units I recv from must be LeabraTdUnit/Spec units (to hold t-1 act vals)\n\
 - Sending connection to a TDRewIntegLayerSpec to integrate predictions with external rewards";
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool TDRewPredLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
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
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_TDRewPredConSpec), quiet, rval,
                  "requires recv connections to be of type TDRewPredConSpec")) {
      return false;
    }
    if(lay->CheckError(cs->wt_limits.sym != false, quiet, rval,
                  "requires recv connections to have wt_limits.sym=false, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      cs->SetUnique("wt_limits", true);
      cs->wt_limits.sym = false;
    }
  }
  return true;
}

void TDRewPredLayerSpec::Init_Acts(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Acts(lay, net);
  // initialize the misc_1 variable to 0.0 -- no prior predictions!
  UNIT_GP_ITR(lay,
              LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
              u->misc_1 = 0.0f;
              );
}

void TDRewPredLayerSpec::Compute_SavePred(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                          LeabraNetwork*) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->misc_1 = u->act_eq;
  }
}

void TDRewPredLayerSpec::Compute_ClampPred(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                           LeabraNetwork*) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->ext = u->misc_1;
    u->SetExtFlag(Unit::EXT);
  }
}

void TDRewPredLayerSpec::Compute_ClampPrev(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, Compute_ClampPred(lay, acc_md, gpidx, net); );
}

void TDRewPredLayerSpec::Compute_TdPlusPhase_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
                                                 int gpidx, LeabraNetwork* net) {
  Compute_SavePred(lay, acc_md, gpidx, net);    // first, always save current predictions!

  LeabraTdUnit* u = (LeabraTdUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraTdUnit* u2 = (LeabraTdUnit*)lay->UnitAccess(acc_md, 1, gpidx);
  u->ext = u->act_m + u2->dav;
  ClampValue_ugp(lay, acc_md, gpidx, net);              // apply new value
  Compute_ExtToPlus_ugp(lay, acc_md, gpidx, net);       // copy ext values to act_p
}

void TDRewPredLayerSpec::Compute_TdPlusPhase(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, Compute_TdPlusPhase_ugp(lay, acc_md, gpidx, net); );
}

void TDRewPredLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);
  if(net->phase_no < net->phase_max-1)
    return; // only at very last phase, do this!  see note on Compute_dWt as to why..
  Compute_TdPlusPhase(lay, net);
}

void TDRewPredLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0) {
    lay->SetExtFlag(Unit::EXT);
    Compute_ClampPrev(lay, net);
    HardClampExt(lay, net);
  }
  else {
    lay->hard_clamped = false;  // run free: generate prediction of future reward
    lay->Init_InputData(net);
  }
}

bool TDRewPredLayerSpec::Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  // doing second because act_p is computed only at end of settling!
  // this is better than clamping the value in the middle of everything
  // and then continuing with settling..
  if(net->phase_no < net->phase_max-1)
    return false; // only do FINAL dwt!
  return true;
}
