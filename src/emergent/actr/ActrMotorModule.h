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

#ifndef ActrMotorModule_h
#define ActrMotorModule_h 1

// parent includes:
#include <ActrModule>

// member includes:
#include <int_Matrix>
#include <String_Matrix>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrMotorParams);

class E_API ActrMotorParams : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR motor parameters
INHERITED(taOBase)
public:
  bool          cursor_noise;  // #DEF_false :cursor-noise in ACT-R -- add noise to the final cursor position relative to the target position
  float         def_targ_width; // #DEF_1 :default-target-width in ACT-R -- The effective width, in degrees of visual angle, of targets with undefined widths when computing the Fitt's law computation
  float         inc_mouse_move;  // #DEF_0:0.05 :incremental-mouse-moves in ACT-R -- if non-zero, updates mouse location every inc_mouse_move seconds when it is in motion (typically 50 ms) -- if 0, only updates location at end of move

  override String       GetTypeDecoKey() const { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrMotorParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActrMotorTimeParams);

class E_API ActrMotorTimeParams : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR motor timing parameters
INHERITED(taOBase)
public:
  float         feat_prep;  // #DEF_0.05 :motor-feature-prep-time in ACT-R -- Time in seconds required to prepare each movement feature
  float         init;       // #DEF_0.05 :motor-initiation-time in ACT-R -- length of time to initiate a motor action in seconds
  float         burst;      // #DEF_0.05 :motor-burst-time in ACT-R -- minimum time required for any motor movement in seconds
  float         min_fitts;  // #DEF_0.1 :min-fitts-time in ACT-R -- minimum movement time in seconds to perform an aimed movement (for which Fitt's timing law is applied)
  float         peck_fitts_coeff; // #DEF_0.075 :peck-fitts-coeff in ACT-R -- the b coefficient in Fitt's equation for the timing of peck style movements

  override String       GetTypeDecoKey() const { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrMotorTimeParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActrMotorModule);

class E_API ActrMotorModule : public ActrModule {
  // The ACT-R motor module -- for keyboard and mouse primarily (todo: extend to generic actions like reaching, grasping, ungrasping, etc)
INHERITED(ActrModule)
public:
  enum Hand {                   // used for accessing hand_pos info
    LEFT,
    RIGHT,
  };
  enum Finger {                 // used for accessing hand_pos info
    INDEX,
    MIDDLE,
    RING,
    PINKIE,
    THUMB,
  };
  enum ColRow {                 // used for accessing hand_pos info
    COL,
    ROW,
  };

  ActrMotorParams       params; // misc motor parameters
  ActrMotorTimeParams   timing; // timing motor parameters
  ActrChunk             prep_act; // #HIDDEN #NO_SAVE action being prepared
  ActrChunk             init_act; // #HIDDEN #NO_SAVE action being initiated
  ActrChunk             exec_act; // #HIDDEN #NO_SAVE action being executed
  int_Matrix            hand_pos; // #HIDDEN #NO_SAVE [x y, finger, hand] matrix for hand positions
  String_Matrix         pos_to_key; // #HIDDEN #NO_SAVE translate position to key name (only for 0..22, 0..6 keyboard -- mouse at 28,2 handled separately in function)

  virtual String PosToKey(int col, int row);
  // #CAT_ActR translate col, row position to corresponding key on keyboard, or if 28,2, the mouse
  virtual bool   KeyToPos(int& col, int& row, const String& key);
  // #CAT_ActR key name to col, row position
  virtual Hand   StringToHand(const String& str);
  // #CAT_ActR translate string to hand enum
  virtual Finger StringToFinger(const String& str);
  // #CAT_ActR translate string to finger enum

  virtual bool   CurFingerPos(int& col, int& row, Hand hand, Finger finger);
  // #CAT_ActR return current col, row position of given finger on given hand
  inline bool    CurHandPos(int& col, int& row, Hand hand)
  { return CurFingerPos(col, row, hand, INDEX); }
  // #CAT_ActR return current col, row position of given hand (as determined by INDEX finger)
  virtual bool   SetFingerPos(int col, int row, Hand hand, Finger finger);
  // #CAT_ActR set current col, row position of given finger on given hand
  inline bool    SetHandPos(int& col, int& row, Hand hand)
  { return SetFingerPos(col, row, hand, INDEX); }
  // #CAT_ActR set current col, row position of given hand (as determined by INDEX finger)

  virtual String  CurFingerKey(Hand hand, Finger finger);
  // #CAT_ActR return key name for current position of given finger on given hand
  virtual bool    SetFingerKey(const String& key, Hand hand, Finger finger);
  // #CAT_ActR set current finger position to given key name

  virtual String  CurFingerAll(int& col, int& row, Hand hand, Finger finger);
  // #CAT_ActR return key name, col, and row for current position of given finger on given hand

  virtual bool    HandOnMouse(Hand hand = RIGHT);
  // #CAT_ActR is the hand on the mouse?
  virtual void    HandToMouse(Hand hand = RIGHT);
  // #CAT_ActR move the hand to the mouse

  virtual void  MotorRequest(ActrEvent& event);
  // #CAT_ActR process a motor request
  virtual void  ClearRequest(ActrEvent& event);
  // #CAT_ActR process a clear request
  virtual void  StdMotorRequest(ActrEvent& event, const String& cmd);
  // #CAT_ActR process a standard motor request -- triggers prep, init, exec sequence

  virtual void  ExecPunch(ActrEvent& event);
  // #CAT_ActR execute a punch action
  virtual void  ExecClickMouse(ActrEvent& event);
  // #CAT_ActR execute a click-mouse action

  virtual void  InitHandPos();
  // #CAT_ActR initialize hand position to home keys
  virtual void  InitPosToKey();
  // #IGNORE initialize the pos_to_key mapping matrix

  override void  InitModule();
  override void  ProcessEvent(ActrEvent& event);
  override bool  ProcessQuery(ActrBuffer* buf, const String& query,
                             bool why_not = false);
  override bool  SetParam(const String& param_nm, Variant par1, Variant par2);
  override void  Init();

  TA_SIMPLE_BASEFUNS(ActrMotorModule);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrMotorModule_h
