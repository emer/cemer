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

#include "RelationFloat.h"

TA_BASEFUNS_CTORS_DEFN(RelationFloat);

void RelationFloat::Initialize() {
  rel = LESSTHANOREQUAL;
  val = 0.0;
  // use_var = false;
}

// bool RelationFloat::CacheVar(RelationFloat& tmp_rel) {
//   tmp_rel.rel = rel;
//   if(use_var && (bool)var) {
//     tmp_rel.val = var->GetVar().toDouble();
//     return true;
//   }
//   tmp_rel.val = val;
//   return false;
// }

bool RelationFloat::Evaluate(double cmp) const {
  // if(use_var && (bool)var) {
  //   eff_val = var->GetVar().toDouble();
  // }

  switch(rel) {
  case EQUAL:
    if((float)cmp == (float)val)        return true;
    break;
  case NOTEQUAL:
    if((float)cmp != (float)val)        return true;
    break;
  case LESSTHAN:
    if((float)cmp < (float)val) return true;
    break;
  case GREATERTHAN:
    if((float)cmp > (float)val) return true;
    break;
  case LESSTHANOREQUAL:
    if((float)cmp <= (float)val)        return true;
    break;
  case GREATERTHANOREQUAL:
    if((float)cmp >= (float)val)        return true;
    break;
  }
  return false;
}
