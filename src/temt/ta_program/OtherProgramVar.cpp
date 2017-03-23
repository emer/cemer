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
#include <taProject>
#include <NameVar_PArray>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(OtherProgramVar);


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
          "in program:", other_prog->name, "path:", other_prog->DisplayPath());
    }
    if(var_2) {
      pv = other_prog->FindVarName(var_2->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_2->name,
          "in program:", other_prog->name, "path:", other_prog->DisplayPath());
    }
    if(var_3) {
      pv = other_prog->FindVarName(var_3->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_3->name,
          "in program:", other_prog->name, "path:", other_prog->DisplayPath());
    }
    if(var_4) {
      pv = other_prog->FindVarName(var_4->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_4->name,
          "in program:", other_prog->name, "path:", other_prog->DisplayPath());
    }
  }
}

String OtherProgramVar::GetDisplayName() const {
  String rval;
  if(set_other)
    rval = "Vars To: ";
  else
    rval = "Vars Fm: ";

  if(other_prog)
    rval += " program=" + other_prog->name;
  else
    rval += " program=? ";

  if(set_other)
     rval += " set=to ";
   else
     rval += " set=from ";

  if(var_1)
    rval += " var_1=" + var_1->name + " ";
  else
    rval += " var_1=? ";

  if(var_2)
    rval += " var_2=" + var_2->name + " ";
  else
    rval += " var_2=? ";

  if(var_3)
    rval += " var_3=" + var_3->name + " ";
  else
    rval += " var_3=? ";

  if(var_4)
    rval += " var_4=" + var_4->name + " ";
  else
    rval += " var_4=? ";

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

bool OtherProgramVar::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if (CvtFmCodeCheckNames(code))
    return true;
  
  String dc = code;  dc.downcase();
  if(dc.startsWith("vars to:") || dc.startsWith("vars fm:") || dc.startsWith("var to:") || dc.startsWith("var fm:"))
    return true;
  return false;
}

bool OtherProgramVar::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true; // nothing we can do

  bool set_oth = false;
  if(dc.startsWith("vars to:") || dc.startsWith("var to:"))
    set_other = true;
  else
    set_other = false;

  String remainder = trim(code.after(":"));
  if(remainder.empty()) return true;

  if(!remainder.contains('=')) {
    // shortcut to just get the program
    taProject* proj = GetMyProj();
    if(proj) {
      Program* np = proj->programs.FindLeafName(remainder);
      if(np) {
        other_prog = np;
      }
    }
    return true;
  }

  NameVar_PArray nv_pairs;
  taMisc::ToNameValuePairs(remainder, nv_pairs);

  for (int i=0; i<nv_pairs.size; i++) {
    String name = nv_pairs.FastEl(i).name;
    name.downcase();
    String value = nv_pairs.FastEl(i).value.toString();

    if (name.startsWith("prog")) {
      taProject* proj = GetMyProj();
      if(proj) {
        Program* np = proj->programs.FindLeafName(value);
        if(np) {
          other_prog = np;
        }
      }
    }
    else if (name == "var_1") {
      var_1 = FindVarNameInScope(value, false); // don't make
    }
    else if (name =="var_2") {
      var_2 = FindVarNameInScope(value, false); // don't make
    }
    else if (name == "var_3") {
      var_3 = FindVarNameInScope(value, false); // don't make
    }
    else if (name == "var_4") {
      var_4 = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("set")) {
      if (value == "to" || value == "true")
        set_other = true;
      else if (value == "from" || value == "false")
        set_other = false;
      else
        set_other = false;
    }
  }
  SigEmitUpdated();
  return true;
}

//bool OtherProgramVar::CvtFmCode(const String& code) {
//  String dc = code;  dc.downcase();
//  String tbn = GetToolbarName(); tbn.downcase();
//  String tn = GetTypeDef()->name; tn.downcase();
//  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true; // nothing we can do
//
//
//  bool set_oth = false;
//  if(dc.startsWith("vars to:")) set_other = true;
//  else                          set_other = false;
//  String dtnm = code.after(": ");
//  if(dtnm.empty()) return true;
//  String rest = dtnm.after("Vars: ");
//  dtnm = dtnm.before(" ");
//  if(!other_prog || other_prog->name != dtnm) {
//    taProject* proj = GetMyProj();
//    if(proj) {
//      Program* np = proj->programs.FindLeafName(dtnm);
//      if(np) {
//        other_prog = np;
//      }
//    }
//  }
//  if(rest.empty()) return true;
//
//  String_Array vrs;
//  vrs.FmDelimString(rest, " ");
//  if(vrs.size > 0) {
//    var_1 = FindVarNameInScope(vrs[0], false); // don't make
//  }
//  if(vrs.size > 1) {
//    var_2 = FindVarNameInScope(vrs[1], false); // don't make
//  }
//  if(vrs.size > 2) {
//    var_3 = FindVarNameInScope(vrs[2], false); // don't make
//  }
//  if(vrs.size > 3) {
//    var_4 = FindVarNameInScope(vrs[3], false); // don't make
//  }
//  SigEmitUpdated();
//  return true;
//}

