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

#include "TiledGpRFPrjnSpec.h"
#include <Network>
#include <int_Array>
#include <taMath_float>

void TiledGpRFPrjnSpec::Initialize() {
  send_gp_size = 4;
  send_gp_skip = 2;
  wrap = false;
  reciprocal = false;
  gauss_sig = 1.0f;
}

void TiledGpRFPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }

  if(reciprocal) {
    Connect_Reciprocal(prjn);
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->gp_geom;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int sg_sz_tot = send_gp_size.Product();
  int alloc_no = sg_sz_tot * su_nunits;

  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        taVector2i su_st;
        if(wrap)        su_st = (ruc-1) * send_gp_skip;
        else            su_st = ruc * send_gp_skip;

        for(int rui=0; rui < ru_nunits; rui++) {
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(alloc_no, prjn);
        }

        taVector2i suc;
        taVector2i suc_wrp;
        for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
            suc_wrp = suc;
            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
              continue;
            int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
            if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

            Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, sgpidx, alloc_loop);
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void TiledGpRFPrjnSpec::Connect_Reciprocal(Projection* prjn) {
  Layer* recv_lay = prjn->from; // from perspective of non-recip!
  Layer* send_lay = prjn->layer;
  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->gp_geom;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int_Array alloc_sz;
  alloc_sz.SetSize(su_geo.Product()); // alloc sizes per each su unit group
  alloc_sz.InitVals(0);

  taVector2i ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      taVector2i su_st;
      if(wrap)  su_st = (ruc-1) * send_gp_skip;
      else      su_st = ruc * send_gp_skip;

      taVector2i suc;
      taVector2i suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
          suc_wrp = suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
          if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;
          alloc_sz[sgpidx] += ru_nunits;
        }
      }
    }
  }

  // now actually allocate
  for(int sug=0; sug < send_lay->gp_geom.n; sug++) {
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sug);
      su_u->RecvConsPreAlloc(alloc_sz[sug], prjn);
    }
  }

  // then connect
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        taVector2i su_st;
        if(wrap)        su_st = (ruc-1) * send_gp_skip;
        else            su_st = ruc * send_gp_skip;

        taVector2i suc;
        taVector2i suc_wrp;
        for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
            suc_wrp = suc;
            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
              continue;
            int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
            if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

            Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, sgpidx, alloc_loop);
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void TiledGpRFPrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
                                          int rgpidx, int sgpidx, int alloc_loop) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  if(reciprocal) {              // reciprocal is backwards!
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        su_u->ConnectFrom(ru_u, prjn, alloc_loop); // recip!
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      for(int sui=0; sui < su_nunits; sui++) {
        Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        ru_u->ConnectFrom(su_u, prjn, alloc_loop); // recip!
      }
    }
  }
}

int TiledGpRFPrjnSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
  if(!(bool)prjn->from) return 0;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return 0;
  if(TestWarning(!prjn->layer->unit_groups, "ProbAddCons_impl",
                 "requires recv layer to have unit groups!")) {
    return 0;
  }
  if(TestWarning(!prjn->from->unit_groups, "ProbAddCons_impl",
                 "requires send layer to have unit groups!")) {
    return 0;
  }

  int rval = 0;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {
    recv_lay = prjn->from;
    send_lay = prjn->layer;
  }

  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->gp_geom;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  taVector2i ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      taVector2i su_st;
      if(wrap)  su_st = (ruc-1) * send_gp_skip;
      else      su_st = ruc * send_gp_skip;

      taVector2i suc;
      taVector2i suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
          suc_wrp = suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
          if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

          for(int rui=0; rui < ru_nunits; rui++) {
            for(int sui=0; sui < su_nunits; sui++) {
              Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
              Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
              if(!self_con && (su_u == ru_u)) continue;

              // just do a basic probabilistic version: too hard to permute..
              if(Random::ZeroOne() > p_add_con) continue; // no-go

              Connection* con;
              if(!reciprocal)
                con = ru_u->ConnectFromCk(su_u, prjn); // gotta check!
              else
                con = su_u->ConnectFromCk(ru_u, prjn);
              if(con) {
                con->wt = init_wt;
                rval++;
              }
            }
          }
        }
      }
    }
  }
  return rval;
}

bool TiledGpRFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
  trg_send_geom.x = send_x;
  trg_send_geom.y = send_y;

  if(wrap)
    trg_recv_geom = (trg_send_geom / send_gp_skip);
  else
    trg_recv_geom = (trg_send_geom / send_gp_skip) - 1;

  // now fix it the other way
  if(wrap)
    trg_send_geom = (trg_recv_geom * send_gp_skip);
  else
    trg_send_geom = ((trg_recv_geom +1) * send_gp_skip);

  SigEmitUpdated();
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool TiledGpRFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
  trg_recv_geom.x = recv_x;
  trg_recv_geom.y = recv_y;

  if(wrap)
    trg_send_geom = (trg_recv_geom * send_gp_skip);
  else
    trg_send_geom = ((trg_recv_geom+1) * send_gp_skip);

  // now fix it the other way
  if(wrap)
    trg_recv_geom = (trg_send_geom / send_gp_skip);
  else
    trg_recv_geom = (trg_send_geom / send_gp_skip) - 1;

  SigEmitUpdated();
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}

void TiledGpRFPrjnSpec::SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist,
                                    int cg_idx) {
  float gaus = taMath_float::gauss_den_nonorm(dist, gauss_sig);
  float wt_val = wt_range.min + gaus * wt_range.Range();
  cg->Cn(cg_idx)->wt = wt_val;
}

void TiledGpRFPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru);

  // Layer* recv_lay = (Layer*)prjn->layer;
  // Layer* send_lay = (Layer*)prjn->from.ptr();

  // for(int i=0; i<cg->size; i++) {
  //   int su_x = i % rf_width.x;
  //   int su_y = i / rf_width.x;

  //   float dst = taMath_float::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
  //   float wt = expf(-0.5 * dst / sig_sq);

  //   cg->Cn(i)->wt = wt;
  // }

}
