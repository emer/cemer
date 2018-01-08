// Co2018ght 2014-2017, Regents of the University of Colorado,
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

#include "ParamSet.h"

#include <taMisc>
#include <ControlPanelMember>
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(ParamSet);


void ParamSet::Initialize() {
  last_activated = false;
}

void ParamSet::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void ParamSet::Copy_(const ParamSet& cp) {
  date = cp.date;
  // note: not copying last_activated or dummy
}

void ParamSet::UpDate() {
  date = taDateTime::CurrentDateTimeStampString();
}

void ParamSet::CopyActiveToSaved(bool info_msg) {
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, sei, mbrs) {
    sei->CopyActiveToSaved();
  }
  ReShowEdit(true);
  if(info_msg) {
    taMisc::Info("Copied current active values of parameters to saved_value strings for all members in ParamSet:", name);
  }
}

void ParamSet::CopySavedToActive(bool info_msg) {
  FOREACH_ELEM_IN_GROUP(ControlPanelMember, sei, mbrs) {
    sei->CopySavedToActive();
  }
  ResetLastActivated();
  last_activated = true;
  ReShowEdit(true);
  if(info_msg) {
    taMisc::Info("Copied saved_value strings to active (live) values for all members in ParamSet:", name);
  }
}

void ParamSet::CopyActiveToSaved_item(int idx) {
  ControlPanelMember* item = mbrs.Leaf(idx);
  if(item && item->base) {
    item->CopyActiveToSaved();
    ReShowEdit(true);
  }
}

void ParamSet::CopySavedToActive_item(int idx) {
  ControlPanelMember* item = mbrs.Leaf(idx);
  if(item && item->base) {
    item->CopySavedToActive();
    ReShowEdit(true);
  }
}

void ParamSet::SetSavedValue(const String& member_name, const String& saved_value,
                             bool no_locked) {
  ControlPanelMember* emi = mbrs.FindLeafName(member_name);
  if(!emi) return;
  if(no_locked && emi->IsLocked()) return;
  emi->data.saved_value = saved_value;
  emi->SigEmitUpdated();
}

void ParamSet::SetMemberState(const String& member_name,
                              ControlPanelMemberData::ParamState state) {
  ControlPanelMember* emi = mbrs.FindLeafName(member_name);
  if(!emi) return;
  emi->data.state = state;
  emi->SigEmitUpdated();
}

bool ParamSet::ActiveEqualsSaved(const String& member_name) {
  ControlPanelMember* emi = mbrs.FindLeafName(member_name);
  if(!emi) return false;
  return emi->ActiveEqualsSaved();
}

void ParamSet::ComparePeers(ParamSet* param_set) {
  taProject* project = GetMyProj();
  project->ParamSetComparePeers(this, param_set);
}

void ParamSet::ResetLastActivated() {
  ParamSet_Group* gp = GET_MY_OWNER(ParamSet_Group);
  if(!gp) return;
  FOREACH_ELEM_IN_GROUP(ParamSet, ps, *gp) {
    ps->last_activated = false;
    ps->SigEmitUpdated();
  }
}

int ParamSet::GetSpecialState() const {
  if(last_activated) return 3;
  return inherited::GetSpecialState();
}

ControlPanelMember* ParamSet::NewDummyMember(const String& nm, const String& val) {
  ControlPanelMember* itm = (ControlPanelMember*)mbrs.NewEl(1);
  TypeDef* td = GetTypeDef();
  MemberDef* md = td->members.FindName("dummy");
  itm->base = this;
  itm->mbr = md;
  itm->SetLabel(nm, true);      // custom
  itm->SetDesc("dummy name=value record", true);
  itm->data.saved_value = val;
  itm->SetToLocked();
  return itm;
}
