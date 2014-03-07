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

#include "Comment.h"
#include <Program>
#include <taMisc>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(Comment);

void Comment::Initialize() {
  static String _def_comment("TODO: Add your program comment here (multi-lines ok).");
  desc = _def_comment;
}

void Comment::GenCssBody_impl(Program* prog) {
  prog->AddLine(this, "/*******************************************************************",
                ProgLine::MAIN_LINE);
  prog->AddDescString(this, desc);
  prog->AddLine(this, "*******************************************************************/");
}

String Comment::GetDisplayName() const {
  return "// " + desc;
}

String Comment::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_disp_name) {
    String rval = GetDisplayName();
    return rval;
  }
  return inherited::GetColText(key, itm_idx);
}

bool Comment::BrowserEditSet(const String& code, int move_after) {
  edit_move_after = 0;
  String cd = code; // CodeGetDesc(code);
  if(CanCvtFmCode(cd, NULL)) {
    bool rval = CvtFmCode(cd);
    SigEmitUpdated();
    return rval;
  }
  orig_prog_code = cd;
  edit_move_after = move_after;
  tabMisc::DelayedFunCall_gui(this, "RevertToCode"); // do it later..
  return true;
}


bool Comment::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("//") || code.startsWith("/*")) return true;
  return false;
}

bool Comment::CvtFmCode(const String& code) {
  if(code.startsWith("//")) desc = trim(code.after("//"));
  else if(code.startsWith("/*")) trim(desc = code.after("/*"));
  if(code.endsWith("*/")) desc = trim(desc.before("*/",-1));
  return true;
}

