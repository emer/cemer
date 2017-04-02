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

#include "GradientWtsPrjnSpec.h"
#include <Network>
#include <taMath_float>

TA_BASEFUNS_CTORS_DEFN(GradientWtsPrjnSpec);

void GradientWtsPrjnSpec::Initialize() {
  wt_range.min = 0.0f;
  wt_range.max = 0.5f;
  wt_range.UpdateAfterEdit_NoGui();
  invert = false;
  grad_x = true;
  grad_y = true;
  wrap = true;
  grad_type = LINEAR;
  use_gps = true;
  gauss_sig = 0.3f;
  Defaults_init();
}

void GradientWtsPrjnSpec::Defaults_init() {
  init_wts = true;
  add_rnd_var = true;
}


void GradientWtsPrjnSpec::Init_Weights_Prjn(Projection* prjn, ConGroup* cg,
                                            Network* net, int thr_no) {
  Unit* ru = cg->OwnUn(net);
  if(use_gps && prjn->layer->unit_groups)
    InitWeights_RecvGps(prjn, cg, ru, net, thr_no);
  else
    InitWeights_RecvFlat(prjn, cg, ru, net, thr_no);
}


void GradientWtsPrjnSpec::SetWtFmDist(Projection* prjn, ConGroup* cg, Unit* ru,
                                      Network* net, float dist, int cg_idx, int thr_no) {
  float wt_val = wt_range.min;
  if(grad_type == LINEAR) {
    if(invert)
      wt_val = wt_range.min + dist * wt_range.Range();
    else
      wt_val = wt_range.max - dist * wt_range.Range();
  }
  else if(grad_type == GAUSSIAN) {
    float gaus = taMath_float::gauss_den_nonorm(dist, gauss_sig);
    if(invert)
      wt_val = wt_range.max - gaus * wt_range.Range();
    else
      wt_val = wt_range.min + gaus * wt_range.Range();
  }
  if(set_scale) {
    SetCnWtRnd(cg, cg_idx, net, thr_no);
    SetCnScale(wt_val, cg, cg_idx, net, thr_no);
  }
  else {
    SetCnWt(wt_val, cg, cg_idx, net, thr_no);
  }
}

///////////////////////////////////////////////
//      0       1       2       3    recv
//      0       .33     .66     1    rgp_x
//
//      0       1       2       3    send
//      0       .33     .66     1    sgp_x
//      4       5       6       7    wrp_x > .5  int
//      1.33    1.66    2       2.33 wrp_x > .5  flt
//      -4      -3      -2      -1   wrp_x < .5  int
//      -1.33   -1      -.66    -.33 wrp_x < .5  flt

void GradientWtsPrjnSpec::InitWeights_RecvGps(Projection* prjn, ConGroup* cg, Unit* ru,
                                              Network* net, int thr_no) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  Unit* lru = (Unit*)ru;
  int rgpidx = lru->UnitGpIdx();
  taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position relative to overall gp geom
  float rgp_x = (float)rgp_pos.x / (float)MAX(recv_lay->gp_geom.x-1, 1);
  float rgp_y = (float)rgp_pos.y / (float)MAX(recv_lay->gp_geom.y-1, 1);

  float max_dist = 1.0f;
  if(grad_x && grad_y)
    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX(send_lay->flat_geom.x-1, 1);
  float mxs_y = (float)MAX(send_lay->flat_geom.y-1, 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i,net);
    taVector2i su_pos;
    send_lay->UnitLogPos(su, su_pos);
    float su_x = (float)su_pos.x / mxs_x;
    float su_y = (float)su_pos.y / mxs_y;

    float wrp_x, wrp_y;
    if(wrap) {
      if(rgp_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
      else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
      if(rgp_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
      else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
    }

    float dist = 0.0f;
    if(grad_x && grad_y) {
      dist = taMath_float::euc_dist(su_x, su_y, rgp_x, rgp_y);
      if(wrap) {
        float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_x, rgp_y);
        if(wrp_dist < dist) {
          dist = wrp_dist;
          wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_x, rgp_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
        else {
          wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_x, rgp_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
      }
    }
    else if(grad_x) {
      dist = fabsf(su_x - rgp_x);
      if(wrap) {
        float wrp_dist = fabsf(wrp_x - rgp_x);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    else if(grad_y) {
      dist = fabsf(su_y - rgp_y);
      if(wrap) {
        float wrp_dist = fabsf(wrp_y - rgp_y);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, cg, ru, net, dist, i, thr_no);
  }
}

void GradientWtsPrjnSpec::InitWeights_RecvFlat(Projection* prjn, ConGroup* cg, Unit* ru,
                                               Network* net, int thr_no) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  taVector2i ru_pos;
  recv_lay->UnitLogPos(ru, ru_pos);
  float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1);
  float ru_y = (float)ru_pos.y / (float)MAX(recv_lay->flat_geom.y-1, 1);

  float max_dist = 1.0f;
  if(grad_x && grad_y)
    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX(send_lay->flat_geom.x-1, 1);
  float mxs_y = (float)MAX(send_lay->flat_geom.y-1, 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i,net);
    taVector2i su_pos;
    send_lay->UnitLogPos(su, su_pos);
    float su_x = (float)su_pos.x / mxs_x;
    float su_y = (float)su_pos.y / mxs_y;

    float wrp_x, wrp_y;
    if(wrap) {
      if(ru_x > .5f)    wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
      else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
      if(ru_y > .5f)    wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
      else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
    }

    float dist = 0.0f;
    if(grad_x && grad_y) {
      dist = taMath_float::euc_dist(su_x, su_y, ru_x, ru_y);
      if(wrap) {
        float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_x, ru_y);
        if(wrp_dist < dist) {
          dist = wrp_dist;
          wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_x, ru_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
        else {
          wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_x, ru_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
      }
    }
    else if(grad_x) {
      dist = fabsf(su_x - ru_x);
      if(wrap) {
        float wrp_dist = fabsf(wrp_x - ru_x);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    else if(grad_y) {
      dist = fabsf(su_y - ru_y);
      if(wrap) {
        float wrp_dist = fabsf(wrp_y - ru_y);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, cg, ru, net, dist, i, thr_no);
  }
}



