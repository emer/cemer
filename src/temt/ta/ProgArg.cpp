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

#include "ProgArg.h"
#include <Program>


void ProgArg::Initialize() {
  arg_type = NULL;
  required = true; // generally true
  setUseStale(true); // always requires recompile
}

void ProgArg::Destroy() {
  CutLinks();
}

void ProgArg::InitLinks() {
  inherited::InitLinks();
  taBase::Own(expr, this);
}

void ProgArg::CutLinks() {
  arg_type = NULL;
  expr.CutLinks();
  inherited::CutLinks();
}

void ProgArg::Copy_(const ProgArg& cp) {
  type = cp.type;
  name = cp.name;
  required = cp.required;
  def_val = cp.def_val;
  expr = cp.expr;
  arg_type = cp.arg_type;
}

void ProgArg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError((required && expr.empty()), quiet, rval,
    "An expression is required for this argument -- enter <no_arg> as the expression to explicitly not pass an argument for calling a Program (does not work for method or function calls)");
}

void ProgArg::SetVarAsExpr(ProgVar* prog_var) {
  if(!prog_var) return;
  expr.expr = prog_var->name;
  expr.UpdateAfterEdit();
}

bool ProgArg::UpdateFromVar(const ProgVar& cp) {
  bool any_changes = false;
  String ntyp = cp.GenCssType();
  if(type != ntyp) {
    any_changes = true;
    type = ntyp;
  }
  if(cp.var_type == ProgVar::T_Int) {
    if(arg_type != &TA_int) {
      arg_type = &TA_int;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_Real) {
    if(arg_type != &TA_double) {
      arg_type = &TA_double;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_String) {
    if(arg_type != &TA_taString) {
      arg_type = &TA_taString;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_Bool) {
    if(arg_type != &TA_bool) {
      arg_type = &TA_bool;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_Object) {
    if(arg_type != cp.object_type) {
      arg_type = cp.object_type;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_HardEnum) {
    if(arg_type != cp.hard_enum_type) {
      arg_type = cp.hard_enum_type;
      any_changes = true;
    }
  }
  else if(cp.var_type == ProgVar::T_DynEnum) {
    if(arg_type != &TA_DynEnum) {
      arg_type = &TA_DynEnum;
      any_changes = true;
    }
  }
  return any_changes;
}

bool ProgArg::UpdateFromType(TypeDef* td) {
  bool any_changes = false;
  if(arg_type != td) {
    arg_type = td;
    any_changes = true;
  }
  if(arg_type) {
    String ntyp = arg_type->Get_C_Name();
    if(type != ntyp) {
      type = ntyp;
      any_changes = true;
    }
  }
  return any_changes;
}

String ProgArg::GetDisplayName() const {
  return type + " " + name + "=" + expr.GetFullExpr();
//  return expr.expr;
}

bool ProgArg::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgArg::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgArg::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

