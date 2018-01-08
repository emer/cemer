// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "LeabraLayer.h"
#include <LeabraNetwork>
#include <LeabraUnitSpec>

#include <LeabraLayerSpec_cpp>

#include <State_main>

eTypeDef_Of(TwoDValLayerSpec);

TA_BASEFUNS_CTORS_DEFN(LeabraLayer);
SMARTREF_OF_CPP(LeabraLayer);

#include "LeabraLayer_core.cpp"

void LeabraLayer::Initialize() {
  spec.SetBaseType(&TA_LeabraLayerSpec);
  projections.SetBaseType(&TA_LeabraPrjn);
  send_prjns.SetBaseType(&TA_LeabraPrjn);
  unit_spec.SetBaseType(&TA_LeabraUnitSpec);

  Initialize_lay_core();
}

void LeabraLayer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(spec, this);

  taBase::Own(margin, this);
  taBase::Own(avg_netin, this);
  taBase::Own(avg_netin_sum, this);

  spec.SetDefaultSpec(this);

  AutoNameMyMembers();
}

void LeabraLayer::CutLinks() {
  inherited::CutLinks();
  spec.CutLinks();
}

void LeabraLayer::CheckSpecs() {
  spec.CheckSpec();
  inherited::CheckSpecs();
}

void LeabraLayer::CheckThisConfig_impl(bool quiet, bool& rval) {
  if(lesioned()) return;
  inherited::CheckThisConfig_impl(quiet, rval);

  if(!spec->CheckConfig_Layer(this, quiet)) {
    rval = false;
  }
}

bool LeabraLayer::SetLayerSpec(LayerSpec* sp) {
  if(sp == NULL)        return false;
  if(sp->CheckObjectType(this)) {
    spec.SetSpec((LeabraLayerSpec*)sp);
  }
  else {
    return false;
  }
  return true;
}

bool LeabraLayer::TwoDValMode() {
  LeabraLayerSpec* ls = (LeabraLayerSpec*)GetMainLayerSpec();
  if(!ls) return false;
  return (ls->InheritsFrom(&TA_TwoDValLayerSpec));
}

void LeabraLayer::ApplyInputData_2d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                    Random* ran, const taVector2i& offs, bool na_by_range) {
  if(TwoDValMode()) {
    // only no unit_group supported!
    if(TestError(unit_groups, "ApplyInputData_2d",
                 "input data must be 4d for layers with unit_groups: outer 2 are group dims, inner 2 are x,y vals and n_vals")) {
      return;
    }
    if(TestError(data->dim(0) < 2, "ApplyInputData_2d",
                 "input data must have at inner dimension of at least 2 for x,y vals")) {
      return;
    }
    LEABRA_UNGP_STATE* lgpd = GetLayUnGpState(net);
    
    // todo: not supporting multiple bumps yet!
    int mx = MIN(1, data->dim(1));
    for(int d_y = 0; d_y < mx; d_y++) {
      if(ext_flags & EXT) {
        lgpd->twod.ext_x = data->SafeElAsFloat(0, d_y);
        lgpd->twod.ext_y = data->SafeElAsFloat(1, d_y);
      }
      else {                    // targ
        lgpd->twod.targ_x = data->SafeElAsFloat(0, d_y);
        lgpd->twod.targ_y = data->SafeElAsFloat(1, d_y);
      }
    }
  }
  else {
    inherited::ApplyInputData_2d(net, data, ext_flags, ran, offs, na_by_range);
  }
}

void LeabraLayer::ApplyInputData_Gp4d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                      Random* ran, bool na_by_range) {
  if(TwoDValMode()) {
    // outer-loop is data-group (groups of x-y data items)
    for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
      for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {
        // todo: not supporting multiple bumps yet!
        int mx = MIN(1, data->dim(1));
        for(int d_y = 0; d_y < mx; d_y++) {
          LEABRA_UNGP_STATE* gpd = (LEABRA_UNGP_STATE*)GetUnGpStateXY(net, dg_x, dg_y);
          if(!gpd) continue;
          if(ext_flags & EXT) {
            gpd->twod.ext_x = data->SafeElAsFloat(0, d_y, dg_x, dg_y);
            gpd->twod.ext_y = data->SafeElAsFloat(1, d_y, dg_x, dg_y);
          }
          else {                    // targ
            gpd->twod.targ_x = data->SafeElAsFloat(0, d_y, dg_x, dg_y);
            gpd->twod.targ_y = data->SafeElAsFloat(1, d_y, dg_x, dg_y);
          }
        }
      }
    }
  }
  else {
    inherited::ApplyInputData_Gp4d(net, data, ext_flags, ran, na_by_range);
  }
}
