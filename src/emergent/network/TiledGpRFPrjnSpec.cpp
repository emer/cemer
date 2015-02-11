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
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TiledGpRFPrjnSpec);

void TiledGpRFPrjnSpec::Initialize() {
  send_gp_size = 4;
  send_gp_skip = 2;
  send_gp_start = 0;
  wrap = false;
  reciprocal = false;
  wts_type = GAUSSIAN;
  gauss_sig = 1.0f;
  gauss_ctr_mv = 0.5f;
  wt_range.min = 0.1f;
  wt_range.max = 0.9f;
  p_high = 0.25f;
}

void TiledGpRFPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(taMisc::is_loading) {
    taVersion v705(7, 0, 5);
    if(taMisc::loading_version < v705) { // set send_gp_start to prev val
      if(wrap) {
        send_gp_start = -send_gp_skip;
      }
      else {
        send_gp_start = 0;
      }
    }
  }
}

void TiledGpRFPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
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

  if(wts_type == BIMODAL_PERMUTED) {
    high_low_wts.Reset();       // keep it fresh
  }
  
  if(reciprocal) {
    Connect_Reciprocal(prjn, make_cons);
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
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      taVector2i su_st;
      su_st = send_gp_start + ruc * send_gp_skip;

      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        if(!make_cons)
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

          Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, sgpidx, !make_cons);
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    prjn->from->SendConsPostAlloc(prjn);
  }
}

void TiledGpRFPrjnSpec::Connect_Reciprocal(Projection* prjn, bool make_cons) {
  Layer* recv_lay = prjn->from; // from perspective of non-recip!
  Layer* send_lay = prjn->layer;
  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->gp_geom;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  taVector2i ruc;
  if(!make_cons) {
    int_Array alloc_sz;
    alloc_sz.SetSize(su_geo.Product()); // alloc sizes per each su unit group
    alloc_sz.InitVals(0);

    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        taVector2i su_st;
        su_st = send_gp_start + ruc * send_gp_skip;

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
  }

  // then connect
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      taVector2i su_st;
      su_st = send_gp_start + ruc * send_gp_skip;

      taVector2i suc;
      taVector2i suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
          suc_wrp = suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
          if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

          Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, sgpidx, !make_cons);
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    prjn->from->SendConsPostAlloc(prjn);
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
      su_st = send_gp_start + ruc * send_gp_skip;

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

              int con = -1;
              if(!reciprocal)   // gotta check
                con = ru_u->ConnectFromCk(su_u, prjn, false, true, init_wt);
              else
                con = su_u->ConnectFromCk(ru_u, prjn, false, true, init_wt);
              if(con >= 0) {
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

void TiledGpRFPrjnSpec::Init_Weights_Prjn(Projection* prjn, ConGroup* cg,
                                          Network* net, int thr_no) {
  if(wts_type == GAUSSIAN) {
    Init_Weights_Gaussian(prjn, cg, net, thr_no);
  }
  else {
    Init_Weights_BimodalPermuted(prjn, cg, net, thr_no);
  }    
}

void TiledGpRFPrjnSpec::Init_Weights_Gaussian(Projection* prjn, ConGroup* cg,
                                              Network* net, int thr_no) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  // todo: need to fix for reciprocal?

  taVector2i full_size = send_lay->un_geom * send_gp_size;
  taVector2f half_size = full_size;
  half_size *= 0.5f;

  float eff_sig = gauss_sig * (float)half_size.x;
  
  taVector2i ru_pos;
  Unit* ru = cg->ThrOwnUn(net, thr_no);
  ru->UnitGpLogPos(ru_pos);

  taVector2f rugpctr = recv_lay->un_geom;
  rugpctr *= 0.5f;
  taVector2f ru_nrm_pos = gauss_ctr_mv * (((taVector2f)ru_pos - rugpctr) / rugpctr);

  taVector2f s_ctr = (ru_nrm_pos * half_size) + half_size;
  
  for(int i=0; i<cg->size; i++) {
    // note: these are organized within unit group first, then by groups
    int ug_idx = i / send_lay->un_geom.n; // which unit group, ordinally
    int un_idx = i % send_lay->un_geom.n; // index in unit group

    int un_x = un_idx % send_lay->un_geom.x;
    int un_y = un_idx / send_lay->un_geom.x;

    int ug_x = ug_idx % send_gp_size.x;
    int ug_y = ug_idx / send_gp_size.x;

    int su_x = ug_x * send_lay->un_geom.x + un_x;
    int su_y = ug_y * send_lay->un_geom.y + un_y;

    float dst = taMath_float::euc_dist(su_x, su_y, s_ctr.x, s_ctr.y);
    float wt = taMath_float::gauss_den_nonorm(dst, eff_sig);
    wt = wt_range.min + wt_range.range * wt;
    
    SetCnWt(cg, i, net, wt);
  }
}

void TiledGpRFPrjnSpec::Init_Weights_BimodalPermuted(Projection* prjn, ConGroup* cg,
                                                     Network* net, int thr_no) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  // todo: need to fix for reciprocal?

  taVector2i full_size = send_lay->un_geom * send_gp_size;
  int n_sz = full_size.Product();
  
  if(high_low_wts.size != n_sz) {
    high_low_wts.SetSize(n_sz);
    int n_on = (int)((float)n_sz * p_high);
    if(n_on == 0) n_on = 1;
    if(n_on > n_sz) n_on = n_sz;
    int i;
    for(i=0; i<n_on; i++) {
      high_low_wts[i] = 1;
    }
    for(;i<n_sz;i++) {
      high_low_wts[i] = 0;
    }
  }

  high_low_wts.Permute();
  
  for(int i=0; i<cg->size; i++) {
    int hilo = high_low_wts[i];
    float wt;
    if(hilo == 1) {
      wt = wt_range.max;
    }
    else {
      wt = wt_range.min;
    }
    SetCnWt(cg, i, net, wt);
  }
}

