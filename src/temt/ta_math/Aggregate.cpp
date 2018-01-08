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

#include "Aggregate.h"
#include <Program>

TA_BASEFUNS_CTORS_DEFN(Aggregate);

void Aggregate::Initialize() {
  op = MEAN;
}

void Aggregate::Destroy() {
}

String Aggregate::GetAggName() const {
  return GetTypeDef()->GetEnumString("Operator", op);
}

Aggregate::ValType Aggregate::MinValType() const {
  if(op == GROUP || op == FIRST || op == LAST || op == N) {
    return VT_STRING;
  }
  else if ((op == COUNT || op == PERCENT || op == FIND_FIRST || op == FIND_LAST) && (rel.rel == Relation::CONTAINS || rel.rel == Relation::NOT_CONTAINS) ) {
    return VT_STRING;
  }
  return VT_INT;
}

Aggregate::ValType Aggregate::MinReturnType() const {
  if(op == MEAN || op == VAR || op == SS || op == STDEV || op == SEM || op == PERCENT) return VT_FLOAT;
  if(op == GROUP || op == FIRST || op == LAST || op == FIND_FIRST || op == FIND_LAST) return VT_STRING;
  return VT_INT;
}

