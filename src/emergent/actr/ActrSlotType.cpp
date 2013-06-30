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

#include "ActrSlotType.h"

#include <ActrChunkType>
#include <ActrModel>

void ActrSlotType::Initialize() {
  val_type = LITERAL;
}

String ActrSlotType::GetDisplayName() const {
  if(val_type == LITERAL) {
    return name + " (literal)";
  }
  if(!val_chunk_type) {
    return name + " (chunk type not set)";
  }
  return name + " (" + val_chunk_type->name + ")";
}

bool ActrSlotType::UpdateFromType(const ActrSlotType& typ) {
  bool any_changes = false;
  if(name != typ.name) {
    name = typ.name;
    any_changes = true;
  }
  if(val_type != typ.val_type) {
    val_type = typ.val_type;
    any_changes = true;
  }
  if(val_type == LITERAL) {
    if((bool)val_chunk_type) {
      val_chunk_type = NULL;         // reset pointer
      any_changes = true;
    }
  }
  else if(val_type == CHUNK) {
    if(val_chunk_type != typ.val_chunk_type) {
      val_chunk_type = typ.val_chunk_type;
      any_changes = true;
    }
  }
  return any_changes;
}

void ActrSlotType::SetChunkType(ActrChunkType* ct) {
  val_chunk_type = ct;
  if(ct != NULL) {
    val_type = CHUNK;
  }
  else {
    val_type = LITERAL;
  }
}

void ActrSlotType::SetChunkTypeName(const String& ct_name) {
  ActrModel* mod = GET_MY_OWNER(ActrModel);
  if(!mod) return;
  ActrChunkType* ct = mod->FindChunkType(ct_name);
  if(ct) {
    SetChunkType(ct);
  }
}
