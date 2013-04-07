// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef Program_h
#define Program_h 1

// parent includes:
#include <taNBase>
#include <AbstractScriptBase>
#include <ProgramRef>

// member includes:
#include <ScriptBase_List>
#include <ProgObjList>
#include <ProgType_List>
#include <ProgVar_List>
#include <Function_List>
#include <ProgEl_List>
#include <Program_List>
#include <ProgLine_List>
#include <String_Array>

// declare all other types mentioned but not required to include:
class cssProgSpace; // 
class cssTA_Base; // 
class ProgLib; // 
class Program_Group; // 
class MemberDef; // 
class ProgLibEl; // 
class ProgVar; // 
class ProgramCallBase; //
class iPanelOfProgram; //
class iPanelSet; //


taTypeDef_Of(Program);

class TA_API Program: public taNBase, public AbstractScriptBase {
  // #STEM_BASE ##TOKENS ##INSTANCE ##EXT_prog ##FILETYPE_Program ##CAT_Program ##UNDO_BARRIER a structured gui-buildable program that generates css script code to actually run
INHERITED(taNBase)
public:
  enum ProgFlags { // #BITS program flags
    PF_NONE             = 0, // #NO_BIT
    NO_STOP_STEP        = 0x0001, // #AKA_NO_STOP this program cannot be stopped by Stop or Step buttons -- set this flag for simple helper programs to prevent them from showing up in the step list of other programs
    SELF_STEP           = 0x0002, // #NO_BIT this program has a StopStepPoint program element within it, and thus it shows up within its own list of Step programs -- this flag is set automatically during Init
    TRACE               = 0x0004, // trace the running of this program by recording each line to the css console as the program runs
    STARTUP_RUN         = 0x0008, // run this prgram at startup (after project is fully loaded and everything else has been initialized) -- if multiple programs are so marked, they will be run in the order they appear in the browser (depth first)
    OBJS_UPDT_GUI       = 0x0010, // when this flag is set, changes to the objs objects update the gui as they happen -- otherwise they are only updated after the program finishes (much faster)
    LOCKED              = 0x0020, // this program should not be edited -- you must uncheck this flag prior to editing
  };

  enum ReturnVal { // system defined return values (<0 are for user defined)
    RV_OK       = 0,    // program finished successfully
    RV_COMPILE_ERR,     // script couldn't be compiled
    RV_CHECK_ERR,       // program or its dependencies failed CheckConfig
    RV_INIT_ERR,        // initialization failed (note: user prog may use its own value)
    RV_RUNTIME_ERR,     // misc runtime error (ex, null pointer ref, etc.)
    RV_NO_PROGRAM,      // no program was available to run
  };

  enum RunState { // current run state for this program
    DONE = 0,   // there is no program running or stopped; any previous run completed
    INIT,       // program is running its init_code
    RUN,        // program is running its prog_code
    STOP,       // the program is stopped (note: NOT the same as "DONE") -- check stop_reason etc for more information
    NOT_INIT,   // init has not yet been run
  };

  enum StopReason {             // reason why the program was stopped
    SR_NONE,                    // no stop reason set (initialized value)
    SR_USER_STOP,               // Stop button was pressed by user -- stop_msg is name of program where Stop() was hit
    SR_USER_ABORT,              // Abort button was pressed by user -- stop_msg is name of program where Abort() was hit
    SR_USER_INTR,               // Ctrl-c was pressed in the console to interrupt processing -- stop_msg is name of css program space that was top at the time
    SR_STEP_POINT,              // the program reached the stopping point associated with Step mode -- stop_msg has program name
    SR_BREAKPOINT,              // a css breakpoint was reached -- stop_msg has info
    SR_ERROR,                   // some form of runtime error occurred -- stop_msg has text
  };

  enum ProgLibs {
    USER_LIB,                   // user's personal library
    SYSTEM_LIB,                 // local system library
    WEB_LIB,                    // web-based library
    SEARCH_LIBS,                // search through the libraries (for loading)
  };

  static ProgLib*       prog_lib; // #NO_SHOW_TREE #NO_SAVE library of available programs

  Program_Group*        prog_gp;
  // #NO_SHOW #READ_ONLY #NO_SAVE #NO_SET_POINTER our owning program group -- needed for control panel stuff

  RunState              run_state;
  // #READ_ONLY #NO_SAVE this program's run state
  static bool           stop_req;
  // #READ_ONLY #NO_SAVE a stop was requested by someone -- stop at next chance
  static StopReason     stop_reason;
  // #READ_ONLY #NO_SAVE reason for the stop request
  static String         stop_msg;
  // #READ_ONLY #NO_SAVE text message associated with stop reason (e.g., err msg, breakpoint info, etc)
  static bool           step_mode;
  // #READ_ONLY #NO_SAVE the program was run in step mode -- check for stepping
  static ProgramRef     cur_step_prog;
  // #READ_ONLY #NO_SAVE the current program to be single-stepped -- set by the Step call of the program that was last run
  static ProgramRef     last_run_prog;
  // #READ_ONLY #NO_SAVE the last program to have been run by the user -- top-level run call
  static ProgramRef     last_stop_prog;
  // #READ_ONLY #NO_SAVE the last program that was stopped from a stop request of any sort
  static int            cur_step_n;
  // #READ_ONLY #NO_SAVE current number of steps to take -- set by the Step call of the program that was last run
  static int            cur_step_cnt;
  // #READ_ONLY #NO_SAVE current step count -- incremented until cur_step_n is reached
  static RunState       global_run_state;
  // #READ_ONLY #NO_SAVE global run state -- set by the last program to run in gui mode (from the Run, Init, Step buttons) -- used primarily to prevent multiple programs from running at the same time
  static String         global_trace;
  // #READ_ONLY #NO_SAVE trace of programs called at point of last stop -- for GlobalTrace function
  static int64_t        global_init_timestamp;
  // #READ_ONLY #NO_SAVE first program to call Init sets this timestamp, and others compare to it and don't run in CallInit multiple times if their timestamp matches this one

  String                short_nm;
  // short name for this program -- as brief as possible -- used for Step display info
  String                tags;
  // #EDIT_DIALOG list of comma separated tags that indicate the basic function of this program -- should be listed in hierarchical order, with most important/general tags first, as this is how they will be sorted in the program library
  String                desc;
  // #EDIT_DIALOG #HIDDEN_INLINE description of what this program does and when it should be used (used for searching in prog_lib -- be thorough!)
  ProgFlags             flags;
  // control flags, for display and execution control
  ProgObjList           objs;
  // #TREEFILT_ProgGp create persistent objects of any type here that are needed for the program -- each object will automatically create an associated variable
  ProgType_List         types;
  // user-defined types for this program (new enumerated types and class objects)
  ProgVar_List          args;
  // global variables that are parameters (arguments) for callers
  ProgVar_List          vars;
  // global variables accessible outside and inside script
  Function_List         functions;
  // function code (for defining subroutines): goes at top of script and can be called from init or prog code
  ProgEl_List           init_code;
  // initialization code: run when the Init button is pressed -- this must be all local to this program (no ProgramCall to other programs), and should generally be very simple and robust code as it is not checked in advance of running (to prevent init-dependent Catch-22 scenarios)
  ProgEl_List           prog_code;
  // program code: run when the Run/Step button is pressed: this is the main code

  int                   ret_val;
  // #HIDDEN #GUI_READ_ONLY #NO_SAVE return value: 0=ok, +ve=sys-defined err, -ve=user-defined err; also accessible inside program
  ProgEl_List           sub_prog_calls;
  // #HIDDEN #NO_SAVE the ProgramCall direct subprogs of this one, enumerated in the PreGen phase (note: these are ProgramCallBase's, not the actual Program's)
  Program_List          sub_progs_dir;
  // #HIDDEN #NO_SAVE direct sub-programs -- called by sub_prog_calls within this program -- also populated during the PreGen phase
  Program_List          sub_progs_all;
  // #HIDDEN #NO_SAVE the full set of all sub-programs, including sub-programs of sub-programs..
  Program_List          sub_progs_step;
  // #HIDDEN #NO_SAVE all the sub programs eligible for single-stepping
  bool                  sub_progs_updtd;
  // #READ_ONLY #NO_SAVE the sub programs were updated -- this is set when updated and cleared after a datachanged
  bool                  m_stale;
  // #READ_ONLY #NO_SAVE dirty bit -- needs to be public for activating the Compile button
  String                view_script;
  // #READ_ONLY #NO_SAVE current view of script
  String                view_listing;
  // #READ_ONLY #NO_SAVE current view of listing
  String                local_trace;
  // #READ_ONLY #NO_SAVE local back trace for this program -- rendered by LocalTrace function

  ProgramRef            step_prog;
  // #FROM_GROUP_sub_progs_step The default program to single step for the Step function in this program
  int                   step_n;
  // #MIN_1 how many steps to take when stepping at this program level (i.e., when this program name is clicked on the Step button of any other program) -- also set by the step button dynamically
  ProgLine_List         script_list;
  // #HIDDEN #NO_SAVE list of the script lines with important meta-data etc -- this is official source of script listing

  ProgEl_List           load_code; // #HIDDEN #NO_SAVE #OBSOLETE obsolete and will be removed later -- only here to elminate load warnings

  inline void           SetProgFlag(ProgFlags flg)   { flags = (ProgFlags)(flags | flg); }
  // #CAT_Flags set flag state on
  inline void           ClearProgFlag(ProgFlags flg) { flags = (ProgFlags)(flags & ~flg); }
  // #CAT_Flags clear flag state (set off)
  inline bool           HasProgFlag(ProgFlags flg) const { return (flags & flg); }
  // #CAT_Flags check if flag is set
  inline void           SetProgFlagState(ProgFlags flg, bool on)
  { if(on) SetProgFlag(flg); else ClearProgFlag(flg); }
  // #CAT_Flags set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           ToggleProgFlag(ProgFlags flg)
  { SetProgFlagState(flg, !HasProgFlag(flg)); }
  // #CAT_Flags toggle program flag

  void                  ToggleTrace();
  // #MENU #MENU_ON_Object #DYN1 toggle the TRACE flag to opposite of current state: flag indicates whether to record a trace of program execution in the css console or not

  virtual void		ProjDirToCurrent();
  // #CAT_Run make sure that the project directory is the current directory
  static void           SetStopReq(StopReason stop_rsn, const String& stop_message = "");
  // #CAT_Run request that the currently-running program stop at its earliest convenience..
  static void           ClearStopReq();
  // #CAT_Run reset the stop request information
  static const String   GetDescString(const String& dsc, int indent_level);
  // #IGNORE get an appropriately formatted version of the description string for css code

  bool                  isStale() const {return m_stale;}
  override void         setStale(); // indicates a component has changed
  void                  SetRunState(RunState value);
  // #IGNORE sets the local AND global run state -- don't use for just local run state updates
  bool                  AlreadyRunning();
  // #IGNORE check if any program anywhere is already running -- if so, don't allow gui run

  override ScriptSource scriptSource() {return ScriptString;}
  override const String scriptString();

  virtual const String  ProgramListing();
  // #CAT_Code generate the listing of the program (NOT the underlying CSS code -- just the program)

  virtual void  Init();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT #CAT_Run #SHORTCUT_F8 set the program state back to the beginning
  virtual void  Run_Gui();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT #CAT_Run #LABEL_Run #SHORTCUT_F9 run the program -- if not yet Init, will run Init first
  virtual void  Run();
  // #CAT_Run run the program -- if not yet Init, will run Init first
  virtual void  Step_Gui(Program* step_prg = NULL);
  // #BUTTON #STEP_BUTTON #CAT_Run #LABEL_Step #SHORTCUT_F10 step the program at the level of the given program -- if NULL then step_prog default value will be used
  virtual void  Step(Program* step_prg = NULL);
  // #CAT_Run step the program at the level of the given program -- if NULL then step_prog default value will be used
  virtual void  Stop();
  // #BUTTON #GHOST_OFF_run_state:RUN #CAT_Run #SHORTCUT_F11 stop the current program at its next natural stopping point (i.e., cleanly stopping when appropriate chunks of computation have completed)
  virtual void  Abort();
  // #BUTTON #GHOST_OFF_run_state:RUN #CAT_Run #SHORTCUT_F12 stop the current program immediately, regardless of where it is
  virtual bool  RunFunction(const String& fun_name);
  // #CAT_Run run a particular function within the program -- returns false if function not found -- function must not take any args
  virtual void  StepCss();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT #CAT_Run run one step of underlying css script code, from current point of execution

  virtual bool  StopCheck();
  // #CAT_Run calls event loop, then checks for STOP state, true if so
  virtual bool  IsStepProg();
  // #CAT_Run is this program the currently selected step_prog? only true if in step_mode too
  virtual void  UpdateUi();
  // #IGNORE update gui from changes in run status etc -- for global program controls

  virtual void  Compile();
  // #BUTTON #GHOST_ON_script_compiled:true #CAT_Code generate and compile the script code that actually runs (if this button is available, you have changed something that needs to be recompiled)
  virtual void  CmdShell();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP #CAT_Code set css command shell to operate on this program, so you can run, debug, etc this script from the command line
  virtual void  ExitShell();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP #CAT_Code exit the command shell for this program (shell returns to previous script)
  virtual void  GlobalTrace();
  // #BUTTON #CAT_Code display trace of all the programs called up to the point of the last stop (e.g., due to error or user stop/step)
    static String  RenderGlobalTrace(bool html = true);
    // #CAT_Code #EXPERT render a string representation of the global trace -- if html then render to html format that is useful for gui dialog with clickable program href links -- else plain text

  virtual void  LocalTrace();
  // #BUTTON #GHOST_OFF_run_state:STOP #CAT_Code display trace of program flow within this program up to the point of the last stop (e.g., due to error or user stop/step)
    virtual String  RenderLocalTrace(bool html = true);
    // #CAT_Code #EXPERT render a string representation of the local trace -- if html then render to html format that is useful for gui dialog with clickable program href links -- else plain text

  virtual void  UpdateCallerArgs();
  // #BUTTON #CAT_Code run UpdateArgs on all the other programs that call me, and also display all these calls in the Find dialog (searching on this program's name) so you can make sure the args are correct for each such program
  virtual void  CssError(int src_ln_no, bool running, const String& err_msg);
  // #IGNORE an error was triggered by css -- this is callback from css Error handling routine for program to update gui with relevant info
  virtual void  CssWarning(int src_ln_no, bool running, const String& err_msg);
  // #IGNORE a warning was triggered by css -- this is callback from css Warning handling routine for program to update gui with relevant info
  virtual void  CssBreakpoint(int src_ln_no, int bpno, int pc, const String& prognm,
                              const String& topnm, const String& src_ln);
  // #IGNORE a breakpoint was triggered by css -- this is callback from css breakpoint handling routine for program to update gui with relevant info
  virtual void  taError(int src_ln_no, bool running, const String& err_msg);
  // #IGNORE a general (ta) error was triggered -- this is callback from taMisc::Error handling routine for program to update gui with relevant info
  virtual void  taWarning(int src_ln_no, bool running, const String& err_msg);
  // #IGNORE a general (ta) warning was triggered -- this is callback from taMisc::Warning handling routine for program to update gui with relevant info

  int                   Call(Program* caller);
  // #CAT_Run runs the program as a subprogram called from another running program, 0=success
  int                   CallInit(Program* caller);
  // #CAT_Run runs the program's Init from a superProg Init, 0=success
  virtual bool          SetVar(const String& var_nm, const Variant& value);
  // #CAT_Variables set the value of a program variable (only top-level variables in vars or args) -- can be called from within a running program
  virtual bool          SetVarFmArg(const String& arg_nm, const String& var_nm, bool quiet = false);
  // #CAT_Variables set the value of a program variable (using SetVar) based on the value of startup argument arg_nm -- typically called from startup scripts -- displays information about variable set if !quiet
  virtual Variant       GetVar(const String& var_nm);
  // #CAT_Variables get the value of a program variable (only top-level variables in vars or args) -- can be called from within a running program
  bool                  HasVar(const String& var_nm);
  // EXPERT #CAT_Variables true if has a var/arg called var_nm (only top-level variables in vars or args) -- can be called from within a running program
  static bool           IsForbiddenName(const String& chk_nm, bool warn=true);
  // #CAT_Code check given name against list of forbidden names -- variables and other objects should check and if forbidden, add an extra character or something
  virtual void          AddVarTo(taNBase* src);
  // #DROPN add a variable in the global vars list to the given object
  virtual void          AddArgTo(taNBase* src);
  // #DROPN add an argument in the global args list to the given object

  virtual void          Reset();
  // #MENU #MENU_ON_Object #MENU_CONTEXT #MENU_SEP_BEFORE #CONFIRM #CAT_Code reset (remove) all program elements -- typically in preparation for loading a new program over this one

  static String         GetProgLibPath(ProgLibs library);
  // #CAT_ProgLib get path to given program library

  virtual void          SaveToProgLib(ProgLibs library = USER_LIB);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #CAT_ProgLib save the program to given program library -- file name = object name -- be sure to add good desc comments!!
  virtual void          LoadFromProgLib(ProgLibEl* prog_type);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #FROM_GROUP_prog_lib #ARG_VAL_FM_FUN #CAT_ProgLib (re)load the program from the program library element of given type

  virtual void          RunLoadInitCode();
  // #CAT_Run Run the initialization code for object pointer variables and program calls -- to resolve pointers after loading

  virtual ProgVar*      FindVarName(const String& var_nm) const;
  // #CAT_Find find given variable within this program -- NULL if not found
  virtual taBase*       FindTypeName(const String& nm) const;
  // #CAT_Find find given type name (e.g., dynamic enum type or value) on list
  virtual Program*      FindProgramName(const String& prog_nm, bool warn_not_found=false) const;
  // #CAT_Find find program of given name, first looking within the group that this program belongs in, and then looking for all programs within the project.  if warn_not_found, then issue a warning if not found
  virtual Program*      FindProgramNameContains(const String& prog_nm, bool warn_not_found=false) const;
  // #CAT_Find find program whose name contains given name, first looking within the group that this program belongs in, and then looking for all programs within the project.  if warn_not_found, then issue a warning if not found
  virtual ProgramCallBase*      FindSubProgTarget(Program* prg);
  // #IGNORE find sub_prog_calls ProgramCallBase that calls given target program

  virtual void          SaveScript(std::ostream& strm);
  // #MENU #MENU_ON_Script #MENU_CONTEXT #CAT_File save the css script generated by the program to a file
  virtual void          SaveListing(std::ostream& strm);
  // #MENU #MENU_SEP_BEFORE #MENU_CONTEXT #CAT_Code save the program listing to a file

  virtual void          ViewScript();
  // #MENU #MENU_CONTEXT #NO_BUSY  #CAT_Code view the css script generated by the program
  virtual bool          ViewScriptEl(taBase* pel);
  // view the css script generated by given program element as highlighted text in view script
  virtual void          ViewScript_Editor();
  // #MENU #MENU_CONTEXT #CAT_Code open css script in editor defined by taMisc::edit_cmd -- saves to a file based on name of object first
  virtual void          ViewScriptUpdate();
  // #IGNORE regenerate view_script listing

  virtual String        GetProgCodeInfo(int line_no, const String& code_str);
  // #CAT_Code attempt to get program code information (e.g., current variable value) for given code string element on given line number -- code_str may contain other surrounding text which is parsed to extract a variable name or other interpretable value

#ifdef TA_GUI
public: // XxxGui versions provide feedback to the user
  virtual void          ViewListing();
  // #MENU #MENU_CONTEXT #NO_BUSY #CAT_Code view the listing of the program
  virtual void          ViewListing_Editor();
  // #MENU #MENU_CONTEXT #CAT_Code open listing of the program in editor defined by taMisc::edit_cmd -- saves to a file based on name of object first
#endif

  virtual void          ClearAllBreakpoints();
  // #MENU #MENU_SEP_BEFORE #MENU_ON_Script #MENU_CONTEXT #CAT_Code clear all breakpoints that might have been set in the program elements
  virtual void          SetAllBreakpoints();
  // #IGNORE re-set all the breakpoints in the code to be currently active -- after recompiling, need to reinstate them all

  static Program*       MakeTemplate(); // #IGNORE make a template instance (with children) suitable for root.templates
  static void           MakeTemplate_fmtype(Program* prog, TypeDef* td); // #IGNORE make from typedef

  virtual bool          SelectCtrlFunsForEdit(SelectEdit* editor,
              const String& extra_label = "", const String& sub_gp_nm = "");
  // #MENU #MENU_ON_SelectEdit #MENU_SEP_BEFORE #NULL_OK_0  #NULL_TEXT_0_NewEditor #CAT_Display add the program control functions (Init, Run, Step, Stop) to a select edit dialog that collects selected members and methods from different objects (if editor is NULL, a new one is created in .edits). returns false if method was already selected.   extra_label is prepended to item names, and if sub_gp_nm is specified, items will be put in this sub-group (new one will be made if it does not yet exist)

  virtual iPanelOfProgram* FindMyProgramPanel();
  // #IGNORE find my program panel, which contains the program editor -- useful for browser-specific operations
  virtual iPanelSet* FindMyDataPanelSet();
  // #IGNORE find my data panel set, which contains all the more specific data panels

  virtual bool          BrowserSelectMe_ProgItem(taOBase* itm);
  // #IGNORE perform BrowserSelectMe function for program sub-item (prog el, etc)
  virtual bool          BrowserExpandAll_ProgItem(taOBase* itm);
  // #IGNORE perform BrowserExpandAll function for program sub-item (prog el, etc)
  virtual bool          BrowserCollapseAll_ProgItem(taOBase* itm);
  // #IGNORE perform BrowserCollapseAll function for program sub-item (prog el, etc)

  virtual bool          ViewCtrlPanel();
  // #CAT_Display select the edit/middle panel view of this object to be for the control panel
  virtual bool          ViewProgEditor(int select_src_line = -1);
  // #CAT_Display select the edit/middle panel view of this object to be for the program editor, and optionally select program element at given source code line if passed
  virtual bool          ViewCssScript(int view_src_line = -1);
  // #CAT_Display select the edit/middle panel view of this object to be for the program css script view, and optionally scroll to source code line if passed
  virtual bool          ViewProperties();
  // #CAT_Display select the edit/middle panel view of this object to be for the program properties

  ////////////////////////////////////////////////////
  //    program script gen interface
  virtual bool          AddLine(taBase* prog_el, const String& code,
                int pline_flags = ProgLine::PL_NONE, const String& desc = _nilString);
  // #IGNORE add one line of code to the program listing -- this is the official interface for adding to program -- code should be pure code without any indent -- indent managed separately -- main_line = this is the main line of code associated with this prog_el that is displayed during verbose logging etc
  virtual bool          AddVerboseLine(ProgEl* prog_el, bool insert_at_start = true,
                                       const String& msg_code = _nilString);
  // #IGNORE add one line of code to the program listing that calls Program::VerboseOut with optional code in msg_code that generates a message output strings (comma separated) that are passed as args to VerboseOut (default will provide listing of program and code -- only augment if can provide useful runtime info) -- if insert_start, then this verbose line will be inserted at the start of the prog_el code -- always call this *after* having added main code lines -- refers to the MAIN_LINE
  inline int            IncIndent()     { return ++cur_indent; }
  // #IGNORE increment the indent level, returning new level
  inline int            DecIndent()     { return --cur_indent; }
  // #IGNORE decrement the indent level, returning new level
  virtual void          AddDescString(taBase* prog_el, const String& dsc);
  // #IGNORE add an appropriately formatted version of the description string to code -- for a full line desc comment at start
  virtual bool          ToggleBreakpoint(ProgEl* pel);
  // #IGNORE toggle breakpoint for given program element -- handles updating -- returns false if not able to complete
    void                SetBreakpoint_impl(ProgEl* pel);
    // #IGNORE actually clear the breakpoint in css for this el
    void                ClearBreakpoint_impl(ProgEl* pel);
    // #IGNORE actually set the breakpoint in css for this el
  virtual bool          ScriptLinesEl(taBase* pel, int& start_ln, int& end_ln);
  // #IGNORE get the script code lines (in 1-based line numbers, as used in css) associated with the given program element

  static void           VerboseOut(Program* prg, int code_line,
                                   const char* a=0, const char* b=0, const char* c=0,
                                   const char* d=0, const char* e=0, const char* f=0,
                                   const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Debug generate verbose output as given by input strings for given code line -- this is called by verbose program elements when they run

  override int          GetSpecialState() const;
  override String       GetTypeDecoKey() const { return "Program"; }
  override Variant      GetGuiArgVal(const String& fun_name, int arg_idx);
  override void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);
  override void         CallFun(const String& fun_name);

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(Program);

public: // ScriptBase i/f
  override TypeDef*     GetThisTypeDef() const {return GetTypeDef();}
  // #IGNORE
  override void*        GetThisPtr() { return (void*)this; }
  // #IGNORE

protected:
  static String_Array   forbidden_names;
  // #NO_SAVE #READ_ONLY #HIDDEN names that should not be used for variables and other such things because they are already in use

  String                m_scriptCache; // cache of script, managed by implementation
  String                m_listingCache; // cache of listing, managed by implementation
  bool                  m_checked; // flag to help us avoid doing CheckConfig twice
  int                   cur_indent;
  // current indent level -- used in adding code
  int64_t               last_init_timestamp;

  override void         UpdateAfterEdit_impl();
  override bool         CheckConfig_impl(bool quiet);
  override void         CheckChildConfig_impl(bool quiet, bool& rval);
  override void         InitScriptObj_impl(); // no "this" and install
  override bool         PreCompileScript_impl(); // CheckConfig & add/update the global vars

  virtual void          Stop_impl();
  virtual int           Run_impl();
  virtual int           Cont_impl();
  override void         ScriptCompiled(); // #IGNORE
  virtual void          UpdateProgVars(); // put global vars in script, set values
  void                  ShowRunError(); // factored error msg code
#ifdef TA_GUI
  virtual void          ViewScript_impl(int sel_ln_st = -1, int sel_ln_ed = -1);
#endif
  virtual void          GetSubProgsAll(int depth=0);
  // populate the sub_progs_all lists of all sub programs
  virtual void          GetSubProgsStep();
  // populate the sub_progs_step lists of all sub programs in sub_progs_all that don't have the NO_STOP_STEP flag set

  static void           InitForbiddenNames();

private:
  void  Copy_(const Program& cp);
  void  Initialize();
  void  Destroy();
};

// note: ProgramRef is in separate ProgramRef file

#endif // Program_h
