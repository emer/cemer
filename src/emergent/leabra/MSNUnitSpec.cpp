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

TA_BASEFUNS_CTORS_DEFN(MatrixActSpec);
TA_BASEFUNS_CTORS_DEFN(MSNUnitSpec);

void MatrixActSpec::Initialize() {
  Defaults_init();
}

void MatrixActSpec::Defaults_init() {
  out_ach_inhib = 0.0f;
  mnt_ach_inhib = 0.0f;
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
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
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

  GateType gt = MatrixGateType(u, net, thr_no);
  if(gt == MAINT) {
    u->gc_i += matrix.mnt_ach_inhib * u->ach;
  }
  else { // OUT
    u->gc_i += matrix.out_ach_inhib * (1.0f - u->ach);
  }
}
