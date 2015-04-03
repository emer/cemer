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

#include "SendDeepNormConSpec.h"

TA_BASEFUNS_CTORS_DEFN(SendDeepNormConSpec);

void SendDeepNormConSpec::Initialize() {
  // SetUnique("learn", true);     // generally doesn't learn..
  // learn = false;
}

void SendDeepNormConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Deep_" + nm;
}

void SendDeepNormConSpec::Trial_Init_Specs(LeabraNetwork* net) {
  inherited::Trial_Init_Specs(net);
  // net->net_misc.deep5b_cons = true;
}

void SendDeepNormConSpec::Init_Weights_sym_s(ConGroup* cg, Network* net, int thr_no) {
  if(!wt_limits.sym) return;
  Unit* su = cg->ThrOwnUn(net, thr_no);
  Layer* sl = su->own_lay();
  Projection* oprjn = sl->send_prjns.FindPrjnTo(cg->prjn->layer); // find prjn to other layer -- this assumes that deep prjn always comes *after* the other one..
  if(!oprjn || oprjn == cg->prjn) return;
  ConGroup* ocg = su->SendConGroupPrjn(oprjn);
  if(!ocg) return;
  int mx = MIN(cg->size, ocg->size);
  for(int i=0; i<mx; i++) {     // assume to be in 1-to-1 corresp..
    cg->OwnCn(i, WT) = ocg->OwnCn(i, WT);
  }
}

