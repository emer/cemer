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

#include "BlankLineEl.h"

TA_BASEFUNS_CTORS_DEFN(BlankLineEl);

#include <Program>
#include <taProject>
#include <tabMisc>

void BlankLineEl::Initialize() {
}

void BlankLineEl::GenCssBody_impl(Program* prog) {
  prog->AddLine(this, "// ", ProgLine::MAIN_LINE);
}

String BlankLineEl::GetDisplayName() const {
  return " ";
}

String BlankLineEl::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_disp_name) {
    String rval = GetDisplayName();
    return rval;
  }
  return inherited::GetColText(key, itm_idx);
}

bool BlankLineEl::BrowserEditSet(const String& code, int move_after) {
  if(move_after != -11) {
    Program* prog = GET_MY_OWNER(Program);
    if(prog) {
      taProject* proj = prog->GetMyProj();
      if(proj) {
        proj->undo_mgr.SaveUndo(this, "BrowserEditSet", prog);
      }
    }
  }
  edit_move_after = 0;
  String cd = code; // CodeGetDesc(code);
  if(CanCvtFmCode(cd, NULL)) {
    bool rval = CvtFmCode(cd);
    SigEmitUpdated();
    return rval;
  }
  code_string = cd;
  edit_move_after = move_after;
  tabMisc::DelayedFunCall_gui(this, "RevertToCode"); // do it later..
  return true;
}


bool BlankLineEl::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String cd = code;
  cd = trim(cd);
  if(cd.empty()) return true;
  return false;
}

bool BlankLineEl::CvtFmCode(const String& code) {
  return true;
}

