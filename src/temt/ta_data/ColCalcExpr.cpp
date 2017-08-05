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
  
  bool space = false;  // true if there is a space before the cursor and after any '.'
  bool dot = false;    // true if there is a '.' before the cursor and after any spas
  
  int space_pos;  // -1 if not found
  int dot_pos;    // -1 if not found
  
  String pre_cursor_text;
  pre_cursor_text = cur_txt.through(cur_pos);
  space_pos = pre_cursor_text.index(' ', -1);
  dot_pos = pre_cursor_text.index('.', -1);
  
  String expression = pre_cursor_text;  // assume until we check if space
  if (space_pos > dot_pos) {
    space = true;
    expression = pre_cursor_text.after(space_pos);
  }
  else if (dot_pos > space_pos) {
    dot = true;
    if (space_pos != -1) { // if there is a space before the dot
      expression = pre_cursor_text.after(space_pos);
    }
  }
  else {
    ; // nothing to do - neither space nor dot
  }
  
  if (dot) {
    String column_name = expression.before(dot_pos);
    DataCol* col = data_cols->FindName(column_name);
    if (col && col->isString()) {
      completions.string_completions.Add("contains(\"");
      completions.string_completions.Add("startsWith(\"");
      completions.string_completions.Add("endsWith(\"");
      completions.string_completions.Add("matches(\"");
      completions.string_completions.Add("matches_regexp(\"");
      completions.string_completions.Add("matches_wildcard(\"");
      
      completions.seed = expression.after(dot_pos);
      completions.pre_text = expression;
    }
  }
  else {
    String prior_expression = cur_txt.before(space_pos);
    if (prior_expression.empty()) {
      if (data_cols) {
        for (int i=0; i<data_cols->size; i++) {
          DataCol* col = data_cols->SafeEl(i);
          completions.object_completions.Link(col);
        }
      }
      completions.seed = expression;
      completions.pre_text = expression;
    }
    else {  // was the prior expression a column? if so don't list the column
      int prior_space_pos = prior_expression.index(' ', -1);
      if (prior_space_pos != -1) {
        prior_expression = prior_expression.after(prior_space_pos);
      }
      if (prior_expression.contains('.')) {  // the column name will be before first '.' in the expression
        prior_expression = prior_expression.before(prior_expression.index('.'));
      }
      // if not a column must be operator or such - ok to add columns to completion list
      DataCol* col = data_cols->FindName(prior_expression);
      if (!col) {
        if (data_cols) {
          for (int i=0; i<data_cols->size; i++) {
          col = data_cols->SafeEl(i);
            completions.object_completions.Link(col);
          }
        }
        completions.seed = expression;
        completions.pre_text = expression;
      }
    }
  }
  
  completions.pre_cursor_text = cur_txt.through(cur_pos);
  return &completions;
}




