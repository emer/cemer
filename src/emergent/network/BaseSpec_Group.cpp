// Copyright 2013-2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "BaseSpec_Group.h"
#include <BaseSpec>

TA_BASEFUNS_CTORS_DEFN(BaseSpec_Group);
SMARTREF_OF_CPP(BaseSpec_Group)


bool BaseSpec_Group::nw_itm_def_arg = false;


void BaseSpec_Group::Initialize() {
  SetBaseType(&TA_BaseSpec);
  setUseStale(true);
}

String BaseSpec_Group::GetTypeDecoKey() const {
  if(size > 0) return FastEl(0)->GetTypeDecoKey();
  if(GetOwner()) return GetOwner()->GetTypeDecoKey();
  return inherited::GetTypeDecoKey();
}

BaseSpec* BaseSpec_Group::FindSpecType(TypeDef* td) {
  // breadth-first search
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(!bs->isDestroying() && (bs->GetTypeDef() == td))
      return bs;    // use equals to find type of spec object
  }
  // then check the children
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    // this is not true -- bad!!
//     if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef())))
//       continue;                      // no hope..
    BaseSpec* rval = bs->children.FindSpecType(td);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecInherits(TypeDef* td, taBase* for_obj) {
  // breadth-first search
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(bs->InheritsFrom(td)) {
      if((for_obj == NULL) || (bs->CheckObjectType_impl(for_obj)))
        return bs;    // object must also be sufficient..
    }
  }
  // then check the children
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef())))
      continue;                 // no hope..
    BaseSpec* rval = bs->children.FindSpecInherits(td, for_obj);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecTypeNotMe(TypeDef* td, BaseSpec* not_me) {
  // breadth-first search
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if((bs->GetTypeDef() == td) && (bs != not_me))
      return bs;    // use equals to find type of spec object
  }
  // then check the children
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef()) || (bs != not_me)))
      continue;                 // no hope..
    BaseSpec* rval = bs->children.FindSpecTypeNotMe(td, not_me);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecInheritsNotMe(TypeDef* td, BaseSpec* not_me, taBase* for_obj) {
  // breadth-first search
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(bs->InheritsFrom(td) && (bs != not_me)) {
      if((for_obj == NULL) || (bs->CheckObjectType_impl(for_obj)))
        return bs;    // object must also be sufficient..
    }
  }
  // then check the children
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef()) || (bs != not_me)))
      continue;                 // no hope..
    BaseSpec* rval = bs->children.FindSpecInheritsNotMe(td, not_me, for_obj);
    if(rval)
      return rval;
  }
  return NULL;
}


BaseSpec* BaseSpec_Group::FindSpecName(const String& nm) {
  BaseSpec* rval = (BaseSpec*)FindLeafName(nm);
  if(rval)
    return rval;
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    rval = bs->children.FindSpecName(nm);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindParent() {
  return GET_MY_OWNER(BaseSpec);
}

BaseSpec* BaseSpec_Group::FindMakeSpec(const String& nm, TypeDef* tp, bool& nw_itm, const String& alt_nm) {
  nw_itm = false;
  BaseSpec* sp = NULL;
  if(nm.nonempty()) {
    sp = (BaseSpec*)FindName(nm);
    if((sp == NULL) && alt_nm.nonempty()) {
      sp = (BaseSpec*)FindName(alt_nm);
      if(sp) {
        sp->SetName(nm);
      }
    }
  }
  else {
    sp = (BaseSpec*)FindType(tp);
  }
  if(sp == NULL) {
    sp = (BaseSpec*)NewEl(1, tp);
    if(nm.nonempty()) {
      sp->name = nm;
    }
    nw_itm = true;
  }
  else if(!sp->InheritsFrom(tp)) {
    RemoveEl(sp);
    sp = (BaseSpec*)NewEl(1, tp);
    if(nm.nonempty()) {
      sp->name = nm;
    }
    nw_itm = true;
  }
  return sp;
}

bool BaseSpec_Group::RemoveSpec(const String& nm, TypeDef* tp) {
  if(nm.nonempty()) {
    return RemoveName(nm);
  }

  int idx = FindTypeIdx(tp);
  if(idx >= 0)
    return RemoveIdx(idx);
  return false;
}

void BaseSpec_Group::Defaults() {
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    bs->Defaults();
    bs->children.Defaults();
  }
}

void BaseSpec_Group::SetParam(TypeDef* spec_type, const String& param_path,
                              const String& value) {
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(bs->InheritsFrom(spec_type))
      bs->SetMember(param_path, value);
  }
}

void BaseSpec_Group::UpdateAllSpecs() {
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    bs->UpdateAfterEdit();
  }

  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    bs->children.UpdateAllSpecs();
  }
}

void BaseSpec_Group::ResetAllSpecIdxs() {
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    bs->ResetAllSpecIdxs();
  }
}

void BaseSpec_Group::RestorePanels() {
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(bs->GetUserDataAsBool("user_pinned")) {
      bs->EditPanel(true, true); // true,true = new tab, pinned in place
    }
  }
  // then check the children
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    bs->children.RestorePanels();
  }
}

void* BaseSpec_Group::El_CopyN_(void* to_, void* fm) {
  taBase* to = (taBase*)to_;
  to->StructUpdate(true);
  void* rval = El_Copy_(to_, fm);
//  El_SetName_(to_, El_GetName_(fm));  // don't do for specs - keep the "_copy" added in the copy step
  to->StructUpdate(false);
  return rval;
}


