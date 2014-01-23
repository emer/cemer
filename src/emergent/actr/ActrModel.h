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
#include <taSmartRefT>

// member includes:
#include <ActrChunkType_List>
#include <ActrModule_List>
#include <ActrBuffer_List>
#include <ActrEvent_List>
#include <ActrProduction>

#include <DataTable>
#include <NameVar_Array>

// declare all other types mentioned but not required to include:
class ActrDeclarativeModule; //
class ActrProceduralModule; //
class ActrImaginalModule; //
class ActrVisionModule; //
class ActrModel; //
SmartRef_Of(ActrModel); // ActrModelRef


eTypeDef_Of(ActrGlobalParams);

class E_API ActrGlobalParams : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_ActR global parameters for ACT-R models
INHERITED(taOBase)
public:
  bool          enable_sub_symbolic; // :esc in ACT-R -- enable the sub-symbolic computations for choosing items from declarative memory and selecting productions, when multiple are available
  bool          enable_rnd;     // :er in ACT-R -- enable random tie-breaking -- only likely to occur when enable_sub_symbolic is off (or noise params are set to 0) -- when enable_rnd is on, then when multiple productions match, one is chosen at random
  
  TA_SIMPLE_BASEFUNS(ActrGlobalParams);
private:
  void	Initialize();
  void 	Destroy()	{ };
};


eTypeDef_Of(ActrModel);

class E_API ActrModel : public taNBase {
  // ##INSTANCE ##CAT_ActR #EXPAND_DEF_2 ##FILETYPE_ActrModel ##EXT_actr a complete ACT-R model, including productions, buffers, chunks, etc
INHERITED(taNBase)
public:
  enum ModelFlags { // #BITS ActR model flags
    MF_NONE             = 0, // #NO_BIT
    LOG_EVENTS          = 0x0001, // log all events processed to data table -- otherwise does not log any events -- specific modules also have more detailed levels of logging available
    UPDATE_GUI          = 0x0002, // update the gui display with state changes so you can see which productions fired and which chunks were selected
    SAVE_ALL_EVENTS     = 0x0004, // never delete any events from the events_list -- useful for debugging to see a trace of everything that happened during a run in the events list -- otherwise culls the list periodically
    UPCASE_LOG          = 0x0008, // attempt to match ACT-R upper-casing of items in the log
  };

  enum RunState { // current run state for this model
    DONE = 0,   // there is no program running or stopped; any previous run completed
    RUN,        // model is currently running
    STOP,       // the model is stopped (note: NOT the same as "DONE")
    NOT_INIT,   // model has not yet been initialized
  };

  enum YY_Flags {               // #IGNORE parsing flags
    YYRet_Exit	= 0,	// script is done being parsed
    YYRet_Ok	= 1,	// everything is fine
    YYRet_NoSrc	= -2,	// don't code last line as source
    YYRet_Err	= -3,	// error
    YYRet_Blank = -4,	// blank line
    YYRet_Parse = -5 	// need to parse more
  };

  String                desc;  // #EDIT_DIALOG #HIDDEN_INLINE description of this model
  ModelFlags            flags; // misc flags for controlling behavior of the model
  float                 cur_time;   // #READ_ONLY #SHOW current time in the model
  ActrGlobalParams      params;     // global parameters

  ActrChunkType_List    sys_chunk_types;  // #NO_EXPAND_ALL system chunk types -- defined automatically by modules
  ActrChunk_List        sys_chunks;       // #NO_EXPAND_ALL system globally-defined chunks stored here -- automatically created by modules
  ActrChunkType_List    chunk_types;  // all chunk types used within the model must be defined here
  ActrChunk_List        chunks;       // #NO_EXPAND_ALL globally-defined chunks stored here
  ActrModule_List       modules;      // modules -- always contains declarative as the first one, and optional other ones
  ActrBuffer_List       buffers;      // #NO_EXPAND_ALL buffers for containing active chunks -- always contains at least retrieval and goal buffers, and others according to modules
  ActrEvent_List        events;       // #NO_SAVE #EXPERT currently scheduled events
  int                   cur_event_idx; // #READ_ONLY current event index in list of events
  DataTableRef          log_table;     // data table to log events into

  RunState              run_state;
  // #READ_ONLY #NO_SAVE this model's running state

  int                   load_debug;    // debug level for loading files -- higher numbers are increasing levels of verbosity
  static ActrModelRef   cur_parse; // #IGNORE current file being parsed
  static NameVar_Array  load_keywords; // #IGNORE keywoards for load parser
  String                load_str;  // #IGNORE string of .actr file being loaded
  int                   load_line; // #IGNORE line number for loading
  int                   load_col;  // #IGNORE column number for loading
  int                   load_pos;  // #IGNORE string pos for loading
  int                   load_st_line; // #IGNORE line number for loading
  int                   load_st_col;  // #IGNORE column number for loading
  int                   load_st_pos;  // #IGNORE string pos for loading
  int                   load_st_line_pos;  // #IGNORE string pos for loading
  bool                  load_bang_expr;    // #IGNORE expecting a bang expr -- parse it
  String                load_last_ln; // #IGNORE last line
  String                load_buf;     // #IGNORE generic buffer used for all parsing
  String                load_comment; // #IGNORE last comment processed
  String                load_name;     // #IGNORE return val for last name read
  String                load_string;  // #IGNORE return val for last string read
  String                load_param;  // #IGNORE current parameter
  String                load_tmp;    // #IGNORE temp string holder
  YY_Flags              load_state;  // #IGNORE state of current parse
  ActrChunkTypeRef      load_chtype;  // #IGNORE current chunk type
  ActrChunkRef          load_chunk;   // #IGNORE current chunk
  ActrProductionRef     load_prod;   // #IGNORE current production
  ActrConditionRef      load_cond;   // #IGNORE current condition
  ActrActionRef         load_act;   // #IGNORE current action


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

  bool  LogEvents()     { return HasModelFlag(LOG_EVENTS); }
  bool  UpdateGui()     { return HasModelFlag(UPDATE_GUI); }

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
  virtual void          LogEventString(DataTable* dt, const String& val, const String& colnm);
  // #IGNORE log a string event to a column -- does the UPCASE_LOG logic

  virtual void          FormatLogTable();
  // #CAT_ActR configure the columns in the log table for holding run trace

  virtual void          DefaultConfig();
  // #CAT_ActR ensure that we have at least the basic default config
  virtual ActrModule*   FindMakeModule(const String& nm, TypeDef* td,
                                       bool& made_new);
  // #CAT_ActR find or make a module of the given name and type -- initializes the module if it makes a new one
  virtual ActrDeclarativeModule*       DeclarativeModule();
  // #CAT_ActR get the declarative module
  virtual ActrProceduralModule*        ProceduralModule();
  // #CAT_ActR get the procedural module
  virtual ActrGoalModule*              GoalModule();
  // #CAT_ActR get the goal module
  virtual ActrImaginalModule*          ImaginalModule();
  // #CAT_ActR get the imaginal module
  virtual ActrVisionModule*            VisionModule();
  // #CAT_ActR get the vision module

  virtual ActrChunkType* FindChunkType(const String& type_name);
  // #CAT_ActR find a chunk type by name or emit error if not found
  virtual ActrChunkType* FindMakeChunkType(const String& type_name);
  // #CAT_ActR find a chunk type by name or make it if not otherwise found (emitting warning message if so) -- this is for *runtime* access to chunktypes -- do not use for initial creation of new chunks (see DefineChunkType instead)
  virtual ActrChunkType* DefineChunkType(const String& type_name,
                                         const String& par_name=_nilString,
                                         const String& slot_0 = _nilString,
                                         const String& slot_1 = _nilString,
                                         const String& slot_2 = _nilString,
                                         const String& slot_3 = _nilString,
                                         const String& slot_4 = _nilString,
                                         const String& slot_5 = _nilString,
                                         const String& slot_6 = _nilString,
                                         const String& slot_7 = _nilString,
                                         const String& slot_8 = _nilString,
                                         const String& slot_9 = _nilString,
                                         const String& slot_a = _nilString,
                                         const String& slot_b = _nilString);
  // #CAT_ActR one-stop define chunk type command, optinally setting parent type and slots
  virtual ActrChunk*    DefineChunk(const String& chunk_nm, const String& type_name);
  // #CAT_ActR define a global chunk of given name and type

  virtual ActrChunkType* DefineChunkTypeSys(const String& type_name,
                                            const String& par_name=_nilString,
                                            const String& slot_0 = _nilString,
                                            const String& slot_1 = _nilString,
                                            const String& slot_2 = _nilString,
                                            const String& slot_3 = _nilString,
                                            const String& slot_4 = _nilString,
                                            const String& slot_5 = _nilString,
                                            const String& slot_6 = _nilString,
                                            const String& slot_7 = _nilString,
                                            const String& slot_8 = _nilString,
                                            const String& slot_9 = _nilString,
                                            const String& slot_a = _nilString,
                                            const String& slot_b = _nilString);
  // #CAT_ActR one-stop define system chunk type command, optinally setting parent type and slots
  virtual ActrChunk*    DefineChunkSys(const String& chunk_nm, const String& type_name);
  // #CAT_ActR define a system global chunk of given name and type

  virtual bool          LoadActrFile(const String& fname="");
  // #BUTTON #MENU #EXT_lisp,actr #CAT_File #FILETYPE_ActrModel #FILE_DIALOG_LOAD read actr model file in standard actr lisp format (leave fname empty to pull up file chooser)
  virtual void          SaveActrFile(const String& fname="");
  // #BUTTON #MENU #EXT_lisp,actr #CAT_File #FILETYPE_ActrModel #FILE_DIALOG_SAVE save actr model file in standard actr lisp format (leave fname empty to pull up file chooser)

  virtual int           Lex();
  // #IGNORE lex for loading actr files -- defined in ActrLex.cpp
  virtual int           Getc();
  // #IGNORE get char for parsing
  virtual int           Peekc();
  // #IGNORE peek next char
  virtual void          unGetc(int c);
  // #IGNORE peek next char
  virtual int           skipwhite();
  // #IGNORE skip whitespace
  virtual int           skipwhite_peek();
  // #IGNORE skip whitespace
  virtual int           skipwhite_nocr();
  // #IGNORE skip whitespace
  virtual int           skipline();
  // #IGNORE skip whitespace
  virtual int           readword(int c);
  // #IGNORE read word into load_buf
  virtual int           skip_till_rp(int init_depth=0);
  // #IGNORE skip over everything until a right paren is found that closes to-be-ignored expression
  virtual void          ResetParse();
  // #IGNORE reset parsing state
  virtual void          InitLoadKeywords();
  // #IGNORE init keywords
  virtual void          ParseErr(const char* er);
  // #IGNORE parsing error handling routine

  virtual void          SetParam(const String& param_nm, Variant par1,
                                 Variant par2 = _nilVariant);
  // #CAT_ActR set parameter -- parses standard ACT-R parameter names

  virtual void          ResetModel();
  // #EXPERT reset the model entirely -- remove everything from existing model -- use with caution!

  String       GetDesc() const CPP11_OVERRIDE {return desc;}
  bool         CheckConfig_impl(bool quiet) CPP11_OVERRIDE;
  void         CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(ActrModel);
protected:
  void UpdateAfterEdit_impl(); 

private:
  SIMPLE_COPY(ActrModel);
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrModel_h
