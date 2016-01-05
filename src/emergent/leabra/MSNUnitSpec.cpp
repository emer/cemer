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

#include "MSNUnitSpec.h"

#include <LeabraNetwork>
#include <PFCUnitSpec>
#include <MSNConSpec>

TA_BASEFUNS_CTORS_DEFN(MatrixActSpec);
TA_BASEFUNS_CTORS_DEFN(MSNUnitSpec);

void MatrixActSpec::Initialize() {
  Defaults_init();
}

void MatrixActSpec::Defaults_init() {
  mnt_gate_inhib = 0.002f;
  out_gate_inhib = 0.0f;
  gate_i_tau = 4.0f;
  out_ach_inhib = 0.3f;
  mnt_ach_inhib = 0.0f;
  mnt_deep_mod = false;
  out_deep_mod = true;
  gate_i_dt = 1.0f / gate_i_tau;
}

void MatrixActSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gate_i_dt = 1.0f / gate_i_tau;
}

void MSNUnitSpec::Initialize() {
  dar = D1R;
  matrix_patch = MATRIX;
  dorsal_ventral = DORSAL;
  valence = APPETITIVE;
}

void MSNUnitSpec::Defaults_init() {
}

void MSNUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // todo: could enforce various combinations here..
}

MSNUnitSpec::GateType MSNUnitSpec::MatrixGateType(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  LeabraLayer* lay = (LeabraLayer*)un->own_lay();
  int ugidx = un->UnitGpIdx();
  int x_pos = ugidx % lay->gp_geom.x;
  int half_x = lay->gp_geom.x / 2;
  if(x_pos < half_x)            // maint on left, out on right..
    return MAINT;
  else
    return OUT;
}

void MSNUnitSpec::Compute_ApplyInhib
(LeabraUnitVars* u, LeabraNetwork* net, int thr_no, LeabraLayerSpec* lspec,
 LeabraInhib* thr, float ival) {
  inherited::Compute_ApplyInhib(u, net, thr_no, lspec, thr, ival);

  if(matrix_patch != MATRIX) return;

  u->gc_i += u->misc_1;         // post-gating inhibition
  
  GateType gt = MatrixGateType(u, net, thr_no);
  if(gt == MAINT) {
    if(u->ach > 0.0f) {
      u->gc_i += matrix.mnt_ach_inhib;
    }
  }
  else { // OUT
    u->gc_i += matrix.out_ach_inhib * (1.0f - u->ach);
  }
}

void MSNUnitSpec::SaveGatingThal(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!Quarter_DeepRawNextQtr(net->quarter))
    return;

  int qtr_cyc;
  int gate_cyc = PFCUnitSpec::PFCGatingCycle(net, true, qtr_cyc); // get earliest value
  
  if(qtr_cyc == gate_cyc) {
    u->thal_cnt = u->thal;      // save into thal_cnt!

    // compute learning trace!
    const int nrg = u->NRecvConGps(net, thr_no); 
    for(int g=0; g< nrg; g++) {
      LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
      if(recv_gp->NotActive()) continue;
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(!cs->InheritsFrom(&TA_MSNConSpec)) continue;
      MSNConSpec* mcs = (MSNConSpec*)cs;
      if(mcs->learn_rule != MSNConSpec::TRACE_THAL_SEP) continue;
      mcs->Compute_Trace_Thal(recv_gp, net, thr_no);
    }
  }
}

void MSNUnitSpec::Compute_Act_Post(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Post(u, net, thr_no);
  if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
    SaveGatingThal(u, net, thr_no);
  }
}

void MSNUnitSpec::Compute_DeepMod(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
    LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
    LeabraLayer* lay = (LeabraLayer*)un->own_lay();
    float dp_lrn = 1.0f;
    float dp_mod = 1.0f;
    if(lay->am_deep_mod_net.max > 0.1f) {
      dp_lrn = u->deep_mod_net / lay->am_deep_mod_net.max;
      dp_mod = deep.mod_min + deep.mod_range * dp_lrn;
    }
    GateType gt = MatrixGateType(u, net, thr_no);
    if(gt == MAINT) {
      if(matrix.mnt_deep_mod) {
        u->deep_lrn = dp_lrn;
        u->deep_mod = dp_mod;
      }
      else {
        u->deep_lrn = u->deep_mod = 1.0f;         // everybody gets 100%
      }
    }
    else { // gt == OUT
      if(matrix.out_deep_mod) {
        u->deep_lrn = dp_lrn;
        u->deep_mod = dp_mod;
      }
      else {
        u->deep_lrn = u->deep_mod = 1.0f;         // everybody gets 100%
      }
    }
  }
  else if(dorsal_ventral == VENTRAL && matrix_patch == MATRIX) {
    LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
    if(deep.SendDeepMod()) {
      u->deep_lrn = u->deep_mod = u->act;      // record what we send!
    }
    else if(deep.IsTRC()) {
      u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
    }
    // must be SUPER units at this point
    else if(lay->am_deep_mod_net.max < 0.01f) { // not enough yet // was .1f
      u->deep_lrn = 0.0f;    // default is 0!
      u->deep_mod = 1.0f;    // TODO: hoping this is the magic guy that bootstraps, but doesn't make totally dependent
    }
    else {
      u->deep_lrn = u->deep_mod_net / lay->am_deep_mod_net.max; // todo: could not normalize this..
      u->deep_mod = 1.0f;                               // do not modulate with deep_mod!
    }
  }
  else { // must be VENTRAL, PATCH
    inherited::Compute_DeepMod(u, net, thr_no); // use D1D2 one
  }
}

void MSNUnitSpec::Compute_DeepStateUpdt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  inherited::Compute_DeepStateUpdt(u, net, thr_no);
  if(matrix_patch != MATRIX) return;

  if(u->thal > 0.0f) {          // we gated
    GateType gt = MatrixGateType(u, net, thr_no);
    if(gt == MAINT) {
      u->misc_1 += matrix.mnt_gate_inhib * u->net_prv_q; // keep the net, not the inhib!
    }
    else {
      u->misc_1 += matrix.out_gate_inhib * u->net_prv_q; // keep the net, not the inhib!
    }
  }
  else {
    u->misc_1 -= matrix.gate_i_dt * u->misc_1; // decay
  }
}

