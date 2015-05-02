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

#include "MatrixConSpec.h"

TA_BASEFUNS_CTORS_DEFN(MatrixLearnSpec);
TA_BASEFUNS_CTORS_DEFN(MatrixConSpec);

void MatrixLearnSpec::Initialize() {
  reset_any_da = true;
  use_thal = true;
  Defaults_init();
}

void MatrixLearnSpec::Defaults_init() {
  da_reset_tr = 0.2f;
  otr_lrate = 0.5f;
  tr_max = 10.0f;
  tr_decay = 0.0f;
}

void MatrixLearnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void MatrixConSpec::Initialize() {
  min_obj_type = &TA_MatrixCon;
  nogo = false;

  Defaults_init();
}

void MatrixConSpec::Defaults_init() {
  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  // SetUnique("lrate", true);
  lrate = 0.001f;

  learn_qtr = Q2_Q4;            // beta frequency default

  ignore_unlearnable = false;
}

void MatrixConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

