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

#include "GpOneToOnePrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(GpOneToOnePrjnSpec);

void GpOneToOnePrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
  if(!(bool)prjn->from) return;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  // special case where we map units to groups, only if no offsets..
  if(send_lay->unit_groups && recv_lay->units.leaves == send_lay->gp_geom.n) {
    Connect_RecvUnitsSendGps(prjn, make_cons);
    return;
  }
  else if(recv_lay->unit_groups && send_lay->units.leaves == recv_lay->gp_geom.n) {
    Connect_SendUnitsRecvGps(prjn, make_cons);
    return;
  }

  // revert to main group if no sub groups
  Layer::AccessMode racc_md = Layer::ACC_GP;
  if(!recv_lay->unit_groups) racc_md = Layer::ACC_LAY;
  Layer::AccessMode sacc_md = Layer::ACC_GP;
  if(!send_lay->unit_groups) sacc_md = Layer::ACC_LAY;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);

  int r_st = recv_start;
  int s_st = send_start;

  int max_n = n_conns;
  if(recv_lay->unit_groups) {
    if(n_conns < 0) 
      max_n = recv_lay->gp_geom.n - recv_start;
    max_n = MIN(recv_lay->gp_geom.n - recv_start, max_n);
  }
  else {
    max_n = 1; 
    r_st = 0;
  }
  if(send_lay->unit_groups) {
    max_n = MIN(send_lay->gp_geom.n - send_start, max_n);
  }
  else {
    max_n = 1;
    s_st = 0;
  }
  max_n = MAX(1, max_n);        // lower limit of 1

  for(int i=0; i<max_n; i++) {  // loop over group index
    int rgpidx = i + r_st;
    int sgpidx = i + s_st;

    if(!make_cons) {
      // pre-allocate connections
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
        ru->RecvConsPreAlloc(su_nunits, prjn);
      }
      for(int sui=0; sui < su_nunits; sui++) {
        Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
        su->SendConsPreAlloc(ru_nunits, prjn);
      }
    }
    else {
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
        for(int sui=0; sui < su_nunits; sui++) {
          Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
          if(self_con || (ru != su))
            ru->ConnectFrom(su, prjn);
        }
      }
    }
  }
}

void GpOneToOnePrjnSpec::Connect_RecvUnitsSendGps(Projection* prjn, bool make_cons) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  Layer::AccessMode racc_md = Layer::ACC_LAY;
  Layer::AccessMode sacc_md = Layer::ACC_GP;

  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);

  int max_n = n_conns;
  if(max_n < 0)
    max_n = send_lay->gp_geom.n - send_start;
  max_n = MIN(recv_lay->units.leaves - recv_start, max_n);
  max_n = MAX(1, max_n);        // lower limit of 1

  for(int i=0; i<max_n; i++) {  // loop over group index
    int runidx = i + recv_start;
    int sgpidx = i + send_start;

    taVector2i sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx);

    if(!make_cons) {
      // pre-allocate connections
      Unit* ru = recv_lay->UnitAtCoord(sgp_pos);
      ru->RecvConsPreAlloc(su_nunits, prjn);

      for(int sui=0; sui < su_nunits; sui++) {
        Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
        su->SendConsPreAlloc(1, prjn);
      }
    }
    else {
      Unit* ru = recv_lay->UnitAtCoord(sgp_pos);
      for(int sui=0; sui < su_nunits; sui++) {
        Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
        if(self_con || (ru != su))
          ru->ConnectFrom(su, prjn);
      }
    }
  }
}

void GpOneToOnePrjnSpec::Connect_SendUnitsRecvGps(Projection* prjn, bool make_cons) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  Layer::AccessMode racc_md = Layer::ACC_GP;
  Layer::AccessMode sacc_md = Layer::ACC_LAY;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);

  int max_n = n_conns;
  if(max_n < 0)
    max_n = recv_lay->gp_geom.n - recv_start;
  max_n = MIN(send_lay->units.leaves - send_start, max_n);
  max_n = MAX(1, max_n);        // lower limit of 1

  for(int i=0; i<max_n; i++) {  // loop over group index
    int rgpidx = i + recv_start;
    int sunidx = i + send_start;

    taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx);

    if(!make_cons) {
      // pre-allocate connections
      Unit* su = send_lay->UnitAtCoord(rgp_pos);
      if(!su) continue;
      su->SendConsPreAlloc(ru_nunits, prjn);

      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
        if(!ru) continue;
        ru->RecvConsPreAlloc(1, prjn);
      }
    }
    else {
      Unit* su = send_lay->UnitAtCoord(rgp_pos);
      if(!su) continue;
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
        if(ru && (self_con || (ru != su)))
          ru->ConnectFrom(su, prjn);
      }
    }
  }
}
