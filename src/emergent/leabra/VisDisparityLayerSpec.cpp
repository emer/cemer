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

#include "VisDisparityLayerSpec.h"
#include <LeabraNetwork>
#include <VisDisparityPrjnSpec>

TA_BASEFUNS_CTORS_DEFN(VisDispLaySpec);

TA_BASEFUNS_CTORS_DEFN(VisDisparityLayerSpec);


void VisDispLaySpec::Initialize() {
  max_l = true;
  incl_other_res = true;
  updt_cycles = 5;
}

void VisDisparityLayerSpec::Initialize() {
//   clamp.hard = false;
}

bool VisDisparityLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);
  if(!rval) return rval;

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);
  if(lay->CheckError(u == NULL, quiet, rval,
                "leabra vis disparity layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }
  if(lay->CheckError(u->recv.size < 2, quiet, rval,
                     "leabra vis disparity units must have >= 2 projections -- less than that:",
                     lay->name)) {
    return false;               // fatal
  }

  LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
  if(lay->CheckError(!cg->prjn->spec.SPtr()->InheritsFrom(&TA_VisDisparityPrjnSpec), quiet, rval,
                     "leabra vis disparity units must have 1st prjn = VisDisparityPrjnSpec -- not",
                     lay->name)) {
    return false;               // fatal
  }
  cg = (LeabraRecvCons*)u->recv.SafeEl(1);
  if(lay->CheckError(!cg->prjn->spec.SPtr()->InheritsFrom(&TA_VisDisparityPrjnSpec), quiet, rval,
                     "leabra vis disparity units must have 2nd prjn = VisDisparityPrjnSpec -- not",
                     lay->name)) {
    return false;               // fatal
  }

  return rval;
}

void VisDisparityLayerSpec::ComputeDispToExt(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    float right = 0.0f;
    float left = 0.0f;

    LeabraRecvCons* cg_r = (LeabraRecvCons*)u->recv.SafeEl(0);
    if(!cg_r || cg_r->size <= 0) return;
    LeabraUnit* su_r = (LeabraUnit*)cg_r->Un(0,net);
    right = su_r->act_eq;

    LeabraRecvCons* cg_l = (LeabraRecvCons*)u->recv.SafeEl(1);
    if(!cg_l || cg_l->size <= 0) return;
    for(int i=0; i < cg_l->size; i++) {
      LeabraUnit* su_l = (LeabraUnit*)cg_l->Un(i,net);
      float wt = cg_l->PtrCn(i,BaseCons::WT,net); // recv mode
      float itm = wt * su_l->act_eq;
      if(disp.max_l)
        left = MAX(left, itm);
      else
        left += itm;
    }
    if(!disp.max_l) {
      left /= (float)cg_l->size;
    }

    float prod = MIN(left, right); // min = product!
    u->misc_1 = prod;

    if(disp.incl_other_res && net->cycle > 1) {
      for(int j=2; j<u->recv.size; j++) {
        LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.FastEl(j);
        LeabraLayer* fm = (LeabraLayer*)cg->prjn->from.ptr();
        LeabraLayerSpec* ls = (LeabraLayerSpec*)fm->spec.SPtr();
        if(!ls->InheritsFrom(&TA_VisDisparityLayerSpec)) continue;
        float netin = 0.0f;
        for(int i=0; i < cg->size; i++) {
          LeabraUnit* su = (LeabraUnit*)cg->Un(i,net);
//        float wt = cg->PtrCn(i,BaseCons::WT,net); // recv mode
//        float itm = wt * su->misc_1;          // note: using misc_1 prod val!
          float itm = su->misc_1; // no need for weight!
          if(disp.max_l)
            netin = MAX(netin, itm);
          else
            netin += itm;
        }
        if(!disp.max_l) {
          if(cg->size > 0)
            netin /= (float)cg->size;
        }
        prod = MIN(netin, prod);
      }
    }

    u->SetExtFlag(Unit::EXT);
    u->ext = prod;
  }
  // todo: deal with horiz apeture prob
}

void VisDisparityLayerSpec::Compute_ExtraNetin(LeabraLayer* lay, LeabraNetwork* net) {
  if(disp.updt_cycles < 0 || net->cycle <= disp.updt_cycles)
    ComputeDispToExt(lay, net); // always do it here -- avail for softclamp
}

void VisDisparityLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(!clamp.hard) {
    inherited::Compute_CycleStats(lay, net);
    return;
  }
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->act = u->ext;
    u->act_lrn = u->act_eq = u->act_nd = u->act;
    u->da = 0.0f;               // I'm fully settled!
  }
  inherited::Compute_CycleStats(lay, net);
}
