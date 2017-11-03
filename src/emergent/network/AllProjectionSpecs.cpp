// Copyright 2017, Regents of the University of Colorado,
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

#include "AllProjectionSpecs.h"

#include <ConSpec_cpp>
#include <Projection>
#include <AllProjectionSpecs_cpp> // need the _cpp versions..

#include <taMisc>
#include <taMath_float>

#include <State_main>


TA_BASEFUNS_CTORS_DEFN(FullPrjnSpec);
#include "FullPrjnSpec.cpp"

TA_BASEFUNS_CTORS_DEFN(OneToOnePrjnSpec);
#include "OneToOnePrjnSpec.cpp"

TA_BASEFUNS_CTORS_DEFN(GpOneToOnePrjnSpec);
#include "GpOneToOnePrjnSpec.cpp"

TA_BASEFUNS_CTORS_DEFN(MarkerGpOneToOnePrjnSpec);
#include "MarkerGpOneToOnePrjnSpec.cpp"

TA_BASEFUNS_CTORS_DEFN(GpMapConvergePrjnSpec);
#include "GpMapConvergePrjnSpec.cpp"

TA_BASEFUNS_CTORS_DEFN(GpMapDivergePrjnSpec);
#include "GpMapDivergePrjnSpec.cpp"


TA_BASEFUNS_CTORS_DEFN(RandomPrjnSpec);
#include "RandomPrjnSpec.cpp"

TA_BASEFUNS_CTORS_DEFN(UniformRndPrjnSpec);
#include "UniformRndPrjnSpec.cpp"

TA_BASEFUNS_CTORS_DEFN(PolarRndPrjnSpec);
#include "PolarRndPrjnSpec.cpp"

void PolarRndPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////
//              TesselPrjnSpec

TA_BASEFUNS_CTORS_DEFN(TesselPrjnSpec);
TA_BASEFUNS_CTORS_DEFN(TessEl);
TA_BASEFUNS_CTORS_LITE_DEFN(TessEl_List);
#include "TesselPrjnSpec.cpp"

void TesselPrjnSpec::UpdateAfterEdit_impl() {
  SyncSendOffs();
  inherited::UpdateAfterEdit_impl(); // calls state sync
  recv_skip.SetGtEq(1);
  recv_group.SetGtEq(1);
}

void TesselPrjnSpec::SyncSendOffs() {
  if(send_offs.size > alloc_send_offs) {
    AllocSendOffs(send_offs.size);
  }
  for(int i=0; i<send_offs.size; i++) {
    TessEl* so = send_offs[i];
    TessEl* cp = send_offs_m + i;
    cp->send_off.SetXY(so->send_off.x, so->send_off.y);
    cp->wt_val = so->wt_val;
  }
  n_send_offs = send_offs.size;
}

void TesselPrjnSpec::CopyToState(void* state_spec, const char* state_suffix) {
  inherited::CopyToState(state_spec, state_suffix);
  CopyToState_SendOffs(state_spec, state_suffix);
}

void TesselPrjnSpec::UpdateStateSpecs() {
  SyncSendOffs();
  inherited::UpdateStateSpecs();
}

// note: following requires access to TessEl_cpp -- hence AllProjectionSpecs_cpp include..

void TesselPrjnSpec::CopyToState_SendOffs(void* state_spec, const char* state_suffix) {
  String ss = state_suffix;
  if(ss == "_cpp") {
    TesselPrjnSpec_cpp* tcc = (TesselPrjnSpec_cpp*)state_spec;
    tcc->AllocSendOffs(n_send_offs);
    for(int i=0; i<n_send_offs; i++) {
      TessEl* so = send_offs_m + i;
      TessEl_cpp* cp = tcc->send_offs_m + i;
      cp->send_off.SetXY(so->send_off.x, so->send_off.y);
      cp->wt_val = so->wt_val;
    }
    tcc->n_send_offs = n_send_offs;
  }
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


////////////////////////////////////////////////////////////////////////////////////
//              GpTesselPrjnSpec

TA_BASEFUNS_CTORS_DEFN(GpTesselPrjnSpec);
TA_BASEFUNS_CTORS_DEFN(GpTessEl);
TA_BASEFUNS_CTORS_LITE_DEFN(GpTessEl_List);

#include "GpTesselPrjnSpec.cpp"

void GpTesselPrjnSpec::UpdateAfterEdit_impl() {
  SyncSendOffs();
  inherited::UpdateAfterEdit_impl();
  recv_gp_skip.SetGtEq(1);
  recv_gp_group.SetGtEq(1);
}

void GpTesselPrjnSpec::SyncSendOffs() {
  if(send_gp_offs.size > alloc_send_gp_offs) {
    AllocSendOffs(send_gp_offs.size);
  }
  for(int i=0; i<send_gp_offs.size; i++) {
    GpTessEl* so = send_gp_offs[i];
    GpTessEl* cp = send_gp_offs_m + i;
    cp->send_gp_off.SetXY(so->send_gp_off.x, so->send_gp_off.y);
    cp->p_con = so->p_con;
  }
  n_send_gp_offs = send_gp_offs.size;
}

void GpTesselPrjnSpec::CopyToState(void* state_spec, const char* state_suffix) {
  inherited::CopyToState(state_spec, state_suffix);
  CopyToState_SendOffs(state_spec, state_suffix);
}

void GpTesselPrjnSpec::UpdateStateSpecs() {
  SyncSendOffs();
  inherited::UpdateStateSpecs();
}

// note: following requires access to GpTessEl_cpp -- hence AllProjectionSpecs_cpp include..

void GpTesselPrjnSpec::CopyToState_SendOffs(void* state_spec, const char* state_suffix) {
  String ss = state_suffix;
  if(ss == "_cpp") {
    GpTesselPrjnSpec_cpp* tcc = (GpTesselPrjnSpec_cpp*)state_spec;
    tcc->AllocSendOffs(n_send_gp_offs);
    for(int i=0; i<n_send_gp_offs; i++) {
      GpTessEl* so = send_gp_offs_m + i;
      GpTessEl_cpp* cp = tcc->send_gp_offs_m + i;
      cp->send_gp_off.SetXY(so->send_gp_off.x, so->send_gp_off.y);
      cp->p_con = so->p_con;
    }
    tcc->n_send_gp_offs = n_send_gp_offs;
  }
}

void GpTesselPrjnSpec::MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y) {
  last_make_cmd = "MakeEllipse( half_width=" + String(half_width)
    + ", half_height=" + String(half_height)
    + ", ctr_x=" + String(ctr_x) + ", ctr_y=" + String(ctr_y) + ")";
  SigEmitUpdated();
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

void GpTesselPrjnSpec::MakeRectangle(int width, int height, int left, int bottom) {
  last_make_cmd = "MakeRectangle(width=" + String(width) + ", height=" + String(height)
    + ", left=" + String(left) + ", bottom=" + String(bottom) + ")";
  SigEmitUpdated();
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

void GpTesselPrjnSpec::SetPCon(float p_con, int start, int end) {
  if(end == -1) end = send_gp_offs.size;  else end = MIN(send_gp_offs.size, end);
  int i;
  for(i=start;i<end;i++) {
    GpTessEl* te = (GpTessEl*)send_gp_offs.FastEl(i);
    te->p_con = p_con;
  }
}




////////////////////////////////////////////////////////////////////////////////////
//              TiledGpRFPrjnSpec

TA_BASEFUNS_CTORS_DEFN(GaussInitWtsSpec);
TA_BASEFUNS_CTORS_DEFN(SigmoidInitWtsSpec);
TA_BASEFUNS_CTORS_DEFN(TiledGpRFPrjnSpec);

#include "TiledGpRFPrjnSpec.cpp"

void TiledGpRFPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  wt_range.UpdateAfterEdit_NoGui();
  if(TestWarning(full_send == ALL_SAME, "UAE",
                 "full_send == ALL_SAME is not a valid option, switching back to BY_UNIT")) {
    full_send = BY_UNIT;
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



TA_BASEFUNS_CTORS_DEFN(TiledGpRFOneToOnePrjnSpec);
#include "TiledGpRFOneToOnePrjnSpec.cpp"

TA_BASEFUNS_CTORS_DEFN(TiledGpRFOneToOneWtsPrjnSpec);
#include "TiledGpRFOneToOneWtsPrjnSpec.cpp"


////////////////////////////////////////////////////////////////////////////////////
//              TiledSubGpRFPrjnSpec

TA_BASEFUNS_CTORS_DEFN(TiledSubGpRFPrjnSpec);

#include "TiledSubGpRFPrjnSpec.cpp"

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


////////////////////////////////////////////////////////////////////////////////////
//              GaussRFPrjnSpec

TA_BASEFUNS_CTORS_DEFN(GaussRFPrjnSpec);

#include "GaussRFPrjnSpec.cpp"

bool GaussRFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
  trg_send_geom.x = send_x;
  trg_send_geom.y = send_y;

  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  // now fix it the other way
  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom +1) * rf_move);

  SigEmitUpdated();
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool GaussRFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
  trg_recv_geom.x = recv_x;
  trg_recv_geom.y = recv_y;

  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom+1) * rf_move);

  // now fix it the other way
  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  SigEmitUpdated();
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}




