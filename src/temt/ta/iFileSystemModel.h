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

#ifndef iFileSystemModel_h
#define iFileSystemModel_h 1

// parent includes:
#include "ta_def.h"
#include <QFileSystemModel>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API iFileSystemModel : public QFileSystemModel {
  // #NO_INSTANCE #NO_CSS custom Qt file system model -- supports sorting by size and date in data EditRole
INHERITED(QFileSystemModel)
  Q_OBJECT
public:

  iFileSystemModel(QObject *parent = 0);
  ~iFileSystemModel();

  QVariant     data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
};

#endif // iFileSystemModel_h
