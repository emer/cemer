// Copyright, 1995-2007, Regents of the University of Colorado,
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

#ifndef IDIALOG_H
#define IDIALOG_H

#include "taiqtso_def.h"

#include <QDialog>

class TAIQTSO_API iDialog : public QDialog
{
    Q_OBJECT

public:
  explicit iDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

  int exec();

protected:
  override void keyPressEvent(QKeyEvent *);
};

#endif // IDIALOG_H
