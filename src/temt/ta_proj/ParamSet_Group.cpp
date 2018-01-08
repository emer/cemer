// Copyright 2014-2017, Regents of the University of Colorado,
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

#include "ParamSet_Group.h"
#include <ParamSet>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ParamSet_Group);

bool ParamSet_Group::ActivateParamSet(const String& set_name, bool err_not_found) {
  ParamSet* set = (ParamSet*)FindLeafName(set_name);
  if (!set) {
    if(err_not_found) {
      taMisc::Error("Parameter set named:", set_name,
                    "not found in group:", name, "in ActivateParamSet");
    }
    return false;
  }
  set->CopySavedToActive(true); // apply the set and emit a message
  return true;
}

void ParamSet_Group::ActivateSaveAll() {
  FOREACH_ELEM_IN_GROUP(ParamSet, ps, *this) {
    ps->Activate();
    ps->SaveCurrent();
  }
}

void ParamSet_Group::SetSavedValue(const String& member_name, const String& saved_value,
                                   bool no_locked) {
  FOREACH_ELEM_IN_GROUP(ParamSet, ps, *this) {
    ps->SetSavedValue(member_name, saved_value, no_locked);
  }
}

void ParamSet_Group::SetMemberState(const String& member_name, int state) {
  FOREACH_ELEM_IN_GROUP(ParamSet, ps, *this) {
    ps->SetMemberState(member_name, (ControlPanelMemberData::ParamState)state);
  }
}
