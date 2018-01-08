// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "ObjDiffRec_List.h"
#include <ObjDiffRec>

TA_BASEFUNS_CTORS_DEFN(ObjDiffRec_List);

void ObjDiffRec_List::Initialize() {
  SetBaseType(&TA_ObjDiffRec);
}

void ObjDiffRec_List::SetAllFlag(int flg) {
  for(int i=0; i<size; i++) {
    ObjDiffRec* rec = FastEl(i);
    rec->SetDiffFlag((ObjDiffRec::DiffFlags)flg);
  }
}

