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

#include "CellRange.h"
#include <QModelIndexList>


void CellRange::SetExtent(int wd, int ht) {
  col_to = col_fr + wd - 1;
  row_to = row_fr + ht - 1;
}

void CellRange::LimitExtent(int wd, int ht) {
  if (width() > wd) col_to = col_fr + wd - 1;
  if (height() > ht) row_to = row_fr + ht - 1;
}

void CellRange::LimitRange(int row_to_, int col_to_) {
  if (col_to > col_to_) col_to = col_to_;
  if (row_to > row_to_) row_to = row_to_;
}

void CellRange::SetFromModel(const QModelIndexList& indexes) {
//NOTE: this assumes that we only allow contiguous areas -- the
//  input list could have discontiguous guys, but here we will
//  just make an overal sel based on the max
  // start with first guy, then expand it out
  if (indexes.count() > 0) {
    const QModelIndex& mi = indexes.first();
    row_fr = row_to = mi.row();
    col_fr = col_to = mi.column();
    for (int i = 1; i < indexes.count(); ++i) {
      const QModelIndex& mi = indexes.at(i);
      int r = mi.row();
      int c = mi.column();
      if (r < row_fr) row_fr = r;
      else if (r > row_to) row_to = r;
      if (c < col_fr) col_fr = c;
      else if (c > col_to) col_to = c;
    }
  } else {
    Set(0, 0, -1, -1);
  }
}

