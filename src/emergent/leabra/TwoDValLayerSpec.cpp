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

#include "TwoDValLayerSpec.h"
#include <LeabraNetwork>
#include <ScalarValSelfPrjnSpec>
#include <MarkerConSpec>
#include <ValIdx_Array>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TwoDValSpec);

TA_BASEFUNS_CTORS_DEFN(TwoDValBias);

TA_BASEFUNS_CTORS_DEFN(TwoDValLayerSpec);


void TwoDValSpec::Initialize() {
  rep = GAUSSIAN;
  n_vals = 1;
  un_width = .3f;
  norm_width = false;
  clamp_pat = false;
  min_sum_act = 0.2f;
  mn_dst = 0.5f;
  clip_val = true;

  x_min = x_val = y_min = y_val = 0.0f;
  x_range = x_incr = y_range = y_incr = 1.0f;
  x_size = y_size = 1;
  un_width_x = un_width_y = un_width;
}

void TwoDValSpec::InitRange(float xmin, float xrng, float ymin, float yrng) {
  x_min = xmin; x_range = xrng; y_min = ymin; y_range = yrng;
  un_width_x = un_width;
  un_width_y = un_width;
  if(norm_width) {
    un_width_x *= x_range;
    un_width_y *= y_range;
  }
}

void TwoDValSpec::InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng) {
  InitRange(xmin, xrng, ymin, yrng);
  x_val = xval; y_val = yval;
  x_size = xsize; y_size = ysize;
  x_incr = x_range / (float)(x_size - 1); // DON'T skip 1st row, and count end..
  y_incr = y_range / (float)(y_size - 1); // DON'T skip 1st row, and count end..
  //  incr -= .000001f;         // round-off tolerance..
}

float TwoDValSpec::GetUnitAct(int unit_idx) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size);
  if(rep == GAUSSIAN) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float x_dist = (x_cur - x_val) / un_width_x;
    float y_cur = y_min + y_incr * (float)y_idx;
    float y_dist = (y_cur - y_val) / un_width_y;
    float dist = x_dist * x_dist + y_dist * y_dist;
    return expf(-dist);
  }
  else if(rep == LOCALIST) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float y_cur = y_min + y_incr * (float)y_idx;
    float x_dist = fabs(x_val - x_cur);
    float y_dist = fabs(y_val - y_cur);
    if((x_dist > x_incr) && (y_dist > y_incr)) return 0.0f;

    return 1.0f - .5 * ((x_dist / x_incr) + (y_dist / y_incr)); // todo: no idea if this is right.
  }
  return 0.0f;
}

void TwoDValSpec::GetUnitVal(int unit_idx, float& x_cur, float& y_cur) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size);
  x_cur = x_min + x_incr * (float)x_idx;
  y_cur = y_min + y_incr * (float)y_idx;
}

void TwoDValBias::Initialize() {
  un = NO_UN;
  un_gain = 1.0f;
  wt = NO_WT;
  wt_gain = 1.0f;
  x_val = 0.0f;
  y_val = 0.0f;
}

void TwoDValLayerSpec::Initialize() {
  min_obj_type = &TA_TwoDValLeabraLayer;

  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 9;
  gp_kwta.k_from = KWTASpec::USE_K;
  gp_kwta.k = 9;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;

  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    x_range.min = -0.5f;   x_range.max = 1.5f; x_range.UpdateAfterEdit_NoGui();
    y_range.min = -0.5f;   y_range.max = 1.5f; y_range.UpdateAfterEdit_NoGui();
    twod.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * twod.un_width_x);
    x_val_range.max = x_range.max - (.5f * twod.un_width_x);
    y_val_range.min = y_range.min + (.5f * twod.un_width_y);
    y_val_range.max = y_range.max - (.5f * twod.un_width_y);
  }
  else if(twod.rep == TwoDValSpec::LOCALIST) {
    x_range.min = 0.0f;  x_range.max = 1.0f;  x_range.UpdateAfterEdit_NoGui();
    y_range.min = 0.0f;  y_range.max = 1.0f;  y_range.UpdateAfterEdit_NoGui();
    x_val_range.min = x_range.min;  x_val_range.max = x_range.max;
    y_val_range.min = y_range.min;  y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void TwoDValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  x_range.UpdateAfterEdit_NoGui(); y_range.UpdateAfterEdit_NoGui();
  twod.UpdateAfterEdit_NoGui();
  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    twod.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * twod.un_width_x);
    y_val_range.min = y_range.min + (.5f * twod.un_width_y);
    x_val_range.max = x_range.max - (.5f * twod.un_width_x);
    y_val_range.max = y_range.max - (.5f * twod.un_width_y);
  }
  else {
    x_val_range.min = x_range.min;    y_val_range.min = y_range.min;
    x_val_range.max = x_range.max;    y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void TwoDValLayerSpec::HelpConfig() {
  String help = "TwoDValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent two-dimensional values.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current twod value is encoded in the twod_vals member of the TwoDValLeabraLayer (x1,y1, x2,y2, etc),\
 which are set by input data, and updated to reflect current values encoded over layer.\
 For no unit groups case, input data should be 2d with inner dim of size 2 (x,y) and outer dim\
 of n_vals size.  For unit_groups, data should be 4d with two extra outer dims of gp_x, gp_y.\
 Provide the actual twod values in input data and it will automatically establish the \
 appropriate distributed representation in the rest of the units.\n\
 \nTwoDValLayerSpec Configuration:\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through a constant excitatory current (GC) or bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the TwoDValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  taMisc::Confirm(help);
}

bool TwoDValLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->CheckError(lay->un_geom.n < 3, quiet, rval,
                "coarse-coded twod representation requires at least 3 units, I just set un_geom.n")) {
    if(twod.rep == TwoDValSpec::LOCALIST) {
      lay->un_geom.n = 9;
      lay->un_geom.x = 3;
      lay->un_geom.y = 3;
    }
    else if(twod.rep == TwoDValSpec::GAUSSIAN) {
      lay->un_geom.n = 121;
      lay->un_geom.x = 11;
      lay->un_geom.y = 11;
    }
  }

  if(lay->InheritsFrom(&TA_TwoDValLeabraLayer)) { // inh will be flagged above
    ((TwoDValLeabraLayer*)lay)->UpdateTwoDValsGeom();
  }

  if(twod.rep == TwoDValSpec::LOCALIST) {
    kwta.k = 1;         // localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == TwoDValBias::GC) {
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
                "twod val layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
                    "twod val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
        cs->SetUnique("wt_scale", true);
        cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
                    "twod val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
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

void TwoDValLayerSpec::ReConfig(Network* net, int n_units) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->spec.SPtr() != this) continue;

    if(n_units > 0) {
      lay->SetNUnits(n_units);
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);    // taking 1st unit as representative

    if(twod.rep == TwoDValSpec::LOCALIST) {
      twod.min_sum_act = .2f;
      kwta.k = 1;
      inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
      inhib.kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = TwoDValBias::GC; bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = 0.0f; x_range.max = 1.0f;
      y_range.min = 0.0f; y_range.max = 1.0f;

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
    else if(twod.rep == TwoDValSpec::GAUSSIAN) {
      inhib.type = LeabraInhibSpec::KWTA_INHIB;
      inhib.kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = TwoDValBias::GC;  bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = -.5f; x_range.max = 1.5f;
      y_range.min = -.5f; y_range.max = 1.5f;

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

void TwoDValLayerSpec::Compute_WtBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                          float x_val, float y_val) {
  Network* net = lay->own_net;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = .03f * bias_val.wt_gain * twod.GetUnitAct(i);
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
         cs->InheritsFrom(TA_MarkerConSpec)) continue;
      for(int ci=0;ci<recv_gp->size;ci++) {
        float& wt = recv_gp->PtrCn(ci, BaseCons::WT, net);
        wt += act;
        if(wt < cs->wt_limits.min) wt = cs->wt_limits.min;
        if(wt > cs->wt_limits.max) wt = cs->wt_limits.max;
      }
      recv_gp->Init_Weights_post(u, lay->own_net);
    }
  }
}

void TwoDValLayerSpec::Compute_UnBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                          float x_val, float y_val) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = bias_val.un_gain * twod.GetUnitAct(i);
    if(bias_val.un == TwoDValBias::GC)
      u->vcb.g_h = act;
    else if(bias_val.un == TwoDValBias::BWT)
      u->bias.OwnCn(0,BaseCons::WT) = act;
  }
}

void TwoDValLayerSpec::Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net) {
  if(bias_val.un != TwoDValBias::NO_UN) {
    UNIT_GP_ITR(lay, Compute_UnBias_Val(lay, acc_md, gpidx, bias_val.x_val, bias_val.y_val););
  }
  if(bias_val.wt == TwoDValBias::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(lay, acc_md, gpidx, bias_val.x_val, bias_val.y_val););
  }
}

void TwoDValLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  Compute_BiasVal(lay, net);
}

void TwoDValLayerSpec::ClampValue_ugp(TwoDValLeabraLayer* lay,
                                      Layer::AccessMode acc_md, int gpidx,
                                      LeabraNetwork* net, float rescale) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  taVector2i gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  // first initialize to zero
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->SetExtFlag(Unit::EXT);
    u->ext = 0.0;
  }
  for(int k=0;k<twod.n_vals;k++) {
    float x_val = lay->GetTwoDVal(TwoDValLeabraLayer::TWOD_X, TwoDValLeabraLayer::TWOD_EXT,
                                  k, gp_geom_pos.x, gp_geom_pos.y);
    float y_val = lay->GetTwoDVal(TwoDValLeabraLayer::TWOD_Y, TwoDValLeabraLayer::TWOD_EXT,
                                  k, gp_geom_pos.x, gp_geom_pos.y);
    if(twod.clip_val) {
      x_val = x_val_range.Clip(x_val);
      y_val = y_val_range.Clip(y_val);
    }
    twod.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      if(u->lesioned()) continue;
      float act = rescale * twod.GetUnitAct(i);
      if(act < us->opt_thresh.send)
        act = 0.0f;
      u->ext += act;
    }
  }
}

void TwoDValLayerSpec::ReadValue(TwoDValLeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, ReadValue_ugp(lay, acc_md, gpidx, net); );
}

void TwoDValLayerSpec::ReadValue_ugp(TwoDValLeabraLayer* lay,
                                     Layer::AccessMode acc_md, int gpidx, LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  taVector2i gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  if(twod.n_vals == 1) {        // special case
    float x_avg = 0.0f; float y_avg = 0.0f;
    float sum_act = 0.0f;
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      if(u->lesioned()) continue;
      float x_cur, y_cur;  twod.GetUnitVal(i, x_cur, y_cur);
      float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
      x_avg += x_cur * act_val;
      y_avg += y_cur * act_val;
      sum_act += act_val;
    }
    sum_act = MAX(sum_act, twod.min_sum_act);
    if(sum_act > 0.0f) {
      x_avg /= sum_act; y_avg /= sum_act;
    }
    // encode the value
    lay->SetTwoDVal(x_avg, TwoDValLeabraLayer::TWOD_X, TwoDValLeabraLayer::TWOD_ACT,
                    0, gp_geom_pos.x, gp_geom_pos.y);
    lay->SetTwoDVal(y_avg, TwoDValLeabraLayer::TWOD_Y, TwoDValLeabraLayer::TWOD_ACT,
                    0, gp_geom_pos.x, gp_geom_pos.y);
  }
  else {                        // multiple items
    // first find the max values, using sum of -1..+1 region
    static ValIdx_Array sort_ary;
    sort_ary.Reset();
    for(int i=0;i<nunits;i++) {
      float sum = 0.0f;
      float nsum = 0.0f;
      for(int x=-1;x<=1;x++) {
        for(int y=-1;y<=1;y++) {
          int idx = i + y * lay->un_geom.x + x;
          if(idx < 0 || idx >= nunits) continue;
          LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, idx, gpidx);
	  if(u->lesioned()) continue;
          float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
          nsum += 1.0f;
          sum += act_val;
        }
      }
      if(nsum > 0.0f) sum /= nsum;
      ValIdx vi(sum, i);
      sort_ary.Add(vi);
    }
    sort_ary.Sort();
    float mn_x = twod.mn_dst * twod.un_width_x * x_range.Range();
    float mn_y = twod.mn_dst * twod.un_width_y * y_range.Range();
    float mn_dist = mn_x * mn_x + mn_y * mn_y;
    int outi = 0;  int j = 0;
    while((outi < twod.n_vals) && (j < sort_ary.size)) {
      ValIdx& vi = sort_ary[sort_ary.size - j - 1]; // going backward through sort_ary
      float x_cur, y_cur;  twod.GetUnitVal(vi.idx, x_cur, y_cur);
      // check distance from all previous!
      float my_mn = x_range.Range() + y_range.Range();
      for(int k=0; k<j; k++) {
        ValIdx& vo = sort_ary[sort_ary.size - k - 1];
        if(vo.val == -1.0f) continue; // guy we skipped over before
        float x_prv, y_prv;  twod.GetUnitVal(vo.idx, x_prv, y_prv);
        float x_d = x_cur - x_prv; float y_d = y_cur - y_prv;
        float dist = x_d * x_d + y_d * y_d;
        my_mn = MIN(dist, my_mn);
      }
      if(my_mn < mn_dist) { vi.val = -1.0f; j++; continue; } // mark with -1 so we know we skipped it

      // encode the value
      lay->SetTwoDVal(x_cur, TwoDValLeabraLayer::TWOD_X, TwoDValLeabraLayer::TWOD_ACT,
                      0, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetTwoDVal(y_cur, TwoDValLeabraLayer::TWOD_Y, TwoDValLeabraLayer::TWOD_ACT,
                      0, gp_geom_pos.x, gp_geom_pos.y);
      j++; outi++;
    }
  }
}

void TwoDValLayerSpec::LabelUnits_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float x_cur, y_cur; twod.GetUnitVal(i, x_cur, y_cur);
    u->name = (String)x_cur + "," + String(y_cur);
  }
}

void TwoDValLayerSpec::LabelUnits(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, LabelUnits_ugp(lay, acc_md, gpidx); );
}

void TwoDValLayerSpec::LabelUnitsNet(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, net->layers) {
    if(l->spec.SPtr() == this)
      LabelUnits(l, net);
  }
}

void TwoDValLayerSpec::HardClampExt(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_HardClamp(lay, net);
}

void TwoDValLayerSpec::Settle_Init_TargFlags_Layer_ugp(TwoDValLeabraLayer* lay,
                                                       Layer::AccessMode acc_md, int gpidx,
                                                       LeabraNetwork* net) {
  taVector2i gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  for(int k=0;k<twod.n_vals;k++) {
    float x_val, y_val;
    lay->GetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_TARG,
                     k, gp_geom_pos.x, gp_geom_pos.y);
    lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_EXT,
                     k, gp_geom_pos.x, gp_geom_pos.y);
  }
}


void TwoDValLayerSpec::Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_TargFlags_Layer(lay, net);
  // need to actually copy over targ to ext vals!
  TwoDValLeabraLayer* tdlay = (TwoDValLeabraLayer*)lay;
  if(lay->HasExtFlag(Unit::TARG)) {     // only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      UNIT_GP_ITR(lay, Settle_Init_TargFlags_Layer_ugp(tdlay, acc_md, gpidx, net); );
    }
  }
}


void TwoDValLayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_Layer(lay, net);

  TwoDValLeabraLayer* tdlay = (TwoDValLeabraLayer*)lay;
  tdlay->UpdateTwoDValsGeom();  // quick, make sure no mismatch

  if(bias_val.un == TwoDValBias::BWT) {
    // if using bias-weight bias, keep a constant scaling (independent of layer size)
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      LeabraConSpec* bspec = (LeabraConSpec*)u->GetUnitSpec()->bias_spec.SPtr();
      u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
      u->bias_scale /= 100.0f;              // keep a constant scaling so it doesn't depend on network size!
    }
  }
}

void TwoDValLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(twod.clamp_pat) {
    inherited::Compute_HardClamp(lay, net);
    return;
  }
  if(!(lay->ext_flag & Unit::EXT)) {
    lay->hard_clamped = false;
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, ClampValue_ugp((TwoDValLeabraLayer*)lay, acc_md, gpidx, net); );
  // now check for actual hard clamping
  if(!clamp.hard) {
    lay->hard_clamped = false;
    return;
  }
  HardClampExt(lay, net);
}

void TwoDValLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  ReadValue((TwoDValLeabraLayer*)lay, net);             // always read out the value
}

void TwoDValLayerSpec::PostSettle(LeabraLayer* ly, LeabraNetwork* net) {
  inherited::PostSettle(ly, net);
  TwoDValLeabraLayer* lay = (TwoDValLeabraLayer*)ly;
  UNIT_GP_ITR(lay, PostSettle_ugp(lay, acc_md, gpidx, net); );
}

void TwoDValLayerSpec::PostSettle_ugp(TwoDValLeabraLayer* lay,
                                      Layer::AccessMode acc_md, int gpidx,
                                      LeabraNetwork* net) {
  taVector2i gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);

  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  for(int k=0;k<twod.n_vals;k++) {
    float x_val, y_val, x_m, y_m, x_p, y_p;
    lay->GetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT,
                     k, gp_geom_pos.x, gp_geom_pos.y);
    lay->GetTwoDVals(x_m, y_m, TwoDValLeabraLayer::TWOD_ACT_M,
                     k, gp_geom_pos.x, gp_geom_pos.y);
    lay->GetTwoDVals(x_p, y_p, TwoDValLeabraLayer::TWOD_ACT_P,
                     k, gp_geom_pos.x, gp_geom_pos.y);

    switch(net->phase_order) {
    case LeabraNetwork::MINUS_PLUS:
      if(no_plus_testing) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(0.0f, 0.0f, TwoDValLeabraLayer::TWOD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        if(net->phase == LeabraNetwork::MINUS_PHASE) {
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
        else {
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetTwoDVals(x_val - x_m, y_val - y_m, TwoDValLeabraLayer::TWOD_ACT_DIF,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      break;
    case LeabraNetwork::PLUS_MINUS:
      if(no_plus_testing) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(0.0f, 0.0f, TwoDValLeabraLayer::TWOD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        if(net->phase == LeabraNetwork::MINUS_PHASE) {
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                           k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetTwoDVals(x_p - x_val, y_p - y_val, TwoDValLeabraLayer::TWOD_ACT_DIF,
                           k, gp_geom_pos.x, gp_geom_pos.y);
        }
        else {
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      break;
    case LeabraNetwork::PLUS_ONLY:
      lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetTwoDVals(0.0f, 0.0f, TwoDValLeabraLayer::TWOD_ACT_DIF,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      break;
    case LeabraNetwork::MINUS_PLUS_NOTHING:
    case LeabraNetwork::MINUS_PLUS_MINUS:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 1) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_val - x_m, y_val - y_m, TwoDValLeabraLayer::TWOD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        if(no_plus_testing) {
          // update act_m because it is actually another test case!
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                           k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      else {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_p - x_val, y_p - y_val, TwoDValLeabraLayer::TWOD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    case LeabraNetwork::PLUS_NOTHING:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_p - x_val, y_p - y_val, TwoDValLeabraLayer::TWOD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    }
  }
}

float TwoDValLayerSpec::Compute_SSE_ugp(LeabraLayer* ly,
                                        Layer::AccessMode acc_md, int gpidx,
                                        int& n_vals) {
  TwoDValLeabraLayer* lay = (TwoDValLeabraLayer*)ly;
  taVector2i gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  float rval = 0.0f;
  for(int k=0;k<twod.n_vals;k++) { // first loop over and find potential target values
    float x_targ, y_targ;
    lay->GetTwoDVals(x_targ, y_targ, TwoDValLeabraLayer::TWOD_TARG, k,
                     gp_geom_pos.x, gp_geom_pos.y);
    // only count if target value is within range -- otherwise considered a non-target
    if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
      n_vals++;
      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<twod.n_vals;j++) {
        float x_act_m, y_act_m;
        lay->GetTwoDVals(x_act_m, y_act_m, TwoDValLeabraLayer::TWOD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float dx = x_targ - x_act_m;
        float dy = y_targ - y_act_m;
        if(fabsf(dx) < us->sse_tol) dx = 0.0f;
        if(fabsf(dy) < us->sse_tol) dy = 0.0f;
        float dist = dx * dx + dy * dy;
        if(dist < mn_dist) {
          mn_dist = dist;
          lay->SetTwoDVals(dx, dy, TwoDValLeabraLayer::TWOD_ERR,
                           k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetTwoDVals(dx*dx, dy*dy, TwoDValLeabraLayer::TWOD_SQERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float TwoDValLayerSpec::Compute_SSE(LeabraLayer* lay, LeabraNetwork*,
                                    int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return 0.0f;
  lay->sse = 0.0f;
  UNIT_GP_ITR(lay,
              lay->sse += Compute_SSE_ugp(lay, acc_md, gpidx, n_vals);
              );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float TwoDValLayerSpec::Compute_NormErr_ugp(LeabraLayer* ly,
                                            Layer::AccessMode acc_md, int gpidx,
                                            LeabraInhib* thr, LeabraNetwork* net) {
  TwoDValLeabraLayer* lay = (TwoDValLeabraLayer*)ly;
  taVector2i gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  float rval = 0.0f;
  for(int k=0;k<twod.n_vals;k++) { // first loop over and find potential target values
    float x_targ, y_targ;
    lay->GetTwoDVals(x_targ, y_targ, TwoDValLeabraLayer::TWOD_TARG, k,
                     gp_geom_pos.x, gp_geom_pos.y);
    // only count if target value is within range -- otherwise considered a non-target
    if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<twod.n_vals;j++) {
        float x_act_m, y_act_m;
        lay->GetTwoDVals(x_act_m, y_act_m, TwoDValLeabraLayer::TWOD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float dx = x_targ - x_act_m;
        float dy = y_targ - y_act_m;
        if(fabsf(dx) < us->sse_tol) dx = 0.0f;
        if(fabsf(dy) < us->sse_tol) dy = 0.0f;
        float dist = fabsf(dx) + fabsf(dy); // only diff from sse!
        if(dist < mn_dist)
          mn_dist = dist;
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float TwoDValLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  float ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      nerr += Compute_NormErr_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      ntot += x_range.range + y_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
    ntot += x_range.range + y_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}
