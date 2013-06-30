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
#include <ActrProduction>
#include <ActrModule>

#include <taMisc>

void ActrSlot::Initialize() {
  flags = SF_NONE;
  val_type = LITERAL;
  rel = Relation::EQUAL;
}

void ActrSlot::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ActrSlot);
  if(owner && owner->InheritsFrom(&TA_ActrChunk)) {
    ActrChunk* ck = (ActrChunk*)owner;
    if(ck->HasChunkFlag(ActrChunk::COND))
      SetSlotFlag(COND);
    else if(ck->HasChunkFlag(ActrChunk::ACT))
      SetSlotFlag(ACT);
  }
}

void ActrSlot::CutLinks() {
  CutLinks_taAuto(&TA_ActrSlot);
  inherited::CutLinks();
}

void ActrSlot::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(owner && owner->InheritsFrom(&TA_ActrChunk)) {
    ActrChunk* ck = (ActrChunk*)owner;
    if(ck->HasChunkFlag(ActrChunk::COND))
      SetSlotFlag(COND);
    else if(ck->HasChunkFlag(ActrChunk::ACT))
      SetSlotFlag(ACT);
  }
  if(HasSlotFlag(COND)) {
    val_type = LITERAL;
  }
}

bool ActrSlot::UpdateFromType(const ActrSlotType& typ) {
  bool any_changes = false;
  if(name != typ.name) {
    name = typ.name;
    any_changes = true;
  }
  if(HasSlotFlag(COND)) {
    val_type = LITERAL;
  }
  else {
    if(val_type != (SlotValType)typ.val_type) {
      val_type = (SlotValType)typ.val_type;
      any_changes = true;
    }
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

String& ActrSlot::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << name << " ";
  if(val_type == LITERAL) {
    strm << val;
  }
  else {
    if(!val_chunk) {
      strm << "nil";
    }
    else {
      if(val_chunk->name.nonempty()) {
        strm << "*" << val_chunk->name;
      }
      else {
        val_chunk->Print(strm, 0);
      }
    }
  }
  if(HasSlotFlag(COND)) {
    if(rel != Relation::EQUAL) {
      String relstr = TA_Relation.GetEnumString("Relations", rel);
      strm << " " << relstr;
    }
  }
  return strm;
}

String ActrSlot::GetDisplayName() const {
  return PrintStr();            // calls Print above
}

String ActrSlot::GetVarName() {
  if(!CondIsVar()) return _nilString;
  String var = val.after('=');
  if(CondIsNeg())
    var = var.before('-',-1);
  return var;
}

bool ActrSlot::IsEmpty() {
  if(val_type == LITERAL) {
    return val.empty();
  }
  if(!val_chunk) return true;
  // todo: maybe more here about chunk?
  return false;
}

bool ActrSlot::IsNil() {
  if(val_type == LITERAL) {
    return (val == "nil");
  }
  if(!val_chunk) return true;
  // todo: maybe more here about chunk?
  return false;
}


bool ActrSlot::MatchesProd(ActrProduction& prod, ActrSlot* os, bool exact, bool why_not) {
  if(!os) {
    if(why_not) {
      taMisc::Info("slot:", GetDisplayName(), "other slot is null");
    }
    return false;
  }
  // explicit nil is a strong requirement -- other must be nil or empty to match
  if(IsNil()) {
    bool rval = os->IsNil() || os->IsEmpty();
    if(!rval) {
      if(why_not) {
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "looking for nil, got:", os->val);
      }
    }
    return rval;
  }
  if(os->IsNil()) {
    bool rval = IsNil() || IsEmpty();
    if(!rval) {
      if(why_not) {
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "other slot was explicit nil, our condition is:", val);
      }
    }
    return rval;
  }
  if(IsEmpty()) {
    bool rval = false;
    if(exact) {          // nil is explicit nil match
      rval = os->IsNil() || os->IsEmpty();
    }
    else {
      return true;      //  if we're empty we don't care
    }
    if(!rval) {
      if(why_not) {
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "looking for empty, got:", os->val);
      }
    }
    return rval;
  }
  if(os->IsEmpty()) { 
    bool rval = false;
    if(exact) {          // nil is explicit nil match
      rval = IsNil() || IsEmpty();
    }
    else {
      return true;      //  if other guy is empty we don't care regardless??
    }
    if(!rval) {
      if(why_not) {
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "looking for empty, got:", os->val);
      }
    }
    return rval;
  }
  if(!exact && CondIsVar()) {
    if(!CondIsNeg()) {
      ActrSlot* var = prod.vars.FindName(GetVarName());
      if(var) {
        var->CopyValFrom(*os);  // we are source for this right now
      }
    }
    return true;  // a variable always matches at this stage
  }
  if(os->val_type == LITERAL) {
    bool rval = false;
    if(rel == Relation::EQUAL) {
      rval = (val == os->val);
    }
    else if(rel == Relation::NOTEQUAL) {
      rval = (val != os->val);
    }
    else if(rel >= Relation::CONTAINS) {
      rval = os->val.contains(val);
      if(rel == Relation::NOT_CONTAINS)
        rval = !rval;
    }
    else {
      Relation rl;
      rl.rel = rel;
      rl.val = (double)val;
      rval = rl.Evaluate((double)os->val);
    }
    if(!rval) {
      if(why_not) {
        String relstr = TA_Relation.GetEnumString("Relations", rel);
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "looking for:", val, relstr, os->val);
      }
    }
    return rval;
  }
  if(val == os->val_chunk->name)
    return true;
  // todo: not sure how matching works on chunk types -- name??
  if(why_not) {
    taMisc::Info("slot:", GetDisplayName(), "CHUNK type fall thru mismatch");
  }
  return false;
}

bool ActrSlot::MatchesMem(ActrSlot* os, bool exact, bool why_not) {
  if(!os) {
    if(why_not) {
      taMisc::Info("slot:", GetDisplayName(), "other slot is null");
    }
    return false;
  }
  // explicit nil is a strong requirement -- other must be nil or empty to match
  if(IsNil()) {
    bool rval = os->IsNil() || os->IsEmpty();
    if(!rval) {
      if(why_not) {
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "looking for nil, got:", os->val);
      }
    }
    return rval;
  }
  if(os->IsNil()) {
    bool rval = IsNil() || IsEmpty();
    if(!rval) {
      if(why_not) {
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "other slot was explicit nil, our condition is:", val);
      }
    }
    return rval;
  }
  if(IsEmpty()) {
    bool rval = false;
    if(exact) {          // nil is explicit nil match
      rval = os->IsNil() || os->IsEmpty();
    }
    else {
      return true;      //  if we're empty we don't care
    }
    if(!rval) {
      if(why_not) {
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "looking for empty, got:", os->val);
      }
    }
    return rval;
  }
  if(os->IsEmpty()) { 
    bool rval = false;
    if(exact) {          // nil is explicit nil match
      rval = IsNil() || IsEmpty();
    }
    else {
      return true;      //  if other guy is empty we don't care regardless??
    }
    if(!rval) {
      if(why_not) {
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "looking for empty, got:", os->val);
      }
    }
    return rval;
  }
  if(os->val_type == LITERAL) {
    bool rval = (val == os->val);
    if(!rval) {
      if(why_not) {
        taMisc::Info("slot:", GetDisplayName(), "value mismatch",
                     "looking for:", val, "got:", os->val);
      }
    }
    return rval;
  }
  // note: not nil b/c that was dealt with above
  if(val_chunk == os->val_chunk)
    return true;
  if(val_chunk->name == os->val_chunk->name)
    return true;
  if(val_chunk->MatchesMem(os->val_chunk, exact, why_not)) // full content match
    return true;
  if(why_not) {
    taMisc::Info("slot:", GetDisplayName(), "CHUNK type fall thru mismatch");
  }
  return false;
}


void ActrSlot::CopyValFrom(const ActrSlot& cp) {
  val_type = cp.val_type;
  if(val_type == LITERAL)
    val = cp.val;
  else
    val_chunk = cp.val_chunk;
}

bool ActrSlot::SetVal(const String& str, Relation::Relations rl) {
  if(TestError(val.nonempty(), "SetVal",
               "chunk value had already been set -- request multiple values of the same slot, please duplicate the action and divide across them")) {
    val += " ";
  }
  val += str;
  rel = rl;
  return true;
}
