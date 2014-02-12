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

TA_BASEFUNS_CTORS_DEFN(ActrGoalModule);


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
  if(made_new) {
    buffer->act_total = 1.0f;   // default ga
  }
}

void ActrGoalModule::ProcessEvent(ActrEvent& event) {
  ProcessEvent_std(event);      // just follow standard events..
}

void ActrGoalModule::Init() {
  inherited::Init();
  buffer->active.CopyFrom(&init_chunk);
  if(init_chunk.size == 1) {
    ActrModel* mod = Model();
    mod->LogEvent(0.0f, "goal", "SET_BUFFER_CHUNK", "", init_chunk.FastEl(0)->name);
  }
  buffer->UpdateState();
}

bool ActrGoalModule::SetParam(const String& param_nm, Variant par1, Variant par2) {
  bool got = false;
  if(param_nm == "ga" && buffer) {
    buffer->act_total = par1.toFloat();
    got = true;
  }
  return got;
}
