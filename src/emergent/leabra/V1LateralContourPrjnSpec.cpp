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

#include "V1LateralContourPrjnSpec.h"
#include <Network>
#include <taMath_float>

void V1LateralContourPrjnSpec::Initialize() {
  radius = 4;
  wrap = true;
  ang_pow = 4.0f;
  dist_sigma = 1.0f;
  con_thr = 0.2f;
  oth_feat_wt = 0.5f;
  init_wts = true;

}

void V1LateralContourPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void V1LateralContourPrjnSpec::Connect_impl(Projection* prjn) {
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

            float nrmdst = dst / (float)radius;
            float gang = atan2f(del.y, del.x); // group angle
            if(gang >= taMath_float::pi) gang -= taMath_float::pi;
            if(gang < 0.0f) gang += taMath_float::pi;

            float gauswt = taMath_float::gauss_den_nonorm(nrmdst, dist_sigma);

            taVector2i run;
            for(run.x = 0; run.x < un_geo.x; run.x++) {
              float rang = taMath_float::pi * ((float)run.x / n_angles);
              float gangwt = powf(fabsf(cosf(gang-rang)), ang_pow);

              taVector2i sun;
              for(sun.x = 0; sun.x < un_geo.x; sun.x++) {
                float sang = taMath_float::pi * ((float)sun.x / n_angles);
                float sangwt = powf(fabsf(cosf(sang-rang)), ang_pow);
                float wt = sangwt * gangwt * gauswt;
                if(wt < con_thr) continue;

                for(run.y = 0; run.y < un_geo.y; run.y++) {
                  for(sun.y = 0; sun.y < un_geo.y; sun.y++) {
                    int rui = run.y * un_geo.x + run.x;
                    int sui = sun.y * un_geo.x + sun.x;

                    float feat_wt = 1.0f;
                    if(run.y != sun.y)
                      feat_wt = oth_feat_wt;
                    float eff_wt = wt * feat_wt;
                    if(eff_wt < con_thr) continue;

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
                      cn->wt = eff_wt;
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

void V1LateralContourPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* lay = prjn->from;
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
    Unit* su = cg->Un(i);
    int sgpidx;
    int sui;
    lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
    taVector2i suc = lay->UnitGpPosFmIdx(sgpidx);
    taVector2i sun;
    sun.SetFmIndex(sui, un_geo.x);

    taVector2i del = suc - ruc; // don't use wrap!
    if(wrap) {                 // dist may be closer in wrapped case..
      suc.WrapMinDist(del, gp_geo, ruc, gp_geo_half);
    }

    float dst = del.Mag();
    float nrmdst = dst / (float)radius;
    float gang = atan2f((float)del.y, (float)del.x); // group angle
    if(gang >= taMath_float::pi) gang -= taMath_float::pi;
    if(gang < 0.0f) gang += taMath_float::pi;
    float rang = taMath_float::pi * ((float)run.x / n_angles);
    float sang = taMath_float::pi * ((float)sun.x / n_angles);
    float sangwt = powf(fabsf(cosf(sang-rang)), ang_pow);
    float gangwt = powf(fabsf(cosf(gang-rang)), ang_pow);
    float gauswt = taMath_float::gauss_den_nonorm(nrmdst, dist_sigma);
    float feat_wt = 1.0f;
    if(run.y != sun.y)
      feat_wt = oth_feat_wt;
    float wt = feat_wt * sangwt * gangwt * gauswt;
    cg->Cn(i)->wt = wt;
  }
}

