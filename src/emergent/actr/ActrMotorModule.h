// Co2018ght 2013-2017, Regents of the University of Colorado,
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
#include <ActrMotorStyle_List>
#include <taMath_float>
#include <taVector2i>
#include <ProgramRef>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrMotorParams);

class E_API ActrMotorParams : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_ActR motor parameters
INHERITED(taOBase)
public:
  bool          cursor_noise;  // #DEF_false :cursor-noise in ACT-R -- add noise to the final cursor position relative to the target position
  float         inc_mouse_move;  // #DEF_0:0.05 :incremental-mouse-moves in ACT-R -- if non-zero, updates mouse location every inc_mouse_move seconds when it is in motion (typically 50 ms) -- if 0, only updates location at end of move
  float         def_targ_width; // #DEF_1 :default-target-width in ACT-R -- The effective width, in degrees of visual angle, of targets with undefined widths when computing the Fitt's law computation

  String       GetTypeDecoKey() const override { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrMotorParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActrMotorTimeParams);

class E_API ActrMotorTimeParams : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_ActR motor timing parameters
INHERITED(taOBase)
public:
  float         feat_prep;  // #DEF_0.05 :motor-feature-prep-time in ACT-R -- Time in seconds required to prepare each movement feature
  float         init;       // #DEF_0.05 :motor-initiation-time in ACT-R -- length of time to initiate a motor action in seconds
  float         burst;      // #DEF_0.05 :motor-burst-time in ACT-R -- minimum time required for any motor movement in seconds
  float         min_fitts;  // #DEF_0.1 :min-fitts-time in ACT-R -- minimum movement time in seconds to perform an aimed movement (for which Fitt's timing law is applied)
  float         peck_fitts_coeff; // #DEF_0.075 :peck-fitts-coeff in ACT-R -- the b coefficient in Fitt's equation for the timing of peck style movements
  float         mouse_fitts_coeff; // #DEF_0.1 :mouse-fitts-coeff in ACT-R -- the b coefficient in Fitt's equation for the timing of mouse cursor movements

  float         Fitts(float b, float dist, float width)
  { float rv = b * taMath_float::log2(dist / width + 0.5f); if(rv < min_fitts) rv = min_fitts;
    return rv;}
  // fitts law equation

  String       GetTypeDecoKey() const override { return "Program"; }

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
  enum MotorStyles {       // these correspond to entries in the styles list of motor styles
    PUNCH,                      // punch a key -- hand, finger features
    HFRT,                       // hand-finger r-theta movement -- those 4 features
    PECK,                       // peck a key -- basically hfrt
    PECK_RECOIL,                // peck and recoil -- same as PECK feature-wise
    PLY,                        // generic ply
    HAND_PLY,                   // hand ply -- to keyboard-centric coordinates
    CURSOR_PLY,                 // hand ply -- to screen coordinates
    N_STYLES,                   // number of styles
  };

  ActrMotorParams       params; // misc motor parameters
  ActrMotorTimeParams   timing; // timing motor parameters
  String                last_key; // #NO_SAVE last key pressed -- will be the string mouse for a mouse click
  float                 last_key_time; // #NO_SAVE time that last key was pressed
  ProgramRef            act_prog;      // program to call when an action is executed -- program can check the info on this module for what happened
  ActrMotorStyle_List   styles; // #HIDDEN #NO_SAVE available motor styles defined
  ActrMotorStyle_List   last_prep; // #HIDDEN #NO_SAVE last prepared motor action
  ActrMotorStyle_List   exec_queue; // #HIDDEN #NO_SAVE list of queued motor actions that have been prepared and are ready to execute
  int_Matrix            hand_pos; // #HIDDEN #NO_SAVE [x y, hand] matrix for hand positions
  int_Matrix            finger_pos; // #HIDDEN #NO_SAVE [x y, finger, hand] matrix for finger offset positions relative to hand positions
  taVector2i            cursor_pos; // #HIDDEN #NO_SAVE current cursor position
  String_Matrix         pos_to_key; // #HIDDEN #NO_SAVE translate position to key name (only for 0..22, 0..6 keyboard -- mouse at 28,2 handled separately in function)
  ActrMotorStyle_List   key_to_cmd; // #HIDDEN #NO_SAVE mapping between keys and motor commands

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
  virtual bool   CurHandPos(int& col, int& row, Hand hand);
  // #CAT_ActR return current col, row position of given hand (as determined by INDEX finger)
  virtual bool   SetFingerPos(int col, int row, Hand hand, Finger finger);
  // #CAT_ActR set current col, row position of given finger on given hand
  virtual bool   SetHandPos(int col, int row, Hand hand);
  // #CAT_ActR set current col, row position of given hand (as determined by INDEX finger)

  virtual String  CurFingerKey(Hand hand, Finger finger);
  // #CAT_ActR return key name for current position of given finger on given hand
  virtual bool    SetFingerKey(const String& key, Hand hand, Finger finger);
  // #CAT_ActR set current finger position to given key name

  virtual String  CurFingerAll(int& col, int& row, Hand hand, Finger finger);
  // #CAT_ActR return key name, col, and row for current position of given finger on given hand

  virtual void  PressKey(const String& key, float time);
  // #CAT_ActR called when a key is pressed -- sets last_key and last_key_time, calls act_prog or if that is NULL just prints out the key output

  virtual bool  MoveFinger(int& col, int& row, Hand hand, Finger finger,
                           float r, float theta);
  // #CAT_ActR move given finger using r, theta parameters
  virtual bool  MoveHand(int& col, int& row, Hand hand, float r, float theta);
  // #CAT_ActR move given hand using r, theta parameters -- also resets associated finger offsets to std form
  virtual bool  MoveCursor(int& col, int& row, float r, float theta);
  // #CAT_ActR move cursor using given r, theta parameters
  virtual bool  HandOnMouse(Hand hand = RIGHT);
  // #CAT_ActR is the hand on the mouse?

  virtual void  MotorRequest(ActrEvent& event);
  // #CAT_ActR process a motor request
  virtual void  ClearRequest(ActrEvent& event);
  // #CAT_ActR process a clear request
  virtual void  PrepareRequest(ActrEvent& event);
  // #CAT_ActR process a prepare request
  virtual void  ExecuteRequest(ActrEvent& event);
  // #CAT_ActR process an execute request
  virtual void  StdMotorRequest(ActrEvent& event);
  // #CAT_ActR process a standard motor request -- triggers prep, init, exec sequence

  virtual ActrMotorStyle* NewPrep(MotorStyles style);
  // #IGNORE get a new motor prep rep for given style
  virtual ActrMotorStyle* MakeMotorCmd(const String& nm, MotorStyles style,
                                       const Variant& par1=_nilVariant,
                                       const Variant& par2=_nilVariant,
                                       const Variant& par3=_nilVariant,
                                       const Variant& par4=_nilVariant,
                                       const Variant& par5=_nilVariant);
  // #IGNORE make a new motor command with given parameters
    
  virtual ActrMotorStyle* LastPrep();
  // #IGNORE return last prep or NULL if none
  virtual void  SetNewLastPrep(ActrMotorStyle* st);
  // #IGNORE set last_prep to this new prep

  virtual ActrMotorStyle* PrepPunch(const String& hand, const String& finger);
  // #CAT_ActR prepare a punch action
  virtual ActrMotorStyle* PrepClickMouse();
  // #CAT_ActR prepare a click_mouse action
  virtual ActrMotorStyle* PrepPeck(const String& hand, const String& finger, float r, float theta);
  // #CAT_ActR prepare a peck action
  virtual ActrMotorStyle* PrepPeckRecoil(const String& hand, const String& finger, float r, float theta);
  // #CAT_ActR prepare a peck_recoil action
  virtual ActrMotorStyle* PrepPressKey(const String& key);
  // #CAT_ActR prepare a press_key action
  virtual ActrMotorStyle* PrepHandPly(const String& hand, float r, float theta);
  // #CAT_ActR prepare a ply (hand movement)
  virtual ActrMotorStyle* PrepHandPlyToCoord(const String& hand, int to_col, int to_row);
  // #CAT_ActR prepare a ply (hand movement)
  virtual ActrMotorStyle* PrepHandToMouse();
  // #CAT_ActR prepare a hand_to_mouse action
  virtual ActrMotorStyle* PrepHandToHome();
  // #CAT_ActR prepare a hand_to_home action
  virtual ActrMotorStyle* PrepCursorPly(float r, float theta, float coeff_mult = 1.0f,
                                        float targ_width = 1.0f);
  // #CAT_ActR prepare a cursor_ply action
  virtual ActrMotorStyle* PrepCursorPlyToCoord(int to_col, int to_row,
                                               float coeff_mult = 1.0f,
                                               float targ_width = 1.0f);
  // #CAT_ActR prepare a cursor_ply action to coords
  virtual ActrMotorStyle* PrepMoveCursor(ActrChunk* obj, ActrChunk* loc,
                                         const String& device);
  // #CAT_ActR prepare a move_cursor action
  virtual ActrMotorStyle* PrepPointHandAtKey(const String& hand, const String& to_key);
  // #CAT_ActR prepare a point_hand_at_key action

  virtual void  ExecAction(ActrEvent& event, ActrMotorStyle* st);
  // #CAT_ActR execute an action
  virtual void  ExecPunch(ActrEvent& event, ActrMotorStyle* st);
  // #CAT_ActR execute a punch action
  virtual void  ExecPeck(ActrEvent& event, ActrMotorStyle* st);
  // #CAT_ActR execute a peck action
  virtual void  ExecPeckRecoil(ActrEvent& event, ActrMotorStyle* st);
  // #CAT_ActR execute a peck_recoil action
  virtual void  ExecHandPly(ActrEvent& event, ActrMotorStyle* st);
  // #CAT_ActR execute a hand_ply action
  virtual void  ExecCursorPly(ActrEvent& event, ActrMotorStyle* st);
  // #CAT_ActR execute a cursor_ply action

  virtual void  InitHandPos();
  // #CAT_ActR initialize hand position to home keys
  virtual void  InitPosToKey();
  // #IGNORE initialize the pos_to_key mapping matrix
  virtual void  InitMotorStyles();
  // #IGNORE initialize the motor styles

  virtual void  Clear_impl();
  // #IGNORE implement clearing

  void  InitModule() override;
  void  ProcessEvent(ActrEvent& event) override;
  bool  ProcessQuery(ActrBuffer* buf, const String& query,
                             bool why_not = false) override;
  bool  SetParam(const String& param_nm, Variant par1, Variant par2) override;
  void  Init() override;

  TA_SIMPLE_BASEFUNS(ActrMotorModule);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrMotorModule_h
