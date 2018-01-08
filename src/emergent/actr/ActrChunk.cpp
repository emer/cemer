// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "ActrChunk.h"
#include <ActrSlotType>
#include <ActrSlot>
#include <ActrCondition>
#include <ActrAction>
#include <ActrModule>
#include <ActrModel>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ActrActTimeVals);

TA_BASEFUNS_CTORS_DEFN(ActrActVals);

TA_BASEFUNS_CTORS_DEFN(ActrChunk);

void ActrActVals::Initialize() {
  act = base = spread = match = noise = 0.0f;
}

void ActrActTimeVals::Initialize() {
  n_act = 0.0f;
  t_new = -1.0f;
  t_act = -1.0f;
}

void ActrChunk::Initialize() {
  flags = CF_NONE;
}

void ActrChunk::Destroy() {
  CutLinks();
}

void ActrChunk::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ActrChunk);
  if(owner && owner->InheritsFrom(&TA_ActrCondition)) {
    SetChunkFlag(COND);
  }
  else if(owner && owner->InheritsFrom(&TA_ActrAction)) {
    SetChunkFlag(ACT);
  }
}

void ActrChunk::CutLinks() {
  CutLinks_taAuto(&TA_ActrChunk);
  inherited::CutLinks();
}

void ActrChunk::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateFromType();
  if(owner && owner->InheritsFrom(&TA_ActrCondition)) {
    SetChunkFlag(COND);
    for(int i=0; i< slots.size; i++) {
      ActrSlot* sl = slots[i];
      sl->SetSlotFlag(ActrSlot::COND); // must be in cond mode!
      sl->val_type = ActrSlot::LITERAL;
    }
  }
  else if(owner && owner->InheritsFrom(&TA_ActrAction)) {
    SetChunkFlag(ACT);
    for(int i=0; i< slots.size; i++) {
      ActrSlot* sl = slots[i];
      sl->SetSlotFlag(ActrSlot::ACT);
      sl->val_type = ActrSlot::LITERAL;
    }
  }
}

void ActrChunk::CopyChunkData(ActrChunk* fm_ck) {
  name = fm_ck->name + "_0";
  chunk_type = fm_ck->chunk_type;
  slots = fm_ck->slots;
  for(int i=0; i< slots.size; i++) {
    ActrSlot* sl = slots[i];
    sl->flags = ActrSlot::SF_NONE; // clear flags
  }
  UpdateFromType();
}

void ActrChunk::ResetChunk() {
  InitChunk(0.0f);
  act.InitAct();
  chunk_type = NULL;
  slots.Reset();  
}

bool ActrChunk::UpdateFromType() {
  if(!chunk_type) return false;

  bool any_changes = false;
  int i;  int ti;
  ActrSlot* sv;
  ActrSlotType* st;
  // delete slots not in type; freshen those that are
  for (i = slots.size - 1; i >= 0; --i) {
    sv = slots.FastEl(i);
    st = chunk_type->slots.FindName(sv->name);
    if(st) {
      any_changes |= sv->UpdateFromType(*st);
    }
    else {
      slots.RemoveIdx(i);
      any_changes = true;
    }
  }
  // add in type not in us, and put in the right order
  for (ti = 0; ti < chunk_type->slots.size; ++ti) {
    st = chunk_type->slots.FastEl(ti);
    i = slots.FindNameIdx(st->name);
    if (i < 0) {
      sv = new ActrSlot();
      sv->UpdateFromType(*st);
      slots.Insert(sv, ti);
      any_changes = true;
    }
    else if (i != ti) {
      slots.MoveIdx(i, ti);
      any_changes = true;
    }
  }
  return any_changes;
}


String& ActrChunk::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << name;
  if(chunk_type) {
    strm << " ISA " << chunk_type->name;
  }
  strm << " (";
  for(int i=0; i<slots.size; i++) {
    ActrSlot* sl = slots.FastEl(i);
    strm << " ";
    sl->Print(strm, 0);
    strm << ";";                // need some kind of sep!!
  }
  strm << " )";
  return strm;
}

String ActrChunk::GetDisplayName() const {
  return inherited::GetDisplayName();
}

String ActrChunk::GetDesc() const {
  return PrintStr();            // calls Print above
}

ActrSlot* ActrChunk::NewSlot() {
  return (ActrSlot*)slots.New(1);
}

void ActrChunk::SetChunkType(ActrChunkType* ck_type) {
  chunk_type = ck_type;
  UpdateAfterEdit();
}

void ActrChunk::SetChunkTypeName(const String& ck_type) {
  ActrModel* mod = GET_MY_OWNER(ActrModel);
  if(mod) {
    ActrChunkType* ct = mod->FindChunkType(ck_type);
    if(ct) {
      SetChunkType(ct);
    }
  }
}

void ActrChunk::SetChunkDefName(int ctr) {
  if(chunk_type) {
    SetName(chunk_type->name + "_" + String(ctr));
  }
  else {
    SetName("nil_type_" + String(ctr));
  }
}

int ActrChunk::GetSpecialState() const {
  if(HasChunkFlag(RETRIEVED)) return 3; // green
  if(HasChunkFlag(ELIGIBLE)) return 4; // red
  if(HasChunkFlag(RECENT)) return 1; // lavendar
  // if(HasChunkFlag(NO_STOP_STEP)) return 2; // pale yellow
  return 0;
}

bool ActrChunk::MatchesProd(ActrProduction& prod, ActrChunk* cmp, bool exact,
                            bool why_not) {
  if(!cmp) {
    if(why_not) {
      taMisc::Info("chunk:", GetDisplayName(), "comparison chunk is null");
    }
    return false;
  }
  if((bool)chunk_type && (bool)cmp->chunk_type) {
    ActrChunkType* compar = chunk_type->CommonChunkType(cmp->chunk_type);
    if(!compar) {
      if(why_not) {
        taMisc::Info("chunk:", GetDisplayName(), "type mismatch");
      }
      return false; // must be same type..
    }
    for(int i=0; i<compar->slots.size; i++) {
      ActrSlot* sl = slots.FastEl(i);
      ActrSlot* os = cmp->slots.SafeEl(i);
      if(!sl->MatchesProd(prod, os, exact, why_not))
        return false;
    }
    return true;                // pass through -- all good!
  }
  // todo: what do we do here??  lookup by names or something?
  if(why_not) {
    if(!chunk_type) {
      taMisc::Info("chunk:", GetDisplayName(), "our chunk type is NULL -- must set chunk_type");
    }
    else {
      taMisc::Info("for chunk:", GetDisplayName(), "comparison chunk type is NULL -- must set chunk_type", cmp->GetDisplayName());
    }
  }
  return false;                 // not yet
}

bool ActrChunk::MatchesMem(ActrChunk* cmp, bool exact, bool why_not) {
  if(!cmp) return false;
  if((bool)chunk_type && (bool)cmp->chunk_type) {
    ActrChunkType* compar = chunk_type->CommonChunkType(cmp->chunk_type);
    if(!compar) {
      if(why_not) {
        taMisc::Info("chunk:", GetDisplayName(), "type mismatch -- no common parent type");
      }
      return false; // must be same type..
    }
    for(int i=0; i<compar->slots.size; i++) {
      ActrSlot* sl = slots.FastEl(i);
      ActrSlot* os = cmp->slots.SafeEl(i);
      if(!sl->MatchesMem(os, exact, why_not))
        return false;
    }
    return true;                // pass through -- all good!
  }
  // todo: what do we do here??  lookup by names or something?
  if(why_not) {
    if(!chunk_type) {
      taMisc::Info("chunk:", GetDisplayName(), "our chunk type is NULL -- must set chunk_type");
    }
    else {
      taMisc::Info("for chunk:", GetDisplayName(), "comparison chunk type is NULL -- must set chunk_type", cmp->GetDisplayName());
    }
  }
  return false;                 // not yet
}


bool ActrChunk::MergeVals(ActrChunk* cmp) {
  if(!cmp) return false;
  if((bool)chunk_type && (bool)cmp->chunk_type) {
    if(TestError(chunk_type != cmp->chunk_type,
                 "MergeVals", "chunk type mismatch")) {
      return false; // must be same type.. (todo: use inheritance here??)
    }
    for(int i=0; i<slots.size; i++) {
      ActrSlot* sl = slots.FastEl(i);
      ActrSlot* os = cmp->slots.SafeEl(i);
      if(!os->IsEmpty()) {
        sl->CopyValFrom(*os);
      }
    }
    return true;
  }
  return false;                 // not yet
}

void ActrChunk::CopyName(ActrChunk* cp) {
  name = cp->name + "_0"; // todo: figure out name business
}

ActrSlot* ActrChunk::FindSlot(const String& slot) {
  ActrSlot* slt = slots.FindName(slot);
  if(TestError(!slt, "SetSlotVal", "slot named:", slot, "not found in chunk of type:", 
               chunk_type ? chunk_type->name : "<no type set>")) {
    return NULL;
  }
  return slt;
}

Variant ActrChunk::GetSlotVal(const String& slot) {
  ActrSlot* sl = FindSlot(slot);
  if(!sl) return _nilVariant;
  if(sl->val_type == ActrSlot::LITERAL) {
    return (Variant)sl->val;
  }
  else {
    return (Variant)sl->val_chunk.ptr();
  }
}

String ActrChunk::GetSlotValLiteral(const String& slot) {
  ActrSlot* sl = FindSlot(slot);
  if(!sl) return _nilString;
  if(TestError(sl->val_type != ActrSlot::LITERAL, "GetSlotValLiteral",
               "slot is not a LITERAL"))
    return _nilString;
  
  return sl->val;
}

ActrChunk* ActrChunk::GetSlotValChunk(const String& slot) {
  ActrSlot* sl = FindSlot(slot);
  if(!sl) return NULL;
  if(TestError(sl->val_type != ActrSlot::CHUNK, "GetSlotValChunk",
               "slot is not a CHUNK pointer"))
    return NULL;
  
  return sl->val_chunk.ptr();
}

bool ActrChunk::SetSlotValLiteral(const String& slot, const String& val) {
  ActrSlot* slt = FindSlot(slot);
  if(!slt) return false;
  if(TestError(slt->val_type != ActrSlot::LITERAL, "SetSlotVal",
               "slot is not a LITERAL"))
    return false;
  slt->val = val;
  return true;
}

bool ActrChunk::SetSlotValChunk(const String& slot, ActrChunk* ck) {
  ActrSlot* slt = FindSlot(slot);
  if(!slt) return false;
  if(TestError(slt->val_type != ActrSlot::CHUNK, "SetSlotValChunk",
               "slot is not a CHUNK pointer"))
    return false;
  slt->val_chunk = ck;
  return true;
}
