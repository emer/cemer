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

#include "ActrDeclarativeModule.h"

#include <ActrModel>

void DeclarativeParams::Initialize() {

}


void ActrDeclarativeModule::Initialize() {

}

void ActrDeclarativeModule::InitModule() {
  if((bool)buffer) return;
  if(!Model()) return;
  ActrModel* mod = Model();
  bool made_new;
  buffer = mod->buffers.FindMakeNameType("retrieval", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
}

void ActrDeclarativeModule::Init() {
  inherited::Init();
  active.Reset();
  active.CopyFrom(&init_chunks);
}

void ActrDeclarativeModule::ProcessEvent(ActrEvent& event) {
  if(event.action == "MODULE-REQUEST") {
    RetrievalRequest(event);
  }
  else if(event.action == "RETRIEVED-CHUNK") {
    RetrievedChunk(event);
  }
  else if(event.action == "RETRIEVAL-FAILURE") {
    RetrievalFailure(event);
  }
  else {
    ProcessEvent_std(event);   // respond to regular requests
  }
}

void ActrDeclarativeModule::RetrievalRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck, "RetrievalRequest", "probe chunk is NULL")) {
    return;
  }

  // todo: what if currently busy
  // ;; If the module has not completed the last request
  // (when (dm-busy dm)
  //   ;; Report a warning about that and remove the unexecuted event 
  //   ;; from the queue.
  //   (model-warning "A retrieval event has been aborted by a new request")
  //   (delete-event (dm-busy dm)))

  ActrModel* mod = Model();

  if(HasModuleFlag(BUSY)) {
    TestWarning(true, "RetrievalRequest",
                "a retrieval request was made while still busy retrieving previous request -- new request should preempt but currently new request is rejected");
    mod->LogEvent(-1.0f, "declarative", "ABORT-RETRIEVAL", "", "");
    return;
  }

  mod->LogEvent(-1.0f, "declarative", "START-RETRIEVAL", "", "");
  SetModuleFlag(BUSY);
  ClearModuleFlag(ERROR);
  buffer->SetReq();
  buffer->ClearChunk();         // always clear before recall
  bool got_some = FindMatching(ck);
  if(!got_some) {
    // todo: model retrieval time
    mod->ScheduleEvent(0.05f, ActrEvent::max_pri, this, this, buffer,
                       "RETRIEVAL-FAILURE", "", event.act_arg);
    return;
  }
  if(eligible.size == 1) {
    retrieved = eligible.FastEl(0);
  }
  else {
    // todo: do utility-based selection of top guy
    retrieved = eligible.FastEl(0); // just pick the first one for now!!
    TestWarning(true, "RetrievalRequest",
                "activity-based selection not avail yet, using first element of:",
                (String)eligible.size, "that matched");
  }
  // todo: model retrieval time
  retrieved->SetChunkFlag(ActrChunk::RETRIEVED);
  if(mod->UpdateGui()) {
    retrieved->SigEmitUpdated();
  }
  mod->ScheduleEvent(0.05f, ActrEvent::max_pri, this, this, buffer,
                     "RETRIEVED-CHUNK", retrieved->name, event.act_arg,
                     retrieved);
}

void ActrDeclarativeModule::RetrievedChunk(ActrEvent& event) {
  buffer->UpdateChunk(retrieved); // should be clear..
  ClearModuleFlag(BUSY);
  buffer->ClearReq();
}

void ActrDeclarativeModule::RetrievalFailure(ActrEvent& event) {
  ClearModuleFlag(BUSY);
  SetModuleFlag(ERROR);         // todo: when are errors cleared??
  buffer->ClearReq();
}

bool ActrDeclarativeModule::AddChunk(ActrChunk* ck, bool merge) {
  if(merge) {
    bool match = FindMatching(ck);
    if(!match || eligible.size > 1) { // none or too many
      eligible.Reset();
      AddChunk(ck, false);
      return false;             // not merged
    }
    // todo: what to do to merge values??
    ActrChunk* mck = eligible[0];
    UpdateBaseAct(mck);
  }
  else {
    active.Add(ck);             // todo: transfer?  some issues of ref counting here
    UpdateBaseAct(ck);
  }
  return true;
}

bool ActrDeclarativeModule::FindMatching(ActrChunk* ck) {
  ActrModel* mod = Model();
  eligible.Reset();
  for(int i=0; i<active.size; i++) {
    ActrChunk* oc = active.FastEl(i);
    bool updt = false;
    if(mod->UpdateGui()) {
      if(oc->HasChunkFlag(ActrChunk::ELIGIBLE) ||
         oc->HasChunkFlag(ActrChunk::RETRIEVED))
        updt = true;
    }
    oc->ClearChunkFlag(ActrChunk::ELIGIBLE);
    oc->ClearChunkFlag(ActrChunk::RETRIEVED);
    if(ck->MatchesMem(oc, false)) { // false = not exact..
      eligible.Link(oc);
      oc->SetChunkFlag(ActrChunk::ELIGIBLE);
    }
    if(updt && mod->UpdateGui()) {
      oc->SigEmitUpdated();
    }
  }
  return (eligible.size > 0);
}

float ActrDeclarativeModule::UpdateBaseAct(ActrChunk* ck) {
  // todo: write code for this
  return 0.5f;
}

