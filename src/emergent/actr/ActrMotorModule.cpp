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

#include "ActrMotorModule.h"
#include <ActrModel>
#include <ActrSlot>

void ActrMotorParams::Initialize() {
  cursor_noise = false;
  def_targ_width = 1.0f;
  inc_mouse_move = 0.0f;
}

void ActrMotorTimeParams::Initialize() {
  feat_prep = 0.05f;
  init = 0.05f;
  burst = 0.05f;
  min_fitts = 0.1f;
  peck_fitts_coeff = 0.075f;
}

void ActrMotorModule::Initialize() {
  hand_pos.SetGeom(3, 2, 5, 2);
}

void ActrMotorModule::InitModule() {
  if((bool)buffer) return;
  if(!Model()) return;
  ActrModel* mod = Model();
  bool made_new;
  buffer = mod->buffers.FindMakeNameType("manual", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  if(made_new) {
    buffer->act_total = 0.0f;   // manual-activation
  }

  ActrChunkType* ck = NULL;
  mod->DefineChunkType("motor_command", "");
  mod->DefineChunkType("click-mouse", "motor-command");
  mod->DefineChunkType("hand-to-mouse", "motor-command");
  mod->DefineChunkType("hand-to-home", "motor-command");
  mod->DefineChunkType("move-cursor", "motor-command", "object", "loc", "device");
  mod->DefineChunkType("peck", "motor-command", "hand",  "finger", "r", "theta");
  mod->DefineChunkType("peck-recoil", "motor-command", "hand", "finger", "r",
                       "theta");
  mod->DefineChunkType("point-hand-at-key", "motor-command", "hand", "to_key");
  mod->DefineChunkType("press-key", "motor-command", "key");
  mod->DefineChunkType("punch", "motor-command", "hand", "finger");
  mod->DefineChunkType("prepare", "motor-command", "style", "hand",  "finger",
                       "r", "theta");
  mod->DefineChunkType("execute", "motor-command");
  mod->DefineChunkType("clear", "");
}

void ActrMotorModule::Init() {
  inherited::Init();
  prep_act.ResetChunk();
  init_act.ResetChunk();
  exec_act.ResetChunk();
  last_cmd = "";
  InitHandPos();
  buffer->UpdateState();
}

void ActrMotorModule::InitHandPos() {
  int inits[] = { 4,4, 3,4, 2,4, 1,4, 5,6,
                  7,4, 8,4, 9,4, 10,4, 6,6 };
  
  hand_pos.InitFromInts(inits);
}

void ActrMotorModule::InitPosToKey() {
  pos_to_key.SetGeom(2, 23, 7);
  const char* keys[] = {
    // function key row
    "escape", "", "F1", "F2", "F3", "F4", "", "F5", "F6", "F7", "F8", "",
    "F9", "F10", "F11", "F12", "", "F13", "F14", "F15", "", "", "",

    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
    "", "", "", "", "", "", "", "", "",

    "backquote", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "hyphen", "=", "delete", "",
    "help", "home", "pageup", "",
    "clear", "=", "/", "*", 

    "tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "backslash ", "",
    "forward-delete", "end", "pagedn", "",
    "keypad-7", "keypad-8", "keypad-9", "keypad-hyphen", 

    "caps-lock", "A", "S", "D", "F", "G", "H", "J", "K", "L", "semicolon", "quote", "return", "return", "",
    "", "", "", "", 
    "keypad-4", "keypad-5", "keypad-6", "keypad-plus",

    "shift", "Z", "X", "C", "V", "B", "N", "M", "comma", "period", "dot ", "/", "right-shift", "right-shift", "", "",
    "", "up-arrow", "", "",
    "keypad-1", "keypad-2", "keypad-3", "keypad-enter",

    "left-control", "left-option", "left-command", "spc", "spc", "spc", "spc", "space", "spc", "spc", "spc", "spc", 
    "right-command", "right-option", "right-control", "",
    "left-arrow", "down-arrow", "right-arrow", "",
    "keypad-0", "keypad-period", "enter" };

  pos_to_key.InitFromChars(keys);
} 

String ActrMotorModule::PosToKey(int col, int row) {
  if(col == 28 && row == 2) return "mouse";
  if(TestError(col > 22 || col < 0, "PosToKey", "column out of range: 0..22"))
    return _nilString;
  if(TestError(row > 6 || row < 0, "PosToKey", "row out of range: 0..6"))
    return _nilString;
  return pos_to_key.FastEl(col, row);
}

bool ActrMotorModule::KeyToPos(int& col, int& row, const String& key) {
  if(key == "mouse") {
    col = 28; row = 2;
    return true;
  }
  int idx = pos_to_key.FindVal_Flat(key);
  if(TestError(idx < 0, "KeyToPos", "key named:", key, "not found")) {
    return false;
  }
  col = idx % 23;
  row = idx / 23;
  return true;
}

ActrMotorModule::Hand ActrMotorModule::StringToHand(const String& str) {
  if(str.startsWith('r') || str.startsWith('R')) {
    return RIGHT;
  }
  return LEFT;
}

ActrMotorModule::Finger ActrMotorModule::StringToFinger(const String& str) {
  if(str.startsWith('i') || str.startsWith('I')) {
    return INDEX;
  }
  else if(str.startsWith('m') || str.startsWith('M')) {
    return MIDDLE;
  }
  else if(str.startsWith('r') || str.startsWith('R')) {
    return RING;
  }
  else if(str.startsWith('p') || str.startsWith('P')) {
    return PINKIE;
  }
  return THUMB;                 // all thumbs..
}

bool ActrMotorModule::CurFingerPos(int& col, int& row, Hand hand, Finger finger) {
  if(TestError(!hand_pos.InRange(ROW, finger, hand), "CurFingerPos",
               "hand:", (String)hand, "or finger:", (String)finger, "is out of range")) {
    return false;
  }
  col = hand_pos.SafeEl(COL, finger, hand);
  row = hand_pos.SafeEl(ROW, finger, hand);
  return true;
}

bool ActrMotorModule::SetFingerPos(int col, int row, Hand hand, Finger finger) {
  if(TestError(!hand_pos.InRange(ROW, finger, hand), "CurFingerPos",
               "hand:", (String)hand, "or finger:", (String)finger, "is out of range")) {
    return false;
  }
  hand_pos.FastEl(COL, finger, hand) = col;
  hand_pos.FastEl(ROW, finger, hand) = row;
  return true;
}

String ActrMotorModule::CurFingerKey(Hand hand, Finger finger) {
  int col, row;
  if(!CurFingerPos(col, row, hand, finger)) return _nilString;
  return PosToKey(col, row);
}

bool ActrMotorModule::SetFingerKey(const String& key, Hand hand, Finger finger) {
  int col, row;
  if(!KeyToPos(col, row, key)) return false;
  return SetFingerPos(col, row, hand, finger);
}

String ActrMotorModule::CurFingerAll(int& col, int& row, Hand hand, Finger finger) {
  if(!CurFingerPos(col, row, hand, finger)) return _nilString;
  return PosToKey(col, row);
}

bool ActrMotorModule::HandOnMouse(Hand hand) {
  return CurFingerKey(hand, INDEX) == "mouse";
}

void ActrMotorModule::HandToMouse(Hand hand) {
  SetFingerKey("mouse", hand, INDEX);
}

void ActrMotorModule::ProcessEvent(ActrEvent& event) {
  if(event.action == "MODULE-REQUEST") {
    MotorRequest(event);
  }
  else {
    ProcessEvent_std(event);   // respond to regular requests
  }
}

void ActrMotorModule::MotorRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck || !ck->chunk_type, "MotorRequest", "chunk is NULL")) {
    return;
  }
  ActrModel* mod = Model();

  if(IsPrep()) {
    TestWarning(true, "MotorRequest",
                "a motor request was made while still preparing previous request -- new request ignored");
    mod->LogEvent(-1.0f, "motor", "ABORT-MOTOR-REQ", "", "");
    return;
  }

  if(event.action == "PREPARATION-COMPLETE") {
    init_act = prep_act;
    ClearModuleFlag(PREP);
    SetModuleFlag(EXEC);
    mod->ScheduleEvent(timing.init, ActrEvent::max_pri, this, this, event.dst_buffer,
                       "INITIATION-COMPLETE", event.params, event.act_arg,
                       ck);
  }
  else if(event.action == "INITIATION-COMPLETE") {
    exec_act = init_act;
    ClearModuleFlag(PROC);
    mod->ScheduleEvent(timing.burst, ActrEvent::max_pri, this, this, event.dst_buffer,
                       "EXECUTE-ACTION", event.params, event.act_arg,
                       ck);
  }
  else if(event.action == "EXECUTE-ACTION") {
    if(event.params == "punch") {
      ExecPunch(event);
    }
    else if(event.params == "click_mouse") {
      ExecClickMouse(event);
    }
  }
  else if(event.action == "FINISH-MOVEMENT") {
    ClearModuleFlag(EXEC);
  }
  else if(ck->name == "clear" ||
     ck->chunk_type->InheritsFromCTName("clear")) {
    ClearRequest(event);
  }
  else if(ck->name == "punch" ||
     ck->chunk_type->InheritsFromCTName("punch")) {
    StdMotorRequest(event, "punch");
  }
  else if(ck->name == "click_mouse" ||
     ck->chunk_type->InheritsFromCTName("click_mouse")) {
    StdMotorRequest(event, "click_mouse");
  }
}

void ActrMotorModule::ClearRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();
  last_cmd = "clear";

  // todo: clear prepared motor features
  ClearModuleFlag(EXEC);
  ClearModuleFlag(PROC);
  ClearModuleFlag(ERROR);
  SetModuleFlag(PREP);

  mod->ScheduleEvent(0.05f, ActrEvent::max_pri, this, this, event.dst_buffer,
                     "CLEAR-STATE", "LAST NONE PREP FREE", event.act_arg,
                     ck);

  ClearModuleFlag(BUSY);
  buffer->ClearReq();
}

void ActrMotorModule::StdMotorRequest(ActrEvent& event, const String& cmd) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  last_cmd = cmd;

  SetModuleFlag(PREP);
  SetModuleFlag(PROC);

  prep_act = *ck;

  // todo: figure out feature preparation time??
  mod->ScheduleEvent(timing.feat_prep, ActrEvent::max_pri, this, this,
                     event.dst_buffer,
                     "PREPARATION-COMPLETE", cmd, event.act_arg,
                     ck);

  ClearModuleFlag(BUSY);
  buffer->ClearReq();   
}  
    
void ActrMotorModule::ExecPunch(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  String hstr = ck->GetSlotVal("hand").toString();
  String fstr = ck->GetSlotVal("finger").toString();
 
  int row, col;
  String key;
  key = CurFingerAll(col, row, StringToHand(hstr), StringToFinger(fstr));

  String out_str = key + " #(" + String(row) + " " + String(col) + ")"; 

  mod->LogEvent(-1.0f, "motor", "OUTPUT-KEY", "", out_str);
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH-MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecClickMouse(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  if(TestWarning(!HandOnMouse(), "Motor", "CLICK-MOUSE requested when hand not at mouse!")) {
    return;
  }

  int row, col;
  String key;
  key = CurFingerAll(col, row, RIGHT, INDEX);

  String out_str = key + " #(" + String(row) + " " + String(col) + ")"; 

  mod->LogEvent(-1.0f, "motor", "OUTPUT-KEY", "", out_str);
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH-MOVEMENT", event.params, event.act_arg,
      ck);
}

bool ActrMotorModule::ProcessQuery(ActrBuffer* buf, const String& query, bool why_not) {
  return ProcessQuery_std(buf, query, why_not);
}


bool ActrMotorModule::SetParam(const String& param_nm, Variant par1, Variant par2) {
  bool got = false;
  if(param_nm == "motor_activation" && buffer) {
    buffer->act_total = par1.toFloat();
    got = true;
  }
  else if(param_nm == "cursor_noise") {
    params.cursor_noise = par1.toBool();
    got = true;
  }
  else if(param_nm == "default_target_width") {
    params.def_targ_width = par1.toFloat();
    got = true;
  }
  else if(param_nm == "incremental_mouse_moves") {
    params.inc_mouse_move = par1.toFloat();
    got = true;
  }
  else if(param_nm == "motor_feature_prep_time") {
    timing.feat_prep = par1.toFloat();
    got = true;
  }
  else if(param_nm == "motor_initiation_time") {
    timing.init = par1.toFloat();
    got = true;
  }
  else if(param_nm == "motor_burst_time") {
    timing.burst = par1.toFloat();
    got = true;
  }
  else if(param_nm == "min_fitts_time") {
    timing.min_fitts = par1.toFloat();
    got = true;
  }
  else if(param_nm == "peck_fitts_coeff") {
    timing.peck_fitts_coeff = par1.toFloat();
    got = true;
  }

  return got;
}
