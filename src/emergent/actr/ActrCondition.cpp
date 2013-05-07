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
#include <ActrSlot>

#include <ProgVar>
#include <Layer>

#include <taMisc>

void ActrCondition::Initialize() {
  flags = CF_NONE;
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
  for(int i=0; i< cmp_chunk.slots.size; i++) {
    ActrSlot* sl = cmp_chunk.slots[i];
    sl->SetSlotFlag(ActrSlot::COND); // must be in cond mode!
    sl->val_type = ActrSlot::LITERAL;
  }
}

String& ActrCondition::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  switch(cond_src) {
  case BUFFER_EQ: {
    if(!src) {
      strm << "=<no buffer!>";
    }
    else {
      strm << "=" << src->GetName() << " ";
      cmp_chunk.Print(strm);
    }
    break;
  }
  case BUFFER_QUERY: {
    if(!src) {
      strm << "?<no buffer!>";
    }
    else {
      strm << "?" << src->GetName() << " ";
    }
    break;
  }
  }
  return strm;
}

String ActrCondition::GetDisplayName() const {
  String strm;
  switch(cond_src) {
  case BUFFER_EQ: {
    if(!src) {
      strm << "=<no buffer!>";
    }
    else {
      strm << "=" << src->GetName();
    }
    break;
  }
  case BUFFER_QUERY: {
    if(!src) {
      strm << "?<no buffer!>";
    }
    else {
      strm << "?" << src->GetName();
    }
    break;
  }
  }
  return strm;
}

String ActrCondition::GetDesc() const {
  return PrintStr();
}

void ActrCondition::UpdateVars(ActrProduction& prod) {
  if(cond_src != BUFFER_EQ) return;
  for(int i=0; i< cmp_chunk.slots.size; i++) {
    ActrSlot* sl = cmp_chunk.slots[i];
    sl->SetSlotFlag(ActrSlot::COND); // must be in cond mode!
    if(!sl->CondIsVar()) continue;
    String var = sl->GetVarName();
    bool made_new = false;
    prod.vars.FindMakeNameType(var, NULL, made_new); // make sure one exists
  }
}

bool ActrCondition::Matches(ActrProduction& prod, bool why_not) {
  switch(cond_src) {
  case BUFFER_EQ: {
    // todo: should be dealt with in check config
    if(TestError(!src, "Matches",
                 "no buffer specified as the source to match against!"))
      return false;
    ActrBuffer* buf = (ActrBuffer*)src.ptr();
    if(!buf->IsFull()) {
      if(why_not) {
        taMisc::Info("condition:", GetDisplayName(), "buffer:", buf->name, "is empty");
      }
      return false;
    }
    ActrChunk* bc =buf->CurChunk();
    bool match = cmp_chunk.MatchesProd(prod, bc, false, why_not); // false = not exact
    return match;
    break;
  }
  case BUFFER_QUERY: {
    if(TestError(!src, "Matches",
                 "no buffer specified as the source to match against!"))
      return false;
    ActrBuffer* buf = (ActrBuffer*)src.ptr();
    bool match = buf->QueryMatches(cmp_val, why_not);
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

bool ActrCondition::MatchVars(ActrProduction& prod, bool why_not) {
  if(cond_src != BUFFER_EQ) return true;
  ActrBuffer* buf = (ActrBuffer*)src.ptr();
  if(!buf->IsFull()) return true; // should not fail
  if(why_not) {
    taMisc::Info("condition:", GetDisplayName(),
                 "starting variable-based matching, vars:", prod.PrintVars());
  }
  ActrChunk* bc =buf->CurChunk();
  for(int i=0; i< cmp_chunk.slots.size; i++) {
    ActrSlot* sl = cmp_chunk.slots[i];
    if(!sl->CondIsVar()) continue;
    ActrSlot* var = prod.vars.FindName(sl->GetVarName());
    if(var) {
      ActrSlot* os = bc->slots.SafeEl(i);
      if(os) {
        if(sl->CondIsNeg()) {
          // true = use exact match
          if(var->MatchesProd(prod, os, true, false)) { // this report will be meaningless
            if(why_not) {
              taMisc::Info("condition:", GetDisplayName(),
                           "negative variable test actually matches!",
                           sl->GetDisplayName());
            }
            return false; // only nonmatch!
          }
        }
        else {
          if(!var->MatchesProd(prod, os, true, why_not)) { // true = exact match
            return false; // only nonmatch!
          }
        }
      }
    }
  }
  return true;
}

void ActrCondition::SendBufferReads(ActrProceduralModule* proc_mod, ActrModel* model) {
  if(cond_src != BUFFER_EQ) return;
  if(HasCondFlag(BUF_UPDT_ACT)) return;
  if(TestError(!src, "Matches",
               "no buffer specified as the source to match against!"))
    return;
  ActrBuffer* buf = (ActrBuffer*)src.ptr();
  // todo: not sure about pri here
  model->ScheduleEvent(0.0f, ActrEvent::max_pri, proc_mod, buf->module, buf,
                       "BUFFER-READ-ACTION", buf->name);
}
