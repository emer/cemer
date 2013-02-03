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

#include "FgBoWedgeGpPrjnSpec.h"
#include <Network>
#include <taMath_float>

void FgBoWedgeGpPrjnSpec::Initialize() {
  wrap = true;
  init_wts = true;
  dist_sigma = 0.8f;
  ang_sigma = 1.0f;
  wt_base = 0.25f;
  wt_range = 0.5f;
}

void FgBoWedgeGpPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CreateStencil();
}

void FgBoWedgeGpPrjnSpec::CreateStencil() {
  int n_angles = 4;
  int n_wedges = 8;
  fgbo_weights.SetGeom(5, send_gp_size.x, send_gp_size.y, 2, n_angles, n_wedges);
  taVector2i suc;                        // send coords
  for(int wedge=0; wedge < n_wedges; wedge++) {
    for(suc.y = 0; suc.y < send_gp_size.y; suc.y++) {
      for(suc.x = 0; suc.x < send_gp_size.x; suc.x++) {
        for(int sang_dx = 0; sang_dx < n_angles; sang_dx++) {
          for(int sdir = 0; sdir < 2; sdir++) { // integrate over sending directions
            float wt;
            if(wedge < 4)
              wt = ConWt_Wedge(wedge, suc, send_gp_size, sang_dx, sdir);
            else
              wt = ConWt_Line(wedge-4, suc, send_gp_size, sang_dx, sdir);
            fgbo_weights.FastEl(suc.x, suc.y, sdir, sang_dx, wedge) = wt;
          }
        }
      }
    }
  }
}

float FgBoWedgeGpPrjnSpec::ConWt_Wedge(int wedge, taVector2i& suc, taVector2i& su_geo, int sang_dx, int sdir) {
  float n_angles = 4.0f;

  taVector2i ctr;
  switch(wedge) {
  case 0:
    ctr.SetXY(0,0);
    break;
  case 1:
    ctr.SetXY(su_geo.x-1,0);
    break;
  case 2:
    ctr.SetXY(su_geo.x-1,su_geo.y-1);
    break;
  case 3:
    ctr.SetXY(0,su_geo.y-1);
    break;
  }

  taVector2i del = suc - ctr;
  float dst = del.Mag();
  float nrmdst = dst / (float)su_geo.x; // assume square

  float gang = atan2f(del.y, del.x); // group angle -- 0..pi or -pi
  if(gang < 0.0f) gang += 2.0f * taMath_float::pi; // keep it positive

  float pang = gang + 0.5f * taMath_float::pi; // perpendicular angle
  if(pang > 2.0f * taMath_float::pi) pang -= 2.0f * taMath_float::pi;

  // dir 0 = 0..pi, dir 1 = pi..2pi
  float sang = taMath_float::pi * ((float)sang_dx / n_angles) + taMath_float::pi * (float)sdir;

  float dang;                   // delta-angle -- keep this positive too
  if(sang < pang)
    dang = (2.0f * taMath_float::pi + sang) - pang;
  else
    dang = sang - pang;
  if(dang >= taMath_float::pi) dang = (2.0f * taMath_float::pi) - dang;

  float netwt = wt_base + wt_range * taMath_float::gauss_den_nonorm(dang, ang_sigma)
    * taMath_float::gauss_den_nonorm((nrmdst-1.0f), dist_sigma);

  return netwt;
}

float FgBoWedgeGpPrjnSpec::ConWt_Line(int line, taVector2i& suc, taVector2i& su_geo, int sang_dx, int sdir) {
  float n_angles = 4.0f;

  taVector2i su_geo_half = su_geo / 2;

  float dst;
  switch(line) {
  case 0:
  case 2:
    dst = (float)(suc.y - su_geo_half.y);
    break;
  case 1:
  case 3:
    dst = (float)(suc.x - su_geo_half.x);
    break;
  }

  float pang = (float)(line) * 0.5f * taMath_float::pi;
  float nrmdst = dst / (float)su_geo.x; // assume square

  // dir 0 = 0..pi, dir 1 = pi..2pi
  float sang = taMath_float::pi * ((float)sang_dx / n_angles) + taMath_float::pi * (float)sdir;

  float dang;                   // delta-angle -- keep this positive too
  if(sang < pang)
    dang = (2.0f * taMath_float::pi + sang) - pang;
  else
    dang = sang - pang;
  if(dang >= taMath_float::pi) dang = (2.0f * taMath_float::pi) - dang;

  float netwt = wt_base + wt_range * taMath_float::gauss_den_nonorm(dang, ang_sigma)
    * taMath_float::gauss_den_nonorm(nrmdst, dist_sigma);

  return netwt;
}

void FgBoWedgeGpPrjnSpec::Connect_impl(Projection* prjn) {
  CreateStencil();
  inherited::Connect_impl(prjn);
}

void FgBoWedgeGpPrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                Layer* send_lay, int rgpidx, int sgpidx, int alloc_loop) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;
  int n_wedges = 8;

  taVector2i sug;
  if(reciprocal) {              // reciprocal is backwards!
    for(int sui=0; sui < su_nunits; sui++) {
      sug.SetFmIndex(sui, send_lay->un_geom.x);
      int cur_depth = sug.y / 2; // v2bo has 2 per depth always
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        int ru_depth = rui / n_wedges;
        if(cur_depth != ru_depth) continue;
        if(!self_con && (su_u == ru_u)) continue;
        su_u->ConnectFrom(ru_u, prjn, alloc_loop); // recip!
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      int ru_depth = rui / n_wedges;
      for(int sui=0; sui < su_nunits; sui++) {
        sug.SetFmIndex(sui, send_lay->un_geom.x);
        int cur_depth = sug.y / 2; // v2bo has 2 per depth always
        if(cur_depth != ru_depth) continue;
        Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        ru_u->ConnectFrom(su_u, prjn, alloc_loop); // recip!
      }
    }
  }
}

void FgBoWedgeGpPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  if(cg->size == 0) return;
  int n_wedges = 8;

  if(reciprocal) {
    Layer* recv_lay = prjn->from;
    Layer* send_lay = prjn->layer;
    taVector2i sgp_geo = send_lay->gp_geom;
    taVector2i sgp_geo_half = sgp_geo / 2;

    int sgpidx;
    int sui;
    send_lay->UnGpIdxFmUnitIdx(ru->idx, sui, sgpidx);  // recv = send!
    taVector2i suc;
    suc.SetFmIndex(sui, send_lay->un_geom.x);
    taVector2i sgpc;
    sgpc.SetFmIndex(sgpidx, send_lay->gp_geom.x);
    int sdir = suc.y % 2;
    int sang_dx = suc.x;

    for(int i=0; i < cg->size; i++) {
      Unit* su = cg->Un(i);     // this is actually ru
      int rgpidx;
      int rui;
      recv_lay->UnGpIdxFmUnitIdx(su->idx, rui, rgpidx); // recv = send!
      int wedge = rui % n_wedges;

      RecvCons* rucg = su->recv.FindFrom(send_lay); // recip prjn
      if(!rucg) continue;
      Unit* ssu = rucg->Un(0);  // first sending unit in that guy's prjn
      int ssgpidx;
      int ssui;
      send_lay->UnGpIdxFmUnitIdx(ssu->idx, ssui, ssgpidx);
      taVector2i ssgpc;
      ssgpc.SetFmIndex(ssgpidx, send_lay->gp_geom.x);

      taVector2i del = sgpc - ssgpc;
      sgpc.WrapMinDist(del, sgp_geo, ssgpc, sgp_geo_half);

      float wt = fgbo_weights.FastEl(del.x, del.y, sdir, sang_dx, wedge);
      cg->Cn(i)->wt = wt;
    }
  }
  else {
    Layer* recv_lay = prjn->layer;
    Layer* send_lay = prjn->from;
    taVector2i sgp_geo = send_lay->gp_geom;
    taVector2i sgp_geo_half = sgp_geo / 2;

    int rgpidx;
    int rui;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
    int wedge = rui % n_wedges;

    Unit* ssu = cg->Un(0);
    int ssgpidx;
    int ssui;
    send_lay->UnGpIdxFmUnitIdx(ssu->idx, ssui, ssgpidx);
    taVector2i ssgpc;
    ssgpc.SetFmIndex(ssgpidx, send_lay->gp_geom.x);

    for(int i=0; i < cg->size; i++) {
      Unit* su = cg->Un(i);
      int sgpidx;
      int sui;
      send_lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
      taVector2i suc;
      suc.SetFmIndex(sui, send_lay->un_geom.x);
      taVector2i sgpc;
      sgpc.SetFmIndex(sgpidx, send_lay->gp_geom.x);

      taVector2i del = sgpc - ssgpc;
      sgpc.WrapMinDist(del, sgp_geo, ssgpc, sgp_geo_half);

      int sdir = suc.y % 2;
      int sang_dx = suc.x;
      float wt = fgbo_weights.FastEl(del.x, del.y, sdir, sang_dx, wedge);
      cg->Cn(i)->wt = wt;
    }
  }
}
