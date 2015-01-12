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

#include "Deep5bConSpec.h"

TA_BASEFUNS_CTORS_DEFN(Deep5bStdSync);
TA_BASEFUNS_CTORS_DEFN(Deep5bConSpec);

void Deep5bStdSync::Initialize() {
  on = false;
  sync_tau = 100;
  sync_dt = 1.0f / sync_tau;
}

void Deep5bStdSync::Defaults_init() {
}

void Deep5bStdSync::UpdateAfterEdit_impl() {
  sync_dt = 1.0f / sync_tau;
}


void Deep5bConSpec::Initialize() {
  SetUnique("learn", true);     // generally doesn't learn..
  learn = false;
}

void Deep5bConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Deep5b_" + nm;
}

void Deep5bConSpec::Trial_Init_Specs(LeabraNetwork* net) {
  inherited::Trial_Init_Specs(net);
  net->net_misc.deep5b_cons = true;
}

void Deep5bConSpec::Init_Weights_sym_s(ConGroup* cg, Network* net, int thr_no) {
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

void Deep5bConSpec::Compute_EpochWeights(LeabraConGroup* cg, LeabraNetwork* net,
                                         int thr_no) {
  if(!std_sync.on) return;
  Unit* su = cg->ThrOwnUn(net, thr_no);
  Layer* sl = su->own_lay();
  Projection* oprjn = sl->send_prjns.FindPrjnTo(cg->prjn->layer); // find prjn to other layer -- this assumes that deep prjn always comes *after* the other one..
  if(!oprjn || oprjn == cg->prjn) return;
  ConGroup* ocg = su->SendConGroupPrjn(oprjn);
  if(!ocg) return;
  int mx = MIN(cg->size, ocg->size);
  for(int i=0; i<mx; i++) {     // assume to be in 1-to-1 corresp..
    float& fwt = cg->OwnCn(i, FWT);
    float  ofw = ocg->OwnCn(i, FWT);
    fwt += std_sync.sync_dt * (ofw - fwt);
    // cg->OwnCn(i, SWT) = fwt;    // todo: slow.. don't do this!
    cg->OwnCn(i, WT) = SigFmLinWt(fwt);
  }
}
