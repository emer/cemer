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
  all = 1.5f;
  patch_dir = 1.0f;
  patch_ind = 1.0f;
  matrix = 1.0f;
  matrix_td = false;
  min_pvneg = 0.1f;
  rec_data = false;
}

void LHbRMTgGains::Defaults_init() {
}

void LHbRMTgUnitSpec::Initialize() {
    SetUnique("deep_qtr", true);
    deep_qtr = Q4;
    SetUnique("act_range", true);
    act_range.max = 2.0f;
    act_range.min = -2.0f;
    act_range.UpdateAfterEdit();
    SetUnique("clamp_range", true);
    clamp_range.max = 2.0f;
    clamp_range.min = -2.0f;
    clamp_range.UpdateAfterEdit();
}

void LHbRMTgUnitSpec::Defaults_init() {
}

void LHbRMTgUnitSpec::HelpConfig() {
  String help = "LHbRMTgUnitSpec (DA value) Computation:\n\
 - Computes DA dips and related phenomena based on inputs from VS Matrix, Patch Direct and Indirect, and PV Pos & Neg.\n\
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

bool LHbRMTgUnitSpec::GetRecvLayers(LeabraUnit* u,                             LeabraLayer*& patch_dir_lay, LeabraLayer*& patch_ind_lay,
                                    LeabraLayer*& matrix_dir_lay, LeabraLayer*& matrix_ind_lay,
                                    LeabraLayer*& pv_pos_lay, LeabraLayer*& pv_neg_lay) {
  patch_dir_lay = NULL;
  patch_ind_lay = NULL;
  matrix_dir_lay = NULL;
  matrix_ind_lay = NULL;
  pv_pos_lay = NULL;
  pv_neg_lay = NULL;
  
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* fmlay = (LeabraLayer*) recv_gp->prjn->from.ptr();
    
    
    if(fmlay->name.contains("Patch")) {
      if(fmlay->name.contains("Ind") || fmlay->name.contains("NoGo") || fmlay->name.contains("D2R")) {
        patch_ind_lay = fmlay;
      }
      else {
        patch_dir_lay = fmlay;
      }
    }
    else if(fmlay->name.contains("Matrix")) {
      if(fmlay->name.contains("Ind") || fmlay->name.contains("NoGo") || fmlay->name.contains("D2R")) {
        matrix_ind_lay = fmlay;
      }
      else {
        matrix_dir_lay = fmlay;
      }
    }
    else if(fmlay->name.contains("PosPV")) {
      pv_pos_lay = fmlay;
    }
    else if(fmlay->name.contains("NegPV")) {
      pv_neg_lay = fmlay;
    }
  }
  return true;
}

// modeled after VTAUnitSpec::Compute_da()
void LHbRMTgUnitSpec::Compute_Lhb(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* patch_dir_lay = NULL;
  LeabraLayer* patch_ind_lay = NULL;
  LeabraLayer* matrix_dir_lay = NULL;
  LeabraLayer* matrix_ind_lay = NULL;
  LeabraLayer* pv_pos_lay = NULL;
  LeabraLayer* pv_neg_lay = NULL;
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  GetRecvLayers(un, patch_dir_lay, patch_ind_lay, matrix_dir_lay, matrix_ind_lay, pv_pos_lay, pv_neg_lay);
  
  // use avg act over layer..
  // note: need acts_q0 for patch to reflect previous trial..
  float patch_dir = patch_dir_lay->acts_q0.avg; // * patch_dir_lay->units.size;
  float patch_ind = patch_ind_lay->acts_q0.avg; // * patch_ind_lay->units.size;
  float matrix_dir = matrix_dir_lay->acts_eq.avg; // * matrix_dir_lay->units.size;
  float matrix_ind = matrix_ind_lay->acts_eq.avg; // * matrix_ind_lay->units.size;
  float pv_pos = pv_pos_lay->acts_eq.avg; // * pv_pos_lay->units.size;
  float pv_neg = pv_neg_lay->acts_eq.avg; // * pv_neg_lay->units.size;
  
  // punishments should never be completely predicted away...
  //float residual_pvneg = 0.0f; // no residual if nothing there to start with
  float residual_pvneg = gains.min_pvneg * pv_neg;
  residual_pvneg = MAX(residual_pvneg, 0.0f); // just a precaution
  
  float net_lhb = pv_neg - (gains.patch_ind * patch_ind) - pv_pos +
                           (gains.patch_dir * patch_dir) + gains.matrix *
                           (matrix_ind - matrix_dir) + residual_pvneg;
  net_lhb *=gains.all;
  // TODO: tweak the gains.params here and for initialization, defaults, etc.
  
  u->act_eq = u->act_nd = u->act = u->net = u->ext = net_lhb;
  
  if(gains.rec_data) {
    LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
    LeabraLayer* lay = un->own_lay();
    lay->SetUserData("pv_pos", pv_pos);
    lay->SetUserData("patch_dir", patch_dir);
    //lay->SetUserData("pv_pos_net", pv_pos_net);
    lay->SetUserData("pv_neg", pv_neg);
    lay->SetUserData("patch_ind", patch_ind);
    //lay->SetUserData("pv_neg_net", pv_neg_net);
    lay->SetUserData("matrix_dir", matrix_dir);
    lay->SetUserData("matrix_ind", matrix_ind);
    //lay->SetUserData("matrix_net", matrix_net);
    lay->SetUserData("net_lhb", net_lhb);
  }
}

void LHbRMTgUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(Quarter_DeepNow(net->quarter)) {
    Compute_Lhb(u, net, thr_no);
  }
  else {
    // todo: why not all the time?
  }
}

void LHbRMTgUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
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

