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

#include "SubiculumLayerSpec.h"
#include <LeabraNetwork>
#include <ECinLayerSpec>
#include <ECoutLayerSpec>

void SubiculumNoveltySpec::Initialize() {
  max_norm_err = 0.2f;
  min_lrate = 0.01f;

  nov_rescale = 1.0f / max_norm_err;
  lrate_factor = (1.0f - min_lrate);
}

void SubiculumNoveltySpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  nov_rescale = 1.0f / max_norm_err;
  lrate_factor = (1.0f - min_lrate);
}

void SubiculumLayerSpec::Initialize() {
  lrate_mod_con_spec.SetBaseType(&TA_LeabraConSpec);
  Defaults_init();
}

void SubiculumLayerSpec::Defaults_init() {
  scalar.rep = ScalarValSpec::GAUSSIAN;
  unit_range.min = -0.5f;
  unit_range.max = 1.5f;
  inhib.kwta_pt = 0.5f;
  kwta.k = 3;
}

bool SubiculumLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  bool rval = true;

  if(lay->CheckError(lay->projections.size < 2, quiet, rval,
        "must receive from at least 2 other layers -- one pair of EC_in / EC_out layers!")) {
    return false;
  }
  if(lay->CheckError(lay->projections.size % 2 != 0, quiet, rval,
        "must receive from an even number of other layers -- sequential matched pairs of EC_in / EC_out layers!")) {
    return false;
  }

  for(int pi=0; pi<lay->projections.size; pi+=2) {
    Projection* pin = lay->projections.FastEl(pi);
    Projection* pout = lay->projections.FastEl(pi+1);
    LeabraLayer* lin = (LeabraLayer*)pin->from.ptr();
    LeabraLayer* lout = (LeabraLayer*)pout->from.ptr();
    LeabraLayerSpec* lsin = (LeabraLayerSpec*)lin->GetLayerSpec();
    LeabraLayerSpec* lsout = (LeabraLayerSpec*)lout->GetLayerSpec();
    if(lay->CheckError(!lsin->InheritsFrom(&TA_ECinLayerSpec), quiet, rval,
                       "projection number:", String(pi), "should have been ECin, instead was:",
                       lsin->name, "type:", lsin->GetTypeDef()->name)) {
      return false;
    }
    if(lay->CheckError(!lsout->InheritsFrom(&TA_ECoutLayerSpec), quiet, rval,
               "projection number:", String(pi+1), "should have been ECout, instead was:",
               lsout->name, "type:", lsout->GetTypeDef()->name)) {
      return false;
    }
    bool geom_eq = (lin->un_geom == lout->un_geom) && (lin->gp_geom == lout->gp_geom);
    if(lay->CheckError(!geom_eq, quiet, rval,
               "projection numbers starting at:", String(pi), "do not have the same geometry, ECin layer:",
                       lin->name, "ECout layer:", lout->name)) {
      return false;
    }
  }

  LeabraConSpec* cs = (LeabraConSpec*)lrate_mod_con_spec.SPtr();
  if(lay->CheckError(!cs, quiet, rval,
                     "lrate_mod_con_spec is NULL")) {
      return false;
  }
  if(cs->lrate_sched.size > 0) {
    TestWarning(true, "CheckConfig", "configuring lrate_sched on lrate_mod_con_spec to be empty:"
                , cs->name);
    cs->lrate_sched.SetSize(0);
    cs->UpdateAfterEdit();      // update subs
  }


  return true;
}

float SubiculumLayerSpec::Compute_ECNormErr_ugp(LeabraLayer* lin, LeabraLayer* lout,
                                           Layer::AccessMode acc_md, int gpidx,
                                           LeabraNetwork* net) {
  int nunits = lin->UnitAccess_NUnits(acc_md);
  float nerr = 0.0f;
  for(int i=0; i<nunits; i++) {
    LeabraUnit* uin = (LeabraUnit*)lin->UnitAccess(acc_md, i, gpidx);
    if(uin->lesioned()) continue;
    LeabraUnit* uout = (LeabraUnit*)lout->UnitAccess(acc_md, i, gpidx);
    if(uout->lesioned()) continue;
    float outval = uout->act_eq;
    if(net->phase == LeabraNetwork::PLUS_PHASE) outval = uout->act_m; // use minus
    if(net->on_errs) { if(outval > 0.5f && uin->act_eq < 0.5f) nerr += 1.0f; }
    if(net->off_errs) { if(outval < 0.5f && uin->act_eq > 0.5f) nerr += 1.0f; }
  }
  return nerr;
}

void SubiculumLayerSpec::Compute_ECNovelty(LeabraLayer* lay, LeabraNetwork* net) {
  float nerr = 0.0f;
  int ntot = 0;
  for(int pi=0; pi<lay->projections.size; pi+=2) {
    Projection* pin = lay->projections.FastEl(pi);
    Projection* pout = lay->projections.FastEl(pi+1);
    LeabraLayer* lin = (LeabraLayer*)pin->from.ptr();
    LeabraLayer* lout = (LeabraLayer*)pout->from.ptr();
    LeabraLayerSpec* lsin = (LeabraLayerSpec*)lin->GetLayerSpec();
    LeabraLayerSpec* lsout = (LeabraLayerSpec*)lout->GetLayerSpec();

    if((lsin->inhib_group != ENTIRE_LAYER) && lin->unit_groups) {
      for(int g=0; g < lay->gp_geom.n; g++) {
        LeabraUnGpData* gpdin = lin->ungp_data.FastEl(g);
        nerr += Compute_ECNormErr_ugp(lin, lout, Layer::ACC_GP, g, net);
        if(net->on_errs && net->off_errs)
          ntot += 2 * gpdin->kwta.k;
        else
          ntot += gpdin->kwta.k;
      }
    }
    else {
      nerr += Compute_ECNormErr_ugp(lin, lout, Layer::ACC_LAY, 0, net);
      if(net->on_errs && net->off_errs)
        ntot += 2 * lin->kwta.k;
      else
        ntot += lin->kwta.k;
    }
  }

  // store norm_err on us too..
  lay->norm_err = nerr / (float)ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  float nov = novelty.ComputeNovelty(lay->norm_err);
  float lrate = novelty.ComputeLrate(nov);
  lay->SetUserData("novelty", nov);
  lay->SetUserData("lrate", lrate);

  // clamp novelty value on our layer
  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     u->ext = nov;
     ClampValue_ugp(lay, acc_md, gpidx, net);
     Compute_ExtToAct_ugp(lay, acc_md, gpidx, net);
     );
}

void SubiculumLayerSpec::Compute_SetLrate(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_ECNovelty(lay, net);
  float nov = novelty.ComputeNovelty(lay->norm_err);
  float lrate = novelty.ComputeLrate(nov);
  LeabraConSpec* cs = (LeabraConSpec*)lrate_mod_con_spec.SPtr();
  if(cs) {
    cs->lrate_sched.default_val = lrate;
    cs->UpdateAfterEdit_NoGui();        // propagate to children
    cs->SetCurLrate(net, net->epoch);	// actually make it take NOW to affect cur_lrate
    FOREACH_ELEM_IN_GROUP(LeabraConSpec, lc, cs->children) {
      lc->lrate_sched.default_val = lrate;
      lc->UpdateAfterEdit_NoGui();        // propagate to children
      lc->SetCurLrate(net, net->epoch);	// actually make it take NOW to affect cur_lrate
    }
  }
}

void SubiculumLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_ECNovelty(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void SubiculumLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    Compute_SetLrate(lay, net);
  }
}

