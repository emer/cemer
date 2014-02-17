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

#include <SelectEdit>
#include "SelectEdit_Group.h"

TA_BASEFUNS_CTORS_DEFN(SelectEdit_Group);

void SelectEdit_Group::AutoEdit() {
  FOREACH_ELEM_IN_GROUP(SelectEdit, se, *this) {
    if(se->autoEdit()) {
      se->EditPanel(true, true);        // true,true = new tab, pinned in place
    }
  }
}
