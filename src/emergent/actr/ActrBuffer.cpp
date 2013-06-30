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

#include "ActrBuffer.h"
#include <ActrDeclarativeModule>

#include <String_Array>

#include <taMisc>

void ActrBuffer::Initialize() {
  act_total = 0.0f;
  flags = STD_FLAGS;
}

bool ActrBuffer::QueryMatches(const String& query, bool why_not) {
  if(query.contains(';')) {
    // multiple queries
    String_Array ary;
    ary.FmDelimString(query, ';');
    for(int i=0; i<ary.size; i++) {
      String qu = ary[i];
      bool rv = module->ProcessQuery(this, qu, why_not); // delegate to module
      if(!rv) return false;
    }
    return true;
  }
  else {
    return module->ProcessQuery(this, query, why_not); // delegate to module
  }
}

void ActrBuffer::Init() {
  active.Reset();
  ClearBufferFlag(FULL);
  ClearBufferFlag(REQ);
}

void ActrBuffer::UpdateState() {
  SetBufferFlagState(FULL, active.size == 1);
}

bool ActrBuffer::HarvestChunk() {
  if(!HasBufferFlag(STRICT_HARVEST)) return false;
  return ClearChunk();
}

bool ActrBuffer::ClearChunk() {
  return ClearChunk_impl(HasBufferFlag(DM_MERGE));
}

bool ActrBuffer::ClearChunk_impl(bool dm_merge) {
  ActrChunk* ck = CurChunk();
  if(!ck) return false;
  if(dm_merge) {
    ActrDeclarativeModule* dm = DeclarativeModule();
    if(TestError(!dm, "ClearChunk", "no declarative module!")) { // shouldn't happen
      return false;
    }
    dm->AddChunk(ck, true);     // true = merge
  }
  active.Reset();               // clear
  ClearBufferFlag(FULL);
  return true;
}

ActrChunk* ActrBuffer::UpdateChunk(ActrChunk* chunk) {
  if(TestError(!chunk, "SetChunk", "called with null chunk!")) { // shouldn't happen
    return NULL;
  }
  if(IsFull()) {
    ActrChunk* ck = CurChunk();
    if(TestWarning(ck->chunk_type != chunk->chunk_type, "UpdateChunk",
                   "new chunk type is not same as current, replacing old with new")) {
      ClearChunk();
      return UpdateChunk(chunk); // redo
    }
    ck->MergeVals(chunk);       // merge with new
    SetBufferFlag(FULL);
    return ck;
  }
  else {
    ActrChunk* nw_ck = (ActrChunk*)active.New(1);
    nw_ck->CopyFrom(chunk);
    nw_ck->CopyName(chunk);
    SetBufferFlag(FULL);
    return nw_ck;
  }
}
