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

#include "ActrProduction.h"
#include <ActrModule>

void ActrProduction::Initialize() {
  util = 0.0f;
  rew = 0.0f;
}

int ActrProduction::GetEnabled() const {
  return !off;
}

void ActrProduction::SetEnabled(bool value) {
  off = !value;
}

bool ActrProduction::Matches() {
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    if(!cnd->Matches()) return false; // only takes one
  }
  return true;
}

String ActrProduction::WhyNot() {
  String rval = "Why production: " + name + " did not match:\n";
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    rval += cnd->WhyNot();
  }
  return rval;
}

void ActrProduction::SendBufferReads(ActrProceduralModule* proc_mod, ActrModel* model) {
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    cnd->SendBufferReads(proc_mod, model);
  }
}
