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
  action = UPDATE;
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

void ActrAction::SetVariables(ActrProduction& prod) {
  for(int i=0; i<chunk.slot_vals.size; i++) {
    ActrSlot* sl = chunk.slot_vals.FastEl(i);
    if(!sl->CondIsVar()) continue;
    ActrSlot* var = prod.vars.FindName(sl->GetVarName());
    if(var) {
      sl->CopyValFrom(*var); 
    }
  }    
}

bool ActrAction::DoAction(ActrProduction& prod, 
                          ActrProceduralModule* proc_mod, ActrModel* model) {
  // todo: what priorities on these??
  switch(action) {
  case UPDATE:
    SetVariables(prod);
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod, buffer->module,
                         buffer,
                         "MOD-BUFFER-CHUNK", buffer->name, this, &this->chunk);
    break;
  case REQUEST:
    SetVariables(prod);
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod, buffer->module,
                         buffer,
                         "MODULE-REQUEST", buffer->name, this, &this->chunk);
    break;
  case CLEAR:
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod, buffer->module,
                         buffer,
                         "CLEAR-BUFFER", buffer->name, this);
    break;
  case OUTPUT: {
    String out = val;
    if(chunk.chunk_type) {
      SetVariables(prod);
      chunk.Print(out);
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
      prog_var->SetVar(val);
    }
    break;
  case PROG_RUN:
    if(program) {
      program->Run();
    }
    break;
  }
  return true;                  // todo: need error condition tracking etc
}
