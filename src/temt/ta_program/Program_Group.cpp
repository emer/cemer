// Copyright 2013-2017, Regents of the University of Colorado,
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

ProgLib Program_Group::prog_lib;

void Program_Group::Initialize() {
  SetBaseType(&TA_Program);
  debug_mode = false;
}

void Program_Group::InitLinks() {
  inherited::InitLinks();
}

void Program_Group::CutLinks() {
  inherited::CutLinks();
}

void Program_Group::Copy_(const Program_Group& cp) {
  desc = cp.desc;
}

void Program_Group::SetProgsStale() {
  //obs: WARNING: this will cause us to also receive setStale for each prog call
  FOREACH_ELEM_IN_GROUP(Program, prog, *this) {
    prog->setStale();
  }
}

void Program_Group::BuildProgLib() {
  prog_lib.BuildLibrary();
}

taBase* Program_Group::AddFromProgLib(ObjLibEl* prog_lib_item) {
  return prog_lib.NewProgram(this, prog_lib_item);
}

taBase* Program_Group::AddFromProgLibByName(const String& prog_nm) {
  return prog_lib.NewProgramFmName(this, prog_nm);
}

void Program_Group::BrowseProgLib(ProgLib::LibLocs location) {
  prog_lib.BrowseLibrary(location);
}

void Program_Group::UpdateFromProgLib(ObjLibEl* prog_lib_item) {
  if(TestError(!prog_lib_item, "UpdateFromProgLib", "program type to load is null")) return;
  prog_lib.UpdateProgramGroup(this, prog_lib_item);
}

Variant Program_Group::GetGuiArgVal(const String& fun_name, int arg_idx) {
  if(fun_name != "UpdateFromProgLib") return inherited::GetGuiArgVal(fun_name, arg_idx);
  ObjLibEl* pel = prog_lib.library.FindName(name); // find our name
  return Variant(pel);
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
  FOREACH_ELEM_IN_GROUP_NESTED(Program, prog, *this) {
    if(prog->GetUserDataAsBool("user_pinned")) {
      prog->EditPanel(true, true);
    }
    FOREACH_ELEM_IN_LIST(taBase, obj, (prog->objs)) {
      if(obj->GetUserDataAsBool("user_pinned")) {
        obj->EditPanel(true, true);
      }
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

void Program_Group::SetGuiUpdt(bool objs_updt_gui) {
  FOREACH_ELEM_IN_GROUP(Program, prog, *this) {
    prog->SetProgFlagState(Program::OBJS_UPDT_GUI, objs_updt_gui);
  }
}


String Program_Group::GetStateDecoKey() const {
  String rval = inherited::GetStateDecoKey();
  if(rval.empty()) {
    if(debug_mode)
      rval = "debug_mode";
  }
  return rval;
}

