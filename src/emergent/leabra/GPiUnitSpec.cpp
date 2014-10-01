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

#include "GPiUnitSpec.h"

#include <LeabraNetwork>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(GPiUnitSpec);
TA_BASEFUNS_CTORS_DEFN(GPiGoNogoSpec);

void GPiGoNogoSpec::Initialize() {
  nogo = 0.5f;
}

void GPiGoNogoSpec::Defaults_init() {
}

void GPiUnitSpec::Initialize() {
}

void GPiUnitSpec::Defaults_init() {
}

void GPiUnitSpec::HelpConfig() {
  String help = "GPiUnitSpec Computation:\n\
 - Computes BG output as a positive activation signal with competition inhibition\n\
 select best gating candidate -- should receive inputs from Matrix_Go and NoGo layers\n                     \
 \nGPiUnitSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure layers.\n\
 - Recv cons should be standard, with no learning";
  taMisc::Confirm(help);
}

bool GPiUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  LeabraUnit* u = (LeabraUnit*)un;
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  LeabraNetwork* net = (LeabraNetwork*)un->own_net();
  net->SetNetFlag(Network::NETIN_PER_PRJN); // this is required for this computation!

  bool rval = true;

  // bool patch_dir = false;
  // bool patch_ind = false;
  // bool matrix_dir = false;
  // bool matrix_ind = false;
  // bool pv_pos = false;
  // bool pv_neg = false;
  // for(int g=0; g<u->recv.size; g++) {
  //   LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
  //   if(recv_gp->NotActive()) continue;
  //   LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();

  //   if(from->name.contains("Patch")) {
  //     if(from->name.contains("Ind")) {
  //       patch_ind = true;
  //     }
  //     else {
  //       patch_dir = true;
  //     }
  //   }
  //   else if(from->name.contains("Matrix")) {
  //     if(from->name.contains("Ind") || from->name.contains("NoGo")) {
  //       matrix_ind = true;
  //     }
  //     else {
  //       matrix_dir = true;
  //     }
  //   }
  //   else if(from->name.contains("PosPV")) {
  //     pv_pos = true;
  //   }
  //   else if(from->name.contains("NegPV")) {
  //     pv_neg = true;
  //   }
  // }

  // if(u->CheckError(!patch_dir, quiet, rval,
  //                  "did not find VS Patch Direct recv projection -- searches for Patch and *not* Ind in layer name")) {
  //   rval = false;
  // }
  // if(u->CheckError(!patch_ind, quiet, rval,
  //                  "did not find VS Patch Indirect recv projection -- searches for Patch and Ind in layer name")) {
  //   rval = false;
  // }
  // matrix is optional
  // if(u->CheckError(!matrix_dir, quiet, rval,
  //                  "did not find VS Matrix Direct recv projection -- searches for Matrix and *not* Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  // if(u->CheckError(!matrix_ind, quiet, rval,
  //                  "did not find VS Matrix Indirect recv projection -- searches for Matrix and Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  // if(u->CheckError(!pv_pos, quiet, rval,
  //                  "did not find PV Positive recv projection -- searches for PosPV in layer name")) {
  //   rval = false;
  // }
  // if(u->CheckError(!pv_neg, quiet, rval,
  //                  "did not find PV Negative recv projection -- searches for NegPV in layer name")) {
  //   rval = false;
  // }

  return rval;
}

void GPiUnitSpec::Compute_NetinRaw(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  int nt = net->lthreads.n_threads_act;

  // note: REQUIRES NetinPerPrjn!  Set automatically in CheckConfig
  float go_in = 0.0f;
  float nogo_in = 0.0f;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();

    float g_nw_nt = 0.0f;
    for(int j=0;j<nt;j++) {
      float& ndval = net->send_netin_tmp.FastEl3d(u->flat_idx, g, j); 
      g_nw_nt += ndval;
      ndval = 0.0f;           // zero immediately upon use -- for threads
    }

    recv_gp->net_raw += g_nw_nt;

    if(from->name.contains("NoGo")) {
      nogo_in += recv_gp->net_raw;
    }
    else {
      go_in += recv_gp->net_raw;
    }
  }

  float gpi_net = 0.0f;
  gpi_net = go_in - go_nogo.nogo * nogo_in;
  gpi_net = MAX(gpi_net, 0.0f);
  u->net_raw = gpi_net;

  u->net_delta = 0.0f;  // clear for next use
  u->gi_delta = 0.0f;  // clear for next use
}

