// Copyright 2013-2017, Regents of the University of Colorado,
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

#include <taiWidgetTokenChooser>

TA_BASEFUNS_CTORS_DEFN(FunctionCall);

void FunctionCall::Initialize() {
}

void FunctionCall::Destroy() {
  if (fun.ptr()) {
    fun->UpdateAfterEdit();
  }
}

void FunctionCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateArgs_impl(); // always do this.. nondestructive and sometimes stuff changes anyway
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

bool FunctionCall::GenCssBody_impl(Program* prog) {
  if (!fun) return false;

  String rval;
  if(result_var) {
    rval += result_var->name + " = ";
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
  return true;
}

String FunctionCall::GetDisplayName() const {
  String rval;
  if(result_var)
    rval = result_var->name + " = ";
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
  else {
    rval += "function_name()";
  }
  return rval;
}

bool FunctionCall::UpdateArgs_impl() {
  if(!fun) return false; // just leave existing stuff for now
  return fun_args.UpdateFromVarList(fun->args);
}

void FunctionCall::UpdateArgs() {
  bool any_changes = UpdateArgs_impl();
  if(any_changes) {
    SigEmitUpdated();
  }
}

bool FunctionCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if (code == GetDisplayName())
    return true;
  String code_dn = code; code_dn.downcase();
  // if assignment of return value from func you may not see funcall
  if(code_dn.startsWith("funcall ")) return true; // definitely
  if(code_dn.startsWith("func ")) return true; // definitely
  if(code_dn.startsWith("fun ")) return true; // definitely
  String lhs = code;
  String funm = lhs;
  if(code.contains('(')) {
    lhs = code.before('(');
  }
  if(lhs.contains('=')) {
    funm = trim(lhs.after('='));
  }
  else {
    funm = lhs;
  }
  
  if((funm.freq('.') + funm.freq("->")) > 0) return false; // exclude method call
  if(!scope_el) return false;
  Program* prog = GET_OWNER(scope_el, Program);
  if(!prog) return false;
  Function* fn = prog->functions.FindName(funm);
  if(fn) return true;
  return false;
}

bool FunctionCall::CvtFmCode(const String& code) {
  String cd = code;
  if(cd.startsWith("FunCall "))  // all of these are old style -- allow for now
    cd = cd.after("FunCall ");
  else if(cd.startsWith("funcall "))
    cd = cd.after("funcall ");
  else if(cd.startsWith("Func "))
    cd = cd.after("Func ");
  else if(cd.startsWith("func "))
    cd = cd.after("func ");
  else if(cd.startsWith("Fun "))
    cd = cd.after("Fun ");
  else if(cd.startsWith("fun "))
    cd = cd.after("fun ");
  String lhs = cd;
  if(lhs.contains('('))
    lhs = lhs.before('(');
  String funm = lhs;
  String var_name = "";
  if(funm.contains('=')) {
    funm = trim(lhs.after('='));
    var_name = lhs.before('=');
  }
  Program* prog = GET_OWNER(this, Program);
  if(!prog) return false;
  Function* fn = prog->functions.FindName(funm);
  if(!fn) return false;
  fun = fn;
  if (!var_name.empty()) {
    var_name = trim(var_name);
    ProgVar* pv = FindVarNameInScope(var_name, true);
    if (pv)
      result_var = pv;
  }
  UpdateAfterEdit_impl();                          // update based on fun
  // now tackle the args
  String args = trim(cd.after('('));
  args = trim(args.before(')', -1));
  fun_args.ParseArgString(args);
  
  // need to get the function to update ui when a FunctionCall is created
  fun->UpdateAfterEdit();
  return true;
}

bool FunctionCall::ChooseMe() {
  // pop a chooser for the user
  if (!fun) {
    taiWidgetTokenChooser* chooser =  new taiWidgetTokenChooser(&TA_Function, NULL, NULL, NULL, 0, "");
    chooser->SetTitleText("Choose the function to call");
    // scope functions to the containing program - not this program
    Program* scope_program = GET_MY_OWNER(Program);
    chooser->GetImageScoped(NULL, &TA_Function, scope_program, &TA_Program); // scope to this guy
    bool okc = chooser->OpenChooser();
    if(okc && chooser->token()) {
      Function* tok = (Function*)chooser->token();
      fun = tok;
      UpdateAfterEdit();
    }
    delete chooser;
  }
  return true;
}



