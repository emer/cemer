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
  n_pfcs = 2;
  cross_connect = false;
  InitBgTable();
}

void BgPfcPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdtBgTable();
}


void BgPfcPrjnSpec::FormatBgTable() {
  DataCol* dc;
  
  dc = bg_table.FindMakeCol("pfc_name", VT_STRING);
  dc->desc = "name of the PFC layer -- layer must uniquely contain this name -- case sensitive maching is used";

  dc = bg_table.FindMakeCol("size_x", VT_INT);
  dc->desc = "x (horizontal) size of this pfc layer in terms of unit groups (stripes) -- only needed if the PFC layers are NOT all the same size -- a -1 indicates that all pfc layers are the same size";

  dc = bg_table.FindMakeCol("size_y", VT_INT);
  dc->desc = "y (vertical) size of this pfc layer in terms of unit groups (stripes) -- only needed if the PFC layers are NOT all the same size -- a -1 indicates that all pfc layers are the same size";

  dc = bg_table.FindMakeCol("start_x", VT_INT);
  dc->desc = "starting x (horizontal) coordinate of the unit group of stripes for this pfc within the bg layer -- a -1 indicates that PFC layers are organized horizontally across, and position computed automatically from sizes";

  dc = bg_table.FindMakeCol("start_y", VT_INT);
  dc->desc = "starting y (vertical) coordinate of the unit group of stripes for this pfc within the bg layer -- layers organized horizontally across by default";

  bg_table.EnforceRows(n_pfcs);
}

void BgPfcPrjnSpec::InitBgTable() {
  n_pfcs = 2;
  FormatBgTable();

  int cur = 0;
  SetBgTableVal(Variant("PFCmnt"), BGT_NAME, cur);
  SetBgTableVal(-1, BGT_SIZE_X, cur);
  SetBgTableVal(-1, BGT_SIZE_Y, cur);
  SetBgTableVal(-1, BGT_START_X, cur);
  SetBgTableVal(0, BGT_START_Y, cur);

  cur++;
  SetBgTableVal(Variant("PFCout"), BGT_NAME, cur);
  SetBgTableVal(-1, BGT_SIZE_X, cur);
  SetBgTableVal(-1, BGT_SIZE_Y, cur);
  SetBgTableVal(-1, BGT_START_X, cur);
  SetBgTableVal(0, BGT_START_Y, cur);
}

void BgPfcPrjnSpec::UpdtBgTable() {
  bg_table.StructUpdate(true);
  FormatBgTable();
  if(bg_table.rows == 0) {
    InitBgTable();
  }
  else {
    String nm = GetBgTableVal(BGT_NAME, 0).toString();
    if(nm.empty())
      InitBgTable();
  }
  bg_table.StructUpdate(false);
}

void BgPfcPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  Layer* pfc_lay = NULL;
  Layer* bg_lay = NULL;

  bool found_a_pfc = false;
  bool found_a_bg = false;

  // first find out if one end of the thing is a pfc at all
  for(int i=0; i<bg_table.rows; i++) {
    String nm = GetBgTableVal(BGT_NAME, i).toString();
    if(recv_lay->name.contains(nm)) {
      found_a_pfc = true;
      pfc_lay = recv_lay;
      bg_lay = send_lay;
    }
    else if(send_lay->name.contains(nm)) {
      found_a_pfc = true;
      pfc_lay = send_lay;
      bg_lay = recv_lay;
    }
  }

  if(!found_a_pfc) {            // look for a bg
    String bg_names = "GP SNr Matrix";
    String_Array bg_nm_ary;
    bg_nm_ary.Split(bg_names, " ");
    for(int i=0; i<bg_nm_ary.size; i++) {
      String nm = bg_nm_ary[i];
      if(recv_lay->name.contains(nm)) {
        found_a_bg = true;
        bg_lay = recv_lay;
        pfc_lay = send_lay;
      }
      else if(send_lay->name.contains(nm)) {
        found_a_bg = true;
        bg_lay = send_lay;
        pfc_lay = recv_lay;
      }
    }
  }

  if(!found_a_pfc && !found_a_bg) {
    if(recv_lay->gp_geom.n < send_lay->gp_geom.n) { // bg is usu bigger!
      pfc_lay = recv_lay;
      bg_lay = send_lay;
    }
    else {
      pfc_lay = send_lay;
      bg_lay = recv_lay;
    }
  }

  if(TestError(!found_a_pfc && !cross_connect, "Connect_impl",
               "could not find PFC layer from either recv layer:", recv_lay->name,
               "or send layer:", send_lay->name,
               "must use cross_connect in this case")) {
    return;
  }
  
  int trg_sz_x = 0;
  int trg_sz_y = 0;
  int sz_x = 0;
  int st_x = -1;
  int st_y = -1;
  for(int i=0; i<bg_table.rows; i++) {
    String nm = GetBgTableVal(BGT_NAME, i).toString();
    int szx = GetBgTableVal(BGT_SIZE_X, i).toInt();
    int szy = GetBgTableVal(BGT_SIZE_Y, i).toInt();
    int stx = GetBgTableVal(BGT_START_X, i).toInt();
    int sty = GetBgTableVal(BGT_START_Y, i).toInt();

    bool got = false;
    if(cross_connect) {
      if(connect_as.contains(nm)) {
        got = true;
      }
    }
    else {
      if(recv_lay->name.contains(nm) || send_lay->name.contains(nm)) {
        got = true;
      }
    }

    if(szx > 0 && szy > 0) {
      trg_sz_x = szx;
      trg_sz_y = szy;
    }
    else if(found_a_pfc) {
      trg_sz_x = pfc_lay->gp_geom.x;
      trg_sz_y = pfc_lay->gp_geom.y;
    }
    else {
      trg_sz_x = bg_lay->gp_geom.x / bg_table.rows; // assume even
      trg_sz_y = bg_lay->gp_geom.y;
    }

    if(got) {
      if(stx < 0) {
        st_x = sz_x;
      }
      else {
        st_x = stx;
      }
      if(sty <= 0) {
        st_y = 0;
      }
      else {
        st_y = sty;
      }
      break;
    }

    if(szx > 0) {
      sz_x += szx;
    }
    else {
      sz_x += trg_sz_x;
    }
  }
    
  if(TestError(pfc_lay == NULL, "Connect_impl",
               "could not find PFC layer from either recv layer:", recv_lay->name,
               "or send layer:", send_lay->name)) {
    return;
  }

  if(TestError(!bg_lay->unit_groups, "Connect_impl",
               "Bg layer must have unit groups", bg_lay->name)) {
    return;
  }

  if(!found_a_pfc) {
    // do full connectivity into each of the bg groups -- typically from an input layer..
    for(int bgy = 0; bgy < trg_sz_y; bgy++) {
      for(int bgx = 0; bgx < trg_sz_x; bgx++) {
        int bggp = bgy * bg_lay->gp_geom.x + bgx;
        if(bg_lay == recv_lay) {
          Connect_Gp(prjn, Layer::ACC_GP, bggp, Layer::ACC_LAY, 0, make_cons);
        }
        else {
          Connect_Gp(prjn, Layer::ACC_LAY, 0, Layer::ACC_GP, bggp, make_cons);
        }
      }
    }
  
    if(!make_cons) { // on first pass through alloc loop, do allocations
      recv_lay->RecvConsPostAlloc(prjn);
      send_lay->SendConsPostAlloc(prjn);
    }
  }
  else {
    if(TestError((bg_lay->gp_geom.x < (st_x + pfc_lay->gp_geom.x)), "Connect_impl",
                 "BG layer is not big enough to hold pfc layer:",
                 pfc_lay->name, "starting at x offfset:",
                 String(st_x), "with x unit groups:", String(pfc_lay->gp_geom.x))) {
      return;
    }
    if(TestError((bg_lay->gp_geom.y < (st_y + pfc_lay->gp_geom.y)), "Connect_impl",
                 "BG layer is not big enough to hold pfc layer:",
                 pfc_lay->name, "starting at y offfset:",
                 String(st_y), "with y unit groups:", String(pfc_lay->gp_geom.y))) {
      return;
    }

    for(int pfcy = 0; pfcy < pfc_lay->gp_geom.y; pfcy++) {
      for(int pfcx = 0; pfcx < pfc_lay->gp_geom.x; pfcx++) {
        int pfcgp = pfcy * pfc_lay->gp_geom.x + pfcx;
        int bgx = st_x + pfcx;
        int bgy = st_y + pfcy;
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
}

