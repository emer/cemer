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

#include "ExtRewLayerSpec.h"
#include <LeabraNetwork>
#include <MarkerConSpec>

#include <taMisc>


void AvgExtRewSpec::Initialize() {
  sub_avg = false;
  avg_dt = .005f;
}

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
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  rew_type = OUT_ERR_REW;

//   kwta.k = 1;
//   scalar.rep = ScalarValSpec::LOCALIST;
//   unit_range.min = 0.0f;  unit_range.max = 1.0f;
//   unit_range.UpdateAfterEdit_NoGui();
//   val_range.min = unit_range.min;
//   val_range.max = unit_range.max;
}

void ExtRewLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  rew.UpdateAfterEdit_NoGui();
  avg_rew.UpdateAfterEdit_NoGui();
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
 - This layer must be before layers that depend on it in list of layers\n\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool ExtRewLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;

  // sometimes this is used as a target layer for ext rew and it should NEVER drive sse
  lay->SetLayerFlag(Layer::NO_ADD_SSE);
  lay->SetLayerFlag(Layer::NO_ADD_COMP_SSE);

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(us->act.avg_dt != 0.0f, quiet, rval,
                "requires UnitSpec act.avg_dt = 0, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  bool got_marker = false;
  LeabraLayer* rew_targ_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
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

void ExtRewLayerSpec::BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net) {
  // that's it: don't do any processing on this layer: set all idx to 0
  lay->units_flat_idx = 0;
  FOREACH_ELEM_IN_GROUP(Unit, un, lay->units) {
    if(un->lesioned()) continue;
    un->flat_idx = 0;
  }
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
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      if(rew_lay->name != "RewTarg") continue;
      LeabraUnit* rtu = (LeabraUnit*)rew_lay->units[0];
      if(rtu->act_eq > .5) {
        got_some = true;
        break;
      }
    }
  }
  return got_some;
}

float ExtRewLayerSpec::GetOutErrRew(LeabraLayer* lay, LeabraNetwork*) {
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative

  // first pass: find the layers: use COMP if no TARG is found
  int   n_targs = 0;            // number of target layers
  int   n_comps = 0;            // number of comp layers
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(!recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec))
      continue;
    LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(rew_lay->name == "RewTarg") continue;

    if(rew_lay->HasExtFlag(Unit::TARG)) n_targs++;
    else if(rew_lay->HasExtFlag(Unit::COMP)) n_comps++;
  }

  int rew_chk_flag = Unit::TARG;
  if(n_targs == 0) rew_chk_flag |= Unit::COMP; // also check comp if no targs!

  float totposs = 0.0f;         // total possible error (unitwise)
  float toterr = 0.0f;          // total error
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(!recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec))
      continue;
    LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(rew_lay->name == "RewTarg") continue;

    if(!rew_lay->HasExtFlag(rew_chk_flag)) continue; // only proceed if valid
    toterr += rew_lay->norm_err;        // now using norm err
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
     Compute_UnitDa(lay, acc_md, gpidx, er, u, net);
     );

  net->ext_rew = er;
  net->ext_rew_avail = true;
  net->norew_val = rew.norew_val;
}

void ExtRewLayerSpec::Compute_ExtRew(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->HasExtFlag(Unit::TARG)) {
    Compute_NoRewAct(lay, net);
    return;
  }

  int n_rew = 0;
  float er_avg = 0.0f;

  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     float er = u->ext;
     if(er == rew.norew_val) {
       u->ext = rew.norew_val;  // this is appropriate to set here..
       ClampValue_ugp(lay, acc_md, gpidx, net);
     }
     else {
       er_avg += er;
       n_rew++;
       Compute_UnitDa(lay, acc_md, gpidx, er, u, net);
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
  net->norew_val = rew.norew_val;
}

void ExtRewLayerSpec::Compute_DaRew(LeabraLayer* lay, LeabraNetwork* net) {
  int n_rew = 0;
  float er_avg = 0.0f;

  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     float er = u->dav;
     if(er == rew.norew_val) {
       u->ext = rew.norew_val;  // this is appropriate to set here..
       ClampValue_ugp(lay, acc_md, gpidx, net);
     }
     else {
       er_avg += er;
       n_rew++;
       Compute_UnitDa(lay, acc_md, gpidx, er, u, net);
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
  net->norew_val = rew.norew_val;
}

void ExtRewLayerSpec::Compute_UnitDa(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                     float er, LeabraUnit* u, LeabraNetwork* net) {
  u->dav = er;
  if(avg_rew.sub_avg) u->dav -= u->act_avg;
  u->ext = u->dav;
  u->act_avg += avg_rew.avg_dt * (er - u->act_avg);
  ClampValue_ugp(lay, acc_md, gpidx, net);
}

void ExtRewLayerSpec::Compute_NoRewAct(LeabraLayer* lay, LeabraNetwork* net) {
  net->ext_rew = -1.1f; // indicates no rew
  net->ext_rew_avail = false;
  net->norew_val = rew.norew_val;

  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     u->ext = rew.norew_val;
     ClampValue_ugp(lay, acc_md, gpidx, net);
     );
}

void ExtRewLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0) {
    lay->SetExtFlag(Unit::EXT);
    Compute_NoRewAct(lay, net); // no reward in minus
    HardClampExt(lay, net);
  }
  else if(net->phase_no == 1) {
    lay->SetExtFlag(Unit::EXT);
    Compute_Rew(lay, net);
    HardClampExt(lay, net);
  }
  else {
    // clamp to prior act_p value: will happen automatically
    HardClampExt(lay, net);
  }
}
