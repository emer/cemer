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

#include "ActrSlot.h"
#include <ActrSlotType>
#include <ActrChunk>

void ActrSlot::Initialize() {
  val_type = LITERAL;
}

bool ActrSlot::UpdateFromType(const ActrSlotType& typ) {
  bool any_changes = false;
  if(name != typ.name) {
    name = typ.name;
    any_changes = true;
  }
  if(val_type != (SlotValType)typ.val_type) {
    val_type = (SlotValType)typ.val_type;
    any_changes = true;
  }
  if(val_type == LITERAL) {
    if((bool)val_chunk) {
      val_chunk = NULL;         // reset pointer
      any_changes = true;
    }
  }
  else if(val_type == CHUNK) {
    if((bool)val_chunk) {
      if(TestWarning(val_chunk->chunk_type != typ.val_chunk_type,
                     "InitFromSlotType",
                     "chunk value type",val_chunk->chunk_type->name,
                     "does not match val_chunk_type from slot type:",
                     typ.val_chunk_type->name)) {
        // don't do anything?
      }
    }
  }
  return any_changes;
}

bool ActrSlot::IsNil() {
  if(val_type == LITERAL) {
    if(val.empty() || val == "nil") return true;
    return false;
  }
  if(!val_chunk) return true;
  // todo: maybe more here about chunk?
  return false;
}


bool ActrSlot::Matches(ActrSlot* os) {
  if(IsNil()) return true;      //  if we're empty we don't care
  if(val_type == LITERAL && val.startsWith('=')) return true; // a variable always matches
  if(!os) return false;         // we're not empty and other guy is..
  if(val_type == LITERAL && os->val_type == LITERAL) {
    return val == os->val;
  }
  // todo: not sure how matching works on chunk types -- must be recursive??
  return false;
}

String ActrSlot::WhyNot(ActrSlot* os) {
  return "not impl\n";
}
