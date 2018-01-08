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

#include "ProgExpr.h"
#include <Function>
#include <Program>

TA_BASEFUNS_CTORS_DEFN(ProgExpr);


bool ProgExpr::StdProgVarFilter(void* base_, void* var_) {
  if(!base_) return true;
  ProgExprBase* base = dynamic_cast<ProgExprBase*>(static_cast<taBase*>(base_));
  if(!base) return true;
  ProgVar* var = dynamic_cast<ProgVar*>(static_cast<taBase*>(var_));
  if(!var || !var->IsLocal()) return true; // definitely all globals
  Function* varfun = GET_OWNER(var, Function);
  if(!varfun) return true;      // not within a function, always go -- can't really tell scoping very well at this level -- could actually do it but it would be recursive and hairy
  Function* basefun = GET_OWNER(base, Function);
  if(basefun != varfun) return false; // different function scope
  return true;
}

void ProgExpr::Initialize() {
}

void ProgExpr::Destroy() {
  CutLinks();
}

void ProgExpr::CutLinks() {
  inherited::CutLinks();
}

void ProgExpr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}
