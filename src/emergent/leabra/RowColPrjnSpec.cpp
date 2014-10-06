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

#include "RowColPrjnSpec.h"

#include <Network>

TA_BASEFUNS_CTORS_DEFN(RowColPrjnSpec);

void RowColPrjnSpec::Initialize() {
  recv_row_col= ROWS;
  recv_gp = false;
  recv_start = 0;
  recv_end = -1;
  send_row_col= ROWS;
  send_gp = false;
  send_start = 0;
  send_end = -1;
  // one_to_one = false;
}

void RowColPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) override {
  if(!(bool)prjn->from) return;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i recv_geo;
  taVector2i send_geo;

  if(recv_row_col == ALL || !recv_gp) {
    recv_geo = recv_lay->flat_geom;
  }
  else {
    recv_geo = recv_lay->gp_geom;
  }
  if(send_row_col == ALL || !send_gp) {
    send_geo = send_lay->flat_geom;
  }
  else {
    send_geo = send_lay->gp_geom;
  }

  int recv_st = recv_start;  int recv_ed = recv_end;
  int recv_oth_n = 0;  int recv_max = 0;
  if(recv_row_col == ALL) {
    recv_oth_n = recv_geo.x;      // go by rows
    recv_max = recv_geo.y;
    recv_st = 0;                // all
    recv_ed = -1;
  }
  else if(recv_row_col == ROWS) {
    recv_oth_n = recv_geo.x;
    recv_max = recv_geo.y;
  }
  else if(recv_row_col == COLS) {
    recv_oth_n = recv_geo.y;
    recv_max = recv_geo.x;
  }
  if(recv_st < 0) recv_st += recv_max;
  if(recv_ed < 0) recv_ed += recv_max;
  if(recv_row_col != ALL) {
    if(TestWarning(recv_st > recv_ed, "Connect_impl", "start is greater than end!")) {
      return;
    }
    if(TestWarning(recv_st < 0 || recv_st >= recv_max, "Connect_impl",
                   "recv start is < 0 or greater than max number of groups in layer!")) {
      return;
    }
    if(TestWarning(recv_ed < 0 || recv_ed >= recv_max, "Connect_impl",
                   "recv end is < 0 or greater than max number of groups in layer!")) {
      return;
    }
  }
  int recv_n = 1+recv_ed-recv_st;
  int recv_n_tot = recv_n * recv_oth_n;
  int recv_n_tot_u = recv_n_tot;
  if(recv_gp)
    recv_n_tot_u *= recv_lay->UnitAccess_NUnits(Layer::ACC_GP);
    
  int send_st = send_start;  int send_ed = send_end;  
  int send_oth_n = 0;  int send_max = 0;
  if(send_row_col == ALL) {
    send_oth_n = send_geo.x;      // go by rows
    send_max = send_geo.y;
    send_st = 0;                // all
    send_ed = -1;
  }
  else if(send_row_col == ROWS) {
    send_oth_n = send_geo.x;
    send_max = send_geo.y;
  }
  else if(send_row_col == COLS) {
    send_oth_n = send_geo.y;
    send_max = send_geo.x;
  }
  if(send_st < 0) send_st += send_max;
  if(send_ed < 0) send_ed += send_max;
  if(send_row_col != ALL) {
    if(TestWarning(send_st > send_ed, "Connect_impl", "start is greater than end!")) {
      return;
    }
    if(TestWarning(send_st < 0 || send_st >= send_max, "Connect_impl",
                   "send start is < 0 or greater than max number of groups in layer!")) {
      return;
    }
    if(TestWarning(send_ed < 0 || send_ed >= send_max, "Connect_impl",
                   "send end is < 0 or greater than max number of groups in layer!")) {
      return;
    }
  }
  int send_n = 1+send_ed-send_st;
  int send_n_tot = send_n * send_oth_n;
  int send_n_tot_u = send_n_tot;
  if(send_gp)
    send_n_tot_u *= send_lay->UnitAccess_NUnits(Layer::ACC_GP);

  // if(one_to_one) {
  // }
  // else {                                         // all to all
  for(int rrc = recv_st; rrc <= recv_ed; rrc++) { // relevant coord
    for(int roc=0; roc < recv_oth_n; roc++) {     // other coord
      int rgpidx = -1;
      int rnu = 1;
      Unit* ru = NULL;
      if(recv_row_col != ALL && recv_gp) {
        if(recv_row_col == COLS)
          rgpidx = roc * recv_oth_n + rrc;
        else
          rgpidx = rrc * recv_max + roc;
        rnu = recv_lay->UnitAccess_NUnits(Layer::ACC_GP);
      }
      else {
        if(recv_row_col == COLS)
          ru = recv_lay->UnitAtCoord(rrc, roc);
        else
          ru = recv_lay->UnitAtCoord(roc, rrc);
      }
      for(int i=0;i<rnu;i++) {
        if(rgpidx >= 0)
          ru = recv_lay->UnitAccess(Layer::ACC_GP, i, rgpidx);
        if(!ru) continue;

        if(!make_cons) {
          ru->RecvConsPreAlloc(send_n_tot_u, prjn);
        }

        // now for the sender!
        for(int src = send_st; src <= send_ed; src++) { // relevant coord
          for(int soc=0; soc < send_oth_n; soc++) {     // other coord
            int sgpidx = -1;
            int snu = 1;
            Unit* su = NULL;
            if(send_row_col != ALL && send_gp) {
              if(send_row_col == COLS)
                sgpidx = soc * send_oth_n + src;
              else
                sgpidx = src * send_max + soc;
              snu = send_lay->UnitAccess_NUnits(Layer::ACC_GP);
            }
            else {
              if(send_row_col == COLS)
                su = send_lay->UnitAtCoord(src, soc);
              else
                su = send_lay->UnitAtCoord(soc, src);
            }
            for(int j=0;j<snu;j++) {
              if(sgpidx >= 0)
                su = send_lay->UnitAccess(Layer::ACC_GP, j, sgpidx);
              if(!su) continue;
              if(self_con || (ru != su))
                ru->ConnectFrom(su, prjn, !make_cons);
            }
          }
        }
      }
    }
  }
  // }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    prjn->from->SendConsPostAlloc(prjn);
  }
}
