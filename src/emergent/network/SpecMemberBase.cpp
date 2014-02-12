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

#include "SpecMemberBase.h"
#include <BaseSpec>
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(SpecMemberBase);

void SpecMemberBase::Defaults() {
  Defaults_impl();
  DefaultsMembers();
  UpdateAfterEdit();
}

void SpecMemberBase::DefaultsMembers() {
  TypeDef* td = GetTypeDef();
  for(int i=TA_SpecMemberBase.members.size; i< td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_BaseSpec)) {
      ((BaseSpec*)md->GetOff((void*)this))->Defaults();
    }
    if(md->type->InheritsFrom(&TA_SpecMemberBase)) {
      ((SpecMemberBase*)md->GetOff((void*)this))->Defaults();
    }
  }
}

void SpecMemberBase::MemberUpdateAfterEdit(MemberDef* md, bool edit_dialog) {
  if(edit_dialog) return;
  if(!owner || !owner->InheritsFrom(&TA_BaseSpec)) return;
  BaseSpec* spown = (BaseSpec*)owner;
  MemberDef* mymd = spown->FindMember((void*)this);
  if(mymd)
    spown->MemberUpdateAfterEdit(mymd, edit_dialog);
}


