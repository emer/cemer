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

#include "Projection_Group.h"
#include <Network>


#include <SigLinkSignal>

Projection* Projection_Group::ConnectFrom(Layer* lay) {
  if(!lay) return NULL;
  Projection* prjn = (Projection*)NewEl(1);
  prjn->SetCustomFrom(lay);
  return prjn;
}

void Projection_Group::SigEmit(int sls, void* op1, void* op2) {
  inherited::SigEmit(sls, op1, op2);
  if(send_prjns) return;
  if (sls == SLS_LIST_ITEM_INSERT) {
    Network* net = GET_MY_OWNER(Network);
    if (net)
      net->RebuildAllViews();
  }
}

