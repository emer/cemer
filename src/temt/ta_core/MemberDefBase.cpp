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

#include "MemberDefBase.h"
#include <MemberDef>
#include <PropertyDef>
#include <TypeDef>
#include <MemberDefBase_List>
#include <taMisc>

#ifndef NO_TA_BASE
taTypeDef_Of(taList_impl);
#include <taiMember>
#endif

void MemberDefBase::Initialize() {
  opt_flags = (MemberOpts)0;
  owner = NULL;
  type = NULL;
  is_static = false;
#ifdef TA_GUI
  im = NULL;
#endif
}

MemberDefBase::MemberDefBase()
:inherited()
{
  Initialize();
}

MemberDefBase::MemberDefBase(const String& nm)
:inherited()
{
  Initialize();
  name = nm;
}

MemberDefBase::MemberDefBase(TypeDef* ty, const String& nm, const String& dsc,
  const String& op, const String& lis, bool is_stat)
:inherited()
{
  Initialize();
  type = ty; name = nm; desc = dsc;
  taMisc::CharToStrArray(opts,op);
  taMisc::CharToStrArray(lists,lis);
  is_static = is_stat;
}

MemberDefBase::MemberDefBase(const MemberDefBase& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

void MemberDefBase::Copy(const MemberDefBase& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void MemberDefBase::Copy(const MemberDefBase* cp) {
  if (TypeInfoKind() == cp->TypeInfoKind())
  switch (TypeInfoKind()) {
  case TIK_MEMBER:
    ((MemberDef*)this)->Copy(*(const MemberDef*)cp);
    return;
  case TIK_PROPERTY:
    ((PropertyDef*)this)->Copy(*(const PropertyDef*)cp);
    return;
  default: break; // compiler food
  }
  Copy(*cp); // should never happen!
}

void MemberDefBase::Copy_(const MemberDefBase& cp) {
  type = cp.type;
  inh_opts = cp.inh_opts;
  is_static = cp.is_static;
// don't copy because delete is not ref counted
//  im = cp.im;
}

MemberDefBase::~MemberDefBase() {
#ifndef NO_TA_BASE
# ifndef NO_TA_GUI
  taRefN::SafeUnRefDone(im);
  im = NULL;
# endif
#endif
}

TypeDef* MemberDefBase::GetOwnerType() const {
  TypeDef* rval=NULL; 
  if((owner) && (owner->owner))
    rval=owner->owner;
  return rval;
}

bool MemberDefBase::CheckList(const String_PArray& lst) const {
  int i;
  for(i=0; i<lists.size; i++) {
    if(lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}

void MemberDefBase::InitOptsFlags() {

  if(HasOptFlag(OPTS_SET)) return;
  SetOptFlag(OPTS_SET);
  
  TypeDef* own_typ = GetOwnerType();
  
  if(HasOption("HIDDEN"))
    SetOptFlag(HIDDEN);
  if(HasOption("HIDDEN_INLINE"))
    SetOptFlag(HIDDEN_INLINE);
  if(HasOption("READ_ONLY"))
    SetOptFlag(READ_ONLY);
  if(HasOption("GUI_READ_ONLY"))
    SetOptFlag(GUI_READ_ONLY);
  if(HasOption("SHOW"))
    SetOptFlag(SHOW);
  if(HasOption("EXPERT"))
    SetOptFlag(EXPERT);
  if(HasOption("NO_SAVE"))
    SetOptFlag(NO_SAVE);
  if(HasOption("TREE_HIDDEN"))
    SetOptFlag(TREE_HIDDEN);
  if(HasOption("TREE_SHOW"))
    SetOptFlag(TREE_SHOW);
  if(OptionAfter("CONDSHOW_").nonempty())
    SetOptFlag(CONDSHOW);
  if(OptionAfter("CONDEDIT_").nonempty())
    SetOptFlag(CONDEDIT);
  if(OptionAfter("CONDTREE_").nonempty())
    SetOptFlag(CONDTREE);
  if(HasOption("NO_FIND"))
    SetOptFlag(NO_FIND);
  if(HasOption("NO_SEARCH"))
    SetOptFlag(NO_SEARCH);
  if(HasOption("NO_DIFF"))
    SetOptFlag(NO_DIFF);
  if(HasOption("OWN_POINTER"))
    SetOptFlag(OWN_POINTER);
  if(HasOption("NO_SET_POINTER"))
    SetOptFlag(NO_SET_POINTER);

  bool is_def_child = false;

  SetOptFlag(IS_VISIBLE);       // start off assuming everything is visible
  // expert is like SHOW
  if(!HasExpert() && !HasShow() && (HasHidden() || HasReadOnly() || HasGuiReadOnly())) {
    SetOptFlag(IS_EDITOR_HIDDEN);
    ClearOptFlag(IS_VISIBLE);   // positively marked as not-visible
  }
#ifndef NO_TA_BASE
  if(type->IsActualTaBase() && type->InheritsFrom(&TA_taList_impl)) {
    String def_child = own_typ->OptionAfter("DEF_CHILD_");
    if(def_child == name) {
      is_def_child = true;
      SetOptFlag(IS_DEF_CHILD);
      SetOptFlag(IS_VISIBLE);
      SetOptFlag(IS_TREE_HIDDEN); // we actually hide on tree b/c it will be shown automatically
      if(!HasShow() && !HasExpert()) {
        SetOptFlag(HIDDEN); // anything that will be shown in the tree is hidden by default
        SetOptFlag(IS_EDITOR_HIDDEN);
      }
    }
    else {
      // default is to show taList_impl unless specifically marked as hidden
      if(HasTreeHidden() || (IsEditorHidden() && !HasTreeShow())) {
        SetOptFlag(IS_TREE_HIDDEN);
        ClearOptFlag(IS_VISIBLE);   // positively marked as not-visible
      }
      else {
        if(!HasShow() && !HasExpert()) {
          SetOptFlag(HIDDEN); // anything that will be shown in the tree is hidden by default
          SetOptFlag(IS_EDITOR_HIDDEN);
          // note: these don't affect visibility!
       }
      }
    }
  }
  else {
    // other types -- default is to be hidden in the tree unless specifically marked otherwise -- doesn't affect overall visibility
    if(!HasTreeShow()) {
      SetOptFlag(IS_TREE_HIDDEN);
    }
    else {
      if(!HasShow() && !HasExpert()) {
        SetOptFlag(HIDDEN); // anything that will be shown in the tree is hidden by default
        SetOptFlag(IS_EDITOR_HIDDEN);
      }
    }
  }
#endif

  // signal various errors and antiquated usage
  
  if(HasOption("NO_SHOW")) {
    taMisc::DebugInfo("Member comment directive error: NO_SHOW is not supported -- use HIDDEN", GetOwnerType()->name, "::", name);
  }
  if(HasOption("SHOW_TREE")) {
    taMisc::DebugInfo("Member comment directive error: SHOW_TREE changed to TREE_SHOW",
                      GetOwnerType()->name, "::", name);
  }
  if(HasOption("HIDDEN_TREE")) {
    taMisc::DebugInfo("Member comment directive error: HIDDEN_TREE changed to TREE_HIDDEN",
                      GetOwnerType()->name, "::", name);
  }
  if(!is_def_child) {
    if(HasShow() && HasHidden()) {
      taMisc::DebugInfo("Member comment directive error: can't have SHOW and HIDDEN",
                        GetOwnerType()->name, "::", name);
    }
    if(HasExpert() && HasHidden()) {
      taMisc::DebugInfo("Member comment directive error: can't have both EXPERT and HIDDEN",
                        GetOwnerType()->name, "::", name);
    }
  }
  // note: this is OK really -- expert items NEVER show up inline, but they can show
  // up in other views and docs under the expert setting..
  // if(own_typ->IsEditInline() && HasExpert()) {
  //   taMisc::DebugInfo("Member comment directive error: can't have EXPERT on INLINE -- either make it HIDDEN or not!",
  //                     GetOwnerType()->name, "::", name);
  // }
}
