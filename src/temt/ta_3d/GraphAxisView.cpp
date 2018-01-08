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

#include "GraphAxisView.h"
#include <GraphTableView>
#include <iViewPanelOfGraphTable>
#include <T3GraphLine>
#include <GraphColView>

#include <Inventor/nodes/SoComplexity.h>

TA_BASEFUNS_CTORS_DEFN(GraphAxisView);

void GraphAxisView::Initialize() {
  labels_on = false;
  row_num = false;
  axis_label = _nilString;
}

void GraphAxisView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(matrix_cell < 0) matrix_cell = 0; // can't have a -1 
}

void GraphAxisView::CopyFromView(GraphAxisView* cp){
  CopyFromView_base(cp);        // get the base
  labels_on = cp->labels_on;
  labels_col_name = cp->labels_col_name;
  row_num = cp->row_num;
}

void GraphAxisView::UpdateOnFlag() {
  if(on) {
    if(!row_num && !GetColPtr())
      on = false; // not actually on!
  }
}

GraphColView* GraphAxisView::GetLabelsColPtr() {
  if(labels_col_name.empty()) return NULL;
  GraphTableView* gv = GET_MY_OWNER(GraphTableView);
  if(!gv) return NULL;
  return (GraphColView*)gv->children.FindName(labels_col_name);
}

DataCol* GraphAxisView::GetLabelsDAPtr() {
  if(!labels_on) return NULL;
  GraphColView* cv = GetLabelsColPtr();
  if(!cv) return NULL;
  return cv->dataCol();
}

void GraphAxisView::SetLabelsColPtr(GraphColView* cgv) {
  if(!cgv) {
    labels_col_name = _nilString;
  }
  else {
    labels_col_name = cgv->GetName();
  }
}

void GraphAxisView::ComputeRange() {
  if(!row_num) {
    inherited::ComputeRange();
    return;
  }
  // ROW_NUM
  GraphTableView* gv = GetGTV();
  SetRange_impl(gv->view_range.min, gv->view_range.max);
}

bool GraphAxisView::UpdateRange() {
  if(!row_num)
    return inherited::UpdateRange();
  // ROW_NUM
  GraphTableView* gv = GetGTV();
  return UpdateRange_impl(gv->view_range.min, gv->view_range.max);
}

void GraphAxisView::SetRange(float the_min, float the_max) {
  inherited::SetRange(the_min, the_max);
}
