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
  out_ach_inhib = 0.3f;
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
(LeabraUnitVars* u, LeabraNetwork* net, int thr_no, LeabraLayer* lay,
 LeabraLayerSpec* lspec, LeabraInhib* thr, float ival) {
  inherited::Compute_ApplyInhib(u, net, thr_no, lay, lspec, thr, ival);

  if(matrix_patch != MATRIX) return;

  GateType gt = MatrixGateType(u, net, thr_no);
  if(gt == OUT) {
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
    u->deep_lrn = u->deep_mod = 1.0f;         // everybody gets 100% -- not using deep
    // inherited::Compute_DeepMod(u, net, thr_no);
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


