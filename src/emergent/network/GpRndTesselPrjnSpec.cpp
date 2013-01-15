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

#include "GpRndTesselPrjnSpec.h"
#include <Network>

void GpTessEl::Initialize() {
  p_con = 1.0f;
}

void GpRndTesselPrjnSpec::Initialize() {
  recv_gp_n = -1;
  recv_gp_skip = 1;
  recv_gp_group = 1;
  send_gp_scale = 1.0f;
  send_gp_offs.SetBaseType(&TA_GpTessEl);

  wrap = true;
  def_p_con = .25f;
  sym_self = true;
  same_seed = false;
  rndm_seed.GetCurrent();
}

void GpRndTesselPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  recv_gp_skip.SetGtEq(1);
  recv_gp_group.SetGtEq(1);
}

void GpRndTesselPrjnSpec::MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y) {
  send_gp_offs.Reset();
  int strt_x = ctr_x - half_width;
  int end_x = ctr_x + half_width;
  int strt_y = ctr_y - half_height;
  int end_y = ctr_y + half_height;
  if(half_width == half_height) { // circle
    int y;
    for(y = strt_y; y <= end_y; y++) {
      int x;
      for(x = strt_x; x <= end_x; x++) {
        int dist = ((x - ctr_x) * (x - ctr_x)) + ((y - ctr_y) * (y - ctr_y));
        if(dist > (half_width * half_width))
          continue;             // outside the circle
        GpTessEl* te = (GpTessEl*)send_gp_offs.New(1, &TA_GpTessEl);
        te->send_gp_off.x = x;
        te->send_gp_off.y = y;
        te->p_con = def_p_con;
      }
    }
  }
  else {                        // ellipse
    float f1_x, f1_y;           // foci
    float f2_x, f2_y;
    float two_a;                        // two times largest axis

    if(half_width > half_height) {
      two_a = (float)half_width * 2;
      float c = sqrtf((float)(half_width * half_width) - (float)(half_height * half_height));
      f1_x = (float)ctr_x - c;
      f1_y = (float)ctr_y;
      f2_x = (float)ctr_x + c;
      f2_y = (float)ctr_y;
    }
    else {
      two_a = (float)half_height * 2;
      float c = sqrtf((float)(half_height * half_height) - (float)(half_width * half_width));
      f1_x = (float)ctr_x;
      f1_y = (float)ctr_y - c;
      f2_x = (float)ctr_x;
      f2_y = (float)ctr_y + c;
    }

    int y;
    for(y = strt_y; y <= end_y; y++) {
      int x;
      for(x = strt_x; x <= end_x; x++) {
        float dist = sqrtf((((float)x - f1_x) * ((float)x - f1_x)) + (((float)y - f1_y) * ((float)y - f1_y))) +
          sqrtf((((float)x - f2_x) * ((float)x - f2_x)) + (((float)y - f2_y) * ((float)y - f2_y)));
        if(dist > two_a)
          continue;
        GpTessEl* te = (GpTessEl*)send_gp_offs.New(1, &TA_GpTessEl);
        te->send_gp_off.x = x;
        te->send_gp_off.y = y;
        te->p_con = def_p_con;
      }
    }
  }
}

void GpRndTesselPrjnSpec::MakeRectangle(int width, int height, int left, int bottom) {
  send_gp_offs.Reset();
  int y;
  for(y = bottom; y < bottom + height; y++) {
    int x;
    for(x = left; x < left + width; x++) {
      GpTessEl* te = (GpTessEl*)send_gp_offs.New(1, &TA_GpTessEl);
      te->send_gp_off.x = x;
      te->send_gp_off.y = y;
      te->p_con = def_p_con;
    }
  }
}

void GpRndTesselPrjnSpec::SetPCon(float p_con, int start, int end) {
  if(end == -1) end = send_gp_offs.size;  else end = MIN(send_gp_offs.size, end);
  int i;
  for(i=start;i<end;i++) {
    GpTessEl* te = (GpTessEl*)send_gp_offs.FastEl(i);
    te->p_con = p_con;
  }
}


void GpRndTesselPrjnSpec::GetCtrFmRecv(taVector2i& sctr, taVector2i ruc) {
  ruc -= recv_gp_off;
  ruc /= recv_gp_group; ruc *= recv_gp_group;   // this takes int part of
  ruc += recv_gp_off;   // then re-add offset
  taVector2f scruc = ruc;
  scruc *= send_gp_scale;
  sctr = scruc;         // center of sending units
  sctr += send_gp_border;
}

void GpRndTesselPrjnSpec::Connect_Gps(int rgpidx, int sgpidx, float p_con,
                                      Projection* prjn, bool send_alloc) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  bool same_gp = (recv_lay == send_lay && rgpidx == sgpidx);

  if(send_alloc) {
    int send_no = 1;
    int recv_no = 1;
    if(p_con < 1.0f) {
      if(p_con < 0) p_con = 1.0f;
      if(!self_con && same_gp)
        recv_no = (int) ((p_con * (float)(su_nunits-1)) + .5f);
      else
        recv_no = (int) ((p_con * (float)su_nunits) + .5f);
      if(recv_no <= 0)  recv_no = 1;
      recv_no = MAX(su_nunits, recv_no);

      // sending number is even distribution across senders plus some imbalance factor
      float send_no_flt = (float)(ru_nunits * recv_no) / (float)su_nunits;
      // add SEM as corrective factor
      float send_sem = send_no_flt / sqrtf(send_no_flt);
      send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
      send_no = MIN(ru_nunits, send_no);

      if(same_gp)
        recv_no += 2;           // bit of extra room here too
    }
    else {
      send_no = ru_nunits;
      recv_no = su_nunits;
    }
    for(int sui=0;sui<su_nunits;sui++) {
      Unit* su = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      su->SendConsAllocInc(send_no, prjn);
    }

    // also do incremental alloc of the recv guy too!
    for(int rui=0;rui<ru_nunits;rui++) {
      Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      ru->RecvConsAllocInc(recv_no, prjn);
    }
    return;
  }

  if(p_con < 0) {               // this means: make symmetric connections!
    Connect_Gps_Sym(rgpidx, sgpidx, p_con, prjn);
  }
  else if(p_con == 1.0f) {
    Connect_Gps_Full(rgpidx, sgpidx, prjn);
  }
  else if(same_gp && sym_self) {
    Connect_Gps_SymSameGp(rgpidx, sgpidx, p_con, prjn);
  }
  else {
    if(recv_lay == send_lay && sym_self) {
      Connect_Gps_SymSameLay(rgpidx, sgpidx, p_con, prjn);
    }
    else {
      Connect_Gps_Std(rgpidx, sgpidx, p_con, prjn);
    }
  }
}

void GpRndTesselPrjnSpec::Connect_Gps_Sym(int rgpidx, int sgpidx,
                                          float p_con, Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  if((prjn->from.ptr() != prjn->layer) || !sym_self)
    return;                     // not applicable otherwise!

  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    for(int g=0;g<ru->send.size;g++) {
      SendCons* scg = ru->send.FastEl(g);
      if((scg->prjn->layer != send_lay) || (scg->prjn->layer != recv_lay))
        continue;               // only deal with self projections to this same layer
      for(int i=0;i<scg->size;i++) {
        Unit* su = scg->Un(i);
        // only connect if this sender is in actual group I'm trying to connect
        int osgpidx = su->UnitGpIdx();
        if(osgpidx == sgpidx) {
          ru->ConnectFromCk(su, prjn);
        }
      }
    }
  }
}

void GpRndTesselPrjnSpec::Connect_Gps_SymSameGp(int rgpidx, int sgpidx,
                                float p_con, Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  // trick is to divide cons in half, choose recv, send at random
  // for 1/2 cons, then go through all units and make the symmetric cons..
  // pre-allocate connections!
  TestWarning(p_con > .95f, "Connect_Gps_SymSameGp",
              "usually produces less than complete connectivity for high values of p_con in symmetric, self-connected layers using permute!");
  int n_cons;
  if(!self_con)
    n_cons = (int) (p_con * (float)(su_nunits-1) + .5f);
  else
    n_cons = (int) (p_con * (float)su_nunits + .5f);
  int first = (int)(.5f * (float)n_cons);
  if(first <= 0) first = 1;


  UnitPtrList ru_list;          // receiver permution list
  for(int rui=0; rui<ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    ru_list.Link(ru);                   // on making a symmetric connection in first pass
  }
  ru_list.Permute();

  UnitPtrList perm_list;
  for(int i=0;i<ru_list.size; i++) {
    Unit* ru = ru_list.FastEl(i);
    perm_list.Reset();
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      if(!self_con && (ru == su)) continue;
      // don't connect to anyone who already recvs from me cuz that will make
      // a symmetric connection which isn't good: symmetry will be enforced later
      RecvCons* scg = su->recv.FindPrjn(prjn);
      if(scg->FindConFromIdx(ru) >= 0) continue;
      perm_list.Link(su);
    }
    perm_list.Permute();

    for(int j=0; j<first && j<perm_list.size; j++) {    // only connect 1/2 of the units
      ru->ConnectFromCk((Unit*)perm_list[j], prjn);
    }
  }
  // now go thru and make the symmetric connections
  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    SendCons* scg = ru->send.FindPrjn(prjn);
    if(scg == NULL) continue;
    for(int i=0;i<scg->size;i++) {
      Unit* su = scg->Un(i);
      ru->ConnectFromCk(su, prjn);
    }
  }
}

void GpRndTesselPrjnSpec::Connect_Gps_SymSameLay(int rgpidx, int sgpidx,
                                                 float p_con, Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  // within the same layer, i want to make connections symmetric: either i'm the
  // first to connect to other group, or other group has already connected to me
  // so I should just make symmetric versions of its connections
  // take first send unit and find if it recvs from anyone in this prjn yet
  Unit* su = send_lay->UnitAtUnGpIdx(0, sgpidx);
  RecvCons* scg = su->recv.FindPrjn(prjn);
  if((scg != NULL) && (scg->size > 0)) {        // sender has been connected already: try to connect me!
    int n_con = 0;              // number of actual connections made

    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      SendCons* scg = ru->send.FindPrjn(prjn);
      if(scg == NULL) continue;
      for(int i=0;i<scg->size;i++) {
        Unit* su = scg->Un(i);
        // only connect if this sender is in actual group I'm trying to connect
        int osgpidx = su->UnitGpIdx();
        if(osgpidx == sgpidx) {
          if(ru->ConnectFromCk(su, prjn))
            n_con++;
        }
      }
    }
    if(n_con > 0)               // made some connections, bail
      return;
    // otherwise, go ahead and make new connections!
  }
  Connect_Gps_Std(rgpidx, sgpidx, p_con, prjn);
}

void GpRndTesselPrjnSpec::Connect_Gps_Std(int rgpidx, int sgpidx,
                                          float p_con, Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  bool same_gp = (recv_lay == send_lay && rgpidx == sgpidx);

  int recv_no;
  if(!self_con && same_gp)
    recv_no = (int) ((p_con * (float)(su_nunits-1)) + .5f);
  else
    recv_no = (int) ((p_con * (float)su_nunits) + .5f);
  if(recv_no <= 0)  recv_no = 1;

  UnitPtrList perm_list;        // permution list
  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    perm_list.Reset();
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      if(!self_con && (ru == su)) continue;
      perm_list.Link(su);
    }
    for(int i=0; i<recv_no; i++)
      ru->ConnectFrom((Unit*)perm_list[i], prjn);
  }
}

void GpRndTesselPrjnSpec::Connect_Gps_Full(int rgpidx, int sgpidx,
                                           Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      if(self_con || (ru != su))
        ru->ConnectFrom(su, prjn);
    }
  }
}

void GpRndTesselPrjnSpec::Connect_RecvGp(int rgpidx, const taVector2i& ruc,
                                         Projection* prjn, bool send_alloc) {

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  taVector2i& su_geo = send_lay->gp_geom;
  taVector2i sctr;
  GetCtrFmRecv(sctr, ruc);  // positions of center of recv in sending layer
  for(int i = 0; i< send_gp_offs.size; i++) {
    GpTessEl* te = (GpTessEl*)send_gp_offs.FastEl(i);
    taVector2i suc = te->send_gp_off + sctr;
    if(suc.WrapClip(wrap, su_geo) && !wrap)
      continue;
    int sgpidx = send_lay->UnitGpIdxFmPos(suc);
    if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;
    Connect_Gps(rgpidx, sgpidx, te->p_con, prjn, send_alloc);
  }
}

void GpRndTesselPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(same_seed)
    rndm_seed.OldSeed();

  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;

  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups,  "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  taVector2i& ru_geo = prjn->layer->gp_geom;
  taVector2i use_recv_gp_n = recv_gp_n;

  if(recv_gp_n.x == -1)
    use_recv_gp_n.x = ru_geo.x;
  if(recv_gp_n.y == -1)
    use_recv_gp_n.y = ru_geo.y;

  taVector2i ruc, nuc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    for(ruc.y = recv_gp_off.y, nuc.y = 0; (ruc.y < ru_geo.y) && (nuc.y < use_recv_gp_n.y);
        ruc.y += recv_gp_skip.y, nuc.y++)
      {
        for(ruc.x = recv_gp_off.x, nuc.x = 0; (ruc.x < ru_geo.x) && (nuc.x < use_recv_gp_n.x);
            ruc.x += recv_gp_skip.x, nuc.x++)
          {
            int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
            if(!recv_lay->UnitGpIdxIsValid(rgpidx)) continue;
            Connect_RecvGp(rgpidx, ruc, prjn, alloc_loop);
          }
      }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      recv_lay->RecvConsPostAlloc(prjn);
      send_lay->SendConsPostAlloc(prjn);
    }
  }
}

