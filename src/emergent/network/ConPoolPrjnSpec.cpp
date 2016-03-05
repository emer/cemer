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

#include "ConPoolPrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(ConPoolPrjnSpec);

void ConPoolPrjnSpec::Initialize() {
  pool_size = 2;
  stride = 2;
}

void ConPoolPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
  if(!(bool)prjn->from) return;

  if(TestError(prjn->from.ptr() != prjn->layer, "Connect_impl",
               "ConPoolPrjnSpec only works for self projections!")) {
    return;
  }

  Layer* lay = prjn->layer;
  
  // todo: these are different for stride != pool_size
  int recv_no = pool_size;
  if(!self_con)
    recv_no--;

  if(!make_cons) {
    // pre-allocate connections!
    lay->RecvConsPreAlloc(recv_no, prjn);
    lay->SendConsPreAlloc(recv_no, prjn);
  }
  else {
    for(int i=0; i< lay->units.leaves; i+= stride) {
      for(int j=0; j< pool_size; j++) {
        Unit* ru = lay->units.Leaf(i + j);
        for(int k=0; k< pool_size; k++) {
          Unit* su = lay->units.Leaf(i + k);
          if(self_con || (ru != su))
            ru->ConnectFrom(su, prjn);
        }
      }
    }
  }
}

