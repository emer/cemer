// Copyright 2017-2018, Regents of the University of Colorado,
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

#include "ObjLibEl.h"

TA_BASEFUNS_CTORS_DEFN(ObjLibEl);
TA_BASEFUNS_CTORS_DEFN(ObjLibEl_List);

void ObjLibEl::Initialize() {
  lib_loc_no = 0;
}

void ObjLibEl::TagsToArray() {
  tags_array.Reset();
  tags_array.Split(tags, ",");
  for(int i=0; i<tags_array.size; i++) {
    tags_array[i].trim();       // nuke spaces
  }
}

void ObjLibEl_List::Initialize() {
  SetBaseType(&TA_ObjLibEl);
}

