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

#include "SimpleMathSpec.h"
#include <taMath>

#include <math.h>

TA_BASEFUNS_CTORS_DEFN(SimpleMathSpec);


void SimpleMathSpec::Initialize() {
  opr = NONE;
  arg = 0.0;
  lw = -1.0;
  hi = 1.0;
}

double SimpleMathSpec::Evaluate(double val) const {
  switch(opr) {
  case NONE:
    return val;
  case THRESH:
    return (val >= arg) ? hi : lw;
  case ABS:
    return fabsf(val);
  case SQUARE:
    return val * val;
  case SQRT:
    return sqrtf(val);
  case LOG:
    return logf(val);
  case LOG10:
    return ((val <= 0) ? taMath::flt_min_10_exp : log10(val));
  case EXP:
    return expf(val);
  case ADD:
    return val + arg;
  case SUB:
    return val - arg;
  case MUL:
    return val * arg;
  case DIV:
    return val / arg;
  case POWER:
    return powf(val, arg);
  case MAX:
    return MAX(val, arg);
  case MIN:
    return MIN(val, arg);
  case MINMAX:
    val = MIN(val, hi);
    return MAX(val, lw);
  case REPLACE:
    return (val == arg) ? lw : val;
  }
  return val;
}

Variant& SimpleMathSpec::EvaluateVar(Variant& val) const {
  // we just detect the NONE case to avoid conversions, otherwise we just double it!
  if (opr == NONE) return val;
  val = Evaluate(val.toDouble());
  return val;
}
