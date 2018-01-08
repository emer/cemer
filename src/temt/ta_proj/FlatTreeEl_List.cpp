// Co2018ght 2017-2017, Regents of the University of Colorado,
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

#include "FlatTreeEl_List.h"
#include <FlatTreeEl>
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(FlatTreeEl_List);

void FlatTreeEl_List::Initialize() {
  SetBaseType(&TA_FlatTreeEl);
}

void FlatTreeEl_List::FlatTreeOf(taBase* obj) {
  Reset();
  top_obj = obj;
  obj->GetFlatTree(*this, 0, NULL, NULL, NULL);
}

FlatTreeEl* FlatTreeEl_List::NewMember
(int nest, MemberDef* md, const taBase* ta_obj, FlatTreeEl* par_el) {
  FlatTreeEl* fel = (FlatTreeEl*)New(1);
  fel->nest_level = nest;
  fel->type = md->type;
  fel->mdef = md;
  fel->addr = md->GetOff(ta_obj);
  fel->obj.set((taBase*)ta_obj);
  fel->parent_el = par_el;
  fel->size = fel->type->size;        // default -- can be overridden if needed
  
  fel->name = md->name;              // name is member name
  return fel;
}

FlatTreeEl* FlatTreeEl_List::NewObject(int nest, const taBase* ta_obj, FlatTreeEl* par_el) {
  FlatTreeEl* fel = (FlatTreeEl*)New(1);
  fel->nest_level = nest;
  fel->type = ta_obj->GetTypeDef();
  fel->mdef = NULL;             // make sure!
  fel->addr = (void*)ta_obj;
  fel->obj.set((taBase*)ta_obj);
  fel->parent_el = par_el;
  fel->size = fel->type->size;        // default -- can be overridden if needed

  fel->name = fel->type->name;       // name is type name by default -- value will be name of obj
  return fel;
}

FlatTreeEl* FlatTreeEl_List::GetFlatTreeMember(int nest, MemberDef* md, FlatTreeEl* obj_fel, const taBase* obj) {
  TypeDef* td = md->type;
  if(td->IsActualTaBase()) {
    taBase* mb_obj = (taBase*)md->GetOff(obj);
    return mb_obj->GetFlatTree(*this, nest, obj_fel, obj, md); // defer to guy
  }

  FlatTreeEl* fel = NewMember(nest, md, obj_fel->obj, obj_fel);
  
  if(td->IsBasePointerType()) {
    taBase* rbase = NULL;
    if((td->IsPointer()) && td->IsTaBase()) rbase = *((taBase**)fel->addr);
    else if(InheritsFrom(TA_taSmartRef)) rbase = ((taSmartRef*)fel->addr)->ptr();
    else if(InheritsFrom(TA_taSmartPtr)) rbase = ((taSmartPtr*)fel->addr)->ptr();
    if(rbase) {
      rbase->GetFlatTreeValue(*this, fel, true); // pointer
    }
  }
  else {
    fel->value = md->GetValStr(obj, TypeDef::SC_VALUE, false); // no force inline
  }
  return fel;
}

void FlatTreeEl_List::GetFlatTreeMembers(FlatTreeEl* obj_fel, const taBase* obj) {
  int new_lev = obj_fel->nest_level + 1;
  
  TypeDef* td = obj_fel->type;
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if(md->HasNoSave() || md->IsInvisible())
      continue;
    if(obj_fel->mdef) { // object is a member
      if(md->HasHiddenInline()) continue;
    }
    if(md->name == "user_data_") {
      continue;                 // too much clutter for now..
    }

    GetFlatTreeMember(new_lev, md, obj_fel, obj);
  }
}

void FlatTreeEl_List::GetFlatTreeMembers_ListsOnly(FlatTreeEl* obj_fel, const taBase* obj) {
  int new_lev = obj_fel->nest_level + 1;
  
  TypeDef* td = obj_fel->type;
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if(!md->type->InheritsFrom(&TA_taList_impl)) // key diff -- only lists!
      continue;
    if(md->HasNoSave() || md->IsInvisible())
      continue;
    if(obj_fel->mdef) { // object is a member
      if(md->HasHiddenInline()) continue;
    }
    if(md->name == "user_data_") {
      continue;                 // too much clutter for now..
    }

    GetFlatTreeMember(new_lev, md, obj_fel, obj);
  }
}

void FlatTreeEl_List::HashToIntArray(int_PArray& array) {
  array.Reset();
  array.Alloc(size);            // hold enough for us!
  for(int i=0; i<size; i++) {
    FlatTreeEl* fel = FastEl(i);
    array.Add(fel->ComputeHashCode());
  }
}

