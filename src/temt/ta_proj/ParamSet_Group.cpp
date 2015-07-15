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

#include "ParamSet_Group.h"
#include <ParamSet>

TA_BASEFUNS_CTORS_DEFN(ParamSet_Group);

void ParamSet_Group::RestorePanels() {
  FOREACH_ELEM_IN_GROUP(ParamSet, cp, *this) {
    if(cp->GetUserDataAsBool("user_pinned")) {
      cp->EditPanel(true, true); // true,true = new tab, pinned in place
    }
  }
}

bool ParamSet_Group::LoadParamSet(const String& set_name) {
  ParamSet* set = FindName(set_name);
  if (!set)
    return false;
  
  return true;
}