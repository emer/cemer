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

#include "SmallWorldPrjnSpec.h"
#include <Network>
#include <DataTable>
#include <taMath_float>

TA_BASEFUNS_CTORS_DEFN(SmallWorldPrjnSpec);

void SmallWorldPrjnSpec::Initialize() {
  lambda = 3.0f;
}

void SmallWorldPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  DataTable* prjns;
  DataTable* coords;

  prjns = new DataTable;
  coords = new DataTable;

  coords->NewColMatrix(taBase::VT_FLOAT, "recv_pos", 2, 3, 1);
  coords->NewColMatrix(taBase::VT_FLOAT, "send_pos", 2, 3, 1);
  coords->AddRows();

  Unit_Group* recv_units;

  Layer* send;
  Layer* recv;

  Unit* ru;
  Unit* su;

  float_Matrix* recv_pos;
  float_Matrix* send_pos;

  int rx, ry, rz;
  int sx, sy, sz;
  int this_rx, this_ry;
  int this_sx, this_sy;
  int this_recv_flat;
  int n_recv_units, this_recv_unit;
  int alloc_loop;

  float distance, probability, power, ratio;
  bool project;

  recv = prjn->layer;

  rx = recv->un_geom.x;
  ry = recv->un_geom.y;
  rz = recv->pos.z;
  n_recv_units = rx*ry;
  recv_units = &(recv->units);
  send = prjn->from;
  sx = send->un_geom.x;
  sy = send->un_geom.y;
  sz = send->pos.z;
 
  prjns->Reset();
  prjns->NewColMatrix(taBase::VT_INT, "prjns", 3, n_recv_units, sx, sy);
  prjns->AddRows();
  this_recv_flat = 0;
 
  // Compute which units will project to eachother in the 'projections' table.
  // Gives a row for each unit for nice GridView plotting

  for (this_rx = 0; this_rx < rx; this_rx++) {
    for (this_ry = 0; this_ry < ry; this_ry++) {
      for (this_sx = 0; this_sx < sx; this_sx++) {
	for (this_sy = 0; this_sy < sy; this_sy++) {

	  coords->SetMatrixVal(this_rx, "recv_pos", 0, 0, 0);
	  coords->SetMatrixVal(this_ry, "recv_pos", 0, 1, 0);
	  coords->SetMatrixVal(rz, "recv_pos", 0, 2, 0);
	  coords->SetMatrixVal(this_sx, "send_pos", 0, 0, 0);
	  coords->SetMatrixVal(this_sy, "send_pos", 0, 1, 0);
	  coords->SetMatrixVal(sz, "send_pos", 0, 2, 0);
	  recv_pos = (float_Matrix*)coords->GetColMatrix(0);
	  send_pos = (float_Matrix*)coords->GetColMatrix(1);
	  distance = taMath_float::vec_dist(recv_pos, send_pos, taMath::EUCLIDIAN);
	  ratio = taMath_float::div(distance, lambda);
	  power = taMath_float::pow(ratio, 2.0);
	  probability = taMath_float::exp_fast(-power);
	  project = Random::BoolProb(probability);

	  if(project) {
	    prjns->SetMatrixVal(project, 0, 0, this_recv_flat, this_sx, this_sy);
	  }
	}
      }
      this_recv_flat = this_recv_flat + 1;
    }
  }
 
  // Allocate memory and connect as per the projections table above

  for (this_recv_unit = 0; this_recv_unit < n_recv_units; this_recv_unit++) {
    ru = recv_units->Leaf(this_recv_unit);
    ru->RecvConsPreAlloc(n_recv_units, prjn);
  }
 
  for (alloc_loop = 1; alloc_loop >= 0; alloc_loop--) {
    this_recv_flat = 0;
    for (this_rx = 0; this_rx < rx; this_rx++) {
      for (this_ry = 0; this_ry < ry; this_ry++) {
	ru = recv->FindUnitFmCoord(this_rx, this_ry);
	for (this_sx = 0; this_sx < sx; this_sx++) {
	  for (this_sy = 0; this_sy < sy; this_sy++) {
	    project = prjns->GetMatrixVal(0, 0, this_recv_flat, this_sx, this_sy).toBool();
	    if(project) {
	      su = send->FindUnitFmCoord(this_sx, this_sy);
	      if(su) {
		ru->ConnectFrom(su, prjn, alloc_loop);
	      }
	    }
	  }
	}
	this_recv_flat = this_recv_flat + 1;
      }
    }
    if(alloc_loop) {
      send->SendConsPostAlloc(prjn);
    }
  }
}


