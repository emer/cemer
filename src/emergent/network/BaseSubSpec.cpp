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

#include "BaseSubSpec.h"
#include <BaseSpec>
#include <MemberDef>

#include <taMisc>

void BaseSubSpec::Initialize() {
}

void BaseSubSpec::Copy_(const BaseSubSpec& cp) {
  unique = cp.unique;
}

void BaseSubSpec::Destroy() {
}

void BaseSubSpec::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(unique, this);
  if(!taMisc::is_loading) {
    StructUpdate(true);
    UpdateSpec();
    StructUpdate(false);
  }
}

void BaseSubSpec::UpdateAfterEdit_impl() {
  taNBase::UpdateAfterEdit_impl();
  UpdateSpec();
}

BaseSpec* BaseSubSpec::FindParentBaseSpec() {
  return GET_MY_OWNER(BaseSpec);
}

BaseSubSpec* BaseSubSpec::FindParent() {
  BaseSpec* bso = FindParentBaseSpec();
  if(bso == NULL)       return NULL;
  BaseSpec* bsoo = bso->FindParent(); // parent's owner
  if(bsoo == NULL)      return NULL;

  String my_path = GetPath(NULL, bso); // get my path to owner..
  MemberDef* md;
  BaseSubSpec* from = (BaseSubSpec*)bsoo->FindFromPath(my_path, md);
  if(!from || !from->InheritsFrom(TA_BaseSubSpec))
    return NULL;                        // corresponding subspec object not found..
  return from;
}

void BaseSubSpec::SetUnique(const char* memb_nm, bool on) {
  MemberDef* md = FindMember(memb_nm);
  if(md)
    SetUnique(md->idx, on);
  else {
    TestError(GetTypeDef()->members.size > 0, "SetUnique",
              "Member named:", memb_nm, "not found");
  }
}

void BaseSubSpec::SetUnique(int memb_no, bool on) {
  if(memb_no < TA_BaseSubSpec.members.size)
    return;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(TestError(!md, "SetUnique",
               "Member number:", String(memb_no), "not found")) {
    return;
  }
  if(on)
    unique.AddUnique(md->name);
  else
    unique.RemoveEl(md->name);
}

bool BaseSubSpec::GetUnique(const char* memb_nm) {
  MemberDef* md = FindMember(memb_nm);
  if(md)
    return GetUnique(md->idx);
  TestError(true, "GetUnique", "Member named:", memb_nm, "not found");
  return false;
}

bool BaseSubSpec::GetUnique(int memb_no) {
  if(memb_no < TA_BaseSubSpec.members.size)
    return false;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(TestError(!md, "GetUnique", "Member number:", String(memb_no), "not found")) {
    return false;
  }
  if(unique.FindEl(md->name) >= 0) return true;
  return false;
}

void BaseSubSpec::UpdateSpec() {
  BaseSubSpec* parent = FindParent();
  if(parent) {
    TypeDef* td = GetTypeDef();
    int i;
    for(i=TA_BaseSubSpec.members.size; i< td->members.size; i++)
      UpdateMember(parent, i);
  }
}

void BaseSubSpec::UpdateMember(BaseSubSpec* from, int memb_no) {
  if((from == NULL) || (memb_no < TA_BaseSubSpec.members.size))
    return;
  TypeDef* td = GetTypeDef();
  TypeDef* frm_td = from->GetTypeDef();
  if(memb_no < frm_td->members.size) {  // parent must have this member
    MemberDef* md = td->members[memb_no];
    if(frm_td->members[memb_no] == md) {        // must be the same member
      // don't copy read only or hidden members! (usually set automatically
      // and might depend on local variables)
      if(!GetUnique(memb_no) &&
         !(md->HasOption("READ_ONLY") || md->HasOption("HIDDEN") ||
           md->HasOption("NO_INHERIT")))
      {
        MemberCopyFrom(memb_no, from);
        // NO NO NO 1000 times no (literally!) -- this causes MASSIVE update chains
//      SigEmitUpdated();
      }
    }
  }
}
