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

#include "ActrDeclarativeModule.h"

#include <ActrModel>
#include <taMath_double>
#include <Random>

void ActrActParams::Initialize() {
  learn = false;
  decay = 0.5f;
  inst_noise = 0.0f;
  perm_noise = 0.0f;
  init = 0.0f;
  n_finst = 4;
  finst_span = 3.0f;
}

void ActrRetrievalParams::Initialize() {
  thresh = 0.0f;
  time_pow = 1.0f;
  time_gain = 1.0f;
}

void ActrPartialParams::Initialize() {
  on = false;
  mismatch_p = 1.0f;            // todo: std val?
  max_diff = -1.0f;
  max_sim = 0.0f;
}

void ActrAssocParams::Initialize() {
  on = false;
  max_str = 1.0f;               // todo: std val?
  neg_ok = false;
}


void ActrDeclarativeModule::Initialize() {
  trace_level = NO_TRACE;
}

void ActrDeclarativeModule::InitModule() {
  if((bool)buffer) return;
  if(!Model()) return;
  ActrModel* mod = Model();
  bool made_new;
  buffer = mod->buffers.FindMakeNameType("retrieval", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  if(made_new) {
    buffer->act_total = 0.0f;
  }
}

void ActrDeclarativeModule::Init() {
  inherited::Init();
  active.Reset();
  retrieved = NULL;
  finsts.Reset();
  eligible.Reset();
  AddInitChunks();
}

void ActrDeclarativeModule::AddInitChunks() {
  active.Reset();
  for(int i=0; i<init_chunks.size; i++) {
    ActrChunk* ck = init_chunks.FastEl(i);
    AddChunk(ck, false);        // false = no merge
  }
}

void ActrDeclarativeModule::ProcessEvent(ActrEvent& event) {
  if(event.action == "MODULE-REQUEST") {
    RetrievalRequest(event);
  }
  else if(event.action == "RETRIEVED-CHUNK") {
    RetrievedChunk(event);
  }
  else if(event.action == "RETRIEVAL-FAILURE") {
    RetrievalFailure(event);
  }
  else {
    ProcessEvent_std(event);   // respond to regular requests
  }
}

void ActrDeclarativeModule::RetrievalRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck, "RetrievalRequest", "probe chunk is NULL")) {
    return;
  }

  ActrModel* mod = Model();

  if(HasModuleFlag(BUSY)) {
    TestWarning(true, "RetrievalRequest",
                "a retrieval request was made while still busy retrieving previous request -- new request should preempt but currently new request is rejected");
    mod->LogEvent(-1.0f, "declarative", "ABORT-RETRIEVAL", "", "");
    return;
  }

  mod->LogEvent(-1.0f, "declarative", "START-RETRIEVAL", "", "");
  SetModuleFlag(BUSY);
  ClearModuleFlag(ERROR);
  buffer->SetReq();
  buffer->ClearChunk();         // always clear before recall

  RemoveOldFinsts();            // update the list to current time

  ComputeAct();                 // compute the activations for everyone

  Recency recency = NO_RECENCY;
  if(event.params.contains("recently_retrieved ")) {
    String val = event.params.after("recently_retrieved ");
    if(val.contains("reset")) {
      finsts.Reset();
    }
    else if(val.contains("nil") || val.contains("false")) {
      recency = NOT_RECENT;
    }
    else {
      recency = RECENT;
    }
  }

  bool got_some = FindMatchingRetrieval(ck, recency);
  float rt = 0.0f;            // hard coded default latency for failure if non-esc
  if(!got_some) {
    if(mod->params.enable_sub_symbolic) {
      rt = ret.GetRt(ret.thresh);
    }
    mod->ScheduleEvent(rt, ActrEvent::max_pri, this, this, buffer,
                       "RETRIEVAL-FAILURE", "", event.act_arg);
    return;
  }
  if(eligible.size == 1) {
    retrieved = eligible.FastEl(0);
  }
  else {
    ChooseFromEligible();
  }

  if(mod->params.enable_sub_symbolic) {
    rt = ret.GetRt(retrieved->act.act);
  }

  mod->ScheduleEvent(rt, ActrEvent::max_pri, this, this, buffer,
                     "RETRIEVED-CHUNK", retrieved->name, event.act_arg,
                     retrieved);
}

void ActrDeclarativeModule::RetrievedChunk(ActrEvent& event) {
  ActrModel* mod = Model();
  retrieved->SetChunkFlag(ActrChunk::RETRIEVED);
  retrieved->ChunkActivated();      // increment use
  if(mod->UpdateGui()) {
    retrieved->SigEmitUpdated();
  }
  UpdateFinsts();                 // uses retrieved

  buffer->UpdateChunk(retrieved); // should be clear..
  ClearModuleFlag(BUSY);
  buffer->ClearReq();
}

void ActrDeclarativeModule::RetrievalFailure(ActrEvent& event) {
  ClearModuleFlag(BUSY);
  SetModuleFlag(ERROR);
  buffer->ClearReq();
}

void ActrDeclarativeModule::ComputeAct() {
  ComputeBaseAct();
  ComputeSpreadAct();
  ComputePartialAct();
  ComputeTotalAct();
}

void ActrDeclarativeModule::ComputeBaseAct() {
  if(!act.learn) return;
  ActrModel* mod = Model();
  const float cur_t = mod->cur_time;
  for(int i=0; i<active.size; i++) {
    ActrChunk* oc = active.FastEl(i);
    oc->ComputeBaseAct(cur_t, act.decay);
  }
}

void ActrDeclarativeModule::ComputeSpreadAct() {
  if(!assoc.on) return;
  if(TestError(assoc.on, "ComputeSpreadAct",
               "spreading activation computation not yet implemented -- please turn assoc.on to off for now")) {
    return;
  }
}

void ActrDeclarativeModule::ComputePartialAct() {
  if(!partial.on) return;
  if(TestError(partial.on, "ComputePartialAct",
               "partial matching activation computation not yet implemented -- please turn partial.on to off for now")) {
    return;
  }
}

void ActrDeclarativeModule::ComputeTotalAct() {
  ActrModel* mod = Model();
  const float cur_t = mod->cur_time;
  for(int i=0; i<active.size; i++) {
    ActrChunk* oc = active.FastEl(i);
    float instn = act.inst_noise * taMath_double::gauss_dev();
    oc->act.ComputeAct(instn);
  }
}

void ActrDeclarativeModule::RemoveOldFinsts() {
  ActrModel* mod = Model();
  for(int i=finsts.size-1; i >= 0; i--) {
    ActrChunk* oc = finsts.FastEl(i);
    if(mod->cur_time - oc->time.t_ret > act.finst_span) {
      oc->ClearChunkFlag(ActrChunk::RECENT);
      finsts.RemoveIdx(i);
    }
  }
}

void ActrDeclarativeModule::UpdateFinsts() {
  ActrModel* mod = Model();
  if(finsts.size < act.n_finst) {
    finsts.Link(retrieved);      // just add it
  }
  else {
    float oldest_t = 0.0f;
    int oldest_i = 0;
    for(int i=finsts.size-1; i >= 0; i--) {
      ActrChunk* oc = finsts.FastEl(i);
      float tm = mod->cur_time - oc->time.t_ret;
      if(tm > oldest_t) {
        oldest_t = tm;
        oldest_i = i;
      }
    }
    finsts.FastEl(oldest_i)->ClearChunkFlag(ActrChunk::RECENT);
    finsts.ReplaceLinkIdx(oldest_i, retrieved); // replace the oldest
  }
  retrieved->SetChunkFlag(ActrChunk::RECENT);
}

bool ActrDeclarativeModule::FindMatchingRetrieval(ActrChunk* ck, Recency recency) {
  ActrModel* mod = Model();
  eligible.Reset();
  ActrChunk_List* lst = &active;
  if(recency == RECENT)
    lst = &finsts;
  for(int i=0; i<lst->size; i++) {
    ActrChunk* oc = lst->FastEl(i);
    if(recency == NOT_RECENT) {
      if(oc->HasChunkFlag(ActrChunk::RECENT)) continue; // skip
    }
    if(mod->params.enable_sub_symbolic && oc->act.act < ret.thresh) {
      continue;
    }
    bool updt = false;
    if(mod->UpdateGui()) {
      if(oc->HasChunkFlag(ActrChunk::ELIGIBLE) ||
         oc->HasChunkFlag(ActrChunk::RETRIEVED))
        updt = true;
    }
    oc->ClearChunkFlag(ActrChunk::ELIGIBLE);
    oc->ClearChunkFlag(ActrChunk::RETRIEVED);
    // todo: partial matching
    if(ck->MatchesMem(oc, false)) { // false = not exact..
      eligible.Link(oc);
      oc->SetChunkFlag(ActrChunk::ELIGIBLE);
    }
    if(updt && mod->UpdateGui()) {
      oc->SigEmitUpdated();
    }
  }
  return (eligible.size > 0);
}

void ActrDeclarativeModule::ChooseFromEligible() {
  int best_idx = 0;
  float best_act = -1.0e6f;
  int_Array ties;
  ActrModel* mod = Model();
  for(int i=0; i<eligible.size; i++) {
    ActrChunk* pr = eligible.FastEl(i);
    float eff_act = pr->act.act;
    if(!mod->params.enable_sub_symbolic) {
      eff_act = 0.0f;   // will turn everything into a tie and engage that mech
    }
    if(eff_act > best_act) {
      ties.Reset();
      ties.Add(i);              // we always want us to be in the tie set
      best_idx = i;
      best_act = eff_act;
    }
    else if(eff_act == best_act) { // keep track of ties
      ties.Add(i);
    }
  }
  if(ties.size > 1) {
    if(mod->params.enable_rnd) {
      int choose = Random::IntZeroN(ties.size);
      retrieved = eligible.FastEl(ties[choose]);
    }
    else {
      retrieved = eligible.FastEl(best_idx); // just choose first
    }
  }
  else {
    retrieved = eligible.FastEl(best_idx);
  }
}

bool ActrDeclarativeModule::FindMatchingBasic(ActrChunk* ck, ActrChunk_List& lst,
                                              ActrChunk_List& matches) {
  ActrModel* mod = Model();
  matches.Reset();
  for(int i=0; i<lst.size; i++) {
    ActrChunk* oc = lst.FastEl(i);
    if(ck->MatchesMem(oc, false)) { // false = not exact..
      matches.Link(oc);
    }
  }
  return (matches.size > 0);
}

bool ActrDeclarativeModule::AddChunk(ActrChunk* ck, bool merge) {
  ActrModel* mod = Model();
  if(merge) {
    bool match = FindMatchingBasic(ck, active, tmp_match);
    if(!match || tmp_match.size > 1) { // none or too many
      tmp_match.Reset();
      AddChunk(ck, false);      // go through non-merge branch
      return false;             // not merged
    }
    // todo: what to do to merge values??
    ActrChunk* mck = tmp_match[0];
    mck->ChunkActivated();      // increment use
  }
  else {
    ActrChunk* nw_ck = (ActrChunk*)ck->Clone(); // make a copy
    nw_ck->CopyName(ck);
    active.Add(nw_ck);
    nw_ck->NewDMChunk(mod->cur_time);
    // todo: need to fix noise equivalencies!
    nw_ck->act.noise = act.perm_noise * taMath_double::gauss_dev();
    if(!act.learn) {
      nw_ck->act.base = act.init;
    }
  }
  return true;
}

