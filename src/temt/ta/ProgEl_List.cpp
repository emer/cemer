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

#include "ProgEl_List.h"
#include <Program>
taTypeDef_Of(ProgCode);

#include <taMisc>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(ProgEl_List);

void ProgEl_List::Initialize() {
  SetBaseType(&TA_ProgEl);
  el_typ = &TA_ProgCode;
  setUseStale(true);
  el_to_repl = NULL;
  el_to_repl_idx = 0;
}

void ProgEl_List::Destroy() {
  Reset();
}

void ProgEl_List::Copy_(const ProgEl_List& cp) {
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Program);
}

void ProgEl_List::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  el_typ = &TA_ProgCode;
}

void ProgEl_List::GenCss(Program* prog) {
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    el->GenCss(prog);
  }
}

const String ProgEl_List::GenListing(int indent_level) {
  String rval;
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    rval += el->GenListing(indent_level);
  }
  return rval;
}

String ProgEl_List::GetColHeading(const KeyString& key) const {
  static String col0("El Type");
  static String col1("El Description");
  if (key == key_type)  return col0;
  else if (key == key_disp_name) return col1;
  else return inherited::GetColHeading(key);
}

const KeyString ProgEl_List::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_type;
  case 1: return key_disp_name;
  default: return _nilKeyString;
  }
}


void ProgEl_List::PreGen(int& item_id) {
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    el->PreGen(item_id);
  }
}

ProgVar* ProgEl_List::FindVarName(const String& var_nm) const {
  for (int i = 0; i < size; ++i) {
    ProgEl* el = FastEl(i);
    ProgVar* pv = el->FindVarName(var_nm);
    if(pv) return pv;
  }
  return NULL;
}

bool ProgEl_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgEl_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgEl_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

bool ProgEl_List::RemoveIdx(int idx) {
  ProgEl* pel = FastEl(idx);
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    prog->brk_pts.DeleteBrkPt(pel);
  }
  return inherited::RemoveIdx(idx);
}

void ProgEl_List::ReplaceLater(ProgEl* el, int idx, const String& fun_on_repl) {
  el_to_repl = el;
  el_to_repl_idx = idx;
  el_to_repl_fun = fun_on_repl;
  tabMisc::DelayedFunCall_gui(this, "DoReplaceLater");
}

void ProgEl_List::DoReplaceLater() {
  if(!el_to_repl) {
    taMisc::Warning("DoReplaceLater: nothing to replace!  Programmer error");
    return;
  }
  ReplaceIdx(el_to_repl_idx, el_to_repl);
  tabMisc::DelayedFunCall_gui(el_to_repl, "BrowserExpandAll");
  tabMisc::DelayedFunCall_gui(el_to_repl, "BrowserSelectMe");
  if(el_to_repl_fun.nonempty()) {
    tabMisc::DelayedFunCall_gui(el_to_repl, el_to_repl_fun); 
  }
  el_to_repl = NULL;
}

void ProgEl_List::MoveElseLater(ProgEl* el, int idx, const String& fun_on_repl) {
  el_to_repl = el;
  el_to_repl_idx = idx;
  el_to_repl_fun = fun_on_repl;
  tabMisc::DelayedFunCall_gui(this, "DoMoveElseLater");
}

void ProgEl_List::DoMoveElseLater() {
  if(!el_to_repl) {
    taMisc::Warning("DoMoveElseLater: nothing to replace!  Programmer error");
    return;
  }

  ProgEl_List* nwown = GET_OWNER(owner, ProgEl_List);
  if(nwown) {
    int nwmyidx = nwown->FindEl(owner);
    if(nwmyidx >= 0) {
      RemoveIdx(el_to_repl_idx); // nuke ProgCode from us
      nwown->Insert(el_to_repl, nwmyidx+1);
      tabMisc::DelayedFunCall_gui(el_to_repl, "BrowserExpandAll");
      tabMisc::DelayedFunCall_gui(el_to_repl, "BrowserSelectMe");
      if(el_to_repl_fun.nonempty()) {
        tabMisc::DelayedFunCall_gui(el_to_repl, el_to_repl_fun); 
      }
      el_to_repl = NULL;
      return;
    }
  }
  el_to_repl = NULL;
  taMisc::Warning("DoMoveElseLater: could not move the else / elseif statment");
}
