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

#include "FullPrjnSpec.h"
#include <Network>
#include <int_Array>

void FullPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  int recv_no = prjn->from->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    recv_no--;

  int send_no = prjn->layer->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    send_no--;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
    FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
      if(self_con || (ru != su))
        ru->ConnectFrom(su, prjn);
    }
  }
}

int FullPrjnSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
  if(!(bool)prjn->from) return 0;

  int rval = 0;

  int no = prjn->from->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    no--;

  int n_new_cons = (int)(p_add_con * (float)no);
  if(n_new_cons <= 0) return 0;
  int_Array new_idxs;
  new_idxs.SetSize(no);
  new_idxs.FillSeq();
  FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
    new_idxs.Permute();
    for(int i=0;i<n_new_cons;i++) {
      Unit* su = (Unit*)prjn->from->units.Leaf(new_idxs[i]);
      Connection* cn = ru->ConnectFromCk(su, prjn); // check means that it won't add any new connections if already there!
      if(cn) {
        cn->wt = init_wt;
        rval++;
      }
    }
  }
  return rval;
}
