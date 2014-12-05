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

TA_BASEFUNS_CTORS_DEFN(ECoutLayerSpec);

void ECoutLayerSpec::Initialize() {
  Defaults_init();
}

void ECoutLayerSpec::Defaults_init() {
  // SetUnique("inhib_group", true);
  // inhib_group = UNIT_GROUPS;
  // SetUnique("inhib", true);
  // inhib.type = LeabraInhibSpec::KWTA_INHIB;
  // inhib.kwta_pt = 0.25f;
  // SetUnique("clamp", true);
  // clamp.max_plus = true;
}

bool ECoutLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  bool rval = true;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
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
    ru->act = rus->clamp_range.Clip(inval);
    ru->act_eq = ru->act_nd = ru->act;
    ru->da = 0.0f;              // I'm fully settled!
    ru->AddToActBuf(rus->syn_delay);
  }
}

void ECoutLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no) {
  if(net->cycle == auto_m_cycles)
    RecordActMid(lay,net);
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    ClampFromECin(lay, net);
    if(net->cycle <= 1)
      Compute_AutoEncStats(lay, net);
  }
  inherited::Compute_CycleStats(lay, net, thread_no);
}

void ThetaPhaseLayerSpec::Compute_AutoEncStats(LeabraLayer* lay, LeabraNetwork* net) {
  // LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  // float norm_err = 0.0f;
  // float sse_err = 0.0f;
  // FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
  //   if(u->lesioned()) continue;
  //   float sse = u->act_eq - u->act_mid;
  //   if(fabsf(sse) < us->sse_tol)
  //     sse = 0.0f;
  //   sse *= sse;
  //   sse_err += sse;

  //   if(net->lstats.on_errs) {
  //     if(u->act_mid > 0.5f && u->act_eq < 0.5f) norm_err += 1.0f;
  //   }
  //   if(net->lstats.off_errs) {
  //     if(u->act_mid < 0.5f && u->act_eq > 0.5f) norm_err += 1.0f;
  //   }
  // }
  // int lay_nunits = lay->UnitAccess_NUnits(Layer::ACC_LAY);
  // int ntot = 0;
  // if(net->lstats.on_errs && net->lstats.off_errs)
  //   ntot = 2 * (int)(lay->acts_m_avg * (float)lay_nunits);
  // else
  //   ntot = (int)(lay->acts_m_avg * (float)lay_nunits);
  // if(ntot > 0)
  //   norm_err = norm_err / (float)ntot;
  // if(norm_err > 1.0f)
  //   norm_err = 1.0f;
  // lay->SetUserData("enc_sse", sse_err);
  // lay->SetUserData("enc_norm_err", norm_err);
}
