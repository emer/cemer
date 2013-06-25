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

#include "ActrModel.h"

#include <ActrDeclarativeModule>
#include <ActrProceduralModule>
#include <ActrGoalModule>
#include <ActrImaginalModule>
#include <ActrVisionModule>
#include <taFiler>

#include "actr_parse.h"

#include <taMisc>

void ActrGlobalParams::Initialize() {
  enable_sub_symbolic = false;
  enable_rnd = false;
}

NameVar_Array ActrModel::load_keywords;
ActrModelRef  ActrModel::cur_parse;

void ActrModel::Initialize() {
  cur_time = 0.0f;
  cur_event_idx = 0;
  flags = (ModelFlags)(LOG_EVENTS | UPDATE_GUI);
  run_state = NOT_INIT;

  load_debug = 0;
  load_line = load_col = load_pos = 0;
  load_st_line = load_st_col = load_st_pos = load_st_line_pos = 0;
}

void ActrModel::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ActrModel);
  DefaultConfig();
  InitLoadKeywords();
}

void ActrModel::CutLinks() {
  CutLinks_taAuto(&TA_ActrModel);
  inherited::CutLinks();
}

void ActrModel::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  DefaultConfig();
  FormatLogTable();
}

ActrModule* ActrModel::FindMakeModule(const String& nm, TypeDef* td,
                                      bool& made_new) {
  ActrModule* rval = modules.FindMakeNameType(nm, td, made_new);
  if(made_new && rval) {
    rval->InitModule();
  }
  return rval;
}

ActrDeclarativeModule* ActrModel::DeclarativeModule() {
  ActrDeclarativeModule* dmod =
    (ActrDeclarativeModule*)modules.FindName("declarative");
  if(TestError(!dmod, "DeclarativeModule", "declarative module not found -- model not yet initialized"))
    return NULL;
  return dmod;
}

ActrProceduralModule* ActrModel::ProceduralModule() {
  ActrProceduralModule* dmod =
    (ActrProceduralModule*)modules.FindName("procedural");
  if(TestError(!dmod, "ProceduralModule", "procedural module not found -- model not yet initialized"))
    return NULL;
  return dmod;
}

ActrGoalModule* ActrModel::GoalModule() {
  ActrGoalModule* dmod =
    (ActrGoalModule*)modules.FindName("goal");
  if(TestError(!dmod, "GoalModule", "goal module not found -- model not yet initialized"))
    return NULL;
  return dmod;
}

ActrImaginalModule* ActrModel::ImaginalModule() {
  ActrImaginalModule* dmod =
    (ActrImaginalModule*)modules.FindName("imaginal");
  if(TestError(!dmod, "ImaginalModule", "imaginal module not found -- model not yet initialized"))
    return NULL;
  return dmod;
}

ActrVisionModule* ActrModel::VisionModule() {
  ActrVisionModule* dmod =
    (ActrVisionModule*)modules.FindName("vision");
  if(TestError(!dmod, "VisionModule", "vision module not found -- model not yet initialized"))
    return NULL;
  return dmod;
}

void ActrModel::DefaultConfig() {
  bool made_new;
  FindMakeModule("procedural", &TA_ActrProceduralModule, made_new);
  FindMakeModule("declarative", &TA_ActrDeclarativeModule, made_new);
  FindMakeModule("goal", &TA_ActrGoalModule, made_new);
  FindMakeModule("imaginal", &TA_ActrImaginalModule, made_new);
  FindMakeModule("vision", &TA_ActrVisionModule, made_new);
}

bool ActrModel::CheckConfig_impl(bool quiet) {
  bool rval = inherited::CheckConfig_impl(quiet);
  return rval;
}

void ActrModel::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  modules.CheckConfig(quiet, rval);
}

void ActrModel::Init() {
  FormatLogTable();
  if(log_table) {
    log_table->ResetData();
  }
  cur_event_idx = 0;
  events.Reset();
  cur_time = 0.0f;
  for(int i=0; i<modules.size; i++) {
    modules.FastEl(i)->Init();  // module-specific run init
  }
  CheckConfig(false);
  if(!taMisc::check_ok)
    run_state = NOT_INIT;
  else
    run_state = DONE;
  SigEmitUpdated();
}

void ActrModel::Step() {
  if(run_state == NOT_INIT || run_state == DONE) {
    Init();
  }
  run_state = RUN;
  SigEmitUpdated();
  RunNextEvent();
  run_state = STOP;
  SigEmitUpdated();
}
 
void ActrModel::Run() {
  if(run_state == NOT_INIT || run_state == DONE) {
    Init();
  }
  run_state = RUN;
  SigEmitUpdated();
  while(run_state != STOP) {
    RunNextEvent();
    taMisc::ProcessEvents();    // todo: optimize this!
  }
  run_state = DONE;
  SigEmitUpdated();
}

void ActrModel::Cont() {
  if(run_state == NOT_INIT || run_state == DONE) {
    Init();
  }
  run_state = RUN;
  SigEmitUpdated();
  while(run_state != STOP) {
    RunNextEvent();
    taMisc::ProcessEvents();    // todo: optimize this!
  }
  // key diff: don't set DONE here!
  SigEmitUpdated();
}

void ActrModel::Stop() {
  run_state = STOP;
  SigEmitUpdated();
}

void ActrModel::RunNextEvent() {
  if(cur_event_idx >= events.size) {
    cur_event_idx = events.size; // make sure..
    // no more events!
    ActrProceduralModule* proc = (ActrProceduralModule*)modules.FindName("procedural");
    if(!proc) {                 // should not happen
      Stop();
      return;
    }
    proc->AddConflictResEvent(); // if nothing else, schedule some conflict resolution!!
  }
  ActrEvent* ev = events.SafeEl(cur_event_idx++); // should definitely work now..
  if(!ev || ev->action == "!stop!") {
    Stop();
    return;
  }
  if(TestError(!ev->dst_module, "RunNextEvent",
               "no destination module in event!  programmer error!")) {
    Stop();
    return;
  }

  cur_time = ev->time;          // set current time to event time
  if(HasModelFlag(LOG_EVENTS) && log_table) {
    ev->LogEvent(*log_table.ptr());
  }
  ev->dst_module->ProcessEvent(*ev);

  if(!HasModelFlag(SAVE_ALL_EVENTS)) {
    if(cur_event_idx >= events.size) { // cull if we run out
      events.Reset();
      cur_event_idx = 0;
    }
    else if(cur_event_idx > 100) {
      // todo: copy remaining items to a tmp list, then reset and move back
      // write a separate function to do this
    }
  }
}

int ActrModel::InsertEventInOrder(ActrEvent* ev) {
  for(int i=cur_event_idx; i<events.size; i++) {
    ActrEvent* oe = events.FastEl(i);
    if(ev->time > oe->time) continue;
    if(ev->priority < oe->priority) continue;
    events.Insert(ev, i);              // this is our spot
    return i;
  }
  events.Add(ev);
  return events.size-1;
}

ActrEvent* ActrModel::ScheduleEvent(float time_fm_now, int priority,
                                    ActrModule* src_mod, ActrModule* dst_mod,
                                    ActrBuffer* dst_buf, const String& action,
                                    const String& params,
                                    ActrAction* act, ActrChunk* chnk,
                                    TypeDef* event_type) {
  ActrEvent* ev = ActrEvent::NewEvent(cur_time + time_fm_now, priority,
                                      src_mod, dst_mod, dst_buf, action,
                                      params, act, chnk, event_type);
  InsertEventInOrder(ev);
  return ev;
}

void ActrModel::LogEvent(float time, const String& module,
                         const String& action, const String& target, 
                         const String& params, const String& dst_module,
                         float priority, const String& prod_action,
                         const String& chunk) {
  if(!log_table) return;
  DataTable* dt = log_table.ptr();
  dt->AddBlankRow();
  if(time < 0) time = cur_time;
  dt->SetVal(time, "time", -1);
  dt->SetVal(module, "module", -1);
  dt->SetVal(action, "action", -1);
  dt->SetVal(target, "target", -1);
  dt->SetVal(params, "params", -1);
  dt->SetVal(dst_module, "dst_module", -1);
  dt->SetVal(priority, "priority", -1);
  dt->SetVal(prod_action, "prod_action", -1);
  dt->SetVal(chunk, "chunk", -1);
  dt->WriteClose();
}

void ActrModel::FormatLogTable() {
  if(!log_table) return;
  DataTable* dt = log_table.ptr();

  DataCol* dc;
  dc = dt->FindMakeCol("time", VT_FLOAT);
  dc->desc = "time in seconds when this event occurred";
  dc = dt->FindMakeCol("module", VT_STRING);
  dc->desc = "module that generated this event or action";
  dc = dt->FindMakeCol("action", VT_STRING);
  dc->desc = "name of action";
  dc = dt->FindMakeCol("target", VT_STRING);
  dc->desc = "name of buffer or module that is the target or destination for the action";
  dc = dt->FindMakeCol("params", VT_STRING);
  dc->desc = "extra parameters associated with the action";
  dc = dt->FindMakeCol("dst_module", VT_STRING);
  dc->desc = "destination module that will process this event or action";
  dc = dt->FindMakeCol("priority", VT_FLOAT);
  dc->desc = "event priority for scheduling";
  dc = dt->FindMakeCol("prod_action", VT_STRING);
  dc->desc = "extra detail on production action that created event";
  dc = dt->FindMakeCol("chunk", VT_STRING);
  dc->desc = "extra detail on chunk that is relevant to this event or action";
}


ActrChunkType* ActrModel::FindChunkType(const String& type_name) {
  if(type_name == "chunk") {    // special case
    bool made_new = false;
    ActrChunkType* ct = chunk_types.FindMakeNameType(type_name, NULL, made_new);
    return ct;
  }
  ActrChunkType* ct = chunk_types.FindName(type_name);
  if(TestError(!ct, "FindChunkType", "chunk type named:", type_name,
               "not found")) {
    return NULL;
  }
  return ct;
}

ActrChunkType* ActrModel::FindMakeChunkType(const String& type_name) {
  bool made_new = false;
  ActrChunkType* ct = chunk_types.FindMakeNameType(type_name, NULL, made_new);
  if(type_name == "chunk") {    // special case
    return ct;
  }
  TestWarning(!ct, "FindMakeChunkType", "chunk type named:", type_name,
              "was created implicitly -- was not defined in advance");
  return ct;
}

void ActrModel::SaveActrFile(const String& fname) {
  
}

int apparse(void);
void aperror(const char *s);
extern int apdebug;

bool ActrModel::LoadActrFile(const String& fname) {
  taFiler* flr = GetLoadFiler(fname, ".lisp,.actr", false);
  bool rval = false;
  if(flr->istrm) {
    ResetModel();
    cur_parse = this;
    load_str.Load_str(*flr->istrm);
    load_line = 1;
    load_col = 0;
    load_pos = 0;
    apdebug = load_debug > 2 ? 1 : 0;
    load_state = YYRet_Ok;
    while(load_state != YYRet_Exit)
      apparse();
  }
  cur_parse = NULL;
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void ActrModel::ResetModel() {
  if(log_table) {
    log_table->ResetData();
  }
  events.Reset();
  cur_event_idx = 0;
  buffers.Reset();
  modules.Reset();
  chunk_types.Reset();
  DefaultConfig();
}

void ActrModel::SetParam(const String& param_nm, Variant par1,
                         Variant par2) {
  if(par1.isStringType() && par1.toString() == "t") {
    par1 = true;
  }
  if(par1.isStringType() && par1.toString() == "nil") {
    par1 = false;
  }

  ActrProceduralModule* pmod = ProceduralModule();
  ActrDeclarativeModule* dmod = DeclarativeModule();
  ActrGoalModule* gmod = GoalModule();
  ActrImaginalModule* imod = ImaginalModule();

  bool got = false;

  if(param_nm == "esc") {
    params.enable_sub_symbolic = par1.toBool();
    got = true;
  }
  else if(param_nm == "er") {
    params.enable_rnd = par1.toBool();
    got = true;
  }
  else if(param_nm == "ul" && pmod) {
    pmod->util.learn = par1.toBool();
    got = true;
  }
  else if(param_nm == "alpha" && pmod) {
    pmod->util.lrate = par1.toFloat();
    got = true;
  }
  else if(param_nm == "iu" && pmod) {
    pmod->util.init = par1.toFloat();
    got = true;
  }
  else if(param_nm == "egs" && pmod) {
    pmod->util.noise = par1.toFloat();
    pmod->util.UpdateAfterEdit();
    got = true;
  }
  else if(param_nm == "ut" && pmod) {
    pmod->util.thresh = par1.toFloat();
    got = true;
  }
  else if(param_nm == "crt" && pmod) {
    pmod->trace_level = ActrProceduralModule::TRACE_ALL;
    got = true;
  }
  else if(param_nm == "cst" && pmod) {
    pmod->trace_level = ActrProceduralModule::TRACE_ELIG;
    got = true;
  }
  else if(param_nm == "ult" && pmod) {
    pmod->trace_level = ActrProceduralModule::UTIL_LEARN;
    got = true;
  }
  else if(param_nm == "bll" && dmod) {
    if(!par1.toBool())
      dmod->act.learn = false;
    else {
      dmod->act.learn = true;
      dmod->act.decay = par1.toFloat();
    }
    got = true;
  }
  else if(param_nm == "ans" && dmod) {
    dmod->act.inst_noise = par1.toFloat();
    got = true;
  }
  else if(param_nm == "pas" && dmod) {
    dmod->act.perm_noise = par1.toFloat();
    got = true;
  }
  else if(param_nm == "blc" && dmod) {
    dmod->act.init = par1.toFloat();
    got = true;
  }
  else if(param_nm == "declarative_num_finsts" && dmod) {
    dmod->act.n_finst = par1.toInt();
    got = true;
  }
  else if(param_nm == "declarative_finst_span" && dmod) {
    dmod->act.finst_span = par1.toInt();
    got = true;
  }
  else if(param_nm == "rt" && dmod) {
    dmod->ret.thresh = par1.toFloat();
    got = true;
  }
  else if(param_nm == "le" && dmod) {
    dmod->ret.time_pow = par1.toFloat();
    got = true;
  }
  else if(param_nm == "lf" && dmod) {
    dmod->ret.time_gain = par1.toFloat();
    got = true;
  }
  else if(param_nm == "mp" && dmod) {
    if(!par1.toBool())
      dmod->partial.on = false;
    else {
      dmod->partial.on = true;
      dmod->partial.mismatch_p = par1.toFloat();
    }
    got = true;
  }
  else if(param_nm == "md" && dmod) {
    dmod->partial.max_diff = par1.toFloat();
    got = true;
  }
  else if(param_nm == "ms" && dmod) {
    dmod->partial.max_sim = par1.toFloat();
    got = true;
  }
  else if(param_nm == "mas" && dmod) {
    if(!par1.toBool())
      dmod->assoc.on = false;
    else {
      dmod->assoc.on = true;
      dmod->assoc.max_str = par1.toFloat();
    }
    got = true;
  }
  else if(param_nm == "nsji" && dmod) {
    dmod->assoc.neg_ok = par1.toBool();
    got = true;
  }
  else if((param_nm == "act" || param_nm == "sact") && dmod) {
    if(par1.toString() == "low")
      dmod->trace_level = ActrDeclarativeModule::TRACE_MATCH;
    else
      dmod->trace_level = ActrDeclarativeModule::TRACE_ALL;
    got = true;
  }
  else if(param_nm == "ga" && gmod && gmod->buffer) {
    gmod->buffer->act_total = par1.toFloat();
    got = true;
  }
  else if(param_nm == "imaginal_activation" && imod && imod->buffer) {
    imod->buffer->act_total = par1.toFloat();
    got = true;
  }
  else if(param_nm == "imaginal_action_activation" && imod && imod->buffer) {
    imod->buffer->act_total = par1.toFloat();
    got = true;
  }

  TestWarning(!got, "SetParam", "was not able to process parameter named:",
              param_nm);
}
