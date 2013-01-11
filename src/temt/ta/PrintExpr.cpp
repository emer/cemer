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

#include "PrintExpr.h"


void PrintExpr::Initialize() {
}

void PrintExpr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Program* my_prog = program();
  if(!my_prog) return;
  if(!debug_level) {
    debug_level = my_prog->vars.FindName("debug_level");
  }
}

void PrintExpr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

void PrintExpr::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();             // re-parse just to be sure!
  String rval = String("cout << ") + expr.GetFullExpr() + " << endl;";

  if(my_mask && debug_level) {
    prog->AddLine(this, "if(" + my_mask->name + " & " + debug_level->name + ") {");
    prog->IncIndent();
    prog->AddLine(this, rval, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
  else {
    prog->AddLine(this, rval, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
  }
}

String PrintExpr::GetDisplayName() const {
  String rval;
  rval += "Print: " + expr.GetFullExpr();
  if(debug_level && my_mask) {
    rval += " (dbg mask: " + my_mask->name + ")";
  }
  return rval;
}

bool PrintExpr::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!(code.startsWith("print ") || code.startsWith("cerr << ") || code.startsWith("cout << ")))
    return false;
  String exprstr;
  if(code.startsWith("print ")) exprstr = trim(code.after("print "));
  else if(code.startsWith("cerr << ")) exprstr = trim(code.after("cerr << "));
  else if(code.startsWith("cout << ")) exprstr = trim(code.after("cout << "));
  if(exprstr.freq('"') >= 2) return true; // not a var guy
  return false;
}

bool PrintExpr::CvtFmCode(const String& code) {
  String exprstr;
  if(code.startsWith("print ")) exprstr = trim(code.after("print "));
  else if(code.startsWith("cerr << ")) exprstr = trim(code.after("cerr << "));
  else if(code.startsWith("cout << ")) exprstr = trim(code.after("cout << "));
  expr.SetExpr(exprstr);
  return true;
}

