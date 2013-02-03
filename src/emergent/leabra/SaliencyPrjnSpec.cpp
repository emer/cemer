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

#include "SaliencyPrjnSpec.h"
#include <Network>
#include <taMath_float>

void SaliencyPrjnSpec::Initialize() {
  //  init_wts = true;
  convergence = 1;
  reciprocal = false;
  feat_only = true;
  feat_gps = 2;
  dog_wts.filter_width = 3;
  dog_wts.filter_size = 7;
  dog_wts.on_sigma = 1;
  dog_wts.off_sigma = 2;
  wt_mult = 1.0f;
  surr_mult = 1.0f;
  units_per_feat_gp = 4;
}

void SaliencyPrjnSpec::Connect_impl(Projection* prjn) {
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
  if(feat_only)
    Connect_feat_only(prjn);
  else
    Connect_full_dog(prjn);
}

void SaliencyPrjnSpec::Connect_feat_only(Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  if(reciprocal) {
    recv_lay = prjn->from;
    send_lay = prjn->layer;

    FOREACH_ELEM_IN_GROUP(Unit, su, send_lay->units) {
      su->RecvConsPreAlloc(1, prjn); // only ever have 1!
    }
  }

  taVector2i rug_geo = recv_lay->gp_geom;
  taVector2i ruu_geo = recv_lay->un_geom;
  taVector2i su_geo = send_lay->gp_geom;

  int fltsz = convergence;
  int sg_sz_tot = fltsz * fltsz;

  int feat_no = 0;
  taVector2i rug;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(rug.y = 0; rug.y < rug_geo.y; rug.y++) {
      for(rug.x = 0; rug.x < rug_geo.x; rug.x++, feat_no++) {
        Unit_Group* ru_gp = recv_lay->UnitGpAtCoord(rug);
        if(!ru_gp) continue;

        int rui = 0;
        taVector2i ruc;
        for(ruc.y = 0; ruc.y < ruu_geo.y; ruc.y++) {
          for(ruc.x = 0; ruc.x < ruu_geo.x; ruc.x++, rui++) {

            taVector2i su_st = ruc*convergence;

            Unit* ru_u = (Unit*)ru_gp->SafeEl(rui);
            if(!ru_u) break;
            if(!reciprocal && !alloc_loop)
              ru_u->RecvConsPreAlloc(sg_sz_tot, prjn);

            taVector2i suc;
            for(suc.y = 0; suc.y < fltsz; suc.y++) {
              for(suc.x = 0; suc.x < fltsz; suc.x++) {
                taVector2i sugc = su_st + suc;
                Unit_Group* su_gp = send_lay->UnitGpAtCoord(sugc);
                if(!su_gp) continue;

                Unit* su_u = (Unit*)su_gp->SafeEl(feat_no);
                if(su_u) {
                  if(reciprocal)
                    su_u->ConnectFrom(ru_u, prjn, alloc_loop);
                  else
                    ru_u->ConnectFrom(su_u, prjn, alloc_loop);
                }
              }
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

void SaliencyPrjnSpec::Connect_full_dog(Projection* prjn) {
  dog_wts.UpdateFilter();
  taMath_float::vec_norm_abs_max(&(dog_wts.net_filter)); // renorm to abs max = 1

  if(TestError(reciprocal, "Connect_full_dog", "full DoG connection not supported in reciprocal mode!!!")) return;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i rug_geo = recv_lay->gp_geom;
  taVector2i ruu_geo = recv_lay->un_geom;
  taVector2i su_geo = send_lay->gp_geom;

  int fltwd = dog_wts.filter_width; // no convergence..
  int fltsz = dog_wts.filter_size * convergence;

  int sg_sz_tot = fltsz * fltsz;
  Unit_Group* su_gp0 = (Unit_Group*)send_lay->units.gp[0];
  int alloc_no = sg_sz_tot * su_gp0->size;

  units_per_feat_gp = su_gp0->size / feat_gps;

  int feat_no = 0;
  taVector2i rug;
  for(rug.y = 0; rug.y < rug_geo.y; rug.y++) {
    for(rug.x = 0; rug.x < rug_geo.x; rug.x++, feat_no++) {
      Unit_Group* ru_gp = recv_lay->UnitGpAtCoord(rug);
      if(!ru_gp) continue;

      int rui = 0;
      taVector2i ruc;
      for(ruc.y = 0; ruc.y < ruu_geo.y; ruc.y++) {
        for(ruc.x = 0; ruc.x < ruu_geo.x; ruc.x++, rui++) {

          taVector2i su_st = ruc*convergence - convergence*fltwd;

          Unit* ru_u = (Unit*)ru_gp->SafeEl(rui);
          if(!ru_u) break;
          ru_u->RecvConsPreAlloc(alloc_no, prjn);

          taVector2i suc;
          for(suc.y = 0; suc.y < fltsz; suc.y++) {
            for(suc.x = 0; suc.x < fltsz; suc.x++) {
              taVector2i sugc = su_st + suc;
              Unit_Group* su_gp = send_lay->UnitGpAtCoord(sugc);
              if(!su_gp) continue;

              for(int sui=0;sui<su_gp->size;sui++) {
                Unit* su_u = (Unit*)su_gp->FastEl(sui);
                if(!self_con && (su_u == ru_u)) continue;
                ru_u->ConnectFrom(su_u, prjn);
              }
            }
          }
        }
      }
    }
  }
}

void SaliencyPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  if(feat_only) {               // just use regular..
    inherited::C_Init_Weights(prjn, cg, ru);
    return;
  }
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int fltwd = dog_wts.filter_width; // no convergence.
  int fltsz = dog_wts.filter_size * convergence;

  Unit_Group* su_gp0 = (Unit_Group*)send_lay->units.gp[0];
  units_per_feat_gp = su_gp0->size / feat_gps;

  Unit_Group* rugp = (Unit_Group*)ru->GetOwner();

  taVector2i rug_geo = recv_lay->gp_geom;
  taVector2i rgp_pos = rugp->GpLogPos();

  int feat_no = rgp_pos.y * rug_geo.x + rgp_pos.x; // unit group index
  int my_feat_gp = feat_no / units_per_feat_gp;
  int fg_st = my_feat_gp * units_per_feat_gp;
  int fg_ed = fg_st + units_per_feat_gp;

  taVector2i ruu_geo = recv_lay->un_geom;
  taVector2i su_st;              // su starting (left)
  su_st.x = (ru->idx % ruu_geo.x)*convergence - convergence*fltwd;
  su_st.y = (ru->idx / ruu_geo.x)*convergence - convergence*fltwd;

  taVector2i su_geo = send_lay->gp_geom;

  int su_idx = 0;
  taVector2i suc;
  for(suc.y = 0; suc.y < fltsz; suc.y++) {
    for(suc.x = 0; suc.x < fltsz; suc.x++) {
      taVector2i sugc = su_st + suc;
      Unit_Group* su_gp = send_lay->UnitGpAtCoord(sugc);
      if(!su_gp) continue;

      float wt = wt_mult * dog_wts.net_filter.FastEl(suc.x/convergence,
                                                     suc.y/convergence);

      if(wt > 0) {
        for(int sui=0;sui<su_gp->size;sui++) {
          if(sui == feat_no)
            cg->Cn(su_idx++)->wt = wt; // target feature
          else
            cg->Cn(su_idx++)->wt = 0.0f; // everyone else
        }
      }
      else {
        for(int sui=0;sui<su_gp->size;sui++) {
          if(sui != feat_no && sui >= fg_st && sui < fg_ed)
            cg->Cn(su_idx++)->wt = -surr_mult * wt;
          else
            cg->Cn(su_idx++)->wt = 0.0f; // not in our group or is guy itself
        }
      }
    }
  }
}

void SaliencyPrjnSpec::GraphFilter(DataTable* graph_data) {
  dog_wts.GraphFilter(graph_data);
}

void SaliencyPrjnSpec::GridFilter(DataTable* graph_data) {
  dog_wts.GridFilter(graph_data);
}

