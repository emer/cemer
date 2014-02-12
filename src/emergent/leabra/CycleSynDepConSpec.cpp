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

#include "CycleSynDepConSpec.h"

TA_BASEFUNS_CTORS_DEFN(CycleSynDepSpec);

TA_BASEFUNS_CTORS_DEFN(CycleSynDepConSpec);

void CycleSynDepSpec::Initialize() {
  rec = 0.002f;
  asymp_act = 0.4f;
  depl = rec * (1.0f - asymp_act); // here the drive is constant
}

void CycleSynDepSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(rec < .00001f) rec = .00001f;
  // chg = rec * (1 - cur) - dep * drive = 0; // equilibrium point
  // rec * (1 - cur) = dep * drive
  // dep = rec * (1 - cur) / drive
  depl = rec * (1.0f - asymp_act); // here the drive is constant
  depl = MAX(depl, 0.0f);
}

void CycleSynDepConSpec::Initialize() {
  min_obj_type = &TA_CycleSynDepCon;
}

void CycleSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  syn_dep.UpdateAfterEdit_NoGui();
}

