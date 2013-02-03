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

#include "V1EndStopPrjnSpec.h"
#include <Network>
#include <taMath_float>

void V1EndStopPrjnSpec::Initialize() {
  n_angles = 4;
  end_stop_dist = 2;
  adjang_wt = 0.2f;
  wrap = true;
  init_wts = true;
}

void V1EndStopPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  wrap = true;
}

static void es_get_angles(float angf, float& cosx, float& siny) {
  cosx = taMath_float::cos(angf);
  siny = taMath_float::sin(angf);
  // always normalize by the largest value so that it is equal to 1
  // not for these guys!!
//   if(fabsf(cosx) > fabsf(siny)) {
//     siny = siny / fabsf(cosx);                       // must come first!
//     cosx = cosx / fabsf(cosx);
//   }
//   else {
//     cosx = cosx / fabsf(siny);
//     siny = siny / fabsf(siny);
//   }
}

void V1EndStopPrjnSpec::InitStencils(Projection* prjn) {
  v1s_ang_slopes.SetGeom(3,2,2,n_angles);
  float ang_inc = taMath_float::pi / (float)n_angles;
  for(int ang=0; ang<n_angles; ang++) {
    float cosx, siny;
    float angf = (float)ang * ang_inc;
    es_get_angles(angf, cosx, siny);
    v1s_ang_slopes.FastEl(X, LINE, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, LINE, ang) = siny;

    es_get_angles(angf + taMath_float::pi * .5f, cosx, siny);
    v1s_ang_slopes.FastEl(X, ORTHO, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, ORTHO, ang) = siny;
  }

  v1c_es_stencils.SetGeom(4, 2, 2, 2, n_angles);

  if(n_angles == 4) {
    v1c_es_stencils.FastEl(X, 0, 0, 0) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 0, 0) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 0, 0) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 0, 0) = end_stop_dist;
    v1c_es_stencils.FastEl(X, 0, 1, 0) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 1, 0) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 1, 0) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 1, 0) = end_stop_dist;

    v1c_es_stencils.FastEl(X, 0, 0, 2) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 0, 2) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 0, 2) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 0, 2) = end_stop_dist;
    v1c_es_stencils.FastEl(X, 0, 1, 2) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 1, 2) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 1, 2) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 1, 2) = end_stop_dist;

    v1c_es_stencils.FastEl(X, 0, 0, 1) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 0, 1) = 0;
    v1c_es_stencils.FastEl(X, 1, 0, 1) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 0, 1) = 0;
    v1c_es_stencils.FastEl(X, 0, 1, 1) = 0;
    v1c_es_stencils.FastEl(Y, 0, 1, 1) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 1, 1) = 0;
    v1c_es_stencils.FastEl(Y, 1, 1, 1) = end_stop_dist;

    v1c_es_stencils.FastEl(X, 0, 0, 3) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 0, 3) = 0;
    v1c_es_stencils.FastEl(X, 1, 0, 3) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 0, 3) = 0;
    v1c_es_stencils.FastEl(X, 0, 1, 3) = 0;
    v1c_es_stencils.FastEl(Y, 0, 1, 3) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 1, 3) = 0;
    v1c_es_stencils.FastEl(Y, 1, 1, 3) = end_stop_dist;
  }
  else {
    for(int ang=0; ang < n_angles; ang++) {
      for(int sidx=0; sidx < 2; sidx++) {
        int side = (sidx == 0) ? -1 : 1;

        int sx = (int)(end_stop_dist * (float)side * v1s_ang_slopes.FastEl(X, LINE, ang));
        int sy = (int)(end_stop_dist * (float)side * v1s_ang_slopes.FastEl(Y, LINE, ang));
        for(int lpdx=0; lpdx < 2; lpdx++) {
          int lpt = (lpdx == 0) ? -1 : 1;
          v1c_es_stencils.FastEl(X, lpdx, sidx, ang) = sx +
            (int)(end_stop_dist *(float)lpt * v1s_ang_slopes.FastEl(X, ORTHO, ang));
          v1c_es_stencils.FastEl(Y, lpdx, sidx, ang) = sy +
            (int)(end_stop_dist * (float)lpt * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
        }
      }
    }
  }

  v1c_es_angwts.SetGeom(2, n_angles, n_angles);
  for(int ang1=0; ang1 < n_angles; ang1++) {
    for(int ang2=0; ang2 < n_angles; ang2++) {
      int ang_dst = ABS(ang1-ang2);
      {
        int wrap_dst = ABS(ang1 - (ang2 + n_angles));
        if(wrap_dst < ang_dst) ang_dst = wrap_dst;
      }
      {
        int wrap_dst = ABS(ang1 - (ang2 - n_angles));
        if(wrap_dst < ang_dst) ang_dst = wrap_dst;
      }
      float wt;
      if(ang_dst == 0)
        wt = 0.0f;
      else if(ang_dst == 1)
        wt = adjang_wt;
      else
        wt = 1.0f;
      v1c_es_angwts.FastEl(ang1, ang2) = wt;
    }
  }
}

void V1EndStopPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  if(TestWarning(recv_lay->gp_geom != send_lay->gp_geom, "Connect_impl",
                 "Recv layer does not have same gp geometry as sending layer -- cannot connect!")) {
    return;
  }

  taVector2i gp_geo = recv_lay->gp_geom; // same as sgp

  taVector2i run_geo = recv_lay->un_geom;
  taVector2i sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  if(TestWarning(run_geo.x != sun_geo.x, "Connect_impl",
                 "Recv layer does not have same x-dimension unit geometry within unit groups as sending layer -- cannot connect!")) {
    return;
  }
  if(TestWarning(run_geo.y != 1, "Connect_impl",
                 "Recv layer should have a single y-dimension unit geometry within unit groups")) {
    return;
  }

  InitStencils(prjn);

  int n_cons = sun_geo.y + 2 * 2 * (n_angles-1) * sun_geo.y;

  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < gp_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < gp_geo.x; ruc.x++, rgpidx++) {

        for(int ang = 0; ang < n_angles; ang++) { // angles
          int rui = ang;
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          if(!ru_u) continue;
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(n_cons, prjn);

          taVector2i sun;
          for(sun.y = 0; sun.y < sun_geo.y; sun.y++) {
            // central point (same as ruc -- use rgpidx)
            int sui = sun.y * n_angles + ang;
            Unit* su_u = send_lay->UnitAtUnGpIdx(sui, rgpidx);
            if(!su_u) continue;
            ru_u->ConnectFrom(su_u, prjn, alloc_loop);

            for(int sidx=0; sidx < 2; sidx++) {
              for(int lpdx=0; lpdx < 2; lpdx++) {
                int xp = v1c_es_stencils.FastEl(X,lpdx,sidx,ang);
                int yp = v1c_es_stencils.FastEl(Y,lpdx,sidx,ang);

                taVector2i suc = ruc;
                suc.x += xp;  suc.y += yp;
                taVector2i suc_wrp = suc;
                if(suc_wrp.WrapClip(wrap, gp_geo) && !wrap)
                  continue;
                int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
                if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

                for(int opang=0; opang<n_angles; opang++) {
                  float angwt = v1c_es_angwts.FastEl(ang, opang);
                  if(angwt == 0.0f) continue;

                  int sui = sun.y * n_angles + opang;
                  Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
                  if(!su_u) continue;
                  ru_u->ConnectFrom(su_u, prjn, alloc_loop);
                }
              }
            }
          }
        }  // for ang
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void V1EndStopPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i gp_geo = recv_lay->gp_geom; // same as sgp

  taVector2i run_geo = recv_lay->un_geom;
  taVector2i sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  int n_cons = sun_geo.y + 2 * 2 * (n_angles-1) * sun_geo.y;
  if(cg->size != n_cons) return; // no can do

  int rgpidx;
  int rui;
  recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);

  int ang = rui;

  int cnidx = 0;
  taVector2i sun;
  for(sun.y = 0; sun.y < sun_geo.y; sun.y++) {
    cg->Cn(cnidx++)->wt = 1.0f; // center point
    for(int sidx=0; sidx < 2; sidx++) {
      for(int lpdx=0; lpdx < 2; lpdx++) {
        for(int opang=0; opang<n_angles; opang++) {
          float angwt = v1c_es_angwts.FastEl(rui, opang);
          if(angwt == 0.0f) continue;
          cg->Cn(cnidx++)->wt = angwt;
        }
      }
    }
  }
}

