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

#include "ScalarValSelfPrjnSpec.h"
#include <LeabraNetwork>

void ScalarValSelfPrjnSpec::Initialize() {
  init_wts = true;
  width = 3;
  wt_width = 2.0f;
  wt_max = 1.0f;
}

void ScalarValSelfPrjnSpec::Connect_UnitGroup(Layer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              Projection* prjn) {
//   float neigh1 = 1.0f / wt_width;
//   float val1 = expf(-(neigh1 * neigh1));
//  float scale_val = wt_max / val1;

  int n_cons = 2*width + 1;
  int nunits = lay->UnitAccess_NUnits(acc_md);

  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(int i=0;i<nunits;i++) {
      LeabraUnit* ru = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);

      if(!alloc_loop)
        ru->RecvConsPreAlloc(n_cons, prjn);

      int j;
      for(j=-width;j<=width;j++) {
        int sidx = i+j;
        if((sidx < 0) || (sidx >= nunits)) continue;
        LeabraUnit* su = (LeabraUnit*)lay->UnitAccess(acc_md, sidx, gpidx);
        if(!self_con && (ru == su)) continue;
        ru->ConnectFromCk(su, prjn);
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void ScalarValSelfPrjnSpec::Connect_impl(Projection* prjn) {
  if(!prjn->from)       return;
  if(TestError(prjn->from.ptr() != prjn->layer, "Connect_impl", "must be used as a self-projection!")) {
    return;
  }

  Layer* lay = prjn->layer;
  UNIT_GP_ITR(lay, Connect_UnitGroup(lay, acc_md, gpidx, prjn); );
}

void ScalarValSelfPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  Network* net = prjn->layer->own_net;
  float neigh1 = 1.0f / wt_width;
  float val1 = expf(-(neigh1 * neigh1));
  float scale_val = wt_max / val1;

  int ru_idx = ru->idx;         // index within owning group

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i,net);
    int su_idx = su->idx;
    float dist = (float)(ru_idx - su_idx) / wt_width;
    float wtval = scale_val * expf(-(dist * dist));
    cg->PtrCn(i,BaseCons::WT,net) = wtval;
  }
}

