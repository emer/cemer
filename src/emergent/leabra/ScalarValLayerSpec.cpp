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

#include "ScalarValLayerSpec.h"
#include <LeabraNetwork>
#include <taMath_float>
#include <ScalarValSelfPrjnSpec>
#include <MarkerConSpec>
#include <MemberDef>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ScalarValBias);
TA_BASEFUNS_CTORS_DEFN(ScalarValSpec);
TA_BASEFUNS_CTORS_DEFN(ScalarValLayerSpec);

void ScalarValSpec::Initialize() {
  rep = LOCALIST;
  un_width = .3f;
  norm_width = false;
  lrnmod_clamp = false;
  clamp_pat = false;
  clip_val = true;
  send_thr = false;
  init_nms = true;

  min = val = 0.0f;
  range = incr = 1.0f;
  un_width_eff = un_width;

  Defaults_init();
}

void ScalarValSpec::Defaults_init() {
  min_sum_act = 0.2f;
}

void ScalarValSpec::InitRange(float umin, float urng) {
  min = umin; range = urng;
  un_width_eff = un_width;
  if(norm_width)
    un_width_eff *= range;
}

void ScalarValSpec::InitVal(float sval, int ugp_size, float umin, float urng) {
  InitRange(umin, urng);
  val = sval;
  if(ugp_size > 1)
    incr = range / (float)(ugp_size - 1); // count end..
  else
    incr = 0.0f;
  //  incr -= .000001f;         // round-off tolerance..
}

// rep 1.5.  ugp_size = 4, incr = 1.5 / 3 = .5
// 0  .5   1
// oooo111122222 = val / incr

// 0 .5  1  val = .8, incr = .5
// 0 .4 .6
// (.4 * .5 + .6 * 1) / (.6 + .4) = .8

// act = 1.0 - (fabs(val - cur) / incr)


float ScalarValSpec::GetUnitAct(int unit_idx) {
  int eff_idx = unit_idx;
  if(rep == GAUSSIAN) {
    float cur = min + incr * (float)eff_idx;
    float dist = (cur - val) / un_width_eff;
    return taMath_float::exp_fast(-(dist * dist));
  }
  else if(rep == LOCALIST) {
    float cur = min + incr * (float)eff_idx;
    if(fabs(val - cur) > incr) return 0.0f;
    return 1.0f - (fabs(val - cur) / incr);
  }
  return 0.0f;                  // compiler food
}

float ScalarValSpec::GetUnitVal(int unit_idx) {
  int eff_idx = unit_idx;
  float cur = min + incr * (float)eff_idx;
  return cur;
}

void ScalarValBias::Initialize() {
  un = NO_UN;
  un_shp = VAL;
  un_gain = 1.0f;
  wt = NO_WT;
  val = 0.0f;
  wt_gain = 1.0f;
}

void ScalarValLayerSpec::Initialize() {
  unit_range.min = -0.5f;   unit_range.max = 1.5f;
  unit_range.UpdateAfterEdit_NoGui();
  avg_act_range.min = 0.1f; avg_act_range.max = 0.9f;
  avg_act_range.UpdateAfterEdit_NoGui();
  Defaults_init();
}

void ScalarValLayerSpec::Defaults_init() {
  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    // kwta.k_from = KWTASpec::USE_K;
    // kwta.k = 3;
    // gp_kwta.k_from = KWTASpec::USE_K;
    // gp_kwta.k = 3;
    lay_inhib.gi = 2.2f;
    unit_gp_inhib.gi = 2.2f;

    scalar.InitRange(unit_range.min, unit_range.range); // needed for un_width_eff
    val_range.min = unit_range.min + (.5f * scalar.un_width_eff);
    val_range.max = unit_range.max - (.5f * scalar.un_width_eff);
  }
  else if(scalar.rep == ScalarValSpec::LOCALIST) {
    // kwta.k_from = KWTASpec::USE_K;
    // kwta.k = 1;
    // gp_kwta.k_from = KWTASpec::USE_K;
    // gp_kwta.k = 1;
    lay_inhib.gi = 2.2f;
    unit_gp_inhib.gi = 2.2f;

    val_range.min = unit_range.min;
    val_range.max = unit_range.max;

    scalar.min_sum_act = .2f;
  }
  else if(scalar.rep == ScalarValSpec::AVG_ACT) {
    scalar.min_sum_act = 0.0f;

    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
  }
  val_range.UpdateAfterEdit_NoGui();
}

void ScalarValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  unit_range.UpdateAfterEdit_NoGui();
  scalar.UpdateAfterEdit_NoGui();
  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    scalar.InitRange(unit_range.min, unit_range.range); // needed for un_width_eff
    val_range.min = unit_range.min + (.5f * scalar.un_width_eff);
    val_range.max = unit_range.max - (.5f * scalar.un_width_eff);
  }
  else {
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
    if(scalar.rep == ScalarValSpec::AVG_ACT) {
      scalar.min_sum_act = 0.0f;
    }
  }
  val_range.UpdateAfterEdit_NoGui();
}

void ScalarValLayerSpec::HelpConfig() {
  String help = "ScalarValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent a single scalar value.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current scalar value is displayed in the act_eq variable of first unit in the layer, which can be clamped\
 and compared, etc (i.e., set the environment patterns to have just one unit and provide\
 the actual scalar value and it will automatically establish the appropriate distributed\
 representation in the rest of the units).  Unlike previous implementations, all units including the first are part of the distributed representation -- first unit is not special except in receiving the input and displaying output as act_eq.\n\
 \nScalarValLayerSpec Configuration:\n\
 - Default UnitSpec and LayerSpec params with FF_FB_INHIB, gi = 2.2 generally works well\n\
 - For 0-1 range, GAUSSIAN: 11 or 21 units works well, LOCALIST: 3 units\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the ScalarValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  taMisc::Confirm(help);
}

bool ScalarValLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(scalar.rep != ScalarValSpec::AVG_ACT) {
    if(lay->CheckError(lay->un_geom.n < 2, quiet, rval,
                       "coarse-coded scalar representation requires at least 2 units, I just set un_geom.n")) {
      if(scalar.rep == ScalarValSpec::LOCALIST) {
        lay->un_geom.n = 3;
        lay->un_geom.x = 3;
      }
      else if(scalar.rep == ScalarValSpec::GAUSSIAN) {
        lay->un_geom.n = 11;
        lay->un_geom.x = 11;
      }
    }

    if(scalar.rep == ScalarValSpec::LOCALIST) {
      // kwta.k = 1;         // localist means 1 unit active!!
      // gp_kwta.k = 1;
    }
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  if(us->act_misc.rec_nd) {
    taMisc::Warning("Scalar val must have UnitSpec.act_misc.rec_nd = false, to record value in act_eq of first unit.  I changed this for you in spec:", us->name, "make sure this is appropriate for all layers that use this spec");
    us->SetUnique("act_misc", true);
    us->act_misc.rec_nd = false;
  }
  if(!us->act_misc.avg_nd) {
    taMisc::Warning("Scalar val must have UnitSpec.act_misc.avg_nd = true, so learning is based on act_nd and NOT act_eq, which is used to record value in first unit.  I changed this for you in spec:", us->name, "make sure this is appropriate for all layers that use this spec");
    us->SetUnique("act_misc", true);
    us->act_misc.avg_nd = true;
  }
  
  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
                "scalar val layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
                    "scalar val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
        cs->SetUnique("wt_scale", true);
        cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
                    "scalar val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
        cs->SetUnique("lrate", true);
        cs->lrate = 0.0f;
      }
    }
  }
  return rval;
}

// todo: deal with lesion flag in lots of special purpose code like this!!!

void ScalarValLayerSpec::Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net) {
  if(bias_val.un != ScalarValBias::NO_UN) {
    if(bias_val.un_shp == ScalarValBias::VAL) {
      UNIT_GP_ITR(lay, Compute_UnBias_Val(lay, net, acc_md, gpidx, bias_val.val););
    }
    else if(bias_val.un_shp == ScalarValBias::NEG_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_NegSlp(lay, net, acc_md, gpidx););
    }
    else if(bias_val.un_shp == ScalarValBias::POS_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_PosSlp(lay, net, acc_md, gpidx););
    }
  }
  if(bias_val.wt == ScalarValBias::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(lay, net, acc_md, gpidx, bias_val.val););
  }
}

void ScalarValLayerSpec::Compute_WtBias_Val
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx, float val) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 2) return;        // must be at least a few units..
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = .03f * bias_val.wt_gain * scalar.GetUnitAct(i);
    const int nrg = u->NRecvConGps();
    for(int g=0; g<nrg; g++) {
      LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
      if(recv_gp->NotActive()) continue;
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
         cs->InheritsFrom(TA_MarkerConSpec)) continue;
      for(int ci=0;ci<recv_gp->size;ci++) {
        float& wt = recv_gp->PtrCn(ci, ConGroup::WT, net);
        wt += act;
        if(wt < cs->wt_limits.min) wt = cs->wt_limits.min;
        if(wt > cs->wt_limits.max) wt = cs->wt_limits.max;
      }
    }
  }
}

void ScalarValLayerSpec::Compute_UnBias_Val
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx, float val) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 2) return;        // must be at least a few units..
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    float act = bias_val.un_gain * scalar.GetUnitAct(i);
    if(bias_val.un == ScalarValBias::BWT)
      uv->bias_wt = act;
  }
}

void ScalarValLayerSpec::Compute_UnBias_NegSlp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 2) return;        // must be at least a few units..
  float val = 0.0f;
  float incr = bias_val.un_gain / (float)(nunits - 1);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    if(bias_val.un == ScalarValBias::BWT)
      uv->bias_wt = -val;
    val += incr;
  }
}

void ScalarValLayerSpec::Compute_UnBias_PosSlp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 2) return;        // must be at least a few units..
  float val = bias_val.un_gain;
  float incr = bias_val.un_gain / (float)(nunits - 1);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    if(bias_val.un == ScalarValBias::BWT)
      uv->bias_wt = val;
    val += incr;
  }
}

void ScalarValLayerSpec::Init_Weights_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights_Layer(lay, net);
  Compute_BiasVal(lay, net);
  if(scalar.init_nms)
    LabelUnits(lay, net);
}

void ScalarValLayerSpec::ClampValue_ugp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx, float rescale) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 1) return;        // must be at least a few units..
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
  LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
  if(!clamp.hard)
    uv->ClearExtFlag(UnitVars::EXT);
  else
    uv->SetExtFlag(UnitVars::EXT);
  float val = uv->ext;
  if(scalar.clip_val)
    val = val_range.Clip(val);          // first unit has the value to clamp
  uv->act_eq = uv->misc_1 = val;        // record this val
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);

  float avg_act = 0.0f;
  if(scalar.rep == ScalarValSpec::AVG_ACT) {
    avg_act = avg_act_range.Project(unit_range.Normalize(val));
  }

  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    float act;
    if(scalar.rep == ScalarValSpec::AVG_ACT) {
      act = avg_act;
    }
    else {
      act = rescale * scalar.GetUnitAct(i);
    }
    if(act < us->opt_thresh.send)
      act = 0.0f;
    uv->SetExtFlag(UnitVars::EXT);
    uv->ext = act;
  }
}

float ScalarValLayerSpec::ClampAvgAct(int ugp_size) {
  if(ugp_size < 2) return 0.0f;
  float val = val_range.min + .5f * val_range.Range(); // half way
  scalar.InitVal(val, ugp_size, unit_range.min, unit_range.range);
  float sum = 0.0f;
  for(int i=0;i<ugp_size;i++) {
    float act = scalar.GetUnitAct(i);
    sum += act;
  }
  sum /= (float)(ugp_size - 1);
  return sum;
}

float ScalarValLayerSpec::ReadValue_ugp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 1) return 0.0f;   // must be at least a few units..

  scalar.InitVal(0.0f, nunits, unit_range.min, unit_range.range);
  float avg = 0.0f;
  float sum_act = 0.0f;
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    float cur = scalar.GetUnitVal(i);
    float act_val = 0.0f;
    const float act_eq = uv->act_eq;
    if(!scalar.send_thr || (act_eq >= us->opt_thresh.send)) // only if over sending thresh!
      act_val = us->clamp_range.Clip(act_eq) / us->clamp_range.max; // clipped & normalized!
    avg += cur * act_val;
    sum_act += act_val;
  }
  sum_act = MAX(sum_act, scalar.min_sum_act);
  if(scalar.rep == ScalarValSpec::AVG_ACT) {
    sum_act /= (float)nunits;
    avg = val_range.Project(avg_act_range.Normalize(sum_act));
  }
  else {
    if(sum_act > 0.0f)
      avg /= sum_act;
  }
  // set the first unit in the group to represent the value
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
  uv->act_eq = uv->misc_1 = avg;
  return avg;
}

void ScalarValLayerSpec::ReadValue(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, ReadValue_ugp(lay, net, acc_md, gpidx); );
}

void ScalarValLayerSpec::Compute_ExtToPlus_ugp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 1) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    uv->act_p = us->clamp_range.Clip(uv->ext);
    uv->act_dif = uv->act_p - uv->act_m;
    // important to clear ext stuff, otherwise it will get added into netin next time around!!
    uv->ext = 0.0f;
    uv->ClearExtFlag(UnitVars::COMP_TARG_EXT);
  }
}

void ScalarValLayerSpec::Compute_LrnModToExt_ugp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 1) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx); // first unit
  if(u->lesioned()) return;
  LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
  uv->ext = uv->targ = uv->lrnmod;
  uv->SetExtFlag(UnitVars::EXT);
  uv->SetExtFlag(UnitVars::TARG);
}

void ScalarValLayerSpec::Compute_ExtToAct_ugp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 1) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    uv->act_eq = uv->act = us->clamp_range.Clip(uv->ext);
    uv->ext = 0.0f;
    uv->ClearExtFlag(UnitVars::COMP_TARG_EXT);
  }
}

void ScalarValLayerSpec::HardClampExt_ugp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 1) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    ((LeabraUnitSpec*)u->GetUnitSpec())->Compute_HardClamp(uv, net, u->ThrNo());
  }
}

void ScalarValLayerSpec::HardClampExt(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, HardClampExt_ugp(lay, net, acc_md, gpidx); );
}

void ScalarValLayerSpec::LabelUnits_ugp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 1) return;        // must be at least a few units..
  scalar.InitVal(0.0f, nunits, unit_range.min, unit_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float cur = scalar.GetUnitVal(i);
    u->name = (String)cur;
  }
}

void ScalarValLayerSpec::LabelUnits(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, LabelUnits_ugp(lay, net, acc_md, gpidx); );
}

void ScalarValLayerSpec::LabelUnitsNet(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, net->layers) {
    if(l->spec.SPtr() == this)
      LabelUnits(l, net);
  }
}

void ScalarValLayerSpec::Quarter_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Quarter_Init_Layer(lay, net);

  if(bias_val.un == ScalarValBias::BWT) {
    // if using bias-weight bias, keep a constant scaling (independent of layer size)
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      LeabraConSpec* bspec = (LeabraConSpec*)u->GetUnitSpec()->bias_spec.SPtr();
      LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
      uv->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
      uv->bias_scale /= 100.0f;
    }
  }
}

void ScalarValLayerSpec::Compute_HardClamp_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(scalar.clamp_pat) {
    inherited::Compute_HardClamp_Layer(lay, net);
    return;
  }
  if(scalar.lrnmod_clamp && net->phase == LeabraNetwork::PLUS_PHASE) {
    lay->SetExtFlag(UnitVars::EXT);
    lay->SetExtFlag(UnitVars::TARG); // do as targ so err stats work..
    UNIT_GP_ITR(lay, Compute_LrnModToExt_ugp(lay, net, acc_md, gpidx); );
  }
  if(!lay->HasExtFlag(UnitVars::EXT)) {
    lay->hard_clamped = false;
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, ClampValue_ugp(lay, net, acc_md, gpidx); );
  // now check for actual hard clamping
  if(!clamp.hard) {
    lay->hard_clamped = false;
    return;
  }
  HardClampExt(lay, net);
  lay->hard_clamped = true;
}

void ScalarValLayerSpec::Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_OutputName(lay, net);
  ReadValue(lay, net);          // always read out the value
  if(scalar.lrnmod_clamp && net->phase == LeabraNetwork::PLUS_PHASE) {
    Compute_HardClamp_Layer(lay, net); // update!
  }
}

float ScalarValLayerSpec::Compute_SSE_ugp(LeabraLayer* lay, LeabraNetwork* net,
                                          Layer::AccessMode acc_md, int gpidx, int& n_vals) {
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  // only count if target value is within range -- otherwise considered a non-target
  if(uv->HasExtFlag(UnitVars::COMP_TARG) && val_range.RangeTestEq(uv->targ)) {
    n_vals++;
    float uerr = uv->targ - uv->act_m;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return uerr * uerr;
  }
  return 0.0f;
}

float ScalarValLayerSpec::Compute_SSE
(LeabraLayer* lay, LeabraNetwork* net, int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  if(!(lay->HasExtFlag(UnitVars::COMP_TARG))) return 0.0f;
  lay->sse = 0.0f;
  UNIT_GP_ITR(lay, lay->sse += Compute_SSE_ugp(lay, net, acc_md, gpidx, n_vals); );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  lay->avg_sse.Increment(lay->sse);
  if(lay->sse > net->stats.cnt_err_tol)
    lay->cur_cnt_err += 1.0;
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(UnitVars::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float ScalarValLayerSpec::Compute_NormErr_ugp
(LeabraLayer* lay, LeabraNetwork* net, Layer::AccessMode acc_md, int gpidx) {
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  // only count if target value is within range -- otherwise considered a non-target
  if(uv->HasExtFlag(UnitVars::COMP_TARG) && val_range.RangeTestEq(uv->targ)) {
    float uerr = uv->targ - uv->act_m;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return fabsf(uerr);
  }
  return 0.0f;
}

float ScalarValLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!lay->HasExtFlag(UnitVars::COMP_TARG)) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  float ntot = 0;
  if(HasUnitGpInhib(lay)) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      nerr += Compute_NormErr_ugp(lay, net, Layer::ACC_GP, g);
      ntot += unit_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, net, Layer::ACC_LAY, 0);
    ntot += unit_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  lay->avg_norm_err.Increment(lay->norm_err);
  
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(UnitVars::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}

