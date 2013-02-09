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

#include "ColCalcExpr.h"
#include <DataCol>

taTypeDef_Of(DataTableCols);

void ColCalcExpr::Initialize() {
  col_lookup = NULL;
  data_cols = NULL;
}

void ColCalcExpr::Destroy() {
  CutLinks();
}

void ColCalcExpr::InitLinks() {
  inherited::InitLinks();
  data_cols = GET_MY_OWNER(DataTableCols);
}

void ColCalcExpr::CutLinks() {
  if(col_lookup) {
    taBase::SetPointer((taBase**)&col_lookup, NULL);
  }
  data_cols = NULL;
  inherited::CutLinks();
}

void ColCalcExpr::Copy_(const ColCalcExpr& cp) {
  if(col_lookup) {
    taBase::SetPointer((taBase**)&col_lookup, NULL);
  }
  expr = cp.expr;
  UpdateAfterEdit_impl();       // gets everything
}

void ColCalcExpr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
//   Program* prg = GET_MY_OWNER(Program);
//   if(!prg || isDestroying()) return;
  if(col_lookup) {
    if(expr.empty())
      expr += col_lookup->name;
    else
      expr += " " + col_lookup->name;
    taBase::SetPointer((taBase**)&col_lookup, NULL);
  }
}

bool ColCalcExpr::SetExpr(const String& ex) {
  taBase::SetPointer((taBase**)&col_lookup, NULL); // justin case
  expr = ex;
  UpdateAfterEdit();            // does parse
  return true;
}

String ColCalcExpr::GetName() const {
  if(owner) return owner->GetName();
  return _nilString;
}

String ColCalcExpr::GetFullExpr() const {
  return expr;
}


