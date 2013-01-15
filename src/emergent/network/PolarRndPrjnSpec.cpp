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

#include "PolarRndPrjnSpec.h"
#include <Network>


void PolarRndPrjnSpec::Initialize() {
  p_con = .25;

  rnd_dist.type = Random::GAUSSIAN;
  rnd_dist.mean = 0.0f;
  rnd_dist.var = .25f;

  rnd_angle.type = Random::UNIFORM;
  rnd_angle.mean = 0.5f;
  rnd_angle.var = 0.5f;

  dist_type = XY_DIST_CENTER_NORM;
  wrap = false;
  max_retries = 1000;

  same_seed = false;
  rndm_seed.GetCurrent();
}

void PolarRndPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

float PolarRndPrjnSpec::UnitDist(UnitDistType typ, Projection* prjn,
                               const taVector2i& ru, const taVector2i& su)
{
  taVector2f half(.5f);
  PosVector2i ru_geom = prjn->layer->flat_geom;
  PosVector2i su_geom = prjn->from->flat_geom;
  switch(typ) {
  case XY_DIST:
    return ru.Dist(su);
  case XY_DIST_CENTER: {
    taVector2f rctr = ru_geom;   rctr *= half;
    taVector2f sctr = su_geom;    sctr *= half;
    taVector2i ruc = ru - (taVector2i)rctr;
    taVector2i suc = su - (taVector2i)sctr;
    return ruc.Dist(suc);
  }
  case XY_DIST_NORM: {
    taVector2f ruc = ru;    ruc /= (taVector2f)ru_geom;
    taVector2f suc = su;    suc /= (taVector2f)su_geom;
    return ruc.Dist(suc);
  }
  case XY_DIST_CENTER_NORM: {
    taVector2f rctr = ru_geom;   rctr *= half;
    taVector2f sctr = su_geom;    sctr *= half;
    taVector2f ruc = ((taVector2f)ru - rctr) / rctr;
    taVector2f suc = ((taVector2f)su - sctr) / sctr;
    return ruc.Dist(suc);
  }
  }
  return 0.0f;
}

Unit* PolarRndPrjnSpec::GetUnitFmOff(UnitDistType typ, bool wrap, Projection* prjn,
                                   const taVector2i& ru, const taVector2f& su_off)
{
  taVector2f half(.5f);
  PosVector2i ru_geom = prjn->layer->flat_geom;
  PosVector2i su_geom = prjn->from->flat_geom;
  taVector2i suc;                // actual su coordinates
  switch(typ) {
  case XY_DIST: {
    suc = su_off;
    suc += ru;
    break;
  }
  case XY_DIST_CENTER: {        // do everything relative to center
    taVector2f rctr = ru_geom;   rctr *= half;
    taVector2f sctr = su_geom;    sctr *= half;
    taVector2i ruc = ru - (taVector2i)rctr;
    suc = su_off;
    suc += ruc;                 // add the centerized coordinates
    suc += (taVector2i)sctr;     // then add the sending center back into it..
    break;
  }
  case XY_DIST_NORM: {
    taVector2f ruc = ru;    ruc /= (taVector2f)ru_geom;
    taVector2f suf = su_off + ruc; // su_off is in normalized coords, so normalize ru
    suf *= (taVector2f)su_geom;
    suc = suf;
    break;
  }
  case XY_DIST_CENTER_NORM: {
    taVector2f rctr = ru_geom;   rctr *= half;
    taVector2f sctr = su_geom;    sctr *= half;
    taVector2f ruc = ((taVector2f)ru - rctr) / rctr;
    taVector2f suf = su_off + ruc;
    suf *= sctr;    suf += sctr;
    suc = suf;
    break;
  }
  }
  if(suc.WrapClip(wrap, su_geom) && !wrap)
    return NULL;

  Unit* su_u = (Unit*)prjn->from->UnitAtCoord(suc);
  return su_u;
}


float PolarRndPrjnSpec::GetDistProb(Projection* prjn, Unit* ru, Unit* su) {
  if(rnd_dist.type == Random::UNIFORM)
    return p_con;
  float prob = p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, su->pos));
  if(wrap) {
    PosVector2i su_geom = prjn->from->flat_geom;
    taVector2i suc = su->pos;
    suc.x += su_geom.x; // wrap around in x
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc.y += su_geom.y; // wrap around in x & y
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc.x = su->pos.x;          // just y
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc = su->pos;
    suc.x -= su_geom.x; // wrap around in x
    prob -= p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc.y -= su_geom.y; // wrap around in y
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc.x = su->pos.x;          // just y
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
  }
  return prob;
}

// todo: could put in some sending limits, and do recvs in random order

void PolarRndPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(same_seed)
    rndm_seed.OldSeed();

  int recv_no;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    recv_no = (int) ((p_con * (float)(prjn->from->units.leaves-1)) + .5f);
  else
    recv_no = (int) ((p_con * (float)prjn->from->units.leaves) + .5f);
  if(recv_no <= 0) recv_no = 1;

  // sending number is even distribution across senders plus some imbalance factor
  float send_no_flt = (float)(prjn->layer->units.leaves * recv_no) / (float)prjn->from->units.leaves;
  // add SEM as corrective factor
  float send_sem = send_no_flt / sqrtf(send_no_flt);
  int send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f); // polar needs some extra insurance
  if(send_no > prjn->layer->units.leaves) send_no = prjn->layer->units.leaves;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  Unit* ru, *su;
  taLeafItr ru_itr;
  PosVector2i ru_geom = prjn->layer->flat_geom;
  taVector2i ru_pos;             // do this according to act_geom..
  int cnt = 0;
  for(ru = (Unit*)prjn->layer->units.FirstEl(ru_itr); ru;
      ru = (Unit*)prjn->layer->units.NextEl(ru_itr), cnt++) {
    ru_pos.y = cnt / ru_geom.x;
    ru_pos.x = cnt % ru_geom.x;
    RecvCons* recv_gp = NULL;
    taVector2f suc;
    int n_con = 0;
    int n_retry = 0;
    while((n_con < recv_no) && (n_retry < max_retries)) { // limit number of retries
      float dist = rnd_dist.Gen();              // just get random deviate from distribution
      float angle = 2.0 * 3.14159265 * rnd_angle.Gen(); // same for angle
      suc.x = dist * cos(angle);
      suc.y = dist * sin(angle);
      su = GetUnitFmOff(dist_type, wrap, prjn, ru_pos, suc);
      if((su == NULL) || (!self_con && (ru == su))) {
        n_retry++;
        continue;
      }
      if(ru->ConnectFromCk(su, prjn, recv_gp))
        n_con++;
      else {
        n_retry++;              // already connected, retry
        continue;
      }
    }
    TestWarning(n_con < recv_no, "Connect_impl",
                "target number of connections:",String(recv_no),
                "not made, only made:",String(n_con));
  }
}

void PolarRndPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  for(int i=0; i<cg->size; i++) {
    cg->Cn(i)->wt = GetDistProb(prjn, ru, cg->Un(i));
  }
}

