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

#include "StaticMethodCall.h"
#include <Program>
#include <taMisc>
#include <MethodDef>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(StaticMethodCall);


void StaticMethodCall::Initialize() {
  method = NULL;
  min_type = &TA_taBase;
  object_type = &TA_taBase;
}

void StaticMethodCall::UpdateAfterEdit_impl() {
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

void StaticMethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!method, quiet, rval, "method is NULL");
}

void StaticMethodCall::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  meth_args.CheckConfig(quiet, rval);
}

void StaticMethodCall::GenCssBody_impl(Program* prog) {
  if (!method) {
    prog->AddLine(this, "//WARNING: StaticMethodCall not generated here -- object or method not specified", true);
    return;
  }

  String rval;
  if (result_var)
    rval += result_var->name + " = ";
  rval += object_type->name;
  rval += "::";
  rval += method->name;
  rval += meth_args.GenCssArgs();
  rval += ";";
  prog->AddLine(this, rval, ProgLine::MAIN_LINE);

  prog->AddVerboseLine(this);
}

String StaticMethodCall::GetDisplayName() const {
  String rval;

  if (!method) {
    rval += object_type->name + "::? - Select a method";
    return rval;
  }

  if(result_var)
    rval += result_var->name + "=";
  rval += object_type->name;
  rval += "::";
  rval += method->name;
  rval += "(";
  for(int i=0;i<meth_args.size;i++) {
    ProgArg* pa = meth_args[i];
    if (i > 0)
      rval += ", ";
    rval += pa->expr.expr;
  }
  rval += ")";
  return rval;
}

bool StaticMethodCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains("::")) return false;
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm = mthobj.before("::");
  TypeDef* td = TypeDef::FindGlobalTypeName(objnm, false);
  if(!td) return false; // todo: maybe trigger an err here??
  // don't compete with subclasses
  if(objnm != "taMisc" && !objnm.contains("taMath") && objnm != "Random"
     && objnm != "taDataGen" && objnm != "taDataAnal" && objnm != "taDataProc"
     && objnm != "taImageProc")
    return true;
  return false;
}

bool StaticMethodCall::CvtFmCode(const String& code) {
  String lhs = trim(code.before('('));
  String mthobj = lhs;
  String rval;
  if(lhs.contains('=')) {
    mthobj = trim(lhs.after('='));
    rval = trim(lhs.before('='));
  }
  String objnm = mthobj.before("::");
  String methnm = mthobj.after("::");
  object_type = TypeDef::FindGlobalTypeName(objnm, false);
  if(rval.nonempty())
    result_var = FindVarNameInScope(rval, true); // true = give option to make one
  UpdateAfterEdit_impl();                          // update based on obj
  MethodDef* md = object_type->methods.FindName(methnm);
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
