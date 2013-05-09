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

#include "ActrProceduralModule.h"
#include <ActrModel>

void ActrProceduralModule::Initialize() {
  mp_time = 0.05f;
  util_lrate = 0.02f;
  util_noise = 0.02f;
}

void ActrProceduralModule::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  productions.CheckConfig(quiet, rval);
}

void ActrProceduralModule::InitModule() {
  if((bool)buffer) return;
  if(!Model()) return;
  ActrModel* mod = Model();
  bool made_new;
  // note: our buffer isn't actually used?
  buffer = mod->buffers.FindMakeNameType("procedural", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
}

void ActrProceduralModule::ProcessEvent(ActrEvent& event) {
  if(event.action == "CONFLICT-RESOLUTION") {
    ConflictResolution();
  }
  else if(event.action == "PRODUCTION-FIRED") {
    ProductionFired();
  }
}

void ActrProceduralModule::Init() {
  inherited::Init();
  eligible.Reset();
  fired = NULL;
  FOREACH_ELEM_IN_GROUP(ActrProduction, pr, productions) {
    if(pr->IsOff()) continue;
    pr->Init();
  }
  SigEmitUpdated();
}

void ActrProceduralModule::AddConflictResEvent() {
  Model()->ScheduleEvent(0.0f, ActrEvent::max_pri, this, this, NULL,
                         "CONFLICT-RESOLUTION", "");
}

void ActrProceduralModule::ConflictResolution() {
  // todo: is state busy at this time??
  ActrModel* mod = Model();
  eligible.Reset();
  FOREACH_ELEM_IN_GROUP(ActrProduction, pr, productions) {
    if(pr->IsOff()) continue;
    bool updt = false;
    if(mod->UpdateGui()) {
      if(pr->HasProdFlag(ActrProduction::FIRED) || 
         pr->HasProdFlag(ActrProduction::ELIGIBLE))
        updt = true;
    }
    pr->ClearProdFlag(ActrProduction::FIRED);
    pr->ClearProdFlag(ActrProduction::ELIGIBLE);
    if(pr->Matches()) {
      eligible.Link(pr);
      pr->SetProdFlag(ActrProduction::ELIGIBLE);
    }
    if(updt && mod->UpdateGui()) {
      pr->SigEmitUpdated();
    }
  }
  if(TestError(eligible.size == 0, "ConflictResolution",
               "No matching productions found!")) {
    mod->Stop();
    return;
  }
  if(eligible.size == 1) {
    fired = eligible.FastEl(0);
  }
  else {
    // todo: do utility-based selection of top guy
    fired = eligible.FastEl(0); // just pick the first one for now!!
    TestWarning(true, "ConflictResolution",
                "utility-based selection not avail yet, using first element of:",
                (String)eligible.size, "that matched");
  }
  // todo: this appears to be an optional extra level of debug:
  fired->SetProdFlag(ActrProduction::FIRED);
  if(mod->UpdateGui()) {
    fired->SigEmitUpdated();
  }
  mod->LogEvent(-1.0f, "procedural", "PRODUCTION-SELECTED", "", fired->name);
  fired->SendBufferReads(this, mod);
  // send all the BUFFER-READ-ACTION's -- clear buffers!?
  // todo: support random jitter in timing
  mod->ScheduleEvent(mp_time, ActrEvent::max_pri, this, this, NULL,
                     "PRODUCTION-FIRED", fired->name);
}

void ActrProceduralModule::ProductionFired() {
  // now we process the actions!
  // todo: is state busy at this time??
  ActrModel* mod = Model();
  if(TestError(!fired, "ProductionFired",
               "Oooops -- production was reset somehow!")) {
    mod->Stop();
    return;
  }
  // todo: could double-check name given during action..
  fired->DoActions(this, mod);
}
