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

#include "GraphAxisView.h"


void GraphAxisView::Initialize() {
  row_num = false;
}

void GraphAxisView::CopyFromView(GraphAxisView* cp){
  CopyFromView_base(cp);        // get the base
  row_num = cp->row_num;
}

void GraphAxisView::UpdateOnFlag() {
  if(on) {
    if(!row_num && !GetColPtr())
      on = false; // not actually on!
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

