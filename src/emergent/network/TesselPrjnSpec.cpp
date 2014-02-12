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

#include "TesselPrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(TesselPrjnSpec);

TA_BASEFUNS_CTORS_DEFN(TessEl);

TA_BASEFUNS_CTORS_LITE_DEFN(TessEl_List);

void TessEl::Initialize() {
  wt_val = 1.0f;
}

void TesselPrjnSpec::Initialize() {
  recv_n = -1;
  recv_skip = 1;
  recv_group = 1;
  wrap = true;
  send_scale = 1.0f;
  send_offs.SetBaseType(&TA_TessEl);
}

void TesselPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  recv_skip.SetGtEq(1);
  recv_group.SetGtEq(1);
}

void TesselPrjnSpec::MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y) {
  last_make_cmd = "MakeEllipse( half_width=" + String(half_width)
    + ", half_height=" + String(half_height)
    + ", ctr_x=" + String(ctr_x) + ", ctr_y=" + String(ctr_y) + ")";
  SigEmitUpdated();
  send_offs.Reset();
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
        TessEl* te = (TessEl*)send_offs.New(1, &TA_TessEl);
        te->send_off.x = x;
        te->send_off.y = y;
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
        TessEl* te = (TessEl*)send_offs.New(1, &TA_TessEl);
        te->send_off.x = x;
        te->send_off.y = y;
      }
    }
  }
}

void TesselPrjnSpec::MakeRectangle(int width, int height, int left, int bottom) {
  last_make_cmd = "MakeRectangle(width=" + String(width) + ", height=" + String(height)
    + ", left=" + String(left) + ", bottom=" + String(bottom) + ")";
  SigEmitUpdated();
  send_offs.Reset();
  int y;
  for(y = bottom; y < bottom + height; y++) {
    int x;
    for(x = left; x < left + width; x++) {
      TessEl* te = (TessEl*)send_offs.New(1, &TA_TessEl);
      te->send_off.x = x;
      te->send_off.y = y;
    }
  }
}
/*TODO
void TesselPrjnSpec::MakeFromNetView(NetView* view) {
  if((view == NULL) || (view->editor == NULL) || (view->editor->netg == NULL))
    return;
  if(view->editor->netg->selectgroup.size <= 0) {
    taMisc::Error("Must select some units to get connection pattern from");
    return;
  }
  send_offs.Reset();
  int i;
  taBase* itm;
  Unit* center = NULL;
  for(i=0; i< view->editor->netg->selectgroup.size; i++) {
    itm = view->editor->netg->selectgroup.FastEl(i);
    if(!itm->InheritsFrom(TA_Unit))      continue;
    Unit* un = (Unit*) itm;
    if(center == NULL) {
      center = un;
      continue;
    }
    TessEl* te = (TessEl*)send_offs.New(1, &TA_TessEl);
    te->send_off = un->pos - center->pos;
  }
}*/

void TesselPrjnSpec::WeightsFromDist(float scale) {
  last_weights_cmd = "WeightsFromDist(scale=" + String(scale) + ")";
  SigEmitUpdated();
  taVector2i zero;
  int i;
  TessEl* te;
  for(i = 0; i< send_offs.size; i++) {
    te = (TessEl*)send_offs.FastEl(i);
    float dist = te->send_off.Dist(zero);
    te->wt_val = scale * (1.0f / dist);
  }
}

void TesselPrjnSpec::WeightsFromGausDist(float scale, float sigma) {
  last_weights_cmd = "WeightsFromGausDist(scale=" + String(scale)
    + ", sigma=" + String(sigma) + ")";
  SigEmitUpdated();
  taVector2i zero;
  int i;
  TessEl* te;
  for(i = 0; i< send_offs.size; i++) {
    te = (TessEl*)send_offs.FastEl(i);
    float dist = te->send_off.Dist(zero);
    te->wt_val = scale * exp(-0.5 * dist / (sigma * sigma));
  }
}

// todo: this assumes that things are in order.. (can't really check otherwise)
// which breaks for clipped patterns
void TesselPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Network* net = prjn->layer->own_net;
  int mxi = MIN(cg->size, send_offs.size);
  int i;
  for(i=0; i<mxi; i++) {
    TessEl* te = (TessEl*)send_offs.FastEl(i);
    cg->Cn(i,BaseCons::WT,net) = te->wt_val;
  }
}

void TesselPrjnSpec::GetCtrFmRecv(taVector2i& sctr, taVector2i ruc) {
  ruc -= recv_off;
  ruc /= recv_group;    ruc *= recv_group;      // this takes int part of
  ruc += recv_off;      // then re-add offset
  taVector2f scruc = ruc;
  scruc *= send_scale;
  scruc += send_off;
  sctr = scruc;         // take int part at the end
}

void TesselPrjnSpec::Connect_RecvUnit(Unit* ru_u, const taVector2i& ruc, Projection* prjn,
                                      bool send_alloc) {
  // allocate cons
  if(!send_alloc)
    ru_u->RecvConsPreAlloc(send_offs.size, prjn);

  PosVector2i su_geo = prjn->from->flat_geom;
  // positions of center of recv in sending layer
  taVector2i sctr;
  GetCtrFmRecv(sctr, ruc);
  int i;
  TessEl* te;
  for(i = 0; i< send_offs.size; i++) {
    te = (TessEl*)send_offs.FastEl(i);
    taVector2i suc = te->send_off + sctr;
    if(suc.WrapClip(wrap, su_geo) && !wrap)
      continue;
    Unit* su_u = prjn->from->UnitAtCoord(suc);
    if((su_u == NULL) || (!self_con && (su_u == ru_u)))
      continue;
    if(send_alloc)
      ru_u->ConnectFrom(su_u, prjn, send_alloc);
    else
      ru_u->ConnectFromCk(su_u, prjn); // check on 2nd pass
  }
}

void TesselPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;

//   TestWarning(!wrap && init_wts, "Connect_impl",
//            "non-wrapped tessel prjn spec with init_wts does not usually work!");
  PosVector2i ru_geo = prjn->layer->flat_geom;

  taVector2i use_recv_n = recv_n;

  if(recv_n.x == -1)
    use_recv_n.x = ru_geo.x;
  if(recv_n.y == -1)
    use_recv_n.y = ru_geo.y;

  taVector2i ruc, nuc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    for(ruc.y = recv_off.y, nuc.y = 0; (ruc.y < ru_geo.y) && (nuc.y < use_recv_n.y);
        ruc.y += recv_skip.y, nuc.y++)
      {
        for(ruc.x = recv_off.x, nuc.x = 0; (ruc.x < ru_geo.x) && (nuc.x < use_recv_n.x);
            ruc.x += recv_skip.x, nuc.x++)
          {
            Unit* ru_u = prjn->layer->UnitAtCoord(ruc);
            if(ru_u == NULL)
              continue;
            Connect_RecvUnit(ru_u, ruc, prjn, alloc_loop);
          }
      }

    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}


