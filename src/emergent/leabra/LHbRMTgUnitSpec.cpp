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
#include <MSNUnitSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LHbRMTgSpecs);
TA_BASEFUNS_CTORS_DEFN(LHbRMTgGains);
TA_BASEFUNS_CTORS_DEFN(LHbRMTgUnitSpec);

void LHbRMTgSpecs::Initialize() {
  patch_cur = false;
  Defaults_init();
}

void LHbRMTgSpecs::Defaults_init() {
  matrix_td = false;
  min_pvneg = 0.1f;
  rec_data = false;
}

void LHbRMTgGains::Initialize() {
  Defaults_init();
}

void LHbRMTgGains::Defaults_init() {
  all = 1.0f;
  vspatch_pos_D1 = 1.0f;
  vspatch_pos_D2 = 1.0f;
  vspatch_pos_net_neg_gain = 0.2f;
  vsmatrix_pos_D1 = 1.0f;
  vsmatrix_pos_D2 = 1.0f;
  vspatch_neg_D1 = 1.0f;
  vspatch_neg_D2 = 1.0f;
  vsmatrix_neg_D1 = 1.0f;
  vsmatrix_neg_D2 = 1.0f;
  vspatch_neg_net_neg_gain = 0.2f;
}

void LHbRMTgUnitSpec::Initialize() {
  SetUnique("deep_raw_qtr", true);
  deep_raw_qtr = Q4;
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

bool LHbRMTgUnitSpec::CheckConfig_Unit(Unit* u, bool quiet) {
  LeabraUnit* un = (LeabraUnit*)u;
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  LeabraNetwork* net = (LeabraNetwork*)un->own_net();
  net->SetNetFlag(Network::NETIN_PER_PRJN); // this is required for this computation!

  bool rval = true;
  
  LeabraLayer* pv_pos_lay = NULL;
  LeabraLayer* vspatch_pos_D1_lay = NULL;
  LeabraLayer* vspatch_pos_D2_lay = NULL;
  LeabraLayer* vsmatrix_pos_D1_lay = NULL;
  LeabraLayer* vsmatrix_pos_D2_lay = NULL;
  
  LeabraLayer* pv_neg_lay = NULL;
  LeabraLayer* vspatch_neg_D1_lay = NULL;
  LeabraLayer* vspatch_neg_D2_lay = NULL;
  LeabraLayer* vsmatrix_neg_D1_lay = NULL;
  LeabraLayer* vsmatrix_neg_D2_lay = NULL;
  
  GetRecvLayers(un, pv_pos_lay, vspatch_pos_D1_lay, vspatch_pos_D2_lay, vsmatrix_pos_D1_lay, vsmatrix_pos_D2_lay, pv_neg_lay, vspatch_neg_D1_lay, vspatch_neg_D2_lay,
                vsmatrix_neg_D1_lay, vsmatrix_neg_D2_lay);

  if(u->CheckError(!vspatch_pos_D1_lay, quiet, rval,
                   "did not find VS Patch D1R recv projection -- searches for MSNUnitSpec::PATCH, D1R")) {
    rval = false;
  }
  if(u->CheckError(!vspatch_pos_D2_lay, quiet, rval,
                   "did not find VS Patch D2R recv projection -- searches for MSNUnitSpec::PATCH, D2R")) {
    rval = false;
  }
  // TODO: all the AVERSIVE guys optional?
  
  // matrix is optional
  // if(u->CheckError(!matrix_dir_lay, quiet, rval,
  //                  "did not find VS Matrix Direct recv projection -- searches for Matrix and *not* Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  // if(u->CheckError(!matrix_ind_lay, quiet, rval,
  //                  "did not find VS Matrix Indirect recv projection -- searches for Matrix and Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  
  if(u->CheckError(!pv_pos_lay, quiet, rval,
                   "did not find PV Positive recv projection -- searches for PosPV in layer name")) {
    rval = false;
  }
  if(u->CheckError(!pv_neg_lay, quiet, rval,
                   "did not find PV Negative recv projection -- searches for NegPV in layer name")) {
    rval = false;
  }

  return rval;
}

bool LHbRMTgUnitSpec::GetRecvLayers(LeabraUnit* u,
                                    LeabraLayer*& pv_pos_lay,
                                    LeabraLayer*& vspatch_pos_D1_lay,
                                    LeabraLayer*& vspatch_pos_D2_lay,
                                    LeabraLayer*& vsmatrix_pos_D1_lay,
                                    LeabraLayer*& vsmatrix_pos_D2_lay,
                                    LeabraLayer*& pv_neg_lay,
                                    LeabraLayer*& vspatch_neg_D1_lay,
                                    LeabraLayer*& vspatch_neg_D2_lay,
                                    LeabraLayer*& vsmatrix_neg_D1_lay,
                                    LeabraLayer*& vsmatrix_neg_D2_lay) {
  
  pv_pos_lay = NULL;
  vspatch_pos_D1_lay = NULL;
  vspatch_pos_D2_lay = NULL;
  vsmatrix_pos_D1_lay = NULL;
  vsmatrix_pos_D2_lay = NULL;
  pv_neg_lay = NULL;
  vspatch_neg_D1_lay = NULL;
  vspatch_neg_D2_lay = NULL;
  vsmatrix_neg_D1_lay = NULL;
  vsmatrix_neg_D2_lay = NULL;
  
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* fmlay = (LeabraLayer*) recv_gp->prjn->from.ptr();
    LeabraUnitSpec* us = (LeabraUnitSpec*) fmlay->GetUnitSpec();
    if(us->InheritsFrom(&TA_MSNUnitSpec)) {
      MSNUnitSpec* mus = (MSNUnitSpec*)us;
      if(mus->matrix_patch == MSNUnitSpec::PATCH) {
        if(mus->dar == MSNUnitSpec::D2R ) {
          if(mus->valence == MSNUnitSpec::APPETITIVE) { vspatch_pos_D2_lay = fmlay; }
          else { vspatch_neg_D2_lay = fmlay; }
        }
        else { // D1R
          if(mus->valence == MSNUnitSpec::APPETITIVE) { vspatch_pos_D1_lay = fmlay; }
          else { vspatch_neg_D1_lay = fmlay; }
        }
      }
      else if(mus->matrix_patch == MSNUnitSpec::MATRIX) {
        if(mus->dar == MSNUnitSpec::D2R) {
          if (mus->valence == MSNUnitSpec::APPETITIVE) { vsmatrix_pos_D2_lay = fmlay; }
          else { vsmatrix_neg_D2_lay = fmlay; }
        }
        else { // D1R
          if (mus->valence == MSNUnitSpec::APPETITIVE) { vsmatrix_pos_D1_lay = fmlay; }
          else { vsmatrix_neg_D1_lay = fmlay; }
        }
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

void LHbRMTgUnitSpec::Compute_Lhb(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* pv_pos_lay = NULL;
  LeabraLayer* vspatch_pos_D1_lay = NULL;
  LeabraLayer* vspatch_pos_D2_lay = NULL;
  LeabraLayer* vsmatrix_pos_D1_lay = NULL;
  LeabraLayer* vsmatrix_pos_D2_lay = NULL;
  LeabraLayer* pv_neg_lay = NULL;
  LeabraLayer* vspatch_neg_D1_lay = NULL;
  LeabraLayer* vspatch_neg_D2_lay = NULL;
  LeabraLayer* vsmatrix_neg_D1_lay = NULL;
  LeabraLayer* vsmatrix_neg_D2_lay = NULL;
  
  
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  GetRecvLayers(un, pv_pos_lay, vspatch_pos_D1_lay, vspatch_pos_D2_lay,
                vsmatrix_pos_D1_lay, vsmatrix_pos_D2_lay, pv_neg_lay,
                vspatch_neg_D1_lay, vspatch_neg_D2_lay,
                vsmatrix_neg_D1_lay, vsmatrix_neg_D2_lay);
  
  // use avg act over layer..
  // note: need acts_q0 for patch to reflect previous trial..
  float vspatch_pos_D1;
  if(lhb.patch_cur)
    vspatch_pos_D1 = vspatch_pos_D1_lay->acts_eq.avg * vspatch_pos_D1_lay->units.size;
  else
    vspatch_pos_D1 = vspatch_pos_D1_lay->acts_q0.avg * vspatch_pos_D1_lay->units.size;
    
  float vspatch_pos_D2;
  if(lhb.patch_cur)
    vspatch_pos_D2 = vspatch_pos_D2_lay->acts_eq.avg * vspatch_pos_D2_lay->units.size;
  else
    vspatch_pos_D2 = vspatch_pos_D2_lay->acts_q0.avg * vspatch_pos_D2_lay->units.size;

  float vspatch_pos_net = (gains.vspatch_pos_D1 * vspatch_pos_D1) - (gains.vspatch_pos_D2 * vspatch_pos_D2); // positive number is net excitatory in LHb, i.e., the "dipper"
  if (vspatch_pos_net < 0.0f) {
    vspatch_pos_net *= gains.vspatch_pos_net_neg_gain;
  }
  
  // repeat for AVERSIVE guys...
  float vspatch_neg_D1;
  if(lhb.patch_cur)
    vspatch_neg_D1 = vspatch_neg_D1_lay->acts_eq.avg * vspatch_neg_D1_lay->units.size;
  else
    vspatch_neg_D1 = vspatch_neg_D1_lay->acts_q0.avg * vspatch_neg_D1_lay->units.size;
  
  float vspatch_neg_D2;
  if(lhb.patch_cur)
    vspatch_neg_D2 = vspatch_neg_D2_lay->acts_eq.avg * vspatch_neg_D2_lay->units.size;
  else
    vspatch_neg_D2 = vspatch_neg_D2_lay->acts_q0.avg * vspatch_neg_D2_lay->units.size;
  
  // TODO: do I need anything like the net_neg_gain guy??????
  float vspatch_neg_net = (gains.vspatch_neg_D2 * vspatch_neg_D2) - (gains.vspatch_neg_D1 * vspatch_neg_D1); // positive number is net inhibitory in LHb - disinhibitory "burster"
  
// TODO: 0.2 gain here seems to be preventing any mitigation of the neg PV
  //  if (vspatch_neg_net > 0.0f) {
//    vspatch_neg_net *= gains.vspatch_neg_net_neg_gain; // TODO: probably only need one gain here, but will keep them separate for now...
//  }
  
  
  float vsmatrix_pos_D1 = 0.0f;
  if(vsmatrix_pos_D1_lay)
    vsmatrix_pos_D1 = vsmatrix_pos_D1_lay->acts_eq.avg * vsmatrix_pos_D1_lay->units.size;
  float vsmatrix_pos_D2 = 0.0f;
  if(vsmatrix_pos_D2_lay)
    vsmatrix_pos_D2 = vsmatrix_pos_D2_lay->acts_eq.avg * vsmatrix_pos_D2_lay->units.size;

  float vsmatrix_neg_D1 = 0.0f;
  if(vsmatrix_neg_D1_lay) {
    vsmatrix_neg_D1 = vsmatrix_neg_D1_lay->acts_eq.avg * vsmatrix_neg_D1_lay->units.size;
  }
  float vsmatrix_neg_D2 = 0.0f;
  if(vsmatrix_neg_D2_lay) {
    //dms_matrix_ind = dms_matrix_ind_lay->acts_eq.max;
    vsmatrix_neg_D2 = vsmatrix_neg_D2_lay->acts_eq.avg * vsmatrix_neg_D2_lay->units.size;
  }
  float pv_pos = pv_pos_lay->acts_eq.avg * pv_pos_lay->units.size;
  float pv_neg = pv_neg_lay->acts_eq.avg * pv_neg_lay->units.size;
  
  
  // actual punishments should never be completely predicted away...
   float residual_pvneg = lhb.min_pvneg * pv_neg;
  residual_pvneg = MAX(residual_pvneg, 0.0f); // just a precaution
 
  // TODO: still double counting matrix_dir via its reflection in both net_pv_pos and net_lv_pos; QUICK FIX
  
  //matrix_dir = matrix_ind = 0.0f; // TODO: quick fix - just ignore it
  
  // TODO: maybe no longer need the net_pv_pos absorb somehow?
  // TODO: OR, also take the MAX(net_pv_pos, net_lv_pos); // call it net_pos and ONLY use that!
  // TODO: this latter seems the best so far...

  
  // net out the VS matrix D1 versus D2 pairs...WATCH the signs - double negatives!
  float vsmatrix_pos_net = (gains.vsmatrix_pos_D1 * vsmatrix_pos_D1) - (gains.vsmatrix_pos_D2 * vsmatrix_pos_D2); // positive number net inhibitory!
  float vsmatrix_neg_net = (gains.vsmatrix_neg_D2 * vsmatrix_neg_D2) - (gains.vsmatrix_neg_D1 * vsmatrix_neg_D1); // positive number net excitatory!
  
  
  
  // don't double count pv going through the matrix guys
  float net_pos = vsmatrix_pos_net;
  if(pv_pos) { net_pos = MAX(pv_pos, vsmatrix_pos_net); }
  float net_neg = vsmatrix_neg_net;
  if(pv_neg) { net_neg = MAX(pv_neg, vsmatrix_neg_net); }
  
  // likewise, don't double count lv going through the matrix guys - SHOULD BE ALREADY DONE NOW!
//  float net_lv_pos = MAX(gains.dms_matrix_dir * dms_matrix_dir,
//                         gains.vs_matrix_dir * matrix_dir);
//  float net_lv_neg = MAX(gains.dms_matrix_ind * dms_matrix_ind,
//                         gains.vs_matrix_ind * matrix_ind);
  
  
  // TODO: better approach to double matrix counting
//  float net_pos = MAX(net_pv_pos, net_lv_pos);
//  float net_neg = MAX(net_pv_neg, net_lv_neg);
  
//  float net_lhb = net_pv_neg - (gains.patch_ind * patch_ind) - net_pv_pos +
//                           (gains.patch_dir * patch_dir) - (gains.dms_matrix_dir * dms_matrix_dir) + (gains.dms_matrix_ind * dms_matrix_ind) + residual_pvneg;
  
  float net_lhb = net_neg - net_pos + vspatch_pos_net - vspatch_neg_net + residual_pvneg;
  
  net_lhb *=gains.all;
  
  // TODO: tweak the gains.params here and for initialization, defaults, etc.
  // TODO: note should matrix (and patch?) dir vs. indirect guys be netted out first to reflect the Go/NoGo competition?
  
  u->act_eq = u->act_nd = u->act = u->net = u->ext = net_lhb;
  
  if(lhb.rec_data) {
    LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
    LeabraLayer* lay = un->own_lay();
    lay->SetUserData("pv_pos", pv_pos);
    lay->SetUserData("vsmatrix_pos_D1", vsmatrix_pos_D1);
    lay->SetUserData("vsmatrix_pos_D2", vsmatrix_pos_D2);
    lay->SetUserData("net_pos", net_pos);
    lay->SetUserData("vspatch_pos_D1", vspatch_pos_D1);
    lay->SetUserData("vspatch_pos_D2", vspatch_pos_D2);
    
    lay->SetUserData("pv_neg", pv_neg);
    lay->SetUserData("vsmatrix_neg_D1", vsmatrix_neg_D1);
    lay->SetUserData("vsmatrix_neg_D2", vsmatrix_neg_D2);
    lay->SetUserData("net_neg", net_neg);
    lay->SetUserData("vspatch_neg_D1", vspatch_neg_D1);
    lay->SetUserData("vspatch_neg_D2", vspatch_neg_D2);
    
    lay->SetUserData("residual_pvneg", residual_pvneg);
    lay->SetUserData("net_lhb", net_lhb);
  }
}

void LHbRMTgUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(Quarter_DeepRawNow(net->quarter)) {
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
  if(lhb.matrix_td) {
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      float matrix_ind = 0.0f;
      const int nrg = u->NRecvConGps(net, thr_no);
      for(int g=0; g<nrg; g++) {
        LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
        if(recv_gp->NotActive()) continue;
        LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
        if(from->name.contains("Matrix") && (from->name.contains("Ind") ||
                                             from->name.contains("NoGo") ||
                                             from->name.contains("D2"))) {
          matrix_ind += recv_gp->net_raw;
        }
      }
      u->misc_1 = matrix_ind;       // save for next time -- this is the raw net..
    }
  }
}

