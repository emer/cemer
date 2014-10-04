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

#include "ToGpRowColPrjnSpec.h"

TA_BASEFUNS_CTORS_DEFN(ToGpRowColPrjnSpec);

void ToGpRowColPrjnSpec::Initialize() {
  row_col= ROWS;
  start = 0;
  end = 0;
}

void ToGpRowColPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) override {
  if(!(bool)prjn->from) return;
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires receiving layer to have unit groups!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->gp_geom;

  if(row_col == ROWS) {
    int n_cols = ru_geo.x;
    int st = start;
    int ed = end;
    if(st < 0) st = ru_geo.y+st;
    if(ed < 0) ed = ru_geo.y+ed;
    if(TestWarning(st > ed, "Connect_impl", "start is greater than end!")) {
      return;
    }
    if(TestWarning(st < 0 || st >= ru_geo.y, "Connect_impl",
                   "start is < 0 or greater than number of rows!")) {
      return;
    }
    if(TestWarning(ed < 0 || ed >= ru_geo.y, "Connect_impl",
                   "end is < 0 or greater than number of rows!")) {
      return;
    }
    int n_rows = 1+ed-st;
    int n_tot = n_cols * n_rows;
    if(!make_cons) {            // alloc
      AllocGp_Send(prjn, Layer::ACC_LAY, 0, Layer::ACC_GP, n_tot);
      for(int rw = st; rw <= ed; rw++) {
        for(int cl=0; cl < n_cols; cl++) {
          AllocGp_Recv(prjn, Layer::ACC_GP, rw * ru_geo.x + cl, Layer::ACC_LAY, 1);
        }
      }
    }
    else {
      for(int rw = st; rw <= ed; rw++) {
        for(int cl=0; cl < n_cols; cl++) {
          Connect_Gp(prjn, Layer::ACC_GP, rw * ru_geo.x + cl, Layer::ACC_LAY, 0,
                     make_cons);
        }
      }
    }
  }
  else {
    int n_rows = ru_geo.y;
    int st = start;
    int ed = end;
    if(st < 0) st = ru_geo.x+st;
    if(ed < 0) ed = ru_geo.x+ed;
    if(TestWarning(st > ed, "Connect_impl", "start is greater than end!")) {
      return;
    }
    if(TestWarning(st < 0 || st >= ru_geo.x, "Connect_impl",
                   "start is < 0 or greater than number of cols!")) {
      return;
    }
    if(TestWarning(ed < 0 || ed >= ru_geo.x, "Connect_impl",
                   "end is < 0 or greater than number of cols!")) {
      return;
    }
    int n_cols = 1+ed-st;
    int n_tot = n_cols * n_rows;
    if(!make_cons) {            // alloc
      AllocGp_Send(prjn, Layer::ACC_LAY, 0, Layer::ACC_GP, n_tot);
      for(int rw = 0; rw < n_rows; rw++) {
        for(int cl=st; cl <= ed; cl++) {
          AllocGp_Recv(prjn, Layer::ACC_GP, rw * ru_geo.x + cl, Layer::ACC_LAY, 1);
        }
      }
    }
    else {
      for(int rw = 0; rw < n_rows; rw++) {
        for(int cl=st; cl <= ed; cl++) {
          Connect_Gp(prjn, Layer::ACC_GP, rw * ru_geo.x + cl, Layer::ACC_LAY, 0,
                     make_cons);
        }
      }
    }
  }
}
