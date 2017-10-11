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

#include "LeabraContextUnitSpec.h"
#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(CtxtUpdateSpec);
TA_BASEFUNS_CTORS_DEFN(CtxtNSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraContextUnitSpec);

void CtxtUpdateSpec::Initialize() {
  fm_hid = 1.0f;
  fm_prv = 0.0f;
  to_out = 1.0f;
}

const String
LeabraContextUnitSpec::do_update_key("LeabraContextUnitSpec__do_update");

void LeabraContextUnitSpec::Initialize() {
  updt.fm_prv = 0.0f;
  updt.fm_hid = 1.0f;
  updt.to_out = 1.0f;
  update_criteria = UC_TRIAL;
  Defaults_init();
}

void LeabraContextUnitSpec::Defaults_init() {
}

bool LeabraContextUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Unit(lay, quiet);

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(lay->units.leaves == 0) return rval;
  LeabraUnit* un = (LeabraUnit*)lay->units.Leaf(0); // take first one
  
  LeabraConState_cpp* cg = (LeabraConState_cpp*)un->RecvConStateSafe(0);
  if(lay->CheckError(!cg, quiet, rval,
                     "Requires one recv projection!")) {
    return false;
  }
  LeabraUnit* su = (LeabraUnit*)cg->SafeUn(0);
  if(lay->CheckError(!su, quiet, rval, 
                     "Requires one unit in recv projection!")) {
    return false;
  }

  return rval;
}

bool LeabraContextUnitSpec::ShouldUpdateNow(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  LeabraUnit* lu = (LeabraUnit*)u->Un(net, thr_no);
  LeabraLayer* lay = (LeabraLayer*)lu->own_lay();
  if(!lay) return false;
  bool do_update = lay->GetUserDataDef(do_update_key, false).toBool();
  if(!do_update) {              // check other criteria
    switch (update_criteria) {
    case UC_TRIAL:
      do_update = true;
      break;
    case UC_MANUAL:
      break; // weren't triggered, so that's it
    case UC_N_TRIAL: {
      // do modulo the trial, adding offset -- add 1 so first trial is not trigger
      do_update = (((net->trial + n_spec.n_offs + 1) % n_spec.n_trials) == 0);
    } break;
    }
  }
  return do_update;
}

void LeabraContextUnitSpec::TriggerUpdate(LeabraLayer* lay, bool update) {
  if (!lay) return;
  if (TestError((lay->unit_spec.spec.ptr() != this),
    "TriggerUpdate", "UnitSpec not set on the layer passed as arg -- must be"))
    return;
  lay->SetUserData(do_update_key, update, false); // false = no update
}

void LeabraContextUnitSpec::Compute_Context(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  LeabraConState_cpp* cg = (LeabraConState_cpp*)u->RecvConStateSafe(net, thr_no, 0);
  LeabraUnitState_cpp* su = (LeabraUnitState_cpp*)cg->UnState(0, net);
  LeabraLayer* fmlay = (LeabraLayer*)cg->prjn->from.ptr();
  if(fmlay->lesioned()) {
    u->act = 0.0f;
  }
  else if(net->cycle == 0) {
    bool up = ShouldUpdateNow(u, net, thr_no);
    if(up) {
      u->act = updt.fm_prv * u->act_q0 + updt.fm_hid * su->act_q0; // compute new value
    }
    else {
      u->act = u->act_q0;       // keep previous
    }
  }
  u->act_eq = u->act_nd = u->act;
  u->da = 0.0f;
  // u->AddToActBuf(syn_delay); // todo:

  if(deep.on && Quarter_DeepRawNow(net->quarter)) {
    Compute_DeepRaw(u, net, thr_no);
  }
}

void LeabraContextUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_Context(u, net, thr_no);
}

void LeabraContextUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_Context(u, net, thr_no);
}

