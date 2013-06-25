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
#include <taMath_double>

#include <taMisc>

void ActrUtilityParams::Initialize() {
  learn = false;
  lrate = 0.2f;
  init = 0.0f;
  noise = 0.0f;
  noise_eff = 0.0f;
  thresh = -1.0e6f;
}

void ActrUtilityParams::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  noise_eff = noise * 1.0f;     // todo: figure out correct conversion factor
}

void ActrProceduralModule::Initialize() {
  mp_time.type = Random::NONE;
  mp_time.mean = 0.05f;
  mp_time.var = 0.0f;
  trace_level = NO_TRACE;
  last_rew_time = 0.0f;
  last_rew = 0.0f;
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
  else {
    ProcessEvent_std(event);
  }
}

void ActrProceduralModule::Init() {
  inherited::Init();
  eligible.Reset();
  fired = NULL;
  last_rew_time = 0.0f;
  FOREACH_ELEM_IN_GROUP(ActrProduction, pr, productions) {
    if(pr->IsOff()) continue;
    pr->Init();
  }
  InitUtils();
  SigEmitUpdated();
}

void ActrProceduralModule::InitUtils() {
  FOREACH_ELEM_IN_GROUP(ActrProduction, pr, productions) {
    if(pr->IsOff()) continue;
    if(pr->init_util == 0.0f)
      pr->util = util.init;
    else
      pr->util = pr->init_util;
    pr->choice_util = pr->util;
  }
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
    if(pr->util >= util.thresh) {
      if(pr->Matches()) {
        eligible.Link(pr);
        pr->SetProdFlag(ActrProduction::ELIGIBLE);
        if(trace_level == TRACE_ALL) {
          mod->LogEvent(-1.0f, "procedural", "TRACE", pr->name,
                        "matched");
        }
      }
      else {
        if(trace_level == TRACE_ALL) {
          mod->LogEvent(-1.0f, "procedural", "TRACE", pr->name,
                        "did not match");
        }
      }
      if(updt && mod->UpdateGui()) {
        pr->SigEmitUpdated();
      }
    }
    else {
      if(trace_level == TRACE_ALL) {
        mod->LogEvent(-1.0f, "procedural", "TRACE", pr->name,
                      "util: " + String(pr->util) + " < thresh");
      }
    }
  }
  if(eligible.size == 0) {
    taMisc::Info("ConflictResolution: No matching productions found -- stopping");
    mod->Stop();
    return;
  }
  if(eligible.size == 1) {
    fired = eligible.FastEl(0);
  }
  else {
    ChooseFromEligible();
  }
  // todo: this appears to be an optional extra level of debug:
  fired->SetProdFlag(ActrProduction::FIRED);
  SetModuleFlag(BUSY);
  if(mod->UpdateGui()) {
    fired->SigEmitUpdated();
  }
  mod->LogEvent(-1.0f, "procedural", "PRODUCTION-SELECTED", fired->name);
  fired->SendBufferReads(this, mod);
  // send all the BUFFER-READ-ACTION's -- clear buffers!

  float ptime = mp_time.Gen();  // always generate a random number to keep seeds more predictable
  if(fired->act_time > 0.0f) {  // custom time
    ptime = fired->act_time;
  }
  mod->ScheduleEvent(ptime, ActrEvent::max_pri, this, this, NULL,
                     "PRODUCTION-FIRED", fired->name);
}

void ActrProceduralModule::ChooseFromEligible() {
  int best_idx = 0;
  float best_util = -1.0e6f;
  int_Array ties;
  ActrModel* mod = Model();
  for(int i=0; i<eligible.size; i++) {
    ActrProduction* pr = eligible.FastEl(i);
    if(!mod->params.enable_sub_symbolic) {
      pr->choice_util = 0.0f;   // will turn everything into a tie and engage that mech
    }
    else {
      pr->choice_util = pr->util;
      if(util.noise > 0.0f) {
        // todo: still need to resolve correspondence of noise_eff relative to gauss_def function vs. act_r_noise function and s sigma
        pr->choice_util += util.noise_eff * taMath_double::gauss_dev();
      }
      if(trace_level == TRACE_ALL || trace_level == TRACE_ELIG) {
        mod->LogEvent(-1.0f, "procedural", "TRACE", pr->name,
                      "elig util: " + String(pr->choice_util));
      }
    }
    if(pr->choice_util > best_util) {
      ties.Reset();
      ties.Add(i);              // we always want us to be in the tie set
      best_idx = i;
      best_util = pr->choice_util;
    }
    else if(pr->choice_util == best_util) { // keep track of ties
      ties.Add(i);
    }
  }
  if(ties.size > 1) {
    if(mod->params.enable_rnd) {
      int choose = Random::IntZeroN(ties.size);
      fired = eligible.FastEl(ties[choose]);
    }
    else {
      fired = eligible.FastEl(best_idx); // just choose first
    }
  }
  else {
    fired = eligible.FastEl(best_idx);
  }
}

void ActrProceduralModule::ProductionFired() {
  // now we process the actions!
  ActrModel* mod = Model();
  if(TestError(!fired, "ProductionFired",
               "Oooops -- production was reset somehow!")) { // shouldn't happen
    mod->Stop();
    return;
  }
  // todo: could double-check name given during action..
  fired->DoActions(this, mod);
  fired->last_fire_time = mod->cur_time;
  if(fired->rew != 0.0f) {
    ComputeReward(fired->rew);
  }
  ClearModuleFlag(BUSY);
}

void ActrProceduralModule::ComputeReward(float rew) {
  if(TestWarning(!util.learn, "ComputeReward",
                 "learning is turned off -- utilities will not be updated")) {
    return;
  }
  ActrModel* mod = Model();
  mod->LogEvent(-1.0f, "procedural", "COMPUTE-REWARD", "",
                "rew: " + String(rew));
  FOREACH_ELEM_IN_GROUP(ActrProduction, pr, productions) {
    if(pr->IsOff()) continue;
    if(pr->last_fire_time > last_rew_time) {
      pr->util += util.lrate * (rew - pr->util);
      if(trace_level == UTIL_LEARN) {
        mod->LogEvent(-1.0f, "procedural", "TRACE", pr->name,
                      "learned util: " + String(pr->util));
      }
    }
  }
  last_rew_time = mod->cur_time;
  last_rew = rew;
}
