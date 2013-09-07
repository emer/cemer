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

#include "TiledGpRFOneToOneWtsPrjnSpec.h"
#include <Network>


void TiledGpRFOneToOneWtsPrjnSpec::Initialize() {
  one_to_one_wt = 0.8f;
  other_wt = 0.2f;
  init_wts = true;
}

void TiledGpRFOneToOneWtsPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  Network* net = prjn->layer->own_net;

  int rgpidx;
  int rui;
  recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
  for(int i=0; i < cg->size; i++) {
    Unit* su = cg->Un(i,net);
    int sgpidx;
    int sui;
    send_lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
    if(sui == rui)
      cg->Cn(i,BaseCons::WT,net) = one_to_one_wt;
    else
      cg->Cn(i,BaseCons::WT,net) = other_wt;
  }
}


