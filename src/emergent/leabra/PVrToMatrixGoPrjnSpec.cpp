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

#include "PVrToMatrixGoPrjnSpec.h"
#include <Network>
#include <LeabraLayer>
#include <SNrThalLayerSpec>
#include <PFCLayerSpec>
#include <MatrixLayerSpec>


void PVrToMatrixGoPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  LeabraLayer* mtx_lay = (LeabraLayer*)prjn->layer;
  LeabraLayer* pvr_lay = (LeabraLayer*)prjn->from.ptr();

  MatrixLayerSpec* mtx_ls = (MatrixLayerSpec*)mtx_lay->GetLayerSpec();
  if(TestError(!mtx_ls || !mtx_ls->InheritsFrom(&TA_MatrixLayerSpec),
	       "Connect_impl", "recv layer is not a MatrixLayerSpec.  recv layer:", mtx_lay->name))
    return;
  if(TestError(pvr_lay->units.size < 4,
	       "Connect_impl", "PVr layer doesn't have at least 4 units:", pvr_lay->name))
    return;

  int n_per_gp = mtx_lay->un_geom.n;
  int n_gps = mtx_lay->gp_geom.n;
  int tot_un = n_per_gp * n_gps;

  Layer::AccessMode racc_md = Layer::ACC_GP;

  Unit* pvr_su = NULL;
  switch(mtx_ls->gating_type) {
  case SNrThalLayerSpec::INPUT:
    pvr_su = pvr_lay->units.SafeEl(2); // .5 middle value unit
    break;
  case SNrThalLayerSpec::IN_MNT:
    pvr_su = pvr_lay->units.SafeEl(2); // .5 middle value unit
    break;
  case SNrThalLayerSpec::OUTPUT:
    pvr_su = pvr_lay->units.SafeEl(3); // 1 right value unit
    break;
  case SNrThalLayerSpec::MNT_OUT:
    pvr_su = pvr_lay->units.SafeEl(2); // .5 middle value unit
    break;
  case SNrThalLayerSpec::OUT_MNT:
    pvr_su = pvr_lay->units.SafeEl(3); // 1 right value unit
    break;       
  default:			// compiler food
    break;
  }

  if(!pvr_su) return;
  pvr_su->SendConsPreAlloc(tot_un, prjn);

  for(int rgi=0; rgi < n_gps; rgi++) {
    for(int rui=0; rui < n_per_gp; rui++) {
      Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
      ru->RecvConsPreAlloc(1, prjn);
    }
  }

  for(int rgi=0; rgi < n_gps; rgi++) {
    for(int rui=0; rui < n_per_gp; rui++) {
      Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
      ru->ConnectFrom(pvr_su, prjn);
    }
  }
}

