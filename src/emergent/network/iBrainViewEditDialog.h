// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef iBrainViewEditDialog_h
#define iBrainViewEditDialog_h 1

// parent includes:
#include "network_def.h"
#include <iDialogRegexp>

// member includes:

// declare all other types mentioned but not required to include:
class QModelIndex; //


class EMERGENT_API iBrainViewEditDialog : public iDialogRegexp {
  Q_OBJECT
  INHERITED(iDialogRegexp)
public:
  iBrainViewEditDialog(
    taiWidgetFieldRegexp* regexp_field,
    const String& field_name,
    iDialogRegexpPopulator *re_populator,
    const void *fieldOwner,
    bool read_only,
    bool editor_mode = true
  );

protected:
  virtual void          AddButtons();
  virtual void          SetColors();

protected slots:
  void                  itemClicked(const QModelIndex & index);
  override void         btnApply_clicked();
  override void         btnRandomColors_clicked();
  override void         btnColorsFromScale_clicked();
};

#endif // iBrainViewEditDialog_h
