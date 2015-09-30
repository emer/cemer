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

#include "BasAmygUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(BasAmygUnitSpec);

void BasAmygUnitSpec::Initialize() {
  acq_ext = ACQ;
  valence = APPETITIVE;
  dar = D1R;
  deep_vg_netin = false;
  Defaults_init();
}

void BasAmygUnitSpec::Defaults_init() {
  deep_norm.raw_val = DeepNormSpec::UNIT; // source
  deep_norm.mod = false;
  deep_norm.immed = false;
}

void BasAmygUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(acq_ext == ACQ) {
    if(valence == APPETITIVE) {
      dar = D1R;
    }
    else {
      dar = D2R;
    }
  }
  else {
    if(valence == APPETITIVE) {
      dar = D2R;                // reversed!
    }
    else {
      dar = D1R;
    }
  }
}

float BasAmygUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no, float& net_syn) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();

  float net_ex = 0.0f;
  if(bias_spec) {
    net_ex += u->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UnitVars::EXT)) {
    net_ex += u->ext * ls->clamp.gain;
  }
  if(deep.on) {
    if(deep_s.d_to_s > 0.0f) {
      if(deep_vg_netin) {
        net_ex += deep_s.d_to_s * u->deep_norm * u->act_eq;
      }
      else {
        net_ex += deep_s.d_to_s * u->deep_mod;
      }
    }
    if(deep_s.ctxt_to_s > 0.0f) {
      net_ex += deep_s.ctxt_to_s * u->deep_ctxt;
    }
    if(deep_s.thal_to_s > 0.0f) {
      net_ex += deep_s.thal_to_s * u->thal;
    }
  }
  if(da_mod.on) {
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      net_ex += da_mod.plus * u->da_p * net_syn;
    }
    else {                      // MINUS_PHASE
      net_ex += da_mod.minus * u->da_p * net_syn;
    }
  }
  return net_ex;
}

