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

#include "ProgArg_List.h"

void ProgArg_List::Initialize() {
  SetBaseType(&TA_ProgArg);
  setUseStale(true);
}

void ProgArg_List::UpdateProgExpr_NewOwner() {
  for (int i = 0; i < size; ++ i) {
    ProgArg* pa = FastEl(i);
    pa->expr.UpdateProgExpr_NewOwner();
  }
}

void ProgArg_List::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  // make sure that once a def args is used, no more have values
  bool has_non_def = false;
  for (int i = size - 1; i >= 0; --i) {
    ProgArg* pa = FastEl(i);
    if (pa->required) break; // ok, no errors, and no more defs
    if (CheckError((has_non_def && pa->expr.empty()), quiet, rval,
      "Arg: " + pa->name + " cannot use default value because later args"
      " have a non-default value specified")) break;
    has_non_def = pa->expr.nonempty();
  }
}

const String ProgArg_List::GenCssArgs() {
  String rval = "(";
  for (int i = 0; i < size; ++ i) {
    ProgArg* pa = FastEl(i);
    //note: we test for violations of rules about def expressions, so
    // ok for us to just break here when we get to the first def
    if (!pa->required && pa->expr.empty()) break;
    pa->expr.ParseExpr();               // re-parse just to be sure!
    if (i > 0) rval += ", ";
    rval += pa->expr.GetFullExpr();
  }
  rval += ")";
  return rval;
}

bool ProgArg_List::UpdateFromVarList(ProgVar_List& targ) {
  bool any_changes = false;
  int i;  int ti;
  ProgArg* pa;
  ProgVar* pv;
  // delete args not in target; freshen those that are
  for (i = size - 1; i >= 0; --i) {
    pa = FastEl(i);
    pv = targ.FindName(pa->name);
    if(pv) {
      any_changes |= pa->UpdateFromVar(*pv);
    } else {
      RemoveIdx(i);
      any_changes = true;
    }
  }
  // add non-result args in target not in us, and put in the right order
  for (ti = 0; ti < targ.size; ++ti) {
    pv =targ.FastEl(ti);
    i = FindNameIdx(pv->name);
    if (i < 0) {
      pa = new ProgArg();
      pa->name = pv->name;
      pa->UpdateFromVar(*pv);
      Insert(pa, ti);
      any_changes = true;
    } else if (i != ti) {
      MoveIdx(i, ti);
      any_changes = true;
    }
  }
  return any_changes;
}

bool ProgArg_List::UpdateFromMethod(MethodDef* md) {
  //NOTE: safe to call during loading
  bool any_changes = false;
  int i;  int ti;
  ProgArg* pa;
  // delete args not in md list
  for (i = size - 1; i >= 0; --i) {
    pa = FastEl(i);
    int ti = md->arg_names.FindEl(pa->name);
    if (ti >= 0) {
      pa->UpdateFromType(md->arg_types[ti]);
    }
    else {
      RemoveIdx(i);
      any_changes = true;
    }
  }
  // add args in target not in us, and put in the right order
  for (ti = 0; ti < md->arg_names.size; ++ti) {
    TypeDef* arg_typ = md->arg_types.FastEl(ti);
    String arg_nm = md->arg_names[ti];
    i = FindNameIdx(arg_nm);
    if (i < 0) {
      pa = new ProgArg();
      pa->name = arg_nm;
      pa->UpdateFromType(arg_typ);
      Insert(pa, ti);
      any_changes = true;
      //pa->expr.SetExpr(def_val); // set to this expr
    }
    else {
      pa = FastEl(i);
      if (i != ti) {
        MoveIdx(i, ti);
        any_changes = true;
      }
    }
    // have to do default for all, since it is not saved
    if(md->fun_argd < 0)
      pa->required = true;
    else
      pa->required = (md->fun_argd > ti);
    if (!pa->required) { // do default processing
      // get default value if available
      String def_val = md->arg_defs.SafeEl(ti);
      if (def_val.nonempty() && arg_typ->IsEnum() && !def_val.contains("::")) {
        TypeDef* ot = arg_typ->GetOwnerType();
        if(ot)
          def_val = ot->name + "::" + def_val;
      }
      else if (arg_typ->InheritsFrom(TA_taString) ||
        ((arg_typ->ptr == 1) && arg_typ->InheritsFrom(&TA_char)))
      {
        if(def_val.empty()) def_val = "\"\""; // empty string
      }
      pa->def_val = def_val;
    }
    else {                      // required
      if(pa->arg_type->InheritsFormal(&TA_enum) && pa->expr.expr.empty()) {
        // pre-fill expr with lookup base for enum type -- makes lookup easier
        String eprfx = pa->arg_type->GetEnumPrefix();
        if(eprfx.nonempty()) {
          TypeDef* ot = pa->arg_type->GetOwnerType();
          if(ot) pa->expr.expr = ot->name + "::";
          pa->expr.expr += eprfx;
        }
      }
    }
  }
  return any_changes;
}

bool ProgArg_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgArg_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgArg_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}
