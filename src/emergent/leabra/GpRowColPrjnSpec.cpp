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

#include "GpRowColPrjnSpec.h"

TA_BASEFUNS_CTORS_DEFN(GpRowColPrjnSpec);

void GpRowColPrjnSpec::Initialize() {
  recv_row_col= ROWS;
  recv_start = 0;
  recv_end = -1;
  send_row_col= ROWS;
  send_start = 0;
  send_end = -1;
}

void GpRowColPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) override {
  if(!(bool)prjn->from) return;
  if(TestWarning(recv_row_col != ALL && !prjn->layer->unit_groups, "Connect_impl",
                 "ROW or COL requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(send_row_col != ALL && !prjn->from->unit_groups, "Connect_impl",
                 "ROW or COL requires send layer to have unit groups!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->gp_geom;

  int recv_st = recv_start;  int recv_ed = recv_end;
  int recv_oth_n = 0;  int recv_max = 0;
  if(recv_row_col == ROWS) {
    recv_oth_n = ru_geo.x;
    recv_max = ru_geo.y;
  }
  else if(recv_row_col == COLS) {
    recv_oth_n = ru_geo.y;
    recv_max = ru_geo.x;
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
    
  int send_st = send_start;  int send_ed = send_end;  
  int send_oth_n = 0;  int send_max = 0;
  if(send_row_col == ROWS) {
    send_oth_n = ru_geo.x;
    send_max = ru_geo.y;
  }
  else if(send_row_col == COLS) {
    send_oth_n = ru_geo.y;
    send_max = ru_geo.x;
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

  

  if(row_col == ROWS) {
    if(!make_cons) {            // alloc
      AllocGp_Recv(prjn, Layer::ACC_LAY, 0, Layer::ACC_GP, n_tot);
      for(int rw = st; rw <= ed; rw++) {
        for(int cl=0; cl < n_cols; cl++) {
          AllocGp_Send(prjn, Layer::ACC_GP, rw * su_geo.x + cl, Layer::ACC_LAY, 1);
        }
      }
    }
    else {
      for(int rw = st; rw <= ed; rw++) {
        for(int cl=0; cl < n_cols; cl++) {
          Connect_Gp(prjn, Layer::ACC_LAY, 0, Layer::ACC_GP, rw * su_geo.x + cl, make_cons,
                     -1, n_tot);
        }
      }
    }
  }
  else {
    if(!make_cons) {            // alloc
      AllocGp_Recv(prjn, Layer::ACC_LAY, 0, Layer::ACC_GP, n_tot);
      for(int rw = 0; rw < n_rows; rw++) {
        for(int cl=st; cl <= ed; cl++) {
          AllocGp_Send(prjn, Layer::ACC_GP, rw * su_geo.x + cl, Layer::ACC_LAY, 1);
        }
      }
    }
    else {
      for(int rw = 0; rw < n_rows; rw++) {
        for(int cl=st; cl <= ed; cl++) {
          Connect_Gp(prjn, Layer::ACC_LAY, 0, Layer::ACC_GP, rw * su_geo.x + cl,
                     make_cons);
        }
      }
    }
  }
}
