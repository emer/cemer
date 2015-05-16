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

#include "TiledSubGpRFPrjnSpec.h"
#include <Network>
#include <taMath_float>

#include <LeabraConSpec>

TA_BASEFUNS_CTORS_DEFN(TiledSubGpRFPrjnSpec);

void TiledSubGpRFPrjnSpec::Initialize() {
  send_gp_size = 4;
  send_gp_skip = 2;
  send_gp_start = 0;
  send_subgp_size = 2;
  recv_subgp_size = 2;
  wrap = false;
  reciprocal = false;
  wts_type = GAUSSIAN;
  gauss_sig = 1.0f;
  gauss_ctr_mv = 0.5f;
  wt_range.min = 0.1f;
  wt_range.max = 0.9f;
  p_high = 0.25f;
}

void TiledSubGpRFPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  wt_range.UpdateAfterEdit_NoGui();
  
  if(TestError(send_subgp_size.x > recv_subgp_size.x, "UAE",
               "send_subgp_size must be smaller than or equal to recv_subgp_size")) {
  }
  if(TestError(send_subgp_size.y > recv_subgp_size.y, "UAE",
               "send_subgp_size must be smaller than or equal to recv_subgp_size")) {
  }
  if(TestError(recv_subgp_size.x % send_subgp_size.x != 0, "UAE",
               "recv_subgp_size must be an even multiple of send_subgp_size")) {
  }
  if(TestError(recv_subgp_size.y % send_subgp_size.y != 0, "UAE",
               "recv_subgp_size must be an even multiple of send_subgp_size")) {
  }
}

void TiledSubGpRFPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
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

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  if(reciprocal) {
    recv_lay = prjn->from;      // backwards -- only diff!
    send_lay = prjn->layer;
  }

  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->gp_geom;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  if(TestWarning(ru_geo.x % recv_subgp_size.x != 0, "Connect_impl",
               "recv layer group geometry must be an even multiple of recv_subgp_size")) {
    return;
  }
  if(TestWarning(ru_geo.y % recv_subgp_size.y != 0, "Connect_impl",
               "recv layer group geometry must be an even multiple of recv_subgp_size")) {
    return;
  }
  if(TestWarning(su_geo.x % send_subgp_size.x != 0, "Connect_impl",
               "send layer group geometry must be an even multiple of send_subgp_size")) {
    return;
  }
  if(TestWarning(su_geo.y % send_subgp_size.y != 0, "Connect_impl",
               "send layer group geometry must be an even multiple of send_subgp_size")) {
    return;
  }

  // topographic receptive field geometry -- rf suffix means in receptive field coords
  taVector2i ru_geo_rf = ru_geo / recv_subgp_size;
  taVector2i su_geo_rf = su_geo / send_subgp_size;

  int sg_sz_tot = send_gp_size.Product();
  int alloc_no = sg_sz_tot * su_nunits; // todo what happens with != subgp sizes??

  taVector2i ruc_rf;            // first loop over outer receptive field coords
  for(ruc_rf.y = 0; ruc_rf.y < ru_geo_rf.y; ruc_rf.y++) {
    for(ruc_rf.x = 0; ruc_rf.x < ru_geo_rf.x; ruc_rf.x++) {
      taVector2i su_st_rf = send_gp_start + ruc_rf * send_gp_skip;

      taVector2i ruc_sg;        // then sub-group coords within
      for(ruc_sg.y = 0; ruc_sg.y < recv_subgp_size.y; ruc_sg.y++) {
        for(ruc_sg.x = 0; ruc_sg.x < recv_subgp_size.x; ruc_sg.x++) {
          taVector2i ruc = ruc_rf * recv_subgp_size + ruc_sg;
          int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
          
          taVector2i suc_rf;
          taVector2i suc_wrp;
          for(suc_rf.y = su_st_rf.y; suc_rf.y < su_st_rf.y + send_gp_size.y; suc_rf.y++) {
            for(suc_rf.x = su_st_rf.x; suc_rf.x < su_st_rf.x + send_gp_size.x; suc_rf.x++) {
              taVector2i suc = (suc_rf * send_subgp_size);
              if(send_subgp_size == recv_subgp_size)
                suc += ruc_sg;
              // + ((send_gp_size * ruc_sg) / recv_subgp_size);

              suc_wrp = suc;
              if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
                continue;

              int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
              if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

              Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, sgpidx, make_cons);
            }
          }
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    prjn->layer->RecvConsPostAlloc(prjn);
    prjn->from->SendConsPostAlloc(prjn);
  }
}

void TiledSubGpRFPrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                             Layer* send_lay,
                                             int rgpidx, int sgpidx, bool make_cons) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  if(reciprocal) {              // reciprocal is backwards!
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        if(!make_cons) {
          su_u->RecvConsAllocInc(1, prjn); // recip!
          ru_u->SendConsAllocInc(1, prjn); // recip!
        }
        else {
          su_u->ConnectFrom(ru_u, prjn); // recip!
        }
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      for(int sui=0; sui < su_nunits; sui++) {
        Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        if(!make_cons) {
          ru_u->RecvConsAllocInc(1, prjn);
          su_u->SendConsAllocInc(1, prjn);
        }
        else {
          ru_u->ConnectFrom(su_u, prjn);
        }
      }
    }
  }
}

bool TiledSubGpRFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
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

  trg_recv_geom *= recv_subgp_size;
  trg_send_geom *= send_subgp_size;
  
  SigEmitUpdated();
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool TiledSubGpRFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
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

  trg_recv_geom *= recv_subgp_size;
  trg_send_geom *= send_subgp_size;
  
  SigEmitUpdated();
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}

void TiledSubGpRFPrjnSpec::Init_Weights_Prjn(Projection* prjn, ConGroup* cg,
                                          Network* net, int thr_no) {
  if(set_scale) {
    if(TestWarning(!cg->GetConSpec()->InheritsFrom(&TA_LeabraConSpec),
                   "Init_Weights_Prjn", "set_scale can only apply to Leabra connections!"))
      return;
  }
  if(wts_type == GAUSSIAN) {
    Init_Weights_Gaussian(prjn, cg, net, thr_no);
  }
}

void TiledSubGpRFPrjnSpec::Init_Weights_Gaussian(Projection* prjn, ConGroup* cg,
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

    if(set_scale) {
      SetCnWtRnd(cg, i, net, thr_no);
      SetCnScale(wt, cg, i, net, thr_no);
    }
    else {
      SetCnWt(wt, cg, i, net, thr_no);
    }
  }
}

