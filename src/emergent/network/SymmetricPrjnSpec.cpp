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

#include "SymmetricPrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(SymmetricPrjnSpec);

void SymmetricPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  int con_idx;

  FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
    int n_cons = 0;
    FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
      if(RecvCons::FindRecipRecvCon(con_idx, su, ru, prjn->layer))
        n_cons++;
    }
    ru->RecvConsPreAlloc(n_cons, prjn);
  }

  // todo: not 100% sure this is correct!!!
  FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
    int n_cons = 0;
    FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
      if(RecvCons::FindRecipSendCon(con_idx, ru, su, prjn->from))
        n_cons++;
    }
    su->SendConsPreAlloc(n_cons, prjn);
  }

  int cnt = 0;
  FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
    FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
      if(RecvCons::FindRecipRecvCon(con_idx, su, ru, prjn->layer))
        if(ru->ConnectFrom(su, prjn))
          cnt++;
    }
  }
  TestWarning(cnt == 0, "Connect_impl", "did not make any connections.",
              "Note that this layer must be *earlier* in list of layers than the one you are trying to symmetrize from.");
}



