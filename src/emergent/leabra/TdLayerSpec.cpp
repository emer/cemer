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

#include "TdLayerSpec.h"
#include <LeabraNetwork>
#include <MarkerConSpec>
#include <LeabraTdUnit>
#include <LeabraTdUnitSpec>
#include <TDRewIntegLayerSpec>
#include <OneToOnePrjnSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TdLayerSpec);


void TdLayerSpec::Initialize() {
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

void TdLayerSpec::HelpConfig() {
  String help = "TdLayerSpec Computation:\n\
 - act of unit(s) = act_dif of unit(s) in reward integration layer we recv from\n\
 - td is dynamically computed in plus phaes and sent all layers that recv from us\n\
 - No Learning\n\
 \nTdLayerSpec Configuration:\n\
 - Single recv connection marked with a MarkerConSpec from reward integration layer\
     (computes expectations and actual reward signals)\n\
 - This layer must be after corresp. reward integration layer in list of layers\n\
 - Sending connections must connect to units of type LeabraTdUnit/Spec \
     (td signal from this layer put directly into td var on units)\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative td = negative activation signal here";
  taMisc::Confirm(help);
}

bool TdLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // must have the appropriate ranges for unit specs..
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError((us->act_range.max != 2.0f) || (us->act_range.min != -2.0f), quiet, rval,
                "requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)") ) {
    us->SetUnique("act_range", true);
    us->act_range.max = 2.0f;
    us->act_range.min = -2.0f;
    us->act_range.UpdateAfterEdit();
  }
  if(lay->CheckError((us->clamp_range.max != 2.0f) || (us->clamp_range.min != -2.0f), quiet, rval,
                "requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("clamp_range", true);
    us->clamp_range.max = 2.0f;
    us->clamp_range.min = -2.0f;
    us->clamp_range.UpdateAfterEdit();
  }

  // check recv connection
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  LeabraLayer* rewinteg_lay = NULL;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(lay->CheckError(fmlay == NULL, quiet, rval,
                  "null from layer in recv projection:", (String)g)) {
      return false;
    }
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)
        && fmlay->spec.SPtr()->InheritsFrom(TA_TDRewIntegLayerSpec)) {
      rewinteg_lay = fmlay;
      // if(lay->CheckError(recv_gp->size <= 0, quiet, rval,
      //                    "requires one recv projection with at least one unit!")) {
      //   return false;
      // }
      if(recv_gp->size <= 0) {
        OneToOnePrjnSpec* pspec = (OneToOnePrjnSpec*)recv_gp->prjn->spec.SPtr();
        pspec->send_start = 1;
        if(lay->CheckError(true, quiet, rval,
                           "requires the OneToOnePrjnSpec to have send_start = 1 -- I just set this for you, but you will have to re-build the network and re-init -- save project after this change")) {
        }
      }
      else {
        if(lay->CheckError(!recv_gp->Un(0,net)->InheritsFrom(TA_LeabraTdUnit), quiet, rval,
                           "I need to receive from a LeabraTdUnit!")) {
          return false;
        }
      }
    }
  }

  if(lay->CheckError(rewinteg_lay == NULL, quiet, rval,
                "did not find TDRewInteg layer to get Td from!")) {
    return false;
  }

  int myidx = lay->own_net->layers.FindLeafEl(lay);
  int rpidx = lay->own_net->layers.FindLeafEl(rewinteg_lay);
  if(lay->CheckError(rpidx > myidx, quiet, rval,
                "reward integration layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }

  // check sending layer projections for appropriate unit types
  int si;
  for(si=0;si<lay->send_prjns.size;si++) {
    Projection* prjn = (Projection*)lay->send_prjns[si];
    if(lay->CheckError(!prjn->from->units.el_typ->InheritsFrom(TA_LeabraTdUnit), quiet, rval,
                  "all layers I send to must have LeabraTdUnits!, layer:",
                  prjn->from->GetPath(),"doesn't")) {
      return false;
    }
  }
  return true;
}

void TdLayerSpec::Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork*) {
  lay->dav = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    u->ext = 0.0f;
    u->SetExtFlag(Unit::EXT);
  }
}

void TdLayerSpec::Compute_Td(LeabraLayer* lay, LeabraNetwork* net) {
  int ri_prjn_idx;
  LeabraLayer* ri_lay = FindLayerFmSpec(lay, ri_prjn_idx, &TA_TDRewIntegLayerSpec);
  // just taking the first unit = scalar val
  LeabraTdUnit* su = (LeabraTdUnit*)ri_lay->units.SafeEl(0);

  lay->dav = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraTdUnit, u, lay->units) {
    u->dav = su->act_eq - su->act_m; // subtract current minus previous!
    u->ext = u->dav;
    u->act_lrn = u->act_eq = u->act_nd = u->act = u->net = u->ext;
    lay->dav += u->dav;
  }
  if(lay->units.leaves > 0) lay->dav /= (float)lay->units.leaves;
}

void TdLayerSpec::Send_Td(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned())       continue;
      for(int j=0;j<send_gp->size; j++) {
        ((LeabraTdUnit*)send_gp->Un(j,net))->dav = u->act;
      }
    }
  }
}

void TdLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_Td(lay, net); // now get the td and clamp it to layer
  Send_Td(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void TdLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0) {
    lay->hard_clamped = true;
    lay->SetExtFlag(Unit::EXT);
    lay->Inhib_SetVals(inhib.kwta_pt); // assume 0 - 1 clamped inputs
    Compute_ZeroAct(lay, net);  // can't do anything during settle anyway -- just zero it
  }
  else {
    // run "free" in plus phase: compute act = td
    lay->hard_clamped = false;
    lay->UnSetExtFlag(Unit::EXT);
  }
  inherited::Compute_HardClamp(lay, net);
}

