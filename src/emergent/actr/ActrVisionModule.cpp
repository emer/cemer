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

#include "ActrVisionModule.h"
#include <ActrModel>

#include <Random>
#include <taMisc>

void ActrVisParams::Initialize() {
  n_finst = 4;
  finst_span = 3.0f;
  onset_span = 0.5f;
  move_tol = 0.5f;
}

void ActrAttnParams::Initialize() {
  auto_attend = false;
  latency = 0.085f;
  scene_chg_thresh = 0.25f;
}

void ActrVisionModule::Initialize() {
}

void ActrVisionModule::InitModule() {
  if((bool)buffer) return;
  if(!Model()) return;
  ActrModel* mod = Model();
  bool made_new;
  buffer = mod->buffers.FindMakeNameType("visual", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  if(made_new) {
    buffer->act_total = 0.0f;   // visual-activation
  }

  location_buffer = mod->buffers.FindMakeNameType("visual_location", NULL, made_new);
  location_buffer->module = this;
  location_buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  if(made_new) {
    location_buffer->act_total = 0.0f;   // visual-location-activation
  }

  ActrChunkType* ck = NULL;
  mod->DefineChunkType("visual_object", "", "screen_pos", "value",
                       "status", "color", "height", "width");
  mod->DefineChunkType("abstract_object", "", "value", "line_pos", "bin_pos");
  mod->DefineChunkType("abstract_letter", "abstract_object");
  mod->DefineChunkType("abstract_number", "abstract_object");
  mod->DefineChunkType("text", "visual_object");
  mod->DefineChunkType("empty_space", "visual_object");
  mod->DefineChunkType("line", "visual_object");
  mod->DefineChunkType("oval", "visual_object");
  mod->DefineChunkType("cursor", "visual_object");
  mod->DefineChunkType("phrase", "visual_object", "objects", "words", "colors");
  ck = mod->DefineChunkType("visual_location", "", "screen_x", "screen_y", "distance",
                            "kind", "color", "value", "height", "width", "size", "object");
  ck->SetSlotChunkType("object", "visual_object"); // object is pointer to visual_object
  mod->DefineChunkType("set_visloc_default", "", "type", "screen_x", "screen_y",
                       "distance", "kind", "color", "value", "height", "width",
                       "size");
  mod->DefineChunkType("char_primitive", "visual_location", "left", "right");
  mod->DefineChunkType("vision_command");
  mod->DefineChunkType("pm_constant");
  mod->DefineChunkType("color");
  ck = mod->DefineChunkType("move_attention", "vision_command", "screen_pos", "scale");
  ck->SetSlotChunkType("screen_pos", "visual_location"); // screen_pos is pointer to vis loc
  
  mod->DefineChunkType("start_tracking", "vision_command");
  mod->DefineChunkType("assign_finst", "vision_command", "object", "location");
  mod->DefineChunkType("clear_scene_change", "vision_command");
  mod->DefineChunkType("clear");

  mod->DefineChunk("lowest", "pm_constant");
  mod->DefineChunk("highest", "pm_constant");
  mod->DefineChunk("current", "pm_constant");
  mod->DefineChunk("current_x", "pm_constant");
  mod->DefineChunk("current_y", "pm_constant");
  mod->DefineChunk("clockwise", "pm_constant");
  mod->DefineChunk("counterclockwise", "pm_constant");
  mod->DefineChunk("external", "pm_constant");
  mod->DefineChunk("internal", "pm_constant");
  mod->DefineChunk("find_location", "vision_command");
  mod->DefineChunk("move_attention", "vision_command");
  mod->DefineChunk("assign_finst", "vision_command");
  mod->DefineChunk("start_tracking", "vision_command");
    
  mod->DefineChunk("black", "color");
  mod->DefineChunk("red", "color");
  mod->DefineChunk("blue", "color");
  mod->DefineChunk("green", "color");
  mod->DefineChunk("white", "color");
  mod->DefineChunk("magenta", "color");
  mod->DefineChunk("yellow", "color");
  mod->DefineChunk("cyan", "color");
  mod->DefineChunk("dark_green", "color");
  mod->DefineChunk("dark_red", "color");
  mod->DefineChunk("dark_cyan", "color");
  mod->DefineChunk("dark_blue", "color");
  mod->DefineChunk("dark_magenta", "color");
  mod->DefineChunk("dark_yellow", "color");
  mod->DefineChunk("light_gray", "color");
  mod->DefineChunk("dark_gray", "color");
        
  mod->DefineChunk("text", "chunk");
  mod->DefineChunk("box", "chunk");
  mod->DefineChunk("line", "chunk");
  mod->DefineChunk("oval", "chunk");
    
  mod->DefineChunk("new", "chunk");
  mod->DefineChunk("clear", "chunk");
}

void ActrVisionModule::Init() {
  inherited::Init();
  found = NULL;
  eligible.Reset();
  visicon.Reset();
  buffer->UpdateState();
  location_buffer->Init();
  location_buffer->UpdateState();
}

void ActrVisionModule::ProcessEvent(ActrEvent& event) {
  if(event.action == "MODULE-REQUEST") {
    VisionRequest(event);
  }
  else if(event.action == "Encoding-complete") {
    EncodingComplete(event);
  }
  // else if(event.action == "CREATE-NEW-BUFFER-CHUNK") {
  //   CreateNewChunk(event);
  // }
  // else if(event.action == "SET-BUFFER-CHUNK") {
  //   SetBufferChunk(event);
  // }
  // else if(event.action == "MOD-VISION-CHUNK") {
  //   ModVisionChunk(event);
  // }
  else {
    ProcessEvent_std(event);   // respond to regular requests
  }
}

void ActrVisionModule::VisionRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck || !ck->chunk_type, "VisionRequest", "chunk is NULL")) {
    return;
  }
  ActrModel* mod = Model();

  if(HasModuleFlag(BUSY)) {
    TestWarning(true, "VisionRequest",
                "a vision request was made while still busy activating previous request -- new request ignored");
    mod->LogEvent(-1.0f, "vision", "ABORT-IMAGINAL-REQ", "", "");
    return;
  }

  RemoveOldFinsts();            // update the list to current time

  if(event.dst_buffer == location_buffer) { // only supports find-location
    VisualLocationRequest(event);
  }
  else if(event.dst_buffer == buffer) { // supports many types
    if(ck->name == "move_attention" ||
       ck->chunk_type->InheritsFromCTName("move_attention")) {
      MoveAttentionRequest(event);
    }
    // etc.
    // else if(ck->name == "clear" || ck->InheritsFromCTName("clear")) {
    //   ClearRequest(event);
    // }
  }
}

void ActrVisionModule::VisualLocationRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  mod->LogEvent(-1.0f, "vision", "Find-location", "", "");
  last_cmd = "visual_location";

  SetModuleFlag(BUSY);
  ClearModuleFlag(ERROR);
  location_buffer->SetReq();
  location_buffer->ClearChunk();         // always clear before find

  String param = event.params;
  if(param.contains("nearest")) {
    // todo: process nearest
  }
  else if(param.contains("attended")) {
    // todo: use finsts
  }

  bool got_some = FindMatchingLocation(ck);
  if(!got_some) {
    ClearModuleFlag(BUSY);
    SetModuleFlag(ERROR);
    location_buffer->ClearReq();
    mod->LogEvent(-1.0f, "vision", "FIND-LOC-FAILURE", "", "");
    return;
  }

  if(eligible.size == 1) {
    found = eligible.FastEl(0);
  }
  else {
    ChooseFromEligibleLocations();
  }

  mod->ScheduleEvent(0.0f, ActrEvent::max_pri, this, this, location_buffer,
                     "SET-BUFFER-CHUNK", found->name, event.act_arg,
                     found);
  // todo: unclear if finsts should be activated for locations as well?
  // UpdateFinsts();                 // uses retrieved

  ClearModuleFlag(BUSY);
  location_buffer->ClearReq();

  if(attn.auto_attend) {
    // todo: technically should be a separate event instead of just log
    mod->LogEvent(-1.0f, "vision", "automatically attending", "", "");

    ActrChunk* ma = mod->chunks.FindName("move_attention");

    mod->ScheduleEvent(0.05f, ActrEvent::max_pri, this, this, buffer,
                       "Move-attention", found->name, event.act_arg,
                       ma);     // ma type triggers move

    // from the vision.lisp module, line 2600:
    // ;; Hack to clear the module state just before the attention shift so as not to 
    // ;; jam things.
         
    // (schedule-event-relative .05 'change-state :params (list :exec 'free :proc 'free) :destination :vision :output nil
    //                          :module :vision :priority 1))
  }
}

bool ActrVisionModule::FindMatchingLocation(ActrChunk* ck) {
  ActrModel* mod = Model();
  eligible.Reset();
  for(int i=0; i<visicon.size; i++) {
    ActrChunk* oc = visicon.FastEl(i);
    bool updt = false;
    if(mod->UpdateGui()) {
      if(oc->HasChunkFlag(ActrChunk::ELIGIBLE) ||
         oc->HasChunkFlag(ActrChunk::RETRIEVED))
        updt = true;
    }
    oc->ClearChunkFlag(ActrChunk::ELIGIBLE);
    oc->ClearChunkFlag(ActrChunk::RETRIEVED);
    if(ck->MatchesMem(oc, false)) { // false = not exact..
      eligible.Link(oc);
      oc->SetChunkFlag(ActrChunk::ELIGIBLE);
    }
    // todo: process highest, lowest, current
    if(updt && mod->UpdateGui()) {
      oc->SigEmitUpdated();
    }
  }
  return (eligible.size > 0);
}

void ActrVisionModule::ChooseFromEligibleLocations() {
  int best_idx = 0;
  float best_time = 1.0e6f;
  int_Array ties;
  ActrModel* mod = Model();
  for(int i=0; i<eligible.size; i++) {
    ActrChunk* pr = eligible.FastEl(i);
    float eff_t = mod->cur_time - pr->time.t_new; // how old
    if(eff_t < best_time) {
      ties.Reset();
      ties.Add(i);              // we always want us to be in the tie set
      best_idx = i;
      best_time = eff_t;
    }
    else if(eff_t == best_time) { // keep track of ties
      ties.Add(i);
    }
  }
  if(ties.size > 1) {
    if(mod->params.enable_rnd) {
      int choose = Random::IntZeroN(ties.size);
      found = eligible.FastEl(ties[choose]);
    }
    else {
      found = eligible.FastEl(best_idx); // just choose first
    }
  }
  else {
    found = eligible.FastEl(best_idx);
  }
}

void ActrVisionModule::RemoveOldFinsts() {
  ActrModel* mod = Model();
  for(int i=finsts.size-1; i >= 0; i--) {
    ActrChunk* oc = finsts.FastEl(i);
    if(mod->cur_time - oc->time.t_ret > params.finst_span) {
      oc->ClearChunkFlag(ActrChunk::RECENT);
      finsts.RemoveIdx(i);
    }
  }
}

void ActrVisionModule::UpdateFinsts(ActrChunk* attend) {
  ActrModel* mod = Model();
  if(finsts.size < params.n_finst) {
    finsts.Link(attend);      // just add it
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
    finsts.ReplaceLinkIdx(oldest_i, attend); // replace the oldest
  }
  attend->SetChunkFlag(ActrChunk::RECENT);
}

void ActrVisionModule::MoveAttentionRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  mod->LogEvent(-1.0f, "vision", "Move-attention", "", "");

  SetModuleFlag(PROC);          // move-attention = processor
  ClearModuleFlag(ERROR);
  buffer->SetReq();
  buffer->ClearChunk();         // always clear before..

  ActrChunk* moveto = NULL;
  String scale;
  if(ck->chunk_type->InheritsFromCTName("move_attention")) {
    moveto = ck->GetSlotValChunk("screen_pos");
    scale = ck->GetSlotValLiteral("scale");
  }
  else {                        // must be the command
    moveto = found;
  }
  if(!moveto) {
    ClearModuleFlag(PROC);
    SetModuleFlag(ERROR);
    buffer->ClearReq();
    mod->LogEvent(-1.0f, "vision", "MOVE-ATTN-FAILURE", "move_attention chunk not found",
                  "");
    return;
  }
  // todo: process the scale command
    
  // if(param.contains("nearest")) {
  //   // todo: process nearest
  // }
  // else if(param.contains("attended")) {
  //   // todo: use finsts
  // }

  ActrChunk* obj = moveto->GetSlotValChunk("object");
  if(!obj) {
    ClearModuleFlag(PROC);
    SetModuleFlag(ERROR);
    buffer->ClearReq();
    mod->LogEvent(-1.0f, "vision", "MOVE-ATTN-FAILURE", "object pointer not set in move_attention visual_location chunk",
                  "");
    return;
  }

  mod->ScheduleEvent(0.0f, ActrEvent::max_pri, this, this, buffer,
                     "Encoding-complete", obj->name, event.act_arg,
                     obj);
  
  // ClearModuleFlag(PROC);
  // buffer->CearReq();
}

void ActrVisionModule::EncodingComplete(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  mod->ScheduleEvent(0.0f, ActrEvent::max_pri, this, this, buffer,
                     "SET-BUFFER-CHUNK", ck->name, event.act_arg,
                     ck);
  
  ClearModuleFlag(PROC);
  buffer->ClearReq();
}

bool ActrVisionModule::ProcessQuery(ActrBuffer* buf, const String& query, bool why_not) {
  String quer = query;
  bool neg = false;
  if(quer.endsWith('-')) {
    quer = quer.before('-',-1);
    neg = true;
  }
  bool rval = false;
  ActrModel* mod = Model();
  if(quer == "attended" || quer == "attended t") {
    if(location_buffer->IsFull()) {
      ActrChunk* ck = location_buffer->CurChunk();
      rval = ck->HasChunkFlag(ActrChunk::RECENT); // attended = full and finst
    }
    else {
      rval = false;
    }
  }
  else if(quer == "unattended" || quer == "attended nil") {
    if(location_buffer->IsFull()) {
      ActrChunk* ck = location_buffer->CurChunk();
      rval = !ck->HasChunkFlag(ActrChunk::RECENT);
    }
    else {
      rval = true;
    }
  }
  else if(quer == "newly_attended" || quer == "attended new") {
    if(location_buffer->IsFull()) {
      ActrChunk* ck = location_buffer->CurChunk();
      float tm = mod->cur_time; // todo: get time
      rval = tm <= params.onset_span;
    }
    else {
      rval = false;
    }
  }
  else if(quer == "scene_change" || quer == "scene_change t") {
    // todo: check if schene changed within past onset_span seconds
  }
  else if(quer == "no_scene_change" || quer == "scene_change not") {
    // todo: check if not schene changed within past onset_span seconds
  }
  else if(quer.startsWith("scene_change_value")) {
    String valstr = quer.after("scene_change_value");
    float val = (float)valstr;
    // todo: check if not schene changed within past onset_span seconds
  }
  // todo: last_command
  else {
    return ProcessQuery_std(buf, query, why_not);
  }
  if(neg) rval = !rval;
  if(!rval && why_not) {
    taMisc::Info("module:",GetDisplayName(), "buffer:", buf->GetDisplayName(),
                 "query:", query, "returned false");
  }
  return rval;
}


bool ActrVisionModule::SetParam(const String& param_nm, Variant par1, Variant par2) {
  bool got = false;
  if(param_nm == "visual_activation" && buffer) {
    buffer->act_total = par1.toFloat();
    got = true;
  }
  else if(param_nm == "visual_location_activation" && location_buffer) {
    location_buffer->act_total = par1.toFloat();
    got = true;
  }
  else if(param_nm == "visual_num_finsts") {
    params.n_finst = par1.toInt();
    got = true;
  }
  else if(param_nm == "visual_finst_span") {
    params.finst_span = par1.toFloat();
    got = true;
  }
  else if(param_nm == "visual_onset_span") {
    params.onset_span = par1.toFloat();
    got = true;
  }
  else if(param_nm == "visual_movement_tolerance") {
    params.move_tol = par1.toFloat();
    got = true;
  }
  else if(param_nm == "auto_attend") {
    attn.auto_attend = par1.toBool();
    got = true;
  }
  else if(param_nm == "visual_attention_latency") {
    attn.latency = par1.toFloat();
    got = true;
  }
  else if(param_nm == "scene_change_threshold") {
    attn.scene_chg_thresh = par1.toFloat();
    got = true;
  }

  return got;
}


void ActrVisionModule::AddToVisIcon(ActrChunk* ck) {
  ActrModel* mod = Model();
  visicon.Add(ck);
  ck->time.t_new = mod->cur_time;
  ck->time.t_ret = -1.0f;
  ck->ClearChunkFlag(ActrChunk::ALL_STATE_FLAGS);
}
