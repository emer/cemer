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

#include "UniformRndPrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(UniformRndPrjnSpec);

void UniformRndPrjnSpec::Initialize() {
  p_con = .25;
  sym_self = true;
  same_seed = false;
  rndm_seed.GetCurrent();
}

void UniformRndPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

void UniformRndPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(same_seed)
    rndm_seed.OldSeed();

  Network* net = prjn->layer->own_net;

  int recv_no;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    recv_no = (int) ((p_con * (float)(prjn->from->units.leaves-1)) + .5f);
  else
    recv_no = (int) ((p_con * ((float)prjn->from->units.leaves)) + .5f);
  if(recv_no <= 0) recv_no = 1;

  // sending number is even distribution across senders plus some imbalance factor
  float send_no_flt = (float)(prjn->layer->units.leaves * recv_no) / (float)prjn->from->units.leaves;
  // add SEM as corrective factor
  float send_sem = send_no_flt / sqrtf(send_no_flt);
  int send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
  if(send_no > prjn->layer->units.leaves) send_no = prjn->layer->units.leaves;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  if((prjn->from.ptr() == prjn->layer) && sym_self) {
    Layer* lay = prjn->layer;
    // trick is to divide cons in half, choose recv, send at random
    // for 1/2 cons, then go through all units and make the symmetric cons..
    TestWarning(p_con > .95f, "Connect_impl", "there is usually less than complete connectivity for high values of p_con (>.95) in symmetric, self-connected layers using permute!");
    // pre-allocate connections!
    int first;
    if(!self_con)
      first = (int) (.5f * p_con * (float)(prjn->from->units.leaves-1));
    else
      first = (int) (.5f * p_con * (float)prjn->from->units.leaves);
    if(first <= 0) first = 1;

    UnitPtrList ru_list;                // receiver permution list
    UnitPtrList perm_list;      // sender permution list

    FOREACH_ELEM_IN_GROUP(Unit, ru, lay->units)   // need to permute recvs because of exclusion
      ru_list.Link(ru);                 // on making a symmetric connection in first pass
    ru_list.Permute();

    for(int i=0;i<ru_list.size; i++) {
      Unit* ru = ru_list.FastEl(i);
      perm_list.Reset();
      FOREACH_ELEM_IN_GROUP(Unit, su, lay->units) {
        if(!self_con && (ru == su)) continue;
        // don't connect to anyone who already recvs from me cuz that will make
        // a symmetric connection which isn't good: symmetry will be enforced later
        RecvCons* scg = su->recv.FindPrjn(prjn);
        if(scg->FindConFromIdx(ru) >= 0) continue;
        perm_list.Link(su);
      }
      perm_list.Permute();
      int j;
      for(j=0; j<first && j<perm_list.size; j++)        // only connect 1/2 of the units
        ru->ConnectFrom((Unit*)perm_list[j], prjn, false, true);
      // true = ignore errs -- to be expected
    }
    // now go thru and make the symmetric connections
    FOREACH_ELEM_IN_GROUP(Unit, ru, lay->units) {
      SendCons* scg = ru->send.FindPrjn(prjn);
      if(scg == NULL) continue;
      int i;
      for(i=0;i<scg->size;i++) {
        Unit* su = scg->Un(i,net);
        ru->ConnectFromCk(su, prjn, true);
        // true = ignore errs -- to be expected
      }
    }
  }
  else {                        // not a symmetric self projection
    UnitPtrList perm_list;      // permution list
    FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
      perm_list.Reset();
      FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
        if(!self_con && (ru == su)) continue;
        perm_list.Link(su);
      }
      perm_list.Permute();
      for(int i=0; i<recv_no && i<perm_list.size; i++)
        ru->ConnectFrom((Unit*)perm_list[i], prjn, false, true);
      // true = ignore errs -- to be expected
    }
  }
}

