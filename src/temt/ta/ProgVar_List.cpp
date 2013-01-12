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

#include "ProgVar_List.h"
#include <Program>
#include <ProgVar>
#include <DataTable>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>


void ProgVar_List::Initialize() {
  SetBaseType(&TA_ProgVar);
  var_context = VC_ProgVars;
  setUseStale(true);
}

void ProgVar_List::Copy_(const ProgVar_List& cp) {
  var_context = cp.var_context;

  // this is not needed: each individual guy will do it
  //  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Program);
}

void ProgVar_List::El_SetIndex_(void* it_, int idx) {
  ProgVar* it = (ProgVar*)it_;
  if (it->name.empty()) {
    it->name = "Var_" + (String)idx;
  }
}

void ProgVar_List::AddVarTo(taNBase* src) {
  if (!src) return;
  // if already exists, just ignore
  for (int i = 0; i < size; ++i) {
    ProgVar* it = FastEl(i);
    if ((it->var_type == ProgVar::T_Object) &&
      (it->object_val.ptr() == src))
      return;
  }
  ProgVar* it = (ProgVar*)New(1);
  it->SetObject(src);
  it->SetName(src->GetName());
  it->UpdateAfterEdit();
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(it, "BrowserSelectMe");
  }
}

void ProgVar_List::CreateDataColVars(DataTable* src) {
  if (!src) return;

  for(int i=0;i<src->data.size; i++) {
    DataCol* da = src->data[i];
    if(da->is_matrix || da->name.empty()) continue;
    ProgVar* it = FindName(da->name);
    if(!it)
      it = (ProgVar*)New(1);
    it->SetName(da->name);
    if(da->isString()) {
      it->var_type = ProgVar::T_String;
    }
    else if(da->valType() == VT_FLOAT || da->valType() == VT_DOUBLE) {
      it->var_type = ProgVar::T_Real;
    }
    else {
      it->var_type = ProgVar::T_Int;
    }
    it->UpdateAfterEdit();
  }
}

const String ProgVar_List::GenCss_FuncArgs() const {
  if(var_context != VC_FuncArgs) return _nilString;
  String rval;
  int cnt = 0;
  for (int i = 0; i < size; ++i) {
    ProgVar* it = FastEl(i);
    if (cnt > 0)
      rval += ", ";
    rval += it->GenCss(true);
    ++cnt;
  }
  return rval;
}

void ProgVar_List::GenCss_ProgVars(Program* prog) const {
  if(var_context != VC_ProgVars) return;
  for (int i = 0; i < size; ++i) {
    ProgVar* it = FastEl(i);
    prog->AddLine(it, it->GenCss(false));
  }
}

void ProgVar_List::GenCssInitFrom(Program* prog) const {
  for(int i=0;i<size;i++) {
    ProgVar* var = FastEl(i);
    var->GenCssInitFrom(prog);
  }
}

const String ProgVar_List::GenListing(int indent_level) const {
  String rval(0, 40 * size, '\0'); // buffer with typical-ish room
  int cnt = 0;
  for (int i = 0; i < size; ++i) {
    ProgVar* it = FastEl(i);
    bool is_arg = (var_context == VC_FuncArgs);
    if (is_arg) {
      if (cnt > 0)
        rval += ", ";
    }
    rval += it->GenListing(is_arg, indent_level);
    ++cnt;
  }
  return rval;
}

ProgVar* ProgVar_List::FindVarType(int varttmp, TypeDef* td) {
  ProgVar::VarType vart = (ProgVar::VarType)varttmp;
  for (int i = 0; i < size; ++i) {
    ProgVar* it = FastEl(i);
    if(it->var_type == vart) {
      if((vart == ProgVar::T_Object) && (td != NULL)) {
        if(it->object_type == td) {
          return it;
        }
      }
      else if((vart == ProgVar::T_HardEnum) && (td != NULL)) {
        if(it->hard_enum_type == td) {
          return it;
        }
      }
      else {
        return it;
      }
    }
  }
  return NULL;
}

void ProgVar_List::setStale() {
  inherited::setStale();
  // note: there are no vars just in program groups anymore..
  // if we are in a program group, dirty all progs
  // note: we have to test if in a prog first, otherwise we'll always get a group
//   Program* prog = GET_MY_OWNER(Program);
//   if (!prog) {
//     Program_Group* grp = GET_MY_OWNER(Program_Group);
//     if (grp)
//       grp->SetProgsStale();
//   }
}

bool ProgVar_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgVar_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgVar_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}
