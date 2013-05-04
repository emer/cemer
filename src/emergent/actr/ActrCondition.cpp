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

#include "ActrCondition.h"

#include <ActrBuffer>
#include <ProgVar>
#include <Layer>

void ActrCondition::Initialize() {
  cond_src = BUFFER;
  src_type = &TA_ActrBuffer;
  rel = EQUAL;
}

void ActrCondition::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  switch(cond_src) {
  case BUFFER:
    src_type = &TA_ActrBuffer;
    if(TestWarning(rel != EQUAL && rel != NOTEQUAL,
                   "UAE", "only equal or not-equal relationship allowed")) {
      rel = EQUAL;
    }
    break;
  case PROG_VAR:
    src_type = &TA_ProgVar;
    break;
  case NET_UNIT:
  case NET_LAYER:
    src_type = &TA_Layer;
    break;
  case OBJ_MEMBER:
    break;
  }
}
