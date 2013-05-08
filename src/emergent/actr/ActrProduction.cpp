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

#include "ActrProduction.h"
#include <ActrModule>
#include <ActrSlot>

#include <taMisc>

void ActrProduction::Initialize() {
  off = false;
  util = 0.0f;
  rew = 0.0f;
}

void ActrProduction::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateNames();
  UpdateVars();
}

void ActrProduction::UpdateNames() {
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    cnd->cmp_chunk.name = name + "_c" + String(i);
    cnd->SigEmitUpdated();
  }
  for(int i=0; i<acts.size; i++) {
    ActrAction* act = acts.FastEl(i);
    act->chunk.name = name + "_a" + String(i);
    act->SigEmitUpdated();
  }
}

void ActrProduction::UpdateVars() {
  vars.Reset();
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    cnd->UpdateVars(*this);
    
    // set buf update act flag 
    cnd->ClearCondFlag(ActrCondition::BUF_UPDT_ACT);
    if(cnd->cond_src == ActrCondition::BUFFER_EQ) {
      for(int j=0; j<acts.size; j++) {
        ActrAction* act = acts.FastEl(j);
        if(act->action != ActrAction::UPDATE) continue;
        if(act->buffer.ptr() == cnd->src.ptr()) {
          cnd->SetCondFlag(ActrCondition::BUF_UPDT_ACT); // we are going to update
          break;
        }
      }
    }
  }
}

void ActrProduction::Init() {
  UpdateNames();
  UpdateVars();
}

String ActrProduction::PrintVars() const {
  String strm;
  strm << name << " vars (";
  for(int i=0; i<vars.size; i++) {
    ActrSlot* sl = vars.FastEl(i);
    strm << " ";
    sl->Print(strm, 0);
    strm << ";";                // need some kind of sep!!
  }
  strm << " )";
  return strm;
}

bool ActrProduction::Matches(bool why_not) {
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    if(!cnd->Matches(*this, why_not)) {
      return false; // only takes one
    }
  }
  // ok, we make first pass -- now second pass to enforce consistency of vars
  // this is definitely too powerful!
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    if(!cnd->MatchVars(*this, why_not)) {
      return false; // only takes one
    }
  }
  return true;
}

bool ActrProduction::WhyNot() {
  taMisc::Info("Why production:",name,"did not match:");
  bool rval = Matches(true);
  taMisc::Info("Production:",name,"returned from match with:", (String)rval);
  return rval;
}

void ActrProduction::SendBufferReads(ActrProceduralModule* proc_mod, ActrModel* model) {
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    cnd->SendBufferReads(proc_mod, model);
  }
}

bool ActrProduction::DoActions(ActrProceduralModule* proc_mod, ActrModel* model) {
  bool good = true;
  for(int i=0; i<acts.size; i++) {
    ActrAction* act = acts.FastEl(i);
    bool rv = act->DoAction(*this, proc_mod, model);
    good = good && rv;
  }
  return good;
}
