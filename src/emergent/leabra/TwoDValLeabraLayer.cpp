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

#include "TwoDValLeabraLayer.h"
#include <TwoDValLayerSpec>
#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(TwoDValLeabraLayer);

void TwoDValLeabraLayer::Initialize() {
}

void TwoDValLeabraLayer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateTwoDValsGeom();
}

void TwoDValLeabraLayer::UpdateTwoDValsGeom() {
  TwoDValLayerSpec* ls = (TwoDValLayerSpec*)GetLayerSpec();
  if(!ls) return;
  if(ls->InheritsFrom(&TA_TwoDValLayerSpec)) {
    if(unit_groups)
      twod_vals.SetGeom(5, 2, TWOD_N, ls->twod.n_vals, gp_geom.x, gp_geom.y);
    else
      twod_vals.SetGeom(5, 2, TWOD_N, ls->twod.n_vals, 1, 1);
  }
}

void TwoDValLeabraLayer::ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
                              Random* ran, const taVector2i& offs, bool na_by_range) {
  // only no unit_group supported!
  if(TestError(unit_groups, "ApplyInputData_2d",
               "input data must be 4d for layers with unit_groups: outer 2 are group dims, inner 2 are x,y vals and n_vals")) {
    return;
  }
  for(int d_y = 0; d_y < data->dim(1); d_y++) {
    int val_idx = offs.y + d_y;
    for(int d_x = 0; d_x < data->dim(0); d_x++) {
      int xy_idx = offs.x + d_x;
      Variant val = data->SafeElAsVar(d_x, d_y);
      if(ext_flags & Unit::EXT)
        twod_vals.SetFmVar(val, xy_idx, TWOD_EXT, val_idx, 0, 0);
      else
        twod_vals.SetFmVar(val, xy_idx, TWOD_TARG, val_idx, 0, 0);
    }
  }
}

void TwoDValLeabraLayer::ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
                                  Random* ran, const taVector2i& offs, bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  if(TestError(!unit_groups, "ApplyInputData_Flat4d",
               "input data must be 2d for layers without unit_groups: x,y vals and n_vals")) {
    return;
  }
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int u_y = offs.y + dg_y * data->dim(1) + d_y; // multiply out data indicies
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int u_x = offs.x + dg_x * data->dim(0) + d_x; // multiply out data indicies
          Unit* un = UnitAtCoord(u_x, u_y);
          if(un) {
            float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
            un->ApplyInputData(val, ext_flags, ran, na_by_range);
          }
        }
      }
    }
  }
}

void TwoDValLeabraLayer::ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags, Random* ran,
                                bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int val_idx = d_y;
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int xy_idx = d_x;
          Variant val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y);
          if(ext_flags & Unit::EXT)
            twod_vals.SetFmVar(val, xy_idx, TWOD_EXT, val_idx, dg_x, dg_y);
          else
            twod_vals.SetFmVar(val, xy_idx, TWOD_TARG, val_idx, dg_x, dg_y);
        }
      }
    }
  }
}
