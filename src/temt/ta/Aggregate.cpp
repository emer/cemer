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

#include "Aggregate.h"

void Aggregate::Initialize() {
  op = MEAN;
}

void Aggregate::Destroy() {
}

String Aggregate::GetAggName() const {
  return GetTypeDef()->GetEnumString("Operator", op);
}

Aggregate::ValType Aggregate::MinValType() const {
  if(op == GROUP || op == FIRST || op == LAST || op == N) return VT_STRING;
  return VT_INT;
}

Aggregate::ValType Aggregate::MinReturnType() const {
  if(op == MEAN || op == VAR || op == SS || op == STDEV || op == SEM) return VT_FLOAT;
  if(op == GROUP || op == FIRST || op == LAST) return VT_STRING;
  return VT_INT;
}

