// Copyright 2013-2017, Regents of the University of Colorado,
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

#include <Program>
#include "ProgExpr_List.h"

TA_BASEFUNS_CTORS_DEFN(ProgExpr_List);


void ProgExpr_List::Initialize() {
  SetBaseType(&TA_ProgExpr);
  setUseStale(true);
}

void ProgExpr_List::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
}

void ProgExpr_List::UpdateProgElVars(const taBase* old_scope, taBase* new_scope) {
  for(int ei=0; ei<size; ei++) {
    ProgExpr* pe = FastEl(ei);
    pe->UpdateProgElVars(old_scope, new_scope);
  }
}
