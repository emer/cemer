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

#include "ActrModel.h"

#include <ActrDeclarativeModule>
#include <ActrProceduralModule>
#include <ActrGoalModule>

#include <taMisc>

void ActrModel::Initialize() {
  cur_time = 0.0f;
  cur_event_idx = 0;
  flags = MF_NONE;
  run_state = NOT_INIT;
}

void ActrModel::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  DefaultConfig();
}

ActrModule* ActrModel::FindMakeModule(const String& nm, TypeDef* td,
                                      bool& made_new) {
  ActrModule* rval = modules.FindMakeNameType(nm, td, made_new);
  if(made_new) {
    rval->InitModule();
  }
  return rval;
}

void ActrModel::DefaultConfig() {
  bool made_new;
  FindMakeModule("procedural", &TA_ActrProceduralModule, made_new);
  FindMakeModule("declarative", &TA_ActrDeclarativeModule, made_new);
  FindMakeModule("goal", &TA_ActrGoalModule, made_new);
}

void ActrModel::Init() {
  cur_event_idx = 0;
  events.Reset();
  cur_time = 0.0f;
  run_state = DONE;
  for(int i=0; i<modules.size; i++) {
    modules.FastEl(i)->Init();  // module-specific run init
  }
}

void ActrModel::Step() {
  if(run_state == NOT_INIT || run_state == DONE) {
    Init();
  }
  RunNextEvent();
}
 
void ActrModel::Run() {
  if(run_state == NOT_INIT || run_state == DONE) {
    Init();
  }
  while(run_state != STOP) {
    RunNextEvent();
    taMisc::ProcessEvents();    // todo: optimize this!
  }
}

void ActrModel::Stop() {
  run_state = STOP;
}

void ActrModel::RunNextEvent() {
  if(cur_event_idx >= events.size) {
    cur_event_idx = events.size; // make sure..
    // no more events!
    ActrProceduralModule* proc = (ActrProceduralModule*)modules.FindName("procedural");
    if(!proc) {                 // should not happen
      Stop();
      return;
    }
    proc->AddConflictResEvent(); // if nothing else, schedule some conflict resolution!!
  }
  ActrEvent* ev = events.SafeEl(cur_event_idx++); // should definitely work now..
  if(!ev || ev->action == "!Stop!") {
    Stop();
    return;
  }
  if(TestError(!ev->dst_module, "RunNextEvent",
               "no destination module in event!  programmer error!")) {
    Stop();
    return;
  }

  cur_time = ev->time;          // set current time to event time
  // todo: log event before it is sent
  ev->dst_module->ProcessEvent(*ev);

  if(!HasModelFlag(SAVE_ALL_EVENTS)) {
    if(cur_event_idx >= events.size) { // cull if we run out
      events.Reset();
      cur_event_idx = 0;
    }
    else if(cur_event_idx > 100) {
      // todo: copy remaining items to a tmp list, then reset and move back
      // write a separate function to do this
    }
  }
}

int ActrModel::InsertEventInOrder(ActrEvent* ev) {
  for(int i=cur_event_idx; i<events.size; i++) {
    ActrEvent* oe = events.FastEl(i);
    if(ev->time > oe->time) continue;
    if(ev->priority < oe->priority) continue;
    events.Insert(ev, i);              // this is our spot
    return i;
  }
  events.Add(ev);
  return events.size-1;
}

ActrEvent* ActrModel::ScheduleEvent(float time_fm_now, int priority,
                                    ActrModule* src_mod, ActrModule* dst_mod,
                                    ActrBuffer* dst_buf, const String& action,
                                    const String& params,
                                    ActrAction* act, ActrChunk* chnk,
                                    TypeDef* event_type) {
  ActrEvent* ev = ActrEvent::NewEvent(cur_time + time_fm_now, priority,
                                      src_mod, dst_mod, dst_buf, action,
                                      params, act, chnk, event_type);
  InsertEventInOrder(ev);
  return ev;
}
