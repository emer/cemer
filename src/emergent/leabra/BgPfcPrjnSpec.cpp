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

#include "BgPfcPrjnSpec.h"

TA_BASEFUNS_CTORS_DEFN(BgPfcPrjnSpec);

void BgPfcPrjnSpec::Initialize() {
  cross_mnt_out = false;
}

void BgPfcPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  Layer* pfc_lay = NULL;
  Layer* bg_lay = NULL;
  bool  pfc_mnt = false;

  String rlnm = recv_lay->name;
  rlnm.downcase();
  String slnm = send_lay->name;
  slnm.downcase();
  
  if(rlnm.contains("pfc") || rlnm.contains("ofc") || rlnm.contains("acc")) {
    pfc_lay = recv_lay;
    bg_lay = send_lay;
    if(rlnm.contains("out")) {
      pfc_mnt = false;
    }
    else {
      pfc_mnt = true;
    }
  }
  else if(slnm.contains("pfc") || slnm.contains("ofc") || slnm.contains("acc")) {
    pfc_lay = send_lay;
    bg_lay = recv_lay;
    if(slnm.contains("out")) {
      pfc_mnt = false;
    }
    else {
      pfc_mnt = true;
    }
  }
  else {
    TestError(true, "Connect_impl", "could not find a PFC layer on either sender or receiver -- layer name must contain 'pfc' or 'ofc' or 'acc' (case insensitive)");
    return;
  }

  if(!CheckLayerGeoms(bg_lay, pfc_lay))
    return;

  for(int pfcy = 0; pfcy < pfc_lay->gp_geom.y; pfcy++) {
    for(int pfcx = 0; pfcx < pfc_lay->gp_geom.x; pfcx++) {
      int pfcgp = pfcy * pfc_lay->gp_geom.x + pfcx;
      int bgx = pfcx; // mnt = left
      if(!pfc_mnt || cross_mnt_out)
        bgx += pfc_lay->gp_geom.x;
      int bgy = pfcy;
      int bggp = bgy * bg_lay->gp_geom.x + bgx;
      if(pfc_lay == recv_lay) {
        Connect_Gp(prjn, Layer::ACC_GP, pfcgp, Layer::ACC_GP, bggp, make_cons);
      }
      else {
        Connect_Gp(prjn, Layer::ACC_GP, bggp, Layer::ACC_GP, pfcgp, make_cons);
      }
    }
  }
  
  if(!make_cons) { // on first pass through alloc loop, do allocations
    recv_lay->RecvConsPostAlloc(prjn);
    send_lay->SendConsPostAlloc(prjn);
  }
}

bool BgPfcPrjnSpec::CheckLayerGeoms(Layer* bg_layer, Layer* pfc_layer) {
  if(TestError(!bg_layer->unit_groups, "CheckLayerGeoms",
               "Bg layer must have unit groups", bg_layer->name)) {
    return false;
  }
  if(TestError(!pfc_layer->unit_groups, "CheckLayerGeoms",
               "PFC layer must have unit groups", pfc_layer->name)) {
    return false;
  }
  
  if(TestError(bg_layer->gp_geom.x != 2 * pfc_layer->gp_geom.x, "CheckLayerGeoms",
               "BG layer must have gp_geom.x == 2 * PFC layer gp_geom.x.  Bg layer:",
               bg_layer->name, "pfc layer:", pfc_layer->name)) {
    return false;
  }
  if(TestError(bg_layer->gp_geom.y != pfc_layer->gp_geom.y, "CheckLayerGeoms",
               "BG layer must have gp_geom.y == PFC layer gp_geom.y.  Bg layer:",
               bg_layer->name, "pfc layer:", pfc_layer->name)) {
    return false;
  }

  return true;
}

