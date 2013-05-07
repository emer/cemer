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

#include "ActrModule.h"
#include <ActrModel>
#include <ActrEvent>

void ActrModule::Initialize() {
  own_model = NULL;
  flags = MF_NONE;
}

void ActrModule::Init() {
  flags = MF_NONE;
  InitModule();
  buffer->Init();
}

bool ActrModule::ProcessEvent_std(ActrEvent& event) {
  bool handled = false;
  if(event.action == "BUFFER-READ-ACTION") {
    if(event.dst_buffer)
      event.dst_buffer->HarvestChunk();
    else
      buffer->HarvestChunk();   // use default
    handled = true;
  }
  else if(event.action == "CLEAR-BUFFER") {
    if(event.dst_buffer)
      event.dst_buffer->ClearChunk();
    else
      buffer->ClearChunk();   // use default
    handled = true;
  }
  else if(event.action == "MOD-BUFFER-CHUNK") {
    if(event.dst_buffer)
      event.dst_buffer->UpdateChunk(event.chunk_arg);
    else
      buffer->UpdateChunk(event.chunk_arg);   // use default
    handled = true;
  }
  return handled;
}

