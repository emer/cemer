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

#include "ActrGoalModule.h"
#include <ActrModel>


void ActrGoalModule::Initialize() {
}

void ActrGoalModule::InitModule() {
  if((bool)buffer) return;
  if(!Model()) return;
  ActrModel* mod = Model();
  bool made_new;
  buffer = mod->buffers.FindMakeNameType("goal", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  buffer->ClearBufferFlag(ActrBuffer::STRICT_HARVEST); // goal doesn't do this!
}

void ActrGoalModule::ProcessEvent(ActrEvent& event) {
  ProcessEvent_std(event);      // just follow standard events..
}

void ActrGoalModule::Init() {
  inherited::Init();
  buffer->active.CopyFrom(&init_chunk);
  buffer->UpdateState();
}

