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

#include "ParamSeq_Group.h"
#include <ParamSeq>

TA_BASEFUNS_CTORS_DEFN(ParamSeq_Group);

void ParamSeq_Group::Initialize() {
  SetBaseType(&TA_ParamSeq);
}

void ParamSeq_Group::SetParamSeqsAtEpoch(int epoch) {
  FOREACH_ELEM_IN_GROUP(ParamSeq, ps, (*this)) {
    if(!ps->on) continue;
    ps->SetParamsAtEpoch(epoch);
  }
}
