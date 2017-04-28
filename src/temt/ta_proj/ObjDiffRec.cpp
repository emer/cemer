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

#include "ObjDiffRec.h"
#include <FlatTreeEl>
#include <MemberDef>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ObjDiffRec);


void ObjDiffRec::Initialize() {
  flags = DF_NONE;
  diff_no = -1;
  a_idx = -1;
  b_idx = -1;
  nest_level = 0;
  a_src = NULL;
  b_src = NULL;
  par_rec = NULL;
  n_diffs = 0;
  diff_no_start = -1;
  diff_no_end = -1;
  widget = NULL;
}

void ObjDiffRec::Copy_(const ObjDiffRec& cp) {
  flags = cp.flags;
  diff_no = cp.diff_no;
  a_idx = cp.a_idx;
  b_idx = cp.b_idx;
  nest_level = cp.nest_level;
  a_src = cp.a_src;
  b_src = cp.b_src;
  par_rec = cp.par_rec;
  n_diffs = cp.n_diffs;
  diff_no_start = cp.diff_no_start;
  diff_no_end = cp.diff_no_end;
}

int ObjDiffRec::SrcNestLevel() {
  if(!a_src || !b_src)
    return 0;
  int lev = a_src->nest_level;
  if(IsBnotA()) {
    lev = b_src->nest_level;
  }
  return lev;
}

String ObjDiffRec::GetDisplayName() const {
  MemberDef* md = GetTypeDef()->members.FindName("flags");
  String rval = md->type->Get_C_EnumString(flags, false);
  if(!a_src || !b_src) {
    return rval;
  }
  String nm = a_src->name;
  if(IsBnotA()) {
    nm = b_src->name;
  }
  rval = String(nest_level) + "_" + rval + "_" + nm;
  return rval;
}

bool ObjDiffRec::ActionAllowed() {
  if(HasDiffFlag(DIFF_ADDEL)) {
    if(HasDiffFlag(SUB_NO_ACT)) return false;
    // if(!type->IsActualTaBase()) return false;
  }
  else if(HasDiffFlag(DIFF_PAR)) {
    return false;
  }
  return true;
}

bool ObjDiffRec::GetCurAction(int a_or_b, String& lbl) {
  bool rval = false;
  lbl = _nilString;
  if(HasDiffFlag(A_NOT_B)) {
    if(a_or_b == 0) {           // "a" guy
      rval = HasDiffFlag(ACT_DEL_A);
      lbl = "Del A";
    }
    else {
      rval = HasDiffFlag(ACT_ADD_A);
      lbl = "Add A";
    }
  }
  else if(HasDiffFlag(B_NOT_A)) {
    if(a_or_b == 0) {           // "a" guy
      rval = HasDiffFlag(ACT_ADD_B);
      lbl = "Add B";
    }
    else {
      rval = HasDiffFlag(ACT_DEL_B);
      lbl = "Del B";
    }
  }
  else if(HasDiffFlag(A_B_DIFF)) {
    if(a_or_b == 0) {           // "a" guy
      rval = HasDiffFlag(ACT_COPY_BA);
      lbl = "Cpy B";
    }
    else {
      rval = HasDiffFlag(ACT_COPY_AB);
      lbl = "Cpy A";
    }
  }
  return rval;
}

void ObjDiffRec::SetCurAction(int a_or_b, bool on_off) {
  if(HasDiffFlag(A_NOT_B)) {
    if(a_or_b == 0) {           // "a" guy
      SetDiffFlagState(ACT_DEL_A, on_off);
    }
    else {
      SetDiffFlagState(ACT_ADD_A, on_off);
    }
  }
  else if(HasDiffFlag(B_NOT_A)) {
    if(a_or_b == 0) {           // "a" guy
      SetDiffFlagState(ACT_ADD_B, on_off);
    }
    else {
      SetDiffFlagState(ACT_DEL_B, on_off);
    }
  }
  else if(HasDiffFlag(A_B_DIFF)) {
    if(a_or_b == 0) {           // "a" guy
      SetDiffFlagState(ACT_COPY_BA, on_off);
      if(on_off && HasDiffFlag(ACT_COPY_AB)) {
        taMisc::Info("Cannot copy both ways -- toggling A->B copy OFF");
        ClearDiffFlag(ACT_COPY_AB);
      }
    }
    else {
      SetDiffFlagState(ACT_COPY_AB, on_off);
      if(on_off && HasDiffFlag(ACT_COPY_BA)) {
        taMisc::Info("Cannot copy both ways -- toggling B->A copy OFF");
        ClearDiffFlag(ACT_COPY_BA);
      }
    }
  }
}

// ObjDiffRec* ObjDiffRec::GetOwnTaBaseRec() {
//   if(tabref && ((taBaseRef*)tabref)->ptr())
//     return this;
//   ObjDiffRec* todr = par_odr;
//   while(todr && (!todr->tabref || !((taBaseRef*)todr->tabref)->ptr()))
//     todr = todr->par_odr;
//   return todr;
// }

// taBase* ObjDiffRec::GetOwnTaBase() {
//   ObjDiffRec* todr = GetOwnTaBaseRec();
//   if(todr && todr->tabref)
//     return ((taBaseRef*)todr->tabref)->ptr();
//   return NULL;
// }

