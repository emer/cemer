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

#include "VisDisparityPrjnSpec.h"
#include <Network>
#include <taMath_float>

void VisDisparityPrjnSpec::Initialize() {
  n_disps = 1;
  disp_range_pct = 0.05f;
  gauss_sig = 0.7f;
  disp_spacing = 2.0f;
  end_extra = 2;
  wrap = true;

  init_wts = true;

  tot_disps = 1 + 2 * n_disps;
  UpdateFmV1sSize(24);
}

void VisDisparityPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_disps = 1 + 2 * n_disps;
}


void VisDisparityPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires sending layer to have unit groups!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i rgp_geo = recv_lay->gp_geom;
  taVector2i sgp_geo = send_lay->gp_geom;

  if(TestWarning(rgp_geo != sgp_geo, "Connect_Gps",
                 "Recv layer does not have same gp geometry as sending layer -- cannot connect!")) {
    return;
  }
  taVector2i run_geo = recv_lay->un_geom;
  taVector2i sun_geo = send_lay->un_geom;

  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  if(TestWarning(ru_n != su_n * tot_disps, "Connect_Gps",
                 "Recv layer unit groups must have n_disps * 2 + 1=",
                 String(tot_disps),"times number of units in send layer unit group=",
                 String(su_n),"  should be:", String(su_n * tot_disps), "is:",
                 String(ru_n))) {
    return;
  }

  if(prjn->recv_idx == 0)       // right eye
    Connect_RightEye(prjn);
  else
    Connect_LeftEye(prjn);
}

void VisDisparityPrjnSpec::Connect_RightEye(Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i gp_geo = recv_lay->gp_geom;

  taVector2i run_geo = recv_lay->un_geom;
  taVector2i sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  // note: could optimize this code b/c sender alloc = tot_disps so could be pre-alloc entirely
  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < gp_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < gp_geo.x; ruc.x++, rgpidx++) {
        for(int rui=0; rui<ru_n; rui++) {
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          if(!ru_u) continue;
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(1, prjn); // just one prjn!

          int sui = rui % su_n; // just modulus -- recv from same features tot_disps times
          Unit* su_u = send_lay->UnitAtUnGpIdx(sui, rgpidx); // rgp = sgp
          if(!su_u) continue;
          ru_u->ConnectFrom(su_u, prjn, alloc_loop);
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void VisDisparityPrjnSpec::InitStencils(Projection* prjn) {
  Layer* send_lay = prjn->from;
  taVector2i gp_geo = send_lay->gp_geom;

  UpdateFmV1sSize(gp_geo.x);

  v1b_widths.SetGeom(1, tot_disps);
  v1b_weights.SetGeom(2, max_width, tot_disps);
  v1b_stencils.SetGeom(2, max_width, tot_disps);

  v1b_weights.InitVals(0.0f);   // could have some lurkers in there from other settings, which can affect normalization

  int twe = disp_range + end_ext;

  // everything is conditional on the disparity
  for(int disp=-n_disps; disp <= n_disps; disp++) {
    int didx = disp + n_disps;
    int doff = disp * disp_spc;
    if(disp == 0) {             // focal
      v1b_widths.FastEl(didx) = 1 + 2 * disp_range;
      for(int tw=-disp_range; tw<=disp_range; tw++) {
        int twidx = tw + disp_range;
        float fx = (float)tw / (float)disp_range;
        v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, gauss_sig);
        v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else if(disp == -n_disps) {
      v1b_widths.FastEl(didx) = 1 + 2 * disp_range + end_ext;
      for(int tw=-twe; tw<=disp_range; tw++) {
        int twidx = tw + twe;
        if(tw < 0)
          v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(0.0f, gauss_sig);
        else {
          float fx = (float)tw / (float)disp_range;
          v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, gauss_sig);
        }
        v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else if(disp == n_disps) {
      v1b_widths.FastEl(didx) = 1 + 2 * disp_range + end_ext;
      for(int tw=-disp_range; tw<=twe; tw++) {
        int twidx = tw + disp_range;
        if(tw > 0)
          v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(0.0f, gauss_sig);
        else {
          float fx = (float)tw / (float)disp_range;
          v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, gauss_sig);
        }
        v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else {
      v1b_widths.FastEl(didx) = 1 + 2 * disp_range;
      for(int tw=-disp_range; tw<=disp_range; tw++) {
        int twidx = tw + disp_range;
        float fx = (float)tw / (float)disp_range;
        v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, gauss_sig);
        v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
  }

  taMath_float::vec_norm_max(&v1b_weights); // max norm to 1
}

void VisDisparityPrjnSpec::Connect_LeftEye(Projection* prjn) {
  InitStencils(prjn);

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i gp_geo = recv_lay->gp_geom; // same as sgp

  taVector2i run_geo = recv_lay->un_geom;
  taVector2i sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < gp_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < gp_geo.x; ruc.x++, rgpidx++) {
        for(int didx=0; didx < tot_disps; didx++) {
          int dwd = v1b_widths.FastEl(didx);

          int strui = didx * su_n; // starting index
          for(int sui=0; sui<su_n; sui++) {
            int rui = strui + sui;
            Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
            if(!ru_u) continue;
            if(!alloc_loop)
              ru_u->RecvConsPreAlloc(dwd, prjn);

            for(int twidx = 0; twidx < dwd; twidx++) {
              int off = v1b_stencils.FastEl(twidx, didx);
              // float wt = v1b_weights.FastEl(twidx, didx);

              taVector2i suc = ruc;
              suc.x += off;     // offset
              taVector2i suc_wrp = suc;
              if(suc_wrp.WrapClip(wrap, gp_geo) && !wrap)
                continue;
              int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
              if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

              Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
              if(!su_u) continue;
              ru_u->ConnectFrom(su_u, prjn, alloc_loop);
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void VisDisparityPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Network* net = prjn->layer->own_net;
  if(cg->size == 1) {           // right eye
    cg->Cn(0,BaseCons::WT,net) = 1.0f;
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i gp_geo = recv_lay->gp_geom; // same as sgp

  taVector2i run_geo = recv_lay->un_geom;
  taVector2i sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  int rgpidx;
  int rui;
  recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
  int didx = rui / su_n;        // disparity index

  int dwd = v1b_widths.SafeEl(didx);
  if(cg->size == dwd) {
    for(int i=0; i<cg->size; i++) {
      float wt = v1b_weights.SafeEl(i, didx);
      cg->Cn(i,BaseCons::WT,net) = wt;
    }
  }
  else {
    taVector2i ruc = recv_lay->UnitGpPosFmIdx(rgpidx);
    if(ruc.x < gp_geo.x / 2) {
      int st = dwd - cg->size;
      for(int i=0; i<cg->size; i++) {
        float wt = v1b_weights.SafeEl(st + i, didx);
        cg->Cn(i,BaseCons::WT,net) = wt;
      }
    }
    else {
      // actually just straight up
      for(int i=0; i<cg->size; i++) {
        float wt = v1b_weights.SafeEl(i, didx);
        cg->Cn(i,BaseCons::WT,net) = wt;
      }
    }
  }
}


