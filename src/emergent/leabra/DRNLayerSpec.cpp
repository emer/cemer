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

#include "DRNLayerSpec.h"

#include <LeabraNetwork>
#include <MarkerConSpec>
#include <OneToOnePrjnSpec>
#include <PPTgUnitSpec>
#include <LHbRMTgUnitSpec>
#include <LearnDriverLayerSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DRNLayerSpec);

TA_BASEFUNS_CTORS_DEFN(DRN5htSpec);

void DRN5htSpec::Initialize() {
  se_base = 0.1f;
  se_gain = 1.0f;
  se_pv_dt = 0.05f;
  se_state_dt = 0.005f;
  sub_pos = false;
}

void DRNLayerSpec::Initialize() {
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 1;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = .25;
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void DRNLayerSpec::HelpConfig() {
  String help = "DRNLayerSpec (5HT serotonin value) Computation:\n\
 - Computes SE value based on inputs from PV and State layers.\n\
 - No Learning\n\
 \nDRNLayerSpec Configuration:\n\
 - Use the Wizard gdPVLV button to automatically configure layers.\n\
 - Recv cons marked with a MarkerConSpec from inputs";
  taMisc::Confirm(help);
}

bool DRNLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // must have the appropriate ranges for unit specs..
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  us->SetUnique("maxda", true);
  us->maxda.val = MaxDaSpec::NO_MAX_DA;

  // if(lay->CheckError((us->act_range.max != 2.0f) || (us->act_range.min != -2.0f), quiet, rval,
  //               "requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:",
  //               us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
  //   us->SetUnique("act_range", true);
  //   us->act_range.max = 2.0f;
  //   us->act_range.min = -2.0f;
  //   us->act_range.UpdateAfterEdit();
  // }
  // if(lay->CheckError((us->clamp_range.max != 2.0f) || (us->clamp_range.min != -2.0f), quiet, rval,
  //               "requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:",
  //               us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
  //   us->SetUnique("clamp_range", true);
  //   us->clamp_range.max = 2.0f;
  //   us->clamp_range.min = -2.0f;
  //   us->clamp_range.UpdateAfterEdit();
  // }

  return rval;
}

void DRNLayerSpec::Compute_Se(LeabraLayer* lay, LeabraNetwork* net) {
  float pospv = 0.0f;
  int   pospv_n  = 0;
  float negpv = 0.0f;
  int   negpv_n  = 0;
  float posstate = 0.0f;
  int   posstate_n = 0;
  float negstate = 0.0f;
  int   negstate_n = 0;

  if(lay->units.leaves == 0) return;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* from = (LeabraLayer*)recv_gp->prjn->from.ptr();
    const float act_avg = from->acts.avg;
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

  if(pospv_n > 0) pospv /= (float)pospv_n;
  if(negpv_n > 0) negpv /= (float)negpv_n;
  if(posstate_n > 0) posstate /= (float)posstate_n;
  if(negstate_n > 0) negstate /= (float)negstate_n;

  if(se.sub_pos) {
    negpv -= pospv;
    negstate -= posstate;
  }

  float negpvd = se.se_pv_dt * (negpv - lay->sev);
  float negstated = se.se_state_dt * (negstate - lay->sev);

  lay->sev += negpvd + negstated;
  if(lay->sev < se.se_base) lay->sev = se.se_base;

  net->pvlv_sev = lay->sev;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->sev = lay->sev;
    u->ext = u->sev;
    u->act_lrn = u->act_eq = u->act_nd = u->act = u->net = u->ext;
    u->da = 0.0f;
  }
}

void DRNLayerSpec::Send_Se(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    const float snd_val = u->act;
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned())       continue;
      for(int j=0;j<send_gp->size; j++) {
        ((LeabraUnit*)send_gp->Un(j,net))->sev = snd_val;
      }
    }
  }
}

void DRNLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_Se(lay, net);
  Send_Se(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void DRNLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  lay->sev = se.se_base;
}
