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
#include <String_Array>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ActrModule);

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
  if(event.action == "BUFFER_READ_ACTION") {
    // todo: it seems that this should generate a CLEAR_BUFFER event .05 latency later, when
    // production actually fires.. -- this is what is recorded in the stack
    // this may make some kind of difference at some point..
    if(event.dst_buffer)
      event.dst_buffer->HarvestChunk();
    else
      buffer->HarvestChunk();   // use default
    handled = true;
  }
  else if(event.action == "CLEAR_BUFFER") {
    if(event.dst_buffer)
      event.dst_buffer->ClearChunk();
    else
      buffer->ClearChunk();   // use default
    handled = true;
  }
  else if(event.action == "SET_BUFFER_CHUNK") {
    if(event.dst_buffer)
      event.dst_buffer->SetChunk(event.chunk_arg);
    else
      buffer->SetChunk(event.chunk_arg);   // use default
    handled = true;
  }
  else if(event.action == "MOD_BUFFER_CHUNK") {
    if(event.dst_buffer)
      event.dst_buffer->UpdateChunk(event.chunk_arg);
    else
      buffer->UpdateChunk(event.chunk_arg);   // use default
    handled = true;
  }
  else if(event.action == "CLEAR_STATE") {
    String_Array pary;
    pary.FmDelimString(event.params, " ");
    if(TestError(pary.size % 2 != 0, "ProcessEvent_std",
                 "CLEAR_STATE must have even number of parameters: state, value pairs")) {
      return true;
    }
    for(int i=0; i<pary.size; i += 2) {
      String st = pary[i];
      String vl = pary[i+1];
      if(st == "LAST") {
        if(vl == "NONE")
          last_cmd = "";
        else
          last_cmd = vl;
      }
      else if(st == "PREP") {
        if(vl == "FREE")
          ClearModuleFlag(PREP);
        else
          SetModuleFlag(PREP);
      }
      else if(st == "PROC") {
        if(vl == "FREE")
          ClearModuleFlag(PROC);
        else
          SetModuleFlag(PROC);
      }
      else if(st == "EXEC") {
        if(vl == "FREE")
          ClearModuleFlag(EXEC);
        else
          SetModuleFlag(EXEC);
      }
      else if(st == "BUSY") {
        if(vl == "FREE")
          ClearModuleFlag(BUSY);
        else
          SetModuleFlag(BUSY);
      }
      else if(st == "ERROR") {
        if(vl == "CLEAR")
          ClearModuleFlag(ERROR);
        else
          SetModuleFlag(ERROR);
      }
    }
    handled = true;
  }
  return handled;
}

bool ActrModule::RequestBufferClear(ActrBuffer* buf) {
  if(!buf) return false;
  ActrModel* mod = Model();
  buf->ClearChunk();         // always clear before find
  mod->LogEvent(-1.0f, "procedural", "CLEAR_BUFFER", buf->name, "");
  return true;
}
