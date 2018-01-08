// Copyright 2016-2017, Regents of the University of Colorado,
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
  last_epoch_set = -1;
}

void ParamSeq::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(prev_name.nonempty() && prev_name != name) {
    for(int i=0; i<steps.size; i++) {
      ParamStep* ps = (ParamStep*)steps[i];
      ps->AutoName();
    }
  }
  prev_name = name;
  steps.SetName(name + "_steps");
  steps.Sort();                 // should sort by name!
}

bool ParamSeq::SetParamsAtEpoch(int epoch) {
  bool got_some = false;
  for(int i=0; i<steps.size; i++) {
    ParamStep* ps = (ParamStep*)steps[i];
    // if we somehow jumped over our epoch, set it!
    if(ps->epoch == epoch || (last_epoch_set < ps->epoch && epoch > ps->epoch)) {
      last_epoch_set = epoch;
      got_some = true;
      if(verbose) {
        FOREACH_ELEM_IN_GROUP(ControlPanelMember, sei, ps->mbrs) {
          String cur_val = sei->CurValAsString();
          sei->CopySavedToActive();
          String new_val = sei->CurValAsString();
          taMisc::Info("ParamStep:", ps->name, "at epoch: " + String(epoch), "set:",
                       sei->label, "to:", new_val, "was:", cur_val);
        }
        ps->ReShowEdit(true);
      }
      else {
        ps->CopySavedToActive();
      }
    }
  }
  return got_some;
}

void ParamSeq::MakeEpochSteps(int epcs_per_step, int n_steps, bool copy_first) {
  if(TestError(n_steps <= 1, "MakeEpochSteps",
               "requires at least 2 steps")) {
    return;
  }
  int epc = 0;
  steps.SetSize(n_steps);
  ParamStep* fs = (ParamStep*)steps[0];
  for(int i=0; i<steps.size; i++) {
    ParamStep* ps = (ParamStep*)steps[i];
    if(copy_first && i >= 1) {
      ps->CopyFrom(fs);
    }      
    ps->SetEpoch(epc);
    epc += epcs_per_step;
  }
  SigEmitUpdated();
}

void ParamSeq::LinearInterp() {
  if(TestError(steps.size < 3, "LinearInterp",
               "requires at least 3 steps to perform linear interpolation")) {
    return;
  }
  ParamStep* fs = (ParamStep*)steps[0];
  ParamStep* ls = (ParamStep*)steps.Peek();
  for(int i=1; i<steps.size-1; i++) {
    ParamStep* ps = (ParamStep*)steps[i];
    for(int lfi = 0; lfi < ps->mbrs.leaves; lfi++) {
      ControlPanelMember* psi = ps->mbrs.Leaf(lfi);
      if(!(psi->data.is_numeric && psi->data.is_single)) continue;
      ControlPanelMember* fsi = fs->mbrs.Leaf(lfi);
      ControlPanelMember* lsi = ls->mbrs.Leaf(lfi);
      if(TestError(!fsi || !lsi, "LinearInterp",
                   "corresponding first and/or last step items not found for item:",
                   psi->label, "aborting")) {
        return;
      }
      double fval = fsi->data.saved_value.toDouble();
      double lval = lsi->data.saved_value.toDouble();
      double ival = fval + (double)i * ((lval - fval) / (double)(steps.size-1));
      psi->data.saved_value = (String)ival;
    }
  }
  SigEmitUpdated();
}
