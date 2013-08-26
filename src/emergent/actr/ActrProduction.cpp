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
#include <ActrModel>

#include <taMisc>

void ActrProdUtilVals::Initialize() {
  init = 0.0f;
  cur = 0.0f;
  choice = 0.0f;
  rew = 0.0f;
}

void ActrProdTimeVals::Initialize() {
  act = 0.0f;
  last_fire = -1.0f;
}

void ActrProduction::Initialize() {
  flags = PF_NONE;
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
    cnd->cmp_chunk.SigEmitUpdated();
  }
  for(int i=0; i<acts.size; i++) {
    ActrAction* act = acts.FastEl(i);
    act->chunk.name = name + "_a" + String(i);
    act->chunk.SigEmitUpdated();
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
        if(act->dest.ptr() == cnd->src.ptr()) {
          cnd->SetCondFlag(ActrCondition::BUF_UPDT_ACT); // we are going to update
          break;
        }
      }
    }
  }
  // extra check on all actions for buffer names -- those need to be marked as UPDT as well apparently
  ActrModel* mod = GET_MY_OWNER(ActrModel);
  if(mod) {
    for(int j=0; j<acts.size; j++) {
      ActrAction* act = acts.FastEl(j);
      for(int ci=0; ci<act->chunk.slots.size; ci++) {
        ActrSlot* as = act->chunk.slots.FastEl(ci);
        if(!as->CondIsVar()) continue;
        String vnm = as->GetVarName();
        ActrBuffer* buf = mod->buffers.FindName(vnm);
        if(buf) {
          for(int i=0; i<conds.size; i++) {
            ActrCondition* cnd = conds.FastEl(i);
            if(cnd->src.ptr() == buf) {
              cnd->SetCondFlag(ActrCondition::BUF_UPDT_ACT); // we are going to update
            }
          }
        }
      }
    }
  }
}

void ActrProduction::InitActionProgs() {
  for(int i=0; i<acts.size; i++) {
    ActrAction* act = acts.FastEl(i);
    act->InitProg();
  }
}

void ActrProduction::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
}

void ActrProduction::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  conds.CheckConfig(quiet, rval);
  acts.CheckConfig(quiet, rval);
}

int ActrProduction::GetSpecialState() const {
  if(HasProdFlag(FIRED)) return 3; // green
  if(HasProdFlag(ELIGIBLE)) return 4; // red
  // if(HasProgFlag(TRACE)) return 1; // lavendar
  // if(HasProgFlag(NO_STOP_STEP)) return 2; // pale yellow
  return 0;
}

void ActrProduction::Init() {
  ClearProdFlag(FIRED);
  ClearProdFlag(ELIGIBLE);
  time.last_fire = -1.0f;
  UpdateNames();
  UpdateVars();
  InitActionProgs();
  SigEmitUpdated();
}

ActrSlot* ActrProduction::FindVar(const String& nm, ActrBuffer*& buf, bool err_msg) {
  buf = NULL;
  ActrSlot* rval = vars.FindName(nm);
  if(rval) return rval;
  ActrModel* mod = GET_MY_OWNER(ActrModel);
  if(mod) {
    buf = mod->buffers.FindName(nm);
    if(buf) return NULL;
  }
  TestError(err_msg, "FindVar", "variable or buffer named:", nm, "not found");
  return NULL;
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
  if(IsOff()) {
    if(why_not) {
      taMisc::Info("production is flagged as OFF!");
    }
    return false;
  }
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
  taMisc::Info("Why production:",name,"did not match (see last line for actual match state):");
  bool rval = Matches(true);
  taMisc::Info("Production:",name,"returned from match with:", (String)rval);
  return rval;
}

void ActrProduction::SendCondActions(ActrProceduralModule* proc_mod, ActrModel* model) {
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    cnd->SendCondActions(proc_mod, model);
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

ActrCondition* ActrProduction::FindCondOnBuffer(ActrBuffer* buf) {
  for(int i=0; i<conds.size; i++) {
    ActrCondition* cnd = conds.FastEl(i);
    if(cnd->src.ptr() == buf)
      return cnd;
  }
  return NULL;
}

bool ActrProduction::SetParam(const String& par_nm, float val) {
  if(par_nm == "u") {
    util.init = val;
  }
  else if(par_nm == "reward") {
    util.rew = val;
  }
  else if(par_nm == "at") {
    time.act = val;
  }
  else {
    TestError(true, "SetParam", "parameter named:", par_nm, "not found");
    return false;
  }
  return true;
}
