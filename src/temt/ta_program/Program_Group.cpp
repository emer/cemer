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

#include <Program>
#include "Program_Group.h"

#include <taMisc>
#include <tabMisc>

#include <QFileInfo>

TA_BASEFUNS_CTORS_DEFN(Program_Group);
SMARTREF_OF_CPP(Program_Group); // Program_GroupRef



using namespace std;


void Program_Group::Initialize() {
  SetBaseType(&TA_Program);
  debug_mode = false;
}

void Program_Group::InitLinks() {
  inherited::InitLinks();
  taBase::Own(step_prog, this);
  if(prog_lib.not_init) {
    taBase::Ref(prog_lib);
    prog_lib.FindPrograms();
  }
}

void Program_Group::CutLinks() {
  inherited::CutLinks();
}

void Program_Group::Copy_(const Program_Group& cp) {
  desc = cp.desc;
}

void Program_Group::SaveToProgLib(ProgLibs library) {
  String path = Program::GetProgLibPath((Program::ProgLibs)library);
  String fname = path + "/" + name + ".progp";
  QFileInfo qfi(fname);
  if(qfi.isFile()) {
    int chs = taMisc::Choice("Program library file: " + fname + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return;
  }
  SaveAs(fname);
  Program_Group::prog_lib.FindPrograms();
}

void Program_Group::SetProgsStale() {
  //obs: WARNING: this will cause us to also receive setStale for each prog call
  FOREACH_ELEM_IN_GROUP(Program, prog, *this) {
    prog->setStale();
  }
}

ProgLib Program_Group::prog_lib;

taBase* Program_Group::AddFromProgLib(ProgLibEl* prog_type) {
  return prog_lib.NewProgram(prog_type, this);
}

taBase* Program_Group::AddFromProgLibByName(const String& prog_nm) {
  return prog_lib.NewProgramFmName(prog_nm, this);
}

Variant Program_Group::GetGuiArgVal(const String& fun_name, int arg_idx) {
  if(fun_name != "LoadFromProgLib") return inherited::GetGuiArgVal(fun_name, arg_idx);
  ProgLibEl* pel = prog_lib.FindName(name); // find our name
  return Variant(pel);
}

void Program_Group::UpdateFromProgLib(ProgLibEl* prog_type) {
  if(TestError(!prog_type, "LoadFromProgLib", "program type to load is null")) return;
  if(TestError(!prog_type->is_group, "LoadFromProgLib",
               "cannot load a single program file into a program group!")) return;
//   FOREACH_ELEM_IN_GROUP(Program, prog, *this) {
//     prog->Reset();
//   }
  prog_type->LoadProgramGroup(this);
}

void Program_Group::ToggleTrace() {
  FOREACH_ELEM_IN_GROUP(Program, prog, *this) {
    prog->ToggleTrace();
  }
}

void Program_Group::ClearAllBreakpoints() {
  FOREACH_ELEM_IN_GROUP(Program, prog, *this) {
    prog->ClearAllBreakpoints();
  }
}

bool Program_Group::RunStartupProgs() {
  bool any_run = false;
  FOREACH_ELEM_IN_GROUP(Program, prog, *this) {
    if(!prog->HasProgFlag(Program::STARTUP_RUN)) continue;
    taMisc::Info("Running startup program:", prog->name);
    prog->Init();
    if((prog->ret_val == Program::RV_OK) && (prog->run_state == Program::DONE)) {
      prog->Run();
      any_run = true;
    }
  }
  return any_run;
}

void Program_Group::BrowserSelectFirstEl() {
  if(!taMisc::gui_active) return;
  if(leaves == 0) return;
  Program* first_guy = Leaf(0);
  tabMisc::DelayedFunCall_gui(first_guy, "BrowserSelectMe");
}

void Program_Group::RestorePanels() {
  FOREACH_ELEM_IN_GROUP(Program, prog, *this) {
    if(prog->GetUserDataAsBool("user_pinned")) {
      prog->EditPanel(true, true);
    }
  }
}


bool Program_Group::BrowserEditTest() {
  bool any_errs = false;
  FOREACH_ELEM_IN_GROUP(Program, prog, *this) {
    bool err = prog->BrowserEditTest();
    if(err) {
      any_errs = true;
    }
  }
  return any_errs;
}


bool Program_Group::InDebugMode() {
  if (debug_mode) {
    return true;
  }
  else {
    Program_Group* prg_grp = GET_MY_OWNER(Program_Group);
    if (prg_grp) {
      return prg_grp->InDebugMode();
    }
    else {
      return false;
    }
  }
}

void Program_Group::ToggleDebug() {
  debug_mode = !debug_mode;
  UpdateAfterEdit();
}

String Program_Group::GetStateDecoKey() const {
  String rval = inherited::GetStateDecoKey();
  if(rval.empty()) {
    if(debug_mode)
      rval = "debug_mode";
  }
  return rval;
}

