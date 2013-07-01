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

#include "ActrChunkType.h"
#include <ActrChunk>
#include <ActrSlotType>

void ActrChunkType::Initialize() {
}

void ActrChunkType::Destroy() {
  CutLinks();
}

void ActrChunkType::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateFromParent();
  UpdateAllSubTypes();
}

ActrSlotType* ActrChunkType::NewSlot(const String& slot_nm) {
  ActrSlotType* rval = (ActrSlotType*)slots.New(1);
  if(rval && slot_nm.nonempty()) {
    rval->SetName(slot_nm);
  }
  return rval;
}

void ActrChunkType::MakeSlots(const String& slot_0, const String& slot_1, 
                              const String& slot_2, const String& slot_3, 
                              const String& slot_4, const String& slot_5, 
                              const String& slot_6, const String& slot_7,
                              const String& slot_8, const String& slot_9,
                              const String& slot_a, const String& slot_b) {
  bool made_new;
  if(slot_0.nonempty()) slots.FindMakeNameType(slot_0, NULL, made_new);
  if(slot_1.nonempty()) slots.FindMakeNameType(slot_1, NULL, made_new);
  if(slot_2.nonempty()) slots.FindMakeNameType(slot_2, NULL, made_new);
  if(slot_3.nonempty()) slots.FindMakeNameType(slot_3, NULL, made_new);
  if(slot_4.nonempty()) slots.FindMakeNameType(slot_4, NULL, made_new);
  if(slot_5.nonempty()) slots.FindMakeNameType(slot_5, NULL, made_new);
  if(slot_6.nonempty()) slots.FindMakeNameType(slot_6, NULL, made_new);
  if(slot_7.nonempty()) slots.FindMakeNameType(slot_7, NULL, made_new);
  if(slot_8.nonempty()) slots.FindMakeNameType(slot_8, NULL, made_new);
  if(slot_9.nonempty()) slots.FindMakeNameType(slot_9, NULL, made_new);
  if(slot_a.nonempty()) slots.FindMakeNameType(slot_a, NULL, made_new);
  if(slot_b.nonempty()) slots.FindMakeNameType(slot_b, NULL, made_new);
} 

void ActrChunkType::SetSlotChunkType(const String& slot_nm, const String& chunk_typ) {
  ActrSlotType* st = slots.FindName("screen_pos");
  if(st) {
    st->SetChunkTypeName("visual_location");
  }
}

void ActrChunkType::UpdateAllChunks() {
  TypeDef* td = &TA_ActrChunk;
  for(int i=0; i<td->tokens.size; i++) {
    ActrChunk* ck = (ActrChunk*)td->tokens[i];
    if(!ck) continue;
    if(ck->chunk_type == this) {
      ck->UpdateAfterEdit();
    }
  }
}

bool ActrChunkType::InheritsFromCT(ActrChunkType* par) {
  if(par == this) return true;
  if(!parent) return false;
  return parent->InheritsFromCT(par);
}

bool ActrChunkType::InheritsFromCTName(const String& chunk_type_nm) {
  if(name == chunk_type_nm) return true;
  if(!parent) return false;
  return parent->InheritsFromCTName(chunk_type_nm);
}

ActrChunkType* ActrChunkType::CommonChunkType(ActrChunkType* other) {
  if(this == other) return this;
  if(this->InheritsFromCT(other)) return other;
  if(other->InheritsFromCT(this)) return this;
  ActrChunkType* mypar = parent;
  ActrChunkType* othcmp = other;
  while(mypar != NULL) {
    if(othcmp->InheritsFromCT(mypar)) return mypar;
    if(mypar->InheritsFromCT(othcmp)) return othcmp;
    ActrChunkType* othpar = othcmp->parent;
    while(othpar != NULL) {
      if(othpar->InheritsFromCT(mypar)) return mypar;
      if(mypar->InheritsFromCT(othpar)) return othpar;
      othpar = othpar->parent;
    }
    mypar = mypar->parent;
  }
  return NULL;
}


void ActrChunkType::SetParent(ActrChunkType* par) {
  parent = par;
  if(par) {
    par->sub_types.LinkUnique(this);
    UpdateFromParent();
  }
}

bool ActrChunkType::UpdateFromParent() {
  if(!parent) return false;
  if(TestError(parent->InheritsFromCT(this), "UpdateFromParent",
               "parent chunk type:", parent->GetDisplayName(),
               "inherits from child chunk type -- very bad -- set parent to null")) {
    parent = NULL;
    return false;
  }
  parent->UpdateFromParent();   // make sure parent is all updated as well
  parent->sub_types.LinkUnique(this); // make sure we're in its list
  return UpdateFromParent_impl();
}

bool ActrChunkType::UpdateFromParent_impl() {
  if(!parent) return false;
  bool any_changes = false;
  int i;  int ti;
  ActrSlotType* sv;
  ActrSlotType* st;
  // delete slots not in parent; freshen those that are
  for (i = slots.size - 1; i >= 0; --i) {
    sv = slots.FastEl(i);
    st = parent->slots.FindName(sv->name);
    if(st) {
      any_changes |= sv->UpdateFromType(*st);
    }
    // allow for derived types to go beyond
    // else {
    //   slots.RemoveIdx(i);
    //   any_changes = true;
    // }
  }
  // add in type not in us, and put in the right order
  for (ti = 0; ti < parent->slots.size; ++ti) {
    st = parent->slots.FastEl(ti);
    i = slots.FindNameIdx(st->name);
    if (i < 0) {
      sv = new ActrSlotType();
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

bool ActrChunkType::UpdateAllSubTypes() {
  bool any_chg = false;
  for(int i=0;i<sub_types.size;i++) {
    ActrChunkType* st = sub_types.FastEl(i);
    bool chg = st->UpdateFromParent_impl();
    bool chg2 = st->UpdateAllSubTypes();
    any_chg |= chg;
    any_chg |= chg2;
  }
  return any_chg;
}
