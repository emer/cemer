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

#include "DataGenCall.h"

TA_BASEFUNS_CTORS_DEFN(DataGenCall);

taTypeDef_Of(taDataGen);

void DataGenCall::Initialize() {
  min_type = &TA_taDataGen;
  object_type = &TA_taDataGen;
}

bool DataGenCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains("::")) return false;
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm = mthobj.before("::");
  TypeDef* td = TypeDef::FindGlobalTypeName(objnm, false);
  if(!td) return false;
  if(objnm == "taDataGen") return true;
  return false;
}
