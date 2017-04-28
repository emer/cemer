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
  nest_level = 0;
  a_idx = -1;
  b_idx = -1;
  mdef = NULL;
  par_rec = NULL;
  n_diffs = 0;
  widget = NULL;
}

void ObjDiffRec::Copy_(const ObjDiffRec& cp) {
  flags = cp.flags;
  nest_level = cp.nest_level;
  a_idx = cp.a_idx;
  b_idx = cp.b_idx;
  a_obj = cp.a_obj;
  b_obj = cp.b_obj;
  mdef = cp.mdef;
  par_rec = cp.par_rec;
  n_diffs = cp.n_diffs;
}

String ObjDiffRec::GetDisplayName() const {
  MemberDef* md = GetTypeDef()->members.FindName("flags");
  String rval = String(nest_level) + "_" + md->type->Get_C_EnumString(flags, false);
  if(mdef) {
    rval += "_" + mdef->name;
  }
  if(IsBnotA()) {
    rval += "_" + BName();
  }
  else if(IsAnotB()) {
    rval += "_" + AName();
  }
  else if(!mdef) {
    rval += "_" + AName();
  }
  return rval;
}

String ObjDiffRec::AName() const {
  if(mdef) {
    return mdef->name;
  }
  if(a_obj) {
    return a_obj->GetDisplayName();
  }
  return _nilString;
}

String ObjDiffRec::BName() const {
  if(mdef) {
    return mdef->name;
  }
  if(b_obj) {
    return b_obj->GetDisplayName();
  }
  return _nilString;
}

String ObjDiffRec::ADecoKey() const {
  if(a_obj)
    return a_obj->GetTypeDecoKey();
  return _nilString;
}

String ObjDiffRec::BDecoKey() const {
  if(b_obj)
    return b_obj->GetTypeDecoKey();
  return _nilString;
}

bool ObjDiffRec::NameContains(const String& nm) const {
  bool a_has = AName().contains(nm);
  bool b_has = BName().contains(nm);
  if(IsBnotA()) {
    return b_has;
  }
  else if(IsAnotB()) {
    return a_has;
  }
  return (a_has || b_has);
}
 
bool ObjDiffRec::ValueContains(const String& nm) const {
  if(IsObjects())
    return NameContains(nm);
  if(IsBnotA()) {               // should never happen...
    return b_val.contains(nm);
  }
  else if(IsAnotB()) {
    return a_val.contains(nm);
  }
  return (a_val.contains(nm) || b_val.contains(nm));
}
  
bool ObjDiffRec::AMemberNoShow() const {
  if(!mdef || !a_obj) return false;
  return !mdef->GetCondOptTest("CONDSHOW", a_obj->GetTypeDef(), a_obj.ptr());
}

bool ObjDiffRec::BMemberNoShow() const {
  if(!mdef || !b_obj) return false;
  return !mdef->GetCondOptTest("CONDSHOW", b_obj->GetTypeDef(), b_obj.ptr());
}

bool ObjDiffRec::AMemberNoEdit() const {
  if(!mdef || !a_obj) return false;
  return !mdef->GetCondOptTest("CONDEDIT", a_obj->GetTypeDef(), a_obj.ptr());
}

bool ObjDiffRec::BMemberNoEdit() const {
  if(!mdef || !b_obj) return false;
  return !mdef->GetCondOptTest("CONDEDIT", b_obj->GetTypeDef(), b_obj.ptr());
}

bool ObjDiffRec::ActionAllowed() const {
  if(IsParent()) return false;
  return IsDiff();
}

bool ObjDiffRec::GetCurAction(int a_or_b, String& lbl) const {
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

