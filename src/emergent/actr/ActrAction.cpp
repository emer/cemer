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

#include "ActrAction.h"
#include <ActrProceduralModule>
#include <ActrModel>
#include <ActrSlot>

#include <taMisc>

void ActrAction::Initialize() {
  flags = AF_NONE;
  action = UPDATE;
}

void ActrAction::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ActrAction);
  ActrProduction* prod = GET_MY_OWNER(ActrProduction);
  if(prod) {
    prod->UpdateNames();
  }
}

void ActrAction::CutLinks() {
  CutLinks_taAuto(&TA_ActrAction);
  inherited::CutLinks();
}

String& ActrAction::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  switch(action) {
  case UPDATE:
    strm << "=";
  case REQUEST:
    if(action == REQUEST) strm << "+"; // fall thru possible
  case CLEAR:
    if(action == CLEAR) strm << "-"; // fall thru possible
    if(!buffer) {
      strm << "<no buffer!>";
    }
    else {
      strm << buffer->GetName() << " ";
      if(action != CLEAR)
        chunk.Print(strm);
    }
    break;
  case OUTPUT:
    strm << "!output! " << val;
    if(chunk.chunk_type)
      chunk.Print(strm);
    break;
  case STOP:
    strm << "!stop!";
    break;
  case PROG_VAR:
    strm << "var ";
    if(prog_var) {
      strm << prog_var->name << "=" << val;
    }
    else {
      strm << "<no var!>";
    }
    break;
  case PROG_RUN:
    strm << "run ";
    if(program) {
      strm << program->name << "()";
    }
    else {
      strm << "<no program!>";
    }
    break;
  }
  return strm;
}

String ActrAction::GetDisplayName() const {
  String strm;
  switch(action) {
  case UPDATE:
    strm << "=";
  case REQUEST:
    if(action == REQUEST) strm << "+"; // fall thru possible
  case CLEAR:
    if(action == CLEAR) strm << "-"; // fall thru possible
    if(!buffer) {
      strm << "<no buffer!>";
    }
    else {
      strm << buffer->GetName() << " ";
    }
    break;
  case OUTPUT:
    strm << "!output!";
    break;
  case STOP:
    strm << "!stop!";
    break;
  case PROG_VAR:
    strm << "var ";
    if(prog_var) {
      strm << prog_var->name;
    }
    else {
      strm << "<no var!>";
    }
    break;
  case PROG_RUN:
    strm << "run ";
    if(program) {
      strm << program->name << "()";
    }
    else {
      strm << "<no program!>";
    }
    break;
  }
  return strm;
}

String ActrAction::GetDesc() const {
  return PrintStr();
}

void ActrAction::SetVarsChunk(ActrProduction& prod, ActrChunk* ck) {
  for(int i=0; i<chunk.slots.size; i++) {
    ActrSlot* sl = chunk.slots.FastEl(i);
    if(!sl->CondIsVar()) continue;
    ActrSlot* var = prod.vars.FindName(sl->GetVarName());
    ActrSlot* cs = ck->slots.FastEl(i);
    if(var && cs) {
      cs->CopyValFrom(*var); 
    }
  }    
}

void ActrAction::SetVarsString(ActrProduction& prod, String& str) {
  for(int i=0; i<prod.vars.size; i++) {
    ActrSlot* var = prod.vars.FastEl(i);
    str.gsub("=" + var->name, var->val);
  }    
}

bool ActrAction::DoAction(ActrProduction& prod, 
                          ActrProceduralModule* proc_mod, ActrModel* model) {
  if(IsOff()) return false;
  // todo: what priorities on these??
  switch(action) {
  case UPDATE: {
    // todo: add a flag to action if it has variables or not
    ActrChunk* new_chunk = new ActrChunk;
    new_chunk->CopyFrom(&chunk);
    SetVarsChunk(prod, new_chunk);
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod, buffer->module,
                         buffer,
                         "MOD-BUFFER-CHUNK", buffer->name, this, new_chunk);
    break;
  }
  case REQUEST: {
    ActrChunk* new_chunk = new ActrChunk;
    new_chunk->CopyFrom(&chunk);
    SetVarsChunk(prod, new_chunk);
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod, buffer->module,
                         buffer,
                         "MODULE-REQUEST", buffer->name, this, new_chunk);
    break;
  }
  case CLEAR:
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod, buffer->module,
                         buffer,
                         "CLEAR-BUFFER", buffer->name, this);
    break;
  case OUTPUT: {
    String out = val;
    SetVarsString(prod, out);
    if(chunk.chunk_type) {
      ActrChunk* new_chunk = new ActrChunk;
      new_chunk->CopyFrom(&chunk);
      SetVarsChunk(prod, new_chunk);
      new_chunk->Print(out);
      delete new_chunk;
    }
    taMisc::ConsoleOutput(out);
    break;
  }
  case STOP:
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod, NULL, NULL,
                         "!stop!", "", this);
    break;
  case PROG_VAR:
    if(prog_var) {
      String sval = val;
      SetVarsString(prod, sval);
      prog_var->SetVar(sval);
    }
    break;
  case PROG_RUN:
    if(program) {
      program->Call(NULL);
    }
    break;
  }
  return true;                  // todo: need error condition tracking etc
}
