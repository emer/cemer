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

#include <LeabraConSpec>

TA_BASEFUNS_CTORS_DEFN(GaussInitWtsSpec);
TA_BASEFUNS_CTORS_DEFN(SigmoidInitWtsSpec);
TA_BASEFUNS_CTORS_DEFN(TiledGpRFPrjnSpec);

void GaussInitWtsSpec::Initialize() {
  on = true;
  sigma = 0.6f;
  wrap_wts = false;
  ctr_mv = 0.8f;
}

void SigmoidInitWtsSpec::Initialize() {
  on = true;
  gain = 0.02f;
  ctr_mv = 0.5f;
}

void TiledGpRFPrjnSpec::Initialize() {
  send_gp_size = 4;
  send_gp_skip = 2;
  send_gp_start = -1;
  wrap = true;
  wts_type = GAUSSIAN;
  share_cons = false;
  reciprocal = false;
  full_send = BY_UNIT;
  full_recv = BY_UNIT;
  wt_range.min = 0.4f;
  wt_range.max = 0.6f;
}

void TiledGpRFPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  wt_range.UpdateAfterEdit_NoGui();
  if(TestWarning(full_send == ALL_SAME, "UAE",
                 "full_send == ALL_SAME is not a valid option, switching back to BY_UNIT")) {
    full_send = BY_UNIT;
  }
  
  if(set_scale) {
    Network* net = GET_MY_OWNER(Network);
    if(TestWarning(net && !net->InheritsFromName("LeabraNetwork"),
                   "Init_Weights_Prjn", "set_scale can only be used with Leabra networks -- turning off")) {
      set_scale = false;
    }
  }

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

  Network* net = GET_MY_OWNER(Network);
  if(TestWarning(set_scale && net && !net->InheritsFromName("LeabraNetwork"),
                 "Init_Weights_Prjn", "set_scale can only be used with Leabra networks -- turning off")) {
    set_scale = false;
  }
  
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {
    recv_lay = prjn->from; // from perspective of non-recip!
    send_lay = prjn->layer;
  }

  taVector2i ru_geo = recv_lay->gp_geom;
  if(!recv_lay->unit_groups) {
    ru_geo = 1;
  }
  taVector2i su_geo = send_lay->gp_geom;
  if(!send_lay->unit_groups) {
    su_geo = 1;
  }
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int sg_sz_tot = send_gp_size.Product();
  int alloc_no = sg_sz_tot * su_nunits;

  taVector2i ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      if(!recv_lay->unit_groups) {
        rgpidx = -1;
      }
      taVector2i su_st;
      su_st = send_gp_start + ruc * send_gp_skip;
      taVector2i suc;
      taVector2i suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
          suc_wrp = suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          int sgpidx = -1;
          if(send_lay->unit_groups) {
            sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
            if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;
          }

          Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, sgpidx, make_cons);
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    prjn->layer->RecvConsPostAlloc(prjn);
    prjn->from->SendConsPostAlloc(prjn);
  }
}

void TiledGpRFPrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                             Layer* send_lay,
                                             int rgpidx, int sgpidx, bool make_cons) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  Network* net = recv_lay->own_net;

  if(reciprocal) {              // reciprocal is backwards!
    // todo: share_cons here requires a separate loop for reciprocal
    
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su_u;
      if(sgpidx >= 0)
        su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      else
        su_u = send_lay->units.SafeEl(sui);
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u;
        if(rgpidx >= 0)
          ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        else
          ru_u = recv_lay->units.SafeEl(rui);
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
      Unit* ru_u;
      if(rgpidx >= 0) {
        ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        if(share_cons && net->RecvOwnsCons() && rgpidx > 0) {
          Unit* shru = recv_lay->UnitAtUnGpIdx(rui, 0); // group 0
          ru_u->ShareRecvConsFrom(shru, prjn);
        }
      }
      else
        ru_u = recv_lay->units.SafeEl(rui);
      for(int sui=0; sui < su_nunits; sui++) {
        Unit* su_u;
        if(sgpidx >= 0)
          su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
        else
          su_u = send_lay->units.SafeEl(sui);
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
    Init_Weights_Sigmoid(prjn, cg, net, thr_no);
  }
}

void TiledGpRFPrjnSpec::Init_Weights_Gaussian(Projection* prjn, ConGroup* cg,
                                              Network* net, int thr_no) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  // if(reciprocal) {
  //   recv_lay = prjn->from; // from perspective of non-recip!
  //   send_lay = prjn->layer;
  // }

  bool no_send_gp = (send_gp_size.Product() == 1);
  
  taVector2f un_full_size = send_lay->un_geom * send_gp_size -1.0f;
  taVector2f un_half_size = un_full_size;
  un_half_size *= 0.5f;

  float full_eff_sig = full_gauss.sigma * un_half_size.x;

  taVector2f gp_full_size = send_lay->un_geom -1.0f;
  taVector2f gp_half_size = gp_full_size;
  gp_half_size *= 0.5f;
  float gp_eff_sig = gp_gauss.sigma * gp_half_size.x;
  
  taVector2i ru_un_pos;
  Unit* ru = cg->ThrOwnUn(net, thr_no);
  ru->UnitGpLogPos(ru_un_pos);

  taVector2f ru_ctr = recv_lay->un_geom -1;
  ru_ctr *= 0.5f;
  taVector2f ru_nrm_pos = (taVector2f(ru_un_pos) - ru_ctr) / ru_ctr;
  taVector2f ru_nrm_pos_full = ru_nrm_pos * full_gauss.ctr_mv;
  taVector2f ru_nrm_pos_gp = ru_nrm_pos * gp_gauss.ctr_mv;
  taVector2f s_un_ctr = (ru_nrm_pos_full * un_half_size) + un_half_size;
  taVector2f s_gp_ctr = (ru_nrm_pos_gp * gp_half_size) + gp_half_size;

  if(full_recv == BY_GROUP && recv_lay->unit_groups) {
    taVector2i ru_gp_geo = recv_lay->gp_geom-1;
    taVector2i ru_gp_pos = recv_lay->UnitGpPosFmIdx(ru->UnitGpIdx());
    taVector2f ru_gp_ctr = ru_gp_geo;
    ru_gp_ctr *= 0.5f;
    taVector2f ru_gp_nrm_pos = ((taVector2f)ru_gp_pos - ru_gp_ctr) / ru_gp_ctr;
    ru_nrm_pos_full = ru_gp_nrm_pos * full_gauss.ctr_mv;
    s_un_ctr = (ru_nrm_pos_full * un_half_size) + un_half_size;
  }
  if(full_recv == ALL_SAME) {
    s_un_ctr = un_half_size;
  }
  
  taVector2i unc, ugc;
  taVector2f suc;
  taVector2f sucw;
  taVector2f suncw;
  
  for(int i=0; i<cg->size; i++) {
    // note: these are organized within unit group first, then by groups
    int ug_idx = i / send_lay->un_geom.n; // which unit group, ordinally
    int un_idx = i % send_lay->un_geom.n; // index in unit group

    unc.x = un_idx % send_lay->un_geom.x;
    unc.y = un_idx / send_lay->un_geom.x;

    ugc.x = ug_idx % send_gp_size.x;
    ugc.y = ug_idx / send_gp_size.x;

    float fwt = 1.0f;
    if(full_gauss.on) {
      suc = ugc * send_lay->un_geom;
      if(full_send == BY_UNIT) {
        suc += unc;
      }
      else {                    // by group
        suc += gp_half_size;
      }
      sucw = suc;
      // wrap coords around to get min dist from ctr either way
      if(full_gauss.wrap_wts) {
        if(fabs((sucw.x + un_full_size.x) - s_un_ctr.x) < fabs(sucw.x - s_un_ctr.x))
          sucw.x = sucw.x + un_full_size.x;
        else if(fabs((sucw.x - un_full_size.x) - s_un_ctr.x) < fabs(sucw.x - s_un_ctr.x))
          sucw.x = sucw.x - un_full_size.x;
        if(fabs((sucw.y + un_full_size.y) - s_un_ctr.y) < fabs(sucw.y - s_un_ctr.y))
          sucw.y = sucw.y + un_full_size.y;
        else if(fabs((sucw.y - un_full_size.y) - s_un_ctr.y) < fabs(sucw.y - s_un_ctr.y))
          sucw.y = sucw.y - un_full_size.y;
      }
    
      float dst = taMath_float::euc_dist(sucw.x, sucw.y, s_un_ctr.x, s_un_ctr.y);
      fwt = taMath_float::gauss_den_nonorm(dst, full_eff_sig);
    }

    float gwt = 1.0f;
    if(gp_gauss.on) {
      suncw = unc;
      if(gp_gauss.wrap_wts) {
        if(fabs((suncw.x + gp_full_size.x) - s_gp_ctr.x) < fabs(suncw.x - s_gp_ctr.x))
          suncw.x = suncw.x + gp_full_size.x;
        else if(fabs((suncw.x - gp_full_size.x) - s_gp_ctr.x) < fabs(suncw.x - s_gp_ctr.x))
          suncw.x = suncw.x - gp_full_size.x;
        if(fabs((suncw.y + gp_full_size.y) - s_gp_ctr.y) < fabs(suncw.y - s_gp_ctr.y))
          suncw.y = suncw.y + gp_full_size.y;
        else if(fabs((suncw.y - gp_full_size.y) - s_gp_ctr.y) < fabs(suncw.y - s_gp_ctr.y))
          suncw.y = suncw.y - gp_full_size.y;
      }
    
      float dst = taMath_float::euc_dist(suncw.x, suncw.y, s_gp_ctr.x, s_gp_ctr.y);
      gwt = taMath_float::gauss_den_nonorm(dst, gp_eff_sig);
    }

    float wt = fwt * gwt;
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

void TiledGpRFPrjnSpec::Init_Weights_Sigmoid(Projection* prjn, ConGroup* cg,
                                             Network* net, int thr_no) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  taVector2f un_full_size = send_lay->un_geom * send_gp_size;
  taVector2f un_half_size = un_full_size;
  un_half_size *= 0.5f;

  float full_eff_gain = full_sig.gain * un_half_size.x;

  taVector2f gp_full_size = send_lay->un_geom;
  taVector2f gp_half_size = gp_full_size;
  gp_half_size *= 0.5f;
  float gp_eff_gain = gp_sig.gain * gp_half_size.x;
  
  taVector2i ru_pos;
  Unit* ru = cg->ThrOwnUn(net, thr_no);
  ru->UnitGpLogPos(ru_pos);

  taVector2f ru_ctr = recv_lay->un_geom;
  ru_ctr *= 0.5f;
  taVector2f ru_nrm_pos = ((taVector2f)ru_pos / ru_ctr);
  taVector2f sgn = 1.0f;
  taVector2f ru_nrm_pos_full;
  taVector2f ru_nrm_pos_gp;
  // su coords
  //  0  1  2   3    4  5  6  7 or - for flipped
  // .2 .4 .6. .8 | -.2 -.4 -.6 -.8
  if(ru_nrm_pos.x >= 1.0f) {
    ru_nrm_pos.x = -ru_nrm_pos.x + 1.0f;
    sgn.x = -1.0f;
    ru_nrm_pos_full.x = (ru_nrm_pos.x + 0.5f) * full_sig.ctr_mv - 0.5f;
    ru_nrm_pos_gp.x = (ru_nrm_pos.x + 0.5f) * gp_sig.ctr_mv - 0.5f;
  }
  else {
    ru_nrm_pos_full.x = (ru_nrm_pos.x - 0.5f) * full_sig.ctr_mv + 0.5f;
    ru_nrm_pos_gp.x = (ru_nrm_pos.x - 0.5f) * gp_sig.ctr_mv + 0.5f;
  }
  if(ru_nrm_pos.y >= 1.0f) {
    ru_nrm_pos.y = -ru_nrm_pos.y + 1.0f;
    sgn.y = -1.0f;
    ru_nrm_pos_full.y = (ru_nrm_pos.y + 0.5f) * full_sig.ctr_mv - 0.5f;
    ru_nrm_pos_gp.y = (ru_nrm_pos.y + 0.5f) * gp_sig.ctr_mv - 0.5f;
  }
  else {
    ru_nrm_pos_full.y = (ru_nrm_pos.y - 0.5f) * full_sig.ctr_mv + 0.5f;
    ru_nrm_pos_gp.y = (ru_nrm_pos.y - 0.5f) * gp_sig.ctr_mv + 0.5f;
  }

  taVector2f s_un_ctr = ru_nrm_pos_full * un_full_size;
  taVector2f s_gp_ctr = ru_nrm_pos_gp * gp_full_size;

  taVector2i unc, ugc;
  taVector2f suc;
  taVector2f sucw;
  taVector2f suncw;
  
  for(int i=0; i<cg->size; i++) {
    // note: these are organized within unit group first, then by groups
    int ug_idx = i / send_lay->un_geom.n; // which unit group, ordinally
    int un_idx = i % send_lay->un_geom.n; // index in unit group

    unc.x = un_idx % send_lay->un_geom.x;
    unc.y = un_idx / send_lay->un_geom.x;

    ugc.x = ug_idx % send_gp_size.x;
    ugc.y = ug_idx / send_gp_size.x;

    float fwt = 1.0f;
    if(full_sig.on) {
      suc = ugc * send_lay->un_geom;
      suc += unc;
      float sig_x = taMath_float::logistic(sgn.x * suc.x, full_eff_gain, s_un_ctr.x);
      float sig_y = taMath_float::logistic(sgn.y * suc.y, full_eff_gain, s_un_ctr.y);
      fwt = sig_x * sig_y;
    }

    float gwt = 1.0f;
    if(gp_sig.on) {
      suncw = unc;
      
      float sig_x = taMath_float::logistic(sgn.x * unc.x, gp_eff_gain, s_gp_ctr.x);
      float sig_y = taMath_float::logistic(sgn.y * unc.y, gp_eff_gain, s_gp_ctr.y);
      gwt = sig_x * sig_y;
    }

    float wt = fwt * gwt;
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
