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


void ScalarValSpec::Initialize() {
  rep = LOCALIST;
  un_width = .3f;
  norm_width = false;
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
  incr = range / (float)(ugp_size - 2); // skip 1st unit, and count end..
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
  int eff_idx = unit_idx - 1;
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
  int eff_idx = unit_idx - 1;
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
  Defaults_init();
}

void ScalarValLayerSpec::Defaults_init() {
  SetUnique("kwta", true);
  SetUnique("gp_kwta", true);
  SetUnique("inhib", true);
  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    kwta.k_from = KWTASpec::USE_K;
    kwta.k = 3;
    gp_kwta.k_from = KWTASpec::USE_K;
    gp_kwta.k = 3;
    inhib.type = LeabraInhibSpec::KWTA_INHIB;
    inhib.kwta_pt = 0.25f;

    unit_range.min = -0.5f;   unit_range.max = 1.5f;
    unit_range.UpdateAfterEdit_NoGui();
    scalar.InitRange(unit_range.min, unit_range.range); // needed for un_width_eff
    val_range.min = unit_range.min + (.5f * scalar.un_width_eff);
    val_range.max = unit_range.max - (.5f * scalar.un_width_eff);
  }
  else if(scalar.rep == ScalarValSpec::LOCALIST) {
    kwta.k_from = KWTASpec::USE_K;
    kwta.k = 1;
    gp_kwta.k_from = KWTASpec::USE_K;
    gp_kwta.k = 1;
    inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
    inhib.kwta_pt = .9f;

    unit_range.min = 0.0f;  unit_range.max = 1.0f;
    unit_range.UpdateAfterEdit_NoGui();
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;

    scalar.min_sum_act = .2f;
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
  }
  val_range.UpdateAfterEdit_NoGui();
}

void ScalarValLayerSpec::HelpConfig() {
  String help = "ScalarValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent a single scalar value.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current scalar value is displayed in the first unit in the layer, which can be clamped\
 and compared, etc (i.e., set the environment patterns to have just one unit and provide\
 the actual scalar value and it will automatically establish the appropriate distributed\
 representation in the rest of the units).  This first unit is only viewable as act_eq,\
 not act, because it must not send activation to other units.\n\
 \nScalarValLayerSpec Configuration:\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through a constant excitatory current (GC) or bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the ScalarValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  taMisc::Confirm(help);
}

bool ScalarValLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->CheckError(lay->un_geom.n < 3, quiet, rval,
                "coarse-coded scalar representation requires at least 3 units, I just set un_geom.n")) {
    if(scalar.rep == ScalarValSpec::LOCALIST) {
      lay->un_geom.n = 4;
      lay->un_geom.x = 4;
    }
    else if(scalar.rep == ScalarValSpec::GAUSSIAN) {
      lay->un_geom.n = 12;
      lay->un_geom.x = 12;
    }
  }

  if(scalar.rep == ScalarValSpec::LOCALIST) {
    kwta.k = 1;         // localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == ScalarValBias::GC) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    if(lay->CheckError(us->hyst.init, quiet, rval,
                  "bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("hyst", true);
      us->hyst.init = false;
    }
    if(lay->CheckError(us->acc.init, quiet, rval,
                  "bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("acc", true);
      us->acc.init = false;
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
                "scalar val layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
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
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
  }
  return rval;
}

void ScalarValLayerSpec::ReConfig(Network* net, int n_units) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->spec.SPtr() != this) continue;

    if(n_units > 0) {
      lay->un_geom.n = n_units;
      lay->un_geom.x = n_units;
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);    // taking 1st unit as representative

    if(scalar.rep == ScalarValSpec::LOCALIST) {
      scalar.min_sum_act = .2f;
      kwta.k = 1;
      inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
      inhib.kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = ScalarValBias::GC;
      bias_val.wt = ScalarValBias::NO_WT;
      unit_range.min = 0.0f; unit_range.max = 1.0f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = false; // false: this is critical for linear mapping of vals..
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    else if(scalar.rep == ScalarValSpec::GAUSSIAN) {
      inhib.type = LeabraInhibSpec::KWTA_INHIB;
      inhib.kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = ScalarValBias::GC;
      bias_val.wt = ScalarValBias::NO_WT;
      unit_range.min = -.5f; unit_range.max = 1.5f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = true;
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    us->UpdateAfterEdit();
  }
  UpdateAfterEdit();
}

// todo: deal with lesion flag in lots of special purpose code like this!!!

void ScalarValLayerSpec::Compute_WtBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md,
                                            int gpidx, float val) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = .03f * bias_val.wt_gain * scalar.GetUnitAct(i);
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
         cs->InheritsFrom(TA_MarkerConSpec)) continue;
      for(int ci=0;ci<recv_gp->size;ci++) {
        LeabraCon* cn = (LeabraCon*)recv_gp->PtrCn(ci);
        cn->wt += act;
        if(cn->wt < cs->wt_limits.min) cn->wt = cs->wt_limits.min;
        if(cn->wt > cs->wt_limits.max) cn->wt = cs->wt_limits.max;
      }
      recv_gp->Init_Weights_post(u);
    }
  }
}

void ScalarValLayerSpec::Compute_UnBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md,
                                            int gpidx, float val) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = bias_val.un_gain * scalar.GetUnitAct(i);
    if(bias_val.un == ScalarValBias::GC)
      u->vcb.g_h = act;
    else if(bias_val.un == ScalarValBias::BWT)
      u->bias.OwnCn(0)->wt = act;
  }
}

void ScalarValLayerSpec::Compute_UnBias_NegSlp(LeabraLayer* lay, Layer::AccessMode acc_md,
                                               int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  float val = 0.0f;
  float incr = bias_val.un_gain / (float)(nunits - 2);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(bias_val.un == ScalarValBias::GC)
      u->vcb.g_a = val;
    else if(bias_val.un == ScalarValBias::BWT)
      u->bias.OwnCn(0)->wt = -val;
  }
}

void ScalarValLayerSpec::Compute_UnBias_PosSlp(LeabraLayer* lay, Layer::AccessMode acc_md,
                                               int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  float val = bias_val.un_gain;
  float incr = bias_val.un_gain / (float)(nunits - 2);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(bias_val.un == ScalarValBias::GC)
      u->vcb.g_h = val;
    else if(bias_val.un == ScalarValBias::BWT)
      u->bias.OwnCn(0)->wt = val;
  }
}

void ScalarValLayerSpec::Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net) {
  if(bias_val.un != ScalarValBias::NO_UN) {
    if(bias_val.un_shp == ScalarValBias::VAL) {
      UNIT_GP_ITR(lay, Compute_UnBias_Val(lay, acc_md, gpidx, bias_val.val););
    }
    else if(bias_val.un_shp == ScalarValBias::NEG_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_NegSlp(lay, acc_md, gpidx););
    }
    else if(bias_val.un_shp == ScalarValBias::POS_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_PosSlp(lay, acc_md, gpidx););
    }
  }
  if(bias_val.wt == ScalarValBias::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(lay, acc_md, gpidx, bias_val.val););
  }
}

void ScalarValLayerSpec::BuildUnits_Threads_ugp(LeabraLayer* lay,
                                                Layer::AccessMode acc_md, int gpidx,
                                                LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* un = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(un->lesioned()) continue;
    if(i == 0) { un->flat_idx = 0; continue; }
    un->flat_idx = net->units_flat.size;
    net->units_flat.Add(un);
  }
}

void ScalarValLayerSpec::BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net) {
  lay->units_flat_idx = net->units_flat.size;
  if(lay->units.leaves == 0) return; // not built yet
  UNIT_GP_ITR(lay, BuildUnits_Threads_ugp(lay, acc_md, gpidx, net););
}

void ScalarValLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  Compute_BiasVal(lay, net);
  if(scalar.init_nms)
    LabelUnits(lay, net);
}

void ScalarValLayerSpec::Compute_AvgMaxVals_ugp(LeabraLayer* lay,
                                                Layer::AccessMode acc_md, int gpidx,
                                                AvgMaxVals& vals, ta_memb_ptr mb_off) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  vals.InitVals();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(i == 0) { continue; } // skip first unit
    float val = *((float*)MemberDef::GetOff_static((void*)u, 0, mb_off));
    vals.UpdtVals(val, i);
  }
  vals.CalcAvg(nunits);
}

void ScalarValLayerSpec::ClampValue_ugp(LeabraLayer* lay,
                                        Layer::AccessMode acc_md, int gpidx,
                                        LeabraNetwork*, float rescale) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
  if(!clamp.hard)
    u->UnSetExtFlag(Unit::EXT);
  else
    u->SetExtFlag(Unit::EXT);
  float val = u->ext;
  if(scalar.clip_val)
    val = val_range.Clip(val);          // first unit has the value to clamp
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = rescale * scalar.GetUnitAct(i);
    if(act < us->opt_thresh.send)
      act = 0.0f;
    u->SetExtFlag(Unit::EXT);
    u->ext = act;
  }
}

float ScalarValLayerSpec::ClampAvgAct(int ugp_size) {
  if(ugp_size < 3) return 0.0f;
  float val = val_range.min + .5f * val_range.Range(); // half way
  scalar.InitVal(val, ugp_size, unit_range.min, unit_range.range);
  float sum = 0.0f;
  for(int i=1;i<ugp_size;i++) {
    float act = scalar.GetUnitAct(i);
    sum += act;
  }
  sum /= (float)(ugp_size - 1);
  return sum;
}

float ScalarValLayerSpec::ReadValue_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                        LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return 0.0f;   // must be at least a few units..

  scalar.InitVal(0.0f, nunits, unit_range.min, unit_range.range);
  float avg = 0.0f;
  float sum_act = 0.0f;
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    float cur = scalar.GetUnitVal(i);
    float act_val = 0.0f;
    if(!scalar.send_thr || (u->act_eq >= us->opt_thresh.send)) // only if over sending thresh!
      act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
    avg += cur * act_val;
    sum_act += act_val;
  }
  sum_act = MAX(sum_act, scalar.min_sum_act);
  if(sum_act > 0.0f)
    avg /= sum_act;
  // set the first unit in the group to represent the value
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  u->act_eq = u->act_nd = avg;
  u->act = 0.0f;                // very important to clamp act to 0: don't send!
  u->da = 0.0f;                 // don't contribute to change in act
  return u->act_eq;
}

void ScalarValLayerSpec::ReadValue(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, ReadValue_ugp(lay, acc_md, gpidx, net); );
}

void ScalarValLayerSpec::Compute_ExtToPlus_ugp(LeabraLayer* lay,
                                               Layer::AccessMode acc_md, int gpidx,
                                               LeabraNetwork*) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(i > 0) u->act_p = us->clamp_range.Clip(u->ext);
    else u->act_p = u->ext;
    u->act_dif = u->act_p - u->act_m;
    // important to clear ext stuff, otherwise it will get added into netin next time around!!
    u->ext = 0.0f;
    u->ext_flag = Unit::NO_EXTERNAL;
  }
}

void ScalarValLayerSpec::Compute_ExtToAct_ugp(LeabraLayer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              LeabraNetwork*) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(i > 0) u->act_eq = u->act = us->clamp_range.Clip(u->ext);
    else u->act_eq = u->ext;
    u->ext = 0.0f;
    u->ext_flag = Unit::NO_EXTERNAL;
  }
}

void ScalarValLayerSpec::HardClampExt(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_HardClamp(lay, net);
  ResetAfterClamp(lay, net);
}

void ScalarValLayerSpec::ResetAfterClamp_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits > 2) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
    u->act = 0.0f;              // must reset so it doesn't contribute!
    u->act_eq = u->act_nd = u->ext;     // avoid clamp_range!
  }
}

void ScalarValLayerSpec::ResetAfterClamp(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, ResetAfterClamp_ugp(lay, acc_md, gpidx, net); );
}

void ScalarValLayerSpec::LabelUnits_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                        LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  scalar.InitVal(0.0f, nunits, unit_range.min, unit_range.range);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float cur = scalar.GetUnitVal(i);
    u->name = (String)cur;
  }
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  u->name = "val";              // overall value
}

void ScalarValLayerSpec::LabelUnits(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, LabelUnits_ugp(lay, acc_md, gpidx, net); );
}

void ScalarValLayerSpec::LabelUnitsNet(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, net->layers) {
    if(l->spec.SPtr() == this)
      LabelUnits(l, net);
  }
}

void ScalarValLayerSpec::Settle_Init_Unit0_ugp(LeabraLayer* lay,
                                               Layer::AccessMode acc_md, int gpidx,
                                               LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits > 2) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
    u->Settle_Init_Unit(net);
  }
}

void ScalarValLayerSpec::Settle_Init_Unit0(LeabraLayer* lay, LeabraNetwork* net) {
  // very important: unit 0 in each layer is used for the netin scale parameter and
  // it is otherwise not computed on this unit b/c it is excluded from units_flat!
  // also the targflags need to be updated
  UNIT_GP_ITR(lay, Settle_Init_Unit0_ugp(lay, acc_md, gpidx, net); );
}

void ScalarValLayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_Layer(lay, net);

  Settle_Init_Unit0(lay, net);

  if(bias_val.un == ScalarValBias::BWT) {
    // if using bias-weight bias, keep a constant scaling (independent of layer size)
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      LeabraConSpec* bspec = (LeabraConSpec*)u->GetUnitSpec()->bias_spec.SPtr();
      u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
      u->bias_scale /= 100.0f;
    }
  }
}

void ScalarValLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(scalar.clamp_pat) {
    inherited::Compute_HardClamp(lay, net);
    return;
  }
  if(!lay->HasExtFlag(Unit::EXT)) {
    lay->hard_clamped = false;
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, ClampValue_ugp(lay, acc_md, gpidx, net); );
  // now check for actual hard clamping
  if(!clamp.hard) {
    lay->hard_clamped = false;
    return;
  }
  HardClampExt(lay, net);
}

void ScalarValLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  ReadValue(lay, net);          // always read out the value
}

float ScalarValLayerSpec::Compute_SSE_ugp(LeabraLayer* lay,
                                          Layer::AccessMode acc_md, int gpidx, int& n_vals) {
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  // only count if target value is within range -- otherwise considered a non-target
  if(u->HasExtFlag(Unit::TARG | Unit::COMP) && val_range.RangeTestEq(u->targ)) {
    n_vals++;
    float uerr = u->targ - u->act_m;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return uerr * uerr;
  }
  return 0.0f;
}

float ScalarValLayerSpec::Compute_SSE(LeabraLayer* lay, LeabraNetwork*,
                                      int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  if(!(lay->HasExtFlag(Unit::TARG | Unit::COMP))) return 0.0f;
  lay->sse = 0.0f;
  UNIT_GP_ITR(lay, lay->sse += Compute_SSE_ugp(lay, acc_md, gpidx, n_vals); );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float ScalarValLayerSpec::Compute_NormErr_ugp(LeabraLayer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              LeabraInhib* thr, LeabraNetwork* net) {
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  // only count if target value is within range -- otherwise considered a non-target
  if(u->HasExtFlag(Unit::TARG | Unit::COMP) && val_range.RangeTestEq(u->targ)) {
    float uerr = u->targ - u->act_m;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return fabsf(uerr);
  }
  return 0.0f;
}

float ScalarValLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!lay->HasExtFlag(Unit::TARG | Unit::COMP)) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  float ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      nerr += Compute_NormErr_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      ntot += unit_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
    ntot += unit_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}

