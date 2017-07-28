// Copyright 2017, Regents of the University of Colorado,
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
#include <DataTableCols>
#include <Completions>


TA_BASEFUNS_CTORS_DEFN(ColCalcExpr);

taTypeDef_Of(DataTableCols);

Completions                 ColCalcExpr::completions;

void ColCalcExpr::Initialize() {
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
  data_cols = NULL;
  inherited::CutLinks();
}

void ColCalcExpr::Copy_(const ColCalcExpr& cp) {
  expr = cp.expr;
  UpdateAfterEdit_impl();       // gets everything
}

void ColCalcExpr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

bool ColCalcExpr::SetExpr(const String& ex) {
  expr = ex;
  UpdateAfterEdit();            // does parse
  return true;
}

String ColCalcExpr::GetName() const {
  if(owner) return owner->GetName();
  return _nilString;
}

String ColCalcExpr::GetDisplayName() const {
  return GetFullExpr();
}

String ColCalcExpr::GetFullExpr() const {
  return expr;
}

Completions* ColCalcExpr::StringFieldLookupForCompleter(const String& cur_txt, int cur_pos,
                                                        const String& mbr_name, int& new_pos) {
  completions.Reset();
  
  if (data_cols) {
    for (int i=0; i<data_cols->size; i++) {
      DataCol* col = data_cols->SafeEl(i);
      completions.object_completions.Link(col);
    }
  }
  completions.pre_cursor_text = cur_txt.through(cur_pos);
  int pos = completions.pre_cursor_text.index(' ', -1);
  if (pos == -1) {
    completions.seed = cur_txt.through(cur_pos);
    completions.pre_text = "";
}
  else {
    completions.seed = cur_txt.after(' ', -1);
    completions.pre_text = cur_txt.before(' ', -1);
  }
  
  return &completions;
}



