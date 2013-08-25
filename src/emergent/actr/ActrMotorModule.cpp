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
  mod->DefineChunkTypeSys("motor_command", "");
  mod->DefineChunkTypeSys("click_mouse", "motor_command");
  mod->DefineChunkTypeSys("hand_to_mouse", "motor_command");
  mod->DefineChunkTypeSys("hand_to_home", "motor_command");
  mod->DefineChunkTypeSys("move_cursor", "motor_command", "object", "loc", "device");
  mod->DefineChunkTypeSys("peck", "motor_command", "hand",  "finger", "r", "theta");
  mod->DefineChunkTypeSys("peck_recoil", "motor_command", "hand", "finger", "r",
                       "theta");
  mod->DefineChunkTypeSys("point_hand_at_key", "motor_command", "hand", "to_key");
  mod->DefineChunkTypeSys("press_key", "motor_command", "key");
  mod->DefineChunkTypeSys("punch", "motor_command", "hand", "finger");
  mod->DefineChunkTypeSys("prepare", "motor_command", "style", "hand",  "finger",
                       "r", "theta");
  mod->DefineChunkTypeSys("execute", "motor_command");
  mod->DefineChunkTypeSys("clear", "");
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
    "forward_delete", "end", "pagedn", "",
    "keypad_7", "keypad_8", "keypad_9", "keypad_hyphen", 

    "caps_lock", "A", "S", "D", "F", "G", "H", "J", "K", "L", "semicolon", "quote", "return", "return", "",
    "", "", "", "", 
    "keypad_4", "keypad_5", "keypad_6", "keypad_plus",

    "shift", "Z", "X", "C", "V", "B", "N", "M", "comma", "period", "dot ", "/", "right_shift", "right_shift", "", "",
    "", "up_arrow", "", "",
    "keypad_1", "keypad_2", "keypad_3", "keypad_enter",

    "left_control", "left_option", "left_command", "spc", "spc", "spc", "spc", "space", "spc", "spc", "spc", "spc", 
    "right_command", "right_option", "right_control", "",
    "left_arrow", "down_arrow", "right_arrow", "",
    "keypad_0", "keypad_period", "enter" };

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
  String lkey = key;
  lkey.downcase();
  if(lkey == "mouse") {
    col = 28; row = 2;
    return true;
  }
  int idx = pos_to_key.FindVal_Flat(lkey);
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

void ActrMotorModule::HandToHome(Hand hand) {
  if(hand == RIGHT) {
    SetFingerKey("j", hand, INDEX);
    SetFingerKey("k", hand, MIDDLE);
    SetFingerKey("l", hand, RING);
    SetFingerKey(";", hand, PINKIE);
    SetFingerPos(6, 6, hand, THUMB);
  }
  else {
    SetFingerKey("f", hand, INDEX);
    SetFingerKey("d", hand, MIDDLE);
    SetFingerKey("s", hand, RING);
    SetFingerKey("a", hand, PINKIE);
    SetFingerPos(5, 6, hand, THUMB);
  }
}

void ActrMotorModule::ProcessEvent(ActrEvent& event) {
  if(event.action == "MODULE_REQUEST") {
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
    mod->LogEvent(-1.0f, "motor", "ABORT_MOTOR_REQ", "", "");
    return;
  }

  if(event.action == "PREPARATION_COMPLETE") {
    if(event.params != "prepare") {
      init_act = prep_act;
      ClearModuleFlag(PREP);
      SetModuleFlag(EXEC);
      mod->ScheduleEvent(timing.init, ActrEvent::max_pri, this, this, event.dst_buffer,
                         "INITIATION_COMPLETE", event.params, event.act_arg,
                         ck);
    }
  }
  else if(event.action == "INITIATION_COMPLETE") {
    exec_act = init_act;
    ClearModuleFlag(PROC);
    mod->ScheduleEvent(timing.burst, ActrEvent::max_pri, this, this, event.dst_buffer,
                       "EXECUTE_ACTION", event.params, event.act_arg,
                       ck);
  }
  else if(event.action == "EXECUTE_ACTION") {
    if(event.params == "punch") {
      ExecPunch(event);
    }
    else if(event.params == "click_mouse") {
      ExecClickMouse(event);
    }
    else if(event.params == "peck") {
      ExecPeck(event);
    }
    else if(event.params == "peck_recoil") {
      ExecPeckRecoil(event);
    }
    else if(event.params == "press_key") {
      ExecPressKey(event);
    }
    else if(event.params == "hand_to_mouse") {
      ExecHandToMouse(event);
    }
    else if(event.params == "hand_to_home") {
      ExecHandToHome(event);
    }
    else if(event.params == "move_cursor") {
      ExecMoveCursor(event);
    }
    else if(event.params == "point_hand_at_key") {
      ExecPointHandAtKey(event);
    }
  }
  else if(event.action == "FINISH_MOVEMENT") {
    ClearModuleFlag(EXEC);
  }
  else if(ck->chunk_type->InheritsFromCTName("clear")) {
    ClearRequest(event);
  }
  else if(ck->chunk_type->InheritsFromCTName("prepare")) {
    PrepareRequest(event);
  }
  else if(ck->chunk_type->InheritsFromCTName("execute")) {
    ExecuteRequest(event);
  }
  else if(ck->chunk_type->InheritsFromCTName("punch")) {
    StdMotorRequest(event, "punch");
  }
  else if(ck->chunk_type->InheritsFromCTName("click_mouse")) {
    StdMotorRequest(event, "click_mouse");
  }
  else if(ck->chunk_type->InheritsFromCTName("peck")) {
    StdMotorRequest(event, "peck");
  }
  else if(ck->chunk_type->InheritsFromCTName("peck_recoil")) {
    StdMotorRequest(event, "peck_recoil");
  }
  else if(ck->chunk_type->InheritsFromCTName("press_key")) {
    StdMotorRequest(event, "press_key");
  }
  else if(ck->chunk_type->InheritsFromCTName("hand_to_mouse")) {
    StdMotorRequest(event, "hand_to_mouse");
  }
  else if(ck->chunk_type->InheritsFromCTName("hand_to_home")) {
    StdMotorRequest(event, "hand_to_home");
  }
  else if(ck->chunk_type->InheritsFromCTName("move_cursor")) {
    StdMotorRequest(event, "move_cursor");
  }
  else if(ck->chunk_type->InheritsFromCTName("point_hand_at_key")) {
    StdMotorRequest(event, "point_hand_at_key");
  }
  else {
    TestWarning(true, "MotorRequest", "chunk type not recognized:", ck->chunk_type->name);
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
                     "CLEAR_STATE", "LAST NONE PREP FREE", event.act_arg,
                     ck);

  ClearModuleFlag(BUSY);
  buffer->ClearReq();
}

void ActrMotorModule::PrepareRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  last_cmd = "prepare";

  SetModuleFlag(PREP);
  SetModuleFlag(PROC);

  prep_act = *ck;

  // todo: parse all the preparation stuff an store it somewhere!?

  // todo: figure out feature preparation time??
  mod->ScheduleEvent(timing.feat_prep, ActrEvent::max_pri, this, this,
                     event.dst_buffer,
                     "PREPARATION_COMPLETE", last_cmd, event.act_arg,
                     ck);

  ClearModuleFlag(BUSY);
  buffer->ClearReq();   
}  
    
void ActrMotorModule::ExecuteRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  last_cmd = "execute";

  // todo: get prepared info from prep_act -- should be prepare chunk type
  // has all possible args

  // todo: do init, then exec, then finish, per usual

  SetModuleFlag(PREP);
  SetModuleFlag(PROC);

  prep_act = *ck;

  // todo: figure out feature preparation time??
  mod->ScheduleEvent(timing.feat_prep, ActrEvent::max_pri, this, this,
                     event.dst_buffer,
                     "PREPARATION_COMPLETE", last_cmd, event.act_arg,
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
                     "PREPARATION_COMPLETE", cmd, event.act_arg,
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

  mod->LogEvent(-1.0f, "motor", "OUTPUT_KEY", "", out_str);
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecClickMouse(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  if(TestWarning(!HandOnMouse(), "Motor", "CLICK_MOUSE requested when hand not at mouse!")) {
    return;
  }

  int row, col;
  String key;
  key = CurFingerAll(col, row, RIGHT, INDEX);

  String out_str = key + " #(" + String(row) + " " + String(col) + ")"; 

  mod->LogEvent(-1.0f, "motor", "OUTPUT_KEY", "", out_str);
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecPeck(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  // todo: do this!
  String hstr = ck->GetSlotVal("hand").toString();
  String fstr = ck->GetSlotVal("finger").toString();
 
  int row, col;
  String key;
  key = CurFingerAll(col, row, StringToHand(hstr), StringToFinger(fstr));

  String out_str = key + " #(" + String(row) + " " + String(col) + ")"; 

  mod->LogEvent(-1.0f, "motor", "OUTPUT_KEY", "", out_str);
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecPeckRecoil(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  // todo: do this!
  String hstr = ck->GetSlotVal("hand").toString();
  String fstr = ck->GetSlotVal("finger").toString();
 
  int row, col;
  String key;
  key = CurFingerAll(col, row, StringToHand(hstr), StringToFinger(fstr));

  String out_str = key + " #(" + String(row) + " " + String(col) + ")"; 

  mod->LogEvent(-1.0f, "motor", "OUTPUT_KEY", "", out_str);
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecPressKey(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  String key = ck->GetSlotVal("key").toString();

  int row, col;
  if(TestWarning(!KeyToPos(col, row, key), "Motor", "No press_key mapping available for key:", key)) {
    return;
  }

  // todo: execute press key itself -- move and move back?  maybe not..

  String out_str = key + " #(" + String(row) + " " + String(col) + ")"; 

  mod->LogEvent(-1.0f, "motor", "OUTPUT_KEY", "", out_str);
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecHandToMouse(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  // todo: figure out ply distance
  // todo: if hand already on mouse, don't even start whole event!

  HandToMouse();
  mod->LogEvent(-1.0f, "motor", "MOVE_A_HAND", "", "RIGHT x,y");
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecHandToHome(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  // todo: figure out ply distance
  // todo: if hand already on home, still do everything

  HandToHome();
  mod->LogEvent(-1.0f, "motor", "MOVE_A_HAND", "", "RIGHT x,y");
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecMoveCursor(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  // todo: decode events, locations, etc -- p 307 in ref manual

  if(TestWarning(!HandOnMouse(), "Motor", "MOVE_CURSOR requested when hand not at mouse!")) {
    return;
  }

  // HandToHome();
  mod->LogEvent(-1.0f, "motor", "MOVE_CURSOR_ABSOLUTE", "", "x,y");
 
  // todo: if incremental, do MOVE_CURSOR_POLAR repeated many times..
 
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecPointHandAtKey(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  String key = ck->GetSlotVal("to_key").toString();

  int row, col;
  if(TestWarning(!KeyToPos(col, row, key), "Motor", "No press_key mapping available for key:", key)) {
    return;
  }

  String hstr = ck->GetSlotVal("hand").toString();

  SetHandPos(col, row, StringToHand(hstr));

  String out_str = hstr + " " + key + " #(" + String(row) + " " + String(col) + ")"; 

  mod->LogEvent(-1.0f, "motor", "MOVE_A_HAND", "", out_str);
  
  float finish_time = timing.init + 2.0f * timing.burst;
  mod->ScheduleEvent(finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
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
