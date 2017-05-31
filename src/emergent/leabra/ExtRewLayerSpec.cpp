// Copyright 2016, Regents of the University of Colorado,
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

#include "ExtRewLayerSpec.h"
#include <LeabraNetwork>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ExtRewSpec);
TA_BASEFUNS_CTORS_DEFN(OutErrSpec);
TA_BASEFUNS_CTORS_DEFN(ExtRewLayerSpec);


void OutErrSpec::Initialize() {
  err_tol = 0.5f;
  graded = false;
  no_off_err = false;
  scalar_val_max = 1.0f;
}

void ExtRewSpec::Initialize() {
  err_val = 0.0f;
  norew_val = 0.5f;
  rew_val = 1.0f;
}

void ExtRewLayerSpec::Initialize() {
  rew_type = OUT_ERR_REW;
  Defaults_init();
}

void ExtRewLayerSpec::Defaults_init() {
  SetUnique("lay_inhib", true);
  lay_inhib.on = true;
  lay_inhib.gi = 1.0f;
  lay_inhib.fb = 0.0f;
  SetUnique("avg_act", true);
  avg_act.targ_init = 0.25f;
  avg_act.fixed = true;
  SetUnique("inhib_misc", true);
  inhib_misc.self_fb = 0.3f;

  SetUnique("scalar", true);
  scalar.rep = ScalarValSpec::LOCALIST;
  SetUnique("unit_range", true);
  unit_range.min = 0.0f;  unit_range.max = 1.0f;
  unit_range.UpdateAfterEdit_NoGui();
  val_range.min = unit_range.min;
  val_range.max = unit_range.max;
}
  
void ExtRewLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  rew.UpdateAfterEdit_NoGui();
  out_err.UpdateAfterEdit_NoGui();
}

void ExtRewLayerSpec::HelpConfig() {
  String help = "ExtRewLayerSpec Computation:\n\
 Computes external rewards based on network performance on an output layer or directly provided rewards.\n\
 - Minus phase = zero reward represented\n\
 - Plus phase = external reward value (computed at start of 1+) is clamped as distributed scalar-val representation.\n\
 \nExtRewLayerSpec Configuration:\n\
 - OUT_ERR_REW: A recv connection from the output layer(s) where error is computed (marked with MarkerConSpec)\n\
 AND a MarkerConSpec from a layer called RewTarg that signals (>.5 act) when output errors count\n\
 - EXT_REW: external TARGET inputs to targ values deliver the reward value (e.g., input pattern or script)\n\
 - DA_REW: A recv connection or other means of setting da values = reward values.\n\
 - Recommend using LOCALIST, 3 units, unit_range = 0,1 for simplest encoding\n\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool ExtRewLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  // sometimes this is used as a target layer for ext rew and it should NEVER drive sse
  lay->SetLayerFlag(Layer::NO_ADD_SSE);
  lay->SetLayerFlag(Layer::NO_ADD_COMP_SSE);

  // check for conspecs with correct params
  bool got_marker = false;
  LeabraLayer* rew_targ_lay = NULL;
  if(lay->units.leaves < 2) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(1);      // taking *2nd* unit as rep
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    if(recv_gp->GetConSpec()->IsMarkerCon()) {
      if(recv_gp->prjn->from->name == "RewTarg")
        rew_targ_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      else
        got_marker = true;
      continue;
    }
  }
  if(!got_marker) {
    lay->CheckError(rew_type == DA_REW, quiet, rval,
               "requires at least one recv MarkerConSpec connection from DA layer",
               "to get reward based on performance.  This was not found -- please fix!");
    lay->CheckError(rew_type == OUT_ERR_REW, quiet, rval,
               "requires at least one recv MarkerConSpec connection from output/response layer(s) to compute",
               "reward based on performance.  This was not found -- please fix!");
  }
  if(rew_type == OUT_ERR_REW) {
    if(lay->CheckError(rew_targ_lay == NULL, quiet, rval,
                  "requires a recv MarkerConSpec connection from layer called RewTarg",
                  "that signals (act > .5) when output error should be used for computing rewards.  This was not found -- please fix!")) {
      return false;                     // fatal
    }
    lay->CheckError(rew_targ_lay->units.size == 0, quiet, rval,
               "RewTarg layer must have one unit (has zero) -- please fix!");
    int myidx = lay->own_net->layers.FindLeafEl(lay);
    int rtidx = lay->own_net->layers.FindLeafEl(rew_targ_lay);
    lay->CheckError(rtidx > myidx, quiet, rval,
               "reward target (RewTarg) layer must be *before* this layer in list of layers -- it is now after, won't work");
  }
  return rval;
}

void ExtRewLayerSpec::Compute_Rew(LeabraLayer* lay, LeabraNetwork* net) {
  if(rew_type == OUT_ERR_REW)
    Compute_OutErrRew(lay, net);
  else if(rew_type == EXT_REW)
    Compute_ExtRew(lay, net);
  else if(rew_type == DA_REW)
    Compute_DaRew(lay, net);
}

bool ExtRewLayerSpec::OutErrRewAvail(LeabraLayer* lay, LeabraNetwork*) {
  bool got_some = false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(1);      // taking 2nd unit as representative
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    if(recv_gp->GetConSpec()->IsMarkerCon()) {
      LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      if(rew_lay->name != "RewTarg") continue;
      LeabraUnit* rtu = (LeabraUnit*)rew_lay->units[0];
      if(rtu->act_eq() > .5) {
        got_some = true;
        break;
      }
    }
  }
  return got_some;
}

float ExtRewLayerSpec::GetOutErrRew(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(1);      // taking 2nd unit as representative

  // first pass: find the layers: use COMP if no TARG is found
  int   n_targs = 0;            // number of target layers
  int   n_comps = 0;            // number of comp layers
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    if(!recv_gp->GetConSpec()->IsMarkerCon()) continue;
    LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(rew_lay->name == "RewTarg") continue;

    if(rew_lay->HasExtFlag(UnitVars::TARG)) n_targs++;
    else if(rew_lay->HasExtFlag(UnitVars::COMP)) n_comps++;
  }

  int rew_chk_flag = UnitVars::TARG;
  if(n_targs == 0) rew_chk_flag |= UnitVars::COMP; // also check comp if no targs!

  float totposs = 0.0f;         // total possible error (unitwise)
  float toterr = 0.0f;          // total error
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    if(!recv_gp->GetConSpec()->IsMarkerCon()) continue;
    LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(rew_lay->name == "RewTarg") continue;

    if(!rew_lay->HasExtFlag(rew_chk_flag)) continue; // only proceed if valid
    //  toterr += rew_lay->norm_err;        // now using norm err
    // this is now no longer computed at the point where we need it!  must compute ourselves!

    LeabraUnitSpec* us = (LeabraUnitSpec*)rew_lay->GetUnitSpec();
    
    float this_err = 0.0f;
    FOREACH_ELEM_IN_GROUP(LeabraUnit, un, rew_lay->units) {
      if(un->lesioned()) continue;
      LeabraUnitVars* uv = (LeabraUnitVars*)un->GetUnitVars();
      bool targ_active = false;
      float unerr = us->Compute_NormErr(uv, net, 0, targ_active);
      if(unerr > 0.0f) {
        this_err = 1.0f;
        break;
      }
    }
    toterr += this_err;
    totposs += 1.0f;
  }
  if(totposs == 0.0f)
    return -1.0f;               // -1 = no reward signal at all
  if(out_err.graded) {
    float nrmerr = toterr / totposs;
    if(nrmerr > 1.0f) nrmerr = 1.0f;
    return 1.0f - nrmerr;
  }
  if(toterr > 0.0f) return 0.0f; // 0 = wrong, 1 = correct
  return 1.0f;
}

void ExtRewLayerSpec::Compute_OutErrRew(LeabraLayer* lay, LeabraNetwork* net) {
  if(!OutErrRewAvail(lay, net)) {
    Compute_NoRewAct(lay, net);
    return;
  }

  float er = GetOutErrRew(lay, net);
  // starts out 0-1, transform into correct range
  er = (rew.rew_val - rew.err_val) * er + rew.err_val;

  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     Compute_UnitDa(lay, net, acc_md, gpidx, er, u);
     );

  net->ext_rew = er;
  net->ext_rew_avail = true;
}

void ExtRewLayerSpec::Compute_ExtRew(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->HasExtFlag(UnitVars::TARG)) {
    Compute_NoRewAct(lay, net);
    return;
  }

  int n_rew = 0;
  float er_avg = 0.0f;

  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
     float er = uv->ext;
     if(er == rew.norew_val) {
       uv->ext = uv->ext_orig = rew.norew_val;  // this is appropriate to set here..
       ClampValue_ugp(lay, net, acc_md, gpidx);
     }
     else {
       er_avg += er;
       n_rew++;
       Compute_UnitDa(lay, net, acc_md, gpidx, er, u);
     }
     );

  if(n_rew > 0) {
    net->ext_rew = er_avg / (float)n_rew;
    net->ext_rew_avail = true;
  }
  else {
    net->ext_rew = -1.1f;       // indicates no rew
    net->ext_rew_avail = false;
  }
}

void ExtRewLayerSpec::Compute_DaRew(LeabraLayer* lay, LeabraNetwork* net) {
  int n_rew = 0;
  float er_avg = 0.0f;

  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
     float er = uv->da_p;
     if(er == rew.norew_val) {
       uv->ext = uv->ext_orig = rew.norew_val;  // this is appropriate to set here..
       ClampValue_ugp(lay, net, acc_md, gpidx);
     }
     else {
       er_avg += er;
       n_rew++;
       Compute_UnitDa(lay, net, acc_md, gpidx, er, u);
     }
     );

  if(n_rew > 0) {
    net->ext_rew = er_avg / (float)n_rew;
    net->ext_rew_avail = true;
  }
  else {
    net->ext_rew = -1.1f;       // indicates no rew
    net->ext_rew_avail = false;
  }
}

void ExtRewLayerSpec::Compute_UnitDa
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx,
 float er, LeabraUnit* u) {
  LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
  uv->da_p = er;
  uv->ext = uv->ext_orig = uv->da_p;
  ClampValue_ugp(lay, net, acc_md, gpidx);
}

void ExtRewLayerSpec::Compute_NoRewAct(LeabraLayer* lay, LeabraNetwork* net) {
  net->ext_rew = -1.1f; // indicates no rew
  net->ext_rew_avail = false;

  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     u->ext() = u->ext_orig() = rew.norew_val;
     ClampValue_ugp(lay, net, acc_md, gpidx);
     );
}

void ExtRewLayerSpec::Compute_HardClamp_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase == LeabraNetwork::MINUS_PHASE) {
    lay->SetExtFlag(UnitVars::EXT);
    Compute_NoRewAct(lay, net); // no reward in minus
    HardClampExt(lay, net);
  }
  else {
    lay->SetExtFlag(UnitVars::EXT);
    Compute_Rew(lay, net);
    HardClampExt(lay, net);
  }
}
