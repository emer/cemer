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

#include "ECoutLayerSpec.h"
#include <LeabraNetwork>
#include <ECinLayerSpec>

void ECoutLayerSpec::Initialize() {
  Defaults_init();
}

void ECoutLayerSpec::Defaults_init() {
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = 0.25f;
  SetUnique("clamp", true);
  clamp.max_plus = true;
}

bool ECoutLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  bool rval = true;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  if(net) {
    if(TestWarning(net->min_cycles < auto_m_cycles + 20, "CheckConfig",
                   "ECoutLayerSpec: setting network min_cycles to be auto_m_cycles + 20 to ensure minimum amount of time to settle")) {
      net->min_cycles = auto_m_cycles + 20;
    }
    if(TestWarning((net->learn_rule != LeabraNetwork::LEABRA_CHL) &&
                   (net->ct_time.minus < auto_m_cycles + 20), "CheckConfig",
                   "ECoutLayerSpec: setting network ct_time.minus to be auto_m_cycles + 20 to ensure minimum amount of time to settle")) {
      net->ct_time.minus = auto_m_cycles + 20;
    }
  }

  int in_prjn_idx;
  LeabraLayer* in_lay = FindLayerFmSpec(lay, in_prjn_idx, &TA_ECinLayerSpec);
  if(!in_lay) return true;		// just hope it works out
  // if(lay->CheckError(!in_lay, quiet, rval,
  //               "no projection from ECin Layer found: must recv from layer with ECinLayerSpec!")) {
  //   return false;
  // }
  if(in_lay->unit_groups) {
    if(lay->CheckError(in_lay->gp_geom.n != lay->gp_geom.n, quiet, rval,
                       "ECout Layer unit groups must = ECinLayer unit groups, copiped from IN Layer; Please do a Build of network")) {
      lay->unit_groups = true;
      lay->gp_geom.n = in_lay->gp_geom.n;
    }
  }
  if(lay->CheckError(in_lay->un_geom.n != lay->un_geom.n, quiet, rval,
                "ECout Layer units must = ECinLayer units, copied from IN Layer; Please do a Build of network")) {
    lay->un_geom = in_lay->un_geom;
  }

  return true;
}

void ECoutLayerSpec::ClampFromECin(LeabraLayer* lay, LeabraNetwork* net) {
  int in_prjn_idx;
  LeabraLayer* in_lay = FindLayerFmSpec(lay, in_prjn_idx, &TA_ECinLayerSpec);
  if(!in_lay) return;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)lay->GetUnitSpec();
  int nunits = MIN(in_lay->units.leaves, lay->units.leaves);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* ru = (LeabraUnit*)lay->units.Leaf(i);
    LeabraUnit* inu = (LeabraUnit*)in_lay->units.Leaf(i);
    float inval = inu->act_eq;
    if(clamp.max_plus) {
      float min_max = MAX(lay->acts_m.max, lay->acts_m2.max); // consider auto enc max too -- esp if doing pretraining on encoder only, this is important
      float clmp = min_max + clamp.plus;
      clmp = MAX(clmp, clamp.min_clamp);
      inval *= clmp;            // downscale
    }
    ru->act = rus->clamp_range.Clip(inval);
    ru->act_lrn = ru->act_eq = ru->act_nd = ru->act;
    ru->da = 0.0f;              // I'm fully settled!
    ru->AddToActBuf(rus->syn_delay);
  }
}

void ECoutLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->ct_cycle == auto_m_cycles)
    RecordActM2(lay,net);
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    ClampFromECin(lay, net);
    if(net->cycle <= 1)
      Compute_AutoEncStats(lay, net);
  }
  inherited::Compute_CycleStats(lay, net);
}
