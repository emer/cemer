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

#ifndef iDialogColorChooser_h
#define iDialogColorChooser_h 1

// parent includes:
#include "ta_def.h"
#include <QColorDialog>

class TA_API iDialogColorChooser : public QColorDialog
{
  Q_OBJECT

public:
  explicit iDialogColorChooser(const QColor &old_clr, QWidget *parent = 0);

protected:
  void keyPressEvent(QKeyEvent *key_event) override;
};

#endif // iDialogColorChooser_h
