// Copyright 2017, Regents of the University of Colorado,
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
  n_mnt_x = -1;
  Defaults_init();
}

void MatrixActSpec::Defaults_init() {
  patch_shunt = 0.2f;
  shunt_ach = true;
  out_ach_inhib = 0.3f;
}

void MSNUnitSpec::Initialize() {
  dar = D1R;
  matrix_patch = MATRIX;
  dorsal_ventral = DORSAL;
  valence = APPETITIVE;
}

void MSNUnitSpec::Defaults_init() {
  deep_mod_zero = true;
}

void MSNUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // todo: could enforce various combinations here..
}

MSNUnitSpec::GateType MSNUnitSpec::MatrixGateType(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  LeabraLayer* lay = (LeabraLayer*)un->own_lay();
  int ugidx = un->UnitGpIdx();
  int x_pos = ugidx % lay->gp_geom.x;
  if(matrix.n_mnt_x < 0) {
    int half_x = lay->gp_geom.x / 2;
    if(x_pos < half_x)            // maint on left, out on right..
      return MAINT;
    else
      return OUT;
  }
  else {
    if(x_pos < matrix.n_mnt_x)
      return MAINT;
    else
      return OUT;
  }
}

void MSNUnitSpec::Compute_ApplyInhib
(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no, LeabraLayer* lay,
 LeabraLayerSpec* lspec, LeabraInhib* thr, float ival) {
  inherited::Compute_ApplyInhib(u, net, thr_no, lay, lspec, thr, ival);

  // note shunting previously applied
  if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
    GateType gt = MatrixGateType(u, net, thr_no);
    if(gt == OUT) {
      u->gc_i += matrix.out_ach_inhib * (1.0f - u->ach);
    }
  }
}


void MSNUnitSpec::Compute_PatchShunt(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  // note: recv this in prior Act_Post from  patch unit spec, apply in Act
  if(u->shunt > 0.0f) {         // todo: could be more quantitative here..
    u->da_p *= matrix.patch_shunt;
    if(matrix.shunt_ach) {
      u->ach *= matrix.patch_shunt;
    }
  }
}

void MSNUnitSpec::SaveGatingThal(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  // if(!Quarter_DeepRawNextQtr(net->quarter))
  //   return;
  // int qtr_cyc;
  // int gate_cyc = PFCUnitSpec::PFCGatingCycle(net, true, qtr_cyc); // get earliest value
  // if(qtr_cyc == gate_cyc) {

  // save gating value into thal_cnt and gated activation into act_g when thal_gate indicates gating -- note 1 trial delayed from actual gating -- updated in computed_act *prior* to computing new act, so it reflects actual gating cycle activation
  if(u->thal_gate > 0.0f) {
    u->thal_cnt = u->thal;
    u->act_g = GetRecAct(u);       // todo: experiment with learning based on this!
  }
}

void MSNUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  // note: critical for this to come BEFORE updating new act!
  if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
    Compute_PatchShunt(u, net, thr_no);
    SaveGatingThal(u, net, thr_no);
  }
  // note: ApplyInhib called here:
  inherited::Compute_Act_Rate(u, net, thr_no);
}

void MSNUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  // note: critical for this to come BEFORE updating new act!
  if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
    Compute_PatchShunt(u, net, thr_no);
    SaveGatingThal(u, net, thr_no);
  }
  inherited::Compute_Act_Spike(u, net, thr_no);
}

void MSNUnitSpec::Compute_DeepMod(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  if(deep.SendDeepMod() || deep.IsTRC()) {
    inherited::Compute_DeepMod(u, net, thr_no);
    return;
  }
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay(); 
  // must be SUPER units at this point
  if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
    u->deep_lrn = u->deep_mod = 1.0f;         // everybody gets 100% -- not using deep
    return;
  }
  else if(dorsal_ventral == VENTRAL && matrix_patch == MATRIX) {
    // if(lay->am_deep_mod_net.max <= deep.mod_thr) { // not enough yet
    if(u->deep_mod_net <= deep.mod_thr) { // per-unit, NOT layer
      u->deep_lrn = 0.0f;    // default is 0!
      u->deep_mod = 1.0f;
    }
    else {
      u->deep_lrn = u->deep_mod_net / lay->am_deep_mod_net.max;
      u->deep_mod = 1.0f;
    }
  }
  else if(dorsal_ventral == VENTRAL && matrix_patch == PATCH) {
    // if(lay->am_deep_mod_net.max <= deep.mod_thr) { // not enough yet
    if(u->deep_mod_net <= deep.mod_thr) { // per-unit, NOT layer
      u->deep_lrn = 0.0f;    // default is 0!
      if(deep_mod_zero) {
        u->deep_mod = 0.0f;
      }
      else {
        u->deep_mod = 1.0f;
      }
    }
    else {
      u->deep_lrn = u->deep_mod_net / lay->am_deep_mod_net.max;
      u->deep_mod = 1.0f;       // don't modulate activation here..
    }
  }
  else {
    inherited::Compute_DeepMod(u, net, thr_no); // use D1D2 one
  }
}


