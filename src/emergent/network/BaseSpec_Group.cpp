// Copyright, 1995-2013, Regents of the University of Colorado,
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

bool BaseSpec_Group::nw_itm_def_arg = false;


void BaseSpec_Group::Initialize() {
  SetBaseType(&TA_BaseSpec);
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


BaseSpec* BaseSpec_Group::FindSpecName(const char* nm) {
  BaseSpec* rval = (BaseSpec*)FindLeafName((char*)nm);
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

BaseSpec* BaseSpec_Group::FindMakeSpec(const char* nm, TypeDef* tp, bool& nw_itm, const char* alt_nm) {
  nw_itm = false;
  BaseSpec* sp = NULL;
  if(nm) {
    sp = (BaseSpec*)FindName(nm);
    if((sp == NULL) && (alt_nm)) {
      sp = (BaseSpec*)FindName(alt_nm);
      if(sp) sp->name = nm;
    }
  }
  else {
    sp = (BaseSpec*)FindType(tp);
  }
  if(sp == NULL) {
    sp = (BaseSpec*)NewEl(1, tp);
    if(nm)
      sp->name = nm;
    nw_itm = true;
  }
  else if(!sp->InheritsFrom(tp)) {
    RemoveEl(sp);
    sp = (BaseSpec*)NewEl(1, tp);
    if(nm)
      sp->name = nm;
    nw_itm = true;
  }
  return sp;
}

bool BaseSpec_Group::RemoveSpec(const char* nm, TypeDef* tp) {
  if(nm)
    return RemoveName(nm);

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
      bs->SetParam(param_path, value);
  }
}
