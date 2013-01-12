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

#include "OtherProgramVar.h"
#include <Program>
#include <ProgVar>
#include <taMisc>


void OtherProgramVar::Initialize() {
  set_other = false;
}

void OtherProgramVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!other_prog, quiet, rval, "other_prog is NULL");
  if(other_prog) {
    ProgVar* pv = NULL;
    if(var_1) {
      pv = other_prog->FindVarName(var_1->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_1->name,
                 "in program:", other_prog->name, "path:", other_prog->GetPathNames());
    }
    if(var_2) {
      pv = other_prog->FindVarName(var_2->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_2->name,
                 "in program:", other_prog->name, "path:", other_prog->GetPathNames());
    }
    if(var_3) {
      pv = other_prog->FindVarName(var_3->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_3->name,
                 "in program:", other_prog->name, "path:", other_prog->GetPathNames());
    }
    if(var_4) {
      pv = other_prog->FindVarName(var_4->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_4->name,
                 "in program:", other_prog->name, "path:", other_prog->GetPathNames());
    }
  }
}

String OtherProgramVar::GetDisplayName() const {
  String rval;
  if(set_other)
    rval = "To: ";
  else
    rval = "Fm: ";
  if(other_prog)
    rval += other_prog->name;
  else
    rval += "(ERROR: other_prog not set!)";
  if(set_other)
    rval += " Fm Vars: ";
  else
    rval += " To Vars: ";
  if(var_1) rval += var_1->name + " ";
  if(var_2) rval += var_2->name + " ";
  if(var_3) rval += var_3->name + " ";
  if(var_4) rval += var_4->name + " ";
  return rval;
}

Program* OtherProgramVar::GetOtherProg() {
  TestError(!other_prog, "GetOtherProg", "Other program is NULL in OtherProgramVar:",
            desc, "in program:", program()->name);
  return other_prog.ptr();
}

bool OtherProgramVar::GenCss_OneVar(Program* prog, ProgVarRef& var, int var_no) {
  if(!var) return false;
  if(set_other) {
    prog->AddVerboseLine(this, false, "\"setting other prog's variable named: "+var->name +
                         " to value:\", String(" + var->name + ")");
    prog->AddLine(this, String("other_prog->SetVar(\"") + var->name + "\", " + var->name +");");
  }
  else {
    prog->AddVerboseLine(this, false, "\"setting my variable named: "+var->name +
                         " current value:\", String(" + var->name + ")");
    prog->AddLine(this, var->name + " = other_prog->GetVar(\"" + var->name + "\");");
    prog->AddVerboseLine(this, false, "\"new value:\", String(" + var->name + ")");
  }
  return true;
}

void OtherProgramVar::GenCssPre_impl(Program* prog) {
  String rval = "{ // other program var: ";
  if (other_prog)
    rval += other_prog->name;
  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
}

void OtherProgramVar::GenCssBody_impl(Program* prog) {
  if (!other_prog) return;
  prog->AddLine(this, String("Program* other_prog = this") + GetPath(NULL, program())
                + "->GetOtherProg();");
  GenCss_OneVar(prog, var_1, 0);
  GenCss_OneVar(prog, var_2, 1);
  GenCss_OneVar(prog, var_3, 2);
  GenCss_OneVar(prog, var_4, 3);
}

void OtherProgramVar::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "} // other program var");
}
