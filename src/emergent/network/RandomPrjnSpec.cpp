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

#include "RandomPrjnSpec.h"
#include <Network>
#include <int_Matrix>

void RandomPrjnSpec::Initialize() {
  p_con = .25;
  sym_self = false;
  same_seed = false;
  rndm_seed.GetCurrent();
}

void RandomPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

void RandomPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(same_seed)
    rndm_seed.OldSeed();

  int n_send_units = prjn->from->units.leaves;
  int n_recv_units = prjn->layer->units.leaves;

  int_Matrix* send_alloc = new int_Matrix;
  int_Matrix* recv_alloc = new int_Matrix;
  int_Matrix* cons = new int_Matrix;

  send_alloc->SetGeom(1, n_send_units);
  recv_alloc->SetGeom(1, n_recv_units);
  cons->SetGeom(2, n_recv_units, n_send_units);

  for (int i = 0; i < n_recv_units; i++) {
    for (int j= 0; j < n_send_units; j++) {
      if (Random::BoolProb(p_con)) {
        cons->Set(1, i, j);
        send_alloc->Set(send_alloc->FastEl(j) + 1, j);
        recv_alloc->Set(recv_alloc->FastEl(i) + 1, i);
      }
    }
  }

  for (int i = 0; i < n_recv_units; i++)
    prjn->layer->units.FastEl(i)->RecvConsPreAlloc(recv_alloc->FastEl(i), prjn);

  for (int j = 0; j < n_send_units; j++)
    prjn->from->units.FastEl(j)->SendConsPreAlloc(send_alloc->FastEl(j), prjn);

  for (int i = 0; i < n_recv_units; i++) {
    for (int j = 0; j < n_send_units; j++) {
      if (cons->FastEl(i, j))
        prjn->layer->units.FastEl(i)->ConnectFrom(prjn->from->units.FastEl(j), prjn);
    }
  }
}


