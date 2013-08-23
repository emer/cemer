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
  if(event.action == "CONFLICT_RESOLUTION") {
    ConflictResolution();
  }
  else if(event.action == "PRODUCTION_FIRED") {
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
    if(pr->util.init == 0.0f) {
      pr->util.cur = util.init;
    }
    else {
      pr->util.cur = pr->util.init;
    }
    pr->util.choice = pr->util.cur;
  }
}

void ActrProceduralModule::AddConflictResEvent() {
  Model()->ScheduleEvent(0.0f, ActrEvent::max_pri, this, this, NULL,
                         "CONFLICT_RESOLUTION", "");
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
    if(pr->util.cur >= util.thresh) {
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
                      "util: " + String(pr->util.cur) + " < thresh");
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
  mod->LogEvent(-1.0f, "procedural", "PRODUCTION_SELECTED", fired->name);
  fired->SendBufferReads(this, mod);
  // send all the BUFFER_READ_ACTION's -- clear buffers!

  float ptime = mp_time.Gen();  // always generate a random number to keep seeds more predictable
  if(fired->time.act > 0.0f) {  // custom time
    ptime = fired->time.act;
  }
  mod->ScheduleEvent(ptime, ActrEvent::max_pri, this, this, NULL,
                     "PRODUCTION_FIRED", fired->name);
}

void ActrProceduralModule::ChooseFromEligible() {
  int best_idx = 0;
  float best_util = -1.0e6f;
  int_Array ties;
  ActrModel* mod = Model();
  for(int i=0; i<eligible.size; i++) {
    ActrProduction* pr = eligible.FastEl(i);
    if(!mod->params.enable_sub_symbolic) {
      pr->util.choice = 0.0f;   // will turn everything into a tie and engage that mech
    }
    else {
      pr->util.choice = pr->util.cur;
      if(util.noise > 0.0f) {
        // todo: still need to resolve correspondence of noise_eff relative to gauss_def function vs. act_r_noise function and s sigma
        pr->util.choice += util.noise_eff * taMath_double::gauss_dev();
      }
      if(trace_level == TRACE_ALL || trace_level == TRACE_ELIG) {
        mod->LogEvent(-1.0f, "procedural", "TRACE", pr->name,
                      "elig util: " + String(pr->util.choice));
      }
    }
    if(pr->util.choice > best_util) {
      ties.Reset();
      ties.Add(i);              // we always want us to be in the tie set
      best_idx = i;
      best_util = pr->util.choice;
    }
    else if(pr->util.choice == best_util) { // keep track of ties
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
  fired->time.last_fire = mod->cur_time;
  if(fired->util.rew != 0.0f) {
    ComputeReward(fired->util.rew);
  }
  ClearModuleFlag(BUSY);
}

void ActrProceduralModule::ComputeReward(float rew) {
  if(TestWarning(!util.learn, "ComputeReward",
                 "learning is turned off -- utilities will not be updated")) {
    return;
  }
  ActrModel* mod = Model();
  mod->LogEvent(-1.0f, "procedural", "COMPUTE_REWARD", "",
                "rew: " + String(rew));
  FOREACH_ELEM_IN_GROUP(ActrProduction, pr, productions) {
    if(pr->IsOff()) continue;
    if(pr->time.last_fire > last_rew_time) {
      pr->Compute_Util(rew, util.lrate);
      //       pr->util.cur += util.lrate * (rew - pr->util.cur);
      if(trace_level == UTIL_LEARN) {
        mod->LogEvent(-1.0f, "procedural", "TRACE", pr->name,
                      "learned util: " + String(pr->util.cur));
      }
    }
  }
  last_rew_time = mod->cur_time;
  last_rew = rew;
}

bool ActrProceduralModule::SetParam(const String& param_nm, Variant par1, Variant par2) {
  bool got = false;
  if(param_nm == "ul") {
    util.learn = par1.toBool();
    got = true;
  }
  else if(param_nm == "alpha") {
    util.lrate = par1.toFloat();
    got = true;
  }
  else if(param_nm == "iu") {
    util.init = par1.toFloat();
    got = true;
  }
  else if(param_nm == "egs") {
    util.noise = par1.toFloat();
    util.UpdateAfterEdit();
    got = true;
  }
  else if(param_nm == "ut") {
    util.thresh = par1.toFloat();
    got = true;
  }
  else if(param_nm == "crt") {
    trace_level = ActrProceduralModule::TRACE_ALL;
    got = true;
  }
  else if(param_nm == "cst") {
    trace_level = ActrProceduralModule::TRACE_ELIG;
    got = true;
  }
  else if(param_nm == "ult") {
    trace_level = ActrProceduralModule::UTIL_LEARN;
    got = true;
  }
  return got;
}
