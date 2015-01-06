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

#include "PFCPrjnSpec.h"

#include <Network>

TA_BASEFUNS_CTORS_DEFN(PFCPrjnSpec);

void PFCPrjnSpec::Initialize() {
  recv_layer = GATING;
  recv_pfc_rows = ALL_PFC;
  recv_gate_rows = ALL_GATING;

  send_layer = PFC;
  send_pfc_rows = ALL_PFC;
  send_gate_rows = ALL_GATING;

  row_1to1 = true;
  col_1to1 = true;
  unit_1to1 = false;
}

void PFCPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(recv_layer == OTHER)
    row_1to1 = false;
}
  
void PFCPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int recv_y;
  int recv_x;
  bool recv_gps = true;

  if(recv_lay->unit_groups && recv_lay->gp_geom.y > 1) {
    recv_y = recv_lay->gp_geom.y;
    recv_gps = true;
  }
  else {
    recv_y = recv_lay->flat_geom.y;
    recv_gps = false;
  }
  if(recv_lay->unit_groups && recv_lay->gp_geom.x > 1) {
    recv_x = recv_lay->gp_geom.x;
  }
  else {
    recv_x = recv_lay->flat_geom.x;
  }

  for(int ry=0; ry<recv_y; ry++) {
    switch(recv_layer) {
    case PFC:
      if(!(recv_pfc_rows & (1 << ry)))
        continue;
      break;
    case GATING:
      if(!(recv_gate_rows & (1 << ry)))
        continue;
      break;
    case OTHER:
      break;
    }

    for(int rx=0; rx<recv_x; rx++) {
      if(row_1to1) {
        Connect_row1to1(prjn, make_cons, rx, ry, recv_x, recv_gps);
      }
      else {
        Connect_rowall(prjn, make_cons, rx, ry, recv_x, recv_gps);
      }
    }
  }
  
  if(!make_cons) { // on first pass through alloc loop, do allocations
    recv_lay->RecvConsPostAlloc(prjn);
    send_lay->SendConsPostAlloc(prjn);
  }
}

void PFCPrjnSpec::Connect_row1to1(Projection* prjn, bool make_cons,
                                  int rx, int ry, int recv_x, bool recv_gps) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int send_y;
  int send_x;
  bool send_gps = true;

  if(send_lay->unit_groups && send_lay->gp_geom.y > 1) {
    send_y = send_lay->gp_geom.y;
    send_gps = true;
  }
  else {
    send_y = send_lay->flat_geom.y;
    send_gps = false;
  }
  if(send_lay->unit_groups && send_lay->gp_geom.x > 1) {
    send_x = send_lay->gp_geom.x;
  }
  else {
    send_x = send_lay->flat_geom.x;
  }

  int sy = ry;

  if(recv_layer == PFC && send_layer == GATING) {
    sy = sy / 2;                // gating is 1/2 PFC
  }
  else if(recv_layer == GATING && send_layer == PFC) {
    // need to do both 2 * sy and 2 * sy + 1
    Connect_cols(prjn, make_cons, rx, ry, recv_x, recv_gps, 2 * sy, send_x, send_gps);
    Connect_cols(prjn, make_cons, rx, ry, recv_x, recv_gps, 2 * sy + 1, send_x, send_gps);
    return;                     // don't do default..
  }
  // otherwise, sy = ry either way

  Connect_cols(prjn, make_cons, rx, ry, recv_x, recv_gps, sy, send_x, send_gps);
}

void PFCPrjnSpec::Connect_rowall(Projection* prjn, bool make_cons,
                                 int rx, int ry, int recv_x, bool recv_gps) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int send_y;
  int send_x;
  bool send_gps = true;

  if(send_lay->unit_groups && send_lay->gp_geom.y > 1) {
    send_y = send_lay->gp_geom.y;
    send_gps = true;
  }
  else {
    send_y = send_lay->flat_geom.y;
    send_gps = false;
  }
  if(send_lay->unit_groups && send_lay->gp_geom.x > 1) {
    send_x = send_lay->gp_geom.x;
  }
  else {
    send_x = send_lay->flat_geom.x;
  }

  for(int sy=0; sy<send_y; sy++) {
    Connect_cols(prjn, make_cons, rx, ry, recv_x, recv_gps, sy, send_x, send_gps);
  }
}

void PFCPrjnSpec::Connect_cols(Projection* prjn, bool make_cons,
                               int rx, int ry, int recv_x, bool recv_gps,
                               int sy, int send_x, bool send_gps) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  // send does the filtering!
  switch(send_layer) {
  case PFC:
    if(!(send_pfc_rows & (1 << sy)))
      return;
    break;
  case GATING:
    if(!(send_gate_rows & (1 << sy)))
      return;
    break;
  case OTHER:
    break;
  }

  if(unit_1to1) {
    Connect_unit1to1(prjn, make_cons, rx, ry, recv_x, recv_gps, sy, send_x, send_gps);
    return;
  }
  
  int rgpidx = -1;
  int rnu = 1;
  Unit* ru = NULL;

  if(col_1to1 && recv_x == send_x) {
    int sx = rx;
    if(recv_gps) {
      rgpidx = ry * recv_x + rx;
      rnu = recv_lay->UnitAccess_NUnits(Layer::ACC_GP);
    }
    else {
      ru = recv_lay->UnitAtCoord(rx, ry);
    }
    for(int ri=0;ri<rnu;ri++) {
       if(rgpidx >= 0)
        ru = recv_lay->UnitAccess(Layer::ACC_GP, ri, rgpidx);
      if(!ru) continue;

      int sgpidx = -1;
      int snu = 1;
      Unit* su = NULL;
      if(send_gps) {
        sgpidx = sy * send_x + sx;
        snu = send_lay->UnitAccess_NUnits(Layer::ACC_GP);
      }
      else {
        su = send_lay->UnitAtCoord(sx, sy);
      }
      for(int si=0;si<snu;si++) {
        if(sgpidx >= 0)
          su = send_lay->UnitAccess(Layer::ACC_GP, si, sgpidx);
        if(!su) continue;

        if(!make_cons) {
          su->SendConsAllocInc(1, prjn);
          ru->RecvConsAllocInc(1, prjn);
        }
        else {
          ru->ConnectFrom(su, prjn);
        }
      }
    }
  }
  else {                        // all to all cols
    for(int sx = 0; sx < send_x; sx++) {
      if(recv_gps) {
        rgpidx = ry * recv_x + rx;
        rnu = recv_lay->UnitAccess_NUnits(Layer::ACC_GP);
      }
      else {
        ru = recv_lay->UnitAtCoord(rx, ry);
      }
      for(int ri=0;ri<rnu;ri++) {
        if(rgpidx >= 0)
          ru = recv_lay->UnitAccess(Layer::ACC_GP, ri, rgpidx);
        if(!ru) continue;

        int sgpidx = -1;
        int snu = 1;
        Unit* su = NULL;
        if(send_gps) {
          sgpidx = sy * send_x + sx;
          snu = send_lay->UnitAccess_NUnits(Layer::ACC_GP);
        }
        else {
          su = send_lay->UnitAtCoord(sx, sy);
        }
        for(int si=0;si<snu;si++) {
          if(sgpidx >= 0)
            su = send_lay->UnitAccess(Layer::ACC_GP, si, sgpidx);
          if(!su) continue;

          if(!make_cons) {
            su->SendConsAllocInc(1, prjn);
            ru->RecvConsAllocInc(1, prjn);
          }
          else {
            ru->ConnectFrom(su, prjn);
          }
        }
      }
    }
  }
}

void PFCPrjnSpec::Connect_unit1to1(Projection* prjn, bool make_cons,
                                   int rx, int ry, int recv_x, bool recv_gps,
                                   int sy, int send_x, bool send_gps) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int rgpidx = -1;
  int rnu = 1;
  int snu = 1;
  Unit* ru = NULL;
  Unit* su = NULL;

  if(recv_gps && !send_gps) {
    rgpidx = ry * recv_x + rx;
    rnu = recv_lay->UnitAccess_NUnits(Layer::ACC_GP);
    snu = send_lay->UnitAccess_NUnits(Layer::ACC_LAY);
    int mx = MIN(rnu, snu);
    for(int ri=0;ri<mx;ri++) {
      ru = recv_lay->UnitAccess(Layer::ACC_GP, ri, rgpidx);
      if(!ru) continue;
      su = send_lay->UnitAccess(Layer::ACC_LAY, ri);
      if(!su) continue;

      if(!make_cons) {
        su->SendConsAllocInc(1, prjn);
        ru->RecvConsAllocInc(1, prjn);
      }
      else {
        ru->ConnectFrom(su, prjn);
      }
    }
  }
  else if(!recv_gps && send_gps) {
    rnu = recv_lay->UnitAccess_NUnits(Layer::ACC_LAY);
    snu = send_lay->UnitAccess_NUnits(Layer::ACC_GP);
    int mx = MIN(rnu, snu);
    for(int sx = 0; sx < send_x; sx++) {
      int sgpidx = sy * send_x + sx;
      for(int ri=0;ri<mx;ri++) {
        ru = recv_lay->UnitAccess(Layer::ACC_LAY, ri);
        if(!ru) continue;
        su = send_lay->UnitAccess(Layer::ACC_GP, ri, sgpidx);
        if(!su) continue;

        if(!make_cons) {
          su->SendConsAllocInc(1, prjn);
          ru->RecvConsAllocInc(1, prjn);
        }
        else {
          ru->ConnectFrom(su, prjn);
        }
      }
    }
  }
  else if(recv_gps && send_gps) {
    rgpidx = ry * recv_x + rx;
    rnu = recv_lay->UnitAccess_NUnits(Layer::ACC_GP);
    snu = send_lay->UnitAccess_NUnits(Layer::ACC_GP);
    int sx = rx;                // assumes col_1to1 -- only sensible option
    int sgpidx = sy * send_x + sx;
    int mx = MIN(rnu, snu);
    for(int ri=0;ri<mx;ri++) {
      ru = recv_lay->UnitAccess(Layer::ACC_GP, ri, rgpidx);
      if(!ru) continue;
      su = send_lay->UnitAccess(Layer::ACC_GP, ri, sgpidx);
      if(!su) continue;

      if(!make_cons) {
        su->SendConsAllocInc(1, prjn);
        ru->RecvConsAllocInc(1, prjn);
      }
      else {
        ru->ConnectFrom(su, prjn);
      }
    }
  }
  else if(!recv_gps && !send_gps) {
    rnu = recv_lay->UnitAccess_NUnits(Layer::ACC_LAY);
    snu = send_lay->UnitAccess_NUnits(Layer::ACC_LAY);
    int mx = MIN(rnu, snu);
    for(int ri=0;ri<mx;ri++) {
      ru = recv_lay->UnitAccess(Layer::ACC_LAY, ri);
      if(!ru) continue;
      su = send_lay->UnitAccess(Layer::ACC_LAY, ri);
      if(!su) continue;

      if(!make_cons) {
        su->SendConsAllocInc(1, prjn);
        ru->RecvConsAllocInc(1, prjn);
      }
      else {
        ru->ConnectFrom(su, prjn);
      }
    }
  }
}
