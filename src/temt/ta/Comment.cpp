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
  return desc;
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

