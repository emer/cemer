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

#include "OneToOnePrjnSpec.h"
#include <Network>

void OneToOnePrjnSpec::Initialize() {
  n_conns = -1;
  recv_start = 0;
  send_start = 0;
  use_gp = false;
  SetUnique("self_con", true);
  self_con = true;              // doesn't make sense to not do self con!
}

void OneToOnePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  int n_recv = prjn->layer->units.leaves - recv_start;
  int n_send = prjn->from->units.leaves - send_start;

  if(n_recv == 0 || n_send == 0) return;

  if(use_gp) {
    if(prjn->layer->unit_groups && prjn->layer->un_geom.n >= n_send) {
      ConnectRecvGp_impl(prjn);
      return;
    }
    else if(prjn->from->unit_groups && prjn->from->un_geom.n >= n_recv) {
      ConnectSendGp_impl(prjn);
      return;
    }
  }

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = n_recv;
  max_n = MIN(n_recv, max_n);
  max_n = MIN(n_send, max_n);
  for(int i=0; i<max_n; i++) {
    Unit* ru = (Unit*)prjn->layer->units.Leaf(recv_start + i);
    Unit* su = (Unit*)prjn->from->units.Leaf(send_start + i);
    if(self_con || (ru != su)) {
      ru->RecvConsPreAlloc(1, prjn);
      su->SendConsPreAlloc(1, prjn);
      ru->ConnectFrom(su, prjn);
    }
  }
}

void OneToOnePrjnSpec::ConnectRecvGp_impl(Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int n_send = send_lay->units.leaves - send_start;
  int n_recv = recv_lay->un_geom.n - recv_start;

  Layer::AccessMode acc_md = Layer::ACC_GP;

  int n_gps = recv_lay->gp_geom.n;

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = n_recv;
  max_n = MIN(n_recv, max_n);
  max_n = MIN(n_send, max_n);
  for(int gi=0; gi < n_gps; gi++) {
    for(int i=0; i<max_n; i++) {
      Unit* ru = recv_lay->UnitAccess(acc_md, recv_start + i, gi);
      Unit* su = (Unit*)prjn->from->units.Leaf(send_start + i);
      if(self_con || (ru != su)) {
        ru->RecvConsPreAlloc(1, prjn);
        if(gi == 0)
          su->SendConsPreAlloc(n_gps, prjn);
        ru->ConnectFrom(su, prjn);
      }
    }
  }
}

void OneToOnePrjnSpec::ConnectSendGp_impl(Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int n_recv = recv_lay->units.leaves - recv_start;
  int n_send = send_lay->un_geom.n - send_start;

  Layer::AccessMode acc_md = Layer::ACC_GP;

  int n_gps = send_lay->gp_geom.n;

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = n_recv;
  max_n = MIN(n_recv, max_n);
  max_n = MIN(n_send, max_n);
  for(int gi=0; gi < n_gps; gi++) {
    for(int i=0; i<max_n; i++) {
      Unit* ru = (Unit*)prjn->layer->units.Leaf(recv_start + i);
      Unit* su = send_lay->UnitAccess(acc_md, send_start + i, gi);
      if(self_con || (ru != su)) {
        if(gi == 0)
          ru->RecvConsPreAlloc(n_gps, prjn);
        su->SendConsPreAlloc(1, prjn);
        ru->ConnectFrom(su, prjn);
      }
    }
  }
}

