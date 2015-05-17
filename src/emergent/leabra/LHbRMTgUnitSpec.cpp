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

#include "LHbRMTgUnitSpec.h"

#include <LeabraNetwork>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LHbRMTgUnitSpec);
TA_BASEFUNS_CTORS_DEFN(LHbRMTgGains);

void LHbRMTgGains::Initialize() {
  all = 4.0f;
  patch_dir = 1.0f;
  patch_ind = 1.0f;
  matrix = 1.0f;
  matrix_td = false;
}

void LHbRMTgGains::Defaults_init() {
}

void LHbRMTgUnitSpec::Initialize() {
}

void LHbRMTgUnitSpec::Defaults_init() {
}

void LHbRMTgUnitSpec::HelpConfig() {
  String help = "LHbRMTgUnitSpec (DA value) Computation:\n\
 - Computes DA dips based on inputs from VS Matrix, Patch Direct and Indirect, and PV Pos & Neg.\n\
 \nLHbRMTgUnitSpec Configuration:\n\
 - Use the Wizard gdPVLV button to automatically configure layers.\n\
 - Recv cons should be standard, with no learning";
  taMisc::Confirm(help);
}

bool LHbRMTgUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  LeabraUnit* u = (LeabraUnit*)un;
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  LeabraNetwork* net = (LeabraNetwork*)un->own_net();
  net->SetNetFlag(Network::NETIN_PER_PRJN); // this is required for this computation!

  bool rval = true;

  bool patch_dir = false;
  bool patch_ind = false;
  bool matrix_dir = false;
  bool matrix_ind = false;
  bool pv_pos = false;
  bool pv_neg = false;
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();

    if(from->name.contains("Patch")) {
      if(from->name.contains("Ind")) {
        patch_ind = true;
      }
      else {
        patch_dir = true;
      }
    }
    else if(from->name.contains("Matrix")) {
      if(from->name.contains("Ind") || from->name.contains("NoGo")) {
        matrix_ind = true;
      }
      else {
        matrix_dir = true;
      }
    }
    else if(from->name.contains("PosPV")) {
      pv_pos = true;
    }
    else if(from->name.contains("NegPV")) {
      pv_neg = true;
    }
  }

  if(u->CheckError(!patch_dir, quiet, rval,
                   "did not find VS Patch Direct recv projection -- searches for Patch and *not* Ind in layer name")) {
    rval = false;
  }
  if(u->CheckError(!patch_ind, quiet, rval,
                   "did not find VS Patch Indirect recv projection -- searches for Patch and Ind in layer name")) {
    rval = false;
  }
  // matrix is optional
  // if(u->CheckError(!matrix_dir, quiet, rval,
  //                  "did not find VS Matrix Direct recv projection -- searches for Matrix and *not* Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  // if(u->CheckError(!matrix_ind, quiet, rval,
  //                  "did not find VS Matrix Indirect recv projection -- searches for Matrix and Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  if(u->CheckError(!pv_pos, quiet, rval,
                   "did not find PV Positive recv projection -- searches for PosPV in layer name")) {
    rval = false;
  }
  if(u->CheckError(!pv_neg, quiet, rval,
                   "did not find PV Negative recv projection -- searches for NegPV in layer name")) {
    rval = false;
  }

  return rval;
}

void LHbRMTgUnitSpec::Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  int nt = net->n_thrs_built;
  int flat_idx = u->UnFlatIdx(net, thr_no);
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif

  // note: REQUIRES NetinPerPrjn!  Set automatically in CheckConfig
  float patch_dir = 0.0f;
  float patch_ind = 0.0f;
  float matrix_dir = 0.0f;
  float matrix_ind = 0.0f;
  float pv_pos = 0.0f;
  float pv_neg = 0.0f;
  const int nrg = u->NRecvConGps(net, thr_no);
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();

    float g_nw_nt = 0.0f;
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendNetinTmpPerPrjn(j, g)[flat_idx]; 
      g_nw_nt += ndval;
#ifndef CUDA_COMPILE
      ndval = 0.0f;           // zero immediately upon use -- for threads
#endif
    }

    recv_gp->net_raw += g_nw_nt;

    if(from->name.contains("Patch")) {
      if(from->name.contains("Ind")) {
        patch_ind += recv_gp->net_raw;
      }
      else {
        patch_dir += recv_gp->net_raw;
      }
    }
    else if(from->name.contains("Matrix")) {
      if(from->name.contains("Ind") || from->name.contains("NoGo")) {
        matrix_ind += recv_gp->net_raw;
      }
      else {
        matrix_dir += recv_gp->net_raw;
      }
    }
    else if(from->name.contains("PosPV")) {
      pv_pos += recv_gp->net_raw;
    }
    else if(from->name.contains("NegPV")) {
      pv_neg += recv_gp->net_raw;
    }
  }

  // todo? do shunting separately in the VTA layer itself, with direct prjns

  // now do the proper subtractions, and individually rectify each term
  // this individual rectification is important so that system is not 
  // sensitive to overshoot of predictor relative to its comparison value
  float matrix_net = 0.0f;
  if(gains.matrix_td) {
    matrix_net = gains.matrix * (u->misc_1 - matrix_ind); // misc_1 holds prior -- looking for dips so sign is reversed!
  }
  else {
    matrix_net = gains.matrix * (matrix_ind - matrix_dir); // net positive dipping action from matrix
  }
  matrix_net = MAX(0.0f, matrix_net);

  float pv_neg_net = gains.patch_ind * (pv_neg - patch_ind); // indir cancels neg
  pv_neg_net = MAX(0.0f, pv_neg_net);

  float pv_pos_net = gains.patch_dir * (patch_dir - pv_pos); // direct cancels pos
  pv_pos_net = MAX(0.0f, pv_pos_net);

  u->net_raw = gains.all * (matrix_net + pv_neg_net + pv_pos_net);
}

void LHbRMTgUnitSpec::Quarter_Final(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  inherited::Quarter_Final(u, net, thr_no);
  if(gains.matrix_td) {
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      float matrix_ind = 0.0f;
      const int nrg = u->NRecvConGps(net, thr_no);
      for(int g=0; g<nrg; g++) {
        LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
        if(recv_gp->NotActive()) continue;
        LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
        if(from->name.contains("Matrix") && (from->name.contains("Ind") ||
                                             from->name.contains("NoGo"))) {
          matrix_ind += recv_gp->net_raw;
        }
      }
      u->misc_1 = matrix_ind;       // save for next time -- this is the raw net..
    }
  }
}

