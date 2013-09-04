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

#include "V2BoLateralPrjnSpec.h"
#include <Network>
#include <taMath_float>


void V2BoLateralPrjnSpec::Initialize() {
  init_wts = true;

  radius = 8;
  wrap = true;
  t_on = true;
  opp_on = true;
  ang_sig = 0.5f;
  dist_sig_line = 0.8f;
  dist_sig_oth = 0.5f;
  line_mag = 0.8f;
  weak_mag = 0.5f;
  con_thr = 0.2f;
  test_ang = -1;
}

void V2BoLateralPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

// static int dbg_hit_cnt = 0;

void V2BoLateralPrjnSpec::CreateStencils() {
  int n_angles = 4;
  int max_cnt = (2 * radius + 1);
  v2ffbo_weights.SetGeom(6, max_cnt, max_cnt, 2, n_angles, 2, n_angles);
  taVector2i suc;                        // send coords
  for(int rang_dx = 0; rang_dx < n_angles; rang_dx++) {
    for(int rdir = 0; rdir < 2; rdir++) {
      for(int sang_dx = 0; sang_dx < n_angles; sang_dx++) {
        for(suc.y = -radius; suc.y <= radius; suc.y++) {
          int ysuc_dx = suc.y + radius;
          for(suc.x = -radius; suc.x <= radius; suc.x++) {
            int xsuc_dx = suc.x + radius;
            for(int sdir = 0; sdir < 2; sdir++) { // integrate over sending directions
              float wt = ConWt(suc, rang_dx, sang_dx, rdir, sdir);
              v2ffbo_weights.FastEl(xsuc_dx, ysuc_dx, sdir, sang_dx, rdir, rang_dx) = wt;
            }
          }
        }
      }
    }
  }
}

float V2BoLateralPrjnSpec::ConWt(taVector2i& suc, int rang_dx, int sang_dx, int rdir, int sdir) {
  float n_angles = 4.0f;
  // integer angles -- useful for precise conditionals..
  int rang_n = rang_dx + rdir * 4;
  int sang_n = sang_dx + sdir * 4;
  int dang_n;
  if(sang_n < rang_n)
    dang_n = (8 + sang_n) - rang_n;
  else
    dang_n = sang_n - rang_n;
  int dang_n_pi = dang_n;
  if(dang_n >= 4) dang_n_pi = 8 - dang_n;
  int abs_dang_n_pi = dang_n_pi < 0 ? -dang_n_pi : dang_n_pi;

  if(!opp_on && dang_n == 4) return 0.0f;       // no opposite angle cons

  if((test_ang >= 0) && (dang_n != test_ang))
    return 0.0f; // don't continue

  taVector2i del = suc;
  float dst = del.Mag();
  if(dst > (float)radius) return 0.0f;
  if(dst == 0.0f) return 0.0f;  // no self con
  float nrmdst = dst / (float)radius;

  float gang = atan2f(del.y, del.x); // group angle -- 0..pi or -pi
  if(gang < 0.0f) gang += 2.0f * taMath_float::pi; // keep it positive

  // dir 0 = 0..pi, dir 1 = pi..2pi
  float rang = taMath_float::pi * ((float)rang_dx / n_angles) + taMath_float::pi * (float)rdir;
  float sang = taMath_float::pi * ((float)sang_dx / n_angles) + taMath_float::pi * (float)sdir;

  float dang;                   // delta-angle -- keep this positive too
  if(sang < rang)
    dang = (2.0f * taMath_float::pi + sang) - rang;
  else
    dang = sang - rang;
  float dang_pi = dang; // this determines type of projection -- equal fabs(dang_pi) are same type
  if(dang >= taMath_float::pi) dang_pi = (2.0f * taMath_float::pi) - dang;
  float abs_dang_pi = fabs(dang_pi);

  float op_mag = 0.0f;
  if(abs_dang_pi < 0.499f * taMath_float::pi)
    op_mag = ((0.5f * taMath_float::pi - abs_dang_pi) / (0.5f * taMath_float::pi)); // 1 for 0, .5 for 45

  float tang = rang + 0.5f * dang; // target angle

  float gtang = gang - tang;
  if(gtang > taMath_float::pi) gtang -= 2.0f * taMath_float::pi;
  if(gtang < -taMath_float::pi) gtang += 2.0f * taMath_float::pi;

  // make symmetric around half sphere
  bool op_side = false;
  if(gtang > taMath_float::pi * 0.5f) { gtang -= taMath_float::pi; op_side = true; }
  if(gtang < -taMath_float::pi * 0.5f){ gtang += taMath_float::pi; op_side = true; }

  float eff_mag = 1.0f;
  if(abs_dang_pi > 0.501f * taMath_float::pi) eff_mag = weak_mag;

  float netwt = eff_mag * taMath_float::gauss_den_nonorm(gtang, ang_sig);
  if(abs_dang_n_pi == 0)
    netwt *= line_mag * taMath_float::gauss_den_nonorm(nrmdst, dist_sig_line);
  else
    netwt *= taMath_float::gauss_den_nonorm(nrmdst, dist_sig_oth);

  if(op_side)
    netwt *= op_mag;

  if(t_on && abs_dang_n_pi == 2 && dst <= 2.9f) {
    float grang = gang - rang;
//     if(dbg_hit_cnt++ <= 100) {
//      taMisc::Info("rang_n: " + String(rang_n), "  sang_n: " + String(sang_n),
//                "   dang_n: " + String(dang_n), "  dang_n_pi: " + String(dang_n_pi),
//                "gang: " + String(gang), "  grang:" + String(grang));
//     }
    if(fabsf(grang - (1.5f * taMath_float::pi)) < .1f ||
       fabsf(grang - (-0.5f * taMath_float::pi)) < .1f) {
      netwt = 1.0f;
    }
  }

  return netwt;
}

void V2BoLateralPrjnSpec::Connect_impl(Projection* prjn) {
//   dbg_hit_cnt = 0;           // debugging

  if(!(bool)prjn->from) return;

  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }
  if(TestWarning(prjn->from != prjn->layer, "Connect_impl",
                 "requires send and recv to be the same layer -- lateral projection!")) {
    return;
  }
  if(TestWarning(prjn->con_spec->wt_limits.sym, "Connect_impl",
                 "cannot have wt_limits.sym on in conspec -- turning off in spec:",
                 prjn->con_spec->name,
                 "This might affect children of this spec.")) {
    prjn->con_spec->SetUnique("wt_limits", true);
    prjn->con_spec->wt_limits.sym = false;
    prjn->con_spec->UpdateAfterEdit();
  }

  CreateStencils();

  Layer* lay = prjn->from;
  taVector2i gp_geo = lay->gp_geom;
  taVector2i un_geo = lay->un_geom;
  float n_angles = (float)un_geo.x;

  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < gp_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < gp_geo.x; ruc.x++, rgpidx++) {
        taVector2i suc;
        taVector2i suc_wrp;
        for(suc.y = ruc.y-radius; suc.y <= ruc.y+radius; suc.y++) {
          for(suc.x = ruc.x-radius; suc.x <= ruc.x+radius; suc.x++) {
            suc_wrp = suc;
            if(suc_wrp.WrapClip(wrap, gp_geo) && !wrap)
              continue;
            int sgpidx = lay->UnitGpIdxFmPos(suc_wrp);
            if(!lay->UnitGpIdxIsValid(sgpidx)) continue;

            taVector2i del = suc - ruc; // don't use wrap!
            float dst = del.Mag();
            if(dst > (float)radius) continue; // out of bounds
            if(dst == 0.0f) continue;         // no selfs

            taVector2i run;
            taVector2i sun;

            for(run.x = 0; run.x < un_geo.x; run.x++) {
              for(sun.x = 0; sun.x < un_geo.x; sun.x++) {
                for(run.y = 0; run.y < un_geo.y; run.y++) {
                  int syst = (run.y / 2) * 2;
                  int syed = syst+2;
                  for(sun.y = syst; sun.y < syed; sun.y++) {
                    // mod 2 on y allows for multiple depths to be replicated
                    float wt = v2ffbo_weights.FastEl(del.x +radius, del.y+radius,
                                                     sun.y % 2, sun.x, run.y % 2, run.x);
                    if(wt <= con_thr) continue;

                    int rui = run.y * un_geo.x + run.x;
                    int sui = sun.y * un_geo.x + sun.x;

                    Unit* ru_u = lay->UnitAtUnGpIdx(rui, rgpidx);
                    if(!ru_u) continue;
                    Unit* su_u = lay->UnitAtUnGpIdx(sui, sgpidx);
                    if(!su_u) continue;
                    if(alloc_loop) {
                      ru_u->RecvConsAllocInc(1, prjn);
                      su_u->SendConsAllocInc(1, prjn);
                    }
                    else {
                      Connection* cn = ru_u->ConnectFrom(su_u, prjn, alloc_loop);
                      cn->wt = wt;
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

void V2BoLateralPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* lay = prjn->from;
  Network* net = prjn->layer->own_net;
  taVector2i gp_geo = lay->gp_geom;
  taVector2i un_geo = lay->un_geom;
  float n_angles = (float)un_geo.x;

  taVector2i gp_geo_half = gp_geo / 2;

  int rgpidx;
  int rui;
  lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
  taVector2i ruc = lay->UnitGpPosFmIdx(rgpidx);
  taVector2i run;
  run.SetFmIndex(rui, un_geo.x);
  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i,net);
    int sgpidx;
    int sui;
    lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
    taVector2i suc = lay->UnitGpPosFmIdx(sgpidx);
    taVector2i sun;
    sun.SetFmIndex(sui, un_geo.x);

    taVector2i del = suc - ruc;
    if(wrap) {                 // dist may be closer in wrapped case..
      suc.WrapMinDist(del, gp_geo, ruc, gp_geo_half);
    }

    float wt = v2ffbo_weights.FastEl(del.x + radius, del.y + radius,
                                     sun.y % 2, sun.x, run.y % 2, run.x);
    cg->Cn(i)->wt = wt;
  }
}

