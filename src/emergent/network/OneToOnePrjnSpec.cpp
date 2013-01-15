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
  SetUnique("self_con", true);
  self_con = true;              // doesn't make sense to not do self con!
}

void OneToOnePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  int i;
  int max_n = n_conns;
  if(n_conns < 0)
    max_n = prjn->layer->units.leaves - recv_start;
  max_n = MIN(prjn->layer->units.leaves - recv_start, max_n);
  max_n = MIN(prjn->from->units.leaves - send_start, max_n);
  for(i=0; i<max_n; i++) {
    Unit* ru = (Unit*)prjn->layer->units.Leaf(recv_start + i);
    Unit* su = (Unit*)prjn->from->units.Leaf(send_start + i);
    if(self_con || (ru != su)) {
      ru->RecvConsPreAlloc(1, prjn);
      su->SendConsPreAlloc(1, prjn);
      ru->ConnectFrom(su, prjn);
    }
  }
}

