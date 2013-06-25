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

#include "ActrVisionModule.h"
#include <ActrModel>

void ActrVisionModule::Initialize() {
}

void ActrVisionModule::InitModule() {
  if((bool)buffer) return;
  if(!Model()) return;
  ActrModel* mod = Model();
  bool made_new;
  buffer = mod->buffers.FindMakeNameType("visual", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  buffer->ClearBufferFlag(ActrBuffer::STRICT_HARVEST); // vision doesn't do this!

  buffer = mod->buffers.FindMakeNameType("visual-location", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  buffer->ClearBufferFlag(ActrBuffer::STRICT_HARVEST); // vision doesn't do this!
}

void ActrVisionModule::Init() {
  inherited::Init();
  buffer->UpdateState();
}

void ActrVisionModule::ProcessEvent(ActrEvent& event) {
  // if(event.action == "MODULE-REQUEST") {
  //   VisionRequest(event);
  // }
  // else if(event.action == "MOD-BUFFER-CHUNK") {
  //   VisionBufferMod(event);
  // }
  // else if(event.action == "CREATE-NEW-BUFFER-CHUNK") {
  //   CreateNewChunk(event);
  // }
  // else if(event.action == "SET-BUFFER-CHUNK") {
  //   SetBufferChunk(event);
  // }
  // else if(event.action == "MOD-VISION-CHUNK") {
  //   ModVisionChunk(event);
  // }
  // else {
    ProcessEvent_std(event);   // respond to regular requests
  // }
}


