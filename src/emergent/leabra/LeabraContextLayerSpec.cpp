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

#include "LeabraContextLayerSpec.h"
#include <LeabraNetwork>
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(CtxtUpdateSpec);

TA_BASEFUNS_CTORS_DEFN(CtxtNSpec);

TA_BASEFUNS_CTORS_DEFN(LeabraContextLayerSpec);

void CtxtUpdateSpec::Initialize() {
  fm_hid = 1.0f;
  fm_prv = 0.0f;
  to_out = 1.0f;
}

const String
LeabraContextLayerSpec::do_update_key("LeabraContextLayerSpec__do_update");

void LeabraContextLayerSpec::Initialize() {
  updt.fm_prv = 0.0f;
  updt.fm_hid = 1.0f;
  updt.to_out = 1.0f;
  SetUnique("decay", true);
  update_criteria = UC_TRIAL;
  Defaults_init();
}

void LeabraContextLayerSpec::Defaults_init() {
  decay.trial = 0.0f;
}

// void LeabraContextLayerSpec::UpdateAfterEdit-impl() {
//   inherited::UpdateAfterEdit_impl();
//   hysteresis_c = 1.0f - hysteresis;
// }

bool LeabraContextLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

//   LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  return rval;
}

taBase::DumpQueryResult LeabraContextLayerSpec::Dump_QuerySaveMember(MemberDef* md) {
  // only save n_spec if needed (to ease backwards compat)
  if (md->name != "n_spec")
    return inherited::Dump_QuerySaveMember(md);
  return (update_criteria == UC_N_TRIAL) ? DQR_SAVE : DQR_NO_SAVE;
}

void LeabraContextLayerSpec::Compute_Context(LeabraLayer* lay, LeabraUnit* u,
                                             LeabraNetwork* net, int thr_no) {
  LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    uv->ext = uv->act_m;          // just use previous minus phase value!
  }
  else {
    LeabraConGroup* cg = (LeabraConGroup*)uv->RecvConGroupSafe(net, thr_no, 0);
    if(TestError(!cg || cg->size == 0, "Compute_Context", "requires one recv projection with at least one unit!")) {
      return;
    }
    LeabraUnitVars* su = (LeabraUnitVars*)cg->UnVars(0, net);
    uv->ext = updt.fm_prv * uv->act_p + updt.fm_hid * su->act_p; // compute new value
  }
  uv->SetExtFlag(UnitVars::EXT);
  ((LeabraUnitSpec*)uv->unit_spec)->Compute_HardClamp(uv, net, thr_no);
}

void LeabraContextLayerSpec::Compute_HardClamp_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  lay->hard_clamped = true;     // cache this flag
  lay->SetExtFlag(UnitVars::EXT);
  bool do_update = lay->GetUserDataDef(do_update_key, false).toBool();
  if (do_update) {
    lay->SetUserData(do_update_key, false); // reset
  }
  else { // not explicit triger, so try other conditions
    switch (update_criteria) {
    case UC_TRIAL:
      do_update = true;
      break;
    case UC_MANUAL: break; // weren't triggered, so that's it
    case UC_N_TRIAL: {
      // do modulo the trial, adding offset -- add 1 so first trial is not trigger
      do_update = (((net->trial + n_spec.n_offs + 1) % n_spec.n_trials) == 0);
    } break;
    }
  }
  if (!do_update) return;

  // lay->Inhib_SetVals(inhib.kwta_pt);            // assume 0 - 1 clamped inputs

  
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    Compute_Context(lay, u, net, u->ThrNo());
  }
}

void LeabraContextLayerSpec::TriggerUpdate(LeabraLayer* lay) {
  if (!lay) return;
  if (TestError((lay->spec.spec.ptr() != this),
    "TriggerUpdate", "Spec does not belong to the layer passed as arg"))
    return;
  lay->SetUserData(do_update_key, true);
}

