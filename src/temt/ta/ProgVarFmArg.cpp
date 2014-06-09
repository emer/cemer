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

#include "ProgVarFmArg.h"
#include <Program>
#include <taMisc>
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(ProgVarFmArg);


void ProgVarFmArg::Initialize() {
}

void ProgVarFmArg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(arg_name.empty())
    arg_name = var_name;
  if(var_name.empty())
    var_name = arg_name;
}

void ProgVarFmArg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!prog, quiet, rval, "prog is NULL");
  CheckError(var_name.empty(), quiet, rval, "var_name is empty");
  CheckError(arg_name.empty(), quiet, rval, "arg_name is empty");

  if((bool)prog && var_name.nonempty()) {
    ProgVar* pv = prog->FindVarName(var_name);
    CheckError(!pv, quiet, rval, "Could not find variable named:", var_name, "in program:",
               prog->name, "path:", prog->GetPathNames());
  }
}

String ProgVarFmArg::GetDisplayName() const {
  String rval;
  if (!var_name.empty())
    rval = "Set Var:" + var_name;
  else
    rval = "Set Var:?";
  
  if(prog)
    rval += " in Program:" + prog->name;
  else
    rval += " in Program:?";
  
  if (!arg_name.empty())
    rval += " from Arg:" + arg_name;
  else
    rval += " from Arg:?";
  return rval;
}

Program* ProgVarFmArg::GetOtherProg() {
  if(!prog) {
    taMisc::CheckError("Program is NULL in ProgVarFmArg:",
                       desc, "in program:", program()->name);
  }
  return prog.ptr();
}

void ProgVarFmArg::GenCssBody_impl(Program* prog) {
  if (!prog) return;
  prog->AddLine(this, String("{ // prog var fm arg: ") + prog->name, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, String("Program* other_prog = this") + GetPath(NULL, program())
                + "->GetOtherProg();");
  prog->AddLine(this, "other_prog->SetVarFmArg(\"" + arg_name + "\", \"" + var_name + "\");");
  prog->DecIndent();
  prog->AddLine(this, "} // prog var fm arg");
}

void ProgVarFmArg::GenRegArgs(Program* prog) {
  prog->AddLine(this, String("taMisc::AddEqualsArgName(\"") + arg_name + "\");");
  prog->AddLine(this, String("taMisc::AddArgNameDesc(\"") + arg_name
                             + "\", \"ProgVarFmArg: prog = " + (((bool)prog) ? prog->name : "NOT SET")
                             + " var_name = " + var_name + "\");");
}

bool ProgVarFmArg::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  String dn = GetDisplayName().downcase();
  dn = dn.before(':');
  if(code.startsWith(tbn) || code.startsWith(tn) || code.startsWith(dn))
    return true;
  return false;
}

bool ProgVarFmArg::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn))
    return true; // nothing we can do
  
  String remainder = trim(code.after(":"));  // use code from here to get proper casing
  if(remainder.empty())
    return true;
  
  String the_var = trim(remainder.before(' '));
  if (the_var != "?")
    var_name = the_var;
  
  remainder = trim(remainder.after("Program:"));
  String the_prog = trim(remainder.before(' '));
  if (the_prog != "?") {
    taProject* prj = GET_MY_OWNER(taProject);
    if (prj) {
       prog = prj->programs.FindLeafName(the_prog);
    }
  }
  
  remainder = trim(remainder.after("Arg:"));
  if (remainder != "?")
    arg_name = remainder;
  
  SigEmitUpdated();
  return true;
}
