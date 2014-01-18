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

#include "LeabraV1LayerSpec.h"
#include <LeabraNetwork>

void V1FeatInhibSpec::Initialize() {
  on = true;
  n_angles = 4;
  inhib_d = 1;
  inhib_g = 0.8f;
  wrap = true;

  tot_ni_len = 2 * inhib_d + 1;
}

void V1FeatInhibSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_ni_len = 2 * inhib_d + 1;
}

void LeabraV1LayerSpec::Initialize() {
}


void LeabraV1LayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateStencils();
  if(feat_inhib.on) {
    if(inhib_group != UNIT_GROUPS)
      inhib_group = UNIT_GROUPS; // must be!
  }
}

bool LeabraV1LayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  if(!feat_inhib.on) return true;

  bool rval = true;
  lay->CheckError(!lay->unit_groups, quiet, rval,
                  "does not have unit groups -- MUST have unit groups!");

  lay->CheckError(lay->un_geom.x != feat_inhib.n_angles, quiet, rval,
                  "un_geom.x must be = to feat_inhib.n_angles -- is not!");
  return rval;
}

static void v1_get_angles(float angf, float& cosx, float& siny) {
  cosx = taMath_float::cos(angf);
  siny = taMath_float::sin(angf);
  // always normalize by the largest value so that it is equal to 1
  if(fabsf(cosx) > fabsf(siny)) {
    siny = siny / fabsf(cosx);                  // must come first!
    cosx = cosx / fabsf(cosx);
  }
  else {
    cosx = cosx / fabsf(siny);
    siny = siny / fabsf(siny);
  }
}

void LeabraV1LayerSpec::UpdateStencils() {
  v1s_ang_slopes.SetGeom(3,2,2,feat_inhib.n_angles);
  float ang_inc = taMath_float::pi / (float)feat_inhib.n_angles;
  for(int ang=0; ang<feat_inhib.n_angles; ang++) {
    float cosx, siny;
    float angf = (float)ang * ang_inc;
    v1_get_angles(angf, cosx, siny);
    v1s_ang_slopes.FastEl3d(X, LINE, ang) = cosx;
    v1s_ang_slopes.FastEl3d(Y, LINE, ang) = siny;

    v1_get_angles(angf + taMath_float::pi * .5f, cosx, siny);
    v1s_ang_slopes.FastEl3d(X, ORTHO, ang) = cosx;
    v1s_ang_slopes.FastEl3d(Y, ORTHO, ang) = siny;
  }
  // config: x,y coords by tot_ni_len, by angles
  v1s_ni_stencils.SetGeom(3, 2, feat_inhib.tot_ni_len, feat_inhib.n_angles);

  for(int ang = 0; ang < feat_inhib.n_angles; ang++) { // angles
    for(int lpt=-feat_inhib.inhib_d; lpt <= feat_inhib.inhib_d; lpt++) {
      int lpdx = lpt + feat_inhib.inhib_d;
      v1s_ni_stencils.FastEl3d(X, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl3d(X, ORTHO, ang)); // ortho
      v1s_ni_stencils.FastEl3d(Y, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl3d(Y, ORTHO, ang));
    }
  }
}


void LeabraV1LayerSpec::Compute_FeatGpActive(LeabraLayer* lay, LeabraUnit_Group* fugp,
                                             LeabraNetwork* net) {
//   fugp->active_buf.size = 0;
//   for(int ui=0; ui<fugp->size; ui++) {
//     LeabraUnit* u = (LeabraUnit*)fugp->FastEl(ui);
//     LeabraUnit_Group* u_own = (LeabraUnit_Group*)u->owner; // NOT fugp!
//     if(u->i_thr >= u_own->i_val.g_i) // compare to their own group's inhib val!
//       fugp->active_buf.Add(u);
//   }
}


void LeabraV1LayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(!feat_inhib.on) {          // do the normal
    inherited::Compute_ApplyInhib(lay, net);
    return;
  }
  if((net->cycle >= 0) && lay->hard_clamped)
    return;                     // don't do this during normal processing
  if(inhib.type == LeabraInhibSpec::UNIT_INHIB) return; // otherwise overwrites!

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  for(int gpidx=0; gpidx < lay->gp_geom.n; gpidx++) {
    LeabraUnGpData* gpd = lay->ungp_data.FastEl(gpidx);
    LeabraInhib* thr = (LeabraInhib*)gpd;
    taVector2i sc;                       // simple coords
    sc.SetFmIndex(gpidx, lay->gp_geom.x);

    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    float inhib_val = thr->i_val.g_i;
    // note: not doing tie break!

    taVector2i fc;               // v1s feature coords
    taVector2i oc;               // other coord
    float uidx = 0;
    for(int polclr = 0; polclr < lay->un_geom.y; polclr++) { // polclr features
      fc.y = polclr;
      for(int ang = 0; ang < feat_inhib.n_angles; ang++, uidx++) { // angles
        fc.x = ang;
        float feat_inhib_max = 0.0f;
        for(int lpdx=0; lpdx < feat_inhib.tot_ni_len; lpdx++) { // go out to neighs
          if(lpdx == feat_inhib.inhib_d) continue;                 // skip self
          int xp = v1s_ni_stencils.FastEl3d(X,lpdx,ang);
          int yp = v1s_ni_stencils.FastEl3d(Y,lpdx,ang);
          oc.x = sc.x + xp;
          oc.y = sc.y + yp;
          if(oc.WrapClip(feat_inhib.wrap, lay->gp_geom)) {
            if(!feat_inhib.wrap) continue; // bail on clipping only
          }
          LeabraUnit* oth_unit = (LeabraUnit*)lay->UnitAtGpCoord(oc, fc);
          float oth_netin = oth_unit->net;
          float ogi = feat_inhib.inhib_g * oth_netin; // note: directly on ithr!
          feat_inhib_max = MAX(feat_inhib_max, ogi);
        }
        float eig;
        if(inhib.type == LeabraInhibSpec::FF_FB_INHIB) {
          eig = inhib.gi * inhib.FFInhib(feat_inhib_max);
        }
        else {
          eig = us->Compute_IThreshNetinOnly(us->g_bar.e * feat_inhib_max);
        }
        float gi_eff = MAX(inhib_val, eig);
        LeabraUnit* u = (LeabraUnit*)lay->UnitAtUnGpIdx((int)uidx, gpidx);
        u->Compute_ApplyInhib(this, net, gi_eff);
      }
    }
  }
}

