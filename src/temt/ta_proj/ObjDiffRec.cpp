// Copyright 2013-2018, Regents of the University of Colorado,
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
  chunk = 0;
  flags = DF_NONE;
  nest_level = 0;
  a_idx = -1;
  b_idx = -1;
  a_obj = NULL;
  b_obj = NULL;
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
  a_val = cp.a_val;
  b_val = cp.b_val;
  a_indep_obj = cp.a_indep_obj;
  b_indep_obj = cp.b_indep_obj;
  par_rec = cp.par_rec;
  n_diffs = cp.n_diffs;
}

String ObjDiffRec::GetDisplayName() const {
  String rval = GetMemberStrVal("flags");
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
  if(IsValues()) {
    return String(a_idx);
  }
  if(mdef) {
    return mdef->name;
  }
  if(a_obj && IsAValid()) {
    return a_obj->GetDisplayName();
  }
  return _nilString;
}

String ObjDiffRec::BName() const {
  if(IsValues()) {
    return String(b_idx);
  }
  if(mdef) {
    return mdef->name;
  }
  if(b_obj && IsBValid()) {
    return b_obj->GetDisplayName();
  }
  return _nilString;
}

String ObjDiffRec::AValue() const {
  if(IsValues() || IsMembers()) {
    return a_val;
  }
  if(a_val.nonempty()) {
    return a_val;
  }
  if(a_obj && IsAValid()) {
    return a_obj->GetTypeDef()->name;
  }
  return _nilString;
}

String ObjDiffRec::BValue() const {
  if(IsValues() || IsMembers()) {
    return b_val;
  }
  if(b_val.nonempty()) {
    return b_val;
  }
  if(b_obj && IsBValid()) {
    return b_obj->GetTypeDef()->name;
  }
  return _nilString;
}

String ObjDiffRec::ADecoKey() const {
  if(a_obj && IsAValid())
    return a_obj->GetTypeDecoKey();
  return _nilString;
}

String ObjDiffRec::BDecoKey() const {
  if(b_obj && IsBValid())
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
  if(!mdef || !a_obj || !IsAValid()) return false;
  return !mdef->GetCondOptTest("CONDSHOW", a_obj->GetTypeDef(), a_obj);
}

bool ObjDiffRec::BMemberNoShow() const {
  if(!mdef || !b_obj || !IsBValid()) return false;
  return !mdef->GetCondOptTest("CONDSHOW", b_obj->GetTypeDef(), b_obj);
}

bool ObjDiffRec::AMemberNoEdit() const {
  if(!mdef || !a_obj || !IsAValid()) return false;
  return !mdef->GetCondOptTest("CONDEDIT", a_obj->GetTypeDef(), a_obj);
}

bool ObjDiffRec::BMemberNoEdit() const {
  if(!mdef || !b_obj || !IsBValid()) return false;
  return !mdef->GetCondOptTest("CONDEDIT", b_obj->GetTypeDef(), b_obj);
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
    }
    else {
      SetDiffFlagState(ACT_COPY_AB, on_off);
    }
  }
}

bool ObjDiffRec::CompareRecObjs(const ObjDiffRec& cp) const {
  if(nest_level != cp.nest_level) return false;
  if(a_idx != cp.a_idx) return false;
  if(b_idx != cp.b_idx) return false;
  if(a_obj != cp.a_obj) return false;
  if(b_obj != cp.b_obj) return false;
  if(a_indep_obj.ptr() != cp.a_indep_obj.ptr()) return false;
  if(b_indep_obj.ptr() != cp.b_indep_obj.ptr()) return false;
  return true;
}

bool ObjDiffRec::CompareRecsAll(const ObjDiffRec& cp) const {
  if(!CompareRecObjs(cp)) return false;
  if(mdef != cp.mdef) return false;
  if(a_val != cp.a_val) return false;
  if(b_val != cp.b_val) return false;
  if(flags != cp.flags) return false;
  return true;
}
