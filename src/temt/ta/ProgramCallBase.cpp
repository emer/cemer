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

#include "ProgramCallBase.h"
#include <Program>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>


void ProgramCallBase::Initialize() {
}

void ProgramCallBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateArgs();         // always do this..  nondestructive and sometimes stuff changes anyway
  Program* prg = program();
  if(prg && !HasProgFlag(OFF)) {
    if(TestError(prg->init_code.IsParentOf(this), "UAE", "Cannot have a program call within init_code -- init_code should generally be very simple and robust code as it is not checked in advance of running (to prevent init-dependent Catch-22 scenarios) -- turning this call OFF"))
      SetProgFlag(OFF);

  }
}

void ProgramCallBase::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  prog_args.CheckConfig(quiet, rval);
  Program* prg = program();
  if(prg && !HasProgFlag(OFF)) {
    if(CheckError(prg->init_code.IsParentOf(this), quiet, rval,
                  "Cannot have a program call within init_code -- init_code should generally be very simple and robust code as it is not checked in advance of running (to prevent init-dependent Catch-22 scenarios) -- turning this call OFF")) {
      SetProgFlag(OFF);
    }
  }
}

void ProgramCallBase::GenCssArgSet_impl(Program* prog, const String trg_var_nm) {
  Program* trg = GetTarget_Compile();
  if(!trg) return;

  if (prog_args.size > 0) {
    prog->AddLine(this, "// set global vars of target", ProgLine::COMMENT);
  }
  String nm;
  bool set_one = false;
  for (int i = 0; i < prog_args.size; ++i) {
    ProgArg* ths_arg = prog_args.FastEl(i);
    nm = ths_arg->name;
    ProgVar* prg_var = trg->args.FindName(nm);
    ths_arg->expr.ParseExpr();          // re-parse just to be sure!
    String argval = ths_arg->expr.GetFullExpr();
    if (!prg_var || argval.empty() || argval == "<no_arg>") continue;
    set_one = true;
    prog->AddLine(this, trg_var_nm + "->SetVar(\"" + prg_var->name + "\", " + argval + ");");
  }
}

void ProgramCallBase::UpdateArgs() {
  Program* trg = GetTarget_Compile();
  if(!trg) return;

  bool any_changes = prog_args.UpdateFromVarList(trg->args);
  // now go through and set default value for variables of same name in this program
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return;
  for(int i=0;i<prog_args.size; i++) {
    ProgArg* pa = prog_args.FastEl(i);
    if(!pa->expr.expr.empty()) continue; // skip if already set
    ProgVar* arg_chk = prg->args.FindName(pa->name);
    ProgVar* var_chk = prg->vars.FindName(pa->name);
    if(!arg_chk && !var_chk) continue;
    pa->expr.SetExpr(pa->name); // we found var of same name; set as arg value
    pa->DataItemUpdated();
  }
  if(any_changes && taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
  }
}
