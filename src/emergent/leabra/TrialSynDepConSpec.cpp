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

#include "TrialSynDepConSpec.h"

TA_BASEFUNS_CTORS_DEFN(TrialSynDepSpec);

TA_BASEFUNS_CTORS_DEFN(TrialSynDepConSpec);

void TrialSynDepSpec::Initialize() {
  rec = 1.0f;
  depl = 1.1f;
}

void TrialSynDepConSpec::Initialize() {
  min_obj_type = &TA_TrialSynDepCon;
}

void TrialSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(syn_dep.rec <= 0.0f)       // can't go to zero!
    syn_dep.rec = 1.0f;
}

