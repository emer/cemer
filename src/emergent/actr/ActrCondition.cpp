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

#include "ActrCondition.h"
#include <ActrBuffer>
#include <ActrModel>
#include <ActrProceduralModule>

#include <ProgVar>
#include <Layer>

void ActrCondition::Initialize() {
  cond_src = BUFFER_EQ;
  src_type = &TA_ActrBuffer;
  rel = EQUAL;
}

void ActrCondition::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  switch(cond_src) {
  case BUFFER_EQ:
  case BUFFER_QUERY:
    src_type = &TA_ActrBuffer;
    if(TestWarning(rel != EQUAL && rel != NOTEQUAL,
                   "UAE", "only equal or not-equal relationship allowed")) {
      rel = EQUAL;
    }
    break;
  case PROG_VAR:
    src_type = &TA_ProgVar;
    break;
  case NET_UNIT:
  case NET_LAYER:
    src_type = &TA_Layer;
    break;
  case OBJ_MEMBER:
    break;
  }
}

bool ActrCondition::Matches() {
  switch(cond_src) {
  case BUFFER_EQ: {
    if(TestError(!src, "Matches",
                 "no buffer specified as the source to match against!"))
      return false;
    ActrBuffer* buf = (ActrBuffer*)src.ptr();
    if(!buf->IsFull()) return false;
    bool match = cmp_chunk.Matches(buf->CurChunk());
    if(rel == NOTEQUAL) match = !match;
    return match;
    break;
  }
  case BUFFER_QUERY: {
    if(TestError(!src, "Matches",
                 "no buffer specified as the source to match against!"))
      return false;
    ActrBuffer* buf = (ActrBuffer*)src.ptr();
    bool match = buf->Matches(cmp_val);
    if(rel == NOTEQUAL) match = !match;
    return match;
    break;
  }
  case PROG_VAR:
    return false;               // not yet
    break;
  case NET_UNIT:
    return false;               // not yet
    break;
  case NET_LAYER:
    return false;               // not yet
    break;
  case OBJ_MEMBER:
    return false;               // not yet
    break;
  }
  return false;
}

String ActrCondition::WhyNot() {
  // todo write
  return "not impl yet\n";
}

void ActrCondition::SendBufferReads(ActrProceduralModule* proc_mod, ActrModel* model) {
  if(cond_src != BUFFER_EQ) return;
  if(TestError(!src, "Matches",
               "no buffer specified as the source to match against!"))
    return;
  ActrBuffer* buf = (ActrBuffer*)src.ptr();
  // todo: not sure about pri here
  model->ScheduleEvent(0.0f, ActrEvent::max_pri, proc_mod, buf->module, buf,
                       "BUFFER-READ-ACTION", buf->name);
}
