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

void MatrixConSpec::Initialize() {
  min_obj_type = &TA_MatrixCon;

  immed_trace = false;
  mnt_decay = 1.0f;
  no_mnt_decay = 1.0f;

  Defaults_init();
}

void MatrixConSpec::Defaults_init() {
  // SetUnique("wt_limits", true);
  wt_limits.sym = false;

  // SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  // SetUnique("wt_sig", true);
  wt_sig.gain = 6.0f;
  wt_sig.off = 1.0f;
  
  ignore_unlearnable = false;
}

void MatrixConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
  //  lmix.err_sb = false;
}

