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

#include "FunctionCall.h"
#include <Program>
#include <taMisc>

void FunctionCall::Initialize() {
}

void FunctionCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateArgs();         // always do this.. nondestructive and sometimes stuff changes anyway
}

void FunctionCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!fun, quiet, rval, "fun is NULL");
  if(fun) {
    Program* myprg = GET_MY_OWNER(Program);
    Program* otprg = GET_OWNER(fun.ptr(), Program);
    CheckError(myprg != otprg, quiet, rval,
               "function call to:",fun.ptr()->GetName(),"not in same program as me -- must be fixed!");
  }
}

void FunctionCall::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  fun_args.CheckConfig(quiet, rval);
}

void FunctionCall::GenCssBody_impl(Program* prog) {
  if (!fun) return;

  String rval;
  if(result_var) {
    rval += result_var->name + "=";
  }
  rval += fun->name + "(";
  for (int i = 0; i < fun_args.size; ++i) {
    ProgArg* ths_arg = fun_args.FastEl(i);
    if(i > 0) rval += ", ";
    ths_arg->expr.ParseExpr();          // re-parse just to be sure!
    rval += ths_arg->expr.GetFullExpr();
  }
  rval += ");";

  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

String FunctionCall::GetDisplayName() const {
  String rval = "FunCall ";
  if(result_var)
    rval += result_var->name + "=";
  if (fun) {
    rval += fun->name;
    rval += "(";
    for(int i=0;i<fun_args.size;i++) {
      ProgArg* pa = fun_args.FastEl(i);
      if(i > 0) rval += ", ";
      rval += pa->expr.expr;  // GetDisplayName();
    }
    rval += ")";
  }
  else
    rval += "(no function set)";
  return rval;
}

void FunctionCall::UpdateArgs() {
  if(!fun) return; // just leave existing stuff for now
  fun_args.UpdateFromVarList(fun->args);
}

bool FunctionCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String funm = lhs;
  if(lhs.contains('='))
    funm = trim(lhs.after('='));
  if((funm.freq('.') + funm.freq("->")) > 0) return false; // exclude method call
  if(!scope_el) return false;
  Program* prog = GET_OWNER(scope_el, Program);
  if(!prog) return false;
  Function* fun = prog->functions.FindName(funm);
  if(fun) return true;
  return false;
}

bool FunctionCall::CvtFmCode(const String& code) {
  String lhs = code.before('(');
  String funm = lhs;
  if(lhs.contains('='))
    funm = trim(lhs.after('='));
  Program* prog = GET_OWNER(this, Program);
  if(!prog) return false;
  Function* fn = prog->functions.FindName(funm);
  if(!fn) return false;
  fun = fn;
  UpdateAfterEdit_impl();                          // update based on fun
  // now tackle the args
  String args = trim(code.after('('));
  if(args.endsWith(')')) args = trim(args.before(')',-1));
  if(args.endsWith(';')) args = trim(args.before(';',-1));
  for(int i=0; i<fun_args.size; i++) {
    ProgArg* pa = fun_args.FastEl(i);
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
