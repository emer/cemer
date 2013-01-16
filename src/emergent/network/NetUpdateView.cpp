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

#include "NetUpdateView.h"
#include <ProgVar>
#include <MemberDef>

#include <taMisc>

void NetUpdateView::Initialize() {
}

void NetUpdateView::Destroy() {
  CutLinks();
}

void NetUpdateView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetUpdateVar();
}

void NetUpdateView::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!network_var, quiet, rval, "network_var = NULL");
  CheckError(!update_var, quiet, rval, "update_var = NULL");
}

String NetUpdateView::GetDisplayName() const {
  return "Net Update View";
}

void NetUpdateView::GetUpdateVar() {
  if (update_var && (update_var->name == "update_net_view")) return;

  Program* my_prog = program();
  if (!my_prog) return;

  update_var = my_prog->vars.FindName("update_net_view");
  if (!update_var) {
    update_var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    update_var->name = "update_net_view";
    update_var->DataItemUpdated();
  }
  update_var->var_type = ProgVar::T_Bool;
}

void NetUpdateView::GenCssBody_impl(Program* prog) {
  if (!network_var) {
    prog->AddLine(this, "// ERROR: network_var not set in NetUpdateView", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "if(update_net_view || this.IsStepProg())"
                + network_var->name + "->UpdateAllViews();", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}


