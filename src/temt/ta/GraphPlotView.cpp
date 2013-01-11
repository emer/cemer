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

#include "GraphPlotView.h"


void GraphPlotView::Initialize() {
  axis = Y;
  line_style = SOLID;
  point_style = CIRCLE;
  alt_y = false;
  eff_y_axis = NULL;
}

// void GraphPlotView::InitLinks() {
//   inherited::InitLinks();
// }

// void GraphPlotView::CutLinks() {
//   inherited::CutLinks();
// }

void GraphPlotView::CopyFromView(GraphPlotView* cp){
  CopyFromView_base(cp);        // get the base
  line_style = cp->line_style;
  point_style = cp->point_style;
  alt_y = cp->alt_y;
}

void GraphPlotView::UpdateOnFlag() {
  if(on) {
    if(!GetColPtr())
      on = false; // not actually on!
  }
}

void GraphPlotView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  // just to make srue!
  if (line_style < (LineStyle)T3GraphLine::LineStyle_MIN)
    line_style = (LineStyle)T3GraphLine::LineStyle_MIN;
  else if (line_style > (LineStyle)T3GraphLine::LineStyle_MAX)
    line_style = (LineStyle)T3GraphLine::LineStyle_MAX;

  if (point_style < (PointStyle)T3GraphLine::MarkerStyle_MIN)
    point_style = (PointStyle)T3GraphLine::MarkerStyle_MIN;
  else if (point_style > (PointStyle)T3GraphLine::MarkerStyle_MAX)
    point_style = (PointStyle)T3GraphLine::MarkerStyle_MAX;
}
