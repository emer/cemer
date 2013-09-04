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

#include "FgBoEllipseGpPrjnSpec.h"
#include <Network>
#include <taMath_float>


void FgBoEllipseGpPrjnEl::Initialize() {
  con_radius = 14;
  wt_radius = 0.7f;
  dist_sig = 0.3f;
  ang_sig = 1.0f;
  ellipse_ratio = 1.0f;
  ellipse_angle = 0.0f;
  max_wt = 1.0f;
  min_wt = 0.1f;
  con_thr = 0.2f;
}

void FgBoEllipseGpPrjnEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CreateStencil();
}

void FgBoEllipseGpPrjnEl::CreateStencil() {
  int n_angles = 4;
  int max_cnt = (2 * con_radius + 1);
  fgbo_weights.SetGeom(4, max_cnt, max_cnt, 2, n_angles);
  taVector2i suc;                        // send coords
  int cnt = 0;
  for(suc.y = -con_radius; suc.y <= con_radius; suc.y++) {
    int ysuc_dx = suc.y + con_radius;
    for(suc.x = -con_radius; suc.x <= con_radius; suc.x++) {
      int xsuc_dx = suc.x + con_radius;
      for(int sang_dx = 0; sang_dx < n_angles; sang_dx++) {
        for(int sdir = 0; sdir < 2; sdir++) { // integrate over sending directions
          float wt = ConWt(suc, sang_dx, sdir);
          fgbo_weights.FastEl(xsuc_dx, ysuc_dx, sdir, sang_dx) = wt;
        }
      }
    }
  }
}

float FgBoEllipseGpPrjnEl::ConWt(taVector2i& suc, int sang_dx, int sdir) {
  float n_angles = 4.0f;

  taVector2i del = suc;
  float dst = del.Mag();
  if(dst > (float)con_radius) return 0.0f;
  if(dst == 0.0f) return 0.0f;  // no self con
  float nrmdst = dst / (float)con_radius;

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

  if(ellipse_ratio < 1.0f) {
    float a = (float)con_radius;
    float b = ellipse_ratio * a;
    float elang = ellipse_angle * taMath_float::rad_per_deg;
    float R = (b*b - a*a) * cosf(2.0f * gang - 2.0f * elang) + a*a + b*b;
    float Q = sqrtf(2) * a * b * sqrtf(R);
    float r = Q / R;
    nrmdst = dst / r;           // normalize relative to outer radius!
  }

  float netwt = max_wt * taMath_float::gauss_den_nonorm(dang, ang_sig)
    * taMath_float::gauss_den_nonorm((nrmdst-wt_radius), dist_sig);

  return netwt;
}

void FgBoEllipseGpPrjnSpec::Initialize() {
  init_wts = true;
  wrap = true;
  reciprocal = false;
  group_specs.SetBaseType(&TA_FgBoEllipseGpPrjnEl);
}

void FgBoEllipseGpPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CreateStencils();
}

// static int dbg_hit_cnt = 0;

FgBoEllipseGpPrjnEl* FgBoEllipseGpPrjnSpec::NewGroupSpec() {
  return (FgBoEllipseGpPrjnEl*)group_specs.New(1);
}

void FgBoEllipseGpPrjnSpec::CreateStencils() {
  for(int i=0; i < group_specs.size; i++) {
    FgBoEllipseGpPrjnEl* el = group_specs.FastEl(i);
    el->CreateStencil();
  }
}

void FgBoEllipseGpPrjnSpec::Connect_impl(Projection* prjn) {
//   dbg_hit_cnt = 0;           // debugging

  if(!(bool)prjn->from) return;

  if(TestWarning(group_specs.size == 0, "Connect_impl",
                 "group_specs must have at least one item!")) {
    return;
  }
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }
//   if(TestWarning(prjn->con_spec->wt_limits.sym, "Connect_impl",
//               "cannot have wt_limits.sym on in conspec -- turning off in spec:",
//               prjn->con_spec->name,
//               "This might affect children of this spec.")) {
//     prjn->con_spec->SetUnique("wt_limits", true);
//     prjn->con_spec->wt_limits.sym = false;
//     prjn->con_spec->UpdateAfterEdit();
//   }

  CreateStencils();

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {              // switch!
    recv_lay = prjn->from;
    send_lay = prjn->layer;
  }
  taVector2i rgp_geo = recv_lay->gp_geom;
  taVector2i run_geo = recv_lay->un_geom;
  taVector2i sgp_geo = send_lay->gp_geom;
  taVector2i sun_geo = send_lay->un_geom;

  int n_depth = sun_geo.y / 2;  // number of different depths represented
  int ruy_per_depth = run_geo.y / n_depth;
  int run_per_depth = recv_lay->un_geom.n / n_depth;
  if(TestWarning(ruy_per_depth * n_depth != run_geo.y, "Connect_impl",
                 "recv layer un_geom.y must be even multiple of number of depths represented in the input:", String(n_depth))) {
    return;
  }

  float n_angles = (float)sun_geo.x;

  if(TestWarning(run_per_depth != group_specs.size, "Connect_impl",
                 "recv layer units per depth is not same as number of group specs -- should be -- redundant or incomplete connections will be created")) {
  }

  taVector2i rgp_sc = sgp_geo / rgp_geo;
  taVector2i rgp_chk = rgp_geo * rgp_sc;

  if(TestWarning(rgp_chk != sgp_geo, "Connect_impl",
                 "recv layer gp_geom is not an even multiple of send layer gp_geom -- should be!  Some units will not be connected", rgp_sc.GetStr(), rgp_chk.GetStr())) {
  }


  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rcnt = 0;
    taVector2i run;
    for(run.y = 0; run.y < run_geo.y; run.y++) {
      int depth = run.y / ruy_per_depth;
      int suy_st = depth * 2;
      for(run.x = 0; run.x < run_geo.x; run.x++, rcnt++) {
        FgBoEllipseGpPrjnEl* el = group_specs.FastEl(rcnt % group_specs.size);

        float wt_renorm = (1.0f - el->min_wt);

        int rgpidx = 0;
        for(ruc.y = 0; ruc.y < rgp_geo.y; ruc.y++) {
          for(ruc.x = 0; ruc.x < rgp_geo.x; ruc.x++, rgpidx++) {
            taVector2i ruc_s = ruc * rgp_sc; // project ruc into s coords

            taVector2i suc;
            taVector2i suc_wrp;
            for(suc.y = ruc_s.y-el->con_radius; suc.y <= ruc_s.y+el->con_radius; suc.y++) {
              for(suc.x = ruc_s.x-el->con_radius; suc.x <= ruc_s.x+el->con_radius; suc.x++) {
                suc_wrp = suc;
                if(suc_wrp.WrapClip(wrap, sgp_geo) && !wrap)
                  continue;
                int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
                if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

                taVector2i del = suc - ruc_s; // don't use wrap!
                float dst = del.Mag();
                if(dst > (float)el->con_radius) continue; // out of bounds
                if(dst == 0.0f) continue;             // no selfs

                taVector2i sun;

                for(sun.x = 0; sun.x < sun_geo.x; sun.x++) {
                  for(sun.y = suy_st; sun.y < suy_st+2; sun.y++) {
                    float wt = el->fgbo_weights.FastEl(del.x +el->con_radius,
                                               del.y+el->con_radius, sun.y-suy_st, sun.x);
                    if(wt <= el->con_thr) continue;
                    wt = wt * wt_renorm + el->min_wt; // renorm to min wt range

                    int rui = run.y * run_geo.x + run.x;
                    int sui = sun.y * sun_geo.x + sun.x;

                    Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
                    if(!ru_u) continue;
                    Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
                    if(!su_u) continue;
                    if(alloc_loop) {
                      if(reciprocal) {
                        su_u->RecvConsAllocInc(1, prjn);
                        ru_u->SendConsAllocInc(1, prjn);
                      }
                      else {
                        ru_u->RecvConsAllocInc(1, prjn);
                        su_u->SendConsAllocInc(1, prjn);
                      }
                    }
                    else {
                      if(reciprocal) {
                        Connection* cn = su_u->ConnectFrom(ru_u, prjn, alloc_loop);
                        if(cn) cn->wt = wt;
                      }
                      else {
                        Connection* cn = ru_u->ConnectFrom(su_u, prjn, alloc_loop);
                        if(cn) cn->wt = wt;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do allocations
      prjn->layer->RecvConsPostAlloc(prjn);
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void FgBoEllipseGpPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {              // switch!
    recv_lay = prjn->from;
    send_lay = prjn->layer;
  }
  Network* net = prjn->layer->own_net;
  taVector2i rgp_geo = recv_lay->gp_geom;
  taVector2i run_geo = recv_lay->un_geom;
  taVector2i sgp_geo = send_lay->gp_geom;
  taVector2i sun_geo = send_lay->un_geom;
  float n_angles = (float)sun_geo.x;
  int n_depth = sun_geo.y / 2;  // number of different depths represented
  int ruy_per_depth = run_geo.y / n_depth;

  taVector2i rgp_sc = sgp_geo / rgp_geo;
  taVector2i sgp_geo_half = sgp_geo / 2;

  if(reciprocal) {
    int sgpidx;
    int sui;
    send_lay->UnGpIdxFmUnitIdx(ru->idx, sui, sgpidx); // send = recv!
    taVector2i suc = send_lay->UnitGpPosFmIdx(sgpidx);
    taVector2i sun;
    sun.SetFmIndex(sui, sun_geo.x);
    int depth = sun.y / 2;
    int suy_st = depth * 2;

    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i,net);
      int rgpidx;
      int rui;
      recv_lay->UnGpIdxFmUnitIdx(su->idx, rui, rgpidx); // recv = send!
      taVector2i ruc = recv_lay->UnitGpPosFmIdx(rgpidx);
      taVector2i ruc_s = ruc * rgp_sc; // project ruc into s coords
      taVector2i run;
      run.SetFmIndex(rui, run_geo.x);
      FgBoEllipseGpPrjnEl* el = group_specs.FastEl(rui % group_specs.size);
      float wt_renorm = (1.0f - el->min_wt);

      taVector2i del = suc - ruc_s;
      if(wrap) {                       // dist may be closer in wrapped case..
        suc.WrapMinDist(del, sgp_geo, ruc_s, sgp_geo_half);
      }

      float wt = el->fgbo_weights.FastEl(del.x + el->con_radius, del.y + el->con_radius,
                                         sun.y-suy_st, sun.x);
      wt = wt * wt_renorm + el->min_wt; // renorm to min wt range
      cg->Cn(i)->wt = wt;
    }
  }
  else {
    int rgpidx;
    int rui;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
    taVector2i ruc = recv_lay->UnitGpPosFmIdx(rgpidx);
    taVector2i ruc_s = ruc * rgp_sc; // project ruc into s coords
    taVector2i run;
    run.SetFmIndex(rui, run_geo.x);
    int depth = run.y / ruy_per_depth;
    int suy_st = depth * 2;

    FgBoEllipseGpPrjnEl* el = group_specs.FastEl(rui % group_specs.size);
    float wt_renorm = (1.0f - el->min_wt);

    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i,net);
      int sgpidx;
      int sui;
      send_lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
      taVector2i suc = send_lay->UnitGpPosFmIdx(sgpidx);
      taVector2i sun;
      sun.SetFmIndex(sui, sun_geo.x);

      taVector2i del = suc - ruc_s;
      if(wrap) {                       // dist may be closer in wrapped case..
        suc.WrapMinDist(del, sgp_geo, ruc_s, sgp_geo_half);
      }

      float wt = el->fgbo_weights.FastEl(del.x + el->con_radius, del.y + el->con_radius,
                                         sun.y-suy_st, sun.x);
      wt = wt * wt_renorm + el->min_wt; // renorm to min wt range
      cg->Cn(i)->wt = wt;
    }
  }
}

