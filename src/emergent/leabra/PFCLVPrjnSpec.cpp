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

#include "PFCLVPrjnSpec.h"
#include <LeabraLayer>

void PFCLVPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  LeabraLayer* lv_lay = (LeabraLayer*)prjn->layer;
  LeabraLayer* pfc_lay = (LeabraLayer*)prjn->from.ptr();

  int tot_pfc_prjns = 0;
  int tot_pfc_stripes = 0;
  int my_start_stripe = 0;
  int my_prjn_idx = 0;
  for(int i=0; i< lv_lay->projections.size; i++) {
    Projection* prj = lv_lay->projections[i];
    if(prj->spec.SPtr()->InheritsFrom(&TA_PFCLVPrjnSpec)) {
      if(prj == prjn) {
        my_start_stripe = tot_pfc_stripes;
        my_prjn_idx = tot_pfc_prjns;
      }
      tot_pfc_prjns++;
      tot_pfc_stripes += prj->from->gp_geom.n;
    }
  }

  int n_lv_stripes = lv_lay->gp_geom.n;
  int n_pfc_stripes = pfc_lay->gp_geom.n;

  if(n_lv_stripes <= 1) {       // just full connectivity
    Connect_Gp(prjn, Layer::ACC_LAY, 0, Layer::ACC_LAY, 0);
  }
  else if(n_lv_stripes == n_pfc_stripes) { // just one-to-one
    for(int i=0; i<n_pfc_stripes; i++) {
      Connect_Gp(prjn, Layer::ACC_GP, i, Layer::ACC_GP, i);
    }
  }
  else if(n_lv_stripes == n_pfc_stripes + 1) { // full plus one-to-one
    Connect_Gp(prjn, Layer::ACC_GP, 0, Layer::ACC_LAY, 0); // full for first prjn
    for(int i=0; i<n_pfc_stripes; i++) { // then gp one-to-one
      Connect_Gp(prjn, Layer::ACC_GP, i+1, Layer::ACC_GP, i);
    }
  }
  else if(n_lv_stripes == tot_pfc_stripes) { // multi-pfc just one-to-one
    for(int i=0; i<n_pfc_stripes; i++) {
      Connect_Gp(prjn, Layer::ACC_GP, my_start_stripe + i, Layer::ACC_GP, i);
    }
  }
  else if(n_lv_stripes == tot_pfc_stripes + 1) { // multi-pfc full plus one-to-one
    Connect_Gp(prjn, Layer::ACC_GP, 0, Layer::ACC_LAY, 0); // full for first prjn
    for(int i=0; i<n_pfc_stripes; i++) { // then gp one-to-one
      Connect_Gp(prjn, Layer::ACC_GP, my_start_stripe + i, Layer::ACC_GP, i);
    }
  }
  else if(n_lv_stripes == tot_pfc_stripes + tot_pfc_prjns) { // multi-pfc separate full plus one-to-one
    Connect_Gp(prjn, Layer::ACC_GP, my_start_stripe + my_prjn_idx, Layer::ACC_LAY, 0); // full for first prjn
    for(int i=0; i<n_pfc_stripes; i++) { // then gp one-to-one
      Connect_Gp(prjn, Layer::ACC_GP, my_start_stripe + my_prjn_idx + i+1, Layer::ACC_GP, i);
    }
  }
  else {
    TestError(true, "Connect_impl", "Number of LV unit groups (stripes) must be either 1, equal to number of PFC stripes (sending prjn), or PFC stripes + 1 -- was not any of these -- connection failed");
  }
}

