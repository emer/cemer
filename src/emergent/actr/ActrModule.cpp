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

#include <taMisc>

void ActrModule::Initialize() {
  own_model = NULL;
  flags = MF_NONE;
}

void ActrModule::Init() {
  flags = MF_NONE;
  InitModule();
  buffer->Init();
}


bool ActrModule::ProcessQuery(ActrBuffer* buf, const String& query, bool why_not) {
  return ProcessQuery_std(buf, query, why_not);
}

bool ActrModule::ProcessQuery_std(ActrBuffer* buf, const String& query, bool why_not) {
  String quer = query;
  bool neg = false;
  if(quer.endsWith('-')) {
    quer = quer.before('-',-1);
    neg = true;
  }
  bool rval = false;
  if(quer == "buffer full" || quer == "full") {
    rval = buf->IsFull();
  }
  else if(quer == "buffer empty" || quer == "empty") {
    rval = buf->IsEmpty();
  }
  else if(quer == "buffer requested" || quer == "requested") {
    rval = buf->IsReq();
  }
  else if(quer == "buffer unrequested" || quer == "unrequested") {
    rval = buf->IsUnReq();
  }
  else if(quer == "state busy" || quer == "busy") {
    rval = IsBusy();
  }
  else if(quer == "state free" || quer == "free") {
    rval = IsFree();
  }
  else if(quer == "state error" || quer == "error") {
    rval = IsError();
  }
  else if(quer == "state busy" || quer == "busy") {
    rval = IsBusy();
  }
  else if(quer == "preparation free") {
    rval = !IsPrep();
  }
  else if(quer == "preparation busy") {
    rval = IsPrep();
  }
  else if(quer == "processor free") {
    rval = !IsProc();
  }
  else if(quer == "processor busy") {
    rval = IsProc();
  }
  else if(quer == "execution free") {
    rval = !IsExec();
  }
  else if(quer == "execution busy") {
    rval = IsExec();
  }
  if(neg) rval = !rval;
  if(!rval && why_not) {
    taMisc::Info("module:",GetDisplayName(), "buffer:", buf->GetDisplayName(),
                 "query:", query, "returned false");
  }
  return rval;
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

