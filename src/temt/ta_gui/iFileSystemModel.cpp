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

#include "iFileSystemModel.h"
#include <QDateTime>

iFileSystemModel::iFileSystemModel(QObject *parent) : inherited(parent) {
}

iFileSystemModel::~iFileSystemModel() {
}

QVariant iFileSystemModel::data(const QModelIndex& index, int role) const {
  if(role != Qt::EditRole)
    return inherited::data(index, role);

  int idx = index.row() - 1;    // +1 for ..
  int col = index.column();

  if(col == 1) {
    return size(index);
  }
  else if(col == 3) {
    return lastModified(index).toTime_t();
  }
  return inherited::data(index, role);
}
