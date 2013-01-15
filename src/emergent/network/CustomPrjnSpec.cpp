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

#include "CustomPrjnSpec.h"
#include <Network>

void CustomPrjnSpec::Connect(Projection* prjn) {
  // make sure i have the correct indicies for my con_groups..
  prjn->recv_idx = -1;
  prjn->send_idx = -1;
  FOREACH_ELEM_IN_GROUP(Unit, u, prjn->layer->units) {
    int idx = u->recv.FindPrjnIdx(prjn);
    if(idx >= 0) {
      prjn->recv_idx = idx;
      break;
    }
  }
  FOREACH_ELEM_IN_GROUP(Unit, u, prjn->from->units) {
    int idx = u->send.FindPrjnIdx(prjn);
    if(idx >= 0) {
      prjn->send_idx = idx;
      break;
    }
  }

  prjn->projected = true;       // don't do anything else..
}

