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

#include "MemberMethodCall.h"
#include <Program>
#include <taMisc>
#include <tabMisc>
#include <MethodDef>

TA_BASEFUNS_CTORS_DEFN(MemberMethodCall);


void MemberMethodCall::Initialize() {
  method = NULL;
}

void MemberMethodCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

//  if(!taMisc::is_loading && method)
  if (method) { // needed to set required etc.
    if(meth_args.UpdateFromMethod(method)) { // changed
      if(taMisc::gui_active) {
        tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
      }
    }
  }
}

void MemberMethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!method, quiet, rval, "method is NULL");
}

void MemberMethodCall::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  meth_args.CheckConfig(quiet, rval);
}

void MemberMethodCall::GenCssBody_impl(Program* prog) {
  if (!((bool)obj && method)) {
    prog->AddLine(this, "// WARNING: MemberMethodCall not generated here -- obj or method not specified");
    return;
  }

  String rval;
  if(result_var)
    rval += result_var->name + " = ";

  if(path.endsWith(']')) {    // is using list comprehension access of some sort
    // use call to iterate over elements on the list
    rval += "call(" + obj->name;
    if(path.startsWith('['))
      rval += path;
    else
      rval += "->" + path;
    rval += ", \"" + method->name + "\"";
    if(meth_args.size > 0) {
      String targs = meth_args.GenCssArgs();
      rval += ", " + targs.after('(');
    }
    else {
      rval += ")";
    }
    rval += ";";
  }
  else {
    rval += obj->name + "->" + path + "->";
    rval += method->name;
    rval += meth_args.GenCssArgs();
    rval += ";";
  }

  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

String MemberMethodCall::GetDisplayName() const {
  String rval;
  if(result_var)
    rval += result_var->name + "=";

  if(obj)
    rval += obj->name;
  else
    rval += "?";
  if(path.empty())
    rval += ".?";
  else if(path.startsWith('['))
    rval += path;
  else
    rval += "." + path;
  if(method)
    rval += "." + method->name + "(";
  else
    rval += ".?(";
  for(int i=0;i<meth_args.size;i++) {
    ProgArg* pa = meth_args[i];
    if (i > 0)
      rval += ", ";
    rval += pa->expr.expr;   // GetDisplayName();
  }
  rval += ")";
  return rval;
}

bool MemberMethodCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  int mbfreq = lhs.freq('.') + lhs.freq("->");
  if(mbfreq <= 1) {
    if(!(mbfreq == 1 && lhs.contains('[') && lhs.contains(']')))
      return false;
  }
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm;
  if(mthobj.contains('.'))
    objnm = mthobj.before('.');
  else
    objnm = mthobj.before("->");
  if(objnm.nonempty()) return true; // syntax above should be enough to rule in..
  return false;
}

bool MemberMethodCall::CvtFmCode(const String& code) {
  String lhs = trim(code.before('('));
  String mthobj = lhs;
  String rval;
  if(lhs.contains('=')) {
    mthobj = trim(lhs.after('='));
    rval = trim(lhs.before('='));
  }
  String objnm;
  String pathnm;
  if(mthobj.contains('.')) {
    objnm = mthobj.before('.');
    pathnm = mthobj.after('.');
  }
  else {
    objnm = mthobj.before("->");
    pathnm = mthobj.after("->");
  }
  if(objnm.contains('[')) {
    if(pathnm.nonempty())
      pathnm = objnm.from('[') + "." + pathnm;
    else
      pathnm = objnm.from('[');
    objnm = objnm.before('[');
  }
  if(objnm == "?") return false;
  ProgVar* pv = FindVarNameInScope(objnm, true); // true = give option to make one
  if(!pv) return false;
  obj = pv;
  if(rval.nonempty())
    result_var = FindVarNameInScope(rval, true); // true = give option to make one
  String methnm;
  if(pathnm.contains('.')) {
    methnm = pathnm.after('.',-1);
    pathnm = pathnm.before('.', -1);
  }
  else {
    methnm = pathnm.after("->",-1);
    pathnm = pathnm.before("->", -1);
  }
  path = pathnm;
  UpdateAfterEdit_impl();                          // update based on obj and path
  if(!obj_type) return false;
  MethodDef* md = obj_type->methods.FindName(methnm);
  if(md) {
    method = md;
    UpdateAfterEdit_impl();                        // update based on obj
  }
  // now tackle the args
  String args = trim(code.after('('));
  if(args.endsWith(')')) args = trim(args.before(')',-1));
  if(args.endsWith(';')) args = trim(args.before(';',-1));
  for(int i=0; i<meth_args.size; i++) {
    ProgArg* pa = meth_args.FastEl(i);
    String arg;
    if(args.contains(',')) {
      arg = trim(args.before(','));
      args = trim(args.after(','));
    }
    else {
      arg = args;
      args = "";                // all done
    }
    pa->expr.SetExpr(arg);
    if(args.empty()) break;
  }
  return true;
}
