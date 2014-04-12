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

#include "Program.h"
#include <Program_Group>
#include <taProject>
#include <ControlPanel>
#include <ProgramCallBase>
#include <taiEditorOfString>
#include <iDialogChoice>

#include <iPanelOfProgramScript>
#include <iNumberedTextView>

taTypeDef_Of(LocalVars);
taTypeDef_Of(ProgCode);
taTypeDef_Of(DynEnumType);

#include <SigLinkSignal>
#include <taSigLinkItr>
#include <iPanelSet>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>

#include <css_machine.h>

#include <QFileInfo>
#include <QDir>

TA_BASEFUNS_CTORS_DEFN(Program);

using namespace std;

ProgLib* Program::prog_lib = NULL;
String_Array Program::forbidden_names;
bool Program::stop_req = false;
Program::StopReason Program::stop_reason = Program::SR_NONE;
String Program::stop_msg;
bool Program::step_mode = false;
ProgramRef Program::cur_step_prog;
ProgramRef Program::last_run_prog;
ProgramRef Program::last_stop_prog;
int Program::cur_step_n = 1;
int Program::cur_step_cnt = 0;
Program::RunState Program::global_run_state = Program::NOT_INIT;
String Program::global_trace;
int64_t Program::global_init_timestamp = 0;

void Program::Initialize() {
  run_state = NOT_INIT;
  flags = PF_NONE;
  objs.SetBaseType(&TA_taNBase);
  ret_val = 0;
  sub_progs_updtd = false;
  m_stale = true;
  prog_gp = NULL;
  m_checked = false;
  step_n = 1;
  last_init_timestamp = 1;	// not same as global init..
  if(!prog_lib)
    prog_lib = &Program_Group::prog_lib;
}

void Program::Destroy() {
  CutLinks();
}

void Program::InitLinks() {
  inherited::InitLinks();
  taBase::Own(objs, this);
  taBase::Own(types, this);
  taBase::Own(args, this);
  taBase::Own(vars, this);
  taBase::Own(functions, this);
  taBase::Own(init_code, this);
  taBase::Own(prog_code, this);
  taBase::Own(sub_prog_calls, this);
  taBase::Own(sub_progs_dir, this);
  taBase::Own(sub_progs_all, this);
  taBase::Own(sub_progs_step, this);
  taBase::Own(step_prog, this);
  taBase::Own(script_list, this);
  taBase::Own(brk_pts, this);
  
  taBase::Own(load_code, this); // todo: obsolete, remove
  
  if(!taMisc::is_loading) {
    if(prog_code.size == 0) {
      prog_code.New(1, &TA_LocalVars); // make this by default because it is typically needed!
    }
  }
  
  init_code.el_typ = &TA_ProgCode;  // make sure this is default
  prog_code.el_typ = &TA_ProgCode;  // make sure this is default
  
  prog_gp = GET_MY_OWNER(Program_Group);
}

void Program::CutLinks() {
  if(script) {                  // clear first, before trashing anything!
    ExitShellScript();
    script_list.Reset();
    script->ClearAll();
    script->prog_vars.Reset();
    delete script;
    script = NULL;
  }
  step_prog.CutLinks();
  sub_progs_step.CutLinks();
  sub_progs_all.CutLinks();
  sub_progs_dir.CutLinks();
  sub_prog_calls.CutLinks();
  prog_code.CutLinks();
  init_code.CutLinks();
  functions.CutLinks();
  vars.CutLinks();
  args.CutLinks();
  types.CutLinks();
  objs.CutLinks();
  brk_pts.CutLinks();
  prog_gp = NULL;
  inherited::CutLinks();
}

void Program::Reset() {
  if(script) {                  // clear first, before trashing anything!
    script->ClearAll();
    script->prog_vars.Reset();
  }
  sub_progs_step.Reset();
  sub_progs_all.Reset();
  sub_progs_dir.Reset();
  sub_prog_calls.Reset();
  prog_code.Reset();
  init_code.Reset();
  functions.Reset();
  vars.Reset();
  args.Reset();
  types.Reset();
  objs.Reset();
  brk_pts.Reset();
}

void Program::Copy_(const Program& cp) {
  if(script) {                  // clear first, before trashing anything!
    script->ClearAll();
    script->prog_vars.Reset();
  }
  tags = cp.tags;
  desc = cp.desc;
  flags = cp.flags;
  objs = cp.objs;
  types = cp.types;
  args = cp.args;
  vars = cp.vars;
  functions = cp.functions;
  init_code = cp.init_code;
  prog_code = cp.prog_code;
  step_prog = cp.step_prog;
  ret_val = 0; // redo
  m_stale = true; // require rebuild/refetch
  m_scriptCache = "";
  m_checked = false; // redo
  sub_prog_calls.RemoveAll();
  sub_progs_step.RemoveAll();
  sub_progs_all.RemoveAll();
  sub_progs_dir.RemoveAll();
  brk_pts.RemoveAll();  // don't copy breakpoints
  UpdatePointers_NewPar((taBase*)&cp, this); // update any pointers within this guy
  UpdatePointers_NewPar_IfParNotCp((taBase*)&cp, &TA_taProject); // also check for project copy
}

void Program::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //WARNING: the running css prog calls this on any changes to our vars,
  // such as ret_val -- therefore, DO NOT do things here that are incompatible
  // with the runtime, in particular, do NOT invalidate the following state flags:
  //   m_stale, script_compiled
  
  // types.el_typ = &TA_DynEnumBase; // NOO!!! el_typ is the default, el_base = base
  types.el_typ = &TA_DynEnumType;  // want to default to valid type for <Enter> to create valid type
  init_code.el_typ = &TA_ProgCode;  // make sure this is default
  prog_code.el_typ = &TA_ProgCode;  // make sure this is default
  
  if(HasProgFlag(LOCKED)) SetBaseFlag(BF_GUI_READ_ONLY);
  else                    ClearBaseFlag(BF_GUI_READ_ONLY);
  
  //TODO: the following *do* affect generated script, so we should probably call
  // setDirty(true) if not running, and these changed:
  // name, (more TBD...)
  
  if(step_n < 1) step_n = 1;
  
  if(!step_prog) {
    if(sub_progs_step.size > 0)
      step_prog = sub_progs_step.Peek(); // set to last guy on list..
  }
  
  if(short_nm.empty()) {
    String use_nm = name;
    // todo: this is hacky and emergent-dependent...
    if(use_nm.startsWith("Leabra")) use_nm = use_nm.after("Leabra");
    if(use_nm.startsWith("Bp")) use_nm = use_nm.after("Bp");
    if(use_nm.startsWith("Cs")) use_nm = use_nm.after("Cs");
    if(use_nm.startsWith("So")) use_nm = use_nm.after("So");
    if(use_nm.length() > 8)
      use_nm = taMisc::RemoveVowels(use_nm);
    short_nm = taMisc::ShortName(use_nm);
  }
}

bool Program::CheckConfig_impl(bool quiet) {
  //TODO: global program dependencies and other objects -- check them all here
  bool rval = inherited::CheckConfig_impl(quiet);
  m_checked = true;
  if (!rval) ret_val = RV_CHECK_ERR;
  return rval;
}


void Program::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  objs.CheckConfig(quiet, rval);
  types.CheckConfig(quiet, rval);
  args.CheckConfig(quiet, rval);
  vars.CheckConfig(quiet, rval);
  functions.CheckConfig(quiet, rval);
  init_code.CheckConfig(quiet, rval);
  prog_code.CheckConfig(quiet, rval);
}

int Program::Call(Program* caller) {
  run_state = RUN;              // just a local update
  int rval = Cont_impl();
  if(stop_req) {
    script->Stop();             // stop us
    if(caller) {
      caller->script->Stop();     // stop caller!
      caller->script->Prog()->Frame()->pc = 0;
    }
    run_state = STOP;           // we are done
    // NOTE: this backs up to restart the entire call to fun -- THIS DEPENDS ON THE CODE
    // that generates the call!!!!!  ALWAYS MUST BE IN A SUB-BLOCK of code..
  }
  else {
    script->Restart();          // restart script at beginning if run again
    run_state = DONE;           // we are done
  }
  return rval;
}

int Program::CallInit(Program* caller) {
  if(last_init_timestamp == global_init_timestamp)
    return ret_val;		// already done it!
  last_init_timestamp = global_init_timestamp;
  return CallInit_impl(caller);
}

int Program::CallInit_impl(Program* caller) {
  run_state = INIT;    // this is redundant if called from an existing INIT but otherwise needed
  SetAllBreakpoints();          // reinstate all active breakpoints
  Run_impl();
  CheckConfig(false);   // check after running!  see below
  script->Restart();    // for init, always restart script at beginning if run again
  if(!taMisc::check_ok)
    run_state = NOT_INIT;
  else
    run_state = DONE;
  return ret_val;
}

void Program::Init() {
  //   cur_step_prog = NULL;  // if a program calls Init() directly, this will prevent stepping
  // it is not clear if we really need to clear this setting here
  if(AlreadyRunning()) return;
  ProjDirToCurrent();
  ClearStopReq();
  taProject* proj = GET_MY_OWNER(taProject);
  if(proj && proj->file_name.nonempty()) {
    QFileInfo fi(proj->file_name); // set to current working dir on init
    QDir::setCurrent(fi.absolutePath());
  }
  taMisc::Busy();
  SetRunState(INIT);
  
  // record new timestamp for this init session -- CallInit will check and not re-run
  QDateTime tm = QDateTime::currentDateTime();
  global_init_timestamp = tm.toTime_t();
  
  SigEmit(SLS_ITEM_UPDATED_ND); // update button state
  // first run the Init code, THEN do the check.  this prevents a catch-22
  // with Init code that is designed to configure things so there won't be
  // config errors!!  It exposes init code to the possibility of
  // running unchecked code, so we need to make sure all progel's have
  // an extra compile-time check and don't just crash (fail quietly -- err will showup later)
  taMisc::CheckConfigStart(false); // CallInit will do CheckConfig..
  if(!CompileScript(true)) {
    if (ret_val != RV_CHECK_ERR)
      ret_val = RV_COMPILE_ERR;
  }
  if(ret_val == RV_OK) {
    // check args and vars before running any code, to get NULL_CHECK etc
    bool chkargs = args.CheckConfig(false);
    bool chkvars = vars.CheckConfig(false);
    if(chkargs && chkvars) {
      SetAllBreakpoints();          // reinstate all active breakpoints
      bool did_struct_updt = false;
      if(!HasProgFlag(OBJS_UPDT_GUI)) {
        objs.StructUpdateEls(true);
        did_struct_updt = true;
      }
      script->SetDebug((int)HasProgFlag(TRACE));
      script->Run();
      if(did_struct_updt)
        objs.StructUpdateEls(false);
    }
  }
  
  // get these here after all the sub-guys have been initialized -- should now be current
  GetSubProgsAll();
  GetSubProgsStep();
  
  taMisc::DoneBusy();
  // now check us..
  CheckConfig(false);
  taMisc::CheckConfigEnd(); // no flag, because any nested fails will have set it
  if (ret_val != RV_OK) ShowRunError();
  script->Restart();            // restart script at beginning if run again
  
  if(!taMisc::check_ok)
    SetRunState(NOT_INIT);
  else
    SetRunState(DONE);
  stop_req = false;  // this does not do full clear, so that information can be queried
  UpdateUi();
  SigEmit(SLS_ITEM_UPDATED_ND); // update after macroscopic button-press action..
}

void Program::InitScriptObj_impl() {
  AbstractScriptBase::InitScriptObj_impl(); // default one turns out to be fine..
  script->own_program = this;               // enables error callbacks
}

bool Program::PreCompileScript_impl() {
  // as noted in abstractscriptbase: you must call this first to reset the script
  // because if you mess with the existing variables in prog_vars prior to
  // resetting the script, it will get all messed up.  vars on this space are referred
  // to by a pointer to the space and an index off of it, which is important for autos
  // but actually not for these guys (but they are/were that way anyway).
  if(!AbstractScriptBase::PreCompileScript_impl()) return false;
  script_list.ClearAllErrors(); // start fresh
  objs.GetVarsForObjs();
  UpdateProgVars();
  return true;
}

bool Program::AlreadyRunning() {
  if(run_state == RUN || run_state == INIT || global_run_state == RUN ||
     global_run_state == INIT) {
    taMisc::Info("A Program is already running -- cannot run until it is done");
    return true;
  }
  return false;
}

void Program::SetRunState(RunState value) {
  run_state = value;
  global_run_state = value;
}

int Program::Run_impl() {
  ret_val = RV_OK;
  if (!CompileScript()) {
    if (ret_val != RV_CHECK_ERR)
      ret_val = RV_COMPILE_ERR;
  }
  if (ret_val == RV_OK) {
    script->SetDebug((int)HasProgFlag(TRACE));
    script->Run();
    // DO NOT DO!
    // SigEmit(SLS_ITEM_UPDATED_ND);
  }
  return ret_val;
}

int Program::Cont_impl() {
  // note: cont is fast and does not do any compile or run checks.
  // the user cannot access this without having pressed Init first, and that
  // does all the checks.  this is the standard paradigm for such things --
  // init does checks. run assumes things are ok & thus can be fast.
  script->SetDebug((int)HasProgFlag(TRACE));
  script->Cont();
  // note: shared var state likely changed, so update gui
  script_compiled = true; // override any run-generated changes!!
  // do not update this -- too tight -- only at end!
  // SigEmit(SLS_ITEM_UPDATED_ND);
  return ret_val;
}

void Program::Run_Gui() {
  if(AlreadyRunning()) return;
  Run();
}

void Program::Run() {
  if(run_state == NOT_INIT) {
    Init();                     // auto-press Init button!
  }
  if(run_state == STOP && stop_reason == SR_ERROR) {
    Init();                     // auto-reinit after errors!
  }
  if(TestError(run_state != DONE && run_state != STOP, "Run",
               "There was a problem with the Initialization of the Program (see css console for error messages) -- must fix before you can run.  Press Init first, look for errors, then Run")) {
    return;
  }
  ProjDirToCurrent();
  ClearStopReq();
  last_run_prog = this;
  SetAllBreakpoints();          // reinstate all active breakpoints
  step_mode = false;
  cur_step_prog = NULL;
  taMisc::Busy();
  SetRunState(RUN);
  UpdateUi();
  SigEmit(SLS_ITEM_UPDATED_ND); // update button state
  bool did_struct_updt = false;
  if(!HasProgFlag(OBJS_UPDT_GUI)) {
    objs.StructUpdateEls(true);
    did_struct_updt = true;
  }
  Cont_impl();
  if(did_struct_updt)
    objs.StructUpdateEls(false);
  taMisc::DoneBusy();
  if (ret_val != RV_OK) ShowRunError();
  // unless we were stopped, we are done
  if(stop_req) {
    SetRunState(STOP);
    if((stop_reason == SR_ERROR) && (ret_val == RV_OK)) {
      ret_val = RV_RUNTIME_ERR;
    }
  }
  else {
    script->Restart();
    SetRunState(DONE);
  }
  UpdateUi();
  stop_req = false;  // this does not do full clear, so that information can be queried
  SigEmit(SLS_ITEM_UPDATED_ND); // update after macroscopic button-press action..
}

void Program::ShowRunError() {
  //note: if there was a ConfigCheck or runtime error, the user already got a dialog
  if (ret_val == RV_CHECK_ERR || ret_val == RV_RUNTIME_ERR) return;
  String err_str = "Error: The Program did not run -- ret_val=";
  err_str.cat( GetTypeDef()->GetEnumString("ReturnVal", ret_val));
  if (ret_val == RV_COMPILE_ERR) {
    err_str += " (a program did not compile correctly: check the console for error messages)";
  }
  taMisc::Error(err_str);
}

void Program::Step_Gui(Program* step_prg) {
  if(AlreadyRunning()) return;  // already running!
  Step(step_prg);
}

void Program::Step(Program* step_prg) {
  if(run_state == NOT_INIT) {
    Init();                     // auto-press Init button!
  }
  if(TestError(run_state != DONE && run_state != STOP, "Step",
               "There was a problem with the Initialization of the Program (see css console for error messages) -- must fix before you can run.  Press Init first, look for errors, then Step")) {
    return;
  }
  ProjDirToCurrent();
  ClearStopReq();
  SetAllBreakpoints();          // reinstate all active breakpoints
  last_run_prog = this;
  step_mode = true;
  if(step_prg)
    cur_step_prog = step_prg;
  else
    cur_step_prog = step_prog;
  
  if(TestError(!cur_step_prog || !cur_step_prog->owner, "Step",
               "step program selected to step by is either NULL or unowned and likely was deleted -- not Stepping")) {
    return;
  }
  
  if(step_prog != step_prg)     // save this as new default..
    step_prog = step_prg;
  
  cur_step_n = cur_step_prog->step_n; // get from target, not us..
  cur_step_cnt = 0;
  
  taMisc::Busy();
  SetRunState(RUN);
  UpdateUi();
  SigEmit(SLS_ITEM_UPDATED_ND); // update button state
  bool did_struct_updt = false;
  if(!HasProgFlag(OBJS_UPDT_GUI)) {
    objs.StructUpdateEls(true);
    did_struct_updt = true;
  }
  Cont_impl();
  if(did_struct_updt)
    objs.StructUpdateEls(false);
  taMisc::DoneBusy();
  if (ret_val != RV_OK) ShowRunError();
  step_mode = false;
  cur_step_prog = NULL;
  if(stop_req) {
    SetRunState(STOP);
    if((stop_reason == SR_ERROR) && (ret_val == RV_OK)) {
      ret_val = RV_RUNTIME_ERR;
    }
  }
  else {
    script->Restart();
    SetRunState(DONE);
  }
  stop_req = false;                 // this does not do full clear, so that information can be queried
  UpdateUi();
  SigEmit(SLS_ITEM_UPDATED_ND); // update after macroscopic button-press action..
}

void Program::ToggleTrace() {
  ToggleProgFlag(TRACE);
  SigEmitUpdated();
}

void Program::ProjDirToCurrent() {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return;
  proj->ProjDirToCurrent();
}

void Program::SetStopReq(StopReason stop_rsn, const String& stop_message) {
  stop_req = true;
  stop_reason = stop_rsn;
  stop_msg = stop_message;
}

void Program::ClearStopReq() {
  stop_req = false;
  stop_reason = SR_NONE;
  stop_msg = _nilString;
  last_stop_prog = NULL;
}

void Program::UpdateUi() {
  if(!taMisc::gui_active) return;
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return;
  proj->UpdateUi();
  taiM->ProcessEvents();
}

void Program::Stop() {
  if(TestError(run_state != RUN, "Stop",
               "Program is not running")) {
    return;
  }
  SetStopReq(SR_USER_STOP, name);
}

void Program::Abort() {
  SetStopReq(SR_USER_ABORT, name);
  Stop_impl();
}

void Program::Stop_impl() {
  last_stop_prog = this;
  global_trace = RenderGlobalTrace(taMisc::gui_active); // gotta grab it while its hot
  script->Stop();
  //   setRunState(STOP);
  //   SigEmit(SLS_ITEM_UPDATED_ND); // update button state
}

bool Program::IsStepProg() {
  return (step_mode && (cur_step_prog.ptr() == this));
}

bool Program::StopCheck() {
  //NOTE: we call event loop even in non-gui compile, since we can presumably
  // have other ways of stopping, such as something from a socket etc.
  taiM->ProcessEvents();
  // note: this has to be the full processevents and not RunPending,
  // otherwise it never seems to get the events.
  //  taiMiscCore::ProcessEvents();
  // NOTE: the return value of this function is not actually what determines stopping
  // the above processEvents will process any Stop events and this will directly cause
  // css to stop in its tracks.
  if(run_state == STOP) return true;
  if(stop_req) {
    Stop_impl();
    return true;
  }
  if(IsStepProg()) {
    cur_step_cnt++;
    if(cur_step_cnt >= cur_step_n) {
      SetStopReq(SR_STEP_POINT, name);  // stop everyone else
      Stop_impl();                      // time for us to stop
      return true;
    }
  }
  return false;
}

void Program::StepCss() {
  if(AlreadyRunning()) return;  // already running!
  if(run_state == NOT_INIT) {
    Init();                     // auto-press Init button!
  }
  if(TestError(run_state != DONE && run_state != STOP, "StepCss",
               "There was a problem with the Initialization of the Program (see css console for error messages) -- must fix before you can run.  Press Init first, look for errors, then Step")) {
    return;
  }
  SetAllBreakpoints();          // reinstate all active breakpoints
  CmdShell();
  taMisc::Busy();
  SetRunState(RUN);
  SigEmit(SLS_ITEM_UPDATED_ND); // update button state
  bool did_struct_updt = false;
  if(!HasProgFlag(OBJS_UPDT_GUI)) {
    objs.StructUpdateEls(true);
    did_struct_updt = true;
  }
  script->step_mode = 1;
  Cont_impl();
  script->step_mode = 0;
  if(did_struct_updt)
    objs.StructUpdateEls(false);
  taMisc::DoneBusy();
  if (ret_val != RV_OK) ShowRunError();
  if(stop_req) {
    SetRunState(STOP);
    if((stop_reason == SR_ERROR) && (ret_val == RV_OK)) {
      ret_val = RV_RUNTIME_ERR;
    }
  }
  else {
    // we stop every step so not done yet!
    SetRunState(STOP);
  }
  stop_req = false;                 // this does not do full clear, so that information can be queried
  UpdateUi();
  SigEmit(SLS_ITEM_UPDATED_ND); // update after macroscopic button-press action..
}

bool Program::RunFunction(const String& fun_name) {
  if(run_state == NOT_INIT) {
    Init();                     // auto-press Init button!
  }
  if(run_state == STOP && stop_reason == SR_ERROR) {
    Init();                     // auto-reinit after errors!
  }
  // todo: not clear how much more control infrastructure we need..
  script->SetDebug((int)HasProgFlag(TRACE));
  cssEl* rval = script->RunFun(fun_name); // no args right now
  return (bool)rval;
}

void Program::CallFun(const String& fun_name) {
  if(!taMisc::gui_active) return;
  MethodDef* md = GetTypeDef()->methods.FindName(fun_name);
  if(md != NULL)
    md->CallFun((void*)this);
  else if(!RunFunction(fun_name))
    TestWarning(true, "CallFun", "function:", fun_name, "not found on object");
}

void Program::Compile() {
  CompileScript(true);          // always force if command entered
}

void Program::CmdShell() {
  CmdShellScript();
}

void Program::ExitShell() {
  ExitShellScript();
}

void Program::UpdateCallerArgs() {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return;
  FOREACH_ELEM_IN_GROUP(Program, pg, proj->programs) {
    ProgramCallBase* pc = pg->FindSubProgTarget(this);
    if(pc) {
      pc->UpdateArgs();
    }
  }
  proj->programs.GuiFindFromMe("\"Call " + name + "\"");   // find all refs to me in programs
}

void Program::CssError(int src_ln_no, bool running, const String& err_msg) {
  global_trace = RenderGlobalTrace(taMisc::gui_active); // gotta grab it while its hot
  ProgLine* pl = script_list.SafeEl(src_ln_no);
  if(!pl) return;
  pl->SetError();
  // css does not otherwise pull up an error dialog, so we can..
  if (taMisc::gui_active) {
    bool cancel = iDialogChoice::ErrorDialog(NULL, err_msg);
    taMisc::ErrorCancelSet(cancel);
  }
}

void Program::CssWarning(int src_ln_no, bool running, const String& err_msg) {
  ProgLine* pl = script_list.SafeEl(src_ln_no);
  if(!pl) return;
  pl->SetWarning();
}

void Program::CssBreakpoint(int src_ln_no, int bpno, int pc, const String& prognm,
                            const String& topnm, const String& src_ln) {
  global_trace = RenderGlobalTrace(taMisc::gui_active); // gotta grab it while its hot
  String fh;
  fh << "Program: " << name << " Stopped on breakpoint number: " << bpno
  << " at css source line: " << src_ln_no << " in prog: "
  << prognm << " pc: " << pc << " css top: " << topnm << "\n"
  << src_ln;
  if(taMisc::gui_active) {
    iDialogChoice::ConfirmDialog(NULL, fh);
    ViewProgEditor(src_ln_no);
  }
}

void Program::taError(int src_ln_no, bool running, const String& err_msg) {
  ProgLine* pl = script_list.SafeEl(src_ln_no);
  if(!pl) return;
  pl->SetError();
}

void Program::taWarning(int src_ln_no, bool running, const String& err_msg) {
  ProgLine* pl = script_list.SafeEl(src_ln_no);
  if(!pl) return;
  pl->SetWarning();
}

void Program::ScriptCompiled() {
  AbstractScriptBase::ScriptCompiled();
  script_list.ClearAllErrors(); // start fresh -- check if this doesn't work for actual errs..
  script_compiled = true;
  ret_val = 0;
  m_stale = false;
  SigEmit(SLS_ITEM_UPDATED_ND); // this will *not* call setDirty
}

void Program::setStale() {
  //note: we don't propagate setStale
  //note: 2nd recursive call of this during itself doesn't do anything
  if(run_state == RUN || run_state == INIT) return;          // change is likely self-generated during running, don't do it!
  bool changed = false;
  if (script_compiled) {
    // make sure this always reflects stale status -- is used as check for compiling..
    script_compiled = false;
    changed = true;
  }
  if (!m_stale) {  // prevent recursion and spurious inherited calls!!!!
    changed = true;
    m_stale = true;
    //note: actions in here will not recurse us, because m_stale is now set
    //     sub_prog_calls.RemoveAll(); // will need to re-enumerate
  }
  if (changed) { // user will need to recompile/INIT
    run_state = NOT_INIT;
    //obs    SigEmit(SLS_ITEM_UPDATED_ND); //note: doesn't recurse ud
    SigEmitUpdated(); //note: doesn't recurse ud
  }
}

bool Program::SetVar(const String& nm, const Variant& value) {
  ProgVar* var = FindVarName(nm);
  if(TestError(!var, "SetVar", "variable named:", nm, "not found!"))
    return false;
  var->SetVar(value);
  return true;
}

Variant Program::GetVar(const String& nm) {
  ProgVar* var = FindVarName(nm);
  if(TestError(!var, "GetVar", "variable named:", nm, "not found!"))
    return false;
  return var->GetVar();
}

bool Program::HasVar(const String& var_nm) {
  return (FindVarName(var_nm));
}

bool Program::SetVarFmArg(const String& arg_nm, const String& var_nm, bool quiet) {
  String arg_str = taMisc::FindArgByName(arg_nm);
  if(arg_str.empty()) return false; // no arg, no action
  bool rval = SetVar(var_nm, arg_str);
  if(TestError(!rval, "SetVarFmArg", "variable:",var_nm,
               "not found in program:", name)) return false;
  if(!quiet)
    taMisc::Info("Set", var_nm, "in program:", name, "to:", arg_str);
  return true;
}

ProgVar* Program::FindVarName(const String& var_nm) const {
  // note: this does NOT look in functions!
  ProgVar* sv = args.FindName(var_nm);
  if(sv) return sv;
  sv = vars.FindName(var_nm);
  if(sv) return sv;
  return prog_code.FindVarName(var_nm);
}

void Program::AddVarTo(taNBase* src) {
  vars.AddVarTo(src);
}

void Program::AddArgTo(taNBase* src) {
  args.AddVarTo(src);
}

taBase* Program::FindTypeName(const String& nm) const {
  return types.FindTypeName(nm);
}

Program* Program::FindProgramName(const String& prog_nm, bool warn_not_found) const {
  Program* rval = NULL;
  if(owner && owner->InheritsFrom(&TA_Program_Group)) {
    Program_Group* pg = (Program_Group*)owner;
    rval = pg->FindName(prog_nm);
    if(!rval) {
      taProject* proj = GET_MY_OWNER(taProject);
      if(proj) {
        rval = proj->programs.FindLeafName(prog_nm);
      }
    }
  }
  if(warn_not_found && !rval) {
    taMisc::Warning("program", name, "is looking for a program named:",
                    prog_nm, "but it was not found! Probably there will be more specific errors when you try to Init the program");
  }
  return rval;
}

Program* Program::FindProgramNameContains(const String& prog_nm, bool warn_not_found) const {
  Program* rval = NULL;
  if(owner && owner->InheritsFrom(&TA_Program_Group)) {
    Program_Group* pg = (Program_Group*)owner;
    rval = pg->FindNameContains(prog_nm);
    if(!rval) {
      taProject* proj = GET_MY_OWNER(taProject);
      if(proj) {
        rval = proj->programs.FindLeafNameContains(prog_nm);
      }
    }
  }
  if(warn_not_found && !rval) {
    taMisc::Warning("program", name, "is looking for a program containing:",
                    prog_nm, "but it was not found! Probably there will be more specific errors when you try to Init the program");
  }
  return rval;
}

ProgramCallBase* Program::FindSubProgTarget(Program* prg) {
  for(int i=0;i<sub_prog_calls.size;i++) {
    ProgramCallBase* pc = (ProgramCallBase*)sub_prog_calls.FastEl(i);
    if(pc->CallsProgram(prg)) {
      return pc;
    }
  }
  return NULL;
}

bool Program::AddLine(taBase* prog_el, const String& code, int pline_flags,
                      const String& desc) {
  String desc_oneline = desc;
  desc_oneline.gsub('\n', ' '); // no multiline in desc comments
  String rmdr = code;
  if(rmdr.contains('\n')) {
    String curln;
    do {
      String curln = rmdr.before('\n');
      rmdr = rmdr.after('\n');
      script_list.AddLine(prog_el, cur_indent, curln, pline_flags);
    } while(rmdr.contains('\n'));
  }
  if(rmdr.nonempty()) {
    if(desc_oneline.nonempty())
      rmdr += "   // " + desc_oneline;
    script_list.AddLine(prog_el, cur_indent, rmdr, pline_flags);
  }
  return true;
}

bool Program::AddVerboseLine(ProgEl* prog_el, bool insert_at_start, const String& msg_code) {
  if(!prog_el->IsVerbose()) return false;
  int start_ln, end_ln;
  ScriptLinesEl(prog_el, start_ln, end_ln);
  if(TestError(start_ln < 1, "AddVerboseLine", "programmer error -- must come after AddLine of at least some program elements"))
    return false;
  int main_line = script_list.FindMainLine(prog_el);
  if(TestError(main_line < 1, "AddVerboseLine", "programmer error -- must come after AddLine of main_line"))
    return false;
  int lno = main_line;
  if(insert_at_start) lno++;    // we're going to bump it..
  String code = String("Program::VerboseOut(") + GetPath() + ", " + String(lno);
  if(msg_code.nonempty())
    code += ", " + msg_code;
  code += ");";
  bool rval = AddLine(prog_el, code, ProgLine::PROG_DEBUG);
  if(insert_at_start) {
    int toln = start_ln;
    while(script_list.FastEl(toln)->IsComment())
      toln++;                   // come in after the comments, to make it look nicer
    script_list.MoveIdx(script_list.size-1, toln);
  }
  return rval;
}

void Program::VerboseOut(Program* prg, int code_line,
                         const char* a, const char* b, const char* c,
                         const char* d, const char* e, const char* f,
                         const char* g, const char* h, const char* i) {
  if(!prg) return;
  String msg;
  taProject* proj = GET_OWNER(prg, taProject);
  if(proj)
    msg = prg->GetPathNames(NULL, &(proj->programs));
  else
    msg = prg->GetPathNames();
  msg += ": \t";
  ProgLine* pl = prg->script_list.SafeEl(code_line);
  if(pl) {
    if((bool)pl->prog_el && pl->prog_el->InheritsFrom(&TA_ProgEl)) {
      ProgEl* pel = (ProgEl*)pl->prog_el.ptr();
      msg += pel->GetToolbarName() + " " + pel->GetTypeDef()->name + " :\t";
    }
    msg += pl->CodeLineNo() + "\n\t"; // start next line indented
  }
  if(a)
    msg += a;
  taMisc::Info(msg, b, c, d, e, f, g, h, i);
}

const String Program::GetDescString(const String& dsc, int indent_level) {
  String rval;
  if(!dsc.empty()) {
    // we support multi-lines by using the multi-line form of comments
    if (dsc.contains('\n')) {
      rval.cat(cssMisc::IndentLines("/* " + dsc + " */\n", indent_level));
    } else {
      rval.cat(cssMisc::Indent(indent_level)).cat("// ").cat(dsc).cat("\n");
    }
  }
  return rval;
}

void Program::AddDescString(taBase* prog_el, const String& dsc) {
  if(dsc.empty()) return;
  String rmdr = dsc;
  if(rmdr.contains('\n')) {
    String curln;
    do {
      String curln = rmdr.before('\n');
      rmdr = rmdr.after('\n');
      AddLine(prog_el, "// " + curln, ProgLine::COMMENT);
    } while(rmdr.contains('\n'));
  }
  if(rmdr.nonempty()) {
    AddLine(prog_el, "// " + rmdr, ProgLine::COMMENT);
  }
}

String Program::GetProgCodeInfo(int line_no, const String& code_str) {
  //   return String("info on line: ") + String(line_no) + " str: " + code_str;
  ProgVar* pv = FindVarName(code_str);
  if(pv && !pv->HasVarFlag(ProgVar::LOCAL_VAR)) {
    if(pv->var_type == ProgVar::T_Object && pv->object_val) {
      return pv->object_val->PrintStr();
    }
    else {
      return pv->GetDisplayName();
    }
  }
  if(script) {
    cssElPtr cssptr = script->ParseName(code_str);
    if((bool)cssptr) {
      return String(code_str) + ": " + cssptr.El()->PrintStr();
    }
  }
  taBase* tv = FindTypeName(code_str);
  if(tv) {
    return tv->GetDisplayName();
  }
  return _nilString;
}

void Program::ClearAllBreakpoints() {
  if(!script)
    return;
  script_list.ClearAllBreakpoints();
  script->DelAllBreaks();
  brk_pts.Reset();
}

// TODO - add brk_pt class code
void Program::SetAllBreakpoints() {
  if(!script) return;
  int nbp = 0;
  script->DelAllBreaks();       // start with clean slate
  ProgEl* last_pel_set = NULL;
  for(int i=1; i<script_list.size; i++) {
    ProgLine* pl = script_list.FastEl(i);
    if(pl->HasPLineFlag(ProgLine::COMMENT)) continue; // never mark comments..
    ProgEl* pel = NULL;
    if(pl->prog_el && pl->prog_el->InheritsFrom(&TA_ProgEl))
      pel = (ProgEl*)pl->prog_el.ptr();
    if(pl->HasPLineFlag(ProgLine::BREAKPOINT) ||
       (pel && pel != last_pel_set && pel->HasProgFlag(ProgEl::BREAKPOINT_ENABLED))) {
      last_pel_set = pel;                     // don't repeat
      pl->SetPLineFlag(ProgLine::BREAKPOINT); // make sure
      script->SetBreak(i);    // set it
      nbp++;
    }
  }
}

bool Program::ToggleBreakpoint(ProgEl* pel) {
  int start_ln, end_ln;
  if(!ScriptLinesEl(pel, start_ln, end_ln))
    return false;
  ProgLine* pl = script_list.FastEl(start_ln);
  if(pel->HasProgFlag(ProgEl::BREAKPOINT_ENABLED)) {
    pl->ClearBreakpoint();	// calls impl below
  }
  else {
    pl->SetBreakpoint();	// calls impl below
  }
  return true;
}

void Program::EnableBreakpoint(ProgEl* pel) {
  int start_ln, end_ln;
  if(!ScriptLinesEl(pel, start_ln, end_ln))
    return;
  ProgLine* pl = script_list.FastEl(start_ln);
  CmdShell();                 // should be using cmd shell if setting breakpoints
  script->SetBreak(start_ln);
  ProgBrkPt* bp = brk_pts.FindBrkPt(pel);
  if (bp) {
    bp->enabled = true;
    SigEmit(SLS_ITEM_UPDATED_ND);
  }
}

void Program::DisableBreakpoint(ProgEl* pel) {
  int start_ln, end_ln;
  if(!ScriptLinesEl(pel, start_ln, end_ln))
    return;
  script->DelBreak(start_ln);
  ProgBrkPt* bp = brk_pts.FindBrkPt(pel);
  if (bp) {
    bp->enabled = false;
    SigEmit(SLS_ITEM_UPDATED_ND);
  }
}

void Program::SetBreakpoint_impl(ProgEl* pel) {
  int start_ln, end_ln;
  if(!ScriptLinesEl(pel, start_ln, end_ln))
    return;
  ProgLine* pl = script_list.FastEl(start_ln);
  ProgBrkPt* bp = brk_pts.AddBrkPt(pel, pl->code);  // add a brk_pt object to the list of breakpoints - used for display/enable/disable gui
  EnableBreakpoint(pel);
}

void Program::ClearBreakpoint_impl(ProgEl* pel) {
  DisableBreakpoint(pel);
  brk_pts.DeleteBrkPt(pel);
}

bool Program::ScriptLinesEl(taBase* pel, int& start_ln, int& end_ln) {
  end_ln = -1;
  start_ln = script_list.FindProgEl(pel, false); // NOT go in reverse
  if(start_ln < 1 || !script) {
    Compile();
    start_ln = script_list.FindProgEl(pel, false); // NOT go in reverse
    if(start_ln < 1 || !script) // check again after compile
      return false;
  }
  // search forward to find last line with this guy
  end_ln = start_ln;
  taBase* lprog;
  do {
    if(++end_ln >= script_list.size) break;
    lprog = script_list.FastEl(end_ln)->prog_el;
  } while (lprog == pel);
  end_ln--; // overshot by one
  return true;
}

void Program::GetSubProgsAll(int depth) {
  if(TestError((depth >= 100), "GetSubProgsAll",
               "Probable recursion in programs detected -- maximum depth of 100 reached -- aborting"))
    return;
  sub_progs_updtd = true;
  sub_progs_all.Reset();
  for(int i=0;i<sub_prog_calls.size; i++) {
    ProgramCallBase* sp = (ProgramCallBase*)sub_prog_calls.FastEl(i);
    sp->AddTargetsToListAll(sub_progs_all);
  }
  int init_sz = sub_progs_all.size;
  for(int i=0;i<init_sz; i++) {
    Program* sp = sub_progs_all[i];
    sp->GetSubProgsAll(depth+1);        // no loops please!!!
    // now get our sub-progs sub-progs..
    for(int j=0;j<sp->sub_progs_all.size;j++) {
      Program* ssp = sp->sub_progs_all[j];
      sub_progs_all.LinkUnique(ssp);
    }
  }
}

void Program::GetSubProgsStep() {
  // strategy here is to just go through the sub_progs_all list and get all the ones with step
  // flags on -- this is the only really reliable way to do this in terms of picking up
  // deep step guys even through ProgramCallVar guys
  sub_progs_step.Reset();
  if(HasProgFlag(SELF_STEP)) {
    sub_progs_step.Link(this);
  }
  for(int i=0;i<sub_progs_all.size; i++) {
    Program* sp = (Program*)sub_progs_all.FastEl(i);
    if(!sp->HasProgFlag(Program::NO_STOP_STEP))
      sub_progs_step.Link(sp);  // guaranteed to be unique already
  }
}

const String Program::scriptString() {
  // enumerate all the progels, esp. to get subprocs registered
  // note: this is regenerated every time because stale is not always right
  // and code may depend on the state of external objects that can be updated
  // outside of the stale mechanism.  When the user presses Init, they get the
  // current fresh code regardless!  note that it doesn't do this obligatory
  // recompiles all the time -- that is only done at init too.
  script_list.Reset();
  AddLine(this, "// blank 0 line to align with css", ProgLine::COMMENT);
  cur_indent = 0;
  sub_prog_calls.Reset();
  sub_progs_dir.Reset();
  ClearProgFlag(SELF_STEP); // this is set by StopStepPoint..
  int item_id = 0;
  functions.PreGen(item_id);
  init_code.PreGen(item_id);
  prog_code.PreGen(item_id);
  
  // now, build the new script code
  AddLine(this, String("// ") + GetName(), ProgLine::COMMENT);
  AddLine(this, "", ProgLine::COMMENT);
  AddLine(this, "/* globals added to hardvars:", ProgLine::COMMENT);
  AddLine(this, "Program::RunState run_state; // our program's run state", ProgLine::COMMENT);
  AddLine(this, "int ret_val;", ProgLine::COMMENT);
  if(args.size > 0) {
    AddLine(this, "// args: global script parameters (arguments)", ProgLine::COMMENT);
    args.GenCss_ProgVars(this);
  }
  if(vars.size > 0) {
    AddLine(this, "// vars: global (non-parameter) variables", ProgLine::COMMENT);
    vars.GenCss_ProgVars(this);
  }
  AddLine(this, "*/", ProgLine::COMMENT);
  AddLine(this, "", ProgLine::COMMENT);
  
  // types
  if(types.size > 0) {
    types.GenCss(this);
  }
  
  // Functions
  functions.GenCss(this);
  
  // __Init() routine, for our own els, and calls to subprog Init()
  AddLine(this, "void __Init() {");
  IncIndent();
  // first, make sure any sub-progs are compiled
  if(sub_prog_calls.size > 0) {
    AddLine(this, "// First compile any subprogs that could be called from this one",
            ProgLine::COMMENT);
    AddLine(this, "{ Program* target;");
    IncIndent();
    // note: this is a list of ProgramCall's, not the actual prog itself!
    for (int i = 0; i < sub_prog_calls.size; ++i) {
      ProgramCallBase* sp = (ProgramCallBase*)sub_prog_calls.FastEl(i);
      if(sp->WillGenCompileScript(this)) {
        AddLine(this, "if(ret_val != Program::RV_OK) return; // checks previous");
        sp->GenCompileScript(this);
      }
    }
    DecIndent();
    AddLine(this, "}");
  }
  AddLine(this, "// init_from vars", ProgLine::COMMENT);
  args.GenCssInitFrom(this);
  vars.GenCssInitFrom(this);
  AddLine(this, "// run our init code", ProgLine::COMMENT);
  init_code.GenCss(this);
  
  if(sub_prog_calls.size > 0) {
    if(init_code.size > 0)
      AddLine(this, "");
    AddLine(this, "// Then call init on any subprogs that could be called from this one",
            ProgLine::COMMENT);
    AddLine(this, "{ Program* target;");
    IncIndent();
    // note: this is a list of ProgramCall's, not the actual prog itself!
    for(int i = 0; i < sub_prog_calls.size; ++i) {
      ProgramCallBase* sp = (ProgramCallBase*)sub_prog_calls.FastEl(i);
      sp->GenCallInit(this);
    }
    DecIndent();
    AddLine(this, "}");
  }
  DecIndent();
  AddLine(this, "}");
  AddLine(this, "");
  
  AddLine(this, "void __Prog() {");
  IncIndent();
  AddLine(this, "// init_from vars", ProgLine::COMMENT);
  args.GenCssInitFrom(this);
  vars.GenCssInitFrom(this);
  AddLine(this, "");
  AddLine(this, "// prog_code", ProgLine::COMMENT);
  prog_code.GenCss(this);
  if(!(flags & NO_STOP_STEP)) {
    AddLine(this, "StopCheck(); // process pending events, including Stop and Step events");
  }
  DecIndent();
  AddLine(this, "}");
  AddLine(this, "");
  AddLine(this, "");
  
  AddLine(this, "ret_val = Program::RV_OK; // set elsewise on failure");
  AddLine(this, "if (run_state == Program::INIT) {");
  IncIndent();
  AddLine(this, "__Init();");
  DecIndent();
  AddLine(this, "} else {");
  IncIndent();
  AddLine(this, "__Prog();");
  DecIndent();
  AddLine(this, "}");
  
  TestWarning(cur_indent != 0, "scriptString",
              "programmer error -- current indentation at end of script generation is != 0");
  
  m_scriptCache.truncate(0);
  script_list.FullListing(m_scriptCache);
  ViewScriptUpdate();
  
  m_stale = false;
  return m_scriptCache;
}

const String Program::ProgramListing() {
  m_listingCache = "// ";
  m_listingCache += GetName();
  
  if (types.size > 0) {
    m_listingCache += "\n// types: new types defined for this program\n";
    m_listingCache += types.GenListing(0);
  }
  
  if (args.size > 0) {
    m_listingCache += "\n// args: global script parameters (arguments)\n";
    m_listingCache += args.GenListing(0);
  }
  if (vars.size > 0) {
    m_listingCache += "\n// vars: global (non-parameter) variables\n";
    m_listingCache += vars.GenListing(0);
  }
  
  if(functions.size > 0) {
    m_listingCache += "\n// functions: functions defined for this program\n";
    m_listingCache += functions.GenListing(0);
  }
  
  if(init_code.size > 0) {
    m_listingCache += "\n// init_code: code to initialize the program\n";
    m_listingCache += init_code.GenListing(0); // ok if empty, returns nothing
  }
  
  if(prog_code.size > 0) {
    m_listingCache += "\n// prog_code: main code to run program\n";
    m_listingCache += prog_code.GenListing(0);
  }
  return m_listingCache;
}

void  Program::UpdateProgVars() {
  // note: this assumes that script has been ClearAll'd
  script->prog_vars.Reset(); // removes/unref-deletes
  
  // add the ones in the object -- note, we use *pointers* to these
  // these are already installed by the InstallThis routine!!
  //   cssEl* el = NULL;
  //   el = new cssCPtr_enum(&run_state, 1, "run_state",
  //                      TA_Program.sub_types.FindName("RunState"));
  //   script->prog_vars.Push(el);
  //   el = new cssCPtr_int(&ret_val, 1, "ret_val");
  //   script->prog_vars.Push(el);
  //   el = new cssTA_Base(&objs, 1, objs.GetTypeDef(), "objs");
  //   script->prog_vars.Push(el);
  
  // add new in the program
  for (int i = 0; i < args.size; ++i) {
    ProgVar* sv = args.FastEl(i);
    cssEl* el = sv->NewCssEl();
    script->prog_vars.Push(el);
    sv->css_idx = script->prog_vars.size-1; // record location
  }
  for (int i = 0; i < vars.size; ++i) {
    ProgVar* sv = vars.FastEl(i);
    cssEl* el = sv->NewCssEl();
    script->prog_vars.Push(el); //refs
    sv->css_idx = script->prog_vars.size-1; // record location
  }
}

String Program::GetProgLibPath(ProgLibs library) {
  if(library == SEARCH_LIBS) {
    taMisc::Error("Cannot do SEARCH_LIBS for saving -- program saved in local directory!");
    return "";
  }
  String path = "./";
  if(library == USER_LIB)
    path = taMisc::prog_lib_paths.GetVal("UserLib").toString();
  else if(library == SYSTEM_LIB) {
    if(taMisc::in_dev_exe) {
      String top_lev_path = taMisc::GetDirFmPath(taMisc::exe_path, 1); // go up 1
      path = top_lev_path + "/prog_lib";
    }
    else {
      path = taMisc::prog_lib_paths.GetVal("SystemLib").toString();
    }
  }
  else if(library == WEB_LIB)
    path = taMisc::prog_lib_paths.GetVal("WebLib").toString();
  if(library != WEB_LIB) {
    QFileInfo qfi(path);
    if(!qfi.isDir()) {
      QDir qd;
      qd.mkpath(path);          // attempt to make it..
      taMisc::Warning("Note: did mkdir for program library directory:", path);
    }
  }
  return path;
}

void Program::SaveToProgLib(ProgLibs library) {
  String path = GetProgLibPath(library);
  String fname = path + "/" + name + ".prog";
  QFileInfo qfi(fname);
  if(qfi.isFile()) {
    int chs = taMisc::Choice("Program library file: " + fname + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return;
  }
  SaveAs(fname);
  Program_Group::prog_lib.FindPrograms();
}

Variant Program::GetGuiArgVal(const String& fun_name, int arg_idx) {
  if(fun_name != "LoadFromProgLib") return inherited::GetGuiArgVal(fun_name, arg_idx);
  if(!prog_lib) return  _nilVariant;
  //  return _nilVariant;                            // return nil anyway!
  ProgLibEl* pel = prog_lib->FindName(name); // find our name
  return Variant(pel);
}

int Program::GetSpecialState() const {
  if(HasProgFlag(TRACE)) return 1; // ?
  if(HasProgFlag(LOCKED)) return 4; // red
  if(HasProgFlag(STARTUP_RUN)) return 3; // green
  if(HasProgFlag(NO_STOP_STEP)) return 2; // may not want this one -- might be too much color..
  return 0;
}

void Program::LoadFromProgLib(ProgLibEl* prog_type) {
  if(TestError(!prog_type, "LoadFromProgLib", "program type is null")) return;
  if(TestError(prog_type->is_group, "LoadFromProgLib",
               "cannot load a program group file into a single program!")) return;
  //   Reset();
  prog_type->LoadProgram(this);
}

void Program::SigEmit(int sls, void* op1, void* op2) {
  // just for debug trapping..
  inherited::SigEmit(sls, op1, op2);
  sub_progs_updtd = false;
}

void Program::RunLoadInitCode() {
  // automatically do the program call guys!
  int item_id = 0;
  functions.PreGen(item_id);
  init_code.PreGen(item_id);
  prog_code.PreGen(item_id);
  for (int i = 0; i < sub_prog_calls.size; ++i) {
    ProgramCallBase* sp = (ProgramCallBase*)sub_prog_calls.FastEl(i);
    sp->LoadInitTarget();       // just call this directly!
  }
}

void Program::SaveScript(ostream& strm) {
  strm << scriptString();
}

void Program::GlobalTrace() {
  if(taMisc::gui_active) {
    TypeDef* td = &TA_Program;
    MemberDef* md = td->members.FindName("global_trace");
    taiEditorOfString* host_ = new taiEditorOfString(md, this, td, true, false, NULL, false, true);
    // args are: read_only, modal, parent, line_nos, rich_text
    host_->Constr("Global Trace of Programs Called to last Stop");
    host_->Edit(false);
  }
  else {
    taMisc::ConsoleOutput(global_trace, false, true); // no err, pager = true
  }
}

void Program::LocalTrace() {
  if(taMisc::gui_active) {
    local_trace = RenderLocalTrace(true);
    TypeDef* td = GetTypeDef();
    MemberDef* md = td->members.FindName("local_trace");
    taiEditorOfString* host_ = new taiEditorOfString(md, this, td, true, false, NULL, true, false);
    // args are: read_only, modal, parent, line_nos, rich_text
    host_->Constr("Local Trace of Program: " + name);
    host_->Edit(false);
  }
  else {
    local_trace = RenderLocalTrace(false);
    taMisc::ConsoleOutput(local_trace, false, true); // no err, pager = true
  }
}

String Program::RenderGlobalTrace(bool html) {
  String rval;
  if(html) {
    rval << "<table border=0 cellpadding=2> <tr><th>Level</th><th>Program</th><th>Code</th></tr>\n";
  }
  int cnt = 0;
  for(int i = cssMisc::top_stack.stack_size-1; i >= 0; i--, cnt++) {
    cssProgSpace* sp = cssMisc::top_stack.stack[i];
    int ln = sp->CurRunSrcLn();
    if(html) rval << "<tr><td>";
    rval << cnt << "\t";
    if(html) rval << "</td>";
    if(sp->own_program) {
      taProject* proj = GET_OWNER(sp->own_program, taProject);
      if(html) {
        rval << "<td><a href=\"ta:" << sp->own_program->GetPath(NULL, proj)
        << "#progln_" << ln << "\">"
        << sp->own_program->name << "</a></td>";
      }
      else {
        rval << sp->own_program->name;
      }
    }
    else {
      rval << sp->name;
    }
    if(html) rval << "<td>";
    rval << "\t\t" << sp->CurFullRunSrc() << "\n";
    if(html)
      rval << "</td></tr>\n";
  }
  if(html) rval << "</table>";
  return rval;
}

String Program::RenderLocalTrace(bool html) {
  String rval;
  if(script) {
    script->BackTrace(rval);
  }
  return rval;
}

#ifdef TA_GUI
void Program::ViewScript() {
  ViewScript_impl();
}

// defined in ta_program_qt.cpp
// bool Program::ViewScriptEl(taBase* pel) {
// }

void Program::ViewScript_Editor() {
  String fnm = name + "_view.css";
  fstream strm;
  strm.open(fnm, ios::out);
  SaveScript(strm);
  strm.close();
  
  taMisc::EditFile(fnm);
}

void Program::ViewScriptUpdate() {
  view_script.truncate(0);
  if(script_list.size <= 1)
    view_script = "// Program must be Compiled (e.g., hit Init button) before css Script is available.<P>\n";
  else
    script_list.FullListingHTML(view_script);
}

void Program::ViewScript_impl(int sel_ln_st, int sel_ln_ed) {
  ViewScriptUpdate();
  TypeDef* td = GetTypeDef();
  MemberDef* md = td->members.FindName("view_script");
  taiEditorOfString* host_ = new taiEditorOfString(md, this, td, true, false, NULL, true, true);
  // args are: read_only, modal, parent, line_nos, rich_text
  host_->Constr("Css Script for program: " + name);
  host_->Edit(false);
  if(sel_ln_st > 0)
    host_->SelectLines(sel_ln_st, sel_ln_ed);
}

void Program::EditProgram() {
  this->FindMyProgramPanel();
}

#endif  // TA_GUI

void Program::SaveListing(ostream& strm) {
  strm << ProgramListing();
}

#ifdef TA_GUI
void Program::ViewListing() {
  taiEditorOfString* host_ = NULL;
  view_listing = ProgramListing();
  if(!host_) {
    TypeDef* td = GetTypeDef();
    MemberDef* md = td->members.FindName("view_listing");
    host_ = new taiEditorOfString(md, this, td, true, false, NULL, true);
    // args are: read_only, modal, parent, line_nos
    host_->Constr("Listing of program elements for program: " + name);
    host_->Edit(false);
  }
}

void Program::ViewListing_Editor() {
  String fnm = name + "_list.css";
  fstream strm;
  strm.open(fnm, ios::out);
  SaveListing(strm);
  strm.close();
  
  taMisc::EditFile(fnm);
}

#endif  // TA_GUI

void Program::InitForbiddenNames() {
  if(forbidden_names.size > 0) return;
  taBase::Ref(forbidden_names);	// otherwise it gets nuked improperly on shutdown, from install-this
  forbidden_names.Add("run_state");
  forbidden_names.Add("ret_val");
  forbidden_names.Add("this");
  
  TypeDef* type_def = &TA_Program;
  for(int i=0; i<type_def->members.size; i++) {
    MemberDef* md = type_def->members.FastEl(i);
    forbidden_names.Add(md->name);
  }
  for(int i=0; i<type_def->methods.size; i++) {
    MethodDef* md = type_def->methods.FastEl(i);
    forbidden_names.Add(md->name);
  }
  for(int i=0; i<cssMisc::Parse.size; i++) {
    cssEl* el = cssMisc::Parse.FastEl(i);
    forbidden_names.Add(el->name);
  }
  for(int i=0; i<cssMisc::Commands.size; i++) {
    cssEl* el = cssMisc::Commands.FastEl(i);
    forbidden_names.Add(el->name);
  }
  for(int i=0; i<cssMisc::Functions.size; i++) {
    cssEl* el = cssMisc::Functions.FastEl(i);
    forbidden_names.Add(el->name);
  }
  for(int i=0; i<cssMisc::Constants.size; i++) {
    cssEl* el = cssMisc::Constants.FastEl(i);
    forbidden_names.Add(el->name);
  }
  for(int i=0; i<cssMisc::Enums.size; i++) {
    cssEl* el = cssMisc::Enums.FastEl(i);
    forbidden_names.Add(el->name);
  }
  // taMisc::Info(forbidden_names.PrintStr());
}

bool Program::IsForbiddenName(const String& chk_nm, bool warn) {
  if(taMisc::is_loading) return false; // don't check for loading -- just causes disaster when renaming during loading
  if(forbidden_names.size == 0)
    InitForbiddenNames();
  if((forbidden_names.FindEl(chk_nm) < 0) &&
     !(bool)TypeDef::FindGlobalTypeName(chk_nm,false)) return false;
  if(!warn) return true;
  taMisc::Error("Program::IsForbiddenName -- Name:", chk_nm,
                "is a css reserved name used for something else -- please choose another name");
  return true;
}

void Program::MakeTemplate_fmtype(Program* prog, TypeDef* td) {
  taBase* tok = (taBase*)td->GetInstance();
  if(tok) {
    taBase* o = tok->MakeToken();
    o->SetName("New" + td->name);
    prog->init_code.Add(o);
  }
  for(int i=0;i<td->children.size;i++) {
    TypeDef* chld = td->children[i];
    MakeTemplate_fmtype(prog, chld);
  }
}

Program* Program::MakeTemplate() {
  //TODO: this will probably get nuked and replaced with a generic maker on .root
  Program* prog = new Program;
  {ProgVar* o = new ProgVar; o->SetName("NewProgVar"); prog->vars.Add(o);}
  //note: prog args go into a ProgramCall etc., so we just add the tmpl to the objects
  {ProgArg* o = new ProgArg; o->SetName("NewProgArg"); prog->objs.Add(o);}
  //note: put in .init since that will get searched first
  
  MakeTemplate_fmtype(prog, &TA_ProgEl);
  return prog;
}

bool Program::AddCtrlFunsToControlPanel(ControlPanel* ctrl_panel, const String& extra_label,
                                    const String& sub_gp_nm) {
  if(!ctrl_panel) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "AddCtrlFunsToControlPanel", "cannot find project")) return false;
    ctrl_panel = (ControlPanel*)proj->ctrl_panels.New(1);
  }
  TypeDef* td = GetTypeDef();
  bool rval = true;
  MethodDef* md = td->methods.FindName("Init"); // "" = desc field
  if(md) rval = ctrl_panel->SelectMethod(this, md, extra_label, "", sub_gp_nm);
  md = td->methods.FindName("Run_Gui");
  if(md) rval |= ctrl_panel->SelectMethod(this, md, extra_label, "", sub_gp_nm);
  md = td->methods.FindName("Step_Gui");
  if(md) rval |= ctrl_panel->SelectMethod(this, md, extra_label, "", sub_gp_nm);
  md = td->methods.FindName("Stop");
  if(md) rval |= ctrl_panel->SelectMethod(this, md, extra_label, "", sub_gp_nm);
  return rval;
}

#ifndef TA_GUI
// see iPanelOfProgram.cpp
iPanelOfProgram* Program::FindMyProgramPanel() {
  return NULL;
}
bool Program::BrowserSelectMe_ProgItem(taOBase* itm) {
  return false;
}
bool Program::BrowserExpandAll_ProgItem(taOBase* itm) {
  return false;
}
bool Program::BrowserCollapseAll_ProgItem(taOBase* itm) {
  return false;
}
#endif

///////////////////////////////////////////////////////////////////////
//      Program specific browser guys!


iPanelSet* Program::FindMyPanelSet() {
  if(!taMisc::gui_active) return NULL;
  taSigLink* link = sig_link();
  if(!link) return NULL;
  taSigLinkItr itr;
  iPanelSet* el;
  FOR_DLC_EL_OF_TYPE(iPanelSet, el, link, itr) {
    //     if (el->data() == this) {
    return el;
    //     }
  }
  return NULL;
}

bool Program::ViewCtrlPanel() {
  iPanelSet* dps = FindMyPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(0);
  return true;
}

bool Program::ViewProgEditor(int src_ln_no) {
  iPanelSet* dps = FindMyPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(1);
  if(src_ln_no >= 0) {
    ProgLine* pl = script_list.SafeEl(src_ln_no);
    if(pl && pl->prog_el) {
      tabMisc::DelayedFunCall_gui(pl->prog_el, "BrowserSelectMe");
    }
  }
  return true;
}

bool Program::ViewCssScript(int src_ln_no) {
  iPanelSet* dps = FindMyPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(2);
  if(src_ln_no >= 0) {
    iPanelOfProgramScript* pnl =
    dynamic_cast<iPanelOfProgramScript*>(dps->panels.SafeEl(2));
    if(pnl && pnl->vs) {
      pnl->vs->setHighlightLines(src_ln_no, 1);
    }
  }
  return true;
}

bool Program::ViewProperties() {
  iPanelSet* dps = FindMyPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(3);
  return true;
}

bool Program::ViewScriptEl(taBase* pel) {
  iPanelSet* dps = FindMyPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(2);
  iPanelOfProgramScript* pnl = dynamic_cast<iPanelOfProgramScript*>(dps->panels.SafeEl(2));
  if(!pnl || !pnl->vs) return false;
  int start_ln, end_ln;
  if(!ScriptLinesEl(pel, start_ln, end_ln))
    return false;
  
  pnl->vs->setHighlightLines(start_ln, (end_ln - start_ln)+1);
  return true;
}

bool Program::EditProgramEl(taBase* pel) {
  return this->BrowserSelectMe_ProgItem(dynamic_cast<taOBase*>(pel));
}

