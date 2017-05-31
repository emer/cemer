// Copyright 2014, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "nvhDataView.h"
#include <T3Color>

TA_BASEFUNS_CTORS_DEFN(nvhDataView);

void nvhDataView::Copy_(const nvhDataView& cp) {
  m_hcolor = cp.m_hcolor;
}

void nvhDataView::setHighlightColor(const T3Color& color) {
  m_hcolor = color;
  DoHighlightColor(true);
}


