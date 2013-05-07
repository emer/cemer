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

#include "ActrEvent.h"
#include <ActrModule>

#include <DataTable>

#include <taMisc>

int ActrEvent::max_pri = 100;
int ActrEvent::min_pri = 0;

void ActrEvent::Initialize() {
  time = 0.0f;
  priority = min_pri;
}

ActrEvent* ActrEvent::NewEvent(float tm, int pri, ActrModule* src_mod,
                               ActrModule* dst_mod, ActrBuffer* dst_buf,
                               const String& act, const String& pars,
                               ActrAction* actn, ActrChunk* chnk,
                               TypeDef* event_type) {
  ActrEvent* rv = NULL;
  if(event_type) {
    rv = (ActrEvent*)taBase::MakeToken(event_type);
  }
  else {
    rv = new ActrEvent;
  }
  rv->time = tm;
  rv->priority = pri;
  rv->src_module = src_mod;
  rv->dst_module = dst_mod;
  rv->dst_buffer = dst_buf;
  rv->action = act;
  rv->params = pars;
  rv->act_arg = actn;
  rv->chunk_arg = chnk;
  return rv;
}

void ActrEvent::LogEvent(DataTable& dt) {
  dt.AddBlankRow();
  dt.SetVal(time, "time", -1);
  if(src_module) {
    dt.SetVal(src_module->name, "module", -1);
  }
  dt.SetVal(action, "action", -1);
  if(dst_buffer) {
    dt.SetVal(dst_buffer->name, "target", -1);
  }
  else if(dst_module) {
    dt.SetVal(dst_module->name, "target", -1);
    dt.SetVal(dst_module->name, "dst_module", -1);
  }
  dt.SetVal(params, "params", -1);
  if(dst_module) {
    dt.SetVal(dst_module->name, "dst_module", -1);
  }
  dt.SetVal(priority, "priority", -1);
  if(act_arg) {
    dt.SetVal(act_arg->PrintStr(), "prod_action", -1);
  }
  if(chunk_arg) {
    dt.SetVal(chunk_arg->PrintStr(), "chunk", -1);
  }
  dt.WriteClose();
}


String& ActrEvent::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << "t: " << time << " p: " << priority;
  if(src_module) {
    strm << " " << src_module->name << " ";
  }
  strm << action;
  if(dst_buffer) {
    strm << " " << dst_buffer->name;
  }
  else if(dst_module) {
    strm << " " << dst_module->name;
  }
  if(params.nonempty()) {
    strm << " " << params;
  }
  return strm;
}

String ActrEvent::GetDisplayName() const {
  return PrintStr();            // returns Print above
}

String ActrEvent::GetDesc() const {
  String rval;
  if(act_arg) {
    rval << " action: " << act_arg->GetDisplayName();
  }
  if(chunk_arg) {
    rval << " chunk: " << chunk_arg->GetDisplayName();
  }
  return rval;
}
