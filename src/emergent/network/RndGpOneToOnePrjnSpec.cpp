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

#include "RndGpOneToOnePrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(RndGpOneToOnePrjnSpec);


void RndGpOneToOnePrjnSpec::Initialize() {
  p_con = .25;
  same_seed = false;
  rndm_seed.GetCurrent();
}

void RndGpOneToOnePrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

void RndGpOneToOnePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(same_seed)
    rndm_seed.OldSeed();

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->gp_geom;

  // revert to main group if no sub groups
  Layer::AccessMode racc_md = Layer::ACC_GP;
  if(!recv_lay->unit_groups) racc_md = Layer::ACC_LAY;
  Layer::AccessMode sacc_md = Layer::ACC_GP;
  if(!send_lay->unit_groups) sacc_md = Layer::ACC_LAY;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = recv_lay->gp_geom.n - recv_start;
  max_n = MIN(recv_lay->gp_geom.n - recv_start, max_n);
  max_n = MIN(send_lay->gp_geom.n - send_start, max_n);
  max_n = MAX(1, max_n);        // lower limit of 1
  for(int i=0; i<max_n; i++) {
    int rgpidx = i + recv_start;
    int sgpidx = i + send_start;

    int recv_no = (int) ((p_con * (float)su_nunits) + .5f);
    if(!self_con && (send_lay == recv_lay && rgpidx == sgpidx))
      recv_no--;
    if(recv_no <= 0)
      recv_no = 1;

    // sending number is even distribution across senders plus some imbalance factor
    float send_no_flt = (float)(ru_nunits * recv_no) / (float)su_nunits;
    // add SEM as corrective factor
    float send_sem = send_no_flt / sqrtf(send_no_flt);
    int send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
    if(send_no > ru_nunits) send_no = ru_nunits;

    // pre-allocate connections
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
      ru->RecvConsPreAlloc(recv_no, prjn);
    }
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
      su->SendConsPreAlloc(send_no, prjn);
    }

    UnitPtrList perm_list;      // permution list
    for(int rui=0; rui < ru_nunits; rui++) {
      perm_list.Reset();
      Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
      for(int sui=0; sui < su_nunits; sui++) {
        Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
        perm_list.Link(su);
      }
      perm_list.Permute();
      for(int j=0; j<recv_no; j++)
        ru->ConnectFrom((Unit*)perm_list[j], prjn);
    }
  }
}


