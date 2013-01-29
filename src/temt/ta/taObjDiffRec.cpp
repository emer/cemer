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

#include "taObjDiffRec.h"
#include <taObjDiff_List>
#include <taBase>
#include <taSmartRefT>
#include <taSmartPtrT>
#include <MemberDef>
#include <tabMisc>
#include <taRootBase>


void taObjDiffRec::Initialize() {
  flags = DF_NONE;
  owner = NULL;
  idx = -1;
  nest_level = 0;
  n_diffs = 0;
  diff_no = -1;
  diff_no_start = -1;
  diff_no_end = -1;
  hash_code = 0;
  type = NULL;
  mdef = NULL;
  addr = NULL;
  par_addr = NULL;
  par_type = NULL;
  par_odr = NULL;
  diff_odr = NULL;
  tabref = NULL;
  widget = NULL;
}

void taObjDiffRec::Copy_(const taObjDiffRec& cp) {
  flags = cp.flags;
  nest_level = cp.nest_level;
  n_diffs = cp.n_diffs;
  diff_no = cp.diff_no;
  diff_no_start = cp.diff_no_start;
  diff_no_end = cp.diff_no_end;
  name = cp.name;
  value = cp.value;
  hash_code = cp.hash_code;
  type = cp.type;
  mdef = cp.mdef;
  addr = cp.addr;
  par_addr = cp.par_addr;
  par_type = cp.par_type;
  par_odr = cp.par_odr;
  diff_odr = cp.diff_odr;
#ifndef NO_TA_BASE
  if(cp.tabref) {
    if(!tabref) tabref = new taBaseRef;
    ((taBaseRef*)tabref)->set(((taBaseRef*)cp.tabref)->ptr());
  }
  else {
    if(tabref) {
      delete (taBaseRef*)tabref;
      tabref = NULL;
    }
  }
#endif
}

taObjDiffRec::taObjDiffRec() {
  Initialize();
}

taObjDiffRec::taObjDiffRec(const taObjDiffRec& cp) {
  Initialize();
  Copy_(cp);
}

taObjDiffRec::taObjDiffRec(taObjDiff_List& odl, int nest, TypeDef* td, MemberDef* md, void* adr,
                           void* par_adr, TypeDef* par_typ, taObjDiffRec* par_od) {
  Initialize();
  nest_level = nest;
  type = td;
  mdef = md;
  addr = adr;
  par_addr = par_adr;
  par_type = par_typ;
  par_odr = par_od;
  GetValue(odl);
}

taObjDiffRec::~taObjDiffRec() {
#ifndef NO_TA_BASE
  if(tabref)
    delete (taBaseRef*)tabref;
  tabref = NULL;
#endif
}

void taObjDiffRec::Copy(const taObjDiffRec& cp) {
  Copy_(cp);
}

void taObjDiffRec::GetValue(taObjDiff_List& odl) {
  if(!type || !addr) return;            // not set!
  value = type->GetValStr(addr, par_addr, mdef, TypeDef::SC_VALUE);
  if(mdef) {
    name = mdef->name;
  }
  else {
    name = type->name;
  }
#ifndef NO_TA_BASE
  if(type->IsActualTaBase()) {
    if(!mdef && nest_level > 0)
      value = type->name + ": " + ((taBase*)addr)->GetDisplayName();
    else
      value = type->name;               // this is the relevant info at this level for diffing
  }
  else if(type->IsBasePointerType()) {
    taBase* rbase = NULL;
    if((type->IsPointer()) && type->IsTaBase()) rbase = *((taBase**)addr);
    else if(type->InheritsFrom(TA_taSmartRef)) rbase = ((taSmartRef*)addr)->ptr();
    else if(type->InheritsFrom(TA_taSmartPtr)) rbase = ((taSmartPtr*)addr)->ptr();
    if(rbase && (rbase->GetOwner() || (rbase == tabMisc::root))) {
      if(rbase->IsChildOf(odl.tab_obj_a)) {
        value = rbase->GetPathNames(NULL, odl.tab_obj_a); // scope by tab obj
        SetDiffFlag(VAL_PATH_REL);
      }
      else {
        // otherwise, always do it relative to project
        value = rbase->GetPathNames(NULL, rbase->GetOwner(&TA_taProject));
      }
    }
  }
#endif
  ComputeHashCode();
}

void taObjDiffRec::ComputeHashCode() {
  // note: level is critical -- don't want to compare at diff levels
  hash_code = taHashEl::HashCode_String(name + "&" + value) + nest_level;
}

String taObjDiffRec::GetDisplayName() {
#ifndef NO_TA_BASE
  if(!mdef && addr && type->IsActualTaBase()) {
    return name + ": " + ((taBase*)addr)->GetDisplayName();
  }
#endif
  return name;
}

bool taObjDiffRec::ActionAllowed() {
  if(HasDiffFlag(DIFF_ADDEL)) {
    if(HasDiffFlag(SUB_NO_ACT)) return false;
    if(!type->IsActualTaBase()) return false;
  }
  else if(HasDiffFlag(DIFF_PAR)) {
    return false;
  }
  return true;
}

bool taObjDiffRec::GetCurAction(int a_or_b, String& lbl) {
  bool rval = false;
  lbl = _nilString;
  if(HasDiffFlag(DIFF_DEL)) {
    if(a_or_b == 0) {           // "a" guy
      rval = HasDiffFlag(ACT_DEL_A);
      lbl = "Del A";
    } else {
      rval = HasDiffFlag(ACT_ADD_A);
      lbl = "Add A";
    }
  }
  else if(HasDiffFlag(DIFF_ADD)) {
    if(a_or_b == 0) {           // "a" guy
      rval = HasDiffFlag(ACT_ADD_B);
      lbl = "Add B";
    } else {
      rval = HasDiffFlag(ACT_DEL_B);
      lbl = "Del B";
    }
  }
  else if(HasDiffFlag(DIFF_CHG)) {
    if(a_or_b == 0) {           // "a" guy
      rval = HasDiffFlag(ACT_COPY_BA);
      lbl = "Cpy B";
    } else {
      rval = HasDiffFlag(ACT_COPY_AB);
      lbl = "Cpy A";
    }
  }
  return rval;
}

void taObjDiffRec::SetCurAction(int a_or_b, bool on_off) {
  if(HasDiffFlag(DIFF_DEL)) {
    if(a_or_b == 0) {           // "a" guy
      SetDiffFlagState(ACT_DEL_A, on_off);
    } else {
      SetDiffFlagState(ACT_ADD_A, on_off);
    }
  }
  else if(HasDiffFlag(DIFF_ADD)) {
    if(a_or_b == 0) {           // "a" guy
      SetDiffFlagState(ACT_ADD_B, on_off);
    } else {
      SetDiffFlagState(ACT_DEL_B, on_off);
    }
  }
  else if(HasDiffFlag(DIFF_CHG)) {
    if(a_or_b == 0) {           // "a" guy
      SetDiffFlagState(ACT_COPY_BA, on_off);
    } else {
      SetDiffFlagState(ACT_COPY_AB, on_off);
    }
  }
}

#ifndef NO_TA_BASE
taObjDiffRec* taObjDiffRec::GetOwnTaBaseRec() {
  if(tabref && ((taBaseRef*)tabref)->ptr())
    return this;
  taObjDiffRec* todr = par_odr;
  while(todr && (!todr->tabref || !((taBaseRef*)todr->tabref)->ptr()))
    todr = todr->par_odr;
  return todr;
}

taBase* taObjDiffRec::GetOwnTaBase() {
  taObjDiffRec* todr = GetOwnTaBaseRec();
  if(todr && todr->tabref)
    return ((taBaseRef*)todr->tabref)->ptr();
  return NULL;
}

String taObjDiffRec::GetTypeDecoKey() {
  taBase* tab = GetOwnTaBase();
  if(tab)
    return tab->GetTypeDecoKey();
  return _nilString;
}
#endif
