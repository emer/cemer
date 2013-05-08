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

#ifndef ActrModel_h
#define ActrModel_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ActrChunkType_List>
#include <ActrModule_List>
#include <ActrBuffer_List>
#include <ActrEvent_List>
#include <DataTable>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrModel);

class E_API ActrModel : public taNBase {
  // ##INSTANCE ##CAT_ActR a complete ACT-R model, including productions, buffers, chunks, etc
INHERITED(taNBase)
public:
  enum ModelFlags { // #BITS ActR model flags
    MF_NONE             = 0, // #NO_BIT
    LOG_EVENTS          = 0x0001, // log all events processed to data table
    SAVE_ALL_EVENTS     = 0x0002, // never delete any events from the events_list -- useful for debugging to see a trace of everything that happened during a run in the events list -- otherwise culls the list periodically
  };

  enum RunState { // current run state for this model
    DONE = 0,   // there is no program running or stopped; any previous run completed
    RUN,        // model is currently running
    STOP,       // the model is stopped (note: NOT the same as "DONE")
    NOT_INIT,   // model has not yet been initialized
  };

  String                desc;  // #EDIT_DIALOG #HIDDEN_INLINE description of this model
  ModelFlags            flags; // misc flags for controlling behavior of the model
  float                 cur_time;   // #READ_ONLY #SHOW current time in the model

  ActrChunkType_List    chunk_types;  // all chunk types used within the model must be defined here
  ActrModule_List       modules;      // modules -- always contains declarative as the first one, and optional other ones
  ActrBuffer_List       buffers;      // buffers for containing active chunks -- always contains at least retrieval and goal buffers, and others according to modules
  ActrEvent_List        events;       // #NO_SAVE #EXPERT currently scheduled events
  int                   cur_event_idx; // #READ_ONLY current event index in list of events
  DataTableRef          log_table;     // data table to log events into

  RunState              run_state;
  // #READ_ONLY #NO_SAVE this model's running state

  inline void           SetModelFlag(ModelFlags flg)   { flags = (ModelFlags)(flags | flg); }
  // #CAT_Flags set flag state on
  inline void           ClearModelFlag(ModelFlags flg) { flags = (ModelFlags)(flags & ~flg); }
  // #CAT_Flags clear flag state (set off)
  inline bool           HasModelFlag(ModelFlags flg) const { return (flags & flg); }
  // #CAT_Flags check if flag is set
  inline void           SetModelFlagState(ModelFlags flg, bool on)
  { if(on) SetModelFlag(flg); else ClearModelFlag(flg); }
  // #CAT_Flags set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           ToggleModelFlag(ModelFlags flg)
  { SetModelFlagState(flg, !HasModelFlag(flg)); }
  // #CAT_Flags toggle model flag


  virtual void          Init();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT #CAT_Run initialize model at the start
  virtual void          Step();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT #CAT_Run run next step of processing
  virtual void          Run();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT #CAT_Run run to completion or until Stop is pressed or emitted
  virtual void          Cont();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT #CAT_Run continue running until Stop is pressed or emitted -- key difference from Run is that it does not set DONE when it finishes -- suitable for using stop to break and then continue where it left off
  virtual void          Stop();
  // #BUTTON #GHOST_OFF_run_state:RUN #CAT_Run stop running if currently running

  virtual void          RunNextEvent();
  // #IGNORE run the next event -- called by above processing functions


  virtual int           InsertEventInOrder(ActrEvent* ev);
  // #CAT_ActR insert a new event in the correct sorted position according to time and priority

  virtual ActrEvent*    ScheduleEvent(float time_fm_now, int priority,
                                     ActrModule* src_module, ActrModule* dst_module,
                                     ActrBuffer* dst_buffer, const String& action,
                                     const String& params,
                                     ActrAction* act = NULL, ActrChunk* chnk = NULL,
                                     TypeDef* event_type = NULL);
  // #CAT_ActR schedule an event -- creates event, adds in proper position to list of currently scheduled events, and returns pointer to new event -- called by modules -- must set dst_module as destination module to process event, unless it is action=!Stop! event -- rest are various optional args to be processed by destination module -- can create subtypes of ActrEvent with event_type arg

  virtual void          LogEvent(float time, const String& module,
                                 const String& action, const String& target = "", 
                                 const String& params = "", const String& dst_module = "",
                                 float priority = 0.0f, const String& prod_action = "",
                                 const String& chunk = "");
  // #CAT_ActR log an event -- writes to the corresponding columns in the log table -- if time <= 0 then cur_time is output

  virtual void          FormatLogTable();
  // #CAT_ActR configure the columns in the log table for holding run trace

  virtual void          DefaultConfig();
  // #CAT_ActR ensure that we have at least the basic default config
  virtual ActrModule*   FindMakeModule(const String& nm, TypeDef* td,
                                       bool& made_new);
  // #CAT_ActR find or make a module of the given name and type -- initializes the module if it makes a new one

  override String       GetDesc() const {return desc;}

  TA_SIMPLE_BASEFUNS(ActrModel);
protected:
  void UpdateAfterEdit_impl(); 

private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrModel_h
