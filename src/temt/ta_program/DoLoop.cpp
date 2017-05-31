// Copyright 2017, Regents of the University of Colorado,
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

#include "DoLoop.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DoLoop);


void DoLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(test.expr.empty(), quiet, rval, "test expression is empty");
}

void DoLoop::GenCssPre_impl(Program* prog) {
  prog->AddLine(this, "do {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before entering loop\""); // move to start
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"starting in loop\""); // don't move to out
}

void DoLoop::GenCssPost_impl(Program* prog) {
  test.ParseExpr();             // re-parse just to be sure!
  prog->DecIndent();
  prog->AddLine(this, String("} while (") + test.GetFullExpr() + ");");
}

String DoLoop::GetDisplayName() const {
  return "do ... while (" + test.GetFullExpr() + ")";
}

bool DoLoop::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
  if(code.startsWith("do(") || code.startsWith("do (") ||
     code.startsWith("do while"))
    return true;
  return false;
}

bool DoLoop::CvtFmCode(const String& code) {
  String cd;
  if (code.startsWith("do while"))
    cd = trim(code.after("do while"));
  else
    cd = trim(code.after("do"));
              
  if(cd.contains('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  test.SetExpr(cd);
  return true;
}
