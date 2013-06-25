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

#include "ActrImaginalModule.h"
#include <ActrModel>


void ActrImaginalModule::Initialize() {
  imaginal_delay.type = Random::NONE;
  imaginal_delay.mean = 0.2f;
  imaginal_delay.var = 0.0f;
}

void ActrImaginalModule::InitModule() {
  if((bool)buffer) return;
  if(!Model()) return;
  ActrModel* mod = Model();
  bool made_new;
  buffer = mod->buffers.FindMakeNameType("imaginal", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  buffer->ClearBufferFlag(ActrBuffer::STRICT_HARVEST); // imaginal doesn't do this!
  if(made_new) {
    buffer->act_total = 0.0f;   // imaginal-activation
  }

  action_buffer = mod->buffers.FindMakeNameType("imaginal-action", NULL, made_new);
  action_buffer->module = this;
  action_buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  action_buffer->ClearBufferFlag(ActrBuffer::STRICT_HARVEST); // imaginal doesn't do this!
  if(made_new) {
    action_buffer->act_total = 0.0f;   // imaginal-activation
  }
}

void ActrImaginalModule::Init() {
  inherited::Init();
  buffer->UpdateState();
}

void ActrImaginalModule::ProcessEvent(ActrEvent& event) {
  if(event.action == "MODULE-REQUEST") {
    ImaginalRequest(event);
  }
  else if(event.action == "MOD-BUFFER-CHUNK") {
    ImaginalBufferMod(event);
  }
  else if(event.action == "CREATE-NEW-BUFFER-CHUNK") {
    CreateNewChunk(event);
  }
  else if(event.action == "SET-BUFFER-CHUNK") {
    SetBufferChunk(event);
  }
  else if(event.action == "MOD-IMAGINAL-CHUNK") {
    ModImaginalChunk(event);
  }
  else {
    ProcessEvent_std(event);   // respond to regular requests
  }
}

void ActrImaginalModule::ImaginalRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck, "ImaginalRequest", "chunk is NULL")) {
    return;
  }

  ActrModel* mod = Model();

  if(HasModuleFlag(BUSY)) {
    TestWarning(true, "ImaginalRequest",
                "a imaginal request was made while still busy activating previous request -- new request ignored");
    mod->LogEvent(-1.0f, "imaginal", "ABORT-IMAGINAL-REQ", "", "");
    return;
  }

  mod->LogEvent(-1.0f, "imaginal", "START-IMAGINAL-REQ", "", "");
  SetModuleFlag(BUSY);
  ClearModuleFlag(ERROR);
  buffer->SetReq();
  buffer->ClearChunk();         // always clear before imaging new
  mod->ScheduleEvent(imaginal_delay.Gen(), ActrEvent::max_pri, this, this, buffer,
                     "CREATE-NEW-BUFFER-CHUNK", "", event.act_arg,
                     ck);
}

void ActrImaginalModule::CreateNewChunk(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck, "CreateNewChunk", "chunk is NULL")) {
    return;
  }
  ActrChunk* nw_ck = (ActrChunk*)ck->Clone();
  nw_ck->CopyName(ck);
  ActrModel* mod = Model();
  mod->ScheduleEvent(0.0f, ActrEvent::max_pri, this, this, buffer,
                     "SET-BUFFER-CHUNK", "", event.act_arg,
                     nw_ck);
}

void ActrImaginalModule::SetBufferChunk(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck, "CreateNewChunk", "chunk is NULL")) {
    return;
  }
  buffer->UpdateChunk(ck);
  ClearModuleFlag(BUSY);
  buffer->ClearReq();
}

void ActrImaginalModule::ImaginalBufferMod(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck, "ImaginalBufferMod", "chunk is NULL")) {
    return;
  }

  ActrModel* mod = Model();

  if(HasModuleFlag(BUSY)) {
    TestWarning(true, "ImaginalBufferMod",
                "a imaginal modification was made while still busy activating previous -- new modification ignored");
    mod->LogEvent(-1.0f, "imaginal", "ABORT-IMAGINAL-MOD", "", "");
    return;
  }

  mod->LogEvent(-1.0f, "imaginal", "START-IMAGINAL-MOD", "", "");
  SetModuleFlag(BUSY);
  ClearModuleFlag(ERROR);
  mod->ScheduleEvent(imaginal_delay.Gen(), ActrEvent::max_pri, this, this, buffer,
                     "MOD-IMAGINAL-CHUNK", "", event.act_arg,
                     ck);
}

void ActrImaginalModule::ModImaginalChunk(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck, "ModImaginalChunk", "chunk is NULL")) {
    return;
  }
  buffer->UpdateChunk(ck);
  ClearModuleFlag(BUSY);
}

