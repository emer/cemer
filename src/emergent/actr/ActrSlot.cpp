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
}

void ActrSlot::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
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

bool ActrSlot::IsNil() {
  if(val_type == LITERAL) {
    if(val.empty() || val == "nil") return true;
    return false;
  }
  if(!val_chunk) return true;
  // todo: maybe more here about chunk?
  return false;
}


bool ActrSlot::Matches(ActrProduction& prod, ActrSlot* os, bool why_not) {
  if(IsNil()) return true;      //  if we're empty we don't care
  if(CondIsVar()) {
    if(!CondIsNeg()) {
      ActrSlot* var = prod.vars.FindName(GetVarName());
      if(var) {
        var->CopyValFrom(*os);  // we are source for this right now
      }
    }
    return true;  // a variable always matches at this stage
  }
  if(!os) {
    if(why_not) {
      taMisc::Info("slot:", GetDisplayName(), "other slot is null");
    }
    return false;         // we're not empty and other guy is..
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
  if(val == os->val_chunk->name)
    return true;
  // todo: not sure how matching works on chunk types -- name??
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

