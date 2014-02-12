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

#include "SNrPrjnSpec.h"
#include <Network>
#include <LeabraLayer>
#include <SNrThalLayerSpec>
#include <PFCLayerSpec>
#include <MatrixLayerSpec>

TA_BASEFUNS_CTORS_DEFN(SNrPrjnSpec);

void SNrPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  LeabraLayer* to_lay = (LeabraLayer*)prjn->layer;
  LeabraLayer* fm_lay = (LeabraLayer*)prjn->from.ptr();

  int n_in, n_mnt, n_mnt_out, n_out, n_out_mnt;
  int snr_st_idx = -1;
  if(to_lay->GetLayerSpec()->InheritsFrom(&TA_SNrThalLayerSpec)) {
    SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)to_lay->GetLayerSpec();
    if(fm_lay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) {
      PFCLayerSpec* pfcls = (PFCLayerSpec*)fm_lay->GetLayerSpec();
      snr_st_idx = snr_ls->SNrThalStartIdx(to_lay, pfcls->pfc_type, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
    }
    else if(fm_lay->GetLayerSpec()->InheritsFrom(&TA_MatrixLayerSpec)) {
      MatrixLayerSpec* mtxls = (MatrixLayerSpec*)fm_lay->GetLayerSpec();
      snr_st_idx = snr_ls->SNrThalStartIdx(to_lay, mtxls->gating_type, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
    }
    if(snr_st_idx >= 0) {
      for(int i=0; i<fm_lay->gp_geom.n; i++) {
	Connect_Gp(prjn, Layer::ACC_GP, snr_st_idx + i, Layer::ACC_GP, i);
      }
    }
  }
  else if(fm_lay->GetLayerSpec()->InheritsFrom(&TA_SNrThalLayerSpec)) {
    SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)fm_lay->GetLayerSpec();
    if(to_lay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) {
      PFCLayerSpec* pfcls = (PFCLayerSpec*)to_lay->GetLayerSpec();
      snr_st_idx = snr_ls->SNrThalStartIdx(fm_lay, pfcls->pfc_type, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
    }
    else if(to_lay->GetLayerSpec()->InheritsFrom(&TA_MatrixLayerSpec)) {
      MatrixLayerSpec* mtxls = (MatrixLayerSpec*)to_lay->GetLayerSpec();
      snr_st_idx = snr_ls->SNrThalStartIdx(fm_lay, mtxls->gating_type, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
    }
    if(snr_st_idx >= 0) {
      for(int i=0; i<to_lay->gp_geom.n; i++) {
	Connect_Gp(prjn, Layer::ACC_GP, i, Layer::ACC_GP, snr_st_idx + i);
      }
    }
  }
  else {
    if(TestError(true,
		 "Connect_impl", "either the recv or send layer must be a SNrThal layer, recv layer:", to_lay->name, "send layer:", fm_lay->name))
      return;
  }
}

