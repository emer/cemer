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

#include "ParamSeq.h"
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ParamSeq);

void ParamSeq::Initialize() {
  on = true;
  verbose = true;
}

void ParamSeq::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(prev_name.nonempty() && prev_name != name) {
    for(int i=0; i<steps.size; i++) {
      ParamStep* ps = steps[i];
      ps->AutoName();
    }
  }
  prev_name = name;
}

void ParamSeq::SetParamsAtEpoch(int epoch) {
  for(int i=0; i<steps.size; i++) {
    ParamStep* ps = steps[i];
    if(ps->epoch == epoch) {
      if(verbose) {
        FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, ps->mbrs) {
          String cur_val = sei->CurValAsString();
          sei->CopySavedToActive();
          String new_val = sei->CurValAsString();
          taMisc::Info("ParamStep:", ps->name, "set:", sei->label,
                       "to:", new_val, "was:", cur_val);
        }
        ps->ReShowEdit(true);
      }
      else {
        ps->CopySavedToActive();
      }
    }
  }
}
