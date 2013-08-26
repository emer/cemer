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
    buffer->act_total = 0.0f;   // imaginal_activation
  }

  action_buffer = mod->buffers.FindMakeNameType("imaginal_action", NULL, made_new);
  action_buffer->module = this;
  action_buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  action_buffer->ClearBufferFlag(ActrBuffer::STRICT_HARVEST); // imaginal doesn't do this!
  if(made_new) {
    action_buffer->act_total = 0.0f;   // imaginal_activation
  }

  mod->DefineChunkTypeSys("generic_action", "", "action", "result");
  mod->DefineChunkTypeSys("simple_action", "", "action");
}

void ActrImaginalModule::Init() {
  inherited::Init();
  action_buffer->Init();
  buffer->UpdateState();
  action_buffer->UpdateState();
}

void ActrImaginalModule::ProcessEvent(ActrEvent& event) {
  if(event.action == "MODULE_REQUEST") {
    ImaginalRequest(event);
  }
  else if(event.action == "MOD_BUFFER_CHUNK") {
    ImaginalBufferMod(event);
  }
  else if(event.action == "CREATE_NEW_BUFFER_CHUNK") {
    CreateNewChunk(event);
  }
  else if(event.action == "MOD_IMAGINAL_CHUNK") {
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
    mod->LogEvent(-1.0f, "imaginal", "ABORT_IMAGINAL_REQ", "", "");
    return;
  }

  RequestBufferClear(event.dst_buffer); // always clear for any request

  // mod->LogEvent(-1.0f, "imaginal", "START_IMAGINAL_REQ", "", "");
  SetModuleFlag(BUSY);
  ClearModuleFlag(ERROR);
  buffer->SetReq();

  mod->ScheduleEvent(imaginal_delay.Gen(), ActrEvent::max_pri, this, this, buffer,
                     "CREATE_NEW_BUFFER_CHUNK", "", event.act_arg,
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
                     "SET_BUFFER_CHUNK", "", event.act_arg,
                     nw_ck);
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
    mod->LogEvent(-1.0f, "imaginal", "ABORT_IMAGINAL_MOD", "", "");
    return;
  }

  mod->LogEvent(-1.0f, "imaginal", "START_IMAGINAL_MOD", "", "");
  SetModuleFlag(BUSY);
  ClearModuleFlag(ERROR);
  mod->ScheduleEvent(imaginal_delay.Gen(), ActrEvent::max_pri, this, this, buffer,
                     "MOD_IMAGINAL_CHUNK", "", event.act_arg,
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

bool ActrImaginalModule::SetParam(const String& param_nm, Variant par1, Variant par2) {
  bool got = false;
  if(param_nm == "imaginal_activation" && buffer) {
    buffer->act_total = par1.toFloat();
    got = true;
  }
  else if(param_nm == "imaginal_action_activation" && action_buffer) {
    action_buffer->act_total = par1.toFloat();
    got = true;
  }
  return got;
}
