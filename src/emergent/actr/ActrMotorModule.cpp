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

#include <taMisc>

// actr source is largely in support/general-pm.lisp
// core-modules/motor.lisp derives much from general-pm.

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
  last_prep.Reset();
  exec_queue.Reset();
  last_cmd = "";
  InitMotorStyles();
  InitHandPos();
  InitPosToKey();
  buffer->UpdateState();
}

void ActrMotorModule::InitMotorStyles() {
  styles.SetSize(N_STYLES);
  ActrMotorStyle* st;
  st = styles[PUNCH];
  st->name = "punch";
  st->style_id = PUNCH;
  st->features.SetSize(2);
  st->features[0].name = "hand";    st->features[0].value.setString("");
  st->features[1].name = "finger";  st->features[1].value.setString("");
  ActrMotorStyle* hfrt = styles[HFRT];
  hfrt->name = "hfrt";
  hfrt->style_id = HFRT;
  hfrt->features.SetSize(4);
  hfrt->features[0].name = "hand";    hfrt->features[0].value.setString("");
  hfrt->features[1].name = "finger";  hfrt->features[1].value.setString("");
  hfrt->features[2].name = "r";       hfrt->features[0].value.setFloat(0.0f);
  hfrt->features[3].name = "theta";   hfrt->features[1].value.setFloat(0.0f);
  st = styles[PECK];
  st->CopyFrom(hfrt);
  st->style_id = PECK;
  st->name = "peck";
  st = styles[PECK_RECOIL];
  st->CopyFrom(hfrt);
  st->style_id = PECK_RECOIL;
  st->name = "peck_recoil";
  st = styles[PLY];
  st->CopyFrom(hfrt);
  st->style_id = PLY;
  st->name = "ply";
  st = styles[HAND_PLY];
  st->CopyFrom(hfrt);
  st->style_id = HAND_PLY;
  st->name = "hand_ply";
  st = styles[CURSOR_PLY];
  st->CopyFrom(hfrt);
  st->style_id = CURSOR_PLY;
  st->name = "cursor_ply";
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
    "escape", "", "f1", "f2", "f3", "f4", "", "f5", "f6", "f7", "f8", "",
    "f9", "f10", "f11", "f12", "", "f13", "f14", "f15", "", "", "",

    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
    "", "", "", "", "", "", "", "",

    "backquote", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "delete", "",
    "help", "home", "pageup", "",
    "clear", "=", "/", "*", 

    "tab", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "backslash ", "",
    "forward_delete", "end", "pagedn", "",
    "keypad_7", "keypad_8", "keypad_9", "keypad_hyphen", 

    "caps_lock", "a", "s", "d", "f", "g", "h", "j", "k", "l", "semicolon", "quote", "return", "return", "",
    "", "", "", "", 
    "keypad_4", "keypad_5", "keypad_6", "keypad_plus",

    "shift", "z", "x", "c", "v", "b", "n", "m", "comma", "period", "dot ", "/", "right_shift", "right_shift", "", "",
    "", "up_arrow", "", "",
    "keypad_1", "keypad_2", "keypad_3", "keypad_enter",

    "left_control", "left_option", "left_command", "spc", "spc", "spc", "spc", "space", "spc", "spc", "spc", "spc", 
    "right_command", "right_option", "right_control", "",
    "left_arrow", "down_arrow", "right_arrow", "",
    "keypad_0", "keypad_period", "enter" };

  pos_to_key.InitFromChars(keys);

  // make press_key lookup table
  key_to_cmd.Reset();
  key_to_cmd.Add(MakeMotorCmd("space", PUNCH, "left", "thumb"));
  key_to_cmd.Add(MakeMotorCmd("backquote", PECK_RECOIL, "left", "pinkie", 2.24, -2.03));
  key_to_cmd.Add(MakeMotorCmd("tab", PECK_RECOIL, "left", "pinkie", 1.41, -2.36));
  key_to_cmd.Add(MakeMotorCmd("1", PECK_RECOIL, "left", "pinkie", 2, -1.57));
  key_to_cmd.Add(MakeMotorCmd("q", PECK_RECOIL, "left", "pinkie", 1, -1.57));
  key_to_cmd.Add(MakeMotorCmd("a", PUNCH, "left", "pinkie"));
  key_to_cmd.Add(MakeMotorCmd("z", PECK_RECOIL, "left", "pinkie", 1, 1.57));
  key_to_cmd.Add(MakeMotorCmd("2", PECK_RECOIL, "left", "ring", 2, -1.57));
  key_to_cmd.Add(MakeMotorCmd("w", PECK_RECOIL, "left", "ring", 1, -1.57));
  key_to_cmd.Add(MakeMotorCmd("s", PUNCH, "left", "ring"));
  key_to_cmd.Add(MakeMotorCmd("x", PECK_RECOIL, "left", "ring", 1, 1.57));
  key_to_cmd.Add(MakeMotorCmd("3", PECK_RECOIL, "left", "middle", 2, -1.57));
  key_to_cmd.Add(MakeMotorCmd("e", PECK_RECOIL, "left", "middle", 1, -1.57));
  key_to_cmd.Add(MakeMotorCmd("d", PUNCH, "left", "middle"));
  key_to_cmd.Add(MakeMotorCmd("c", PECK_RECOIL, "left", "middle", 1, 1.57));
  key_to_cmd.Add(MakeMotorCmd("4", PECK_RECOIL, "left", "index", 2, -1.57));
  key_to_cmd.Add(MakeMotorCmd("r", PECK_RECOIL, "left", "index", 1, -1.57));
  key_to_cmd.Add(MakeMotorCmd("f", PUNCH, "left", "index"));
  key_to_cmd.Add(MakeMotorCmd("v", PECK_RECOIL, "left", "index", 1, 1.57));
  key_to_cmd.Add(MakeMotorCmd("s", PECK_RECOIL, "left", "index", 2.24, -1.11));
  key_to_cmd.Add(MakeMotorCmd("t", PECK_RECOIL, "left", "index", 1.41, -0.79));
  key_to_cmd.Add(MakeMotorCmd("g", PECK_RECOIL, "left", "index", 1, 0));
  key_to_cmd.Add(MakeMotorCmd("b", PECK_RECOIL, "left", "index", 1.41, 0.79));
  key_to_cmd.Add(MakeMotorCmd("6", PECK_RECOIL, "right", "index", 2.24, -2.03));
  key_to_cmd.Add(MakeMotorCmd("y", PECK_RECOIL, "right", "index", 1.41, -2.36));
  key_to_cmd.Add(MakeMotorCmd("h", PECK_RECOIL, "right", "index", 1, 3.14));
  key_to_cmd.Add(MakeMotorCmd("n", PECK_RECOIL, "right", "index", 1.41, 2.36));
  key_to_cmd.Add(MakeMotorCmd("7", PECK_RECOIL, "right", "index", 2, -1.57));
  key_to_cmd.Add(MakeMotorCmd("u", PECK_RECOIL, "right", "index", 1, -1.57));
  key_to_cmd.Add(MakeMotorCmd("j", PUNCH, "right", "index"));
  key_to_cmd.Add(MakeMotorCmd("m", PECK_RECOIL, "right", "index", 1, 1.57));
  key_to_cmd.Add(MakeMotorCmd("8", PECK_RECOIL, "right", "middle", 2, -1.57));
  key_to_cmd.Add(MakeMotorCmd("i", PECK_RECOIL, "right", "middle", 1, -1.57));
  key_to_cmd.Add(MakeMotorCmd("k", PUNCH, "right", "middle"));
  key_to_cmd.Add(MakeMotorCmd("comma", PECK_RECOIL, "right", "middle", 1, 1.57));
  key_to_cmd.Add(MakeMotorCmd("9", PECK_RECOIL, "right", "ring", 2, -1.57));
  key_to_cmd.Add(MakeMotorCmd("o", PECK_RECOIL, "right", "ring", 1, -1.57));
  key_to_cmd.Add(MakeMotorCmd("l", PUNCH, "right", "ring"));
  key_to_cmd.Add(MakeMotorCmd("period", PECK_RECOIL, "right", "ring", 1, 1.57));
  key_to_cmd.Add(MakeMotorCmd("0", PECK_RECOIL, "right", "pinkie", 2, -1.57));
  key_to_cmd.Add(MakeMotorCmd("p", PECK_RECOIL, "right", "pinkie", 1, -1.57));
  key_to_cmd.Add(MakeMotorCmd("semicolon", PUNCH, "right", "pinkie"));
  key_to_cmd.Add(MakeMotorCmd("slash", PECK_RECOIL, "right", "pinkie", 1, 1.57));
  key_to_cmd.Add(MakeMotorCmd("hyphen", PECK_RECOIL, "right", "pinkie", 2.24, -1.11));
  key_to_cmd.Add(MakeMotorCmd("-", PECK_RECOIL, "right", "pinkie", 2.24, -1.11));
  key_to_cmd.Add(MakeMotorCmd("[", PECK_RECOIL, "right", "pinkie", 1.41, -0.78));
  key_to_cmd.Add(MakeMotorCmd("quote", PECK_RECOIL, "right", "pinkie", 1, 0));
  key_to_cmd.Add(MakeMotorCmd("return", PECK_RECOIL, "right", "pinkie", 2, 0));
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

bool ActrMotorModule::MoveFinger(int& col, int& row, Hand hand, Finger finger,
                                 float r, float theta) {
  CurFingerPos(col, row, hand, finger);
  if(r > 0.0f) {
    col += (int) taMath_float::round(r * cosf(theta));
    row += (int) taMath_float::round(r * sinf(theta));
    SetFingerPos(col, row, hand, finger);
  }
  return true;
}

void ActrMotorModule::PressKey(const String& key, float time) {
  last_key = key;
  last_key_time = time;
  if(act_prog) {
    Program* prg = act_prog.ptr();
    if(prg->run_state == Program::NOT_INIT) {
      prg->CallInit(NULL);
    }
    if(TestError(prg->run_state == Program::NOT_INIT,
                 "cannot run program:", prg->name,
                 "because it cannot be initialized -- see console for errors")) {
    }
    else {
      prg->Call(NULL);
    }
  }
  else {
    taMisc::Info("key pressed:", key, "at time:", String(time));
  }
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
  ActrModel* mod = Model();
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck || !ck->chunk_type, "MotorRequest", "chunk is NULL")) {
    return;
  }

  if(event.action == "MODULE_REQUEST") {
    MotorRequest(event);
  }
  else if(event.action == "PREPARATION_COMPLETE") {
    ActrMotorStyle* st = LastPrep();
    if(st && st->exec_immediate) {
      ClearModuleFlag(PREP);
      SetModuleFlag(EXEC);
      mod->ScheduleEvent(timing.init, ActrEvent::max_pri, this, this, event.dst_buffer,
                         "INITIATION_COMPLETE", event.params, event.act_arg,
                         ck);
    }
  }
  else if(event.action == "INITIATION_COMPLETE") {
    ClearModuleFlag(PROC);
    ActrMotorStyle* st = LastPrep();
    if(st) {
      mod->ScheduleEvent(st->exec_time, ActrEvent::max_pri, this, this, event.dst_buffer,
                         "EXECUTE_ACTION", event.params, event.act_arg,
                         ck);
      exec_queue.Add(st);       // now we add to the exec queue
    }
  }
  else if(event.action == "EXECUTE_ACTION") {
    ActrMotorStyle* st = exec_queue.Pop();
    if(st) {
      ExecAction(event, st);
    }
  }
  else if(event.action == "FINISH_MOVEMENT") {
    ClearModuleFlag(EXEC);
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

  RequestBufferClear(event.dst_buffer); // always clear for any request

  mod->LogEvent(-1.0f, "motor", ck->chunk_type->name, "", ck->PrintStr());

  if(ck->chunk_type->InheritsFromCTName("clear")) {
    ClearRequest(event);
  }
  else if(ck->chunk_type->InheritsFromCTName("prepare")) {
    PrepareRequest(event);
  }
  else if(ck->chunk_type->InheritsFromCTName("execute")) {
    ExecuteRequest(event);
  }
  else {
    StdMotorRequest(event);
  }
}

void ActrMotorModule::ClearRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();
  last_cmd = "clear";

  last_prep.Reset();
  exec_queue.Reset();
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

  // todo: figure out feature preparation time??
  mod->ScheduleEvent(timing.feat_prep, ActrEvent::max_pri, this, this,
                     event.dst_buffer,
                     "PREPARATION_COMPLETE", last_cmd, event.act_arg,
                     ck);

  ClearModuleFlag(BUSY);
  buffer->ClearReq();   
}  
    
void ActrMotorModule::StdMotorRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  last_cmd = ck->chunk_type->name;

  ActrMotorStyle* st = NULL;
  if(ck->chunk_type->InheritsFromCTName("punch")) {
    st = PrepPunch(ck->GetSlotVal("hand").toString(),
                                   ck->GetSlotVal("finger").toString());
  }
  else if(ck->chunk_type->InheritsFromCTName("click_mouse")) {
    st = PrepClickMouse();
  }
  else if(ck->chunk_type->InheritsFromCTName("peck")) {
    st = PrepPeck(ck->GetSlotVal("hand").toString(),
                                  ck->GetSlotVal("finger").toString(),
                                  ck->GetSlotVal("r").toFloat(),
                                  ck->GetSlotVal("theta").toFloat());
  }
  else if(ck->chunk_type->InheritsFromCTName("peck_recoil")) {
    st = PrepPeckRecoil(ck->GetSlotVal("hand").toString(),
                                        ck->GetSlotVal("finger").toString(),
                                        ck->GetSlotVal("r").toFloat(),
                                        ck->GetSlotVal("theta").toFloat());
  }
  else if(ck->chunk_type->InheritsFromCTName("press_key")) {
    st = PrepPressKey(ck->GetSlotVal("key").toString());
  }
  else if(ck->chunk_type->InheritsFromCTName("hand_to_mouse")) {
    st = PrepHandToMouse();
  }
  else if(ck->chunk_type->InheritsFromCTName("hand_to_home")) {
    st = PrepHandToHome();
  }
  else if(ck->chunk_type->InheritsFromCTName("move_cursor")) {
    st = PrepMoveCursor();
  }
  else if(ck->chunk_type->InheritsFromCTName("point_hand_at_key")) {
    st = PrepPointHandAtKey(ck->GetSlotVal("hand").toString(),
                                            ck->GetSlotVal("to_key").toString());
  }
  else {
    TestWarning(true, "MotorRequest", "chunk type not recognized:", ck->chunk_type->name);
  }

  if(st) {
    SetNewLastPrep(st);
    SetModuleFlag(PREP);
    SetModuleFlag(PROC);

    mod->ScheduleEvent(st->fprep_time, ActrEvent::max_pri, this, this,
                       event.dst_buffer,
                       "PREPARATION_COMPLETE", last_cmd, event.act_arg,
                       ck);
  }
  else {
    // todo: clear everything -- prep failed
  }

  ClearModuleFlag(BUSY);
  buffer->ClearReq();   
}  

ActrMotorStyle* ActrMotorModule::NewPrep(MotorStyles style) {
  ActrMotorStyle* st = new ActrMotorStyle;
  st->CopyFrom(styles[style]);
  st->name = styles[style]->name;
  return st;
}

ActrMotorStyle* ActrMotorModule::MakeMotorCmd(const String& nm, MotorStyles style,
                                              const Variant& par1,
                                              const Variant& par2,
                                              const Variant& par3,
                                              const Variant& par4,
                                              const Variant& par5) {
  ActrMotorStyle* st = NULL;
  switch(style) {
  case PUNCH:
    st = PrepPunch(par1.toString(), par2.toString());
    break;
  case PECK:
    st = PrepPeck(par1.toString(), par2.toString(), par3.toFloat(), par4.toFloat());
    break;
  case PECK_RECOIL:
    st = PrepPeckRecoil(par1.toString(), par2.toString(), par3.toFloat(), par4.toFloat());
    break;
  case HAND_PLY:
    st = PrepHandPly(par1.toString(), par2.toString(), par3.toFloat(), par4.toFloat());
    break;
  case CURSOR_PLY:
    st = PrepCursorPly(par1.toString(), par2.toString(), par3.toFloat(), par4.toFloat());
    break;
  default:
    st = NULL;
    break;
  }
  if(st)
    st->name = nm;
  return st;
}

ActrMotorStyle* ActrMotorModule::LastPrep() {
  if(last_prep.size == 0) return NULL;
  return last_prep[0];
}

void ActrMotorModule::SetNewLastPrep(ActrMotorStyle* st) {
  last_prep.Reset();            // ensure empty
  last_prep.Add(st);
}

ActrMotorStyle* ActrMotorModule::PrepPunch(const String& hand, const String& finger) {
  ActrMotorStyle* st = NewPrep(PUNCH);
  st->features.SetVal("hand", hand);
  st->features.SetVal("finger", finger);

  int n_feats = st->features.size;
  ActrMotorStyle* lp = LastPrep();
  if(lp && lp->name == st->name) {
    if(lp->features[0].value == hand) {
      if(lp->features[1].value == finger)
        n_feats = 0;
      else
        n_feats = 1;
    }
  }

  st->fprep_time = timing.feat_prep * (1.0f + (float)n_feats);
  st->exec_time = 0.01f;         // not doc'd but seems to be the case
  st->finish_time = 2.0f * timing.burst - st->exec_time;
  st->exec_immediate = true;
  return st;
}  
    
ActrMotorStyle* ActrMotorModule::PrepClickMouse() {
  if(TestWarning(!HandOnMouse(), "Motor", "CLICK_MOUSE requested when hand not at mouse!")) {
    return NULL;
  }

  return PrepPunch("right", "index");
}  
    
ActrMotorStyle* ActrMotorModule::PrepPeck(const String& hand, const String& finger, float r, float theta) {
  ActrMotorStyle* st = NewPrep(PECK);
  st->features.SetVal("hand", hand);
  st->features.SetVal("finger", finger);
  st->features.SetVal("r", r);
  st->features.SetVal("theta", theta);

  int n_feats = st->features.size;
  ActrMotorStyle* lp = LastPrep();
  if(lp && lp->name == st->name) {
    if(lp->features[0].value == hand) {
      if(lp->features[1].value == finger) {
        int ns = 2;
        if(lp->features[2].value == r) ns--;
        if(lp->features[3].value == theta) ns--;
        n_feats = ns;
      }
      else {
        n_feats = 3;
      }
    }
  }

  st->fprep_time = timing.feat_prep * (1.0f + (float)n_feats);
  st->exec_time = MAX(timing.Fitts(timing.peck_fitts_coeff, r), timing.burst); 
  st->finish_time = timing.burst; // not clear but this seems to be the case
  st->exec_immediate = true;
  return st;
}  
    
ActrMotorStyle* ActrMotorModule::PrepPeckRecoil(const String& hand, const String& finger,
                                                float r, float theta) {
  ActrMotorStyle* st = PrepPeck(hand, finger, r, theta);
  st->style_id = PECK_RECOIL;
  st->name = "peck_recoil";
  if(st) {
    st->finish_time = st->exec_time + timing.burst;  // finish is usual burst + full exec time to get back
  }
  return st;
}  
    
ActrMotorStyle* ActrMotorModule::PrepPressKey(const String& key) {
  String tkey = key;
  tkey.downcase();
  
  ActrMotorStyle* st = key_to_cmd.FindName(tkey);
  if(st) {
    return st;
  }

  // fall-through
  int row, col;
  if(TestWarning(!KeyToPos(col, row, key), "Motor", "No press_key mapping available for key:", key)) {
    // todo: log a BAD_KEY message
    return NULL;
  }

  return PrepPeckRecoil("right", "index", 1, 0); // dummy values -- just do something
}  
    
ActrMotorStyle* ActrMotorModule::PrepHandPly(const String& hand, const String& finger, float r, float theta) {
  ActrMotorStyle* st = NewPrep(HAND_PLY);
  st->features.SetVal("hand", hand);
  st->features.SetVal("finger", finger);
  st->features.SetVal("r", r);
  st->features.SetVal("theta", theta);

  int n_feats = st->features.size;
  ActrMotorStyle* lp = LastPrep();
  if(lp && lp->name == st->name) {
    if(lp->features[0].value == hand) {
      if(lp->features[1].value == finger) {
        int ns = 2;
        if(lp->features[2].value == r) ns--;
        if(lp->features[3].value == theta) ns--;
        n_feats = ns;
      }
      else {
        n_feats = 3;
      }
    }
  }

  st->fprep_time = timing.feat_prep * (1.0f + (float)n_feats);
  st->exec_time = MAX(timing.Fitts(timing.peck_fitts_coeff, r), timing.burst); 
  st->finish_time = timing.burst; // not clear but this seems to be the case
  st->exec_immediate = true;
  return st;
}  

ActrMotorStyle* ActrMotorModule::PrepHandPlyToCoord(const String& hand, const String& finger, float x, float y) {
  // todo: convert x,y to r, theta, then call PrepPly
  return PrepHandPly(hand, finger, 1.0f, 0.0f);
}  

ActrMotorStyle* ActrMotorModule::PrepHandToMouse() {
  if(HandOnMouse()) {           // already there!
    return NULL;
  }
  return PrepHandPlyToCoord("right", "index", 28.0, 2.0);
}
    
ActrMotorStyle* ActrMotorModule::PrepHandToHome() {
  // if hand already on home, still do everything
  return PrepHandPlyToCoord("right", "index", 7.0, 4.0);
}
    
ActrMotorStyle* ActrMotorModule::PrepCursorPly(const String& hand, const String& finger, float r, float theta) {
  ActrMotorStyle* st = NewPrep(CURSOR_PLY);
  st->features.SetVal("hand", hand);
  st->features.SetVal("finger", finger);
  st->features.SetVal("r", r);
  st->features.SetVal("theta", theta);

  int n_feats = st->features.size;
  ActrMotorStyle* lp = LastPrep();
  if(lp && lp->name == st->name) {
    if(lp->features[0].value == hand) {
      if(lp->features[1].value == finger) {
        int ns = 2;
        if(lp->features[2].value == r) ns--;
        if(lp->features[3].value == theta) ns--;
        n_feats = ns;
      }
      else {
        n_feats = 3;
      }
    }
  }

  st->fprep_time = timing.feat_prep * (1.0f + (float)n_feats);
  st->exec_time = MAX(timing.Fitts(timing.peck_fitts_coeff, r), timing.burst); 
  st->finish_time = timing.burst; // not clear but this seems to be the case
  st->exec_immediate = true;
  return st;
}  

ActrMotorStyle* ActrMotorModule::PrepMoveCursor() {
  if(TestWarning(!HandOnMouse(), "Motor", "MOVE_CURSOR requested when hand not at mouse!")) {
    return NULL;
  }

  // todo: decode object, location, etc

  return PrepCursorPly("right", "index", 1.0f, 0.0f); // temp dummy
}

ActrMotorStyle* ActrMotorModule::PrepPointHandAtKey(const String& hand,
                                                    const String& to_key) {
  int row, col;
  if(TestWarning(!KeyToPos(col, row, to_key), "Motor", "No press_key mapping available for key:", to_key)) {
    // todo: log a BAD_KEY message
    return NULL;
  }

  return PrepPeckRecoil(hand, "index", 1, 0); // dummy values -- just do something
}  

void ActrMotorModule::ExecAction(ActrEvent& event, ActrMotorStyle* st) {
  switch(st->style_id) {
  case PUNCH:
    ExecPunch(event, st);
    break;
  case PECK:
    ExecPeck(event, st);
    break;
  case PECK_RECOIL:
    ExecPeckRecoil(event, st);
    break;
  case HAND_PLY:
    ExecHandPly(event, st);
    break;
  case CURSOR_PLY:
    ExecCursorPly(event, st);
    break;
  }
}
    
void ActrMotorModule::ExecPunch(ActrEvent& event, ActrMotorStyle* st) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  String hstr = st->features.GetVal("hand").toString();
  String fstr = st->features.GetVal("finger").toString();
 
  int row, col;
  String key;
  key = CurFingerAll(col, row, StringToHand(hstr), StringToFinger(fstr));

  String out_str = key + " #(" + String(col) + " " + String(row) + ")"; 

  if(col == 28 && row == 2) {
    mod->LogEvent(-1.0f, "motor", "OUTPUT_KEY", "", out_str);
  }
  else {
    mod->LogEvent(-1.0f, "motor", "CLICK_MOUSE", "", out_str);
  }

  PressKey(key, mod->cur_time);
  
  mod->ScheduleEvent(st->finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecPeck(ActrEvent& event, ActrMotorStyle* st) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  String hstr = st->features.GetVal("hand").toString();
  String fstr = st->features.GetVal("finger").toString();
  float r = st->features.GetVal("r").toFloat();
  float theta = st->features.GetVal("theta").toFloat();

  Hand hand = StringToHand(hstr);
  Finger finger = StringToFinger(fstr);

  int row, col;
  MoveFinger(col, row, hand, finger, r, theta);

  String key = CurFingerKey(hand, finger);
  String out_str = key + " #(" + String(col) + " " + String(row) + ")"; 

  mod->LogEvent(-1.0f, "motor", "OUTPUT_KEY", "", out_str);
  
  PressKey(key, mod->cur_time);

  mod->ScheduleEvent(st->finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecPeckRecoil(ActrEvent& event, ActrMotorStyle* st) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  String hstr = st->features.GetVal("hand").toString();
  String fstr = st->features.GetVal("finger").toString();
  float r = st->features.GetVal("r").toFloat();
  float theta = st->features.GetVal("theta").toFloat();

  Hand hand = StringToHand(hstr);
  Finger finger = StringToFinger(fstr);

  int orig_row, orig_col;
  CurFingerPos(orig_col, orig_row, hand, finger);;

  int row, col;
  MoveFinger(col, row, hand, finger, r, theta);

  String key = CurFingerKey(hand, finger);
  String out_str = key + " #(" + String(col) + " " + String(row) + ")"; 

  SetFingerPos(orig_col, orig_row, hand, finger); // restore original positions

  mod->LogEvent(-1.0f, "motor", "OUTPUT_KEY", "", out_str);
  
  PressKey(key, mod->cur_time);

  mod->ScheduleEvent(st->finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecHandPly(ActrEvent& event, ActrMotorStyle* st) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  // todo: decode events, locations, etc -- p 307 in ref manual

  // HandToHome();
  mod->LogEvent(-1.0f, "motor", "MOVE_CURSOR_ABSOLUTE", "", "x,y");
 
  // todo: if incremental, do MOVE_CURSOR_POLAR repeated many times..
 
  mod->ScheduleEvent(st->finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
      "FINISH_MOVEMENT", event.params, event.act_arg,
      ck);
}

void ActrMotorModule::ExecCursorPly(ActrEvent& event, ActrMotorStyle* st) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  // todo: decode events, locations, etc -- p 307 in ref manual

  // HandToHome();
  mod->LogEvent(-1.0f, "motor", "MOVE_CURSOR_ABSOLUTE", "", "x,y");
 
  // todo: if incremental, do MOVE_CURSOR_POLAR repeated many times..
 
  mod->ScheduleEvent(st->finish_time, ActrEvent::max_pri, this, this, event.dst_buffer,
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
