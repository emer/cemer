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

#include "MemberAssign.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(MemberAssign);


void MemberAssign::Initialize() {
  update_after = false;
}

void MemberAssign::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if((bool)obj && (bool)obj->object_val && expr.empty()) { // assume ok to set default here based on obj
    update_after = obj->object_val->UAEProgramDefault();
  }
}


void MemberAssign::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

void MemberAssign::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();             // re-parse just to be sure!
  if (!(bool)obj || path.empty() || expr.empty()) {
    prog->AddLine(this, "// WARNING: MemberAssign not generated here -- obj or path not specified or expr empty", ProgLine::MAIN_LINE);
    return;
  }

  String path_term = path;
  String path_pre = path;
  if(path_term.contains('.')) {
    path_term = path_term.after('.', -1);
    path_pre = path.before('.', -1);
  }
  String opath;
  if(path_term != path) {
    if(path_pre.startsWith('['))
      opath = obj->name + path_pre;
    else
      opath = obj->name + "->" + path_pre;
  }
  else {
    opath = obj->name;
  }
  opath = trim(opath);
  String fpath = opath + "->" + path_term;
  String rval;
  if(opath.endsWith(']')) {     // itr expression
    rval = "set(" + opath + ", \"" + path_term + "\", " + expr.GetFullExpr() + ");";
  }
  else {
    rval = fpath + " = " + expr.GetFullExpr() + ";";
  }
  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  if (update_after) {
    prog->AddLine(this, obj->name + "->UpdateAfterEdit();");
    if(path_term != path) {
        // also do uae on immediate owner!
      if(opath.endsWith(']')) { // itr expression -- need to use call
        prog->AddLine(this, "call(" + opath + ", \"UpdateAfterEdit\");");
      }
      else {
        prog->AddLine(this, opath + "->UpdateAfterEdit();");
      }
    }
  }
  prog->AddVerboseLine(this, true, "\"prev value:\", String(" + fpath + ")"); // moved above
  prog->AddVerboseLine(this, false, "\"new  value:\", String(" + fpath + ")"); // after
}

String MemberAssign::GetDisplayName() const {
  if (!obj || path.empty())
    return "(object or path not selected)";

  String rval;
  rval = obj->name;
  if(path.startsWith('['))
    rval += path;
  else
    rval += "->" + path;
  rval += " = ";
  rval += expr.GetFullExpr();
  return rval;
}

bool MemberAssign::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains('=')) return false;
  String lhs = code.before('=');
  if(!(lhs.contains('.') || lhs.contains("->"))) return false;
  return true;                  // probably enough?
}

bool MemberAssign::CvtFmCode(const String& code) {
  String lhs = trim(code.before('='));
  if(lhs.contains('(') || lhs.contains(' ')) return false; // exclude others
  String objnm;
  String pathnm;
  if(lhs.contains('.')) {
    objnm = lhs.before('.');
    pathnm = lhs.after('.');
  }
  else {
    objnm = lhs.before("->");
    pathnm = lhs.after("->");
  }
  if(objnm.contains('[')) {
    if(pathnm.nonempty())
      pathnm = objnm.from('[') + "." + pathnm;
    else
      pathnm = objnm.from('[');
    objnm = objnm.before('[');
  }
  ProgVar* pv = FindVarNameInScope(objnm, true); // true = give option to make one
  if(!pv) return false;
  obj = pv;
  path = pathnm;
  String rhs = trim(code.after('='));
  if(rhs.endsWith(';')) rhs = rhs.before(';',-1);
  expr.SetExpr(rhs);
  UpdateAfterEdit_impl();
  return true;
}


